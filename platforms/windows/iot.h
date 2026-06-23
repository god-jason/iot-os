/**
 * @file iot.h
 * @brief Windows 平台适配头文件
 * 
 * 本文件提供跨平台统一接口，接口顺序为：
 * 1. 内存管理 (mem)
 * 2. 互斥锁 (mutex)
 * 3. 信号量 (semaphore)
 * 4. 任务 (task)
 * 5. 定时器 (timer)
 * 6. Socket 网络接口 (socket)
 */
#ifndef IOT_PLATFORM_WINDOWS_H
#define IOT_PLATFORM_WINDOWS_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 1. 内存管理接口 (mem)
 *===========================================================*/

/**
 * @brief 分配内存
 * @param size 要分配的字节数
 * @return 指向分配内存的指针，失败返回 NULL
 */
#define iot_malloc(size) \
    malloc((size_t)(size))

/**
 * @brief 分配并初始化内存（置零）
 * @param nmemb 元素个数
 * @param size 每个元素的大小
 * @return 指向分配内存的指针，失败返回 NULL
 */
#define iot_calloc(nmemb, size) \
    calloc((size_t)(nmemb), (size_t)(size))

/**
 * @brief 重新分配内存
 * @param ptr 原内存指针
 * @param size 新大小
 * @return 指向重新分配内存的指针，失败返回 NULL
 */
#define iot_realloc(ptr, size) \
    realloc((void *)(ptr), (size_t)(size))

/**
 * @brief 释放内存
 * @param ptr 要释放的内存指针
 */
#define iot_free(ptr) \
    free((void *)(ptr))

/**
 * @brief 复制字符串（Windows 特有实现）
 * @param str 要复制的源字符串
 * @return 指向新字符串的指针
 */
#define iot_strdup(str) \
    _strdup((str))

/*===========================================================
 * 2. 互斥锁接口 (mutex)
 *===========================================================*/

/**
 * @brief 互斥锁类型
 */
#define iot_mutex_t              HANDLE

/**
 * @brief 创建互斥锁
 * @return 互斥锁句柄，失败返回 NULL
 */
#define iot_mutex_create() \
    CreateMutex(NULL, FALSE, NULL)

/**
 * @brief 锁定互斥锁
 * @param mutex 互斥锁句柄
 * @param timeout_ms 超时时间（毫秒），IOT_WAIT_FOREVER 表示永久等待
 * @return 成功返回 true，失败返回 false
 */
#define iot_mutex_lock(mutex, timeout_ms) \
    (WaitForSingleObject((mutex), (timeout_ms)) == WAIT_OBJECT_0)

/**
 * @brief 解锁互斥锁
 * @param mutex 互斥锁句柄
 * @return 成功返回 true，失败返回 false
 */
#define iot_mutex_unlock(mutex) \
    ReleaseMutex((mutex))

/**
 * @brief 删除互斥锁
 * @param mutex 互斥锁句柄
 */
#define iot_mutex_delete(mutex) \
    CloseHandle((mutex))

/*===========================================================
 * 3. 信号量接口 (semaphore)
 *===========================================================*/

/**
 * @brief 信号量类型
 */
#define iot_sem_t                HANDLE

/**
 * @brief 创建信号量
 * @param max_count 信号量最大值
 * @param initial_count 信号量初始值
 * @return 信号量句柄，失败返回 NULL
 */
#define iot_sem_create(max_count, initial_count) \
    CreateSemaphore(NULL, (initial_count), (max_count), NULL)

/**
 * @brief 等待信号量（永久等待）
 * @param sem 信号量句柄
 * @return 成功返回 true，失败返回 false
 */
#define iot_sem_wait(sem) \
    (WaitForSingleObject((sem), INFINITE) == WAIT_OBJECT_0)

/**
 * @brief 等待信号量（带超时）
 * @param sem 信号量句柄
 * @param timeout_ms 超时时间（毫秒）
 * @return 成功返回 true，超时或失败返回 false
 */
#define iot_sem_wait_timeout(sem, timeout_ms) \
    (WaitForSingleObject((sem), (timeout_ms)) == WAIT_OBJECT_0)

/**
 * @brief 释放信号量（+1）
 * @param sem 信号量句柄
 * @return 成功返回 true，失败返回 false
 */
#define iot_sem_post(sem) \
    ReleaseSemaphore((sem), 1, NULL)

/**
 * @brief 获取信号量当前计数
 * @param sem 信号量句柄
 * @return 信号量计数
 */
#define iot_sem_get_count(sem) \
    ((DWORD)0)

/**
 * @brief 删除信号量
 * @param sem 信号量句柄
 */
#define iot_sem_delete(sem) \
    CloseHandle((sem))

/*===========================================================
 * 4. 任务接口 (task)
 *===========================================================*/

/**
 * @brief 任务类型（线程句柄）
 */
#define iot_task_t               HANDLE

/**
 * @brief 任务优先级类型
 */
#define iot_osPriority_t         int

/* 任务优先级定义（值越小优先级越低） */
#define IOT_OS_PRIO_IDLE         THREAD_PRIORITY_IDLE       /**< 空闲优先级 */
#define IOT_OS_PRIO_LOW          THREAD_PRIORITY_LOWEST    /**< 低优先级 */
#define IOT_OS_PRIO_LOWEST       THREAD_PRIORITY_BELOW_NORMAL /**< 次低优先级 */
#define IOT_OS_PRIO_BELOW_NORMAL THREAD_PRIORITY_NORMAL    /**< 低于正常优先级 */
#define IOT_OS_PRIO_NORMAL       THREAD_PRIORITY_NORMAL    /**< 正常优先级 */
#define IOT_OS_PRIO_ABOVE_NORMAL THREAD_PRIORITY_ABOVE_NORMAL /**< 高于正常优先级 */
#define IOT_OS_PRIO_HIGH         THREAD_PRIORITY_HIGHEST   /**< 高优先级 */
#define IOT_OS_PRIO_HIGHEST      THREAD_PRIORITY_TIME_CRITICAL /**< 最高优先级 */
#define IOT_OS_PRIO_ERROR        0                         /**< 错误优先级 */

/**
 * @brief 任务函数类型
 */
#define iot_task_func_t          DWORD (__stdcall *)(LPVOID)

/**
 * @brief 创建任务（线程）
 * @param name 任务名称（Windows 不使用）
 * @param func 任务函数
 * @param arg 传递给任务的参数
 * @param stack_size 栈大小（字节），0 表示使用默认大小
 * @param priority 任务优先级
 * @return 任务句柄，失败返回 NULL
 */
#define iot_task_create(name, func, arg, stack_size, priority) \
    CreateThread(NULL, (stack_size), (LPTHREAD_START_ROUTINE)(func), (arg), 0, NULL)

/**
 * @brief 任务延时
 * @param ms 延时时间（毫秒）
 */
#define iot_task_delay(ms) \
    Sleep((ms))

/**
 * @brief 删除任务（强制终止）
 * @param task 任务句柄
 */
#define iot_task_delete(task) \
    TerminateThread((task), 0)

/**
 * @brief 获取当前任务句柄
 * @return 当前线程句柄
 */
#define iot_task_get_current() \
    GetCurrentThread()

/**
 * @brief 获取系统运行时间
 * @return 系统运行时间（毫秒）
 */
#define iot_get_tick_ms() \
    GetTickCount()

/**
 * @brief 获取系统时钟频率
 * @return 时钟频率（Hz），通常为 1000
 */
#define iot_get_tick_freq() \
    (1000)

/**
 * @brief 退出当前任务
 */
#define iot_task_exit() \
    ExitThread(0)

/*===========================================================
 * 5. 定时器接口 (timer)
 *===========================================================*/

/**
 * @brief 定时器类型
 */
#define iot_timer_t              HANDLE

/**
 * @brief 定时器类型枚举
 */
#define iot_timer_type_t         int
#define IOT_TIMER_ONCE           0   /**< 单次定时器 */
#define IOT_TIMER_PERIODIC       1   /**< 周期定时器 */

/**
 * @brief 定时器回调函数类型
 */
#define iot_timer_callback_t     VOID (__stdcall *)(PVOID)

/**
 * @brief 创建定时器
 * @param cb 定时器回调函数
 * @param arg 传递给回调的参数
 * @param period_ms 定时周期（毫秒）
 * @param type 定时器类型（IOT_TIMER_ONCE 或 IOT_TIMER_PERIODIC）
 * @return 定时器句柄，失败返回 NULL
 */
#define iot_timer_create(cb, arg, period_ms, type) ({ \
    HANDLE _iot_timer = NULL; \
    CreateTimerQueueTimer(&_iot_timer, NULL, (WAITORTIMERCALLBACK)(cb), (PVOID)(arg), \
        (DWORD)(period_ms), \
        ((type) == IOT_TIMER_PERIODIC) ? (DWORD)(period_ms) : 0, \
        WT_EXECUTEDEFAULT); \
    _iot_timer; \
})

/**
 * @brief 启动定时器
 * @param timer 定时器句柄
 * @param period_ms 定时周期（毫秒）
 * @return 成功返回 0，失败返回 -1
 */
#define iot_timer_start(timer, period_ms) \
    (0)

/**
 * @brief 停止定时器
 * @param timer 定时器句柄
 */
#define iot_timer_stop(timer) \
    DeleteTimerQueueTimer(NULL, (timer), NULL)

/**
 * @brief 删除定时器
 * @param timer 定时器句柄
 */
#define iot_timer_delete(timer) \
    DeleteTimerQueueTimer(NULL, (timer), NULL)

/**
 * @brief 检查定时器是否运行
 * @param timer 定时器句柄
 * @return 运行返回 true，停止返回 false
 */
#define iot_timer_is_running(timer) \
    ((timer) != NULL)

/*===========================================================
 * 6. Socket 网络接口 (socket)
 *===========================================================*/

/**
 * @brief Socket 句柄类型
 */
#define iot_socket_t              SOCKET

/**
 * @brief 无效 Socket 值
 */
#define IOT_SOCKET_INVALID        INVALID_SOCKET

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
#define IOT_SHUT_RD               SD_RECEIVE        /**< 关闭读 */
#define IOT_SHUT_WR               SD_SEND           /**< 关闭写 */
#define IOT_SHUT_RDWR             SD_BOTH           /**< 关闭读写 */

/* Socket 地址结构 */
#define iot_sockaddr_t            struct sockaddr
#define iot_sockaddr_in_t         struct sockaddr_in
#define iot_sockaddr_in6_t        struct sockaddr_in6
#define iot_in_addr_t             struct in_addr
#define iot_in6_addr_t            struct in6_addr
#define iot_socklen_t             int

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
    closesocket((sock))

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
    accept((sock), (struct sockaddr*)(addr), (int*)(addrlen))

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
    send((sock), (const char*)(buf), (int)(len), (flags))

/**
 * @brief 发送数据到指定地址
 * @param sock Socket 句柄
 * @param buf 数据缓冲区
 * @param len 数据长度
 * @param flags 发送标志
 * @param dest_addr 目标地址
 * @param dest_len 地址长度
 * @return 成功返回发送的字节数，失败返回 -1
 */
#define iot_sendto(sock, buf, len, flags, dest_addr, dest_len) \
    sendto((sock), (const char*)(buf), (int)(len), (flags), \
        (const struct sockaddr*)(dest_addr), (int)(dest_len))

/**
 * @brief 接收数据
 * @param sock Socket 句柄
 * @param buf 数据缓冲区
 * @param len 缓冲区长度
 * @param flags 接收标志（通常为 0）
 * @return 成功返回接收的字节数，失败返回 -1，连接关闭返回 0
 */
#define iot_recv(sock, buf, len, flags) \
    recv((sock), (char*)(buf), (int)(len), (flags))

/**
 * @brief 接收数据并获取发送者地址
 * @param sock Socket 句柄
 * @param buf 数据缓冲区
 * @param len 缓冲区长度
 * @param flags 接收标志
 * @param src_addr 发送者地址（可为 NULL）
 * @param src_len 地址长度（可为 NULL）
 * @return 成功返回接收的字节数，失败返回 -1
 */
#define iot_recvfrom(sock, buf, len, flags, src_addr, src_len) \
    recvfrom((sock), (char*)(buf), (int)(len), (flags), \
        (struct sockaddr*)(src_addr), (int*)(src_len))

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
    setsockopt((sock), (level), (optname), (const char*)(optval), (int)(optlen))

/**
 * @brief 获取 Socket 选项
 * @param sock Socket 句柄
 * @param level 选项级别
 * @param optname 选项名称
 * @param optval 存储选项值的缓冲区
 * @param optlen 选项长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_getsockopt(sock, level, optname, optval, optlen) \
    getsockopt((sock), (level), (optname), (char*)(optval), (int*)(optlen))

/**
 * @brief 获取连接对端地址
 * @param sock Socket 句柄
 * @param addr 存储地址的缓冲区
 * @param addrlen 地址长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_getpeername(sock, addr, addrlen) \
    getpeername((sock), (struct sockaddr*)(addr), (int*)(addrlen))

/**
 * @brief 获取本地地址
 * @param sock Socket 句柄
 * @param addr 存储地址的缓冲区
 * @param addrlen 地址长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_getsockname(sock, addr, addrlen) \
    getsockname((sock), (struct sockaddr*)(addr), (int*)(addrlen))

/* Select 多路复用操作 */
#define iot_fd_set_t              fd_set

/**
 * @brief 将文件描述符加入集合
 */
#define iot_fd_set(fd, set) \
    FD_SET((fd), (set))

/**
 * @brief 将文件描述符从集合移除
 */
#define iot_fd_clr(fd, set) \
    FD_CLR((fd), (set))

/**
 * @brief 检查文件描述符是否在集合中
 */
#define iot_fd_isset(fd, set) \
    FD_ISSET((fd), (set))

/**
 * @brief 清空文件描述符集合
 */
#define iot_fd_zero(set) \
    FD_ZERO((set))

/**
 * @brief Select 最大描述符数量
 */
#define iot_select_fd_set_max     FD_SETSIZE

/**
 * @brief I/O 多路复用 select
 * @param nfds 最大文件描述符 + 1
 * @param readfds 可读集合
 * @param writefds 可写集合
 * @param exceptfds 异常集合
 * @param timeout 超时时间
 * @return 就绪描述符数量，超时返回 0，失败返回 -1
 */
#define iot_select(nfds, readfds, writefds, exceptfds, timeout) \
    select((nfds), (readfds), (writefds), (exceptfds), (timeout))

/* 地址转换函数 */
/**
 * @brief 点分十进制 IP 转换为二进制
 * @param cp 点分十进制字符串
 * @return 二进制 IP 地址
 */
#define iot_inet_addr(cp) \
    inet_addr((cp))

/**
 * @brief 点分十进制 IP 转换为二进制（支持多地址）
 * @param cp 点分十进制字符串
 * @param inp 存储结果的 in_addr 结构
 * @return 成功返回 1，失败返回 0
 */
#define iot_inet_aton(cp, inp) \
    ((int)inet_aton((cp), (inp)))

/**
 * @brief 二进制 IP 转换为点分十进制字符串
 * @param addr 二进制 IP 地址
 * @return 点分十进制字符串
 */
#define iot_inet_ntoa(addr) \
    inet_ntoa((addr))

/**
 * @brief 二进制 IP 转换为字符串
 * @param af 地址族（AF_INET, AF_INET6）
 * @param src 二进制 IP 源
 * @param dst 目标字符串缓冲区
 * @param size 缓冲区大小
 * @return 成功返回 dst，失败返回 NULL
 */
#define iot_inet_ntop(af, src, dst, size) \
    inet_ntop((af), (const void*)(src), (dst), (size))

/**
 * @brief 字符串 IP 转换为二进制
 * @param af 地址族
 * @param src 源字符串
 * @param dst 目标二进制地址
 * @return 成功返回 1，失败返回 0
 */
#define iot_inet_pton(af, src, dst) \
    inet_pton((af), (src), (void*)(dst))

/* DNS 解析函数 */
/**
 * @brief 通过主机名获取 IP 地址
 * @param name 主机名
 * @return hostent 结构指针，失败返回 NULL
 */
#define iot_gethostbyname(name) \
    gethostbyname((name))

/**
 * @brief 通过 IP 地址获取主机信息
 * @param addr IP 地址
 * @param len 地址长度
 * @param type 地址类型
 * @return hostent 结构指针，失败返回 NULL
 */
#define iot_gethostbyaddr(addr, len, type) \
    gethostbyaddr((const char*)(addr), (int)(len), (int)(type))

/* Socket 错误处理 */
/**
 * @brief 获取最近一次 Socket 错误码
 * @return 错误码
 */
#define iot_socket_errno() \
    WSAGetLastError()

/**
 * @brief 获取最近一次 Socket 错误码（别名）
 */
#define iot_socket_error() \
    WSAGetLastError()

/* Socket 错误码映射 */
/** 操作将阻塞 */
#define IOT_EAGAIN                WSAEWOULDBLOCK
/** 操作将阻塞 */
#define IOT_EWOULDBLOCK           WSAEWOULDBLOCK
/** 操作正在进行中 */
#define IOT_EINPROGRESS           WSAEINPROGRESS
/** 操作已在进行中 */
#define IOT_EALREADY              WSAEALREADY
/** 参数不是 Socket */
#define IOT_ENOTSOCK              WSAENOTSOCK
/** 缺少目标地址 */
#define IOT_EDESTADDRREQ          WSAEDESTADDRREQ
/** 消息过长 */
#define IOT_EMSGSIZE              WSAEMSGSIZE
/** 协议类型错误 */
#define IOT_EPROTOTYPE            WSAEPROTOTYPE
/** 协议选项不支持 */
#define IOT_ENOPROTOOPT           WSAENOPROTOOPT
/** 协议不支持 */
#define IOT_EPROTONOSUPPORT       WSAEPROTONOSUPPORT
/** Socket 类型不支持 */
#define IOT_ESOCKTNOSUPPORT       WSAESOCKTNOSUPPORT
/** 操作不支持 */
#define IOT_EOPNOTSUPP            WSAEOPNOTSUPP
/** 协议族不支持 */
#define IOT_EPFNOSUPPORT          WSAEPFNOSUPPORT
/** 地址族不支持 */
#define IOT_EAFNOSUPPORT          WSAEAFNOSUPPORT
/** 地址已在使用 */
#define IOT_EADDRINUSE            WSAEADDRINUSE
/** 地址不可用 */
#define IOT_EADDRNOTAVAIL         WSAEADDRNOTAVAIL
/** 网络已断开 */
#define IOT_ENETDOWN              WSAENETDOWN
/** 网络不可达 */
#define IOT_ENETUNREACH           WSAENETUNREACH
/** 网络重置 */
#define IOT_ENETRESET             WSAENETRESET
/** 连接中止 */
#define IOT_ECONNABORTED          WSAECONNABORTED
/** 连接被重置 */
#define IOT_ECONNRESET            WSAECONNRESET
/** 缓冲区不足 */
#define IOT_ENOBUFS               WSAENOBUFS
/** 已连接 */
#define IOT_EISCONN               WSAEISCONN
/** 未连接 */
#define IOT_ENOTCONN              WSAENOTCONN
/** Socket 已关闭 */
#define IOT_ESHUTDOWN             WSAESHUTDOWN
/** 连接超时 */
#define IOT_ETIMEDOUT             WSAETIMEDOUT
/** 连接被拒绝 */
#define IOT_ECONNREFUSED          WSAECONNREFUSED
/** 主机已关闭 */
#define IOT_EHOSTDOWN             WSAEHOSTDOWN
/** 主机不可达 */
#define IOT_EHOSTUNREACH          WSAEHOSTUNREACH
/** 网络不存在 */
#define IOT_ENONET                WSAENONET

/**
 * @brief Socket 库初始化
 * @return 成功返回 0，失败返回错误码
 */
static inline int iot_socket_init(void) {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

/**
 * @brief Socket 库清理
 */
static inline void iot_socket_deinit(void) {
    WSACleanup();
}

/*===========================================================
 * 7. 标准输出接口 (stdout)
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
 * 辅助接口（文件系统、路径、DNS）
 *===========================================================*/

/* 文件系统路径最大长度 */
#define IOT_FS_MAX_PATH          255

#define iot_fs_file_t            FILE*
#define iot_fs_dir_t             intptr_t

/* Windows 目录遍历上下文结构 */
typedef struct {
    intptr_t handle;
    struct _finddata_t entry;
    int valid;
} _iot_fs_dirent_ctx_t;

#define iot_fs_dirent_t          _iot_fs_dirent_ctx_t

/* 目录项成员访问宏 */
#define iot_fs_dirent_name(dirent)       ((dirent).valid ? (dirent).entry.name : "")
#define iot_fs_dirent_is_dir(dirent)     ((dirent).valid ? (((dirent).entry.attrib & 0x10) != 0) : 0)
#define iot_fs_dirent_size(dirent)       ((dirent).valid ? (dirent).entry.size : 0)

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

/* Windows _findnext 实现 */
static inline int _iot_fs_readdir(iot_fs_dir_t dir, iot_fs_dirent_t* entry) {
    if (!entry) return -1;
    entry->handle = dir;
    int ret = _findnext(dir, &entry->entry);
    entry->valid = (ret == 0) ? 1 : 0;
    return ret;
}
#define iot_fs_readdir(dir, entry) \
    _iot_fs_readdir((dir), (entry))

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
#define IOT_FS_OPEN_USES_STRING_MODE

#define IOT_FS_SEEK_SET          SEEK_SET
#define IOT_FS_SEEK_CUR          SEEK_CUR
#define IOT_FS_SEEK_END          SEEK_END

/* 路径操作 */
#define IOT_PATH_SEPARATOR       '\\'
#define IOT_PATH_ALT_SEPARATOR   '/'

static inline char iot_path_separator(void) {
    return '\\';
}

static inline const char* iot_path_separator_str(void) {
    return "\\";
}

static inline int iot_path_is_separator(char c) {
    return c == '/' || c == '\\';
}

/**
 * @brief DNS 域名解析
 * @param name 域名
 * @param ip 存储解析结果的 IP 字符串缓冲区
 * @param ip_len 缓冲区长度
 * @return 成功返回 0，失败返回 -1
 */
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

/*===========================================================
 * 平台事件初始化（由 platform.c 实现）
 *===========================================================*/

void iot_event_init(void);
void iot_event_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* IOT_PLATFORM_WINDOWS_H */
