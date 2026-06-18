/**
 * @file iot_socket.c
 * @brief ML307N 平台 Socket 适配器实现文件
 * @details 基于 lwip 库实现统一的 Socket 接口封装，
 *          支持 TCP/UDP Socket 的同步/异步操作。
 */

#include "iot_socket.h"
#include "iot_os.h"
#include "iot_mem.h"
#include "iot_log.h"
#include <string.h>
#include <errno.h>

/* 引入 lwip socket 头文件 */
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"

/* ============================================================
 * Socket 控制块结构
 * ============================================================ */

/** @brief Socket 信息结构体 */
typedef struct {
    int sockfd;                          /**< lwip socket 描述符 */
    iot_socket_callback_t callback;       /**< 事件回调函数 */
    void *user_data;                     /**< 用户数据 */
    iot_socket_type_t type;              /**< Socket 类型 */
    bool is_connected;                   /**< 连接状态 */
} iot_socket_ctrl_t;

/** @brief 最大 Socket 数量 */
#define IOT_SOCKET_MAX_NUM  16

/** @brief Socket 描述符表 */
static iot_socket_ctrl_t g_socket_table[IOT_SOCKET_MAX_NUM];
static bool g_socket_init = false;

/* ============================================================
 * 内部函数声明
 * ============================================================ */

/**
 * @brief 初始化 Socket 模块
 */
static void iot_socket_module_init(void);

/**
 * @brief 根据描述符查找 Socket 控制块
 * @param[in] sock IoT Socket 描述符
 * @return Socket 控制块指针，NULL 表示未找到
 */
static iot_socket_ctrl_t *iot_socket_find_ctrl(iot_socket_t sock);

/**
 * @brief 将 lwip errno 转换为 IoT 错误码
 * @param[in] err lwip errno
 * @return IoT 错误码
 */
static int iot_socket_err_conv(int err);

/**
 * @brief 将 IoT 地址结构转换为 lwip sockaddr
 * @param[in] addr     IoT 地址结构
 * @param[out] sa      lwip sockaddr 存储位置
 * @param[out] salen   地址长度
 * @return 0 成功，<0 失败
 */
static int iot_socket_addr_to_sockaddr(const iot_socket_addr_t *addr, struct sockaddr *sa, socklen_t *salen);

/**
 * @brief 将 lwip sockaddr 转换为 IoT 地址结构
 * @param[in] sa       lwip sockaddr
 * @param[out] addr    IoT 地址结构存储位置
 */
static void iot_socket_sockaddr_to_addr(const struct sockaddr *sa, iot_socket_addr_t *addr);

/* ============================================================
 * 函数实现
 * ============================================================ */

static void iot_socket_module_init(void)
{
    if (!g_socket_init) {
        memset(g_socket_table, 0, sizeof(g_socket_table));
        g_socket_init = true;
    }
}

static iot_socket_ctrl_t *iot_socket_find_ctrl(iot_socket_t sock)
{
    if (sock < 0 || sock >= IOT_SOCKET_MAX_NUM) {
        return NULL;
    }
    if (g_socket_table[sock].sockfd < 0) {
        return NULL;
    }
    return &g_socket_table[sock];
}

static int iot_socket_err_conv(int err)
{
    switch (err) {
        case 0:
            return 0;
        case ENOMEM:
            return IOT_ERR_MEM;
        case EAGAIN:
#if EAGAIN != EWOULDBLOCK
        case EWOULDBLOCK:
#endif
            return IOT_ERR_BUSY;
        case EINVAL:
            return IOT_ERR_PARAM;
        case ENOTCONN:
        case ECONNREFUSED:
        case ETIMEDOUT:
        case ECONNRESET:
            return IOT_ERR;
        default:
            return IOT_ERR;
    }
}

static int iot_socket_addr_to_sockaddr(const iot_socket_addr_t *addr, struct sockaddr *sa, socklen_t *salen)
{
    if (!addr || !sa || !salen) {
        return -1;
    }

    memset(sa, 0, sizeof(struct sockaddr));

    if (addr->family == IOT_AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in *)sa;
        sin->sin_family = AF_INET;
        sin->sin_port = addr->port;
        sin->sin_addr.s_addr = addr->addr.addr4;
        *salen = sizeof(struct sockaddr_in);
    }
#if LWIP_IPV6
    else if (addr->family == IOT_AF_INET6) {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
        sin6->sin6_family = AF_INET6;
        sin6->sin6_port = addr->port;
        memcpy(&sin6->sin6_addr, addr->addr.addr6, 16);
        *salen = sizeof(struct sockaddr_in6);
    }
#endif
    else {
        return -1;
    }

    return 0;
}

static void iot_socket_sockaddr_to_addr(const struct sockaddr *sa, iot_socket_addr_t *addr)
{
    if (!sa || !addr) {
        return;
    }

    memset(addr, 0, sizeof(iot_socket_addr_t));

    if (sa->sa_family == AF_INET) {
        const struct sockaddr_in *sin = (const struct sockaddr_in *)sa;
        addr->family = IOT_AF_INET;
        addr->port = sin->sin_port;
        addr->addr.addr4 = sin->sin_addr.s_addr;
    }
#if LWIP_IPV6
    else if (sa->sa_family == AF_INET6) {
        const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6 *)sa;
        addr->family = IOT_AF_INET6;
        addr->port = sin6->sin6_port;
        memcpy(addr->addr.addr6, &sin6->sin6_addr, 16);
    }
#endif
}

/* ============================================================
 * Socket API 实现
 * ============================================================ */

iot_socket_t iot_socket_create(const iot_socket_config_t *config)
{
    int sockfd;
    int domain;
    int type;
    int protocol;
    iot_socket_ctrl_t *ctrl = NULL;
    int i;

    if (!config) {
        return IOT_INVALID_SOCKET;
    }

    iot_socket_module_init();

    /* 查找空闲槽位 */
    for (i = 0; i < IOT_SOCKET_MAX_NUM; i++) {
        if (g_socket_table[i].sockfd < 0) {
            ctrl = &g_socket_table[i];
            break;
        }
    }
    if (!ctrl) {
        return IOT_INVALID_SOCKET;
    }

    /* 转换协议族 */
    domain = (config->family == IOT_AF_INET6) ? AF_INET6 : AF_INET;

    /* 转换 Socket 类型 */
    switch (config->type) {
        case IOT_SOCK_DGRAM:
            type = SOCK_DGRAM;
            break;
        case IOT_SOCK_RAW:
            type = SOCK_RAW;
            break;
        case IOT_SOCK_STREAM:
        default:
            type = SOCK_STREAM;
            break;
    }

    /* 转换协议 */
    protocol = config->protocol;

    /* 创建 lwip socket */
    sockfd = lwip_socket(domain, type, protocol);
    if (sockfd < 0) {
        return IOT_INVALID_SOCKET;
    }

    /* 设置非阻塞模式 */
    int flags = lwip_fcntl(sockfd, F_GETFL, 0);
    if (flags >= 0) {
        lwip_fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    }

    /* 初始化控制块 */
    ctrl->sockfd = sockfd;
    ctrl->callback = config->callback;
    ctrl->user_data = config->user_data;
    ctrl->type = config->type;
    ctrl->is_connected = false;

    return (iot_socket_t)i;
}

int iot_socket_close(iot_socket_t sock)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl) {
        return IOT_ERR_PARAM;
    }

    int ret = lwip_close(ctrl->sockfd);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    /* 清空控制块 */
    ctrl->sockfd = -1;
    ctrl->callback = NULL;
    ctrl->user_data = NULL;
    ctrl->is_connected = false;

    return 0;
}

int iot_socket_connect(iot_socket_t sock, const iot_socket_addr_t *addr)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl || !addr) {
        return IOT_ERR_PARAM;
    }

    struct sockaddr sa;
    socklen_t salen;
    if (iot_socket_addr_to_sockaddr(addr, &sa, &salen) < 0) {
        return IOT_ERR_PARAM;
    }

    int ret = lwip_connect(ctrl->sockfd, &sa, salen);
    if (ret < 0) {
        if (errno == EINPROGRESS || errno == EAGAIN) {
            /* 非阻塞模式，正在连接中 */
            ctrl->is_connected = false;
            return 0;
        }
        return iot_socket_err_conv(errno);
    }

    ctrl->is_connected = true;
    return 0;
}

int iot_socket_bind(iot_socket_t sock, const iot_socket_addr_t *addr)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl || !addr) {
        return IOT_ERR_PARAM;
    }

    struct sockaddr sa;
    socklen_t salen;
    if (iot_socket_addr_to_sockaddr(addr, &sa, &salen) < 0) {
        return IOT_ERR_PARAM;
    }

    int ret = lwip_bind(ctrl->sockfd, &sa, salen);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    return 0;
}

int iot_socket_listen(iot_socket_t sock, int backlog)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl) {
        return IOT_ERR_PARAM;
    }

    int ret = lwip_listen(ctrl->sockfd, backlog);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    return 0;
}

iot_socket_t iot_socket_accept(iot_socket_t sock, iot_socket_addr_t *addr)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl) {
        return IOT_INVALID_SOCKET;
    }

    struct sockaddr sa;
    socklen_t salen = sizeof(sa);
    int clientfd = lwip_accept(ctrl->sockfd, &sa, &salen);
    if (clientfd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return IOT_INVALID_SOCKET;
        }
        return IOT_INVALID_SOCKET;
    }

    /* 查找空闲槽位 */
    iot_socket_ctrl_t *new_ctrl = NULL;
    int i;
    for (i = 0; i < IOT_SOCKET_MAX_NUM; i++) {
        if (g_socket_table[i].sockfd < 0) {
            new_ctrl = &g_socket_table[i];
            break;
        }
    }
    if (!new_ctrl) {
        lwip_close(clientfd);
        return IOT_INVALID_SOCKET;
    }

    /* 初始化新控制块 */
    new_ctrl->sockfd = clientfd;
    new_ctrl->callback = ctrl->callback;
    new_ctrl->user_data = ctrl->user_data;
    new_ctrl->type = ctrl->type;
    new_ctrl->is_connected = true;

    /* 填充客户端地址 */
    if (addr) {
        iot_socket_sockaddr_to_addr(&sa, addr);
    }

    return (iot_socket_t)i;
}

int iot_socket_send(iot_socket_t sock, const void *data, size_t len, int flags)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl || !data) {
        return IOT_ERR_PARAM;
    }

    int ret = lwip_send(ctrl->sockfd, data, len, flags);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    return ret;
}

int iot_socket_recv(iot_socket_t sock, void *buf, size_t len, int flags)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl || !buf) {
        return IOT_ERR_PARAM;
    }

    int ret = lwip_recv(ctrl->sockfd, buf, len, flags);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    return ret;
}

int iot_socket_sendto(iot_socket_t sock, const void *data, size_t len,
                       const iot_socket_addr_t *addr, int flags)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl || !data) {
        return IOT_ERR_PARAM;
    }

    if (addr) {
        struct sockaddr sa;
        socklen_t salen;
        if (iot_socket_addr_to_sockaddr(addr, &sa, &salen) < 0) {
            return IOT_ERR_PARAM;
        }
        return lwip_sendto(ctrl->sockfd, data, len, flags, &sa, salen);
    } else {
        return lwip_send(ctrl->sockfd, data, len, flags);
    }
}

int iot_socket_recvfrom(iot_socket_t sock, void *buf, size_t len,
                        iot_socket_addr_t *addr, int flags)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl || !buf) {
        return IOT_ERR_PARAM;
    }

    struct sockaddr sa;
    socklen_t salen = sizeof(sa);
    int ret = lwip_recvfrom(ctrl->sockfd, buf, len, flags, &sa, &salen);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    if (addr) {
        iot_socket_sockaddr_to_addr(&sa, addr);
    }

    return ret;
}

int iot_socket_shutdown(iot_socket_t sock, int how)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl) {
        return IOT_ERR_PARAM;
    }

    int ret = lwip_shutdown(ctrl->sockfd, how);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    return 0;
}

int iot_socket_get_local_addr(iot_socket_t sock, iot_socket_addr_t *addr)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl || !addr) {
        return IOT_ERR_PARAM;
    }

    struct sockaddr sa;
    socklen_t salen = sizeof(sa);
    int ret = lwip_getsockname(ctrl->sockfd, &sa, &salen);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    iot_socket_sockaddr_to_addr(&sa, addr);
    return 0;
}

int iot_socket_get_peer_addr(iot_socket_t sock, iot_socket_addr_t *addr)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl || !addr) {
        return IOT_ERR_PARAM;
    }

    struct sockaddr sa;
    socklen_t salen = sizeof(sa);
    int ret = lwip_getpeername(ctrl->sockfd, &sa, &salen);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    iot_socket_sockaddr_to_addr(&sa, addr);
    return 0;
}

int iot_socket_setsockopt(iot_socket_t sock, int level, int optname,
                                const void *optval, socklen_t optlen)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl) {
        return IOT_ERR_PARAM;
    }

    int ret = lwip_setsockopt(ctrl->sockfd, level, optname, optval, optlen);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    return 0;
}

int iot_socket_getsockopt(iot_socket_t sock, int level, int optname,
                                void *optval, socklen_t *optlen)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl || !optval || !optlen) {
        return IOT_ERR_PARAM;
    }

    int ret = lwip_getsockopt(ctrl->sockfd, level, optname, optval, optlen);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    return 0;
}

bool iot_socket_is_connected(iot_socket_t sock)
{
    iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(sock);
    if (!ctrl) {
        return false;
    }

    if (!ctrl->is_connected) {
        return false;
    }

    /* 检查 socket 状态 */
    int err = 0;
    socklen_t errlen = sizeof(err);
    if (lwip_getsockopt(ctrl->sockfd, SOL_SOCKET, SO_ERROR, &err, &errlen) < 0) {
        return false;
    }

    return (err == 0);
}

int iot_socket_select(iot_socket_t *reads, int read_count,
                            iot_socket_t *writes, int write_count,
                            uint32_t timeout_ms)
{
    fd_set readfds, writefds;
    int maxfd = -1;
    struct timeval tv;
    int ret, i;

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    /* 添加读 socket */
    if (reads && read_count > 0) {
        for (i = 0; i < read_count; i++) {
            iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(reads[i]);
            if (ctrl && ctrl->sockfd >= 0) {
                FD_SET(ctrl->sockfd, &readfds);
                if (ctrl->sockfd > maxfd) {
                    maxfd = ctrl->sockfd;
                }
            }
        }
    }

    /* 添加写 socket */
    if (writes && write_count > 0) {
        for (i = 0; i < write_count; i++) {
            iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(writes[i]);
            if (ctrl && ctrl->sockfd >= 0) {
                FD_SET(ctrl->sockfd, &writefds);
                if (ctrl->sockfd > maxfd) {
                    maxfd = ctrl->sockfd;
                }
            }
        }
    }

    if (maxfd < 0) {
        return IOT_ERR_PARAM;
    }

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ret = lwip_select(maxfd + 1, &readfds, &writefds, NULL, &tv);
    if (ret < 0) {
        return iot_socket_err_conv(errno);
    }

    /* 更新返回的 socket 数组 */
    if (reads && read_count > 0) {
        int j = 0;
        for (i = 0; i < read_count; i++) {
            iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(reads[i]);
            if (ctrl && ctrl->sockfd >= 0 && FD_ISSET(ctrl->sockfd, &readfds)) {
                reads[j++] = reads[i];
            }
        }
    }

    if (writes && write_count > 0) {
        int j = 0;
        for (i = 0; i < write_count; i++) {
            iot_socket_ctrl_t *ctrl = iot_socket_find_ctrl(writes[i]);
            if (ctrl && ctrl->sockfd >= 0 && FD_ISSET(ctrl->sockfd, &writefds)) {
                writes[j++] = writes[i];
            }
        }
    }

    return 0;
}

int iot_socket_get_host_by_name(const char *hostname, uint32_t *ipaddr)
{
    if (!hostname || !ipaddr) {
        return IOT_ERR_PARAM;
    }

    struct hostent *he = lwip_gethostbyname(hostname);
    if (!he) {
        return IOT_ERR;
    }

    struct in_addr *addr = (struct in_addr *)he->h_addr_list[0];
    if (!addr) {
        return IOT_ERR;
    }

    *ipaddr = addr->s_addr;
    return 0;
}

int iot_socket_inet_pton(int af, const char *src, void *dst)
{
    int family;

    if (af == IOT_AF_INET6) {
        family = AF_INET6;
    } else {
        family = AF_INET;
    }

    return lwip_inet_pton(family, src, dst);
}

const char *iot_socket_inet_ntop(int af, const void *src, char *dst, size_t size)
{
    int family;

    if (af == IOT_AF_INET6) {
        family = AF_INET6;
    } else {
        family = AF_INET;
    }

    return lwip_inet_ntop(family, src, dst, size);
}

uint16_t iot_socket_htons(uint16_t hostshort)
{
    return lwip_htons(hostshort);
}

uint16_t iot_socket_ntohs(uint16_t netshort)
{
    return lwip_ntohs(netshort);
}

uint32_t iot_socket_htonl(uint32_t hostlong)
{
    return lwip_htonl(hostlong);
}

uint32_t iot_socket_ntohl(uint32_t netlong)
{
    return lwip_ntohl(netlong);
}
