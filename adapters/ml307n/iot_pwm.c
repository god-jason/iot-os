/**
 * @file iot_pwm.c
 * @brief ML307N 平台 PWM 适配器实现
 * @details 基于 cm_pwm 接口实现 PWM 功能封装，
 *          支持枚举类型参数转换。
 */

#include "iot_pwm.h"

/* ============================================================
 * PWM 操作函数实现
 * ============================================================ */

/**
 * @brief 打开PWM设备
 * @param[in] pwm PWM设备
 * @param[in] config PWM配置参数
 * @return 0 成功
 */
int iot_pwm_open(iot_pwm_t pwm, const iot_pwm_config_t *config)
{
    cm_pwm_clk_e clk;
    
    if (config == NULL) {
        return IOT_ERR_PARAM;
    }
    
    /* 时钟源转换 */
    clk = (config->clk == IOT_PWM_CLK_32K) ? CM_PWM_CLK_32K : CM_PWM_CLK_12800K;
    
    /* 设置时钟源 */
    cm_pwm_set_clk((cm_pwm_dev_e)pwm, clk);
    
    /* 计算周期和占空比 */
    uint32_t period = (1000000000UL / config->freq);
    uint32_t period_h = (period * config->duty) / 100;
    
    /* 打开PWM */
    return cm_pwm_open_ns((cm_pwm_dev_e)pwm, period, period_h);
}

/**
 * @brief 关闭PWM设备
 * @param[in] pwm PWM设备
 * @return 0 成功
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
 * @return 0 成功
 */
int iot_pwm_set(iot_pwm_t pwm, uint32_t freq, uint8_t duty)
{
    uint32_t period = (1000000000UL / freq);
    uint32_t period_h = (period * duty) / 100;
    
    /* 关闭后重新打开 */
    cm_pwm_close((cm_pwm_dev_e)pwm);
    return cm_pwm_open_ns((cm_pwm_dev_e)pwm, period, period_h);
}

/**
 * @brief 设置PWM占空比
 * @param[in] pwm PWM设备
 * @param[in] duty 占空比(0-100)
 * @return 0 成功
 */
int iot_pwm_set_duty(iot_pwm_t pwm, uint8_t duty)
{
    /* PWM不支持动态修改占空比，需要重新打开 */
    (void)pwm;
    (void)duty;
    return IOT_ERR_NOT_SUPPORT;
}

/**
 * @brief 使能PWM
 * @param[in] pwm PWM设备
 * @return 0 成功
 */
int iot_pwm_enable(iot_pwm_t pwm)
{
    /* cm_pwm_open_ns已经使能 */
    (void)pwm;
    return 0;
}

/**
 * @brief 禁用PWM
 * @param[in] pwm PWM设备
 * @return 0 成功
 */
int iot_pwm_disable(iot_pwm_t pwm)
{
    return cm_pwm_close((cm_pwm_dev_e)pwm);
}