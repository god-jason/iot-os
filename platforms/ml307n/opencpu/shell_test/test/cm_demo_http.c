/*********************************************************
 *  @file    cm_demo_http.c
 *  @brief   OpenCPU HTTP示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot3000 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_HTTP_SUPPORT
#include <stdio.h>
#include "cm_http.h"
#include "cm_ssl.h"

#define CM_HTTP_SYNC_ENABLE
#define HTTP_LOG(fmt, ...) osPrintf("[HTTP][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

typedef struct
{
    unsigned char **cmd;
    char len;
}http_demo_t;

//www.baidu.com根证书
static const char *http_ca = {"-----BEGIN CERTIFICATE-----\r\n" \
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
"-----END CERTIFICATE-----\r\n"
};

/*网址：https://onlinebooks.library.upenn.edu/banned-books-1994.html 证书*/
static const char *https_ca = \
{
        \
    "-----BEGIN CERTIFICATE-----\r\n" \
    "MIIG7DCCBVSgAwIBAgIQQzKpuGj3csxoDYrD/V5nKTANBgkqhkiG9w0BAQwFADBE\r\n" \
    "MQswCQYDVQQGEwJVUzESMBAGA1UEChMJSW50ZXJuZXQyMSEwHwYDVQQDExhJbkNv\r\n" \
    "bW1vbiBSU0EgU2VydmVyIENBIDIwHhcNMjQwNzA5MDAwMDAwWhcNMjUwNzA5MjM1\r\n" \
    "OTU5WjB7MQswCQYDVQQGEwJVUzEVMBMGA1UECBMMUGVubnN5bHZhbmlhMTcwNQYD\r\n" \
    "VQQKEy5UaGUgVHJ1c3RlZXMgb2YgdGhlIFVuaXZlcnNpdHkgb2YgUGVubnN5bHZh\r\n" \
    "bmlhMRwwGgYDVQQDDBMqLmxpYnJhcnkudXBlbm4uZWR1MIIBIjANBgkqhkiG9w0B\r\n" \
    "AQEFAAOCAQ8AMIIBCgKCAQEA4kAmnswP8NyYDpYH0uu6hJff2Dwj6Y5Sfuk2NsNK\r\n" \
    "Ly0jGFz5QNYy0CT17erhEdKBcedS5i8B2NoymccVPmqe5+tai5C4P8PnGLExYNp5\r\n" \
    "c1bebXYoCJ2QF5sCQ+50iMYey6/jP/6O9Pe54N4nuHhNOx1EFuLdMLCrQGGwg/8E\r\n" \
    "G4o8xaUcnsp/pOjHaYvnBiSoeEvGX8mUwNXAXAktBRvp0OpDJXdSHRI5JDLB7aMW\r\n" \
    "wmoBD/v2mSToZhJphGlKan27uQzklcz97pIgpB5MWJssOUNA5bmj4YFqMVRgjNyr\r\n" \
    "qiyxiKMxjbBLA2WC3yRIvLBzZVVCQy4s6aoylhcS0VUSFQIDAQABo4IDITCCAx0w\r\n" \
    "HwYDVR0jBBgwFoAU70wAkqb7di5eleLJX4cbGdVN4tkwHQYDVR0OBBYEFFUdm1bg\r\n" \
    "sl+gCCsFQofLv7FtZY4dMA4GA1UdDwEB/wQEAwIFoDAMBgNVHRMBAf8EAjAAMB0G\r\n" \
    "A1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjBJBgNVHSAEQjBAMDQGCysGAQQB\r\n" \
    "sjEBAgJnMCUwIwYIKwYBBQUHAgEWF2h0dHBzOi8vc2VjdGlnby5jb20vQ1BTMAgG\r\n" \
    "BmeBDAECAjBABgNVHR8EOTA3MDWgM6Axhi9odHRwOi8vY3JsLnNlY3RpZ28uY29t\r\n" \
    "L0luQ29tbW9uUlNBU2VydmVyQ0EyLmNybDBwBggrBgEFBQcBAQRkMGIwOwYIKwYB\r\n" \
    "BQUHMAKGL2h0dHA6Ly9jcnQuc2VjdGlnby5jb20vSW5Db21tb25SU0FTZXJ2ZXJD\r\n" \
    "QTIuY3J0MCMGCCsGAQUFBzABhhdodHRwOi8vb2NzcC5zZWN0aWdvLmNvbTAeBgNV\r\n" \
    "HREEFzAVghMqLmxpYnJhcnkudXBlbm4uZWR1MIIBfQYKKwYBBAHWeQIEAgSCAW0E\r\n" \
    "ggFpAWcAdgDd3Mo0ldfhFgXnlTL6x5/4PRxQ39sAOhQSdgosrLvIKgAAAZCYG/01\r\n" \
    "AAAEAwBHMEUCIQCIvYDAl46J1mouQSHF8y+0jap60FjqtyOLdfWj2RUMvAIgTdWa\r\n" \
    "mSq6cSXsfiqGxo+Kz9a71vYGnpt0enEPK/fs350AdgAN4fIwK9MNwUBiEgnqVS78\r\n" \
    "R3R8sdfpMO8OQh60fk6qNAAAAZCYG/z9AAAEAwBHMEUCID2dc7K7Y2gWMgxLGmaj\r\n" \
    "GwqecsRjUAnhJ5gAbDQ6Mv9yAiEAxwc/QZNoKvvaAQkmxQGKWHKGjzDP9iHJrm1p\r\n" \
    "rYdE7zgAdQAS8U40vVNyTIQGGcOPP3oT+Oe1YoeInG0wBYTr5YYmOgAAAZCYG/zX\r\n" \
    "AAAEAwBGMEQCIE4ABJUEVCsIhhToe/fgjtfvSNCFVENosy7T6jcHKVB4AiA1iCil\r\n" \
    "ihzgKX25Q81l7axbh6AOvgCzUUngDQRec+j03DANBgkqhkiG9w0BAQwFAAOCAYEA\r\n" \
    "Y6HoUfZw+KR/tIHixGQP0wk0QQRJ/+n3cJDMLnEkDV+GGVC5afQuTuz2apVdRgSK\r\n" \
    "5RvoZONJ5gHDtdIeMQoOptLQCuEy4+JDH7paPme/6FTZRdtp0aOdY1CQdxatMsK5\r\n" \
    "dlSl6LDXFzLlYDniCRhgX6qZnPIyg4dxxuwHSRavrU3xQJH1BMLzr4mRWpPdFgL2\r\n" \
    "zUspxZxqyQLBZO1AjhCMt+jaeRjIkU4+toYnqlpug10XXw27HsLcBAkVgn8NThP6\r\n" \
    "p0qLbiANpgr1kt7ddcCO3nWBTsrjkouRJc5BC7RipSAHWtocGMwP1877aThOJUYN\r\n" \
    "cTTHexhrexCDzh8GWZmlhK7hlWUpy+VmZWj/+uTU7+vYJU8k6OcbeKrX/Te8wAs/\r\n" \
    "YlyeyUG0bfUIr6W7NOPYGtl2jGik22dw4BjEFBkVaGYjl5y4+MGCL0Rfk1oyP9Gv\r\n" \
    "CxudSlyMVtiUInXVIPQqbMljUl1ckvWSDMSIOnzzENonFcoNCfXXFlB9lDbz3ua8\r\n" \
    "-----END CERTIFICATE-----\r\n"
};

/**HTTP实例句柄*/
static cm_httpclient_handle_t client = NULL;

static void __cm_httpclient_callback(cm_httpclient_handle_t client_handle, cm_httpclient_callback_event_e event, void *param)
{
    HTTP_LOG("%s():[%d], event [%d]\n", __func__, __LINE__, event);

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
            if(callback_param == NULL)
            {
                HTTP_LOG("callback_param == NULL\n");
                break;
            }

            if(200 != callback_param->response_code && callback_param->response_header != NULL)
            {
                HTTP_LOG("CM_HTTP_CALLBACK_EVENT_RSP_HEADER_IND response_code err: %d\n", callback_param->response_code);
                HTTP_LOG("%.*s\n", callback_param->response_header_len, callback_param->response_header);
                break;
            }

            HTTP_LOG("%s\n", callback_param->response_header);

            break;
        }
        case CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND:
        {
            cm_httpclient_callback_rsp_content_param_t *callback_param = (cm_httpclient_callback_rsp_content_param_t *)param;
            if(callback_param == NULL)
            {
                HTTP_LOG("callback_param == NULL\n");
                break;
            }

            HTTP_LOG("CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND [%d, %d, %d]\n", callback_param->total_len,callback_param->sum_len, callback_param->current_len);

            if(0 != callback_param->current_len && callback_param->response_content != NULL)
            {
                HTTP_LOG("%.*s\n", callback_param->current_len, callback_param->response_content);
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
            HTTP_LOG( "CM_HTTP_CALLBACK_EVENT_ERROR_IND http err: %d\n", callback_param);

            break;
        }
        default:
            break;
    }
}

/**
 *  HTTP功能调试使用示例，注意使用前需先设置TEST_HTTP_SERVER和TEST_HTTP_PATH宏控中的信息
 *
 *  CM:http ASYNCHHTTP          //异步HTTP测试
 *  CM:http ASYNCHHTTPS0        //异步HTTPS无身份认证
 *  CM:http ASYNCHHTTPS1        //异步HTTPS单向认证
 *  CM:http ASYNCHHTTPS2        //异步HTTPS双向认证
 *  CM:http RESPONSE            //查询HTTP响应结果码
 *  CM:http TERMINATE           //终止HTTP连接
 *
 */
void cm_test_http(unsigned char **cmd,int len)
{
    if(cmd == NULL || len <= 1)
    {
        HTTP_LOG("cm_test_http param error\n");
        return;
    }

    for(int i = 0; i < len; i++)
    {
        if(cmd[i] == NULL)
        {
            HTTP_LOG("cm_test_http param error\n");
            return;
        }
    }

    unsigned char operation[20] = {0};
    cm_httpclient_ret_code_e ret = CM_HTTP_RET_CODE_UNKNOWN_ERROR;
    snprintf((char *)operation, sizeof(operation), "%.*s", sizeof(operation), cmd[1]);
    HTTP_LOG("operation=%.*s\n", sizeof(operation), operation);

     int cert_verify = 0;
    if (0 == strcmp((const char *)operation, "ASYNCHHTTPS0"))
        cert_verify = 0;
    else if (0 == strcmp((const char *)operation, "ASYNCHHTTPS1"))
        cert_verify = 1;
    else if (0 == strcmp((const char *)operation, "ASYNCHHTTPS2"))
        cert_verify = 2;

    if (0 == strcmp((const char *)operation, "ASYNCHHTTP"))  //异步http测试
    {
        if (NULL == client)
        {
            ret = cm_httpclient_create((const uint8_t *)"http://www.baidu.com", __cm_httpclient_callback, &client);          //创建客户端实例

            if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
            {
                HTTP_LOG("cm_httpclient_create() error!\r\n");
                return;
            }

            cm_httpclient_cfg_t client_cfg={0};
//            #ifdef HTTPCLIENT_SSL_ENABLE
            client_cfg.ssl_enable = false;                                                  //使用SSL，即HTTPS连接方式。使用HTTP方式时该值为false
            client_cfg.ssl_id = 0;                                                          //设置SSL索引号
//            #endif
            client_cfg.cid = 0xFF;                                                             //设置PDP索引号，目前不支持该项设置，设置任意值即可
            client_cfg.conn_timeout = HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT;
            client_cfg.rsp_timeout = HTTPCLIENT_WAITRSP_TIMEOUT_DEFAULT;
//            client_cfg.dns_priority = 0;                                                    //设置DNS解析优先级

            ret = cm_httpclient_set_cfg(client, client_cfg);                                //客户端参数设置

        }
        ret = cm_httpclient_request_start(client, HTTPCLIENT_REQUEST_GET, (const uint8_t *)"http://www.baidu.com", false, 0);

        if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
        {
            HTTP_LOG("cm_httpclient_request_start() error! ret is %d\r\n", ret);
            return;
        }
    }
    else if ((0 == strcmp((const char *)operation, "ASYNCHHTTPS0")) || (0 == strcmp((const char *)operation, "ASYNCHHTTPS1"))
        || (0 == strcmp((const char *)operation, "ASYNCHHTTPS2")))//异步https示例
    {
        if (NULL == client)
        {
    //            ret = cm_httpclient_create((const uint8_t *)"https://onlinebooks.library.upenn.edu/banned-books-1994.html", __cm_httpclient_callback, &httpclient->client);          //创建客户端实例
            if(cert_verify == 0)  //无身份认证
                ret = cm_httpclient_create((const uint8_t *)"https://www.baidu.com", __cm_httpclient_callback, &client);          //创建客户端实�
            else if(cert_verify == 1)  //单向验证
                ret = cm_httpclient_create((const uint8_t *)"https://www.baidu.com", __cm_httpclient_callback, &client);          //创建客户端实�
            else if(cert_verify == 2)  //双向验证
                ret = cm_httpclient_create((const uint8_t *)"https://onlinebooks.library.upenn.edu/banned-books-1994.html", __cm_httpclient_callback, &client);          //创建客户端实�
            else
                ret = cm_httpclient_create((const uint8_t *)"https://onlinebooks.library.upenn.edu/banned-books-1994.html", __cm_httpclient_callback, &client);          //创建客户端实�


            if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
            {
                HTTP_LOG("cm_httpclient_create() error!\r\n");
                return;
            }

            cm_httpclient_cfg_t client_cfg={0};
    //      #ifdef HTTPCLIENT_SSL_ENABLE
            client_cfg.ssl_enable = true;                                                  //使用SSL，即HTTPS连接方式。使用HTTP方式时该值为false
            client_cfg.ssl_id = 1;                                                          //设置SSL索引号
    //      #endif

            client_cfg.cid = 0xFF;                                                             //设置PDP索引号，目前不支持该项设置，设置任意值即可
            client_cfg.conn_timeout = HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT;
            client_cfg.rsp_timeout = HTTPCLIENT_WAITRSP_TIMEOUT_DEFAULT;
    //      client_cfg.dns_priority = 0;                                                    //设置DNS解析优先级

            ret = cm_httpclient_set_cfg(client, client_cfg);                                //客户端参数设置

        }
        char tmp;
        if(cert_verify == 0)  //无身份认证
        {
            tmp = 0;
            cm_ssl_setopt(1 ,CM_SSL_PARAM_VERIFY, (void *)(uintptr_t)tmp);                                    //设置SSL验证方式
//                cm_ssl_setopt(1,CM_SSL_PARAM_CA_CERT, (char*)http_ca);                          //设置CA证书

        ret = cm_httpclient_request_start(client, HTTPCLIENT_REQUEST_GET, (const uint8_t *)"https://www.baidu.com", false, 0);
        }
        else if(cert_verify == 1)  //服务器认证-单向认证
        {
            tmp = 1;
            cm_ssl_setopt(1 ,CM_SSL_PARAM_VERIFY, (void *)(uintptr_t)tmp);                                    //设置SSL验证方式
            cm_ssl_setopt(1,CM_SSL_PARAM_CA_CERT_FILENAME, (char*)http_ca);                          //设置CA证书

            ret = cm_httpclient_request_start(client, HTTPCLIENT_REQUEST_GET, (const uint8_t *)"https://www.baidu.com", false, 0);
        }
        else if(cert_verify == 2)  //服务器和客户端认证-双向认证
        {
            tmp = 2;
            cm_ssl_setopt(1 ,CM_SSL_PARAM_VERIFY, (void *)(uintptr_t)tmp);                                    //设置SSL验证方式
            cm_ssl_setopt(1,CM_SSL_PARAM_CA_CERT_FILENAME, (char*)https_ca);                          //设置CA证书

            ret = cm_httpclient_request_start(client, HTTPCLIENT_REQUEST_GET, (const uint8_t *)"https://onlinebooks.library.upenn.edu/banned-books-1994.html", false, 0);
        }

//            ret = cm_httpclient_request_start(client, HTTPCLIENT_REQUEST_GET, (const uint8_t *)"https://onlinebooks.library.upenn.edu/banned-books-1994.html", false, 0);
        if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
        {
            HTTP_LOG("cm_httpclient_request_start() error! ret is %d\r\n", ret);
            return;
        }
    }
    else if (0 == strcmp((const char *)operation, "RESPONSE"))
    {
        if (NULL != client)
        {
            HTTP_LOG("client[%x] code is %d\r\n", client, cm_httpclient_get_response_code(client));
        }
    }
    else if (0 == strcmp((const char *)operation, "TERMINATE"))
    {
        if (NULL != client)
        {
            cm_httpclient_terminate(client);
            if (CM_HTTP_RET_CODE_OK == cm_httpclient_delete(client))
            {
                HTTP_LOG("client[%x] delete\r\n", client);
                client = NULL;
            }
            else
            {
                HTTP_LOG("client[%x] delete error\r\n", client);
            }
        }
        else
        {
            HTTP_LOG("client in non-existent\r\n");
        }
    }
    else
    {
        HTTP_LOG("illegal operation\n");
    }
    return;
}


void cm_http_demo(void *param)
{
    if(param == NULL)
    {
        HTTP_LOG("illegal param\n");
        return;
    }

    http_demo_t *http_demo = (http_demo_t*)param;
    cm_test_http(http_demo->cmd, http_demo->len);
}

#if defined(OS_USING_SHELL)
#include <nr_micro_shell.h>

static void SHELL_http(char argc, char **argv)
{
    if(argv == NULL || argc < 2 || argv[0] == NULL || argv[1] == NULL)
    {
        shell_printf("shell param error\r\n");
        return;
    }

    if(argc == 2) /* help */
    {
        if (0 == strcmp(argv[1], "help"))
        {
            shell_printf("-----------------http help---------------------\r\n");
            shell_printf("1 http ASYNCHHTTP\r\n");
            shell_printf("2 http ASYNCHHTTPS0\r\n");
            shell_printf("3 http ASYNCHHTTPS1\r\n");
            shell_printf("4 http ASYNCHHTTPS2\r\n");
            shell_printf("5 http RESPONSE\r\n");
            shell_printf("6 http TERMINATE\r\n");
            shell_printf("-----------------http--------------------------\r\n");

        }
        else
        {
            shell_printf("shell http argv %s %s argc: %d\r\n", argv[0], argv[1], argc);
            osThreadAttr_t attr = {0};
            attr.name = "http_demo";
            attr.stack_size = 4096;
            attr.priority = osPriorityNormal;

            http_demo_t http_demo = {0};
            http_demo.cmd = argv;
            http_demo.len = argc;

            osThreadNew(cm_http_demo, (void *)(&http_demo), &attr);
            osDelay(200);
        }
    }
    else
        shell_printf("http parameter invalid, http help\r\n");
}

NR_SHELL_CMD_EXPORT(http, SHELL_http);

#endif

#endif


