#ifndef IOT_NET_H
#define IOT_NET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* sock_t;
#define INVALID_SOCKET  NULL

typedef enum {
    SOCK_TYPE_STREAM = 0,
    SOCK_TYPE_DGRAM  = 1,
} net_sock_type_t;

typedef enum {
    NET_EVENT_CONNECTED    = 0,
    NET_EVENT_DISCONNECTED = 1,
    NET_EVENT_ACCEPT       = 2,
    NET_EVENT_RECV         = 3,
    NET_EVENT_SEND         = 4,
    NET_EVENT_ERROR        = 5,
} net_event_type_t;

typedef enum {
    NET_SOCK_STATE_CLOSED      = 0,
    NET_SOCK_STATE_OPENED      = 1,
    NET_SOCK_STATE_LISTENING   = 2,
    NET_SOCK_STATE_CONNECTING  = 3,
    NET_SOCK_STATE_CONNECTED   = 4,
    NET_SOCK_STATE_SSL_HANDSHAKE = 5,
    NET_SOCK_STATE_ERROR       = 6,
} net_sock_state_t;

typedef struct net_socket net_socket_t;

typedef void (*net_socket_callback_t)(net_socket_t* sock, net_event_type_t event, void* user_data);

typedef enum {
    NET_SSL_VERIFY_NONE      = 0,
    NET_SSL_VERIFY_OPTIONAL  = 1,
    NET_SSL_VERIFY_REQUIRED  = 2,
} net_ssl_verify_mode_t;

typedef enum {
    NET_SSL_PROTOCOL_TLS12   = 0,
    NET_SSL_PROTOCOL_TLS13   = 1,
    NET_SSL_PROTOCOL_TLCP    = 2,
    NET_SSL_PROTOCOL_AUTO    = 3,
} net_ssl_protocol_t;

typedef struct {
    net_ssl_protocol_t protocol;
    net_ssl_verify_mode_t verify_mode;
    const char* ca_file;
    const char* cert_file;
    const char* key_file;
    const char* key_password;
    const char* hostname;
    int handshake_timeout_ms;
} net_ssl_config_t;

sock_t net_socket_create(net_sock_type_t type, const net_ssl_config_t* ssl_config,
                        net_socket_callback_t callback, void* user_data);

int net_socket_bind(sock_t sock, const char* ip, uint16_t port);

int net_socket_listen(sock_t sock, int backlog);

int net_socket_connect(sock_t sock, const char* ip, uint16_t port);

sock_t net_socket_accept(sock_t listen_sock, net_socket_callback_t callback, void* user_data);

int net_socket_send(sock_t sock, const void* data, size_t len);

int net_socket_recv(sock_t sock, void* buf, size_t len);

void net_socket_close(sock_t sock);

net_sock_state_t net_socket_get_state(sock_t sock);

bool net_socket_is_ssl(sock_t sock);

bool net_socket_ssl_handshake_done(sock_t sock);

const char* net_socket_ssl_get_error(sock_t sock);

void* net_socket_get_user_data(sock_t sock);

void net_socket_set_user_data(sock_t sock, void* user_data);

const char* net_socket_get_recv_buf(sock_t sock);

size_t net_socket_get_recv_len(sock_t sock);

void net_socket_clear_recv_buf(sock_t sock);

typedef void (*net_dns_callback_t)(const char* name, const char* ip, void* user_data);

int net_dns_resolve(const char* name, net_dns_callback_t callback, void* user_data);

int net_init(void);

void net_deinit(void);

uint32_t net_inet_addr(const char* ip);
const char* net_inet_ntoa(uint32_t addr, char* buf, size_t len);

void net_ssl_config_init(net_ssl_config_t* config);

#ifdef __cplusplus
}
#endif

#endif /* IOT_NET_H */