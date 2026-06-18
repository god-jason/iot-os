/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        chip_dma_req.h
 *
 * @brief       芯片DMA硬通道请求定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21   ict team        创建
 ************************************************************************************
 */

#ifndef _CHIP_DMA_REQ
#define _CHIP_DMA_REQ

/**
 * @addtogroup DrvCommon
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define DMA_REQ_UART0_TX                          0
#define DMA_REQ_UART0_RX                          1
#define DMA_REQ_LCD                               2
#define DMA_REQ_SPI_CAM_TX                        3
#define DMA_REQ_SPI_CAM_RX                        4
#define DMA_REQ_I2S_RXF0                          5
#define DMA_REQ_I2S_RXF1                          6
#define DMA_REQ_I2S_TXF                           7
#define DMA_REQ_USIM                              8
#define DMA_REQ_GP_SSP_TX                         9
#define DMA_REQ_GP_SSP_RX                         10
#define DMA_REQ_SBY_LPUART_TX                     11
#define DMA_REQ_SBY_LPUART_RX                     12
#define DMA_REQ_XIP_SFC_TX                        13
#define DMA_REQ_XIP_SFC_RX                        14
#define DMA_REQ_PWM                               15
#define DMA_REQ_UART2_TX                          16
#define DMA_REQ_UART2_RX                          17
#define DMA_REQ_DAC                               18
#define DMA_REQ_UART3_TX                          19
#define DMA_REQ_UART3_RX                          20
#define DMA_REQ_SSP1_TX                           21
#define DMA_REQ_SSP1_RX                           22

#endif   // _CHIP_DMA_REQ
/** @} */