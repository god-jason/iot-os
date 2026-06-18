/**
 * @file        cm_demo_adc.c
 * @brief       OpenCPU adc测试例程
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 */


 /****************************************************************************
 * Included Files
 ****************************************************************************/
#ifdef CM_DEMO_ADC_SUPPORT
#ifdef OS_USING_SHELL

#include <os.h>
#include <stdlib.h>
#include <string.h>
#include <nr_micro_shell.h>
#include "cm_adc.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)
#define cm_demo_printf osPrintf
#define RET_SUCCESS               0
#define CM_DEMO_ADC_LOG     cm_demo_printf

/****************************************************************************
 * Private Types
 ****************************************************************************/


 
/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
 
 
/****************************************************************************
 * Private Data
 ****************************************************************************/


/****************************************************************************
 * Private Functions
 ****************************************************************************/


/****************************************************************************
 * Public Functions
 ****************************************************************************/
/**
 * @brief  adc接口函数测试示例
 *
 * @return  void
 *
 * @details NONE
 */
void cm_test_adc(int argc, char **argv)
{
    int32_t voltage=0;
    uint32_t voltage_vbat=0;
    int32_t ret;
    int32_t temperature = 0;
    
    CM_DEMO_ADC_LOG("adc test start!!\r\n");
    
    ret = cm_adc_vbat_read(&voltage_vbat);
    if(ret != RET_SUCCESS)
    {
        CM_DEMO_ADC_LOG("adc vbate read err,ret=%d\r\n", ret);
        return;
    }
    CM_DEMO_ADC_LOG("adc vbate read:%ld(mv)!!\r\n",voltage_vbat);

    //测试ADC0
    ret = cm_adc_read(CM_ADC_0,&voltage);
    if(ret != RET_SUCCESS)
    {
        CM_DEMO_ADC_LOG("adcCM_ADC_0 read err,ret=0x%08x\r\n", ret);
        return;
    }
    CM_DEMO_ADC_LOG("adc CM_ADC_0 read:%ld(mv)!!\r\n",voltage);
    CM_DEMO_ADC_LOG("adc test end!!\r\n");

    ret = cm_adc_read(CM_ADC_1,&voltage);
    if(ret != RET_SUCCESS)
    {
        CM_DEMO_ADC_LOG("adcCM_ADC_1 read err,ret=0x%08x\r\n", ret);
        return;
    }
    CM_DEMO_ADC_LOG("adc CM_ADC_1 read:%ld(mv)!!\r\n",voltage);

    //温度获取测试 
    ret = cm_adc_temperature_read(&temperature);
    if(ret != RET_SUCCESS)
    {
        CM_DEMO_ADC_LOG("cm_adc_temperature_read err,ret=0x%08x\r\n", ret);
        return;
    }
    CM_DEMO_ADC_LOG("cm_adc_temperature_read:%ld!!\r\n",temperature);
    
    CM_DEMO_ADC_LOG("adc test end!!\r\n");
}

NR_SHELL_CMD_EXPORT(adc, cm_test_adc);
#endif
#endif
