/**
 * @file iot_log.h
 * @brief IoT日志模块
 *
 * 本模块提供日志功能，支持多参数打印和日志级别控制。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.20
 */

#ifndef IOT_LOG_H
#define IOT_LOG_H

#include "lua.h"
#include "lauxlib.h"
#include <stdarg.h>
#include <stdio.h>

/* 日志级别定义 */
typedef enum {
    LOG_LEVEL_NONE = 0,    /* 不输出任何日志 */
    LOG_LEVEL_ERROR = 1,   /* 错误级别 */
    LOG_LEVEL_WARN = 2,    /* 警告级别 */
    LOG_LEVEL_INFO = 3,    /* 信息级别 */
    LOG_LEVEL_DEBUG = 4,   /* 调试级别 */
    LOG_LEVEL_TRACE = 5,   /* 跟踪级别 */
} iot_log_level_t;

/**
 * @brief 设置日志级别
 * @param level 日志级别
 */
void iot_log_set_level(iot_log_level_t level);

/**
 * @brief 获取当前日志级别
 * @return 当前日志级别
 */
iot_log_level_t iot_log_get_level(void);

/**
 * @brief 日志输出函数（内部使用）
 * @param level 日志级别
 * @param tag 模块标签
 * @param fmt 格式字符串
 * @param ... 可变参数
 */
void iot_log_printf(iot_log_level_t level, const char* tag, const char* fmt, ...);

/**
 * @brief 日志宏定义 - 输出带函数名和行号的日志
 * @param fmt 格式字符串
 * @param ... 可变参数
 */
#define LOG(fmt, ...) iot_printf("[iot] %s():%d " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

/**
 * @brief 注册log模块到Lua
 * @param L Lua状态机
 * @return 1（模块表压栈）
 */
LUAMOD_API int luaopen_log(lua_State* L);

#endif /* IOT_LOG_H */
