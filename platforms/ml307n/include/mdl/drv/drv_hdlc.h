/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_hdlc.h
 *
 * @brief       HDLC驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team         创建
 ************************************************************************************
 */

#ifndef _DRV_HDLC_H_
#define _DRV_HDLC_H_

#include "drv_common.h"
#include "os_def.h"

/**
 * @addtogroup Hdlc
 */

/**@{*/

/***********************************************************************************************/

//                                       寄存器定义

/***********************************************************************************************/

/*** Bit definition for [version] register(0x00000000) ****/
#define HDLC_VER_REG_H_POS              (24)
#define HDLC_VER_REG_H_MSK              (0xFFUL << HDLC_VER_REG_H_POS)
#define HDLC_VER_REG_H                  HDLC_VER_REG_H_MSK                      /*!<High 8bits of Hardware version of hdlc module */
#define HDLC_VER_REG_L_POS              (16)
#define HDLC_VER_REG_L_MSK              (0xFFUL << HDLC_VER_REG_L_POS)
#define HDLC_VER_REG_L                  HDLC_VER_REG_L_MSK                      /*!<Low 8bits of Hardware version of hdlc module */


/*** Bit definition for [config] register(0x00000004) ****/
#define HDLC_AWBURST_POS                (1)
#define HDLC_AWBURST_MSK                (0x1UL << HDLC_AWBURST_POS)
#define HDLC_AWBURST_INCR               (0x0UL << HDLC_AWBURST_POS)             /*!<AXI burst类型:INCR */
#define HDLC_AWBURST_FIXED              (0x1UL << HDLC_AWBURST_POS)             /*!<AXI burst类型:Fixed */
#define HDLC_CRC_EN_POS                 (0)
#define HDLC_CRC_EN_MSK                 (0x1UL << HDLC_CRC_EN_POS)
#define HDLC_CRC_DISABLE                (0x0UL << HDLC_CRC_EN_POS)              /*!<Not calculate /insert crc */
#define HDLC_CRC_ENABLE                 (0x1UL << HDLC_CRC_EN_POS)              /*!<Auto insert crc result in tail of log */


/*** Bit definition for [start] register(0x00000008) ****/
#define HDLC_START_POS                  (0)
#define HDLC_START_MSK                  (0x1UL << HDLC_START_POS)
#define HDLC_START                      HDLC_START_MSK                          /*!<Enable hdlc encoder */


/*** Bit definition for [flush] register(0x0000000c) ****/
#define HDLC_FLUSH_POS                  (0)
#define HDLC_FLUSH_MSK                  (0x1UL << HDLC_FLUSH_POS)
#define HDLC_FLUSH                      HDLC_FLUSH_MSK                          /*!<Flush dout fifo */


/*** Bit definition for [addr0] register(0x00000010) ****/
#define HDLC_ADDR0_POS                  (0)
#define HDLC_ADDR0_MSK                  (0xFFFFFFFFUL << HDLC_ADDR0_POS)
#define HDLC_ADDR0                      HDLC_ADDR0_MSK                          /*!<Channel0 Log address */


/*** Bit definition for [len0] register(0x00000014) ****/
#define HDLC_COMPRESS0_POS              (16)
#define HDLC_COMPRESS0_MSK              (0x1UL << HDLC_COMPRESS0_POS)
#define HDLC_NOT_COMPRESS0              (0x0UL << HDLC_COMPRESS0_POS)           /*!<Channel0 not compress */
#define HDLC_COMPRESS0                  (0x1UL << HDLC_COMPRESS0_POS)           /*!<Channel0 compress */
#define HDLC_LEN0_POS                   (0)
#define HDLC_LEN0_MSK                   (0xFFFFUL << HDLC_LEN0_POS)
#define HDLC_LEN0                       HDLC_LEN0_MSK                           /*!<Channel0 Log length */


/*** Bit definition for [addr1] register(0x00000018) ****/
#define HDLC_ADDR1_POS                  (0)
#define HDLC_ADDR1_MSK                  (0xFFFFFFFFUL << HDLC_ADDR1_POS)
#define HDLC_ADDR1                      HDLC_ADDR1_MSK                          /*!<Channel1 Log address */


/*** Bit definition for [len1] register(0x0000001c) ****/
#define HDLC_COMPRESS1_POS              (16)
#define HDLC_COMPRESS1_MSK              (0x1UL << HDLC_COMPRESS1_POS)
#define HDLC_NOT_COMPRESS1              (0x0UL << HDLC_COMPRESS1_POS)           /*!<Channel1 not compress */
#define HDLC_COMPRESS1                  (0x1UL << HDLC_COMPRESS1_POS)           /*!<Channel1 compress */
#define HDLC_LEN1_POS                   (0)
#define HDLC_LEN1_MSK                   (0xFFFFUL << HDLC_LEN1_POS)
#define HDLC_LEN1                       HDLC_LEN1_MSK                           /*!<Channel1 Log length */


/*** Bit definition for [obufBase] register(0x00000020) ****/
#define HDLC_OBUF_BASE_POS              (0)
#define HDLC_OBUF_BASE_MSK              (0xFFFFFFFFUL << HDLC_OBUF_BASE_POS)
#define HDLC_OBUF_BASE                  HDLC_OBUF_BASE_MSK                      /*!<Output buffer base address */


/*** Bit definition for [obufSize] register(0x00000024) ****/
#define HDLC_OBUF_SIZE_POS              (0)
#define HDLC_OBUF_SIZE_MSK              (0xFFFFFFUL << HDLC_OBUF_SIZE_POS)
#define HDLC_OBUF_SIZE                  HDLC_OBUF_SIZE_MSK                      /*!<Output buffer size is OBUF_SIZE + 1 */


/*** Bit definition for [obufRptr] register(0x00000028) ****/
#define HDLC_OBUF_RPTR_POS              (0)
#define HDLC_OBUF_RPTR_MSK              (0xFFFFFFFFUL << HDLC_OBUF_RPTR_POS)
#define HDLC_OBUF_RPTR                  HDLC_OBUF_RPTR_MSK                      /*!<Output buffer read pointer */


/*** Bit definition for [obufWptr] register(0x0000002c) ****/
#define HDLC_OBUF_WPTR_POS              (0)
#define HDLC_OBUF_WPTR_MSK              (0xFFFFFFFFUL << HDLC_OBUF_WPTR_POS)
#define HDLC_OBUF_WPTR                  HDLC_OBUF_WPTR_MSK                      /*!<Output buffer write pointer */


/*** Bit definition for [cmdFifoStatus] register(0x00000030) ****/
#define HDLC_FIFO1_NOT_FULL_POS         (17)
#define HDLC_FIFO1_NOT_FULL_MSK         (0x1UL << HDLC_FIFO1_NOT_FULL_POS)
#define HDLC_FIFO1_FULL                 (0x0UL << HDLC_FIFO1_NOT_FULL_POS)      /*!<FIFO1 full */
#define HDLC_FIFO1_NOT_FULL             (0x1UL << HDLC_FIFO1_NOT_FULL_POS)      /*!<FIFO1 not full */
#define HDLC_FIFO0_NOT_FULL_POS         (16)
#define HDLC_FIFO0_NOT_FULL_MSK         (0x1UL << HDLC_FIFO0_NOT_FULL_POS)
#define HDLC_FIFO0_FULL                 (0x0UL << HDLC_FIFO0_NOT_FULL_POS)      /*!<FIFO0 full */
#define HDLC_FIFO0_NOT_FULL             (0x1UL << HDLC_FIFO0_NOT_FULL_POS)      /*!<FIFO0 not full */
#define HDLC_FIFO1_SPACE_CNT_POS        (8)
#define HDLC_FIFO1_SPACE_CNT_MSK        (0x1FUL << HDLC_FIFO1_SPACE_CNT_POS)
#define HDLC_FIFO1_SPACE_CNT            HDLC_FIFO1_SPACE_CNT_MSK                /*!<Command FIFO1 space */
#define HDLC_FIFO0_SPACE_CNT_POS        (0)
#define HDLC_FIFO0_SPACE_CNT_MSK        (0x1FUL << HDLC_FIFO0_SPACE_CNT_POS)
#define HDLC_FIFO0_SPACE_CNT            HDLC_FIFO0_SPACE_CNT_MSK                /*!<command FIFO0 space */


/*** Bit definition for [flushStatus] register(0x00000034) ****/
#define HDLC_FLUSH_ING_POS              (6)
#define HDLC_FLUSH_ING_MSK              (0x1UL << HDLC_FLUSH_ING_POS)
#define HDLC_FLUSH_ING                  HDLC_FLUSH_ING_MSK                      /*!<Flush-ing dout buffer/dout fifo */
#define HDLC_DOUT_BUF_PTR_NE_0_POS      (5)
#define HDLC_DOUT_BUF_PTR_NE_0_MSK      (0x1UL << HDLC_DOUT_BUF_PTR_NE_0_POS)
#define HDLC_DOUT_BUF_PTR_NE_0          HDLC_DOUT_BUF_PTR_NE_0_MSK              /*!<Dout buffer write pointer not equal 0 */
#define HDLC_DOUT_FIFO_NOT_EMPTY_POS    (4)
#define HDLC_DOUT_FIFO_NOT_EMPTY_MSK    (0x1UL << HDLC_DOUT_FIFO_NOT_EMPTY_POS)
#define HDLC_DOUT_FIFO_NOT_EMPTY        HDLC_DOUT_FIFO_NOT_EMPTY_MSK            /*!<Dout_fifo status */
#define HDLC_AXIW_BUSY_POS              (3)
#define HDLC_AXIW_BUSY_MSK              (0x1UL << HDLC_AXIW_BUSY_POS)
#define HDLC_AXIW_BUSY                  HDLC_AXIW_BUSY_MSK                      /*!<Axi write sub module busy */
#define HDLC_DOUT_FIFO_LEVEL_8_POS      (2)
#define HDLC_DOUT_FIFO_LEVEL_8_MSK      (0x1UL << HDLC_DOUT_FIFO_LEVEL_8_POS)
#define HDLC_DOUT_FIFO_LEVEL_8          HDLC_DOUT_FIFO_LEVEL_8_MSK              /*!<Dout fifo level >= 8 */
#define HDLC_DIN_FIFO_NOT_EMPTY_POS     (1)
#define HDLC_DIN_FIFO_NOT_EMPTY_MSK     (0x1UL << HDLC_DIN_FIFO_NOT_EMPTY_POS)
#define HDLC_DIN_FIFO_NOT_EMPTY         HDLC_DIN_FIFO_NOT_EMPTY_MSK             /*!<Din fifo status */
#define HDLC_AXIR_BUSY_POS              (0)
#define HDLC_AXIR_BUSY_MASK             (0x1UL << HDLC_AXIR_BUSY_POS)
#define HDLC_AXIR_BUSY                  HDLC_AXIR_BUSY_MASK                     /*!<Axi read sub module busy */


#define HDLC_FIFO_DEPTH                 (16)

/**
 * @brief  HDLC_TypeDef
 */
typedef struct {
    __IO uint32_t                         version;                     /*!< Offset 0x00000000 */
    __IO uint32_t                         config;                      /*!< Offset 0x00000004 */
    __IO uint32_t                         start;                       /*!< Offset 0x00000008 */
    __IO uint32_t                         flush;                       /*!< Offset 0x0000000c */
    __IO uint32_t                         addr0;                       /*!< Offset 0x00000010 */
    __IO uint32_t                         len0;                        /*!< Offset 0x00000014 */
    __IO uint32_t                         addr1;                       /*!< Offset 0x00000018 */
    __IO uint32_t                         len1;                        /*!< Offset 0x0000001c */
    __IO uint32_t                         obufBase;                    /*!< Offset 0x00000020 */
    __IO uint32_t                         obufSize;                    /*!< Offset 0x00000024 */
    __IO uint32_t                         obufRptr;                    /*!< Offset 0x00000028 */
    __IO uint32_t                         obufWptr;                    /*!< Offset 0x0000002c */
    __IO uint32_t                         cmdFifoStatus;               /*!< Offset 0x00000030 */
    __IO uint32_t                         flushStatus;                 /*!< Offset 0x00000034 */
} HDLC_TypeDef;

typedef enum {
    HDLC_MEMORY_TYPE,   ///< 输出到memory
    HDLC_UART_TYPE,     ///< 输出到UART外设
    HDLC_SPI_TYPE,      ///< 输出到SPI外设
} HDLC_Peripheral_Type;

typedef struct
{
    const struct HDLC_Res  *pHdlcRes;
    bool_t                  start;
    HDLC_TypeDef           *regs;
    HDLC_Peripheral_Type    type;
    void                   *peripheralHandle;
    uint8_t                *flushData;   // 用于睡眠前、唤醒后, HDLC发送几包无效空包, 辅助PC工具清除睡眠前接收的残留HDLC数据包
    uint32_t                flushDataSize;
} HDLC_Handle;

typedef enum {
    HDLC_FIFO0_Sel,     ///< FIFO0
    HDLC_FIFO1_Sel,     ///< FIFO1
} HDLC_Fifo_Sel;

/**
 ***********************************************************************************************************************
 * @brief           HDLC初始化接口
 *
 * @param[in]       type                        输出外设内型
 * @param[in]       peripheralHandle            输出外设句柄. CP侧忽略该入参               
 *
 * @return          请求结果.
 * @retval          DRV_OK                      初始化成功
 *                  <0                          失败
 ***********************************************************************************************************************
 */
int32_t HDLC_Init(HDLC_Peripheral_Type type, void *peripheralHandle);

/**
 ***********************************************************************************************************************
 * @brief           HDLC启动编码器.
 *
 * @param[in]                    
 *
 * @return          无.
 * @retval          
 *                  
 ***********************************************************************************************************************
 */
void HDLC_Start(void);

/**
 ***********************************************************************************************************************
 * @brief           检查HDLC FIFO是否有空位
 *
 * @param[in]       
 *
 * @return          检查结果.
 * @retval          OS_TRUE                     有空位
 *                  OS_FALSE                    无空位
 ***********************************************************************************************************************
 */
bool_t HDLC_GetFifoNotFull();

/**
 ***********************************************************************************************************************
 * @brief           HDLC编码请求, 非阻塞
 *
 * @param[in]       data                        数据指针
 * @param[in]       len                         数据长度
 * @param[in]       compressFlag                是否启用压缩. 0:不压缩; 1:压缩
 *
 * @return          请求结果.
 * @retval          DRV_OK                      请求成功
 *                  DRV_ERR_FULL                请求失败, FIFO已满
 *                  DRV_ERR_PARAMETER           请求失败, 无效输入参数
 ***********************************************************************************************************************
 */
int32_t HDLC_Request(uint8_t *data, uint16_t len, bool_t compressFlag);

/**
 ***********************************************************************************************************************
 * @brief           获取命令FIFO中的空位数.
 *                  
 * @note            据(FIFO深度-SPACE_CNT)可算出FIFO中待处理的请求数.
 *                  软件维护的已请求待处理数减去这里算出的待处理数即为已处理的请求数.
 *                  根据软件维护的历史记录释放对应的内存空间即可.
 *
 * @param[in]       无
 *
 * @return          获取结果.
 * @retval          >=0                         空位数
 *                  <0                          异常
 ***********************************************************************************************************************
 */
int32_t HDLC_GetCmdFifoSpaceCount();

/**
 ***********************************************************************************************************************
 * @brief           检查HDLC FIFO是否为空
 *
 * @param[in]       fifoSel                     fifo选择
 *
 * @return          检查结果.
 * @retval          OS_TRUE                     空
 *                  OS_FALSE                    非空
 ***********************************************************************************************************************
 */
bool_t HDLC_IsFifoEmpty(HDLC_Fifo_Sel fifoSel);

void HDLC_ObufConfig(uint8_t *data, uint32_t len);

uint32_t HDLC_GetObufBase();

uint32_t HDLC_GetObufSize();

uint32_t HDLC_GetObufWritePtr();

uint32_t HDLC_GetObufReadPtr();

void HDLC_SetObufReadPtr(uint32_t readPtr);

void HDLC_SetFlushData(uint8_t *flushData, uint32_t flushDataSize);

uint32_t HDLC_GetFlushStatus();

void HDLC_PsmFlushData(uint32_t cnt);

void HDLC_Stop(void);
void HDLC_Flush();

#endif /* End of _DRV_HDLC_H_ */

/** @} */