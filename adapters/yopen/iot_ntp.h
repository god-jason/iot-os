/**
 * @file iot_ntp.h
 * @brief YOPEN 平台 NTP 时间同步适配器头文件
 * @details 基于平台接口实现 NTP 时间同步功能，
 *          支持默认服务器和自定义服务器配置。
 */

#ifndef IOT_NTP_YOPEN_H
#define IOT_NTP_YOPEN_H

#include "../../iot_types.h"

/* 引入 yopen 平台网络时间头文件 */
#include "yopen_nw.h"

/* ============================================================
 * NTP 时间同步宏定义
 * ============================================================ */

/**
 * @brief 默认NTP服务器列表
 */
#define IOT_NTP_SERVER_DEFAULT_LIST \
    "cn.ntp.org.cn", "ntp.aliyun.com", "time.google.com"

/**
 * @brief NTP同步状态检查（需要应用层实现）
 * @return true 已同步，false 未同步
 */
bool iot_ntp_is_synced(void);

/**
 * @brief 发起NTP时间同步请求
 * @param[in] server NTP服务器地址（可为空，使用默认服务器）
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功，<0 失败
 */
int32_t iot_ntp_sync_request(const char *server, uint32_t timeout_ms);

/**
 * @brief 获取当前时间（NTP同步后有效）
 * @param[out] time 当前时间结构体
 * @return 0 成功，<0 失败
 */
int32_t iot_ntp_get_time(iot_rtc_time_t *time);

/**
 * @brief 将NTP时间转换为Unix时间戳
 * @param[in] time NTP时间结构体
 * @return Unix时间戳(秒)
 */
uint32_t iot_ntp_to_timestamp(const iot_rtc_time_t *time);

/**
 * @brief 将Unix时间戳转换为NTP时间
 * @param[in] timestamp Unix时间戳(秒)
 * @param[out] time NTP时间结构体
 */
void iot_ntp_from_timestamp(uint32_t timestamp, iot_rtc_time_t *time);

#endif /* IOT_NTP_YOPEN_H */
