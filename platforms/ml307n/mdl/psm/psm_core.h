/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        psm_core.h
 *
 * @brief       PSM驱动代码
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team          创建
 ************************************************************************************
 */
#ifndef __PSM_CORE__
#define __PSM_CORE__

/************************************************************************************
 *                                 头文件定义
 ************************************************************************************/
#include <os_def.h>

/**
 * @addtogroup Psm
 */

/**@{*/
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define PSM_SLEEPTIME_INV       (0xFFFFFFFFFFFFFFFF)
#define PSM_SLEEPTIME_MAX       (0xFFFFFFFFFFFFFFFE)
#define PSM_SLEEPTIME_REL       (0xFFFFFFFFFFFFFFFD)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/**
\brief: psm status.
*/
typedef enum {
    PSM_ACTIVE = 0,
    PSM_IDLE,
    PSM_MDM_IDLE,
    PSM_SW_IDLE,
    PSM_LIGHT_SLEEP = PSM_SW_IDLE,
    PSM_DEEP_SLEEP,
    PSM_STANDBY,
    PSM_IGNORE,
    PSM_MAX_IDLESTATE,
} PSM_Mode;

/**
 ************************************************************************************
 *\brief: Idle线程休眠入口检查回调函数
 * PsmGetSleepMode: 获取休眠模式和时长
 *                  mode: 休眠模式，不关心当前休眠模式时返回 PSM_IGNORE，
 *                                  禁止休眠时返回PSM_IDLE
 *                                  允许DEEPSLEEP 休眠返回 PSM_DEEP_SLEEP
 *                                  允许STANDBY 休眠返回 PSM_STANDBY
 *                  sleeptime: 休眠时长（ms），无休眠时长时返回 PSM_SLEEPTIME_MAX
 *                  该接口在休眠入口处调用，判断是否可以休眠
 * PsmSleepCheckNoIrq: 依据当前休眠模式判断是否依旧满足休眠条件，禁止阻塞
 *                   mode: 当前休眠模式
 *                   sleeptime: 依据当前休眠模式更新sleeptime
 *                   retval：DRV_OK：满足休眠条件，DRV_ERR：不满足休眠条件
 * PsmGetSleepMode 接口必须实现，在休眠入口处调用，判断是否可以休眠，
 * 如果实现了PsmSleepCheckNoIrq 接口，在关中断休眠检查时调用该接口
 * 如果没有实现PsmSleepCheckNoIrq 接口，在关中断休眠检查时再次调用PsmGetSleepMode接口检查
 * 在PsmGetSleepMode 流程执行时间较长时，建议实现PsmSleepCheckNoIrq接口，以优化省电流程，减少休眠时长
 ************************************************************************************
*/
typedef struct {
    void (*PsmGetSleepMode)(PSM_Mode *mode, uint64_t *sleeptime);
    int (*PsmSleepCheckNoIrq)(PSM_Mode mode, uint64_t *sleeptime);
} PSM_IdleCallback;

/**
 ************************************************************************************
 *\brief: 低功耗回调接口集: PsmSuspendNoirq 接口必须实现
 * PsmSuspendNoirq：休眠流程的回调函数，关中断后调用，中断保护，禁止调用mutex等阻塞接口
 * PsmResumeNoirq：唤醒流程的回调函数，开中断后调用，中断保护，禁止调用mutex等阻塞接口；
 * param:
 *        param: 回调参数
 *        mode：当前休眠流程的低功耗模式，请区分Deepsleep和standby场景下的处理流程
 *        saveAddr: 驱动保存寄存器数据的起始地址
 * retval：
 *           >0 成功，保存数据使用的地址空间大小
 *           <0 失败
 *           =0 成功，但没有保存数据
 *           PsmSuspendNoirq和PsmResumeNoirq返回值应当一致
 ************************************************************************************
*/
typedef struct {
    int (*PsmSuspendNoirq)(void *param, PSM_Mode mode, uint32_t *saveAddr);
    int (*PsmResumeNoirq)(void *param, PSM_Mode mode, uint32_t *saveAddr);
} PSM_DpmOps;

/**
\brief: device power manage info.
*/
typedef struct {
    PSM_DpmOps *ops;
    void *param;
    uint8_t isSuspendNoirq:1;
    uint8_t isSuspendSave:1;
} PSM_DpmInfo;

#define PSM_LEVEL_HIGH              "0"
#define PSM_LEVEL_MIDDLE            "1"
#define PSM_LEVEL_LOW               "2"

#define PSM_CMNDEV_LDO              "0"
#define PSM_CMNDEV_CLK              "1"
#define PSM_CMNDEV_LPM              "2"
#define PSM_CMNDEV_PAD              "3"
#define PSM_CMNDEV_GPIO             "4"
#define PSM_CMNDEV_UART1            "5"
#define PSM_CMNDEV_HDLC             "6"
#define PSM_CMNDEV_ICP              "7"
#define PSM_CMNDEV_DMA              "8"

typedef enum {
    PSM_DEV_SUBSYS = 0,
    PSM_DEV_SYS = 1,
} PSM_CallbackFlag;

#if defined (_PSM_TEST) && (defined(_USE_PHY_CAT1_SLEEP) || defined(_USE_PSM))
/**
 ************************************************************************************
 *sleepmode_func ：获取休眠模式和时长接口
 *sleepcheck_func：关中断后休眠检查接口，依据当前休眠模式判断是否依旧满足休眠条件，禁止阻塞
 *level : 优先级
 ************************************************************************************
*/
#define PSM_IDLE_CALLBACK_DEFINE(sleepmode_func, sleepcheck_func, level) \
                OS_USED PSM_IdleCallback __psm##sleepmode_func =  \
                {sleepmode_func, sleepcheck_func}

/**
 ************************************************************************************
 *ops ：PSM_DpmOps 结构体
 *param ：回调入参
 *level : 优先级
 ************************************************************************************
*/
#define PSM_DPM_INFO_DEFINE(name, ops, param, level) \
        OS_USED PSM_DpmInfo __psm_dpminfo##name =  \
        {&ops, param, 0, 0}

#else
/**
 ************************************************************************************
 *sleepmode_func ：获取休眠模式和时长接口
 *sleepcheck_func：关中断后休眠检查接口，依据当前休眠模式判断是否依旧满足休眠条件，禁止阻塞
 *level : 优先级
 ************************************************************************************
*/
#define PSM_IDLE_CALLBACK_DEFINE(sleepmode_func, sleepcheck_func, level) \
                OS_USED PSM_IdleCallback __psm##sleepmode_func SECTION(".psm_sleepmode." level) =  \
                {sleepmode_func, sleepcheck_func}

/**
 ************************************************************************************
 *ops ：PSM_DpmOps 结构体
 *param ：回调入参
 *level : 优先级
 ************************************************************************************
*/
#define PSM_DPM_INFO_DEFINE(name, ops, param, level) \
        OS_USED PSM_DpmInfo __psm_dpminfo##name SECTION(".psm_dpminfo." level) =  \
        {&ops, param, 0, 0}
#endif
/**
 ************************************************************************************
 *ops ：PSM_DpmOps 结构体
 *param ：回调入参
 *level : 优先级
 ************************************************************************************
*/
#define PSM_CMNDPM_INFO_DEFINE(name, ops, param, level) \
        OS_USED PSM_DpmInfo __psm_cmndpminfo##name SECTION(".psm_cmndpminfo." level) =  \
        {&ops, param, 0, 0}

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           休眠保存回调
 *
 * @param[in]       mode  休眠模式
 *                  flag  子系统设备/全系统设备
 * @return          DRV_ERR 执行失败，不能休眠
 *                  DRV_OK 执行成功，不能休眠
 * @retval
 ************************************************************************************
*/
__attribute__((noinline)) int PSM_SuspendNoirq(PSM_Mode mode, PSM_CallbackFlag flag);

/**
 ************************************************************************************
 * @brief           唤醒恢复回调
 *
 * @param[in]       mode  休眠模式
 *                  flag  子系统设备/全系统设备
 * @return          void
 * @retval
 ************************************************************************************
*/
__attribute__((noinline)) void PSM_ResumeNoirq(PSM_Mode mode, PSM_CallbackFlag flag);

/**
 ************************************************************************************
 * @brief           配置是否打开省电功能
 *
 * @param[in]       bool_t  true：使能;    false: 禁用
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PSM_Enable(bool_t en);

#endif
/** @} */