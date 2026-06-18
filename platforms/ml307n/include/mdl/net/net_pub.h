/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief 定义net共用的结构，枚举等信息
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */
#ifndef __NET_PUB_H__
#define __NET_PUB_H__

#include "slog_print.h"
#include "net_debug.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup NetApi
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define CID_DEFAULT_NETIF (1)
#define CID_IMS_NETIF (8)

#define NET_CID_MIN (1)
#define NET_CID_MAX (8)
#define NET_CID_COUNT (8)

#define NETIF_MTU_LEN_DEFAULT  (1500)

#if 0
//#define MID_NET_PRINT_INFO(format, ...)   slogPrintf(SLOG_LEVEL_DEBUG, SLOG_PRINT_SUBMDL_MID_NET, format, ##__VA_ARGS__)
#define MID_NET_PRINT_INFO(format, ...)   slogPrintf(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_MID_NET, format, ##__VA_ARGS__)
#define MID_NET_PRINT_ERROR(format, ...)  slogPrintf(SLOG_LEVEL_ERROR, SLOG_PRINT_SUBMDL_MID_NET, format, ##__VA_ARGS__)
//#define MID_NET_PRINT_WARN(format, ...)  slogPrintf(SLOG_LEVEL_WARN, SLOG_PRINT_SUBMDL_MID_NET, format, ##__VA_ARGS__)
#define MID_NET_tcpdump(pData, iLength)  do{if(NET_Debug_TcpdumpFlagGet()) slogDump(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_IP_PACKET, (pData), (iLength));}while(0)
#define MID_NET_tcpdumpims(pData, iLength)  do{if((!NET_Debug_TcpdumpFlagGet()) && (NET_Debug_TcpdumpImsFlagGet())) slogDump(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_IP_PACKET, (pData), (iLength));}while(0)
#else
#define MID_NET_PRINT_INFO(format, ...)   osPrintf(format, ##__VA_ARGS__)
#define MID_NET_PRINT_ERROR(format, ...)  osPrintf(format, ##__VA_ARGS__)
#define MID_NET_tcpdump(pData, iLength)  do{if(NET_Debug_TcpdumpFlagGet()) slogDump(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_IP_PACKET, (pData), (iLength));}while(0)
#define MID_NET_tcpdumpims(pData, iLength)  do{if((!NET_Debug_TcpdumpFlagGet()) && (NET_Debug_TcpdumpImsFlagGet())) slogDump(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_IP_PACKET, (pData), (iLength));}while(0)
#endif

#if 1
#define MID_NAT_PRINT_INFO(format, ...)  slogPrintf(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_MID_NET, format, ##__VA_ARGS__)
#define MID_NAT_PRINT_ERROR(format, ...)  slogPrintf(SLOG_LEVEL_ERROR, SLOG_PRINT_SUBMDL_MID_NET, format, ##__VA_ARGS__)
#define MID_DNS_PRINT_INFO(format, ...)  slogPrintf(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_MID_NET, format, ##__VA_ARGS__)
#else
#define MID_NAT_PRINT_INFO(format, ...)  osPrintf(format, ##__VA_ARGS__)
#define MID_NAT_PRINT_ERROR(format, ...)  osPrintf(format, ##__VA_ARGS__)
#define MID_DNS_PRINT_INFO(format, ...)  osPrintf(format, ##__VA_ARGS__)
#endif

#if 1
#define MID_PPP_PRINT_INFO(format, ...)   slogPrintf(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_MID_NET, format, ##__VA_ARGS__)
#else
#define MID_PPP_PRINT_INFO(format, ...)   osPrintf(format, ##__VA_ARGS__)
#endif

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
//define pdp active type
typedef enum{
    NET_IP_4   = 1,
    NET_IP_6   = 2,
    NET_IP_4_6 = 3,
    NET_IP_MAX,
}NET_IpType;

#ifdef __cplusplus
}
#endif
#endif /* __NET_PUB_H__ */

/** @} */
