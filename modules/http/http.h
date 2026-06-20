/**
 * @file http.h
 * @brief HTTP 客户端接口定义
 *
 * 基于 net_socket 实现的 HTTP/1.1 客户端，支持 GET、POST、PUT、DELETE 等方法，
 * 支持文件下载、自定义头、超时配置等功能。
 */
#ifndef IOT_HTTP_H
#define IOT_HTTP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

/* HTTP 方法 */
typedef enum {
    HTTP_METHOD_GET     = 0,
    HTTP_METHOD_POST    = 1,
    HTTP_METHOD_PUT     = 2,
    HTTP_METHOD_DELETE  = 3,
    HTTP_METHOD_HEAD    = 4,
    HTTP_METHOD_OPTIONS = 5,
} http_method_t;

/* HTTP 状态码 */
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

/* HTTP 客户端句柄 */
typedef struct http_client http_client_t;

/* HTTP 响应结构 */
typedef struct {
    int status_code;              /* HTTP 状态码 */
    char* header;                 /* 响应头 */
    size_t header_len;            /* 响应头长度 */
    char* body;                   /* 响应体 */
    size_t body_len;              /* 响应体长度 */
    char* error;                  /* 错误信息 */
} http_response_t;

/* HTTP 回调函数类型 */
typedef void (*http_response_callback_t)(http_client_t* client, http_response_t* response, void* user_data);
typedef void (*http_download_callback_t)(http_client_t* client, size_t downloaded, size_t total, void* user_data);

/* HTTP 配置选项 */
typedef struct {
    const char* url;              /* 请求 URL */
    http_method_t method;         /* HTTP 方法 */
    const char* host;             /* 主机名 */
    uint16_t port;                /* 端口号 */
    const char* path;             /* 请求路径 */
    const char* query;            /* 查询参数 */
    const char* body;             /* 请求体 */
    size_t body_len;              /* 请求体长度 */
    const char* content_type;     /* Content-Type */
    const char* headers;          /* 自定义请求头（格式："Key1: Value1\r\nKey2: Value2"） */
    int timeout_ms;               /* 超时时间（毫秒），默认 30000 */
    int max_redirects;            /* 最大重定向次数，默认 3 */
    const char* download_path;    /* 下载文件保存路径 */
    http_download_callback_t download_cb; /* 下载进度回调 */
} http_request_t;

/*===========================================================
 * HTTP 客户端接口
 *===========================================================*/

/**
 * @brief 创建 HTTP 客户端
 * @return HTTP 客户端句柄，失败返回 NULL
 */
http_client_t* http_client_create(void);

/**
 * @brief 销毁 HTTP 客户端
 * @param client HTTP 客户端句柄
 */
void http_client_destroy(http_client_t* client);

/**
 * @brief 执行同步 HTTP 请求
 * @param client HTTP 客户端句柄
 * @param request 请求配置
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
int http_client_request(http_client_t* client, const http_request_t* request, http_response_t* response);

/**
 * @brief 执行异步 HTTP 请求
 * @param client HTTP 客户端句柄
 * @param request 请求配置
 * @param callback 响应回调函数
 * @param user_data 用户数据
 * @return 0 成功（请求已提交），-1 失败
 */
int http_client_request_async(http_client_t* client, const http_request_t* request, 
                              http_response_callback_t callback, void* user_data);

/**
 * @brief 执行 GET 请求（同步）
 * @param client HTTP 客户端句柄
 * @param url 请求 URL
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
int http_client_get(http_client_t* client, const char* url, http_response_t* response);

/**
 * @brief 执行 POST 请求（同步）
 * @param client HTTP 客户端句柄
 * @param url 请求 URL
 * @param body 请求体
 * @param body_len 请求体长度
 * @param content_type Content-Type
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
int http_client_post(http_client_t* client, const char* url, const char* body, size_t body_len,
                     const char* content_type, http_response_t* response);

/**
 * @brief 下载文件（同步）
 * @param client HTTP 客户端句柄
 * @param url 文件 URL
 * @param save_path 保存路径
 * @param progress_cb 进度回调（可选）
 * @param user_data 用户数据
 * @return 0 成功，-1 失败
 */
int http_client_download(http_client_t* client, const char* url, const char* save_path,
                         http_download_callback_t progress_cb, void* user_data);

/**
 * @brief 设置 HTTP 客户端超时时间
 * @param client HTTP 客户端句柄
 * @param timeout_ms 超时时间（毫秒）
 */
void http_client_set_timeout(http_client_t* client, int timeout_ms);

/**
 * @brief 设置 HTTP 客户端最大重定向次数
 * @param client HTTP 客户端句柄
 * @param max_redirects 最大重定向次数
 */
void http_client_set_max_redirects(http_client_t* client, int max_redirects);

/**
 * @brief 释放 HTTP 响应内存
 * @param response HTTP 响应结构体
 */
void http_response_free(http_response_t* response);

/*===========================================================
 * 便捷接口
 *===========================================================*/

/**
 * @brief 执行简单 GET 请求（创建临时客户端）
 * @param url 请求 URL
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
int http_get(const char* url, http_response_t* response);

/**
 * @brief 执行简单 POST 请求（创建临时客户端）
 * @param url 请求 URL
 * @param body 请求体
 * @param body_len 请求体长度
 * @param content_type Content-Type
 * @param response 响应结果（输出参数）
 * @return 0 成功，-1 失败
 */
int http_post(const char* url, const char* body, size_t body_len, const char* content_type, http_response_t* response);

/**
 * @brief 下载文件（创建临时客户端）
 * @param url 文件 URL
 * @param save_path 保存路径
 * @return 0 成功，-1 失败
 */
int http_download(const char* url, const char* save_path);

/*===========================================================
 * URL 解析辅助接口
 *===========================================================*/

/**
 * @brief 解析 HTTP URL，提取 host、port、path
 * @param url URL 字符串
 * @param host 输出主机名缓冲区
 * @param host_len 缓冲区长度
 * @param port 输出端口号
 * @param path 输出路径缓冲区
 * @param path_len 缓冲区长度
 * @return 0 成功，-1 失败
 */
int http_parse_url(const char* url, char* host, size_t host_len, uint16_t* port, char* path, size_t path_len);

#ifdef __cplusplus
}
#endif

#endif /* IOT_HTTP_H */
