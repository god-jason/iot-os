/**
 * @file iot_mem.h
 * @brief ML307N 平台内存管理适配器头文件
 * @details 封装 ML307N 平台 cm_mem 接口，提供统一的内存分配/释放/查询操作宏定义。
 *          包含堆内存和SLAB缓存管理，编译时零开销映射到底层函数。
 */

#ifndef IOT_MEM_ML307N_H
#define IOT_MEM_ML307N_H

#include "../../iot_types.h"
#include "cm_mem.h"

/** @brief 内存堆状态信息结构体重定义 */
#define iot_mem_heap_info_t     cm_mem_heap_info_t

/** @brief SLAB缓存状态信息结构体重定义 */
#define iot_mem_slab_info_t     cm_mem_slab_info_t

/** @brief 内存对齐分配（按4字节对齐）
 * @param[in] size 分配内存大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_mem_alloc(size) \
    cm_malloc((size_t)(size))

/** @brief 内存分配并清零（按4字节对齐）
 * @param[in] nmemb 元素个数
 * @param[in] size  每个元素大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_mem_calloc(nmemb, size) \
    cm_calloc((size_t)(nmemb), (size_t)(size))

/** @brief 重新分配内存（按4字节对齐）
 * @param[in] ptr  原内存指针（可为NULL）
 * @param[in] size 新内存大小（字节）
 * @return 新分配的内存指针，NULL表示失败
 */
#define iot_mem_realloc(ptr, size) \
    cm_realloc((void *)(ptr), (size_t)(size))

/** @brief 释放内存
 * @param[in] ptr 待释放的内存指针（可为NULL）
 */
#define iot_mem_free(ptr) \
    cm_free((void *)(ptr))

/** @brief 获取当前堆内存使用状态
 * @param[out] info 堆状态信息结构体指针
 */
#define iot_mem_get_heap_info(info) \
    cm_mem_get_heap_info((cm_mem_heap_info_t *)(info))

/** @brief 获取SLAB缓存使用状态
 * @param[out] info SLAB缓存状态信息结构体指针
 */
#define iot_mem_get_slab_info(info) \
    cm_mem_get_slab_info((cm_mem_slab_info_t *)(info))

/** @brief 计算实际分配内存大小
 * @param[in] size 请求分配的内存大小
 * @return 实际分配的内存大小（字节）
 */
#define iot_mem_get_alloc_size(size) \
    cm_mem_get_alloc_size((size_t)(size))

/** @brief 获取程序占用内存大小
 * @return 程序当前占用内存大小（字节）
 */
#define iot_mem_get_progmem_size() \
    cm_mem_get_progmem_size()

#endif /* IOT_MEM_ML307N_H */
