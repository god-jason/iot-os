/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_ADC_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>
#include "cm_adc.h"
#define RET_SUCCESS               0

#define cm_demo_printf osPrintf

int cm_test_adc(char argc, char **argv)
{
    int32_t voltage=0;
    uint32_t voltage_vbat=0;
    int32_t ret;

    cm_demo_printf("adc test start!!\n");

    ret = cm_adc_vbat_read(&voltage_vbat);
    if(ret != RET_SUCCESS)
    {
        cm_demo_printf("adc vbate read err,ret=%d\n", ret);
        return -1;
    }
    cm_demo_printf("adc vbate read:%ld(mv)!!\n",voltage_vbat);

    //测试ADC0
    ret = cm_adc_read(CM_ADC_0,&voltage);
    if(ret != RET_SUCCESS)
    {
        cm_demo_printf("adcCM_ADC_0 read err,ret=0x%08x\n", ret);
        return -1;
    }
    cm_demo_printf("adc CM_ADC_0 read:%ld(mv)!!\n",voltage);
    //测试ADC1
    ret = cm_adc_read(CM_ADC_1,&voltage);
    if(ret != RET_SUCCESS)
    {
        cm_demo_printf("adcCM_ADC_1 read err,ret=0x%08x\n", ret);
        return -1;
    }
    cm_demo_printf("adc CM_ADC_1 read:%ld(mv)!!\n",voltage);
    cm_demo_printf("adc test end!!\n");

    return 0;
}




NR_SHELL_CMD_EXPORT(cm_adc, cm_test_adc);
#endif
#endif

