/****************************************************************************************************
 * Copyright (c) 2023, 芯昇科技有限公司
 * 
 * @file        psevent.h
 *
 * @brief       ps event
 *
 * @revision
 * Date                   Author            Notes
 * 2023-4-20
*****************************************************************************************************/

#ifndef EVENTDEF_H
#define EVENTDEF_H

#define EVENT_PS_BASE                            (uint16_t)0x0000

/*Message range definition*/

/*PS <--> PHY (65)*/
#define PS_PHY_EVENT_BASE                        (uint16_t)(EVENT_PS_BASE + 1)
#define PS_PHY_EVENT_END                         (uint16_t)(PS_PHY_EVENT_BASE + 65)
#define PS_PHY_RSP_EVENT                         (uint16_t)(PS_PHY_EVENT_BASE + 35)

/*ATI <--> NAS (300)*/
#define ATI_NAS_EVENT_BASE                       (uint16_t)(PS_PHY_EVENT_END + 1)
#define ATI_NAS_EVENT_END                        (uint16_t)(ATI_NAS_EVENT_BASE + 299)

/*ATC <--> IMS (100)*/
#define ATC_IMS_EVENT_BASE                       (uint16_t)(ATI_NAS_EVENT_END + 1)
#define ATC_IMS_EVENT_END                        (uint16_t)(ATC_IMS_EVENT_BASE + 99)

/*ATC <-->AS (100)*/
#define ATI_AS_EVENT_BASE                        (uint16_t)(ATC_IMS_EVENT_END + 1)
#define ATI_AS_EVENT_END                         (uint16_t)(ATI_AS_EVENT_BASE + 99)

/*NAS <--> RRC (100)*/
#define NAS_RRC_EVENT_BASE                       (uint16_t)(ATI_AS_EVENT_END + 1)
#define NAS_RRC_EVENT_END                        (uint16_t)(NAS_RRC_EVENT_BASE + 99)
#define NAS_RRC_RSP_EVENT                        (uint16_t)(NAS_RRC_EVENT_BASE + 50)

/*NAS <--> L2 (100)*/
#define NAS_L2_EVENT_BASE                        (uint16_t)(NAS_RRC_EVENT_END + 1)
#define NAS_L2_EVENT_END                         (uint16_t)(NAS_L2_EVENT_BASE + 99)

/*RRC <--> L2 (200)*/
#define RRC_L2_EVENT_BASE                        (uint16_t)(NAS_L2_EVENT_END + 1)
#define RRC_L2_EVENT_END                         (uint16_t)(RRC_L2_EVENT_BASE + 199)

/*AP <--> PDCP (30)*/
#define AP_PDCP_EVENT_BASE                       (uint16_t)(RRC_L2_EVENT_END + 1)
#define AP_PDCP_EVENT_END                        (uint16_t)(AP_PDCP_EVENT_BASE + 29)

/*NAS internal (50)*/
#define NAS_INTERNAL_EVENT_BASE                  (uint16_t)(AP_PDCP_EVENT_END + 1)
#define NAS_INTERNAL_EVENT_END                   (uint16_t)(NAS_INTERNAL_EVENT_BASE + 49)

/*RRC internal (20)*/
#define RRC_INTERNAL_EVENT_BASE                  (uint16_t)(NAS_INTERNAL_EVENT_END + 1)
#define RRC_INTERNAL_EVENT_END                   (uint16_t)(RRC_INTERNAL_EVENT_BASE + 20)

/*L2 internal (50)*/
#define L2_INTERNAL_EVENT_BASE                   (uint16_t)(RRC_INTERNAL_EVENT_END + 1)
#define L2_INTERNAL_EVENT_END                    (uint16_t)(L2_INTERNAL_EVENT_BASE + 49)

/* Module timer range */
#define EMM_TIMER_EVENT_BASE                     (uint16_t)(L2_INTERNAL_EVENT_END + 1)
#define EMM_TIMER_EVENT_END                      (uint16_t)(EMM_TIMER_EVENT_BASE + 39)

#define ESM_TIMER_EVENT_BASE                     (uint16_t)(EMM_TIMER_EVENT_END + 1)
#define ESM_TIMER_EVENT_END                      (uint16_t)(ESM_TIMER_EVENT_BASE + 19)

#define SMS_TIMER_EVENT_BASE                     (uint16_t)(ESM_TIMER_EVENT_END + 1)
#define SMS_TIMER_EVENT_END                      (uint16_t)(SMS_TIMER_EVENT_BASE + 19)

#define USIM_TIMER_EVENT_BASE                    (uint16_t)(SMS_TIMER_EVENT_END + 1)
#define USIM_TIMER_EVENT_END                     (uint16_t)(USIM_TIMER_EVENT_BASE + 19)

#define STM_TIMER_EVENT_BASE                     (uint16_t)(USIM_TIMER_EVENT_END + 1)
#define STM_TIMER_EVENT_END                      (uint16_t)(STM_TIMER_EVENT_BASE + 9)

#define RRC_TIMER_EVENT_BASE                     (uint16_t)(STM_TIMER_EVENT_END + 1)
#define RRC_TIMER_EVENT_END                      (uint16_t)(RRC_TIMER_EVENT_BASE + 30)

#define ROHCv1_TIMER_EVENT_BASE                  (uint16_t)(RRC_TIMER_EVENT_END + 1)
#define ROHCv1_TIMER_EVENT_END                   (uint16_t)(ROHCv1_TIMER_EVENT_BASE + 19)

#define PSA_TIMER_EVENT_BASE                     (uint16_t)(ROHCv1_TIMER_EVENT_END + 1)
#define PSA_TIMER_EVENT_END                      (uint16_t)(PSA_TIMER_EVENT_BASE + 19)

#define ROHCv2_TIMER_EVENT_BASE                  (uint16_t)(PSA_TIMER_EVENT_END + 1)
#define ROHCv2_TIMER_EVENT_END                   (uint16_t)(ROHCv2_TIMER_EVENT_BASE + 19)

#define PDCP_TIMER_EVENT_BASE                    (uint16_t)(ROHCv2_TIMER_EVENT_END + 1)
#define PDCP_TIMER_EVENT_END                     (uint16_t)(PDCP_TIMER_EVENT_BASE + 19)

#define RLC_TIMER_EVENT_BASE                     (uint16_t)(PDCP_TIMER_EVENT_END + 1)
#define RLC_TIMER_EVENT_END                      (uint16_t)(RLC_TIMER_EVENT_BASE + 9)

#define MAC_TIMER_EVENT_BASE                     (uint16_t)(RLC_TIMER_EVENT_END + 1)
#define MAC_TIMER_EVENT_END                      (uint16_t)(MAC_TIMER_EVENT_BASE + 9)

#define ERR_ROHCv1_BASE                          (uint16_t)(MAC_TIMER_EVENT_END + 1)
#define ERR_ROHCv1_END                           (uint16_t)(ERR_ROHCv1_BASE + 100)

#define ERR_ROHCv2_BASE                          (uint16_t)(ERR_ROHCv1_END + 1)
#define ERR_ROHCv2_END                           (uint16_t)(ERR_ROHCv2_BASE + 100)

/*NAS <--> IMS (100)*/
#define NAS_IMS_EVENT_BASE                       (uint16_t)(ERR_ROHCv2_END + 1)
#define NAS_IMS_EVENT_END                        (uint16_t)(NAS_IMS_EVENT_BASE + 99)

#define EVENT_PS_OTHER_BASE                      (uint16_t)(ERR_ROHCv2_END + 1)

/*Message range definition End*/

/*****************************Message range between ATI and NAS Start*******************************/
/*ATI <--> COMMON (10)*/
#define EVENT_ATI_COM_BASE                       (uint16_t)(ATI_NAS_EVENT_BASE)
#define ATI_COM_RSP_EVENT                        (uint16_t)(EVENT_ATI_COM_BASE + 4)
#define ATI_COM_EVENT_END                        (uint16_t)(ATI_COM_RSP_EVENT + 9)

/*ATI <--> EMM (100)*/
#define EVENT_ATI_EMM_BASE                       (uint16_t)(ATI_COM_EVENT_END)
#define ATI_EMM_RSP_EVENT                        (uint16_t)(EVENT_ATI_EMM_BASE + 50)
#define ATI_EMM_EVENT_END                        (uint16_t)(ATI_EMM_RSP_EVENT + 50)

/*ATI <--> ESM (50)*/
#define EVENT_ATI_ESM_BASE                       (uint16_t)(ATI_EMM_EVENT_END + 1)
#define ATI_ESM_RSP_EVENT                        (uint16_t)(EVENT_ATI_ESM_BASE + 30)
#define ATI_ESM_EVENT_END                        (uint16_t)(ATI_ESM_RSP_EVENT + 19)

/*ATI <--> USIM (60)*/
#define EVENT_ATI_USIM_BASE                      (uint16_t)(ATI_ESM_EVENT_END + 1)
#define ATI_USIM_RSP_EVENT                       (uint16_t)(EVENT_ATI_USIM_BASE + 30)
#define ATI_USIM_EVENT_END                       (uint16_t)(ATI_USIM_RSP_EVENT + 29)

/*ATI <--> SMS (50)*/
#define EVENT_ATI_SMS_BASE                       (uint16_t)(ATI_USIM_EVENT_END + 1)
#define ATI_SMS_RSP_EVENT                        (uint16_t)(EVENT_ATI_SMS_BASE + 30)
#define ATI_SMS_EVENT_END                        (uint16_t)(ATI_SMS_RSP_EVENT + 19)

/*****************************Message range between ATI and NAS End*****************************/
/*****************************Message range between ATI and AS Start*******************************/
/*ATI <--> RRC (20)*/
#define EVENT_ATI_RRC_BASE                       (uint16_t)(ATI_AS_EVENT_BASE + 1)
#define ATI_RRC_RSP_EVENT                        (uint16_t)(EVENT_ATI_RRC_BASE + 10)
#define ATI_RRC_EVENT_END                        (uint16_t)(ATI_RRC_RSP_EVENT + 9)

/*ATI <--> L2 (20)*/
#define EVENT_ATI_L2_BASE                       (uint16_t)(ATI_RRC_EVENT_END + 1)
#define ATI_L2_RSP_EVENT                        (uint16_t)(EVENT_ATI_L2_BASE + 10)
#define ATI_L2_EVENT_END                        (uint16_t)(ATI_L2_RSP_EVENT + 9)

/*****************************Message range between ATI and AS End*****************************/

/***************************** Message range between ATC and IMS Start***********************************/
#define ATC_IMS_ATD_REQ                          (uint16_t)(ATC_IMS_EVENT_BASE + 1)
#define ATC_IMS_ATA_REQ                          (uint16_t)(ATC_IMS_EVENT_BASE + 2)
#define ATC_IMS_ATH_REQ                          (uint16_t)(ATC_IMS_EVENT_BASE + 3)
#define ATC_IMS_CHUP_REQ                         (uint16_t)(ATC_IMS_EVENT_BASE + 4)
#define ATC_IMS_CVMOD_REQ                        (uint16_t)(ATC_IMS_EVENT_BASE + 5)
#define ATC_IMS_CVMOD_READ_REQ                   (uint16_t)(ATC_IMS_EVENT_BASE + 6)
#define ATC_IMS_VTS_REQ                          (uint16_t)(ATC_IMS_EVENT_BASE + 7)
#define ATC_IMS_IMSCWA_REQ                       (uint16_t)(ATC_IMS_EVENT_BASE + 8)
#define ATC_IMS_CCWA_REQ                         (uint16_t)(ATC_IMS_EVENT_BASE + 9)
#define ATC_IMS_CCWA_READ_REQ                    (uint16_t)(ATC_IMS_EVENT_BASE + 10)
#define ATC_IMS_IMSUSD_REQ                       (uint16_t)(ATC_IMS_EVENT_BASE + 11)
#define ATC_IMS_IMSUSD_READ_REQ                  (uint16_t)(ATC_IMS_EVENT_BASE + 12)
#define ATC_IMS_CLCC_REQ                         (uint16_t)(ATC_IMS_EVENT_BASE + 13)
#define ATC_IMS_CHLD_REQ                         (uint16_t)(ATC_IMS_EVENT_BASE + 14)
#define ATC_IMS_IMSPLUS_REQ                      (uint16_t)(ATC_IMS_EVENT_BASE + 15)
#define ATC_IMS_IMSCFC_REQ                       (uint16_t)(ATC_IMS_EVENT_BASE + 16)
#define ATC_IMS_IMSLCK_REQ                       (uint16_t)(ATC_IMS_EVENT_BASE + 17)
#define ATC_IMS_IMSD_REQ                         (uint16_t)(ATC_IMS_EVENT_BASE + 18)
#define ATC_IMS_IMSREF_REQ                       (uint16_t)(ATC_IMS_EVENT_BASE + 19)
#define ATC_IMS_IMSIPV_REQ                       (uint16_t)(ATC_IMS_EVENT_BASE + 20)
#define ATC_IMS_IMSAMRW_REQ                      (uint16_t)(ATC_IMS_EVENT_BASE + 21)
#define ATC_IMS_IMSAMRW_READ_REQ                 (uint16_t)(ATC_IMS_EVENT_BASE + 22)
#define ATC_IMS_SS_COMMON_REQ                    (uint16_t)(ATC_IMS_EVENT_BASE + 23)
#define ATC_IMS_SS_COMMON_RSP                    (uint16_t)(ATC_IMS_EVENT_BASE + 24)
#define ATC_IMS_IMSAPN_REQ                       (uint16_t)(ATC_IMS_EVENT_BASE + 25)
#define ATC_IMS_IMSAPN_READ_REQ                  (uint16_t)(ATC_IMS_EVENT_BASE + 26)
#define ATC_IMS_IMSCONFURI_REQ                   (uint16_t)(ATC_IMS_EVENT_BASE + 27)
#define ATC_IMS_IMSCONFURI_READ_REQ              (uint16_t)(ATC_IMS_EVENT_BASE + 28)
#define ATC_IMS_IMSTEST_REQ                      (uint16_t)(ATC_IMS_EVENT_BASE + 29)
#define ATC_IMS_CFUN_REQ                         (uint16_t)(ATC_IMS_EVENT_BASE + 30)
#define ATC_IMS_CMSMSOIN_REQ                     (uint16_t)(ATC_IMS_EVENT_BASE + 31)
#define ATC_IMS_CMSMSOIN_READ_REQ                (uint16_t)(ATC_IMS_EVENT_BASE + 32)
#define ATC_IMS_IMSCDU_REQ                       (uint16_t)(ATC_IMS_EVENT_BASE + 33)
#define ATC_IMS_CMIMSSTATE_READ_REQ              (uint16_t)(ATC_IMS_EVENT_BASE + 34)
#ifdef IMS_AP_CFW_INTERFACE
#define CFW_IMS_API_CC_SETUP_REQ                 (uint16_t)(ATC_IMS_EVENT_BASE + 36)
#define CFW_IMS_API_CC_CONNECT_REQ               (uint16_t)(ATC_IMS_EVENT_BASE + 37)
#define CFW_IMS_API_CC_DISC_REQ                  (uint16_t)(ATC_IMS_EVENT_BASE + 38)
#define CFW_IMS_API_CC_RELEASE_REQ               (uint16_t)(ATC_IMS_EVENT_BASE + 39)
#define CFW_IMS_API_CC_RELEASECMPLT_REQ          (uint16_t)(ATC_IMS_EVENT_BASE + 40)
#define CFW_IMS_API_CC_STARTDTMF_REQ             (uint16_t)(ATC_IMS_EVENT_BASE + 41)
#define CFW_IMS_API_CC_STOPDTMF_REQ              (uint16_t)(ATC_IMS_EVENT_BASE + 42)
#define CFW_IMS_API_CC_CALLHOLD_REQ              (uint16_t)(ATC_IMS_EVENT_BASE + 43)
#define CFW_IMS_API_CC_CALLRETRIEVE_REQ          (uint16_t)(ATC_IMS_EVENT_BASE + 44)
#define CFW_IMS_API_CC_CALLSWAP_REQ              (uint16_t)(ATC_IMS_EVENT_BASE + 45)
#define CFW_IMS_API_CC_FACILITY_REQ              (uint16_t)(ATC_IMS_EVENT_BASE + 46)
#define CFW_IMS_API_CC_AUDIO_ON_REQ              (uint16_t)(ATC_IMS_EVENT_BASE + 47)
#define CFW_IMS_API_CC_STOP_AUTIO_REQ            (uint16_t)(ATC_IMS_EVENT_BASE + 48)
#define CFW_IMS_API_SS_ACT_REQ                   (uint16_t)(ATC_IMS_EVENT_BASE + 49)
#define CFW_IMS_API_SMS_SET_PRI_REQ              (uint16_t)(ATC_IMS_EVENT_BASE + 50)
#define CFW_IMS_MAIN_TO_SS_ACT_REQ               (uint16_t)(ATC_IMS_EVENT_BASE + 51)
#define CFW_IMS_API_SET_ISIM_REQ                 (uint16_t)(ATC_IMS_EVENT_BASE + 52)


#endif
/*****************************Message range between ATC and IMS End*****************************/


/*****************************Message range between NAS and IMS Start*******************************/

/*IMS <--> ATI (20)*/
#define EVENT_IMS_ATI_BASE                       (uint16_t)(NAS_IMS_EVENT_BASE)
#define IMS_ATI_RSP_EVENT                        (uint16_t)(EVENT_IMS_ATI_BASE + 10)
#define IMS_ATI_EVENT_END                        (uint16_t)(IMS_ATI_RSP_EVENT + 9)

/*IMS <--> EMM (20)*/
#define EVENT_IMS_EMM_BASE                       (uint16_t)(IMS_ATI_EVENT_END + 1)
#define IMS_EMM_RSP_EVENT                        (uint16_t)(EVENT_IMS_EMM_BASE + 10)
#define IMS_EMM_EVENT_END                        (uint16_t)(IMS_EMM_RSP_EVENT + 9)

/*IMS <--> SM (20)*/
#define EVENT_IMS_SM_BASE                        (uint16_t)(IMS_EMM_EVENT_END + 1)
#define IMS_SM_RSP_EVENT                         (uint16_t)(EVENT_IMS_SM_BASE + 10)
#define IMS_SM_EVENT_END                         (uint16_t)(IMS_SM_RSP_EVENT + 9)

/*IMS <--> USIM (20)*/
#define EVENT_IMS_USIM_BASE                      (uint16_t)(IMS_SM_EVENT_END + 1)
#define IMS_USIM_RSP_EVENT                       (uint16_t)(EVENT_IMS_USIM_BASE + 10)
#define IMS_USIM_EVENT_END                       (uint16_t)(IMS_USIM_RSP_EVENT + 9)

/*IMS <--> SMS (10)*/
#define EVENT_IMS_SMS_BASE                       (uint16_t)(IMS_USIM_EVENT_END + 1)
#define IMS_SMS_RSP_EVENT                        (uint16_t)(EVENT_IMS_SMS_BASE + 5)
#define IMS_SMS_EVENT_END                        (uint16_t)(IMS_SMS_RSP_EVENT + 4)


/*****************************Message range between NAS and IMS End*****************************/


/***************************** NAS internal message Start***********************************/
/*EMM <--> USIM (10)*/
#define EVENT_EMM_USIM_BASE                      (uint16_t)(NAS_INTERNAL_EVENT_BASE + 0)
#define EMM_USIM_RSP_EVENT                       (uint16_t)(EVENT_EMM_USIM_BASE + 4)
#define EMM_USIM_EVENT_END                       (uint16_t)(EMM_USIM_RSP_EVENT + 5)

/*SMS <--> USIM (10)*/
#define EVENT_SMS_USIM_BASE                      (uint16_t)(EMM_USIM_EVENT_END + 1)
#define SMS_USIM_EVENT_END                       (uint16_t)(EVENT_SMS_USIM_BASE + 9)

/*EMM <--> SMS (10)*/
#define EVENT_EMM_SMS_BASE                       (uint16_t)(SMS_USIM_EVENT_END + 1)
#define EMM_SMS_EVENT_END                        (uint16_t)(EVENT_EMM_SMS_BASE + 9)

/*EMM <-->  ESM (10)*/
#define EVENT_EMM_ESM_BASE                       (uint16_t)(EMM_SMS_EVENT_END + 1)
#define EMM_ESM_EVENT_END                        (uint16_t)(EVENT_EMM_ESM_BASE + 9)

/*USIM internal message range (12)*/
#define EVENT_USIM_INNER_BASE                    (uint16_t)(EMM_ESM_EVENT_END + 1)
#define USIM_INNER_EVENT_END                     (uint16_t)(EVENT_USIM_INNER_BASE + 12)

/***************************** NAS internal message End***********************************/

/***************************** ESM/EMM and PDCP***********************************/
/*ESM <-->  PDCP (10)*/
#define ESM_PDCP_EVENT_BASE                      (uint16_t)(NAS_L2_EVENT_BASE + 0)
#define ESM_PDCP_EVENT_END                       (uint16_t)(ESM_PDCP_EVENT_BASE + 9)

/*EMM <-->  PDCP (10)*/
#define EMM_PDCP_EVENT_BASE                      (uint16_t)(ESM_PDCP_EVENT_END + 0)
#define EMM_PDCP_EVENT_END                       (uint16_t)(EMM_PDCP_EVENT_BASE + 9)

/***************************** ESM/EMM and PDCP End***********************************/
#define RRC_PDCP_EVENT_BASE                      (uint16_t)(RRC_L2_EVENT_BASE + 0)
#define RRC_PDCP_EVENT_END                       (uint16_t)(RRC_PDCP_EVENT_BASE + 15)

#define RRC_RLC_EVENT_BASE                       (uint16_t)(RRC_PDCP_EVENT_END + 1)
#define RRC_RLC_EVENT_END                        (uint16_t)(RRC_RLC_EVENT_BASE + 10)

#define RRC_MAC_EVENT_BASE                       (uint16_t)(RRC_RLC_EVENT_END + 1)
#define RRC_MAC_EVENT_END                        (uint16_t)(RRC_MAC_EVENT_BASE + 15)

#define RRC_L2_RSP_EVENT                         (uint16_t)(RRC_MAC_EVENT_END + 1)
#define RRC_L2_RSP_END                           (uint16_t)(RRC_L2_RSP_EVENT + 20)

/*---------------------------------------------------------------------------*/

#define PRI_TEST_EVENT_BASE                      (uint16_t)(EVENT_PS_OTHER_BASE + 1)//UICC,NV,IMSI
#define PRI_TEST_EVENT_END                       (uint16_t)(PRI_TEST_EVENT_BASE + 19)

#define TC_EVENT_BASE                            (uint16_t)(PRI_TEST_EVENT_END + 1)
#define TC_EVENT_END                             (uint16_t)(TC_EVENT_BASE + 29)


#define SIGTRACE_EVENT_BASE                      (uint16_t)(TC_EVENT_END + 1)
#define SIGTRACE_EVENT_END                       (uint16_t)(SIGTRACE_EVENT_BASE + 100)

#define EUDBG_EVENT_BASE                         (uint16_t)(SIGTRACE_EVENT_END + 1)
#define EUDBG_EVENT_END                          (uint16_t)(EUDBG_EVENT_BASE + 19)


#define NUNIT_ESM_EVENT_BASE                     (uint16_t)(EUDBG_EVENT_BASE + 1500)
#define NUNIT_ESM_RSP_EVENT                      (uint16_t)(NUNIT_ESM_EVENT_BASE + 9)
#define NUNIT_ESM_EVENT_END                      (uint16_t)(NUNIT_ESM_EVENT_BASE + 29)

#define NUNIT_EMM_EVENT_BASE                     (uint16_t)(NUNIT_ESM_EVENT_END + 1)
#define NUNIT_EMM_RSP_EVENT                      (uint16_t)(NUNIT_EMM_EVENT_BASE + 5)
#define NUNIT_EMM_EVENT_END                      (uint16_t)(NUNIT_EMM_EVENT_BASE + 29)

#define ENB_EMM_ESM_EVENT_BASE                   (uint16_t)(NUNIT_EMM_EVENT_END + 1)
#define ENB_EMM_ESM_RSP_EVENT                    (uint16_t)(ENB_EMM_ESM_EVENT_BASE + 9)
#define ENB_EMM_ESM_EVENT_END                    (uint16_t)(ENB_EMM_ESM_EVENT_BASE + 19)

#define ENB_RRC_EMM_EVENT_BASE                   (uint16_t)(ENB_EMM_ESM_EVENT_END + 1)
#define ENB_RRC_EMM_RSP_EVENT                    (uint16_t)(ENB_RRC_EMM_EVENT_BASE + 9)
#define ENB_RRC_EMM_EVENT_END                    (uint16_t)(ENB_RRC_EMM_EVENT_BASE + 19)

#define ENB_RRC_EVENT_BASE                       (uint16_t)(ENB_RRC_EMM_EVENT_END + 1)
#define ENB_RRC_RSP_EVENT                        (uint16_t)(ENB_RRC_EVENT_BASE + 25)
#define ENB_RRC_EVENT_END                        (uint16_t)(ENB_RRC_EVENT_BASE + 34)

#define ENRRC_ENPDCP_EVENT_BASE                  (uint16_t)(ENB_RRC_EVENT_END + 1)
#define ENRRC_ENPDCP_RSP_EVENT                   (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 20)
#define ENRRC_ENPDCP_EVENT_END                   (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 44)

#define PS_ENDCP_EVENT_BASE                      (uint16_t)(ENRRC_ENPDCP_EVENT_END + 1)
#define PS_ENDCP_RSP_EVENT                       (uint16_t)(PS_ENDCP_EVENT_BASE + 9)
#define PS_ENDCP_EVENT_END                       (uint16_t)(PS_ENDCP_EVENT_BASE + 19)


#define ENPDCP_TIMER_EVENT_BASE                  (uint16_t)(PS_ENDCP_EVENT_END + 1)
#define ENPDCP_TIMER_EVENT_END                   (uint16_t)(ENPDCP_TIMER_EVENT_BASE + 19)


#define ENRLC_TIMER_EVENT_BASE                   (uint16_t)(ENPDCP_TIMER_EVENT_END + 1)
#define ENRLC_TIMER_EVENT_END                    (uint16_t)(ENRLC_TIMER_EVENT_BASE + 9)


#define ENRRC_ENRLC_EVENT_BASE                   (uint16_t)(ENRLC_TIMER_EVENT_END + 1)
#define ENRRC_ENRLC_RSP_EVENT                    (uint16_t)(ENRRC_ENRLC_EVENT_BASE + 9)
#define ENRRC_ENRLC_EVENT_END                    (uint16_t)(ENRRC_ENRLC_EVENT_BASE + 19)

#define ENMAC_EPHY_EVENT_BASE                    (uint16_t)(ENRRC_ENRLC_EVENT_END + 1)
#define ENMAC_EPHY_RSP_EVENT                     (uint16_t)(ENMAC_EPHY_EVENT_BASE + 9)
#define ENMAC_EPHY_EVENT_END                     (uint16_t)(ENMAC_EPHY_EVENT_BASE + 19)

#define ENRRC_ENMAC_EVENT_BASE                   (uint16_t)(ENMAC_EPHY_EVENT_END + 1)
#define ENRRC_ENMAC_RSP_EVENT                    (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 9)
#define ENRRC_ENMAC_EVENT_END                    (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 19)

#define ENRRC_EPHY_EVENT_BASE                    (uint16_t)(ENRRC_ENMAC_EVENT_END + 1)
#define ENRRC_EPHY_RSP_EVENT                     (uint16_t)(ENRRC_EPHY_EVENT_BASE + 9)
#define ENRRC_EPHY_EVENT_END                     (uint16_t)(ENRRC_EPHY_EVENT_BASE + 19)

#define NUNIT_EPHY_EVENT_BASE                    (uint16_t)(ENRRC_EPHY_EVENT_END + 1)
#define NUNIT_EPHY_RSP_EVENT                     (uint16_t)(NUNIT_EPHY_EVENT_BASE + 10)
#define NUNIT_EPHY_EVENT_END                     (uint16_t)(NUNIT_EPHY_EVENT_BASE + 19)

#define NUNIT_ENMAC_EVENT_BASE                   (uint16_t)(NUNIT_EPHY_EVENT_END + 1)
#define NUNIT_ENMAC_RSP_EVENT                    (uint16_t)(NUNIT_ENMAC_EVENT_BASE + 10)
#define NUNIT_ENMAC_EVENT_END                    (uint16_t)(NUNIT_ENMAC_EVENT_BASE + 19)

#define ENPSA_ENPDCP_EVENT_BASE                  (uint16_t)(NUNIT_ENMAC_EVENT_END + 1)
#define ENPSA_ENPDCP_RSP_EVENT                   (uint16_t)(ENPSA_ENPDCP_EVENT_BASE + 10)
#define ENPSA_ENPDCP_EVENT_END                   (uint16_t)(ENPSA_ENPDCP_EVENT_BASE + 19)

#define NUNIT_SIMULPSA_EVENT_BASE                (uint16_t)(ENPSA_ENPDCP_EVENT_END + 1)
#define NUNIT_SIMULPSA_RSP_EVENT                 (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 10)
#define NUNIT_SIMULPSA_EVENT_END                 (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 19)

#define NUNIT_SIMULENPSA_EVENT_BASE              (uint16_t)(NUNIT_SIMULPSA_EVENT_END + 1)
#define NUNIT_SIMULENPSA_RSP_EVENT               (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 10)
#define NUNIT_SIMULENPSA_EVENT_END               (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 19)

/* =========================================================================

=========================================================================*/
#define LTE_GCF_NUNIT_EVENT_BASE                 (uint16_t)(NUNIT_SIMULENPSA_EVENT_END + 1)
#define LTE_GCF_NUNIT_RSP_EVENT                  (uint16_t)(LTE_GCF_NUNIT_EVENT_BASE + 10)
#define LTE_GCF_NUNIT_EVENT_END                  (uint16_t)(LTE_GCF_NUNIT_EVENT_BASE + 19)

#define LTE_GCF_TIMER_EVENT_BASE                 (uint16_t)(LTE_GCF_NUNIT_EVENT_END + 1)
#define LTE_GCF_TIMER_EVENT_END                  (uint16_t)(LTE_GCF_TIMER_EVENT_BASE + 19)



#define NUNIT_ENRLC_EVENT_BASE                   (uint16_t)(LTE_GCF_TIMER_EVENT_END + 1)
#define NUNIT_ENRLC_RSP_EVENT                    (uint16_t)(NUNIT_ENRLC_EVENT_BASE + 10)
#define NUNIT_ENRLC_EVENT_END                    (uint16_t)(NUNIT_ENRLC_EVENT_BASE + 19)

#define ENPDCP_ENRLC_EVENT_BASE                  (uint16_t)(NUNIT_ENRLC_EVENT_END + 1)
#define ENPDCP_ENRLC_EVENT_END                   (uint16_t)(ENPDCP_ENRLC_EVENT_BASE + 9)

#ifdef EMBMS_SUPPORT
#define ENMEL2_EVENT_BASE                        (uint16_t)(ENPDCP_ENRLC_EVENT_END + 1)
#define ENMEL2_RSP_EVENT                         (uint16_t)(ENMEL2_EVENT_BASE + 5)
#define ENMEL2_EVENT_END                         (uint16_t)(ENMEL2_EVENT_BASE + 6)
#endif

#define TOOL_FUN_TRACE_BASE                      (uint16_t)(ENPDCP_ENRLC_EVENT_END + 8)

#ifdef CFW_API_SUPPORT
#define CFW_API_NAS_BASE                         (uint16_t)(TOOL_FUN_TRACE_BASE + 50)
#define CFW_API_NAS_END                          (uint16_t)(CFW_API_NAS_BASE + 100)
#endif

/***************************** Event ID Start***********************************/

/*ATI <--> COMMON (10)*/
#define ATI_COMMON_CNF                              (uint16_t)(EVENT_ATI_COM_BASE+0)
#define ATI_COMMON_SMS_CNF                          (uint16_t)(EVENT_ATI_COM_BASE+1)

/*ATI <--> RRC (20)*/
#define ATI_RRC_CMIMSBAR_SET_REQ                    (uint16_t)(EVENT_ATI_RRC_BASE+ 0)
#define ATI_RRC_CMSINR_REQ                          (uint16_t)(EVENT_ATI_RRC_BASE+ 1)
#define ATI_RRC_CESQ_REQ                            (uint16_t)(EVENT_ATI_RRC_BASE+ 2)
#define ATI_RRC_CSQ_REQ                             (uint16_t)(EVENT_ATI_RRC_BASE+ 3)
#define ATI_RRC_QCSQ_REQ                            (uint16_t)(EVENT_ATI_RRC_BASE+ 4)

#define RRC_ATI_CMLTENOCELL_IND                     (uint16_t)(ATI_RRC_RSP_EVENT+ 0)
#define RRC_ATI_CMIMSBAR_SET_CNF                    (uint16_t)(ATI_RRC_RSP_EVENT+ 1)
#define RRC_ATI_CMSINR_CNF                          (uint16_t)(ATI_RRC_RSP_EVENT+ 2)
#define RRC_ATI_CESQ_CNF                            (uint16_t)(ATI_RRC_RSP_EVENT+ 3)
#define RRC_ATI_CESQ_IND                            (uint16_t)(ATI_RRC_RSP_EVENT+ 4)
#define RRC_ATI_CSQ_CNF                             (uint16_t)(ATI_RRC_RSP_EVENT+ 5)
#define RRC_ATI_QCSQ_CNF                            (uint16_t)(ATI_RRC_RSP_EVENT+ 6)
#define RRC_ATI_QCSQ_IND                            (uint16_t)(ATI_RRC_RSP_EVENT+ 7)

/*ATI <--> L2 (20)*/
#define ATI_L2_QGDCNT_REQ                           (uint16_t)(EVENT_ATI_L2_BASE+ 0)
#define ATI_L2_QAUGDCNT_REQ                         (uint16_t)(EVENT_ATI_L2_BASE+ 1)

#define L2_ATI_QGDCNT_QUERY_CNF                     (uint16_t)(ATI_L2_RSP_EVENT+ 0)
#define L2_ATI_QAUGDCNT_QUERY_CNF                   (uint16_t)(ATI_L2_RSP_EVENT+ 1)


/*ATI <--> EMM (50)*/
#define ATI_EMM_RF_CTRL_REQ                         (uint16_t)(EVENT_ATI_EMM_BASE+0)
#define ATI_EMM_SEARCH_PLMN_REQ                     (uint16_t)(EVENT_ATI_EMM_BASE+1)
#define ATI_EMM_LIST_PLMN_REQ                       (uint16_t)(EVENT_ATI_EMM_BASE+2)
#define ATI_EMM_ACTIVE_REQ                          (uint16_t)(EVENT_ATI_EMM_BASE+3)
#define ATI_EMM_CEMODE_REQ                          (uint16_t)(EVENT_ATI_EMM_BASE+4)
#define ATI_EMM_SETSMSOVERIP_REQ                    (uint16_t)(EVENT_ATI_EMM_BASE+5)
#define ATI_EMM_CEREG_QUERY_REQ                     (uint16_t)(EVENT_ATI_EMM_BASE+6)
#define ATI_EMM_ABORT_REQ                           (uint16_t)(EVENT_ATI_EMM_BASE+7)
#define ATI_EMM_SYS_INFO_REQ                        (uint16_t)(EVENT_ATI_EMM_BASE+8)
#define ATI_EMM_CGATT_QUERY_REQ                     (uint16_t)(EVENT_ATI_EMM_BASE+9)
#define ATI_EMM_CEER_CAUSE_REQ                      (uint16_t)(EVENT_ATI_EMM_BASE+10)
#define ATI_EMM_CFUN_QUERY_REQ                      (uint16_t)(EVENT_ATI_EMM_BASE+11)
#define ATI_EMM_COPS_QUERY_REQ                      (uint16_t)(EVENT_ATI_EMM_BASE+12)
#define ATI_EMM_CEDRXRDP_REQ                        (uint16_t)(EVENT_ATI_EMM_BASE+13)
#define ATI_EMM_CEMODE_QUERY_REQ                    (uint16_t)(EVENT_ATI_EMM_BASE+14)
#define ATI_EMM_CEDRXS_SET_REQ                      (uint16_t)(EVENT_ATI_EMM_BASE+15)
#define ATI_EMM_CEDRXS_QUERY_REQ                    (uint16_t)(EVENT_ATI_EMM_BASE+16)
#define ATI_EMM_CPSMS_SET_REQ                       (uint16_t)(EVENT_ATI_EMM_BASE+17)
#define ATI_EMM_CPSMS_QUERY_REQ                     (uint16_t)(EVENT_ATI_EMM_BASE+18)

#define ATI_EMM_LIST_PLMN_CNF                       (uint16_t)(ATI_EMM_RSP_EVENT+ 0)
#define ATI_EMM_LIST_PLMN_CMNCELL_CNF               (uint16_t)(ATI_EMM_RSP_EVENT+ 1)
#define ATI_EMM_CEREG_QUERY_CNF                     (uint16_t)(ATI_EMM_RSP_EVENT+ 2)
#define ATI_EMM_CEREG_IND                           (uint16_t)(ATI_EMM_RSP_EVENT+ 3)
#define ATI_EMM_MM_INFO_IND                         (uint16_t)(ATI_EMM_RSP_EVENT+ 4)
#define ATI_EMM_CIREPI_IND                          (uint16_t)(ATI_EMM_RSP_EVENT+ 5)
#define ATI_EMM_EMER_BEARER_IND                     (uint16_t)(ATI_EMM_RSP_EVENT+ 6)
#define ATI_EMM_CSCON_IND                           (uint16_t)(ATI_EMM_RSP_EVENT+ 7)
#define ATI_EMM_SYS_INFO_CNF                        (uint16_t)(ATI_EMM_RSP_EVENT+ 8)
#define ATI_EMM_CGATT_QUERY_CNF                     (uint16_t)(ATI_EMM_RSP_EVENT+ 9)
#define ATI_EMM_CEER_CAUSE_CNF                      (uint16_t)(ATI_EMM_RSP_EVENT+ 10)
#define ATI_EMM_CFUN_QUERY_CNF                      (uint16_t)(ATI_EMM_RSP_EVENT+ 11)
#define ATI_EMM_COPS_QUERY_CNF                      (uint16_t)(ATI_EMM_RSP_EVENT+ 12)
#define ATI_EMM_CEDRXRDP_CNF                        (uint16_t)(ATI_EMM_RSP_EVENT+ 13)
#define ATI_EMM_CEMODE_QUERY_CNF                    (uint16_t)(ATI_EMM_RSP_EVENT+ 14)
#define ATI_EMM_CEDRXS_QUERY_CNF                    (uint16_t)(ATI_EMM_RSP_EVENT+ 15)
#define ATI_EMM_CPSMS_QUERY_CNF                     (uint16_t)(ATI_EMM_RSP_EVENT+ 16)
#define ATI_EMM_CEDRXP_IND                          (uint16_t)(ATI_EMM_RSP_EVENT+ 17)

/*ATI <--> ESM (50)*/
#define ATI_ESM_PDP_ACTIVATE_REQ                    (uint16_t)(EVENT_ATI_ESM_BASE+0)
#define ATI_ESM_PDP_DEACTIVATE_REQ                  (uint16_t)(EVENT_ATI_ESM_BASE+1)
#define ATI_ESM_PDP_MODIFY_REQ                      (uint16_t)(EVENT_ATI_ESM_BASE+2)
#define ATI_ESM_ACTIVATED_CID_QUERY_REQ             (uint16_t)(EVENT_ATI_ESM_BASE+3)
#define ATI_ESM_PDP_STATUS_QUERY_REQ                (uint16_t)(EVENT_ATI_ESM_BASE+4)
#define ATI_ESM_PDP_ADDR_QUERY_REQ                  (uint16_t)(EVENT_ATI_ESM_BASE+5)
#define ATI_ESM_CGCONTRDP_REQ                       (uint16_t)(EVENT_ATI_ESM_BASE+6)
#define ATI_ESM_CGSCONTRDP_REQ                      (uint16_t)(EVENT_ATI_ESM_BASE+7)
#define ATI_ESM_CGTFTRDP_REQ                        (uint16_t)(EVENT_ATI_ESM_BASE+8)
#define ATI_ESM_EPS_QOS_QUERY_REQ                   (uint16_t)(EVENT_ATI_ESM_BASE+9)
#define ATI_ESM_TFT_SET_REQ                         (uint16_t)(EVENT_ATI_ESM_BASE+10)
#define ATI_ESM_EQOS_SET_REQ                        (uint16_t)(EVENT_ATI_ESM_BASE+11)
#define ATI_ESM_CGEQOS_QUERY_REQ                    (uint16_t)(EVENT_ATI_ESM_BASE+12)
#define ATI_ESM_CGTFT_QUERY_REQ                     (uint16_t)(EVENT_ATI_ESM_BASE+13)

#ifdef CFW_API_SUPPORT
#define ATI_ESM_DEFPCOCFG_REQ                       (uint16_t)(EVENT_ATI_ESM_BASE+14)
#endif

#define ATI_ESM_CMGIPDNS_IND                        (uint16_t)(ATI_ESM_RSP_EVENT+0)
#define ATI_ESM_ACTIVATED_CID_QUERY_CNF             (uint16_t)(ATI_ESM_RSP_EVENT+1)
#define ATI_ESM_PDP_STATUS_QUERY_CNF                (uint16_t)(ATI_ESM_RSP_EVENT+2)
#define ATI_ESM_PDP_ADDR_QUERY_CNF                  (uint16_t)(ATI_ESM_RSP_EVENT+3)
#define ATI_ESM_CGCONTRDP_SET_CNF                   (uint16_t)(ATI_ESM_RSP_EVENT+4)
#define ATI_ESM_CGSCONTRDP_SET_CNF                  (uint16_t)(ATI_ESM_RSP_EVENT+5)
#define ATI_ESM_CGTFTRDP_SET_CNF                    (uint16_t)(ATI_ESM_RSP_EVENT+6)
#define ATI_ESM_EPS_QOS_QUERY_CNF                   (uint16_t)(ATI_ESM_RSP_EVENT+7)
#define ATI_ESM_CGEV_IND                            (uint16_t)(ATI_ESM_RSP_EVENT+8)
#define ATI_ESM_CGEQOS_QUERY_CNF                    (uint16_t)(ATI_ESM_RSP_EVENT+9)
#define ATI_ESM_CGTFT_QUERY_CNF                     (uint16_t)(ATI_ESM_RSP_EVENT+10)


/*ATI <--> USIM (60)*/
#define ATI_USIM_UPDATE_ITEM_REQ                    (uint16_t)(EVENT_ATI_USIM_BASE + 0)
#define ATI_USIM_CPIN_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 1)
#define ATI_USIM_CMRAP_REQ                          (uint16_t)(EVENT_ATI_USIM_BASE + 2)
#define ATI_USIM_CLCK_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 3)
#define ATI_USIM_CPWD_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 4)
#define ATI_USIM_CSIM_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 5)
#define ATI_USIM_CRSM_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 6)
#define ATI_USIM_CCHO_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 7)
#define ATI_USIM_CGLA_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 8)
#define ATI_USIM_CCHC_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 9)
#define ATI_USIM_CUSATT_REQ                         (uint16_t)(EVENT_ATI_USIM_BASE + 10)
#define ATI_USIM_CUSATE_REQ                         (uint16_t)(EVENT_ATI_USIM_BASE + 11)
#define ATI_USIM_CEID_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 12)
#define ATI_USIM_CNUM_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 13)
#define ATI_USIM_CMUINIT_REQ                        (uint16_t)(EVENT_ATI_USIM_BASE + 14)
#define ATI_USIM_ICCID_REQ                          (uint16_t)(EVENT_ATI_USIM_BASE + 15)
#define ATI_USIM_CIMI_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 16)
#define ATI_USIM_CPOL_SET_REQ                       (uint16_t)(EVENT_ATI_USIM_BASE + 17)
#define ATI_USIM_CPOL_QUERY_REQ                     (uint16_t)(EVENT_ATI_USIM_BASE + 18)
#define ATI_USIM_CSCA_REQ                           (uint16_t)(EVENT_ATI_USIM_BASE + 19)
#define ATI_USIM_CMSLOT_REQ                         (uint16_t)(EVENT_ATI_USIM_BASE + 20)
#define ATI_USIM_USIM_POWER_OFF_REQ                 (uint16_t)(EVENT_ATI_USIM_BASE + 21)


#define ATI_USIM_CUSATE_CNF                         (uint16_t)(ATI_USIM_RSP_EVENT + 0)
#define ATI_USIM_USAT_PROC_CMD_IND                  (uint16_t)(ATI_USIM_RSP_EVENT + 1)
#define ATI_USIM_USAT_SESSION_END_IND               (uint16_t)(ATI_USIM_RSP_EVENT + 2)
#define ATI_USIM_USAT_REFRESH_IND                   (uint16_t)(ATI_USIM_RSP_EVENT + 3)
#define ATI_USIM_CRSM_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 4)
#define ATI_USIM_CSIM_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 5)
#define ATI_USIM_CMRAP_CNF                          (uint16_t)(ATI_USIM_RSP_EVENT + 6)
#define ATI_USIM_CLCK_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 7)
#define ATI_USIM_CCHO_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 8)
#define ATI_USIM_CGLA_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 9)
#define ATI_USIM_CCHC_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 10)
#define ATI_USIM_CEID_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 11)
#define ATI_USIM_INIT_STAT_IND                      (uint16_t)(ATI_USIM_RSP_EVENT + 12)
#define ATI_USIM_CMUSLOT_IND                        (uint16_t)(ATI_USIM_RSP_EVENT + 13)
#define ATI_USIM_SMS_INIT_IND                       (uint16_t)(ATI_USIM_RSP_EVENT + 14)
#define ATI_USIM_CNUM_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 15)
#define ATI_USIM_CMUINIT_CNF                        (uint16_t)(ATI_USIM_RSP_EVENT + 16)
#define ATI_USIM_CPIN_QUERY_CNF                     (uint16_t)(ATI_USIM_RSP_EVENT + 17)
#define ATI_USIM_ICCID_CNF                          (uint16_t)(ATI_USIM_RSP_EVENT + 18)
#define ATI_USIM_CIMI_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 19)
#define ATI_USIM_CPOL_QUERY_CNF                     (uint16_t)(ATI_USIM_RSP_EVENT + 20)
#define ATI_USIM_CSCA_CNF                           (uint16_t)(ATI_USIM_RSP_EVENT + 21)
#define ATI_USIM_CMSLOT_RSP                         (uint16_t)(ATI_USIM_RSP_EVENT + 22)


/*ATI <--> SMS (50)*/
#define ATI_SMS_CMGS_PDU_SEND_REQ                   (uint16_t)(EVENT_ATI_SMS_BASE+0)
#define ATI_SMS_CMSS_SEND_REQ                       (uint16_t)(EVENT_ATI_SMS_BASE+1)
#define ATI_SMS_CSMP_SET_REQ                        (uint16_t)(EVENT_ATI_SMS_BASE+2)
#define ATI_SMS_CNMI_SET_REQ                        (uint16_t)(EVENT_ATI_SMS_BASE+3)
#define ATI_SMS_CPMS_REQ                            (uint16_t)(EVENT_ATI_SMS_BASE+4)
#define ATI_SMS_CMGL_REQ                            (uint16_t)(EVENT_ATI_SMS_BASE+5)
#define ATI_SMS_CMGD_REQ                            (uint16_t)(EVENT_ATI_SMS_BASE+6)
#define ATI_SMS_CMGW_REQ                            (uint16_t)(EVENT_ATI_SMS_BASE+7)
#define ATI_SMS_CMGR_REQ                            (uint16_t)(EVENT_ATI_SMS_BASE+8)
#define ATI_SMS_CMMENA_REQ                          (uint16_t)(EVENT_ATI_SMS_BASE+9)
#ifdef CFW_API_SUPPORT
#define ATI_SMS_REC_RSP_REQ                         (uint16_t)(EVENT_ATI_SMS_BASE+10)
#endif

#define ATI_SMS_CMGL_PDU_ITEM_IND                   (uint16_t)(ATI_SMS_RSP_EVENT + 0)
#define ATI_SMS_CMGR_CNF                            (uint16_t)(ATI_SMS_RSP_EVENT + 1)
#define ATI_SMS_CMGD_TEST_CNF                       (uint16_t)(ATI_SMS_RSP_EVENT + 2)
#define ATI_SMS_CMSS_SEND_CNF                       (uint16_t)(ATI_SMS_RSP_EVENT + 3)
#define ATI_SMS_CPMS_CNF                            (uint16_t)(ATI_SMS_RSP_EVENT + 4)
#define ATI_SMS_CMGW_CNF                            (uint16_t)(ATI_SMS_RSP_EVENT + 5)
#define ATI_SMS_CMGS_PDU_SEND_CNF                   (uint16_t)(ATI_SMS_RSP_EVENT + 6)
#define ATI_SMS_RECV_STORE_MSG_IND                  (uint16_t)(ATI_SMS_RSP_EVENT + 7)
#define ATI_SMS_STATUS_REPORT_PDU_IND               (uint16_t)(ATI_SMS_RSP_EVENT + 8)
#define ATI_SMS_DELIVER_PDU_IND                     (uint16_t)(ATI_SMS_RSP_EVENT + 9)
#define ATI_SMS_CMMGSF_IND                          (uint16_t)(ATI_SMS_RSP_EVENT + 10)


#define EMM_USIM_POWERON_REQ                        (uint16_t)(EVENT_EMM_USIM_BASE+0)

#define EMM_USIM_POWERON_IND                        (uint16_t)(EMM_USIM_RSP_EVENT+1)
#define EMM_USIM_NOCARD_IND                         (uint16_t)(EMM_USIM_RSP_EVENT+2)
#define EMM_USIM_UNSYNC_IND                         (uint16_t)(EMM_USIM_RSP_EVENT+3)

#define USIM_SMS_USAT_SEND_SM_REQ                   (uint16_t)(EVENT_SMS_USIM_BASE+0)
#define USIM_SMS_MEM_AVAIL_REQ                      (uint16_t)(EVENT_SMS_USIM_BASE+1)
#define USIM_SMS_CNMI_MEM_CHECK_REQ                 (uint16_t)(EVENT_SMS_USIM_BASE+2)


#define EMM_SMS_DL_MESSAGE_IND                      (uint16_t)(EVENT_EMM_SMS_BASE+0)



#define ESM_PDCP_DEACTIVATE_REQ                  (uint16_t)(ESM_PDCP_EVENT_BASE + 0)
#define ESM_PDCP_DEL_USER_PLANE_BUFFER_DATA      (uint16_t)(ESM_PDCP_EVENT_BASE + 1)

#define PDCP_EMM_EST_REQ                         (uint16_t)(EMM_PDCP_EVENT_BASE+0)
#define EMM_PDCP_EST_REJ                         (uint16_t)(EMM_PDCP_EVENT_BASE+1)
#define EMM_PDCP_BAR_ALLEVIATE_NOTIFY            (uint16_t)(EMM_PDCP_EVENT_BASE+2)

#define AP_PDCP_DATA_REQ                         (uint16_t)(AP_PDCP_EVENT_BASE + 0)

#define ROHCv1_T_IR_EXPIRY                       (uint16_t)(ROHCv1_TIMER_EVENT_BASE + 0)
#define ROHCv1_T_SO2FO_EXPIRY                    (uint16_t)(ROHCv1_TIMER_EVENT_BASE + 1)
#define ENROHCv1_T_IR_EXPIRY                     (uint16_t)(ROHCv1_TIMER_EVENT_BASE + 2)
#define ENROHCv1_T_SO2FO_EXPIRY                  (uint16_t)(ROHCv1_TIMER_EVENT_BASE + 3)
#define ROHCv1_T_NACK_FDBK_CNT_EXPIRY            (uint16_t)(ROHCv1_TIMER_EVENT_BASE + 4)

#define ROHCv2_T_IR_EXPIRY                       (uint16_t)(ROHCv2_TIMER_EVENT_BASE + 0)
#define ENROHCv2_T_IR_EXPIRY                     (uint16_t)(ROHCv2_TIMER_EVENT_BASE + 1)


#define PSA_T_SWITCHLED_EXPIRY                   (uint16_t)(PSA_TIMER_EVENT_BASE + 0)
#define PSA_T_WAITDNSACK_EXPIRY                  (uint16_t)(PSA_TIMER_EVENT_BASE + 1)
#define PSA_T_WAITZSSACK_EXPIRY                  (uint16_t)(PSA_TIMER_EVENT_BASE + 2)
#define PSA_T_WAIT_BUF_EXPIRY                    (uint16_t)(PSA_TIMER_EVENT_BASE + 3)

#define STM_MEMAVAILD_EXPIRY                     (uint16_t)(STM_TIMER_EVENT_BASE + 0)


#define FOR_TEST_TEMP                               (uint16_t)(PRI_TEST_EVENT_BASE + 0)
#define TEST_SET_UICC_RLT                           (uint16_t)(PRI_TEST_EVENT_BASE + 1)
#define TEST_SET_UICC_DATA                          (uint16_t)(PRI_TEST_EVENT_BASE + 2)
#define TEST_SET_NV_DATA                            (uint16_t)(PRI_TEST_EVENT_BASE + 3)
#define TEST_SET_NV_DATA_IMEI                       (uint16_t)(PRI_TEST_EVENT_BASE + 4)
#define TEST_SET_NV_DATA_SPCLFUNC                   (uint16_t)(PRI_TEST_EVENT_BASE + 5)
#define TEST_SET_COMP_IND                           (uint16_t)(PRI_TEST_EVENT_BASE + 6)



#define TC_ACTIVE_TEST_REQ                          (uint16_t)(TC_EVENT_BASE + 0)
#define TC_ACTIVE_TEST_CNF                          (uint16_t)(TC_EVENT_BASE + 1)
#define TC_DEACTIVE_TEST_REQ                        (uint16_t)(TC_EVENT_BASE + 2)
#define TC_CLOSE_LOOP_REQ                           (uint16_t)(TC_EVENT_BASE + 3)
#define TC_CLOSE_LOOP_CNF                           (uint16_t)(TC_EVENT_BASE + 4)
#define TC_CLOSE_LOOP_REQ_URLC                      (uint16_t)(TC_EVENT_BASE + 5)
#define TC_OPEN_LOOP_REQ                            (uint16_t)(TC_EVENT_BASE + 6)
#define TC_CLOSE_LOOP_REQ_WRLC                      (uint16_t)(TC_EVENT_BASE + 7)
#define EMM_TC_TEST_CONTROL_REQ                     (uint16_t)(TC_EVENT_BASE + 8)
#define TC_EMM_TEST_CONTROL_CNF                     (uint16_t)(TC_EVENT_BASE + 9)
#define TC_OPEN_LOOP_TEST_REQ                       (uint16_t)(TC_EVENT_BASE + 10)
#define TC_CLOSE_LOOP_TEST_REQ                      (uint16_t)(TC_EVENT_BASE + 11)

#define EMM_T3440_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 0)
#define EMM_T3245_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 1)
#define EMM_T_SHHPLMN_EXPIRY                        (uint16_t)(EMM_TIMER_EVENT_BASE + 2)
#define EMM_T_NOCELL_EXPIRY                         (uint16_t)(EMM_TIMER_EVENT_BASE + 3)
#define EMM_T_LIMIT_EXPIRY                          (uint16_t)(EMM_TIMER_EVENT_BASE + 4)
#define EMM_T_PLMNLIST_EXPIRY                       (uint16_t)(EMM_TIMER_EVENT_BASE + 5)
#define EMM_T3416_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 6)
#define EMM_T3417_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 7)
#define EMM_T3418_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 8)
#define EMM_T3420_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 9)
#define EMM_T3421_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 10)
#define EMM_T3430_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 11)
#define EMM_T_POWEROFF_EXPIRY                       (uint16_t)(EMM_TIMER_EVENT_BASE + 12)
#define EMM_T_WAITRELIND_EXPIRY                     (uint16_t)(EMM_TIMER_EVENT_BASE + 13)
#define EMM_T3411_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 14)
#define EMM_T3402_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 15)
#define EMM_T3324_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 16)
#define EMM_T3412EXT_EXPIRY                         (uint16_t)(EMM_TIMER_EVENT_BASE + 17)
#define EMM_T3410_EXPIRY                            (uint16_t)(EMM_TIMER_EVENT_BASE + 18)
#define EMM_POWEROFF_DELAY_RESET                    (uint16_t)(EMM_TIMER_EVENT_BASE + 19)
#define EMM_T_EDRX_WAKE_EXPIRY                      (uint16_t)(EMM_TIMER_EVENT_BASE + 20)
#define EMM_T_PSEUDO_TAI_CHECK_EXPIRY               (uint16_t)(EMM_TIMER_EVENT_BASE + 21)
#define EMM_T_PSEUDO_TAI_RELEASE_EXPIRY             (uint16_t)(EMM_TIMER_EVENT_BASE + 22)



#define ESM_TIMER3480_EXPIRY                        (uint16_t)(ESM_TIMER_EVENT_BASE + 0)
#define ESM_TIMER3481_EXPIRY                        (uint16_t)(ESM_TIMER_EVENT_BASE + 1)
#define ESM_TIMER3482_EXPIRY                        (uint16_t)(ESM_TIMER_EVENT_BASE + 2)
#define ESM_TIMER3492_EXPIRY                        (uint16_t)(ESM_TIMER_EVENT_BASE + 3)
#define ESM_T_MTACTANSWER_EXPIRY                    (uint16_t)(ESM_TIMER_EVENT_BASE + 4)
#define ESM_T_PTIBUF_EXPIRY                         (uint16_t)(ESM_TIMER_EVENT_BASE + 6)
#define ESM_T_CMEST_EXPIRY                          (uint16_t)(ESM_TIMER_EVENT_BASE + 7)

#define SMS_TR1M_EXPIRY                             (uint16_t)(SMS_TIMER_EVENT_BASE+0)
#define SMS_TRAM_EXPIRY                             (uint16_t)(SMS_TIMER_EVENT_BASE+1)
#define SMS_TC1M_MO_EXPIRY                          (uint16_t)(SMS_TIMER_EVENT_BASE+2)
#define SMS_TMMS_EXPIRY                             (uint16_t)(SMS_TIMER_EVENT_BASE+3)
#define SMS_TR2M_EXPIRY                             (uint16_t)(SMS_TIMER_EVENT_BASE+4)
#define SMS_TC1M_MT_EXPIRY                          (uint16_t)(SMS_TIMER_EVENT_BASE+5)

#define USIM_POLL_TIMER_EXPIRY                      (uint16_t)(USIM_TIMER_EVENT_BASE+0)
#define USIM_SMS_MEM_AVAIL_EXPIRY                   (uint16_t)(USIM_TIMER_EVENT_BASE+1)
#define USIM_AUTO_POWER_DOWN_EXPIRY                 (uint16_t)(USIM_TIMER_EVENT_BASE+2)
#define USIM_DROP_RETRY_EXPIRY                      (uint16_t)(USIM_TIMER_EVENT_BASE+3)


#define USAT_PROACTIVE_CMD_IND                      (uint16_t)(EVENT_USIM_INNER_BASE + 0)
#define DRV_USIM_MOVECARD_IND                       (uint16_t)(EVENT_USIM_INNER_BASE + 1)
#define DRV_USIM_INSERTCARD_IND                     (uint16_t)(EVENT_USIM_INNER_BASE + 2)
#define USIM_PERFORM_INIT_STEP_REQ                  (uint16_t)(EVENT_USIM_INNER_BASE + 3)
#define USIM_START_AUTO_INIT_REQ                    (uint16_t)(EVENT_USIM_INNER_BASE + 4)
#define USIM_INIT_ISIM_APP_REQ                      (uint16_t)(EVENT_USIM_INNER_BASE + 5)
#define USIM_START_INIT_SMS_REQ                     (uint16_t)(EVENT_USIM_INNER_BASE + 6)
#define USIM_INIT_SMS_REC_REQ                       (uint16_t)(EVENT_USIM_INNER_BASE + 7)
#define USIM_CMGL_ITEM_PROC_REQ                     (uint16_t)(EVENT_USIM_INNER_BASE + 8)
#define USIM_CMGD_ITEM_PROC_REQ                     (uint16_t)(EVENT_USIM_INNER_BASE + 9)
#define USIM_SMS_STORE_RECV_SM_REQ                  (uint16_t)(EVENT_USIM_INNER_BASE + 10)
#define USAT_REFRESN_DETACH_END_IND                 (uint16_t)(EVENT_USIM_INNER_BASE + 11)
#define USIM_INIT_ISIM_FILE_REQ                     (uint16_t)(EVENT_USIM_INNER_BASE + 12)//IMS_SUPPORT
#define USIM_MESSAGE_ID_END                         USIM_INIT_ISIM_FILE_REQ

#define PDCP_T_DISCARD_EXPIRY                       (uint16_t)(PDCP_TIMER_EVENT_BASE + 0)
#define PDCP_T_DELAYMODEB_EXPIRY                    (uint16_t)(PDCP_TIMER_EVENT_BASE + 1)
#define PDCP_T_EXPIRY                               (uint16_t)(PDCP_TIMER_EVENT_BASE + 2)


#define RLC_T_POLL_RETRANSMIT_EXPIRY                (uint16_t)(RLC_TIMER_EVENT_BASE + 0)
#define RLC_T_STATUS_PROHIBIT_EXPIRY                (uint16_t)(RLC_TIMER_EVENT_BASE + 1)
#define RLC_T_REORDERING_EXPIRY                     (uint16_t)(RLC_TIMER_EVENT_BASE + 2)
#define L2LOG_T_EXPIRY                              (uint16_t)(RLC_TIMER_EVENT_BASE + 3)


#define MAC_T_EXPIRY                                (uint16_t)(MAC_TIMER_EVENT_BASE + 0)


#define RESC_T300_EXPIRY                            (uint16_t)(RRC_TIMER_EVENT_BASE + 0)
#define RESC_T301_EXPIRY                            (uint16_t)(RRC_TIMER_EVENT_BASE + 1)
#define RESC_T302_EXPIRY                            (uint16_t)(RRC_TIMER_EVENT_BASE + 2)
#define RESC_T303_EXPIRY                            (uint16_t)(RRC_TIMER_EVENT_BASE + 3)
#define RESC_T304_EXPIRY                            (uint16_t)(RRC_TIMER_EVENT_BASE + 4)
#define RESC_T305_EXPIRY                            (uint16_t)(RRC_TIMER_EVENT_BASE + 5)
#define RESC_T310_EXPIRY                            (uint16_t)(RRC_TIMER_EVENT_BASE + 6)
#define RESC_T311_EXPIRY                            (uint16_t)(RRC_TIMER_EVENT_BASE + 7)
#define RESC_REL_DELAY_EXPIRY                       (uint16_t)(RRC_TIMER_EVENT_BASE + 8)
#define RESC_RLF_VALID_EXPIRY                       (uint16_t)(RRC_TIMER_EVENT_BASE + 9)

#define MBC_T320_EXPIRY                             (uint16_t)(RRC_TIMER_EVENT_BASE + 10)
#define MBC_T_MEAS_REPORT_EXPIRY                    (uint16_t)(RRC_TIMER_EVENT_BASE + 11)
#define MBC_T_HYSTNORMAL_EXPIRY                     (uint16_t)(RRC_TIMER_EVENT_BASE + 12)
#define MBC_T_CELLINFO_REPORT_EXPIRY                (uint16_t)(RRC_TIMER_EVENT_BASE + 13)
#define MBC_T_EST_OFFSET_EXPIRY                     (uint16_t)(RRC_TIMER_EVENT_BASE + 14)
#define MBC_T325_EXPIRY                             (uint16_t)(RRC_TIMER_EVENT_BASE + 15)
#define MBC_T321_EXPIRY                             (uint16_t)(RRC_TIMER_EVENT_BASE + 16)

#define CSC_T_SI_MODI_EXPIRY                        (uint16_t)(RRC_TIMER_EVENT_BASE + 17)
#define CSC_T_FREQ_SCAN_EXPIRY                      (uint16_t)(RRC_TIMER_EVENT_BASE + 18)
#define CSC_T_CELL_SEARCH_EXPIRY                    (uint16_t)(RRC_TIMER_EVENT_BASE + 19)
#define CSC_T_3HOUR_EXPIRY                          (uint16_t)(RRC_TIMER_EVENT_BASE + 20)
#define CSC_T_SWITCH_RADIO_EXPIRY                   (uint16_t)(RRC_TIMER_EVENT_BASE + 21)
#define CSC_T_SIB1GUARD_EXPIRY                      (uint16_t)(RRC_TIMER_EVENT_BASE + 22)
#define CSC_T_SIMSGGUARD_EXPIRY                     (uint16_t)(RRC_TIMER_EVENT_BASE + 23)
#define RRC_T_BAR_EXPIRY                            (uint16_t)(RRC_TIMER_EVENT_BASE + 24)


#define NAS_RRC_CAMPON_SUIT_CELL_REQ                (uint16_t)(NAS_RRC_EVENT_BASE+0)
#define NAS_RRC_CAMPON_ANY_CELL_REQ                 (uint16_t)(NAS_RRC_EVENT_BASE+1)
#define NAS_RRC_EPLMN_FTAI_CHG_IND                  (uint16_t)(NAS_RRC_EVENT_BASE+2)
#define NAS_RRC_LIST_PLMN_REQ                       (uint16_t)(NAS_RRC_EVENT_BASE+3)
#define NAS_RRC_SWITCH_RF_REQ                       (uint16_t)(NAS_RRC_EVENT_BASE+4)
#define NAS_RRC_CAMPON_HPPLMN_REQ                   (uint16_t)(NAS_RRC_EVENT_BASE+5)
#define NAS_RRC_ABORT_LIST_PLMN_REQ                 (uint16_t)(NAS_RRC_EVENT_BASE+6)
#define NAS_RRC_ABORT_CAMPON_HPPLMN_REQ             (uint16_t)(NAS_RRC_EVENT_BASE+7)
#define NAS_RRC_CFG_FULL_SCAN_REQ                   (uint16_t)(NAS_RRC_EVENT_BASE+8)
#define NAS_RRC_UL_TRANS_REQ                        (uint16_t)(NAS_RRC_EVENT_BASE+9)
#define NAS_RRC_STOP_UL_TRANS_REQ                   (uint16_t)(NAS_RRC_EVENT_BASE+10)
#define NAS_RRC_EST_REQ                             (uint16_t)(NAS_RRC_EVENT_BASE+11)
#define NAS_RRC_REL_CONN_REQ                        (uint16_t)(NAS_RRC_EVENT_BASE+12)
#define NAS_RRC_KENB_RSP                            (uint16_t)(NAS_RRC_EVENT_BASE+13)
#define NAS_RRC_EDRX_PARA_REQ                       (uint16_t)(NAS_RRC_EVENT_BASE+14)
#define NAS_RRC_STOP_EST                            (uint16_t)(NAS_RRC_EVENT_BASE+15)
#define NAS_RRC_DETACH_REQ                          (uint16_t)(NAS_RRC_EVENT_BASE+16)
#define NAS_RRC_NOCELL_REQ                          (uint16_t)(NAS_RRC_EVENT_BASE+17)

#define NAS_RRC_CELL_INFO_IND                       (uint16_t)(NAS_RRC_RSP_EVENT+0)
#define NAS_RRC_NO_CELL_IND                         (uint16_t)(NAS_RRC_RSP_EVENT+1)
#define NAS_RRC_LIST_PLMN_CNF                       (uint16_t)(NAS_RRC_RSP_EVENT+2)
#define NAS_RRC_SWITCH_RF_CNF                       (uint16_t)(NAS_RRC_RSP_EVENT+3)
#define NAS_RRC_CAMPON_HPPLMN_CNF                   (uint16_t)(NAS_RRC_RSP_EVENT+4)
#define NAS_RRC_CAMPON_HPPLMN_REJ                   (uint16_t)(NAS_RRC_RSP_EVENT+5)
#define NAS_RRC_UL_TRANS_CNF                        (uint16_t)(NAS_RRC_RSP_EVENT+6)
#define NAS_RRC_DL_TRANS_IND                        (uint16_t)(NAS_RRC_RSP_EVENT+7)
#define NAS_RRC_EST_CNF                             (uint16_t)(NAS_RRC_RSP_EVENT+8)
#define NAS_RRC_EST_REJ                             (uint16_t)(NAS_RRC_RSP_EVENT+9)
#define NAS_RRC_ALV_BAR_IND                         (uint16_t)(NAS_RRC_RSP_EVENT+10)
#define NAS_RRC_REL_CONN_IND                        (uint16_t)(NAS_RRC_RSP_EVENT+11)
#define NAS_RRC_UL_TRANS_FAIL_IND                   (uint16_t)(NAS_RRC_RSP_EVENT+12)
#define NAS_RRC_KENB_REQ                            (uint16_t)(NAS_RRC_RSP_EVENT+13)
#define NAS_RRC_PAGE_IND                            (uint16_t)(NAS_RRC_RSP_EVENT+14)
#define NAS_RRC_DRB_SETUP_IND                       (uint16_t)(NAS_RRC_RSP_EVENT+15)

#define RRC_PDCP_SMC_CHECK_REQ                      (uint16_t)(RRC_PDCP_EVENT_BASE + 0)
#define RRC_PDCP_CONFIG_REQ                         (uint16_t)(RRC_PDCP_EVENT_BASE + 1)
#define RRC_PDCP_UL_DCCH_REQ                        (uint16_t)(RRC_PDCP_EVENT_BASE + 2)
#define RRC_PDCP_REEST_REQ                          (uint16_t)(RRC_PDCP_EVENT_BASE + 3)
#define RRC_PDCP_RELEASE_REQ                        (uint16_t)(RRC_PDCP_EVENT_BASE + 4)
#define RRC_PDCP_RESUME_REQ                         (uint16_t)(RRC_PDCP_EVENT_BASE + 5)
#define RRC_PDCP_SUSPEND_REQ                        (uint16_t)(RRC_PDCP_EVENT_BASE + 6)
#define RRC_PDCP_DECIPHER_AND_INT_CHECK_REQ         (uint16_t)(RRC_PDCP_EVENT_BASE + 7)
#define RRC_PDCP_HO_SUCC_IND                        (uint16_t)(RRC_PDCP_EVENT_BASE + 8)
#define RRC_PDCP_HO_FAIL_IND                        (uint16_t)(RRC_PDCP_EVENT_BASE + 9)
#define RRC_PDCP_ENABLE_UL_CIPHER_REQ               (uint16_t)(RRC_PDCP_EVENT_BASE + 10)

#define RRC_RLC_CONFIG_REQ                      (uint16_t)(RRC_RLC_EVENT_BASE + 0)
#define RRC_RLC_REESTABLISH_REQ                 (uint16_t)(RRC_RLC_EVENT_BASE + 1)
#define RRC_RLC_RELEASE_REQ                     (uint16_t)(RRC_RLC_EVENT_BASE + 2)

#define RRC_MAC_UL_CCCH_REQ                     (uint16_t)(RRC_MAC_EVENT_BASE + 0)
#define RRC_MAC_COMM_CONFIG_REQ                 (uint16_t)(RRC_MAC_EVENT_BASE + 1)
#define RRC_MAC_DEDI_CONFIG_REQ                 (uint16_t)(RRC_MAC_EVENT_BASE + 2)
#define RRC_MAC_REL_REQ                         (uint16_t)(RRC_MAC_EVENT_BASE + 3)
#define RRC_MAC_RESET_MAC_REQ                   (uint16_t)(RRC_MAC_EVENT_BASE + 4)
#define RRC_MAC_RESUME_RB_REQ                   (uint16_t)(RRC_MAC_EVENT_BASE + 5)
#define RRC_MAC_SUSPEND_RB_REQ                  (uint16_t)(RRC_MAC_EVENT_BASE + 6)
#define RRC_MAC_RA_REQ                          (uint16_t)(RRC_MAC_EVENT_BASE + 7)
#define RRC_MAC_HO_REQ                          (uint16_t)(RRC_MAC_EVENT_BASE + 8)
#define RRC_MAC_SR_CONFIG_REQ                   (uint16_t)(RRC_MAC_EVENT_BASE + 9)

/*Put Msg from L2 to RRC in sequence*/
#define RRC_PDCP_SMC_CHECK_CNF                  (uint16_t)(RRC_L2_RSP_EVENT + 0)
#define RRC_PDCP_DL_DCCH_DATA                   (uint16_t)(RRC_L2_RSP_EVENT + 1)
#define RRC_PDCP_INTEGIRTY_FAIL_IND             (uint16_t)(RRC_L2_RSP_EVENT + 2)
#define RRC_PDCP_UL_DCCH_CNF                    (uint16_t)(RRC_L2_RSP_EVENT + 3)
#define RRC_PDCP_REEST_CNF                      (uint16_t)(RRC_L2_RSP_EVENT + 4)
#define RRC_PDCP_CONFIG_CNF                     (uint16_t)(RRC_L2_RSP_EVENT + 5)
#define RRC_RLC_CONFIG_CNF                      (uint16_t)(RRC_L2_RSP_EVENT + 6)
#define RRC_RLC_REESTABLISH_CNF                 (uint16_t)(RRC_L2_RSP_EVENT + 7)
#define RRC_L2_RADIOLINK_FAIL_IND               (uint16_t)(RRC_L2_RSP_EVENT + 8)
#define RRC_MAC_DL_CCCH_IND                     (uint16_t)(RRC_L2_RSP_EVENT + 9)
#define RRC_MAC_RA_PROBLEM_IND                  (uint16_t)(RRC_L2_RSP_EVENT + 10)
#define RRC_MAC_RA_SUCC_IND                     (uint16_t)(RRC_L2_RSP_EVENT + 11)
#define RRC_MAC_HO_CNF                          (uint16_t)(RRC_L2_RSP_EVENT + 12)
#define RRC_MAC_PUCCH_SRS_REL_REQ               (uint16_t)(RRC_L2_RSP_EVENT + 13)


#define PDCP_EDCP_COMPLETE_IND                  (uint16_t)(L2_INTERNAL_EVENT_BASE + 0)
#define LPM_SUBFRAM_IND                         (uint16_t)(L2_INTERNAL_EVENT_BASE + 1)
#define PDCP_AP_NO_UL_DATA                      (uint16_t)(L2_INTERNAL_EVENT_BASE + 2)

/* ========================================================================
   ROHCv1  Error Codes
========================================================================  */
#define ERR_ROHCv1_CFG_PBINSTID                   (uint32_t)(ERR_ROHCv1_BASE + 1)
#define ERR_ROHCv1_CFG_PRFNUM                     (uint32_t)(ERR_ROHCv1_BASE + 2)
#define ERR_ROHCv1_CFG_PRFVAL                     (uint32_t)(ERR_ROHCv1_BASE + 3)
#define ERR_ROHCv1_CFG_MAXCID                     (uint32_t)(ERR_ROHCv1_BASE + 4)
#define ERR_ROHCv1_NOIDLEINST                     (uint32_t)(ERR_ROHCv1_BASE + 5)
#define ERR_ROHCv1_INSTID                         (uint32_t)(ERR_ROHCv1_BASE + 6)
#define ERR_ROHCv1_INSTNOTEXIST                   (uint32_t)(ERR_ROHCv1_BASE + 7)
#define ERR_ROHCv1_PROFILE_NOTEXIST               (uint32_t)(ERR_ROHCv1_BASE + 8)
#define ERR_ROHCv1_CID_VAL                        (uint32_t)(ERR_ROHCv1_BASE + 9)
#define ERR_ROHCv1_TIMEOUT_MSGID                  (uint32_t)(ERR_ROHCv1_BASE + 10)
#define ERR_ROHCv1_COMPCTX_NOTEXIST               (uint32_t)(ERR_ROHCv1_BASE + 11)
#define ERR_ROHCv1_DECOMPCTX_NOTEXIST             (uint32_t)(ERR_ROHCv1_BASE + 11)
#define ERR_ROHCv1_TIMEOUT_NOTUMODE               (uint32_t)(ERR_ROHCv1_BASE + 12)
#define ERR_ROHCv1_TIMEOUT_STATE                  (uint32_t)(ERR_ROHCv1_BASE + 13)
#define ERR_ROHCv1_IPVER_INVALID                  (uint32_t)(ERR_ROHCv1_BASE + 14)
#define ERR_ROHCv1_COMP_DECODEPKT_LEN             (uint32_t)(ERR_ROHCv1_BASE + 15)
#define ERR_ROHCv1_PKTLEN                         (uint32_t)(ERR_ROHCv1_BASE + 16)
#define ERR_ROHCv1_CTX_FORBID                     (uint32_t)(ERR_ROHCv1_BASE + 17)
#define ERR_ROHCv1_NO_ENOUGH_CONTEXT_RESOURCE     (uint32_t)(ERR_ROHCv1_BASE + 18)
#define ERR_ROHCv1_FIELD_TYPE                     (uint32_t)(ERR_ROHCv1_BASE + 19)
#define ERR_ROHCv1_WLSB_NOITEM                    (uint32_t)(ERR_ROHCv1_BASE + 20)
#define ERR_ROHCv1_NO_CTX_REUSE                   (uint32_t)(ERR_ROHCv1_BASE + 21)
#define ERR_ROHCv1_COMPINFONULL                   (uint32_t)(ERR_ROHCv1_BASE + 22)
#define ERR_ROHCv1_COMPHDNULL                     (uint32_t)(ERR_ROHCv1_BASE + 23)
#define ERR_ROHCv1_ORIGHDNULL                     (uint32_t)(ERR_ROHCv1_BASE + 24)
#define ERR_ROHCv1_FDBK_CID_LEN                   (uint32_t)(ERR_ROHCv1_BASE + 25)
#define ERR_ROHCv1_FDBK2_TYPE                     (uint32_t)(ERR_ROHCv1_BASE + 26)
#define ERR_ROHCv1_MODE_TYPE                      (uint32_t)(ERR_ROHCv1_BASE + 27)
#define ERR_ROHCv1_FDBK2_CRCFAIL                  (uint32_t)(ERR_ROHCv1_BASE + 28)
#define ERR_ROHCv1_FDBKOPTS_LEN                   (uint32_t)(ERR_ROHCv1_BASE + 29)
#define ERR_ROHCv1_FDBK2OPTLEN_NOT_MATCH          (uint32_t)(ERR_ROHCv1_BASE + 30)
#define ERR_ROHCv1_FDBK2OPT_REPEAT                (uint32_t)(ERR_ROHCv1_BASE + 31)
#define ERR_ROHCv1_FDBK_CRC_WRONG                 (uint32_t)(ERR_ROHCv1_BASE + 32)
#define ERR_ROHCv1_OPT_CRC_NOTEXIST               (uint32_t)(ERR_ROHCv1_BASE + 33)
#define ERR_ROHCv1_FDBK_TYPE                      (uint32_t)(ERR_ROHCv1_BASE + 34)
#define ERR_ROHCv1_FDBK_SN_VAL                    (uint32_t)(ERR_ROHCv1_BASE + 35)
#define ERR_ROHCv1_ACK_SNNOTVALID                 (uint32_t)(ERR_ROHCv1_BASE + 36)
#define ERR_ROHCv1_FDBK_NOSNSW                    (uint32_t)(ERR_ROHCv1_BASE + 37)
#define ERR_ROHCv1_FDBK_SN_BITS                   (uint32_t)(ERR_ROHCv1_BASE + 38)
#define ERR_ROHCv1_FDBK_OTHERSNNOTVALID           (uint32_t)(ERR_ROHCv1_BASE + 39)
#define ERR_ROHCv1_FDBK_NOTMATCH_CTXSTATE         (uint32_t)(ERR_ROHCv1_BASE + 40)
#define ERR_ROHCv1_FDBK_CTXMEM_NOTMATCH_PRF       (uint32_t)(ERR_ROHCv1_BASE + 41)
#define ERR_ROHCv1_FDBK_NOTMATCH_CTXMODE          (uint32_t)(ERR_ROHCv1_BASE + 42)
#define ERR_ROHCv1_COMPPKT_SEGMENT                (uint32_t)(ERR_ROHCv1_BASE + 43)
#define ERR_ROHCv1_DECODE_LARGECID                (uint32_t)(ERR_ROHCv1_BASE + 44)
#define ERR_ROHCv1_PT_INVALID                     (uint32_t)(ERR_ROHCv1_BASE + 45)
#define ERR_ROHCv1_NOT_SUPPORT                    (uint32_t)(ERR_ROHCv1_BASE + 46)
#define ERR_ROHCv1_EXT3_EXISTIPID                 (uint32_t)(ERR_ROHCv1_BASE + 47)
#define ERR_ROHCv1_EXT3_RTP_MARKER                (uint32_t)(ERR_ROHCv1_BASE + 48)
#define ERR_ROHCv1_RTP_CC_NOT_EQUAL               (uint32_t)(ERR_ROHCv1_BASE + 49)
#define ERR_ROHCv1_RTP_ET                         (uint32_t)(ERR_ROHCv1_BASE + 50)
#define ERR_ROHCv1_COMPPKT_DECOMPSTATE            (uint32_t)(ERR_ROHCv1_BASE + 51)
#define ERR_ROHCv1_CRC_CHK_FAIL                   (uint32_t)(ERR_ROHCv1_BASE + 52)
#define ERR_ROHCv1_REF_LIST_NOTEXIST              (uint32_t)(ERR_ROHCv1_BASE + 53)
#define ERR_ROHCv1_REF_TTNODE_NOTEXIST            (uint32_t)(ERR_ROHCv1_BASE + 54)
#define ERR_ROHCv1_DECOMPINFONULL                 (uint32_t)(ERR_ROHCv1_BASE + 55)
#define ERR_ROHCv1_DECOMPHDNULL                   (uint32_t)(ERR_ROHCv1_BASE + 56)
#define ERR_ROHCv1_IPv4_SEGMENT_FG_WRONG          (uint32_t)(ERR_ROHCv1_BASE + 57)
#define ERR_ROHCv1_NULL                           (uint32_t)(ERR_ROHCv1_BASE + 58)
#define ERR_ROHCv1_TIMEOUT_MSG                    (uint32_t)(ERR_ROHCv1_BASE + 59)

/* ========================================================================
   ROHCv2  Error Codes
========================================================================  */
#define ERR_ROHCv2_CFG_PRFNUM                       (uint32_t)(ERR_ROHCv2_BASE + 1)
#define ERR_ROHCv2_CFG_PBINSTID                     (uint32_t)(ERR_ROHCv2_BASE + 2)
#define ERR_ROHCv2_CFG_MAXCID                       (uint32_t)(ERR_ROHCv2_BASE + 3)
#define ERR_ROHCv2_CFG_PRFVAL                       (uint32_t)(ERR_ROHCv2_BASE + 4)
#define ERR_ROHCv2_INSTNULL                         (uint32_t)(ERR_ROHCv2_BASE + 5)
#define ERR_ROHCv2_INSTID                           (uint32_t)(ERR_ROHCv2_BASE + 6)
#define ERR_ROHCv2_RCFG_INSTNOTEXIST                (uint32_t)(ERR_ROHCv2_BASE + 7)
#define ERR_ROHCv2_DESTROY_INSTNOTEXIST             (uint32_t)(ERR_ROHCv2_BASE + 8)
#define ERR_ROHCv2_REORDERRATIO                     (uint32_t)(ERR_ROHCv2_BASE + 9)
#define ERR_ROHCv2_ORIGHDNULL                       (uint32_t)(ERR_ROHCv2_BASE + 10)
#define ERR_ROHCv2_COMPHDNULL                       (uint32_t)(ERR_ROHCv2_BASE + 11)
#define ERR_ROHCv2_COMPINFONULL                     (uint32_t)(ERR_ROHCv2_BASE + 12)
#define ERR_ROHCv2_IPVER                            (uint32_t)(ERR_ROHCv2_BASE + 13)
#define ERR_ROHCv2_DRAWORIGPKT_LEN                  (uint32_t)(ERR_ROHCv2_BASE + 14)
#define ERR_ROHCv2_NO_ENOUGH_CID                    (uint32_t)(ERR_ROHCv2_BASE + 15)
#define ERR_ROHCv2_OVER_MAXCOMPLEN                  (uint32_t)(ERR_ROHCv2_BASE + 16)
#define ERR_ROHCv2_SEARCH_CTX_PARAMATER             (uint32_t)(ERR_ROHCv2_BASE + 17)
#define ERR_ROHCv2_NO_SPECIFIED_CID                 (uint32_t)(ERR_ROHCv2_BASE + 18)
#define ERR_ROHCv2_COMPPKT_SEGMENT                  (uint32_t)(ERR_ROHCv2_BASE + 19)
#define ERR_ROHCv2_DECOMPINFONULL                   (uint32_t)(ERR_ROHCv2_BASE + 20)
#define ERR_ROHCv2_TIMEOUT_MSGID                    (uint32_t)(ERR_ROHCv2_BASE + 21)
#define ERR_ROHCv2_TIMEOUT_INSTID                   (uint32_t)(ERR_ROHCv2_BASE + 22)
#define ERR_ROHCv2_TIMEOUT_INSTNULL                 (uint32_t)(ERR_ROHCv2_BASE + 23)
#define ERR_ROHCv2_TIMEOUT_PROFILE                  (uint32_t)(ERR_ROHCv2_BASE + 24)
#define ERR_ROHCv2_TIMEOUT_CID                      (uint32_t)(ERR_ROHCv2_BASE + 25)
#define ERR_ROHCv2_COMPCTX_NOTEXIST                 (uint32_t)(ERR_ROHCv2_BASE + 26)
#define ERR_ROHCv2_NOIDLEINST                       (uint32_t)(ERR_ROHCv2_BASE + 27)
#define ERR_ROHCv2_FDBK2_CRCFAIL                    (uint32_t)(ERR_ROHCv2_BASE + 28)
#define ERR_ROHCv2_FDBK_NOCOMPCTX                   (uint32_t)(ERR_ROHCv2_BASE + 29)
#define ERR_ROHCv2_FDBK2_TYPE                       (uint32_t)(ERR_ROHCv2_BASE + 30)
#define ERR_ROHCv2_DLFDBK_LEN                       (uint32_t)(ERR_ROHCv2_BASE + 31)
#define ERR_ROHCv2_FDBK2OPTLEN_REJ                  (uint32_t)(ERR_ROHCv2_BASE + 32)
#define ERR_ROHCv2_FDBK2OPTLEN_ACKNBNOTVALID        (uint32_t)(ERR_ROHCv2_BASE + 33)
#define ERR_ROHCv2_FDBK2OPTLEN_CTXMEM               (uint32_t)(ERR_ROHCv2_BASE + 34)
#define ERR_ROHCv2_FDBK2OPTLEN_CLOCK                (uint32_t)(ERR_ROHCv2_BASE + 35)
#define ERR_ROHCv2_FDBK2OPT_TYPEUNKOWN              (uint32_t)(ERR_ROHCv2_BASE + 36)
#define ERR_ROHCv2_FDBKOPT_ACKNBNOTVALID_SN         (uint32_t)(ERR_ROHCv2_BASE + 37)
#define ERR_ROHCv2_FDBKOPT_ACKNBNOTVALID_FDBKTYPE   (uint32_t)(ERR_ROHCv2_BASE + 38)
#define ERR_ROHCv2_FDBK2OPT_REPEAT                  (uint32_t)(ERR_ROHCv2_BASE + 39)
#define ERR_ROHCv2_FDBKOPT_SUMLEN                   (uint32_t)(ERR_ROHCv2_BASE + 40)
#define ERR_ROHCv2_FDBKTYPE_PRF0                    (uint32_t)(ERR_ROHCv2_BASE + 41)
#define ERR_ROHCv2_FDBK_NOSNSW                      (uint32_t)(ERR_ROHCv2_BASE + 42)
#define ERR_ROHCv2_FDBK_OTHERSNNOTVALID             (uint32_t)(ERR_ROHCv2_BASE + 43)
#define ERR_ROHCv2_FDBK_ESPSNNOTVALID               (uint32_t)(ERR_ROHCv2_BASE + 44)
#define ERR_ROHCv2_REMOVEPADDING                    (uint32_t)(ERR_ROHCv2_BASE + 45)
#define ERR_ROHCv2_COMPPKT_DECOMPSTATE              (uint32_t)(ERR_ROHCv2_BASE + 46)
#define ERR_ROHCv2_COMPPKT_UNKNOW                   (uint32_t)(ERR_ROHCv2_BASE + 47)
#define ERR_ROHCv2_DECOMP_ORIGLEN                   (uint32_t)(ERR_ROHCv2_BASE + 48)
#define ERR_ROHCv2_DECOMP_CRCCHK_FAIL               (uint32_t)(ERR_ROHCv2_BASE + 49)
#define ERR_ROHCv2_FDBK_CIDLEN                      (uint32_t)(ERR_ROHCv2_BASE + 50)
#define ERR_ROHCv2_COMMONPKT_FLAG_RESEVED           (uint32_t)(ERR_ROHCv2_BASE + 51)
#define ERR_ROHCv2_COMMONPKT_FLAG_OUTIP             (uint32_t)(ERR_ROHCv2_BASE + 52)
#define ERR_ROHCv2_CSRCLIST_RESEVED                 (uint32_t)(ERR_ROHCv2_BASE + 53)
#define ERR_ROHCv2_ORIGHD_LEN                       (uint32_t)(ERR_ROHCv2_BASE + 54)
#define ERR_ROHCv2_COMMONPKT_TS_FG                  (uint32_t)(ERR_ROHCv2_BASE + 55)
#define ERR_ROHCv2_DECODE_LARGECID                  (uint32_t)(ERR_ROHCv2_BASE + 56)
#define ERR_ROHCv2_COREPAIR_R1                      (uint32_t)(ERR_ROHCv2_BASE + 57)
#define ERR_ROHCv2_COREPAIR_R2                      (uint32_t)(ERR_ROHCv2_BASE + 58)
#define ERR_ROHCv2_IPV4DYN_RESERVED                 (uint32_t)(ERR_ROHCv2_BASE + 59)
#define ERR_ROHCv2_IPV6DYN_RESERVED                 (uint32_t)(ERR_ROHCv2_BASE + 60)
#define ERR_ROHCv2_UDPLITEDYN_RESERVED              (uint32_t)(ERR_ROHCv2_BASE + 61)
#define ERR_ROHCv2_RTPDYN_RESERVED                  (uint32_t)(ERR_ROHCv2_BASE + 62)
#define ERR_ROHCv2_ESPDYN_RESERVED                  (uint32_t)(ERR_ROHCv2_BASE + 63)
#define ERR_ROHCv2_UDPDYN_RESERVED                  (uint32_t)(ERR_ROHCv2_BASE + 64)
#define ERR_ROHCv2_IP_NOTINNER                      (uint32_t)(ERR_ROHCv2_BASE + 65)
#define ERR_ROHCv2_IPSTAT_RESERVED                  (uint32_t)(ERR_ROHCv2_BASE + 66)
#define ERR_ROHCv2_IR_PRF0_RES                      (uint32_t)(ERR_ROHCv2_BASE + 67)
#define ERR_ROHCv2_COMPPKTLEN                       (uint32_t)(ERR_ROHCv2_BASE + 68)
#define ERR_ROHCv2_NODECOMPCTX                      (uint32_t)(ERR_ROHCv2_BASE + 69)
#define ERR_ROHCv2_ANALYSEFDBKLEN                   (uint32_t)(ERR_ROHCv2_BASE + 70)
#define ERR_ROHCv2_TSSTRIDE                         (uint32_t)(ERR_ROHCv2_BASE + 71)
#define ERR_ROHCv2_CSRC_NOTFOUND                    (uint32_t)(ERR_ROHCv2_BASE + 72)
#define ERR_ROHCv2_CRCFAIL_IR                       (uint32_t)(ERR_ROHCv2_BASE + 73)
#define ERR_ROHCv2_CRCFAIL_CTRL                     (uint32_t)(ERR_ROHCv2_BASE + 74)
#define ERR_ROHCv2_COMPPKT_PADDING                  (uint32_t)(ERR_ROHCv2_BASE + 75)
#define ERR_ROHCv2_IPVER_INVALID                    (uint32_t)(ERR_ROHCv2_BASE + 76)
#define ERR_ROHCv2_NO_ENOUGH_CONTEXT_RESOURCE       (uint32_t)(ERR_ROHCv2_BASE + 77)
#define ERR_ROHCv2_PROFILE_INVALID                  (uint32_t)(ERR_ROHCv2_BASE + 78)
#define ERR_ROHCv2_CID_INVALID                      (uint32_t)(ERR_ROHCv2_BASE + 79)
#define ERR_ROHCv2_DECOMPRESS_CID_LINK_EMPTY        (uint32_t)(ERR_ROHCv2_BASE + 80)
#define ERR_ROHCv2_CID_NOT_EXIST                    (uint32_t)(ERR_ROHCv2_BASE + 81)
#define ERR_ROHCv2_PKT_TYPE                         (uint32_t)(ERR_ROHCv2_BASE + 82)
#define ERR_ROHCv2_NULL                             (uint32_t)(ERR_ROHCv2_BASE + 83)
#define ERR_ROHCv2_INSTID_INVALID                   (uint32_t)(ERR_ROHCv2_BASE + 84)
#define ERR_ROHCv2_COMP_DECODEPKT_LEN               (uint32_t)(ERR_ROHCv2_BASE + 85)
#define ERR_ROHCv2_WLSB_NOITEM                      (uint32_t)(ERR_ROHCv2_BASE + 86)
#define ERR_ROHCv2_FIELD_TYPE                       (uint32_t)(ERR_ROHCv2_BASE + 87)
#define ERR_ROHCv2_DEL_FDBK_NODE_EMPTY              (uint32_t)(ERR_ROHCv2_BASE + 88)


#define RRC_CSC_STATE_CHG_IND                   (uint16_t)(RRC_INTERNAL_EVENT_BASE + 0)
#define RRC_RESC_SI_CHG_IND                     (uint16_t)(RRC_INTERNAL_EVENT_BASE + 1)
#define RRC_RESC_CELL_CAMP_IND                  (uint16_t)(RRC_INTERNAL_EVENT_BASE + 2)
#define RRC_RESC_CELL_STATE_IND                 (uint16_t)(RRC_INTERNAL_EVENT_BASE + 3)
#define RRC_CSC_READ_CGI_REQ                    (uint16_t)(RRC_INTERNAL_EVENT_BASE + 4)
#define RRC_MBC_CGI_CNF                         (uint16_t)(RRC_INTERNAL_EVENT_BASE + 5)
#define RRC_MBC_SI_CHGED_IND                    (uint16_t)(RRC_INTERNAL_EVENT_BASE + 6)
#define RRC_MBC_SI_READY_IND                    (uint16_t)(RRC_INTERNAL_EVENT_BASE + 7)
#define RRC_MBC_CELL_RESEL_REJ_IND              (uint16_t)(RRC_INTERNAL_EVENT_BASE + 8)
#define RRC_MBC_ENABLE_MEASCFG                  (uint16_t)(RRC_INTERNAL_EVENT_BASE + 9)
#define RRC_MBC_BAR_INFO_CHANGE_IND             (uint16_t)(RRC_INTERNAL_EVENT_BASE + 10)
#define RRC_RESC_HND_BUF_DLDCCH_REQ             (uint16_t)(RRC_INTERNAL_EVENT_BASE + 11)

//@MSG-PHY(L1_LOG_SUBMDL_TOP){{
#define PS_PHY_FREQ_SCAN_REQ                    (uint16_t)(PS_PHY_EVENT_BASE + 0)
#define PS_PHY_CELL_SEARCH_REQ                  (uint16_t)(PS_PHY_EVENT_BASE + 1)
#define PS_PHY_READ_SIB1_REQ                    (uint16_t)(PS_PHY_EVENT_BASE + 2)
#define PS_PHY_READ_SI_REQ                      (uint16_t)(PS_PHY_EVENT_BASE + 3)
#define PS_PHY_STOP_SI_READ_REQ                 (uint16_t)(PS_PHY_EVENT_BASE + 4)
#define PS_PHY_STOP_SCAN_SEARCH_REQ             (uint16_t)(PS_PHY_EVENT_BASE + 5)
#define PS_PHY_MEAS_CONFIG_REQ                  (uint16_t)(PS_PHY_EVENT_BASE + 6)
#define PS_PHY_MEAS_GAP_CONFIG_REQ              (uint16_t)(PS_PHY_EVENT_BASE + 7)
#define PS_PHY_MEAS_MASK_SET_REQ                (uint16_t)(PS_PHY_EVENT_BASE + 8)
#define PS_PHY_ABORT_MEAS_REQ                   (uint16_t)(PS_PHY_EVENT_BASE + 9)
#define PS_PHY_EARFCN_BAND_INFO                 (uint16_t)(PS_PHY_EVENT_BASE + 10)
#define PS_PHY_COMMON_CONFIG_REQ                (uint16_t)(PS_PHY_EVENT_BASE + 11)
#define PS_PHY_DEDICATED_CONFIG_REQ             (uint16_t)(PS_PHY_EVENT_BASE + 12)
#define PS_PHY_HANDOVER_REQ                     (uint16_t)(PS_PHY_EVENT_BASE + 13)
#define PS_PHY_MAC_RESET_REQ                    (uint16_t)(PS_PHY_EVENT_BASE + 14)
#define PS_PHY_RELEASE_REQ                      (uint16_t)(PS_PHY_EVENT_BASE + 15)
#define PS_PHY_ACCESS_REQ                       (uint16_t)(PS_PHY_EVENT_BASE + 16)
#define PS_PHY_ABORT_ACCESS_REQ                 (uint16_t)(PS_PHY_EVENT_BASE + 17)
#define PS_PHY_TA_CMD_REQ                       (uint16_t)(PS_PHY_EVENT_BASE + 18)
#define PS_PHY_DRX_CMD_REQ                      (uint16_t)(PS_PHY_EVENT_BASE + 19)
#define PS_PHY_TA_TIMER_STOP_IND                (uint16_t)(PS_PHY_EVENT_BASE + 20)
#define PS_PHY_RESET_REQ                        (uint16_t)(PS_PHY_EVENT_BASE + 21)
#define PS_PHY_EPCG_REQ                         (uint16_t)(PS_PHY_EVENT_BASE + 22)
#define PS_PHY_MEAS_PERIOD_CHG_REQ              (uint16_t)(PS_PHY_EVENT_BASE + 23)
#define PS_PHY_RESEL_PARA_CFG                   (uint16_t)(PS_PHY_EVENT_BASE + 24)
#define PS_PHY_IDLE_SERV_CELL_CFG               (uint16_t)(PS_PHY_EVENT_BASE + 25)
#define PS_PHY_WAKEUP_REQ                       (uint16_t)(PS_PHY_EVENT_BASE + 26)
#define PS_PHY_WIFISCAN_START_REQ               (uint16_t)(PS_PHY_EVENT_BASE + 27)
#define PS_PHY_WIFISCANNAP_START_REQ            (uint16_t)(PS_PHY_EVENT_BASE + 28)
#define PS_PHY_WIFISCAN_STOP_REQ                (uint16_t)(PS_PHY_EVENT_BASE + 29)

#define PS_PHY_WIFISENTEST_START_REQ            (uint16_t)(PS_PHY_EVENT_BASE + 30)
#define PS_PHY_WIFISENTEST_STOP_REQ             (uint16_t)(PS_PHY_EVENT_BASE + 31)
#define PS_PHY_WIFISENTEST_ON_REQ               (uint16_t)(PS_PHY_EVENT_BASE + 32)
#define PS_PHY_WIFISENTEST_OFF_REQ              (uint16_t)(PS_PHY_EVENT_BASE + 33)






#define PS_PHY_SI_DATA_IND                      (uint16_t)(PS_PHY_RSP_EVENT + 0)
#define PS_PHY_PBCH_READ_FAIL_IND               (uint16_t)(PS_PHY_RSP_EVENT + 1)
#define PS_PHY_FREQ_SCAN_CNF                    (uint16_t)(PS_PHY_RSP_EVENT + 2)
#define PS_PHY_CELL_SEARCH_CNF                  (uint16_t)(PS_PHY_RSP_EVENT + 3)
#define PS_PHY_PCH_DATA_IND                     (uint16_t)(PS_PHY_RSP_EVENT + 4)
#define PS_PHY_RESET_CNF                        (uint16_t)(PS_PHY_RSP_EVENT + 5)
#define PS_PHY_RESEL_IND                        (uint16_t)(PS_PHY_RSP_EVENT + 6)
#define PS_PHY_INTRA_MEAS_IND                   (uint16_t)(PS_PHY_RSP_EVENT + 7)
#define PS_PHY_INTER_MEAS_IND                   (uint16_t)(PS_PHY_RSP_EVENT + 8)
#define PS_PHY_DRX_STATE_IND                    (uint16_t)(PS_PHY_RSP_EVENT + 9)
#define PS_PHY_HANDOVER_CNF                     (uint16_t)(PS_PHY_RSP_EVENT + 10)
#define PS_PHY_OUT_OF_SYNC_IND                  (uint16_t)(PS_PHY_RSP_EVENT + 11)
#define PS_PHY_RECOVERY_SYNC_IND                (uint16_t)(PS_PHY_RSP_EVENT + 12)
#define PS_PHY_PUCCH_SRS_REL_IND                (uint16_t)(PS_PHY_RSP_EVENT + 13)
#define PS_PHY_RELEASE_CNF                      (uint16_t)(PS_PHY_RSP_EVENT + 14)
#define PS_PHY_ACCESS_CNF                       (uint16_t)(PS_PHY_RSP_EVENT + 15)
#define PS_PHY_MAC_INIT_RA_REQ                  (uint16_t)(PS_PHY_RSP_EVENT + 16)
#define PS_PHY_RA_RESPONSE_IND                  (uint16_t)(PS_PHY_RSP_EVENT + 17)
#define PS_PHY_DLSCH_DATA_IND                   (uint16_t)(PS_PHY_RSP_EVENT + 18)
#define PS_PHY_ULSCH_INFO_IND                   (uint16_t)(PS_PHY_RSP_EVENT + 19)
#define PS_PHY_EPCG_CNF                         (uint16_t)(PS_PHY_RSP_EVENT + 20)
#define PS_PHY_PHYWAKEUPPS_REQ                  (uint16_t)(PS_PHY_RSP_EVENT + 21)
#define PS_PHY_ICP_REQ                          (uint16_t)(PS_PHY_RSP_EVENT + 22)
#define PS_PHY_AMT_MSG_IND                      (uint16_t)(PS_PHY_RSP_EVENT + 23)
#define PS_PHY_WIFISCAN_START_CNF               (uint16_t)(PS_PHY_RSP_EVENT + 24)
#define PS_PHY_WIFISCAN_STOP_CNF                (uint16_t)(PS_PHY_RSP_EVENT + 25)
#define PS_PHY_WIFISCAN_IND                     (uint16_t)(PS_PHY_RSP_EVENT + 26)
#define PS_PHY_WIFISENTEST_START_CNF            (uint16_t)(PS_PHY_RSP_EVENT + 27)
#define PS_PHY_WIFISENTEST_STOP_CNF             (uint16_t)(PS_PHY_RSP_EVENT + 28)
#define PS_PHY_WIFISENTEST_RESULT_IND           (uint16_t)(PS_PHY_RSP_EVENT + 29)
#define PS_PHY_WIFISENTEST_ON_CNF               (uint16_t)(PS_PHY_RSP_EVENT + 30)
#define PS_PHY_WIFISENTEST_OFF_CNF              (uint16_t)(PS_PHY_RSP_EVENT + 31)

// mts
#define PS_PHY_UL_DATA_SEND_CTRL_INFO_REQ       (uint16_t)(PS_PHY_RSP_EVENT + 24)
#define PS_PHY_UL_SR_SEND_REQ                   (uint16_t)(PS_PHY_RSP_EVENT + 25)
#define PS_PHY_MTS_STOP_REQ                     (uint16_t)(PS_PHY_RSP_EVENT + 26)


//}}

#define NUNIT_ESM_DATA_REQ                      (uint16_t)(NUNIT_ESM_EVENT_BASE + 0)
#define NUNIT_ESM_L3TC_DATA_REQ                 (uint16_t)(NUNIT_ESM_EVENT_BASE + 1)
#define NUNIT_ESM_ACT_DED_EBCON_ACC             (uint16_t)(NUNIT_ESM_RSP_EVENT + 0)
#define NUNIT_ESM_ACT_DED_EBCON_REJ             (uint16_t)(NUNIT_ESM_RSP_EVENT + 1)
#define NUNIT_ESM_ACT_DEF_EBCON_ACC             (uint16_t)(NUNIT_ESM_RSP_EVENT + 2)
#define NUNIT_ESM_ACT_DEF_EBCON_REJ             (uint16_t)(NUNIT_ESM_RSP_EVENT + 3)
#define NUNIT_ESM_BR_ALLOC_REQ                  (uint16_t)(NUNIT_ESM_RSP_EVENT + 4)
#define NUNIT_ESM_BR_MOD_REQ                    (uint16_t)(NUNIT_ESM_RSP_EVENT + 5)
#define NUNIT_ESM_DEACT_EBCON_ACC               (uint16_t)(NUNIT_ESM_RSP_EVENT + 6)
#define NUNIT_ESM_ESMINFO_RSP                   (uint16_t)(NUNIT_ESM_RSP_EVENT + 7)
#define NUNIT_ESM_ESMSTATUS                     (uint16_t)(NUNIT_ESM_RSP_EVENT + 8)
#define NUNIT_ESM_MOD_EBCON_ACC                 (uint16_t)(NUNIT_ESM_RSP_EVENT + 9)
#define NUNIT_ESM_MOD_EBCON_REJ                 (uint16_t)(NUNIT_ESM_RSP_EVENT + 10)
#define NUNIT_ESM_PDN_CON_REQ                   (uint16_t)(NUNIT_ESM_RSP_EVENT + 11)
#define NUNIT_ESM_PDN_DISC_REQ                  (uint16_t)(NUNIT_ESM_RSP_EVENT + 12)
#define NUNIT_ESM_L3TC_CLOSE_UETEST_LOOP_CMP    (uint16_t)(NUNIT_ESM_RSP_EVENT + 13)
#define NUNIT_ESM_L3TC_OPEN_UETEST_LOOP_CMP     (uint16_t)(NUNIT_ESM_RSP_EVENT + 14)
#define NUNIT_ESM_L3TC_ACT_TEST_MODE_CMP        (uint16_t)(NUNIT_ESM_RSP_EVENT + 15)
#define NUNIT_ESM_L3TC_DEACT_TEST_MODE_CMP      (uint16_t)(NUNIT_ESM_RSP_EVENT + 16)
#define NUNIT_ESM_L3TC_OPEN_UETEST_LOOP_CMBMSPACKETCNT_RESP     (uint16_t)(NUNIT_ESM_RSP_EVENT + 17)

#define NUNIT_EMM_DATA_REQ                      (uint16_t)(NUNIT_EMM_EVENT_BASE + 0)
#define NUNIT_EMM_MAPPED_SEC_PARAM              (uint16_t)(NUNIT_EMM_EVENT_BASE + 1)
#define NUNIT_EMM_PS_HO_FROM_EUTRA              (uint16_t)(NUNIT_EMM_EVENT_BASE + 2)
#define NUNIT_EMM_PS_HO_TO_EUTRA                (uint16_t)(NUNIT_EMM_EVENT_BASE + 3)

#define ENB_NUNIT_ESM_CMD                       (uint16_t)(NUNIT_EMM_RSP_EVENT + 0)
#define NUNIT_EMM_L3MSG_ATTACH_REQ              (uint16_t)(NUNIT_EMM_RSP_EVENT + 1)
#define NUNIT_EMM_L3MSG_ATTACH_CMP              (uint16_t)(NUNIT_EMM_RSP_EVENT + 2)
#define NUNIT_EMM_L3MSG_AUTH_FAIL               (uint16_t)(NUNIT_EMM_RSP_EVENT + 3)
#define NUNIT_EMM_L3MSG_AUTH_REJ                (uint16_t)(NUNIT_EMM_RSP_EVENT + 4)
#define NUNIT_EMM_L3MSG_AUTH_RSP                (uint16_t)(NUNIT_EMM_RSP_EVENT + 5)
#define NUNIT_EMM_L3MSG_DETACH_APT              (uint16_t)(NUNIT_EMM_RSP_EVENT + 6)
#define NUNIT_EMM_L3MSG_DETACH_REQ              (uint16_t)(NUNIT_EMM_RSP_EVENT + 7)
#define NUNIT_EMM_L3MSG_ULNAS_TRANS             (uint16_t)(NUNIT_EMM_RSP_EVENT + 8)
#define NUNIT_EMM_L3MSG_SERVICE_REQ             (uint16_t)(NUNIT_EMM_RSP_EVENT + 9)
#define NUNIT_EMM_L3MSG_EXSERVICE_REQ           (uint16_t)(NUNIT_EMM_RSP_EVENT + 10)
#define NUNIT_EMM_L3MSG_GUTI_CMP                (uint16_t)(NUNIT_EMM_RSP_EVENT + 11)
#define NUNIT_EMM_L3MSG_IDNT_RSP                (uint16_t)(NUNIT_EMM_RSP_EVENT + 12)
#define NUNIT_EMM_L3MSG_SMC_COM                 (uint16_t)(NUNIT_EMM_RSP_EVENT + 13)
#define NUNIT_EMM_L3MSG_SMC_REJ                 (uint16_t)(NUNIT_EMM_RSP_EVENT + 14)
#define NUNIT_EMM_L3MSG_TAU_REQ                 (uint16_t)(NUNIT_EMM_RSP_EVENT + 15)
#define NUNIT_EMM_L3MSG_TAU_CMP                 (uint16_t)(NUNIT_EMM_RSP_EVENT + 16)
#define NUNIT_EMM_L3MSG_EMMSTATUS               (uint16_t)(NUNIT_EMM_RSP_EVENT + 17)
#define NUNIT_EMM_DATA_IND                      (uint16_t)(NUNIT_EMM_RSP_EVENT + 18)

#define ENB_EMM_ESM_DATA_REQ                    (uint16_t)(ENB_EMM_ESM_EVENT_BASE + 0)
#define ENB_EMM_ESM_DATA_IND                    (uint16_t)(ENB_EMM_ESM_RSP_EVENT + 0)

#define ENB_RRC_EMM_DATA_REQ                    (uint16_t)(ENB_RRC_EMM_EVENT_BASE + 0)
#define ENB_RRC_EMM_DLSQN                       (uint16_t)(ENB_RRC_EMM_EVENT_BASE + 1)

#define ENB_RRC_EMM_DATA_IND                    (uint16_t)(ENB_RRC_EMM_RSP_EVENT + 0)

#define ENBRRC_RRC_CONN_REQ                     (uint16_t)(ENB_RRC_EVENT_BASE + 0)
#define ENBRRC_RRC_CONN_SETUP                   (uint16_t)(ENB_RRC_EVENT_BASE + 1)
#define ENBRRC_RRC_CONN_REJ                     (uint16_t)(ENB_RRC_EVENT_BASE + 2)
#define ENBRRC_RRC_CONN_CMP                     (uint16_t)(ENB_RRC_EVENT_BASE + 3)
#define ENBRRC_SEC_MODE_CMD                     (uint16_t)(ENB_RRC_EVENT_BASE + 4)
#define ENBRRC_SEC_MODE_CMP                     (uint16_t)(ENB_RRC_EVENT_BASE + 5)
#define ENBRRC_SEC_MODE_FAIL                    (uint16_t)(ENB_RRC_EVENT_BASE + 6)
#define ENBRRC_RRC_CONN_RECONFIG                (uint16_t)(ENB_RRC_EVENT_BASE + 7)
#define ENBRRC_RRC_CONN_RECONFIG_CMP            (uint16_t)(ENB_RRC_EVENT_BASE + 8)
#define ENBRRC_RRC_CONN_REEST_REQ               (uint16_t)(ENB_RRC_EVENT_BASE + 9)
#define ENBRRC_RRC_CONN_REEST                   (uint16_t)(ENB_RRC_EVENT_BASE + 10)
#define ENBRRC_RRC_CONN_REEST_REJ               (uint16_t)(ENB_RRC_EVENT_BASE + 11)
#define ENBRRC_RRC_CONN_REEST_CMP               (uint16_t)(ENB_RRC_EVENT_BASE + 12)
#define ENBRRC_RRC_CONN_REL                     (uint16_t)(ENB_RRC_EVENT_BASE + 13)
#define ENBRRC_UE_CAP_ENQUIRY                   (uint16_t)(ENB_RRC_EVENT_BASE + 14)
#define ENBRRC_UE_CAP_INFO                      (uint16_t)(ENB_RRC_EVENT_BASE + 15)
#define ENBRRC_UE_MEAS_RPT                      (uint16_t)(ENB_RRC_EVENT_BASE + 16)
#define ENB_NASRRC_CMD                          (uint16_t)(ENB_RRC_EVENT_BASE + 17)
#define ENB_NASRRC_RSP                          (uint16_t)(ENB_RRC_EVENT_BASE + 18)
#define NUNIT_EPHY_UE_MEAS_REQ                  (uint16_t)(ENB_RRC_EVENT_BASE + 19)
#define NUNIT_EPHY_UE_CER_HO_REQ_CTL            (uint16_t)(ENB_RRC_EVENT_BASE + 20)
#define ENBRRC_COUNTER_CHECK_SUCC_IND           (uint16_t)(ENB_RRC_EVENT_BASE + 21)
#define ENBRRC_COUNTER_CHECK_FAIL_IND           (uint16_t)(ENB_RRC_EVENT_BASE + 22)
#define ENBRRC_MOBILITY_FROM_EUTRA              (uint16_t)(ENB_RRC_EVENT_BASE + 23)
#define ENBRRC_START_HO_FROM_EUTRA_IND          (uint16_t)(ENB_RRC_EVENT_BASE + 24)
#define ENBRRC_START_HO_TO_EUTRA                (uint16_t)(ENB_RRC_EVENT_BASE + 25)
#define ENBRRC_PROXIMITY_RPT                    (uint16_t)(ENB_RRC_EVENT_BASE + 26)
#define ENBRRC_UE_INFO_REQ                      (uint16_t)(ENB_RRC_EVENT_BASE + 27)
#define ENBRRC_UE_INFO_RSP                      (uint16_t)(ENB_RRC_EVENT_BASE + 28)
#define ENBRRC_MBSFN_AREA_CONFIG                (uint16_t)(ENB_RRC_EVENT_BASE + 29)
#define ENBRRC_MBMS_COUNTING_REQ                (uint16_t)(ENB_RRC_EVENT_BASE + 30)
#define ENBRRC_GROUPCALL_CONFIG                 (uint16_t)(ENB_RRC_EVENT_BASE + 31)
#define ENBRRC_GROUPCALL_RELEASE                (uint16_t)(ENB_RRC_EVENT_BASE + 32)
#define ENRRC_NEIGHBOURINFO_CONFIG              (uint16_t)(ENB_RRC_EVENT_BASE + 33)


#define ENRRC_ENPDCP_SMC_INTEGRITY_CHECK_REQ    (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 0)
#define ENRRC_ENPDCP_CONFIG_CIPHER_KEY_REQ      (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 1)
#define ENRRC_ENPDCP_CONFIG_INTEGRITY_KEY_REQ   (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 2)
#define ENRRC_ENPDCP_CONFIG_REQ                 (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 3)
#define ENRRC_ENPDCP_DATA_REQ                   (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 4)
#define ENRRC_ENPDCP_REESTABLISH_REQ            (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 5)
#define ENRRC_ENPDCP_RELEASE_REQ                (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 6)
#define ENRRC_ENPDCP_RESUME_REQ                 (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 7)
#define ENRRC_ENPDCP_SUSPEND_REQ                (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 8)
#define ENRRC_ENPDCP_DECIPHER_AND_INTCHECK_REQ  (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 9)
#define ENRRC_ENPDCP_SEC_CONFIG_REQ             (uint16_t)(ENRRC_ENPDCP_EVENT_BASE + 10)

#define ENRRCENPDCP_SMC_INTEGRITY_CHECK_CNF     (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 0)
#define ENRRCENPDCP_DATA_IND                    (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 1)
#define ENRRCENPDCP_INTEGIRTY_FAIL_IND          (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 2)
#define ENRRCENPDCP_CONFIG_CNF                  (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 3)
#define ENRRCENPDCP_DATA_CNF                    (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 4)
#define ENRRCENPDCP_REESTABLISH_CNF             (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 5)
#define ENRRCENPDCP_ENABLE_UL_CIPHER_REQ        (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 6)
#define ENRRCENPDCP_ENABLE_UL_DECIPHER_REQ      (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 7)
#define ENRRCENPDCP_COUNTER_CHECK_REQ           (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 8)
#define ENRRCENPDCP_MDT_REQ                     (uint16_t)(ENRRC_ENPDCP_RSP_EVENT + 9)

#define ENPDCP_EDCP_COMPLETE_IND                (uint16_t)(PS_ENDCP_RSP_EVENT + 0)
#define ENRLC_ENMAC_COMPLETE_IND                (uint16_t)(PS_ENDCP_RSP_EVENT + 1)

#define ENPDCP_DISCARDTIMER                     (uint16_t)(ENPDCP_TIMER_EVENT_BASE + 0)
#define SIMEPHY_TIMER_CREAT_REQ                 (uint16_t)(ENPDCP_TIMER_EVENT_BASE + 1)
#define SIMEPHY_TIMER_START_REQ                 (uint16_t)(ENPDCP_TIMER_EVENT_BASE + 2)
#define SIMEPHY_TIMER_STOP_REQ                  (uint16_t)(ENPDCP_TIMER_EVENT_BASE + 3)
#define SIMEPHY_TIMER_DESTROY_REQ               (uint16_t)(ENPDCP_TIMER_EVENT_BASE + 4)


#define ENPSA_DATA_REQ                          (uint16_t)(ENPSA_ENPDCP_EVENT_BASE + 0)

#define ENPSA_DATA_IND                          (uint16_t)(ENPSA_ENPDCP_RSP_EVENT + 0)
#define ENPSA_NOT_READY_IND                     (uint16_t)(ENPSA_ENPDCP_RSP_EVENT + 1)
#define ENPSA_READY_IND                         (uint16_t)(ENPSA_ENPDCP_RSP_EVENT + 2)




#define LTE_GCF_STARTCHECK_REQ                  (uint16_t)(LTE_GCF_NUNIT_EVENT_BASE + 0)
#define LTE_GCF_CHECKPASS_IND                   (uint16_t)(LTE_GCF_NUNIT_EVENT_BASE + 1)
#define LTE_GCF_CHECKFAIL_IND                   (uint16_t)(LTE_GCF_NUNIT_EVENT_BASE + 2)

#define LTE_GCF_CHECK_TIMER                     (uint16_t)(LTE_GCF_TIMER_EVENT_BASE + 0)


#define NUNIT_ENRLC_UMPDU_REQ                   (uint16_t)(NUNIT_ENRLC_EVENT_BASE + 0)
#define NUNIT_ENRLC_AMPDU_REQ                   (uint16_t)(NUNIT_ENRLC_EVENT_BASE + 1)
#define NUNIT_ENRLC_SDU_REQ                     (uint16_t)(NUNIT_ENRLC_EVENT_BASE + 2)
#define NUNIT_ENRLC_AUTOACK_REQ                 (uint16_t)(NUNIT_ENRLC_EVENT_BASE + 3)


#define SIMULATI_DATA_REQ                       (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 0)
#define SIMULATI_PERIOD_DATA_REQ                (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 1)
#define SIMULATI_SEND_DATA_TIMER                (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 2)
#define SIMULATI_PERIOD_DATA_STOP               (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 3)
#define SIMULATI_CLEAR_STATISTICS               (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 4)
#define SIMULATI_SHOW_STATISTICS                (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 5)
#define SIMULATI_DATA_REQ_EX                    (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 6)
#define SIMULATI_DATA_IND                       (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 7)
#define SIMULPSI_CONFIG                         (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 8)
#define SIMULATI_ROHC_IPV4_DATA_CONFIG          (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 9)
#define SIMULATI_ROHC_IPV6_DATA_CONFIG          (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 10)
#define SIMULATI_ROHC_UDP_DATA_CONFIG           (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 11)
#define SIMULATI_ROHC_RTP_DATA_CONFIG           (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 12)
#define SIMULATI_ROHC_ESP_DATA_CONFIG           (uint16_t)(NUNIT_SIMULPSA_EVENT_BASE + 13)

#define SIMULENPSA_DATA_REQ                     (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 0)
#define SIMULENPSA_PERIOD_DATA_REQ              (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 1)
#define SIMULENPSA_SEND_DATA_TIMER              (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 2)
#define SIMULENPSA_PERIOD_DATA_STOP             (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 3)
#define SIMULENPSA_CLEAR_STATISTICS             (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 4)
#define SIMULENPSA_SHOW_STATISTICS              (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 5)
#define SIMULENPSA_DATA_REQ_EX                  (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 6)
#define SIMULENPSA_ROHC_IPV4_DATA_CONFIG        (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 7)
#define SIMULENPSA_ROHC_IPV6_DATA_CONFIG        (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 8)
#define SIMULENPSA_ROHC_UDP_DATA_CONFIG         (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 9)
#define SIMULENPSA_ROHC_RTP_DATA_CONFIG         (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 10)
#define SIMULENPSA_ROHC_ESP_DATA_CONFIG         (uint16_t)(NUNIT_SIMULENPSA_EVENT_BASE + 11)

#define ENRRC_ENRLC_CONFIG_REQ                  (uint16_t)(ENRRC_ENRLC_EVENT_BASE + 0)
#define ENRRC_ENRLC_REESTABLISH_REQ             (uint16_t)(ENRRC_ENRLC_EVENT_BASE + 1)
#define ENRRC_ENRLC_RELEASE_REQ                 (uint16_t)(ENRRC_ENRLC_EVENT_BASE + 2)

#define ENRLC_ENRRC_CONFIG_CNF                  (uint16_t)(ENRRC_ENRLC_RSP_EVENT + 0)
#define ENRLC_ENRRC_REESTABLISH_CNF             (uint16_t)(ENRRC_ENRLC_RSP_EVENT + 1)
#define ENRLC_ENRRC_RETX_FAIL_IND               (uint16_t)(ENRRC_ENRLC_RSP_EVENT + 2)

#define ENRLC_ENPDCP_UMDATA_CNF                 (uint16_t)(ENPDCP_ENRLC_EVENT_BASE + 0)


#define ENRLC_REORDERING_TIMER                  (uint16_t)(ENRLC_TIMER_EVENT_BASE + 0)

#define EPHY_ENMAC_DATA_IND                     (uint16_t)(ENMAC_EPHY_RSP_EVENT + 0)


#define ENMAC_CCCH_DATA_REQ                     (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 0)
#define ENMAC_COMM_CONFIG_REQ                   (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 1)
#define ENMAC_DEDI_CONFIG_REQ                   (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 2)
#define ENMAC_REL_REQ                           (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 3)
#define ENMAC_RESET_MAC_REQ                     (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 4)
#define ENMAC_RESUME_RB_REQ                     (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 5)
#define ENMAC_SUSPEND_RB_REQ                    (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 6)
#define ENMAC_ACTIVE_CONFIG_REQ                 (uint16_t)(ENRRC_ENMAC_EVENT_BASE + 7)



#define ENMAC_CCCH_DATA_IND                     (uint16_t)(ENRRC_ENMAC_RSP_EVENT + 0)
#define ENMAC_RA_PROBLEM_IND                    (uint16_t)(ENRRC_ENMAC_RSP_EVENT + 1)
#define ENMAC_RA_SUCCESS_IND                    (uint16_t)(ENRRC_ENMAC_RSP_EVENT + 2)


#define EPHY_TIMER_INTERUPT                     (uint16_t)(NUNIT_EPHY_EVENT_BASE + 1)
#define NUNIT_EPHY_DUPLICATE_SEND_CONFIG        (uint16_t)(NUNIT_EPHY_EVENT_BASE + 2)
#define EPHY_DL_RARESULT_CONFIG_REQ             (uint16_t)(NUNIT_EPHY_EVENT_BASE + 3)
#define EPHY_DL_CRRESULT_CONFIG_REQ             (uint16_t)(NUNIT_EPHY_EVENT_BASE + 4)
#define NUNIT_EPHY_DISCARD_CONFIG_REQ           (uint16_t)(NUNIT_EPHY_EVENT_BASE + 5)
#define NUNIT_EPHY_GRANT_CONFIG                 (uint16_t)(NUNIT_EPHY_EVENT_BASE + 6)
#define NUNIT_EPHY_DISORDER_SEND_CONFIG         (uint16_t)(NUNIT_EPHY_EVENT_BASE + 7)

#define EPHY_ULGRANT_REQ                        (uint16_t)(NUNIT_EPHY_EVENT_BASE + 8)
#define EPHY_DLGRANT_REQ                        (uint16_t)(NUNIT_EPHY_EVENT_BASE + 9)
#define EPHY_GRANTARRAYCONFIG_REQ               (uint16_t)(NUNIT_EPHY_EVENT_BASE + 10)
#define EPHY_IDLE_PERIOD_REP_CONFIG_REQ         (uint16_t)(NUNIT_EPHY_EVENT_BASE + 11)
#define EPHY_CONFIG_REQ                         (uint16_t)(NUNIT_EPHY_EVENT_BASE + 12)

#define NUNIT_FREQ_SCAN_IND                     (uint16_t)(NUNIT_EPHY_EVENT_BASE + 13)
#define NUNIT_CELL_SCAN_IND                     (uint16_t)(NUNIT_EPHY_EVENT_BASE + 14)
#define NUNIT_CELL_DEL_IND                      (uint16_t)(NUNIT_EPHY_EVENT_BASE + 15)
#define NUNIT_CELL_MOD_IND                      (uint16_t)(NUNIT_EPHY_EVENT_BASE + 16)
#define NUNIT_MODE_SET_IND                      (uint16_t)(NUNIT_EPHY_EVENT_BASE + 17)
#define NUNIT_MSG_MODE_SET_IND                  (uint16_t)(NUNIT_EPHY_EVENT_BASE + 18)
#define EPHY_CELLINFOCONFIG_REQ                 (uint16_t)(NUNIT_EPHY_EVENT_BASE + 19)
#define EPHY_EXTGRANTCONFIG_REQ                 (uint16_t)(NUNIT_EPHY_EVENT_BASE + 20)

#define NUNIT_ENMAC_TA_CONFIG_REQ               (uint16_t)(NUNIT_ENMAC_EVENT_BASE + 1)
#define NUNIT_ENMAC_DRX_CONFIG_REQ              (uint16_t)(NUNIT_ENMAC_EVENT_BASE + 2)
#define NUNIT_ENMAC_CCCH_CONFIG_REQ             (uint16_t)(NUNIT_ENMAC_EVENT_BASE + 3)
#define NUNIT_ENMAC_CRID_CONFIG_REQ             (uint16_t)(NUNIT_ENMAC_EVENT_BASE + 4)
#define NUNIT_ENMAC_BACKOFF_CONFIG_REQ          (uint16_t)(NUNIT_ENMAC_EVENT_BASE + 5)

#ifdef EMBMS_SUPPORT
#define ENMAC_MCCH_DATA_REQ                     (uint16_t)(ENMEL2_EVENT_BASE + 0 )
#define ENRRC_ENMEL2_REL_CONFIG_REQ             (uint16_t)(ENMEL2_EVENT_BASE + 1)
#define NUNIT_ENMEL2_RLC_SDU_REQ                (uint16_t)(ENMEL2_EVENT_BASE + 2)
#define NUNIT_ENMEL2_RLC_PDU_REQ                (uint16_t)(ENMEL2_EVENT_BASE + 3)
#define NUNIT_ENMEL2_MSI_CONFIG_REQ             (uint16_t)(ENMEL2_EVENT_BASE + 4)
#endif

#define EUDBG_EMM_PLAIN_DL_MSG                  (uint16_t)(EUDBG_EVENT_BASE + 1)
#define EUDBG_SEND_RLCSRBPDU_INFO               (uint16_t)(EUDBG_EVENT_BASE + 2)
#define EUDBG_RECV_RLCSRBPDU_INFO               (uint16_t)(EUDBG_EVENT_BASE + 3)
#define EUDBG_AM_SEND_STATUS_PDU_INFO           (uint16_t)(EUDBG_EVENT_BASE + 4)
#define EUDBG_AM_RECV_STATUS_PDU_INFO           (uint16_t)(EUDBG_EVENT_BASE + 5)
#define PS_PHY_DLSCH_DATA_TRACE                  (uint16_t)(EUDBG_EVENT_BASE + 6)
#define PS_PHY_ULSCH_DATA_TRACE                  (uint16_t)(EUDBG_EVENT_BASE + 7)
#define LTE_P_MAC_SR_REQ                        (uint16_t)(EUDBG_EVENT_BASE + 8)
#define LTE_EL2_THROUGHPUT_IND                  (uint16_t)(EUDBG_EVENT_BASE + 9)
#define LTE_EL2_STATE_IND                       (uint16_t)(EUDBG_EVENT_BASE + 10)
#define EUDBG_EMM_PLAIN_UL_MSG                  (uint16_t)(EUDBG_EVENT_BASE + 11)

#define AS_FUNC_CLEARBARFREQINFO                (uint16_t)(TOOL_FUN_TRACE_BASE + 0)
#define AS_FUNC_CLEARBARCELLINFO                (uint16_t)(TOOL_FUN_TRACE_BASE + 1)
#define RRC_SIM_SIB1_V8H0_IEs_INFO              (uint16_t)(TOOL_FUN_TRACE_BASE + 2)
#define RRC_SIM_SIB2_V8H0_IEs_INFO              (uint16_t)(TOOL_FUN_TRACE_BASE + 3)
#define RRC_SIM_SIB5_V8H0_IEs_INFO              (uint16_t)(TOOL_FUN_TRACE_BASE + 4)
#define MSGTRACEPS_CELL_DISPLAY_REQ             (uint16_t)(TOOL_FUN_TRACE_BASE + 5)

#ifdef IMS_SUPPORT
#define IMS_NAS_PDP_ACTIVATE_REQ                ATI_ESM_PDP_ACTIVATE_REQ
#define IMS_NAS_CGCONTRDP_REQ                   ATI_ESM_CGCONTRDP_REQ
#define IMS_NAS_PDP_DEACTIVATE_REQ              ATI_ESM_PDP_DEACTIVATE_REQ
#define IMS_NAS_CEREG_QUERY_REQ                 ATI_EMM_CEREG_QUERY_REQ
#define IMS_NAS_RF_CTRL_REQ                     ATI_EMM_RF_CTRL_REQ

/*IMS <--> EMM */
#define IMS_NAS_IMSREG_STATE_REQ                (uint16_t)(EVENT_IMS_EMM_BASE + 0)

#define IMS_NAS_CEREG_QUERY_CNF                 (uint16_t)(IMS_EMM_RSP_EVENT + 0)
#define NAS_IMS_EPS_REG_STATE_IND               (uint16_t)(IMS_EMM_RSP_EVENT + 1)
#define NAS_IMS_IMS_VO_PS_IND                   (uint16_t)(IMS_EMM_RSP_EVENT + 2)
#define NAS_IMS_IEMERGENCY_BEARER_IND           (uint16_t)(IMS_EMM_RSP_EVENT + 3)
#define NAS_IMS_EMC_NUM_IND                     (uint16_t)(IMS_EMM_RSP_EVENT + 4)

/*IMS <--> SM */
#define NAS_IMS_CGEV_IND                        (uint16_t)(IMS_SM_RSP_EVENT + 0)
#define IMS_NAS_PDP_ACTIVATE_CNF                (uint16_t)(IMS_SM_RSP_EVENT + 1)
#define IMS_NAS_PDP_DEACTIVATE_CNF              (uint16_t)(IMS_SM_RSP_EVENT + 2)
#define IMS_NAS_CGCONTRDP_CNF                   (uint16_t)(IMS_SM_RSP_EVENT + 3)

/*IMS <--> USIM */
#define IMS_NAS_READ_ECC_REQ                    (uint16_t)(EVENT_IMS_USIM_BASE + 0)
#define IMS_NAS_AUTH_REQ                        (uint16_t)(EVENT_IMS_USIM_BASE + 1)

#define IMS_NAS_READ_ECC_CNF                    (uint16_t)(IMS_USIM_RSP_EVENT + 0)
#define IMS_NAS_AUTH_CNF                        (uint16_t)(IMS_USIM_RSP_EVENT + 1)
#define NAS_IMS_UICC_INFO_IND                   (uint16_t)(IMS_USIM_RSP_EVENT + 2)
#define NAS_IMS_ISIM_INFO_IND                   (uint16_t)(IMS_USIM_RSP_EVENT + 3)

/*IMS <--> SMS */
#define SMS_IMS_SEND_DATA_REQ                   (uint16_t)(EVENT_IMS_SMS_BASE + 0)
#define SMS_IMS_REL_ENTITY_IND                  (uint16_t)(EVENT_IMS_SMS_BASE + 1)
#define SMS_IMS_SEND_DATA_CNF                   (uint16_t)(IMS_SMS_RSP_EVENT + 0)
#define IMS_SMS_RCV_DATA_IND                    (uint16_t)(IMS_SMS_RSP_EVENT + 1)

/*IMS <--> ATI */
#define IMS_ATI_CGDCONT_REQ                     (uint16_t)(EVENT_IMS_ATI_BASE + 0)
#define IMS_ATI_GET_PLMN_NAME_REQ               (uint16_t)(EVENT_IMS_ATI_BASE + 1)
#define IMS_ATI_IMSBAR_QUERY_REQ                (uint16_t)(EVENT_IMS_ATI_BASE + 2)

#define IMS_ATI_CGDCONT_CNF                     (uint16_t)(IMS_ATI_RSP_EVENT + 0)
#define IMS_ATI_GET_PLMN_NAME_CNF               (uint16_t)(IMS_ATI_RSP_EVENT + 1)
#define IMS_ATI_IMSBAR_QUERY_CNF                (uint16_t)(IMS_ATI_RSP_EVENT + 2)
#define NAS_IMS_LTENOCELL_IND                   (uint16_t)(IMS_ATI_RSP_EVENT + 3)

#endif

#endif

