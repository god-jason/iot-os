/**
 *  @file    cm_demo_sys.c
 *  @brief   OpenCPU sys示例
 *  @copyright copyright ? 2025 China Mobile IOT. All rights reserved.
 *  @author by cmiot0752
 *  @date 2025/03/17
 */
#ifdef CM_DEMO_SYS_SUPPORT
#ifdef OS_USING_SHELL

#include <stdio.h>
#include <string.h>
#include "cm_sys.h"
#include "cm_sim.h"

#define cm_demo_printf  osPrintf
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
void cm_test_sys(char argc, char **argv)
{
    char str[33] = {0};
    if (argv[1] != NULL && 0 == strcmp(argv[1], "imei"))
    {
        if(cm_sys_get_imei(str) < 0)
        {
            cm_demo_printf("imei read fail\n");
        }
        else
        {
            cm_demo_printf("imei is %s\n", str);
        }
        return;
    }
    else if (argv[1] != NULL && 0 == strcmp(argv[1], "sn"))
    {
        if(cm_sys_get_sn(str) < 0)
        {
            cm_demo_printf("sn read fail\n");
        }
        else
        {
            cm_demo_printf("sn is %s\n", str);
        }
        return;
    }
    else if (argv[1] != NULL && 0 == strcmp(argv[1], "cm_ver"))
    {
        char buf[CM_VER_LEN] = {0};
        if(cm_sys_get_cm_ver(buf, CM_VER_LEN) < 0)
        {
            cm_demo_printf("cm_ver read fail\n");
        }
        else
        {
            cm_demo_printf("cm_ver is %s\n", buf);
        }
        return;
    }
    else if (argv[1] != NULL && 0 == strcmp(argv[1], "iccid"))
    {
        if(cm_sim_get_iccid(str) < 0)
        {
            cm_demo_printf("iccid read fail\n");
        }
        else
        {
            cm_demo_printf("iccid is %s\n", str);
        }
        return;
    }
    else if (argv[1] != NULL && 0 == strcmp(argv[1], "imsi"))
    {
        if(cm_sim_get_imsi(str) < 0)
        {
            cm_demo_printf("imsi read fail\n");
        }
        else
        {
            cm_demo_printf("imsi is %s\n", str);
        }
        return;
    }
    else if (argv[1] != NULL && 0 == strcmp(argv[1], "cpin"))
    {
        int32_t cpin = cm_sim_get_cpin();
        if(cpin < 0)
        {
            cm_demo_printf("cpin read fail\n");
        }
        else
        {
            cm_demo_printf("cpin is %d\n", cpin);
        }
        return;
    }
    else if (argv[1] != NULL && 0 == strcmp(argv[1], "eid"))
    {
        if(cm_sim_get_eid(str) < 0)
        {
            cm_demo_printf("eid read fail\n");
        }
        else
        {
            cm_demo_printf("eid is %s\n", str);
        }
        return;
    }
    else if (argv[1] != NULL && 0 == strcmp(argv[1], "get_switch"))
    {
        cm_sim_switch_t mode = {0};
        if(cm_sim_get_switch(&mode) < 0)
        {
            cm_demo_printf("get switch fail\n");
        }
        else
        {
            cm_demo_printf("sim0_enable[%d],sim0_hotswap_enable[%d],sim1_enable[%d],sim1_hotswap_enable[%d]\n",
                mode.sim0_enable, mode.sim0_hotswap_enable, mode.sim1_enable, mode.sim1_hotswap_enable);
        }
        return;
    }
    else if (argc == 6 && 0 == strcmp(argv[1], "set_switch"))
    {
        cm_sim_switch_t mode_set = {0};
        mode_set.sim0_enable = atoi(argv[2]);
        mode_set.sim0_hotswap_enable = atoi(argv[3]);
        mode_set.sim1_enable = atoi(argv[4]);
        mode_set.sim1_hotswap_enable = atoi(argv[5]);
        if(cm_sim_set_switch(mode_set) < 0)
        {
            cm_demo_printf("set switch fail\n");
        }
        else
        {
            cm_demo_printf("set switch succ,reboot to check\n");
        }
        return;
    }
    cm_demo_printf("Usage:\n");
    cm_demo_printf("  sys imei          - get imei          \n");
    cm_demo_printf("  sys sn            - get sn            \n");
    cm_demo_printf("  sys cm_ver        - get sdk version   \n");
    cm_demo_printf("  sys iccid         - get iccid         \n");
    cm_demo_printf("  sys imsi          - get imsi          \n");
    cm_demo_printf("  sys cpin          - get cpin state    \n");
    cm_demo_printf("  sys eid           - get eid           \n");
    cm_demo_printf("  sys get_switch    - get sim and hotplug state\n");
    cm_demo_printf("  sys set_switch sim0_enable sim0_hotswap_enable sim1_enable sim1_hotswap_enable \n");
}
#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(sys, cm_test_sys);
#endif
#endif

