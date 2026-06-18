/*********************************************************
 *  @file    cm_demo_powerkey.c
 *  @brief   OpenCPU powerkey示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by zyf 2021/08/30
 ********************************************************/
#include "stdlib.h"
#include "cm_sys.h"
#include "cm_powerkey.h"
#include "cm_demo_powerkey.h"
#include "cm_os.h"
#include "cm_demo_uart.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/
static int parse_params(const char *args, int param_num, const char *param[])
{
	int i = 0;
    for(i = 0; i < param_num; i++)
    {
        param[i] = embeddedCliGetToken(args, i + 2);
        if (param[i] == NULL)
        {
            cm_demo_printf("invalid param[%d]\n", i);
            return -1;
        }
    }
    return 0;
}

void cm_pm_powerkey_cb(cm_powerkey_event_e event, void *param)
{
    if(CM_POWERKEY_EVENT_RELEASE == event)
    {
        cm_demo_printf("[PM]powerkey CM_POWERKEY_EVENT_RELEASE\n");
    }
    else
    {
        cm_demo_printf("[PM]powerkey CM_POWERKEY_EVENT_PRESS\n");
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
void cm_test_powerkey(EmbeddedCli *cli, char *args, void *context)
{
    uint8_t mode = 0;
    const char *param[1] = {NULL};
    int param_num = 1;
    
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }

    if (strncmp(cmd, STR_ITEM("start")) == 0)
    {
        if (parse_params(args, param_num, param) != 0)
        {
            return;
        }

        mode = atoi((char *)param[0]);
        
        if(mode == 0)
        {
            cm_pm_powerkey_callback(NULL, NULL);
        }
        else
        {
            cm_pm_powerkey_callback(cm_pm_powerkey_cb, NULL);
        }
    }
    else
    {
        cm_demo_printf("invalid cmd=%s error\n", cmd);
    }
}
