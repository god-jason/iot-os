/**
 * @file iot_rtc.h
 * @brief YOPEN 平台 RTC 适配器头文件
 * @details 基于 yopen_rtc 接口实现 RTC 功能封装，
 *          定义统一的 RTC 类型，支持跨平台编译。
 */

#ifndef IOT_RTC_YOPEN_H
#define IOT_RTC_YOPEN_H

#include "../iot_common.h"
#include "yopen_rtc.h"

/* ============================================================
 * 统一 RTC 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief RTC时间格式枚举（只有两个值，保持原样） */
typedef enum {
    IOT_RTC_FORMAT_BIN = 0,           /**< 二进制格式 */
    IOT_RTC_FORMAT_BCD = 1,           /**< BCD格式 */
} iot_rtc_format_t;

/** @brief RTC时间结构体 */
typedef struct {
    uint16_t year;                   /**< 年份 */
    uint8_t month;                   /**< 月份(1-12) */
    uint8_t day;                     /**< 日期(1-31) */
    uint8_t hour;                    /**< 小时(0-23) */
    uint8_t minute;                   /**< 分钟(0-59) */
    uint8_t second;                   /**< 秒(0-59) */
    uint8_t weekday;                 /**< 星期(0-6, 0=周日) */
} iot_rtc_time_t;

/* ============================================================
 * RTC 操作宏定义
 * ============================================================ */

/**
 * @brief 初始化RTC
 * @return 0 成功
 */
#define iot_rtc_init() \
    yopen_rtc_init()

/**
 * @brief 去初始化RTC
 */
#define iot_rtc_deinit() \
    yopen_rtc_deinit()

/**
 * @brief 获取RTC时间（秒）
 * @return 时间(秒)
 */
#define iot_rtc_get_time() \
    yopen_rtc_get_time()

/**
 * @brief 设置RTC时间（秒）
 * @param[in] sec 时间(秒)
 */
#define iot_rtc_set_time(sec) \
    yopen_rtc_set_time(sec)

/**
 * @brief 获取RTC日期时间
 * @param[out] dt 日期时间结构体
 * @return 0 成功
 */
#define iot_rtc_get_datetime(dt) \
    yopen_rtc_get_datetime((yopen_rtc_time_t *)(dt))

/**
 * @brief 设置RTC日期时间
 * @param[in] dt 日期时间结构体
 * @return 0 成功
 */
#define iot_rtc_set_datetime(dt) \
    yopen_rtc_set_datetime(*(yopen_rtc_time_t *)(dt))

/**
 * @brief 设置时区
 * @param[in] zone 时区
 */
#define iot_rtc_set_timezone(zone) \
    yopen_rtc_set_timezone(zone)

/**
 * @brief 获取时区
 * @return 时区
 */
#define iot_rtc_get_timezone() \
    yopen_rtc_get_timezone()

#endif /* IOT_RTC_YOPEN_H */