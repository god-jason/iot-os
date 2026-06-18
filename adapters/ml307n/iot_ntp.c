/**
 * @file iot_ntp.c
 * @brief ML307N 平台 NTP 适配器实现
 * @details 基于 cm_ntp 接口实现 NTP 功能封装，
 *          提供统一的网络时间同步函数。
 */

#include "iot_ntp.h"

/**
 * @brief 设置NTP同步服务器地址
 * @param[in] server 服务器地址枚举值
 * @param[in] host   自定义服务器地址字符串（server为CM_NTP_SERVER_CUSTOM时使用）
 * @param[in] port   NTP端口号（默认为123）
 * @return 0 成功，-1 失败
 */
int32_t iot_ntp_set_server(iot_ntp_server_addr_t server, const char *host, uint16_t port)
{
    return cm_ntp_set_server((cm_ntp_server_addr_e)server, (char *)host, port);
}

/**
 * @brief 使能NTP同步
 * @return 0 成功，-1 失败
 */
int32_t iot_ntp_enable(void)
{
    return cm_ntp_enable();
}

/**
 * @brief 禁止NTP同步
 * @return 0 成功，-1 失败
 */
int32_t iot_ntp_disable(void)
{
    return cm_ntp_disable();
}

/**
 * @brief 获取本地时间（毫秒级）
 * @param[out] buf     时间缓冲区（>=10字节）
 * @param[in]  buf_len 缓冲区长度
 * @return >=0 获取到的时间字符串长度，<0 失败
 */
int32_t iot_ntp_get_local_time_ms(char *buf, uint32_t buf_len)
{
    return cm_ntp_get_local_time_ms(buf, buf_len);
}

/**
 * @brief 获取网络时间
 * @param[out] buf     时间缓冲区（>=26字节）
 * @param[in]  buf_len 缓冲区长度
 * @return >=0 获取到的时间字符串长度，<0 失败
 */
int32_t iot_ntp_get_network_time(char *buf, uint32_t buf_len)
{
    return cm_ntp_get_network_time(buf, buf_len);
}

/**
 * @brief 设置NTP时区（分钟）
 * @param[in] tz_offset 时区偏移量（分钟），范围[-720, 840]
 * @return 0 成功，-1 失败
 */
int32_t iot_ntp_set_timezone(int16_t tz_offset)
{
    return cm_ntp_set_time_zone(tz_offset);
}

/**
 * @brief 获取NTP时区（分钟）
 * @param[out] tz_offset 时区偏移量指针
 * @return 0 成功，-1 失败
 */
int32_t iot_ntp_get_timezone(int16_t *tz_offset)
{
    return cm_ntp_get_time_zone(tz_offset);
}

/**
 * @brief 查询NTP功能使能状态
 * @param[out] status 使能状态指针（1使能/0禁止）
 * @return 0 成功，-1 失败
 */
int32_t iot_ntp_get_enable_status(uint8_t *status)
{
    return cm_ntp_get_enable_status(status);
}