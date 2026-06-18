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
#include <stdbool.h>

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
    CM_POWERKEY_EVENT_SHORTPRESS,          /*!<按键被短按*/
    CM_POWERKEY_EVENT_LONGPRESS,           /*!<按键被长按*/
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
    CM_PM_POWER_ON = 0,     /*!< 正常上电 */
    CM_PM_PIN_RESET,        /*!< PIN按键复位 */
    CM_PM_SOFT_RESET,       /*!< 软件复位 */
    CM_PM_UTC_WAKEUP,       /*!< UTC超时唤醒 */
    CM_PM_EXTPIN_WAKEUP,    /*!< WAKEUP-PIN唤醒 */
    CM_PM_WDT_RESET,        /*!< 硬件看门狗复位 */
    CM_PM_UNKNOWN_ON,       /*!< 未知，一般为硬件错误 */
    CM_PM_EXCEPTRESET,
    CM_PM_BOOST_IN,
    CM_PM_LOCALUPDATE,
    CM_PM_FOTA,
    CM_PM_HDT_TEST,
    CM_PM_STANDBY,
    CM_PM_AMT,
}cm_pm_power_on_reason_e;

/** 重启类型 */
typedef enum
{
    CM_PM_REBOOT_TYPE0_NONE = 0,     /*!< 无重启 */
    CM_PM_REBOOT_TYPE1_NORMAL,       /*!< 正常重启 */
    CM_PM_REBOOT_TYPEMAX,            /*!< 重启枚举最大值 */
}cm_pm_reboot_type_e;

/** 协议栈低功耗状态,不支持 */
typedef enum
{
    CM_PM_PS_STATUS_CONNECT = 0,  /*!< 连接态 */
    CM_PM_PS_STATUS_IDLE,         /*!< 空闲态 */
    CM_PM_PS_STATUS_PSM,          /*!< PSM模式 */
    CM_PM_PS_STATUS_MAX,
}cm_pm_ps_status_e;

/** 模组低功耗状态 */
typedef enum
{
    CM_PM_SLEEP_STATUS_ACTIVE = 0,    /*!< ACTIVE */
    CM_PM_SLEEP_STATUS_LIGHT,         /*!< 浅睡眠,不支持 */
    CM_PM_SLEEP_STATUS_DEEP,          /*!< 深睡眠 */
    CM_PM_SLEEP_STATUS_MAX,
}cm_pm_sleep_status_e;

/** 模组低功耗模式 */
typedef enum
{
    CM_PM_SLEEP_MODE_ACTIVE = 0,    /*!< 关闭休眠模式 */
    CM_PM_SLEEP_MODE_LIGHT,         /*!< 允许浅睡眠,不支持 */
    CM_PM_SLEEP_MODE_DEEP,          /*!< 允许浅睡眠和深睡眠 */
    CM_PM_SLEEP_MODE_MAX,
}cm_pm_sleep_mode_e;

/** 低功耗模式配置结构体 */
typedef struct
{
    int mode;           /*!< 休眠模式，取值参考cm_pm_sleep_mode_e */
    bool permanent;     /*!< 是否保存至flash，设置时有效，获取时忽略 */
}cm_pm_sleep_mode_t;

/** 低功耗管理配置选项 */
typedef enum
{
    CM_PM_CFG_PSIND,        /*!< 设置协议栈低功耗上报，对应cm_pm_psind_cb，不支持 */
    CM_PM_CFG_SLEEPIND,     /*!< 设置模组低功耗上报，对应cm_pm_sleepind_cb */
    CM_PM_CFG_SLEEPMODE,    /*!< 设置模组低功耗模式，对应cm_pm_sleep_mode_t */
    CM_PM_CFG_DELAYSLEEP,   /*!< 设置延时休眠时间，unsigned short类型，不支持 */
    CM_PM_CFG_DTR,          /*!< 设置DTR引脚控制休眠唤醒，bool类型，不支持 */
    CM_PM_CFG_WAKEUPIN,     /*!< 设置wakeup_in引脚控制休眠唤醒，bool类型，不支持 */
    CM_PM_CFG_WAKEUPOUT,    /*!< 设置wakeup_out引脚指示休眠唤醒，bool类型，不支持 */
    CM_PM_CFG_MAX,
}cm_pm_cfg_type_e;

/** 唤醒源 */
typedef enum
{
    CM_PM_WAKEUP_SOURCE_NONE = 0,         /*!< 无唤醒源 */
    CM_PM_WAKEUP_SOURCE_RTC_TIMER,        /*!< RTC定时器唤醒 */
    CM_PM_WAKEUP_SOURCE_GPIO_INTERRUPT,   /*!< GPIO中断唤醒 */
    CM_PM_WAKEUP_SOURCE_UART_WAKEUP,      /*!< UART唤醒 */
    CM_PM_WAKEUP_SOURCE_NETWORK_EVENT,    /*!< 网络事件唤醒 */
    CM_PM_WAKEUP_SOURCE_USB_WAKEUP,       /*!< USB唤醒 */
    CM_PM_WAKEUP_SOURCE_CHARGER_ATTACH,   /*!< 充电器插入唤醒 */
    CM_PM_WAKEUP_SOURCE_KEYPAD_PRESS,     /*!< 按键按下唤醒 */
    CM_PM_WAKEUP_SOURCE_EXTERNAL_SENSOR,  /*!< 外部传感器唤醒 */
    CM_PM_WAKEUP_SOURCE_OTHERS,           /*!< 其他唤醒源 */
    CM_PM_WAKEUP_SOURCE_MAX               /*!< 枚举最大值 */
}cm_pm_wakeup_source_e;

/** 工作状态锁类型
 * 总共32位；
 * sys为系统锁，用户不可使用；
 * 用户仅可使用user0及后面位的锁，可以自行扩展，但不能超过32位；
 */
typedef enum
{
    CM_PM_WORK_LOCK_TYPE_SYSNETWORK = (1U << 0),     /*!< 网络活动锁 - bit0 */
    CM_PM_WORK_LOCK_TYPE_SYSGPS = (1U << 1),         /*!< GPS定位锁 - bit1 */
    CM_PM_WORK_LOCK_TYPE_SYSBLUETOOTH = (1U << 2),   /*!< 蓝牙连接锁 - bit2 */
    CM_PM_WORK_LOCK_TYPE_SYSAUDIO = (1U << 3),       /*!< 音频处理锁 - bit3 */
    CM_PM_WORK_LOCK_TYPE_USER0 = (1U << 4),          /*!< 用户使用锁 - bit4 */
    CM_PM_WORK_LOCK_TYPE_USER1 = (1U << 5),          /*!< 用户使用锁 - bit5 */
}cm_pm_work_lock_type_e;

/** 协议栈低功耗回调，不支持 */
typedef void (*cm_pm_psind_cb)(cm_pm_ps_status_e status);

/** 模组低功耗回调，source值详见电源管理文档 */
typedef void (*cm_pm_sleepind_cb)(cm_pm_sleep_status_e status, cm_pm_wakeup_source_e source);

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
 * @details 关闭模组电源
 */
void cm_pm_poweroff(void);

/**
 * @brief 模组重启
 *
 * @param [in] type 重启类型代码
 *
 * OS_REBOOT_DOWNLOAD = 0,
 * OS_REBOOT_NORMAL = 1,
 * OS_REBOOT_FOTA = 2,
 * OS_REBOOT_EXCEPT = 3,
 *
 * @return 空
 *
 * @details 根据指定类型重启模组
 *
 * @deprecated 不建议使用cm_pm_reboot，该接口仅用于老版本兼容，推荐使用cm_pm_reboot_v2接口。
 */
void cm_pm_reboot(void);
void cm_pm_reboot_v2(uint8_t type);

/**
 * @brief 获取上电原因
 *
 * @return
 *   >= 0  - 上电原因，参考cm_pm_power_on_reason_e \n
 *   < 0   - 失败，返回错误码
 *
 * @details 获取模组本次上电的原因
 */
cm_pm_power_on_reason_e cm_pm_get_power_on_reason(void);

/**
 * @brief 设置模组电源模式
 *
 * @param [in] pm_mode 模组电源模式
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败，返回错误码
 *
 * @details 设置模组的电源管理模式
 */
int32_t cm_pm_set_mode(int32_t pm_mode);

/**
 * @brief 获取模组电源模式
 *
 * @param [out] pm_mode 模组电源模式指针
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败，返回错误码
 *
 * @details 获取当前模组的电源管理模式
 */
int32_t cm_pm_get_mode(int32_t *pm_mode);

/**
 * @brief 锁定工作状态，禁止进入深睡眠
 *
 * @param [in] lock_type 锁类型，用于区分不同的锁定原因
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败，返回错误码
 *
 * @details 执行后，模组将无法进入深睡眠状态
 */
int32_t cm_pm_work_lock_with_type(cm_pm_work_lock_type_e lock_type);

/**
 * @brief 解锁工作状态，允许进入深睡眠
 *
 * @param [in] lock_type 锁类型，用于区分不同的锁定原因
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败，返回错误码
 *
 * @details 执行后，模组将允许进入深睡眠状态
 */
int32_t cm_pm_work_unlock_with_type(cm_pm_work_lock_type_e lock_type);

/**
 * @brief 查询当前持有的工作状态锁
 *
 * @return
 *   返回一个位掩码，每一位代表一种锁类型是否被持有 \n
 *   例如：bit0=1表示CM_PM_WORK_LOCK_TYPE_SYSNETWORK被持有 \n
 *         bit1=1表示CM_PM_WORK_LOCK_TYPE_SYSGPS被持有 \n
 *         bit2=1表示CM_PM_WORK_LOCK_TYPE_SYSBLUETOOTH被持有 \n
 *         bit3=1表示CM_PM_WORK_LOCK_TYPE_SYSAUDIO被持有 \n
 *         bit4=1表示CM_PM_WORK_LOCK_TYPE_USER0被持有 \n
 *         bit5=1表示CM_PM_WORK_LOCK_TYPE_USER1被持有 \n
 *         以此类推,cm_pm_work_lock_type_e
 *
 * @details 可以通过与运算检查特定类型的锁是否被持有
 */
uint32_t cm_pm_work_locks_query(void);

/**
 * @brief 低功耗管理配置
 *
 * @param [in] type 配置类型，详见cm_pm_cfg_type_e
 *                 - CM_PM_CFG_PSIND: info对应cm_pm_psind_cb类型的回调函数指针
 *                 - CM_PM_CFG_SLEEPIND: info对应cm_pm_sleepind_cb类型的回调函数指针
 *                 - CM_PM_CFG_SLEEPMODE: info对应cm_pm_sleep_mode_t类型的配置结构体指针
 *                 - CM_PM_CFG_DELAYSLEEP: info对应unsigned short类型的延时时间(单位秒)
 *                 - CM_PM_CFG_DTR: info对应bool类型的DTR控制使能标志
 *                 - CM_PM_CFG_WAKEUPIN: info对应bool类型的wakeup_in引脚控制使能标志
 *                 - CM_PM_CFG_WAKEUPOUT: info对应bool类型的wakeup_out引脚控制使能标志
 * @param [in] info 配置信息，根据type参数传入对应的结构体或值的指针
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败，返回值为错误码
 *
 * @details info需传入对应参数类型指针
 */
int32_t cm_pm_set_cfg(cm_pm_cfg_type_e type, void *info);

/**
 * @brief 获取低功耗管理配置
 *
 * @param [in] type 配置类型，详见cm_pm_cfg_type_e
 *                 - CM_PM_CFG_PSIND: info对应cm_pm_psind_cb类型的回调函数指针
 *                 - CM_PM_CFG_SLEEPIND: info对应cm_pm_sleepind_cb类型的回调函数指针
 *                 - CM_PM_CFG_SLEEPMODE: info对应cm_pm_sleep_mode_t类型的配置结构体指针
 *                 - CM_PM_CFG_DELAYSLEEP: info对应unsigned short类型的延时时间(单位秒)指针
 *                 - CM_PM_CFG_DTR: info对应bool类型的DTR控制使能标志指针
 *                 - CM_PM_CFG_WAKEUPIN: info对应bool类型的wakeup_in引脚控制使能标志指针
 *                 - CM_PM_CFG_WAKEUPOUT: info对应bool类型的wakeup_out引脚控制使能标志指针
 * @param [out] info 信息缓存指针，用于接收对应类型的配置信息
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败，返回值为错误码
 *
 * @details info需传入对应参数类型指针
 */
int32_t cm_pm_get_cfg(cm_pm_cfg_type_e type, void *info);

/**
 * @brief pm初始化
 *
 * @param [in] pm_cfg:参数配置
 *
 * @return void
 *
 * @details More details
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，推荐使用cm_pm_work_lock_with_type接口直接实现初始化和上锁，
 *              使用cm_pm_set_cfg接口实现配置。
 */
void cm_pm_init(cm_pm_cfg_t pm_cfg);


/**
 * @brief 睡眠模式上锁
 *
 * @return 空
 *
 * @details 执行后，模组将无法进入休眠状态
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，推荐使用cm_pm_work_lock_with_type接口。
 */
void cm_pm_work_lock(void);

/**
 * @brief 睡眠模式解锁
 *
 * @return 空
 *
 * @details 执行后，模组将允许进入休眠状态
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，推荐使用cm_pm_work_unlock_with_type接口。
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
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，暂时保留。
 */
int32_t cm_pm_powerkey_regist_callback(cm_pm_powerkey_cb_t callback);


#undef EXTERN
#ifdef __cplusplus
}
#endif


#endif /* __CM_PM_H__ */

/** @}*/
