/**
 * @file iot_audio.h
 * @brief ML307N 平台音频播放与录音适配器头文件
 * @details 封装 ML307N 平台音频播放(cm_audio_player)和录音(cm_audio_recorder)接口，
 *          提供统一的音频操作函数，支持跨平台编译。
 */

#ifndef IOT_AUDIO_ML307N_H
#define IOT_AUDIO_ML307N_H

#include "cm_audio_common.h"
#include "cm_audio_player.h"
#include "cm_audio_recorder.h"

/* ============================================================
 * 统一音频类型定义（跨平台兼容）
 * ============================================================ */

/** @brief 音频格式枚举 */
typedef enum {
    IOT_AUDIO_FORMAT_PCM  = 0,   /**< PCM格式 */
    IOT_AUDIO_FORMAT_WAV  = 1,   /**< WAV格式 */
    IOT_AUDIO_FORMAT_MP3  = 2,   /**< MP3格式 */
    IOT_AUDIO_FORMAT_AMR  = 3,   /**< AMR格式 */
    IOT_AUDIO_FORMAT_FLAC = 4,   /**< FLAC格式 */
} iot_audio_format_t;

/** @brief 音频采样率枚举 */
typedef enum {
    IOT_AUDIO_SAMPLE_RATE_8K  = 8000,    /**< 8K采样率 */
    IOT_AUDIO_SAMPLE_RATE_16K = 16000,   /**< 16K采样率 */
    IOT_AUDIO_SAMPLE_RATE_22K = 22050,   /**< 22.05K采样率 */
    IOT_AUDIO_SAMPLE_RATE_24K = 24000,   /**< 24K采样率 */
    IOT_AUDIO_SAMPLE_RATE_32K = 32000,   /**< 32K采样率 */
    IOT_AUDIO_SAMPLE_RATE_44K = 44100,  /**< 44.1K采样率 */
    IOT_AUDIO_SAMPLE_RATE_48K = 48000,  /**< 48K采样率 */
} iot_audio_sample_rate_t;

/** @brief 音频播放音量等级枚举 */
typedef enum {
    IOT_AUDIO_VOLUME_0 = 0,           /**< 音量0 */
    IOT_AUDIO_VOLUME_1 = 1,           /**< 音量1 */
    IOT_AUDIO_VOLUME_2 = 2,           /**< 音量2 */
    IOT_AUDIO_VOLUME_3 = 3,           /**< 音量3 */
    IOT_AUDIO_VOLUME_4 = 4,           /**< 音量4 */
    IOT_AUDIO_VOLUME_5 = 5,           /**< 音量5 */
    IOT_AUDIO_VOLUME_MAX = IOT_AUDIO_VOLUME_5,
} iot_audio_volume_t;

/** @brief 音频播放通道枚举 */
typedef enum {
    IOT_AUDIO_CHANNEL_LEFT   = 0,    /**< 左声道 */
    IOT_AUDIO_CHANNEL_RIGHT  = 1,     /**< 右声道 */
    IOT_AUDIO_CHANNEL_STEREO = 2,    /**< 立体声 */
} iot_audio_channel_t;

/** @brief 音频采样参数结构体 */
typedef struct {
    uint32_t sample_rate;             /**< 采样率(Hz) */
    uint8_t channels;                 /**< 声道数 */
    uint8_t bit_width;                /**< 位宽 */
} iot_audio_sample_param_t;

/** @brief 音频播放回调函数类型 */
typedef void (*iot_audio_callback_t)(void *cb_param);

/** @brief 录音数据回调函数类型 */
typedef void (*iot_audio_record_callback_t)(const void *data, uint32_t size, void *cb_param);

/* ============================================================
 * 音频播放操作函数声明
 * ============================================================ */

/**
 * @brief 从内存中播放音频数据
 * @param[in] data         音频数据指针
 * @param[in] size         音频数据长度
 * @param[in] format       音频格式
 * @param[in] sample_param PCM采样参数（非PCM格式传NULL）
 * @param[in] cb           播放完成回调函数
 * @param[in] cb_param     回调用户参数
 * @return 0 成功，-1 失败
 */
int iot_audio_play(const void *data, uint32_t size, iot_audio_format_t format,
                         const iot_audio_sample_param_t *sample_param,
                         iot_audio_callback_t cb, void *cb_param);

/**
 * @brief 播放音频文件
 * @param[in] path         文件路径
 * @param[in] format       音频格式
 * @param[in] sample_param PCM采样参数
 * @param[in] cb           播放完成回调函数
 * @param[in] cb_param     回调用户参数
 * @return 0 成功，-1 失败
 */
int iot_audio_play_file(const char *path, iot_audio_format_t format,
                              const iot_audio_sample_param_t *sample_param,
                              iot_audio_callback_t cb, void *cb_param);

/**
 * @brief 暂停播放
 * @return 0 成功，-1 失败
 */
int iot_audio_play_pause(void);

/**
 * @brief 继续播放
 * @return 0 成功，-1 失败
 */
int iot_audio_play_resume(void);

/**
 * @brief 停止播放
 * @return 0 成功，-1 失败
 */
int iot_audio_play_stop(void);

/**
 * @brief 设置播放音量
 * @param[in] vol 音量值 0-100（10的倍数）
 * @return 0 成功，其他失败
 */
int iot_audio_play_set_volume(int vol);

/**
 * @brief 获取播放音量
 * @param[out] vol 音量值指针
 * @return 0 成功，其他失败
 */
int iot_audio_play_get_volume(int *vol);

/* ============================================================
 * 流播放操作函数声明
 * ============================================================ */

/**
 * @brief 打开流播放通道
 * @param[in] format       音频格式
 * @param[in] sample_param PCM采样参数
 * @return 0 成功，-1 失败
 */
int iot_audio_stream_open(iot_audio_format_t format, const iot_audio_sample_param_t *sample_param);

/**
 * @brief 往流播放通道推送音频数据
 * @param[in] data 音频数据指针
 * @param[in] size 数据长度
 * @return >=0 实际写入长度，-1 失败
 */
int iot_audio_stream_push(const void *data, uint32_t size);

/**
 * @brief 关闭流播放通道
 * @return 0 成功
 */
int iot_audio_stream_close(void);

/* ============================================================
 * 录音操作函数声明
 * ============================================================ */

/**
 * @brief 开始录音
 * @param[in] format       录音音频格式
 * @param[in] sample_param PCM采样参数
 * @param[in] cb           录音数据回调函数
 * @param[in] cb_param     回调用户参数
 * @return 0 成功，-1 失败
 */
int iot_audio_record_start(iot_audio_format_t format, const iot_audio_sample_param_t *sample_param,
                                 iot_audio_record_callback_t cb, void *cb_param);

/**
 * @brief 停止录音
 * @return 0 成功
 */
int iot_audio_record_stop(void);

#endif /* IOT_AUDIO_ML307N_H */