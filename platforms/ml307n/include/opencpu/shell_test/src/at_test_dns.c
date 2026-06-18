/*********************************************************
 *  @file    at_test_dns.c
 *  @brief   OpenCPU DNS示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created by liulongyi 2024/01/22
 ********************************************************/
#ifdef CM_DEMO_DNS_SUPPORT
#ifdef OS_USING_SHELL

#include <os.h>
#include <stdlib.h>

// #include "cm_common_inc.h"
// #include "cm_common_api.h"
#include "at_api.h"
#include "at_parser.h"
#include "cm_mem.h"
#include "lwip/arch.h"
#include "cm_asocket.h"
#include "cm_eloop.h"
#include "cm_async_dns.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>


typedef AT_CommandItem at_cmd_t;

#define DNS_LOG(fmt, ...) osPrintf("[DNS][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define DNS_HOST_STR_LEN     256

#define CM_DNS_RET_CODE_OK 0 //成功
#define CM_DNS_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_DNS_RET_CODE_PARAM_ERROR 50 //参数错误 50

void dns_oc_test_entry(os_uint8_t id, os_uint8_t *at_buf, os_uint16_t lenOfPara);
int func_cm_asocket_dns(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_asocket_dns6(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_async_dns_set_priority(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);

static cm_eloop_handle_t test_eloop = NULL;
static char *host_name = NULL;
static os_uint8_t AddrTye = 0;

AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_dns_oc[] =
{
    {.name = "+ASYNCDNS",   .setFunc = dns_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

/* 执行测试命令 */
void dns_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    DNS_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$");
        if (para_count < 1 || para_count > 3)
        {
            ret = CM_DNS_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            ret = CM_DNS_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_asocket_dns") == 0)
        {
            ret = func_cm_asocket_dns(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_async_dns_set_priority") == 0)
        {
            ret = func_cm_async_dns_set_priority(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            DNS_LOG("Error func name[%s]", func_name);
            ret = CM_DNS_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}


// DNS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 异步DNS事件回调函数(执行环境为eloop线程) */
static void async_dns_event(int req_id, cm_async_dns_event_e event, void *cb_param,
                                 const char *host_name, const cm_async_dns_ip_addr_t *ip_addr)
{
    DNS_LOG("__on_async_dns_event(%d, %d, %s)\n", req_id, event, host_name);

    switch (event)
    {
        case CM_ASYNC_DNS_RESOLVE_OK: 
        {
            char ip_str[128] = "";

            if (CM_ASYNC_DNS_ADDRTYPE_IPV4 == ip_addr->type)
            {
                inet_ntop(AF_INET, &(ip_addr->u_addr.sin_addr), ip_str, sizeof(ip_str));
            }

            else if (CM_ASYNC_DNS_ADDRTYPE_IPV6 == ip_addr->type)
            {
                inet_ntop(AF_INET6, &(ip_addr->u_addr.sin6_addr), ip_str, sizeof(ip_str));
            }
            cm_uart_printf_urc("+ASYNCDNS: dns req ok: %s->%s\n", host_name, ip_str);
            DNS_LOG("dns req(%d) ok: %s->%s\n", req_id, host_name, ip_str);
            break;
        }

        case CM_ASYNC_DNS_RESOLVE_FAIL:
        {
            cm_uart_printf_urc("+ASYNCDNS: dns req(%d) fail: %s\n", req_id, host_name);
            DNS_LOG("dns req(%d) fail: %s\n", req_id, host_name);
            break;
        }

        default:
        {
            break;
        }
    }
}

/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void eloop_cmd_DNS_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    char *host_name = (char *)cb_param;

    DNS_LOG("__on_eloop_cmd_DNS_recv_event host_name=%s\n", host_name);

    /* 处理命令 */
    cm_async_dns_ip_addr_t ip_addr;
    int ret = cm_async_dns_request(host_name, AddrTye, &ip_addr, async_dns_event, NULL);

    if (0 == ret)
    {
        char ip_str[128] = "";

        if (CM_ASYNC_DNS_ADDRTYPE_IPV4 == ip_addr.type)
        {
            inet_ntop(AF_INET, &(ip_addr.u_addr.sin_addr), ip_str, sizeof(ip_str));
        }

        else if (CM_ASYNC_DNS_ADDRTYPE_IPV6 == ip_addr.type)
        {
            inet_ntop(AF_INET6, &(ip_addr.u_addr.sin6_addr), ip_str, sizeof(ip_str));
        }
        cm_uart_printf_urc("+ASYNCDNS: dns req ok: %s->%s\n", host_name, ip_str);
        DNS_LOG("dns req ok: %s->%s\n", host_name, ip_str);
    }

    else if (ret > 0)
    {
        DNS_LOG("dns req(%d) pending...\n", ret);
    }

    else
    {
        cm_uart_printf_urc("+ASYNCDNS: dns req fail %d\n", ret);
        DNS_LOG("dns req fail %d\n", ret);
    }

    free(host_name);
}

int func_cm_asocket_dns(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    host_name = cm_malloc(DNS_HOST_STR_LEN);
    if (host_name == NULL) {
        return CM_DNS_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u,%u", host_name, DNS_HOST_STR_LEN, &AddrTye))
    {
        cm_free(host_name);
        return CM_DNS_RET_CODE_PARAM_ERROR;
    }
    if(AddrTye<0 || AddrTye>3)
    {
        cm_free(host_name);
        return CM_DNS_RET_CODE_PARAM_ERROR;
    }

    DNS_LOG("__on_cmd_DNS host_name=%s\n", host_name);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_DNS_recv_event = cm_eloop_register_event(cm_async_dns_eloop(), eloop_cmd_DNS_recv_event, (void *)host_name);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_DNS_recv_event);
    return CM_DNS_RET_CODE_OK;
}

int func_cm_async_dns_set_priority(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle;
    int priority;
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &priority))
    {
        return CM_DNS_RET_CODE_PARAM_ERROR;
    }

    handle = cm_async_dns_set_priority((uint8_t )priority);
    if (handle < 0 )
    {
        cm_uart_printf_urc("+DNSSETPRIORITY: %d", handle);
    }

    return 0;
}
#endif
#endif
