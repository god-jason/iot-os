/**
 * @file iot_os.c
 * @brief ML307N 平台操作系统适配器实现
 * @details 基于 cm_os 接口实现操作系统功能封装，
 *          提供统一的 OS 操作函数。
 */

#include "iot_os.h"

/* ============================================================
 * OS 操作函数实现
 * ============================================================ */

/**
 * @brief 创建互斥锁
 * @return 互斥锁句柄
 */
iot_mutex_t iot_mutex_create(void)
{
    return osMutexNew(NULL);
}

/**
 * @brief 锁定互斥锁
 * @param[in] mutex 互斥锁句柄
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
int iot_mutex_lock(iot_mutex_t mutex, uint32_t timeout_ms)
{
    osStatus_t status = osMutexAcquire(mutex, timeout_ms);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 解锁互斥锁
 * @param[in] mutex 互斥锁句柄
 * @return 0 成功
 */
int iot_mutex_unlock(iot_mutex_t mutex)
{
    osStatus_t status = osMutexRelease(mutex);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 删除互斥锁
 * @param[in] mutex 互斥锁句柄
 * @return 0 成功
 */
int iot_mutex_delete(iot_mutex_t mutex)
{
    osStatus_t status = osMutexDelete(mutex);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 创建信号量
 * @param[in] max_count 最大计数
 * @param[in] initial_count 初始计数
 * @return 信号量句柄
 */
iot_sem_t iot_sem_create(uint32_t max_count, uint32_t initial_count)
{
    return osSemaphoreNew(max_count, initial_count, NULL);
}

/**
 * @brief 等待信号量（永久等待）
 * @param[in] sem 信号量句柄
 * @return 0 成功
 */
int iot_sem_wait(iot_sem_t sem)
{
    osStatus_t status = osSemaphoreAcquire(sem, osWaitForever);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 等待信号量（带超时）
 * @param[in] sem 信号量句柄
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
int iot_sem_wait_timeout(iot_sem_t sem, uint32_t timeout_ms)
{
    osStatus_t status = osSemaphoreAcquire(sem, timeout_ms);
    return (status == osOK) ? IOT_OK : IOT_ERR_TIMEOUT;
}

/**
 * @brief 释放信号量
 * @param[in] sem 信号量句柄
 * @return 0 成功
 */
int iot_sem_post(iot_sem_t sem)
{
    osStatus_t status = osSemaphoreRelease(sem);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 获取信号量计数
 * @param[in] sem 信号量句柄
 * @return 信号量计数
 */
uint32_t iot_sem_get_count(iot_sem_t sem)
{
    return osSemaphoreGetCount(sem);
}

/**
 * @brief 删除信号量
 * @param[in] sem 信号量句柄
 * @return 0 成功
 */
int iot_sem_delete(iot_sem_t sem)
{
    osStatus_t status = osSemaphoreDelete(sem);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 创建消息队列
 * @param[in] msg_num 消息数量
 * @param[in] msg_size 消息大小(字节)
 * @return 消息队列句柄
 */
iot_queue_t iot_queue_create(uint32_t msg_num, uint32_t msg_size)
{
    return osMessageQueueNew(msg_num, msg_size, NULL);
}

/**
 * @brief 发送消息
 * @param[in] queue 消息队列句柄
 * @param[in] data 消息数据
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
int iot_queue_send(iot_queue_t queue, const void *data, uint32_t timeout_ms)
{
    osStatus_t status = osMessageQueuePut(queue, data, 0, timeout_ms);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 接收消息（永久等待）
 * @param[in] queue 消息队列句柄
 * @param[out] data 消息数据缓冲区
 * @param[in] timeout_ms 超时时间(毫秒)
 * @return 0 成功
 */
int iot_queue_recv(iot_queue_t queue, void *data, uint32_t timeout_ms)
{
    osStatus_t status = osMessageQueueGet(queue, data, NULL, timeout_ms);
    return (status == osOK) ? IOT_OK : IOT_ERR_TIMEOUT;
}

/**
 * @brief 删除消息队列
 * @param[in] queue 消息队列句柄
 * @return 0 成功
 */
int iot_queue_delete(iot_queue_t queue)
{
    osStatus_t status = osMessageQueueDelete(queue);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 创建定时器
 * @param[in] cb 定时器回调函数
 * @param[in] arg 回调参数
 * @param[in] period_ms 定时周期(毫秒)
 * @param[in] type 定时器类型
 * @return 定时器句柄
 */
iot_timer_t iot_timer_create(iot_timer_callback_t cb, void *arg, uint32_t period_ms, iot_timer_type_t type)
{
    return osTimerNew((osTimerFunc_t)cb, 
                      (type == IOT_TIMER_PERIODIC) ? osTimerPeriodic : osTimerOnce,
                      arg, NULL);
}

/**
 * @brief 启动定时器
 * @param[in] timer 定时器句柄
 * @param[in] period_ms 定时周期(毫秒)
 * @return 0 成功
 */
int iot_timer_start(iot_timer_t timer, uint32_t period_ms)
{
    osStatus_t status = osTimerStart(timer, period_ms);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 停止定时器
 * @param[in] timer 定时器句柄
 * @return 0 成功
 */
int iot_timer_stop(iot_timer_t timer)
{
    osStatus_t status = osTimerStop(timer);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 删除定时器
 * @param[in] timer 定时器句柄
 * @return 0 成功
 */
int iot_timer_delete(iot_timer_t timer)
{
    osStatus_t status = osTimerDelete(timer);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 创建任务
 * @param[in] name 任务名称
 * @param[in] func 任务函数
 * @param[in] arg 任务参数
 * @param[in] stack_size 栈大小(字节)
 * @param[in] priority 任务优先级
 * @return 任务句柄
 */
iot_task_t iot_task_create(const char *name, iot_task_func_t func, void *arg, uint32_t stack_size, int32_t priority)
{
    osThreadAttr_t attr = {0};
    attr.name = name;
    attr.stack_size = stack_size;
    attr.priority = (osPriority_t)priority;
    return osThreadNew(func, arg, &attr);
}

/**
 * @brief 任务延时
 * @param[in] ms 延时时间(毫秒)
 */
void iot_task_delay(uint32_t ms)
{
    osDelay(ms);
}

/**
 * @brief 删除任务
 * @param[in] task 任务句柄
 * @return 0 成功
 */
int iot_task_delete(iot_task_t task)
{
    osStatus_t status = osThreadTerminate(task);
    return (status == osOK) ? 0 : -1;
}

/**
 * @brief 获取当前任务句柄
 * @return 当前任务句柄
 */
iot_task_t iot_task_get_current(void)
{
    return osThreadGetId();
}

/**
 * @brief 获取系统tick(毫秒)
 * @return tick值
 */
uint32_t iot_get_tick_ms(void)
{
    return osKernelGetTickCount() * 1000 / osKernelGetTickFreq();
}

/**
 * @brief 获取系统tick频率
 * @return tick频率
 */
uint32_t iot_get_tick_freq(void)
{
    return osKernelGetTickFreq();
}