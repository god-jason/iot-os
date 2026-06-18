#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "HTTPClient.h"

#include "yopen_os.h"
#include "yopen_nw.h"
#include "yopen_debug.h"
#include "yopen_app_main.h"
#include "yopen_fota.h"
#include "yopen_power.h"

#define DEMO_FOTA_TRACE(fmt, ...) yopen_trace("[FOTA]"#fmt, ##__VA_ARGS__)

/* 
*  1. DEMO_FOTA_NEW_VERSION宏编译默认关闭， 编译出OLD版本
*  2. 打开DEMO_FOTA_NEW_VERSION宏, 编译出NEW版本
*  3. OLD和NEW生成差分包文件， 并放到服务器， 修改HTTP_URL路径
*  4. 板子下载OLD版本进行测试
*/
//#define DEMO_FOTA_NEW_VERSION 

//测试时需要跟换服务器
#define HTTP_URL "http://122.51.209.56:8000/default_delta.par"

#define HTTP_RSP_HEAD_BUFFER_SIZE 800
#define HTTP_RSP_CONTENT_BUFFER_SIZE  (150*1024)


#ifndef DEMO_FOTA_NEW_VERSION
static HttpClientData* _http_malloc_data(void)
{
    HttpClientData* clientData = malloc(sizeof(HttpClientData));
    do{
        if(clientData == NULL){
            break;
        }
        memset(clientData, 0, sizeof(HttpClientData));
        clientData->headerBufLen = HTTP_RSP_HEAD_BUFFER_SIZE;
        clientData->headerBuf = malloc(HTTP_RSP_HEAD_BUFFER_SIZE);
        if(clientData->headerBuf == NULL){
            free(clientData);
            clientData = NULL;
            break;
        }
        clientData->respBufLen = HTTP_RSP_CONTENT_BUFFER_SIZE;
        clientData->respBuf = malloc(HTTP_RSP_CONTENT_BUFFER_SIZE);
        if(clientData->respBuf == NULL){
            free(clientData->headerBuf);
            clientData->headerBuf = NULL;
            free(clientData);
            clientData = NULL;
            break;
        }
    }while (0);
    

    return clientData;
}

static void _http_free_data(HttpClientData* clientData)
{
    if(!clientData)
    {
        return;
    }
    if(clientData->headerBuf)
    {
        free(clientData->headerBuf);
        clientData->headerBufLen = 0;
    }
    if(clientData->respBuf)
    {
        free(clientData->respBuf);
        clientData->respBufLen = 0;
    }
    free(clientData);
}
#endif

void yopen_fota_demo_task(void* argv)
{
#ifdef DEMO_FOTA_NEW_VERSION
    yopen_errcode_fota_e ret;
    yopen_fota_result_e fota_result;

    yopen_rtos_task_sleep_ms(2000);

    ret = yopen_fota_get_result(&fota_result);
    DEMO_FOTA_TRACE("==========yopen_fota_get_result %d==========", fota_result);

    //清除标记
    yopen_fota_clear(NULL, YOPEN_FOTA_INTERNAL_FLASH, TRUE);

    ret = yopen_fota_get_result(&fota_result);
    DEMO_FOTA_TRACE("==========yopen_fota_clear fota_result %d==========", fota_result);

    DEMO_FOTA_TRACE("==========YOPEN_FOTA_FINISHED==========");

    yopen_rtos_task_delete(NULL);
#else
    yopen_nw_reg_status_info_s nw_status;
    HttpClientContext* clientContext = NULL;
    HttpClientData* clientData = NULL;
    HTTPResult      result;
    uint32_t        headerLen;
    uint32_t        count = 0;
    yopen_errcode_fota_e ret;

    do{
		yopen_nw_get_reg_status(0, &nw_status);

		yopen_rtos_task_sleep_ms(1000);

		DEMO_FOTA_TRACE("==========nw_status %d==========", nw_status.data_reg.state);

		if(nw_status.data_reg.state == YOPEN_NW_REG_STATE_DENIED)
		{
			DEMO_FOTA_TRACE("network reg denied exit !!!!");
			goto exit;
		}
		
	}while(!(nw_status.data_reg.state == YOPEN_NW_REG_STATE_HOME_NETWORK || nw_status.data_reg.state == YOPEN_NW_REG_STATE_ROAMING));

    clientContext = malloc(sizeof(HttpClientContext));

    memset(clientContext, 0, sizeof(HttpClientContext));

    clientContext->timeout_s = 2;  //default send timeout 2 second,no need to wait for a long time for TCP
    clientContext->timeout_r = 20;  //default recv timeout 20 second
    clientContext->socket = -1;
    clientContext->pdpId = 1;
    clientContext->saveMem = 1;

    clientData = _http_malloc_data();

    if(!clientData)
    {
        DEMO_FOTA_TRACE("no memory!!!");
        goto exit;
    }

    result = httpConnect(clientContext, HTTP_URL);

    if(result != HTTP_OK)
    {
        DEMO_FOTA_TRACE("httpConnect error %d!!!", result);
        goto exit;
    }
    DEMO_FOTA_TRACE("httpConnect URL %s OK", HTTP_URL);

    result = httpSendRequest(clientContext, HTTP_URL, HTTP_GET, clientData);

    if(result != HTTP_OK)
    {
        DEMO_FOTA_TRACE("httpSendRequest error %d!!!", result);
        goto exit;
    }

    DEMO_FOTA_TRACE("httpSendRequest OK");

    do {
        DEMO_FOTA_TRACE("httpRecvResponse loop.");
        memset(clientData->headerBuf, 0, clientData->headerBufLen);
        memset(clientData->respBuf, 0, clientData->respBufLen);
        result = httpRecvResponse(clientContext, clientData);

        DEMO_FOTA_TRACE("httpRecvResponse loop.result %d", result);
        if(result == HTTP_OK || result == HTTP_MOREDATA){
            headerLen = strlen(clientData->headerBuf);
            if(headerLen > 0)
            {
                DEMO_FOTA_TRACE("total content length=%d", clientData->recvContentLength);
            }
            
            count += clientData->blockContentLen;
            DEMO_FOTA_TRACE("has recv=%d", count);
        }
    } while (result == HTTP_MOREDATA || result == HTTP_CONN);

exit:
    DEMO_FOTA_TRACE("DEMO HTTP END");

    ret = yopen_fotanvm_init();
    DEMO_FOTA_TRACE("yopen_fotanvm_init=%d", ret);
    ret = yopen_fotanvm_write(0, (uint8_t*)clientData->respBuf, clientData->respBufLen);
    DEMO_FOTA_TRACE("yopen_fotanvm_write=%d", ret);
    ret = yopen_fota_image_verify(0, YOPEN_FOTA_INTERNAL_FLASH);
    DEMO_FOTA_TRACE("yopen_fota_image_verify=%d", ret);

    if (ret == 0)
    {
        DEMO_FOTA_TRACE("yopen_fota_power_reset");
        yopen_fota_power_reset();
    }

    if(clientContext != NULL)
    {
        if(clientContext->caCert != NULL)
        {
            free((char*)clientContext->caCert);
        }
        if(clientContext->clientCert != NULL)
        {
            free((char*)clientContext->clientCert);
        }
        if(clientContext->clientPk != NULL)
        {
            free((char*)clientContext->clientPk);
        }
        free((void*)clientContext);
    }
    _http_free_data(clientData);
    yopen_rtos_task_delete(NULL);
#endif
}
