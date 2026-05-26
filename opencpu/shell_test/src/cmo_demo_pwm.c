/*********************************************************
 *  @file    cm_demo_pwm.c
 *  @brief   OpenCPU PWM示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by zyf 2021/08/30
 ********************************************************/

#ifdef CM_DEMO_PWM_SUPPORT
#ifdef OS_USING_SHELL

 /****************************************************************************
 * Included Files
 ****************************************************************************/
#include <os.h>
#include <stdlib.h>
//#include "chip_pin_list.h"
#include "cm_pwm.h"
#include "cm_iomux.h"
#include <nr_micro_shell.h>
//#include <drv_timer.h>
// #include "drv_gpio.h"

//TIM_Handle* g_testTimerHandle = NULL;

#define cm_demo_printf osPrintf

void cm_test_pwm_start(char argc, char **argv)
{
    uint32_t period = 100000, period_h = 50000;
    uint8_t dev = CM_PWM_DEV_0;

    if(argc < 2)
    {
        cm_demo_printf(" param err! eg: cm_pwm_statrt dev_no period period_h :cm_pwm_statrt 0\n");
        return;
    }

    cm_demo_printf("pwm test start... ...\n");
    if (argc >= 2)
        dev = atoi(argv[1]);
    if (argc >= 3)
        period = atoi(argv[2]);
    if (argc >= 4)
        period_h = atoi(argv[3]);

    //if(clk == )
    cm_demo_printf("pwm_dev=%d, period=%d,period_h =%d\n", dev, period, period_h);

    if(dev==CM_PWM_DEV_0)
    {
        cm_iomux_set_pin_func(OPENCPU_TEST_PWM0_IOMUX);
    }
    else if(dev==CM_PWM_DEV_1)
    {
        cm_iomux_set_pin_func(OPENCPU_TEST_PWM1_IOMUX);
    }

    /*if(0 != cm_pwm_set_clk(dev, clk))//时钟源选择需要放在open之前
    {
        cm_demo_printf("pwm%d cm_pwm_set_clk error\n", dev);
        return;
    }*/

    if(0 != cm_pwm_open_ns(dev, period,period_h))
    {
        cm_demo_printf("pwm%d open error\n", dev);
    }
    cm_demo_printf("pwm%d open successfully :)\n", dev);

}
NR_SHELL_CMD_EXPORT(cm_t_pwm_start, cm_test_pwm_start);


void cm_test_pwm_close(char argc, char **argv)
{
    uint8_t dev = CM_PWM_DEV_0;

    cm_demo_printf("pwm test close... ...\n");

    if(argc != 2)
    {
        cm_demo_printf(" parm error, eg:!\n");
        return;
    }

    if (argc >= 2)
    {
        dev = atoi(argv[1]);
    }


    if(cm_pwm_close(dev) == 0)
    {
        cm_demo_printf("[PWM]Stop Success\n");
    }
    else
    {
        cm_demo_printf("[PWM]Stop Fail\n");
    }
    cm_demo_printf("pwm%d close successfully :)\n", dev);
}
NR_SHELL_CMD_EXPORT(cm_t_pwm_close, cm_test_pwm_close);

#if 0
void general_pwm(char argc, char **argv)//该PWM测试适用于作为音频输出的pwm管脚，pd_pwm
{
    uint8_t testPin = 0;
    uint32_t period = 1000;
    uint32_t duty = 500;

    if (argc >= 2)
        testPin = atoi(argv[1]);
    if (argc >= 3)
        period = atoi(argv[2]);
    if (argc >= 4)
        duty = atoi(argv[3]);

    if (testPin == 0)
        PIN_SetMux(PIN_RES(PIN_14), PIN_14_MUX_PD_PWM);
    else if (testPin == 1)
        PIN_SetMux(PIN_RES(PIN_25), PIN_25_MUX_PD_PWM);
    else if (testPin == 2)
        PIN_SetMux(PIN_RES(PIN_26), PIN_26_MUX_PD_PWM);
    else if (testPin == 3)
        PIN_SetMux(PIN_RES(PIN_29), PIN_29_MUX_PD_PWM);
    else if (testPin == 4)
        PIN_SetMux(PIN_RES(PIN_33), PIN_33_MUX_PD_PWM);
    else if (testPin == 5)
        PIN_SetMux(PIN_RES(PIN_39), PIN_39_MUX_PD_PWM);

    PWM_Start(duty, period);
    osThreadMsSleep(20000);
    PWM_Stop();              // 停止
    return;

}
NR_SHELL_CMD_EXPORT(general_pwm_test, general_pwm);


// 只有PWM_TIMER0 支持PWM 输出
void Timer_PwmTest(char argc, char **argv)//该PWM测试适用于复用为pwm_0及pwm_1类型的管脚
{
    uint8_t timId = PWM_TIMER0_0;

    if (argc >= 2)
        timId = atoi(argv[1]);

    // if(timId != PWM_TIMER0_0 && timId != PWM_TIMER0_1)
    //     return;

    PIN_SetMux(PIN_RES(PIN_9), PIN_9_MUX_AON_PWM_0);   // PWM_TIMER0_0
//    PIN_SetMux(PIN_RES(PIN_10), PIN_10_MUX_AON_PWM_1); // PWM_TIMER1_0
//    PIN_SetMux(PIN_RES(PIN_7), PIN_7_MUX_AON_PWM_0);   // PWM_TIMER0_0
    PIN_SetMux(PIN_RES(PIN_8), PIN_8_MUX_AON_PWM_1);   //// PWM_TIMER1_0 输入命令tim_pwm 4
    // WRITE_U32(0xF1003000, 6 << 21 | 6 << 24);

    g_testTimerHandle = TIM_Request(timId, NULL);
    if (!g_testTimerHandle)
        OS_ASSERT(0);
    cm_demo_printf("[PWM]TIM_Request Success\n");

    int8_t ret = TIM_EnablePwm(g_testTimerHandle, true);
    if(ret != 0)
    {
        cm_demo_printf("[PWM]TIM_EnablePwm fail %d\n", ret);
        return;
    }
    cm_demo_printf("[PWM]TIM_EnablePwm Success\n");
    ret = TIM_Startup(g_testTimerHandle, 100, 25, TIMER_PERIOD_MODE, 0);
    if(ret != 0)
    {
        cm_demo_printf("[PWM]TIM_Startup fail %d\n", ret);
        return;
    }
    cm_demo_printf("[PWM]TIM_Startup Success\n");
    osThreadMsSleep(30000);//延时30s便于观察波形

    TIM_Release(&g_testTimerHandle);
    cm_demo_printf("[PWM]Timer_PwmTest Success\n");

    return;
}

NR_SHELL_CMD_EXPORT(tim_pwm, Timer_PwmTest);

#endif

#endif
#endif
