/**
 * @file iot_wifi.h
 * @brief ML307N 平台 WiFi扫描适配器头文件
 * @details 封装 ML307N 平台 cm_wifiscan 接口，提供统一的WiFi扫描操作宏定义。
 *          支持设置扫描参数、开始/停止扫描、查询扫描结果等功能。
 */

#ifndef IOT_WIFI_ML307N_H
#define IOT_WIFI_ML307N_H

#include "../../iot_types.h"
#include "cm_wifiscan.h"

/** @brief WiFi扫描配置结构体重定义 */
#define iot_wifi_scan_cfg_t     cm_wifiscan_cfg_t

/** @brief WiFi扫描结果结构体重定义 */
#define iot_wifi_scan_result_t  cm_wifiscan_result_t

/** @brief WiFi扫描结果信息结构体重定义 */
#define iot_wifi_ap_info_t      cm_wifiscan_ap_info_t

/**
 * @brief 设置WiFi扫描参数
 * @param[in] cfg 扫描配置结构体指针
 * @return 0 成功，-1 失败
 */
#define iot_wifi_scan_set_cfg(cfg) \
    cm_wifiscan_set_cfg((cm_wifiscan_cfg_t *)(cfg))

/**
 * @brief 开始WiFi扫描
 * @return 0 成功，-1 失败
 * @note 扫描结果通过回调函数异步返回
 */
#define iot_wifi_scan_start() \
    cm_wifiscan_start()

/**
 * @brief 停止WiFi扫描
 * @return 0 成功，-1 失败
 */
#define iot_wifi_scan_stop() \
    cm_wifiscan_stop()

/**
 * @brief 查询WiFi扫描结果
 * @param[out] result 扫描结果结构体指针
 * @return 0 成功，-1 失败
 */
#define iot_wifi_scan_get_result(result) \
    cm_wifiscan_get_result((cm_wifiscan_result_t *)(result))

#endif /* IOT_WIFI_ML307N_H */
