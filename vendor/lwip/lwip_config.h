#ifndef __LWIP_CONFIG_H__
#define __LWIP_CONFIG_H__
/* OS */
#define OS_USING_LIBC_ADAPTER
#define LWIP_NETIF_API                  0
/* Network */


/* TCP/IP */

/* LwIP */

#define NET_USING_LWIP
#define NET_USING_LWIP212
#define NET_USING_LWIP_IPV6
#define LWIP_SOCKET_OFFSET 1
//#define LWIP_USING_STATS ///Recommend no stats
//#define LWIP_USING_IGMP ///Recommend no IGMP
#define LWIP_USING_ICMP
#define LWIP_USING_DNS
//#define LWIP_USING_DHCP
// if not define LWIP_USING_DHCP, should define LWIP_IP_ACCEPT_UDP_PORT
#define LWIP_IP_ACCEPT_UDP_PORT(port) ((port) == PP_NTOHS(LWIP_IANA_PORT_DHCP_CLIENT))
#define DHCPD_USING_ROUTER
#ifdef USE_TOP_PPP
#define LWIP_USING_PPP
#define LWIP_USING_PPPOS
#define PPP_INPROC_IRQ_SAFE 1
#endif
#define PRINTPKT_SUPPORT 1
#define PPP_NOTIFY_PHASE 1
#define LWIP_ENABLE_PPP_DEBUG
//#define LWIP_ENABLE_IP_DEBUG
//#define LWIP_ENABLE_IPV6_DEBUG
//#define LWIP_ENABLE_TCPIP_DEBUG
//#define LWIP_ENABLE_NETIF_DEBUG

//#define LWIP_ENABLE_WAKEUP_TIMER_DEBUG
#define LWIP_USING_WAKEUP_TIMER   1

/* SNTP */
#define LWIP_USING_SNTP
#define SNTP_SERVER_ADDRESS "cn.ntp.org.cn"
/* end of SNTP */

/* Static IPv4 Address */
#define LWIP_STATIC_IPADDR_PPP "192.168.115.20"
#define LWIP_STATIC_IPADDR "192.168.115.30"
#define LWIP_STATIC_GWADDR "192.168.115.1"
#define LWIP_STATIC_MSKADDR "255.255.255.0"
/* end of Static IPv4 Address */
#define LWIP_USING_NAT_IP4          1
//#define LWIP_USING_ROUTE
#define LWIP_USING_FLOWCTRL         1
#define TCP_MSS                 1460 ///Recommend:1340 Reference:1460
#define LWIP_USING_UDP
#define LWIP_USING_TCP
#define LWIP_USING_RAW
#define LWIP_TCP_SND_BUF  8196 ///EC618:(6 * TCP_MSS) 2100 8196
#define TCP_SND_QUEUELEN  ((2 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS)) ///copy from EC618
#define LWIP_TCP_WND_SIZE 64240
#define LWIP_TCP_SACK_OUT           1
#define LWIP_TCP_TASK_PRIORITY (32-6)  //pri:6
#define LWIP_TCP_TASK_MBOX_SIZE (256)
#if defined(LWIP_USING_PPP) && PPP_INPROC_IRQ_SAFE
#define LWIP_TCP_TASK_STACKSIZE 3072
#else
#define LWIP_TCP_TASK_STACKSIZE 2048
#endif

#define LWIP_REASSEMBLY_FRAG
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK   1
#define SO_REUSE                   1
#define LWIP_SO_RCVTIMEO           1
#define LWIP_SO_SNDTIMEO           1
#define LWIP_SO_RCVBUF             1
#define LWIP_SO_LINGER             1
#define LWIP_NETIF_LOOPBACK        0
#define LWIP_USING_PING
#define LWIP_IPV6_MLD              1 ///Recommend no MLD6_GROUP_base   // filter pc ipv6 multicast packet
#define LWIP_IPV6_FRAG             1
//#define IP_FRAG 0 在 lwipopts.h中定义
#define LWIP_HOOK_IP4_ROUTE_SRC   ip4_get_bind_netif
#define LWIP_SOCKET_EVNET_CALLBACK  1 //  LWIP事件上报到用户注册的回调函数
#define LWIP_SOCKET_TCP_RECV_IGNORE_CALLBACK  1 // 用于对 TCP 接收数据丢包处理的回调函数宏定义
#define LWIP_NETIF_EXT_STATUS_CALLBACK 1

#define LWIP_FIONREAD_LINUXMODE         0

/* sub define for project 一些关于NUM的定义和特殊功能 */
// #if defined(USE_TOP_VOLTE) || defined(USE_TOP_VOLTE_SMS)
//     #include "lwip_config_sub_volte.h"
// #else
//     #include "lwip_config_sub_mdl.h"
// #endif

/* end of LwIP */
/* end of TCP/IP */

/* Protocols */

/* CoAP */

/* libcoap-v4.2.1 */

/* end of libcoap-v4.2.1 */
/* end of CoAP */

/* HTTP */
#define  NET_USING_HTTPCLIENT

#define  HTTPCLIENT_USING_SAMPLE
/* httpclient-v1.1.0 */

/* end of httpclient-v1.1.0 */
/* end of HTTP */

/* LWM2M */

/* LWM2M-v1.0.0 */

/* end of LWM2M-v1.0.0 */
/* end of LWM2M */

/* MQTT */

/* pahomqtt-v1.1.0 */

/* end of pahomqtt-v1.1.0 */
/* end of MQTT */
/* end of Protocols */

#define SECURITY_USING_MBEDTLS
// #define MBEDTLS_TIMING_ALT
// #define MBEDTLS_THREADING_ALT
//#define MBEDTLS_PLATFORM_GMTIME_R_ALT
#define SECURITY_USING_TLS_CLIENT_DEMO
/* Socket */

/* end of Socket */

/* IPSec */
//#define LWIP_USING_IPSEC
/* end of IPSec */

#endif /* __LWIP_CONFIG_H__ */

