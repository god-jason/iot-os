/**
 * @file iot.h
 * @brief WebAssembly 平台适配头文件
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
#ifndef IOT_PLATFORM_WASM_H
#define IOT_PLATFORM_WASM_H

#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

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
 * @brief 复制字符串
 * @param str 要复制的源字符串
 * @return 指向新字符串的指针
 */
#define iot_strdup(str) \
    strdup((str))

/*===========================================================
 * 2. 互斥锁接口 (mutex)
 *===========================================================*/

/**
 * @brief 互斥锁类型
 */
#define iot_mutex_t              int

/**
 * @brief 创建互斥锁
 * @return 互斥锁句柄，失败返回 NULL
 */
#define iot_mutex_create() \
    (1)

/**
 * @brief 锁定互斥锁
 * @param mutex 互斥锁句柄
 * @param timeout_ms 超时时间（毫秒），IOT_WAIT_FOREVER 表示永久等待
 * @return 成功返回 true，失败返回 false
 */
#define iot_mutex_lock(mutex, timeout_ms) \
    (1)

/**
 * @brief 解锁互斥锁
 * @param mutex 互斥锁句柄
 */
#define iot_mutex_unlock(mutex) \
    (1)

/**
 * @brief 删除互斥锁
 * @param mutex 互斥锁句柄
 */
#define iot_mutex_delete(mutex) \
    do {} while(0)

/*===========================================================
 * 3. 信号量接口 (semaphore)
 *===========================================================*/

/**
 * @brief 信号量类型
 */
#define iot_sem_t                int

/**
 * @brief 创建信号量
 * @param max_count 信号量最大值
 * @param initial_count 信号量初始值
 * @return 信号量句柄，失败返回 NULL
 */
#define iot_sem_create(max_count, initial_count) \
    ((initial_count))

/**
 * @brief 等待信号量（永久等待）
 * @param sem 信号量句柄
 * @return 成功返回 true，失败返回 false
 */
#define iot_sem_wait(sem) \
    (1)

/**
 * @brief 等待信号量（带超时）
 * @param sem 信号量句柄
 * @param timeout_ms 超时时间（毫秒）
 * @return 成功返回 true，超时或失败返回 false
 */
#define iot_sem_wait_timeout(sem, timeout_ms) \
    (1)

/**
 * @brief 释放信号量（+1）
 * @param sem 信号量句柄
 */
#define iot_sem_post(sem) \
    (1)

/**
 * @brief 获取信号量当前计数
 * @param sem 信号量句柄
 * @return 信号量计数
 */
#define iot_sem_get_count(sem) \
    (sem)

/**
 * @brief 删除信号量
 * @param sem 信号量句柄
 */
#define iot_sem_delete(sem) \
    do {} while(0)

/*===========================================================
 * 4. 任务接口 (task)
 *===========================================================*/

/**
 * @brief 任务类型（线程句柄）
 */
#define iot_task_t               int

/**
 * @brief 任务优先级类型
 */
#define iot_osPriority_t         int

/**
 * @brief 任务函数类型
 */
#define iot_task_func_t          void (*)(void*)

/* 任务优先级定义（值越小优先级越低） */
#define IOT_OS_PRIO_IDLE         0   /**< 空闲优先级 */
#define IOT_OS_PRIO_LOW          1   /**< 低优先级 */
#define IOT_OS_PRIO_LOWEST       2   /**< 次低优先级 */
#define IOT_OS_PRIO_BELOW_NORMAL 3   /**< 低于正常优先级 */
#define IOT_OS_PRIO_NORMAL       4   /**< 正常优先级 */
#define IOT_OS_PRIO_ABOVE_NORMAL 5   /**< 高于正常优先级 */
#define IOT_OS_PRIO_HIGH         6   /**< 高优先级 */
#define IOT_OS_PRIO_HIGHEST      7   /**< 最高优先级 */
#define IOT_OS_PRIO_ERROR        -1  /**< 错误优先级 */

/**
 * @brief 创建任务（线程）
 * @param name 任务名称
 * @param func 任务函数
 * @param arg 传递给任务的参数
 * @param stack_size 栈大小（字节），0 表示使用默认大小
 * @param priority 任务优先级
 * @return 任务句柄，失败返回 NULL
 */
#define iot_task_create(name, func, arg, stack_size, priority) ({ \
    emscripten_async_call((func), (arg), 0); \
    1; \
})

/**
 * @brief 任务延时
 * @param ms 延时时间（毫秒）
 */
#define iot_task_delay(ms) \
    emscripten_sleep((ms))

/**
 * @brief 删除任务
 * @param task 任务句柄
 */
#define iot_task_delete(task) \
    do {} while(0)

/**
 * @brief 获取当前任务句柄
 * @return 当前任务句柄
 */
#define iot_task_get_current() \
    (1)

/**
 * @brief 获取系统运行时间
 * @return 系统运行时间（毫秒）
 */
#define iot_get_tick_ms() \
    emscripten_get_now()

/**
 * @brief 获取系统时钟频率
 * @return 时钟频率（Hz），通常为 1000
 */
#define iot_get_tick_freq() \
    (1000)

/**
 * @brief 获取系统运行时间（毫秒）
 * @return 系统运行时间（毫秒）
 */
#define iot_get_time_ms() \
    emscripten_get_now()

/**
 * @brief 获取系统运行时间（秒）
 * @return 系统运行时间（秒）
 */
#define iot_get_time_s() \
    (emscripten_get_now() / 1000)

/**
 * @brief 退出当前任务
 */
#define iot_task_exit() \
    do {} while(0)

/*===========================================================
 * 5. 定时器接口 (timer)
 *===========================================================*/

/**
 * @brief 定时器类型
 */
#define iot_timer_t              int

/**
 * @brief 定时器类型枚举
 */
#define iot_timer_type_t         int
#define IOT_TIMER_ONCE           0   /**< 单次定时器 */
#define IOT_TIMER_PERIODIC       1   /**< 周期定时器 */

/**
 * @brief 定时器回调函数类型
 */
#define iot_timer_callback_t     void (*)(void*)

/**
 * @brief 创建定时器
 * @param cb 定时器回调函数
 * @param arg 传递给回调的参数
 * @param period_ms 定时周期（毫秒）
 * @param type 定时器类型（IOT_TIMER_ONCE 或 IOT_TIMER_PERIODIC）
 * @return 定时器句柄，失败返回 NULL
 */
#define iot_timer_create(cb, arg, period_ms, type) ({ \
    if ((type) == IOT_TIMER_PERIODIC) { \
        emscripten_set_interval((cb), (arg), (period_ms)); \
    } else { \
        emscripten_set_timeout((cb), (arg), (period_ms)); \
    } \
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
 * @return 成功返回 0，失败返回 -1
 */
#define iot_timer_stop(timer) \
    emscripten_clear_interval((timer))

/**
 * @brief 删除定时器
 * @param timer 定时器句柄
 * @return 成功返回 0，失败返回 -1
 */
#define iot_timer_delete(timer) \
    emscripten_clear_interval((timer))

/**
 * @brief 检查定时器是否运行中
 * @param timer 定时器句柄
 * @return 运行中返回 true，否则返回 false
 */
#define iot_timer_is_running(timer) \
    ((timer) != 0)

/*===========================================================
 * 6. Socket 网络接口 (socket)
 *===========================================================*/

/**
 * @brief Socket句柄类型
 */
#define iot_socket_t              int

/**
 * @brief 无效Socket句柄
 */
#define IOT_SOCKET_INVALID        (-1)

/**
 * @brief Socket类型
 */
#define IOT_SOCK_STREAM           1   /**< 流式Socket（TCP） */
#define IOT_SOCK_DGRAM            2   /**< 数据报Socket（UDP） */
#define IOT_SOCK_RAW              3   /**< 原始Socket */

/**
 * @brief 协议类型
 */
#define IOT_IPPROTO_TCP           6   /**< TCP协议 */
#define IOT_IPPROTO_UDP           17  /**< UDP协议 */
#define IOT_IPPROTO_IP            0   /**< IP协议 */

/**
 * @brief Socket选项级别
 */
#define IOT_SOL_SOCKET            1

/**
 * @brief Socket选项
 */
#define IOT_SO_REUSEADDR          1   /**< 允许重用本地地址 */
#define IOT_SO_KEEPALIVE          2   /**< 启用TCP保活 */
#define IOT_SO_RCVBUF             3   /**< 接收缓冲区大小 */
#define IOT_SO_SNDBUF             4   /**< 发送缓冲区大小 */
#define IOT_SO_ERROR              5   /**< 获取Socket错误 */
#define IOT_TCP_NODELAY           6   /**< 禁用Nagle算法 */

/**
 * @brief Socket关闭方式
 */
#define IOT_SHUT_RD               0   /**< 关闭读端 */
#define IOT_SHUT_WR               1   /**< 关闭写端 */
#define IOT_SHUT_RDWR             2   /**< 关闭读写端 */

/**
 * @brief Socket地址类型
 */
#define iot_sockaddr_t            struct sockaddr
#define iot_sockaddr_in_t         struct sockaddr_in
#define iot_sockaddr_in6_t        struct sockaddr_in6
#define iot_in_addr_t             struct in_addr
#define iot_in6_addr_t            struct in6_addr
#define iot_socklen_t             int

/**
 * @brief 创建Socket
 * @param domain 地址族
 * @param type Socket类型
 * @param protocol 协议类型
 * @return Socket句柄，失败返回 IOT_SOCKET_INVALID
 */
#define iot_socket(domain, type, protocol) \
    (IOT_SOCKET_INVALID)

/**
 * @brief 创建一对无名Socket
 * @param domain 地址族
 * @param type Socket类型
 * @param protocol 协议类型
 * @param sv Socket句柄数组（大小为2）
 * @return 成功返回 0，失败返回 -1
 */
#define iot_socketpair(domain, type, protocol, sv) \
    (-1)

/**
 * @brief 关闭Socket
 * @param sock Socket句柄
 * @return 成功返回 0，失败返回 -1
 */
#define iot_close(sock) \
    (0)

/**
 * @brief 关闭Socket读/写端
 * @param sock Socket句柄
 * @param how 关闭方式
 * @return 成功返回 0，失败返回 -1
 */
#define iot_shutdown(sock, how) \
    (0)

/**
 * @brief 绑定Socket到本地地址
 * @param sock Socket句柄
 * @param addr 本地地址
 * @param addrlen 地址长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_bind(sock, addr, addrlen) \
    (-1)

/**
 * @brief 监听Socket
 * @param sock Socket句柄
 * @param backlog 最大连接数
 * @return 成功返回 0，失败返回 -1
 */
#define iot_listen(sock, backlog) \
    (-1)

/**
 * @brief 接受连接
 * @param sock Socket句柄
 * @param addr 客户端地址
 * @param addrlen 地址长度
 * @return 新连接Socket句柄，失败返回 IOT_SOCKET_INVALID
 */
#define iot_accept(sock, addr, addrlen) \
    (IOT_SOCKET_INVALID)

/**
 * @brief 连接到远程主机
 * @param sock Socket句柄
 * @param addr 远程地址
 * @param addrlen 地址长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_connect(sock, addr, addrlen) \
    (-1)

/**
 * @brief 发送数据
 * @param sock Socket句柄
 * @param buf 数据缓冲区
 * @param len 数据长度
 * @param flags 标志位
 * @return 成功返回发送的字节数，失败返回 -1
 */
#define iot_send(sock, buf, len, flags) \
    (-1)

/**
 * @brief 发送数据到指定地址
 * @param sock Socket句柄
 * @param buf 数据缓冲区
 * @param len 数据长度
 * @param flags 标志位
 * @param dest_addr 目标地址
 * @param dest_len 地址长度
 * @return 成功返回发送的字节数，失败返回 -1
 */
#define iot_sendto(sock, buf, len, flags, dest_addr, dest_len) \
    (-1)

/**
 * @brief 接收数据
 * @param sock Socket句柄
 * @param buf 数据缓冲区
 * @param len 缓冲区大小
 * @param flags 标志位
 * @return 成功返回接收的字节数，失败返回 -1
 */
#define iot_recv(sock, buf, len, flags) \
    (-1)

/**
 * @brief 从指定地址接收数据
 * @param sock Socket句柄
 * @param buf 数据缓冲区
 * @param len 缓冲区大小
 * @param flags 标志位
 * @param src_addr 源地址
 * @param src_len 地址长度
 * @return 成功返回接收的字节数，失败返回 -1
 */
#define iot_recvfrom(sock, buf, len, flags, src_addr, src_len) \
    (-1)

/**
 * @brief 设置Socket选项
 * @param sock Socket句柄
 * @param level 选项级别
 * @param optname 选项名称
 * @param optval 选项值
 * @param optlen 选项值长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_setsockopt(sock, level, optname, optval, optlen) \
    (0)

/**
 * @brief 获取Socket选项
 * @param sock Socket句柄
 * @param level 选项级别
 * @param optname 选项名称
 * @param optval 选项值缓冲区
 * @param optlen 选项值长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_getsockopt(sock, level, optname, optval, optlen) \
    (0)

/**
 * @brief 获取对等方地址
 * @param sock Socket句柄
 * @param addr 地址缓冲区
 * @param addrlen 地址长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_getpeername(sock, addr, addrlen) \
    (-1)

/**
 * @brief 获取本地地址
 * @param sock Socket句柄
 * @param addr 地址缓冲区
 * @param addrlen 地址长度
 * @return 成功返回 0，失败返回 -1
 */
#define iot_getsockname(sock, addr, addrlen) \
    (-1)

/**
 * @brief 文件描述符集合类型
 */
#define iot_fd_set_t              int

/**
 * @brief 将文件描述符加入集合
 */
#define iot_fd_set(fd, set) \
    do {} while(0)

/**
 * @brief 将文件描述符从集合中移除
 */
#define iot_fd_clr(fd, set) \
    do {} while(0)

/**
 * @brief 检查文件描述符是否在集合中
 */
#define iot_fd_isset(fd, set) \
    (0)

/**
 * @brief 清空文件描述符集合
 */
#define iot_fd_zero(set) \
    do {} while(0)

/**
 * @brief 文件描述符集合最大大小
 */
#define iot_select_fd_set_max     1024

/**
 * @brief 多路复用（选择）
 * @param nfds 最大文件描述符+1
 * @param readfds 读描述符集合
 * @param writefds 写描述符集合
 * @param exceptfds 异常描述符集合
 * @param timeout 超时时间
 * @return 成功返回就绪描述符数，失败返回 -1
 */
#define iot_select(nfds, readfds, writefds, exceptfds, timeout) \
    (-1)

/**
 * @brief 将IP地址字符串转换为网络字节序整数
 * @param cp IP地址字符串
 * @return 网络字节序整数
 */
#define iot_inet_addr(cp) \
    (0)

/**
 * @brief 将IP地址字符串转换为in_addr结构
 * @param cp IP地址字符串
 * @param inp in_addr结构指针
 * @return 成功返回非零值，失败返回 0
 */
#define iot_inet_aton(cp, inp) \
    (0)

/**
 * @brief 将网络字节序整数转换为IP地址字符串
 * @param addr in_addr结构
 * @return IP地址字符串
 */
#define iot_inet_ntoa(addr) \
    ("0.0.0.0")

/**
 * @brief 将网络地址转换为字符串
 * @param af 地址族
 * @param src 地址结构指针
 * @param dst 字符串缓冲区
 * @param size 缓冲区大小
 * @return 成功返回 dst，失败返回 NULL
 */
#define iot_inet_ntop(af, src, dst, size) \
    (NULL)

/**
 * @brief 将字符串转换为网络地址
 * @param af 地址族
 * @param src 地址字符串
 * @param dst 地址结构指针
 * @return 成功返回 0，失败返回 -1
 */
#define iot_inet_pton(af, src, dst) \
    (0)

/**
 * @brief 通过主机名获取主机信息
 * @param name 主机名
 * @return 主机信息结构，失败返回 NULL
 */
#define iot_gethostbyname(name) \
    (NULL)

/**
 * @brief 通过IP地址获取主机信息
 * @param addr IP地址
 * @param len 地址长度
 * @param type 地址类型
 * @return 主机信息结构，失败返回 NULL
 */
#define iot_gethostbyaddr(addr, len, type) \
    (NULL)

/**
 * @brief 获取Socket错误码
 * @return 错误码
 */
#define iot_socket_errno() \
    (0)

/**
 * @brief 获取Socket错误码（兼容旧接口）
 * @return 错误码
 */
#define iot_socket_error() \
    (0)

/* Socket错误码定义 */
#define IOT_EAGAIN                11
#define IOT_EWOULDBLOCK           11
#define IOT_EINPROGRESS           115
#define IOT_EALREADY              114
#define IOT_ENOTSOCK              88
#define IOT_EDESTADDRREQ          89
#define IOT_EMSGSIZE              90
#define IOT_EPROTOTYPE            91
#define IOT_ENOPROTOOPT           92
#define IOT_EPROTONOSUPPORT       93
#define IOT_ESOCKTNOSUPPORT       94
#define IOT_EOPNOTSUPP            95
#define IOT_EPFNOSUPPORT          96
#define IOT_EAFNOSUPPORT          97
#define IOT_EADDRINUSE            98
#define IOT_EADDRNOTAVAIL         99
#define IOT_ENETDOWN              100
#define IOT_ENETUNREACH           101
#define IOT_ENETRESET             102
#define IOT_ECONNABORTED          103
#define IOT_ECONNRESET            104
#define IOT_ENOBUFS               105
#define IOT_EISCONN               106
#define IOT_ENOTCONN              107
#define IOT_ESHUTDOWN             108
#define IOT_ETIMEDOUT             110
#define IOT_ECONNREFUSED          111
#define IOT_EHOSTDOWN             112
#define IOT_EHOSTUNREACH          113
#define IOT_ENONET                116

/**
 * @brief 设置Socket为非阻塞模式
 * @param fd Socket句柄
 * @return 成功返回 0，失败返回 -1
 */
int net_port_set_nonblocking(iot_socket_t fd);

/**
 * @brief 初始化Socket环境
 * @return 成功返回 0，失败返回 -1
 */
static inline int iot_socket_init(void) {
    return 0;
}

/*===========================================================
 * 7. 文件系统接口 (fs)
 *===========================================================*/

/**
 * @brief 文件状态结构类型
 */
#define iot_fs_stat_t             struct stat

/**
 * @brief 打开文件
 * @param path 文件路径
 * @param flags 打开标志
 * @param mode 文件权限
 * @return 文件描述符，失败返回 -1
 */
#define iot_fs_open(path, flags, mode) \
    open((path), (flags), (mode))

/**
 * @brief 关闭文件
 * @param fd 文件描述符
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_close(fd) \
    close((fd))

/**
 * @brief 读取文件
 * @param fd 文件描述符
 * @param buf 数据缓冲区
 * @param count 读取字节数
 * @return 成功返回读取的字节数，失败返回 -1
 */
#define iot_fs_read(fd, buf, count) \
    read((fd), (buf), (count))

/**
 * @brief 写入文件
 * @param fd 文件描述符
 * @param buf 数据缓冲区
 * @param count 写入字节数
 * @return 成功返回写入的字节数，失败返回 -1
 */
#define iot_fs_write(fd, buf, count) \
    write((fd), (buf), (count))

/**
 * @brief 文件指针定位
 * @param fd 文件描述符
 * @param offset 偏移量
 * @param whence 偏移基准
 * @return 成功返回新的偏移位置，失败返回 -1
 */
#define iot_fs_lseek(fd, offset, whence) \
    lseek((fd), (offset), (whence))

/**
 * @brief 获取文件状态
 * @param path 文件路径
 * @param buf 状态缓冲区
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_stat(path, buf) \
    stat((path), (buf))

/**
 * @brief 获取已打开文件的状态
 * @param fd 文件描述符
 * @param buf 状态缓冲区
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_fstat(fd, buf) \
    fstat((fd), (buf))

/**
 * @brief 创建目录
 * @param path 目录路径
 * @param mode 目录权限
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_mkdir(path, mode) \
    mkdir((path), (mode))

/**
 * @brief 删除目录
 * @param path 目录路径
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_rmdir(path) \
    rmdir((path))

/**
 * @brief 删除文件
 * @param path 文件路径
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_unlink(path) \
    unlink((path))

/**
 * @brief 重命名文件/目录
 * @param oldpath 原路径
 * @param newpath 新路径
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_rename(oldpath, newpath) \
    rename((oldpath), (newpath))

/**
 * @brief 检查文件访问权限
 * @param path 文件路径
 * @param mode 权限模式
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_access(path, mode) \
    access((path), (mode))

/**
 * @brief 打开目录
 * @param path 目录路径
 * @return 目录流指针，失败返回 NULL
 */
#define iot_fs_opendir(path) \
    opendir((path))

/**
 * @brief 关闭目录
 * @param dir 目录流指针
 * @return 成功返回 0，失败返回 -1
 */
#define iot_fs_closedir(dir) \
    closedir((dir))

/**
 * @brief 读取目录条目
 * @param dir 目录流指针
 * @return 目录条目结构，失败返回 NULL
 */
#define iot_fs_readdir(dir) \
    readdir((dir))

/**
 * @brief 获取目录流当前位置
 * @param dir 目录流指针
 * @return 当前位置
 */
#define iot_fs_telldir(dir) \
    telldir((dir))

/**
 * @brief 设置目录流位置
 * @param dir 目录流指针
 * @param loc 位置值
 */
#define iot_fs_seekdir(dir, loc) \
    seekdir((dir), (loc))

/**
 * @brief 重置目录流到开头
 * @param dir 目录流指针
 */
#define iot_fs_rewinddir(dir) \
    rewinddir((dir))

/* 文件打开标志 */
#define IOT_O_RDONLY              O_RDONLY
#define IOT_O_WRONLY              O_WRONLY
#define IOT_O_RDWR                O_RDWR
#define IOT_O_CREAT               O_CREAT
#define IOT_O_TRUNC               O_TRUNC
#define IOT_O_APPEND              O_APPEND

/* 文件权限 */
#define IOT_S_IRUSR               S_IRUSR
#define IOT_S_IWUSR               S_IWUSR
#define IOT_S_IXUSR               S_IXUSR
#define IOT_S_IRGRP               S_IRGRP
#define IOT_S_IWGRP               S_IWGRP
#define IOT_S_IXGRP               S_IXGRP
#define IOT_S_IROTH               S_IROTH
#define IOT_S_IWOTH               S_IWOTH
#define IOT_S_IXOTH               S_IXOTH

/* 权限检查 */
#define IOT_R_OK                  R_OK
#define IOT_W_OK                  W_OK
#define IOT_X_OK                  X_OK
#define IOT_F_OK                  F_OK

/* lseek 偏移 */
#define IOT_SEEK_SET              SEEK_SET
#define IOT_SEEK_CUR              SEEK_CUR
#define IOT_SEEK_END              SEEK_END

/*===========================================================
 * 8. 标准输出接口 (stdout)
 *===========================================================*/

/**
 * @brief 格式化输出到标准输出
 */
#define iot_printf(...) \
    printf(__VA_ARGS__)

/**
 * @brief 格式化输出到指定流
 */
#define iot_fprintf(stream, ...) \
    fprintf((stream), __VA_ARGS__)

/**
 * @brief 可变参数格式化输出到标准输出
 */
#define iot_vprintf(format, ap) \
    vprintf((format), (ap))

/**
 * @brief 可变参数格式化输出到指定流
 */
#define iot_vfprintf(stream, format, ap) \
    vfprintf((stream), (format), (ap))

/**
 * @brief 输出字符到标准输出
 */
#define iot_putchar(c) \
    putchar((c))

/**
 * @brief 输出字符串到标准输出
 */
#define iot_puts(s) \
    puts((s))

/**
 * @brief 刷新标准输出缓冲区
 */
#define iot_flush() \
    fflush(stdout)

/*===========================================================
 * 9. 系统接口 (system)
 *===========================================================*/

/**
 * @brief 获取环境变量
 * @param name 环境变量名
 * @return 环境变量值，不存在返回 NULL
 */
#define iot_getenv(name) \
    getenv((name))

/**
 * @brief 设置环境变量
 * @param name 环境变量名
 * @param value 环境变量值
 * @param overwrite 是否覆盖已存在的变量
 * @return 成功返回 0，失败返回 -1
 */
#define iot_setenv(name, value, overwrite) \
    setenv((name), (value), (overwrite))

/**
 * @brief 取消环境变量
 * @param name 环境变量名
 * @return 成功返回 0，失败返回 -1
 */
#define iot_unsetenv(name) \
    unsetenv((name))

/**
 * @brief 执行系统命令
 * @param command 命令字符串
 * @return 命令执行状态
 */
#define iot_system(command) \
    system((command))

#ifdef __cplusplus
}
#endif

#endif /* IOT_PLATFORM_WASM_H */