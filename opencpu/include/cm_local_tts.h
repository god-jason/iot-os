/**
 * @file        cm_local_tts.h
 * @brief       TTS接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By WP0201
 * @date        2021/04/12
 *
 * @defgroup tts tts
 * @ingroup AUDIO
 * @{
 */

#ifndef __CM_LOCAL_TTS_H__
#define __CM_LOCAL_TTS_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "os.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/
/**
 * @brief 中文文本编码方式
 *
 * @details 范围根据实际情况确定, 目前仅支持 UTF-8，不支持 GB2312
 */
typedef enum
{
    CM_LOCAL_TTS_ENCODE_TYPE_GB2312 = 1,      /*!< 普通字符为ASCII格式，汉字为GBK格式 */
    CM_LOCAL_TTS_ENCODE_TYPE_UTF8,            /*!< 普通字符为ASCII格式，汉字为UTF-8格式 */
} cm_local_tts_encode_e;

/**
 * @brief 数字播放方式
 */
typedef enum
{
    CM_LOCAL_TTS_DIGIT_AUTO = 0,              /*!< 自动模式 */
    CM_LOCAL_TTS_DIGIT_AS_NUMBER,             /*!< 数字模式 */
    CM_LOCAL_TTS_DIGIT_AS_VALUE,              /*!< 数值模式 */
} cm_local_tts_digit_e;

/**
 * @brief 音调
 *
 * @details 范围根据实际情况确定
 */
typedef enum
{
    CM_LOCAL_TTS_TONE_NORMAL = 1,             /*!< 默认音调 */
} cm_local_tts_tone_e;

/**
 * @brief 音效
 *
 * @details 范围根据实际情况确定
 */
typedef enum
{
    CM_LOCAL_TTS_EFFECT_NORMAL = 1,           /*!< 默认音效 */
} cm_local_tts_effect_e;

/**
 * @brief TTS状态
 *
 */
typedef enum
{
    CM_LOCAL_TTS_STATE_IDLE = 1,              /*!< TTS空闲 */
    CM_LOCAL_TTS_STATE_WORKING,               /*!< TTS工作中 */
} cm_local_tts_state_e;

/** 音频播放设置类型 */
typedef struct
{
    int32_t speed;                      /*!< 语速，范围1~9 */
    int32_t volume;                     /*!< TTS转化时音量，范围1~9 */
    cm_local_tts_encode_e encode;       /*!< 中文文本编码方式 */
    cm_local_tts_digit_e digit;         /*!< 数字播放模式 */
    cm_local_tts_tone_e tone;           /*!< 音调，暂不支持 */
    cm_local_tts_effect_e effect;       /*!< 音效，暂不支持 */
} cm_local_tts_cfg_t;

/** TTS回调的事件类型 */
typedef enum
{
    CM_LOCAL_TTS_EVENT_SYNTH_DATA = 1,        /*!< 语音合成数据上报，调用cm_tts_synth()接口时可能上报的事件 */
    CM_LOCAL_TTS_EVENT_SYNTH_FAIL,            /*!< 语音合成失败，调用cm_tts_synth()接口时可能上报的事件 */
    CM_LOCAL_TTS_EVENT_SYNTH_INTERRUPT,       /*!< 语音合成（异常）中断，调用cm_tts_synth()接口时可能上报的事件 */
    CM_LOCAL_TTS_EVENT_SYNTH_FINISH,          /*!< 语音合成完成，调用cm_tts_synth()接口时可能上报的事件 */
    CM_LOCAL_TTS_EVENT_PLAY_FAIL,             /*!< 语音播放失败，调用cm_tts_play()接口时可能上报的事件 */
    CM_LOCAL_TTS_EVENT_PLAY_INTERRUPT,        /*!< 语音播放中断，调用cm_tts_play()接口时可能上报的事件 */
    CM_LOCAL_TTS_EVENT_PLAY_FINISH,           /*!< 语音播放完成，调用cm_tts_play()接口时可能上报的事件 */
} cm_local_tts_event_e;

/**
 * @brief TTS转码回调函数
 *
 * @param [in] event 事件类型
 * @param [in] param 事件参数（事件类型为CM_TTS_EVENT_SYNTH_DATA时，需要将param强转为cm_tts_synth_data_t型。其余情况该参数为用户传入参数）
 */
typedef void (*cm_local_tts_callback)(cm_local_tts_event_e event, void *param);

/**
 * @brief TTS初始化
 *
 * @param [in] cfg  配置参数指针
 *
 * @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 */
int32_t cm_local_tts_init(cm_local_tts_cfg_t *cfg);

/**
 * @brief TTS去初始化
 *
 */
void cm_local_tts_deinit(void);

/**
 * @brief TTS播放文字
 *
 * @param [in] text     将要播放的文字
 * @param [in] len      text的字符串长度，当等于-1时，函数内部自动判断
 * @param [in] cb       TTS回调函数（回调函数不能阻塞），暂不支持，该参数应传NULL
 * @param [in] cb_param 用户参数（参见cm_tts_callback回调函数中param参数描述），暂不支持，该参数应传NULL
 *
 * @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 * @details 异步函数，播放任务建立后，此函数会立即返回
 */
int32_t cm_local_tts_play(const char *text, int32_t len, cm_local_tts_callback cb, void *cb_param);

/**
 * @brief 停止TTS播放文字
 *
 * @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 */
int32_t cm_local_tts_play_stop(void);

/**
 * @brief 获取TTS状态
 *
 * @return
 *   当前TTS状态
 */
cm_local_tts_state_e cm_local_tts_get_state(void);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_LOCAL_TTS_H__ */

/** @}*/
