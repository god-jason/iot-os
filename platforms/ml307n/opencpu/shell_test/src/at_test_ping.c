/*********************************************************
 *  @file    at_test_ping.c
 *  @brief   OpenCPU PING示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created by xuxuzhu 2024/01/20
 ********************************************************/

#ifdef CM_DEMO_PING_SUPPORT
#ifdef OS_USING_SHELL

#include <os.h>
#include <stdlib.h>

// #include "cm_common_inc.h"
// #include "cm_common_api.h"
#include "at_api.h"
#include "at_parser.h"
#include "cm_ping.h"
#include "cm_mem.h"
#include "cm_demo_common.h"

typedef AT_CommandItem at_cmd_t;

#define PING_LOG(fmt, ...) osPrintf("[PING][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define PING_CMD_STR_LEN 512
#define HOST_STR_LEN     512

#define AT_PING_CID_DEFUALT              (0xff)

#define CM_PING_RET_CODE_OK 0 //成功
#define CM_PING_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_PING_RET_CODE_PARAM_ERROR 50 //参数错误 50
#define CM_PING_RET_CODE_NOT_INIT 3 //操作不被允许 3
#define CM_PING_ERROR_PDP_NO_ACTIVE 570



void ping_oc_test_entry(os_uint8_t id, os_uint8_t *at_buf, os_uint16_t lenOfPara);
int func_cm_ping_init(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ping_start(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);

static char *__user_data = "ping_test";



AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_ping_oc[] =
{
    {.name = "+PING",   .setFunc = ping_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

// AT+PING="XX", param1, param2, ...
// "XX" 为函数名
void ping_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    PING_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$,$,$");
        if (para_count < 1 || para_count > 7)
        {
            ret = CM_PING_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            ret = CM_PING_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_ping_init") == 0)
        {
            ret = func_cm_ping_init(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ping_start") == 0)
        {
            ret = func_cm_ping_start(id, (os_uint8_t *)para_p_new, lenOfPara_new);
            if(ret < 0)
            {
                ret = CM_PING_RET_CODE_NOT_INIT;
            }
        }
        else
        {
            PING_LOG("Error func name[%s]", func_name);
            ret = CM_PING_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}


static void __cm_ping_callback_at(cm_ping_cb_type_e type, cm_ping_cb_result_e result, cm_ping_reply_t *resp, void *user_data)
{
    PING_LOG("ping_callback %s\n", user_data);

    switch (type)
    {
        case CM_PING_CB_TYPE_PING_ONCE:
        {
            PING_LOG("ping_callback: ip: %s, packet_len: %d, time: %d, ttl: %d\n", resp->ip, resp->packet_len, resp->time, resp->ttl);
            cm_uart_printf_urc("ping_callback: ip: %s, packet_len: %d, time: %d, ttl: %d\n", resp->ip, resp->packet_len, resp->time, resp->ttl);
        }
        break;
        case CM_PING_CB_TYPE_PING_TOTAL:
        {
            PING_LOG("ping_callback: ip: %s, send_cnt: %d, recv_cnt: %d, lost_cnt: %d, rtt_max: %d, rtt_min: %d, rtt_avg: %d\n", resp->ip, resp->send_cnt, resp->recv_cnt, resp->lost_cnt, resp->rtt_max, resp->rtt_min, resp->rtt_avg);
            cm_uart_printf_urc("ping_callback: ip: %s, send_cnt: %d, recv_cnt: %d, lost_cnt: %d, rtt_max: %d, rtt_min: %d, rtt_avg: %d\n", resp->ip, resp->send_cnt, resp->recv_cnt, resp->lost_cnt, resp->rtt_max, resp->rtt_min, resp->rtt_avg);
        }
        break;
        case CM_PING_CB_TYPE_PING_ERROR:
        {
            PING_LOG("ping_callback: error result: %d\n", result);
            cm_uart_printf_urc("ping_callback: error result: %d\n", result);
        }
        break;
    }
}

// AT+PING="cm_ping_init","host",timeout,ping_num,packet_len,cid
int func_cm_ping_init(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{

    char* host = NULL;
    int timeout = 10;
    int ping_num = 5;
    int packet_len = 40;
    int cid = 1;

    host = cm_malloc(HOST_STR_LEN);
    if (host == NULL) {
        cm_uart_printf_urc("+PINGINIT: %d", CM_PING_RET_CODE_MALLOC_FAIL);
        return CM_PING_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", host, HOST_STR_LEN))
    {
        cm_free(host);
        cm_uart_printf_urc("+PINGINIT: %d", CM_PING_RET_CODE_PARAM_ERROR);
        return CM_PING_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &timeout))
    {
        cm_free(host);
        cm_uart_printf_urc("+PINGINIT: %d", CM_PING_RET_CODE_PARAM_ERROR);
        return CM_PING_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &ping_num))
    {
        cm_free(host);
        cm_uart_printf_urc("+PINGINIT: %d", CM_PING_RET_CODE_PARAM_ERROR);
        return CM_PING_RET_CODE_PARAM_ERROR;
    }


    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &packet_len))
    {
        cm_free(host);
        cm_uart_printf_urc("+PINGINIT: %d", CM_PING_RET_CODE_PARAM_ERROR);
        return CM_PING_RET_CODE_PARAM_ERROR;
    }
    
    int ret = xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &cid);
    if(ret == AT_ERRNO_ABORT_PARAM_ERROR)
    {
        cid= AT_PING_CID_DEFUALT;
    }
    else if(ret != AT_ERRNO_NO_ERROR)
    {
        cm_free(host);
        cm_uart_printf_urc("+PINGINIT: %d", CM_PING_RET_CODE_PARAM_ERROR);
        return CM_PING_RET_CODE_PARAM_ERROR;
    }

    PING_LOG("host[%s] timeout[%d] ping_num[%d] packet_len[%d] cid[%d]", \
            host, timeout, ping_num, packet_len, cid);


    cm_ping_cfg_t ping_cfg = {0};
    ping_cfg.host = host;
    ping_cfg.timeout = timeout;
    ping_cfg.ping_num = ping_num;
    ping_cfg.packet_len = packet_len;
    ping_cfg.cid = cid;
    ret = cm_ping_init(&ping_cfg, __cm_ping_callback_at);
    if (ret != 0)
    {
        PING_LOG("ping init err\n");
        cm_uart_printf_urc("+PINGINIT: %d", ret);
        if(ret == -7)
            return CM_PING_ERROR_PDP_NO_ACTIVE;
        else
            return CM_PING_RET_CODE_PARAM_ERROR;
    }
    cm_uart_printf_urc("+PINGINIT: %d", ret);
    cm_free(host);
    return 0;
}

// AT+PING="cm_ping_start"
int func_cm_ping_start(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = -1;

    ret = cm_ping_start((void *)__user_data);
    if (ret < 0)
    {
        PING_LOG("ping start err\n");
        cm_uart_printf_urc("+PINGSTART: %d", ret);
        return ret;
    }

    cm_uart_printf_urc("+PINGSTART: %d", ret);
    return 0;
}
#endif
#endif


