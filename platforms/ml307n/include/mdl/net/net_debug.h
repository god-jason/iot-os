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

#ifndef __NET_DEBUG_H__
#define __NET_DEBUG_H__


#include "net_debug.h"


#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/************************************************************************************
 *                                 变量声明
 ************************************************************************************/



/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void NET_Debug_TcpdumpFlagSet(uint8_t cfg);
uint8_t NET_Debug_TcpdumpFlagGet(void);
void NET_Debug_TcpdumpImsFlagSet(uint8_t cfg);
uint8_t NET_Debug_TcpdumpImsFlagGet(void);

#ifdef OS_USING_FAST_RELEASE_RRC
void NET_FastReleaseRrcTimeSet(uint32_t time);
uint32_t NET_FastReleaseRrcTimeGet(void);
void NET_FastReleaseRrcTickClean(void);
void NET_FastReleaseRrcFlagSet(void);
int32_t NET_FastReleaseRrcTimerStart(void);
void NET_FastReleaseRrcTimerStop(void);
#endif

#ifdef __cplusplus
}
#endif
#endif

