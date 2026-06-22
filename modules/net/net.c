/**
 * @file net.c
 * @brief 基于 lwip 的 socket 异步接口封装实现
 *
 * 底层使用 lwip 的 socket API 实现，提供异步事件驱动模式
 * 支持 SSL/TLS 安全连接（基于 GmSSL）
 */

#include "net.h"
#include "platform.h"

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

/* GmSSL 支持 */
#include <gmssl/tls.h>
#include <gmssl/x509.h>

/*===========================================================
 * 内部数据结构
 *===========================================================*/

/* socket 上下文 */
struct net_socket {
    int fd;                         /* lwip socket 文件描述符 */
    net_sock_type_t type;           /* socket 类型 */
    net_sock_state_t state;         /* socket 状态 */
    net_socket_callback_t callback; /* 事件回调 */
    void* user_data;                /* 用户数据 */
    
    /* SSL/TLS 支持 */
    bool is_ssl;                    /* 是否为 SSL socket */
    TLS_CTX* tls_ctx;              /* TLS 上下文 */
    TLS* tls;                      /* TLS 连接 */
    net_ssl_config_t ssl_config;   /* SSL 配置 */
    bool ssl_handshake_done;        /* 握手是否完成 */
    char ssl_error[256];            /* SSL 错误信息 */
    
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

/*===========================================================
 * 内部函数声明
 *===========================================================*/

static void socket_trigger_event(struct net_socket* sock, net_event_type_t event);

/*===========================================================
 * socket 管理实现
 *===========================================================*/

static void socket_trigger_event(struct net_socket* sock, net_event_type_t event) {
    if (sock && sock->callback) {
        sock->callback(sock, event, sock->user_data);
    }
}

/*===========================================================
 * SSL 内部函数
 *===========================================================*/

static int ssl_do_handshake(struct net_socket* sock);
static int ssl_socket_read(void* ctx, uint8_t* buf, size_t len);
static int ssl_socket_write(void* ctx, const uint8_t* buf, size_t len);

/*===========================================================
 * socket API 实现
 *===========================================================*/

sock_t net_socket_create(net_sock_type_t type, const net_ssl_config_t* ssl_config,
                        net_socket_callback_t callback, void* user_data) {
    /* 创建 socket 结构 */
    struct net_socket* sock = (struct net_socket*)iot_malloc(sizeof(struct net_socket));
    if (!sock) {
        return INVALID_SOCKET;
    }
    
    /* 创建 lwip socket */
    int domain = PF_INET;  /* IPv4 */
    int socktype = (type == SOCK_TYPE_STREAM) ? SOCK_STREAM : SOCK_DGRAM;
    int protocol = (type == SOCK_TYPE_STREAM) ? IPPROTO_TCP : IPPROTO_UDP;
    
    int fd = lwip_socket(domain, socktype, protocol);
    if (fd < 0) {
        iot_free(sock);
        return INVALID_SOCKET;
    }
    
    /* 初始化 socket 结构 */
    memset(sock, 0, sizeof(struct net_socket));
    sock->fd = fd;
    sock->type = type;
    sock->state = NET_SOCK_STATE_OPENED;
    sock->callback = callback;
    sock->user_data = user_data;
    
    /* 初始化 SSL（如果配置了） */
    if (ssl_config != NULL && type == SOCK_TYPE_STREAM) {
        sock->is_ssl = true;
        memcpy(&sock->ssl_config, ssl_config, sizeof(net_ssl_config_t));
        
        /* 创建 TLS 上下文 */
        sock->tls_ctx = tls_ctx_new();
        if (!sock->tls_ctx) {
            lwip_close(fd);
            iot_free(sock);
            return INVALID_SOCKET;
        }
        
        /* 创建 TLS 连接 */
        sock->tls = tls_new(sock->tls_ctx);
        if (!sock->tls) {
            tls_ctx_free(sock->tls_ctx);
            lwip_close(fd);
            iot_free(sock);
            return INVALID_SOCKET;
        }
        
        /* 设置读写回调 */
        tls_set_read_callback(sock->tls, ssl_socket_read, sock);
        tls_set_write_callback(sock->tls, ssl_socket_write, sock);
        
        /* 设置协议版本 */
        int proto = TLS_protocol_tls12;
        switch (ssl_config->protocol) {
            case NET_SSL_PROTOCOL_TLS1:   proto = TLS_protocol_tls1; break;
            case NET_SSL_PROTOCOL_TLS11:  proto = TLS_protocol_tls11; break;
            case NET_SSL_PROTOCOL_TLS12:  proto = TLS_protocol_tls12; break;
            case NET_SSL_PROTOCOL_TLS13:  proto = TLS_protocol_tls13; break;
            case NET_SSL_PROTOCOL_TLCP:   proto = TLS_protocol_tlcp; break;
            default:                       proto = TLS_protocol_tls12; break;
        }
        tls_set_protocol(sock->tls, proto);
        
        /* 设置加密套件 */
        int cipher_suites[] = {
            TLS_cipher_sm4_gcm_sm3,
            TLS_cipher_ecdhe_sm4_cbc_sm3,
            TLS_cipher_aes_128_gcm_sha256,
            TLS_cipher_aes_256_gcm_sha384,
        };
        size_t cipher_count = sizeof(cipher_suites) / sizeof(cipher_suites[0]);
        
        if (ssl_config->cipher == NET_SSL_CIPHER_SM4) {
            cipher_count = 2;
        } else if (ssl_config->cipher == NET_SSL_CIPHER_AES) {
            cipher_suites[0] = TLS_cipher_aes_128_gcm_sha256;
            cipher_suites[1] = TLS_cipher_aes_256_gcm_sha384;
            cipher_count = 2;
        }
        tls_set_cipher_suites(sock->tls, cipher_suites, cipher_count);
        
        /* 设置证书验证 */
        if (ssl_config->verify_mode == NET_SSL_VERIFY_NONE) {
            tls_set_verify(sock->tls, 0);
        } else {
            tls_set_verify(sock->tls, 1);
            if (ssl_config->ca_file) {
                tls_load_ca_file(sock->tls, ssl_config->ca_file);
            }
            if (ssl_config->ca_dir) {
                tls_load_ca_path(sock->tls, ssl_config->ca_dir);
            }
        }
        
        /* 设置 SNI */
        if (ssl_config->hostname) {
            tls_set_hostname(sock->tls, ssl_config->hostname);
        }
    }
    
    /* 设置非阻塞模式 */
    int nonblock = 1;
    lwip_ioctl(fd, FIONBIO, &nonblock);
    
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
    
    /* TCP 连接建立成功 */
    if (s->is_ssl) {
        /* SSL socket：开始握手，握手完成后才触发 CONNECTED 事件 */
        ssl_do_handshake(s);
    } else {
        s->state = NET_SOCK_STATE_CONNECTED;
        socket_trigger_event(s, NET_EVENT_CONNECTED);
    }
    
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
    
    /* SSL socket：使用 TLS 加密发送 */
    if (s->is_ssl && s->ssl_handshake_done && s->tls) {
        int sent = tls_write(s->tls, (const uint8_t*)data, len);
        if (sent < 0) {
            s->state = NET_SOCK_STATE_ERROR;
            snprintf(s->ssl_error, sizeof(s->ssl_error), "SSL write failed");
            socket_trigger_event(s, NET_EVENT_ERROR);
            return -1;
        }
        return sent;
    }
    
    /* 普通 socket：直接发送数据 */
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
    
    /* SSL socket：使用 TLS 解密接收 */
    if (s->is_ssl && s->ssl_handshake_done && s->tls) {
        int received = tls_read(s->tls, (uint8_t*)buf, len);
        if (received < 0) {
            s->state = NET_SOCK_STATE_ERROR;
            snprintf(s->ssl_error, sizeof(s->ssl_error), "SSL read failed");
            socket_trigger_event(s, NET_EVENT_ERROR);
            return -1;
        }
        if (received == 0) {
            s->state = NET_SOCK_STATE_CLOSED;
            socket_trigger_event(s, NET_EVENT_DISCONNECTED);
        }
        return received;
    }
    
    /* 普通 socket：直接接收数据 */
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
    
    /* SSL 关闭 */
    if (s->is_ssl && s->tls && s->ssl_handshake_done) {
        tls_shutdown(s->tls);
    }
    if (s->tls) {
        tls_free(s->tls);
        s->tls = NULL;
    }
    if (s->tls_ctx) {
        tls_ctx_free(s->tls_ctx);
        s->tls_ctx = NULL;
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
    
    /* 释放 socket 结构内存 */
    iot_free(s);
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
 * SSL 辅助接口
 *===========================================================*/

bool net_socket_is_ssl(sock_t sock) {
    struct net_socket* s = (struct net_socket*)sock;
    return s ? s->is_ssl : false;
}

bool net_socket_ssl_handshake_done(sock_t sock) {
    struct net_socket* s = (struct net_socket*)sock;
    return s ? s->ssl_handshake_done : false;
}

const char* net_socket_ssl_get_error(sock_t sock) {
    struct net_socket* s = (struct net_socket*)sock;
    if (!s) {
        return "Invalid socket";
    }
    return s->ssl_error[0] ? s->ssl_error : "No error";
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
    return 0;
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

/*===========================================================
 * SSL 内部函数实现
 *===========================================================*/

static int ssl_do_handshake(struct net_socket* sock) {
    if (!sock || !sock->tls) {
        return -1;
    }
    
    int ret = tls_do_handshake(sock->tls);
    
    if (ret == 1) {
        /* 握手成功 */
        sock->ssl_handshake_done = true;
        sock->state = NET_SOCK_STATE_CONNECTED;
        socket_trigger_event(sock, NET_EVENT_CONNECTED);
        return 0;
    } else if (ret == -1) {
        /* 握手失败 */
        sock->state = NET_SOCK_STATE_ERROR;
        snprintf(sock->ssl_error, sizeof(sock->ssl_error), "SSL handshake failed");
        socket_trigger_event(sock, NET_EVENT_ERROR);
        return -1;
    }
    
    /* 握手进行中，需要继续 */
    return 1;
}

static int ssl_socket_read(void* ctx, uint8_t* buf, size_t len) {
    struct net_socket* sock = (struct net_socket*)ctx;
    if (!sock || sock->fd < 0) {
        return -1;
    }
    
    /* 读取加密数据 */
    int ret = lwip_recv(sock->fd, buf, len, 0);
    if (ret <= 0) {
        return -1;
    }
    return ret;
}

static int ssl_socket_write(void* ctx, const uint8_t* buf, size_t len) {
    struct net_socket* sock = (struct net_socket*)ctx;
    if (!sock || sock->fd < 0) {
        return -1;
    }
    
    /* 发送加密数据 */
    int ret = lwip_send(sock->fd, buf, len, 0);
    if (ret <= 0) {
        return -1;
    }
    return ret;
}

/*===========================================================
 * SSL 配置接口
 *===========================================================*/

void net_ssl_config_init(net_ssl_config_t* config) {
    if (!config) return;
    
    memset(config, 0, sizeof(net_ssl_config_t));
    config->protocol = NET_SSL_PROTOCOL_AUTO;
    config->cipher = NET_SSL_CIPHER_AUTO;
    config->verify_mode = NET_SSL_VERIFY_OPTIONAL;
    config->handshake_timeout_ms = 30000;
}
