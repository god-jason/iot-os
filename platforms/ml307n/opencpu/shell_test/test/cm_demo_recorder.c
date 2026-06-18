#ifdef CM_DEMO_AUDIO_SUPPORT
#include "cm_os.h"
#include "cm_sys.h"
#include "cm_local_tts.h"
#include "cm_pm.h"
#include "string.h"
#include "stdbool.h"
#include "cm_audio_player.h"
#include "cm_audio_recorder.h"
#include "cm_fs.h"
#include "cm_mem.h"
#include <nr_micro_shell.h>


#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#define APP_RECORDER_TTS_SOUND1                 ("您的录音将被保存为AMR文件，录音时间为2秒，请开始录音")
#define APP_RECORDER_TTS_SOUND2                 ("录音已完成，您的录音是")
#define APP_RECORDER_TTS_SOUND3                 ("您的录音将被保存为AMR文件，录音时间为8秒，请开始录音")

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164
#define cm_log_printf(log_level, fmt, ...)  app_printf("[audio_record]" fmt "\r\n", ##__VA_ARGS__)



typedef struct riff_wave_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t wave_id;
}riff_wave_header;

typedef struct chunk_header {
    uint32_t id;
    uint32_t sz;
}chunk_header;

typedef struct chunk_fmt {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
}chunk_fmt;

typedef struct wave_header {
    riff_wave_header riff;
    chunk_header chunk;
    chunk_fmt fmt;
    chunk_header chunk2;
}wave_header;




uint8_t Tts_sound2_buff[320 * 50 * 4] = {0};    //记录APP_RECORDER_TTS_SOUND2转码数据的数组
uint8_t Tts_sound3_buff[320 * 50 * 10] = {0};   //记录APP_RECORDER_TTS_SOUND3转码数据的数组
uint32_t Tts_sound2_buff_len = 0;               //记录已完成的APP_RECORDER_TTS_SOUND2转码数据的长度
uint32_t Tts_sound3_buff_len = 0;               //记录已完成的APP_RECORDER_TTS_SOUND3转码数据的长度

uint32_t app_pcm_data_buff_len = 0;             //记录录音的PCM数据长度


static cm_fs_t app_amr_file_fd = NULL;                   //保存至文件系统中的AMR录音文件句柄


bool app_tts_is_working = false;                //TTS是否处于工作状态
bool app_player_is_working = false;             //播放器是否处于工作状态


osThreadId_t RECORDER_APP_TaskHandle = NULL;    //APP主任务句柄

static void __app_recorder_tts_callback(cm_local_tts_event_e event, void *param)
{
    switch(event)
    {
        case CM_LOCAL_TTS_EVENT_SYNTH_FAIL:
        case CM_LOCAL_TTS_EVENT_SYNTH_INTERRUPT:
        case CM_LOCAL_TTS_EVENT_SYNTH_FINISH:
            break;
        case CM_LOCAL_TTS_EVENT_PLAY_FAIL:
            app_tts_is_working = false;
            cm_log_printf(0, "[TTS] [%s] CM_LOCAL_TTS_EVENT_PLAY_FAIL\n", (char *)param);
            break;
        case CM_LOCAL_TTS_EVENT_PLAY_INTERRUPT:
            app_tts_is_working = false;
            cm_log_printf(0, "[TTS] [%s] CM_LOCAL_TTS_EVENT_PLAY_INTERRUPT\n", (char *)param);
            break;
        case CM_LOCAL_TTS_EVENT_PLAY_FINISH:
            app_tts_is_working = false;
            cm_log_printf(0, "[TTS] [%s] CM_LOCAL_TTS_EVENT_PLAY_FINISH\n", (char *)param);
            break;
        default:
            break;
    }
}

static void __app_amr_record_cb(cm_audio_record_event_e event, void *param)
{
    cm_audio_record_data_t *record_data = (cm_audio_record_data_t *)param;

    /* 不存在录音文件句柄，结束录音并报错 */
    if (NULL == app_amr_file_fd)
    {
        cm_log_printf(0, "[%s] fs error, no user_amr_fd\n", __func__);
        return;
    }
    if (CM_AUDIO_RECORD_EVENT_DATA == event)
    {
        if(record_data == NULL)
        {
            cm_log_printf(0, "[%s] record_data NULL\n", __func__);
            return;
        }
        /* 将录音的音频数据写入文件系统中的AMR文件 */
        int32_t write_len = cm_fs_write(app_amr_file_fd, record_data->data, record_data->len);
        /* 数据写入错误 */
        if (write_len != record_data->len)
        {
            cm_log_printf(0, "[%s] fs write fail\n", __func__);
            cm_fs_close(app_amr_file_fd);
            app_amr_file_fd = NULL;
        }
    }
    return;
}


//播放回调函数
static void __app_player_process_event(cm_audio_play_event_e event, void *param)
{
    if (event == CM_AUDIO_PLAY_EVENT_FINISHED)                      //判断播放结束
    {
        /* 通知事件为中断触发，不可阻塞，不可做耗时较长的操作 */
        app_player_is_working = false;
    }
}

/**
 * 录音为AMR文件并播放
 *
 */
static void app_recorder_amr(void)
{
    /* 开始录音 */
    if (true == cm_fs_exist("record_file.amr"))
    {
        cm_fs_delete("record_file.amr");
    }

    app_amr_file_fd = cm_fs_open("record_file.amr", CM_FS_WB);

    if (app_amr_file_fd == NULL)
    {
        cm_log_printf(0, "[%s] fs open fail\n", __func__);
        return;
    }
    cm_audio_sample_param_t frame = {.sample_format = CM_AUDIO_SAMPLE_FORMAT_16BIT, .rate = CM_AUDIO_SAMPLE_RATE_8000HZ, .num_channels = CM_AUDIO_SOUND_MONO};
    cm_audio_recorder_start(CM_AUDIO_RECORD_FORMAT_AMRNB_475, &frame, (cm_audio_record_callback_t)__app_amr_record_cb, "475");

    osDelay(1600);

    /* 结束录音 */
    cm_audio_recorder_stop();
    osDelay(140);
    cm_fs_close(app_amr_file_fd);
    app_amr_file_fd = NULL;

    /* 播放录音 */
    cm_audio_play_file("record_file.amr", CM_AUDIO_PLAY_FORMAT_AMRNB, NULL, __app_player_process_event, "FILEAMR");

    osDelay(2000);
}

/**
 * TTS应用初始化
 *
 */
static void app_recorder_tts_init(void)
{
    /* 播放器音量设置为最大值 */
    int32_t volume = 100;
    cm_audio_play_set_cfg(CM_AUDIO_PLAY_CFG_VOLUME, (void *)(uintptr_t)volume);

    /* TTS初始化 */
    cm_local_tts_cfg_t cfg = {0};

    cfg.speed = 5;
    cfg.volume = 1;
    cfg.encode = CM_LOCAL_TTS_ENCODE_TYPE_UTF8;
    cfg.digit = 0;

    if (0 != cm_local_tts_init(&cfg))       //若初始化失败，可能是由于使用的模组型号并非是TTS版本模组型号导致
    {
        cm_log_printf(0, "[TTS] init fail, reboot\n");

        osDelay(200);
        return;
    }
}

/**
 * 录音为WAV文件并播放
 *
 */
static void app_recorder_wav(void)
{
    /* TTS播放开始录音 */
    if (0 != cm_local_tts_play(APP_RECORDER_TTS_SOUND1, strlen(APP_RECORDER_TTS_SOUND1), __app_recorder_tts_callback, "sound1"))
    {
        cm_log_printf(0, "[TTS] synth play, reboot\n");
        osDelay(200);
        return;
    }

    app_tts_is_working = true;

    /* 等待TTS播放完成，__app_recorder_tts_callback()回调中收到相关事件会将app_tts_is_working置为false */
    while(1)
    {
        if (false == app_tts_is_working)
        {
            break;
        }

        osDelay(20);
    }

    //目前还不支持WAV录音
    cm_audio_player_set_volume(70);
}

/**
 * 录音机线程
 *
 */
void recorder_demo_appimg_enter(char *param)
{
    cm_audio_player_set_driver(CM_AUDIO_PLAY_DRIVER_CODEC);
    cm_audio_enable_mobvoi(true);
    /* TTS应用初始化 */
    app_recorder_tts_init();

    /* 不支持WAV录音 只有TTS*/
    app_recorder_wav();

    while(1)
    {
        osDelay(400);

        /* 录音为AMR文件并播放 */
        app_recorder_amr();
    }
}

int cm_audio_recorder_entry(char * param)
{
    osThreadAttr_t app_task_attr = {0};
    app_task_attr.name  = "recorder_demo_task";
    app_task_attr.stack_size = 4096 * 2;
    app_task_attr.priority = osPriorityNormal1;

    RECORDER_APP_TaskHandle = osThreadNew((osThreadFunc_t)recorder_demo_appimg_enter,0,&app_task_attr);

    return 0;
}

NR_SHELL_CMD_EXPORT(recorder, cm_audio_recorder_entry);
#endif

