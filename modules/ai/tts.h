/**
 * @file tts.h
 * @brief 文本转语音 (Text-to-Speech) 接口
 *
 * 通过 HTTP 调用云端 TTS API，将文本转换为语音。
 * 支持的服务商:
 *   - OpenAI TTS API      (openai)
 *   - 百度语音合成         (baidu)
 *   - 讯飞语音合成         (iflytek)
 *   - 阿里云语音合成       (aliyun)
 *   - 腾讯云语音合成       (tencent)
 *   - Edge TTS (免费)      (edge)
 *
 * 支持功能:
 *   - 多音色/发音人选择
 *   - 语速/音量/音调调节
 *   - 流式合成 (边生成边播放)
 *   - 离线缓存 (合成结果保存为文件)
 *   - MP3 / WAV / Opus / AAC 输出格式
 *
 * 依赖:
 *   - modules/http (HTTP 客户端)
 *   - platform HAL (音频播放: I2S / DAC / PWM)
 *
 * @author 杰神 & CodeBuddy
 * @date   2026.08.01
 */
#ifndef IOT_TTS_H
#define IOT_TTS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

/* TTS 服务商 */
typedef enum {
    IOT_TTS_PROVIDER_OPENAI   = 0,   /* OpenAI TTS */
    IOT_TTS_PROVIDER_BAIDU    = 1,   /* 百度语音 */
    IOT_TTS_PROVIDER_IFLYTEK  = 2,   /* 讯飞语音 */
    IOT_TTS_PROVIDER_ALIYUN   = 3,   /* 阿里云 */
    IOT_TTS_PROVIDER_TENCENT  = 4,   /* 腾讯云 */
    IOT_TTS_PROVIDER_EDGE     = 5,   /* Microsoft Edge TTS (免费) */
} iot_tts_provider_t;

/* TTS 音频格式 */
typedef enum {
    IOT_TTS_FORMAT_MP3        = 0,   /* MP3 */
    IOT_TTS_FORMAT_WAV        = 1,   /* WAV */
    IOT_TTS_FORMAT_OPUS       = 2,   /* Opus */
    IOT_TTS_FORMAT_AAC        = 3,   /* AAC */
    IOT_TTS_FORMAT_FLAC       = 4,   /* FLAC */
    IOT_TTS_FORMAT_PCM        = 5,   /* PCM 16-bit */
} iot_tts_audio_format_t;

/* TTS 音色 */
typedef enum {
    IOT_TTS_VOICE_DEFAULT     = 0,   /* 默认音色 */
    IOT_TTS_VOICE_FEMALE_ZH   = 1,   /* 中文女声 */
    IOT_TTS_VOICE_MALE_ZH     = 2,   /* 中文男声 */
    IOT_TTS_VOICE_FEMALE_EN   = 3,   /* 英文女声 */
    IOT_TTS_VOICE_MALE_EN     = 4,   /* 英文男声 */
    IOT_TTS_VOICE_GENTLE      = 5,   /* 温柔女声 */
    IOT_TTS_VOICE_STANDARD    = 6,   /* 标准播音 */
    IOT_TTS_VOICE_CHILD       = 7,   /* 童声 */
    IOT_TTS_VOICE_ELDER       = 8,   /* 老年声 */
    IOT_TTS_VOICE_CUSTOM      = 99,  /* 自定义 (需提供 voice_id) */
} iot_tts_voice_t;

/* TTS 配置 */
typedef struct {
    iot_tts_provider_t   provider;       /* 服务商 */
    char             api_key[128];   /* API 密钥 */
    char             api_secret[128];/* API 密钥 */
    char             app_id[64];     /* App ID */
    char             voice_id[64];   /* 音色 ID (如 "alloy", "echo", "fable") */
    iot_tts_voice_t      voice;          /* 预设音色 */
    iot_tts_audio_format_t audio_format; /* 输出格式 */
    float            speed;          /* 语速 (0.25 ~ 4.0, 默认 1.0) */
    float            volume;         /* 音量 (0.0 ~ 1.0, 默认 1.0) */
    float            pitch;          /* 音调 (-20.0 ~ 20.0, 默认 0.0) */
    char             language[8];    /* 语言: zh-CN / en-US / ja-JP */
    int              timeout_ms;     /* HTTP 超时 (ms) */
    char             cache_dir[128]; /* 缓存目录 (空=不缓存) */
} iot_tts_config_t;

/* TTS 结果 */
typedef struct {
    uint8_t* audio_data;     /* 音频数据 */
    size_t   audio_len;      /* 数据长度 */
    int      duration_ms;    /* 音频时长 (ms) */
    char*    format;         /* 格式 (如 "mp3") */
    int      status_code;    /* HTTP 状态码 */
    char*    error;          /* 错误信息 (NULL=成功) */
} iot_tts_result_t;

/* TTS 句柄 */
typedef struct iot_tts_ctx iot_tts_t;

/* 异步回调 */
typedef void (*iot_tts_callback_t)(iot_tts_result_t* result, void* user_data);

/*===========================================================
 * 生命周期
 *===========================================================*/

/**
 * @brief 创建 TTS 实例
 * @param config 配置参数
 * @return TTS 句柄
 */
iot_tts_t* iot_tts_create(const iot_tts_config_t* config);

/**
 * @brief 更新配置
 * @param tts TTS 句柄
 * @param config 新配置
 * @return 0 成功, -1 失败
 */
int iot_tts_set_config(iot_tts_t* tts, const iot_tts_config_t* config);

/**
 * @brief 释放 TTS 实例
 * @param tts TTS 句柄
 */
void iot_tts_free(iot_tts_t* tts);

/**
 * @brief 获取服务商名称
 * @param provider 服务商枚举
 * @return 名称字符串
 */
const char* iot_tts_provider_name(iot_tts_provider_t provider);

/*===========================================================
 * 同步合成
 *===========================================================*/

/**
 * @brief 文本转语音 (同步)
 * @param tts TTS 句柄
 * @param text 待合成文本
 * @param result 输出结果 (调用者需 iot_tts_result_free)
 * @return 0 成功, -1 失败
 */
int iot_tts_synthesize(iot_tts_t* tts, const char* text, iot_tts_result_t* result);

/**
 * @brief SSML 合成 (支持 XML 标记控制发音)
 * @param tts TTS 句柄
 * @param ssml SSML 字符串
 * @param result 输出结果
 * @return 0 成功
 */
int iot_tts_synthesize_ssml(iot_tts_t* tts, const char* ssml, iot_tts_result_t* result);

/**
 * @brief 文本转语音并保存到文件
 * @param tts TTS 句柄
 * @param text 待合成文本
 * @param file_path 保存路径 (如 "/app/tts/output.mp3")
 * @return 0 成功, -1 失败
 */
int iot_tts_synthesize_to_file(iot_tts_t* tts, const char* text, const char* file_path);

/*===========================================================
 * 流式合成
 *===========================================================*/

/**
 * @brief 流式合成 (边生成边回调)
 *
 * 每收到一个音频 chunk 时调用 data_cb，
 * 全部完成后调用 done_cb。
 *
 * @param tts TTS 句柄
 * @param text 文本
 * @param data_cb 数据块回调 (chunk_data, chunk_len, user_data)
 * @param done_cb 完成回调 (success, user_data)
 * @param user_data 用户数据
 * @return 0 已启动
 */
int iot_tts_synthesize_stream(iot_tts_t* tts, const char* text,
                           void (*data_cb)(const uint8_t* chunk, size_t len, void* ud),
                           void (*done_cb)(bool success, void* ud),
                           void* user_data);

/*===========================================================
 * 异步合成
 *===========================================================*/

/**
 * @brief 异步合成
 * @param tts TTS 句柄
 * @param text 文本
 * @param callback 完成回调
 * @param user_data 用户数据
 * @return 0 已提交
 */
int iot_tts_synthesize_async(iot_tts_t* tts, const char* text,
                          iot_tts_callback_t callback, void* user_data);

/*===========================================================
 * 播放控制
 *===========================================================*/

/**
 * @brief 合成并立即播放 (TTS + 音频播放)
 * @param tts TTS 句柄
 * @param text 文本
 * @return 0 成功
 */
int iot_tts_speak(iot_tts_t* tts, const char* text);

/**
 * @brief 停止播放
 * @param tts TTS 句柄
 * @return 0 成功
 */
int iot_tts_stop_speaking(iot_tts_t* tts);

/**
 * @brief 是否正在播放
 * @param tts TTS 句柄
 * @return true 播放中
 */
bool iot_tts_is_speaking(iot_tts_t* tts);

/**
 * @brief 播放音频数据
 *
 * 将音频数据通过平台 HAL 播放出来。
 * 支持 PCM / WAV / MP3（取决于平台解码能力）。
 *
 * @param tts TTS 句柄
 * @param audio_data 音频数据
 * @param audio_len 数据长度
 * @return 0 成功
 */
int iot_tts_play_audio(iot_tts_t* tts, const uint8_t* audio_data, size_t audio_len);

/*===========================================================
 * 辅助
 *===========================================================*/

/**
 * @brief 释放合成结果
 * @param result 结果指针
 */
void iot_tts_result_free(iot_tts_result_t* result);

/**
 * @brief 获取音色映射表 (方便 Lua 层使用)
 * @param voice 音色枚举
 * @param provider 服务商
 * @return 该服务商对应的音色 ID 字符串
 */
const char* iot_tts_voice_id_for_provider(iot_tts_voice_t voice, iot_tts_provider_t provider);

/**
 * @brief SSML 辅助: 包装文本为 SSML
 * @param text 纯文本
 * @param buf 输出缓冲区
 * @param bufsize 缓冲区大小
 * @return 0 成功
 */
int iot_tts_text_to_ssml(const char* text, char* buf, size_t bufsize);

#ifdef __cplusplus
}
#endif

#endif /* IOT_TTS_H */
