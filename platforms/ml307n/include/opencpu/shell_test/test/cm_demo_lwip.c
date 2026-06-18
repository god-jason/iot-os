/*********************************************************
 *  @file    cm_demo_lwip.c
 *  @brief   OpenCPU lwip示例
 *  Copyright (c) 2023 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot1325 2023/6/29
 ********************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h> 
#include "string.h"
#include "cm_os.h"
#include "lwip/lwip/netdb.h"
#include "lwip/lwip/sockets.h"

#include "cm_demo_lwip.h"
#include "cm_demo_uart.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

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

/* 测试前请先补充如下参数 */
static char *test_dns_addr = "www.baidu.com";     //DNS测试域名
static char *test_tcp_addr = "";     //TCP测试地址 例"192.168.0.1"
static char *test_udp_addr = "";     //UDP测试地址 例"192.168.0.1"
static int test_tcp_port = 9999;       //TCP测试端口
static int test_udp_port = 9999;       //UDP测试端口

static int test_tcp_sock = -1;
static int test_udp_sock = -1;
static osThreadId_t tcp_recv_task_handle = NULL;
static osThreadId_t udp_recv_task_handle = NULL;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* DNS示例 */
static void __cm_test_lwip_dns(void)
{
    char host_ip[20] = {0};

    struct hostent *host = gethostbyname(test_dns_addr);    //请先补充测试域名

    if (host == NULL)
    {
        cm_demo_printf("DNS error\n");
        return;
    }

    inet_ntop(host->h_addrtype, host->h_addr_list[0], host_ip, sizeof(host_ip));

    cm_demo_printf("host ip:%s\n", host_ip);
}

/* TCP连接示例 */
static int __cm_test_lwip_conn_tcp(void)
{
    int ret = 0;

    test_tcp_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (test_tcp_sock == -1)
    {
        cm_demo_printf("tcp socket create error\n");
        return -1;
    }
    
    cm_demo_printf("tcp socketid %d\n", test_tcp_sock);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_len = sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(test_tcp_port);            //请先补充测试端口
    server_addr.sin_addr.s_addr = inet_addr(test_tcp_addr); //请先补充测试地址

    ret = connect(test_tcp_sock, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    if (ret == -1)
    {
        cm_demo_printf("tcp connect error\n");
        close(test_tcp_sock);
        return -1;
    }

    cm_demo_printf("tcp connect succ\n");
    return 0;
}

/* UDP连接，由于UDP是面向无连接传输，所以只需要创建socket即可 */
static int __cm_test_lwip_conn_udp(void)
{
    test_udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (test_udp_sock == -1)
    {
        cm_demo_printf("udp socket create error\n");
        return -1;
    }
    
    cm_demo_printf("udp socketid %d\n", test_udp_sock);

    return 0;
}

/* TCP发送示例 */
static void __cm_test_lwip_send_tcp(void)
{
    int ret = 0;
    char test_msg[] = "tcp network test\n";
    
    ret = send(test_tcp_sock, (char *)test_msg, strlen(test_msg), 0);
    cm_demo_printf("tcp send ret %d\n", ret);

    if (ret < 0)
    {
        cm_demo_printf("tcp send error\n");
        close(test_tcp_sock);
    }
}

/* UDP发送示例 */
static void __cm_test_lwip_send_udp(void)
{
    int ret = 0;
    char test_msg[] = "udp network test\n";
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_len = sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(test_udp_port);            //请先补充测试端口
    server_addr.sin_addr.s_addr = inet_addr(test_udp_addr); //请先补充测试地址

    ret = sendto(test_udp_sock, (char *)test_msg, strlen(test_msg), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    cm_demo_printf("udp send ret %d\n", ret);

    if (ret < 0)
    {
        cm_demo_printf("udp send error\n");
    }
}

/* tcp接收线程，仅供参考 */
static void __cm_test_lwip_tcp_recv_task(void)
{
    int ret = 0;
    fd_set sets;
    struct timeval timeout;
    char buf[128] = {0};

    do {
        FD_ZERO(&sets);
        FD_SET(test_tcp_sock, &sets);

        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        ret = select(test_tcp_sock + 1, &sets, NULL, NULL, &timeout);
        
        if (ret > 0) 
        {
            ret = recv(test_tcp_sock, buf, sizeof(buf), 0);
            
            if (ret > 0) 
            {
                cm_demo_printf("tcp_read_test recv %s\n", buf);
                memset(buf, 0, sizeof(buf));
            } 
            else if (ret <= 0)
            {
                /* 走到这说明连接已断开，客户端需要执行close */
                cm_demo_printf("tcp_read_test closed %d\n", errno);
                close(test_tcp_sock);
            }
        } 
        else if (ret == 0)
        {
            /* 走到这是select等待超时 */
            cm_demo_printf("tcp_read_test select timeout %d\n", errno);
        } 
        else 
        {
            /* 走到这是当前socket已关闭，此时select将一直报错，故而挂起线程，等待重新连接后再次恢复线程 */
            osThreadSuspend(tcp_recv_task_handle);
        }
    } while (1);
}

static void __cm_test_lwip_udp_recv_task(void)
{
    int ret = 0;
    fd_set sets;
    struct timeval timeout;
    char buf[128] = {0};

    do {
        FD_ZERO(&sets);
        FD_SET(test_udp_sock, &sets);

        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        ret = select(test_udp_sock + 1, &sets, NULL, NULL, &timeout);
        
        if (ret > 0) 
        {
            ret = recv(test_udp_sock, buf, sizeof(buf), 0);
            
            if (ret > 0) 
            {
                cm_demo_printf("udp_read_test recv %s\n", buf);
                memset(buf, 0, sizeof(buf));
            } 
            else if (ret <= 0)
            {
                
                /* 对于UDP一般不会到这，因为服务器关闭UDP连接后，客户端一般不会收到消息 */
                cm_demo_printf("udp_read_test closed %d\n", errno);
                close(test_udp_sock);
            }
        } 
        else if (0 == ret) 
        {
            /* 走到这是select等待超时 */
            cm_demo_printf("udp_read_test select timeout %d\n", errno);
        } 
        else 
        {
            /* 走到这是当前socket已关闭，此时select将一直报错，故而挂起线程，等待重新连接后再次恢复线程 */
            osThreadSuspend(udp_recv_task_handle);
        }
    } while (1);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* 执行测试命令 */
void cm_test_lwip(EmbeddedCli *cli, char *args, void *context)
{
    /* lwip [cmd]... */
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }

    int ret = -1;

    /* DNS测试 */
    /* lwip dns */
    if (strncmp(cmd, STR_ITEM("dns")) == 0)
    {
        __cm_test_lwip_dns();
    }

    /* 连接测试 */
    /* lwip conn [mode] */
    else if (strncmp(cmd, STR_ITEM("conn")) == 0)
    {
        const char *param = embeddedCliGetToken(args, 2);
        if (param == NULL)
        {
            cm_demo_printf("invalid param\n");
            return;
        }
        
        const char *mode = (const char *)param;

        /* lwip conn tcp */
        if (strncmp(mode, STR_ITEM("tcp")) == 0)
        {
            ret = __cm_test_lwip_conn_tcp();
            if (ret == 0)
            {
                /* 建立新的线程供select异步读取，仅供参考 */
                if (tcp_recv_task_handle == NULL)
                {
                    osThreadAttr_t tcp_recv_task_attr = {0};
                    tcp_recv_task_attr.name  = "tcp_test";
                    tcp_recv_task_attr.stack_size = 1024 * 2;
                    tcp_recv_task_attr.priority = osPriorityNormal;
                    
                    tcp_recv_task_handle = osThreadNew((osThreadFunc_t)__cm_test_lwip_tcp_recv_task, 0, &tcp_recv_task_attr);
                }
                else
                {
                    osThreadResume(tcp_recv_task_handle);
                }
                
            }
        }

        /* lwip conn udp */
        else if (strncmp(mode, STR_ITEM("udp")) == 0)
        {
            ret = __cm_test_lwip_conn_udp();
            if (ret == 0)
            {
                /* 建立新的线程供select异步读取，仅供参考 */
                if (udp_recv_task_handle == NULL)
                {
                    osThreadAttr_t udp_recv_task_attr = {0};
                    udp_recv_task_attr.name  = "udp_test";
                    udp_recv_task_attr.stack_size = 1024 * 2;
                    udp_recv_task_attr.priority = osPriorityNormal;

                    udp_recv_task_handle = osThreadNew((osThreadFunc_t)__cm_test_lwip_udp_recv_task, 0, &udp_recv_task_attr);
                }
                else
                {
                    osThreadResume(udp_recv_task_handle);
                }
            }
        }
    }

    /* 发送测试 */
    /* lwip send [mode] */
    else if (strncmp(cmd, STR_ITEM("send")) == 0)
    {
        const char *param = embeddedCliGetToken(args, 2);
        if (param == NULL)
        {
            cm_demo_printf("invalid param\n");
            return;
        }
        
        const char *mode = (const char *)param;

        /* lwip send tcp */
        if (strncmp(mode, STR_ITEM("tcp")) == 0)
        {
            __cm_test_lwip_send_tcp();
        }

        /* lwip send udp */
        else if (strncmp(mode, STR_ITEM("udp")) == 0)
        {
            __cm_test_lwip_send_udp();
        }
    }

    /* 关闭测试 */
    /* lwip close [mode] */
    else if (strncmp(cmd, STR_ITEM("close")) == 0)
    {
        const char *param = embeddedCliGetToken(args, 2);
        if (param == NULL)
        {
            cm_demo_printf("invalid param\n");
            return;
        }
        
        const char *mode = (const char *)param;

        /* lwip close tcp */
        if (strncmp(mode, STR_ITEM("tcp")) == 0)
        {
            close(test_tcp_sock);
        }

        /* lwip close udp */
        else if (strncmp(mode, STR_ITEM("udp")) == 0)
        {
            close(test_udp_sock);
        }
    }
}
