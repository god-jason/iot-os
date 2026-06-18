
#ifndef   AP_PS_INTERFACE_H
#define   AP_PS_INTERFACE_H

#include <os.h>
#include <oss_types.h>

#ifndef BITS
typedef uint8_t                                   BITS;
#endif

#ifdef CFW_API_SUPPORT
#ifndef bool
#define bool    uint8_t
#endif
#endif

#ifndef R_VALID
#define R_VALID                                     (uint8_t)1
#endif
#ifndef R_INVALID
#define R_INVALID                                   (uint8_t)0
#endif

#ifndef R_SUCC
#define R_SUCC                                      (uint8_t)0
#endif
#ifndef R_FAIL
#define R_FAIL                                      (uint8_t)1
#endif

#ifndef R_TRUE
#define R_TRUE                                      (uint8_t)1
#endif
#ifndef R_FALSE
#define R_FALSE                                     (uint8_t)0
#endif

#define PS_NAS_THREAD_ID                            (uint8_t)0
#define PS_RRC_THREAD_ID                            (uint8_t)1
#define PS_L2_THREAD_ID                             (uint8_t)2

#define IMEILEN                                     (uint8_t)8
#define IMEISVLEN                                   (uint8_t)8

/************************************************************************************
 *                                 AT命令错误码 Start                                    *
 ************************************************************************************/
/* 自定义错误码 */
typedef enum
{
    CME_ERR_CMD_CONFLICT         = 6000,  //通道堵塞
    CME_ERR_CMD_OVERFLOW         = 6001,  //命令长度输入超限
    CME_ERR_CMD_UNKNOW           = 6003,  //命令不识别
    CME_ERR_UNVALID_PARAM        = 6004,  //命令参数不对
    CME_ERR_APN_AUTH_FAIL        = 6007,   //APN auth fail
    CME_ERR_UNKNOW_PDP_TYPE      = 6008,   //#28 cause

    // CME ERR code for AT framework
    CME_ERR_CHANNEL_BUSY         = 9000,
    CME_ERR_LENGTH_TOO_LONG      = 9001,
    CME_ERR_SYSTEM_NO_MEMORY     = 9002,
    CME_ERR_UNVALID_AT_CTRL      = 9003,
    CME_ERR_SYSTEM_ERROR         = 9004,

    AT_CME_ERR_CODE_MAX          = 9999  ///<   CME ERROR错误码支持最多4位
} AT_ApiCmeErrorCode;

/* IMS错误码 */
typedef enum
{
    IMS_APP_CALL_CME_UNPERMMITED=3,
    IMS_APP_CALL_CME_UNSUPPORTED=4,
    IMS_APP_CALL_CME_NOT_FOUND=22,
    IMS_APP_CALL_CME_CONF_INNNER_ERROR=132,    
    IMS_APP_CALL_CME_CHANNEL_BUSY=6000,
    IMS_APP_CALL_CME_PARAM_UNSUPPORTED=6004,
    IMS_APP_CALL_CME_HOLD_ERR_1=7000,
    IMS_APP_CALL_CME_HOLD_ERR_2=7001,
    IMS_APP_CALL_CME_HOLD_ERR_3=7002,
    IMS_APP_CALL_CME_INVALID_ZID=7003,
    IMS_APP_CALL_CME_INVALID_PID=7004,
    IMS_APP_CALL_CME_INVALID_CONTEXT=7005,
    IMS_APP_CALL_CME_ZINFO_FAILD=7006,
    IMS_APP_CALL_CHLD2_BEFORE_CHLD3_NOT_NECCESSARY=7007,
    IMS_APP_CALL_CME_REDUNDANT_HANG_UP=7008,
    IMS_APP_CALL_CME_CHLD_ERR_1=7009,
    IMS_APP_CALL_CME_CONCURRENT=7010,
    IMS_APP_CALL_CHLD2_BEFORE_TRANSFER_CLIENT_NOT_NECCESSARY=7011,
    IMS_APP_CALL_HANGUP_AFTER_TRANSFER_SUCCESS_NOT_NECCESSARY=7012,
    IMS_APP_CALL_IMS_BAR = 7013,

    IMS_APP_CALL_CME_CLEAR_AT_CHANNEL_BY_SRVCC=8000,
}T_IMS_APP_CALL_CME_ERROR;

typedef enum
{
    CMS_ERR_SMSCONTENT_WRONG     = 6002,
    CME_ERR_SENDMSG_FAIL         = 6005,
    AT_CMS_ERR_CODE_MAX          = 9999  ///<   CMS ERROR错误码支持最多4位
} AT_ApiCmsErrorCode;

/* +CME ERROR:<err> failure cause define.27007 9.2 */
#define AT_CAUSE_PHONEFAIL                          (uint8_t)0  /*phone failure*/
#define AT_CAUSE_OPTNOTALW                          (uint8_t)3  /*operation not allowed*/
#define AT_CAUSE_OPTNOTSPT                          (uint8_t)4  /*operation not supported*/
#define AT_CAUSE_SIMNOTINSERT                       (uint8_t)10 /*SIM not inserted*/
#define AT_CAUSE_SIMPINREQ                          (uint8_t)11 /*SIM PIN required*/
#define AT_CAUSE_SIMPUKREQ                          (uint8_t)12 /*SIM PUK required*/
#define AT_CAUSE_SIMFAIL                            (uint8_t)13 /*SIM faliure*/
#define AT_CAUSE_SIMBUSY                            (uint8_t)14 /*SIM busy*/
#define AT_CAUSE_SIMWRONG                           (uint8_t)15 /*SIM wrong*/
#define AT_CAUSE_INCORRECTPWD                       (uint8_t)16 /*incorrect password*/
#define AT_CAUSE_SIMPIN2REQ                         (uint8_t)17 /*SIM PIN2 required*/
#define AT_CAUSE_SIMPUK2REQ                         (uint8_t)18 /*SIM PUK2 required*/
#define AT_CAUSE_MEMFULL                            (uint8_t)20
#define AT_CAUSE_INVALID_INDEX                      (uint8_t)21 /*invalid index*/
#define AT_CAUSE_NOT_FOUND                          (uint8_t)22 /*not found*/
#define AT_CAUSE_MEMFAIL                            (uint8_t)23 /*memory failure*/
#define AT_CAUSE_NOTNETSVC                          (uint8_t)30 /*no network service*/
#define AT_CAUSE_EMCCALLONLY                        (uint8_t)32 /*network not allowed - emergency calls only*/
#define AT_CAUSE_INCORRECT_PARAM                    (uint8_t)50 /*Incorrect parameters*/
#define AT_CAUSE_UNKNOWNERR                         (uint8_t)100 /*unknown*/
#define AT_CAUSE_ILLEGALMS_ERR                      (uint8_t)103 /*Illegal MS*/
#define AT_CAUSE_ILLEGALME_ERR                      (uint8_t)106 /*Illegal ME*/
#define AT_CAUSE_GPRSSRVNOTALW_ERR                  (uint8_t)107 /*GPRS services not allowed*/
#define AT_CAUSE_PLMNNOTALW_ERR                     (uint8_t)111 /*PLMN not allowed*/
#define AT_CAUSE_LANOTALW_ERR                       (uint8_t)112 /*Location area not allowed*/
#define AT_CAUSE_ROAMNOTALWINLA_ERR                 (uint8_t)113 /*Roaming not allowed in this location area*/

/* additional +CMS ERROR:<err> failure cause define.27005 3.2.5 */
#define ATI_CMS_MEM_CAPA_EXCEED_ERR                 (uint16_t)211
#define ATI_CMS_SMSSVCRSV_ERR                       (uint16_t)301
#define ATI_CMS_MEFAILURE_ERR                       (uint16_t)300
#define ATI_CMS_OPTNOTALLOW_ERR                     (uint16_t)302
#define ATI_CMS_OPTNOTSPT_ERR                       (uint16_t)303
#define ATI_CMS_INVALIDPDUPARA_ERR                  (uint16_t)304
#define ATI_CMS_INVALIDTEXTPARA_ERR                 (uint16_t)305
#define ATI_CMS_USIMNOTINSERT_ERR                   (uint16_t)310
#define ATI_CMS_USIMPINREQ_ERR                      (uint16_t)311
#define ATI_CMS_PHUSIMPINREQ_ERR                    (uint16_t)312
#define ATI_CMS_USIMFAIL_ERR                        (uint16_t)313
#define ATI_CMS_USIMBUSY_ERR                        (uint16_t)314
#define ATI_CMS_USIMWRONG_ERR                       (uint16_t)315
#define ATI_CMS_USIMPUKREQ_ERR                      (uint16_t)316
#define ATI_CMS_USIMPIN2REQ_ERR                     (uint16_t)317
#define ATI_CMS_USIMPUK2REQ_ERR                     (uint16_t)318
#define ATI_CMS_MEMFAIL_ERR                         (uint16_t)320
#define ATI_CMS_INVALIDMEM_ERR                      (uint16_t)321
#define ATI_CMS_MEMFULL_ERR                         (uint16_t)322
#define ATI_CMS_SCAUNKNOWN_ERR                      (uint16_t)330
#define ATI_CMS_NONETWORKSVC_ERR                    (uint16_t)331
#define ATI_CMS_NETWORKTIMEOUT_ERR                  (uint16_t)332
#define ATI_CMS_NOCNMAACK_ERR                       (uint16_t)340
#define ATI_CMS_UNKNOWN_ERR                         (uint16_t)500  /* other values are reserved.*/
#define ATI_CMS_MOABORTOK_ERR                       (uint16_t)501  /* MO SM aborted ok*/
#define ATI_CMS_NOTMATCH_ERR                        (uint16_t)502

/************************************************************************************
 *                                 AT命令错误码 End                                      *
 ************************************************************************************/

/* CEER error code */
/* EPS Mobility Management failure reason */
#define ATI_INVALIDPARA                        (uint8_t)0
#define ATI_ILLEGALMS                          (uint8_t)103    //Illegal Ms
#define ATI_ILLEGALME                          (uint8_t)106    //Illegal Me
#define ATI_GPRSNOTALLOWED                     (uint8_t)107    //Gprs Services Not Allowed
#define ATI_GPRSANDNONGPRSNOTALLOWED           (uint8_t)108    //Gprs and Non-Gprs Services Not Allowed
#define ATI_UEIDCANNOTBEDERIVEDBYNET           (uint8_t)109    //UE identity cannot be derived by the network
#define ATI_UEIMPLICITLYDETACHED               (uint8_t)110    //UE implicitly detached
#define ATI_PLMNNOTALLOWED                     (uint8_t)111    //Plmn Not Allowed
#define ATI_LANOTALLOWED                       (uint8_t)112    //Location area not allowed
#define ATI_ROAMINGNOTALLOWEDINLA              (uint8_t)113    //Roaming not allowed in this location area
#define ATI_GPRSNOTALLOWEDINTHEPLMN            (uint8_t)114    //GPRS services not allowed in the PLMN
#define ATI_NOSUITABLECELLINLA                 (uint8_t)115    //No suitable cells in routing area
#define ATI_AUTHREJ                            (uint8_t)116    //Authentication Reject
#define ATI_AUTHFAILURE                        (uint8_t)117    //Authentication Failure
#define ATI_IMEINOTACCEPT                      (uint8_t)118    //IMEI Not Accepted
#define ATI_IMSIPAGING                         (uint8_t)119    //Imsi Paging
#define ATI_MTDETACHREATTACH                   (uint8_t)120    //MT detach, Reattach Required
#define ATI_USERDEACTIVED                      (uint8_t)121    //User Deactived
#define ATI_CONGESTION                         (uint8_t)122    //Congestion
#define ATI_ESMFAILURE                         (uint8_t)123    //ESM failure
#define ATI_NOPDPCONTEXTACTIVATED              (uint8_t)124    //No PDP context activated
#define ATI_NOTAUTHORIZEDFORCSG                (uint8_t)125    //Not Authorized For This CSG
#define ATI_ATTACHLOWERLAYERFAIORREL           (uint8_t)126    //Attach Lower Layer Failure Or Release
#define ATI_T3410TIMEOUT                       (uint8_t)127    //T3410 Time Out
#define ATI_REQSERVICENOTAUTHORIZED            (uint8_t)128    //Requested service option not authorized in this PLMN
#define ATI_ACTIVEDEFAULTEPSBEARREJ            (uint8_t)129    //Active Default Eps Bear Reject
#define ATI_AUTHMACFAIL                        (uint8_t)130    //Authentication Mac Failure
#define ATI_AUTHSYNCFAIL                       (uint8_t)131    //Authentication Sync Failure
#define ATI_AUTHNONEEPS                        (uint8_t)132    //Authentication None Eps parameter
#define ATI_MTDETACHNOCAUSE                    (uint8_t)133    //Reattach not required and no cause
#define ATI_GETUEINFOFAILURE                   (uint8_t)134    //Get Ue Information Failure
#define ATI_T3310TIMEOUT                       (uint8_t)135    //T3310 Time Out
#define ATI_AUTHUNACCEPTABLE                   (uint8_t)136    //Authentication Unacceptable
#define ATI_ATTACHESTREJ                       (uint8_t)137    //Attach Establish Reject
#define ATI_UNSPECIFIEDGPRSERROR               (uint8_t)148    //Unspecified GPRS error
#define ATI_NORMALFAILFIVETIMES                (uint8_t)149    //Normal Fail Continuously Five Times
#define ATI_SEMANTICALLYINCORMSG               (uint8_t)172    //Semantically incorrect message
#define ATI_INVALIDMANDATORYINFO               (uint8_t)173    //Invalid mandatory information
#define ATI_MSGTYPENOEXSITORIMPL               (uint8_t)174    //Message type non-existent or not implemented
#define ATI_IENOEXISTORIMPL                    (uint8_t)175    //Information element non-existent or not implemented
#define ATI_PROTOCOLERROR                      (uint8_t)176    //Protocol error, unspecified
#define ATI_MSGTYPENOTCOMPWITHPROTOCSTAT       (uint8_t)177    //Message type not compatible with the protocol state
#define ATI_CONDITIONALIEERR                   (uint8_t)178    //Conditional IE error
#define ATI_MSGNOTCOMPWITHPROTOCSTAT           (uint8_t)179    //Message not compatible with the protocol state
#define ATI_MSGNORMALWITHNEWCELL_ERR           (uint8_t)180    //retry upon entry into a new cell
#define ATI_UESECURITYCAPAMISMATCH             (uint8_t)181    //UE security capabilities mismatch
#define ATI_SECURITYMODEREJECTED               (uint8_t)182    //Security mode rejected, unspecified

#define ATI_SSOPTMODE_QUERY                         (uint8_t)2
#define ATI_FACVALUE_SC                             (uint8_t)3
#define ATI_FACVALUE_P2                             (uint8_t)21

#define CFUN_OPER_POWROFF                           (uint8_t)0
#define CFUN_OPER_POWRON                            (uint8_t)1
#define CFUN_OPER_INFLY                             (uint8_t)4
#define CFUN_OPER_CARD_PWROFF                       (uint8_t)5

#define COPS_MODE_AUTO                              (uint8_t)0
#define COPS_MODE_MANU                              (uint8_t)1
#define COPS_MODE_DEREG                             (uint8_t)2
#define COPS_MODE_SETONLY                           (uint8_t)3
#define COPS_MODE_MANUAUTO                          (uint8_t)4

#define COPS_OPERFORMAT_LONGALPHA                   (uint8_t)0
#define COPS_OPERFORMAT_SHORTALPHA                  (uint8_t)1
#define COPS_OPERFORMAT_NUMERIC                     (uint8_t)2

#define COPS_STATE_UNKNOWNPLMN                      (uint8_t)0
#define COPS_STATE_AVAILPLMN                        (uint8_t)1
#define COPS_STATE_CURPLMN                          (uint8_t)2
#define COPS_STATE_FPLMN                            (uint8_t)3

#define COPS_MAX_NAME_LEN                           (uint8_t)64

#define PLMN_LIST_MAX_NUM                           (uint8_t)30    /*available PLMN NUM*/
#define AT_USIM_HEXDATA_MAX_LEN                     (uint8_t)255   /*max length of HEX Parameter */

#define CEREG_N_DISABLE                             (uint8_t)0
#define CEREG_N_ENABLE_REGSTATE                     (uint8_t)1
#define CEREG_N_ENABLE_CELL                         (uint8_t)2

#define CEREG_NOREGNOSEARCH                         (uint8_t)0
#define CEREG_REGHPLMN                              (uint8_t)1
#define CEREG_NOREGBUTSEARCH                        (uint8_t)2
#define CEREG_REGDENIED                             (uint8_t)3
#define CEREG_UNKNOWN                               (uint8_t)4
#define CEREG_REGROAMING                            (uint8_t)5

#define ATI_SRV_STATUS_NOSRV                            (uint8_t)0
#define ATI_SRV_STATUS_LIMITSRV                         (uint8_t)1
#define ATI_SRV_STATUS_SRV                              (uint8_t)2

#define ATI_SRV_DOMAIN_EPSONLY                          (uint8_t)4

#define ATI_ROAM_STATUS_NOTROAM                         (uint8_t)0
#define ATI_ROAM_STATUS_ROAM                            (uint8_t)1

/*MODE, SYSINFO act*/
#define ATI_NOSERVE                                     (uint8_t)0
#define ATI_LTE                                         (uint8_t)17

/*SUBMODE, SYSINFO*/
#define ATI_SUBMODE_LTE                                 (uint8_t)11/* LTE mode*/

#define TAC_INVALID                                 0xFFFF
#define CELLID_INVALID                              0xFFFFFFFF

#define SUBMODE_NOSERVE                             (uint8_t)0   /* no service */
#define SUBMODE_LTE_TDD                             (uint8_t)9   /* LTE TDD */
#define SUBMODE_LTE_FDD                             (uint8_t)10  /* LTE FDD*/

#define ACTIVE_STATE_DETACH                         (uint8_t)0
#define ACTIVE_STATE_ATTACH                         (uint8_t)1
#define ACTIVE_STATE_DEREGISTER                     (uint8_t)2

#define CPSMS_MODE_DIABLE                           (uint8_t)0
#define CPSMS_MODE_ENABLE                           (uint8_t)1

#define CEDRXS_MODE_DIABLE                          (uint8_t)0
#define CEDRXS_MODE_ENABLE                          (uint8_t)1

#define CMEACT_LTE_TDD_ONLY                         (uint8_t)0
#define CMEACT_LTE_FDD_ONLY                         (uint8_t)1
#define CMEACT_LTE_TDD_PREF                         (uint8_t)2
#define CMEACT_LTE_FDD_PREF                         (uint8_t)3

#define CEMODE_PSMODE2                              (uint8_t)0
#define CEMODE_CSPSMODE1                            (uint8_t)1
#define CEMODE_CSPSMODE2                            (uint8_t)2
#define CEMODE_PSMODE1                              (uint8_t)3

#define IMS_DEREGISTERED                            (uint8_t)0
#define IMS_REGISTERED                              (uint8_t)1

/* <stat> */
#define ATI_MSGSTAT_RECUNREAD                       (uint8_t)0
#define ATI_MSGSTAT_RECREAD                         (uint8_t)1
#define ATI_MSGSTAT_STOUNSENT                       (uint8_t)2
#define ATI_MSGSTAT_STOSENT_NOSTATRPT               (uint8_t)3
#define ATI_MSGSTAT_ALL                             (uint8_t)4
#define ATI_MSGSTAT_STOSENT_STATRPTNOTRCV           (uint8_t)5
#define ATI_MSGSTAT_STOSENT_STATRPTSTOOTHR          (uint8_t)6
#define ATI_MSGSTAT_STOSENT_STATRPTSTOSMSR          (uint8_t)7

/* parameter <mt> of +CNMI Command.*/
#define ATSMS_CNMIMT_NOROUTE                        (uint8_t)0
#define ATSMS_CNMIMT_STOREIND                       (uint8_t)1
#define ATSMS_CNMIMT_DIRECTROUTE                    (uint8_t)2
#define ATSMS_CNMIMT_CLASS3ROUTE                    (uint8_t)3

/* parameter <ds> of +CNMI Command.*/
#define ATSMS_CNMIDS_NOROUTE                        (uint8_t)0
#define ATSMS_CNMIDS_DIRECTROUTE                    (uint8_t)1
#define ATSMS_CNMIDS_STOREIND                       (uint8_t)2

/* parameter <avail> of +CMMENA Command.*/
#define ATI_SMS_MEM_FREE  0
#define ATI_SMS_MEM_FULL  1

/* TPDU message type ID.*/
#define ATSMS_DELIVER                           (uint8_t)0
#define ATSMS_DELIVER_RPT                       (uint8_t)0
#define ATSMS_SUBMIT                            (uint8_t)1
#define ATSMS_SUBMIT_RPT                        (uint8_t)1
#define ATSMS_STATUS_RPT                        (uint8_t)2
#define ATSMS_COMMAND                           (uint8_t)2
#define ATSMS_RESERVE                           (uint8_t)3

/* TP-VPF setting */
#define ATSMS_TPVP_NONEXIST                     (uint8_t)0
#define ATSMS_TPVP_ENHANCE                      (uint8_t)1
#define ATSMS_TPVP_RELATIVE                     (uint8_t)2
#define ATSMS_TPVP_ABSOLUTE                     (uint8_t)3


#define DEV_ATMEM_CONTXT_FIRST                  (uint8_t)0
#define DEV_ATMEM_CONTXT_SECOND                 (uint8_t)1
#define DEV_ATMEM_PDP_NOTDEFINE                 (uint8_t)0
#define DEV_ATMEM_PDP_DEFINE                    (uint8_t)1

#define DEV_ATMEM_MININUM_CID                   (uint8_t)1

#define DEV_ATMEM_MAX_CID_NUM                   (uint8_t)8
#define DEV_ATMEM_MAXINUM_CID                   (uint8_t)8
#define DEV_ATMEM_CID_MAXINDEX                  (uint8_t)7
#define DEV_ATMEM_MAX_TFADID_NUM                (uint8_t)8
#define DEV_ATMEM_MAXINUM_TFADID                (uint8_t)8
#define DEV_ATMEM_TFADID_MAXINDEX               (uint8_t)7

#define ATSM_MAX_CID_NUM                        (uint8_t)DEV_ATMEM_MAX_CID_NUM

#define ATSM_IPV4_ADDR_MAX_LEN                  (uint8_t)4
#define ATSM_IPV6_ADDR_MAX_LEN                  (uint8_t)16
#define ATSM_ADDR_MAX_LEN                       (uint8_t)ATSM_IPV6_ADDR_MAX_LEN
#define ATSM_APN_MAX_LEN                        (uint8_t)100
#define ATSM_MAX_PDPADDR_LEN                    (uint8_t)20

#define ATSM_PCO_ITEM_VAL_MAX_LEN               (uint8_t)134

#define ATSM_PCO_PROT_ITEM_MAX_NUM              (uint8_t)6
#define ATSM_PCO_PARA_ITEM_MAX_NUM              (uint8_t)13
#define ATSM_MAX_PDPADDR_MASK_LEN               (uint8_t)32
#define ATSM_MAX_FILTER_ID                      (uint8_t)16

#define DEV_ATMEM_MININUM_FILTER                (uint8_t)1
#define DEV_ATMEM_MAXINUM_FILTER                (uint8_t)16

/*GPCOAUTH <auth_type>macro*/
#define ATSM_PDP_AUTH_TYPE_NONE                 (uint8_t)0
#define ATSM_PDP_AUTH_TYPE_PAP                  (uint8_t)1
#define ATSM_PDP_AUTH_TYPE_CHAP                 (uint8_t)2
#define ATSM_PDP_AUTH_TYPE_PAPCHAP              (uint8_t)3


/*PDN TYPE*/
#define ATESM_IPV4                              (uint8_t)1
#define ATESM_IPV6                              (uint8_t)2
#define ATESM_IPV4V6                            (uint8_t)3

/* <IPv4AddrAlloc>  */
#define ATESM_IPV4ADDR_ALLOC_THROUGH_SIGNAL     (uint8_t)0
#define ATESM_IPV4ADDR_ALLOC_THROUGH_DHCP       (uint8_t)1

/* <Emergency Indication>  */
#define ATESM_PDPCONT_IS_NOT_FOR_EMER_BEARER_SERV (uint8_t)0
#define ATESM_PDPCONT_IS_FOR_EMER_BEARER_SERV     (uint8_t)1

/*<P-CSCF_discovery>*/
#define ATESM_PCSCF_NOT_INFLUENCED              (uint8_t)0
#define ATESM_PCSCF_THROUGH_NAS_SIGNAL          (uint8_t)1
#define ATESM_PCSCF_THROUGH_DHCP                (uint8_t)2

/*<IM_CN_Signalling_Flag_Ind>*/
#define ATESM_PDPCONT_IS_NOT_FOR_IMCN           (uint8_t)0
#define ATESM_PDPCONT_IS_FOR_IMCN               (uint8_t)1


#define ATSM_ATCMD_CGACT                        (uint8_t)0
#define ATSM_ATCMD_CMGACT                     (uint8_t)1
#define ATSM_ATCMD_INTER_REACT                (uint8_t)2

/* +CGEV event */
#define ATSM_CGEV_ME_PRI_PDN_ACT                (uint8_t)0
#define ATSM_CGEV_NW_PRI_PDN_DEACT              (uint8_t)1
#define ATSM_CGEV_ME_PRI_PDN_DEACT              (uint8_t)2
#define ATSM_CGEV_NW_PRI_PDN_ACT                (uint8_t)3
#define ATSM_CGEV_NW_MODIFY                     (uint8_t)4
#define ATSM_CGEV_ME_MODIFY                     (uint8_t)5
#define ATSM_CGEV_NW_SEC_PDN_ACT                (uint8_t)6
#define ATSM_CGEV_ME_SEC_PDN_ACT                (uint8_t)7
#define ATSM_CGEV_NW_SEC_PDN_DEACT              (uint8_t)8
#define ATSM_CGEV_ME_SEC_PDN_DEACT              (uint8_t)9
#define ATSM_CGEV_MT_ACT_REJ                    (uint8_t)10
#define ATSM_CGEV_NW_REACT                      (uint8_t)11

#define ATSM_CGEV_CHNG_TFT                      (uint8_t)1
#define ATSM_CGEV_CHNG_QOS                      (uint8_t)2
#define ATSM_CGEV_CHNG_TFT_AND_QOS              (uint8_t)3

#define ATSM_CGEV_EVENT_TYPE_INFO               (uint8_t)0
#define ATSM_CGEV_EVENT_TYPE_ACKREQ             (uint8_t)1

#define ATSM_CGEV_TYPE_PRI_PDN_IND              (uint8_t)0
#define ATSM_CGEV_TYPE_MODIFY_IND               (uint8_t)1
#define ATSM_CGEV_TYPE_SEC_PDN_IND              (uint8_t)2

/* +CGEV result of updating*/
#define ATESM_MOD_REASON_TFT_ONLY               (uint8_t)1
#define ATESM_MOD_REASON_QOS_ONLY               (uint8_t)2
#define ATESM_MOD_REASON_TFT_AND_QOS            (uint8_t)3

/* +CGEV default / dedicated PDP*/
#define ATESM_CGEV_PDN_CON                      (uint8_t)1
#define ATESM_CGEV_DED_BEARER                   (uint8_t)0

/* failure reason */
#define ATSM_MEM_FAIL                           (uint16_t)23
#define ATSM_PDP_HANDLE_BUSY                    (uint16_t)114
#define ATSM_PDP_HANDLE_ABORT                   (uint16_t)115
#define ATSM_PDP_ADDRESS_TYPE_UNKNOW            (uint16_t)128
#define ATSM_SRV_NOT_SPT                        (uint16_t)132
#define ATSM_SRV_NOT_SUB                        (uint16_t)133
#define ATSM_SRV_OUTOF_ORDER                    (uint16_t)134
#define ATSM_UNSPEC_GPRS_ERR                    (uint16_t)148
#define ATSM_PDP_AUTH_FAIL                      (uint16_t)149
#define ATSM_LAST_PDN_NOT_DEACT                 (uint16_t)151

#define ATI_SMS_MAX_PDU_LEN                    (uint8_t)176
#define ATI_SMS_MAX_ADDRVALUE_LEN              (uint8_t)10
#define ATIEMM_MAX_TXTSTR_NUM                  (uint8_t)218

#define ATI_USIM_REMOVE_CARD                  (uint8_t)0
#define ATI_USIM_INSERT_CARD                  (uint8_t)1

#define ATI_USIM_APDU_REQ_MAX_LEN             (uint16_t)262
#define ATI_USIM_APDU_CNF_MAX_LEN             (uint16_t)258

#define ATI_USIM_MAX_DIRAID_LEN               (uint8_t)16
#define ATI_MAX_CALLNAME_LEN                  (uint8_t)241


/* <mode> value for +CLCK */
#define ATI_USIM_FAC_DISABLE_PIN              (uint8_t)0
#define ATI_USIM_FAC_ENABLE_PIN               (uint8_t)1
#define ATI_USIM_FAC_QUERY_PIN                (uint8_t)2

/* pin eanbled or disabled ,reference 27007*/
#define ATI_USIM_PIN_NOTACTIVE                (uint8_t)0
#define ATI_USIM_PIN_ACTIVE                   (uint8_t)1

/*pin state on card,reference 27007*/
#define ATI_USIM_PIN_READY                    (uint8_t)0
#define ATI_USIM_PIN_SIMPIN                   (uint8_t)1
#define ATI_USIM_PIN_SIMPUK                   (uint8_t)2

/*AT command CRSM*/
#define ATI_USIM_MAX_PATH_LEN                 (uint8_t)8

#define ATI_USIM_CMD_OK                       (uint8_t)30
#define ATI_USIM_ELEMENTARY_INIT_OK           (uint8_t)31

#define ATI_USIM_CMD_TYPE_QUERY               (uint8_t)0     /* Query Command */
#define ATI_USIM_CMD_TYPE_SET                 (uint8_t)1     /* Set Command */

#define ATI_USIM_PIN_NUM                      (uint8_t)8     /* PIN MAX byte length */
#define ATI_USIM_PUK_NUM                      (uint8_t)8     /* PUK max byte length */

#define ATI_USIM_CARD_STATUS_INVALID          (uint8_t)0
#define ATI_USIM_CARD_STATUS_VALID            (uint8_t)1
#define ATI_USIM_CARD_STATUS_NOCARD_NEEDPIN   (uint8_t)255

#define CSCON_DISABLE_URC                     (uint8_t)0
#define CSCON_ENABLE_URC                      (uint8_t)1

/* operation type */
#define ATI_SMSCMDTYPE_SET                     (uint8_t)0
#define ATI_SMSCMDTYPE_QUERY                   (uint8_t)1
#define ATI_SMSCMDTYPE_TEST                    (uint8_t)2

/* <delflag> of +CMGD Command */
#define ATI_SMSDELFLG_BYIDX                    (uint8_t)0
#define ATI_SMSDELFLG_READ                     (uint8_t)1
#define ATI_SMSDELFLG_READSEND                 (uint8_t)2
#define ATI_SMSDELFLG_READSENDUNSEND           (uint8_t)3
#define ATI_SMSDELFLG_ALL                      (uint8_t)4

#define ATI_OPERATION_FAILURE                  (uint8_t)0
#define ATI_OPERATION_SUCCESS                  (uint8_t)1

#define VERSION_NUMBER_MAX_LEN                 (uint8_t)40

#define ATI_URC_CHANNEL                        (uint8_t)255

#define ATI_USIM_EID_LEN                       (uint8_t)10

//+CMIMSBAR 参数<mode>
#define ATI_CM_IMS_BAR_RESERVED  (uint8_t)0
#define ATI_CM_IMS_BAR_VOCIE     (uint8_t)1
#define ATI_CM_IMS_BAR_VIDEO     (uint8_t)2
#define ATI_CM_IMS_BAR_SMS       (uint8_t)3
#define ATI_CM_IMS_BAR_EMERGENCY (uint8_t)4

#define ATI_MAC_STR_MAX_LEN                    (uint8_t)17


//bAtMode
#define ATI_CMD_MODE_READ   (uint8_t)0   //查询
#define ATI_CMD_MODE_SET    (uint8_t)1   //设置
#define ATI_CMD_MODE_TEST   (uint8_t)2   //测试
#define ATI_CMD_MODE_ACT    (uint8_t)3   //执行

/***************+CMEPCG***************/
#define EPCG_IND_CMD    25
#define NORMAL_SRV      0
#define LIMIT_SRV       1
#define NO_SRV          2

#define EPCG_PARAM_NUMBER      (uint8_t)25

#define  AP_AT_EPCG_SET        (uint8_t)0
#define  AP_AT_EPCG_QRY        (uint8_t)1

/* parameter bAckMode define .*/
#define ATISMS_CNMA_RPACK                        (uint8_t)0
#define ATISMS_CNMA_RPERROR                      (uint8_t)1


#define ATI_NAS_MAX_ICCID_LEN       (uint8_t)10
#define MAX_IMSI_LEN                (uint8_t)16
#define ATI_MAX_NAME_LEN            (uint8_t)64

#define NAS_MAX_UPLMN_NUM           30
#define NAS_MAX_OPLMN_NUM           30
#define NAS_MAX_HPLMN_NUM           30
#define NAS_MAX_PLMN_NUM            30 /* max number of OPLMN/UPLMN/HPLMN in UICC*/
#define NAS_MAX_HPLMNWACT_NUM       30

#define ATI_ADDPARAID_IPV6DNS                              (uint8_t)0x03
#define ATI_PROTOCOLID_IPCP                                (uint16_t)0x8021  /*IPCP Protocol ID*/

#define ATI_PRIMDNSSER_ADDR                                (uint8_t)0x81
#define ATI_SECMDNSSER_ADDR                                (uint8_t)0x83

#define L3_MAX_PCO_CONTENT_LEN                  (uint8_t)251

#define ATI_SM_MAX_APN_LEN                              (uint8_t)100
#define ATI_SM_IPV4_ADDR_MAX_LEN                        (uint8_t)4
#define ATI_SM_IPV6_ADDR_MAX_LEN                        (uint8_t)16
#define ATI_SM_IPV4V6_ADDR_MAX_LEN                      (uint8_t)(ATI_SM_IPV4_ADDR_MAX_LEN + ATI_SM_IPV6_ADDR_MAX_LEN)

/*<P-CSCF_discovery>*/
#define ATI_PREFER_PCSCF_NOT_INFLUENCED_BY_CGDCONT      (uint8_t)0
#define ATI_PREFER_PCSCF_THROUGH_NAS_SIGNAL             (uint8_t)1
#define ATI_PREFER_PCSCF_THROUGH_DHCP                   (uint8_t)2

/*<IM_CN_Signalling_Flag_Ind>*/
#define ATI_IPDPCONT_IS_NOT_FOR_IMCN                     (uint8_t)0
#define ATI_IPDPCONT_IS_FOR_IMCN                         (uint8_t)1

/* <IPv4AddrAlloc>  */
#define ATI_IPV4ADDR_ALLOC_THROUGH_SIGNAL               (uint8_t)0
#define ATI_IPV4ADDR_ALLOC_THROUGH_DHCP                 (uint8_t)1

/* <Emergency Indication>  */
#define ATI_IPDPCONT_IS_NOT_FOR_EMER_BEARER_SERV         (uint8_t)0
#define ATI_IPDPCONT_IS_FOR_EMER_BEARER_SERV             (uint8_t)1

/* Data compression（T_PS_APAT_CidPdpContext中bDComp） */
#define ATI_SM_DATACOMPRESS_OFF                         (uint8_t)0
#define ATI_SM_DATACOMPRESS_ON                          (uint8_t)1
#define ATI_SM_DATACOMPRESS_V42                         (uint8_t)2
#define ATI_SM_DATACOMPRESS_V44                         (uint8_t)3

/* Header compression（T_PS_APAT_CidPdpContext中bHComp） */
#define ATI_SM_HEADERCOMPRESS_OFF                       (uint8_t)0
#define ATI_SM_HEADERCOMPRESS_ON                        (uint8_t)1
#define ATI_SM_HEADERCOMPRESS_RFC1144                   (uint8_t)2
#define ATI_SM_HEADERCOMPRESS_RFC2507                   (uint8_t)3
#define ATI_SM_HEADERCOMPRESS_RFC3095                   (uint8_t)4

#define ATI_PDP_NOTDEFINE                               (uint8_t)0
#define ATI_PDP_DEFINE                                  (uint8_t)1

#define ATI_OPER_NUMTOLONG                              (uint8_t)0
#define ATI_OPER_NUMTOSHORT                             (uint8_t)1

#define ATI_CONTXT_FIRST                                (uint8_t)0
#define ATI_CONTXT_SECND                                (uint8_t)1


#define ATI_EMM_ABORT_TYPE_PLMNLIST                     (uint8_t)0
#define ATI_EMM_ABORT_TYPE_NCELLLIST                    (uint8_t)1

#define ATI_QUERY_CEREG                                 (uint8_t)0
#define ATI_QUERY_CGREG                                 (uint8_t)1
#define ATI_QUERY_CREG                                  (uint8_t)2
#define ATI_QUERY_NULL                                  (uint8_t)0xff

/*--------------------------------------------------------------LINE------------------------------------------------------------------------*/
typedef struct {
    uint8_t                            abId[MAX_IMSI_LEN]; /* Each uint8_t contain one BCD*/
    uint8_t                            bNum;           /*BCD count*/
}S_IMSI;


/*PlmnId*/
/* @NV-PS {
            "name": "网络ID",
            "description": "网络ID"
   }
*/
/* 客户可见可修改 */
typedef struct {
    /* 客户可见可修改 */
    /* @NV-ITEM {
                "name": "MCC1",
                "range": "0-9,15",
                "description": "MCC1,0xF表示无效"
       }
    */
    BITS                            bMCC1     :4;

    /* 客户可见可修改 */
    /* @NV-ITEM {
                "name": "MCC2",
                "range": "0-9,15",
                "description": "MCC2,0xF表示无效"
       }
    */
    BITS                            bMCC2     :4;

    /* 客户可见可修改 */
    /* @NV-ITEM {
                "name": "MCC3",
                "range": "0-9,15",
                "description": "MCC3,0xF表示无效"
       }
    */
    BITS                            bMCC3     :4;

    /* 客户可见可修改 */
    /* @NV-ITEM {
                "name": "MNC3",
                "range": "0-9,15",
                "description": "MNC3,0xF表示无效"
       }
    */
    BITS                            bMNC3     :4;

    /* 客户可见可修改 */
    /* @NV-ITEM {
                "name": "MNC1",
                "range": "0-9,15",
                "description": "MNC1,0xF表示无效"
       }
    */
    BITS                            bMNC1     :4;

    /* 客户可见可修改 */
    /* @NV-ITEM {
                "name": "MNC2",
                "range": "0-9,15",
                "description": "MNC2,0xF表示无效"
       }
    */
    BITS                            bMNC2     :4;
}S_PS_PlmnId;

typedef struct {
    uint8_t                        bUtran     :1;
    uint8_t                        bGsm       :1;
    uint8_t                        bGsmCompact:1;
    uint8_t                        bEutran    :1;
    uint8_t                        bSpare     :4;
}S_Usim_Act;

typedef struct {
    uint8_t                        bPosId;
    S_PS_PlmnId                    tPlmnId;
    S_Usim_Act                     tAct;
} S_Usim_Plmn_Act;

typedef struct {
    uint8_t                        bMaxCnt;   /* 卡中最大能存的UPLMN个数, 不可修改, 0表示卡里没有UPLMN */
    uint8_t                        bValidCnt; /* 卡中目前实际有效的UPLMN个数 */
    S_Usim_Plmn_Act                atUplmn[NAS_MAX_UPLMN_NUM];
} S_Usim_UPLMN;

typedef struct {
    uint8_t                        bMaxCnt;   /* 卡中最大能存的OPLMN个数, 不可修改, 0表示卡里没有OPLMN */
    uint8_t                        bValidCnt; /* 卡中目前实际有效的OPLMN个数 */
    S_Usim_Plmn_Act                atOplmn[NAS_MAX_OPLMN_NUM];
} S_Usim_OPLMN;

typedef struct {
    uint8_t                        bMaxCnt;   /* 卡中最大能存的PLMN个数, 不可修改, 0表示卡里没有PLMN */
    uint8_t                        bValidCnt; /* 卡中目前实际有效的PLMN个数 */
    S_Usim_Plmn_Act                atPlmn[NAS_MAX_PLMN_NUM];
} S_AtiUsim_CpolPlmn;

/*==============================================================================
 Primitive:   ATI_USIM_CRSM_REQ (ATI->USIM)
 Description: +CRSM restiction usim access
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bFidFg;       /* indicate <fid> valid*/
    uint8_t    bParamFg;     /* indicate <P1> <P2> <P3> valid */

    uint8_t    bCmd;         /* <cmd> */
    uint8_t    bP1;          /* <p1> */
    uint8_t    abFid[2];     /* <fid> */

    uint8_t    bP2;          /* <p2> */
    uint8_t    bP3;          /* <p3> */
    uint8_t    bDataLen;     /* <data> */
    uint8_t    bPathLen;     /* <pathid> */

    uint8_t   *pbCrsmData;  /* <data> */
    uint8_t    bChId;       /* +CRSM logical channel, VoLte use */

    uint8_t    abPath[ATI_USIM_MAX_PATH_LEN];   /* <pathid> */
} S_AtiUsim_Crsm_Req;

/*==============================================================================
 Primitive:   ATI_USIM_CRSM_CNF (USIM->ATI)
 Description: ATI_USIM_CRSM_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bSW1;
    uint8_t bSW2;
    uint16_t wRspLen;
    uint8_t *pbRspData;
    uint8_t bChId;
}S_AtiUsim_CrsmCnf;

/*==============================================================================
 Primitive:   ATI_USIM_CSIM_REQ (ATI->USIM)
 Description: +CSIM usim access
==============================================================================*/
typedef struct
{
    uint8_t   bChannelId;

    uint8_t   bSeq;        /* <seq> not protocol param*/
    uint8_t   bSeqFg;      /* indicate <seq> valid*/

    uint16_t  wLength;     /* APDU len */
    uint8_t   *pbCsimApdu;  /* APDU byte code */
} S_AtiUsim_Csim_Req;

/*==============================================================================
 Primitive:   ATI_USIM_CSIM_CNF (USIM->ATI)
 Description: ATI_USIM_CSIM_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bSeqFg;
    uint8_t bSeq;
    uint16_t wRspLen;
    uint8_t *pbRspData;
}S_AtiUsim_CsimCnf;

/*==============================================================================
 Primitive:   ATI_USIM_CCHO_REQ (ATI->USIM)
 Description: +CCHO open one logical usim channel
==============================================================================*/
typedef struct{
    uint8_t    bChannelId;
    uint8_t    bAidLen;
    uint8_t    abAid[ATI_USIM_MAX_DIRAID_LEN];
} S_AtiUsim_Ccho_Req;


/*==============================================================================
 Primitive:   ATI_USIM_CCHO_CNF (USIM->ATI)
 Description: ATI_USIM_CCHO_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bSessionID;
}S_AtiUsim_CchoCnf;

/*==============================================================================
 Primitive:   ATI_USIM_CCHC_REQ (ATI->USIM)
 Description: +CCHC close one logical usim channel
==============================================================================*/
typedef struct{
    uint8_t    bChannelId;
    uint8_t    bSessionID;  /*  channel ID */
} S_AtiUsim_Cchc_Req;


/*==============================================================================
 Primitive:   ATI_USIM_CGLA_REQ (ATI->USIM)
 Description: +CGLA access usim specific logical channel
==============================================================================*/
typedef struct{
    uint8_t    bChannelId;
    uint8_t    bSessionID;  /* logical ID */

    uint16_t    wLength;    /* APDU len, max ATI_USIM_APDU_REQ_MAX_LEN*/
    uint8_t    *pbCglaApdu;/* APDU byte code */
} S_AtiUsim_Cgla_Req;


/*==============================================================================
 Primitive:   ATI_USIM_CGLA_CNF (USIM->ATI)
 Description: ATI_USIM_CGLA_REQ Response
==============================================================================*/
typedef struct {
    uint16_t wRspLen;
    uint8_t *pbRspData;
}S_AtiUsim_CglaCnf;

/*==============================================================================
 Primitive:   ATI_USIM_CPIN_REQ (ATI->USIM)
 Description: +CPIN or UNBLOCK PIN
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bAtMode;   /* AT 模式 0: 查询模式， 1：设置模式*/
    uint8_t    bNewPinFlg;  /* indicate <newpin> valid */
    uint8_t    bPinNum;
    uint8_t    abPin[ATI_USIM_PIN_NUM];   /* <pin> */
    uint8_t    abNewPin[ATI_USIM_PIN_NUM];/* <newpin> */
} S_AtiUsim_Cpin_Req;

typedef struct {
    uint8_t    bPinStatus;
}S_AtiUsim_CpinQueryCnf;

/*==============================================================================
 Primitive:   ATI_USIM_CPWD_REQ (ATI->USIM)
 Description: +CPWD modify pin
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bFac;          /*ATI_FACVALUE_SC, ATI_FACVALUE_P2*/
    uint8_t    bChId;
    uint8_t    abOldPwd[ATI_USIM_PIN_NUM];/* <oldpwd> */
    uint8_t    abNewPwd[ATI_USIM_PIN_NUM];/* <newpwd> */
} S_AtiUsim_Cpwd_Req;


/*==============================================================================
 Primitive:   ATI_USIM_CLCK_REQ (ATI->USIM)
 Description: +CLCK enable/disable pin
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bFac;        /* <fac>: ATI_FACVALUE_SC, ATI_FACVALUE_P2*/
    uint8_t    bMode;       /* <mode>: ATI_USIM_FAC_DISABLE_PIN, ATI_USIM_FAC_ENABLE_PIN, ATI_USIM_FAC_QUERY_PIN */
    uint8_t    abPasswd[ATI_USIM_PIN_NUM];/* <passwd>: PIN 码 */
} S_AtiUsim_Clck_Req;

/*==============================================================================
 Primitive:   ATI_USIM_CLCK_CNF (USIM->ATI)
 Description: ATI_USIM_CLCK_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bIsPinEnabled;
}S_AtiUsim_ClckCnf;

/*==============================================================================
 Primitive:   ATI_USIM_CMRAP_REQ (ATI->USIM)
 Description: +CMRAP query pin/puk remain try count
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
} S_AtiUsim_Cmrap_Req;

/*==============================================================================
 Primitive:   ATI_USIM_CMRAP_CNF (USIM->ATI)
 Description: ATI_USIM_CMRAP_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bPin1Remain;
    uint8_t bPin2Remain;
    uint8_t bPuk1Remain;
    uint8_t bPuk2Remain;
}S_AtiUsim_CmrapCnf;

/*==============================================================================
 Primitive:   ATI_USIM_CEID_REQ (ATI->USIM)
 Description: +CEID query EID
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
} S_AtiUsim_Ceid_Req;

/*==============================================================================
 Primitive:   ATI_USIM_CEID_CNF (USIM->ATI)
 Description: ATI_USIM_CEID_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bEidLen;
    uint8_t *pbEid;
}S_AtiUsim_CeidCnf;

/*==============================================================================
 Primitive:   ATI_USIM_CNUM_REQ (ATI->USIM)
 Description: +CNUM query MSISDN
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
} S_AtiUsim_Cnum_Req;

#define MSISDN_MAX_NUM_COUNT        (uint8_t)10
#define CALL_ID_MAX_NUM             (uint8_t)40

typedef struct {
    uint8_t                         bNumType;
    uint8_t                         bNumLen;
    uint8_t                         abNum[CALL_ID_MAX_NUM];
}S_CallNum;

typedef struct {
    uint8_t                         bChannelId;
    uint8_t                         bCnumCount;
    S_CallNum                       atCnum[MSISDN_MAX_NUM_COUNT];
} S_AtiUsim_Cnum_Rsp;


/*==============================================================================
 Primitive:   ATI_USIM_CMUINIT_REQ (ATI->USIM)
 Description: usim init
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
} S_AtiUsim_Cmuinit_Req;

typedef struct {
    uint8_t    bChannelId;
    uint32_t   dwUiccState;
    uint8_t    abIccId[ATI_NAS_MAX_ICCID_LEN]; /*EF iccid*/
} S_AtiUsim_Cmuinit_Rsp;

/*==============================================================================
 Primitive:   ATI_USIM_CMSLOT_REQ (ATI->USIM)
 Description: Set Slot
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bAtMode; // 0:read 1:set
    uint8_t    bSlot;
} S_AtiUsim_Cmslot_Req;

// use for read success
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bSlot;
} S_AtiUsim_Cmslot_Rsp;

/*==============================================================================
 Primitive:   ATI_USIM_USIM_POWER_OFF_REQ (ATI->USIM)
 Description: Power down Usim
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
} S_AtiUsim_PowerDown_Req;


/*==============================================================================
 Primitive:   ATI_USIM_UPDATE_ITEM_REQ (ATI->USIM)
 Description: UPDATE USIM files, smsp,uplmn
==============================================================================*/
typedef struct {
    uint8_t   bChannelId;
    uint16_t  wFid;
    uint16_t  wDataLen;   /* data byte len */
    uint8_t   *pbData;     /* data byte code*/
}S_AtiUsim_Update_Item_Req;


/*==============================================================================
 Primitive:   ATI_USIM_CUSATT_REQ (ATI->USIM)
 Description: send TERMINAL RESPONSE command request to usim
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bDataLen;
    uint8_t   *pbTermRsp;
} S_AtiUsim_Cusatt_Req;


/*==============================================================================
 Primitive:   ATI_USIM_CUSATE_REQ (ATI->USIM)
 Description: sen ENVELOPE command request to usim
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bDataLen;
    uint8_t   *pbEnvelope;
} S_AtiUsim_Cusate_Req;


/*==============================================================================
 Primitive:   NONE
 Description: +CUSATE response ENVELOPE to atc from usim
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bBusyFlag;    /* indicate <busy> valid */
    uint8_t    bBusy;        /* <busy> 0,1 */
    uint8_t    bSw1;         /* <sw1>  */
    uint8_t    bSw2;         /* <sw2> */
    uint8_t    bEnvRspLen;   /* <envelope response> */
    uint8_t   *pbEnvRsp;     /* <envelope response> */
} S_AtiUsim_Cusate_Cnf;


/*==============================================================================
 Primitive:   ATI_USIM_SLOT_IND (ATI->USIM)
 Description:  +CMUSLOT usim report usim card removed or inserted
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;    /*source index*/
    uint8_t    bCardState;    /*card status ATI_USIM_REMOVE_CARD  ATI_USIM_INSERT_CARD*/
}S_AtiUsim_Card_Act_Ind;


/*dedicated PDP context type*/
typedef struct
{
    uint8_t    bCid;               /*  SET command must be */
    uint8_t    abPadding[2];

    uint8_t    bPrimaryCidFg;
    uint8_t    bPrimaryCid;        /* primary PDP contextcid */
    uint8_t    bDComp;             /* same as S_Ati_CidPdpContext */
    uint8_t    bHComp;
    uint8_t    bImcnSignInd;
}S_AtiEsm_SecPdpContext;


typedef struct
{
    uint8_t    bPdpType;
    uint8_t    abIPv4Addr[ATSM_IPV4_ADDR_MAX_LEN];
    uint8_t    abIPv6Addr[ATSM_IPV6_ADDR_MAX_LEN];
}S_AtiEsm_PdpAddr;


/* PCO info */
typedef struct
{
    uint16_t wId;        /*ProtocolId or ContainerId*/
    uint8_t  bLen;
    uint8_t *pContent;   /*ProtocolId Contents or ContainerId contents*/
}S_AtiEsm_Container;


typedef struct
{
    uint8_t               bProtNum;
    uint8_t               bAddParaNum;
    S_AtiEsm_Container    atProtList[ATSM_PCO_PROT_ITEM_MAX_NUM];
    S_AtiEsm_Container    atAddParaList[ATSM_PCO_PARA_ITEM_MAX_NUM];
}S_Esm_PcoInfo;


typedef struct
{
    uint8_t          bCid;
    S_Esm_PcoInfo    *ptPco;
}S_AtiEsm_CidPcoInfo;



typedef struct
{
    uint8_t    bFilterId;          /* 1..8。 0 invalid */
    uint8_t    bPreIndex;          /* priority of filter */
    uint8_t    bPadding;
    BITS       bIPvFg:1;           /* 0=IPv4,1=IPv6 */
    BITS       bPidNextHeaderFg:1;
    BITS       bTosTcMaskFg:1;
    BITS       bAddrMaskFg:1;
    BITS       bLocalPortFg:1;
    BITS       bRemotePortFg:1;
    BITS       bSpiFg:1;
    BITS       bFlowLabelFg:1;

    uint8_t    bPidNextHeader;     /* IPv4 protocol id/IPv6 next header */
    uint8_t    bTosTc;             /* type of service (tos) (ipv4)/ traffic class (ipv6)
                                    bTosTcMaskFg=1 valid*/
    uint8_t    bMask;              /* mask(ipv4/ipv6), bTosTcMaskFg=1 valid*/
    uint8_t    bDirection;

    uint8_t    abAddrMask[ATSM_MAX_PDPADDR_MASK_LEN];  /* addr and mask，IPv4 first 8 bytes valid*/

    uint16_t   wFLocalPort;        /*first Local port，0..65535, bLocalPortFg=1 valid*/
    uint16_t   wTLocalPort;        /*term Local port，0..65535。if   wFLocalPort==wTLocalPort，
                                    means single port. bLocalPortFg=1 valid*/

    uint16_t   wFRemotePort;       /*first Remote port，0..65535,bRemotePortFg=1 valid */
    uint16_t   wTRemotePort;       /*term Remote port， 0..65535, if wFRemotePort==wTRemotePort，
                                    means single port. bRemotePortFg=1 valid*/

    uint32_t   dwSpi;              /* security parameter index */
    uint32_t   dwFlowLabel;        /* IPv6 flow label */
}S_Dev_AtMem_FilterType;


typedef struct
{
    BITS       bSpare:5;
    BITS       bQciFlg:1;
    BITS       bUlDlGbrFlg:1;
    BITS       bUlDlMbrFlg:1;
    uint8_t    bQci;               /*QCI of EPS QoS*/
    uint32_t   dwUlGbr;           /*Uplink GBR in EPS QoS unit kbps*/
    uint32_t   dwDlGbr;           /*Downlink GBR in EPS QoS unit kbps*/

    uint32_t   dwUlMbr;           /* Uplink MBR in EPS QoS unit kbps*/
    uint32_t   dwDlMbr;           /*Downlink MBR in EPS QoS unit kbps*/
}S_AtiEsm_QosInfo;


/*==============================================================================
 Primitive:   ATI_EMM_RF_CTRL_REQ ( ATI-> EMM )
 Description: user initiate CFUN
==============================================================================*/
typedef struct {
    uint8_t     bChannelId;
    uint8_t     bOper;    /* Refer CFUN_OPER_POWROFF*/
    uint8_t     bFastFlg; /* fast poweroff flag when shutdown*/
#ifdef CFW_API_SUPPORT
    uint8_t     Plmn[3];   // for   __LOCATION_LOCK__
    uint8_t     manualMode;/*manual sel mode set TRUE,else set FALSE*/
    uint8_t     vsimFlag; //填0
#endif
}S_AtiEmm_RFControl_Req;


/*==============================================================================
 Primitive:   ATI_EMM_SEARCH_PLMN_REQ ( ATI-> EMM )
 Description: user initiate searching plmn
==============================================================================*/
typedef struct {
    uint8_t        bChannelId;
    uint8_t        bMode;        /* 0: auto mode    1:manual mode ,Refer COPS_MODE_AUTO*/
    uint8_t        bFailToAuto;
    S_PS_PlmnId    tPlmnId;
    uint8_t        bFormat;     /* Refer COPS_OPERFORMAT_LONGALPHA etc.*/
    uint8_t        bNameIndxStart; /* Valid if format is alpha. Start index in NV array */
    uint8_t        bNameIndxEnd;   /* Valid if format is alpha. End index in NV array */
}S_AtiEmm_SearchPlmn_Req;



/*==============================================================================
 Primitive:   ATI_EMM_CEREG_QUERY_REQ ( ATI-> EMM )
 Description: user QUERY cereg Parameter
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bCmd;      //0:cereg 1:cgreg 2:creg
}S_AtiEmm_CeregQuery_Req;

/*==============================================================================
 Primitive:   ATI_EMM_CEREG_QUERY_CNF or ATI_EMM_CEREG_IND ( EMM-> ATI )
 Description: ATI_EMM_CEREG_QUERY_REQ Response or Report
==============================================================================*/
typedef struct {
    uint8_t bCmd;
    uint8_t bState;
    uint8_t bSubAct;
    uint16_t wTac;
    uint32_t dwCell;
}S_AtiEmm_CeregQueryCnf,S_AtiEmm_CeregInd;

/*==============================================================================
 Primitive:   ATI_EMM_LIST_PLMN_REQ ( ATI-> EMM )
 Description: user initiate list plmn
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bNcellFg;         /* 0: indicate +COPS=? 1: indicate +CMNCELL */
}S_AtiEmm_ListPlmn_Req;


typedef struct {
    uint8_t    bChannelId;
    uint8_t    bAbortType;   /* 0:plmn list  1: ncell list */
}S_AtiPs_AbortReq;
/*==============================================================================
 Primitive:   ATI_EMM_LIST_PLMN_CNF ( ENM-> ATI )
 Description: ATI_EMM_LIST_PLMN_REQ Response
==============================================================================*/
typedef struct {
    S_PS_PlmnId tPlmnList;
    uint8_t   bStateList;
}S_AtiEmm_ListPlmnInfo;

typedef struct {
    uint8_t    bPlmnNum;
    S_AtiEmm_ListPlmnInfo *ptList;
}S_AtiEmm_ListPlmnCnf;

/*==============================================================================
 Primitive:   ATI_EMM_LIST_PLMN_CMNCELL_CNF ( ENM-> ATI )
 Description: ATI_EMM_LIST_PLMN_REQ Response +CMNCELL
==============================================================================*/
typedef struct
{
    uint32_t          dwEarfcn;
    uint16_t          wPci;
    uint32_t          dwCellId;
    uint16_t          wCellTac;
    S_PS_PlmnId       tPlmnId;
    int8_t            cQ_RxLevMin;
    uint8_t           bRsrp;
    uint8_t           bRsrq;
    uint8_t           bandwidth;
}S_AtiPs_NeighCellInfo;

typedef struct {
    uint8_t bNcellNum;
    S_AtiPs_NeighCellInfo *ptNcellInfo;
}S_AtiEmm_CmncellCnf;

/*==============================================================================
 Primitive:   ATI_EMM_ACTIVE_REQ ( ATI-> ENM )
 Description: user initiate CGATT, cops=2
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bState;     /* Refer ACTIVE_STATE_DETACH */
}S_AtiEmm_Active_Req;


/*==============================================================================
 Primitive:   ATI_EMM_CEMODE_REQ ( ATI-> EMM )
 Description: user initiate CEMODE
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bMode;    /* Refer CEMODE_PSMODE2 */
}S_AtEmm_Cemode_Req;




/* TP-FO data struct */
typedef struct
{
    BITS    bMsgTypeId:2;
    BITS    bMsgFeature:1;
    BITS    bTpvpFormat:2;
    BITS    bStatRptFeat:1;
    BITS    bTpudHeadInd:1;
    BITS    bReplyPathInd:1;
}S_AtiSms_TpFo;

/*  address type */
typedef struct
{
    uint8_t    bAddrLength;
    uint8_t    bAddrType; /* 1+TON(3bits)+NPI(low 4bits)*/
    uint8_t    abAddrValue[ATI_SMS_MAX_ADDRVALUE_LEN];
}S_AtiSms_TpAddr;

/* VP */
typedef union {
    uint8_t    abAbsTime[7];    /*Absolute  or Enhanced format*/
    uint8_t    bRelTime;        /* Relative format*/
}S_AtiSms_TpVp;

/*==============================================================================
 Primitive:   ATI_SMS_CNMI_SET_REQ ( ATI-> SMS )
 Description:   +CNMI set
==============================================================================*/
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bModeFg;
    uint8_t    bMode;
    uint8_t    bMtFg;
    uint8_t    bMt;
    uint8_t    bBmFg;
    uint8_t    bBm;
    uint8_t    bDsFg;
    uint8_t    bDs;
    uint8_t    bBfrFg;
    uint8_t    bBfr;
}S_AtiSms_CnmiSetReq;


/*==============================================================================
 Primitive:   NONE
 Description:  ATI query cnmi parameter from SMS by function
==============================================================================*/
typedef struct
{
    uint8_t    bMode;
    uint8_t    bMt;
    uint8_t    bBm;
    uint8_t    bDs;
    uint8_t    bBfr;
}S_AtiSms_CnmiQueryReq;

/*==============================================================================
 Primitive:   ATI_SMS_CMGS_PDU_SEND_REQ ( ATI-> SMS )
 Description:   +CMGS excecute send PDU
==============================================================================*/
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bMmsVal;
    uint8_t    bPduLen;             /* PDU length (0-176B).*/
    uint8_t    *pbPduBuf;
}S_AtiSms_CmgsSendReq;

/*==============================================================================
 Primitive:   ATI_SMS_CMGS_PDU_SEND_CNF ( SMS-> ATI )
 Description: ATI_SMS_CMGS_PDU_SEND_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bResult;
    uint8_t bTpMsgRef;
    uint32_t dwCause;
}S_AtiSms_CmgsPduSendCnf;

/*==============================================================================
 Primitive:   ATI_SMS_CMSS_SEND_REQ ( ATI-> SMS )
 Description:   +CMSS excecute send sms from usim
==============================================================================*/
typedef struct
{
    uint8_t            bChannelId;
    uint16_t           wIndex;
    S_AtiSms_TpAddr    tTpDestAddr;
}S_AtiSms_CmssSendReq;

/*==============================================================================
 Primitive:   ATI_SMS_CMSS_SEND_CNF ( SMS-> ATI )
 Description: ATI_SMS_CMSS_SEND_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bResult;
    uint8_t bTpMsgRef;
    uint32_t dwCause;
}S_AtiSms_CmssSendCnf;

/*==============================================================================
 Primitive:   ATI_SMS_CSMP_SET_REQ ( ATI-> SMS )
 Description:  +CSMP set text mode parameters
==============================================================================*/
typedef struct
{
    uint8_t            bChannelId;
    uint8_t            bFoFlg;
    S_AtiSms_TpFo      tFo;     /*TPDU, SET message type and other parameters
                                 * 7     6        5        4   3    2      1   0
                                 * RP  UDHI  SRI/SRR/SRQ VPF VPF MMS/RD  MTI MTI
                                 */
    uint8_t            bVpFlg;
    S_AtiSms_TpVp      tVp;     /* type and length due to TP-FO*/
    uint8_t            bPidFlg;
    uint8_t            bPid;    /* TPDU TP-PID.*/
    uint8_t            bDcsFlg;
    uint8_t            bDcs;    /* TPDU TP-DCS.*/
}S_AtiSms_CsmpSetReq;

/*==============================================================================
 Primitive:   NONE
 Description: ATI query csmp parameter from SMS by function
==============================================================================*/
typedef struct
{
    S_AtiSms_TpFo      tFo;
    S_AtiSms_TpVp      tVp;
    uint8_t            bPid;
    uint8_t            bDcs;
}S_AtiSms_CsmpQueryReq;

/*==============================================================================
 Primitive:   ATI_SMS_CPMS_REQ ( ATI-> SMS )
 Description: +CPMS set SMS storage memory
==============================================================================*/
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bCommandInd; /* ATI_SMSCMDTYPE_SET, ATI_SMSCMDTYPE_QUERY*/
    uint8_t    bMem1;       /* only support ATI_CPMSMEM_SM,  ATI_CPMSMEM_SR */
    uint8_t    bMem2OperFg;
    uint8_t    bMem2;       /* only support ATI_CPMSMEM_SM */
    uint8_t    bMem3OperFg;
    uint8_t    bMem3;       /* only support ATI_CPMSMEM_SM */
}S_AtiSms_CpmsReq;

/*==============================================================================
 Primitive:   NONE
 Description: USIM response  cpsms parameter by function
==============================================================================*/
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bResult;     /* ATI_OPERATION_SUCCESS, ATI_OPERATION_FAILURE */
    uint16_t   wReason;     /* failure cause  */
    uint8_t    bCommandInd; /* ATI_SMSCMDTYPE_SET, ATI_SMSCMDTYPE_QUERY */
    uint8_t    bMem1;       /* <mem1> */
    uint8_t    bMem2;       /* <mem2> */
    uint8_t    bMem3;       /* <mem3> */
    uint16_t   wMem1Used;   /* <mem1> used record count */
    uint16_t   wMem1Total;  /* <mem1> total record count */
    uint16_t   wMem2Used;   /* <mem2> used record count */
    uint16_t   wMem2Total;  /* <mem2> total record count */
    uint16_t   wMem3Used;   /* <mem3> used record count */
    uint16_t   wMem3Total;  /* <mem3> total record count */
}S_AtiSms_CpmsCnf;


/*==============================================================================
 Primitive:   ATI_SMS_CMGL_REQ ( ATI-> SMS )
 Description: +CMGL request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bStat;         /* +CMGL <stat> */
}S_AtiSms_CmglReq;

/*==============================================================================
 Primitive:   ATI_SMS_CMGL_PDU_ITEM_IND ( SMS-> ATI )
 Description: ATI_SMS_CMGL_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bStat;
    uint8_t bRecId;
    uint8_t bTpduLen;
    uint8_t bPduLen;
    uint8_t *pbPduData;
}S_AtiSms_CmglPduItemInd;


/*==============================================================================
 Primitive:   ATI_SMS_CMGD_REQ ( ATI-> SMS )
 Description: +CMGD request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bCmdType;   /* ATI_SMSCMDTYPE_SET, ATI_SMSCMDTYPE_TEST */
    uint8_t    bDelFlag;   /* +CMGD<delflag> val:
                            *APAT_SMSDELFLG_BYIDX            0 (or omitted)
                            *APAT_SMSDELFLG_READ             1
                            *APAT_SMSDELFLG_READSEND         2
                            *APAT_SMSDELFLG_READSENDUNSEND   3
                            *APAT_SMSDELFLG_All              4  */
    uint16_t    wIndex;     /* +CMGD<index> */
}S_AtiSms_CmgdReq;


/*==============================================================================
 Primitive:   ATI_SMS_CMGD_TEST_CNF ( SMS-> ATI )
 Description: ATI_SMS_CMGD_REQ Response test mode
==============================================================================*/
typedef struct {
    uint8_t bRecNum;
    uint8_t *pbRecIndex;
}S_AtiSms_CmgdTestCnf;

/*==============================================================================
 Primitive:   ATI_SMS_CMGW_REQ ( ATI-> SMS )
 Description: +CMGW request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bStat;        /* +CMGW<stat> */
    uint8_t    bTpduLen;     /* SMS TPDU len，max 164 bytes，not include sca len*/
    uint8_t    bPduDataLen;  /* SMS PDU len*/
    uint8_t    *pbPduData;   /* SMS PDU */
}S_AtiSms_CmgwReq;


/*==============================================================================
 Primitive:   ATI_SMS_CMGW_CNF ( SMS-> ATI )
 Description: ATI_SMS_CMGW_REQ Response
==============================================================================*/
typedef struct {
    uint16_t wIndex;
}S_AtiSms_CmgwCnf;

/*==============================================================================
 Primitive:   ATI_SMS_CMGR_REQ ( ATI-> SMS )
 Description: +CMGR request
==============================================================================*/
typedef struct {
    uint8_t     bChannelId;
    uint16_t    wIndex;
}S_AtiSms_CmgrReq;

/*==============================================================================
 Primitive:   ATI_SMS_CMGR_CNF ( SMS-> ATI )
 Description: ATI_SMS_CMGR_REQ Response
==============================================================================*/
typedef struct {
    uint8_t bStat;
    uint8_t bTpduLen;
    uint8_t bPduLen;
    uint8_t *pbPduData;
}S_AtiSms_CmgrCnf;

/*==============================================================================
 Primitive:   ATI_SMS_CMMENA_REQ ( ATI-> SMS )
 Description:  +CMMENA request
==============================================================================*/
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bAvailStat;     /* ATI_SMS_MEM_FREE, ATI_SMS_MEM_FULL */
}S_AtiSms_CmmenaReq;

/*==============================================================================
 primitive：ATI_SMS_REC_RSP_REQ(ATI->SMS)
 NOTE: MT service，returned response report
==============================================================================*/
typedef struct
{
    uint8_t      bChannelId;
    uint8_t      bAckMode;              /* sms-deliver-report result */
    uint8_t      bCause;
    uint8_t      bTpduLen;              /* TPDU length:(0-164B).*/
    uint8_t     *pbTPduData;
}S_AtiSms_RecRsp_Req;

/*==============================================================================
 Primitive:   ATI_ESM_PDP_ACTIVATE_REQ (ATI->ESM)
 Description: MO PDP context active
==============================================================================*/
typedef struct{
    uint8_t              bPcoLen;
    uint8_t              abPco[L3_MAX_PCO_CONTENT_LEN];
}S_L3_ProtCfgOpt_LV;

typedef struct
{
    uint8_t    bSrcIndex;
    uint8_t    bCidNum;
    uint8_t    bAtType;    /*
                         * AT command
                         * +CGACT:  ATSM_ATCMD_CGACT
                         * +CMGACT:  ATSM_ATCMD_CMGACT
                         */
    uint8_t    abCid[ATSM_MAX_CID_NUM];
#ifdef CFW_API_SUPPORT
    S_L3_ProtCfgOpt_LV  tPco;
#endif
} S_AtiEsm_PdpAct_Req;


/*==============================================================================
 Primitive:   ATI_ESM_PDP_DEACTIVATE_REQ (ATI->ESM)
 Description: MO PDP context deactive request
==============================================================================*/
typedef struct
{
    uint8_t    bSrcIndex;
    uint8_t    bCidNum;
    uint8_t    bAtType;    /*
                         * AT command
                         * +CGACT:  ATSM_ATCMD_CGACT
                         * +CMGACT:  ATSM_ATCMD_CMGACT
                         */
    uint8_t    abCid[ATSM_MAX_CID_NUM];
}S_AtiEsm_PdpDeact_Req;


/*==============================================================================
 Primitive:   AT_SM_NEG_QOS_QUERY_REQ_EV, AT_SM_NEG_EQOS_QUERY_REQ_EV,
        ATI_ESM_PDP_ADDR_QUERY_REQ, ATI_ESM_PDP_MODIFY_REQ,     (ATI->ESM)
 Description:  QOS info request
==============================================================================*/
typedef struct
{
    uint8_t    bSrcIndex;
    uint8_t    bCidNum;
    uint8_t    abCid[ATSM_MAX_CID_NUM];
}S_AtiEsm_QueryPdpAddr_Req,S_AtiEsm_PdpMod_Req;


/*==============================================================================
 Primitive:   NONE
 Description: return query PDP address (static address or dynamical address )
==============================================================================*/
typedef struct
{
    uint8_t            bCid;

    S_AtiEsm_PdpAddr   tPdpAddr;
}S_AtiEsm_CidPdpAddr;

typedef struct
{
    uint8_t              bCidNum;
    S_AtiEsm_CidPdpAddr  atPdpAddr[ATSM_MAX_CID_NUM];
}S_AtiEsm_QueryPdpAddr_Cnf;


/*==============================================================================
 Primitive:   AT_ESM_EPS_QOS_QUERY_REQ_EV (ATI->ESM)
 Description: +CGEQOSRDP，query EPS QoS set in bearer context
==============================================================================*/
typedef struct
{
    uint8_t    bSrcIndex;
    uint8_t    bCidFlg; /*
                      * ATESM_CMD_WITHOUTCID
                      * ATESM_CMD_WITHCID
                      */
    uint8_t    bCid;
}S_AtiEsm_EpsQosQuery_Req;


/*==============================================================================
 Primitive:   NONE
 Description:  +CGEQOSRDP: return EPS QoS set in bearer context
==============================================================================*/
typedef struct
{
    uint8_t             bCid;
    S_AtiEsm_QosInfo    tQosInfo;
}S_AtiEsm_EpsQosInfo;

//ATI_ESM_EPS_QOS_QUERY_CNF
typedef struct
{
    uint8_t               bCidNum;
    S_AtiEsm_EpsQosInfo   atEpsQosInfo[DEV_ATMEM_MAX_CID_NUM];
}S_AtiEsm_EpsQosQuery_Cnf;

/*==============================================================================
 Primitive:   ATI_ESM_CGEQOS_QUERY_REQ (ATI->ESM)
 Description: +CGEQOS，query EPS QoS set in bearer context
==============================================================================*/
typedef struct
{
    uint8_t    bSrcIndex;
}S_AtiEsm_CgeqosQueryReq;

/*==============================================================================
 Primitive:   ATI_ESM_CGTFTRDP_REQ (ATI->ESM)
 Description: read TFTrelated parameter
==============================================================================*/
typedef struct
{
    uint8_t   bSrcIndex;
    uint8_t   bCidFlg;
    uint8_t   bCid;
} S_AtiEsm_Cgtftrdp_Req;


/*==============================================================================
 Primitive:   NONE
 Description:  response TFT related parameter
==============================================================================*/
typedef struct {
    uint8_t                  bCid;          /* 标识PDP context */
    uint8_t                  bNwFilterId;   /* filter ID: 1~16  */
    S_Dev_AtMem_FilterType   tFilterData;
} S_AtiEsm_CidFilter;

/* read TFTrelated parameter*/
typedef struct
{
    uint8_t                  bFilterNum;
    S_AtiEsm_CidFilter       tNwAssignFilter[ATSM_MAX_FILTER_ID];
} S_AtiEsm_Cgtftrdp_Para;

//ATI_ESM_CGTFTRDP_SET_CNF
typedef struct
{
    uint8_t                  bCidNum;
    uint8_t                  bResult;  /* R_SUCC/R_FAIL */
    S_AtiEsm_Cgtftrdp_Para   atNwAssignInfo[ATSM_MAX_CID_NUM];  /* TFTrelated parameter*/
} S_AtiEsm_CgtftrdpSetCnf;

/*==============================================================================
 Primitive:   ATI_ESM_CGTFT_QUERY_REQ (ATI->ESM)
 Description: read TFTrelated parameter
==============================================================================*/
typedef struct
{
    uint8_t   bSrcIndex;
} S_AtiEsm_CgtftQueryReq;

/*==============================================================================
 Primitive:   ATI_ESM_ACTIVATED_CID_QUERY_REQ (ATI->ESM)
 Description:  query actived PDP context cid
==============================================================================*/
typedef struct
{
    uint8_t    bSrcIndex;
    uint8_t    bCmdType; /*
                       * ATSM_CMD_CGCMOD
                       * ATSM_CMD_CGEQNEG
                       * ATSM_CMD_CGCONTRDP
                       * ATSM_CMD_CGSCONTRDP
                       * ATSM_CMD_CGTFTRDP
                       * ATSM_CMD_CGEQOSRDP
                       */
}S_AtiEsm_QueryActCid_Req;

/*==============================================================================
 Primitive:   ATI_ESM_ACTIVATED_CID_QUERY_CNF (ESM -> ATI)
 Description:  ATI_ESM_ACTIVATED_CID_QUERY_REQ Response
==============================================================================*/
typedef struct {
    uint8_t    bCmdType;
    uint8_t    bCidNum;
    uint8_t    abCid[ATSM_MAX_CID_NUM];
}S_AtiEsm_GetEsmActCidCnf;


/*==============================================================================
 Primitive:   ATI_ESM_PDP_STATUS_QUERY_REQ, AT_SM_PDP_CAUSE_QUERY_REQ_EV (ATI->ESM)
 Description:   Query command
==============================================================================*/
typedef struct
{
    uint8_t     bSrcIndex;
}S_AtiEsm_QueryPdpStatus_Req,S_AtiEsm_QueryPdpCause_Req;


/*==============================================================================
 Primitive:  NONE
 Description: return all defined PDP context state
==============================================================================*/
typedef struct
{
    uint8_t    bActCidNum;
    uint8_t    bInactCidNum;
    uint8_t    abActCid[ATSM_MAX_CID_NUM];     /* actived cid list  */
    uint8_t    abInactCid[ATSM_MAX_CID_NUM];   /* inactived cid list  */
}S_AtiEsm_QueryPdpStatus_Cnf;


/*==============================================================================
 Primitive:   ATI_ESM_CGCONTRDP_REQ (ATI->ESM)
 Description: read first PDP context related parameters
==============================================================================*/
typedef struct
{
    uint8_t    bSrcIndex;
    uint8_t    bCidFlg;    /* R_VALID/R_INVALID */
    uint8_t    bCid;       /* PDP context */
} S_AtiEsm_Cgcontrdp_Req;


/*==============================================================================
 Primitive:   NONE
 Description: response first PDP context related parameters
==============================================================================*/
typedef struct
{
    BITS    bSrcAddrAndMaskFlg:1;
    BITS    bGwAddrFlg:1;
    BITS    bDnsPriAddrFlg:1;
    BITS    bDnsSecAddrFlg:1;
    BITS    bPCscfPriAddrFlg:1;
    BITS    bPCscfSecAddrFlg:1;
    BITS    bPCscfSecAddr2Flg:1;
    BITS    bPCscfSecAddr3Flg:1;

    uint8_t    bImcnSignInd;

    uint8_t    abSrcAddrAndMask[ATSM_MAX_PDPADDR_MASK_LEN];
    uint8_t    abGwAddr[ATSM_ADDR_MAX_LEN];
    uint8_t    abDnsPriAddr[ATSM_ADDR_MAX_LEN];
    uint8_t    abDnsSecAddr[ATSM_ADDR_MAX_LEN];
    uint8_t    abPCscfPriAddr[ATSM_ADDR_MAX_LEN];
    uint8_t    abPCscfSecAddr[ATSM_ADDR_MAX_LEN];
    uint8_t    abPCscfSecAddr2[ATSM_ADDR_MAX_LEN];
    uint8_t    abPCscfSecAddr3[ATSM_ADDR_MAX_LEN];
} S_AtiEsm_Cgcontrdp_AddrInfo;

typedef struct
{
    uint8_t                       bCid;
    uint8_t                       bBearerId;
    uint8_t                       bPdpType;  /*
                                              * ATESM_IPV4
                                              * ATESM_IPV6
                                              * ATESM_IPV4V6
                                              */

    uint8_t                       abApn[ATSM_APN_MAX_LEN];
    S_AtiEsm_Cgcontrdp_AddrInfo   atNwEstIPv4Info;
    S_AtiEsm_Cgcontrdp_AddrInfo   atNwEstIPv6Info;
}S_AtiEsm_Cgcontrdp_Para;

typedef struct
{
    uint8_t                   bCidNum;  /* PDP context number*/
    uint8_t                   bResult;  /* R_SUCC/R_FAIL */
    S_AtiEsm_Cgcontrdp_Para   atNwEstInfo[ATSM_MAX_CID_NUM];  /* first PDP context related parameters*/
}S_AtiEsm_Cgcontrdp_Cnf;

/*==============================================================================
 Primitive:   ATI_ESM_CGSCONTRDP_REQ (ATI->ESM)
 Description: read second PDP context related parameter
==============================================================================*/
typedef struct
{
    uint8_t    bSrcIndex;
    uint8_t    bDesIndex;
    uint8_t    bCidFlg;
    uint8_t    bCid;
} S_AtiEsm_Cgscontrdp_Req;


/*==============================================================================
 Primitive:   NONE
 Description: response second PDP context related parameter
==============================================================================*/
typedef struct
{
    uint8_t    bCid;
    uint8_t    bPriCid;
    uint8_t    bBearerId;
    uint8_t    bImcnSignInd;
} S_AtiEsm_Cgscontrdp_Para;

typedef struct
{
    uint8_t                   bCidNum;
    uint8_t                   bResult; /* R_SUCC/R_FAIL */
    S_AtiEsm_Cgscontrdp_Para  tSecPdpPara[ATSM_MAX_CID_NUM];  /* second PDP context related parameter*/
} S_AtiEsm_Cgscontrdp_Cnf;


/*==============================================================================
 Primitive:   NONE
 Description:  PDP report
==============================================================================*/
/* first PDP context active, deactive report*/
typedef struct
{
    uint8_t    bType;  /*
                     * +CGEV report:
                     * ATSM_CGEV_ME_PRI_PDN_ACT
                     * ATSM_CGEV_NW_PRI_PDN_DEACT
                     * ATSM_CGEV_ME_PRI_PDN_DEACT
                     * ATSM_CGEV_NW_PRI_PDN_ACT
                     */

    uint8_t    bCid;  /* first PDP context  */
} S_AtiEsm_Cgev_PriPdn;

/* modify PDP context report*/
typedef struct
{
    uint8_t    bType;       /*
                          * +CGEV report:
                          * ATSM_CGEV_NW_MODIFY
                          * ATSM_CGEV_ME_MODIFY
                          */

    uint8_t    bCid;        /* first PDP context */
    uint8_t    bChngReason; /*
                          * ATSM_CGEV_CHNG_TFT
                          * ATSM_CGEV_CHNG_QOS
                          * ATSM_CGEV_CHNG_TFT_AND_QOS
                          */

    uint8_t    bEventType;  /*
                          * ATSM_CGEV_EVENT_TYPE_INFO
                          * ATSM_CGEV_EVENT_TYPE_ACKREQ
                          */
} S_AtiEsm_Cgev_ModifyPdn;

/* second PDP context active , deactive report*/
typedef struct
{
    uint8_t    bType;  /*
                     * +CGEV report:
                     * ATSM_CGEV_NW_SEC_PDN_ACT
                     * ATSM_CGEV_ME_SEC_PDN_ACT
                     * ATSM_CGEV_NW_SEC_PDN_DEACT
                     * ATSM_CGEV_ME_SEC_PDN_DEACT
                     */

    uint8_t    bPriCid;  /* first PDP context */
    uint8_t    bCid;  /* second PDP context */
    uint8_t    bEventType;  /*
                          * ATSM_CGEV_EVENT_TYPE_INFO
                          * ATSM_CGEV_EVENT_TYPE_ACKREQ
                          */
} S_AtiEsm_Cgev_SecPdn;

typedef struct
{
    uint8_t                        bCgevType;  /*
                                             * ATSM_CGEV_TYPE_PRI_PDN_IND
                                             * ATSM_CGEV_TYPE_MODIFY_IND
                                             * ATSM_CGEV_TYPE_SEC_PDN_IND
                                             * ATSM_CGEV_TYPE_R5_VER_IND
                                             */
    union
    {
        S_AtiEsm_Cgev_PriPdn    tPriPdnInd;  /* first PDP context active 、deactive report*/
        S_AtiEsm_Cgev_ModifyPdn tModifyInd;  /* modify PDP context report*/
        S_AtiEsm_Cgev_SecPdn    tSecPdnInd;  /* second PDP context active 、deactive report*/
    }u;
}S_AtiEsm_Cgev_Ind;



typedef struct
{
    uint8_t             bCid;
    uint8_t             bHandleResult;      /* R_SUCC/R_FAIL */
    uint8_t             bCause;    /*
                                    * ATSM_PDP_ACT_CAUSE_IPV4_ONLY_ALLOWED
                                    * ATSM_PDP_ACT_CAUSE_IPV6_ONLY_ALLOWED
                                    * ATSM_PDP_ACT_CAUSE_SINGLE_ADDR_ONLY_ALLOWED
                                    */

    uint32_t            dwReason;
    S_AtiEsm_PdpAddr    tPdpAddr;
}S_AtiEsm_PdpActRes;

typedef struct
{
    uint8_t     bCid;
    uint8_t     bHandleResult;   /* R_SUCC/R_FAIL */
    uint32_t    dwReason;
}S_AtiEsm_PdpModRes;

/*==============================================================================
 Primitive:   ATI_ESM_TFT_SET_REQ_EV (ATI->ESM)
 Description:   +CGTFT
==============================================================================*/
typedef struct
{
    uint8_t        bCid;
    uint8_t        bFilterIdFg:1;
    uint8_t        bPreIndexFg:1;
    BITS        :6;
    S_Dev_AtMem_FilterType tFilter;
}S_AtiEsm_SetCidFilter;

typedef struct
{
    uint8_t        bSrcIndex;
#define ATSM_DEFINE_TFT       0
#define ATSM_UNDEFINE_TFT     1
    uint8_t        bSetType; /*ATSM_DEFINE_TFT,ATSM_UNDEFINE_TFT*/
    S_AtiEsm_SetCidFilter    tTft;
}S_AtiEsm_SetTft_Req;

/*==============================================================================
 Primitive:  NONE
 Description: +CGTFT? result
==============================================================================*/
typedef struct
{
    uint8_t                 bCid;
    uint8_t                 bFilterNum;
    S_Dev_AtMem_FilterType  atFilter[DEV_ATMEM_MAXINUM_FILTER];
}S_AtiEsm_PacketFilter;

typedef struct
{
    uint8_t        bCidNum;
    S_AtiEsm_PacketFilter  atCidFilter[ATSM_MAX_CID_NUM];
}S_AtiEsm_TftInfo;


/*==============================================================================
 Primitive:   ATI_ESM_EQOS_SET_REQ_EV (ATI->ESM)
 Description:  +CGEQOS  set command
==============================================================================*/
typedef struct
{
    uint8_t              bSrcIndex;
#define ATSM_DEFINE_EQOS           0
#define ATSM_UNDEFINE_EQOS         1
    uint8_t              bSetType;
    uint8_t              bPadding;
    S_AtiEsm_EpsQosInfo  tEpsQosInfo;
}S_AtiEsm_EqosSet_Req;



/*==============================================================================
 Primitive:   NONE
 Description:  +CGEQOS? result
==============================================================================*/
typedef struct
{
    uint8_t               bCidNum;
    S_AtiEsm_EpsQosInfo   atEqosInfo[ATSM_MAX_CID_NUM];
}S_AtiEsm_QueryEpsQos_Result;


typedef struct{
    uint8_t    bNumSpare  : 3;
    uint8_t    bAddCI  : 1;
    uint8_t    bCodeSchm  : 3;
    uint8_t    bSpare0: 1;
    uint8_t    bTxtStrLen;
    uint8_t    abTxtStr[ATIEMM_MAX_TXTSTR_NUM];
}S_AtiEmm_NetName;

typedef struct{
    uint8_t    bYear;
    uint8_t    bMonth;
    uint8_t    bDay;
    uint8_t    bHour;
    uint8_t    bMinute;
    uint8_t    bSecond;
    uint8_t    bTmZone;
}S_AtiEmm_TZAndTime;

/*==============================================================================
 Primitive:   NONE
 Description: EMM report mminfo
==============================================================================*/
typedef struct {
    BITS                  bFNameFg:1;
    BITS                  bSNameFg:1;
    BITS                  bLTZoneFg:1;
    BITS                  bUTimeFg:1;
    BITS                  bSTimeFg:1;
    BITS                  :3;
    uint8_t               bLocalTZone;    /* to see 24008 10.5.3.8*/
    S_AtiEmm_NetName      tFullName;
    S_AtiEmm_NetName      tShortName;
    S_AtiEmm_TZAndTime    tUTime;         /* to see 24008 10.5.3.9*/
    uint8_t               bSavTime;       /* to see 24008 10.5.3.12 0*/
    S_PS_PlmnId           tCurPlmnId;     /*current plmn ID*/
}S_AtiEmm_MmInfo_Ind;

typedef struct {
  uint16_t tm_year;          /*    年 >   2000  */
  uint8_t tm_mon;           /*    月    [1:12] */
  uint8_t tm_mday;          /*    日    [1:31] */
  uint8_t tm_hour;          /*    小时 [0:23] */
  uint8_t tm_min;           /*    分钟 [0:59] */
  uint8_t tm_sec;           /*    秒    [0:59] */
  int8_t tm_zone;          /*    时区    [-96:96]*/
  uint8_t tm_dst;          /*    夏时令    [0:2] */
}S_Ati_DateTimeZone;


typedef struct
{
    int16_t       sinrInt1;           //取值范围 -20~40  整数部分
    uint16_t       sinrDec1;           //取值范围 0~9 小数部分
    int16_t       sinrInt2;           //取值范围 -20~40  整数部分
    uint16_t       sinrDec2;           //取值范围 0~9 小数部分
}S_AtiRrc_ScellSinr;

/*==============================================================================
 Primitive:   NONE
 Description: Module report
==============================================================================*/
#define AT_AS_NCELL_NUM      1

typedef struct {

    int16_t           sRsrp;                //  -1410~-440, real value*10, Invalid: -32768
    int16_t           sRsrq;                //  -195~-30, real value*10, Invalid: -32768
    int16_t           sRssi;                //  -1000~-250, real value*10, Invalid: -32768
    int16_t           sSinr;                //  -230~400, real value*10, Invalid: -32768
}S_AT_UEStats_Meas;

typedef struct {
    uint8_t            tMCC[3];               // 0-9,
    uint8_t            tMNC[3];               // 0-9, Invalid: 0xFF
    uint8_t            bBandWidth;            // 0-5,dlBandWidth,servCell Valid,nCell not support now, reserved, Invalid: 0xFF
    uint8_t            bEarfcnOff;            // not support now, reserved
    uint8_t            bIsTdd;                // 0:FDD 1:TDD,Invalid: 0xFF
    uint8_t            bUlBandWidth;          // 0,5,ulBandWidth,servCell Valid,nCell not support now, reserved, Invalid: 0xFF
    uint16_t           wPci;	              // 0~503,    Invalid: 65535
    uint32_t           dwEarfcn;              // 0~68535,  Invalid: -1
    S_AT_UEStats_Meas          tCellMeasInfo;
    int16_t            sSrxlev;               // -970~960, real value*10, Invalid: -32768 servCell Valid,nCell not support now, reserved
}S_AT_UEStats_CellInfo;

typedef struct {
    uint8_t         bNcellNum;                   //=0 now, no need NCELL, reserved
    S_AT_UEStats_CellInfo       tServCell;         // Serving Cell information
    S_AT_UEStats_CellInfo       atNCell[AT_AS_NCELL_NUM];    //no need NCELL.
}S_AT_UEStats_Cell;


typedef struct {
    uint8_t            bAct;                   // 0: Not Camp￡? 4:LTE
    uint8_t            bEcl;                   // enhanced coverage 0:normal￡?1,2￡oenhanced￡?255￡oinvalid. not support now, reserved.
    uint16_t           wTac;                   //TrackingAreaCode
    uint32_t           dwCellId;                // 28bit CellId from sib1
    S_AT_UEStats_CellInfo  tLastServCell;         // Serving Cell information
    int16_t             sTxpower;               // not support now, reserved
    uint16_t            wTxTime;                // not support now, reserved
    uint16_t            wRxTime;                // not support now, reserved
}S_AT_UEStats_Raido;

//typedef struct {
//}S_AT_UEStats_Bler;

//typedef struct {
//}S_AT_UEStats_Thp;

typedef struct {
    uint8_t                  bBand;         //1~66, 0xFF: not camp
    S_AT_UEStats_Raido        tRadio;
    S_AT_UEStats_Cell         tCell;
    //S_AT_UEStats_Bler         tBler;         //
    //S_AT_UEStats_Thp          tThp;          //
}S_AT_UEStats;


typedef struct
{
    uint16_t msgId;
    void*   pmsgData;
}S_Ati_Msg_Req;

#define USIM_AUTH_RAND_LEN              16
#define USIM_AUTH_AUTN_MAX_LEN          16
#define USIM_AUTH_RES_MAX_LEN           16
#define USIM_AUTH_CK_LEN                16
#define USIM_AUTH_IK_LEN                16
#define USIM_AUTH_AUTS_MAX_LEN          14

typedef struct {
    uint8_t                                bChanId;
    uint8_t                                bIsimChid;
    uint8_t                                bAutnLength;
    uint8_t                                abRand[USIM_AUTH_RAND_LEN];
    uint8_t                                abAutn[USIM_AUTH_AUTN_MAX_LEN];
}S_EmmUsim_AuthReq;

typedef struct {
    uint8_t                                bChanId;
    uint8_t                                bAuthResult;
    uint8_t                                bResLen;
    uint8_t                                bAutsLen;
    uint8_t                                abRes[USIM_AUTH_RES_MAX_LEN];
    uint8_t                                abCk[USIM_AUTH_CK_LEN];
    uint8_t                                abIk[USIM_AUTH_IK_LEN];
    uint8_t                                abAuts[USIM_AUTH_AUTS_MAX_LEN];
}S_EmmUsim_AuthRsp;


/* PDP context type */
typedef struct
{
    uint8_t                                        bCid;
    uint8_t                                        bPdpTypeFg;
    uint8_t                                        bPdpType;           /* bPdpTypeFg=1
                                                                     * APAT_SM_PPP
                                                                     * APAT_SM_IPV4
                                                                     * APAT_SM_IPV6
                                                                     */
    uint8_t                                        bDComp;             /*
                                                                     * ATI_SM_DATACOMPRESS_OFF
                                                                     * ATI_SM_DATACOMPRESS_ON
                                                                     * ATI_SM_DATACOMPRESS_V42
                                                                     * ATI_SM_DATACOMPRESS_V44
                                                                     * other-reserved
                                                                     */

    uint8_t                                        bHComp;             /*
                                                                     * ATI_SM_HEADERCOMPRESS_OFF
                                                                     * ATI_SM_HEADERCOMPRESS_ON
                                                                     * ATI_SM_HEADERCOMPRESS_RFC1144
                                                                     * ATI_SM_HEADERCOMPRESS_RFC2507
                                                                     * ATI_SM_HEADERCOMPRESS_RFC3095
                                                     * other-reserved
                                                              */
    uint8_t                                        bIPv4AddrAlloc; /*
                                                                * IPv4 address，range：
                                                                * ATI_IPV4ADDR_ALLOC_THROUGH_SIGNAL
                                                                * ATI_IPV4ADDR_ALLOC_THROUGH_DHCP
                                                                */
    uint8_t                                        bEmergencyInd;  /*
                                                                 * indicate PDP context if it used as emergency bearer services，range：
                                                                 * ATI_IPDPCONT_IS_NOT_FOR_EMER_BEARER_SERV
                                                                 * ATI_IPDPCONT_IS_FOR_EMER_BEARER_SERV
                                                                 */
   uint8_t                                        bPCscfDisc; /* * ATI_PREFER_PCSCF_NOT_INFLUENCED_BY_CGDCONT        (uint8_t)0
                                                             * ATI_PREFER_PCSCF_THROUGH_NAS_SIGNAL        (uint8_t)1
                                                             * ATI_PREFER_PCSCF_THROUGH_DHCP        (uint8_t)1*/
    uint8_t                                        bImcnSignInd;/*  * ATI_IPDPCONT_IS_NOT_FOR_IMCN        (uint8_t)0
                                                                 * ATI_IPDPCONT_IS_FOR_IMCN            (uint8_t)1*/
    uint8_t                                        abPadding[3];
    uint8_t                                        abApn[ATI_SM_MAX_APN_LEN];

    uint8_t                                        abPdpAddr[ATI_SM_IPV4V6_ADDR_MAX_LEN];
                                                                    /* all 0: address dynamically。10.40.64.3:
                                                                     * abPdpAddr[0]=10,abPdpAddr[1]=40,abPdpAddr[2]=64,abPdpAddr[3]=3*/
}S_Ati_CidPdpContext;


/*==============================================================================
 Primitive:   ATI_COMMON_CNF ( ->ATI)
 Description: At common response: OK or CME ERROR
==============================================================================*/
typedef struct {
    uint8_t    bResult;
    uint16_t   wCause;
}S_Ati_CommonCmdRsp;

/*==============================================================================
 Primitive:   ATI_COMMON_SMS_CNF ( ->ATI)
 Description: At common SMS response: OK or CMS ERROR
==============================================================================*/
typedef struct {
    uint8_t    bResult;
    uint16_t   wCause;
}S_AtiSms_CommonCmdRsp;


/*==============================================================================
 Primitive:   ATI_EMM_CIREPI_IND (ENM ->ATI)
 Description: +CIREPI Report
==============================================================================*/
typedef struct {
    uint8_t bImsOverPsInd;
}S_AtiEmm_CirepiInd;

/*==============================================================================
 Primitive:   ATI_EMM_EMER_BEARER_IND (ENM ->ATI)
 Description: +CNEMS1 Report
==============================================================================*/
typedef struct {
    uint8_t bEmergencybearerInd;
}S_AtiEmm_EmerberInd;

/*==============================================================================
 Primitive:   ATI_EMM_CSCON_IND (ENM ->ATI)
 Description: +CSCON Report
==============================================================================*/
typedef struct {
    uint8_t bMode;
}S_AtiEmm_CsconInd;


/*==============================================================================
 Primitive:   ATI_SMS_RECV_STORE_MSG_IND (SMS ->ATI)
 Description: +CMTI or +CDSI Report
==============================================================================*/
typedef struct {
    uint8_t bMsgType;
    uint8_t bMem;
    uint16_t wIndex;
}S_AtiSms_RecvStoreMsgInd;

/*==============================================================================
 Primitive:   ATI_SMS_STATUS_REPORT_PDU_IND (SMS ->ATI)
 Description: +CDS Report
==============================================================================*/
typedef struct {
    uint8_t bPduLen;
    uint8_t *pbPduBuf;
}S_AtiSms_StatusReportPduInd;

/*==============================================================================
 Primitive:   ATI_SMS_DELIVER_PDU_IND (SMS ->ATI)
 Description: +CMT Report
==============================================================================*/
typedef struct {
    uint8_t bPduLen;
    uint8_t *pbPduBuf;
}S_AtiSms_DeliverPduInd;

/*==============================================================================
 Primitive:   ATI_SMS_CMMGSF_IND (SMS ->ATI)
 Description: +CMMGSF Report
==============================================================================*/
typedef struct {
    uint16_t wCause;
}S_AtiSms_CmmgsfInd;

/*==============================================================================
 Primitive:   ATI_USIM_USAT_PROC_CMD_IND (SMS ->ATI)
 Description: +CUSATP Report
==============================================================================*/
typedef struct {
    uint16_t wDataLen;
    uint8_t *pbData;
}S_AtiUsim_UsatProcCmdInd;

/*==============================================================================
 Primitive:   ATI_USIM_INIT_STAT_IND (SMS ->ATI)
 Description: +CMISIMINIT、+CMURDY、+CMPBIC、+CPIN Report
==============================================================================*/
typedef struct {
    uint8_t bInitStat;
    uint8_t bChanId;
    S_IMSI  tImsi;
}S_AtiUsim_InitStatInd;

/*==============================================================================
 Primitive:   ATI_USIM_CMUSLOT_IND (SMS ->ATI)
 Description: +CMUSLOT Report
==============================================================================*/
typedef struct {
    uint8_t bSlotNum;
    uint8_t bSlocAct;
}S_AtiUsim_CmuslotInd;

/*==============================================================================
 Primitive:   ATI_USIM_SMS_INIT_IND (SMS ->ATI)
 Description: +CMPBIC Report
==============================================================================*/
typedef struct {
    uint8_t bIsInitSucc;
    uint8_t bOprtType;
}S_AtiUsim_SmsInitInd;

/*==============================================================================
 Primitive:   ATI_ESM_CMGIPDNS_IND (ESM ->ATI)
 Description: +CMGIPDNS Report
==============================================================================*/
typedef struct {
    uint8_t                                         iCid;
    uint8_t                                         iPdpType;   /*
                                                             * 1-IPv4
                                                             * 2-IPv6
                                                             * 3-IPV4v6
                                                             */
 /*all  0 is dynamic ip. else eg: 10.10.1.1    iPdpAddr[0]=10,iPdpAddr[1]=10,iPdpAddr[2]=1,iPdpAddr[3]=1*/
    uint8_t                                         iPdpAddr[ATSM_MAX_PDPADDR_LEN];
    uint8_t                                         iGateWay[ATSM_MAX_PDPADDR_LEN];
    uint8_t                                         iPrimaryDns[ATSM_MAX_PDPADDR_LEN];
    uint8_t                                         iSecondaryDns[ATSM_MAX_PDPADDR_LEN];
}S_Ati_SmIpDnsInfo;

typedef struct {
    uint8_t                 bCidNum;
    S_Ati_SmIpDnsInfo       atSmIpDnsInfo[ATSM_MAX_CID_NUM];
}S_AtiEsm_Cmgipdnsind;

/*==============================================================================
 Primitive:   ATI_EMM_SYS_INFO_REQ (ATI ->ENM)
 Description: ^SYSINFO Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiEmm_SysInfoReq;

typedef struct {
    uint8_t bSrvStatus;
    uint8_t bSrvDomain;
    uint8_t bRoamStatus;
    uint8_t bSysMode;
    uint8_t bSimStat;
    uint8_t bSubModeFg;
    uint8_t bSubMode;
}S_AtiEmm_SysInfoCnf;

/*==============================================================================
 Primitive:   ATI_EMM_CGATT_QUERY_REQ (ATI ->ENM)
 Description: +CGATT read Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiEmm_CgattQueryReq;

typedef struct {
    uint8_t    bState;
}S_AtiEmm_CgattQueryCnf;

/*==============================================================================
 Primitive:   ATI_EMM_CEER_CAUSE_REQ (ATI ->ENM)
 Description: +CEER exec Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiEmm_CeerCauseReq;

typedef struct {
    uint8_t bEmmCause;
    uint8_t bEsmCause;
}S_AtiEmm_CeerCauseCnf;

/*==============================================================================
 Primitive:   ATI_EMM_CFUN_QUERY_REQ (ATI ->ENM)
 Description: +CFUN read Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiEmm_CfunQueryReq;

typedef struct {
    uint8_t bFun;
}S_AtiEmm_CfunQueryCnf;

/*==============================================================================
 Primitive:   ATI_EMM_COPS_QUERY_REQ (ATI ->ENM)
 Description: +CFUN read Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiEmm_CopsQueryReq;

typedef struct {
    uint8_t  bCopsMode;
    uint8_t  bOperFlg;
    uint8_t  bOperFormat;
    S_PS_PlmnId tPlmnId;
}S_AtiEmm_CopsQueryCnf;

/*==============================================================================
 Primitive:   ATI_EMM_CEDRXRDP_REQ (ATI ->ENM)
 Description: +CEDRXRDP exec Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiEmm_CedrxrdpReq;

typedef struct {
    uint8_t bEdrxflag;
    uint8_t bEdrxValue;
    uint8_t bNwEdrxValue;
    uint8_t bPageTimeValue;
}S_AtiEmm_CedrxrdpCnf,S_AtiEmm_CedrxpInd;

/*==============================================================================
 Primitive:   ATI_EMM_CEMODE_QUERY_REQ (ATI ->ENM)
 Description: +CEMODE read Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiEmm_CemodeQueryReq;

typedef struct {
    uint8_t bMode;
}S_AtiEmm_CemodeQueryCnf;

/*==============================================================================
 Primitive:   ATI_EMM_CEDRXS_SET_REQ (ATI ->ENM)
 Description: +CEDRXS set Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bModeFlg;
    uint8_t    bMode;
    uint8_t    bActFlg;
    uint8_t    bAct;
    uint8_t    bEdrxValueFlg;
    uint8_t    bEdrxValue;
}S_AtiEmm_CedrxsSetReq;

/*==============================================================================
 Primitive:   ATI_EMM_CEDRXS_QUERY_REQ (ATI ->ENM)
 Description: +CEDRXS read Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiEmm_CedrxsQueryReq;

typedef struct {
    uint8_t bEdrxValue;
}S_AtiEmm_CedrxsQueryCnf;

/*==============================================================================
 Primitive:   ATI_EMM_CPSMS_SET_REQ (ATI ->ENM)
 Description: +CPSMS read Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bMode;
    uint8_t    bRauTimer;
    uint8_t    bGprsTimer;
    uint8_t    bTauTimerFlg;
    uint8_t    bTauTimer;
    uint8_t    bActTimerFlg;
    uint8_t    bActTimer;
}S_AtiEmm_CpsmsSetReq;



/*==============================================================================
 Primitive:   ATI_EMM_CPSMS_QUERY_REQ (ATI ->ENM)
 Description: +CPSMS read Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiEmm_CpsmsQueryReq;

typedef struct {
    uint8_t bMode;
    uint8_t bTauTimer;
    uint8_t bActTimer;
}S_AtiEmm_CpsmsQueryCnf;

/*==============================================================================
 Primitive:   ATI_RRC_CMIMSBAR_SET_REQ (ATI ->RRC)
 Description: +CMIMSBAR set Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bMode;
}S_AtiRrc_CmimsbarSetReq;

typedef struct {
    uint8_t bStatus;
}S_AtiRrc_CmimsbarSetCnf;

/*==============================================================================
 Primitive:   ATI_RRC_CMSINR_REQ (ATI ->RRC)
 Description: +CMSINR exec Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiRrc_CmsinrReq;

/*==============================================================================
 Primitive:   ATI_RRC_CESQ_REQ (ATI ->RRC)
 Description: +CESQ exec Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiRrc_CesqReq;

typedef struct {
    uint8_t bRsrq;
    uint8_t bRsrp;
}S_AtiRrc_CesqCnf,S_AtiRrc_CesqInd;

/*==============================================================================
 Primitive:   ATI_RRC_CSQ_REQ (ATI ->RRC)
 Description: +CSQ exec Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiRrc_CsqReq;


typedef struct {
    uint8_t bRssi;
}S_AtiRrc_CsqCnf;

/*==============================================================================
 Primitive:   ATI_RRC_QCSQ_REQ (ATI ->RRC)
 Description: +QCSQ Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bAtType;
    uint8_t    bEnable;//for at set
}S_AtiRrc_QcsqReq;

#define ATI_QCSQ_SERVICE_NONE                             (uint8_t)0
#define ATI_QCSQ_SERVICE_LTE                              (uint8_t)1

typedef struct {
    uint8_t bAtType;//for set/read/act
    uint8_t bEnable;//for at get
    uint8_t bSrvTpye;//0: no service 1: lte
    int16_t sRsrq;
    int16_t sRsrp;
    uint8_t bSinr;
    int16_t sRssi;
}S_AtiRrc_QcsqCnf,S_AtiRrc_QcsqInd;

/*==============================================================================
 Primitive:   ATI_USIM_ICCID_REQ (ATI ->USIM)
 Description: +ICCID exec Request
==============================================================================*/
#define ATI_CMD_ICCID                                 (uint8_t)0
#define ATI_CMD_CCID                                  (uint8_t)1
typedef struct {
    uint8_t    bChannelId;
    uint8_t    bCmd;
}S_AtiUsim_IccidReq;

typedef struct {
    uint8_t    bCmd;
    uint8_t abIccid[ATI_NAS_MAX_ICCID_LEN];
}S_AtiUsim_IccidCnf;

/*==============================================================================
 Primitive:   ATI_USIM_ICCID_REQ (ATI ->USIM)
 Description: +ICCID exec Request
==============================================================================*/
typedef struct {
    uint8_t    bChannelId;
}S_AtiUsim_CimiReq;

typedef struct {
    S_IMSI     tImsi;
}S_AtiUsim_CimiCnf;

/****************************************************************************
  Primitive:ATI_USIM_CPOL_SET_REQ(ATI->USIM)
  Function:modify UPLMN selector
  AT CMD:+CPOL=[<index>][,<format>[,<oper>[,<GSM_AcT>,<GSM_Compact_AcT>,<UTRAN_AcT>,<E-UTRAN>]]]
 ***************************************************************************/
 typedef struct {
    uint8_t                                         bLen;
    uint8_t                                         abName[ATI_MAX_NAME_LEN];
/*
   long   alphanumeric format : upto 16 characters
   short  alphanumeric format : upto 8 characters
   numeric format: mcc + mnc:
       abName[0]:MCC2 MCC1
       abName[1]:MNC3  MCC3
       abName[2]:MNC2 MNC1
*/
}S_Ati_NetName;

typedef struct {
    uint8_t            bChannelId;
    uint8_t            bSetMode; //0:add or modify 1:delete
    uint8_t            bIndexFg;
    uint8_t            bIndex;
    uint8_t            bAcT;    /* bit1:0 GSM  bit2:GSM Compact bit3:UTRAN bit4:E-UTRAN */
    S_Ati_NetName      tOper;
}S_AtiUsim_CpolSetReq;

/****************************************************************************
  Primitive:ATI_USIM_CPOL_QUERY_REQ(ATI ->USIM)
  Function:query current UPLMN or OPLMN list
  AT CMD:+CPOL read or test command 
 ***************************************************************************/
typedef struct {
    uint8_t            bChannelId;
    uint8_t            bQueryMode; //0:read cmd 1:test cmd
    uint8_t            bPlmnType; /* 0:UPLMN  1:OPLMN 2:HPLMN*/
}S_AtiUsim_CpolQueryReq;

typedef struct {
    uint8_t            bQueryMode; //0:read cmd 1:test cmd
    uint8_t            bPlmnType; /* 0:UPLMN  1:OPLMN 2:HPLMN*/

    S_AtiUsim_CpolPlmn tPlmn;

}S_AtiUsim_CpolQueryCnf;

/****************************************************************************
  Primitive:ATI_USIM_CSCA_REQ(ATI ->USIM)
  Function:+CSCA set or read command
 ***************************************************************************/
typedef struct {
    uint8_t            bChannelId;
    uint8_t            bAtMode; //0:read cmd 1:set cmd
    uint8_t            bScaValLen;
    uint8_t            abScaVal[ATI_SMS_MAX_ADDRVALUE_LEN];
    uint8_t            bScaTypeFg;
    uint8_t            bScaType; /* 1+TON(3bits)+NPI(low 4bits)*/
}S_AtiUsim_CscaReq;

typedef struct {
    uint8_t            bScaValLen;
    uint8_t            abScaVal[ATI_SMS_MAX_ADDRVALUE_LEN];
    uint8_t            bScaType; /* 1+TON(3bits)+NPI(low 4bits)*/
}S_AtiUsim_CscaCnf;

typedef struct 
{
    uint8_t            bCeer;      // 3gpp network reject cause
    int16_t            sRsrp;
    uint8_t            Mcc[3];     //[0] mcc1, [1]mcc2, [2] mcc3
    uint8_t            Mnc[3];     //[0] mnc1, [1]mnc2, [2] mnc3
} S_Ati_NetCauseItem;

typedef struct 
{
#define API_NET_CAUSE_INFO_NUM   10
    uint8_t     bNUM;
    S_Ati_NetCauseItem atCauseItem[API_NET_CAUSE_INFO_NUM];
} S_Ati_NetCauseInfo;

#define AT_NET_NCELL_NUM                 (uint8_t)8
#define AT_NET_TIMESTAMP_CHAR_SIZE       (uint8_t)25

typedef struct {
    uint32_t       Earfcn;       // 0~68535
    uint16_t        Pci;          // 0~503
    int16_t       sRsrp;         //  (-140~-44)*10
}S_Ati_NCellInfo;

typedef struct {
    uint32_t       T3402_len;      //unit ms
    uint32_t       T3412_len;      //unit ms
    uint32_t       T3412ext_len;   //unit ms
    uint32_t       T3324_len;      //unit ms
    uint32_t       TedrxCycle;      //cycle from network 0~15
    uint32_t       Tptw;       //ptw from network, 0~15
    uint8_t        CurrPlmnRejCause;  //current plmn network reject cause,  if none valu is 0
    uint8_t        Mcc[3];     //[0] mcc1, [1]mcc2, [2] mcc3
    uint8_t        Mnc[3];     //[0] mnc1, [1]mnc2, [2] mnc3
    uint16_t       Tac;
    char           timestamp_last_rej[AT_NET_TIMESTAMP_CHAR_SIZE];
    uint32_t       Earfcn;        // 0~68535,  Invalid: 0xffffffff
    uint16_t       Pci;           // 0~503,    Invalid: 0xffff, ignore other para if invalid
    int16_t       sRsrp;         //  (-140~-44)*10
    int16_t       sRsrq;         //  (-20~-3)*10
    int16_t       Sinr;          //  (-20~40)*10
    int16_t       Qrxlevmin;     //  -140~-44, by step 2
    uint8_t       nCellNum;
    S_Ati_NCellInfo   atNCell[AT_NET_NCELL_NUM];
}S_Ati_CellInfo;

#ifdef CFW_API_SUPPORT

typedef struct {
    uint8_t    bSrcIndex;
    S_L3_ProtCfgOpt_LV  tPco;
}S_AtiEsm_DefPcoConfigReq;

#endif

/*==============================================================================
 Primitive:   ATI_L2_QGDCNT_REQ (ATI->L2)
 Description: +QGDCNT flow statistics
==============================================================================*/
typedef struct{
    uint8_t    bChannelId;
    uint8_t    bAtType;  /* ATI_CMD_MODE_SET/ATI_CMD_MODE_READ */
    uint8_t    bOp;  /*  use in bAtType == ATI_CMD_MODE_SET */
} S_AtiL2_Qgdcnt_Req;

typedef struct{
    uint32_t   dwByteSend;
    uint32_t   dwByteRecv;
} S_AtiL2_Qgdcnt_Query_Cnf;

/*==============================================================================
 Primitive:   ATI_L2_QAUGDCNT_REQ (ATI->L2)
 Description: +QGDCNT flow statistics
==============================================================================*/
typedef struct{
    uint8_t    bChannelId;
    uint8_t    bAtType;
    uint16_t    wValue;  /*  use in bAtType == ATI_CMD_MODE_SET */
} S_AtiL2_Qaugdcnt_Req;

typedef struct{
    uint16_t    wValue;
} S_AtiL2_Qaugdcnt_Query_Cnf;

/*--------------------------------------------------------------LINE------------------------------------------------------------------------*/
#ifdef IMS_SUPPORT

//ATC_IMS_ATA_REQ
//ATC_IMS_CHUP_REQ
//ATC_IMS_CLCC_REQ
//ATC_IMS_ATH_REQ
//ATC_IMS_CCWA_READ_REQ
//ATC_IMS_CVMOD_READ_REQ
typedef struct
{
    uint8_t    bChannelId;
} S_Atc_Ims_Com_Req;
//ATC_IMS_IMSCDU_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    abUrl[65];
} S_Atc_Ims_Imscdu_Req;
//ATC_IMS_CHLD_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bNFg;
    uint8_t    bN;
    uint8_t    bCauseFg;
    uint8_t    bCause;
} S_Atc_Ims_Chld_Req;

//ATC_IMS_ATD_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    abNumber[42];
    uint8_t    bTypeExist;
} S_Atc_Ims_Atd_Req;


//ATC_IMS_IMSAMRW_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bOn;
} S_Atc_Ims_Imsamrw_Req;
//ATC_IMS_IMSCWA_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bOn;
} S_Atc_Ims_Imscwa_Req;

//ATC_IMS_CCWA_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bNFg;
    uint8_t    bN;
    uint8_t    bModeFg;
    uint8_t    bMode;
    uint8_t    bClassFg;
    uint8_t    bClass;
} S_Atc_Ims_Ccwa_Req;

typedef struct
{
    uint8_t    bState;
    uint8_t    bClass;
} S_Atc_Ims_Ccwa_Cnf_Para;


typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bInfoNum;
    S_Atc_Ims_Ccwa_Cnf_Para atInfo[10];
} S_Atc_Ims_Ccwa_Cnf;

typedef struct
{
    uint8_t    bIdx;
    uint8_t    bDir;
    uint8_t    bStat;
    uint8_t    bMode;
    uint8_t    bMpty;
    uint8_t    bNumberFg;
    S_AtiSms_TpAddr tNumber;
} S_Atc_Ims_Clcc_Cnf_Para;

typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bInfoNum;
    S_Atc_Ims_Clcc_Cnf_Para atInfo[7];
} S_Atc_Ims_Clcc_Cnf;

//ATC_IMS_IMSPLUS_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bId;
    uint8_t    bAction;
} S_Atc_Ims_Imsplus_Req;
//ATC_IMS_ATS_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bOn;
} S_Atc_Ims_Ats_Req;
//ATC_IMS_VTS_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    abStr[30];
    uint8_t    bDurationFg;
    uint32_t   dwDuration;
} S_Atc_Ims_Vts_Req;
//ATC_IMS_IMSSET_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    abName[16];
    uint8_t    bValue;
} S_Atc_Ims_Imset_Req;


//ATC_IMS_CMSMSOIN_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bOn;
} S_Atc_Ims_Cmsmsoin_Req;
//ATC_IMS_IMSCONFURI_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    abConfuri[282];
} S_Atc_Ims_Imsconfuri_Req;
//ATC_IMS_IMSD_REQ
typedef struct
{
    uint8_t    abNumber[41];
    uint8_t    bAnon;
} S_Atc_Ims_Conf_Part_Item;
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bPartNumber;
    S_Atc_Ims_Conf_Part_Item    atItems[6];
} S_Atc_Ims_Imsd_Req;
//ATC_IMS_IMSREF_REQ
typedef struct
{
    uint8_t    bChannelId;
    S_Atc_Ims_Conf_Part_Item tPart;
} S_Atc_Ims_Imsref_Req;

//ATC_IMS_IMSUSD_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bNFg;
    uint8_t    bN;
    uint8_t    bStrFg;
    uint8_t    abStr[257];
    uint8_t    bDcsFg;
    uint8_t    abDcs[17];
} S_Atc_Ims_Imsusd_Req;
//ATC_IMS_CFUN_REQ
//ATC_IMS_CVMOD_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bVmodeFg;
    uint8_t    bVmode;
} S_Atc_Ims_Cvmod_Req;

//ATC_IMS_CVMOD_READ_REQ
typedef struct
{
    uint8_t    bChannelId;
} S_Atc_Ims_Cvmod_Read_Req;

//ATC_IMS_ IMSAPN _REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    abApn[33];
} S_Atc_Ims_Imsapn_Req;
//ATC_IMS_ IMSIPV _REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bIpv;
} S_Atc_Ims_Imsipv_Req;

//ATC_IMS_IMSTEST_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    abCmd[50];
    uint8_t    bNFg;
    uint32_t   dwN;
} S_Atc_Ims_Imstest_Req;

//ATC_IMS_CMSMSOIN_REQ
typedef struct
{
    uint8_t    bChannelId;
    uint8_t    bN;
} S_Atc_Ims_Imsmsoin_Req;


/*==============================================================================
 Primitive:   IMS_ATI_CGDCONT_REQ (IMS->ATI)
 Description: CGDCONT_REQ
==============================================================================*/


typedef struct
{
    uint8_t ChanelId;
    S_Ati_CidPdpContext  CidContext;
    //S_Ati_CidPdpContext  CidContext; //编译报错，暂时删掉
}S_ImsAti_Cgdcont_Req;


#endif

/*==============================================================================
 Description: response for AT just contain a result or cause
 Parameter:bResult        Refer R_SUCC;
==============================================================================*/
void AtiNasCommonCnf(uint8_t bChannelId, uint8_t bResult, uint32_t dwCause);


/*==============================================================================
 Description: success response for cops=?
 Parameter:
            bPlmnNum        plmn list count;
            ptPlmnList      plmn list, max count refer PLMN_LIST_MAX_NUM;
            ptStateList     plmn state list,  Refer COPS_STATE_UNKNOWNPLMN;
==============================================================================*/
void AtiNasListPlmnCnf(uint8_t bChannelId, uint8_t bPlmnNum, S_PS_PlmnId *ptPlmnList, uint8_t *ptStateList );


/*==============================================================================
 Description: success response for +ncell
 Parameter:
            bNcellNum       cell count;
            ptNcellInfo     cellinfo list
==============================================================================*/
void AtiNasListNcellCnf(uint8_t bChannelId, uint8_t bNcellNum,  S_AtiPs_NeighCellInfo *ptNcellInfo);


/*==============================================================================
 Description: Unsolicited report cereg info
 Parameter:  bChannelId   if ATI_INVALID_CHANNEL, for URC, else for cereg?
         bState  Refer CEREG_REGHPLMN
         wTac     if TAC_INVALID , not report
         dwCell   if CELLID_INVALID, not report
         bSubAct  if 0xff, not report
==============================================================================*/
void AtiNasPlmnInfoInd(uint8_t bChannelId,uint8_t bCmd,uint8_t bState, uint16_t wTac, uint32_t dwCell, uint8_t bSubAct);

void AtiUsim_SysinfoGetSimStat(uint8_t *pbCardState);

/*==============================================================================
 Description: Unsolicited report CMIMSVOPS info
 Parameter:
==============================================================================*/
void AtiNasImsvopsInd(uint8_t bImsOverPsInd);


/*==============================================================================
 Description: Unsolicited report Emer bearer info
 Parameter:
==============================================================================*/
void AtiNasEmerberInd(uint8_t bEmergencybearerInd);


/*==============================================================================
 Description: Unsolicited report CMMMI info
 Parameter:
==============================================================================*/
void AtiNasMminfoInd(S_AtiEmm_MmInfo_Ind *pMmInfo);


/*==============================================================================
 Description: Handle sms command then return OK or +CMS ERROR general function
 Parameter:
             bResult: APAT_OPERATION_SUCCESS, APAT_OPERATION_FAILURE
             dwCause: failure cause
==============================================================================*/
void AtiSms_CommonCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause);


/*==============================================================================
 Description: response of +CMGS PDU mode
 Parameter:
==============================================================================*/
void AtiSms_CmgsPduSendCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t bTpMsgRef);


/*==============================================================================
 Description: response of +CMSS
 Parameter:
==============================================================================*/
void AtiSms_CmssSendCnf(uint8_t bChannelId,uint8_t bResult,uint32_t dwCause,uint8_t bTpMsgRef);


/*==============================================================================
 Description: Unsoicited report MT sms stored position, +CDSI, +CMTI
 Parameter:
==============================================================================*/
void AtiSms_RecvStoreMsgInd(uint8_t bMsgType,uint8_t bMem,uint16_t wIndex);


/*==============================================================================
 Description: Unsoicited report +CDS  report status SMS pdu
 Parameter:
==============================================================================*/
void AtiSms_StatusRecvPduInd(uint8_t bPduLen,uint8_t *pbPduBuf);


/*==============================================================================
 Description: Unsoicited report MT sms pdu
 Parameter:
            bNcellNum       cell count;
            ptNcellInfo     cellinfo list
==============================================================================*/
void AtiSms_DeliverRecvPduInd(uint8_t bPduLen,uint8_t *pbPduBuf);


/*==============================================================================
 Description: query CNMI parameter from SMS
 Parameter:
==============================================================================*/
extern uint32_t AtiSms_CnmiQueryReq(S_AtiSms_CnmiQueryReq *ptCnmiQuery);

/*==============================================================================
 Description: query CSMP parameter from SMS
 Parameter:
==============================================================================*/
#ifdef TEXT_SMS_SUPPORT
extern uint32_t AtiSms_CsmpQueryReq(S_AtiSms_CsmpQueryReq *ptCsmpQuery);
#endif

/*==============================================================================
 Description: Check is there one default pdp context using
 Parameter:
==============================================================================*/
extern uint8_t Esm_IsDefPdpContextUsing(uint8_t bCid);

/*==============================================================================
 Description: response for at+cgtft?
 Parameter:out  ptTftInfo;
==============================================================================*/
void Esm_GetTft(S_AtiEsm_TftInfo *ptTftInfo);


/*==============================================================================
 Description: response for at+cgeqos?
 Parameter:out  ptEqos;
==============================================================================*/
void Esm_GetEpsQos(S_AtiEsm_QueryEpsQos_Result *ptEqos);


/*==============================================================================
 Description: Get pdp info from ESM
 Parameter:
==============================================================================*/
uint8_t Esm_GetPdpInfoByCid( uint8_t bCid, S_AtiEsm_PdpAddr *ptApAtPdpAddr, S_Esm_PcoInfo *ptApAtPcoInfo);

/*==============================================================================
 Description: success response for +ncell
 Parameter:
            bNcellNum       cell count;
            ptNcellInfo     cellinfo list
==============================================================================*/
uint32_t Esm_GetCauseValue(void);


/*==============================================================================
 Description: get cfun info
 Parameter:
==============================================================================*/
uint8_t Nas_CfunGet(void);


/*==============================================================================
 Description: get cops set info
 Parameter: pCurrPlmnid   current PlmnId
 Return: R_VALID /R_INVALID
==============================================================================*/
uint8_t Emm_CopsGetCurrPlmnId(S_PS_PlmnId *pCurrPlmnid);


/*==============================================================================
 Description: get cgatt state info
 Parameter: return state
==============================================================================*/
uint8_t Emm_CgattGet(void);


/*==============================================================================
 Description: get ceer info
 Parameter: return dwCause;
==============================================================================*/
uint8_t Emm_CauseGet(void);


/*==============================================================================
 Description: get cereg info
 Parameter: return CeregState;
==============================================================================*/
uint8_t Emm_CeregStaGet(void);


/*==============================================================================
 Description: Unsolicited SS EDRX Parameter info
 Parameter:
==============================================================================*/
void Emm_EdrxInfo(uint8_t* bEdrxflag, uint8_t* bNwEdrxValue, uint8_t* bPageTimeValue);


/*==============================================================================
 Description: Check earfcn whether supported
 Parameter:
==============================================================================*/
extern uint8_t Rrc_IsSupportEarfcn(uint32_t dwArfcn);


/*==============================================================================
 Description: success response for +ncell
 Parameter:
            bNcellNum       cell count;
            ptNcellInfo     cellinfo list
==============================================================================*/
extern int32_t PsNet_UlDataSend(uint8_t cid, uint8_t SendPri, uint8_t *pbPayloadAddr, uint16_t iPayloadLen);

/*==============================================================================
 Description: success response for +ncell
 Parameter:
            ptUeInfo       output info;
 return:    0:succ  1: wrong state 2: ReadSib1 fail
==============================================================================*/
uint8_t Rrc_GetUEStats(S_AT_UEStats *ptUeInfo);

/*==============================================================================
 Description: success response for +cmsinr
 Parameter:
            point to struct S_AtiRrc_ScellSinr
 return:    NULL
==============================================================================*/
void AtiRrc_GetScellSinr(S_AtiRrc_ScellSinr *ptScellSinr);

/*   FUNCTION-DESCRIPTION:    ati_GetUEStats
** DESCRIPTION:     Interface to Module to get UEStats
** PARAMETERS:      ptUeInfo: pointer of the input parameter to get Ue Info.
**                  Get the valid values when UE in idle or connected State
*                   Get invalid valuse when in other State  or missing SIB1
** RETURN VALUES:   0:SUCCESS,1:Wrong UE State,2:Missing SIB1
*/
uint8_t ati_GetUEStats(S_AT_UEStats *ptUeInfo);

/*==============================================================================
 Description: get Scell Rsrq
 Parameter:
 return:    rsrq
==============================================================================*/
uint8_t Hmbc_GetScellRsrq(VOID);

/*==============================================================================
 Description: get Scell Rsrp
 Parameter:
 return:    rsrp
==============================================================================*/
uint8_t Hmbc_GetScellRsrp(VOID);

/*==============================================================================
 Description: get PS module threader Id
 Parameter:
 return:    threader id
==============================================================================*/
extern osThread_t Ps_getThreader(uint8_t bDestThreadIdx);

/*==============================================================================
 Description: whether the ptFstPlmn and ptSndPlmn are the same
 Parameter:
 return:    R_VALID/R_INVALID
==============================================================================*/
extern uint8_t Emm_IsPlmnIdEqual(S_PS_PlmnId ptFstPlmn, S_PS_PlmnId ptSndPlmn);


extern uint16_t Usim_SmsCvtUsimErrorToSmsError(uint16_t wRst);

/*==============================================================================
 Description: Unsolicited report the signalling connection status
 Parameter:
==============================================================================*/
void AtiNasCsconInd(uint8_t bMode);

/*==============================================================================
 Description: get the signalling connection status
 Parameter:
 return:    signalling connection status
==============================================================================*/
uint8_t Emm_GetMode(void);

extern void Pdcp_ProUlApNoData();

/*--------------------------------------------------------------------------------------------
函数名称：Api_GetImsi -- 获取IMSI的接口
函数入参：char* pImsiPtr
         -- 需要调用方申请一个不小于16字节的空间作为入参传入,函数返回获取的信息会写入pImsiPtr
函数返回值： 0-表示返回成功；其他值表示获取IMSI失败
---------------------------------------------------------------------------------------------*/
/*please make sure the length of the input buf at least 16BYTE*/
SINT32 Api_GetImsi(char* pImsiPtr);

/*---------------------------------------------------------------------------------------------
函数名称：Api_GetImei -- 获取IMEI的接口
函数入参：char* pImsiPtr
         -- 需要调用方申请一个不小于16字节的空间作为入参传入,函数返回获取的信息会写入pImeiPtr
函数返回值：0-表示返回成功；其他值表示获取IMEI失败
---------------------------------------------------------------------------------------------*/
/*please make sure the length of the input buf at least 16BYTE*/
SINT32 Api_GetImei(char* pImeiPtr);

#ifdef IMS_SUPPORT
/*==============================================================================
 Description: 查询bar状态
 Parameter:
            uint8_t bMode;  //0:reserved, 1: voice; 2: video; 3: sms; 4: emergency
 return:    0: not bar, 1:bar
==============================================================================*/
uint8_t Rrc_GetBarStatus(uint8_t bMode);
#endif

/*==============================================================================
 Description: Get Pdp Ctx type;
                   if Pdp Ctx activated and the pdp Ctx is default Pdp Ctx, return the pdp type of the pdp Ctx
                   if Pdp Ctx not activated  and the Pdp Ctx defined as default pdp ctx, return the define pdp ctx pdp type
                   if the Pdp Ctx not defined or defined as second pdp ctx, return fail
 Parameter:
            uint8_t bCid;  //bCid:1~8
 return:    0: pdp ctx not defined or pdp ctx defined as second pdp ctx
               1: IPV4
               2: IPV6
               3: IPV4V6
==============================================================================*/
UINT8 Api_GetPdpType(uint8_t bCid);

/*==============================================================================
 Description: send msg to ati function
 Parameter:
==============================================================================*/
uint8_t ati_SendMsgToAti(uint8_t channelId,uint16_t wMsgId, void *pMsg);

/*==============================================================================
 Description: ati msg entry
 Parameter:
==============================================================================*/
uint8_t Ati_Entry(uint8_t bChannelId, void* pMessage);

/*==============================================================================
 Description: get current ati cmd channel id
 Parameter:
==============================================================================*/
uint8_t Ati_GetCurCmdChId(void);

/*---------------------------------------------------------------------------------------
函数名称：api_GetRaFailResult -- 获取随机接入失败的原因                                
函数入参：无                                                                     
函数返回值：uint8_t --- 见下面宏定义                                         
---------------------------------------------------------------------------------------*/
#define RA_FAIL_RESULT_NO_FAIL              0
#define RA_FAIL_RESULT_PREAM_TRANS_MAX      1
#define RA_FAIL_RESULT_CR_TIMER_EXIPRE      2
#define RA_FAIL_RESULT_CR_COMPARE_FAIL      3
#define RA_FAIL_RESULT_MSG5_MAX_RETX        4
extern uint8_t api_GetRaFailResult();

/*---------------------------------------------------------------------------------------
函数名称：api_GetNetRejectCauseInfo -- 获取网络拒绝原因信息                            
函数入参：S_Ati_NetCauseInfo *pNetCauseInfo                                             
          -- 需要调用方申请内存作为入参传入，函数返回获取的信息会写入pNetCauseInfo     
函数返回值：VOID                                                                       
---------------------------------------------------------------------------------------*/
extern VOID api_GetNetRejectCauseInfo(S_Ati_NetCauseInfo *pNetCauseInfo);

/*---------------------------------------------------------------------------------------
函数名称：api_GetNetInfo -- 获取网络小区信息                           
函数入参：S_Ati_CellInfo *pNetInfo                                                      
          --需要调用方申请内存作为入参传入，函数返回获取的信息会写入pNetInfo           
函数返回值：VOID                                                                       
---------------------------------------------------------------------------------------*/
extern VOID api_GetNetInfo(S_Ati_CellInfo *pNetInfo);

/*---------------------------------------------------------------------------------------
函数名称：Api_GetConnStatus -- 获取小区链路状态                           
函数入参：无                                                      
函数返回值：uint8_t -- 返回小区链路状态 0  IDLE, 1 CONNECT                             
---------------------------------------------------------------------------------------*/
extern uint8_t Api_GetConnStatus(void);

/*---------------------------------------------------------------------------------------
函数名称：Api_GetIpDnsInfo -- 获取IP地址信息                           
函数入参：S_Ati_SmIpDnsInfo *ptInfo                                                      
          --需要调用方申请内存作为入参传入，函数返回获取的信息会写入ptInfo           
函数返回值：bool_t        TRUE or FALSE                                                                      
---------------------------------------------------------------------------------------*/
extern bool_t Api_GetIpDnsInfo(uint8_t bCid, S_Ati_SmIpDnsInfo *ptInfo);

/*---------------------------------------------------------------------------------------
函数名称：Api_GetCsq -- 获取信号强度信息
函数入参：无
函数返回值：uint8_t    取值范围:0~31,99 99表示未知或不可测
---------------------------------------------------------------------------------------*/
extern uint8_t Api_GetCsq(void);

/*--------------------------------------------------------------------------------------------
函数名称：Api_GetIccid -- 获取ICCID的接口
函数入参：char* pIccidStr
         -- 需要调用方申请一个不小于21字节的空间作为入参传入,函数返回获取的信息会写入pIccidStr
函数返回值： 0-表示返回成功；其他值表示获取ICCID失败
---------------------------------------------------------------------------------------------*/
extern SINT32 Api_GetIccid(char* pIccidStr);

/*---------------------------------------------------------------------------------------
函数名称：api_GetLteBand -- 获取锁定的band                           
函数入参：无                                                      
函数出参：uint8_t *pbBand: 获取的band，按支持的band值置位对应bit
          uint8_t bBandInBytes: 接口调用者申请的pbBand buff的字节长度
          uint8_t *pbBandOutBytes: 存入pbBand有效的字节长度
函数返回值：R_FAIL/R_SUCC

说明：以pbBand[0]为例，输入参数按以下顺序。
Bit7    Bit6    Bit5    Bit4    Bit3    Bit2    Bit1    Bit0
Band8   Band7   Band6   Band5   Band4   Band3   Band2   Band1

pbBand[1] 对应 band9-band16,以此类推。
---------------------------------------------------------------------------------------*/
extern uint8_t api_GetLteBand(uint8_t *pbBand, uint8_t bBandInBytes, uint8_t *pbBandOutBytes);

/*---------------------------------------------------------------------------------------
函数名称：api_SetLteBand -- 设置锁定的band                           
函数入参：无                                                      
函数出参：uint8_t bResetBand: 重置支持的band值
          uint8_t *pbBand: 设置的band，按支持的band值置位对应bit
          uint8_t *pbBandBytes: pbBand有效的字节长度
函数返回值：R_FAIL/R_SUCC

说明：以pbBand[0]为例，输入参数按以下顺序。
Bit7    Bit6    Bit5    Bit4    Bit3    Bit2    Bit1    Bit0
Band8   Band7   Band6   Band5   Band4   Band3   Band2   Band1

pbBand[1] 对应 band9-band16,以此类推。
---------------------------------------------------------------------------------------*/
extern uint8_t api_SetLteBand(uint8_t bResetBand, uint8_t *pbBand, uint8_t bBandBytes);

/*---------------------------------------------------------------------------------------
函数名称：Api_GetNetInfoState -- QENG指令<state>状态查询                           
函数入参：无                                                      
函数出参：无
函数返回值：0:search 1:limsrv 2:noconn 3:connect
---------------------------------------------------------------------------------------*/
extern uint8_t Api_GetNetInfoState(void);

#endif


