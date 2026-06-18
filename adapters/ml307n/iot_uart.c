/**
 * @file iot_uart.c
 * @brief ML307N 平台 UART 适配器实现
 * @details 基于 cm_uart 接口实现 UART 功能封装，
 *          支持枚举类型参数转换。
 */

#include "iot_uart.h"

/* ============================================================
 * UART 回调处理
 * ============================================================ */

/** @brief UART回调用户数据结构体 */
typedef struct {
    iot_uart_callback_t user_cb;
    void *user_data;
} iot_uart_cb_data_t;

/** @brief UART回调数据数组（每个端口一个） */
static iot_uart_cb_data_t g_uart_cb_data[CM_UART_MAX] = {0};

/**
 * @brief 平台UART回调函数
 * @param[in] uart UART端口
 * @param[in] event 平台事件
 * @param[in] size 数据大小
 */
static void iot_uart_platform_callback(cm_uart_port_e uart, cm_uart_event_e event, uint32_t size)
{
    iot_uart_event_t event_id = IOT_UART_EVENT_RX_ARRIVED;
    
    if (g_uart_cb_data[uart].user_cb == NULL) {
        return;
    }
    
    /* 平台事件转统一事件 */
    switch (event) {
        case CM_UART_EVENT_RX_ARRIVED:
            event_id = IOT_UART_EVENT_RX_ARRIVED;
            break;
        case CM_UART_EVENT_RX_OVERFLOW:
            event_id = IOT_UART_EVENT_RX_OVERFLOW;
            break;
        case CM_UART_EVENT_TX_COMPLETE:
            event_id = IOT_UART_EVENT_TX_COMPLETE;
            break;
        default:
            return;
    }
    
    /* 调用用户回调 */
    g_uart_cb_data[uart].user_cb(event_id, size, g_uart_cb_data[uart].user_data);
}

/* ============================================================
 * UART 操作函数实现
 * ============================================================ */

/**
 * @brief 初始化UART
 * @param[in] uart UART端口
 * @param[in] config UART配置参数
 * @return 0 成功
 */
int iot_uart_init(iot_uart_t uart, const iot_uart_config_t *config)
{
    cm_uart_config_t uart_config = {0};
    
    if (config == NULL) {
        return IOT_ERR_PARAM;
    }
    
    /* 配置参数转换 */
    uart_config.baud_rate = config->baud_rate;
    uart_config.data_bits = (cm_uart_data_bits_e)config->data_bits;
    uart_config.stop_bits = (cm_uart_stop_bits_e)config->stop_bits;
    uart_config.parity = (cm_uart_parity_e)config->parity;
    uart_config.flow_ctrl = (cm_uart_flow_ctrl_e)config->flow_ctrl;
    
    /* 调用平台初始化 */
    return cm_uart_init((cm_uart_port_e)uart, &uart_config);
}

/**
 * @brief 去初始化UART
 * @param[in] uart UART端口
 * @return 0 成功
 */
int iot_uart_deinit(iot_uart_t uart)
{
    g_uart_cb_data[uart].user_cb = NULL;
    g_uart_cb_data[uart].user_data = NULL;
    
    return cm_uart_deinit((cm_uart_port_e)uart);
}

/**
 * @brief 发送数据（同步）
 * @param[in] uart UART端口
 * @param[in] data 数据缓冲区
 * @param[in] len 数据长度
 * @return 实际发送长度
 */
int iot_uart_send(iot_uart_t uart, const void *data, uint32_t len)
{
    return cm_uart_send((cm_uart_port_e)uart, (const uint8_t *)data, len, IOT_WAIT_FOREVER);
}

/**
 * @brief 接收数据（同步）
 * @param[in] uart UART端口
 * @param[out] data 数据缓冲区
 * @param[in] len 缓冲区大小
 * @return 实际接收长度
 */
int iot_uart_recv(iot_uart_t uart, void *data, uint32_t len)
{
    return cm_uart_recv((cm_uart_port_e)uart, (uint8_t *)data, len, IOT_WAIT_FOREVER);
}

/**
 * @brief 注册UART事件回调
 * @param[in] uart UART端口
 * @param[in] cb 回调函数
 * @param[in] user_data 用户数据
 * @return 0 成功
 */
int iot_uart_register_callback(iot_uart_t uart, iot_uart_callback_t cb, void *user_data)
{
    g_uart_cb_data[uart].user_cb = cb;
    g_uart_cb_data[uart].user_data = user_data;
    
    /* 注册平台回调 */
    if (cb != NULL) {
        cm_uart_register_callback((cm_uart_port_e)uart, iot_uart_platform_callback);
    } else {
        cm_uart_register_callback((cm_uart_port_e)uart, NULL);
    }
    
    return 0;
}