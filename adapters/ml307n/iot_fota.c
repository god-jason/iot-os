/**
 * @file iot_fota.c
 * @brief ML307N 平台 FOTA 适配器实现
 * @details 基于 cm_fota 接口实现 FOTA 功能封装，
 *          提供统一的 FOTA 操作函数。
 */

#include "iot_fota.h"

/**
 * @brief 设置FOTA服务器URL（HTTP/HTTPS）
 * @param[in] url 服务器URL，最大256字节，必须指定协议类型
 * @return 0 成功，其他失败
 */
int32_t iot_fota_set_url(const char *url)
{
    return cm_fota_set_url((char *)url);
}

/**
 * @brief 设置FOTA失败时的下一次重启时间
 * @param[in] time 重启时间结构体指针
 * @return 0 成功，其他失败
 */
int32_t iot_fota_set_reboot_time(const iot_fota_reboot_time_t *time)
{
    return cm_fota_set_reboot_time((cm_fota_reboot_time *)time);
}

/**
 * @brief 读取FOTA服务器配置
 * @param[out] cfg FOTA信息结构体指针
 */
int32_t iot_fota_read_config(iot_fota_info_t *cfg)
{
    return cm_fota_read_config((cm_fota_info_t *)cfg);
}

/**
 * @brief 注册FOTA结果回调函数
 * @param[in] cb 回调函数指针
 */
int32_t iot_fota_register_callback(iot_fota_result_callback_t cb)
{
    return cm_fota_res_callback_register((cm_fota_result_callback)cb);
}

/**
 * @brief 执行FOTA升级
 * @return 0 成功，<0 失败
 */
int32_t iot_fota_exec_upgrade(void)
{
    return cm_fota_exec_upgrade();
}