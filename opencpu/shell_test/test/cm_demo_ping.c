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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cm_ping.h"
#include "cm_demo_uart.h"
#include "cm_demo_ping.h"

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
void cm_test_ping(EmbeddedCli *cli, char *args, void *context)
{
    /* ping [cmd] */
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }

    /* ping init */
    /* 测试需首先进行参数配置 */
    if (strncmp(cmd, STR_ITEM("init")) == 0)
    {
        cm_ping_cfg_t ping_cfg = {0};
        ping_cfg.cid = 0xff;
        ping_cfg.host = "www.baidu.com";
        ping_cfg.packet_len = 16;
        ping_cfg.ping_num = 4;
        ping_cfg.timeout = 10;

        if (cm_ping_init(&ping_cfg, __cm_ping_callback_demo))
        {
            cm_demo_printf("ping init err\n");
        }
    }
    
    /* ping start */
    /* 执行ping，ping结果将从回调函数上报 */
    else if (strncmp(cmd, STR_ITEM("start")) == 0)
    {
        if(cm_ping_start(NULL))
        {
            cm_demo_printf("ping start err\n");
        }
    }
}
