/**
 * @file iot.h
 * @brief ML307N 平台适配头文件
 *
 * 本文件提供跨平台统一接口，接口顺序为：
 * 1. 内存管理 (mem)
 * 2. 互斥锁 (mutex)
 * 3. 信号量 (semaphore)
 * 4. 任务 (task)
 * 5. 定时器 (timer)
 * 6. Socket 网络接口 (socket)
 * 7. 文件系统接口 (fs)
 * 8. 标准输出接口 (stdout)
 */
#ifndef IOT_PLATFORM_ML307N_H
#define IOT_PLATFORM_ML307N_H

/*===========================================================
 * 平台依赖头文件
 *===========================================================*/

#include "cm_os.h"
#include "cm_sys.h"
#include "cm_mem.h"
#include "cm_fs.h"
#include <stdint.h>
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/netdb.h"
#include "lwip/errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 1. 内存管理接口 (mem)
 *===========================================================*/

/**
 * @brief 分配内存（按4字节对齐）
 * @param size 要分配的字节数
 * @return 指向分配内存的指针，失败返回 NULL
 */
#define iot_malloc(size) \
    cm_malloc((size_t)(size))

/**
 * @brief 分配并初始化内存（置零，按4字节对齐）
 * @param nmemb 元素个数
 * @param size 每个元素的大小
 * @return 指向分配内存的指针，失败返回 NULL
 */
#define iot_calloc(nmemb, size) \
    cm_calloc((size_t)(nmemb), (size_t)(size))

/**
 * @brief 重新分配内存（按4字节对齐）
 * @param ptr 原内存指针
 * @param size 新大小
 * @return 指向重新分配内存的指针，失败返回 NULL
 */
#define iot_realloc(ptr, size) \
    cm_realloc((void *)(ptr), (size_t)(size))

/**
 * @brief 释放内存
 * @param ptr 要释放的内存指针
 */
#define iot_free(ptr) \
    cm_free((void *)(ptr))

/**
 * @brief 复制字符串（分配新内存）
 * @param str 要复制的源字符串
 * @return 指向新字符串的指针，失败返回 NULL
 */
#define iot_strdup(str) ({ \
    const char* _s = (str); \
    if (!_s) NULL; \
    else { \
        size_t _len = strlen(_s) + 1; \
        char* _d = (char*)cm_malloc(_len); \
        if (_d) memcpy(_d, _s, _len); \
        _d; \
    } \
})

/*===========================================================
 * 2. 互斥锁接口 (mutex)
 *===========================================================*/

/**
 * @brief 互斥锁类型
 */
#define iot_mutex_t              osMutexId_t

/**
 * @brief 创建互斥锁
 * @return 互斥锁句柄，失败返回 NULL
 */
#define iot_mutex_create() \
    osMutexNew(NULL)

/**
 * @brief 锁定互斥锁
 * @param mutex 互斥锁句柄
 * @param timeout_ms 超时时间（毫秒），osWaitForever 表示永久等待
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_mutex_lock(mutex, timeout_ms) \
    osMutexAcquire((mutex), (timeout_ms))

/**
 * @brief 解锁互斥锁
 * @param mutex 互斥锁句柄
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_mutex_unlock(mutex) \
    osMutexRelease((mutex))

/**
 * @brief 删除互斥锁
 * @param mutex 互斥锁句柄
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_mutex_delete(mutex) \
    osMutexDelete((mutex))

/*===========================================================
 * 3. 信号量接口 (semaphore)
 *===========================================================*/

/**
 * @brief 信号量类型
 */
#define iot_sem_t                osSemaphoreId_t

/**
 * @brief 创建信号量
 * @param max_count 信号量最大值
 * @param initial_count 信号量初始值
 * @return 信号量句柄，失败返回 NULL
 */
#define iot_sem_create(max_count, initial_count) \
    osSemaphoreNew((max_count), (initial_count), NULL)

/**
 * @brief 等待信号量（永久等待）
 * @param sem 信号量句柄
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_sem_wait(sem) \
    osSemaphoreAcquire((sem), osWaitForever)

/**
 * @brief 等待信号量（带超时）
 * @param sem 信号量句柄
 * @param timeout_ms 超时时间（毫秒）
 * @return 成功返回 osOK，超时或失败返回错误码
 */
#define iot_sem_wait_timeout(sem, timeout_ms) \
    osSemaphoreAcquire((sem), (timeout_ms))

/**
 * @brief 释放信号量（+1）
 * @param sem 信号量句柄
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_sem_post(sem) \
    osSemaphoreRelease((sem))

/**
 * @brief 获取信号量当前计数
 * @param sem 信号量句柄
 * @return 信号量计数
 */
#define iot_sem_get_count(sem) \
    osSemaphoreGetCount((sem))

/**
 * @brief 删除信号量
 * @param sem 信号量句柄
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_sem_delete(sem) \
    osSemaphoreDelete((sem))

/*===========================================================
 * 4. 任务接口 (task)
 *===========================================================*/

/**
 * @brief 任务类型（线程句柄）
 */
#define iot_task_t               osThreadId_t

/**
 * @brief 任务优先级类型
 */
#define iot_osPriority_t         osPriority_t

/**
 * @brief 任务函数类型
 */
#define iot_task_func_t          osThreadFunc_t

/* 任务优先级定义（CMSIS-RTOS2 优先级，值越小优先级越低） */
#define IOT_OS_PRIO_IDLE         osPriorityIdle     /**< 空闲优先级 */
#define IOT_OS_PRIO_LOW          osPriorityLow      /**< 低优先级 */
#define IOT_OS_PRIO_LOWEST       osPriorityLowest   /**< 次低优先级 */
#define IOT_OS_PRIO_BELOW_NORMAL osPriorityBelowNormal /**< 低于正常优先级 */
#define IOT_OS_PRIO_NORMAL       osPriorityNormal   /**< 正常优先级 */
#define IOT_OS_PRIO_ABOVE_NORMAL osPriorityAboveNormal /**< 高于正常优先级 */
#define IOT_OS_PRIO_HIGH         osPriorityHigh     /**< 高优先级 */
#define IOT_OS_PRIO_HIGHEST      osPriorityHighest  /**< 最高优先级 */
#define IOT_OS_PRIO_ERROR        osPriorityError    /**< 错误优先级 */

/**
 * @brief 创建任务（线程）
 * @param name 任务名称
 * @param func 任务函数
 * @param arg 传递给任务的参数
 * @param stack_size 栈大小（字节）
 * @param priority 任务优先级
 * @return 任务句柄，失败返回 NULL
 */
#define iot_task_create(name, func, arg, stack_size, priority) \
    osThreadNew((func), (arg), \
        &(const osThreadAttr_t){ \
            .name = (name), \
            .stack_size = (stack_size), \
            .priority = (priority) \
        })

/**
 * @brief 任务延时
 * @param ms 延时时间（毫秒）
 */
#define iot_task_delay(ms) \
    osDelay((ms) * 1000 / 1000)

/**
 * @brief 删除任务
 * @param task 任务句柄
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_task_delete(task) \
    osThreadTerminate((task))

/**
 * @brief 获取当前任务句柄
 * @return 当前任务句柄
 */
#define iot_task_get_current() \
    osThreadGetId()

/**
 * @brief 获取系统运行时间
 * @return 系统运行时间（毫秒）
 */
#define iot_get_tick_ms() \
    osKernelGetTickCount()

/**
 * @brief 获取系统时钟频率
 * @return 时钟频率（Hz）
 */
#define iot_get_tick_freq() \
    osKernelGetTickFreq()

/**
 * @brief 退出当前任务
 */
#define iot_task_exit() \
    osThreadExit()

/*===========================================================
 * 5. 定时器接口 (timer)
 *===========================================================*/

/**
 * @brief 定时器类型
 */
#define iot_timer_t              osTimerId_t

/**
 * @brief 定时器类型枚举
 */
#define iot_timer_type_t         osTimerType_t
#define IOT_TIMER_ONCE           osTimerOnce     /**< 单次定时器 */
#define IOT_TIMER_PERIODIC       osTimerPeriodic /**< 周期定时器 */

/**
 * @brief 定时器回调函数类型
 */
#define iot_timer_callback_t     osTimerFunc_t

/**
 * @brief 创建定时器
 * @param cb 定时器回调函数
 * @param arg 传递给回调的参数
 * @param period_ms 定时周期（毫秒）
 * @param type 定时器类型（IOT_TIMER_ONCE 或 IOT_TIMER_PERIODIC）
 * @return 定时器句柄，失败返回 NULL
 */
#define iot_timer_create(cb, arg, period_ms, type) \
    osTimerNew((cb), (type), (arg), NULL)

/**
 * @brief 启动定时器
 * @param timer 定时器句柄
 * @param period_ms 定时周期（毫秒）
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_timer_start(timer, period_ms) \
    osTimerStart((timer), (period_ms))

/**
 * @brief 停止定时器
 * @param timer 定时器句柄
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_timer_stop(timer) \
    osTimerStop((timer))

/**
 * @brief 删除定时器
 * @param timer 定时器句柄
 * @return 成功返回 osOK，失败返回错误码
 */
#define iot_timer_delete(timer) \
    osTimerDelete((timer))

/**
 * @brief 检查定时器是否运行
 * @param timer 定时器句柄
 * @return 运行返回 true，停止返回 false
 */
#define iot_timer_is_running(timer) \
    osTimerIsRunning((timer))

/*===========================================================
 * 6. Socket 网络接口 (socket)
 *===========================================================*/

/**
 * @brief Socket 句柄类型
 */
#define iot_socket_t              int

/**
 * @brief 无效 Socket 值
 */
#define IOT_SOCKET_INVALID        (-1)

/* Socket 类型 */
#define IOT_SOCK_STREAM           SOCK_STREAM       /**< TCP 流式 Socket */
#define IOT_SOCK_DGRAM            SOCK_DGRAM        /**< UDP 数据报 Socket */
#define IOT_SOCK_RAW              SOCK_RAW          /**< 原始 Socket */

/* Socket 协议 */
#define IOT_IPPROTO_TCP           IPPROTO_TCP       /**< TCP 协议 */
#define IOT_IPPROTO_UDP           IPPROTO_UDP       /**< UDP 协议 */
#define IOT_IPPROTO_IP            IPPROTO_IP        /**< IP 协议 */

/* Socket 选项级别 */
#define IOT_SOL_SOCKET            SOL_SOCKET        /**< 套接字级别选项 */

/* Socket 选项 */
#define IOT_SO_REUSEADDR          SO_REUSEADDR      /**< 地址重用 */
#define IOT_SO_KEEPALIVE          SO_KEEPALIVE      /**< 保持连接 */
#define IOT_SO_RCVBUF             SO_RCVBUF         /**< 接收缓冲区大小 */
#define IOT_SO_SNDBUF             SO_SNDBUF         /**< 发送缓冲区大小 */
#define IOT_SO_ERROR              SO_ERROR          /**< 获取错误码 */
#define IOT_TCP_NODELAY           TCP_NODELAY       /**< TCP 无延迟 */

/* Socket 关闭方式 */
#define IOT_SHUT_RD               SHUT_RD           /**< 关闭读 */
#define IOT_SHUT_WR               SHUT_WR           /**< 关闭写 */
#define IOT_SHUT_RDWR             SHUT_RDWR         /**< 关闭读写 */

/* Socket 地址结构 */
#define iot_sockaddr_t            struct sockaddr
#define iot_sockaddr_in_t         struct sockaddr_in
#define iot_sockaddr_in6_t        struct sockaddr_in6
#define iot_in_addr_t             struct in_addr
#define iot_in6_addr_t            struct in6_addr
#define iot_socklen_t             socklen_t

/* fd_set 相关类型 */
#define iot_fd_set_t              fd_set
#define iot_select_fd_set_max     FD_SETSIZE

/**
 * @brief Socket 初始化
 * @return 成功返回 0，失败返回错误码
 */
static inline int iot_socket_init(void) {
    return 0;
}

/**
 * @brief Socket 反初始化
 */
static inline void iot_socket_deinit(void) {
}

/**
 * @brief 设置 socket 为非阻塞模式（供 net 模块使用）
 * @param fd socket 句柄
 * @return 成功返回 0，失败返回 -1
 */
int net_port_set_nonblocking(iot_socket_t fd);

/**
 * @brief 创建 Socket
 * @param domain 协议域（AF_INET, AF_INET6 等）
 * @param type Socket 类型
 * @param protocol 协议类型
 * @return Socket 句柄，失败返回 IOT_SOCKET_INVALID
 */
#define iot_socket(domain, type, protocol) \
    socket((domain), (type), (protocol))

/**
 * @brief 创建成对连接的 Socket
 * @param domain 协议域
 * @param type Socket 类型
 * @param protocol 协议类型
 * @param sv 用于存储两个 Socket 句柄的数组
 * @return 成功返回 0，失败返回 -1
 */
#define iot_socketpair(domain, type, protocol, sv) \
    socketpair((domain), (type), (protocol), (sv))

/**
 * @brief 关闭 Socket
 * @param sock Socket 句柄
 * @return 成功返回 0，失败返回 -1
 */
#define iot_close(sock) \
    close((sock))

/**
 * @brief 关闭 Socket 的读/写半连接
 * @param sock Socket 句柄
 * @param how 关闭方式（IOT_SHUT_RD/IOT_SHUT_WR/IOT_SHUT_RDWR）
 * @return 成功返回 0，失败返回 -1
 */
#define iot_shutdown(sock, how) \
    shutdown((sock), (how))

/**
 * @brief 绑定 Socket 到地址
 * @param sock Socket 句柄
 * @param addr 地址结构指针
 * @param addrlen 地址结构长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_bind(sock, addr, addrlen) \
    bind((sock), (const struct sockaddr*)(addr), (addrlen))

/**
 * @brief 监听 Socket 连接
 * @param sock Socket 句柄
 * @param backlog 最大连接队列长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_listen(sock, backlog) \
    listen((sock), (backlog))

/**
 * @brief 接受连接请求
 * @param sock 监听 Socket 句柄
 * @param addr 客户端地址（可为 NULL）
 * @param addrlen 地址长度（可为 NULL）
 * @return 连接 Socket 句柄，失败返回 IOT_SOCKET_INVALID
 */
#define iot_accept(sock, addr, addrlen) \
    accept((sock), (struct sockaddr*)(addr), (socklen_t*)(addrlen))

/**
 * @brief 连接远程服务器
 * @param sock Socket 句柄
 * @param addr 服务器地址
 * @param addrlen 地址长度
 * @return 成功返回 0，失败返回 -1（非阻塞下可能返回 IOT_EINPROGRESS）
 */
#define iot_connect(sock, addr, addrlen) \
    connect((sock), (const struct sockaddr*)(addr), (addrlen))

/**
 * @brief 发送数据
 * @param sock Socket 句柄
 * @param buf 数据缓冲区
 * @param len 数据长度
 * @param flags 发送标志（通常为 0）
 * @return 成功返回发送的字节数，失败返回 -1
 */
#define iot_send(sock, buf, len, flags) \
    send((sock), (const void*)(buf), (len), (flags))

/**
 * @brief 发送数据到指定地址
 * @param sock Socket 句柄
 * @param buf 数据缓冲区
 * @param len 数据长度
 * @param flags 发送标志（通常为 0）
 * @param dest_addr 目标地址
 * @param dest_len 地址长度
 * @return 成功返回发送的字节数，失败返回 -1
 */
#define iot_sendto(sock, buf, len, flags, dest_addr, dest_len) \
    sendto((sock), (const void*)(buf), (len), (flags), \
        (const struct sockaddr*)(dest_addr), (dest_len))

/**
 * @brief 接收数据
 * @param sock Socket 句柄
 * @param buf 数据缓冲区
 * @param len 缓冲区长度
 * @param flags 接收标志（通常为 0）
 * @return 成功返回接收的字节数，失败返回 -1，连接关闭返回 0
 */
#define iot_recv(sock, buf, len, flags) \
    recv((sock), (void*)(buf), (len), (flags))

/**
 * @brief 接收数据并获取发送方地址
 * @param sock Socket 句柄
 * @param buf 数据缓冲区
 * @param len 缓冲区长度
 * @param flags 接收标志（通常为 0）
 * @param src_addr 发送方地址（可为 NULL）
 * @param src_len 地址长度（可为 NULL）
 * @return 成功返回接收的字节数，失败返回 -1，连接关闭返回 0
 */
#define iot_recvfrom(sock, buf, len, flags, src_addr, src_len) \
    recvfrom((sock), (void*)(buf), (len), (flags), \
        (struct sockaddr*)(src_addr), (socklen_t*)(src_len))

/**
 * @brief 设置 Socket 选项
 * @param sock Socket 句柄
 * @param level 选项级别
 * @param optname 选项名称
 * @param optval 选项值
 * @param optlen 选项长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_setsockopt(sock, level, optname, optval, optlen) \
    setsockopt((sock), (level), (optname), (const void*)(optval), (optlen))

/**
 * @brief 获取 Socket 选项
 * @param sock Socket 句柄
 * @param level 选项级别
 * @param optname 选项名称
 * @param optval 选项值
 * @param optlen 选项长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_getsockopt(sock, level, optname, optval, optlen) \
    getsockopt((sock), (level), (optname), (void*)(optval), (socklen_t*)(optlen))

/**
 * @brief 获取连接对端地址
 * @param sock Socket 句柄
 * @param addr 地址结构
 * @param addrlen 地址长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_getpeername(sock, addr, addrlen) \
    getpeername((sock), (struct sockaddr*)(addr), (socklen_t*)(addrlen))

/**
 * @brief 获取本地地址
 * @param sock Socket 句柄
 * @param addr 地址结构
 * @param addrlen 地址长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_getsockname(sock, addr, addrlen) \
    getsockname((sock), (struct sockaddr*)(addr), (socklen_t*)(addrlen))

/**
 * @brief 将文件描述符加入 fd_set
 */
#define iot_fd_set(fd, set) \
    FD_SET((fd), (set))

/**
 * @brief 将文件描述符从 fd_set 移除
 */
#define iot_fd_clr(fd, set) \
    FD_CLR((fd), (set))

/**
 * @brief 检查文件描述符是否在 fd_set 中
 */
#define iot_fd_isset(fd, set) \
    FD_ISSET((fd), (set))

/**
 * @brief 清空 fd_set
 */
#define iot_fd_zero(set) \
    FD_ZERO((set))

/**
 * @brief select 多路复用
 * @param nfds 最大的文件描述符 +1
 * @param readfds 读文件描述符集
 * @param writefds 写文件描述符集
 * @param exceptfds 异常文件描述符集
 * @param timeout 超时时间
 * @return 就绪的文件描述符数量，失败返回 -1，超时返回 0
 */
#define iot_select(nfds, readfds, writefds, exceptfds, timeout) \
    select((nfds), (readfds), (writefds), (exceptfds), (timeout))

/**
 * @brief 字符串转 IP 地址（IPv4）
 * @param cp 点分十进制字符串
 * @return IP 地址（网络字节序）
 */
#define iot_inet_addr(cp) \
    inet_addr((cp))

/**
 * @brief 字符串转 IP 地址结构（IPv4）
 * @param cp 点分十进制字符串
 * @param inp IP 地址结构
 * @return 成功返回 1，失败返回 0
 */
#define iot_inet_aton(cp, inp) \
    inet_aton((cp), (inp))

/**
 * @brief IP 地址转字符串（IPv4）
 * @param addr IP 地址
 * @return 点分十进制字符串
 */
#define iot_inet_ntoa(addr) \
    inet_ntoa((addr))

/**
 * @brief 二进制转字符串 IP 地址
 * @param af 地址族（AF_INET, AF_INET6）
 * @param src 源地址
 * @param dst 目标字符串
 * @param size 目标字符串大小
 * @return 成功返回 dst，失败返回 NULL
 */
#define iot_inet_ntop(af, src, dst, size) \
    inet_ntop((af), (const void*)(src), (dst), (size))

/**
 * @brief 字符串转二进制 IP 地址
 * @param af 地址族（AF_INET, AF_INET6）
 * @param src 源字符串
 * @param dst 目标地址
 * @return 成功返回 1，失败返回 0
 */
#define iot_inet_pton(af, src, dst) \
    inet_pton((af), (src), (void*)(dst))

/**
 * @brief 通过主机名获取 IP 地址
 * @param name 主机名
 * @return 主机信息结构，失败返回 NULL
 */
#define iot_gethostbyname(name) \
    gethostbyname((name))

/**
 * @brief 通过 IP 地址获取主机信息
 * @param addr IP 地址
 * @param len 地址长度
 * @param type 地址类型
 * @return 主机信息结构，失败返回 NULL
 */
#define iot_gethostbyaddr(addr, len, type) \
    gethostbyaddr((const char*)(addr), (len), (type))

/**
 * @brief 获取 Socket 错误码
 * @return 错误码
 */
#define iot_socket_errno() \
    errno

/**
 * @brief 获取 Socket 错误码
 * @return 错误码
 */
#define iot_socket_error() \
    errno

/* Socket 错误码映射 */
#define IOT_EAGAIN                EAGAIN              /**< 资源暂时不可用 */
#define IOT_EWOULDBLOCK           EWOULDBLOCK         /**< 操作会阻塞 */
#define IOT_EINPROGRESS           EINPROGRESS         /**< 操作正在进行 */
#define IOT_EALREADY              EALREADY            /**< 操作已在进行 */
#define IOT_ENOTSOCK              ENOTSOCK            /**< 不是 Socket */
#define IOT_EDESTADDRREQ          EDESTADDRREQ        /**< 缺少目标地址 */
#define IOT_EMSGSIZE              EMSGSIZE            /**< 消息过长 */
#define IOT_EPROTOTYPE            EPROTOTYPE          /**< 协议类型错误 */
#define IOT_ENOPROTOOPT           ENOPROTOOPT         /**< 协议选项不可用 */
#define IOT_EPROTONOSUPPORT       EPROTONOSUPPORT     /**< 协议不支持 */
#define IOT_ESOCKTNOSUPPORT       ESOCKTNOSUPPORT     /**< Socket 类型不支持 */
#define IOT_EOPNOTSUPP            EOPNOTSUPP          /**< 操作不支持 */
#define IOT_EPFNOSUPPORT          EPFNOSUPPORT        /**< 协议族不支持 */
#define IOT_EAFNOSUPPORT          EAFNOSUPPORT        /**< 地址族不支持 */
#define IOT_EADDRINUSE            EADDRINUSE          /**< 地址已在使用 */
#define IOT_EADDRNOTAVAIL         EADDRNOTAVAIL       /**< 地址不可用 */
#define IOT_ENETDOWN              ENETDOWN            /**< 网络已关闭 */
#define IOT_ENETUNREACH           ENETUNREACH         /**< 网络不可达 */
#define IOT_ENETRESET             ENETRESET           /**< 网络重置 */
#define IOT_ECONNABORTED          ECONNABORTED        /**< 连接中止 */
#define IOT_ECONNRESET            ECONNRESET          /**< 连接重置 */
#define IOT_ENOBUFS               ENOBUFS             /**< 缓冲区空间不足 */
#define IOT_EISCONN               EISCONN             /**< 已连接 */
#define IOT_ENOTCONN              ENOTCONN            /**< 未连接 */
#define IOT_ESHUTDOWN             ESHUTDOWN           /**< 已关闭 */
#define IOT_ETIMEDOUT             ETIMEDOUT           /**< 连接超时 */
#define IOT_ECONNREFUSED          ECONNREFUSED        /**< 连接被拒绝 */
#define IOT_EHOSTDOWN             EHOSTDOWN           /**< 主机已关闭 */
#define IOT_EHOSTUNREACH          EHOSTUNREACH        /**< 主机不可达 */
#define IOT_ENONET                ENONET              /**< 机器不在网络中 */

/*===========================================================
 * 日志适配层
 *===========================================================*/

/* LOG_INFO 宏定义已移至 iot_log.h */

/*===========================================================
 * 8. 标准输出接口 (stdout)
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
 * 7. 文件系统接口 (fs)
 *===========================================================*/

/**
 * @brief 文件系统路径最大长度
 */
#define IOT_FS_MAX_PATH          255

/**
 * @brief 文件句柄类型
 */
#define iot_fs_file_t            cm_fs_t

/**
 * @brief 目录句柄类型
 */
#define iot_fs_dir_t             uint32_t

/**
 * @brief 目录项类型
 */
#define iot_fs_dirent_t          cm_fs_file_data_t

/**
 * @brief 打开文件
 * @param path 文件路径
 * @param mode 打开模式
 * @return 文件句柄，失败返回 NULL
 */
#define iot_fs_open(path, mode) \
    cm_fs_open((path), (mode))

/**
 * @brief 关闭文件
 * @param fp 文件句柄
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_close(fp) \
    cm_fs_close((fp))

/**
 * @brief 读取文件
 * @param fp 文件句柄
 * @param buf 数据缓冲区
 * @param size 读取大小
 * @return 成功返回读取的字节数，失败返回 -1
 */
#define iot_fs_read(fp, buf, size) \
    cm_fs_read((fp), (buf), (size))

/**
 * @brief 写入文件
 * @param fp 文件句柄
 * @param buf 数据缓冲区
 * @param size 写入大小
 * @return 成功返回写入的字节数，失败返回 -1
 */
#define iot_fs_write(fp, buf, size) \
    cm_fs_write((fp), (buf), (size))

/**
 * @brief 文件定位
 * @param fp 文件句柄
 * @param offset 偏移量
 * @param whence 定位方式
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_seek(fp, offset, whence) \
    cm_fs_seek((fp), (offset), (whence))

/**
 * @brief 同步文件
 * @param fp 文件句柄
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_sync(fp) \
    cm_fs_sync((fp))

/**
 * @brief 创建目录
 * @param path 目录路径
 * @param mode 权限模式
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_mkdir(path, mode) \
    cm_fs_mkdir((path))

/**
 * @brief 删除文件
 * @param path 文件路径
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_remove(path) \
    cm_fs_delete((path))

/**
 * @brief 重命名文件
 * @param oldpath 原文件路径
 * @param newpath 新文件路径
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_rename(oldpath, newpath) \
    cm_fs_move((oldpath), (newpath))

/**
 * @brief 检查文件是否存在
 * @param path 文件路径
 * @param mode 检查模式
 * @return 存在返回非0，不存在返回 0
 */
#define iot_fs_access(path, mode) \
    cm_fs_exist((path))

/**
 * @brief 检查文件是否存在
 * @param path 文件路径
 * @return 存在返回非0，不存在返回 0
 */
#define iot_fs_file_exists(path) \
    cm_fs_exist((path))

/**
 * @brief 获取文件大小
 * @param path 文件路径
 * @return 文件大小，失败返回 -1
 */
#define iot_fs_filesize(path) \
    cm_fs_filesize((path))

/**
 * @brief 重置文件位置到开头
 * @param fp 文件句柄
 */
#define iot_fs_rewind(fp) \
    cm_fs_seek((fp), 0, CM_FS_SEEK_SET)

/**
 * @brief 截断文件
 * @param fd 文件描述符
 * @param length 截断长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_ftruncate(fd, length) \
    ((int)-1)

/**
 * @brief 递归删除目录
 * @param path 目录路径
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_rmdir_recursive(path) \
    ((int)-1)

/**
 * @brief 删除目录
 * @param path 目录路径
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_rmdir(path) \
    cm_fs_rmdir((path))

/**
 * @brief 获取文件系统信息
 * @param info 文件系统信息结构
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_getinfo(info) \
    cm_fs_getinfo((info))

/**
 * @brief 查找第一个文件
 * @param path 查找路径
 * @param file_data 文件数据
 * @return 查找句柄，失败返回 -1
 */
#define iot_fs_find_first(path, file_data) \
    cm_fs_find_first((path), (file_data))

/**
 * @brief 查找下一个文件
 * @param find_fd 查找句柄
 * @param file_data 文件数据
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_find_next(find_fd, file_data) \
    cm_fs_find_next((find_fd), (file_data))

/**
 * @brief 关闭查找
 * @param find_fd 查找句柄
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_find_close(find_fd) \
    cm_fs_find_close((find_fd))

/**
 * @brief 打开目录
 * @param path 目录路径
 * @return 目录句柄，失败返回 NULL
 */
#define iot_fs_opendir(path) \
    cm_fs_find_first((path), NULL)

/**
 * @brief 读取目录项
 * @param dir 目录句柄
 * @param entry 目录项
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_readdir(dir, entry) \
    cm_fs_find_next((dir), (entry))

/**
 * @brief 关闭目录
 * @param dir 目录句柄
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_closedir(dir) \
    cm_fs_find_close((dir))

/**
 * @brief 获取文件当前位置
 * @param fp 文件句柄
 * @return 当前位置
 */
#define iot_fs_file_tell(fp) \
    ((long)0)

/**
 * @brief 获取文件大小（通过句柄）
 * @param fp 文件句柄
 * @return 文件大小
 */
#define iot_fs_size(fp) \
    ((long)0)

/* 文件打开模式 */
#define IOT_FS_RB                CM_FS_RB    /**< 只读二进制 */
#define IOT_FS_WB                CM_FS_WB    /**< 只写二进制 */
#define IOT_FS_AB                CM_FS_AB    /**< 追加二进制 */
#define IOT_FS_WBPLUS            CM_FS_WBPLUS /**< 读写二进制（新建） */
#define IOT_FS_ABPLUS            CM_FS_ABPLUS /**< 读写二进制（追加） */
#define IOT_FS_RBPLUS            CM_FS_RBPLUS /**< 读写二进制（打开） */

/* 文件定位方式 */
#define IOT_FS_SEEK_SET          CM_FS_SEEK_SET    /**< 从文件开头定位 */
#define IOT_FS_SEEK_CUR          CM_FS_SEEK_CUR    /**< 从当前位置定位 */
#define IOT_FS_SEEK_END          CM_FS_SEEK_END    /**< 从文件末尾定位 */

/*===========================================================
 * 路径操作适配层
 *===========================================================*/

#define IOT_PATH_SEPARATOR       '/'
#define IOT_PATH_ALT_SEPARATOR   '\\'

static inline char iot_path_separator(void) {
    return '/';
}

static inline const char* iot_path_separator_str(void) {
    return "/";
}

static inline int iot_path_is_separator(char c) {
    return c == '/' || c == '\\';
}

/*===========================================================
 * DNS 解析适配层
 *===========================================================*/

static inline int iot_dns_resolve(const char* name, char* ip, size_t ip_len) {
    struct hostent* he = gethostbyname(name);
    if (!he || !he->h_addr_list || !he->h_addr_list[0] || ip_len == 0) {
        return -1;
    }
    const char* addr = inet_ntoa(*(struct in_addr*)he->h_addr_list[0]);
    if (!addr) {
        return -1;
    }
    strncpy(ip, addr, ip_len - 1);
    ip[ip_len - 1] = '\0';
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* IOT_PLATFORM_ML307N_H */
