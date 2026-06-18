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
#ifndef __NETIF_PSNETIF_H__
#define __NETIF_PSNETIF_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

#define PS_NETIF_NAME_BASE_0	('C')
#define PS_NETIF_NAME_BASE_1	('0')

#define PS_NETIF_MTU_LEN	 NETIF_MTU_LEN_DEFAULT

#define SEND_RS_INTERVAL     (osTickFromMs(3000U))

#define TECT_IMS_CID	         7


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

typedef struct NETIF_PARA_S
{
    uint16_t mtu;
    uint16_t resv;

}NETIF_PARA_T;

typedef uint8_t (*IsIMSTECT)(void);

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/

struct netif * NET_PSNetIfAdd(const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw, const void *ipPara);

struct netif * NET_PSNetIfUpdate(struct netif *netif,const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw, const void *ipPara);

err_t NET_PsNetIfRemove(struct netif *inp);

void NET_PSNetIfSendRs(struct netif *inp);

err_t NET_PSNetIfRaCB(struct netif *inp,ip6_addr_t prefix);

void NET_PsNetIfActUrc(uint8_t cid);

void NET_PsNetIfDeactUrc(uint8_t cid);

void NET_PsNetIfIp6Reset(void);

void NET_PSNetIfAddIP6RouteEntry(ip6_addr_t prefix,uint16_t prefix_len,struct netif *inp);

void NET_PSNetIfRemoveIP6RouteEntry(ip6_addr_t prefix);

err_t NET_PSNetIfIp6Resume(struct netif *inp, void *ipPara);

void FCL_isIMSTectReg(IsIMSTECT callback);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NETIF_CAT1NETIF_H__ */
