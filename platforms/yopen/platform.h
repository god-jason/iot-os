/**
 * @file platform.h
 * @brief YOPEN 平台适配头文件
 */
#ifndef IOT_PLATFORM_YOPEN_H
#define IOT_PLATFORM_YOPEN_H

/*===========================================================
 * OS 适配层
 *===========================================================*/

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

/*===========================================================
 * 日志适配层
 *===========================================================*/

#include "yopen_debug.h"

/* LOG 宏定义已移至 iot_log.h */

/*===========================================================
 * 平台打印接口
 *===========================================================*/

/**
 * @brief 打印字符串（不带换行）
 * @param str 要打印的字符串
 */
#define iot_puts(str) yopen_trace("%s", str)

/**
 * @brief 格式化打印（不带换行）
 * @param fmt 格式字符串
 * @param ... 可变参数
 */
#define iot_printf(fmt, ...) yopen_trace(fmt, ##__VA_ARGS__)

/*===========================================================
 * 内存适配层
 *===========================================================*/

#include "yopen_type.h"

/** @brief 内存分配
 * @param[in] size 分配内存大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_malloc(size) \
    yopen_malloc((size_t)(size))

/** @brief 内存分配并清零
 * @param[in] nmemb 元素个数
 * @param[in] size  每个元素大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_calloc(nmemb, size) \
    yopen_calloc((size_t)(nmemb), (size_t)(size))

/** @brief 重新分配内存
 * @param[in] ptr  原内存指针（可为NULL）
 * @param[in] size 新内存大小（字节）
 * @return 新分配的内存指针，NULL表示失败
 */
#define iot_realloc(ptr, size) \
    yopen_realloc((void *)(ptr), (size_t)(size))

/** @brief 释放内存
 * @param[in] ptr 待释放的内存指针（可为NULL）
 */
#define iot_free(ptr) \
    yopen_free((void *)(ptr))

/** @brief 获取内存使用信息
 * @param[out] total 总内存
 * @param[out] used 已用内存
 * @param[out] free 空闲内存
 */
#define iot_mem_info(total, used, free) \
    yopen_mem_info((total), (used), (free))

/*===========================================================
 * 文件系统适配层
 *===========================================================*/

#include "yopen_fs.h"
#include "yopen_type.h"

#define iot_fs_file_t            QFILE
#define iot_fs_dir_t             QDIR*
#define iot_fs_dirent_t          qdirent

#define iot_fs_open(path, mode) \
    yopen_fopen((path), (mode))

#define iot_fs_close(fp) \
    yopen_fclose((fp))

#define iot_fs_read(fp, buf, size) \
    yopen_fread((fp), (buf), (size))

#define iot_fs_write(fp, buf, size) \
    yopen_fwrite((fp), (buf), (size))

#define iot_fs_seek(fp, offset, whence) \
    yopen_fseek((fp), (offset), (whence))

#define iot_fs_sync(fp) \
    yopen_fflush((fp))

#define iot_fs_mkdir(path) \
    yopen_mkdir((path))

#define iot_fs_remove(path) \
    yopen_remove((path))

#define iot_fs_rename(oldpath, newpath) \
    yopen_frename((oldpath), (newpath))

#define iot_fs_access(path, mode) \
    yopen_file_exists((path))

#define iot_fs_tell(fp) \
    yopen_ftell((fp))

#define iot_fs_size(fp) \
    yopen_fsize((fp))

#endif /* IOT_PLATFORM_YOPEN_H */
