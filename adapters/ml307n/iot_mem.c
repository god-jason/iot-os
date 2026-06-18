/**
 * @file iot_mem.c
 * @brief ML307N 平台内存管理适配器实现
 * @details 基于 cm_mem 接口实现内存管理功能封装，
 *          提供统一的内存分配/释放/查询操作函数。
 */

#include "iot_mem.h"

/**
 * @brief 内存对齐分配（按4字节对齐）
 * @param[in] size 分配内存大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
void *iot_mem_alloc(uint32_t size)
{
    return cm_malloc((size_t)size);
}

/**
 * @brief 内存分配并清零（按4字节对齐）
 * @param[in] nmemb 元素个数
 * @param[in] size  每个元素大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
void *iot_mem_calloc(uint32_t nmemb, uint32_t size)
{
    return cm_calloc((size_t)nmemb, (size_t)size);
}

/**
 * @brief 重新分配内存（按4字节对齐）
 * @param[in] ptr  原内存指针（可为NULL）
 * @param[in] size 新内存大小（字节）
 * @return 新分配的内存指针，NULL表示失败
 */
void *iot_mem_realloc(void *ptr, uint32_t size)
{
    return cm_realloc(ptr, (size_t)size);
}

/**
 * @brief 释放内存
 * @param[in] ptr 待释放的内存指针（可为NULL）
 */
void iot_mem_free(void *ptr)
{
    cm_free(ptr);
}

/**
 * @brief 获取当前堆内存使用状态
 * @param[out] info 堆状态信息结构体指针
 */
void iot_mem_get_heap_info(iot_mem_heap_info_t *info)
{
    cm_mem_get_heap_info((cm_mem_heap_info_t *)info);
}

/**
 * @brief 获取SLAB缓存使用状态
 * @param[out] info SLAB缓存状态信息结构体指针
 */
void iot_mem_get_slab_info(iot_mem_slab_info_t *info)
{
    cm_mem_get_slab_info((cm_mem_slab_info_t *)info);
}

/**
 * @brief 计算实际分配内存大小
 * @param[in] size 请求分配的内存大小
 * @return 实际分配的内存大小（字节）
 */
uint32_t iot_mem_get_alloc_size(uint32_t size)
{
    return cm_mem_get_alloc_size((size_t)size);
}

/**
 * @brief 获取程序占用内存大小
 * @return 程序当前占用内存大小（字节）
 */
uint32_t iot_mem_get_progmem_size(void)
{
    return cm_mem_get_progmem_size();
}