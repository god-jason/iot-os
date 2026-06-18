/**
 * @file iot_http.c
 * @brief ML307N 平台 HTTP 客户端适配器实现
 * @details 基于 cm_http 接口实现 HTTP 功能封装，
 *          提供统一的 HTTP/HTTPS 操作函数。
 */

#include "iot_http.h"

/**
 * @brief 创建并配置HTTP客户端
 * @param[in] server HTTP服务器地址结构体指针
 * @return HTTP客户端句柄（NULL表示创建失败）
 */
iot_http_handle_t iot_http_client_create(const iot_http_server_t *server)
{
    return cm_http_client_create((cm_http_server_t *)server);
}

/**
 * @brief 关闭并释放HTTP客户端资源
 * @param[in] handle HTTP客户端句柄
 */
void iot_http_client_close(iot_http_handle_t handle)
{
    cm_http_client_close((cm_http_client_handle_t)handle);
}

/**
 * @brief 连接HTTP服务器
 * @param[in] handle HTTP客户端句柄
 * @return 0 成功，<0 失败
 */
int32_t iot_http_client_connect(iot_http_handle_t handle)
{
    return cm_http_client_connect((cm_http_client_handle_t)handle);
}

/**
 * @brief 断开HTTP服务器连接
 * @param[in] handle HTTP客户端句柄
 * @return 0 成功，<0 失败
 */
int32_t iot_http_client_disconnect(iot_http_handle_t handle)
{
    return cm_http_client_disconnect((cm_http_client_handle_t)handle);
}

/**
 * @brief 读取服务器响应头
 * @param[in]  handle   HTTP客户端句柄
 * @param[out] response 响应结构体指针
 * @return 0 成功，<0 失败
 */
int32_t iot_http_client_read_response(iot_http_handle_t handle, iot_http_response_t *response)
{
    return cm_http_client_read_response((cm_http_client_handle_t)handle, (cm_http_response_param_t *)response);
}

/**
 * @brief 发送HTTP请求
 * @param[in] handle  HTTP客户端句柄
 * @param[in] request 请求参数结构体指针
 * @return 0 成功，<0 失败
 */
int32_t iot_http_client_send_request(iot_http_handle_t handle, const iot_http_request_header_t *request)
{
    return cm_http_client_send_request((cm_http_client_handle_t)handle, (cm_http_request_param_t *)request);
}

/**
 * @brief 发送POST请求body数据
 * @param[in] handle HTTP客户端句柄
 * @param[in] data   发送数据缓冲区
 * @param[in] size   缓冲区大小
 * @return >=0 实际发送长度，<0 失败
 */
int32_t iot_http_client_send_post_data(iot_http_handle_t handle, const uint8_t *data, uint32_t size)
{
    return cm_http_client_send_post_data((cm_http_client_handle_t)handle, (uint8_t *)data, size);
}

/**
 * @brief 读取响应数据（POST请求响应体）
 * @param[in]  handle HTTP客户端句柄
 * @param[out] data   接收缓冲区
 * @param[in]  size   缓冲区大小
 * @return >=0 实际读取长度，<0 失败
 */
int32_t iot_http_client_read_post_data(iot_http_handle_t handle, uint8_t *data, uint32_t size)
{
    return cm_http_client_read_post_data((cm_http_client_handle_t)handle, data, size);
}

/**
 * @brief 设置接收缓冲区大小
 * @param[in] handle     HTTP客户端句柄
 * @param[in] buffer_len 缓冲区大小（字节），建议>=128
 * @return 0 成功，<0 失败
 */
int32_t iot_http_client_set_recv_buffer_len(iot_http_handle_t handle, uint32_t buffer_len)
{
    return cm_http_client_set_recv_buffer_len((cm_http_client_handle_t)handle, buffer_len);
}

/**
 * @brief 设置发送超时时间
 * @param[in] handle  HTTP客户端句柄
 * @param[in] timeout 超时时间结构体指针
 * @return 0 成功，<0 失败
 */
int32_t iot_http_client_set_send_timeout(iot_http_handle_t handle, const iot_http_timeout_t *timeout)
{
    return cm_http_client_set_send_timeout((cm_http_client_handle_t)handle, (cm_http_timeout_t *)timeout);
}

/**
 * @brief 设置接收超时时间
 * @param[in] handle  HTTP客户端句柄
 * @param[in] timeout 超时时间结构体指针
 * @return 0 成功，<0 失败
 */
int32_t iot_http_client_set_recv_timeout(iot_http_handle_t handle, const iot_http_timeout_t *timeout)
{
    return cm_http_client_set_recv_timeout((cm_http_client_handle_t)handle, (cm_http_timeout_t *)timeout);
}