/**
 * @file iot_http.h
 * @brief ML307N 平台 HTTP 客户端适配器头文件
 * @details 封装 ML307N 平台 cm_http 接口，提供统一的 HTTP/HTTPS 操作宏定义。
 *          支持创建/关闭HTTP客户端、设置请求头、发送请求、读取响应等功能。
 */

#ifndef IOT_HTTP_ML307N_H
#define IOT_HTTP_ML307N_H

#include "../../iot_types.h"
#include "cm_http.h"

/** @brief HTTP传输超时结构体重定义 */
#define iot_http_timeout_t          cm_http_timeout_t

/** @brief HTTP服务器地址结构体重定义 */
#define iot_http_server_t           cm_http_server_t

/** @brief HTTP请求头参数结构体重定义 */
#define iot_http_request_header_t   cm_http_request_param_t

/** @brief HTTP响应参数结构体重定义 */
#define iot_http_response_t         cm_http_response_param_t

/** @brief HTTP请求方法枚举重定义 */
#define iot_http_method_t           cm_http_method_e

/** @brief HTTP客户端句柄重定义 */
#define iot_http_handle_t           cm_http_client_handle_t

/**
 * @brief 创建并配置HTTP客户端
 * @param[in] server HTTP服务器地址结构体指针
 * @return HTTP客户端句柄（NULL表示创建失败）
 * @note 创建后不会立即建立连接
 */
#define iot_http_client_create(server) \
    cm_http_client_create((cm_http_server_t *)(server))

/**
 * @brief 关闭并释放HTTP客户端资源
 * @param[in] handle HTTP客户端句柄
 */
#define iot_http_client_close(handle) \
    cm_http_client_close((cm_http_client_handle_t)(handle))

/**
 * @brief 连接HTTP服务器
 * @param[in] handle HTTP客户端句柄
 * @return 0 成功，<0 失败
 * @note 该接口需在大系统调用响应回调后才能调用
 */
#define iot_http_client_connect(handle) \
    cm_http_client_connect((cm_http_client_handle_t)(handle))

/**
 * @brief 断开HTTP服务器连接
 * @param[in] handle HTTP客户端句柄
 * @return 0 成功，<0 失败
 * @note 调用后不会释放资源
 */
#define iot_http_client_disconnect(handle) \
    cm_http_client_disconnect((cm_http_client_handle_t)(handle))

/**
 * @brief 读取服务器响应头
 * @param[in]  handle   HTTP客户端句柄
 * @param[out] response 响应结构体指针
 * @return 0 成功，<0 失败
 */
#define iot_http_client_read_response(handle, response) \
    cm_http_client_read_response((cm_http_client_handle_t)(handle), (cm_http_response_param_t *)(response))

/**
 * @brief 发送HTTP请求
 * @param[in] handle  HTTP客户端句柄
 * @param[in] request 请求参数结构体指针
 * @return 0 成功，<0 失败
 */
#define iot_http_client_send_request(handle, request) \
    cm_http_client_send_request((cm_http_client_handle_t)(handle), (cm_http_request_param_t *)(request))

/**
 * @brief 发送POST请求body数据
 * @param[in] handle HTTP客户端句柄
 * @param[in] data   发送数据缓冲区
 * @param[in] size   缓冲区大小
 * @return >=0 实际发送长度，<0 失败
 */
#define iot_http_client_send_post_data(handle, data, size) \
    cm_http_client_send_post_data((cm_http_client_handle_t)(handle), (uint8_t *)(data), (uint32_t)(size))

/**
 * @brief 读取响应数据（POST请求响应体）
 * @param[in]  handle HTTP客户端句柄
 * @param[out] data   接收缓冲区
 * @param[in]  size   缓冲区大小
 * @return >=0 实际读取长度，<0 失败
 */
#define iot_http_client_read_post_data(handle, data, size) \
    cm_http_client_read_post_data((cm_http_client_handle_t)(handle), (uint8_t *)(data), (uint32_t)(size))

/**
 * @brief 设置接收缓冲区大小
 * @param[in] handle     HTTP客户端句柄
 * @param[in] buffer_len 缓冲区大小（字节），建议>=128
 * @return 0 成功，<0 失败
 */
#define iot_http_client_set_recv_buffer_len(handle, buffer_len) \
    cm_http_client_set_recv_buffer_len((cm_http_client_handle_t)(handle), (uint32_t)(buffer_len))

/**
 * @brief 设置发送超时时间
 * @param[in] handle  HTTP客户端句柄
 * @param[in] timeout 超时时间结构体指针
 * @return 0 成功，<0 失败
 */
#define iot_http_client_set_send_timeout(handle, timeout) \
    cm_http_client_set_send_timeout((cm_http_client_handle_t)(handle), (cm_http_timeout_t *)(timeout))

/**
 * @brief 设置接收超时时间
 * @param[in] handle  HTTP客户端句柄
 * @param[in] timeout 超时时间结构体指针
 * @return 0 成功，<0 失败
 */
#define iot_http_client_set_recv_timeout(handle, timeout) \
    cm_http_client_set_recv_timeout((cm_http_client_handle_t)(handle), (cm_http_timeout_t *)(timeout))

#endif /* IOT_HTTP_ML307N_H */
