/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_VIRT_AT_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>
#include "cm_virt_at.h"

#define cm_demo_printf osPrintf

void SHELL_testVirtAt_sync(char argc, char **argv)
{
    if (argc < 2 || argc > 2)
    {
        cm_demo_printf("wrong shell cmd, example: cm_virt_at_sync AT+CFUN=1 \r\n");
        return;
    }
    char operation[64] = {0};
    sprintf(operation, "%s\r\n", argv[1]);
    uint8_t rsp[128] = {0};
    int32_t rsp_len = 0;

    if(cm_virt_at_send_sync((const uint8_t *)operation, rsp, &rsp_len, 10) == 0)
    {
        cm_demo_printf("rsp=%s rsp_len=%d\n", rsp, rsp_len);
    }
    else
    {
        cm_demo_printf("ret != 0\n");
        cm_demo_printf("rsp=%s rsp_len=%d\n", rsp, rsp_len);
    }
}


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

void SHELL_testVirtAt_async(char argc, char **argv)
{
    if (argc < 2 || argc > 2)
    {
        cm_demo_printf("wrong shell cmd, example: cm_virt_at_async AT+COPS=? \r\n");
        return;
    }
    char operation[64] = {0};
    sprintf(operation, "%s\r\n", argv[1]);
    if(cm_virt_at_send_async((const uint8_t *)operation, oc_resp_cb, "operation") != 0)
    {
        cm_demo_printf("cm_virt_at_send_async(, oc_resp_cb, operation) != 0");
    }
}

//在回调函数中对所需URC进行匹配
void oc_msg_cb(uint8_t *urc, int32_t urc_len)
{
    cm_demo_printf("oc_msg_cb urc:%s urc_len:%d\n", urc, urc_len);
}

//注册接收短信所需URC
void SHELL_testVirtAt_urc_cb_reg(char argc, char **argv)
{
    if (argc < 2 || argc > 2)
    {
        cm_demo_printf("wrong shell cmd, example: cm_virt_at_urc_reg +CMT \r\n");
        return;
    }
    cm_virt_at_urc_cb_reg((const uint8_t *)argv[1],(cm_virt_at_urc_cb)oc_msg_cb);
}

//取消注册注册接收短信所需URC
void SHELL_testVirtAt_urc_cb_dereg(char argc, char **argv)
{
    if (argc < 2 || argc > 2)
    {
        cm_demo_printf("wrong shell cmd, example: cm_virt_at_urc_dereg +CMT \r\n");
        return;
    }
    cm_virt_at_urc_cb_dereg((const uint8_t *)argv[1],(cm_virt_at_urc_cb)oc_msg_cb);
}

NR_SHELL_CMD_EXPORT(cm_virt_at_sync, SHELL_testVirtAt_sync);

NR_SHELL_CMD_EXPORT(cm_virt_at_async, SHELL_testVirtAt_async);

NR_SHELL_CMD_EXPORT(cm_virt_at_urc_reg, SHELL_testVirtAt_urc_cb_reg);

NR_SHELL_CMD_EXPORT(cm_virt_at_urc_dereg, SHELL_testVirtAt_urc_cb_dereg);

#endif
#endif

