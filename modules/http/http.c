/**
 * @file http.c
 * @brief HTTP 客户端实现
 *
 * 基于 net_socket 实现的 HTTP/1.1 客户端，支持同步和异步请求，文件下载等功能。
 */

#include "http.h"
#include "net.h"
#include "platform.h"
#include "url.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*===========================================================
 * 内部数据结构
 *===========================================================*/

#define HTTP_MAX_HEADER_SIZE 8192
#define HTTP_MAX_BODY_SIZE   1024 * 1024

struct http_client {
    net_socket_t* sock;           /* socket 句柄 */
    int timeout_ms;               /* 超时时间 */
    int max_redirects;            /* 最大重定向次数 */
    http_response_callback_t callback; /* 异步回调 */
    void* user_data;              /* 用户数据 */
    
    /* 接收缓冲区 */
    char* recv_buf;
    size_t recv_len;
    size_t recv_capacity;
    
    /* 请求状态 */
    int redirect_count;           /* 已重定向次数 */
    int content_length;           /* 期望内容长度 */
    int chunked;                  /* 是否分块传输 */
    
    /* 下载相关 */
    int fd;                       /* 下载文件句柄 */
    size_t downloaded;            /* 已下载字节数 */
    size_t total_size;            /* 总大小 */
    http_download_callback_t download_cb;
};

/*===========================================================
 * 内部函数声明
 *===========================================================*/

static const char* http_method_to_string(http_method_t method);
static int http_build_request(const http_request_t* req, char* buf, size_t buf_len);
static int http_parse_response(struct http_client* client, http_response_t* response);
static int http_parse_status_line(const char* line, int* status_code);
static int http_parse_header(const char* header, const char* key, char* value, size_t value_len);
static int http_connect(struct http_client* client, const char* host, uint16_t port);
static void http_recv_data(struct http_client* client);
static void http_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data);

/*===========================================================
 * HTTP 方法转字符串
 *===========================================================*/

static const char* http_method_to_string(http_method_t method) {
    switch (method) {
        case HTTP_METHOD_GET:     return "GET";
        case HTTP_METHOD_POST:    return "POST";
        case HTTP_METHOD_PUT:     return "PUT";
        case HTTP_METHOD_DELETE:  return "DELETE";
        case HTTP_METHOD_HEAD:    return "HEAD";
        case HTTP_METHOD_OPTIONS: return "OPTIONS";
        default:                  return "GET";
    }
}

/*===========================================================
 * HTTP 请求构建
 *===========================================================*/

static int http_build_request(const http_request_t* req, char* buf, size_t buf_len) {
    const char* method = http_method_to_string(req->method);
    const char* path = req->path ? req->path : "/";
    
    size_t len = snprintf(buf, buf_len, "%s %s", method, path);
    
    if (req->query) {
        len += snprintf(buf + len, buf_len - len, "?%s", req->query);
    }
    
    len += snprintf(buf + len, buf_len - len, " HTTP/1.1\r\n");
    len += snprintf(buf + len, buf_len - len, "Host: %s:%d\r\n", req->host, req->port);
    
    if (req->content_type) {
        len += snprintf(buf + len, buf_len - len, "Content-Type: %s\r\n", req->content_type);
    }
    
    if (req->body && req->body_len > 0) {
        len += snprintf(buf + len, buf_len - len, "Content-Length: %zu\r\n", req->body_len);
    }
    
    if (req->headers) {
        len += snprintf(buf + len, buf_len - len, "%s\r\n", req->headers);
    }
    
    len += snprintf(buf + len, buf_len - len, "Connection: close\r\n");
    len += snprintf(buf + len, buf_len - len, "\r\n");
    
    if (req->body && req->body_len > 0) {
        if (len + req->body_len < buf_len) {
            memcpy(buf + len, req->body, req->body_len);
            len += req->body_len;
        } else {
            return -1;
        }
    }
    
    return len;
}

/*===========================================================
 * HTTP 响应解析
 *===========================================================*/

static int http_parse_status_line(const char* line, int* status_code) {
    const char* p = line;
    while (*p && !isdigit((unsigned char)*p)) p++;
    if (!*p) return -1;
    
    *status_code = atoi(p);
    return 0;
}

static int http_parse_header(const char* header, const char* key, char* value, size_t value_len) {
    const char* p = strstr(header, key);
    if (!p) return -1;
    
    p += strlen(key);
    while (*p == ':' || *p == ' ') p++;
    
    const char* end = strstr(p, "\r\n");
    if (!end) end = p + strlen(p);
    
    size_t len = end - p;
    if (len >= value_len) len = value_len - 1;
    
    memcpy(value, p, len);
    value[len] = '\0';
    
    return 0;
}

static int http_parse_response(struct http_client* client, http_response_t* response) {
    if (!client || !client->recv_buf || client->recv_len < 4) {
        return -1;
    }
    
    memset(response, 0, sizeof(http_response_t));
    
    /* 查找响应头结束标记 */
    char* header_end = strstr(client->recv_buf, "\r\n\r\n");
    if (!header_end) {
        return -1;
    }
    
    size_t header_len = header_end - client->recv_buf;
    
    /* 解析状态行 */
    char* status_end = strstr(client->recv_buf, "\r\n");
    if (status_end) {
        *status_end = '\0';
        http_parse_status_line(client->recv_buf, &response->status_code);
        *status_end = '\r';
    }
    
    /* 提取响应头 */
    response->header = (char*)iot_malloc(header_len + 1);
    if (!response->header) return -1;
    memcpy(response->header, client->recv_buf, header_len);
    response->header[header_len] = '\0';
    response->header_len = header_len;
    
    /* 获取 Content-Length */
    char content_len_str[32];
    if (http_parse_header(response->header, "Content-Length", content_len_str, sizeof(content_len_str)) == 0) {
        client->content_length = atoi(content_len_str);
        client->total_size = client->content_length;
    } else {
        client->content_length = -1;
    }
    
    /* 检查是否分块传输 */
    char transfer_encoding[64];
    if (http_parse_header(response->header, "Transfer-Encoding", transfer_encoding, sizeof(transfer_encoding)) == 0) {
        client->chunked = (strstr(transfer_encoding, "chunked") != NULL);
    } else {
        client->chunked = 0;
    }
    
    /* 提取响应体 */
    size_t body_start = header_len + 4;
    size_t body_available = client->recv_len - body_start;
    
    if (!client->chunked && client->content_length > 0) {
        /* 普通传输 */
        if (body_available >= (size_t)client->content_length) {
            response->body = (char*)iot_malloc(client->content_length + 1);
            if (response->body) {
                memcpy(response->body, client->recv_buf + body_start, client->content_length);
                response->body[client->content_length] = '\0';
                response->body_len = client->content_length;
            }
            return 0;
        }
        return -1; /* 数据不完整 */
    } else if (!client->chunked && client->content_length < 0) {
        /* 无 Content-Length，读取全部剩余数据 */
        if (body_available > 0) {
            response->body = (char*)iot_malloc(body_available + 1);
            if (response->body) {
                memcpy(response->body, client->recv_buf + body_start, body_available);
                response->body[body_available] = '\0';
                response->body_len = body_available;
            }
            return 0;
        }
        return 0;
    }
    
    return -1;
}

/*===========================================================
 * Socket 连接
 *===========================================================*/

static int http_connect(struct http_client* client, const char* host, uint16_t port) {
    char ip[32];
    int ret = net_gethostbyname(host, ip);
    if (ret != 0) {
        return -1;
    }
    
    if (client->sock) {
        net_socket_close((sock_t)client->sock);
    }
    
    client->sock = (net_socket_t*)net_socket_create(SOCK_TYPE_STREAM, http_socket_callback, client);
    if (!client->sock) {
        return -1;
    }
    
    ret = net_socket_connect((sock_t)client->sock, ip, port);
    if (ret < 0) {
        return -1;
    }
    
    return 0;
}

/*===========================================================
 * 数据接收
 *===========================================================*/

static void http_recv_data(struct http_client* client) {
    if (!client || !client->sock) return;
    
    char buf[4096];
    int len;
    
    while ((len = net_socket_recv((sock_t)client->sock, buf, sizeof(buf))) > 0) {
        /* 扩展缓冲区 */
        if (client->recv_len + len > client->recv_capacity) {
            size_t new_cap = client->recv_capacity + HTTP_MAX_HEADER_SIZE;
            char* new_buf = (char*)iot_realloc(client->recv_buf, new_cap);
            if (!new_buf) {
                return;
            }
            client->recv_buf = new_buf;
            client->recv_capacity = new_cap;
        }
        
        memcpy(client->recv_buf + client->recv_len, buf, len);
        client->recv_len += len;
        
        /* 如果是下载模式，写入文件 */
        if (client->fd >= 0) {
            iot_fs_write(client->fd, buf, len);
            client->downloaded += len;
            
            if (client->download_cb) {
                client->download_cb(client, client->downloaded, client->total_size, client->user_data);
            }
        }
    }
}

/*===========================================================
 * Socket 事件回调
 *===========================================================*/

static void http_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    struct http_client* client = (struct http_client*)user_data;
    if (!client) return;
    
    switch (event) {
        case NET_EVENT_CONNECTED: {
            /* 连接成功，发送请求 */
            break;
        }
        case NET_EVENT_RECV: {
            http_recv_data(client);
            break;
        }
        case NET_EVENT_DISCONNECTED: {
            /* 连接断开 */
            break;
        }
        case NET_EVENT_ERROR: {
            break;
        }
        default:
            break;
    }
}

/*===========================================================
 * HTTP 客户端 API 实现
 *===========================================================*/

http_client_t* http_client_create(void) {
    struct http_client* client = (struct http_client*)iot_malloc(sizeof(struct http_client));
    if (!client) return NULL;
    
    memset(client, 0, sizeof(struct http_client));
    client->timeout_ms = 30000;
    client->max_redirects = 3;
    client->fd = -1;
    client->content_length = -1;
    
    return client;
}

void http_client_destroy(http_client_t* client) {
    if (!client) return;
    
    if (client->sock) {
        net_socket_close((sock_t)client->sock);
    }
    
    if (client->recv_buf) {
        iot_free(client->recv_buf);
    }
    
    if (client->fd >= 0) {
        iot_fs_close(client->fd);
    }
    
    iot_free(client);
}

int http_client_request(http_client_t* client, const http_request_t* request, http_response_t* response) {
    if (!client || !request || !response) {
        return -1;
    }
    
    /* 解析 URL */
    char host[64], path[512];
    uint16_t port = request->port ? request->port : 80;
    
    if (request->url) {
        if (http_parse_url(request->url, host, sizeof(host), &port, path, sizeof(path)) != 0) {
            return -1;
        }
    } else {
        if (request->host) {
            strncpy(host, request->host, sizeof(host) - 1);
            host[sizeof(host) - 1] = '\0';
        } else {
            return -1;
        }
        if (request->path) {
            strncpy(path, request->path, sizeof(path) - 1);
            path[sizeof(path) - 1] = '\0';
        } else {
            strcpy(path, "/");
        }
    }
    
    /* 构建请求 */
    http_request_t req = *request;
    req.host = host;
    req.port = port;
    req.path = path;
    
    char req_buf[4096];
    int req_len = http_build_request(&req, req_buf, sizeof(req_buf));
    if (req_len < 0) {
        return -1;
    }
    
    /* 连接服务器 */
    if (http_connect(client, host, port) != 0) {
        return -1;
    }
    
    /* 发送请求 */
    net_socket_send((sock_t)client->sock, req_buf, req_len);
    
    /* 等待响应 */
    int wait_ms = 0;
    while (wait_ms < client->timeout_ms) {
        http_recv_data(client);
        
        /* 检查是否收到完整响应 */
        char* header_end = strstr(client->recv_buf, "\r\n\r\n");
        if (header_end) {
            /* 解析响应头 */
            char header_copy[HTTP_MAX_HEADER_SIZE];
            size_t header_len = header_end - client->recv_buf;
            if (header_len >= HTTP_MAX_HEADER_SIZE) header_len = HTTP_MAX_HEADER_SIZE - 1;
            memcpy(header_copy, client->recv_buf, header_len);
            header_copy[header_len] = '\0';
            
            /* 解析状态码 */
            http_parse_status_line(client->recv_buf, &response->status_code);
            
            /* 检查重定向 */
            if ((response->status_code == 301 || response->status_code == 302) && 
                client->redirect_count < client->max_redirects) {
                char location[512];
                if (http_parse_header(header_copy, "Location", location, sizeof(location)) == 0) {
                    client->redirect_count++;
                    
                    /* 释放当前响应 */
                    if (client->recv_buf) {
                        iot_free(client->recv_buf);
                        client->recv_buf = NULL;
                        client->recv_len = 0;
                        client->recv_capacity = 0;
                    }
                    
                    /* 重新请求 */
                    http_request_t redirect_req = *request;
                    redirect_req.url = location;
                    return http_client_request(client, &redirect_req, response);
                }
            }
            
            /* 解析完整响应 */
            if (http_parse_response(client, response) == 0) {
                net_socket_close((sock_t)client->sock);
                client->sock = NULL;
                return 0;
            }
        }
        
        iot_task_sleep(10);
        wait_ms += 10;
    }
    
    return -1;
}

int http_client_get(http_client_t* client, const char* url, http_response_t* response) {
    http_request_t request = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 30000,
    };
    return http_client_request(client, &request, response);
}

int http_client_post(http_client_t* client, const char* url, const char* body, size_t body_len,
                     const char* content_type, http_response_t* response) {
    http_request_t request = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .body = body,
        .body_len = body_len,
        .content_type = content_type ? content_type : "application/x-www-form-urlencoded",
        .timeout_ms = 30000,
    };
    return http_client_request(client, &request, response);
}

int http_client_download(http_client_t* client, const char* url, const char* save_path,
                         http_download_callback_t progress_cb, void* user_data) {
    if (!client || !url || !save_path) {
        return -1;
    }
    
    /* 解析 URL */
    char host[64], path[512];
    uint16_t port = 80;
    
    if (http_parse_url(url, host, sizeof(host), &port, path, sizeof(path)) != 0) {
        return -1;
    }
    
    /* 构建请求 */
    http_request_t request = {
        .host = host,
        .port = port,
        .path = path,
        .method = HTTP_METHOD_GET,
        .timeout_ms = client->timeout_ms,
    };
    
    char req_buf[1024];
    int req_len = http_build_request(&request, req_buf, sizeof(req_buf));
    if (req_len < 0) {
        return -1;
    }
    
    /* 创建文件 */
    client->fd = iot_fs_open(save_path, IOT_FS_O_WRONLY | IOT_FS_O_CREAT | IOT_FS_O_TRUNC, 0644);
    if (client->fd < 0) {
        return -1;
    }
    
    client->download_cb = progress_cb;
    client->user_data = user_data;
    client->downloaded = 0;
    client->total_size = 0;
    
    /* 连接并发送请求 */
    if (http_connect(client, host, port) != 0) {
        iot_fs_close(client->fd);
        client->fd = -1;
        return -1;
    }
    
    net_socket_send((sock_t)client->sock, req_buf, req_len);
    
    /* 等待下载完成 */
    int wait_ms = 0;
    while (wait_ms < client->timeout_ms) {
        http_recv_data(client);
        
        /* 检查连接是否断开 */
        if (net_socket_get_state((sock_t)client->sock) == NET_SOCK_STATE_CLOSED) {
            break;
        }
        
        iot_task_sleep(10);
        wait_ms += 10;
    }
    
    iot_fs_close(client->fd);
    client->fd = -1;
    
    net_socket_close((sock_t)client->sock);
    client->sock = NULL;
    
    return 0;
}

void http_client_set_timeout(http_client_t* client, int timeout_ms) {
    if (client) {
        client->timeout_ms = timeout_ms;
    }
}

void http_client_set_max_redirects(http_client_t* client, int max_redirects) {
    if (client) {
        client->max_redirects = max_redirects;
    }
}

void http_response_free(http_response_t* response) {
    if (!response) return;
    
    if (response->header) {
        iot_free(response->header);
        response->header = NULL;
    }
    
    if (response->body) {
        iot_free(response->body);
        response->body = NULL;
    }
    
    if (response->error) {
        iot_free(response->error);
        response->error = NULL;
    }
}

/*===========================================================
 * 便捷接口实现
 *===========================================================*/

int http_get(const char* url, http_response_t* response) {
    http_client_t* client = http_client_create();
    if (!client) return -1;
    
    int ret = http_client_get(client, url, response);
    http_client_destroy(client);
    
    return ret;
}

int http_post(const char* url, const char* body, size_t body_len, const char* content_type, http_response_t* response) {
    http_client_t* client = http_client_create();
    if (!client) return -1;
    
    int ret = http_client_post(client, url, body, body_len, content_type, response);
    http_client_destroy(client);
    
    return ret;
}

int http_download(const char* url, const char* save_path) {
    http_client_t* client = http_client_create();
    if (!client) return -1;
    
    int ret = http_client_download(client, url, save_path, NULL, NULL);
    http_client_destroy(client);
    
    return ret;
}

/*===========================================================
 * URL 解析实现
 *===========================================================*/

int http_parse_url(const char* url, char* host, size_t host_len, uint16_t* port, char* path, size_t path_len) {
    if (!url || !host || !port || !path) {
        return -1;
    }
    
    const char* p = url;
    
    /* 跳过 scheme */
    if (strncmp(p, "http://", 7) == 0) {
        p += 7;
        *port = 80;
    } else if (strncmp(p, "https://", 8) == 0) {
        p += 8;
        *port = 443;
    } else {
        *port = 80;
    }
    
    /* 提取 host */
    const char* host_end = p;
    while (*host_end && *host_end != ':' && *host_end != '/' && *host_end != '?') {
        host_end++;
    }
    
    size_t host_length = host_end - p;
    if (host_length >= host_len) host_length = host_len - 1;
    memcpy(host, p, host_length);
    host[host_length] = '\0';
    
    /* 提取 port */
    if (*host_end == ':') {
        p = host_end + 1;
        *port = (uint16_t)atoi(p);
        while (*p && *p != '/' && *p != '?') p++;
    } else {
        p = host_end;
    }
    
    /* 提取 path */
    if (*p == '/') {
        const char* path_end = p;
        while (*path_end && *path_end != '?') path_end++;
        size_t path_length = path_end - p;
        if (path_length >= path_len) path_length = path_len - 1;
        memcpy(path, p, path_length);
        path[path_length] = '\0';
    } else {
        strcpy(path, "/");
    }
    
    return 0;
}
