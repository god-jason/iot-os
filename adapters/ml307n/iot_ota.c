/**
 * @file iot_ota.c
 * @brief ML307N 平台 OTA 适配器实现
 * @details 基于 cm_ota 接口实现 OTA 功能封装，
 *          提供统一的 OTA 操作函数。
 */

#include "iot_ota.h"

/**
 * @brief 初始化OTA下载
 * @param[in] url      升级包URL
 * @param[in] cb       下载数据回调函数
 * @param[in] cb_param 回调用户参数
 * @return 0 成功，-1 失败
 */
int32_t iot_ota_init(const char *url, iot_ota_download_callback_t cb, void *cb_param)
{
    return cm_ota_init((char *)url, (cm_ota_download_callback)cb, cb_param);
}

/**
 * @brief 开始OTA下载
 * @return 0 成功，-1 失败
 */
int32_t iot_ota_start(void)
{
    return cm_ota_start();
}

/**
 * @brief 取消OTA下载
 * @return 0 成功，-1 失败
 */
int32_t iot_ota_cancel(void)
{
    return cm_ota_cancel();
}

/**
 * @brief 获取OTA下载状态
 * @param[out] info 状态信息结构体指针
 * @return 0 成功，-1 失败
 */
int32_t iot_ota_get_status(iot_ota_status_info_t *info)
{
    return cm_ota_get_status((cm_ota_status_info_t *)info);
}

/**
 * @brief 注册OTA状态回调函数
 * @param[in] cb 状态回调函数指针
 */
int32_t iot_ota_register_status_callback(iot_ota_status_callback_t cb)
{
    return cm_ota_register_status_callback((cm_ota_status_callback)cb);
}

/**
 * @brief 写入OTA固件数据
 * @param[in] data 固件数据指针
 * @param[in] len  数据长度
 * @return 0 成功，-1 失败
 */
int32_t iot_ota_write_data(const uint8_t *data, uint32_t len)
{
    return cm_ota_write_data((uint8_t *)data, len);
}

/**
 * @brief 校验OTA固件完整性
 * @return 0 成功，-1 失败
 */
int32_t iot_ota_verify(void)
{
    return cm_ota_verify();
}

/**
 * @brief 触发OTA升级
 * @return 0 成功，-1 失败
 */
int32_t iot_ota_trigger_upgrade(void)
{
    return cm_ota_trigger_upgrade();
}