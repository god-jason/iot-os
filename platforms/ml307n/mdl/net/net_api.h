/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief 定义net对外API
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */


#ifndef __NET_API_H__
#define __NET_API_H__
#include <os.h>


#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 *                                类型定义
 ************************************************************************************/
/**
 * @brief           NETDEVCTL 状态回调函数指针类型定义
 *
 * @param[in]       cid     pdp id
 * @param[out]      status  上报的状态. 状态值见 netdevctl_urc_state 枚举
 * @return          void
 * @attention .
 */
typedef void (* netDevCtlStatusCb)(uint8_t cid, uint8_t status);

/**
 ************************************************************************************
 *@brief                           netdevctl 命令主动上报状态
 ************************************************************************************
*/
typedef enum
{
    NETDEVCTL_URC_STATE_DEACTIVE = 0,     ///<   netdevctl去活成功
    NETDEVCTL_URC_STATE_ACT,              ///<   netdevctl激活成功
    NETDEVCTL_URC_STATE_REPEAT,           ///<   netdevctl重复激活，不执行
    NETDEVCTL_URC_STATE_LINK_DIS,         ///<   spi网卡断开，netdevctl激活失败
    NETDEVCTL_URC_STATE_PDP_DIS,          ///<   modem未激活，netdevctl执行失败
    NETDEVCTL_URC_STATE_OTHER,            ///<   其他错误，netdevctl执行失败
} netdevctl_urc_state;

/**
 * @addtogroup NetApi
 */

/**@{*/

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/

/**
 * lwip standby init
 *
 * @param[in]
 * @return
 * @attention
 */
void lwip_sby_init(void);


/**
 * recv data from ps
 *
 * @param[in]  cid          : context id
 * @param[in]  payload      : pbuf payload addr
 * @param[in]  payloadLen   : pbuf payload length
 * @return     =0    :succeed
               <0    :failed
 * @attention
 */
int8_t NetPs_DLDataRecv(uint8_t cid, void *payload, uint16_t payloadLen);


/**
 * recv msg from ps if the ip packet been recved in a drb
 *
 * @return     =0    :succeed
               <0    :failed
 * @attention .
 */
int8_t NetPs_DLMsgNotify(void);

/**
 ************************************************************************************
 * @brief           判断是否 ping 报文, 获取 ping 报文类型
 *
 * @param[in]       p      IP(IPv4 或 IPv6) 数据指针
 * @param[in]       len    IP(IPv4 或 IPv6) 数据长度
 *
 * @return          0: 不是 ping 报文; 其它: 见枚举类型 FCL_IcmpType
 ************************************************************************************
*/
uint8_t NetPs_IcmpTypeGet(void *p, uint16_t len);

/**
 * send data to lwip by drv
 *
 * @param[in]  payload      : pbuf payload addr
 * @param[in]  payloadLen   : pbuf payload length
 * @return     =0    :succeed
               <0    :failed
 * @attention .
 */
int8_t  NetDrv_UlDataSend(void *payload, uint16_t payloadLen);

/**
 * send msg by drv if the ip packet been recved in one transc
 *
 * @return     =0    :succeed
               <0    :failed
 * @attention .
 */
int8_t  NetDrv_UlMsgNotify(void);


/**
 * alloc sys mem
 *
 * @param[in]  payloadLen  : payload len
 * @return     not NULL    : payload buffer
               NULL        : alloc failed
 * @attention.
 */
void* NET_PbufAllocDL(uint16_t payloadLen);

/**
 * alloc pbuf
 *
 * @param[in]  payloadLen  : payload len
 * @return     not NULL    : payload buffer
               NULL        : alloc failed
 * @attention.
 */
void* NET_PbufAlloc(uint16_t payloadLen);


/**
 * free pbuf
 *
 * @param[in]  payload   : payload addr
 * @attention .
 */
void NET_PbufFree(void *payload);


/**
 * at urc callback for net
 *
 * @param[in]  pdata          : urc content
 * @param[in]  lenOfData      : urc content length
 * @return     =0    :succeed
               <0    :failed
 * @attention.
 */
int8_t NET_MgrCallBack(char *pdata, uint32_t lenOfData);


/**
 * cmd[NETDEVCTL] set function
 *
 * @param[in]  pdpState       : 0:down the ethernet; 1: up the ethernet
 * @param[in]  pdpCid         : default cid
 * @return     =0    :succeed
               <0    :failed
 * @attention .
 */
int8_t NET_MgrPdpNetDevCtl(uint8_t pdpState,uint8_t pdpCid);

/**
 * eth link up callback by spi_netif
 *
 * @param[in]  connState       : 0: spi-mux disconnct; not 0: spi-mux connct
 * @return     =0    :succeed
               <0    :failed
 * @attention .
 */
int8_t NET_EthLinkConnCB(uint8_t connState);
int8_t NET_EthNetDevCtlStateGet(uint8_t cid, uint8_t *state);

/**
 * @brief           NETDEVCTL register status callback
 *
 * @param[in]       callback     回调指针
 * @return          void
 * @attention .
 */
void NET_MgrNetDevCtlStatusCbRegister(netDevCtlStatusCb callback);

#ifdef __cplusplus
}
#endif
#endif /* __NET_API_H__ */

/** @} */
