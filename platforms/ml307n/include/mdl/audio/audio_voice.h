/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        audio_voice.h
 *
 * @brief       audio voice 接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */
#ifndef _AUDIO_VOICE_H_
#define _AUDIO_VOICE_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#define     VOICE_PLAY_MODE          1
#define     VOICE_RECORD_MODE        2
/**@name 驱动通用返回值定义
 * @{
 */
#define VOICE_OK                     0    ///< 操作成功
#define VOICE_ERR                   -1    ///< 未知错误
#define VOICE_ERR_BUSY              -2    ///< 设备忙
#define VOICE_ERR_TIMEOUT           -3    ///< 操作超时
#define VOICE_ERR_UNSUPPORTED       -4    ///< 不支持的操作
#define VOICE_ERR_PARAMETER         -5    ///< 参数错误
#define VOICE_ERR_FULL              -6    ///< 满状态
#define VOICE_ERR_EMPTY             -7    ///< 空状态
#define VOICE_ERR_SPECIFIC          -8    ///< 私有错误的起始编号


#define VOICE_NB_SAMPLE_RATE        8000
#define VOICE_WB_SAMPLE_RATE        16000
#define VOICE_DATA_BITS_PER_SAMPLE  16
#define VOICE_NB_FRAME_SIZE         320
#define VOICE_WB_FRAME_SIZE         640


int8_t Voice_Open(uint8_t sampleBits, uint16_t framSize, uint16_t sampleRate);
int8_t Voice_Close(void);
int8_t Voice_ReadStart(void);
int8_t Voice_ReadStop(void);
int8_t Voice_WriteStart(void);
int8_t Voice_WriteStop(void);
int8_t Voice_WriteOneFrame(uint8_t *frameBuf, uint16_t frameSize, uint32_t timeOut);
int8_t Voice_ReadOneFrame(uint8_t *frameBuf, uint16_t frameSize, uint32_t timeOut);
int8_t Voice_StartLocalRingTone(void);
int8_t Voice_StopLocalRingTone(void);


#endif
