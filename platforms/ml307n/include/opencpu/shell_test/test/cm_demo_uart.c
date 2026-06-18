/*********************************************************
 *  @file    cm_demo_uart.c
 *  @brief   OpenCPU UART示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by zyf 2021/08/30
 ********************************************************/

#ifdef CM_DEMO_UART_SUPPORT
#ifdef OS_USING_SHELL
/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cm_os.h"
#include "cm_uart.h"
#include "cm_iomux.h"
#include "cm_gpio.h"
#include "cm_mem.h"
#include "cm_uart.h"


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/
#define UART_RX_LEN 128

#define OPENCPU_UART_MPIN_28_29               CM_UART_DEV_2
#define OPENCPU_UART_MPIN_38_39               CM_UART_DEV_1
#define OPENCPU_UART_MPIN_17_18               CM_UART_DEV_0
#define OPENCPU_UART_MPIN_25_49               CM_UART_DEV_3

#define RET_SUCCESS               0
#define RXRB_BUF_LEN        8192 //2048
#define FC_HIGN_THRESH      6144 //1230
#define FC_LOW_THRESH       4096 //820

#define cm_log_printf(log_level, fmt, ...)  osPrintf("[uart-demo]" fmt "\r\n", ##__VA_ARGS__)

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/
static void test_uart_read(cm_uart_dev_e dev);
static void test_uart_send(cm_uart_dev_e dev);

static void USER_TEST_UART_Callback(void *hUart, uint32_t event)
{
    char strevent[30] = {0};
    if(hUart == NULL)
    {
        return;
    }
    if((event & CM_UART_EVENT_TYPE_RX_ARRIVED) !=0)
    {
        cm_uart_dev_e dev = CM_UART_DEV_0;
        if(strcmp(hUart, "uart0") == 0)
        {
            dev = CM_UART_DEV_0;
        }
        else if(strcmp(hUart, "uart1") == 0)
        {
            dev = CM_UART_DEV_0;
        }
        else if(strcmp(hUart, "uart2") == 0)
        {
            dev = CM_UART_DEV_2;
        }
        else if(strcmp(hUart, "uart3") == 0)
        {
            dev = CM_UART_DEV_3;
        }
        cm_log_printf(0, "USER_TEST_UART_Callback %s,RX_ARRIVED, cm_uart_get_rxrb_data_len[%d]\r\n", hUart, cm_uart_get_rxrb_data_len(dev));
        return;
    }
    else if((event & CM_UART_EVENT_TYPE_RX_OVERFLOW) !=0)
    {
        strncpy(strevent, "RX_OVERFLOW", 30);
    }
    else if((event & CM_UART_EVENT_TYPE_TX_COMPLETE) !=0)
    {
        strncpy(strevent, "TX_COMPLETE", 30);
    }
    else if((event & CM_UART_EVENT_TYPE_RX_FLOWCTRL) !=0)
    {
        strncpy(strevent, "RX_FLOWCTRL", 30);
    }
    cm_log_printf(0, "USER_TEST_UART_Callback %s,%s\r\n", hUart, strevent);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

static void uart_test_init(cm_uart_dev_e dev, cm_uart_cfg_t *config, cm_uart_event_t *event)
{
    cm_uart_cfg_t get_config = {0};

    /* 注册事件和回调函数 */
    int ret = cm_uart_register_event(dev, event);
    if (ret != RET_SUCCESS)
    {
        cm_log_printf(0, "uart dev[%d]register event err,ret=%d \r\n",dev, ret);
        return;
    }
    else
    {
        cm_log_printf(0, "cm_uart_register_event dev[%d] success \r\n",dev);
    }

    /* 开启串口 */
    cm_log_printf(0, "cm_uart_open dev[%d]start... ...\r\n",dev);
    ret = cm_uart_open(dev, config);
    if (ret != RET_SUCCESS)
    {
        cm_log_printf(0, "uart init dev[%d] err,ret=%d \r\n", dev,ret);
        return;
    }
    else
    {
        cm_log_printf(0, "cm_uart_open success \r\n");
    }

    ret = cm_uart_get_cfg(dev, &get_config);
    if (ret != 0)
    {
         cm_log_printf(0, "cm_uart_get_cfg dev[%d] failed,ret=%d", dev, ret);
    }
    else
    {
        cm_log_printf(0, "baudrate:%d,flow_ctrl:%d,rxrb_buf_size:%d", get_config.baudrate, get_config.flow_ctrl, get_config.rxrb_buf_size);
        if (get_config.baudrate == config->baudrate)
        {
            cm_log_printf(0, "cm_uart_get_cfg suceess");
        }
        else
        {
             cm_log_printf(0, "cm_uart_get_cfg failed");
        }
    }
}

static void close_test_uart(cm_uart_dev_e dev)
{
    int ret = cm_uart_close(dev);
    if (ret != RET_SUCCESS)
    {
        cm_log_printf(0, "cm_uart_close dev[%d] err,ret=%d", dev, ret);
    }
    else
    {
        cm_log_printf(0, "cm_uart_close dev[%d] success",dev);
    }
}
static int32_t test_uart_clean(cm_uart_dev_e dev)
{
    int32_t ret = cm_uart_clean(dev);
    if (ret < 0)
    {
        cm_log_printf(0, "test_uart_clean err,ret=%d", ret);
        return -1;
    }
    cm_log_printf(0, "test_uart_clean success");

    return 0;
}
static int32_t get_uart_send_stat(cm_uart_dev_e dev)
{
    int32_t ret = cm_uart_is_sending(dev);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_is_sending err,ret=%d\n", ret);
        return -1;
    }
    cm_log_printf(0, "cm_uart_is_sending success ret[%d]\r\n",ret);

    return 0;
}

static void test_uart_send(cm_uart_dev_e dev)
{
    uint8_t *p_send = "HELLO";

    cm_log_printf(0, "cm_uart_write start... ...\r\n");
    get_uart_send_stat(dev);
    int ret = cm_uart_write(dev, p_send, strlen(p_send), 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_write err,ret=%d\n", ret);
    }
    else
    {
        cm_log_printf(0, "cm_uart_write success \r\n");
    }
}
static void test_uart_read(cm_uart_dev_e dev)
{
    uint8_t data_rec[UART_RX_LEN + 1] = {0};

    cm_log_printf(0, "cm_uart_read start... ...\n");
    uint16_t len = cm_uart_get_rxrb_data_len(dev);
    cm_log_printf(0, "cm_uart_get_rxrb_data_len %d ...\r\n",len);
    int ret = cm_uart_read(dev, data_rec, UART_RX_LEN, 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_read err,ret=%d\n", ret);
    }
    else
    {
        int cnt = 0;
        while(ret >= 0)
        {
            if(ret == 0)
            {
                ret = cm_uart_read(dev, data_rec, UART_RX_LEN, 0);
            }
            cm_log_printf(0, "cm_uart_read ret=%d\n", ret);
            if(ret > 0)
            {
                cm_log_printf(0, "cm_uart_read success  receive date[%d][%s]\r\n", ret, data_rec);
                break;
            }
            osDelay(1000);
            cnt ++;
            if(cnt > 3)
            {
                cm_log_printf(0, "cm_uart_read fail timeout\r\n");
                break;
            }
        }

    }
}


void uart_mpin_init(cm_uart_dev_e dev, cm_uart_byte_size_e byte_size, cm_uart_parity_e parity,
                                cm_uart_stop_bit_e stop_bit, uint32_t baudrate, uint8_t func)
{
    int32_t ret = -1;
    cm_uart_cfg_t get_config = {0};
    cm_uart_event_t event = {0};

    if(dev != OPENCPU_UART_MPIN_38_39 && dev != OPENCPU_UART_MPIN_25_49 && dev != OPENCPU_UART_MPIN_28_29
        && dev != OPENCPU_UART_MPIN_17_18)
    {
        cm_log_printf(0,"uart_mpin_init err dev %d, dev 0/1/2/3!\r\n", dev);
        return;
    }

    /* 配置参数 */
    cm_uart_cfg_t config =
    {
        byte_size,
        parity,
        stop_bit,
        CM_UART_FLOW_CTRL_NONE,
        baudrate,
        func,   //配置为普通串口模式，若要配置为低功耗模式可改为1
        RXRB_BUF_LEN,   //环形缓存区大小按照默认配置8k
        0,
        0,
    };
    cm_log_printf(0,"uart NUM = %d demo start... ...\r\n", dev);
     /* 事件参数 */
    if(dev == OPENCPU_UART_MPIN_38_39)
    {
        event.event_type = CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW;   //注册需要上报的事件类型
        event.event_param = "uart1";    //用户参数
        event.event_entry = USER_TEST_UART_Callback;    //上报事件的回调函数

        cm_log_printf(0,"please open uart poart C \r\n ");
        /* config mpin 38/39 iomux func3(UART1 RX/TX) */
        cm_iomux_set_pin_func(CM_IOMUX_PIN_38, CM_IOMUX_FUNC_FUNCTION3);
        cm_iomux_set_pin_func(CM_IOMUX_PIN_39, CM_IOMUX_FUNC_FUNCTION3);
        cm_log_printf(0, "OPENCPU_UART_MPIN_38_39 cm_iomux_set_pin_func end \r\n");
    }
    else if(dev == OPENCPU_UART_MPIN_25_49)
    {
        event.event_type = CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW;   //注册需要上报的事件类型
        event.event_param = "uart3";    //用户参数
        event.event_entry = USER_TEST_UART_Callback;    //上报事件的回调函数

        cm_log_printf(0,"please open uart poart\r\n ");
        /* config mpin 49/25 iomux func3(UART3 RX/TX) */
        cm_iomux_set_pin_func(CM_IOMUX_PIN_49, CM_IOMUX_FUNC_FUNCTION3);
        cm_iomux_set_pin_func(CM_IOMUX_PIN_25, CM_IOMUX_FUNC_FUNCTION3);
        cm_log_printf(0, "OPENCPU_UART_MPIN_25_49 cm_iomux_set_pin_func end \r\n");
    }
    else if(dev == OPENCPU_UART_MPIN_28_29)
    {
        event.event_type = CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW;   //注册需要上报的事件类型
        event.event_param = "uart2";    //用户参数
        event.event_entry = USER_TEST_UART_Callback;    //上报事件的回调函数

        cm_log_printf(0,"please open uart poart D \r\n ");

        /* 配置引脚复用 */
        /* uart2服用到 PD_GPIO_6 PD_GPIO_7 引脚      */
        cm_iomux_set_pin_func(CM_IOMUX_PIN_28, CM_IOMUX_FUNC_FUNCTION2);
        cm_iomux_set_pin_func(CM_IOMUX_PIN_29, CM_IOMUX_FUNC_FUNCTION2);
        cm_log_printf(0, "cm_iomux_set_pin_func end \r\n");
    }
    else if(dev == OPENCPU_UART_MPIN_17_18)
    {
        event.event_type = CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_FLOWCTRL;   //注册需要上报的事件类型
        event.event_param = "uart0";    //用户参数
        event.event_entry = USER_TEST_UART_Callback;    //上报事件的回调函数
        config.rxrb_buf_size = 0;
        config.flow_ctrl = CM_UART_FLOW_CTRL_HW;

        cm_log_printf(0,"please open uart poart A \r\n ");

        /* 配置引脚复用 */
        /* config mpin 17/18 iomux func3(UART1 RX/TX) */
        cm_iomux_set_pin_func(CM_IOMUX_PIN_17, CM_IOMUX_FUNC_FUNCTION0);
        cm_iomux_set_pin_func(CM_IOMUX_PIN_18, CM_IOMUX_FUNC_FUNCTION0);
        cmo_gpio_set_pull(CM_GPIO_NUM_14, CM_GPIO_PULL_UP);
        cm_log_printf(0, "OPENCPU_UART_MPIN_17_18 cm_iomux_set_pin_func end \r\n");
    }

    uart_test_init(dev, &config, &event);
}

/* 若要修改测试串口可在cm_common.h中修改宏定义 */
void cm_test_uart(char argc, char **argv)
{
    uint8_t dev = 0;
    if ( argc > 1 && argv != NULL && argv[1] != NULL)
    {
        const char *sub_cmd = argv[1];
        if(0 == strcmp(sub_cmd, "init"))
        {
            if(argc < 8
                ||argv[2]==NULL||argv[3]==NULL||argv[4]==NULL||argv[5]==NULL||argv[6]==NULL||argv[7]==NULL)
            {
                cm_log_printf(0,"parm err ,eg:cm_uart init dev byte_size parity stop_bit baudrate\r\n");
                cm_log_printf(0, "uart init 0/1/2/3 8 0 0 115200 0\r\n");
                return;
            }
            //绿色底板：dev0(1718,A口,需要在board.c中把AT口屏蔽掉),dev1(3839,C口,需要在board.c中把log口屏蔽掉),dev2(2829,D口,需要在board.c中把SHELL口屏蔽掉)
            //全功能板：dev3(2549,B口,需要飞线测试)
            dev = atoi(argv[2]);
            cm_uart_byte_size_e byte_size = atoi(argv[3]);
            cm_uart_parity_e parity = atoi(argv[4]);
            cm_uart_stop_bit_e stop_bit = atoi(argv[5]);
            uint32_t baudrate = atoi(argv[6]);
            uint8_t func = atoi(argv[7]);
            uart_mpin_init(dev, byte_size, parity, stop_bit, baudrate, func);
        }
        else if(0 == strcmp(sub_cmd, "close"))
        {
             if(argc < 3 || argv[2]==NULL)
            {
                cm_log_printf(0,"parm err ,eg:cm_uart close dev\r\n");
                cm_log_printf(0, "uart close 0/1/2/3\r\n");
                return;
            }
            dev = atoi(argv[2]);
            if(dev >= CM_UART_DEV_0 && dev <= CM_UART_DEV_3)
            {
                close_test_uart(dev);
            }
            else
            {
                cm_log_printf(0,"err dev %d\r\n",dev);
            }
        }
        else if(0 == strcmp(sub_cmd, "send"))
        {
            if(argc < 3 ||argv[2]==NULL)
            {
                cm_log_printf(0,"parm err ,eg:cm_uart send dev\r\n");
                cm_log_printf(0, "uart send 0/1/2/3\r\n");
                return;
            }
            dev = atoi(argv[2]);
            if(dev >= CM_UART_DEV_0 && dev <= CM_UART_DEV_3)
            {
                test_uart_send(dev);
            }
            else
            {
                cm_log_printf(0,"err dev %d\r\n",dev);
            }
        }
        else if(0 == strcmp(sub_cmd, "read"))
        {
            if(argc < 3 ||argv[2]==NULL)
            {
                cm_log_printf(0,"parm err ,eg:cm_uart read dev\r\n");
                cm_log_printf(0, "uart read 0/1/2/3\r\n");
                return;
            }
            dev = atoi(argv[2]);
            if(dev >= CM_UART_DEV_0 && dev <= CM_UART_DEV_3)
            {
                test_uart_read(dev);
            }
            else
            {
                cm_log_printf(0,"err dev %d\r\n",dev);
            }
        }
        else if(0 == strcmp(sub_cmd, "clean"))
        {
            if(argc < 3 ||argv[2]==NULL)
            {
                cm_log_printf(0,"parm err ,eg:cm_uart clean dev\r\n");
                cm_log_printf(0, "uart clean 0/1/2/3\r\n");
                return;
            }
            dev = atoi(argv[2]);
            if(dev >= CM_UART_DEV_0 && dev <= CM_UART_DEV_3)
            {
                test_uart_clean(dev);
            }
            else
            {
                cm_log_printf(0,"err dev %d\r\n",dev);
            }
        }
    }
    else
    {
        cm_log_printf(0, "uart init dev byte_size parity stop_bit baudrate is_lpuart\r\n");
        cm_log_printf(0, "uart send 0/1/2/3\r\n");
        cm_log_printf(0, "uart read 0/1/2/3\r\n");
        cm_log_printf(0, "uart clean 0/1/2/3\r\n");
        cm_log_printf(0, "uart close 0/1/2/3\r\n");
    }
}

#include "nr_micro_shell.h"

NR_SHELL_CMD_EXPORT(uart, cm_test_uart);


#endif
#endif
