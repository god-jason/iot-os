/**
 * @file        oneos_lbs.c
 * @brief       oneos 基站定位，示例仅供参考
 * @copyright   Copyright © 2024 China Mobile IOT. All rights reserved.
 * @author      by yangqiang
 */
#ifdef CM_DEMO_LBS_SUPPORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdarg.h"
#include "cm_mem.h"
#include "cm_sys.h"
#include "cm_os.h"
#include "cm_pm.h"
#include "cm_async_dns.h"
#include "cm_lbs.h"
#include <nr_micro_shell.h>

/*测试变量区，请修改以下参数*/
#define APP_DEMO_ONEOS_LBS_PID "rypgcHHgdA" //默认为空，底层将模组内置的PID，与其他模组用户共用每天400W次调用次数，QPS 30
#define cm_log_printf(log_level, fmt, ...)  osPrintf("[LBS]" fmt "\r\n", ##__VA_ARGS__)

static int s_oneos_lbs_flag = 1;
static char s_oneos_lbs_longitude[15] = {0};  
static char s_oneos_lbs_latitude[15] = {0};

/**
 *  @brief oneos LBS回调
 *
 *  @param [out] event 回调事件ID
 *  @param [out] location 定位信息参数
 *  @param [out] cb_arg 回调参数
 *
 *  @return
 *
 *  @details 底层为asocket的回调，面向用户层开放，可以自行在各类事件中 添加自己的处理函数
 */
void __cm_oneos_lbs_cb(cm_lbs_callback_event_e event,cm_lbs_location_rsp_t *location,void *cb_arg)
{
    if(event == CM_LBS_LOCATION_OK)
    {
        strcpy(s_oneos_lbs_longitude,location->longitude);
        strcpy(s_oneos_lbs_latitude,location->latitude);   
    }
    cm_lbs_deinit();
    s_oneos_lbs_flag =  event;
}

/**
 *  @brief oneos 同步获取LBS数据
 *
 *  @param [out] longitude 经度字符串
 *  @param [out] latitude 纬度字符串
 *  @param [in] timeout_s 超时时间，单位秒，(0-60s)
 *
 *  @return
 *
 *  @details 同步阻塞获取LBS数据
 */
int cm_oneos_lbs_get_sync(char *longitude,char *latitude,int timeout_s)
{
    int wait_timeout = 0;
    cm_lbs_oneospos_attr_t oneospos_cfg = {
        .pid = APP_DEMO_ONEOS_LBS_PID,  //可为""，将使用模组内部的pid
        .nearbts_enable = 1,    //启动邻区
        .time_out = timeout_s
    };
    //1. 初始化oneos lbs定位
    int ret = cm_lbs_init(CM_LBS_PLAT_ONEOSPOS,&oneospos_cfg);
    if(ret < 0)
    {
        return ret;
    }
    cm_log_printf(0,"[1/4]cm_lbs_init ok!");
    
    //2. 设置域名解析ipv4优先（底层默认采用ipv6优先，若要测试ipv6，请检查SIM卡是否支持ipv6）
    cm_log_printf(0,"[2/4]dns priority is:%d!",cm_async_dns_get_type_by_priority());
    cm_async_dns_set_priority(0);

    //3. 注册LBS获取位置信息(异步)
    s_oneos_lbs_flag = 1;
    memset(s_oneos_lbs_longitude,0,sizeof(s_oneos_lbs_longitude));
    memset(s_oneos_lbs_latitude,0,sizeof(s_oneos_lbs_latitude));
    
    ret = cm_lbs_location(__cm_oneos_lbs_cb,NULL);
    if(ret < 0)
    {
        cm_lbs_deinit();
        return ret;
    }
    cm_log_printf(0,"[3/4]cm_lbs_location exe ok!");

    //4. 等待LBS获取位置信息的结果
    while(1)
    {
        if(s_oneos_lbs_flag == CM_LBS_LOCATION_OK)  //oneos lbs定位成功
        {
            strcpy(longitude,s_oneos_lbs_longitude);
            strcpy(latitude,s_oneos_lbs_latitude);
            cm_log_printf(0,"[4/4]oneos lbs location ok!");
            break;
        }
        else if(s_oneos_lbs_flag < 0)
        {
            return s_oneos_lbs_flag;
        }
        wait_timeout ++;
        if(wait_timeout * 100 >= (timeout_s + 3) * 1000) //超时后未产生回调
        {
            cm_lbs_deinit();
            return CM_LBS_TIMEOUT;
        }
        osDelay(20);
    }
    return CM_LBS_LOCATION_OK;
}

/**
 *  @brief oneos 同步获取LBS数据
 *
 *  @param 
 *
 *  @return
 *
 *  @details 通过oneos LBS获取经纬度
 */
void cm_oneos_lbs_demo_func()
{
    char longitude[15] = {0},latitude[15] = {0};
    int ret = cm_oneos_lbs_get_sync(longitude,latitude,20);
    if(ret == 0)
    {
        cm_log_printf(0,"get oneos lbs data success,longitude:%s,latitude:%s",longitude,latitude);
    }
    else
    {
        cm_log_printf(0,"get oneos lbs data fail,ret:%d",ret);
    }
}

/* oneos lbs测试线程 */
void cm_oneos_lbs_task_test(void *p)
{
    /* 等待模组PDP激活 */
    int32_t pdp_time_out = 0;
    /*while(1)
    {
        if(pdp_time_out > 20)
        {
            cm_log_printf(0, "network timeout\n");
            cm_pm_reboot();
        }
        if(cm_modem_get_pdp_state(1) == 1)
        {
            cm_log_printf(0, "network ready\n");
            break;
        }
        osDelay(200);
        pdp_time_out++;
    }*/

    /* 适当等待（模组PDP激活到能进行数据通信需要适当等待，或者通过PING操作确定模组数据通信是否可用） */
    osDelay(1000);

    while(1)
    {
        cm_oneos_lbs_demo_func();
        osDelay(20 * 200);
    }
}

/* opencpu程序主入口，禁止任何阻塞操作*/
int cm_opencpu_entry(void *arg)
{
    (void)arg;
    osThreadAttr_t app_task_attr = {0};
    app_task_attr.name  = "test_task";
    app_task_attr.stack_size = 1024 * 4;
    app_task_attr.priority = osPriorityNormal1;

    osThreadNew((osThreadFunc_t)cm_oneos_lbs_task_test, 0, &app_task_attr);
    return 0;
}

NR_SHELL_CMD_EXPORT(oneos_lbs, cm_opencpu_entry);
#endif

