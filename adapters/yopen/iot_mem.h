/**
 * @file iot_mem.h
 * @brief YOPEN 平台内存管理适配器头文件
 * @details 提供内存管理功能封装，支持内存分配、释放、查询等。
 */

#ifndef IOT_MEM_YOPEN_H
#define IOT_MEM_YOPEN_H

#include "../../iot_types.h"

/* ============================================================
 * 内存管理宏定义
 * ============================================================ */

/**
 * @brief 分配内存
 * @param[in] size 内存大小(字节)
 * @return 内存指针，失败返回NULL
 */
#define iot_mem_malloc(size) \
    malloc((size_t)(size))

/**
 * @brief 释放内存
 * @param[in] ptr 内存指针
 */
#define iot_mem_free(ptr) \
    free((void *)(ptr))

/**
 * @brief 重新分配内存
 * @param[in] ptr 原内存指针
 * @param[in] size 新大小(字节)
 * @return 新内存指针，失败返回NULL
 */
#define iot_mem_realloc(ptr, size) \
    realloc((void *)(ptr), (size_t)(size))

/**
 * @brief 分配内存（清零）
 * @param[in] nmemb 元素个数
 * @param[in] size 元素大小(字节)
 * @return 内存指针，失败返回NULL
 */
#define iot_mem_calloc(nmemb, size) \
    calloc((size_t)(nmemb), (size_t)(size))

/**
 * @brief 获取内存堆信息
 * @param[out] info 内存堆信息结构体
 * @return 0 成功，<0 失败
 */
int32_t iot_mem_get_heap_info(iot_mem_heap_info_t *info);

#endif /* IOT_MEM_YOPEN_H */
