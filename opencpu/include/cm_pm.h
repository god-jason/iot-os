/**
 * @file        cm_pm.h
 * @brief       PM接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By thf
 * @date        2021/04/18
 *
 * @defgroup pm pm
 * @ingroup PM
 * @{
 */
/********************************************************************************************************
 *******************************************IOMUX使用注意事项*************************************
 * 1、回调函数中不可执行耗时任务和其它重进入函数，比如打印等;
 * 2、如果使用reset键重启，由于是硬件直接重启，因此获取到的结果为最新软重启原因；
 *********************************************************************************************************/

#ifndef __CMO_PM_H__
#define __CMO_PM_H__


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
 typedef enum
{
    CM_POWERKEY_EVENT_RELEASE = 0,         /*!<按键被释放*/
    CM_POWERKEY_EVENT_PRESS = 1,           /*!<按键被按下*/
} cm_powerkey_event_e;
  
typedef void (*cm_pm_event_cb_t)(void);//回调函数中不可执行耗时任务和其它重进入函数
/* reason
typedef enum {
    PSM_ACTIVE = 0,
    PSM_IDLE,
    PSM_SW_IDLE,
    PSM_LIGHT_SLEEP,
    PSM_DEEP_SLEEP,
    PSM_STANDBY,
    PSM_IGNORE,
    PSM_MAX_IDLESTATE,
} PSM_Mode;
*/
typedef void (*cm_pm_exit_cb_t)(uint32_t reason);//回调函数中不可执行耗时任务和其它重进入函数
typedef void (*cm_pm_powerkey_cb_t)(cm_powerkey_event_e event);//回调函数中不可执行耗时任务和其它重进入函数

/** 上电原因 */
typedef enum
{
    CM_PM_UNKNOWN = 0,       /*!<未知原因*/
    CM_PM_CHARG_POW_OFF,     /*!<Charging in powering off,USB等充电方式供电时产生的重启,不支持*/
    CM_PM_RD_PRO_MODE     ,  /*!<Rd production mode,产线工具下发导致的重启*/
    CM_PM_RTC_ALARM,         /*!<rtc Alarm，定时闹钟开机*/
    CM_PM_POWER_ON,          /*!<Normal power on,正常重启*/
    CM_PM_ERROR_RESET,       /*!<Error reset (WDT),看门狗超时导致的重启*/
}cm_pm_power_on_reason_e;

/** 配置 */
typedef struct{
    cm_pm_event_cb_t cb_enter;  /*!< 进入低功耗回调函数*/
    cm_pm_exit_cb_t cb_exit;   /*!< 退出低功耗回调函数*/
} cm_pm_cfg_t;

extern cm_pm_cfg_t pmcfg;

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
 * @brief 关机
 *  
 * @return 空
 *  
 * @details More details
 */
void cm_pm_poweroff(void);
/**
 * @brief 获取上电原因
 * 
 * @return 
 *   >= 0  - 上电原因\n
 *   < 0  - 失败, 返回值为错误码
 *  
 * @details 如果使用reset键重启，由于是硬件直接重启，因此获取到的结果为最新软重启原因
 */
int cm_pm_get_power_on_reason(void);


/**
 * @brief 重启
 * 
 * @param [in] void
 * 
 * @return void
 *  
 * @details More details
 */
void cm_pm_reboot(void);

/**
 * @brief pm初始化
 * 
 * @param [in] pm_cfg:参数配置
 * 
 * @return void
 *  
 * @details More details
 */
void cm_pm_init(cm_pm_cfg_t pm_cfg);


/**
 * @brief 睡眠模式上锁
 *  
 * @return 空
 *  
 * @details 执行后，模组将无法进入休眠状态
 */
void cm_pm_work_lock(void);

/**
 * @brief 睡眠模式解锁
 *  
 * @return 空
 *  
 * @details 执行后，模组将允许进入休眠状态
 */

void cm_pm_work_unlock(void);

/**
 * @brief powerkey按键回调函数注册
 *  
 * @param [in]  callback回调函数指针
 * @return
 *  = 0  - 成功 \n
 *  < 0  - 失败, 返回值为错误码
 *  
 * @details 如果回调函数非空，无论长短按都将调用回调函数，且不会关机，仅做按键功能，如果为空，将长按为关机功能\n
 */
int32_t cm_pm_powerkey_regist_callback(cm_pm_powerkey_cb_t callback);


#undef EXTERN
#ifdef __cplusplus
}
#endif


#endif /* __CM_PM_H__ */

/** @}*/
