/**
 * @file iot_at.c
 * @brief ML307N 平台虚拟AT通道适配器实现
 * @details 将 ML307N 平台 cm_virt_at 接口封装为统一命名规范的函数，
 *          支持跨平台编译。
 */

#include "iot_at.h"

/* ============================================================
 * AT 操作函数实现
 * ============================================================ */

/**
 * @brief 注册URC上报回调函数
 * @param[in] prefix URC消息前缀
 * @param[in] cb     URC回调函数
 * @return 0 成功，<0 失败
 */
int32_t iot_at_register_urc(const char *prefix, iot_at_urc_callback_t cb)
{
    return cm_virt_at_urc_cb_reg((const uint8_t *)prefix, (cm_virt_at_urc_cb)cb);
}

/**
 * @brief 取消注册URC上报回调函数
 * @param[in] prefix URC消息前缀
 * @param[in] cb     URC回调函数
 * @return 0 成功，<0 失败
 */
int32_t iot_at_unregister_urc(const char *prefix, iot_at_urc_callback_t cb)
{
    return cm_virt_at_urc_cb_dereg((const uint8_t *)prefix, (cm_virt_at_urc_cb)cb);
}

/**
 * @brief 通过虚拟AT通道异步发送AT指令
 * @param[in] at_cmd   发送的AT指令字符串
 * @param[in] cb       消息回调函数
 * @param[in] user_param 用户自定义参数
 * @return 0 成功，<0 失败
 */
int32_t iot_at_send_async(const char *at_cmd, iot_at_callback_t cb, void *user_param)
{
    return cm_virt_at_send_async((const uint8_t *)at_cmd, (cm_virt_at_cb)cb, user_param);
}

/**
 * @brief 通过虚拟AT通道同步发送AT指令
 * @param[in]  at_cmd   发送的AT指令字符串
 * @param[out] rsp      接收响应数据的缓冲区
 * @param[out] rsp_len  输入为缓冲区大小，输出为实际响应长度
 * @param[in]  timeout  超时时间（毫秒）
 * @return 0 成功，<0 失败
 * @warning 不建议使用等待时间超过5秒的指令，确保rsp空间足够防止踩内存
 */
int32_t iot_at_send_sync(const char *at_cmd, char *rsp, int32_t *rsp_len, uint32_t timeout)
{
    return cm_virt_at_send_sync((const uint8_t *)at_cmd, (uint8_t *)rsp, rsp_len, timeout);
}