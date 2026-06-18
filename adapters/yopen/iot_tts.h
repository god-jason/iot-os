/**
 * @file iot_tts.h
 * @brief YOPEN 平台 TTS 语音合成适配器头文件
 * @details 基于 yopen_tts 接口实现 TTS 语音合成功能，
 *          支持语速、音调、音量设置，支持 UTF-8/GBK 编码。
 */

#ifndef IOT_TTS_YOPEN_H
#define IOT_TTS_YOPEN_H

#include "../../iot_types.h"

/* 引入 yopen 平台 TTS 头文件 */
#include "yopen_tts.h"

/* ============================================================
 * TTS 配置参数宏定义
 * ============================================================ */

/** @brief TTS速度设置范围 */
#define IOT_TTS_SPEED_MIN       YOPEN_TTS_SPEED_MIN
#define IOT_TTS_SPEED_NORMAL    YOPEN_TTS_SPEED_NORMAL
#define IOT_TTS_SPEED_MAX       YOPEN_TTS_SPEED_MAX

/** @brief TTS音调设置范围 */
#define IOT_TTS_PITCH_MIN       YOPEN_TTS_PITCH_MIN
#define IOT_TTS_PITCH_NORMAL    YOPEN_TTS_PITCH_NORMAL
#define IOT_TTS_PITCH_MAX       YOPEN_TTS_PITCH_MAX

/** @brief TTS音量设置范围 */
#define IOT_TTS_VOLUME_MIN      YOPEN_TTS_VOLUME_MIN
#define IOT_TTS_VOLUME_NORMAL   YOPEN_TTS_VOLUME_NORMAL
#define IOT_TTS_VOLUME_MAX      YOPEN_TTS_VOLUME_MAX

/** @brief TTS编码格式 */
typedef enum {
    IOT_TTS_CODEPAGE_ASCII      = YOPEN_TTS_CODEPAGE_ASCII,   /**< ASCII */
    IOT_TTS_CODEPAGE_GBK        = YOPEN_TTS_CODEPAGE_GBK,     /**< GBK */
    IOT_TTS_CODEPAGE_UTF8       = YOPEN_TTS_CODEPAGE_UTF8,    /**< UTF-8 */
    IOT_TTS_CODEPAGE_UNICODE    = YOPEN_TTS_CODEPAGE_UNICODE, /**< Unicode */
} iot_tts_codepage_t;

/** @brief TTS数字播报格式 */
typedef enum {
    IOT_TTS_READDIGIT_AUTO      = YOPEN_TTS_READDIGIT_AUTO,     /**< 自动 */
    IOT_TTS_READDIGIT_AS_NUMBER = YOPEN_TTS_READDIGIT_AS_NUMBER, /**< 按数字读 */
    IOT_TTS_READDIGIT_AS_VALUE  = YOPEN_TTS_READDIGIT_AS_VALUE,  /**< 按值读 */
} iot_tts_read_digit_t;

/** @brief TTS句柄类型 */
typedef yopen_tts_t iot_tts_handle_t;

/** @brief TTS播放输出回调类型 */
typedef void (*iot_tts_output_cb)(const uint8_t *data, uint16_t len, void *arg);

/** @brief TTS播放结束回调类型 */
typedef void (*iot_tts_play_end_cb)(iot_tts_state_t state, void *arg);

/* ============================================================
 * TTS 宏定义
 * ============================================================ */

/**
 * @brief 创建TTS引擎
 * @param[out] handle TTS句柄
 * @param[in] resource 资源文件（可为空）
 * @return 0 成功，<0 失败
 */
#define iot_tts_create(handle, resource) \
    yopen_tts_create((yopen_tts_t *)(handle), (resource))

/**
 * @brief 销毁TTS引擎
 * @param[in] handle TTS句柄
 * @return 0 成功，<0 失败
 */
#define iot_tts_destroy(handle) \
    yopen_tts_destory((yopen_tts_t)(handle))

/**
 * @brief 设置TTS语速
 * @param[in] handle TTS句柄
 * @param[in] speed 语速（范围-32768~32767）
 * @return 0 成功，<0 失败
 */
#define iot_tts_set_speed(handle, speed) \
    yopen_tts_set_config_param((yopen_tts_t)(handle), YOPEN_TTS_CONFIG_SPEED, (int)(speed))

/**
 * @brief 设置TTS音调
 * @param[in] handle TTS句柄
 * @param[in] pitch 音调（范围-32768~32767）
 * @return 0 成功，<0 失败
 */
#define iot_tts_set_pitch(handle, pitch) \
    yopen_tts_set_config_param((yopen_tts_t)(handle), YOPEN_TTS_CONFIG_VOICE_PITCH, (int)(pitch))

/**
 * @brief 设置TTS音量
 * @param[in] handle TTS句柄
 * @param[in] volume 音量（范围-32768~32767）
 * @return 0 成功，<0 失败
 */
#define iot_tts_set_volume(handle, volume) \
    yopen_tts_set_config_param((yopen_tts_t)(handle), YOPEN_TTS_CONFIG_VOLUME, (int)(volume))

/**
 * @brief 设置TTS编码格式
 * @param[in] handle TTS句柄
 * @param[in] codepage 编码格式
 * @return 0 成功，<0 失败
 */
#define iot_tts_set_codepage(handle, codepage) \
    yopen_tts_set_config_param((yopen_tts_t)(handle), YOPEN_TTS_CONFIG_CODEPAGE, (int)(codepage))

/**
 * @brief 设置TTS数字播报格式
 * @param[in] handle TTS句柄
 * @param[in] mode 数字播报模式
 * @return 0 成功，<0 失败
 */
#define iot_tts_set_read_digit(handle, mode) \
    yopen_tts_set_config_param((yopen_tts_t)(handle), YOPEN_TTS_CONFIG_READ_DIGIT, (int)(mode))

/**
 * @brief 获取TTS配置参数
 * @param[in] handle TTS句柄
 * @param[in] type 配置类型
 * @param[out] value 参数值
 * @return 0 成功，<0 失败
 */
#define iot_tts_get_config(handle, type, value) \
    yopen_tts_get_config_param((yopen_tts_t)(handle), (YOPEN_TTS_CONFIG_E)(type), (int *)(value))

/**
 * @brief 开始TTS播放
 * @param[in] handle TTS句柄
 * @param[in] text 文本内容
 * @param[in] len 文本长度
 * @param[in] output_cb PCM输出回调（可为NULL）
 * @param[in] end_cb 播放结束回调
 * @param[in] user_data 用户数据
 * @return 0 成功，<0 失败
 */
int iot_tts_start(iot_tts_handle_t handle, const char *text, uint32_t len,
                  iot_tts_output_cb output_cb, iot_tts_play_end_cb end_cb, void *user_data);

/**
 * @brief 停止TTS播放
 * @param[in] handle TTS句柄
 * @return 0 成功，<0 失败
 */
#define iot_tts_stop(handle) \
    yopen_tts_stop((yopen_tts_t)(handle))

/**
 * @brief 检查TTS是否正在播放
 * @param[in] handle TTS句柄
 * @return true 正在播放，false 未播放
 */
#define iot_tts_is_playing(handle) \
    yopen_tts_is_running((yopen_tts_t)(handle))

#endif /* IOT_TTS_YOPEN_H */
