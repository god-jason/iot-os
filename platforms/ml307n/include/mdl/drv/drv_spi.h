/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_spi.h
 *
 * @brief       SPI驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef _DRV_SPI_H
#define _DRV_SPI_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include "drv_common.h"
#include "drv_spi_core.h"
#include "drv_dma.h"

/**
 * @addtogroup Spi
 */

/**@{*/

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/*** Bit definition for [version] register(0x00000000) ****/
#define SPI_VER_H_Pos                     (24)
#define SPI_VER_H_Msk                     (0xFFUL << SPI_VER_H_Pos)
#define SPI_VER_H                         SPI_VER_H_Msk                     /*!<主版本号 */
#define SPI_VER_L_Pos                     (16)
#define SPI_VER_L_Msk                     (0xFFUL << SPI_VER_L_Pos)
#define SPI_VER_L                         SPI_VER_L_Msk                     /*!<次版本号 */


/*** Bit definition for [protocol] register(0x00000010) ****/
#define SPI_SLV_SW_SSN_Pos                (21)
#define SPI_SLV_SW_SSN_Msk                (0x1UL << SPI_PIN_MODE_Pos)
#define SPI_SLV_SW_SSN                    SPI_PIN_MODE_Msk                  /*!<0:LOW   1:HIGH */
#define SPI_SLV_SW_SSN_EN_Pos             (20)
#define SPI_SLV_SW_SSN_EN_Msk             (0x1UL << SPI_PIN_MODE_Pos)
#define SPI_SLV_SW_SSN_EN                 SPI_PIN_MODE_Msk                  /*!<0:MASTER 1:SOFT */
#define SPI_PIN_MODE_Pos                  (17)
#define SPI_PIN_MODE_Msk                  (0x1UL << SPI_PIN_MODE_Pos)
#define SPI_PIN_MODE                      SPI_PIN_MODE_Msk                  /*!<0:TXD/RXD 1:MOSI/MISO */
#define SPI_WORD_SIZE_Pos                 (12)
#define SPI_WORD_SIZE_Msk                 (0xFUL << SPI_WORD_SIZE_Pos)
#define SPI_WORD_SIZE                     SPI_WORD_SIZE_Msk                 /*!<word size */
#define SPI_DRDY_MOD_Pos                  (11)
#define SPI_DRDY_MOD_Msk                  (0x1UL << SPI_DRDY_MOD_Pos)
#define SPI_DRDY_MOD                      SPI_DRDY_MOD_Msk                  /*!<0：双功能输入  1：单功能输入 */
#define SPI_DRDY_EN_Pos                   (10)
#define SPI_DRDY_EN_Msk                   (0x1UL << SPI_DRDY_EN_Pos)
#define SPI_DRDY_EN                       SPI_DRDY_EN_Msk                   /*!<硬件流控 */
#define SPI_SLV_SEL_Pos                   (8)
#define SPI_SLV_SEL_Msk                   (0x3UL << SPI_SLV_SEL_Pos)
#define SPI_SLV_SEL                       SPI_SLV_SEL_Msk                   /*!<多片选 */
#define SPI_RTX_CTRL_Pos                  (6)
#define SPI_RTX_CTRL_Msk                  (0x3UL << SPI_RTX_CTRL_Pos)
#define SPI_RTX_CTRL                      SPI_RTX_CTRL_Msk                  /*!<0：TX Only 1：RX only  2：full duplex */
#define SPI_SPECULAT_Pos                  (5)
#define SPI_SPECULAT_Msk                  (0x1UL << SPI_SPECULAT_Pos)
#define SPI_SPECULAT                      SPI_SPECULAT_Msk                  /*!<是否使用投机提高接口速率 */
#define SPI_SCLK_STYLE_Pos                (4)
#define SPI_SCLK_STYLE_Msk                (0x1UL << SPI_SCLK_STYLE_Pos)
#define SPI_SCLK_STYLE                    SPI_SCLK_STYLE_Msk                /*!<时钟类型 */
#define SPI_CHPA_Pos                      (3)
#define SPI_CHPA_Msk                      (0x1UL << SPI_CHPA_Pos)
#define SPI_CHPA                          SPI_CHPA_Msk                      /*!<时钟相位 */
#define SPI_CPOL_Pos                      (2)
#define SPI_CPOL_Msk                      (0x1UL << SPI_CPOL_Pos)
#define SPI_CPOL                          SPI_CPOL_Msk                      /*!<时钟极性 */
#define SPI_LOOP_Pos                      (1)
#define SPI_LOOP_Msk                      (0x1UL << SPI_LOOP_Pos)
#define SPI_LOOP                          SPI_LOOP_Msk                      /*!<回环 */
#define SPI_MS_SEL_Pos                    (0)
#define SPI_MS_SEL_Msk                    (0x1UL << SPI_MS_SEL_Pos)
#define SPI_MS_SEL                        SPI_MS_SEL_Msk                    /*!<Master/Slave */


/*** Bit definition for [txCtrl] register(0x00000014) ****/
#define SPI_TX_QTY_Pos                    (0)
#define SPI_TX_QTY_Msk                    (0x1FFFFUL << SPI_TX_QTY_Pos)
#define SPI_TX_QTY                        SPI_TX_QTY_Msk                    /*!<发送数据量 */


/*** Bit definition for [rxCtrl] register(0x00000018) ****/
#define SPI_RX_QTY_MOD_Pos                (31)
#define SPI_RX_QTY_MOD_Msk                (0x1UL << SPI_RX_QTY_MOD_Pos)
#define SPI_RX_QTY_MOD                    SPI_RX_QTY_MOD_Msk                /*!<接收数据量模式 */
#define SPI_RX_QTY_Pos                    (0)
#define SPI_RX_QTY_Msk                    (0x1FFFFUL << SPI_RX_QTY_Pos)
#define SPI_RX_QTY                        SPI_RX_QTY_Msk                    /*!<接收数据量 */


/*** Bit definition for [slvRxCnt] register(0x0000001c) ****/
#define SPI_RX_CNT_Pos                    (0)
#define SPI_RX_CNT_Msk                    (0x1FFFFUL << SPI_RX_CNT_Pos)
#define SPI_RX_CNT                        SPI_RX_CNT_Msk                    /*!<Slave接收到的数据量 */


/*** Bit definition for [crcCtrl0] register(0x00000020) ****/
#define SPI_OUT_REF_Pos                   (5)
#define SPI_OUT_REF_Msk                   (0x1UL << SPI_OUT_REF_Pos)
#define SPI_OUT_REF                       SPI_OUT_REF_Msk                   /*!<output reflection */
#define SPI_IN_REF_Pos                    (4)
#define SPI_IN_REF_Msk                    (0x1UL << SPI_IN_REF_Pos)
#define SPI_IN_REF                        SPI_IN_REF_Msk                    /*!<input reflection */
#define SPI_RX_CRC16_EN_Pos               (1)
#define SPI_RX_CRC16_EN_Msk               (0x1UL << SPI_RX_CRC16_EN_Pos)
#define SPI_RX_CRC16_EN                   SPI_RX_CRC16_EN_Msk               /*!<rx crc16 enable */
#define SPI_TX_CRC16_EN_Pos               (0)
#define SPI_TX_CRC16_EN_Msk               (0x1UL << SPI_TX_CRC16_EN_Pos)
#define SPI_TX_CRC16_EN                   SPI_TX_CRC16_EN_Msk               /*!<tx crc16 enable */


/*** Bit definition for [crcCtrl1] register(0x00000024) ****/
#define SPI_OUT_XOR_Pos                   (16)
#define SPI_OUT_XOR_Msk                   (0xFFFFUL << SPI_OUT_XOR_Pos)
#define SPI_OUT_XOR                       SPI_OUT_XOR_Msk                   /*!<crc16 final xor value */
#define SPI_CRC_INIT_Pos                  (0)
#define SPI_CRC_INIT_Msk                  (0xFFFFUL << SPI_CRC_INIT_Pos)
#define SPI_CRC_INIT                      SPI_CRC_INIT_Msk                  /*!<crc16 initial value */


/*** Bit definition for [crcSta] register(0x00000028) ****/
#define SPI_RX_CRC_Pos                    (16)
#define SPI_RX_CRC_Msk                    (0xFFFFUL << SPI_RX_CRC_Pos)
#define SPI_RX_CRC                        SPI_RX_CRC_Msk                    /*!<接收CRC16的结果 */
#define SPI_TX_CRC_Pos                    (0)
#define SPI_TX_CRC_Msk                    (0xFFFFUL << SPI_TX_CRC_Pos)
#define SPI_TX_CRC                        SPI_TX_CRC_Msk                    /*!<发送CRC16的结果 */


/*** Bit definition for [crcDbg] register(0x0000002c) ****/
#define SPI_RX_CRC_HIST1_Pos              (16)
#define SPI_RX_CRC_HIST1_Msk              (0xFFFFUL << SPI_RX_CRC_HIST1_Pos)
#define SPI_RX_CRC_HIST1                  SPI_RX_CRC_HIST1_Msk              /*!<crc16历史值 */
#define SPI_RX_CRC_HIST0_Pos              (0)
#define SPI_RX_CRC_HIST0_Msk              (0xFFFFUL << SPI_RX_CRC_HIST0_Pos)
#define SPI_RX_CRC_HIST0                  SPI_RX_CRC_HIST0_Msk              /*!<crc16历史值 */


/*** Bit definition for [data] register(0x00000030) ****/
#define SPI_DATA_Pos                      (0)
#define SPI_DATA_Msk                      (0xFFFFUL << SPI_DATA_Pos)
#define SPI_DATA                          SPI_DATA_Msk                      /*!<txfifo data/rxfifo data */


/*** Bit definition for [fifoCsr] register(0x00000034) ****/
#define SPI_TXF_SPACE_CNT_Pos             (24)
#define SPI_TXF_SPACE_CNT_Msk             (0xFFUL << SPI_TXF_SPACE_CNT_Pos)
#define SPI_TXF_SPACE_CNT                 SPI_TXF_SPACE_CNT_Msk             /*!<free space quantity */
#define SPI_RXF_DAT_CNT_Pos               (16)
#define SPI_RXF_DAT_CNT_Msk               (0xFFUL << SPI_RXF_DAT_CNT_Pos)
#define SPI_RXF_DAT_CNT                   SPI_RXF_DAT_CNT_Msk               /*!<valid data quantity */
#define SPI_TXF_NOT_FULL_Pos              (13)
#define SPI_TXF_NOT_FULL_Msk              (0x1UL << SPI_TXF_NOT_FULL_Pos)
#define SPI_TXF_NOT_FULL                  SPI_TXF_NOT_FULL_Msk              /*!<tx fifo not full */
#define SPI_RXF_NOT_EMPTY_Pos             (12)
#define SPI_RXF_NOT_EMPTY_Msk             (0x1UL << SPI_RXF_NOT_EMPTY_Pos)
#define SPI_RXF_NOT_EMPTY                 SPI_RXF_NOT_EMPTY_Msk             /*!<rx fifo not full */
#define SPI_TXF_THRES_Pos                 (8)
#define SPI_TXF_THRES_Msk                 (0xFUL << SPI_TXF_THRES_Pos)
#define SPI_TXF_THRES                     SPI_TXF_THRES_Msk                 /*!<tx 阈值 */
#define SPI_RXF_THRES_Pos                 (4)
#define SPI_RXF_THRES_Msk                 (0xFUL << SPI_RXF_THRES_Pos)
#define SPI_RXF_THRES                     SPI_RXF_THRES_Msk                 /*!<rx 阈值 */
#define SPI_TXF_CLR_Pos                   (3)
#define SPI_TXF_CLR_Msk                   (0x1UL << SPI_TXF_CLR_Pos)
#define SPI_TXF_CLR                       SPI_TXF_CLR_Msk                   /*!<清空tx fifo */
#define SPI_RXF_CLR_Pos                   (2)
#define SPI_RXF_CLR_Msk                   (0x1UL << SPI_RXF_CLR_Pos)
#define SPI_RXF_CLR                       SPI_RXF_CLR_Msk                   /*!<清空rx fifo */
#define SPI_TX_DMA_EN_Pos                 (1)
#define SPI_TX_DMA_EN_Msk                 (0x1UL << SPI_TX_DMA_EN_Pos)
#define SPI_TX_DMA_EN                     SPI_TX_DMA_EN_Msk                 /*!<使能TX DMA */
#define SPI_RX_DMA_EN_Pos                 (0)
#define SPI_RX_DMA_EN_Msk                 (0x1UL << SPI_RX_DMA_EN_Pos)
#define SPI_RX_DMA_EN                     SPI_RX_DMA_EN_Msk                 /*!<使能RX DMA */


/*** Bit definition for [intrEn] register(0x00000038) ****/
#define SPI_TXF_THRES_IE_Pos              (1)
#define SPI_TXF_THRES_IE_Msk              (0x1UL << SPI_TXF_THRES_IE_Pos)
#define SPI_TXF_THRES_IE                  SPI_TXF_THRES_IE_Msk              /*!<使能TX阈值中断 */
#define SPI_RXF_THRES_IE_Pos              (0)
#define SPI_RXF_THRES_IE_Msk              (0x1UL << SPI_RXF_THRES_IE_Pos)
#define SPI_RXF_THRES_IE                  SPI_RXF_THRES_IE_Msk              /*!<使能RX阈值中断 */


/*** Bit definition for [intrSr] register(0x0000003c) ****/
#define SPI_TXF_THRES_INTR_Pos            (1)
#define SPI_TXF_THRES_INTR_Msk            (0x1UL << SPI_TXF_THRES_INTR_Pos)
#define SPI_TXF_THRES_INTR                SPI_TXF_THRES_INTR_Msk            /*!<TX阈值中断标志、清除标志 */
#define SPI_RXF_THRES_INTR_Pos            (0)
#define SPI_RXF_THRES_INTR_Msk            (0x1UL << SPI_RXF_THRES_INTR_Pos)
#define SPI_RXF_THRES_INTR                SPI_RXF_THRES_INTR_Msk            /*!<RX阈值中断标志、清除标志 */


/*** Bit definition for [runCtrl] register(0x00000100) ****/
#define SPI_START_Pos                     (0)
#define SPI_START_Msk                     (0x1UL << SPI_START_Pos)
#define SPI_START                         SPI_START_Msk                     /*!<启动 */


/*** Bit definition for [intrEn2] register(0x00000104) ****/
#define SPI_SLV_DONE_IE_Pos               (3)
#define SPI_SLV_DONE_IE_Msk               (0x1UL << SPI_SLV_DONE_IE_Pos)
#define SPI_SLV_DONE_IE                   SPI_SLV_DONE_IE_Msk               /*!<slave mode transaction complete interrupt enable. */
#define SPI_MST_DONE_IE_Pos               (2)
#define SPI_MST_DONE_IE_Msk               (0x1UL << SPI_MST_DONE_IE_Pos)
#define SPI_MST_DONE_IE                   SPI_MST_DONE_IE_Msk               /*!<master mode transaction complete interrupt enable. */
#define SPI_TXF_UNDERRUN_IE_Pos           (1)
#define SPI_TXF_UNDERRUN_IE_Msk           (0x1UL << SPI_TXF_UNDERRUN_IE_Pos)
#define SPI_TXF_UNDERRUN_IE               SPI_TXF_UNDERRUN_IE_Msk           /*!<slave mode transmit FIFO underrun interrupt enable */
#define SPI_RXF_OVERRUN_IE_Pos            (0)
#define SPI_RXF_OVERRUN_IE_Msk            (0x1UL << SPI_RXF_OVERRUN_IE_Pos)
#define SPI_RXF_OVERRUN_IE                SPI_RXF_OVERRUN_IE_Msk            /*!<slave mode receive FIFO overrun interrupt enable */


/*** Bit definition for [intrSr2] register(0x00000108) ****/
#define SPI_SLV_DONE_INTR_Pos             (3)
#define SPI_SLV_DONE_INTR_Msk             (0x1UL << SPI_SLV_DONE_INTR_Pos)
#define SPI_SLV_DONE_INTR                 SPI_SLV_DONE_INTR_Msk             /*!<slave mode transaction complete interrupt status; */
#define SPI_MST_DONE_INTR_Pos             (2)
#define SPI_MST_DONE_INTR_Msk             (0x1UL << SPI_MST_DONE_INTR_Pos)
#define SPI_MST_DONE_INTR                 SPI_MST_DONE_INTR_Msk             /*!<master mode transaction complete intrrupt status; */
#define SPI_TXF_UNDERRUN_INTR_Pos         (1)
#define SPI_TXF_UNDERRUN_INTR_Msk         (0x1UL << SPI_TXF_UNDERRUN_INTR_Pos)
#define SPI_TXF_UNDERRUN_INTR             SPI_TXF_UNDERRUN_INTR_Msk         /*!<transmit fifo underrun interrupt status; */
#define SPI_RXF_OVERRUN_INTR_Pos          (0)
#define SPI_RXF_OVERRUN_INTR_Msk          (0x1UL << SPI_RXF_OVERRUN_INTR_Pos)
#define SPI_RXF_OVERRUN_INTR              SPI_RXF_OVERRUN_INTR_Msk          /*!<receive fifo overrun interrupt status; */




/**
 * @brief  REG_Spi
 */
typedef struct {
  __IO uint32_t  version;                          /*!< Offset 0x00000000 */
  __IO uint32_t  rsvd0[3];                         /*!< Offset 0x00000004 */
  __IO uint32_t  protocol;                         /*!< Offset 0x00000010 */
  __IO uint32_t  txCtrl;                           /*!< Offset 0x00000014 */
  __IO uint32_t  rxCtrl;                           /*!< Offset 0x00000018 */
  __IO uint32_t  slvRxCnt;                         /*!< Offset 0x0000001c */
  __IO uint32_t  crcCtrl0;                         /*!< Offset 0x00000020 */
  __IO uint32_t  crcCtrl1;                         /*!< Offset 0x00000024 */
  __IO uint32_t  crcSta;                           /*!< Offset 0x00000028 */
  __IO uint32_t  crcDbg;                           /*!< Offset 0x0000002c */
  __IO uint32_t  data;                             /*!< Offset 0x00000030 */
  __IO uint32_t  fifoCsr;                          /*!< Offset 0x00000034 */
  __IO uint32_t  intrEn;                           /*!< Offset 0x00000038 */
  __IO uint32_t  intrSr;                           /*!< Offset 0x0000003c */
  __IO uint32_t  rsvd1[48];                        /*!< Offset 0x00000040 */
  __IO uint32_t  runCtrl;                          /*!< Offset 0x00000100 */
  __IO uint32_t  intrEn2;                          /*!< Offset 0x00000104 */
  __IO uint32_t  intrSr2;                          /*!< Offset 0x00000108 */
} REG_Spi;


enum{
    SPI_CONFIG_COMMON = 0,
    SPI_CONFIG_SPEC,
    SPI_CONFIG_TRANS,
    SPI_CONFIG_VAL,
};

/***********************************************************************************************/

//                                       SPI 特殊配置

/***********************************************************************************************/

/*** 管脚单向/双向模式 bits[1:0] ****/
#define SPI_BIDIR_MODE_Pos          (0)
#define SPI_BIDIR_MODE_Len          (2)
#define SPI_BIDIR_MODE_Msk          (0x3UL << SPI_BIDIR_MODE_Pos)
#define SPI_BIDIR_MODE_UNI_DIR      (0x1UL << SPI_BIDIR_MODE_Pos)
#define SPI_BIDIR_MODE_BI_DIR       (0x2UL << SPI_BIDIR_MODE_Pos)

/*** 方向 bits[19:18] ****/
#define SPI_DIR_MODE_Pos            (SPI_BIDIR_MODE_Pos + SPI_BIDIR_MODE_Len)
#define SPI_DIR_MODE_Len            (2)
#define SPI_DIR_MODE_Msk            (0x3UL << SPI_DIR_MODE_Pos)
#define SPI_DIR_MODE_TX_ONLY        (0x1UL << SPI_DIR_MODE_Pos)
#define SPI_DIR_MODE_RX_ONLY        (0x2UL << SPI_DIR_MODE_Pos)
#define SPI_DIR_MODE_TX_RX          (0x3UL << SPI_DIR_MODE_Pos)

/*** 硬件流控 bits[3:2] ****/
#define SPI_HW_FLOWCTRL_Pos         (SPI_DIR_MODE_Pos + SPI_DIR_MODE_Len)
#define SPI_HW_FLOWCTRL_Len         (2)
#define SPI_HW_FLOWCTRL_Msk         (0x3UL << SPI_HW_FLOWCTRL_Pos)
#define SPI_HW_FLOWCTRL_EN          (0x1UL << SPI_HW_FLOWCTRL_Pos)
#define SPI_HW_FLOWCTRL_DIS         (0x2UL << SPI_HW_FLOWCTRL_Pos)

/*** 硬件流控信号是双功能还是单功能信号 bits[5:4] ****/
#define SPI_HW_FLOWCTRL_MODE_Pos    (SPI_HW_FLOWCTRL_Pos + SPI_HW_FLOWCTRL_Len)
#define SPI_HW_FLOWCTRL_MODE_Len    (2)
#define SPI_HW_FLOWCTRL_MODE_Msk    (0x3UL << SPI_HW_FLOWCTRL_MODE_Pos)
#define SPI_HW_FLOWCTRL_MODE_SINGLE (0x1UL << SPI_HW_FLOWCTRL_MODE_Pos)
#define SPI_HW_FLOWCTRL_MODE_DUL    (0x2UL << SPI_HW_FLOWCTRL_MODE_Pos)

/*** 是否投机 bits[5:4] ****/
#define SPI_SPECULAT_SPEED_Pos      (SPI_HW_FLOWCTRL_MODE_Pos + SPI_HW_FLOWCTRL_MODE_Len)
#define SPI_SPECULAT_SPEED_Len      (2)
#define SPI_SPECULAT_SPEED_Msk      (0x3UL << SPI_SPECULAT_SPEED_Pos)
#define SPI_SPECULAT_SPEED_EN       (0x1UL << SPI_SPECULAT_SPEED_Pos)
#define SPI_SPECULAT_SPEED_DIS      (0x2UL << SPI_SPECULAT_SPEED_Pos)

/*** 时钟类型 bits[7:6] ****/
#define SPI_CLK_STYLE_Pos           (SPI_SPECULAT_SPEED_Pos + SPI_SPECULAT_SPEED_Len)
#define SPI_CLK_STYLE_Len           (2)
#define SPI_CLK_STYLE_Msk           (0x3UL << SPI_CLK_STYLE_Pos)
#define SPI_CLK_STYLE_IDLE_PROMPT   (0x1UL << SPI_CLK_STYLE_Pos)
#define SPI_CLK_STYLE_IDLE_DELAY    (0x2UL << SPI_CLK_STYLE_Pos)

/*** 接收长度使用硬解长度域还是软件 bits[9:8] ****/
#define SPI_RX_QTY_MODE_Pos         (SPI_CLK_STYLE_Pos + SPI_CLK_STYLE_Len)
#define SPI_RX_QTY_MODE_Len         (2)
#define SPI_RX_QTY_MODE_Msk         (0x3UL << SPI_RX_QTY_MODE_Pos)
#define SPI_RX_QTY_MODE_MANULE      (0x1UL << SPI_RX_QTY_MODE_Pos)
#define SPI_RX_QTY_MODE_AUTO        (0x2UL << SPI_RX_QTY_MODE_Pos)

/*** 自动计算并发送CRC16 bits[11:10] ****/
#define SPI_TX_CRC16_MODE_Pos       (SPI_RX_QTY_MODE_Pos + SPI_RX_QTY_MODE_Len)
#define SPI_TX_CRC16_MODE_Len       (2)
#define SPI_TX_CRC16_MODE_Msk       (0x3UL << SPI_TX_CRC16_MODE_Pos)
#define SPI_TX_CRC16_MODE_MANULE    (0x1UL << SPI_TX_CRC16_MODE_Pos)
#define SPI_TX_CRC16_MODE_AUTO      (0x2UL << SPI_TX_CRC16_MODE_Pos)

/*** 自动计算RX CRC16 bits[13:12] ****/
#define SPI_RX_CRC16_MODE_Pos       (SPI_TX_CRC16_MODE_Pos + SPI_TX_CRC16_MODE_Len)
#define SPI_RX_CRC16_MODE_Len       (2)
#define SPI_RX_CRC16_MODE_Msk       (0x3UL << SPI_RX_CRC16_MODE_Pos)
#define SPI_RX_CRC16_MODE_MANULE    (0x1UL << SPI_RX_CRC16_MODE_Pos)
#define SPI_RX_CRC16_MODE_AUTO      (0x2UL << SPI_RX_CRC16_MODE_Pos)

/*** 使能 IN REF bits[15:14] ****/
#define SPI_INPUT_REF_MODE_Pos      (SPI_RX_CRC16_MODE_Pos + SPI_RX_CRC16_MODE_Len)
#define SPI_INPUT_REF_MODE_Len      (2)
#define SPI_INPUT_REF_MODE_Msk      (0x3UL << SPI_INPUT_REF_MODE_Pos)
#define SPI_INPUT_REF_MODE_DIS      (0x1UL << SPI_INPUT_REF_MODE_Pos)
#define SPI_INPUT_REF_MODE_EN       (0x2UL << SPI_INPUT_REF_MODE_Pos)

/*** 使能 OUT REF bits[17:16] ****/
#define SPI_OUTPUT_REF_MODE_Pos     (SPI_INPUT_REF_MODE_Pos + SPI_INPUT_REF_MODE_Len)
#define SPI_OUTPUT_REF_MODE_Len     (2)
#define SPI_OUTPUT_REF_MODE_Msk     (0x3UL << SPI_OUTPUT_REF_MODE_Pos)
#define SPI_OUTPUT_REF_MODE_DIS     (0x1UL << SPI_OUTPUT_REF_MODE_Pos)
#define SPI_OUTPUT_REF_MODE_EN      (0x2UL << SPI_OUTPUT_REF_MODE_Pos)



/***********************************************************************************************/

//                                       SPI CRC参数

/***********************************************************************************************/

/*** XOR VAL ****/
#define SPI_CRC_OUT_XOR_Pos         (0)
#define SPI_CRC_OUT_XOR_Len         (16)
#define SPI_CRC_OUT_XOR_Msk         (0xFFUL << SPI_CRC_OUT_XOR_Pos)
#define SPI_CRC_OUT_XOR(n)          (((n) & 0xFFUL) << SPI_CRC_OUT_XOR_Pos)

/*** INIT VAL ****/
#define SPI_CRC_INIT_VAL_Pos        (SPI_CRC_OUT_XOR_Pos + SPI_CRC_OUT_XOR_Len)
#define SPI_CRC_INIT_VAL_Len        (16)
#define SPI_CRC_INIT_VAL_Msk        (0xFFUL << SPI_CRC_INIT_VAL_Pos)
#define SPI_CRC_INIT_VAL(n)         (((n) & 0xFFUL) << SPI_CRC_INIT_VAL_Pos)




typedef struct {
    union{
        uint32_t val;
        struct{
            uint32_t busy               : 1;
            uint32_t sendIncomplete     : 1;
            uint32_t recvIncomplete     : 1;
            // uint32_t txIrq              : 1;
            // uint32_t rxIrq              : 1;
            // uint32_t txDma              : 1;
            // uint32_t rxDma              : 1;
            // uint32_t dmaEnd             : 1;
            // uint32_t spiEnd             : 1;
            uint32_t reserved           : 29;
        };
    };
} SPI_STATUS;


/****** SPI Event *****/
#define SPI_EVENT_TRANSFER_COMPLETE (1UL << 0)  ///< 数据传输完成
#define SPI_EVENT_DATA_LOST         (1UL << 1)  ///< 数据丢失: Receive overflow / Transmit underflow
#define SPI_EVENT_MODE_FAULT        (1UL << 2)  ///< Master Mode Fault (SS deactivated when Master)



typedef void (*SPI_SignalEvent) (void* data, uint32_t event);  ///< Pointer to \ref SPI_SignalEvent : Signal SPI Event.
typedef void (*SPI_ExtFunc) (void *extParam);

/**
\brief SPI Driver Capabilities.
*/
typedef struct {
    uint32_t simplex          : 1;        ///< supports Simplex Mode (Master and Slave) @deprecated Reserved (must be zero)
    uint32_t ti_ssi           : 1;        ///< supports TI Synchronous Serial Interface
    uint32_t microwire        : 1;        ///< supports Microwire Interface
    uint32_t event_mode_fault : 1;        ///< Signal Mode Fault event: \ref SPI_EVENT_MODE_FAULT
    uint32_t hdlc             : 1;
} SPI_Capabilities;


typedef struct {
    uint16_t                number;
    uint16_t                priorityMax;
    uint8_t                 level;
}SPI_IrqInfo;

#define SPI_USE_DMA
#define SPI_USE_TX_LLI

#ifdef SPI_USE_TX_LLI
#define SPI_TX_LLI_NUM_MAX         (15)
#endif

typedef struct {
#ifdef SPI_USE_DMA
    DMA_ChHandle           *rxHdl;
    DMA_ChHandle           *txHdl;

#ifdef SPI_USE_TX_LLI
    DMA_LliDesc             txLliCfg[SPI_TX_LLI_NUM_MAX];
#endif
    DMA_ChCfg               txCfg;
    DMA_ChCfg               rxCfg;
#endif
} SPI_DmaInfo;

typedef struct {
    uint8_t                *buf;
    uint32_t                len;
    uint32_t                cnt;
    uint32_t                actualLen;
    uint16_t                crc;
    uint16_t                crc0;
    uint16_t                crc1;
    uint16_t                crcRecv;
}SPI_Msg;

#ifdef SPI_USE_TX_LLI
typedef struct {
    uint8_t                *buf[SPI_TX_LLI_NUM_MAX];
    uint32_t                len[SPI_TX_LLI_NUM_MAX];
    uint16_t                crc;
    uint8_t                 num;
    uint32_t                cnt;
    uint32_t                total;
}SPI_MsgArray;
#endif

typedef struct {
#ifdef SPI_USE_TX_LLI
    SPI_MsgArray            txMsgs;
#endif

    SPI_Msg                 txMsg;
    SPI_Msg                 rxMsg;

// #define SPI_MSG_CRC_END             (0x1)
#define SPI_MSG_MST_RX_OVERRUN      (0x2)
#define SPI_MSG_SLV_TX_UNFINISH     (0x4)
#define SPI_MSG_SLV_RX_UNFINISH     (0x8)
#define SPI_MSG_SLV_RX_OVERRUN      (0x10)
    uint8_t                 err;

#define SPI_TRANS_CTRL_MULTI_MSG    (0x1)
    uint8_t                 ctrl;
}SPI_Trans;

typedef struct {
#ifdef USE_TOP_ICC_ASIC
    struct clk                           *pclk;
    struct clk                           *wclk;
#endif

    uint32_t                              freq;
}SPI_Clock;

typedef struct {
    const struct SPI_Res   *res;
    REG_Spi                *regs;
    SPI_IrqInfo             irq;
    SPI_DmaInfo             dma;
    SPI_Trans               trans;
    SPI_Clock               clock;
    SPI_STATUS              status;
    SPI_Capabilities        capabilities;

    SPI_SignalEvent         cbEvent;
    void                   *userData;

    SPI_ExtFunc             extFunc;
    void                   *extParam;

#define SPI_INIT            ((uint8_t)0x01)
#define SPI_POWER           ((uint8_t)0x02)
#define SPI_CLOCK_REQ       ((uint8_t)0x10)
#define SPI_TX_DMA_INIT     ((uint8_t)0x04)
#define SPI_RX_DMA_INIT     ((uint8_t)0x08)
    uint8_t                 ctrl;

    // uint32_t                config;
    union{
        uint32_t config;
        struct
        {
            uint32_t busNumber : 2;
            uint32_t clkSrc : 2;
            uint32_t clkDiv : 4;
            uint32_t ms : 2;
            uint32_t polpha : 3;
            uint32_t dataWidth : 6;
            uint32_t transMode : 3;
            uint32_t csMode : 2;
            uint32_t reserved :8;
        }config_s;
    };
    uint32_t                valConfig;
    union{
        uint32_t config2;
        struct
        {
            uint32_t txThres : 5;
            uint32_t rxThres : 5;
            uint32_t defVal  : 8;
        }config2_s;
    };

    union
    {
        uint32_t            speConfig;
        struct {
            uint32_t        biDir   : 2;
            uint32_t        dir     : 2;
            uint32_t        flow    : 2;
            uint32_t        speculat: 2;
            uint32_t        clkStyle: 2;
            uint32_t        rxMode  : 2;
            uint32_t        txCrc   : 2;
            uint32_t        rxCrc   : 2;
            uint32_t        inRef   : 2;
            uint32_t        outRef  : 2;
            uint32_t        reserved: 12;
        }speConfigT;
    };
    
    
    uint32_t                crc16Param;
    uint32_t                timeout;
}SPI_Handle;





/**
 ************************************************************************************
 * @brief           SPI初始化
 *
 * @param[in]       hdl             SPI句柄
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_Initialize(SPI_Handle *hdl);

/**
 ************************************************************************************
 * @brief           SPI去初始化
 *
 * @param[in]       hdl             SPI句柄
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_Uninitialize(SPI_Handle *hdl);

/**
 ************************************************************************************
 * @brief           设置SPI控制器的供电
 *
 * @param[in]       hdl             SPI句柄
 * @param[in]       state           设置控制的状态
 *
 * @return          int32_t
 * @retval          DRV_ERR_PARAMETER             错误的参数
 *                  DRV_ERR_UNSUPPORTED           不支持             
 *                  DRV_ERR                       错误
 *                  DRV_OK                        成功   
 ************************************************************************************
*/
int32_t SPI_PowerControl(SPI_Handle *hdl, DRV_POWER_STATE state);

/**
 ************************************************************************************
 * @brief           配置SPI控制器
 *
 * @param[in]       hdl             SPI句柄
 * @param[in]       control         控制命令
 * @param[in]       arg             控制参数
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_Control(SPI_Handle *hdl, uint32_t control, uint32_t arg);

/**
 ************************************************************************************
 * @brief           获取SPI状态
 *
 * @param[in]       hdl             SPI句柄
 *
 * @return          SPI_STATUS
 ************************************************************************************
*/
SPI_STATUS SPI_GetStatus(SPI_Handle *hdl);

/**
 ************************************************************************************
 * @brief           获取SPI RX的CRC值
 *
 * @param[in]       hdl             SPI句柄
 *
 * @return          uint16_t
 ************************************************************************************
*/
uint16_t SPI_GetRxCrc16(SPI_Handle *hdl);

/**
 ************************************************************************************
 * @brief           获取SPI TX的CRC值
 *
 * @param[in]       hdl             SPI句柄
 *
 * @return          uint16_t
 ************************************************************************************
*/
uint16_t SPI_GetTxCrc16(SPI_Handle *hdl);
void SPI_CommonCfg(SPI_Handle *hdl, uint32_t arg);
void SPI_SpecCfg(SPI_Handle *hdl, uint32_t arg);
int32_t SPI_Transfer(SPI_Handle *hdl, uint8_t *txData, uint32_t txLen, uint8_t *rxData, uint32_t rxLen, uint32_t timeout);

/**
 ************************************************************************************
 * @brief           获取SPI实际发送长度
 * 
 * 使用场景：在slave发送的时候，无法保证对端主机何时结束传输，无法保证数据是否全部发送完
 *
 * @param[in]       hdl             SPI句柄
 *
 * @return          uint32_t
 ************************************************************************************
*/
uint32_t SPI_SendLengthActual(SPI_Handle *hdl);

/**
 ************************************************************************************
 * @brief           获取SPI实际接收长度
 * 
 * 使用场景：
 * 1、在Master模式下，同时开启硬解长度域功能，这个时候实际接收的长度受对端从机控制，需要获取实际接收的长度
 * 2、在Slave模式下，无法保证对端主机何时结束传输，无法确认发送的数据量
 *
 * @param[in]       hdl             SPI句柄
 *
 * @return          uint32_t
 ************************************************************************************
*/
uint32_t SPI_RecvLengthActual(SPI_Handle *hdl);

/**
 ************************************************************************************
 * @brief           SPI Master发送
 *
 * @param[in]       hdl             SPI句柄
 * @param[in]       data            发送的数据
 * @param[in]       len             发送的长度
 * @param[in]       crc             是否开启硬件CRC
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_MasterSend(SPI_Handle *hdl, uint8_t *data, uint32_t len, bool crc, uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Slave发送
 *
 * @param[in]       hdl             SPI句柄
 * @param[in]       data            发送的数据
 * @param[in]       len             发送的长度
 * @param[in]       crc             是否开启硬件CRC
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_SlaveSend(SPI_Handle *hdl, uint8_t *data, uint32_t len, bool crc, uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Slave发送(开启后额外进行回调)
 *
 * @param[in]       hdl             SPI句柄
 * @param[in]       data            发送的数据
 * @param[in]       len             发送的长度
 * @param[in]       extFunc         回调函数
 * @param[in]       extParam        回调参数
 * @param[in]       crc             是否开启硬件CRC
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_SlaveSendExtFunc(SPI_Handle *hdl, uint8_t *data, uint32_t len, void (*extFunc)(void *), void *extParam, bool crc, uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Slave接收(开启后额外进行回调)
 *
 * @param[in]       hdl             SPI句柄
 * @param[in]       data            接收的数据
 * @param[in]       len             接收的长度
 * @param[in]       extFunc         回调函数
 * @param[in]       extParam        回调参数
 * @param[in]       crc             是否开启硬件CRC
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_SlaveRecvExcFunc(SPI_Handle *hdl, uint8_t *data, uint32_t len, void (*extFunc)(void *), void *extParam, bool crc, bool qty, uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Master接收
 * 
 * 1、只使能CRC：将所有接收到的数据计算CRC，
 * 2、只使能QTY：按照帧格式接收数据，硬解长度域后获取数据长度，然后接收指定长度的数据后，再接收CRC字段
 * 3、同时使能： 一帧数据中的CRC字段会被接收，但是不参与CRC计算
 *
 * @param[in]       hdl             SPI句柄
 * @param[in]       data            接收地址
 * @param[in]       len             接收最大长度
 * @param[in]       crc             是否计算CRC（和SLAVE不同，Master会多发两个时钟，来获取CRC的值，但是不参与CRC计算，所以接收地址需要预留2个字节空间存放CRC）
 * @param[in]       qty             是否硬件抽取数据帧（同CRC要求一样）
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_MasterRecv(SPI_Handle *hdl, uint8_t *data, uint32_t len, bool crc, bool qty, uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Slave接收
 * 
 * 如果只使能CRC，则会将所有接收到的数据计算CRC
 * 如果只使能QTY，则超过长度域的数据会丢弃
 * 同时使能，一帧数据中的CRC字段会被接收，但是不参与CRC计算
 *
 * @param[in]       hdl             SPI句柄
 * @param[in]       data            接收地址
 * @param[in]       len             接收最大长度
 * @param[in]       crc             是否计算CRC
 * @param[in]       qty             是否硬件抽取数据帧
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_SlaveRecv(SPI_Handle *hdl, uint8_t *data, uint32_t len, bool crc, bool qty, uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Master读写
 * 
 * @param[in]       hdl             SPI句柄
 * @param[in]       txData          发送的数据
 * @param[in]       txLen           发送的长度
 * @param[in]       txCrc           是否计算CRC
 * @param[in]       rxData          接收地址
 * @param[in]       rxLen           接收最大长度
 * @param[in]       rxCrc           是否计算CRC
 * @param[in]       rxQty           是否硬件抽取数据帧
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_MasterTransfer(SPI_Handle *hdl, uint8_t *txData, uint32_t txLen, bool txCrc,
                                            uint8_t *rxData, uint32_t rxLen, bool rxCrc, bool rxQty, 
                                            uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Slave读写
 * 
 * @param[in]       hdl             SPI句柄
 * @param[in]       txData          发送的数据
 * @param[in]       txLen           发送的长度
 * @param[in]       txCrc           是否计算CRC
 * @param[in]       rxData          接收地址
 * @param[in]       rxLen           接收最大长度
 * @param[in]       rxCrc           是否计算CRC
 * @param[in]       rxQty           是否硬件抽取数据帧
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_SlaveTransfer(SPI_Handle *hdl, uint8_t *txData, uint32_t txLen, bool txCrc,
                                           uint8_t *rxData, uint32_t rxLen, bool rxCrc, bool rxQty, 
                                           uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Master链表发送
 * 
 * @param[in]       hdl             SPI句柄
 * @param[in]       txArray         发送的数
 * @param[in]       crc             是否计算CRC
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_MasterLliSend(SPI_Handle *hdl, SPI_MsgArray *txArray, bool crc, uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Master链表传输
 * 
 * @param[in]       hdl             SPI句柄
 * @param[in]       txArray         发送的数
 * @param[in]       txCrc           是否计算CRC
 * @param[in]       rxData          接收地址
 * @param[in]       rxLen           接收最大长度
 * @param[in]       rxCrc           是否计算CRC
 * @param[in]       rxQty           是否硬件抽取数据帧
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_MasterLliTransfer(SPI_Handle *hdl, SPI_MsgArray *txArray, bool txCrc,
                                               uint8_t *rxData, uint32_t rxLen, bool rxCrc, bool rxQty, 
                                               uint32_t timeout);

/**
 ************************************************************************************
 * @brief           SPI Slave链表传输
 * 
 * @param[in]       hdl             SPI句柄
 * @param[in]       txArray         发送的数
 * @param[in]       txCrc           是否计算CRC
 * @param[in]       rxData          接收地址
 * @param[in]       rxLen           接收最大长度
 * @param[in]       rxCrc           是否计算CRC
 * @param[in]       rxQty           是否硬件抽取数据帧
 * @param[in]       timeout         超时时间
 *
 * @return          int32_t
 * @retval          !0              错误
 *                  0               成功
 ************************************************************************************
*/
int32_t SPI_SlaveLliTransfer(SPI_Handle *hdl, SPI_MsgArray *txArray, bool txCrc,
                                              uint8_t *rxData, uint32_t rxLen, bool rxCrc, bool rxQty, 
                                              uint32_t timeout);

/**
 ************************************************************************************
 * @brief           获取SPI实际接收长度
 *
 * @param[in]       hdl             SPI句柄
 *
 * @return          uint32_t        长度
 ************************************************************************************
*/
uint32_t SPI_SlaveRxCntGet(SPI_Handle *hdl);

/**
 ************************************************************************************
 * @brief           设置SPI CS电平
 *
 * @param[in]       hdl             SPI句柄
 *
 * @return          void        
 ************************************************************************************
*/
void SPI_CsSet(SPI_Handle *hdl, uint8_t level);
#endif
/** @} */