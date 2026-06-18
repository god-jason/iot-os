/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */

#ifndef __LWIP_PPPMGR_H__
#define __LWIP_PPPMGR_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "lwip/pbuf.h"
#include "lwip/netif.h"
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/


/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
int32_t PPP_Mgr_AT_DataStart(uint8_t channelId, uint8_t cid);
int32_t PPP_Mgr_AT_DataRecv(uint8_t channelId, uint8_t eventId, char* buf, uint32_t bufLen);
void PPP_Mgr_AT_DataStop(void);
void PPP_IP_Input(struct pbuf *pin, struct netif *inp);
void PPP_IP_Dump(uint8_t *dump, uint16_t dumpLen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LWIP_PPPMGR_H__ */
