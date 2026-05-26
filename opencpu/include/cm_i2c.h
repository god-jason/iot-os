/**
 * @file        cm_i2c.h
 * @brief       I2C驱动接口
 * @copyright   Copyright @2021 China Mobile IOT. All rights reserved.
 * @author      By tw
 * @date        2021/05/18
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


/** 时钟频率 *//*仅支持以下时钟频率，不可任意更改*/
#define    CM_I2C_CLK_100KHZ          (100000U)
#define    CM_I2C_CLK_400KHZ          (400000U)
#define    CM_I2C_CLK_HS_KHZ          1 /*特定值，表示从节点传输速率为 1Mbps*/
#define    CM_I2C_CLK_HS_FAST_KHZ     2 /*特定值，表示从节点传输速率为 3.4 Mbps*/


/****************************************************************************
 * Public Types
 ****************************************************************************/


/**
 *  @brief I2C 通道
 */
typedef enum
{
    CM_I2C_DEV_0 = 0,   /*!< I2C0*/
    CM_I2C_DEV_1,       /*!< I2C1 */
    CM_I2C_DEV_2,       /*!< I2C2 */
    CM_I2C_DEV_NUM,     /*!< I2C设备个数 */
} cm_i2c_dev_e;

/**
 *  @brief I2C 支持主从模式
 */
typedef enum
{
    CM_I2C_MODE_MASTER = 0,   /*!< 主模式*/
    CM_I2C_MODE_SLAVER,       /*!< 从模式*/
} cm_i2c_mode_e;

/** 寻址 */
typedef enum
{
    CM_I2C_ADDR_TYPE_7BIT,     /*!< 7bit地址*/
    CM_I2C_ADDR_TYPE_10BIT,    /*!< 10bit地址*/
} cm_i2c_addr_type_e;

/**
 *  @brief I2C 配置
 */
typedef struct
{
    cm_i2c_addr_type_e addr_type;
    cm_i2c_mode_e  mode;
    int  clk;
} cm_i2c_cfg_t;

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * @brief i2c开启
 *
 * @param [in]  dev i2c设备ID
 * @param [in]  cfg i2c配置
 * @return
 *   = 0  - 成功. \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details 仅支持主机模式.
 */
int32_t cm_i2c_open(cm_i2c_dev_e dev, cm_i2c_cfg_t *cfg);

/**
 * @brief i2c关闭
 *
 * @param [in]  dev i2c设备ID
 * @return
 *   = 0  - 成功. \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details More details
 */

int32_t cm_i2c_close(cm_i2c_dev_e dev);

/**
 * @brief i2c写数据
 *
 * @param [in]  dev i2c设备
 * @param [in]  slave_addr i2c设备地址
 * @param [in]  data 待写数据
 * @param [in]  len 待写数据长度
 * @return
 *   = 实际写入长度 - 成功. \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details More details
 */
int32_t cm_i2c_write(cm_i2c_dev_e dev, uint16_t slave_addr, uint8_t *data, uint32_t len);

/**
 * @brief i2c读数据
 *
 * @param [in]  dev i2c设备
 * @param [in]  slave_addr i2c设备地址
 * @param [out]  data 读取数据存放地址，需先申请内存
 * @param [in]  len 读取数据长度
 * @return
 *   = 实际读出长度 - 成功. \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details More details
 */
int32_t cm_i2c_read(cm_i2c_dev_e dev, uint16_t slave_addr, uint8_t *data, uint32_t len);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_I2C_H__ */

/** @}*/
