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

#if defined(_WIN32) && !defined(strncasecmp)
#define strncasecmp _strnicmp
#endif

#define HTTP_MAX_HEADER_SIZE 8192
#define HTTP_MAX_BODY_SIZE   1024 * 1024

static const char* http_method_to_string(http_method_t method);
static int http_build_request(http_client_t* client, char* buf, size_t buf_len);
static int http_parse_response(http_client_t* client, http_response_t* response, int finalize);
static int http_body_is_complete(http_client_t* client, size_t body_start, size_t body_available);
static int http_assign_body(http_client_t* client, http_response_t* response,
                            size_t body_start, size_t body_len);
static size_t http_decode_chunked_body(const char* src, size_t src_len,
                                       uint8_t** out, size_t* out_len);
static int http_parse_status_line(const char* line, int* status_code);
static int http_parse_header(const char* header, const char* key, char* value, size_t value_len);
static int http_connect(http_client_t* client);
static void http_recv_data(http_client_t* client);
static void http_socket_callback(net_socket_t* sock, net_event_type_t event, void* user_data);
static int http_client_do_request(http_client_t* client);

static const char* http_find_crlfcrlf(const char* buf, size_t len) {
    if (!buf || len < 4) return NULL;
    for (size_t i = 0; i + 3 < len; i++) {
        if (buf[i] == '\r' && buf[i + 1] == '\n' &&
            buf[i + 2] == '\r' && buf[i + 3] == '\n') {
            return buf + i;
        }
    }
    return NULL;
}

static const char* http_find_crlf(const char* buf, size_t len) {
    if (!buf || len < 2) return NULL;
    for (size_t i = 0; i + 1 < len; i++) {
        if (buf[i] == '\r' && buf[i + 1] == '\n') {
            return buf + i;
        }
    }
    return NULL;
}

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
    const char* end = strstr(line, "\r\n");
    if (!end) {
        end = line + strlen(line);
    }

    /* 跳过协议版本 token，定位到状态码 */
    while (p < end && *p != ' ') {
        p++;
    }
    while (p < end && *p == ' ') {
        p++;
    }
    if (p >= end || !isdigit((unsigned char)*p)) {
        return -1;
    }

    *status_code = atoi(p);
    return 0;
}

static int http_header_find(const char* header, const char* key, char* value, size_t value_len) {
    if (!header || !key || !value || value_len == 0) {
        return -1;
    }

    size_t key_len = strlen(key);
    const char* p = header;

    while (*p) {
        const char* line_end = strstr(p, "\r\n");
        if (!line_end) {
            line_end = p + strlen(p);
        }

        if ((size_t)(line_end - p) >= key_len &&
            strncasecmp(p, key, key_len) == 0 &&
            p[key_len] == ':') {
            const char* val = p + key_len + 1;
            while (*val == ' ' || *val == '\t') {
                val++;
            }

            size_t len = (size_t)(line_end - val);
            if (len >= value_len) {
                len = value_len - 1;
            }
            memcpy(value, val, len);
            value[len] = '\0';
            return 0;
        }

        if (*line_end == '\0') {
            break;
        }
        p = line_end + 2;
    }

    return -1;
}

static int http_parse_header(const char* header, const char* key, char* value, size_t value_len) {
    return http_header_find(header, key, value, value_len);
}

static int http_header_has_token(const char* value, const char* token) {
    if (!value || !token) {
        return 0;
    }

    size_t token_len = strlen(token);
    const char* p = value;

    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == ',') {
            p++;
        }
        if (strncasecmp(p, token, token_len) == 0 &&
            (p[token_len] == '\0' || p[token_len] == ' ' || p[token_len] == '\t' || p[token_len] == ',')) {
            return 1;
        }
        while (*p && *p != ',') {
            p++;
        }
        if (*p == ',') {
            p++;
        }
    }
    return 0;
}

static int http_chunked_body_complete(const char* body, size_t body_len) {
    if (!body || body_len < 5) {
        return 0;
    }

    for (size_t i = 0; i + 4 < body_len; i++) {
        if (body[i] == '0' && body[i + 1] == '\r' && body[i + 2] == '\n' &&
            body[i + 3] == '\r' && body[i + 4] == '\n') {
            return 1;
        }
    }
    return 0;
}

static int http_body_is_complete(http_client_t* client, size_t body_start, size_t body_available) {
    if (!client) {
        return 0;
    }

    if (client->chunked) {
        return http_chunked_body_complete(client->recv_buf + body_start, body_available) ||
               client->conn_closed;
    }

    if (client->content_length >= 0) {
        return body_available >= (size_t)client->content_length;
    }

    return client->conn_closed;
}

static size_t http_decode_chunked_body(const char* src, size_t src_len,
                                       uint8_t** out, size_t* out_len) {
    if (!src || src_len == 0 || !out || !out_len) {
        return 0;
    }

    size_t cap = src_len;
    uint8_t* dst = (uint8_t*)iot_malloc(cap);
    if (!dst) {
        return 0;
    }

    size_t written = 0;
    size_t pos = 0;

    while (pos < src_len) {
        size_t line_end = pos;
        while (line_end + 1 < src_len && !(src[line_end] == '\r' && src[line_end + 1] == '\n')) {
            line_end++;
        }
        if (line_end + 1 >= src_len) {
            break;
        }

        unsigned long chunk_size = strtoul(src + pos, NULL, 16);
        pos = line_end + 2;
        if (chunk_size == 0) {
            break;
        }
        if (pos + chunk_size > src_len) {
            break;
        }

        if (written + chunk_size > cap) {
            size_t new_cap = cap + chunk_size + 256;
            uint8_t* new_dst = (uint8_t*)iot_realloc(dst, new_cap);
            if (!new_dst) {
                iot_free(dst);
                return 0;
            }
            dst = new_dst;
            cap = new_cap;
        }

        memcpy(dst + written, src + pos, chunk_size);
        written += chunk_size;
        pos += chunk_size;

        if (pos + 1 < src_len && src[pos] == '\r' && src[pos + 1] == '\n') {
            pos += 2;
        }
    }

    if (written == 0) {
        iot_free(dst);
        *out = NULL;
        *out_len = 0;
        return 0;
    }

    *out = dst;
    *out_len = written;
    return written;
}

static int http_assign_body(http_client_t* client, http_response_t* response,
                            size_t body_start, size_t body_len) {
    const char* raw = client->recv_buf + body_start;

    if (client->chunked) {
        uint8_t* decoded = NULL;
        size_t decoded_len = 0;
        if (http_decode_chunked_body(raw, body_len, &decoded, &decoded_len) == 0 || !decoded) {
            return -1;
        }

        if (client->response_gzip && client->options.auto_decompress && !client->options.download_path) {
            uint8_t* decompressed = http_gzip_decompress_alloc(decoded, decoded_len, &response->body_len);
            iot_free(decoded);
            if (!decompressed) {
                return -1;
            }
            response->body = (char*)decompressed;
            return 0;
        }

        response->body = (char*)decoded;
        response->body_len = decoded_len;
        return 0;
    }

    if (client->response_gzip && client->options.auto_decompress && !client->options.download_path) {
        uint8_t* decompressed = http_gzip_decompress_alloc((const uint8_t*)raw, body_len, &response->body_len);
        if (!decompressed) {
            return -1;
        }
        response->body = (char*)decompressed;
        return 0;
    }

    response->body = (char*)iot_malloc(body_len + 1);
    if (!response->body) {
        return -1;
    }
    memcpy(response->body, raw, body_len);
    response->body[body_len] = '\0';
    response->body_len = body_len;
    return 0;
}

static int http_parse_response(http_client_t* client, http_response_t* response, int finalize) {
    if (!client || !client->recv_buf || client->recv_len < 4) {
        return -1;
    }
    
    memset(response, 0, sizeof(http_response_t));
    
    char* header_end = (char*)http_find_crlfcrlf(client->recv_buf, client->recv_len);
    if (!header_end) {
        return -1;
    }
    
    size_t header_len = (size_t)(header_end - client->recv_buf);
    
    const char* status_end = http_find_crlf(client->recv_buf, client->recv_len);
    if (status_end) {
        char status_line[256];
        size_t status_len = (size_t)(status_end - client->recv_buf);
        if (status_len >= sizeof(status_line)) status_len = sizeof(status_line) - 1;
        memcpy(status_line, client->recv_buf, status_len);
        status_line[status_len] = '\0';
        http_parse_status_line(status_line, &response->status_code);
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
        client->chunked = http_header_has_token(transfer_encoding, "chunked");
    } else {
        client->chunked = 0;
    }
    
    client->response_gzip = http_gzip_check_response(response->header);
    
    size_t body_start = header_len + 4;
    size_t body_available = client->recv_len - body_start;
    
    if (body_available == 0) {
        if (client->content_length == 0) {
            return 0;
        }
        return -1;
    }

    size_t body_len = body_available;
    if (!client->chunked && client->content_length >= 0) {
        if (body_available < (size_t)client->content_length &&
            !finalize && !client->conn_closed) {
            return -1;
        }
        if (body_available > (size_t)client->content_length) {
            body_len = (size_t)client->content_length;
        }
    } else if (client->chunked) {
        if (!http_body_is_complete(client, body_start, body_available) && !finalize) {
            return -1;
        }
    } else if (!finalize && !client->conn_closed) {
        /* 无 Content-Length：gzip 正文可在连接关闭前收齐 */
        if (client->response_gzip && client->options.auto_decompress &&
            !client->options.download_path) {
            if (http_assign_body(client, response, body_start, body_len) == 0) {
                return 0;
            }
        }
        return -1;
    }

    if (http_assign_body(client, response, body_start, body_len) != 0) {
        if (finalize && client->response_gzip && client->options.auto_decompress) {
            bool saved = client->options.auto_decompress;
            client->options.auto_decompress = false;
            if (http_assign_body(client, response, body_start, body_len) == 0) {
                client->options.auto_decompress = saved;
                return 0;
            }
            client->options.auto_decompress = saved;
        }
        return -1;
    }

    return 0;
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

    size_t prev_len = client->recv_len;
    size_t got;

    while ((got = net_socket_drain_recv((sock_t)client->sock,
                                        &client->recv_buf,
                                        &client->recv_len,
                                        &client->recv_capacity)) > 0) {
        if (client->fd && client->recv_len > prev_len) {
            size_t n = client->recv_len - prev_len;
            iot_fs_write(client->fd, client->recv_buf + prev_len, n);
            client->downloaded += n;

            if (client->options.download_cb) {
                client->options.download_cb(client, client->downloaded, client->total_size,
                                            client->options.user_data);
            }
        }
        prev_len = client->recv_len;
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
            /* 由 http_client_do_request 轮询 drain，避免与主线程互斥锁死锁 */
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
        LOG_ERROR("http client create failed: out of memory");
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
    client->chunked = 0;
    client->conn_closed = 0;
    
    client->mutex = iot_mutex_create();
    client->sem = iot_sem_create(1, 0);
    
    client->gzip_ctx = http_gzip_create();
    
    list_init(&client->list_node);
    
    LOG_INFO("http client created");
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

    int connect_wait = 0;
    while (net_socket_get_state((sock_t)client->sock) == NET_SOCK_STATE_CONNECTING &&
           connect_wait < client->options.timeout_ms) {
        iot_task_delay(10);
        connect_wait += 10;
    }
    if (net_socket_get_state((sock_t)client->sock) != NET_SOCK_STATE_CONNECTED) {
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

        iot_mutex_lock(client->mutex, -1);

        if (!client->options.download_path && client->recv_buf && client->recv_len > 0) {
            const char* header_end = http_find_crlfcrlf(client->recv_buf, client->recv_len);
            if (header_end) {
                char header_copy[HTTP_MAX_HEADER_SIZE];
                size_t header_len = (size_t)(header_end - client->recv_buf);
                if (header_len >= HTTP_MAX_HEADER_SIZE) header_len = HTTP_MAX_HEADER_SIZE - 1;
                memcpy(header_copy, client->recv_buf, header_len);
                header_copy[header_len] = '\0';

                int status_code;
                char status_line[256];
                const char* status_end = http_find_crlf(client->recv_buf, client->recv_len);
                if (status_end) {
                    size_t status_len = (size_t)(status_end - client->recv_buf);
                    if (status_len >= sizeof(status_line)) status_len = sizeof(status_line) - 1;
                    memcpy(status_line, client->recv_buf, status_len);
                    status_line[status_len] = '\0';
                    http_parse_status_line(status_line, &status_code);
                } else {
                    status_code = 0;
                }

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
                        client->options = new_options;

                        iot_mutex_unlock(client->mutex);
                        return http_client_do_request(client);
                    }
                }

                if (http_parse_response(client, &client->response, 0) == 0) {
                    client->request_done = 1;
                    client->request_failed = 0;
                    iot_mutex_unlock(client->mutex);
                    break;
                }
            }
        }

        net_sock_state_t sock_state = net_socket_get_state((sock_t)client->sock);
        if (sock_state == NET_SOCK_STATE_CLOSED || sock_state == NET_SOCK_STATE_ERROR) {
            client->conn_closed = 1;
            iot_mutex_unlock(client->mutex);
            http_recv_data(client);
            iot_mutex_lock(client->mutex, -1);
            if (client->options.download_path) {
                client->request_done = 1;
                client->request_failed = (client->downloaded == 0);
            } else if (client->recv_len > 0) {
                if (http_parse_response(client, &client->response, 1) == 0) {
                    client->request_done = 1;
                    client->request_failed = 0;
                } else {
                    client->request_done = 1;
                    client->request_failed = 1;
                }
            } else {
                client->request_done = 1;
                client->request_failed = 1;
            }
            iot_mutex_unlock(client->mutex);
            break;
        }
        iot_mutex_unlock(client->mutex);

        iot_task_delay(10);
        wait_ms += 10;
    }

    if (!client->request_done) {
        http_recv_data(client);
        client->conn_closed = 1;
        if (client->options.download_path) {
            client->request_done = 1;
            client->request_failed = (client->downloaded == 0);
        } else if (client->recv_len > 0 &&
            http_parse_response(client, &client->response, 1) == 0) {
            client->request_done = 1;
            client->request_failed = 0;
        } else {
            client->request_done = 1;
            client->request_failed = 1;
        }
    }

    if (client->fd) {
        iot_fs_close(client->fd);
        client->fd = NULL;
    }
    
    if (client->sock) {
        net_socket_close((sock_t)client->sock);
        client->sock = NULL;
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
    client->content_length = -1;
    client->chunked = 0;
    client->conn_closed = 0;
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