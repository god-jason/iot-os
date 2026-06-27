# IoT-OS Platform 平台适配说明

本文档描述 IoT-OS 在各平台上的适配层接口定义，确保上层代码可以通过统一的接口调用底层功能。

## 目录

- [平台列表](#平台列表)
- [头文件说明](#头文件说明)
- [内存管理接口](#1-内存管理接口)
- [互斥锁接口](#2-互斥锁接口)
- [信号量接口](#3-信号量接口)
- [任务接口](#4-任务接口)
- [定时器接口](#5-定时器接口)
- [Socket 网络接口](#6-socket-网络接口)
- [文件系统接口](#7-文件系统接口)
- [标准输出接口](#8-标准输出接口)
- [日志接口](#9-日志接口)
- [GPIO 接口](#10-gpio-接口)
- [I2C 接口](#11-i2c-接口)
- [SPI 接口](#12-spi-接口)

---

## 平台列表

| 平台 | 路径 | 说明 |
|------|------|------|
| ML307N | `platforms/ml307n/` | 中国移动物联网平台，使用 cm_os/cm_fs |
| YOPEN | `platforms/yopen/` | Yocto 嵌入式平台，使用 yopen_os/yopen_fs |
| ESP32 | `platforms/esp32/` | Espressif ESP32 平台，使用 FreeRTOS |
| Linux | `platforms/linux/` | Linux PC 平台，使用 pthread |
| Windows | `platforms/windows/` | Windows PC 平台，使用 Win32 API |
| WASM | `platforms/wasm/` | WebAssembly 浏览器平台，使用 Emscripten |

### 平台特性对比

| 特性 | ML307N | YOPEN | ESP32 | Linux | Windows | WASM |
|------|--------|-------|-------|-------|---------|------|
| **操作系统** | OneOS (CMSIS-RTOS2) | Yocto Linux | FreeRTOS | Linux | Windows | 浏览器事件循环 |
| **内存管理** | cm_malloc (4字节对齐) | yopen_malloc | heap_caps_malloc | malloc | malloc | malloc (Emscripten) |
| **多线程** | ✅ 支持 | ✅ 支持 | ✅ 支持 | ✅ 支持 | ✅ 支持 | ⚠️ 单线程 (async_call) |
| **互斥锁** | ✅ osMutex | ✅ yopen_mutex | ✅ xSemaphore | ✅ pthread_mutex | ✅ CreateMutex | ⚠️ 空操作 (单线程) |
| **信号量** | ✅ osSemaphore | ✅ yopen_sem | ✅ xSemaphore | ✅ sem_t | ✅ CreateSemaphore | ⚠️ 空操作 (单线程) |
| **定时器** | ✅ osTimer | ✅ yopen_timer | ✅ xTimer | ✅ timer_create | ✅ 自定义实现 | ✅ setInterval/setTimeout |
| **网络** | ✅ LwIP | ✅ BSD Socket | ✅ LwIP | ✅ BSD Socket | ✅ Winsock2 | ⚠️ WebSocket / Fetch |
| **文件系统** | ✅ cm_fs (VFS) | ✅ yopen_fs | ✅ SPIFFS / FATFS | ✅ POSIX | ✅ CRT / Win32 | ⚠️ MEMFS (内存文件系统) |
| **GPIO** | ✅ cm_gpio | ✅ yopen_gpio | ✅ gpio | ⚠️ sysfs | ⚠️ 模拟 | ❌ 不支持 |
| **I2C** | ✅ cm_i2c | ✅ yopen_i2c | ✅ i2c_driver | ⚠️ /dev/i2c-N | ⚠️ 模拟 | ❌ 不支持 |
| **SPI** | ✅ cm_spi | ✅ yopen_spi | ✅ spi_bus | ⚠️ /dev/spidev | ⚠️ 模拟 | ❌ 不支持 |
| **图形显示** | ❌ 无 | ❌ 无 | ✅ LVGL | ✅ LVGL (SDL) | ✅ LVGL (SDL) | ✅ LVGL (SDL2 + Canvas) |

---

## 头文件说明

```c
#include "iot.h"      // 基础平台接口 (OS, 内存, 日志, FS, Socket)
#include "iot_ext.h"  // 扩展平台接口 (GPIO, I2C, SPI)
```

### iot.h 接口分类

| 分类 | 接口前缀 | 说明 |
|------|----------|------|
| 内存管理 | `iot_malloc/iot_free` | 动态内存分配 |
| 互斥锁 | `iot_mutex_*` | 线程同步 |
| 信号量 | `iot_sem_*` | 线程同步 |
| 任务 | `iot_task_*` | 线程管理 |
| 定时器 | `iot_timer_*` | 定时器 |
| Socket | `iot_socket/iot_*` | 网络编程 |
| 文件系统 | `iot_fs_*` | 文件操作 |
| 标准输出 | `iot_puts/iot_printf` | 日志输出 |

---

## 1. 内存管理接口

### 类型和常量

| 类型/常量 | 说明 |
|-----------|------|
| `iot_malloc(size)` | 分配内存 |
| `iot_calloc(n, size)` | 分配并初始化内存 |
| `iot_realloc(ptr, size)` | 重新分配内存 |
| `iot_free(ptr)` | 释放内存 |
| `iot_strdup(str)` | 复制字符串 |

### 各平台实现

| 接口 | ML307N | YOPEN | ESP32/Linux | Windows |
|------|--------|-------|-------------|---------|
| `iot_malloc(size)` | `cm_malloc(size)` | `yopen_malloc(size)` | `malloc(size)` | `malloc(size)` |
| `iot_calloc(n, size)` | `cm_calloc(n, size)` | `yopen_calloc(n, size)` | `calloc(n, size)` | `calloc(n, size)` |
| `iot_realloc(ptr, size)` | `cm_realloc(ptr, size)` | `yopen_realloc(ptr, size)` | `realloc(ptr, size)` | `realloc(ptr, size)` |
| `iot_free(ptr)` | `cm_free(ptr)` | `yopen_free(ptr)` | `free(ptr)` | `free(ptr)` |
| `iot_strdup(str)` | `cm_strdup(str)` | `yopen_strdup(str)` | `strdup(str)` | `_strdup(str)` |

---

## 2. 互斥锁接口

### 类型定义

| 类型 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_mutex_t` | `osMutexId_t` | `yopen_mutex_t` | `SemaphoreHandle_t` | `pthread_mutex_t*` | `HANDLE` |

### 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_mutex_create()` | `osMutexNew()` | `yopen_mutex_create()` | `xSemaphoreCreateMutex()` | `pthread_mutex_init()` | `CreateMutex()` |
| `iot_mutex_lock(m, to)` | `osMutexAcquire(m, to)` | `yopen_mutex_lock(m, to)` | `xSemaphoreTake(m, to)` | `pthread_mutex_lock(m)` | `WaitForSingleObject(m, to)` |
| `iot_mutex_unlock(m)` | `osMutexRelease(m)` | `yopen_mutex_unlock(m)` | `xSemaphoreGive(m)` | `pthread_mutex_unlock(m)` | `ReleaseMutex(m)` |
| `iot_mutex_delete(m)` | `osMutexDelete(m)` | `yopen_mutex_delete(m)` | `vSemaphoreDelete(m)` | `pthread_mutex_destroy(m)` | `CloseHandle(m)` |

---

## 3. 信号量接口

### 类型定义

| 类型 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_sem_t` | `osSemaphoreId_t` | `yopen_sem_t` | `SemaphoreHandle_t` | `sem_t*` | `HANDLE` |

### 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_sem_create(max, init)` | `osSemaphoreNew(max, init, NULL)` | `yopen_sem_create(max, init)` | `xSemaphoreCreateCounting(max, init)` | `sem_open("/semph", ...)` | `CreateSemaphore()` |
| `iot_sem_wait(s)` | `osSemaphoreAcquire(s, IOT_WAIT_FOREVER)` | `yopen_sem_wait(s)` | `xSemaphoreTake(s, portMAX_DELAY)` | `sem_wait(s)` | `WaitForSingleObject(s, INFINITE)` |
| `iot_sem_wait_timeout(s, ms)` | `osSemaphoreAcquire(s, ms)` | `yopen_sem_wait_timeout(s, ms)` | `xSemaphoreTake(s, ms/portTICK_RATE_MS)` | `sem_trywait(s)` | `WaitForSingleObject(s, ms)` |
| `iot_sem_post(s)` | `osSemaphoreRelease(s)` | `yopen_sem_post(s)` | `xSemaphoreGive(s)` | `sem_post(s)` | `ReleaseSemaphore(s, 1, NULL)` |
| `iot_sem_get_count(s)` | 不支持 | `yopen_sem_get_count(s)` | `uxSemaphoreGetCount(s)` | `sem_getvalue(s)` | 固定返回 0 |
| `iot_sem_delete(s)` | `osSemaphoreDelete(s)` | `yopen_sem_delete(s)` | `vSemaphoreDelete(s)` | `sem_close(s)` | `CloseHandle(s)` |

---

## 4. 任务接口

### 类型和优先级定义

| 类型/常量 | ML307N | YOPEN | ESP32 | Linux | Windows |
|-----------|--------|-------|-------|-------|---------|
| `iot_task_t` | `osThreadId_t` | `yopen_task_t` | `TaskHandle_t` | `pthread_t*` | `HANDLE` |
| `iot_task_func_t` | `osThreadFunc_t` | `yopen_task_func_t` | `TaskFunction_t` | `void*(*)(void*)` | `DWORD(*)(LPVOID)` |
| `IOT_OS_PRIO_IDLE` | 最低 | 最低 | `tskIDLE_PRIORITY` | `sched_get_priority_min()` | `THREAD_PRIORITY_IDLE` |
| `IOT_OS_PRIO_NORMAL` | 正常 | 正常 | 正常 | 正常 | `THREAD_PRIORITY_NORMAL` |
| `IOT_OS_PRIO_HIGHEST` | 最高 | 最高 | `configMAX_PRIORITIES-1` | `sched_get_priority_max()` | `THREAD_PRIORITY_TIME_CRITICAL` |

### 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_task_create(name, func, arg, stack, prio)` | `osThreadNew(func, arg, &attr)` | `yopen_task_create(name, func, arg, prio)` | `xTaskCreate(func, name, stack, arg, prio, NULL)` | `pthread_create(...)` | `CreateThread(NULL, stack, func, arg, 0, &tid)` |
| `iot_task_delay(ms)` | `osDelay(ms)` | `yopen_sleep(ms)` | `vTaskDelay(ms/portTICK_RATE_MS)` | `usleep(ms*1000)` | `Sleep(ms)` |
| `iot_task_delete(t)` | `osThreadTerminate(t)` | `yopen_task_delete(t)` | `vTaskDelete(t)` | `pthread_cancel(t)` | `TerminateThread(t, 0)` |
| `iot_task_get_current()` | `osThreadGetId()` | `yopen_task_self()` | `xTaskGetCurrentTaskHandle()` | `pthread_self()` | `GetCurrentThread()` |
| `iot_get_tick_ms()` | `osKernelGetTickCount()` | `yopen_get_tick()` | `xTaskGetTickCount()*portTICK_RATE_MS` | `clock_gettime(CLOCK_MONOTONIC)` | `GetTickCount()` |
| `iot_get_tick_freq()` | 1000 | 1000 | 1000 | 1000 | 1000 |
| `iot_task_exit()` | `osThreadExit()` | `yopen_task_exit()` | `vTaskDelete(NULL)` | `pthread_exit(NULL)` | `ExitThread(0)` |

---

## 5. 定时器接口

### 类型和常量

| 类型/常量 | 说明 |
|-----------|------|
| `iot_timer_t` | 定时器句柄 |
| `iot_timer_callback_t` | 回调函数类型 `void (*)(void*)` |
| `IOT_TIMER_ONCE` | 单次定时器 |
| `IOT_TIMER_PERIODIC` | 周期定时器 |

### 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_timer_create(cb, arg, period, type)` | 内部实现 | 内部实现 | `xTimerCreate(cb, period, type, ...)` | `timer_create()` | `CreateWaitableTimer()` |
| `iot_timer_start(timer, period)` | 内部实现 | 内部实现 | `xTimerStart(timer, ...)` | `timer_settime()` | `SetWaitableTimer()` |
| `iot_timer_stop(timer)` | 内部实现 | 内部实现 | `xTimerStop(timer, ...)` | `timer_delete()` | `CancelWaitableTimer()` |
| `iot_timer_delete(timer)` | 内部实现 | 内部实现 | `xTimerDelete(timer, ...)` | `timer_delete()` | `CloseHandle()` |
| `iot_timer_is_running(timer)` | 内部实现 | 内部实现 | `xTimerIsTimerActive(timer)` | 检查状态 | 检查状态 |

---

## 6. Socket 网络接口

### 类型定义

| 类型 | 说明 |
|------|------|
| `iot_socket_t` | Socket 句柄 |
| `iot_sockaddr_t` | 地址结构 |
| `iot_sockaddr_in_t` | IPv4 地址结构 |
| `iot_socklen_t` | 地址长度类型 |

### Socket 类型常量

| 常量 | 值 | 说明 |
|------|-----|------|
| `IOT_SOCK_STREAM` | 1 | TCP 流式 Socket |
| `IOT_SOCK_DGRAM` | 2 | UDP 数据报 Socket |
| `IOT_SOCK_RAW` | 3 | 原始 Socket |

### Socket 选项常量

| 常量 | 说明 |
|------|------|
| `IOT_SO_REUSEADDR` | 地址重用 |
| `IOT_SO_KEEPALIVE` | 保持连接 |
| `IOT_SO_RCVBUF` | 接收缓冲区大小 |
| `IOT_SO_SNDBUF` | 发送缓冲区大小 |
| `IOT_TCP_NODELAY` | TCP 无延迟 |

### Socket 关闭方式

| 常量 | 说明 |
|------|------|
| `IOT_SHUT_RD` | 关闭读 |
| `IOT_SHUT_WR` | 关闭写 |
| `IOT_SHUT_RDWR` | 关闭读写 |

### Socket 接口

| 接口 | 说明 |
|------|------|
| **创建和关闭** | |
| `iot_socket(domain, type, protocol)` | 创建 Socket |
| `iot_socketpair(domain, type, protocol, sv)` | 创建成对 Socket |
| `iot_close(sock)` | 关闭 Socket |
| `iot_shutdown(sock, how)` | 关闭半连接 |
| **连接和数据传输** | |
| `iot_bind(sock, addr, len)` | 绑定地址 |
| `iot_listen(sock, backlog)` | 监听连接 |
| `iot_accept(sock, addr, len)` | 接受连接 |
| `iot_connect(sock, addr, len)` | 连接服务器 |
| `iot_send(sock, buf, len, flags)` | 发送数据 |
| `iot_sendto(sock, buf, len, flags, dest, destlen)` | 发送数据到地址 |
| `iot_recv(sock, buf, len, flags)` | 接收数据 |
| `iot_recvfrom(sock, buf, len, flags, src, srclen)` | 接收数据并获取地址 |
| **选项控制** | |
| `iot_setsockopt(sock, level, optname, optval, optlen)` | 设置选项 |
| `iot_getsockopt(sock, level, optname, optval, optlen)` | 获取选项 |
| `iot_getpeername(sock, addr, len)` | 获取对端地址 |
| `iot_getsockname(sock, addr, len)` | 获取本地地址 |
| **I/O 多路复用** | |
| `iot_select_fd_set_max` | Select 最大描述符数 |
| `iot_select(nfds, rfds, wfds, efds, timeout)` | I/O 多路复用 |
| `iot_fd_set/iot_fd_clr/iot_fd_isset/iot_fd_zero` | fd_set 操作 |
| **地址转换** | |
| `iot_inet_addr(cp)` | 点分十进制转二进制 |
| `iot_inet_aton(cp, inp)` | 点分十进制转 in_addr |
| `iot_inet_ntoa(addr)` | in_addr 转点分十进制 |
| `iot_inet_ntop(af, src, dst, size)` | 二进制转字符串 |
| `iot_inet_pton(af, src, dst)` | 字符串转二进制 |
| **DNS 和错误** | |
| `iot_gethostbyname(name)` | DNS 解析 |
| `iot_gethostbyaddr(addr, len, type)` | IP 反向解析 |
| `iot_socket_errno()` | 获取错误码 |

### 错误码映射

| 常量 | 说明 |
|------|------|
| `IOT_EAGAIN` | 操作将阻塞 |
| `IOT_EINPROGRESS` | 操作正在进行中 |
| `IOT_EWOULDBLOCK` | 操作将阻塞 |
| `IOT_ENOTCONN` | 未连接 |
| `IOT_ECONNRESET` | 连接被重置 |
| `IOT_ETIMEDOUT` | 连接超时 |
| `IOT_ECONNREFUSED` | 连接被拒绝 |
| `IOT_ENETUNREACH` | 网络不可达 |
| `IOT_EHOSTUNREACH` | 主机不可达 |

### Socket 初始化

| 平台 | 初始化/清理 |
|------|-------------|
| ML307N | 内部自动处理 |
| YOPEN | 内部自动处理 |
| ESP32 | `esp_netif_init()` |
| Linux | `socket()` 自动初始化 |
| Windows | `WSAStartup()` / `WSACleanup()` |

---

## 7. 文件系统接口

### 类型定义

| 类型 | 说明 |
|------|------|
| `iot_fs_file_t` | 文件句柄 |
| `iot_fs_dir_t` | 目录句柄 |
| `iot_fs_dirent_t` | 目录项 |
| `IOT_FS_MAX_PATH` | 路径最大长度 (255) |

### 目录项访问宏

| 宏 | 说明 |
|----|------|
| `iot_fs_dirent_name(dirent)` | 获取目录项名称 |
| `iot_fs_dirent_is_dir(dirent)` | 判断是否为目录 |

### 文件操作接口

| 接口 | 说明 |
|------|------|
| **打开和关闭** | |
| `iot_fs_open(path, mode)` | 打开文件 |
| `iot_fs_close(fd)` | 关闭文件 |
| **读写操作** | |
| `iot_fs_write(fd, buf, len)` | 写入数据 |
| `iot_fs_read(fd, buf, len)` | 读取数据 |
| `iot_fs_flush(fd)` | 刷新缓冲区 |
| **文件指针** | |
| `iot_fs_seek(fd, offset, whence)` | 移动文件指针 |
| `iot_fs_tell(fd)` | 获取当前偏移 |
| `iot_fs_rewind(fd)` | 重置到文件开头 |
| **文件信息** | |
| `iot_fs_size(fd)` | 获取文件大小 |
| `iot_fs_eof(fd)` | 检查是否到达文件末尾 |
| **目录操作** | |
| `iot_fs_opendir(path)` | 打开目录 |
| `iot_fs_closedir(dir)` | 关闭目录 |
| `iot_fs_readdir(dir)` | 读取目录项 |
| `iot_fs_mkdir(path)` | 创建目录 |
| `iot_fs_rmdir(path)` | 删除目录 |
| **文件管理** | |
| `iot_fs_remove(path)` | 删除文件 |
| `iot_fs_rename(old, new)` | 重命名文件 |
| `iot_fs_access(path, mode)` | 检查访问权限 |
| `iot_fs_file_exists(path)` | 检查文件是否存在 |
| `iot_fs_filesize(path)` | 获取文件大小 |

### 文件打开模式

| 模式 | 说明 |
|------|------|
| `"r"` | 只读，文件不存在则失败 |
| `"w"` | 只写，创建或截断文件 |
| `"a"` | 追加，创建或末尾添加 |
| `"r+"` | 读写，文件必须存在 |
| `"w+"` | 读写，创建或截断文件 |
| `"a+"` | 读写追加，创建或末尾添加 |

### 各平台实现

| 接口 | ML307N | YOPEN | ESP32/Linux | Windows |
|------|--------|-------|-------------|---------|
| `iot_fs_open` | `cm_fs_fopen` | `yopen_fopen` | `fopen` | `fopen` |
| `iot_fs_close` | `cm_fs_fclose` | `yopen_fclose` | `fclose` | `fclose` |
| `iot_fs_read` | `cm_fs_fread` | `yopen_fread` | `fread` | `fread` |
| `iot_fs_write` | `cm_fs_fwrite` | `yopen_fwrite` | `fwrite` | `fwrite` |
| `iot_fs_seek` | `cm_fs_fseek` | `yopen_fseek` | `fseek` | `fseek` |
| `iot_fs_remove` | `cm_fs_delete` | `yopen_remove` | `remove` | `remove` |
| `iot_fs_rename` | `cm_fs_move` | `yopen_frename` | `rename` | `rename` |
| `iot_fs_access` | `cm_fs_exist` | `yopen_file_exists` | `access` | `_access` |
| `iot_fs_mkdir` | `cm_fs_mkdir` | `yopen_mkdir` | `mkdir` | `_mkdir` |

---

## 8. 标准输出接口

### 接口

| 接口 | 说明 |
|------|------|
| `iot_puts(str)` | 打印字符串（不带换行） |
| `iot_printf(fmt, ...)` | 格式化打印（不带换行） |

### 各平台实现

| 接口 | ML307N | YOPEN | ESP32/Linux | Windows |
|------|--------|-------|-------------|---------|
| `iot_puts(str)` | `cm_log_printf(0, "%s", str)` | `yopen_trace("%s", str)` | `printf("%s", str)` | `printf("%s", str)` |
| `iot_printf(fmt, ...)` | `cm_log_printf(0, fmt, ...)` | `yopen_trace(fmt, ...)` | `printf(fmt, ...)` | `printf(fmt, ...)` |

---

## 9. 日志接口

### 日志级别定义 (iot_log.h)

| 级别 | 值 | 说明 |
|------|-----|------|
| `LOG_LEVEL_TRACE` | 0 | 跟踪级别（最详细） |
| `LOG_LEVEL_DEBUG` | 1 | 调试级别 |
| `LOG_LEVEL_INFO` | 2 | 信息级别 |
| `LOG_LEVEL_WARN` | 3 | 警告级别 |
| `LOG_LEVEL_ERROR` | 4 | 错误级别 |
| `LOG_LEVEL_NONE` | 5 | 关闭日志 |

### 日志宏定义

```c
// 宏定义（在 iot_log.h 中）
#define LOG_TRACE(fmt, ...) iot_log_printf(LOG_LEVEL_TRACE, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) iot_log_printf(LOG_LEVEL_DEBUG, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  iot_log_printf(LOG_LEVEL_INFO,  "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) iot_log_printf(LOG_LEVEL_WARN,  "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) iot_log_printf(LOG_LEVEL_ERROR, "[%s:%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
```

### 日志函数

| 函数 | 说明 |
|------|------|
| `iot_log_set_level(level)` | 设置日志级别 |
| `iot_log_get_level()` | 获取当前日志级别 |
| `iot_log_print(level, fmt, ...)` | 打印指定级别日志 |

### 输出格式

```
[LEVEL] [function:line] message
```

### 日志级别使用建议

| 级别 | 使用场景 |
|------|----------|
| `TRACE` | 频繁跟踪（如循环内、状态变化） |
| `DEBUG` | 调试信息（创建/销毁、配置） |
| `INFO` | 一般信息（连接成功、初始化完成） |
| `WARN` | 警告信息（参数错误、可恢复错误） |
| `ERROR` | 错误信息（分配失败、连接失败） |

---

## 10. GPIO 接口

### 常量定义

| 常量 | 说明 |
|------|------|
| `IOT_GPIO_DIR_INPUT` | 输入模式 |
| `IOT_GPIO_DIR_OUTPUT` | 输出模式 |
| `IOT_GPIO_LEVEL_LOW` | 低电平 |
| `IOT_GPIO_LEVEL_HIGH` | 高电平 |
| `IOT_GPIO_MAX_PIN` | 最大引脚数 (32) |

### 中断类型常量

| 常量 | ML307N | ESP32 |
|------|--------|-------|
| `IOT_GPIO_IRQ_RISING` | `CM_GPIO_IT_EDGE_RISING` | `GPIO_INTR_POSEDGE` |
| `IOT_GPIO_IRQ_FALLING` | `CM_GPIO_IT_EDGE_FALLING` | `GPIO_INTR_NEGEDGE` |
| `IOT_GPIO_IRQ_BOTH` | `CM_GPIO_IT_EDGE_BOTH` | `GPIO_INTR_ANYEDGE` |
| `IOT_GPIO_IRQ_HIGH` | `CM_GPIO_IT_LEVEL_HIGH` | `GPIO_INTR_HIGH_LEVEL` |
| `IOT_GPIO_IRQ_LOW` | `CM_GPIO_IT_LEVEL_LOW` | `GPIO_INTR_LOW_LEVEL` |

### 上下拉常量

| 常量 | ML307N | ESP32 |
|------|--------|-------|
| `IOT_GPIO_PULL_NONE` | `CM_GPIO_PULL_NONE` | `GPIO_FLOATING` |
| `IOT_GPIO_PULL_UP` | `CM_GPIO_PULL_UP` | `GPIO_PULLUP_ONLY` |
| `IOT_GPIO_PULL_DOWN` | `CM_GPIO_PULL_DOWN` | `GPIO_PULLDOWN_ONLY` |

### 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_gpio_init(pin, dir)` | `cm_gpio_init(pin, cfg)` | `yopen_gpio_init(pin, dir, ...)` | `gpio_config(cfg)` | `/sys/class/gpio` | 模拟表 |
| `iot_gpio_deinit(pin)` | `cm_gpio_deinit(pin)` | `yopen_gpio_deinit(pin)` | `gpio_reset_pin(pin)` | `/sys/class/gpio` | 清空表 |
| `iot_gpio_write(pin, lvl)` | `cm_gpio_set_level(pin, lvl)` | `yopen_gpio_set_level(pin, lvl)` | `gpio_set_level(pin, lvl)` | sysfs | 写表 |
| `iot_gpio_read(pin)` | `cm_gpio_get_level(pin)` | `yopen_gpio_get_level(pin)` | `gpio_get_level(pin)` | sysfs | 读表 |
| `iot_gpio_toggle(pin)` | 读取后翻转 | 不支持 | 读取后翻转 | sysfs | 读取后翻转 |
| `iot_gpio_register_irq(pin, mode, cb)` | `cm_gpio_interrupt_enable(...)` | 不支持 | `gpio_isr_handler_add(...)` | 不支持 | 不支持 |
| `iot_gpio_unregister_irq(pin)` | `cm_gpio_interrupt_disable(pin)` | 不支持 | `gpio_isr_handler_remove(pin)` | 不支持 | 不支持 |

---

## 11. I2C 接口

### 总线常量

| 常量 | 说明 |
|------|------|
| `IOT_I2C_BUS_0` | I2C 总线 0 |
| `IOT_I2C_BUS_1` | I2C 总线 1 |

### 地址类型常量

| 常量 | ML307N | ESP32 |
|------|--------|-------|
| `IOT_I2C_ADDR_7BIT` | `CM_I2C_ADDR_TYPE_7BIT` | `I2C_ADDR_BIT_7` |
| `IOT_I2C_ADDR_10BIT` | `CM_I2C_ADDR_TYPE_10BIT` | `I2C_ADDR_BIT_10` |

### 速度常量

| 常量 | 速度 |
|------|------|
| `IOT_I2C_CLK_100K` | 100 KHz |
| `IOT_I2C_CLK_400K` | 400 KHz |
| `IOT_I2C_CLK_1M` | 1 MHz |

### 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_i2c_init(bus, speed)` | `cm_i2c_open_v2(bus, cfg)` | `yopen_I2cInit(bus, speed)` | `i2c_param_config(bus, cfg)` | `/dev/i2c-N` | 模拟表 |
| `iot_i2c_deinit(fd)` | `cm_i2c_close(fd)` | `yopen_I2cRelease(bus)` | `i2c_driver_delete(fd)` | `close(fd)` | 清表 |
| `iot_i2c_write(fd, addr, reg, data, len)` | `cm_i2c_write_v2(...)` | `yopen_I2cWrite(...)` | `i2c_master_write()` | `write(fd, buf, len)` | 写表 |
| `iot_i2c_read(fd, addr, reg, data, len)` | `cm_i2c_read_v2(...)` | `yopen_I2cRead(...)` | `i2c_master_read()` | `read(fd, buf, len)` | 读表 |
| `iot_i2c_write_raw(fd, addr, data, len)` | 不带寄存器 | 不带寄存器 | 同上 | 同上 | 写表 |
| `iot_i2c_read_raw(fd, addr, data, len)` | 不带寄存器 | 不带寄存器 | 同上 | 同上 | 读表 |

---

## 12. SPI 接口

### 总线常量

| 常量 | 说明 |
|------|------|
| `IOT_SPI_BUS_0` | SPI 总线 0 |
| `IOT_SPI_BUS_1` | SPI 总线 1 |

### 模式常量

| 常量 | CPOL | CPHA | 说明 |
|------|------|------|------|
| `IOT_SPI_MODE_0` | 0 | 0 | 空闲低电平，上升沿采样 |
| `IOT_SPI_MODE_1` | 0 | 1 | 空闲低电平，下降沿采样 |
| `IOT_SPI_MODE_2` | 1 | 0 | 空闲高电平，下降沿采样 |
| `IOT_SPI_MODE_3` | 1 | 1 | 空闲高电平，上升沿采样 |

### 接口

| 接口 | ML307N | YOPEN | ESP32 | Linux | Windows |
|------|--------|-------|-------|-------|---------|
| `iot_spi_init(bus, cs, mode, speed)` | `cm_spi_open(bus, cfg)` | `yopen_spi_init(port, cfg)` | `spi_bus_add_device()` | `/dev/spidevN.N` | 模拟表 |
| `iot_spi_deinit(fd)` | `cm_spi_close(fd)` | `yopen_spi_deinit(port)` | `spi_bus_remove_device()` | `close(fd)` | 清表 |
| `iot_spi_write(fd, data, len)` | `cm_spi_write(fd, data, len)` | `yopen_spi_send(port, ...)` | `spi_device_transmit()` | `write(fd, data, len)` | 写表 |
| `iot_spi_read(fd, data, len)` | `cm_spi_read(fd, data, len)` | `yopen_spi_recv(port, ...)` | `spi_device_transmit()` | `read(fd, data, len)` | 读表 |
| `iot_spi_transfer(fd, tx, rx, len)` | `cm_spi_transfer(fd, tx, rx, len)` | `yopen_spi_transfer(port, ...)` | `spi_device_transmit()` | `ioctl(SPI_IOC_MESSAGE(1))` | 组合读写 |
| `iot_spi_transfer_raw(fd, tx, rx, len, speed)` | 不支持 | `yopen_spi_transfer(port, ...)` | `spi_device_transmit()` | `ioctl(...)` | 组合读写 |

---

## 代码示例

### 互斥锁使用

```c
iot_mutex_t mutex = iot_mutex_create();
if (mutex == NULL) {
    LOG_ERROR("Failed to create mutex");
    return -1;
}

if (iot_mutex_lock(mutex, 5000)) {
    // 临界区代码
    iot_mutex_unlock(mutex);
} else {
    LOG_ERROR("Failed to lock mutex");
}

iot_mutex_delete(mutex);
```

### 信号量使用

```c
iot_sem_t sem = iot_sem_create(10, 0);
if (sem == NULL) {
    LOG_ERROR("Failed to create semaphore");
    return -1;
}

// 等待信号量（5秒超时）
if (iot_sem_wait_timeout(sem, 5000)) {
    LOG_INFO("Got semaphore");
} else {
    LOG_WARN("Timeout waiting for semaphore");
}

// 释放信号量
iot_sem_post(sem);

iot_sem_delete(sem);
```

### Socket 客户端

```c
iot_socket_t sock = iot_socket(AF_INET, IOT_SOCK_STREAM, 0);
if (sock == IOT_SOCKET_INVALID) {
    LOG_ERROR("Failed to create socket");
    return -1;
}

struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(8080);
addr.sin_addr.s_addr = inet_addr("192.168.1.100");

if (iot_connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    LOG_ERROR("Failed to connect: %d", iot_socket_errno());
    iot_close(sock);
    return -1;
}

LOG_INFO("Connected to server");

char buf[1024];
int len = iot_recv(sock, buf, sizeof(buf), 0);
if (len > 0) {
    LOG_DEBUG("Received %d bytes", len);
}

iot_close(sock);
```

### 文件读写

```c
iot_fs_file_t f = iot_fs_open("test.txt", "w");
if (f == NULL) {
    LOG_ERROR("Failed to open file");
    return -1;
}

const char* data = "Hello, IoT-OS!";
iot_fs_write(f, data, strlen(data));
iot_fs_close(f);

// 读取文件
f = iot_fs_open("test.txt", "r");
if (f) {
    char buf[256];
    int len = iot_fs_read(f, buf, sizeof(buf) - 1);
    if (len > 0) {
        buf[len] = '\0';
        LOG_INFO("Read: %s", buf);
    }
    iot_fs_close(f);
}
```

---

## 13. WASM 平台专用模块

WASM 平台额外提供浏览器环境专用的 Lua 模块，位于 `platforms/wasm/modules/`：

### 模块列表

| 模块 | 文件 | 说明 |
|------|------|------|
| `websocket` | `websocket.c` | WebSocket 客户端 |
| `fetch` | `fetch.c` | HTTP 请求 (Fetch API) |
| `storage` | `storage.c` | IndexedDB 持久化存储 |
| `localStorage` | `localStorage.c` | 浏览器本地存储 |
| `device` | `device.c` | 设备/浏览器信息 |
| `canvas` | `canvas.c` | HTML5 Canvas 2D 绘图 |

### Lua API 示例

#### WebSocket

```lua
local ws = websocket.create("wss://echo.websocket.org", {
    on_open = function()
        print("WebSocket connected")
    end,
    on_message = function(data)
        print("Received:", data)
    end,
    on_close = function(code, reason)
        print("Closed:", code, reason)
    end,
    on_error = function(msg)
        print("Error:", msg)
    end
})
websocket.send(ws, "hello")
websocket.close(ws)
```

#### Fetch

```lua
-- 同步 GET
local resp = fetch.get("https://api.example.com/data")
print("Status:", resp.status)
print("Body:", resp.body)

-- POST
local resp = fetch.post("https://api.example.com/submit",
    '{"key":"value"}',
    "application/json")
```

#### localStorage

```lua
localStorage.set("username", "test")
localStorage.set("score", tostring(100))

local name = localStorage.get("username")
local score = tonumber(localStorage.get("score"))

localStorage.remove("username")
local keys = localStorage.keys()
local count = localStorage.length()
```

#### device

```lua
local info = device.info()
print("Browser:", info.browser, info.version)
print("Language:", info.language)
print("Platform:", info.platform)
print("Screen:", info.screenWidth, "x", info.screenHeight)
print("Device type:", info.deviceType)  -- desktop / mobile / tablet
```

#### storage (IndexedDB)

```lua
local db = storage.open("mydb", "1.0")
storage.set(db, "user", '{"name":"test","age":25}')

local data = storage.get(db, "user")
print(data)

storage.remove(db, "user")
storage.close(db)
```

#### canvas

```lua
local canvas = canvas.create(320, 240)
local ctx = canvas.getContext()

ctx:clearRect(0, 0, 320, 240)
ctx:setFillStyle("#FF0000")
ctx:fillRect(10, 10, 100, 50)
ctx:setStrokeStyle("#00FF00")
ctx:setLineWidth(2)
ctx:strokeRect(10, 10, 100, 50)
ctx:setFont("16px Arial")
ctx:fillText("Hello", 50, 30)

-- 路径绘图
ctx:beginPath()
ctx:moveTo(0, 0)
ctx:lineTo(100, 100)
ctx:closePath()
ctx:stroke()

-- 圆弧
ctx:beginPath()
ctx:arc(100, 100, 50, 0, math.pi * 2)
ctx:fill()
```
