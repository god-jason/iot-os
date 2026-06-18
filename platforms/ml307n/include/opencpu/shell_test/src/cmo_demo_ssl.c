/*********************************************************
 *  @file    cm_demo_ssl.c
 *  @brief   OpenCPU ssl示例
 *  Copyright (c) 2023 China Mobile IOT.
 *  All rights reserved.
 *  created By SMR 2023/6/29
 ********************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#if defined(CM_DEMO_SSL_SUPPORT)

#include "lwip/sockets.h"
#include "lwip/arch.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cm_asocket.h"
#include "cm_mem.h"
#include "cm_ssl.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/timing.h"
#include "mbedtls/debug.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"

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
static char *test_ssl_addr = "183.230.40.96";     //测试地址, 例: "192.168.0.1"
static int test_ssl_port = 443;       //测试端口

static cm_ssl_ctx_t *test_ssl_ctx = NULL;          //ssl 上下文
static int test_ssl_sock = -1;             //ssl sock
static int is_shaking_hands = 0;           //ssl握手标志，握手阶段不能进行数据读取

/* ca证书 */
//static char *test_serve_ca = "";

/* 客户端证书 */
static char *test_client_ca = "";

/* 客户端密钥 */
static char *test_client_key = "";

static const char *test_serve_ca = \
{
        \
    "-----BEGIN CERTIFICATE-----\r\n"
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
    "-----END CERTIFICATE-----"
};

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
static void __on_asocket_event(int sock, cm_asocket_event_e event, void *user_param)
{
    switch (event)
    {
        /* TCP连接成功，之后才可进行SSL连接 */
        case CM_ASOCKET_EV_CONNECT_OK: 
        {
            osPrintf("sock(%d) connect_ok\n", sock);
            break;
        }
        
        case CM_ASOCKET_EV_CONNECT_FAIL: 
        {
            osPrintf("sock(%d) connect_fail\n", sock);
            break;
        }

        /* SSL握手阶段会上报读取事件，握手阶段不可进行数据读取 */
        case CM_ASOCKET_EV_RECV_IND: 
        {
            /* 正在握手，不可执行读取 */
            if (1 == is_shaking_hands)
            {
                osPrintf("ssl shaking hands\n");
                return;
            }
            
            /* 接收数据 */
            uint8_t recv_buf[128] = {0};
            int ret = 0;

            /* 使用cm_ssl_get_bytes_avail()获取可读数据长度之前，需要先执行一次cm_ssl_read(test_ssl_ctx, NULL, 0); */
            cm_ssl_read(test_ssl_ctx, NULL, 0);
            osPrintf("ssl get_bytes %d\n", cm_ssl_get_bytes_avail(test_ssl_ctx));

            /* 此为测试使用，正式使用时应尽量在其他线程中进行读取操作，回调函数中勿做复杂或阻塞业务 */
            while(1)
            {
                ret = cm_ssl_read(test_ssl_ctx, (void*)recv_buf, 128);
            
                if (ret > 0)
                {
                    osPrintf("ssl read data=%s\n", recv_buf);
                }
                
                else
                {
                    osPrintf("ssl read ret=%d\n", ret);
                    break;
                }

                if (cm_ssl_check_pending(test_ssl_ctx) == 0)//查询ssl缓存中是否有未读数据
                {
                    osPrintf("ssl no pending data");
                    break;
                }
            }
            
            break;
        }

        case CM_ASOCKET_EV_SEND_IND:
        {
            osPrintf("sock(%d) send_ind\n", sock);
            break;
        }

        case CM_ASOCKET_EV_ACCEPT_IND:
        {
            osPrintf("sock(%d) accept_ind\n", sock);
            break;
        }

        case CM_ASOCKET_EV_ERROR_IND: 
        {
            /* 获取socket错误码 */
            int sock_error = 0;
            socklen_t opt_len = sizeof(sock_error);
            cm_asocket_getsockopt(sock, SOL_SOCKET, SO_ERROR, &sock_error, &opt_len);
            osPrintf("sock(%d) error_ind: sock_error(%d)\n", sock, sock_error);
            
            if (103 == sock_error)
            {
                /* Connection aborted */
                osPrintf("sock(%d) error_ind: Connection aborted\n", sock);
            }
            
            else if (ECONNRESET == sock_error)
            {
                /* Connection reset */
                osPrintf("sock(%d) error_ind: Connection reset\n", sock);
            }
            
            else if (107 == sock_error)
            {
                /* Connection closed */
                osPrintf("sock(%d) error_ind: Connection closed\n", sock);
            }
            
            break;
        }
        
        default:
        {
            break;
        }
    }
}

static void __on_eloop_cmd_OPEN_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 创建socket */
    int sock = -1;
    sock = cm_asocket_open(AF_INET, SOCK_STREAM, IPPROTO_TCP, __on_asocket_event, NULL);

    if (-1 == sock)
    {
        osPrintf("open failed\n");
        return;
    }
    
    osPrintf("sock(%d) open\n", sock);

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
            osPrintf("sock(%d) open request success, wait connect...\n", sock);
        }
        
        else
        {
            osPrintf("sock(%d) connect failed(%d)\n", sock, errno);
        }
    }
    
    else if (0 == ret)
    {
        osPrintf("sock(%d) connect_ok\n", sock);
    }
}

static void __on_cmd_OPEN(void)
{
    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_OPEN_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_OPEN_recv_event, NULL);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_OPEN_recv_event);
}

static void __on_eloop_cmd_CLOSE_recv_event(cm_eloop_event_handle_t event, void *cb_param)
{
    /* 注销Event */
    cm_eloop_unregister_event(event);

    /* 关闭socke */
    int ret = cm_asocket_close(test_ssl_sock);
    
    if (0 == ret)
    {
        osPrintf("sock(%d) close success\n", test_ssl_sock);
    }
    
    else
    {
        osPrintf("sock(%d) close error(%d)\n", test_ssl_sock, errno);
    }
}

/* SSL关闭连接 */
static void __on_cmd_SSL_CLOSE(void)
{
    /* 先关闭SSL再关闭TCP */
    cm_ssl_close((void **)&test_ssl_ctx);

    /* 注册Event(将命令封装成Event发送到eloop执行) */
    cm_eloop_event_handle_t cmd_CLOSE_recv_event = cm_eloop_register_event(cm_asocket_eloop(), __on_eloop_cmd_CLOSE_recv_event, NULL);
    /* 发送到eloop执行 */
    cm_eloop_post_event(cmd_CLOSE_recv_event);
}

/* SSL发送 */
static void __on_cmd_SSL_SEND(void)
{
    int send_len = 0;
    int ret = 0;

    char send_buffer[128] = "GET / HTTP/1.1\r\nHost: open.iot.10086.cn\r\n\r\n";
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

    osPrintf("send len:%d\n", send_len);
}

/* SSL连接 */
static void __on_cmd_SSL_CONN(void *param)
{
    int type = atoi(((char **)param)[0]);

    /* 事先需要设置SSL参数，先选择使用通道，再设置该通道的验证类型，验证所需的证书 */
    int ssl_id = 1;     //SSL通道
    uint16_t negotime = 60;  //握手超时(s)
    uint8_t session = 1;
    uint8_t sni = 0;
    uint8_t version = 255;
    int cipher_suite = 0x0000;
    uint8_t ignorstamp  = 0;
    uint8_t ignorverify  = 1;
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CA_CERT, test_serve_ca); 
    //cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CLI_CERT, test_client_ca); 
    //cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CLI_KEY, test_client_key); 
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_NEGOTIME, &negotime);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_SESSION, &session);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_SNI, &sni);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERSION, &version);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CIPHER_SUITE, &cipher_suite);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_IGNORESTAMP, &ignorstamp);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_IGNOREVERIFY, &ignorverify);
    switch (type)
    {
        case 0:
        {
            char verify = 0;     //无证书验证
            cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERIFY, &verify); 
            break;
        }

        case 1:
        {
            char verify = 1;     //单向验证
            cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERIFY, &verify); 
            break;
        }

        case 2:
        {
            char verify = 2;     //双向验证
            cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERIFY, &verify); 
            break;
        }

        default:
        {
            osPrintf("invalid param\n");
            return;
        }
    }

    /* 开始握手连接，握手阶段不可进行数据读取，使用标志位控制。cm_ssl_conn是同步接口，直接返回握手结果 */
    is_shaking_hands = 1;
    int ret = cm_ssl_conn((void **)&test_ssl_ctx, ssl_id, test_ssl_sock, 0);
    is_shaking_hands = 0;

    osPrintf("ssl connect ret %d\n", ret);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* 执行测试命令 */
void cm_test_ssl(char len, char** cmd)
{
    osPrintf("cm_test_ssl len=%d\n", len);

    /* CM:SSL:[CMD]... */
    if (len < 3)
    {
        osPrintf("invalid param\n");
        return;
    }

    osPrintf("cmd[0]=%s,cmd[1]=%s,cmd[2]=%s\n", cmd[0], cmd[1], cmd[2]);

    const char *sub_cmd = (const char *)cmd[2];

    /* CM:SSL:OPEN */
    /* 进行握手之前，先用TCP连接平台 */
    if (strncmp(sub_cmd, STR_ITEM("OPEN")) == 0)
    {
        __on_cmd_OPEN();
    }

    /* CM:SSL:CONN:[type] */
    /* CM:SSL:CONN:0  无证书验证 */
    /* CM:SSL:CONN:1  单向验证 */
    /* CM:SSL:CONN:2  双向验证 */
    /* SSL握手，连接前先设置好认证方式和证书 */
    else if (strncmp(sub_cmd, STR_ITEM("CONN")) == 0)
    {
        if (len < 4)
        {
            osPrintf("invalid param\n");
            return;
        }

        __on_cmd_SSL_CONN(cmd + 3);
    }

    /* CM:SSL:CLOSE */
    /* 关闭连接，先关闭SSL连接再关闭TCP连接 */
    else if (strncmp(sub_cmd, STR_ITEM("CLOSE")) == 0)
    {
        __on_cmd_SSL_CLOSE();
    }

    /* CM:SSL:SEND */
    /* SSL发送数据 */
    else if (strncmp(sub_cmd, STR_ITEM("SEND")) == 0)
    {
        __on_cmd_SSL_SEND();
    }

    /* CM:SSL:CIPHER */
    /* 获取支持的加密套件列表 */
    else if (strncmp(sub_cmd, STR_ITEM("CIPHER")) == 0)
    {
        int* list = cm_ssl_list_cipher();
        int size = 0;
        
        while (list[size] != 0)
        {
            size++;
        }
        
        uint8_t* data = cm_malloc(size * 7 + 1);
        
        if (data)
        {
            uint32_t len = 0;

            osPrintf("cipher list: ");
            uint32_t list_offset = 0;
            uint32_t remain_size = size;
            
            /* 防止打印溢出 */
            while (remain_size)
            {
                memset(data, 0, size * 7 + 1);
                len = 0;
                
                if (remain_size > 83)
                {
                
                    for (int i = 0; i < 83; i++)
                    {
                        len += snprintf((char *)data + len, (size * 7 + 1 - len), "0x%x,", list[list_offset + i]);
                    }
                    
                    list_offset += 83;
                    remain_size -= 83;
                    osPrintf("%s", data);
                }
                else
                {
                
                    for (int i = 0; i < remain_size; i++)
                    {
                        len += snprintf((char *)data + len, (size * 7 + 1 - len), "0x%x,", list[list_offset + i]);
                    }
                    
                    remain_size = 0;
                    osPrintf("%s", data);
                }
                
            }
            
            osPrintf("\n");
            cm_free(data);
        }
    }
}

#include "nr_micro_shell.h"
NR_SHELL_CMD_EXPORT(cm_test_ssl_sample, cm_test_ssl);
#endif

