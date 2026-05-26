/**
 * @file        cm_spi.h
 * @brief       SPI驱动接口
 * @copyright   Copyright @2021 China Mobile IOT. All rights reserved.
 * @author      By tw
 * @date        2021/05/18
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

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/** 时钟频率 *//*仅支持以下时钟频率，不可任意更改*/
#define    CM_SPI_CLK_812_5KHZ     (812500U)  /*not support */
#define    CM_SPI_CLK_1_625MHZ     (1625000U) /*not support */
#define    CM_SPI_CLK_3_25MHZ      (3250000U) /*not support */
#define    CM_SPI_CLK_6_5MHZ       (6500000U) /*not support */
#define    CM_SPI_CLK_13MHZ        (13000000U)
#define    CM_SPI_CLK_26MHZ        (26000000U)
#define    CM_SPI_CLK_39MHZ        (39000000U)
#define    CM_SPI_CLK_52MHZ        (52000000U)/*not support */


/****************************************************************************
 * Public Types
 ****************************************************************************/

/** 设备ID */
typedef enum
{
    CM_SPI_DEV_0, /*!< 设备1*/
    CM_SPI_DEV_1, /* 暂不支持 */
    CM_SPI_DEV_NUM
} cm_spi_dev_e;

/** SPI主从模式 */
typedef enum
{
  CM_SPI_MODE_SLAVE,    /*!< SPI外设从机模式 */
  CM_SPI_MODE_MASTER     /*!< SPI外设主机模式*/
} cm_spi_mode_e;

/** 模式 */
typedef enum
{
    CM_SPI_WOKR_MODE_0 ,/*!< CPOL = 0,CPHA = 0*/
    CM_SPI_WOKR_MODE_1,/*!< CPOL = 0,CPHA = 1*/
    CM_SPI_WOKR_MODE_2 ,/*!< CPOL = 1,CPHA = 0,该模式下建议配置时钟低于13MHZ或提高传输数据宽度，否则可能收发数据不连续*/
    CM_SPI_WOKR_MODE_3/*!< CPOL = 1,CPHA = 1,该模式下建议配置时钟低于13MHZ或提高传输数据宽度，否则可能收发数据不连续*/
} cm_spi_work_mode_e;

/** SPI传输数据宽度 */
typedef enum
{
    CM_SPI_DATA_WIDTH_8BIT,/*!< SPI外设传输数据宽度8位    */
    CM_SPI_DATA_WIDTH_16BIT,/*!< SPI外设传输数据宽度16位，请注意字节对齐，传输字节数需为16bit倍数   */
    CM_SPI_DATA_WIDTH_32BIT /*!< SPI外设传输数据宽度32位，请注意字节对齐，传输字节数需为32bit倍数*/
} cm_spi_data_width_e;

/** SPI的NSS管理模式 */
typedef enum
{
  CM_SPI_NSS_SOFT, /*!< SPI外设软件控制NSS，暂不支持*/
  CM_SPI_NSS_HARD, /*!< SPI外设硬件控制NSS*/
} cm_spi_nss_e;

/** 配置 */
typedef struct
{
    cm_spi_mode_e mode;  /*!< SPI主从模式 */
    cm_spi_work_mode_e work_mode; /*!< 模式 */
    cm_spi_data_width_e data_width;  /*!< SPI传输数据宽度 */
    cm_spi_nss_e nss;/*!< SPI的NSS管理模式 */
    int clk; /*!< 时钟频率*/
} cm_spi_cfg_t;

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
 * @brief spi开启指定通道
 *
 * @param [in] dev spi设备
 * @param [in] config spi配置
 *
 * @return  
 *   = 0  - 成功. \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details 建议在mode2/3模式下配置clk低于13MHZ，否则可能收发数据不连续。
 */
int32_t cm_spi_open(cm_spi_dev_e dev, cm_spi_cfg_t *cfg);

/**
 * @brief spi关闭，去初始化
 *
 * @param [in] dev spi设备
 *
 * @return  
 *   = 0  - 成功. \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details More details
 * 
 */
int32_t cm_spi_close(cm_spi_dev_e dev);

/**
 * @brief spi写数据 
 * 
 * @param [in] dev spi设备
 * @param [in] data 待写数据
 * @param [in] len  待写数据长度
 * @return  
 *    = 实际写入长度 - 成功 \n
 *   < 0  - 失败, 返回值为错误码.
 *
 * @details 单次写数据不超过1024字节。
 * 
 */
int32_t cm_spi_write(cm_spi_dev_e dev, const void *data, uint32_t len);

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
 *  @details 单次读取数据不超过1024字节。
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
 *  @details 单次读写长度之和不能超过1024字节。
 */
int32_t cm_spi_write_then_read(cm_spi_dev_e dev, const void *w_data, uint32_t w_len, void *r_data, uint32_t r_len);

#undef EXTERN
#ifdef __cplusplus
}
#endif


#endif /* __CM_SPI_H__ */

/** @}*/
