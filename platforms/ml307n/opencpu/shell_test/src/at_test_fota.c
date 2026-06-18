/*********************************************************
 *  @file    at_test_fota.c
 *  @brief   OpenCPU FOTA/OTA示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by guoliangke 2025/1/21
 ********************************************************/
#ifdef CM_DEMO_FOTA_SUPPORT

#include <os.h>
#include <stdlib.h>

#include "at_api.h"
#include "at_parser.h"
#include "cm_ota.h"
#include "cm_fota.h"
#include "cm_demo_common.h"
#include "cm_mem.h"
#include "cm_fs.h"

#define CM_OTA_SUCCESS 0

typedef AT_CommandItem at_cmd_t;

#define OTA_LOG(fmt, ...)   osPrintf("[OC_LWM2M_TEST][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define OTA_CMD_STR_LEN 256

#define CM_OTA_RET_CODE_OK 0 //成功
#define CM_OTA_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_OTA_RET_CODE_PARAM_ERROR 50 //参数错误 50

/* fota/ota 错误码 */
#define CM_OTA_UNKOWN_ERROR 850    //未知错误
#define CM_OTA_CFG_ERROR 851       //设置参数错误
#define CM_OTA_URL_ERROR 852       //设置url失败
#define CM_OTA_WRITE_ERROR 853     //写入或者获取升级包失败
#define CM_OTA_UPGRADE_ERROR 854   //升级失败
#define CM_OTA_DOWNLOAD_ERROR 855  //下载升级包失败
#define CM_OTA_ERASE_ERROR 857     //擦除升级包失败


#define CM_TEST_MAX_OTA_TIMEOUT  180
#define CM_TEST_MAX_OTA_SIZE  (448*1024)
#define FOTA_TASK_PRIORITY  osPriorityNormal
static osSemaphoreId_t g_test_uart_semaphore = NULL;
static osThreadId_t g_fota_threadId = NULL;
static os_uint8_t g_fota_id = 0;

typedef void (*cm_datamode_cb_t)(void *data, int data_len, void *arg, int event);

typedef struct
{
    UINT32 at_handle;
    int mode;          //0 透传模式；1 数据模式不检测1A，1B； 2 数据模式检测1A，1B
    int length;        //输入发送最大长度
    int recv_len;      //已接收长度
    int timeout;       //超时时间，单位100ms
    void *arg;         //可用作各应用内部ID区分，如TCPIP 0-5
    void *timer;       //超时定时器
    void *wait;        //内部等待定时器
    cm_datamode_cb_t datamode_cb;
} cm_datamode_ctx_t;
typedef enum
{
    CM_DATA_MODE_NORMAL = 1, //数据到来
    CM_DATA_MODE_RECVALL,    //接收完成
    CM_DATA_MODE_TIMEOUT,    //超时
    CM_DATA_MODE_WAITOUT,    //内部定时器超时
    CM_DATA_MODE_ESCAPE,     //+++
    CM_DATA_MODE_DEINIT,
    CM_DATA_MODE_RETURN,
} cm_datamode_event_e;
typedef enum
{
    CM_TRAN_MODE = 0, //透传模式
    CM_DATA_MODE1,    //数据模式不检测1A，1B
    CM_DATA_MODE2,    //数据模式检测1A，1B
    CM_NONE_MODE,
} cm_data_mode_e;

void cm_test_take_uart_semaphore(void)
{
    if (g_test_uart_semaphore)
    {
        if (osSemaphoreAcquire(g_test_uart_semaphore, osWaitForever) != osOK)
        {
            cm_assert(0);
        }
    }
}

void cm_test_give_uart_semaphore(void)
{
    if (g_test_uart_semaphore)
    {
        if (osSemaphoreRelease(g_test_uart_semaphore) != osOK)
        {
            cm_assert(0);
        }
    }
}

void cm_test_uart_semaphore_create(void)
{
    if (g_test_uart_semaphore == NULL)
    {
       g_test_uart_semaphore = osSemaphoreNew(1, 1, NULL);
    }
}

static void __cm_test_write_datamode_cb(void *data, int data_len, void *arg, int event)
{
    cm_datamode_ctx_t *cm_datamode_ctx = arg;
    int is_send = 0;
    int ret = 0;
    uint8_t data_buffer[520] = {0};

    for(int i = 0; i < data_len; i++)
        OTA_LOG("%02x ", ((uint8_t*)data)[i]);
    OTA_LOG("\r\n");

    if ((data != NULL) && (data_len != 0))
    {
        memcpy(data_buffer, data, data_len);
    }

    OTA_LOG("recv data_len[%d]", data_len);

    if (event != CM_DATA_MODE_WAITOUT)
    {
        OTA_LOG("__cm_tcpip_send_tr_cb %d %d %d %d %d\r\n",
                   event, (int)cm_datamode_ctx->arg, data_len,
                   (int)cm_datamode_ctx->recv_len, cm_datamode_ctx->length);
    }

    if ((event == CM_DATA_MODE_NORMAL) || (event == CM_DATA_MODE_RECVALL))
    {
        if ((data_len == 1) && (cm_datamode_ctx->mode == CM_DATA_MODE2) &&
            ((*(char *)data == 0X1A) || (*(char *)data == 0X1B)))
        {
            is_send = 1;
        }
        else
        {
            cm_test_take_uart_semaphore();
            ret = cm_ota_firmware_write((uint8_t *)data_buffer, data_len);
            cm_test_give_uart_semaphore();
        }
    }

    if ((event == CM_DATA_MODE_TIMEOUT) || (event == CM_DATA_MODE_RECVALL) || ret < 0)
    {
        is_send = 1;
    }

    if (is_send)
    {
        cm_datamode_deinit(cm_datamode_ctx->at_handle);
        if (ret >= 0)
        {
           cm_uart_printf_result(cm_datamode_ctx->at_handle, 0);
        }
        else
        {
            cm_uart_printf_result(cm_datamode_ctx->at_handle, 853);
        }
    }
}

void ota_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
void fota_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);

int func_cm_ota_init(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ota_firmware_write(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara, os_uint32_t para_count);
int func_cm_ota_get_written_size(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ota_firmware_erase(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ota_upgrade(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ota_set_otasize(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_ota_get_capacity(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);

int func_cm_fota_set_url(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_fota_set_reboot_time(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_fota_read_config(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_fota_res_callback_register(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);
int func_cm_fota_exec_upgrade(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara);


AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_ota_oc[] =
{
    {.name = "+OTA",   .setFunc = ota_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
    {.name = "+FOTA",   .setFunc = fota_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

// AT+OTA="XX", param1, param2, ...
// "XX" 为函数名
void ota_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;
    os_uint32_t error_no = 0;

    OTA_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$,$,$");
        OTA_LOG("para_count[%d]", para_count);
        if (para_count < 1 || para_count > 3)
        {
            ret = CM_OTA_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            OTA_LOG("func_name[%s]", func_name);
            ret = CM_OTA_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_ota_init") == 0)
        {
            ret = func_cm_ota_init(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ota_firmware_write") == 0)
        {
            if((para_count < 2 || para_count > 3))
            {
                OTA_LOG("param error para_count[%u]", para_count);
                ret = CM_OTA_RET_CODE_PARAM_ERROR;
                break;
            }

            ret = func_cm_ota_firmware_write(id, (os_uint8_t *)para_p_new, lenOfPara_new, para_count);
            if(para_count == 2 && ret == 0)
                return;
        }
        else if (strcmp(func_name, "cm_ota_get_written_size") == 0)
        {
            ret = func_cm_ota_get_written_size(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ota_firmware_erase") == 0)
        {
            ret = func_cm_ota_firmware_erase(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ota_upgrade") == 0)
        {
            ret = func_cm_ota_upgrade(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ota_set_otasize") == 0)
        {
            ret = func_cm_ota_set_otasize(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_ota_get_capacity") == 0)
        {
            ret = func_cm_ota_get_capacity(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            OTA_LOG("Error func name[%s]", func_name);
            ret = CM_OTA_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}

// AT+FOTA="XX", param1, param2, ...
// "XX" 为函数名
void fota_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;
    os_uint32_t para_count = 0;
    os_uint32_t error_no = 0;

    OTA_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        xs_at_parse_para_count_cm(&para_p_new, &lenOfPara_new, &para_count, "$,$,$,$,$,$,$,$");
        OTA_LOG("para_count[%u]", para_count);
        if(para_count < 1 || para_count > 2)
        {
            ret = CM_OTA_RET_CODE_PARAM_ERROR;
            break;
        }

        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[128] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 128))
        {
            OTA_LOG("func_name[%s]", func_name);
            ret = CM_OTA_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_fota_set_url") == 0)
        {
            ret = func_cm_fota_set_url(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_fota_set_reboot_time") == 0)
        {
            OTA_LOG("Error func name[%s]", func_name);
            ret = CM_OTA_RET_CODE_PARAM_ERROR;
        }
        else if (strcmp(func_name, "cm_fota_read_config") == 0)
        {
            ret = func_cm_fota_read_config(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_fota_res_callback_register") == 0)
        {
            OTA_LOG("Error func name[%s]", func_name);
            ret = CM_OTA_RET_CODE_PARAM_ERROR;
        }
        else if (strcmp(func_name, "cm_fota_exec_upgrade") == 0)
        {
            ret = func_cm_fota_exec_upgrade(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            OTA_LOG("Error func name[%s]", func_name);
            ret = CM_OTA_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}

/********************************************** ota function ******************************************/
// AT+OTA="cm_ota_init"
int func_cm_ota_init(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    ret = cm_ota_init();

    if (ret != CM_OTA_SUCCESS)
    {
        OTA_LOG("cm_ota_init fail");
        return CM_OTA_UNKOWN_ERROR;
    }

    return 0;
}

// AT+OTA="cm_ota_firmware_write",length,data
int func_cm_ota_firmware_write(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara, os_uint32_t para_count)
{
    uint8_t *sdata = NULL;
    uint8_t *data = NULL;
    int32_t ret = 0;
    os_uint32_t write_length = 0;
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &write_length))
    {
        OTA_LOG("write_length[%u]", write_length);
        return CM_OTA_RET_CODE_PARAM_ERROR;
    }

    if(write_length == 0)
    {
        OTA_LOG("param error len = 0\r\n");
        return CM_OTA_RET_CODE_PARAM_ERROR;
    }

    OTA_LOG("write_length:%u\r\n", write_length);

    if(para_count == 2)   //数据模式
    {
        cm_test_uart_semaphore_create();

        cm_datamode_init(id,
                        write_length ? CM_DATA_MODE1 : CM_DATA_MODE2,
                        write_length ? write_length : CM_TEST_MAX_OTA_SIZE,
                        CM_TEST_MAX_OTA_TIMEOUT,
                        __cm_test_write_datamode_cb,
                        NULL);

        cm_printf_ch_noCRLF(id ,"\r\n> ");
        return 0;
    }

    data = cm_malloc(1024+1);
    if(data == NULL)
    {
        OTA_LOG("malloc fail\r\n");
        return CM_OTA_RET_CODE_MALLOC_FAIL;
    }
    memset(data, 0, 1024+1);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s", data))
    {
        OTA_LOG("param error\r\n");
        goto EXIT;
    }

    if((write_length > 512) || ((int)strlen(data) != (write_length*2)))
    {
        OTA_LOG("param error\r\n");
        goto EXIT;
    }

    sdata = cm_malloc(write_length+1);
    if(sdata == NULL)
    {
        OTA_LOG("malloc fail\r\n");
        goto EXIT;
    }
    memset(sdata, 0, write_length+1);

    if(cm_util_str2hex(data, (char*)sdata, write_length*2) == 0)
    {
        OTA_LOG("str2hex fail\r\n");
        goto EXIT;
    }

    ret = cm_ota_firmware_write(sdata, write_length);
    if (ret != CM_OTA_SUCCESS)
    {
        OTA_LOG("cm_ota_firmware_write fail");
        if(data)
            cm_free(data);
        if(sdata)
            cm_free(sdata);
        return CM_OTA_WRITE_ERROR;
    }

    if(data)
        cm_free(data);
    if(sdata)
        cm_free(sdata);
    return 0;
EXIT:
    if(data)
        cm_free(data);
    if(sdata)
        cm_free(sdata);
     return CM_OTA_RET_CODE_PARAM_ERROR;
}

// AT+OTA="cm_ota_get_written_size"
int func_cm_ota_get_written_size(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = 0;
    ret = cm_ota_get_written_size();

    if (ret < CM_OTA_SUCCESS)
    {
        OTA_LOG("cm_ota_get_written_size fail");
        return CM_OTA_WRITE_ERROR;
    }

    cm_uart_printf_urc("+OTA_GET_WRITTEN_SIZE: %d", ret);
    return 0;
}

// AT+OTA="cm_ota_firmware_erase"
int func_cm_ota_firmware_erase(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = 0;
    ret = cm_ota_firmware_erase();

    if (ret != CM_OTA_SUCCESS)
    {
        OTA_LOG("cm_ota_firmware_erase fail");
        return CM_OTA_ERASE_ERROR;
    }

    return 0;
}

// AT+OTA="cm_ota_upgrade"
int func_cm_ota_upgrade(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = 0;
    ret = cm_ota_upgrade();

    if (ret != CM_OTA_SUCCESS)
    {
        OTA_LOG("cm_ota_upgrade fail");
        return CM_OTA_UPGRADE_ERROR;
    }

    return 0;
}

// AT+OTA="cm_ota_set_otasize",upgrade_package_total_length
int func_cm_ota_set_otasize(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int total_length = 0;
    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &total_length))
    {
        OTA_LOG("total_length[%u]", total_length);
        return CM_OTA_RET_CODE_PARAM_ERROR;
    }
    OTA_LOG("total_length:%u\r\n", total_length);
    cm_ota_set_otasize(total_length);
    return 0;
}

// AT+OTA="cm_ota_get_capacity"
int func_cm_ota_get_capacity(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int32_t ret = 0;
    ret = cm_ota_get_capacity();

    if (ret < CM_OTA_SUCCESS)
    {
        OTA_LOG("cm_ota_get_capacity fail");
        return CM_OTA_UNKOWN_ERROR;
    }

    cm_uart_printf_urc("+OTA_GET_CAPACITY: free size %d", ret);
    return 0;
}


/****************************************************************************
 * Public Types
 ****************************************************************************/
typedef enum
{
    FOTA_IDLE = 0,
    FOTA_DOWNLOADING,
    FOTA_DOWNLOADED,
    FOTA_PENDING = 4,
    FOTA_MAX,
} fota_info_state_e;

typedef struct
{
    uint8_t  ssl_id;
    uint8_t  state;
    uint8_t  timeout;
    uint32_t package_offset;
    uint32_t package_total;
    uint32_t pkgsize;
    cm_fs_t  fd;
    uint8_t  url[257];
} fota_t;

/* 使用http的range方式请求数据 */
#if 1
/**HTTP请求响应超时最大时间*/
#define HTTPCLIENT_WAITRSP_TIMEOUT_MAXTIME        60
/**HTTP连接超时默认时间*/
#define HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT        60
#define HTTPCLIENT_SSL_ENABLE

typedef void *cm_httpclient_handle_t;

/**HTTP回调事件*/
typedef enum{
    CM_HTTP_CALLBACK_EVENT_REQ_START_SUCC_IND=1,        //请求启动成功事件
    CM_HTTP_CALLBACK_EVENT_RSP_HEADER_IND,              //接收到报头事件
    CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND,             //接收到消息体事件
    CM_HTTP_CALLBACK_EVENT_RSP_END_IND,                 //请求响应结束事件
    CM_HTTP_CALLBACK_EVENT_ERROR_IND,                   //请求失败事件
}cm_httpclient_callback_event_e;

typedef enum
{
    AT_RET_HTTP_RET_CODE_UNKNOWN_ERROR = 650,
    AT_RET_HTTP_RET_CODE_NO_MORE_FREE_CLIENT,
    AT_RET_HTTP_RET_CODE_CLIENT_NOT_CREATE,
    AT_RET_HTTP_RET_CODE_CLIENT_IS_BUSY,
    AT_RET_HTTP_RET_CODE_URL_PARSE_FAIL,
    AT_RET_HTTP_RET_CODE_SSL_NOT_ENABLE,
    AT_RET_HTTP_RET_CODE_CONNECT_FAIL,
    AT_RET_HTTP_RET_CODE_SEND_DATA_FAIL,
    AT_RET_HTTP_RET_CODE_OPEN_FILE_FAIL,
}http_ret_code_t;

/**HTTP结果码*/
typedef enum{
    CM_HTTP_RET_CODE_OK = 0, //成功
    CM_HTTP_RET_CODE_OPERATION_NOT_ALLOWED = 3, //操作不被允许 3
    CM_HTTP_RET_CODE_MALLOC_FAIL = 23, //内存分配失败 23
    CM_HTTP_RET_CODE_PARAM_ERROR = 50, //参数错误 50
    CM_HTTP_RET_CODE_UNKNOWN_ERROR = AT_RET_HTTP_RET_CODE_UNKNOWN_ERROR, //未知错误 650
    CM_HTTP_RET_CODE_NO_MORE_FREE_CLIENT = AT_RET_HTTP_RET_CODE_NO_MORE_FREE_CLIENT, //没有空闲客户端 651
    CM_HTTP_RET_CODE_CLIENT_NOT_CREATE = AT_RET_HTTP_RET_CODE_CLIENT_NOT_CREATE, //客户端未创建 652
    CM_HTTP_RET_CODE_CLIENT_IS_BUSY = AT_RET_HTTP_RET_CODE_CLIENT_IS_BUSY, //客户端忙 653
    CM_HTTP_RET_CODE_URL_PARSE_FAIL = AT_RET_HTTP_RET_CODE_URL_PARSE_FAIL, //URL解析失败 654
    CM_HTTP_RET_CODE_SSL_NOT_ENABLE = AT_RET_HTTP_RET_CODE_SSL_NOT_ENABLE, //SSL未使能 655
    CM_HTTP_RET_CODE_CONNECT_FAIL = AT_RET_HTTP_RET_CODE_CONNECT_FAIL, //连接失败 656
    CM_HTTP_RET_CODE_SEND_DATA_FAIL = AT_RET_HTTP_RET_CODE_SEND_DATA_FAIL, //数据发送失败 657
    CM_HTTP_RET_CODE_OPEN_FILE_FAIL = AT_RET_HTTP_RET_CODE_OPEN_FILE_FAIL, //打开文件失败 658
} cm_httpclient_ret_code_e;

/**HTTP请求类型*/
typedef enum {
    HTTPCLIENT_REQUEST_NONE = 0,
    HTTPCLIENT_REQUEST_GET,
    HTTPCLIENT_REQUEST_POST,
    HTTPCLIENT_REQUEST_PUT,
    HTTPCLIENT_REQUEST_DELETE,
    HTTPCLIENT_REQUEST_HEAD,
    HTTPCLIENT_REQUEST_MAX
} cm_httpclient_request_type_e;

/**HTTP异常状态码*/
typedef enum{
    CM_HTTP_EVENT_CODE_DNS_FAIL=1,                      /*!< DNS解析失败 */
    CM_HTTP_EVENT_CODE_CONNECT_FAIL,                    /*!< 连接服务器失败 */
    CM_HTTP_EVENT_CODE_CONNECT_TIMEOUT,                 /*!< 连接超时 */
    CM_HTTP_EVENT_CODE_SSL_CONNECT_FAIL,                /*!< SSL握手失败 */
    CM_HTTP_EVENT_CODE_CONNECT_BREAK,                   /*!< 连接异常断开 */
    CM_HTTP_EVENT_CODE_WAITRSP_TIMEOUT,                 /*!< 等待响应超时 */
    CM_HTTP_EVENT_CODE_DATA_PARSE_FAIL,                 /*!< 数据解析失败 */
    CM_HTTP_EVENT_CODE_CACHR_NOT_ENOUGH,                /*!< 缓存空间不足 */
    CM_HTTP_EVENT_CODE_DATA_DROP,                       /*!< 数据丢包 */
    CM_HTTP_EVENT_CODE_WRITE_FILE_FAIL,                 /*!< 写文件失败 */
    CM_HTTP_EVENT_CODE_UNKNOWN=255,                     /*!< 未知错误 */
}cm_httpclient_error_event_e;

/**HTTP相关回调函数*/
typedef struct {
    uint32_t total_len;
    uint32_t sum_len;
    uint32_t current_len;
    const uint8_t *response_content;
}cm_httpclient_callback_rsp_content_param_t;

/**HTTP相关回调函数*/
typedef struct {
    uint16_t response_code;
    uint16_t response_header_len;
    const uint8_t *response_header;
}cm_httpclient_callback_rsp_header_param_t;

/**HTTP可配参数*/
typedef struct {
#ifdef CM_OPENCPU_SUPPORT
    uint8_t ssl_enable;             /*!< 是否使用HTTPS */
    int32_t ssl_id;                 /*!< ssl 索引号 */
    uint8_t cid;                    /*!< PDP索引 */
    uint8_t conn_timeout;           /*!< 连接超时时间 */
    uint8_t rsp_timeout;            /*!< 响应超时时间 */
    uint8_t dns_priority;           /*!< dns解析优先级 0：使用全局优先级。1：v4优先。2：v6优先 */
#else
#ifdef HTTPCLIENT_SSL_ENABLE
    uint8_t ssl_enable;             //是否使用HTTPS
    int32_t ssl_id;                 //ssl 索引号
#endif
    uint8_t cid;                    //PDP索引
    uint8_t conn_timeout;           //连接超时时间
    uint8_t rsp_timeout;            //响应超时时间
#endif
}cm_httpclient_cfg_t;

typedef void (*cm_httpclient_event_callback_func)(cm_httpclient_handle_t client_handle, cm_httpclient_callback_event_e event, void *param);

extern cm_httpclient_ret_code_e cm_httpclient_create(const uint8_t *url, cm_httpclient_event_callback_func callback, cm_httpclient_handle_t *handle);
extern cm_httpclient_ret_code_e cm_httpclient_request_start(cm_httpclient_handle_t handle, cm_httpclient_request_type_e method, const uint8_t *path,
bool chunked, uint32_t content_length);
extern cm_httpclient_ret_code_e cm_httpclient_set_cfg(cm_httpclient_handle_t handle, cm_httpclient_cfg_t cfg);
extern cm_httpclient_ret_code_e cm_httpclient_delete(cm_httpclient_handle_t handle);

#endif

static fota_t fota_info = {0,0,0,0,0,0,NULL,"0"};
static cm_httpclient_handle_t fota_http_client = NULL;
static uint8_t fota_http_range_head[32] = {0};
static uint8_t *fota_buffer = NULL;

int func_fota_download_firmware();

/* 下载失败,清除数据 */
static int func_fota_erase()
{
    if(fota_info.fd)
    {
        cm_file_close(fota_info.fd);
        fota_info.fd = NULL;
    }

    int ret = cm_ota_firmware_erase();
    if(ret != 0)
    {
        OTA_LOG("ret:%d", ret);
        return CM_OTA_UNKOWN_ERROR;
    }

    uint8_t url[257] = {0};
    memcpy(url, fota_info.url, strlen(fota_info.url));
    memset(&fota_info, 0, sizeof(fota_t));
    memcpy(fota_info.url, url, strlen(url));
    OTA_LOG("url:%s", url);
    if(fota_buffer)
    {
        cm_free(fota_buffer);
        fota_buffer = NULL;
    }
    return 0;
}

static void func_fota_get_firmware_totalsize(const uint8_t *header)
{
    char *tmp = strstr((const char *)header, (const char *)"Content-Range: bytes ");
    if(tmp)
    {
        char *length = strstr((const char *)tmp, (const char *)"/");
        fota_info.package_total =  atoi((const char *)(length+1));
    }
}

static void func_fota_httpclient_callback(cm_httpclient_handle_t client_handle, cm_httpclient_callback_event_e event, void *param)
{
    switch(event)
    {
        case CM_HTTP_CALLBACK_EVENT_REQ_START_SUCC_IND:
        {
            OTA_LOG("START_SUCC_IND");
            break;
        }
        case CM_HTTP_CALLBACK_EVENT_RSP_HEADER_IND:
        {
            cm_httpclient_callback_rsp_header_param_t *callback_param = (cm_httpclient_callback_rsp_header_param_t *)param;
            OTA_LOG("callback_param->response_code:%u", callback_param->response_code);
            if(callback_param->response_code != 200 && callback_param->response_code != 206)  //排除错误包
            {
                OTA_LOG("%u %u %u", fota_info.state, fota_info.package_total, fota_info.package_offset);
                char write_report[32] = {0};
                sprintf(write_report, "+MFWDLOAD:3,%u,0", fota_info.package_total);
                fota_info.state = FOTA_PENDING;
                cm_uart_printf_urc((const char *)write_report);
                OTA_LOG("erase_val:%d", func_fota_erase());
                break;
            }
            if(fota_info.package_total == 0)
            {
                func_fota_get_firmware_totalsize(callback_param->response_header);
                OTA_LOG("fota_info.package_total:%u", fota_info.package_total);
            }
            break;
        }
        case CM_HTTP_CALLBACK_EVENT_RSP_CONTENT_IND:
        {
            if(fota_info.state == FOTA_PENDING)
            {
                break;
            }
            if(fota_info.package_total < 256) // 总长度太小，排除不进行写入
            {
                OTA_LOG("%u %u %u", fota_info.state, fota_info.package_total, fota_info.package_offset);
                char write_report[32] = {0};
                sprintf(write_report, "+MFWDLOAD: 3,%u,0", fota_info.package_total);
                fota_info.state = FOTA_PENDING;
                cm_uart_printf_urc((const char *)write_report);
                OTA_LOG("erase_val:%d", func_fota_erase());
                break;
            }
            cm_httpclient_callback_rsp_content_param_t *callback_param = (cm_httpclient_callback_rsp_content_param_t *)param;
            OTA_LOG("recv fwlen:%d", callback_param->current_len);
            OTA_LOG("callback_param->sum_len:%u callback_param->total_len:%u", callback_param->sum_len, callback_param->total_len);

            memcpy(fota_buffer + callback_param->sum_len - callback_param->current_len, callback_param->response_content, callback_param->current_len);
            if(callback_param->sum_len != callback_param->total_len)
            {
               break;
            }
            int ret = cm_ota_firmware_write(fota_buffer, callback_param->total_len);
            if(ret != 0) /* 写文件失败 */
            {
                OTA_LOG("%u %u %u", fota_info.state, fota_info.package_total, fota_info.package_offset);
                OTA_LOG("ret:%d\r\n", ret);
                OTA_LOG("erase_val:%d", func_fota_erase());
                break;
            }

            /* 数据写完成后,清空buffer */
            memset(fota_buffer, 0, fota_info.pkgsize + 1);

            fota_info.package_offset = fota_info.package_offset + callback_param->total_len;
            /* 显示下载进度 */
            char write_report[32] = {0};
            sprintf(write_report, "+MFWDLOAD: %u%%", (fota_info.package_offset*100)/fota_info.package_total);
            cm_uart_printf_urc((const char *)write_report);

            if(fota_info.package_offset < fota_info.package_total)
            {
                /* 没下载完成,继续下载 */
                if(func_fota_download_firmware() != 0)
                    OTA_LOG("erase_val:%d", func_fota_erase());
            }
            else
            {
                /* 下载完成 */
                fota_info.state = FOTA_DOWNLOADED;
                cm_httpclient_delete(fota_http_client);
                fota_http_client = NULL;

                /* 查看下载的内容是否正确：1、查看文件大小 2、查看开始10个字节和结尾10个字节 */
                /* 文件偏移 */
                if(fota_info.fd)
                {
                    cm_file_close(fota_info.fd);
                    fota_info.fd = NULL;
                }

                if(fota_buffer)
                {
                    cm_free(fota_buffer);
                    fota_buffer = NULL;
                }
            }
            break;
        }
        case CM_HTTP_CALLBACK_EVENT_RSP_END_IND:
        {
            OTA_LOG("RSP_END_IND");
            break;
        }
        case CM_HTTP_CALLBACK_EVENT_ERROR_IND:
        {
            cm_httpclient_error_event_e callback_param = (cm_httpclient_error_event_e)param;
           // if(callback_param == CM_HTTP_EVENT_CODE_WAITRSP_TIMEOUT || callback_param == CM_HTTP_EVENT_CODE_CONNECT_TIMEOUT)
            {
                char error_report[32] = {0};
                sprintf(error_report, "+MFWDLOAD: 3,%u,%u",(unsigned int)fota_info.package_total,(unsigned int)fota_info.package_offset);
                cm_uart_printf_urc((const char *)error_report);
                osDelay(300);
                memset(error_report, 0, 32);
                sprintf(error_report, "+MFWDLOAD: 0");
                cm_uart_printf_urc((const char *)error_report);
                fota_info.state = FOTA_PENDING;         //出现网络错误
                OTA_LOG("erase_val:%d", func_fota_erase());
            }
            OTA_LOG("ERROR_IND");
            break;
        }
        default:
        {
            break;
        }
    }
}

static void func_fota_range_header_update(void)
{
    if(fota_info.pkgsize == 0)
    {
        fota_info.pkgsize = 4096;
    }

    memset((void *)fota_http_range_head, 0, 32);
    if(fota_info.package_total)
    {
        /* 最后一个报文，不足packsize大小 */
        if(((fota_info.package_total - fota_info.package_offset)/fota_info.pkgsize) < 1)
            sprintf((char *)fota_http_range_head, "Range: bytes=%u-%u", (unsigned int)fota_info.package_offset, (unsigned int)(fota_info.package_total-1));
        else
            sprintf((char *)fota_http_range_head, "Range: bytes=%u-%u", (unsigned int)fota_info.package_offset, (unsigned int)(fota_info.package_offset + fota_info.pkgsize-1));
    }
    else
    {
        sprintf((char *)fota_http_range_head, "Range: bytes=%u-%u", (unsigned int)fota_info.package_offset, (unsigned int)(fota_info.package_offset + fota_info.pkgsize-1));
    }
}

int func_fota_download_firmware()
{
    int ret = 0;
    if(fota_info.state != FOTA_DOWNLOADED)
    {
        if(strlen((const char *)(fota_info.url)))
        {
            /* 1.创建http客户端 */
            if(fota_http_client == NULL)
            {
                ret = cm_httpclient_create(fota_info.url, func_fota_httpclient_callback, &fota_http_client);
                if(ret != 0)
                {
                    OTA_LOG("creat http client fail, ret:%d",ret);
                    return CM_OTA_UNKOWN_ERROR;
                }

                cm_httpclient_cfg_t client_cfg = {0};
                if(strncasecmp(fota_info.url, "https",strlen("https")) == 0)
                {
                    client_cfg.ssl_enable = 1;
                    client_cfg.ssl_id = fota_info.ssl_id;
                }
                client_cfg.cid = 0xFF;
                client_cfg.conn_timeout = (fota_info.timeout == 0)? HTTPCLIENT_CONNECT_TIMEOUT_DEFAULT:fota_info.timeout;
                client_cfg.rsp_timeout = HTTPCLIENT_WAITRSP_TIMEOUT_MAXTIME;
                int result = cm_httpclient_set_cfg(fota_http_client, client_cfg);
                if(result != 0)
                {
                    OTA_LOG("creat http client set cfg fail, ret:%d",ret);
                    return CM_OTA_UNKOWN_ERROR;
                }
            }

            /* 处理http响应,需要把数据缓存起来 */
            if(fota_info.pkgsize == 0)
            {
                fota_info.pkgsize = 4096;
            }
            if(fota_buffer == NULL)
            {
                fota_buffer = (uint8_t*) cm_malloc(fota_info.pkgsize + 1);
                if(fota_buffer == NULL)
                {
                    OTA_LOG("fwup_buffer malloc fail");
                    return CM_OTA_RET_CODE_MALLOC_FAIL;
                }
                memset(fota_buffer, 0, fota_info.pkgsize + 1);
            }
        }
        else
        {
            OTA_LOG("param error url");
            return CM_OTA_UNKOWN_ERROR;
        }

        /* 第一次下载,需要擦除文件内容              */
        if(fota_info.state == FOTA_IDLE)
        {
            cm_ota_firmware_erase();
        }
    }
    else
    {
        OTA_LOG("param error, fota_info.state:%u", fota_info.state);
        return CM_OTA_UNKOWN_ERROR;
    }

    /* 2.状态设置为下载中 */
    fota_info.state = FOTA_DOWNLOADING;
    /* 3.设置http的range特俗头部 */
    func_fota_range_header_update();
    ret = cm_httpclient_specific_header_set(fota_http_client, fota_http_range_head, strlen((char *)fota_http_range_head));
    if(ret != 0)
    {
        OTA_LOG("param error ret:%d", ret);
        return CM_OTA_UNKOWN_ERROR;
    }
    /* 4.发送http/https请求*/
    ret = cm_httpclient_request_start(fota_http_client, HTTPCLIENT_REQUEST_GET,  fota_info.url, false, 0);
    if(ret != 0)
    {
        OTA_LOG("param error ret:%u", ret);
        return CM_OTA_UNKOWN_ERROR;
    }

    return 0;
}

/********************************************** fota function ******************************************/
// AT+FOTA="cm_fota_set_url","url"
int func_cm_fota_set_url(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    char *url = NULL;

    url = cm_malloc(OTA_CMD_STR_LEN+1);
    if (url == NULL) {
        return CM_OTA_RET_CODE_MALLOC_FAIL;
    }
    memset(url, 0, OTA_CMD_STR_LEN+1);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u", url, OTA_CMD_STR_LEN))
    {
        cm_free(url);
        OTA_LOG("param error url tool long\r\n");
        return CM_OTA_RET_CODE_PARAM_ERROR;
    }

    /* 1.设置url */
    ret = cm_fota_set_url(url);
    if (ret != CM_OTA_SUCCESS)
    {
        OTA_LOG("cm_fota_set_url fail");
        cm_free(url);
        return CM_OTA_URL_ERROR;
    }

    cm_free(url);
    return 0;
}

// AT+FOTA="cm_fota_set_reboot_time"
int func_cm_fota_set_reboot_time(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    return 0;
}

// AT+FOTA="cm_fota_read_config"
int func_cm_fota_read_config(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    cm_fota_info_t cfg = {0};
    cm_fota_read_config(&cfg);

    cm_uart_printf_urc("+FOTA_READ_CONFIG: mode:%d url:%s", cfg.fixed_info.fota_mode, cfg.fixed_info.url);
    return 0;
}

// AT+FOTA="cm_fota_res_callback_register"
int func_cm_fota_res_callback_register(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    return 0;
}

static void fota_tsk_entry(void *argument)
{
    os_uint8_t id = *((os_uint8_t *)argument);
    int ret = 0;
    ret = cm_fota_exec_upgrade();
    cm_uart_printf_urc("+FOTA_EXEC_UPGRADE: %d", ret);
    if (ret != CM_OTA_SUCCESS)
    {
        OTA_LOG("cm_fota_exec_upgrade fail");
        cm_uart_printf_result(id, CM_OTA_UPGRADE_ERROR);
    }
    else
    {
        cm_uart_printf_result(id, 0);
    }

    if(g_fota_threadId != NULL)
    {
        g_fota_threadId = NULL;
    }
}

// AT+FOTA="cm_fota_exec_upgrade"
int func_cm_fota_exec_upgrade(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    if(g_fota_threadId == NULL)
    {
        g_fota_id = id;
        osThreadAttr_t fota_task_attr = {"FotaTask", osThreadDetached, NULL, 0U, NULL, 4096, FOTA_TASK_PRIORITY, 0U, 0U};
        g_fota_threadId = osThreadNew(fota_tsk_entry, &g_fota_id, &fota_task_attr);
        if(g_fota_threadId == NULL)
        {
            OTA_LOG("g_fota_threadId create fail");
            return CM_OTA_UPGRADE_ERROR;
        }
        return 0;
    }
    else
    {
        return CM_OTA_UNKOWN_ERROR;
    }
}

#endif


