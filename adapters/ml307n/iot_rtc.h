/**
 * @file iot_rtc.h
 * @brief ML307N 平台 RTC 适配器头文件
 * @details 基于 cm_rtc 接口实现 RTC 功能封装，
 *          定义统一的 RTC 类型，支持跨平台编译。
 */

#ifndef IOT_RTC_ML307N_H
#define IOT_RTC_ML307N_H

#include "cm_rtc.h"
#include "../../iot_types.h"

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

/** @brief RTC闹钟回调函数类型 */
typedef void (*iot_rtc_alarm_cb_t)(void *arg);

/* ============================================================
 * RTC 操作函数声明
 * ============================================================ */

/**
 * @brief 初始化RTC
 * @return 0 成功
 */
int iot_rtc_init(void);

/**
 * @brief 去初始化RTC
 * @return 0 成功
 */
int iot_rtc_deinit(void);

/**
 * @brief 获取RTC时间（秒）
 * @return 时间(秒)
 */
uint32_t iot_rtc_get_time(void);

/**
 * @brief 设置RTC时间（秒）
 * @param[in] sec 时间(秒)
 * @return 0 成功
 */
int iot_rtc_set_time(uint32_t sec);

/**
 * @brief 获取RTC日期时间
 * @param[out] dt 日期时间结构体
 * @return 0 成功
 */
int iot_rtc_get_datetime(iot_rtc_time_t *dt);

/**
 * @brief 设置RTC日期时间
 * @param[in] dt 日期时间结构体
 * @return 0 成功
 */
int iot_rtc_set_datetime(const iot_rtc_time_t *dt);

/**
 * @brief 设置时区
 * @param[in] zone 时区
 * @return 0 成功
 */
int iot_rtc_set_timezone(int8_t zone);

/**
 * @brief 获取时区
 * @return 时区
 */
int8_t iot_rtc_get_timezone(void);

/**
 * @brief 注册RTC闹钟
 * @param[in] sec 闹钟时间(秒)
 * @param[in] arg 回调参数
 * @param[in] cb 闹钟回调函数
 * @return 闹钟ID
 */
int iot_rtc_register_alarm(uint32_t sec, void *arg, iot_rtc_alarm_cb_t cb);

/**
 * @brief 取消RTC闹钟
 * @param[in] id 闹钟ID
 * @return 0 成功
 */
int iot_rtc_unregister_alarm(int id);

#endif /* IOT_RTC_ML307N_H */