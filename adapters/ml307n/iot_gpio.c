/**
 * @file iot_gpio.c
 * @brief ML307N 平台 GPIO 适配器实现
 * @details 基于 cm_gpio_v2 接口实现 GPIO 功能封装，
 *          支持跨平台编译。
 */

#include "iot_gpio.h"

/* ============================================================
 * 类型转换函数
 * ============================================================ */

/**
 * @brief 中断触发模式转换
 */
static cm_gpio_interrupt_e iot_gpio_irq_mode_to_platform(iot_gpio_irq_mode_t mode)
{
    switch (mode) {
        case IOT_GPIO_IRQ_RISING:
            return CM_GPIO_IT_EDGE_RISING;
        case IOT_GPIO_IRQ_FALLING:
            return CM_GPIO_IT_EDGE_FALLING;
        case IOT_GPIO_IRQ_BOTH:
            return CM_GPIO_IT_EDGE_BOTH;
        case IOT_GPIO_IRQ_HIGH_LEVEL:
            return CM_GPIO_IT_LEVEL_HIGH;
        case IOT_GPIO_IRQ_LOW_LEVEL:
            return CM_GPIO_IT_LEVEL_LOW;
        default:
            return CM_GPIO_IT_EDGE_RISING;
    }
}

/* ============================================================
 * GPIO 操作函数实现
 * ============================================================ */

/**
 * @brief 初始化GPIO
 * @param[in] pin GPIO引脚
 * @param[in] config GPIO配置参数
 * @return 0 成功，负值表示失败
 */
int iot_gpio_init(iot_gpio_t pin, const iot_gpio_config_t *config)
{
    cm_gpio_cfg_t gpio_config = {0};
    
    if (config == NULL) {
        return IOT_ERR_PARAM;
    }
    
    /* 配置参数转换 */
    gpio_config.direction = (cm_gpio_direction_e)config->dir;
    gpio_config.pull = (cm_gpio_pull_e)config->pull;
    gpio_config.mode = CM_GPIO_MODE_NUM; /* 不支持，设为无效值 */
    
    /* 调用平台初始化 */
    return cm_gpio_init(pin, &gpio_config);
}

/**
 * @brief 去初始化GPIO
 * @param[in] pin GPIO引脚
 * @return 0 成功，负值表示失败
 */
int iot_gpio_deinit(iot_gpio_t pin)
{
    return cm_gpio_deinit(pin);
}

/**
 * @brief 设置GPIO方向
 * @param[in] pin GPIO引脚
 * @param[in] dir 方向
 * @return 0 成功，负值表示失败
 */
int iot_gpio_set_dir(iot_gpio_t pin, iot_gpio_dir_t dir)
{
    return cm_gpio_set_direction(pin, (cm_gpio_direction_e)dir);
}

/**
 * @brief 获取GPIO方向
 * @param[in] pin GPIO引脚
 * @param[out] dir 方向指针
 * @return 0 成功，负值表示失败
 */
int iot_gpio_get_dir(iot_gpio_t pin, iot_gpio_dir_t *dir)
{
    cm_gpio_direction_e direction;
    int ret;
    
    if (dir == NULL) {
        return IOT_ERR_PARAM;
    }
    
    ret = cm_gpio_get_direction(pin, &direction);
    if (ret == IOT_OK) {
        *dir = (iot_gpio_dir_t)direction;
    }
    return ret;
}

/**
 * @brief 设置GPIO输出电平
 * @param[in] pin GPIO引脚
 * @param[in] level 电平
 * @return 0 成功，负值表示失败
 */
int iot_gpio_write(iot_gpio_t pin, iot_gpio_level_t level)
{
    return cm_gpio_set_level(pin, (cm_gpio_level_e)level);
}

/**
 * @brief 读取GPIO输入电平
 * @param[in] pin GPIO引脚
 * @return GPIO电平
 */
iot_gpio_level_t iot_gpio_read(iot_gpio_t pin)
{
    cm_gpio_level_e level;
    
    if (cm_gpio_get_level(pin, &level) != IOT_OK) {
        return IOT_GPIO_LEVEL_LOW; /* 默认值 */
    }
    return (iot_gpio_level_t)level;
}

/**
 * @brief 翻转GPIO输出电平
 * @param[in] pin GPIO引脚
 * @return 0 成功，负值表示失败
 */
int iot_gpio_toggle(iot_gpio_t pin)
{
    cm_gpio_level_e level;
    int ret;
    
    /* 先获取当前电平 */
    ret = cm_gpio_get_level(pin, &level);
    if (ret != IOT_OK) {
        return ret;
    }
    
    /* 翻转并设置 */
    level = (level == CM_GPIO_LEVEL_HIGH) ? CM_GPIO_LEVEL_LOW : CM_GPIO_LEVEL_HIGH;
    return cm_gpio_set_level(pin, level);
}

/**
 * @brief 注册GPIO中断
 * @param[in] pin GPIO引脚
 * @param[in] mode 中断触发模式
 * @param[in] cb 中断回调函数
 * @param[in] arg 回调参数
 * @return 0 成功，负值表示失败
 */
int iot_gpio_irq_enable(iot_gpio_t pin, iot_gpio_irq_mode_t mode, iot_gpio_irq_cb_t cb, void *arg)
{
    int ret;
    
    /* 注册中断回调 */
    ret = cm_gpio_interrupt_register(pin, (void *)cb);
    if (ret != IOT_OK) {
        return ret;
    }
    
    /* 使能中断并设置触发模式 */
    return cm_gpio_interrupt_enable(pin, iot_gpio_irq_mode_to_platform(mode));
}

/**
 * @brief 禁用GPIO中断
 * @param[in] pin GPIO引脚
 * @return 0 成功，负值表示失败
 */
int iot_gpio_irq_disable(iot_gpio_t pin)
{
    return cm_gpio_interrupt_disable(pin);
}