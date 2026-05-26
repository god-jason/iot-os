/*********************************************************
 *  @file    at_test_http.c
 *  @brief   OpenCPU at HTTP示例
 *  Copyright (c) 2025 China Mobile IOT.
 *  All rights reserved.
 *  created By ZY 2025/1/26
 ********************************************************/

#ifdef CM_DEMO_HTTP_SUPPORT

#include <os.h>
#include <stdlib.h>

// #include "cm_common_inc.h"
// #include "cm_common_api.h"
#include "at_api.h"
#include "at_parser.h"
#include "cm_http.h"
#include "cm_mem.h"

typedef AT_CommandItem at_cmd_t;
#define HTTP_LOG(fmt, ...) osPrintf("[HTTP][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

//#define HTTP_CMD_STR_LEN 512
#define HTTP_URL_LEN_MAX 1024
#define CM_AT_PARA_STRING_MAX_LENGTH 4096

#define CM_HTTP_RET_CODE_OK 0 //成功
#define CM_HTTP_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_HTTP_RET_CODE_PARAM_ERROR 50 //参数错误 50
#define CM_HTTP_RET_CODE_UNKNOWN_ERROR 650 //未知错误 650
#define CM_HTTP_RET_CODE_CLIENT_NOT_CREATE 652 //客户端未创建 652
#define CM_HTTP_RET_CODE_CLIENT_IS_BUSY 653 //客户端忙 653

void http_oc_test_entry(os_uint8_t id, os_uint8_t *at_buf, os_uint16_t lenOfPara);
int func_cm_httpclient_uri_encode(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_uri_encode_component(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_create(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_delete(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_is_busy(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_set_cfg(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_terminate(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_custom_header_set(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_custom_header_free(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_specific_header_set(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_specific_header_free(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_request_start(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_request_send(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_request_end(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_get_response_code(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_parse_header(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_sync_request(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_sync_request(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_sync_free_data(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_httpclient_request_content_set(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);




AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_http_oc[] =
{
    {.name = "+HTTP",   .setFunc = http_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

static cm_httpclient_handle_t client = NULL;
char *custom_header = NULL;
char *specific_header = NULL;
char *content = NULL;



static void __cm_httpclient_callback(cm_httpclient_handle_t client_handle, cm_httpclient_callback_event_e event, void *param)
{
    HTTP_LOG("\n event [%d]\n", event);

    switch(event)
    {
        case CM_HTTP_CALLBACK_EVENT_REQ_START_SUCC_IND:
        {
            if (NULL != content)
            {
                int32_t ret = cm_httpclient_request_send(client_handle, (const uint8_t *)content, strlen(content));
                HTTP_LOG("\ncm_httpclient_request_send() ret is %d\n", ret);
                cm_free(content);
            }

            break;
        }
        case CM_HTTP_CALLBACK_EVENT_RSP_HEADER_IND:
        {
            uint32_t rsp_string_len = 0;
            uint8_t rsp_string[64]={0};
            cm_httpclient_callback_rsp_header_param_t *callback_param = (cm_httpclient_callback_rsp_header_param_t *)param;
            if (callback_param->response_header_len > 0
                && callback_param->response_header) 
            {
                rsp_string_len = sprintf((char *)rsp_string, "+MHTTPURC: \"header\",0,%d,",
                    //(int)httpclient->httpclient_id,
                    (int)callback_param->response_code);
                rsp_string_len += sprintf((char *)(rsp_string+rsp_string_len), "%d,",callback_param->response_header_len);
                cm_uart_printf_urc_data(rsp_string, strlen((char *)rsp_string), callback_param->response_header, callback_param->response_header_len);
            }

            break;
        }
        case CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND:
        {
            cm_httpclient_callback_rsp_content_param_t *callback_param = (cm_httpclient_callback_rsp_content_param_t *)param;

            if(callback_param->current_len == 0)
            {
                cm_uart_printf_urc("+MHTTPURC: \"content\",0,%d,%d,%d",
                        //clientid,
                        callback_param->total_len,
                        callback_param->sum_len,                        //已接收
                        callback_param->current_len);                   //本包长度
                return;
            }
            else
            {
                uint32_t rsp_string_len = 0;
                uint8_t rsp_string[64]={0};
                rsp_string_len = sprintf((char *)rsp_string, "+MHTTPURC: \"content\",0,%d,",
                    //(int)clientid,
                    (int)callback_param->total_len);
                rsp_string_len += sprintf((char *)(rsp_string+rsp_string_len), "%d,%d,", (int)callback_param->sum_len, (int)callback_param->current_len);
                cm_uart_printf_urc_data(rsp_string, strlen((char *)rsp_string), callback_param->response_content, callback_param->current_len);
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
            HTTP_LOG( "\nCM_HTTP_CALLBACK_EVENT_ERROR_IND http err: %d\n", callback_param);
            cm_uart_printf_urc("+MHTTPURC: \"err\",,%d", /*httpclient->httpclient_id,*/ callback_param);
            break;
        }
        default:
            break;
    }
}



// AT+HTTP="XX", param1, param2, ...
// "XX" 为函数名
void http_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    cm_uart_print_set_id(id);

    HTTP_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if (para_count < 1 || para_count > 7)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_httpclient_uri_encode") == 0)
        {
            ret = func_cm_httpclient_uri_encode(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_uri_encode_component") == 0)
        {
            ret = func_cm_httpclient_uri_encode_component(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_create") == 0)
        {
            ret = func_cm_httpclient_create(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_delete") == 0)
        {
            ret = func_cm_httpclient_delete(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_is_busy") == 0)
        {
            ret = func_cm_httpclient_is_busy(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_set_cfg") == 0)
        {
            ret = func_cm_httpclient_set_cfg(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_terminate") == 0)
        {
            ret = func_cm_httpclient_terminate(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_custom_header_set") == 0)
        {
            ret = func_cm_httpclient_custom_header_set(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_custom_header_free") == 0)
        {
            ret = func_cm_httpclient_custom_header_free(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_specific_header_set") == 0)
        {
            ret = func_cm_httpclient_specific_header_set(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_specific_header_free") == 0)
        {
            ret = func_cm_httpclient_specific_header_free(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        
        else if (strcmp(func_name, "cm_httpclient_request_start") == 0)
        {
            ret = func_cm_httpclient_request_start(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_request_content_set") == 0)
        {
            ret = func_cm_httpclient_request_content_set(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        #if 0
        else if (strcmp(func_name, "cm_httpclient_request_send") == 0)
        {
            ret = func_cm_httpclient_request_send(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        #endif
        else if (strcmp(func_name, "cm_httpclient_request_end") == 0)
        {
            ret = func_cm_httpclient_request_end(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_get_response_code") == 0)
        {
            ret = func_cm_httpclient_get_response_code(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_parse_header") == 0)
        {
            ret = func_cm_httpclient_parse_header(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_sync_request") == 0)
        {
            ret = func_cm_httpclient_sync_request(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_httpclient_sync_free_data") == 0)
        {
            ret = func_cm_httpclient_sync_free_data(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            HTTP_LOG("Error func name[%s]", func_name);
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}

// AT+HTTP="cm_httpclient_uri_encode","url"
int func_cm_httpclient_uri_encode(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int iHttpId = -1;
    int ret = 0;
    char* url = NULL;
    uint8_t *encode_data = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;
    HTTP_LOG("func_cm_httpclient_uri_encode start.");

   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 2)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        
        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;  
        url = cm_malloc(HTTP_URL_LEN_MAX + 1);
        if(!url)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u\r", url, HTTP_URL_LEN_MAX+1))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        if(strlen(url) < 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        HTTP_LOG("%s(): url : %s\n", __func__, url);
        encode_data = cm_httpclient_uri_encode((const uint8_t *)url, (const uint32_t)strlen(url));
        if (NULL != encode_data)
        {
            cm_uart_printf_urc("+HTTPURIENCODE: %s", encode_data);
        }
        else
        {
            ret = CM_HTTP_RET_CODE_UNKNOWN_ERROR; 
        }
    }while (0);

    if(url) cm_free(url);
    if(encode_data) cm_free(encode_data);

    return ret;
}

// AT+HTTP="cm_httpclient_uri_encode_component","url"
int func_cm_httpclient_uri_encode_component(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int iHttpId = -1;
    int ret = 0;
    char* url = NULL;
    uint8_t *encode_data = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;

   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 2)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;  
        url = cm_malloc(HTTP_URL_LEN_MAX + 1);
        if(!url)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u\r", url, HTTP_URL_LEN_MAX+1))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        if(strlen(url) < 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        HTTP_LOG("%s(): url : %s\n", __func__, url);
        encode_data = cm_httpclient_uri_encode_component((const uint8_t *)url, (const uint32_t)strlen(url));
        if (NULL != encode_data)
        {
            cm_uart_printf_urc("+HTTPURIENCODECOMP: %s", encode_data);
        }
        else
        {
            ret = CM_HTTP_RET_CODE_UNKNOWN_ERROR; 
        }
    }while (0);

    if(url) cm_free(url);
    if(encode_data) cm_free(encode_data);

    return ret;
}

// AT+HTTP="cm_cm_httpclient_create","url"
int func_cm_httpclient_create(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    char* url = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 2)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;  
        url = cm_malloc(HTTP_URL_LEN_MAX + 1);
        if(!url)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u\r", url, HTTP_URL_LEN_MAX+1))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        if(strlen(url) < 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        HTTP_LOG("%s(): url : %s\n", __func__, url);
        ret = cm_httpclient_create((const uint8_t *)url, __cm_httpclient_callback, &client);          //创建客户端实例
        if (0 == ret && NULL == client)
        {
            ret = CM_HTTP_RET_CODE_UNKNOWN_ERROR;
        } 
    }while (0);

    if(url) cm_free(url);

    return ret;
}

// AT+HTTP="cm_httpclient_delete"
int func_cm_httpclient_delete(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        ret = cm_httpclient_delete(client);
        //client = NULL;
    }while (0);

    return ret;
}

// AT+HTTP="cm_httpclient_is_busy"
int func_cm_httpclient_is_busy(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = CM_HTTP_RET_CODE_OK;
    char* url = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        cm_uart_printf_urc("+HTTPISBUSY: %d", cm_httpclient_is_busy(client));

    }while (0);

    return ret;
}


// AT+HTTP="cm_httpclient_set_cfg",ssl_enable,ssl_id,cid,conn_timeout,rsp_timeout
int func_cm_httpclient_set_cfg(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    cm_httpclient_cfg_t cfg = {0};

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count < 1 || para_count > 6)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;  
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &cfg.ssl_enable))
        {
            break;         
        }
        HTTP_LOG("ssl_enable=%d", cfg.ssl_enable);
        if (cfg.ssl_enable != 0 && cfg.ssl_enable != 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break;    
        }

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &cfg.ssl_id))
        {
            break;         
        }
        HTTP_LOG("ssl_id=%d", cfg.ssl_id);
        if (cfg.ssl_enable == 1 && cfg.ssl_id == -1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break; 
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &cfg.cid))
        {
            break;         
        }
        HTTP_LOG("cid=%d", cfg.cid);
        if (cfg.cid < 1 || cfg.cid > 8)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break; 
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &cfg.conn_timeout))
        {
            break;         
        }
        HTTP_LOG("conn_timeout=%d", cfg.conn_timeout);
        if (cfg.conn_timeout < 0 || cfg.conn_timeout > 180)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break; 
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &cfg.rsp_timeout))
        {
            break;         
        }
        HTTP_LOG("rsp_timeout=%d", cfg.rsp_timeout);
        if (cfg.rsp_timeout < 0 || cfg.rsp_timeout > 60)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break; 
        }
    }while (0);

    if (0 == ret)
    {
        ret = cm_httpclient_set_cfg(client, cfg);
    }

    return ret;
}

// AT+HTTP="cm_httpclient_terminate"
int func_cm_httpclient_terminate(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    char* url = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        cm_httpclient_terminate(client);
    }while (0);

    return ret;
}

// AT+HTTP="cm_httpclient_custom_header_set",iLength,header
int func_cm_httpclient_custom_header_set(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    int iLength = 0;
    //char *cHeader = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 3)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &iLength))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break;         
        }
        HTTP_LOG("iLength=%d", iLength);
        if (custom_header) cm_free(custom_header);
        custom_header = cm_malloc(CM_AT_PARA_STRING_MAX_LENGTH + 3);
        if (!custom_header)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u",custom_header, CM_AT_PARA_STRING_MAX_LENGTH+1))
        {
            cm_free(custom_header);
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        HTTP_LOG("custom_header=%s", custom_header);
        if (iLength != strlen(custom_header))
        {
            cm_free(custom_header);
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ; 
        }
  
        ret = cm_httpclient_custom_header_set(client, custom_header, iLength);
    }while (0);

    return ret;
}

// AT+HTTP="cm_httpclient_custom_header_free"
int func_cm_httpclient_custom_header_free(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        ret = cm_httpclient_custom_header_free(client);
        if (custom_header) cm_free(custom_header);
    }while (0);

    return ret;
}

// AT+HTTP="cm_httpclient_specific_header_set",iLength,header
int func_cm_httpclient_specific_header_set(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    int iLength = 0;
    //char *cHeader = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 3)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;  
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &iLength))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break;         
        }
        HTTP_LOG("iLength=%d", iLength);
        if(specific_header) cm_free(specific_header);
        specific_header = cm_malloc(CM_AT_PARA_STRING_MAX_LENGTH + 3);
        if (!specific_header)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u",specific_header, CM_AT_PARA_STRING_MAX_LENGTH+1))
        {
            cm_free(specific_header);
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        HTTP_LOG("specific_header=%s", specific_header);
        if (iLength != strlen(specific_header))
        {
            cm_free(specific_header);
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ; 
        }

        ret = cm_httpclient_specific_header_set(client, specific_header, iLength);
    }while (0);

    return ret;
}

// AT+HTTP="cm_httpclient_specific_header_free"
int func_cm_httpclient_specific_header_free(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        ret = cm_httpclient_specific_header_free(client);
        if(specific_header) cm_free(specific_header);
    }while (0);

    return ret;
}

// AT+HTTP="cm_httpclient_request_start",iMethod,path,chunked,content_length
int func_cm_httpclient_request_start(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    int iMethod = -1;
    char *cPath = NULL;
    int chunked;
    int content_length;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 5)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;  
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &iMethod))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break;         
        }
        HTTP_LOG("iMethod=%d", iMethod);
        if(iMethod >=HTTPCLIENT_REQUEST_MAX)
        {
            HTTP_LOG("%s:method error %d",__func__,iMethod);
            return CM_HTTP_RET_CODE_PARAM_ERROR;
        }

        cPath = cm_malloc(CM_AT_PARA_STRING_MAX_LENGTH + 3);
        if (!cPath)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u",cPath, CM_AT_PARA_STRING_MAX_LENGTH+1))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        HTTP_LOG("content_length=%s", cPath);
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u",&chunked))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        HTTP_LOG("chunked=%d", chunked);
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u",&content_length))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        HTTP_LOG("content_length=%d", content_length);
        ret = cm_httpclient_request_start(client, (cm_httpclient_request_type_e)iMethod, cPath, chunked, content_length);
    }while (0);

    if (cPath) cm_free(cPath);
    return ret;
}

// AT+HTTP="cm_httpclient_request_content_set"
int func_cm_httpclient_request_content_set(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    char* url = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 2)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        if(content) cm_free(content);
        content = cm_malloc(CM_AT_PARA_STRING_MAX_LENGTH + 3);
        if (!content)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;  
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u",content, CM_AT_PARA_STRING_MAX_LENGTH+1))
        {
            cm_free(content);
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        HTTP_LOG("content=%s", content);
    }while (0);

    return ret;
}


// AT+HTTP="cm_httpclient_request_send",iLength,content
int func_cm_httpclient_request_send(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    int iLength = 0;
    char *cData = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 3)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;  
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &iLength))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break;         
        }
        HTTP_LOG("iLength=%d", iLength);

        cData = cm_malloc(CM_AT_PARA_STRING_MAX_LENGTH + 3);
        if (!cData)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u",cData, CM_AT_PARA_STRING_MAX_LENGTH+1))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        HTTP_LOG("cData=%s", cData);
        if (iLength != 0 && iLength != strlen(cData))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ; 
        }
  
        ret = cm_httpclient_request_send(client, cData, iLength);
    }while (0);

    if (cData) cm_free(cData);

    return ret;
}

// AT+HTTP="cm_httpclient_request_end"
int func_cm_httpclient_request_end(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    char* url = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        ret = cm_httpclient_request_end(client);
    }while (0);

    return ret;
}

// AT+HTTP="cm_httpclient_get_response_code"
int func_cm_httpclient_get_response_code(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    //int iHttpId = -1;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        cm_uart_printf_urc("+HTTPGETRSPCODE: %d", cm_httpclient_get_response_code(client));
    }while (0);

    return ret;
} 

// AT+HTTP="cm_httpclient_parse_header",header,key
int func_cm_httpclient_parse_header(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    char *cHeader = NULL;
    char *key = NULL;
    char * value = NULL;
    int value_len = 0;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 3)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;
        cHeader = cm_malloc(CM_AT_PARA_STRING_MAX_LENGTH + 3);
        if (!cHeader)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u",cHeader, CM_AT_PARA_STRING_MAX_LENGTH+1))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        strcat(cHeader, "\r\n");
        HTTP_LOG("cHeader=%s", cHeader);
        key = cm_malloc(128);
        if (!key)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u",key, 128+1))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        HTTP_LOG("key=%s", key);

        value_len = cm_httpclient_parse_header(cHeader, key, &value);
        if (value_len >= 0 && NULL != value)
        {
            value[value_len]='\0';
            HTTP_LOG("value_len=%d, value=%s", value_len, value);
            cm_uart_printf_urc("+HTTPPARSEHEADER: %s: %s", key, value);
        }
    }while (0);

    if (cHeader) cm_free(cHeader);
    if (key) cm_free(key);
    return ret;
}  

// AT+HTTP="cm_httpclient_sync_request",iMethod,path,content_length,content
int func_cm_httpclient_sync_request(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    int iMethod = -1;
    char *cPath = NULL;
    int content_length;
    char *cData = NULL;
    cm_httpclient_sync_param_t param = {0};
    cm_httpclient_sync_response_t response = {0};

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;
   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count > 5 || para_count < 4)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }

        para_p_new = (char *)para_p;
        lenOfPara_new = (uint32_t)lenOfPara;  
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &iMethod))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break;         
        }
        if(iMethod >=HTTPCLIENT_REQUEST_MAX)
        {
            HTTP_LOG("%s:method error %d",__func__,iMethod);
            return CM_HTTP_RET_CODE_PARAM_ERROR;
        }

        cPath = cm_malloc(CM_AT_PARA_STRING_MAX_LENGTH + 3);
        if (!cPath)
        {
            ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u",cPath, CM_AT_PARA_STRING_MAX_LENGTH+1))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%u", &content_length))
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break;         
        }

        if (para_count == 5)
        {
            cData = cm_malloc(CM_AT_PARA_STRING_MAX_LENGTH + 3);
            if (!cData)
            {
                ret = CM_HTTP_RET_CODE_MALLOC_FAIL;
                break ;
            }
            if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, ",%s.u",cData, CM_AT_PARA_STRING_MAX_LENGTH+1))
            {
                ret = CM_HTTP_RET_CODE_PARAM_ERROR;
                break ;
            }
            
            if (content_length != 0 && content_length != strlen(cData))
            {
                ret = CM_HTTP_RET_CODE_PARAM_ERROR;
                break ; 
            }
        }

        param.method = iMethod;
        param.path = cPath;
        param.content_length = content_length;
        param.content = cData;
        ret = cm_httpclient_sync_request(client, param, &response);

        cm_uart_printf_urc("response_code:%d", response.response_code);
        cm_uart_printf_urc_data(response.response_header, response.response_header_len, response.response_content, response.response_content_len);

    }while (0);

    if (cPath) cm_free(cPath);
    if (cData) cm_free(cData);
    return ret;
}

// AT+HTTP="cm_httpclient_sync_free_data"
int func_cm_httpclient_sync_free_data(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    //int iHttpId = -1;
    int ret = 0;
    char* url = NULL;

    OS_ASSERT(para_p != OS_NULL);

    const char *para_p_new = (char *)para_p;
    uint32_t lenOfPara_new = (uint32_t)lenOfPara;
    os_uint32_t para_count = 0;


   do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$");
        HTTP_LOG("para_count=%d", para_count);
        if(para_count != 1)
        {
            ret = CM_HTTP_RET_CODE_PARAM_ERROR;
            break ;
        }
        cm_httpclient_sync_free_data(client);
    }while (0);
    return ret;
}

#endif


