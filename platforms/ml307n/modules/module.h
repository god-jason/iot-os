/**
 * @file module.h
 * @brief IoT基础定义
 *
 * 本模块提供IoT基础定义，包括内存分配函数、日志宏定义等。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_MODULE_H
#define IOT_MODULE_H


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

#include "iot_log.h"
#include "iot_os.h"
#include "iot_mem.h"

#endif /* IOT_MODULE_H */
