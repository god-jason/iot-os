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

#endif /* IOT_PLATFORM_ESP32_H */
