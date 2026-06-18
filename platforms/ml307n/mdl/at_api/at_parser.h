/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief 实现AT解析API
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */


#ifndef __AT_CMD_PARSER_H__
#define __AT_CMD_PARSER_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AtParser
 */

/**@{*/


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
/*
 * Defining macro AT_PARSER_IGNOR_SPACE  means that the function of ignoring the spaces before and after the AT command parameters is enabled.
 * However, when resolving as RAW type, spaces are NEVER ignored.
 */

//#define AT_PARSER_IGNOR_SPACE

//typedef int bool_t;

typedef enum {
    AT_ERRNO_NO_ERROR = 0, /* No error in the AT parsing */
    AT_ERRNO_ABORT_PARAM_ERROR, /* Parsing aborted for error parameter of AT command */
    AT_ERRNO_ABORT_FORMAT_ERROR, /* Parsing aborted for error parameter of format */
    AT_ERRNO_ABORT_OUT_OF_MEM, /* Parsing aborted for out of memory generally caused by Insufficient memory for output parameters */
    AT_ERRNO_FAIL_ILLEGAL_INPUT, /* Execution failed for illegal input data, generally caused by function parameter input error */
    AT_ERRNO_FAIL_EXCEPTION, /* Execution failed for inner exception of parser that should not occur in normal */
    AT_ERRNO_PARAM_NULL,
} at_errno_t;

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
/**
 * @brief used to parse AT parameter
 * @attention KEYWORDs in the AT command MUST be ESCAPED if they need to be used in parameters, that is, the escape character('\\')
 *            MUST be added before the original. Keywords list of AT command: '\r', '\n', ',', '"'.
 */

/**
 * Used to judge whether AT starts with a prefix
 *
 * @param[in]  at : AT command
 * @param[in]  size : size of AT command
 * @param[in]  prefix : string type prefix
 * @return uint32_t 0:FALSE 1:TRUE
 */
uint32_t at_parser_is_at_start_with(const char *at, uint32_t size, const char *prefix);

/**
 * Gets the pointer of the substring appears in the AT command
 *
 * @param[in]  at : AT command
 * @param[in]  size : size of AT command
 * @param[in]  substr : sub string to get
 * @return const char* : the pointer of the substring in AT, NULL means substr is not a substring of AT
 * @attention Case insensitive during matching, an empty string is considered a substring of any string
 */
const char* at_parser_get_at_substr(const char *at, uint32_t size, const char *substr);

/**
 * Used to judge whether this AT is Final Result, Only those containing the
 * following strings(Case insensitive) are considered as final result codes:
 *   "\r\nOK\r\n",
 *   "\r\nERROR\r\n",
 *   "\r\n+CME ERROR:",
 *   "\r\n+CMS ERROR:",
 *   "\r\n+CIS ERROR:",
 *   "\r\nCONNECT",
 *   "\r\nNO CARRIER\r\n",
 *   "\r\nNO DIALTONE\r\n",
 *   "\r\nBUSY\r\n",
 *   "\r\nNO ANSWER\r\n",
 *   "\r\nRING\r\n",
 *
 * @param[in]  at : AT command
 * @param[in]  size : size of AT command
 * @return uint32_t  0:FALSE 1:TRUE
 */
uint32_t at_parser_is_at_result(const char *at, uint32_t size);

/**
 * Get the offset address of the AT command parameter
 *
 * @param[in]  at   : AT command
 * @param[in]  size : size of AT command
 * @param[in]  param_order : prameter order which start from 1
 * @param[out]  offset     : parameter offset from AT command
 * @return at_errno_t :
 */
at_errno_t at_parser_get_at_param_offset(const char *at, uint32_t size, uint16_t param_order, uint32_t *offset);

/**
 * Get the AT command parameter size
 *
 * @param[in]  at      : AT command
 * @param[in]  at_size : size of AT command
 * @param[in]  param_order : prameter order which start from 1
 * @param[out] param_size : parameter string size
 * @return at_errno_t
 */
at_errno_t at_parser_get_at_param_size(const char *at, uint32_t at_size, uint16_t param_order, uint32_t *param_size);

/**
 * Convert hex string to bytes
 *
 * @param[in]  str_hex  : hex string
 * @param[in]  hex_size  : size str_hex
 * @param[out] byte_buff : bytes buffer
 * @param[in]  buff_size : size of byte_buff
 * @param[out]  data_size : got data size
 * @return at_errno
 */
at_errno_t at_parser_hex_to_bytes(const char *str_hex, uint32_t hex_size, uint8_t *byte_buff, uint32_t buff_size, uint32_t *data_size);

/**
 * Extracting bytes stream from AT parameter
 *
 * @param[in]  at       : AT command
 * @param[in]  at_size  : size of AT command
 * @param[in]  param_order : prameter order which start from 1
 * @param[in]  at_errno : AT get result
 * @param[out] byte_stream : bytes stream
 * @param[in]  byte_size   : size of byte_stream
 * @param[out]  data_size   : got data size
 * @return at_errno
 * @attention The caller can get the byte_size by at_parser_get_at_param_size, then malloc the memory for byte_stream.
 */
at_errno_t at_parser_get_byte_stream(const char *at, uint32_t at_size, uint16_t param_order, uint8_t *byte_stream, uint32_t byte_size, uint32_t *data_size);

/**
 * Used to parse the AT parameters of AT string
 *
 * @param[in]  param_string : AT command string(no prefix)
 * @param[in]  size : size of param_string
 * @param[in]  format : parameter format, used to specify the parameter type and size,
 *                      support unsigned %u-4 byte, %hu-2 bytes, %hhu-1 bytes, %lu-4 byte, %llu-8 byte, float %f,
 *                      double %lf, string %s or %s.u, hex %x or %x.u(string type), raw %r or %r.u and wildcard $,
 *                      must not be NULL¡£
 *                      The format character 's','r' can be followed by 'e' to indicate that the ESCAPE
 *                      function is used.
 * @param[out] ... : parameter list, pointer type or uint32_t, must be completely consistent with format
 * @return at_errno_t : error number
 * @attention the caller call the interface at_parser_is_at_start_with, then parse the AT parameters by AT_parse
 *            at_parser_is_at_start_with and AT_parse work same as at_parser_at_parse_matched.
 * @warning 1.the .u in %s.u, %x.u, %r.u means that the size of the output parameter which is int32_t type MUST Must immediately
 *          follow the parameter pointer in the param[out] list.
 *          2.Commas can be supported in the parameters of the at command, but the commas must follow the escape character.
 */
at_errno_t at_parser_parse_cmd(const char *param_string, uint32_t size, const char *format, ...);

/**
 * Used to parse the AT parameters of AT string with prefix
 *
 * @param[in]  at_cmd : AT command string(with prefix)
 * @param[in]  size : size of param_string
 * @param[in]  prefix : the prefix expected
 * @param[in]  format : parameter format, used to specify the parameter type and size,
 *                      support unsigned %u-4 byte, %hu-2 bytes, %hhu-1 bytes, %lu-4 byte, %llu-8 byte, float %f,
 *                      double %lf, string %s or %s.u, hex %x or %x.u(string type), raw %r or %r.u and wildcard $,
 *                      must not be NULL.
 *                      the format character 's','r' can be followed by 'e' to indicate that the ESCAPE
 *                      function is used.
 * @param[out] ... : parameter list, pointer type or uint32_t, must be completely consistent with format
 * @return at_errno_t : error number
 * @warning 1.the .u in %s.u, %x.u, %r.u means that the size of the output parameter which is int32_t type MUST Must immediately
 *          follow the parameter pointer in the param[out] list.
 *          2.Commas can be supported in the parameters of the at command, but the commas must follow the escape character.
 */
at_errno_t at_parser_matched(const char *at_cmd, uint32_t at_size, const char *prefix, const char *format, ...);

/**
 * Escape a character
 *
 * @param[in]  str_src : original chars pointer
 * @param[in]  src_size : original chars' size
 * @param[out] dest: escaped char
 * @return uint32_t : length of characters converted
 */
uint32_t at_parser_escape_to_char(const char *str_src, uint32_t src_size, char *dest);

/**
 * Escape a string array
 *
 * @param[in]  str_src : original chars pointer
 * @param[in]  src_size : original chars' size
 * @param[out]  str_dest: dest string
 * @param[in]  dest_size: dest string size
 * @return uint32_t : length of dest string
 */
uint32_t at_parser_escape_array(const char *str_src, uint32_t src_size, char *str_dest, uint32_t dest_size);

/**
 * get count of paramters
 *
 * @param[in]  at       : AT command
 * @param[in]  at_size  : size of AT command
 * @param[out]  count   : count of paramters
 * @return at_errno
 * @attention paramter split by ',' in AT command string
 */
at_errno_t at_parser_param_count(const char *at, uint32_t at_size, uint32_t *count);

 /**
 ************************************************************************************************
 * @brief           单字节的数据转成两个字节的十六进制(例如: 输入字符串 abcd , 转换后变成十六进制 61626364 ).
 *                  转换后的十六进制(一个十六进制占两个字节)的长度是原字符串(一个字符占两个字节)的两倍.
 *
 * @param[in]       bytes           需要转换的数据指针
 * @param[in]       bytesLen        需要转换的数据长度
 * @param[out]      hex             转换后的 HEX 指针. 内存空间由调用者申请, 内存大小是 (bytesLen * 2 +1)
 * @param[in]       hexLen          转换后的 HEX 长度
 *
 * @return          成功返回0; 失败返回小于0.
 ************************************************************************************************
 */
 at_errno_t at_parser_bytes_to_hex(char *bytes, uint32_t bytesLen, char *hex, uint32_t hexLen);

// CM8610 adaptor

at_errno_t xs_at_parse_cm(const char **param_string, uint32_t *size, const char *format, ...);
at_errno_t xs_at_parse_para_count_cm(const char **param_string, uint32_t *size, uint32_t *para_count, const char *format, ...);

#define xs_at_parse_is_at_start_with(at, size, prefix) at_parser_is_at_start_with(at, size, prefix)
#define xs_at_parse_get_at_substr(at, size, substr) at_parser_get_at_substr(at, size, substr)
#define xs_at_parse_is_at_result(at, size) at_parser_is_at_result(at, size)
#define xs_at_parse_get_at_param_offset(at, size, param_order, offset) at_parser_get_at_param_offset(at, size, param_order, offset)
#define xs_at_parse_get_at_param_size(at, at_size, param_order, param_size) at_parser_get_at_param_size(at, at_size, param_order, param_size)
#define xs_at_parse_hex_to_bytes(str_hex, hex_size, byte_buff, buff_size, data_size) at_parser_hex_to_bytes(str_hex, hex_size, byte_buff, buff_size, data_size)
#define xs_at_parse_get_byte_stream(at, at_size, param_order, byte_stream, byte_size, data_size)  at_parser_get_byte_stream(at, at_size, param_order, byte_stream, byte_size, data_size)
#define xs_at_parse(param_string, size, format, ...)  at_parser_parse_cmd(param_string, size, format, ##__VA_ARGS__)
#define xs_at_parse_matched(at_cmd, at_size, prefix, format, ...)  at_parser_matched(at_cmd, at_size, prefix, format, ##__VA_ARGS__)
#define xs_at_parse_escape_to_char(str_src, src_size, dest) at_parser_escape_to_char(str_src, src_size, dest)
#define xs_at_parse_escape_array(str_src, src_size, str_dest, dest_size) at_parser_escape_array(str_src, src_size, str_dest, dest_size)
#define xs_at_parse_param_count(at, at_size, count) at_parser_param_count(at, at_size, count)
#define xs_at_parse_bytes_to_hex(bytes, bytesLen, hex, hexLen) at_parser_bytes_to_hex(bytes, bytesLen, hex, hexLen)
#define xs_get_at_substr(at_string, len, substr) at_parser_get_at_substr(at_string, len, substr)
#define xs_is_at_start_with(at_string, len, prefix) at_parser_is_at_start_with(at_string, len, prefix)

#ifdef __cplusplus
}
#endif
#endif /* __AT_CMD_PARSER_H__ */

/** @} */
