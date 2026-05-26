/*********************************************************
 *  @file    cm_demo_sms.c
 *  @brief   OpenCPU 短信及URC示例
 *  Copyright (c) 2025 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot4594 2025/05
 ********************************************************/

#include "cm_os.h"
#include "cm_mem.h"
#include "cm_sms.h"
#include "cm_sys.h"
#include "cm_demo_common.h"
#include "cm_demo_uart.h"
#include "cm_demo_sms.h"

void cm_test_sendmsg(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }
    int ret = -1;
    if(0 == strcasecmp((const char *)cmd, "text") || 0 == strcasecmp((const char *)cmd, "TEXT"))
    {
        ret = cm_sms_send_txt("zhongyimozu", "10086", CM_MSG_MODE_GSM_7, CM_SIM_ID_0);//发送txt短信
        if(ret == 0)
        {
            cm_demo_printf("send txt sms success\n");
        }
        else
        {
            cm_demo_printf("send txt sms fail\n");
        }
    }
    else if(0 == strcasecmp((const char *)cmd, "pdu") || 0 == strcasecmp((const char *)cmd, "PDU"))
    {
        ret = cm_sms_send_pdu("0891683108200305F011000B913195643832F50000AA0E7AF4DB7D3ED7DFF934F9ED3E03", "27", CM_MSG_MODE_GSM_7, CM_SIM_ID_0);//发送PDU短信
        if(ret == 0)
        {
            cm_demo_printf("send pdu sms success\n");
        }
        else
        {
            cm_demo_printf("send pdu sms fail\n");
        }
    }
    else
    {
        cm_demo_printf("invalid param\n");
    }
}
