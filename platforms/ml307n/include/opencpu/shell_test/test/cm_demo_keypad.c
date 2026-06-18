/*********************************************************
 *  @file    cm_demo_keypad.c
 *  @brief   OpenCPU KEYPAD示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by CLS 2024/12/16
 ********************************************************/
#ifdef CM_DEMO_KEYPAD_SUPPORT
#ifdef OS_USING_SHELL

#include <stdint.h>
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>
#include "cm_iomux_v2.h"
#include "cm_keypad.h"
#include "cm_gpio.h"
#include "cm_mem.h"
#include "pinmap.h"
#include <cmsis_os2.h>
#include "cm_pm.h"


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define     CM_DEMO_KPD_LOG                 osPrintf
#define     WKP_SOURCE_KEYPAD                   1<<21
#define     OPENCPU_TEST_KEYIN_PIN          CM_IOMUX_PIN_28
#define     OPENCPU_TEST_KEYOUT_PIN         CM_IOMUX_PIN_29
#define     OPENCPU_TEST_KEYPAD_FUNCTION    CM_IOMUX_FUNC_FUNCTION0
#define     OPENCPU_TEST_KEYIN_IOMUX        CM_IOMUX_PIN_81, CM_IOMUX_FUNC_FUNCTION2
#define     OPENCPU_TEST_KEYOUT_IOMUX       CM_IOMUX_PIN_29, CM_IOMUX_FUNC_FUNCTION3




/****************************************************************************
 * Private Types
 ****************************************************************************/
static KPD_Handle g_KPDHandle[1];

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/


KPD_Handle KPD_Test = {
    .pRes = (void *)DRV_RES(KEY, 0),
    .userData = 0,
};

static void kpd_Callback(void* kpd, uint32_t event)
{
    kpd_key_item item;
    uint32_t ret = 0;
    ret = KPD_KeyStatus(kpd, &item, sizeof(kpd_key_item));
    osPrintf("kpd_Callback ret = %d \r\n", ret);
}

/****************************************************************************
 * Private Data
 ****************************************************************************/
static uint8_t cm_keypad_sleep_flag = 0;

/****************************************************************************
 * Private Functions
 ****************************************************************************/
static void cm_keypad_callback(cm_keypad_map_t key, cm_keypad_event_e event)
{
    if(cm_keypad_sleep_flag == 1)
    {
        cm_keypad_sleep(0);//按键回调函数执行时，按键才退出休眠
        cm_keypad_sleep_flag = 0;
    }
    osPrintf("cm_keypad_callback key:%d event:%d\r\n", key, event);
}

static void cm_keypad_pm_enter(void)
{
    cm_keypad_sleep(1);//切换按键中断为休眠中断
    cm_keypad_sleep_flag = 1;
    osPrintf("cm_keypad_pm_enter\r\n");
}

static void cm_keypad_pm_exit(uint32_t reason)
{
    if(!(reason & WKP_SOURCE_KEYPAD))
    {
        ;
    }
}

static void pm_event_cb(void)
{
    osPrintf("Entering pmeventcb function\n");
}

static void pm_exit_cb(uint32_t reason)
{
    osPrintf("exiting pm function, reason %d\n",reason);
}

static void pm_sleep_cb(cm_pm_sleep_status_e status, cm_pm_wakeup_source_e source)
{
    osPrintf("pm_sleep_cb status %d, source %d\n", status, source);
}

/****************************************************************************
 * Public Functions
****************************************************************************/
void SHELL_testKpd(char argc, char **argv)
{
    int32_t i, ret = -1;
    cm_pm_cfg_t test_pm_cfg;
    test_pm_cfg.cb_enter = pm_event_cb;
    test_pm_cfg.cb_exit  = pm_exit_cb;
    cm_pm_sleep_mode_t pmmode = {CM_PM_SLEEP_MODE_ACTIVE, false}, pmmode_tmp;
    cm_pm_sleepind_cb sleepind_cb_tmp;
    int32_t *pm_mode;

    cm_keypad_cfg_t cm_keypad_cfg = {0};
    CM_DEMO_KPD_LOG(" keypad test start \r\n");

    cm_keypad_cfg.cm_col[0] = CM_KP_MKI2;
    cm_keypad_cfg.cm_col[1] = CM_KP_MKI3;
    cm_keypad_cfg.cm_row[0] = CM_KP_MKO2;
    cm_keypad_cfg.cm_row[1] = CM_KP_MKO3;
    cm_keypad_cfg.cm_col_len = 2;
    cm_keypad_cfg.cm_row_len = 2;

    ret = cm_keypad_register(cm_keypad_callback);
    if (ret != DRV_OK) {
        CM_DEMO_KPD_LOG(" cm_keypad_register fail. ret = %d\r\n" ,ret);
        return ret;
    }

    cm_iomux_set_pin_func(CM_IOMUX_PIN_22, CM_IOMUX_FUNC_FUNCTION1);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_23, CM_IOMUX_FUNC_FUNCTION2);
    cm_iomux_set_pin_cmd(CM_IOMUX_PIN_22,CM_IOMUX_PINCMD3_PULL, PULL_UP);
    cm_iomux_set_pin_cmd(CM_IOMUX_PIN_23, CM_IOMUX_PINCMD3_PULL,PULL_UP);

    cm_iomux_set_pin_func(CM_IOMUX_PIN_38, CM_IOMUX_FUNC_FUNCTION2);
    //cm_iomux_set_pin_func(CM_IOMUX_PIN_55, CM_IOMUX_FUNC_FUNCTION3);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_54, CM_IOMUX_FUNC_FUNCTION3);
    //cm_test_pm_cb_register(0,cm_keypad_pm_enter,cm_keypad_pm_exit);//需要在初始化cm_test_pm_init函数之后执行，否则会被清除

    ret = cm_keypad_config(&cm_keypad_cfg);
    if (ret != DRV_OK) {
        CM_DEMO_KPD_LOG(" cm_keypad_config fail. ret = %d\r\n" ,ret);
        return ret;
    }

    ret = cm_keypad_init();
    if (ret != 0)
    {
         CM_DEMO_KPD_LOG("cm_keypad_init failed \r\n");
         return -1;
    }

    cm_pm_init(test_pm_cfg);       //初始化休眠锁
    //cm_pm_work_lock();  //加低功耗锁不进入休眠态，目前8620硬件不支持中断唤醒功能
    cm_pm_set_cfg(CM_PM_CFG_SLEEPMODE, &pmmode);
    cm_pm_get_cfg(CM_PM_CFG_SLEEPMODE, &pmmode_tmp);
    if ((pmmode_tmp.mode != pmmode.mode) || (pmmode_tmp.permanent != pmmode.permanent))
    {
         CM_DEMO_KPD_LOG("cm_keypad_init set CM_PM_CFG_SLEEPMODE failed \r\n");
         return -1;
    }

    cm_pm_set_cfg(CM_PM_CFG_SLEEPIND, pm_sleep_cb);
    cm_pm_get_cfg(CM_PM_CFG_SLEEPIND, &sleepind_cb_tmp);
    if (sleepind_cb_tmp != pm_sleep_cb)
    {
         CM_DEMO_KPD_LOG("cm_keypad_init set CM_PM_CFG_SLEEPIND failed \r\n");
         return -1;
    }

    ret = cm_pm_set_mode(CM_PM_SLEEP_MODE_DEEP);
    cm_pm_get_mode(&pm_mode);
    if (pm_mode != CM_PM_SLEEP_MODE_DEEP || ret != 0)
    {
         CM_DEMO_KPD_LOG("cm_keypad_init cm_pm_set_mode failed \r\n");
         return -1;
    }

    CM_DEMO_KPD_LOG(" keypad test end \r\n");
    for(i = CM_PM_WORK_LOCK_TYPE_SYSNETWORK; i <= CM_PM_WORK_LOCK_TYPE_USER1; i++)
    {
        cm_pm_work_lock_with_type(i);
    }
    CM_DEMO_KPD_LOG(" work lock map 0x%x \r\n", cm_pm_work_locks_query());
    for(i = CM_PM_WORK_LOCK_TYPE_SYSNETWORK; i <= CM_PM_WORK_LOCK_TYPE_USER1; i++)
    {
        cm_pm_work_unlock_with_type(i);
    }
    CM_DEMO_KPD_LOG(" work lock map 0x%x \r\n", cm_pm_work_locks_query());

    return;

}

NR_SHELL_CMD_EXPORT(cm_keypad, SHELL_testKpd);

void SHELL_test_deinit_kpd(char argc, char **argv)
{
    cm_keypad_unregister(cm_keypad_callback);
    cm_keypad_deinit();
    //cm_pm_work_unlock();

    CM_DEMO_KPD_LOG(" SHELL_test_deinit_kpd end \r\n");
    return;
}
NR_SHELL_CMD_EXPORT(cm_de_keypad, SHELL_test_deinit_kpd);


#endif
#endif

