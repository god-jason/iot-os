/**  @file
  ycom_uart.h

  @brief
  TODO

*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/

#ifndef _YOPEN_UART_H_
#define _YOPEN_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "yopen_api_common.h"
#include "yopen_type.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define YOPEN_UART_ERRCODE_BASE    (YOPEN_COMPONENT_BSP_UART<<16)
#define YOPEN_UART_PIN_NONE        (0)

/*===========================================================================
 * Enum
 ===========================================================================*/

typedef enum
{
    YOPEN_UART_SUCCESS             = 0,
    YOPEN_UART_EXECUTE_ERR         = 1|YOPEN_UART_ERRCODE_BASE,
    YOPEN_UART_OPEN_REPEAT_ERR,
    YOPEN_UART_INVALID_PARAM_ERR,
    YOPEN_UART_BUSY,
    YOPEN_UART_NOT_OPEN_ERR        = 5|YOPEN_UART_ERRCODE_BASE,
} yopen_uart_errcode_e;

typedef enum 
{
    YOPEN_PORT_NONE = -1,
	YOPEN_UART_PORT_0,
	YOPEN_UART_PORT_1,
	YOPEN_UART_PORT_2,
    YOPEN_UART_PORT_3,
	YOPEN_USB_PORT_COM,
	YOPEN_PORT_MAX,
}yopen_uart_port_number_e;

typedef enum
{
	YOPEN_FC_NONE = 0,
	YOPEN_FC_HW,
}yopen_uart_flowctrl_e;

typedef enum
{
    YOPEN_UART_BAUD_AUTO       = 0,
	YOPEN_UART_BAUD_600       	= 600,
	YOPEN_UART_BAUD_1200       = 1200,
    YOPEN_UART_BAUD_2400       = 2400,
    YOPEN_UART_BAUD_4800       = 4800,
    YOPEN_UART_BAUD_9600       = 9600,
    YOPEN_UART_BAUD_14400      = 14400,
    YOPEN_UART_BAUD_19200      = 19200,
    YOPEN_UART_BAUD_28800      = 28800,
    YOPEN_UART_BAUD_38400      = 38400,
    YOPEN_UART_BAUD_57600      = 57600,
    YOPEN_UART_BAUD_115200     = 115200,
    YOPEN_UART_BAUD_230400     = 230400,
    YOPEN_UART_BAUD_460800     = 460800,
    YOPEN_UART_BAUD_921600     = 921600,
}yopen_uart_baud_e;

typedef enum
{
	YOPEN_UART_DATABIT_7 = 7,
	YOPEN_UART_DATABIT_8 = 8,      //8910 ARM UART hardware only support 8bit Data
}yopen_uart_databit_e;

typedef enum
{
	YOPEN_UART_STOP_1  =1,         
	YOPEN_UART_STOP_2  =2,
}yopen_uart_stopbit_e;

typedef enum
{
	YOPEN_UART_PARITY_NONE,
    YOPEN_UART_PARITY_ODD,
	YOPEN_UART_PARITY_EVEN,
}yopen_uart_paritybit_e;

typedef enum
{
    YOPEN_UART_TX_SENDING,
	YOPEN_UART_TX_COMPLETE,
}yopen_uart_tx_status_e;

typedef enum
{
    YOPEN_UART_EVENT_RX_ARRIVED = (1 << 0),  ///< Received new data
    YOPEN_UART_EVENT_RX_OVERFLOW = (1 << 1), ///< Rx fifo overflowed
    YOPEN_UART_EVENT_TX_COMPLETE = (1 << 2)  ///< All data had been sent
}yopen_uart_event_e;

typedef enum
{
    YOPEN_UART_TX_DRIVER = 1,
	YOPEN_UART_TX_OPAQ = 2,
	YOPEN_UART_TX_DRIVER_DMA = 3,
}yopen_uart_tx_way_e;

typedef enum
{
    YOPEN_UART_MODE_NORMAL = 1, /*!< Uart mode, used for user to transmit custom data */
    YOPEN_UART_MODE_AT = 2,     /*!< AT mode*/
} yopen_uart_mode_e;

typedef enum
{
    YOPEN_UART_IOCTL_SET_CCIO_MODE, //yopen_uart_mode_e  
} yopen_uart_ioctl_cmd_e;
/*===========================================================================
 * Struct
 ===========================================================================*/
typedef struct
{
    yopen_uart_port_number_e port;
    uint8_t tx_pin;
    uint8_t tx_func;
    uint8_t rx_pin;
    uint8_t rx_func;
	uint8_t cts_pin;
    uint8_t cts_func;
    uint8_t rts_pin;
    uint8_t rts_func;
} yopen_uart_func_s;

typedef struct
{
	yopen_uart_baud_e baudrate;
	yopen_uart_databit_e data_bit;
	yopen_uart_stopbit_e stop_bit;
	yopen_uart_paritybit_e parity_bit;
	yopen_uart_flowctrl_e flow_ctrl;
	bool isPrintfPort;
}yopen_uart_config_s;

/*****************************************************************
* Description: UART callback
* 
* Parameters:
*   ind_type        [in]    事件类型 yopen_api_event_id_e,
*                                          UART RX接收数据 YOPEN_UART_RX_RECV_DATA_IND 、
*                                          RX buffer溢出 YOPEN_UART_RX_OVERFLOW_IND、
*                                          TX fifo发送完成 YOPEN_UART_TX_COMPLETE_APP_IND
* 	port	  		[in] 	串口
* 	size	  		[in] 	数据大小
*
*****************************************************************/
typedef void (*yopen_uart_callback)(uint32 ind_type, yopen_uart_port_number_e port, uint32 size);

/**
 * @brief 配置串口信息
 * 
 * @param port 串口通道
 * @param dcb 串口配置结构体
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_uart_set_dcbconfig(yopen_uart_port_number_e port, yopen_uart_config_s *dcb);


/**
 * @brief 获取串口配置
 * 
 * @param port 串口通道
 * @param dcb 串口配置结构体
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_uart_get_dcbconfig(yopen_uart_port_number_e port, yopen_uart_config_s *dcb);


/**
 * @brief 打开串口
 * 
 * @param port 串口通道
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_uart_open(yopen_uart_port_number_e port);


/**
 * @brief 关闭串口
 * 
 * @param port 串口通道
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_uart_close(yopen_uart_port_number_e port);


/**
 * @brief 串口发送（非阻塞方式发送，将数据放到TX buf[共4096字节]。若上次发送未完成，将返回YOPEN_UART_BUSY错误）
 * 
 * @param port 串口通道
 * @param data 要发送的数据 
 * @param data_len 要发送的数据长度，支持最大4096字节
 * @return yopen_uart_errcode_e
 */
extern yopen_uart_errcode_e yopen_uart_write(yopen_uart_port_number_e port, unsigned char *data, unsigned int data_len);


/**
 * @brief 读取串口数据（非阻塞方式读取，实际读取RX buf[共512字节]中的数据，若无数据，则返回0）
 * 
 * @param port 串口通道
 * @param data 读取到的数据
 * @param data_len 要读取的数据长度
 * @return int 实际读取的数据长度
 */
extern int yopen_uart_read(yopen_uart_port_number_e port, unsigned char *data, unsigned int data_len);


/**
 * @brief 注册UART 事件回调函数
 * 
 * @param port 串口通道
 * @param uart_cb 需要注册的回调函数，若为NULL,则取消注册callback，可使用轮询读取数据
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_uart_register_cb(yopen_uart_port_number_e port, yopen_uart_callback uart_cb);


/**
 * @brief 配置UART发送数据的方式
 * 
 * @param way 选择用opaq方式发送数据或直接调用底层驱动发数据，默认为opaq
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_uart_tx_way_config(yopen_uart_tx_way_e way);


/**
 * @brief 设置log输出的端口
 * 
 * @param value 表示log输出的方式，设置0时，仅从USB输出log；设置1，则仅从debug uart口输出log；设置2，则开机先从debug uart口输出log，待USB初始化后自动转到USB输出log
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_log_port_set(uint32_t value);


/**
 * @brief 获取log输出的端口
 * 
 * @param value 表示获取log输出的方式，设置0时，仅从USB输出log；设置1，则仅从debug uart口输出log；设置2，则开机先从debug uart口输出log，待USB初始化后自动转到USB输出log
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_log_port_get(uint32_t *value);


/**
 * @brief 设置debug uart口的波特率（仅在debug uart口作为log输出口时有效）
 * 
 * @param baud debug uart口的波特率，目前最大支持3M
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_debugport_logbuad_set(uint32_t baud);

/**
 * @brief 控制串口
 * 
 * @param port 串口通道
 * @param cmd 控制命令
 * @param arg 控制参数
 * @return yopen_uart_errcode_e 
 */
extern yopen_uart_errcode_e yopen_uart_ioctl(yopen_uart_port_number_e port,yopen_uart_ioctl_cmd_e cmd,void * arg );
#ifdef __cplusplus
}
#endif
#endif

