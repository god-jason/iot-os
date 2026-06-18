/*********************************************************
 *  @file    cm_demo_ftp.c
 *  @brief   OpenCPU FTP示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created By ZY 2024/12/9
 ********************************************************/

#ifdef CM_DEMO_VIRT_AT_SUPPORT
#ifdef OS_USING_SHELL

#include <os.h>
#include <stdlib.h>

// #include "cm_common_inc.h"
#include "at_api.h"
#include "at_parser.h"
#include "cm_demo_common.h"
#include "cm_virt_at.h"
#include "cm_mem.h"

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

typedef AT_CommandItem at_cmd_t;


#define VIRTAT_CMD_STR_LEN 128
#define CM_VIRTAT_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_VIRTAT_RET_CODE_PARAM_ERROR 50 //参数错误 50


void virt_at_oc_test_entry(os_uint8_t id, os_uint8_t *at_buf, os_uint16_t lenOfPara);
int func_cm_virt_at_urc_cb_reg(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_virt_at_urc_cb_dereg(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_virt_at_send_async(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_virt_at_send_sync(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);


void urc_cb(uint8_t *urc, int32_t urc_len)
{
    cm_uart_printf_urc("[urc_cb]%s", urc);
}

void at_send_cb(cm_virt_at_param_t *param)
{
    if(param->event == CM_VIRT_AT_OK)
    {
        cm_uart_printf_urc("at_send_cb:event=%d rsp=%s rsp_len=%d\n", param->event, param->rsp, param->rsp_len);
    }
    else
    {
        cm_uart_printf_urc("error\n");
    }
}


AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_virt_at_oc[] =
{
    {.name = "+VIRTAT",   .setFunc = virt_at_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

// AT+VIRTAT="XX", param1, param2, ...
// "XX" 为函数名
void virt_at_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    osPrintf("id[%d] para_p[%s] lenOfPara[%d]\n", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm((char **)&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$");
        if (para_count < 1 || para_count > 4)
        {
            ret = CM_VIRTAT_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            ret = CM_VIRTAT_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_virt_at_urc_cb_reg") == 0)
        {
            ret = func_cm_virt_at_urc_cb_reg(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_virt_at_urc_cb_dereg") == 0)
        {
            ret = func_cm_virt_at_urc_cb_dereg(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_virt_at_send_async") == 0)
        {
            ret = func_cm_virt_at_send_async(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        /*else if (strcmp(func_name, "cm_virt_at_send_sync") == 0)
        {
            ret = func_cm_virt_at_send_sync(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }*/
        else
        {
            osPrintf("Error func name[%s]\n", func_name);
            ret = -1;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}

// AT+VIRTAT="cm_virt_at_urc_cb_reg","prefix"
int func_cm_virt_at_urc_cb_reg(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;
    char* prefix = NULL;


    prefix = cm_malloc(VIRTAT_CMD_STR_LEN);
    if (prefix == NULL) {
        return CM_VIRTAT_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", prefix, VIRTAT_CMD_STR_LEN))
    {
        cm_free(prefix);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }

    if(strlen(prefix) == 0)
    {
        cm_free(prefix);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }

    ret = cm_virt_at_urc_cb_reg(prefix, (cm_virt_at_urc_cb)urc_cb);

    if (ret < 0)
    {
        cm_uart_printf_urc("+VIRTATCBREG: %d", ret);
    }

    cm_free(prefix);
    return 0;
}

// AT+VIRTAT="cm_virt_at_urc_cb_dereg","prefix"
int func_cm_virt_at_urc_cb_dereg(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;
    char* prefix = NULL;


    prefix = cm_malloc(VIRTAT_CMD_STR_LEN);
    if (prefix == NULL) {
        return CM_VIRTAT_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", prefix, VIRTAT_CMD_STR_LEN))
    {
        cm_free(prefix);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }

    if(strlen(prefix) == 0)
    {
        cm_free(prefix);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }

    ret = cm_virt_at_urc_cb_dereg(prefix, (cm_virt_at_urc_cb)urc_cb);

    if (ret < 0)
    {
        cm_uart_printf_urc("+VIRTATCBDEREG: %d", ret);
    }

    cm_free(prefix);
    return 0;
}

// AT+VIRTAT="cm_virt_at_send_async","at_cmd","user_param"
int func_cm_virt_at_send_async(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;
    char operation[64] = {0};
    char* at_cmd = NULL;
    char* user_param = NULL; 

    at_cmd = cm_malloc(VIRTAT_CMD_STR_LEN);
    if (at_cmd == NULL) {
        return CM_VIRTAT_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%se.u", at_cmd, VIRTAT_CMD_STR_LEN))
    {
        cm_free(at_cmd);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }

    if(strlen(at_cmd) == 0)
    {
        cm_free(at_cmd);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }
    user_param = cm_malloc(VIRTAT_CMD_STR_LEN);
    if (user_param == NULL) {
        return CM_VIRTAT_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", user_param, VIRTAT_CMD_STR_LEN))
    {
        cm_free(at_cmd);
        cm_free(user_param);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }

    osPrintf("at_cmd[%s] user_param[%s]", at_cmd, user_param);

    sprintf(operation, "%s\r\n", at_cmd);
    ret = cm_virt_at_send_async(operation, at_send_cb, user_param);

    if (ret < 0)
    {
        cm_uart_printf_urc("+VIRTATSEND: %d", ret);
    }

    cm_free(at_cmd);
    cm_free(user_param);
    return 0;
}

// AT+VIRTAT="cm_virt_at_send_sync","at_cmd",timeout
int func_cm_virt_at_send_sync(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = -1;
    char* at_cmd = NULL;
    uint32_t timeout = 0; 
    char operation[64] = {0};
    uint8_t rsp[128] = {0};
    int32_t rsp_len = 0;

    at_cmd = cm_malloc(VIRTAT_CMD_STR_LEN);
    if (at_cmd == NULL) {
        return CM_VIRTAT_RET_CODE_MALLOC_FAIL;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%se.u", at_cmd, VIRTAT_CMD_STR_LEN))
    {
        cm_free(at_cmd);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }

    if(strlen(at_cmd) == 0)
    {
        cm_free(at_cmd);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }
    
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &timeout))
    {
        cm_free(at_cmd);
        return CM_VIRTAT_RET_CODE_PARAM_ERROR;
    }

    osPrintf("at_cmd[%s] timeout[%d]\n", at_cmd, timeout);

    sprintf(operation, "%s\r\n", at_cmd);

    ret = cm_virt_at_send_sync(operation, rsp, &rsp_len, timeout);

    if (ret < 0)
    {
        osPrintf("func_cm_virt_at_send_async fail\n");
    }

    cm_free(at_cmd);
    return 0;
}

#endif
#endif


