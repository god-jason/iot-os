#ifndef IOT_OS_LINUX_H
#define IOT_OS_LINUX_H

#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define iot_mutex_t              pthread_mutex_t*
#define iot_sem_t                sem_t*
#define iot_timer_t              timer_t
#define iot_task_t               pthread_t*

#define iot_timer_type_t         int
#define IOT_TIMER_ONCE           0
#define IOT_TIMER_PERIODIC       1

#define iot_task_func_t          void* (*)(void*)
#define iot_timer_callback_t     void (*)(sigval_t)

#define iot_osPriority_t         int
#define IOT_OS_PRIO_IDLE         0
#define IOT_OS_PRIO_LOW          1
#define IOT_OS_PRIO_LOWEST       2
#define IOT_OS_PRIO_BELOW_NORMAL 3
#define IOT_OS_PRIO_NORMAL       4
#define IOT_OS_PRIO_ABOVE_NORMAL 5
#define IOT_OS_PRIO_HIGH         6
#define IOT_OS_PRIO_HIGHEST      7
#define IOT_OS_PRIO_ERROR        -1

#define iot_mutex_create() ({ \
    pthread_mutex_t* m = malloc(sizeof(pthread_mutex_t)); \
    pthread_mutex_init(m, NULL); \
    m; \
})

#define iot_mutex_lock(mutex, timeout_ms) \
    (pthread_mutex_lock((mutex)) == 0)

#define iot_mutex_unlock(mutex) \
    pthread_mutex_unlock((mutex))

#define iot_mutex_delete(mutex) ({ \
    pthread_mutex_destroy((mutex)); \
    free((mutex)); \
})

#define iot_sem_create(max_count, initial_count) ({ \
    sem_t* s = malloc(sizeof(sem_t)); \
    sem_init(s, 0, (initial_count)); \
    s; \
})

#define iot_sem_wait(sem) \
    (sem_wait((sem)) == 0)

#define iot_sem_wait_timeout(sem, timeout_ms) ({ \
    struct timespec ts; \
    clock_gettime(CLOCK_REALTIME, &ts); \
    ts.tv_sec += (timeout_ms) / 1000; \
    ts.tv_nsec += ((timeout_ms) % 1000) * 1000000; \
    (sem_timedwait((sem), &ts) == 0); \
})

#define iot_sem_post(sem) \
    sem_post((sem))

#define iot_sem_get_count(sem) ({ \
    int cnt; \
    sem_getvalue((sem), &cnt); \
    cnt; \
})

#define iot_sem_delete(sem) ({ \
    sem_destroy((sem)); \
    free((sem)); \
})

#define iot_timer_create(cb, arg, period_ms, type) ({ \
    timer_t* t = malloc(sizeof(timer_t)); \
    struct sigevent sev; \
    sev.sigev_notify = SIGEV_THREAD; \
    sev.sigev_notify_function = (void(*)(sigval_t))(cb); \
    sev.sigev_value.sival_ptr = (arg); \
    timer_create(CLOCK_REALTIME, &sev, t); \
    *t; \
})

#define iot_timer_start(timer, period_ms) ({ \
    struct itimerspec its; \
    its.it_value.tv_sec = (period_ms) / 1000; \
    its.it_value.tv_nsec = ((period_ms) % 1000) * 1000000; \
    its.it_interval = its.it_value; \
    timer_settime((timer), 0, &its, NULL); \
    0; \
})

#define iot_timer_stop(timer) \
    timer_delete((timer))

#define iot_timer_delete(timer) \
    timer_delete((timer))

#define iot_timer_is_running(timer) \
    ((timer) != (timer_t)0)

#define iot_task_create(name, func, arg, stack_size, priority) ({ \
    pthread_t* t = malloc(sizeof(pthread_t)); \
    pthread_attr_t attr; \
    pthread_attr_init(&attr); \
    pthread_attr_setstacksize(&attr, (stack_size)); \
    pthread_create(t, &attr, (func), (arg)); \
    pthread_attr_destroy(&attr); \
    t; \
})

#define iot_task_delay(ms) \
    usleep((ms) * 1000)

#define iot_task_delete(task) ({ \
    pthread_cancel(*(task)); \
    pthread_join(*(task), NULL); \
    free((task)); \
})

#define iot_task_get_current() \
    (pthread_self())

#define iot_get_tick_ms() ({ \
    struct timespec ts; \
    clock_gettime(CLOCK_MONOTONIC, &ts); \
    ts.tv_sec * 1000 + ts.tv_nsec / 1000000; \
})

#define iot_get_tick_freq() \
    (1000)

#define iot_task_exit() \
    pthread_exit(NULL)

#endif