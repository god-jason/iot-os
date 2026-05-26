/*********************************************************
 *  @file    cm_demo_virt_at.c
 *  @brief   OpenCPU 虚拟AT示例
 *  Copyright (c) 2025 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot4594 2025/05
 ********************************************************/

#include "cm_os.h"
#include "cm_mem.h"
#include "cm_virt_at.h"
#include "cm_sys.h"
#include "cm_demo_common.h"
#include "cm_demo_uart.h"
#include "cm_demo_virt_at.h"

void oc_resp_cb(cm_virt_at_param_t *param)
{
    if(param->event == CM_VIRT_AT_OK)
    {
        cm_demo_printf("oc_resp_cb:event=%d rsp=%s rsp_len=%d user_param=%s\n", param->event, param->rsp, param->rsp_len, param->user_param);
    }
    else
    {
        cm_demo_printf("error\n");
    }
}

//在回调函数中对所需URC进行匹配
void oc_msg_cb(uint8_t *urc, int32_t urc_len)
{
    cm_demo_printf("oc_msg_cb urc:%s urc_len:%d\n", urc, urc_len);
}

void cm_virt_at_test_sync(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }
    char operation[64] = {0};
    sprintf(operation, "%s\r\n", cmd);
    uint8_t rsp[128] = {0};
    int32_t rsp_len = 128;
    int ret = 0;
    ret = cm_virt_at_send_sync((const uint8_t *)operation, rsp, &rsp_len, 10);

    if(ret == 0)
    {
        cm_demo_printf("rsp=%s rsp_len=%d\n", rsp, rsp_len);
    }
    else
    {
        cm_demo_printf("ret != %d\n",ret);
    }

}

void cm_virt_at_test_async(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }
    char operation[64] = {0};
    sprintf(operation, "%s\r\n", cmd);
    if(cm_virt_at_send_async((const uint8_t *)operation, oc_resp_cb, "operation") != 0)
    {
        cm_demo_printf("cm_virt_at_send_async(, oc_resp_cb, operation) != 0");
    }
}

void cm_test_urc(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }
    if(0 == strcasecmp((const char *)cmd, "reg") || 0 == strcasecmp((const char *)cmd, "REG"))
    {
        cm_virt_at_urc_cb_reg((const uint8_t *)"+CMT:",(cm_virt_at_urc_cb)oc_msg_cb);
        cm_demo_printf("+CMT reg ok\n");
    }
    else if(0 == strcasecmp((const char *)cmd, "dereg") || 0 == strcasecmp((const char *)cmd, "DEREG"))
    {
        cm_virt_at_urc_cb_dereg((const uint8_t *)"+CMT:",(cm_virt_at_urc_cb)oc_msg_cb);
        cm_demo_printf("+CMT dereg ok\n");
    }
    else
    {
        cm_demo_printf("invalid param\n");
    }
}

