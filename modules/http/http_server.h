/**
 * @file http_server.h
 * @brief HTTP 服务器接口定义
 *
 * 基于 net_socket 实现的 HTTP/1.1 服务器头文件，提供服务器创建、启动、停止、
 * 路由注册、静态文件服务、请求回调等接口。支持 GET、POST、PUT、DELETE、HEAD、
 * OPTIONS 等 HTTP 方法的请求处理，以及 HTTP 响应的构建与发送。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
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

typedef struct iot_http_server iot_http_server_t;

typedef enum {
    IOT_HTTP_SERVER_METHOD_GET     = 0,
    IOT_HTTP_SERVER_METHOD_POST    = 1,
    IOT_HTTP_SERVER_METHOD_PUT     = 2,
    IOT_HTTP_SERVER_METHOD_DELETE  = 3,
    IOT_HTTP_SERVER_METHOD_HEAD    = 4,
    IOT_HTTP_SERVER_METHOD_OPTIONS = 5,
    IOT_HTTP_SERVER_METHOD_ALL     = 6,
} iot_http_server_method_t;

typedef struct {
    const char* method;
    const char* path;
    const char* query;
    const char* header;
    size_t header_len;
    const char* body;
    size_t body_len;
} iot_http_server_request_t;

typedef struct {
    int status_code;
    char* content_type;
    char* headers;
    size_t headers_len;
    char* body;
    size_t body_len;
} iot_http_server_response_t;

typedef void (*iot_http_server_handler_t)(const iot_http_server_request_t* req, iot_http_server_response_t* resp);

typedef void (*iot_http_server_request_callback_t)(iot_http_server_t* server, 
                                               const iot_http_server_request_t* req, 
                                               iot_http_server_response_t* resp,
                                               void* user_data);

/*===========================================================
 * HTTP 服务器接口
 *===========================================================*/

iot_http_server_t* iot_http_server_create(void);
void iot_http_server_destroy(iot_http_server_t* server);

int iot_http_server_start(iot_http_server_t* server, uint16_t port);
void iot_http_server_stop(iot_http_server_t* server);

int iot_http_server_register_handler(iot_http_server_t* server, iot_http_server_method_t method,
                                 const char* path, iot_http_server_handler_t handler);

int iot_http_server_set_static_dir(iot_http_server_t* server, const char* dir);

void iot_http_server_set_request_callback(iot_http_server_t* server, 
                                      iot_http_server_request_callback_t callback,
                                      void* user_data);

/*===========================================================
 * HTTP 响应接口
 *===========================================================*/

void iot_http_server_response_write_head(iot_http_server_response_t* resp, int status_code, 
                                     const char* content_type, const char* headers);
void iot_http_server_response_set_header(iot_http_server_response_t* resp, const char* key, const char* value);
void iot_http_server_response_end(iot_http_server_response_t* resp, const char* body, size_t body_len);
void iot_http_server_response_free(iot_http_server_response_t* resp);

#ifdef __cplusplus
}
#endif

#endif /* IOT_HTTP_SERVER_H */