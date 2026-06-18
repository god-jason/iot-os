/*********************************************************
 *  @file    cm_demo_wifiscan.c
 *  @brief   OpenCPU wifiscan示例
 *  Copyright (c) 2023 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot1325 2023/6/29
 ********************************************************/
#ifdef CM_DEMO_WIFISCAN_SUPPORT
#ifdef OS_USING_SHELL

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cm_wifiscan.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define cm_demo_printf osPrintf

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

static void __NumToHex(char *inData, int inLen, char *outData)
{
    int i=0;
    char tmpBuf[10];

    memset(tmpBuf, 0, sizeof(tmpBuf));

    if (inLen > 0)
    {
        for (i = 0; i < inLen; i++)
        {
            sprintf(tmpBuf, "%02X", (uint8_t)inData[i]);
            strcat(outData, tmpBuf);
        }
    }
}

static void __wifiscan_callback_demo(cm_wifi_scan_info_t *param, void *user_param)
{
    cm_demo_printf("__wifiscan_callback_demo %s\n", user_param);
    char macBuf[50];
    int i = 0;

    for (i = 0; i < param->bssid_number; i++)
    {
        memset(macBuf, 0, sizeof(macBuf));
        __NumToHex((char*)param->channel_cell_list[i].bssid, sizeof(param->channel_cell_list[i].bssid), macBuf);
        cm_demo_printf("%d, %s, %d, %d\n", i+1, macBuf, param->channel_cell_list[i].rssi, param->channel_cell_list[i].channel_number);
    }
}

/* 执行测试命令 */
void cm_test_wifiscan(char argc, char **argv)
{
    if (argv[1] != NULL)
    {
        const char *cmd = argv[1];
        /* wifiscan cfg */
        /* 测试需首先进行参数配置 */
        if (strncmp(cmd, STR_ITEM("cfg")) == 0)
        {
            uint8_t round = 3;
            uint8_t max = 10;
            uint8_t time_out = 30;
            uint8_t priority = CM_WIFI_SCAN_WIFI_HIGH;

            if (cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_ROUND, &round))
            {
                cm_demo_printf("wifiscan cfg round err\n");
            }

            if (cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_MAX_COUNT, &max))
            {
                cm_demo_printf("wifiscan cfg max err\n");
            }

            if (cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_TIMEOUT, &time_out))
            {
                cm_demo_printf("wifiscan cfg time_out err\n");
            }

            if (cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_PRIORITY, &priority))
            {
                cm_demo_printf("wifiscan cfg priority err\n");
            }
            return;
        }

        /* wifiscan start */
        /* 开始扫描，扫描结果将从回调函数异步上报，切换CFUN过程中禁止扫描WiFi */
        else if (strncmp(cmd, STR_ITEM("start")) == 0)
        {
            if(cm_wifiscan_start((cm_wifiscan_callback_t)__wifiscan_callback_demo, "wifiscan"))
            {
                cm_demo_printf("wifiscan start err\n");
            }
            return;
        }

        /* wifiscan stop */
        /* 停止扫描，超时时间未到或结果未完全返回时，可中断wifiscan功能 */
        else if (strncmp(cmd, STR_ITEM("stop")) == 0)
        {
            if(cm_wifiscan_stop())
            {
                cm_demo_printf("wifiscan stop err\n");
            }
            return;
        }

        /* wifiscan query */
        /* 查询扫描结果，查询返回的是最后一次扫描的结果 */
        else if (strncmp(cmd, STR_ITEM("query")) == 0)
        {
            cm_wifi_scan_info_t *param = NULL;
            if(cm_wifiscan_query(&param))
            {
                cm_demo_printf("wifiscan query err\n");
                return;
            }

            char macBuf[50];
            int i = 0;

            for(i = 0; i < param->bssid_number; i++)
            {
                memset(macBuf,0,sizeof(macBuf));
                __NumToHex((char*)param->channel_cell_list[i].bssid, sizeof(param->channel_cell_list[i].bssid), macBuf);
                cm_demo_printf("%d, %s, %d, %d\n", i+1, macBuf, param->channel_cell_list[i].rssi, param->channel_cell_list[i].channel_number);
            }
            return;
        }
    }
    cm_demo_printf("Usage:\n");
    cm_demo_printf("  wifiscan cfg         - wifiscan param cfg test    \n");
    cm_demo_printf("  wifiscan start       - wifiscan start test        \n");
    cm_demo_printf("  wifiscan stop        - wifiscan stop test         \n");
    cm_demo_printf("  wifiscan query       - wifiscan result query      \n");
}

#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(wifiscan, cm_test_wifiscan);

#endif
#endif
