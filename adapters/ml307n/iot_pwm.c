/**
 * @file iot_pwm.c
 * @brief ML307N 平台 PWM 适配器实现
 * @details 基于 cm_pwm 接口实现 PWM 功能封装，
 *          支持跨平台编译。
 */

#include "iot_pwm.h"

/* ============================================================
 * PWM 操作函数实现
 * ============================================================ */

/**
 * @brief 打开PWM设备
 * @param[in] pwm PWM设备
 * @param[in] config PWM配置参数
 * @return 0 成功，负值表示失败
 */
int iot_pwm_open(iot_pwm_t pwm, const iot_pwm_config_t *config)
{
    if (config == NULL) {
        return IOT_ERR_PARAM;
    }
    
    /* 设置时钟源 */
    if (cm_pwm_set_clk((cm_pwm_dev_e)pwm, (cm_pwm_clk_e)config->clk) != IOT_OK) {
        return IOT_ERR;
    }
    
    /* 计算周期和占空比（单位：纳秒） */
    uint32_t period = (1000000000UL / config->freq);
    uint32_t period_h = (period * config->duty) / 100;
    
    /* 打开PWM */
    return cm_pwm_open_ns((cm_pwm_dev_e)pwm, period, period_h);
}

/**
 * @brief 关闭PWM设备
 * @param[in] pwm PWM设备
 * @return 0 成功，负值表示失败
 */
int iot_pwm_close(iot_pwm_t pwm)
{
    return cm_pwm_close((cm_pwm_dev_e)pwm);
}

/**
 * @brief 设置PWM频率和占空比
 * @param[in] pwm PWM设备
 * @param[in] freq 频率(Hz)
 * @param[in] duty 占空比(0-100)
 * @return 0 成功，负值表示失败
 */
int iot_pwm_set(iot_pwm_t pwm, uint32_t freq, uint8_t duty)
{
    /* 计算周期和占空比（单位：纳秒） */
    uint32_t period = (1000000000UL / freq);
    uint32_t period_h = (period * duty) / 100;
    
    /* 关闭后重新打开 */
    if (cm_pwm_close((cm_pwm_dev_e)pwm) != IOT_OK) {
        return IOT_ERR;
    }
    return cm_pwm_open_ns((cm_pwm_dev_e)pwm, period, period_h);
}

/**
 * @brief 使能PWM
 * @param[in] pwm PWM设备
 * @return 0 成功，负值表示失败
 */
int iot_pwm_enable(iot_pwm_t pwm)
{
    /* cm_pwm_open_ns已经使能，无需额外操作 */
    (void)pwm;
    return IOT_OK;
}

/**
 * @brief 禁用PWM
 * @param[in] pwm PWM设备
 * @return 0 成功，负值表示失败
 */
int iot_pwm_disable(iot_pwm_t pwm)
{
    return cm_pwm_close((cm_pwm_dev_e)pwm);
}