/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        comm.h
 *
 * @brief       通讯头文件.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-19     ICT Team         创建
 ************************************************************************************
 */

#ifndef _COMM_H_
#define _COMM_H_

#include "os.h"
#include "slog_def.h"

#ifndef _OS_WIN
#include "drv_common.h"
#include "drv_uart.h"
#endif

#ifndef _OS_WIN
#define MDL_PHONE                 (1)
#endif
#define MDL_TOOLS_INVALID         (0)
#define MDL_TOOLS_BASE            (100)
#define MDL_CATCH_DATA            (MDL_TOOLS_BASE + 0)
#define MDL_NV                    (MDL_TOOLS_BASE + 3)
#define MDL_SLOG                  (MDL_TOOLS_BASE + 15)
#define MDL_SIMMTS                (MDL_TOOLS_BASE + 18) // 118

#define SUBMDL_TOOL_AGENT_BASE    (85)
#define SUBMDL_SLOG_AGENT         (SUBMDL_TOOL_AGENT_BASE + 0)
#define SUBMDL_CATCH_DATA_AGENT   (SUBMDL_TOOL_AGENT_BASE + 1)
#define SUBMDL_TOOL_MTS 		  (SUBMDL_TOOL_AGENT_BASE + 2)	// 87

#define SUBMDL_OSSBASE            (10)
#define SUBMDL_SHELL              (SUBMDL_OSSBASE + 13)   ///< SHELL代理模块

#ifdef USE_TOP_ASIC
#define COMM_WAIT_SEND_CMPL_TIMEOUT         (2000)
#else
#define COMM_WAIT_SEND_CMPL_TIMEOUT         (250)
#endif // End of USE_TOP_ASIC
#if defined(USE_SLOG_DIAG)
#define COMM_READ_THREAD_STACK_SIZE         (3584)
#else
#define COMM_READ_THREAD_STACK_SIZE         (1280)
#endif // End of USE_SLOG_DIAG
#define COMM_WRITE_THREAD_STACK_SIZE        (1024)
#define COMM_MSG_TYPE                       (1)
#define COMM_PACKET_MSG_TYPE                (5)
#define COMM_MSG_UE_QUERY                   (6)
#ifndef USE_TOP_MTS
#if defined(USE_SLOG_DIAG)
#define COMM_MAX_READ_BUF_NUM               5
#define COMM_READ_BUFFER_SIZE               (512)
#define COMM_RECV_BUFFER_SIZE               (1024)
#define COMM_DECODE_BUFFER_SIZE             (1536)
#else
#define COMM_MAX_READ_BUF_NUM               5
#define COMM_READ_BUFFER_SIZE               (160)
#define COMM_RECV_BUFFER_SIZE               (800)
#define COMM_DECODE_BUFFER_SIZE             (1024 * 1)
#endif // End of USE_SLOG_DIAG
#else
#define COMM_MAX_READ_BUF_NUM               4
#define COMM_READ_BUFFER_SIZE               (1024 * 2)
#define COMM_RECV_BUFFER_SIZE               (1024 * 8)
#define COMM_DECODE_BUFFER_SIZE             (1024 * 12)
#endif // End of USE_TOP_MTS

#define COMM_RECV_IDLE                      (0)
#define COMM_RECV_BUSY                      (1)
#define COMM_RECV_ERR                       (2)

typedef struct
{
    uint8_t     msgType;            ///< comm msg type, COMM_MSG_TYPE
    uint8_t     pad;                ///< pad
    uint16_t    reserved;           ///< 0xAAAA
    uint8_t     tgtMdlNo;           ///< target module no
    uint8_t     tgtSubMdlNo;        ///< target module sub module no
    uint8_t     srcMdlNo;           ///< source module no
    uint8_t     srcSubMdlNo;        ///< source module sub module no
    uint32_t    bufLen;             ///< msg data len
} COMM_Header;

typedef enum
{
    LOG_CHANNEL_INVALID = -1,
    LOG_CHANNEL_RSVD,
    LOG_CHANNEL_UART,
    LOG_CHANNEL_SPI,
} COMM_LogChannel;

typedef enum
{
    LOG_HDLC_INVALID = -1,
    LOG_HDLC_DISABLE,
    LOG_HDLC_ENABLE,
} COMM_HdlcCtrl;

extern uint32_t Board_GetLogUartBaudRate(void);
extern bool_t Board_GetLogUartFlowCtrlEnable(void);

#if defined(_CPU_AP)
typedef struct
{
	uint8_t     *buf;
	uint32_t     bufIndex;
	uint32_t     bufSize;
} COMM_Hdlc_Buffer;

typedef void(*COMM_ModuleFunc)(const uint8_t *buf, uint32_t msgLen);
typedef int32_t (*COMM_SpiSend)(uint8_t *buf, uint32_t bufLen, bool_t immediately);

typedef struct
{
    SLOG_ListNode       node;
    uint32_t            subMdlNo;
    uint32_t            totalSendLen;
    uint32_t            totalRecvLen;
    COMM_ModuleFunc     moduleFunc;
} COMM_SubMdlInfo;

typedef enum
{
    LOG_DEV_INVALID = 0,
    LOG_DEV_INIT,
    LOG_DEV_WORK,
    LOG_DEV_WRITE_TIMEOUT,
} COMM_LogDevStatus;

typedef struct
{
    uint8_t    *bufPtr[COMM_MAX_READ_BUF_NUM];
    uint16_t    bufRecvSize[COMM_MAX_READ_BUF_NUM];
    uint16_t    bufSize;
    uint8_t     writeIndex;
    uint8_t     readIndex;
    uint8_t     count;
    uint8_t     state;
} COMM_ReadBuffer;

typedef struct
{
    uint8_t    *buf;
    uint32_t    bufSize;
} COMM_RecvBuffer;

typedef enum
{
    COMM_UART_SEND_IDLE = 0,
    COMM_UART_SEND_BUSY,
    COMM_UART_SEND_COMPLETE,
} COMM_UartSendState;

typedef struct
{
    SLOG_List             subModList;
    struct osMutex        regMutex;

#ifdef _OS_WIN
    void                 *uartHandle;
#else
    UART_Handle          *uartHandle;
#endif
    osCompletion          sendCmpl;
    osCompletion          recvCmpl;

    COMM_LogDevStatus     logDevStatus;
    COMM_LogChannel       logChannel;

    struct osThread       readThread;
    uint8_t              *readThreadStack;

#if defined(_OS_WIN) || defined(USE_LOG_WITHOUT_HDLC) || defined(ENABLE_SPI_SEND_LOG)
    struct osSemaphore    eventSem;
    struct osThread       writeThread;
    uint8_t              *writeThreadStack;
    volatile uint8_t      uartSendState;    // COMM_UartSendState。会在任务和中断中改变，任务中改变需要锁中断
#endif

#if defined(ENABLE_SPI_SEND_LOG) && !defined(USE_LOG_WITHOUT_HDLC)
    uint32_t              hdlcObufBaseAddr;
#endif

#if defined(ENABLE_SPI_SEND_LOG)
    volatile uint8_t      spiTransImmediately;
    COMM_SpiSend          spiSend;
    uint64_t              spiSendTotalSize;
    uint64_t              spiSendDropSize;
    uint64_t              spiSendOKSize;
    uint64_t              spiSendErrorSize;

    uint64_t              spiRecvTotalCount;
    uint64_t              spiRecvDropCount;
    uint64_t              spiRecvProcCount;
#endif

    COMM_ReadBuffer       readBuf;
    COMM_RecvBuffer       recvBuf;

    bool_t                init;
} COMM_Handle;

extern COMM_Handle g_commHandle;

int32_t COMM_Register(uint8_t subMdlNo, COMM_ModuleFunc moduleFunc);
int32_t COMM_Init(void);

#if defined(USE_TOP_MTS)
uint8_t *Comm_Malloc(uint32_t size);
void Comm_Free(void *memPtr);
int32_t COMM_SendData(uint8_t *buf, uint32_t bufLen, uint8_t tgtMdlNo,
                uint8_t tgtSubMdlNo, uint8_t srcSubMdlNo, bool_t cmdFlag);
#endif

#if defined(_OS_WIN) || defined(USE_LOG_WITHOUT_HDLC)
int32_t COMM_Transmit(uint8_t *buf, uint32_t len);
#endif

#if defined(USE_LOG_WITHOUT_HDLC)
int32_t COMM_UartTransmit(uint8_t *buf, uint16_t len);
#endif

#ifdef ENABLE_SPI_SEND_LOG
void COMM_SpiTransDone(uint8_t num, uint8_t *data[], uint32_t dataLen[], bool_t sendResultFlag);
void COMM_RegisterSpiChannel(COMM_SpiSend spiSend);
void COMM_LogMuxDisConnect();   // SPI吐LOG下使用
void COMM_LogMuxConnect();      // SPI吐LOG下使用
#endif
#endif  // End of defined(_CPU_AP)

#endif /* End of _COMM_H_ */