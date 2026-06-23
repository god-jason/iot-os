/**
 * @file iotos.h
 * @brief IoT核心调度头文件
 *
 * 本模块提供IoT核心调度接口，包括Lua任务启动、消息队列初始化等。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOTOS_HEADER
#define IOTOS_HEADER

#include <stdbool.h>
#include <stdint.h>
#include "lua.h"
#include "iot_rtos.h"

/**
 * @brief 启动IoT Lua应用
 */
bool iot_start(void);

#endif /* IOTOS_HEADER */
