/**
 * @file        cm_spi_lcd.h
 * @brief       spi_lcd接口
 * @copyright   Copyright © 2025 China Mobile IOT. All rights reserved.
 * @author      By cmiot1325
 * @date        2025/03/25
 *
 * @defgroup spi_lcd spi_lcd
 * @ingroup lcd
 * @{
 */


#ifndef __CM_SPI_LCD_H__
#define __CM_SPI_LCD_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include "cm_gpio.h"
#include "cm_spi.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Public Types
 ****************************************************************************/

 /** spi_lcd配置 */
typedef struct {
    cm_spi_dev_e spi_dev;    /*!< 使用的SPI设备号 目前仅支持CM_SPI_DEV_2*/
    cm_gpio_num_e dcx_gpio;  /*!< DCX GPIO号 */
    cm_gpio_num_e rst_gpio;  /*!< RST GPIO号 */
} cm_spi_lcd_cfg_t;

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
 * @brief 发送命令
 *  
 * @param [in] cmd  命令数组
 * @param [in] len  命令长度
 *
 * @return 
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details 可用于连续发送多个命令，cmd传入命令数组，len传入要发送的命令个数，如果要发送单个命令，len传入1即可
            目前仅支持单命令发送，len需要传入1
 */
int32_t cm_spi_lcd_write_cmd(uint8_t *cmd, uint32_t len);

/**
 * @brief 发送数据
 *  
 * @param [in] data 需要发送的数据
 * @param [in] len  数据长度
 *
 * @return 
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details 可用于连续发送数据，data传入数据，len传入数据长度
            通过前一次cm_spi_lcd_write_cmd传入的命令来判断数据类型
            前一次传入命令为0x2C时，此函数传入的数据被认定为屏显数据
            前一次传入命令不为0x2C时，此函数传入的数据被认定为参数数据
 */
int32_t cm_spi_lcd_write_data(uint8_t *data, uint32_t len);

/**
 * @brief spi_lcd驱动初始化
 * 
 * @param [in] cfg  spi_lcd配置
 *
 * @return 
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details 对底层驱动进行初始化，初始化前请先完成对应GPIO引脚的功能复用
 */
int32_t cm_spi_lcd_init(cm_spi_lcd_cfg_t *cfg);

/**
 * @brief spi_lcd驱动去初始化
 * 
 * @return 
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details 释放底层驱动资源
 */
int32_t cm_spi_lcd_deinit(void);

/**
 * @brief spi_lcd获取ID
 *
 * @param [in]  cmd     读取id的命令数组
 * @param [in]  cmd_len 命令长度
 * @param [out] id_buf  保存id的缓存地址
 * @param [in]  buf_len 缓存buf长度
 *
 * @return 
 *    = 0 - 成功 \n
 *    < 0 - 失败
 *
 * @details cmd传入命令数组，若读取命令只有一个字节，则cmd_len传入1即可
            目前仅支持单命令发送，len需要传入1
 */
int32_t cm_spi_lcd_read_id(uint8_t *cmd, uint32_t cmd_len, uint8_t *id_buf, uint32_t buf_len);

/**
 * @brief 发送刷屏数据(旧接口暂时保留，不建议使用)
 *  
 * @param [in] data 需要发送的数据
 * @param [in] len  数据长度
 *
 * @return 
 *
 * @details More details
 */
void cm_lcd_write_buf(unsigned char *data, int len);

/**
 * @brief lcd 发送cmd和data(旧接口暂时保留，不建议使用)
 *
 * @param [in] cmd 需要发送的命令
 * @param [in] data 需要发送的数据
 * @param [in] len  数据长度
 *
 * @return 
 *
 * @details More details
 */
void cm_lcd_write_cmd_and_data(unsigned char cmd, unsigned char *data, int len);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_SPI_LCD_H__ */

/** @}*/
