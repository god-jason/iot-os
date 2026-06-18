/**
 * @file module.h
 * @brief IoT基础定义
 *
 * 本模块提供IoT基础定义，包括内存分配函数、日志宏定义等。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_COMMON_H
#define IOT_COMMON_H


// C 标准库
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// 系统头文件
#include "os.h"
#include "cm_os.h"
#include "cm_sys.h"
#include "cm_mem.h"
#include "lua.h"
#include "lauxlib.h"


// 定义内存分配函数，用于动态分配内存，避免直接使用malloc/free等系统函数
#define iot_malloc(size) cm_malloc(size)
#define iot_free(ptr) cm_free(ptr)
#define iot_realloc(ptr, size) cm_realloc(ptr, size)
#define iot_calloc(size, count) cm_calloc(size, count)


// 日志定义，打印函数史，行号，以及换行回车
#undef LOG
#define LOG(fmt, ...) cm_log_printf("[iot] %s():%d " fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)


#endif /* IOT_BASE_H */
