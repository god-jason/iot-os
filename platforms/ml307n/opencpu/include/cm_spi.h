/**
 * @file        cm_spi.h
 * @brief       SPI接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By ZHANGXW
 * @date        2021/03/09
 *
 * @defgroup spi spi
 * @ingroup PI
 * @{
 */

#ifndef __CM_SPI_H__
#define __CM_SPI_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** 设备ID */
typedef enum{
    CM_SPI_DEV_0,              /*!< 设备1*/
    CM_SPI_DEV_1,              /*!< 设备2 暂不支持 */
    CM_SPI_DEV_2, 			   /*!<  LCD专用 */
    CM_SPI_DEV_NUM
} cm_spi_dev_e;

/** 模式 */
typedef enum{
    CM_SPI_WORK_MODE_0,             /*!< CPOL = 0,CPHA = 0*/
    CM_SPI_WORK_MODE_1,             /*!< CPOL = 0,CPHA = 1*/
    CM_SPI_WORK_MODE_2,             /*!< CPOL = 1,CPHA = 0*/
    CM_SPI_WORK_MODE_3              /*!< CPOL = 1,CPHA = 1*/
} cm_spi_work_mode_e;

/** SPI主从模式 */
typedef enum
{
  CM_SPI_MODE_SLAVE,    /*!< SPI外设从机模式 */
  CM_SPI_MODE_MASTER     /*!< SPI外设主机模式*/
} cm_spi_mode_e;


/** SPI传输数据宽度 */
typedef enum
{
    CM_SPI_DATA_WIDTH_8BIT,/*!< SPI外设传输数据宽度8位    */
    CM_SPI_DATA_WIDTH_16BIT,/*!< SPI外设传输数据宽度16位   */
    CM_SPI_DATA_WIDTH_32BIT /*!< SPI外设传输数据宽度32位,不支持   */
} cm_spi_data_width_e;

/** SPI的NSS管理模式 */
typedef enum
{
  CM_SPI_NSS_SOFT, /*!< SPI外设软件控制NSS,暂不支持*/
  CM_SPI_NSS_HARD, /*!< SPI外设硬件控制NSS*/
} cm_spi_nss_e;

/** 配置 */
typedef struct
{
    cm_spi_mode_e mode;  /*!< SPI主从模式 */
    cm_spi_work_mode_e work_mode; /*!< 工作模式 */
    cm_spi_data_width_e data_width;  /*!< SPI传输数据宽度 */
    cm_spi_nss_e nss;/*!< SPI的NSS管理模式 */
    int clk; /*!< 时钟频率,单位MHz,只支持13,26,39,78*/
} cm_spi_cfg_t;

/** 消息结构 */
typedef struct{
    bool cs_ctrl;       /*!< 是否在传输完成后失能片选信号 */
    void* w_data;       /*!< 待写入的数据缓冲区指针 */
    void* r_data;       /*!< 用于接收数据的缓冲区指针 */
    int w_len;          /*!< 待写入数据的长度，单位字节 */
    int r_len;          /*!< 期望读取数据的长度，单位字节 */
} cm_spi_msg_t;//ToDo：细化数据结构定义

typedef enum
{
    CM_SPI_LINE,              /*!< SPI线制设置，3：三线制；4 ：四线制，不支持三线*/
    CM_SPI_CLK,               /*!< 频率设置 */
    CM_SPI_WORK_MODE,         /*!< 工作模式 */
    CM_SPI_DATA_WIDTH,        /*!< 数据宽度*/
    CM_SPI_LCD_DC_GPIO,       /*!< 适配spi_lcd接口时使用，设置DC引脚GPIO号供内部使用,不支持 */
}cm_spi_cfg_type_e;

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
 *  @brief 打开spi设备
 *
 *  @param [in] dev SPI设备ID
 *  @param [in] cfg 配置
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_spi_open(cm_spi_dev_e dev, cm_spi_cfg_t *cfg);

/**
 *  @brief SPI写数据
 *
 *  @param [in] dev SPI设备ID
 *  @param [in] data 待写数据
 *  @param [in] len 长度
 *
 *  @return
 *    = 实际写入长度 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_spi_write(cm_spi_dev_e dev, const void *data, int32_t len);

/**
 *  @brief SPI读数据
 *
 *  @param [in] dev SPI设备ID
 *  @param [out] data 待读数据缓存
 *  @param [in] len 长度
 *
 *  @return
 *    = 实际读出长度 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_spi_read(cm_spi_dev_e dev, void *data, int32_t len);

/**
 *  @brief SPI写数据后读出数据
 *
 *  @param [in] dev SPI设备ID
 *  @param [in] w_data 待写数据
 *  @param [in] w_len 写入长度
 *  @param [out] r_data 待读缓存
 *  @param [in] r_len 读取长度
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_spi_write_then_read(cm_spi_dev_e dev, const void *w_data, int32_t w_len, void *r_data, int32_t r_len);

/**
 *  @brief 消息传输
 *
 *  @param [in] dev SPI设备ID
 *  @param [in] msg 消息体
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details More details
 */
int32_t cm_spi_transfer_msg(cm_spi_dev_e dev, cm_spi_msg_t *msg);

/**
 *  @brief 关闭spi设备
 *
 *  @param [in] dev SPI设备ID
 *
 *  @return void
 *
 *  @details More details
 */
void cm_spi_close(cm_spi_dev_e dev);

/**
 * @brief SPI设备参数设置
 *
 * @param [in] dev spi设备ID
 * @param [in] type 配置类型，详见cm_spi_cfg_type_e
 * @param [in] info 配置信息指针
 *
 * @return
 *   = 0  - 成功 \n
 *   <0  - 失败, 返回值为错误码
 *
 * @details info需传入对应类型结构体
 */
int32_t cm_spi_set_cfg(cm_spi_dev_e dev, cm_spi_cfg_type_e type, void *info);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_SPI_H__ */

/** @}*/
