/*********************************************************
 *  @file    cm_demo_virt_at.c
 *  @brief   OpenCPU 虚拟AT示例
 *  Copyright (c) 2025 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot4594 2025/05
 ********************************************************/

#ifdef CM_DEMO_VIRT_AT_SUPPORT
#ifdef OS_USING_SHELL

#include <os.h>
#include <stdlib.h>
#include <string.h>
#include <nr_micro_shell.h>
#include "cm_virt_at.h"

#define cm_demo_printf osPrintf

void oc_resp_cb(cm_virt_at_param_t *param)
{
    if (param == NULL)
    {
        cm_demo_printf("oc_resp_cb: param is NULL\n");
        return;
    }
    if(param->event == CM_VIRT_AT_OK)
    {
        cm_demo_printf("oc_resp_cb:event=%d rsp=%s rsp_len=%d user_param=%s\r\n", param->event, param->rsp, param->rsp_len, param->user_param);
    }
    else
    {
        cm_demo_printf("error\r\n");
    }
}

//在回调函数中对所需URC进行匹配
void oc_msg_cb(uint8_t *urc, int32_t urc_len)
{
    if (urc == NULL)
    {
        cm_demo_printf("oc_msg_cb: urc is NULL\n");
        return;
    }
    cm_demo_printf("oc_msg_cb urc:%s urc_len:%d\r\n", urc, urc_len);
}

void cm_virt_at_test_sync(int argc, char **argv)
{
    if(argv == NULL || argc < 2)
    {
        cm_demo_printf("invalid param\r\n");
        return;
    }
    if (argv[1] == NULL)
    {
        cm_demo_printf("cm_virt_at_test_sync: argv[1] is NULL\n");
        return;
    }
    
    char operation[64] = {0};
    sprintf(operation, "%s\r\n", argv[1]);
    uint8_t rsp[128] = {0};
    int32_t rsp_len = 128;
    int ret = 0;
    ret = cm_virt_at_send_sync((const uint8_t *)operation, rsp, &rsp_len, 10);

    if(ret == 0)
    {
        cm_demo_printf("rsp=%s rsp_len=%d\r\n", rsp, rsp_len);
    }
    else
    {
        cm_demo_printf("ret != %d\r\n",ret);
    }

}

void cm_virt_at_test_async(int argc, char **argv)
{
    if(argv == NULL || argc < 2)
    {
        cm_demo_printf("invalid param\r\n");
        return;
    }
    if (argv[1] == NULL)
    {
        cm_demo_printf("cm_virt_at_test_async: argv[1] is NULL\n");
        return;
    }
    
    char operation[64] = {0};
    sprintf(operation, "%s\r\n", argv[1]);
    if(cm_virt_at_send_async((const uint8_t *)operation, oc_resp_cb, "operation") != 0)
    {
        cm_demo_printf("cm_virt_at_send_async(, oc_resp_cb, operation) != 0\r\n");
    }
}

void cm_test_urc(int argc, char **argv)
{
    if(argv == NULL || argc < 2)
    {
        cm_demo_printf("invalid param\r\n");
        return;
    }
    if (argv[1] == NULL)
    {
        cm_demo_printf("cm_test_urc: argv[1] is NULL\n");
        return;
    }
    
    const char *cmd = argv[1];
    if(0 == strcasecmp(cmd, "reg") || 0 == strcasecmp(cmd, "REG"))
    {
        cm_virt_at_urc_cb_reg((const uint8_t *)"+CMT:",(cm_virt_at_urc_cb)oc_msg_cb);
        cm_demo_printf("+CMT reg ok\r\n");
    }
    else if(0 == strcasecmp(cmd, "dereg") || 0 == strcasecmp(cmd, "DEREG"))
    {
        cm_virt_at_urc_cb_dereg((const uint8_t *)"+CMT:",(cm_virt_at_urc_cb)oc_msg_cb);
        cm_demo_printf("+CMT dereg ok\r\n");
    }
    else
    {
        cm_demo_printf("invalid param\r\n");
    }
}

NR_SHELL_CMD_EXPORT(virt_at_sync, cm_virt_at_test_sync);
NR_SHELL_CMD_EXPORT(virt_at_async, cm_virt_at_test_async);
NR_SHELL_CMD_EXPORT(urc, cm_test_urc);
#endif
#endif
