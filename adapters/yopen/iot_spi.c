/**
 * @file iot_spi.c
 * @brief YOPEN 平台 SPI 适配器实现
 * @details 基于 yopen_spi 接口实现 SPI 功能封装，
 *          支持跨平台编译。
 */

#include "iot_spi.h"

/* ============================================================
 * 类型转换函数
 * ============================================================ */

/**
 * @brief SPI模式转平台枚举
 * @param[in] mode SPI模式
 * @return 平台SPI模式枚举值
 */
static yopen_spi_mode_e iot_spi_mode_to_platform(iot_spi_mode_t mode)
{
    return (mode == IOT_SPI_MODE_MASTER) ? YOPEN_SPI_MODE_MASTER : YOPEN_SPI_MODE_SLAVE;
}

/**
 * @brief SPI时钟极性转平台枚举
 * @param[in] cpol 时钟极性
 * @return 平台时钟极性枚举值
 */
static yopen_spi_cpol_e iot_spi_cpol_to_platform(iot_spi_cpol_t cpol)
{
    return (cpol == IOT_SPI_CPOL_HIGH) ? YOPEN_SPI_CPOL_HIGH : YOPEN_SPI_CPOL_LOW;
}

/**
 * @brief SPI时钟相位转平台枚举
 * @param[in] cpha 时钟相位
 * @return 平台时钟相位枚举值
 */
static yopen_spi_cpha_e iot_spi_cpha_to_platform(iot_spi_cpha_t cpha)
{
    return (cpha == IOT_SPI_CPHA_EDGE2) ? YOPEN_SPI_CPHA_EDGE2 : YOPEN_SPI_CPHA_EDGE1;
}

/* ============================================================
 * SPI 操作函数实现
 * ============================================================ */

/**
 * @brief 初始化SPI
 * @param[in] spi SPI端口
 * @param[in] config SPI配置参数
 * @return 0 成功
 */
iot_err_t iot_spi_init(iot_spi_t spi, const iot_spi_config_t *config)
{
    yopen_spi_config_s spi_config = {0};
    
    if (config == NULL) {
        return IOT_ERR_PARAM;
    }
    
    /* 配置参数转换 */
    spi_config.mode = iot_spi_mode_to_platform(config->mode);
    spi_config.cpol = iot_spi_cpol_to_platform(config->cpol);
    spi_config.cpha = iot_spi_cpha_to_platform(config->cpha);
    spi_config.freq = config->freq;
    
    /* 调用平台初始化 */
    yopen_spi_set_config(spi, &spi_config);
    return yopen_spi_open(spi);
}

/**
 * @brief 去初始化SPI
 * @param[in] spi SPI端口
 * @return 0 成功
 */
iot_err_t iot_spi_deinit(iot_spi_t spi)
{
    return yopen_spi_close(spi);
}

/**
 * @brief SPI发送数据
 * @param[in] spi SPI端口
 * @param[in] data 发送数据缓冲区
 * @param[in] len 数据长度
 * @return 实际发送长度
 */
int iot_spi_send(iot_spi_t spi, const void *data, uint32_t len)
{
    return yopen_spi_write(spi, (const uint8_t *)data, len, YOPEN_WAIT_FOREVER);
}

/**
 * @brief SPI接收数据
 * @param[in] spi SPI端口
 * @param[out] data 接收数据缓冲区
 * @param[in] len 数据长度
 * @return 实际接收长度
 */
int iot_spi_recv(iot_spi_t spi, void *data, uint32_t len)
{
    return yopen_spi_read(spi, (uint8_t *)data, len, YOPEN_WAIT_FOREVER);
}

/**
 * @brief SPI同时发送和接收数据
 * @param[in] spi SPI端口
 * @param[in] tx_data 发送数据缓冲区
 * @param[in] tx_len 发送数据长度
 * @param[out] rx_data 接收数据缓冲区
 * @param[in] rx_len 接收数据长度
 * @return 实际传输长度
 */
int iot_spi_transfer(iot_spi_t spi, const void *tx_data, uint32_t tx_len, void *rx_data, uint32_t rx_len)
{
    return yopen_spi_transfer(spi, (const uint8_t *)tx_data, tx_len, 
                              (uint8_t *)rx_data, rx_len, YOPEN_WAIT_FOREVER);
}