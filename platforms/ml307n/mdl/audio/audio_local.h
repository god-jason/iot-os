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
 * @file        audio_local.h
 *
 * @brief       This file provides operation functions declaration for audio.
 *
 * @revision
 * Date         Author          Notes
 * 2020-02-20   OneOS Team      First Version
 ***********************************************************************************************************************
 */
#ifndef __DRV_AUDIO_H__
#define __DRV_AUDIO_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "audio_monitor.h"
#include "audio_codec.h"

#define     AUDIO_PLAY_MODE          1
#define     AUDIO_RECORD_MODE        2
#define     AUDIO_PLAY_RECORD_MODE   3

/**
 * @brief AMR 相关定义
 * 
 */
#define     AMR_WB_FILE_TAG_LEN             9       /** WB 文件头标识长度*/
#define     AMR_NB_FILE_TAG_LEN             6       /** NB 文件头标识长度*/
#define     AMR_NB_SAMPLERATE               8000    /** NB 采样率*/
#define     AMR_WB_SAMPLERATE               16000   /** WB 采样率*/
#define     AMR_NB_FRAME_SIZE               320     /** NB 帧大小*/
#define     AMR_WB_FRAME_SIZE               640     /** WB 帧大小*/
#define     AMR_BITS_PER_SAMPLE             16      /** 采样位数*/

/**
 * @brief 默认帧大小 
 * 
 */
#define     AUDIO_DEFAULT_FRAME_SIZE        4096

/**
 * @brief Audio 回调类型
 */
typedef void (*AudioCallBack) (uint32_t event, void *para);  

/**
 * @brief 音频文件类型
 * 
 */
typedef enum 
{
    WAV_RAW_DATA = 0,       /** WAV 文件转成 数组数据*/
    MP3_RAW_DATA,       /** MP3 文件转成 数组数据*/
    AMR_RAW_DATA,       /** AMR NB文件转成 数组数据*/
    PCM_RAW_DATA,       /** 纯PCM 数据*/
    AMR_WB_RAW_DATA,    /** AMR WB 数组数据*/
    WAV_FILE,
    MP3_FILE,
    AMR_FILE,


}AudioFileType;

/**
 * @brief 音频数据信息
 * 
 */
typedef struct 
{
    uint8_t ctrl;           /** 播放 或 录音*/
    uint8_t audioType;      /** 音频类型 AudioFileType*/
    uint8_t isLoop;         /** 是否循环播放*/
    Codec_UsageType usageType;  /** 使用场景， default music*/
    /* 纯PCM数据，
       需要指定音频参数*/
    uint8_t dataBits;       /** 采样点位数*/
    uint8_t chnls;          /** 通道个数*/
    uint32_t sampleRate;    /** 采样率*/
    uint32_t frameSize;     /** 帧大小*/

    char file[64];          /** 文件名， 用于文件系统音频文件*/
    uint8_t *data;          /** 播放数据的空间， 用于代码块中的音频数据*/
    uint32_t size;          /** 播放数据大小或录音存储的空间大小*/

    uint8_t *recordData;    /** 录音数据的空间 */
    uint32_t recordSize;    /** 录音存储空间大小 */

    AudioCallBack cb;       /** 用户回调接口，用于通知使用者*/
    AudioCallBack sysCb;     /** 系统管理使用*/
}AudioInfo;

typedef AudioInfo   LocalAudioCfg;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/


int8_t AudioLocal_Open(void * cfg);
int8_t AudioLocal_Close(uint8_t );
int8_t Audio_GetWavFileInfo(uint8_t *data, uint32_t dataSize, uint8_t *dataBits, uint32_t *sampleRate, uint8_t *chnls);
#endif
