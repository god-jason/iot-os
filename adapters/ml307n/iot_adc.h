/**
 * @file iot_adc.h
 * @brief ML307N 平台 ADC 适配器头文件
 * @details 基于 cm_adc 接口实现 ADC 功能封装，
 *          定义统一的 ADC 类型，支持跨平台编译。
 */

#ifndef IOT_ADC_ML307N_H
#define IOT_ADC_ML307N_H

#include "cm_adc.h"
#include "../../iot_types.h"

/* ============================================================
 * 统一 ADC 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief ADC设备类型（直接使用平台定义） */
typedef cm_adc_dev_e iot_adc_t;

/* ============================================================
 * ADC 操作函数声明
 * ============================================================ */

/**
 * @brief 初始化ADC
 * @param[in] adc ADC设备 (CM_ADC_0 或 CM_ADC_1)
 * @return 0 成功，负值表示失败
 */
int iot_adc_init(iot_adc_t adc);

/**
 * @brief 去初始化ADC
 * @param[in] adc ADC设备
 * @return 0 成功，负值表示失败
 */
int iot_adc_deinit(iot_adc_t adc);

/**
 * @brief 读取ADC电压值
 * @param[in] adc ADC设备 (CM_ADC_0 或 CM_ADC_1)
 * @param[out] voltage 电压值指针（单位:mV）
 * @return 0 成功，负值表示失败
 */
int iot_adc_read(iot_adc_t adc, int32_t *voltage);

/**
 * @brief 读取VBAT电压
 * @param[out] voltage 电压值指针（单位:mV）
 * @return 0 成功，负值表示失败
 */
int iot_adc_read_vbat(int32_t *voltage);

/**
 * @brief 读取温度
 * @param[out] temperature 温度值指针（单位:0.1摄氏度）
 * @return 0 成功，负值表示失败
 */
int iot_adc_read_temperature(int32_t *temperature);

#endif /* IOT_ADC_ML307N_H */