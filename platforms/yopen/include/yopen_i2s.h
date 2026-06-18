
/**  @file
  yopen_audio.h

  @brief
  TODO

*/
                
#ifndef _YOPEN_I2S_H_
#define _YOPEN_I2S_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "yopen_osi_def.h"
#include "yopen_type.h"

/**
 * @defgroup I2S功能
 * @{
*/

/*========================================================================
 *	Macro
 *========================================================================*/


/*========================================================================
*  Enumeration Definition
*========================================================================*/

/*===========================================================================
 * Struct
 ===========================================================================*/

typedef enum
{
  YOPEN_I2S_SUCCESS = 0,
  YOPEN_I2S_NOT_SUPORRT = 10 | (YOPEN_COMPONENT_I2S << 16),
  YOPEN_I2S_ERROR    = 11  | (YOPEN_COMPONENT_I2S << 16),
  YOPEN_I2S_BUSY = 12 | (YOPEN_COMPONENT_I2S << 16),
} yopen_errcode_i2s_e;

typedef enum
{
    YOPEN_I2S_PORT_0,
    YOPEN_I2S_PORT_MAX,
}yopen_i2s_port_e;

typedef enum
{
    YOPEN_MSB_MODE                = 0,        ///< Left aligned mode
    YOPEN_LSB_MODE                = 1,        ///< Right aligned mode
    YOPEN_I2S_MODE                = 2,        ///< I2S mode
    YOPEN_PCM_MODE                = 3,        ///< PCM mode
}yopen_i2s_mode_e;

typedef enum
{
    YOPEN_SLAVE_MODE          = 0,        ///< I2S is slave 
    YOPEN_MASTER_MODE         = 1,        ///< I2S is master
}yopen_i2s_role_e;

typedef enum
{
    YOPEN_SAMPLERATE_8K           = 0,        ///< Sample rate 8k
    YOPEN_SAMPLERATE_16K          = 1,        ///< Sample rate 16k
    YOPEN_SAMPLERATE_22_05K       = 2,        ///< Sample rate 22.05k
    YOPEN_SAMPLERATE_24K	        = 3,        ///< Sample rate 24k
    YOPEN_SAMPLERATE_32K          = 4,        ///< Sample rate 32k
    YOPEN_SAMPLERATE_44_1K        = 5,        ///< Sample rate 44.1k
    YOPEN_SAMPLERATE_48K          = 6,        ///< Sample rate 48k
    YOPEN_SAMPLERATE_96K          = 7,        ///< Sample rate 96k
}yopen_i2s_sample_rate_e;

typedef enum
{
    YOPEN_FRAME_SIZE_16_16        = 0,        ///< WordSize 16bit, SlotSize 16bit
    YOPEN_FRAME_SIZE_16_32        = 1,        ///< WordSize 16bit, SlotSize 32bit
    YOPEN_FRAME_SIZE_24_32        = 2,        ///< WordSize 24bit, SlotSize 32bit
    YOPEN_FRAME_SIZE_32_32        = 3,        ///< WordSize 32bit, SlotSize 32bit
}yopen_i2s_frame_size_e;

typedef enum
{
    YOPEN_POLARITY_0              = 0,
    YOPEN_POLARITY_1              = 1,
}yopen_i2s_bclk_polarity_e;

typedef enum
{
    YOPEN_MONO                    = 0,
    YOPEN_DUAL_CHANNEL            = 1,
}yopen_i2s_channel_sel_e;

typedef struct
{
    yopen_i2s_mode_e               mode;       ///< Audio mode choose
    yopen_i2s_role_e               role;       ///< Role choose
    yopen_i2s_sample_rate_e         sample_rate; ///< Sample rate choose
    yopen_i2s_frame_size_e          frame_size;  ///< Frame size choose
    yopen_i2s_bclk_polarity_e       polarity;   ///< Bclk polarity choose
    yopen_i2s_channel_sel_e         channel_sel; ///< Mono or dual channel select
} yopen_i2s_config_s;

typedef void (*yopen_i2s_callback)(void* user_data);

/** @}*/

/**
 * @brief 打开I2S
 * @param i2s_port 		I2S port
 * @param config 		配置参数
 * @return ** yopen_errcode_i2s_e 
 */
extern yopen_errcode_i2s_e yopen_i2s_open(	yopen_i2s_port_e i2s_port,	yopen_i2s_config_s *config);

/**
 * @brief 关闭I2S
 * @param i2s_port 		I2S port
 * @return ** yopen_errcode_i2s_e 
 */
extern yopen_errcode_i2s_e yopen_i2s_close(yopen_i2s_port_e i2s_port);

/**
 * @brief I2S接受数据 默认是阻塞接口， 设置回填函数就变成非阻塞
 * @param i2s_port 	I2S port
 * @param data 		数据发送缓冲区
 * @param data_len  数据长度
 * @param stop      接受结束后是否stop
 * @return ** yopen_errcode_i2s_e 
 */
extern yopen_errcode_i2s_e yopen_i2s_read(yopen_i2s_port_e i2s_port, uint8_t *data, uint32_t data_len, bool stop);

/**
 * @brief I2S发送数据  默认是阻塞接口， 设置回填函数就变成非阻塞
 * @param i2s_port 		I2S port
 * @param data 		数据发送缓冲区
 * @param data_len  数据长度
 * @param stop      发送结束后是否stop
 * @return ** yopen_errcode_i2s_e 
 */
extern yopen_errcode_i2s_e yopen_i2s_write(yopen_i2s_port_e i2s_port, uint8_t *data, uint32_t data_len, bool stop);

/**
 * @brief 设置I2S write回调函数 
 * @param cb 		回调函数
 * @param user_data 回调函数参数
 * @return ** yopen_errcode_i2s_e 
 */
extern void yopen_i2s_write_set_callback(yopen_i2s_callback cb, void *user_data);

/**
 * @brief 设置I2S read回调函数 
 * @param cb 		回调函数
 * @param user_data 回调函数参数
 * @return ** yopen_errcode_i2s_e   
 */
extern void yopen_i2s_read_set_callback(yopen_i2s_callback cb, void *user_data);

#ifdef __cplusplus
} /*"C" */
#endif

#endif
