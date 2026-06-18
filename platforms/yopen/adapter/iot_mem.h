/**
 * @file iot_mem.h
 * @brief YOPEN 平台内存管理适配器头文件
 * @details 封装 YOPEN 平台内存接口，提供统一的内存分配/释放/查询操作宏定义。
 *          包含堆内存和SLAB缓存管理，编译时零开销映射到底层函数。
 */

#ifndef IOT_MEM_YOPEN_H
#define IOT_MEM_YOPEN_H

#include "yopen_type.h"

/** @brief 内存分配
 * @param[in] size 分配内存大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_malloc(size) \
    yopen_malloc((size_t)(size))

/** @brief 内存分配并清零
 * @param[in] nmemb 元素个数
 * @param[in] size  每个元素大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_calloc(nmemb, size) \
    yopen_calloc((size_t)(nmemb), (size_t)(size))

/** @brief 重新分配内存
 * @param[in] ptr  原内存指针（可为NULL）
 * @param[in] size 新内存大小（字节）
 * @return 新分配的内存指针，NULL表示失败
 */
#define iot_realloc(ptr, size) \
    yopen_realloc((void *)(ptr), (size_t)(size))

/** @brief 释放内存
 * @param[in] ptr 待释放的内存指针（可为NULL）
 */
#define iot_free(ptr) \
    yopen_free((void *)(ptr))

/** @brief 获取内存使用信息
 * @param[out] total 总内存
 * @param[out] used 已用内存
 * @param[out] free 空闲内存
 */
#define iot_mem_info(total, used, free) \
    yopen_mem_info((total), (used), (free))

#endif /* IOT_MEM_YOPEN_H */