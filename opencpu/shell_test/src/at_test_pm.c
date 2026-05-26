#ifdef CM_DEMO_PM_SUPPORT

#include <os.h>
#include <stdlib.h>

#include "at_api.h"
#include "at_parser.h"
#include "cm_pm.h"
#include "cm_mem.h"
#include "cm_demo_common.h"

typedef AT_CommandItem at_cmd_t;


#define FTP_LOG(fmt, ...) osPrintf("[PM][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define FTP_CMD_STR_LEN 512

#define CM_FTP_RET_CODE_OK 0 //成功
#define CM_FTP_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_FTP_RET_CODE_PARAM_ERROR 50 //参数错误 50

// AT+PM="cm_pm_poweroff"
int func_cm_pm_poweroff(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    cm_pm_poweroff();
    return 0;
}

// AT+PM="cm_pm_get_power_on_reason"
int func_cm_pm_get_power_on_reason(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;
    handle = cm_pm_get_power_on_reason();
    cm_uart_printf_urc("+PMGETPWRONREASON: %d", handle);
    return 0;
}


// AT+PM="cm_pm_reboot"
int func_cm_pm_reboot(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    cm_pm_reboot();
    return 0;
}

void pm_EventCb(void)
{
    osPrintf("Entering PSM function\n");
}

void pm_ExitCb(uint32_t reason)
{
    osPrintf("Exiting PSM function, reason %d\n",reason);
}

// AT+PM="cm_pm_init"
int func_cm_pm_init(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    cm_pm_cfg_t pm_cfg = {0};
    pm_cfg.cb_enter = pm_EventCb;
    pm_cfg.cb_exit = pm_ExitCb;

    cm_pm_init(pm_cfg);
    return 0;
}

// AT+PM="cm_pm_work_lock"
int func_cm_pm_work_lock(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    cm_pm_work_lock();
    return 0;
}

// AT+PM="cm_pm_work_unlock"
int func_cm_pm_work_unlock(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    cm_pm_work_unlock();
    return 0;
}

static void cm_pm_powerkey_callback(cm_powerkey_event_e event)
{
    if(CM_POWERKEY_EVENT_RELEASE == event)
    {
        cm_uart_printf_urc("[PM]powerkey CM_POWERKEY_EVENT_RELEASE\n");
    }
    else
    {
        cm_uart_printf_urc("[PM]powerkey CM_POWERKEY_EVENT_PRESS\n");
    }
}
// AT+PM="cm_pm_powerkey_regist_callback"
int func_cm_pm_powerkey_regist_callback(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int cb_null = 0;
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &cb_null))
    {
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }
    if(cb_null == 0)
        return cm_pm_powerkey_regist_callback(cm_pm_powerkey_callback);
    else
        return cm_pm_powerkey_regist_callback(NULL);
}

// AT+PM="XX", param1, param2, ...
// "XX" 为函数名
void pm_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;

    FTP_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[35] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 35))
        {
            ret = CM_FTP_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_pm_poweroff") == 0)
        {
            ret = func_cm_pm_poweroff(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_pm_get_power_on_reason") == 0)
        {
            ret = func_cm_pm_get_power_on_reason(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_pm_reboot") == 0)
        {
            ret = func_cm_pm_reboot(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_pm_init") == 0)
        {
            ret = func_cm_pm_init(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_pm_work_lock") == 0)
        {
            ret = func_cm_pm_work_lock(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_pm_work_unlock") == 0)
        {
            ret = func_cm_pm_work_unlock(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_pm_powerkey_regist_callback") == 0)
        {
            ret = func_cm_pm_powerkey_regist_callback(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            FTP_LOG("Error func name[%s]", func_name);
            ret = CM_FTP_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}


AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_pm_oc[] =
{
    {.name = "+PM",   .setFunc = pm_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

#endif


