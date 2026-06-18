/**
 * @file iot_pwm.h
 * @brief ML307N 平台 PWM 适配器头文件
 * @details 基于 cm_pwm 接口实现 PWM 功能封装，
 *          定义统一的 PWM 类型，支持跨平台编译。
 */

#ifndef IOT_PWM_ML307N_H
#define IOT_PWM_ML307N_H

#include "cm_pwm.h"
#include "../../iot_types.h"

/* ============================================================
 * 统一 PWM 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief PWM设备类型 */
typedef cm_pwm_dev_e iot_pwm_t;

/** @brief PWM时钟源枚举（与平台CM_PWM_CLK_xxx值一致） */
typedef enum {
    IOT_PWM_CLK_32K = CM_PWM_CLK_32K,           /**< 32k时钟源，支持低功耗 */
    IOT_PWM_CLK_12800K = CM_PWM_CLK_12800K,     /**< 12.8M时钟源，不支持低功耗 */
} iot_pwm_clk_t;

/** @brief PWM配置结构体 */
typedef struct {
    uint32_t freq;                 /**< 频率(Hz) */
    uint8_t duty;                  /**< 占空比(0-100) */
    iot_pwm_clk_t clk;             /**< 时钟源 */
} iot_pwm_config_t;

/* ============================================================
 * PWM 操作函数声明
 * ============================================================ */

/**
 * @brief 打开PWM设备
 * @param[in] pwm PWM设备
 * @param[in] config PWM配置参数
 * @return 0 成功，负值表示失败
 */
int iot_pwm_open(iot_pwm_t pwm, const iot_pwm_config_t *config);

/**
 * @brief 关闭PWM设备
 * @param[in] pwm PWM设备
 * @return 0 成功，负值表示失败
 */
int iot_pwm_close(iot_pwm_t pwm);

/**
 * @brief 设置PWM频率和占空比
 * @param[in] pwm PWM设备
 * @param[in] freq 频率(Hz)
 * @param[in] duty 占空比(0-100)
 * @return 0 成功，负值表示失败
 */
int iot_pwm_set(iot_pwm_t pwm, uint32_t freq, uint8_t duty);

/**
 * @brief 使能PWM
 * @param[in] pwm PWM设备
 * @return 0 成功，负值表示失败
 */
int iot_pwm_enable(iot_pwm_t pwm);

/**
 * @brief 禁用PWM
 * @param[in] pwm PWM设备
 * @return 0 成功，负值表示失败
 */
int iot_pwm_disable(iot_pwm_t pwm);

#endif /* IOT_PWM_ML307N_H */