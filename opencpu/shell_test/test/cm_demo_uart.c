/*********************************************************
 *  @file    cm_demo_uart.c
 *  @brief   OpenCPU UART示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot1325 2021/08/30
 ********************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "cm_uart.h"
#include "cm_demo_uart.h"
#include "cm_iomux.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "cm_os.h"
#include "cm_mem.h"
#include "cm_demo_common.h"
#include "cm_sys.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
 
#define UART_BUF_LEN            1024

/****************************************************************************
 * Private Types
 ****************************************************************************/
 
typedef struct{
    int msg_type;
} uart_event_msg_t;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
 
void cm_demo_printf(char *str, ...);
static void cm_uart_recv_task(void *param);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static osThreadId_t OC_Uart_TaskHandle = NULL; //串口数据接收、解析任务Handle
static void* g_uart_sem = NULL;
static osMessageQueueId_t uart_event_queue = NULL;
static osThreadId_t uart_event_thread = NULL;

extern osEventFlagsId_t g_cmd_task_flag;
 
/****************************************************************************
 * Private Functions
 ****************************************************************************/

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

/* 回调函数中不可输出LOG、串口打印、执行复杂任务或消耗过多资源，建议以信号量或消息队列形式控制其他线程执行任务 */
static void cm_serial_uart_callback(void *param, uint32_t type)
{
    uart_event_msg_t msg = {0};
    if (CM_UART_EVENT_TYPE_RX_ARRIVED & type)
    {
        /* 收到接收事件，触发其他线程执行读取数据 */
        osSemaphoreRelease(g_uart_sem);
    }
    
    if ((CM_UART_EVENT_TYPE_RX_OVERFLOW & type) || (CM_UART_EVENT_TYPE_RX_FLOWCTRL & type))
    {
        /* 收到溢出事件或流控事件，触发其他线程处理事件 */
        msg.msg_type = type;
        
        if (uart_event_queue != NULL)//向队列发送数据
        {
            osMessageQueuePut(uart_event_queue, &msg, 0, 0);
        }
    }
}

/* 串口接收示例，平时使用信号量挂起，当收到接收事件后，释放信号量以触发读取任务 */
static void cm_uart_recv_task(void *param)
{
    while (1)
    {
        if (g_uart_sem != NULL)
        {
            osSemaphoreAcquire(g_uart_sem, osWaitForever);//阻塞
        }
        
        osEventFlagsSet(g_cmd_task_flag, 0x00000001U);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* 从测试串口打印字符串 */
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
    cm_uart_write(OPENCPU_MAIN_URAT, s, len, 1000);
}

/* 若要修改测试串口可在cm_demo_common.h中修改宏定义 */
void cm_demo_uart(void)
{
    int32_t ret = -1;

    /* 配置参数 */
    cm_uart_cfg_t config = 
    {
        CM_UART_BYTE_SIZE_8, 
        CM_UART_PARITY_NONE,
        CM_UART_STOP_BIT_ONE, 
        CM_UART_FLOW_CTRL_NONE, 
        CM_UART_BAUDRATE_115200,
        0,   //配置为普通串口模式，若要配置为低功耗模式可改为1
        0,   //环形缓存区大小按照默认配置8k
        0,
        0,
        0,
        0,
    };

    /* 事件参数 */
    cm_uart_event_t event = 
    {
        CM_UART_EVENT_TYPE_RX_ARRIVED|CM_UART_EVENT_TYPE_RX_OVERFLOW,   //注册需要上报的事件类型
        "uart0",                                                        //用户参数
        cm_serial_uart_callback                                         //上报事件的回调函数
    };

    cm_log_printf(0, "uart NUM = %d demo start... ...", OPENCPU_MAIN_URAT);

    /* 配置引脚复用 */
    cm_iomux_set_pin_func(OPENCPU_TEST_UARTTX_IOMUX);
    cm_iomux_set_pin_func(OPENCPU_TEST_UARTRX_IOMUX);

    
    /* 若要开启流控，配置流控引脚复用 */
    if (config.flow_ctrl == CM_UART_FLOW_CTRL_HW)
    {
        cm_iomux_set_pin_func(OPENCPU_TEST_UARTRTS_IOMUX);
        cm_iomux_set_pin_func(OPENCPU_TEST_UARTCTS_IOMUX);
            
        /* 可注册流控事件上报 */
        event.event_type |= CM_UART_EVENT_TYPE_RX_FLOWCTRL;
    }

    /* 注册事件和回调函数 */
    ret = cm_uart_register_event(OPENCPU_MAIN_URAT, &event);
    
    if (ret != RET_SUCCESS)
    {
        cm_log_printf(0, "uart register event err,ret=%d\n", ret);
        return;
    }

    /* 若使能了串口唤醒，配置RX引脚 */
    if (config.rx_wakeup_en == 1)
    {
        config.rx_wakeup_pin = OPENCPU_TEST_UARTRX_PIN;
    }

    /* 开启串口 */
    ret = cm_uart_open(OPENCPU_MAIN_URAT, &config);
    
    if (ret != RET_SUCCESS)
    {
        cm_log_printf(0, "uart init err,ret=%d\n", ret);
        return;
    }

    /* 以下为串口接收示例，不影响串口配置，用户可酌情参考 */
    osThreadAttr_t uart_task_attr = {0};
    uart_task_attr.name = "uart_task";
    uart_task_attr.stack_size = 2048;
    uart_task_attr.priority= UART_TASK_PRIORITY;
    
    if (g_uart_sem == NULL)
    {
        g_uart_sem = osSemaphoreNew(1, 0, NULL);
    }

    OC_Uart_TaskHandle = osThreadNew(cm_uart_recv_task, 0, &uart_task_attr);

    uart_event_task_create();
}

/* 关闭串口 */
void cm_test_uart_close(char **cmd, int len)
{
    cm_uart_dev_e dev = atoi(cmd[2]);
    
    if (0 == cm_uart_close(dev))
    {
        cm_demo_printf("uart%d close is ok\n", dev);
    }
    
    else
    {
        cm_demo_printf("uart%d close is error\n", dev);
    }
}
