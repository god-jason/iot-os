#ifndef __LWIP_CONFIG_SUB_VOLTE_H__
#define __LWIP_CONFIG_SUB_VOLTE_H__

/*  带VOLTE功能时 LWIP NUM 定义 */

#define IP_FRAG                     1
#define LWIP_TCP_WND_SIZE (8196 * 2)

/* NUM of LwIP */

#define LWIP_MEMP_NUM_NETCONN   32
#define LWIP_PBUF_NUM           32
#define LWIP_PBUF_POOL_NUM      128
#define LWIP_RAW_PCB_NUM        4
#define LWIP_UDP_PCB_NUM        32
#define LWIP_TCP_PCB_NUM        32
#define LWIP_TCP_SEG_NUM        40
#define MEMP_NUM_PBUF            12
#define MEMP_NUM_ARP_QUEUE       2
#define MEMP_NUM_FRAG_PBUF       25
#define MEMP_NUM_ND6_QUEUE       15
#define MEMP_NUM_NETBUF          24
#define MEMP_NUM_NETDB           2
#define MEMP_NUM_TCPIP_MSG_API   128
#define MEMP_NUM_TCPIP_MSG_INPKT 64
#define MEMP_NUM_TCP_PCB_LISTEN  16
#define LWIP_ND6_NUM_NEIGHBORS   5
#define LWIP_ND6_NUM_DESTINATIONS 5

/* end NUM of LwIP */

/* IPSec */
#define LWIP_USING_IPSEC
/* end of IPSec */

#endif /* __LWIP_CONFIG_SUB_VOLTE_H__ */
