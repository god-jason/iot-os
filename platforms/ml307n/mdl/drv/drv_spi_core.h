#ifndef _DRV_SPI_CORE_H
#define _DRV_SPI_CORE_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include "drv_common.h"

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/
/**
 * @addtogroup SpiCore
 */

/**@{*/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/***********************************************************************************************/

//                                       SPI通用配置

/***********************************************************************************************/

/*** 控制器ID bits[1:0] ****/
#define SPI_BUS_NUMBER_Pos          (0)         // bits[1:0]
#define SPI_BUS_NUMBER_Len          (2)
#define SPI_BUS_NUMBER_Msk          (0x3UL << SPI_BUS_NUMBER_Pos)
#define SPI_BUS_NUMBER(n)           (((n) & 0x3UL) << SPI_BUS_NUMBER_Pos)

/*** 时钟源 bits[4:2] ****/
#define SPI_CLK_SRC_Pos             (SPI_BUS_NUMBER_Pos + SPI_BUS_NUMBER_Len)
#define SPI_CLK_SRC_Len             (3)
#define SPI_CLK_SRC_Msk             (0x7UL << SPI_CLK_SRC_Pos)
#define SPI_CLK_SRC_13M             (0x1UL << SPI_CLK_SRC_Pos)
#define SPI_CLK_SRC_26M             (0x2UL << SPI_CLK_SRC_Pos)
#define SPI_CLK_SRC_39M             (0x3UL << SPI_CLK_SRC_Pos)
#define SPI_CLK_SRC_78M             (0x4UL << SPI_CLK_SRC_Pos)

/*** 时钟分频系数 bits[8:5] ****/
#define SPI_CLK_DIV_Pos             (SPI_CLK_SRC_Pos + SPI_CLK_SRC_Len)
#define SPI_CLK_DIV_Len             (4)
#define SPI_CLK_DIV_Msk             (0xfUL << SPI_CLK_DIV_Pos)
#define SPI_CLK_DIV(n)              (((n) & 0xFUL) << SPI_CLK_DIV_Pos)

/*** 主机模式/从机模式 bits[10:9] ****/
#define SPI_MS_MODE_Pos             (SPI_CLK_DIV_Pos + SPI_CLK_DIV_Len)
#define SPI_MS_MODE_Len             (2)
#define SPI_MS_MODE_Msk             (0x3UL << SPI_MS_MODE_Pos)
#define SPI_MS_MODE_MASTER          (0x1UL << SPI_MS_MODE_Pos)
#define SPI_MS_MODE_SLAVE           (0x2UL << SPI_MS_MODE_Pos)

/*** pha & pol 配置 bits[13:11] ****/
#define SPI_POLPHA_Pos              (SPI_MS_MODE_Pos + SPI_MS_MODE_Len)
#define SPI_POLPHA_Len              (3)
#define SPI_POLPHA_Msk              (0x7UL << SPI_POLPHA_Pos)
#define SPI_POLPHA_00               (0x1UL << SPI_POLPHA_Pos)
#define SPI_POLPHA_01               (0x2UL << SPI_POLPHA_Pos)
#define SPI_POLPHA_10               (0x3UL << SPI_POLPHA_Pos)
#define SPI_POLPHA_11               (0x4UL << SPI_POLPHA_Pos)

/*** 数据位宽（主机和从机需要对应） bits[19:14] ****/
#define SPI_DATA_BITS_Pos           (SPI_POLPHA_Pos + SPI_POLPHA_Len)
#define SPI_DATA_BITS_Len           (6)
#define SPI_DATA_BITS_Msk           (0x3FUL << SPI_DATA_BITS_Pos)
#define SPI_DATA_BITS(n)            (((n) & 0x3FUL) << SPI_DATA_BITS_Pos)

/*** 传输方式 bits[22:20] ****/
#define SPI_TRANS_MODE_Pos          (SPI_DATA_BITS_Pos + SPI_DATA_BITS_Len)
#define SPI_TRANS_MODE_Len          (3)
#define SPI_TRANS_MODE_Msk          (0x7UL << SPI_TRANS_MODE_Pos)
#define SPI_TRANS_MODE_POLL         (0x1UL << SPI_TRANS_MODE_Pos)
#define SPI_TRANS_MODE_INT          (0x2UL << SPI_TRANS_MODE_Pos)
#define SPI_TRANS_MODE_DAM          (0x3UL << SPI_TRANS_MODE_Pos)
#define SPI_TRANS_MODE_DAM_POLL     (0x4UL << SPI_TRANS_MODE_Pos)
#define SPI_TRANS_MODE_POLL_DMA     (0x5UL << SPI_TRANS_MODE_Pos)
#define SPI_TRANS_MODE_HDLC         (0x6UL << SPI_TRANS_MODE_Pos)

/*** CS由软件控制还是硬件自动控制 bits[24:23] ****/
#define SPI_CS_MODE_Pos             (SPI_TRANS_MODE_Pos + SPI_TRANS_MODE_Len)
#define SPI_CS_MODE_Len             (2)
#define SPI_CS_MDOE_Msk             (0x3UL << SPI_CS_MODE_Pos)
#define SPI_CS_MDOE_AUTO            (0x1UL << SPI_CS_MODE_Pos)
#define SPI_CS_MDOE_MANUAL          (0x2UL << SPI_CS_MODE_Pos)



/***********************************************************************************************/

//                                       SPI 传输参数配置

/***********************************************************************************************/

/*** TX 阈值 bits[4:0] ****/
#define SPI_TX_THRES_Pos            (0)
#define SPI_TX_THRES_Len            (5)
#define SPI_TX_THRES_Msk            (0x1FUL << SPI_TX_THRES_Pos)
#define SPI_TX_THRES_VALUE(n)       (((n) & 0x1FUL) << SPI_TX_THRES_Pos)

/*** TX 阈值 bits[10:5] ****/
#define SPI_RX_THRES_Pos            (SPI_TX_THRES_Pos + SPI_TX_THRES_Len)
#define SPI_RX_THRES_Len            (5)
#define SPI_RX_THRES_Msk            (0x1FUL << SPI_RX_THRES_Pos)
#define SPI_RX_THRES_VALUE(n)       (((n) & 0x1FUL) << SPI_RX_THRES_Pos)

/*** RX dummy时发送的默认值 bits[31:24] ****/
#define SPI_TX_VALUE_Pos            (SPI_RX_THRES_Pos + SPI_RX_THRES_Len)
#define SPI_TX_VALUE_Len            (8)
#define SPI_TX_VALUE_Msk            (0xFFUL << SPI_TX_VALUE_Pos)
#define SPI_TX_VALUE(n)             (((n) & 0xFFUL) << SPI_TX_VALUE_Pos)

/***********************************************************************************************/

//                                       SPI 错误码

/***********************************************************************************************/
/****** 错误码 *****/
#define SPI_ERROR_MODE              (DRV_ERR_SPECIFIC - 1)                  // 不支持该模式
#define SPI_ERROR_FRAME_FORMAT      (DRV_ERR_SPECIFIC - 2)                  // 不支持该数据帧格式
#define SPI_ERROR_DATA_BITS         (DRV_ERR_SPECIFIC - 3)                  // 不支持该数据位宽
#define SPI_ERROR_BIT_ORDER         (DRV_ERR_SPECIFIC - 4)                  // 不支持指定的位顺序
#define SPI_ERROR_SS_MODE           (DRV_ERR_SPECIFIC - 5)                  // 不支持指定的slave选择模式
#define SPI_ERROR_TRANS_MODE        (DRV_ERR_SPECIFIC - 6)                  // 不支持指定的slave选择模式


#endif
/** @} */