/*********************************************************
 *  @file    cm_demo_gpio.c
 *  @brief   OpenCPU GPIO示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by zyf 2021/08/30
 ********************************************************/
#ifdef CM_DEMO_GPIO_SUPPORT
#ifdef OS_USING_SHELL

 /****************************************************************************
 * Included Files
 ****************************************************************************/
#include "stdlib.h"
#include <string.h>
#include "cm_gpio_v2.h"
#include "cm_iomux_v2.h"
#include "cm_os.h"


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define cm_demo_printf  osPrintf
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

/****************************************************************************
 * Public Functions
****************************************************************************/
void cm_test_gpio(char argc, char **argv)
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

    if(argv == NULL)
    {
        cm_demo_printf("shell cmd wrong\n");
        return;
    }
    if (argc == 5 && argv[1]!=NULL && strncmp(argv[1], STR_ITEM("read")) == 0
        && argv[2]!=NULL && argv[3]!=NULL && argv[4]!=NULL)
    {
        test_gpio = (cm_gpio_num_e)atoi(argv[2]);
        pin = (cm_iomux_pin_e)atoi(argv[3]);
        fun = (cm_iomux_func_e)atoi(argv[4]);

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
    else if (argc == 6 && argv[1]!=NULL && strncmp(argv[1], STR_ITEM("write")) == 0
        && argv[2]!=NULL && argv[3]!=NULL && argv[4]!=NULL && argv[5]!=NULL)
    {
        test_gpio = (cm_gpio_num_e)atoi(argv[2]);
        pin = (cm_iomux_pin_e)atoi(argv[3]);
        fun = (cm_iomux_func_e)atoi(argv[4]);
        level = atoi(argv[5]);

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
    else if (argc == 5 && argv[1]!=NULL && strncmp(argv[1], STR_ITEM("irq")) == 0
        && argv[2]!=NULL && argv[3]!=NULL && argv[4]!=NULL)
    {
        irq_count0 = 0;
        irq_count1 = 0;
        test_gpio = (cm_gpio_num_e)atoi(argv[2]);
        pin = (cm_iomux_pin_e)atoi(argv[3]);
        fun = (cm_iomux_func_e)atoi(argv[4]);
        irq_pin = pin;

        cfg.direction = CM_GPIO_DIRECTION_INPUT;
        cfg.pull = CM_GPIO_PULL_DOWN;

        cm_iomux_set_pin_func(pin, fun);//初始化之前一定要先设置引脚复用
        cm_gpio_init(test_gpio, &cfg);
        cm_gpio_interrupt_register(test_gpio, cm_test_gpio_irq_callback);
        cm_gpio_interrupt_enable(test_gpio, CM_GPIO_IT_EDGE_RISING);
        irq_gpio = test_gpio;
    }
    else if (argc == 5 && argv[1]!=NULL && strncmp(argv[1], STR_ITEM("func")) == 0
        && argv[2]!=NULL && argv[3]!=NULL)
    {
        test_gpio = atoi(argv[2]);
        fun = atoi(argv[3]);

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
    else if (argc == 5 && argv[1]!=NULL && strncmp(argv[1], STR_ITEM("pin_cmd")) == 0)
    {
        cm_iomux_pin_e pin = atoi(argv[2]);
        cm_iomux_pincmd_e pin_cmd = atoi(argv[3]);
        uint8_t cmd_arg = atoi(argv[4]);
        cm_iomux_set_pin_cmd(pin,pin_cmd,cmd_arg);
    }
    else if (argc == 5 && argv[1]!=NULL && strncmp(argv[1], STR_ITEM("iomux")) == 0
        && argv[2]!=NULL && argv[3]!=NULL && argv[4]!=NULL)
    {
        int ret = -1;
        uint8_t example = atoi(argv[2]);
        cm_iomux_pin_e pin = atoi(argv[3]);
        cm_iomux_func_e fun = atoi(argv[4]);

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
    else if (argc == 2 && argv[1]!=NULL && strncmp(argv[1], STR_ITEM("irq_count")) == 0)
    {
        cm_demo_printf(" gpio irq_count0 = %d,irq_count1 = %d!\n", irq_count0,irq_count1);
    }
    else
    {
        cm_demo_printf("Usage:\n");
        cm_demo_printf("  gpio read gpio_num iomux_pin iomux_func   \n");
        cm_demo_printf("  gpio write gpio_num iomux_pin iomux_func level    \n");
        cm_demo_printf("  gpio irq gpio_num iomux_pin iomux_func    \n");
        cm_demo_printf("  gpio func(0:deinit,1:interrupt_disable,2:interrupt_enable)    \n");
        cm_demo_printf("  gpio pin_cmd iomux_pin iomux_pincmd cmd_arg   \n");
        cm_demo_printf("  gpio iomux example(0:set,1:get) iomux_pin iomux_func  \n");
        cm_demo_printf("  gpio irq_count    \n");
    }
}

#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(gpio, cm_test_gpio);
#endif
#endif

