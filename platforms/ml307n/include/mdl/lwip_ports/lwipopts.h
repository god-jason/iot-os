#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include <lwip_config.h>
#define ERRNO                       1

#define LWIP_SOCKET_SELECT 1
#define LWIP_SOCKET_POLL 1

#define LWIP_IPV4                   1

#ifdef NET_USING_LWIP_IPV6
#define LWIP_IPV6                   1
#else
#define LWIP_IPV6                   0
#endif /* NET_USING_LWIP_IPV6 */

#define NO_SYS                      0
#define LWIP_SOCKET                 1
#define LWIP_NETCONN                1

#ifdef LWIP_USING_IGMP
#define LWIP_IGMP                   1
#else
#define LWIP_IGMP                   0
#endif

#ifdef LWIP_USING_ICMP
#define LWIP_ICMP                   1
#else
#define LWIP_ICMP                   0
#endif

#ifdef LWIP_USING_SNMP
#define LWIP_SNMP                   1
#else
#define LWIP_SNMP                   0
#endif

#ifdef LWIP_USING_DNS
#define LWIP_DNS                    1
#else
#define LWIP_DNS                    0
#endif

#ifdef LWIP_USING_SNTP
#define LWIP_SNTP                   1
#else
#define LWIP_SNTP                   0
#endif

#ifndef SNTP_SERVER_DNS
#define SNTP_SERVER_DNS             1
#endif

#define LWIP_HAVE_LOOPIF            1

#define LWIP_PLATFORM_BYTESWAP      0

#ifndef BYTE_ORDER
#define BYTE_ORDER                  LITTLE_ENDIAN
#endif

#ifdef LWIP_USING_IPSEC
#define LWIP_IPSEC                  1
#else
#define LWIP_IPSEC                  0
#endif

#if LWIP_IPSEC
/* #define LWIP_PBUF_CUSTOM_DATA (u8_t cb[8];) */
#define LWIP_IPSEC_USE_MBEDTLS_CIPHER_WRAPPER
#define IPSEC_OUTBOUND_SOCKET_SAD_SUPPORT       0
#endif  /* LWIP_IPSEC */

#define LWIP_ENABLE_DEBUG

/*
#define LWIP_ENABLE_API_LIB_DEBUG
#define LWIP_ENABLE_API_MSG_DEBUG
#define LWIP_ENABLE_TCPIP_DEBUG
#define LWIP_ENABLE_SOCKETS_DEBUG
#define LWIP_ENABLE_TCP_DEBUG
#define LWIP_ENABLE_TCP_INPUT_DEBUG
#define LWIP_ENABLE_TCP_OUTPUT_DEBUG
*/

//#define PPP_DEBUG                   LWIP_DBG_ON
#ifdef LWIP_ENABLE_DEBUG
#define LWIP_DEBUG
#endif

/* ---------- Debug options ---------- */
#ifdef LWIP_DEBUG
#ifdef LWIP_ENABLE_SYS_DEBUG
#define SYS_DEBUG                   LWIP_DBG_ON
#else
#define SYS_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_ETHARP_DEBUG
#define ETHARP_DEBUG                LWIP_DBG_ON
#else
#define ETHARP_DEBUG                LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_PPP_DEBUG
#define PPP_DEBUG                   LWIP_DBG_ON
#else
#define PPP_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_NAT_DEBUG
#define NAT_DEBUG                   LWIP_DBG_ON
#else
#define NAT_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_MEM_DEBUG
#define MEM_DEBUG                   LWIP_DBG_ON
#else
#define MEM_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_MEMP_DEBUG
#define MEMP_DEBUG                  LWIP_DBG_ON
#else
#define MEMP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_PBUF_DEBUG
#define PBUF_DEBUG                  LWIP_DBG_ON
#else
#define PBUF_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_API_LIB_DEBUG
#define API_LIB_DEBUG               LWIP_DBG_ON
#else
#define API_LIB_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_API_MSG_DEBUG
#define API_MSG_DEBUG               LWIP_DBG_ON
#else
#define API_MSG_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCPIP_DEBUG
#define TCPIP_DEBUG                 LWIP_DBG_ON
#else
#define TCPIP_DEBUG                 LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_NETIF_DEBUG
#define NETIF_DEBUG                 LWIP_DBG_ON
#else
#define NETIF_DEBUG                 LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_SOCKETS_DEBUG
#define SOCKETS_DEBUG               LWIP_DBG_ON
#else
#define SOCKETS_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_DNS_DEBUG
#define DNS_DEBUG                   LWIP_DBG_ON
#else
#define DNS_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_AUTOIP_DEBUG
#define AUTOIP_DEBUG                LWIP_DBG_ON
#else
#define AUTOIP_DEBUG                LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_DHCP_DEBUG
#define DHCP_DEBUG                  LWIP_DBG_ON
#else
#define DHCP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_IP_DEBUG
#define IP_DEBUG                    LWIP_DBG_ON
#else
#define IP_DEBUG                    LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_IPV6_DEBUG
#define IP6_DEBUG                   LWIP_DBG_ON
#else
#define IP6_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_IP_REASS_DEBUG
#define IP_REASS_DEBUG              LWIP_DBG_ON
#else
#define IP_REASS_DEBUG              LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_ICMP_DEBUG
#define ICMP_DEBUG                  LWIP_DBG_ON
#else
#define ICMP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_IGMP_DEBUG
#define IGMP_DEBUG                  LWIP_DBG_ON
#else
#define IGMP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_UDP_DEBUG
#define UDP_DEBUG                   LWIP_DBG_ON
#else
#define UDP_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCP_DEBUG
#define TCP_DEBUG                   LWIP_DBG_ON
#else
#define TCP_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCP_INPUT_DEBUG
#define TCP_INPUT_DEBUG             LWIP_DBG_ON
#else
#define TCP_INPUT_DEBUG             LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCP_OUTPUT_DEBUG
#define TCP_OUTPUT_DEBUG            LWIP_DBG_ON
#else
#define TCP_OUTPUT_DEBUG            LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCP_RTO_DEBUG
#define TCP_RTO_DEBUG               LWIP_DBG_ON
#else
#define TCP_RTO_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCP_CWND_DEBUG
#define TCP_CWND_DEBUG              LWIP_DBG_ON
#else
#define TCP_CWND_DEBUG              LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCP_WND_DEBUG
#define TCP_WND_DEBUG               LWIP_DBG_ON
#else
#define TCP_WND_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCP_FR_DEBUG
#define TCP_FR_DEBUG                LWIP_DBG_ON
#else
#define TCP_FR_DEBUG                LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCP_QLEN_DEBUG
#define TCP_QLEN_DEBUG              LWIP_DBG_ON
#else
#define TCP_QLEN_DEBUG              LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_TCP_RST_DEBUG
#define TCP_RST_DEBUG               LWIP_DBG_ON
#else
#define TCP_RST_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_SNTP_DEBUG
#define SNTP_DEBUG                  LWIP_DBG_ON
#else
#define SNTP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_ESP_DEBUG
#define ESP_DEBUG                  LWIP_DBG_ON
#else
#define ESP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_ENABLE_WAKEUP_TIMER_DEBUG
#define WAKEUP_TIMER_DEBUG         LWIP_DBG_ON
#else
#define WAKEUP_TIMER_DEBUG         LWIP_DBG_OFF
#endif

#endif /* LWIP_DEBUG */

//#define LWIP_DBG_TYPES_ON           (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)
#define LWIP_DBG_TYPES_ON           (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH)

/* ---------- Memory options ---------- */
#define MEM_ALIGNMENT                 32//4 //for cache
#define MEMP_OVERFLOW_CHECK           0 ////
#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 1 ////
//#define MEM_LIBC_MALLOC             1
//#define MEM_USE_POOLS               1
//#define MEMP_USE_CUSTOM_POOLS       1
//#define MEM_SIZE                    (1024*64)

#define MEMP_MEM_MALLOC             0

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#if !defined(MEMP_NUM_PBUF)
#define MEMP_NUM_PBUF               256//32 //16
#endif

/* the number of struct netconns */
#ifdef LWIP_MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN            LWIP_MEMP_NUM_NETCONN
#endif

/* the number of UDP protocol control blocks. One per active RAW "connection". */
#ifdef LWIP_RAW_PCB_NUM
#define MEMP_NUM_RAW_PCB            LWIP_RAW_PCB_NUM
#endif

/* the number of UDP protocol control blocks. One per active UDP "connection". */
#ifdef LWIP_UDP_PCB_NUM
#define MEMP_NUM_UDP_PCB            LWIP_UDP_PCB_NUM
#endif

/* the number of simulatenously active TCP connections. */
#ifdef LWIP_TCP_PCB_NUM
#define MEMP_NUM_TCP_PCB            LWIP_TCP_PCB_NUM
#endif

/* the number of simultaneously queued TCP */
#ifdef LWIP_TCP_SEG_NUM
#define MEMP_NUM_TCP_SEG            LWIP_TCP_SEG_NUM
#else
#define MEMP_NUM_TCP_SEG            TCP_SND_QUEUELEN
#endif

/*
 * You can re-define following setting in rtcofnig.h to overwrite the default
 * setting in the lwip opts.h
 */
/* MEMP_NUM_NETBUF: the number of struct netbufs. */
//#define MEMP_NUM_NETBUF             128
/* MEMP_NUM_NETCONN: the number of struct netconns. */
// #define MEMP_NUM_NETCONN            4

/* MEMP_NUM_TCPIP_MSG_*: the number of struct tcpip_msg, which is used
   for sequential API communication and incoming packets. Used in
   src/api/tcpip.c. */
// #define MEMP_NUM_TCPIP_MSG_API      16
// #define MEMP_NUM_TCPIP_MSG_INPKT    16

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#if defined(LWIP_PBUF_POOL_NUM)
#define PBUF_POOL_SIZE               LWIP_PBUF_POOL_NUM
#elif defined(LWIP_PBUF_NUM)
#define PBUF_POOL_SIZE               LWIP_PBUF_NUM
#endif

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#ifdef OS_LWIP_PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE            OS_LWIP_PBUF_POOL_BUFSIZE
#endif

/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. */
#define PBUF_LINK_HLEN             0//14
/**
 * PBUF_LINK_ENCAPSULATION_HLEN: the number of bytes that should be allocated
 * for an additional encapsulation header before ethernet headers (e.g. 802.11)
 */
#define PBUF_LINK_ENCAPSULATION_HLEN    0//44 //rndis header size

#ifdef OS_LWIP_ETH_PAD_SIZE
#define ETH_PAD_SIZE                OS_LWIP_ETH_PAD_SIZE
#endif

/** SYS_LIGHTWEIGHT_PROT
 * define SYS_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task protection
 * for certain critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT        (NO_SYS==0)

#ifdef LWIP_USING_NAT
#define IP_NAT                      1
#else
#define IP_NAT                      0
#endif

/* ---------- TCP options ---------- */
#ifdef LWIP_USING_TCP
#define LWIP_TCP                    1
#else
#define LWIP_TCP                    0
#endif

#define TCP_TTL                     255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ             1

/* TCP Maximum segment size. */
#if !defined TCP_MSS || defined __DOXYGEN__
#define TCP_MSS                     1460
#endif

/* TCP sender buffer space (bytes). */
#ifdef LWIP_TCP_SND_BUF
#define TCP_SND_BUF                 LWIP_TCP_SND_BUF
#else
#define TCP_SND_BUF                 (TCP_MSS * 2)
#endif

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#if !defined TCP_SND_QUEUELEN || defined __DOXYGEN__
#define TCP_SND_QUEUELEN                ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
#endif
/* TCP writable space (bytes). This must be less than or equal
   to TCP_SND_BUF. It is the amount of space which must be
   available in the tcp snd_buf for select to return writable */
#define TCP_SNDLOWAT                (TCP_SND_BUF/2)
#define TCP_SNDQUEUELOWAT           TCP_SND_QUEUELEN/2

#define LWIP_WND_SCALE                  1
#define TCP_RCV_SCALE                   0
/* TCP receive window. */
#ifdef LWIP_TCP_WND_SIZE
#define TCP_WND                     LWIP_TCP_WND_SIZE
#else
#define TCP_WND                     (TCP_MSS * 2)
#endif

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX                  12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX               4

/* tcpip thread options */
#ifdef LWIP_TCP_TASK_PRIORITY
#define TCPIP_MBOX_SIZE             LWIP_TCP_TASK_MBOX_SIZE
#define TCPIP_THREAD_PRIO           LWIP_TCP_TASK_PRIORITY
#define TCPIP_THREAD_STACKSIZE      LWIP_TCP_TASK_STACKSIZE
#else
#define TCPIP_MBOX_SIZE             8
#define TCPIP_THREAD_PRIO           128
#define TCPIP_THREAD_STACKSIZE      4096
#endif
#define TCPIP_THREAD_NAME           "tcpip"
#define DEFAULT_TCP_RECVMBOX_SIZE   128

/* ---------- ARP options ---------- */
#define LWIP_ARP                    1
#define ARP_TABLE_SIZE              2
#define ARP_QUEUEING                1

/* ---------- Checksum options ---------- */
#ifdef LWIP_USING_HW_CHECKSUM
#define CHECKSUM_GEN_IP                 0
#define CHECKSUM_GEN_UDP                0
#define CHECKSUM_GEN_TCP                0
#define CHECKSUM_GEN_ICMP               0
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0
#define CHECKSUM_CHECK_TCP              0
#define CHECKSUM_CHECK_ICMP             0
#endif

/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#define IP_FORWARD                  0

/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#ifdef LWIP_REASSEMBLY_FRAG
#define IP_REASSEMBLY               1
//#define IP_FRAG                     0
#define IP_REASS_MAX_PBUFS          50 ///Recommend:50 Reference:10
#define MEMP_NUM_REASSDATA          25 ///Recommend:25 Reference:10
#else
#define IP_REASSEMBLY               0
//#define IP_FRAG                     0
#endif

/* ---------- ICMP options ---------- */
#define ICMP_TTL                    255

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#ifdef LWIP_USING_DHCP
#define LWIP_DHCP                   1
#else
#define LWIP_DHCP                   0
#endif

/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#define DHCP_DOES_ARP_CHECK         (LWIP_DHCP)

/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP                 0
#define LWIP_DHCP_AUTOIP_COOP       (LWIP_DHCP && LWIP_AUTOIP)

/* ---------- UDP options ---------- */
#ifdef LWIP_USING_UDP
#define LWIP_UDP                    1
#else
#define LWIP_UDP                    0
#endif

#define LWIP_UDPLITE                0
#define UDP_TTL                     255
#define DEFAULT_UDP_RECVMBOX_SIZE   128

/* ---------- RAW options ---------- */
#ifdef LWIP_USING_RAW
#define LWIP_RAW                    1
#else
#define LWIP_RAW                    0
#endif

#define DEFAULT_RAW_RECVMBOX_SIZE   10
#define DEFAULT_ACCEPTMBOX_SIZE     10

/* ---------- Statistics options ---------- */
#ifdef LWIP_USING_STATS
#define LWIP_STATS                  1
#define LWIP_STATS_DISPLAY          1
#else
#define LWIP_STATS                  0
#endif

#if LWIP_STATS
#define LINK_STATS                  1
#define IP_STATS                    1
#define ICMP_STATS                  1
#define IGMP_STATS                  1
#define IPFRAG_STATS                1
#define UDP_STATS                   1
#define TCP_STATS                   1
#define MEM_STATS                   1
#define MEMP_STATS                  1
#define PBUF_STATS                  1
#define SYS_STATS                   1
#define MIB2_STATS                  1
#endif /* LWIP_STATS */

/* ---------- PPP options ---------- */
#ifdef LWIP_USING_PPP
#define PPP_SUPPORT                 1      /* Set > 0 for PPP */
#else
#define PPP_SUPPORT                 0      /* Set > 0 for PPP */
#endif
#define PPP_SERVER PPP_SUPPORT

#if PPP_SUPPORT
#define NUM_PPP                     1      /* Max PPP sessions. */

/* Select modules to enable.  Ideally these would be set in the makefile but
 * we're limited by the command line length so you need to modify the settings
 * in this file.
 */
#ifdef LWIP_USING_PPPOE
#define PPPOE_SUPPORT               1
#else
#define PPPOE_SUPPORT               0
#endif

#ifdef LWIP_USING_PPPOS
#define PPPOS_SUPPORT               1
#else
#define PPPOS_SUPPORT               0
#endif

#define PAP_SUPPORT                 0      /* Set > 0 for PAP. */
#define CHAP_SUPPORT                0      /* Set > 0 for CHAP. */
#define MSCHAP_SUPPORT              0      /* Set > 0 for MSCHAP (NOT FUNCTIONAL!) */
#define CBCP_SUPPORT                0      /* Set > 0 for CBCP (NOT FUNCTIONAL!) */
#define CCP_SUPPORT                 0      /* Set > 0 for CCP (NOT FUNCTIONAL!) */
#define VJ_SUPPORT                  0      /* Set > 0 for VJ header compression. */
#define MD5_SUPPORT                 0      /* Set > 0 for MD5 (see also CHAP) */

#endif /* PPP_SUPPORT */

/**
 * LWIP_POSIX_SOCKETS_IO_NAMES==1: Enable POSIX-style sockets functions names.
 * Disable this option if you use a POSIX operating system that uses the same
 * names (read, write & close). (only used if you use sockets.c)
 */
#ifndef LWIP_POSIX_SOCKETS_IO_NAMES
#ifndef OS_USING_LIBC_ADAPTER
#define LWIP_POSIX_SOCKETS_IO_NAMES     1
#else
#define LWIP_POSIX_SOCKETS_IO_NAMES     0
#endif
#endif

/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT
 * options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set
 * in seconds. (does not require sockets.c, and will affect tcp.c)
 */
#ifndef LWIP_TCP_KEEPALIVE
#define LWIP_TCP_KEEPALIVE              1
#endif

/**
 * LWIP_NETIF_API==1: Support netif api (in netifapi.c)
 */
#ifndef LWIP_NETIF_API
#define LWIP_NETIF_API                  1
#endif

#ifdef LWIP_IGMP
#include <stdlib.h>
#define LWIP_RAND                  rand
#endif
/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/*
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#ifndef LWIP_SOCKET
#define LWIP_SOCKET                     1
#endif

/*
 * LWIP_COMPAT_SOCKETS==1: Enable BSD-style sockets functions names.
 * (only used if you use sockets.c)
 */
#ifdef NET_USING_BSD
#define LWIP_COMPAT_SOCKETS             0
#else
#ifndef LWIP_COMPAT_SOCKETS
#define LWIP_COMPAT_SOCKETS             1
#endif
#endif

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#ifndef LWIP_SO_SNDTIMEO
#define LWIP_SO_SNDTIMEO                1
#endif

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#ifndef LWIP_SO_RCVTIMEO
#define LWIP_SO_RCVTIMEO                1
#endif

/**
 * LWIP_SO_RCVBUF==1: Enable SO_RCVBUF processing.
 */
#ifndef LWIP_SO_RCVBUF
#define LWIP_SO_RCVBUF                  1
#endif

/**
 * If LWIP_SO_RCVBUF is used, this is the default value for recv_bufsize.
 */
#ifndef RECV_BUFSIZE_DEFAULT
#define RECV_BUFSIZE_DEFAULT            8192
#endif

/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 */
#ifndef SO_REUSE
#define SO_REUSE                        0
#endif

/*
   ------------------------------------
   ------- Applications options -------
   ------------------------------------
*/

/**
 * Max. length of TFTP filename
 */
#ifdef OS_LWIP_TFTP_MAX_FILENAME_LEN
#define TFTP_MAX_FILENAME_LEN           OS_LWIP_TFTP_MAX_FILENAME_LEN
#elif defined(OS_VFS_ELM_MAX_LFN)
#define TFTP_MAX_FILENAME_LEN           OS_VFS_ELM_MAX_LFN
#else
#define TFTP_MAX_FILENAME_LEN           64
#endif


#endif /* __LWIPOPTS_H__ */
