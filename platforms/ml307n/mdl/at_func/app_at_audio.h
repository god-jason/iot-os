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
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */

#ifndef __APP_AT_AUDIO_H__
#define __APP_AT_AUDIO_H__

#include "os.h"
#include "slog_print.h"
#include "at_parser.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/



/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void APP_AT_X_TTSetup_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);

void APP_AT_X_TTSetup_Test(uint8_t channelId);

void APP_AT_X_TTS_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);

void APP_AT_X_TTS_Read(uint8_t channelId);

void APP_AT_X_TTS_Test(uint8_t channelId);

void APP_AT_X_AUDSW_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);

void APP_AT_X_AUDSW_Read(uint8_t channelId);

void APP_AT_X_AUDSW_Test(uint8_t channelId);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __APP_AT_AUDIO_H__ */
