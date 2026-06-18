/*********************************************************
 *  @file    cm_demo_lbs.c
 *  @brief   OpenCPU LBS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by zyf 2021/08/30
 ********************************************************/

#ifdef CM_DEMO_LBS_SUPPORT
#ifdef OS_USING_SHELL

#include "stdlib.h"
#include "cm_lbs.h"
#include <nr_micro_shell.h>

#define cm_demo_printf osPrintf

static char cm_test_cb_arg[] = {"cm lbs location"};

 void cm_lbs_cb(cm_lbs_callback_event_e event,cm_lbs_location_rsp_t *location,void *cb_arg)
{
    cm_lbs_location_platform_e lbs_platform = 0;
    int ret =0;
    
    cm_demo_printf("cm_lbs_callback_event =%d\r\n",event);

    if(NULL != cb_arg)
    {
        cm_demo_printf("cb_arg=%s\r\n",(char *)cb_arg);
    }

    if(event == CM_LBS_LOCATION_OK)
    {
        lbs_platform = location->platform;
        cm_demo_printf("location.platform=%d\r\n",location->platform);
        cm_demo_printf("location.state=%d\r\n",location->state);
        cm_demo_printf("location.longittude=%s\r\n",location->longitude);
        cm_demo_printf("location.latitude=%s\r\n",location->latitude);
        cm_demo_printf("location.precision=%s\r\n",location->location_describe);
        cm_demo_printf("location.desc=%s\r\n",location->radius);
        cm_demo_printf("location.country=%s\r\n",location->country);
        cm_demo_printf("location.province=%s\r\n",location->province);
        cm_demo_printf("location.city=%s\r\n",location->city);
        cm_demo_printf("location.citycode=%s\r\n",location->citycode);
        cm_demo_printf("location.adcode=%s\r\n",location->adcode);
        cm_demo_printf("location.poi=%s\r\n",location->poi);

        cm_lbs_deinit();

        if(lbs_platform == CM_LBS_PLAT_AMAP10)
        {
            uint8_t aplikey[64] = {0};
            cm_lbs_amap_location_attr_t apap10_cfg_acqure = {aplikey,0};
            
            ret = cm_lbs_get_attr(lbs_platform,&apap10_cfg_acqure);
            
            if(ret == 0)
            {
                cm_demo_printf("apap10_cfg_acqure timeout=%d\r\n",apap10_cfg_acqure.time_out);
                cm_demo_printf("apap10_cfg_acqure aplikey =%s\r\n",apap10_cfg_acqure.api_key);
            }
            else
            {
                cm_demo_printf("cm_lbs_get_attr ret=%d\r\n",ret);
            }
        }
        else if(lbs_platform == CM_LBS_PLAT_AMAP20)
        {
            uint8_t aplikey[64] = {0};
            cm_lbs_amap_location_attr_t apap20_cfg_acqure = {aplikey,0};
            
            ret = cm_lbs_get_attr(CM_LBS_PLAT_AMAP20,&apap20_cfg_acqure);
            
            if(ret == 0)
            {
                cm_demo_printf("apap20_cfg_acqure timeout=%d\r\n",apap20_cfg_acqure.time_out);
                cm_demo_printf("apap20_cfg_acqure aplikey =%s\r\n",apap20_cfg_acqure.api_key);
            }
            else
            {
                cm_demo_printf("cm_lbs_get_attr ret=%d\r\n",ret);
            }
        }
        else if(lbs_platform == CM_LBS_PLAT_ONEOSPOS)
        {
            char pid[64] = {0};
            cm_lbs_oneospos_attr_t noeospos_cfg_acqure = {pid,0};
            
            ret = cm_lbs_get_attr(lbs_platform,&noeospos_cfg_acqure);
            
            if(ret == 0)
            {
                    cm_demo_printf("noeospos_cfg_acqure timeout=%d\r\n",noeospos_cfg_acqure.time_out);
                    cm_demo_printf("noeospos_cfg_acqure pid =%s\r\n",noeospos_cfg_acqure.pid);
            }
            else
            {
                cm_demo_printf("cm_lbs_get_attr ret=%d\r\n",ret);
            }
        }
    }

 }
 
void SHELL_test_lbs_start(char argc, char **argv)
{
    int ret = 0;
    cm_lbs_location_platform_e lbs_platform = CM_LBS_PLAT_START;
    
    cm_demo_printf("cmd len =%d\r\n",argc);
    if( argv == NULL)
    {
        cm_demo_printf(" cmd = NULL or cmd len = %d error!\n", argc);
        return;
    }

    if(argc > 2)
    {
        lbs_platform = atoi((char *)argv[2]);
    }
    else
    {
        cm_demo_printf("cmd len error\r\n");
        return;
    }

    if((lbs_platform == CM_LBS_PLAT_AMAP10)&&(argc > 6))
    {
        cm_lbs_amap_location_attr_t apap10_cfg = {0};
        uint8_t aplikey[64] = {0};
        cm_lbs_amap_location_attr_t apap10_cfg_acqure = {aplikey,0};
    
        apap10_cfg.api_key = (uint8_t *)argv[3];

        if(apap10_cfg.api_key != NULL)
        {
            cm_demo_printf("apap10_cfg aplikey =%s\r\n",apap10_cfg.api_key);
        }
        else
        {
            cm_demo_printf("apap10_cfg aplikey =NULL\r\n");
            return;
        }
        
        apap10_cfg.time_out = atoi((char *)argv[4]);
        apap10_cfg.nearbts_enable = atoi((char *)argv[5]);
        apap10_cfg.digital_sign_enable = atoi((char *)argv[6]);
        
        if((apap10_cfg.digital_sign_enable == 1)&&(argc > 7))
        {
            apap10_cfg.digital_key = (uint8_t *)argv[7];
            
            if(apap10_cfg.digital_key != NULL)
            {
                cm_demo_printf("apap10_cfg digital_key =%s\r\n",apap10_cfg.digital_key);
            }
            else
            {
                cm_demo_printf("apap10_cfg digital_key =NULL\r\n");
                return;
            }
        }        
        ret = cm_lbs_init(CM_LBS_PLAT_AMAP10,&apap10_cfg);
        
        cm_demo_printf("cm_lbs_init ret=%d\r\n",ret);
        
        ret = cm_lbs_get_attr(CM_LBS_PLAT_AMAP10,&apap10_cfg_acqure);
        
        if(ret == 0)
        {
            cm_demo_printf("apap10_cfg_acqure timeout=%d\r\n",apap10_cfg_acqure.time_out);
            cm_demo_printf("apap10_cfg_acqure aplikey =%s\r\n",apap10_cfg_acqure.api_key);
        }
        else
        {
            cm_demo_printf("cm_lbs_get_attr ret=%d\r\n",ret);
        }
        
        ret = cm_lbs_location(cm_lbs_cb,(void *)cm_test_cb_arg);
        cm_demo_printf("cm_lbs_location ret=%d\r\n",ret);
    }
    else if((lbs_platform == CM_LBS_PLAT_AMAP20)&&(argc > 7))
    {
        cm_lbs_amap_location_attr_t apap20_cfg = {0};
        uint8_t aplikey[64] = {0};
        cm_lbs_amap_location_attr_t apap20_cfg_acqure = {aplikey,0};
    
        apap20_cfg.api_key = (uint8_t *)argv[3];

        if(apap20_cfg.api_key != NULL)
    {
            cm_demo_printf("config aplikey =%s\r\n",apap20_cfg.api_key);
    }
    else
    {
        cm_demo_printf("config aplikey =NULL\r\n");
        return;
    }
    
        apap20_cfg.time_out = atoi((char *)argv[4]);
        apap20_cfg.show_fields_enable = atoi((char *)argv[5]);
        apap20_cfg.nearbts_enable = atoi((char *)argv[6]);
        apap20_cfg.digital_sign_enable = atoi((char *)argv[7]);
        
        if((apap20_cfg.digital_sign_enable == 1)&&(argc > 8))
        {
            apap20_cfg.digital_key = (uint8_t *)argv[8];
            
            if(apap20_cfg.digital_key != NULL)
            {
                cm_demo_printf("apap20_cfg digital_key =%s\r\n",apap20_cfg.digital_key);
            }
            else
            {
                cm_demo_printf("apap20_cfg digital_key =NULL\r\n");
                return;
            }
        }
        
        
        ret = cm_lbs_init(CM_LBS_PLAT_AMAP20,&apap20_cfg);
        
        cm_demo_printf("cm_lbs_init ret=%d\r\n",ret);
        
        ret = cm_lbs_get_attr(CM_LBS_PLAT_AMAP20,&apap20_cfg_acqure);
    
        if(ret == 0)
        {
                cm_demo_printf("apap20_cfg_acqure timeout=%d\r\n",apap20_cfg_acqure.time_out);
                cm_demo_printf("apap20_cfg_acqure aplikey =%s\r\n",apap20_cfg_acqure.api_key);
        }
        else
        {
            cm_demo_printf("cm_lbs_get_attr ret=%d\r\n",ret);
        }
    
        ret = cm_lbs_location(cm_lbs_cb,(void *)cm_test_cb_arg);
        cm_demo_printf("cm_lbs_location ret=%d\r\n",ret);
    }
    else if((lbs_platform == CM_LBS_PLAT_ONEOSPOS)&&(argc > 5))
    {
        cm_lbs_oneospos_attr_t noeospos_cfg = {0};
        char pid[64] = {0};
        cm_lbs_oneospos_attr_t noeospos_cfg_acqure = {pid,0};
    
        noeospos_cfg.pid = (char *)argv[3];

        if(noeospos_cfg.pid != NULL)
        {
            cm_demo_printf("noeospos_cfg pid =%s\r\n",noeospos_cfg.pid);
        }
        else
        {
            cm_demo_printf("noeospos_cfg pid =NULL\r\n");
            return;
        }
    
        noeospos_cfg.time_out = atoi((char *)argv[4]);
        noeospos_cfg.nearbts_enable = atoi((char *)argv[5]);
        
        ret = cm_lbs_init(CM_LBS_PLAT_ONEOSPOS,&noeospos_cfg);
        
        cm_demo_printf("cm_lbs_init ret=%d\r\n",ret);
        
        ret = cm_lbs_get_attr(CM_LBS_PLAT_ONEOSPOS,&noeospos_cfg_acqure);
        
        if(ret == 0)
        {
            cm_demo_printf("noeospos_cfg_acqure timeout=%d\r\n",noeospos_cfg_acqure.time_out);
            cm_demo_printf("noeospos_cfg_acqure pid =%s\r\n",noeospos_cfg_acqure.pid);
        }
        else
        {
            cm_demo_printf("cm_lbs_get_attr ret=%d\r\n",ret);
        }
        
        ret = cm_lbs_location(cm_lbs_cb,(void *)cm_test_cb_arg);
        cm_demo_printf("cm_lbs_location ret=%d\r\n",ret);
    }
}
NR_SHELL_CMD_EXPORT(cm_lbs, SHELL_test_lbs_start);

#endif
#endif

