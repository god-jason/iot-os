/**
 * @file iot_i2c.h
 * @brief ML307N 平台 I2C 适配器头文件
 * @details 基于 cm_i2c 接口实现 I2C 功能封装，
 *          定义统一的 I2C 类型，支持跨平台编译。
 */

#ifndef IOT_I2C_ML307N_H
#define IOT_I2C_ML307N_H

#include "cm_i2c.h"

/* ============================================================
 * 统一 I2C 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief I2C设备类型 */
typedef cm_i2c_dev_e iot_i2c_t;

/** @brief I2C地址类型枚举（只有两个值，保持原样） */
typedef enum {
    IOT_I2C_ADDR_TYPE_7BIT = 0,    /**< 7bit地址 */
    IOT_I2C_ADDR_TYPE_10BIT = 1,   /**< 10bit地址 */
} iot_i2c_addr_type_t;

/** @brief I2C模式枚举（只有两个值，保持原样） */
typedef enum {
    IOT_I2C_MODE_MASTER = 0,       /**< 主模式 */
    IOT_I2C_MODE_SLAVE = 1,       /**< 从模式 */
} iot_i2c_mode_t;

/** @brief I2C配置结构体 */
typedef struct {
    iot_i2c_addr_type_t addr_type; /**< 地址类型 */
    iot_i2c_mode_t mode;           /**< 工作模式 */
    uint32_t freq;                 /**< 时钟频率(Hz) */
} iot_i2c_config_t;

/* ============================================================
 * I2C 操作函数声明
 * ============================================================ */

/**
 * @brief 打开I2C设备
 * @param[in] i2c I2C设备
 * @param[in] config I2C配置参数
 * @return 0 成功
 */
int iot_i2c_open(iot_i2c_t i2c, const iot_i2c_config_t *config);

/**
 * @brief 关闭I2C设备
 * @param[in] i2c I2C设备
 * @return 0 成功
 */
int iot_i2c_close(iot_i2c_t i2c);

/**
 * @brief I2C写数据
 * @param[in] i2c I2C设备
 * @param[in] slave_addr 从设备地址
 * @param[in] data 待写数据
 * @param[in] len 数据长度
 * @return >=0 实际写入长度，<0 失败
 */
int32_t iot_i2c_write(iot_i2c_t i2c, uint16_t slave_addr, const void *data, uint32_t len);

/**
 * @brief I2C读数据
 * @param[in] i2c I2C设备
 * @param[in] slave_addr 从设备地址
 * @param[out] data 读数据缓存
 * @param[in] len 读取长度
 * @return >=0 实际读出长度，<0 失败
 */
int32_t iot_i2c_read(iot_i2c_t i2c, uint16_t slave_addr, void *data, uint32_t len);

/**
 * @brief I2C先写后读
 * @param[in] i2c I2C设备
 * @param[in] slave_addr 从设备地址
 * @param[in] w_data 待写数据
 * @param[in] w_len 待写数据长度
 * @param[out] r_data 读数据缓存
 * @param[in] r_len 读取数据长度
 * @return >=0 实际读出长度，<0 失败
 */
int32_t iot_i2c_write_then_read(iot_i2c_t i2c, uint16_t slave_addr,
                                 const void *w_data, uint32_t w_len,
                                 void *r_data, uint32_t r_len);

#endif /* IOT_I2C_ML307N_H */