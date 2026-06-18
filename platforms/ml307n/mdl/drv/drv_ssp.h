/**
 ***********************************************************************************************************************
 * Copyright (c) 2022, 芯昇科技有限公司
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        drv_ssp.h
 *
 * @brief       Implementation of ssp function.
 *
 * @par History:
 * <table>
 * <tr><th>Date                <th> Author              <th> Notes
 * <tr><td>2022-04-06    <td> ICT Team           <td> First version
 * </table>
 *
 */
#ifndef _DRV_SSP_H
#define _DRV_SSP_H

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup Ssp
 */

/**@{*/

#define SSP_USE_DMA
#define SSP_USE_IRQ
// #define SSP_USE_ASIC
// #define SSP_USE_PINMUX
#define SSP_PrintDebug(fmt, ...)       osPrintf("\033[" "30m" "(%s-%d):" fmt "\r\n" "\033[0m", __func__, __LINE__, ##__VA_ARGS__)
#define SSP_PrintError(fmt, ...)       osPrintf("\033[" "31m" "(%s-%d):" fmt "\r\n" "\033[0m", __func__, __LINE__, ##__VA_ARGS__)

#define SSP_USE_TX_LLI

#ifdef SSP_USE_TX_LLI
#define SSP_LLI_NUM_MAX         (15)
#endif


#include "drv_common.h"

#ifdef SSP_USE_DMA
#include <drv_dma.h>
#endif
#ifdef USE_TOP_ICC_ASIC
#include <ict_clk.h>
#endif

/***********************************************************************************************/

//                                       CMSIS接口

/***********************************************************************************************/

#define SSP_CONTROL_Pos              0
#define SSP_CONTROL_Msk             (0xFUL << SSP_CONTROL_Pos)

#define SSP_FREQ_Pos                4
#define SSP_FREQ_Msk                (3UL << SSP_FREQ_Pos)
#define SSP_FREQ_13M                (0UL << SSP_FREQ_Pos)                   // 13M
#define SSP_FREQ_26M                (1UL << SSP_FREQ_Pos)                   // 26M
#define SSP_FREQ_39M                (2UL << SSP_FREQ_Pos)                   // 39M
#define SSP_FREQ_78M                (3UL << SSP_FREQ_Pos)                   // 78M

#define SSP_MS_MODE_Pos             6
#define SSP_MS_MODE_Msk             (1UL << SSP_MS_MODE_Pos)
#define SSP_MODE_MASTER             (0UL << SSP_MS_MODE_Pos)                // SSP 主机模式 (默认)
#define SSP_MODE_SLAVE              (1UL << SSP_MS_MODE_Pos)                // SSP 从机模式

#define SSP_POLPHA_Pos              7
#define SSP_POLPHA_Msk              (3UL << SSP_POLPHA_Pos)
#define SSP_CPOL0_CPHA0             (0UL << SSP_POLPHA_Pos)                 // Clock Polarity 0, Clock Phase 0 (default)
#define SSP_CPOL0_CPHA1             (1UL << SSP_POLPHA_Pos)                 // Clock Polarity 0, Clock Phase 1
#define SSP_CPOL1_CPHA0             (2UL << SSP_POLPHA_Pos)                 // Clock Polarity 1, Clock Phase 0
#define SSP_CPOL1_CPHA1             (3UL << SSP_POLPHA_Pos)                 // Clock Polarity 1, Clock Phase 1

#define SSP_FRAME_FORMAT_Pos        9
#define SSP_FRAME_FORMAT_Msk        (3UL << SSP_FRAME_FORMAT_Pos)
#define SSP_MOTOROLA_SPI            (0UL << SSP_FRAME_FORMAT_Pos)           // Motorola SPI 帧格式 (default)
#define SSP_TI_SSP                  (1UL << SSP_FRAME_FORMAT_Pos)           // TI SSP 帧格式
#define SSP_ISI_SPI                 (2UL << SSP_FRAME_FORMAT_Pos)           // ISI SPI 帧格式

#define SSP_DATA_BITS_Pos            11
#define SSP_DATA_BITS_Msk           (0x3FUL << SSP_DATA_BITS_Pos)
#define SSP_DATA_BITS(n)            (((n) & 0x3FUL) << SSP_DATA_BITS_Pos)   // 数据bit宽度

#define SSP_TRANS_MODE_Pos          18
#define SSP_TRANS_MODE_Msk          (3UL << SSP_TRANS_MODE_Pos)
#define SSP_TRANS_POLL_MODE         (0UL << SSP_TRANS_MODE_Pos)             // 轮训模式
#define SSP_TRANS_INT_MODE          (1UL << SSP_TRANS_MODE_Pos)             // 中断模式
#define SSP_TRANS_DAM_MODE          (2UL << SSP_TRANS_MODE_Pos)             // DMA模式

#define SSP_CS_MODE_Pos             20
#define SSP_CS_MDOE_Msk             (1UL << SSP_CS_MODE_Pos)
#define SSP_CS_AUTO                 (0UL << SSP_CS_MODE_Pos)                // 硬件自动控制CS管脚
#define SSP_CS_MANUAL               (1UL << SSP_CS_MODE_Pos)                // 用户手动控制CS管脚

#define SSP_DEF_TX_VALUE_Pos        21
#define SSP_DEF_TX_VALUE_Msk        (0XFFUL << SSP_DEF_TX_VALUE_Pos)        // RX dummy时发送的默认值

/****** SSP Slave Select Signal definitions *****/
#define SSP_SS_INACTIVE              0UL                                    // SSP SLAVE选择信号未激活
#define SSP_SS_ACTIVE                1UL                                    // SSP SLAVE选择信号激活


/****** SSP specific error codes *****/
#define SSP_ERROR_MODE              (DRV_ERR_SPECIFIC - 1)                  // 不支持该模式
#define SSP_ERROR_FRAME_FORMAT      (DRV_ERR_SPECIFIC - 2)                  // 不支持该数据帧格式
#define SSP_ERROR_DATA_BITS         (DRV_ERR_SPECIFIC - 3)                  // 不支持该数据位宽
#define SSP_ERROR_BIT_ORDER         (DRV_ERR_SPECIFIC - 4)                  // 不支持指定的位顺序
#define SSP_ERROR_SS_MODE           (DRV_ERR_SPECIFIC - 5)                  // 不支持指定的slave选择模式
#define SSP_ERROR_TRANS_MODE        (DRV_ERR_SPECIFIC - 6)                  // 不支持指定的slave选择模式

/**
\brief SSP Status
*/
typedef struct _SSP_STATUS {
    uint32_t busy       : 1;              // 正在发送/接收
    uint32_t data_lost  : 1;              // 数据丢失: Receive overflow / Transmit underflow (cleared on start of trans operation)
    uint32_t mode_fault : 1;              // 检测到模式错误; optional (cleared on start of trans operation)
    uint32_t reserved   : 29;
} SSP_STATUS;


/****** SSP Event *****/
#define SSP_EVENT_TRANSFER_COMPLETE (1UL << 0)  ///< 数据传输完成
#define SSP_EVENT_DATA_LOST         (1UL << 1)  ///< 数据丢失: Receive overflow / Transmit underflow
#define SSP_EVENT_MODE_FAULT        (1UL << 2)  ///< Master Mode Fault (SS deactivated when Master)



typedef void (*SSP_SignalEvent) (void* data);  ///< Pointer to \ref SSP_SignalEvent : Signal SSP Event.


/**
\brief SSP Driver Capabilities.
*/
typedef struct _SSP_CAPABILITIES {
    uint32_t simplex          : 1;        ///< supports Simplex Mode (Master and Slave) @deprecated Reserved (must be zero)
    uint32_t ti_ssi           : 1;        ///< supports TI Synchronous Serial Interface
    uint32_t microwire        : 1;        ///< supports Microwire Interface
    uint32_t event_mode_fault : 1;        ///< Signal Mode Fault event: \ref SSP_EVENT_MODE_FAULT
    uint32_t reserved         : 28;       ///< Reserved (must be zero)
} SSP_CAPABILITIES;


typedef struct {
    uint16_t                number;
    uint16_t                priorityMax;
    uint8_t                 level;
}SSP_IrqInfo;

// typedef struct {
// #ifdef SSP_USE_DMA
//     uint32_t                rxId;       // ssp rx Dma channel number
//     uint32_t                txId;
//     T_Dma_Chan              rxHandle;    // ssp rx dma channel handler
//     T_Dma_Chan              txHandle;
//     IctDma_Para_CbkFunc     rxCallback;    // ssp rx callback function
//     IctDma_Para_CbkFunc     txCallback;
//     void                   *param;
// #ifdef SSP_USE_TX_LLI
//     T_Dma_Chan              txCfgs[SSP_LLI_NUM_MAX];
// #endif
// #endif
//     uint32_t                txBurstLen;
// 	uint32_t                rxBurstLen;
// } SSP_DmaInfo;

typedef struct {
#ifdef SSP_USE_DMA
    DMA_ChHandle           *rxHdl;
    DMA_ChHandle           *txHdl;

#ifdef SSP_USE_TX_LLI
    DMA_LliDesc             txLliCfg[SSP_LLI_NUM_MAX];
#endif
    DMA_ChCfg               txCfg;
    DMA_ChCfg               rxCfg;
#endif
    uint32_t                txBurstLen;
	uint32_t                rxBurstLen;
} SSP_DmaInfo;

typedef struct {
    uint8_t                *buf;
    uint32_t                len;
    uint32_t                cnt;
    uint32_t                mask;
    uint8_t                 flags;
}SSP_Msg;

typedef struct {
    SSP_Msg                 txMsg;
    SSP_Msg                 rxMsg;
    SSP_Msg                *txMsgs;
    uint32_t                txNum;
}SSP_Transfer;

typedef struct {
    SSP_SignalEvent         cbEvent;
    void                   *cbDate;
    SSP_STATUS              status;
    uint32_t                control;
    uint32_t                busSpeed;
    uint32_t                defVal;
}SSP_Info;

typedef struct {
#ifdef USE_TOP_ICC_ASIC
    struct clk                           *pclk;
    struct clk                           *wclk;
#endif

#define SSP_CLK_SRC_13M        ((uint32_t)13000000)
#define SSP_CLK_SRC_26M        ((uint32_t)26000000)
#define SSP_CLK_SRC_39M        ((uint32_t)39000000)
#define SSP_CLK_SRC_78M        ((uint32_t)78000000)
    uint32_t                              freq;
}SSP_Clock;

typedef struct {
    const struct SSP_Res   *res;
    void                   *regs;
    SSP_IrqInfo             irq;
    SSP_DmaInfo             dma;
    SSP_Transfer            trans;
    SSP_Info                info;
    SSP_Clock               clock;

#define SSP_INIT            ((uint8_t)0x01)
#define SSP_POWER           ((uint8_t)0x02)
#define SSP_CLOCK_REQ       ((uint8_t)0x10)
    uint8_t                 ctrl;
}SSP_Handle;


int32_t SSP_Initialize(SSP_Handle *hdl);
int32_t SSP_Uninitialize(SSP_Handle *hdl);
int32_t SSP_PowerControl(SSP_Handle *hdl, DRV_POWER_STATE state);
int32_t SSP_TransmitReceive(SSP_Handle *hdl, const void *txData, uint32_t txLen, void *rxData, uint32_t rxLen);
int32_t SSP_MultiTransfer(SSP_Handle *hdl, SSP_Msg *msgs, uint32_t num, uint8_t *rxData, uint32_t rxLen);
int32_t SSP_Control(SSP_Handle *hdl, uint32_t control, uint32_t arg);
SSP_STATUS SSP_GetStatus(SSP_Handle *hdl);
uint32_t SSP_GetDataCount(SSP_Handle *hdl);
uint32_t SSP_ActualRxLen(SSP_Handle *hdl);
void SSP_Stop(SSP_Handle *hdl);
void SSP_WaitIdle(SSP_Handle *hdl);
int32_t SSP_WriteMsg(SSP_Handle *hdl);
int32_t SSP_ReadMsg(SSP_Handle *hdl);
int32_t SSP_TxDmaStart(SSP_Handle *hdl);
int32_t SSP_RxDmaStart(SSP_Handle *hdl);


#ifdef  __cplusplus
}
#endif

#endif /* _DRV_SSP_H */

/** @} */