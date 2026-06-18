/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_WIFISCAN_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>
#include "cm_wifiscan.h"

#define cm_demo_printf osPrintf

#define STR_LEN(s) (sizeof(s) - 1)
#define STR_ITEM(s) (s), STR_LEN(s)

void __NumToHex(char *inData, int inLen, char *outData)
{
    int i=0;
    char tmpBuf[10];

    memset(tmpBuf, 0, sizeof(tmpBuf));

    if (inLen > 0)
    {
        for (i = 0; i < inLen; i++)
        {
            sprintf(tmpBuf, "%02X", inData[i]);
            strcat(outData, tmpBuf);
        }
    }
}
static void __wifiscan_callback_demo(cm_wifi_scan_info_t *param, void *user_param)
{
    cm_demo_printf("__wifiscan_callback_demo %s\n", user_param);
    char macBuf[20];

    for (int i = 0; i < param->bssid_number; i++)
    {
        memset(macBuf, 0, sizeof(macBuf));
        __NumToHex((char*)param->channel_cell_list[i].bssid, sizeof(param->channel_cell_list[i].bssid), macBuf);
        cm_demo_printf("%d, %s, %d, %d\n", i+1, macBuf, param->channel_cell_list[i].rssi, param->channel_cell_list[i].channel_number);
    }
}

static void __wifiscan_callback_demo1(cm_wifi_scan_info_t *param, void *user_param)
{
    cm_demo_printf("__wifiscan_callback_demo1: %d\n",param->bssid_number);
}

void test_round(void)
{
    uint8_t round = 3;

    //test1：round设置正常边界值，设置返回成功
    if(cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_ROUND, &round))
    {
        cm_demo_printf("test_round_1 fail\n");
    }

    //test2：round设置低于边界值异常值，设置返回失败
    round = 0;
    if(0 == cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_ROUND, &round))
    {
        cm_demo_printf("test_round_2 fail\n");
    }
    //test3：round设置高于边界异常值，设置返回失败
    round = 4;
    if(0 == cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_ROUND, &round))
    {
        cm_demo_printf("test_round_3 fail\n");
    }
}

void test_maxcount(void)
{
    uint8_t max = 5;

    //test1：maxcount设置正常边界值，设置返回成功
    if(cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_MAX_COUNT, &max))
    {
        cm_demo_printf("test_maxcount_1 fail\n");
    }

    //test2：maxcount设置低于边界值异常值，设置返回失败
    max = 0;
    if(0 == cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_MAX_COUNT, &max))
    {
        cm_demo_printf("test_maxcount_2 fail\n");
    }
    //test3：round设置高于边界异常值，设置返回失败
    max = 11;
    if(0 == cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_MAX_COUNT, &max))
    {
        cm_demo_printf("test_maxcount_3 fail\n");
    }
}

void test_timeout(void)
{
    uint8_t time_out = 10;

    //test1：timeout设置正常边界值，设置返回成功
    if(cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_TIMEOUT, &time_out))
    {
        cm_demo_printf("test_timeout_1 fail\n");
    }

    //test2：timeout设置低于边界值异常值，设置返回失败
    time_out = 1;
    if(0 == cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_TIMEOUT, &time_out))
    {
        cm_demo_printf("test_timeout_2 fail\n");
    }
}

void test_priority(void)
{
    uint8_t priority = CM_WIFI_SCAN_WIFI_HIGH;

    //test1：priority设置正常边界值，设置返回成功
    if(cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_PRIORITY, &priority))
    {
        cm_demo_printf("test_priority_1 fail\n");
    }

    //test2：priority设置高于于边界值异常值，设置返回失败
    priority = 3;
    if(0 == cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_PRIORITY, &priority))
    {
        cm_demo_printf("test_priority_2 fail\n");
    }
}

void test_stop(void)
{
    uint8_t round = 3;
    uint8_t max = 1;
    uint8_t time_out = 2;
    uint8_t priority = CM_WIFI_SCAN_WIFI_HIGH;
    cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_ROUND, &round);
    cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_MAX_COUNT, &max);
    cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_TIMEOUT, &time_out);
    cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_PRIORITY, &priority);

    if(cm_wifiscan_start((cm_wifiscan_callback_t)__wifiscan_callback_demo1, NULL))
    {
        cm_demo_printf("test_start_2 fail\n");
    }
    if(cm_wifiscan_stop())
    {
        cm_demo_printf("test_stop_1 fail\n");
    }
}

void test_query(void)
{
    cm_wifi_scan_info_t *param1 = (cm_wifi_scan_info_t *)osMalloc(sizeof(cm_wifi_scan_info_t));
    if(cm_wifiscan_query(NULL)==0)
    {
        cm_demo_printf("test_query_1 fail\n");
    }
    cm_demo_printf("9999\n");
    if(cm_wifiscan_query(&param1))
    {
        cm_demo_printf("test_query_2 fail\n");
    }
    else

    {
        char macBuf[20];
        for(int i = 0; i < param1->bssid_number; i++)
        {
            memset(macBuf,0,sizeof(macBuf));
            __NumToHex((char*)param1->channel_cell_list[i].bssid, sizeof(param1->channel_cell_list[i].bssid), macBuf);
            cm_demo_printf("%d, %s, %d, %d\n", i+1, macBuf, param1->channel_cell_list[i].rssi, param1->channel_cell_list[i].channel_number);
        }
    }
    osFree(param1);
    cm_demo_printf("SHELL_testWifiscan end\n");
}

/* 执行测试命令 */
void SHELL_testWifiscan(char argc, char **argv)
{
    const char *sub_cmd = argv[1];

    /* CM:WIFISCAN:CFG */
    /* 测试需首先进行参数配置 */
    if (strncmp(sub_cmd, STR_ITEM("CFG")) == 0)
    {
        test_round();
        test_maxcount();
        test_timeout();
        test_priority();
    }

    /* CM:WIFISCAN:START */
    /* 开始扫描，扫描结果将从回调函数异步上报，切换CFUN过程中禁止扫描WiFi */
    else if (strncmp(sub_cmd, STR_ITEM("START")) == 0)
    {
        cm_demo_printf("wifiscan start\n");
        if(cm_wifiscan_start(NULL, "wifiscan") == 0)
        {
            cm_demo_printf("test_start_NULL fail\n");
        }
        uint8_t max = 2;
        uint8_t time_out = 60;
        cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_MAX_COUNT, &max);
        cm_wifiscan_cfg(CM_WIFI_SCAN_CFG_TIMEOUT, &time_out);
        if(cm_wifiscan_start((cm_wifiscan_callback_t)__wifiscan_callback_demo, NULL))
        {
            cm_demo_printf("wifiscan start err\n");
        }
        else
        {
            cm_demo_printf("wifiscan start success\n");
        }
    }

    /* CM:WIFISCAN:STOP */
    /* 停止扫描，超时时间未到或结果未完全返回时，可中断wifiscan功能 */
    else if (strncmp(sub_cmd, STR_ITEM("STOP")) == 0)
    {
        test_stop();
    }

    /* CM:WIFISCAN:QUERY */
    /* 查询扫描结果，查询返回的是最后一次扫描的结果 */
    else if (strncmp(sub_cmd, STR_ITEM("QUERY")) == 0)
    {
        test_query();
    }
    else
    {
        cm_demo_printf("-----------------cm_wifiscan help---------------------\r\n");
        cm_demo_printf("1 cm_wifiscan CFG\r\n");
        cm_demo_printf("2 cm_wifiscan START\r\n");
        cm_demo_printf("3 cm_wifiscan STOP\r\n");
        cm_demo_printf("3 cm_wifiscan QUERY\r\n");
        cm_demo_printf("-----------------cm_wifiscan--------------------------\r\n");
    }

}

NR_SHELL_CMD_EXPORT(cm_wifiscan, SHELL_testWifiscan);

#endif
#endif
