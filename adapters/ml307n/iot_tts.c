/**
 * @file iot_tts.c
 * @brief ML307N 平台 TTS 适配器实现
 * @details 基于 cm_local_tts 接口实现 TTS 功能封装，
 *          提供统一的语音合成操作函数。
 */

#include "iot_tts.h"

/**
 * @brief 初始化TTS模块
 * @param[in] cb TTS状态回调函数指针
 * @return 0 成功，-1 失败
 */
int32_t iot_tts_init(iot_tts_callback_t cb)
{
    return cm_local_tts_init((cm_tts_callback_t)cb);
}

/**
 * @brief 去初始化TTS模块
 * @return 0 成功，-1 失败
 */
int32_t iot_tts_deinit(void)
{
    return cm_local_tts_deinit();
}

/**
 * @brief 播放文字
 * @param[in] text 待播放的文字字符串（UTF-8编码）
 * @return 0 成功，-1 失败
 */
int32_t iot_tts_play(const char *text)
{
    return cm_local_tts_play((char *)text);
}

/**
 * @brief 停止播放
 * @return 0 成功，-1 失败
 */
int32_t iot_tts_stop(void)
{
    return cm_local_tts_stop();
}

/**
 * @brief 设置TTS参数
 * @param[in] type  参数类型枚举值
 * @param[in] value 参数值指针
 * @return 0 成功，-1 失败
 */
int32_t iot_tts_set_param(iot_tts_param_type_t type, const void *value)
{
    return cm_local_tts_set_param((cm_tts_param_type_e)type, (void *)value);
}

/**
 * @brief 获取TTS参数
 * @param[in]  type  参数类型枚举值
 * @param[out] value 参数值指针
 * @return 0 成功，-1 失败
 */
int32_t iot_tts_get_param(iot_tts_param_type_t type, void *value)
{
    return cm_local_tts_get_param((cm_tts_param_type_e)type, value);
}

/**
 * @brief 获取TTS当前状态
 * @return TTS状态枚举值
 */
iot_tts_state_t iot_tts_get_state(void)
{
    return cm_local_tts_get_state();
}