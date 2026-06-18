/****************************************************************************************************
 * Copyright (c) 2023, 芯昇科技有限公司
 * 
 * @file        sys_func_atcfg.h
 *
 * @brief       This file contains atcfg process
 *
 * @revision
 * Date                   Author            Notes
 * 2023-4-20
*****************************************************************************************************/

#ifndef     SYSCONFIG_FNC_ATCFG_H
#define     SYSCONFIG_FNC_ATCFG_H

#include "ps_phy_nv.h"
#define USE_TECT_TEST_ENABLE   1
#define USE_TECT_TEST_DISABLE  0


/* CAT1    REL */
#define USE_PS_LTE_RELEASE_R8  0
#define USE_PS_LTE_RELEASE_R9  1
#define USE_PS_LTE_RELEASE_R10  2
#define USE_PS_LTE_RELEASE_R11  3
#define USE_PS_LTE_RELEASE_R12  4
#define USE_PS_LTE_RELEASE_R13  5


typedef struct  
{
        uint32_t  param1; 
        uint32_t  param2;
        uint32_t  param3;
        uint32_t  param4;
        uint32_t  param5;
        uint32_t  param6;
        uint32_t  param7;
        uint32_t  param8;
        uint32_t  flag1;
        uint32_t  flag2;
        uint32_t  flag3;
        uint32_t  flag4;
        uint32_t  flag5;
        uint32_t  flag6;
        uint32_t  flag7;
        uint32_t  flag8;
        uint32_t  result;
}CommAtCfgCmdParam;

typedef enum
{
    RESET_NORMAL = 0,
    RESET_CHARGING,
    RESET_ALRAM,
    RESET_EXCEPTRESET,
    RESET_FOTA,
    RESET_RTC,
    RESET_RESET,
    RESET_HDT_TEST,
    RESET_LOCALUPDATE,
    RESET_BOOST_IN,
    RESET_AMT,
    RESET_PRODUCTION,
    RESET_WDT,
    RESET_STANDBY,
    RESET_INVALID,
}T_Drv_ResetReason_Type;

CommAtCfgCmdParam oss_AtCfgSet(char* cmd,CommAtCfgCmdParam * cmdParm);
CommAtCfgCmdParam oss_AtCfgGet(char* cmd);
uint32_t oss_SetTectTestFlag(uint32_t flag);
uint32_t oss_GetTectTestFlag (void);
uint32_t oss_SetphyCfgFlg(uint32_t wMeasPchCfgFlg);
uint16_t oss_GetphyCfgFlg(void);
uint8_t ATCFG_GetImeiSv(void);
uint32_t atcfg_SetLteLcCfg(S_Ps_P_NV_LteLcCfg *ptLteLcCfg);
const S_Ps_P_NV_LteLcCfg* atcfg_GetLteLcCfg(void);
uint8_t atcfg_GetStationaryMeterCtrl(void);
uint32_t atcfg_LteSetSValueOffset(uint16_t param1, uint16_t param2, uint16_t flag1, uint16_t flag2);
uint8_t atcfg_LteGetSValueOffset(uint8_t bCellFlg);
uint8_t atcfg_GetWeakFieldEndhanceParam(void);
uint32_t oss_GetRfComplexFlag (void);
uint8_t oss_GetTemperatureProtectEnable(void);
#endif

