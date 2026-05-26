/*********************************************************
 *  @file    cm_demo_pwm.c
 *  @brief   OpenCPU PWM示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by zyf 2021/08/30
 ********************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdlib.h>
#include "cm_demo_pwm.h"
#include "cm_pwm.h"
#include "cm_demo_common.h"
#include "cm_iomux.h"
#include "cm_demo_uart.h"
#include "cm_os.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)

/****************************************************************************
 * Private Types
 ****************************************************************************/


/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
static int parse_params(const char *args, const char **param, int param_num);
void cm_test_pwm(EmbeddedCli *cli, char *args, void *context);

/****************************************************************************
 * Private Data
 ****************************************************************************/


/****************************************************************************
 * Private Functions
 ****************************************************************************/
static int parse_params(const char *args, const char **param, int param_num)
{
	int i = 0;
    for(i = 0; i < param_num; i++)
    {
        param[i] = embeddedCliGetToken(args, i + 2);
        if (param[i] == NULL)
        {
            cm_demo_printf("invalid param[%d]\n", i);
            return -1;
        }
    }
    return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
void cm_test_pwm(EmbeddedCli *cli, char *args, void *context)
{
    uint32_t clk = 0, period = 0, period_h = 0;
    uint8_t dev = CM_PWM_DEV_0;
    const char *param[4] = {NULL};
    int param_num = 4;
    
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }

    if (strncmp(cmd, STR_ITEM("start")) == 0)
    {
        if (parse_params(args, param, param_num) != 0)
        {
            return;
        }

        dev = atoi((char *)param[0]);
        clk = atoi((char *)param[1]);
        period = atoi((char *)param[2]);
        period_h = atoi((char *)param[3]);
        
        cm_demo_printf("pwm=%d,clk=%d, period=%d,period_h =%d\n", dev, clk, period, period_h);
        
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
        
        if(0 != cm_pwm_set_clk(dev, clk))//时钟源选择需要放在open之前
        {
            cm_demo_printf("pwm%d cm_pwm_set_clk error\n", dev);
            return;
        }
        
        if(0 != cm_pwm_open_ns(dev, period, period_h))
        {
            cm_demo_printf("pwm%d open error\n", dev);
        }
    }
    else if (strncmp(cmd, STR_ITEM("close")) == 0)
    {
        param_num = 1;
		int i = 0;
        for(i = 0; i < param_num; i++)
        {
            param[i] = embeddedCliGetToken(args, i + 2);
            if (param[i] == NULL)
            {
                cm_demo_printf("invalid param[%d]\n", i);
                return;
            }
        }

        dev = atoi((char *)param[0]);
        
        if(cm_pwm_close(dev) == 0)
        {
            cm_demo_printf("[PWM]Stop Success\n");
        }
        else
        {
            cm_demo_printf("[PWM]Stop Fail\n");
        }
    }
    else
    {
        cm_demo_printf("invalid cmd=%s error\n", cmd);
    }
}

