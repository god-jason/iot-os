/**
 * @file        cm_uart.h
 * @brief       UART接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By SMR
 * @date        2021/03/09
 *
 * @defgroup uart uart
 * @ingroup PI
 * @{
 */

#ifndef __CM_UART_H__
#define __CM_UART_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/** 标准波特率 */
/**
 * lpuart具有唤醒功能，根据不同波特率设置不同的省电模式
 * 具体配置如下：
 * 1.LPUART 9600      32K   不打开DEEPSLEEP 26M
 * 2.LPUART  <=115200 26M   打开DEEPSLEEP 26M
 * 3.LPUART  其他波特率不允许睡眠
 *
 * 不满足上述条件，则进行如下条件判断：
 * 任意UART作为UART_PORT_CONSOLE，必须配置为26M（省电要求）
 * 任意UART作为UART_PORT_AT，波特率小于等于WAKEUP_MAX_BAUDERATE，必须配置为26M
 * 任意UART除了UART_PORT_CONSOLE && UART_PORT_AT以外使用默认的时钟配置
 */
#define    CM_UART_BAUDRATE_2400        (2400U)
#define    CM_UART_BAUDRATE_4800        (4800U)
#define    CM_UART_BAUDRATE_9600        (9600U)
#define    CM_UART_BAUDRATE_14400       (14400U)
#define    CM_UART_BAUDRATE_19200       (19200U)
#define    CM_UART_BAUDRATE_38400       (38400U)
#define    CM_UART_BAUDRATE_57600       (57600U)
#define    CM_UART_BAUDRATE_115200      (115200U)
#define    CM_UART_BAUDRATE_230400      (230400U)
#define    CM_UART_BAUDRATE_460800      (460800U)
#define    CM_UART_BAUDRATE_921600      (921600U)
#define    CM_UART_BAUDRATE_3000000     (3000000U)
#define    CM_UART_BAUDRATE_6000000     (6000000U)
#define    CM_UART_BAUDRATE_12000000    (12000000U)

/****************************************************************************
 * Public Types
 ****************************************************************************/

 /** 设备ID ，详情参照资源综述*/
typedef enum{
    CM_UART_DEV_0,              /*!< 设备0*/
    CM_UART_DEV_1,              /*!< 设备1*/
    CM_UART_DEV_2,              /*!< 设备2*/
    CM_UART_DEV_3,
    CM_UART_DEV_NUM
} cm_uart_dev_e;

/** 数据位 */
typedef enum{
    CM_UART_BYTE_SIZE_8 = 8,
    CM_UART_BYTE_SIZE_7 = 7,
    CM_UART_BYTE_SIZE_6 = 6,
    CM_UART_BYTE_SIZE_5 = 5,
} cm_uart_byte_size_e;

/** 奇偶校验 */
typedef enum{
    CM_UART_PARITY_NONE,
    CM_UART_PARITY_ODD,
    CM_UART_PARITY_EVEN,
    CM_UART_PARITY_MARK,    /*!< 不支持*/
    CM_UART_PARITY_SPACE    /*!< 不支持*/
} cm_uart_parity_e;

/** 停止位 */
typedef enum{
    CM_UART_STOP_BIT_ONE,
    CM_UART_STOP_BIT_ONE_HALF,    /*!< 不支持*/
    CM_UART_STOP_BIT_TWO
} cm_uart_stop_bit_e;

/** 流控制 */
/*
 * 开启流控后，当缓存区内数据超过流控上限阈值将拉高RTS(PIN23)，此时若注册了流控事件将上报流控事件
 * 通过读取缓存区数据直到低于流控下限阈值才会重新拉低RTS(PIN23)
 */
typedef enum{
    CM_UART_FLOW_CTRL_NONE,
    CM_UART_FLOW_CTRL_HW,   /*!< 使能流控*/
} cm_uart_flow_ctrl_e;

/** 结果码 */
typedef enum{
    CM_UART_RET_OK = 0,
    CM_UART_RET_INVALID_PARAM = -1,
    CM_UART_RET_CTS_HIGH = -2,
} cm_uart_ret_e;

/** 配置 */
typedef struct{
    cm_uart_byte_size_e byte_size;      /*!< 数据位，枚举*/
    cm_uart_parity_e parity;            /*!< 校验位，枚举*/
    cm_uart_stop_bit_e stop_bit;        /*!< 停止位，枚举*/
    cm_uart_flow_ctrl_e flow_ctrl;      /*!< 流控制，枚举*/
    uint32_t baudrate;                  /*!< 波特率，支持的波特率见本文件中标准波特率宏定义和特殊波特率宏定义*/
    uint32_t is_lpuart;                 /*!< 1:低功耗串口(仅支持115200以内的标准波特率)；0:普通串口,只有LPUART0支持*/
    uint32_t rxrb_buf_size;             /*!< 环形缓存区大小，若为0则按默认配置8K；若为0且开了流控，则流控上下限阈值按默认配置6K和4K*/
    uint32_t fc_high_threshold;         /*!< 主串口流控上限阈值，如果flow_ctrl为1且rxrb_buf_size不为0则必须配置此参数，不能超过rxrb_buf_size减2k*/
    uint32_t fc_low_threshold;          /*!< 主串口流控下限阈值，如果flow_ctrl为1且rxrb_buf_size不为0则必须配置此参数，不能超过fc_high_threshold*/
} cm_uart_cfg_t;

/** 事件类型 */
typedef enum
{
    CM_UART_EVENT_TYPE_RX_ARRIVED  = (1 << 0),  /*!< 接收到新的数据*/
    CM_UART_EVENT_TYPE_RX_OVERFLOW = (1 << 1),  /*!< 接收FIFO缓存溢出*/
    CM_UART_EVENT_TYPE_TX_COMPLETE = (1 << 2),  /*!< 不支持该事件*/
    CM_UART_EVENT_TYPE_RX_FLOWCTRL = (1 << 3)   /*!< 流控事件*/
}cm_uart_event_type_e;

/** 事件 */
typedef struct{
    uint32_t event_type;    /*!< 要注册的事件，数据可读/溢出/流控等，注册后才会上报*/
    void *event_param;      /*!< 事件参数*/
    void *event_entry;      /*!< 事件执行入口*/
} cm_uart_event_t;

typedef void (*cm_uart_event_cb_t)(void *param, uint32_t evt);

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************/

/**
 *  @brief 打开串口
 *
 *  @param [in] dev 串口设备ID
 *  @param [in] cfg 串口配置
 *
 *  @return
 *    = 0  - 成功 \n
 *    < 0  - 失败, 返回值为错误码
 *
 *  @details open之前必须先对引脚复用功能进行设置。\n
 *           串口支持的波特率及模式见cm_uart_cfg_t结构体注释说明，请详细查看波特率宏定义处的注意事项
 */
int32_t cm_uart_open(cm_uart_dev_e dev, cm_uart_cfg_t *cfg);

/**
 *  @brief 注册串口事件
 *
 *  @param [in] dev 串口设备ID
 *  @param [in] event 串口事件
 *
 *  @return
 *    = 0  - 成功 \n
 *    < 0  - 失败, 返回值为错误码
 *
 *  @details 事件包括串口数据可读/溢出/流控等。需在open之前注册。\n
 *           内置默认8K缓存区(缓存区大小可配置)保存未读出的数据，若注册了溢出事件，缓存区满后将上报溢出，缓存区溢出状态下将丢弃新接收的数据。\n
 *           回调函数中不可输出LOG、串口打印、执行复杂任务或消耗过多资源。
 */
int32_t cm_uart_register_event(cm_uart_dev_e dev, void *event);

/**
 *  @brief 关闭串口
 *
 *  @param [in] dev 串口设备ID
 *
 *  @return
 *    = 0  - 成功 \n
 *    < 0  - 失败, 返回值为错误码
 *
 *  @details
 */
int32_t cm_uart_close(cm_uart_dev_e dev);

/**
 *  @brief 串口写数据
 *
 *  @param [in] dev 串口设备ID
 *  @param [in] data 待写入数据
 *  @param [in] len 长度
 *  @param [in] timeout 超时时间(ms)(无效参数)
 *
 *  @return
 *    = 实际写入长度 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details 当主串口开启流控，发送前若检测CTS(PIN23)为高则返回失败，若发送过程中检测CTS(PIN23)为高则最终返回实际发送的长度(包含已经发送的长度和缓存区中待发送的长度)
 */
int32_t cm_uart_write(cm_uart_dev_e dev, const void *data, int32_t len, int32_t timeout);

/**
 *  @brief 串口读数据
 *
 *  @param [in] dev 串口设备ID
 *  @param [out] data 待读数据
 *  @param [in] len 长度
 *  @param [in] timeout 超时时间(ms)(无效参数)
 *
 *  @return
 *    = 实际读出长度 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details
 */
int32_t cm_uart_read(cm_uart_dev_e dev, void* data, int32_t len, int32_t timeout);

/**
 *  @brief 获取串口配置
 *
 *  @param [in]  dev 串口设备ID
 *  @param [out] cfg 串口配置
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details 用户自行维护cfg内存空间
 */
int32_t cm_uart_get_cfg(cm_uart_dev_e dev, cm_uart_cfg_t *cfg);

/**
 *  @brief 获取串口缓存区待读数据长度
 *
 *  @param [in] dev 串口设备ID
 *
 *  @return
 *   >= 0 - 待读数据长度 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details
 */
int32_t cm_uart_get_rxrb_data_len(cm_uart_dev_e dev);

/**
 *  @brief 清空串口接收缓存区数据
 *
 *  @param [in] dev 串口设备ID
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details
 */
int32_t cm_uart_clean(cm_uart_dev_e dev);

/**
 *  @brief 查询串口发送状态
 *
 *  @param [in] dev 串口设备ID
 *
 *  @return
 *    = 0 - 未在发送 \n
 *    = 1 - 正在发送
 *
 *  @details 在cm_uart_write返回之后使用，实际为查询tx fifo状态，若为空则说明数据已从fifo全部取出，接口返回0 \n
 *           若用于开启流控的流控串口，cm_uart_write返回后此时fifo中的数据可能受流控控制而无法被取出发送，此时接口将返回1
 */
int32_t cm_uart_is_sending(cm_uart_dev_e dev);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_UART_H__ */

/** @}*/
