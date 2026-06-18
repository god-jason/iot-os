/*********************************************************
 *  @file    cm_demo_ping.c
 *  @brief   OpenCPU ping示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot1325 2024/3/28
 ********************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#ifdef CM_DEMO_PING_SUPPORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <os.h>

#include "cm_ping.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)
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
static char *__user_data = "ping_test";


/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void __cm_ping_callback_demo(cm_ping_cb_type_e type, cm_ping_cb_result_e result, cm_ping_reply_t *resp, void *user_data)
{
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

/* 执行测试命令 */
void cm_test_ping(char **cmd, int len)
{
    if(cmd == NULL || cmd[1] == NULL)
    {
        cm_demo_printf("cmd is null\r\n");
        return;
    }
    char sub_cmd[20] = {0};
    if(len == 2)
    {
        snprintf(sub_cmd, sizeof(sub_cmd), "%s", cmd[1]);
    }
    else if(len == 7)
    {
        if(cmd[2] == NULL||cmd[3] == NULL||cmd[4] == NULL||cmd[5] == NULL||cmd[6] == NULL)
        {
            cm_demo_printf("cmd[2-6] null\r\n");
            return;
        }
        cm_ping_cfg_t ping_cfg = {0};
        char *host_ptr = calloc(1, 256);
        if(host_ptr == NULL)
        {
            cm_demo_printf("malloc failed\r\n");
            return;
        }
        snprintf(host_ptr, 256, "%s", cmd[1]);
        ping_cfg.host = host_ptr;

        ping_cfg.timeout = atoi(cmd[2]);
        ping_cfg.ping_num = atoi(cmd[3]);
        ping_cfg.packet_len = atoi(cmd[4]);
        ping_cfg.cid = atoi(cmd[5]);
        ping_cfg.interval_time = atoi(cmd[6]);
        if (cm_ping_init(&ping_cfg, __cm_ping_callback_demo))
        {
            cm_demo_printf("ping init err\n");
            free(host_ptr);
            host_ptr = NULL;
            return;
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


    /* ping init */
    /* 测试需首先进行参数配置 */
    if (strncmp((const char*)sub_cmd, STR_ITEM("init")) == 0)
    {
        cm_ping_cfg_t ping_cfg = {0};
        ping_cfg.cid = 0xff;
        ping_cfg.host = "www.baidu.com";
        ping_cfg.packet_len = 16;
        ping_cfg.ping_num = 4;
        ping_cfg.timeout = 10;
        ping_cfg.interval_time = 200;

        if (cm_ping_init(&ping_cfg, __cm_ping_callback_demo))
        {
            cm_demo_printf("ping init err\n");
        }
    }

    /* ping start */
    /* 执行ping，ping结果将从回调函数上报 */
    else if (strncmp((const char*)sub_cmd, STR_ITEM("start")) == 0)
    {
        if(cm_ping_start((void *)__user_data))
        {
            cm_demo_printf("ping start err\n");
        }
    }
}
#ifdef OS_USING_SHELL
#include <nr_micro_shell.h>
static int argv_sanity_check(int argc, char **argv)
{
    if(argv == NULL)
    {
       cm_demo_printf("cm_ping argv null\r\n");
       return -1;
    }
    for(int i = 0; i < argc; i++)
    {
        if(argv[i] == NULL)
        {
            cm_demo_printf("argv[%d] is NULL, invaild\r\n", i);
            return -2;
        }
    }
    return 0;
}

static void SHELL_ping(char argc, char **argv)
{
    int ret = argv_sanity_check(argc, argv);
    if(ret != 0)
    {
        cm_demo_printf("argv_sanity_check failed ret[%d]\r\n",ret);
        return;
    }
    if(argc == 7)
    {
        cm_demo_printf("cm_ping argv %s %s %s %s %s %s %s argc: %d\r\n",
                argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argc);
        cm_test_ping(argv, 7);
    }
    else if(argc == 2) /* help */
    {
        if (0 == strcasecmp(argv[1], "help"))
        {
            cm_demo_printf("---------------- ping help---------------------\r\n");
            cm_demo_printf("1 cm_ping INIT\r\n");
            cm_demo_printf("2 cm_ping START\r\n");
            cm_demo_printf("2 cm_ping host timeout ping_num packet_len cid interval\r\n");
            cm_demo_printf("-----------------ping--------------------------\r\n");
        }
        else if ((0 == strcasecmp(argv[1], "INIT")) || (0 == strcasecmp(argv[1], "START")))
            cm_test_ping(argv, 2);
        else
            cm_demo_printf("cm_ping parameter invalid\r\n");
    }
    else
        cm_demo_printf("cm_ping parameter invalid\r\n");

}

NR_SHELL_CMD_EXPORT(cm_ping, SHELL_ping);
#endif
#endif
