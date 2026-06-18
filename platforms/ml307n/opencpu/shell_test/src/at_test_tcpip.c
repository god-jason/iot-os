/*********************************************************
 *  @file    cm_demo_tcpip.c
 *  @brief   OpenCPU TCPIP示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created by xuxuzhu 2024/01/21
 ********************************************************/

#ifdef CM_DEMO_ASOCKET_SUPPORT
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
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>



typedef AT_CommandItem at_cmd_t;

#define TCPIP_LOG(fmt, ...) osPrintf("[OC_TCPIP_TEST][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define TCPIP_CMD_STR_LEN 512
#define TCPIP_HOST_LEN    512
#define TCPIP_SEND_STRING_LEN 1024

#define CM_TCPIP_RET_CODE_OK 0 //成功
#define CM_TCPIP_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_TCPIP_RET_CODE_PARAM_ERROR 50 //参数错误 50

void tcpip_oc_test_entry(os_uint8_t id, os_uint8_t *at_buf, os_uint16_t lenOfPara);
int func_cm_asocket_init(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_asocket_open(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_asocket_send(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_asocket_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);

static cm_eloop_handle_t test_eloop = NULL;
static char *host_addr = NULL;
static int   port = 80;
static int send_len = 0;
static char *send_string = NULL;
static int ssl_sock = -1;
static uint8_t is_ipv6 = 0;

AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_tcpip_oc[] =
{
    {.name = "+TCPIP",   .setFunc = tcpip_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

// AT+TCPIP="XX", param1, param2, ...
// "XX" 为函数名
void tcpip_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    TCPIP_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$");
        if (para_count < 1 || para_count > 5)
        {
            ret = CM_TCPIP_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            ret = CM_TCPIP_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_asocket_open") == 0)
        {
            ret = func_cm_asocket_open(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_asocket_send") == 0)
        {
            ret = func_cm_asocket_send(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_asocket_close") == 0)
        {
            ret = func_cm_asocket_close(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            TCPIP_LOG("Error func name[%s]", func_name);
            ret = CM_TCPIP_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}
/* 异步socket消息回调(执行环境为eloop线程) */
static void __on_asocket_event(int sock, cm_asocket_event_e event, void *user_param)
{
    switch (event)
    {
        // responses
        case CM_ASOCKET_EV_CONNECT_OK:
        {
            TCPIP_LOG("sock(%d) connect_ok\n", sock);
            cm_uart_printf_urc("+TCPIPOPEN: sock(%d) connect_ok\n", sock);
            break;
        }
        case CM_ASOCKET_EV_CONNECT_FAIL:
        {
            TCPIP_LOG("sock(%d) connect_fail\n", sock);
            cm_uart_printf_urc("+TCPIPOPEN: sock(%d) connect_fail\n", sock);
            break;
        }
        // indications
        case CM_ASOCKET_EV_RECV_IND:
        {
            if(sock == ssl_sock)
            {
                break;
            }
            /* 取得获取接收缓存中可读的数据长度 */
            int recv_avail = 0;
            cm_asocket_ioctl(sock, FIONREAD, &recv_avail);

            /* 接收数据 */
            uint8_t recv_buf[128] = {0};
            int ret = cm_asocket_recv(sock, recv_buf, sizeof(recv_buf), 0);

            if (ret > 0)
            {
                TCPIP_LOG("sock(%d) recv_ind: recv_avail=%d, recv_len=%d, data=%.*s\n", sock, recv_avail, ret, ret, recv_buf);
            }

            else
            {
                TCPIP_LOG("sock(%d) recv_ind error(%d)\n", sock, errno);

                if (ENOTCONN == errno)
                {
                    /* Connection closed */
                    TCPIP_LOG("sock(%d) recv_ind: Connection closed\n", sock);
                }
            }
            break;
        }

        case CM_ASOCKET_EV_SEND_IND:
        {
            TCPIP_LOG("sock(%d) send_ind\n", sock);
            break;
        }

        case CM_ASOCKET_EV_ACCEPT_IND:
        {
            TCPIP_LOG("sock(%d) accept_ind\n", sock);
            break;
        }

        case CM_ASOCKET_EV_ERROR_IND: 
        {
            /* 获取socket错误码 */
            int sock_error = 0;
            socklen_t opt_len = sizeof(sock_error);

            cm_asocket_getsockopt(sock, SOL_SOCKET, SO_ERROR, &sock_error, &opt_len);
            TCPIP_LOG("sock(%d) error_ind: sock_error(%d)\n", sock, sock_error);

            if (ECONNABORTED == sock_error)
            {
                /* Connection aborted */
                TCPIP_LOG("sock(%d) error_ind: Connection aborted\n", sock);
            }
            else if (ECONNRESET == sock_error)
            {
                /* Connection reset */
                TCPIP_LOG("sock(%d) error_ind: Connection reset\n", sock);
            }
            else if (ENOTCONN == sock_error)
            {
                /* Connection closed */
                TCPIP_LOG("sock(%d) error_ind: Connection closed\n", sock);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_OPEN_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    int type = (int)cb_param;
    TCPIP_LOG("__on_eloop_cmd_OPEN_recv_event type=%d\n", type);

    /* 处理命令 */
    int sock = -1;
    int domain= (is_ipv6 == 1) ? AF_INET6 : AF_INET;
    if (0 == type)
    {
        /* TCP */
        sock = cm_asocket_open(domain, SOCK_STREAM, IPPROTO_TCP, __on_asocket_event, NULL);
    }
    else if(1 == type)
    {
        /* UDP */
        sock = cm_asocket_open(domain, SOCK_DGRAM, IPPROTO_UDP, __on_asocket_event, NULL);
    }
    else
    {
        sock = cm_asocket_open(domain, SOCK_STREAM, IPPROTO_TCP, __on_asocket_event, NULL);
        ssl_sock = sock;
    }
    if (-1 == sock)
    {
        TCPIP_LOG("open failed\n");
        return;
    }
    TCPIP_LOG("sock(%d) open\n", sock);

    int ret = 0; 

    if(is_ipv6 == 1)
    {
        struct sockaddr_in6 server_addr6;
        memset(&server_addr6, 0, sizeof(server_addr6));

        server_addr6.sin6_port = htons(port);
        server_addr6.sin6_family = AF_INET6;
        inet_pton(AF_INET6, host_addr, &(server_addr6.sin6_addr));
        TCPIP_LOG("IPv6 address: %s", host_addr);
        
        ret = cm_asocket_connect(sock, (const struct sockaddr *)&server_addr6, sizeof(server_addr6));
    }
    else
    {
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(host_addr);
        server_addr.sin_port = htons(port);

        ret = cm_asocket_connect(sock, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    }
    if (ret < 0)
    {
        if (EINPROGRESS == errno)
        {
            TCPIP_LOG("sock(%d) open request success, wait connect...\n", sock);
        }
        else
        {
            TCPIP_LOG("sock(%d) connect failed(%d)\n", sock, errno);
            if(1 == type)
            {
                cm_uart_printf_urc("+TCPIPOPEN: sock(%d) fail\n", sock);
            }
        }
    }
    else if (0 == ret)
    {
        TCPIP_LOG("sock(%d) connect_ok\n", sock);
        if (1 == type)
        {
            cm_uart_printf_urc("+TCPIPOPEN: sock(%d) ok\n", sock);
        }
    }
    cm_free(host_addr);
}

// AT+TCPIP="cm_asocket_open", "host", port, type
int func_cm_asocket_open(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int type = 0;

    host_addr = cm_malloc(TCPIP_HOST_LEN);
    if (host_addr == NULL) {
        return CM_TCPIP_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", host_addr, TCPIP_HOST_LEN))
    {
        cm_free(host_addr);
        TCPIP_LOG("host_addr error\n");
        return CM_TCPIP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &is_ipv6))
    {
        cm_free(host_addr);
        TCPIP_LOG("is_ipv6 error\n");
        return CM_TCPIP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &port))
    {
        cm_free(host_addr);
        TCPIP_LOG("port error\n");
        return CM_TCPIP_RET_CODE_PARAM_ERROR;
    }
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &type))    
    {
        cm_free(host_addr);
        TCPIP_LOG("type error\n");
        return CM_TCPIP_RET_CODE_PARAM_ERROR;
    }
    TCPIP_LOG("__on_cmd_OPEN type=%d\n", type);
    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_OPEN_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_OPEN_recv_event, (void *)type);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_OPEN_recv_event);
}
static void __on_eloop_cmd_SEND_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    int sock = (int)cb_param;

    TCPIP_LOG("__on_eloop_cmd_SEND_recv_event sock=%d\n", sock);

    /* 处理命令 */
    if (-1 != sock)
    {
        int ret = cm_asocket_send(sock, send_string, send_len, 0);
        if (ret > 0)
        {
            TCPIP_LOG("sock(%d) send len=%d\n", sock, ret);
            cm_uart_printf_urc("+TCPIPSEND: send %d\n", ret);
        }
        else
        {
            TCPIP_LOG("sock(%d) send error(%d)\n", sock, ret);
            cm_uart_printf_urc("+TCPIPSEND: send %d\n", ret);
        }
    }
    cm_free(send_string);
}

// AT+TCPIP="cm_asocket_send", sock, len, "string"
int func_cm_asocket_send(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int sock = 0;
    send_string = cm_malloc(TCPIP_SEND_STRING_LEN);
    if (send_string == NULL) {
        return CM_TCPIP_RET_CODE_MALLOC_FAIL;
    }
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &sock))
    {
        cm_free(send_string);
        return CM_TCPIP_RET_CODE_PARAM_ERROR;
    }
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &send_len))
    {
        cm_free(send_string);
        return CM_TCPIP_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", send_string, TCPIP_SEND_STRING_LEN))
    {
        cm_free(send_string);
        return CM_TCPIP_RET_CODE_PARAM_ERROR;
    }

    TCPIP_LOG("__on_cmd_SEND sock=%d\n", sock);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_SEND_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_SEND_recv_event, (void *)sock);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_SEND_recv_event);
}
static void __on_eloop_cmd_CLOSE_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    int sock = (int)cb_param;

    TCPIP_LOG("__on_eloop_cmd_CLOSE_recv_event sock=%d\n", sock);

    /* 处理命令 */
    int ret = cm_asocket_close(sock);
    if (0 == ret)
    {
        TCPIP_LOG("sock(%d) close success\n", sock);
        cm_uart_printf_urc("+TCPIPCLOSE: OK");
    }
    else
    {
        TCPIP_LOG("sock(%d) close error(%d)\n", sock, errno);
        cm_uart_printf_urc("+TCPIPCLOSE: fail");
    }
}

// AT+TCPIP="cm_asocket_close"
int func_cm_asocket_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int sock = 0;
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &sock))
    {
        return CM_TCPIP_RET_CODE_PARAM_ERROR;
    }
    TCPIP_LOG("__on_cmd_CLOSE sock=%d\n", sock);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_CLOSE_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_CLOSE_recv_event, (void *)sock);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_CLOSE_recv_event);
}

#endif
#endif


