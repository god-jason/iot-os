/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_config.h
 *
 * @brief     os配置头文件.
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

#ifndef __OS_CONFIG_H__
#define __OS_CONFIG_H__


/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */
//#define OS_DEBUG_INIT 1
//#define USE_TOP_ICC_ASIC
#define ARCH_RISCV_N310
#define ARCH_CPU_64BIT
#define ARCH_RISCV

#if defined(USE_TOP_PM)
#define OS_USING_PM
#endif

#define OS_USING_FAST_RELEASE_RRC

#define OS_USING_TCM
#define OS_USING_RE_IRAM

#define OS_USING_CPU_FFS
#define OS_USING_BENCHMARK
#define OS_USING_THREAD_METRIC

/* RT-Thread Kernel */

#define OS_NAME_MAX 16
#define OS_ALIGN_SIZE 8
#define OS_THREAD_PRIORITY_32
#define OS_THREAD_PRIORITY_MAX 32
#if !defined(_CPU_AP)
#define OS_THREAD_IDLE_CLEANUP
#endif
#define OS_TICK_PER_SECOND 1000
#define OS_USING_OVERFLOW_CHECK
//#define OS_USING_HOOK
//#define OS_USING_IDLE_HOOK
#define OS_IDLE_HOOK_LIST_SIZE 4
#define OS_TIMER_THREAD_STACK_SIZE g_osTimerThreadStackSize
#define OS_DEBUG
#define OS_DEBUG_COLOR

/* Printf  */

#if defined(_CPU_AP) || defined(_PLAT_TEST)
#define OS_PRINTF_LONGLONG
#define OS_PRINTF_FLOAT
//#define OS_PRINTF_SPECIAL
#endif

/* Inter-Thread communication */
#define OS_USING_SEMAPHORE
#define OS_USING_MUTEX
#define OS_USING_MAILBOX
#if defined(_CPU_AP) || defined(_PLAT_TEST)
#define OS_USING_EVENT
#define OS_USING_MESSAGEQUEUE
#define OS_USING_TIMER
#define OS_USING_TIMER_SOFT
#define OS_USING_HRTIMER
#endif

/* Memory Management */

#define OS_USING_MEMPOOL
#define OS_USING_MEM_TRACE
#if defined(_CPU_AP)
#define OS_USING_HEAP
#define OS_USING_DLMALLOC
#define OS_USING_AP_SYSTEM_MEM
#define OS_USING_AP_UPNODE
#define OS_USING_AP_SHAREBUF
#define OSI_USING_PROFILE
#else
#ifdef _PLAT_TEST
#define OS_USING_HEAP
#endif
#define OS_USING_CP_SYSTEM_MEM
#define OS_USING_CP_HARQBUF
#endif
#define OS_USING_MODEM_SHAREBUF
#define OS_USING_SMALLHEAP
#define OS_USING_SMALLHEAP_STAT

/* Work Queue */
#if defined(_CPU_AP)
#define OS_USING_WORKQUEUE
#ifdef _PLAT_TEST
#define OS_WORKQUEUE_THREAD_STACK_SIZE 4096
#define OS_WORKQUEUE_THREAD_PRIORITY 10
#else
#define OS_WORKQUEUE_THREAD_STACK_SIZE g_osWorkQueueThreadStackSize
#define OS_WORKQUEUE_THREAD_PRIORITY 16
#endif
#endif

/* Kernel Device Object */

//#define OS_USING_DEVICE
//#define OS_USING_CONSOLE
#define OS_CONSOLEBUF_SIZE 255
//#define OS_CONSOLE_DEVICE_NAME "uart1"

/* RT-Thread Components */

#define OS_USING_COMPONENTS_INIT
#define OS_USING_USER_MAIN
#ifdef _CPU_AP
#define OS_MAIN_THREAD_STACK_SIZE 4096
#else
#define OS_MAIN_THREAD_STACK_SIZE 2016
#endif

#if defined(_CPU_AP)
#define OS_USING_SHUTDOWN
#define OS_USING_MODULE
#endif

#ifdef OS_USING_SHELL
#ifdef _PLAT_TEST
#define OS_SHELL_THREAD_STACK_SIZE 4000
#define OS_SHELL_THREAD_PRIORITY 10
#else
#define OS_SHELL_THREAD_STACK_SIZE 2016
#define OS_SHELL_THREAD_PRIORITY 25
#endif
#endif
/* C++ features */

/* File system */
#ifdef OS_USING_VFS
#define VFS_THREADSAFE
#define VFS_SUPPORT_CWD
#define VFS_SHELL_CMD
#endif

/* Command shell */

//#define OS_USING_FINSH

/* Device virtual file system */


/* Device Drivers */

#define OS_USING_DEVICE_IPC
#define OS_PIPE_BUFSZ 512
//#define OS_USING_SERIAL
/*#define OS_SERIAL_USING_DMA*/

/* Using Flash */
#if defined(_CPU_AP)
//#define OS_USING_XIPSFC_DMA
#define OS_USING_XIP
#define OS_USING_XIPREQ_INT
#define OS_USING_FLASH_IDLE_LOOP
//#define OS_USING_XIPSFC_XFER_INT
#endif

/* Using WiFi */


/* Using USB */


/* POSIX layer and C standard library */
#define OS_USING_MINILIBC


/* Network */

/* Socket abstraction layer */


/* light weight TCP/IP stack */


/* Modbus master and slave stack */


/* AT commands */


/* VBUS(Virtual Software BUS) */


/* Utilities */


/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */


/* multimedia packages */


/* tools packages */


/* system packages */


/* peripheral libraries and drivers */


/* miscellaneous packages */


/* sample package */

/* samples: kernel and components samples */


/* example package: hello */


/* Privated Packages of RealThread */


/* Network Utilities */

/* Trace */
#define OS_USING_THREAD_TRACE
#define OS_USING_IRQ_TRACE
#define OS_USING_SCHEDULER_HOOK
#define OS_USING_INTERRUPT_HOOK
#define OS_USING_CPU_MONITER
//#define OS_USING_ARCH_PERF
#define OS_USING_TRACE_RAM

#ifdef USE_TOP_FPGA
#define FPGA_TIME_DELAY_RATE    (8)
#endif
#ifdef USE_TOP_ASIC
#define FPGA_TIME_DELAY_RATE    (1)
#endif

#endif
