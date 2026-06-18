/*********************************************************
 *  @file    cm_demo_fota.c
 *  @brief   OpenCPU FOTA示例
 *  Copyright (c) 2025 China Mobile IOT.
 *  All rights reserved.
 *  created by cmiot1325 2025/3/25
 ********************************************************/
#ifdef CM_DEMO_FOTA_SUPPORT
#include "stdio.h"
#include "string.h"
#include "cm_sys.h"
#include "cm_http.h"
#include "cm_ota.h"

#define UPGRADE_PACKAGE "/upgrade.bin"
#define cm_demo_printf osPrintf

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
        cm_demo_printf("[FOTA]set total\n");
        cm_ota_set_otasize(ota_pkg->total_len);
        ota_pkg_total_len = ota_pkg->total_len;
    }

    /* 若升级包总大小大于用于升级的flash空间大小，则升级无法完成，也不应该写入升级包 */
    if (ota_pkg_total_len > cm_ota_get_capacity())
    {
        cm_demo_printf("[FOTA]ota pkg too large %d %d\n", ota_pkg_total_len, cm_ota_get_capacity());
        return;
    }

    ret = cm_ota_firmware_write((const char*)ota_pkg->response_content, ota_pkg->current_len);
    if(ret)
    {
        return;
    }
    cm_demo_printf("[FOTA]Writed: %d/%d - %d%%\n",
        /*ota_pkg->sum_len*/cm_ota_get_written_size(),
        ota_pkg->total_len, ota_pkg->sum_len * 100 / ota_pkg->total_len);

    if(ota_pkg->sum_len >= ota_pkg->total_len)//下载完成
    {
        if(ota_pkg->sum_len > cm_ota_get_written_size())
        {
            cm_demo_printf("[FOTA]download ota pkg err!\n");
        }
        else
        {
            cm_demo_printf("[FOTA]download ota pkg complete!\n");
        }
    }
}

/**
 *  通用OTA功能调试使用示例
 *  本示例使用HTTP下载升级包；
 *  CM:OTA:INIT                         //OTA初始化
 *  CM:OTA:WRITE_PACKAGE                //OTA 下载/写入固件升级包至文件系统
 *  CM:OTA:UPGRADE                      //校验并升级，须在将OTA升级包写入文件系统后运行
 *  CM:OTA:ERASE                        //文件系统中擦除OTA升级包
 */
void cm_test_ota(unsigned char **cmd, char len)
{
    if(cmd == NULL || len < 3) /* 3:防止cmd与len不对应,后续最大访问argv[2]越界 */
    {
        cm_demo_printf("[FOTA]param error\n");
        return;
    }

    for(int i = 0; i < len; i++)
    {
        if(cmd[i] == NULL)
        {
            cm_demo_printf("[FOTA]param error\n");
            return;
        }
    }

    unsigned char operation[20] = {0};
    unsigned char url[256] = {0};
    snprintf((char *)url, sizeof(url), "%.*s", sizeof(url), cmd[1]);
    snprintf((char *)operation, sizeof(operation), "%.*s", sizeof(operation), cmd[2]);
    int ret = 0;

    if (0 == strcmp((const char *)operation, "INIT"))          //初始化
    {
        cm_demo_printf("[FOTA]ota update success\n");
        cm_ota_init();
    }
    else if (0 == strcmp((const char *)operation, "WRITE_PACKAGE"))          //文件系统OTA方式升级
    {
        /* 例如: char url[32] = "http://118.195.254.239:8080/fota.bin" */
        if(s_ota_http_handle == NULL)
        {
//            cm_httpclient_create((const uint8_t*)server_url, __cm_ota_download_cb, &s_ota_http_handle);//初始化并设置回调函数
            cm_httpclient_create((const uint8_t*)url, __cm_ota_download_cb, &s_ota_http_handle);//初始化并设置回调函数
        }

        if(s_ota_http_handle == NULL)//初始化失败
        {
            cm_demo_printf("[FOTA]http handhle init fail\n");
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
        cm_demo_printf("[FOTA]file system res size[%d]\n", cm_ota_get_capacity());
    }
    else
    {
        cm_demo_printf("[FOTA] Illegal operation\n");
    }
    return;
}

#if defined(OS_USING_SHELL)
#include <nr_micro_shell.h>
static void SHELL_ota_oc(char argc, char **argv)
{
    if(argv == NULL)
    {
        shell_printf("shell param error\r\n");
        return;
    }

    if(argc == 3 && argv[0] != NULL && argv[1] != NULL && argv[2] != NULL)
    {
        shell_printf("shell ota argv %s %s %s argc: %d\r\n", argv[0], argv[1], argv[2], argc);
        cm_test_ota(argv, argc);
    }
    else
    {
        shell_printf("-----------------ota help---------------------\r\n");
        shell_printf("1 ota_test OTA INIT\r\n");
        shell_printf("2 ota_test url WRITE_PACKAGE\r\n");
        shell_printf("3 ota_test OTA UPGRADE\r\n");
        shell_printf("4 ota_test OTA ERASE\r\n");
        shell_printf("5 ota_test OTA RES_SIZE\r\n");
        shell_printf("-----------------ota--------------------------\r\n");
    }
}

NR_SHELL_CMD_EXPORT(ota_test, SHELL_ota_oc);

#endif
#endif
