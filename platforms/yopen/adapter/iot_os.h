/**
 * @file iot_os.h
 * @brief YOPEN 平台操作系统适配器宏定义
 * @details 基于 yopen_os.h 接口实现操作系统功能宏定义，
 *          提供统一的 OS 类型和操作接口，支持跨平台编译。
 */

#ifndef IOT_OS_YOPEN_H
#define IOT_OS_YOPEN_H

#include "yopen_os.h"

/* ============================================================
 * 统一 OS 类型定义（跨平台兼容）
 * ============================================================ */

#define iot_mutex_t              yopen_mutex_t
#define iot_sem_t                yopen_sem_t
#define iot_timer_t              yopen_timer_t
#define iot_task_t               yopen_task_t

#define iot_timer_type_t         yopen_timertype_t
#define IOT_TIMER_ONCE           YOPEN_TimerOnce
#define IOT_TIMER_PERIODIC       YOPEN_TimerPeriodic

#define iot_task_func_t          void (*)(void*)
#define iot_timer_callback_t     yopen_timer_cb

#define iot_osPriority_t         uint8_t
#define IOT_OS_PRIO_IDLE         1
#define IOT_OS_PRIO_LOW          2
#define IOT_OS_PRIO_LOWEST       3
#define IOT_OS_PRIO_BELOW_NORMAL 4
#define IOT_OS_PRIO_NORMAL       5
#define IOT_OS_PRIO_ABOVE_NORMAL 6
#define IOT_OS_PRIO_HIGH         7
#define IOT_OS_PRIO_HIGHEST      8
#define IOT_OS_PRIO_ERROR        0

/* ============================================================
 * OS 操作宏定义
 * ============================================================ */

#define iot_mutex_create() \
    yopen_rtos_mutex_create()

#define iot_mutex_lock(mutex, timeout_ms) \
    yopen_rtos_mutex_acquire((mutex), (timeout_ms))

#define iot_mutex_unlock(mutex) \
    yopen_rtos_mutex_release((mutex))

#define iot_mutex_delete(mutex) \
    yopen_rtos_mutex_delete((mutex))

#define iot_sem_create(max_count, initial_count) \
    yopen_rtos_semaphore_create((max_count), (initial_count))

#define iot_sem_wait(sem) \
    yopen_rtos_semaphore_acquire((sem), YOPEN_WAIT_FOREVER)

#define iot_sem_wait_timeout(sem, timeout_ms) \
    yopen_rtos_semaphore_acquire((sem), (timeout_ms))

#define iot_sem_post(sem) \
    yopen_rtos_semaphore_release((sem))

#define iot_sem_get_count(sem) \
    yopen_rtos_semaphore_get_count((sem))

#define iot_sem_delete(sem) \
    yopen_rtos_semaphore_delete((sem))

#define iot_timer_create(cb, arg, period_ms, type) \
    yopen_rtos_timer_create((cb), (arg), (period_ms), (type))

#define iot_timer_start(timer, period_ms) \
    yopen_rtos_timer_start((timer), (period_ms))

#define iot_timer_stop(timer) \
    yopen_rtos_timer_stop((timer))

#define iot_timer_delete(timer) \
    yopen_rtos_timer_delete((timer))

#define iot_timer_is_running(timer) \
    yopen_rtos_timer_is_running((timer))

#define iot_task_create(name, func, arg, stack_size, priority) \
    yopen_rtos_task_create(NULL, (stack_size), (priority), (name), (func), (arg))

#define iot_task_delete(task) \
    yopen_rtos_task_delete((task))

#define iot_task_delay(ms) \
    yopen_rtos_task_delay((ms))

#define iot_task_delay_until(ms) \
    yopen_rtos_task_delay_until((ms))

#define iot_msleep(ms) \
    yopen_rtos_task_delay(ms)

#define iot_get_time_ms() \
    yopen_rtos_get_time_ms()

#define iot_get_time_s() \
    (yopen_rtos_get_time_ms() / 1000)

/* ============================================================
 * 内存屏障和中断操作
 * ============================================================ */

#define iot_enter_critical() \
    yopen_int_lock()

#define iot_exit_critical() \
    yopen_int_unlock()

#define iot_get_cpu_id() \
    0

#define iot_get_pid() \
    0

#endif /* IOT_OS_YOPEN_H */