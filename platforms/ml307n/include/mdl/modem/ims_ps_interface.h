
#ifndef   IMS_PS_INTERFACE_H
#define   IMS_PS_INTERFACE_H

#include <os.h>
#include <oss_types.h>
#include "ap_ps_interface.h"
#include "l3cdec_header.h"
#include "usim_data.h"

#ifdef IMS_IT
#ifdef MID_AT_PRINT_INFO
#undef MID_AT_PRINT_INFO
#define MID_AT_PRINT_INFO(format, ...)   osPrintf(format, ##__VA_ARGS__)
#endif
#endif

#define ATC_USIM_EID_LEN                       (uint8_t)10
#define IMS_PS_CHID_BASE                       200 /*200-254IMS 和 无线协议 MODEM 交互 AT 命令通道*/
#define IMS_PS_CHID_MAX                        254 /*注意不能是255,255是主动上报用的*/

/*--------------------------------------------------------------LINE------------------------------------------------------------------------*/
#define IMS_AT_IMEI_MAX_LEN  16
#define IMS_AT_IMSI_MAX_LEN  16
#define IMS_MAX_URL_SIZE 256

#define  IMS_PROC_STATE_NULL                (uint8_t)0
#define  IMS_PROC_STATE_REG_START           (uint8_t)1
#define  IMS_PROC_STATE_REG_END             (uint8_t)2
#define  IMS_PROC_STATE_EMERG_START         (uint8_t)3
#define  IMS_PROC_STATE_EMERG_END           (uint8_t)4
#define  IMS_PROC_STATE_VOICE_START         (uint8_t)5
#define  IMS_PROC_STATE_VOICE_END           (uint8_t)6
#define  IMS_PROC_STATE_VEDIO_START         (uint8_t)7
#define  IMS_PROC_STATE_VEDIO_END           (uint8_t)8
#define  IMS_PROC_STATE_SMS_START           (uint8_t)9
#define  IMS_PROC_STATE_SMS_END             (uint8_t)10

#define IMS_SUBMODE_LTE_TDD                 (uint8_t)0
#define IMS_SUBMODE_LTE_FDD                 (uint8_t)1
typedef enum
{
    IMS_MODULE_REG = 0x01,
    IMS_MODULE_CC = 0x02,
    IMS_MODULE_SMS = 0x04,
    IMS_MODULE_SS = 0x08,
    IMS_MODULE_USSD = 0x10,
}E_IMS_MODULE;



#if 1
#define USIM_MAX_ECC_REC_NUM    20
#define USIM_ECC_CALL_NUM_LEN   3
#define USIM_ECC_MIN_LEN        4

typedef struct {
    uint8_t                        bSrvCategory;   /* Serivce Category */
    uint8_t                        atCallNum[USIM_ECC_CALL_NUM_LEN];
} S_Usim_EccRecInfo;

typedef struct {
    uint8_t                        bRecNum;   /* ECC 记录数 */
    S_Usim_EccRecInfo              atEccRec[USIM_MAX_ECC_REC_NUM];
} S_Usim_Ecc;
#endif

/*==============================================================================
 Primitive:   NAS_IMS_UICC_INFO_IND (NAS->IMS)
 Description: UICC_INFO_IND
==============================================================================*/
typedef struct
{
    uint8_t bUiccReady;/*1:ready，0:not ready,比如需要pin*/
    uint8_t bMnclen;
    uint8_t bImsilen;
    uint8_t abImsi[IMS_AT_IMSI_MAX_LEN];
    uint8_t abImei[IMS_AT_IMEI_MAX_LEN/2];
}S_NasIms_UiccInfo_Ind;

/*==============================================================================
 Primitive:   NAS_IMS_ISIM_INFO_IND (NAS->IMS)
 Description: ISIM_INFO_IND
==============================================================================*/
typedef struct
{
    uint8_t bIsimExistFlag;
    uint8_t bIsimChnlId;
    char acDomain[IMS_MAX_URL_SIZE];
    char acImpi[IMS_MAX_URL_SIZE];
    char acImpu[IMS_MAX_URL_SIZE];
}S_NasIms_IsimInfo_Ind;

/*==============================================================================
 Primitive:   NAS_IMS_EPS_REG_STATE_IND (NAS->IMS)
 Description: EPS_REG_STATE_IND
==============================================================================*/
typedef struct
{
    uint8_t EpsRegState;
}S_NasIms_EpsRegState_Ind;

/*==============================================================================
 Primitive:   NAS_IMS_IMS_VO_PS_IND (NAS->IMS)
 Description: IMS_VO_PS_IND
==============================================================================*/
typedef struct
{
    uint8_t ImsVoPs;
    uint8_t bSbyWakeFlg; /* bool, indicate is standby wakeup */
}S_NasIms_ImsVoPs_Ind;

/*==============================================================================
 Primitive:   NAS_IMS_IEMERGENCY_BEARER_IND (NAS->IMS)
 Description: IEMERGENCY_BEARER_IND
==============================================================================*/
typedef struct
{
    uint8_t bEmergencybearerInd;
    uint8_t bEmergencyFallback;
}S_NasIms_EmergencyBearer_Ind;

/*==============================================================================
 Primitive:   NAS_IMS_EMC_NUM_IND (NAS->IMS)
 Description: EMC_NUM_IND
==============================================================================*/
typedef struct
{
    S_L3_EcNumList tEmerNumberList;
}S_NasIms_EmcNum_Ind;

/*==============================================================================
 Primitive:   NAS_IMS_LTENOCELL_IND (NAS->IMS)
 Description: LTENOCELL_IND
==============================================================================*/


/*==============================================================================
 Primitive:   NAS_IMS_CGEV_IND (NAS->IMS)
 Description: CGEV_IND
==============================================================================*/
typedef S_AtiEsm_Cgev_Ind S_NasIms_Cgev_Ind;

/*==============================================================================
 Primitive:   IMS_NAS_READ_ECC_REQ (IMS->NAS)
 Description: READ_ECC_REQ
==============================================================================*/
typedef struct
{
    uint8_t bChannelId;
}S_ImsNas_ReadEcc_Req;

/*==============================================================================
 Primitive:   IMS_NAS_READ_ECC_CNF (NAS->IMS)
 Description: READ_ECC_CNF
==============================================================================*/
typedef struct
{
    uint8_t bChannelId;
    S_Usim_Ecc tEmerNumberList;
}S_ImsNas_ReadEcc_Cnf;

/*==============================================================================
 Primitive:   IMS_ATI_CGDCONT_CNF (ATI->IMS)
 Description: CGDCONT set cnf
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;
    uint8_t Cid;
    uint8_t Rslt;
}S_ImsAti_Cgdcont_Cnf;

/*==============================================================================
 Primitive:   IMS_NAS_PDP_ACTIVATE_REQ (IMS->NAS)
 Description: PDP_ACTIVATE_REQ
==============================================================================*/

typedef  S_AtiEsm_PdpAct_Req   S_ImsNas_PdpAct_Req;

/*==============================================================================
 Primitive:   IMS_NAS_PDP_ACTIVATE_CNF (NAS->IMS)
 Description: PDP_ACTIVATE_CNF
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;
    uint8_t Cid;
    uint8_t Rslt;
}S_ImsNas_PdpAct_Cnf;

/*==============================================================================
 Primitive:   IMS_NAS_CGCONTRDP_REQ (IMS->NAS)
 Description: CGCONTRDP_REQ
==============================================================================*/
typedef S_AtiEsm_Cgcontrdp_Req S_ImsNas_Cgcontrdp_Req;

/*==============================================================================
 Primitive:   IMS_NAS_CGCONTRDP_CNF (NAS->IMS)
 Description: CGCONTRDP_CNF
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;
    uint8_t bResult;
    S_AtiEsm_Cgcontrdp_Para  tCgdcontrdp;
}S_ImsNas_Cgcontrdp_Cnf;

/*==============================================================================
 Primitive:   IMS_NAS_AUTH_REQ (IMS->NAS)
 Description: AUTH_REQ
==============================================================================*/
typedef S_EmmUsim_AuthReq S_ImsNas_Auth_Req;

/*==============================================================================
 Primitive:   IMS_NAS_AUTH_CNF (NAS->IMS)
 Description: AUTH_CNF
==============================================================================*/
typedef S_EmmUsim_AuthRsp S_ImsNas_Auth_Cnf;

/*==============================================================================
 Primitive:   IMS_NAS_IMSREG_STATE_REQ (IMS->NAS)
 Description: IMSREG_STATE_REQ
==============================================================================*/
typedef struct 
{
    uint8_t bChannelId;
    uint8_t bImsRegState; /*ATENM_IMS_REGISTERED;
                            ATENM_IMS_DEREGISTERED*/
}S_ImsNas_ImsRegState_Req;

/*==============================================================================
 Primitive:   IMS_NAS_IMSPROC_STATE_REQ (IMS->NAS)
 Description: ims notify nas all progress state
==============================================================================*/
typedef struct 
{
    uint8_t bChannelId;
    uint8_t bImsProcState; /*Refer IMS_PROC_STATE_REG_START*/
}S_ImsNas_ImsProcState_Req;


/*==============================================================================
 Primitive:   IMS_NAS_CEREG_QUERY_REQ (IMS->NAS)
 Description: CEREG_QUERY_REQ
==============================================================================*/
typedef S_AtiEmm_CeregQuery_Req S_ImsNas_CeregQuery_Req;

/*==============================================================================
 Primitive:   IMS_NAS_CEREG_QUERY_CNF (NAS->IMS)
 Description: CEREG_QUERY_CNF
==============================================================================*/
#define RPLMN_IS_HPLMN     0
#define RPLMN_IS_EHPLMN    1
#define RPLMN_IS_VPLMN     2

typedef struct
{
    uint8_t bChannelId;
    uint8_t bTacFg;
    uint8_t bCellFg;
    uint8_t bEpsRegState;
    uint8_t bAct;
    uint8_t bSubAct;/*这个还有没有，如果没有，填啥，P-Access-Network-Info要用*/
    uint16_t wTac;
    uint32_t dwCell;
    uint8_t bRPlmnFg;/* 0:HPLMN 1:EHPLMN 2:VPLMN */
    S_PS_PlmnId tCurPlmn;
}S_ImsNas_CeregQuery_Cnf;

/*==============================================================================
 Primitive:   IMS_NAS_PDP_DEACTIVATE_REQ (IMS->NAS)
 Description: PDP_DEACTIVATE_REQ
==============================================================================*/
typedef S_AtiEsm_PdpAct_Req S_ImsNas_PdpDeact_Req;

/*==============================================================================
 Primitive:   IMS_NAS_PDP_DEACTIVATE_CNF (NAS->IMS)
 Description: PDP_DEACTIVATE_CNF
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;
    uint8_t Cid;
    uint8_t Rslt;
}S_ImsNas_PdpDeact_Cnf;

/*==============================================================================
 Primitive:   NAS_IMS_C5GREG_STATE_IND (NAS->IMS)
 Description: C5GREG_STATE_IND
==============================================================================*/
typedef struct
{
    uint8_t C5gRegState;
}S_NasIms_C5gRegState_Ind;

/*==============================================================================
 Primitive:   IMS_NAS_C5GREG_QUERY_REQ (IMS->NAS)
 Description: C5REG_QUERY_REQ
==============================================================================*/
//typedef S_AtiEnm_C5gregReq S_ImsNas_C5gRegQuery_Req;

/*==============================================================================
 Primitive:   IMS_NAS_C5GREG_QUERY_CNF (NAS->IMS)
 Description: C5REG_QUERY_CNF
==============================================================================*/
typedef struct
{
    uint8_t  bChannelId;
    uint8_t  bTacFg:1;
    uint8_t  bCellFg:1;
    uint8_t  bRegState:4;
    uint8_t  bSubAct;/*这个还有没有，P-Access-Network-Info要用*/
    uint32_t  dwTac;
    uint64_t  u64Ci;
}S_ImsNas_C5gRegQuery_Cnf;


/*==============================================================================
 Primitive:   IMS_ATI_GET_PLMN_NAME_REQ (IMS->ATI)
 Description: GET_PLMN_NAME_REQ
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;
    S_PS_PlmnId tPlmnId;
}S_ImsAti_GetPlmnName_Req;

/*==============================================================================
 Primitive:   IMS_ATI_GET_PLMN_NAME_CNF (ATI->IMS)
 Description: GET_PLMN_NAME_CNF
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;
    uint8_t Result;
    char fullname[MAX_LONGNAME_LEN+1];
    char shortname[MAX_LONGNAME_LEN+1];
}S_ImsAti_GetPlmnName_Cnf;


/*==============================================================================
 Primitive:   IMS_ATI_IMSBAR_QUERY_REQ (IMS->ATI)
 Description: IMSBAR_QUERY_REQ
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;	
    uint8_t bSrvType;/* 1 : voice; 2: video; 3: sms; 4: emergency */
}S_ImsAti_ImsbarQuery_Req;

/*==============================================================================
 Primitive:   IMS_ATI_IMSBAR_QUERY_CNF (ATI->IMS)
 Description: IMSBAR_QUERY_CNF
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;
    uint8_t bValue;
}S_ImsAti_ImsbarQuery_Cnf;

/*==============================================================================
 Primitive:   IMS_NAS_EMERGENCY_SERVICE_FALLBACK_REQ (IMS->NAS)
 Description: EMERGENCY_SERVICE_FALLBACK_REQ
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;	
}S_ImsNas_EmergencyServiceFallback_Req;

/*==============================================================================
 Primitive:   NAS_IMS_EMERGENCY_SERVICE_FALLBACK_CNF (NAS->IMS)
 Description: EMERGENCY_SERVICE_FALLBACK_CNF
==============================================================================*/
typedef struct
{
    uint8_t ChanelId;
    uint8_t bResult;    /* 0: Success; other: fail */
}S_ImsNas_EmergencyServiceFallback_Cnf;

/*==============================================================================
 Primitive:   ATC_IMS_CFUN_REQ/IMS_NAS_RF_CTRL_REQ (ATI->IMS)
 Description: ATC_IMS_CFUN_REQ/IMS_NAS_RF_CTRL_REQ
==============================================================================*/
typedef S_AtiEmm_RFControl_Req S_ImsNas_RFControl_Req;



/*--------------------------------------------------------------LINE------------------------------------------------------------------------*/

#define IMS_SMS_RP_MSG_MAX_SIZE  (uint8_t)176
#define IMS_SCA_STRING_MAX_SIZE  (uint8_t)25

/*==============================================================================
 Primitive:   IMS_SMS_RCV_DATA_IND (IMS->NAS)
 Description: IMS通知NAS SMS模块收到RP层消息
==============================================================================*/
typedef struct
{
    uint8_t   bChanId;
    uint8_t   bRpMsgLen;        /*IMS收到的RP层数据长度*/
    uint8_t   abRpMsg[255];     /*IMS收到的RP层数据内容*/
}S_ImsSms_RcvDataInd;

/*==============================================================================
 Primitive:   SMS_IMS_SEND_DATA_REQ (NAS->IMS)
 Description: NAS SMS模块请求IMS向网络侧发送RP层消息
==============================================================================*/
typedef struct
{
    uint8_t   bChanId;
    uint8_t   bRpMsgType; /* 0: RP-DATA; 2: RP-ACK; 4: RP-ERROR; 6: RP-SMMA */
    uint8_t   bRpMsgLen;  /*需要IMS发送的RP层数据长度*/
    uint8_t   abRpMsg[IMS_SMS_RP_MSG_MAX_SIZE];     /*需要IMS发送的RP层数据内容*/
    char      acSca[IMS_SCA_STRING_MAX_SIZE];       /*短信中心号码； MO SMS时填写Request-URI 和To*/
}S_ImsSms_SendDataReq;

/*==============================================================================
 Primitive:   SMS_IMS_SEND_DATA_CNF (IMS->NAS)
 Description: IMS通知NAS SMS模块发送RP层消息的结果
==============================================================================*/
typedef struct
{
    uint8_t   bChanId;
    uint8_t   bResult;  /* 数据发送结果， 0： 发送成功； 1：发送失败 */
    uint8_t   bCause;  /* 发送失败时使用，表示失败原因 */
    uint8_t   bRpMsgType;
}S_ImsSms_SendDataCnf;

/*==============================================================================
 Primitive:   SMS_IMS_REL_ENTITY_IND (NAS->IMS)
 Description: NAS SMS模块通知IMS清除SMS实体
==============================================================================*/
typedef struct
{
    uint8_t   bSmsEntityType; /* 需要清除的实体类型 0： SMS_MO； 1：SMS_MT */
    uint8_t   bDestryTsxFlag;
}S_ImsSms_RelEntityInd;

#define MODE_DISABLE       0
#define MODE_ENABLE        1
#define MODE_QUERY         2
#define IMSLIR_0           0
#define IMSLIR_1           1
#define IMSLIR_2           2
#define IMSLIR_MODE_QUERY  3
typedef uint8_t IMS_SS_OP_MODE;

#define STATE_DISABLE   0
#define STATE_ENABLE    1
#define STATE_UNSIGNED  2
typedef uint8_t IMS_SS_STATE;

#define FAC_AI  0
#define FAC_IR  1
#define FAC_OI  2
#define FAC_OR  3
#define FAC_AO  4
typedef uint8_t IMS_SS_ICB_FAC;

#define IMS_SS_IMSLIP   0
#define IMS_SS_IMSLIR   1
#define IMS_SS_IMSOLP   2
#define IMS_SS_IMSOLR   3
#define IMS_SS_IMSCWA   4
typedef uint8_t IMS_SS_AT_TYPE;

#define REASON_CFU    0
#define REASON_CFB    1
#define REASON_CFNR   2
#define REASON_CFNRc  3
typedef uint8_t IMS_SS_CF_REASON;

#define IMS_SS_AT_MODE_QUERY   0
#define IMS_SS_AT_MODE_SET      1
typedef uint8_t IMS_SS_AT_MODE;

#define IMS_SS_TIME_MAX_LEN 64
#define IMS_SS_NR_TIMER_LEN 8
#define IMS_CALL_NUMBER_LEN 25

/*==============================================================================
 Primitive:   ATC_IMS_IMSCFC_REQ (ATI->IMS)
 Description: AT命令+IMSCFC的请求消息
==============================================================================*/
typedef struct
{
    uint8_t   bChannelId;
    uint8_t   bReason;     /* 前转条件，0：无条件(CFU)；           1：用户忙(CFB)；
                                        2: 无应答(CFNR)；3：不可达(CFNRc) */ 
    uint8_t   bMode;       /* 命令模式，0：去激活； 1：激活； 2：查询 */      
    uint8_t   bNumberFg;                                    
    char      acNumber[IMS_CALL_NUMBER_LEN+1];/* 前转号码 */ 
    uint8_t   bTimerFg;
    char      acTime[IMS_SS_TIME_MAX_LEN+1];
    uint8_t   bCfnrTimer;
}S_AtcIms_ImscfcReq;

/*==============================================================================
 Primitive:   ATC_IMS_IMSCFC_RSP (IMS->ATI)
 Description: AT命令+IMSCFC的响应消息
==============================================================================*/
typedef struct
{
    uint8_t   bChannelId;
    uint32_t  bResult;   /* 0表示成功, 其他值表示错误原因*/
    uint8_t   bMode;     /* 命令模式      0：去激活；1：激活；2：查询(该模式需携带以下参数)*/  
    uint8_t   bReason;   /* 前转条件      0：无条件(CFU)；1：用户忙(CFB)；
                                      2: 无应答(CFNR)；3：不可达(CFNRc) */   
    uint8_t   bStatus;   /* 前转状态      0：未激活； 1：已激活； 2：未签约 */
    char      acNumber[IMS_CALL_NUMBER_LEN];  /* 前转号码 */ 
    char      acNoReplyTimer[IMS_SS_NR_TIMER_LEN];
    char      acTime[IMS_SS_TIME_MAX_LEN];
}S_AtcIms_ImscfcRsp;

/*==============================================================================
 Primitive:   ATC_IMS_IMSLCK_REQ (ATI->IMS)
 Description: AT命令+IMSLCK的请求消息
==============================================================================*/
typedef struct
{
    uint8_t         bChannelId;
    IMS_SS_ICB_FAC  bFac;    /* 0:AI; 1:IR; 2:OI; 3:OR; 4:AO */ 
    uint8_t         bMode;   /* 0:去激活; 1:激活; 2:查询 */ 
}S_AtcIms_ImslckReq;

/*==============================================================================
 Primitive:   ATC_IMS_IMSLCK_RSP (IMS->ATI)
 Description: AT命令+IMSLCK的响应消息
==============================================================================*/
typedef struct
{
    uint8_t         bChannelId;
    IMS_SS_ICB_FAC  bFac;    /* 0:AI; 1:IR; 2:OR; 3:OI; 4:AO */ 
    uint8_t         bMode;   /* 0:去激活; 1:激活; 2:查询 */ 
    uint16_t        bResult; /* 0表示成功, 其他值表示错误原因*/
    uint8_t         bStatus; /* 0:未激活; 1:已激活; 2:未签约 mode为2时携带*/ 
}S_AtcIms_ImslckRsp;

/*==============================================================================
 Primitive:   ATC_IMS_SS_COMMON_REQ, (ATI->IMS)
 Description: AT命令+IMSLIP，+IMSLIR，+IMSOLP, +IMSOLR共用的设置请求消息
==============================================================================*/
typedef struct
{
    uint8_t         bChannelId;
    IMS_SS_AT_TYPE  bAtType;   /* 0:IMSLIP; 1:IMSLIR; 2:IMSOLP; 3:IMSOLR; */ 
    IMS_SS_AT_MODE  bAtMode;   /* AT 模式 0: 查询模式， 1： 设置模式*/
    uint8_t         bNFg;      /* 是否携带参数m       0:未携带; 1:携带 */ 
    uint8_t         bN;
    uint8_t         bMFg;      /* 是否携带参数m       0:未携带; 1:携带 */ 
    uint8_t         bM;
}S_AtcSs_CommonReq;

/*==============================================================================
 Primitive:   ATC_IMS_SS_COMMON_RSP (IMS->ATI)
 Description: AT命令+IMSLIP，+IMSLIR，+IMSOLP，+IMSOLR共用的响应消息
==============================================================================*/
typedef struct
{
    uint8_t         bChannelId;
    IMS_SS_AT_TYPE  bAtType;  /* 0:IMSLIP; 1:IMSLIR; 2:IMSOLP; 3:IMSOLR; */ 
    IMS_SS_AT_MODE  bAtMode;  /* AT 模式 0: 查询模式， 1： 设置模式*/
    uint16_t        bResult;  /* 0表示成功, 其他值表示错误原因*/
    uint8_t         bN;
    uint8_t         bM;
}S_AtcSs_CommonRsp;

void ims_is_ongonging_set(E_IMS_MODULE mod);
void ims_is_ongonging_clear(E_IMS_MODULE mod);

extern int32_t Ims_Com_SendAtMsgtoIms2Ps(int32_t ChId,int32_t MsgId,uint8_t* pMsgCont, int32_t MsgContsize);
extern int32_t Ims_Com_SendAtiMsgtoMain(int32_t ChId,uint32_t MsgId,uint8_t* pMsgCont, int32_t MsgContsize,int32_t transparent);
extern int32_t Ims_SendMsgtoIms(int32_t MsgId,uint8_t* pMsgCont, int32_t MsgContsize);

extern uint8_t Rrc_GetBarStatus(uint8_t bMode);
extern void AtiIms_CommonCnf(uint8_t bChannelId, uint8_t bResult, uint32_t dwCause);
extern void AtiIms_CvmodReadCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t bVMode);
extern void AtiIms_CcwaSetCnf(S_Atc_Ims_Ccwa_Cnf *ptCcwaCnf);
extern void AtiIms_CcwaReadCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t bN);
extern void AtiIms_ImsusdReadCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t bN);
extern void AtiIms_ClccExecCnf(S_Atc_Ims_Clcc_Cnf *ptClccCnf);
extern void AtiIms_ImscfcSetCnf(S_AtcIms_ImscfcRsp *ptImscfcRsp);
extern void AtiIms_ImslckSetCnf(S_AtcIms_ImslckRsp *ptImslckRsp);
extern void AtiIms_ImsamrwReadCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t bN);
extern void AtiIms_SsCommonCnf(S_AtcSs_CommonRsp *ptRsp);
extern void AtiIms_ImsapnReadCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t *pbApn);
extern void AtiIms_ImsconfuriReadCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t *pbConfuri);
extern void AtiIms_ImstestSetCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t *pbConfuri, uint8_t dwN);
extern void AtiIms_CmsmsoinReadCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t bN);
#endif


