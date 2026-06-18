/**
 * @file        cm_mem.h
 * @brief       内存管理
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By ljw
 * @date        2021/05/28
 *
 * @defgroup mem mem
 * @ingroup SYS
 * @{
 */

#ifndef __CM_MEM_H__
#define __CM_MEM_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "stdint.h"
#include "stddef.h"

//#include "memleak.h"


/**
 * @ingroup cm_mem 内存管理
 * @{
 */
 
/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** heap状态 */
typedef struct{
    uint32_t total_size;            /*!< 总量大小 */
    uint32_t allocated;             /*!< 目前已分配大小 */
    uint32_t free;                  /*!< 剩余可分配大小 */
    uint32_t max_continuity_free;   /*!< 本平台无效   */
    uint32_t allocated_peak;        /*!< 本平台无效   */
    uint32_t allocated_times;       /*!< 本平台无效   */
    uint32_t free_times;            /*!< 本平台无效   */
} cm_heap_stats_t;

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * @brief 获取目前系统heap状态
 *  
 * @param [out] stats heap状态
 *
 * @return 
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details 获取的为MemPool信息
 */
int32_t cm_mem_get_heap_stats(cm_heap_stats_t *stats);

/**
 * @brief 申请内存
 *  
 * @param [in] size 申请长度
 *
 * @return 返回内存地址
 *  
 * @details 内存操作接口请勿使用C库接口，申请长度不能超过剩余可分配大小
 */
//void *cm_malloc(size_t size);


/**
 * @brief 申请内存
 *  
 * @param [in] ptr 待重新分配的内存块
 * @param [in] size 内存块的新大小
 *
 * @return 返回内存地址
 *  
 * @details 内存操作接口请勿使用C库接口，申请长度不能超过剩余可分配大小
 */
//void *cm_realloc(void *ptr, size_t size);

/**
 * @brief 申请内存
 *  
 * @param [in] nitems 要被分配的元素个数
 * @param [in] size 元素的大小
 *
 * @return 返回内存地址
 *  
 * @details 内存操作接口请勿使用C库接口，申请长度不能超过剩余可分配大小
 */
//void *cm_calloc(size_t nitems, size_t size);

/**
 * @brief 释放内存
 *  
 * @param [in] buf 待释放内存
 *
 * @return 空
 *  
 * @details 内存操作接口请勿使用C库接口
 */ 
//void cm_free(void *ptr);

#ifdef CM_MEMLEAK_SUPPORT
#define cm_malloc(n)        cm_dbg_malloc(n)
#define cm_realloc(p,s)     cm_dbg_realloc(p, s)
#define cm_calloc(n,s)      cm_dbg_calloc(n, s)
#define cm_free(buf)    \
            do                  \
            {                   \
                if (buf)        \
                {               \
                    cm_dbg_free(buf);  \
                    buf = NULL; \
                }               \
            } while (0)
#else
#define cm_malloc(n)           malloc(n)
#define cm_realloc(n,s)        realloc(n, s)
#define cm_calloc(n,s)         calloc(n, s)
#define cm_free(buf)    \
            do                  \
            {                   \
                if (buf)        \
                {               \
                    free(buf);  \
                    buf = NULL; \
                }               \
            } while (0)
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif

/** @}*/

#endif /* __CM_MEM_H__ */

