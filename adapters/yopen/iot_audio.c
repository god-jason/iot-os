/**
 * @file iot_audio.c
 * @brief YOPEN 平台音频播放与录音适配器实现
 * @details 基于 yopen_pwmaud 接口实现音频播放功能封装，
 *          支持枚举类型参数转换。
 */

#include "iot_audio.h"

/* ============================================================
 * 音频播放操作函数实现
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
iot_err_t iot_audio_play(const void *data, uint32_t size, iot_audio_format_t format,
                         const iot_audio_sample_param_t *sample_param,
                         iot_audio_callback_t cb, void *cb_param)
{
    return yopen_audio_play(data, size, (yopen_audio_format_e)format, 
                           (yopen_audio_sample_param_s *)sample_param, 
                           (yopen_audio_callback_t)cb, cb_param);
}

/**
 * @brief 播放音频文件
 * @param[in] path         文件路径
 * @param[in] format       音频格式
 * @param[in] sample_param PCM采样参数
 * @param[in] cb           播放完成回调函数
 * @param[in] cb_param     回调用户参数
 * @return 0 成功，-1 失败
 */
iot_err_t iot_audio_play_file(const char *path, iot_audio_format_t format,
                              const iot_audio_sample_param_t *sample_param,
                              iot_audio_callback_t cb, void *cb_param)
{
    return yopen_audio_play_file(path, (yopen_audio_format_e)format, 
                                (yopen_audio_sample_param_s *)sample_param, 
                                (yopen_audio_callback_t)cb, cb_param);
}

/**
 * @brief 暂停播放
 * @return 0 成功，-1 失败
 */
iot_err_t iot_audio_play_pause(void)
{
    return yopen_audio_pause();
}

/**
 * @brief 继续播放
 * @return 0 成功，-1 失败
 */
iot_err_t iot_audio_play_resume(void)
{
    return yopen_audio_resume();
}

/**
 * @brief 停止播放
 * @return 0 成功，-1 失败
 */
iot_err_t iot_audio_play_stop(void)
{
    return yopen_audio_stop();
}

/**
 * @brief 设置播放音量
 * @param[in] vol 音量值 0-100（10的倍数）
 * @return 0 成功，其他失败
 */
iot_err_t iot_audio_play_set_volume(int vol)
{
    return yopen_audio_set_volume(vol);
}

/**
 * @brief 获取播放音量
 * @param[out] vol 音量值指针
 * @return 0 成功，其他失败
 */
iot_err_t iot_audio_play_get_volume(int *vol)
{
    return yopen_audio_get_volume(vol);
}

/* ============================================================
 * 流播放操作函数实现
 * ============================================================ */

/**
 * @brief 打开流播放通道
 * @param[in] format       音频格式
 * @param[in] sample_param PCM采样参数
 * @return 0 成功，-1 失败
 */
iot_err_t iot_audio_stream_open(iot_audio_format_t format, const iot_audio_sample_param_t *sample_param)
{
    return yopen_audio_stream_open((yopen_audio_format_e)format, 
                                  (yopen_audio_sample_param_s *)sample_param);
}

/**
 * @brief 往流播放通道推送音频数据
 * @param[in] data 音频数据指针
 * @param[in] size 数据长度
 * @return >=0 实际写入长度，-1 失败
 */
int iot_audio_stream_push(const void *data, uint32_t size)
{
    return yopen_audio_stream_push((uint8_t *)data, size);
}

/**
 * @brief 关闭流播放通道
 * @return 0 成功
 */
iot_err_t iot_audio_stream_close(void)
{
    yopen_audio_stream_close();
    return IOT_OK;
}

/* ============================================================
 * 录音操作函数实现
 * ============================================================ */

/**
 * @brief 开始录音
 * @param[in] format       录音音频格式
 * @param[in] sample_param PCM采样参数
 * @param[in] cb           录音数据回调函数
 * @param[in] cb_param     回调用户参数
 * @return 0 成功，-1 失败
 */
iot_err_t iot_audio_record_start(iot_audio_format_t format, const iot_audio_sample_param_t *sample_param,
                                 iot_audio_record_callback_t cb, void *cb_param)
{
    return yopen_audio_record_start((yopen_audio_format_e)format, 
                                   (yopen_audio_sample_param_s *)sample_param, 
                                   (yopen_audio_record_callback_t)cb, cb_param);
}

/**
 * @brief 停止录音
 * @return 0 成功
 */
iot_err_t iot_audio_record_stop(void)
{
    yopen_audio_record_stop();
    return IOT_OK;
}