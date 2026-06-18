/**
 * @file iot_ntp.h
 * @brief ML307N 平台 NTP(网络时间协议)适配器头文件
 * @details 封装 ML307N 平台 cm_ntp 接口，提供统一的网络时间同步宏定义。
 *          支持设置NTP服务器、同步网络时间、设置时区等功能。
 */

#ifndef IOT_NTP_ML307N_H
#define IOT_NTP_ML307N_H

#include "../../iot_types.h"
#include "cm_ntp.h"

/** @brief NTP服务器地址枚举重定义 */
#define iot_ntp_server_addr_t   cm_ntp_server_addr_e

/**
 * @brief 设置NTP同步服务器地址
 * @param[in] server 服务器地址枚举值
 * @param[in] host   自定义服务器地址字符串（server为CM_NTP_SERVER_CUSTOM时使用）
 * @param[in] port   NTP端口号（默认为123）
 * @return 0 成功，-1 失败
 * @note 仅支持地址长度为127以内，端口号0-65535
 */
#define iot_ntp_set_server(server, host, port) \
    cm_ntp_set_server((cm_ntp_server_addr_e)(server), (char *)(host), (uint16_t)(port))

/**
 * @brief 使能NTP同步
 * @return 0 成功，-1 失败
 */
#define iot_ntp_enable() \
    cm_ntp_enable()

/**
 * @brief 禁止NTP同步
 * @return 0 成功，-1 失败
 */
#define iot_ntp_disable() \
    cm_ntp_disable()

/**
 * @brief 获取本地时间（毫秒级）
 * @param[out] buf     时间缓冲区（>=10字节）
 * @param[in]  buf_len 缓冲区长度
 * @return >=0 获取到的时间字符串长度，<0 失败
 * @note 与网络时间（ms）存在秒级偏差，建议配合 iot_rtc_get_time 使用
 */
#define iot_ntp_get_local_time_ms(buf, buf_len) \
    cm_ntp_get_local_time_ms((char *)(buf), (uint32_t)(buf_len))

/**
 * @brief 获取网络时间
 * @param[out] buf     时间缓冲区（>=26字节）
 * @param[in]  buf_len 缓冲区长度
 * @return >=0 获取到的时间字符串长度，<0 失败
 * @note 时间格式：YYYY/MM/DD,hh:mm:ss，网络时间以ms为单位的时区补偿偏差
 */
#define iot_ntp_get_network_time(buf, buf_len) \
    cm_ntp_get_network_time((char *)(buf), (uint32_t)(buf_len))

/**
 * @brief 设置NTP时区（分钟）
 * @param[in] tz_offset 时区偏移量（分钟），范围[-720, 840]
 * @return 0 成功，-1 失败
 * @note 例如东八区为480
 */
#define iot_ntp_set_timezone(tz_offset) \
    cm_ntp_set_time_zone((int16_t)(tz_offset))

/**
 * @brief 获取NTP时区（分钟）
 * @param[out] tz_offset 时区偏移量指针
 * @return 0 成功，-1 失败
 */
#define iot_ntp_get_timezone(tz_offset) \
    cm_ntp_get_time_zone((int16_t *)(tz_offset))

/**
 * @brief 查询NTP功能使能状态
 * @param[out] status 使能状态指针（1使能/0禁止）
 * @return 0 成功，-1 失败
 */
#define iot_ntp_get_enable_status(status) \
    cm_ntp_get_enable_status((uint8_t *)(status))

#endif /* IOT_NTP_ML307N_H */
