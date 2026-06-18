/**
 * @file iot_spi.h
 * @brief ML307N 平台 SPI 适配器头文件
 * @details 基于 cm_spi 接口实现 SPI 功能封装，
 *          定义统一的 SPI 类型，支持跨平台编译。
 */

#ifndef IOT_SPI_ML307N_H
#define IOT_SPI_ML307N_H

#include "cm_spi.h"
#include "../../iot_types.h"

/* ============================================================
 * 统一 SPI 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief SPI模式枚举（只有两个值，保持原样） */
typedef enum {
    IOT_SPI_MODE_MASTER = 0,         /**< 主机模式 */
    IOT_SPI_MODE_SLAVE  = 1,         /**< 从机模式 */
} iot_spi_mode_t;

/** @brief SPI时钟极性枚举（只有两个值，保持原样） */
typedef enum {
    IOT_SPI_CPOL_LOW  = 0,           /**< 时钟低电平空闲 */
    IOT_SPI_CPOL_HIGH = 1,           /**< 时钟高电平空闲 */
} iot_spi_cpol_t;

/** @brief SPI时钟相位枚举（只有两个值，保持原样） */
typedef enum {
    IOT_SPI_CPHA_EDGE1 = 0,          /**< 第一个边沿采样 */
    IOT_SPI_CPHA_EDGE2 = 1,          /**< 第二个边沿采样 */
} iot_spi_cpha_t;

/** @brief SPI配置结构体 */
typedef struct {
    iot_spi_mode_t mode;              /**< SPI模式 */
    iot_spi_cpol_t cpol;              /**< 时钟极性 */
    iot_spi_cpha_t cpha;              /**< 时钟相位 */
    uint32_t freq;                    /**< 通信频率(Hz) */
} iot_spi_config_t;

/* ============================================================
 * SPI 句柄类型
 * ============================================================ */

/** @brief SPI句柄类型 */
typedef int iot_spi_t;

/* ============================================================
 * SPI 操作函数声明
 * ============================================================ */

/**
 * @brief 初始化SPI
 * @param[in] spi SPI端口
 * @param[in] config SPI配置参数
 * @return 0 成功
 */
int iot_spi_init(iot_spi_t spi, const iot_spi_config_t *config);

/**
 * @brief 去初始化SPI
 * @param[in] spi SPI端口
 * @return 0 成功
 */
int iot_spi_deinit(iot_spi_t spi);

/**
 * @brief SPI发送数据
 * @param[in] spi SPI端口
 * @param[in] data 发送数据缓冲区
 * @param[in] len 数据长度
 * @return 实际发送长度
 */
int iot_spi_send(iot_spi_t spi, const void *data, uint32_t len);

/**
 * @brief SPI接收数据
 * @param[in] spi SPI端口
 * @param[out] data 接收数据缓冲区
 * @param[in] len 数据长度
 * @return 实际接收长度
 */
int iot_spi_recv(iot_spi_t spi, void *data, uint32_t len);

/**
 * @brief SPI同时发送和接收数据
 * @param[in] spi SPI端口
 * @param[in] tx_data 发送数据缓冲区
 * @param[in] tx_len 发送数据长度
 * @param[out] rx_data 接收数据缓冲区
 * @param[in] rx_len 接收数据长度
 * @return 实际传输长度
 */
int iot_spi_transfer(iot_spi_t spi, const void *tx_data, uint32_t tx_len, void *rx_data, uint32_t rx_len);

#endif /* IOT_SPI_ML307N_H */