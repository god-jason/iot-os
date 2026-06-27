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

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 1. 内存管理接口 (mem)
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
 * 2. 互斥锁接口 (mutex)
 *===========================================================*/

#define iot_mutex_t              int

#define iot_mutex_create() \
    (1)

#define iot_mutex_lock(mutex, timeout_ms) \
    (1)

#define iot_mutex_unlock(mutex) \
    (1)

#define iot_mutex_delete(mutex) \
    do {} while(0)

/*===========================================================
 * 3. 信号量接口 (semaphore)
 *===========================================================*/

#define iot_sem_t                int

#define iot_sem_create(max_count, initial_count) \
    ((initial_count))

#define iot_sem_wait(sem) \
    (1)

#define iot_sem_wait_timeout(sem, timeout_ms) \
    (1)

#define iot_sem_post(sem) \
    (1)

#define iot_sem_get_count(sem) \
    (sem)

#define iot_sem_delete(sem) \
    do {} while(0)

/*===========================================================
 * 4. 任务接口 (task)
 *===========================================================*/

#define iot_task_t               int

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

#define iot_task_func_t          void (*)(void*)

#define iot_task_create(name, func, arg, stack_size, priority) ({ \
    emscripten_async_call((func), (arg), 0); \
    1; \
})

#define iot_task_delay(ms) \
    emscripten_sleep((ms))

#define iot_task_delete(task) \
    do {} while(0)

#define iot_task_get_current() \
    (1)

#define iot_get_tick_ms() \
    emscripten_get_now()

#define iot_get_tick_freq() \
    (1000)

#define iot_task_exit() \
    do {} while(0)

/*===========================================================
 * 5. 定时器接口 (timer)
 *===========================================================*/

#define iot_timer_t              int

#define iot_timer_type_t         int
#define IOT_TIMER_ONCE           0
#define IOT_TIMER_PERIODIC       1

#define iot_timer_callback_t     void (*)(void*)

#define iot_timer_create(cb, arg, period_ms, type) ({ \
    if ((type) == IOT_TIMER_PERIODIC) { \
        emscripten_set_interval((cb), (arg), (period_ms)); \
    } else { \
        emscripten_set_timeout((cb), (arg), (period_ms)); \
    } \
})

#define iot_timer_start(timer, period_ms) \
    (0)

#define iot_timer_stop(timer) \
    emscripten_clear_interval((timer))

#define iot_timer_delete(timer) \
    emscripten_clear_interval((timer))

#define iot_timer_is_running(timer) \
    ((timer) != 0)

/*===========================================================
 * 6. Socket 网络接口 (socket)
 *===========================================================*/

#define iot_socket_t              int

#define IOT_SOCKET_INVALID        (-1)

#define IOT_SOCK_STREAM           1
#define IOT_SOCK_DGRAM            2
#define IOT_SOCK_RAW              3

#define IOT_IPPROTO_TCP           6
#define IOT_IPPROTO_UDP           17
#define IOT_IPPROTO_IP            0

#define IOT_SOL_SOCKET            1

#define IOT_SO_REUSEADDR          1
#define IOT_SO_KEEPALIVE          2
#define IOT_SO_RCVBUF             3
#define IOT_SO_SNDBUF             4
#define IOT_SO_ERROR              5
#define IOT_TCP_NODELAY           6

#define IOT_SHUT_RD               0
#define IOT_SHUT_WR               1
#define IOT_SHUT_RDWR             2

#define iot_sockaddr_t            struct sockaddr
#define iot_sockaddr_in_t         struct sockaddr_in
#define iot_sockaddr_in6_t        struct sockaddr_in6
#define iot_in_addr_t             struct in_addr
#define iot_in6_addr_t            struct in6_addr
#define iot_socklen_t             int

#define iot_socket(domain, type, protocol) \
    (IOT_SOCKET_INVALID)

#define iot_socketpair(domain, type, protocol, sv) \
    (-1)

#define iot_close(sock) \
    (0)

#define iot_shutdown(sock, how) \
    (0)

#define iot_bind(sock, addr, addrlen) \
    (-1)

#define iot_listen(sock, backlog) \
    (-1)

#define iot_accept(sock, addr, addrlen) \
    (IOT_SOCKET_INVALID)

#define iot_connect(sock, addr, addrlen) \
    (-1)

#define iot_send(sock, buf, len, flags) \
    (-1)

#define iot_sendto(sock, buf, len, flags, dest_addr, dest_len) \
    (-1)

#define iot_recv(sock, buf, len, flags) \
    (-1)

#define iot_recvfrom(sock, buf, len, flags, src_addr, src_len) \
    (-1)

#define iot_setsockopt(sock, level, optname, optval, optlen) \
    (0)

#define iot_getsockopt(sock, level, optname, optval, optlen) \
    (0)

#define iot_getpeername(sock, addr, addrlen) \
    (-1)

#define iot_getsockname(sock, addr, addrlen) \
    (-1)

#define iot_fd_set_t              int

#define iot_fd_set(fd, set) \
    do {} while(0)

#define iot_fd_clr(fd, set) \
    do {} while(0)

#define iot_fd_isset(fd, set) \
    (0)

#define iot_fd_zero(set) \
    do {} while(0)

#define iot_select_fd_set_max     1024

#define iot_select(nfds, readfds, writefds, exceptfds, timeout) \
    (-1)

#define iot_inet_addr(cp) \
    (0)

#define iot_inet_aton(cp, inp) \
    (0)

#define iot_inet_ntoa(addr) \
    ("0.0.0.0")

#define iot_inet_ntop(af, src, dst, size) \
    (NULL)

#define iot_inet_pton(af, src, dst) \
    (0)

#define iot_gethostbyname(name) \
    (NULL)

#define iot_gethostbyaddr(addr, len, type) \
    (NULL)

#define iot_socket_errno() \
    (0)

#define iot_socket_error() \
    (0)

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

int net_port_set_nonblocking(iot_socket_t fd);

static inline int iot_socket_init(void) {
    return 0;
}

/*===========================================================
 * 7. 文件系统接口 (fs)
 *===========================================================*/

#define iot_fs_stat_t             struct stat

#define iot_fs_open(path, flags, mode) \
    open((path), (flags), (mode))

#define iot_fs_close(fd) \
    close((fd))

#define iot_fs_read(fd, buf, count) \
    read((fd), (buf), (count))

#define iot_fs_write(fd, buf, count) \
    write((fd), (buf), (count))

#define iot_fs_lseek(fd, offset, whence) \
    lseek((fd), (offset), (whence))

#define iot_fs_stat(path, buf) \
    stat((path), (buf))

#define iot_fs_fstat(fd, buf) \
    fstat((fd), (buf))

#define iot_fs_mkdir(path, mode) \
    mkdir((path), (mode))

#define iot_fs_rmdir(path) \
    rmdir((path))

#define iot_fs_unlink(path) \
    unlink((path))

#define iot_fs_rename(oldpath, newpath) \
    rename((oldpath), (newpath))

#define iot_fs_access(path, mode) \
    access((path), (mode))

#define iot_fs_opendir(path) \
    opendir((path))

#define iot_fs_closedir(dir) \
    closedir((dir))

#define iot_fs_readdir(dir) \
    readdir((dir))

#define iot_fs_telldir(dir) \
    telldir((dir))

#define iot_fs_seekdir(dir, loc) \
    seekdir((dir), (loc))

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

#define iot_printf(...) \
    printf(__VA_ARGS__)

#define iot_fprintf(stream, ...) \
    fprintf((stream), __VA_ARGS__)

#define iot_vprintf(format, ap) \
    vprintf((format), (ap))

#define iot_vfprintf(stream, format, ap) \
    vfprintf((stream), (format), (ap))

#define iot_putchar(c) \
    putchar((c))

#define iot_puts(s) \
    puts((s))

#define iot_flush() \
    fflush(stdout)

/*===========================================================
 * 9. 系统接口 (system)
 *===========================================================*/

#define iot_getenv(name) \
    getenv((name))

#define iot_setenv(name, value, overwrite) \
    setenv((name), (value), (overwrite))

#define iot_unsetenv(name) \
    unsetenv((name))

#define iot_system(command) \
    system((command))

#ifdef __cplusplus
}
#endif

#endif /* IOT_PLATFORM_WASM_H */