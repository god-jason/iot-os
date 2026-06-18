/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */

#ifndef __IP4_NAT_H__
#define __IP4_NAT_H__

#if LWIP_USING_FLOWCTRL
#include "net_flowctrl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define NAT_WAN_NETIF_TYPE_MAX      (NET_CID_MAX + 1)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum
{
    PORT_USER_NONE = 0,
    PORT_USER_NAT,
    PORT_USER_LWIP,
    PORT_USER_AT,
}NAT_PortUser;

typedef enum
{
    PORT_TYPE_NONE = 0,
    PORT_TYPE_TCP,
    PORT_TYPE_UDP,
    PORT_TYPE_TCP_UDP,
}NAT_PortType;

typedef enum
{
    NAT_IP4_DIRECTON_IN = 0,   /* 下行方向*/
    NAT_IP4_DIRECTON_OUT,      /* 上行方向*/
}NAT_Ip4Direction; /*  */

typedef enum
{
    NAT_LAN_NETIF_TYPE_GMAC = 0,
    NAT_LAN_NETIF_TYPE_USB,
    NAT_LAN_NETIF_TYPE_PPP,
    NAT_LAN_NETIF_TYPE_WIFI,
    NAT_LAN_NETIF_TYPE_MAX,
}NAT_LanNetifType; /* LAN 口接口类型 */

typedef struct
{
    uint8_t         portType:4;  /* 第0比特位代表 TCP 是否占用, 第1比特位代表 UDP 是否占用. 值为1表示已占用, 为0表示未占用 */
    uint8_t         portUser:4;  /* 第6比特位代表是否被 NAT 占用, 第6比特位代表是否被 LWIP 占用. 值为1表示已占用, 为0表示未占用 */
}NAT_PortManage;     /*端口号管理*/

/**
*前置和后置的概念借鉴Linux中iptables的前置路由和后置路由.
*此文件中的前置和后置nat表分别指的是tcp或udp源端口在范围(10000-10096)内和范围外建立的nat表.
*srcPort和natPort都采用网络字节序.
*/
typedef struct
{
    int32_t               ttl;
    ip4_addr_t            srcIp;
    ip4_addr_t            dstIp;
} NAT_EntryCommon;

typedef struct
{
    NAT_EntryCommon       common;
    uint16_t              srcPort; /* 网络字节序 */
    uint16_t              dstPort; /* 网络字节序 */
    uint16_t              natPort; /* 网络字节序 */
#if LWIP_USING_FLOWCTRL
    fcl_westwood_t        fclPriv;  /* TCP流控结构体 */
#endif
} NAT_EntryTcpPre; /* tcp 前置表项成员 (24 字节)*/

typedef struct
{
    NAT_EntryCommon       common;
    uint16_t              srcPort; /* 网络字节序 */
    uint16_t              dstPort; /* 网络字节序 */
#if LWIP_USING_FLOWCTRL
    fcl_westwood_t        fclPriv;  /* TCP流控结构体 */
#endif
} NAT_EntryTcpPost; /* tcp 后置表项成员 (20 字节) */

typedef struct
{
    NAT_EntryCommon       common;
    uint16_t              srcPort; /* 网络字节序 */
    uint16_t              dstPort; /* 网络字节序 */
    uint16_t              natPort; /* 网络字节序 */
} NAT_EntryUdpPre; /* udp 前置表项成员 (20 字节) */

typedef struct
{
    NAT_EntryCommon       common;
    uint16_t              srcPort; /* 网络字节序 */
    uint16_t              dstPort; /* 网络字节序 */
} NAT_EntryUdpPost; /* udp 后置表项成员 (16 字节) */

typedef struct
{
    NAT_EntryCommon       common;
    //uint16_t              id;
    //uint16_t              seqno;
} NAT_EntryIcmp /* icmp 表项成员 (12 字节) */;

typedef struct
{
    struct netif *netif;
}NAT_WanInfo; /* WAN 口和 LAN 口的接口信息 */

typedef struct
{
    struct netif *netif;
    struct ip4_addr ip; /* 分配给LAN口的IP地址,   网络字节序 */
    uint8_t wanNetifType;   /* LAN 口对应的 WAN 口 */
}NAT_LanInfo; /* LAN 口的接口信息 */

typedef struct
{
    NAT_WanInfo wanInfo[NAT_WAN_NETIF_TYPE_MAX];
    NAT_LanInfo lanInfo[NAT_LAN_NETIF_TYPE_MAX];
}NAT_InterfaceInfo; /* WAN 口和 LAN 口的接口信息 (84 字节) */

/**
 ************************************************************************************
 * @brief           转换端口号获取,以下两种情况需要调用此函数.
 *                  1.当pc发送tcp或udp数据的源端口在10000-10096范围内时.
 *                  2.当本地应用层创建绑定tcp或udp连接时.
 *
 * @param[in]       portUser           端口号使用者
 * @param[in]       portType           端口号类型
 * @param[in]       srcPort            源端口号值(本地字节序)，lwip时srcPort填0
 * @param[out]      *getPort           获取的转换端口号值
 *
 * @return          ERR_OK：成功；ERR_VAL：失败
 ************************************************************************************
*/
int32_t NAT_Ip4LocalPortGet(uint8_t portUser, uint8_t portType, uint16_t srcPort, uint16_t *getPort);

/**
 ************************************************************************************
 * @brief           转换端口号释放,以下两种情况需要调用此函数.
 *                  1.当nat表有效时间为减为0时.
 *                  2.当本地应用层释放tcp或udp连接时.
 *
 * @param[in]       portUser           端口号使用者
 * @param[in]       portType           端口号类型
 * @param[in]       port               端口号值
 *
 * @return          void
 ************************************************************************************
*/
void NAT_Ip4LocalPortFree(uint8_t portUser, uint8_t portType, uint16_t port);

/**
 ************************************************************************************
 * @brief           对下行 IPv4 数据进行 NAT 处理，处理方法：
 *                  1.当数据类型为 TCP 时，如果目的端口号在10000-10096范围内，首先查找 TCP 后置
 *                  NAT 链表，如果查到链表，则将ttl值置为初始最大值、目的端口号改为链表表项中的转换
 *                  端口号、调整 TCP 头部校验和；如果没有查到链表，则结束、返回失败.
 *                  2.当数据类型为 TCP 时，如果目的端口号在10000-10096范围外，首先查找 TCP 后置
 *                  NAT 链表，如果查到链表，则将ttl值置为初始最大值；如果没有查到链表，则建立 TCP 后
 *                  置NAT 链表，将 TTL 值置为初始最大值.
 *                  3.如果是下行建 TCP 后置链表, NAT 表项的源 IP 地址默认 netif 类型对应的 IP 地址.
 *                  4.当数据类型为 UDP 时，处理方法与 TCP 类似.
 *                  5.当数据类型为 ICMP 时，如果是 ICMP 请求，首先查找 ICMP NAT链表，如果查到链表，
 *                  则说明上一次的 ICMP 请求还没得到应答，此次请求失败；如果没有查到链表，则将 TTL 值
 *                  置为初始最大值，建立 ICMP NAT 链表.
 *                  6.当数据类型为 ICMP 时，如果是 ICMP 应答，首先查找 ICMP NAT链表，如果查到链表，
 *                  则应答成功，将 TTL 置为0；如果没有查到链表，则应答失败.
 *                  7.如果 NAT 处理成功，当数据类型为TCP 或者 UDP 时，将目的 IP 地址修改为 UE 侧分配
 *                  给 WAN 口的 IP 地址，同时修改 TCP 或者 UDP 的头部校验和、修改 IP 头部校验和.
 *                  8.LAN 口 netif 的获取方法: 将上行建立的 NAT 表中 LAN 口对应的 IP 地址与设置 netif
 *                  时设置的 LAN 口 IP 地址进行匹配, 找到 LAN 口的 netif 类型, 即可找到对应的 LAN 口netif.
 *                  9.LAN 口对应的 IP 地址就是上行建立的 NAT 表中 LAN 口对应的 IP 地址.
 *
 * @param[in]       p            下行数据指针
 *
 * @return          1：nat处理成功.  0：nat处理失败.
 ************************************************************************************
*/
uint8_t NAT_Ip4In(struct pbuf *p);

/**
 ************************************************************************************
 * @brief           对上行 IPv4 数据进行 NAT 处理，处理方法：
 *                  1.当数据类型为 TCP 时，如果源端口号在10000-10096范围内，首先查找 TCP 前置 NAT
 *                  链表，如果查到链表，则将 TTL 值置为初始最大值、源端口号改为链表表项中的转换端口号、
 *                  调整 TCP 头部校验和；如果没有查到链表，则申请转换端口号、建立 TCP 前置 NAT 链表，
 *                  将 TTL 值置为初始最大值、源端口号改为链表表项中的转换端口号、调整 TCP 头部校验和.
 *                  2.当数据类型为 TCP 时，如果源端口号在10000-10096范围外，首先查找 TCP 后置 NAT
 *                  链表，如果查到链表，则将 TTL 值置为初始最大值；如果没有查到链表，则建立 TCP 前置
 *                  NAT 链表，将 TCP 值置为初始最大值.
 *                  3.当数据类型为 UDP 时，处理方法与 TCP 类似.
 *                  4.当数据类型为 ICMP 时，如果是 ICMP 请求，首先查找 ICMP NAT 链表，如果查到链表，
 *                  则说明上一次的 ICMP 请求还没得到应答，此次请求失败；如果没有查到链表，则将 TTL 值
 *                  置为初始最大值，建立 ICMP NAT 链表.
 *                  5.当数据类型为 ICMP 时，如果是 ICMP 应答，首先查找ICMP NAT 链表，如果查到链表，
 *                  则应答成功，将 TTL 置为0；如果没有查到链表，则应答失败.
 *                  6.如果 NAT 处理成功，当数据类型为 TCP 或者 UDP 时，将源 IP 地址修改为服务器分配给
 *                  WAN 口的 IP 地址，同时修改 TCP 或者 UDP 的头部校验和、修改 IP 头部校验和.
 *                  7.WAN 口 netif 的获取方法: 将上行 IP 报文中的源 IP 地址与设置 netif 时设置的 LAN 口
 *                  IP 地址进行匹配, 找到 WAN 口的 netif 类型, 即可找到对应的 netif.
 *                  8.WAN 口对应的 IP 地址就是添加WAN 口 netif 时设置的 IP 地址.
 *
 * @param[in]       p            上行数据指针
 *
 * @return          1：nat处理成功.  0：nat处理失败.
 ************************************************************************************
*/
uint8_t NAT_Ip4Out(struct pbuf *p);

/**
 ************************************************************************************
 * @brief           netif 接口设置
 *
 * @param[in]       wanIf            WAN口netif指针
 * @param[in]       lanIf            LAN口netif指针
 * @param[in]       lanIfType        LAN口netif类型
 * @param[in]       lanIp            LAN口对应分配的IP地址
 *
 * @return          void
 ************************************************************************************
*/
void NAT_Ip4EntryAdd(struct netif *wanIf, struct netif *lanIf, uint8_t lanIfType, ip4_addr_t lanIp);

/**
 ************************************************************************************
 * @brief           netif 接口清除
 *
 * @param[in]       wanIf            WAN口netif指针
 * @param[in]       lanIf            LAN口netif指针
 *
 * @return          void
 ************************************************************************************
*/
void NAT_Ip4EntryRemove(struct netif *wanIf, struct netif *lanIf);

/**
 ************************************************************************************
 * @brief           ipv4 nat启动
 *
 * @param[in]       void
 *
 * @return          void
 ************************************************************************************
*/
int32_t NAT_Ip4Start(void);

/**
 ************************************************************************************
 * @brief           ipv4 nat停止
 *
 * @param[in]       void
 *
 * @return          void
 ************************************************************************************
*/
void NAT_Ip4Stop(void);

/**
 ************************************************************************************
 * @brief           ipv4 nat初始化，进行端口号内存申请、创建互斥锁
 *
 * @param[in]       void
 *
 * @return          ERR_OK：初始化成功.  其它：初始化失败.
 ************************************************************************************
*/
int32_t NAT_Ip4Init(void);

#ifdef __cplusplus
}
#endif
#endif

