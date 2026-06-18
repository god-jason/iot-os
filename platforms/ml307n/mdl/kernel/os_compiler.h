/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        os_compiler.h
 *
 * @brief       编译宏定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-25   ICT Team        创建
 ************************************************************************************
 */

#ifndef _OS_COMPILER_H_
#define _OS_COMPILER_H_

#include <os_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_USING_IRAM

#ifdef OS_USING_TCM
#define __ITCM_CODE         __attribute__((section(".itcm.code"))) __attribute__((noinline))
#define __DTCM_RO           __attribute__((section(".dtcm.rodata")))
#define __DTCM_SR           __attribute__((section(".dtcm.srdata")))
#define __DTCM_RT           __attribute__((section(".dtcm.rtdata")))
#else
#define __ITCM_CODE
#define __DTCM_RO
#define __DTCM_SR
#define __DTCM_RT
#endif

#ifdef OS_USING_XIP
#define __PSRAM_CODE         __attribute__((section(".psram.code"))) __attribute__((noinline))
#define __PSRAM_DATA_RO      __attribute__((section(".psram.rodata")))
#else
#define __PSRAM_CODE
#define __PSRAM_DATA_RO
#endif

#ifdef _CPU_CP
#define __XIP_DATA_RO       __attribute__((section(".xip.rodata")))
#endif

#ifdef OS_USING_IRAM
#define __IRAM_DATA_SHARE   __attribute__((section(".iram.sharedata")))
#define __IRAM_CODE         __attribute__((section(".iram.code"))) __attribute__((noinline))
#define __IRAM_DATA_RO      __attribute__((section(".iram.rodata")))
#define __IRAM_DATA_SR      __attribute__((section(".iram.srdata")))
#define __IRAM_DATA_RT      __attribute__((section(".iram.rtdata")))
#else
#define __IRAM_DATA_SHARE
#define __IRAM_CODE
#define __IRAM_DATA_RO
#define __IRAM_DATA_SR
#define __IRAM_DATA_RT
#endif

#define __PSRAM_BSS_DEBUG __attribute__((section(".psram.debugbss")))

#ifdef OS_USING_RE_IRAM
#define __IRAM_CODE_PSM_RE  __attribute__((section(".iram.psm_recode"))) __attribute__((noinline))
#define __IRAM_DATA_PSM_RE  __attribute__((section(".iram.psm_redata")))
#define __IRAM_CODE_RE      __attribute__((section(".iram.recode"))) __attribute__((noinline))
#define __IRAM_DATA_RE      __attribute__((section(".iram.redata")))
#else
#define __IRAM_CODE_PSM_RE
#define __IRAM_DATA_PSM_RE
#define __IRAM_CODE_RE
#define __IRAM_DATA_RE
#endif

#define IRAM_CACHE_TO_NONCACHE_OFFSET       (0xB0000000)
#define DDR_CACHE_TO_NONCACHE_OFFSET        (0x40000000)

#define IRAM_ADDR_CACHE_TO_NONCACHE(addr)   ((ubase_t)(addr) - IRAM_CACHE_TO_NONCACHE_OFFSET)
#define IRAM_ADDR_NONCACHE_TO_CACHE(addr)   ((ubase_t)(addr) + IRAM_CACHE_TO_NONCACHE_OFFSET)

#define DDR_ADDR_CACHE_TO_NONCACHE(addr)    ((ubase_t)(addr) - DDR_CACHE_TO_NONCACHE_OFFSET)
#define DDR_ADDR_NONCACHE_TO_CACHE(addr)    ((ubase_t)(addr) + DDR_CACHE_TO_NONCACHE_OFFSET)

#define ITCM_ADDR_TO_SLAVE_ITCM(addr)       ((ubase_t)(addr) + 0xF3FF8000)
#define DTCM_ADDR_TO_SLAVE_DTCM(addr)       ((ubase_t)(addr) + 0xF3FF4000)

#define CPU_SHIFT   (0)

#ifndef __ASSEMBLER__
extern int __text_start;
extern int __text_end;
extern int __tcm_code_start;
extern int __tcm_text_start;
extern int __tcm_code_end;
extern int __tcm_data_start;
extern int __tcm_data_end;
extern int __tcm_srdata_start;
extern int __tcm_srdata_end;

extern int __stack_start;
extern int __stack_end;

extern int __iram_code_start;
extern int __iram_rodata_start;
extern int __psram_code_start;
extern int __psram_code_end;

extern int __malloc_start;
extern int __malloc_end;

#define HEAP_BEGIN  (&__malloc_start)
#define HEAP_END    (&__malloc_end)

#endif

#ifdef __cplusplus
}
#endif

#endif /* End of _OS_COMPILER_H_ */
