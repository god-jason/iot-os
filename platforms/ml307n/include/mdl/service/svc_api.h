/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief service模块对外接口定义
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */

#ifndef __SVC_API_H__
#define __SVC_API_H__

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
#define SVC_EVENT_ID_MASK   (0x7FFF)
#define SVC_EVENT_FREE_FLAG (0x8000)

#define SVC_EVENT_ID(event)   ((event) & SVC_EVENT_ID_MASK)
#define SVC_EVENT_FREE(event) (((event) & SVC_EVENT_FREE_FLAG) >> 15)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

typedef enum{
    SVC_EVT_COMM_READY_IND,      ///<   MODEM ready  2210不使用

    /***************SMS 相关 begin******************************/
    SVC_EVT_SMS_NEWSMS_IND,      ///<   +CMT新短信上报 消息体对应结构体 SVC_SMS_SmsInd
    SVC_EVT_SMS_SEND_RESULT,     ///<   CMGS短信发送结果上报 消息体对应整型结果码 0 表示成功
    SVC_EVT_SMS_CSCA_IND,        ///<   CSCA短信中心读取结果上报 消息体对应对应结构体 SVC_SMS_CscaInd
    SVC_EVT_SMS_CSCA_SET_RESULT, ///<   CSCA短信中心设置结果上报 消息体对应整型结果码 0 表示成功
    SVC_EVT_SMS_CMGD_SET_RESULT, ///<   CMGD短信删除结果上报 消息体对应整型结果码 0 表示成功
    /***************SMS 相关 end******************************/

    /***************USIM 相关 begin******************************/
    SVC_EVT_USIM_CMUSLOT_IND,    ///<   +CMUSLOT主动上报 消息体对应结构体 SVC_USIM_Cmuslot
    SVC_EVT_USIM_ICCID_IND,      ///<   +ICCID主动上报 消息体对应结构体 SVC_USIM_IccidInfo
    SVC_EVT_USIM_CMURDY_IND,     ///<   +CMURDY主动上报 消息体对应整型结果码+CMURDY命令中的<initresult>
    /***************USIM 相关 end******************************/

    /***************network 相关 begin******************************/
    SVC_EVT_NETWORK_CEREG_IND,          ///<   +CEREG主动上报 消息体对应结构体 SVC_NETWORK_CeregInfo
    SVC_EVT_NETWORK_IMSVMODE_IND,       ///<   +IMSVMODE主动上报 IMS注册结果上报 消息体对应整型结果码 1 表示已经注册
    SVC_EVT_NETWORK_SET_CFUN_RESULT,    ///<   CFUN设置结果上报 消息体对应整型结果码 0 表示成功
    SVC_EVT_NETWORK_SET_CEREG_RESULT,   ///<   CEREG设置结果上报 消息体对应整型结果码 0 表示成功
    SVC_EVT_NETWORK_READ_CSQ_RESULT,    ///<   CSQ值上报 消息体对应整型结果码 携带+CSQ中的rssi值
    SVC_EVT_NETWORK_SET_CGACT_RESULT,   ///<   CGACT设置结果上报 消息体对应整型结果码 0 表示成功
    SVC_EVT_NETWORK_SET_CMGACT_RESULT,  ///<   CMGACT设置结果上报 2210不使用
    SVC_EVT_NETWORK_SET_NETDEVCTL_RESULT,  ///<   NETDEVCTL设置结果上报 消息体对应整型结果码 0 表示成功
    SVC_EVT_NETWORK_READ_COPS_RESULT,   ///<   COPS读取结果上报 消息体对应结构体 SVC_NETWORK_CopsReadInfo
    SVC_EVT_NETWORK_CMGACTSTAT_IND,     ///<   CMGACT结果上报     对应结构体 SVC_NETWORK_CmgactstatInfo 2210不使用
    SVC_EVT_NETWORK_NETDEVCTL_IND,      ///<   +NETDEVCTL主动上报 消息体对应结构体 SVC_NETWORK_NetdevctlInfo 该消息会重复上报
    SVC_EVT_NETWORK_NETSTAT_IND,        ///<   +NETSTAT主动上报 消息体对应结构体 SVC_NETWORK_NetstatInfo 该消息会重复上报
    SVC_EVT_NETWORK_CGEV_IND,           ///<   CGEV主动上报 消息体对应结构体 SVC_NETWORK_CgevInfo
    SVC_EVT_NETWORK_IPV4_READY_IND,     ///<   IP网络V4准备好了, 该消息会重复上报
    SVC_EVT_NETWORK_IPV6_READY_IND,     ///<   IP网络V6准备好了, 该消息会重复上报
    SVC_EVT_NETWORK_TIME_IND,           ///<   +CTZE主动上报 消息体对应结构体 SVC_NETWORK_DateTime
    /***************network 相关 end******************************/

    /***************语音呼叫 相关 begin******************************/
    SVC_EVT_CALL_SET_ATD_RESULT,        ///<   ATD设置结果上报 拨号MO 消息体对应整型结果码 0 表示成功
    SVC_EVT_CALL_SET_ATA_RESULT,        ///<   ATA设置结果上报 接听MT 消息体对应整型结果码 0 表示成功
    SVC_EVT_CALL_SET_CHUP_RESULT,       ///<   CHUP设置结果上报 挂断 消息体对应整型结果码 0 表示成功
    SVC_EVT_CALL_DSCI_IND,              ///<   +DSCI通话中间过程上报 消息体对应结构体 SVC_CALL_DsciInfo
    SVC_EVT_CALL_CEND_IND,              ///<   +CEND主动上报 通话结束 消息体对应结构体 SVC_CALL_CendInfo
    /***************语音呼叫 相关 end******************************/

    /***************USAT 相关 begin******************************/
    SVC_EVT_USAT_SET_TERMINAL_RESPONSE_RESULT,  ///<   用AT+CUSATT向UICC发送terminal response 消息体对应整型结果码 0 表示成功
    SVC_EVT_USAT_SET_ENVELOPE_COMMAND_RESULT,   ///<   用AT+CUSATE向UICC发送envelope command 消息体对应整型结果码 0 表示成功
    SVC_EVT_USAT_PROACTIVE_COMMAND_IND,         ///<   +CUSATP UICC主动式命令上报 消息体对应结构体 SVC_USAT_ProactiveCommandInfo
    SVC_EVT_USAT_END_IND,                       ///<   +CUSATEND 上报 无内容
    /***************USAT 相关 end******************************/
}SVC_EVENT_ID;

/**
 ************************************************************************************
 *@brief                         事件消息内容参数
 ************************************************************************************
*/
typedef union{
    void*        ptr_param;   ///<   事件指针参数，携带结构体信息，如果event最高bit是1表示接收方需要os_free释放内存
    int32_t      int_value;   ///<   事件附加值，整型值，不存在内存释放问题
}SVC_Event_Param;

/**
 ************************************************************************************
 *@brief                         事件消息内容
 ************************************************************************************
*/
typedef struct
{
    uint16_t          event;  ///<   最高位bit位表示param要不要os_free, 其他bit位表示事件ID
    SVC_Event_Param   param;  ///<   事件参数，根据不同事件存在指针参数和整型参数两种
} SVC_Event_Info;

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief   service层入口,初始化service需要的资源
 *
 * @param[in] none
 *
 * @return
 * @note
 ************************************************************************************
*/
void service_init(void);

/**
 ************************************************************************************
 * @brief   service层处理主动上报入口
 *
 * @param[in] cmd  主动上报命令
 * @param[in] cmdLen  主动上报命令长度
 *
 * @return
 * @note
 ************************************************************************************
*/
void svc_urc_callback(const char *cmd, uint32_t cmdLen);

/**
 ************************************************************************************
 * @brief 接收Service消息
 *
 * @param[in/out]   event  事件
 * @param[in]    timeout 超时osNoWait   或 osWaitForever
 *
 * @return
 * @note
 ************************************************************************************
*/
int svc_event_recv(SVC_Event_Info *event, uint32_t timeout);

/**
 ************************************************************************************
 * @brief 接收Service消息
 *
 * @param[in]   event  事件指针
 *
 * @return
 * @note
 ************************************************************************************
*/
void svc_event_free(SVC_Event_Info *event);

/**
 ************************************************************************************
 * @brief 16进制字符串转字节流
 *
 * @param[in]    pSrc 需要转换的16进制字符串
 * @param[in]    nSrcLength pSrc中的字符个数
 * @param[out]   pOut  转换后的字节流
 *
 * @return
 * @note
 ************************************************************************************
*/
int svc_HexStr2Bytes(const char* pSrc, int nSrcLength, uint8_t* pOut);
/**
 ************************************************************************************
 * @brief 数组转16进制字符串
 *
 * @param[in]    pSrc 需要转换的数据长度
 * @param[in]    nSrcLength pSrc中的字节个数
 * @param[out]   pOut  转换后的字符串
 *
 * @return
 * @note  输出内容函数会添加字符串结束符0, 调用者要预留空间
 ************************************************************************************
*/
int svc_Bytes2HexStr(const uint8_t *pSrc, int nSrcLength, char *pOut);


#ifdef __cplusplus
}
#endif
#endif /* __SVC_API_H__ */
