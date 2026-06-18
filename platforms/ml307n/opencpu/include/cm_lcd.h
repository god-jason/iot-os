/**
 * @file        cm_lcd.h
 * @brief       内存管理
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By sr
 * @date        2023/08/17
 *
 * @defgroup lcd
 * @ingroup lcd
 *
 * @deprecated 不建议使用该头文件，该接口仅用于老版本兼容，推荐使用cm_spi_lcd.h头文件。
 *
 *
 * @{
 */

#ifndef __CM_LCD_H__
#define __CM_LCD_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "stdint.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define LCD_SPI_DCX_VAL       0
#define LCD_READ_DMY_CYC_VAL  9
#define LCD_SPI_BIDIR_VAL     0

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief 配置时发送cmd
 *
 * @param [in] cmd 需要发送的命令
 *
 * @return
 *
 * @details More details
 */
void cm_lcd_write_cmd(unsigned char cmd);

/**
 * @brief 配置时发送data
 *
 * @param [in] data 需要发送的数据
 *
 * @return
 *
 * @details More details
 */
void cm_lcd_write_data(unsigned char data);

/**
 * @brief 发送刷屏数据
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
 * @brief lcd 驱动初始化
 *
 *
 * @return
 *
 * @details More details
 */
void cm_lcd_init(void);

/**
 * @brief lcd 驱动去初始化
 *
 *
 * @return
 *
 * @details More details
 */
void cm_lcd_deinit(void);

/**
 * @brief lcd 获取ID
 *
 *
 * @return
 *   > 0 - lcd ID \n
 *   = 0 - 失败
 *
 * @details 只适配ST7735S
 */
uint32_t cm_lcd_read_id(void);

/**
 * @brief lcd 发送cmd和data
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


#endif
