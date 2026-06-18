/**
 * @file        cm_rtc.h
 * @brief       RTC管理
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By ljw
 * @date        2021/05/28
 *
 * @defgroup rtc rtc
 * @ingroup SYS
 * @{
 */

#ifndef __CM_RTC_H__
#define __CM_RTC_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Public Types
 ****************************************************************************/

/** RTC定时器ID枚举 */
typedef enum{
    CM_RTC_ID_0,
    CM_RTC_ID_1,
    CM_RTC_ID_2,
    CM_RTC_ID_3,
    CM_RTC_ID_4,
    CM_RTC_ID_END,
}cm_rtc_id_e;

typedef struct {
    int32_t tm_sec;     /*!< 秒 – 取值区间为[0,59] */
    int32_t tm_min;     /*!< 分 - 取值区间为[0,59] */
    int32_t tm_hour;    /*!< 时 - 取值区间为[0,23] */
    int32_t tm_mday;    /*!< 日期 - 取值区间为[1,31] */
    int32_t tm_mon;     /*!< 月份 - 取值区间为[1,12] */
    int32_t tm_year;    /*!< 年份 */
} cm_tm_t;

typedef void (*cm_rtc_alarm_cb)(void);

/**
 * @brief RTC定时器回调函数指针
 *
 * @param [in] arg 入参
 *
 * @return 空
 *
 * @details 创建定时器时传入
 */
typedef void (*cm_rtc_timer_cb)(void * arg);

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * @brief 设置系统时间（秒）
 *
 * @param [in] second 时间戳(单位：秒)
  *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details UTC时间
 */
int32_t cm_rtc_set_current_time(uint64_t second);

/**
 * @brief 设置系统时间（毫秒）
 *
 * @param [in] ms 时间戳(单位：毫秒)
  *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details UTC时间（暂不支持）
 */
int32_t cm_rtc_set_current_time_ms(uint64_t ms);

/**
 * @brief 获取系统时间（秒）
 *
 * @return 时间戳（单位：秒）
 *
 * @details UTC时间
 */
uint64_t cm_rtc_get_current_time(void);

/**
 * @brief 获取系统时间（毫秒）
 *
 * @return 时间戳（单位：毫秒）
 *
 * @details UTC时间（暂不支持）
 */
uint64_t cm_rtc_get_current_time_ms(void);

/**
 * @brief 设置时区
 *
 * @param [in] timezone 时区
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details timezone设置范围为-12~12，-12为西十二区，0为世界时间，12为东十二区
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，推荐使用cm_rtc_set_timezone_v2接口。
 */
int32_t cm_rtc_set_timezone(int32_t timezone);

/**
 * @brief 获取时区
 *
 * @return 时区
 *
 * @details timezone范围为-12~12，-12为西十二区，0为世界时间，12为东十二区
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，推荐使用cm_rtc_get_timezone_v2接口。
 */
int32_t cm_rtc_get_timezone(void);

/**
 * @brief 设置时区
 *
 * @param [in] timezone 时区
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details timezone设置范围为-48~48，时区值单位是15分钟,-48为西十二区，0为世界时间，48为东十二区
 */
int32_t cm_rtc_set_timezone_v2(int32_t timezone);

/**
 * @brief 获取时区
 *
 * @return 时区
 *
 * @details timezone范围为-48~48，时区值单位是15分钟,-48为西十二区，0为世界时间，48为东十二区
 */
int32_t cm_rtc_get_timezone_v2(void);

/**
 * @brief 启动rtc定时器
 *
 * @param [in] rtc_id 定时器id
 * @param [in] timeout 定时时间，单位ms
 * @param [in] cb 定时器回调函数
 * @param [in] data 用户参数
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details rtc定时器可唤醒模组睡眠
 */
int32_t cm_rtc_timer_start(cm_rtc_id_e rtc_id, uint32_t timeout, cm_rtc_timer_cb cb, void *data);

/**
 * @brief 停止rtc定时器
 *
 * @param [in] rtc_id 定时器id
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details rtc定时器可唤醒模组睡眠
 */
int32_t cm_rtc_timer_stop(cm_rtc_id_e rtc_id);

/**
 * @brief 注册闹钟回调函数，闹钟定时到达时触发
 *
 * @param [in] cb 用户回调函数
 *
 * @return None
 *
 * @details 禁止在中断回调中做阻塞业务
 */
void cm_rtc_register_alarm_cb(cm_rtc_alarm_cb cb);

/**
 * @brief 使能/失能闹钟
 *
 * @param [in] on_off true：使能，false：失能
 *
 * @return None
 *
 * @details
 */
void cm_rtc_enable_alarm(bool on_off);

/**
 * @brief 设置闹钟
 *
 * @param [in] a_time 时间
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details
 */
int32_t cm_rtc_set_alarm(cm_tm_t *a_time);

/**
 * @brief 读取闹钟
 *
 * @param [in] a_time 时间
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details
 */
int32_t cm_rtc_get_alarm(cm_tm_t *a_time);

/**
 * @brief 注册闹钟回调函数，闹钟定时到达时触发
 *
 * @param [in] rtc_id 闹钟id
 * @param [in] cb 用户回调函数
 *
 * @return None
 *
 * @details 禁止在中断回调中做阻塞业务,最多支持3个闹钟
 */
void cm_rtc_register_alarm_cb_v2(cm_rtc_id_e rtc_id, cm_rtc_alarm_cb cb);

/**
 * @brief 使能/失能闹钟
 *
 * @param [in] rtc_id 闹钟id
 * @param [in] on_off true：失能，false：失能
 *
 * @return None
 *
 * @details
 */
void cm_rtc_enable_alarm_v2(cm_rtc_id_e rtc_id, bool on_off);

/**
 * @brief 设置闹钟
 *
 * @param [in] rtc_id 闹钟id
 * @param [in] a_time 时间
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details:最大闹钟个数为3个
 */
int32_t cm_rtc_set_alarm_v2(cm_rtc_id_e rtc_id, cm_tm_t *a_time);

/**
 * @brief 读取闹钟
 *
 * @param [in] rtc_id 闹钟id
 * @param [in] a_time 时间
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details
 */
int32_t cm_rtc_get_alarm_v2(cm_rtc_id_e rtc_id, cm_tm_t *a_time);


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_RTC_H__ */

/** @}*/
