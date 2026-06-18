/*********************************************************
 *  @file    cm_demo_asocket.c
 *  @brief   OpenCPU asocket async_dns eloop示例
 *  Copyright (c) 2023 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot1325 2023/6/29
 ********************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#ifdef CM_DEMO_ASOCKET_SUPPORT
#ifdef OS_USING_SHELL

#include <nr_micro_shell.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/sockets.h"
#include "lwip/arch.h"
#include "cm_asocket.h"
#include "cm_eloop.h"
#include "cm_async_dns.h"
#include "cm_mem.h"


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define cm_demo_printf osPrintf
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)
#define MAX_IP_LEN (128+1)
/****************************************************************************
 * Private Types
 ****************************************************************************/

 typedef enum
{
    TEST_PROTOCOL_TCP = 0,               /*!< TCP协议 */
    TEST_PROTOCOL_UDP = 1,               /*!< UDP协议 */
} asocket_test_protocol_e;

typedef struct
{
    asocket_test_protocol_e      protocol;                       /* !< 协议类型 */
    uint16_t                     server_port;                    /* !< 服务器端口 */
    int32_t                      socket;                         /* !< 套接字 */
    struct sockaddr_in           addr;                           /* !< 服务器地址（解析完成的网络类型地址 IPV4） */
} asocket_test_client_t;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
static int argv_sanity_check(int argc, char **argv);


/****************************************************************************
 * Private Data
 ****************************************************************************/

static cm_eloop_handle_t test_eloop = NULL;     //eloop测试

/* 测试前需补充完整如下参数 */
static char test_tcp_addr[MAX_IP_LEN] = {0};     //TCP测试地址 例"192.168.0.1"
static char test_udp_addr[MAX_IP_LEN] = {0};     //UDP测试地址 例"192.168.0.1"
static uint16_t test_tcp_port = 9000;       //TCP测试端口
static uint16_t test_udp_port = 9001;       //UDP测试端口

static asocket_test_client_t test_client_tcp =
{
    .protocol = TEST_PROTOCOL_TCP,
    .server_port = 0,
    .socket = -1,
    .addr = {0},
};

static asocket_test_client_t test_client_udp =
{
    .protocol = TEST_PROTOCOL_UDP,
    .server_port = 0,
    .socket = -1,
    .addr = {0},
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* 异步socket消息回调(执行环境为eloop线程) */
static void __on_asocket_event(int sock, cm_asocket_event_e event, void *ev_param, void *user_param)
{
    if(user_param == NULL)
    {
        cm_demo_printf("user_param is null\r\n");
        return;
    }
    uint32_t data = (uint32_t)ev_param;
    int ret = -1;
    asocket_test_client_t *test_client = (asocket_test_client_t *)user_param;
    if (sock != test_client->socket)
    {
        cm_demo_printf("sock error(%d)(%d)\n", sock, test_client->socket);
    }

    switch (event)
    {
        // responses
        case CM_ASOCKET_EV_CONNECT_OK:
        {
            cm_demo_printf("sock(%d) connect_ok\n", sock);
            break;
        }

        case CM_ASOCKET_EV_CONNECT_FAIL:
        {
            cm_demo_printf("sock(%d) connect_fail\n", sock);
            break;
        }

        // indications
        case CM_ASOCKET_EV_RECV_IND:
        {
            /* 取得获取接收缓存中可读的数据长度 */
            int recv_avail = 0;
            if (cm_asocket_ioctl(sock, FIONREAD, &recv_avail) != 0)
            {
                cm_demo_printf("sock(%d) get recv_avail error(%d)\n", sock, errno);
                break;
            }
            cm_demo_printf("sock(%d) get recv_avail (%d)\n", sock, recv_avail);

            if (recv_avail >= 0)
            {
                if(test_client->protocol == TEST_PROTOCOL_TCP)
                {
                    if(recv_avail > 0)
                    {
                        /* 接收数据 */
                        char *recv_buf = (char *)cm_malloc(recv_avail + 1);
                        ret = cm_asocket_recv(sock, recv_buf, recv_avail, 0);
                        if (ret > 0)
                        {
                            cm_demo_printf("sock(%d) recv_ind: recv_avail=%d, recv_len=%d, data=%.*s\n", sock, recv_avail, ret, ret, recv_buf);
                        }
                        else
                        {
                            cm_demo_printf("sock(%d) recv_ind error(%d)\n", sock, errno);
                        }
                        cm_free(recv_buf);
                    }
                    else if(recv_avail == 0)
                    {
                        cm_demo_printf("sock(%d) recv_ind: recv_avail=%d\n", sock, recv_avail);
                    }
                }
                else
                {
                    int read_len = data;
                    cm_demo_printf("sock(%d) get udp read_len (%d)\n", sock, read_len);

                    if(read_len > 0)
                    {
                        char *recv_buf = (char *)cm_malloc(read_len + 1);
                        socklen_t addr_len = sizeof(test_client->addr);
                        ret = cm_asocket_recvfrom(sock, recv_buf, read_len, 0, (struct sockaddr *)&test_client->addr, &addr_len);
                        if (ret > 0)
                        {
                            cm_demo_printf("sock(%d) recv_ind: read_len=%d, recv_len=%d, data=%.*s\n", sock, read_len, ret, ret, recv_buf);
                        }
                        else
                        {
                            cm_demo_printf("sock(%d) recv_ind error(%d)\n", sock, errno);
                        }
                        cm_free(recv_buf);
                    }
                    else if(read_len == 0)
                    {
                        cm_demo_printf("sock(%d) recv_ind: read_len=%d\n", sock, read_len);
                    }
                }
            }
            else
            {
                cm_demo_printf("sock(%d) error(%d)\n", sock, errno);
            }
            break;
        }

        case CM_ASOCKET_EV_SEND_IND:
        {
            cm_demo_printf("sock(%d) send_ind %d\n", sock, data);
            break;
        }

        case CM_ASOCKET_EV_ACCEPT_IND:
        {
            cm_demo_printf("sock(%d) accept_ind\n", sock);
            break;
        }

        case CM_ASOCKET_EV_ERROR_IND:
        {
            /* 获取socket错误码 */
            int sock_error = 0;
            socklen_t opt_len = sizeof(sock_error);

            cm_asocket_getsockopt(sock, SOL_SOCKET, SO_ERROR, &sock_error, &opt_len);
            cm_demo_printf("sock(%d) error_ind: sock_error(%d)\n", sock, sock_error);

            if (ECONNABORTED == sock_error)
            {
                /* Connection aborted */
                cm_demo_printf("sock(%d) error_ind: Connection aborted\n", sock);
            }

            else if (ECONNRESET == sock_error)
            {
                /* Connection reset */
                cm_demo_printf("sock(%d) error_ind: Connection reset\n", sock);
            }

            else if (ENOTCONN == sock_error)
            {
                /* Connection closed */
                cm_demo_printf("sock(%d) error_ind: Connection closed\n", sock);
            }

            break;
        }

        default:
        {
            break;
        }
    }
}

// OPEN ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_open_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    int type = (int)cb_param;
    cm_demo_printf("__on_eloop_cmd_open_recv_event type=%d\n", type);

    /* 处理命令 */
    int sock = -1;
    if (0 == type)
    {
        /* TCP */
        sock = cm_asocket_open(AF_INET, SOCK_STREAM, IPPROTO_TCP, __on_asocket_event, (void *)&test_client_tcp);
        cm_demo_printf("sock(%d) open\n", sock);

        if (sock >= 0)
        {
            struct sockaddr_in server_addr;
            memset(&server_addr, 0, sizeof(server_addr));

            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(test_tcp_port);
            inet_pton(AF_INET, test_tcp_addr, &server_addr.sin_addr);

            int ret = cm_asocket_connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

            if (ret < 0)
            {
                if (EINPROGRESS == errno)
                {
                    cm_demo_printf("sock(%d) tcp connect request success, wait connect...\n", sock);
                }

                else
                {
                    cm_demo_printf("sock(%d) tcp connect failed(%d)\n", sock, errno);
                    cm_asocket_close(sock);
                    return;
                }
            }
            else if (0 == ret)
            {
                cm_demo_printf("sock(%d) tcp connect_ok\n", sock);
            }

            test_client_tcp.socket = sock;
            test_client_tcp.protocol = TEST_PROTOCOL_TCP;
            test_client_tcp.server_port = test_tcp_port;
            memcpy(&test_client_tcp.addr, &server_addr, sizeof(server_addr));
        }
        else
        {
            cm_demo_printf("tcp open failed\n");
            return;
        }
    }
    else
    {
        /* UDP */
        sock = cm_asocket_open(AF_INET, SOCK_DGRAM, IPPROTO_UDP, __on_asocket_event, (void *)&test_client_udp);
        cm_demo_printf("sock(%d) open\n", sock);

        if (sock >= 0)
        {
            /* udp无连接 */
            struct sockaddr_in server_addr;
            memset(&server_addr, 0, sizeof(server_addr));

            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(test_udp_port);
            inet_pton(AF_INET, test_udp_addr, &server_addr.sin_addr);

            test_client_udp.socket = sock;
            test_client_udp.protocol = TEST_PROTOCOL_UDP;
            test_client_udp.server_port = test_udp_port;
            memcpy(&test_client_udp.addr, &server_addr, sizeof(server_addr));

            cm_demo_printf("sock(%d) udp connect_ok\n", sock);
        }
        else
        {
            cm_demo_printf("sock(%d) udp open failed\n", sock);
            return;
        }
    }
}

static void __on_cmd_open(char argc, char **argv)
{
    int ret = argv_sanity_check(argc, argv);
    if(ret != 0)
    {
        cm_demo_printf("argv_sanity_check failed ret[%d]\r\n",ret);
        return;
    }
    if(argc < 5)
    {
        cm_demo_printf("argc[%d] is too few\r\n",argc);
        return;
    }
    /* 处理参数 */
    int type = 0;//tcp 0 or udp 1
    int port = atoi((char *)argv[4]);//port
    cm_eloop_event_handle_t cmd_open_recv_event = NULL;
    ip_addr_t ip_addr={0};

    //判断argv[3]是否是规范ip地址
    ret = ipaddr_aton(argv[3], &ip_addr);
    if(ret != 1)
    {
        cm_demo_printf("__on_cmd_OPEN argv[3] is not ip_addr\n");
        return;
    }

    //判断argv[2]是否是tcp或udp
    if (strncmp(argv[2], STR_ITEM("tcp")) == 0)
    {
        type = 0;
        test_tcp_port = port;
        memset(test_tcp_addr, 0, sizeof(test_tcp_addr));
        snprintf(test_tcp_addr, sizeof(test_tcp_addr), "%s", argv[3]);
    }
    else if (strncmp(argv[2], STR_ITEM("udp")) == 0)
    {
        type = 1;
        test_udp_port = port;
        memset(test_udp_addr, 0, sizeof(test_udp_addr));
        snprintf(test_udp_addr, sizeof(test_udp_addr), "%s", argv[3]);
    }
    else
    {
        cm_demo_printf("__on_cmd_open argv[2] is error\n");
        return;
    }
    cm_demo_printf("__on_cmd_open mode=%d\n", type);

    cmd_open_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_open_recv_event, (void *)type);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_open_recv_event);
}

// CLOSE ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_close_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);
    if(cb_param == NULL)
    {
        cm_demo_printf("cb_param is null\n");
        return;
    }
    /* 取得参数 */
    asocket_test_client_t *test_client = (asocket_test_client_t *)cb_param;

    /* 处理命令 */
    int ret = cm_asocket_close(test_client->socket);

    if (0 == ret)
    {
        cm_demo_printf("sock(%d) close success\n", test_client->socket);
    }
    else
    {
        cm_demo_printf("sock(%d) close error(%d)(%d)\n", test_client->socket, ret, errno);
    }
}

static void __on_cmd_close(void *param)
{
    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_close_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_close_recv_event, (void *)param);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_close_recv_event);
}

// SEND ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_send_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);
    if(cb_param == NULL)
    {
        cm_demo_printf("cb_param is null\n");
        return;
    }

    /* 取得参数 */
    asocket_test_client_t *test_client = (asocket_test_client_t *)cb_param;

    int ret = -1;

    if (test_client == &test_client_tcp)
    {
        ret = cm_asocket_send(test_client->socket, "hello", 5, 0);
    }
    else if (test_client == &test_client_udp)
    {
        struct sockaddr addr = {0};
        memcpy(&addr, &test_client->addr, sizeof(test_client->addr));

        ret = cm_asocket_sendto(test_client->socket, "hello", 5, 0, &addr, sizeof(test_client->addr));
    }

    if (ret > 0)
    {
        cm_demo_printf("sock(%d) send len=%d\n", test_client->socket, ret);
    }
    else
    {
        cm_demo_printf("sock(%d) send error(%d)(%d)\n", test_client->socket, ret, errno);
    }
}

static void __on_cmd_send(void *param)
{
    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_send_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_send_recv_event, (void *)param);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_send_recv_event);
}

// DNS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 异步DNS事件回调函数(执行环境为eloop线程) */
static void __on_async_dns_event(int req_id, cm_async_dns_event_e event, void *cb_param,
                                 const char *host_name, const cm_async_dns_ip_addr_t *ip_addr)
{
    if(host_name == NULL || ip_addr == NULL)
    {
        cm_demo_printf("[DNS CB]invalid null input param\r\n");
        return;
    }
    if(host_name[0] == '\0')
    {
        cm_demo_printf("[DNS CB]empty hostname\r\n");
        return;
    }

    cm_demo_printf("__on_async_dns_event(%d, %d, %s)\n", req_id, event, host_name);

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

            cm_demo_printf("dns req(%d) ok: %s->%s\n", req_id, host_name, ip_str);
            break;
        }

        case CM_ASYNC_DNS_RESOLVE_FAIL:
        {
            cm_demo_printf("dns req(%d) fail: %s\n", req_id, host_name);
            break;
        }

        default:
        {
            break;
        }
    }
}

/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_dns_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);
    if(cb_param == NULL)
    {
        cm_demo_printf("cb_param is null\n");
        return;
    }

    /* 取得参数 */
    char *host_name = (char *)cb_param;

    cm_demo_printf("__on_eloop_cmd_dns_recv_event host_name=%s\n", host_name);

    /* 处理命令 */
    cm_async_dns_ip_addr_t ip_addr;
    int ret = cm_async_dns_request(host_name, CM_ASYNC_DNS_ADDRTYPE_IPV4_IPV6, &ip_addr, __on_async_dns_event, NULL);

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

        cm_demo_printf("dns req ok: %s->%s\n", host_name, ip_str);
    }

    else if (ret > 0)
    {
        cm_demo_printf("dns req(%d) pending...\n", ret);
    }

    else
    {
        cm_demo_printf("dns req fail %d\n", ret);
    }

    cm_free(host_name);
}

static void __on_cmd_dns(void *param)
{
    if(param == NULL)
    {
        cm_demo_printf("__on_cmd_dns6 param is null\r\n");
        return;
    }
    size_t len = strlen((char *)param);

    /* 处理参数 */
    char *host_name = (char *)cm_malloc(len + 1);
    if(host_name == NULL)
    {
        cm_demo_printf("host_name malloc failed\r\n");
        return;
    }
    memset(host_name, 0, len + 1);
    memcpy(host_name, param, len);

    cm_demo_printf("__on_cmd_dns host_name=%s\n", host_name);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_dns_recv_event = cm_eloop_register_event(cm_async_dns_eloop(), __on_eloop_cmd_dns_recv_event, (void *)host_name);
    if(cmd_dns_recv_event == NULL)
    {
        cm_free(host_name);
        host_name = NULL;
    }
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_dns_recv_event);
}

// DNS6 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 异步DNS事件回调函数(执行环境为eloop线程) */
static void __on_async_dns6_event(int req_id, cm_async_dns_event_e event, void *cb_param,
                                  const char *host_name, const cm_async_dns_ip_addr_t *ip_addr)
{
    if(host_name == NULL || ip_addr == NULL)
    {
        cm_demo_printf("[DNS CB]invalid null input param\r\n");
        return;
    }
    if(host_name[0] == '\0')
    {
        cm_demo_printf("[DNS CB]empty hostname\r\n");
        return;
    }

    cm_demo_printf("__on_async_dns6_event(%d, %d, %s)\n", req_id, event, host_name);

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

            cm_demo_printf("dns6 req(%d) ok: %s->%s\n", req_id, host_name, ip_str);
            break;
        }
        case CM_ASYNC_DNS_RESOLVE_FAIL:
        {
            cm_demo_printf("dns6 req(%d) fail: %s\n", req_id, host_name);
            break;
        }

        default:
        {
            break;
        }
    }
}

/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_dns6_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);
    if(cb_param == NULL)
    {
        cm_demo_printf("cb_param is null\n");
        return;
    }

    /* 取得参数 */
    char *host_name = (char *)cb_param;

    cm_demo_printf("__on_eloop_cmd_dns6_recv_event host_name=%s\n", host_name);

    /* 处理命令 */
    cm_async_dns_ip_addr_t ip_addr;
    int ret = cm_async_dns_request(host_name, CM_ASYNC_DNS_ADDRTYPE_IPV6_IPV4, &ip_addr, __on_async_dns6_event, NULL);

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

        cm_demo_printf("dns6 req ok: %s->%s\n", host_name, ip_str);
    }

    else if (ret > 0)
    {
        cm_demo_printf("dns6 req(%d) pending...\n", ret);
    }

    else
    {
        cm_demo_printf("dns6 req fail %d\n", ret);
    }

    cm_free(host_name);
}

static void __on_cmd_dns6(void *param)
{
    if(param == NULL)
    {
        cm_demo_printf("__on_cmd_dns6 param is null\r\n");
        return;
    }
    size_t len = strlen((char *)param);

    /* 处理参数 */
    char *host_name = (char *)cm_malloc(len + 1);
    if(host_name == NULL)
    {
        cm_demo_printf("host_name malloc failed\r\n");
        return;
    }
    memset(host_name, 0, len + 1);
    memcpy(host_name, param, len);

    cm_demo_printf("__on_cmd_dns6 host_name=%s\n", host_name);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_dns6_recv_event = cm_eloop_register_event(cm_async_dns_eloop(), __on_eloop_cmd_dns6_recv_event, (void *)host_name);
    if(cmd_dns6_recv_event == NULL)
    {
        cm_free(host_name);
        host_name = NULL;
    }
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_dns6_recv_event);
}

static void __on_async_dns_all_event(int req_id, cm_async_dns_event_e event, void *cb_param,
                                  const char *host_name, const cm_async_dns_ip_addr_t *ip_addr)
{
    if(host_name == NULL || ip_addr == NULL)
    {
        cm_demo_printf("[DNS CB]invalid null input param\r\n");
        return;
    }
    if(host_name[0] == '\0')
    {
        cm_demo_printf("[DNS CB]empty hostname\r\n");
        return;
    }
    cm_demo_printf("__on_async_dns_all_event(%d, %d, %s)\n", req_id, event, host_name);

    switch (event)
    {
        case CM_ASYNC_DNS_RESOLVE_OK:
        {
            int result_num = cm_async_dns_get_result_addr_num(ip_addr);

            for (int i = 0; i < result_num; i++)
            {
                char ip_str[128] = "";

                if (CM_ASYNC_DNS_ADDRTYPE_IPV4 == (&(ip_addr[i]))->type)
                {
                    inet_ntop(AF_INET, &((&(ip_addr[i]))->u_addr.sin_addr), ip_str, sizeof(ip_str));
                }

                else if (CM_ASYNC_DNS_ADDRTYPE_IPV6 == (&(ip_addr[i]))->type)
                {
                    inet_ntop(AF_INET6, &((&(ip_addr[i]))->u_addr.sin6_addr), ip_str, sizeof(ip_str));
                }

                cm_demo_printf("dns_all req(%d) ok: %s->%s\n", req_id, host_name, ip_str);
            }

            break;
        }
        case CM_ASYNC_DNS_RESOLVE_FAIL:
        {
            cm_demo_printf("dns_all req(%d) fail: %s\n", req_id, host_name);
            break;
        }

        default:
        {
            break;
        }
    }
}

/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_dns_all_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);
    if(cb_param == NULL)
    {
        cm_demo_printf("__on_eloop_cmd_dns_all_recv_event cb_param is null\r\n");
        return;
    }

    /* 取得参数 */
    char *host_name = (char *)cb_param;

    cm_demo_printf("__on_eloop_cmd_dns_all_recv_event host_name=%s\n", host_name);

    /* 处理命令 */
    cm_async_dns_ip_addr_t ip_addr[4] = {0};
    int ret = cm_async_dns_req_all_v2(host_name, CM_ASYNC_DNS_ADDRTYPE_IPV6_IPV4, (cm_async_dns_ip_addr_t *)&ip_addr, 4, __on_async_dns_all_event, NULL, 0xff);

    if (0 == ret)
    {
        __on_async_dns_all_event(0, CM_ASYNC_DNS_RESOLVE_OK, NULL, host_name, (cm_async_dns_ip_addr_t *)&ip_addr);
    }

    else if (ret > 0)
    {
        cm_demo_printf("dns_all req(%d) pending...\n", ret);
    }

    else
    {
        cm_demo_printf("dns_all req fail %d\n", ret);
    }

    cm_free(host_name);
}

static void __on_cmd_dns_all(void *param)
{
    if(param == NULL)
    {
        cm_demo_printf("__on_cmd_dns_all param is null\r\n");
        return;
    }
    size_t len = strlen((char *)param);
    /* 处理参数 */
    char *host_name = (char *)cm_malloc(len + 1);
    if(host_name == NULL)
    {
        cm_demo_printf("host_name malloc failed\r\n");
        return;
    }
    memset(host_name, 0, len + 1);
    memcpy(host_name, param, len);

    cm_demo_printf("__on_cmd_dns_all host_name=%s\n", host_name);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_dns_recv_event = cm_eloop_register_event(cm_async_dns_eloop(), __on_eloop_cmd_dns_all_recv_event, (void *)host_name);
    if(cmd_dns_recv_event == NULL)
    {
        cm_free(host_name);
        host_name = NULL;
    }
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_dns_recv_event);
}

/* eloop接收到事件后执行回调函数 */
static void __on_eloop_cmd_eloop_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event，每次执行回调函数需要手动注销event */
    cm_eloop_unregister_event(event);
    cm_demo_printf("eloop post test\n");
}

/* eloop执行线程 */
static void __eloop_thread_proc(void *argument)
{
    cm_demo_printf("__eloop_thread_proc\n");

    /* 循环接收事件 */
    while (1)
    {
        int ret = cm_eloop_wait_event(test_eloop, 5000);

        if (ret < 0)
        {
            break;
        }

        cm_demo_printf("eloop wait event timeout\n");
    }

    /* 删除eloop */
    cm_eloop_delete(test_eloop);
}

/* 创建eloop测试模块 */
/* eloop用于实现事件驱动架构 */
static void __test_eloop_init(void)
{
    /* 创建eloop */
    test_eloop = cm_eloop_create(64);

    /* 创建eloop执行线程 */
    osThreadAttr_t thread_attr = {0};
    thread_attr.name = "demo_eloop_main";
    thread_attr.stack_size = 1024 * 2;
    osThreadNew(__eloop_thread_proc, NULL, &thread_attr);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
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
static int argv_idx_safe(int argc, char **argv, int idx)
{
    if(idx < 0 || idx >= argc)
    {
        return 0;
    }
    return (argv[idx] != NULL);
}
/* 执行测试命令 */
void cm_test_asocket(char argc, char **argv)
{
    int ret = argv_sanity_check(argc, argv);
    if(ret != 0)
    {
        cm_demo_printf("argv_sanity_check failed ret[%d]\r\n",ret);
        return;
    }

    /* asocket open [mode]... */
    /* asocket open tcp
       asocket open udp       */
    if (argv_idx_safe(argc, argv, 1) && strncmp(argv[1], STR_ITEM("open")) == 0)
    {
        /* 在eloop环境下执行连接，异步执行，连接结果在回调函数中获取 */
        __on_cmd_open(argc,argv);
    }

    /* asocket close [mode]... */
    /* asocket close tcp
       asocket close udp       */
    else if (argv_idx_safe(argc, argv, 1) && strncmp(argv[1], STR_ITEM("close")) == 0)
    {
        if (argv_idx_safe(argc, argv, 2) && strncmp(argv[2], STR_ITEM("tcp")) == 0)
        {
            __on_cmd_close((void *)&test_client_tcp);
        }
        else if (argv_idx_safe(argc, argv, 2) && strncmp(argv[2], STR_ITEM("udp")) == 0)
        {
            __on_cmd_close((void *)&test_client_udp);
        }
    }

    /* asocket send [mode]... */
    /* asocket send tcp
       asocket send udp       */
    else if (argv_idx_safe(argc, argv, 1) && strncmp(argv[1], STR_ITEM("send")) == 0)
    {
        if (argv_idx_safe(argc, argv, 2) && strncmp(argv[2], STR_ITEM("tcp")) == 0)
        {
            __on_cmd_send((void *)&test_client_tcp);
        }
        else if (argv_idx_safe(argc, argv, 2) && strncmp(argv[2], STR_ITEM("udp")) == 0)
        {
            __on_cmd_send((void *)&test_client_udp);
        }
    }

    /* asocket dns_set */
    else if (argv_idx_safe(argc, argv, 1) && strncmp(argv[1], STR_ITEM("dns_set")) == 0)
    {
        cm_async_dns_ip_addr_t dns_server;
        uint8_t max_retries;
        int32_t timeout;
        cm_async_dns_addrtype_e addr_type;
        int32_t ret;
        int i;

        /* 测试cm_async_dns_get_server */
        cm_demo_printf("Testing cm_async_dns_get_server:\n");
        for (i = 0; i < 4; i++) {
            ret = cm_async_dns_get_server(i, &dns_server);
            if (ret == 0) {
                char ip_str[64] = "";
                if (dns_server.type == CM_ASYNC_DNS_ADDRTYPE_IPV4) {
                    inet_ntop(AF_INET, &(dns_server.u_addr.sin_addr), ip_str, sizeof(ip_str));
                    cm_demo_printf("  DNS Server %d (IPv4): %s\n", i, ip_str);
                } else if (dns_server.type == CM_ASYNC_DNS_ADDRTYPE_IPV6) {
                    inet_ntop(AF_INET6, &(dns_server.u_addr.sin6_addr), ip_str, sizeof(ip_str));
                    cm_demo_printf("  DNS Server %d (IPv6): %s\n", i, ip_str);
                }
            } else {
                cm_demo_printf("  Failed to get DNS Server %d, ret=%ld\n", i, ret);
            }
        }

        /* 测试cm_async_dns_set_max_retries和cm_async_dns_get_max_retries */
        cm_demo_printf("Testing cm_async_dns_set_max_retries and cm_async_dns_get_max_retries:\n");
        ret = cm_async_dns_set_max_retries(3);
        cm_demo_printf("  Set max retries result: %ld\n", ret);

        ret = cm_async_dns_get_max_retries(&max_retries);
        cm_demo_printf("  Get max retries result: %ld, value: %d\n", ret, max_retries);

        /* 测试cm_async_dns_set_timeout和cm_async_dns_get_timeout */
        cm_demo_printf("Testing cm_async_dns_set_timeout and cm_async_dns_get_timeout:\n");
        cm_async_dns_set_timeout(10);
        cm_demo_printf("  Set timeout to 10 seconds\n");

        timeout = cm_async_dns_get_timeout();
        cm_demo_printf("  Get timeout value: %ld seconds\n", timeout);

        /* 测试cm_async_dns_get_type_by_priority */
        cm_demo_printf("Testing cm_async_dns_get_type_by_priority:\n");
        addr_type = cm_async_dns_get_type_by_priority();
        cm_demo_printf("  Address type priority: %d\n", addr_type);

        /* 测试cm_async_dns_get_type_by_priority_ex */
        cm_demo_printf("Testing cm_async_dns_get_type_by_priority_ex:\n");
        addr_type = cm_async_dns_get_type_by_priority_ex(1);
        cm_demo_printf("  Address type priority for CID 1: %d\n", addr_type);
    }

    /* asocket dns_all [host_name]... */
    else if (argv_idx_safe(argc, argv, 1) && strncmp(argv[1], STR_ITEM("dns_all")) == 0)
    {
        __on_cmd_dns_all((void *)argv[2]);
    }

    /* asocket dns6 [host_name]... */
    else if (argv_idx_safe(argc, argv, 1) && strncmp(argv[1], STR_ITEM("dns6")) == 0)
    {
        __on_cmd_dns6((void *)argv[2]);
    }

    /* asocket dns [host_name]... */
    else if (argv_idx_safe(argc, argv, 1) && strncmp(argv[1], STR_ITEM("dns")) == 0)
    {
        __on_cmd_dns((void *)argv[2]);
    }

    /* eloop 测试示例 */
    /* asocket eloop init    创建一个新的eloop */
    /* asocket eloop post    测试eloop事件 */
    else if (argv_idx_safe(argc, argv, 1) && strncmp(argv[1], STR_ITEM("eloop")) == 0)
    {
        if (argv_idx_safe(argc, argv, 2) && strncmp(argv[2], STR_ITEM("init")) == 0)
        {
            /* 创建一个新的eloop */
            __test_eloop_init();
        }

        else if (argv_idx_safe(argc, argv, 2) && strncmp(argv[2], STR_ITEM("post")) == 0)
        {
            /* 向新的eloop发送事件 */
            cm_eloop_event_handle_t cmd_post_recv_event = cm_eloop_register_event(test_eloop, __on_eloop_cmd_eloop_recv_event, NULL);
            cm_eloop_post_event(cmd_post_recv_event);
        }
    }
    else
    {
        cm_demo_printf("cm_ntp parameter invalid\r\n");
    }
}


NR_SHELL_CMD_EXPORT(cm_asocket, cm_test_asocket);


#endif
#endif

