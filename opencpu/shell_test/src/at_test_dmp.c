/*********************************************************
 *  @file    cm_demo_dmp.c
 *  @brief   OpenCPU DMP示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created by thf 2025/01/20
 ********************************************************/

#ifdef CM_DEMO_DMP_SUPPORT

#include <os.h>
#include <stdlib.h>

#include "at_api.h"
#include "at_parser.h"
#include "cm_dmp.h"

typedef AT_CommandItem at_cmd_t;

#define DMP_LOG(fmt, ...) osPrintf("[OC_DMP_TEST][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define DMP_CMD_STR_LEN 512

#define CM_DMP_RET_CODE_OK 0 //成功
#define CM_DMP_RET_AT_FAIL 1101 //AT 返回失败
#define CM_DMP_RET_CODE_PARAM_ERROR 50 //参数错误 50

void dmp_oc_test_entry(os_uint8_t id, os_uint8_t *at_buf, os_uint16_t lenOfPara);
int func_cm_dm_set_option(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_dm_get_option(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);

AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_dmp_oc[] =
{
    {.name = "+DMP",   .setFunc = dmp_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

// AT+DMP="XX", param1, param2, ...
// "XX" 为函数名
void dmp_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    DMP_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$");
        if (para_count > 3)
        {
            ret = CM_DMP_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            ret = CM_DMP_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_dm_start") == 0)
        {
            ret = cm_mobile_dm_start();
            DMP_LOG("dm_start executing.... ");
        }
        else if (strcmp(func_name, "cm_dm_stop") == 0)
        {
            ret = cm_mobile_dm_stop();
            DMP_LOG("dm_stop executing! ");
        }
        else if (strcmp(func_name, "cm_dm_get_state") == 0)
        {
            ret = cm_mobile_dm_get_state();
            cm_uart_printf_urc("+DMP: %d",ret);
            if(ret >= 0)
                ret = 0;
        }
        else if (strcmp(func_name, "cm_dm_set_option") == 0)
        {
            ret = func_cm_dm_set_option(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_dm_get_option") == 0)
        {
            ret = func_cm_dm_get_option(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            DMP_LOG("Error func name[%s]", func_name);
            ret = CM_DMP_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}

// AT+DMP="cm_dm_set_option",type,param
int func_cm_dm_set_option(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t handle = -1;
    int ret = 0;
    int  type  = 0;
    int  param = 0;
    char inputStr[50] = {0};

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &type))
    {
        return CM_DMP_RET_CODE_PARAM_ERROR;
    }
    if(type == CM_MOBILE_DM_INTERVAL || type == CM_MOBILE_DM_TMLTYPE || type == CM_MOBILE_DM_PORT ||
       type == CM_MOBILE_DM_WEARING || type == CM_MOBILE_DM_BATCAPCURR || type == CM_MOBILE_DM_NETSTS ||
       type == CM_MOBILE_DM_ENABLE)
    {        
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &param))
        {
            return CM_DMP_RET_CODE_PARAM_ERROR;
        }
        ret = cm_mobile_dm_set_option((cm_mobile_dm_option_e)type,&param,2);
    }
    else
    {    
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", inputStr, 50))
        {
            return CM_DMP_RET_CODE_PARAM_ERROR;
        }
        ret = cm_mobile_dm_set_option((cm_mobile_dm_option_e)type,inputStr,50);
    } 
 
    DMP_LOG("type[%d] param[%d] inputStr[%s]", type, param, inputStr);
    return ret;
}

// AT+DMP="cm_dm_get_option",type
int func_cm_dm_get_option(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    int  type  = 0;
    int  param = 0;
    char inputStr[50] = {0};

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &type))
    {
        return CM_DMP_RET_CODE_PARAM_ERROR;
    }
    if(type == CM_MOBILE_DM_INTERVAL || type == CM_MOBILE_DM_TMLTYPE || type == CM_MOBILE_DM_PORT ||
       type == CM_MOBILE_DM_WEARING || type == CM_MOBILE_DM_BATCAPCURR || type == CM_MOBILE_DM_NETSTS ||
       type == CM_MOBILE_DM_ENABLE)
    {        
        ret = cm_mobile_dm_get_option((cm_mobile_dm_option_e)type,&param,2);
        if(ret > 0)
            cm_uart_printf_urc("+DMP: %d,%d",type,param);
    }
    else
    {    
        ret = cm_mobile_dm_get_option((cm_mobile_dm_option_e)type,inputStr,50);
        if(ret > 0)
            cm_uart_printf_urc("+DMP: %d,%s",type,inputStr);
    } 
 
    DMP_LOG("type[%d] param[%d] inputStr[%s]", type, param, inputStr);
    if(ret == -1)
        ret = CM_DMP_RET_AT_FAIL;
    else
        ret = 0;
    return ret;
}

#endif


