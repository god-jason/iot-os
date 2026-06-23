/**
 * @file http_client.h
 * @brief HTTP 客户端接口定义
 *
 * 基于 net_socket 实现的 HTTP/1.1 客户端，支持 GET、POST、PUT、DELETE 等方法，
 * 支持文件下载、自定义头、超时配置、gzip 压缩等功能。
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
    HTTP_METHOD_GET     = 0,
    HTTP_METHOD_POST    = 1,
    HTTP_METHOD_PUT     = 2,
    HTTP_METHOD_DELETE  = 3,
    HTTP_METHOD_HEAD    = 4,
    HTTP_METHOD_OPTIONS = 5,
} http_method_t;

typedef enum {
    HTTP_STATUS_OK                = 200,
    HTTP_STATUS_CREATED           = 201,
    HTTP_STATUS_NO_CONTENT        = 204,
    HTTP_STATUS_REDIRECT          = 301,
    HTTP_STATUS_FOUND             = 302,
    HTTP_STATUS_NOT_MODIFIED      = 304,
    HTTP_STATUS_BAD_REQUEST       = 400,
    HTTP_STATUS_UNAUTHORIZED      = 401,
    HTTP_STATUS_FORBIDDEN         = 403,
    HTTP_STATUS_NOT_FOUND         = 404,
    HTTP_STATUS_SERVER_ERROR      = 500,
    HTTP_STATUS_SERVICE_UNAVAIL   = 503,
} http_status_t;

struct http_client;
typedef struct http_client http_client_t;

typedef struct {
    int status_code;
    char* header;
    size_t header_len;
    char* body;
    size_t body_len;
    char* error;
} http_response_t;

typedef void (*http_response_callback_t)(http_client_t* client, http_response_t* response, void* user_data);
typedef void (*http_download_callback_t)(http_client_t* client, size_t downloaded, size_t total, void* user_data);

typedef struct {
    const char* url;
    http_method_t method;
    const char* body;
    size_t body_len;
    const char* content_type;
    const char* headers;
    int timeout_ms;
    int max_redirects;
    const char* download_path;
    http_download_callback_t download_cb;
    http_response_callback_t response_cb;
    void* user_data;
    
    bool enable_gzip;
    int gzip_level;
    bool auto_decompress;
} http_client_options_t;

struct http_client {
    net_socket_t* sock;
    http_client_options_t options;
    
    char host[256];
    uint16_t port;
    char path[512];
    
    char* recv_buf;
    size_t recv_len;
    size_t recv_capacity;
    
    int redirect_count;
    int content_length;
    int chunked;
    
    iot_fs_file_t fd;
    size_t downloaded;
    size_t total_size;
    
    http_response_t response;
    int request_done;
    int request_failed;
    
    iot_mutex_t mutex;
    iot_sem_t sem;
    http_gzip_ctx_t* gzip_ctx;
    bool response_gzip;
    
    list_head_t list_node;
};

/*===========================================================
 * HTTP 客户端接口
 *===========================================================*/

http_client_t* http_client_create(const http_client_options_t* options);
void http_client_destroy(http_client_t* client);

int http_client_execute(http_client_t* client);

http_response_t* http_client_get_response(http_client_t* client);

void http_client_set_options(http_client_t* client, const http_client_options_t* options);

void http_response_free(http_response_t* response);

/*===========================================================
 * 便捷接口
 *===========================================================*/

int http_get(const char* url, http_response_t* response);

int http_post(const char* url, const char* body, size_t body_len, 
              const char* content_type, http_response_t* response);

int http_put(const char* url, const char* body, size_t body_len,
             const char* content_type, http_response_t* response);

int http_delete(const char* url, http_response_t* response);

int http_download(const char* url, const char* save_path);

#ifdef __cplusplus
}
#endif

#endif /* IOT_HTTP_CLIENT_H */