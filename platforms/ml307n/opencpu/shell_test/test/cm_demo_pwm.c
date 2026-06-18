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
#include <stdlib.h>
#include "cm_pwm.h"
#include "cm_iomux.h"
#include "cm_os.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define cm_demo_printf  osPrintf
#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)
#define OPENCPU_TEST_PWM0_IOMUX CM_IOMUX_PIN_74, CM_IOMUX_FUNC_FUNCTION6     //PIN_9_MUX_AON_PWM_0
#define OPENCPU_TEST_PWM1_IOMUX CM_IOMUX_PIN_79, CM_IOMUX_FUNC_FUNCTION6    //PIN_10_MUX_AON_PWM_1
#define OPENCPU_TEST_PWM2_IOMUX CM_IOMUX_PIN_75, CM_IOMUX_FUNC_FUNCTION2    //PIN_26_MUX_PD_PWM
#define OPENCPU_TEST_PWM3_IOMUX CM_IOMUX_PIN_57, CM_IOMUX_FUNC_FUNCTION4    //PIN_26_MUX_PD_PWM

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
void cm_test_pwm(char argc, char **argv)
{
    uint32_t period = 0, period_h = 0;
    uint8_t dev = CM_PWM_DEV_0;
    const char *param[4] = {NULL};
    int param_num = 4;

    if (argc == 5 && argv != NULL && strncmp(argv[1], STR_ITEM("start")) == 0)
    {
        if(argv[2] != NULL && argv[3] != NULL && argv[4] != NULL )
        {
            dev = atoi(argv[2]);
            period = atoi(argv[3]);
            period_h = atoi(argv[4]);
            cm_demo_printf("pwm=%d, period=%d,period_h =%d\n", dev, period, period_h);
            if(dev==0)
            {
                cm_iomux_set_pin_func(OPENCPU_TEST_PWM0_IOMUX);
            }
            else if(dev==1)
            {
                cm_iomux_set_pin_func(OPENCPU_TEST_PWM1_IOMUX);
            }
            else if(dev==2)
            {
                cm_iomux_set_pin_func(OPENCPU_TEST_PWM2_IOMUX);
            }
            else if(dev==3)
            {
                cm_iomux_set_pin_func(OPENCPU_TEST_PWM3_IOMUX);
            }
    /*        if(0 != cm_pwm_set_clk(dev, clk))//时钟源选择需要放在open之前
            {
                cm_demo_printf("pwm%d cm_pwm_set_clk error\n", dev);
                return;
            }
    */
            if(0 != cm_pwm_open_ns(dev, period, period_h))
            {
                cm_demo_printf("pwm%d open error\n", dev);
            }
            return;
        }
    }
    else if (argc == 3 && argv != NULL && strncmp(argv[1], STR_ITEM("close")) == 0)
    {
        if(argv[2] != NULL)
        {
            dev = atoi(argv[2]);
            if(cm_pwm_close(dev) == 0)
            {
                cm_demo_printf("[PWM]Stop Success\n");
            }
            else
            {
                cm_demo_printf("[PWM]Stop Fail\n");
            }
            return;
        }
    }

    cm_demo_printf("Usage:\n");
    cm_demo_printf("  pwm start dev period period_h      - open pwm\n");
    cm_demo_printf("  pwm close dev                      - close pwm\n");
}

#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(pwm, cm_test_pwm);
#endif
#endif

