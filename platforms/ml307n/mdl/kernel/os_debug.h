/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_debug.h
 *
 * @brief     os调试头文件.
 *
 * @revision  1.0
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version   <th>Author     <th>Description
 * <tr><td>2023-06-21  <td>1.0       <td>ICT Team   <td>初始版本
 * </table>
 ************************************************************************************
 */

#ifndef __OS_DEBUG_H__
#define __OS_DEBUG_H__

#include <os_config.h>

/* settings depend check */
#ifdef OS_USING_POSIX
#if !defined(OS_USING_DFS) || !defined(OS_USING_DFS_DEVFS)
#error "POSIX poll/select, stdin need file system(OS_USING_DFS) and device file system(OS_USING_DFS_DEVFS)"
#endif

#if !defined(OS_USING_LIBC)
#error "POSIX layer need standard C library(OS_USING_LIBC)"
#endif

#endif

#ifdef OS_USING_POSIX_TERMIOS
#if !defined(OS_USING_POSIX)
#error "termios need POSIX layer(OS_USING_POSIX)"
#endif
#endif

/* Using this macro to control all kernel debug features. */
#ifdef OS_DEBUG

/* Turn on some of these (set to non-zero) to debug kernel */
#ifndef OS_DEBUG_MEM
#define OS_DEBUG_MEM                   0
#endif

#ifndef OS_DEBUG_MEMHEAP
#define OS_DEBUG_MEMHEAP               0
#endif

#ifndef OS_DEBUG_MODULE
#define OS_DEBUG_MODULE                0
#endif

#ifndef OS_DEBUG_SCHEDULER
#define OS_DEBUG_SCHEDULER             0
#endif

#ifndef OS_DEBUG_SLAB
#define OS_DEBUG_SLAB                  0
#endif

#ifndef OS_DEBUG_THREAD
#define OS_DEBUG_THREAD                0
#endif

#ifndef OS_DEBUG_TIMER
#define OS_DEBUG_TIMER                 0
#endif

#ifndef OS_DEBUG_IRQ
#define OS_DEBUG_IRQ                   0
#endif

#ifndef OS_DEBUG_IPC
#define OS_DEBUG_IPC                   0
#endif

#ifndef OS_DEBUG_INIT
#define OS_DEBUG_INIT                  0
#endif

#ifndef OS_DEBUG_SHUTDOWN
#define OS_DEBUG_SHUTDOWN              0
#endif

/* Turn on this to enable context check */
#ifndef OS_DEBUG_CONTEXT_CHECK
#define OS_DEBUG_CONTEXT_CHECK         1
#endif

#define OS_DEBUG_LOG(type, message)                                           \
do                                                                            \
{                                                                             \
    if (type)                                                                 \
        osPrintf message;                                                   \
}                                                                             \
while (0)

/**
 ************************************************************************************
 * @brief           系统断言
 *
 * @param[in]       EX      断言判断表达式，为假时断言
 *
 * @return          void.
 ************************************************************************************
 */

#define OS_ASSERT(EX)                                                       \
do                                                                          \
{                                                                           \
    if (!(EX))                                                              \
    {                                                                       \
        osAssertHandler(__FILE__, __LINE__);                                \
    }                                                                       \
} while (0)

#define OS_ASSERT_EX(EX, fmt, ...)                                          \
do                                                                          \
{                                                                           \
    if (!(EX))                                                              \
    {                                                                       \
        osPrintf(fmt, ##__VA_ARGS__);                                       \
        osAssertHandler(__FILE__, __LINE__);                                \
    }                                                                       \
} while (0)

/* Macro to check current context */
#if OS_DEBUG_CONTEXT_CHECK
#define OS_DEBUG_NOT_IN_INTERRUPT                                             \
do                                                                            \
{                                                                             \
    ubase_t level;                                                          \
    level = osInterruptDisable();                                        \
    if (osInterruptGetNest() != 0)                                         \
    {                                                                         \
        osPrintf("Function[%s] shall not be used in ISR\n", __FUNCTION__);  \
        OS_ASSERT(0);                                                          \
    }                                                                         \
    osInterruptEnable(level);                                            \
}                                                                             \
while (0)

/* "In thread context" means:
 *     1) the scheduler has been started
 *     2) not in interrupt context.
 */
#define OS_DEBUG_IN_THREAD_CONTEXT                                            \
do                                                                            \
{                                                                             \
    ubase_t level;                                                          \
    level = osInterruptDisable();                                        \
    if (osThreadSelf() == OS_NULL)                                          \
    {                                                                         \
        osPrintf("Function[%s] shall not be used before scheduler start\n", \
                   __FUNCTION__);                                             \
        OS_ASSERT(0);                                                          \
    }                                                                         \
    OS_DEBUG_NOT_IN_INTERRUPT;                                                \
    osInterruptEnable(level);                                            \
}                                                                             \
while (0)
#else
#define OS_DEBUG_NOT_IN_INTERRUPT
#define OS_DEBUG_IN_THREAD_CONTEXT
#endif

#else /* OS_DEBUG */

#define OS_ASSERT(EX)
#define OS_ASSERT_EX(EX, fmt, ...)
#define OS_DEBUG_LOG(type, message)
#define OS_DEBUG_NOT_IN_INTERRUPT
#define OS_DEBUG_IN_THREAD_CONTEXT

#endif /* OS_DEBUG */

#endif /* __OS_DEBUG_H__ */
