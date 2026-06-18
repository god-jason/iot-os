/**
 * @file iot_os.h
 * @brief ML307N 平台操作系统适配器头文件
 * @details 基于 cm_os 接口实现操作系统功能封装，
 *          定义统一的 OS 类型，支持跨平台编译。
 */

#ifndef IOT_OS_ML307N_H
#define IOT_OS_ML307N_H

#include "cm_os.h"

/* ============================================================
 * 统一 OS 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief 互斥锁句柄类型 */
typedef osMutexId_t iot_mutex_t;

/** @brief 信号量句柄类型 */
typedef osSemaphoreId_t iot_sem_t;

/** @brief 消息队列句柄类型 */
typedef osMessageQueueId_t iot_queue_t;

/** @brief 定时器句柄类型 */
typedef osTimerId_t iot_timer_t;

/** @brief 任务/线程句柄类型 */
typedef osThreadId_t iot_task_t;

/** @brief 定时器类型枚举（只有两个值，保持原样） */
typedef enum {
    IOT_TIMER_ONCE     = 0,          /**< 单次定时 */
    IOT_TIMER_PERIODIC = 1,          /**< 周期定时 */
} iot_timer_type_t;

/** @brief 定时器回调函数类型 */
typedef osTimerFunc_t iot_timer_callback_t;

/** @brief 任务函数类型 */
typedef osThreadFunc_t iot_task_func_t;

/* ============================================================
 * OS 类型转换宏（平台适配）
 * ============================================================ */

/** @brief 统一定时器类型转平台定时器类型 */
#define IOT_TIMER_TYPE_TO_PLATFORM(type) \
    ((type) == IOT_TIMER_PERIODIC ? osTimerPeriodic : osTimerOnce)

/* ============================================================
 * OS 操作函数声明
 * ============================================================ */

/**
 * @brief 创建互斥锁
 * @return 互斥锁句柄
 */
iot_mutex_t iot_mutex_create(void);

/**
 * @brief 锁定互斥锁
 * @param[in] mutex 互斥锁句柄
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
int iot_mutex_lock(iot_mutex_t mutex, uint32_t timeout_ms);

/**
 * @brief 解锁互斥锁
 * @param[in] mutex 互斥锁句柄
 * @return 0 成功
 */
int iot_mutex_unlock(iot_mutex_t mutex);

/**
 * @brief 删除互斥锁
 * @param[in] mutex 互斥锁句柄
 * @return 0 成功
 */
int iot_mutex_delete(iot_mutex_t mutex);

/**
 * @brief 创建信号量
 * @param[in] max_count 最大计数
 * @param[in] initial_count 初始计数
 * @return 信号量句柄
 */
iot_sem_t iot_sem_create(uint32_t max_count, uint32_t initial_count);

/**
 * @brief 等待信号量（永久等待）
 * @param[in] sem 信号量句柄
 * @return 0 成功
 */
int iot_sem_wait(iot_sem_t sem);

/**
 * @brief 等待信号量（带超时）
 * @param[in] sem 信号量句柄
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
int iot_sem_wait_timeout(iot_sem_t sem, uint32_t timeout_ms);

/**
 * @brief 释放信号量
 * @param[in] sem 信号量句柄
 * @return 0 成功
 */
int iot_sem_post(iot_sem_t sem);

/**
 * @brief 获取信号量计数
 * @param[in] sem 信号量句柄
 * @return 信号量计数
 */
uint32_t iot_sem_get_count(iot_sem_t sem);

/**
 * @brief 删除信号量
 * @param[in] sem 信号量句柄
 * @return 0 成功
 */
int iot_sem_delete(iot_sem_t sem);

/**
 * @brief 创建消息队列
 * @param[in] msg_num 消息数量
 * @param[in] msg_size 消息大小(字节)
 * @return 消息队列句柄
 */
iot_queue_t iot_queue_create(uint32_t msg_num, uint32_t msg_size);

/**
 * @brief 发送消息
 * @param[in] queue 消息队列句柄
 * @param[in] data 消息数据
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
int iot_queue_send(iot_queue_t queue, const void *data, uint32_t timeout_ms);

/**
 * @brief 接收消息（永久等待）
 * @param[in] queue 消息队列句柄
 * @param[out] data 消息数据缓冲区
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
int iot_queue_recv(iot_queue_t queue, void *data, uint32_t timeout_ms);

/**
 * @brief 删除消息队列
 * @param[in] queue 消息队列句柄
 * @return 0 成功
 */
int iot_queue_delete(iot_queue_t queue);

/**
 * @brief 创建定时器
 * @param[in] cb 定时器回调函数
 * @param[in] arg 回调参数
 * @param[in] period_ms 定时周期(毫秒)
 * @param[in] type 定时器类型
 * @return 定时器句柄
 */
iot_timer_t iot_timer_create(iot_timer_callback_t cb, void *arg, uint32_t period_ms, iot_timer_type_t type);

/**
 * @brief 启动定时器
 * @param[in] timer 定时器句柄
 * @param[in] period_ms 定时周期(毫秒)
 * @return 0 成功
 */
int iot_timer_start(iot_timer_t timer, uint32_t period_ms);

/**
 * @brief 停止定时器
 * @param[in] timer 定时器句柄
 * @return 0 成功
 */
int iot_timer_stop(iot_timer_t timer);

/**
 * @brief 删除定时器
 * @param[in] timer 定时器句柄
 * @return 0 成功
 */
int iot_timer_delete(iot_timer_t timer);

/**
 * @brief 创建任务
 * @param[in] name 任务名称
 * @param[in] func 任务函数
 * @param[in] arg 任务参数
 * @param[in] stack_size 栈大小(字节)
 * @param[in] priority 任务优先级
 * @return 任务句柄
 */
iot_task_t iot_task_create(const char *name, iot_task_func_t func, void *arg, uint32_t stack_size, int32_t priority);

/**
 * @brief 任务延时
 * @param[in] ms 延时时间(毫秒)
 */
void iot_task_delay(uint32_t ms);

/**
 * @brief 删除任务
 * @param[in] task 任务句柄
 * @return 0 成功
 */
int iot_task_delete(iot_task_t task);

/**
 * @brief 获取当前任务句柄
 * @return 当前任务句柄
 */
iot_task_t iot_task_get_current(void);

/**
 * @brief 获取系统tick(毫秒)
 * @return tick值
 */
uint32_t iot_get_tick_ms(void);

/**
 * @brief 获取系统tick频率
 * @return tick频率
 */
uint32_t iot_get_tick_freq(void);

#endif /* IOT_OS_ML307N_H */