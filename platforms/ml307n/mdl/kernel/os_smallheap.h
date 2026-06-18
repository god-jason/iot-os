/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_smallheap.h
 *
 * @brief     os smallheap头文件.
 *
 * @revision  1.0
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version   <th>Author     <th>Description
 * <tr><td>2023-05-23  <td>1.0       <td>ICT Team   <td>初始版本
 * </table>
 ************************************************************************************
 */

#ifndef __OS_SMALLHEAP_H__
#define __OS_SMALLHEAP_H__

#define SMALLHEAP_POOL_MAX_LEVELS   (32)

typedef struct {
    osList_t head;
} mem_list_t;

typedef struct {
    void *start;
    void *end;
    uint32_t min_shift;
    uint32_t min_size;
    uint32_t min_mask;
    uint32_t max_size;
    uint32_t free_bitmap;
    mem_list_t  free_list[SMALLHEAP_POOL_MAX_LEVELS];
#ifdef OS_USING_SMALLHEAP_STAT
    uint32_t max_used;
    uint32_t used;
    uint32_t user_used;
    uint16_t alloc_cnt[SMALLHEAP_POOL_MAX_LEVELS];
    uint16_t max_alloc_cnt[SMALLHEAP_POOL_MAX_LEVELS];
#endif
} memPool_t;

typedef struct {
    memPool_t  mem_pool[2];
#ifdef OS_USING_SMALLHEAP_STAT
    uint32_t max_alloc;
    uint32_t min_alloc;
#endif
} memHeap_t;

int   osSmallheapInit(memHeap_t *heap, uint32_t min_shift, void *start, void *end);
void* osSmallheapMalloc(memHeap_t *heap, uint32_t size);
void* osSmallheapAlignedMalloc(memHeap_t *heap, uint32_t align, uint32_t size);
void* osSmallheapRealloc(memHeap_t *heap, void *ptr, uint32_t size);
void  osSmallheapFree(memHeap_t *heap, void *ptr);
void  osSmallheapPrint(memHeap_t *heap, int detail);
void osSmallheapinfo(uint32_t *heap_size, uint32_t *heap_alloced, uint32_t *heap_avalible);

#endif  /* End of __OS_SMALLHEAP_H__ */

