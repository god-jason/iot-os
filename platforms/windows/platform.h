/**
 * @file platform.h
 * @brief Windows 平台适配头文件
 */
#ifndef IOT_PLATFORM_WINDOWS_H
#define IOT_PLATFORM_WINDOWS_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>

/*===========================================================
 * OS 适配层
 *===========================================================*/

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

#define iot_timer_is_running(timer) \
    ((timer) != NULL)

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

/*===========================================================
 * 日志适配层
 *===========================================================*/

/* LOG 宏定义已移至 iot_log.h */

/*===========================================================
 * 平台打印接口
 *===========================================================*/

/**
 * @brief 打印字符串（不带换行）
 * @param str 要打印的字符串
 */
#define iot_puts(str) printf("%s", str)

/**
 * @brief 格式化打印（不带换行）
 * @param fmt 格式字符串
 * @param ... 可变参数
 */
#define iot_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)

/*===========================================================
 * 内存适配层
 *===========================================================*/

#define iot_malloc(size) \
    malloc((size_t)(size))

#define iot_calloc(nmemb, size) \
    calloc((size_t)(nmemb), (size_t)(size))

#define iot_realloc(ptr, size) \
    realloc((void *)(ptr), (size_t)(size))

#define iot_free(ptr) \
    free((void *)(ptr))

#define iot_strdup(str) \
    _strdup((str))

/*===========================================================
 * 文件系统适配层
 *===========================================================*/

#include <direct.h>
#include <sys/stat.h>

/* 文件系统路径最大长度 */
#define IOT_FS_MAX_PATH          255

#define iot_fs_file_t            FILE*
#define iot_fs_dir_t             intptr_t
#define iot_fs_dirent_t          struct _finddata_t

#define iot_fs_open(path, mode) \
    fopen((path), (mode))

#define iot_fs_close(fp) \
    fclose((fp))

#define iot_fs_read(fp, buf, size) \
    fread((buf), 1, (size), (fp))

#define iot_fs_write(fp, buf, size) \
    fwrite((buf), 1, (size), (fp))

#define iot_fs_seek(fp, offset, whence) \
    fseek((fp), (offset), (whence))

#define iot_fs_sync(fp) \
    fflush((fp))

#define iot_fs_mkdir(path, mode) \
    (_mkdir((path)) == 0)

#define iot_fs_remove(path) \
    (remove((path)) == 0)

#define iot_fs_rename(oldpath, newpath) \
    (rename((oldpath), (newpath)) == 0)

#define iot_fs_access(path, mode) \
    (_access((path), 0) == 0)

#define iot_fs_file_exists(path) \
    (_access((path), 0) == 0)

#define iot_fs_filesize(path) \
    ({ struct _stat st; _stat((path), &st); st.st_size; })

#define iot_fs_rewind(fp) \
    rewind((fp))

#define iot_fs_ftruncate(fd, length) \
    (_chsize(_fileno((fd)), (length)) == 0)

#define iot_fs_rmdir_recursive(path) \
    ((int)-1)

#define iot_fs_rmdir(path) \
    (_rmdir((path)) == 0)

#define iot_fs_getinfo(info) \
    ((int)-1)

#define iot_fs_find_first(path, file_data) \
    ((uint32_t)-1)

#define iot_fs_find_next(find_fd, file_data) \
    ((int)-1)

#define iot_fs_find_close(find_fd) \
    ((int)-1)

#define iot_fs_opendir(path) \
    ((intptr_t)_findfirst((path), NULL))

#define iot_fs_readdir(dir, entry) \
    ((void*)(dir))

#define iot_fs_closedir(dir) \
    (_findclose((intptr_t)(dir)))

#define iot_fs_file_tell(fp) \
    ftell((fp))

#define iot_fs_size(fp) \
    ({ long _cur = ftell(fp); fseek(fp, 0, SEEK_END); long _sz = ftell(fp); fseek(fp, _cur, SEEK_SET); _sz; })

#define IOT_FS_RB                "rb"
#define IOT_FS_WB                "wb"
#define IOT_FS_AB                "ab"
#define IOT_FS_WBPLUS            "wb+"
#define IOT_FS_ABPLUS            "ab+"
#define IOT_FS_RBPLUS            "rb+"

#define IOT_FS_SEEK_SET          SEEK_SET
#define IOT_FS_SEEK_CUR          SEEK_CUR
#define IOT_FS_SEEK_END          SEEK_END

#endif /* IOT_PLATFORM_WINDOWS_H */
