/**
 * @file iot_types.h
 * @brief IoT 平台通用类型和错误码定义
 * @details 定义跨平台兼容的通用类型、错误码和常量，
 *          所有平台适配器头文件应包含此文件以确保接口统一。
 */

#ifndef IOT_TYPES_H
#define IOT_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================
 * 通用错误码定义
 * ============================================================ */

/** @brief 错误码类型 */
typedef int32_t iot_err_t;

/** @brief 操作成功 */
#define IOT_OK              0
/** @brief 通用错误 */
#define IOT_ERR            -1
/** @brief 参数错误 */
#define IOT_ERR_PARAM      -2
/** @brief 内存错误 */
#define IOT_ERR_MEM        -3
/** @brief 忙碌中 */
#define IOT_ERR_BUSY       -4
/** @brief 不支持 */
#define IOT_ERR_NOT_SUPPORT -5
/** @brief 超时 */
#define IOT_ERR_TIMEOUT     -6
/** @brief 无效句柄 */
#define IOT_ERR_INVALID     -7

/* ============================================================
 * 通用时间常量
 * ============================================================ */

/** @brief 永久等待超时值 */
#define IOT_WAIT_FOREVER    0xFFFFFFFF

/* ============================================================
 * 通用句柄类型定义
 * ============================================================ */

/** @brief 成功操作的回调类型 */
typedef void (*iot_callback_t)(void *arg);

/** @brief 数据大小类型 */
typedef uint32_t iot_size_t;

/** @brief 布尔类型 */
typedef bool iot_bool_t;

/* ============================================================
 * 通用状态枚举
 * ============================================================ */

/** @brief 通用状态枚举 */
typedef enum {
    IOT_FALSE = 0,
    IOT_TRUE  = 1,
} iot_status_t;

#endif /* IOT_TYPES_H */
