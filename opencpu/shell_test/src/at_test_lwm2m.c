/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/7/14
 ********************************************************/

#ifdef CM_DEMO_LWM2M_SUPPORT

#include <os.h>
#include <stdlib.h>

#include "at_api.h"
#include "at_parser.h"
#include "cm_lwm2m.h"
#include "cm_mem.h"
#include "cm_demo_common.h"

typedef AT_CommandItem at_cmd_t;

#define LWM2M_LOG(fmt, ...)   osPrintf("[OC_LWM2M_TEST][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LWM2M_CMD_STR_LEN 512

#define CM_LWM2M_RET_CODE_OK 0 //成功
#define CM_LWM2M_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_LWM2M_RET_CODE_PARAM_ERROR 50 //参数错误 50

static cm_lwm2m_handle_t onenet_dev = NULL;         //OneNET设备

void lwm2m_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_create(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_delete(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_add_obj(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_del_obj(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_discover(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_open(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_update(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_notify_packing(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_notify(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_read_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_write_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_execute_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_param_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_lwm2m_observe_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);


AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_lwm2m_oc[] =
{
    {.name = "+LWM2M",   .setFunc = lwm2m_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};


static void cm_lwm2m_event_cb(int32_t event, cm_lwm2m_cb_param_t param)
{
    LWM2M_LOG("[%p %s] event: %d", param.handle, (char *)param.cb_param, event);
    switch (event)
    {
        case CM_LWM2M_EVENT_REG_SUCCESS:
        {
            LWM2M_LOG("lwm2m register ok");
        }
        break;
        case CM_LWM2M_EVENT_REG_FAILED:
        {
            LWM2M_LOG("lwm2m register fail");
        }
        break;
        case CM_LWM2M_EVENT_UNREG_DONE:
        {
            LWM2M_LOG("lwm2m unregister done");
        }
        break;
        default:
        break;
    }
}

static void cm_lwm2m_notify_cb(int32_t mid, cm_lwm2m_cb_param_t param)
{
    LWM2M_LOG("[%p %s]mid: %d", param.handle, (char *)param.cb_param, mid);
}

// +MIPLREAD: <ref>,<mid>,<objid>,<insid>,<resid>
static void cm_lwm2m_read_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, cm_lwm2m_cb_param_t param)
{
    LWM2M_LOG("[%p %s]mid[%d]objid[%d]insid[%d]resid[%d]", param.handle, (char *)param.cb_param, \
                                                            mid, objid, insid, resid);

    cm_uart_printf_urc("+MIPLREAD: %d,%d,%d,%d", mid, objid, insid, resid);
}

// +MIPLWRITE: <ref>,<mid>,<objid>,<insid>,<resid>,<type>,<len>,<value>,<flag>
static void cm_lwm2m_write_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, int32_t type, int32_t is_over, char *data, int32_t len, cm_lwm2m_cb_param_t param)
{
    LWM2M_LOG("[%p %s]mid[%d]objid[%d]insid[%d]resid[%d]type[%d]is_over[%d]len[%d]data[%s]", \
                param.handle, (char *)param.cb_param, mid, objid, insid, resid, type, is_over, len, data);

    cm_uart_printf_urc("+MIPLWRITE: %d,%d,%d,%d,%d,%d,%s,%d", mid, objid, insid, resid, type, len, data, is_over);
}

// +MIPLEXECUTE: <ref>,<mid>,<objid>,<insid>,<resid>[,<len>,<cmd>]
static void cm_lwm2m_execute_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, char *data, int32_t len, cm_lwm2m_cb_param_t param)
{
    LWM2M_LOG("[%p %s]mid:%d objid:%d insid:%d resid:%d len:%d len:%s", param.handle, (char *)param.cb_param, mid, objid, insid, resid, len, data);

    cm_uart_printf_urc("+MIPLEXECUTE: %d,%d,%d,%d,%d,%s", mid, objid, insid, resid, len, data);
}

// +MIPLOBSERVE: <ref>,<mid>,<flag>,<objid>,<insid>,<resid>
static void cm_lwm2m_observe_cb(int32_t mid, int32_t observe, int32_t objid, int32_t insid, int32_t resid, cm_lwm2m_cb_param_t param)
{
    LWM2M_LOG("[%p %s]%d_%d_%d: %d", param.handle, (char *)param.cb_param, objid, insid, resid, observe);

    cm_uart_printf_urc("+MIPLOBSERVE: %d,%d,%d,%d,%d", mid, observe, objid, insid, resid);
}

static void cm_lwm2m_discover_cb(int32_t mid, int32_t objid, cm_lwm2m_cb_param_t param)
{
    LWM2M_LOG("[%p %s]discover mid:%d objid:%d", param.handle, (char *)param.cb_param, mid, objid);
}

//+MIPLPARAMETER: <ref>,<mid>,<objid>,<insid>,<resid>,<len>,<para>
static void cm_lwm2m_params_cb(int32_t mid, int32_t objid, int32_t insid, int32_t resid, char *parameter, int32_t len, cm_lwm2m_cb_param_t param)
{
    LWM2M_LOG("[%p %s]params mid:%d objid:%d insid:%d resid:%d len:%d parameter:%s", param.handle, (char *)param.cb_param, mid, objid, insid, resid, resid, parameter);

    cm_uart_printf_urc("+MIPLPARAMETER: %d,%d,%d,%d,%d,%s", mid, objid, insid, resid, len, parameter);
}

// AT+LWM2M="XX", param1, param2, ...
// "XX" 为函数名
void lwm2m_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;

    LWM2M_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$,$,$");
        if (para_count < 1 /*|| para_count > 7*/)
        {
            ret = CM_LWM2M_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            ret = CM_LWM2M_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_lwm2m_create") == 0)
        {
            ret = func_cm_lwm2m_create(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_delete") == 0)
        {
            ret = func_cm_lwm2m_delete(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_add_obj") == 0)
        {
            ret = func_cm_lwm2m_add_obj(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_del_obj") == 0)
        {
            ret = func_cm_lwm2m_del_obj(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_discover") == 0)
        {
            ret = func_cm_lwm2m_discover(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_open") == 0)
        {
            ret = func_cm_lwm2m_open(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_update") == 0)
        {
            ret = func_cm_lwm2m_update(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_close") == 0)
        {
            ret = func_cm_lwm2m_close(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_notify_packing") == 0)
        {
            ret = func_cm_lwm2m_notify_packing(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_notify") == 0)
        {
            ret = func_cm_lwm2m_notify(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_read_rsp") == 0)
        {
            ret = func_cm_lwm2m_read_rsp(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_write_rsp") == 0)
        {
            ret = func_cm_lwm2m_write_rsp(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_execute_rsp") == 0)
        {
            ret = func_cm_lwm2m_execute_rsp(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_param_rsp") == 0)
        {
            ret = func_cm_lwm2m_param_rsp(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_lwm2m_observe_rsp") == 0)
        {
            ret = func_cm_lwm2m_observe_rsp(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            LWM2M_LOG("Error func name[%s]", func_name);
            ret = CM_LWM2M_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);

}

// AT+LWM2M="cm_lwm2m_create", host, flag, authcode
int func_cm_lwm2m_create(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    char *host = NULL;
    int flag = 0;
    char *authcode = NULL;

    host = cm_malloc(LWM2M_CMD_STR_LEN);
    if (host == NULL) {
        return CM_LWM2M_RET_CODE_MALLOC_FAIL;
    }
    memset(host, 0, LWM2M_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", host, LWM2M_CMD_STR_LEN))
    {
        cm_free(host);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }


    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &flag))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    authcode = cm_malloc(LWM2M_CMD_STR_LEN);
    if (authcode == NULL) {
        cm_free(host);
        return CM_LWM2M_RET_CODE_MALLOC_FAIL;
    }
    memset(authcode, 0, LWM2M_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", authcode, LWM2M_CMD_STR_LEN))
    {
        cm_free(host);
        cm_free(authcode);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    cm_lwm2m_cfg_t cfg = {0};
    cfg.platform = CM_LWM2M_ONENET;
    cfg.endpoint.pattern = 2;
    cfg.endpoint.name = NULL;
    cfg.host = host;
    cfg.flag = flag;
    cfg.auth_code = authcode;
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
        LWM2M_LOG("create fail");
    }

    cm_uart_printf_urc("+LWM2MCREATE: %d", ret);
    cm_free(host);
    cm_free(authcode);
    return 0;
}

// AT+LWM2M="cm_lwm2m_delete"
int func_cm_lwm2m_delete(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    ret = cm_lwm2m_delete(onenet_dev);

    if (CM_LWM2M_SUCCESS != ret)
    {
        LWM2M_LOG("delete fail");
    }

    cm_uart_printf_urc("+LWM2MDELETE: %d", ret);
    return 0;
}

// AT+LWM2M="cm_lwm2m_add_obj", objid,data,inscount
int func_cm_lwm2m_add_obj(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int objid;
    char *bitmap = NULL;
    uint8_t *instances;
    int inscount = 0;
    int attcount = 0;
    int actcount = 0;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &objid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    bitmap = cm_malloc(LWM2M_CMD_STR_LEN);
    if (bitmap == NULL) {
        return CM_LWM2M_RET_CODE_MALLOC_FAIL;
    }
    memset(bitmap, 0, LWM2M_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", bitmap, LWM2M_CMD_STR_LEN))
    {
        cm_free(bitmap);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &inscount))
    {
        cm_free(bitmap);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (strlen(bitmap) != inscount || inscount == 0) {
        cm_free(bitmap);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    for (int i = 0; i < inscount; i++) {
        if (bitmap[i] != '0' && bitmap[i] != '1') {
            cm_free(bitmap);
            return CM_LWM2M_RET_CODE_PARAM_ERROR;
        }
    }


    instances = cm_calloc(inscount, sizeof(uint8_t));
    if (instances == NULL) {
        cm_free(bitmap);
        return CM_LWM2M_RET_CODE_MALLOC_FAIL;
    }

    for (int i = 0; i < inscount; i++) {
        if (bitmap[i] != '0')
            instances[i] = 1;
    }

    for(int i = 0; i < inscount; i++) {
        osPrintf("ins%d[%d] ", i, instances[i]);
    }
    osPrintf("\r\n");

    ret = cm_lwm2m_add_obj(onenet_dev, objid, instances, inscount, attcount, actcount);

    if (ret != CM_LWM2M_SUCCESS) {
        LWM2M_LOG("add obj[%d] fail", objid);
    }

    cm_uart_printf_urc("+LWM2MADDOBJ: %d", ret);
    cm_free(bitmap);
    cm_free(instances);
    return 0;
}

// AT+LWM2M="cm_lwm2m_del_obj", objid
int func_cm_lwm2m_del_obj(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int objid;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &objid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_del_obj(onenet_dev, objid);

    if (ret != CM_LWM2M_SUCCESS) {
        LWM2M_LOG("del obj[%d] fail", objid);
    }

    cm_uart_printf_urc("+LWM2MDELOBJ: %d", ret);
    return 0;
}


// AT+LWM2M="cm_lwm2m_discover",objid,resoures,rescount
int func_cm_lwm2m_discover(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int objid;
    int rescount;
    char *data;
    int *resources;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &objid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    data = cm_malloc(LWM2M_CMD_STR_LEN);
    if (data == NULL) {
        return CM_LWM2M_RET_CODE_MALLOC_FAIL;
    }
    memset(data, 0, LWM2M_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", data, LWM2M_CMD_STR_LEN))
    {
        cm_free(data);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &rescount))
    {
        cm_free(data);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    resources = (int *)cm_calloc(rescount, sizeof(int));
    if (resources == NULL) {
        cm_free(data);
        return CM_LWM2M_RET_CODE_MALLOC_FAIL;
    }

    char *temp = data;
    int index = 0;
    do
    {
        int resid = atoi(temp);
        resources[index++] = resid;
        temp = strchr(temp, ';');
        if (temp) {
            temp += 1;
            if (index >= rescount) {
                cm_free(data);
                return CM_LWM2M_RET_CODE_PARAM_ERROR;
            }
        }
    } while (temp);

    if (index < rescount) {
        cm_free(data);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    for(int i = 0; i < rescount; i++) {
        osPrintf("res%d[%d] ", i, resources[i]);
    }
    osPrintf("\r\n");

    ret = cm_lwm2m_discover(onenet_dev, objid, resources, rescount);

    if (ret != CM_LWM2M_SUCCESS)
    {
        LWM2M_LOG("discover fail");
    }

    cm_uart_printf_urc("+LWM2MDISCOVER: %d", ret);
    cm_free(data);
    cm_free(resources);
    return 0;
}

// AT+LWM2M="cm_lwm2m_open", timeout, lifetime
int func_cm_lwm2m_open(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int timeout = 0;
    int lifetime = 0;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &timeout))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &lifetime))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_open(onenet_dev, timeout, lifetime);

    if (ret != CM_LWM2M_SUCCESS)
    {
        LWM2M_LOG("open fail");
    }

    cm_uart_printf_urc("+LWM2MOPEN: %d", ret);
    return 0;
}

// AT+LWM2M="cm_lwm2m_update",lifetime,update_object
int func_cm_lwm2m_update(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int lifetime;
    int update_object;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &lifetime))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &update_object))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (update_object != 0 && update_object != 1) {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_update(onenet_dev, lifetime, update_object);

    if (ret != CM_LWM2M_SUCCESS) {
        LWM2M_LOG("update fail");
    }

    cm_uart_printf_urc("+LWM2MUPDATE: %d", ret);
    return 0;
}

// AT+LWM2M="cm_lwm2m_close"
int func_cm_lwm2m_close(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    ret = cm_lwm2m_close(onenet_dev);

    if (CM_LWM2M_SUCCESS != ret)
    {
        LWM2M_LOG("close fail");
    }

    cm_uart_printf_urc("+LWM2MCLOSE: %d", ret);
    return 0;
}

// AT+LWM2M="cm_lwm2m_notify_packing",objid,insid,resid,type,value,content_type
int func_cm_lwm2m_notify_packing(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int objid;
    int insid;
    int resid;
    int type;
    char *value;
    int len;
    int content_type;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &objid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &insid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &resid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &type))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    value = cm_malloc(LWM2M_CMD_STR_LEN);
    if (value == NULL) {
        return CM_LWM2M_RET_CODE_MALLOC_FAIL;
    }
    memset(value, 0, LWM2M_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", value, LWM2M_CMD_STR_LEN))
    {
        cm_free(value);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &content_type))
    {
        cm_free(value);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_notify_packing(onenet_dev, objid, insid, resid, type, value, strlen(value), content_type);

    if (CM_LWM2M_SUCCESS != ret)
    {
        LWM2M_LOG("notify packing fail");
    }

    cm_uart_printf_urc("+LWM2MNOTIFYPACKING: %d", ret);
    cm_free(value);
    return 0;
}


// AT+LWM2M="cm_lwm2m_notify",mid
int func_cm_lwm2m_notify(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int mid;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &mid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_notify(onenet_dev, mid);

    if (CM_LWM2M_SUCCESS != ret)
    {
        LWM2M_LOG("notify fail");
    }

    cm_uart_printf_urc("+LWM2MNOTIFY: %d", ret);
    return 0;
}

// AT+LWM2M="cm_lwm2m_read_rsp",mid,result,objid,insid,resid,type,value
int func_cm_lwm2m_read_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int mid;
    int result;
    int objid;
    int insid;
    int resid;
    int type;
    char *value;
    int len;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &mid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &result))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &objid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &insid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &resid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &type))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    value = cm_malloc(LWM2M_CMD_STR_LEN);
    if (value == NULL) {
        return CM_LWM2M_RET_CODE_MALLOC_FAIL;
    }
    memset(value, 0, LWM2M_CMD_STR_LEN);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", value, LWM2M_CMD_STR_LEN))
    {
        cm_free(value);
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_read_rsp(onenet_dev, mid, result, objid, insid, resid, type, value, strlen(value));

    if (CM_LWM2M_SUCCESS != ret)
    {
        LWM2M_LOG("read rsp fail");
    }

    cm_uart_printf_urc("+LWM2MREADRSP: %d", ret);
    cm_free(value);
    return 0;
}

// AT+LWM2M="cm_lwm2m_write_rsp",mid,result
int func_cm_lwm2m_write_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int mid;
    int result;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &mid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &result))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_write_rsp(onenet_dev, mid, result);

    if (CM_LWM2M_SUCCESS != ret)
    {
        LWM2M_LOG("write rsp fail");
    }

    cm_uart_printf_urc("+LWM2MWRITERSP: %d", ret);
    return 0;
}

// AT+LWM2M="cm_lwm2m_execute_rsp",mid,result
int func_cm_lwm2m_execute_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int mid;
    int result;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &mid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &result))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_execute_rsp(onenet_dev, mid, result);

    if (CM_LWM2M_SUCCESS != ret)
    {
        LWM2M_LOG("execute rsp fail");
    }

    cm_uart_printf_urc("+LWM2MEXECUTERSP: %d", ret);
    return 0;
}

// AT+LWM2M="cm_lwm2m_param_rsp",mid,result
int func_cm_lwm2m_param_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int mid;
    int result;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &mid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &result))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_param_rsp(onenet_dev, mid, result);

    if (CM_LWM2M_SUCCESS != ret)
    {
        LWM2M_LOG("param rsp fail");
    }

    cm_uart_printf_urc("+LWM2MPARAMRSP: %d", ret);
    return 0;
}

// AT+LWM2M="cm_lwm2m_observe_rsp",mid,result
int func_cm_lwm2m_observe_rsp(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;

    int mid;
    int result;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &mid))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &result))
    {
        return CM_LWM2M_RET_CODE_PARAM_ERROR;
    }

    ret = cm_lwm2m_observe_rsp(onenet_dev, mid, result);

    if (CM_LWM2M_SUCCESS != ret)
    {
        LWM2M_LOG("observe rsp fail");
    }

    cm_uart_printf_urc("+LWM2MOBSERVERSP: %d", ret);
    return 0;
}


#endif /* end CM_DEMO_LWM2M_SUPPORT*/

