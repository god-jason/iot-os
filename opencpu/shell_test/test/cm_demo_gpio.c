/*********************************************************
 *  @file    cm_demo_gpio.c
 *  @brief   OpenCPU GPIO示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by zyf 2021/08/30
 ********************************************************/

 /****************************************************************************
 * Included Files
 ****************************************************************************/
#include "stdlib.h"
#include <string.h>
#include "cm_demo_gpio.h"
#include "cm_gpio.h"
#include "cm_iomux.h"
#include "cm_demo_uart.h"
#include "cm_os.h"
#include "cm_pm.h"
#include "cm_sys.h"
#include "cm_mem.h"


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
 
 
/****************************************************************************
 * Private Data
 ****************************************************************************/
static int irq_count0 = 0;
static int irq_count1 = 0;
static cm_gpio_num_e irq_pin = 0;
/****************************************************************************
 * Private Functions
 ****************************************************************************/
/*如果需要GPIO唤醒，参照如下例程*/
cm_gpio_num_e irq_gpio = 0;
static void cm_test_gpio_irq_callback(void)
{
    cm_gpio_level_e level = 0;
    cm_gpio_get_level(irq_gpio, &level);
    if(level == 1)
    {
        irq_count1++;//不能处理打印和耗时任务
    }
    else
    {
        irq_count0++;//不能处理打印和耗时任务
    }
}
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
void cm_test_gpio(EmbeddedCli *cli, char *args, void *context)
{
    cm_gpio_cfg_t cfg = {0};
    cm_gpio_level_e level = 0;
    cm_gpio_num_e test_gpio = 0;
    cm_iomux_pin_e pin = 0;
    cm_iomux_func_e fun = 0;
    cm_iomux_func_e fun1 = 0;
    cm_gpio_direction_e dir = 0;
    const char *param[4] = {NULL};
    int param_num = 4;
    
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }

    if (strncmp(cmd, STR_ITEM("read")) == 0)
    {
        param_num = 3;
        if (parse_params(args, param, param_num) != 0)
        {
            return;
        }
        
        test_gpio = (cm_gpio_num_e)atoi((char *)param[0]);
        pin = (cm_iomux_pin_e)atoi((char *)param[1]);
        fun = (cm_iomux_func_e)atoi((char *)param[2]);
        
        cfg.direction = CM_GPIO_DIRECTION_INPUT;
        cfg.pull = CM_GPIO_PULL_DOWN;
        
        cm_iomux_set_pin_func(pin, fun);//初始化之前一定要先设置引脚复用
        cm_iomux_get_pin_func(pin, &fun1);
        if(fun1 !=fun)
        {
            cm_demo_printf("read gpio_%d set iomux failed\n", test_gpio);
            return;
        }
        
        cm_gpio_init(test_gpio, &cfg);
        cm_iomux_get_pin_func(pin, &fun1);
        cm_gpio_get_level(test_gpio, &level);
        cm_demo_printf("read gpio_%d level = %d\n", test_gpio, level);
        cm_gpio_get_direction(test_gpio, &dir);
        cm_demo_printf("read gpio_%d dir = %d\n", test_gpio, dir);
        cm_gpio_ioctl(test_gpio, CM_GPIO_CMD_GET_DIRECTION, &dir);
        cm_demo_printf("read gpio_%d dir = %d\n", test_gpio, dir);
        cm_gpio_ioctl(test_gpio, CM_GPIO_CMD_GET_LEVEL, &level);
        cm_demo_printf("read gpio_%d level = %d\n", test_gpio, level);
        cm_gpio_deinit(test_gpio);
    }
    else if (strncmp(cmd, STR_ITEM("write")) == 0)
    {
        if (parse_params(args, param, param_num) != 0)
        {
            return;
        }
    
        test_gpio = (cm_gpio_num_e)atoi((char *)param[0]);
        pin = (cm_iomux_pin_e)atoi((char *)param[1]);
        fun = (cm_iomux_func_e)atoi((char *)param[2]);
        level = atoi((char *)param[3]);
        
        if((level == CM_GPIO_LEVEL_HIGH) ||(level == CM_GPIO_LEVEL_LOW))
        {
            cfg.direction = CM_GPIO_DIRECTION_OUTPUT;
            cfg.pull = CM_GPIO_PULL_UP;
            
            cm_iomux_set_pin_func(pin, fun);//初始化之前一定要先设置引脚复用
            
            cm_gpio_init(test_gpio, &cfg);
            cm_gpio_set_level(test_gpio, level);
            cm_demo_printf("set gpio_%d level = %d\n", test_gpio, level);
        }
        else
        {
            cm_demo_printf("set gpio_%d level = %d,please set 0 or 1\n", test_gpio, level);
        }
    }
    else if (strncmp(cmd, STR_ITEM("irq")) == 0)
    {
        param_num = 3;
        if (parse_params(args, param, param_num) != 0)
        {
            return;
        }
    
        irq_count0 = 0;
        irq_count1 = 0;
        test_gpio = (cm_gpio_num_e)atoi((char *)param[0]);
        pin = (cm_iomux_pin_e)atoi((char *)param[1]);
        fun = (cm_iomux_func_e)atoi((char *)param[2]);
        irq_pin = pin;
    
        cfg.direction = CM_GPIO_DIRECTION_INPUT;
        cfg.pull = CM_GPIO_PULL_DOWN;
        
        cm_iomux_set_pin_func(pin, fun);//初始化之前一定要先设置引脚复用
        cm_iomux_set_pin_cmd(pin, CM_IOMUX_PINCMD4_ST,CM_IOMUX_PINCMD4_ST_ENABLE);//初始化之前一定要先设置引脚复用
        
        cm_gpio_init(test_gpio, &cfg);
        cm_gpio_interrupt_register(test_gpio, cm_test_gpio_irq_callback);
        cm_gpio_interrupt_enable(test_gpio, CM_GPIO_IT_EDGE_RISING);
        irq_gpio = test_gpio;
    }
    else if (strncmp(cmd, STR_ITEM("func")) == 0)
    {
        param_num = 2;
        if (parse_params(args, param, param_num) != 0)
        {
            return;
        }
    
        test_gpio = atoi((char *)param[0]);
        fun = atoi((char *)param[1]);
        
        if(fun == 0)
        {
            cm_gpio_deinit(test_gpio);
        }
        else if(fun == 1)
        {
            cm_gpio_interrupt_disable(test_gpio);
        }
        else if(fun == 2)
        {
            cm_gpio_interrupt_enable(test_gpio,CM_GPIO_IT_EDGE_BOTH);
        }
    }
    else if (strncmp(cmd, STR_ITEM("pin_cmd")) == 0)
    {
        param_num = 3;
        if (parse_params(args, param, param_num) != 0)
        {
            return;
        }
    
        cm_iomux_pin_e pin = atoi((char *)param[0]);
        cm_iomux_pincmd_e pin_cmd = atoi((char *)param[1]);
        uint8_t cmd_arg = atoi((char *)param[2]);
        
        cm_iomux_set_pin_cmd(pin,pin_cmd,cmd_arg);
    }
    else if (strncmp(cmd, STR_ITEM("iomux")) == 0)
    {
        param_num = 3;
        if (parse_params(args, param, param_num) != 0)
        {
            return;
        }
    
        int ret = -1;
        uint8_t example = atoi((char *)param[0]);
        cm_iomux_pin_e pin = atoi((char *)param[1]);
        cm_iomux_func_e fun = atoi((char *)param[2]);

        if(example == 0)
        {
            ret = cm_iomux_set_pin_func(pin,fun);
            cm_demo_printf(" cm_iomux_set_pin_func ret = %d!\n",ret);
        }
        else
        {
            ret = cm_iomux_get_pin_func(pin,&fun);
            cm_demo_printf(" cm_iomux_get_pin_func ret = %d,pin=%d,fun=%d!\n", ret, pin, fun);
        }
    }
    else if (strncmp(cmd, STR_ITEM("irq_count")) == 0)
    {
        cm_demo_printf(" gpio irq_count0 = %d,irq_count1 = %d!\n", irq_count0,irq_count1);
    }
    else
    {
        cm_demo_printf("invalid cmd=%s error\n",cmd);
    }
}
