/**
 * @file stt.c
 * @brief 语音识别实现
 *
 * 处理流程:
 *   1. 平台音频采集 (通过 audio HAL 接口)
 *   2. VAD 检测 (静音切除)
 *   3. 音频编码 (PCM -> WAV / MP3 / Opus)
 *   4. HTTP 提交到云端 STT API
 *   5. 解析识别结果
 *
 * 各服务商 API 端点:
 *   - OpenAI Whisper: POST /v1/audio/transcriptions
 *   - 百度:  POST https://vop.baidu.com/server_api
 *   - 讯飞:  WebSocket wss://iat-api.xfyun.cn/v2/iat
 *   - 阿里云: POST https://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/asr
 *   - 腾讯云: POST https://asr.tencentcloudapi.com
 *
 * 平台音频采集:
 *   - ESP32: I2S 接口 (INMP411 / MAX9814 / ES8311)
 *   - Linux: ALSA / PulseAudio
 *   - Windows: WASAPI / WinMM
 *
 * @author 杰神 & CodeBuddy
 * @date   2026.08.01
 */
#include "stt.h"
#include "http_client.h"
#include "http_form.h"
#include "iot_log.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*===========================================================
 * 常量
 *===========================================================*/

#define STT_JSON_BUF_SIZE   4096
#define STT_RESP_BUF_SIZE   8192
#define STT_AUDIO_BUF_SIZE  (16000 * 2 * 60)  /* 16kHz, 16bit, 1ch, 60s */

/* VAD 默认参数 */
#define STT_VAD_FRAME_SIZE    512     /* 帧大小 (采样点) */
#define STT_VAD_ENERGY_THRESHOLD 500  /* 能量阈值 */
#define STT_VAD_MIN_SPEECH_MS 200    /* 最短有效语音 */
#define STT_VAD_MAX_SILENCE_MS 800   /* 最长静音 (触发断句) */

/*===========================================================
 * 内部 JSON 构建 (轻量, 与 llm.c 中的类似) */
/*===========================================================*/

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
 * STT 内部结构
 *===========================================================*/

typedef struct {
    stt_config_t config;
    bool         recording;
    bool         continuous_listen;

    uint8_t*     audio_buffer;
    size_t       audio_len;
    size_t       audio_capacity;

    stt_vad_callback_t   vad_cb;
    void*                vad_ud;
    stt_audio_callback_t audio_cb;
    void*                audio_ud;

    char         session_id[64];   /* 连续识别会话 ID */
    char         accumulate_text[2048]; /* 累积识别文本 */
} stt_impl_t;

/*===========================================================
 * 提供者名称
 *===========================================================*/

const char* stt_provider_name(stt_provider_t provider)
{
    static const char* names[] = {
        "auto", "openai", "baidu", "iflytek", "aliyun", "tencent"
    };
    if (provider > STT_PROVIDER_TENCENT) return "unknown";
    return names[provider];
}

/*===========================================================
 * API URL 构建
 *===========================================================*/

static const char* stt_get_api_url(stt_provider_t provider)
{
    switch (provider) {
    case STT_PROVIDER_OPENAI:
        return "https://api.openai.com/v1/audio/transcriptions";
    case STT_PROVIDER_BAIDU:
        return "https://vop.baidu.com/server_api";
    case STT_PROVIDER_ALIYUN:
        return "https://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/asr";
    case STT_PROVIDER_TENCENT:
        return "https://asr.tencentcloudapi.com";
    case STT_PROVIDER_IFLYTEK:
        /* 讯飞使用 WebSocket, 需要特殊处理 */
        return "wss://iat-api.xfyun.cn/v2/iat";
    default:
        return NULL;
    }
}

/*===========================================================
 * 文件读取
 *===========================================================*/

static int stt_read_file(const char* path, uint8_t** out_data, size_t* out_size)
{
    if (!path || !out_data || !out_size) return -1;

    FILE* fp = fopen(path, "rb");
    if (!fp) {
        LOG_ERROR("stt", "Cannot open: %s", path);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size <= 0) { fclose(fp); return -1; }
    fseek(fp, 0, SEEK_SET);

    uint8_t* data = (uint8_t*)malloc((size_t)size);
    if (!data) { fclose(fp); return -1; }

    size_t read = fread(data, 1, (size_t)size, fp);
    fclose(fp);

    if (read != (size_t)size) { free(data); return -1; }

    *out_data = data;
    *out_size = (size_t)size;
    return 0;
}

/*===========================================================
 * OpenAI Whisper 识别实现
 *===========================================================*/

static int stt_openai_recognize(stt_t* stt, const uint8_t* audio, size_t len,
                                 stt_result_t* result)
{
    stt_impl_t* impl = (stt_impl_t*)stt;

    /* 构建 multipart/form-data body */
    /* 手动拼接 (简化) */

    char boundary[64];
    snprintf(boundary, sizeof(boundary), "----IoTOS-STT-%06x", rand() & 0xFFFFFF);

    /* model=whisper-1 */
    char model_part[256];
    int model_len = snprintf(model_part, sizeof(model_part),
        "--%s\r\n"
        "Content-Disposition: form-data; name=\"model\"\r\n\r\n"
        "whisper-1\r\n", boundary);

    /* language */
    char lang_part[256] = {0};
    int lang_len = 0;
    if (impl->config.language[0]) {
        lang_len = snprintf(lang_part, sizeof(lang_part),
            "--%s\r\n"
            "Content-Disposition: form-data; name=\"language\"\r\n\r\n"
            "%s\r\n", boundary, impl->config.language);
    }

    /* file */
    char file_header[512];
    int header_len = snprintf(file_header, sizeof(file_header),
        "--%s\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n"
        "Content-Type: audio/wav\r\n\r\n", boundary);

    char file_footer[128];
    int footer_len = snprintf(file_footer, sizeof(file_footer),
        "\r\n--%s--\r\n", boundary);

    /* 组装完整 body */
    size_t total = model_len + lang_len + header_len + len + footer_len;
    uint8_t* body = (uint8_t*)malloc(total);
    if (!body) { result->error = iot_strdup("OOM"); return -1; }

    size_t offset = 0;
    memcpy(body + offset, model_part, model_len); offset += model_len;
    if (lang_len > 0) {
        memcpy(body + offset, lang_part, lang_len); offset += lang_len;
    }
    memcpy(body + offset, file_header, header_len); offset += header_len;
    memcpy(body + offset, audio, len);               offset += len;
    memcpy(body + offset, file_footer, footer_len);  offset += footer_len;

    /* 构建 Content-Type header */
    char content_type[128];
    snprintf(content_type, sizeof(content_type),
             "multipart/form-data; boundary=%s", boundary);

    /* URL 可能带查询参数 */
    char url[512];
    snprintf(url, sizeof(url), "%s", stt_get_api_url(STT_PROVIDER_OPENAI));

    /* 构建请求头 */
    char headers[512];
    snprintf(headers, sizeof(headers),
             "Authorization: Bearer %s\r\n", impl->config.api_key);

    http_response_t http_resp;
    memset(&http_resp, 0, sizeof(http_resp));

    int ret = http_post(url, (const char*)body, total,
                        content_type, &http_resp);
    free(body);

    memset(result, 0, sizeof(stt_result_t));

    if (ret != 0 || http_resp.error) {
        result->error = iot_strdup(http_resp.error ? http_resp.error : "HTTP error");
        result->status_code = http_resp.status_code;
        http_response_free(&http_resp);
        return -1;
    }

    result->status_code = http_resp.status_code;

    if (http_resp.body) {
        result->text = json_extract_string(http_resp.body, "text");
        result->raw_response = iot_strdup(http_resp.body);
    }

    /* Whisper API 不直接返回置信度 */
    result->confidence = 0.95f;
    http_response_free(&http_resp);

    return result->text ? 0 : -1;
}

/*===========================================================
 * 百度语音识别实现
 *===========================================================*/

static int stt_baidu_recognize(stt_t* stt, const uint8_t* audio, size_t len,
                                stt_result_t* result)
{
    stt_impl_t* impl = (stt_impl_t*)stt;

    /* 百度需要先获取 access_token */
    char token_url[512];
    snprintf(token_url, sizeof(token_url),
             "https://aip.baidubce.com/oauth/2.0/token?"
             "grant_type=client_credentials&"
             "client_id=%s&client_secret=%s",
             impl->config.api_key, impl->config.api_secret);

    /* 构建 JSON body */
    char json_body[STT_JSON_BUF_SIZE];
    int body_len = snprintf(json_body, sizeof(json_body),
        "{"
        "\"format\":\"pcm\","
        "\"rate\":%d,"
        "\"channel\":%d,"
        "\"cuid\":\"iot-device\","
        "\"token\":\"TODO_token\","
        "\"speech\":\"TODO_base64\","
        "\"len\":%zu,"
        "\"dev_pid\":%s"
        "}",
        impl->config.sample_rate,
        impl->config.channels,
        len,
        (strcmp(impl->config.language, "en") == 0) ? "1737" : "1537"
    );

    memset(result, 0, sizeof(stt_result_t));

    /*
     * TODO: 完整的百度 STT 实现需要:
     *   1. 先获取 access_token
     *   2. 将音频 base64 编码后填入
     *   3. POST 到 https://vop.baidu.com/server_api
     *   4. 解析响应中的 result JSON
     */
    LOG_WARN("stt", "Baidu STT: full implementation requires token + base64");
    result->error = iot_strdup("Baidu STT not fully implemented");
    return -1;
}

/*===========================================================
 * 讯飞语音识别实现
 *===========================================================*/

static int stt_iflytek_recognize(stt_t* stt, const uint8_t* audio, size_t len,
                                  stt_result_t* result)
{
    memset(result, 0, sizeof(stt_result_t));

    /*
     * TODO: 讯飞使用 WebSocket 协议，需要:
     *   1. 建立 WebSocket 连接
     *   2. 发送鉴权帧
     *   3. 分帧发送音频数据
     *   4. 接收识别结果 (流式)
     * 实现较为复杂，建议在 Lua 层通过 websocket 模块完成。
     */
    LOG_WARN("stt", "iFlytek STT: requires WebSocket support");
    result->error = iot_strdup("iFlytek STT not fully implemented");
    return -1;
}

/*===========================================================
 * 统一识别入口
 *===========================================================*/

static int stt_do_recognize(stt_t* stt, const uint8_t* audio, size_t len,
                              stt_result_t* result)
{
    stt_impl_t* impl = (stt_impl_t*)stt;
    if (!stt || !audio || !result) return -1;

    memset(result, 0, sizeof(stt_result_t));

    switch (impl->config.provider) {
    case STT_PROVIDER_OPENAI:
        return stt_openai_recognize(stt, audio, len, result);
    case STT_PROVIDER_BAIDU:
        return stt_baidu_recognize(stt, audio, len, result);
    case STT_PROVIDER_IFLYTEK:
        return stt_iflytek_recognize(stt, audio, len, result);
    case STT_PROVIDER_ALIYUN:
    case STT_PROVIDER_TENCENT:
        /* 类似实现, 参考 OpenAI 的 multipart 上传方式 */
        LOG_WARN("stt", "Provider %s: not yet implemented",
                 stt_provider_name(impl->config.provider));
        result->error = iot_strdup("Provider not yet implemented");
        return -1;
    default:
        result->error = iot_strdup("Unknown STT provider");
        return -1;
    }
}

/*===========================================================
 * 公共 API - 生命周期
 *===========================================================*/

stt_t* stt_create(const stt_config_t* config)
{
    if (!config) return NULL;

    stt_impl_t* impl = (stt_impl_t*)calloc(1, sizeof(stt_impl_t));
    if (!impl) return NULL;

    memcpy(&impl->config, config, sizeof(stt_config_t));

    /* 默认值 */
    if (impl->config.sample_rate == 0)     impl->config.sample_rate = 16000;
    if (impl->config.channels == 0)        impl->config.channels = 1;
    if (impl->config.bits_per_sample == 0) impl->config.bits_per_sample = 16;
    if (impl->config.timeout_ms == 0)      impl->config.timeout_ms = 10000;
    if (impl->config.language[0] == '\0')  strncpy(impl->config.language, "zh", sizeof(impl->config.language) - 1);

    /* 预分配音频缓冲区 */
    impl->audio_capacity = STT_AUDIO_BUF_SIZE;
    impl->audio_buffer = (uint8_t*)malloc(impl->audio_capacity);
    impl->audio_len = 0;

    snprintf(impl->session_id, sizeof(impl->session_id),
             "stt-%06d", rand() % 1000000);

    LOG_INFO("stt", "Created: provider=%s, rate=%d, lang=%s",
             stt_provider_name(impl->config.provider),
             impl->config.sample_rate, impl->config.language);

    return (stt_t*)impl;
}

int stt_set_config(stt_t* stt, const stt_config_t* config)
{
    if (!stt || !config) return -1;
    stt_impl_t* impl = (stt_impl_t*)stt;
    memcpy(&impl->config, config, sizeof(stt_config_t));
    return 0;
}

void stt_free(stt_t* stt)
{
    if (!stt) return;
    stt_impl_t* impl = (stt_impl_t*)stt;
    stt_stop_recording(stt);
    stt_stop_continuous_listen(stt);
    free(impl->audio_buffer);
    free(impl);
}

/*===========================================================
 * 公共 API - 同步识别
 *===========================================================*/

int stt_recognize_file(stt_t* stt, const char* file_path,
                        stt_result_t* result)
{
    if (!stt || !file_path || !result) return -1;

    uint8_t* audio = NULL;
    size_t   audio_len = 0;

    if (stt_read_file(file_path, &audio, &audio_len) != 0) {
        result->error = iot_strdup("Failed to read audio file");
        return -1;
    }

    /* 如果是 WAV 文件, 跳过 44 字节 WAV 头 */
    const char* ext = strrchr(file_path, '.');
    if (ext && strcasecmp(ext, ".wav") == 0) {
        if (audio_len > 44) {
            /* 提取 PCM 数据 (跳过 WAV 头) */
            audio     += 44;
            audio_len -= 44;
        }
    }

    int ret = stt_do_recognize(stt, audio, audio_len, result);

    /* 恢复指针后释放 */
    if (ext && strcasecmp(ext, ".wav") == 0) {
        free(audio - 44);
    } else {
        free(audio);
    }

    return ret;
}

int stt_recognize_buffer(stt_t* stt, const uint8_t* audio_data,
                          size_t audio_len, stt_result_t* result)
{
    if (!stt || !audio_data || !result) return -1;
    return stt_do_recognize(stt, audio_data, audio_len, result);
}

/*===========================================================
 * 公共 API - 实时录音识别
 *
 * TODO: 需要平台音频采集 HAL 支持:
 *   - iot_audio_init(sample_rate, channels, bits)
 *   - iot_audio_start_read(buffer, size, callback)
 *   - iot_audio_stop()
 *   完成后在回调中上报识别结果
 *===========================================================*/

int stt_start_record_and_recognize(stt_t* stt, stt_callback_t callback,
                                    void* user_data)
{
    stt_impl_t* impl = (stt_impl_t*)stt;
    if (!stt || !callback) return -1;

    if (impl->recording) {
        LOG_WARN("stt", "Already recording");
        return -1;
    }

    impl->recording = true;
    impl->audio_len = 0;

    LOG_INFO("stt", "Recording started (session=%s)", impl->session_id);

    /*
     * TODO: 实际实现
     * 1. 初始化音频采集硬件
     * 2. 启动 DMA / 中断传输
     * 3. 在回调中:
     *    a. VAD 检测语音活动
     *    b. 存入 audio_buffer
     *    c. 检测到语音结束后调用 stt_do_recognize
     *    d. 通过 callback 返回结果
     */
    LOG_WARN("stt", "Audio recording requires platform HAL");
    return 0;
}

int stt_stop_recording(stt_t* stt)
{
    if (!stt) return -1;
    stt_impl_t* impl = (stt_impl_t*)stt;

    impl->recording = false;
    LOG_INFO("stt", "Recording stopped");
    return 0;
}

bool stt_is_recording(stt_t* stt)
{
    if (!stt) return false;
    return ((stt_impl_t*)stt)->recording;
}

void stt_set_vad_callback(stt_t* stt, stt_vad_callback_t vad_cb,
                           void* user_data)
{
    if (!stt) return;
    stt_impl_t* impl = (stt_impl_t*)stt;
    impl->vad_cb = vad_cb;
    impl->vad_ud = user_data;
}

void stt_set_audio_callback(stt_t* stt, stt_audio_callback_t audio_cb,
                             void* user_data)
{
    if (!stt) return;
    stt_impl_t* impl = (stt_impl_t*)stt;
    impl->audio_cb = audio_cb;
    impl->audio_ud = user_data;
}

/*===========================================================
 * 公共 API - 连续识别
 *===========================================================*/

int stt_set_wake_word(stt_t* stt, const char* wake_word)
{
    if (!stt || !wake_word) return -1;

    /*
     * TODO: 唤醒词检测通常使用端侧关键词检测模型 (KWS)
     * 类似 TensorFlow Lite Micro 的 micro_speech 示例
     * 需要:
     *   1. 训练/下载唤醒词模型 (如 "小杰小杰")
     *   2. 在音频流中实时检测
     *   3. 检测到唤醒词后触发后续识别流程
     */
    LOG_INFO("stt", "Wake word set: '%s' (requires KWS model)", wake_word);
    return 0;
}

int stt_start_continuous_listen(stt_t* stt, stt_callback_t callback,
                                 void* user_data)
{
    stt_impl_t* impl = (stt_impl_t*)stt;
    if (!stt || !callback) return -1;

    impl->continuous_listen = true;
    impl->recording = true;
    LOG_INFO("stt", "Continuous listening started");
    /* TODO: 实现连续监听循环 */
    return 0;
}

int stt_stop_continuous_listen(stt_t* stt)
{
    if (!stt) return -1;
    stt_impl_t* impl = (stt_impl_t*)stt;
    impl->continuous_listen = false;
    return 0;
}

/*===========================================================
 * 公共 API - 辅助
 *===========================================================*/

void stt_result_free(stt_result_t* result)
{
    if (!result) return;
    free(result->text);
    free(result->language);
    free(result->error);
    free(result->raw_response);
    memset(result, 0, sizeof(stt_result_t));
}

int stt_pcm_to_wav(const uint8_t* pcm_data, size_t pcm_size,
                    int sample_rate, int channels, int bits_per_sample,
                    uint8_t** wav_data, size_t* wav_size)
{
    if (!pcm_data || !wav_data || !wav_size) return -1;

    uint32_t data_size = (uint32_t)pcm_size;
    uint32_t header_size = 44;
    uint32_t total_size = header_size + data_size;
    uint16_t block_align = (uint16_t)(channels * (bits_per_sample / 8));
    uint32_t byte_rate = (uint32_t)(sample_rate * block_align);

    uint8_t* buf = (uint8_t*)calloc(1, total_size);
    if (!buf) return -1;

    /* RIFF header */
    memcpy(buf,      "RIFF", 4);
    buf[4]  = (total_size - 8) & 0xFF;
    buf[5]  = ((total_size - 8) >> 8) & 0xFF;
    buf[6]  = ((total_size - 8) >> 16) & 0xFF;
    buf[7]  = ((total_size - 8) >> 24) & 0xFF;
    memcpy(buf + 8,  "WAVE", 4);

    /* fmt chunk */
    memcpy(buf + 12, "fmt ", 4);
    buf[16] = 16; buf[17] = 0; buf[18] = 0; buf[19] = 0;     /* subchunk1 size = 16 */
    buf[20] = 1;  buf[21] = 0;                                /* audio format = PCM */
    buf[22] = channels & 0xFF;   buf[23] = (channels >> 8) & 0xFF;
    buf[24] = sample_rate & 0xFF; buf[25] = (sample_rate >> 8) & 0xFF;
    buf[26] = (sample_rate >> 16) & 0xFF; buf[27] = (sample_rate >> 24) & 0xFF;
    buf[28] = byte_rate & 0xFF;  buf[29] = (byte_rate >> 8) & 0xFF;
    buf[30] = (byte_rate >> 16) & 0xFF; buf[31] = (byte_rate >> 24) & 0xFF;
    buf[32] = block_align & 0xFF; buf[33] = (block_align >> 8) & 0xFF;
    buf[34] = bits_per_sample & 0xFF; buf[35] = (bits_per_sample >> 8) & 0xFF;

    /* data chunk */
    memcpy(buf + 36, "data", 4);
    buf[40] = data_size & 0xFF; buf[41] = (data_size >> 8) & 0xFF;
    buf[42] = (data_size >> 16) & 0xFF; buf[43] = (data_size >> 24) & 0xFF;

    /* PCM 数据 */
    memcpy(buf + 44, pcm_data, pcm_size);

    *wav_data = buf;
    *wav_size = total_size;
    return 0;
}

bool stt_vad_detect(const int16_t* pcm_data, size_t len, int threshold)
{
    if (!pcm_data || len == 0) return false;
    if (threshold <= 0) threshold = STT_VAD_ENERGY_THRESHOLD;

    double energy = 0.0;
    size_t samples = len;

    for (size_t i = 0; i < samples; i++) {
        energy += (double)pcm_data[i] * (double)pcm_data[i];
    }
    energy /= (double)samples;

    return energy > (double)threshold;
}
