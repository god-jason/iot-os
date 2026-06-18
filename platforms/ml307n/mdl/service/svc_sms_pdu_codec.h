/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief service模块短信PDU编解码部分
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */

#ifndef __SVC_SMS_PDU_CODEC_H__
#define __SVC_SMS_PDU_CODEC_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <os.h>


#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/



/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

typedef struct
{
    /* If 'year' is between 96 and 99, the actual year is 1900 + 'year';
     if 'year' is between 00 and 95, the actual year is 2000 + 'year'.
     NOTE: Each field has two BCD digits and UINT8 arrangement is <MSB, ... ,LSB>
    */
    uint8_t      year;        /* 0x00-0x99 */
    uint8_t      month;       /* 0x01-0x12 */
    uint8_t      day;         /* 0x01-0x31 */
    uint8_t      hour;        /* 0x00-0x23 */
    uint8_t      minute;      /* 0x00-0x59 */
    uint8_t      second;      /* 0x00-0x59 */
    int8_t       timezone;    /* +/-, [-48,+48] number of 15 minutes  */
} SVC_SMS_TimeStamp;

typedef enum
{
    SVC_SMS_DIGIT_MODE_4_BIT     = 0,  /* DTMF digits */
    SVC_SMS_DIGIT_MODE_8_BIT     = 1,
} SVC_SMS_DigitMode;

typedef enum
{
    SVC_SMS_NUMBER_MODE_NONE_DATA_NETWORK      = 0,
    SVC_SMS_NUMBER_MODE_DATA_NETWORK           = 1,
} SVC_SMS_NumberMode;

typedef enum
{
    SVC_SMS_NUMBER_UNKNOWN        = 0,

    SVC_SMS_NUMBER_INTERNATIONAL  = 1,
    SVC_SMS_NUMBER_NATIONAL       = 2,
    SVC_SMS_NUMBER_NETWORK        = 3,
    SVC_SMS_NUMBER_SUBSCRIBER     = 4,
    SVC_SMS_NUMBER_ALPHANUMERIC   = 5, /* GSM SMS: addr value is GSM 7-bit chars */
    SVC_SMS_NUMBER_ABBREVIATED    = 6,
    SVC_SMS_NUMBER_RESERVED_7     = 7,

    /* The above are used when number mode is not data network address.
     The following are used only when number mode is data network address
     mode.
    */
    SVC_SMS_NUMBER_DATA_IP        = 1,
    SVC_SMS_NUMBER_INTERNET_EMAIL = 2,
    /* In the address data "digits", each uint8_t contains an ASCII character.
    ** Examples are "x@y.com,a@b.com" - ref TIA/EIA-637A 3.4.3.3.
    */

} SVC_SMS_NumberType;

typedef enum
{
    SVC_SMS_NUMBER_PLAN_UNKNOWN     = 0,
    SVC_SMS_NUMBER_PLAN_TELEPHONY   = 1,  /* CCITT E.164 and E.163, including ISDN plan */
    SVC_SMS_NUMBER_PLAN_RESERVED_2  = 2,
    SVC_SMS_NUMBER_PLAN_DATA        = 3,  /* CCITT X.121 */
    SVC_SMS_NUMBER_PLAN_TELEX       = 4,  /* CCITT F.69 */
    SVC_SMS_NUMBER_PLAN_RESERVED_5  = 5,
    SVC_SMS_NUMBER_PLAN_RESERVED_6  = 6,
    SVC_SMS_NUMBER_PLAN_RESERVED_7  = 7,
    SVC_SMS_NUMBER_PLAN_RESERVED_8  = 8,
    SVC_SMS_NUMBER_PLAN_PRIVATE     = 9,
    SVC_SMS_NUMBER_PLAN_RESERVED_10 = 10,
    SVC_SMS_NUMBER_PLAN_RESERVED_11 = 11,
    SVC_SMS_NUMBER_PLAN_RESERVED_12 = 12,
    SVC_SMS_NUMBER_PLAN_RESERVED_13 = 13,
    SVC_SMS_NUMBER_PLAN_RESERVED_14 = 14,
    SVC_SMS_NUMBER_PLAN_RESERVED_15 = 15,
} SVC_SMS_NumberPlan;

/* Message class:
** The message class in the Data Coding Scheme of a TPDU or CB message
*/
typedef enum
{
    SVC_SMS_MESSAGE_CLASS_0 = 0,
    SVC_SMS_MESSAGE_CLASS_1,
    SVC_SMS_MESSAGE_CLASS_2,
    SVC_SMS_MESSAGE_CLASS_3,
    SVC_SMS_MESSAGE_CLASS_NONE,
} SVC_SMS_MessageClass;

//   Character sets
typedef enum
{
    SVC_SMS_ALPHABET_7_BIT_DEFAULT  = 0,
    SVC_SMS_ALPHABET_8_BIT          = 1,
    SVC_SMS_ALPHABET_UCS2           = 2,
    SVC_SMS_ALPHABET_RESERVED       = 3,
} SVC_SMS_AlphabetType;

//   message waiting indication group
typedef enum
{
    SVC_SMS_GROUP_WAITING_NONE,
    SVC_SMS_GROUP_WAITING_DISCARD,
    SVC_SMS_GROUP_WAITING_STORE,
    SVC_SMS_GROUP_WAITING_NONE_1111,
} SVC_SMS_GroupWaitingType;

// message waiting indcation type
typedef enum
{
    SVC_SMS_MSG_WAITING_VOICEMAIL = 0,
    SVC_SMS_MSG_WAITING_FAX,
    SVC_SMS_MSG_WAITING_EMAIL,
    SVC_SMS_MSG_WAITING_OTHER,
} SVC_SMS_MessageWaitingType;

// Data Coding Scheme
typedef struct
{
    SVC_SMS_MessageClass          msg_class;
    uint8_t                       is_compressed;
    SVC_SMS_AlphabetType          alphabet;
    SVC_SMS_GroupWaitingType      msg_waiting_group;
    uint8_t                       msg_waiting_active;
    SVC_SMS_MessageWaitingType    msg_waiting_kind;
    /* Raw DCS Byte */
    uint8_t                       raw_dcs_data;
} SVC_SMS_DcsInfo;

typedef enum
{
    /* values from 0x00 to 0x1f are for SM-AL protocols:
    */
    SVC_SMS_PID_DEFAULT           = 0x00,  /* default value to use */

    /* values from 0x20 to 0x3f are for telematic interworking:
    */
    SVC_SMS_PID_IMPLICIT          = 0x20,
    SVC_SMS_PID_TELEX             = 0x21,
    SVC_SMS_PID_G3_FAX            = 0x22,
    SVC_SMS_PID_G4_FAX            = 0x23,
    SVC_SMS_PID_VOICE_PHONE       = 0x24,
    SVC_SMS_PID_ERMES             = 0x25,
    SVC_SMS_PID_NAT_PAGING        = 0x26,
    SVC_SMS_PID_VIDEOTEX          = 0x27,
    SVC_SMS_PID_TELETEX_UNSPEC    = 0x28,
    SVC_SMS_PID_TELETEX_PSPDN     = 0x29,
    SVC_SMS_PID_TELETEX_CSPDN     = 0x2a,
    SVC_SMS_PID_TELETEX_PSTN      = 0x2b,
    SVC_SMS_PID_TELETEX_ISDN      = 0x2c,
    SVC_SMS_PID_UCI               = 0x2d,
    SVC_SMS_PID_RESERVED_0x2e     = 0x2e,
    SVC_SMS_PID_RESERVED_0x2f     = 0x2f,
    SVC_SMS_PID_MSG_HANDLING      = 0x30,
    SVC_SMS_PID_X400              = 0x31,
    SVC_SMS_PID_INTERNET_EMAIL    = 0x32,
    SVC_SMS_PID_RESERVED_0x33     = 0x33,
    SVC_SMS_PID_RESERVED_0x34     = 0x34,
    SVC_SMS_PID_RESERVED_0x35     = 0x35,
    SVC_SMS_PID_RESERVED_0x36     = 0x36,
    SVC_SMS_PID_RESERVED_0x37     = 0x37,
    SVC_SMS_PID_SC_SPECIFIC_1     = 0x38,
    SVC_SMS_PID_SC_SPECIFIC_2     = 0x39,
    SVC_SMS_PID_SC_SPECIFIC_3     = 0x3a,
    SVC_SMS_PID_SC_SPECIFIC_4     = 0x3b,
    SVC_SMS_PID_SC_SPECIFIC_5     = 0x3c,
    SVC_SMS_PID_SC_SPECIFIC_6     = 0x3d,
    SVC_SMS_PID_SC_SPECIFIC_7     = 0x3e,
    SVC_SMS_PID_GSM_UMTS          = 0x3f,

    /* values from 0x40 to 0x7f: */
    SVC_SMS_PID_SM_TYPE_0         = 0x40,
    SVC_SMS_PID_REPLACE_SM_1      = 0x41,
    SVC_SMS_PID_REPLACE_SM_2      = 0x42,
    SVC_SMS_PID_REPLACE_SM_3      = 0x43,
    SVC_SMS_PID_REPLACE_SM_4      = 0x44,
    SVC_SMS_PID_REPLACE_SM_5      = 0x45,
    SVC_SMS_PID_REPLACE_SM_6      = 0x46,
    SVC_SMS_PID_REPLACE_SM_7      = 0x47,
    SVC_SMS_PID_DEVICE_TRIG_SM    = 0x48,
    /* values from 0x49 to 0x5d are reserved */
    SVC_SMS_PID_EMS               = 0x5e,
    SVC_SMS_PID_RETURN_CALL       = 0x5f,
    /* values from 0x60 to 0x7b are reserved */
    SVC_SMS_PID_ANSI136_R_DATA    = 0x7c,
    SVC_SMS_PID_ME_DATA_DOWNLOAD  = 0x7d,
    SVC_SMS_PID_ME_DEPERSONALIZE  = 0x7e,
    SVC_SMS_PID_SIM_DATA_DOWNLOAD = 0x7f,

    /* values from 0x80 to 0xbf are reserved */

    /* values from 0xc0 to 0xff are for SC specific use */

} SVC_SMS_PID;


/* Validity Period Formats TP-VPF */
typedef enum
{
    SVC_SMS_VALIDITY_PERIOD_NONE = 0,
    SVC_SMS_VALIDITY_PERIOD_RELATIVE = 2, // relative format
    SVC_SMS_VALIDITY_PERIOD_ABSOLUTE = 3, // absolute format
    SVC_SMS_VALIDITY_PERIOD_ENHANCED = 1, // enhanced format
} SVC_SMS_ValidityPeriodFormat;

/* Validity Period info TP-VP */
typedef struct
{
    SVC_SMS_ValidityPeriodFormat format;
    SVC_SMS_TimeStamp            time;
} SVC_SMS_ValidityPeriodInfo;


#define SVC_SMS_GW_ADDRESS_MAX   (20)
#define SVC_SMS_ADDRESS_MAX      (48)

typedef struct
{
    /* Indicates 4-bit or 8-bit */
    SVC_SMS_DigitMode   digit_mode;

    /* Used in CDMA only: only meaningful when digit_mode is 8-bit */
    SVC_SMS_NumberMode  number_mode;

    /* In case of CDMA address, this is used only when digit_mode is 8-bit.
    ** To specify an international address for CDMA, use the following:
    **   digit_mode  = 8-bit
    **   number_mode = NONE_DATA_NETWORK
    **   number_type = INTERNATIONAL
    **   number_plan = TELEPHONY
    **   number_of_digits = number of digits
    **   digits = ASCII digits, e.g. '1', '2', '3', '4' and '5'.
    */
    SVC_SMS_NumberType  number_type;

    /* In case of CDMA address, this is used only when digit_mode is 8-bit. */
    SVC_SMS_NumberPlan  number_plan;

    uint8_t             number_of_digits;
    /* Each uint8_t in this array represents a 4-bit or 8-bit digit of
    ** address data.
    */
    uint8_t             digits[SVC_SMS_ADDRESS_MAX];
} SVC_SMS_AddressInfo;

/* User Data Header Information Element Identifier */
typedef enum
{
    SVC_SMS_UDH_IEI_CONCATENATED_SM_8           = 0x00,
    SVC_SMS_UDH_IEI_SPECIAL_SMS_MESSAGE         = 0x01,
    /* 02 Reserved */
    SVC_SMS_UDH_IEI_NOT_USED                    = 0x03,
    SVC_SMS_UDH_IEI_APP_PORT_ADDR_8             = 0x04,
    SVC_SMS_UDH_IEI_APP_PORT_ADDR_16            = 0x05,
    SVC_SMS_UDH_IEI_SMSC_CONTROL_PARAMS         = 0x06,
    SVC_SMS_UDH_IEI_UDH_SOURCE_INDICATOR        = 0x07,
    SVC_SMS_UDH_IEI_CONCATENATED_SM_16          = 0x08,
    SVC_SMS_UDH_IEI_WIRELESS_CONTROL_MESSAGE_PROTOCOL = 0x09,
    SVC_SMS_UDH_IEI_TEXT_FORMATING              = 0x0A,
    SVC_SMS_UDH_PRE_DEFINED_SOUND               = 0x0B,
    SVC_SMS_UDH_USER_DEFINED_SOUND              = 0x0C,
    SVC_SMS_UDH_PRE_DEFINED_ANIM                = 0x0D,
    SVC_SMS_UDH_LARGE_ANIMATION                 = 0x0E,
    SVC_SMS_UDH_SMALL_ANIMATION                 = 0x0F,
    SVC_SMS_UDH_LARGE_PICTURE                   = 0x10,
    SVC_SMS_UDH_SMALL_PICTURE                   = 0x11,
    SVC_SMS_UDH_VARIABLE_PICTURE                = 0x12,

    SVC_SMS_UDH_USER_PROMPT_INDICATOR           = 0x13,
    SVC_SMS_UDH_EXTENDED_OBJECT                 = 0x14,

    SVC_SMS_UDH_REUSED_EXTENDED_OBJECT          = 0x15,
    SVC_SMS_UDH_COMPRESSION_CONTROL             = 0x16,
    SVC_SMS_UDH_OBJECT_DISTRIBUTION_INDICATOR   = 0x17,
    SVC_SMS_UDH_STANDARD_WVG_OBJECT             = 0x18,
    SVC_SMS_UDH_CHARACTER_SIZE_WVG_OBJECT       = 0x19,
    SVC_SMS_UDH_EXTENDED_OBJECT_DATA_REQUEST_COMMAND = 0x1A,
    /* 1B - 1F    Reserved for future EMS */

    SVC_SMS_UDH_RFC5322_EMAIL_HEADER            = 0x20,
    SVC_SMS_UDH_HYPERLINK_FORMAT_ELEMENT        = 0x21,
    SVC_SMS_UDH_REPLY_ADDRESS_ELEMENT           = 0x22,
    SVC_SMS_UDH_ENHANCED_VOICE_MAIL_INFORMATION = 0x23,
    SVC_SMS_UDH_NATIONAL_LANGUAGE_SINGLE_SHIFT  = 0x24,
    SVC_SMS_UDH_NATIONAL_LANGUAGE_LOCKING_SHIFT = 0x25,

    /*  21 - 23, 26 - 6F    Reserved for future use */
    /*  70 - 7f    Reserved for (U)SIM Toolkit Security Headers */
    /*  80 - 9F    SME to SME specific use */
    /*  A0 - BF    Reserved for future use */
    /*  C0 - DF    SC specific use */
    /*  E0 - FF    Reserved for future use */
} SVC_SMS_UDH_IEIType;

//   长短信, 连接信息
typedef struct
{
    /*
       Concatenated short message reference number:
       counter indicating the reference number for a particular concatenated short message.
       This reference number shall remain constant for every short message which makes up a particular concatenated short message.
    */
    uint16_t    reference_num; //  拼接短信的reference number
    /*
        Maximum number of short messages in the concatenated short message:
        indicating the total number of short messages within the concatenated short message.
    */
    uint8_t     max_num;   //  拼接短信的总数
    /*
        Sequence number of the current short message:
        indicating the sequence number of a particular short message within the concatenated short message.
    */
    uint8_t     seq_num;   //  当前短信的序号
}SVC_SMS_ConcatenatedInfo;


#define SVC_SMS_SM_DATA_MAX_LEN     (160)

typedef struct
{
    uint8_t                     is_concat_sms; //   是否拼接短信 1表示是拼接短信
    SVC_SMS_ConcatenatedInfo    concat_info; //   当is_concat_sms为1时，该字段值有效

    uint16_t                    sm_len;
    /* If DCS indicates the default 7-bit alphabet, each uint8_t holds one character
    ** with bit 7 equal to 0 and sm_len indicates the number of characters;
    ** If DCS indicates otherwise, each character may occupy multiple bytes
    ** and sm_len indicates the total number of bytes.
    */
    uint8_t                     sm_data[SVC_SMS_SM_DATA_MAX_LEN+1]; // 多定义一个字节，存放字符中结束符0
} SVC_SMS_UserData;

typedef struct
{
    uint8_t                          type;                     /* TP-MTI */
    uint8_t                          is_more;                     /* TP-MMS */
    //uint8_t                          loop_prevention;          /* TP-LP  */
    uint8_t                          reply_path;               /* TP-RP */
    uint8_t                          user_data_header_indicator; /* TP-UDHI */
    uint8_t                          status_report_indication;    /* TP-SRI */
    SVC_SMS_AddressInfo              address;                  /* TP-OA */
    uint8_t                          tp_pid;                      /* TP-PID */
    SVC_SMS_DcsInfo                  tp_dcs;                      /* TP-DCS */
    SVC_SMS_TimeStamp                timestamp;                /* TP-SCTS */
    SVC_SMS_UserData                 user_data;                /* TP-UD */
} SVC_SMS_DeliverInfo;

typedef struct
{
//    uint8_t                           SCA[44];           // 短消息服务中心号码(SMSC地址)

    uint8_t                           reject_duplicates;        /* TP-RD 0:可以重复接收  1: 不能重复接收*/
    uint8_t                           reply_path_present;       /* TP-RP 0:不携带应答路径*/
    uint8_t                           TP_UDHI;                 /* TP-UDHI 1: 有header */
    uint8_t                           TP_SRR;                 /* TP-SRR 1:允许状态报告 */
    uint8_t                           TP_MR;                  /* TP-MR 短信发送计数器 */
    SVC_SMS_AddressInfo               address;                  /* TP-DA sc的路由 */
    SVC_SMS_PID                       TP_PID;                      /* TP-PID 协议id */
    SVC_SMS_DcsInfo                   TP_DCS;                      /* TP-DCS 短信编码模式 */
    SVC_SMS_ValidityPeriodInfo        TP_VP;                 /* TP-VPF & TP-VP 短信有效生命期 */
    SVC_SMS_UserData                  user_data;                /* TP-UD */
} SVC_SMS_SubmitInfo;

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
int svc_sms_encode_sca(const char *str_sca, uint8_t *out_data);
int svc_sms_decode(const uint8_t *in_pdu, const uint16_t in_pdu_length, SVC_SMS_DeliverInfo *out_deliver_info);
int svc_sms_encode(SVC_SMS_SubmitInfo *in_submit_info, uint8_t *out_pdu, const uint16_t out_pdu_length);

#ifdef __cplusplus
}
#endif
#endif /* __SVC_SMS_PDU_CODEC_H__ */
