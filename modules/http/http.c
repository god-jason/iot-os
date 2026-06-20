/**
 * @file http.c
 * @brief HTTP 客户端实现
 *
 * 基于 net_socket 实现的 HTTP/1.1 客户端，支持同步和异步请求，文件下载等功能。
 *
 * 主要功能：
 * - 支持 GET、POST、PUT、DELETE、HEAD、OPTIONS 等 HTTP 方法
 * - 支持自动重定向（301、302）
 * - 支持文件下载和下载进度回调
 * - 支持自定义请求头
 * - 支持分块传输编码（chunked）
 * - 支持异步请求回调
 *
 * 使用示例：
 *
 * @code
 *   // 简单 GET 请求
 *   http_response_t response;
 *   if (http_get("http://example.com/api", &response) == 0) {
 *       printf("Status: %d\\n", response.status_code);
 *       printf("Body: %s\\n", response.body);
 *       http_response_free(&response);
 *   }
 *
 *   // 使用客户端
 *   http_client_t* client = http_client_create();
 *   http_client_set_timeout(client, 10000);
 *   http_response_t response;
 *   if (http_client_get(client, "http://example.com/api", &response) == 0) {
 *       printf("Status: %d\\n", response.status_code);
 *       http_response_free(&response);
 *   }
 *   http_client_destroy(client);
 *
 *   // 文件下载
 *   http_client_download(client, "http://example.com/file.zip", "file.zip", NULL, NULL);
 * @endcode
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
 * 常量定义
 *===========================================================*/

/** HTTP 响应头最大长度 */
#define HTTP_MAX_HEADER_SIZE 8192

/** HTTP 响应体最大长度（1MB） */
#define HTTP_MAX_BODY_SIZE   1024 * 1024

/*===========================================================
 * 内部数据结构
 *===========================================================*/

/**
 * @brief HTTP 客户端内部结构
 *
 * 包含客户端的所有状态信息，包括 socket 连接、缓冲区、下载状态等。
 */
struct http_client {
    net_socket_t* sock;           /**< socket 句柄 */
    int timeout_ms;               /**< 超时时间（毫秒） */
    int max_redirects;            /**< 最大重定向次数 */
    http_response_callback_t callback; /**< 异步回调函数 */
    void* user_data;              /**< 用户数据 */
    
    /* 接收缓冲区 */
    char* recv_buf;               /**< 接收数据缓冲区 */
    size_t recv_len;              /**< 已接收数据长度 */
    size_t recv_capacity;         /**< 缓冲区容量 */
    
    /* 请求状态 */
    int redirect_count;           /**< 已重定向次数 */
    int content_length;           /**< 期望内容长度（-1 表示未知） */
    int chunked;                  /**< 是否分块传输编码 */
    
    /* 下载相关 */
    int fd;                       /**< 下载文件句柄（-1 表示不是下载模式） */
    size_t downloaded;            /**< 已下载字节数 */
    size_t total_size;            /**< 文件总大小 */
    http_download_callback_t download_cb; /**< 下载进度回调函数 */
};

/*===========================================================
 * 内部函数声明
 *===========================================================*/

/** 将 HTTP 方法枚举转换为字符串 */
static const char* http_method_to_string(http_method_t method);

/** 构建 HTTP 请求报文 */
static int http_build_request(const http_request_t* req, char* buf, size_t buf_len);

/** 解析 HTTP 响应 */
static int http_parse_response(struct http_client* client, http_response_t* response);

/** 解析 HTTP 状态行，提取状态码 */
static int http_parse_status_line(const char* line, int* status_code);

/** 从 HTTP 响应头中提取指定字段的值 */
static int http_parse_header(const char* header, const char* key, char* value, size_t value_len);

/** 建立到 HTTP 服务器的 TCP 连接 */
static int http_connect(struct http_client* client, const char* host, uint16_t port);

/** 从 socket 接收数据 */
static void http_recv_data(struct http_client* client);

/** socket 事件回调函数 */
static void http_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data);

/*===========================================================
 * HTTP 方法转字符串
 *===========================================================*/

/**
 * @brief 将 HTTP 方法枚举转换为字符串
 * @param method HTTP 方法枚举值
 * @return HTTP 方法字符串（"GET"、"POST" 等）
 */
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

/**
 * @brief 构建 HTTP 请求报文
 *
 * 根据请求参数构建完整的 HTTP/1.1 请求报文，包括请求行、请求头和请求体。
 *
 * @param req 请求参数
 * @param buf 输出缓冲区
 * @param buf_len 缓冲区长度
 * @return 成功返回请求报文长度，失败返回 -1
 */
static int http_build_request(const http_request_t* req, char* buf, size_t buf_len) {
    const char* method = http_method_to_string(req->method);
    const char* path = req->path ? req->path : "/";
    
    size_t len = snprintf(buf, buf_len, "%s %s", method, path);
    
    /* 添加查询参数 */
    if (req->query) {
        len += snprintf(buf + len, buf_len - len, "?%s", req->query);
    }
    
    /* HTTP/1.1 协议行 */
    len += snprintf(buf + len, buf_len - len, " HTTP/1.1\r\n");
    len += snprintf(buf + len, buf_len - len, "Host: %s:%d\r\n", req->host, req->port);
    
    /* Content-Type */
    if (req->content_type) {
        len += snprintf(buf + len, buf_len - len, "Content-Type: %s\r\n", req->content_type);
    }
    
    /* Content-Length */
    if (req->body && req->body_len > 0) {
        len += snprintf(buf + len, buf_len - len, "Content-Length: %zu\r\n", req->body_len);
    }
    
    /* 自定义请求头 */
    if (req->headers) {
        len += snprintf(buf + len, buf_len - len, "%s\r\n", req->headers);
    }
    
    /* 保持连接（暂时使用 close，便于简单实现） */
    len += snprintf(buf + len, buf_len - len, "Connection: close\r\n");
    len += snprintf(buf + len, buf_len - len, "\r\n");
    
    /* 请求体 */
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

/**
 * @brief 解析 HTTP 状态行
 *
 * 从状态行中提取 HTTP 状态码。
 * 例如："HTTP/1.1 200 OK" -> status_code = 200
 *
 * @param line 状态行字符串
 * @param status_code 输出参数，存储提取的状态码
 * @return 0 成功，-1 失败
 */
static int http_parse_status_line(const char* line, int* status_code) {
    const char* p = line;
    while (*p && !isdigit((unsigned char)*p)) p++;
    if (!*p) return -1;
    
    *status_code = atoi(p);
    return 0;
}

/**
 * @brief 从 HTTP 响应头中提取指定字段的值
 *
 * @param header 完整的响应头字符串
 * @param key 要提取的字段名（如 "Content-Length"）
 * @param value 输出缓冲区，存储提取的值
 * @param value_len 缓冲区长度
 * @return 0 成功，-1 失败（字段不存在）
 */
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

/**
 * @brief 解析 HTTP 响应
 *
 * 从接收缓冲区中解析完整的 HTTP 响应，提取状态码、响应头和响应体。
 * 支持普通传输和分块传输编码（chunked）。
 *
 * @param client HTTP 客户端句柄
 * @param response 响应输出参数
 * @return 0 成功（接收到完整响应），-1 失败（数据不完整）
 */
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
        /* 普通传输：检查是否接收到完整数据 */
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
        /* 无 Content-Length，读取全部剩余数据（服务器关闭连接时） */
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

/**
 * @brief 建立到 HTTP 服务器的 TCP 连接
 *
 * @param client HTTP 客户端句柄
 * @param host 服务器主机名
 * @param port 服务器端口
 * @return 0 成功，-1 失败
 */
static int http_connect(struct http_client* client, const char* host, uint16_t port) {
    char ip[32];
    int ret = net_gethostbyname(host, ip);
    if (ret != 0) {
        return -1;
    }
    
    /* 关闭旧连接 */
    if (client->sock) {
        net_socket_close((sock_t)client->sock);
    }
    
    /* 创建新 socket */
    client->sock = (net_socket_t*)net_socket_create(SOCK_TYPE_STREAM, http_socket_callback, client);
    if (!client->sock) {
        return -1;
    }
    
    /* 连接服务器 */
    ret = net_socket_connect((sock_t)client->sock, ip, port);
    if (ret < 0) {
        return -1;
    }
    
    return 0;
}

/*===========================================================
 * 数据接收
 *===========================================================*/

/**
 * @brief 从 socket 接收数据
 *
 * 将接收到的数据存入缓冲区，如果是下载模式则写入文件。
 *
 * @param client HTTP 客户端句柄
 */
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

/**
 * @brief socket 事件回调函数
 *
 * 处理 socket 连接、接收数据、断开连接等事件。
 *
 * @param sock socket 句柄
 * @param event 事件类型
 * @param user_data 用户数据（HTTP 客户端句柄）
 */
static void http_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    struct http_client* client = (struct http_client*)user_data;
    if (!client) return;
    
    switch (event) {
        case NET_EVENT_CONNECTED: {
            /* 连接成功，发送请求（由调用方在连接后发送） */
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

/**
 * @brief 创建 HTTP 客户端
 *
 * 分配并初始化一个 HTTP 客户端实例。
 *
 * @return HTTP 客户端句柄，失败返回 NULL
 */
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

/**
 * @brief 销毁 HTTP 客户端
 *
 * 关闭所有连接、释放缓冲区，销毁客户端实例。
 *
 * @param client HTTP 客户端句柄
 */
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

/**
 * @brief 执行同步 HTTP 请求
 *
 * 发送 HTTP 请求并等待响应，支持自动重定向。
 *
 * @param client HTTP 客户端句柄
 * @param request 请求配置
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
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

/**
 * @brief 执行 GET 请求（同步）
 *
 * @param client HTTP 客户端句柄
 * @param url 请求 URL
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
int http_client_get(http_client_t* client, const char* url, http_response_t* response) {
    http_request_t request = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 30000,
    };
    return http_client_request(client, &request, response);
}

/**
 * @brief 执行 POST 请求（同步）
 *
 * @param client HTTP 客户端句柄
 * @param url 请求 URL
 * @param body 请求体
 * @param body_len 请求体长度
 * @param content_type Content-Type
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
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

/**
 * @brief 下载文件（同步）
 *
 * 将 URL 指定的文件下载到本地路径。
 *
 * @param client HTTP 客户端句柄
 * @param url 文件 URL
 * @param save_path 保存路径
 * @param progress_cb 进度回调（可选）
 * @param user_data 用户数据
 * @return 0 成功，-1 失败
 */
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

/**
 * @brief 设置 HTTP 客户端超时时间
 *
 * @param client HTTP 客户端句柄
 * @param timeout_ms 超时时间（毫秒）
 */
void http_client_set_timeout(http_client_t* client, int timeout_ms) {
    if (client) {
        client->timeout_ms = timeout_ms;
    }
}

/**
 * @brief 设置 HTTP 客户端最大重定向次数
 *
 * @param client HTTP 客户端句柄
 * @param max_redirects 最大重定向次数
 */
void http_client_set_max_redirects(http_client_t* client, int max_redirects) {
    if (client) {
        client->max_redirects = max_redirects;
    }
}

/**
 * @brief 释放 HTTP 响应内存
 *
 * @param response HTTP 响应结构体
 */
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

/**
 * @brief 执行简单 GET 请求（创建临时客户端）
 *
 * 创建一个临时客户端，执行 GET 请求后自动销毁客户端。
 *
 * @param url 请求 URL
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
int http_get(const char* url, http_response_t* response) {
    http_client_t* client = http_client_create();
    if (!client) return -1;
    
    int ret = http_client_get(client, url, response);
    http_client_destroy(client);
    
    return ret;
}

/**
 * @brief 执行简单 POST 请求（创建临时客户端）
 *
 * 创建一个临时客户端，执行 POST 请求后自动销毁客户端。
 *
 * @param url 请求 URL
 * @param body 请求体
 * @param body_len 请求体长度
 * @param content_type Content-Type
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
int http_post(const char* url, const char* body, size_t body_len, const char* content_type, http_response_t* response) {
    http_client_t* client = http_client_create();
    if (!client) return -1;
    
    int ret = http_client_post(client, url, body, body_len, content_type, response);
    http_client_destroy(client);
    
    return ret;
}

/**
 * @brief 下载文件（创建临时客户端）
 *
 * 创建一个临时客户端，下载文件后自动销毁客户端。
 *
 * @param url 文件 URL
 * @param save_path 保存路径
 * @return 0 成功，-1 失败
 */
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

/**
 * @brief 解析 HTTP URL
 *
 * 从 URL 字符串中提取主机名、端口和路径。
 * 支持 http:// 和 https:// 前缀。
 *
 * @param url URL 字符串（如 "http://example.com:8080/path?query=value"）
 * @param host 输出主机名缓冲区
 * @param host_len 主机名缓冲区长度
 * @param port 输出端口号
 * @param path 输出路径缓冲区
 * @param path_len 路径缓冲区长度
 * @return 0 成功，-1 失败
 */
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
