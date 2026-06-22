/**
 * @file platform.h
 * @brief Linux 平台适配头文件
 */
#ifndef IOT_PLATFORM_LINUX_H
#define IOT_PLATFORM_LINUX_H

#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

/*===========================================================
 * OS 适配层
 *===========================================================*/

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
    strdup((str))

/*===========================================================
 * 文件系统适配层
 *===========================================================*/

#include <dirent.h>
#include <sys/stat.h>

/* 文件系统路径最大长度 */
#define IOT_FS_MAX_PATH          255

#define iot_fs_file_t            FILE*
#define iot_fs_dir_t             DIR*
#define iot_fs_dirent_t          struct dirent

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
    (mkdir((path), (mode)) == 0)

#define iot_fs_remove(path) \
    (remove((path)) == 0)

#define iot_fs_rename(oldpath, newpath) \
    (rename((oldpath), (newpath)) == 0)

#define iot_fs_access(path, mode) \
    (access((path), (mode)) == 0)

#define iot_fs_file_exists(path) \
    (access((path), 0) == 0)

#define iot_fs_filesize(path) \
    ({ struct stat st; stat((path), &st); st.st_size; })

#define iot_fs_rewind(fp) \
    rewind((fp))

#define iot_fs_ftruncate(fd, length) \
    (ftruncate(fileno((fd)), (length)) == 0)

#define iot_fs_rmdir_recursive(path) \
    ({ int _ret = 0; DIR *_d = opendir((path)); if (_d) { struct dirent *_p; while ((_p = readdir(_d)) != NULL) { char _path[512]; snprintf(_path, sizeof(_path), "%s/%s", (path), _p->d_name); if (_p->d_type == DT_DIR) { _ret = iot_fs_rmdir_recursive(_path); } else { _ret = remove(_path); } } closedir(_d); _ret = rmdir((path)); } else { _ret = -1; } _ret; })

#define iot_fs_rmdir(path) \
    (rmdir((path)) == 0)

#define iot_fs_getinfo(info) \
    ((int)-1)

#define iot_fs_find_first(path, file_data) \
    ((uint32_t)-1)

#define iot_fs_find_next(find_fd, file_data) \
    ((int)-1)

#define iot_fs_find_close(find_fd) \
    ((int)-1)

#define iot_fs_opendir(path) \
    opendir((path))

#define iot_fs_readdir(dir, entry) \
    readdir((dir))

#define iot_fs_closedir(dir) \
    closedir((dir))

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

#endif /* IOT_PLATFORM_LINUX_H */
