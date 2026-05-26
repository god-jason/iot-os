/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 南京创芯慧联技术有限公司
 *            保留所有权利。
 *
 * @file      cmsis_os2.h
 *
 * @brief     cmsis os2头文件.
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

#ifndef CMSIS_OS2_H_
#define CMSIS_OS2_H_

#ifndef __NO_RETURN
#if   defined(__CC_ARM)
#define __NO_RETURN __declspec(noreturn)
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define __NO_RETURN __attribute__((__noreturn__))
#elif defined(__GNUC__)
#define __NO_RETURN __attribute__((__noreturn__))
#elif defined(__ICCARM__)
#define __NO_RETURN __noreturn
#else
#define __NO_RETURN
#endif
#endif

#include <stdint.h>
#include <stddef.h>

#ifdef  __cplusplus
extern "C"
{
#endif


/**
 * @addtogroup Cmsis
 */

/**@{*/

/**
 * @brief 枚举类型、结构体类型、宏定义
 */
//  ==== Enumerations, structures, defines ====

/**
 * @brief 版本信息
 */
/// Version information.
typedef struct {
  uint32_t                       api;   ///< API version (major.minor.rev: mmnnnrrrr dec).
  uint32_t                    kernel;   ///< Kernel version (major.minor.rev: mmnnnrrrr dec).
} osVersion_t;

/// Kernel state.
/**
 * @brief 内核状态
 */
typedef enum {
  osKernelInactive        =  0,         ///< Inactive.
  osKernelReady           =  1,         ///< Ready.
  osKernelRunning         =  2,         ///< Running.
  osKernelLocked          =  3,         ///< Locked.
  osKernelSuspended       =  4,         ///< Suspended.
  osKernelError           = -1,         ///< Error.
  osKernelReserved        = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
} osKernelState_t;

/// Thread state.
/**
 * @brief 线程状态
 */
typedef enum {
  osThreadInactive        =  0,         ///< Inactive.
  osThreadReady           =  1,         ///< Ready.
  osThreadRunning         =  2,         ///< Running.
  osThreadBlocked         =  3,         ///< Blocked.
  osThreadTerminated      =  4,         ///< Terminated.
  osThreadError           = -1,         ///< Error.
  osThreadReserved        = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
} osThreadState_t;

/// Priority values.
/**
 * @brief 线程优先级
 */
typedef enum {
  osPriorityNone          =  0,         ///< No priority (not initialized).
  osPriorityIdle          =  1,         ///< Reserved for Idle thread.
  osPriorityLow           =  8,         ///< Priority: low
  osPriorityLow1          =  8+1,       ///< Priority: low + 1
  osPriorityLow2          =  8+2,       ///< Priority: low + 2
  osPriorityLow3          =  8+3,       ///< Priority: low + 3
  osPriorityBelowNormal   = 12,         ///< Priority: below normal
  osPriorityBelowNormal1  = 12+1,       ///< Priority: below normal + 1
  osPriorityBelowNormal2  = 12+2,       ///< Priority: below normal + 2
  osPriorityBelowNormal3  = 12+3,       ///< Priority: below normal + 3
  osPriorityNormal        = 16,         ///< Priority: normal
  osPriorityNormal1       = 16+1,       ///< Priority: normal + 1
  osPriorityNormal2       = 16+2,       ///< Priority: normal + 2
  osPriorityNormal3       = 16+3,       ///< Priority: normal + 3
  osPriorityAboveNormal   = 20,         ///< Priority: above normal
  osPriorityAboveNormal1  = 20+1,       ///< Priority: above normal + 1
  osPriorityAboveNormal2  = 20+2,       ///< Priority: above normal + 2
  osPriorityAboveNormal3  = 20+3,       ///< Priority: above normal + 3
  osPriorityHigh          = 24,         ///< Priority: high
  osPriorityHigh1         = 24+1,       ///< Priority: high + 1
  osPriorityHigh2         = 24+2,       ///< Priority: high + 2
  osPriorityHigh3         = 24+3,       ///< Priority: high + 3
  osPriorityRealtime      = 28,         ///< Priority: realtime
  osPriorityRealtime1     = 28+1,       ///< Priority: realtime + 1
  osPriorityRealtime2     = 28+2,       ///< Priority: realtime + 2
  osPriorityRealtime3     = 28+3,       ///< Priority: realtime + 3
  osPriorityISR           = 32,         ///< Reserved for ISR deferred thread.
  osPriorityError         = -1,         ///< System cannot determine priority or illegal priority.
  osPriorityReserved      = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
} osPriority_t;

/// Entry point of a thread.
/**
 * @brief 线程入口函数
 */
typedef void (*osThreadFunc_t) (void *argument);

/// Timer callback function.
/**
 * @brief 定时器回调函数
 */
typedef void (*osTimerFunc_t) (void *argument);

/// Timer type.
/**
 * @brief 定时器类型
 */
typedef enum {
  osTimerOnce               = 0,          ///< One-shot timer.
  osTimerPeriodic           = 1           ///< Repeating timer.
} osTimerType_t;

// Timeout value.
/**
 * @brief 超时值
 */
#define osWaitForever         0xFFFFFFFFU ///< Wait forever timeout value.
#define osNoWait              0           ///< No wait timeout value.

// Flags options (\ref osThreadFlagsWait and \ref osEventFlagsWait).
/**
 * @brief 标志选项
 * @note  osThreadFlagsWait和osEventFlagsWait标志.
 */
#define osFlagsWaitAny        0x00000000U ///< Wait for any flag (default).
#define osFlagsWaitAll        0x00000001U ///< Wait for all flags.
#define osFlagsNoClear        0x00000002U ///< Do not clear flags which have been specified to wait for.

// Flags errors (returned by osThreadFlagsXxxx and osEventFlagsXxxx).
/**
 * @brief 标志错误
 * @note  osThreadFlagsXxxx和osEventFlagsXxxx标志错误.
 */
#define osFlagsError          0x80000000U ///< Error indicator.
#define osFlagsErrorUnknown   0xFFFFFFFFU ///< osError (-1).
#define osFlagsErrorTimeout   0xFFFFFFFEU ///< osErrorTimeout (-2).
#define osFlagsErrorResource  0xFFFFFFFDU ///< osErrorResource (-3).
#define osFlagsErrorParameter 0xFFFFFFFCU ///< osErrorParameter (-4).
#define osFlagsErrorISR       0xFFFFFFFAU ///< osErrorISR (-6).

// Thread attributes (attr_bits in \ref osThreadAttr_t).
/**
 * @brief 线程属性
 * @note  osThreadAttr_t里面的attr_bits.
 */
#define osThreadDetached      0x00000000U ///< Thread created in detached mode (default)
#define osThreadJoinable      0x00000001U ///< Thread created in joinable mode
#define osThreadAutoStart     0x00000000U
#define osThreadManuStart     0x00000002U
#define osThreadAttrMask      0x000000FFU

// Mutex attributes (attr_bits in \ref osMutexAttr_t).
/**
 * @brief 互斥体属性
 * @note  osMutexAttr_t里面的attr_bits.
 */
#define osMutexRecursive      0x00000001U ///< Recursive mutex.
#define osMutexPrioInherit    0x00000002U ///< Priority inherit protocol.
#define osMutexRobust         0x00000008U ///< Robust mutex.

/// Status code values returned by CMSIS-RTOS functions.
/**
 * @brief CMSIS-RTOS函数返回的状态码
 */
typedef enum {
  osOK                      =  0,         ///< Operation completed successfully.
  osError                   = -1,         ///< Unspecified RTOS error: run-time error but no other error message fits.
  osErrorTimeout            = -2,         ///< Operation not completed within the timeout period.
  osErrorResource           = -3,         ///< Resource not available.
  osErrorParameter          = -4,         ///< Parameter error.
  osErrorNoMemory           = -5,         ///< System is out of memory: it was impossible to allocate or reserve memory for the operation.
  osErrorISR                = -6,         ///< Not allowed in ISR context: the function cannot be called from interrupt service routines.
  osErrorResourceFull       = -7,         /**< The resource is full */
  osErrorBusy               = -9,         /**< Busy */
  osErrorNoSys              = -8,         /**< No system */
  osErrorIO                 = -10,        /**< IO error */
  osErrorIntr               = -11,        /**< Interrupted system call */
  osStatusReserved          = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
} osStatus_t;


/// \details Thread ID identifies the thread.
/**
 * @brief 线程id
 * @note  标识线程.
 */
typedef void *osThreadId_t;

/// \details Timer ID identifies the timer.
/**
 * @brief 定时器id
 * @note  标识定时器.
 */
typedef void *osTimerId_t;

/// \details Event Flags ID identifies the event flags.
/**
 * @brief 事件id
 * @note  标识事件.
 */
typedef void *osEventFlagsId_t;

/// \details Mutex ID identifies the mutex.
/**
 * @brief 互斥体id
 * @note  标识互斥体.
 */
typedef void *osMutexId_t;

/// \details Semaphore ID identifies the semaphore.
/**
 * @brief 信号量id
 * @note  标识信号量.
 */
typedef void *osSemaphoreId_t;

/// \details Memory Pool ID identifies the memory pool.
/**
 * @brief 内存池id
 * @note  标识内存池.
 */
typedef void *osMemoryPoolId_t;

/// \details Message Queue ID identifies the message queue.
/**
 * @brief 消息队列id
 * @note  标识消息队列.
 */
typedef void *osMessageQueueId_t;


#ifndef TZ_MODULEID_T
#define TZ_MODULEID_T
/// \details Data type that identifies secure software modules called by a process.
typedef uint32_t TZ_ModuleId_t;
#endif


/// Attributes structure for thread.
/**
 * @brief 线程属性结构体
 */
typedef struct {
  const char                   *name;   ///< 线程名称
  uint32_t                 attr_bits;   ///< 线程属性，osThreadDetached(默认)或osThreadJoinable
  void                      *cb_mem;    ///< 线程结构体指针
  uint32_t                   cb_size;   ///< 线程结构体大小
  void                   *stack_mem;    ///< 线程栈起始地址
  uint32_t                stack_size;   ///< 线程栈大小
  osPriority_t              priority;   ///< 线程初始优先级
  TZ_ModuleId_t            tz_module;   ///< TrustZone模块标识符，暂时不用
  uint32_t                  reserved;   ///< 保留字段(必须为0)
} osThreadAttr_t;

/// Attributes structure for timer.
/**
 * @brief 线程属性
 * @note  osThreadAttr_t里面的attr_bits.
 */
#define osTimerFreeParameter    0x00000001U     ///< Frees timeout function's parameter when deleting timer
#define osTimerHardTimer        0x00000002U     ///< Hard timer, default Soft timer
#define osTimerHrtimer          0x00000004U     ///< Hrtimer, default Soft timer
#define osTimerPsmWakeup        0x00000010U     ///< Psm wakeup, default psm wakeup
#define osTimerAttrMask         0x000000FFU

/**
 * @brief 定时器属性结构体
 */
typedef struct {
  const char                   *name;   ///< 定时器名称
  uint32_t                 attr_bits;   ///< 定时器属性
  void                      *cb_mem;    ///< 定时器结构体指针
  uint32_t                   cb_size;   ///< 定时器结构体大小
} osTimerAttr_t;

/// Attributes structure for event flags.
/**
 * @brief 事件标志属性结构体
 */
typedef struct {
  const char                   *name;   ///< name of the event flags
  uint32_t                 attr_bits;   ///< attribute bits
  void                      *cb_mem;    ///< memory for control block
  uint32_t                   cb_size;   ///< size of provided memory for control block
} osEventFlagsAttr_t;

/// Attributes structure for mutex.
/**
 * @brief 互斥体属性结构体
 */
typedef struct {
  const char                   *name;   ///< 互斥体名称
  uint32_t                 attr_bits;   ///< 互斥体属性，固定为osMutexRecursive | osMutexPrioInherit
  void                      *cb_mem;    ///< 互斥体结构体指针
  uint32_t                   cb_size;   ///< 互斥体结构体大小
} osMutexAttr_t;

/// Attributes structure for semaphore.
/**
 * @brief 信号量属性结构体
 */
typedef struct {
  const char                   *name;   ///< 信号量名称
  uint32_t                 attr_bits;   ///< 信号量属性，暂时不用，为0
  void                      *cb_mem;    ///< 信号量结构体指针
  uint32_t                   cb_size;   ///< 信号量结构体大小
} osSemaphoreAttr_t;

/// Attributes structure for memory pool.
/**
 * @brief 内存池属性结构体
 */
typedef struct {
  const char                   *name;   ///< name of the memory pool
  uint32_t                 attr_bits;   ///< attribute bits
  void                      *cb_mem;    ///< memory for control block
  uint32_t                   cb_size;   ///< size of provided memory for control block
  void                      *mp_mem;    ///< memory for data storage
  uint32_t                   mp_size;   ///< size of provided memory for data storage
} osMemoryPoolAttr_t;

/// Attributes structure for message queue.
/**
 * @brief 消息队列属性结构体
 */
typedef struct {
  const char                   *name;   ///< 消息队列名称
  uint32_t                 attr_bits;   ///< 消息队列属性，暂时不用，为0
  void                      *cb_mem;    ///< 消息队列结构体
  uint32_t                   cb_size;   ///< 消息队列结构体大小
  void                      *mq_mem;    ///< 消息队列空间指针
  uint32_t                   mq_size;   ///< 消息队列空间总大小
} osMessageQueueAttr_t;


//  ==== Kernel Management Functions ====

/// Initialize the RTOS Kernel.
/// \return status code that indicates the execution status of the function.
osStatus_t osKernelInitialize (void);

///  Get RTOS Kernel Information.
/// \param[out]    version       pointer to buffer for retrieving version information.
/// \param[out]    id_buf        pointer to buffer for retrieving kernel identification string.
/// \param[in]     id_size       size of buffer for kernel identification string.
/// \return status code that indicates the execution status of the function.
osStatus_t osKernelGetInfo (osVersion_t *version, char *id_buf, uint32_t id_size);

/// Get the current RTOS Kernel state.
/// \return current RTOS Kernel state.
osKernelState_t osKernelGetState (void);

/// Start the RTOS Kernel scheduler.
/// \return status code that indicates the execution status of the function.
osStatus_t osKernelStart (void);

/// Lock the RTOS Kernel scheduler.
/// \return previous lock state (1 - locked, 0 - not locked, error code if negative).
int32_t osKernelLock (void);

/// Unlock the RTOS Kernel scheduler.
/// \return previous lock state (1 - locked, 0 - not locked, error code if negative).
int32_t osKernelUnlock (void);

/// Restore the RTOS Kernel scheduler lock state.
/// \param[in]     lock          lock state obtained by \ref osKernelLock or \ref osKernelUnlock.
/// \return new lock state (1 - locked, 0 - not locked, error code if negative).
int32_t osKernelRestoreLock (int32_t lock);

/// Suspend the RTOS Kernel scheduler.
/// \return time in ticks, for how long the system can sleep or power-down.
uint32_t osKernelSuspend (void);

/// Resume the RTOS Kernel scheduler.
/// \param[in]     sleep_ticks   time in ticks for how long the system was in sleep or power-down mode.
void osKernelResume (uint32_t sleep_ticks);

/**
 ************************************************************************************
 * @brief           获取系统当前tick数
 *
 * @param[in]       void
 *
 * @return          系统当前tick数.
 ************************************************************************************
 */
/// Get the RTOS kernel tick count.
/// \return RTOS kernel current tick count.
uint32_t osKernelGetTickCount (void);

/// Get the RTOS kernel tick frequency.
/// \return frequency of the kernel tick in hertz, i.e. kernel ticks per second.
uint32_t osKernelGetTickFreq (void);

/// Get the RTOS kernel system timer count.
/// \return RTOS kernel current system timer count as 32-bit value.
uint32_t osKernelGetSysTimerCount (void);

/// Get the RTOS kernel system timer frequency.
/// \return frequency of the system timer in hertz, i.e. timer ticks per second.
uint32_t osKernelGetSysTimerFreq (void);


//  ==== Thread Management Functions ====

/// Create a thread and add it to Active Threads.
/// \param[in]     func          thread function.
/// \param[in]     argument      pointer that is passed to the thread function as start argument.
/// \param[in]     attr          thread attributes; NULL: default values.
/// \return thread ID for reference by other functions or NULL in case of error.
/**
 ************************************************************************************
 * @brief           创建线程并启动线程
 *
 * @param[in]       func            线程入口函数
 * @param[in]       argument        线程入口函数参数
 * @param[in]       attr            线程属性指针，为NULL时使用默认值
 *
 * @return          线程id或NULL.
 * @retval          线程id            线程创建成功
 *                  NULL            线程创建失败
 ************************************************************************************
 */
osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr);

/// Get name of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return name as null-terminated string.
/**
 ************************************************************************************
 * @brief           根据线程id获取线程名称
 *
 * @param[in]       thread_id       线程id，通过osThreadNew或者osThreadGetId获取
 *
 * @return          线程名称.
 ************************************************************************************
 */
const char *osThreadGetName (osThreadId_t thread_id);

/// Return the thread ID of the current running thread.
/// \return thread ID for reference by other functions or NULL in case of error.
/**
 ************************************************************************************
 * @brief           获取当前运行线程的id
 *
 * @param[in]       void
 *
 * @return          线程id或NULL.
 * @retval          线程id            获取成功
 *                  NULL            获取失败
 ************************************************************************************
 */
osThreadId_t osThreadGetId (void);

/// Get current thread state of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return current thread state of the specified thread.
/**
 ************************************************************************************
 * @brief           获取线程的当前状态
 *
 * @param[in]       thread_id       线程id，通过osThreadNew或者osThreadGetId获取
 *
 * @return          线程状态.
 ************************************************************************************
 */
osThreadState_t osThreadGetState (osThreadId_t thread_id);

/// Get stack size of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return stack size in bytes.
uint32_t osThreadGetStackSize (osThreadId_t thread_id);

/// Get available stack space of a thread based on stack watermark recording during execution.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return remaining stack space in bytes.
uint32_t osThreadGetStackSpace (osThreadId_t thread_id);

/// Change priority of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \param[in]     priority      new priority value for the thread function.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           设置线程的优先级
 *
 * @param[in]       thread_id       线程id，通过osThreadNew或者osThreadGetId获取
 * @param[in]       priorty         优先级
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority);

/// Get current priority of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return current priority value of the specified thread.
/**
 ************************************************************************************
 * @brief           获取线程的当前优先级
 *
 * @param[in]       thread_id       线程id，通过osThreadNew或者osThreadGetId获取
 *
 * @return          线程的当前优先级.
 ************************************************************************************
 */
osPriority_t osThreadGetPriority (osThreadId_t thread_id);

/// Pass control to next thread that is in state \b READY.
/// \return status code that indicates the execution status of the function.
osStatus_t osThreadYield (void);

/// Suspend execution of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           挂起线程的运行
 *
 * @param[in]       thread_id       线程id，通过osThreadNew或者osThreadGetId获取
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osThreadSuspend (osThreadId_t thread_id);

/// Resume execution of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           恢复线程的运行
 *
 * @param[in]       thread_id       线程id，通过osThreadNew或者osThreadGetId获取
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osThreadResume (osThreadId_t thread_id);

/// Detach a thread (thread storage can be reclaimed when thread terminates).
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return status code that indicates the execution status of the function.
osStatus_t osThreadDetach (osThreadId_t thread_id);

/// Wait for specified thread to terminate.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return status code that indicates the execution status of the function.
osStatus_t osThreadJoin (osThreadId_t thread_id);

/// Terminate execution of current running thread.
/**
 ************************************************************************************
 * @brief           当前运行线程退出，回收资源
 *
 * @param[in]       void
 *
 * @return          void.
 ************************************************************************************
 */
__NO_RETURN void osThreadExit (void);

/// Terminate execution of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           终止线程的执行，回收资源
 *
 * @param[in]       thread_id   线程id，通过osThreadNew或者osThreadGetId获取
 *
 * @return          void.
 ************************************************************************************
 */
osStatus_t osThreadTerminate (osThreadId_t thread_id);

/// Get number of active threads.
/// \return number of active threads.
uint32_t osThreadGetCount (void);

/// Enumerate active threads.
/// \param[out]    thread_array  pointer to array for retrieving thread IDs.
/// \param[in]     array_items   maximum number of items in array for retrieving thread IDs.
/// \return number of enumerated threads.
uint32_t osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items);


//  ==== Thread Flags Functions ====

/// Set the specified Thread Flags of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \param[in]     flags         specifies the flags of the thread that shall be set.
/// \return thread flags after setting or error code if highest bit set.
uint32_t osThreadFlagsSet (osThreadId_t thread_id, uint32_t flags);

/// Clear the specified Thread Flags of current running thread.
/// \param[in]     flags         specifies the flags of the thread that shall be cleared.
/// \return thread flags before clearing or error code if highest bit set.
uint32_t osThreadFlagsClear (uint32_t flags);

/// Get the current Thread Flags of current running thread.
/// \return current thread flags.
uint32_t osThreadFlagsGet (void);

/// Wait for one or more Thread Flags of the current running thread to become signaled.
/// \param[in]     flags         specifies the flags to wait for.
/// \param[in]     options       specifies flags options (osFlagsXxxx).
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return thread flags before clearing or error code if highest bit set.
uint32_t osThreadFlagsWait (uint32_t flags, uint32_t options, uint32_t timeout);


//  ==== Generic Wait Functions ====

/// Wait for Timeout (Time Delay).
/// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           当前运行线程睡眠，让出cpu
 *
 * @param[in]       ticks       睡眠时长，单位：tick
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osDelay (uint32_t ticks);

/// Wait until specified time.
/// \param[in]     ticks         absolute time in ticks
/// \return status code that indicates the execution status of the function.
osStatus_t osDelayUntil (uint32_t ticks);


//  ==== Timer Management Functions ====

/// Create and Initialize a timer.
/// \param[in]     func          function pointer to callback function.
/// \param[in]     type          \ref osTimerOnce for one-shot or \ref osTimerPeriodic for periodic behavior.
/// \param[in]     argument      argument to the timer callback function.
/// \param[in]     attr          timer attributes; NULL: default values.
/// \return timer ID for reference by other functions or NULL in case of error.
/**
 ************************************************************************************
 * @brief           创建并初始化定时器
 *
 * @param[in]       func       定时器回调函数指针
 * @param[in]       type       单次定时器：osTimerOnce，循环定时器：osTimerPeriodic
 * @param[in]       argument   回调函数入参
 * @param[in]       attr       定时器属性，为NULL时使用默认值
 *
 * @return          定时器id或NULL.
 * @retval          定时器id      创建成功
 *                  NULL       创建失败
 ************************************************************************************
 */
osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr);

/// Get name of a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return name as null-terminated string.
const char *osTimerGetName (osTimerId_t timer_id);

/// Start or restart a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value of the timer.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           启动或者重启定时器，支持睡眠唤醒
 *
 * @param[in]       timer_id        定时器id，通过osTimerNew获取
 * @param[in]       time            定时器时长，hrtimer单位：us，hardtimer和softtimer单位：tick
                                    睡眠唤醒时间精度：ms
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osTimerStart (osTimerId_t timer_id, uint32_t time);

/// Start or restart a timer with relaxed timeout.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value of the timer.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           使用relaxed方式启动或者重启定时器，单位tick
 *
 * @param[in]       timer_id        定时器id，通过osTimerNew获取
 * @param[in]       time            定时器时长，单位：tick
 * @param[in]       relaxed_time    relaxed时长，hrtimer单位：us，hardtimer和softtimer单位：tick
                                    relaxed_time等于osWaitForever，不支持睡眠唤醒
                                    relaxed_time等于0，支持睡眠唤醒，等同于osTimerStart
                                    relaxed_time等于其他值，time + relaxed_time时长后唤醒
                                    睡眠唤醒时间精度：ms
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osTimerStartRelaxed(osTimerId_t timer_id, uint32_t time, uint32_t relaxed_time);

/**
 ************************************************************************************
 * @brief           修改定时器的relaxed time，单位tick
 *
 * @param[in]       timer_id        定时器id，通过osTimerNew获取
 * @param[in]       relaxed_time    relaxed时长，hrtimer单位：us，hardtimer和softtimer单位：tick
                                    relaxed_time等于osWaitForever，不支持睡眠唤醒
                                    relaxed_time等于0，支持睡眠唤醒，等同于osTimerStart
                                    relaxed_time等于其他值，time + relaxed_time时长后唤醒
                                    睡眠唤醒时间精度：ms
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osTimerChangeRelaxed(osTimerId_t timer_id, uint32_t relaxed_time);

/// Stop a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           停止定时器
 *
 * @param[in]       timer_id        定时器id，通过osTimerNew获取
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osTimerStop (osTimerId_t timer_id);

/// Check if a timer is running.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return 0 not running, 1 running.
uint32_t osTimerIsRunning (osTimerId_t timer_id);

/// Delete a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           删除定时器，释放资源
 *
 * @param[in]       timer_id        定时器id，通过osTimerNew获取
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osTimerDelete (osTimerId_t timer_id);


//  ==== Event Flags Management Functions ====

/// Create and Initialize an Event Flags object.
/// \param[in]     attr          event flags attributes; NULL: default values.
/// \return event flags ID for reference by other functions or NULL in case of error.
osEventFlagsId_t osEventFlagsNew (const osEventFlagsAttr_t *attr);

/// Get name of an Event Flags object.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \return name as null-terminated string.
const char *osEventFlagsGetName (osEventFlagsId_t ef_id);

/// Set the specified Event Flags.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \param[in]     flags         specifies the flags that shall be set.
/// \return event flags after setting or error code if highest bit set.
uint32_t osEventFlagsSet (osEventFlagsId_t ef_id, uint32_t flags);

/// Clear the specified Event Flags.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \param[in]     flags         specifies the flags that shall be cleared.
/// \return event flags before clearing or error code if highest bit set.
uint32_t osEventFlagsClear (osEventFlagsId_t ef_id, uint32_t flags);

/// Get the current Event Flags.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \return current event flags.
uint32_t osEventFlagsGet (osEventFlagsId_t ef_id);

/// Wait for one or more Event Flags to become signaled.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \param[in]     flags         specifies the flags to wait for.
/// \param[in]     options       specifies flags options (osFlagsXxxx).
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return event flags before clearing or error code if highest bit set.
uint32_t osEventFlagsWait (osEventFlagsId_t ef_id, uint32_t flags, uint32_t options, uint32_t timeout);

/// Delete an Event Flags object.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \return status code that indicates the execution status of the function.
osStatus_t osEventFlagsDelete (osEventFlagsId_t ef_id);


//  ==== Mutex Management Functions ====

/// Create and Initialize a Mutex object.
/// \param[in]     attr          mutex attributes; NULL: default values.
/// \return mutex ID for reference by other functions or NULL in case of error.
/**
 ************************************************************************************
 * @brief           创建并初始化互斥体
 *
 * @param[in]       attr       互斥体属性，为NULL时使用默认值
 *
 * @return          互斥体id或NULL.
 * @retval          互斥体id      创建成功
 *                  NULL       创建失败
 ************************************************************************************
 */
osMutexId_t osMutexNew (const osMutexAttr_t *attr);

/// Get name of a Mutex object.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexNew.
/// \return name as null-terminated string.
const char *osMutexGetName (osMutexId_t mutex_id);

/// Acquire a Mutex or timeout if it is locked.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexNew.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           获取互斥体
 *
 * @param[in]       mutex_id        互斥体id，通过osMutexNew获取
 * @param[in]       timeout         超时时长
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osMutexAcquire (osMutexId_t mutex_id, uint32_t timeout);

/// Release a Mutex that was acquired by \ref osMutexAcquire.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexNew.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           释放互斥体，只能由获取该互斥体的线程释放
 *
 * @param[in]       mutex_id        互斥体id，通过osMutexNew获取
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osMutexRelease (osMutexId_t mutex_id);

/// Get Thread which owns a Mutex object.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexNew.
/// \return thread ID of owner thread or NULL when mutex was not acquired.
/**
 ************************************************************************************
 * @brief           获取互斥体拥有者
 *
 * @param[in]       mutex_id    互斥体id，通过osMutexNew获取
 *
 * @return          线程id或NULL.
 * @retval          线程id        互斥体拥有者
 *                  NULL        互斥体未被获取
 ************************************************************************************
 */
osThreadId_t osMutexGetOwner (osMutexId_t mutex_id);

/// Delete a Mutex object.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexNew.
/// \return status code that indicates the execution status of the function.
/**
 ************************************************************************************
 * @brief           删除互斥体，释放资源
 *
 * @param[in]       mutex_id    互斥体id，通过osMutexNew获取
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osMutexDelete (osMutexId_t mutex_id);


//  ==== Semaphore Management Functions ====

/**
 ************************************************************************************
 * @brief           创建并初始化信号量
 *
 * @param[in]       max_count       信号量最大计数值
 * @param[in]       initial_count   信号量初始计数值
 * @param[in]       attr            信号量属性，为NULL时使用默认值
 *
 * @return          信号量id或NULL.
 * @retval          信号量id      创建成功
 *                  NULL       创建失败
 ************************************************************************************
 */
/// Create and Initialize a Semaphore object.
/// \param[in]     max_count     maximum number of available tokens.
/// \param[in]     initial_count initial number of available tokens.
/// \param[in]     attr          semaphore attributes; NULL: default values.
/// \return semaphore ID for reference by other functions or NULL in case of error.
osSemaphoreId_t osSemaphoreNew (uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr);

/// Get name of a Semaphore object.
/// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
/// \return name as null-terminated string.
const char *osSemaphoreGetName (osSemaphoreId_t semaphore_id);

/**
 ************************************************************************************
 * @brief           获取信号量
 *
 * @param[in]       semaphore_id    信号量id，通过osSemaphoreNew获取
 * @param[in]       timeout         超时时长
 *
 * @return          状态码.
 ************************************************************************************
 */
/// Acquire a Semaphore token or timeout if no tokens are available.
/// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
osStatus_t osSemaphoreAcquire (osSemaphoreId_t semaphore_id, uint32_t timeout);
osStatus_t osSemaphoreAcquireRelaxed(osSemaphoreId_t semaphore_id, uint32_t timeout, uint32_t relaxed_timeout);

/**
 ************************************************************************************
 * @brief           释放信号量，计数值增加，不能超过最大计数值
 *
 * @param[in]       semaphore_id    信号量id，通过osSemaphoreNew获取
 *
 * @return          状态码.
 ************************************************************************************
 */
/// Release a Semaphore token up to the initial maximum count.
/// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
/// \return status code that indicates the execution status of the function.
osStatus_t osSemaphoreRelease (osSemaphoreId_t semaphore_id);

/**
 ************************************************************************************
 * @brief           获取信号量当前计数值
 *
 * @param[in]       semaphore_id    信号量id，通过osSemaphoreNew获取
 *
 * @return          信号量当前计数值.
 ************************************************************************************
 */
/// Get current Semaphore token count.
/// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
/// \return number of tokens available.
uint32_t osSemaphoreGetCount (osSemaphoreId_t semaphore_id);

/**
 ************************************************************************************
 * @brief           删除信号量，释放资源
 *
 * @param[in]       semaphore_id    信号量id，通过osSemaphoreNew获取
 *
 * @return          状态码.
 ************************************************************************************
 */
/// Delete a Semaphore object.
/// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
/// \return status code that indicates the execution status of the function.
osStatus_t osSemaphoreDelete (osSemaphoreId_t semaphore_id);


//  ==== Memory Pool Management Functions ====

/// Create and Initialize a Memory Pool object.
/// \param[in]     block_count   maximum number of memory blocks in memory pool.
/// \param[in]     block_size    memory block size in bytes.
/// \param[in]     attr          memory pool attributes; NULL: default values.
/// \return memory pool ID for reference by other functions or NULL in case of error.
osMemoryPoolId_t osMemoryPoolNew (uint32_t block_count, uint32_t block_size, const osMemoryPoolAttr_t *attr);

/// Get name of a Memory Pool object.
/// \param[in]     mp_id         memory pool ID obtained by \ref osMemoryPoolNew.
/// \return name as null-terminated string.
const char *osMemoryPoolGetName (osMemoryPoolId_t mp_id);

/// Allocate a memory block from a Memory Pool.
/// \param[in]     mp_id         memory pool ID obtained by \ref osMemoryPoolNew.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return address of the allocated memory block or NULL in case of no memory is available.
void *osMemoryPoolAlloc (osMemoryPoolId_t mp_id, uint32_t timeout);

/// Return an allocated memory block back to a Memory Pool.
/// \param[in]     mp_id         memory pool ID obtained by \ref osMemoryPoolNew.
/// \param[in]     block         address of the allocated memory block to be returned to the memory pool.
/// \return status code that indicates the execution status of the function.
osStatus_t osMemoryPoolFree (osMemoryPoolId_t mp_id, void *block);

/// Get maximum number of memory blocks in a Memory Pool.
/// \param[in]     mp_id         memory pool ID obtained by \ref osMemoryPoolNew.
/// \return maximum number of memory blocks.
uint32_t osMemoryPoolGetCapacity (osMemoryPoolId_t mp_id);

/// Get memory block size in a Memory Pool.
/// \param[in]     mp_id         memory pool ID obtained by \ref osMemoryPoolNew.
/// \return memory block size in bytes.
uint32_t osMemoryPoolGetBlockSize (osMemoryPoolId_t mp_id);

/// Get number of memory blocks used in a Memory Pool.
/// \param[in]     mp_id         memory pool ID obtained by \ref osMemoryPoolNew.
/// \return number of memory blocks used.
uint32_t osMemoryPoolGetCount (osMemoryPoolId_t mp_id);

/// Get number of memory blocks available in a Memory Pool.
/// \param[in]     mp_id         memory pool ID obtained by \ref osMemoryPoolNew.
/// \return number of memory blocks available.
uint32_t osMemoryPoolGetSpace (osMemoryPoolId_t mp_id);

/// Delete a Memory Pool object.
/// \param[in]     mp_id         memory pool ID obtained by \ref osMemoryPoolNew.
/// \return status code that indicates the execution status of the function.
osStatus_t osMemoryPoolDelete (osMemoryPoolId_t mp_id);


//  ==== Message Queue Management Functions ====

/**
 ************************************************************************************
 * @brief           创建并初始化消息队列
 *
 * @param[in]       msg_count       最大消息数
 * @param[in]       msg_size        最大消息大小
 * @param[in]       attr            消息队列属性，为NULL时使用默认值
 *
 * @return          消息队列id或NULL.
 * @retval          消息队列id          创建成功
 *                  NULL            创建失败
 ************************************************************************************
 */
/// Create and Initialize a Message Queue object.
/// \param[in]     msg_count     maximum number of messages in queue.
/// \param[in]     msg_size      maximum message size in bytes.
/// \param[in]     attr          message queue attributes; NULL: default values.
/// \return message queue ID for reference by other functions or NULL in case of error.
osMessageQueueId_t osMessageQueueNew (uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr);

/// Get name of a Message Queue object.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return name as null-terminated string.
const char *osMessageQueueGetName (osMessageQueueId_t mq_id);

/**
 ************************************************************************************
 * @brief           发送消息到消息队列
 *
 * @param[in]       mq_id           消息队列id，通过osMessageQueueNew获取
 * @param[in]       msg_ptr         消息指针
 * @param[in]       size            消息指针空间大小
 * @param[in]       msg_prio        消息优先级，暂时无用
 * @param[in]       timeout         超时时长
 *
 * @return          状态码.
 ************************************************************************************
 */
/// Put a Message into a Queue or timeout if Queue is full.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \param[in]     msg_ptr       pointer to buffer with message to put into a queue.
/// \param[in]     msg_prio      message priority.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
osStatus_t osMessageQueuePut (osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);
osStatus_t osMessageQueuePutRelaxed (osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout, uint32_t relaxed_timeout);

/**
 ************************************************************************************
 * @brief           从消息队列获取消息
 *
 * @param[in]       mq_id           消息队列id，通过osMessageQueueNew获取
 * @param[out]      msg_ptr         消息指针
 * @param[in]       size            消息指针空间大小
 * @param[out]      msg_prio        消息优先级指针，暂时无用
 * @param[in]       timeout         超时时长
 *
 * @return          状态码.
 ************************************************************************************
 */
/// Get a Message from a Queue or timeout if Queue is empty.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \param[out]    msg_ptr       pointer to buffer for message to get from a queue.
/// \param[out]    msg_prio      pointer to buffer for message priority or NULL.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
osStatus_t osMessageQueueGet (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);
osStatus_t osMessageQueueGetRelaxed (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout, uint32_t relaxed_timeout);

/// Get maximum number of messages in a Message Queue.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return maximum number of messages.
uint32_t osMessageQueueGetCapacity (osMessageQueueId_t mq_id);

/// Get maximum message size in a Message Queue.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return maximum message size in bytes.
uint32_t osMessageQueueGetMsgSize (osMessageQueueId_t mq_id);

/// Get number of queued messages in a Message Queue.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return number of queued messages.
uint32_t osMessageQueueGetCount (osMessageQueueId_t mq_id);

/**
 ************************************************************************************
 * @brief           获取消息队列剩余空闲消息条数
 *
 * @param[in]       mq_id           消息队列id，通过osMessageQueueNew获取
 *
 * @return          剩余空闲消息条数.
 ************************************************************************************
 */
/// Get number of available slots for messages in a Message Queue.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return number of available slots for messages.
uint32_t osMessageQueueGetSpace (osMessageQueueId_t mq_id);

/// Reset a Message Queue to initial empty state.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return status code that indicates the execution status of the function.
osStatus_t osMessageQueueReset (osMessageQueueId_t mq_id);

/**
 ************************************************************************************
 * @brief           删除消息队列，释放资源
 *
 * @param[in]       mq_id           消息队列id，通过osMessageQueueNew获取
 *
 * @return          状态码.
 ************************************************************************************
 */
/// Delete a Message Queue object.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return status code that indicates the execution status of the function.
osStatus_t osMessageQueueDelete (osMessageQueueId_t mq_id);




#ifdef  __cplusplus
}
#endif

#endif  // CMSIS_OS2_H_
/** @} */
