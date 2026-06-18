#include "yopen_uart.h"
#include "yopen_gpio.h"
#include "yopen_os.h"
#include "yopen_type.h"
#include "yopen_debug.h"
#include <stdio.h>
#include <string.h>

#define DEMO_UART_TRACE(fmt, ...) yopen_trace("[UART]"fmt, ##__VA_ARGS__)


#ifdef TYPE_EC718M
//uart 0
#define YOPEN_UART0_RX_PIN               YOPEN_PIN_GPIO16
#define YOPEN_UART0_RX_FUNC                1
#define YOPEN_UART0_TX_PIN                YOPEN_PIN_GPIO17
#define YOPEN_UART0_TX_FUNC               1

//uart1
#define YOPEN_UART1_RX_PIN               YOPEN_PIN_GPIO18
#define YOPEN_UART1_RX_FUNC                1
#define YOPEN_UART1_TX_PIN                YOPEN_PIN_GPIO19
#define YOPEN_UART1_TX_FUNC               1

//uart2
#define YOPEN_UART2_RX_PIN               YOPEN_PIN_GPIO10
#define YOPEN_UART2_RX_FUNC                3
#define YOPEN_UART2_TX_PIN                YOPEN_PIN_GPIO11
#define YOPEN_UART2_TX_FUNC               3

//uart3
#define YOPEN_UART3_RX_PIN               YOPEN_PIN_GPIO34
#define YOPEN_UART3_RX_FUNC                3
#define YOPEN_UART3_TX_PIN                YOPEN_PIN_GPIO35
#define YOPEN_UART3_TX_FUNC               3
#else
//uart 0
#define YOPEN_UART0_RX_PIN               YOPEN_PIN_GPIO6
#define YOPEN_UART0_RX_FUNC                1
#define YOPEN_UART0_TX_PIN                YOPEN_PIN_GPIO7
#define YOPEN_UART0_TX_FUNC               1

//uart1
#define YOPEN_UART1_RX_PIN               YOPEN_PIN_GPIO8
#define YOPEN_UART1_RX_FUNC                1
#define YOPEN_UART1_TX_PIN                YOPEN_PIN_GPIO9
#define YOPEN_UART1_TX_FUNC               1

//uart2
#define YOPEN_UART2_RX_PIN               YOPEN_PIN_GPIO4
#define YOPEN_UART2_RX_FUNC                2
#define YOPEN_UART2_TX_PIN                YOPEN_PIN_GPIO5
#define YOPEN_UART2_TX_FUNC               2
#endif

//#define YOPEN_UART_PORT_0_TEST_DEMO  //用于调试，不建议使用，影响后面debug
#define YOPEN_UART_PORT_1_TEST_DEMO
#define YOPEN_UART_PORT_2_TEST_DEMO
#define YOPEN_UART_PORT_3_TEST_DEMO

#define YOPEN_USB_PORT_TEST_DEMO

//#define YOPEN_UART_PORT_1_AT_MODE_TEST_DEMO
#ifdef YOPEN_UART_PORT_1_AT_MODE_TEST_DEMO
#undef YOPEN_UART_PORT_1_TEST_DEMO
#endif

#define YOPEN_UART_RX_BUFF_SIZE                512
#define YOPEN_UART_TX_BUFF_SIZE                512
#define MIN(a,b) ((a) < (b) ? (a) : (b))

yopen_queue_t  queue_ref;

typedef struct 
{
	yopen_uart_port_number_e port;
    int evet;
    int size;
} yopen_fota_msg_msg_s;

static int total_len;
unsigned char rxbuff[YOPEN_UART_RX_BUFF_SIZE];

//中断回调函数
void yopen_uart_notify_cb(uint32 ind_type, yopen_uart_port_number_e port, uint32 size)
{
    yopen_fota_msg_msg_s msg;

    memset(&msg, 0, sizeof(msg));
    msg.evet = ind_type;
	msg.port = port;

    switch(ind_type)
    {
        case YOPEN_UART_RX_RECV_DATA_IND:
        {
            msg.size = size;
            break;
        }
        default:
            break;
    }
    yopen_rtos_queue_release(queue_ref, sizeof(msg), (uint8*)&msg, YOPEN_NO_WAIT);
}

void yopen_uart_demo_thread(void *arvg)
{
	int ret = 0;
	yopen_uart_config_s uart_config = {0};
	uart_config.baudrate = YOPEN_UART_BAUD_115200;
	uart_config.data_bit = YOPEN_UART_DATABIT_8;
	uart_config.flow_ctrl = YOPEN_FC_NONE;
	uart_config.stop_bit = YOPEN_UART_STOP_1;
	uart_config.parity_bit = YOPEN_UART_PARITY_NONE;
	uart_config.isPrintfPort = TRUE;

	yopen_rtos_task_sleep_ms(2000);
	DEMO_UART_TRACE("========== uart demo start ==========");

	DEMO_UART_TRACE("==========Uart Printf Demo Init: Baudrate-%d ==========\r\n", uart_config.baudrate);
	
	#ifdef YOPEN_UART_PORT_0_TEST_DEMO
	uint32_t value;
	//表示获取log输出的方式，设置0时，仅从USB输出log；设置1，则仅从debug uart口输出log；设置2，则开机先从debug uart口输出log，待USB初始化后自动转到USB输出log
	ret = yopen_log_port_get(&value);
	DEMO_UART_TRACE("==========yopen_log_port_get port %d==========\r\n", value);
	
	//将log port设置为仅USB输出
	yopen_log_port_set(0);

	yopen_pin_set_func(YOPEN_UART0_RX_PIN, YOPEN_UART0_RX_FUNC);
	yopen_pin_set_func(YOPEN_UART0_TX_PIN, YOPEN_UART0_TX_FUNC);
	
	yopen_uart_set_dcbconfig(YOPEN_UART_PORT_0, &uart_config);

	ret = yopen_uart_open(YOPEN_UART_PORT_0);

	if(ret == YOPEN_UART_SUCCESS)
	{
		yopen_uart_register_cb(YOPEN_UART_PORT_0, yopen_uart_notify_cb);
	}
	else
	{
		DEMO_UART_TRACE("========== yopen_uart_open error !!! ==========");
	}
	#endif

	#ifdef YOPEN_UART_PORT_1_AT_MODE_TEST_DEMO
	yopen_uart_mode_e mode = YOPEN_UART_MODE_AT;
	ret = yopen_uart_ioctl(YOPEN_UART_PORT_1, YOPEN_UART_IOCTL_SET_CCIO_MODE, &mode);
	DEMO_UART_TRACE("========== yopen_uart_ioctl YOPEN_UART_PORT_1 ret %d ==========", ret);
	#endif

	#ifdef YOPEN_UART_PORT_1_TEST_DEMO
	yopen_pin_set_func(YOPEN_UART1_RX_PIN, YOPEN_UART1_RX_FUNC);
	yopen_pin_set_func(YOPEN_UART1_TX_PIN, YOPEN_UART1_TX_FUNC);
	
	yopen_uart_set_dcbconfig(YOPEN_UART_PORT_1, &uart_config);

	ret = yopen_uart_open(YOPEN_UART_PORT_1);

	if(ret == YOPEN_UART_SUCCESS)
	{
		yopen_uart_register_cb(YOPEN_UART_PORT_1, yopen_uart_notify_cb);
	}
	else
	{
		DEMO_UART_TRACE("==========yopen_uart_open error !!! ==========");
	}
	#endif
	#ifdef YOPEN_UART_PORT_2_TEST_DEMO
	yopen_pin_set_func(YOPEN_UART2_RX_PIN, YOPEN_UART2_RX_FUNC);
	yopen_pin_set_func(YOPEN_UART2_TX_PIN, YOPEN_UART2_TX_FUNC);

	yopen_uart_set_dcbconfig(YOPEN_UART_PORT_2, &uart_config);

	ret = yopen_uart_open(YOPEN_UART_PORT_2);

	if(ret == YOPEN_UART_SUCCESS)
	{
		yopen_uart_register_cb(YOPEN_UART_PORT_2, yopen_uart_notify_cb);
	}
	else
	{
		DEMO_UART_TRACE("========== yopen_uart_open error !!! ==========");
	}
	#endif

	#if defined(TYPE_EC718M) && defined(YOPEN_UART_PORT_3_TEST_DEMO)

	yopen_pin_set_func(YOPEN_UART3_RX_PIN, YOPEN_UART3_RX_FUNC);
	yopen_pin_set_func(YOPEN_UART3_TX_PIN, YOPEN_UART3_TX_FUNC);

	yopen_uart_set_dcbconfig(YOPEN_UART_PORT_3, &uart_config);

	ret = yopen_uart_open(YOPEN_UART_PORT_3);
	DEMO_UART_TRACE("========== yopen_uart_open YOPEN_UART_PORT_3 ret %d ==========", ret);
	
	if(ret == YOPEN_UART_SUCCESS)
	{
		yopen_uart_register_cb(YOPEN_UART_PORT_3, yopen_uart_notify_cb);
	}
	else
	{
		DEMO_UART_TRACE("========== yopen_uart_open YOPEN_UART_PORT_3 error !!! ==========");
	}

	#endif

	#ifdef YOPEN_USB_PORT_TEST_DEMO

	yopen_uart_set_dcbconfig(YOPEN_USB_PORT_COM, &uart_config);

	ret = yopen_uart_open(YOPEN_USB_PORT_COM);

	if(ret == YOPEN_UART_SUCCESS)
	{
		yopen_uart_register_cb(YOPEN_USB_PORT_COM, yopen_uart_notify_cb);
	}
	else
	{
		DEMO_UART_TRACE("==========yopen_uart_open error !!! ==========");
	}
	#endif

    ret = yopen_rtos_queue_create(&queue_ref, sizeof(yopen_fota_msg_msg_s), 50);
    DEMO_UART_TRACE("========== yopen_rtos_queue_create ret %d, queue_ref %x ==========", ret, queue_ref);
    
    while(1)
    {
        yopen_fota_msg_msg_s msg;

        yopen_rtos_queue_wait(queue_ref, (uint8*)&msg, sizeof(msg), YOPEN_WAIT_FOREVER);
        DEMO_UART_TRACE("========== yopen_rtos_queue_wait evet %x, size %d ==========", msg.evet, msg.size);

#ifdef YOPEN_OVERFLOW_TEST
        yopen_rtos_task_sleep_ms(200);
#endif

        if (YOPEN_UART_RX_RECV_DATA_IND == msg.evet)
        {
            //回显测试
            int read_len = yopen_uart_read(msg.port, rxbuff, YOPEN_UART_RX_BUFF_SIZE);

            total_len += read_len;
            DEMO_UART_TRACE("========== yopen_uart_read PORT %d %d,%d/%d ==========", msg.port, msg.size, read_len, total_len);
      
            //等待上次发送结束
            do
            {
                ret = yopen_uart_write(msg.port, rxbuff, read_len);
            } while (ret == YOPEN_UART_BUSY);
        }
        else if (YOPEN_UART_RX_OVERFLOW_IND == msg.evet)
        {
            DEMO_UART_TRACE("========== YOPEN_UART_RX_OVERFLOW_IND ========== ");
        }
        else if (YOPEN_UART_TX_COMPLETE_APP_IND == msg.evet)
        {
            DEMO_UART_TRACE("========== YOPEN_UART_TX_COMPLETE_APP_IND ==========");
        }
    }
}
