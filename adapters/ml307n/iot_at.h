/**
 * @file iot_at.h
 * @brief ML307N 平台虚拟AT通道适配器头文件
 * @details 将 ML307N 平台 cm_virt_at 接口封装为统一命名规范的宏定义，
 *          编译时直接展开到底层函数调用，零运行时性能损失。
 */

#ifndef IOT_AT_ML307N_H
#define IOT_AT_ML307N_H

#include "cm_virt_at.h"
#include "../../iot_types.h"

/* ============================================================
 * 统一AT类型定义（跨平台兼容）
 * ============================================================ */

/** @brief AT事件类型值定义 */
#define IOT_AT_EV_SUCCESS_VAL  0   /**< AT命令成功 */
#define IOT_AT_EV_ERROR_VAL    1   /**< AT命令错误 */
#define IOT_AT_EV_TIMEOUT_VAL  2   /**< AT命令超时 */
#define IOT_AT_EV_URC_VAL      3   /**< URC主动上报 */

/** @brief AT回调函数类型 */
typedef void (*iot_at_callback_t)(uint8_t event, void *user_param);

/** @brief URC回调函数类型 */
typedef void (*iot_at_urc_callback_t)(const char *urc_line, void *user_data);

/** @brief AT URC结构体 */
typedef struct {
    char *prefix;                   /**< URC前缀，如"+CSQ:" */
    iot_at_urc_callback_t callback;  /**< URC回调函数 */
    void *user_data;                /**< 用户数据 */
} iot_at_urc_table_t;

/* ============================================================
 * AT 操作宏定义
 * ============================================================ */

/**
 * @brief 注册URC上报回调函数
 * @param[in] prefix URC消息前缀
 * @param[in] cb     URC回调函数
 * @return 0 成功，<0 失败
 */
#define iot_at_register_urc(prefix, cb) \
    cm_virt_at_urc_cb_reg((const uint8_t *)(prefix), (cm_virt_at_urc_cb)(cb))

/**
 * @brief 取消注册URC上报回调函数
 * @param[in] prefix URC消息前缀
 * @param[in] cb     URC回调函数
 * @return 0 成功，<0 失败
 */
#define iot_at_unregister_urc(prefix, cb) \
    cm_virt_at_urc_cb_dereg((const uint8_t *)(prefix), (cm_virt_at_urc_cb)(cb))

/**
 * @brief 通过虚拟AT通道异步发送AT指令
 * @param[in] at_cmd   发送的AT指令字符串
 * @param[in] cb       消息回调函数
 * @param[in] user_param 用户自定义参数
 * @return 0 成功，<0 失败
 */
#define iot_at_send_async(at_cmd, cb, user_param) \
    cm_virt_at_send_async((const uint8_t *)(at_cmd), (cm_virt_at_cb)(cb), (void *)(user_param))

/**
 * @brief 通过虚拟AT通道同步发送AT指令
 * @param[in]  at_cmd   发送的AT指令字符串
 * @param[out] rsp      接收响应数据的缓冲区
 * @param[out] rsp_len  输入为缓冲区大小，输出为实际响应长度
 * @param[in]  timeout  超时时间（毫秒）
 * @return 0 成功，<0 失败
 * @warning 不建议使用等待时间超过5秒的指令，确保rsp空间足够防止踩内存
 */
#define iot_at_send_sync(at_cmd, rsp, rsp_len, timeout) \
    cm_virt_at_send_sync((const uint8_t *)(at_cmd), (uint8_t *)(rsp), (int32_t *)(rsp_len), (uint32_t)(timeout))

#endif /* IOT_AT_ML307N_H */