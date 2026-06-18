/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_dma.h
 *
 * @brief       DMA驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef DRV_DMA_H_
#define DRV_DMA_H_


/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>

/**
 * @addtogroup Dma
 */

/**@{*/

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define LLI_DESC_ITEM(src, dst, count, control)  {.SrcAddr = src, .DestAddr = dst, .Count = count, .Control = control, .LLI = 0, }

/**
 * @brief 逻辑通道总数为23
 */
#define DMA_TOTAL (0x17UL)

/**
 * @brief 通道个数定义
 */
#define DMA_CH_NUM 8

/**
 * @brief DMA传输最大长度
 */
#define DMA_MAX_TRANS_LEN 65532

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum
{
    DMA_RMOD_DEV  = 0,        /* peripheral req */
    DMA_RMOD_SOFT = 1,        /* soft req */
    DMA_RMOD_TOTAL
} Dma_ReqMod;

typedef enum
{
    DMA_AMOD_RAM  = 0,        /* RAM mode, addr increase when tr */
    DMA_AMOD_FIFO = 1,        /* FIFO mode, addr not change during tr */

    DMA_AMOD_TOTAL
} Dma_AddrMod;

typedef enum
{
    DMA_IMOD_ALL_DISABLE = 0,        /* Disable all irq */
    DMA_IMOD_TC_ENABLE   = 1,        /* TC irq enable */
    DMA_IMOD_ERR_ENABLE  = 2,        /* Err irq enable */
    DMA_IMOD_ALL_ENABLE  = 3,        /* Enable all irq */

    DMA_IMOD_TOTAL
} Dma_IrqMod;

typedef enum
{
    DMA_BSIZE_8BIT   = 0,
    DMA_BSIZE_16BIT  = 1,
    DMA_BSIZE_32BIT  = 2,
    DMA_BSIZE_64BIT  = 3,
    DMA_BSIZE_128BIT = 4,
    DMA_BSIZE_TOTAL
} Dma_BurstSize;


typedef enum
{
    DMA_BLEN_1 = 0,        /* 1 tr each burst */
    DMA_BLEN_2,            /* 2 tr each burst */
    DMA_BLEN_3,            /* 3 tr each burst */
    DMA_BLEN_4,            /* 4 tr each burst */
    DMA_BLEN_5,            /* 5 tr each burst */
    DMA_BLEN_6,            /* 6 tr each burst */
    DMA_BLEN_7,            /* 7 tr each burst */
    DMA_BLEN_8,            /* 8 tr each burst */
    DMA_BLEN_9,            /* 9 tr each burst */
    DMA_BLEN_10,           /* 10 tr each burst */
    DMA_BLEN_11,           /* 11 tr each burst */
    DMA_BLEN_12,           /* 12 tr each burst */
    DMA_BLEN_13,           /* 13 tr each burst */
    DMA_BLEN_14,           /* 14 tr each burst */
    DMA_BLEN_15,           /* 15 tr each burst */
    DMA_BLEN_16,           /* 16 tr each burst */

    DMA_BLEN_TOTAL
} Dma_BurstLen;

typedef enum
{
    DMA_INT1,
    DMA_INT2,
    DMA_INT3,
} Dma_IntSel;

typedef struct _DMA_CONTROL
{
  uint32_t Enable : 1;   /*DMA Enable*/
  uint32_t BurstReqMod :1; /* DMA Request mode*/
  uint32_t SrcMod  :1;      /* DMA Src addr */
  uint32_t DestMod :1;     /* DMA Dest addr */
  uint32_t IrqMod  :2;
  uint32_t SrcBurstSize :3; /* DMA burst size£¬Count must be integer multiple of
                                max(SrcBurstSize,DestBurstSize) */
  uint32_t SrcBurstLen :4 ;
  uint32_t DestBurstSize :3; /* DMA burst size£¬Count must be integer multiple of
                                max(SrcBurstSize,DestBurstSize) */
  uint32_t resverd1 :4;
  uint32_t IntSel   :4;        /* select witch core will deal with the dma int */
  uint32_t resverd2  :7;
  uint32_t ForceClose :1;
} Dma_Ctrl;

typedef struct _DMA_LliDesc
{
    uint32_t SrcAddr;            /**< reg:0x0,   R/W LLI Src Addr reg */
    uint32_t DestAddr;           /**< reg:0x4,   R/W LLI Dest Addr reg */
    uint32_t Count;              /**< reg:0x8,   R/W LLI X param: Src X count/Dest X count */
    uint32_t ZYpara;             /**< reg:0xc,   R/W LLI Y param: Y count/Y modify */
    uint32_t SrcZYstep;          /**< reg:0x10,  R/W LLI Src Z param: Z count/Z modify */
    uint32_t DestZYstep;         /**< reg:0x14,  R/W LLI Dest Y param: Y count/Y modify */
    uint32_t resv0;              /**< reg:0x18   R   Rsv */
    uint32_t LLI;                /**< reg:0x1C,  R/W LLI Addr reg, the last one must be NULL */
    Dma_Ctrl Control;            /**< reg:0x20,  R/W LLI Channel Ctrl */
} DMA_LliDesc;

typedef struct _Dma_Chan_Cfg
{
    uint32_t SrcAddr;                         /* DMA src addr */
    uint32_t DestAddr;                        /* DMA Dest addr */
    uint32_t Count;
    Dma_Ctrl Control;
} DMA_ChCfg;

typedef enum
{
    DMA_ISTAT_ERR,
    DMA_ISTAT_END,

    DMA_ISTAT_TOTAL
} Dma_IntStat;

typedef struct{
    Dma_IntStat state;
    void *para;
}Dma_CbkPara;

typedef enum
{
    INITIALIZE = 0,
    REQUESTED,
    RELEASED,
}DMA_CHSTATUS;

typedef struct _DMA_ChHandle
{
  void *reg;
  void          *para;
  void  (*callback)(void *para);
  int32_t logicID;
  uint32_t  ocuppied;
  DMA_CHSTATUS chStatus;
} DMA_ChHandle;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief           请求DMA通道
 *
 * @param[in]       logicID           请求映射物理DMA通道的逻辑通道号
 *
 * @return          请求结果.
 * @retval          == NULL           请求失败
 *                  != NULL           请求成功，返回通道句柄
 ************************************************************************************
 */
DMA_ChHandle *DMA_Request(uint32_t logicID);

/**
 ************************************************************************************
 * @brief           DMA 通道释放接口
 *
 * @param[in]       chHandle         DMA通道Handle
 *
 * @return          释放结果.
 * @retval          ==0              发送成功
 *                  < 0              失败
 ************************************************************************************
 */
void DMA_Release(DMA_ChHandle* chHandle);

/**
 ************************************************************************************
 * @brief           启动DMA
 *
 * @param[in]       chHandle           DMA通道Handle
 * @param[in]       CallBack           DMA通道中断回调函数
 * @param[in]       para               DMA通道中断回调函数参数
 * @return          请求结果.
 * @retval          ==0              发送成功
 *                  < 0              失败
 ************************************************************************************
 */
int32_t DMA_Start(DMA_ChHandle *chHandle, DMA_ChCfg *pChCfg);

/**
 ************************************************************************************
 * @brief           DMA lli传输启动
 *
 * @param[in]       chHandle           DMA通道Handle
 * @param[in]       lliDesc                DMA通道传输源地址
 * @param[in]       lliDescCnt               DMA通道传输目的地址
 * @return          请求结果.
 * @retval          ==0              发送成功
 *                  < 0              失败
 ************************************************************************************
 */
int32_t DMA_LliStart(DMA_ChHandle *chHandle, DMA_LliDesc *pLliDesc, uint32_t lliDescCnt);
/**
 ************************************************************************************
 * @brief           DMA lli循环传输启动
 *
 * @param[in]       chHandle           DMA通道Handle
 * @param[in]       lliDesc                DMA通道传输源地址
 * @param[in]       lliDescCnt               DMA通道传输目的地址
 * @return          请求结果.
 * @retval          ==0              发送成功
 *                  < 0              失败
 ************************************************************************************
 */
int32_t DMA_LliLoopStart(DMA_ChHandle *chHandle, DMA_LliDesc *pLliDesc, uint32_t lliDescCnt);
/**
 ************************************************************************************
 * @brief           关闭DMA
 *
 * @param[in]       chHandle           DMA通道Handle
 * @return          请求结果.
 * @retval          ==0              成功
 ************************************************************************************
 */
int32_t DMA_Stop(DMA_ChHandle *chHandle);

/**
 ************************************************************************************
 * @brief           获取DMA通道当前传输长度（仅用于非链表方式）
 *
 * @param[in]       chHandle           DMA通道Handle
 * @return          请求结果.
 * @retval
 ************************************************************************************
 */
uint32_t DMA_GetCount(DMA_ChHandle *chHandle);

/**
 ************************************************************************************
 * @brief           获取DMA通道状态
 *
 * @param[in]       chHandle           DMA通道Handle
 * @return          请求结果.
 * @retval          1 工作状态 0 idle
 ************************************************************************************
 */
uint8_t DMA_GetStatus(DMA_ChHandle *chHandle);

/**
 ************************************************************************************
 * @brief           获取所有申请的DMA通道状态状态
 *
 * @param[in]       void
 * @return          请求结果.
 * @retval          非0 工作状态 0 idle
 ************************************************************************************
 */
uint32_t DMA_GetALLStatus(void);
#endif

/** @} */