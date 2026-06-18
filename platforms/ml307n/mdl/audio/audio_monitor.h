
#ifndef __AUDIO_MONITOR_H
#define __AUDIO_MONITOR_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "audio_local.h"
#include "audio_voice.h"
/**
 * @brief 错误码 
 * 
 */
#define     AUDIO_OK                    0
#define     AUDIO_ERR                   -1
#define     AUDIO_BUSY                  -2
#define     AUDIO_CODEC_ERR             -3
#define     AUDIO_BUS_ERR               -4
#define     AUDIO_PARA_ERR              -5
/**
 * @brief 应用优先级定义(值越大，优先级越高)
 * 
 */
typedef enum
{
    POWER_ON_AUDIO_PRIO = 1,
    SETTING_TIPS_AUDIO_PRIO,
    MOVE_FRAME_AUDIO_PRIO,
    WECHAT_NEW_MSG_AUDIO_PRIO,
    APP_SEARCH_DEV_AUDIO_PRIO,
    ALERT_ALARM_AUDIO_PRIO,
    POWER_OFF_AUDIO_PRIO,
    ONLINE_PLAYER_AUDIO_PRIO,       /** 在线播放器*/
    VIDEO_CALL_IN_AUDIO_PRIO,
    VOLTE_CALL_IN_AUDIO_PRIO,
    VOLTE_LOCAL_TONE_AUDIO_PRIO,

    WECHAT_VOICE_AUDIO_PRIO = 0xF0,
    VIDEO_VOICE_CALL_AUDIO_PRIO,
    VOLTE_VOICE_CALL_AUDIO_PRIO
}AUDIO_Prio;
       
/**
 * @brief 语音控制类型
 * 
 */
#define     LOCAL_AUDIO      0          ///< 本地音频播放
#define     VOICE_CALL       1          ///< 实时语音通话
/**
 * @brief 回调事件类型
 */
#define     PLAY_OVER_CB_EVENT          1   /** 播放结束 */
#define     RECORD_OVER_CB_EVENT        2   /** 录音结束 */
#define     SCHEDULE_QUIT_EVENT         4   /** 抢占退出 */
#define     ONLINE_PLAYER_OVER_EVENT    8   /** 在线数据播放完 */

typedef enum 
{
    AUDIO_IDLE_STATE    = (1 << 0),        ///< 空闲状态，可被申请使用
    AUDIO_READY_STATE   = (1 << 1),        ///< 准备状态，已被申请，等待启动播放或录音，可被高优先级应用抢占
    AUDIO_PLAY_STATE    = (1 << 2),        ///< 播放状态，可被高优先级应用抢占
    AUDIO_RECORD_STATE  = (1 << 3)         ///< 录音状态，可被高优先级应用抢占
}AudioSate;

#endif
