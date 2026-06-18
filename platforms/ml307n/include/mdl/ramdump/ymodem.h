/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        ymodem.h
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */
#ifndef __YMODEM_H_
#define __YMODEM_H_

/************************************************************************************
*                                 头文件
************************************************************************************/
#include <os.h>
#include <stdbool.h>

/* Comment this out if you don't plan to use extra CRC32 - removes ~1K */
//#define WITH_CRC32

#define PACKET_SEQNO_INDEX         (1)
#define PACKET_SEQNO_COMP_INDEX    (2)

#define PACKET_HEADER              (3) /* start, block, block-complement */
#define PACKET_TRAILER             (2) /* CRC bytes */
#define PACKET_OVERHEAD            (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE                (128)
#define PACKET_1K_SIZE             (1024)
#define PACKET_TIMEOUT             (3000)

#define FILE_NAME_LENGTH           (64)
#define FILE_SIZE_LENGTH           (16)

/* ASCII control codes: */
#define SOH                        (0x01) /* start of 128-uint8_t data packet */
#define STX                        (0x02) /* start of 1024-uint8_t data packet */
#define EOT                        (0x04) /* end of transmission */
#define ACK                        (0x06) /* receive OK */
#define NAK                        (0x15) /* receiver error; retry */
#define CAN                        (0x18) /* two of these in succession aborts transfer */
#define CRC                        (0x43) /* use in place of first NAK for CRC mode */

/* Number of consecutive receive errors before giving up: */
#define MAX_ERRORS                 (5)

typedef struct
{
#ifdef YMODEM_RECVFILE
   char       name[FILE_NAME_LENGTH];
#else
   const char *name;
#endif
   uint32_t   size;
   uint8_t    *buf;
} T_YModemFile;

extern int32_t ymodem_receive(T_YModemFile files[], uint32_t count);
extern int32_t ymodem_send(T_YModemFile files[], uint32_t count, bool crc_en);

typedef int32_t (*F_ConGetc)(uint32_t timeout);
typedef void (*F_ConPutc)(uint8_t ch);
typedef void (*F_ConPutBytes)(uint8_t *bytes, uint32_t bytesLen, uint32_t timeout);

#endif /* __YMODEM_H_ */
