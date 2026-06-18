/**
 * @file iot_wifi.c
 * @brief ML307N 平台 WiFi扫描适配器实现
 * @details 基于 cm_wifiscan 接口实现 WiFi扫描功能封装，
 *          提供统一的 WiFi扫描操作函数。
 */

#include "iot_wifi.h"

/**
 * @brief 设置WiFi扫描参数
 * @param[in] cfg 扫描配置结构体指针
 * @return 0 成功，-1 失败
 */
int32_t iot_wifi_scan_set_cfg(const iot_wifi_scan_cfg_t *cfg)
{
    return cm_wifiscan_set_cfg((cm_wifiscan_cfg_t *)cfg);
}

/**
 * @brief 开始WiFi扫描
 * @return 0 成功，-1 失败
 */
int32_t iot_wifi_scan_start(void)
{
    return cm_wifiscan_start();
}

/**
 * @brief 停止WiFi扫描
 * @return 0 成功，-1 失败
 */
int32_t iot_wifi_scan_stop(void)
{
    return cm_wifiscan_stop();
}

/**
 * @brief 查询WiFi扫描结果
 * @param[out] result 扫描结果结构体指针
 * @return 0 成功，-1 失败
 */
int32_t iot_wifi_scan_get_result(iot_wifi_scan_result_t *result)
{
    return cm_wifiscan_get_result((cm_wifiscan_result_t *)result);
}