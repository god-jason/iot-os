#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "net.h"
#include "platform.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/inet.h"

#include "gmssl/tls.h"
#include "gmssl/x509.h"

#define NET_SOCKET_RECV_BUF_SIZE 4096
#define NET_SOCKET_SEND_BUF_SIZE 4096
#define NET_POLL_INTERVAL_MS 10

struct net_socket {
    int fd;
    net_sock_type_t type;
    net_sock_state_t state;
    net_socket_callback_t callback;
    void* user_data;

    bool is_ssl;
    TLS_CTX* tls_ctx;
    TLS* tls;
    net_ssl_config_t ssl_config;
    bool ssl_handshake_done;
    char ssl_error[256];

    char* recv_buf;
    size_t recv_len;
    size_t recv_cap;

    char* send_buf;
    size_t send_len;

    struct net_socket* next;
};

static struct net_socket* s_socket_list = NULL;
static iot_mutex_t s_socket_mutex;
static iot_task_t s_net_task;
static bool s_net_running = false;

static void net_socket_set_state(net_socket_t* sock, net_sock_state_t state);
static void net_socket_trigger_event(net_socket_t* sock, net_event_type_t event);
static int net_socket_set_nonblocking(int fd);
static int ssl_init_client(net_socket_t* sock);
static int ssl_do_handshake(net_socket_t* sock);
static void net_socket_add(net_socket_t* sock);
static void net_socket_remove(net_socket_t* sock);
static void net_poll_thread(void* arg);

sock_t net_socket_create(net_sock_type_t type, const net_ssl_config_t* ssl_config,
                        net_socket_callback_t callback, void* user_data)
{
    struct net_socket* sock = (struct net_socket*)iot_malloc(sizeof(struct net_socket));
    if (!sock) {
        return INVALID_SOCKET;
    }

    memset(sock, 0, sizeof(struct net_socket));

    int domain = PF_INET;
    int socktype = (type == SOCK_TYPE_STREAM) ? SOCK_STREAM : SOCK_DGRAM;
    int protocol = (type == SOCK_TYPE_STREAM) ? IPPROTO_TCP : IPPROTO_UDP;

    int fd = lwip_socket(domain, socktype, protocol);
    if (fd < 0) {
        iot_free(sock);
        return INVALID_SOCKET;
    }

    if (net_socket_set_nonblocking(fd) != 0) {
        lwip_close(fd);
        iot_free(sock);
        return INVALID_SOCKET;
    }

    sock->fd = fd;
    sock->type = type;
    sock->state = NET_SOCK_STATE_OPENED;
    sock->callback = callback;
    sock->user_data = user_data;

    if (ssl_config) {
        sock->is_ssl = true;
        memcpy(&sock->ssl_config, ssl_config, sizeof(net_ssl_config_t));
        sock->ssl_handshake_done = false;
    }

    sock->recv_cap = NET_SOCKET_RECV_BUF_SIZE;
    sock->recv_buf = (char*)iot_malloc(sock->recv_cap);
    sock->send_buf = (char*)iot_malloc(NET_SOCKET_SEND_BUF_SIZE);

    net_socket_add(sock);

    return (sock_t)sock;
}

int net_socket_bind(sock_t sock, const char* ip, uint16_t port)
{
    net_socket_t* s = (net_socket_t*)sock;
    if (!s || s->state != NET_SOCK_STATE_OPENED) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (ip && strlen(ip) > 0) {
        addr.sin_addr.s_addr = inet_addr(ip);
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
    }

    int ret = lwip_bind(s->fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int net_socket_listen(sock_t sock, int backlog)
{
    net_socket_t* s = (net_socket_t*)sock;
    if (!s || s->state != NET_SOCK_STATE_OPENED || s->type != SOCK_TYPE_STREAM) {
        return -1;
    }

    int ret = lwip_listen(s->fd, backlog);
    if (ret != 0) {
        return -1;
    }

    net_socket_set_state(s, NET_SOCK_STATE_LISTENING);
    return 0;
}

int net_socket_connect(sock_t sock, const char* ip, uint16_t port)
{
    net_socket_t* s = (net_socket_t*)sock;
    if (!s || s->state != NET_SOCK_STATE_OPENED || s->type != SOCK_TYPE_STREAM) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    net_socket_set_state(s, NET_SOCK_STATE_CONNECTING);

    int ret = lwip_connect(s->fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == 0) {
        net_socket_set_state(s, NET_SOCK_STATE_CONNECTED);
        net_socket_trigger_event(s, NET_EVENT_CONNECTED);

        if (s->is_ssl) {
            if (ssl_init_client(s) != 0) {
                net_socket_set_state(s, NET_SOCK_STATE_ERROR);
                net_socket_trigger_event(s, NET_EVENT_ERROR);
                return -1;
            }
            net_socket_set_state(s, NET_SOCK_STATE_SSL_HANDSHAKE);
        }
        return 0;
    }

    if (errno == EINPROGRESS || errno == EWOULDBLOCK) {
        return 0;
    }

    net_socket_set_state(s, NET_SOCK_STATE_ERROR);
    net_socket_trigger_event(s, NET_EVENT_ERROR);
    return -1;
}

sock_t net_socket_accept(sock_t listen_sock, net_socket_callback_t callback, void* user_data)
{
    net_socket_t* ls = (net_socket_t*)listen_sock;
    if (!ls || ls->state != NET_SOCK_STATE_LISTENING) {
        return INVALID_SOCKET;
    }

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    int client_fd = lwip_accept(ls->fd, (struct sockaddr*)&addr, &addr_len);
    if (client_fd < 0) {
        return INVALID_SOCKET;
    }

    net_socket_t* client_sock = (net_socket_t*)iot_malloc(sizeof(struct net_socket));
    if (!client_sock) {
        lwip_close(client_fd);
        return INVALID_SOCKET;
    }

    memset(client_sock, 0, sizeof(struct net_socket));
    client_sock->fd = client_fd;
    client_sock->type = ls->type;
    client_sock->state = NET_SOCK_STATE_CONNECTED;
    client_sock->callback = callback;
    client_sock->user_data = user_data;

    net_socket_set_nonblocking(client_fd);

    client_sock->recv_cap = NET_SOCKET_RECV_BUF_SIZE;
    client_sock->recv_buf = (char*)iot_malloc(client_sock->recv_cap);
    client_sock->send_buf = (char*)iot_malloc(NET_SOCKET_SEND_BUF_SIZE);

    net_socket_add(client_sock);
    net_socket_trigger_event(ls, NET_EVENT_ACCEPT);

    return (sock_t)client_sock;
}

int net_socket_send(sock_t sock, const void* data, size_t len)
{
    net_socket_t* s = (net_socket_t*)sock;
    if (!s || (s->state != NET_SOCK_STATE_CONNECTED && s->state != NET_SOCK_STATE_SSL_HANDSHAKE)) {
        return -1;
    }

    if (s->is_ssl && !s->ssl_handshake_done) {
        return -1;
    }

    if (s->is_ssl) {
        return tls_write(s->tls, data, len);
    }

    return lwip_send(s->fd, data, len, 0);
}

int net_socket_recv(sock_t sock, void* buf, size_t len)
{
    net_socket_t* s = (net_socket_t*)sock;
    if (!s || (s->state != NET_SOCK_STATE_CONNECTED && s->state != NET_SOCK_STATE_SSL_HANDSHAKE)) {
        return -1;
    }

    if (s->is_ssl && !s->ssl_handshake_done) {
        return -1;
    }

    if (s->is_ssl) {
        return tls_read(s->tls, buf, len);
    }

    return lwip_recv(s->fd, buf, len, 0);
}

void net_socket_close(sock_t sock)
{
    net_socket_t* s = (net_socket_t*)sock;
    if (!s) {
        return;
    }

    net_socket_remove(s);

    if (s->is_ssl) {
        if (s->tls) {
            tls_shutdown(s->tls);
            tls_free(s->tls);
        }
        if (s->tls_ctx) {
            tls_ctx_free(s->tls_ctx);
        }
    }

    if (s->fd >= 0) {
        lwip_close(s->fd);
    }

    if (s->recv_buf) {
        iot_free(s->recv_buf);
    }
    if (s->send_buf) {
        iot_free(s->send_buf);
    }

    iot_free(s);
}

net_sock_state_t net_socket_get_state(sock_t sock)
{
    net_socket_t* s = (net_socket_t*)sock;
    return s ? s->state : NET_SOCK_STATE_CLOSED;
}

bool net_socket_is_ssl(sock_t sock)
{
    net_socket_t* s = (net_socket_t*)sock;
    return s ? s->is_ssl : false;
}

bool net_socket_ssl_handshake_done(sock_t sock)
{
    net_socket_t* s = (net_socket_t*)sock;
    return s ? s->ssl_handshake_done : false;
}

const char* net_socket_ssl_get_error(sock_t sock)
{
    net_socket_t* s = (net_socket_t*)sock;
    return s ? s->ssl_error : "";
}

void* net_socket_get_user_data(sock_t sock)
{
    net_socket_t* s = (net_socket_t*)sock;
    return s ? s->user_data : NULL;
}

void net_socket_set_user_data(sock_t sock, void* user_data)
{
    net_socket_t* s = (net_socket_t*)sock;
    if (s) {
        s->user_data = user_data;
    }
}

const char* net_socket_get_recv_buf(sock_t sock)
{
    net_socket_t* s = (net_socket_t*)sock;
    return s ? s->recv_buf : NULL;
}

size_t net_socket_get_recv_len(sock_t sock)
{
    net_socket_t* s = (net_socket_t*)sock;
    return s ? s->recv_len : 0;
}

void net_socket_clear_recv_buf(sock_t sock)
{
    net_socket_t* s = (net_socket_t*)sock;
    if (s) {
        s->recv_len = 0;
    }
}

typedef struct {
    net_dns_callback_t callback;
    void* user_data;
    char hostname[256];
} dns_resolve_ctx_t;

static void dns_resolve_callback(const char* name, const ip_addr_t* ipaddr, void* callback_arg)
{
    dns_resolve_ctx_t* ctx = (dns_resolve_ctx_t*)callback_arg;

    if (ctx->callback) {
        const char* ip = NULL;
        if (ipaddr) {
            static char ip_str[16];
            ip = ip4addr_ntoa_r(ipaddr, ip_str, sizeof(ip_str));
        }
        ctx->callback(ctx->hostname, ip, ctx->user_data);
    }

    iot_free(ctx);
}

int net_dns_resolve(const char* name, net_dns_callback_t callback, void* user_data)
{
    if (!name || !callback) {
        return -1;
    }

    dns_resolve_ctx_t* ctx = (dns_resolve_ctx_t*)iot_malloc(sizeof(dns_resolve_ctx_t));
    if (!ctx) {
        return -1;
    }

    memset(ctx, 0, sizeof(dns_resolve_ctx_t));
    strncpy(ctx->hostname, name, sizeof(ctx->hostname) - 1);
    ctx->callback = callback;
    ctx->user_data = user_data;

    ip_addr_t addr;
    err_t err = dns_gethostbyname(name, &addr, dns_resolve_callback, ctx);
    if (err == ERR_OK) {
        char ip_str[16];
        ctx->callback(name, ip4addr_ntoa_r(&addr, ip_str, sizeof(ip_str)), user_data);
        iot_free(ctx);
        return 0;
    }

    if (err == ERR_INPROGRESS) {
        return 0;
    }

    iot_free(ctx);
    return -1;
}

int net_init(void)
{
    s_socket_mutex = iot_mutex_create();
    if (!s_socket_mutex) {
        return -1;
    }

    s_net_running = true;
    s_net_task = iot_task_create("net_poll", net_poll_thread, NULL, 4096, IOT_OS_PRIO_NORMAL);
    if (!s_net_task) {
        iot_mutex_destroy(s_socket_mutex);
        s_net_running = false;
        return -1;
    }

    return 0;
}

void net_deinit(void)
{
    s_net_running = false;
    iot_task_delay(100);

    if (s_net_task) {
        iot_task_destroy(s_net_task);
    }

    if (s_socket_mutex) {
        iot_mutex_destroy(s_socket_mutex);
    }

    while (s_socket_list) {
        net_socket_close((sock_t)s_socket_list);
    }
}

static void net_poll_thread(void* arg)
{
    fd_set read_fds, write_fds, except_fds;
    int max_fd = -1;

    while (s_net_running) {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);

        iot_mutex_lock(s_socket_mutex, -1);

        net_socket_t* sock = s_socket_list;
        while (sock) {
            if (sock->fd >= 0) {
                FD_SET(sock->fd, &read_fds);

                if (sock->state == NET_SOCK_STATE_CONNECTING) {
                    FD_SET(sock->fd, &write_fds);
                }

                if (sock->state == NET_SOCK_STATE_SSL_HANDSHAKE) {
                    FD_SET(sock->fd, &read_fds);
                    FD_SET(sock->fd, &write_fds);
                }

                if (sock->fd > max_fd) {
                    max_fd = sock->fd;
                }
            }
            sock = sock->next;
        }

        iot_mutex_unlock(s_socket_mutex);

        if (max_fd < 0) {
            iot_task_delay(NET_POLL_INTERVAL_MS);
            continue;
        }

        struct timeval tv;
        tv.tv_sec = NET_POLL_INTERVAL_MS / 1000;
        tv.tv_usec = (NET_POLL_INTERVAL_MS % 1000) * 1000;

        int ret = lwip_select(max_fd + 1, &read_fds, &write_fds, &except_fds, &tv);
        if (ret <= 0) {
            continue;
        }

        iot_mutex_lock(s_socket_mutex, -1);

        sock = s_socket_list;
        while (sock) {
            net_socket_t* next = sock->next;

            if (sock->fd < 0) {
                sock = next;
                continue;
            }

            if (FD_ISSET(sock->fd, &write_fds)) {
                if (sock->state == NET_SOCK_STATE_CONNECTING) {
                    int err = 0;
                    socklen_t err_len = sizeof(err);
                    lwip_getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, (void*)&err, &err_len);

                    if (err == 0) {
                        net_socket_set_state(sock, NET_SOCK_STATE_CONNECTED);
                        net_socket_trigger_event(sock, NET_EVENT_CONNECTED);

                        if (sock->is_ssl) {
                            if (ssl_init_client(sock) != 0) {
                                net_socket_set_state(sock, NET_SOCK_STATE_ERROR);
                                net_socket_trigger_event(sock, NET_EVENT_ERROR);
                            } else {
                                net_socket_set_state(sock, NET_SOCK_STATE_SSL_HANDSHAKE);
                            }
                        }
                    } else {
                        net_socket_set_state(sock, NET_SOCK_STATE_ERROR);
                        net_socket_trigger_event(sock, NET_EVENT_ERROR);
                    }
                } else if (sock->state == NET_SOCK_STATE_SSL_HANDSHAKE) {
                    ssl_do_handshake(sock);
                }
            }

            if (FD_ISSET(sock->fd, &read_fds)) {
                if (sock->state == NET_SOCK_STATE_LISTENING) {
                    net_socket_trigger_event(sock, NET_EVENT_ACCEPT);
                } else if (sock->state == NET_SOCK_STATE_CONNECTED || 
                           sock->state == NET_SOCK_STATE_SSL_HANDSHAKE) {
                    if (sock->state == NET_SOCK_STATE_SSL_HANDSHAKE) {
                        ssl_do_handshake(sock);
                    }

                    if (sock->state == NET_SOCK_STATE_CONNECTED) {
                        char buf[NET_SOCKET_RECV_BUF_SIZE];
                        int len = net_socket_recv((sock_t)sock, buf, sizeof(buf));
                        if (len > 0) {
                            if (sock->recv_len + len > sock->recv_cap) {
                                size_t new_cap = sock->recv_cap + NET_SOCKET_RECV_BUF_SIZE;
                                char* new_buf = (char*)iot_realloc(sock->recv_buf, new_cap);
                                if (!new_buf) {
                                    sock = next;
                                    continue;
                                }
                                sock->recv_buf = new_buf;
                                sock->recv_cap = new_cap;
                            }
                            memcpy(sock->recv_buf + sock->recv_len, buf, len);
                            sock->recv_len += len;
                            net_socket_trigger_event(sock, NET_EVENT_RECV);
                        } else if (len == 0) {
                            net_socket_set_state(sock, NET_SOCK_STATE_CLOSED);
                            net_socket_trigger_event(sock, NET_EVENT_DISCONNECTED);
                        } else {
                            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                                net_socket_set_state(sock, NET_SOCK_STATE_ERROR);
                                net_socket_trigger_event(sock, NET_EVENT_ERROR);
                            }
                        }
                    }
                }
            }

            sock = next;
        }

        iot_mutex_unlock(s_socket_mutex);
    }
}

uint32_t net_inet_addr(const char* ip)
{
    return ip ? inet_addr(ip) : 0;
}

const char* net_inet_ntoa(uint32_t addr, char* buf, size_t len)
{
    ip_addr_t ip_addr;
    IP4_ADDR(&ip_addr, (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
             (addr >> 8) & 0xFF, addr & 0xFF);
    return ip4addr_ntoa_r(&ip_addr, buf, len);
}

void net_ssl_config_init(net_ssl_config_t* config)
{
    if (config) {
        memset(config, 0, sizeof(net_ssl_config_t));
        config->protocol = NET_SSL_PROTOCOL_AUTO;
        config->verify_mode = NET_SSL_VERIFY_NONE;
        config->handshake_timeout_ms = 5000;
    }
}

static void net_socket_set_state(net_socket_t* sock, net_sock_state_t state)
{
    if (sock) {
        sock->state = state;
    }
}

static void net_socket_trigger_event(net_socket_t* sock, net_event_type_t event)
{
    if (sock && sock->callback) {
        sock->callback(sock, event, sock->user_data);
    }
}

static int net_socket_set_nonblocking(int fd)
{
    int flags = lwip_fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    return lwip_fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static void net_socket_add(net_socket_t* sock)
{
    iot_mutex_lock(s_socket_mutex, -1);
    sock->next = s_socket_list;
    s_socket_list = sock;
    iot_mutex_unlock(s_socket_mutex);
}

static void net_socket_remove(net_socket_t* sock)
{
    iot_mutex_lock(s_socket_mutex, -1);
    if (s_socket_list == sock) {
        s_socket_list = sock->next;
    } else {
        net_socket_t* prev = s_socket_list;
        while (prev && prev->next != sock) {
            prev = prev->next;
        }
        if (prev) {
            prev->next = sock->next;
        }
    }
    sock->next = NULL;
    iot_mutex_unlock(s_socket_mutex);
}

static int ssl_init_client(net_socket_t* sock)
{
    int protocol = TLS_protocol_tls12;
    if (sock->ssl_config.protocol == NET_SSL_PROTOCOL_TLS13) {
        protocol = TLS_protocol_tls13;
    } else if (sock->ssl_config.protocol == NET_SSL_PROTOCOL_TLCP) {
        protocol = TLS_protocol_tlcp;
    }

    sock->tls_ctx = tls_ctx_new(TLS_client_method());
    if (!sock->tls_ctx) {
        snprintf(sock->ssl_error, sizeof(sock->ssl_error), "Failed to create TLS context");
        return -1;
    }

    tls_ctx_set_version(sock->tls_ctx, protocol);

    if (sock->ssl_config.ca_file) {
        X509_CERT* ca_cert = x509_cert_from_pem_file(sock->ssl_config.ca_file);
        if (ca_cert) {
            tls_ctx_set_ca_cert(sock->tls_ctx, ca_cert);
        }
    }

    if (sock->ssl_config.cert_file && sock->ssl_config.key_file) {
        X509_CERT* cert = x509_cert_from_pem_file(sock->ssl_config.cert_file);
        SM2_KEY* key = sm2_key_from_pem_file(sock->ssl_config.key_file, sock->ssl_config.key_password);
        if (cert && key) {
            tls_ctx_set_identity(sock->tls_ctx, cert, key);
        }
    }

    sock->tls = tls_new(sock->tls_ctx);
    if (!sock->tls) {
        snprintf(sock->ssl_error, sizeof(sock->ssl_error), "Failed to create TLS connection");
        tls_ctx_free(sock->tls_ctx);
        sock->tls_ctx = NULL;
        return -1;
    }

    tls_set_fd(sock->tls, sock->fd);

    if (sock->ssl_config.hostname) {
        tls_set_servername(sock->tls, sock->ssl_config.hostname);
    }

    return 0;
}

static int ssl_do_handshake(net_socket_t* sock)
{
    if (!sock->tls) {
        return -1;
    }

    int ret = tls_connect(sock->tls);
    if (ret == 1) {
        sock->ssl_handshake_done = true;
        net_socket_set_state(sock, NET_SOCK_STATE_CONNECTED);
        net_socket_trigger_event(sock, NET_EVENT_CONNECTED);
        return 0;
    }

    if (ret < 0) {
        snprintf(sock->ssl_error, sizeof(sock->ssl_error), "SSL handshake failed");
        net_socket_set_state(sock, NET_SOCK_STATE_ERROR);
        net_socket_trigger_event(sock, NET_EVENT_ERROR);
        return -1;
    }

    return 0;
}