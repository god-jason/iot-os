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

---

## 平台列表

| 平台 | 路径 | 说明 |
|------|------|------|
| ML307N | `platforms/ml307n/` | 中国移动物联网平台，使用 cm_os/cm_fs |
| YOPEN | `platforms/yopen/` | Yocto 嵌入式平台，使用 yopen_os/yopen_fs |
| ESP32 | `platforms/esp32/` | Espressif ESP32 平台，使用 FreeRTOS |
| Linux | `platforms/linux/` | Linux PC 平台，使用 pthread |
| Windows | `platforms/windows/` | Windows PC 平台，使用 Win32 API |

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
| `iot_sem_delete(s)` | `osSemaphoreDelete(s)` | `yopen_rtos_semaphore_delete(s)` | `vSemaphoreDelete(s)` | `sem_destroy(s)` | `CloseHandle(s)` |

### 任务/线程接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_task_create(n, f, a, sz, p)` | `osThreadNew(f, a, attr)` | `yopen_rtos_task_create(sz, p, n, f, a)` | `xTaskCreate(f, n, sz, a, p, h)` | `pthread_create(t, attr, f, a)` | `CreateThread(NULL, sz, f, a, 0, NULL)` |
| `iot_task_delay(ms)` | `osDelay(ms)` | `yopen_rtos_task_delay(ms)` | `vTaskDelay(pdMS_TO_TICKS(ms))` | `usleep(ms * 1000)` | `Sleep(ms)` |
| `iot_task_delete(t)` | `osThreadTerminate(t)` | `yopen_rtos_task_delete(t)` | `vTaskDelete(t)` | `pthread_cancel(t)` | `TerminateThread(t, 0)` |
| `iot_task_get_current()` | `osThreadGetId()` | `-` | `xTaskGetCurrentTaskHandle()` | `pthread_self()` | `GetCurrentThread()` |
| `iot_get_tick_ms()` | `osKernelGetTickCount()` | `yopen_rtos_get_time_ms()` | `xTaskGetTickCount() * portTICK_PERIOD_MS` | `clock_gettime()` | `GetTickCount()` |

### 优先级常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_OS_PRIO_IDLE` | `osPriorityIdle` | `1` | `tskIDLE_PRIORITY` | `0` | `THREAD_PRIORITY_IDLE` |
| `IOT_OS_PRIO_LOW` | `osPriorityLow` | `2` | `1` | `1` | `THREAD_PRIORITY_LOWEST` |
| `IOT_OS_PRIO_NORMAL` | `osPriorityNormal` | `5` | `4` | `4` | `THREAD_PRIORITY_NORMAL` |
| `IOT_OS_PRIO_HIGH` | `osPriorityHigh` | `7` | `6` | `6` | `THREAD_PRIORITY_HIGHEST` |

---

## 2. 定时器接口各平台实现

### 类型定义

| 类型 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_timer_t` | `osTimerId_t` | `yopen_timer_t` | `esp_timer_handle_t` | `timer_t` | `HANDLE` |

### 定时器模式常量

| 常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `IOT_TIMER_ONCE` | `osTimerOnce` | `YOPEN_TimerOnce` | `0` | `0` | `0` |
| `IOT_TIMER_PERIODIC` | `osTimerPeriodic` | `YOPEN_TimerPeriodic` | `1` | `1` | `WT_EXECUTEPERIODIC` |

### 定时器接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_timer_create(cb, arg, ms, type)` | `osTimerNew(cb, type, arg, NULL)` | `yopen_rtos_timer_create(cb, arg, ms, type)` | `esp_timer_create(args, &t)` | `timer_create(sev, t)` | `CreateTimerQueueTimer(NULL, NULL, cb, arg, 0, ms, flags)` |
| `iot_timer_start(t, ms)` | `osTimerStart(t, ms)` | `yopen_rtos_timer_start(t, ms)` | `esp_timer_start_periodic(t, ms*1000)` | `timer_settime(t, 0, its, NULL)` | `0` |
| `iot_timer_stop(t)` | `osTimerStop(t)` | `yopen_rtos_timer_stop(t)` | `esp_timer_stop(t)` | `timer_delete(t)` | `DeleteTimerQueueTimer(NULL, t, NULL)` |
| `iot_timer_delete(t)` | `osTimerDelete(t)` | `yopen_rtos_timer_delete(t)` | `esp_timer_delete(t)` | `timer_delete(t)` | `DeleteTimerQueueTimer(NULL, t, NULL)` |
| `iot_timer_is_running(t)` | `osTimerIsRunning(t)` | `yopen_rtos_timer_is_running(t)` | `esp_timer_is_active(t)` | `t != 0` | `t != NULL` |

---

## 3. FS接口各平台实现

### 类型定义

| 类型 | ML307N | YOPEN | ESP32/Linux/Windows |
|------|--------|-------|---------------------|
| `iot_fs_file_t` | `cm_fs_t` | `QFILE` | `FILE*` |
| `iot_fs_dir_t` | `uint32_t` | `QDIR*` | `DIR*` / `intptr_t` |

### 文件打开模式常量

| 常量 | ML307N | 其他平台 |
|------|--------|---------|
| `IOT_FS_RB` | `CM_FS_RB` | `"rb"` |
| `IOT_FS_WB` | `CM_FS_WB` | `"wb"` |
| `IOT_FS_WBPLUS` | `CM_FS_WBPLUS` | `"wb+"` |
| `IOT_FS_RBPLUS` | `CM_FS_RBPLUS` | `"rb+"` |

### 文件指针位置常量

| 常量 | ML307N | 其他平台 |
|------|--------|---------|
| `IOT_FS_SEEK_SET` | `CM_FS_SEEK_SET` | `SEEK_SET` (0) |
| `IOT_FS_SEEK_CUR` | `CM_FS_SEEK_CUR` | `SEEK_CUR` (1) |
| `IOT_FS_SEEK_END` | `CM_FS_SEEK_END` | `SEEK_END` (2) |

### 文件系统接口

| 接口 | ML307N | YOPEN | ESP32/Linux | Windows |
|------|--------|-------|-------------|---------|
| `iot_fs_open(path, mode)` | `cm_fs_open(path, mode)` | `yopen_fopen(path, mode)` | `fopen(path, mode)` | `fopen(path, mode)` |
| `iot_fs_close(fp)` | `cm_fs_close(fp)` | `yopen_fclose(fp)` | `fclose(fp)` | `fclose(fp)` |
| `iot_fs_read(fp, buf, size)` | `cm_fs_read(fp, buf, size)` | `yopen_fread(buf, 1, size, fp)` | `fread(buf, 1, size, fp)` | `fread(buf, 1, size, fp)` |
| `iot_fs_write(fp, buf, size)` | `cm_fs_write(fp, buf, size)` | `yopen_fwrite(buf, 1, size, fp)` | `fwrite(buf, 1, size, fp)` | `fwrite(buf, 1, size, fp)` |
| `iot_fs_seek(fp, off, whence)` | `cm_fs_seek(fp, off, whence)` | `yopen_fseek(fp, off, whence)` | `fseek(fp, off, whence)` | `fseek(fp, off, whence)` |
| `iot_fs_sync(fp)` | `cm_fs_sync(fp)` | `yopen_fflush(fp)` | `fflush(fp)` | `fflush(fp)` |
| `iot_fs_tell(fp)` | `0` | `yopen_ftell(fp)` | `ftell(fp)` | `ftell(fp)` |
| `iot_fs_mkdir(path, mode)` | `cm_fs_mkdir(path)` | `yopen_mkdir(path)` | `mkdir(path, mode)` | `_mkdir(path)` |
| `iot_fs_remove(path)` | `cm_fs_delete(path)` | `yopen_remove(path)` | `remove(path)` | `remove(path)` |
| `iot_fs_rename(old, new)` | `cm_fs_move(old, new)` | `yopen_frename(old, new)` | `rename(old, new)` | `rename(old, new)` |
| `iot_fs_access(path, mode)` | `cm_fs_exist(path)` | `yopen_file_exists(path)` | `access(path, mode)` | `_access(path, 0)` |
| `iot_fs_filesize(path)` | `cm_fs_filesize(path)` | `yopen_fsize(path)` | `stat().st_size` | `_stat().st_size` |
| `iot_fs_rmdir(path)` | `cm_fs_rmdir(path)` | `yopen_rmdir(path)` | `rmdir(path)` | `_rmdir(path)` |
| `iot_fs_rewind(fp)` | `cm_fs_seek(fp, 0, SET)` | `yopen_frewind(fp)` | `rewind(fp)` | `rewind(fp)` |
| `iot_fs_ftruncate(fd, len)` | 不支持 (-1) | `yopen_ftruncate(fd, len)` | `ftruncate(fileno(fd), len)` | `_chsize(_fileno(fd), len)` |
| `iot_fs_rmdir_recursive(p)` | 不支持 (-1) | `yopen_rmdir_recursive(p)` | 递归实现 | 不支持 (-1) |
| `iot_fs_opendir(path)` | `cm_fs_find_first(path, NULL)` | 不支持 | `opendir(path)` | `_findfirst(path, NULL)` |
| `iot_fs_readdir(d, e)` | `cm_fs_find_next(d, e)` | 不支持 | `readdir(d)` | 模拟实现 |
| `iot_fs_closedir(d)` | `cm_fs_find_close(d)` | 不支持 | `closedir(d)` | `_findclose(d)` |

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
| `IOT_SPI_BUS_0` | `CM_SPI_DEV_0` | `YOPEN_SPI_PORT0` | `SPI_HOST` | `/dev/spidevN.N` | 模拟表 |
| `IOT_SPI_BUS_1` | `CM_SPI_DEV_1` | `YOPEN_SPI_PORT1` | `SPI_HOST` | `/dev/spidevN.N` | 模拟表 |

### 模式常量

| 常量 | ML307N | YOPEN | ESP32 | Linux |
|------|--------|-------|-------|-------|
| `IOT_SPI_MODE_0` | `CM_SPI_WORK_MODE_0` | `CPOL_LOW \| CPHA_1Edge` | `SPI_MODE_0` | `SPI_MODE_0` |
| `IOT_SPI_MODE_1` | `CM_SPI_WORK_MODE_1` | `CPOL_LOW \| CPHA_2Edge` | `SPI_MODE_1` | `SPI_MODE_1` |
| `IOT_SPI_MODE_2` | `CM_SPI_WORK_MODE_2` | `CPOL_HIGH \| CPHA_1Edge` | `SPI_MODE_2` | `SPI_MODE_2` |
| `IOT_SPI_MODE_3` | `CM_SPI_WORK_MODE_3` | `CPOL_HIGH \| CPHA_2Edge` | `SPI_MODE_3` | `SPI_MODE_3` |

### SPI 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_spi_init(bus, cs, mode, speed)` | `cm_spi_open(bus, cfg)` | `yopen_spi_init(port, cfg)` | `spi_bus_initialize()` | `/dev/spidevN.N` | 模拟表 |
| `iot_spi_deinit(fd)` | `cm_spi_close(fd)` | `yopen_spi_deinit(port)` | `spi_bus_remove_device()` | `close(fd)` | 清表 |
| `iot_spi_write(fd, data, len)` | `cm_spi_write(fd, data, len)` | `yopen_spi_send(port, data, len)` | `spi_device_transmit()` | `write(fd, data, len)` | 写表 |
| `iot_spi_read(fd, data, len)` | `cm_spi_read(fd, data, len)` | `yopen_spi_recv(port, data, len)` | `spi_device_transmit()` | `read(fd, data, len)` | 读表 |
| `iot_spi_transfer(fd, tx, rx, len)` | `cm_spi_transfer(fd, tx, rx, len)` | `yopen_spi_transfer(port, tx, rx, len)` | `spi_device_transmit()` | `ioctl(fd, SPI_IOC_MESSAGE(1), &xfer)` | 组合读写 |

---

## 使用说明

### 引入头文件

```c
#include "platform.h"      // 基础平台接口 (OS, 内存, 日志, FS)
#include "platform_ext.h"  // 扩展平台接口 (GPIO, I2C, SPI)
```

### 代码示例

```c
// 创建互斥锁
iot_mutex_t mutex = iot_mutex_create();
iot_mutex_lock(mutex, 1000);  // 1秒超时
// ... 临界区代码
iot_mutex_unlock(mutex);
iot_mutex_delete(mutex);

// 创建信号量
iot_sem_t sem = iot_sem_create(10, 0);
iot_sem_wait_timeout(sem, 5000);  // 5秒超时
iot_sem_post(sem);
iot_sem_delete(sem);

// 创建任务
iot_task_create("my_task", my_task_func, arg, 4096, IOT_OS_PRIO_NORMAL);

// 文件操作
iot_fs_file_t fp = iot_fs_open("/test.txt", IOT_FS_WBPLUS);
if (fp) {
    iot_fs_write(fp, "hello", 5);
    iot_fs_close(fp);
}

// GPIO 操作
iot_gpio_init(LED_PIN, IOT_GPIO_DIR_OUTPUT);
iot_gpio_write(LED_PIN, IOT_GPIO_LEVEL_HIGH);

// I2C 操作
iot_i2c_init(IOT_I2C_BUS_0, IOT_I2C_CLK_400K);
uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};
iot_i2c_write(IOT_I2C_BUS_0, 0x50, 0x00, data, 4);
iot_i2c_read(IOT_I2C_BUS_0, 0x50, 0x00, data, 4);
```

---

## 注意事项

1. **Windows 平台限制**：Windows 平台的 GPIO/I2C/SPI 使用模拟表实现，仅用于开发调试，不支持实际硬件操作。

2. **Linux 平台限制**：GPIO 需要 kernel 5.10+ 提供的 `/sys/class/gpio` 接口支持。

3. **YOPEN 平台限制**：目录操作（`iot_fs_opendir`/`readdir`/`closedir`）暂不支持。

4. **ML307N 平台限制**：`iot_fs_ftruncate` 和 `iot_fs_rmdir_recursive` 暂不支持。

5. **线程安全**：所有平台适配层本身不是线程安全的，上层代码需要自行使用互斥锁保护共享资源。
