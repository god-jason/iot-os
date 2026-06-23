# IoT-OS Platform 平台适配说明

本文档描述 IoT-OS 在各平台上的适配层接口定义，确保上层代码可以通过统一的接口调用底层功能。

## 目录

- [平台列表](#平台列表)
- [线程、锁、信号量接口](#1-线程锁信号量各平台实现)
- [定时器接口](#2-定时器接口各平台实现)
- [文件系统接口](#3-fs接口各平台实现)
- [GPIO 接口](#4-gpio接口各平台实现)
- [I2C 接口](#5-i2c接口各平台实现)
- [SPI 接口](#6-spi接口各平台实现)
- [打印和日志接口](#7-打印和日志接口)
- [内存管理接口](#8-内存管理接口)

---

## 平台列表

| 平台 | 路径 | 说明 |
|------|------|------|
| ML307N | `platforms/ml307n/` | 中国移动物联网平台，使用 cm_os/cm_fs |
| YOPEN | `platforms/yopen/` | Yocto 嵌入式平台，使用 yopen_os/yopen_fs |
| ESP32 | `platforms/esp32/` | Espressif ESP32 平台，使用 FreeRTOS |
| Linux | `platforms/linux/` | Linux PC 平台，使用 pthread |
| Windows | `platforms/windows/` | Windows PC 平台，使用 Win32 API |

### 头文件说明

```c
#include "iot.h"      // 基础平台接口 (OS, 内存, 日志, FS)
#include "iot_ext.h"  // 扩展平台接口 (GPIO, I2C, SPI)
```

---

## 1. 线程、锁、信号量各平台实现

### 类型定义

| 类型 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_mutex_t` | `osMutexId_t` | `yopen_mutex_t` | `SemaphoreHandle_t` | `pthread_mutex_t*` | `HANDLE` |
| `iot_sem_t` | `osSemaphoreId_t` | `yopen_sem_t` | `SemaphoreHandle_t` | `sem_t*` | `HANDLE` |
| `iot_task_t` | `osThreadId_t` | `yopen_task_t` | `TaskHandle_t` | `pthread_t*` | `HANDLE` |

### 互斥锁接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_mutex_create()` | `osMutexNew(NULL)` | `yopen_rtos_mutex_create()` | `xSemaphoreCreateMutex()` | `pthread_mutex_init()` | `CreateMutex()` |
| `iot_mutex_lock(m, t)` | `osMutexAcquire(m, t)` | `yopen_rtos_mutex_acquire(m, t)` | `xSemaphoreTake(m, pdMS_TO_TICKS(t))` | `pthread_mutex_lock(m)` | `WaitForSingleObject(m, t)` |
| `iot_mutex_unlock(m)` | `osMutexRelease(m)` | `yopen_rtos_mutex_release(m)` | `xSemaphoreGive(m)` | `pthread_mutex_unlock(m)` | `ReleaseMutex(m)` |
| `iot_mutex_delete(m)` | `osMutexDelete(m)` | `yopen_rtos_mutex_delete(m)` | `vSemaphoreDelete(m)` | `pthread_mutex_destroy(m)` | `CloseHandle(m)` |

### 信号量接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_sem_create(max, init)` | `osSemaphoreNew(max, init, NULL)` | `yopen_rtos_semaphore_create(max, init)` | `xSemaphoreCreateCounting(max, init)` | `sem_init(s, 0, init)` | `CreateSemaphore(NULL, init, max, NULL)` |
| `iot_sem_wait(s)` | `osSemaphoreAcquire(s, osWaitForever)` | `yopen_rtos_semaphore_acquire(s, FOREVER)` | `xSemaphoreTake(s, portMAX_DELAY)` | `sem_wait(s)` | `WaitForSingleObject(s, INFINITE)` |
| `iot_sem_wait_timeout(s, t)` | `osSemaphoreAcquire(s, t)` | `yopen_rtos_semaphore_acquire(s, t)` | `xSemaphoreTake(s, pdMS_TO_TICKS(t))` | `sem_timedwait(s, ts)` | `WaitForSingleObject(s, t)` |
| `iot_sem_post(s)` | `osSemaphoreRelease(s)` | `yopen_rtos_semaphore_release(s)` | `xSemaphoreGive(s)` | `sem_post(s)` | `ReleaseSemaphore(s, 1, NULL)` |
| `iot_sem_get_count(s)` | `osSemaphoreGetCount(s)` | `yopen_rtos_semaphore_get_count(s)` | `uxSemaphoreGetCount(s)` | `sem_getvalue(s, &cnt)` | `0` |
| `iot_sem_delete(s)` | `osSemaphoreDelete(s)` | `yopen_rtos_semaphore_delete(s)` | `vSemaphoreDelete(s)` | `sem_destroy(s)` | `CloseHandle(s)` |

### 任务/线程接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_task_create(n, f, a, sz, p)` | `osThreadNew(f, a, attr)` | `yopen_rtos_task_create(sz, p, n, f, a)` | `xTaskCreate(f, n, sz, a, p, h)` | `pthread_create(t, attr, f, a)` | `CreateThread(NULL, sz, f, a, 0, NULL)` |
| `iot_task_delay(ms)` | `osDelay(ms)` | `yopen_rtos_task_delay(ms)` | `vTaskDelay(pdMS_TO_TICKS(ms))` | `usleep(ms * 1000)` | `Sleep(ms)` |
| `iot_task_delete(t)` | `osThreadTerminate(t)` | `yopen_rtos_task_delete(t)` | `vTaskDelete(t)` | `pthread_cancel(t)` + `pthread_join(t)` | `TerminateThread(t, 0)` |
| `iot_task_get_current()` | `osThreadGetId()` | 不支持 | `xTaskGetCurrentTaskHandle()` | `pthread_self()` | `GetCurrentThread()` |
| `iot_get_tick_ms()` | `osKernelGetTickCount()` | `yopen_rtos_get_time_ms()` | `xTaskGetTickCount() * portTICK_PERIOD_MS` | `clock_gettime()` | `GetTickCount()` |
| `iot_get_tick_freq()` | `osKernelGetTickFreq()` | 不支持 | `configTICK_RATE_HZ` | `1000` | `1000` |
| `iot_task_exit()` | `osThreadExit()` | 不支持 | `vTaskDelete(NULL)` | `pthread_exit(NULL)` | `ExitThread(0)` |

### 优先级常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_OS_PRIO_IDLE` | `osPriorityIdle` | `1` | `tskIDLE_PRIORITY` | `0` | `THREAD_PRIORITY_IDLE` |
| `IOT_OS_PRIO_LOW` | `osPriorityLow` | `2` | `1` | `1` | `THREAD_PRIORITY_LOWEST` |
| `IOT_OS_PRIO_LOWEST` | `osPriorityLowest` | `3` | `2` | `2` | `THREAD_PRIORITY_BELOW_NORMAL` |
| `IOT_OS_PRIO_BELOW_NORMAL` | `osPriorityBelowNormal` | `4` | `3` | `3` | `THREAD_PRIORITY_NORMAL` |
| `IOT_OS_PRIO_NORMAL` | `osPriorityNormal` | `5` | `4` | `4` | `THREAD_PRIORITY_NORMAL` |
| `IOT_OS_PRIO_ABOVE_NORMAL` | `osPriorityAboveNormal` | `6` | `5` | `5` | `THREAD_PRIORITY_ABOVE_NORMAL` |
| `IOT_OS_PRIO_HIGH` | `osPriorityHigh` | `7` | `6` | `6` | `THREAD_PRIORITY_HIGHEST` |
| `IOT_OS_PRIO_HIGHEST` | `osPriorityHighest` | `8` | `7` | `7` | `THREAD_PRIORITY_TIME_CRITICAL` |

### 任务回调类型

```c
typedef osThreadFunc_t          iot_task_func_t;      // ML307N
typedef void (*)(void*)         iot_task_func_t;      // YOPEN/ESP32/Linux/Windows
```

---

## 2. 定时器接口各平台实现

### 类型定义

| 类型 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_timer_t` | `osTimerId_t` | `yopen_timer_t` | `esp_timer_handle_t` | `timer_t` | `HANDLE` |
| `iot_timer_type_t` | `osTimerType_t` | `yopen_timertype_t` | `int` | `int` | `int` |

### 定时器模式常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_TIMER_ONCE` | `osTimerOnce` | `YOPEN_TimerOnce` | `0` | `0` | `0` |
| `IOT_TIMER_PERIODIC` | `osTimerPeriodic` | `YOPEN_TimerPeriodic` | `1` | `1` | `WT_EXECUTEDEFAULT` |

### 定时器回调类型

```c
typedef osTimerFunc_t           iot_timer_callback_t; // ML307N
typedef yopen_timer_cb          iot_timer_callback_t; // YOPEN
typedef void (*)(void*)         iot_timer_callback_t; // ESP32/Linux/Windows
```

### 定时器接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_timer_create(cb, arg, ms, type)` | `osTimerNew(cb, type, arg, NULL)` | `yopen_rtos_timer_create(cb, arg, ms, type)` | `esp_timer_create(args, &t)` | `timer_create(sev, t)` | `CreateTimerQueueTimer(NULL, NULL, cb, arg, 0, ms, flags)` |
| `iot_timer_start(t, ms)` | `osTimerStart(t, ms)` | `yopen_rtos_timer_start(t, ms)` | `esp_timer_start_periodic(t, ms*1000)` | `timer_settime(t, 0, its, NULL)` | 直接在创建时设置周期 |
| `iot_timer_stop(t)` | `osTimerStop(t)` | `yopen_rtos_timer_stop(t)` | `esp_timer_stop(t)` | `timer_delete(t)` | `DeleteTimerQueueTimer(NULL, t, NULL)` |
| `iot_timer_delete(t)` | `osTimerDelete(t)` | `yopen_rtos_timer_delete(t)` | `esp_timer_delete(t)` | `timer_delete(t)` | `DeleteTimerQueueTimer(NULL, t, NULL)` |
| `iot_timer_is_running(t)` | `osTimerIsRunning(t)` | `yopen_rtos_timer_is_running(t)` | `esp_timer_is_active(t)` | `t != 0` | `t != NULL` |

---

## 3. FS接口各平台实现

### 类型定义

| 类型 | ML307N | YOPEN | ESP32/Linux | Windows |
|------|--------|-------|-------------|---------|
| `iot_fs_file_t` | `cm_fs_t` | `QFILE` | `FILE*` | `FILE*` |
| `iot_fs_dir_t` | `uint32_t` | `QDIR*` | `DIR*` | `intptr_t` |
| `iot_fs_dirent_t` | `cm_fs_file_data_t` | `qdirent` | `struct dirent` | `struct _finddata_t` |

### 文件打开模式常量

| 常量 | ML307N | 其他平台 |
|------|--------|---------|
| `IOT_FS_RB` | `CM_FS_RB` (0) | `"rb"` |
| `IOT_FS_WB` | `CM_FS_WB` (1) | `"wb"` |
| `IOT_FS_AB` | `CM_FS_AB` (2) | `"ab"` |
| `IOT_FS_WBPLUS` | `CM_FS_WBPLUS` (3) | `"wb+"` |
| `IOT_FS_ABPLUS` | `CM_FS_ABPLUS` (4) | `"ab+"` |
| `IOT_FS_RBPLUS` | `CM_FS_RBPLUS` (5) | `"rb+"` |

### 文件指针位置常量

| 常量 | ML307N | 其他平台 |
|------|--------|---------|
| `IOT_FS_SEEK_SET` | `CM_FS_SEEK_SET` (0) | `SEEK_SET` (0) |
| `IOT_FS_SEEK_CUR` | `CM_FS_SEEK_CUR` (1) | `SEEK_CUR` (1) |
| `IOT_FS_SEEK_END` | `CM_FS_SEEK_END` (2) | `SEEK_END` (2) |

### 基础文件操作接口

| 接口 | ML307N | YOPEN | ESP32/Linux | Windows |
|------|--------|-------|-------------|---------|
| `iot_fs_open(path, mode)` | `cm_fs_open(path, mode)` | `yopen_fopen(path, mode)` | `fopen(path, mode)` | `fopen(path, mode)` |
| `iot_fs_close(fp)` | `cm_fs_close(fp)` | `yopen_fclose(fp)` | `fclose(fp)` | `fclose(fp)` |
| `iot_fs_read(fp, buf, size)` | `cm_fs_read(fp, buf, size)` | `yopen_fread(buf, 1, size, fp)` | `fread(buf, 1, size, fp)` | `fread(buf, 1, size, fp)` |
| `iot_fs_write(fp, buf, size)` | `cm_fs_write(fp, buf, size)` | `yopen_fwrite(buf, 1, size, fp)` | `fwrite(buf, 1, size, fp)` | `fwrite(buf, 1, size, fp)` |
| `iot_fs_seek(fp, off, whence)` | `cm_fs_seek(fp, off, whence)` | `yopen_fseek(fp, off, whence)` | `fseek(fp, off, whence)` | `fseek(fp, off, whence)` |
| `iot_fs_sync(fp)` | `cm_fs_sync(fp)` | `yopen_fflush(fp)` | `fflush(fp)` | `fflush(fp)` |
| `iot_fs_file_tell(fp)` | 不支持 | `yopen_ftell(fp)` | `ftell(fp)` | `ftell(fp)` |
| `iot_fs_rewind(fp)` | `cm_fs_seek(fp, 0, SET)` | `yopen_frewind(fp)` | `rewind(fp)` | `rewind(fp)` |
| `iot_fs_size(fp)` | 不支持 | `yopen_fsize(fp)` | 计算实现 | 计算实现 |

### 目录操作接口

| 接口 | ML307N | YOPEN | ESP32/Linux | Windows |
|------|--------|-------|-------------|---------|
| `iot_fs_mkdir(path, mode)` | `cm_fs_mkdir(path)` | `yopen_mkdir(path)` | `mkdir(path, mode)` | `_mkdir(path)` |
| `iot_fs_rmdir(path)` | `cm_fs_rmdir(path)` | `yopen_rmdir(path)` | `rmdir(path)` | `_rmdir(path)` |
| `iot_fs_rmdir_recursive(p)` | 不支持 (-1) | `yopen_rmdir_recursive(p)` | 递归实现 | 不支持 (-1) |
| `iot_fs_opendir(path)` | `cm_fs_find_first(path, NULL)` | 不支持 | `opendir(path)` | `_findfirst(path, NULL)` |
| `iot_fs_readdir(d, e)` | `cm_fs_find_next(d, e)` | 不支持 | `readdir(d)` | 模拟实现 |
| `iot_fs_closedir(d)` | `cm_fs_find_close(d)` | 不支持 | `closedir(d)` | `_findclose(d)` |

### 文件查找接口

| 接口 | ML307N | YOPEN | ESP32/Linux | Windows |
|------|--------|-------|-------------|---------|
| `iot_fs_find_first(path, data)` | `cm_fs_find_first(path, data)` | 不支持 (-1) | 不支持 | 不支持 |
| `iot_fs_find_next(fd, data)` | `cm_fs_find_next(fd, data)` | 不支持 | 不支持 | 不支持 |
| `iot_fs_find_close(fd)` | `cm_fs_find_close(fd)` | 不支持 | 不支持 | 不支持 |

### 文件管理接口

| 接口 | ML307N | YOPEN | ESP32/Linux | Windows |
|------|--------|-------|-------------|---------|
| `iot_fs_remove(path)` | `cm_fs_delete(path)` | `yopen_remove(path)` | `remove(path)` | `remove(path)` |
| `iot_fs_rename(old, new)` | `cm_fs_move(old, new)` | `yopen_frename(old, new)` | `rename(old, new)` | `rename(old, new)` |
| `iot_fs_access(path, mode)` | `cm_fs_exist(path)` | `yopen_file_exists(path)` | `access(path, mode)` | `_access(path, 0)` |
| `iot_fs_file_exists(path)` | `cm_fs_exist(path)` | `yopen_file_exists(path)` | `access(path, 0) == 0` | `_access(path, 0) == 0` |
| `iot_fs_filesize(path)` | `cm_fs_filesize(path)` | `yopen_fsize(path)` | `stat().st_size` | `_stat().st_size` |
| `iot_fs_ftruncate(fd, len)` | 不支持 (-1) | `yopen_ftruncate(fd, len)` | `ftruncate(fileno(fd), len)` | `_chsize(_fileno(fd), len)` |
| `iot_fs_getinfo(info)` | `cm_fs_getinfo(info)` | 不支持 (-1) | 不支持 (-1) | 不支持 (-1) |

---

## 4. GPIO接口各平台实现

### 方向常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_GPIO_DIR_INPUT` | `CM_GPIO_DIRECTION_INPUT` | `GPIO_INPUT` | `GPIO_MODE_INPUT` | `0` | `0` |
| `IOT_GPIO_DIR_OUTPUT` | `CM_GPIO_DIRECTION_OUTPUT` | `GPIO_OUTPUT` | `GPIO_MODE_OUTPUT` | `1` | `1` |

### 电平常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_GPIO_LEVEL_LOW` | `CM_GPIO_LEVEL_LOW` | `LVL_LOW` | `0` | `0` | `0` |
| `IOT_GPIO_LEVEL_HIGH` | `CM_GPIO_LEVEL_HIGH` | `LVL_HIGH` | `1` | `1` | `1` |

### 中断类型常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_GPIO_IRQ_RISING` | `CM_GPIO_IT_EDGE_RISING` | 不支持 | `GPIO_INTR_POSEDGE` | 不支持 | 不支持 |
| `IOT_GPIO_IRQ_FALLING` | `CM_GPIO_IT_EDGE_FALLING` | 不支持 | `GPIO_INTR_NEGEDGE` | 不支持 | 不支持 |
| `IOT_GPIO_IRQ_BOTH` | `CM_GPIO_IT_EDGE_BOTH` | 不支持 | `GPIO_INTR_ANYEDGE` | 不支持 | 不支持 |
| `IOT_GPIO_IRQ_HIGH` | `CM_GPIO_IT_LEVEL_HIGH` | 不支持 | `GPIO_INTR_HIGH_LEVEL` | 不支持 | 不支持 |
| `IOT_GPIO_IRQ_LOW` | `CM_GPIO_IT_LEVEL_LOW` | 不支持 | `GPIO_INTR_LOW_LEVEL` | 不支持 | 不支持 |

### 上下拉常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_GPIO_PULL_NONE` | `CM_GPIO_PULL_NONE` | `PULL_DEFAULT` | `GPIO_FLOATING` | N/A | N/A |
| `IOT_GPIO_PULL_UP` | `CM_GPIO_PULL_UP` | `FORCE_PULL_UP` | `GPIO_PULLUP_ONLY` | N/A | N/A |
| `IOT_GPIO_PULL_DOWN` | `CM_GPIO_PULL_DOWN` | `FORCE_PULL_DOWN` | `GPIO_PULLDOWN_ONLY` | N/A | N/A |

### GPIO 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_gpio_init(pin, dir)` | `cm_gpio_init(pin, cfg)` | `yopen_gpio_init(pin, dir, pull, lvl)` | `gpio_config(cfg)` | `/sys/class/gpio` | 模拟表 |
| `iot_gpio_deinit(pin)` | `cm_gpio_deinit(pin)` | `yopen_gpio_deinit(pin)` | `gpio_reset_pin(pin)` | `/sys/class/gpio` | 清空表 |
| `iot_gpio_set_dir(pin, d)` | `cm_gpio_set_direction(pin, d)` | `yopen_gpio_set_direction(pin, d)` | `gpio_set_direction(pin, d)` | `/sys/class/gpio/gpioN/direction` | 写表 |
| `iot_gpio_write(pin, lvl)` | `cm_gpio_set_level(pin, lvl)` | `yopen_gpio_set_level(pin, lvl)` | `gpio_set_level(pin, lvl)` | `/sys/class/gpio/gpioN/value` | 写表 |
| `iot_gpio_read(pin)` | `cm_gpio_get_level(pin)` | `yopen_gpio_get_level(pin)` | `gpio_get_level(pin)` | `/sys/class/gpio/gpioN/value` | 读表 |
| `iot_gpio_toggle(pin)` | 读取后翻转 | 不支持 | 读取后翻转 | 读取后翻转 | 读取后翻转 |
| `iot_gpio_set_pull(pin, p)` | `cmo_gpio_set_pull(pin, p)` | `yopen_gpio_set_pull(pin, p)` | `gpio_set_pull_mode(pin, p)` | 不支持 | 不支持 |
| `iot_gpio_register_irq(p, m, cb)` | `cm_gpio_interrupt_enable(p, m)` | 不支持 | `gpio_isr_handler_add(p, cb)` | 不支持 | 不支持 |
| `iot_gpio_unregister_irq(p)` | `cm_gpio_interrupt_disable(p)` | 不支持 | `gpio_isr_handler_remove(p)` | 不支持 | 不支持 |

---

## 5. I2C接口各平台实现

### 总线常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_I2C_BUS_0` | `CM_I2C_DEV_0` | `i2c_0` | `I2C_NUM_0` | `0` | `0` |
| `IOT_I2C_BUS_1` | `CM_I2C_DEV_1` | `i2c_1` | `I2C_NUM_1` | `1` | `1` |

### 地址类型常量

| 常量 | ML307N | ESP32 |
|------|--------|-------|
| `IOT_I2C_ADDR_7BIT` | `CM_I2C_ADDR_TYPE_7BIT` | `I2C_ADDR_BIT_7` |
| `IOT_I2C_ADDR_10BIT` | `CM_I2C_ADDR_TYPE_10BIT` | `I2C_ADDR_BIT_10` |

### 速度常量

| 常量 | ML307N | YOPEN | ESP32 | Linux |
|------|--------|-------|-------|-------|
| `IOT_I2C_CLK_100K` | `I2C_CLK_100KHZ` | `STANDARD_MODE` | `100000` | `100000` |
| `IOT_I2C_CLK_400K` | `I2C_CLK_400KHZ` | `FAST_MODE` | `400000` | `400000` |
| `IOT_I2C_CLK_1M` | `I2C_CLK_1MHZ` | N/A | `1000000` | `1000000` |

### I2C 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_i2c_init(bus, speed)` | `cm_i2c_open_v2(bus, cfg)` | `yopen_I2cInit(bus, speed)` | `i2c_param_config(bus, cfg)` | `/dev/i2c-N` | 模拟表 |
| `iot_i2c_deinit(fd)` | `cm_i2c_close(fd)` | `yopen_I2cRelease(bus)` | `i2c_driver_delete(fd)` | `close(fd)` | 清表 |
| `iot_i2c_write(fd, addr, reg, data, len)` | `cm_i2c_write_v2(fd, addr, data, len)` | `yopen_I2cWrite(bus, addr, reg, data, len)` | `i2c_master_write()` | `write(fd, buf, len)` | 写表 |
| `iot_i2c_read(fd, addr, reg, data, len)` | `cm_i2c_read_v2(fd, addr, data, len)` | `yopen_I2cRead(bus, addr, reg, data, len)` | `i2c_master_read()` | `read(fd, buf, len)` | 读表 |
| `iot_i2c_write_raw(fd, addr, data, len)` | `cm_i2c_write_v2(fd, addr, data, len)` | `yopen_I2cWrite_Noaddr(bus, addr, data, len)` | 同 iot_i2c_write | 同上 | 写表 |
| `iot_i2c_read_raw(fd, addr, data, len)` | `cm_i2c_read_v2(fd, addr, data, len)` | `yopen_I2cRead_Noaddr(bus, addr, data, len)` | 同 iot_i2c_read | 同上 | 读表 |
| `iot_i2c_write_then_read(...)` | `cm_i2c_write_then_read(...)` | 不支持 | `i2c_master_write_then_read()` | 组合调用 | 组合调用 |

---

## 6. SPI接口各平台实现

### 总线常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_SPI_BUS_0` | `CM_SPI_DEV_0` | `YOPEN_SPI_PORT0` | `SPI2_HOST` | `0` | `0` |
| `IOT_SPI_BUS_1` | `CM_SPI_DEV_1` | `YOPEN_SPI_PORT1` | `SPI3_HOST` | `1` | `1` |

### 模式常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_SPI_MODE_0` | `CM_SPI_WORK_MODE_0` | `CPOL_LOW \| CPHA_1Edge` | `0` | `0` | `0` |
| `IOT_SPI_MODE_1` | `CM_SPI_WORK_MODE_1` | `CPOL_LOW \| CPHA_2Edge` | `1` | `1` | `1` |
| `IOT_SPI_MODE_2` | `CM_SPI_WORK_MODE_2` | `CPOL_HIGH \| CPHA_1Edge` | `2` | `2` | `2` |
| `IOT_SPI_MODE_3` | `CM_SPI_WORK_MODE_3` | `CPOL_HIGH \| CPHA_2Edge` | `3` | `3` | `3` |

### YOPEN SPI 时钟常量

| 常量 | 值 |
|------|-----|
| `IOT_SPI_CLK_812K` | `YOPEN_SPI_CLK_812_5KHZ` |
| `IOT_SPI_CLK_1_6M` | `YOPEN_SPI_CLK_1_625MHZ` |
| `IOT_SPI_CLK_3_2M` | `YOPEN_SPI_CLK_3_25MHZ` |
| `IOT_SPI_CLK_6_5M` | `YOPEN_SPI_CLK_6_5MHZ` |
| `IOT_SPI_CLK_13M` | `YOPEN_SPI_CLK_13MHZ` |

### SPI 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_spi_init(bus, cs, mode, speed)` | `cm_spi_open(bus, cfg)` | `yopen_spi_init(port, cfg)` | `spi_bus_add_device()` | `/dev/spidevN.N` | 模拟表 |
| `iot_spi_deinit(fd)` | `cm_spi_close(fd)` | `yopen_spi_deinit(port)` | `spi_bus_remove_device()` | `close(fd)` | 清表 |
| `iot_spi_write(fd, data, len)` | `cm_spi_write(fd, data, len)` | `yopen_spi_send(port, data, len)` | `spi_device_transmit()` | `write(fd, data, len)` | 写表 |
| `iot_spi_read(fd, data, len)` | `cm_spi_read(fd, data, len)` | `yopen_spi_recv(port, data, len)` | `spi_device_transmit()` | `read(fd, data, len)` | 读表 |
| `iot_spi_transfer(fd, tx, rx, len)` | `cm_spi_transfer(fd, tx, rx, len)` | `yopen_spi_transfer(port, tx, rx, len)` | `spi_device_transmit()` | `ioctl(fd, SPI_IOC_MESSAGE(1), &xfer)` | 组合读写 |
| `iot_spi_transfer_raw(fd, tx, rx, len, speed)` | 不支持 | `yopen_spi_transfer(port, tx, rx, len)` | `spi_device_transmit()` | `ioctl(fd, SPI_IOC_MESSAGE(1), &xfer)` | 组合读写 |

---

## 7. 打印和日志接口

### 打印接口

| 接口 | ML307N | YOPEN | ESP32/Linux/Windows |
|------|--------|-------|---------------------|
| `iot_puts(str)` | `cm_log_printf(0, "%s", str)` | `yopen_trace("%s", str)` | `printf("%s", str)` |
| `iot_printf(fmt, ...)` | `cm_log_printf(0, fmt, ...)` | `yopen_trace(fmt, ...)` | `printf(fmt, ...)` |

### 日志宏 (定义在 iot_log.h)

```c
#define LOG_INFO(fmt, ...) iot_printf("[iot] %s():%d " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
```

---

## 8. 内存管理接口

### 内存分配接口

| 接口 | ML307N | YOPEN | ESP32/Linux/Windows |
|------|--------|-------|---------------------|
| `iot_malloc(size)` | `cm_malloc(size)` | `yopen_malloc(size)` | `malloc(size)` |
| `iot_calloc(n, size)` | `cm_calloc(n, size)` | `yopen_calloc(n, size)` | `calloc(n, size)` |
| `iot_realloc(ptr, size)` | `cm_realloc(ptr, size)` | `yopen_realloc(ptr, size)` | `realloc(ptr, size)` |
| `iot_free(ptr)` | `cm_free(ptr)` | `yopen_free(ptr)` | `free(ptr)` |

### ML307N/YOPEN 特有接口

| 接口 | ML307N | YOPEN |
|------|--------|-------|
| `iot_mem_info(total, used, free)` | 不支持 | `yopen_mem_info(total, used, free)` |

---

## 代码示例

### 互斥锁使用

```c
iot_mutex_t mutex = iot_mutex_create();
if (iot_mutex_lock(mutex, 1000)) {
    // 临界区代码
    iot_mutex_unlock(mutex);
}
iot_mutex_delete(mutex);
```

### 信号量使用

```c
iot_sem_t sem = iot_sem_create(10, 0);
iot_sem_wait_timeout(sem, 5000);  // 5秒超时
iot_sem_post(sem);
iot_sem_delete(sem);
```

### 任务创建

```c
void my_task(void* arg) {
    while (1) {
        iot_task_delay(1000);
    }
    iot_task_exit();
}

iot_task_t task = iot_task_create("my_task", my_task_func, arg, 4096, IOT_OS_PRIO_NORMAL);
```

### 定时器使用

```c
void timer_callback(void* arg) {
    LOG_INFO("Timer triggered!");
}

iot_timer_t timer = iot_timer_create(timer_callback, NULL, 1000, IOT_TIMER_PERIODIC);
iot_timer_start(timer, 1000);  // 1秒周期
```

### 文件操作

```c
iot_fs_file_t fp = iot_fs_open("/test.txt", IOT_FS_WBPLUS);
if (fp) {
    iot_fs_write(fp, "hello", 5);
    iot_fs_seek(fp, 0, IOT_FS_SEEK_SET);
    char buf[32];
    int len = iot_fs_read(fp, buf, sizeof(buf));
    iot_fs_close(fp);
}
```

### GPIO 操作

```c
iot_gpio_init(LED_PIN, IOT_GPIO_DIR_OUTPUT);
iot_gpio_write(LED_PIN, IOT_GPIO_LEVEL_HIGH);
// 或翻转电平
iot_gpio_toggle(LED_PIN);
```

### I2C 操作

```c
iot_i2c_init(IOT_I2C_BUS_0, IOT_I2C_CLK_400K);
uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};
iot_i2c_write(IOT_I2C_BUS_0, 0x50, 0x00, data, 4);
iot_i2c_read(IOT_I2C_BUS_0, 0x50, 0x00, data, 4);
```

### SPI 操作

```c
iot_spi_init(IOT_SPI_BUS_0, 0, IOT_SPI_MODE_0, 1000000);
uint8_t tx_buf[4] = {0xAA, 0xBB, 0xCC, 0xDD};
uint8_t rx_buf[4];
iot_spi_transfer(IOT_SPI_BUS_0, tx_buf, rx_buf, 4);
```

---

## 注意事项

### 平台限制

| 平台 | 限制说明 |
|------|---------|
| **Windows** | GPIO/I2C/SPI 使用模拟表实现，仅用于开发调试 |
| **Linux** | GPIO 需要 kernel 5.10+ 的 `/sys/class/gpio` 接口支持 |
| **YOPEN** | 目录操作（`iot_fs_opendir`/`readdir`/`closedir`）暂不支持 |
| **YOPEN** | 文件查找接口（`iot_fs_find_*`）暂不支持 |
| **ML307N** | `iot_fs_ftruncate` 和 `iot_fs_rmdir_recursive` 暂不支持 |
| **ML307N** | `iot_fs_file_tell` 和 `iot_fs_size` 暂不支持 |
| **ML307N** | 文件查找接口（`iot_fs_find_*`）为 cm_fs 封装，与标准接口不同 |
| **ESP32/Linux** | `iot_fs_getinfo` 暂不支持 |

### 线程安全

- 所有平台适配层本身**不是线程安全**的
- 上层代码需要自行使用互斥锁保护共享资源

### 内存对齐

- **ML307N** 平台使用 4 字节对齐的内存分配 (`cm_malloc` 等)
- 其他平台使用标准 C 内存分配函数
