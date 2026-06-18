/**
 * @file iot_wifi.h
 * @brief YOPEN 平台 WiFi 扫描适配器头文件
 * @details 提供 WiFi 扫描功能封装。
 * @note 暂未支持，需要平台实现
 */

#ifndef IOT_WIFI_YOPEN_H
#define IOT_WIFI_YOPEN_H

#include "../../iot_types.h"

/* ============================================================
 * WiFi 类型定义
 * ============================================================ */

/** @brief WiFi安全类型 */
typedef enum {
    IOT_WIFI_SEC_NONE = 0,           /**< 开放网络 */
    IOT_WIFI_SEC_WEP = 1,            /**< WEP加密 */
    IOT_WIFI_SEC_WPA = 2,            /**< WPA加密 */
    IOT_WIFI_SEC_WPA2 = 3,           /**< WPA2加密 */
    IOT_WIFI_SEC_WPA3 = 4,           /**< WPA3加密 */
} iot_wifi_security_t;

/* ============================================================
 * WiFi 接口（暂未支持）
 * ============================================================ */

/**
 * @brief 初始化WiFi模块
 * @return 0 成功，<0 失败
 */
int32_t iot_wifi_init(void);

/**
 * @brief 去初始化WiFi模块
 * @return 0 成功，<0 失败
 */
int32_t iot_wifi_deinit(void);

/**
 * @brief 扫描WiFi热点
 * @param[in] cb 扫描结果回调
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int32_t iot_wifi_scan(iot_wifi_scan_cb_t cb, void *user_data);

/**
 * @brief 获取已保存的WiFi列表
 * @param[out] results 结果缓冲区
 * @param[in/out] count 输入缓冲区大小，输出实际数量
 * @return 0 成功，<0 失败
 */
int32_t iot_wifi_get_saved_results(iot_wifi_scan_result_t *results, uint32_t *count);

/**
 * @brief 连接WiFi热点
 * @param[in] ssid SSID名称
 * @param[in] password 密码（可为空）
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功，<0 失败
 */
int32_t iot_wifi_connect(const char *ssid, const char *password, uint32_t timeout_ms);

/**
 * @brief 断开WiFi连接
 * @return 0 成功，<0 失败
 */
int32_t iot_wifi_disconnect(void);

/**
 * @brief 获取WiFi连接状态
 * @return true 已连接，false 未连接
 */
bool iot_wifi_is_connected(void);

#endif /* IOT_WIFI_YOPEN_H */
