/*********************************************************
 *  @file    cm_demo_ping.c
 *  @brief   OpenCPU ping示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created By SMR 2024/3/28
 ********************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#ifdef CM_DEMO_PING_SUPPORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cm_ping.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define cm_demo_printf osPrintf
#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)
#ifndef bool
#define bool int
#endif


/****************************************************************************
 * Private Types
 ****************************************************************************/


/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/

static char *__user_data = "ping_test";

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void __cm_ping_callback_demo(cm_ping_cb_type_e type, cm_ping_cb_result_e result, cm_ping_reply_t *resp, void *user_data)
{
    cm_demo_printf("ping_callback %s\n", user_data);

    switch (type)
    {
        case CM_PING_CB_TYPE_PING_ONCE:
        {
            cm_demo_printf("ping_callback: ip: %s, packet_len: %d, time: %d, ttl: %d\n", resp->ip, resp->packet_len, resp->time, resp->ttl);
        }
        break;
        case CM_PING_CB_TYPE_PING_TOTAL:
        {
            cm_demo_printf("ping_callback: ip: %s, send_cnt: %d, recv_cnt: %d, lost_cnt: %d, rtt_max: %d, rtt_min: %d, rtt_avg: %d\n", resp->ip, resp->send_cnt, resp->recv_cnt, resp->lost_cnt, resp->rtt_max, resp->rtt_min, resp->rtt_avg);
        }
        break;
        case CM_PING_CB_TYPE_PING_ERROR:
        {
            cm_demo_printf("ping_callback: error result: %d\n", result);
        }
        break;
    }
}

// internal test if char is a digit (0-9)
// \return true if char is a digit
static bool _is_digit(char ch)
{
    return (ch >= '0') && (ch <= '9');
}

// internal ASCII string to unsigned int conversion
static unsigned int _atoi(const char ** str)
{
    unsigned int i = 0U;
    while(_is_digit(**str)) {
        i = i * 10U + (unsigned int)(*((*str)++) - '0');
    }
    return i;
}

/* 执行测试命令 */
void cm_test_ping(unsigned char **cmd, int len)
{
    unsigned char sub_cmd[20] = {0};
    if(len == 2)
    {
        sprintf(sub_cmd, "%s", cmd[1]);
    }
    else if(len == 6)
    {
        cm_ping_cfg_t ping_cfg = {0};
        char *host_ptr = calloc(1, 256);
        if(host_ptr == NULL)
            return -1;
        sprintf(host_ptr, "%s", cmd[1]);
        ping_cfg.host = host_ptr;
        ping_cfg.timeout = _atoi(&cmd[2]);
        ping_cfg.ping_num = _atoi(&cmd[3]);
        ping_cfg.packet_len = _atoi(&cmd[4]);
        ping_cfg.cid = _atoi(&cmd[5]);
        if (cm_ping_init(&ping_cfg, __cm_ping_callback_demo))
        {
            cm_demo_printf("ping init err\n");
        }
        if(cm_ping_start((void *)__user_data))
        {
            cm_demo_printf("ping start err\n");
        }
        if(host_ptr)
        {
            free(host_ptr);
            host_ptr = NULL;
        }
    }
    else
        return;

    /* CM:PING:INIT */
    /* 测试需首先进行参数配置 */
    if (strncmp((const char*)sub_cmd, STR_ITEM("INIT")) == 0)
    {
        cm_ping_cfg_t ping_cfg = {0};
        ping_cfg.cid = 1;
        ping_cfg.host = "www.baidu.com";
        ping_cfg.packet_len = 32;
        ping_cfg.ping_num = 4;
        ping_cfg.timeout = 10;

        if (cm_ping_init(&ping_cfg, __cm_ping_callback_demo))
        {
            cm_demo_printf("ping init err\n");
        }
    }

    /* CM:PING:START */
    /* 执行ping，ping结果将从回调函数上报 */
    else if (strncmp((const char*)sub_cmd, STR_ITEM("START")) == 0)
    {
        if(cm_ping_start((void *)__user_data))
        {
            cm_demo_printf("ping start err\n");
        }
    }
}

#if defined(OS_USING_SHELL)
#include <nr_micro_shell.h>

static void SHELL_ping(char argc, char **argv)
{
    if(argc == 6)
    {
        shell_printf("cm_ping argv %s %s %s %s %s %s argc: %d\r\n",
                argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argc);
        cm_test_ping(argv, 6);
    }
    else if(argc == 2) /* help */
    {
        if (0 == strcmp(argv[1], "help"))
        {
            shell_printf("---------------- ping help---------------------\r\n");
            shell_printf("1 cm_ping INIT\r\n");
            shell_printf("2 cm_ping START\r\n");
            shell_printf("2 cm_ping host timeout ping_num packet_len cid\r\n");
            shell_printf("-----------------ping--------------------------\r\n");
        }
        else if ((0 == strcmp(argv[1], "INIT")) || (0 == strcmp(argv[1], "START")))
            cm_test_ping(argv, 2);
        else
            shell_printf("cm_ping parameter invalid\r\n");
    }
    else
        shell_printf("cm_ping parameter invalid\r\n");

}

NR_SHELL_CMD_EXPORT(cm_ping, SHELL_ping);

#endif
#endif

