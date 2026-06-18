/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_trim.h
 *
 * @brief       TRIM驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team         创建
 ************************************************************************************
 */

#ifndef _DRV_TRIM_H
#define _DRV_TRIM_H

#include <drv_efuse.h>
#include <iram_layout.h>

/**
 * @addtogroup Trim
 */

/**@{*/

/*************************************************************************
*                                  Include files                         *
*************************************************************************/

/*************************************************************************
*                                  Macro                                 *
*************************************************************************/

#define TRIM_AON_SYS_CTRL  (0xF2809800)
#define TRIM_SBY_LSP_REG   (0xF0000000)
#define TRIM_RF_CTRL_BASE  (0xF2809000)

#define TRIM_EFUSE_WORD_WIDTH (32)

#define TRIM_MASK(x) ((0x1 << (x)) - 1)
#define REG_VAL_GET_FROM_EFUSE(b, a) \
    (uint16_t)(a.trimInfo.trimWidth <= (TRIM_EFUSE_WORD_WIDTH - a.trimInfo.trimOffs % TRIM_EFUSE_WORD_WIDTH) ? \
    ((b[a.trimInfo.trimOffs / TRIM_EFUSE_WORD_WIDTH] >> (a.trimInfo.trimOffs % TRIM_EFUSE_WORD_WIDTH)) & (TRIM_MASK(a.trimInfo.trimWidth))) : \
    ((b[a.trimInfo.trimOffs / TRIM_EFUSE_WORD_WIDTH] >> (a.trimInfo.trimOffs % TRIM_EFUSE_WORD_WIDTH)) & (TRIM_MASK(TRIM_EFUSE_WORD_WIDTH - a.trimInfo.trimOffs % TRIM_EFUSE_WORD_WIDTH))) | \
    ((b[a.trimInfo.trimOffs / TRIM_EFUSE_WORD_WIDTH + 1] & (TRIM_MASK(a.trimInfo.trimWidth + a.trimInfo.trimOffs % TRIM_EFUSE_WORD_WIDTH - TRIM_EFUSE_WORD_WIDTH))) << (TRIM_EFUSE_WORD_WIDTH - a.trimInfo.trimOffs % TRIM_EFUSE_WORD_WIDTH)))

#define TRIM_EFUSE_DIEID_LEN (8)
/**************************************************************************
*                                  Types                                  *
**************************************************************************/

typedef struct
{
    uint8_t trimOffs;
    uint8_t trimWidth;
} T_TrimEfuse;

typedef struct
{
    uint8_t regOffs;
    uint8_t regWidth;
} T_TrimRegs;

typedef struct
{
    T_TrimEfuse   trimInfo;
    T_TrimRegs    trimReg;
    uint32_t      reg;
    int8_t        cali;
} T_TrimSet;

typedef struct
{
    uint16_t dig_aux_adc0_sbgr_045;
    uint16_t dig_aux_adc0_sbgr_135;
    uint16_t dig_aux_adc3_sbgr_16;
    uint16_t dig_aux_adc3_sbgr_08;
    uint16_t adc_cali;
} T_TrimAdc;

/**************************************************************************
*                           Function Prototypes                           *
**************************************************************************/

/*******************************************************************************
* Function: Trim_GetDieID
* Description: get DieID from trim
* Parameters:
*   Input:
*
*   Output:
*
* Returns:
*
*
* Others:
********************************************************************************/
void *Trim_GetDieID(uint32_t *len);

#endif/*_DRV_TRIM_H*/

/** @} */