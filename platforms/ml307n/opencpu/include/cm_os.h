/**
 * @file        cm_os.h
 * @brief       操作系统接口
 *
 * @section CMSIS_RTOS_TimeOutValue Timeout Value
 *  Timeout values are an argument to several \b osXxx functions to allow time for resolving a request. A timeout value of \b 0 means that the RTOS does not wait and the function returns instantly, even when no resource is available. A timeout value of @ref osWaitForever means that the RTOS waits infinitely until a resource becomes available. Or one forces the thread to resume using @ref osThreadResume which is discouraged.
 *  The timeout value specifies the number of timer ticks until the time delay elapses. The value is an upper bound and depends on the actual time elapsed since the last timer tick.
 *  Examples:
 *    - timeout value \b 0 : the system does not wait, even when no resource is available the RTOS function returns instantly.
 *    - timeout value \b 1 : the system waits until the next timer tick occurs; depending on the previous timer tick, it may be a very short wait time.
 *    - timeout value \b 2 : actual wait time is between 1 and 2 timer ticks.
 *    - timeout value @ref osWaitForever : system waits infinite until a resource becomes available.
 *
 * @defgroup os os
 * @ingroup OS
 * @{
 */

#ifndef __CM_OS_H__
#define __CM_OS_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <stddef.h>

#include "cmsis_os2.h"

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

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 *
 * @brief Lock the RTOS Kernel scheduler.
 *
 * @return previous lock state (1 - locked, 0 - not locked, error code if negative).
 */
int32_t osKernelLock (void);

/**
 * @brief Unlock the RTOS Kernel scheduler.
 *
 * @return previous lock state (1 - locked, 0 - not locked, error code if negative).
 */
int32_t osKernelUnlock (void);

/**
 * @brief Get the RTOS kernel tick frequency.
 *
 * @return frequency of the kernel tick in hertz, i.e. kernel ticks per second.
 */
uint32_t osKernelGetTickFreq (void);

/**
 * @brief Get the RTOS kernel tick count.
 *
 * @return RTOS kernel current tick count.
 */
uint32_t osKernelGetTickCount (void);

/**
 * @brief Create a thread and add it to Active Threads.
 *
 * @param[in]     func          thread function.
 * @param[in]     argument      pointer that is passed to the thread function as start argument.
 * @param[in]     attr          thread attributes; NULL: default values.
 *
 * @return thread ID for reference by other functions or NULL in case of error.
 */
osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr);

/**
 * @brief Get name of a thread.
 *
 * @param[in]     thread_id     thread ID obtained by @ref osThreadNew or @ref osThreadGetId.
 *
 * @return name as null-terminated string.
 */
const char *osThreadGetName (osThreadId_t thread_id);

/**
 * @brief Return the thread ID of the current running thread.
 *
 * @return thread ID for reference by other functions or NULL in case of error.
 */
osThreadId_t osThreadGetId (void);

/**
 * @brief Get current thread state of a thread.
 *
 * @param[in]     thread_id     thread ID obtained by @ref osThreadNew or @ref osThreadGetId.
 *
 * @return current thread state of the specified thread.
 */
osThreadState_t osThreadGetState (osThreadId_t thread_id);

/**
 * @brief Get stack size of a thread.
 *
 * @param[in]     thread_id     thread ID obtained by @ref osThreadNew or @ref osThreadGetId.
 *
 * @return stack size in bytes.
 */
uint32_t osThreadGetStackSize (osThreadId_t thread_id);

/**
 * @brief Get available stack space of a thread based on stack watermark recording during execution.
 *
 * @param[in]     thread_id     thread ID obtained by @ref osThreadNew or @ref osThreadGetId.
 *
 * @return remaining stack space in bytes.
 */
uint32_t osThreadGetStackSpace (osThreadId_t thread_id);

/**
 * @brief Change priority of a thread.
 *
 * @param[in]     thread_id     thread ID obtained by @ref osThreadNew or @ref osThreadGetId.
 * @param[in]     priority      new priority value for the thread function.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority);

/**
 * @brief Get current priority of a thread.
 *
 * @param[in]     thread_id     thread ID obtained by @ref osThreadNew or @ref osThreadGetId.
 *
 * @return current priority value of the specified thread.
 */
osPriority_t osThreadGetPriority (osThreadId_t thread_id);

/**
 * @brief Pass control to next thread that is in state \b READY.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osThreadYield (void);

/**
 * @brief Suspend execution of a thread.
 *
 * @param[in]     thread_id     thread ID obtained by @ref osThreadNew or @ref osThreadGetId.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osThreadSuspend (osThreadId_t thread_id);

/**
 * @brief Resume execution of a thread.
 *
 * @param[in]     thread_id     thread ID obtained by @ref osThreadNew or @ref osThreadGetId.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osThreadResume (osThreadId_t thread_id);



/** Terminate execution of current running thread. */
//__NO_RETURN void osThreadExit (void);

/**
 * @brief Terminate execution of a thread.
 *
 * @param[in]     thread_id     thread ID obtained by @ref osThreadNew or @ref osThreadGetId.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osThreadTerminate (osThreadId_t thread_id);

/**
 * @brief Get number of active threads.
 *
 * @return number of active threads.
 */
uint32_t osThreadGetCount (void);



/**
 * @brief Wait for Timeout (Time Delay).
 *
 * @param[in]     ticks         @ref CMSIS_RTOS_TimeOutValue "time ticks" value
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osDelay (uint32_t ticks);



/**
 * @brief Create and Initialize a timer.
 *
 * @param[in]     func          function pointer to callback function.
 * @param[in]     type          @ref osTimerOnce for one-shot or @ref osTimerPeriodic for periodic behavior.
 * @param[in]     argument      argument to the timer callback function.
 * @param[in]     attr          timer attributes; NULL: default values.
 *
 * @return timer ID for reference by other functions or NULL in case of error.
 */
osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr);

/**
 * @brief Get name of a timer.
 *
 * @param[in]     timer_id      timer ID obtained by @ref osTimerNew.
 *
 * @return name as null-terminated string.
 */
//const char *osTimerGetName (osTimerId_t timer_id);

/**
 * @brief Start or restart a timer.
 *
 * @param[in]     timer_id      timer ID obtained by @ref osTimerNew.
 * @param[in]     ticks         @ref CMSIS_RTOS_TimeOutValue "time ticks" value of the timer.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osTimerStart (osTimerId_t timer_id, uint32_t ticks);

/**
 * @brief Stop a timer.
 *
 * @param[in]     timer_id      timer ID obtained by @ref osTimerNew.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osTimerStop (osTimerId_t timer_id);

/**
 * @brief Check if a timer is running.
 *
 * @param[in]      timer_id      timer ID obtained by @ref osTimerNew.
 *
 * @return
 *   = 0 -  not running \n
 *   = 1 -  running
 */
uint32_t osTimerIsRunning (osTimerId_t timer_id);

/**
 * @brief Delete a timer.
 *
 * @param[in]     timer_id      timer ID obtained by @ref osTimerNew.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osTimerDelete (osTimerId_t timer_id);

/**
 * @brief Create and Initialize an Event Flags object.
 *
 * @param[in]     attr          event flags attributes; NULL: default values.
 *
 * @return event flags ID for reference by other functions or NULL in case of error.
 */
osEventFlagsId_t osEventFlagsNew (const osEventFlagsAttr_t *attr);


/**
 * @brief Set the specified Event Flags.
 *
 * @param[in]     ef_id         event flags ID obtained by @ref osEventFlagsNew.
 * @param[in]     flags         specifies the flags that shall be set.
 *
 * @return thread flags input or error code if highest bit set.
 */
uint32_t osEventFlagsSet (osEventFlagsId_t ef_id, uint32_t flags);

/**
 * @brief Clear the specified Event Flags.
 *
 * @param[in]     ef_id         event flags ID obtained by @ref osEventFlagsNew.
 * @param[in]     flags         specifies the flags that shall be cleared.
 *
 * @return event flags before clearing or error code if highest bit set.
 */
//uint32_t osEventFlagsClear (osEventFlagsId_t ef_id, uint32_t flags);

/**
 * @brief Get the current Event Flags.
 *
 * @param[in]     ef_id         event flags ID obtained by @ref osEventFlagsNew.
 *
 * @return current event flags.
 */
//uint32_t osEventFlagsGet (osEventFlagsId_t ef_id);

/**
 * @brief Wait for one or more Event Flags to become signaled.
 *
 * @param[in]     ef_id         event flags ID obtained by @ref osEventFlagsNew.
 * @param[in]     flags         specifies the flags to wait for.
 * @param[in]     options       specifies flags options (osFlagsXxxx).
 * @param[in]     timeout       @ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
 *
 * @return event flags before clearing or error code if highest bit set.
 */
uint32_t osEventFlagsWait (osEventFlagsId_t ef_id, uint32_t flags, uint32_t options, uint32_t timeout);

/**
 * @brief Delete an Event Flags object.
 *
 * @param[in]      ef_id         event flags ID obtained by @ref osEventFlagsNew.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osEventFlagsDelete (osEventFlagsId_t ef_id);

/**
 * @brief Create and Initialize a Mutex object.
 *
 * @param[in]      attr          mutex attributes; NULL: default values.
 *
 * @return mutex ID for reference by other functions or NULL in case of error.
 */
osMutexId_t osMutexNew (const osMutexAttr_t *attr);


/**
 * @brief Acquire a Mutex or timeout if it is locked.
 *
 * @param[in]     mutex_id      mutex ID obtained by @ref osMutexNew.
 * @param[in]     timeout       @ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osMutexAcquire (osMutexId_t mutex_id, uint32_t timeout);

/**
 * @brief Release a Mutex that was acquired by @ref osMutexAcquire.
 *
 * @param[in]     mutex_id      mutex ID obtained by @ref osMutexNew.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osMutexRelease (osMutexId_t mutex_id);

/**
 * @brief Get Thread which owns a Mutex object.
 *
 * @param[in]     mutex_id      mutex ID obtained by @ref osMutexNew.
 *
 * @return thread ID of owner thread or NULL when mutex was not acquired.
 */
//osThreadId_t osMutexGetOwner (osMutexId_t mutex_id);

/**
 * @brief Delete a Mutex object.
 *
 * @param[in]     mutex_id      mutex ID obtained by @ref osMutexNew.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osMutexDelete (osMutexId_t mutex_id);

/**
 * @brief Create and Initialize a Semaphore object.
 *
 * @param[in]     max_count     maximum number of available tokens.
 * @param[in]     initial_count initial number of available tokens.
 * @param[in]     attr          semaphore attributes; NULL: default values.
 *
 * @return semaphore ID for reference by other functions or NULL in case of error.(only binary semaphore are supported)
 */
osSemaphoreId_t osSemaphoreNew (uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr);

/**
 * @brief Acquire a Semaphore token or timeout if no tokens are available.
 *
 * @param[in]     semaphore_id  semaphore ID obtained by @ref osSemaphoreNew.
 * @param[in]     timeout       @ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osSemaphoreAcquire (osSemaphoreId_t semaphore_id, uint32_t timeout);

/**
 * @brief Release a Semaphore token up to the initial maximum count.
 *
 * @param[in]     semaphore_id  semaphore ID obtained by @ref osSemaphoreNew.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osSemaphoreRelease (osSemaphoreId_t semaphore_id);

/**
 * @brief Get current Semaphore token count.
 *
 * @param[in]     semaphore_id  semaphore ID obtained by @ref osSemaphoreNew.
 *
 * @return number of tokens available.
 */
uint32_t osSemaphoreGetCount (osSemaphoreId_t semaphore_id);

/**
 * @brief Delete a Semaphore object.
 *
 * @param[in]     semaphore_id  semaphore ID obtained by @ref osSemaphoreNew.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osSemaphoreDelete (osSemaphoreId_t semaphore_id);




/**
 * @brief Create and Initialize a Message Queue object.
 *
 * @param[in]     msg_count     maximum number of messages in queue.
 * @param[in]     msg_size      maximum message size in bytes.
 * @param[in]     attr          message queue attributes; NULL: default values.
 *
 * @return message queue ID for reference by other functions or NULL in case of error.
 */
osMessageQueueId_t osMessageQueueNew (uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr);


/**
 * @brief Put a Message into a Queue or timeout if Queue is full.
 *
 * @param[in]     mq_id         message queue ID obtained by @ref osMessageQueueNew.
 * @param[in]     msg_ptr       pointer to buffer with message to put into a queue.
 * @param[in]     msg_prio      message priority.
 * @param[in]     timeout       @ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osMessageQueuePut (osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);

/**
 * @brief Get a Message from a Queue or timeout if Queue is empty.
 *
 * @param[in]     mq_id         message queue ID obtained by @ref osMessageQueueNew.
 * @param[out]    msg_ptr       pointer to buffer for message to get from a queue.
 * @param[out]    msg_prio      pointer to buffer for message priority or NULL.
 * @param[in]     timeout       @ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osMessageQueueGet (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);

/**
 * @brief Get maximum number of messages in a Message Queue.
 *
 * @param[in]     mq_id         message queue ID obtained by @ref osMessageQueueNew.
 *
 * @return maximum number of messages.
 */
//uint32_t osMessageQueueGetCapacity (osMessageQueueId_t mq_id);

/**
 * @brief Get maximum message size in a Message Queue.
 *
 * @param[in]     mq_id         message queue ID obtained by @ref osMessageQueueNew.
 *
 * @return maximum message size in bytes.
 */
//uint32_t osMessageQueueGetMsgSize (osMessageQueueId_t mq_id);

/**
 * @brief Get number of queued messages in a Message Queue.
 *
 * @param[in]     mq_id         message queue ID obtained by @ref osMessageQueueNew.
 *
 * @return number of queued messages.
 */
//uint32_t osMessageQueueGetCount (osMessageQueueId_t mq_id);

/**
 * @brief Get number of available slots for messages in a Message Queue.
 *
 * @param[in]     mq_id         message queue ID obtained by @ref osMessageQueueNew.
 *
 * @return number of available slots for messages.
 */
//uint32_t osMessageQueueGetSpace (osMessageQueueId_t mq_id);

/**
 * @brief Reset a Message Queue to initial empty state.
 *
 * @param[in]     mq_id         message queue ID obtained by @ref osMessageQueueNew.
 *
 * @return status code that indicates the execution status of the function.
 */
//osStatus_t osMessageQueueReset (osMessageQueueId_t mq_id);

/**
 * @brief Delete a Message Queue object.
 *
 * @param[in]     mq_id         message queue ID obtained by @ref osMessageQueueNew.
 *
 * @return status code that indicates the execution status of the function.
 */
osStatus_t osMessageQueueDelete (osMessageQueueId_t mq_id);


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif  /* __CM_OS_H__ */

/** @}*/