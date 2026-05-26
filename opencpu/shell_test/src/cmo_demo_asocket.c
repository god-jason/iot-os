#ifdef CM_DEMO_ASOCKET_SUPPORT
#ifdef OS_USING_SHELL
#include "lwip/sockets.h"
#include "lwip/arch.h"
#include "cm_asocket.h"
#include "cm_eloop.h"
#include "cm_async_dns.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <nr_micro_shell.h>
#include "cm_ssl.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define cm_demo_printf osPrintf
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
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

static cm_eloop_handle_t test_eloop = NULL;     //eloop测试
osThreadId_t ThreadId = NULL;

/** IP地址(异步DNS解析结果)类型 */
typedef struct cm_asocket_addr
{
    int addr_port;
    ip_addr_t addr;
    int conn_type;
} cm_asocket_addr_t;
static cm_asocket_addr_t conn_info = {0};
//static struct in6_addr asocket_sin6_addr; /**< IPv6 address */

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* 异步socket消息回调(执行环境为eloop线程) */
static void __on_asocket_event(int sock, cm_asocket_event_e event, void *user_param)
{
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
            cm_asocket_ioctl(sock, FIONREAD, &recv_avail);

            /* 接收数据 */
            uint8_t recv_buf[128] = {0};
            int ret = cm_asocket_recv(sock, recv_buf, sizeof(recv_buf), 0);

            if (ret > 0)
            {
                cm_demo_printf("sock(%d) recv_ind: recv_avail=%d, recv_len=%d, data=%.*s\n", sock, recv_avail, ret, ret, recv_buf);
            }

            else
            {
                cm_demo_printf("sock(%d) recv_ind error(%d)\n", sock, errno);

                if (ENOTCONN == errno)
                {
                    /* Connection closed */
                    cm_demo_printf("sock(%d) recv_ind: Connection closed\n", sock);
                }
            }

            break;
        }

        case CM_ASOCKET_EV_SEND_IND:
        {
            cm_demo_printf("sock(%d) send_ind\n", sock);
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
static void __on_eloop_cmd_OPEN_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    cm_asocket_addr_t *conn_info = (cm_asocket_addr_t *)cb_param;
    cm_demo_printf("__on_eloop_cmd_OPEN_recv_event type=%d\n", conn_info->conn_type);

    /* 处理命令 */
    int sock = -1;
    if (0 == conn_info->conn_type)
    {
        /* TCP */
        sock = cm_asocket_open(AF_INET, SOCK_STREAM, IPPROTO_TCP, __on_asocket_event, NULL);
    }

    else
    {
        /* UDP */
        sock = cm_asocket_open(AF_INET, SOCK_DGRAM, IPPROTO_UDP, __on_asocket_event, NULL);
    }

    if (-1 == sock)
    {
        cm_demo_printf("open failed\n");
        return;
    }

    cm_demo_printf("sock(%d) open\n", sock);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = conn_info->addr.u_addr.ip4.addr;
    server_addr.sin_port = htons(conn_info->addr_port);

    int ret = cm_asocket_connect(sock, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    if (ret < 0)
    {
        if (EINPROGRESS == errno)
        {
            cm_demo_printf("sock(%d) open request success, wait connect...\n", sock);
        }

        else
        {
            cm_demo_printf("sock(%d) connect failed(%d)\n", sock, errno);
        }
    }

    else if (0 == ret)
    {
        cm_demo_printf("sock(%d) connect_ok\n", sock);
    }
}

// OPEN ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_OPEN_recv_event6(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    cm_asocket_addr_t *conn_info = (cm_asocket_addr_t *)cb_param;
    cm_demo_printf("__on_eloop_cmd_OPEN_recv_event type=%d\n", conn_info->conn_type);

    /* 处理命令 */
    int sock = -1;
    if (0 == conn_info->conn_type)
    {
        /* TCP */
        sock = cm_asocket_open(AF_INET6, SOCK_STREAM, IPPROTO_TCP, __on_asocket_event, NULL);
    }

    else
    {
        /* UDP */
        sock = cm_asocket_open(AF_INET6, SOCK_DGRAM, IPPROTO_UDP, __on_asocket_event, NULL);
    }

    if (-1 == sock)
    {
        cm_demo_printf("open failed\n");
        return;
    }

    cm_demo_printf("sock(%d) open\n", sock);

    struct sockaddr_in6 server_addr6;
    memset(&server_addr6, 0, sizeof(server_addr6));

    server_addr6.sin6_len = sizeof(struct sockaddr_in6);
    server_addr6.sin6_family = AF_INET6;

    server_addr6.sin6_port = htons(conn_info->addr_port);

    inet6_addr_from_ip6addr(&server_addr6.sin6_addr, &(conn_info->addr.u_addr.ip6));

//    memcpy(&(server_addr6.sin6_addr),&asocket_sin6_addr,sizeof(asocket_sin6_addr));

    int ret = cm_asocket_connect(sock, (struct sockaddr *)&server_addr6,sizeof(server_addr6));

    if (ret < 0)
    {
        if (EINPROGRESS == errno)
        {
            cm_demo_printf("sock(%d) open request success, wait connect...\n", sock);
        }

        else
        {
            cm_demo_printf("sock(%d) connect failed(%d)\n", sock, errno);
        }
    }
    else if (0 == ret)
    {
        cm_demo_printf("sock(%d) connect_ok\n", sock);
    }
}

static void __on_cmd_OPEN(char **cmd,int len)
{
    /* 处理参数 */
    int type = atoi((char *)cmd[2]);//tcp 0 or udp 1
    char *ip_addr = (char *)cmd[3];//address
    int port = atoi((char *)cmd[4]);//port
    cm_eloop_event_handle_t cmd_OPEN_recv_event = NULL;

    cm_demo_printf("__on_cmd_OPEN type=%d\n", type);
    memset(&conn_info, 0, sizeof(cm_asocket_addr_t));
    conn_info.addr_port = port;
    conn_info.conn_type = type;
    int32_t ret = ipaddr_aton(ip_addr, &conn_info.addr);
    if(ret != 1)
    {
        cm_demo_printf("__on_cmd_OPEN ipaddr_aton error\n");
        return;
    }
    /* 注册Event(将命令封装成Event发送到eloop执行) */
    if(conn_info.addr.type == IPADDR_TYPE_V4)
    {
        cmd_OPEN_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_OPEN_recv_event, (void *)&conn_info);
    }
    else
    {
        cmd_OPEN_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_OPEN_recv_event6, (void *)&conn_info);
    }
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_OPEN_recv_event);
}

// CLOSE ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_CLOSE_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    int sock = (int)cb_param;

    cm_demo_printf("__on_eloop_cmd_CLOSE_recv_event sock=%d\n", sock);

    /* 处理命令 */
    int ret = cm_asocket_close(sock);

    if (0 == ret)
    {
        cm_demo_printf("sock(%d) close success\n", sock);
    }

    else
    {
        cm_demo_printf("sock(%d) close error(%d)\n", sock, errno);
    }
}

static void __on_cmd_CLOSE(void *param)
{
    /* 处理参数 */
    int sock = atoi(((char **)param)[0]);

    cm_demo_printf("__on_cmd_CLOSE sock=%d\n", sock);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_CLOSE_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_CLOSE_recv_event, (void *)sock);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_CLOSE_recv_event);
}

// SEND ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 执行环境为eloop线程(和异步socket在同一个线程,可以安全地处理socket创建和数据收发等操作) */
static void __on_eloop_cmd_SEND_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    int sock = (int)cb_param;

    cm_demo_printf("__on_eloop_cmd_SEND_recv_event sock=%d\n", sock);

    /* 处理命令 */
    if (-1 != sock)
    {
        int ret = cm_asocket_send(sock, "hello", 5, 0);

        if (ret > 0)
        {
            cm_demo_printf("sock(%d) send len=%d\n", sock, ret);
        }

        else
        {
            cm_demo_printf("sock(%d) send error(%d)\n", sock, ret);
        }
    }
}

static void __on_cmd_SEND(void *param)
{
    /* 处理参数 */
    int sock = atoi(((char **)param)[0]);

    cm_demo_printf("__on_cmd_SEND sock=%d\n", sock);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_SEND_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_SEND_recv_event, (void *)sock);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_SEND_recv_event);
}

// DNS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 异步DNS事件回调函数(执行环境为eloop线程) */
static void __on_async_dns_event(int req_id, cm_async_dns_event_e event, void *cb_param,
                                 const char *host_name, const cm_async_dns_ip_addr_t *ip_addr)
{
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
static void __on_eloop_cmd_DNS_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    char *host_name = (char *)cb_param;

    cm_demo_printf("__on_eloop_cmd_DNS_recv_event host_name=%s\n", host_name);

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

    free(host_name);
}

static void __on_cmd_DNS(void *param)
{
    /* 处理参数 */
    char *host_name = strdup(((char **)param)[0]);
    assert(NULL != host_name);

    cm_demo_printf("__on_cmd_DNS host_name=%s\n", host_name);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_DNS_recv_event = cm_eloop_register_event(cm_async_dns_eloop(), __on_eloop_cmd_DNS_recv_event, (void *)host_name);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_DNS_recv_event);
}

// DNS6 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 异步DNS事件回调函数(执行环境为eloop线程) */
static void __on_async_dns6_event(int req_id, cm_async_dns_event_e event, void *cb_param,
                                  const char *host_name, const cm_async_dns_ip_addr_t *ip_addr)
{
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
static void __on_eloop_cmd_DNS6_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 取得参数 */
    char *host_name = (char *)cb_param;

    cm_demo_printf("__on_eloop_cmd_DNS6_recv_event host_name=%s\n", host_name);

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
            //memcpy(&asocket_sin6_addr,&(ip_addr.u_addr.sin6_addr),sizeof(asocket_sin6_addr));
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

    free(host_name);
}

static void __on_cmd_DNS6(void *param)
{
    /* 处理参数 */
    char *host_name = strdup(((char **)param)[0]);
    assert(NULL != host_name);

    cm_demo_printf("__on_cmd_DNS6 host_name=%s\n", host_name);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_DNS6_recv_event = cm_eloop_register_event(cm_async_dns_eloop(), __on_eloop_cmd_DNS6_recv_event, (void *)host_name);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_DNS6_recv_event);
}

/* eloop接收到事件后执行回调函数 */
static void __on_eloop_cmd_ELOOP_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event，每次执行回调函数需要手动注销event */
    cm_eloop_unregister_event(event);
    cm_demo_printf("cm_eloop_unregister_event succ \n");
}

/* eloop执行线程 */
static void __eloop_thread_proc(void *argument)
{
    cm_demo_printf("__eloop_thread_proc\n");

    /* 循环接收事件 */
    while (1)
    {
        int ret = cm_eloop_wait_event(test_eloop, 5000);
        cm_demo_printf("rdy_count=%d\n", ret);
        if (ret < 0)
        {
            break;
        }
        else if (ret > 0)
        {
            cm_demo_printf("cm_eloop_post_event succ \n");
        }
        else
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
    ThreadId = osThreadNew(__eloop_thread_proc, NULL, &thread_attr);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* 执行测试命令 */
void SHELL_testAsocket(char argc, char **argv)
{
    const char *sub_cmd = (const char *)argv[1];

    /* CM:ASOCKET:OPEN:[type]... */
    /* CM:ASOCKET:OPEN:0    TCP
       CM:ASOCKET:OPEN:1    UDP  */
    if (strncmp(sub_cmd, STR_ITEM("OPEN")) == 0)
    {
        /* 在eloop环境下执行连接，异步执行，连接结果在回调函数中获取 */
        __on_cmd_OPEN(argv,0);
    }

    /* CM:ASOCKET:CLOSE:[sock]... */
    /* sock号在连接时会打印 */
    else if (strncmp(sub_cmd, STR_ITEM("CLOSE")) == 0)
    {
        __on_cmd_CLOSE(argv + 2);
    }

    /* CM:ASOCKET:SEND:[sock]... */
    /* sock号在连接时会打印 */
    else if (strncmp(sub_cmd, STR_ITEM("SEND")) == 0)
    {
        __on_cmd_SEND(argv + 2);
    }

    /* CM:ASOCKET:DNS6:[host_name]... */
    else if (strncmp(sub_cmd, STR_ITEM("DNS6")) == 0)
    {
        __on_cmd_DNS6(argv + 2);
    }

    /* CM:ASOCKET:DNS:[host_name]... */
    else if (strncmp(sub_cmd, STR_ITEM("DNS")) == 0)
    {
        __on_cmd_DNS(argv + 2);
    }

    /* eloop 测试示例 */
    /* CM:ASOCKET:ELOOP:INIT    创建一个新的eloop */
    /* CM:ASOCKET:ELOOP:POST    测试eloop事件 */
    else if (strncmp(sub_cmd, STR_ITEM("ELOOP")) == 0)
    {
        const char *eloop_cmd = (const char *)argv[2];

        if (strncmp(eloop_cmd, STR_ITEM("INIT")) == 0)
        {
            /* 创建一个新的eloop */
            __test_eloop_init();
            cm_demo_printf("cm_eloop_create succ! \n");
        }
        else if (strncmp(eloop_cmd, STR_ITEM("POST")) == 0)
        {
            cm_eloop_event_handle_t  cmd_POST_recv_event = cm_eloop_register_event(test_eloop, __on_eloop_cmd_ELOOP_recv_event, NULL);
            cm_demo_printf("cm_eloop_register_event succ! \n");
            /* 向新的eloop发送事件 */
            cm_eloop_post_event(cmd_POST_recv_event);
        }
        else if(strncmp(eloop_cmd, STR_ITEM("DELE")) == 0)
        {
            osThreadTerminate(ThreadId);
            /* 删除eloop */
            cm_eloop_delete(test_eloop);
            cm_demo_printf("cm_eloop_delete succ! \n");
        }
    }
}


NR_SHELL_CMD_EXPORT(cm_asocket, SHELL_testAsocket);


#endif
#endif

