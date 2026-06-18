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
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */

#ifndef __SRV_NETWORK_H__
#define __SRV_NETWORK_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
/*cereg主动上报*/
typedef struct
{
    uint8_t stat;
    char  tac[6];   /* 字符串类型；16进制格式的2个字节，跟踪区编码 */
    char  ci[10];   /* 字符串类型；16进制格式的4个字节，小区识别码*/
    uint8_t act;    /* 当前服务小区的接入技术 */
    uint8_t subact; /* 子制式    */
} SVC_NETWORK_CeregInfo;

/*cgev主动上报*/
typedef struct
{
    uint8_t stat; //  0: 去激活      1:激活
    uint8_t cid;  //  PDP上下文的CID值
} SVC_NETWORK_CgevInfo;

/*cmgactstat主动上报*/
typedef struct
{
    uint8_t stat; //  0: 去激活      1:激活
    uint8_t cid;  //  PDP上下文的CID值
} SVC_NETWORK_CmgactstatInfo;

/*netdevctl主动上报*/
typedef struct
{
    uint8_t stat; //  0: 去激活      1:激活
    uint8_t cid;  //  PDP上下文的CID值
} SVC_NETWORK_NetdevctlInfo;

/*netstat主动上报*/
typedef struct
{
    uint8_t stat; //  0: 去激活      1:激活
    uint8_t cid;  //  PDP上下文的CID值
} SVC_NETWORK_NetstatInfo;

/*cops read结果上报*/
typedef struct
{
    uint8_t mode; // 0~4
    uint8_t format; // 0~2
    char oper[20];  //运营商名称
    //uint8_t act; // 7 E-UTRAN
    uint8_t subact; // 子制式   : 0 TDD  1 FDD
} SVC_NETWORK_CopsReadInfo;

/*ctze结果上报*/
typedef struct
{
    int sec;
    int min;
    int hour;
    int mday;
    int mon;
    int year;
    int zone;
} SVC_NETWORK_DateTime;



/************************************************************************************
 *                                 函数声明
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief    配置CFUN值 发送AT+CFUN=<fun>命令
 *
 * @param[in] cfun的配置值
 *
 * @return
 * @note 配置结果通过 SVC_EVT_NETWORK_SET_CFUN_RESULT 消息上报
 ************************************************************************************
*/
int svc_network_set_cfun(uint8_t fun);
/**
 ************************************************************************************
 * @brief    配置CEREG   发送AT+CEREG=<n>命令
 *
 * @param[in] CEREG n值
 *
 * @return
 * @note 配置结果通过 SVC_EVT_NETWORK_SET_CEREG_RESULT 消息上报
 ************************************************************************************
*/
int svc_network_set_cereg(uint8_t n);

/**
 ************************************************************************************
 * @brief    读取CSQ值 发送AT+CSQ命令
 *
 * @param[in] none
 *
 * @return
 * @note CSQ值通过 SVC_EVT_NETWORK_READ_CSQ_RESULT 消息上报
 ************************************************************************************
*/
int svc_network_read_csq(void);

/**
 ************************************************************************************
 * @brief    配置PDP激活或去激活 发送AT+CGACT=<stat>,<cid>命令
 *
 * @param[in] stat   0 去激活  1 激活
 * @param[in] cid    PDP上下文的CID值
 *
 * @return
 * @note 配置结果通过 SVC_EVT_NETWORK_SET_CGACT_RESULT 消息上报
 ************************************************************************************
*/
int svc_network_set_cgact(uint8_t stat, uint8_t cid);

/**
 ************************************************************************************
 * @brief    PDP数据传输激活或去激活 发送AT+CMGACT=<stat>,<cid>命令
 *
 * @param[in] stat   0 去激活  1 激活
 * @param[in] cid     PDP上下文的CID值
 *
 * @return
 * @note  2210不使用
    配置结果通过 SVC_EVT_NETWORK_SET_CMGACT_RESULT 消息上报
 ************************************************************************************
*/
int svc_network_set_cmgact(uint8_t stat, uint8_t cid);

/**
 ************************************************************************************
 * @brief    eth数据传输激活或去激活 发送AT+NETDEVCTL=<enable>,<cid>命令
 *
 * @param[in] enable   0 去激活  1 激活
 * @param[in] cid     PDP上下文的CID值
 *
 * @return
 * @note 配置结果通过 SVC_EVT_NETWORK_SET_NETDEVCTL_RESULT 消息上报
 ************************************************************************************
*/
int svc_network_set_netdevctl(uint8_t enable, uint8_t cid);

/**
 ************************************************************************************
 * @brief    读取COPS的值 发送AT+COPS?命令
 *
 * @param[in] none
 *
 * @return
 * @note 结果通过 SVC_EVT_NETWORK_READ_COPS_RESULT 消息上报
 ************************************************************************************
*/
int svc_network_read_cops(void);


#ifdef __cplusplus
}
#endif
#endif /* __SRV_NETWORK_H__ */
