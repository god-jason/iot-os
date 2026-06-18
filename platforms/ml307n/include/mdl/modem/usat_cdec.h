#ifndef _USAT_CDEC_H_
#define _USAT_CDEC_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ap_ps_interface.h"

#define BIP_SUPPORT

/*terminal profile byte 1*/
#define USAT_TP_PROFILE_DOWNLOAD              (uint8_t)0
#define USAT_TP_SMS_PP_DOWNLOAD               (uint8_t)1
#define USAT_TP_CELL_BROADCAST_DOWNLOAD       (uint8_t)2
#define USAT_TP_MENU_SELECTION                (uint8_t)3
#define USAT_TP_SMS_PP_DOWNLOAD_SUPPORT       (uint8_t)4
#define USAT_TP_TIMER_EXPIRATION              (uint8_t)5
#define USAT_TP_CALL_CONTROL_SUPPORT_1        (uint8_t)6
#define USAT_TP_CALL_CONTROL_SUPPORT_2        (uint8_t)7

/*terminal profile byte 2*/
#define USAT_TP_COMMAND_RESULT                (uint8_t)8
#define USAT_TP_CALL_CONTROL                  (uint8_t)9
#define USAT_TP_CALL_CONTROL_SUPPORT3         (uint8_t)10
#define USAT_TP_MO_SHORT_MSG_CONTROL          (uint8_t)11
#define USAT_TP_CALL_CONTROL_SUPPORT4         (uint8_t)12
#define USAT_TP_UCS2_ENTRY_SUPPORTED          (uint8_t)13
#define USAT_TP_UCS2_DISPLAY_SUPPORTED        (uint8_t)14
#define USAT_TP_DISPLAY_TEXT                  (uint8_t)15

/*terminal profile byte 3*/
#define USAT_TP_PROC_UICC_DISPLAY_TEXT        (uint8_t)16
#define USAT_TP_PROC_UICC_GET_INKEY           (uint8_t)17
#define USAT_TP_GET_INPUT                     (uint8_t)18
#define USAT_TP_MORE_TIME                     (uint8_t)19
#define USAT_TP_PLAY_TONE                     (uint8_t)20
#define USAT_TP_POLL_INTERVAL                 (uint8_t)21
#define USAT_TP_POLLING_OFF                   (uint8_t)22
#define USAT_TP_REFRESH                       (uint8_t)23

/*terminal profile byte 4*/
#define USAT_TP_SELECT_ITEM                    (uint8_t)24
#define USAT_TP_SEND_SHORT_MESSAGE             (uint8_t)25
#define USAT_TP_SEND_SS                        (uint8_t)26
#define USAT_TP_SEND_USSD                      (uint8_t)27
#define USAT_TP_PROC_UICC_SET_UP_CALL          (uint8_t)28
#define USAT_TP_SET_UP_MENU                    (uint8_t)29
#define USAT_TP_PROVIDE_LOCAL_INFO_MCC_IMEI    (uint8_t)30
#define USAT_TP_PROVIDE_LOCAL_INFO_NMR         (uint8_t)31

/*terminal profile byte 5*/
#define USAT_TP_SET_UP_EVENT_LIST              (uint8_t)32
#define USAT_TP_EVENT_MT_CALL                  (uint8_t)33
#define USAT_TP_EVENT_CALL_CONNECTED           (uint8_t)34
#define USAT_TP_EVENT_CALL_DISCONNECTED        (uint8_t)35
#define USAT_TP_EVENT_LOCATIOAN_STATUS         (uint8_t)36
#define USAT_TP_EVENT_USER_ACTIVITY            (uint8_t)37
#define USAT_TP_EVENT_IDLE_SCREEN_AVAILABLE    (uint8_t)38
#define USAT_TP_EVENT_CARD_READER_STATUS       (uint8_t)39

/*terminal profile byte 6*/
#define USAT_TP_EVENT_LANGUAGE_SELECTION       (uint8_t)40
#define USAT_TP_EVENT_BROWSER_TERMINATION      (uint8_t)41
#define USAT_TP_EVENT_DATA_AVAILABLE           (uint8_t)42
#define USAT_TP_EVENT_CHANNEL_STATUS           (uint8_t)43
#define USAT_TP_EVENT_ACCESS_TECH_CHANGE       (uint8_t)44
#define USAT_TP_EVENT_DISPLAY_PARAM_CHANGED    (uint8_t)45
#define USAT_TP_EVENT_LOCAL_CONNECTION         (uint8_t)46
#define USAT_TP_EVENT_NW_SEARCH_MODE_CHANGE    (uint8_t)47

/*terminal profile byte 7*/
#define USAT_TP_POWER_ON_CARD                  (uint8_t)48
#define USAT_TP_POWER_OFF_CARD                 (uint8_t)49
#define USAT_TP_PERFORM_APDU                   (uint8_t)50
#define USAT_TP_GET_CARD_READER_STATUS         (uint8_t)51
#define USAT_TP_GET_CARD_READER_ID             (uint8_t)52
#define USAT_TP_BYTE_7_RFU_1                   (uint8_t)53
#define USAT_TP_BYTE_7_RFU_2                   (uint8_t)54
#define USAT_TP_BYTE_7_RFU_3                   (uint8_t)55

/*terminal profile byte 8*/
#define USAT_TP_TIMER_MNG_START_STOP           (uint8_t)56
#define USAT_TP_TIMER_MNG_GET_CURRENT_VALUE    (uint8_t)57
#define USAT_TP_PROVIDE_LOCAL_INFO_DATE_TIME   (uint8_t)58
#define USAT_TP_GET_INKEY                      (uint8_t)59
#define USAT_TP_SET_UP_IDLE_MODE_TEXT          (uint8_t)60
#define USAT_TP_RUN_AT_COMMAND                 (uint8_t)61
#define USAT_TP_SET_UP_CALL                    (uint8_t)62
#define USAT_TP_CALL_CONTROL_BY_NAA            (uint8_t)63

/*terminal profile byte 9*/
#define USAT_TP_DISPLAY_TEXT_2                 (uint8_t)64
#define USAT_TP_SEND_DTMF_COMMAND              (uint8_t)65
#define USAT_TP_PROVIDE_LOCAL_INFO_NMR_2       (uint8_t)66
#define USAT_TP_PROVIDE_LOCAL_INFO_LANGUAGE    (uint8_t)67
#define USAT_TP_PROVIDE_LOCAL_INFO_TIME_ADVAN  (uint8_t)68
#define USAT_TP_LANGUAGE_NOTIFICATION          (uint8_t)69
#define USAT_TP_LAUNCH_BROWSER                 (uint8_t)70
#define USAT_TP_PROVIDE_LOCAL_INFO_ACCESS_TECH (uint8_t)71

/*terminal profile byte 10*/
#define USAT_TP_SOFT_KEY_SUPPORT_SELECT_ITEM   (uint8_t)72
#define USAT_TP_SOFT_KEY_SUPPORT_SET_UP_MENU   (uint8_t)73
#define USAT_TP_BYTE_10_RFU_1                  (uint8_t)74
#define USAT_TP_BYTE_10_RFU_2                  (uint8_t)75
#define USAT_TP_BYTE_10_RFU_3                  (uint8_t)76
#define USAT_TP_BYTE_10_RFU_4                  (uint8_t)77
#define USAT_TP_BYTE_10_RFU_5                  (uint8_t)78
#define USAT_TP_BYTE_10_RFU_6                  (uint8_t)79

/*terminal profile byte 11, soft key information, set 'FF' for future use*/
#define USAT_TP_BYTE_11_RFU_1                  (uint8_t)80
#define USAT_TP_BYTE_11_RFU_2                  (uint8_t)81
#define USAT_TP_BYTE_11_RFU_3                  (uint8_t)82
#define USAT_TP_BYTE_11_RFU_4                  (uint8_t)83
#define USAT_TP_BYTE_11_RFU_5                  (uint8_t)84
#define USAT_TP_BYTE_11_RFU_6                  (uint8_t)85
#define USAT_TP_BYTE_11_RFU_7                  (uint8_t)86
#define USAT_TP_BYTE_11_RFU_8                  (uint8_t)87

/*terminal profile byte 12 BIP*/
#define USAT_TP_OPEN_CHANNEL                   (uint8_t)88
#define USAT_TP_CLOSE_CHANNEL                  (uint8_t)89
#define USAT_TP_RECEIVE_DATA                   (uint8_t)90
#define USAT_TP_SEND_DATA                      (uint8_t)91
#define USAT_TP_GET_CHANNEL_STATUS             (uint8_t)92
#define USAT_TP_SERVICE_SEARCH                 (uint8_t)93
#define USAT_TP_GET_SERVICE_INFORMATION        (uint8_t)94
#define USAT_TP_DECLARE_SERVICE                (uint8_t)95

/*terminal profile byte 13 BIP*/
#define USAT_TP_CSD                            (uint8_t)96
#define USAT_TP_GPRS                           (uint8_t)97
#define USAT_TP_BLUETOOTH                      (uint8_t)98
#define USAT_TP_IRDA                           (uint8_t)99
#define USAT_TP_RS232                          (uint8_t)100
#define USAT_TP_NUMBER_OF_CHANNELS_SUPPORTED_BIT_1       (uint8_t)101
#define USAT_TP_NUMBER_OF_CHANNELS_SUPPORTED_BIT_2       (uint8_t)102
#define USAT_TP_NUMBER_OF_CHANNELS_SUPPORTED_BIT_3       (uint8_t)103

/*terminal profile byte 14 */
#define USAT_TP_NUM_OF_CHAR_DOWN_TERMINAL_DISPLAY_BIT_1  (uint8_t)104
#define USAT_TP_NUM_OF_CHAR_DOWN_TERMINAL_DISPLAY_BIT_2  (uint8_t)105
#define USAT_TP_NUM_OF_CHAR_DOWN_TERMINAL_DISPLAY_BIT_3  (uint8_t)106
#define USAT_TP_NUM_OF_CHAR_DOWN_TERMINAL_DISPLAY_BIT_4  (uint8_t)107
#define USAT_TP_NUM_OF_CHAR_DOWN_TERMINAL_DISPLAY_BIT_5  (uint8_t)108
#define USAT_TP_NO_DISPLAY_CAPABILITY                    (uint8_t)109
#define USAT_TP_NO_KEYPAD_CAPABILITY                     (uint8_t)110
#define USAT_TP_SCREEN_SIZE_PARM_SUPPORTED               (uint8_t)111

/*terminal profile byte 15 */
#define USAT_TP_NUM_OF_CHAR_CROSS_TERMINAL_DISPLAY_BIT_1 (uint8_t)112
#define USAT_TP_NUM_OF_CHAR_CROSS_TERMINAL_DISPLAY_BIT_2 (uint8_t)113
#define USAT_TP_NUM_OF_CHAR_CROSS_TERMINAL_DISPLAY_BIT_3 (uint8_t)114
#define USAT_TP_NUM_OF_CHAR_CROSS_TERMINAL_DISPLAY_BIT_4 (uint8_t)115
#define USAT_TP_NUM_OF_CHAR_CROSS_TERMINAL_DISPLAY_BIT_5 (uint8_t)116
#define USAT_TP_NUM_OF_CHAR_CROSS_TERMINAL_DISPLAY_BIT_6 (uint8_t)117
#define USAT_TP_NUM_OF_CHAR_CROSS_TERMINAL_DISPLAY_BIT_7 (uint8_t)118
#define USAT_TP_VARIABLE_SIZE_FONTS                      (uint8_t)119

/*terminal profile byte 16 */
#define USAT_TP_DISPLAY_CAN_BE_RESIZED                   (uint8_t)120
#define USAT_TP_TEXT_WRAPPING_SUPPORTED                  (uint8_t)121
#define USAT_TP_TEXT_SCROLLING_SUPPORTED                 (uint8_t)122
#define USAT_TP_TEXT_ATTRIBUTES_SUPPORTED                (uint8_t)123
#define USAT_TP_BYTE_16_RFU                              (uint8_t)124
#define USAT_TP_WIDTH_REDUCTION_IN_MENU_BIT_1            (uint8_t)125
#define USAT_TP_WIDTH_REDUCTION_IN_MENU_BIT_2            (uint8_t)126
#define USAT_TP_WIDTH_REDUCTION_IN_MENU_BIT_3            (uint8_t)127

/*terminal profile byte 17 */
#define USAT_TP_TCP_UICC_CLIENT_REMOTE_CONNECTION        (uint8_t)128
#define USAT_TP_UDP_UICC_CLIENT_REMOTE_CONNECTION        (uint8_t)129
#define USAT_TP_TCP_UICC_SERVER                          (uint8_t)130
#define USAT_TP_TCP_UICC_CLIENT_LOCAL_CONNECTION         (uint8_t)131
#define USAT_TP_UDP_UICC_CLIENT_LOCAL_CONNECTION         (uint8_t)132
#define USAT_TP_DIRECT_COMMUNICATE_CHANNEL               (uint8_t)133
#define USAT_TP_E_UTRAN                                  (uint8_t)134
#define USAT_TP_HSDPA                                    (uint8_t)135

/*terminal profile byte 18 */
#define USAT_TP_DISPLAY_TEXT_VARIABLE_TIMEOUT            (uint8_t)136
#define USAT_TP_GET_INKEY_HELP_SUPPORT                   (uint8_t)137
#define USAT_TP_USB                                      (uint8_t)138
#define USAT_TP_GET_INKEY_VARIABLE_TIMEOUT               (uint8_t)139
#define USAT_TP_PROVIDE_LOCAL_INFO_ESN                   (uint8_t)140
#define USAT_TP_CALL_CONTROL_ON_GPRS                     (uint8_t)141
#define USAT_TP_PROVIDE_LOCAL_INFO_IMEISV                (uint8_t)142
#define USAT_TP_PROVIDE_LOCAL_INFO_SEARCH_MODE_CHANGE    (uint8_t)143

/*terminal profile byte 19 */
#define USAT_TP_PROTOCOL_VERSION_BIT_1                   (uint8_t)144
#define USAT_TP_PROTOCOL_VERSION_BIT_2                   (uint8_t)145
#define USAT_TP_PROTOCOL_VERSION_BIT_3                   (uint8_t)146
#define USAT_TP_PROTOCOL_VERSION_BIT_4                   (uint8_t)147
#define USAT_TP_BYTE_19_RFU_1                            (uint8_t)148
#define USAT_TP_BYTE_19_RFU_2                            (uint8_t)149
#define USAT_TP_BYTE_19_RFU_3                            (uint8_t)150
#define USAT_TP_BYTE_19_RFU_4                            (uint8_t)151

/*terminal profile byte 20 */
#define USAT_TP_BYTE_20_RFU_1                            (uint8_t)152
#define USAT_TP_BYTE_20_RFU_2                            (uint8_t)153
#define USAT_TP_BYTE_20_RFU_3                            (uint8_t)154
#define USAT_TP_BYTE_20_RFU_4                            (uint8_t)155
#define USAT_TP_BYTE_20_RFU_5                            (uint8_t)156
#define USAT_TP_BYTE_20_RFU_6                            (uint8_t)157
#define USAT_TP_BYTE_20_RFU_7                            (uint8_t)158
#define USAT_TP_BYTE_20_RFU_8                            (uint8_t)159

/*terminal profile byte 21 */
#define USAT_TP_XML                                      (uint8_t)160
#define USAT_TP_XHTML                                    (uint8_t)161
#define USAT_TP_HTML                                     (uint8_t)162
#define USAT_TP_CHTML                                    (uint8_t)163
#define USAT_TP_BYTE_21_RFU_1                            (uint8_t)164
#define USAT_TP_BYTE_21_RFU_2                            (uint8_t)165
#define USAT_TP_BYTE_21_RFU_3                            (uint8_t)166
#define USAT_TP_BYTE_21_RFU_4                            (uint8_t)167

/*terminal profile byte 22 */
#define USAT_TP_UTRAN_PS_WITH_EXTEND_PARM                (uint8_t)168
#define USAT_TP_PROVIDE_LOCAL_INFO_BATTERY_STATE         (uint8_t)169
#define USAT_TP_PLAY_TONE_MELODY_THEMED_SUPPORT          (uint8_t)170
#define USAT_TP_MULTI_MEDIA_CALL_IN_SET_UP_CALL          (uint8_t)171
#define USAT_TP_TOOLKIT_INITEATED_GBA                    (uint8_t)172
#define USAT_TP_PROC_UICC_RETRIEVE_MMS                   (uint8_t)173
#define USAT_TP_PROC_UICC_SUBMIT_MMS                     (uint8_t)174
#define USAT_TP_PROC_UICC_DISPLAY_MMS                    (uint8_t)175

/*terminal profile byte 23 */
#define USAT_TP_PROC_UICC_SET_FRAMES                     (uint8_t)176
#define USAT_TP_PROC_UICC_GET_FRAME_STATUS               (uint8_t)177
#define USAT_TP_MMS_NOTIFICATION_DOWNLOAD                (uint8_t)178
#define USAT_TP_ALPHA_ID_IN_REFRESH_SUPPORT_BY_TERMINAL  (uint8_t)179
#define USAT_TP_GEOGRAPHICAL_LOCATION_REPORTING          (uint8_t)180
#define USAT_TP_PROC_UICC_PROVIDE_LOCAL_INFO_MEID        (uint8_t)181
#define USAT_TP_PROC_UICC_PROVIDE_LOCAL_INFO_NMR_UTRAN   (uint8_t)182
#define USAT_TP_USSD_DATA_DOWNLOAD_AND_APP_MODE          (uint8_t)183

/*terminal profile byte 24*/
#define USAT_TP_MAX_FRAME_NUM_BIT_1                      (uint8_t)184
#define USAT_TP_MAX_FRAME_NUM_BIT_2                      (uint8_t)185
#define USAT_TP_MAX_FRAME_NUM_BIT_3                      (uint8_t)186
#define USAT_TP_MAX_FRAME_NUM_BIT_4                      (uint8_t)187
#define USAT_TP_BYTE_24_RFU_1                            (uint8_t)188
#define USAT_TP_BYTE_24_RFU_2                            (uint8_t)189
#define USAT_TP_BYTE_24_RFU_3                            (uint8_t)190
#define USAT_TP_BYTE_24_RFU_4                            (uint8_t)191

/*terminal profile byte 25*/
#define USAT_TP_EVENT_BROWSING_STATUS                    (uint8_t)192
#define USAT_TP_EVENT_MMS_TRANSFER_STATUS                (uint8_t)193
#define USAT_TP_EVENT_FRAME_INFO_CHANGED                 (uint8_t)194
#define USAT_TP_EVENT_I_WLAN_ACCESS_STATUS               (uint8_t)195
#define USAT_TP_EVENT_NETWORT_REJECTION                  (uint8_t)196
#define USAT_TP_EVENT_HCI_CONNECTIVITY_EVENT             (uint8_t)197
#define USAT_TP_E_UTRAN_SUPPORT_IN_EVENT_NETWORK_REJECT  (uint8_t)198
#define USAT_TP_MULTIPLE_ACCESS_TECH_SUPPORT             (uint8_t)199

/*terminal profile byte 26*/
#define USAT_TP_EVENT_CSG_CELL_SELECTION                 (uint8_t)200
#define USAT_TP_EVENT_CONTACTLESS_STATE_REQUEST          (uint8_t)201
#define USAT_TP_BYTE_26_RFU_1                            (uint8_t)202
#define USAT_TP_BYTE_26_RFU_2                            (uint8_t)203
#define USAT_TP_BYTE_26_RFU_3                            (uint8_t)204
#define USAT_TP_BYTE_26_RFU_4                            (uint8_t)205
#define USAT_TP_BYTE_26_RFU_5                            (uint8_t)206
#define USAT_TP_BYTE_26_RFU_6                            (uint8_t)207

/*terminal profile byte 27*/
#define USAT_TP_BYTE_27_RFU_1                            (uint8_t)208
#define USAT_TP_BYTE_27_RFU_2                            (uint8_t)209
#define USAT_TP_BYTE_27_RFU_3                            (uint8_t)210
#define USAT_TP_BYTE_27_RFU_4                            (uint8_t)211
#define USAT_TP_BYTE_27_RFU_5                            (uint8_t)212
#define USAT_TP_BYTE_27_RFU_6                            (uint8_t)213
#define USAT_TP_BYTE_27_RFU_7                            (uint8_t)214
#define USAT_TP_BYTE_27_RFU_8                            (uint8_t)215

/*terminal profile byte 28*/
#define USAT_TP_ALIGNMENT_LEFT_SUPPORT_BY_TERMINAL       (uint8_t)216
#define USAT_TP_ALIGNMENT_CENTRE_SUPPORT_BY_TERMINAL     (uint8_t)217
#define USAT_TP_ALIGNMENT_RIGHT_SUPPORT_BY_TERMINAL      (uint8_t)218
#define USAT_TP_FONT_SIZE_NORMAL_SUPPORT_BY_TERMINAL     (uint8_t)219
#define USAT_TP_FONT_SIZE_LARGE_SUPPORT_BY_TERMINAL      (uint8_t)220
#define USAT_TP_FONT_SIZE_SMALL_SUPPORT__BY_TERMINAL     (uint8_t)221
#define USAT_TP_BYTE_28_RFU_1                            (uint8_t)222
#define USAT_TP_BYTE_28_RFU_2                            (uint8_t)223

/*terminal profile byte 29*/
#define USAT_TP_STYLE_NORMAL_SUPPORT_BY_TERMINAL         (uint8_t)224
#define USAT_TP_STYLE_BOLD_SUPPORT_BY_TERMINAL           (uint8_t)225
#define USAT_TP_STYLE_ITALIC_SUPPORT_BY_TERMINAL         (uint8_t)226
#define USAT_TP_STYLE_UNDERLINED_SUPPORT_BY_TERMINAL     (uint8_t)227
#define USAT_TP_STYLE_STRIKETHROUGH_SUPPORT_BY_TERMINAL  (uint8_t)228
#define USAT_TP_STYLE_TEXT_FORGROUND_COLOUR_SUPPORT_BY_TERMINAL  (uint8_t)229
#define USAT_TP_STYLE_TEXT_BACKGROUND_COLOUR_SUPPORT_BY_TERMINAL (uint8_t)230
#define USAT_TP_BYTE_29_RFU  (uint8_t)231

/*terminal profile byte 30*/
#define USAT_TP_I_WLAN_BEARER_SUPPORT                    (uint8_t)232
#define USAT_TP_PROVIDE_LOCAL_INFO_WSID_OF_I_WLAN        (uint8_t)233
#define USAT_TP_TERMINAL_APPLICATIONS                    (uint8_t)234
#define USAT_TP_STEERING_OF_ROAMING_REFRESH_SUPPORT      (uint8_t)235
#define USAT_TP_ACTIVATE                                 (uint8_t)236
#define USAT_TP_GEOGRAPHICAL_LOCATION_REQUEST            (uint8_t)237
#define USAT_TP_PROVIDE_LOCAL_INFO_BROADCAST_NETWORK_INFO (uint8_t)238
#define USAT_TP_STEERING_OF_ROAMING_FOR_I_WLAN_REFRESH_SUPPORT  (uint8_t)239

/*terminal profile byte 31*/
#define USAT_TP_PROC_UICC_CONTACTLESS_STATE_CHANGED      (uint8_t)240
#define USAT_TP_SUPPORT_OF_CSG_CELL_DISCOVERY            (uint8_t)241
#define USAT_TP_CONFIRM_PARM_SUPPORT_FOR_OPEN_CHANNEL_IN_TERM_SERV_MODE  (uint8_t)242
#define USAT_TP_COMMUNICATION_CONTROL_FOR_IMS            (uint8_t)243
#define USAT_TP_SUPPORT_OF_CAT_OVER_MODEM_INTERFACE      (uint8_t)244
#define USAT_TP_SUPPORT_FOR_INCOMING_IMS_DATA            (uint8_t)245
#define USAT_TP_SUPPORT_FOR_REG_IMS_DATA                 (uint8_t)246
#define USAT_TP_PROC_UICC_PROFILE_ENV_CMD_CONTANER       (uint8_t)247

/*terminal profile byte 32*/
#define USAT_TP_SUPPORT_OF_IMS_AS_BEARER_FOR_BIP         (uint8_t)248
#define USAT_TP_BYTE_32_RFU_1                            (uint8_t)249
#define USAT_TP_BYTE_32_RFU_2                            (uint8_t)250
#define USAT_TP_BYTE_32_RFU_3                            (uint8_t)251
#define USAT_TP_BYTE_32_RFU_4                            (uint8_t)252
#define USAT_TP_BYTE_32_RFU_5                            (uint8_t)253
#define USAT_TP_BYTE_32_RFU_6                            (uint8_t)254
#define USAT_TP_BYTE_32_RFU_7                            (uint8_t)255

typedef struct
{
    uint8_t bCmdType;
    uint8_t bCmdQual;
    uint8_t bTpIndex;
}S_Usat_CmdTpMap;

#define USAT_TAG_HIGH_BIT_MASK              0x80
#define USAT_PROACTIVE_CMD_MIN_LEN          11
#define USAT_TAG_LEN_OFFSET                 2

#define USAT_IMEI_LEN                       8
#define USAT_IMEISV_LEN                     9
#define USAT_MAX_TPDU_LEN                   184
#define USAT_MAX_FILE_LIST_LEN              240
#define USAT_MAX_ADD_RESULT_LEN             4
#define USAT_MAX_ENVELOPE_CMD_BUF_NUM       5
#define USAT_TERMINAL_RESPONE_MIN_LEN       12
#define USAT_ENVELOPE_CMD_MIN_LEN           9
#define USAT_MAX_MNR_RLT_LEN                16
#define USAT_CMD_DETAIL_CODE_LEN            5
#define USAT_DEVICE_ID_CODE_LEN             4
#define USIM_EID_LEN                        10

#define USAT_MAX_BEARER_PARAM_LEN           14
#define USAT_MAX_OTHER_ADDR_LEN             16
#define USAT_MAX_APN_LEN                    50
#define USAT_MAX_TEXT_STR_LEN               30
#define USAT_MAX_ADD_RESULT_LEN             4

#define USAT_EVT_DATA_AVAILABLE             0x09
#define USAT_EVT_CHANNEL_STATUS             0x0A

#define USAT_PROAC_CMD_TYPE_OPEN_CHANNEL       0x40
#define USAT_PROAC_CMD_TYPE_CLOSE_CHANNEL      0x41
#define USAT_PROAC_CMD_TYPE_RECEIVE_DATA       0x42
#define USAT_PROAC_CMD_TYPE_SEND_DATA          0x43
#define USAT_PROAC_CMD_TYPE_GET_CHANNEL_STATUS 0x44

#define USAT_COMMAND_DETAILS_TAG   0x01 
#define USAT_DEVICE_IDENTITY_TAG   0x02 
#define USAT_RESULT_TAG            0x03 
#define USAT_DURATION_TAG          0x04 
#define USAT_ALPHA_IDENTIFIER_TAG  0x05 
#define USAT_ADDRESS_TAG           0x06 
#define USAT_TPDU_TAG              0x0B
#define USAT_TEXT_STRING_TAG       0x0D
#define USAT_FILE_LIST_TAG         0x12 
#define USAT_LOC_INFORMATION_TAG   0x13 
#define USAT_IMEI_TAG              0x14 
#define USAT_EVENT_LIST_TAG        0x19 
#define USAT_LOCATION_STATUS_TAG   0x1B 
#define USAT_AID_TAG               0x2F 
#define USAT_BEARER_TAG            0x32 
#define USAT_BEARER_DESCRIPT_TAG   0x35 
#define USAT_CHANNEL_DATA_TAG      0x36 
#define USAT_CHANNEL_DATA_LEN_TAG  0x37 
#define USAT_CHANNEL_STATUS_TAG    0x38 
#define USAT_BUFFER_SIZE_TAG       0x39 
#define USAT_INT_TRANSPT_LEVEL_TAG 0x3C 
#define USAT_OTHER_ADDRESS_TAG     0x3E 
#define USAT_ACCESS_TECHNOLOGY_TAG 0x3F
#define USAT_NETWK_ACCESS_NAME_TAG 0x47 
#define USAT_IMEISV_TAG            0x62 
#define USAT_PLMN_ID_TAG           0x09
#define USAT_SMS_TPDU_TAG          0x0B
#define USAT_DATA_CONNECT_STAT_TAG 0x1D
#define USAT_ROUT_AREA_INF_TAG     0x73
#define USAT_UPDATA_ATTACH_TYPE    0x74
#define USAT_REJECT_CAUSE_CODE_TAG 0x75
#define USAT_GEO_LOC_PARAM_TAG     0x76
#define USAT_GAD_SHAPES_TAG        0x77
#define USAT_NMEA_SENTENCE_TAG     0x78
#define USAT_PLMN_LIST_TAG         0x79
#define USAT_TRACKING_AREA_ID_TAG  0x7D
#define USAT_EXT_REJ_CAUSE_TAG     0x57

#define USAT_RECEIVE_DATA_MAX_LEN        237
#define USAT_SEND_DATA_MAX_LEN           240

#define USAT_BIP_PROTOCAL_TYPE_UDP       1
#define USAT_BIP_PROTOCAL_TYPE_TCP       2

#define USAT_BEARER_DESC_TYPE_DEFAULT    3
#define USAT_BEARER_DESC_TYPE_EUTRAN_2   2
#define USAT_BEARER_DESC_TYPE_EUTRAN_9   9
#define USAT_BEARER_DESC_TYPE_EUTRAN_11  11

#define USAT_ADDRESS_TYPE_IPV4           0x21
#define USAT_ADDRESS_TYPE_IPV6           0x57

#define USAT_ENV_CMD_TYPE_MENU_SELECTION 0xD3
#define USAT_ENV_CMD_TYPE_EVENT_DOWNLOAD 0xD6
#define USAT_ENV_CMD_GEO_LOC_REPORT      0xDD

#define USAT_CHANNEL_STAT_LINK_DROPED    5

#define USAT_MAXENT_LIST_LEN            0x22
#define USAT_LOCATION_STATUS            0x03
#define USAT_DATA_AVAILABLE             0x09
#define USAT_CHANNEL_STATUS             0x0A
#define USAT_NETWORK_REJECTION          0x12

#define USAT_PROAC_CMD_TYPE_REFRESH            0x01
#define USAT_PROAC_CMD_TYPE_MORE_TIME          0x02
#define USAT_PROAC_CMD_TYPE_POLL_INTERVAL      0x03
#define USAT_PROAC_CMD_TYPE_POLLING_OFF        0x04
#define USAT_PROAC_CMD_TYPE_SETUP_EVENT_LIST   0x05
#define USAT_PROAC_CMD_TYPE_SEND_SHORT_MESSAGE 0x13
#define USAT_PROAC_CMD_TYPE_GEO_LOC_REQ        0x16
#define USAT_PROAC_CMD_TYPE_PROVIDE_LOC_INFORM 0x26
#define USAT_PROAC_CMD_TYPE_OPEN_CHANNEL       0x40
#define USAT_PROAC_CMD_TYPE_CLOSE_CHANNEL      0x41
#define USAT_PROAC_CMD_TYPE_RECEIVE_DATA       0x42
#define USAT_PROAC_CMD_TYPE_SEND_DATA          0x43
#define USAT_PROAC_CMD_TYPE_GET_CHANNEL_STATUS 0x44
#define USAT_PROAC_CMD_TYPE_DISPLAY_TEXT       0x21
#define USAT_PROAC_CMD_TYPE_GET_INKEY          0x22
#define USAT_PROAC_CMD_TYPE_GET_INPUT          0x23
#define USAT_PROAC_CMD_TYPE_SELECT_ITEM        0x24
#define USAT_PROAC_CMD_TYPE_SETUP_MENU         0x25


#define USAT_PROAC_CMD_TAG         0xD0
#define USAT_COMMAND_DETAILS_TAG   0x01
#define USAT_DEVICE_IDENTITY_TAG   0x02
#define USAT_RESULT_TAG            0x03
#define USAT_DURATION_TAG          0x04
#define USAT_ALPHA_IDENTIFIER_TAG  0x05
#define USAT_ADDRESS_TAG           0x06
#define USAT_TPDU_TAG              0x0B
#define USAT_TEXT_STRING_TAG       0x0D
#define USAT_FILE_LIST_TAG         0x12
#define USAT_LOC_INFORMATION_TAG   0x13
#define USAT_IMEI_TAG              0x14
#define USAT_MEASURE_RPT_TAG       0x16
#define USAT_EVENT_LIST_TAG        0x19
#define USAT_LOCATION_STATUS_TAG   0x1B
#define USAT_AID_TAG               0x2F
#define USAT_BEARER_TAG            0x32
#define USAT_BEARER_DESCRIPT_TAG   0x35
#define USAT_CHANNEL_DATA_TAG      0x36
#define USAT_CHANNEL_DATA_LEN_TAG  0x37
#define USAT_CHANNEL_STATUS_TAG    0x38
#define USAT_BUFFER_SIZE_TAG       0x39
#define USAT_INT_TRANSPT_LEVEL_TAG 0x3C
#define USAT_OTHER_ADDRESS_TAG     0x3E
#define USAT_ACCESS_TECHNOLOGY_TAG 0x3F
#define USAT_NETWK_ACCESS_NAME_TAG 0x47
#define USAT_IMEISV_TAG            0x62
#define USAT_MEASURE_QUAL_TAG	   0x69
#define USAT_PLMN_ID_TAG           0x09
#define USAT_SMS_TPDU_TAG          0x0B
#define USAT_DATA_CONNECT_STAT_TAG 0x1D
#define USAT_ROUT_AREA_INF_TAG     0x73
#define USAT_UPDATA_ATTACH_TYPE    0x74
#define USAT_REJECT_CAUSE_CODE_TAG 0x75
#define USAT_PLMN_LIST_TAG         0x79
#define USAT_TRACKING_AREA_ID_TAG  0x7D
#define USAT_EXT_REJ_CAUSE_TAG     0x57

//USAT result, Terminal Response 填写result字段(不是additional result)
#define USAT_RST_PERFM_SUCCESS                     0x00 //Command performed successfully
#define USAT_RST_PERFM_PARTIAL_COMPREHENSION       0x01 //Command performed with partial comprehension
#define USAT_RST_PERFM_MISS_INFORM                 0x02 //Command performed, with missing information
#define USAT_RST_REFRESH_PERFM_WITH_ADD_EFS_READ   0x03 //REFRESH performed with additional EFs read
#define USAT_RST_CMD_PERFM_SUCCESS_LIMIT_SERV      0x06 //Command performed successfully, limited service
#define USAT_RST_CMD_PERFM_WITH_MODIFY             0x07 //Command performed with modification
#define USAT_RST_REFRESH_PERFM_IND_NAA_NOT_ACTIVE  0x08 //REFRESH performed but indicated NAA was not active
#define USAT_RST_PROAC_USIM_SESSION_TERM_BY_USER   0x10 //Proactive UICC session terminated by the user
#define USAT_RST_NO_RESPONSE_FROM_USER             0x12 //No response from user
#define USAT_RST_TERM_CURT_UNABLE_PROCESS          0x20 //Terminal currently unable to process command
#define USAT_RST_NETWORK_CURT_UNABLE_PROCESS       0x21 //Network currently unable to process command
#define USAT_RST_USER_DID_NOT_ACCEPT_THE_CMD       0x22 //User did not accept the proactive command
#define USAT_RST_BEYOND_TERM_CAPABIL               0x30 //Command beyond terminal's capabilities
#define USAT_RST_TYPE_NOT_UNDERSTOOD_BY_TERM       0x31 //Command type not understood by terminal
#define USAT_RST_DATA_NOT_UNDERSTOOD_BY_TERM       0x32 //Command data not understood by terminal;
#define USAT_RST_SMS_RP_ERROR                      0x35 //SMS RP-ERROR
#define USAT_RST_ERR_REQUIRED_VALUE_MISS           0x36 //Error, required values are missing;
#define USAT_RST_BIP_ERROR                         0x3A //Bearer Independent Protocol error

//Additional result(补充原因), terminal response中上述部分result需要填写
#define BIP_ADD_RST_NULL               0x00   //No specific cause can be given; 
#define BIP_ADD_RST_NO_CHAN_AVAIL      0x01   //No channel available; 
#define BIP_ADD_RST_CHAN_CLOSED        0x02   //Channel closed; 
#define BIP_ADD_RST_INVALID_CHAN_ID    0x03   //Channel identifier not valid; 
#define BIP_ADD_RST_INVALID_BUF_SIZE   0x04   //Requested buffer size not available; 
#define BIP_ADD_RST_SEC_ERR            0x05   //Security error (unsuccessful authentication); 
#define BIP_ADD_RST_TRANS_LEVEL_ERR    0x06   //Requested UICC/terminal interface transport level not available; 
#define BIP_ADD_RST_UNREACHABLE        0x07   //Remote device is not reachable (not present, not physically connected, switched off, etc.); 
#define BIP_ADD_RST_SRVICE_ERR         0x08   //Service error (service not available on remote device); 
#define BIP_ADD_RST_SRV_ID_UNKNOWN     0x09   //Service identifier unknown


#define USAT_DEVICE_ID_CHANN_NULL      0x20
#define USAT_DEVICE_ID_CHANN_1         0x21
#define USAT_DEVICE_ID_CHANN_2         0x22
#define USAT_DEVICE_ID_CHANN_3         0x23
#define USAT_DEVICE_ID_CHANN_4         0x24
#define USAT_DEVICE_ID_CHANN_5         0x25
#define USAT_DEVICE_ID_CHANN_6         0x26
#define USAT_DEVICE_ID_CHANN_7         0x27
#define USAT_DEVICE_ID_UICC            0x81
#define USAT_DEVICE_ID_TERMINAL        0x82
#define USAT_DEVICE_ID_NETWORK         0x83


//REFRESG TYPE Command Qualifier
#define USAT_NAA_INIT_FULL_FILE_CHANGE 0x00  //NAA Initialization and Full File Change Notification;
#define USAT_FILE_CHANGE               0x01  //File Change Notification;
#define USAT_NAA_INIT_FILE_CHANGE      0x02  //NAA Initialization and File Change Notification;
#define USAT_NAA_INIT                  0x03  //NAA Initialization;
#define USAT_UICC_RESET                0x04  //UICC Reset;
#define USAT_NAA_APP_RESET             0x05  //NAA Application Reset, only applicable for a 3G platform;
#define USAT_NAA_SESSION_RESET         0x06  //NAA Session Reset, only applicable for a 3G platform;

//PROVIDE LOCAL INFORMATION TYPE Command Qualifier
#define USAT_LOCAL_INFO_LOC_INF   0x00
#define USAT_LOCAL_INFO_IMEI      0x01
#define USAT_LOCAL_INFO_NMR       0x02
#define USAT_LOCAL_INFO_ACT       0x06
#define USAT_LOCAL_INFO_IMEISV    0x08

#define USAT_MEASURE_QUAL_INTRA_FRQC    0x05
#define USAT_MEASURE_QUAL_INTER_FRQC    0x06

//SEND DATA Command Qualifier
#define USAT_SEND_DATA_STORE            0
#define USAT_SEND_DATA_IMMEDIATELY      1

//SEND SHORT MESSAGE Command Qualifier
#define USAT_SEND_SHORT_MESSAGE_UNPACK    0
#define USAT_SEND_SHORT_MESSAGE_PACK      1

//SMS-PP data download
#define USIM_SMSPP_TOOLKIT_BUSY        0xD4
#define USIM_SMSPP_USIM_DL_ERR         0xD5

/*ucUpdateAttachType*/
#define USAT_UPDATE_TYPE_EPS_ATTACH    0x09  //"EPS Attach" in the case of an EMM ATTACH REQUEST message
#define USAT_UPDATE_TYPE_COMB_ATTACH   0x0A  //"Combined EPS/IMSI Attach" in the case of an EMM ATTACH REQUEST message
#define USAT_UPDATE_TYPE_TAU           0x0B  //"TA updating " in the case of an EMM TRACKING AREA UPDATE REQUEST message
#define USAT_UPDATE_TYPE_COMB_TAU      0x0C  //"Combined TA/LA updating" in the case of an EMM TRACKING AREA UPDATE REQUEST message
#define USAT_UPDATE_TYPE_COMB_TAU_IMSI 0x0D  //"Combined TA/LA updating with IMSI attach" in the case of an EMM TRACKING AREA UPDATE REQUEST message
#define USAT_UPDATE_TYPE_PERIOD_TAU    0x0E  //"Periodic updating" in the case of an EMM TRACKING AREA UPDATE REQUEST message

#define NAS_MAX_SCA_LEN             10
#define USIM_MAX_APDU_LEN           256

typedef struct {
    uint8_t                        bLen;                              /*length of following data, include NPI and TON*/
    uint8_t                        b4Npi                         : 4; /*Numbering plan identification*/
    uint8_t                        b3Ton                         : 3; /*type of number*/
    uint8_t                        b1Ext                         : 1; /*const value 1*/
    uint8_t                        abSca[NAS_MAX_SCA_LEN];
} S_Usim_SCA;

typedef struct
{
    uint8_t bLen;
    uint8_t abAddRst[USAT_MAX_ADD_RESULT_LEN];
}S_Usat_TR_AddRst;

typedef struct
{
    uint8_t                   bDeviceIdTag;
    uint8_t                   bLen;
    uint8_t                   bSrcId;
    uint8_t                   bDstId;
}S_Usat_Proc_Cmd_Dev_Id;

typedef struct
{
    uint8_t                   bFileListTag;
    uint8_t                   bLen;
    uint8_t                   bFileNum;
    uint8_t                   abFile[USAT_MAX_FILE_LIST_LEN];
}S_Usat_File_List;

typedef struct
{
    uint8_t                   bAidTag;
    uint8_t                   bLen;
    uint8_t                   abAid[16];
}S_Usat_Aid;

typedef struct
{
    uint8_t                   bDurationTag;
    uint8_t                   bLen;
    uint8_t                   bTimeUnit;
    uint8_t                   bTimeInterval;
}S_Usat_Duration;

typedef struct
{
    uint16_t                  bLocStat     :1;
    uint16_t                  bNwRej       :1;
    uint16_t                  bDataAvail   :1;
    uint16_t                  bChanStat    :1;
    uint16_t                  bPad         :12;
}S_Usat_SetupEvtList;

typedef struct
{
    uint8_t                   bTpduTag;
    uint8_t                   bLen;
    uint8_t                   abTpdu[USAT_MAX_TPDU_LEN];
}S_Usat_Sms_Tpdu;

typedef struct
{
    S_Usim_SCA          tAddress;
    S_Usat_Sms_Tpdu     tTpdu;
}S_Usat_Send_Sms;

typedef struct
{
    S_Usat_File_List    tFileList;
    S_Usat_Aid          tAid;
}S_Usat_Refresh;

typedef struct
{
    uint8_t             bMeasureQualTag;
    uint8_t             bLen;
    uint8_t             bMeasureQual;
}S_Usat_ProvideLocInfo;

typedef struct
{
    uint8_t             bGeoLocTag;
    uint8_t             bLen;
    uint8_t             bHorztAcc;       //Horizontal accuracy
    uint8_t             bVertCoord;      //Vertical coordinate
    uint8_t             bVelocity;       //Velocity
    uint8_t             bPrefGadShap;    //Preferred GAD shapes
    uint8_t             bPrefNmeaSent;   //Preferred NMEA sentences
    uint8_t             bPrefMaxRspTime; //Preferred Maximum Response Time
}S_Usat_GeoLocReq;

#define USAT_MAX_GEO_SHAPE_CODE_LEN  91
#define USAT_MAX_VELOCITY_LEN        7
#define USAT_MAX_NMEA_SENT_LEN       128

typedef struct
{
    uint8_t             bLen;
    uint8_t             abNmeaSent[USAT_MAX_NMEA_SENT_LEN];
}S_Usat_NmeaSentence;


//目前支持的SHAPE类型
#define USAT_SHAPE_TYPE_EP            0    //Ellipsoid Point
#define USAT_SHAPE_TYPE_EP_ALT        8    //Ellipsoid point with altitude

//目前支持的velocity类型
#define USAT_VELOCITY_TYPE_HORIZONTAL 0    //Horizontal Velocity

typedef struct
{
    uint8_t             bS;                //0: north； 1： south
    uint32_t            dwDegOfLati;       //Degrees of latitude, 0 - 0x7FFFFF
    uint32_t            dwDegOfLong;       //Degrees of longitude,0 - 0xFFFFFF
}S_Usat_Ep;

typedef struct
{
    S_Usat_Ep           tEp;
    uint16_t            bD          :1;    //0: Altitude expresses height;
                                           //1: Altitude expresses depth
    uint16_t            wAltitude   :15;   //Altitude
}S_Usat_Ep_Alt;

typedef struct
{
    uint16_t            wBearing;  //Bearing,          0 - 0x01FF
    uint16_t            wHorSpeed; //Horizontal Speed, 0 - 0xFFFF
}S_Usat_HorVeloc;

typedef struct
{
    uint8_t             bShapeType;//uShape类型，目前只支持 USAT_SHAPE_TYPE_EP 和 USAT_SHAPE_TYPE_EP_ALTITUDE
    union {
        S_Usat_Ep       tEp;       //Ellipsoid Point
        S_Usat_Ep_Alt   tEpAlt;    //Ellipsoid point with altitude
        //RFU
    }u;
}S_Usat_Shape;

typedef struct
{
    uint8_t             bVelType;  //uVelocity类型，目前只支持 USAT_VELOCITY_TYPE_HORIZONTAL
    union {
        S_Usat_HorVeloc tHorVel;   //Horizontal Speed
        //RFU
    }u;
}S_Usat_Velocity;

typedef struct
{

    uint8_t             bVelFlag;  //velocity是否存在的标志位，0：不存在，1：存在
    S_Usat_Shape        tShape;
    S_Usat_Velocity     tVelocity;
}S_Usat_GadShapes;

typedef struct
{
    uint8_t             bGadShapeFlg;    //GAD shape是否存在的标志位，0:没有GadShapes; 1:有GadShapes
    uint8_t             bNmeaSentFlg;    //NMEA sentence是否存在的标志位，0:没有tNmeaSent; 1:有tNmeaSent
    S_Usat_GadShapes    tGadShapes;
    S_Usat_NmeaSentence tNmeaSent;
}S_Usat_Env_GeoLocRpt;


typedef struct
{
    uint8_t bImdtLinkEst      : 1; /* 0 = on demand link establishment;
                                      1 = immediate link establishment. */
    uint8_t bAutoReconnct     : 1; /* 0 = no automatic reconnection; 
                                      1 = automatic reconnection. */
    uint8_t bImdtLinkInBgMode : 1; /* 0 = no background mode;  
                                      1 = immediate link establishment in background mode (bit 1 is ignored). */
    uint8_t bDnsSrvRequset    : 1; /* 0 = no DNS server address(es) requested;
                                      1 = DNS server address(es) requested (for packet data service only).*/
    uint8_t bRfu              : 4;
}S_Usat_Qual_OpenChan;

typedef struct
{
    uint8_t bSendImdt          : 1; /* USAT_SEND_DATA_STORE :只把数据加到数据缓存最后; 
                                       USAT_SEND_DATA_IMMEDIATELY = 把数据加到数据缓存最后，并且立即发送 */
    uint8_t bRfu               : 7;
}S_Usat_Qual_SendData;

typedef struct
{
    uint8_t                   bCmdDetailTag;
    uint8_t                   bLen;
    uint8_t                   bCmdNum;
    uint8_t                   bCmdType;
    union {
        S_Usat_Qual_OpenChan  tOpenChan;
        S_Usat_Qual_SendData  tSendData;
        uint8_t               bCmdQual;
    } uQual;
}S_Usat_Cmd_Details;

typedef struct
{
    uint8_t                   bDeviceIdTag;//USAT_DEVICE_IDENTITY_TAG
    uint8_t                   bLen;
    uint8_t                   bSrcId;
    uint8_t                   bDstId;
}S_Usat_DeviceId;

typedef struct
{
    uint8_t                   bRstTag;//USAT_RESULT_TAG
    uint8_t                   bRstCode;  //USAT_RST_PERFM_SUCCESS,USAT_RST_BIP_ERROR...
    uint8_t                   bAddRstLen;
    uint8_t                   abAddRst[USAT_MAX_ADD_RESULT_LEN];
}S_Usat_Result;

typedef struct {
    uint8_t                   bQci;
    uint32_t                  ulDl_Gbr;
    uint32_t                  ulUl_Gbr;
    uint32_t                  ulDl_Mbr;
    uint32_t                  ulUl_Mbr;
}T_Bip_CgeqosParam;

typedef struct {
    uint8_t                   bLen;
    uint8_t                   bQci;
    uint8_t                   bMBRUl;
    uint8_t                   bMBRDl;
    uint8_t                   bGBRUl;
    uint8_t                   bGBRDl;
    uint8_t                   bMBRUlExt;
    uint8_t                   bMBRDlExt;
    uint8_t                   bGBRUlExt;
    uint8_t                   bGBRDlExt;
    uint8_t                   bMBRUlExt2;
    uint8_t                   bMBRDlExt2;
    uint8_t                   bGBRUlExt2;
    uint8_t                   bGBRDlExt2;
} S_Usat_EpsQos;

typedef struct {
    uint8_t                   bType;     //USAT_BEARER_DESC_TYPE_DEFAULT:使用默认承载；USAT_BEARER_DESC_TYPE_EUTRAN_11：需要根据下面参数新建一路承载；其他值不支持
    T_Bip_CgeqosParam         tCgeqos;   //CGEQOS命令使用的参数
} S_Usat_DecBdParam;

typedef struct {
    uint8_t                   bTag;      //USAT_BEARER_DESCRIPT_TAG
    uint8_t                   bLen;
    uint8_t                   abBdParam[USAT_MAX_BEARER_PARAM_LEN + 1];
} S_Usat_OrgBdParam;

typedef struct
{
    S_Usat_DecBdParam         tDecParam;//解码后的承载信息参数，供AP侧使用
    S_Usat_OrgBdParam         tOrgParam;//原始的承载信息参数，供PS内部使用
}S_Usat_BearerDesc;

typedef struct
{
    uint8_t                   bTag;//USAT_BUFFER_SIZE_TAG
    uint8_t                   bLen;
    uint16_t                  wSize;
}S_Usat_BuffSize;

typedef struct
{
    uint8_t                   bTag;//USAT_NETWK_ACCESS_NAME_TAG
    uint8_t                   bLen;
    char                      acApn[USAT_MAX_APN_LEN + 1];
}S_Usat_Apn;

typedef struct
{
    uint8_t                   bTag;//USAT_OTHER_ADDRESS_TAG
    uint8_t                   bLen;
    uint8_t                   bType;//USAT_ADDRESS_TYPE_IPV4;USAT_ADDRESS_TYPE_IPV6;其他类型保留
    uint8_t                   abAddr[USAT_MAX_OTHER_ADDR_LEN];
}S_Usat_OtherAddr;

typedef struct
{
    uint8_t                   bTag;//USAT_TEXT_STRING_TAG
    uint8_t                   bLen;
    uint8_t                   bCodeScheme;/* 00:7 bits  04:8 bits  08:UCS2 */
    char                      abTextStr[USAT_MAX_TEXT_STR_LEN + 1];
}S_Usat_TextString;

typedef struct
{
    uint8_t                   bTag;//USAT_INT_TRANSPT_LEVEL_TAG
    uint8_t                   bLen;
    uint8_t                   bProtocolType;//1: USAT_BIP_PROTOCAL_TYPE_UDP; 2: USAT_BIP_PROTOCAL_TYPE_TCP; 其他值不支持
    uint16_t                  wPortNum;     //端口号
}S_Usat_TransLevel;

typedef struct
{
    S_Usat_BearerDesc         tBearDesc;   //CGEQOS使用的QCI等信息
    S_Usat_BuffSize           tBuffSize;   //socket收发数据的缓存大小
    S_Usat_Apn                tApn;        //CGDCONT使用的APN
    S_Usat_TextString         tUserLog;    //CGAUTH使用的用户名(AUTH类型卡不提供，可以固定填PAP)
    S_Usat_TextString         tUserPwd;    //CGAUTH使用的密码
    S_Usat_TransLevel         tTransLevel; //TCP/UDP类型，端口号等信息
    S_Usat_OtherAddr          tDestAddr;   //BIP server的地址
}S_Usat_PC_OpenChannel;

typedef struct
{
    uint8_t                   bTag;//USAT_CHANNEL_DATA_LEN_TAG
    uint8_t                   bLen;
    uint8_t                   bDataLen;
}S_Usat_ChannelDataLen;

typedef struct
{
    uint8_t                   bTag;//USAT_CHANNEL_DATA_TAG
    uint8_t                   bLen;
    uint8_t                   abData[USAT_RECEIVE_DATA_MAX_LEN];
}S_Usat_ChannelData;

typedef struct
{
    uint8_t                   bTag;//USAT_CHANNEL_DATA_LEN_TAG
    uint8_t                   bLen;
    uint8_t                   bDataSize;
}S_Usat_ChanDataSize;

typedef struct
{
    S_Usat_ChanDataSize       tChDataLen;
}S_Usat_PC_ReceiveData;

typedef struct
{
    S_Usat_ChannelData        tChData;
}S_Usat_PC_SendData;

typedef struct
{
    uint8_t                   bTag;//USAT_CHANNEL_STATUS_TAG
    uint8_t                   bLen;
    uint8_t                   bChanId   :3;//0:没有合法通道；1-7:通道号
    uint8_t                   bRfu      :4;
    uint8_t                   bLinkEsted:1;//0:通道连接不可用；1:通道连接可用
    uint8_t                   bFurtherInfo;//0:没有更多信息；USAT_CHANNEL_STAT_LINK_DROPED(5):连接断开(网络失败或用户取消)；其他值无效
}S_Usat_ChannelStatus;

typedef struct
{
    S_Usat_ChannelStatus      tChanStat;
    S_Usat_BearerDesc         tBearerDesc;
    S_Usat_BuffSize           tBuffSize;
}S_Usat_TR_OpenChannel;

typedef struct
{
    S_Usat_ChannelData        tChData;
    uint8_t                   tRemainDataLen;//socket数据缓存里还有多少数据，如果大于255，填255
}S_Usat_TR_ReceiveData;

typedef struct
{
    uint8_t                   bBuffLeftLen;
}S_Usat_TR_SendData;

typedef struct
{
    S_Usat_ChannelStatus      tChanStat;
}S_Usat_TR_GetChanStatus;

typedef struct
{
    S_Usat_ChannelStatus      tChanStat;
    uint8_t                   tChDataLen;
}S_Usat_Env_DataAvailable;

typedef struct
{
    S_Usat_ChannelStatus      tChanStat;
}S_Usat_Env_ChannelStatus;


typedef struct
{
    uint8_t                        bEnvCmdType;// USAT_EVT_DATA_AVAILABLE, USAT_EVT_CHANNEL_STATUS
                                               //选主菜单填 USAT_ENV_CMD_TYPE_MENU_SELECTION
    union {
        S_Usat_Env_DataAvailable   tChanAvail;
        S_Usat_Env_ChannelStatus   tChanStat;
        //todo reserve for menu selection
    }u;
}S_Usat_EnvCmd;

typedef struct
{
    S_Usat_Cmd_Details          tCmdDetail;
    S_Usat_Proc_Cmd_Dev_Id      tDeviceId;
    union {
        S_Usat_Refresh          tRefresh;
        S_Usat_Duration         tDuration;
        S_Usat_SetupEvtList     tEventList;
        S_Usat_Send_Sms         tSendSms;
        S_Usat_ProvideLocInfo   tPrvdLocInf;
        S_Usat_GeoLocReq        tGeoLocReq;
#ifdef BIP_SUPPORT
        S_Usat_PC_OpenChannel   tOpenChan;
        S_Usat_PC_ReceiveData   tRcvData;
        S_Usat_PC_SendData      tSendData;
#endif
    } u;
}S_Usat_Proc_Cmd;

typedef struct
{
    uint8_t              bDataLen;
    uint8_t              abData[USIM_MAX_APDU_LEN];
} S_Usat_Envelope_Req;


typedef struct
{
    uint16_t              wStatusByte;
    uint16_t              wDataLen;
    uint8_t               abData[USIM_MAX_APDU_LEN];
} S_Usat_Envelope_Rsp;

typedef struct
{
    S_PS_PlmnId tPlmnId;
    uint16_t    wTac;
    uint32_t    dwCellId;
}S_Usat_Location_Info;


typedef struct
{
    uint8_t bNmrLen;
    uint8_t abNmr[USAT_MAX_MNR_RLT_LEN];
}S_Usat_NmrInfo;


typedef struct
{
    union {
        S_Usat_Location_Info   tLocInfo;
        uint8_t                bAct;
        uint8_t                abImei[USAT_IMEI_LEN];
        uint8_t                abImeiSv[USAT_IMEISV_LEN];
        S_Usat_NmrInfo         tNmr;
    } u;
}S_Usat_TR_ProvLocInf;

typedef struct
{
    S_PS_PlmnId    tPlmnId;
    uint16_t       wTac;
    uint8_t        bUpdateAttachType;
    uint8_t        bRejectCause;
}S_Usat_Env_Network_Reject;

typedef struct
{
#define USAT_ENV_LOC_STAT_NORMAL_SERVICE  0x00
#define USAT_ENV_LOC_STAT_LIMITED_SERVICE 0x01
#define USAT_ENV_LOC_STAT_NO_SERVICE      0x02
    uint8_t                 bLocStat;
    S_Usat_Location_Info    tLocInfo;
}S_Usat_Env_Loc_Status;


/*******************************************************************************
  Function:    Usat_DecProacCmd
  Describe:    Decode Proactive Command
  Input:       uint8_t *pbSrcCode              : 解码前的原始码流
               uint16_t wSrcCodeLen            : 解码前的原始码流字节数
  Output:      S_Usat_Proc_Cmd *ptProacCmd     : 解码后的主动式命令结构
  Return:      0:success  1:fail
  Note:
*******************************************************************************/
extern uint8_t Usat_DecProacCmd(S_Usat_Proc_Cmd *ptProacCmd, uint8_t *pbSrcCode, uint16_t  wSrcCodeLen);

/*******************************************************************************
  Function:    Usat_EncTerminalResponse
  Describe:    Terminal Response 编码函数
  Input:       bRstCode       : USAT_RST_PERFM_SUCCESS, USAT_RST_BIP_ERROR, USAT_RST_TERM_CURT_UNABLE_PROCESS...
               ptAddRst       : bRstCode为成功时填NULL，USAT_RST_BIP_ERROR等异常场景时需要填写
               *pAddInfo      ：根据具体命令填写, 如OpenChannel 填写结构 S_Usat_TR_OpenChannel
  Output:      uint8_t *pCode : 编码后的码流
  Return:      编码后的码流长度
  Note:
*******************************************************************************/
extern uint8_t Usat_EncTerminalResponse(uint8_t *pCode, uint8_t bRstCode, S_Usat_TR_AddRst *ptAddRst, void *pAddInfo);

/*******************************************************************************
  Function:    Usat_EnvelopeCommand
  Describe:    ENVELOPE命令的编码函数
  Input:       bEnvCmdId      : ENVEELOP命令ID，如 USAT_DATA_AVAILABLE, USAT_CHANNEL_STATUS
               EnvCmdInfo     ：ENVELOPE命令内容，根据命令ID填写结构内容，如 S_Usat_Env_DataAvailable, S_Usat_Env_ChannelStatus
  Output:      pCode          : 编码后的码流
               pLen           ：编码后的码流长度
  Return:      0：编码成功； 1：编码失败
  Note:
*******************************************************************************/
extern uint8_t Usat_EncEnvelopeCommand(uint8_t *pbCode, uint8_t *pLen, uint8_t bEnvCmdId, void *EnvCmdInfo);
#endif
