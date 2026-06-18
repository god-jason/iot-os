/**
 * @file iot_pm.h
 * @brief YOPEN 平台电源管理适配器头文件
 * @details 基于 yopen_power 接口实现电源管理功能，
 *          支持休眠控制、唤醒锁、关机重启等功能。
 */

#ifndef IOT_PM_YOPEN_H
#define IOT_PM_YOPEN_H

#include "../iot_common.h"
#include "yopen_power.h"

/* ============================================================
 * 统一电源管理类型定义（跨平台兼容）
 * ============================================================ */

/** @brief 电源模式枚举 */
typedef enum {
    IOT_PM_MODE_NORMAL     = 0,       /**< 正常模式 */
    IOT_PM_MODE_LOW_POWER  = 1,       /**< 低功耗模式 */
    IOT_PM_MODE_SLEEP      = 2,       /**< 休眠模式 */
    IOT_PM_MODE_DEEP_SLEEP = 3,       /**< 深度休眠模式 */
} iot_pm_mode_t;

/** @brief 上电原因枚举 */
typedef enum {
    IOT_PM_POWER_ON_REASON_KEY      = 0,   /**< 按键上电 */
    IOT_PM_POWER_ON_REASON_ALARM    = 1,   /**< 闹钟上电 */
    IOT_PM_POWER_ON_REASON_CHARGER   = 2,   /**< 充电上电 */
    IOT_PM_POWER_ON_REASON_USB       = 3,   /**< USB上电 */
    IOT_PM_POWER_ON_REASON_RTC       = 4,   /**< RTC上电 */
    IOT_PM_POWER_ON_REASON_UNKNOWN   = 5,   /**< 未知原因 */
} iot_pm_power_on_reason_t;

/** @brief 重启原因枚举 */
typedef enum {
    IOT_PM_RESET_REASON_NORMAL    = 0,   /**< 正常重启 */
    IOT_PM_RESET_REASON_FOTA      = 1,   /**< FOTA重启 */
    IOT_PM_RESET_REASON_EXCEPTION = 2,   /**< 异常重启 */
    IOT_PM_RESET_REASON_DOWNLOAD  = 3,   /**< 下载重启 */
} iot_pm_reset_reason_t;

/** @brief 休眠状态枚举 */
typedef enum {
    IOT_PM_SLEEP_STATUS_NORMAL = 0,   /**< 正常运行 */
    IOT_PM_SLEEP_STATUS_SLEEP  = 1,   /**< 休眠中 */
} iot_pm_sleep_status_t;

/** @brief 唤醒源枚举 */
typedef enum {
    IOT_PM_WAKEUP_SOURCE_RTC    = 0,   /**< RTC唤醒 */
    IOT_PM_WAKEUP_SOURCE_GPIO   = 1,   /**< GPIO唤醒 */
    IOT_PM_WAKEUP_SOURCE_UART   = 2,   /**< UART唤醒 */
    IOT_PM_WAKEUP_SOURCE_TIMER  = 3,   /**< 定时器唤醒 */
    IOT_PM_WAKEUP_SOURCE_ALARM  = 4,   /**< 闹钟唤醒 */
} iot_pm_wakeup_source_t;

/** @brief 工作状态锁类型枚举 */
typedef enum {
    IOT_PM_WORK_LOCK_TYPE_UART   = 0,   /**< UART锁 */
    IOT_PM_WORK_LOCK_TYPE_SPI    = 1,   /**< SPI锁 */
    IOT_PM_WORK_LOCK_TYPE_I2C    = 2,   /**< I2C锁 */
    IOT_PM_WORK_LOCK_TYPE_GPIO   = 3,   /**< GPIO锁 */
    IOT_PM_WORK_LOCK_TYPE_APP    = 4,   /**< 应用锁 */
} iot_pm_work_lock_type_t;

/* ============================================================
 * 电源管理宏定义
 * ============================================================ */

/**
 * @brief 获取开机原因
 * @return 开机原因枚举值
 */
#define iot_pm_get_power_on_reason() \
    yopen_get_power_on_reason()

/**
 * @brief 获取重启原因
 * @return 重启原因枚举值
 */
#define iot_pm_get_reset_reason() \
    yopen_get_reset_reason()

/**
 * @brief 系统关机
 * @param[in] mode 关机模式（0=立即，1=正常）
 * @return 0 成功，<0 失败
 */
#define iot_pm_power_off(mode) \
    yopen_power_off((yopen_PowdMode)(mode))

/**
 * @brief 系统重启
 * @param[in] mode 重启模式（0=立即，1=正常）
 * @return 0 成功，<0 失败
 */
#define iot_pm_reboot(mode) \
    yopen_system_reset((yopen_ResetMode)(mode))

/**
 * @brief 获取系统运行状态
 * @return true 休眠中，false 正常运行
 */
#define iot_pm_is_sleep() \
    (YOPEN_SYSTEM_IS_SLEEP == yopen_get_sleep_state())

/**
 * @brief 使能自动休眠
 * @param[in] allow 是否允许休眠
 * @return 0 成功，<0 失败
 */
#define iot_pm_auto_sleep_enable(allow) \
    yopen_autosleep_enable((YOPEN_SLEEP_FLAG_E)(allow))

/**
 * @brief 注册进入休眠回调
 * @param[in] cb 休眠回调函数
 * @return 0 成功，<0 失败
 */
#define iot_pm_register_enter_sleep_cb(cb) \
    yopen_register_enter_sleep_callback((yopen_enter_sleep_callback)(cb))

/**
 * @brief 注册退出休眠回调
 * @param[in] cb 唤醒回调函数
 * @return 0 成功，<0 失败
 */
#define iot_pm_register_exit_sleep_cb(cb) \
    yopen_register_exit_sleep_callback((yopen_exit_sleep_callback)(cb))

/* ============================================================
 * 唤醒锁操作
 * ============================================================ */

/**
 * @brief 创建唤醒锁
 * @param[in] name 唤醒锁名称
 * @param[out] lock 唤醒锁句柄
 * @return 0 成功，<0 失败
 */
int32_t iot_pm_wakelock_create(const char *name, void **lock);

/**
 * @brief 销毁唤醒锁
 * @param[in] lock 唤醒锁句柄
 * @return 0 成功，<0 失败
 */
int32_t iot_pm_wakelock_destroy(void *lock);

/**
 * @brief 获取唤醒锁
 * @param[in] lock 唤醒锁句柄
 * @return 0 成功，<0 失败
 */
int32_t iot_pm_wakelock_acquire(void *lock);

/**
 * @brief 释放唤醒锁
 * @param[in] lock 唤醒锁句柄
 * @return 0 成功，<0 失败
 */
int32_t iot_pm_wakelock_release(void *lock);

#endif /* IOT_PM_YOPEN_H */