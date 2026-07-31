/**
 * @file stt.h
 * @brief 语音识别 (Speech-to-Text) 接口
 *
 * 录音后通过 HTTP 调用云端 STT API 进行语音转文字。
 * 支持的 STT 服务商:
 *   - OpenAI Whisper API         (openai)
 *   - 百度语音识别               (baidu)
 *   - 讯飞语音听写               (iflytek)
 *   - 阿里云一句话识别           (aliyun)
 *   - 腾讯云 ASR                 (tencent)
 *
 * 支持功能:
 *   - 实时录音识别 (边录边传)
 *   - 离线录音文件识别 (上传文件)
 *   - 多语言支持 (中/英/日/韩 等)
 *   - 热词/自定义词库
 *
 * 依赖:
 *   - modules/http (HTTP 客户端)
 *   - platform HAL (音频采集: I2S / ADC / MEMS 麦克风)
 *
 * @author 杰神 & CodeBuddy
 * @date   2026.08.01
 */
#ifndef IOT_STT_H
#define IOT_STT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

/* STT 服务商 */
typedef enum {
    STT_PROVIDER_AUTO     = 0,   /* 自动选择 (优先本地, 后云端) */
    STT_PROVIDER_OPENAI   = 1,   /* OpenAI Whisper */
    STT_PROVIDER_BAIDU    = 2,   /* 百度语音 */
    STT_PROVIDER_IFLYTEK  = 3,   /* 讯飞语音 */
    STT_PROVIDER_ALIYUN   = 4,   /* 阿里云 */
    STT_PROVIDER_TENCENT  = 5,   /* 腾讯云 */
} stt_provider_t;

/* 音频编码格式 */
typedef enum {
    STT_AUDIO_PCM_S16LE   = 0,   /* PCM 16-bit signed little-endian */
    STT_AUDIO_WAV         = 1,   /* WAV (PCM 16-bit) */
    STT_AUDIO_MP3         = 2,   /* MP3 */
    STT_AUDIO_OPUS        = 3,   /* Opus */
} stt_audio_format_t;

/* STT 配置 */
typedef struct {
    stt_provider_t   provider;       /* 服务商 */
    char             api_key[128];   /* API 密钥 */
    char             api_secret[128];/* API 密钥 (部分服务商需要) */
    char             app_id[64];     /* App ID (部分服务商需要) */
    char             language[8];    /* 语言代码: zh / en / ja / ko */
    stt_audio_format_t audio_format; /* 音频格式 */
    int              sample_rate;    /* 采样率: 8000 / 16000 / 24000 */
    int              channels;       /* 声道数: 1 (单声道) / 2 (立体声) */
    int              bits_per_sample;/* 位深: 16 */
    bool             enable_vad;     /* 启用 VAD (语音活动检测) */
    int              vad_silence_ms; /* VAD 静音阈值 (ms) */
    int              max_duration_s; /* 最大录音时长 (秒, 0=不限制) */
    char**           hotwords;       /* 热词列表 (以 NULL 结尾) */
    int              hotword_count;  /* 热词数量 */
    int              timeout_ms;     /* HTTP 超时 (ms) */
} stt_config_t;

/* STT 结果 */
typedef struct {
    char*    text;           /* 识别文本 */
    float    confidence;     /* 置信度 (0.0 ~ 1.0) */
    char*    language;       /* 检测到的语言 */
    int      duration_ms;    /* 音频时长 (ms) */
    int      status_code;    /* HTTP 状态码 */
    char*    error;          /* 错误信息 (NULL 表示成功) */
    char*    raw_response;   /* 原始响应 (调试用) */
} stt_result_t;

/* STT 句柄 (不透明) */
typedef struct stt_ctx stt_t;

/* 异步回调 */
typedef void (*stt_callback_t)(stt_result_t* result, void* user_data);

/* VAD 事件回调 (不说话->开始说话 / 说话中->沉默) */
typedef void (*stt_vad_callback_t)(bool is_speech, void* user_data);

/* 实时录音回调 */
typedef int (*stt_audio_callback_t)(const uint8_t* data, size_t len,
                                     void* user_data);

/*===========================================================
 * 生命周期
 *===========================================================*/

/**
 * @brief 创建 STT 实例
 * @param config 配置参数
 * @return STT 句柄, 失败返回 NULL
 */
stt_t* stt_create(const stt_config_t* config);

/**
 * @brief 更新配置
 * @param stt STT 句柄
 * @param config 新配置
 * @return 0 成功, -1 失败
 */
int stt_set_config(stt_t* stt, const stt_config_t* config);

/**
 * @brief 释放 STT 实例
 * @param stt STT 句柄
 */
void stt_free(stt_t* stt);

/**
 * @brief 获取服务商名称
 * @param provider 服务商枚举
 * @return 名称字符串
 */
const char* stt_provider_name(stt_provider_t provider);

/*===========================================================
 * 同步识别
 *===========================================================*/

/**
 * @brief 识别音频文件
 * @param stt STT 句柄
 * @param file_path 音频文件路径 (如 /app/record.wav)
 * @param result 输出识别结果 (调用者需 stt_result_free)
 * @return 0 成功, -1 失败
 */
int stt_recognize_file(stt_t* stt, const char* file_path,
                        stt_result_t* result);

/**
 * @brief 识别音频缓冲区
 * @param stt STT 句柄
 * @param audio_data 音频数据
 * @param audio_len 数据长度
 * @param result 输出识别结果
 * @return 0 成功, -1 失败
 */
int stt_recognize_buffer(stt_t* stt, const uint8_t* audio_data,
                          size_t audio_len, stt_result_t* result);

/*===========================================================
 * 实时录音识别
 *===========================================================*/

/**
 * @brief 开始录音并实时识别
 *
 * 从麦克风采集音频 -> VAD 检测语音片段 -> HTTP 发送 -> 返回识别结果。
 * 整个过程在后台线程中执行，完成时通过回调通知。
 *
 * @param stt STT 句柄
 * @param callback 完成回调
 * @param user_data 用户数据
 * @return 0 已启动, -1 失败
 */
int stt_start_record_and_recognize(stt_t* stt, stt_callback_t callback,
                                    void* user_data);

/**
 * @brief 停止录音
 * @param stt STT 句柄
 * @return 0 成功, -1 失败
 */
int stt_stop_recording(stt_t* stt);

/**
 * @brief 是否正在录音
 * @param stt STT 句柄
 * @return true 录音中
 */
bool stt_is_recording(stt_t* stt);

/**
 * @brief 设置 VAD 事件回调
 * @param stt STT 句柄
 * @param vad_cb VAD 回调
 * @param user_data 用户数据
 */
void stt_set_vad_callback(stt_t* stt, stt_vad_callback_t vad_cb,
                           void* user_data);

/**
 * @brief 设置实时音频数据回调 (用于波形显示等)
 * @param stt STT 句柄
 * @param audio_cb 音频数据回调
 * @param user_data 用户数据
 */
void stt_set_audio_callback(stt_t* stt, stt_audio_callback_t audio_cb,
                             void* user_data);

/*===========================================================
 * 连续识别 (唤醒词模式)
 *===========================================================*/

/**
 * @brief 设置唤醒词
 * @param stt STT 句柄
 * @param wake_word 唤醒词 (如 "小杰小杰")
 * @return 0 成功, -1 失败
 */
int stt_set_wake_word(stt_t* stt, const char* wake_word);

/**
 * @brief 开始连续监听 (唤醒词 + 自动识别)
 * @param stt STT 句柄
 * @param callback 识别完成回调
 * @param user_data 用户数据
 * @return 0 已启动
 */
int stt_start_continuous_listen(stt_t* stt, stt_callback_t callback,
                                 void* user_data);

/**
 * @brief 停止连续监听
 * @param stt STT 句柄
 * @return 0 成功
 */
int stt_stop_continuous_listen(stt_t* stt);

/*===========================================================
 * 辅助
 *===========================================================*/

/**
 * @brief 释放识别结果
 * @param result 结果指针
 */
void stt_result_free(stt_result_t* result);

/**
 * @brief WAV 头写入 (用于将 PCM 编码为 WAV 文件)
 * @param pcm_data PCM 数据
 * @param pcm_size PCM 大小
 * @param sample_rate 采样率
 * @param channels 声道数
 * @param bits_per_sample 位深
 * @param wav_data 输出 WAV 数据 (调用者 free)
 * @param wav_size 输出 WAV 大小
 * @return 0 成功
 */
int stt_pcm_to_wav(const uint8_t* pcm_data, size_t pcm_size,
                    int sample_rate, int channels, int bits_per_sample,
                    uint8_t** wav_data, size_t* wav_size);

/**
 * @brief 简单的 VAD 检测 (基于能量阈值)
 * @param pcm_data PCM 数据
 * @param len 数据长度 (字节)
 * @param threshold 能量阈值 (默认 500)
 * @return true 检测到语音活动
 */
bool stt_vad_detect(const int16_t* pcm_data, size_t len, int threshold);

#ifdef __cplusplus
}
#endif

#endif /* IOT_STT_H */
