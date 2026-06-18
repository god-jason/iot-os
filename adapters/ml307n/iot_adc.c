/**
 * @file iot_adc.c
 * @brief ML307N 平台 ADC 适配器实现
 * @details 基于 cm_adc 接口实现 ADC 功能封装，
 *          支持枚举类型参数转换。
 */

#include "iot_adc.h"

/* ============================================================
 * ADC 操作函数实现
 * ============================================================ */

/**
 * @brief 初始化ADC
 * @param[in] adc ADC设备
 * @return 0 成功
 */
int iot_adc_init(iot_adc_t adc)
{
    (void)adc;
    return 0;
}

/**
 * @brief 去初始化ADC
 * @param[in] adc ADC设备
 * @return 0 成功
 */
int iot_adc_deinit(iot_adc_t adc)
{
    (void)adc;
    return 0;
}

/**
 * @brief 读取ADC原始值
 * @param[in] adc ADC设备
 * @param[in] channel ADC通道
 * @return ADC原始值，负值表示失败
 */
int32_t iot_adc_read_raw(iot_adc_t adc, iot_adc_channel_t channel)
{
    return cm_adc_read((cm_adc_dev_e)adc, (int32_t *)channel);
}

/**
 * @brief 读取ADC电压值
 * @param[in] adc ADC设备
 * @param[out] voltage 电压值指针（单位:mV）
 * @return 0 成功
 */
int iot_adc_read_voltage(iot_adc_t adc, int32_t *voltage)
{
    return cm_adc_read((cm_adc_dev_e)adc, voltage);
}

/**
 * @brief 读取VBAT电压
 * @param[out] voltage 电压值指针（单位:mV）
 * @return 0 成功
 */
int iot_adc_read_vbat(int32_t *voltage)
{
    return cm_adc_vbat_read(voltage);
}

/**
 * @brief 读取温度
 * @param[out] temperature 温度值指针（单位:0.1摄氏度）
 * @return 0 成功
 */
int iot_adc_read_temperature(int32_t *temperature)
{
    return cm_adc_temperature_read(temperature);
}