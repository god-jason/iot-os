/**
 * @file iot_at.h
 * @brief YOPEN 平台 AT指令适配器头文件
 * @details 基于 yopen_ril 接口实现统一的 AT 指令操作，
 *          支持同步/异步发送、URC 注册等功能。
 */

#ifndef IOT_AT_YOPEN_H
#define IOT_AT_YOPEN_H

#include "../iot_common.h"
#include "yopen_ril.h"

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
 * AT 指令宏定义
 * ============================================================ */

/**
 * @brief AT初始化
 * @param[in] cb URC主动上报回调函数
 * @return 0 成功，<0 失败
 */
#define iot_at_init(cb) \
    yopen_ril_init((yopen_ril_urc_callback)(cb))

/**
 * @brief AT去初始化
 * @return 0 成功，<0 失败
 */
#define iot_at_deinit() \
    yopen_ril_deinit()

/**
 * @brief 发送AT指令（异步方式）
 * @param[in] cmd     AT指令字符串（需包含\r\n结尾）
 * @param[in] cmd_len 指令长度
 * @param[in] cb      指令返回回调函数
 * @param[in] param   回调函数用户参数
 * @return 0 成功，<0 失败
 */
#define iot_at_send_async(cmd, cmd_len, cb, param) \
    yopen_ril_send_atcmd((char *)(cmd), (uint32_t)(cmd_len), (yopen_ril_callback)(cb), (void *)(param))

/* ============================================================
 * AT 扩展接口
 * ============================================================ */

/**
 * @brief AT模块初始化
 * @param[in] urc_table URC注册表
 * @param[in] table_size URC表大小
 * @return 0 成功，<0 失败
 */
int32_t iot_at_init_ex(const iot_at_urc_table_t *urc_table, uint32_t table_size);

/**
 * @brief AT发送同步命令并等待响应
 * @param[in] cmd     AT指令字符串
 * @param[in] cmd_len 指令长度
 * @param[out] rsp     响应缓冲区
 * @param[in] rsp_len 响应缓冲区长度
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return >=0 响应长度，<0 失败
 */
int32_t iot_at_send_sync(const char *cmd, uint32_t cmd_len, char *rsp, uint32_t rsp_len, uint32_t timeout_ms);

/**
 * @brief 注册URC主动上报回调
 * @param[in] prefix URC前缀，如"+CSQ:"
 * @param[in] cb     回调函数
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int32_t iot_at_register_urc(const char *prefix, iot_at_urc_callback_t cb, void *user_data);

/**
 * @brief 注销URC主动上报回调
 * @param[in] prefix URC前缀
 * @return 0 成功，<0 失败
 */
int32_t iot_at_unregister_urc(const char *prefix);

#endif /* IOT_AT_YOPEN_H */