/**
 *  @file    cm_demo_ntp.c
 *  @brief   OpenCPU NTP相关测试例程
 *  @copyright copyright ? 2022 China Mobile IOT. All rights reserved.
 *  @author by cmiot3000
 *  @date 2022/06/02
 */



 /****************************************************************************
 * Included Files
 ****************************************************************************/
#ifdef CM_DEMO_NTP_SUPPORT
#ifdef OS_USING_SHELL

#include <stdio.h>
#include <string.h>
#include <nr_micro_shell.h>
#include "cm_ntp.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define cm_demo_printf osPrintf
#define NTP_TIMEOUT 6000


/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void _cm_ntp_cb(cm_ntp_event_e event, uint64_t *event_param, void *cb_param)
{
    char *tag = cb_param ? (char *)cb_param : "CB_PARAM_PTR_NULL";
    char * temp_eventp = event_param ? event_param : "NULL";
    if (CM_NTP_EVENT_SYNC_OK == event)
    {
        cm_demo_printf("[NTP][%s] get net time %s\n", tag, (int)temp_eventp);
    }
    else
    {
        cm_demo_printf("[NTP][%s] error\n", tag);
    }
}
/*
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
*/
/****************************************************************************
 * Private Data
 ****************************************************************************/


/****************************************************************************
 * Private Functions
 ****************************************************************************/



/**
*  UART口NTP功能功能调试使用示例
*  ntp GETNETTIME                           //仅获取网络时间，不同步至本地时间
*  ntp SYNCHTIME                            //获取网络时间并将网络时间与本地时间同步
*
*  使用注意：
*  1.示例中的时间服务器仅作为参数，实际使用时需配置成为用户提供授时服务的服务器信息；
*  2.若使用公共时间服务器，因公共时间服务器未响应模组授时请求而出现获取时间失败属于正常现象；
*  3.根据NTP协议规定，NTP基于UDP报文进行传输，UDP为无连接的协议，不提供可靠性的传输；
*  4.正如UDP协议特性，NTP存在一定程度失败的可能。应用开发一般可采用错误重试、定时进行NTP服务等多次进行NTP请求的策略获取时间。
*/
void cm_test_ntp(char *cmd)
{
    if (0 == strcasecmp((const char *)cmd, "GETNETTIME"))
    {
        uint16_t port = 123;
        uint32_t timeout = 6000;
        uint32_t dns_priority = 1;
        bool set_rtc = 0;

        /* 示例中的时间服务器仅作为参数，实际使用时需配置成为用户提供授时服务的服务器信息。
           若使用公共时间服务器，因公共时间服务器未响应模组授时请求而出现获取时间失败属于正常现象。
           根据NTP协议规定，NTP基于UDP报文进行传输，UDP为无连接的协议，不提供可靠性的传输。
           正如UDP协议特性，NTP存在一定程度失败的可能。应用开发一般可采用错误重试、定时进行NTP服务等多次进行NTP请求的策略获取时间 */
        cm_ntp_set_cfg(CM_NTP_CFG_SERVER, "ntp1.aliyun.com");
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
    else if (0 == strcasecmp((const char *)cmd, "SYNCHTIME"))
    {
        uint16_t port = 123;
        uint32_t timeout = 6000;
        uint32_t dns_priority = 1;
        bool set_rtc = 1;

        /* 示例中的时间服务器仅作为参数，实际使用时需配置成为用户提供授时服务的服务器信息。
           若使用公共时间服务器，因公共时间服务器未响应模组授时请求而出现获取时间失败属于正常现象。
           根据NTP协议规定，NTP基于UDP报文进行传输，UDP为无连接的协议，不提供可靠性的传输。
           正如UDP协议特性，NTP存在一定程度失败的可能。应用开发一般可采用错误重试、定时进行NTP服务等多次进行NTP请求的策略获取时间 */
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

static int argv_sanity_check(int argc, char **argv)
{
    if(argv == NULL)
    {
       cm_demo_printf("cm_ping argv null\r\n");
       return -1;
    }
    for(int i = 0; i < argc; i++)
    {
        if(argv[i] == NULL)
        {
            cm_demo_printf("argv[%d] is NULL, invaild\r\n", i);
            return -2;
        }
    }
    return 0;
}

void SHELL_testNtp(char argc, char **argv)
{
    int ret = argv_sanity_check(argc, argv);
    if(ret != 0)
    {
        cm_demo_printf("argv_sanity_check failed ret[%d]\r\n",ret);
        return;
    }

    if(argc == 2) /* help */
    {
        if (0 == strcasecmp(argv[1], "help"))
        {
            cm_demo_printf("---------------- ping help---------------------\r\n");
            cm_demo_printf("1 cm_ntp GETNETTIME\r\n");
            cm_demo_printf("2 cm_ntp SYNCHTIME\r\n");
            cm_demo_printf("-----------------ping--------------------------\r\n");
        }
        else
            cm_test_ntp(argv[1]);
    }
    else
        cm_demo_printf("cm_ntp parameter invalid\r\n");


}


NR_SHELL_CMD_EXPORT(cm_ntp, SHELL_testNtp);

#endif
#endif

