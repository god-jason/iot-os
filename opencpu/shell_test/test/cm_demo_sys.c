/**
 *  @file    cm_demo_sys.c
 *  @brief   OpenCPU sys示例
 *  @copyright copyright ? 2025 China Mobile IOT. All rights reserved.
 *  @author by cmiot0752
 *  @date 2025/03/17
 */


#include <stdio.h>
#include <string.h>
#include "cm_sys.h"
#include "cm_sim.h"

/**
 *  @brief SYS测试
 *
 *  @param [in] cli cli句柄
 *  @param [in] args 参数列表
 *  @param [in] context 用户参数
 *  @return None
 *
 *  @details
 */
void cm_test_sys(EmbeddedCli *cli, char *args, void *context)
{
    char str[33] = {0};
    const char *cmd = embeddedCliGetToken(args, 1);
    const char *data = embeddedCliGetToken(args, 2);

    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }

    if (strcasecmp(cmd, "imei") == 0)
    {
        if(cm_sys_get_imei(str) < 0)
        {
            cm_demo_printf("imei read fail\n");
        }
        else
        {
            cm_demo_printf("imei is %s\n", str);
        }
    }
    else if (strcasecmp(cmd, "sn") == 0)
    {
        if(cm_sys_get_sn(str) < 0)
        {
            cm_demo_printf("sn read fail\n");
        }
        else
        {
            cm_demo_printf("sn is %s\n", str);
        }
    }
    else if (strcasecmp(cmd, "iccid") == 0)
    {
        if(cm_sim_get_iccid(str) < 0)
        {
            cm_demo_printf("iccid read fail\n");
        }
        else
        {
            cm_demo_printf("iccid is %s\n", str);
        }
    }
    else if (strcasecmp(cmd, "imsi") == 0)
    {
        if(cm_sim_get_imsi(str) < 0)
        {
            cm_demo_printf("imsi read fail\n");
        }
        else
        {
            cm_demo_printf("imsi is %s\n", str);
        }
    }
    else if (strcasecmp(cmd, "hww") == 0)
    {
        if(cm_sys_set_hw_ver((void *)data) < 0)
        {
            cm_demo_printf("hww read fail\n");
        }
        else
        {
            cm_demo_printf("hww is %s\n", str);
        }
    }
    else if (strcasecmp(cmd, "hwr") == 0)
    {
        if(cm_sys_get_hw_ver(str,30) < 0)
        {
            cm_demo_printf("hwr read fail\n");
        }
        else
        {
            cm_demo_printf("hwr is %s\n", str);
        }
    }

    else
    {
        cm_demo_printf("sys cmd error\n");
    }

}

