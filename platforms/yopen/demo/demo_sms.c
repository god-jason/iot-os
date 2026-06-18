/**
 * @File Name: yopen_sms_demo.c
 * @brief
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yopen_os.h"
#include "yopen_debug.h"
#include "yopen_sms.h"
#include "yopen_type.h"

typedef struct
{
    int event;
    yopen_sms_new_s arg;
}demoSmsMessage;

static yopen_queue_t s_sms_queue_ref;

void user_sms_event_callback(uint8_t nSim, int event_id, void *ctx)
{
    demoSmsMessage msg;
    msg.event = event_id;
    switch (event_id)
    {
        case YOPEN_SMS_INIT_OK_IND:
        {
            yopen_trace("YOPEN_SMS_INIT_OK_IND");
            yopen_rtos_queue_release(s_sms_queue_ref, sizeof(msg), (uint8*)&msg, YOPEN_NO_WAIT);
            break;
        }
        case YOPEN_SMS_NEW_MSG_IND:
        {
            yopen_sms_new_s *new_sms = (yopen_sms_new_s *)ctx;
            yopen_trace("sms_demo: new_msg sim=%d, index=%d, storage memory=%d", nSim, new_sms->index, new_sms->mem);
            memcpy(&msg.arg, new_sms, sizeof(yopen_sms_new_s));
            break;
        }
        
        case YOPEN_SMS_MEM_FULL_IND:
        {
            yopen_sms_new_s *new_sms = (yopen_sms_new_s *)ctx;
            yopen_trace("sms_demo: YOPEN_SMS_MEM_FULL_IND sim=%d, memory=%d", nSim, new_sms->mem);
            memcpy(&msg.arg, new_sms, sizeof(yopen_sms_new_s));
            break;
        }
        default:
            return;
            break;
    }
    yopen_rtos_queue_release(s_sms_queue_ref, sizeof(msg), (uint8*)&msg, YOPEN_NO_WAIT);
}


/**
 * @brief  单个十六进制字符转十进制数值
 */
static int hex_char_to_val(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

/**
 * @brief  UCS2十六进制字符串解析为UTF-8字符串
 */
int yopen_ucs2_decode(const char* ucs2_hex, char** utf8_out)
{
    if (ucs2_hex == NULL || utf8_out == NULL) {
        printf("错误：入参为空\n");
        return -1;
    }

    int hex_len = strlen(ucs2_hex);
    if (hex_len % 4 != 0) {
        printf("错误：UCS2字符串长度必须是4的倍数\n");
        return -1;
    }

    int ucs2_cnt = hex_len / 4;
    int utf8_max_len = ucs2_cnt * 3 + 1;
    *utf8_out = (char*)malloc(utf8_max_len);
    if (*utf8_out == NULL) {
        printf("错误：内存分配失败\n");
        return -1;
    }
    memset(*utf8_out, 0, utf8_max_len);
    char* utf8_ptr = *utf8_out;

    for (int i = 0; i < hex_len; i += 4) {
        int v1 = hex_char_to_val(ucs2_hex[i]);
        int v2 = hex_char_to_val(ucs2_hex[i + 1]);
        int v3 = hex_char_to_val(ucs2_hex[i + 2]);
        int v4 = hex_char_to_val(ucs2_hex[i + 3]);

        if (v1 < 0 || v2 < 0 || v3 < 0 || v4 < 0) {
            printf("错误：非法十六进制字符\n");
            free(*utf8_out);
            *utf8_out = NULL;
            return -1;
        }

        uint16_t ucs2 = (v1 << 12) | (v2 << 8) | (v3 << 4) | v4;

        // Unicode 转 UTF-8
        if (ucs2 <= 0x007F) {
            *utf8_ptr++ = (char)ucs2;
        }
        else if (ucs2 <= 0x07FF) {
            *utf8_ptr++ = 0xC0 | ((ucs2 >> 6) & 0x1F);
            *utf8_ptr++ = 0x80 | (ucs2 & 0x3F);
        }
        else {
            *utf8_ptr++ = 0xE0 | ((ucs2 >> 12) & 0x0F);
            *utf8_ptr++ = 0x80 | ((ucs2 >> 6) & 0x3F);
            *utf8_ptr++ = 0x80 | (ucs2 & 0x3F);
        }
    }

    return 0;
}


void yopen_sms_demo_task(void *param)
{
    char addr[20]      = {0};
    uint8_t nSim       = 0;
    int err = YOPEN_OSI_SUCCESS;
    uint8_t totalSms, index;
    demoSmsMessage msg;
    yopen_sms_recv_s *sms_recv   = NULL;

    yopen_trace("yopen_sms_demo_task enter");

    err = yopen_rtos_queue_create(&s_sms_queue_ref, sizeof(demoSmsMessage), 10);
    if (err != YOPEN_OSI_SUCCESS)
    {
        yopen_trace("demo_sms created queue failed, ret = 0x%x", err);
        goto exit;
    }
    yopen_sms_callback_register(user_sms_event_callback);

    while (1)
    {
        yopen_rtos_queue_wait(s_sms_queue_ref, (uint8*)&msg, sizeof(msg), YOPEN_WAIT_FOREVER);
        if(msg.event == YOPEN_SMS_INIT_OK_IND)
        {
            yopen_trace("sms_demo: got init ok");
            break;
        }
    }
    
    {
        yopen_rtos_task_sleep_ms(1000);

        if (YOPEN_SMS_SUCCESS == yopen_sms_get_center_address(nSim, addr, sizeof(addr)))
        {
            yopen_trace("sms_demo: yopen_sms_get_center_address OK, addr=%s", addr);
        }
        else
        {
            yopen_trace("sms_demo: yopen_sms_get_center_address FAIL");
        }
#if 1
        // Send English text message
        if (YOPEN_SMS_SUCCESS == yopen_sms_send_text(nSim, "+8610010", "101", YOPEN_GSM))
        {
            yopen_trace("sms_demo: yopen_sms_send_msg OK");
        }
        else
        {
            yopen_trace("sms_demo: yopen_sms_send_msg FAIL");
        }
#endif

        // Get how many SMS messages can be stored in the SIM card in total and how much storage is used
#if 1
        yopen_sms_stor_info_s stor_info;
        if (YOPEN_SMS_SUCCESS == yopen_sms_get_storage_info(nSim, &stor_info))
        {
            yopen_trace("sms_demo: yopen_sms_get_storage_info OK");
            yopen_trace("sms_demo: SM used=%u,SM total=%u,ME used=%u,ME total=%u, newSmsStorId=%u",
                        stor_info.usedSlotSM,
                        stor_info.totalSlotSM,
                        stor_info.usedSlotME,
                        stor_info.totalSlotME,
                        stor_info.newSmsStorId);
        }
        else
        {
            yopen_trace("sms_demo: yopen_sms_get_storage_info FAIL");
        }
#endif

        // The first parameter specifies that SMS messages are read from SM
        yopen_sms_set_storage(nSim, YOPEN_SM, YOPEN_SM, YOPEN_SM);

        // Read all messages in SIM
#if 1
        uint16_t msg_len            = 512;
        yopen_sms_mem_info_s sms_mem = {0};


        char *msg = (char *)malloc(msg_len);
        if (msg == NULL)
        {
            yopen_trace("sms_demo: malloc yopen_sms_msg_s fail");
            goto exit;
        }
        memset(msg, 0, msg_len);

        yopen_sms_get_storage(nSim, &sms_mem);
        yopen_trace("sms_demo: mem1=%d, mem2=%d, mem3=%d", sms_mem.mem1, sms_mem.mem2, sms_mem.mem3);

        if(sms_mem.mem1 == YOPEN_ME)
        {
            totalSms = stor_info.usedSlotME;
        }
        else
        {
            totalSms = stor_info.usedSlotSM;
        }

        // Read SMS messages as text
        sms_recv = (yopen_sms_recv_s *)malloc(sizeof(yopen_sms_recv_s));
        if (sms_recv == NULL)
        {
            yopen_trace("sms_demo: calloc FAIL");
            goto exit;
        }
        for(index = 0; index < totalSms; index++)
        {
            memset(sms_recv, 0, sizeof(yopen_sms_recv_s));
            if (YOPEN_SMS_SUCCESS == yopen_sms_read(nSim, index, sms_recv))
            {
                yopen_trace("sms_demo index=%d, addr %s, type=%d, coding=%d", index, sms_recv->addr_str, sms_recv->type, sms_recv->coding);
                yopen_trace("sms_demo: index=%d,status=%d",
                            sms_recv->index,
                            sms_recv->status);

                yopen_trace("sms_demo: scst=%d/%d/%d %d:%d:%d %+d",
                            sms_recv->scts.uYear,
                            sms_recv->scts.uMonth,
                            sms_recv->scts.uDay,
                            sms_recv->scts.uHour,
                            sms_recv->scts.uMinute,
                            sms_recv->scts.uSecond,
                            sms_recv->scts.iZone);

                yopen_trace("sms_demo: uid=%u,total=%u,seg=%u,data=%s",
                            sms_recv->consms_uid,
                            sms_recv->consms_total,
                            sms_recv->consms_seq,
                            sms_recv->body_str);

                if (sms_recv->coding == YOPEN_SMS_MSG_CODING_UCS2)
                {
                    char *utf8_out = NULL;
                    int ret = yopen_ucs2_decode((const char *)sms_recv->body_str, &utf8_out);

                    if (ret==0)
                    {
                        yopen_trace("sms_demo: yopen_ucs2_decode %s", utf8_out);
                        free(utf8_out);
                    }
                    else
                    {
                        yopen_trace("sms_demo: yopen_ucs2_decode FAIL");
                    }
                }       
            }
        }
        
        if (sms_recv)
            free(sms_recv);
#endif

    }
    while (1)
    {
        yopen_rtos_queue_wait(s_sms_queue_ref, (uint8*)&msg, sizeof(msg), YOPEN_WAIT_FOREVER);

        switch (msg.event)
        {
            case YOPEN_SMS_NEW_MSG_IND:
                sms_recv = (yopen_sms_recv_s *)malloc(sizeof(yopen_sms_recv_s));
                if (sms_recv == NULL)
                {
                    yopen_trace("sms_demo: calloc FAIL");
                    goto exit;
                }
                if (YOPEN_SMS_SUCCESS == yopen_sms_read(nSim, msg.arg.index, sms_recv))
                {
                    yopen_trace("sms_demo index=%d, addr %s, type=%d, coding=%d", msg.arg.index, sms_recv->addr_str, sms_recv->type, sms_recv->coding);
                    yopen_trace("sms_demo: index=%d,status=%d",
                                sms_recv->index,
                                sms_recv->status);

                    yopen_trace("sms_demo: scst=%d/%d/%d %d:%d:%d %+d",
                                sms_recv->scts.uYear,
                                sms_recv->scts.uMonth,
                                sms_recv->scts.uDay,
                                sms_recv->scts.uHour,
                                sms_recv->scts.uMinute,
                                sms_recv->scts.uSecond,
                                sms_recv->scts.iZone);

                    yopen_trace("sms_demo: uid=%u,total=%u,seg=%u,data=%s",
                                sms_recv->consms_uid,
                                sms_recv->consms_total,
                                sms_recv->consms_seq,
                                sms_recv->body_str);

					if (sms_recv->coding == YOPEN_SMS_MSG_CODING_UCS2)
			        {
			            char *utf8_out = NULL;
			            int ret = yopen_ucs2_decode((const char *)sms_recv->body_str, &utf8_out);

			            if (ret==0)
			            {
			                yopen_trace("sms_demo: yopen_ucs2_decode %s", utf8_out);
			                free(utf8_out);
			            }
			            else
			            {
			                yopen_trace("sms_demo: yopen_ucs2_decode FAIL");
			            }
			        }
                }
                else
                {
                    yopen_trace("sms_demo: read index %d sms FAIL", index);
                }
                free(sms_recv);
                break;
            case YOPEN_SMS_MEM_FULL_IND:
                if (YOPEN_SMS_SUCCESS == yopen_sms_delete(nSim, 0, YOPEN_SMS_DEL_ALL))
                {
                    yopen_trace("sms_demo: delete msg OK");
                }
                else
                {
                    yopen_trace("sms_demo: delete sms FAIL");
                }
                break;
            default:
                break;
        }
    }
    

exit:
    yopen_rtos_task_delete(NULL);
}

