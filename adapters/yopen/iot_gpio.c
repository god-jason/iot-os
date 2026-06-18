/**
 * @file iot_gpio.c
 * @brief YOPEN 平台 GPIO 适配器实现
 * @details 基于 yopen_gpio 接口实现 GPIO 功能封装，
 *          支持枚举类型参数转换。
 */

#include "iot_gpio.h"

/* ============================================================
 * GPIO 操作函数实现
 * ============================================================ */

/**
 * @brief 初始化GPIO
 * @param[in] pin GPIO引脚
 * @param[in] config GPIO配置参数
 * @return 0 成功
 */
iot_err_t iot_gpio_init(iot_gpio_t pin, const iot_gpio_config_t *config)
{
    yopen_gpio_config_s gpio_config = {0};
    
    if (config == NULL) {
        return IOT_ERR_PARAM;
    }
    
    /* 配置参数转换 */
    gpio_config.dir = (yopen_gpio_dir_e)config->dir;
    gpio_config.pull = (yopen_gpio_pull_e)config->pull;
    
    /* 调用平台初始化 */
    return yopen_gpio_init(pin, &gpio_config);
}

/**
 * @brief 设置GPIO方向
 * @param[in] pin GPIO引脚
 * @param[in] dir 方向
 * @return 0 成功
 */
iot_err_t iot_gpio_set_dir(iot_gpio_t pin, iot_gpio_dir_t dir)
{
    return yopen_gpio_set_dir(pin, (yopen_gpio_dir_e)dir);
}

/**
 * @brief 设置GPIO输出电平
 * @param[in] pin GPIO引脚
 * @param[in] level 电平
 * @return 0 成功
 */
iot_err_t iot_gpio_write(iot_gpio_t pin, iot_gpio_level_t level)
{
    return yopen_gpio_write(pin, (yopen_gpio_level_e)level);
}

/**
 * @brief 读取GPIO输入电平
 * @param[in] pin GPIO引脚
 * @return GPIO电平
 */
iot_gpio_level_t iot_gpio_read(iot_gpio_t pin)
{
    return (iot_gpio_level_t)yopen_gpio_read(pin);
}

/**
 * @brief 翻转GPIO输出电平
 * @param[in] pin GPIO引脚
 * @return 0 成功
 */
iot_err_t iot_gpio_toggle(iot_gpio_t pin)
{
    return yopen_gpio_toggle(pin);
}

/**
 * @brief 注册GPIO中断
 * @param[in] pin GPIO引脚
 * @param[in] mode 中断触发模式
 * @param[in] cb 中断回调函数
 * @param[in] arg 回调参数
 * @return 0 成功
 */
iot_err_t iot_gpio_irq_enable(iot_gpio_t pin, iot_gpio_irq_mode_t mode, iot_gpio_irq_cb_t cb, void *arg)
{
    return yopen_gpio_irq_register(pin, (yopen_gpio_irq_mode_e)mode, (yopen_gpio_irq_cb_t)cb, arg);
}

/**
 * @brief 禁用GPIO中断
 * @param[in] pin GPIO引脚
 * @return 0 成功
 */
iot_err_t iot_gpio_irq_disable(iot_gpio_t pin)
{
    return yopen_gpio_irq_unregister(pin);
}