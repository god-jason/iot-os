/**
 * @file iot_pm.c
 * @brief ML307N 平台电源管理(Power Management)适配器实现
 * @details 封装 ML307N 平台 cm_pm 接口，提供统一的电源管理函数。
 *          支持关机、重启、电源模式设置、休眠锁管理、低功耗配置等功能。
 */

#include "iot_pm.h"

/* ============================================================
 * 电源管理操作函数实现
 * ============================================================ */

/**
 * @brief 关机（关闭模组电源）
 */
void iot_pm_poweroff(void)
{
    cm_pm_poweroff();
}

/**
 * @brief 模组重启
 * @param[in] reason 重启原因
 */
void iot_pm_reboot(iot_pm_reset_reason_t reason)
{
    cm_pm_reboot_v2((uint8_t)reason);
}

/**
 * @brief 获取上电原因
 * @return 上电原因枚举值
 */
iot_pm_power_on_reason_t iot_pm_get_power_on_reason(void)
{
    return (iot_pm_power_on_reason_t)cm_pm_get_power_on_reason();
}

/**
 * @brief 设置模组电源模式
 * @param[in] mode 电源模式枚举值
 * @return 0 成功，<0 失败
 */
int iot_pm_set_mode(iot_pm_mode_t mode)
{
    return cm_pm_set_mode((int32_t)mode);
}

/**
 * @brief 获取模组电源模式
 * @param[out] mode 电源模式指针
 * @return 0 成功，<0 失败
 */
int iot_pm_get_mode(iot_pm_mode_t *mode)
{
    int32_t pm_mode = 0;
    int ret;
    
    if (mode == NULL) {
        return IOT_ERR_PARAM;
    }
    
    ret = cm_pm_get_mode(&pm_mode);
    if (ret == 0) {
        *mode = (iot_pm_mode_t)pm_mode;
    }
    
    return ret;
}

/**
 * @brief 锁定工作状态，禁止进入深睡眠
 * @param[in] lock_type 锁类型枚举值
 * @return 0 成功，<0 失败
 */
int iot_pm_work_lock(iot_pm_work_lock_type_t lock_type)
{
    return cm_pm_work_lock_with_type((cm_pm_work_lock_type_e)lock_type);
}

/**
 * @brief 解锁工作状态，允许进入深睡眠
 * @param[in] lock_type 锁类型枚举值
 * @return 0 成功，<0 失败
 */
int iot_pm_work_unlock(iot_pm_work_lock_type_t lock_type)
{
    return cm_pm_work_unlock_with_type((cm_pm_work_lock_type_e)lock_type);
}

/**
 * @brief 查询当前持有的工作状态锁
 * @return 位掩码，每一位代表一种锁类型是否被持有
 */
uint32_t iot_pm_work_locks_query(void)
{
    return cm_pm_work_locks_query();
}