/**
 * @file        cm_audio_common.h
 * @brief       Audio 通用接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By WP0201
 * @date        2021/4/12
 *
 * @defgroup audio_common
 * @ingroup audio_common
 * @{
 */

#ifndef __OPENCPU_AUDIO_COMMON_H__
#define __OPENCPU_AUDIO_COMMON_H__
/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* use it if audio PA solution use PA chip */
#define AUDIO_PA_USE_CHIP

/** 音频播放格式支持         */
typedef enum
{
    CM_AUDIO_PLAY_FORMAT_PCM = 1, /*!< PCM格式 */
    CM_AUDIO_PLAY_FORMAT_WAV,     /*!< WAV格式 */
    CM_AUDIO_PLAY_FORMAT_MP3,     /*!< MP3格式 */
    CM_AUDIO_PLAY_FORMAT_AMRNB,   /*!< AMR-NB格式 */
    CM_AUDIO_PLAY_FORMAT_AMRWB,   /*!< AMR-WB格式 */
    CM_AUDIO_PLAY_FORMAT_FR,      /*!< 预留，以芯片实际支持情况为准 */
    CM_AUDIO_PLAY_FORMAT_HR,      /*!< 预留，以芯片实际支持情况为准 */
    CM_AUDIO_PLAY_FORMAT_EFR,     /*!< 预留，以芯片实际支持情况为准 */
    CM_AUDIO_PLAY_FORMAT_AAC,     /*!< 预留，以芯片实际支持情况为准 */
    CM_AUDIO_PLAY_FORMAT_MID,     /*!< 预留，以芯片实际支持情况为准 */
    CM_AUDIO_PLAY_FORMAT_QTY,     /*!< 预留，以芯片实际支持情况为准 */
} cm_audio_play_format_e;

/** 音频录音格式支持，本平台需要外置codec并在完成外置codec驱动开发后可使用 */
typedef enum
{
    CM_AUDIO_RECORD_FORMAT_PCM = 1,    /*!< PCM格式 */
    CM_AUDIO_RECORD_FORMAT_WAVPCM,     /*!< WAV格式 */
    CM_AUDIO_RECORD_FORMAT_MP3,        /*!< MP3格式 ------MP3不支持*/
    CM_AUDIO_RECORD_FORMAT_AMRNB_475,  /*!< 4.75 kbps AMR格式 */
    CM_AUDIO_RECORD_FORMAT_AMRNB_515,  /*!< 5.15 kbps AMR格式 */
    CM_AUDIO_RECORD_FORMAT_AMRNB_590,  /*!< 5.90 kbps AMR格式 */
    CM_AUDIO_RECORD_FORMAT_AMRNB_670,  /*!< 6.70 kbps AMR格式 */
    CM_AUDIO_RECORD_FORMAT_AMRNB_740,  /*!< 7.40 kbps AMR格式 */
    CM_AUDIO_RECORD_FORMAT_AMRNB_795,  /*!< 7.95 kbps AMR格式 */
    CM_AUDIO_RECORD_FORMAT_AMRNB_1020, /*!< 10.20 kbps AMR格式 */
    CM_AUDIO_RECORD_FORMAT_AMRNB_1220, /*!< 12.20 kbps AMR格式 */
    CM_AUDIO_RECORD_FORMAT_AMRWB_660,  /*!< 6.60 kbps AWB格式 ----wb不支持----*/
    CM_AUDIO_RECORD_FORMAT_AMRWB_885,  /*!< 8.85 kbps AWB格式 */
    CM_AUDIO_RECORD_FORMAT_AMRWB_1265, /*!< 12.65 kbps AWB格式 */
    CM_AUDIO_RECORD_FORMAT_AMRWB_1425, /*!< 14.25 kbps AWB格式 */
    CM_AUDIO_RECORD_FORMAT_AMRWB_1585, /*!< 15.85 kbps AWB格式 */
    CM_AUDIO_RECORD_FORMAT_AMRWB_1825, /*!< 18.25 kbps AWB格式 */
    CM_AUDIO_RECORD_FORMAT_AMRWB_1985, /*!< 19.85 kbps AWB格式 */
    CM_AUDIO_RECORD_FORMAT_AMRWB_2305, /*!< 23.05 kbps AWB格式 */
    CM_AUDIO_RECORD_FORMAT_AMRWB_2385, /*!< 23.85 kbps AWB格式 */
} cm_audio_record_format_e;

/** PCM音频采样格式 */
typedef enum
{
    CM_AUDIO_SAMPLE_FORMAT_8BIT = 8,            /*!< 8位，暂不开放，请使用CM_AUDIO_SAMPLE_FORMAT_16BIT */
    CM_AUDIO_SAMPLE_FORMAT_16BIT = 16,               /*!< 16位, 小端 */
    CM_AUDIO_SAMPLE_FORMAT_24BIT = 24,               /*!< 24位, 本平台不支持 */
    CM_AUDIO_SAMPLE_FORMAT_32BIT = 32,               /*!< 32位, 暂不开放，请使用CM_AUDIO_SAMPLE_FORMAT_16BIT */
} cm_audio_sample_format_e;

/** 音频播放支持的采样率 */
typedef enum
{
    CM_AUDIO_SAMPLE_RATE_8000HZ  =  8000,
    CM_AUDIO_SAMPLE_RATE_11025HZ = 11025,
    CM_AUDIO_SAMPLE_RATE_12000HZ = 12000,
    CM_AUDIO_SAMPLE_RATE_16000HZ = 16000,
    CM_AUDIO_SAMPLE_RATE_22050HZ = 22050,
    CM_AUDIO_SAMPLE_RATE_24000HZ = 24000,
    CM_AUDIO_SAMPLE_RATE_32000HZ = 32000,
    CM_AUDIO_SAMPLE_RATE_44100HZ = 44100,
    CM_AUDIO_SAMPLE_RATE_48000HZ = 48000,
    CM_AUDIO_SAMPLE_RATE_96000HZ = 96000,   //mp3不支持
} cm_audio_sample_rate_e;

/** PCM音频采样通道 */
typedef enum
{
    CM_AUDIO_SOUND_MONO = 1,            /*!< （默认）单通道，录音时默认且只能使用CM_AUDIO_SOUND_MONO */
    CM_AUDIO_SOUND_STEREO = 2,          /*!< 双通道（立体声） */
} cm_audio_sound_channel_e;

#define     CM_AMR_WB_FILE_TAG_LEN             9       /** WB 文件头标识长度*/
#define     CM_AMR_NB_FILE_TAG_LEN             6       /** NB 文件头标识长度*/
#define     CM_AMR_NB_SAMPLERATE               8000    /** NB 采样率*/
#define     CM_AMR_WB_SAMPLERATE               16000   /** WB 采样率*/
#define     CM_AMR_NB_FRAME_SIZE               320     /** NB 帧大小*/
#define     CM_AMR_WB_FRAME_SIZE               640     /** WB 帧大小*/
#define     CM_AMR_BITS_PER_SAMPLE             16      /** 采样位数*/
#define CM_AMR_WB_TAG      "#!AMR-WB\n"
#define CM_AMR_TAG         "#!AMR\n"
#define CM_AMR_WB_TAG_LEN  (sizeof(CM_AMR_WB_TAG) -1)
#define CM_AMR_TAG_LEN     (sizeof(CM_AMR_TAG) -1)
/** 音频采样参数结构体 */
typedef struct
{
    cm_audio_sample_format_e sample_format;     /*!< 采样格式（PCM） */
    cm_audio_sample_rate_e rate;                /*!< 采样率（PCM） */
    cm_audio_sound_channel_e num_channels;      /*!< 采样声道（PCM） */
} cm_audio_sample_param_t;

typedef enum
{
    CM_AUDIO_PATH_DAC = 0,
    CM_AUDIO_PATH_CODE,
    CM_AUDIO_PATH_MAX
}cm_audio_path_e;

typedef enum
{
    CM_AUDIO_TASK_PLAY = 0,
    CM_AUDIO_TASK_RECORD
}cm_audio_task_e;
void cm_audio_init(void);

/**
 * @brief set audio hardware path in NV
 *
 * @param path_id  0-DAC,1-CODEC,2-PWM
 *
 * @return =0 succ; <0 failed
 */
int cm_audio_path_set_nv(uint8_t path_id);

/**
 * @brief get audio hardware path in NV
 *
 * @return <0 failed, other: audio hardware path id
 */
int cm_audio_path_get_nv(void);

/**
 * @brief get current audio hardware path
 *
 * @return <0 failed, other: audio hardware path id
 */
int cm_audio_path_get(void);

/**
 * @brief set audio PA control id in NV
 *
 * @param pa_ctrl_id  0/1/2/3/4
 *     0/1/2/3 corresponds to 0/1/2/3 in AT+MGPIO(module-pin 76/76/86/87)
 *
 * @return =0 succ; <0 failed
 */
int cm_audio_pa_ctrl_set_nv(uint8_t pa_ctrl_id);

/**
 * @brief get audio PA control id in NV
 *
 * @return <0 failed, other: audio hardware PA control id
 */
int cm_audio_pa_ctrl_get_nv(void);

/**
 * @brief get current audio PA control id
 *
 * @return <0 failed, other: audio hardware PA control id
 */
int cm_audio_pa_ctrl_get(void);

/**
 * @brief turn audio PA ON/OFF
 * @param state
 *     0 - turn off, 1 - turn on
 *
 * @return none
 */
void cm_audio_pa_control(int pa_ctrl_id, uint8_t state);

/**
 * @brief get audio state
 *
 * @return audio state
 *    0 - idle, 1 - busy
 */
uint8_t cm_audio_state_get(void);

/**
 * @brief set audio state
 *
 * @param state
 *     0 - idle, 1 - busy
 *
 * @return none
 *
 */
uint8_t cm_audio_state_set(uint8_t state, cm_audio_task_e task);
#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif
/** @}*/