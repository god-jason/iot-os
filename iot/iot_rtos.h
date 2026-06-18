/**
 * @file iot_rtos.h
 * @brief RTOS消息队列
 *
 * 本模块提供RTOS消息队列的创建、发送、接收等功能。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_RTOS_HEADER
#define IOT_RTOS_HEADER

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "iot_params.h"
#include "lua.h"

/**
 * @brief 消息类型枚举
 */
typedef enum {
    MSG_TYPE_TIMEOUT = 1,
    MSG_TYPE_PUBLISH = 2,
    MSG_TYPE_CALL = 3
} msg_type_t;

/* RTOS消息结构（包含动态参数） */
typedef struct {
    msg_type_t type;
    uint32_t id;
    void* data;           /* PUBLISH时为const char*，CALL时为userdata，仅支持常量字符串 */
    params_t* params;     /* 仅PUBLISH和CALL有效 */
} rtos_msg_t;

/**
 * @brief 设置全局 Lua 状态机
 */
void iot_set_lua_state(lua_State* L);

/**
 * @brief 获取全局 Lua 状态机
 */
lua_State* iot_get_lua_state(void);

/**
 * @brief 初始化RTOS消息队列
 */
bool iot_rtos_msg_init(void);

/**
 * @brief 发送PUBLISH消息
 */
void iot_rtos_publish(const char* str, params_t* params);

/**
 * @brief 发送CALL消息
 */
void iot_rtos_call(void* userdata, params_t* params);

/**
 * @brief 发送定时器超时消息
 * @param timer_id 定时器ID
 */
void iot_rtos_timeout(uint32_t timer_id);

/**
 * @brief 销毁消息
 */
void iot_rtos_msg_destroy(rtos_msg_t* msg);

/* Lua模块入口 */
LUAMOD_API int luaopen_rtos(lua_State* L);

#endif /* IOT_RTOS_HEADER */
