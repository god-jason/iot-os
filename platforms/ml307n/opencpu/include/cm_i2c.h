/**
 * @file        cm_i2c.h
 * @brief       I2C接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By ZHANGXW
 * @date        2021/03/09
 *
 * @defgroup i2c i2c
 * @ingroup PI
 * @{
 */

#ifndef __CM_I2C_H__
#define __CM_I2C_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/** 时钟频率 */
#define I2C_CLK_100KHZ (100000U)
#define I2C_CLK_200KHZ (200000U)
#define I2C_CLK_400KHZ (400000U)
#define I2C_CLK_1MHZ   (1000000U)
#define I2C_CLK_3M4HZ  (3400000U)


#define    CM_I2C_CLK_100KHZ          (100000U)
#define    CM_I2C_CLK_400KHZ          (400000U)
#define    CM_I2C_CLK_HS_KHZ          1 /*特定值，表示从节点传输速率为 1Mbps*/
#define    CM_I2C_CLK_HS_FAST_KHZ     2 /*特定值，表示从节点传输速率为 3.4 Mbps*/
/****************************************************************************
 * Public Types
 ****************************************************************************/

/** 设备ID */
typedef enum
{
    CM_I2C_DEV_0, /*!< 设备1*/
    CM_I2C_DEV_1, /*!< 设备2*/
    CM_I2C_DEV_2, /*!< 设备3*/
    CM_I2C_DEV_NUM
} cm_i2c_dev_e;

/** 寻址 */
typedef enum
{
    CM_I2C_ADDR_TYPE_7BIT,  /*!< 7bit地址*/
    CM_I2C_ADDR_TYPE_10BIT, /*!< 10bit地址*/
} cm_i2c_addr_type_e;

/** @brief I2C 支持主从模式*/
typedef enum
{
    CM_I2C_MODE_MASTER = 0, /**< 主模式 */
    CM_I2C_MODE_SLAVE,      /**< 从模式(不支持)*/
} cm_i2c_mode_e;

/** 配置 */
typedef struct
{
    cm_i2c_addr_type_e addr_type; /*!< 地址类型，7bit或10bit*/
    cm_i2c_mode_e mode;           /*!< 工作模式 */
    uint32_t clk;                 /*!< 时钟频率，单位Hz */
} cm_i2c_cfg_t;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/**
 *  @brief 打开I2C设备
 *
 *  @param [in] dev I2C设备ID，取值范围为cm_i2c_dev_e枚举值
 *  @param [in] cfg 配置参数，不能为NULL
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details 仅支持主机模式
 *
 * @deprecated 不建议使用cm_i2c_open，该接口仅用于老版本兼容，推荐使用cm_i2c_open_v2接口。
 */
int32_t cm_i2c_open(cm_i2c_dev_e dev, cm_i2c_cfg_t *cfg);
int32_t cm_i2c_open_v2(cm_i2c_dev_e dev, const cm_i2c_cfg_t *cfg);

/**
 *  @brief 关闭I2C设备
 *
 *  @param [in] dev I2C设备ID，取值范围为cm_i2c_dev_e枚举值
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_i2c_close(cm_i2c_dev_e dev);

/**
 *  @brief I2C写数据
 *
 *  @param [in] dev I2C设备ID
 *  @param [in] slave_addr 从设备地址
 *  @param [in] data 待写数据
 *  @param [in] len 长度
 *
 *  @return
 *    = 实际写入长度 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 *
 * @deprecated 不建议使用cm_i2c_write，该接口仅用于老版本兼容，推荐使用cm_i2c_write_v2接口。
 */
int32_t cm_i2c_write(cm_i2c_dev_e dev, uint16_t slave_addr, uint8_t *data, uint32_t len);
int32_t cm_i2c_write_v2(cm_i2c_dev_e dev, uint16_t slave_addr, const void *data, uint32_t len);

/**
 *  @brief I2C读数据
 *
 *  @param [in] dev I2C设备ID
 *  @param [in] slave_addr 从设备地址
 *  @param [out] data 读数据缓存
 *  @param [in] len 读取长度
 *
 *  @return
 *    = 实际读出长度 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 *
 * @deprecated 不建议使用cm_i2c_read，该接口仅用于老版本兼容，推荐使用cm_i2c_read_v2接口。
 */
int32_t cm_i2c_read(cm_i2c_dev_e dev, uint16_t slave_addr, uint8_t *data, uint32_t len);
int32_t cm_i2c_read_v2(cm_i2c_dev_e dev, uint16_t slave_addr, void *data, uint32_t len);

/**
 *  @brief I2C先写后读数据
 *
 *  @param [in] dev I2C设备ID
 *  @param [in] slave_addr 从设备地址
 *  @param [in] w_data 待写数据
 *  @param [in] w_len 待写数据长度
 *  @param [out] r_data 读数据缓存
 *  @param [in] r_len 读取数据长度
 *
 *  @return
 *    = 实际读出长度 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_i2c_write_then_read(cm_i2c_dev_e dev, uint16_t slave_addr, const void *w_data, uint32_t w_len, void *r_data, uint32_t r_len);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_I2C_H__ */

/** @}*/
