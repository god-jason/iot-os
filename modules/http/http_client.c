/**
 * @file http_client.c
 * @brief HTTP 客户端实现
 *
 * 基于 net_socket 实现的 HTTP/1.1 客户端，支持 GET、POST、PUT、DELETE 等方法，
 * 支持文件下载、自定义头、超时配置、gzip 压缩等功能。
 */

#include "http_client.h"
#include "http_gzip.h"
#include "http_url.h"
#include "net.h"
#include "iot.h"
#include "iot_list.h"
#include "iot_log.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define HTTP_MAX_HEADER_SIZE 8192
#define HTTP_MAX_BODY_SIZE   1024 * 1024

static const char* http_method_to_string(http_method_t method);
static int http_build_request(http_client_t* client, char* buf, size_t buf_len);
static int http_parse_response(http_client_t* client, http_response_t* response);
static int http_parse_status_line(const char* line, int* status_code);
static int http_parse_header(const char* header, const char* key, char* value, size_t value_len);
static int http_connect(http_client_t* client);
static void http_recv_data(http_client_t* client);
static void http_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data);
static int http_client_do_request(http_client_t* client);

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

static int http_build_request(http_client_t* client, char* buf, size_t buf_len) {
    const char* method = http_method_to_string(client->options.method);
    const char* path = client->path;
    
    size_t len = snprintf(buf, buf_len, "%s %s HTTP/1.1\r\n", method, path);
    len += snprintf(buf + len, buf_len - len, "Host: %s\r\n", client->host);
    
    if (client->options.content_type) {
        len += snprintf(buf + len, buf_len - len, "Content-Type: %s\r\n", client->options.content_type);
    }
    
    if (client->options.body && client->options.body_len > 0) {
        len += snprintf(buf + len, buf_len - len, "Content-Length: %zu\r\n", client->options.body_len);
    }
    
    if (client->options.headers) {
        len += snprintf(buf + len, buf_len - len, "%s\r\n", client->options.headers);
    }
    
    if (client->options.enable_gzip) {
        len += snprintf(buf + len, buf_len - len, "Accept-Encoding: gzip, deflate\r\n");
    }
    
    len += snprintf(buf + len, buf_len - len, "Connection: close\r\n");
    len += snprintf(buf + len, buf_len - len, "\r\n");
    
    if (client->options.body && client->options.body_len > 0) {
        if (len + client->options.body_len < buf_len) {
            memcpy(buf + len, client->options.body, client->options.body_len);
            len += client->options.body_len;
        } else {
            return -1;
        }
    }
    
    return len;
}

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

static int http_parse_response(http_client_t* client, http_response_t* response) {
    if (!client || !client->recv_buf || client->recv_len < 4) {
        return -1;
    }
    
    memset(response, 0, sizeof(http_response_t));
    
    char* header_end = strstr(client->recv_buf, "\r\n\r\n");
    if (!header_end) {
        return -1;
    }
    
    size_t header_len = header_end - client->recv_buf;
    
    char* status_end = strstr(client->recv_buf, "\r\n");
    if (status_end) {
        *status_end = '\0';
        http_parse_status_line(client->recv_buf, &response->status_code);
        *status_end = '\r';
    }
    
    response->header = (char*)iot_malloc(header_len + 1);
    if (!response->header) return -1;
    memcpy(response->header, client->recv_buf, header_len);
    response->header[header_len] = '\0';
    response->header_len = header_len;
    
    char content_len_str[32];
    if (http_parse_header(response->header, "Content-Length", content_len_str, sizeof(content_len_str)) == 0) {
        client->content_length = atoi(content_len_str);
        client->total_size = client->content_length;
    } else {
        client->content_length = -1;
    }
    
    char transfer_encoding[64];
    if (http_parse_header(response->header, "Transfer-Encoding", transfer_encoding, sizeof(transfer_encoding)) == 0) {
        client->chunked = (strstr(transfer_encoding, "chunked") != NULL);
    } else {
        client->chunked = 0;
    }
    
    client->response_gzip = http_gzip_check_response(response->header);
    
    size_t body_start = header_len + 4;
    size_t body_available = client->recv_len - body_start;
    
    if (body_available == 0) {
        return 0;
    }
    
    if (client->response_gzip && client->options.auto_decompress && !client->options.download_path) {
        uint8_t* decompressed = http_gzip_decompress_alloc((const uint8_t*)(client->recv_buf + body_start),
                                                           body_available, &response->body_len);
        if (decompressed) {
            response->body = (char*)decompressed;
            return 0;
        }
    }
    
    if (!client->chunked && client->content_length > 0) {
        if (body_available >= (size_t)client->content_length) {
            response->body = (char*)iot_malloc(client->content_length + 1);
            if (response->body) {
                memcpy(response->body, client->recv_buf + body_start, client->content_length);
                response->body[client->content_length] = '\0';
                response->body_len = client->content_length;
            }
            return 0;
        }
        return -1;
    } else if (!client->chunked && client->content_length < 0) {
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

static int http_connect(http_client_t* client) {
    char ip[32];
    int ret = iot_dns_resolve(client->host, ip, sizeof(ip));
    if (ret < 0) {
        return -1;
    }
    
    if (client->sock) {
        net_socket_close((sock_t)client->sock);
    }
    
    client->sock = (net_socket_t*)net_socket_create(SOCK_TYPE_STREAM, NULL, http_socket_callback, client);
    if (!client->sock) {
        return -1;
    }
    
    ret = net_socket_connect((sock_t)client->sock, ip, client->port);
    if (ret < 0) {
        return -1;
    }
    
    return 0;
}

static void http_recv_data(http_client_t* client) {
    if (!client || !client->sock) return;
    
    char buf[4096];
    int len;
    
    while ((len = net_socket_recv((sock_t)client->sock, buf, sizeof(buf))) > 0) {
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
        
        if (client->fd) {
            iot_fs_write(client->fd, buf, len);
            client->downloaded += len;
            
            if (client->options.download_cb) {
                client->options.download_cb(client, client->downloaded, client->total_size, client->options.user_data);
            }
        }
    }
}

static void http_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    http_client_t* client = (http_client_t*)user_data;
    if (!client) return;
    
    switch (event) {
        case NET_EVENT_CONNECTED: {
            break;
        }
        case NET_EVENT_RECV: {
            http_recv_data(client);
            break;
        }
        case NET_EVENT_DISCONNECTED: {
            break;
        }
        case NET_EVENT_ERROR: {
            break;
        }
        default:
            break;
    }
}

http_client_t* http_client_create(const http_client_options_t* options) {
    http_client_t* client = (http_client_t*)iot_malloc(sizeof(http_client_t));
    if (!client) {
        LOG("[HTTP] Client create failed: out of memory");
        return NULL;
    }
    
    memset(client, 0, sizeof(http_client_t));
    
    if (options) {
        client->options = *options;
    }
    
    if (client->options.timeout_ms <= 0) {
        client->options.timeout_ms = 30000;
    }
    if (client->options.max_redirects <= 0) {
        client->options.max_redirects = 3;
    }
    if (client->options.gzip_level <= 0) {
        client->options.gzip_level = 6;
    }
    
    client->fd = NULL;
    client->content_length = -1;
    
    client->mutex = iot_mutex_create();
    client->sem = iot_sem_create(1, 0);
    
    client->gzip_ctx = http_gzip_create();
    
    list_init(&client->list_node);
    
    LOG("[HTTP] Client created");
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
    
    if (client->fd) {
        iot_fs_close(client->fd);
    }
    
    http_response_free(&client->response);
    
    if (client->gzip_ctx) {
        http_gzip_destroy(client->gzip_ctx);
    }
    
    if (client->mutex) {
        iot_mutex_delete(client->mutex);
    }
    
    if (client->sem) {
        iot_sem_delete(client->sem);
    }
    
    iot_free(client);
}

void http_client_set_options(http_client_t* client, const http_client_options_t* options) {
    if (!client || !options) return;
    
    iot_mutex_lock(client->mutex, -1);
    client->options = *options;
    iot_mutex_unlock(client->mutex);
}

static int http_client_do_request(http_client_t* client) {
    http_url_t url;
    if (http_url_parse(client->options.url, &url) != 0) {
        return -1;
    }
    
    strncpy(client->host, url.host, sizeof(client->host) - 1);
    client->host[sizeof(client->host) - 1] = '\0';
    client->port = url.port;
    strncpy(client->path, url.path, sizeof(client->path) - 1);
    client->path[sizeof(client->path) - 1] = '\0';
    
    if (url.query[0]) {
        size_t path_len = strlen(client->path);
        size_t query_len = strlen(url.query);
        if (path_len + 1 + query_len < sizeof(client->path)) {
            client->path[path_len] = '?';
            memcpy(client->path + path_len + 1, url.query, query_len);
            client->path[path_len + 1 + query_len] = '\0';
        }
    }
    
    if (client->options.download_path) {
        client->fd = iot_fs_open(client->options.download_path, IOT_FS_WB);
        if (!client->fd) {
            return -1;
        }
        client->downloaded = 0;
        client->total_size = 0;
    }
    
    char req_buf[4096];
    int req_len = http_build_request(client, req_buf, sizeof(req_buf));
    if (req_len < 0) {
        if (client->fd) {
            iot_fs_close(client->fd);
            client->fd = NULL;
        }
        return -1;
    }
    
    if (http_connect(client) != 0) {
        if (client->fd) {
            iot_fs_close(client->fd);
            client->fd = NULL;
        }
        return -1;
    }
    
    net_socket_send((sock_t)client->sock, req_buf, req_len);
    
    int wait_ms = 0;
    while (wait_ms < client->options.timeout_ms) {
        http_recv_data(client);
        
        if (!client->options.download_path) {
            char* header_end = strstr(client->recv_buf, "\r\n\r\n");
            if (header_end) {
                char header_copy[HTTP_MAX_HEADER_SIZE];
                size_t header_len = header_end - client->recv_buf;
                if (header_len >= HTTP_MAX_HEADER_SIZE) header_len = HTTP_MAX_HEADER_SIZE - 1;
                memcpy(header_copy, client->recv_buf, header_len);
                header_copy[header_len] = '\0';
                
                int status_code;
                http_parse_status_line(client->recv_buf, &status_code);
                
                if ((status_code == 301 || status_code == 302) && 
                    client->redirect_count < client->options.max_redirects) {
                    char location[512];
                    if (http_parse_header(header_copy, "Location", location, sizeof(location)) == 0) {
                        client->redirect_count++;
                        
                        if (client->recv_buf) {
                            iot_free(client->recv_buf);
                            client->recv_buf = NULL;
                            client->recv_len = 0;
                            client->recv_capacity = 0;
                        }
                        
                        if (client->fd) {
                            iot_fs_close(client->fd);
                            client->fd = NULL;
                        }
                        
                        http_client_options_t new_options = client->options;
                        new_options.url = location;
                        http_client_set_options(client, &new_options);
                        
                        return http_client_do_request(client);
                    }
                }
                
                if (http_parse_response(client, &client->response) == 0) {
                    client->request_done = 1;
                    client->request_failed = 0;
                    break;
                }
            }
        }
        
        if (net_socket_get_state((sock_t)client->sock) == NET_SOCK_STATE_CLOSED) {
            if (client->options.download_path) {
                client->request_done = 1;
                client->request_failed = 0;
            } else if (client->recv_len > 0) {
                http_parse_response(client, &client->response);
                client->request_done = 1;
                client->request_failed = 0;
            }
            break;
        }
        
        iot_task_delay(10);
        wait_ms += 10;
    }
    
    if (client->fd) {
        iot_fs_close(client->fd);
        client->fd = NULL;
    }
    
    if (client->sock) {
        net_socket_close((sock_t)client->sock);
        client->sock = NULL;
    }
    
    if (!client->request_done) {
        client->request_done = 1;
        client->request_failed = 1;
    }
    
    return client->request_failed ? -1 : 0;
}

int http_client_execute(http_client_t* client) {
    if (!client) return -1;
    
    iot_mutex_lock(client->mutex, -1);
    http_response_free(&client->response);
    client->request_done = 0;
    client->request_failed = 0;
    client->redirect_count = 0;
    client->recv_len = 0;
    iot_mutex_unlock(client->mutex);
    
    int ret = http_client_do_request(client);
    
    if (client->options.response_cb) {
        client->options.response_cb(client, &client->response, client->options.user_data);
    }
    
    return ret;
}

http_response_t* http_client_get_response(http_client_t* client) {
    if (!client) return NULL;
    
    iot_mutex_lock(client->mutex, -1);
    if (!client->request_done) {
        iot_mutex_unlock(client->mutex);
        return NULL;
    }
    iot_mutex_unlock(client->mutex);
    
    return &client->response;
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
    
    response->header_len = 0;
    response->body_len = 0;
    response->status_code = 0;
}

int http_get(const char* url, http_response_t* response) {
    http_client_options_t options = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 30000,
        .enable_gzip = true,
        .auto_decompress = true,
    };
    
    http_client_t* client = http_client_create(&options);
    if (!client) return -1;
    
    int ret = http_client_execute(client);
    
    if (ret == 0) {
        http_response_t* resp = http_client_get_response(client);
        if (resp) {
            *response = *resp;
        }
    }
    
    http_client_destroy(client);
    
    return ret;
}

int http_post(const char* url, const char* body, size_t body_len, 
              const char* content_type, http_response_t* response) {
    http_client_options_t options = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .body = body,
        .body_len = body_len,
        .content_type = content_type ? content_type : "application/x-www-form-urlencoded",
        .timeout_ms = 30000,
        .enable_gzip = true,
        .auto_decompress = true,
    };
    
    http_client_t* client = http_client_create(&options);
    if (!client) return -1;
    
    int ret = http_client_execute(client);
    
    if (ret == 0) {
        http_response_t* resp = http_client_get_response(client);
        if (resp) {
            *response = *resp;
        }
    }
    
    http_client_destroy(client);
    
    return ret;
}

int http_put(const char* url, const char* body, size_t body_len,
             const char* content_type, http_response_t* response) {
    http_client_options_t options = {
        .url = url,
        .method = HTTP_METHOD_PUT,
        .body = body,
        .body_len = body_len,
        .content_type = content_type ? content_type : "application/json",
        .timeout_ms = 30000,
        .enable_gzip = true,
        .auto_decompress = true,
    };
    
    http_client_t* client = http_client_create(&options);
    if (!client) return -1;
    
    int ret = http_client_execute(client);
    
    if (ret == 0) {
        http_response_t* resp = http_client_get_response(client);
        if (resp) {
            *response = *resp;
        }
    }
    
    http_client_destroy(client);
    
    return ret;
}

int http_delete(const char* url, http_response_t* response) {
    http_client_options_t options = {
        .url = url,
        .method = HTTP_METHOD_DELETE,
        .timeout_ms = 30000,
        .enable_gzip = true,
        .auto_decompress = true,
    };
    
    http_client_t* client = http_client_create(&options);
    if (!client) return -1;
    
    int ret = http_client_execute(client);
    
    if (ret == 0) {
        http_response_t* resp = http_client_get_response(client);
        if (resp) {
            *response = *resp;
        }
    }
    
    http_client_destroy(client);
    
    return ret;
}

int http_download(const char* url, const char* save_path) {
    http_client_options_t options = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .download_path = save_path,
        .timeout_ms = 60000,
        .enable_gzip = true,
        .auto_decompress = true,
    };
    
    http_client_t* client = http_client_create(&options);
    if (!client) return -1;
    
    int ret = http_client_execute(client);
    
    http_client_destroy(client);
    
    return ret;
}