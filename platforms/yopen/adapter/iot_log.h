/**
 * @file iot_log.h
 * @brief YOPEN 平台日志适配器头文件
 * @details 封装 YOPEN 平台日志接口，提供统一的日志输出宏定义。
 */

#ifndef IOT_LOG_YOPEN_H
#define IOT_LOG_YOPEN_H

#include "yopen_debug.h"

// 日志定义，打印函数史，行号，以及换行回车
#undef LOG
#define LOG(fmt, ...) yopen_trace("[iot] %s():%d " fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define iot_log_debug(tag, fmt, ...) \
    yopen_trace("[%s] %s():%d " fmt "\r\n", tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define iot_log_info(tag, fmt, ...) \
    yopen_trace("[%s] %s():%d " fmt "\r\n", tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define iot_log_warn(tag, fmt, ...) \
    yopen_trace("[%s] %s():%d " fmt "\r\n", tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define iot_log_error(tag, fmt, ...) \
    yopen_trace("[%s] %s():%d " fmt "\r\n", tag, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif /* IOT_LOG_YOPEN_H */