#ifndef BME280_H
#define BME280_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers.h"

/*===========================================================
 * BME280 寄存器定义
 *===========================================================*/

#define BME280_REG_ID             0xD0
#define BME280_REG_RESET          0xE0
#define BME280_REG_CTRL_HUM       0xF2
#define BME280_REG_STATUS         0xF3
#define BME280_REG_CTRL_MEAS      0xF4
#define BME280_REG_CONFIG         0xF5
#define BME280_REG_PRESS_MSB      0xF7
#define BME280_REG_PRESS_LSB      0xF8
#define BME280_REG_PRESS_XLSB     0xF9
#define BME280_REG_TEMP_MSB       0xFA
#define BME280_REG_TEMP_LSB       0xFB
#define BME280_REG_TEMP_XLSB      0xFC
#define BME280_REG_HUM_MSB        0xFD
#define BME280_REG_HUM_LSB        0xFE

/*===========================================================
 * BME280 设备地址
 *===========================================================*/

#define BME280_ADDR_PRIMARY       0x76
#define BME280_ADDR_SECONDARY     0x77

/*===========================================================
 * BME280 复位值
 *===========================================================*/

#define BME280_RESET_VALUE        0xB6

/*===========================================================
 * BME280 配置结构体
 *===========================================================*/

typedef struct {
    uint8_t addr;
    uint8_t osrs_t;
    uint8_t osrs_p;
    uint8_t osrs_h;
    uint8_t mode;
    uint8_t t_sb;
    uint8_t filter;
} bme280_config_t;

/*===========================================================
 * BME280 校准数据结构体
 *===========================================================*/

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} bme280_calib_t;

/*===========================================================
 * BME280 私有数据结构体
 *===========================================================*/

typedef struct {
    bme280_config_t config;
    bme280_calib_t calib;
    int32_t t_fine;
} bme280_priv_t;

/*===========================================================
 * BME280 数据结构体
 *===========================================================*/

typedef struct {
    float temperature;
    float humidity;
    float pressure;
} bme280_data_t;

/*===========================================================
 * BME280 接口函数
 *===========================================================*/

int bme280_init(driver_t* drv, const bme280_config_t* config);
int bme280_read(driver_t* drv, bme280_data_t* data);
int bme280_reset(driver_t* drv);

extern driver_t drv_bme280;

#ifdef __cplusplus
}
#endif

#endif /* BME280_H */
