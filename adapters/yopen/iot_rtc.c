/**
 * @file iot_rtc.c
 * @brief YOPEN 平台 RTC 适配器实现
 * @details 基于 yopen_rtc 接口实现 RTC 功能封装，
 *          支持跨平台编译。
 */

#include "iot_rtc.h"

/* ============================================================
 * RTC 操作函数实现
 * ============================================================ */

/**
 * @brief 初始化RTC
 * @return 0 成功
 */
iot_err_t iot_rtc_init(void)
{
    return yopen_rtc_init();
}

/**
 * @brief 去初始化RTC
 * @return 0 成功
 */
iot_err_t iot_rtc_deinit(void)
{
    return yopen_rtc_deinit();
}

/**
 * @brief 获取RTC时间（秒）
 * @return 时间(秒)
 */
uint32_t iot_rtc_get_time(void)
{
    return yopen_rtc_get_time();
}

/**
 * @brief 设置RTC时间（秒）
 * @param[in] sec 时间(秒)
 * @return 0 成功
 */
iot_err_t iot_rtc_set_time(uint32_t sec)
{
    return yopen_rtc_set_time(sec);
}

/**
 * @brief 获取RTC日期时间
 * @param[out] dt 日期时间结构体
 * @return 0 成功
 */
iot_err_t iot_rtc_get_datetime(iot_rtc_time_t *dt)
{
    yopen_rtc_time_s rtc_time = {0};
    int ret;
    
    if (dt == NULL) {
        return IOT_ERR_PARAM;
    }
    
    ret = yopen_rtc_get_datetime(&rtc_time);
    if (ret != 0) {
        return ret;
    }
    
    /* 转换时间结构体 */
    dt->year = rtc_time.year;
    dt->month = rtc_time.month;
    dt->day = rtc_time.day;
    dt->hour = rtc_time.hour;
    dt->minute = rtc_time.minute;
    dt->second = rtc_time.second;
    dt->weekday = rtc_time.weekday;
    
    return IOT_OK;
}

/**
 * @brief 设置RTC日期时间
 * @param[in] dt 日期时间结构体
 * @return 0 成功
 */
iot_err_t iot_rtc_set_datetime(const iot_rtc_time_t *dt)
{
    yopen_rtc_time_s rtc_time = {0};
    
    if (dt == NULL) {
        return IOT_ERR_PARAM;
    }
    
    /* 转换时间结构体 */
    rtc_time.year = dt->year;
    rtc_time.month = dt->month;
    rtc_time.day = dt->day;
    rtc_time.hour = dt->hour;
    rtc_time.minute = dt->minute;
    rtc_time.second = dt->second;
    rtc_time.weekday = dt->weekday;
    
    return yopen_rtc_set_datetime(&rtc_time);
}

/**
 * @brief 设置时区
 * @param[in] zone 时区
 * @return 0 成功
 */
iot_err_t iot_rtc_set_timezone(int8_t zone)
{
    return yopen_rtc_set_timezone(zone);
}

/**
 * @brief 获取时区
 * @return 时区
 */
int8_t iot_rtc_get_timezone(void)
{
    return yopen_rtc_get_timezone();
}

/**
 * @brief 注册RTC闹钟
 * @param[in] sec 闹钟时间(秒)
 * @param[in] arg 回调参数
 * @param[in] cb 闹钟回调函数
 * @return 闹钟ID
 */
int iot_rtc_register_alarm(uint32_t sec, void *arg, iot_rtc_alarm_cb_t cb)
{
    return yopen_rtc_register_alarm(sec, arg, (yopen_rtc_alarm_cb_t)cb);
}

/**
 * @brief 取消RTC闹钟
 * @param[in] id 闹钟ID
 * @return 0 成功
 */
iot_err_t iot_rtc_unregister_alarm(int id)
{
    return yopen_rtc_unregister_alarm(id);
}