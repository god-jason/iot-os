/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_soc.h
 *
 * @brief       SOC集成功能接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */


#ifndef _DRV_SOC_H
#define _DRV_SOC_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include "driver/src/soc/drv_clk.h"
#include "driver/src/soc/drv_regulator.h"
#include "driver/src/soc/drv_sysctrl.h"

#include <iram_layout.h>

#define Is_EcoChip      (*(volatile unsigned int *)(IRAM_BASE_ADDR_ECO_VERSION))
#endif  // _DRV_SOC_H
