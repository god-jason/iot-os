/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_psram.h
 *
 * @brief       PSRAM模块头文件
 *
 * @revision
 *
 * 日期             作者                修改内容
 * 2023-05-11     ICT Team          创建
 ************************************************************************************
 */


#ifndef _DRV_PSRAM_H
#define _DRV_PSRAM_H

#if defined(OS_USING_PM)
#include "psm_common.h"
#else
#undef  SECTION_IRAM_FUNC
#define SECTION_IRAM_FUNC
#endif

#if defined(OS_USING_PM)
void SECTION_IRAM_FUNC PSRAM_EnterHalfSleep(void);
void SECTION_IRAM_FUNC PSRAM_ExitHalfSleep(void);
void SECTION_IRAM_FUNC PSRAM_Resume(void);
void SECTION_IRAM_FUNC PSRAM_Suspend(void);
#endif

#ifdef _CPU_AP
int SECTION_IRAM_FUNC PSRAM_SetCurrentFreq(int freq);
#endif

#endif // _DRV_PSRAM_H
