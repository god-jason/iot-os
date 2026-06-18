
#ifndef _PS_USIM_H_
#define _PS_USIM_H_


#include <stdint.h>
#include "ps.h"
#include "l3cdec_header.h"
#include "usat_cdec.h"

/*******************File ID *******************/
#define USIM_FID_DIR        0x2F00
#define USIM_FID_PL         0x2F05
#define USIM_FID_ICCID      0x2FE2
#define USIM_FID_IMSI       0x6F07
#define USIM_FID_HPPLMN     0x6F31
#define USIM_FID_UST        0x6F38
#define USIM_FID_SMS        0x6F3C
#define USIM_FID_SMSP       0x6F42
#define USIM_FID_SMSS       0x6F43
#define USIM_FID_SMSR       0x6F47
#define USIM_FID_EST        0x6F56
#define USIM_FID_ACL        0x6F57
#define USIM_FID_UPLMN      0x6F60
#define USIM_FID_OPLMN      0x6F61
#define USIM_FID_HPLMNWACT  0x6F62
#define USIM_FID_ACC        0x6F78
#define USIM_FID_FPLMN      0x6F7B
#define USIM_FID_LOCI       0x6F7E
#define USIM_FID_AD         0x6FAD
#define USIM_FID_EHPLMN     0x6FD9
#define USIM_FID_LRPLMNSI   0x6FDC
#define USIM_FID_EPSLOCI    0x6FE3
#define USIM_FID_EPSNSC     0x6FE4
#define USIM_FID_NASCONFIG  0x6FE8
#define USIM_FID_MSISDN     0x6F40
#define USIM_FID_EXT5       0x6F4E

#ifdef IMS_SUPPORT
#define USIM_FID_IMPI       0x6F02
#define USIM_FID_DOMAIN     0x6F03
#define USIM_FID_IMPU       0x6F04
#define USIM_FID_ECC        0x6FB7
#endif
/*********ucUsimValidFlg*******************/
#define USIM_DATA_INVALID           0
#define USIM_DATA_VALID             1
#define USIM_DATA_INVALID_TMEP      2

#define NAS_MAX_PIN_LEN             8
#define NAS_MAX_K_ASME_LEN          32

#define USIM_MAX_SMS_REC_NUM        50
#define USIM_MAX_SMSR_REC_NUM       20
#define USIM_MAX_SMS_REC_REMAIN_LEN 175

#define NAS_MAX_FPLMN_NUM    85
#define NAS_MAX_EHPLMN_NUM   16

/*S_Usim_EPSLOCI->ucEpsUpdateStat*/
#define NAS_EPS_UPDATE_STAT_UPDATED         0
#define NAS_EPS_UPDATE_STAT_NOT_UPDATED     1
#define NAS_EPS_UPDATE_STAT_ROAM_NOT_ALLOW  2

#define USIM_SMSR_MAX_LEN 30
#define USIM_SMS_MAX_LEN  176
#define USIM_LOCI_LEN     11
#define USIM_IMSI_LEN     9
#define USIM_EPSLOCI_LEN  18
#define USIM_EPSNSC_LEN   54

#define USIM_ACL_MAX_APN_NUM 10
#define USIM_MAX_APN_LEN 100

#define USIM_MAX_CALL_BCD_NUM   10

typedef struct
{
  char abApn[USIM_MAX_APN_LEN + 1];
}S_Usim_Apn;

typedef struct
{
  uint8_t         bApnNum;
  S_Usim_Apn   *ptApns[USIM_ACL_MAX_APN_NUM];
}S_Usim_Acl_Struct;

typedef struct {
    uint8_t                        bTpmr;
    uint8_t                        bMemCapFlag;
}S_Usim_SMSS;

typedef struct {
    S_L3_EPSmobidGUTI              tGuti;
    S_PS_TAI                       tTai;
    uint8_t                        bEpsUpdateStat;
}S_Usim_EPSLOCI;

typedef struct {
    uint8_t                        abTmsi[4];
    S_LAI                          tLai;
    uint8_t                        bRfu;
    uint8_t                        bLuSta;
}S_Usim_LOCI;


typedef struct {
    uint8_t                        bNasSn;
    uint16_t                       wNasOverflowCnt;
} S_Usim_NasCount;

typedef struct {
    uint8_t                        b3NasCipherAlg                 : 3;
    uint8_t                        b1Spare1                       : 1;
    uint8_t                        b3NasIntegerAlg                : 3;
    uint8_t                        b1Spare2                       : 1;
} S_Usim_IntEncAlgId;

typedef struct {
    uint8_t                        bKsiAsmeFlg                     : 1;
    uint8_t                        bKAsmeFlg                       : 1;
    uint8_t                        bUpNasCntFlg                    : 1;
    uint8_t                        bDlNasCntFlg                    : 1;
    uint8_t                        bNasIntEncAlgIdFlg              : 1;
    uint8_t                        bKsiAsme                        : 3;
    uint8_t                        abKasme[NAS_MAX_K_ASME_LEN];
    S_Usim_NasCount                tNasUlCount;
    S_Usim_NasCount                tNasDlCount;
    S_Usim_IntEncAlgId             tNasIntEncAlgId;
}S_Usim_EPSNSC;

typedef struct {
    uint8_t                        bMaxCnt;
    uint8_t                        bValidCnt;
    S_PS_PlmnId                    atFplmn[NAS_MAX_FPLMN_NUM];
} S_Usim_FPLMN;

typedef struct {
    uint8_t                        bMaxCnt;   /* 卡中最大能存的PLMN个数, 不可修改*/
    uint8_t                        bValidCnt; /* 卡中目前实际有效的PLMN个数 */
    S_PS_PlmnId                    *atPlmnId;
} S_Usim_Plmn_List;

typedef struct {
    uint8_t                        bMaxCnt;   /* 卡中最大能存的PLMN个数, 不可修改, 0表示卡里没有UPLMN */
    uint8_t                        bValidCnt; /* 卡中目前实际有效的PLMN个数 */
    S_Usim_Plmn_Act                *atPlmnAct;
} S_Usim_Plmn_Act_List;

typedef struct {
    uint8_t                        bMaxCnt;   /* 卡中最大能存的EPLMN个数, 不可修改, 0表示卡里没有EHPLMN */
    uint8_t                        bValidCnt; /* 卡中目前实际有效的EPLMN个数 */
    S_PS_PlmnId                    atEhplmn[NAS_MAX_EHPLMN_NUM];
} S_Usim_EHPLMN;

typedef struct {
    uint8_t   bMinPeriosearchTimerFlg    : 1;
    uint8_t   b7Spare                    : 7;

    uint8_t   bMinPeriosearchTimer;
} S_Usim_NASCONFIG;

typedef struct {
    uint8_t                        bAlphaLen;
    uint8_t                        bParamId;
    S_Usim_SCA                     tDstAddr;
    S_Usim_SCA                     tSca;
    uint8_t                        bTPPid;
    uint8_t                        bTPDcs;
    uint8_t                        bTPVp;
} S_Usim_SMSP;

typedef struct {
    uint8_t                        bRecId;
    uint8_t                        bStatus;
    uint8_t                        abRemainder[USIM_MAX_SMS_REC_REMAIN_LEN];
} S_Usim_SmsRecItem;

typedef struct {
    uint8_t                        bStat;
    uint8_t                        bMti;
    uint8_t                        bTpmr;
} S_Usim_SmsRecInfo;

typedef struct {
    uint8_t                        bRecId;
    uint8_t                        bTpmr;
    //uint8_t                           bStat;
} S_Usim_SmsrRecInfo;

typedef struct {
    uint8_t                        bTotalRecNum;   /* SMS 总记录数 */
    uint8_t                        bUsedRecNum;    /* 已使用的 SMS 记录数 */
    uint8_t                        bFileSize;      /* SMS文件大小 */
    S_Usim_SmsRecInfo              *ptRecInfo;     /* 根据记录数申请空间，记录对应记录的关键信息 */
} S_Usim_SMS;

typedef struct {
    uint8_t                        bTotalRecNum;   /* SMSR 总记录数 */
    uint8_t                        bUsedRecNum;    /* 已使用的 SMS 记录数 */
    uint8_t                        bFileSize;      /* SMSR文件大小 */
    S_Usim_SmsrRecInfo             *ptRecInfo;     /* 根据记录数申请空间，记录对应记录的关键信息 */
} S_Usim_SMSR;


typedef struct {
    uint8_t                        b1SmsPpDownload              : 1; /* 是否支持 SMS PP download */
    uint8_t                        b1EhplmnSpt                  : 1; /* 是否支持 EHPLMN */
    uint8_t                        b7Spare                      : 6;
} S_Usim_UST;

typedef struct {
    uint8_t                        b1AclSupport                 : 1; /* 是否支持 ACL */
    uint8_t                        b7Spare                      : 7;
}S_Usim_EST;

typedef struct
{
    uint8_t                        bLen;
    uint8_t                        *pbApnList;
}S_Usim_ACL;

typedef struct
{
    uint8_t                        bLen;
    uint8_t                        abApnList[USIM_MAX_APDU_LEN];
}S_Usim_ACL_ARRAY;

typedef struct {
    uint8_t                        bTotalRecNum;   /* SMS 总记录数 */
    uint8_t                        bUsedRecNum;    /* 已使用的 SMS 记录数 */
    S_Usim_SmsRecInfo              atSmsInfo[USIM_MAX_SMS_REC_NUM];
} S_Usim_SMS_ARRAY;

typedef struct {
    uint8_t                        bTotalRecNum;   /* SMSR 总记录数 */
    uint8_t                        bUsedRecNum;    /* 已使用的 SMSR 记录数 */
    S_Usim_SmsrRecInfo             atSmsrInfo[USIM_MAX_SMSR_REC_NUM];
} S_Usim_SMSR_ARRAY;


typedef struct {
    uint8_t                        bLen;
    uint8_t                        bNumType;
    uint8_t                        abNum[USIM_MAX_CALL_BCD_NUM];
    uint8_t                        bExtRecId;
}S_Usim_Msisdn;

typedef struct {
    uint8_t                        bRecType;
    uint8_t                        bNumLen;
    uint8_t                        abNum[USIM_MAX_CALL_BCD_NUM];
    uint8_t                        bExtRecId;
}S_Usim_Ext;

#define USIM_SMS_STAT_NOT_INIT    0
#define USIM_SMS_STAT_BASE_INIT   1 //SMS 基础文件SMSS SMSP已初始化，SMS，SMSR属性已获取，但记录文件尚未初始化
#define USIM_SMS_STAT_REC_INITING 2 //SMS SMS记录文件初始化过程中
#define USIM_SMS_STAT_READY       3
#define USIM_SMS_STAT_INIT_FAIL   4
#define USIM_SMS_STAT_LOCKED      5

#define USIM_EPSLOCI_BIT_POS     0
#define USIM_EPSNSC_BIT_POS      1
#define USIM_FPLMN_BIT_POS       2
#define USIM_LOCI_BIT_POS        3
#define USIM_SMSS_BIT_POS        4
#define USIM_SMSP_BIT_POS        5
#define USIM_UPLMN_BIT_POS       6
#define USIM_UPDATE_FILE_CNT     7

#define USIM_EF_INIT_STAGE_NULL  0 //第一组文件还没有初始化完
#define USIM_EF_INIT_STAGE_1     1 //第一组文件初始化完成，IMSI, EPSLOCI, EPSNSC, ACC, AD, LRPLMNSI, EST, ACL 可用
#define USIM_EF_INIT_STAGE_2     2 //第二组文件初始化完成，EHPLMN, UPLMN, OPLMN, FPLMN 可用
#define USIM_EF_INIT_STAGE_3     3 //第三组文件初始化完成，HPPLMN, UST, NASCONFIG 可用

typedef struct {
    uint32_t                   wEfInitStat        :18;/* 初始化过程中对应bit位的文件是否已经初始化 */
    uint32_t                   bIsSmsUnderDfTel   :1; /* SMS是否位于 DFtelecom */
    uint32_t                   bIsSmssUnderDfTel  :1; /* SMSS是否位于 DFtelecom */
    uint32_t                   bIsSmspUnderDfTel  :1; /* SMSP是否位于 DFtelecom */
    uint32_t                   bIsSmsrUnderDfTel  :1; /* SMSR是否位于 DFtelecom */
    uint32_t                   bIsMsisdnUnderDfTel:1; /* MSISDN是否位于 DFtelecom */
    uint32_t                   bIsExt5UnderDfTel  :1; /* EXT5是否位于 DFtelecom */
    uint32_t                   bUsimValidFlg      :2; /* 当前USIM数据是否可用：USIM_DATA_INVALID / USIM_DATA_VALID / USIM_DATA_INVALID_TMEP */
    uint32_t                   bPsEfInitStage     :4; /* 表示PS相关文件的初始化阶段 */
    uint32_t                   bPad               :2;
    uint8_t                    bImsiInstr;    /* 1 表示IMSI是仪表卡instrument */
    uint8_t                    bSmsStat;      /* 短信相关文件初始化状态 */

    /*以下数据不可写*/
    uint8_t                    bMncLen;       /* 从EFad获取，MNC的长度,取值2或3，默认值2 */
    uint8_t                    bHpplmn;       /* 默认值:    0xFF */
    uint8_t                    bLrlplmnsi;    /* 默认值: 0x00 */
    S_Usim_UST                 tUst;          /* 只记录支持的功能 */
    S_Usim_EST                 tEst;          /* 只记录支持的功能 */
    uint8_t                    abAcc[2];      /* 默认值: {0xf8,0x00} */
    uint8_t                    abIccid[ATI_NAS_MAX_ICCID_LEN];/* 默认值全FF */
    S_L3_E_MobileIdentity      tImsi;
    S_Usim_Plmn_List           tEhplmn;       /* 最大支持16个EHPLMN */
    S_Usim_Plmn_Act_List       tOplmn;        /* 最大支持30个OPLMNwact */
    S_Usim_NASCONFIG           tNasconfig;    /* 默认值全0 */
    S_Usim_ACL                 tAcl;          /* apn个数和长度均不定，保存apdu原始码流，需要使用时解析 */

    /*以下数据可写*/

    /* 下列MASK用于：
       1. 判断wEfExistFlg中对应的EF文件在USIM卡中是否存在（如果卡里不存在EPSLOCI或EPSNSC，则由本模块
          操作NV中的数据，对于外部模块EPSLOCI或EPSNSC这两个文件始终可用，不需要判断wEfExistFlg）；
       2. 判断wEfUpdateFlg中对应的EF文件是否被更新 */
#define USIM_EF_EPSLOCI_MSK  (1 << USIM_EPSLOCI_BIT_POS)
#define USIM_EF_EPSNSC_MSK   (1 << USIM_EPSNSC_BIT_POS)
#define USIM_EF_FPLMN_MSK    (1 << USIM_FPLMN_BIT_POS)
#define USIM_EF_UPLMN_MSK    (1 << USIM_UPLMN_BIT_POS)
#define USIM_EF_LOCI_MSK     (1 << USIM_LOCI_BIT_POS)
#define USIM_EF_SMSS_MSK     (1 << USIM_SMSS_BIT_POS)
#define USIM_EF_SMSP_MSK     (1 << USIM_SMSP_BIT_POS)

    uint16_t                   wEfExistFlg;    /* 根据BIT位记录对应可写文件在USIM卡中是否存在*/
    uint16_t                   wEfUpdateFlg;   /* 根据BIT位记录对应可写文件是否被更新过 */
    S_Usim_EPSLOCI             tEpsloci;       /* 默认值全FF */
    S_Usim_EPSNSC              tEpsNsc;        /* 默认值全FF */
    S_Usim_Plmn_List           tFplmn;         /* 最大支持85个FPLMN */
    S_Usim_Plmn_Act_List       tUplmnAct;      /* 最大支持30个UPLMN */
    S_Usim_LOCI                tLoci;          /* 联合附着时使用 */
    S_Usim_SMSS                tSmss;          /* 默认值   0x00,0xFF */
    S_Usim_SMSP                tSmsp;          /* 默认值全FF */
    S_Usim_SMS                 tSms;           /* 50条短信需占用约9K字节，暂定不缓存SMS内容 */
    S_Usim_SMSR                tSmsr;
}S_Usim_Data_Buf;//只保存目前2100使用的文件

extern uint8_t dev_ReadUsimItem(uint16_t wFid, uint8_t *pbData);

#endif

