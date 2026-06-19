/**
 * @file net.c
 * @brief 基于 lwip 的 socket 异步接口封装实现
 *
 * 底层使用 lwip 的 socket API 实现，提供异步事件驱动模式
 */

#include "net.h"
#include "iot_os.h"
#include "iot_mem.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 检查 lwip 配置 */
#ifdef LWIP_SOCKET
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"
#else
#error "Please enable LWIP_SOCKET in lwipopts.h"
#endif

/*===========================================================
 * 内部数据结构
 *===========================================================*/

#define MAX_SOCKET_COUNT 32

/* socket 上下文 */
struct net_socket {
    int fd;                         /* lwip socket 文件描述符 */
    net_sock_type_t type;           /* socket 类型 */
    net_sock_state_t state;         /* socket 状态 */
    net_socket_callback_t callback; /* 事件回调 */
    void* user_data;                /* 用户数据 */
    
    /* 接收缓冲区 */
    char* recv_buf;
    size_t recv_len;
    size_t recv_capacity;
    
    /* 发送队列 */
    char* send_buf;
    size_t send_len;
    size_t send_capacity;
    
    /* 链表节点 */
    struct net_socket* next;
};

/* 网络管理器 */
static struct {
    struct net_socket* sockets;     /* socket 链表 */
    int socket_count;              /* socket 数量 */
    iot_mutex_t mutex;              /* 互斥锁 */
    int fd_readset[FD_SETSIZE];    /* 读文件描述符集合 */
    int fd_writeset[FD_SETSIZE];    /* 写文件描述符集合 */
    int fd_max;                     /* 最大 fd */
    int initialized;               /* 初始化标志 */
} g_net_mgr;

/*===========================================================
 * 内部函数声明
 *===========================================================*/

static struct net_socket* socket_find_by_fd(int fd);
static struct net_socket* socket_find_free(void);
static void socket_add(struct net_socket* sock);
static void socket_remove(struct net_socket* sock);
static int socket_init_manager(void);
static void socket_trigger_event(struct net_socket* sock, net_event_type_t event);

/*===========================================================
 * socket 管理实现
 *===========================================================*/

static struct net_socket* socket_find_by_fd(int fd) {
    struct net_socket* sock = g_net_mgr.sockets;
    while (sock) {
        if (sock->fd == fd) {
            return sock;
        }
        sock = sock->next;
    }
    return NULL;
}

static struct net_socket* socket_find_free(void) {
    struct net_socket* sock = g_net_mgr.sockets;
    while (sock) {
        if (sock->fd < 0) {
            return sock;
        }
        sock = sock->next;
    }
    
    /* 尝试分配新的 socket */
    if (g_net_mgr.socket_count < MAX_SOCKET_COUNT) {
        struct net_socket* new_sock = (struct net_socket*)iot_malloc(sizeof(struct net_socket));
        if (new_sock) {
            memset(new_sock, 0, sizeof(struct net_socket));
            new_sock->fd = -1;
            new_sock->next = g_net_mgr.sockets;
            g_net_mgr.sockets = new_sock;
            g_net_mgr.socket_count++;
            return new_sock;
        }
    }
    
    return NULL;
}

static void socket_add(struct net_socket* sock) {
    iot_mutex_lock(g_net_mgr.mutex, -1);
    
    /* 确保在链表中 */
    struct net_socket* p = g_net_mgr.sockets;
    while (p) {
        if (p == sock) {
            iot_mutex_unlock(g_net_mgr.mutex);
            return;
        }
        p = p->next;
    }
    
    sock->next = g_net_mgr.sockets;
    g_net_mgr.sockets = sock;
    g_net_mgr.socket_count++;
    
    iot_mutex_unlock(g_net_mgr.mutex);
}

static void socket_remove(struct net_socket* sock) {
    iot_mutex_lock(g_net_mgr.mutex, -1);
    
    struct net_socket** pp = &g_net_mgr.sockets;
    while (*pp) {
        if (*pp == sock) {
            *pp = sock->next;
            g_net_mgr.socket_count--;
            break;
        }
        pp = &(*pp)->next;
    }
    
    iot_mutex_unlock(g_net_mgr.mutex);
}

static int socket_init_manager(void) {
    if (!g_net_mgr.initialized) {
        memset(&g_net_mgr, 0, sizeof(g_net_mgr));
        g_net_mgr.mutex = iot_mutex_create();
        if (!g_net_mgr.mutex) {
            return -1;
        }
        g_net_mgr.initialized = 1;
        g_net_mgr.fd_max = -1;
    }
    return 0;
}

static void socket_trigger_event(struct net_socket* sock, net_event_type_t event) {
    if (sock && sock->callback) {
        sock->callback(sock, event, sock->user_data);
    }
}

/*===========================================================
 * socket API 实现
 *===========================================================*/

sock_t net_socket_create(net_sock_type_t type, net_socket_callback_t callback, void* user_data) {
    if (socket_init_manager() != 0) {
        return INVALID_SOCKET;
    }
    
    struct net_socket* sock = socket_find_free();
    if (!sock) {
        return INVALID_SOCKET;
    }
    
    /* 创建 lwip socket */
    int domain = PF_INET;  /* IPv4 */
    int socktype = (type == SOCK_TYPE_STREAM) ? SOCK_STREAM : SOCK_DGRAM;
    int protocol = (type == SOCK_TYPE_STREAM) ? IPPROTO_TCP : IPPROTO_UDP;
    
    int fd = lwip_socket(domain, socktype, protocol);
    if (fd < 0) {
        return INVALID_SOCKET;
    }
    
    /* 初始化 socket 结构 */
    sock->fd = fd;
    sock->type = type;
    sock->state = NET_SOCK_STATE_OPENED;
    sock->callback = callback;
    sock->user_data = user_data;
    
    /* 设置非阻塞模式 */
    int nonblock = 1;
    lwip_ioctl(fd, FIONBIO, &nonblock);
    
    socket_add(sock);
    
    return (sock_t)sock;
}

int net_socket_bind(sock_t sock, const char* ip, uint16_t port) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s || s->fd < 0) {
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (ip) {
        addr.sin_addr.s_addr = inet_addr(ip);
    } else {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    int ret = lwip_bind(s->fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        s->state = NET_SOCK_STATE_ERROR;
        return -1;
    }
    
    return 0;
}

int net_socket_listen(sock_t sock, int backlog) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s || s->fd < 0) {
        return -1;
    }
    
    int ret = lwip_listen(s->fd, backlog);
    if (ret < 0) {
        s->state = NET_SOCK_STATE_ERROR;
        return -1;
    }
    
    s->state = NET_SOCK_STATE_LISTENING;
    return 0;
}

int net_socket_connect(sock_t sock, const char* ip, uint16_t port) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s || s->fd < 0) {
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    
    s->state = NET_SOCK_STATE_CONNECTING;
    
    int ret = lwip_connect(s->fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        /* 非阻塞模式下，连接会异步完成 */
        if (errno == EINPROGRESS || errno == EWOULDBLOCK) {
            return 0;  /* 异步连接启动成功 */
        }
        s->state = NET_SOCK_STATE_ERROR;
        return -1;
    }
    
    s->state = NET_SOCK_STATE_CONNECTED;
    socket_trigger_event(s, NET_EVENT_CONNECTED);
    return 0;
}

sock_t net_socket_accept(sock_t listen_sock, net_socket_callback_t callback, void* user_data) {
    struct net_socket* listen_socket = (struct net_socket*)listen_sock;
    if (!listen_socket || listen_socket->fd < 0) {
        return INVALID_SOCKET;
    }
    
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    int client_fd = lwip_accept(listen_socket->fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            return INVALID_SOCKET;
        }
        return INVALID_SOCKET;  /* 非阻塞模式下暂时没有连接 */
    }
    
    /* 创建 client socket 结构 */
    struct net_socket* client_sock = socket_find_free();
    if (!client_sock) {
        lwip_close(client_fd);
        return INVALID_SOCKET;
    }
    
    /* 初始化 client socket */
    client_sock->fd = client_fd;
    client_sock->type = listen_socket->type;
    client_sock->state = NET_SOCK_STATE_CONNECTED;
    client_sock->callback = callback;
    client_sock->user_data = user_data;
    
    /* 设置非阻塞模式 */
    int nonblock = 1;
    lwip_ioctl(client_fd, FIONBIO, &nonblock);
    
    socket_add(client_sock);
    
    /* 触发 accept 事件 */
    socket_trigger_event(client_sock, NET_EVENT_ACCEPT);
    socket_trigger_event(client_sock, NET_EVENT_CONNECTED);
    
    return (sock_t)client_sock;
}

int net_socket_send(sock_t sock, const void* data, size_t len) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s || s->fd < 0 || !data || len == 0) {
        return -1;
    }
    
    /* 直接发送数据 */
    int sent = lwip_send(s->fd, data, len, 0);
    if (sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            /* 发送缓冲区满，异步等待 */
            return 0;
        }
        s->state = NET_SOCK_STATE_ERROR;
        socket_trigger_event(s, NET_EVENT_ERROR);
        return -1;
    }
    
    if (sent < (int)len) {
        /* 发送部分数据，触发可写事件等待继续发送 */
        socket_trigger_event(s, NET_EVENT_SEND);
    }
    
    return sent;
}

int net_socket_recv(sock_t sock, void* buf, size_t len) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s || s->fd < 0 || !buf || len == 0) {
        return -1;
    }
    
    int received = lwip_recv(s->fd, buf, len, 0);
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  /* 暂时没有数据 */
        }
        s->state = NET_SOCK_STATE_ERROR;
        socket_trigger_event(s, NET_EVENT_ERROR);
        return -1;
    }
    
    if (received == 0) {
        /* 连接关闭 */
        s->state = NET_SOCK_STATE_CLOSED;
        socket_trigger_event(s, NET_EVENT_DISCONNECTED);
        return 0;
    }
    
    return received;
}

void net_socket_close(sock_t sock) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s) {
        return;
    }
    
    if (s->fd >= 0) {
        lwip_close(s->fd);
        s->fd = -1;
    }
    
    /* 释放缓冲区 */
    if (s->recv_buf) {
        iot_free(s->recv_buf);
        s->recv_buf = NULL;
    }
    if (s->send_buf) {
        iot_free(s->send_buf);
        s->send_buf = NULL;
    }
    
    s->state = NET_SOCK_STATE_CLOSED;
    s->recv_len = 0;
    s->send_len = 0;
    s->recv_capacity = 0;
    s->send_capacity = 0;
}

net_sock_state_t net_socket_get_state(sock_t sock) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s) {
        return NET_SOCK_STATE_CLOSED;
    }
    return s->state;
}

int net_socket_setopt(sock_t sock, int optname, const void* optval, size_t optlen) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s || s->fd < 0) {
        return -1;
    }
    
    return lwip_setsockopt(s->fd, SOL_SOCKET, optname, optval, optlen);
}

int net_socket_get_local_addr(sock_t sock, char* ip, size_t ip_len, uint16_t* port) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s || s->fd < 0) {
        return -1;
    }
    
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    
    if (lwip_getsockname(s->fd, (struct sockaddr*)&addr, &addr_len) < 0) {
        return -1;
    }
    
    if (ip && ip_len > 0) {
        snprintf(ip, ip_len, "%s", inet_ntoa(addr.sin_addr));
    }
    if (port) {
        *port = ntohs(addr.sin_port);
    }
    
    return 0;
}

int net_socket_get_remote_addr(sock_t sock, char* ip, size_t ip_len, uint16_t* port) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s || s->fd < 0) {
        return -1;
    }
    
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    
    if (lwip_getpeername(s->fd, (struct sockaddr*)&addr, &addr_len) < 0) {
        return -1;
    }
    
    if (ip && ip_len > 0) {
        snprintf(ip, ip_len, "%s", inet_ntoa(addr.sin_addr));
    }
    if (port) {
        *port = ntohs(addr.sin_port);
    }
    
    return 0;
}

/*===========================================================
 * DNS 解析实现
 *===========================================================*/

int net_dns_resolve(const char* name, net_dns_callback_t callback, void* user_data) {
    if (!name || !callback) {
        return -1;
    }
    
    /* 使用 lwip 的 DNS 解析 */
    /* 注意：lwip 的 dns_gethostbyname 是同步的，这里简化为同步解析 */
    struct hostent* he = lwip_gethostbyname(name);
    if (he) {
        struct in_addr* addr = (struct in_addr*)he->h_addr_list[0];
        if (addr) {
            char ip[32];
            snprintf(ip, sizeof(ip), "%s", inet_ntoa(*addr));
            callback(name, ip, user_data);
            return 0;
        }
    }
    
    callback(name, NULL, user_data);
    return -1;
}

/*===========================================================
 * 网络初始化实现
 *===========================================================*/

int net_init(void) {
    if (socket_init_manager() != 0) {
        return -1;
    }
    return 0;
}

void net_deinit(void) {
    iot_mutex_lock(g_net_mgr.mutex, -1);
    
    /* 关闭所有 socket */
    struct net_socket* sock = g_net_mgr.sockets;
    while (sock) {
        if (sock->fd >= 0) {
            lwip_close(sock->fd);
            sock->fd = -1;
        }
        if (sock->recv_buf) {
            iot_free(sock->recv_buf);
            sock->recv_buf = NULL;
        }
        if (sock->send_buf) {
            iot_free(sock->send_buf);
            sock->send_buf = NULL;
        }
        sock = sock->next;
    }
    
    iot_mutex_unlock(g_net_mgr.mutex);
    
    if (g_net_mgr.mutex) {
        iot_mutex_delete(g_net_mgr.mutex);
        g_net_mgr.mutex = NULL;
    }
    
    g_net_mgr.initialized = 0;
}

void net_poll(int timeout_ms) {
    if (!g_net_mgr.initialized) {
        return;
    }
    
    iot_mutex_lock(g_net_mgr.mutex, -1);
    
    /* 构建 fd_set */
    fd_set readfds, writefds, errorfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&errorfds);
    
    int maxfd = -1;
    struct net_socket* sock = g_net_mgr.sockets;
    
    while (sock) {
        if (sock->fd >= 0) {
            FD_SET(sock->fd, &readfds);
            FD_SET(sock->fd, &writefds);
            FD_SET(sock->fd, &errorfds);
            
            if (sock->fd > maxfd) {
                maxfd = sock->fd;
            }
        }
        sock = sock->next;
    }
    
    iot_mutex_unlock(g_net_mgr.mutex);
    
    if (maxfd < 0) {
        /* 没有 socket需要轮询，sleep 一下 */
        iot_task_sleep(timeout_ms);
        return;
    }
    
    /* 等待事件 */
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    
    int ret = lwip_select(maxfd + 1, &readfds, &writefds, &errorfds, &tv);
    
    if (ret <= 0) {
        return;  /* 超时或错误 */
    }
    
    iot_mutex_lock(g_net_mgr.mutex, -1);
    
    sock = g_net_mgr.sockets;
    while (sock) {
        if (sock->fd >= 0) {
            /* 检查错误 */
            if (FD_ISSET(sock->fd, &errorfds)) {
                int err;
                socklen_t len = sizeof(err);
                lwip_getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, &err, &len);
                if (err != 0) {
                    if (sock->state == NET_SOCK_STATE_CONNECTING) {
                        sock->state = NET_SOCK_STATE_ERROR;
                        socket_trigger_event(sock, NET_EVENT_ERROR);
                    }
                }
            }
            
            /* 检查可读 */
            if (FD_ISSET(sock->fd, &readfds)) {
                if (sock->state == NET_SOCK_STATE_LISTENING) {
                    /* 监听 socket 收到连接，在 accept 时处理 */
                } else {
                    socket_trigger_event(sock, NET_EVENT_RECV);
                }
            }
            
            /* 检查可写 */
            if (FD_ISSET(sock->fd, &writefds)) {
                if (sock->state == NET_SOCK_STATE_CONNECTING) {
                    sock->state = NET_SOCK_STATE_CONNECTED;
                    socket_trigger_event(sock, NET_EVENT_CONNECTED);
                } else {
                    socket_trigger_event(sock, NET_EVENT_SEND);
                }
            }
        }
        sock = sock->next;
    }
    
    iot_mutex_unlock(g_net_mgr.mutex);
}

/*===========================================================
 * 辅助函数实现
 *===========================================================*/

uint32_t net_inet_addr(const char* ip) {
    if (!ip) {
        return 0;
    }
    return inet_addr(ip);
}

const char* net_inet_ntoa(uint32_t addr, char* buf, size_t len) {
    if (!buf || len < 16) {
        return NULL;
    }
    
    struct in_addr in;
    in.s_addr = addr;
    snprintf(buf, len, "%s", inet_ntoa(in));
    return buf;
}

int net_gethostbyname(const char* hostname, char* ip) {
    if (!hostname || !ip) {
        return -1;
    }
    
    struct hostent* he = lwip_gethostbyname(hostname);
    if (!he) {
        return -1;
    }
    
    struct in_addr* addr = (struct in_addr*)he->h_addr_list[0];
    if (!addr) {
        return -1;
    }
    
    snprintf(ip, 16, "%s", inet_ntoa(*addr));
    return 0;
}
