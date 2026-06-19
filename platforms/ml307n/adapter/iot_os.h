/**
 * @file iot_os.h
 * @brief ML307N 平台操作系统适配器宏定义
 * @details 基于 cm_os.h 接口实现操作系统功能宏定义，
 *          提供统一的 OS 类型和操作接口，支持跨平台编译。
 */

#ifndef IOT_OS_ML307N_H
#define IOT_OS_ML307N_H

#include "cm_os.h"

/* ============================================================
 * 统一 OS 类型定义（跨平台兼容）
 * ============================================================ */

#define iot_mutex_t              osMutexId_t
#define iot_sem_t                osSemaphoreId_t
#define iot_timer_t              osTimerId_t
#define iot_task_t               osThreadId_t

#define iot_timer_type_t         osTimerType_t
#define IOT_TIMER_ONCE           osTimerOnce
#define IOT_TIMER_PERIODIC       osTimerPeriodic

#define iot_task_func_t          osThreadFunc_t
#define iot_timer_callback_t     osTimerFunc_t

#define iot_osPriority_t         osPriority_t
#define IOT_OS_PRIO_IDLE         osPriorityIdle
#define IOT_OS_PRIO_LOW          osPriorityLow
#define IOT_OS_PRIO_LOWEST       osPriorityLowest
#define IOT_OS_PRIO_BELOW_NORMAL  osPriorityBelowNormal
#define IOT_OS_PRIO_NORMAL       osPriorityNormal
#define IOT_OS_PRIO_ABOVE_NORMAL osPriorityAboveNormal
#define IOT_OS_PRIO_HIGH         osPriorityHigh
#define IOT_OS_PRIO_HIGHEST      osPriorityHighest
#define IOT_OS_PRIO_ERROR        osPriorityError

/* ============================================================
 * OS 操作宏定义
 * ============================================================ */

#define iot_mutex_create() \
    osMutexNew(NULL)

#define iot_mutex_lock(mutex, timeout_ms) \
    osMutexAcquire((mutex), (timeout_ms))

#define iot_mutex_unlock(mutex) \
    osMutexRelease((mutex))

#define iot_mutex_delete(mutex) \
    osMutexDelete((mutex))

#define iot_sem_create(max_count, initial_count) \
    osSemaphoreNew((max_count), (initial_count), NULL)

#define iot_sem_wait(sem) \
    osSemaphoreAcquire((sem), osWaitForever)

#define iot_sem_wait_timeout(sem, timeout_ms) \
    osSemaphoreAcquire((sem), (timeout_ms))

#define iot_sem_post(sem) \
    osSemaphoreRelease((sem))

#define iot_sem_get_count(sem) \
    osSemaphoreGetCount((sem))

#define iot_sem_delete(sem) \
    osSemaphoreDelete((sem))

#define iot_timer_create(cb, arg, period_ms, type) \
    osTimerNew((cb), (type), (arg), NULL)

#define iot_timer_start(timer, period_ms) \
    osTimerStart((timer), (period_ms))

#define iot_timer_stop(timer) \
    osTimerStop((timer))

#define iot_timer_delete(timer) \
    osTimerDelete((timer))

#define iot_timer_is_running(timer) \
    osTimerIsRunning((timer))

#define iot_task_create(name, func, arg, stack_size, priority) \
    osThreadNew((func), (arg), \
        &(const osThreadAttr_t){ \
            .name = (name), \
            .stack_size = (stack_size), \
            .priority = (priority) \
        })

#define iot_task_delay(ms) \
    osDelay((ms) * 1000 / 1000)

#define iot_task_delete(task) \
    osThreadTerminate((task))

#define iot_task_get_current() \
    osThreadGetId()

#define iot_get_tick_ms() \
    osKernelGetTickCount()

#define iot_get_tick_freq() \
    osKernelGetTickFreq()

#define iot_task_exit() \
    osThreadExit()

#endif /* IOT_OS_ML307N_H */