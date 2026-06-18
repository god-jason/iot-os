/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_wdt.h
 *
 * @brief       watchdog驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef __DRV_WDT_H_
#define __DRV_WDT_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include "drv_common.h"
#if defined(OS_USING_PM) 
#include "psm_common.h"
#endif

/**
 * @addtogroup Wdt
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define WDT_CMP_VALUE       (2)
#define WDT_MIN_FEED_TIME   (3 + WDT_CMP_VALUE)

#define WDT_AP_TIMEOUT_FLAG 0xAB
#define WDT_CP_TIMEOUT_FLAG 0xCD

#define DRV_EXCEPTION_RESET_ON  0x6D
#define DRV_EXCEPTION_RESET_OFF 0xB2

#define WDT_DEFAULT_FEED_TIME_SECOND        1
#define WDT_CP_DEFAULT_TIMEOUT_TIME_SECOND  10

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef struct
{
    uint32_t    feedDogPeriod;      /* 喂狗周期 */
    uint32_t    timeOutPeriod;      /* 超时周期 */
}WDT_Handle;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           硬件看门狗启动.
 *
 * @param[in]       timeOutPeriod       超时周期，单位毫秒, WDT_TIMER_PERIOD_100MS 的整数倍
 * @param[in]       feedDogPeriod       喂狗周期，单位毫秒, WDT_TIMER_PERIOD_100MS 的整数倍
 * 
 * @return          申请返回值.
 * @retval          0                   成功
 *                  <0                  失败
 ************************************************************************************
 */
uint8_t HardWDT_Initlize(uint32_t timeOutPeriod, uint32_t feedDogPeriod);

/**
 ************************************************************************************
 * @brief           硬件看门狗关闭.
 *
 * @param[in]       void
 * 
 * @return          void.
 ************************************************************************************
 */
void HardWDT_UnInitlize(void);
/**
 ************************************************************************************
 * @brief           看门狗停止.
 *
 * @param[in]       handle              看门狗操作句柄
 * 
 * @return          void.
 ************************************************************************************
 */
void HardWDT_Abort(void);

/**
 ************************************************************************************
 * @brief           看门狗喂狗.
 *
 * @param[in]       handle              看门狗操作句柄
 * 
 * @return          void.
 ************************************************************************************
 */
void HardWDT_FeedDog(void);

/**
 ************************************************************************************
 * @brief           是否开放Watchdog功能给客户使用.
 *
 * @param[in]       
 * 
 * @return          bool
 * @note            开放给客户使用后，需要客户维护watchdog的喂狗动作
 ************************************************************************************
 */
bool HardWDT_IsOpen(void);

#if defined(_CPU_AP)
/**
 ************************************************************************************
 * @brief           软件看门狗.
 *
 * @param[in]       monitor_task              监测的任务，用于超时打印任务信息（可传NULL）
 * 
 * @return          void*.                    软件看门狗句柄
 ************************************************************************************
 */
void *SoftWdt_Create(void *monitor_task);
int SoftWdt_Start(void *wdt, uint32_t ms);
void SoftWdt_Stop(void *wdt);
void SoftWdt_Feed(void *wdt, uint32_t ms);
void SoftWdt_Delete(void *wdt);
#endif

// 打印看门狗的寄存器
void WDT_Monitor(void *parameter);

uint16_t HardWDT_GetConfig(void);
void HardWDT_SetConfig(uint16_t val);
uint8_t HardWDT_GetTimeoutFlag(void);
void HardWDT_SetTimeoutFlag(uint8_t val);
uint8_t HardWDT_GetExceptionResetFlag(void);
void HardWDT_SetExceptionResetFlag(uint8_t val);

#if defined(OS_USING_PM) 
/**
 ************************************************************************************
 * @brief           省电使用看门狗开始.代码放在IRAM
 *
 * @param[in]       timeOutPeriod       超时周期，单位毫秒
 * 
 * @return          void.
 ************************************************************************************
 */
void SECTION_IRAM_FUNC HardWDT_PsmStart(uint32_t timeOutPeriod);

/**
 ************************************************************************************
 * @brief           省电使用看门狗停止
 *
 * @param[in]       void
 * 
 * @return          void.
 ************************************************************************************
 */
void  HardWDT_PsmStop(void);
#endif

#endif
/** @} */
