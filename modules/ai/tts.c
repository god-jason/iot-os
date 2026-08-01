/**
 * @file tts.c
 * @brief 文本转语音实现
 *
 * 处理流程:
 *   1. 构建请求 (JSON body 含 text / voice / speed 等参数)
 *   2. HTTP POST 到 TTS API
 *   3. 接收音频数据 (二进制, 非 JSON)
 *   4. 可选: 写入缓存文件 + 音频播放
 *
 * 各服务商 API 端点:
 *   - OpenAI:  POST /v1/audio/speech  -> 返回 audio/mpeg 二进制
 *   - 百度:    POST https://tsn.baidu.com/text2audio  -> 返回 audio/mp3
 *   - Edge:    POST wss://speech.platform.bing.com/...  -> WebSocket 流式
 *   - 阿里云:  POST https://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/tts
 *   - 腾讯云:  POST https://tts.tencentcloudapi.com
 *   - 讯飞:    WebSocket wss://tts-api.xfyun.cn/v2/tts
 *
 * @author 杰神 & CodeBuddy
 * @date   2026.08.01
 */
#include "tts.h"
#include "http_client.h"
#include "iot_log.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*===========================================================
 * 常量
 *===========================================================*/

#define IOT_TTS_JSON_BUF_SIZE   4096
#define IOT_TTS_AUDIO_MAX_SIZE  (512 * 1024)  /* 单次合成最大 512KB */

/*===========================================================
 * 内部结构
 *===========================================================*/

typedef struct {
    iot_tts_config_t config;
    bool         is_speaking;

    /* 播放回调 */
    void (*stream_data_cb)(const uint8_t*, size_t, void*);
    void (*stream_done_cb)(bool, void*);
    void*        stream_ud;

    /* 缓存 */
    char         cache_path[256]; /* 最后合成的缓存文件路径 */
} tts_impl_t;

/*===========================================================
 * 内部工具函数
 *===========================================================*/

static char* json_extract_string(const char* json, const char* key)
{
    if (!json || !key) return NULL;
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    const char* pos = strstr(json, pattern);
    if (!pos) return NULL;
    pos += strlen(pattern);
    while (*pos && (*pos == ' ' || *pos == ':')) pos++;
    if (*pos != '\"') return NULL;
    pos++;
    char value[4096] = {0};
    int i = 0;
    while (*pos && i < (int)sizeof(value) - 1) {
        if (*pos == '\\') {
            pos++;
            if      (*pos == 'n') value[i++] = '\n';
            else if (*pos == 'r') value[i++] = '\r';
            else if (*pos == 't') value[i++] = '\t';
            else if (*pos == '"') value[i++] = '"';
            else if (*pos == '\\') value[i++] = '\\';
            else { value[i++] = '\\'; if (*pos) value[i++] = *pos; }
        } else if (*pos == '"') {
            break;
        } else {
            value[i++] = *pos;
        }
        pos++;
    }
    value[i] = '\0';
    return iot_strdup(value);
}

/*===========================================================
 * 提供者名称
 *===========================================================*/

const char* iot_tts_provider_name(iot_tts_provider_t provider)
{
    static const char* names[] = {
        "openai", "baidu", "iflytek", "aliyun", "tencent", "edge"
    };
    if (provider > IOT_TTS_PROVIDER_EDGE) return "unknown";
    return names[provider];
}

/*===========================================================
 * 音色映射
 *===========================================================*/

const char* iot_tts_voice_id_for_provider(iot_tts_voice_t voice, iot_tts_provider_t provider)
{
    /* OpenAI 音色映射 */
    static const char* openai_voices[] = {
        [IOT_TTS_VOICE_DEFAULT]     = "alloy",
        [IOT_TTS_VOICE_FEMALE_ZH]   = "nova",
        [IOT_TTS_VOICE_MALE_ZH]     = "onyx",
        [IOT_TTS_VOICE_FEMALE_EN]   = "shimmer",
        [IOT_TTS_VOICE_MALE_EN]     = "echo",
        [IOT_TTS_VOICE_GENTLE]      = "nova",
        [IOT_TTS_VOICE_STANDARD]    = "alloy",
        [IOT_TTS_VOICE_CHILD]       = "fable",
        [IOT_TTS_VOICE_ELDER]       = "onyx",
    };

    /* Edge TTS 音色映射 (免费) */
    static const char* edge_voices[] = {
        [IOT_TTS_VOICE_DEFAULT]     = "zh-CN-XiaoxiaoNeural",
        [IOT_TTS_VOICE_FEMALE_ZH]   = "zh-CN-XiaoxiaoNeural",
        [IOT_TTS_VOICE_MALE_ZH]     = "zh-CN-YunxiNeural",
        [IOT_TTS_VOICE_FEMALE_EN]   = "en-US-JennyNeural",
        [IOT_TTS_VOICE_MALE_EN]     = "en-US-GuyNeural",
        [IOT_TTS_VOICE_GENTLE]      = "zh-CN-XiaohanNeural",
        [IOT_TTS_VOICE_STANDARD]    = "zh-CN-YunyangNeural",
        [IOT_TTS_VOICE_CHILD]       = "zh-CN-XiaoyiNeural",
        [IOT_TTS_VOICE_ELDER]       = "zh-CN-YunfengNeural",
    };

    if (voice > IOT_TTS_VOICE_ELDER) return "alloy";

    switch (provider) {
    case IOT_TTS_PROVIDER_OPENAI:
        return openai_voices[voice] ? openai_voices[voice] : "alloy";
    case IOT_TTS_PROVIDER_EDGE:
        return edge_voices[voice] ? edge_voices[voice] : "zh-CN-XiaoxiaoNeural";
    default:
        return "default";
    }
}

/*===========================================================
 * 获取 API URL
 *===========================================================*/

static const char* iot_tts_get_api_url(iot_tts_provider_t provider)
{
    switch (provider) {
    case IOT_TTS_PROVIDER_OPENAI:
        return "https://api.openai.com/v1/audio/speech";
    case IOT_TTS_PROVIDER_BAIDU:
        return "https://tsn.baidu.com/text2audio";
    case IOT_TTS_PROVIDER_ALIYUN:
        return "https://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/tts";
    case IOT_TTS_PROVIDER_TENCENT:
        return "https://tts.tencentcloudapi.com";
    case IOT_TTS_PROVIDER_EDGE:
        return "https://eastus.tts.speech.microsoft.com/cognitiveservices/v1";
    case IOT_TTS_PROVIDER_IFLYTEK:
        return "wss://tts-api.xfyun.cn/v2/tts";
    default:
        return NULL;
    }
}

/*===========================================================
 * OpenAI TTS 实现
 *===========================================================*/

static int iot_tts_openai_synthesize(iot_tts_t* tts, const char* text, const char* ssml,
                                  iot_tts_result_t* result)
{
    tts_impl_t* impl = (tts_impl_t*)tts;

    /* 构建请求 JSON body */
    const char* voice_id = impl->config.voice_id[0]
                         ? impl->config.voice_id
                         : iot_tts_voice_id_for_provider(impl->config.voice, IOT_TTS_PROVIDER_OPENAI);

    char format_str[16];
    switch (impl->config.audio_format) {
    case IOT_TTS_FORMAT_MP3:  strncpy(format_str, "mp3",  sizeof(format_str)); break;
    case IOT_TTS_FORMAT_OPUS: strncpy(format_str, "opus", sizeof(format_str)); break;
    case IOT_TTS_FORMAT_AAC:  strncpy(format_str, "aac",  sizeof(format_str)); break;
    case IOT_TTS_FORMAT_FLAC: strncpy(format_str, "flac", sizeof(format_str)); break;
    case IOT_TTS_FORMAT_WAV:
    case IOT_TTS_FORMAT_PCM:
    default:              strncpy(format_str, "mp3",  sizeof(format_str)); break;
    }

    char json_body[IOT_TTS_JSON_BUF_SIZE];
    if (ssml) {
        /* SSML 模式 */
        snprintf(json_body, sizeof(json_body),
            "{"
            "\"model\":\"tts-1\","
            "\"input\":\"%s\","
            "\"voice\":\"%s\","
            "\"response_format\":\"%s\","
            "\"speed\":%.2f"
            "}",
            ssml, voice_id, format_str, impl->config.speed);
    } else {
        /* 纯文本模式 */
        char escaped_text[IOT_TTS_JSON_BUF_SIZE / 2];
        char* p = escaped_text;
        for (const char* s = text; *s && (p - escaped_text) < (int)sizeof(escaped_text) - 3; s++) {
            switch (*s) {
            case '"':  *p++='\\'; *p++='"';  break;
            case '\\': *p++='\\'; *p++='\\'; break;
            case '\n': *p++='\\'; *p++='n';  break;
            case '\r': *p++='\\'; *p++='r';  break;
            case '\t': *p++='\\'; *p++='t';  break;
            default:   *p++ = *s; break;
            }
        }
        *p = '\0';

        snprintf(json_body, sizeof(json_body),
            "{"
            "\"model\":\"tts-1\","
            "\"input\":\"%s\","
            "\"voice\":\"%s\","
            "\"response_format\":\"%s\","
            "\"speed\":%.2f"
            "}",
            escaped_text, voice_id, format_str, impl->config.speed);
    }

    LOG_DEBUG("tts", "OpenAI TTS: voice=%s, format=%s, speed=%.2f, len=%zu",
              voice_id, format_str, impl->config.speed, strlen(text));

    /* HTTP POST */
    iot_http_response_t http_resp;
    memset(&http_resp, 0, sizeof(http_resp));

    int ret = iot_http_post(iot_tts_get_api_url(IOT_TTS_PROVIDER_OPENAI),
                        json_body, strlen(json_body),
                        "application/json", &http_resp);

    memset(result, 0, sizeof(iot_tts_result_t));

    if (ret != 0) {
        result->error = iot_strdup(http_resp.error ? http_resp.error : "HTTP error");
        result->status_code = http_resp.status_code;
        iot_http_response_free(&http_resp);
        return -1;
    }

    result->status_code = http_resp.status_code;

    /* OpenAI TTS 直接返回音频二进制数据 */
    if (http_resp.status_code == 200 && http_resp.body && http_resp.body_len > 0) {
        result->audio_data = (uint8_t*)malloc(http_resp.body_len);
        if (result->audio_data) {
            memcpy(result->audio_data, http_resp.body, http_resp.body_len);
            result->audio_len = http_resp.body_len;
            result->format = iot_strdup(format_str);

            LOG_INFO("tts", "Synthesized: %zu bytes (%s)",
                     result->audio_len, format_str);
        }
    } else {
        result->error = iot_strdup(http_resp.body
                                   ? http_resp.body
                                   : "Empty response");
    }

    iot_http_response_free(&http_resp);
    return result->error ? -1 : 0;
}

/*===========================================================
 * Edge TTS 实现 (免费)
 *
 * Edge TTS 使用 Microsoft Cognitive Services:
 *   POST https://<region>.tts.speech.microsoft.com/cognitiveservices/v1
 *   Headers:
 *     Authorization: Bearer <token>  (可选, 匿名也可用)
 *     Content-Type: application/ssml+xml
 *     X-Microsoft-OutputFormat: audio-16khz-32kbitrate-mono-mp3
 *   Body: SSML XML
 *===========================================================*/

static int iot_tts_edge_synthesize(iot_tts_t* tts, const char* text, const char* ssml,
                                iot_tts_result_t* result)
{
    tts_impl_t* impl = (tts_impl_t*)tts;

    const char* voice_id = impl->config.voice_id[0]
                         ? impl->config.voice_id
                         : iot_tts_voice_id_for_provider(impl->config.voice, IOT_TTS_PROVIDER_EDGE);

    /* 构建 SSML */
    char ssml_body[IOT_TTS_JSON_BUF_SIZE];
    const char* actual_text = ssml ? ssml : text;

    /* 如果是纯文本, 包装为 SSML */
    if (!ssml) {
        snprintf(ssml_body, sizeof(ssml_body),
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<speak version=\"1.0\" xmlns=\"http://www.w3.org/2001/10/synthesis\" "
            "xmlns:mstts=\"http://www.w3.org/2001/mstts\" "
            "xml:lang=\"%s\">"
            "<voice name=\"%s\">"
            "<prosody rate=\"%.2f\" volume=\"%.2f\" pitch=\"%.2f%%\">"
            "%s"
            "</prosody>"
            "</voice>"
            "</speak>",
            impl->config.language, voice_id,
            impl->config.speed, impl->config.volume * 100.0f,
            100.0f + impl->config.pitch,
            actual_text);
    } else {
        snprintf(ssml_body, sizeof(ssml_body), "%s", actual_text);
    }

    LOG_DEBUG("tts", "Edge TTS: voice=%s, rate=%.2f", voice_id, impl->config.speed);

    iot_http_response_t http_resp;
    memset(&http_resp, 0, sizeof(http_resp));

    int ret = iot_http_post(iot_tts_get_api_url(IOT_TTS_PROVIDER_EDGE),
                        ssml_body, strlen(ssml_body),
                        "application/ssml+xml", &http_resp);

    memset(result, 0, sizeof(iot_tts_result_t));

    if (ret != 0) {
        result->error = iot_strdup(http_resp.error ? http_resp.error : "HTTP error");
        result->status_code = http_resp.status_code;
        iot_http_response_free(&http_resp);
        return -1;
    }

    result->status_code = http_resp.status_code;

    if (http_resp.status_code == 200 && http_resp.body && http_resp.body_len > 0) {
        result->audio_data = (uint8_t*)malloc(http_resp.body_len);
        if (result->audio_data) {
            memcpy(result->audio_data, http_resp.body, http_resp.body_len);
            result->audio_len = http_resp.body_len;
            result->format = iot_strdup("mp3");
            LOG_INFO("tts", "Edge TTS: %zu bytes", result->audio_len);
        }
    } else {
        result->error = iot_strdup(http_resp.body ? http_resp.body : "Empty response");
    }

    iot_http_response_free(&http_resp);
    return result->error ? -1 : 0;
}

/*===========================================================
 * 统一合成入口
 *===========================================================*/

static int iot_tts_do_synthesize(iot_tts_t* tts, const char* text, const char* ssml,
                              iot_tts_result_t* result)
{
    tts_impl_t* impl = (tts_impl_t*)tts;
    if (!tts || (!text && !ssml) || !result) return -1;

    memset(result, 0, sizeof(iot_tts_result_t));

    switch (impl->config.provider) {
    case IOT_TTS_PROVIDER_OPENAI:
        return iot_tts_openai_synthesize(tts, text, ssml, result);
    case IOT_TTS_PROVIDER_EDGE:
        return iot_tts_edge_synthesize(tts, text, ssml, result);
    case IOT_TTS_PROVIDER_BAIDU:
    case IOT_TTS_PROVIDER_IFLYTEK:
    case IOT_TTS_PROVIDER_ALIYUN:
    case IOT_TTS_PROVIDER_TENCENT:
        LOG_WARN("tts", "Provider %s: not yet implemented",
                 iot_tts_provider_name(impl->config.provider));
        result->error = iot_strdup("Provider not yet implemented");
        return -1;
    default:
        result->error = iot_strdup("Unknown provider");
        return -1;
    }
}

/*===========================================================
 * 公共 API - 生命周期
 *===========================================================*/

iot_tts_t* iot_tts_create(const iot_tts_config_t* config)
{
    if (!config) return NULL;

    tts_impl_t* impl = (tts_impl_t*)calloc(1, sizeof(tts_impl_t));
    if (!impl) return NULL;

    memcpy(&impl->config, config, sizeof(iot_tts_config_t));

    /* 默认值 */
    if (impl->config.speed <= 0.0f)  impl->config.speed = 1.0f;
    if (impl->config.volume <= 0.0f) impl->config.volume = 1.0f;
    if (impl->config.timeout_ms == 0) impl->config.timeout_ms = 15000;
    if (impl->config.language[0] == '\0')
        strncpy(impl->config.language, "zh-CN", sizeof(impl->config.language) - 1);

    impl->is_speaking = false;

    LOG_INFO("tts", "Created: provider=%s, voice=%d",
             iot_tts_provider_name(impl->config.provider),
             impl->config.voice);

    return (iot_tts_t*)impl;
}

int iot_tts_set_config(iot_tts_t* tts, const iot_tts_config_t* config)
{
    if (!tts || !config) return -1;
    tts_impl_t* impl = (tts_impl_t*)tts;
    memcpy(&impl->config, config, sizeof(iot_tts_config_t));
    return 0;
}

void iot_tts_free(iot_tts_t* tts)
{
    if (!tts) return;
    free(tts);
}

/*===========================================================
 * 公共 API - 同步合成
 *===========================================================*/

int iot_tts_synthesize(iot_tts_t* tts, const char* text, iot_tts_result_t* result)
{
    return iot_tts_do_synthesize(tts, text, NULL, result);
}

int iot_tts_synthesize_ssml(iot_tts_t* tts, const char* ssml, iot_tts_result_t* result)
{
    return iot_tts_do_synthesize(tts, NULL, ssml, result);
}

int iot_tts_synthesize_to_file(iot_tts_t* tts, const char* text, const char* file_path)
{
    if (!tts || !text || !file_path) return -1;

    iot_tts_result_t result;
    memset(&result, 0, sizeof(result));

    if (iot_tts_synthesize(tts, text, &result) != 0) {
        iot_tts_result_free(&result);
        return -1;
    }

    if (!result.audio_data) {
        iot_tts_result_free(&result);
        return -1;
    }

    FILE* fp = fopen(file_path, "wb");
    if (!fp) {
        LOG_ERROR("tts", "Cannot write: %s", file_path);
        iot_tts_result_free(&result);
        return -1;
    }

    fwrite(result.audio_data, 1, result.audio_len, fp);
    fclose(fp);

    LOG_INFO("tts", "Saved: %s (%zu bytes)", file_path, result.audio_len);
    iot_tts_result_free(&result);
    return 0;
}

/*===========================================================
 * 公共 API - 流式合成
 *===========================================================*/

int iot_tts_synthesize_stream(iot_tts_t* tts, const char* text,
                           void (*data_cb)(const uint8_t*, size_t, void*),
                           void (*done_cb)(bool, void*),
                           void* user_data)
{
    tts_impl_t* impl = (tts_impl_t*)tts;
    if (!tts || !text || !data_cb) return -1;

    impl->stream_data_cb = data_cb;
    impl->stream_done_cb = done_cb;
    impl->stream_ud = user_data;

    /*
     * TODO: 流式实现
     * 1. 设置请求 stream=true (如果 API 支持)
     * 2. 使用 chunked transfer 接收音频数据
     * 3. 每收到一个 chunk 调用 data_cb
     * 4. 完成后调用 done_cb
     *
     * 当前为回退实现: 先全部合成后再逐块回调
     */
    LOG_INFO("tts", "Stream synthesis (fallback mode)");

    iot_tts_result_t result;
    memset(&result, 0, sizeof(result));

    if (iot_tts_synthesize(tts, text, &result) != 0) {
        if (done_cb) done_cb(false, user_data);
        iot_tts_result_free(&result);
        return -1;
    }

    /* 模拟流式: 按 4096 字节分块发送 */
    const size_t chunk_size = 4096;
    for (size_t offset = 0; offset < result.audio_len; offset += chunk_size) {
        size_t remaining = result.audio_len - offset;
        size_t send_size = (remaining < chunk_size) ? remaining : chunk_size;
        data_cb(result.audio_data + offset, send_size, user_data);
    }

    if (done_cb) done_cb(true, user_data);

    iot_tts_result_free(&result);
    return 0;
}

/*===========================================================
 * 公共 API - 异步合成
 *===========================================================*/

int iot_tts_synthesize_async(iot_tts_t* tts, const char* text,
                          iot_tts_callback_t callback, void* user_data)
{
    if (!tts || !text || !callback) return -1;

    LOG_INFO("tts", "Async synthesis requested");

    /*
     * TODO: 后台线程合成
     * iot_task_create("tts_async", tts_async_task, ...);
     */
    LOG_WARN("tts", "Async not available, falling back to sync");
    iot_tts_result_t result;
    memset(&result, 0, sizeof(result));
    iot_tts_synthesize(tts, text, &result);
    callback(&result, user_data);
    iot_tts_result_free(&result);
    return 0;
}

/*===========================================================
 * 公共 API - 播放控制
 *
 * TODO: 需要平台音频播放 HAL:
 *   - iot_audio_init(sample_rate, channels, bits)
 *   - iot_audio_play(data, len)
 *   - iot_audio_stop()
 *===========================================================*/

int iot_tts_speak(iot_tts_t* tts, const char* text)
{
    if (!tts || !text) return -1;
    tts_impl_t* impl = (tts_impl_t*)tts;

    iot_tts_result_t result;
    memset(&result, 0, sizeof(result));

    if (iot_tts_synthesize(tts, text, &result) != 0) {
        iot_tts_result_free(&result);
        return -1;
    }

    int ret = iot_tts_play_audio(tts, result.audio_data, result.audio_len);
    iot_tts_result_free(&result);
    return ret;
}

int iot_tts_stop_speaking(iot_tts_t* tts)
{
    if (!tts) return -1;
    tts_impl_t* impl = (tts_impl_t*)tts;
    impl->is_speaking = false;
    /* TODO: iot_audio_stop(); */
    return 0;
}

bool iot_tts_is_speaking(iot_tts_t* tts)
{
    if (!tts) return false;
    return ((tts_impl_t*)tts)->is_speaking;
}

int iot_tts_play_audio(iot_tts_t* tts, const uint8_t* audio_data, size_t audio_len)
{
    (void)tts;
    if (!audio_data || audio_len == 0) return -1;

    LOG_INFO("tts", "Playing audio: %zu bytes", audio_len);

    /*
     * TODO: 平台音频播放实现
     *
     * ESP32:  I2S 输出到 MAX98357 / ES8388 / NS4168
     * Linux:  ALSA aplay 或 PulseAudio
     * Windows:WASAPI 或 WinMM PlaySound
     *
     * 简化示意:
     *   iot_audio_init(16000, 1, 16);
     *   iot_audio_play(audio_data, audio_len);
     */
    LOG_WARN("tts", "Audio playback requires platform HAL");

    return 0;
}

/*===========================================================
 * 公共 API - 辅助
 *===========================================================*/

void iot_tts_result_free(iot_tts_result_t* result)
{
    if (!result) return;
    free(result->audio_data);
    free(result->format);
    free(result->error);
    memset(result, 0, sizeof(iot_tts_result_t));
}

int iot_tts_text_to_ssml(const char* text, char* buf, size_t bufsize)
{
    if (!text || !buf || bufsize == 0) return -1;

    snprintf(buf, bufsize,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<speak version=\"1.0\" xmlns=\"http://www.w3.org/2001/10/synthesis\" "
        "xml:lang=\"zh-CN\">"
        "%s"
        "</speak>",
        text);

    return 0;
}
