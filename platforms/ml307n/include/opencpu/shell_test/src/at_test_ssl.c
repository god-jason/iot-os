/*********************************************************
 *  @file    cm_demo_ftp.c
 *  @brief   OpenCPU FTP示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created By ZY 2024/12/9
 ********************************************************/

#ifdef CM_DEMO_SSL_SUPPORT
#ifdef OS_USING_SHELL

#include <os.h>
#include <stdlib.h>

#include "at_api.h"
#include "at_parser.h"
#include "cm_ssl.h"
#include "cm_mem.h"


typedef AT_CommandItem at_cmd_t;

#if 0
#ifdef CM_MEMLEAK_SUPPORT
#define cm_malloc(n)        cm_dbg_malloc(n)
#define cm_realloc(p,s)     cm_dbg_realloc(p, s)
#define cm_calloc(n,s)      cm_dbg_calloc(n, s)
#define cm_free(buf)    \
    do                  \
    {                   \
        if (buf)        \
        {               \
            cm_dbg_free(buf);  \
            buf = NULL; \
        }               \
    } while (0)
#else
#define cm_malloc(n)           malloc(n)
#define cm_realloc(n,s)        realloc(n, s)
#define cm_calloc(n,s)         calloc(n, s)
#define cm_free(buf)    \
    do                  \
    {                   \
        if (buf)        \
        {               \
            free(buf);  \
            buf = NULL; \
        }               \
    } while (0)
#endif
#endif

#define SSL_CMD_STR_LEN 128
#define CM_SSL_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_SSL_RET_CODE_PARAM_ERROR 50 //参数错误 50
#define SSL_MAX_HOST_NAME_LEN 255
static cm_ssl_ctx_t *test_ssl_ctx = NULL;          //ssl 上下文

typedef struct cm_ssl_datamode_msg_
{
    int msg_len;      //当前接收长度
    char *msg;         //存储数据
} cm_ssl_datamode_msg_t;

typedef void (*cm_datamode_cb_t)(void *data, int data_len, void *arg, int event);

typedef struct
{
    UINT32 at_handle;
    int mode;          //0 透传模式；1 数据模式不检测1A，1B； 2 数据模式检测1A，1B
    int length;        //输入发送最大长度
    int recv_len;      //已接收长度
    int timeout;       //超时时间，单位100ms
    void *arg;         //可用作各应用内部ID区分，如TCPIP 0-5
    void *timer;       //超时定时器
    void *wait;        //内部等待定时器
    cm_datamode_cb_t datamode_cb;
} cm_datamode_ctx_t;

extern void cm_datamode_init(UINT32 at_handle, int mode, int length, int timeout, cm_datamode_cb_t cm_data_cb, void *arg);

void ssl_oc_test_entry(os_uint8_t id, os_uint8_t *at_buf, os_uint16_t lenOfPara);
int func_cm_ssl_setopt(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ssl_conn(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ssl_conn_with_host(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ssl_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ssl_write(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara, int para_count);
int func_cm_ssl_read(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ssl_check_pending(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ssl_get_bytes_avail(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ssl_list_cipher(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);



// AT+SSL="XX", param1, param2, ...
// "XX" 为函数名
void ssl_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    osPrintf("id[%d] para_p[%s] lenOfPara[%d]\n", id, para_p, lenOfPara);
    char func_name[128] = {0};
    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$,$");
        if (para_count < 1 || para_count > 6)
        {
            ret = CM_SSL_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;  

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            ret = CM_SSL_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_ssl_setopt") == 0)
        {
            ret = func_cm_ssl_setopt(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ssl_conn") == 0)
        {
            ret = func_cm_ssl_conn(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ssl_conn_with_host") == 0)
        {
            ret = func_cm_ssl_conn_with_host(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ssl_close") == 0)
        {
            ret = func_cm_ssl_close(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ssl_write") == 0)
        {
            ret = func_cm_ssl_write(id, (os_uint8_t *)para_p_new, lenOfPara_new, para_count);
        }
        else if (strcmp(func_name, "cm_ssl_read") == 0)
        {
            ret = func_cm_ssl_read(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ssl_check_pending") == 0)
        {
            ret = func_cm_ssl_check_pending(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ssl_get_bytes_avail") == 0)
        {
            ret = func_cm_ssl_get_bytes_avail(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ssl_list_cipher") == 0)
        {
            ret = func_cm_ssl_list_cipher(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            osPrintf("Error func name[%s]\n", func_name);
            ret = -1;
        }
    } while(0);


    cm_uart_printf_result(id, ret);
}

// AT+SSL="cm_ssl_setopt",ssl_id,type,value
int func_cm_ssl_setopt(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;
    int32_t ssl_id = 0;
    os_uint8_t type = 0;
    os_uint16_t val = 0;


    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &ssl_id))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &type))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }

    if(ssl_id >= 6)
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }

    if(type >= CM_SSL_PARAM_VERIFY && type <= CM_SSL_PARAM_IGNOREVERIFY)
    {
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &val))
        {
            return CM_SSL_RET_CODE_PARAM_ERROR;
        }
        osPrintf("ssl_id[%d], type[%d], val[%d]\n", ssl_id, type, val);
        if(type == CM_SSL_PARAM_NEGOTIME || type == CM_SSL_PARAM_CIPHER_SUITE)
        {   
            ret = cm_ssl_setopt(ssl_id, type, &val);
        }   
        else
        {
            os_uint8_t u8_val = val;
            ret = cm_ssl_setopt(ssl_id, type, &u8_val);
        }
        if (ret < 0)
        {
            cm_uart_printf_urc("+SSLSETOPT: %d", ret);
        }
    }
    else if(type >= CM_SSL_PARAM_CA_CERT_DELETE && type <= CM_SSL_PARAM_CLI_KEY_DELETE)
    {
        ret = cm_ssl_setopt(ssl_id, type, NULL);
        if (ret < 0)
        {
            cm_uart_printf_urc("+SSLSETOPT: %d", ret);
        }
    }
    else
    {
        cm_uart_printf_urc("+SSLSETOPT: OPERATION NOT SUPPORT");
    }
    return 0;
}


// AT+SSL="cm_ssl_conn",ssl_id,socket,timeout
int func_cm_ssl_conn(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;
    int32_t ssl_id = 0;
        
    int32_t socket = 0;
    int32_t timeout = 0;


    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &ssl_id))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &socket))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &timeout))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }

    osPrintf("ssl_id[%d] socket[%d] timeout[%d]", ssl_id, socket, timeout);

    ret = cm_ssl_conn(&test_ssl_ctx, ssl_id, socket, timeout);

    if (ret != 0)
    {
        cm_uart_printf_urc("+SSLCONN: %d", ret);
    }

    return 0;
}

// AT+SSL="cm_ssl_conn_with_host",ssl_id,socket,timeout,"host"
int func_cm_ssl_conn_with_host(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;
    int32_t ssl_id = 0;
    int32_t socket = 0;
    int32_t timeout = 0;
    char host_str[SSL_MAX_HOST_NAME_LEN] = {0};

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &ssl_id))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &socket))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &timeout))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", host_str, SSL_MAX_HOST_NAME_LEN))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }
    osPrintf("ssl_id[%d] socket[%d] timeout[%d] host[%s]", ssl_id, socket, timeout, host_str);

    ret = cm_ssl_conn_with_host(&test_ssl_ctx, ssl_id, socket, timeout, host_str);

    if (ret != 0)
    {
        cm_uart_printf_urc("+SSLCONNHOST: %d", ret);
    }

    return 0;
}

// AT+SSL="cm_ssl_close"
int func_cm_ssl_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    if(test_ssl_ctx == NULL)
    {
        cm_uart_printf_urc("+SSLCLOSE: fail! call cm_ssl_conn first");
        return 0;
    }
    cm_ssl_close((void **)&test_ssl_ctx);

    return 0;
}

static void __cm_ssl_datamode_cb(void *data, int data_len, void *arg, int event)
{
    cm_datamode_ctx_t *cm_datamode_ctx = arg;
    cm_ssl_datamode_msg_t *ssl_msg = cm_datamode_ctx->arg;
    int ret = 0;
    if(data_len == 0)
    {
        ret = CM_SSL_RET_CODE_PARAM_ERROR;
    }
    else
    {
        memcpy(ssl_msg->msg + ssl_msg->msg_len, data, data_len);
        ret = cm_ssl_write(test_ssl_ctx, data, data_len);
        cm_uart_printf_urc("+SSLWRITE: %d", ret);
    }
    cm_free(ssl_msg->msg);
    cm_free(ssl_msg);
    cm_datamode_deinit(cm_datamode_ctx->at_handle);
}

// AT+SSL="cm_ssl_write","string"
int func_cm_ssl_write(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara, int para_count)
{
    int32_t ret = -1;
    char write_str[SSL_MAX_HOST_NAME_LEN] = {0};

    if(test_ssl_ctx == NULL)
    {
        cm_uart_printf_urc("+SSLWRITE: fail! call cm_ssl_conn first");
        return 0;
    }

    if(para_count == 1)
    {
        cm_ssl_datamode_msg_t *ssl_msg = cm_malloc(sizeof(cm_ssl_datamode_msg_t));
        ssl_msg->msg = cm_malloc(SSL_MAX_HOST_NAME_LEN + 1);
        ssl_msg->msg_len = 0;
        cm_datamode_init(id,
                        1,
                        255,
                        10,
                        __cm_ssl_datamode_cb,
                        (void *)ssl_msg);

        cm_printf_ch_noCRLF(id ,"\r\n> ");
        return 0;
    }   
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", write_str, SSL_MAX_HOST_NAME_LEN))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }
    osPrintf("write string[%s]", write_str);
    ret = cm_ssl_write(test_ssl_ctx, write_str, strlen(write_str));

    cm_uart_printf_urc("+SSLWRITE: %d", ret);

    return 0;
}

// AT+SSL="cm_ssl_read"
int func_cm_ssl_read(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;
    char *readbuf;
    int32_t read_len = 0;

    if(test_ssl_ctx == NULL)
    {
        cm_uart_printf_urc("+SSLREAD: fail! call cm_ssl_conn first");
        return 0;
    }
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &read_len))
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }
    if(read_len >= 1023)
    {
        return CM_SSL_RET_CODE_PARAM_ERROR;
    }
    readbuf = cm_malloc(read_len+1);
    if (readbuf == NULL) {
        return CM_SSL_RET_CODE_MALLOC_FAIL;
    }

    ret = cm_ssl_read(test_ssl_ctx, readbuf, read_len);
    readbuf[read_len] = 0;
    if (ret > 0)
    {
        cm_uart_printf_urc("+SSLREAD: %s\n", readbuf);
    }
    else
    {
        cm_uart_printf_urc("+SSLREAD rtn %d", ret);
    }
  
    cm_free(readbuf);
    return 0;
}

// AT+SSL="cm_ssl_check_pending"
int func_cm_ssl_check_pending(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;

    if(test_ssl_ctx == NULL)
    {
        cm_uart_printf_urc("+SSLCHECKPENDING: fail! call cm_ssl_conn first");
        return 0;
    }
    
    ret = cm_ssl_check_pending(test_ssl_ctx);
    cm_uart_printf_urc("+SSLCHECKPENDING: %d", ret);

    return 0;
}

// AT+SSL="cm_ssl_get_bytes_avail"
int func_cm_ssl_get_bytes_avail(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;

    if(test_ssl_ctx == NULL)
    {
        cm_uart_printf_urc("+SSLGETBYTES: fail! call cm_ssl_conn first");
        return 0;
    }

    cm_ssl_read(test_ssl_ctx, NULL, 0);
    ret = cm_ssl_get_bytes_avail(test_ssl_ctx);
    cm_uart_printf_urc("+SSLGETBYTES: %d", ret);

    return 0;
}

// AT+SSL="cm_ssl_list_cipher"
int func_cm_ssl_list_cipher(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int i = 0;
    const int *ciphersuites = cm_ssl_list_cipher();
    for (i = 0; ciphersuites[i] != 0; i++)
    {
        ;
    }

    char *rsp = cm_malloc(i * 7);
    int len = 0;

    for (i = 0; ciphersuites[i] != 0; i++)
    {
        len += sprintf(rsp + len, "0X%X,", ciphersuites[i]);
    }
    rsp[len - 1] = 0;
    cm_uart_printf_urc("+SSLLISTCIPHER: %s", rsp);
    cm_free(rsp);
    return 0;
}


AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_ssl_oc[] =
{
    {.name = "+SSL",   .setFunc = ssl_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

#endif
#endif



