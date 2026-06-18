/**
 * @file        cm_ntp.h
 * @brief       NTP协议客户端接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By THF
 * @date        2022/06/02
 *
 * @defgroup ntp ntp
 * @ingroup DS
 * @{
 */

#ifndef __CM_NTP_H__
#define __CM_NTP_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/** NTP事件类型 */
typedef enum
{
    CM_NTP_EVENT_SYNC_FAIL,     /*!< NTP时间同步失败, 参数(event_param): NULL */
    CM_NTP_EVENT_SYNC_OK,       /*!< NTP时间同步成功, 参数(event_param): 获取到的NTP时间, ISO-8601时间格式字符串, 例如: 2021-04-21T17:10:08.868+08:00 */
    CM_NTP_EVENT_SETTIME_FAIL,  /*!< NTP时间同步成功但设置本地时间失败, 参数(event_param): 获取到的NTP时间, ISO-8601时间格式字符串, 例如: 2021-04-21T17:10:08.868+08:00 */
} cm_ntp_event_e;

/** NTP设置类型 */
typedef enum
{
    CM_NTP_CFG_SERVER = 1,              /*!< NTP服务器，支持域名、ipv4地址、ipv6地址，设置参数为字符串型（最多128字节），未设置时使用默认服务器 */
    CM_NTP_CFG_PORT,                    /*!< NTP服务器端口号，端口号为uint16_t型，未设置时使用123端口 */
    CM_NTP_CFG_TIMEOUT,                 /*!< NTP超时时间，未设置时为3000毫秒，支持设置范围1000~10000，单位为毫秒 */
    CM_NTP_CFG_DNS,                     /*!< NTP服务器为域名时DNS解析IP地址优先级，优先级为uint32_t型，0(默认)：使用全局优先级。1：v4优先。2：v6优先 */
    CM_NTP_CFG_SET_RTC,                 /*!< 是否将获取到的NTP时间设置到RTC设置，参数为bool型，0：不同步至系统时间。1（默认）：同步至系统时间 */
    CM_NTP_CFG_CB,                      /*!< NTP服务回调函数设置，回调函数类型见cm_ntp_event_cb */
    CM_NTP_CFG_CB_PARAM,                /*!< NTP服务回调函数中传递的用户参数指针 */
} cm_ntp_cfg_type_e;

/** NTP事件回调函数 */
typedef void (*cm_ntp_event_cb)(cm_ntp_event_e event, void *event_param, void *cb_param);

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
 * @brief NTP服务设置参数
 *
 * @param [in] type  设置参数类型
 * @param [in] value 设置参数数值
 *
 * @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 * @details NTP服务进行中不可设置
 */
int32_t cm_ntp_set_cfg(cm_ntp_cfg_type_e type, void *value);

/**
 * @brief 执行NTP时间同步
 *
 * @return 
 *   = 0  - 成功 \n
 *   < 0  - 错误码
 *
 * @details NTP模块底层依赖asocket、async_dns和eloop模块，使用NTP功能时应先初始化前述模块 \n
 * @details NTP时间同步至系统时间采用cm_rtc_set_date()和cm_rtc_set_time()接口以修改系统时间 \n
 * @details NTP服务获取的为时间戳信息，不包括时区信息，回调通知中的时区信息通过cm_rtc_get_timezone()获取 \n
 * @details 用户未通过cm_rtc_set_timezone()设置时区时，cm_ntp_sync()回调通知中时区不准确，不应使用。
 */
int32_t cm_ntp_sync(void);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_NTP_H__ */

/** @}*/
