#include "stdio.h"
#include "string.h"

#include "yopen_debug.h"
#include "yopen_adc.h"
#include "yopen_dev.h"


#define DEMO_ADC_TRACE(fmt, ...) yopen_trace("[ADC] "fmt, ##__VA_ARGS__)

void yopen_adc_demo_task(void * arg)
{
    yopen_errcode_adc_e ret;
    int adc_value;
    int count = 100;

    DEMO_ADC_TRACE("========== ADC demo start ==========");

    while(count--)
    {
        //由于aio电气特性, adc输入电压不要超过3.6V，防止芯片损坏

        //量程0-1.2v
        ret = yopen_adc_get_volt(YOPEN_ADC0_CHANNEL, &adc_value); //adc_value 0-1200mV
        DEMO_ADC_TRACE("========== YOPEN_ADC0_CHANNEL ret %d adc_value %d ==========", ret, adc_value);

        yopen_adc_get_volt(YOPEN_ADC1_CHANNEL, &adc_value); //adc_value 0-1200mV
        DEMO_ADC_TRACE("========== YOPEN_ADC1_CHANNEL ret %d adc_value %d ==========", ret, adc_value);

        yopen_adc_get_volt(YOPEN_ADC_TEM_CHANNEL, &adc_value);
        DEMO_ADC_TRACE("========== YOPEN_ADC_THERMAL_CHANNEL ret %d adc_value %d ==========", ret, adc_value);

        yopen_adc_get_volt(YOPEN_ADC_VBAT_CHANNEL, &adc_value); 
        DEMO_ADC_TRACE("========== YOPEN_ADC_VBAT_CHANNEL ret %d adc_value %d ==========", ret, adc_value);
		
        yopen_rtos_task_sleep_ms(1000);

		//量程0-2.4v
        ret = yopen_adc_get_volt_raw(YOPEN_ADC0_CHANNEL, YOPEN_ADC_AIO_RESDIV_RATIO_8OVER16, &adc_value);
        DEMO_ADC_TRACE("========== YOPEN_ADC0_CHANNEL RATIO_8OVER16 ret %d adc_value %d ==========", ret, adc_value*16/8);

        yopen_adc_get_volt_raw(YOPEN_ADC1_CHANNEL, YOPEN_ADC_AIO_RESDIV_RATIO_8OVER16, &adc_value);
        DEMO_ADC_TRACE("========== YOPEN_ADC1_CHANNEL RATIO_8OVER16 ret %d adc_value %d ==========", ret, adc_value*16/8);
        
        //注：THERMAL yopen_adc_div 参数没有意义
        yopen_adc_get_volt_raw(YOPEN_ADC_TEM_CHANNEL, 0, &adc_value);
        DEMO_ADC_TRACE("========== YOPEN_ADC_THERMAL_CHANNEL div ret %d adc_value %d ==========", ret, adc_value);

        //注：VBAT 分压使用YOPEN_AdcVbatResdiv_e和adc0 adc1不同
        yopen_adc_get_volt_raw(YOPEN_ADC_VBAT_CHANNEL, YOPEN_ADC_VBAT_RESDIV_RATIO_8OVER32, &adc_value);
        DEMO_ADC_TRACE("========== YOPEN_ADC_VBAT_CHANNEL RESDIV_RATIO_8OVER32 ret %d adc_value %d ==========", ret, adc_value*32/8);
        
        //量程0-3.6v  //由于aio电气特性, adc输入电压不要超过3.6V，防止芯片损坏
        ret = yopen_adc_get_volt_raw(YOPEN_ADC0_CHANNEL, YOPEN_ADC_AIO_RESDIV_RATIO_4OVER16, &adc_value);
        DEMO_ADC_TRACE("========== YOPEN_ADC0_CHANNEL RATIO_4OVER16 ret %d adc_value %d ==========", ret, adc_value*16/4);

        yopen_adc_get_volt_raw(YOPEN_ADC1_CHANNEL, YOPEN_ADC_AIO_RESDIV_RATIO_4OVER16, &adc_value);
        DEMO_ADC_TRACE("========== YOPEN_ADC1_CHANNEL RATIO_4OVER16 ret %d adc_value %d ==========", ret, adc_value*16/4);
        
        yopen_rtos_task_sleep_ms(1000);
    }
    DEMO_ADC_TRACE("========== fota demo end ==========");

    yopen_rtos_task_delete(NULL);
}
