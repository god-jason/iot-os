/**
 * @file        cm_audio_recorder.h
 * @brief       Audio recorder接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By WP0201
 * @date        2021/4/12
 *
 * @defgroup audio_recorder
 * @ingroup audio_recorder
 * @{
 */

#ifndef __OPENCPU_AUDIO_RECORDER_H__
#define __OPENCPU_AUDIO_RECORDER_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "cm_audio_common.h"

//录音功能裁剪开关 关闭此宏定义将不支持录音功能
#define CM_AUDIO_RECORDER_ENABLE

#ifdef CM_AUDIO_RECORDER_ENABLE
/*录音编码器开关
放开宏定义，将对应格式编码器加入编译
*/
#define CM_AUDIO_RECORDER_AMR_ENABLE

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Public Types
 ****************************************************************************/

/** 录音通道支持 */
typedef enum
{
    CM_AUDIO_RECORD_CHANNEL_MAIN = 1,           /*!< 主MIC通道，不支持设置 */
    CM_AUDIO_RECORD_CHANNEL_HP,                 /*!< 耳机MIC通道，不支持设置 */
#if 0
    CM_AUDIO_RECORD_CHANNEL_REMOTE,             /*!< 从远端通话中录音（需建立通话） */
#endif
} cm_audio_record_channel_e;

/** 录音回调的事件类型 */
typedef enum
{
    CM_AUDIO_RECORD_EVENT_DATA = 1,             /*!< 录音数据上报 */
    CM_AUDIO_RECORD_EVENT_FINISHED,             /*!< 录音结束 */
    CM_AUDIO_RECORD_EVENT_INTERRUPT,            /*!< 录音中断 */
} cm_audio_record_event_e;

/** 录音设置类型 */
typedef enum
{
    CM_AUDIO_RECORD_CFG_CHANNEL = 1,            /*!< 录音通道，支持范围参见cm_audio_record_channel_e枚举量，不支持设置 */
    CM_AUDIO_RECORD_CFG_GAIN,                   /*!< 录音增益，不支持设置 */
} cm_audio_record_cfg_type_e;

/*
 * 录音数据结构体
 */
typedef struct
{
    uint8_t *data;                              /*!< 录音数据 */
    uint32_t len;                               /*!< 录音数据长度 */
    void *user;                                 /*!< 用户传入参数 */
    cm_audio_record_format_e fmt;
} cm_audio_record_data_t;

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif


/**
 * @brief 录音回调函数
 *
 * @param [in] event 事件类型
 * @param [in] param 事件参数（事件类型为CM_AUDIO_RECORD_DATA时，需要将param强转为cm_audio_record_data_t型。其余情况该参数为用户传入参数）
 *
 * @details  须在录音API中传入 \n
 *           通知事件为中断触发，不可阻塞，不可做耗时较长的操作，例如不可使用UART打印log
 */
typedef void (*cm_audio_record_callback_t)(cm_audio_record_event_e event, void *param);

/**
 *  @brief 开始录音
 *
 *  @param [in] format       录制音频格式
 *  @param [in] sample_param 录制音频PCM采样参数
 *  @param [in] cb           录音回调函数（回调函数不能阻塞）
 *  @param [in] cb_param     用户参数（参见cm_audio_record_callback回调函数中param参数描述，长度最大为8）
 *
 *  @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 *  @details 回调函数不能阻塞，支持PCM格式\n
 *           通知事件为中断触发，不可阻塞，不可做耗时较长的操作，例如不可使用UART打印log \n
 *
 *           ML307N仅用户外挂音频设备完成录音并通过PCM接口将录音PCM数据发送至模组方案支持录音功能，PWM接口输出音频模拟信号方案不支持录音 \n
 *           该方案有如下特性： \n
 *           1.模组自身无录制音频能力，录音相关API执行效果为通过硬件PCM口接收用户外挂音频设备发送的录音PCM数据并将数据通过用户注册的回调函数回调至用户应用程序 \n
 *           2.录制音频操作由用户外挂音频设备完成，音效校准、降噪等由用户外挂音频设备实现 \n
 *           3.如用户外挂音频设备需要驱动才可使用，可注册用户外挂音频设备开启/关闭操作的回调函数，底层在相应操作下自行调用用户注册的回调 \n
 *           4.默认提供“ES8311 CODEC芯片+无降噪芯片”方案，用户使用此方案仅需完成IIC从设备地址和ES8311 CODEC参数适配后即可使用 \n
 *           5.本方案录制音频能力为用户外挂音频设备提供，相关音效、降噪、校准等能力请咨询外挂音频设备厂商 \n
 *           6.用户应在完成外挂音频芯片驱动开发并确保音频芯片工作正常和具备录音功能下，方可使用此接口 \n
 */
int32_t cm_audio_recorder_start(cm_audio_record_format_e format, cm_audio_sample_param_t *sample_param, cm_audio_record_callback_t cb, void *cb_param);

/**
 *  @brief 结束录音
 *
 *  @details ML307N仅用户外挂音频设备完成录音并通过PCM接口将录音PCM数据发送至模组方案支持录音功能，PWM接口输出音频模拟信号方案不支持录音 \n
 *           该方案有如下特性： \n
 *           1.模组自身无录制音频能力，录音相关API执行效果为通过硬件PCM口接收用户外挂音频设备发送的录音PCM数据并将数据通过用户注册的回调函数回调至用户应用程序 \n
 *           2.录制音频操作由用户外挂音频设备完成，音效校准、降噪等由用户外挂音频设备实现 \n
 *           3.如用户外挂音频设备需要驱动才可使用，可注册用户外挂音频设备开启/关闭操作的回调函数，底层在相应操作下自行调用用户注册的回调 \n
 *           4.默认提供“ES8311 CODEC芯片+无降噪芯片”方案，用户使用此方案仅需完成IIC从设备地址和ES8311 CODEC参数适配后即可使用 \n
 *           5.本方案录制音频能力为用户外挂音频设备提供，相关音效、降噪、校准等能力请咨询外挂音频设备厂商 \n
 *           6.用户应在完成外挂音频芯片驱动开发并确保音频芯片工作正常和具备录音功能下，方可使用此接口 \n
 */
void cm_audio_recorder_stop(void);

#if defined(USE_MOBVOI_DSP)
/**
 *  @brief 使能/失能语音后处理
 *
 *  @param [in] on_off true：使能，false：失能
 *
 *  @return None
 *
 *  @details
 */
void cm_audio_enable_mobvoi(bool on_off);
#endif

#endif //#ifdef CM_AUDIO_RECORDER_ENABLE
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif


/** @}*/
