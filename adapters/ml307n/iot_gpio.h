/**
 * @file iot_gpio.h
 * @brief ML307N 平台 GPIO 适配器头文件
 * @details 基于 cm_gpio 接口实现 GPIO 功能封装，
 *          定义统一的 GPIO 类型，支持跨平台编译。
 */

#ifndef IOT_GPIO_ML307N_H
#define IOT_GPIO_ML307N_H

#include "cm_gpio.h"

/* ============================================================
 * 统一 GPIO 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief GPIO方向枚举（只有两个值，保持原样） */
typedef enum {
    IOT_GPIO_DIR_INPUT  = 0,         /**< 输入模式 */
    IOT_GPIO_DIR_OUTPUT = 1,         /**< 输出模式 */
} iot_gpio_dir_t;

/** @brief GPIO电平枚举（只有两个值，保持原样） */
typedef enum {
    IOT_GPIO_LEVEL_LOW  = 0,         /**< 低电平 */
    IOT_GPIO_LEVEL_HIGH = 1,         /**< 高电平 */
} iot_gpio_level_t;

/** @brief GPIO上下拉模式枚举 */
typedef enum {
    IOT_GPIO_PULL_NONE = 0,          /**< 无上下拉 */
    IOT_GPIO_PULL_UP   = 1,          /**< 上拉 */
    IOT_GPIO_PULL_DOWN = 2,          /**< 下拉 */
} iot_gpio_pull_t;

/** @brief GPIO中断触发模式枚举 */
typedef enum {
    IOT_GPIO_IRQ_RISING      = 0,    /**< 上升沿触发 */
    IOT_GPIO_IRQ_FALLING     = 1,    /**< 下降沿触发 */
    IOT_GPIO_IRQ_BOTH        = 2,    /**< 双边沿触发 */
    IOT_GPIO_IRQ_HIGH_LEVEL  = 3,    /**< 高电平触发 */
    IOT_GPIO_IRQ_LOW_LEVEL   = 4,    /**< 低电平触发 */
} iot_gpio_irq_mode_t;

/** @brief GPIO配置结构体 */
typedef struct {
    iot_gpio_dir_t dir;             /**< 方向 */
    iot_gpio_pull_t pull;           /**< 上下拉模式 */
} iot_gpio_config_t;

/** @brief GPIO中断回调函数类型 */
typedef void (*iot_gpio_irq_cb_t)(void *arg);

/* ============================================================
 * GPIO 句柄类型
 * ============================================================ */

/** @brief GPIO引脚类型 */
typedef cm_gpio_pin_e iot_gpio_t;

/* ============================================================
 * GPIO 操作函数声明
 * ============================================================ */

/**
 * @brief 初始化GPIO
 * @param[in] pin GPIO引脚
 * @param[in] config GPIO配置参数
 * @return 0 成功
 */
int iot_gpio_init(iot_gpio_t pin, const iot_gpio_config_t *config);

/**
 * @brief 设置GPIO方向
 * @param[in] pin GPIO引脚
 * @param[in] dir 方向
 * @return 0 成功
 */
int iot_gpio_set_dir(iot_gpio_t pin, iot_gpio_dir_t dir);

/**
 * @brief 设置GPIO输出电平
 * @param[in] pin GPIO引脚
 * @param[in] level 电平
 * @return 0 成功
 */
int iot_gpio_write(iot_gpio_t pin, iot_gpio_level_t level);

/**
 * @brief 读取GPIO输入电平
 * @param[in] pin GPIO引脚
 * @return GPIO电平
 */
iot_gpio_level_t iot_gpio_read(iot_gpio_t pin);

/**
 * @brief 翻转GPIO输出电平
 * @param[in] pin GPIO引脚
 * @return 0 成功
 */
int iot_gpio_toggle(iot_gpio_t pin);

/**
 * @brief 注册GPIO中断
 * @param[in] pin GPIO引脚
 * @param[in] mode 中断触发模式
 * @param[in] cb 中断回调函数
 * @param[in] arg 回调参数
 * @return 0 成功
 */
int iot_gpio_irq_enable(iot_gpio_t pin, iot_gpio_irq_mode_t mode, iot_gpio_irq_cb_t cb, void *arg);

/**
 * @brief 禁用GPIO中断
 * @param[in] pin GPIO引脚
 * @return 0 成功
 */
int iot_gpio_irq_disable(iot_gpio_t pin);

#endif /* IOT_GPIO_ML307N_H */