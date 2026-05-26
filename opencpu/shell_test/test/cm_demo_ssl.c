/*********************************************************
 *  @file    cm_demo_ssl.c
 *  @brief   OpenCPU ssl示例
 *  Copyright (c) 2023 China Mobile IOT.
 *  All rights reserved.
 *  created by ShiMingRui 2023/6/29
 ********************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lwip/sockets.h"
#include "lwip/arch.h"
#include "cm_asocket.h"
#include "cm_os.h"
#include "cm_demo_uart.h"
#include "cm_demo_ssl.h"
#include <assert.h>
#include "cm_sys.h"
#include "cm_ssl.h"
#include "cm_mem.h"
#include "mbedtls/ssl.h"

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
static char *test_ssl_addr = "";     //测试地址, 例: "192.168.0.1"
static int test_ssl_port = 9999;       //测试端口

static void *test_ssl_ctx = NULL;          //ssl 上下文
static int test_ssl_sock = -1;             //ssl sock
static int is_shaking_hands = 0;           //ssl握手标志，握手阶段不能进行数据读取

static char *test_serve_ca_filename = "serve_ca";
static char *test_client_ca_filename = "client_ca";
static char *test_client_key_filename = "client_key";

/* ca证书 */
static char *test_serve_ca = "";

/* 客户端证书 */
static char *test_client_ca = "";

/* 客户端密钥 */
static char *test_client_key = "";

/* 证书格式如下，以www.baidu.com根证书为例 */
/*static char *http_ca = "-----BEGIN CERTIFICATE-----\r\n" \
"MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n" \
"A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n" \
"b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n" \
"MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n" \
"YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n" \
"aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n" \
"jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n" \
"xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n" \
"1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n" \
"snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n" \
"U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n" \
"9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n" \
"BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n" \
"AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n" \
"yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n" \
"38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n" \
"AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n" \
"DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n" \
"HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n" \
"-----END CERTIFICATE-----\r\n" \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIEaTCCA1GgAwIBAgILBAAAAAABRE7wQkcwDQYJKoZIhvcNAQELBQAwVzELMAkG\r\n" \
"A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n" \
"b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw0xNDAyMjAxMDAw\r\n" \
"MDBaFw0yNDAyMjAxMDAwMDBaMGYxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n" \
"YWxTaWduIG52LXNhMTwwOgYDVQQDEzNHbG9iYWxTaWduIE9yZ2FuaXphdGlvbiBW\r\n" \
"YWxpZGF0aW9uIENBIC0gU0hBMjU2IC0gRzIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\r\n" \
"DwAwggEKAoIBAQDHDmw/I5N/zHClnSDDDlM/fsBOwphJykfVI+8DNIV0yKMCLkZc\r\n" \
"C33JiJ1Pi/D4nGyMVTXbv/Kz6vvjVudKRtkTIso21ZvBqOOWQ5PyDLzm+ebomchj\r\n" \
"SHh/VzZpGhkdWtHUfcKc1H/hgBKueuqI6lfYygoKOhJJomIZeg0k9zfrtHOSewUj\r\n" \
"mxK1zusp36QUArkBpdSmnENkiN74fv7j9R7l/tyjqORmMdlMJekYuYlZCa7pnRxt\r\n" \
"Nw9KHjUgKOKv1CGLAcRFrW4rY6uSa2EKTSDtc7p8zv4WtdufgPDWi2zZCHlKT3hl\r\n" \
"2pK8vjX5s8T5J4BO/5ZS5gIg4Qdz6V0rvbLxAgMBAAGjggElMIIBITAOBgNVHQ8B\r\n" \
"Af8EBAMCAQYwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUlt5h8b0cFilT\r\n" \
"HMDMfTuDAEDmGnwwRwYDVR0gBEAwPjA8BgRVHSAAMDQwMgYIKwYBBQUHAgEWJmh0\r\n" \
"dHBzOi8vd3d3Lmdsb2JhbHNpZ24uY29tL3JlcG9zaXRvcnkvMDMGA1UdHwQsMCow\r\n" \
"KKAmoCSGImh0dHA6Ly9jcmwuZ2xvYmFsc2lnbi5uZXQvcm9vdC5jcmwwPQYIKwYB\r\n" \
"BQUHAQEEMTAvMC0GCCsGAQUFBzABhiFodHRwOi8vb2NzcC5nbG9iYWxzaWduLmNv\r\n" \
"bS9yb290cjEwHwYDVR0jBBgwFoAUYHtmGkUNl8qJUC99BM00qP/8/UswDQYJKoZI\r\n" \
"hvcNAQELBQADggEBAEYq7l69rgFgNzERhnF0tkZJyBAW/i9iIxerH4f4gu3K3w4s\r\n" \
"32R1juUYcqeMOovJrKV3UPfvnqTgoI8UV6MqX+x+bRDmuo2wCId2Dkyy2VG7EQLy\r\n" \
"XN0cvfNVlg/UBsD84iOKJHDTu/B5GqdhcIOKrwbFINihY9Bsrk8y1658GEV1BSl3\r\n" \
"30JAZGSGvip2CTFvHST0mdCF/vIhCPnG9vHQWe3WVjwIKANnuvD58ZAWR65n5ryA\r\n" \
"SOlCdjSXVWkkDoPWoC209fN5ikkodBpBocLTJIg1MGCUF7ThBCIxPTsvFwayuJ2G\r\n" \
"K1pp74P1S8SqtCr4fKGxhZSM9AyHDPSsQPhZSZg=\r\n" \
"-----END CERTIFICATE-----\r\n";*/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* 异步socket消息回调(执行环境为eloop线程) */
static void __on_asocket_event(int sock, cm_asocket_event_e event, void *ev_param, void *user_param)
{
    switch (event)
    {
        /* TCP连接成功，之后才可进行SSL连接 */
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

        /* SSL握手阶段会上报读取事件，握手阶段不可进行数据读取 */
        case CM_ASOCKET_EV_RECV_IND: 
        {
            /* 正在握手，不可执行读取 */
            if (1 == is_shaking_hands)
            {
                cm_demo_printf("ssl shaking hands\n");
                return;
            }
            
            /* 接收数据 */
            uint8_t recv_buf[128] = {0};
            int ret = 0;

            /* 使用cm_ssl_get_bytes_avail()获取可读数据长度之前，需要先执行一次cm_ssl_read(test_ssl_ctx, NULL, 0); */
            cm_ssl_read(test_ssl_ctx, NULL, 0);
            cm_demo_printf("ssl get_bytes %d\n", cm_ssl_get_bytes_avail(test_ssl_ctx));

            /* 此为测试使用，正式使用时应尽量在其他线程中进行读取操作，回调函数中勿做复杂或阻塞业务 */
            while(1)
            {
                ret = cm_ssl_read(test_ssl_ctx, (void*)recv_buf, 128);
            
                if (ret > 0)
                {
                    cm_demo_printf("ssl read data=%s\n", recv_buf);
                }
                
                else
                {
                    cm_demo_printf("ssl read ret=%d\n", ret);
                    break;
                }

                if (cm_ssl_check_pending(test_ssl_ctx) == 0)//查询ssl缓存中是否有未读数据
                {
                    cm_demo_printf("ssl no pending data");
                    break;
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

static void __on_eloop_cmd_open_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 创建socket */
    int sock = -1;
    sock = cm_asocket_open(AF_INET, SOCK_STREAM, IPPROTO_TCP, __on_asocket_event, NULL);

    if (-1 == sock)
    {
        cm_demo_printf("open failed\n");
        return;
    }
    
    cm_demo_printf("sock(%d) open\n", sock);

    test_ssl_sock = sock;  //以一个变量保存创建的socket以便后续进行SSL连接

    /* 配置连接参数并执行连接 */
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(test_ssl_addr);  //自行补充服务器地址
    server_addr.sin_port = htons(test_ssl_port);  //自行补充端口号
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

static void __on_cmd_open(void)
{
    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_open_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_open_recv_event, NULL);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_open_recv_event);
}

static void __on_eloop_cmd_close_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 关闭socke */
    int ret = cm_asocket_close(test_ssl_sock);
    
    if (0 == ret)
    {
        cm_demo_printf("sock(%d) close success\n", test_ssl_sock);
    }
    
    else
    {
        cm_demo_printf("sock(%d) close error(%d)\n", test_ssl_sock, errno);
    }
}

/* SSL关闭连接 */
static void __on_cmd_ssl_close(void)
{
    /* 先关闭SSL再关闭TCP */
    cm_ssl_close((void **)&test_ssl_ctx);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_close_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_close_recv_event, NULL);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_close_recv_event);
}

/* SSL发送 */
static void __on_cmd_ssl_send(void)
{
    int send_len = 0;
    int ret = 0;

    char send_buffer[128] = "hello";
    int len = strlen(send_buffer);

    while(send_len < len)
    {
        ret = cm_ssl_write(test_ssl_ctx, (void*)(send_buffer + send_len), len - send_len);

        if(ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ)//MBEDTLS正在等待发送或等待接收
        {
            continue;
        }
        
        if(ret >= 0)//发送成功
        {
            send_len += ret;
        }
        
        else//发送异常
        {
            send_len = ret;
            break;
        }
    }

    cm_demo_printf("send len:%d\n", send_len);
}

/* SSL连接 */
static void __on_cmd_ssl_conn(void *param)
{
    int type = atoi(param);

    /* 事先需要设置SSL参数，先选择使用通道，再设置该通道的验证类型，验证所需的证书 */
    int ssl_id = 1;     //SSL通道
    uint16_t negotime = 60;  //握手超时(s)
    uint8_t session = 1;
    uint8_t sni = 0;
    uint8_t version = 255;
    int32_t cipher_suite = 0x0000;
    uint8_t ignorstamp  = 0;
    uint8_t ignorverify  = 0;
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CA_CERT_FILENAME, test_serve_ca_filename)); 
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CLI_CERT_FILENAME, test_client_ca_filename)); 
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CLI_KEY_FILENAME, test_client_key_filename)); 
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_NEGOTIME, (void *)(uintptr_t)negotime));
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_SESSION, (void *)(uintptr_t)session));
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_SNI, (void *)(uintptr_t)sni));
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERSION, (void *)(uintptr_t)version));
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_SUITES, (void *)(uintptr_t)cipher_suite));
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_IGNORE_STAMP, (void *)(uintptr_t)ignorstamp));
    cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_IGNORE_VERIFY, (void *)(uintptr_t)ignorverify));
    
    switch (type)
    {
        case 0:
        {
            int verify = 0;     //无证书验证
            cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERIFY, (void *)(uintptr_t)verify)); 
            break;
        }

        case 1:
        {
            int verify = 1;     //单向验证
            cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERIFY, (void *)(uintptr_t)verify)); 
            cm_demo_printf("ssl write_cert ret %d\n", cm_ssl_write_cert_file((uint8_t *)test_serve_ca_filename, strlen(test_serve_ca), 0, (uint8_t *)test_serve_ca, CM_SSL_FILE_TYPE_CERT));
            break;
        }

        case 2:
        {
            int verify = 2;     //双向验证
            cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERIFY, (void *)(uintptr_t)verify)); 
            cm_ssl_write_cert_file((uint8_t *)test_serve_ca_filename, strlen(test_serve_ca), 0, (uint8_t *)test_serve_ca, CM_SSL_FILE_TYPE_CERT);
            cm_ssl_write_cert_file((uint8_t *)test_client_ca_filename, strlen(test_client_ca), 0, (uint8_t *)test_client_ca, CM_SSL_FILE_TYPE_CERT);
            cm_ssl_write_cert_file((uint8_t *)test_client_key_filename, strlen(test_client_key), 0, (uint8_t *)test_client_key, CM_SSL_FILE_TYPE_KEY);

            uint8_t *data = NULL;
            uint16_t date_len = 0;
            int ret = cm_ssl_read_cert_file((uint8_t *)test_serve_ca_filename, &data, &date_len);
            if (ret)
            {
                cm_demo_printf("ssl read serve ca err %d\n", ret);
            }
            else
            {
                cm_demo_printf("ssl read serve ca %d %d %.*s\n", ret, date_len, 500, data);
            }
            
            date_len = 0;
            cm_free(data);
            data = NULL;
            break;
        }

        default:
        {
            cm_demo_printf("invalid param\n");
            return;
        }
    }

    /* 开始握手连接，握手阶段不可进行数据读取，使用标志位控制。cm_ssl_conn是同步接口，直接返回握手结果 */
    is_shaking_hands = 1;
    int ret = cm_ssl_conn((void **)&test_ssl_ctx, ssl_id, test_ssl_sock, 0);
    is_shaking_hands = 0;

    cm_demo_printf("ssl connect ret %d\n", ret);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* 执行测试命令 */
void cm_test_ssl(EmbeddedCli *cli, char *args, void *context)
{
    /* ssl [cmd]... */
    int i = 0;
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }

    /* ssl open */
    /* 进行握手之前，先用TCP连接平台 */
    if (strncmp(cmd, STR_ITEM("open")) == 0)
    {
        __on_cmd_open();
    }

    /* ssl conn [type] */
    /* ssl conn 0  无证书验证 */
    /* ssl conn 1  单向验证 */
    /* ssl conn 2  双向验证 */
    /* SSL握手，连接前先设置好认证方式和证书 */
    else if (strncmp(cmd, STR_ITEM("conn")) == 0)
    {
        const char *param = embeddedCliGetToken(args, 2);
        if (param == NULL)
        {
            cm_demo_printf("invalid param\n");
            return;
        }

        __on_cmd_ssl_conn((void *)param);
    }

    /* ssl close */
    /* 关闭连接，先关闭SSL连接再关闭TCP连接 */
    else if (strncmp(cmd, STR_ITEM("close")) == 0)
    {
        __on_cmd_ssl_close();
    }

    /* ssl send */
    /* SSL发送数据 */
    else if (strncmp(cmd, STR_ITEM("send")) == 0)
    {
        __on_cmd_ssl_send();
    }

    /* ssl cipher */
    /* 获取支持的加密套件列表 */
    else if (strncmp(cmd, STR_ITEM("cipher")) == 0)
    {
        int* list = cm_ssl_lis_cipher();
        int size = 0;
        
        while (list[size] != 0)
        {
            size++;
        }
        
        uint8_t* data = cm_malloc(size * 7 + 1);
        
        if (data)
        {
            memset(data, 0, size * 7 + 1);
            uint32_t len = 0;
            
            for (i = 0; i < size; i++)
            {
                len += snprintf((char *)data + len, (size * 7 + 1 - len), "0x%x,", list[i]);
            }
            
            cm_demo_printf("cipher list: %s\n", data);
            cm_free(data);
        }
    }
}
