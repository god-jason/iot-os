/*********************************************************
 *  @file    at_test_lbs.c
 *  @brief   OpenCPU LBS示例
 *  Copyright (c) 2024 China Mobile IOT.
 *  All rights reserved.
 *  created by thf 2025/01/20
 ********************************************************/

#ifdef CM_DEMO_ATLBS_SUPPORT

#include <os.h>
#include <stdlib.h>

#include "at_api.h"
#include "at_parser.h"
#include "cm_lbs.h"

typedef AT_CommandItem at_cmd_t;

#define LBS_LOG(fmt, ...) osPrintf("[OC_LBS_TEST][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LBS_CMD_STR_LEN 512

#define CM_LBS_RET_CODE_OK 0 //成功
#define CM_LBS_RET_AT_FAIL 1101 //AT 返回失败
#define CM_LBS_RET_CODE_PARAM_ERROR 50 //参数错误 50

void lbs_oc_test_entry(os_uint8_t id, os_uint8_t *at_buf, os_uint16_t lenOfPara);
int func_cm_lbs_init(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lbs_get_attr(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lbs_location(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);


AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_lbs_oc[] =
{
    {.name = "+LBS",   .setFunc = lbs_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};
static char cm_test_cb_arg[] = {"cm lbs location"};
void cm_atlbs_cb(cm_lbs_callback_event_e event,cm_lbs_location_rsp_t *location,void *cb_arg)
{
    cm_lbs_location_platform_e lbs_platform = 0;
    int ret =0;
    
    LBS_LOG("cm_lbs_callback_event =%d\r\n",event);

    if(NULL != cb_arg)
    {
        LBS_LOG("cb_arg=%s\r\n",(char *)cb_arg);
    }

    if(event == CM_LBS_LOCATION_OK)
    {
        lbs_platform = location->platform;
        LBS_LOG("location.platform=%d\r\n",location->platform);
        LBS_LOG("location.state=%d\r\n",location->state);
        LBS_LOG("location.longittude=%s\r\n",location->longitude);
        LBS_LOG("location.latitude=%s\r\n",location->latitude);
        LBS_LOG("location.precision=%s\r\n",location->location_describe);
        LBS_LOG("location.desc=%s\r\n",location->radius);
        LBS_LOG("location.country=%s\r\n",location->country);
        LBS_LOG("location.province=%s\r\n",location->province);
        LBS_LOG("location.city=%s\r\n",location->city);
        LBS_LOG("location.citycode=%s\r\n",location->citycode);
        LBS_LOG("location.adcode=%s\r\n",location->adcode);
        LBS_LOG("location.poi=%s\r\n",location->poi);
        
        cm_uart_printf_urc("+LBS: %s,%s",location->longitude,location->latitude);

        cm_lbs_deinit();

        if(lbs_platform == CM_LBS_PLAT_ONEOSPOS)
        {
            char pid[64] = {0};
            cm_lbs_oneospos_attr_t noeospos_cfg_acqure = {pid,0};
            
            ret = cm_lbs_get_attr(lbs_platform,&noeospos_cfg_acqure);
            
            if(ret == 0)
            {
                    LBS_LOG("noeospos_cfg_acqure timeout=%d\r\n",noeospos_cfg_acqure.time_out);
                    LBS_LOG("noeospos_cfg_acqure pid =%s\r\n",noeospos_cfg_acqure.pid);
            }
            else
            {
                LBS_LOG("cm_lbs_get_attr ret=%d\r\n",ret);
            }
        }
        else
        {
            LBS_LOG("Not support this platform %d\r\n",lbs_platform);
        }
    }

 }
cm_lbs_oneospos_attr_t noeospos_cfg = {0};
cm_lbs_location_platform_e lbs_platform = CM_LBS_PLAT_START;
char pid[CM_LBS_NVCONFIG_ONEOSPOS_PIDLEN] = {0};

// AT+LBS="XX", param1, param2, ...
// "XX" 为函数名
void lbs_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    LBS_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$,$");
        if (para_count > 6)
        {
            ret = CM_LBS_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        int platform = 0;
        char func_name[50]={0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 50))
        {
            ret = CM_LBS_RET_CODE_PARAM_ERROR;
            break;
        }
        if (strcmp(func_name, "cm_lbs_init") == 0)
        {
            ret = func_cm_lbs_init(id, (os_uint8_t *)para_p_new, lenOfPara_new);
            LBS_LOG("cm_lbs_init ret=%d\r\n",ret);
        }
        else if (strcmp(func_name, "cm_lbs_get_attr") == 0)
        {
            ret = func_cm_lbs_get_attr(id, (os_uint8_t *)para_p_new, lenOfPara_new);
            LBS_LOG("cm_lbs_get_attr ret=%d\r\n",ret);
        }
        else if (strcmp(func_name, "cm_lbs_location") == 0)
        {
            ret = func_cm_lbs_location(id, (os_uint8_t *)para_p_new, lenOfPara_new);
            LBS_LOG("cm_lbs_location ret=%d\r\n",ret);
        }
        else
        {
            LBS_LOG("Not support other function[%s]", func_name);
            ret = CM_LBS_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}
// AT+LBS="cm_lbs_init",<platform>,<pid>,<timeout>,<nearbts_enable>
int func_cm_lbs_init(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    int ret_val = 0;
    xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &lbs_platform);
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u,%u,%u", pid,CM_LBS_NVCONFIG_ONEOSPOS_PIDLEN,&noeospos_cfg.time_out,&noeospos_cfg.nearbts_enable))
    {
        return CM_LBS_RET_CODE_PARAM_ERROR;
    } 
    noeospos_cfg.pid=pid;

    LBS_LOG("pid[%s] port[%d] timeout[%d] nearbts_enable[%d]", \
            noeospos_cfg.pid, noeospos_cfg.time_out, noeospos_cfg.nearbts_enable);
    ret_val = cm_lbs_init(lbs_platform,&noeospos_cfg);
    cm_uart_printf_urc("+BSINIT: %d", ret_val);
    return ret;
}
// AT+LBS="cm_lbs_get_attr"
int func_cm_lbs_get_attr(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &lbs_platform);
    if(lbs_platform == CM_LBS_PLAT_ONEOSPOS)
    {
        char pid[64] = {0};
        cm_lbs_oneospos_attr_t noeospos_cfg_acqure = {pid,0};
        ret = cm_lbs_get_attr(lbs_platform,&noeospos_cfg_acqure);
        if(ret == 0)
        {
            cm_uart_printf_urc("pid[%s]  timeout[%d] nearbts_enable[%d]", \
                    noeospos_cfg_acqure.pid, noeospos_cfg_acqure.time_out, noeospos_cfg_acqure.nearbts_enable);
        }
    }
    else
    {
        char pid[64] = {0};
        cm_lbs_amap_location_attr_t us_amap_attr = {0};
        ret = cm_lbs_get_attr(lbs_platform,&us_amap_attr);
        if(ret == 0)
        {
            cm_uart_printf_urc("apikey[%s],digit_sig_enable[%d],timeout[%d],nearbts_enable[%d],show_fileds_enable[%d] ", \
                    us_amap_attr.api_key,us_amap_attr.digital_sign_enable, us_amap_attr.time_out, us_amap_attr.nearbts_enable,us_amap_attr.show_fields_enable);
        }
    }
    return ret;
}
// AT+LBS="cm_lbs_location"
int func_cm_lbs_location(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    if(lbs_platform == CM_LBS_PLAT_ONEOSPOS)
    {
        ret = cm_lbs_location(cm_atlbs_cb,(void *)cm_test_cb_arg);
    }
    else
    {
        LBS_LOG("LBS_LBS_LOCATION:not support this platform %d",lbs_platform);
        return CM_LBS_RET_CODE_PARAM_ERROR;
    }    
    return ret;
}

#endif


