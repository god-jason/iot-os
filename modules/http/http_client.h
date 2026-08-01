/**
 * @file http_client.h
 * @brief HTTP 客户端接口定义
 *
 * 基于 net_socket 实现的 HTTP/1.1 客户端头文件，提供客户端创建、销毁、请求执行、
 * 响应解析等核心接口。支持 GET、POST、PUT、DELETE、HEAD、OPTIONS 等 HTTP 方法，
 * 支持文件下载、自定义请求头、超时配置、自动重定向、gzip 压缩/解压等功能。
 * 同时提供 http_get、http_post、http_put、http_delete、http_download 等便捷接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */
#ifndef IOT_HTTP_CLIENT_H
#define IOT_HTTP_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "iot_list.h"
#include "iot.h"
#include "net.h"
#include "http_gzip.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

typedef enum {
    IOT_HTTP_METHOD_GET     = 0,
    IOT_HTTP_METHOD_POST    = 1,
    IOT_HTTP_METHOD_PUT     = 2,
    IOT_HTTP_METHOD_DELETE  = 3,
    IOT_HTTP_METHOD_HEAD    = 4,
    IOT_HTTP_METHOD_OPTIONS = 5,
} iot_http_method_t;

typedef enum {
    IOT_HTTP_STATUS_OK                = 200,
    IOT_HTTP_STATUS_CREATED           = 201,
    IOT_HTTP_STATUS_NO_CONTENT        = 204,
    IOT_HTTP_STATUS_REDIRECT          = 301,
    IOT_HTTP_STATUS_FOUND             = 302,
    IOT_HTTP_STATUS_NOT_MODIFIED      = 304,
    IOT_HTTP_STATUS_BAD_REQUEST       = 400,
    IOT_HTTP_STATUS_UNAUTHORIZED      = 401,
    IOT_HTTP_STATUS_FORBIDDEN         = 403,
    IOT_HTTP_STATUS_NOT_FOUND         = 404,
    IOT_HTTP_STATUS_SERVER_ERROR      = 500,
    IOT_HTTP_STATUS_SERVICE_UNAVAIL   = 503,
} iot_http_status_t;

struct iot_http_client;
typedef struct iot_http_client iot_http_client_t;

typedef struct {
    int status_code;
    char* header;
    size_t header_len;
    char* body;
    size_t body_len;
    char* error;
} iot_http_response_t;

typedef void (*iot_http_response_callback_t)(iot_http_client_t* client, iot_http_response_t* response, void* user_data);
typedef void (*iot_http_download_callback_t)(iot_http_client_t* client, size_t downloaded, size_t total, void* user_data);

typedef struct {
    const char* url;
    iot_http_method_t method;
    const char* body;
    size_t body_len;
    const char* content_type;
    const char* headers;
    int timeout_ms;
    int max_redirects;
    const char* download_path;
    iot_http_download_callback_t download_cb;
    iot_http_response_callback_t response_cb;
    void* user_data;
    
    bool enable_gzip;
    int gzip_level;
    bool auto_decompress;
} iot_http_client_options_t;

struct iot_http_client {
    iot_net_socket_t* sock;
    iot_http_client_options_t options;
    
    char host[256];
    uint16_t port;
    char path[512];
    
    char* recv_buf;
    size_t recv_len;
    size_t recv_capacity;
    
    int redirect_count;
    int content_length;
    int chunked;
    int conn_closed;
    
    iot_fs_file_t fd;
    size_t downloaded;
    size_t total_size;
    
    iot_http_response_t response;
    int request_done;
    int request_failed;
    
    iot_mutex_t mutex;
    iot_sem_t sem;
    iot_http_gzip_ctx_t* gzip_ctx;
    bool response_gzip;
    
    list_head_t list_node;
};

/*===========================================================
 * HTTP 客户端接口
 *===========================================================*/

iot_http_client_t* iot_http_client_create(const iot_http_client_options_t* options);
void iot_http_client_destroy(iot_http_client_t* client);

int iot_http_client_execute(iot_http_client_t* client);

iot_http_response_t* iot_http_client_get_response(iot_http_client_t* client);

void iot_http_client_set_options(iot_http_client_t* client, const iot_http_client_options_t* options);

void iot_http_response_free(iot_http_response_t* response);

/*===========================================================
 * 便捷接口
 *===========================================================*/

int iot_http_get(const char* url, iot_http_response_t* response);

int iot_http_post(const char* url, const char* body, size_t body_len, 
              const char* content_type, iot_http_response_t* response);

int iot_http_put(const char* url, const char* body, size_t body_len,
             const char* content_type, iot_http_response_t* response);

int iot_http_delete(const char* url, iot_http_response_t* response);

int iot_http_download(const char* url, const char* save_path);

#ifdef __cplusplus
}
#endif

#endif /* IOT_HTTP_CLIENT_H */