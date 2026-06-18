#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "yopen_debug.h"
#include "yopen_os.h"
#include "yopen_gpio.h"
#include "yopen_uart.h"

typedef enum
{
	GPIO_MSG_GPIO_CTRL,
	GPIO_MSG_GPIO_INIT,
	GPIO_MSG_UART_RECV,
} gpio_msg_s;

typedef enum
{
	GPIO_OUTPUT_LVL_HIGH,  //输出高电平
	GPIO_OUTPUT_LVL_LOW,  //输出低电平
	GPIO_INPUT_EDGE_INT_PULL_NONE, //输入中断，边沿触发方式，无上下拉
	GPIO_INPUT_EDGE_INT_PULL_UP, //输入中断，边沿触发方式，上拉
	GPIO_INPUT_EDGE_INT_PULL_DOWN, //输入中断，边沿触发方式，下拉
	GPIO_INPUT_LEVEL_INT_PULL_NONE, //输入中断，电平触发方式，无上下拉
	GPIO_INPUT_LEVEL_INT_PULL_UP, //输入中断，电平触发方式，上拉
	GPIO_INPUT_LEVEL_INT_PULL_DOWN //输入中断，电平触发方式，下拉
}gpio_ctl_mode_e;

//选择GPIO端口
static int s_gpio_port = -1; //yopen_GpioNum
//选择GPIO控制模式
static int s_gpio_mode = GPIO_OUTPUT_LVL_HIGH; //gpio_ctl_mode_e
//选择中断触发方式
static int s_gpio_int_trig_mode = EDGE_BOTH; 
//
static yopen_Lvl gpio_vol = Vol_1_80V;
//通过串口发送AT,配置GPIO端口
//AT+GPIO=<yopen_GpioNum>,<dir>[,level]
// dir : 0->output; 1->input_edge_int; 2->input_level_int
// level : 0->low; 1->high；没有该参数表示高阻态

//例如 AT+GPIO=2,0,1 //将GPIO2输出高电平
//选择AT端口,USB, U0,U1,U2
static int s_uart_port = YOPEN_USB_PORT_COM; //yopen_uart_port_number_e

#define DEMO_GPIO_TRACE(fmt, ...) yopen_trace("[GPIO]"fmt, ##__VA_ARGS__)

static yopen_queue_t s_queue;

static void demo_gpio_cb(void *param)
{
	int gpio = (int)param;
	gpio_msg_s msg;

	yopen_int_disable(gpio);
	msg = GPIO_MSG_GPIO_INIT;
	yopen_rtos_queue_release(s_queue, sizeof(msg), (uint8*)&msg, YOPEN_NO_WAIT);
}

static int parse_at_command(const char* input, char* command, int* param1, int* param2, int* param3) {
    // 检查输入是否为空
    if (input == NULL || command == NULL || param1 == NULL || param2 == NULL) {
        DEMO_GPIO_TRACE("Invalid input or output parameters.\n");
        return -1;
    }

    // 初始化变量
    *param1 = -1;
    *param2 = -1;
    command[0] = '\0';

    // 找到等号位置
    const char* equal_sign = strchr(input, '=');
    if (equal_sign == NULL) {
        DEMO_GPIO_TRACE("Invalid AT command format. No '=' found.\n");
        return -1;
    }

    // 提取命令部分
    int command_length = equal_sign - input;
    if (command_length > 0) {
        strncpy(command, input, command_length);
        command[command_length] = '\0'; // 确保字符串以null结尾
    } else {
        DEMO_GPIO_TRACE("Invalid command length.\n");
        return -1;
    }

    // 提取参数部分
    const char* params_start = equal_sign + 1;
    const char* comma = strchr(params_start, ',');
    if (comma == NULL) {
        DEMO_GPIO_TRACE("Invalid parameter format. No ',' found.\n");
        return -1;
    }

    // 提取第一个参数
    int param1_length = comma - params_start;
    char param1_str[param1_length + 1];
    strncpy(param1_str, params_start, param1_length);
    param1_str[param1_length] = '\0';
    *param1 = atoi(param1_str);

    // 提取第二个参数
    const char* param2_start = comma + 1;
    *param2 = atoi(param2_start);


    comma = strchr(param2_start, ',');

    if(comma)
    {
        const char* param3_start = comma + 1;
        *param3 = atoi(param3_start);
    }
    else
    {
        *param3 = 0xff;
    }

    

    return 0;
}


static void _uart_notify_cb(uint32 ind_type, yopen_uart_port_number_e port, uint32 size)
{
    gpio_msg_s msg;

    switch(ind_type)
    {
        case YOPEN_UART_RX_RECV_DATA_IND:
        {
            msg = GPIO_MSG_UART_RECV;
			DEMO_GPIO_TRACE("YOPEN_UART_RX_RECV_DATA_IND size %d", size);
			yopen_rtos_queue_release(s_queue, sizeof(msg), (uint8*)&msg, YOPEN_NO_WAIT);
            break;
        }
        default:
            break;
    }
    
}

void demo_uart_write(char *data)
{
	yopen_uart_write(s_uart_port, (unsigned char*)data, strlen(data));
}

void demo_uart_init(void)
{
	yopen_uart_config_s uart_config = {0};
	uart_config.baudrate = YOPEN_UART_BAUD_115200;
	uart_config.data_bit = YOPEN_UART_DATABIT_8;
	uart_config.flow_ctrl = YOPEN_FC_NONE;
	uart_config.stop_bit = YOPEN_UART_STOP_1;
	uart_config.parity_bit = YOPEN_UART_PARITY_NONE;
	uart_config.isPrintfPort = TRUE;

	switch(s_uart_port)
	{
		case YOPEN_UART_PORT_0:
		{
			yopen_pin_set_func(YOPEN_PIN_GPIO6, 1);
			yopen_pin_set_func(YOPEN_PIN_GPIO7, 1);
			break;
		}
		case YOPEN_UART_PORT_1:
		{
			yopen_pin_set_func(YOPEN_PIN_GPIO8, 1);
			yopen_pin_set_func(YOPEN_PIN_GPIO9, 1);
			break;
		}
		case YOPEN_UART_PORT_2:
		{
			yopen_pin_set_func(YOPEN_PIN_GPIO4, 2);
			yopen_pin_set_func(YOPEN_PIN_GPIO5, 2);
			break;
		}
	}

	yopen_uart_close(s_uart_port);
	yopen_uart_set_dcbconfig(s_uart_port, &uart_config);
	yopen_uart_open(s_uart_port);

	yopen_uart_register_cb(s_uart_port, _uart_notify_cb);
}

static void demo_pin_init(void)
{
#ifdef TYPE_EC718M
	if (s_gpio_port>=GPIO_20 && s_gpio_port<=GPIO_28)
	{
		yopen_aon_power_on();
	}
#else
	if (s_gpio_port>=GPIO_10 && s_gpio_port<=GPIO_16)
	{
		yopen_aon_power_on();
	}
#endif

	switch(s_gpio_port)
	{
		case GPIO_0: yopen_pin_set_func(YOPEN_PIN_GPIO0, 0); return;
		case GPIO_1: yopen_pin_set_func(YOPEN_PIN_GPIO1, 0); return;
		case GPIO_2: yopen_pin_set_func(YOPEN_PIN_GPIO2, 0); return;
		case GPIO_3: yopen_pin_set_func(YOPEN_PIN_GPIO3, 0); return;
		case GPIO_4: yopen_pin_set_func(YOPEN_PIN_GPIO4, 0); return;
		case GPIO_5: yopen_pin_set_func(YOPEN_PIN_GPIO5, 0); return;
		case GPIO_6: yopen_pin_set_func(YOPEN_PIN_GPIO6, 0); return;
		case GPIO_7: yopen_pin_set_func(YOPEN_PIN_GPIO7, 0); return;
		case GPIO_8: yopen_pin_set_func(YOPEN_PIN_GPIO8, 0); return;
		case GPIO_9: yopen_pin_set_func(YOPEN_PIN_GPIO9, 0); return;
		case GPIO_10: yopen_pin_set_func(YOPEN_PIN_GPIO10, 0); return;
		case GPIO_11: yopen_pin_set_func(YOPEN_PIN_GPIO11, 0); return;
		case GPIO_12: yopen_pin_set_func(YOPEN_PIN_GPIO12, 0); return;
		case GPIO_13: yopen_pin_set_func(YOPEN_PIN_GPIO13, 0); return;
		case GPIO_14: yopen_pin_set_func(YOPEN_PIN_GPIO14, 0); return;
		case GPIO_15: yopen_pin_set_func(YOPEN_PIN_GPIO15, 0); return;
		case GPIO_16: yopen_pin_set_func(YOPEN_PIN_GPIO16, 0); return;
#ifdef TYPE_EC718M
		case GPIO_17: yopen_pin_set_func(YOPEN_PIN_GPIO17, 0); return;
		case GPIO_18: yopen_pin_set_func(YOPEN_PIN_GPIO18, 0); return;
		case GPIO_19: yopen_pin_set_func(YOPEN_PIN_GPIO19, 0); return;
		case GPIO_20: yopen_pin_set_func(YOPEN_PIN_GPIO20, 0); return;
		case GPIO_21: yopen_pin_set_func(YOPEN_PIN_GPIO21, 0); return;
		case GPIO_22: yopen_pin_set_func(YOPEN_PIN_GPIO22, 0); return;
		case GPIO_23: yopen_pin_set_func(YOPEN_PIN_GPIO23, 0); return;
		case GPIO_24: yopen_pin_set_func(YOPEN_PIN_GPIO24, 0); return;
		case GPIO_25: yopen_pin_set_func(YOPEN_PIN_GPIO25, 0); return;
		case GPIO_26: yopen_pin_set_func(YOPEN_PIN_GPIO26, 0); return;
		case GPIO_27: yopen_pin_set_func(YOPEN_PIN_GPIO27, 0); return;
		//case GPIO_28: yopen_pin_set_func(YOPEN_PIN_GPIO28, 0); return; //内部占用
		case GPIO_29: yopen_pin_set_func(YOPEN_PIN_GPIO29, 0); return;
		case GPIO_30: yopen_pin_set_func(YOPEN_PIN_GPIO30, 0); return;
		case GPIO_31: yopen_pin_set_func(YOPEN_PIN_GPIO31, 0); return;
		case GPIO_32: yopen_pin_set_func(YOPEN_PIN_GPIO32, 0); return;
		case GPIO_33: yopen_pin_set_func(YOPEN_PIN_GPIO33, 0); return;
		case GPIO_34: yopen_pin_set_func(YOPEN_PIN_GPIO34, 0); return;
		case GPIO_35: yopen_pin_set_func(YOPEN_PIN_GPIO35, 0); return;
		case GPIO_36: yopen_pin_set_func(YOPEN_PIN_GPIO36, 0); return;
		case GPIO_37: yopen_pin_set_func(YOPEN_PIN_GPIO37, 0); return;
		case GPIO_38: yopen_pin_set_func(YOPEN_PIN_GPIO38, 0); return;
#else
		case GPIO_17: yopen_pin_set_func(YOPEN_PIN_GPIO17, 4); return;
		case GPIO_18: yopen_pin_set_func(YOPEN_PIN_GPIO18, 4); return;
		case GPIO_19: yopen_pin_set_func(YOPEN_PIN_GPIO19, 4); return;
		case GPIO_20: yopen_pin_set_func(YOPEN_PIN_GPIO20, 4); return;
		case GPI_0:
		case GPI_1:
		case GPI_2:
		case GPI_3:
		case GPI_4:
		case GPI_5:
#endif		
			yopen_aon_power_on();
			break;
	}
}

static bool demo_gpio_init(void)
{
	int ret = 0;
	yopen_GpioDir gpio_dir = GPIO_OUTPUT;
    yopen_PullMode gpio_pull = FORCE_PULL_NONE;
    yopen_LvlMode gpio_lvl = LVL_LOW;
	yopen_TriggerMode gpio_trigger = EDGE_TRIGGER;
	bool int_flag = false;

	switch(s_gpio_mode)
	{
		case GPIO_OUTPUT_LVL_HIGH:
		{
			gpio_dir = GPIO_OUTPUT;
			gpio_pull = FORCE_PULL_NONE;
			gpio_lvl = LVL_HIGH;
			
			break;
		}
		case GPIO_OUTPUT_LVL_LOW:
		{
			gpio_dir = GPIO_OUTPUT;
			gpio_pull = FORCE_PULL_NONE;
			gpio_lvl = LVL_LOW;
			break;
		}
		case GPIO_INPUT_EDGE_INT_PULL_NONE:
		{
			gpio_dir = GPIO_INPUT;
			gpio_pull = FORCE_PULL_NONE;
			gpio_lvl = LVL_LOW;
			int_flag = true;
			break;
		}
		case GPIO_INPUT_EDGE_INT_PULL_UP:
		{
			gpio_dir = GPIO_INPUT;
			gpio_pull = FORCE_PULL_UP;
			gpio_lvl = LVL_LOW;
			int_flag = true;
			break;
		}
		case GPIO_INPUT_EDGE_INT_PULL_DOWN:
		{
			gpio_dir = GPIO_INPUT;
			gpio_pull = FORCE_PULL_DOWN;
			gpio_lvl = LVL_LOW;
			int_flag = true;
			break;
		}
		case GPIO_INPUT_LEVEL_INT_PULL_NONE:
		{
			gpio_dir = GPIO_INPUT;
			gpio_pull = FORCE_PULL_NONE;
			gpio_lvl = LVL_LOW;
			int_flag = true;
			gpio_trigger = LEVEL_TRIGGER;
			break;
		}
		case GPIO_INPUT_LEVEL_INT_PULL_UP:
		{
			gpio_dir = GPIO_INPUT;
			gpio_pull = FORCE_PULL_UP;
			gpio_lvl = LVL_LOW;
			int_flag = true;
			gpio_trigger = LEVEL_TRIGGER;
			break;
		}
		case GPIO_INPUT_LEVEL_INT_PULL_DOWN:
		{
			gpio_dir = GPIO_INPUT;
			gpio_pull = FORCE_PULL_DOWN;
			gpio_lvl = LVL_LOW;
			int_flag = true;
			gpio_trigger = LEVEL_TRIGGER;
			break;
		}
	}

	ret |= yopen_gpio_init(s_gpio_port, gpio_dir, gpio_pull, gpio_lvl);
	DEMO_GPIO_TRACE("yopen_gpio_init gpio %d, ret %d", s_gpio_port, ret);
	

	if (int_flag)
	{
		ret |= yopen_int_register(s_gpio_port, gpio_trigger, DEBOUNCE_DIS, 
				s_gpio_int_trig_mode, gpio_pull, demo_gpio_cb, (void *)s_gpio_port);
		DEMO_GPIO_TRACE("yopen_int_register gpio %d, ret %d", s_gpio_port, ret);

		ret |= yopen_int_enable(s_gpio_port);
		DEMO_GPIO_TRACE("yopen_int_register gpio %d, ret %d", s_gpio_port, ret);
	}
	
	if (ret)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void yopen_gpio_demo_task(void * arg)
{
	int ret;
	gpio_msg_s msg;
	char data[64];

	yopen_rtos_task_sleep_ms(5000);

	yopen_gpio_set_voltage(gpio_vol);

	ret = yopen_rtos_queue_create(&s_queue, sizeof(gpio_msg_s), 50);

	msg = GPIO_MSG_GPIO_CTRL;
	yopen_rtos_queue_release(s_queue, sizeof(msg), (uint8*)&msg, YOPEN_NO_WAIT);

	demo_uart_init();

	while(1)
	{
		static int gpio = -1;

		ret = yopen_rtos_queue_wait(s_queue, (uint8*)&msg, sizeof(msg), YOPEN_WAIT_FOREVER);
		
		DEMO_GPIO_TRACE("yopen_rtos_queue_wait msg %d", msg);
		
		switch(msg)
		{
			case GPIO_MSG_GPIO_CTRL:
			{
                gpio = s_gpio_port;

				if (gpio != -1)
				{
					yopen_gpio_deinit(gpio);
				}

				demo_pin_init();
				ret = demo_gpio_init();

                if(gpio != -1)
                {
                    memset(data, 0, sizeof(data));
                    if(ret)
                    {
                        sprintf(data, "%s", "\r\nOK\r\n");
                    }
                    else
                    {
                        sprintf(data, "%s", "\r\nERROR\r\n");
                    }
                    demo_uart_write(data);
                }
				
				
				
				yopen_rtos_task_sleep_ms(100);
			}
			break;

			case GPIO_MSG_GPIO_INIT:
			{
				yopen_LvlMode gpio_lvl;

				yopen_rtos_task_sleep_ms(50);
				yopen_gpio_get_level(s_gpio_port, &gpio_lvl);
				yopen_int_enable(s_gpio_port);
				DEMO_GPIO_TRACE("+GPIO INT %d", gpio_lvl);

				memset(data, 0, sizeof(data));
				sprintf(data, "\r\n+GPIO INT %d\r\n", gpio_lvl);
				demo_uart_write(data);
				
				break;
			}

			case GPIO_MSG_UART_RECV:
			{
				char command[64];
				int param1, param2, param3 = 0Xff;

				memset(data, 0, sizeof(data));

				int read_len = yopen_uart_read(s_uart_port, (unsigned char *)data, sizeof(data));
				
				DEMO_GPIO_TRACE("yopen_uart_read len %d", read_len);

				
				if (read_len > 2)
				{
					for (int i=0; i<2; i++)
					{
						if (data[read_len-i] == 0xd)
						{
							data[read_len-i] = 0;
						}
					}

					parse_at_command(data, command, &param1, &param2, &param3);
					DEMO_GPIO_TRACE("GPIO_MSG_UART_RECV %s,cmmand %s,param1 %d,param2 %d, param3 %d", data,command, param1,param2, param3);
					s_gpio_port = param1;

                    if(param2 == 0) //输出
                    {
                        if(param3)
                        {
                            s_gpio_mode = GPIO_OUTPUT_LVL_HIGH;
                        }
                        else
                        {
                            s_gpio_mode = GPIO_OUTPUT_LVL_LOW;
                        }
                    }
					else if(param2 == 1) //输入边沿中断
                    {
                        if(param3 == 1)
                        {
                            s_gpio_mode = GPIO_INPUT_EDGE_INT_PULL_UP;
                        }
                        else if(param3 == 0xff)
                        {
                            s_gpio_mode = GPIO_INPUT_EDGE_INT_PULL_NONE;
                        }
                        else
                        {
                            s_gpio_mode = GPIO_INPUT_EDGE_INT_PULL_DOWN;
                        }
                    }
                    else if(param2 == 2) //输入电平中断
                    {
                        if(param3 == 1)
                        {
                            s_gpio_mode = GPIO_INPUT_LEVEL_INT_PULL_UP;
                        }
                        else if(param3 == 0xff)
                        {
                            s_gpio_mode = GPIO_INPUT_LEVEL_INT_PULL_NONE;
                        }
                        else
                        {
                            s_gpio_mode = GPIO_INPUT_LEVEL_INT_PULL_DOWN;
                        }
                    }
					else if(3 == param2) // 获取状态
                    {
                        yopen_LvlMode gpio_lvl;
                        yopen_gpio_get_level(s_gpio_port, &gpio_lvl);
                        memset(data, 0, sizeof(data));
                        sprintf(data, "\r\n+GPIO LVL %d\r\n", gpio_lvl);
                        demo_uart_write(data);
                    }

					msg = GPIO_MSG_GPIO_CTRL;
					yopen_rtos_queue_release(s_queue, sizeof(msg), (uint8*)&msg, YOPEN_NO_WAIT);
					
				}
				
				break;
			}
		}
		
	}
}
