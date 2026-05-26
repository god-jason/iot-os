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
#ifdef CM_DEMO_GPIO_SUPPORT
#ifdef OS_USING_SHELL

#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>
#include "cm_gpio.h"
#include "cm_iomux.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TEST_IRQ        0   /* 测试中断的时候 打开该宏 测试GPIO的读写的时候关闭该宏                         */
#define cm_demo_printf osPrintf

/****************************************************************************
 * Private Types
 ****************************************************************************/



/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
static int irq_count = 0;
/****************************************************************************
 * Private Functions
 ****************************************************************************/


/****************************************************************************
 * Public Functions
****************************************************************************/
int g_irq_is_success = 0;
/*该例程实现gpio控制LED灯点亮功能，参照如下例程*/
static void cm_test_gpio_irq_callback(void)
{
    g_irq_is_success = 1;
    cm_demo_printf("gpio_test_case_2 pass\r\n");
    //cm_gpio_interrupt_disable(CM_GPIO_NUM_21);
}
void gpio_test_case_1(void)
{
    uint32_t timeout = 0;
    cm_gpio_cfg_t cfg;
    int32_t ret = -1;
    cm_gpio_level_e level;

    cm_gpio_level_e level1 = CM_GPIO_LEVEL_HIGH;
    cm_iomux_func_e fun = CM_IOMUX_FUNC_FUNCTION0;
    cm_iomux_func_e fun1 = 0;
    cm_gpio_direction_e dir = 0;
    cm_gpio_pull_e pull = 0;

    cfg.direction = CM_GPIO_DIRECTION_OUTPUT;
    cfg.mode = CM_GPIO_MODE_NUM;
    cfg.pull = CM_GPIO_PULL_NONE;

    cm_demo_printf("gpio_test_case_1 start ...\r\n");

    /* pin 和 test_gpio 指向同一个引脚资源 */
    cm_iomux_set_pin_func(CM_IOMUX_PIN_25, fun);
    cm_iomux_get_pin_func(CM_IOMUX_PIN_25, &fun1);
    if (fun1 == fun)
    {
        cm_demo_printf("cm_iomux_set_pin_func success fun = %d \r\n",fun1);
    }
    else
    {
        cm_demo_printf("cm_iomux_set_pin_func failed \r\n");
        return;
    }

    cm_gpio_init(CM_GPIO_NUM_7, &cfg);
    cm_gpio_set_level(CM_GPIO_NUM_7,level1);
    cm_gpio_get_level(CM_GPIO_NUM_7, &level);
    if (level == level1)
    {
        cm_demo_printf("cm_gpio_get_level success level = %d \r\n",level);
    }
    else
    {
         cm_demo_printf("cm_gpio_get_level failed \r\n");
         return;
    }

    cm_gpio_get_direction(CM_GPIO_NUM_7, &dir);
    if (dir == cfg.direction)
    {
        cm_demo_printf("cm_gpio_get_direction success dir = %d \r\n",dir);
    }
    else
    {
         cm_demo_printf("cm_gpio_get_direction failed \r\n");
         return;
    }

    cmo_gpio_get_pull(CM_GPIO_NUM_7, &pull);
    if (pull == cfg.pull)
    {
        cm_demo_printf("cmo_gpio_get_pull success dir = %d \r\n",dir);
    }
    else
    {
         cm_demo_printf("cmo_gpio_get_pull failed \r\n");
         return;
    }

    cm_gpio_ioctl(CM_GPIO_NUM_7, CM_GPIO_CMD_GET_DIRECTION, &dir);
    cm_demo_printf("read gpio_%d dir = %d\r\n", 2, dir);

    cm_gpio_ioctl(CM_GPIO_NUM_7, CM_GPIO_CMD_GET_LEVEL, &level);
    cm_demo_printf("read gpio_%d level = %d\r\n", 2, level);
    cm_gpio_deinit(CM_GPIO_NUM_7);

    cm_demo_printf("gpio_test_case_1 end ...\r\n");
}

void gpio_test_case_2(void)
{

    cm_gpio_cfg_t cfg = {0};
    //g_irq_is_success = 0;
    cm_demo_printf("gpio_test_case_2 start ...\r\n");
    /*PIN复用*/
    cm_iomux_set_pin_func(CM_IOMUX_PIN_63, CM_IOMUX_FUNC_FUNCTION0);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_62, CM_IOMUX_FUNC_FUNCTION0);

    cm_iomux_set_pin_func(CM_IOMUX_PIN_74, CM_IOMUX_FUNC_FUNCTION2);

    cfg.direction = CM_GPIO_DIRECTION_OUTPUT;
    cm_gpio_init(CM_GPIO_NUM_19, &cfg);
    cm_gpio_set_level(CM_GPIO_NUM_19, CM_GPIO_LEVEL_LOW);
    cfg.direction = CM_GPIO_DIRECTION_INPUT;
    cfg.pull = CM_GPIO_PULL_DOWN;
    cm_gpio_init(CM_GPIO_NUM_21, &cfg);
    cm_gpio_interrupt_enable(CM_GPIO_NUM_21, CM_GPIO_IT_LEVEL_HIGH);
    cm_gpio_interrupt_register(CM_GPIO_NUM_21, cm_test_gpio_irq_callback);

    cm_gpio_set_level(CM_GPIO_NUM_19, CM_GPIO_LEVEL_HIGH);
    osDelay(100);
    if( g_irq_is_success == 1)
    {
        g_irq_is_success = 0;
        cm_demo_printf("gpio_test_case_2 pass deinit\r\n");
        cm_gpio_interrupt_disable(CM_GPIO_NUM_21);
    }


}

void gpio_test_deinit(void)
{
    cm_gpio_cfg_t cfg = {0};
    cfg.direction = CM_GPIO_DIRECTION_OUTPUT;
    cfg.mode = CM_GPIO_MODE_NUM;
    cfg.pull = CM_GPIO_PULL_NONE;

    cm_iomux_set_pin_func(CM_IOMUX_PIN_25, CM_IOMUX_FUNC_FUNCTION0);
    cm_gpio_init(CM_IOMUX_PIN_25, &cfg);
    cm_gpio_set_level(CM_IOMUX_PIN_25, CM_GPIO_LEVEL_HIGH);
    osDelay(1000);
    int ret = cm_gpio_deinit(CM_IOMUX_PIN_25);
    if(ret != 0)
    {
        cm_demo_printf("gpio_test_deinit fail\r\n");
        return -1;
    }
    cm_demo_printf("gpio_test_deinit pass\r\n");

}
void gpio_test_pull(cm_gpio_pull_e type)
{
    cm_gpio_cfg_t cfg = {0};
    cfg.direction = CM_GPIO_DIRECTION_OUTPUT;
    cfg.mode = CM_GPIO_MODE_NUM;
    cfg.pull = CM_GPIO_PULL_NONE;

    cm_iomux_set_pin_func(CM_IOMUX_PIN_25, CM_IOMUX_FUNC_FUNCTION0);
    cm_gpio_init(CM_IOMUX_PIN_25, &cfg);
    cmo_gpio_set_pull(CM_IOMUX_PIN_25, type);

    cm_demo_printf("gpio_test_pull [%d] pass\r\n", type);

}

static bool _is_digit(char ch)
{
    return (ch >= '0') && (ch <= '9');
}

static unsigned int _atoi(const char ** str)
{
    unsigned int i = 0U;
    while(_is_digit(**str)) {
        i = i * 10U + (unsigned int)(*((*str)++) - '0');
    }
    return i;
}


void SHELL_testGpio(char argc, char **argv)
{
    const char *sub_cmd = argv[1];
    if(argc < 2)
    {
        cm_demo_printf("pram err, eg: cm_gpio 1/2/3/4\r\n");
        return;
    }
    if(0 == strcmp(sub_cmd, "1"))
    {
        gpio_test_case_1();
    }
    else if(0 == strcmp(sub_cmd, "2"))
    {
        gpio_test_case_2();
    }
    else if(0 == strcmp(sub_cmd, "3"))
    {
        gpio_test_deinit();
    }
    else if(0 == strcmp(sub_cmd, "4"))
    {
        if(argc < 3)
        {
            cm_demo_printf("pram err, eg: cm_gpio 4 0/1/2\r\n");
            return;
        }
        uint32_t type = _atoi(&argv[2]);
        if(type > CM_GPIO_PULL_UP)
        {
            cm_demo_printf("pram err, eg: cm_gpio 4 0/1/2\r\n");
            return;
        }
        else
        {
            gpio_test_pull(type);
        }

    }
    else
    {
        cm_demo_printf("pram err, eg: cm_gpio 1/2/3/4\r\n");
        return;
    }
}

NR_SHELL_CMD_EXPORT(cm_gpio, SHELL_testGpio);
#endif
#endif
