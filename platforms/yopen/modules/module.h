/**
 * @file module.h
 * @brief YOPEN 平台 IoT 基础定义
 *
 * 本模块提供 YOPEN 平台基础定义，包括内存分配函数、日志宏定义等。
 *
 * @author  TRAE
 * @date    2026.06.19
 */

#ifndef YOPEN_MODULE_H
#define YOPEN_MODULE_H

// C 标准库
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// YOPEN 系统头文件
#include "yopen_type.h"
#include "yopen_os.h"
#include "yopen_error.h"
#include "yopen_api_common.h"

// Lua 头文件
#include "lua.h"
#include "lauxlib.h"

// 平台适配层
#include "iot_log.h"
#include "iot_os.h"
#include "iot_mem.h"
#include "iot_fs.h"

#endif /* YOPEN_MODULE_H */