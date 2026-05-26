/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_MEM_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>
#include "cm_mem.h"

#define cm_demo_printf osPrintf

void SHELL_testMemHeap(char argc, char **argv)
{
    cm_heap_stats_t stats = {0};

    if(cm_mem_get_heap_stats(&stats)<0)
    {
        cm_demo_printf("cm_mem_get_heap_stats fail\n");
    }
    cm_demo_printf("heap total:%d,used:%d,remain:%d\n",stats.total_size,stats.allocated,stats.free);

}

void SHELL_testMem(char argc, char **argv)
{
    char* buf1 = NULL;
    char* buf2 = NULL;
    char* buf3 = NULL;
    char* buf4 = NULL;
    int ret;
    cm_heap_stats_t stats = {0};

    cm_demo_printf("CM OpenCPU testMem Starts\n");
    if(cm_mem_get_heap_stats(NULL)==0)
    {
        cm_demo_printf("cm_mem_get_heap_stats test1 fail\n");
    }
    if(cm_mem_get_heap_stats(&stats)<0)
    {
        cm_demo_printf("cm_mem_get_heap_stats test2 fail\n");
    }
    cm_demo_printf("heap total:%d,used:%d,remain:%d\n",stats.total_size,stats.allocated,stats.free);

    buf1 = (char*)cm_malloc(20);
    cm_demo_printf("cm_malloc %p\n", buf1);
    cm_mem_get_heap_stats(&stats);
    cm_demo_printf("after malloc 20,heap total:%d,used:%d,remain:%d\n",stats.total_size,stats.allocated,stats.free);

    buf2 = (char*)cm_realloc(buf1,10);
    cm_demo_printf("cm_realloc %p\n", buf2);
    cm_mem_get_heap_stats(&stats);
    cm_demo_printf("after realloc 10,heap total:%d,used:%d,remain:%d\n",stats.total_size,stats.allocated,stats.free);

    buf3 = (char*)cm_realloc(buf2,30);
    cm_demo_printf("cm_realloc %p\n", buf3);
    cm_mem_get_heap_stats(&stats);
    cm_demo_printf("after realloc 30,heap total:%d,used:%d,remain:%d\n",stats.total_size,stats.allocated,stats.free);

    buf4 = (char*)cm_calloc(40, sizeof(uint8_t));
    cm_demo_printf("cm_calloc %p\n", buf4);
    cm_mem_get_heap_stats(&stats);
    cm_demo_printf("after calloc 40,heap total:%d,used:%d,remain:%d\n",stats.total_size,stats.allocated,stats.free);

    cm_free(buf3);
    cm_free(buf4);
    cm_mem_get_heap_stats(&stats);
    cm_demo_printf("after free 30+40,heap total:%d,used:%d,remain:%d\n",stats.total_size,stats.allocated,stats.free);

    cm_demo_printf("CM OpenCPU testMem Ends\n");
}

NR_SHELL_CMD_EXPORT(cm_mem, SHELL_testMem);
NR_SHELL_CMD_EXPORT(cm_mem_heap, SHELL_testMemHeap);
#endif
#endif

