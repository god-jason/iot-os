/**
 * @file iot_pm.h
 * @brief ML307N 平台电源管理(Power Management)适配器头文件
 * @details 封装 ML307N 平台 cm_pm 接口，提供统一的电源管理函数。
 *          支持关机、重启、电源模式设置、休眠锁管理、低功耗配置等功能。
 */

#ifndef IOT_PM_ML307N_H
#define IOT_PM_ML307N_H

#include "cm_pm.h"
#include "../../iot_types.h"

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
 * 电源管理操作函数声明
 * ============================================================ */

/**
 * @brief 关机（关闭模组电源）
 */
void iot_pm_poweroff(void);

/**
 * @brief 模组重启
 * @param[in] reason 重启原因
 */
void iot_pm_reboot(iot_pm_reset_reason_t reason);

/**
 * @brief 获取上电原因
 * @return 上电原因枚举值
 */
iot_pm_power_on_reason_t iot_pm_get_power_on_reason(void);

/**
 * @brief 设置模组电源模式
 * @param[in] mode 电源模式枚举值
 * @return 0 成功，<0 失败
 */
int iot_pm_set_mode(iot_pm_mode_t mode);

/**
 * @brief 获取模组电源模式
 * @param[out] mode 电源模式指针
 * @return 0 成功，<0 失败
 */
int iot_pm_get_mode(iot_pm_mode_t *mode);

/**
 * @brief 锁定工作状态，禁止进入深睡眠
 * @param[in] lock_type 锁类型枚举值
 * @return 0 成功，<0 失败
 */
int iot_pm_work_lock(iot_pm_work_lock_type_t lock_type);

/**
 * @brief 解锁工作状态，允许进入深睡眠
 * @param[in] lock_type 锁类型枚举值
 * @return 0 成功，<0 失败
 */
int iot_pm_work_unlock(iot_pm_work_lock_type_t lock_type);

/**
 * @brief 查询当前持有的工作状态锁
 * @return 位掩码，每一位代表一种锁类型是否被持有
 */
uint32_t iot_pm_work_locks_query(void);

#endif /* IOT_PM_ML307N_H */