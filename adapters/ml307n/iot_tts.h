/**
 * @file iot_tts.h
 * @brief ML307N 平台 TTS(语音合成)适配器头文件
 * @details 封装 ML307N 平台 cm_local_tts 接口，提供统一的TTS语音合成宏定义。
 *          支持初始化、播放文字、停止播放、设置参数等功能。
 */

#ifndef IOT_TTS_ML307N_H
#define IOT_TTS_ML307N_H

#include "../../iot_types.h"
#include "cm_local_tts.h"

/** @brief TTS参数类型枚举重定义 */
#define iot_tts_param_type_t    cm_tts_param_type_e

/** @brief TTS状态枚举重定义 */
#define iot_tts_state_t         cm_tts_state_e

/** @brief TTS回调函数类型重定义 */
#define iot_tts_callback_t      cm_tts_callback_t

/**
 * @brief 初始化TTS模块
 * @param[in] cb TTS状态回调函数指针
 * @return 0 成功，-1 失败
 */
#define iot_tts_init(cb) \
    cm_local_tts_init((cm_tts_callback_t)(cb))

/**
 * @brief 去初始化TTS模块
 * @return 0 成功，-1 失败
 */
#define iot_tts_deinit() \
    cm_local_tts_deinit()

/**
 * @brief 播放文字
 * @param[in] text 待播放的文字字符串（UTF-8编码）
 * @return 0 成功，-1 失败
 */
#define iot_tts_play(text) \
    cm_local_tts_play((char *)(text))

/**
 * @brief 停止播放
 * @return 0 成功，-1 失败
 */
#define iot_tts_stop() \
    cm_local_tts_stop()

/**
 * @brief 设置TTS参数
 * @param[in] type  参数类型枚举值
 * @param[in] value 参数值指针
 * @return 0 成功，-1 失败
 */
#define iot_tts_set_param(type, value) \
    cm_local_tts_set_param((cm_tts_param_type_e)(type), (void *)(value))

/**
 * @brief 获取TTS参数
 * @param[in]  type  参数类型枚举值
 * @param[out] value 参数值指针
 * @return 0 成功，-1 失败
 */
#define iot_tts_get_param(type, value) \
    cm_local_tts_get_param((cm_tts_param_type_e)(type), (void *)(value))

/**
 * @brief 获取TTS当前状态
 * @return TTS状态枚举值
 */
#define iot_tts_get_state() \
    cm_local_tts_get_state()

#endif /* IOT_TTS_ML307N_H */
