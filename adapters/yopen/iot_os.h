/**
 * @file iot_os.h
 * @brief YOPEN 平台操作系统适配器头文件
 * @details 基于 yopen_os 接口实现操作系统功能封装，
 *          定义统一的 OS 类型，支持跨平台编译。
 */

#ifndef IOT_OS_YOPEN_H
#define IOT_OS_YOPEN_H

#include "../iot_common.h"
#include "yopen_os.h"

/* ============================================================
 * 统一 OS 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief 互斥锁句柄类型 */
typedef yopen_mutex_t iot_mutex_t;

/** @brief 信号量句柄类型 */
typedef yopen_sem_t iot_sem_t;

/** @brief 消息队列句柄类型 */
typedef yopen_queue_t iot_queue_t;

/** @brief 定时器句柄类型 */
typedef yopen_timer_t iot_timer_t;

/** @brief 任务/线程句柄类型 */
typedef yopen_task_t iot_task_t;

/** @brief 定时器类型枚举（只有两个值，保持原样） */
typedef enum {
    IOT_TIMER_ONCE     = 0,          /**< 单次定时 */
    IOT_TIMER_PERIODIC = 1,          /**< 周期定时 */
} iot_timer_type_t;

/** @brief 定时器回调函数类型 */
typedef void (*iot_timer_callback_t)(void *arg);

/* ============================================================
 * OS 类型转换宏（平台适配）
 * ============================================================ */

/** @brief 统一定时器类型转平台定时器类型 */
#define IOT_TIMER_TYPE_TO_PLATFORM(type) \
    ((type) == IOT_TIMER_PERIODIC ? YOPEN_TimerPeriodic : YOPEN_TimerOnce)

/* ============================================================
 * OS 操作宏定义
 * ============================================================ */

/**
 * @brief 创建互斥锁
 * @return 互斥锁句柄
 */
#define iot_mutex_create() \
    ({ yopen_mutex_t m; yopen_rtos_mutex_create(&m); m; })

/**
 * @brief 锁定互斥锁
 * @param[in] mutex 互斥锁句柄
 * @return 0 成功
 */
#define iot_mutex_lock(mutex) \
    yopen_rtos_mutex_lock((mutex), YOPEN_WAIT_FOREVER)

/**
 * @brief 解锁互斥锁
 * @param[in] mutex 互斥锁句柄
 * @return 0 成功
 */
#define iot_mutex_unlock(mutex) \
    yopen_rtos_mutex_unlock(mutex)

/**
 * @brief 删除互斥锁
 * @param[in] mutex 互斥锁句柄
 */
#define iot_mutex_delete(mutex) \
    yopen_rtos_mutex_delete(mutex)

/**
 * @brief 创建信号量
 * @param[in] count 初始计数
 * @return 信号量句柄
 */
#define iot_sem_create(count) \
    ({ yopen_sem_t s; yopen_rtos_semaphore_create(&s, (count)); s; })

/**
 * @brief 等待信号量（永久等待）
 * @param[in] sem 信号量句柄
 * @return 0 成功
 */
#define iot_sem_wait(sem) \
    yopen_rtos_semaphore_wait((sem), YOPEN_WAIT_FOREVER)

/**
 * @brief 等待信号量（带超时）
 * @param[in] sem 信号量句柄
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
#define iot_sem_wait_timeout(sem, timeout_ms) \
    yopen_rtos_semaphore_wait((sem), (timeout_ms))

/**
 * @brief 释放信号量
 * @param[in] sem 信号量句柄
 * @return 0 成功
 */
#define iot_sem_post(sem) \
    yopen_rtos_semaphore_release(sem)

/**
 * @brief 删除信号量
 * @param[in] sem 信号量句柄
 */
#define iot_sem_delete(sem) \
    yopen_rtos_semaphore_delete(sem)

/**
 * @brief 创建消息队列
 * @param[in] msg_num 消息数量
 * @param[in] msg_size 消息大小(字节)
 * @return 消息队列句柄
 */
#define iot_queue_create(msg_num, msg_size) \
    ({ yopen_queue_t q; yopen_rtos_queue_create(&q, (msg_size), (msg_num)); q; })

/**
 * @brief 发送消息
 * @param[in] queue 消息队列句柄
 * @param[in] data 消息数据
 * @param[in] size 消息大小
 * @return 0 成功
 */
#define iot_queue_send(queue, data, size) \
    yopen_rtos_queue_release((queue), (size), (data), YOPEN_WAIT_FOREVER)

/**
 * @brief 接收消息（永久等待）
 * @param[in] queue 消息队列句柄
 * @param[out] data 消息数据缓冲区
 * @param[in] size 缓冲区大小
 * @return 0 成功
 */
#define iot_queue_recv(queue, data, size) \
    yopen_rtos_queue_wait((queue), (data), (size), YOPEN_WAIT_FOREVER)

/**
 * @brief 接收消息（带超时）
 * @param[in] queue 消息队列句柄
 * @param[out] data 消息数据缓冲区
 * @param[in] size 缓冲区大小
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
#define iot_queue_recv_timeout(queue, data, size, timeout_ms) \
    yopen_rtos_queue_wait((queue), (data), (size), (timeout_ms))

/**
 * @brief 删除消息队列
 * @param[in] queue 消息队列句柄
 */
#define iot_queue_delete(queue) \
    yopen_rtos_queue_delete(queue)

/**
 * @brief 创建定时器
 * @param[in] cb 定时器回调函数
 * @param[in] arg 回调参数
 * @param[in] period_ms 定时周期(毫秒)
 * @param[in] type 定时器类型
 * @return 定时器句柄
 */
#define iot_timer_create(cb, arg, period_ms, type) \
    ({ yopen_timer_t t; yopen_rtos_timer_create(&t, IOT_TIMER_TYPE_TO_PLATFORM(type), (void (*)(void *))(cb), (arg)); yopen_rtos_timer_start(t, (period_ms)); t; })

/**
 * @brief 启动定时器
 * @param[in] timer 定时器句柄
 * @return 0 成功
 */
#define iot_timer_start(timer) \
    yopen_rtos_timer_start((timer), 0)

/**
 * @brief 停止定时器
 * @param[in] timer 定时器句柄
 * @return 0 成功
 */
#define iot_timer_stop(timer) \
    yopen_rtos_timer_stop(timer)

/**
 * @brief 删除定时器
 * @param[in] timer 定时器句柄
 */
#define iot_timer_delete(timer) \
    yopen_rtos_timer_delete(timer)

/**
 * @brief 创建任务
 * @param[in] name 任务名称
 * @param[in] func 任务函数
 * @param[in] arg 任务参数
 * @param[in] stack_size 栈大小(字节)
 * @param[in] priority 任务优先级
 * @return 任务句柄
 */
#define iot_task_create(name, func, arg, stack_size, priority) \
    ({ yopen_task_t t; yopen_rtos_task_create(&t, (stack_size), (priority), (name), (void (*)(void *))(func), (arg)); t; })

/**
 * @brief 任务延时
 * @param[in] ms 延时时间(毫秒)
 */
#define iot_task_delay(ms) \
    yopen_rtos_task_sleep_ms(ms)

/**
 * @brief 删除任务
 * @param[in] task 任务句柄
 */
#define iot_task_delete(task) \
    yopen_rtos_task_delete(task)

/**
 * @brief 获取系统tick(毫秒)
 * @return tick值
 */
#define iot_get_tick_ms() \
    yopen_rtos_get_system_tick()

/**
 * @brief 禁用中断
 * @return 中断状态
 */
#define iot_interrupt_disable() \
    yopen_rtos_enter_critical()

/**
 * @brief 恢复中断
 * @param[in] flags 中断状态
 */
#define iot_interrupt_enable(flags) \
    yopen_rtos_exit_critical(flags)

#endif /* IOT_OS_YOPEN_H */