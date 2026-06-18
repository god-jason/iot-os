/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 * Copyright (c) 2019-Present Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020/03/26     hqfang       Nuclei RISC-V Core porting code.
 */

#ifndef __CPUPORT_H__
#define __CPUPORT_H__

#include <os_config.h>
#include <riscv_interrupt.h>
#include <riscv_timer.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief Execute fence instruction, p -> pred, s -> succ
 * \details
 * the FENCE instruction ensures that all memory accesses from instructions preceding
 * the fence in program order (the `predecessor set`) appear earlier in the global memory order than
 * memory accesses from instructions appearing after the fence in program order (the `successor set`).
 * For details, please refer to The RISC-V Instruction Set Manual
 * \param p     predecessor set, such as iorw, rw, r, w
 * \param s     successor set, such as iorw, rw, r, w
 **/
#define __FENCE(p, s) __asm__ volatile ("fence " #p "," #s : : : "memory")

/**
 * \brief   Fence.i Instruction
 * \details
 * The FENCE.I instruction is used to synchronize the instruction
 * and data streams.
 */
static inline void __FENCE_I(void)
{
    __asm__ volatile("fence.i");
}

/** \brief Read & Write Memory barrier */
#define __RWMB()        __FENCE(iorw,iorw)

/* Scheduler utilities. */
#define RT_YIELD()                                                              \
{                                                                               \
    /* Set a software interrupt(SWI) request to request a context switch. */    \
    SysTimer_SetSWIRQ();                                                        \
    /* Barriers are normally not required but do ensure the code is completely  \
    within the specified behaviour for the architecture. */                     \
    __RWMB();                                                                   \
    __FENCE_I();                                                                \
}

extern void rt_hw_ticksetup(void);
extern void rt_hw_taskswitch(void);
/** ict add**/
extern void rt_hw_interrupt_clrpending(int irq_id);
#ifdef __cplusplus
}
#endif

#endif
