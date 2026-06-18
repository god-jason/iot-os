/****************************************************************************************************
 * Copyright (c) 2023, 芯昇科技有限公司
 * 
 * @file        l3cdec_header.h
 *
 * @brief       defined NAS IE and Message struct base on 24301 and 24008
 *
 * @revision
 * Date                   Author            Notes
 * 2023-4-20
*****************************************************************************************************/

/*====================================================================
* macro defination
*=====================================================================*/

#ifndef __PS_L3CDEC_HEADER_H_
#define __PS_L3CDEC_HEADER_H_

#include "ps.h"

/*********************L3 MSG TYPE DEFINATION********************/
/*L3_PD_TC*/

#define L3_MsgLteCloseLoop               (uint8_t)128
#define L3_MsgLteCloseLoopCmplt          (uint8_t)129
#define L3_MsgLteOpenLoop                (uint8_t)130
#define L3_MsgLteOpenLoopCmpt            (uint8_t)131
#define L3_MsgLteActTestMode             (uint8_t)132
#define L3_MsgLteActTestModeCmpt         (uint8_t)133
#define L3_MsgLteDeActTestMode           (uint8_t)134
#define L3_MsgLteDeActTestModeCmpt       (uint8_t)135
/*L3_PD_SMS*/
#define L3_MsgCpData                     (uint8_t)1
#define L3_MsgCpAck                      (uint8_t)4
#define L3_MsgCpError                    (uint8_t)16

#define L3_MsgRpDataMO                   (uint8_t)0
#define L3_MsgRpDataMT                   (uint8_t)1
#define L3_MsgRpAckMT                    (uint8_t)2
#define L3_MsgRpAckMO                    (uint8_t)3
#define L3_MsgRpErrorMT                  (uint8_t)4
#define L3_MsgRpErrorMO                  (uint8_t)5
#define L3_MsgRpSmma                     (uint8_t)6

#define L3_MsgSMSpd                      (uint8_t)9
/*L3_PD_EMM*/
#define L3_MsgAttachaccept               (uint8_t)66
#define L3_MsgAttachcomplete             (uint8_t)67
#define L3_MsgAttachreject               (uint8_t)68
#define L3_MsgAttachrequest              (uint8_t)65
#define L3_MsgAuthfailure                (uint8_t)92
#define L3_MsgAuthreject                 (uint8_t)84
#define L3_MsgAuthrequest                (uint8_t)82
#define L3_MsgAuthresponse               (uint8_t)83
#define L3_MsgCSsrvnoti                  (uint8_t)100
#define L3_MsgDetachaccept               (uint8_t)70
#define L3_MsgDetachrequestMO            (uint8_t)69
#define L3_MsgDetachrequestMT            (uint8_t)69
#define L3_MsgDlNASTrans                 (uint8_t)98
#define L3_MsgUlNASTrans                 (uint8_t)99
#define L3_MsgEMMinfo                    (uint8_t)97
#define L3_MsgEMMstatus                  (uint8_t)96
#define L3_MsgExtsrvreq                  (uint8_t)76
#define L3_MsgCpsrvreq                   (uint8_t)77
#define L3_MsgGUTIrllctncmmnd            (uint8_t)80
#define L3_MsgGUTIrllctncmp              (uint8_t)81
#define L3_MsgIdnttyrequest              (uint8_t)85
#define L3_MsgIdnttyresponse             (uint8_t)86
#define L3_MsgSecmodecmd                 (uint8_t)93
#define L3_MsgScmdcomp                   (uint8_t)94
#define L3_MsgScmdrej                    (uint8_t)95
#define L3_MsgSrvrej                     (uint8_t)78
#define L3_MsgSrvaccept                  (uint8_t)79
#define L3_MsgTauaccept                  (uint8_t)73
#define L3_MsgTaucmp                     (uint8_t)74
#define L3_MsgTaurej                     (uint8_t)75
#define L3_MsgTaureq                     (uint8_t)72
#define L3_MsgDlGenNasTrans              (uint8_t)104
#define L3_MsgUlGenNasTrans              (uint8_t)105

/*L3_PD_ESM*/
#define L3_MsgActDedBcntAccept           (uint8_t)198
#define L3_MsgActDedBcntReject           (uint8_t)199
#define L3_MsgActDedBcntReq              (uint8_t)197
#define L3_MsgActDefauBcntAccept         (uint8_t)194
#define L3_MsgActDefauBcntReject         (uint8_t)195
#define L3_MsgActDefauBcntReq            (uint8_t)193
#define L3_MsgBResAlloReject             (uint8_t)213
#define L3_MsgBResAlloReq                (uint8_t)212
#define L3_MsgBResModifReject            (uint8_t)215
#define L3_MsgBResModifReq               (uint8_t)214
#define L3_MsgDeActBcntAccept            (uint8_t)206
#define L3_MsgDeActBcntRequest           (uint8_t)205
#define L3_MsgESMInfoRequest             (uint8_t)217
#define L3_MsgESMInfoResponse            (uint8_t)218
#define L3_MsgESMStatus                  (uint8_t)232
#define L3_MsgModifBcntAccept            (uint8_t)202
#define L3_MsgModifBcntReject            (uint8_t)203
#define L3_MsgModifBcntReq               (uint8_t)201
#define L3_MsgEsmNotification            (uint8_t)219
#define L3_MsgEsmDummyMsg                (uint8_t)220
#define L3_MsgPDNConnReject              (uint8_t)209
#define L3_MsgPDNConnReq                 (uint8_t)208
#define L3_MsgPDNDConnReject             (uint8_t)211
#define L3_MsgPDNDConnRequest            (uint8_t)210
#define L3_MsgEsmDataTransport           (uint8_t)235

/*L3_PD_LTETC*/

/*********************IE TYPE DEFINATION**********************/

#define IE_TYPE_V                              0
#define IE_TYPE_V_NONCE                        1 //Nonce
#define IE_TYPE_V_LAI                          2
#define IE_TYPE_V_HALF                         3

#define IE_TYPE_LV                             4
#define IE_TYPE_LV_TFT                         5  //Traffic Flow Template
#define IE_TYPE_LV_TAILST                      6  //Tracking Area Id List
#define IE_TYPE_LV_EMOBID                      7  //EPS mobile identity
#define IE_TYPE_LV_MOBID                       8  //Mobile_Identity
#define IE_TYPE_LV_PDNADDR                     9  //PDN Address
#define IE_TYPE_LV_SECCAP                      10 //UE security capability
#define IE_TYPE_LV_NWCAP                       11 //UE network capability
#define IE_TYPE_LV_EQOS                        12 //EPS quality of service
#define IE_TYPE_LV_PLMNLST                     13 //PLMN List
#define IE_TYPE_LV_EMERLST                     14 //Emergency Number List
#define IE_TYPE_LV_NETNAME                     15 //Net name
#define IE_TYPE_LV_HDRCPRS                     16 //Header Compress Config
#define IE_TYPE_LV_PCO                         17 //Extended Prot Config Options
#define IE_TYPE_LV_QOS                         18 //Quality of service
#define IE_TYPE_LV_AMBR                        19 //APN aggregate maximum bit rate
#define IE_TYPE_LV_TI                          20 //Linked TI

#define IE_TYPE_LVE                            21
#define IE_TYPE_LVE_CONT                       22


#define IE_TYPE_MVMLV_CLOSETCLP                30 //CLOSE TC LOOP

/********************* MSG & IE MACRO DEFINATION**********************/
//EMM MESSAGE
#define IE_DETATCH_ACCPT_ORI_START                 0
#define IE_DETATCH_ACCPT_ORI_IE_NUM                1
#define IE_TAU_COMPLETE_START                      0
#define IE_TAU_COMPLETE_IE_NUM                     1
#define IE_GUTI_REALLOC_CMPLT_START                0
#define IE_GUTI_REALLOC_CMPLT_IE_NUM               1
#define IE_AUTH_REJECT_START                       0
#define IE_AUTH_REJECT_IE_NUM                      1
#define IE_IDENTITY_REQ_START                      1
#define IE_IDENTITY_REQ_IE_NUM                     1
#define IE_SEC_MODE_REJECT_START                   1
#define IE_SEC_MODE_REJECT_IE_NUM                  1
#define IE_EMM_STATUS_START                        1
#define IE_EMM_STATUS_IE_NUM                       1
#define IE_ATTACH_REQ_START                        (IE_EMM_STATUS_START + IE_EMM_STATUS_IE_NUM)
#define IE_ATTACH_REQ_IE_NUM                       17
#define IE_ATTACH_ACCEPT_START                     (IE_ATTACH_REQ_START + IE_ATTACH_REQ_IE_NUM)
#define IE_ATTACH_ACCEPT_IE_NUM                    17
#define IE_ATTACH_COMPLETE_START                   (IE_ATTACH_ACCEPT_START + IE_ATTACH_ACCEPT_IE_NUM)
#define IE_ATTACH_COMPLETE_IE_NUM                  2
#define IE_ATTACH_REJECT_START                     (IE_ATTACH_COMPLETE_START + IE_ATTACH_COMPLETE_IE_NUM)
#define IE_ATTACH_REJECT_IE_NUM                    5
#define IE_DETATCH_REQ_TER_START                   (IE_ATTACH_REJECT_START + IE_ATTACH_REJECT_IE_NUM)
#define IE_DETATCH_REQ_TER_IE_NUM                  2
#define IE_DETACH_REQ_ORI_START                    (IE_DETATCH_REQ_TER_START + IE_DETATCH_REQ_TER_IE_NUM)
#define IE_DETACH_REQ_ORI_IE_NUM                   2
#define IE_TAU_REQ_START                           (IE_DETACH_REQ_ORI_START + IE_DETACH_REQ_ORI_IE_NUM)
#define IE_TAU_REQ_IE_NUM                          17
#define IE_TAU_ACCEPT_START                        (IE_TAU_REQ_START + IE_TAU_REQ_IE_NUM)
#define IE_TAU_ACCEPT_IE_NUM                       19
#define IE_TAU_REJECT_START                        (IE_TAU_ACCEPT_START + IE_TAU_ACCEPT_IE_NUM)
#define IE_TAU_REJECT_IE_NUM                       3
#define IE_EXT_SERV_REQ_START                      (IE_TAU_REJECT_START + IE_TAU_REJECT_IE_NUM)
#define IE_EXT_SERV_REQ_IE_NUM                     5
#define IE_CP_SERV_REQ_START                       (IE_EXT_SERV_REQ_START + IE_EXT_SERV_REQ_IE_NUM)
#define IE_CP_SERV_REQ_IE_NUM                      5
#define IE_SERV_REJECT_START                       (IE_CP_SERV_REQ_START + IE_CP_SERV_REQ_IE_NUM)
#define IE_SERV_REJECT_IE_NUM                      3
#define IE_SERV_ACCEPT_START                       (IE_SERV_REJECT_START + IE_SERV_REJECT_IE_NUM)
#define IE_SERV_ACCEPT_IE_NUM                      2
#define IE_GUTI_REALLOC_CMD_START                  (IE_SERV_ACCEPT_START + IE_SERV_ACCEPT_IE_NUM)
#define IE_GUTI_REALLOC_CMD_IE_NUM                 3
#define IE_AUTH_REQ_START                          (IE_GUTI_REALLOC_CMD_START + IE_GUTI_REALLOC_CMD_IE_NUM)
#define IE_AUTH_REQ_IE_NUM                         2
#define IE_AUTH_RSP_START                          (IE_AUTH_REQ_START + IE_AUTH_REQ_IE_NUM)
#define IE_AUTH_RSP_IE_NUM                         2
#define IE_IDENTITY_RSP_START                      (IE_AUTH_RSP_START + IE_AUTH_RSP_IE_NUM)
#define IE_IDENTITY_RSP_IE_NUM                     2
#define IE_AUTH_FAILURE_START                      (IE_IDENTITY_RSP_START + IE_IDENTITY_RSP_IE_NUM)
#define IE_AUTH_FAILURE_IE_NUM                     2
#define IE_SEC_MODE_CMD_START                      (IE_AUTH_FAILURE_START + IE_AUTH_FAILURE_IE_NUM)
#define IE_SEC_MODE_CMD_IE_NUM                     5
#define IE_SEC_MODE_CMPLT_START                    (IE_SEC_MODE_CMD_START + IE_SEC_MODE_CMD_IE_NUM)
#define IE_SEC_MODE_CMPLT_IE_NUM                   2
#define IE_EMM_INFO_START                          (IE_SEC_MODE_CMPLT_START + IE_SEC_MODE_CMPLT_IE_NUM)
#define IE_EMM_INFO_IE_NUM                         6
#define IE_DL_NAS_TRANS_START                      (IE_EMM_INFO_START + IE_EMM_INFO_IE_NUM)
#define IE_DL_NAS_TRANS_IE_NUM                     2
#define IE_UL_NAS_TRANS_START                      (IE_DL_NAS_TRANS_START + IE_DL_NAS_TRANS_IE_NUM)
#define IE_UL_NAS_TRANS_IE_NUM                     2
#define IE_DL_GEN_NAS_TRANS_START                  (IE_UL_NAS_TRANS_START + IE_UL_NAS_TRANS_IE_NUM)
#define IE_DL_GEN_NAS_TRANS_IE_NUM                 3
#define IE_UL_GEN_NAS_TRANS_START                  (IE_DL_GEN_NAS_TRANS_START + IE_DL_GEN_NAS_TRANS_IE_NUM)
#define IE_UL_GEN_NAS_TRANS_IE_NUM                 3

//ESM MESSAGE
#define IE_ESM_INFO_REQ_START                      0
#define IE_ESM_INFO_REQ_IE_NUM                     1
#define IE_ESM_DUMMY_START                         0
#define IE_ESM_DUMMY_IE_NUM                        1
#define IE_ACT_DEF_CNTXT_REQ_START                 (IE_ESM_DUMMY_START + IE_ESM_DUMMY_IE_NUM)
#define IE_ACT_DEF_CNTXT_REQ_IE_NUM                12
#define IE_ACT_DEF_CNTXT_ACCEPT_START              (IE_ACT_DEF_CNTXT_REQ_START + IE_ACT_DEF_CNTXT_REQ_IE_NUM)
#define IE_ACT_DEF_CNTXT_ACCEPT_IE_NUM             2
#define IE_ACT_DEF_CNTXT_REJECT_START              (IE_ACT_DEF_CNTXT_ACCEPT_START + IE_ACT_DEF_CNTXT_ACCEPT_IE_NUM)
#define IE_ACT_DEF_CNTXT_REJECT_IE_NUM             2
#define IE_ACT_DED_BCNT_REQ_START                  (IE_ACT_DEF_CNTXT_REJECT_START + IE_ACT_DEF_CNTXT_REJECT_IE_NUM)
#define IE_ACT_DED_BCNT_REQ_IE_NUM                 9
#define IE_ACT_DED_BCNT_ACCEPT_START               (IE_ACT_DED_BCNT_REQ_START + IE_ACT_DED_BCNT_REQ_IE_NUM)
#define IE_ACT_DED_BCNT_ACCEPT_IE_NUM              2
#define IE_ACT_DED_BCNT_REJECT_START               (IE_ACT_DED_BCNT_ACCEPT_START + IE_ACT_DED_BCNT_ACCEPT_IE_NUM)
#define IE_ACT_DED_BCNT_REJECT_IE_NUM              2
#define IE_MODIFY_CTX_REQ_START                    (IE_ACT_DED_BCNT_REJECT_START + IE_ACT_DED_BCNT_REJECT_IE_NUM)
#define IE_MODIFY_CTX_REQ_IE_NUM                   9
#define IE_MODIFY_CTX_ACCEPT_START                 (IE_MODIFY_CTX_REQ_START + IE_MODIFY_CTX_REQ_IE_NUM)
#define IE_MODIFY_CTX_ACCEPT_IE_NUM                2
#define IE_MODIFY_CTX_REJECT_START                 (IE_MODIFY_CTX_ACCEPT_START + IE_MODIFY_CTX_ACCEPT_IE_NUM)
#define IE_MODIFY_CTX_REJECT_IE_NUM                2
#define IE_DEACT_CTX_REQ_START                     (IE_MODIFY_CTX_REJECT_START + IE_MODIFY_CTX_REJECT_IE_NUM)
#define IE_DEACT_CTX_REQ_IE_NUM                    3
#define IE_DEACT_CTX_ACCEPT_START                  (IE_DEACT_CTX_REQ_START + IE_DEACT_CTX_REQ_IE_NUM)
#define IE_DEACT_CTX_ACCEPT_IE_NUM                 2
#define IE_PDN_CONNCT_REQ_START                    (IE_DEACT_CTX_ACCEPT_START + IE_DEACT_CTX_ACCEPT_IE_NUM)
#define IE_PDN_CONNCT_REQ_IE_NUM                   5
#define IE_PDN_CONNCT_REJECT_START                 (IE_PDN_CONNCT_REQ_START + IE_PDN_CONNCT_REQ_IE_NUM)
#define IE_PDN_CONNCT_REJECT_IE_NUM                4
#define IE_PDN_DISCONNCT_REQ_START                 (IE_PDN_CONNCT_REJECT_START + IE_PDN_CONNCT_REJECT_IE_NUM)
#define IE_PDN_DISCONNCT_REQ_IE_NUM                2
#define IE_PDN_DISCONNCT_REJECT_START              (IE_PDN_DISCONNCT_REQ_START + IE_PDN_DISCONNCT_REQ_IE_NUM)
#define IE_PDN_DISCONNCT_REJECT_IE_NUM             2
#define IE_BEAR_RESRC_ALLO_REQ_START               (IE_PDN_DISCONNCT_REJECT_START + IE_PDN_DISCONNCT_REJECT_IE_NUM)
#define IE_BEAR_RESRC_ALLO_REQ_IE_NUM              5
#define IE_BEAR_RESRC_ALLO_REJECT_START            (IE_BEAR_RESRC_ALLO_REQ_START + IE_BEAR_RESRC_ALLO_REQ_IE_NUM)
#define IE_BEAR_RESRC_ALLO_REJECT_IE_NUM           4
#define IE_BEAR_RESRC_MODIFY_REQ_START             (IE_BEAR_RESRC_ALLO_REJECT_START + IE_BEAR_RESRC_ALLO_REJECT_IE_NUM)
#define IE_BEAR_RESRC_MODIFY_REQ_IE_NUM            6
#define IE_BEAR_RESRC_MODIFY_REJECT_START          (IE_BEAR_RESRC_MODIFY_REQ_START + IE_BEAR_RESRC_MODIFY_REQ_IE_NUM)
#define IE_BEAR_RESRC_MODIFY_REJECT_IE_NUM         4
#define IE_ESM_INFO_RSP_START                      (IE_BEAR_RESRC_MODIFY_REJECT_START + IE_BEAR_RESRC_MODIFY_REJECT_IE_NUM)
#define IE_ESM_INFO_RSP_IE_NUM                     3
#define IE_ESM_NOTI_START                          (IE_ESM_INFO_RSP_START + IE_ESM_INFO_RSP_IE_NUM)
#define IE_ESM_NOTI_IE_NUM                         2
#define IE_ESM_STATUS_START                        (IE_ESM_NOTI_START + IE_ESM_NOTI_IE_NUM)
#define IE_ESM_STATUS_IE_NUM                       1
#define IE_ESM_DATA_TRANS_START                    (IE_ESM_STATUS_START + IE_ESM_STATUS_IE_NUM)
#define IE_ESM_DATA_TRANS_IE_NUM                   3

//TC MESSAGE
#define IE_CLOSE_UE_TEST_LOOP_START                0
#define IE_CLOSE_UE_TEST_LOOP_IE_NUM               2
#define IE_CLOSE_UE_TEST_LOOP_COMPLETE_START       (IE_CLOSE_UE_TEST_LOOP_START + IE_CLOSE_UE_TEST_LOOP_IE_NUM)
#define IE_CLOSE_UE_TEST_LOOP_COMPLETE_IE_NUM      1
#define IE_OPEN_UE_TEST_LOOP_START                 (IE_CLOSE_UE_TEST_LOOP_START + IE_CLOSE_UE_TEST_LOOP_IE_NUM)
#define IE_OPEN_UE_TEST_LOOP_IE_NUM                1
#define IE_OPEN_UE_TEST_LOOP_COMPLETE_START        (IE_CLOSE_UE_TEST_LOOP_START + IE_CLOSE_UE_TEST_LOOP_IE_NUM)
#define IE_OPEN_UE_TEST_LOOP_COMPLETE_IE_NUM       1
#define IE_DEACTIVATE_TEST_MODE_COMPLETE_START     (IE_CLOSE_UE_TEST_LOOP_START + IE_CLOSE_UE_TEST_LOOP_IE_NUM)
#define IE_DEACTIVATE_TEST_MODE_COMPLETE_IE_NUM    1
#define IE_DEACTIVATE_TEST_MODE_START              (IE_CLOSE_UE_TEST_LOOP_START + IE_CLOSE_UE_TEST_LOOP_IE_NUM)
#define IE_DEACTIVATE_TEST_MODE_IE_NUM             1
#define IE_ACTIVATE_TEST_MODE_COMPLETE_START       (IE_CLOSE_UE_TEST_LOOP_START + IE_CLOSE_UE_TEST_LOOP_IE_NUM)
#define IE_ACTIVATE_TEST_MODE_COMPLETE_IE_NUM      1
#define IE_ACTIVATE_TEST_MODE_START                (IE_ACTIVATE_TEST_MODE_COMPLETE_START + IE_ACTIVATE_TEST_MODE_COMPLETE_IE_NUM)
#define IE_ACTIVATE_TEST_MODE_IE_NUM               1

//SMS MESSAGE
#define IE_CP_DATA_START                           0
#define IE_CP_DATA_IE_NUM                          2
#define IE_CP_ACK_START                            (IE_CP_DATA_START + IE_CP_DATA_IE_NUM)
#define IE_CP_ACK_IE_NUM                           1
#define IE_CP_ERROR_START                          (IE_CP_ACK_START + IE_CP_ACK_IE_NUM)
#define IE_CP_ERROR_IE_NUM                         1

#define IE_RP_DATA_MO_START                        0
#define IE_RP_DATA_MO_IE_NUM                       4
#define IE_RP_DATA_MT_START                        (IE_RP_DATA_MO_START + IE_RP_DATA_MO_IE_NUM)
#define IE_RP_DATA_MT_IE_NUM                       4
#define IE_RP_ACK_MT_START                         (IE_RP_DATA_MT_START + IE_RP_DATA_MT_IE_NUM)
#define IE_RP_ACK_MT_IE_NUM                        2
#define IE_RP_ACK_MO_START                         (IE_RP_DATA_MT_START + IE_RP_DATA_MT_IE_NUM)
#define IE_RP_ACK_MO_IE_NUM                        2
#define IE_RP_ERROR_MT_START                       (IE_RP_ACK_MO_START + IE_RP_ACK_MO_IE_NUM)
#define IE_RP_ERROR_MT_IE_NUM                      3
#define IE_RP_ERROR_MO_START                       (IE_RP_ACK_MO_START + IE_RP_ACK_MO_IE_NUM)
#define IE_RP_ERROR_MO_IE_NUM                      3
#define IE_RP_SMMA_START                           (IE_RP_ERROR_MO_START + IE_RP_ERROR_MO_IE_NUM)
#define IE_RP_SMMA_IE_NUM                          1


/*********************ARRAY MAX INDEX DEFINATION**********************/
#define L3_MAX_RAIVAL_NUM                       (uint8_t)6
#define L3_MAX_AUTHRSPPARAEXT_NUM               (uint8_t)12
#define L3_MAX_LSAID_NUM                        (uint8_t)3
#define L3_MAX_SPCHIND                          (uint8_t)12
#define L3_MAX_CALLEDBCDNUM_NUM                 (uint8_t)40
#define L3_MAX_CALLEDSUBADDR_NUM                (uint8_t)20
#define L3_MAX_CALLINGSUBADDR_NUM               (uint8_t)20
#define L3_MAX_CAUSE_NUM                        (uint8_t)27
#define L3_MAX_CONNNUM_NUM                      (uint8_t)11
#define L3_MAX_CONNSUBADDR_NUM                  (uint8_t)20
#define L3_MAX_REDIRBCDNUM_NUM                  (uint8_t)16
#define L3_MAX_REDIRSUBADDR_NUM                 (uint8_t)20
#define L3_MAX_SETUPMSG_NUM                     (uint8_t)10
#define L3_MAX_SSVERSINFO_NUM                   (uint8_t)10
#define L3_MAX_USERINFM_NUM                     (uint8_t)10
#define L3_MAX_RCVNPDUNUMLIST_NUM               (uint8_t)11
#define L3_MAX_INTERRATINFOCON_NUM              (uint8_t)248
#define L3_MAX_EUTRAINTERRATINFOCON_NUM         (uint8_t)255
#define L3_MAX_PDPADDR_NUM                      (uint8_t)20
#define L3_MAX_MBMSCONSTA_NSAPI_NUM             (uint8_t)16
#define L3_PACKET_FILTER_SIZE                   (uint8_t)60
#define L3_PARA_UNIT_LEN                        (uint8_t)128
#define L3_SPNASMSG_LEN                         (uint16_t)1500

#define L3_PACKET_FILTER_NUM                    (uint8_t)15
#define L3_MAX_PROTOIDCONT_NUM                  (uint8_t)134  // =ATSM_PCO_ITEM_VAL_MAX_LEN

#define L3_PARA_UNIT_NUM                        (uint8_t)16
#define L3_MAX_CPUSERDATA_NUM                   (uint8_t)248
#define L3_MAX_RPUSERDATA_NUM                   (uint8_t)232
#define L3_MAX_RPADRS_NUM                       (uint8_t)11
#define L3_MAX_RPCAUS_NUM                       (uint8_t)2
#define L3_MAX_PROTOID_NUM                      (uint8_t)19
#define L3_MAX_APN_NUM                          (uint8_t)100
#define L3_MAX_MOBID_NUM                        (uint8_t)8
#define L3_MAX_PLMN_NUM                         (uint8_t)15
#define L3_MAX_EMERNUM_DIGIT_NUM                (uint8_t)8
#define L3_MAX_EMERNUM_NUM                      (uint8_t)10
#define L3_MAX_CODECBITMAPBITS_NUM              (uint8_t)2
#define L3_MAX_SYSTEMID_NUM                     (uint8_t)50
#define L3_MAX_TXTSTR_NUM                       (uint8_t)218
#define L3_MAX_CALLINGBCDNUM_NUM                (uint8_t)10
#define L3_MAX_E_MOBID_NUM                      (uint8_t)8
#define L3_MAX_TAI_NUM                          (uint8_t)16
#define L3_MAX_RESEPS_NUM                       (uint8_t)16
#define L3_MAX_LCSCLIENTID_NUM                  (uint8_t)255
#define L3_MAX_PDNADDINFO_NUM                   (uint8_t)12
#define L3_MAX_MODEA_LB_ENTITIES_NUM            (uint8_t)8
#define L3_MAX_EPCO_CONTENT_LEN                 (uint16_t)560
#define L3_MAX_ESM_DATA_TRANS_LEN               (uint16_t)560
#define L3_MAX_HEADER_COMPRESS_PARA_LEN         (uint8_t)251

#define  L3_MAX_UeLoopLBLen                     (uint8_t)5

/***********************PD macro defination************************/
#define L3_PD_TC                                (uint8_t)15
#define L3_PD_SMS                               (uint8_t)9

#define L3_PD_ESM                               (uint8_t)2
#define L3_PD_EMM                               (uint8_t)7
#define L3_PD_PDCP                              (uint8_t)16

/*******************L3 message cdec  Error Codes********************/
#define ERR_L3CDEC_UnknowMsgType                (uint8_t)1    /*    MsgType Unknow */
#define ERR_L3CDEC_MandMsg                      (uint8_t)2    /*    Mandatory part error of message */
#define ERR_L3CDEC_MissMandIe                   (uint8_t)3    /*    Miss mandatory IE */
#define ERR_L3CDEC_UnknowIeWithComp             (uint8_t)4    /*    Unknow IE, and comprehension required */
#define ERR_L3CDEC_OptIeSyntacIncorrect         (uint8_t)5    /*    Syntax error of optional IE */
#define ERR_L3CDEC_SemantiMsgContentIncorrect   (uint8_t)6    /*    Semantic error of messgae content */
#define ERR_L3CDEC_MsgShort                     (uint8_t)7    /*    message too short */
#define ERR_L3CDEC_unfrsnEPSbid                 (uint8_t)8    /*    Unknow EPSbid */
#define ERR_L3CDEC_unfrsnPti                    (uint8_t)9    /*    Unknow Pti */
#define ERR_L3CDEC_UnknowSecHead                (uint8_t)10   /*    Unknow SecHead */
#define ERR_L3CDEC_BasicError                   (uint8_t)11   /*    Basic codec error */
#define ERR_L3CDEC_ParamError                   (uint8_t)12   /*    Param error */


typedef struct{
    uint8_t   bTag;              /*option IEI, mandaoty IE is 0*/
    uint8_t   b5IeType:5;         /*ie data type*/
    uint8_t   b3Spare :3;
    uint8_t   bMin;              /*min value in protocol defination*/
}S_L3_MsgIeInfoCut;

typedef struct{
    uint8_t   bTag;              /*option IEI, mandaoty IE is 0*/
    uint8_t   b5IeType:5;         /*ie data type*/
    uint8_t   b3IsSupportedIe:3;  /*IE support flag*/
    uint8_t   bOptIdx;           /*option ie index，calc flag pos*/
    uint8_t   bMin;              /*min value in protocol defination*/
    uint16_t   wMax;              /*max value in protocol defination*/
    uint16_t   wIeStructOffSet;   /*ie offset value in curr struct*/
}S_L3_MsgIeInfo;


typedef struct
{
    uint8_t   bMsgType;            /*MSG TYPE*/
    uint8_t   b5TotalIeNum:5;       /*IE total num in curr msg*/
    uint8_t   b3MandatoryIeNum:3;   /*mandatory IE num in curr msg*/
    uint8_t   bMinMsgLen;          /*the min msg len */
    uint8_t   bIeStartIdx;         /*the offset index in curr ie list */
    uint16_t   wMsgStrSize;         /* curr msg len*/
}S_L3_MsgInfo;

/*ESM MSG Header */
typedef struct
{
    uint8_t  bPd: 4;
    uint8_t  bEPSbid: 4;
    uint8_t  bPti;
    uint8_t  bMsgType;
} S_Esm_L3MsgHeader;

/*24008_10.5.5.6 , DRX_parameter*/
typedef struct
{
    uint8_t                     bSPLITPGCode;
    uint8_t                     bNonDrxTmr  : 3;
    uint8_t                     bSPLITonCCCH  : 1;
    uint8_t                     bLenCoeff  : 4;
} S_L3_DrxPara;

/*24008_10.5.5.28 , Voice_domain_preference_and_UE_usage_setting*/
typedef struct
{
    uint8_t                     bLen;
    uint8_t                     bEUTRANVoiDom  : 2;
    uint8_t                     bUeUsageSet  : 1;
    uint8_t                     bSpare0: 5;
} S_L3_VoiDomUeUsaSet;

/*24008_10.5.7.8 , Device_properties*/
typedef struct
{
    uint8_t                     bLowProFlg : 1;
    uint8_t                     bSpare     : 7;
} S_L3_DeviceProper;
/*24008_10.5.7.4a , GPRS_Timer_3*/
typedef struct
{
    uint8_t                     bTmr3Len;      /*not used*/
    uint8_t                     bTmrVal        : 5;
    uint8_t                     bUnit          : 3;
} S_L3_GprsTimer3;


/*24008_10.5.7.3 , GPRS_Timer*/
typedef struct
{
    uint8_t                     bTmrVal  : 5;
    uint8_t                     bUnit  : 3;
} S_L3_GprsTimer;

/*24008_10.5.6.13 , Temporary_Mobile_Group_Identity*/
typedef struct
{
    uint8_t                     abMBMSSevId[3];
    uint8_t                     bpresentfg1;
    S_PS_PlmnId          tPlmn;
} S_L3_TMGI;

/*24008_10.5.1.4a , Mobile_Identity_TMGI_and_MBMS_SESSION_ID*/
typedef struct
{
    uint8_t                     bIdType  : 3;
    uint8_t                     bSpare3: 1;
    uint8_t                     bMCCMNCInd  : 1;
    uint8_t                     bMBMSSesIdInd  : 1;
    uint8_t                     bSpare0: 2;

    uint8_t                     abMBMSSevId[3];
    uint8_t                     bpresentfg1;
    S_PS_PlmnId                 tPlmn;
    uint8_t                     bpresentfg2;
    uint8_t                     bMBMSSesId;
} S_L3_MobIdTMGISesId;

/*24008_10.5.1.4 , Mobile_Identity_IMSI_TMSI_IMEI*/
typedef struct
{
    uint8_t                     bIdType  : 3;
    uint8_t                     bOorE: 1;
    uint8_t                     bSpare0: 4;

    uint8_t                     bIdNum;
    uint8_t                     abId[L3_MAX_MOBID_NUM];
} S_L3_MobileIdentity;

/*24008_10.5.1.4 , Mobile_Identity*/
typedef struct
{
    uint8_t                     bIdx;
    union
    {
        S_L3_MobileIdentity     tMobileId;
        S_L3_MobIdTMGISesId     tTMGISesId;
    } Val;
} S_L3_MobId;

/*24008_10.5.1.13 , PLMN_List*/
typedef struct
{
    uint8_t                     bPLMNLen;
    S_PS_PlmnId          atPLMN[L3_MAX_PLMN_NUM];
} S_L3_PlmnList;

/*24008_10.5.3.13a , Emergency_Number_information*/
typedef struct
{
    uint8_t                     bSrvCateg  : 5;
    uint8_t                     bSpare0: 3;
    uint8_t                     bNumLen;
    uint8_t                     abNum[L3_MAX_EMERNUM_DIGIT_NUM];
} S_L3_EmerNum;

/*24008_10.5.3.13 , Emergency_Number_List*/
typedef struct
{
    uint8_t                     bEmerNumLen;
    S_L3_EmerNum         atEmerNum[L3_MAX_EMERNUM_NUM];
} S_L3_EcNumList;

/*24008_10.5.1.6 , Mobile_station_classmark_2*/
typedef struct
{
    uint8_t                     bLen;

    uint8_t                     bRfCapa  : 3;
    uint8_t                     bA51  : 1;
    uint8_t                     bESIND  : 1;
    uint8_t                     bRevLevel  : 2;
    uint8_t                     bSpare0: 1;

    uint8_t                     bFC  : 1;
    uint8_t                     bVGCS  : 1;
    uint8_t                     bVBS  : 1;
    uint8_t                     bSmCapa  : 1;
    uint8_t                     bSSInd  : 2;
    uint8_t                     bPScapa  : 1;
    uint8_t                     bSpare5: 1;

    uint8_t                     bA52  : 1;
    uint8_t                     bA53  : 1;
    uint8_t                     bCMSP  : 1;
    uint8_t                     bSoLSA  : 1;
    uint8_t                     bUCS2  : 1;
    uint8_t                     bLCSVACAP  : 1;
    uint8_t                     bSpare13: 1;
    uint8_t                     bCM3  : 1;
} S_L3_MsClassmark2;

/*24008_10.5.4.32a , System_Identification*/
typedef struct
{
    uint8_t                     bSystemId;
    uint8_t                     bCodBitmapLen;
    uint8_t                     abCodBitmap[L3_MAX_CODECBITMAPBITS_NUM];
} S_L3_SystemId;

/*24008_10.5.4.32 , Supported_Codec_List*/
typedef struct
{
    uint8_t                     bSystemIdLen;
    S_L3_SystemId        atSystemId[L3_MAX_SYSTEMID_NUM];
} S_L3_SuppCdecList;

/*24008_10.5.3.2.2 , Authentication_Failure_parameter*/
typedef struct
{
    uint8_t                     bLen;
    uint8_t                     abAuthFail[14];
} S_L3_AuthFailPara;

/*24008_10.5.3.1 , Authentication_parameter_RAND*/
typedef struct
{
    uint8_t                     abRAND[16];
} S_L3_Rand;

/*24008_10.5.3.1.1 , Authentication_Parameter_AUTN*/
typedef struct
{
    uint8_t                     bLen;
    uint8_t                     abAUTN[16];
} S_L3_Autn;

/*24008_10.5.3.5a , Network_Name*/
typedef struct
{
    uint8_t                     bNumSpare  : 3;
    uint8_t                     bAddCI  : 1;
    uint8_t                     bCodeSchm  : 3;
    uint8_t                     bSpare0: 1;

    uint8_t                     bTxtStrLen;
    uint8_t                     abTxtStr[L3_MAX_TXTSTR_NUM];
} S_L3_NetName;

/*24008_10.5.3.9 , Time_Zone_and_Time*/
typedef struct
{
    uint8_t                     bYear;
    uint8_t                     bMonth;
    uint8_t                     bDay;
    uint8_t                     bHour;
    uint8_t                     bMinute;
    uint8_t                     bSecond;
    uint8_t                     bTmZone;
} S_L3_TZAndTime;

/*24008_10.5.3.12 , daylight saving time*/
typedef struct
{
    uint8_t                     bLen;
    uint8_t                     bValue  : 2;
    uint8_t                     bSpare0    : 6;
} S_L3_SavingTime;


/*24008_10.5.4.9 , Calling_party_BCD_number*/
typedef struct
{
    uint8_t                     bLen;
    uint8_t                     bNumPlanId  : 4;
    uint8_t                     bTypeNum  : 3;
    uint8_t                     bExt  : 1;
    uint8_t                     bScrnInd  : 2;
    uint8_t                     bSpare5: 3;
    uint8_t                     bPrestInd  : 2;
    uint8_t                     bSpare3: 1;
    uint8_t                     bNumLen;
    uint8_t                     abNum[L3_MAX_CALLINGBCDNUM_NUM];
} S_L3_CallingBcdNum;

/*24301_9.9.3.34 , UE_network_capability*/
typedef struct
{
    uint8_t                     bEEA7  : 1;
    uint8_t                     bEEA6  : 1;
    uint8_t                     bEEA5  : 1;
    uint8_t                     bEEA4  : 1;
    uint8_t                     bEEA3  : 1;
    uint8_t                     b128EEA2  : 1;
    uint8_t                     b128EEA1  : 1;
    uint8_t                     bEEA0  : 1;

    uint8_t                     bEIA7  : 1;
    uint8_t                     bEIA6  : 1;
    uint8_t                     bEIA5  : 1;
    uint8_t                     bEIA4  : 1;
    uint8_t                     bEIA3  : 1;
    uint8_t                     b128EIA2  : 1;
    uint8_t                     b128EIA1  : 1;
    uint8_t                     bEIA0  : 1;

    uint8_t                     bpresentfg1;
    uint8_t                     bUEA7  : 1;
    uint8_t                     bUEA6  : 1;
    uint8_t                     bUEA5  : 1;
    uint8_t                     bUEA4  : 1;
    uint8_t                     bUEA3  : 1;
    uint8_t                     bUEA2  : 1;
    uint8_t                     bUEA1  : 1;
    uint8_t                     bUEA0  : 1;

    uint8_t                     bpresentfg2;
    uint8_t                     bUIA7  : 1;
    uint8_t                     bUIA6  : 1;
    uint8_t                     bUIA5  : 1;
    uint8_t                     bUIA4  : 1;
    uint8_t                     bUIA3  : 1;
    uint8_t                     bUIA2  : 1;
    uint8_t                     bUIA1  : 1;
    uint8_t                     bUCS2  : 1;

    uint8_t                     bpresentfg3;
    uint8_t                     bNF  : 1;
    uint8_t                     b1xSRVCC  : 1;
    uint8_t                     bLCS  : 1;
    uint8_t                     bLPP  : 1;
    uint8_t                     bSpare32: 4;

    uint8_t                     bpresentfg4;
    uint8_t                     bSpare37: 8;
    uint8_t                     bpresentfg5;
    uint8_t                     bSpare38: 8;
    uint8_t                     bpresentfg6;
    uint8_t                     bSpare39: 8;
    uint8_t                     bpresentfg7;
    uint8_t                     bSpare40: 8;
    uint8_t                     bpresentfg8;
    uint8_t                     bSpare41: 8;
    uint8_t                     bpresentfg9;
    uint8_t                     bSpare42: 8;
    uint8_t                     bpresentfg10;
    uint8_t                     bSpare43: 8;
    uint8_t                     bpresentfg11;
    uint8_t                     bSpare44: 8;
} S_L3_EPSUEnetworkcapability;

/*24008_10.5.7.4 , GPRS_Timer_2*/
typedef struct
{
    uint8_t                     bTmr2Len;      /*not used*/
    uint8_t                     bTmr2Val;
} S_L3_GprsTimer2;
/*24008_10.5.5.32 ,Extended_DRX_parameters*/
typedef struct
{
    uint8_t                     beDRXLen;      /*not used*/
    uint8_t                     beDRXVal       : 4;
    uint8_t                     bPTW           : 4;
} S_L3_eDRXPara;

/*9.9.3.4B , SMS_Services_status*/
typedef struct
{
    uint8_t           bSmsServStatVal                :3;
    uint8_t           b1Spare                        :1;
    uint8_t           b4Spare                        :4;
} S_L3_SmsServStatus;

/*9.9.3.26A , Extend_EMM_cause*/
typedef struct
{
    uint8_t                     bEutraFlg      : 1;
    uint8_t                     bEpsOpInfo     : 1;
    uint8_t                     bSpare0        : 2;
    uint8_t                     bSpare         : 4;
} S_L3_eEMMCause;



/*9.9.3.45 , GUTI_type*/
typedef struct
{
    uint8_t                     bGutiTypeFlg   : 1;
    uint8_t                     bSpare0        : 3;
    uint8_t                     bSpare         : 4;
} S_L3_GutiType;

/*24008_10.5.1.15 , MS_network_feature_support*/
typedef struct
{
    uint8_t                     bPeriodTmrFlg : 1;
    uint8_t                     bSpare        : 7;
} S_L3_MsNetWorkFeaSupp;

/*9.9.4.27 , Header_compression_configuration_status*/
typedef struct
{
    uint8_t                     bLen;          /*not used*/

    uint8_t                     bEBI0Flg       : 1;
    uint8_t                     bEBI1Flg       : 1;
    uint8_t                     bEBI2Flg       : 1;
    uint8_t                     bEBI3Flg       : 1;
    uint8_t                     bEBI4Flg       : 1;
    uint8_t                     bEBI5Flg       : 1;
    uint8_t                     bEBI6Flg       : 1;
    uint8_t                     bEBI7Flg       : 1;

    uint8_t                     bEBI8Flg       : 1;
    uint8_t                     bEBI9Flg       : 1;
    uint8_t                     bEBI10Flg      : 1;
    uint8_t                     bEBI11Flg      : 1;
    uint8_t                     bEBI12Flg      : 1;
    uint8_t                     bEBI13Flg      : 1;
    uint8_t                     bEBI14Flg      : 1;
    uint8_t                     bEBI15Flg      : 1;
} S_L3_HeaderComPressCfgStatus;

/*9.9.3.47 , Control_plane_service_type*/
typedef struct
{
    uint8_t                     bCpSrTypeVal   : 3;
    uint8_t                     bActiveFlg     : 1;
    uint8_t                     bSpare         : 4;
} S_L3_CpSrType;

/*9.9.3.15 , ESM_message_container*/
/*9.9.3.43 , generic_message_container*/
/*9.9.4.24 , User_data_container*/
typedef struct
{
    uint16_t                    wLen;
    uint8_t                     *pbContainer;
} S_L3_Container;

/*9.9.3.22 , Nas_message_container*/
typedef struct
{
    uint8_t                     bNasMsgLen;
    uint8_t                     abNascntnrList[251];
} S_L3_NasMsgContainer;


/*24008_10.5.1.4 , E_Mobile_Identity_IMSI_IMEI*/
typedef struct{
    uint8_t                     bIdType  :3;
    uint8_t                     bOorE:1;
    uint8_t                     bSpare0:4;

    uint8_t                     bIdNum;
    uint8_t                     abId[L3_MAX_E_MOBID_NUM];
}S_L3_E_MobileIdentity;


typedef struct{
    uint8_t                                bTAC;
    uint8_t                                bTACcon;
}S_L3_TAC;

/*9.9.3.32    Tracking area identity*/
typedef struct{
    S_PS_PlmnId                        tPlmn;
    S_L3_TAC                        ttac;
}S_L3_Tai;

typedef struct{
    S_PS_PlmnId                        tPlmn;
    S_L3_TAC                        *ptList;
}S_L3_PtailistType0;

typedef struct{
    S_L3_Tai                       *ptList;
}S_L3_PtailistType2;

typedef struct{
    BITS                            bNumOfElem:5;/********************
                                                 *List 中数组的实际有效长度为该字段值加1
                                                 5  4 3 2 1
                                                 0  0 0 0 0   1 element
                                                 0  0 0 0 1   2 elements
                                                 0  0 0 1 0   3 elements
                                                 …
                                                 0  1 1 0 1   14 elements
                                                 0  1 1 1 0   15 elements
                                                 0  1 1 1 1   16 elements
                                                 ********************/
    BITS                            bTypeOfList:2;
    /*0          tType0
      1          tType1
      2          tType2*/
    BITS                                bSpare2:1;

    union{
        S_L3_PtailistType0        tType0;
        S_L3_Tai        tType1;
        S_L3_PtailistType2  tType2;
    }Val;

}S_L3_Ptailist;

/*9.9.3.33  Tracking area identity list*/
typedef struct{
    uint8_t                bNum;
    S_L3_Ptailist   atList[L3_MAX_TAI_NUM];
}S_L3_Tailist;


/*9.9.3.12  EPS mobile identity   GUTI*/
typedef struct{
    BITS                            bIdType:3;
    BITS                            bOorE:1;
    BITS                            bSpare2:4;

    S_PS_PlmnId                     tPlmn;
    uint8_t                            bMMEGROUPID[2];
    uint8_t                            bMMECODE;
    uint8_t                            bMTMSI[4];
}S_L3_EPSmobidGUTI;

/*9.9.3.12  EPS mobile identity*/
typedef struct{
    uint8_t                            bIdx; /* 0-tGUTI 1-tIMSI 2-tIMEI */
    union{
        S_L3_EPSmobidGUTI          tGUTI;
        S_L3_E_MobileIdentity      tIMSI;
        S_L3_E_MobileIdentity      tIMEI;
    }Val;
}S_L3_EPSmobid;

/*24301_9.9.3.23 , NAS_security_algorithms*/
typedef struct{
    uint8_t                     bTypeofintgrtyalg  :3;
    uint8_t                     bSpare0:1;
    uint8_t                     bTypeofcphralg  :3;
    uint8_t                     bSpare2:1;
}S_L3_NASsecalg;

/*24301_9.9.3.21 , NAS_key_set_identifier*/
typedef struct{
    uint8_t                     bNASkeysetid  :3;
    uint8_t                     bTSC  :1;
    uint8_t                     bSpare2:4;
}S_L3_NASkeysetId;

/*24301_9.9.3.36 , UE_security_capability*/
typedef struct{
    uint8_t                     bEEA7  :1;
    uint8_t                     bEEA6  :1;
    uint8_t                     bEEA5  :1;
    uint8_t                     bEEA4  :1;
    uint8_t                     bEEA3  :1;
    uint8_t                     b128EEA2  :1;
    uint8_t                     b128EEA1  :1;
    uint8_t                     bEEA0  :1;

    uint8_t                     bEIA7  :1;
    uint8_t                     bEIA6  :1;
    uint8_t                     bEIA5  :1;
    uint8_t                     bEIA4  :1;
    uint8_t                     bEIA3  :1;
    uint8_t                     b128EIA2  :1;
    uint8_t                     b128EIA1  :1;
    uint8_t                     bEIA0  :1;

    uint8_t                     bpresentfg1;
    uint8_t                     bUEA7  :1;
    uint8_t                     bUEA6  :1;
    uint8_t                     bUEA5  :1;
    uint8_t                     bUEA4  :1;
    uint8_t                     bUEA3  :1;
    uint8_t                     bUEA2  :1;
    uint8_t                     bUEA1  :1;
    uint8_t                     bUEA0  :1;

    uint8_t                     bpresentfg2;
    uint8_t                     bUIA7  :1;
    uint8_t                     bUIA6  :1;
    uint8_t                     bUIA5  :1;
    uint8_t                     bUIA4  :1;
    uint8_t                     bUIA3  :1;
    uint8_t                     bUIA2  :1;
    uint8_t                     bUIA1  :1;
    uint8_t                     bSpare24:1;

    uint8_t                     bpresentfg3;
    uint8_t                     bGEA7  :1;
    uint8_t                     bGEA6  :1;
    uint8_t                     bGEA5  :1;
    uint8_t                     bGEA4  :1;
    uint8_t                     bGEA3  :1;
    uint8_t                     bGEA2  :1;
    uint8_t                     bGEA1  :1;
    uint8_t                     bSpare32:1;
}S_L3_UEseccapa;

/*24008 10.5.5.10 , IMEISV request*/
typedef struct{
    uint8_t                     bImeisvReq  :3;
    uint8_t                     bSpare  :1;
    uint8_t                     bSpare2:4;
}S_L3_ImeisvReq;


/*24301_9.9.3.10 , EPS_attach_result*/
typedef struct{
    uint8_t                     bEPSattres  :3;
    uint8_t                     bSpare  :1;
    uint8_t                     bSpare2:4;
}S_L3_EPSattres;

/*24301_9.9.3.9 , EMM_cause*/
typedef struct{
    uint8_t                     bCause;
}S_L3_EmmCause;

/*24301_9.9.3.4 , Authentication_response_parameter_EPS*/
typedef struct{
    uint8_t                     bRESEPSLen;
    uint8_t                     abRESEPS[L3_MAX_RESEPS_NUM];
}S_L3_AuthresparamEPS;

/*24301_9.9.3.39 , SS_Code*/
typedef struct{
    uint8_t                     bSSCodeval;
}S_L3_SSCode;

/*24301_9.9.3.40 , LCS_indicator*/
typedef struct{
    uint8_t                     bLCSindval;
}S_L3_LCSindicator;

/*24301_9.9.3.41 , LCS_client_identity*/
typedef struct{
    uint8_t                     bLCSclientidLen;
    uint8_t                     abLCSclientid[L3_MAX_LCSCLIENTID_NUM];
}S_L3_LCSclientid;

/*24301_9.9.3.7 , EMM_Detach_type*/
typedef struct{
    uint8_t                     bTypeofdetach  :3;
    uint8_t                     bSwitchoff  :1;
    uint8_t                     bSpare2:4;
}S_L3_EMMDetachtype;

/*24301_9.9.3.27 , EPS_Service_type*/
typedef struct{
    uint8_t                     bSrvtype  :4;
    uint8_t                     bSpare1:4;
}S_L3_EPSSrvtype;

/*24301_9.9.3.5 , CSFB_response*/
typedef struct{
    uint8_t                     bCSFBresp  :3;
    uint8_t                     bSpare0:1;
    uint8_t                     bSpare2:4;
}S_L3_CSFBresp;

/*24301_9.9.3.13 , EPS_update_result*/
typedef struct{
    uint8_t                     bEPSupdateresult  :3;
    uint8_t                     bSpare0:1;
    uint8_t                     bSpare2:4;
}S_L3_EPSupdateresult;

/*24301_9.9.2.1 , EPS_bearer_context_status*/
typedef struct{
    uint8_t                     bLen;
    uint8_t                     abEBI[2];
}S_L3_EPSbrctextstatus;

/*24301_9.9.3.11 , EPS_attach_type*/
typedef struct{
    uint8_t                     bEPSattachtype  :3;
    uint8_t                     bSpare0:1;
    uint8_t                     bSpare2:4;
}S_L3_EPSattachtype;

/*24301_9.9.3.14 , EPS_update_type*/
typedef struct{
    uint8_t                     bEPSupdatetype  :3;
    uint8_t                     bActiveflag  :1;
    uint8_t                     bSpare2:4;
}S_L3_EPSupdatetype;

/*24301_9.9.3.35 , UE_radio_capability_information_update_needed*/
typedef struct{
    uint8_t                     bURCupd  :1;
    uint8_t                     bSpare0:3;
    uint8_t                     bSpare2:4;
}S_L3_UErcapaupdateneeded;

/*24301_9.9.3.12A , EPS_network_feature_support*/
typedef struct
{
    uint8_t                     bLen;
    uint8_t                     bIMSVoPS       : 1;
    uint8_t                     bEMCBS         : 1;
    uint8_t                     bLCS           : 1;
    uint8_t                     bCS_LCS        : 2;
    uint8_t                     bESR_PS        : 1;
    uint8_t                     bERwo_PDN      : 1;
    uint8_t                     bCP_Ciot       : 1;

    uint8_t                     bUP_Ciot       : 1;
    uint8_t                     bS1_Udata      : 1;
    uint8_t                     bHC_CP_Ciot    : 1;
    uint8_t                     bePCO          : 1;
    uint8_t                     bSpare1        : 4;
} S_L3_EPSNetFeaSupp;

/*24301_9.9.2.7 , NAS_security_parameters_to_E_UTRA*/
typedef struct{
    uint32_t                    dwNonceMme;
    uint8_t                     bIntAlg  :3;
    uint8_t                     bSpare1:1;
    uint8_t                     bCipAlg  :3;
    uint8_t                     bSpare3:1;
    uint8_t                     bKsiSGSN  :3;
    uint8_t                     bTsc  :1;
    uint8_t                     bSpare5:4;
}S_L3_NasSecParaToLte;

/* 24301 9.9.3.0A Additional update result */
typedef struct
{
    uint8_t           bAddRstVal                  :2;
    uint8_t           bSpare1                     :2;
    uint8_t           bSpare                      :4;
} S_L3_AddUpdateResult;

// @STD-PS(NAS_STD_ATTACH_ACCEPT, "NAS", "", "Attach accept")
/*Attachaccept  24301 8.2.1 Attach_accept */
typedef struct
{
    uint8_t                     bPd  : 4;
    uint8_t                     bSechdtype  : 4;
    uint8_t                     bMsgType;
    S_L3_EPSattres           tEPSattres;
    S_L3_GprsTimer           tT3412;

    uint8_t                     bGUTIFg            : 1;
    uint8_t                     bLaiFg             : 1;
    uint8_t                     bMSidFg            : 1;
    uint8_t                     bEMMcauseFg        : 1;
    uint8_t                     bT3402Fg           : 1;
    uint8_t                     bT3423Fg           : 1;
    uint8_t                     bPLMNlistFg        : 1;
    uint8_t                     bEmerNumListFg     : 1;

    uint8_t                     bEPSNetFeaSuppFg   : 1;
    uint8_t                     bAddUpdateResFg    : 1;
    uint8_t                     bT3412ExValFg      : 1;
    uint8_t                     bT3324ValFg        : 1;
    uint8_t                     bEdrxParaFg        : 1;
    uint8_t                     bSmsServStatFg     : 1;
    uint8_t                     bSpareX            : 2;

    S_L3_Tailist                   tTAIlist;
    S_L3_Container           tEsmMsgCntn;

    S_L3_EPSmobid                  tGUTI;
    S_LAI                          tLai;
    S_L3_MobId                     tMSid;
    S_L3_EmmCause                  tEMMcause;
    S_L3_GprsTimer                 tT3402;
    S_L3_GprsTimer                 tT3423;
    S_L3_PlmnList                  tPLMNlist;
    S_L3_EcNumList                 tEmerNumList;
    S_L3_EPSNetFeaSupp             tEPSNetFeaSupp;
    S_L3_AddUpdateResult           tAddUpdateRes;
    S_L3_GprsTimer3                tT3412ExVal;
    S_L3_GprsTimer2                tT3324Val;
    S_L3_eDRXPara                  tEdrxPara;
    S_L3_SmsServStatus             tSmsServStat;
} S_L3_Attachaccept;

// @STD-PS(NAS_STD_ATTACH_CMPLT, "NAS", "", "Attach complete")
/*Attachcomplete  24301 8.2.2 Attach_complete */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    S_L3_Container     tEsmMsgCntn;
}S_L3_Attachcomplete;

// @STD-PS(NAS_STD_ATTACH_REJ, "NAS", "", "Attach reject")
/*Attachreject  24301 8.2.3 Attach_reject */
typedef struct
{
    uint8_t                     bPd  : 4;
    uint8_t                     bSechdtype  : 4;
    uint8_t                     bMsgType;
    S_L3_EmmCause            tEMMcause;

    uint8_t                     bESMcntnrFg        : 1;
    uint8_t                     bT3346ValFg        : 1;
    uint8_t                     bT3402valueFg      : 1;
    uint8_t                     beEmmCauseFg       : 1;
    uint8_t                     bSpareX            : 4;

    S_L3_Container    tEsmMsgCntn;
    S_L3_GprsTimer2         tT3346Val;
    S_L3_GprsTimer2         bT3402Val;
    S_L3_eEMMCause          teEmmCause;
} S_L3_Attachreject;

/* 24008 10.5.5.4  TMSI status*/
typedef struct
{
    uint8_t                     bTmsiFlag   : 1;
    uint8_t                     bSpare  : 7;
} S_L3_TMSIstatus;


/* 24301 9.9.3.0.B  Additional update type*/
typedef struct
{
    uint8_t                     bAutv      : 1;
    uint8_t                     bSaf       : 1;
    uint8_t                     bPnbCiot  : 2;
    uint8_t                     bSpare     : 4;
} S_L3_AddUpdateType;

/* 24301 9.9.3.45  GUTI type*/
typedef struct
{
    uint8_t                     bGutiType   : 1;
    uint8_t                     bSpare  : 7;
} S_L3_OldGUTItype;

// @STD-PS(NAS_STD_ATTACH_REQ, "NAS", "", "Attach request")
/*Attachrequest  24301 8.2.4 Attach_request */
typedef struct
{
    uint8_t                            bPd  : 4;
    uint8_t                            bSechdtype  : 4;
    uint8_t                            bMsgType;
    uint8_t                            bEPSattachtype  :3;
    uint8_t                            bSpare0:1;
    uint8_t                            bNASkeysetid  :3;
    uint8_t                            bTSC  :1;

    uint8_t                            bAddGUTImobidFg         : 1;
    uint8_t                            bLastTAIFg              : 1;
    uint8_t                            bDRXparameterFg         : 1;
    uint8_t                            bOldlaiFg               : 1;
    uint8_t                            bTMSIstatusFg           : 1;
    uint8_t                            bAddUpdateTypeFg        : 1;
    uint8_t                            bVoiceDomUEUsageFg      : 1;
    uint8_t                            btDevicProperFg         : 1;

    uint8_t                            bOldGUTItypeFg          : 1;
    uint8_t                            bMsNetworkFeaSuppFg     : 1;
    uint8_t                            bT3324ValueFg           : 1;
    uint8_t                            bT3412ExValFg           : 1;
    uint8_t                            bEdrxParaFg             : 1;
    uint8_t                            bSpareXX                : 3;

    S_L3_EPSmobid                  tEpsMobileId;
    S_L3_EPSUEnetworkcapability    tUEnetworkcapa;
    S_L3_Container                 tEsmMsgCntn;

    S_L3_EPSmobid                  tAddGUTImobid;
    S_L3_Tai                       tLastTAI;
    S_L3_DrxPara                   tDRXparameter;

    S_LAI                           tOldlai;
    S_L3_TMSIstatus                 tTMSIstatus;

    S_L3_AddUpdateType              tAddUpdateType;
    S_L3_VoiDomUeUsaSet            tVoiceDomUEUsage;
    S_L3_DeviceProper              tDevicProper;
    S_L3_OldGUTItype               tOldGUTItype;
    S_L3_MsNetWorkFeaSupp          tMsNetworkFeaSupp;
    S_L3_GprsTimer2                tT3324Val;
    S_L3_GprsTimer3                tT3412ExVal;
    S_L3_eDRXPara                  tEdrxPara;
} S_L3_Attachrequest;

// @STD-PS(NAS_STD_AUTH_FALI, "NAS", "", "Authentication failure")
/*Authfailure  24301 8.2.5 Authentication_failure */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;
    S_L3_EmmCause            tEMMcause;

    uint8_t                     bAuthfailparamFg :1;
    uint8_t                     bSpareX:7;

    S_L3_AuthFailPara        tAuthfailparam;
}S_L3_Authfailure;

// @STD-PS(NAS_STD_AUTH_REJ, "NAS", "", "Authentication reject")
/*Authreject  24301 8.2.6 Authentication_reject */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

}S_L3_Authreject;

// @STD-PS(NAS_STD_AUTH_REQ, "NAS", "", "Authentication request")
/*Authrequest  24301 8.2.7 Authentication_request */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;
    S_L3_NASkeysetId         tkystidASME;
    S_L3_Rand                tAuthRAND;

    S_L3_Autn                tAuthAUTN;
}S_L3_Authrequest;

// @STD-PS(NAS_STD_AUTH_RESP, "NAS", "", "Authentication response")
/*Authresponse  24301 8.2.8 Authentication_response */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    S_L3_AuthresparamEPS     tAuthresppara;
}S_L3_Authresponse;


/*CSsrvnoti  24301 8.2.9 CS_service_notification */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    uint8_t                     bSpareX:3;
    uint8_t                     bLCSclientidFg :1;
    uint8_t                     bLCSindFg :1;
    uint8_t                     bSSCodeFg :1;
    uint8_t                     bCLIFg :1;
    uint8_t                     bPageIdFg :1;

    uint8_t                     bPageId;
    S_L3_CallingBcdNum     tCLI;
    S_L3_SSCode          tSSCode;
    S_L3_LCSindicator     tLCSind;
    S_L3_LCSclientid     tLCSclientid;
}S_L3_CSsrvnoti;

// @STD-PS(NAS_STD_DETACH_ACCEPT, "NAS", "", "Detach accept")
/*Detachaccept  24301 8.2.10 Detach_accept */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

}S_L3_Detachaccept;

// @STD-PS(NAS_STD_DETACH_REQ_UL, "NAS", "", "Detach request(UL)")
/*DetachrequestMO  24301 8.2.11.1 Detach_request */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;
    uint8_t                     bTypeofdetach  :3;
    uint8_t                     bSwitchoff  :1;
    uint8_t                     bNASkeysetid  :3;
    uint8_t                     bTSC  :1;

    S_L3_EPSmobid           tEPSmobid;
}S_L3_DetachrequestMO;

// @STD-PS(NAS_STD_DETACH_REQ_DL, "NAS", "", "Detach request(DL)")
/*DetachrequestMT  24301 8.2.11.2 Detach_request */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;
    S_L3_EMMDetachtype       tDetachtype;

    uint8_t                     bEMMcauseFg :1;
    uint8_t                     bSpareX:7;

    S_L3_EmmCause            tEMMcause;
}S_L3_DetachrequestMT;

// @STD-PS(NAS_STD_DL_NAS_TRANS, "NAS", "", "Downlink NAS Transport")
/*DlNASTrans  24301 8.2.12 Downlink_NAS_Transport */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    S_L3_NasMsgContainer     tNasMsgCtn;
}S_L3_DlNASTrans;

// @STD-PS(NAS_STD_UL_NAS_TRANS, "NAS", "", "Uplink NAS Transport")
/*UlNASTrans  24301 8.2.30 Uplink_NAS_Transport */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    S_L3_NasMsgContainer     tNasMsgCtn;
}S_L3_UlNASTrans;

// @STD-PS(NAS_STD_EMM_INFO, "NAS", "", "EMM information")
/*EMMinfo  24301 8.2.13 EMM_information */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    uint8_t                     bFullnameFg :1;
    uint8_t                     bShortnameFg :1;
    uint8_t                     bLocaltimezoneFg :1;
    uint8_t                     bUtimeandlocaltzoneFg :1;
    uint8_t                     bsavingtimeFg :1;
    uint8_t                     bSpareX:3;

    S_L3_NetName             tFullname;
    S_L3_NetName             tShortname;
    uint8_t                     bLocaltimezone;
    S_L3_TZAndTime           tUtimeandlocaltzone;
    S_L3_SavingTime          tSavingtime;
}S_L3_EMMinfo;

// @STD-PS(NAS_STD_EMM_STAT, "NAS", "", "EMM status")
/*EMMstatus  24301 8.2.14 EMM_status */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    S_L3_EmmCause        tEMMcause;
}S_L3_EMMstatus;

// @STD-PS(NAS_STD_EXT_SERV_REQ, "NAS", "", "Extended service request")
/*Extsrvreq  24301 8.2.15 Extended_service_request */
typedef struct
{
    uint8_t                        bPd  : 4;
    uint8_t                        bSechdtype  : 4;
    uint8_t                        bMsgType;
    uint8_t                        bSrvtype  :4;
    uint8_t                        bNASkeysetid  :3;
    uint8_t                        bTSC  :1;

    uint8_t                        bCSFBrespFg         : 1;
    uint8_t                        bEPSBearCtxStatusFg : 1;
    uint8_t                        bDevicProperFg      : 1;
    uint8_t                        bSpareX             : 5;

    S_L3_MobId                 tMTMSI;
    S_L3_CSFBresp              tCSFBresp;
    S_L3_EPSbrctextstatus      tEPSBearCtxStatus;
    S_L3_DeviceProper          tDevicProper;
} S_L3_Extsrvreq;

// @STD-PS(NAS_STD_GUTI_REALLOC_CMD, "NAS", "", "GUTI reallocation command")
/*GUTIrllctncmmnd  24301 8.2.16 GUTI_reallocation_command */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    uint8_t                     bTAIlistFg :1;
    uint8_t                     bSpareX:7;

    S_L3_EPSmobid            tGUTI;
    S_L3_Tailist             tTAIlist;
}S_L3_GUTIrllctncmmnd;

// @STD-PS(NAS_STD_GUTI_REALLOC_CMPLT, "NAS", "", "GUTI reallocation complete")
/*GUTIrllctncmp  24301 8.2.17 GUTI_reallocation_complete */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

}S_L3_GUTIrllctncmp;

// @STD-PS(NAS_STD_IDENTITY_REQ, "NAS", "", "Identity request")
/*Idnttyrequest  24301 8.2.18 Identity_request */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    uint8_t                     bIdtype  :4;
    uint8_t                     bSpare  :4;
}S_L3_Idnttyrequest;

// @STD-PS(NAS_STD_IDENTITY_RESP, "NAS", "", "Identity response")
/*Idnttyresponse  24301 8.2.19 Identity_response */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    S_L3_MobId           tMobid;
}S_L3_Idnttyresponse;

// @STD-PS(NAS_STD_SECURITY_MODE_CMD, "NAS", "", "Security mode command")
/*Secmodecmd  24301 8.2.20 Security_mode_command */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;
    S_L3_NASsecalg           tSelsecalg;
    S_L3_NASkeysetId         tkeystid;

    uint8_t                     bIMEISVreqFg :1;
    uint8_t                     bRenonceFg :1;
    uint8_t                     bNonceMMEFg :1;
    uint8_t                     bSpareX:5;

    S_L3_UEseccapa           tReUEseccapa;
    S_L3_ImeisvReq           tIMEISVreq;
    uint32_t                    dwRenonce;
    uint32_t                    dwNonceMME;
}S_L3_Secmodecmd;

// @STD-PS(NAS_STD_SECURITY_MODE_CMPLT, "NAS", "", "Security mode complete")
/*Scmdcomp  24301 8.2.21 Security_mode_complete */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    uint8_t                     bIMEISVFg :1;
    uint8_t                     bSpareX:7;

    S_L3_MobId               tIMEISV;
}S_L3_Scmdcomp;

// @STD-PS(NAS_STD_SECURITY_MODE_REJ, "NAS", "", "Security mode reject")
/*Scmdrej  24301 8.2.22 Security_mode_reject */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

    S_L3_EmmCause        tEMMcause;
}S_L3_Scmdrej;

// @STD-PS(NAS_STD_SERV_REJ, "NAS", "", "Service reject")
/*Srvrej  24301 8.2.24 Service reject */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bSechdtype  : 4;
    uint8_t                    bMsgType;
    S_L3_EmmCause           tEMMcause;

    uint8_t                    bT3442Fg        : 1;
    uint8_t                    bT3346ValFg     : 1;
    uint8_t                    bSpareX         : 6;

    S_L3_GprsTimer         tT3442;
    S_L3_GprsTimer2        tT3346Val;
} S_L3_Srvrej;

// @STD-PS(NAS_STD_TAU_ACCEPT, "NAS", "", "Tracking area update accept")
/*Tauaccept  24301 8.2.26 Tracking_area_update_accept */
typedef struct
{
    uint8_t                                bPd  : 4;
    uint8_t                                bSechdtype  : 4;
    uint8_t                                bMsgType;
    S_L3_EPSupdateresult                tEPSupdateresult;

    uint8_t                                bT3412Fg                : 1;
    uint8_t                                bGUTIFg                 : 1;
    uint8_t                                bTAIlistFg              : 1;
    uint8_t                                bEPSbrrctxstatusFg      : 1;
    uint8_t                                bLaiFg                  : 1;
    uint8_t                                bMSidFg                 : 1;
    uint8_t                                bEMMcauseFg             : 1;
    uint8_t                                bT3402Fg                : 1;

    uint8_t                                bT3423Fg                : 1;
    uint8_t                                bEquivalentPLMNsFg      : 1;
    uint8_t                                bEmerNumberListFg       : 1;
    uint8_t                                bEPSNetFeaSuppFg        : 1;
    uint8_t                                bAddUpdateResFg         : 1;
    uint8_t                                bT3412ExValFg           : 1;
    uint8_t                                bT3324ValFg             : 1;
    uint8_t                                bEdrxParaFg             : 1;

    uint8_t                                bHeadCompresCfgFg       : 1;
    uint8_t                                bSmsServStatFg          : 1;
    uint8_t                                bSpareX                 : 6;

    S_L3_GprsTimer                     tT3412;
    S_L3_EPSmobid                      tGUTI;
    S_L3_Tailist                       tTAIlist;
    S_L3_EPSbrctextstatus              tEPSbrrctxstatus;
    S_LAI                              tLai;
    S_L3_MobId                         tMSid;
    S_L3_EmmCause                      tEMMcause;
    S_L3_GprsTimer                     tT3402;
    S_L3_GprsTimer                     tT3423;
    S_L3_PlmnList                      tEquivalentPLMNs;
    S_L3_EcNumList                     tEmerNumberList;
    S_L3_EPSNetFeaSupp                 tEPSNetFeaSupp;
    S_L3_AddUpdateResult               tAddUpdateRes;
    S_L3_GprsTimer3                    tT3412ExVal;
    S_L3_GprsTimer2                    tT3324Val;
    S_L3_eDRXPara                      tEdrxPara;
    S_L3_HeaderComPressCfgStatus       tHeadCompresCfg;
    S_L3_SmsServStatus                 tSmsServStat;
} S_L3_Tauaccept;

// @STD-PS(NAS_STD_TAU_CMPLT, "NAS", "", "Tracking area update complete")
/*Taucmp  24301 8.2.27 Tracking_area_update_complete */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;

}S_L3_Taucmp;

// @STD-PS(NAS_STD_TAU_REJ, "NAS", "", "Tracking area update reject")
/*Taurej  24301 8.2.28 Tracking_area_update_reject */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bSechdtype  : 4;
    uint8_t                    bMsgType;
    S_L3_EmmCause           tEMMcause;

    uint8_t                    bT3346ValFg     : 1;
    uint8_t                    beEMMCauseFg    : 1;
    uint8_t                    bSpareX         : 6;

    S_L3_GprsTimer2         tT3346Val;
    S_L3_eEMMCause          teEMMCause;
} S_L3_Taurej;

// @STD-PS(NAS_STD_TAU_REQ, "NAS", "", "Tracking area update request")
/*Taureq  24301 8.2.29 Tracking_area_update_request */
typedef struct
{
    uint8_t                     bPd  : 4;
    uint8_t                     bSechdtype  : 4;
    uint8_t                     bMsgType;
    uint8_t                     bEPSupdatetype  :3;
    uint8_t                     bActiveflag  :1;
    uint8_t                     bNASkeysetid  :3;
    uint8_t                     bTSC  :1;

    uint8_t                            bUEnetworkcapaFg        : 1;
    uint8_t                            bLastTAIFg              : 1;
    uint8_t                            bDRXparamFg             : 1;
    uint8_t                            bUErcapaupdateneededFg  : 1;
    uint8_t                            bEPSbrrcntxstatusFg     : 1;
    uint8_t                            bOldlaiFg               : 1;
    uint8_t                            bTMSIstatusFg           : 1;
    uint8_t                            bAddUpdateTypeFg        : 1;

    uint8_t                            bVoiceDomUeUsageFg      : 1;
    uint8_t                            bOldGUTItypeFg          : 1;
    uint8_t                            bDeviceProperFg         : 1;
    uint8_t                            bMsNetworkFeaSuppFg     : 1;
    uint8_t                            bT3324ValFg             : 1;
    uint8_t                            bT3412ExValFg           : 1;
    uint8_t                            bEDRXParaFg             : 1;
    uint8_t                            bSpareXX                : 1;



    S_L3_EPSmobid                  tOldGUTI;

    S_L3_EPSUEnetworkcapability    tUEnetworkcapa;
    S_L3_Tai                       tLastTAI;
    S_L3_DrxPara                   tDRXparam;
    S_L3_UErcapaupdateneeded       tUErcapaupdateneeded;
    S_L3_EPSbrctextstatus          tEPSbrrcntxstatus;

    S_LAI                          tOldlai;
    S_L3_TMSIstatus                tTMSIstatus;

    S_L3_AddUpdateType              tAddUpdateType;
    S_L3_VoiDomUeUsaSet            tVoiceDomUeUsage;
    S_L3_OldGUTItype               tOldGUTItype;
    S_L3_DeviceProper              tDeviceProper;
    S_L3_MsNetWorkFeaSupp          tMsNetworkFeaSupp;
    S_L3_GprsTimer2                tT3324Val;
    S_L3_GprsTimer3                tT3412ExVal;
    S_L3_eDRXPara                  tEDRXPara;
} S_L3_Taureq;


/*AddInfo  24301 9.9.2.0  Addtional_Infomation*/
typedef struct{
    uint8_t                    bLen;
    uint8_t                    bAddInfo[255];
}S_L3_AddInfo;

// @STD-PS(NAS_STD_DL_GENERIC_NAS_TRANS, "NAS", "", "Downlink generic NAS transport")
/*DlGenNasTrans  24301 8.2.31 Downlink_generic_NAS_transport */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;
    uint8_t                     bGenMsgContainerType;

    uint8_t                     bAddInfoFg :1;
    uint8_t                     bSpare :7;

    S_L3_Container           tGenricMsgCtn;
    S_L3_AddInfo             tAddInfo;
}S_L3_DlGenNasTrans;

// @STD-PS(NAS_STD_UL_GENERIC_NAS_TRANS, "NAS", "", "Uplink generic NAS transport")
/*UlGenNasTrans  24301 8.2.32 Uplink_generic_NAS_transport */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bMsgType;
    uint8_t                     bGenMsgContainerType;

    uint8_t                     bAddInfoFg :1;
    uint8_t                     bSpare :7;

    S_L3_Container           tGenricMsgCtn;
    S_L3_AddInfo             tAddInfo;
}S_L3_UlGenNasTrans;

// @STD-PS(NAS_STD_SP_NAS_MSG, "NAS", "", "Security protected NAS message")
/*SPNASmsg  24301 8.2.23 Security_protected_NAS_message */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint32_t                    dwMac;
    uint8_t                     bSeqnum;
    uint16_t                     wLen;
    uint8_t                     *pbNASmsgList;
}S_L3_SPNASmsg;

// @STD-PS(NAS_STD_SERV_REQ, "NAS", "", "Service request")
/*Srvcreq  24301 8.2.25 Service_request */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSechdtype  :4;
    uint8_t                     bKSIseqnum;
    uint16_t                     wSMac;
}S_L3_Srvcreq;

// @STD-PS(NAS_STD_CP_SERV_REQ, "NAS", "", "CONTROL PLANE SERVICE REQUEST")
/*Srvcreq  24301 8.2.33 Control_Plan_Service_request */
typedef struct
{
    uint8_t                    bPd            : 4;
    uint8_t                    bSechdtype     : 4;
    uint8_t                    bMsgType;
    uint8_t                    bCpSrTypeVal   : 3;
    uint8_t                    bActiveFlg     : 1;
    uint8_t                    bNASkeysetid  :3;
    uint8_t                    bTSC  :1;

    uint8_t                    bEsmMsgContainerFg  : 1;
    uint8_t                    bNasMsgContainerFg  : 1;
    uint8_t                    bEPSbrcntxstatFg    : 1;
    uint8_t                    bDevProperFg        : 1;
    uint8_t                    bSpare              : 4;

    S_L3_Container         tEsmMsgContainer;
    S_L3_NasMsgContainer   tNasMsgContainer;
    S_L3_EPSbrctextstatus  tEPSbrcntxstat;
    S_L3_DeviceProper      tDevProper;
} S_L3_CpSrReq;

// @STD-PS(NAS_STD_SERV_ACCEPT, "NAS", "", "Service Accept")
/*SrvAccept  24301 8.2.34 Service_Accept */
typedef struct
{
    uint8_t                    bPd            : 4;
    uint8_t                    bSechdtype     : 4;
    uint8_t                    bMsgType;

    uint8_t                    bEPSbrcntxstatusFg   : 1;
    uint8_t                    bSpare               : 7;

    S_L3_EPSbrctextstatus  tEPSbrcntxstatus;
} S_L3_SrAccept;

/*24008_11.2.3.1.3 , Transaction_identifier_SM*/
typedef struct{
    uint8_t                     bTio  :3;
    uint8_t                     bTIflag  :1;
    uint8_t                     bSpare  :4;
    uint8_t                     bTie  :7;
    uint8_t                     bExt  :1;
}S_L3_Ti;

/*24008_10.5.6.4 , Packet_data_protocol_address*/
typedef struct{
    uint8_t                     bPdpType  :4;
    uint8_t                     bSpare0:4;
    uint8_t                     bPdpTypeNum;
    uint8_t                     bAddrInfoLen;
    uint8_t                     abAddrInfo[L3_MAX_PDPADDR_NUM];
}S_L3_PdpAddr;



/*Traffic Flow Template defination*/
typedef struct
{
    uint8_t           bFilterId;                                   /*filter id*/
    uint8_t           bPreIndex;                                   /*filter priority */
    /*every bit indicate that the filter is used or unused, 0-unused 1-used*/
    uint8_t           bContentLen;                                 /*packet filter len */
    uint8_t           *pbFilterContent;     /*packet filter content*/
}S_L3_Packet_Filter;

typedef union
{
    uint8_t                     bPacketFilter;
    S_L3_Packet_Filter   tPacketFilter;
}S_L3_Tft_PF_Unit;

typedef struct
{
    uint8_t                     bParaId;                                     /*parameter identifier
                                                                           * 0x01H-Authorization Token
                                                                           * 0x02H-Flow Identifier
                                                                           */
    uint8_t                     bContentLen;
    uint8_t                     *pbContent;                /*Authorization Token或Flow Identifier*/
}S_L3_Tft_Para;

typedef struct
{
    BITS                     bFilterNum:4;                                /*packet filter num ,0-8*/
    BITS                     bEBit:1;
    BITS                     bTftOpCode:3;                                /*TFT operation code:
                                                                          * 000-Spare
                                                                          * 001-Create new TFT
                                                                          * 010-Delete existing TFT
                                                                          * 020-Add packet filter to existing TFT
                                                                          * 100-Replace packet filters in existing TFT
                                                                          * 101-Delete packet filters from existing TFT
                                                                          * 110-No TFT operation
                                                                          * 120-Reserved
    */
    S_L3_Tft_PF_Unit     atTftPfList[L3_PACKET_FILTER_NUM];          /*bTftOpCode=101 use uint8_t type
                                                                             * other value use S_L3_Packet_Filter type */
    uint8_t                     bTftParaNum;                                 /*Authorization Token and Flow Identifier total num in curr entity*/
    S_L3_Tft_Para        atTftParaList[L3_PARA_UNIT_NUM];            /*TFT Parameter List*/
}S_L3_Tft;

/*24008_10.5.6.3a , Protocol_ID*/
typedef struct{
    uint16_t                     wProtoId;
    uint8_t                     bProtoIdContLen;
    uint8_t                     *pbProtoIdCont;
}S_L3_ProtoID;

/*24008_10.5.6.3 , Protocol_configuration_options*/
typedef struct{
    uint8_t                     bCfgProto  :3;
    uint8_t                     bSpare1:4;
    uint8_t                     bSpare0:1;

    uint8_t                     bProtoIDLen;
    S_L3_ProtoID         atProtoID[L3_MAX_PROTOID_NUM];
}S_L3_ProtCfgOpt;

/*24008_10.5.6.7 , Linked_TI*/
typedef struct{
    uint8_t                     bSpare2  :4;
    uint8_t                     bTiVal   :3;
    uint8_t                     bTIflag  :1;

    uint8_t                     bpresentfg1;
    uint8_t                     bTiVal1  :7;
    uint8_t                     bSpare3  :1;
}S_L3_LinkTi;

/*24008_10.5.6.5 , Quality_of_service*/
typedef struct{
    uint8_t                     bReliabClss  :3;
    uint8_t                     bDlyClss  :3;
    uint8_t                     bSpare0:2;
    uint8_t                     bPrecClss  :3;
    uint8_t                     bSpare4:1;
    uint8_t                     bPeakPut  :4;
    uint8_t                     bMeanPut  :5;
    uint8_t                     bSpare6:3;

    uint8_t                     bpresentfg1;
    uint8_t                     bDelivErrSDU  :3;
    uint8_t                     bDelivOrd  :2;
    uint8_t                     bTraffClss  :3;
    uint8_t                     bMaxSDUsiz;
    uint8_t                     bMaxBitUp;
    uint8_t                     bMaxBitDown;
    uint8_t                     bSDUerrRat  :4;
    uint8_t                     bResiBER  :4;
    uint8_t                     bTrafHand  :2;
    uint8_t                     bTranLay  :6;

    uint8_t                     bGuarBitUp;
    uint8_t                     bGuarBitDown;

    uint8_t                     bpresentfg2;
    uint8_t                     bSourDescr  :4;
    uint8_t                     bSignInd  :1;
    uint8_t                     bSpare20:3;

    uint8_t                     bpresentfg3;
    uint8_t                     bMaxBitExtDl;
    uint8_t                     bGuarBitExtDl;

    uint8_t                     bpresentfg4;
    uint8_t                     bMaxBitExtUl;
    uint8_t                     bGuarBitExtUl;
}S_L3_Qos;


typedef struct{
    uint8_t                     bLen;
    uint8_t                     abQos[16];
}S_L3_Qos_LV;

/*24008_10.5.6.9 , LLC service access point identifer*/
typedef struct{
    uint8_t                     bSapi  :4;
    uint8_t                     bSpare2:4;
}S_L3_LLCServApnId;

/*24008_10.5.7.2 , Radio priority*/
typedef struct{
    uint8_t                     bPriorLevel  :3;
    uint8_t                     bSpare2 :5;
}S_L3_RadioPriority;


/*24008_10.5.6.11 , Packet Flow Identifier*/
typedef struct{
    uint8_t                     bLen;
    uint8_t                     bPktFlowId   :7;
    uint8_t                     bSpare2      :1;
}S_L3_PktFlowId;

/*24008_10.5.6.1 , Access_point_name*/
typedef struct{
    uint8_t                     bApnValLen;
    uint8_t                     abApnVal[L3_MAX_APN_NUM];
}S_L3_Apn;

/*24008_10.5.6.17 , Request_type*/
typedef struct{
    uint8_t                     bRequesttype  :3;
    uint8_t                     bSpare0:1;
    uint8_t                     bSpare2:4;
}S_L3_Requesttype;


/*ActPdpReq  */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSparePd  :4;
    S_L3_Ti              tTi;
    uint8_t                     bMsgType;

    uint8_t                     bSpareX:5;
    uint8_t                     bRequesttypeFg :1;
    uint8_t                     bProtCfgFg :1;
    uint8_t                     bApnFg :1;

    uint8_t                     bNsapi;
    uint8_t                     bLlcSapi;
    S_L3_Qos             tReqQoS;
    S_L3_PdpAddr         tReqPdpAddr;
    S_L3_Apn             tApn;
    S_L3_ProtCfgOpt      tProtCfg;
    S_L3_Requesttype     tRequesttype;
}S_L3_ActPdpReq;


/*ActPdpAccept  */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSparePd  :4;
    S_L3_Ti              tTi;
    uint8_t                     bMsgType;

    uint8_t                     bSpareX:4;
    uint8_t                     bSmCause2Fg :1;
    uint8_t                     bPackIdFg :1;
    uint8_t                     bProtCfgFg :1;
    uint8_t                     bPdpAddrFg :1;

    uint8_t                     bLlcSapi;
    S_L3_Qos             tQoS;
    uint8_t                     bRaioPrio  :4;
    uint8_t                     bSpare  :4;
    S_L3_PdpAddr         tPdpAddr;
    S_L3_ProtCfgOpt      tProtCfg;
    uint8_t                     bPackId;
    uint8_t                     bSmCause2;
}S_L3_ActPdpAccept;


/*9.9.4.7A , Notification indicator*/
typedef struct
{
    uint8_t                     bLen;
    uint8_t                     bNotifyInd;
} S_L3_NotifInd;

/*9.9.4.13A , Re_attempt_indicator*/
typedef struct
{
    uint8_t                     bLen;          /*not used*/
    uint8_t                     bRATCVal       : 1;
    uint8_t                     bEPLMNCVal     : 1;
    uint8_t                     bSpare         : 6;
} S_L3_ReAttempInd;

/*9.9.4.22 , Header_compression_configutation*/
typedef struct
{
    uint8_t                     bP0x0002       : 1;
    uint8_t                     bP0x0003       : 1;
    uint8_t                     bP0x0004       : 1;
    uint8_t                     bP0x0006       : 1;
    uint8_t                     bP0x0102       : 1;
    uint8_t                     bP0x0103       : 1;
    uint8_t                     bP0x0104       : 1;
    uint8_t                     bSpare0        : 1;

    uint16_t                     wMaxCID;
    uint8_t                     bSetupTpye;
    uint8_t                     bLen;
    uint8_t                     abSetupParaList[L3_MAX_HEADER_COMPRESS_PARA_LEN];
} S_L3_HeaderCompressCfg;

/*9.9.4.23 , Control_plane_only_indication*/
typedef struct
{
    uint8_t                     bCpOnlyFlg        : 1;
    uint8_t                     bSpare0           : 7;
} S_L3_CpOnlyInd;


/*9.9.4.25 , Release_assistance_indication*/
typedef struct
{
    uint8_t                     bDDXVal        : 2;
    uint8_t                     bSpare0        : 6;
} S_L3_RelAssistInd;

/*9.9.4.26 , Extended_protocol_configutation_options*/
typedef struct
{
    uint16_t                     wePCOLen;
    uint8_t                     abePCOContentList[L3_MAX_EPCO_CONTENT_LEN];
} S_L3_ePCOCfgOpt;

/*9.9.4.28 , Serving_PLMN_rate_control*/
typedef struct
{
    uint8_t                     bLen;
    uint8_t                     abSPlmnRateCtrltList[2];
} S_L3_SPlmnRateCtrl;

/*24301_9.9.4.5 , ESM_information_transfer_flag*/
typedef struct{
    uint8_t                     bEIT  :1;
    uint8_t                     bSpare0:3;
    uint8_t                     bSpare2:4;
}S_L3_ESMinfotranflag;

/*24301_9.9.4.4 , ESM_cause*/
typedef struct{
    uint8_t                     bCauVal;
}S_L3_ESMcause;

/*24301_9.9.4.6 , Linked_EPS_bearer_identity*/
typedef struct{
    uint8_t                     bLinEPSBearerIdV  :4;
    uint8_t                     bSpare1:4;
}S_L3_LinkedEPSbid;

/*24301_9.9.4.3 , EPS_quality_of_service*/
typedef struct{
    uint8_t                     bQCI;
    uint8_t                     bpresentfg1;
    uint8_t                     bUlMaxBitRate;
    uint8_t                     bpresentfg2;
    uint8_t                     bDlMaxBitRate;
    uint8_t                     bpresentfg3;
    uint8_t                     bUlGuaBitRate;
    uint8_t                     bpresentfg4;
    uint8_t                     bDlGuaBitRate;
    uint8_t                     bpresentfg5;
    uint8_t                     bUlMaxBitRateExt;
    uint8_t                     bpresentfg6;
    uint8_t                     bDlMaxBitRateExt;
    uint8_t                     bpresentfg7;
    uint8_t                     bUlGuaBitRateExt;
    uint8_t                     bpresentfg8;
    uint8_t                     bDlGuaBitRateExt;
}S_L3_EPSqos;

/*24301_9.9.4.9 , PDN_address*/
typedef struct{
    uint8_t                     bPDNtype  :3;
    uint8_t                     bSpare0:5;
    uint8_t                     bPDNaddinfoLen;
    uint8_t                     abPDNaddinfo[L3_MAX_PDNADDINFO_NUM];
}S_L3_PDNaddress;

/*24301_9.9.4.2 , APN_aggregate_maximum_bit_rate*/
typedef struct{
    uint8_t                     bDlAMBR;
    uint8_t                     bUlAMBR;
    uint8_t                     bpresentfg1;
    uint8_t                     bExtended;
    uint8_t                     bpresentfg2;
    uint8_t                     bUlAMBRExt;
    uint8_t                     bpresentfg3;
    uint8_t                     bDlAMBRExt2;
    uint8_t                     bpresentfg4;
    uint8_t                     bUlAMBRExt2;
}S_L3_Ambr;

/*24301_9.9.4.10 , PDN_type*/
typedef struct{
    uint8_t                     bPDNtype  :3;
    uint8_t                     bSpare0:1;
    uint8_t                     bSpare2:4;
}S_L3_PDNtype;

// @STD-PS(NAS_STD_ACT_DED_BEARER_ACCEPT, "NAS", "", "Activate dedicated EPS bearer context accept")
/*ActDedBcntAccept  24301 8.3.1 Activate_dedicated_EPS_bearer_context_accept */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;

    uint8_t                    bProtCfgOptFg   : 1;
    uint8_t                    bSpareX         : 7;

    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_ActDedBcntAccept;

// @STD-PS(NAS_STD_ACT_DED_BEARER_REJ, "NAS", "", "Activate dedicated EPS bearer context reject")
/*ActDedBcntReject  24301 8.3.2 Activate_dedicated_EPS_bearer_context_reject */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_ESMcause           tEsmCause;

    uint8_t                    bProtCfgOptFg   : 1;
    uint8_t                    bSpareX         : 7;

    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_ActDedBcntReject;

// @STD-PS(NAS_STD_ACT_DED_BEARER_REQ, "NAS", "", "Activate dedicated EPS bearer context request")
/*ActDedBcntReq  24301 8.3.3 Activate_dedicated_EPS_bearer_context_request */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_LinkedEPSbid       tLinEPSBId;

    uint8_t                    bTiFg               : 1;
    uint8_t                    bNQoSFg             : 1;
    uint8_t                    bLLCSAPIFg          : 1;
    uint8_t                    bRadiopriorityFg    : 1;
    uint8_t                    bPfIFg              : 1;
    uint8_t                    bProtCfgOptFg       : 1;
    uint8_t                    bSpareX             : 2;

    S_L3_EPSqos            tEPSQoS;
    S_L3_Tft               tTFT;
    S_L3_LinkTi            tTi;
    S_L3_Qos               tNQoS;
    S_L3_LLCServApnId      tLLCSAPI;
    S_L3_RadioPriority      tRadiopriority;
    S_L3_PktFlowId          tPfI;
    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_ActDedBcntReq;

// @STD-PS(NAS_STD_ACT_DEF_BEARER_ACCEPT, "NAS", "", "Activate default EPS bearer context accept")
/*ActDefauBcntAccept  24301 8.3.4 Activate_default_EPS_bearer_context_accept */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;

    uint8_t                    bProtCfgOptFg      : 1;
    uint8_t                    bSpareX            : 7;

    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_ActDefauBcntAccept;

// @STD-PS(NAS_STD_ACT_DEF_BEARER_REJ, "NAS", "", "Activate default EPS bearer context reject")
/*ActDefauBcntReject  24301 8.3.5 Activate_default_EPS_bearer_context_reject */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_ESMcause           tEsmCause;

    uint8_t                    bProtCfgOptFg   : 1;
    uint8_t                    bSpareX         : 7;

    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_ActDefauBcntReject;

// @STD-PS(NAS_STD_ACT_DEF_BEARER_REQ, "NAS", "", "Activate default EPS bearer context request")
/*ActDefauBcntReq  24301 8.3.6 Activate_default_EPS_bearer_context_request */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;

    uint8_t                    bTiFg                   : 1;
    uint8_t                    bNQoSFg                 : 1;
    uint8_t                    bLLCSAPIFg              : 1;
    uint8_t                    bRadiopriorityFg        : 1;
    uint8_t                    bPfIFg                  : 1;
    uint8_t                    bAMBRFg                 : 1;
    uint8_t                    bESMcauseFg             : 1;
    uint8_t                    bProtCfgOptFg           : 1;

    S_L3_EPSqos            tEPSQoS;
    S_L3_Apn               tApn;
    S_L3_PDNaddress        tPDNaddress;
    S_L3_LinkTi            tTi;
#ifdef CFW_API_SUPPORT
    S_L3_Qos_LV               tNQoS;
#else
    S_L3_Qos               tNQoS;
#endif
    S_L3_LLCServApnId      tLLCSAPI;
    S_L3_RadioPriority      tRadiopriority;
    S_L3_PktFlowId          tPfI;
    S_L3_Ambr              tAMBR;
    S_L3_ESMcause          tESMcause;
    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_ActDefauBcntReq;

// @STD-PS(NAS_STD_BEARER_RES_ALLOC_REJ, "NAS", "", "Bearer resource allocation reject")
/*BResAlloReject  24301 8.3.7 Bearer_Resource_Allocation_reject */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_ESMcause           tEsmCause;

    uint8_t                    bProtCfgOptFg       : 1;
    uint8_t                    bBackOffTmrValFg    : 1;
    uint8_t                    bReAttempValFg      : 1;
    uint8_t                    bSpareX             : 5;

    S_L3_ProtCfgOpt        tProtCfgOpt;
    S_L3_GprsTimer3        tBackOffTmrVal;
    S_L3_ReAttempInd       tReAttempVal;
} S_L3_BResAlloReject;

// @STD-PS(NAS_STD_BEARER_RES_ALLOC_REQ, "NAS", "", "Bearer resource allocation request")
/*BResAlloReq  24301 8.3.8 Bearer_resource_allocation_request */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_LinkedEPSbid       tLinkedEPSbid;

    uint8_t                    bProtCfgOptFg   : 1;
    uint8_t                    bDeviceProperFg : 1;
    uint8_t                    bSpareX         : 6;

    S_L3_Tft               tTfa;
    S_L3_EPSqos            tRequiredQoS;
    S_L3_ProtCfgOpt        tProtCfgOpt;
    S_L3_DeviceProper      tDeviceProper;
} S_L3_BResAlloReq;

// @STD-PS(NAS_STD_BEARER_RES_MODI_REJ, "NAS", "", "Bearer resource modification reject")
/*BResModifReject  24301 8.3.9 Bearer_Resource_Modification_reject */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_ESMcause           tEsmCause;

    uint8_t                    bProtCfgOptFg           : 1;
    uint8_t                    bBackOffTmrValFg        : 1;
    uint8_t                    bReAttempValFg          : 1;
    uint8_t                    bSpareX                 : 5;

    S_L3_ProtCfgOpt        tProtCfgOpt;
    S_L3_GprsTimer3        tBackOffTmrVal;
    S_L3_ReAttempInd       tReAttempVal;
} S_L3_BResModifReject;

// @STD-PS(NAS_STD_BEARER_RES_MODI_REQ, "NAS", "", "Bearer resource modification request")
/*BResModifReq  24301 8.3.10 Bearer_resource_modification_request */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_LinkedEPSbid       tEPSbid;

    uint8_t                    bRequiredQoSFg          : 1;
    uint8_t                    bESMcauseFg             : 1;
    uint8_t                    bProtCfgOptFg           : 1;
    uint8_t                    bDeviceProperFg         : 1;
    uint8_t                    bSpareX                 : 4;

    S_L3_Tft               tTfa;
    S_L3_EPSqos            tRequiredQoS;
    S_L3_ESMcause          tESMcause;
    S_L3_ProtCfgOpt        tProtCfgOpt;
    S_L3_DeviceProper      tDeviceProper;
} S_L3_BResModifReq;

// @STD-PS(NAS_STD_DEACT_BEARER_ACCEPT, "NAS", "", "Deactivate EPS bearer context accept")
/*DeActBcntAccept  24301 8.3.11 Deactivate_EPS_bearer_context_accept */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;

    uint8_t                    bProtCfgOptFg   : 1;
    uint8_t                    bSpareX         : 7;

    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_DeActBcntAccept;

// @STD-PS(NAS_STD_DEACT_BEARER_REQ, "NAS", "", "Deactivate EPS bearer context request")
/*DeActBcntRequest  24301 8.3.12 Deactivate_EPS_bearer_context_request */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_ESMcause          tEsmCause;

    uint8_t                    bProtCfgOptFg   : 1;
    uint8_t                    bT3396ValFg   : 1;
    uint8_t                    bSpareX         : 6;

    S_L3_ProtCfgOpt        tProtCfgOpt;
    S_L3_GprsTimer3        tT3396Val;
} S_L3_DeActBcntRequest;

// @STD-PS(NAS_STD_ESM_INFO_REQ, "NAS", "", "ESM information request")
/*ESMInfoRequest  24301 8.3.13 ESM_Information_request */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bEPSbid  :4;
    uint8_t                     bPti;
    uint8_t                     bMsgType;

}S_L3_ESMInfoRequest;

// @STD-PS(NAS_STD_ESM_INFO_RESP, "NAS", "", "ESM information response")
/*ESMInfoResponse  24301 8.3.14 ESM_Information_response */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;

    uint8_t                    bApnFg          : 1;
    uint8_t                    bProtCfgOptFg   : 1;
    uint8_t                    bSpareX         : 6;

    S_L3_Apn               tApn;
#ifdef CFW_API_SUPPORT
    S_L3_ProtCfgOpt_LV     tProtCfgOpt;
#else
    S_L3_ProtCfgOpt        tProtCfgOpt;
#endif
} S_L3_ESMInfoResponse;

// @STD-PS(NAS_STD_ESM_STAT, "NAS", "", "ESM status")
/*ESMStatus  24301 8.3.15 ESM_status */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bEPSbid  :4;
    uint8_t                     bPti;
    uint8_t                     bMsgType;

    S_L3_ESMcause        tEsmCause;
}S_L3_ESMStatus;

// @STD-PS(NAS_STD_MODI_BEARER_ACCEPT, "NAS", "", "Modify EPS bearer context accept")
/*ModifBcntAccept  24301 8.3.16 Modify_EPS_bearer_context_accept */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;

    uint8_t                    bProtCfgOptFg   : 1;
    uint8_t                    bSpareX         : 7;

    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_ModifBcntAccept;

// @STD-PS(NAS_STD_MODI_BEARER_REJ, "NAS", "", "Modify EPS bearer context reject")
/*ModifBcntReject  24301 8.3.17 Modify_EPS_bearer_context_reject */
typedef struct
{
    uint8_t                     bPd  : 4;
    uint8_t                     bEPSbid  : 4;
    uint8_t                     bPti;
    uint8_t                     bMsgType;
    S_L3_ESMcause            tEsmCause;

    uint8_t                     bProtCfgOptFg  : 1;
    uint8_t                     bSpareX        : 7;

    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_ModifBcntReject;

// @STD-PS(NAS_STD_MODI_BEARER_REQ, "NAS", "", "Modify EPS bearer context request")
/*ModifBcntReq  24301 8.3.18 Modify_EPS_bearer_context_request */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;

    uint8_t                    bNewEPSQoSFg : 1;
    uint8_t                    bTFTFg : 1;
    uint8_t                    bNewQoSFg : 1;
    uint8_t                    bLLCSAPIFg : 1;
    uint8_t                    bRadiopriorityFg : 1;
    uint8_t                    bPfIFg : 1;
    uint8_t                    bAMBRFg : 1;
    uint8_t                    bProtCfgOptFg : 1;

    S_L3_EPSqos            tNewEPSQoS;
    S_L3_Tft               tTFT;
#ifdef CFW_API_SUPPORT
    S_L3_Qos_LV            tNewQoS;
#else
    S_L3_Qos               tNewQoS;
#endif
    S_L3_LLCServApnId      tLLCSAPI;
    S_L3_RadioPriority      tRadiopriority;
    S_L3_PktFlowId          tPfI;
    S_L3_Ambr              tAMBR;
#ifdef CFW_API_SUPPORT
    S_L3_ProtCfgOpt_LV     tProtCfgOpt;
#else
    S_L3_ProtCfgOpt        tProtCfgOpt;
#endif
} S_L3_ModifBcntReq;

// @STD-PS(NAS_STD_NOTIFY, "NAS", "", "Notification")
/*EsmNotify  */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bEPSbid  :4;
    uint8_t                     bPti;
    uint8_t                     bMsgType;

    S_L3_NotifInd            tNotifyInd;
}S_L3_EsmNotify;

// @STD-PS(NAS_STD_PDN_CONN_REJ, "NAS", "", "PDN connectivity reject")
/*PDNConnReject  24301 8.3.19 PDN_connectivity_reject */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_ESMcause           tEsmCause;

    uint8_t                    bProtCfgOptFg       : 1;
    uint8_t                    bBackOffTmrValFg    : 1;
    uint8_t                    bReAttempValFg      : 1;
    uint8_t                    bSpareX             : 5;
#ifdef CFW_API_SUPPORT
    S_L3_ProtCfgOpt_LV     tProtCfgOpt;
#else
    S_L3_ProtCfgOpt        tProtCfgOpt;
#endif
    S_L3_GprsTimer3        tBackOffTmrVal;
    S_L3_ReAttempInd       tReAttempVal;
} S_L3_PDNConnReject;

// @STD-PS(NAS_STD_PDN_CONN_REQ, "NAS", "", "PDN connectivity request")
/*PDNConnReq  24301 8.3.20 PDN_connectivity_request */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    uint8_t                    bRequesttype  :3;
    uint8_t                    bSpare0:1;
    uint8_t                    bPDNtype  :3;
    uint8_t                    bSpare1:1;

    uint8_t                    bESMinfotransflagFg     : 1;
    uint8_t                    bApnFg                  : 1;
    uint8_t                    bProtCfgOptFg           : 1;
    uint8_t                    bDeviceProperFg         : 1;
    uint8_t                    bSpareX                 : 4;

    S_L3_ESMinfotranflag   tESMinfotransflag;
    S_L3_Apn               tApn;
#ifdef CFW_API_SUPPORT
        S_L3_ProtCfgOpt_LV     tProtCfgOpt;
#else
        S_L3_ProtCfgOpt        tProtCfgOpt;
#endif

    S_L3_DeviceProper      tDeviceProper;
} S_L3_PDNConnReq;

// @STD-PS(NAS_STD_PDN_DISCONN_REJ, "NAS", "", "PDN disconnect reject")
/*PDNDConnReject  24301 8.3.21 PDN_disconnect_reject */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_ESMcause           tEsmCause;

    uint8_t                    bProtCfgOptFg       : 1;
    uint8_t                    bSpareX             : 7;

    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_PDNDConnReject;

// @STD-PS(NAS_STD_PDN_DISCONN_REQ, "NAS", "", "PDN disconnect request")
/*PDNDConnRequest  24301 8.3.22 PDN_disconnect_request */
typedef struct
{
    uint8_t                    bPd  : 4;
    uint8_t                    bEPSbid  : 4;
    uint8_t                    bPti;
    uint8_t                    bMsgType;
    S_L3_LinkedEPSbid       tLinEPSBId;

    uint8_t                    bProtCfgOptFg           : 1;
    uint8_t                    bSpareX                 : 7;

    S_L3_ProtCfgOpt        tProtCfgOpt;
} S_L3_PDNDConnRequest;

// @STD-PS(NAS_STD_ESM_DUMMY_MSG, "NAS", "", "ESM dummy message")
/*ESMdummymessage  24301 8.3.12A ESM_dummy_message */
typedef struct
{
    uint8_t                     bPd                    : 4;
    uint8_t                     bEPSbid                : 4;
    uint8_t                     bPti;
    uint8_t                     bMsgType;
} S_L3_ESMDummyMsg;

// @STD-PS(NAS_STD_ESM_DATA_TRANS, "NAS", "", "ESM DATA TRANSPORT")
/*ESMDATATRANSPORT  24301 8.3.25 ESM_DATA_TRANSPORT */
typedef struct
{
    uint8_t                        bPd                 : 4;
    uint8_t                        bEPSbid             : 4;
    uint8_t                        bPti;
    uint8_t                        bMsgType;

    uint8_t                        bRelAssistIndFg          : 1;
    uint8_t                        bSpareX                  : 7;

    S_L3_Container             tUsrDataContainer;
    S_L3_RelAssistInd          tRelAssistInd;
} S_L3_ESMDataTrans;

/*24011_U.8.1.0 , CP_User_Data*/
typedef struct{
    uint8_t                     bUserdataLen;
    uint8_t                     abUserdata[L3_MAX_CPUSERDATA_NUM];
}S_L3_CpUserData;

/*24011_U.8.1.4 , RP_User_Data*/
typedef struct{
    uint8_t                     bRpUserDataLen;
    uint8_t                     abRpUserData[L3_MAX_RPUSERDATA_NUM];
}S_L3_RpUserData;

/*24011_U.8.1.3 , RP_Address_number*/
typedef struct{
    uint8_t                     bRpAdrsLen;
    uint8_t                     abRpAdrs[L3_MAX_RPADRS_NUM];
}S_L3_RpAdrs;

/*24011_U.8.1.4 , RP_Cause*/
typedef struct{
    uint8_t                     bRpCausLen;
    uint8_t                     abRpCaus[L3_MAX_RPCAUS_NUM];
}S_L3_RpCaus;


/*CpData  24011 7.2.1 */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bTi  :4;
    uint8_t                     bMsgType;

    S_L3_CpUserData      tCpUd;
}S_L3_CpData;


/*CpAck  24011 7.2.2 */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bTi  :4;
    uint8_t                     bMsgType;

}S_L3_CpAck;


/*CpError  24011 7.2.3 */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bTi  :4;
    uint8_t                     bMsgType;

    uint8_t                     bCpCause;
}S_L3_CpError;


/*RpDataMT  24011 7.3.1.1 */
typedef struct{
    uint8_t                     bMsgType  :3;
    uint8_t                     bSpare    :5;
    uint8_t                     bRpMR;

    S_L3_RpAdrs              tRpOA;
    uint8_t                     bRpDA;
    S_L3_RpUserData          tRpUd;
}S_L3_RpDataMT;


/*RpDataMO  24011 7.3.1.2 */
typedef struct{
    uint8_t                     bMsgType  :3;
    uint8_t                     bSpare    :5;
    uint8_t                     bRpMR;

    uint8_t                     bRpOA;
    S_L3_RpAdrs              tRpDA;
    S_L3_RpUserData          tRpUd;
}S_L3_RpDataMO;


/*RpSmma  24011 7.3.2 */
typedef struct{
    uint8_t                     bMsgType  :3;
    uint8_t                     bSpare    :5;
    uint8_t                     bRpMR;
}S_L3_RpSmma;


/*RpAck  24011 7.3.3 */
typedef struct{
    uint8_t                     bMsgType  :3;
    uint8_t                     bSpare    :5;
    uint8_t                     bRpMR;

    uint8_t                     bRpUdFg :1;
    uint8_t                     bSpareX:7;

    S_L3_RpUserData          tRpUd;
}S_L3_RpAck;

/*RpError  24011 7.3.4 */
typedef struct{
    uint8_t                     bMsgType  :3;
    uint8_t                     bSpare    :5;
    uint8_t                     bRpMR;

    uint8_t                     bRpUdFg :1;
    uint8_t                     bSpareX:7;

    S_L3_RpCaus              tRpCaus;
    S_L3_RpUserData          tRpUd;
}S_L3_RpError;

/*36509_U.6.1.2 , LB_setup_DRB_IE*/
typedef struct{
    uint16_t                     wUlPDCPSduSize;
    uint8_t                     bDataRBId;
}S_L3_DRBIe;

/*36509_U.6.1.3 , UE_test_loop_mode_C_setup*/
typedef struct{
    uint8_t                     bMbsfnAreaId;
    uint8_t                     bMchId;
    uint8_t                     bLogChanlId;
}S_L3_UeTestLoopCSetup;

/*36509_U.6.1.3 , UE_test_loop_mode_B_LB_setup*/
typedef struct{
    uint8_t                     bIPPduDelay;
}S_L3_UeTestLoopBSetup;

/*36509_U.6.1.1 , UE_test_loop_mode_A_LB_setup*/
typedef struct{
    uint8_t                     bLBSetupListLen;
    S_L3_DRBIe           atLBSetupList[L3_MAX_MODEA_LB_ENTITIES_NUM];
}S_L3_UeTestLoopASetup;

/*36509_U.6.1.0 , UE_test_loop_mode_A/B_LB/C_setup*/
typedef struct{
    uint8_t                     bUeTestLoopMode;
    union {
    S_L3_UeTestLoopASetup     tModeASetup;
    S_L3_UeTestLoopBSetup     tModeBSetup;
    S_L3_UeTestLoopCSetup     tModeCSetup;
    }Val;
}S_L3_UeTestLoopLBSetup;


// @STD-PS(NAS_STD_CLOSE_LOOP, "NAS", "", "CLOSE TEST LOOP")
/*LteCloseLoop  36509 6.1 CLOSE_UE_TEST_LOOP */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

    S_L3_UeTestLoopLBSetup     tUETestLoopLBSetup;
}S_L3_LteCloseLoop;


// @STD-PS(NAS_STD_CLOSE_LOOPCMPLT, "NAS", "", "CLOSE TEST LOOP COMPLETE")
/*LteCloseLoopCmplt  36509 6.2 CLOSE_UE_TEST_LOOP_COMPLETE */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

}S_L3_LteCloseLoopCmplt;


// @STD-PS(NAS_STD_OPEN_LOOP, "NAS", "", "OPEN TEST LOOP")
/*LteOpenLoop  36509 6.3 OPEN_UE_TEST_LOOP */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

}S_L3_LteOpenLoop;


// @STD-PS(NAS_STD_OPEN_LOOPCMPLT, "NAS", "", "OPEN TEST LOOP COMPLETE")
/*LteOpenLoopCmpt  36509 6.4 OPEN_UE_TEST_LOOP_COMPLETE */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

}S_L3_LteOpenLoopCmpt;


// @STD-PS(NAS_STD_ACT_TESTMODE, "NAS", "", "ACTIVATE TEST LOOP")
/*LteActTestMode  36509 6.5 ACTIVATE_TEST_MODE */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

    uint8_t                     bUeTestLoopMode;
}S_L3_LteActTestMode;


// @STD-PS(NAS_STD_ACT_TESTMODECMPLT, "NAS", "", "ACTIVATE TEST LOOP COMPLETE")
/*LteActTestModeCmpt  36509 6.6 ACTIVATE_TEST_MODE_COMPLETE */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

}S_L3_LteActTestModeCmpt;


// @STD-PS(NAS_STD_DEACT_TESTMODE, "NAS", "", "DEACTIVATE TEST LOOP")
/*LteDeActTestMode  36509 6.7 DEACTIVATE_TEST_MODE */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

}S_L3_LteDeActTestMode;


// @STD-PS(NAS_STD_DEACT_TESTMODECMPLT, "NAS", "", "DEACTIVATE TEST LOOP COMPLETE")
/*LteDeActTestModeCmpt  36509 6.8 DEACTIVATE_TEST_MODE_COMPLETE */
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

}S_L3_LteDeActTestModeCmpt;

/*LteUeTestLoopModeCMbmsCntReq  36509 6.10 UE_TEST_LOOP_MODE_C_MBMS_PACKET_COUNTER_REQUEST*/
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

}S_L3_LteUeTestLoopModeCMbmsCntReq;

/*LteUeTestLoopModeCMbmsCntResp  36509 6.11 UE_TEST_LOOP_MODE_C_MBMS_PACKET_COUNTER_RESPONSE*/
typedef struct{
    uint8_t                     bPd  :4;
    uint8_t                     bSkip  :4;
    uint8_t                     bMsgType;

    uint32_t                 dwMbmsPacketCntVal;
}S_L3_LteUeTestLoopModeCMbmsCntResp;
#endif
