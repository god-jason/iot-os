/**
 * @file iot_i2c.c
 * @brief ML307N 平台 I2C 适配器实现
 * @details 基于 cm_i2c 接口实现 I2C 功能封装，
 *          支持枚举类型参数转换。
 */

#include "iot_i2c.h"

/* ============================================================
 * I2C 操作函数实现
 * ============================================================ */

/**
 * @brief 打开I2C设备
 * @param[in] i2c I2C设备
 * @param[in] config I2C配置参数
 * @return 0 成功
 */
int iot_i2c_open(iot_i2c_t i2c, const iot_i2c_config_t *config)
{
    cm_i2c_cfg_t i2c_config = {0};
    
    if (config == NULL) {
        return IOT_ERR_PARAM;
    }
    
    /* 配置参数转换 */
    i2c_config.addr_type = (cm_i2c_addr_type_e)config->addr_type;
    i2c_config.mode = (cm_i2c_mode_e)config->mode;
    i2c_config.clk = config->freq;
    
    /* 调用平台打开 */
    return cm_i2c_open_v2((cm_i2c_dev_e)i2c, &i2c_config);
}

/**
 * @brief 关闭I2C设备
 * @param[in] i2c I2C设备
 * @return 0 成功
 */
int iot_i2c_close(iot_i2c_t i2c)
{
    return cm_i2c_close((cm_i2c_dev_e)i2c);
}

/**
 * @brief I2C写数据
 * @param[in] i2c I2C设备
 * @param[in] slave_addr 从设备地址
 * @param[in] data 待写数据
 * @param[in] len 数据长度
 * @return >=0 实际写入长度，<0 失败
 */
int32_t iot_i2c_write(iot_i2c_t i2c, uint16_t slave_addr, const void *data, uint32_t len)
{
    return cm_i2c_write_v2((cm_i2c_dev_e)i2c, slave_addr, data, len);
}

/**
 * @brief I2C读数据
 * @param[in] i2c I2C设备
 * @param[in] slave_addr 从设备地址
 * @param[out] data 读数据缓存
 * @param[in] len 读取长度
 * @return >=0 实际读出长度，<0 失败
 */
int32_t iot_i2c_read(iot_i2c_t i2c, uint16_t slave_addr, void *data, uint32_t len)
{
    return cm_i2c_read_v2((cm_i2c_dev_e)i2c, slave_addr, data, len);
}

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
                                 void *r_data, uint32_t r_len)
{
    return cm_i2c_write_then_read((cm_i2c_dev_e)i2c, slave_addr, 
                                  w_data, w_len, r_data, r_len);
}