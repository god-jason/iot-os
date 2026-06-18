#ifndef __LWIP_CONFIG_SUB_MBL_H__
#define __LWIP_CONFIG_SUB_MBL_H__

/*  数传模块 LWIP NUM 定义 */

#define IP_FRAG                     1
#define LWIP_TCP_WND_SIZE (8196 * 2)

/* NUM of LwIP */

#define LWIP_MEMP_NUM_NETCONN   8 ///EC618:14   2100:8
#define LWIP_PBUF_NUM           128

#ifdef USE_TOP_PPP
#define LWIP_PBUF_POOL_NUM     16
#else
#define LWIP_PBUF_POOL_NUM      128  ///EC618:8 2100 LWIP_PBUF_NUM
#endif

#define LWIP_RAW_PCB_NUM        4  ///EC618:2 2100:4
#define LWIP_UDP_PCB_NUM        9 ///EC618:14 2100:4
#define LWIP_TCP_PCB_NUM        8 ///EC618:12 2100:4
#define LWIP_TCP_SEG_NUM        40 ///EC618:13 2100:40
#define MEMP_NUM_PBUF            256 ///EC618:12   2100:256
#define MEMP_NUM_ARP_QUEUE       30 ///EC618:32   2100:30
#define MEMP_NUM_FRAG_PBUF       15 ///EC618:25   2100:15
#define MEMP_NUM_ND6_QUEUE       20 ///EC618:15   2100:20
#define MEMP_NUM_NETBUF          128 ///EC618:24   2100:2
#define MEMP_NUM_NETDB           1  ///EC618:2    2100:1
#define MEMP_NUM_TCPIP_MSG_API   8 ///EC618:24   2100:8
#define MEMP_NUM_TCPIP_MSG_INPKT 256  ///EC618:3    2100:256
#define MEMP_NUM_TCP_PCB_LISTEN  8  ///EC618:2    2100:8
#define LWIP_ND6_NUM_NEIGHBORS   10  ///EC618:5    2100:10
#define LWIP_ND6_NUM_DESTINATIONS 10

/* end NUM of LwIP */

/* IPSec */
//#define LWIP_USING_IPSEC
/* end of IPSec */

#endif /* __LWIP_CONFIG_SUB_MBL_H__ */
