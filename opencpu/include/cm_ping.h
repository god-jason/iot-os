/**
 * @file        cm_ping.h
 * @brief       ping 接口
 * @copyright   Copyright © 2024 China Mobile IOT. All rights reserved.
 * @author      By SMR
 * @date        2024/3/25
 *
 * @defgroup ping
 * @ingroup ping
 * @{
 */

#ifndef __CM_PING_H__
#define __CM_PING_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Public Types
 ****************************************************************************/

/* ping配置参数 */
typedef struct
{
    uint8_t   cid;          /*!< PDP cid 若为0则会取默认cid*/
    char *    host;         /*!< 地址 长度需小于250字节*/
    uint16_t  timeout;      /*!< PING超时时间(s) 范围1-60*/
    uint16_t  packet_len;   /*!< PING包长度 范围1-1400*/
    uint16_t  ping_num;     /*!< PING次数 范围1-65535*/
} cm_ping_cfg_t;

/* 回调函数上报的ping结果数据 */
typedef struct
{
    char *    ip;           /*!< ip地址*/
    uint32_t  packet_len;   /*!< ping包大小*/
    uint16_t  send_cnt;     /*!< 发送ping包次数*/
    uint32_t  recv_cnt;     /*!< 已收到ping包次数*/
    uint32_t  lost_cnt;     /*!< 丢包次数*/
    uint8_t   ttl;          /*!< 单次ping包路由跳转次数*/
    uint32_t  time;         /*!< 单次ping包响应时长(ms)*/
    uint32_t  rtt_min;      /*!< 最小响应时间(ms)*/
    uint32_t  rtt_max;      /*!< 最大响应时间(ms)*/
    uint32_t  rtt_avg;      /*!< 平均响应时间(ms)*/
} cm_ping_reply_t;

/* 回调函数上报的结果类型 */
typedef enum
{
    CM_PING_CB_TYPE_PING_ONCE,  /*!< 单次ping结果*/
    CM_PING_CB_TYPE_PING_TOTAL, /*!< 总的统计结果*/
    CM_PING_CB_TYPE_PING_ERROR, /*!< 错误反馈*/
} cm_ping_cb_type_e;

/* 回调函数上报的单次ping执行结果 */
typedef enum
{
    CM_PING_RESULT_SUCC,            /*!< 成功*/
    CM_PING_RESULT_DNS_FAIL,        /*!< DNS失败*/
    CM_PING_RESULT_DNS_TIME_OUT,    /*!< DNS超时*/
    CM_PING_RESULT_PING_FAIL,       /*!< ping响应错误*/
    CM_PING_RESULT_PING_TIME_OUT,   /*!< ping响应超时*/
    CM_PING_RESULT_UNKNOWN_ERR,     /*!< 其他错误*/
} cm_ping_cb_result_e;

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
 * @brief ping执行结果回调函数
 *
 * @param [out] type        上报类型
 * @param [out] result      单次执行结果
 * @param [out] resp        执行结果数据
 * @param [out] user_data   用户参数
 *
 * @details 当type为CM_PING_CB_TYPE_PING_ONCE时，上报单次的结果数据，可查看resp中的time和ttl; \n
 *          当type为CM_PING_CB_TYPE_PING_TOTAL时，上报总的统计结果，可查看resp中的send_cnt、recv_cnt、lost_cnt、rtt_min、rtt_max和rtt_avg; \n
 *          当type为CM_PING_CB_TYPE_PING_ERROR时，说明执行出现错误，可查看result获取执行结果; \n
 *          当result为CM_PING_RESULT_PING_FAIL或者CM_PING_RESULT_PING_TIME_OUT时，说明单包接收失败或者超时，此时若还有包还没发会继续发送
 */
typedef void (*cm_ping_callback_func)(cm_ping_cb_type_e type, cm_ping_cb_result_e result, cm_ping_reply_t *resp, void *user_data);

/**
 * @brief ping配置参数和回调函数
 *
 * @param [in] cfg  配置参数
 * @param [in] cb   回调函数
 *
 * @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 * @details 回调函数不可为NULL
 */
int32_t cm_ping_init(cm_ping_cfg_t *cfg, cm_ping_callback_func cb);

/**
 * @brief 执行ping操作
 *
 * @param [in] user_data  用户参数
 *
 * @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *  
 *  @details 非阻塞方式，ping最终执行结果需要等待回调函数上报。 \n
 *           user_data将在回调函数中返回，用以区别回调函数上报的是哪一次的结果，函数内仅保存指针，若用户申请了内存空间请自行维护
 */
int32_t cm_ping_start(void *user_data);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif

/** @}*/

