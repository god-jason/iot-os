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
#ifndef __APP_AT_PDP_H__
#define __APP_AT_PDP_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define APP_AT_PDP_CONTEXT_MIN (1)
#define APP_AT_PDP_CONTEXT_MAX (3)
#define APP_AT_PDP_CONTEXT_ID_CHECK(ContextID) (ContextID >= APP_AT_PDP_CONTEXT_MIN  && ContextID <= APP_AT_PDP_CONTEXT_MAX)
#define APP_AT_PDP_CONTEXT_APN_MAX (99 + 1)
#define APP_AT_PDP_CONTEXT_USERNAME_MAX (64 + 1)
#define APP_AT_PDP_CONTEXT_PASSWORD_MAX (64 + 1)
#define APP_AT_PDP_CONTEXT_IPV4ADDR_MAX (15 + 1)
#define APP_AT_PDP_CONTEXT_IPV6ADDR_MAX (39 + 1)
/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
#define APP_AT_PDP_CID_INVALID (0)
typedef struct
{
    uint8_t context_id;
    uint8_t context_type; // 定义中的IPV4V6类型 参考 APP_AT_PDP_CONTEXT_TYPE_e
    char apn[APP_AT_PDP_CONTEXT_APN_MAX];
    char username[APP_AT_PDP_CONTEXT_USERNAME_MAX];
    char password[APP_AT_PDP_CONTEXT_PASSWORD_MAX];
    uint8_t authentication;
    uint8_t pdp_id;    // 关联的Modem的CID, 0是无效值
    uint8_t status;    // 状态信息，激活或未激活 参考 APP_AT_PDP_CONTEXT_STATUS_e
    uint8_t is_act;    // 是否是通过QIACT命令激活的
//    char str_ipv4_addr[APP_AT_PDP_CONTEXT_IPV4ADDR_MAX];  //  PDP激活后的IPV4地址,字符串
//    char str_ipv6_addr[APP_AT_PDP_CONTEXT_IPV6ADDR_MAX];  //  PDP激活后的IPV6地址,字符串
}APP_AT_PDP_ContextInfo;

typedef struct
{
    uint8_t pdp_id;    //   关联的Modem的CID, 0是无效值
    uint8_t context_status; //   状态信息，激活或未激活
    uint8_t context_type;
    char str_ipv4_addr[APP_AT_PDP_CONTEXT_IPV4ADDR_MAX];  //  PDP激活后的IPV4地址,字符串
    char str_ipv6_addr[APP_AT_PDP_CONTEXT_IPV6ADDR_MAX];  //  PDP激活后的IPV6地址,字符串
}APP_AT_PDP_ActiveInfo;

typedef enum
{
    APP_AT_PDP_CONTEXT_TYPE_IPV4    =   1,
    APP_AT_PDP_CONTEXT_TYPE_IPV6    =   2,
    APP_AT_PDP_CONTEXT_TYPE_IPV4V6  =   3,
    APP_AT_PDP_CONTEXT_TYPE_NONE,
}APP_AT_PDP_CONTEXT_TYPE_e;
#define APP_AT_PDP_CONTEXT_TYPE_CHECK(ip_type) (ip_type >= APP_AT_PDP_CONTEXT_TYPE_IPV4  && ip_type <= APP_AT_PDP_CONTEXT_TYPE_IPV4V6)
#define APP_AT_PDP_CONTEXT_TYPE_STRING(ip_type) ((ip_type == APP_AT_PDP_CONTEXT_TYPE_IPV4) ? "IP" : (ip_type == APP_AT_PDP_CONTEXT_TYPE_IPV6 ? "IPV6" : "IPV4V6"))
typedef enum
{
    APP_AT_PDP_CONTEXT_AUTH_NONE    =   0,
    APP_AT_PDP_CONTEXT_AUTH_PAP     =   1,
    APP_AT_PDP_CONTEXT_AUTH_CHAP    =   2,
    APP_AT_PDP_CONTEXT_AUTH_PAP_OR_CHAP    =   3,
}APP_AT_PDP_CONTEXT_AUTH_e;
#define APP_AT_PDP_CONTEXT_AUTH_CHECK(auth_type) (auth_type >= APP_AT_PDP_CONTEXT_AUTH_NONE  && auth_type <= APP_AT_PDP_CONTEXT_AUTH_PAP_OR_CHAP)

typedef enum
{
    APP_AT_PDP_CONTEXT_STATUS_DEACTIVE      =   0,
    APP_AT_PDP_CONTEXT_STATUS_ACTIVE        =   1,
    //  APP_AT_PDP_CONTEXT_STATUS_IPV4_ACTIVE   =   1,
    //  APP_AT_PDP_CONTEXT_STATUS_IPV6_ACTIVE   =   2,
    //  APP_AT_PDP_CONTEXT_STATUS_IPV4V6_ACTIVE =   3,
}APP_AT_PDP_CONTEXT_STATUS_e;

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void APP_AT_X_ICSGP_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_ICSGP_Test(uint8_t channelId);

void APP_AT_X_IACT_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_IACT_Read(uint8_t channelId);
void APP_AT_X_IACT_Test(uint8_t channelId);

void APP_AT_X_IDEACT_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_IDEACT_Test(uint8_t channelId);

void APP_AT_X_IDNSCFG_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_IDNSCFG_Test(uint8_t channelId);

void APP_AT_X_IDNSGIP_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_IDNSGIP_Test(uint8_t channelId);

//   MODEM PDP激活去激活通知
void APP_AT_PDP_DEActive_Notify(uint8_t pdp_id);
void APP_AT_PDP_Active_Notify(uint8_t pdp_id);

//   ContextID 取值范围 [1:APP_AT_PDP_CONTEXT_MAX]
uint8_t APP_AT_PDP_Get_Context_PdpId(uint8_t ContextID);
//   ContextID 取值范围 [1:APP_AT_PDP_CONTEXT_MAX]
uint8_t APP_AT_PDP_Get_Context_Status(uint8_t ContextID);
//   ContextID 取值范围 [1:APP_AT_PDP_CONTEXT_MAX]
uint8_t APP_AT_PDP_Get_Context_Type(uint8_t ContextID);

//   在任务中发送AT+CFUN=0和AT+CFUN=1实现重新搜网的功能
void APP_AT_4GNet_Reboot_Aysn(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __APP_AT_PDP_H__ */
