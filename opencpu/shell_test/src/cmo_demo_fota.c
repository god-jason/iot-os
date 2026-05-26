/*********************************************************
 *  @file    cm_demo_fota.c
 *  @brief   OpenCPU FOTA示例
 *  Copyright (c) 2022 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/8/29
 ********************************************************/
#ifdef CM_DEMO_FOTA_SUPPORT
#include "stdio.h"
#include "string.h"
#include "cm_fota.h"
#include "cm_sys.h"
#include "cm_fs.h"
#include "cm_http.h"
#include "cm_ota.h"

#define UPGRADE_PACKAGE "/upgrade.bin"
#define cm_demo_printf osPrintf

cm_httpclient_handle_t s_ota_http_handle = NULL;

#if ONENET_SWITCH
char OneNET_dowmload_header[256] = {0};                 //HTTP异步接口执行cm_httpclient_specific_header_set()时，需保证请求过程中不做修改和释放，如外部释放，故使用全局变量

/* 产品ID，平台创建产品后由平台分配的ID（https://open.iot.10086.cn/console/product/own）。此处为示例数据，实际使用须据实修改 */
static char OneNET_pid[16] = "07av4iD9CC";

/* 用户ID，平台网页中用户中心-账户信息（https://open.iot.10086.cn/account/profile）中查询到的用户ID。此处为示例数据，实际使用须据实修改 */
static char OneNET_userid[16] = "148865";

/* 设备名称，平台添加设备时添加设备的设备名称（https://open.iot.10086.cn/console/device/manage/devs）。此处为示例数据，实际使用须据实修改 */
static char OneNET_devname[16] = "ML307A-DSLN";

/* 用户Accesskey，平台网页中用户中心-访问权限（https://open.iot.10086.cn/account/access）中查询到的用户Accesskey。此处为示例数据，实际使用须据实修改 */
static char OneNET_userkey[128] = "xxxxxxx9acyjz4upifwnvbwuETqplZ0DEJ+9jTj2jWD0GzrDSrLu7/oqLmKIZk18SUPu6vQZDsaOrwtPethioA==";

/* 模组OneNET软件版本号，平台添加差分升级包时填写的待升级版本。此处为示例数据，实际使用须据实修改 */
static char OneNET_version[16] = "1.0";
#endif

static void __cm_fota_cb(cm_fota_error_e error)
{
    cm_demo_printf("[FOTA] error code is %d\n", error);
    if(CM_FOTA_CMIOT_ERROR_PKG_TYPE_FILESYSTEM == error)
        cm_demo_printf("[FOTA] This is not mini ota package!\n");

}

/**
 *  \brief http下载回调函数
 *
 *  \param [in] client_handle http句柄
 *  \param [in] event http回调类型
 *  \param [in] param http数据
 *
 *  \return None
 *
 *  \details 此示例下，http content内容即为差分包内容
 */
static void __cm_ota_download_cb(cm_httpclient_handle_t client_handle, cm_httpclient_callback_event_e event, void *param)
{
    (void)client_handle;
    int ret = 0;

    if(param == NULL || event != CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND)
    {
        return;
    }

    /*写入升级包*/
    cm_httpclient_callback_rsp_content_param_t *ota_pkg = (cm_httpclient_callback_rsp_content_param_t *)param;

    if(ota_pkg->current_len == ota_pkg->sum_len)
    {
        cm_log_printf(0, "set total\n");
        cm_ota_set_otasize(ota_pkg->total_len);
    }

    ret = cm_ota_firmware_write((const char*)ota_pkg->response_content, ota_pkg->current_len);
    if(ret)
    {
        return;
    }
    cm_log_printf(0, "Writed: %d/%d - %d%%\n",
        /*ota_pkg->sum_len*/cm_ota_get_written_size(),
        ota_pkg->total_len, ota_pkg->sum_len * 100 / ota_pkg->total_len);

    if(ota_pkg->sum_len >= ota_pkg->total_len)//下载完成
    {
        if(ota_pkg->sum_len > cm_ota_get_written_size())
        {
            cm_demo_printf("download ota pkg err!\n");
        }
        else
        {
            cm_demo_printf("download ota pkg complete!\n");
        }
    }
}

#if ONENET_SWITCH
static void __cm_ota_onenet_download_cb(cm_httpclient_handle_t client_handle, cm_httpclient_callback_event_e event, void *param)
{
    (void)client_handle;

    cm_demo_printf("__cm_ota_onenet_download_cb() event %d\n", event);

    if (CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND == event)
    {
        cm_httpclient_callback_rsp_content_param_t *ota_pkg = (cm_httpclient_callback_rsp_content_param_t *)param;

        cm_demo_printf("__cm_ota_onenet_download_cb() current_len %d, sum_len %d, total_len %d\n", ota_pkg->current_len, ota_pkg->sum_len, ota_pkg->total_len);

        if(ota_pkg->current_len == ota_pkg->sum_len)
        {
            cm_ota_set_otasize(ota_pkg->total_len);
        }

        int32_t ret = cm_ota_firmware_write((const char*)ota_pkg->response_content, ota_pkg->current_len);

        if(ret)
        {
            return;
        }

        cm_log_printf(0, "Writed: %d/%d - %d%%\n",
        /*ota_pkg->sum_len*/cm_ota_get_written_size(),
        ota_pkg->total_len, ota_pkg->sum_len * 100 / ota_pkg->total_len);

        if(ota_pkg->sum_len >= ota_pkg->total_len)//下载完成
        {
            if(ota_pkg->sum_len > cm_ota_get_written_size())
            {
                cm_demo_printf("download ota pkg err!\n");
            }
            else
            {
                cm_demo_printf("download ota pkg complete!\n");
            }
        }
    }
    else if (CM_HTTP_CALLBACK_EVENT_RSP_HEADER_IND == event)
    {
        cm_httpclient_callback_rsp_header_param_t *header = (cm_httpclient_callback_rsp_header_param_t *)param;

        //cm_demo_printf("__cm_ota_onenet_download_cb() response_header_len %d\n", header->response_header_len);
        cm_demo_printf("__cm_ota_onenet_download_cb() header: %s\n", header->response_header);
    }
}

/**
 * @brief 上报当前设备OneNET版本号
 *
 * @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 *  @details 所使用的OneNET接口地址为http://ota.heclouds.com/fuse-ota/{pro_id}/{dev_name}/version \n
 *           参考网页https://open.iot.10086.cn/doc/v5/fuse/detail/1454 \n
 *           与平台交互过程中使用鉴权信息会过期，开发使用过程中须保证在时间过期前完成升级。安全鉴权参考https://open.iot.10086.cn/doc/v5/fuse/detail/1464 \n
 *           因为OneNET平台会持续迭代，本demo示例在2023年6月5日可以正常使用OneNET FOTA功能，但不能保证后续OneNET平台版本迭代后还可使用 \n
 *           本接口为示例代码，开发人员可参考开发
 */
static int32_t __cm_onenet_fota_report_ver(void)
{
    int32_t ret = -1;

    cm_httpclient_handle_t client = NULL;
    ret = cm_httpclient_create((const uint8_t *)"http://ota.heclouds.com", NULL, &client);

    if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
    {
        cm_log_printf(0, "[OneNET] cm_httpclient_create() error!\r\n");
        return -1;
    }

    cm_httpclient_cfg_t client_cfg;
    client_cfg.ssl_enable = false;
    client_cfg.ssl_id = 0;
    client_cfg.cid = 0;
    client_cfg.conn_timeout = HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT;
    client_cfg.rsp_timeout = HTTPCLIENT_WAITRSP_TIMEOUT_DEFAULT;
    client_cfg.dns_priority = 0;

    ret = cm_httpclient_set_cfg(client, client_cfg);

    char path[128] = {0};
    char header[256] = {0};
    char message[128] = {0};
    char content[64] = {0};
    unsigned char base64_tmp[128] = {0};
    int32_t et = cm_rtc_get_current_time() + 2 * 24 * 60 * 60;

    sprintf(path, "/fuse-ota");
    sprintf(path + strlen((char*)path), "/%s/%s/version", OneNET_pid, OneNET_devname);

    /* 新接口必须要上报MCU版本号，OpenCPU开发通常不涉及MCU，故s_version传入任意值 */
    sprintf(content, "{\"s_version\": \"%s\",\"f_version\": \"%s\"}", "MCU1.0", OneNET_version);
    sprintf(header, "%ld\nsha1\nuserid/%s\n2022-05-01", et, OneNET_userid);

    int base64_len = 0;
    mbedtls_base64_decode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)OneNET_userkey, strlen(OneNET_userkey));

    mbedtls_md_context_t ctx = {0};
    const mbedtls_md_info_t *info = NULL;
    mbedtls_md_init(&ctx);

    info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    mbedtls_md_setup(&ctx, info, 1);
    mbedtls_md_hmac_starts(&ctx, (void *)base64_tmp, base64_len);
    mbedtls_md_hmac_update(&ctx, (void *)header, strlen(header));
    mbedtls_md_hmac_finish(&ctx, (void *)message);
    mbedtls_md_free(&ctx);
    mbedtls_base64_encode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)message, 20);//SHA-1 算法输出为20字节

    sprintf(header, "Authorization: version=2022-05-01&res=userid%s%s&et=%ld&method=sha1&sign=", "%2F", OneNET_userid, et);

    for(int i = 0; i < base64_len; i++)
    {
        if((base64_tmp[i] == '+') || (base64_tmp[i] == '/') || (base64_tmp[i] == '='))
        {
            sprintf(header+strlen(header), "%s", (base64_tmp[i] == '+')?("%2B"):((base64_tmp[i] == '/')?("%2F"):("%3D")));
        }
        else
        {
            sprintf(header+strlen(header), "%c", base64_tmp[i]);
        }
    }

    sprintf(header + strlen(header), "\r\nContent-Type:%s\r\n", "application/json");

    cm_httpclient_sync_response_t response = {};
    cm_httpclient_sync_param_t param = {HTTPCLIENT_REQUEST_POST, (const uint8_t *)path, strlen(content), (uint8_t *)content};

    cm_httpclient_specific_header_set(client, (uint8_t *)header, strlen(header));

    ret = cm_httpclient_sync_request(client, param, &response);

    if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
    {
        cm_log_printf(0, "[OneNET] cm_httpclient_sync_request() error! ret is %d\r\n", ret);

        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return -1;
    }
    else
    {
        cm_log_printf(0, "%s\r\n", response.response_content);
    }

    cJSON *root = cJSON_Parse((const char *)response.response_content);
    cJSON *code = cJSON_GetObjectItem(root, "code");

    if(code == NULL)
    {
        cJSON_Delete(root);
        cm_log_printf(0, "[OneNET] HTTP GET DATA ERROR\r\n");

        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return -1;
    }

    if(0 == code->valueint)
    {
        cJSON_Delete(root);
        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return 0;
    }
    else
    {
        cm_log_printf(0, "[OneNET] REPORT VERSION ERROR [%d]\r\n", code->valueint);

        cJSON_Delete(root);
        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return -1;
    }
}

/**
 * @brief 检测平台是否存在升级任务
 *
 * @return
 *   =  0 - 存在升级任务 \n
 *   = -1 - 失败或者不存在升级任务
 *
 *  @details 所使用的OneNET接口地址为http://ota.heclouds.com/fuse-ota/{pro_id}/{dev_name}/check \n
 *           参考网页https://open.iot.10086.cn/doc/v5/fuse/detail/1447 \n
 *           与平台交互过程中使用鉴权信息会过期，开发使用过程中须保证在时间过期前完成升级。安全鉴权参考https://open.iot.10086.cn/doc/v5/fuse/detail/1464 \n
 *           因为OneNET平台会持续迭代，本demo示例在2023年6月5日可以正常使用OneNET FOTA功能，但不能保证后续OneNET平台版本迭代后还可使用 \n
 *           本接口为示例代码，开发人员可参考开发
 */
static int32_t __cm_onenet_fota_check_upgrade_task(void)
{
    int32_t ret = -1;

    cm_httpclient_handle_t client = NULL;
    ret = cm_httpclient_create((const uint8_t *)"http://ota.heclouds.com", NULL, &client);

    if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
    {
        cm_log_printf(0, "[OneNET] cm_httpclient_create() error!\r\n");
        return -1;
    }

    cm_httpclient_cfg_t client_cfg;
    client_cfg.ssl_enable = false;
    client_cfg.ssl_id = 0;
    client_cfg.cid = 0;
    client_cfg.conn_timeout = HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT;
    client_cfg.rsp_timeout = HTTPCLIENT_WAITRSP_TIMEOUT_DEFAULT;
    client_cfg.dns_priority = 0;

    ret = cm_httpclient_set_cfg(client, client_cfg);

    char path[128] = {0};
    char header[256] = {0};
    char message[128] = {0};
    unsigned char base64_tmp[128] = {0};
    int32_t et = cm_rtc_get_current_time() + 2 * 24 * 60 * 60;

    sprintf(path, "/fuse-ota");
    sprintf(path + strlen((char*)path), "/%s/%s/check?type=1&version=%s", OneNET_pid, OneNET_devname, OneNET_version);

    sprintf(header, "%ld\nsha1\nuserid/%s\n2022-05-01", et, OneNET_userid);

    int base64_len = 0;
    mbedtls_base64_decode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)OneNET_userkey, strlen(OneNET_userkey));

    mbedtls_md_context_t ctx = {0};
    const mbedtls_md_info_t *info = NULL;
    mbedtls_md_init(&ctx);

    info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    mbedtls_md_setup(&ctx, info, 1);
    mbedtls_md_hmac_starts(&ctx, (void *)base64_tmp, base64_len);
    mbedtls_md_hmac_update(&ctx, (void *)header, strlen(header));
    mbedtls_md_hmac_finish(&ctx, (void *)message);
    mbedtls_md_free(&ctx);
    mbedtls_base64_encode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)message, 20);//SHA-1 算法输出为20字节

    sprintf(header, "Authorization: version=2022-05-01&res=userid%s%s&et=%ld&method=sha1&sign=", "%2F", OneNET_userid, et);

    for(int i = 0; i < base64_len; i++)
    {
        if((base64_tmp[i] == '+') || (base64_tmp[i] == '/') || (base64_tmp[i] == '='))
        {
            sprintf(header+strlen(header), "%s", (base64_tmp[i] == '+')?("%2B"):((base64_tmp[i] == '/')?("%2F"):("%3D")));
        }
        else
        {
            sprintf(header+strlen(header), "%c", base64_tmp[i]);
        }
    }

    sprintf(header + strlen(header), "\r\nContent-Type:%s\r\n", "application/json");

    cm_httpclient_sync_response_t response = {};
    cm_httpclient_sync_param_t param = {HTTPCLIENT_REQUEST_GET, (const uint8_t *)path, 0, NULL};

    cm_httpclient_specific_header_set(client, (uint8_t *)header, strlen(header));

    ret = cm_httpclient_sync_request(client, param, &response);

    if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
    {
        cm_log_printf(0, "[OneNET] cm_httpclient_sync_request() error! ret is %d\r\n", ret);

        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return -1;
    }
    else
    {
        cm_log_printf(0, "%s\r\n", response.response_content);
    }

    cJSON *root = cJSON_Parse((const char *)response.response_content);
    cJSON *code = cJSON_GetObjectItem(root, "code");

    if(code == NULL)
    {
        cJSON_Delete(root);
        cm_log_printf(0, "[OneNET] HTTP GET DATA ERROR\r\n");

        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return -1;
    }

    if(0 == code->valueint)
    {
        cJSON *data = cJSON_GetObjectItem(root, "data");

        if(data == NULL)
        {
            cm_log_printf(0, "[OneNET] HTTP GET data ERROR\r\n");

            cJSON_Delete(root);
            cm_httpclient_specific_header_free(client);
            cm_httpclient_terminate(client);
            cm_httpclient_delete(client);
            client = NULL;

            return -1;
        }

        cJSON *tid = cJSON_GetObjectItem(data, "tid");

        if(tid == NULL)
        {
            cm_log_printf(0, "[OneNET] HTTP GET tid ERROR\r\n");

            cJSON_Delete(root);
            cm_httpclient_specific_header_free(client);
            cm_httpclient_terminate(client);
            cm_httpclient_delete(client);
            client = NULL;

            return -1;
        }

        char c_tid[16] = {0};
        sprintf(c_tid, "%d", tid->valueint);

        /* 将tid写入文件系统，用于记录 */
        if (true == cm_fs_exist("onenet_tid.txt"))
        {
            cm_fs_delete("onenet_tid.txt");
        }

        int32_t fd = cm_fs_open("onenet_tid.txt", CM_FS_WB);

        int32_t f_len = cm_fs_write(fd, c_tid, strlen(c_tid));

        if (f_len != strlen(c_tid))
        {
            cm_log_printf(0, "[OneNET] FS WRITE tid ERROR\r\n");
        }

        cm_fs_close(fd);

        cJSON_Delete(root);
        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return 0;
    }
    else
    {
        cm_log_printf(0, "[OneNET] REPORT VERSION ERROR [%d]\r\n", code->valueint);

        cJSON_Delete(root);
        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return -1;
    }
}

/**
 * @brief 组包下载url地址，并通过cm_fota_set_url()完成配置
 *
 * @return
 *   =  0 - 成功 \n
 *   <  0 - 失败
 *
 * @details  与平台交互过程中使用鉴权信息会过期，开发使用过程中须保证在时间过期前完成升级。安全鉴权参考https://open.iot.10086.cn/doc/v5/fuse/detail/1464 \n
 *           因为OneNET平台会持续迭代，本demo示例在2023年6月5日可以正常使用OneNET FOTA功能，但不能保证后续OneNET平台版本迭代后还可使用 \n
 *           鉴权数据原为header中数据，因本模组平台仅可使用最小系统升级方案，不支持header配置，故需将header数据以参数形式添加进url中（OneNET平台支持该方式） \n
 *           本接口为示例代码，开发人员可参考开发
 */
static int32_t __cm_onenet_fota_set_url(void)
{
    char download_url[256] = {0};

    char c_tid[16] = {0};
    int32_t fd = cm_fs_open("onenet_tid.txt", CM_FS_RB);
    int32_t f_len = cm_fs_read(fd, c_tid, 32);
    cm_fs_close(fd);

    if (0 == f_len)
    {
        cm_log_printf(0, "[OneNET] no tid\r\n");
        return -1;
    }

    int32_t et = cm_rtc_get_current_time() + 2 * 24 * 60 * 60;

    sprintf(download_url, "http://ota.heclouds.com");
    sprintf(download_url + strlen((char*)download_url), "/fuse-ota/%s/%s/%s/download", OneNET_pid, OneNET_devname, c_tid);

    /* 鉴权数据原为header中数据，因本模组平台仅可使用最小系统升级方案，不支持header配置，故需将header数据以参数形式添加进url中（OneNET平台支持该方式） */
    char header[128] = {0};
    unsigned char base64_tmp[128] = {0};
    char message[128] = {0};
    int base64_len = 0;
    sprintf(header, "%ld\nsha1\nuserid/%s\n2022-05-01", et, OneNET_userid);

    mbedtls_base64_decode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)OneNET_userkey, strlen(OneNET_userkey));

    mbedtls_md_context_t ctx = {0};
    const mbedtls_md_info_t *info = NULL;
    mbedtls_md_init(&ctx);

    info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    mbedtls_md_setup(&ctx, info, 1);
    mbedtls_md_hmac_starts(&ctx, (void *)base64_tmp, base64_len);
    mbedtls_md_hmac_update(&ctx, (void *)header, strlen(header));
    mbedtls_md_hmac_finish(&ctx, (void *)message);
    mbedtls_md_free(&ctx);
    mbedtls_base64_encode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)message, 20);//SHA-1 算法输出为20字节

    /* 鉴权数据原为header中数据，因本模组平台仅可使用最小系统升级方案，不支持header配置，故需将header数据以参数形式添加进url中（OneNET平台支持该方式） */
    sprintf(download_url + strlen((char*)download_url), "?res=userid%s%s&et=%ld&method=sha1&sign=", "%2F", OneNET_userid, et);

    for(int i = 0; i < base64_len; i++)
    {
        if((base64_tmp[i] == '+') || (base64_tmp[i] == '/') || (base64_tmp[i] == '='))
        {
            sprintf(download_url+strlen(download_url), "%s", (base64_tmp[i] == '+')?("%2B"):((base64_tmp[i] == '/')?("%2F"):("%3D")));
        }
        else
        {
            sprintf(download_url+strlen(download_url), "%c", base64_tmp[i]);
        }
    }

    cm_log_printf(0, "[OneNET] download_url:%s\r\n", download_url);

    cm_demo_printf("[OneNET] download_url:%s\r\n", download_url);

    cm_fota_res_callback_register((cm_fota_result_callback)__cm_fota_cb);

    return cm_fota_set_url(download_url);
}

/**
 * @brief 上报当前设备升级完成状态
 *
 * @return
 *   =  0 - 存在升级任务 \n
 *   = -1 - 失败或者不存在升级任务
 *
 *  @details 所使用的OneNET接口地址为http://ota.heclouds.com/fuse-ota/{pro_id}/{dev_name}/{tid}/status \n
 *           参考网页https://open.iot.10086.cn/doc/v5/fuse/detail/1449 \n
 *           此接口示例用于模组升级完成时向平台上报升级完成状态，不含上报其余状态逻辑 \n
 *           与平台交互过程中使用鉴权信息会过期，开发使用过程中须保证在时间过期前完成升级。安全鉴权参考https://open.iot.10086.cn/doc/v5/fuse/detail/1464 \n
 *           因为OneNET平台会持续迭代，本demo示例在2023年6月5日可以正常使用OneNET FOTA功能，但不能保证后续OneNET平台版本迭代后还可使用 \n
 *           本接口为示例代码，开发人员可参考开发
 */
static int32_t __cm_onenet_fota_report_progress(void)
{
    int32_t ret = -1;

    char c_tid[16] = {0};
    int32_t fd = cm_fs_open("onenet_tid.txt", CM_FS_RB);
    int32_t f_len = cm_fs_read(fd, c_tid, 32);
    cm_fs_close(fd);

    if (0 == f_len)
    {
        cm_log_printf(0, "[OneNET] no tid\r\n");
        return -1;
    }

    cm_httpclient_handle_t client = NULL;
    ret = cm_httpclient_create((const uint8_t *)"http://ota.heclouds.com", NULL, &client);

    if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
    {
        cm_log_printf(0, "[OneNET] cm_httpclient_create() error!\r\n");
        return -1;
    }

    cm_httpclient_cfg_t client_cfg;
    client_cfg.ssl_enable = false;
    client_cfg.ssl_id = 0;
    client_cfg.cid = 0;
    client_cfg.conn_timeout = HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT;
    client_cfg.rsp_timeout = HTTPCLIENT_WAITRSP_TIMEOUT_DEFAULT;
    client_cfg.dns_priority = 0;

    ret = cm_httpclient_set_cfg(client, client_cfg);

    char path[128] = {0};
    char header[256] = {0};
    char message[128] = {0};
    char content[64] = "{\"step\":201}";        //升级成功，此时平台会把设备的版本号修改为任务的目标版本（设备状态变成：升级完成）
    unsigned char base64_tmp[128] = {0};
    int32_t et = cm_rtc_get_current_time() + 2 * 24 * 60 * 60;

    sprintf(path, "/fuse-ota");
    sprintf(path + strlen((char*)path), "/%s/%s/%s/status", OneNET_pid, OneNET_devname, c_tid);
    sprintf(header, "%ld\nsha1\nuserid/%s\n2022-05-01", et, OneNET_userid);

    int base64_len = 0;
    mbedtls_base64_decode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)OneNET_userkey, strlen(OneNET_userkey));

    mbedtls_md_context_t ctx = {0};
    const mbedtls_md_info_t *info = NULL;
    mbedtls_md_init(&ctx);

    info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    mbedtls_md_setup(&ctx, info, 1);
    mbedtls_md_hmac_starts(&ctx, (void *)base64_tmp, base64_len);
    mbedtls_md_hmac_update(&ctx, (void *)header, strlen(header));
    mbedtls_md_hmac_finish(&ctx, (void *)message);
    mbedtls_md_free(&ctx);
    mbedtls_base64_encode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)message, 20);//SHA-1 算法输出为20字节

    sprintf(header, "Authorization: version=2022-05-01&res=userid%s%s&et=%ld&method=sha1&sign=", "%2F", OneNET_userid, et);

    for(int i = 0; i < base64_len; i++)
    {
        if((base64_tmp[i] == '+') || (base64_tmp[i] == '/') || (base64_tmp[i] == '='))
        {
            sprintf(header+strlen(header), "%s", (base64_tmp[i] == '+')?("%2B"):((base64_tmp[i] == '/')?("%2F"):("%3D")));
        }
        else
        {
            sprintf(header+strlen(header), "%c", base64_tmp[i]);
        }
    }

    sprintf(header + strlen(header), "\r\nContent-Type:%s\r\n", "application/json");

    cm_httpclient_sync_response_t response = {};
    cm_httpclient_sync_param_t param = {HTTPCLIENT_REQUEST_POST, (const uint8_t *)path, strlen(content), (uint8_t *)content};

    cm_httpclient_specific_header_set(client, (uint8_t *)header, strlen(header));

    ret = cm_httpclient_sync_request(client, param, &response);                         //发送请求，同步接口

    if (CM_HTTP_RET_CODE_OK != ret || NULL == client)
    {
        cm_log_printf(0, "[OneNET] cm_httpclient_sync_request() error! ret is %d\r\n", ret);

        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return -1;
    }
    else
    {
        cm_log_printf(0, "%s\r\n", response.response_content);
    }

    cJSON *root = cJSON_Parse((const char *)response.response_content);
    cJSON *code = cJSON_GetObjectItem(root, "code");

    if(code == NULL)
    {
        cJSON_Delete(root);
        cm_log_printf(0, "[OneNET] HTTP GET DATA ERROR\r\n");

        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return -1;
    }

    if(0 == code->valueint)
    {
        cJSON_Delete(root);
        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return 0;
    }
    else
    {
        cm_log_printf(0, "[OneNET] REPORT VERSION ERROR [%d]\r\n", code->valueint);

        cJSON_Delete(root);
        cm_httpclient_specific_header_free(client);
        cm_httpclient_terminate(client);
        cm_httpclient_delete(client);
        client = NULL;

        return -1;
    }
}

/**
 * @brief 从OneNET平台下载升级包并写入模组
 *
 * @return
 *   =  0 - 成功 \n
 *   <  0 - 失败
 *
 * @details  因为OneNET平台会持续迭代，本demo示例在2023年8月2日可以正常使用OneNET FOTA功能，但不能保证后续OneNET平台版本迭代后还可使用 \n
 *           本接口为示例代码，开发人员可参考开发
 */
static int32_t __cm_onenet_fota_download(void)
{
    char download_url[64] = {0};
    char download_path[96] = {0};

    char c_tid[16] = {0};
    int32_t fd = cm_fs_open("onenet_tid.txt", CM_FS_RB);
    int32_t f_len = cm_fs_read(fd, c_tid, 32);
    cm_fs_close(fd);

    if (0 == f_len)
    {
        cm_log_printf(0, "[OneNET] no tid\r\n");
        return -1;
    }

    int32_t et = cm_rtc_get_current_time() + 2 * 24 * 60 * 60;

    sprintf(download_url, "http://ota.heclouds.com");
    sprintf(download_path, "/fuse-ota/%s/%s/%s/download", OneNET_pid, OneNET_devname, c_tid);

    unsigned char base64_tmp[128] = {0};
    char message[128] = {0};
    int base64_len = 0;

    memset(OneNET_dowmload_header, 0 ,256);
    sprintf(OneNET_dowmload_header, "%ld\nsha1\nuserid/%s\n2022-05-01", et, OneNET_userid);

    mbedtls_base64_decode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)OneNET_userkey, strlen(OneNET_userkey));

    mbedtls_md_context_t ctx = {0};
    const mbedtls_md_info_t *info = NULL;
    mbedtls_md_init(&ctx);

    info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    mbedtls_md_setup(&ctx, info, 1);
    mbedtls_md_hmac_starts(&ctx, (void *)base64_tmp, base64_len);
    mbedtls_md_hmac_update(&ctx, (void *)OneNET_dowmload_header, strlen(OneNET_dowmload_header));
    mbedtls_md_hmac_finish(&ctx, (void *)message);
    mbedtls_md_free(&ctx);
    mbedtls_base64_encode((unsigned char*)base64_tmp, 128, (unsigned int*)&base64_len, (const unsigned char*)message, 20);//SHA-1 算法输出为20字节

    sprintf(OneNET_dowmload_header, "Authorization: version=2022-05-01&res=userid%s%s&et=%ld&method=sha1&sign=", "%2F", OneNET_userid, et);

    for(int i = 0; i < base64_len; i++)
    {
        if((base64_tmp[i] == '+') || (base64_tmp[i] == '/') || (base64_tmp[i] == '='))
        {
            sprintf(OneNET_dowmload_header+strlen(OneNET_dowmload_header), "%s", (base64_tmp[i] == '+')?("%2B"):((base64_tmp[i] == '/')?("%2F"):("%3D")));
        }
        else
        {
            sprintf(OneNET_dowmload_header+strlen(OneNET_dowmload_header), "%c", base64_tmp[i]);
        }
    }

    if(s_ota_http_handle == NULL)
    {
        cm_httpclient_create((const uint8_t*)download_url, __cm_ota_onenet_download_cb, &s_ota_http_handle);//初始化并设置回调函数
    }

    if(s_ota_http_handle == NULL)//初始化失败
    {
        cm_demo_printf("http handhle init fail\n");
        return -1;
    }

    cm_httpclient_ret_code_e ret = cm_httpclient_specific_header_set(s_ota_http_handle, (uint8_t *)OneNET_dowmload_header, strlen(OneNET_dowmload_header));

    ret = cm_httpclient_request_start(s_ota_http_handle, HTTPCLIENT_REQUEST_GET, (const uint8_t*)download_path, false, 0);

    if (CM_HTTP_RET_CODE_OK == ret)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
#endif

extern int32_t cm_ota_get_written_size(void);
int32_t cm_ota_firmware_read(const char *buffer, size_t size)
{
    cm_fs_t fd = cm_fs_open(UPGRADE_PACKAGE, CM_FS_RB);
    if(fd == NULL)
    {
        cm_log_printf(0, "open %s fail\n", UPGRADE_PACKAGE);
        return -1;
    }

    int32_t read_len = cm_fs_read(fd, buffer, size);
    cm_log_printf(0, "read_len %d, size %d\n", read_len, size);
    if(read_len != size)
    {
        if(fd)
            cm_fs_close(fd);
        return -1;
    }
    if(fd)
        cm_fs_close(fd);
    return 0;
}

/**
 *  UART口FOTA功能调试使用示例
 *  CM:FOTA:SETURL                       //HTTP FOTA完整包升级方式，配置HTTP服务器信息（仅为示例，配置的url不可使用）
 *  CM:FOTA:ONENET:operation             //从OneNET平台获取和配置下载升级包的url，operation为操作参数，详见下方代码
 *  CM:FOTA:UPGRADE                      //升级，须在完成配置后使用
 */
void cm_test_fota(unsigned char **cmd,int len)
{
    unsigned char operation[20] = {0};
    sprintf((char *)operation, "%s", cmd[2]);
    int ret = 0;

    if (0 == strcmp((const char *)operation, "SETURL"))          //HTTP FOTA完整包升级方式，配置FOTA信息
    {
        char *url = (char *)malloc(256);
        if(url == NULL)
        {
            cm_log_printf(0, "memory malloc fail\r\n");
            return;
        }
        memset(url, 0, 256);
        snprintf((char *)url, 256,"%s", cmd[1]);
        cm_fota_res_callback_register((cm_fota_result_callback)__cm_fota_cb);
        /* 1.设置url */
        ret = cm_fota_set_url(url);
        if (0 != ret){
            cm_log_printf(0, "cm_fota_set_url error\r\n");
            free(url);
            return;
        }
        else
        {
            cm_log_printf(0, "cm_fota_set_url 123succeed\r\n");
            free(url);
        }
    }
    else if (0 == strcmp((const char *)operation, "UPGRADE"))                 //启动升级
    {
        cm_fs_system_info_t fs_info = {0, 0};
        cm_fs_getinfo(&fs_info);

        /* 文件系统剩余空间为0时不建议执行升级，关键文件系统操作可能会失败 */
        if (0 == fs_info.free_size)
        {
            cm_log_printf(0, "insufficient space left in the file system");
            return;
        }

        cm_fota_info_t info = {0};
        cm_fota_read_config(&info);
        if (CM_FOTA_TYPE_HTTP_HTTPS == info.fixed_info.fota_mode)
        {
            cm_log_printf(0, "HTTP server [%s]", info.fixed_info.url);
        }
        else
        {
            cm_log_printf(0, "HTTP server error");
            return;
        }

        osDelay(200);

        cm_fota_exec_upgrade();
    }
    else if (0 == strcmp((const char *)operation, "READ"))                 //读升级包内容
    {
        int32_t file_size = cm_ota_get_written_size();
        if(file_size <= 0)
        {
            cm_log_printf(0, "get firmware size fail");
            return;
        }
        char *buffer = os_malloc(file_size+1);
        if(buffer == NULL)
        {
            cm_log_printf(0, "malloc error");
            return;
        }
        memset(buffer, 0, file_size+1);
        int32_t ret = cm_ota_firmware_read(buffer, file_size);
        if(ret < 0)
        {
            cm_log_printf(0, "read firmware error");
            os_free(buffer);
            return;
        }
        /* 显示固件内容 */
        osPrintf("firmware content\r\n");
        for(int i = 0; i < file_size; i++)
            osPrintf("%02x ", buffer[i]);
        osPrintf("\r\nend\r\n");
        os_free(buffer);
    }
    #if ONENET_SWITCH
    else if (0 == strcmp((const char *)operation, "ONENET"))                 //使用OneNET FOTA服务程序示例
    {
        /* 使用本示例需要用户将OneNET_pid、OneNET_userid、OneNET_devname、OneNET_userkey、OneNET_version填写为用户账号及产品的真实内容 */
        /* OneNET HTTP升级必要步骤参考链接https://open.iot.10086.cn/doc/v5/fuse/detail/1204 */
        /* 与平台交互过程中使用鉴权信息会过期，开发使用过程中须保证在时间过期前完成升级。安全鉴权参考https://open.iot.10086.cn/doc/v5/fuse/detail/1464 */

        /* OneNET FOTA（模组采用最小系统升级方案）服务步骤如下 */
        /* 步骤1：上报当前版本号（测试指令CM:FOTA:ONENET:VERSION） */
        /* 步骤2：增值服务-OTA升级（https://open.iot.10086.cn/console/valueAdded/ota/package）页面添加升级包（差分包），批量升级，跳过验证。该步骤在平台侧网页完成https://open.iot.10086.cn/doc/v5/fuse/detail/1068 */
        /* 步骤3：检测升级任务并调用cm_fota_set_url()接口配置升级url（测试指令CM:FOTA:ONENET:CHECK） */
        /* 步骤4：下载升级包并升级固件，此步由最小系统完成（测试指令CM:FOTA:UPGRADE） */
        /* 步骤5：（待模组升级完成并重启后）上报升级完成状态（测试指令CM:FOTA:ONENET:PROGRESS） */

        /* 因为OneNET平台会持续迭代，本demo示例在2023年6月5日可以正常使用OneNET FOTA功能，但不能保证后续OneNET平台版本迭代后还可使用 */

        unsigned char onenet_operation[20] = {0};
        sprintf((char *)onenet_operation, "%s", cmd[3]);

        if (0 == strcmp((const char *)onenet_operation, "VERSION"))
        {
            ret = __cm_onenet_fota_report_ver();

            if (0 == ret)
            {
                cm_demo_printf("[OneNET] REPORT VERSION SUCC\r\n");
            }
            else
            {
                cm_demo_printf("[OneNET] REPORT VERSION ERROR\r\n");
            }
        }
        else if (0 == strcmp((const char *)onenet_operation, "CHECK"))
        {
            ret = __cm_onenet_fota_check_upgrade_task();

            if (0 == ret)
            {
                cm_demo_printf("[OneNET] UPGRADE TASK IS EXIST\r\n");

                ret = __cm_onenet_fota_set_url();

                if (0 == ret)
                {
                    cm_demo_printf("[OneNET] __cm_onenet_fota_set_url SUCC\r\n");
                }
                else
                {
                    cm_demo_printf("[OneNET] __cm_onenet_fota_set_url ERROR\r\n");
                }
            }
            else
            {
                cm_demo_printf("[OneNET] CHEAK ERROR OR UPGRADE TASK IS NOT EXIST\r\n");
            }
        }
        else if (0 == strcmp((const char *)onenet_operation, "PROGRESS"))
        {
            ret = __cm_onenet_fota_report_progress();

            if (0 == ret)
            {
                cm_demo_printf("[OneNET] REPORT PROGRESS SUCC\r\n");
            }
            else
            {
                cm_demo_printf("[OneNET] REPORT PROGRESS ERROR\r\n");
            }
        }
        else
        {
            cm_demo_printf("[FOTA] Illegal operation\n");
        }
    }
    #endif
    else
    {
        cm_demo_printf("[FOTA] Illegal operation\n");
    }
    return;
}

/**
 *  通用OTA功能调试使用示例，使用cm_ota.h中的通用OTA接口实现通用OTA功能，用户可将升级包用接口写入文件系统，需要升级时再调用升级接口；
 *  包含全量差分升级以及app整包升级，升级包大小加备份文件预留空间大小不可大于当前可用文件系统剩余空间(可用cm_fs_getinfo或cm_ota_get_capacity查询)，否则将无法写入；
 *  升级包制作工具在tools\fota_tool，升级包制作以及备份文件预留空间大小请查看tools\fota_tool\ota_package_make.txt；
 *  本示例使用HTTP下载升级包；
 *  CM:OTA:INIT                         //OTA初始化
 *  CM:OTA:WRITE_PACKAGE                //OTA 下载/写入固件升级包至文件系统
 *  CM:OTA:UPGRADE                      //校验并升级，须在将OTA升级包写入文件系统后运行
 *  CM:OTA:ERASE                        //文件系统中擦除OTA升级包
 *  CM:OTA:ONENET:operation             //从OneNET平台获取和配置下载升级包的url，operation为操作参数，详见下方代码
 */
void cm_test_ota(unsigned char **cmd,int len)
{
    unsigned char operation[20] = {0};
    unsigned char url[256] = {0};
    sprintf((char *)url, "%s", cmd[1]);
    sprintf((char *)operation, "%s", cmd[2]);
    int ret = 0;

    if (0 == strcmp((const char *)operation, "INIT"))          //初始化
    {
        cm_demo_printf("ota update success\n");
        cm_ota_init();
    }
    else if (0 == strcmp((const char *)operation, "WRITE_PACKAGE"))          //文件系统OTA方式升级
    {
//        const char *server_url = "http://xxx.com:8080";          //仅作为示例，url不可使用
//        const char *url_path   = "/download/system_patch.bin";   //仅作为示例，url path不可使用

        if(s_ota_http_handle == NULL)
        {
//            cm_httpclient_create((const uint8_t*)server_url, __cm_ota_download_cb, &s_ota_http_handle);//初始化并设置回调函数
            cm_httpclient_create((const uint8_t*)url, __cm_ota_download_cb, &s_ota_http_handle);//初始化并设置回调函数
        }

        if(s_ota_http_handle == NULL)//初始化失败
        {
            cm_demo_printf("http handhle init fail\n");
            return;
        }
//        cm_httpclient_request_start(s_ota_http_handle, HTTPCLIENT_REQUEST_GET, (const uint8_t*)url_path, false, 0);
        cm_httpclient_request_start(s_ota_http_handle, HTTPCLIENT_REQUEST_GET, (const uint8_t*)url, false, 0);
    }
    else if (0 == strcmp((const char *)operation, "UPGRADE"))                 //启动升级
    {
        cm_ota_upgrade();//开始升级，包括升级包校验、复位等操作
    }
    else if (0 == strcmp((const char *)operation, "ERASE"))                   //从文件系统删除升级包
    {
        cm_ota_firmware_erase();
    }
    else if (0 == strcmp((const char *)operation, "RES_SIZE"))                //文件系统剩余空间大小查询
    {
        cm_demo_printf("file system res size[%d]\n", cm_ota_get_capacity());
    }
    #if ONENET_SWITCH
    else if (0 == strcmp((const char *)operation, "ONENET"))                 //使用OneNET FOTA服务程序示例
    {
        /* 使用本示例需要用户将OneNET_pid、OneNET_userid、OneNET_devname、OneNET_userkey、OneNET_version填写为用户账号及产品的真实内容 */
        /* OneNET HTTP升级必要步骤参考链接https://open.iot.10086.cn/doc/v5/fuse/detail/1204 */
        /* 与平台交互过程中使用鉴权信息会过期，开发使用过程中须保证在时间过期前完成升级。安全鉴权参考https://open.iot.10086.cn/doc/v5/fuse/detail/1464 */
        /* 使用前须确保用于升级的文件系统空间大于升级包大小，即当前文件系统能够存放升级包文件 */

        /* OneNET FOTA（模组采用全系统差分升级方案或者APP整包升级方案）服务步骤如下 */
        /* 步骤1：初始化OTA（测试指令CM:OTA:INIT） */
        /* 步骤2：上报当前版本号（测试指令CM:OTA:ONENET:VERSION） */
        /* 步骤3：增值服务-OTA升级（https://open.iot.10086.cn/console/valueAdded/ota/package）页面添加升级包（差分包），批量升级，跳过验证。该步骤在平台侧网页完成https://open.iot.10086.cn/doc/v5/fuse/detail/1068 */
        /* 步骤4：检测升级任务并下载（测试指令CM:OTA:ONENET:DOWNLOAD */
        /* 步骤5：升级固件（测试指令CM:OTA:UPGRADE） */
        /* 步骤6：（待模组升级完成并重启后）上报升级完成状态（测试指令CM:OTA:ONENET:PROGRESS） */

        /* 因为OneNET平台会持续迭代，本demo示例在2023年8月2日可以正常使用OneNET FOTA功能，但不能保证后续OneNET平台版本迭代后还可使用 */

        unsigned char onenet_operation[20] = {0};
        sprintf((char *)onenet_operation, "%s", cmd[3]);

        if (0 == strcmp((const char *)onenet_operation, "VERSION"))
        {
            ret = __cm_onenet_fota_report_ver();

            if (0 == ret)
            {
                cm_demo_printf("[OneNET] REPORT VERSION SUCC\r\n");
            }
            else
            {
                cm_demo_printf("[OneNET] REPORT VERSION ERROR\r\n");
            }
        }
        else if (0 == strcmp((const char *)onenet_operation, "DOWNLOAD"))
        {
            ret = __cm_onenet_fota_check_upgrade_task();

            osDelay(40);

            if (0 == ret)
            {
                cm_demo_printf("[OneNET] UPGRADE TASK IS EXIST\r\n");

                ret = __cm_onenet_fota_download();

                if (0 == ret)
                {
                    cm_demo_printf("[OneNET] __cm_onenet_fota_download SUCC\r\n");
                }
                else
                {
                    cm_demo_printf("[OneNET] __cm_onenet_fota_download ERROR\r\n");
                }
            }
            else
            {
                cm_demo_printf("[OneNET] CHEAK ERROR OR UPGRADE TASK IS NOT EXIST\r\n");
            }
        }
        else if (0 == strcmp((const char *)onenet_operation, "PROGRESS"))
        {
            ret = __cm_onenet_fota_report_progress();

            if (0 == ret)
            {
                cm_demo_printf("[OneNET] REPORT PROGRESS SUCC\r\n");
            }
            else
            {
                cm_demo_printf("[OneNET] REPORT PROGRESS ERROR\r\n");
            }
        }
        else
        {
            cm_demo_printf("[FOTA] Illegal operation\n");
        }
    }
    #endif
    else
    {
        cm_demo_printf("[OTA] Illegal operation\n");
    }
    return;
}

#if defined(OS_USING_SHELL)
#include <nr_micro_shell.h>

static void SHELL_ota_oc(char argc, char **argv)
{
    if(argc == 3)
    {
        shell_printf("shell ota argv %s %s %s argc: %d\r\n", argv[0], argv[1], argv[2], argc);
        cm_test_ota(argv, 0);
    }
    else if(argc == 2) /* help */
    {
        if (0 == strcmp(argv[1], "help"))
        {
            shell_printf("-----------------ota help---------------------\r\n");
            shell_printf("1 cm_ota OTA INIT\r\n");
            shell_printf("2 cm_ota url WRITE_PACKAGE\r\n");
            shell_printf("3 cm_ota OTA UPGRADE\r\n");
            shell_printf("4 cm_ota OTA ERASE\r\n");
            shell_printf("5 cm_ota OTA RES_SIZE\r\n");
            shell_printf("-----------------ota--------------------------\r\n");
        }
        else
            shell_printf("ota parameter invalid\r\n");
    }
    else
        shell_printf("ota parameter invalid\r\n");
}

static void SHELL_fota_oc(char argc, char **argv)
{
    if(argc == 3)
    {
        shell_printf("shell cm_fota argv %s %s %s argc: %d\r\n", argv[0], argv[1], argv[2], argc);
        cm_test_fota(argv, 0);
    }
    else if(argc == 2) /* help */
    {
        if (0 == strcmp(argv[1], "help"))
        {
            shell_printf("-----------------cm_fota help---------------------\r\n");
            shell_printf("1 cm_fota url SETURL\r\n");
            shell_printf("2 cm_fota OTA UPGRADE\r\n");
            shell_printf("3 cm_fota OTA READ\r\n");
            shell_printf("-----------------cm_fota--------------------------\r\n");
        }
        else
            shell_printf("fota parameter invalid\r\n");
    }
    else
        shell_printf("fota parameter invalid\r\n");

}

NR_SHELL_CMD_EXPORT(cm_ota, SHELL_ota_oc);
NR_SHELL_CMD_EXPORT(cm_fota, SHELL_fota_oc);

#endif
#endif
