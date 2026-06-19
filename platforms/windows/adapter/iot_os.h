#ifndef IOT_OS_WINDOWS_H
#define IOT_OS_WINDOWS_H

#include <windows.h>

#define iot_mutex_t              HANDLE
#define iot_sem_t                HANDLE
#define iot_timer_t              HANDLE
#define iot_task_t               HANDLE

#define iot_timer_type_t         int
#define IOT_TIMER_ONCE           0
#define IOT_TIMER_PERIODIC       1

#define iot_task_func_t          DWORD (__stdcall *)(LPVOID)
#define iot_timer_callback_t     VOID (__stdcall *)(PVOID)

#define iot_osPriority_t         int
#define IOT_OS_PRIO_IDLE         THREAD_PRIORITY_IDLE
#define IOT_OS_PRIO_LOW          THREAD_PRIORITY_LOWEST
#define IOT_OS_PRIO_LOWEST       THREAD_PRIORITY_BELOW_NORMAL
#define IOT_OS_PRIO_BELOW_NORMAL THREAD_PRIORITY_NORMAL
#define IOT_OS_PRIO_NORMAL       THREAD_PRIORITY_NORMAL
#define IOT_OS_PRIO_ABOVE_NORMAL THREAD_PRIORITY_ABOVE_NORMAL
#define IOT_OS_PRIO_HIGH         THREAD_PRIORITY_HIGHEST
#define IOT_OS_PRIO_HIGHEST      THREAD_PRIORITY_TIME_CRITICAL
#define IOT_OS_PRIO_ERROR        0

#define iot_mutex_create() \
    CreateMutex(NULL, FALSE, NULL)

#define iot_mutex_lock(mutex, timeout_ms) \
    (WaitForSingleObject((mutex), (timeout_ms)) == WAIT_OBJECT_0)

#define iot_mutex_unlock(mutex) \
    ReleaseMutex((mutex))

#define iot_mutex_delete(mutex) \
    CloseHandle((mutex))

#define iot_sem_create(max_count, initial_count) \
    CreateSemaphore(NULL, (initial_count), (max_count), NULL)

#define iot_sem_wait(sem) \
    (WaitForSingleObject((sem), INFINITE) == WAIT_OBJECT_0)

#define iot_sem_wait_timeout(sem, timeout_ms) \
    (WaitForSingleObject((sem), (timeout_ms)) == WAIT_OBJECT_0)

#define iot_sem_post(sem) \
    ReleaseSemaphore((sem), 1, NULL)

#define iot_sem_get_count(sem) \
    ((DWORD)0)

#define iot_sem_delete(sem) \
    CloseHandle((sem))

#define iot_timer_create(cb, arg, period_ms, type) \
    CreateTimerQueueTimer(NULL, NULL, (WAITORTIMERCALLBACK)(cb), (arg), 0, \
        (type == IOT_TIMER_PERIODIC) ? (period_ms) : 0, WT_EXECUTEDEFAULT)

#define iot_timer_start(timer, period_ms) \
    (0)

#define iot_timer_stop(timer) \
    DeleteTimerQueueTimer(NULL, (timer), NULL)

#define iot_timer_delete(timer) \
    DeleteTimerQueueTimer(NULL, (timer), NULL)

#define iot_task_create(name, func, arg, stack_size, priority) \
    CreateThread(NULL, (stack_size), (LPTHREAD_START_ROUTINE)(func), (arg), 0, NULL)

#define iot_task_delay(ms) \
    Sleep((ms))

#define iot_task_delete(task) \
    TerminateThread((task), 0)

#define iot_task_get_current() \
    GetCurrentThread()

#define iot_get_tick_ms() \
    GetTickCount()

#define iot_get_tick_freq() \
    (1000)

#define iot_task_exit() \
    ExitThread(0)

#endif