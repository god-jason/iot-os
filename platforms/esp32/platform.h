/**
 * @file platform.h
 * @brief ESP32 平台适配头文件
 */
#ifndef IOT_PLATFORM_ESP32_H
#define IOT_PLATFORM_ESP32_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/timers.h>
#include <esp_timer.h>
#include <esp_log.h>
#include <esp_err.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

/*===========================================================
 * OS 适配层
 *===========================================================*/

#define iot_mutex_t              SemaphoreHandle_t
#define iot_sem_t                SemaphoreHandle_t
#define iot_timer_t              esp_timer_handle_t
#define iot_task_t               TaskHandle_t

#define iot_timer_type_t         int
#define IOT_TIMER_ONCE           0
#define IOT_TIMER_PERIODIC       1

#define iot_task_func_t          void (*)(void*)
#define iot_timer_callback_t     void (*)(void*)

#define iot_osPriority_t         UBaseType_t
#define IOT_OS_PRIO_IDLE         tskIDLE_PRIORITY
#define IOT_OS_PRIO_LOW          1
#define IOT_OS_PRIO_LOWEST       2
#define IOT_OS_PRIO_BELOW_NORMAL 3
#define IOT_OS_PRIO_NORMAL       4
#define IOT_OS_PRIO_ABOVE_NORMAL 5
#define IOT_OS_PRIO_HIGH         6
#define IOT_OS_PRIO_HIGHEST      7
#define IOT_OS_PRIO_ERROR        -1

#define iot_mutex_create() \
    xSemaphoreCreateMutex()

#define iot_mutex_lock(mutex, timeout_ms) \
    (xSemaphoreTake((mutex), pdMS_TO_TICKS(timeout_ms)) == pdTRUE)

#define iot_mutex_unlock(mutex) \
    xSemaphoreGive((mutex))

#define iot_mutex_delete(mutex) \
    vSemaphoreDelete((mutex))

#define iot_sem_create(max_count, initial_count) \
    xSemaphoreCreateCounting((max_count), (initial_count))

#define iot_sem_wait(sem) \
    (xSemaphoreTake((sem), portMAX_DELAY) == pdTRUE)

#define iot_sem_wait_timeout(sem, timeout_ms) \
    (xSemaphoreTake((sem), pdMS_TO_TICKS(timeout_ms)) == pdTRUE)

#define iot_sem_post(sem) \
    xSemaphoreGive((sem))

#define iot_sem_get_count(sem) \
    (uxSemaphoreGetCount((sem)))

#define iot_sem_delete(sem) \
    vSemaphoreDelete((sem))

#define iot_timer_create(cb, arg, period_ms, type) ({ \
    esp_timer_handle_t timer = NULL; \
    const esp_timer_create_args_t args = { \
        .callback = (esp_timer_cb_t)(cb), \
        .arg = (arg), \
        .dispatch_method = ESP_TIMER_TASK, \
        .name = "iot_timer" \
    }; \
    esp_timer_create(&args, &timer); \
    timer; \
})

#define iot_timer_start(timer, period_ms) \
    esp_timer_start_periodic((timer), (period_ms) * 1000)

#define iot_timer_stop(timer) \
    esp_timer_stop((timer))

#define iot_timer_delete(timer) \
    esp_timer_delete((timer))

#define iot_timer_is_running(timer) \
    esp_timer_is_active((timer))

#define iot_task_create(name, func, arg, stack_size, priority) ({ \
    TaskHandle_t handle = NULL; \
    xTaskCreate((func), (name), (stack_size), (arg), (priority), &handle); \
    handle; \
})

#define iot_task_delay(ms) \
    vTaskDelay(pdMS_TO_TICKS(ms))

#define iot_task_delete(task) \
    vTaskDelete((task))

#define iot_task_get_current() \
    xTaskGetCurrentTaskHandle()

#define iot_get_tick_ms() \
    (xTaskGetTickCount() * portTICK_PERIOD_MS)

#define iot_get_tick_freq() \
    (configTICK_RATE_HZ)

#define iot_task_exit() \
    vTaskDelete(NULL)

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
#define IOT_FS_OPEN_USES_STRING_MODE

#define IOT_FS_SEEK_SET          SEEK_SET
#define IOT_FS_SEEK_CUR          SEEK_CUR
#define IOT_FS_SEEK_END          SEEK_END

#endif /* IOT_PLATFORM_ESP32_H */
