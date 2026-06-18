/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        slog_helper.h
 *
 * @brief       SLOG辅助函数头文件.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-19     ICT Team         创建
 ************************************************************************************
 */

#ifndef _SLOG_HELPER_H_
#define _SLOG_HELPER_H_

#include "os.h"
#include "slog_def.h"
#include "slog.h"
#include "comm.h"
#if defined(_CPU_AP)
#include "nvparam_pubcfg.h"
#include "nvm.h"
#endif

#define SLOG_BLOCK_BUF_FULL_THRESHOLD   sizeof(COMM_Header)
#define SLOG_COMPRESS_FLAG              OS_TRUE
#define SLOG_UNCOMPRESS_FLAG            OS_FALSE
#if defined(_CPU_AP)
#define SLOG_ITCM_CODE                  //__ITCM_CODE
#define SLOG_DTCM_RO                    //__DTCM_RO
#define SLOG_DTCM_RT                    //__DTCM_RT
#define SLOG_DTCM_SR                    //__DTCM_SR
#else
#define SLOG_ITCM_CODE                  __ITCM_CODE
#define SLOG_DTCM_RO                    __DTCM_RO
#define SLOG_DTCM_RT                    __DTCM_RT
#define SLOG_DTCM_SR                    __DTCM_SR
#endif
#define SLOG_TOTAL_SUBFRAMES_OF_SFN_CYCLE   10240
#define SLOG_U32_INC(x)                 ((x) = (((x) == 0xFFFFFFFF) ? 0 : ((x) + 1)))
#define SLOG_HDLC_CTRL_SUSPEND          1
#define SLOG_HDLC_CTRL_RESUME           2
#define SLOG_INVALID_BLOCK_INDEX        0x3F
#if defined(_CPU_AP)
#define SLOG_HDLC_REQ_THRESHOLD         2
#else
#define SLOG_HDLC_REQ_THRESHOLD         2
#endif

#if defined(_CPU_AP) || defined (USE_TOP_MINI)
#define SLOG_GET_SFN                    (PS_LTE_GET_HFN)
#else
extern uint32_t L1_Tpu_GetTimeSubframNum();
#define SLOG_GET_SFN                    L1_Tpu_GetTimeSubframNum()
#endif
extern int32_t g_slogIsrLogTotalLen;
extern int32_t g_slogMallocLogTotalLen;
extern int32_t g_slogIsrLogHisMaxLen;
extern int32_t g_slogMallocLogHisMaxLen;
extern SLOG_DTCM_SR int8_t g_slogHdlcCtrl;
extern bool_t osIsPanic(void);

typedef void(*SLOG_FlushFunc)(void);

typedef struct
{
    const void *owner;  ///< SLOG_BufBlock
	uint8_t    *buffer;
}SLOG_Buffer;

typedef struct
{
    uint32_t    magic;
    uint32_t    freeBlockBitMap;
    uint8_t     linkStatus;
    uint8_t     printWndBitMap;
    uint8_t     filterChangeFlag;
    uint8_t     apHdlcSuspend:1;
    uint8_t     cpHdlcSuspend:1;
    uint8_t     rsvd:6;
    uint8_t     psFilterMask[SLOG_LEVEL_NUM][SLOG_MASK_BYTES_NUM];
    uint8_t     phyFilterMask[SLOG_LEVEL_NUM][SLOG_MASK_BYTES_NUM];
    uint8_t     prnFilterMask[SLOG_LEVEL_NUM][SLOG_MASK_BYTES_NUM];
    uint8_t     platFilterMask[SLOG_LEVEL_NUM][SLOG_MASK_BYTES_NUM];
    uint32_t    cpMagic;
    uint32_t    cpPrivateFreeBlockBitMap;
    SLOG_List   cpCachedBlocks;
    uint32_t    hdlcReadPtr;
    uint32_t    apTimeStamp;
    uint16_t    logCtrlValue;
    uint8_t     logCtrlEnable;
    int8_t      logChannel;
    int8_t      hdlcCtrl;
    uint8_t     uartFlowCtrlEnable;
    uint8_t     rsvd2[2];
    uint32_t    uartRegBase;
}SLOG_ShareCtrl;

typedef enum
{
    SLOG_BUF_BLOCK_TYPE_CMD = 0,
    SLOG_BUF_BLOCK_TYPE_PRIVATE,
    SLOG_BUF_BLOCK_TYPE_SHARE,
    SLOG_BUF_BLOCK_TYPE_MALLOC_ALIGN,
    SLOG_BUF_BLOCK_TYPE_CATCH_DATA,
    SLOG_BUF_BLOCK_TYPE_CATCH_DATA_ERR,
} SLOG_BUF_BLOCK_TYPE;

typedef enum
{
    SLOG_CORE_TYPE_UNKNOWN = 0,
    SLOG_CORE_TYPE_AP,
    SLOG_CORE_TYPE_CP,
} SLOG_CORE_TYPE;

typedef struct
{
    uint8_t    *addr;          ///< NULL, 未分配Block
    uint16_t    capacity;
    uint16_t    writePos;
    uint16_t    commHeaderPos;
    uint8_t     blockIndex:6;
    uint8_t     coreType:2;
    uint8_t     bufBlockType:3;
    uint8_t     compressFlag:1;
    uint8_t     rsvd:4;
    uint64_t    timeStamp;
}SLOG_BufBlock;

typedef struct
{
    uint32_t packetNum;
    SLOG_BufBlock packetBlock[SLOG_MAX_PACKET_NUM];
} SLOG_PacketBuffer;

typedef struct
{
    SLOG_ListNode   node;
    uint8_t        *addr;
    uint16_t        size;
    uint8_t         blockIndex:6;
    uint8_t         coreType:2;
    uint8_t         bufBlockType:3;
    uint8_t         compressFlag:1;
    uint8_t         packetNum:3;
    uint8_t         packetFlag:1;
}SLOG_CachedBufBlock;

typedef struct
{
    SLOG_ListNode   node;
    uint16_t        size;
    uint8_t         compressFlag;
    uint8_t         rsvd;
}SLOG_CachedIntPrint;

typedef struct
{
    bool_t           init;
    struct osMutex   mutex;

    SLOG_ShareCtrl  *shareCtrlAddr;           ///< AP/CP共享控制
    uint8_t         *shareBufBaseAddr;        ///< AP/CP共享使用
    uint32_t         shareBufTotalSize;       ///< AP/CP共享使用
#if defined(_CPU_AP)
    uint32_t         cmdFreeBlockBitMap;      ///< 与PC工具进行交互的命令类消息
    uint8_t         *cmdBaseAddr;
    uint32_t         cmdTotalSize;
    SLOG_BufBlock    cmdBufBlock;
#endif

    uint32_t         freeBlockBitMap;         ///< AP或CP专用
    uint8_t         *baseAddr;                ///< AP或CP专用
    uint32_t         totalSize;
    SLOG_BufBlock    compressBufBlock;
    SLOG_BufBlock    uncompressBufBlock;

    SLOG_List        cachedBlocks;            ///< 等待配置给HDLC硬件的block队列
    SLOG_List        hdlcReqOkBlocks;         ///< 软件HDLC_Request请求成功的block队列

#if defined(_CPU_AP) && defined(USE_LOG_WITHOUT_HDLC)
    SLOG_List        cpCachedBlocks;
    SLOG_List        collectSendBlocks;       ///< 收集的来自AP/CP的block队列
    SLOG_List        sendBlocks;              ///< 等待配置给驱动发送的block队列, 中断中会使用, 需要锁中断
    SLOG_List        sendCompleteBlocks;      ///< 驱动发送完成的队列, 中断中会使用, 需要锁中断
    SLOG_List        sendingBlocks;           ///< 正在发送的block队列, 中断中会使用, 需要锁中断
#endif

#if defined(_CPU_AP)
    SLOG_List        cachedIntPrints;         ///< 中断打印队列
    SLOG_List        cachedExpPrints;         ///< 异常打印队列
#endif
    uint32_t         hdlcReqOkBlockCnt;       ///< 软件HDLC_Request请求成功的block个数
} SLOG_BufferPool;

typedef struct
{
    uint32_t    ctrlId;
} SLOG_HdlcControl;

extern volatile bool_t g_slogHdlcSuspend;
extern volatile bool_t g_slogSuspend;
extern SLOG_DTCM_SR SLOG_BufferPool g_slogBufPool;
#if defined(_CPU_AP)
extern NV_LogConfig g_slogNvCtrl;
#endif

OS_INLINE void SLOG_ListInit(SLOG_List *l)
{
    l->head = OS_NULL;
    l->tail = OS_NULL;
    l->count = 0;
}

OS_INLINE void SLOG_ListInsertTail(SLOG_List *l, SLOG_ListNode *n)
{
    n->next = OS_NULL;

    if (OS_NULL == l->head)
    {
        l->count = 0;
        l->head = n;
    }
    else
    {
        l->tail->next = n;
    }

    l->tail = n;
    l->count++;
}

OS_INLINE void SLOG_ListInsertHead(SLOG_List *l, SLOG_ListNode *n)
{
    if (OS_NULL == l->head)
    {
        l->count = 0;
        n->next = OS_NULL;
        l->head = n;
        l->tail = n;
    }
    else
    {
        n->next = l->head;
        l->head = n;
    }

    l->count++;
}

OS_INLINE uint32_t SLOG_ListLen(const SLOG_List *l)
{
    return l->count;
}

OS_INLINE void SLOG_ListRemove(SLOG_List *l, SLOG_ListNode *n)
{
    if (0 != l->count)
    {
        SLOG_ListNode *node = l->head;
        if (node == n)
        {
            l->head = l->head->next;

            if (l->tail == n)
            {
                l->tail = l->head;
            }
            l->count--;
        }
        else
        {
            while (node->next && node->next != n) node = node->next;

            if (node->next != OS_NULL)
            {
                node->next = node->next->next;

                if (l->tail == n)
                {
                    l->tail = node;
                }

                l->count--;
            }
        }
    }
}

OS_INLINE SLOG_ListNode *SLOG_ListFirst(SLOG_List *l)
{
    return l->head;
}

OS_INLINE SLOG_ListNode *SLOG_ListNext(SLOG_ListNode *n)
{
    return n->next;
}

OS_INLINE int SLOG_ListIsEmpty(SLOG_List *l)
{
    return 0 == l->count;
}

// merge后, n是空链表
OS_INLINE void SLOG_ListMergeList(SLOG_List *l, SLOG_List *n)
{
    if (OS_NULL == l->head)
    {
        l->count = 0;
        l->head = n->head;
        l->tail = n->tail;
    }
    else
    {
        l->tail->next = n->head;
        l->tail = n->tail;
    }

    l->count += n->count;
    n->head = OS_NULL;
    n->tail = OS_NULL;
    n->count = 0;
}

// add后, n保持不变
OS_INLINE void SLOG_ListAddList(SLOG_List *l, SLOG_List *n)
{
    if (OS_NULL == l->head)
    {
        l->count = 0;
        l->head = n->head;
        l->tail = n->tail;
    }
    else
    {
        l->tail->next = n->head;
        l->tail = n->tail;
    }

    l->count += n->count;
}

#if defined(_OS_WIN) || (defined(_CPU_AP) && defined(USE_LOG_WITHOUT_HDLC))
SLOG_CachedBufBlock* SLOG_GetCachedBlock();
void SLOG_FreeCachedBlock(SLOG_CachedBufBlock *cachedBufBlock, uint32_t *shareFreeBitMap);
void SLOG_FreeCpAndShareCachedBlock(uint32_t cpPrivateFreeBitMap, uint32_t shareFreeBitMap);
#endif

#if defined(_CPU_AP) && defined(USE_LOG_WITHOUT_HDLC)
void SLOG_FreeSendCompleteBlocks();
#endif
#if defined(_CPU_CP)
void SLOG_CheckNeedLogWakeUp();
#endif

void SLOG_FreeSendReqBufferBlock();
SLOG_Buffer SLOG_GetCommBuffer(bool_t compressFlag, uint16_t len, uint32_t timeOut);
void SLOG_PutCommBuffer(SLOG_Buffer slogBuf, uint16_t len);
#if defined(_CPU_AP)
SLOG_Buffer SLOG_GetCmdCommBuffer(uint8_t msgType, uint16_t len, uint32_t timeOut);
void SLOG_PutCmdCommBuffer(SLOG_Buffer slogBuf, uint16_t len);
void SLOG_PutCmdCommBufferUrgent(SLOG_Buffer slogBuf, uint16_t len);
bool_t SLOG_GetCommBufferPacket(uint16_t len, SLOG_PacketBuffer *packetBuffer, uint32_t timeOut);
void SLOG_PutCommBufferPacket(SLOG_PacketBuffer *packetBuffer);
void SLOG_UpdateTimeStamp();
SLOG_Buffer SLOG_GetCommMallocBuffer(bool_t compressFlag, uint16_t len);
void SLOG_PutCommMallocBuffer(SLOG_Buffer slogBuf, uint16_t len);
void SLOG_PutCommMallocBufferUrgent(SLOG_Buffer slogBuf, uint16_t len);
void SLOG_CleanCommMallocBufferInLinkOff();
SLOG_ITCM_CODE void SLOG_AddBufBlockToCachedBlocks(SLOG_BufBlock *bufBlock);
#endif
int32_t SLOG_BufferPoolInit(uint8_t *shareCtrlAddr, uint32_t shareCtrlSize,
                            uint8_t *shareBaseAddr, uint32_t shareTotalSize, uint16_t shareBlockSize,
                            uint8_t *baseAddr, uint32_t totalSize, uint16_t blockSize);
void SLOG_HeaderInit(SLOG_Header *slogHeader, uint8_t msgType, uint16_t len);
SLOG_ShareCtrl *SLOG_GetShareCtrlAddr();
#if defined(USE_TOP_MTS) && defined(_CPU_AP)
int32_t SLOG_WriteCommBuffer(uint8_t *commBuf, uint16_t commBuflen, uint32_t timeOut, bool_t cmdFlag);
#endif
void SLOG_CheckBufBlockTimeout(bool_t forceTimeout, uint32_t mutexWaitTime);
void SLOG_RegisterFlushFunc(SLOG_FlushFunc flushFunc);
SLOG_CORE_TYPE SLOG_GetCoreType();
osStatus_t SLOG_LockBufPool(uint32_t timeout);
void SLOG_UnlockBufPool();
void SLOG_CheckCachedIntPrints();
void SLOG_CheckCachedBlocks();
void SLOG_CheckLogQueue();
bool_t SLOG_CheckPsmFlow();
SLOG_ITCM_CODE void *SLOG_Memcpy(void *dst, const void *src, ubase_t count);
__ITCM_CODE void SLOG_LockShareBlockBitMap(ubase_t *plevel);
__IRAM_CODE osStatus_t SLOG_TryLockShareBlockBitMap(ubase_t *plevel);
__ITCM_CODE void SLOG_UnlockShareBlockBitMap(ubase_t level);
SLOG_ITCM_CODE uint16_t SLOG_VintEncode(uint8_t *vintEncodeBuf, uint16_t vintEncodeBufMaxSize, uint8_t paraNum, va_list args);
void SLOG_IdleHandle(void);
void SLOG_CheckLogUartCTSSignal();

#endif /* End of _SLOG_HELPER_H_ */