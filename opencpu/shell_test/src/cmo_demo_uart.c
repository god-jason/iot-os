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
#include "cm_mem.h"
#include "cm_sys.h"
#include "cm_virt_at.h"
#include "cm_uart.h"
#include "cm_gpio.h"


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define UART_BUF_LEN            4096

/****************************************************************************
 * Private Types
 ****************************************************************************/
#define UART_RX_LEN 128
#define OPENCPU_MAIN_UART                     CM_UART_DEV_2
#define OPENCPU_TEST_UARTTX_PIN               CM_IOMUX_PIN_29
#define OPENCPU_TEST_UARTRX_PIN               CM_IOMUX_PIN_28
#define OPENCPU_TEST_UARTTX_IOMUX             CM_IOMUX_FUNC_FUNCTION2
#define OPENCPU_TEST_UARTRX_IOMUX             CM_IOMUX_FUNC_FUNCTION2
#define OPENCPU_UART_WEKEUP_PIN               CM_IOMUX_PIN_76

#define OPENCPU_UART_MPIN_76_87               CM_UART_DEV_1
#define OPENCPU_UART_MPIN_38_39               OPENCPU_UART_MPIN_76_87
#define OPENCPU_UART_MPIN_17_18               CM_UART_DEV_0
#define OPENCPU_UART_MPIN_25_49               CM_UART_DEV_3




#define UART_TASK_PRIORITY                    (osPriorityNormal)

#define cm_demo_printf osPrintf

#define CM_PRT_INF(fmt,arg...)    cm_demo_printf("[CM_INF]line %d in %s:"fmt,__LINE__,__FILE__,##arg)
#define CM_PRT_WRN(fmt,arg...)    cm_demo_printf("[CM_WRN]line %d in %s:"fmt,__LINE__,__FILE__,##arg)
#define CM_PRT_ERR(fmt,arg...)    cm_demo_printf("[CM_ERR]line %d in %s:"fmt,__LINE__,__FILE__,##arg)
#define RET_SUCCESS               0
#define RXRB_BUF_LEN        8192 //2048
#define FC_HIGN_THRESH      6144 //1230
#define FC_LOW_THRESH       4096 //820
#define EVENT_QUEUE_NUM     10

#define USE_FLOW_CTRL

#define UART_FUNC_USER      0
#define UART_FUNC_AT        1
#define UART_FUNC_CONSOLE   2
#define UART_FUNC_LOG       3




/**
 *  \brief 串口接收数据结构体
 */
typedef struct
{
    unsigned char buf[1024];
    int len;
}cm_uart_recv_data_buf_t;
/**命令结构体*/
typedef struct cm_cmd {
    char * cmdstr;
    void (* cmdfunc)(unsigned char **,int);
} cm_cmd_t;
typedef void *cm_thread_id_t;

/**
 *  \brief 串口接收命令接口
 */
typedef struct
{
    int cmd_execute;  //命令执行标志，1未被执行，0执行完成
    unsigned char * buf[20]; //最多接受20个参数
    int len;
}cm_uart_cmd_recv_t;



typedef struct{
    int msg_type;
    uint32_t msg_data_len; //消息体长度
    void*   pmsgData;  //   消息体
} uart_event_msg_t;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
static void cm_uart_recv_task(void *param);
static int __cm_cmd_engine(char * prefix,char * uart_buf,int* uart_buf_len,char **cmd_buf,int *cmd_len);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static int rx_rev_len = 0;
static char rx_rev_data[UART_BUF_LEN +1] = {0};
static osThreadId_t OC_Uart_TaskHandle = NULL; //串口数据接收、解析任务Handle
static osThreadId_t uart_event_thread = NULL;
cm_uart_cmd_recv_t gstUartCmdRecv = {0}; //串口命令缓冲区

static osThreadId_t g_uart_rx_task = NULL;
static osThreadId_t g_uart_test_task = NULL;
static osMessageQueueId_t uart_event_queue = NULL;

// extern osEventFlagsId_t cmd_task_flag;

/****************************************************************************
 * Private Functions
 ****************************************************************************/
static void test_uart_read(cm_uart_dev_e dev);
static void test_uart_send(cm_uart_dev_e dev);


/* 用于测试串口事件，用户可参考 */
static void uart_event_task(void *arg)
{
    uart_event_msg_t msg = {0};

    while (1)
    {
        if (osMessageQueueGet(uart_event_queue, &msg, NULL, osWaitForever) == osOK)
        {
            //cm_log_printf(0, "uart event msg type = %d\n", msg.msg_type);
            if (CM_UART_EVENT_TYPE_RX_OVERFLOW & msg.msg_type)
            {
                cm_log_printf(0, "CM_UART_EVENT_TYPE_RX_OVERFLOW... ...");
            }

            if (CM_UART_EVENT_TYPE_RX_FLOWCTRL & msg.msg_type)
            {
                cm_log_printf(0, "CM_UART_EVENT_TYPE_RX_FLOWCTRL... ...");
            }
        }
    }
}

/* 用于测试串口事件，用户可参考 */
static int uart_event_task_create(void)
{
    if (uart_event_queue == NULL)
    {
        uart_event_queue = osMessageQueueNew(10, sizeof(uart_event_msg_t), NULL);
    }

    if (uart_event_thread == NULL)
    {
        osThreadAttr_t attr1 = {
            .name = "uart_event",
            .priority = UART_TASK_PRIORITY,
            .stack_size = 1024 * 2,
        };
        uart_event_thread = osThreadNew(uart_event_task, NULL, (const osThreadAttr_t*)&attr1);
    }

    return 0;
}

/* 解析指令函数，用于SDK测试，与串口配置无关 */
static int __cm_cmd_engine(char * prefix,char * uart_buf,int* uart_buf_len,char **cmd_buf,int *cmd_len)
{
    char *p1 = NULL;
    char *p2 = NULL;
    char *p3 = NULL;
    char *temp1 = NULL;
    char *temp2 = NULL;

    uart_buf[*uart_buf_len] = 0;//结尾

    p1 = strstr(uart_buf, prefix);

    if (p1 == 0)
    {
        *uart_buf_len = 0;
        return -1;
    }

    p2 = strstr(p1, "\r\n");

    if (p2 == 0)
    {
        *uart_buf_len = 0;
        return -1;
    }

    *cmd_len = 0;

    p3 = strchr(p1, ':');

    if (p3 == 0)
    {
        *uart_buf_len = 0;
        return -1;
    }

    cmd_buf[(*cmd_len)++] = cm_malloc(p3 - p1 + 1 + 1);
    memset(cmd_buf[*cmd_len - 1], 0, p3 - p1 + 1);

    if (cmd_buf[*cmd_len - 1] == 0)
    {
        *uart_buf_len = 0;
        return -1;
    }

    memcpy(cmd_buf[*cmd_len - 1], p1, (int)(p3 - p1));
    temp1 = p3;

    while (1)
    {
        if (*cmd_len >= 20)
        {
            *uart_buf_len = 0;
            for (int j = 0; j < *cmd_len; j++)
            {
                cm_free(cmd_buf[j]);
            }
            return -1;
        }

        temp2 = strchr(temp1 + 1, ':');

        if (temp2 == NULL)
        {
            break;
        }

        if ((temp2 - temp1) < 1)
        {
            for (int j = 0; j < *cmd_len; j++)
            {
                cm_free(cmd_buf[j]);
            }

            *uart_buf_len = 0;
            return -1;
        }

        cmd_buf[(*cmd_len)++] = cm_malloc(temp2 - temp1 + 1);

        if (cmd_buf[*cmd_len -1] == 0)
        {
            for(int j = 0; j < *cmd_len; j++)
            {
                cm_free(cmd_buf[j]);
            }

            *uart_buf_len = 0;
            return -1;
        }

        memset(cmd_buf[*cmd_len - 1], 0, temp2 - temp1);
        memcpy(cmd_buf[*cmd_len - 1], temp1 + 1, temp2 - temp1 - 1);
        temp1 = temp2;
    }

    if ((p2 - temp1 - 1) < 1)
    {
        for (int j = 0; j < *cmd_len; j++)
        {
            cm_free(cmd_buf[j]);
        }

        *uart_buf_len = 0;
        return -1;
    }

    cmd_buf[(*cmd_len)++] = cm_malloc(p2 - temp1 + 1);
    memset(cmd_buf[*cmd_len - 1], 0, p2 - temp1);
    memcpy(cmd_buf[*cmd_len - 1], temp1 + 1, p2 - 1 - temp1);
    *uart_buf_len = 0;
    return 0;
}
static bool uart_send_msg(uart_event_msg_t *msg)
{
    if(uart_event_queue != NULL)
    {
        if(osOK != osMessageQueuePut(uart_event_queue, msg, 0, 0))
        {
            cm_log_printf(0," cm_messagequeue_send fail\r\n");
            return false;
        }
        return true;
    }
    else
    {
        cm_log_printf(0," uart_event_queue NULL\r\n");
        return false;
    }
}

static void USER_TEST_UART_Callback(void *hUart, uint32_t event)
{
    cm_gpio_level_e level = 0;
    //cm_log_printf(0, "USER_TEST_UART_Callback event[0x%X] hUart %s\r\n", event,hUart);
    if((event & CM_UART_EVENT_TYPE_RX_ARRIVED) !=0)
    {
        uart_event_msg_t msg = {0};

        if(strcmp(hUart, "uart1") == 0)
        {
            msg.msg_type = CM_UART_DEV_1;
        }
        else if(strcmp(hUart, "uart2") == 0)
        {
            msg.msg_type = CM_UART_DEV_2;
        }
        else if(strcmp(hUart, "uart0") == 0)
        {
            msg.msg_type = CM_UART_DEV_0;
        }
        else if(strcmp(hUart, "uart3") == 0)
        {
            msg.msg_type = CM_UART_DEV_3;
        }
        msg.msg_data_len = 0;
        msg.pmsgData = NULL;
        uart_send_msg(&msg);
    }
#ifdef USE_FLOW_CTRL
    int ret = cm_gpio_get_level(CM_GPIO_NUM_24, &level);

    if((event & CM_UART_EVENT_TYPE_RX_FLOWCTRL) !=0)
    {
        if(ret == 0 && level == CM_GPIO_LEVEL_LOW)
        {
            cm_gpio_set_level(CM_GPIO_NUM_24,CM_GPIO_LEVEL_HIGH);
            cm_log_printf(0, "set rts high\r\n");
        }

    }
    else
    {

        if(ret == 0 && level == CM_GPIO_LEVEL_HIGH)
        {
            cm_gpio_set_level(CM_GPIO_NUM_24,CM_GPIO_LEVEL_LOW);
            cm_log_printf(0, "set rts low\r\n");
        }
    }

#endif
}

static void cm_uart_recv_task(void *param)
{
    uint32_t ret = -1;
    //int32_t  ret_len = 0;
    uint8_t data_rec[UART_RX_LEN] = {0};
    cm_log_printf(0, "cm_uart_recv_task run ...\r\n");

    while(1)
    {
        //ret = cm_uart_read(OPENCPU_MAIN_UART, data_rec, UART_RX_LEN, 0);
        ret = cm_uart_read(OPENCPU_UART_MPIN_38_39, data_rec, UART_RX_LEN, 0);
        if (ret < 0)
        {
            cm_log_printf(0, "cm_uart_read err,ret=%d\r\n", ret);
            break;
        }
        else
        {
            //ret_len = cm_uart_get_rxrb_data_len(OPENCPU_UART_MPIN_38_39);
            if(ret > 0)
            {
                cm_log_printf(0, "cm_uart_read success  receive date count is %d \r\n", ret);
                for (uint8_t i = 0; i < ret; i++)
                {
                    cm_log_printf(0, "data_rec[%d] = %x \r\n", i,data_rec[i]);
                }
                cm_log_printf(0, "OPENCPU_UART_MPIN_38_39 data_rec[%s]\r\n", data_rec);
#if 0
                cm_log_printf(0, "cm_uart_read test end \r\n");
                cm_uart_clean(OPENCPU_UART_MPIN_38_39);
                ret = cm_uart_close(OPENCPU_UART_MPIN_38_39);
                if (ret == 0)
                {
                    cm_log_printf(0, "cm_uart_close success \r\n");
                }
                else
                {
                   cm_log_printf(0, "cm_uart_close failed ret = %d \r\n", ret);
                }

                break;
#endif
            }
            osDelay(1000);

        }
    }

}

static void uart_read_and_send(cm_uart_dev_e dev, uint16_t len)
{

    int ret = cm_uart_read(dev, rx_rev_data, len, 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_read dev[%d]err,ret=%d\r\n",dev, ret);
        return;
    }
    ret = cm_uart_write(dev, rx_rev_data, len, 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_write dev[%d] err,ret=%d\n", dev, ret);
    }

}

static void test_uart(cm_uart_dev_e dev, uint16_t len)
{
    uint16_t remain_len = len;
    while(remain_len > 0)
    {
        uint16_t handle_len = (remain_len > UART_BUF_LEN) ? UART_BUF_LEN : remain_len;
        uart_read_and_send(dev, handle_len);
        remain_len -= handle_len;
    }
#if 0
    if(len <= UART_BUF_LEN)
    {
        uart_read_and_send(dev,len);
    }
    else
    {
        int ret_len = len;
        do
        {
            uart_read_and_send(dev,UART_BUF_LEN);
            ret_len -= UART_BUF_LEN;
        }while(ret_len > UART_BUF_LEN);
        if(ret_len > 0)
        {
            uart_read_and_send(dev,ret_len);
        }

    }
#endif

}


static void cm_uart_test_task(void *param)
{
    uart_event_msg_t msg = {0};

    cm_log_printf(0, "cm_uart_test_task run ...\r\n");

    while(1)
    {

        if(cm_messagequeue_wait(uart_event_queue, &msg) == 0)
        {
            uint16_t len = cm_uart_get_rxrb_data_len(msg.msg_type);
            cm_log_printf(0, "get msg radlen %d ...\r\n",len);
            if(msg.msg_type == CM_UART_DEV_1)
            {
                test_uart(OPENCPU_UART_MPIN_38_39, len);

            }
            else if(msg.msg_type == CM_UART_DEV_2)
            {
                test_uart(OPENCPU_MAIN_UART, len);
            }
        }
        else
        {
            cm_log_printf(0,"[UART] Task Get Msg Err\r\n");
        }

        //test_uart(OPENCPU_UART_MPIN_38_39);
        //test_uart(OPENCPU_MAIN_UART);
    }

}


/* 切换debug引脚打印的log到usb打印，掉电不保存配置 */
static void cm_virt_at_log2cat(void)
{
    char operation[64] = {0};
    sprintf(operation, "%s\r\n", "AT+MCFG=log2cat,1");
    uint8_t rsp[128] = {0};
    int32_t rsp_len = 0;

    if(cm_virt_at_send_sync((const uint8_t *)operation, rsp, &rsp_len, 10) == 0)
    {
        cm_log_printf(0, "rsp=%s rsp_len=%d\n", rsp, rsp_len);
    }
    else
    {
        cm_log_printf(0, "ret != 0\n");
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* 从测试串口打印字符串 */
#if 0
void cm_demo_printf(char *str, ...)
{
    char s[600] = {0}; //This needs to be large enough to store the string TODO Change magic number
    va_list args;
    int len;

    if ((str == NULL) || (strlen(str) == 0))
    {
        return;
    }

    va_start(args, str);
    len = vsnprintf((char*)s, 600, str, args);
    va_end(args);
    cm_uart_write(OPENCPU_MAIN_UART, s, len, 1000);
}
#endif


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
        cm_log_printf(0, "baudrate=%d,flow_ctrl:%d", get_config.baudrate, get_config.flow_ctrl);
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


void uart_test_case_mpin_76_87(void)
{
    int32_t ret = -1;
    uint8_t *p_send = "HELLO";
    uint8_t data_rec[UART_RX_LEN + 1] = {0};
    cm_uart_cfg_t get_config = {0};

    /* 配置参数 */
    cm_uart_cfg_t config =
    {
        CM_UART_BYTE_SIZE_8,
        CM_UART_PARITY_NONE,
        CM_UART_STOP_BIT_ONE,
#ifdef USE_FLOW_CTRL
        CM_UART_FLOW_CTRL_HW,
#else
        CM_UART_FLOW_CTRL_NONE,
#endif
        CM_UART_BAUDRATE_115200,
        0,   //配置为普通串口模式，若要配置为低功耗模式可改为1,只有uart0具备低功耗功能
        RXRB_BUF_LEN,   //环形缓存区大小按照默认配置8k
        FC_HIGN_THRESH,
        FC_LOW_THRESH,
    };

    /* 事件参数 */
    cm_uart_event_t event =
    {
        CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW,   //注册需要上报的事件类型
        "uart1",                                                        //用户参数
        USER_TEST_UART_Callback                                              //上报事件的回调函数
    };

    cm_demo_printf("uart NUM = %d demo start... ...\r\n", OPENCPU_UART_MPIN_76_87);
    cm_demo_printf("please open uart poart B \r\n ");

    /* 配置引脚复用 */
    /* uart2服用到 PD_GPIO_12 PD_GPIO_13 引脚      */
    //PIN_SetMux(PIN_RES(PIN_33), PIN_33_MUX_UART1_RX);
    //PIN_SetMux(PIN_RES(PIN_34), PIN_34_MUX_UART1_TX);
    //PIN_SetMux(PIN_RES(PIN_35), PIN_35_MUX_UART1_CTS);
    //PIN_SetMux(PIN_RES(PIN_36), PIN_36_MUX_UART1_RTS);

    /* config mpin 76/87 iomux func5(UART1 TX/RX) */
    cm_iomux_set_pin_func(CM_IOMUX_PIN_76, CM_IOMUX_FUNC_FUNCTION5);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_87, CM_IOMUX_FUNC_FUNCTION5);
#ifdef USE_FLOW_CTRL
    cm_iomux_set_pin_func(CM_IOMUX_PIN_86, CM_IOMUX_FUNC_FUNCTION5);//PD_gpio_14,PIN_35_MUX_UART1_CTS
    cm_iomux_set_pin_func(CM_IOMUX_PIN_77, CM_IOMUX_FUNC_FUNCTION5);//PD_gpio_15,PIN_36_MUX_UART1_RTS
#endif
    cm_log_printf(0, "OPENCPU_UART_MPIN_76_87 cm_iomux_set_pin_func end \r\n");

    uart_test_init(OPENCPU_UART_MPIN_76_87,&config, &event);
    cm_log_printf(0, "cm_uart_write start... ...\r\n");
    ret = cm_uart_write(OPENCPU_UART_MPIN_76_87, p_send, strlen(p_send), 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_write err,ret=%d\n", ret);
        return;
    }
    else
    {
        cm_log_printf(0, "OPENCPU_UART_MPIN_76_87 cm_uart_write success \r\n");
    }
    // g_uart_rx_task = cm_thread_create("uart_rx", 2048, UART_TASK_PRIORITY, cm_uart_recv_task);
    //     /* 以下为串口接收示例，不影响串口配置，用户可酌情参考 */
    // osThreadAttr_t uart_task_attr = {0};
    // uart_task_attr.name = "uart_task";
    // uart_task_attr.stack_size = 2048;
    // uart_task_attr.priority= UART_TASK_PRIORITY;

    // OC_Uart_TaskHandle = osThreadNew(cm_uart_recv_task, 0, &uart_task_attr);


    cm_log_printf(0, "cm_uart_read start... ...\n");
    ret = cm_uart_read(OPENCPU_UART_MPIN_76_87, data_rec, UART_RX_LEN, 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_read err,ret=%d\n", ret);
    }
    else
    {
        while(ret == 0)
        {
            ret = cm_uart_read(OPENCPU_UART_MPIN_76_87, data_rec, UART_RX_LEN, 0);
            if(ret > 0)
            {
                cm_log_printf(0, "cm_uart_read success  receive date count is %d\r\n", ret);
                for (uint8_t i = 0; i < ret; i++)
                {
                    cm_log_printf(0, "data_rec[%d] = %x \r\n", i,data_rec[i]);
                }
                cm_log_printf(0, "data_rec[%s] \r\n", data_rec);
            }
            cm_delay(1000);

        }
        cm_log_printf(0, "OPENCPU_UART_MPIN_76_87 cm_uart_read test end \r\n");
    }
    cm_log_printf(0, "cm_uart_close start... ...\r\n");
    close_test_uart(OPENCPU_UART_MPIN_76_87);
}

void uart_test_case_mpin_38_39(void)
{
    int32_t ret = -1;
    uint8_t *p_send = "HELLO";
    uint8_t data_rec[UART_RX_LEN + 1] = {0};
    cm_uart_cfg_t get_config = {0};

    /* 配置参数 */
    cm_uart_cfg_t config =
    {
        CM_UART_BYTE_SIZE_8,
        CM_UART_PARITY_NONE,
        CM_UART_STOP_BIT_ONE,
        CM_UART_FLOW_CTRL_NONE,
        CM_UART_BAUDRATE_9600,
        0,   //配置为普通串口模式，若要配置为低功耗模式可改为1
        RXRB_BUF_LEN,   //环形缓存区大小按照默认配置8k
        FC_HIGN_THRESH,
        FC_LOW_THRESH,
    };

    /* 事件参数 */
    cm_uart_event_t event =
    {
        CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW,   //注册需要上报的事件类型
        "uart1",                                                        //用户参数
        USER_TEST_UART_Callback                                              //上报事件的回调函数
    };

    cm_demo_printf("uart NUM = %d demo start... ...\r\n", OPENCPU_UART_MPIN_38_39);
    cm_demo_printf("please open uart poart C \r\n ");

    /* config mpin 38/39 iomux func3(UART1 RX/TX) */
    cm_iomux_set_pin_func(CM_IOMUX_PIN_38, CM_IOMUX_FUNC_FUNCTION3);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_39, CM_IOMUX_FUNC_FUNCTION3);
    cm_log_printf(0, "OPENCPU_UART_MPIN_38_39 cm_iomux_set_pin_func end \r\n");

    uart_test_init(OPENCPU_UART_MPIN_38_39, &config, &event);

    cm_log_printf(0, "cm_uart_write start... ...\r\n");
    ret = cm_uart_write(OPENCPU_UART_MPIN_38_39, p_send, strlen(p_send), 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_write err,ret=%d\n", ret);
    }
    else
    {
        cm_log_printf(0, "cm_uart_write success \r\n");
    }
#if 1
    osThreadAttr_t attr = {0};
    attr.name       = "uart_rx";
    attr.stack_size = 2048;
    attr.priority   = UART_TASK_PRIORITY;
    g_uart_rx_task = osThreadNew(cm_uart_recv_task, NULL, &attr);
#else
    cm_log_printf(0, "cm_uart_read start... ...\n");
    ret = cm_uart_read(OPENCPU_UART_MPIN_38_39, data_rec, UART_RX_LEN, 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_read err,ret=%d\n", ret);
    }
    else
    {
        int cnt = 0;
        while(ret == 0)
        {
            ret = cm_uart_read(OPENCPU_UART_MPIN_38_39, data_rec, UART_RX_LEN, 0);
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
    cm_log_printf(0, "cm_uart_close start... ...\r\n");
    ret = cm_uart_close(OPENCPU_UART_MPIN_38_39);
    if (ret != RET_SUCCESS)
    {
        cm_log_printf(0, "cm_uart_close err,ret=%d\n", ret);
    }
    else
    {
        cm_log_printf(0, "cm_uart_close success\n");
    }
#endif
}

void uart_mpin_7687_init(void)
{
    int32_t ret = -1;
    cm_uart_cfg_t get_config = {0};

    /* 配置参数 */
    cm_uart_cfg_t config =
    {
        CM_UART_BYTE_SIZE_8,
        CM_UART_PARITY_NONE,
        CM_UART_STOP_BIT_ONE,
#ifdef USE_FLOW_CTRL
        CM_UART_FLOW_CTRL_HW,
#else
        CM_UART_FLOW_CTRL_NONE,
#endif
        CM_UART_BAUDRATE_115200,
        0,   //配置为普通串口模式，若要配置为低功耗模式可改为1
        RXRB_BUF_LEN,   //环形缓存区大小按照默认配置8k
        FC_HIGN_THRESH,
        FC_LOW_THRESH,
    };

    /* 事件参数 */
    cm_uart_event_t event =
    {
        CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW,   //注册需要上报的事件类型
        "uart1",                                                        //用户参数
        USER_TEST_UART_Callback                                              //上报事件的回调函数
    };

    cm_demo_printf("uart NUM = %d demo start... ...\r\n", OPENCPU_UART_MPIN_76_87);
    cm_demo_printf("please open uart poart B \r\n ");

    /* config mpin 76/87 iomux func5(UART1 TX/RX) */
    cm_iomux_set_pin_func(CM_IOMUX_PIN_76, CM_IOMUX_FUNC_FUNCTION5);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_87, CM_IOMUX_FUNC_FUNCTION5);
#ifdef USE_FLOW_CTRL
    cm_iomux_set_pin_func(CM_IOMUX_PIN_86, CM_IOMUX_FUNC_FUNCTION5);//PD_gpio_14,PIN_35_MUX_UART1_CTS
    cm_iomux_set_pin_func(CM_IOMUX_PIN_77, CM_IOMUX_FUNC_FUNCTION5);//PD_gpio_15,PIN_36_MUX_UART1_RTS
#endif
    cm_log_printf(0, "OPENCPU_UART_MPIN_76_87 cm_iomux_set_pin_func end \r\n");

    uart_test_init(OPENCPU_UART_MPIN_76_87, &config, &event);

}


//
void uart_mpin_init(cm_uart_dev_e dev, cm_uart_byte_size_e byte_size, cm_uart_parity_e parity,
                                cm_uart_stop_bit_e stop_bit, uint32_t baudrate, uint8_t func)
{
    int32_t ret = -1;
    cm_uart_cfg_t get_config = {0};
    cm_uart_event_t event = {0};

    if(dev != OPENCPU_UART_MPIN_38_39 && dev != OPENCPU_UART_MPIN_25_49 && dev != OPENCPU_MAIN_UART)
    {
        cm_demo_printf("uart_mpin_init err dev %d, dev 1/2/3!\r\n", dev);
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
    cm_demo_printf("uart NUM = %d demo start... ...\r\n", dev);
     /* 事件参数 */
    if(dev == OPENCPU_UART_MPIN_38_39)
    {
        event.event_type = CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW;   //注册需要上报的事件类型
        event.event_param = "uart1";    //用户参数
        event.event_entry = USER_TEST_UART_Callback;    //上报事件的回调函数

        cm_demo_printf("please open uart poart C \r\n ");
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

        cm_demo_printf("please open uart poart\r\n ");
        /* config mpin 49/25 iomux func3(UART3 RX/TX) */
        cm_iomux_set_pin_func(CM_IOMUX_PIN_49, CM_IOMUX_FUNC_FUNCTION3);
        cm_iomux_set_pin_func(CM_IOMUX_PIN_25, CM_IOMUX_FUNC_FUNCTION3);
        cm_log_printf(0, "OPENCPU_UART_MPIN_25_49 cm_iomux_set_pin_func end \r\n");
    }
    else if(dev == OPENCPU_MAIN_UART)
    {

        event.event_type = CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW;   //注册需要上报的事件类型
        event.event_param = "uart2";    //用户参数
        event.event_entry = USER_TEST_UART_Callback;    //上报事件的回调函数

        cm_demo_printf("uart NUM = %d demo start... ...\r\n", OPENCPU_MAIN_UART);
        cm_demo_printf("please open uart poart D \r\n ");

        /* 配置引脚复用 */
        /* uart2服用到 PD_GPIO_6 PD_GPIO_7 引脚      */
        cm_iomux_set_pin_func(CM_IOMUX_PIN_28, CM_IOMUX_FUNC_FUNCTION2);
        cm_iomux_set_pin_func(CM_IOMUX_PIN_29, CM_IOMUX_FUNC_FUNCTION2);
        cm_log_printf(0, "cm_iomux_set_pin_func end \r\n");
    }

    uart_test_init(dev, &config, &event);
}
void uart_mpin_1718_init(void)
{
    int32_t ret = -1;
    cm_uart_cfg_t get_config = {0};

    /* 配置参数 */
    cm_uart_cfg_t config =
    {
        CM_UART_BYTE_SIZE_8,
        CM_UART_PARITY_NONE,
        CM_UART_STOP_BIT_ONE,
        CM_UART_FLOW_CTRL_NONE,
        CM_UART_BAUDRATE_115200,
        0,   //配置为普通串口模式，若要配置为低功耗模式可改为1
        RXRB_BUF_LEN,   //环形缓存区大小按照默认配置8k
        FC_HIGN_THRESH,
        FC_LOW_THRESH,
    };

    /* 事件参数 */
    cm_uart_event_t event =
    {
        CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW|CM_UART_EVENT_TYPE_RX_FLOWCTRL,   //注册需要上报的事件类型
        "uart0",                                                        //用户参数
        USER_TEST_UART_Callback                                              //上报事件的回调函数
    };

    cm_demo_printf("uart NUM = %d demo start... ...\r\n", OPENCPU_UART_MPIN_17_18);
    cm_demo_printf("please open uart poart A \r\n ");

    /* config mpin 17/18 iomux func3(UART1 RX/TX) */
    cm_iomux_set_pin_func(CM_IOMUX_PIN_17, CM_IOMUX_FUNC_FUNCTION0);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_18, CM_IOMUX_FUNC_FUNCTION0);
    cm_log_printf(0, "OPENCPU_UART_MPIN_17_18 cm_iomux_set_pin_func end \r\n");

    uart_test_init(OPENCPU_UART_MPIN_17_18, &config, &event);
}


void uart_mpin_2549_init(void)
{

    int32_t ret = -1;
    cm_uart_cfg_t get_config = {0};

    /* 配置参数 */
    cm_uart_cfg_t config =
    {
        CM_UART_BYTE_SIZE_8,
        CM_UART_PARITY_NONE,
        CM_UART_STOP_BIT_ONE,
        CM_UART_FLOW_CTRL_NONE,
        CM_UART_BAUDRATE_115200,
        UART_FUNC_CONSOLE,   //配置为普通串口模式，若要配置为低功耗模式可改为1
        RXRB_BUF_LEN,   //环形缓存区大小按照默认配置8k
        FC_HIGN_THRESH,
        FC_LOW_THRESH,
    };

    /* 事件参数 */
    cm_uart_event_t event =
    {
        CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW|CM_UART_EVENT_TYPE_RX_FLOWCTRL,   //注册需要上报的事件类型
        "uart3",                                                        //用户参数
        USER_TEST_UART_Callback                                              //上报事件的回调函数
    };

    cm_demo_printf("uart NUM = %d demo start... ...\r\n", OPENCPU_UART_MPIN_25_49);
    cm_demo_printf("please open uart poart  \r\n ");

    /* config mpin 49/25 iomux func3(UART3 RX/TX) */
    cm_iomux_set_pin_func(CM_IOMUX_PIN_49, CM_IOMUX_FUNC_FUNCTION3);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_25, CM_IOMUX_FUNC_FUNCTION3);
    cm_log_printf(0, "OPENCPU_UART_MPIN_25_49 cm_iomux_set_pin_func end \r\n");

    uart_test_init(OPENCPU_UART_MPIN_25_49, &config, &event);
}


void uart_test_case_mpin_28_29(void)
{
    int32_t ret = -1;
    //uint8_t data[2];
    uint8_t *p_send = "HELLO";
    uint8_t data_rec[UART_RX_LEN + 1] = {0};
    cm_uart_cfg_t get_config;


    /* 配置参数 */
    cm_uart_cfg_t config =
    {
        CM_UART_BYTE_SIZE_8,
        CM_UART_PARITY_NONE,
        CM_UART_STOP_BIT_ONE,
        CM_UART_FLOW_CTRL_NONE,
        CM_UART_BAUDRATE_115200,
        0,   //配置为普通串口模式，若要配置为低功耗模式可改为1
        RXRB_BUF_LEN,   //环形缓存区大小按照默认配置8k
        FC_HIGN_THRESH,
        FC_LOW_THRESH,
    };

    /* 事件参数 */
    cm_uart_event_t event =
    {
        CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW,   //注册需要上报的事件类型
        "uart2",                                                        //用户参数
        USER_TEST_UART_Callback                                              //上报事件的回调函数
    };

    cm_demo_printf("uart NUM = %d demo start... ...\r\n", OPENCPU_MAIN_UART);
    cm_demo_printf("please open uart poart D \r\n ");

    /* 配置引脚复用 */
    /* uart2服用到 PD_GPIO_6 PD_GPIO_7 引脚      */
    //PIN_SetMux(PIN_RES(PIN_27), PIN_27_MUX_UART2_RX);
    //PIN_SetMux(PIN_RES(PIN_28), PIN_28_MUX_UART2_TX);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_28, CM_IOMUX_FUNC_FUNCTION2);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_29, CM_IOMUX_FUNC_FUNCTION2);
    cm_log_printf(0, "cm_iomux_set_pin_func end \r\n");

    uart_test_init(OPENCPU_MAIN_UART, &config, &event);

    cm_log_printf(0, "cm_uart_write start... ...\r\n");
    ret = cm_uart_write(OPENCPU_MAIN_UART,p_send, strlen(p_send), 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_write err,ret=%d\n", ret);
        return;
    }
    else
    {
        cm_log_printf(0, "cm_uart_write success \r\n");
    }
    // g_uart_rx_task = cm_thread_create("uart_rx", 2048, UART_TASK_PRIORITY, cm_uart_recv_task);
    //     /* 以下为串口接收示例，不影响串口配置，用户可酌情参考 */
    // osThreadAttr_t uart_task_attr = {0};
    // uart_task_attr.name = "uart_task";
    // uart_task_attr.stack_size = 2048;
    // uart_task_attr.priority= UART_TASK_PRIORITY;

    cm_log_printf(0, "cm_uart_read start... ...\n");
    ret = cm_uart_read(OPENCPU_MAIN_UART, data_rec, UART_RX_LEN, 0);
    if (ret < 0)
    {
        cm_log_printf(0, "cm_uart_read err,ret=%d\n", ret);
        return;
    }
    else
    {
        while(ret == 0)
        {
            ret = cm_uart_read(OPENCPU_MAIN_UART, data_rec, UART_RX_LEN, 0);
            if(ret > 0)
            {
                cm_log_printf(0, "cm_uart_read success  receive date count is %d\r\n", ret);
                for (uint8_t i = 0; i < ret; i++)
                {
                    cm_log_printf(0, "data_rec[%d] = %x \r\n", i,data_rec[i]);
                }
                cm_log_printf(0, "data_rec[%s] \r\n", data_rec);
            }
            cm_delay(1000);

        }

        cm_log_printf(0, "cm_uart_read test end \r\n");
    }
    cm_log_printf(0, "cm_uart_close start... ...\r\n");
    ret = cm_uart_close(OPENCPU_MAIN_UART);
    if (ret != RET_SUCCESS)
    {
        cm_log_printf(0, "cm_uart_close err,ret=%d\n", ret);
        return;
    }
    else
    {
        cm_log_printf(0, "cm_uart_close success\n");
    }
}

void uart_mpin_2829_init(void)
{
    int32_t ret = -1;
    cm_uart_cfg_t get_config;

    /* 配置参数 */
    cm_uart_cfg_t config =
    {
        CM_UART_BYTE_SIZE_8,
        CM_UART_PARITY_NONE,
        CM_UART_STOP_BIT_ONE,
        CM_UART_FLOW_CTRL_NONE,
        CM_UART_BAUDRATE_115200,
        UART_FUNC_CONSOLE,   //配置为普通串口模式，若要配置为低功耗模式可改为1
        RXRB_BUF_LEN,   //环形缓存区大小按照默认配置8k
        FC_HIGN_THRESH,
        FC_LOW_THRESH,
    };

    /* 事件参数 */
    cm_uart_event_t event =
    {
        CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW,   //注册需要上报的事件类型
        "uart2",                                                        //用户参数
        USER_TEST_UART_Callback                                              //上报事件的回调函数
    };

    cm_demo_printf("uart NUM = %d demo start... ...\r\n", OPENCPU_MAIN_UART);
    cm_demo_printf("please open uart poart D \r\n ");

    /* 配置引脚复用 */
    cm_iomux_set_pin_func(CM_IOMUX_PIN_28, CM_IOMUX_FUNC_FUNCTION2);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_29, CM_IOMUX_FUNC_FUNCTION2);
    cm_log_printf(0, "cm_iomux_set_pin_func end \r\n");

    uart_test_init(OPENCPU_MAIN_UART, &config, &event);
}

//测试3839_2829两个串口同时收发
static void test_uart3839_2829(void)
{
    uart_mpin_2829_init();
    uart_mpin_init(OPENCPU_UART_MPIN_38_39,CM_UART_BYTE_SIZE_8, CM_UART_PARITY_NONE, CM_UART_STOP_BIT_ONE,
                    CM_UART_BAUDRATE_115200, UART_FUNC_CONSOLE);

    if(g_uart_test_task == NULL)
    {
        osThreadAttr_t attr = {0};
        attr.name       = "uart_test";
        attr.stack_size = 2048;
        attr.priority   = UART_TASK_PRIORITY;
        g_uart_test_task = osThreadNew(cm_uart_test_task, NULL, &attr);
    }

    if (uart_event_queue == NULL)
    {
        uart_event_queue = osMessageQueueNew(EVENT_QUEUE_NUM, sizeof(uart_event_msg_t), NULL);
    }
}

static void close_uart3839_2829(void)
{
    cm_log_printf(0, "at_close_uart3839_2829_test start... ...\r\n");
    close_test_uart(OPENCPU_UART_MPIN_38_39);
    close_test_uart(OPENCPU_MAIN_UART);

    osThreadTerminate(g_uart_test_task);
}

static void at_test_uart3839_2829(uint8_t id)
{
    test_uart3839_2829();
    cm_uart_printf_result(id, 0);
    return;
}

static void at_close_uart3839_2829(uint8_t id)
{
    close_uart3839_2829();
    cm_uart_printf_result(id, 0);
    return;
}
#if 0
#include <drv_uart.h>
#include <drv_pin.h>


UART_Handle g_uartHandle_0;

osCompletion uartCmpl_tx;
osCompletion uartCmpl_rx;

static ALIGN(32) char           data_save[ 2048 ];

ALIGN(OS_CACHE_LINE_SZ) uint8_t data_send[OS_CACHE_LINE_SZ] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x12,0x13,0x14};
ALIGN(OS_CACHE_LINE_SZ) uint8_t data_recv[OS_CACHE_LINE_SZ] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

static void TEST_UART_Callback(void *hUart, uint32_t event)
{

    if(((UART_Handle *)hUart)->userData)
    {
        osComplete(((UART_Handle *)hUart)->userData);
    }

    if(((event & UART_EVENT_SEND_COMPLETE) !=0) && (((UART_Handle *)hUart)->userData == OS_NULL))
    {
      osPrintf("UART_EVENT_SEND_COMPLETE\r\n");
      osComplete(&uartCmpl_tx);
    }

    if(((event & UART_EVENT_RECEIVE_COMPLETE) !=0)  &&  (((UART_Handle *)hUart)->userData == OS_NULL))
    {
      osPrintf("UART_EVENT_RECEIVE_COMPLETE\r\n");
      osComplete(&uartCmpl_rx);
    }
}

void  TEST_UART_0_FIFO(char argc, char **argv)
{
    osCompletion uartCmpl;
    UART_Handle *huart = &g_uartHandle_0;
    uint32_t control = 0;
    memset(&g_uartHandle_0, 0x0, sizeof(g_uartHandle_0));

	huart->pRes = (void *)DRV_RES(UART, 1);

	/*设置引脚复用*/
	//PIN_SetMux(PIN_RES(PIN_5), PIN_5_MUX_LPUART0_RX);
	//PIN_SetMux(PIN_RES(PIN_6), PIN_6_MUX_LPUART0_TX);
	/* 配置引脚复用 */
    /* uart1服用到 PD_GPIO_12 PD_GPIO_13 引脚      */
    PIN_SetMux(PIN_RES(PIN_33), PIN_33_MUX_UART1_RX);
    PIN_SetMux(PIN_RES(PIN_34), PIN_34_MUX_UART1_TX);
    PIN_SetMux(PIN_RES(PIN_35), PIN_35_MUX_UART1_CTS);
    PIN_SetMux(PIN_RES(PIN_36), PIN_36_MUX_UART1_RTS);

    UART_Initialize(huart, TEST_UART_Callback);

    /*回调功能初始化*/
    osInitCompletion(&uartCmpl);
    huart->userData = &uartCmpl;
    huart->capabilities.flow_control_cts = 1;
    huart->capabilities.flow_control_rts = 1;
    UART_PowerControl(huart, DRV_POWER_FULL);
    UART_Control(huart, UART_FLOW_CONTROL_RTS_CTS, 0);
    control = UART_DATA_BITS_8| UART_PARITY_NONE | UART_STOP_BITS_1;
    UART_Control(huart, control, 115200);
    osPrintf("control 0x%x\r\n", control);
    UART_STATUS status = {0};
    UART_GetStatus(huart, &status);
    osPrintf("wordLen %d, parity %d, stop_bit %d, flow_ctrl %d, baudRate =%d setBauderate = %d\r\n", status.wordLen,
            status.parity, status.stopBit, status.flowControl, status.baudRate, status.setBauderate);

    UART_Send(huart, data_send, sizeof(data_send));
    if (osWaitForCompletion(huart->userData, osWaitForever))
    {
    }
    for (int i = 0; i < UART_GetTxCount(huart);i++)
    {
		osPrintf("send[%d] 0x%x\r\n", i,data_send[ i ]);
    }

    UART_Receive(huart, data_recv, sizeof(data_recv));
    if (osWaitForCompletion(huart->userData, osWaitForever))
    {
    }
    for (int i = 0; i < UART_GetRxCount(huart);i++)
    {
		osPrintf("recv[%d] 0x%x\r\n", i,data_recv[ i ]);
    }
	osDelay(100);
    UART_PowerControl(huart, DRV_POWER_OFF);
}
NR_SHELL_CMD_EXPORT(testUartFIFO, TEST_UART_0_FIFO);

#endif
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
static void uart_test_case_mpin_2549(void)
{
    uart_mpin_2549_init();
    get_uart_send_stat(OPENCPU_UART_MPIN_25_49);
    test_uart_send(OPENCPU_UART_MPIN_25_49);
    test_uart_read(OPENCPU_UART_MPIN_25_49);
    test_uart_clean(OPENCPU_UART_MPIN_25_49);
    close_test_uart(OPENCPU_UART_MPIN_25_49);
}
static void uart_test_case_mpin_1718(void)
{
    uart_mpin_1718_init();  //需要在board.c中把AT口屏蔽掉,并在cm_extern_psm_init中int ret = cm_extern_set_sleepmode(0, 0);
    get_uart_send_stat(OPENCPU_UART_MPIN_17_18);
    test_uart_send(OPENCPU_UART_MPIN_17_18);
    test_uart_read(OPENCPU_UART_MPIN_17_18);
    test_uart_clean(OPENCPU_UART_MPIN_17_18);
    close_test_uart(OPENCPU_UART_MPIN_17_18);
}

/* 若要修改测试串口可在cm_common.h中修改宏定义 */
void SHELL_testUart(char argc, char **argv)
{
    static cm_uart_dev_e dev = 0;

    if ( argc > 1)
    {
        const char *sub_cmd = argv[1];
        if (0 == strcmp(sub_cmd, "7687"))
        {
            uart_test_case_mpin_76_87();    //需要在board.c中把log口屏蔽掉
        }
        else if (0 == strcmp(sub_cmd, "2829"))
        {
            uart_test_case_mpin_28_29();        ////需要在board.c中把SHELL口屏蔽掉
        }
        else if (0 == strcmp(sub_cmd, "3839"))
        {
            uart_test_case_mpin_38_39();    //需要在board.c中把log口屏蔽掉
        }
        else if(0 == strcmp(sub_cmd, "1718"))
        {
            uart_test_case_mpin_1718();//需要在board.c中把AT口屏蔽掉,并在cm_extern_psm_init中int ret = cm_extern_set_sleepmode(0, 0);
        }
        else if (0 == strcmp(sub_cmd, "2549"))
        {
            uart_test_case_mpin_2549();    //全功能板硬件不支持
        }
        else if(0 == strcmp(sub_cmd, "close"))
        {
            if(dev == OPENCPU_UART_MPIN_38_39)
            {
                close_test_uart(OPENCPU_UART_MPIN_38_39);   //需要在board.c中把log口屏蔽掉
                if(g_uart_rx_task != NULL)
                {
                    osThreadTerminate(g_uart_rx_task);
                }
            }
            else if(dev == OPENCPU_UART_MPIN_25_49)
            {
                close_test_uart(OPENCPU_UART_MPIN_25_49);
            }
            else if(dev == OPENCPU_MAIN_UART)
            {
                close_test_uart(OPENCPU_MAIN_UART);
            }
            else
            {
                cm_log_printf(0,"err dev %d\r\n",dev);
                return;
            }

        }
        else if(0 == strcmp(sub_cmd, "init"))
        {

            if(argc < 7)
            {
                cm_log_printf(0,"parm err ,eg:cm_uart init dev byte_size parity stop_bit baudrate\r\n");
                cm_log_printf(0, " cm_uart init 1/3 8 0 0 115200 0\r\n");
                return;
            }
            dev = _atoi(&argv[2]);
            cm_uart_byte_size_e byte_size = _atoi(&argv[3]);
            //cm_log_printf(0, "byte_size %d\n",byte_size);
            cm_uart_parity_e parity = _atoi(&argv[4]);
            cm_uart_stop_bit_e stop_bit = _atoi(&argv[5]);
            uint32_t baudrate = _atoi(&argv[6]);
            uint8_t func = _atoi(&argv[7]);
            uart_mpin_init(dev, byte_size, parity, stop_bit, baudrate, func); //需要在board.c中把log口屏蔽掉
        }
        else if(0 == strcmp(sub_cmd, "send"))
        {
            if(dev == OPENCPU_UART_MPIN_38_39)
            {
                test_uart_send(OPENCPU_UART_MPIN_38_39);    //需要在board.c中把log口屏蔽掉
            }
            else if(dev == OPENCPU_UART_MPIN_25_49)
            {
                test_uart_send(OPENCPU_UART_MPIN_25_49);
            }
            else if(dev == OPENCPU_MAIN_UART)
            {
                test_uart_send(OPENCPU_MAIN_UART);
            }
            else
            {
                cm_log_printf(0,"err dev %d\r\n",dev);
                return;
            }

        }
        else if(0 == strcmp(sub_cmd, "read"))
        {
            if(dev == OPENCPU_UART_MPIN_38_39)
            {
                test_uart_read(OPENCPU_UART_MPIN_38_39);    //需要在board.c中把log口屏蔽掉
            }
            else if(dev == OPENCPU_UART_MPIN_25_49)
            {
                test_uart_read(OPENCPU_UART_MPIN_25_49);
            }
            else if(dev == OPENCPU_MAIN_UART)
            {
                test_uart_read(OPENCPU_MAIN_UART);
            }
            else
            {
                cm_log_printf(0,"err dev %d\r\n",dev);
                return;
            }

        }
        else if(0 == strcmp(sub_cmd, "clean"))
        {
            if(argc < 2)
            {
                cm_log_printf(0,"parm err ,eg:cm_uart clean dev\r\n");
                cm_log_printf(0, " cm_uart clean 0\1\2\3\r\n");
                return;
            }
            cm_uart_dev_e c_dev = _atoi(&argv[2]);
            if(c_dev > 3)
            {
                cm_log_printf(0, " cm_uart clean 0\1\2\3\r\n");
                return;
            }
            test_uart_clean(c_dev);    //需要在board.c中把log口屏蔽掉

        }
        else if(0 == strcmp(sub_cmd, "7687init"))
        {
            uart_mpin_7687_init();  //需要在board.c中把log口屏蔽掉,并把chip_pin_list中复用uart1的管脚改为GPIO
        }
        else if(0 == strcmp(sub_cmd, "7687send"))
        {
            test_uart_send(OPENCPU_UART_MPIN_76_87);    //需要在board.c中把log口屏蔽掉
        }
        else if(0 == strcmp(sub_cmd, "7687read"))
        {
            test_uart_read(OPENCPU_UART_MPIN_76_87);    //需要在board.c中把log口屏蔽掉
        }
        else if(0 == strcmp(sub_cmd, "7687clear"))
        {
            test_uart_clean(OPENCPU_UART_MPIN_76_87);    //需要在board.c中把log口屏蔽掉
        }
        else if(0 == strcmp(sub_cmd, "7687s_stat"))
        {
            get_uart_send_stat(OPENCPU_UART_MPIN_76_87);    //需要在board.c中把log口屏蔽掉
        }
        else if(0 == strcmp(sub_cmd, "7687close"))
        {
            close_test_uart(OPENCPU_UART_MPIN_76_87);   //需要在board.c中把log口屏蔽掉
        }
        else if(0 == strcmp(sub_cmd, "test"))
        {
            test_uart3839_2829();       //需要在board.c中把log口屏蔽掉,并把A口换成shell口
        }
        else if(0 == strcmp(sub_cmd, "test_close"))
        {
            close_uart3839_2829();  //需要在board.c中把log口屏蔽掉,并把A口换成shell口，并在cm_extern_psm_init中int ret = cm_extern_set_sleepmode(0, 0);
        }
        else
        {
            uart_test_case_mpin_38_39();    //需要在board.c中把log口屏蔽掉
        }
    }
    else
    {
        uart_test_case_mpin_38_39();    //需要在board.c中把log口屏蔽掉
    }
}

#include "nr_micro_shell.h"

NR_SHELL_CMD_EXPORT(cm_uart, SHELL_testUart);


#endif
#endif
