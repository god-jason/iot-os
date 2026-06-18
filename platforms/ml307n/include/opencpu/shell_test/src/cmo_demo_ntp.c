/**
 *  @file    cm_demo_ntp.c
 *  @brief   OpenCPU NTP相关测试例程
 *  @copyright copyright © 2022 China Mobile IOT. All rights reserved.
 *  @author by THF
 *  @date 2022/06/02
 */



 /****************************************************************************
 * Included Files
 ****************************************************************************/
#ifdef CM_DEMO_NTP_SUPPORT
#ifdef OS_USING_SHELL

#include <stdio.h>
#include <string.h>
#include "cm_ntp.h"

#define cm_demo_printf osPrintf
#define NTP_TIMEOUT 6000

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
static void _cm_ntp_cb(cm_ntp_event_e event, void *event_param, void *cb_param)
{
    if (CM_NTP_EVENT_SYNC_OK == event)
    {
        cm_demo_printf("[NTP][%s] get net time %s\n", (char *)cb_param, event_param);
    }
    else if (CM_NTP_EVENT_SETTIME_FAIL == event)
    {
        cm_demo_printf("[NTP][%s] get net time %s OK, but set time fail\n", (char *)cb_param, event_param);
    }
    else
    {
        cm_demo_printf("[NTP][%s] error\n", (char *)cb_param);
    }
}

/****************************************************************************
 * Private Data
 ****************************************************************************/


/****************************************************************************
 * Private Functions
 ****************************************************************************/



/**
*  UART口NTP功能功能调试使用示例
*  CM:NTP:GETNETTIME                           //仅获取网络时间，不同步至本地时间
*  CM:NTP:SYNCHTIME                            //获取网络时间并将网络时间与本地时间同步
*/
void test_1()
{
    uint16_t port = 123;
    uint32_t timeout = NTP_TIMEOUT;
    uint32_t dns_priority = 1;
    bool set_rtc = 0;

    if (0 == cm_ntp_set_cfg(CM_NTP_CFG_SERVER, "ntp1.aliyun.com"))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SERVER success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SERVER fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_PORT, &port))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_PORT success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_PORT fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_TIMEOUT, &timeout))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_TIMEOUT success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_TIMEOUT fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_DNS, &dns_priority))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_DNS success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_DNS fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_SET_RTC, &set_rtc))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SET_RTC success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SET_RTC fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_CB, _cm_ntp_cb))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_CB_PARAM, "SYNCHTIME"))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB_PARAM success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB_PARAM fail\n");
        return;
    }


    if (0 != cm_ntp_sync())
    {
        cm_demo_printf("[NTP]cm_ntp_sync() fail\n");
    }
}

void test_2()
{
    uint16_t port = 123;
    uint32_t timeout = NTP_TIMEOUT;
    uint32_t dns_priority = 1;
    bool set_rtc = 0;

    set_rtc = 1;

    if(cm_rtc_set_current_time(cm_rtc_get_current_time()+600)<0) //+10min set
    {
        cm_demo_printf("cm_rtc_get_current_time fail\n");
    }

    if (0 == cm_ntp_set_cfg(CM_NTP_CFG_SERVER, "ntp1.aliyun.com"))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SERVER success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SERVER fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_PORT, &port))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_PORT success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_PORT fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_TIMEOUT, &timeout))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_TIMEOUT success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_TIMEOUT fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_DNS, &dns_priority))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_DNS success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_DNS fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_SET_RTC, &set_rtc))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SET_RTC success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SET_RTC fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_CB, _cm_ntp_cb))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB fail\n");
        return;
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_CB_PARAM, "SYNCHTIME"))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB_PARAM success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB_PARAM fail\n");
        return;
    }


    if (0 != cm_ntp_sync())
    {
        cm_demo_printf("[NTP]cm_ntp_sync() fail\n");
    }

}

void test_3()
{
    uint16_t port = 123;
    uint32_t timeout = NTP_TIMEOUT;
    uint32_t dns_priority = 1;
    bool set_rtc = 0;

    set_rtc = 1;

    if (0 == cm_ntp_set_cfg(CM_NTP_CFG_SERVER, NULL))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SERVER success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SERVER fail\n");
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_PORT, NULL))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_PORT success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_PORT fail\n");
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_TIMEOUT, NULL))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_TIMEOUT success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_TIMEOUT fail\n");
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_DNS, NULL))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_DNS success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_DNS fail\n");
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_SET_RTC, NULL))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SET_RTC success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_SET_RTC fail\n");
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_CB, NULL))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB fail\n");
    }
    if(0 == cm_ntp_set_cfg(CM_NTP_CFG_CB_PARAM, NULL))
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB_PARAM success\n");
    }
    else
    {
        cm_demo_printf("cm_ntp_set_cfg CM_NTP_CFG_CB_PARAM fail\n");
    }


    if (0 != cm_ntp_sync())
    {
        cm_demo_printf("[NTP]cm_ntp_sync() fail\n");
    }
}

void SHELL_testNtp(char argc, char **argv)
{
    if(cm_rtc_set_current_time(cm_rtc_get_current_time()+6000)<0) //+10min set
    {
        cm_demo_printf("cm_rtc_get_current_time fail\n");
    }
    cm_demo_printf("test_1 before time is %ld\n", cm_rtc_get_current_time());
    test_1();
    osDelay(NTP_TIMEOUT*2);
    cm_demo_printf("test_1 after time is %ld\n", cm_rtc_get_current_time());

    cm_demo_printf("test_2 before time is %ld\n", cm_rtc_get_current_time());
    test_2();
    osDelay(NTP_TIMEOUT*2);
    cm_demo_printf("test_2 after time is %ld\n", cm_rtc_get_current_time());

    test_3();

}

#if 0
void SHELL_testNtp(char argc, char **argv)
{
    unsigned char operation[20] = {0};
    if(argv == NULL || argc <2)
    {
        osPrintf("Please input operation,e.g. cm_ntp GETNETTIME\n");
        return;
    }
    sprintf((char *)operation, "%s", argv[1]);
    osPrintf("argv[1] is %s\n",argv[1]);
    if (0 == strcmp((const char *)operation, "GETNETTIME"))
    {
        uint16_t port = 123;
        uint32_t timeout = 6000;
        uint32_t dns_priority = 1;
        bool set_rtc = 0;

        cm_ntp_set_cfg(CM_NTP_CFG_SERVER, "cn.ntp.org.cn");
        //cm_ntp_set_cfg(CM_NTP_CFG_SERVER, "ntp1.aliyun.com");
        cm_ntp_set_cfg(CM_NTP_CFG_PORT, &port);
        cm_ntp_set_cfg(CM_NTP_CFG_TIMEOUT, &timeout);
        cm_ntp_set_cfg(CM_NTP_CFG_DNS, &dns_priority);
        cm_ntp_set_cfg(CM_NTP_CFG_SET_RTC, &set_rtc);
        cm_ntp_set_cfg(CM_NTP_CFG_CB, _cm_ntp_cb);
        cm_ntp_set_cfg(CM_NTP_CFG_CB_PARAM, "GETNETTIME");

        if (0 != cm_ntp_sync())
        {
            cm_demo_printf("[NTP]cm_ntp_sync() fail\n");
        }
    }
    else if (0 == strcmp((const char *)operation, "SYNCHTIME"))
    {
        uint16_t port = 123;
        uint32_t timeout = 6000;
        uint32_t dns_priority = 1;
        bool set_rtc = 1;

        cm_ntp_set_cfg(CM_NTP_CFG_SERVER, "ntp1.aliyun.com");
        cm_ntp_set_cfg(CM_NTP_CFG_PORT, &port);
        cm_ntp_set_cfg(CM_NTP_CFG_TIMEOUT, &timeout);
        cm_ntp_set_cfg(CM_NTP_CFG_DNS, &dns_priority);
        cm_ntp_set_cfg(CM_NTP_CFG_SET_RTC, &set_rtc);
        cm_ntp_set_cfg(CM_NTP_CFG_CB, _cm_ntp_cb);
        cm_ntp_set_cfg(CM_NTP_CFG_CB_PARAM, "SYNCHTIME");

        if (0 != cm_ntp_sync())
        {
            cm_demo_printf("[NTP]cm_ntp_sync() fail\n");
        }
    }
    else
    {
        cm_demo_printf("[NTP] Illegal operation\n");
    }
}
#endif

#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(cm_ntp, SHELL_testNtp);

#endif
#endif

