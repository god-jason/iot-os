/**
 ***********************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        drv_codec.h
 *
 * @brief       This file provides operation functions declaration for audio.
 *
 * @revision
 * Date         Author          Notes
 * 2020-02-20   OneOS Team      First Version
 ***********************************************************************************************************************
 */

#ifndef __DRV_CODEC_H__
#define __DRV_CODEC_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**@name 驱动通用返回值定义
 * @{
 */
#define CODEC_OK                     0    ///< 操作成功
#define CODEC_ERR                   -1    ///< 未知错误
#define CODEC_ERR_BUSY              -2    ///< 设备忙
#define CODEC_ERR_TIMEOUT           -3    ///< 操作超时
#define CODEC_ERR_UNSUPPORTED       -4    ///< 不支持的操作
#define CODEC_ERR_PARAMETER         -5    ///< 参数错误
#define CODEC_ERR_FULL              -6    ///< 满状态
#define CODEC_ERR_EMPTY             -7    ///< 空状态
#define CODEC_ERR_SPECIFIC          -8    ///< 私有错误的起始编号

#define     CODEC_PLAY          1
#define     CODEC_RECORD        2

#define     CODEC_ES8311        1
#define     CODEC_TYPE          CODEC_ES8311

#define     PA_AW87390          1
#define     CODEC_PA_TYPE       PA_AW87390


#define     HTONL(x) ((((x) & (uint32_t)0x000000ffUL) << 24) |\
                    (((x) & (uint32_t)0x0000ff00UL) <<  8) |\
                    (((x) & (uint32_t)0x00ff0000UL) >>  8) |\
                    (((x) & (uint32_t)0xff000000UL) >> 24))

typedef enum
{
    MUSIC_TYPE,
    TALK_TYPE,
    VOICE_MSG_TYPE,
    ONLINE_PLAYER_TYPE,
}Codec_UsageType;

#define     AUDIO_NV_MAGIC_NUMBER           0x61756469
typedef enum
{
    ADC_DIGITAL_GAIN = 1,
    DAC_DIGITAL_GAIN,
    ADC_PGA,
    MIC_GAIN,
    NR_GATE,
    ADC_EQ,
    DAC_EQ,
    SIDE_TONE,
    DRC,
    ALC,
    CODEC_ID,
    MCU_ID,
    VOLUME_LEVEL
}AudioParaType;

typedef struct
{
    uint32_t paraNo;
    int32_t minVal;
    int32_t MaxVal;
    int32_t step;
}CodecParaRange;

typedef struct
{
    const struct PIN_Res *pinRes;
    uint8_t func;
}CodecPinCfg;

typedef struct
{
    CodecPinCfg codecWs;        ///< codec ws 信号
    CodecPinCfg codecSck;       ///< codec bit 时钟
    CodecPinCfg codecOut;       ///< 发送输出给codec
    CodecPinCfg codecIn;        ///< 从codec接收数据
    CodecPinCfg codecScl;       ///< 配置codec iic 时钟
    CodecPinCfg codecSda;       ///< 配置codec iic 数据
    uint8_t busNo;              ///< I2C 总线
}CodecPin;

int8_t Codec_Initialize(uint32_t sampleRate, uint8_t dataBits);
int8_t Codec_UnInitialize(void);
int8_t Codec_Start(uint8_t mode);
int8_t Codec_Stop(uint8_t mode);
int8_t Codec_SetVolume(uint8_t volume);
int8_t Codec_SetVolumeLevel(uint8_t level);
void Codec_PinBoardInit(CodecPin *pin);
int8_t Codec_LoadAllCfgFromDev(void);
int8_t Codec_SyncAllToDev(void);
void Codec_LoadAllCfgFromNv(void);
int8_t Codec_CheckSampleRate(uint32_t sampleRate);
int8_t Codec_GetAdcDigitGain(uint8_t *para, uint8_t *len);
int8_t Codec_SetAdcDigitGain(uint8_t *para, uint8_t len);
int8_t Codec_GetDacDigitGain(uint8_t *para, uint8_t *len);
int8_t Codec_SetDacDigitGain(uint8_t *para, uint8_t len);
int8_t Codec_GetAdcPgaGain(uint8_t *para, uint8_t *len);
int8_t Codec_SetAdcPgaGain(uint8_t *para, uint8_t len);
int8_t Codec_GetMicGain(uint8_t *para, uint8_t *len);
int8_t Codec_SetMicGain(uint8_t *para, uint8_t len);
int8_t Codec_GetNrGate(uint8_t *para, uint8_t *len);
int8_t Codec_SetNrGate(uint8_t *para, uint8_t len);
int8_t Codec_GetAdcEQ(uint8_t *para, uint8_t *len);
int8_t Codec_SetAdcEQ(uint8_t *para, uint8_t len);
int8_t Codec_GetDacEQ(uint8_t *para, uint8_t *len);
int8_t Codec_SetDacEQ(uint8_t *para, uint8_t len);
int8_t Codec_GetDrc(uint8_t *para, uint8_t *len);
int8_t Codec_SetDrc(uint8_t *para, uint8_t len);
int8_t Codec_GetAlc(uint8_t *para, uint8_t *len);
int8_t Codec_SetAlc(uint8_t *para, uint8_t len);
int8_t Codec_SetSideTone(uint8_t *para, uint8_t len);
int8_t Codec_GetSideTone(uint8_t *para, uint8_t *len);
int8_t Codec_GetCodecID(uint8_t *para, uint8_t *len);
int8_t Codec_GetMcuID(uint8_t *para, uint8_t *len);
int8_t Codec_GetVolumeLevelPara(uint8_t *para, uint8_t *len);
int8_t Codec_SetVolumeLevelPara(uint8_t *para, uint8_t len);
#endif
