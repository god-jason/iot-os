/**
 * @file iot_adc.h
 * @brief ML307N 平台 ADC 适配器头文件
 * @details 基于 cm_adc 接口实现 ADC 功能封装，
 *          定义统一的 ADC 类型，支持跨平台编译。
 */

#ifndef IOT_ADC_ML307N_H
#define IOT_ADC_ML307N_H

#include "cm_adc.h"

/* ============================================================
 * 统一 ADC 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief ADC通道枚举 */
typedef enum {
    IOT_ADC_CH_0 = 0,              /**< ADC通道0 */
    IOT_ADC_CH_1 = 1,              /**< ADC通道1 */
    IOT_ADC_CH_VBAT = 2,           /**< 电池电压通道 */
    IOT_ADC_CH_TEMP = 3,           /**< 温度传感器通道 */
} iot_adc_channel_t;

/** @brief ADC设备类型 */
typedef cm_adc_dev_e iot_adc_t;

/* ============================================================
 * ADC 操作函数声明
 * ============================================================ */

/**
 * @brief 初始化ADC
 * @param[in] adc ADC设备
 * @return 0 成功
 */
int iot_adc_init(iot_adc_t adc);

/**
 * @brief 去初始化ADC
 * @param[in] adc ADC设备
 * @return 0 成功
 */
int iot_adc_deinit(iot_adc_t adc);

/**
 * @brief 读取ADC原始值
 * @param[in] adc ADC设备
 * @param[in] channel ADC通道
 * @return ADC原始值，负值表示失败
 */
int32_t iot_adc_read_raw(iot_adc_t adc, iot_adc_channel_t channel);

/**
 * @brief 读取ADC电压值
 * @param[in] adc ADC设备
 * @param[out] voltage 电压值指针（单位:mV）
 * @return 0 成功
 */
int iot_adc_read_voltage(iot_adc_t adc, int32_t *voltage);

/**
 * @brief 读取VBAT电压
 * @param[out] voltage 电压值指针（单位:mV）
 * @return 0 成功
 */
int iot_adc_read_vbat(int32_t *voltage);

/**
 * @brief 读取温度
 * @param[out] temperature 温度值指针（单位:0.1摄氏度）
 * @return 0 成功
 */
int iot_adc_read_temperature(int32_t *temperature);

#endif /* IOT_ADC_ML307N_H */