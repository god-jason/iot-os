/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_uart.h
 *
 * @brief       uart驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef DRIVER_UART_H_
#define DRIVER_UART_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>
#include <drv_dma.h>
#include <os_def.h>
#if defined(OS_USING_PM) && defined(_CPU_AP)
#include <psm_wakelock.h>
#endif
#if defined(_CPU_AP)
#include "ringbuffer.h"
#endif
/**
 * @addtogroup Uart
 */

/**@{*/

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
/****** UART Control Codes *****/

#define UART_CONTROL_Pos 0
#define UART_CONTROL_Msk (0xFFUL << UART_CONTROL_Pos)

/*----- UART Control Codes: Mode -----*/
#define UART_MODE_IRDA (0x01UL << UART_CONTROL_Pos) ///< UART IrDA; arg = Baudrate
/*----- UART Control Codes: Miscellaneous Controls  -----*/
#define UART_CONTROL_TX    (0x02UL << UART_CONTROL_Pos) ///< Transmitter; arg: 0=disabled, 1=enabled
#define UART_CONTROL_RX    (0x03UL << UART_CONTROL_Pos) ///< Receiver; arg: 0=disabled, 1=enabled
#define UART_CONTROL_AUTO_BAUD (0x04UL << UART_CONTROL_Pos) ///< set auto BAUD
#define UART_ABORT_SEND    (0x05UL << UART_CONTROL_Pos) ///< Abort \ref UART_Send
#define UART_ABORT_RECEIVE (0x06UL << UART_CONTROL_Pos) ///< Abort \ref UART_Receive
#define UART_SET_TX_FIFO_SEL (0x07UL << UART_CONTROL_Pos) ///< set tx fifo sel
#define UART_SET_SPECIAL_CHAR (0x08UL << UART_CONTROL_Pos) ///< set special char
#define UART_UART0_RESET (0x09UL << UART_CONTROL_Pos) ///< uart0 Reset
#define UART_UART2_RESET (0x0AUL << UART_CONTROL_Pos) ///< uart2 Reset
#define UART_CONTROL_LOOPBACK (0x0BUL << UART_CONTROL_Pos) ///< uart loop back
#define UART_GET_SUPPORTED_BAUDERATE_LIST (0x0CUL << UART_CONTROL_Pos) ///< get supported baudrate list

/*----- UART Control Codes: Mode Parameters: Data Bits -----*/
#define UART_DATA_BITS_Pos 8
#define UART_DATA_BITS_Msk (7UL << UART_DATA_BITS_Pos)
#define UART_DATA_BITS_5   (5UL << UART_DATA_BITS_Pos) ///< 5 Data bits
#define UART_DATA_BITS_6   (6UL << UART_DATA_BITS_Pos) ///< 6 Data bit
#define UART_DATA_BITS_7   (7UL << UART_DATA_BITS_Pos) ///< 7 Data bits
#define UART_DATA_BITS_8   (0UL << UART_DATA_BITS_Pos) ///< 8 Data bits (default)
#define UART_DATA_BITS_9   (1UL << UART_DATA_BITS_Pos) ///< 9 Data bits

/*----- UART Control Codes: Mode Parameters: Parity -----*/
#define UART_PARITY_Pos  12
#define UART_PARITY_Msk  (3UL << UART_PARITY_Pos)
#define UART_PARITY_NONE (0UL << UART_PARITY_Pos) ///< No Parity (default)
#define UART_PARITY_EVEN (1UL << UART_PARITY_Pos) ///< Even Parity
#define UART_PARITY_ODD  (2UL << UART_PARITY_Pos) ///< Odd Parity

/*----- UART Control Codes: Mode Parameters: Stop Bits -----*/
#define UART_STOP_BITS_Pos 14
#define UART_STOP_BITS_Msk (3UL << UART_STOP_BITS_Pos)
#define UART_STOP_BITS_1   (0UL << UART_STOP_BITS_Pos) ///< 1 Stop bit (default)
#define UART_STOP_BITS_2   (1UL << UART_STOP_BITS_Pos) ///< 2 Stop bits

/*----- UART Control Codes: Mode Parameters: Flow Control -----*/
#define UART_FLOW_CONTROL_Pos     16
#define UART_FLOW_CONTROL_Msk     (3UL << UART_FLOW_CONTROL_Pos)
#define UART_FLOW_CONTROL_NONE    (0UL << UART_FLOW_CONTROL_Pos) ///< No Flow Control (default)
#define UART_FLOW_CONTROL_RTS     (1UL << UART_FLOW_CONTROL_Pos) ///< RTS Flow Control
#define UART_FLOW_CONTROL_CTS     (2UL << UART_FLOW_CONTROL_Pos) ///< CTS Flow Control
#define UART_FLOW_CONTROL_RTS_CTS (3UL << UART_FLOW_CONTROL_Pos) ///< RTS/CTS Flow Control

/****** UART specific error codes *****/
#define UART_ERROR_MODE         (DRV_ERR_SPECIFIC - 1) ///< Specified Mode not supported
#define UART_ERROR_BAUDRATE     (DRV_ERR_SPECIFIC - 2) ///< Specified baudrate not supported
#define UART_ERROR_DATA_BITS    (DRV_ERR_SPECIFIC - 3) ///< Specified number of Data bits not supported
#define UART_ERROR_PARITY       (DRV_ERR_SPECIFIC - 4) ///< Specified Parity not supported
#define UART_ERROR_STOP_BITS    (DRV_ERR_SPECIFIC - 5) ///< Specified number of Stop bits not supported
#define UART_ERROR_FLOW_CONTROL (DRV_ERR_SPECIFIC - 6) ///< Specified Flow Control not supported


// UART flags
#define UART_FLAG_INITIALIZED ((uint8_t)(1U))
#define UART_FLAG_POWERED     ((uint8_t)(1U << 1))
#define UART_FLAG_CONFIGURED  ((uint8_t)(1U << 2))
#define UART_FLAG_TX_ENABLED  ((uint8_t)(1U << 3))
#define UART_FLAG_RX_ENABLED  ((uint8_t)(1U << 4))
#define UART_FLAG_AUTO_BAUD   ((uint8_t)(1U << 5))

/****** UART Event *****/
#define UART_EVENT_SEND_COMPLETE        (1UL << 0)  ///< Send completed; however UART may still transmit data
#define UART_EVENT_RECEIVE_COMPLETE     (1UL << 1)  ///< Receive completed
#define UART_EVENT_RECEIVE_SPECIAL_CHAR (1UL << 2)  ///< Transfer completed
#define UART_EVENT_TX_COMPLETE          (1UL << 3)  ///< Transmit completed (optional)
#define UART_EVENT_TX_UNDERFLOW         (1UL << 4)  ///< Transmit data not available (Synchronous Slave)
#define UART_EVENT_RX_OVERFLOW          (1UL << 5)  ///< Receive data overflow
#define UART_EVENT_RX_TIMEOUT           (1UL << 6)  ///< Receive character timeout (optional)
#define UART_EVENT_RX_BREAK             (1UL << 7)  ///< Break detected on receive
#define UART_EVENT_RX_FRAMING_ERROR     (1UL << 8)  ///< Framing error detected on receive
#define UART_EVENT_RX_PARITY_ERROR      (1UL << 9)  ///< Parity error detected on receive
#define UART_EVENT_CTS                  (1UL << 10) ///< CTS state changed (optional)
#define UART_EVENT_DSR                  (1UL << 11) ///< DSR state changed (optional)
#define UART_EVENT_DCD                  (1UL << 12) ///< DCD state changed (optional)
#define UART_EVENT_RI                   (1UL << 13) ///< RI  state changed (optional)
#define UART_EVENT_RX_ARRIVED           (1UL << 14) ///< Receive new data
/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

typedef void (*UART_Callback)(void *UART, uint32_t event); ///< Pointer to \ref UART_SignalEvent : Signal UART Event.
typedef void (*UART_ReAdaptBaud)(void);       ///< Pointer to reAdaptBaud.

// uart supported bauderate
  typedef struct _UART_SUPPORTED_BAUDERATE
  {

    uint32_t *bauderateList;
    uint32_t bauderateNum;

  }UartSupportedBauderate;
  // UART STATUS (Run-Time)
  typedef struct _UART_STATUS
  {
    uint8_t tx_busy;          // Transmitter busy flag
    uint8_t rx_busy;          // Receiver busy flag
    uint32_t setBauderate;    //对设置的波特率进行保存
    uint8_t wordLen;          //数据位  0x3: 8bit;  0x2: 7bit; 0x1 6bit; 0x0 5bit
    uint8_t stopBit;          //停止位  0x1: 2 Stop bits  0x0: 1 Stop bit (default)
    uint8_t flowControl;      //流控制  0x3:RTS/CTS Flow Control  0x2:CTS Flow Control  0x1:RTS Flow Control  0x0:No Flow Control (default)
    uint8_t parity;           //奇偶校验位  0x2:Odd Parity  0x1:Even Parity  0x0:No Parity (default)
    uint32_t baudRate;        //查询已配置的波特率
  } UART_STATUS;

  // UART Transfer Information (Run-Time)
  typedef struct _UART_TRANSFER_INFO
  {
    uint32_t rx_num;      // Total number of receive data
    uint32_t tx_num;      // Total number of transmit data
    uint8_t *rx_buf;      // Pointer to in data buffer
    uint8_t *tx_buf;      // Pointer to out data buffer
    uint32_t rx_cnt;      // Number of data received
    uint32_t tx_cnt;      // Number of data sent
    uint8_t  break_flag;  // Transmit break flag
    uint8_t  send_active; // Send active flag
  } UART_TRANSFER_INFO;

  typedef struct _UartDmaInfo
  {
    DMA_ChHandle *rxDmaHandle;
    DMA_ChHandle *txDmaHandle;

    DMA_ChCfg rxChCfg;
    DMA_ChCfg txChCfg;
  } UartDmaInfo;

  // UART Information (Run-time)
  typedef struct _UART_INFO
  {
    UART_Callback cb_event;     // Event Callback
    UART_STATUS status;       // Status flags
    uint8_t flags;        // Current UART flags
    uint8_t flow_control; // Flow control
    uint32_t mask;         // interrupt mask set
    UartDmaInfo dmaInfo;
    uint32_t      workClkFreq;
    uint8_t  select_Clock; //0:9600 32K 1: 9600 26M
    #if defined(OS_USING_PM) && defined(_CPU_AP)
    WakeLock lock;
    #endif
  } UART_INFO;

  /**
  \brief UART Device Driver Capabilities.
  */
  typedef struct _UART_CAPABILITIES
  {
    uint32_t rxDMAEnable : 1;        // 0:disable 1:enable
    uint32_t txDMAEnable : 1;        // 0:disable 1:enable
    uint32_t flow_control_rts : 1;   // 0:disable 1:enable
    uint32_t flow_control_cts : 1;   // 0:disable 1:enable
    uint32_t useForHDLC : 1;         // 0:general 1:HDLC
    uint32_t wakeup : 1;             // 0:disable 1:enable  lpuart可配置支持唤醒，其他UART不支持
  } UART_CAPABILITIES;

  typedef struct
  {
      void *addr;              //AT端口寄存器的基地址
      uint8_t store[128];      //AT端口数据的临时保存地址
      uint32_t size;           //AT端口数据的临时保存长度
      bool_t sleep;            //AT端口的状态 0:work;1:sleep
  }UART_RestoreInfo;

  typedef struct
  {
      void *addr;
      uint8_t store[128];
      uint8_t size;
      bool_t  wfi;
  }UART_WFIInfo;

/**
  \brief uart function
  */
  typedef enum
  {
    UART_PORT_USER = 0,            //AP侧供用户使用，不具备唤醒功能，156M时钟
    UART_PORT_AT ,                 //AT功能,具备唤醒功能，32K(9600)或者26M(<=115200)工作时钟，156M时钟;具备自适应功能，最高自适应115200
    UART_PORT_CONSOLE,             //控制台调试功能，不具备唤醒功能，156M时钟，在AP或者CP切换
    UART_PORT_LOG,                 //LOG功能，不具备唤醒功能，156M时钟, 共享UART，谁先醒谁恢复
  } UART_PORT_FUNCTION;

  /**
  \brief Access structure of UART_Handle.
  */
  typedef struct
  {
    osList_t node;
    osList_t funcNode;
    UART_PORT_FUNCTION func;
    uint8_t adaptChClock;
    const struct UART_Res  *pRes;
    UART_CAPABILITIES   capabilities;
    UART_INFO          info;
    UART_TRANSFER_INFO xfer;
    /* 高层使用定义 */
    void *userData;
    UART_ReAdaptBaud re_adaptbaud; // Adaptbaud reset
    uint8_t *uart_ringbuf;
    #if defined(_CPU_AP)
      os_ringbuffer_t ringbuffer;
    #endif
    UART_WFIInfo  *wfiInfo;
    void *userbuf;
    uint32_t  userbuflen;
  } UART_Handle;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief           初始化UART控制器
 *
 * @param[in]       UART            UART控制器句柄
 *
 * @return          int32_t
 * @retval          DRV_OK          成功
 ************************************************************************************
*/
  extern int32_t UART_Initialize(UART_Handle *UART, UART_Callback cb_event);


/**
 ************************************************************************************
 * @brief           去初始化UART控制器
 *
 * @param[in]       UART            UART控制器句柄
 *
 * @return          int32_t
 * @retval          DRV_OK          成功
 ************************************************************************************
*/
  extern int32_t UART_Uninitialize(UART_Handle *UART);

/**
 ************************************************************************************
 * @brief           设置UART控制器的供电
 *
 * @param[in]       UART            UART控制器句柄
 * @param[in]       state           设置控制的状态
 *
 * @return          int32_t
 * @retval          DRV_ERR_PARAMETER             错误的参数
 *                  DRV_ERR_UNSUPPORTED           不支持
 *                  DRV_ERR                       错误
 *                  DRV_OK                        成功
 ************************************************************************************
*/
  extern int32_t UART_PowerControl(UART_Handle *UART, DRV_POWER_STATE state);

/**
 ************************************************************************************
 * @brief           启动UART控制器发送数据，启动立即返回
 *
 * @param[in]       UART            UART控制器句柄
 * @param[in]       data            发送数据缓存的buffer
 * @param[in]       num             发送数据的个数
 *
 * @return          int32_t
 * @retval          DRV_ERR_PARAMETER             错误的参数
 *                  DRV_ERR                       错误
 *                  DRV_ERR_BUSY                  设备忙
 *                  DRV_OK                        成功
 ************************************************************************************
*/
  extern int32_t UART_Send(UART_Handle *UART, const void *data, uint32_t num);

/**
 ************************************************************************************
 * @brief           设置UART控制器轮询发送数据
 *
 * @param[in]       UART            UART控制器句柄
 * @param[in]       data            发送数据缓存的buffer
 * @param[in]       num             发送数据的个数
 *
 * @return          int32_t
 * @retval          DRV_ERR_PARAMETER             错误的参数
 *                  DRV_ERR                       错误
 *                  DRV_ERR_BUSY                  设备忙
 *                  DRV_OK                        成功
 ************************************************************************************
*/
  extern int32_t UART_PollSend(UART_Handle *UART, const void *data, uint32_t num);

/**
 ************************************************************************************
 * @brief           启动UART控制器接收数据，启动立即返回
 *                  注意：UART控制器工作在DMA方式，
 *                  函数调用者传入的参数data是cacheline(32)对齐的，num是cacheline(32)的整数倍
 * @param[in]       UART            UART控制器句柄
 * @param[in]       data            接收数据缓存的buffer
 * @param[in]       num             接收数据的个数
 *
 * @return          int32_t
 * @retval          DRV_ERR_PARAMETER             错误的参数
 *                  DRV_ERR                       错误
 *                  DRV_ERR_BUSY                  设备忙
 *                  DRV_OK                        成功
 ************************************************************************************
 */
  extern int32_t UART_Receive(UART_Handle *UART, void *data, uint32_t num);

/**
 ************************************************************************************
 * @brief           设置UART控制器接收数据
 *                  此接口接收数据使用的是RingBuffer方案
 *                  UART控制器初始化的时候会将RX中断打开，接收到数据之后存储到RingBuffer中，读取数据从RingBuffer中读取
 * @param[in]       UART            UART控制器句柄
 * @param[in]       data            接收数据缓存的buffer
 * @param[in]       num             接收数据的个数
 *
 * @return          int32_t
 * @retval          DRV_ERR_PARAMETER             错误的参数
 *                  DRV_ERR                       错误
 *                  DRV_ERR_BUSY                  设备忙
 *                  DRV_OK                        成功
 *                  read_count                    返回接收到数据的个数
 ************************************************************************************
 */
  extern int32_t UART_Receive_Copy(UART_Handle *uart, void *data, uint32_t num);

/**
 ************************************************************************************
 * @brief           获取UART控制器发送的字节的个数.
 *
 * @param[in]       UART            UART控制器句柄
 *
 * @return          uint32_t
 * @retval          >=0             发送的字节的个数
 *
 ************************************************************************************
*/
  extern uint32_t UART_GetTxCount(UART_Handle *UART);

/**
 ************************************************************************************
 * @brief           获取UART控制器收到的字节的个数.
 *
 * @param[in]       UART            UART控制器句柄
 *
 * @return          uint32_t
 * @retval          >=0             收到的字节的个数
 *
 ************************************************************************************
 */
  extern uint32_t UART_GetRxCount(UART_Handle *UART);

/**
 ************************************************************************************
 * @brief           控制UART控制器.
 *
 * @param[in]       UART            UART控制器句柄
 * @param[in]       control         控制命令
 * @param[in]       control         控制命令参数
 *
 * @return          初始化返回值.
 * @retval          ==DRV_OK        执行成功
 *                  ==DRV_ERR       执行错误
 *                  ==DRV_ERR_BUSY  设备忙
 *                  == UART_ERROR_DATA_BITS 错误的数据位
 *                  ==UART_ERROR_PARITY     错误的校验位
 *                  ==UART_ERROR_STOP_BITS  错误的停止位
 *                  ==UART_ERROR_FLOW_CONTROL 错误的流控
 *                  ==UART_ERROR_BAUDRATE     错误的波特率
 ************************************************************************************
 */
  extern int32_t UART_Control(UART_Handle *UART, uint32_t control, uint32_t arg);

 /**
 ************************************************************************************
 * @brief           获取UART控制器状态
 *
 * @param[in]       UART            UART控制器句柄
 *
 * @return          UART控制器状态.
 * @retval          UART_STATUS      UART状态枚举值
 ************************************************************************************
 */
  extern int32_t UART_GetStatus(UART_Handle *uart, UART_STATUS *status);

/**
 ************************************************************************************
 * @brief           获取LPUART的发送状态
 *
 * @param[in]       bool_t
 *
 * @return          bool_t
 * @retval          true            空闲状态
                    false           发送忙
 ************************************************************************************
 */
extern bool_t UART_GetLpuartIdleState(void);

/**
 ************************************************************************************
 * @brief           轮询读取FIFO数据,供PSM读取
 *
 * @param[in]       void            UART控制器寄存器地址
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void UART_PSMRxPoll(void);

/**
 ************************************************************************************
 * @brief           IDLE 预读取
 *
 * @param[in]       void            UART控制器寄存器地址
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void UART_WFIRestore(void);

/**
 ************************************************************************************
 * @brief           添加唤醒UART,只有UART_PORT_AT才具有唤醒功能，具有唤醒功能必须波特率小于等于WAKEUP_MAX_BAUDERATE
 *
 * @param[in]       uart         UART控制器句柄
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void UART_updateUartWakeUp(UART_Handle *uart);

/**
 ************************************************************************************
 * @brief           查找链表,返回查找到的第一个
 *
 * @param[in]       func            功能
 *
 * @return          UART_Handle *
 * @retval
 ************************************************************************************
*/
UART_Handle *UART_findUart(UART_PORT_FUNCTION func);

/**
 ************************************************************************************
 * @brief           获取当前UART波特率
 *
 * @param[in]       UART            UART控制器句柄
 *
 * @return          UART波特率.
 ************************************************************************************
 */
int32_t UART_GetCurrentBaudRate(UART_Handle *uart);

#if defined(_CPU_AP)
bool_t UART_CheckUartIdleSupport(void);
#endif

#endif // DRIVER_UART_H_
/** @} */