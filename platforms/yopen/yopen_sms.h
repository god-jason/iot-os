/**  @file
  yopen_sms.h

  @brief
  TODO

*/


#ifndef _YOPEN_SMS_H_
#define _YOPEN_SMS_H_


#include "yopen_api_common.h"
#include "yopen_type.h"

/*========================================================================
 *  Marco Definition
 *========================================================================*/

#define YOPEN_SMS_MIN_LEN     1
#define YOPEN_MAX_LONGSMS_SEGMENT  8
#define YOPEN_MAX_SMS_LENGTH       (160 * 4)
#define YOPEN_SMS_BUF_LEN          (YOPEN_MAX_SMS_LENGTH + 32)
#define YOPEN_MAX_LONGSMS_LENGTH   (160 * 4 * YOPEN_MAX_LONGSMS_SEGMENT)
// #define YOPEN_TEL_MAX_LEN     21
// #define YOPEN_SMS_PDU_MAX_LEN 176

/*
 * 1> DCS 7bit, max size is 160, IRA
 * 2> DCS 8bit, max size is 140, HEX string type
 * 3> DCS UCS2, max size is 280, HEX string type
*/
#define YOPEN_SMS_MAX_LEN                 640//280
#define YOPEN_SMS_MAX_ADDR_LEN                80

/*========================================================================
 *  Enumeration Definition
 *========================================================================*/

typedef enum
{
    YOPEN_SMS_SUCCESS = 0,
    YOPEN_SMS_ERROR   = 1 | (YOPEN_COMPONENT_SMS << 16),
    YOPEN_SMS_NOT_INIT_ERR,
    YOPEN_SMS_PARA_ERR,
    YOPEN_SMS_NO_MEMORY_ERR,
    YOPEN_SMS_SEM_CREATE_ERR,
    YOPEN_SMS_SEM_TIMEOUT_ERR,
    YOPEN_SMS_NO_MSG_ERR,
    YOPEN_SMS_NOT_SUPORRT
} yopen_sms_errcode_e;

typedef enum
{
    YOPEN_SMS_INIT_OK_IND = 1 | (YOPEN_COMPONENT_SMS << 16), /* Not used */
    YOPEN_SMS_NEW_MSG_IND,
    YOPEN_SMS_MEM_FULL_IND,
} yopen_sms_event_id_e;

typedef enum
{
    YOPEN_ME = 1, // Mobile Equipment message storage
    YOPEN_SM = 2, // SIM message storage
} yopen_sms_stor_e;

typedef enum
{
    YOPEN_GSM  = 0,
    YOPEN_UCS2 = 1,
} yopen_sms_code_e;

typedef enum
{
    YOPEN_PDU  = 0,
    YOPEN_TEXT = 1,
} yopen_sms_format_e;

typedef enum
{
    YOPEN_SMS_UNREAD = 0,
    YOPEN_SMS_READ   = 1,
    YOPEN_SMS_UNSENT = 2,
    YOPEN_SMS_SENT   = 3,
    YOPEN_SMS_ALL,
} yopen_sms_status_e;

typedef enum
{
    YOPEN_SMS_DEL_INDEX = 0,      /* Delete the message specified in <index> */
    YOPEN_SMS_DEL_RECV_READ,      /* Delete all read messages from <mem1> storage */
    YOPEN_SMS_DEL_RECV_READ_SENT, /* Delete all read messages from <mem1> storage and sent mobile originated messages */
    YOPEN_SMS_DEL_RECV_READ_MO,   /* Delete all read messages from <mem1> storage, sent and unsent mobile originated
                                    messages */
    YOPEN_SMS_DEL_ALL,            /* Delete all messages from <mem1> storage */

    YOPEN_SMS_DEL_MODES_NUMS
} yopen_sms_delete_mode_e;

typedef enum
{
    YOPEN_SMS_MSG_CODING_DEFAULT_7BIT,
    YOPEN_SMS_MSG_CODING_8BIT,
    YOPEN_SMS_MSG_CODING_UCS2
} yopen_sms_cscs_e;

typedef enum
{
    YOPEN_SMS_TYPE_DELIVER = 0,       /* SMS-Deliver PDU */
    YOPEN_SMS_TYPE_DELIVER_REPORT,    /* SMS-Deliver Report PDU */
    YOPEN_SMS_TYPE_STATUS_REPORT,     /* SMS-Status Report PDU */
    YOPEN_SMS_TYPE_CB_ETWS_CMAS,      /* CB SMS */
    YOPEN_SMS_TYPE_SUBMIT,            /* SMS-SUBMIT */
    YOPEN_SMS_TYPE_COMMAND,           /* SMS-COMMAND */
    YOPEN_SMS_TYPE_RESERVE
}yopen_sms_type;

/*========================================================================
 *	Struct Definition
 *========================================================================*/
typedef struct
{
    uint8 status;
    uint8 index;
    char *buf;
    uint16 buf_len;
} yopen_sms_msg_s;

typedef struct
{
    uint8 index;
    uint8 mem;
} yopen_sms_new_s;

typedef struct
{
    uint16 usedSlotSM;
    uint16 totalSlotSM;
    // uint16 unReadRecordsSM;
    uint16 usedSlotME;
    uint16 totalSlotME;
    // uint16 unReadRecordsME;
    yopen_sms_stor_e newSmsStorId;
} yopen_sms_stor_info_s;

typedef struct
{
    yopen_sms_stor_e mem1; // messages to be read and deleted from this memory storage.
    yopen_sms_stor_e mem2; // messages will be written and sent to this memory storage.
    yopen_sms_stor_e mem3; // received messages will be placed in this memory storage if routing to PC is not set.
} yopen_sms_mem_info_s;

typedef struct
{
    uint16 uYear; // Only the last two digits
    uint8 uMonth;
    uint8 uDay;
    uint8 uHour;
    uint8 uMinute;
    uint8 uSecond;
    int8 iZone;
} yopen_sms_time_stamp_s;

typedef struct
{
    
    yopen_sms_type         type;
    yopen_sms_cscs_e       coding;   
    yopen_sms_status_e     status;
    yopen_sms_time_stamp_s scts;        // Service Centre Time Stamp
    uint8                  index;       // Index in storage
    
    uint8                  consms_uid;   // Message identification of a concatenated message
    uint8                  consms_seq;   // Sequence number of a concatenated message
    uint8                  consms_total; // The total number of the segments of one concatenated message.

    uint8                  addr_str[YOPEN_SMS_MAX_ADDR_LEN + 1];    /* originating or destination address string */
    uint8                  body_str[YOPEN_SMS_MAX_LEN + 1];      /* message body buffer text/pdu string */
} yopen_sms_recv_s;

/*========================================================================
 *  Callback Definition
 *========================================================================*/
typedef void (*yopen_sms_event_handler_t)(uint8 sim_id, yopen_sms_event_id_e event_id, void *ctx);

/**
 * @brief 发送短信（TEXT格式）
 * 
 * @param nSim SIM卡索引，取值：0-1
 * @param phone_num 目的手机号码
 * @param data 短消息数据
 * @param code 短消息发送时采用的格式
 * @return  0 成功 other 错误码 
 */
yopen_sms_errcode_e yopen_sms_send_text(uint8 nSim, char *phone_num, char *data, yopen_sms_code_e code);

/**
 * @brief  发送短信（PDU格式）
 * 
 * @param nSim SIM卡索引，取值：0-1
 * @param pdu PDU内容
 * @param pdu_len PDU内容长度
 * @return  0 成功 other 错误码 
 */
yopen_sms_errcode_e yopen_sms_send_pdu(uint8_t nSim, const char *pdu, uint32_t pdu_len);

/**
 * @brief  读取单条短消息
 * 
 * @param nSim SIM卡索引，取值：0-1
 * @param index 短信索引
 * @param sms    短信内容
 * @return  0 成功 other 错误码 
 */
yopen_sms_errcode_e yopen_sms_read(uint8_t nSim, uint8_t index, yopen_sms_recv_s *sms);

/**
 * @brief  删除单条消息
 * 
 * @param nSim SIM卡索引，取值：0-1
 * @param index 短信索引
 * @param del_mode yopen_sms_delete_mode_e类型数据
 * 
 * @return  0 成功 other 错误码 
 */
yopen_sms_errcode_e yopen_sms_delete(uint8_t nSim, uint8_t index, yopen_sms_delete_mode_e del_mode);

/**
 * @brief  获取短消息中心号码
 * 
 * @param nSim SIM卡索引，取值：0-1
 * @param address 短消息中心号码
 * @param address_len 短消息中心号码的数组长度
 * 
 * @return  0 成功 other 错误码 
 */
yopen_sms_errcode_e yopen_sms_get_center_address(uint8_t nSim, char *address, uint8_t address_len);

/**
 * @brief  设置短消息中心号码
 * 
 * @param nSim SIM卡索引，取值：0-1
 * @param address 短消息中心号码
 * 
 * @return  0 成功 other 错误码 
 */
yopen_sms_errcode_e yopen_sms_set_center_address(uint8_t nSim, char *address);

/**
 * @brief  获取SM与ME的存储信息
 * 
 * @param nSim SIM卡索引，取值：0-1
 * @param stor_info 用于保存短信存储信息的结构体
 * 
 * @return  0 成功 other 错误码 
 */
yopen_sms_errcode_e yopen_sms_get_storage_info(uint8_t nSim, yopen_sms_stor_info_s *stor_info);

/**
 * @brief  设置短信存储位置
 * 
 * @param nSim SIM卡索引，取值：0-1
 * @param mem1 读取和删除消息所在内存空间
 * @param mem2 写入和发送消息的内存空间
 * @param mem3 接收消息的存储位置
 * 
 * @return  0 成功 other 错误码 
 */
yopen_sms_errcode_e yopen_sms_set_storage(uint8_t nSim, yopen_sms_stor_e mem1, yopen_sms_stor_e mem2, yopen_sms_stor_e mem3);

/**
 * @brief  获取短信存储位置
 * 
 * @param nSim SIM卡索引，取值：0-1
 * @param mem_info 见yopen_sms_mem_info_s结构体

 * @return  0 成功 other 错误码 
 */
yopen_sms_errcode_e yopen_sms_get_storage(uint8_t nSim, yopen_sms_mem_info_s *mem_info);

/**
 * @brief  注册短信回调函数
 * 
 * @param cb 回调函数
 * 
 * @return
 */
void yopen_sms_callback_register(yopen_sms_event_handler_t cb);

#endif
