/**
 * @file http_server.h
 * @brief HTTP 服务器接口定义
 *
 * 基于 net_socket 实现的 HTTP/1.1 服务器，支持路由注册、静态文件服务等功能。
 */

#ifndef IOT_HTTP_SERVER_H
#define IOT_HTTP_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

typedef struct http_server http_server_t;

typedef enum {
    HTTP_SERVER_METHOD_GET     = 0,
    HTTP_SERVER_METHOD_POST    = 1,
    HTTP_SERVER_METHOD_PUT     = 2,
    HTTP_SERVER_METHOD_DELETE  = 3,
    HTTP_SERVER_METHOD_HEAD    = 4,
    HTTP_SERVER_METHOD_OPTIONS = 5,
    HTTP_SERVER_METHOD_ALL     = 6,
} http_server_method_t;

typedef struct {
    const char* method;
    const char* path;
    const char* query;
    const char* header;
    size_t header_len;
    const char* body;
    size_t body_len;
} http_server_request_t;

typedef struct {
    int status_code;
    char* content_type;
    char* headers;
    size_t headers_len;
    char* body;
    size_t body_len;
} http_server_response_t;

typedef void (*http_server_handler_t)(const http_server_request_t* req, http_server_response_t* resp);

typedef void (*http_server_request_callback_t)(http_server_t* server, 
                                               const http_server_request_t* req, 
                                               http_server_response_t* resp,
                                               void* user_data);

/*===========================================================
 * HTTP 服务器接口
 *===========================================================*/

http_server_t* http_server_create(void);
void http_server_destroy(http_server_t* server);

int http_server_start(http_server_t* server, uint16_t port);
void http_server_stop(http_server_t* server);

int http_server_register_handler(http_server_t* server, http_server_method_t method,
                                 const char* path, http_server_handler_t handler);

int http_server_set_static_dir(http_server_t* server, const char* dir);

void http_server_set_request_callback(http_server_t* server, 
                                      http_server_request_callback_t callback,
                                      void* user_data);

/*===========================================================
 * HTTP 响应接口
 *===========================================================*/

void http_server_response_write_head(http_server_response_t* resp, int status_code, 
                                     const char* content_type, const char* headers);
void http_server_response_set_header(http_server_response_t* resp, const char* key, const char* value);
void http_server_response_end(http_server_response_t* resp, const char* body, size_t body_len);
void http_server_response_free(http_server_response_t* resp);

#ifdef __cplusplus
}
#endif

#endif /* IOT_HTTP_SERVER_H */