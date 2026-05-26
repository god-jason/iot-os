/*********************************************************
 *  @file    cm_demo_http.c
 *  @brief   OpenCPU HTTP示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot3000 2022/7/14
 ********************************************************/

#include "stdio.h"
#include "cm_http.h"
#include "cm_ssl.h"
#include "cm_demo_uart.h"
#include "cm_demo_http.h"


//www.baidu.com根证书
static const char *http_ca = "-----BEGIN CERTIFICATE-----\r\n" \
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
"-----END CERTIFICATE-----\r\n";

static cm_httpclient_handle_t client = NULL;

static void __cm_httpclient_callback(cm_httpclient_handle_t client_handle, cm_httpclient_callback_event_e event, void *param)
{
    cm_demo_printf("\n%s():[%d], event [%d]\n", __func__, __LINE__, event);

    switch(event)
    {
        case CM_HTTP_CALLBACK_EVENT_REQ_START_SUCC_IND:
        {
#if 0
            int32_t ret = cm_httpclient_request_send(client_handle, (const uint8_t *)"puttest", strlen("puttest"));
            cm_demo_printf("\ncm_httpclient_request_send() ret is %d\n", ret);
#endif
            break;
        }
        case CM_HTTP_CALLBACK_EVENT_RSP_HEADER_IND:
        {
            cm_httpclient_callback_rsp_header_param_t *callback_param = (cm_httpclient_callback_rsp_header_param_t *)param;

            if(200 != callback_param->response_code)
            {
                cm_demo_printf("\nCM_HTTP_CALLBACK_EVENT_RSP_HEADER_IND response_code err: %d\n", callback_param->response_code);
                cm_demo_printf("\n%s\n", callback_param->response_header);
                break;
            }

            cm_demo_printf("\n%s\n", callback_param->response_header);

            break;
        }
        case CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND:
        {
            cm_httpclient_callback_rsp_content_param_t *callback_param = (cm_httpclient_callback_rsp_content_param_t *)param;

            cm_demo_printf("\nCM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND [%d, %d, %d]\n", callback_param->total_len,callback_param->sum_len, callback_param->current_len);

            if (0 != callback_param->current_len)
            {
                cm_demo_printf("\n%s\n", callback_param->response_content);
            }

            break;
        }
        case CM_HTTP_CALLBACK_EVENT_RSP_END_IND:
        {
            break;
        }
        case CM_HTTP_CALLBACK_EVENT_ERROR_IND:
        {
            cm_httpclient_error_event_e callback_param = (cm_httpclient_error_event_e)(int)param;
            cm_demo_printf( "\nCM_HTTP_CALLBACK_EVENT_ERROR_IND http err: %d\n", callback_param);

            break;
        }
        default:
            break;
    }
}

/**
 *  HTTP功能调试使用示例，注意使用前需先设置TEST_HTTP_SERVER和TEST_HTTP_PATH宏控中的信息 
 *
 *  http SYNCHHTTP           //同步HTTP测试
 *  http ASYNCHHTTP          //异步HTTP测试
 *  http RESPONSE            //查询HTTP响应结果码
 *  http TERMINATE           //终止HTTP连接
 *
 */
void cm_test_http(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1);
    cm_httpclient_ret_code_e ret = CM_HTTP_RET_CODE_UNKNOWN_ERROR;

    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }

    if (0 == strcasecmp((const char *)cmd, "SYNCHHTTP"))
    {
        if (NULL == client)
        {
            /*.服务器地址url需要填写完整,例如（服务器url仅为格式示例）"http://www.baidu.com","http://[2408:400e:16b:9700:d3df:3f8b:7c2b:8888]:80".*/
            ret = cm_httpclient_create((const uint8_t *)"https://www.baidu.com", NULL, &client);          //创建客户端实例

            if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
            {
                cm_demo_printf("cm_httpclient_create() error!\r\n");
                return;
            }

            cm_httpclient_cfg_t client_cfg;
            client_cfg.ssl_enable = true;                                                   //使用SSL，即HTTPS连接方式。使用HTTP方式时该值为false
            client_cfg.ssl_id = 2;                                                          //设置SSL索引号
            client_cfg.cid = 0xFF;                                                             //设置PDP索引号，目前不支持该项设置，设置任意值即可
            client_cfg.conn_timeout = HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT;
            client_cfg.rsp_timeout = HTTPCLIENT_WAITRSP_TIMEOUT_DEFAULT;
            client_cfg.dns_priority = 1;                                                    //设置DNS解析优先级，ipv6解析优先
            ret = cm_httpclient_set_cfg(client, client_cfg);                                //客户端参数设置

            if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
            {
                cm_demo_printf("cm_httpclient_set_cfg() error!\r\n");
                return;
            }

            int tmp = 1;
            cm_ssl_setopt(2 ,CM_SSL_PARAM_VERIFY, (void *)(uintptr_t)tmp);                                    //设置SSL验证方式
            cm_demo_printf("ssl set ret %d\n", cm_ssl_setopt(client_cfg.ssl_id, CM_SSL_PARAM_CA_CERT_FILENAME, "http_serve_ca"));
            cm_ssl_write_cert_file((uint8_t *)"http_serve_ca", strlen((char *)http_ca), 0, (uint8_t *)http_ca, CM_SSL_FILE_TYPE_CERT);
        }

        cm_httpclient_sync_response_t response = {};
        cm_httpclient_sync_param_t param = {HTTPCLIENT_REQUEST_GET, (const uint8_t *)"/", 0, NULL};          //GET方法，必须设置请求路径

        ret = cm_httpclient_sync_request(client, param, &response);                         //发送请求，同步接口

        if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
        {
            cm_demo_printf("cm_httpclient_sync_request() error! ret is %d\r\n", ret);
            return;
        }
        else
        {
            cm_demo_printf("response_code is %d\r\n", response.response_code);
            cm_demo_printf("response_header_len is %d\r\n", response.response_header_len);
            cm_demo_printf("response_content_len is %d\r\n", response.response_content_len);

        }
        
        cm_httpclient_sync_free_data(client);                                               //释放响应数据
    }
    else if (0 == strcasecmp((const char *)cmd, "ASYNCHHTTP"))
    {
        if (NULL == client)
        {
            ret = cm_httpclient_create((const uint8_t *)"http://aip.baidubce.com", __cm_httpclient_callback, &client);          //创建客户端实例

            if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
            {
                cm_demo_printf("cm_httpclient_create() error!\r\n");
                return;
            }

            cm_httpclient_cfg_t client_cfg;
            client_cfg.ssl_enable = false;                                                  //使用SSL，即HTTPS连接方式。使用HTTP方式时该值为false
            client_cfg.ssl_id = 0;                                                          //设置SSL索引号
            client_cfg.cid = 0xFF;                                                             //设置PDP索引号，目前不支持该项设置，设置任意值即可
            client_cfg.conn_timeout = HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT;
            client_cfg.rsp_timeout = HTTPCLIENT_WAITRSP_TIMEOUT_DEFAULT;
            client_cfg.dns_priority = 0;                                                    //设置DNS解析优先级

            ret = cm_httpclient_set_cfg(client, client_cfg);                                //客户端参数设置

            char token_url[1024] = {0};

            sprintf(token_url, "/oauth/2.0/token?grant_type=client_credentials");
            sprintf(token_url + strlen((char*)token_url), "&client_id=%s", "v6pdyZF12345r0TqBwv5bjcO");                 //该数据为错误数据，仅用于demo示例，无实际效果
            sprintf(token_url + strlen((char*)token_url), "&client_secret=%s", "FKW3r54321n0uICxbuTy4ClTqNuuzXsy");     //该数据为错误数据，仅用于demo示例，无实际效果

            ret = cm_httpclient_request_start(client, HTTPCLIENT_REQUEST_GET, (uint8_t*)token_url, false, 0);

            if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
            {
                cm_demo_printf("cm_httpclient_request_start() error! ret is %d\r\n", ret);
                return;
            }
        }
    }
    else if (0 == strcasecmp((const char *)cmd, "RESPONSE"))
    {
        if (NULL != client)
        {
            cm_demo_printf("client[%x] code is %d\r\n", client, cm_httpclient_get_response_code(client));
        }
    }
    else if (0 == strcasecmp((const char *)cmd, "TERMINATE"))
    {
        if (NULL != client)
        {
            cm_httpclient_terminate(client);

            if (CM_HTTP_RET_CODE_OK == cm_httpclient_delete(client))
            {
                cm_demo_printf("client[%x] delete\r\n", client);
                client = NULL;
            }
            else
            {
                cm_demo_printf("client[%x] delete error\r\n", client);
            }
        }
        else
        {
            cm_demo_printf("client in non-existent\r\n");
        }
    }
    else
    {
        cm_demo_printf("illegal operation\n"); 
    }
    return;
}
