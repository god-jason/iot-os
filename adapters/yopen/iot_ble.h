/**
 * @file iot_ble.h
 * @brief YOPEN 平台 BLE 蓝牙适配器头文件
 * @details 提供 BLE 蓝牙功能封装。
 * @note 暂未支持，保留接口占位
 */

#ifndef IOT_BLE_YOPEN_H
#define IOT_BLE_YOPEN_H

#include "../../iot_types.h"

/* ============================================================
 * BLE 类型定义
 * ============================================================ */

/** @brief BLE连接状态 */
typedef enum {
    IOT_BLE_STATE_DISCONNECTED = 0,  /**< 未连接 */
    IOT_BLE_STATE_CONNECTED = 1,     /**< 已连接 */
    IOT_BLE_STATE_ADVERTISING = 2,   /**< 广播中 */
} iot_ble_state_t;

/* ============================================================
 * BLE 接口（暂未支持）
 * ============================================================ */

/**
 * @brief 初始化BLE模块
 * @return 0 成功，<0 失败
 */
int32_t iot_ble_init(void);

/**
 * @brief 去初始化BLE模块
 * @return 0 成功，<0 失败
 */
int32_t iot_ble_deinit(void);

/**
 * @brief 开始广播
 * @param[in] param 广播参数
 * @return 0 成功，<0 失败
 */
int32_t iot_ble_start_adv(const iot_ble_adv_param_t *param);

/**
 * @brief 停止广播
 * @return 0 成功，<0 失败
 */
int32_t iot_ble_stop_adv(void);

/**
 * @brief 连接远程设备
 * @param[in] addr 设备地址
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功，<0 失败
 */
int32_t iot_ble_connect(const char *addr, uint32_t timeout_ms);

/**
 * @brief 断开连接
 * @return 0 成功，<0 失败
 */
int32_t iot_ble_disconnect(void);

/**
 * @brief 发送数据
 * @param[in] data 数据指针
 * @param[in] len 数据长度
 * @return >=0 成功发送的字节数，<0 失败
 */
int32_t iot_ble_send(const uint8_t *data, uint32_t len);

/**
 * @brief 注册数据回调
 * @param[in] cb 数据回调函数
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int32_t iot_ble_register_data_cb(iot_ble_data_cb_t cb, void *user_data);

/**
 * @brief 注册状态回调
 * @param[in] cb 状态回调函数
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int32_t iot_ble_register_state_cb(iot_ble_state_cb_t cb, void *user_data);

#endif /* IOT_BLE_YOPEN_H */
