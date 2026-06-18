/**
 * @file iot_pm.c
 * @brief YOPEN 平台电源管理(Power Management)适配器实现
 * @details 基于 yopen_power 接口实现电源管理功能封装，
 *          支持跨平台编译。
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
    yopen_poweroff();
}

/**
 * @brief 模组重启
 * @param[in] reason 重启原因
 */
void iot_pm_reboot(iot_pm_reset_reason_t reason)
{
    yopen_system_reset((yopen_ResetMode)reason);
}

/**
 * @brief 获取上电原因
 * @return 上电原因枚举值
 */
iot_pm_power_on_reason_t iot_pm_get_power_on_reason(void)
{
    return (iot_pm_power_on_reason_t)yopen_get_power_on_reason();
}

/**
 * @brief 设置模组电源模式
 * @param[in] mode 电源模式枚举值
 * @return 0 成功，<0 失败
 */
iot_err_t iot_pm_set_mode(iot_pm_mode_t mode)
{
    return yopen_pm_set_mode((yopen_pm_mode_e)mode);
}

/**
 * @brief 获取模组电源模式
 * @param[out] mode 电源模式指针
 * @return 0 成功，<0 失败
 */
iot_err_t iot_pm_get_mode(iot_pm_mode_t *mode)
{
    yopen_pm_mode_e pm_mode = YOPEN_PM_MODE_NORMAL;
    int ret;
    
    if (mode == NULL) {
        return IOT_ERR_PARAM;
    }
    
    ret = yopen_pm_get_mode(&pm_mode);
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
iot_err_t iot_pm_work_lock(iot_pm_work_lock_type_t lock_type)
{
    return yopen_pm_work_lock((yopen_pm_work_lock_type_e)lock_type);
}

/**
 * @brief 解锁工作状态，允许进入深睡眠
 * @param[in] lock_type 锁类型枚举值
 * @return 0 成功，<0 失败
 */
iot_err_t iot_pm_work_unlock(iot_pm_work_lock_type_t lock_type)
{
    return yopen_pm_work_unlock((yopen_pm_work_lock_type_e)lock_type);
}

/**
 * @brief 查询当前持有的工作状态锁
 * @return 位掩码，每一位代表一种锁类型是否被持有
 */
uint32_t iot_pm_work_locks_query(void)
{
    return yopen_pm_work_locks_query();
}