/**
 * @file platform.h
 * @brief ML307N 平台适配头文件
 */
#ifndef IOT_PLATFORM_ML307N_H
#define IOT_PLATFORM_ML307N_H

/*===========================================================
 * OS 适配层
 *===========================================================*/

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

/*===========================================================
 * 日志适配层
 *===========================================================*/

#include "os.h"
#include "cm_os.h"
#include "cm_sys.h"

/* LOG 宏定义已移至 iot_log.h */

/*===========================================================
 * 平台打印接口
 *===========================================================*/

/**
 * @brief 打印字符串（不带换行）
 * @param str 要打印的字符串
 */
#define iot_puts(str) cm_log_printf(0, "%s", str)

/**
 * @brief 格式化打印（不带换行）
 * @param fmt 格式字符串
 * @param ... 可变参数
 */
#define iot_printf(fmt, ...) cm_log_printf(0, fmt, ##__VA_ARGS__)

/*===========================================================
 * 内存适配层
 *===========================================================*/

#include "cm_mem.h"

/** @brief 内存对齐分配（按4字节对齐）
 * @param[in] size 分配内存大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_malloc(size) \
    cm_malloc((size_t)(size))

/** @brief 内存分配并清零（按4字节对齐）
 * @param[in] nmemb 元素个数
 * @param[in] size  每个元素大小（字节）
 * @return 分配成功的内存指针，NULL表示失败
 */
#define iot_calloc(nmemb, size) \
    cm_calloc((size_t)(nmemb), (size_t)(size))

/** @brief 重新分配内存（按4字节对齐）
 * @param[in] ptr  原内存指针（可为NULL）
 * @param[in] size 新内存大小（字节）
 * @return 新分配的内存指针，NULL表示失败
 */
#define iot_realloc(ptr, size) \
    cm_realloc((void *)(ptr), (size_t)(size))

/** @brief 释放内存
 * @param[in] ptr 待释放的内存指针（可为NULL）
 */
#define iot_free(ptr) \
    cm_free((void *)(ptr))

/*===========================================================
 * 文件系统适配层
 *===========================================================*/

#include <stdint.h>
#include "cm_fs.h"

#define iot_fs_file_t            cm_fs_t
#define iot_fs_dir_t             uint32_t
#define iot_fs_dirent_t          cm_fs_file_data_t

#define iot_fs_open(path, mode) \
    cm_fs_open((path), (mode))

#define iot_fs_close(fp) \
    cm_fs_close((fp))

#define iot_fs_read(fp, buf, size) \
    cm_fs_read((fp), (buf), (size))

#define iot_fs_write(fp, buf, size) \
    cm_fs_write((fp), (buf), (size))

#define iot_fs_seek(fp, offset, whence) \
    cm_fs_seek((fp), (offset), (whence))

#define iot_fs_sync(fp) \
    cm_fs_sync((fp))

#define iot_fs_mkdir(path, mode) \
    cm_fs_mkdir((path))

#define iot_fs_remove(path) \
    cm_fs_delete((path))

#define iot_fs_rename(oldpath, newpath) \
    cm_fs_move((oldpath), (newpath))

#define iot_fs_access(path, mode) \
    cm_fs_exist((path))

#define iot_fs_filesize(path) \
    cm_fs_filesize((path))

#define iot_fs_rmdir(path) \
    cm_fs_rmdir((path))

#define iot_fs_getinfo(info) \
    cm_fs_getinfo((info))

#define iot_fs_find_first(path, file_data) \
    cm_fs_find_first((path), (file_data))

#define iot_fs_find_next(find_fd, file_data) \
    cm_fs_find_next((find_fd), (file_data))

#define iot_fs_find_close(find_fd) \
    cm_fs_find_close((find_fd))

#define iot_fs_opendir(path) \
    cm_fs_find_first((path), NULL)

#define iot_fs_readdir(dir, entry) \
    cm_fs_find_next((dir), (entry))

#define iot_fs_closedir(dir) \
    cm_fs_find_close((dir))

#define iot_fs_tell(fp) \
    ((long)0)

#define iot_fs_size(fp) \
    ((long)0)

#define IOT_FS_RB                CM_FS_RB
#define IOT_FS_WB                CM_FS_WB
#define IOT_FS_AB                CM_FS_AB
#define IOT_FS_WBPLUS            CM_FS_WBPLUS
#define IOT_FS_ABPLUS            CM_FS_ABPLUS
#define IOT_FS_RBPLUS            CM_FS_RBPLUS

#define IOT_FS_SEEK_SET          CM_FS_SEEK_SET
#define IOT_FS_SEEK_CUR          CM_FS_SEEK_CUR
#define IOT_FS_SEEK_END          CM_FS_SEEK_END

#endif /* IOT_PLATFORM_ML307N_H */
