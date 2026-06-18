/**
 *  @file    cm_demo_dns.c
 *  @brief   OpenCPU DNS相关测试例程
 *  @copyright copyright © 2022 China Mobile IOT. All rights reserved.
 *  @author by XXZ
 *  @date 2026/03/27
 */



 /****************************************************************************
 * Included Files
 ****************************************************************************/
#ifdef CM_DEMO_DNS_SUPPORT

#include <stdio.h>
#include <string.h>
#include "cm_mem.h"
#include "cm_asocket.h"
#include "cm_eloop.h"
#include "cm_async_dns.h"
#include <string.h>
#include <stdlib.h>


#define cm_demo_printf osPrintf
#define DNS_LOG(fmt, ...) osPrintf("[DNS][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define DNS_HOST_STR_LEN     256

#define CM_DNS_RET_CODE_OK 0 //成功
#define CM_DNS_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_DNS_RET_CODE_PARAM_ERROR 50 //参数错误 50
static char *domain_name = NULL;
static unsigned char addr_type = 0;
static unsigned char cid = 0;

/****************************************************************************
 * Private Data
 ****************************************************************************/


// DNS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 异步DNS事件回调函数(执行环境为eloop线程) */
static void async_dns_event(int req_id, cm_async_dns_event_e event, void *cb_param,
                                 const char *host_name, const cm_async_dns_ip_addr_t *ip_addr)
{
    char *name = host_name ? host_name : "unknown host_name";

    DNS_LOG("__on_async_dns_event(%d, %d, %s)\n", req_id, event, name);

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
            cm_demo_printf("+ASYNCDNS: dns req ok: %s->%s\n", name, ip_str);
            DNS_LOG("dns req(%d) ok: %s->%s\n", req_id, name, ip_str);
            break;
        }

        case CM_ASYNC_DNS_RESOLVE_FAIL:
        {
            cm_demo_printf("+ASYNCDNS: dns req(%d) fail: %s\n", req_id, name);
            DNS_LOG("dns req(%d) fail: %s\n", req_id, name);
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
    int ret = cm_async_dns_request_v2(host_name, addr_type, &ip_addr, async_dns_event, NULL, cid);

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
        cm_demo_printf("+ASYNCDNS: dns req ok: %s->%s\n", host_name, ip_str);
        DNS_LOG("dns req ok: %s->%s\n", host_name, ip_str);
    }

    else if (ret > 0)
    {
        DNS_LOG("dns req(%d) pending...\n", ret);
    }

    else
    {
        cm_demo_printf("+ASYNCDNS: dns req fail %d\n", ret);
        DNS_LOG("dns req fail %d\n", ret);
    }

    free(host_name);
    host_name = NULL;
}
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

int SHELL_testDns(char argc, char **argv)
{
    int ret = argv_sanity_check(argc, argv);
    if(ret != 0)
    {
        cm_demo_printf("argv_sanity_check failed ret[%d]\r\n",ret);
        return CM_DNS_RET_CODE_PARAM_ERROR;
    }

    if (argc != 4)
    {
        cm_demo_printf("usage: cm_dns hostname addrtype cid\n");
        cm_demo_printf("example: cm_dns www.baidu.com 2 1\n");
        return CM_DNS_RET_CODE_PARAM_ERROR;
    }
    domain_name = cm_malloc(DNS_HOST_STR_LEN);
    if (domain_name == NULL) {
        cm_demo_printf("host_name malloc failed\n");
        return CM_DNS_RET_CODE_MALLOC_FAIL;
    }

    strncpy(domain_name, argv[1], strlen(argv[1]));
    addr_type = atoi(argv[2]);

    cid = atoi(argv[3]);

    if(addr_type<0 || addr_type>3)
    {
        cm_free(domain_name);
        cm_demo_printf("wrong  AddrTye \n");
        return CM_DNS_RET_CODE_PARAM_ERROR;
    }

    DNS_LOG("__on_cmd_DNS host_name=%s\n", domain_name);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_DNS_recv_event = cm_eloop_register_event(cm_async_dns_eloop(), eloop_cmd_DNS_recv_event, (void *)domain_name);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_DNS_recv_event);
    return CM_DNS_RET_CODE_OK;
}
#ifdef OS_USING_SHELL
#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(cm_dns, SHELL_testDns);
#endif
#endif

