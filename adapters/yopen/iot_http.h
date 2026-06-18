/**
 * @file iot_http.h
 * @brief YOPEN 平台 HTTP 客户端适配器头文件
 * @details 提供 HTTP 客户端功能封装，
 *          支持 GET/POST/PUT/DELETE 等请求方法。
 */

#ifndef IOT_HTTP_YOPEN_H
#define IOT_HTTP_YOPEN_H

#include "../../iot_types.h"

/* ============================================================
 * HTTP 客户端类型定义
 * ============================================================ */

/** @brief HTTP请求方法字符串 */
#define IOT_HTTP_METHOD_GET_STR    "GET"
#define IOT_HTTP_METHOD_POST_STR   "POST"
#define IOT_HTTP_METHOD_PUT_STR    "PUT"
#define IOT_HTTP_METHOD_DELETE_STR "DELETE"
#define IOT_HTTP_METHOD_HEAD_STR   "HEAD"

/** @brief HTTP客户端句柄类型 */
typedef void* iot_http_handle_t;

/** @brief HTTP配置参数类型 */
typedef enum {
    IOT_HTTP_CONFIG_TIMEOUT,         /**< 超时时间(毫秒) */
    IOT_HTTP_CONFIG_BUFF_SIZE,       /**< 缓冲区大小 */
    IOT_HTTP_CONFIG_SSL_ENABLE,      /**< SSL使能 */
} iot_http_config_key_t;

/* ============================================================
 * HTTP 客户端宏定义
 * ============================================================ */

/**
 * @brief 创建HTTP客户端实例
 * @return HTTP句柄，失败返回NULL
 */
#define iot_http_create() \
    ((iot_http_handle_t)0)  // TODO: 待平台实现

/**
 * @brief 销毁HTTP客户端实例
 * @param[in] handle HTTP句柄
 * @return 0 成功，<0 失败
 */
#define iot_http_destroy(handle) \
    (0)  // TODO: 待平台实现

/**
 * @brief 设置HTTP客户端配置参数
 * @param[in] handle HTTP句柄
 * @param[in] key 配置参数类型
 * @param[in] value 配置参数值
 * @return 0 成功，<0 失败
 */
#define iot_http_set_config(handle, key, value) \
    (0)  // TODO: 待平台实现

/**
 * @brief HTTP GET请求
 * @param[in] handle HTTP句柄
 * @param[in] url 请求URL
 * @param[in] headers 请求头（可为NULL）
 * @param[out] rsp 响应缓冲区
 * @param[in] rsp_size 响应缓冲区大小
 * @return >=0 响应体长度，<0 失败
 */
int32_t iot_http_get(iot_http_handle_t handle, const char *url,
                     const char *headers, char *rsp, uint32_t rsp_size);

/**
 * @brief HTTP POST请求
 * @param[in] handle HTTP句柄
 * @param[in] url 请求URL
 * @param[in] headers 请求头（可为NULL）
 * @param[in] body 请求体
 * @param[in] body_len 请求体长度
 * @param[out] rsp 响应缓冲区
 * @param[in] rsp_size 响应缓冲区大小
 * @return >=0 响应体长度，<0 失败
 */
int32_t iot_http_post(iot_http_handle_t handle, const char *url,
                      const char *headers, const char *body, uint32_t body_len,
                      char *rsp, uint32_t rsp_size);

/**
 * @brief HTTP PUT请求
 * @param[in] handle HTTP句柄
 * @param[in] url 请求URL
 * @param[in] headers 请求头（可为NULL）
 * @param[in] body 请求体
 * @param[in] body_len 请求体长度
 * @param[out] rsp 响应缓冲区
 * @param[in] rsp_size 响应缓冲区大小
 * @return >=0 响应体长度，<0 失败
 */
int32_t iot_http_put(iot_http_handle_t handle, const char *url,
                     const char *headers, const char *body, uint32_t body_len,
                     char *rsp, uint32_t rsp_size);

/**
 * @brief HTTP DELETE请求
 * @param[in] handle HTTP句柄
 * @param[in] url 请求URL
 * @param[in] headers 请求头（可为NULL）
 * @param[out] rsp 响应缓冲区
 * @param[in] rsp_size 响应缓冲区大小
 * @return >=0 响应体长度，<0 失败
 */
int32_t iot_http_delete(iot_http_handle_t handle, const char *url,
                        const char *headers, char *rsp, uint32_t rsp_size);

/**
 * @brief HTTP HEAD请求
 * @param[in] handle HTTP句柄
 * @param[in] url 请求URL
 * @param[in] headers 请求头（可为NULL）
 * @return >=0 成功，<0 失败
 */
int32_t iot_http_head(iot_http_handle_t handle, const char *url, const char *headers);

/**
 * @brief 获取HTTP响应状态码
 * @param[in] handle HTTP句柄
 * @return HTTP状态码
 */
int32_t iot_http_get_status_code(iot_http_handle_t handle);

/**
 * @brief 获取HTTP响应头
 * @param[in] handle HTTP句柄
 * @param[out] buf 响应头缓冲区
 * @param[in] size 缓冲区大小
 * @return >=0 实际长度，<0 失败
 */
int32_t iot_http_get_response_header(iot_http_handle_t handle, char *buf, uint32_t size);

#endif /* IOT_HTTP_YOPEN_H */
