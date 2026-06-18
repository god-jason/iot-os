/****************************************************************************************************
 * Copyright (c) 2023, 芯昇科技有限公司
 * 
 * @file        ps_database.h
 *
 * @brief       ps database
 *
 * @revision
 * Date                   Author            Notes
 * 2023-4-20
*****************************************************************************************************/

#ifndef DEV_DATA_H
#define DEV_DATA_H


#include "nvparam_ps.h"

#define DEV_NV_AMTLTEBAND_NUM                            (uint8_t)16

typedef enum
{
    NvId_Roamconfig,
    NvId_NetOperPlmnID,
    NvId_PlmnInfo,
    NvId_EpsParam,
    NvId_LteFreqHistoryInfo,
    NvId_OperatorLteBandInfo,
    NVID_BANDBACKUP,
    NvId_LTECustomTxPower,
    NVID_LTEBAND,
    NVID_CMSET,
    NVID_BITSET,
    NvId_PsmInfo,
    NvId_EdrxInfo,
    NvId_SmsMemInfo,
    NvId_SmsCnmi,
    NVID_PDCPTIMERLEN,
    NVID_LIMIITNOCELLLEN,
    NvId_CopsMode,
    NvId_CidPdpContext,
    NvId_CesqIndSet,
    NvId_FlowCnt,
    NvId_PrvEnd,

    NVID_RO_START,
    NvId_UeCapa,
    NvId_FplmnList,
    NvId_MeOperPlmnID,
    NVID_RO_END
}EN_Nv_Id;

#define DEV_MAX_NVITEM_NUM                              (uint8_t)(NvId_PrvEnd)


typedef struct {
    EN_Nv_Id        enNvId;
    uint16_t        wItemOffset;
    uint16_t        wItemSize;
    uint8_t         *pData;
}S_Ps_NV_ItemInfo;

typedef struct
{
    const char  *pchCmd;
    EN_Nv_Id    enNvId;
    uint8_t     bMin;
    uint8_t     bMax;
}S_Ps_NV_Atcfg_Cmd;

/**
 * @brief PS NV information struct
 */
typedef struct {
    S_Ps_Dev_NV_PsData    *ptPsData;
    uint32_t              dwPsNvPriOffset;
    uint16_t              awPtrOffset[DEV_MAX_NVITEM_NUM];
    uint16_t              awDataSize[DEV_MAX_NVITEM_NUM];
    S_Ps_Dev_NV_UeCapa    *ptRoUeCapa;/*UeCapa rrc会频繁读取,上电就保存*/
    S_Ps_Dev_NV_PsExtData *ptPsExtData;/*如果NV_PsData里的nv不够用再用这个nv*/
}S_Dev_NVInfo;


uint32_t dev_NVInit(void);
uint8_t dev_NVReadRoItem(uint8_t bNvItem, uint8_t *pbData);
void dev_NVReadItem(uint8_t bNvItem, void *pItemOut);
uint8_t dev_NVWriteItem(uint8_t bNvItem, void *pItemIn);
void dev_NVFlush(void);
uint8_t dev_IsTECT(void);
uint8_t dev_GetEl2TraceFlg(void);
uint8_t dev_LteGetSValueOffset(int8_t *pSCellSValueOffset, int8_t *pNCellSValueOffset);
void dev_GetAmtNvAndPsNvLteBand(uint8_t *pAmtnvBand);
int32_t dev_NvGetVersion(char * pVersion);
void dev_NVReadPsExt(void *pItemOut);
uint8_t dev_NVWritePsExt(void *pItemIn, uint8_t bSync);

#endif

