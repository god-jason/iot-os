/**
 * @file iot_uart.h
 * @brief ML307N 平台 UART 适配器头文件
 * @details 基于 cm_uart 接口实现 UART 功能封装，
 *          定义统一的 UART 类型，支持跨平台编译。
 */

#ifndef IOT_UART_ML307N_H
#define IOT_UART_ML307N_H

#include "cm_uart.h"

/* ============================================================
 * 统一 UART 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief UART校验位枚举 */
typedef enum {
    IOT_UART_PARITY_NONE = 0,        /**< 无校验 */
    IOT_UART_PARITY_ODD  = 1,        /**< 奇校验 */
    IOT_UART_PARITY_EVEN = 2,        /**< 偶校验 */
} iot_uart_parity_t;

/** @brief UART停止位枚举（只有两个值，保持原样） */
typedef enum {
    IOT_UART_STOP_BITS_1 = 1,        /**< 1位停止位 */
    IOT_UART_STOP_BITS_2 = 2,        /**< 2位停止位 */
} iot_uart_stop_bits_t;

/** @brief UART数据位枚举（只有两个值，保持原样） */
typedef enum {
    IOT_UART_DATA_BITS_7 = 7,        /**< 7位数据位 */
    IOT_UART_DATA_BITS_8 = 8,        /**< 8位数据位 */
} iot_uart_data_bits_t;

/** @brief UART流控模式枚举 */
typedef enum {
    IOT_UART_FLOW_CTRL_NONE     = 0,    /**< 无流控 */
    IOT_UART_FLOW_CTRL_RTS       = 1,    /**< RTS流控 */
    IOT_UART_FLOW_CTRL_CTS       = 2,    /**< CTS流控 */
    IOT_UART_FLOW_CTRL_RTS_CTS   = 3,    /**< RTS+CTS流控 */
} iot_uart_flow_ctrl_t;

/** @brief UART配置结构体 */
typedef struct {
    uint32_t baud_rate;               /**< 波特率 */
    iot_uart_data_bits_t data_bits;   /**< 数据位 */
    iot_uart_stop_bits_t stop_bits;   /**< 停止位 */
    iot_uart_parity_t parity;         /**< 校验位 */
    iot_uart_flow_ctrl_t flow_ctrl;   /**< 流控 */
} iot_uart_config_t;

/** @brief UART事件类型枚举 */
typedef enum {
    IOT_UART_EVENT_RX_ARRIVED   = 0,   /**< 接收数据到达 */
    IOT_UART_EVENT_RX_OVERFLOW  = 1,   /**< 接收溢出 */
    IOT_UART_EVENT_TX_COMPLETE  = 2,   /**< 发送完成 */
} iot_uart_event_t;

/** @brief UART回调函数类型 */
typedef void (*iot_uart_callback_t)(iot_uart_event_t event, uint32_t size, void *user_data);

/* ============================================================
 * UART 句柄类型
 * ============================================================ */

/** @brief UART句柄类型 */
typedef int iot_uart_t;

/* ============================================================
 * UART 操作函数声明
 * ============================================================ */

/**
 * @brief 初始化UART
 * @param[in] uart UART端口
 * @param[in] config UART配置参数
 * @return 0 成功
 */
int iot_uart_init(iot_uart_t uart, const iot_uart_config_t *config);

/**
 * @brief 去初始化UART
 * @param[in] uart UART端口
 * @return 0 成功
 */
int iot_uart_deinit(iot_uart_t uart);

/**
 * @brief 发送数据（同步）
 * @param[in] uart UART端口
 * @param[in] data 数据缓冲区
 * @param[in] len 数据长度
 * @return 实际发送长度
 */
int iot_uart_send(iot_uart_t uart, const void *data, uint32_t len);

/**
 * @brief 接收数据（同步）
 * @param[in] uart UART端口
 * @param[out] data 数据缓冲区
 * @param[in] len 缓冲区大小
 * @return 实际接收长度
 */
int iot_uart_recv(iot_uart_t uart, void *data, uint32_t len);

/**
 * @brief 注册UART事件回调
 * @param[in] uart UART端口
 * @param[in] cb 回调函数
 * @param[in] user_data 用户数据
 * @return 0 成功
 */
int iot_uart_register_callback(iot_uart_t uart, iot_uart_callback_t cb, void *user_data);

#endif /* IOT_UART_ML307N_H */