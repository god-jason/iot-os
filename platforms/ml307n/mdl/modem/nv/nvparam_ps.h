/****************************************************************************************************
 * Copyright (c) 2023, 芯昇科技有限公司
 *
 * @file        nvparam_ps.h
 *
 * @brief       Headfile of nv
 *
 * @revision
 * Date                   Author            Notes
 * 2023-4-20
*****************************************************************************************************/

#ifndef NVPARAM_PS_H
#define NVPARAM_PS_H

#include "ap_ps_interface.h"

#define MAX_EPLMN_NUM                                (uint8_t)15/*EPLMN max count, not include current reg plmn*/
#define DEV_NV_EPSNSC_MAX_NUM                        (uint8_t)3
#define DEV_NV_NEIGH_CELL_CGI_NUM                    (uint8_t)8

#define NV_LTE_BAND_INFO_NUM                         (uint8_t)9

#define MAX_LTE_BAND_NUM                             (uint8_t)41
#define MAX_LTE_SIMULBANDS_NUM                       (uint8_t)2
#define DEV_NV_LTE_STORE_FREQ_NUM                    (uint8_t)5
#define DEV_NV_LTE_STORE_HIST_EXT_NUM                (uint8_t)3
#define DEV_NV_MAX_PLMNLIST_NUM                      (uint8_t)20

#define DEV_UICC_ICCID_MAX_NUM                       (uint8_t)10
#define DEV_MAX_EXP_EF_NUM                           (uint8_t)30
#define DEV_MAX_SELCET_DATA_LEN                      (uint8_t)10

#define DEV_NV_FPLMNLIST_NUM                         (uint8_t)5

/*==================operator bands======================*/
#define MAX_OPERATOR_NUM                             (uint8_t)4   /* max configurable operator number */
#define MAX_OPERATOR_PLMN_NUM                        (uint8_t)7  /* max configurable operator PLMN number*/
#define MAX_OPERATOR_BAND_NUM                        (uint8_t)9  /* max configurable operator band number*/

#define MAX_OPERATOR_FREQ_NUM                        (uint8_t)8  /* max configurable operator frequency number*/

/*===================PDP context===========================*/
#define DEV_MAX_APN_LEN                              (uint8_t)100
#define DEV_MAX_PDPADDR_LEN                          (uint8_t)20
#define DEV_MAX_STR_LEN                              (uint8_t)64  /*APN auth max length*/
#define MAX_CID_NUM                                  (uint8_t)8

/*=====================PLMN INFO ========================*/
#define MAX_ME_OPER_NUM                              (uint8_t)18 /*NV ME PLMN info length，no modifying*/
#define MAX_NET_OPER_NUM                             (uint8_t)12 /*NV NET PLMN info length，no modifying*/
#define MAX_LONGNAME_LEN                             (uint8_t)32 /*operator long name, 16 in 3GPP27007，no modifying*/
#define MAX_SHORTNAME_LEN                            (uint8_t)16 /*operator short name 8 in 3GPP27007，no modifying*/
#define MAX_OPERATE_PLMNNUM                          (uint8_t)15 /*number of PLMNID*/

#define NV_MAX_MANU_PLMN_NUM                         (uint8_t)10

#define MAX_USAT_TERM_PROF_LEN                       (uint8_t)32 /* length of USAT Terminal Profile */

#define NV_MAX_PSEUDO_MAX_NUM                         (uint8_t)5  /* 伪基站存储最多个数 */

#ifndef BITS
typedef uint8_t          BITS;
#endif

#define MAX_ECELL_NUM                                (uint8_t)4
#define PS_MAX_EFREQ                                 (uint8_t)8
#define PS_MAX_STORED_EFRQE_NUM                      (uint8_t)(PS_MAX_EFREQ + 1)

/*=====================IMS INFO ========================*/
#define DEV_IMS_APN_MAX_LEN 32
#define DEV_IMS_CONF_URI_MAX_LEN 281
#define DEV_MCCMNC_MAX_LEN  6
#define DEV_MCCMNC_ITEM_MAX_COUNT 5
/* @NV-PS {
        "name": "Plmn信息",
        "description": "EPLMN、RPLMN和频点信息"
   }
*/
/* 客户可见不可修改 */
typedef struct {
    /* @NV-ITEM {
            "name": "EPLMN个数",
            "range": "0-15",
            "description": "EPLMN个数, 范围: 0~15, 默认值 0 "
           }
    */
    /* 客户可见不可修改 */
    uint8_t     bEPlmnNum;  /* EPLMN count saved before switchoff, val: 0~5, default 0 */

    /* @NV-ITEM {
        "name": "RPlmn信息",
        "range": "",
        "description": "RPlmn信息"
       }
    */
    /* 客户可见不可修改 */
    S_PS_PlmnId tRPlmnId;/* RPLMN info saved before switchoff, val 0~9, invalid val 0xf */

    /* @NV-ITEM {
        "name": "EPlmn信息",
        "range": "",
        "description": "保存的EPlmn信息, 个数由前面的EPlmnNum指定"
       }
    */
    /* 客户可见不可修改 */
    S_PS_PlmnId atEPlmnList[MAX_EPLMN_NUM]; /* EPLMN info saved before switchoff, default val all bits are 1 */
}S_Ps_Dev_NV_PlmnInfoList;

/* cat1最多支持到band41*/
/* @NV-PS {
           "name": "LTE支持的频段及相应的频点等信息",
           "description": "LTE支持的频段及相应的频点信息,需要按照协议填写"
  }
*/
typedef struct{
    /* @NV-ITEM {
           "name": "频段号",
           "range": "1-41",
           "description": "频段号,范围1-41,实际支持还取决于校准band"
      }
    */
    /* 客户可见可修改 */
    uint8_t     bBand;           /* 1-41, cat1最多支持到41*/

    /* @NV-ITEM {
               "name": "频段是否支持",
               "range": "0-1",
               "description": "频段是否支持, 0:不支持,1:支持"
          }
    */
    /* 客户可见可修改 */
    BITS        bSupported:1;    /* band configuration, 0: not support, 1:support */

    /* @NV-ITEM {
           "name": "频段的双工模式",
           "range": "0-2",
           "description": "该频段的双工模式,0:无效值,1:FDD,2:TDD"
      }
    */
    /* 客户可见可修改 */
    BITS        bDuplexMode:2;    /* 1:FDD, 2:TDD */

    /* @NV-ITEM {
           "name": "频段是否支持半双工",
           "range": "0-1",
           "description": "频段是否支持半双工,0:不支持, 1:支持"
      }
   */
   /* 客户可见可修改 */
    BITS        bHalfDuplex:1;    /* Half-duplex supportted on not in Band1, 1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "频段的功率等级",
           "range": "0-3",
           "description": "频段的功率等级,取值范围0~3,default:2"
      }
   */
   /* 客户可见可修改 */
    BITS        bPwrClassOfBand:4; /* Powerclass of UE on band, range:0~3, default:2*/

    /* @NV-ITEM {
           "name": "下行频段带宽",
           "range": "",
           "description": "下行频段带宽,UINT16类型,单位是  100K HZ"
      }
    */
    /* 客户可见可修改 */
    uint16_t    dlBandRange;      /* 下行频段带宽 单位是  100K HZ */

    /* @NV-ITEM {
           "name": "下行起始频率",
           "range": "",
           "description": "下行起始频率,UINT16类型,单位是  100K HZ"
      }
    */
    /* 客户可见可修改 */
    uint16_t    dlStratHz;        /* 下行起始频率 单位是  100K HZ */

    /* @NV-ITEM {
           "name": "最小下行频点",
           "range": "",
           "description": "频段的最小下行频点,UINT32类型"
      }
    */
    /* 客户可见可修改 */
    uint32_t    dwMinDlEarfcn;    /* minmum downlink earfcn */

    /* @NV-ITEM {
           "name": "上行频段带宽",
           "range": "",
           "description": "上行频段带宽,UINT16类型,单位是  100K HZ"
      }
    */
    /* 客户可见可修改 */
    uint16_t    ulBandRange;      /* 上行频段带宽 单位是  100K HZ */

    /* @NV-ITEM {
           "name": "上行起始频率",
           "range": "",
           "description": "上行起始频率,UINT16类型,单位是  100K HZ"
      }
    */
    /* 客户可见可修改 */
    uint16_t    ulStratHz;        /* 上行起始频率 单位是  100K HZ */

    /* @NV-ITEM {
           "name": "最小上行频点",
           "range": "",
           "description": "最小上行频点,UINT32类型"
      }
    */
    /* 客户可见可修改 */
    uint32_t    minUlEarfcn;      /* minmum uplink earfcn */
}S_Ps_Dev_NV_EuraBandInfo;

/*S_Ps_Dev_NV_CustomTxPower: tx power limination for serving cell, default 0. this config working after device retart*/
/* @NV-PS {
           "name": "主模下服务小区的上行功率限制信息",
           "description": "主模下服务小区的上行功率限制信息"
   }
*/
/* 客户不可见 */
typedef struct{
    /* @NV-ITEM {
                   "name": "LTE上行功率限制信息",
                   "range": "0-63",
                   "description": "LTE上行功率限制信息,范围1-63,实际范围-30到33dbm,0为无效值,默认0"
           }
    */
    /* 客户不可见 */
    uint8_t      bLteCustomPemax;      /* range: 1-63, 0 invalid，actual range is -30 to 33 dbm*/
}S_Ps_Dev_NV_CustomTxPower;

/* @NV-PS {
           "name": "运营商定制频段信息列表",
           "description": "运营商定制频段信息列表的具体内容"
   }
*/
/* 客户可见可修改 */
typedef struct{
    /* @NV-ITEM {
               "name": "运营商频段信息是否有效",
               "range": "0-1",
               "description": "运营商频段信息是否有效,0:无效,1:有效"
       }
    */
    /* 客户可见可修改 */
    uint8_t              bFlag;    /* whether customized band infomation is R_VALID or R_INVALID */

    /* @NV-ITEM {
               "name": "运营商PLMN个数",
               "range": "0-7",
               "description": "运营商PLMN个数，最大值7"
       }
    */
    /* 客户可见可修改 */
    uint8_t              bPlmnNum;

    /* @NV-ITEM {
               "name": "运营商定制频段个数",
               "range": "0-9",
               "description": "运营商定制频段个数,最大值9"
       }
    */
    /* 客户可见可修改 */
    uint8_t              bBandNum;

    /* @NV-ITEM {
               "name": "运营商定制常用频点个数",
               "range": "0-8",
               "description": "运营商定制常用频点个数,最大值8"
       }
    */
    /* 客户可见可修改 */
    uint8_t              bFreqNum;

    /* @NV-ITEM {
               "name": "运营商PLMN信息列表",
               "range": "",
               "description": "运营商PLMN信息列表"
       }
    */
    /* 客户可见可修改 */
    S_PS_PlmnId          atPlmnList[MAX_OPERATOR_PLMN_NUM];  /* plmn list for operator */

    /* @NV-ITEM {
               "name": "频段号",
               "range": "1-41",
               "description": "频段号，范围1-41"
       }
    */
    /* 客户可见可修改 */
    uint8_t              abOperBand[MAX_OPERATOR_BAND_NUM];  /* operator customized band infomation */

    /* @NV-ITEM {
               "name": "运营商定制常用频点列表",
               "range": "",
               "description": "运营商定制常用频点列表"
       }
    */
    /* 客户可见可修改 */
    uint32_t             adwFreqList[MAX_OPERATOR_FREQ_NUM]; /* operator customized frequency infomation */
}S_Ps_Dev_NV_OperatorListInfo;

/* @NV-PS {
           "name": "运营商频段信息",
           "description": "运营商频段信息"
   }
*/
/* 客户可见可修改 */
typedef struct{
    /* @NV-ITEM {
           "name": "运营商定制频段信息列表",
           "range": "",
           "description": "运营商定制频段信息列表,最大5个"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_OperatorListInfo       tOperatorList[MAX_OPERATOR_NUM];      /* operator customized band infomation */
}S_Ps_Dev_NV_OperatorLteBandInfo;

/* @NV-PS {
        "name": "UE能力",
        "description": "UE支持的BAND、安全算法等信息"
   }
*/
typedef struct {
    /* UE support security capability parameter，refer to 24.301 9.9.3.36 */
    /*  EPS cipher alg */
    /* @NV-ITEM {
        "name": "是否支持加密算法EEA0",
        "range": "0-1",
        "description": "是否支持EPS 128EEA0加密算法，1：支持；0：不支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS        b128EEA0:1; /*support EPS 128EEA0 cipher alg，1 support, 0 not support, default 1 */

    /* @NV-ITEM {
        "name": "是否支持加密算法EEA1",
        "range": "0-1",
        "description": "是否支持EPS 128EEA1加密算法，1：支持；0：不支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS        b128EEA1:1; /*support EPS 128EEA1 cipher alg，1 support, 0 not support, default 1 */

    /* @NV-ITEM {
        "name": "是否支持加密算法EEA2",
        "range": "0-1",
        "description": "是否支持EPS 128EEA2加密算法，1：支持；0：不支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS        b128EEA2:1; /*support EPS 128EEA2 cipher alg，1 support, 0 not support, default 1 */

    /* @NV-ITEM {
        "name": "是否支持加密算法EEA3",
        "range": "0-1",
        "description": "是否支持EPS 128EEA3加密算法，1：支持；0：不支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS        bEEA3:1;  /*support EPS 128EEA3 cipher alg，1 support, 0 not support, default 1 */

    /* @NV-ITEM {
        "name": "是否支持加密算法EEA4",
        "range": "0-1",
        "description": "是否支持EPS EEA4加密算法，1：支持；0：不支持，默认0"
       }
    */
    /* 客户可见不可修改 */
    BITS        bEEA4:1;  /*support EPS 128EEA4 cipher alg，1 support, 0 not support, default 0 */

    /* @NV-ITEM {
        "name": "是否支持加密算法EEA5",
        "range": "0-1",
        "description": "是否支持EPS EEA5加密算法，1：支持；0：不支持，默认0"
       }
    */
    /* 客户可见不可修改 */
    BITS        bEEA5:1;  /*support EPS 128EEA5 cipher alg，1 support, 0 not support, default 0 */

    /* @NV-ITEM {
        "name": "是否支持加密算法EEA6",
        "range": "0-1",
        "description": "是否支持EPS EEA6加密算法，1：支持；0：不支持，默认0"
       }
    */
    /* 客户可见不可修改 */
    BITS        bEEA6:1;  /*support EPS 128EEA6 cipher alg，1 support, 0 not support, default 0 */

    /* @NV-ITEM {
        "name": "是否支持加密算法EEA7",
        "range": "0-1",
        "description": "是否支持EPS EEA7加密算法，1：支持；0：不支持，默认0"
       }
    */
    /* 客户可见不可修改 */
    BITS        bEEA7:1;  /*support EPS 128EEA7 cipher alg，1 support, 0 not support, default 0 */

    /* EPS intergrity protect alg */

    /* @NV-ITEM {
        "name": "是否支持完整性保护算法EIA0",
        "range": "0-1",
        "description": "是否支持EPS EIA0完整性保护算法，1：支持；0：不支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS        bEIA0:1;  /*support EPS EIA0 cipher alg，1 support, 0 not support, default 1 */

    /* @NV-ITEM {
        "name": "是否支持完整性保护算法EIA1",
        "range": "0-1",
        "description": "是否支持EPS 128EIA1完整性保护算法，1：支持；0：不支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS        b128EIA1:1; /*support EPS 128EIA1 cipher alg，1 support, 0 not support, default 1 */

    /* @NV-ITEM {
        "name": "是否支持完整性保护算法EIA2",
        "range": "0-1",
        "description": "是否支持EPS 128EIA2完整性保护算法，1：支持；0：不支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS        b128EIA2:1; /*support EPS 128EIA2 cipher alg，1 support, 0 not support, default 1 */

    /* @NV-ITEM {
        "name": "是否支持完整性保护算法EIA3",
        "range": "0-1",
        "description": "是否支持EPS EIA3完整性保护算法，1：支持；0：不支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS        bEIA3:1;   /*support EPS EIA3 cipher alg，1 support, 0 not support, default 1 */

    /* @NV-ITEM {
        "name": "是否支持完整性保护算法EIA4",
        "range": "0-1",
        "description": "是否支持EPS EIA4完整性保护算法，1：支持；0：不支持，默认0"
       }
    */
    /* 客户可见不可修改 */
    BITS        bEIA4:1;   /*support EPS EIA4 cipher alg，1 support, 0 not support, default 0 */

    /* @NV-ITEM {
        "name": "是否支持完整性保护算法EIA5",
        "range": "0-1",
        "description": "是否支持EPS EIA5完整性保护算法，1：支持；0：不支持，默认0"
       }
    */
    /* 客户可见不可修改 */
    BITS        bEIA5:1;   /*support EPS EIA5 cipher alg，1 support, 0 not support, default 0 */

    /* @NV-ITEM {
        "name": "是否支持完整性保护算法EIA6",
        "range": "0-1",
        "description": "是否支持EPS EIA6完整性保护算法，1：支持；0：不支持，默认0"
       }
    */
    /* 客户可见不可修改 */
    BITS        bEIA6:1;   /*support EPS EIA6 cipher alg，1 support, 0 not support, default 0 */

    /* @NV-ITEM {
        "name": "是否支持完整性保护算法EIA7",
        "range": "0-1",
        "description": "是否支持EPS EIA7完整性保护算法，1：支持；0：不支持，默认0"
       }
    */
    /* 客户可见不可修改 */
    BITS        bEIA7:1;   /*support EPS EIA7 cipher alg，1 support, 0 not support, default 0 */

    /* @NV-ITEM {
        "name": "FeaturGrpInd1",
        "range": "0-1",
        "description": "Intra-subframe frequency hopping for PUSCH scheduled by UL grant,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd1:1;  /* -Intra-subframe frequency hopping for PUSCH scheduled by UL grant
                                        - DCI format 3a (TPC commands for PUCCH and PUSCH with single bit power adjustments)
                                        - Multi-user MIMO for PDSCH
                                        - Aperiodic CQI/PMI/RI reporting on PUSCH: Mode 2-0 – UE selected subband CQI without PMI
                                        - Aperiodic CQI/PMI/RI reporting on PUSCH: Mode 2-2 – UE selected subband CQI with multiple PMI
                                        -PDSCH transmission mode 5
                                        1:support, 0:not support, default: 1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd2",
        "range": "0-1",
        "description": "Simultaneous CQI and ACK/NACK on PUCCH,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd2:1;  /* - Simultaneous CQI and ACK/NACK on PUCCH, i.e. PUCCH format 2a and 2b
                                        - Absolute TPC command for PUSCH
                                        - Resource allocation type 1 for PDSCH
                                        - Periodic CQI/PMI/RI reporting on PUCCH: Mode 2-0 – UE selected subband CQI without PMI
                                        - Periodic CQI/PMI/RI reporting on PUCCH: Mode 2-1 – UE selected subband CQI with single PMI
                                        1:support, 0:not support, default: 1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd3",
        "range": "0-1",
        "description": "5bit RLC UM SN, 7bit PDCP SN,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd3:1;  /* - 5bit RLC UM SN
                                      - 7bit PDCP SN
                                      1:support, 0:not support, default: 1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd4",
        "range": "0-1",
        "description": "Short DRX cycle,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd4:1;  /* - Short DRX cycle
                                      1:support, 0:not support, default: 1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd5",
        "range": "0-1",
        "description": "Long DRX cycle,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd5:1;  /* - Long DRX cycle
                                      - DRX command MAC control element
                                      1:support, 0:not support, default: 1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd6",
        "range": "0-1",
        "description": "Prioritised bit rate,1:support, 0:not support"
       }
   */
   /* 客户可见可修改 */
    BITS         bFeaturGrpInd6:1;  /* - Prioritised bit rate
                                      1:support, 0:not support, default: 1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd7",
        "range": "0-1",
        "description": "RLC UM,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd7:1;  /*- RLC UM
                                      1:support, 0:not support, default: 1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd13",
        "range": "0-1",
        "description": "Inter-frequency handover,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd13:1;  /* - Inter-frequency handover
                                         1:support, 0:not support, default: 1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd14",
        "range": "0-1",
        "description": "Measurement reporting event:A4,A5,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd14:1;  /* - Measurement reporting event: Event A4 – Neighbour > threshold
                                       - Measurement reporting event: Event A5 – Serving < threshold1 & Neighbour > threshold2
                                         1:support, 0:not support, default:1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd16",
        "range": "0-1",
        "description": "Periodical measurement reporting  for non-ANR related measurements,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd16:1;  /* - Periodical measurement reporting for non-ANR related measurements
                                         1:support, 0:not support, default:1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd17",
        "range": "0-1",
        "description": "Periodical measurement reporting for SON / ANR,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd17:1;  /* - Periodical measurement reporting for SON / ANR
                                       - ANR related intra-frequency measurement reporting events
                                         1:support, 0:not support, default:0*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd18",
        "range": "0-1",
        "description": "ANR related inter-frequency measurement reporting events,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd18:1;  /* - ANR related inter-frequency measurement reporting events
                                         1:support, 0:not support, default:0*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd20",
        "range": "0-1",
        "description": "If bit number 7 is set to ‘0’:- SRB1 and SRB2 for DCCH + 8x AM DRB
                        If bit number 7 is set to ‘1’:- SRB1 and SRB2 for DCCH + 8x AM DRB - SRB1 and SRB2 for DCCH + 5x AM DRB + 3x UM DRB
                        ,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd20:1;  /* If bit number 7 is set to ‘0’:
                                            - SRB1 and SRB2 for DCCH + 8x AM DRB
                                        If bit number 7 is set to ‘1’:
                                            - SRB1 and SRB2 for DCCH + 8x AM DRB
                                            - SRB1 and SRB2 for DCCH + 5x AM DRB + 3x UM DRB
                                         1:support, 0:not support, default:1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd21",
        "range": "0-1",
        "description": "Predefined intra- and inter-subframe frequency hopping for PUSCH with N_sb > 1,
                        Predefined inter-subframe frequency hopping for PUSCH with N_sb > 1,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd21:1;  /* - Predefined intra- and inter-subframe frequency hopping for PUSCH with N_sb > 1
                                       - Predefined inter-subframe frequency hopping for PUSCH with N_sb > 1
                                         1:support, 0:not support, default:1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd25",
        "range": "0-1",
        "description": "Inter-frequency measurements and reporting in E-UTRA connected mode,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd25:1;  /* - Inter-frequency measurements and reporting in E-UTRA connected mode
                                         1:support, 0:not support, default:1*/
    /* @NV-ITEM {
        "name": "FeaturGrpInd28",
        "range": "0-1",
        "description": "TTI bundling,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd28:1;  /* - TTI bundling
                                         1:support, 0:not support, default:1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd29",
        "range": "0-1",
        "description": "Semi-persistent scheduling,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd29:1;  /* - Semi-persistent scheduling
                                         1:support, 0:not support, default:1*/

    /* @NV-ITEM {
        "name": "FeaturGrpInd30",
        "range": "0-1",
        "description": "HandOver between FDD and TDD,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd30:1;  /* - HandOver between FDD and TDD
                                         1:support, 0:not support, default:1*/
    /* @NV-ITEM {
        "name": "FeaturGrpInd31",
        "range": "0-1",
        "description": "Indicates whether the UE supports the mechanisms defined for cells broadcasting multi band information,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd31:1;  /* - Indicates whether the UE supports the mechanisms defined for cells broadcasting multi band information
                                         1:support, 0:not support, default:0*/

    /* @NV-ITEM {
           "name": "FeaturGrpInd101",
           "range": "0-1",
           "description": "DMRS with OCC (orthogonal cover code) and SGH (sequence group hopping) disabling,1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd101:1; /* - DMRS with OCC (orthogonal cover code) and SGH (sequence group hopping) disabling
                                         1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "FeaturGrpInd102",
           "range": "0-1",
           "description": "Trigger type 1 SRS (aperiodic SRS) transmission (Up to X ports),1:support, 0:not support"
       }
    */
    /* 客户可见可修改 */
    BITS         bFeaturGrpInd102:1; /* - Trigger type 1 SRS (aperiodic SRS) transmission (Up to X ports)
                                          1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "是否支持FDD的PDSH传输模式5",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认1"
       }
    */
     /* 客户不可见 */
    BITS         bTm5FddSup:1;           /* Indicates whether the UE supports the PDSCH transmission mode 5 in FDD, 1:support, 0:not support, default:1 */

    /* @NV-ITEM {
           "name": "是否支持TDD的PDSH传输模式5",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认1"
       }
    */
    /* 客户不可见 */
    BITS         bTm5TddSup:1;           /* Indicates whether the UE supports the PDSCH transmission mode 5 in TDD,1:support, 0:not support, default:1 */

    /* @NV-ITEM {
           "name": "是否支持同频小区切换前的系统消息读取",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认1"
       }
    */
    /* 客户不可见 */
    BITS         bIntraFreqSiAcqForHo:1;   /* System infomation acquire in intrafreq-HO supportted or not, 1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "是否支持异频小区切换前的系统消息读取",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认1"
       }
    */
    /* 客户不可见 */
    BITS         bInterFreqSiAcqForHo:1;   /* System infomation acquire in interfreq-HO supportted or not, 1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "ROHC 3095 RTP支持与否",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bProfile_0001:1;  /*  ROHC 3095 RTP  0~1  init：0 */

    /* @NV-ITEM {
           "name": "ROHC 3095 UDP支持与否",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bProfile_0002:1;  /*  ROHC 3095 UDP 0~1  init：0 */

    /* @NV-ITEM {
           "name": "ROHC 3095 ESP支持与否",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bProfile_0003:1;  /*  ROHC 3095 ESP 0~1  init：0 */

    /* @NV-ITEM {
           "name": "ROHC 3843 IP支持与否",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bProfile_0004:1;  /*  ROHC 3843 IP：0~1  init：0 */

    /* @NV-ITEM {
           "name": "ROHC 4996 TCP支持与否",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bProfile_0006:1;  /*  ROHC 4996 TCP：0~1  init：0 */

    /* @NV-ITEM {
           "name": "ROHC v2 5225 RTP支持与否",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bProfile_0101:1;  /*  ROHC v2 5225 RTP：0~1  init：0 */

    /* @NV-ITEM {
           "name": "ROHC v2 5225 UDP支持与否",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bProfile_0102:1;  /*  ROHC v2 5225 UDP：0~1  init：0 */

    /* @NV-ITEM {
           "name": "ROHC v2 5225 ESP支持与否",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bProfile_0103:1;  /*  ROHC v2 5225 ESP：0~1  init：0 */

    /* @NV-ITEM {
           "name": "ROHC v2 5225 IP支持与否",
           "range": "0-1",
           "description": "0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bProfile_0104:1;  /*  ROHC v2 5225 IP：0~1  init：0 */

    /* @NV-ITEM {
           "name": "发射天线选择功能支持与否  ",
           "range": "0-1",
           "description": "物理层参数,0:不支持,1:支持,默认0"
      }
    */
    /* 客户可见不可修改 */
    BITS         bTxAnteSelSupported:1;  /*  0~1   init：0*/

    /* @NV-ITEM {
           "name": "指定参考信号功能支持与否",
           "range": "0-1",
           "description": "物理层参数,0:不支持,1:支持,默认1"
      }
    */
    /* 客户可见不可修改 */
    BITS         bSpecRefSigSupported:1; /*  0~1   init：1*/

    /* @NV-ITEM {
           "name": "是否支持宽带参考信号接收功率测量",
           "range": "0-1",
           "description": "0:不支持   ,1:支持,默认1"
      }
    */
    /* 客户不可见 */
    BITS         bWBRsrqMeasSup:1;               /*WideBandRsrq measurement supportted or not, 1:support, 0:not support, default:0*/

    /* @NV-ITEM {
           "name": "是否支持降低频率优先级",
           "range": "0-1",
           "description": "0:不支持    ,1:支持,默认1"
      }
    */
    /* 客户不可见 */
    BITS         bDeprioSup:1;                   /*Deprioritisation of frequency supportted or not, 1:support, 0:not support, default:0*/

    /* @NV-ITEM {
        "name": "是否支持E-UTRA     FDD",
        "range": "0-1",
        "description": "是否支持E-UTRA     FDD,1:支持;0:不支持,默认1"
       }
    */
    /* 客户不可见 */
    BITS         bEutraFDD:1;    /*Indication of E-UTRA FDD support, 1:support, 0:not support, default: 1 */

    /* @NV-ITEM {
        "name": "是否支持E-UTRA     TDD",
        "range": "0-1",
        "description": "是否支持E-UTRA     TDD,1:支持;0:不支持,默认1"
       }
    */
    /* 客户不可见 */
    BITS         bEutraTDD:1;    /*Indication of E-UTRA TDD support, 1:support, 0:not support, default: 1 */

    /* @NV-ITEM {
           "name": "是否支持运营商频点扫频",
           "range": "0-1",
           "description": "是否支持运营商频点扫频,1:支持;0:不支持,默认1"
       }
    */
    /* 客户可见可修改 */
    BITS         bOperatorFreqsSupt:1;   /*OperatorFreqs support or not, 1:support, 0:not support, default:1*/

    /* @NV-ITEM {
           "name": "是否支持MultiBandPriorityAdjustment",
           "range": "0-1",
           "description": "是否支持MultiBandPriorityAdjustment,1:支持;0:不支持,默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bMultiBandPriorityAdjustment:1;   /*MultiBandPriorityAdjustment support or not, 1:support, 0:not support, default:0*/

    /* @NV-ITEM {
           "name": "是否支持NsPmax",
           "range": "0-1",
           "description": "是否支持NsPmax,1:支持;0:不支持,默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bNsPmax:1;   /*NsPmax support or not, 1:support, 0:not support, default:0*/

    /* @NV-ITEM {
           "name": "是否支持LogicalChannelSRProhibitTimer",
           "range": "0-1",
           "description": "是否支持LogicalChannelSRProhibitTimer,1:支持;0:不支持,默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bLogicalChannelSRProhibitTimer:1;   /*LogicalChannelSRProhibitTimer support or not, 1:support, 0:not support, default:0*/

    /* @NV-ITEM {
           "name": "ROHC头压缩支持最大数量",
           "range": "0-13",
           "description": "取值范围: 0~13    ，默认13"
      }
    */
    /* 客户可见不可修改 */
    uint8_t      bMaxNumRohcCs;          /*  maxNumberROHC-ContextSessions ：0~13    init：13*/
    uint8_t      bPadding;   /*Padding*/

    /* @NV-ITEM {
           "name": "LTE支持的BAND及相应的EARFCN的信息",
           "range": "",
           "description": "LTE支持的BAND及相应的EARFCN的信息,需要按照协议填写"
       }
    */
    S_Ps_Dev_NV_EuraBandInfo   atEuraBandInfo[NV_LTE_BAND_INFO_NUM];   /*Supportted LTE band and Earfcn info*/
}S_Ps_Dev_NV_UeCapa;
/* cat1最多支持到41 */
/* @NV-PS {
        "name": "频段是否支持",
        "description": "频段是否支持"
   }
*/
typedef struct {
    /* @NV-ITEM {
           "name": "是否支持频段1",
           "range": "0-1",
           "description": "LTE 频段1是否支持，0：不支持，1：支持，默认1"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand1:1;  /* Band1 supportted or not,  1:support, 0:not support, default:1 */

    /* @NV-ITEM {
           "name": "是否支持频段2",
           "range": "0-1",
           "description": "LTE 频段2是否支持，0：不支持，1：支持，默认0"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand2:1;  /* Band2 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "是否支持频段3",
           "range": "0-1",
           "description": "LTE 频段3是否支持，0：不支持，1：支持，默认1"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand3:1;  /* Band3 supportted or not,  1:support, 0:not support, default:1 */

    /* @NV-ITEM {
           "name": "是否支持频段4",
           "range": "0-1",
           "description": "LTE 频段4是否支持，0：不支持，1：支持，默认0"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand4:1;  /* Band4 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "是否支持频段5",
           "range": "0-1",
           "description": "LTE 频段5是否支持，0：不支持，1：支持，默认1"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand5:1;  /* Band5 supportted or not,  1:support, 0:not support, default:1 */

    /* @NV-ITEM {
           "name": "是否支持频段6",
           "range": "0-1",
           "description": "LTE 频段6是否支持，0：不支持，1：支持，默认0"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand6:1;  /* Band6 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "是否支持频段7",
           "range": "0-1",
           "description": "LTE 频段7是否支持，0：不支持，1：支持，默认0"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand7:1;  /* Band7 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "是否支持频段8",
           "range": "0-1",
           "description": "LTE 频段8是否支持，0：不支持，1：支持，默认1"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand8:1;  /* Band8 supportted or not,  1:support, 0:not support, default:1 */

    /* @NV-ITEM {
           "name": "是否支持频段9",
           "range": "0-1",
           "description": "LTE 频段9是否支持，0：不支持，1：支持，默认0"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand9:1;  /* Band9 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "是否支持频段10",
           "range": "0-1",
           "description": "LTE 频段10是否支持，0：不支持，1：支持，默认0"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand10:1; /* Band10 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "是否支持频段11",
           "range": "0-1",
           "description": "LTE 频段11是否支持，0：不支持，1：支持，默认0"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand11:1; /* Band11 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
           "name": "是否支持频段12",
           "range": "0-1",
           "description": "LTE 频段12是否支持，0：不支持，1：支持，默认0"
      }
    */
    /* 客户可见可修改 */
    BITS         bBand12:1; /* Band12 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段13",
            "range": "0-1",
            "description": "LTE 频段13是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand13:1; /* Band13 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段14",
            "range": "0-1",
            "description": "LTE 频段14是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand14:1; /* Band14 supportted or not,  1:support, 0:not support, default:0 */
    BITS         bBand15:1; /* Reserved,  default:0 */
    BITS         bBand16:1; /* Reserved,  default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段17",
            "range": "0-1",
            "description": "LTE 频段17是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand17:1; /* Band17 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段18",
            "range": "0-1",
            "description": "LTE 频段18是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand18:1; /* Band18 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段19",
            "range": "0-1",
            "description": "LTE 频段19是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand19:1; /* Band19 supportted or not,  1:support, 0:not support, default:0 */
    BITS         bBand20:1; /* Reserved,  default:0 */
    BITS         bBand21:1; /* Reserved,  default:0 */
    BITS         bBand22:1; /* Reserved,  default:0 */
    BITS         bBand23:1; /* Reserved,  default:0 */
    BITS         bBand24:1; /* Reserved,  default:0 */
    BITS         bBand25:1; /* Reserved,  default:0 */
    BITS         bBand26:1; /* Reserved,  default:0 */
    BITS         bBand27:1; /* Reserved,  default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段28",
            "range": "0-1",
            "description": "LTE 频段28是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand28:1; /* Band28 supportted or not,  1:support, 0:not support, default:0  */
    BITS         bBand29:1; /* Reserved,  default:0 */
    BITS         bBand30:1; /* Reserved,  default:0 */
    BITS         bBand31:1; /* Reserved,  default:0 */
    BITS         bBand32:1; /* Reserved,  default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段33",
            "range": "0-1",
            "description": "LTE 频段33是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand33:1; /* Band33 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段34",
            "range": "0-1",
            "description": "LTE 频段34是否支持，0：不支持，1：支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand34:1; /* Band34 supportted or not,  1:support, 0:not support, default:1 */

    /* @NV-ITEM {
            "name": "是否支持频段35",
            "range": "0-1",
            "description": "LTE 频段35是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand35:1; /* Band35 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段36",
            "range": "0-1",
            "description": "LTE 频段36是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand36:1; /* Band36 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段37",
            "range": "0-1",
            "description": "LTE 频段37是否支持，0：不支持，1：支持，默认0"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand37:1; /* Band37 supportted or not,  1:support, 0:not support, default:0 */

    /* @NV-ITEM {
            "name": "是否支持频段38",
            "range": "0-1",
            "description": "LTE 频段38是否支持，0：不支持，1：支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand38:1; /* Band38 supportted or not,  1:support, 0:not support, default:1 */

    /* @NV-ITEM {
            "name": "是否支持频段39",
            "range": "0-1",
            "description": "LTE 频段39是否支持，0：不支持，1：支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand39:1; /* Band39 supportted or not,  1:support, 0:not support, default:1 */

    /* @NV-ITEM {
            "name": "是否支持频段40",
            "range": "0-1",
            "description": "LTE 频段40是否支持，0：不支持，1：支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand40:1; /* Band40 supportted or not,  1:support, 0:not support, default:1 */

    /* @NV-ITEM {
            "name": "是否支持频段41",
            "range": "0-1",
            "description": "LTE 频段41是否支持，0：不支持，1：支持，默认1"
       }
    */
    /* 客户可见可修改 */
    BITS         bBand41:1; /* Band41 supportted or not,  1:support, 0:not support, default:1 */
}S_Ps_Dev_NV_Lteband;

#define NV_MAX_E_MOBID_NUM                      (uint8_t)8
typedef struct{
    uint8_t                     bIdType  :3;
    uint8_t                     bOorE:1;
    uint8_t                     bSpare0:4;

    uint8_t                     bIdNum;
    uint8_t                     abId[NV_MAX_E_MOBID_NUM];
}S_Ps_NV_MobileIdentity;

typedef struct{
    BITS                            bIdType:3;
    BITS                            bOorE:1;
    BITS                            bSpare2:4;

    S_PS_PlmnId                     tPlmn;
    uint8_t                         bMMEGROUPID[2];
    uint8_t                         bMMECODE;
    uint8_t                         bMTMSI[4];
}S_Ps_NV_EPSmobidGUTI;

/*TAC*/
typedef uint16_t                S_PS_TAC;

/*Tracking Area Identity*/
typedef struct {
    S_PS_PlmnId                 tPlmnId;
    S_PS_TAC                    tTac;
}S_PS_TAI;

typedef struct {
    S_Ps_NV_EPSmobidGUTI           tGuti;
    S_PS_TAI                       tTai;
    uint8_t                        bEpsUpdateStat;
}S_Ps_NV_EPSLOCI;

#define NV_MAX_K_ASME_LEN          32

typedef struct {
    uint8_t                           bNasSn;
    uint16_t                          wNasOverflowCnt;
} S_Ps_NV_NasCount;

typedef struct {
    uint8_t                           b3NasCipherAlg                 : 3;
    uint8_t                           b1Spare1                       : 1;
    uint8_t                           b3NasIntegerAlg                : 3;
    uint8_t                           b1Spare2                       : 1;
} S_Ps_NV_IntEncAlgId;

typedef struct {
    uint8_t                           bKsiAsmeFlg                     : 1;
    uint8_t                           bKAsmeFlg                       : 1;
    uint8_t                           bUpNasCntFlg                    : 1;
    uint8_t                           bDlNasCntFlg                    : 1;
    uint8_t                           bNasIntEncAlgIdFlg              : 1;
    uint8_t                           bKsiAsme                        : 3;
    uint8_t                           abKasme[NV_MAX_K_ASME_LEN];
    S_Ps_NV_NasCount                  tNasUlCount;
    S_Ps_NV_NasCount                  tNasDlCount;
    S_Ps_NV_IntEncAlgId               tNasIntEncAlgId;
}S_Ps_NV_EPSNSC;


typedef struct {
   S_Ps_NV_MobileIdentity        tImsi; /* IMSI, default bNum is 0，other uint8_ts all FF */
   S_Ps_NV_EPSLOCI               tEpsLoci;  /* default: bTauStatus is 1，Tac is 0， other uint8_ts all FF */
   S_Ps_NV_EPSNSC                tEPSncs; /* default：bValidFg 0，bCodeLen 100 (Note: this is    array, each elem should be inited) */
   uint8_t                       tTin;       /* TIN, temp ID of next update, 0 no temp id, 1  P-TMSI, 2 GUTI, 3 RAT_RELATED_TMSI, default 0 */
}S_Ps_Dev_NV_EpsParam;

typedef struct
{
    uint32_t                      dwEarfcn;
    uint16_t                      wPci;
    uint32_t                      dwCellId;
    uint16_t                      wCellTac;
    S_PS_PlmnId                   tPlmnId;
    int8_t                        cQ_RxLevMin;
    uint8_t                       bRsrp;
    uint8_t                       bRsrq;
    uint8_t                       bandwidth;
}S_Ps_Dev_NV_HistoryInfo;


/* store the earfcn that has normal campon successfully */
/* @NV-PS {
        "name": "历史频点信息",
        "description": "保存的历史频点列表"
   }
*/
/* 客户不可见 */
typedef struct
{
    /* @NV-ITEM {
            "name": "存储的频点的个数",
            "range": "0-5",
            "description": "存储的频点的个数"
       }
    */
    /* 客户不可见 */
    uint8_t                          bLteStoreFreqNum;

    /* @NV-ITEM {
            "name": "驻留成功的频点",
            "range": "",
            "description": "驻留成功的频点,都是UINT32类型"
       }
    */
    /* 客户不可见 */
    uint32_t                         adwEarfcn[DEV_NV_LTE_STORE_FREQ_NUM];
    uint8_t                          bHistNum;
    S_Ps_Dev_NV_HistoryInfo          aHistInfoExt[DEV_NV_LTE_STORE_HIST_EXT_NUM];
}S_Ps_Dev_NV_LteFreqHistoryInfo;

/* @NV-PS {
        "name": "CID上下文信息",
        "description": "CID上下文信息"
   }
*/
/* 客户可见可修改 */
typedef struct
{
    /* @NV-ITEM {
        "name": "pdp类型",
        "range": "1-3",
        "description": "pdp类型,1:IPV4, 2:IPV6, 3:IPV4V6"
    }
    */
    /* 客户可见可修改 */
    BITS                            bPdpType:3;             /* 1:IPV4, 2:IPV6, 3:IPV4V6*/

    /* @NV-ITEM {
        "name": "获取IPV4地址信息方式",
        "range": "0-1",
        "description": "获取IPV4地址信息方式,0:通过NAS信令分配IPV4地址,1:通过DHCP分配IPv4 地址,默认0"
       }
    */
    /* 客户可见可修改 */
    BITS                            bIPv4AddrAlloc:1;       /* default 0 */

    /* @NV-ITEM {
        "name": "PDP上下文是否用于紧急承载服务",
        "range": "0-1",
        "description": "PDP上下文是否用于紧急承载服务,0:不是紧急,1:是紧急"
       }
    */
    /* 客户可见可修改 */
    BITS                            bEmergencyInd:1;        /* default 0 */

    /* @NV-ITEM {
        "name": "是否是IMS专用上下文",
        "range": "0-1",
        "description": "是否是IMS专用上下文,0:不是IMS,1:是IMS"
       }
    */
    /* 客户可见可修改 */
    BITS                            bImcnSignInd:1;         /* default 0 */

    /* @NV-ITEM {
        "name": "P-CSCF的获取方式",
        "range": "0-2",
        "description": "P-CSCF的获取方式,0:不支持通过+CGDCONT命令获取P-CSCF地址,1:通过NAS信令获取P-CSCF地址,2:通过DHCP获取P-CSCF地址"
       }
    */
    /* 客户可见可修改 */
    BITS                            bPCscfDisc:2;           /* 范围 0-2 */

    /* @NV-ITEM {
        "name": "数据压缩类型",
        "range": "0-3",
        "description": "数据压缩类型,0:不压缩,1:压缩,2:V.42bis,3:V.44,默认0,不压缩"
       }
    */
    /* 客户可见可修改 */
    BITS                            bDComp:3;               /* 范围 0-3 */

    /* @NV-ITEM {
        "name": "头压缩类型",
        "range": "0-4",
        "description": "头压缩类型,0:不压缩,1:压缩,2:RFC1144(用于SNDCP),3:RFC2507,4:RFC3095(用于PDCP),默认0,不压缩"
       }
    */
    /* 客户可见可修改 */
    BITS                            bHComp:3;               /* 范围 0-4 */

    /* @NV-ITEM {
        "name": "接入点域名",
        "range": "",
        "description": "接入点域名,字符串类型的APN名"
       }
    */
    /* 客户可见可修改 */
    uint8_t                          abApn[DEV_MAX_APN_LEN];          /* default 0 */

    /* @NV-ITEM {
        "name": "终端的IP地址",
        "range": "",
        "description": "终端的IP地址"
       }
    */
    /* 客户可见可修改 */
    uint8_t                           abPdpAddr[DEV_MAX_PDPADDR_LEN];  /* default 0 */

}S_Ps_NV_CidContext;

/* @NV-PS {
        "name": "PCO鉴权信息",
        "description": "PCO鉴权信息"
   }
*/
/* 客户可见可修改 */
typedef struct
{
    /* @NV-ITEM {
        "name": "PDP鉴权类型",
        "range": "0-3",
        "description": "PDP鉴权类型,0:无鉴权类型,1:PAP类型,2:CHAP类型,3:PAP和CHAP类型"
       }
    */
    /* 客户可见可修改 */
    uint8_t                            bAuthType:4;            /* authentication type, default 0*/

    /* @NV-ITEM {
        "name": "用户密码",
        "range": "",
        "description": "用户密码,最大长度64,鉴权类型不是0时有效"
       }
    */
    /* 客户可见可修改 */
    uint8_t                            abPassWord[DEV_MAX_STR_LEN+1]; /* range 0-64, default 0 */

    /* @NV-ITEM {
        "name": "用户名",
        "range": "",
        "description": "用户名,最大长度64,鉴权类型不是0时有效"
       }
    */
    /* 客户可见可修改 */
    uint8_t                            abUserName[DEV_MAX_STR_LEN+1]; /* range 0-64, default 0 */
}S_Ps_NV_PcoAuth;

/* @NV-PS {
        "name": "PDP上下文信息",
        "description": "PDP上下文信息"
   }
*/
/* 客户可见可修改 */
typedef struct
{
    /* @NV-ITEM {
        "name": "PDP上下文是否定义",
        "range": "0-1",
        "description": "PDP上下文是否定义,0:未定义,1:已定义"
       }
    */
    /* 客户可见可修改 */
    BITS                            bPdpDefineFg:1; /* 0 not define， 1 define */

    /* @NV-ITEM {
        "name": "PDP上下文类型",
        "range": "0-1",
        "description": "PDP上下文类型,0:一次上下文,1:二次上下文"
       }
    */
    /* 客户可见可修改 */
    BITS                            bContFg:1;    /* 0 first PDP， 1 second PDP */

    /* @NV-ITEM {
        "name": "PCO鉴权信息是否定义",
        "range": "0-1",
        "description": "PCO鉴权信息是否定义,0:未定义,1:已定义"
       }
    */
    /* 客户可见可修改 */
    BITS                            bPcoAuthFg:1;   /* default 0 */

    /* @NV-ITEM {
        "name": "一次上下文对应的CID",
        "range": "0-8",
        "description": "一次上下文对应的CID,如果定义的是二次上下文，则需要设置对应的一次上下文的CID; 0是无效值"
       }
    */
    /* 客户可见可修改 */
    uint8_t                            bPreCid:4;      /*  CGDSCONT */

    /* @NV-ITEM {
        "name": "CID上下文信息",
        "range": "",
        "description": "CID上下文信息"
       }
    */
    S_Ps_NV_CidContext              tCidContext;    /*  CidContext */

    /* @NV-ITEM {
        "name": "PCO鉴权信息",
        "range": "",
        "description": "PCO鉴权信息"
       }
    */
    S_Ps_NV_PcoAuth                 tPcoAuth;       /*  PcoAuth info */
}S_Ps_Dev_NV_CidPdpContext;

/* @NV-PS {
        "name": "PDP上下文信息列表",
        "description": "PDP上下文信息列表，最大8组"
   }
*/
/* 客户可见可修改 */
typedef struct
{
    /* @NV-ITEM {
        "name": "PDP上下文信息",
        "range": "",
        "description": "PDP上下文信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_CidPdpContext    tCidPdpCont[MAX_CID_NUM];
}S_Ps_Dev_NV_PdpCont;

/* @NV-PS {
        "name": "运营商网络信息",
        "description": "运营商网络信息"
   }
*/
/* 客户可见可修改 */
typedef struct {
    /* @NV-ITEM {
        "name": "网络ID",
        "range": "",
        "description": "网络ID"
       }
    */
    /* 客户可见可修改 */
    S_PS_PlmnId                tPlmnId;

    /* @NV-ITEM {
        "name": "网络长名",
        "range": "",
        "description": "网络长名"
       }
    */
    /* 客户可见可修改 */
    uint8_t                     abFullName[MAX_LONGNAME_LEN];

    /* @NV-ITEM {
        "name": "网络短名",
        "range": "",
        "description": "网络短名"
       }
    */
    /* 客户可见可修改 */
    uint8_t                      abShortName[MAX_SHORTNAME_LEN];
}S_Ps_Dev_NV_OperPlmnInfo;

/* @NV-PS {
        "name": "ME配置的运营商网络信息",
        "description": "ME配置的运营商网络信息：网络ID和网络长短名"
   }
*/
/* 客户可见可修改 */
typedef struct {
    /* @NV-ITEM {
        "name": "ME配置的运营商网络信息列表",
        "range": "",
        "description": "ME配置的运营商网络信息列表，最大18个"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_OperPlmnInfo     atNvMePlmnIDInfo[MAX_ME_OPER_NUM];
}S_Ps_Dev_NV_MePlmnInfo;

/* @NV-PS {
        "name": "网络配置的运营商网络信息列表",
        "description": "网络配置的运营商网络信息列表，最大12个"
   }
*/
/* 客户可见可修改 */
typedef struct {
    /* @NV-ITEM {
        "name": "网络配置的运营商网络信息列表",
        "range": "",
        "description": "网络配置的运营商网络信息列表，最大12个"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_OperPlmnInfo         atNvNetPlmnIDInfo[MAX_NET_OPER_NUM];
}S_Ps_Dev_NV_NetPlmnInfo;

/* @NV-PS {
        "name": "搜网模式",
        "description": "对应+COPS命令设置的参数"
   }
*/
/* 客户可见可修改 */
typedef struct{
    /* @NV-ITEM {
        "name": "COPS模式",
        "range": "0,1,4",
        "description": "搜网模式,0:自动搜网模式,1:手动搜网模式,4:手动失败转自动"
       }
    */
    /* 客户可见可修改 */
    BITS                       bCopsMode:4;        /* default 0 */
    BITS                       bOperFg:1;          /* default 0 */
    BITS                       bCopsFormat:2;
    uint8_t                    bNameIndxStart; /* Valid if format is alpha. Start index in NV array */
    uint8_t                    bNameIndxEnd;   /* Valid if format is alpha. End index in NV array */

    /* @NV-ITEM {
        "name": "网络ID",
        "range": "",
        "description": "网络ID"
       }
    */
    /* 客户可见可修改 */
    S_PS_PlmnId                tPlmnId;         /* default 0xff */
    BITS                       bAct:4;          /*7：E_UTRAN*/
    BITS                       bLteSubAct:4;    /*0：TDD，1：FDD*/
}S_Ps_Dev_NV_CopsMode;

/* @NV-PS {
        "name": "短消息存储器类型",
        "description": "短消息存储器类型"
   }
*/
/* 客户可见可修改 */
typedef struct{
    /* @NV-ITEM {
        "name": "短消息读存储器<mem1>",
        "range": "0-1",
        "description": "短消息读存储器<mem1>,0:SR,1:SM"
       }
    */
    /* 客户可见可修改 */
    uint8_t              bMem1;  /* MEM1: default 1(SM)*/

    /* @NV-ITEM {
        "name": "短消息写存储器<mem2>",
        "range": "0-1",
        "description": "短消息写存储器<mem2>,0:SR,1:SM"
       }
    */
    /* 客户可见可修改 */
    uint8_t              bMem2;  /* MEM2: default 1(SM)*/

    /* @NV-ITEM {
        "name": "短消息接收存储器<mem3>",
        "range": "0-1",
        "description": "短消息接收存储器<mem3>,0:SR,1:SM"
       }
    */
    /* 客户可见可修改 */
    uint8_t              bMem3;  /* MEM3: default 1(SM)*/
}S_Ps_Dev_NV_SmsMemInfo;

/* @NV-PS {
        "name": "NV中的禁止PLMN列表",
        "description": "NV中的禁止PLMN列表"
   }
*/
/* 客户可见可修改 */
typedef struct{
    /* @NV-ITEM {
            "name": "禁止PLMN列表",
            "range": "",
            "description": "禁止PLMN列表,最大5个"
       }
    */
    /* 客户可见可修改 */
    S_PS_PlmnId    atFPlmnID[DEV_NV_FPLMNLIST_NUM];/*atFPlmnID[0]: 46020, other 0xff*/
}S_Ps_Dev_NV_FPlmnList;

/* @NV-PS {
         "name": "省电的T3324信息",
         "description": "省电的T3324信息"
    }
*/
/* 客户可见可修改 */
typedef struct{
     /* @NV-ITEM {
        "name": "省电模式",
        "range": "0-3",
        "description": "是否开启省电,0:不开,1:开,2:重置参数,3:自主PSM休眠模式"
       }
    */
    /* 客户可见可修改 */
    uint8_t                     bMode; /*0 disable psm, 1 enable psm, 2 disable psm and discard all parameter for psm */

     /* @NV-ITEM {
        "name": "单位",
        "range": "0,1,2,7",
        "description": "单位,0: 2秒,1: 1分钟,2: 6分钟,7: 未激活"
       }
    */
    /* 客户可见可修改 */
    uint8_t                     bUnit; /*step：0： 2S， 1：1min，2：6min， 7：deactive*/

     /* @NV-ITEM {
        "name": "定时器的值",
        "range": "0-31",
        "description": "定时器的值"
       }
    */
    /* 客户可见可修改 */
    uint8_t                     bTmrVal;
}S_Ps_Dev_NV_T3324;

/* @NV-PS {
        "name": "省电的T3412信息",
        "description": "省电的T3412信息"
   }
*/
/* 客户可见可修改 */
typedef struct{
     /* @NV-ITEM {
        "name": "单位",
        "range": "0-7",
        "description": "单位,0: 10分钟,1: 1小时,2: 10小时,3: 2秒,4: 30秒,5: 1分钟,6: 320小时,7:未激活"
       }
    */
    /* 客户可见可修改 */
    uint8_t                     bUnit; /*step：0:10min， 1：1hour，2：10hour， 3：2s，      4：30s，5：1min，6：320hour，7：deactive*/

    /* @NV-ITEM {
        "name": "定时器的值",
        "range": "0-31",
        "description": "定时器的值"
       }
    */
    /* 客户可见可修改 */
    uint8_t                     bTmrVal;
}S_Ps_Dev_NV_ExT3412;

/* @NV-PS {
        "name": "eDRX信息",
        "description": "eDRX信息"
   }
*/
/* 客户可见可修改 */
typedef struct {
    /* @NV-ITEM {
        "name": "寻呼时间窗",
        "range": "0-15",
        "description": "寻呼时间窗"
       }
    */
    /* 客户可见可修改 */
    uint8_t             bPageTimeValue;

    /* @NV-ITEM {
        "name": "eDRX的值",
        "range": "0-15",
        "description": "eDRX的值"
       }
    */
    /* 客户可见可修改 */
    uint8_t             bEdrxValue;

    /* @NV-ITEM {
        "name": "是否启用eDRX",
        "range": "0-3",
        "description": "是否启用eDRX,0:关闭,1:启用,2:启用eDRX并开启主动上报,3:关闭eDrx并且重置eDrx参数"
       }
    */
    /* 客户可见可修改 */
    uint8_t             bEdrxOper;       /*0 disable use of eDrx, 1 enable use of edrx, 2 enable use of eDrx and enable the unsolicited result code
                                       3. disable use of eDrx and discard all parameters for eDrx, or if available, reset to the manufacturer specific default value*/
}S_Ps_Dev_NV_EdrxInfo;

/* @NV-PS {
        "name": "短信Cnmi命令设置信息",
        "description": "用于设置某类短信到达时如何处理，存储上报、直接上报等"
   }
*/
/* 客户可见可修改 */
typedef struct {
    /* @NV-ITEM {
        "name": "通知TE的方式",
        "range": "0-3",
        "description": "通知TE的方式,一般不使用,直接使用后面的参数指示"
       }
    */
    /* 客户可见可修改 */
    uint8_t                            bCnmiMd;

    /* @NV-ITEM {
        "name": "设置短消息存储和通知TE内容格式",
        "range": "0-3",
        "description": "设置短消息存储和通知TE内容格式,0:只存储不上报,1:存储后上报+CMTI通知TE,
                       2:class 2类型的与1的操作相同,其他类型的直接用+CMT上报,3:class 3类型直接上报,其他类型与1的操作相同"
       }
    */
    /* 客户可见可修改 */
    uint8_t                            bCnmiMt;        /* AT+CNMI */

    /* @NV-ITEM {
        "name": "设置小区广播通知TE内容格式",
        "range": "0-3",
        "description": "0:不上报,1:存储后上报+CBMI通知TE,
                       2:用+CBM上报,3:class 3类型直接上报,存储后上报同1"
       }
    */
    /* 客户可见可修改 */
    uint8_t                            bCnmiBm;

    /* @NV-ITEM {
        "name": "设置状态报告通知TE内容格式",
        "range": "0-2",
        "description": "0:不上报,1:用+CDS通知TE,2:存储后上报+CDSI通知TE"
       }
    */
    /* 客户可见可修改 */
    uint8_t                            bCnmiDs;
    uint8_t                            bCnmiBfr;
}S_Ps_Dev_NV_SMS_CnmiCmd;

/* @NV-PS {
    "name": "LTE制式下定制信息",
    "description": "LTE制式下定制信息"
}
*/
/* 客户可见可修改 */
typedef struct {
    /* @NV-ITEM {
        "name": "开机进行全扫频标识",
        "range": "0-1",
        "description": "开机进行全扫频标识, 0：保持原有处理, 1：开机都走全扫频"
       }
    */
    /* 客户可见可修改 */
    uint8_t  bLteCellSearchFlag : 1; /* 开机进行全扫频标识, 0：保持原有处理, 1：开机都走全扫频 */

    /* @NV-ITEM {
        "name": "进行接入时是否进行随机数限制",
        "range": "0-1",
        "description": "进行接入时是否进行随机数限制, 0：保持原有处理, 1：屏蔽接入时的随机数限制"
       }
    */
    /* 客户可见可修改 */
    uint8_t  bLteAccessRandContrlFlg : 1; /* 进行接入时是否进行随机数限制, 0：保持原有处理, 1：屏蔽接入时的随机数限制 */
    //uint8_t  bLteCgautoFlg : 1;

    /* @NV-ITEM {
        "name": "cfun适配",
        "range": "0-1",
        "description": "此功能暂不使用，不要设置，目前只是占位用"
       }
    */
    /* 客户可见可修改 */
    uint8_t  bCfunFlag : 1; /*0 将cfun适配成(卡初始化、开射频，搜网)1只执行开射频动作*/
    uint8_t  Reserved : 5;
}S_Ps_Dev_NV_LteFlagInfo;

/* @NV-PS {
        "name": "省电信息",
        "description": "省电信息"
   }
*/
typedef struct{
    /* @NV-ITEM {
        "name": "省电的T3324信息",
        "range": "",
        "description": "省电的T3324信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_T3324                   tNvT3324;

    /* @NV-ITEM {
        "name": "省电的T3412信息",
        "range": "",
        "description": "省电的T3412信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_ExT3412                 tNvExT3412;
}S_Ps_Dev_NV_PsmInfo;


typedef struct{
    uint32_t  dwEarfcn;
    S_PS_TAI  tTai;
    int16_t   wPci;
}S_Ps_Dev_NV_PseudoStationItem;

/* @NV-PS {
        "name": "伪基站信息",
        "description": "伪基站信息"
   }
*/
typedef struct {
    /* @NV-ITEM {
        "name": "timer PseudoRemainlen",
        "range": "",
        "description": "伪基站黑名单有效期timer剩余时长"
       }
    */
    /* 客户不可见 */
    uint64_t     tPseudo_remainlen;
    
    /* @NV-ITEM {
        "name": "Pseudo timer len",
        "range": "",
        "description": "伪基站黑名单有效期timer时长, 单位秒，默认72*60*60"
       }
    */
    /* 客户可见可修改 */
    uint32_t     dwPseudo_timerlen;

    /* @NV-ITEM {
        "name": "count",
        "range": "",
        "description": "伪基站个数"
       }
    */
    /* 客户不可见 */
    uint8_t   bCount;

    /* @NV-ITEM {
        "name": "station items",
        "range": "",
        "description": "伪基站黑名单内容"
       }
    */
    /* 客户不可见 */
    S_Ps_Dev_NV_PseudoStationItem atItems[NV_MAX_PSEUDO_MAX_NUM];
}S_Ps_Dev_NV_PseudoStationInfo;


/* @NV-PS {
        "name": "流量数据统计",
        "description": "流量数据发送接收字节数"
   }
*/
typedef struct{
    /* 客户不可见  */
    uint32_t   dwByteSend;
    /* 客户不可见*/
    uint32_t   dwByteRecv;
} S_Ps_Dev_NV_FlowCnt;


/* @NV-PS {
        "name": "UE保存的信息",
        "description": "UE保存的网络的运营商信息、Plmn信息、EPS安全上下文、历史频点、运营商频段信息"
   }
*/
typedef struct {
    /* @NV-ITEM {
        "name": "是否支持漫游",
        "range": "0-1",
        "description": "是否支持漫游,0:不支持,1支持,默认1"
       }
    */
    /* 客户不可见 */
    uint8_t                        bRoam;     /* default: 1 enable roaming*/

    /* @NV-ITEM {
        "name": "网络配置的运营商网络信息列表",
        "range": "",
        "description": "网络配置的运营商网络信息列表,最大12个"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_OperPlmnInfo       atNvNetPlmnIDInfo[MAX_NET_OPER_NUM];/* information from network*/

    /* @NV-ITEM {
        "name": "PLMN信息",
        "range": "",
        "description": "EPLMN、RPLMN和运营商信息"
       }
    */
    /* 客户可见不可修改 */
    S_Ps_Dev_NV_PlmnInfoList       tPlmnData;/* store plmn information when UE working*/

    S_Ps_Dev_NV_EpsParam           tEpsParam;/*security context for EPS*/

    /* @NV-ITEM {
        "name": "历史频点信息",
        "range": "",
        "description": "历史频点信息"
       }
    */
    /* 客户不可见 */
    S_Ps_Dev_NV_LteFreqHistoryInfo tLteFreqHistoryInfo;/* LTE history frequency*/

    /* @NV-ITEM {
           "name": "运营商频段信息",
           "range": "",
           "description": "运营商频段信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_OperatorLteBandInfo tOperatorLteBandInfo;   /* Operator band info */

    /* @NV-ITEM {
        "name": "备份的频段是否支持信息",
        "range": "",
        "description": "默认的用于恢复频段支持信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_Lteband     tBandBackup;
}S_Ps_Dev_NV_UeSaveInfo;

/* @NV-PS {
        "name": "AT+CMSET设置的信息",
        "description": "AT+CMSET设置的信息"
   }
*/
/* 客户可见可修改 */
typedef struct {
    /* @NV-ITEM {
        "name": "异常重启开关",
        "range": "0-1",
        "description": "1:打开， 0:关闭，default:0 "
       }
    */
    /* 客户可见可修改 */
    UINT8     exceptReset;

    union {
        /* @NV-ITEM {
            "name": "LTE制式下定制信息",
            "range": "",
            "description": "LTE制式下定制信息"
        }
        */
        /* 客户可见可修改 */
        S_Ps_Dev_NV_LteFlagInfo tLteFlagInfo;
        uint8_t                 bAscfg;
    }tLteInfo;                             /* AT+CMSET="ASCFG", default:0*/

    /* @NV-ITEM {
        "name": "cat1协议版本",
        "range": "0-5",
        "description": "cat1协议版本, 0:R8,1:R9,...5:R13,默认值5"
       }
    */
    /* 客户可见可修改 */
    BITS                    bAsRel:4;        /*AT+CMSET="CAT1_RELEASE",LTE AS release info,0：R8,1：R9,2：R10,3：R11,4：R12,5：R13, default: 5(AccessStratumRelease_rel13) */

    /* @NV-ITEM {
        "name": "Usim APDU是否打印",
        "range": "0-5",
        "description": "Usim APDU是否打印, 0:关闭打印,1:开启打印,默认值0"
       }
    */
    /* 客户可见可修改 */
    BITS                    bUsimApduPrintFlg:1;     /* AT+CMSET="USIM_APDU_PRINT",0: Close Usim APDU Print  1:open Usim APDU Print,default: 0*/

    /* @NV-ITEM {
        "name": "是否进行轮询操作",
        "range": "0-1",
        "description": "是否进行轮询操作,0:不轮询,1:要轮询,默认值0,如果项目需要USAT功能，需要配置为1"
       }
    */
    /* 客户可见可修改 */
    BITS                    bPollFlag:1;     /* AT+CMSET="POLL_FLAG",uicc poll 1:open poll,0:close,default;0*/

   /* @NV-ITEM {
        "name": "是否中移入库外场测试",
        "range": "0-1",
        "description": "是否中移入库外场测试,0:非中移入库外场测试,1:中移入库外场测试场景,默认值0"
       }
    */
   /* 客户可见可修改 */
    BITS                    bCmccFieldPctFlag:1;     /* AT+CMSET="CMCC_FIELD_PCT_FLAG",0:CMCC external field Pct disabled ,1:CMCC external field Pct enabled, default;0*/
    /* @NV-ITEM {
        "name": "EL2 TRACE信息",
        "range": "",
        "description": "EL2 TRACE信息,bit0:LogOutPut,bit1:DL/UL SCH DATA IND OutPut,bit2:Rlc Status OutPut, bit3:L1E延时统计.每个bit:0:disable,1:enable,default:0"
       }
    */
    /* 客户可见可修改 */
    uint8_t                    bEl2Trace:6;   /* AT+CMSET="EL2_TRACE",bit0: LogOutPut, bit1:DL/UL SCH DATA IND OutPut, bit2:Rlc Status OutPut, bit3:L1E延时统计.0:disable，1:enable,default:0*/
    
    /* @NV-ITEM {
         "name": "是否支持数传版本任意驻留",
         "range": "0-1",
         "description": "是否支持数传版本任意驻留,0:不支持,1:支持,默认值0"
        }
     */
    /* 客户可见可修改 */
     BITS                    bMdlAnyCamp:1;     /* AT+CMSET="MDL_ANYCAMP",0: disabled ,1: enabled, default;0*/

    /* @NV-ITEM {
        "name": "UICC概要信息配置",
        "range": "",
        "description": "UICC概要信息配置,按协议配置的码流,default: 0200E842110C00009000001F0000000043400000000000404000000000000000"
       }
    */
    /* 客户可见可修改 */
    uint8_t                    abUsatTermProf[MAX_USAT_TERM_PROF_LEN];   /* AT+CMSET="TERMINAL_PROFILE",33613890,285999104,2415919135,0,1128267776,64,1073741824,0*/

    /* @NV-ITEM {
        "name": "协议栈省电场景开关配置",
        "range": "0-1",
        "description": "bit化配置: bit0 standby自主开关, 其他bit 保留"
       }
    */
    /* 客户可见不可修改 */
    uint16_t                wPsLpmCfg;  /* bit0=1 自定义的standby场景启动，否则进deepsleep */
}S_Ps_Dev_NV_CmSet;

/* @NV-PS {
        "name": "设置的开关等信息",
        "description": "设置的CEREG、CMUSLOT开关等信息"
   }
*/
typedef struct {
    /* @NV-ITEM {
        "name": "卡初始化结束状态上报开关",
        "range": "0-1",
        "description": "是否上报+CMURDY,0:关闭,1:打开,默认值1"
       }
    */
    /* 客户可见可修改 */
    BITS                    bUrdySet:1;      /* AT+CMURDY setting. 0 disable，1 enable,default:1*/

    /* @NV-ITEM {
        "name": "插拔卡上报开关",
        "range": "0-1",
        "description": "是否上报+CMUSLOT,0:关闭,1:打开,默认值1"
       }
    */
    /* 客户可见可修改 */
    BITS                    bUslotSet:1;     /* AT+CMUSLOT setting. 0 disable，1 enable,default:1*/

    /* @NV-ITEM {
        "name": "v6地址压缩格式开关",
        "range": "0-1",
        "description": "v6地址压缩格式开关,0:关闭V6地址压缩模式,1:使能V6地址压缩模式"
       }
    */
    /* 客户可见可修改 */
    BITS                    bCgpiafSet:1;    /* cgpiaf,0 disable，1 cgpiaf enable,default:0*/

    /* @NV-ITEM {
        "name": "sms over ip是否支持开关",
        "range": "0-1",
        "description": "sms over ip是否支持开关,0:关闭,1:打开,默认值0"
       }
    */
    /* 客户可见可修改 */
    BITS                    bSuppSmsOverIms:1; /* AT+CMSMSOIN ， 0 not support sms over ip, 1 support */

    /* @NV-ITEM {
        "name": "PS模式信息",
        "range": "0-3",
        "description": "PS模式信息,0:PS mode2,1:CS/PS mode1,2:CS/PS mode2,3:PS mode1,默认值:2,支持IMS时默认为3,"
       }
    */
    /* 客户可见可修改 */
    BITS                    bUeMode:2;       /* 0 PS mode 2 of operation,1 CS/PS mode 1 of operation,2  CS/PS mode 2 of operation,
                                              3  PS mode 1 of operation,default:2*/
    BITS                    bCmslotSet:2;    /* AT+CMSLOT命令设置的卡槽号 */

    /* @NV-ITEM {
        "name": "网络注册状态上报开关",
        "range": "0-2",
        "description": "网络注册状态上报开关,0:不上报,1:只上报状态,2:上报状态和小区信息,默认值0"
       }
    */
    /* 客户可见可修改 */
    BITS                    bCeregSet:2;     /* cereg,0 disable，1 regstat enable,2 CellId enable,default:0*/

    /* @NV-ITEM {
        "name": "网络注册状态上报开关",
        "range": "0-2",
        "description": "网络注册状态上报开关,0:不上报,1:只上报状态,2:上报状态和小区信息,默认值0"
       }
    */
    /* 客户可见可修改 */
    BITS                    bCgregSet:2;     /* cgreg,0 disable，1 regstat enable,2 CellId enable,default:0*/

    /* @NV-ITEM {
        "name": "网络注册状态上报开关",
        "range": "0-2",
        "description": "网络注册状态上报开关,0:不上报,1:只上报状态,2:上报状态和小区信息,默认值0"
       }
    */
    /* 客户可见可修改 */
    BITS                    bCregSet:2;     /* creg,0 disable，1 regstat enable,2 CellId enable,default:0*/

    /* @NV-ITEM {
        "name": "网络信息上报开关",
        "range": "0-3",
        "description": "上报格式,0:不上报,1:+CTZV: <tz>,2:+CTZE: <tz>,<dst>,[<time>],3:+CTZEU: <tz>,<dst>,[<utime>],默认值2"
       }
    */
    /* 客户可见可修改 */
    BITS                    bCtzrSet:2;       /* AT+CTZR value setting,0 disable，1 +CTZV: <tz>,2 +CTZE: <tz>,<dst>,[<time>],3 +CTZEU: <tz>,<dst>,[<utime>],default:2 */

}S_Ps_Dev_NV_BitSet;

/* @NV-PS {
        "name": "网络信号主动上报参数设置",
        "description": "设置定时器时间和触发上报的变化阈值"
   }
*/
typedef struct {
    /* @NV-ITEM {
        "name": "轮询时间间隔",
        "range": "0-255",
        "description": "检查信号值变化的轮询时间间隔,单位为秒,默认值5秒。"
       }
    */
    /* 客户可见可修改  */
    uint8_t                    bPollTime;

    /* @NV-ITEM {
        "name": "变化阈值",
        "range": "0-255",
        "description": "网络信号变化幅度超过阈值时上报,单位为db,默认值2db。"
       }
    */
    /* 客户可见可修改  */
    uint8_t                    bTrigRange;
}S_Ps_Dev_NV_CesqIndSet;

/* @NV-PS {
        "name": "用户设置的信息",
        "description": "用户设置的PDP上下文、搜网模式等信息"
   }
*/
typedef struct {
    /* @NV-ITEM {
           "name": "主模下服务小区的上行功率限制信息",
           "range": "",
           "description": "主模下服务小区的上行功率限制信息"
       }
    */
    /* 客户不可见 */
    S_Ps_Dev_NV_CustomTxPower     tCustomTxPower;

    /* @NV-ITEM {
        "name": "频段是否支持信息",
        "range": "",
        "description": "频段是否支持信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_Lteband     tLteband;

    /* @NV-ITEM {
        "name": "AT+CMSET设置的信息",
        "range": "",
        "description": "AT+CMSET设置的信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_CmSet       tCmSet;

    /* @NV-ITEM {
        "name": "设置的开关等信息",
        "range": "",
        "description": "设置的开关等信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_BitSet      tBitSet;

    /* @NV-ITEM {
        "name": "省电信息",
        "range": "",
        "description": "省电信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_PsmInfo     tNvPsmInfo;    /* PSM*/

    /* @NV-ITEM {
        "name": "eDRX信息",
        "range": "",
        "description": "eDRX信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_EdrxInfo    tNvEdrxInfo;   /* Edrx*/

    /* @NV-ITEM {
        "name": "短消息存储器类型",
        "range": "",
        "description": "短消息存储器类型"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_SmsMemInfo  tSmsMemInfo;   /* SmsMem*/

    /* @NV-ITEM {
        "name": "短信Cnmi命令设置信息",
        "range": "",
        "description": "用于设置某类短信到达时如何处理，存储上报、直接上报等"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_SMS_CnmiCmd tNvSmsCnmi;    /* SMS CNMI*/

    /* @NV-ITEM {
           "name": "PDCP定时器T1的时长",
           "range": "0,50-255",
           "description": "表示PDCP定时器T1的时长,单位:秒,0  表示定时器时长无效,默认0"
       }
    */
    /* 客户不可见 */
    uint8_t    bT1TimerLen; /*******************************************
                         功能：表示PDCP定时器T1的时长，单位：秒
                         取值范围：50-255, 0  表示定时器时长无效，不启动此规避功能
                         出厂值：60
                         **********************************************************/

    /* @NV-ITEM {
           "name": "搜网间隔的最大时长",
           "range": "0,10-90",
           "description": "搜网间隔的最大时长,单位:秒,0  表示定时器时长无效,默认0"
       }
    */
    /* 客户可见可修改 */
    uint8_t    bLimitNocellLen; /* 当NV值不为0且本轮搜网间隔时间大于NV值时使用NV值。 */

    /* @NV-ITEM {
        "name": "搜网模式",
        "range": "",
        "description": "搜网模式,对应+COPS命令设置的参数"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_CopsMode    tNvCopsMode;   /* configuration for network searching mode*/

    /* @NV-ITEM {
        "name": "PDP上下文信息",
        "range": "",
        "description": "PDP上下文信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_PdpCont     tPdpContext;   /* configuration for PDP context*/

    /* @NV-ITEM {
        "name": "网络信号主动上报参数设置",
        "range": "",
        "description": "设置定时器时间和触发上报的变化阈值"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_CesqIndSet    tNvCesqIndSet;   /* configuration for cesq URC*/

    /* 客户不可见 */
    S_Ps_Dev_NV_FlowCnt  tNvFlowCnt;
}S_Ps_Dev_NV_UserSetInfo;
/*ps Ro NV*/
/* @NV-PS {
        "name": "PS RO NV数据",
        "description": "PS RO NV数据"
   }
*/
typedef struct {
    /* @NV-ITEM {
        "name": "UE能力",
        "range": "",
        "description": "UE支持的BAND、安全算法等"
       }
    */
    S_Ps_Dev_NV_UeCapa         tUeCapa;

    /* @NV-ITEM {
        "name": "NV中的禁止PLMN列表",
        "range": "",
        "description": "NV中的禁止PLMN列表"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_FPlmnList      tFPlmnList;/* forbidden plmn list*/

    /* @NV-ITEM {
        "name": "ME配置的运营商网络信息列表",
        "range": "",
        "description": "ME配置的运营商网络信息列表，最大18个"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_OperPlmnInfo   atNvMePlmnIDInfo[MAX_ME_OPER_NUM];/* ME operator information*/
}S_Ps_Dev_NV_PsRo;

/*ps Rw NV*/
/* @NV-PS {
        "name": "PS RW NV数据",
        "description": "PS RW NV数据"
   }
*/
typedef struct {
    /* @NV-ITEM {
        "name": "UE保存的信息",
        "range": "",
        "description": "UE保存的网络的运营商信息、Plmn信息、EPS安全上下文、历史频点、运营商频段信息"
       }
    */
    S_Ps_Dev_NV_UeSaveInfo   tUeSaveInfo;

    /* @NV-ITEM {
        "name": "用户设置的信息",
        "range": "",
        "description": "用户设置的PDP上下文、搜网模式等信息"
       }
    */
    S_Ps_Dev_NV_UserSetInfo  tUserSetInfo;
}S_Ps_Dev_NV_PsData;

/* @NV-PS {
        "name": "PS RW 扩展 NV数据",
        "description": "PS RW 扩展 NV数据"
   }
*/
typedef struct {
    /* @NV-ITEM {
        "name": "伪基站存储信息",
        "range": "",
        "description": "伪基站存储信息"
       }
    */
    /* 客户可见可修改 */
    S_Ps_Dev_NV_PseudoStationInfo tPseudoStationInfo;
}S_Ps_Dev_NV_PsExtData;

typedef struct
{
	CHAR ImsApn[DEV_IMS_APN_MAX_LEN+1];/*apn of ims pdn:"ims"-default*/
	CHAR SsApn[DEV_IMS_APN_MAX_LEN+1];/*apn of ss pdn:""-default*/
	CHAR ConfUri[DEV_IMS_CONF_URI_MAX_LEN+1];/*conference URI:""-default*/
	UINT8 QKEmcalltoNormalcall; /*treat IMS EM call as Normal Call : 0-default,off; 1,on*/
	UINT8 ImsUriType; /*type of request-uri:1,sip-uri;0-default,tel-uri*/
	UINT32 ImsRegExpires; /*Value of Re-register period:default 600000s*/
	UINT8 QKMtNetTest;/*MT TEST,QK protocal conformance switch: 0-default,off; 1,on*/
	UINT8 NsIotTest;/*NS_IOT test switch:0-default,off; 1,on*/
	UINT8 QKCvmod;/*call mode :0,CS_ONLY; 1,VOIP_ONLY; 2,CS first;3-default,VOIP first*/
	UINT8 QKAmrWbFlg;/*support of AMR-WB: 0,not support;1-default,support*/
	UINT8 QKEmCallSupport;/*support of IMS EM CALL: 0-default,not support;1,support*/
	UINT8 NetImsCap; /*use the IMS capa from IMS CN: 0-default, use; 1,not use*/
	UINT16 UdpThresh;/*threshold of MTU with UDP (against TCP): Max value 65535;default 1300; 0 for udp only*/
	UINT8 IpsecDisable;/*IpsecDisable or not :0-default, ipsec enable;1: ipsec disable;*/
	UINT8 PreconditionNotSupport;/*precondition not supported or not: 0-default,support; 1,precondition not support;*/
    CHAR MCCMNCsPreconditionNotSupport[DEV_MCCMNC_ITEM_MAX_COUNT][DEV_MCCMNC_MAX_LEN];/*MCCMNCs without precondition support*/
	UINT32 WaitRtpTime;/*time util rtp packets received in early-media period:0-default*/
	UINT8 CwaOpt;/*option of Ccwa :0-Local option;1: Network option;*/
	UINT8 LocalCwaSet;/*Local Ccwa set:0-Disabled;1: Actived;*/
	UINT16 SsDeactTimer;/*Timer Value to deactive SS PDN after SS finish;*/
}S_IMS_NV_Info;

#endif


