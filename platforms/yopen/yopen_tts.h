#ifndef __YOPEN_TTS_H__
#define __YOPEN_TTS_H__

#include "yopen_osi_def.h"
#include "yopen_type.h"

/**
 * @defgroup yopen_tts tts功能
 * @{
*/

extern const char tts_resource_16k[];
extern const char tts_resource_8k[];
typedef enum
{
    YOPEN_TTS_CONFIG_SPEED = 0x00000502,	    ///< 设置播放TTS的速度
    YOPEN_TTS_CONFIG_VOICE_PITCH = 0x00000503,	///< 设置播放TTS的音调
	YOPEN_TTS_CONFIG_VOLUME = 0x00000504,	    ///< 设置播放TTS的音量
	YOPEN_TTS_CONFIG_CODEPAGE = 0x00000101,  	///< 设置输入文本的编码格式
	YOPEN_TTS_CONFIG_READ_DIGIT = 0x00000302,  ///< 设置数字播报格式
} YOPEN_TTS_CONFIG_E;

/* constants for values of parameter YOPEN_TTS_CONFIG_SPEED */
/* the range of voice speed value is from -32768 to +32767 */
#define YOPEN_TTS_SPEED_MIN					-32768		/* slowest voice speed */
#define YOPEN_TTS_SPEED_NORMAL				0			/* normal voice speed (default) */
#define YOPEN_TTS_SPEED_MAX					+32767		/* fastest voice speed */

/* constants for values of parameter YOPEN_TTS_CONFIG_VOICE_PITCH */
/* the range of voice tone value is from -32768 to +32767 */
#define YOPEN_TTS_PITCH_MIN					-32768		/* lowest voice tone */
#define YOPEN_TTS_PITCH_NORMAL				0			/* normal voice tone (default) */
#define YOPEN_TTS_PITCH_MAX					+32767		/* highest voice tone */

/* constants for values of parameter YOPEN_TTS_CONFIG_VOLUME */
/* the range of volume value is from -32768 to +32767 */
#define YOPEN_TTS_VOLUME_MIN				-32768		/* minimized volume */
#define YOPEN_TTS_VOLUME_NORMAL				0			/* normal volume */
#define YOPEN_TTS_VOLUME_MAX				+32767		/* maximized volume (default) */


/* constants for values of parameter YOPEN_TTS_CONFIG_CODEPAGE */
#define YOPEN_TTS_CODEPAGE_ASCII			437			/* ASCII */
#define YOPEN_TTS_CODEPAGE_GBK				936			/* GBK (default) */
#define YOPEN_TTS_CODEPAGE_BIG5				950			/* Big5 */
#define YOPEN_TTS_CODEPAGE_UTF16LE			1200		/* UTF-16 little-endian */
#define YOPEN_TTS_CODEPAGE_UTF16BE			1201		/* UTF-16 big-endian */
#define YOPEN_TTS_CODEPAGE_UTF8				65001		/* UTF-8 */
#define YOPEN_TTS_CODEPAGE_GB2312			YOPEN_TTS_CODEPAGE_GBK
#define YOPEN_TTS_CODEPAGE_GB18030			YOPEN_TTS_CODEPAGE_GBK
#define YOPEN_TTS_CODEPAGE_UTF16			YOPEN_TTS_CODEPAGE_UTF16LE
#define YOPEN_TTS_CODEPAGE_UNICODE			YOPEN_TTS_CODEPAGE_UTF16
#define YOPEN_TTS_CODEPAGE_PHONETIC_PLAIN	23456		/* Kingsoft Phonetic Plain */


/* constants for values of parameter YOPEN_TTS_CONFIG_READ_DIGIT */
#define YOPEN_TTS_READDIGIT_AUTO			0			/* decide automatically (default) */
#define YOPEN_TTS_READDIGIT_AS_NUMBER		1			/* say digit as number */
#define YOPEN_TTS_READDIGIT_AS_VALUE		2			/* say digit as value */


typedef void * yopen_tts_t;

/**
 * @brief 播放输出回调函数，输出PCM数据
 * @param tts_ref TTS引擎句柄
 * @param data  PCM数据地址
 * @param len   PCM数据长度
 */
typedef void (*yopen_tts_output_data_cb)(yopen_tts_t tts_ref, uint8_t *data, uint16_t len);
/**
 * @brief 播放结果回调函数
 * @param tts_ref TTS引擎句柄
 * @param end_result  播放结果，参考yopen_error.h
 */
typedef void (*yopen_tts_play_end_cb)(yopen_tts_t tts_ref, int end_result);

/** 
 * @brief 创建TTS引擎
 * @param resource: 资源文件
 * @return 参考yopen_error.h, YOPEN_RET_OK表示正常
 */
int yopen_tts_create(yopen_tts_t* tts_ref, const char* resource);

/**
 * @brief 设置播放参数
 * @param tts_ref     TTS引擎句柄
 * @param type        设置参数类型，参考 YOPEN_TTS_CONFIG_E
 * @param value       设置参数值
 * @return 参考yopen_error.h, YOPEN_RET_OK表示正常
 */
int yopen_tts_set_config_param(yopen_tts_t tts_ref, YOPEN_TTS_CONFIG_E type, int value);

/**
 * @brief 获取播放参数
 * @param tts_ref     TTS引擎句柄
 * @param type        参数类型，参考 YOPEN_TTS_CONFIG_E
 * @param value[OUT]  参数值
 * @return 参考yopen_error.h, YOPEN_RET_OK表示正常
 */
int yopen_tts_get_config_param(yopen_tts_t tts_ref, YOPEN_TTS_CONFIG_E type, int* value);

/**
 * @brief 开始播放
 * @param tts_ref      TTS引擎句柄
 * @param textString   utf-8字符串数据
 * @param textLen      utf-8字符串长度
 * @param output_cb    播放输出回调函数，输出PCM数据
 * @param end_cb       播放结果回调函数
 * @return 参考yopen_error.h, YOPEN_RET_OK表示正常
 */
int yopen_tts_start(yopen_tts_t tts_ref, const char *textString, uint32 textLen, 
                                    yopen_tts_output_data_cb output_cb, 
                                    yopen_tts_play_end_cb   end_cb
                                    );

/**
 * @brief 判断是否正在播放
 * @param tts_ref TTS引擎句柄
 * @return TRUE：正在播放
 */
bool yopen_tts_is_running(yopen_tts_t tts_ref);

/**
 * @brief 停止播放
 * @param tts_ref TTS引擎句柄
 * @return 参考yopen_error.h, YOPEN_RET_OK表示正常
 */
int yopen_tts_stop(yopen_tts_t tts_ref);

/**
 * @brief 销毁TTS引擎
 * @param tts_ref TTS引擎句柄
 * @return 参考yopen_error.h, YOPEN_RET_OK表示正常
 */
int yopen_tts_destory(yopen_tts_t tts_ref);


/** @}*/

#endif
