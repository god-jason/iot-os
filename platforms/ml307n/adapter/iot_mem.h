/**
 * @file iot_mem.h
 * @brief ML307N 平台内存管理适配器头文件
 * @details 封装 ML307N 平台 cm_mem 接口，提供统一的内存分配/释放/查询操作宏定义。
 *          包含堆内存和SLAB缓存管理，编译时零开销映射到底层函数。
 */

#ifndef IOT_MEM_ML307N_H
#define IOT_MEM_ML307N_H

#include "cm_mem.h"

/** @brief 内存对齐分配（按4字节对齐）
 * @param[in] size 分配内存大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_malloc(size) \
    cm_malloc((size_t)(size))

/** @brief 内存分配并清零（按4字节对齐）
 * @param[in] nmemb 元素个数
 * @param[in] size  每个元素大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_calloc(nmemb, size) \
    cm_calloc((size_t)(nmemb), (size_t)(size))

/** @brief 重新分配内存（按4字节对齐）
 * @param[in] ptr  原内存指针（可为NULL）
 * @param[in] size 新内存大小（字节）
 * @return 新分配的内存指针，NULL表示失败
 */
#define iot_realloc(ptr, size) \
    cm_realloc((void *)(ptr), (size_t)(size))

/** @brief 释放内存
 * @param[in] ptr 待释放的内存指针（可为NULL）
 */
#define iot_free(ptr) \
    cm_free((void *)(ptr))

#endif /* IOT_MEM_ML307N_H */
