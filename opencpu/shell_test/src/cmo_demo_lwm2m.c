/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_LWM2M_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>
#include "cm_lwm2m.h"


#define cm_demo_printf(fmt, ...)           osPrintf("[DEMO]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
static cm_lwm2m_handle_t onenet_dev = NULL;         //OneNET设备

static void cm_lwm2m_event_cb(int32_t event, cm_lwm2m_cb_param_t param)
{
    cm_demo_printf("[%p %s] event: %d", param.handle, (char *)param.cb_param, event);
    switch (event)
    {
        case CM_LWM2M_EVENT_REG_SUCCESS:
        {
            cm_demo_printf("lwm2m register ok");
        }
        break;
        case CM_LWM2M_EVENT_REG_FAILED:
        {
            cm_demo_printf("lwm2m register fail");
        }
        break;
        case CM_LWM2M_EVENT_UNREG_DONE:
        {
            cm_demo_printf("lwm2m unregister done");
        }
        break;
        default:
        break;
    }
}

static void cm_lwm2m_notify_cb(int32_t mid, cm_lwm2m_cb_param_t param)
{
    cm_demo_printf("[%p %s]mid: %d", param.handle, (char *)param.cb_param, mid);
}

static void cm_lwm2m_read_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, cm_lwm2m_cb_param_t param)
{
    cm_demo_printf("[%p %s]mid[%d]objid[%d]insid[%d]resid[%d]", param.handle, (char *)param.cb_param, \
                                                            mid, objid, insid, resid);
}

static void cm_lwm2m_write_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, int32_t type, int32_t is_over, char *data, int32_t len, cm_lwm2m_cb_param_t param)
{
    cm_demo_printf("[%p %s]mid[%d]objid[%d]insid[%d]resid[%d]type[%d]is_over[%d]len[%d]data[%s]", \
                param.handle, (char *)param.cb_param, mid, objid, insid, resid, type, is_over, len, data);

    int32_t ret = cm_lwm2m_write_rsp(onenet_dev, mid, CM_LWM2M_RESULT_204_CHANGED);

    if (ret != CM_LWM2M_SUCCESS) {
        cm_demo_printf("write rsp fail, ret[%d]", ret);
        return;
    }
}

static void cm_lwm2m_execute_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, char *data, int32_t len, cm_lwm2m_cb_param_t param)
{
    cm_demo_printf("[%p %s]mid:%d objid:%d insid:%d resid:%d len:%d len:%s", param.handle, (char *)param.cb_param, mid, objid, insid, resid, len, data);

    int32_t ret = cm_lwm2m_execute_rsp(param.handle, mid, CM_LWM2M_RESULT_204_CHANGED);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("execute fail, ret is %d", ret);
        return;
    }
}

static void cm_lwm2m_observe_cb(int32_t mid, int32_t observe, int32_t objid, int32_t insid, int32_t resid, cm_lwm2m_cb_param_t param)
{
    cm_demo_printf("[%p %s]%d_%d_%d: %d", param.handle, (char *)param.cb_param, objid, insid, resid, observe);
}

static void cm_lwm2m_discover_cb(int32_t mid, int32_t objid, cm_lwm2m_cb_param_t param)
{
    cm_demo_printf("[%p %s]discover mid:%d objid:%d", param.handle, (char *)param.cb_param, mid, objid);
}

static void cm_lwm2m_params_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, char *parameter, int32_t len, cm_lwm2m_cb_param_t param)
{
    cm_demo_printf("[%p %s]params mid:%d objid:%d insid:%d resid:%d len:%d parameter:%s", param.handle, (char *)param.cb_param, mid, objid, insid, resid, resid, parameter);

    int32_t ret = cm_lwm2m_param_rsp(param.handle, mid, CM_LWM2M_RESULT_204_CHANGED);

    if (CM_LWM2M_SUCCESS != ret)
    {
        cm_demo_printf("params fail, ret is %d", ret);
        return;
    }
}

/**
 *  UART口OneNET功能功能调试使用示例
 */
void SHELL_LWM2M_TEST_ONENET(char argc, char **argv)
{
    int32_t ret = -1;
    static int mid = 1;

    if (argc != 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        cm_demo_printf("Usage: %s <cmd>", argv[0]);
        cm_demo_printf("Cmd: CHECKIN NOTIFY NOTIFY_TLV CHECKOUT");
        return;
    }

    if (!strcmp(argv[1], "CHECKIN"))
    {
        if (NULL == onenet_dev)
        {
            cm_lwm2m_cfg_t cfg = {0};
            cfg.platform = CM_LWM2M_ONENET;
            cfg.endpoint.pattern = 2;
            cfg.endpoint.name = NULL;
            cfg.host = "nbiotbt.heclouds.com:5683";
            cfg.flag = 3;
            cfg.auth_code = NULL;
            cfg.psk = NULL;
            cfg.pskid = NULL;
            cfg.auto_update = 1;
            cfg.cb.onEvent = cm_lwm2m_event_cb;
            cfg.cb.onNotify = cm_lwm2m_notify_cb;
            cfg.cb.onRead = cm_lwm2m_read_cb;
            cfg.cb.onWrite = cm_lwm2m_write_cb;
            cfg.cb.onExec = cm_lwm2m_execute_cb;
            cfg.cb.onObserve = cm_lwm2m_observe_cb;
            cfg.cb.onDiscover = cm_lwm2m_discover_cb;
            cfg.cb.onParams = cm_lwm2m_params_cb;
            cfg.cb_param = "OneNET";

            ret = cm_lwm2m_create(&cfg, &onenet_dev);

            if (ret != CM_LWM2M_SUCCESS)
            {
                cm_demo_printf("create fail, ret : %d", ret);
                return;
            }

            uint8_t instances_3303[1] = {1};
            ret = cm_lwm2m_add_obj(onenet_dev, 3303, instances_3303, 1, 0, 0);

            if (ret != CM_LWM2M_SUCCESS)
            {
                cm_demo_printf("add obj[%d] fail, ret : %d", 3303, ret);
                return;
            }

            uint8_t instances_3306[1] = {1};
            ret = cm_lwm2m_add_obj(onenet_dev, 3306, instances_3306, 1, 0, 0);

            if (ret != CM_LWM2M_SUCCESS) {
                cm_demo_printf("add obj[%d] fail, ret : %d", 3306, ret);
                return;
            }

            int32_t resources_3303[2] = {5700, 5601};
            ret = cm_lwm2m_discover(onenet_dev, 3303, resources_3303, 2);

            if (ret != CM_LWM2M_SUCCESS)
            {
                cm_demo_printf("discover obj[%d] fail, ret : %d", 3303, ret);
                return;
            }

            int32_t resources_3306[2] = {5852, 5853};
            ret = cm_lwm2m_discover(onenet_dev, 3306, resources_3306, 2);

            if (ret != CM_LWM2M_SUCCESS)
            {
                cm_demo_printf("discover obj[%d] fail, ret : %d", 3306, ret);
                return;
            }

            ret = cm_lwm2m_open(onenet_dev, 30, 3000);

            if (ret != CM_LWM2M_SUCCESS)
            {
                cm_demo_printf("open fail, ret : %d", ret);
                return;
            }

            cm_demo_printf("onenet_dev[%p] CHECKIN SUCC", onenet_dev);
        }

    }
    else if (!strcmp(argv[1], "NOTIFY"))
    {
        if (NULL != onenet_dev)
        {
            /* 服务器支持的编码格式（即content_type入参）支持情况请与平台确认，本用例开发时OneNET平台支持的格式为TLV */
            ret = cm_lwm2m_notify_packing(onenet_dev, 3303, 0, 5700, 4, "20.8", strlen("20.8"), 4);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("notify packing fail, ret : %d", ret);
                return;
            }

            ret = cm_lwm2m_notify(onenet_dev, mid++);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("notify fail, ret : %d", ret);
                return;
            }

            /* 服务器支持的编码格式（即content_type入参）支持情况请与平台确认，本用例开发时OneNET平台支持的格式为TLV */
            ret = cm_lwm2m_notify_packing(onenet_dev, 3306, 0, 5853, 1, "test", strlen("test"), 4);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("notify packing fail, ret : %d", ret);
                return;
            }

            ret = cm_lwm2m_notify(onenet_dev, mid++);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("notify fail, ret : %d", ret);
                return;
            }
        }
    }
    else if (!strcmp(argv[1], "NOTIFY_TLV"))
    {
        if (NULL != onenet_dev)
        {
            /* 服务器支持的编码格式（即content_type入参）支持情况请与平台确认，本用例开发时OneNET平台支持的格式为TLV */
            ret = cm_lwm2m_notify_packing(onenet_dev, 3303, 0, 5700, 4, "21.8", strlen("21.8"), 4);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("notify packing fail, ret : %d", ret);
                return;
            }

            /* 服务器支持的编码格式（即content_type入参）支持情况请与平台确认，本用例开发时OneNET平台支持的格式为TLV */
            ret = cm_lwm2m_notify_packing(onenet_dev, 3303, 0, 5601, 4, "21.7", strlen("21.7"), 4);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("notify packing fail, ret : %d", ret);
                return;
            }

            ret = cm_lwm2m_notify(onenet_dev, mid++);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("notify fail, ret : %d", ret);
                return;
            }
        }
    }
    else if (!strcmp(argv[1], "CHECKOUT"))
    {
        if (NULL != onenet_dev)
        {
            ret = cm_lwm2m_del_obj(onenet_dev, 3303);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("del 3303 obj fail, ret : %d", ret);
                return;
            }

            ret = cm_lwm2m_del_obj(onenet_dev, 3306);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("del 3306 obj fail, ret : %d", ret);
                return;
            }

            ret = cm_lwm2m_close(onenet_dev);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("close fail, ret : %d", ret);
                return;
            }

            ret = cm_lwm2m_delete(onenet_dev);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("delete fail, ret : %d", ret);
                return;
            }

            onenet_dev = NULL;
        }
    }
    else
    {
        cm_demo_printf("Illegal operation");
    }
}

/**
 *  UART口CTWing功能功能调试使用示例
 */
void SHELL_LWM2M_TEST_CTWING(char argc, char **argv)
{
    int32_t ret = -1;

    if (argc != 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
        cm_demo_printf("Usage: %s <cmd>", argv[0]);
        cm_demo_printf("Cmd: CHECKIN NOTIFY CHECKOUT");
        return;
    }

    if (!strcmp(argv[1], "CHECKIN"))
    {
        if (NULL == onenet_dev)
        {
            cm_lwm2m_cfg_t cfg = {0};
            cfg.platform = CM_LWM2M_CTWING;
            cfg.endpoint.pattern = 1;
            cfg.endpoint.name = NULL;
            cfg.host = "221.229.214.202";
            cfg.flag = 0;
            cfg.auth_code = NULL;
            cfg.psk = NULL;
            cfg.pskid = NULL;
            cfg.auto_update = 1;
            cfg.cb.onRead = cm_lwm2m_read_cb;
            cfg.cb.onWrite = cm_lwm2m_write_cb;
            cfg.cb.onExec = cm_lwm2m_execute_cb;
            cfg.cb.onObserve = cm_lwm2m_observe_cb;
            cfg.cb.onParams = cm_lwm2m_params_cb;
            cfg.cb.onEvent = cm_lwm2m_event_cb;
            cfg.cb.onNotify = cm_lwm2m_notify_cb;
            cfg.cb.onDiscover = cm_lwm2m_discover_cb;
            cfg.cb_param = "CTWing";

            ret = cm_lwm2m_create(&cfg, &onenet_dev);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("create fail, ret : %d", ret);
                return;
            }

            uint8_t instances_19[1] = {1};
            ret = cm_lwm2m_add_obj(onenet_dev, 19, instances_19, 1, 0, 0);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("add obj[%d] fail, ret : %d", 19, ret);
                return;
            }

            int32_t resoures_19[1] = {0};
            ret = cm_lwm2m_discover(onenet_dev, 19, resoures_19, 1);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("discover obj[%d] fail, ret : %d", 19, ret);
                return;
            }

            ret = cm_lwm2m_open(onenet_dev, 30, 3000);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("open fail, ret is %d", ret);
                return;
            }

            cm_demo_printf("onenet_dev[%p] CHECKIN SUCC", onenet_dev);
        }
    }
    else if (!strcmp(argv[1], "NOTIFY"))
    {
        if (NULL != onenet_dev)
        {
            ret = cm_lwm2m_notify_packing(onenet_dev, 19, 0, 0, 1, "test", strlen("test"), 1);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("notify_packing fail, ret is %d", ret);
                return;
            }

            static int mid = 1;
            ret = cm_lwm2m_notify(onenet_dev, mid++);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("notify fail, ret is %d", ret);
                return;
            }
        }
    }
    else if (!strcmp(argv[1], "CHECKOUT"))
    {
        if (NULL != onenet_dev)
        {
            ret = cm_lwm2m_del_obj(onenet_dev, 19);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("del_obj 19 fail, ret is %d", ret);
                return;
            }

//NR_SHELL_CMD_EXPORT(cm_Lwm2m_create, SHELL_LWM2M_CREATE);
//NR_SHELL_CMD_EXPORT(cm_Lwm2m_delete, SHELL_LWM2M_DELETE);
//NR_SHELL_CMD_EXPORT(cm_Lwm2m_update, SHELL_LWM2M_UPDATE);
            ret = cm_lwm2m_close(onenet_dev);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("close fail, ret is %d", ret);
                return;
            }

            ret = cm_lwm2m_delete(onenet_dev);

            if (CM_LWM2M_SUCCESS != ret)
            {
                cm_demo_printf("delete fail, ret is %d", ret);
                return;
            }

            onenet_dev = NULL;
        }
    }
    else
    {
        cm_demo_printf("[CTWing] Illegal operation");
    }
}

NR_SHELL_CMD_EXPORT(cm_lwm2m_test_onent, SHELL_LWM2M_TEST_ONENET);
NR_SHELL_CMD_EXPORT(cm_lwm2m_test_ctwing, SHELL_LWM2M_TEST_CTWING);

#endif
#endif /* end CM_DEMO_LWM2M_SUPPORT*/
