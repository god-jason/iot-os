/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      lwping.h
 *
 * @brief     ping 功能实现.
 *
 * @revision  1.0
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version   <th>Author     <th>Description
 * <tr><td>2023-04-21  <td>1.0       <td>ict        <td>初始版本
 * </table>
 ************************************************************************************
 */
/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#ifndef _LWPING_H_
#define _LWPING_H_

#include <os.h>
#include <lwip/ip.h>

#define LWPING_NAME_LEN_MAX         128
#define GETHOST_TIMEOUT             10

typedef struct
{
    struct osTimer *ackTimer;
    char pingHost[LWPING_NAME_LEN_MAX];
    ip_addr_t hostIpAddr;
    uint8_t ExecFlag; /* lwping 执行标志. 0: 停止; 1: 开始 */
    uint8_t pingCid;
    uint8_t channelId;
    uint8_t dnsParseOk;
    uint16_t lastId; /* 本地字节序 */
    uint16_t lastSeqno; /* 本地字节序 */
    uint32_t pingCnt;
    uint32_t timeOut;
    uint32_t packetSize;
    uint32_t interval;
    uint32_t lastRtt;
    uint32_t lastTtl;
    uint32_t lastSendTick;
    uint32_t minRtt;
    uint32_t maxRtt;
    uint32_t sumRtt;
    uint32_t totalAckCnt;
    uint32_t totalReqCnt;
}LWPING_Param;

typedef enum
{
    LWPING_CB_EVENT_REPLY_OK            = 0,      /* lwping 每ping一次应答 OK */
    LWPING_CB_EVENT_DNS_FAIL            = 1,      /* lwping DNS 解析失败 */
    LWPING_CB_EVENT_DNS_TIMEOUT         = 2,      /* lwping DNS 解析超时 */
    LWPING_CB_EVENT_REPLY_ERROR         = 3,      /* lwping 每ping一次应答错误 */
    LWPING_CB_EVENT_REPLY_TIMEOUT       = 4,      /* lwping 每ping一次应答超时 */
    LWPING_CB_EVENT_UNKNOWN             = 5,      /* lwping 未知错误 */
    LWPING_CB_EVENT_MEM_FAIL            = 6,      /* lwping 内存申请失败 */
    LWPING_CB_EVENT_CONN_FAIL           = 7,      /* lwping 连接失败 */
    LWPING_CB_EVENT_FINAL_RESULT        = 8,      /* lwping 最终统计结果 */
}LWPING_CallbackEvent;        /* lwping 回调的枚举事件 */

typedef enum
{
    LWPING_EXEC_FLAG_STOP             = 0,      /* lwping 停止 */
    LWPING_EXEC_FLAG_START            = 1,      /* lwping 开始 */
}LWPING_ExecFlag;     /* lwping 执行标志 */

typedef void (* lwpingEventcb)(uint16_t event, LWPING_Param *pingParam);


/**
 ************************************************************************************
 * @brief          注册 PING 响应的回调函数
 *
 * @param[in]       callback       回调函数指针
 *
 * @return          void
 ************************************************************************************
 */
void LWPING_RegistCallback(lwpingEventcb callback);

/**
 ************************************************************************************
 * @brief          注销 PING 响应的回调函数
 *
 * @param[in]       void
 *
 * @return          void
 ************************************************************************************
 */
void LWPING_UnregistCallback(void);

/**
 ************************************************************************************
 * @brief          启动Ping 功能，不支持同时多次ping
 *
 * @param[in]       pingHost                ping 主机IP或域名
 * @param[in]       pingCnt                 ping次数
 * @param[in]       timeOut                 超时时间
 * @param[in]       pingPacketSize          ping 包大小
 * @param[in]       usedCid                 cid
 *
 * @return          初始化返回值.
 * @retval          >=0                     启动成功
 *                  < 0                     错误
 ************************************************************************************
 */
int8_t LWPING_StartPing(const char *pingHost, uint32_t pingCnt, uint32_t timeOut, uint32_t pingPacketSize, uint32_t interval, uint8_t usedCid, uint8_t channelId);

/**
 ************************************************************************************
 * @brief          停止Ping 功能
 *
 * @param[in]       void
 *
 * @return          void
 ************************************************************************************
 */
void LWPING_StopPing(void);

#endif
