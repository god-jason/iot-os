/**
 * @file        cm_demo_adc.c
 * @brief       OpenCPU adc测试例程
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 */
 

 /****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdint.h>
#include "cm_adc.h"
#include "cm_os.h"
#include "cm_demo_uart.h"
#include "cm_demo_adc.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define CM_DEMO_ADC_LOG     cm_demo_printf
#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)

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
void cm_test_adc(EmbeddedCli *cli, char *args, void *context)
{
    int32_t voltage=0;
    uint32_t voltage_vbat=0;
    int32_t ret;
    int32_t temperature = 0;
    
    CM_DEMO_ADC_LOG("adc test start!!\n");
    
    ret = cm_adc_vbat_read(&voltage_vbat);
    if(ret != RET_SUCCESS)
    {
        CM_DEMO_ADC_LOG("adc vbate read err,ret=%d\n", ret);
        return;
    }
    CM_DEMO_ADC_LOG("adc vbate read:%ld(mv)!!\n",voltage_vbat);

    //测试ADC0
    ret = cm_adc_read(CM_ADC_0,&voltage);
    if(ret != RET_SUCCESS)
    {
        CM_DEMO_ADC_LOG("adcCM_ADC_0 read err,ret=0x%08x\n", ret);
        return;
    }
    CM_DEMO_ADC_LOG("adc CM_ADC_0 read:%ld(mv)!!\n",voltage);
    CM_DEMO_ADC_LOG("adc test end!!\n");

    ret = cm_adc_read(CM_ADC_1,&voltage);
    if(ret != RET_SUCCESS)
    {
        CM_DEMO_ADC_LOG("adcCM_ADC_1 read err,ret=0x%08x\n", ret);
        return;
    }
    CM_DEMO_ADC_LOG("adc CM_ADC_1 read:%ld(mv)!!\n",voltage);

    //温度获取测试 
    ret = cm_adc_temperature_read(&temperature);
    if(ret != RET_SUCCESS)
    {
        CM_DEMO_ADC_LOG("cm_adc_temperature_read err,ret=0x%08x\n", ret);
        return;
    }
    CM_DEMO_ADC_LOG("cm_adc_temperature_read:%ld!!\n",temperature);
    
    CM_DEMO_ADC_LOG("adc test end!!\n");
}
