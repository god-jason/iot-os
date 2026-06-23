/**
 * @file net_port.h
 * @brief Platform-specific socket and DNS headers for the net module
 */

#ifndef IOT_NET_PORT_H
#define IOT_NET_PORT_H

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#define lwip_socket        socket
#define lwip_close         closesocket
#define lwip_bind          bind
#define lwip_listen        listen
#define lwip_connect       connect
#define lwip_accept        accept
#define lwip_send          send
#define lwip_recv          recv
#define lwip_select        select
#define lwip_getsockopt    getsockopt

static inline int net_port_set_nonblocking(int fd)
{
    u_long mode = 1;
    return ioctlsocket((SOCKET)fd, FIONBIO, &mode) == 0 ? 0 : -1;
}

#else
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/inet.h"

static inline int net_port_set_nonblocking(int fd)
{
    int flags = lwip_fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    return lwip_fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
#endif

#endif /* IOT_NET_PORT_H */
