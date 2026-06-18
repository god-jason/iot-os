/*********************************************************
 *  @file    cm_demo_pm.c
 *  @brief   OpenCPU PM示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by thf 2021/08/30
 ********************************************************/
#ifdef CM_DEMO_PM_SUPPORT
#ifdef OS_USING_SHELL

#include "stdlib.h"
#include "cm_pm.h"
#include "cmsis_os2.h"
#include <nr_micro_shell.h>

#define cm_demo_printf osPrintf
void pmEventCb(void)
{
    cm_demo_printf("Entering pmeventcb function\n");
}

void pmExitCb(uint32_t reason)
{
    cm_demo_printf("exiting pm function, reason %d\n",reason);
}

void cm_test_pm_init(void)
{
    cm_pm_cfg_t test_pm_cfg;
    test_pm_cfg.cb_enter = pmEventCb;
    test_pm_cfg.cb_exit  = pmExitCb;

	cm_pm_init(test_pm_cfg);
}

void SHELL_test_pm_reboot(char argc, char **argv)
{
    if(argv == NULL)
    {
        cm_demo_printf(" argv = NULL error!\n");
        return;
    }

    cm_demo_printf("[REBOOT]rebooting...\n");
    cm_pm_reboot_v2(0);
}

void SHELL_test_pm_poweroff(char argc, char **argv)
{
    if(argv == NULL)
    {
        cm_demo_printf(" argv = NULL error!\n");
        return;
    }

    cm_demo_printf("[POWEROFF]poweroff...\n");
    os_task_msleep(200);
    cm_pm_poweroff();
}

void SHELL_test_pm_power_on_reason(char argc, char **argv)
{
    if(argv == NULL)
    {
        cm_demo_printf(" argv = NULL error!\n");
        return;
    }

    cm_demo_printf("[PM]power on reason:%d\n", cm_pm_get_power_on_reason());
}

void SHELL_test_sleep_or_wakeup_enable(char argc, char **argv)
{
    uint8_t mode = 0;

    if(argv == NULL || argc < 2)
    {
        cm_demo_printf(" argv = NULL error!\n");
        return;
    }
    mode = atoi((char *)argv[1]);
    cm_test_pm_init();

    if(mode == 1)
    {
        cm_pm_work_unlock();
    }
    else if(mode == 2)
    {
        cm_pm_work_lock();
    }
    else
    {
        cm_demo_printf("[PM] sleep wakeup mode(%d) is error...\n",mode);
    }
}

void cm_pm_powerkey_callback(cm_powerkey_event_e event)
{
    if(CM_POWERKEY_EVENT_RELEASE == event)
    {
        cm_demo_printf("[PM]powerkey CM_POWERKEY_EVENT_RELEASE\n");
    }
    else if(CM_POWERKEY_EVENT_SHORTPRESS == event)
    {
        cm_demo_printf("[PM]powerkey CM_POWERKEY_EVENT_SHORTPRESS\n");
    }
    else if(CM_POWERKEY_EVENT_LONGPRESS == event)
    {
        cm_demo_printf("[PM]powerkey CM_POWERKEY_EVENT_LONGPRESS\n");
    }
    else
    {
        cm_demo_printf("[PM]powerkey CM_POWERKEY_EVENT_PRESS\n");
    }
}

void SHELL_test_pm_powerkey(char argc, char **argv)
{
    uint8_t mode = 0;

    if(argv == NULL || argc < 2)
    {
        cm_demo_printf(" argv = NULL error!\n");
        return;
    }

    mode = atoi((char *)argv[1]);

    if(mode == 0)
    {
        cm_pm_powerkey_regist_callback(NULL);
    }
    else
    {
        cm_pm_powerkey_regist_callback(cm_pm_powerkey_callback);
    }
}

NR_SHELL_CMD_EXPORT(cm_reboot, SHELL_test_pm_reboot);
NR_SHELL_CMD_EXPORT(cm_poweroff, SHELL_test_pm_poweroff);
NR_SHELL_CMD_EXPORT(cm_poweron, SHELL_test_pm_power_on_reason);
NR_SHELL_CMD_EXPORT(cm_sleep, SHELL_test_sleep_or_wakeup_enable);
NR_SHELL_CMD_EXPORT(cm_pwrkey, SHELL_test_pm_powerkey);



#endif
#endif


