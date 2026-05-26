/*********************************************************
 *  @file    cm_demo_fota.c
 *  @brief   OpenCPU FOTA示例
 *  Copyright (c) 2025 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot1325 2025/3/25
 ********************************************************/

#include "stdio.h"
#include "string.h"
#include "cm_sys.h"
#include "cm_os.h"
#include "cm_demo_uart.h"
#include "cm_demo_fota.h"
#include "cm_http.h"
#include "cm_ota.h"

cm_httpclient_handle_t s_ota_http_handle = NULL;
static int ota_pkg_total_len = 0;

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
        cm_log_printf(0, "set total len %d\n", ota_pkg->total_len);
        ota_pkg_total_len = ota_pkg->total_len;
    }

    /* 若升级包总大小大于用于升级的flash空间大小，则升级无法完成，也不应该写入升级包 */
    if (ota_pkg_total_len > cm_ota_get_capacity())
    {
        cm_log_printf(0, "ota pkg too large %d %d\n", ota_pkg_total_len, cm_ota_get_capacity());
        return;
    }

    ret = cm_ota_firmware_write((char*)ota_pkg->response_content, ota_pkg->current_len);
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

/**
 *  升级包制作工具在tools\fota_tool，升级包制作方法可查看路径下的文档；
 *  本示例使用HTTP下载升级包；
 *  ota init                         //OTA初始化
 *  ota write_package                //OTA 下载/写入固件升级包
 *  ota upgrade                      //校验并升级，须在将OTA升级包写入后运行
 *  ota erase                        //擦除OTA升级包
 */
void cm_test_ota(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }
    
    unsigned char operation[20] = {0};
    sprintf((char *)operation, "%s", cmd);

    if (0 == strcmp((const char *)operation, "init"))          //初始化
    {
        cm_ota_init();
    }
    else if (0 == strcmp((const char *)operation, "write_package"))     //启动升级包下载和写入
    {
        const char *server_url = "http://xxx.com:8080";          //仅作为示例，url不可使用
        const char *url_path   = "/download/system_patch.bin";   //仅作为示例，url path不可使用
        if(s_ota_http_handle == NULL)
        {
            cm_httpclient_create((const uint8_t*)server_url, __cm_ota_download_cb, &s_ota_http_handle);//初始化并设置回调函数
        }

        if(s_ota_http_handle == NULL)//初始化失败
        {
            cm_demo_printf("http handhle init fail\n");
            return;
        }
        cm_httpclient_request_start(s_ota_http_handle, HTTPCLIENT_REQUEST_GET, (const uint8_t*)url_path, false, 0);
    }
    else if (0 == strcmp((const char *)operation, "upgrade"))                 //启动升级
    {
        cm_ota_upgrade();//开始升级，包括升级包校验、复位等操作
    }
    else if (0 == strcmp((const char *)operation, "erase"))                   //删除升级包
    {
        cm_ota_firmware_erase();
    }
    else if (0 == strcmp((const char *)operation, "res_size"))                //剩余空间大小查询
    {
        cm_demo_printf("file system res size[%d]\n", cm_ota_get_capacity());
    }
    else
    {
        cm_demo_printf("[OTA] Illegal operation\n"); 
    }
    return;
}
