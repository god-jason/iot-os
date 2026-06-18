/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_def.h
 *
 * @brief     os定义头文件.
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

#ifndef __OS_DEF_H__
#define __OS_DEF_H__

/* include rtconfig header to import configuration */
#include <os_config.h>

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup OsBasic
 */

/*@{*/

/* RT-Thread version information */
#define OS_VERSION                      3L              /**< major version number */
#define OS_SUBVERSION                   1L              /**< minor version number */
#define OS_REVISION                     5L              /**< revise version number */

/* RT-Thread version */
#define RTTHREAD_VERSION                ((OS_VERSION * 10000) + \
                                         (OS_SUBVERSION * 100) + OS_REVISION)

typedef int                             bool_t;      /**< boolean type */
typedef long                            base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   ubase_t;     /**< Nbit unsigned CPU related data type */

typedef uint64_t                     osTime_t;      /**< Type for time stamp */
typedef uint64_t                     osTick_t;      /**< Type for tick count */
typedef base_t                       osFlag_t;      /**< Type for flags */
typedef ubase_t                      osDev_t;       /**< Type for device */
typedef base_t                       osOff_t;       /**< Type for offset */

/* boolean type definitions */
#define OS_TRUE                         1               /**< boolean true  */
#define OS_FALSE                        0               /**< boolean fails */

/* maximum value of base type */
#define OS_UINT8_MAX                    0xff            /**< Maxium number of UINT8 */
#define OS_UINT16_MAX                   0xffff          /**< Maxium number of UINT16 */
#define OS_UINT32_MAX                   0xffffffff      /**< Maxium number of UINT32 */
#define OS_UINT64_MAX                   0xffffffffffffffff      /**< Maxium number of UINT64 */
#define OS_TICK_MAX                     OS_UINT64_MAX   /**< Maxium number of tick */

/* maximum value of ipc type */
#define OS_SEM_VALUE_MAX                OS_UINT16_MAX   /**< Maxium number of semaphore .value */
#define OS_MUTEX_VALUE_MAX              OS_UINT16_MAX   /**< Maxium number of mutex .value */
#define OS_MUTEX_HOLD_MAX               OS_UINT8_MAX    /**< Maxium number of mutex .hold */
#define OS_MB_ENTRY_MAX                 OS_UINT16_MAX   /**< Maxium number of mailbox .entry */
#define OS_MQ_ENTRY_MAX                 OS_UINT16_MAX   /**< Maxium number of message queue .entry */


#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define __CLANG_ARM
#endif

/* Compiler Related Definitions */
#if defined(__CC_ARM) || defined(__CLANG_ARM)           /* ARM Compiler */
    #include <stdarg.h>
    #define SECTION(x)                  __attribute__((section(x)))
    #define OS_UNUSED                   __attribute__((unused))
    #define OS_USED                     __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))

    #define OS_WEAK                     __attribute__((weak))
    #define OS_INLINE                   static __inline
    /* module compiling */
    #ifdef OS_USING_MODULE
        #define RTT_API                 __declspec(dllimport)
    #else
        #define RTT_API                 __declspec(dllexport)
    #endif

#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #include <stdarg.h>
    #define SECTION(x)                  @ x
    #define OS_UNUSED
    #define OS_USED                     __root
    #define PRAGMA(x)                   _Pragma(#x)
    #define ALIGN(n)                    PRAGMA(data_alignment=n)
    #define OS_WEAK                     __weak
    #define OS_INLINE                   static inline
    #define RTT_API

#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #ifdef OS_USING_NEWLIB
        #include <stdarg.h>
    #else
        /* the version of GNU GCC must be greater than 4.x */
        typedef __builtin_va_list       __gnuc_va_list;
        typedef __gnuc_va_list          va_list;
        #define va_start(v,l)           __builtin_va_start(v,l)
        #define va_end(v)               __builtin_va_end(v)
        #define va_arg(v,l)             __builtin_va_arg(v,l)
    #endif

    #define SECTION(x)                  __attribute__((section(x)))
    #define OS_UNUSED                   __attribute__((unused))
    #define OS_USED                     __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))
    #define OS_WEAK                     __attribute__((weak))
    #define OS_INLINE                   static __inline
    #define RTT_API
#elif defined (__ADSPBLACKFIN__)        /* for VisualDSP++ Compiler */
    #include <stdarg.h>
    #define SECTION(x)                  __attribute__((section(x)))
    #define OS_UNUSED                   __attribute__((unused))
    #define OS_USED                     __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))
    #define OS_WEAK                     __attribute__((weak))
    #define OS_INLINE                   static inline
    #define RTT_API
#elif defined (_MSC_VER)
    #include <stdarg.h>
    #define SECTION(x)
    #define OS_UNUSED
    #define OS_USED
    #define ALIGN(n)                    __declspec(align(n))
    #define OS_WEAK
    #define OS_INLINE                   static __inline
    #define RTT_API
#elif defined (__TI_COMPILER_VERSION__)
    #include <stdarg.h>
    /* The way that TI compiler set section is different from other(at least
     * GCC and MDK) compilers. See ARM Optimizing C/C++ Compiler 5.9.3 for more
     * details. */
    #define SECTION(x)
    #define OS_UNUSED
    #define OS_USED
    #define PRAGMA(x)                   _Pragma(#x)
    #define ALIGN(n)
    #define OS_WEAK
    #define OS_INLINE                   static inline
    #define RTT_API
#else
    #error not supported tool chain
#endif

/**@}*/

/**
 * @addtogroup OsInit
 */

/*@{*/

/* initialization export */
#ifdef OS_USING_COMPONENTS_INIT

/*
 *  flash_qspi driver   at OS_INIT_SUBLEVEL_HIGH
 *  sflash driver       at OS_INIT_SUBLEVEL_2
 *  fal_part driver     at OS_INIT_SUBLEVEL_3
 *  NV driver           at OS_INIT_SUBLEVEL_4
 *  RNDIS driver        at OS_INIT_SUBLEVEL_MIDDLE
 *  USB devices driver  at OS_INIT_SUBLEVEL_LOW
 *
 *  Tips:
 *  Not change the init level of flash_qspi -> sflash -> fal_part -> NV;
 *
 *  If driver use NV to read/save paramters, its init level must be higer than
 *  OS_INIT_SUBLEVEL_4 (lower priority);
 */

#define OS_INIT_SUBLEVEL_HIGH   "1"
#define OS_INIT_SUBLEVEL_2      "2"
#define OS_INIT_SUBLEVEL_3      "3"
#define OS_INIT_SUBLEVEL_4      "4"
#define OS_INIT_SUBLEVEL_MIDDLE "5"
#define OS_INIT_SUBLEVEL_6      "6"
#define OS_INIT_SUBLEVEL_LOW    "7"


typedef int (*init_fn_t)(void);
#ifdef _MSC_VER /* we do not support MS VC++ compiler */
    #define INIT_EXPORT(fn, level, sublevel)
#else
    #if OS_DEBUG_INIT
        struct rt_init_desc
        {
            const char* fn_name;
            const init_fn_t fn;
        };
        #define INIT_EXPORT(fn, level, sublevel)                                                       \
            const char __rti_##fn##_name[] = #fn;                                            \
            OS_USED const struct rt_init_desc __rt_init_desc_##fn SECTION(".init_call." level sublevel) = \
            { __rti_##fn##_name, fn};
    #else
        #define INIT_EXPORT(fn, level, sublevel)                                                       \
            OS_USED const init_fn_t __rt_init_##fn SECTION(".init_call." level sublevel) = fn
    #endif
#endif
#else
#define INIT_EXPORT(fn, level, sublevel)
#endif

/*
 * Components Initialization will initialize some driver and components as following
 * order:
 *
 * rti_board_start   --> 0.0
 *
 *
 * BOARD_EXPORT      --> 0.0
 ***********************************
 * PCU                |---> 0.1
 * UART(CPU)          |---> 0.2
 * DMA                |---> 0.3
 * FLASH              |---> 0.4
 * RTC                |---> 0.5
 *
 ***********************************
 * rti_board_end     --> 0.end
 *
 *
 * CORE_EXPORT       --> 1.0
 ************************************
 * LittleFS           |---> 1.5
 * NV                 |---> 1.6
 *
 ***********************************
 *
 *
 * BUS_EXPORT        --> 2.0
 ***********************************
 * I2C                |---> 2.1
 * SPI                |---> 2.2
 * NET(ECM/RNDIS)     |---> 2.3
 ***********************************
 *
 *
 * DEVICE_EXPORT     --> 3.0
 ***********************************
 * LCD                |---> 3.1
 * CAM                |---> 3.2
 *
 ***********************************
 *
 *
 * COMPONENT_EXPORT  --> 4.0
 ***********************************
 * AGENT              |---> 4.1
 * SHELL              |---> 4.2
 *
 ***********************************
 *
 *
 * MODEM_EXPORT      --> 5.0
 ***********************************
 * PHY_ENTRY          |---> 5.1
 * PS_ENTRY           |---> 5.2
 *
 ***********************************
 *
 *
 * MIDDLEWARE_EXPORT --> 6.0
 ***********************************
 * AT                |---> 6.1
 * OTA               |---> 6.2
 *
 *
 ***********************************
 *
 *
 * APP_EXPORT        --> 7.0
 ***********************************
 * APP_LEVEL1        |---> 7.1
 * APP_LEVEL2        |---> 7.2
 * APP_LEVEL3        |---> 7.3
 * APP_LEVEL4        |---> 7.4
 * APP_LEVEL5        |---> 7.5
 * APP_LEVEL6        |---> 7.6
 *
 ***********************************
 *
 * rti_end           --> 7.end
 *
 * These automatically initialization, the driver or component initial function must
 * be defined with:
 * INIT_BOARD_EXPORT(fn);
 * INIT_DEVICE_EXPORT(fn);
 * ...
 * INIT_APP_EXPORT(fn);
 * etc.
 */

/**
 * @name 开机初始化接口调用
 * @{
 */

/* board init routines will be called in board_init() function */
#define INIT_BOARD_EXPORT(fn, sublevel)             INIT_EXPORT(fn, "0.", sublevel)  ///< board init routines will be called in board_init() function

/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initilization) */
/* board init routines will be called in board_init() function */
#define INIT_CORE_EXPORT(fn, sublevel)              INIT_EXPORT(fn, "1.", sublevel)  ///< pre/device/component/env/app init routines will be called in init_thread

#define INIT_BUS_EXPORT(fn, sublevel)               INIT_EXPORT(fn, "2.", sublevel)  ///< bus initialization
/* device initialization */
#define INIT_DEVICE_EXPORT(fn, sublevel)            INIT_EXPORT(fn, "3.", sublevel)  ///< device initialization
/* components initialization (dfs, lwip, ...) */
#define INIT_COMPONENT_EXPORT(fn, sublevel)         INIT_EXPORT(fn, "4.", sublevel)  ///< components initialization (dfs, lwip, ...)
/* modem initialization (phy,ps ...) */
#define INIT_MODEM_EXPORT(fn, sublevel)             INIT_EXPORT(fn, "5.", sublevel)  ///< modem initialization (phy,ps ...)
/* middleware initialization (at,ota etc ...) */
#define INIT_MIDDLEWARE_EXPORT(fn, sublevel)        INIT_EXPORT(fn, "6.", sublevel)  ///< middleware initialization (at,ota etc ...)
/* appliation initialization (rtgui application etc ...) */
#define INIT_APP_EXPORT(fn, sublevel)               INIT_EXPORT(fn, "7.", sublevel)  ///< appliation initialization (rtgui application etc ...)
/**@}*/


/* exit function for system shutdown and reboot */

typedef enum osShutdownType {
    OS_SHUTDOWN = 1,
    OS_REBOOT,
} osShutdownType_t;

typedef enum osRebootReason {
    OS_REBOOT_DOWNLOAD = 0,
    OS_REBOOT_NORMAL,
    OS_REBOOT_FOTA,
    OS_REBOOT_EXCEPT,
    OS_SHUTDOWN_Immediately = 0,
    OS_SHUTDOWN_NORMAL = 1,
} osRebootReason_t;

/**
 ************************************************************************************
 * @brief           关机通知回调.
 *
 * @param[in]       type            关机类型;
 * @param[in]       reason          关机原因;
 * @return          0
 *
 * @note 关机任务栈受限，在此函数中不要执行复杂的流程，例：文件的相关操作等;
 *       若有复杂流程需求，可提交到 WorkQueue, 然后等待 work 完成后再返回.
 ************************************************************************************
 */
typedef int (*shutdown_fn_t)(osShutdownType_t type, osRebootReason_t reason);

#ifndef OS_USING_SHUTDOWN
    #define SHUTDOWN_EXPORT(fn, level, sublevel)
#else
    #if OS_DEBUG_SHUTDOWN
        struct rt_shutdown_desc
        {
            const char* fn_name;
            const shutdown_fn_t fn;
        };
        #define SHUTDOWN_EXPORT(fn, level, sublevel)                                                       \
            const char __rti_##fn##_name[] = #fn;                                            \
            OS_USED const struct rt_exit_desc __rt_exit_desc_##fn SECTION(".shutdown_call." level sublevel) = \
            { __rti_##fn##_name, fn};
    #else
        #define SHUTDOWN_EXPORT(fn, level, sublevel)                                                       \
            OS_USED const shutdown_fn_t __rt_exit_##fn SECTION(".shutdown_call." level sublevel) = fn
    #endif
#endif

#define OS_SHUTDOWN_SUBLEVEL_HIGH   "1"
#define OS_SHUTDOWN_SUBLEVEL_2      "2"
#define OS_SHUTDOWN_SUBLEVEL_3      "3"
#define OS_SHUTDOWN_SUBLEVEL_4      "4"
#define OS_SHUTDOWN_SUBLEVEL_MIDDLE "5"
#define OS_SHUTDOWN_SUBLEVEL_6      "6"
#define OS_SHUTDOWN_SUBLEVEL_LOW    "7"

/**
 * @name 关机去初始化接口调用
 * @{
 */
#define SHUTDOWN_PREPARE_EXPORT(fn, sublevel)               SHUTDOWN_EXPORT(fn, "0.", sublevel)  ///< prepare shutdown
#define SHUTDOWN_MODEM_EXPORT(fn, sublevel)                 SHUTDOWN_EXPORT(fn, "1.", sublevel)  ///< shutdown functions for modem
#define SHUTDOWN_MIDDLEWARE_EXPORT(fn, sublevel)            SHUTDOWN_EXPORT(fn, "2.", sublevel)  ///< shutdown functions for middleware
#define SHUTDOWN_STORAGE_EXPORT(fn, sublevel)               SHUTDOWN_EXPORT(fn, "3.", sublevel)  ///< shutdown functions for storage (filesystem、nv)
#define SHUTDOWN_NOTIFY_EXPORT(fn, sublevel)                SHUTDOWN_EXPORT(fn, "4.", sublevel)  ///< need some notify for application
#define SHUTDOWN_DEVICE_EXPORT(fn, sublevel)                SHUTDOWN_EXPORT(fn, "5.", sublevel)  ///< shutdown functions for devices (lcd...)
#define SHUTDOWN_LAST_EXPORT(fn, sublevel)                  SHUTDOWN_EXPORT(fn, "7.", sublevel)  ///< the last shutdown function
/**@}*/


/**@}*/

/* kernel malloc definitions */
#ifndef OS_KERNEL_MALLOC
#define OS_KERNEL_MALLOC(sz)            osMalloc(sz)
#endif

#ifndef OS_KERNEL_FREE
#define OS_KERNEL_FREE(ptr)             osFree(ptr)
#endif

#ifndef OS_KERNEL_REALLOC
#define OS_KERNEL_REALLOC(ptr, size)    osRealloc(ptr, size)
#endif

/**
 * @ingroup OsBasic
 *
 * @def OS_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. OS_ALIGN(13, 4)
 * would return 16.
 */
#define OS_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup OsBasic
 *
 * @def OS_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. OS_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define OS_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#define OS_IS_ALIGN(size, align)        (!((size) & (align - 1)))

/**
 * @ingroup OsBasic
 *
 * @def OS_NULL
 * Similar as the \c NULL in C library.
 */
#define OS_NULL                         (0)

/**
 * @ingroup OsBasic
 *
 * @def BUILD_BUG_ON
 * Checks error condition when compiling.
 */
#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(cond)              ((void)sizeof(char[1-2*!!(cond)]))
#endif

/**
 * @ingroup OsBasic
 *
 * @def OS_UNREFERENCE
 * Unreference objects.
 */
#define OS_UNREFERENCE(x)               ((void)(x))

/**
 * Double List structure
 */
struct osListNode
{
    struct osListNode *next;                          /**< point to next node. */
    struct osListNode *prev;                          /**< point to prev node. */
};
typedef struct osListNode osList_t;                  /**< Type for lists. */

/**
 * Single List structure
 */
struct osSlistNode
{
    struct osSlistNode *next;                         /**< point to next node. */
};
typedef struct osSlistNode osSlist_t;                /**< Type for single list. */

/**
 * @addtogroup KernelObject
 */

/**@{*/

/*
 * kernel object macros
 */
#define OS_OBJECT_FLAG_MODULE           0x80            /**< is module object. */

/**
 * Base structure of Kernel object
 */
struct osObject
{
    uint8_t    type;                                    /**< type of kernel object */
    uint8_t    flag;                                    /**< flag of kernel object */
    uint16_t   attr;                                    /**< attr of kernel object */
};
typedef struct osObject *osObject_t;                  /**< Type for kernel objects. */

#define OSOBJ_ATTR(obj)                 (((osObject_t)obj)->attr)

/**
 *  The object type can be one of the follows with specific
 *  macros enabled:
 *  - Thread
 *  - Semaphore
 *  - Mutex
 *  - Event
 *  - MailBox
 *  - MessageQueue
 *  - MemHeap
 *  - MemPool
 *  - Device
 *  - Timer
 *  - Module
 *  - Unknown
 *  - Static
 */
enum osObjectClassType
{
    osObject_Class_Null          = 0x00,      /**< The object is not used. */
    osObject_Class_Thread        = 0x01,      /**< The object is a thread. */
    osObject_Class_Semaphore     = 0x02,      /**< The object is a semaphore. */
    osObject_Class_Mutex         = 0x03,      /**< The object is a mutex. */
    osObject_Class_Event         = 0x04,      /**< The object is a event. */
    osObject_Class_MailBox       = 0x05,      /**< The object is a mail box. */
    osObject_Class_MessageQueue  = 0x06,      /**< The object is a message queue. */
    osObject_Class_MemHeap       = 0x07,      /**< The object is a memory heap. */
    osObject_Class_MemPool       = 0x08,      /**< The object is a memory pool. */
    osObject_Class_Device        = 0x09,      /**< The object is a device. */
    osObject_Class_Timer         = 0x0a,      /**< The object is a timer. */
    osObject_Class_Module        = 0x0b,      /**< The object is a module. */
    osObject_Class_Unknown       = 0x0c,      /**< The object is unknown. */
    osObject_Class_Static        = 0x80       /**< The object is a static object. */
};

/**
 * The hook function call macro
 */
#ifdef OS_USING_HOOK
#define OS_OBJECT_HOOK_CALL(func, argv) \
    do { if ((func) != OS_NULL) func argv; } while (0)
#else
#define OS_OBJECT_HOOK_CALL(func, argv)
#endif

/**@}*/

/**
 * @addtogroup Clock
 */

/**@{*/

/**
 * clock & timer macros
 */

#define MSEC_PER_SEC    1000ul
#define USEC_PER_MSEC   1000ul
#define NSEC_PER_USEC   1000ul
#define NSEC_PER_MSEC   1000000ul
#define USEC_PER_SEC    1000000ul
#define NSEC_PER_SEC    1000000000ull

#define OS_TIMER_FLAG_DEACTIVATED       0x0             /**< timer is deactive */
#define OS_TIMER_FLAG_ACTIVATED         0x1             /**< timer is active */

#define OS_TIMER_FLAG_ONE_SHOT          0x0             /**< one shot timer */
#define OS_TIMER_FLAG_PERIODIC          0x2             /**< periodic timer */

#define OS_TIMER_FLAG_SOFT_TIMER        0x0             /**< soft timer,the timer's callback function will be called in timer thread. */
#define OS_TIMER_FLAG_HARD_TIMER        0x4             /**< hard timer,the timer's callback function will be called in tick isr. */
#define OS_TIMER_FLAG_HRTIMER           0x8             /**< hrtimer,the timer's callback function will be called in isr. */

#define OS_TIMER_FLAG_PS_TIMER          0x10            /**< PS timer. */

#define OS_TIMER_CTRL_SET_TIME          0x0             /**< set timer control command */
#define OS_TIMER_CTRL_GET_TIME          0x1             /**< get timer control command */
#define OS_TIMER_CTRL_SET_ONESHOT       0x2             /**< change timer to one shot */
#define OS_TIMER_CTRL_SET_PERIODIC      0x3             /**< change timer to periodic */
#define OS_TIMER_CTRL_GET_STATE         0x4             /**< get timer run state active or deactive*/
#define OS_TIMER_CTRL_SET_RELAXED       0x5             /**< set relaxed control command */
#define OS_TIMER_CTRL_SET_HRTIMER       0x6             /**< change timer to hrtimer */

#ifndef OS_TIMER_SKIP_LIST_LEVEL
#define OS_TIMER_SKIP_LIST_LEVEL          1
#endif

/* 1 or 3 */
#ifndef OS_TIMER_SKIP_LIST_MASK
#define OS_TIMER_SKIP_LIST_MASK         0x3
#endif

/**
 * timer structure
 */
struct osTimer
{
    struct osObject parent;                            /**< inherit from osObject */
    const char      *name;                         /**< name of kernel object */

    osList_t        row[OS_TIMER_SKIP_LIST_LEVEL];

    void (*timeout_func)(void *parameter);              /**< timeout function */
    void            *parameter;                         /**< timeout function's parameter */

    uint32_t        timeout;                         /**< timer timeout time, timer is tick, hrtimer is usec */
    uint32_t        relaxed;                         /**< timer relaxed time, timer is tick */
    uint64_t        timeout_time;                      /**< timeout time, timer is tick, hrtimer is usec */
};
typedef struct osTimer *osTimer_t;

/**@}*/

/**
 * @addtogroup Signal
 */
#ifdef OS_USING_SIGNALS
#include <libc/libc_signal.h>
typedef unsigned long osSigSet_t;
typedef void (*osSigHandler_t)(int signo);
typedef siginfo_t osSigInfo_t;

#define OS_SIG_MAX          32
#endif
/**@}*/

/**
 * @addtogroup Thread
 */

/**@{*/

/*
 * Thread
 */

/*
 * thread state definitions
 */
#define OS_THREAD_INIT                  0x00                /**< Initialized status */
#define OS_THREAD_READY                 0x01                /**< Ready status */
#define OS_THREAD_SUSPEND               0x02                /**< Suspend status */
#define OS_THREAD_RUNNING               0x03                /**< Running status */
#define OS_THREAD_BLOCK                 OS_THREAD_SUSPEND   /**< Blocked status */
#define OS_THREAD_CLOSE                 0x04                /**< Closed status */
#define OS_THREAD_STAT_MASK             0x0f

#define OS_THREAD_STAT_SIGNAL           0x10                /**< task hold signals */
#define OS_THREAD_STAT_SIGNAL_READY     (OS_THREAD_STAT_SIGNAL | OS_THREAD_READY)
#define OS_THREAD_STAT_SIGNAL_WAIT      0x20                /**< task is waiting for signals */
#define OS_THREAD_STAT_SIGNAL_PENDING   0x40                /**< signals is held and it has not been procressed */
#define OS_THREAD_STAT_SIGNAL_MASK      0xf0

/**
 * thread control command definitions
 */
#define OS_THREAD_CTRL_STARTUP          0x00                /**< Startup thread. */
#define OS_THREAD_CTRL_CLOSE            0x01                /**< Close thread. */
#define OS_THREAD_CTRL_CHANGE_PRIORITY  0x02                /**< Change thread priority. */
#define OS_THREAD_CTRL_INFO             0x03                /**< Get thread information. */

/**
 * Base structure of IPC object
 */
struct osIpcObject
{
    struct osObject parent;                            /**< inherit from osObject */

    osList_t        suspendThread;                    /**< threads pended on this resource */
};

#ifdef OS_USING_SEMAPHORE
/**
 * Semaphore structure
 */
struct osSemaphore
{
    struct osIpcObject parent;                        /**< inherit from ipc_object */
    const char         *name;                         /**< name of kernel object */
    uint16_t          value;                         /**< value of semaphore. */
    uint16_t          max_value;                         /**< max value of semaphore. */
};
typedef struct osSemaphore *osSem_t;
typedef struct osSemaphore  osCompletion;

#endif

#ifdef OS_USING_MAILBOX
/**
 * mailbox structure
 */
struct osMailbox
{
    struct osIpcObject parent;                        /**< inherit from ipc_object */

    ubase_t          *msg_pool;                      /**< start address of message buffer */

    uint16_t          size;                          /**< size of message pool */

    uint16_t          entry;                         /**< index of messages in msg_pool */
    uint16_t          max_entry;                     /**< max index of messages in msg_pool */
    uint16_t          urgent_entry;                  /**< index of urgent messages in msg_pool */
    uint16_t          in_offset;                     /**< input offset of the message buffer */
    uint16_t          out_offset;                    /**< output offset of the message buffer */

    osList_t            suspend_sender_thread;         /**< sender thread suspended on this mailbox */
};
typedef struct osMailbox *osMb_t;

#endif

typedef struct osCpuUsage
{
    uint32_t start_time; /* Record the time of switching to the thread.*/
    uint32_t total_time; /* Accumulation of running time. */
} osCpuUsage_t;

/**
 * Thread structure
 */
struct osThread
{
    /* os object */
    struct osObject parent;
    const char      *name;                      /**< name of thread*/

    osList_t        resource_node;

    struct osThread *join_thread;               /**< joined thread */

#ifdef _CPU_AP
    void       *module_id;                              /**< id of application module */
#endif
    osList_t        tlist;                      /**< the thread list */

    /* stack point and entry */
    void       *sp;                                     /**< stack point */
    void       *entry;                                  /**< entry */
    void       *parameter;                              /**< parameter */
    void       *stackAddr;                             /**< stack address */
    uint32_t    stackSize;                             /**< stack size */

    /* error code */
    int32_t     error;                                  /**< error code */

    uint32_t    wait_flags;
    uint32_t    thread_flags;
    uint8_t     flags_options;

    uint8_t     stat;                                   /**< thread status */

    /* priority */
    uint8_t  currentPriority;                       /**< current priority */
    uint8_t  initPriority;                          /**< initialized priority */
#if OS_THREAD_PRIORITY_MAX > 32
    uint8_t  number;
    uint8_t  highMask;
#endif
    uint32_t numberMask;

#if defined(OS_USING_EVENT)
    /* thread event */
    uint32_t eventSet;
    uint8_t  eventInfo;
#endif

#if defined(OS_USING_SIGNALS)
    osSigSet_t     sigPending;                        /**< the pending signals */
    osSigSet_t     sigMask;                           /**< the mask bits of signal */

#ifndef OS_USING_SMP
    void            *sigRet;                           /**< the return stack pointer from signal */
#endif
    osSigHandler_t  *sigVectors;                       /**< vectors of signal handler */
    void            *siList;                           /**< the signal infor list */
#endif

    ubase_t  initTick;                              /**< thread's initialized tick */
    ubase_t  remainingTick;                         /**< remaining tick */

#ifdef OS_USING_TIMER
    struct osTimer threadTimer;                       /**< built-in thread timer */
#endif

    void (*cleanup)(struct osThread *tid);             /**< cleanup function when thread exit */

    /* light weight process if present */
#ifdef OS_USING_LWP
    void        *lwp;
#endif

    ubase_t     userData;                             /**< private user data beyond this thread */
#ifdef OS_USING_CPU_MONITER
    osCpuUsage_t usageInfo;
#endif
#ifdef OSI_USING_PROFILE
    ubase_t  threadNumber;
#endif
};
typedef struct osThread *osThread_t;

#define OS_MSG_MAGIC    (0x3A3A3A3A)

struct osMsg
{
    uint32_t    id;
    osThread_t  sender;
    osThread_t  receiver;
    osTimer_t   timer;
    uint32_t    magic;
    void        *buf;
    size_t      size;
};
typedef struct osMsg *osMsg_t;

/**@}*/

/**
 * @addtogroup IPC
 */

/**@{*/

/**
 * IPC flags and control command definitions
 */
#define OS_IPC_FLAG_FIFO                0x00            /**< FIFOed IPC. @ref IPC. */
#define OS_IPC_FLAG_PRIO                0x01            /**< PRIOed IPC. @ref IPC. */

#define OS_IPC_CMD_UNKNOWN              0x00            /**< unknown IPC command */
#define OS_IPC_CMD_RESET                0x01            /**< reset IPC object */

#ifdef OS_USING_MUTEX
/**
 * Mutual exclusion (mutex) structure
 */
struct osMutex
{
    struct osIpcObject parent;                        /**< inherit from ipc_object */
    const char         *name;                         /**< name of kernel object */

    uint16_t          value;                         /**< value of mutex */

    uint8_t           originPriority;             /**< priority of last thread hold the mutex */
    uint8_t           hold;                          /**< numbers of thread hold the mutex */

    struct osThread    *owner;                         /**< current owner of mutex */
};
typedef struct osMutex *osMutex_t;
#endif

#ifdef OS_USING_EVENT
/**
 * flag defintions in event
 */
#define OS_EVENT_FLAG_AND               0x01            /**< logic and */
#define OS_EVENT_FLAG_OR                0x02            /**< logic or */
#define OS_EVENT_FLAG_CLEAR             0x04            /**< clear flag */

/*
 * event structure
 */
struct osEvent
{
    struct osIpcObject parent;                        /**< inherit from ipc_object */
    const char         *name;                         /**< name of kernel object */

    uint32_t           set;                           /**< event set */
};
typedef struct osEvent *osEvent_t;
#endif

#ifdef OS_USING_MESSAGEQUEUE
/**
 * message queue structure
 */
struct osMessageQueue
{
    struct osMailbox    parent;                     /**< inherit from osMailbox */
    const char         *name;                       /**< name of kernel object */
    size_t              msg_size;                   /**< message size of each message */
};
typedef struct osMessageQueue *osMq_t;
#endif

/**@}*/

/**
 * @addtogroup MM
 */

/**@{*/

#ifdef OS_USING_MEMPOOL
/**
 * Base structure of Memory pool object
 */
struct osMempool
{
    struct osIpcObject parent;                      /**< inherit from osObject */
    const char      *name;                         /**< name of kernel object */

    void            *start_address;                     /**< memory pool start */
    size_t        size;                              /**< size of memory pool */

    size_t        block_size;                        /**< size of memory blocks */
    uint8_t      *block_list;                        /**< memory blocks list */

    size_t        block_total_count;                 /**< numbers of memory block */
    size_t        block_free_count;                  /**< numbers of free memory block */
};
typedef struct osMempool *osMempool_t;

#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/* RT-Thread definitions for C++ */
namespace rtthread {

enum TICK_WAIT {
    WAIT_NONE = 0,
    WAIT_FOREVER = -1,
};

}

#endif /* end of __cplusplus */

#endif
