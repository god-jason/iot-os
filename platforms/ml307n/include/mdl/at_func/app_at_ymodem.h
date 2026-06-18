/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */

#ifndef __APP_AT_YMODEM_H__
#define __APP_AT_YMODEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


void at_ymodem_send(uint8_t ch_id, const char *fileName, uint8_t *buf, uint32_t len, void (*callback)(void *), void *param);



#ifdef __cplusplus
}
#endif
#endif

