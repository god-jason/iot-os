#ifdef CM_DEMO_AUDIO_SUPPORT
#include "cm_os.h"
#include "cm_sys.h"
#include "cm_pm.h"
#include "string.h"
#include "stdbool.h"
#include "cm_audio_player.h"
#include "cm_audio_recorder.h"
#include "cm_fs.h"
#include "cm_mem.h"
#include "cm_i2c.h"
#include <nr_micro_shell.h>



/***************参数定义***************/
#define STATEconfirm        0xFC            //状态机确认 回读STATEconfirm的寄存值确认IC正常工作状态
#define NORMAL_I2S          0x00
#define NORMAL_LJ           0x01
#define NORMAL_DSPA         0x03
#define NORMAL_DSPB         0x23
#define Format_Len24        0x00
#define Format_Len20        0x01
#define Format_Len18        0x02
#define Format_Len16        0x03
#define Format_Len32        0x04

#define VDDA_3V3            0x00
#define VDDA_1V8            0x01
#define MCLK_PIN            0x00
#define SCLK_PIN            0x01

/***************参数选择***************/
#define MSMode_MasterSelOn  0               //产品主从模式选择:默认选择0为SlaveMode,打开为1选择MasterMode
#define Ratio               64              //实际Ratio=MCLK/LRCK比率，需要和实际时钟比例匹配
#define Format              NORMAL_DSPA     //数据格式选择,需要和实际时序匹配
#define Format_Len          Format_Len16    //数据长度选择,需要和实际时序匹配
#define SCLK_DIV            4               //SCLK分频选择:(选择范围1~20),SCLK=MCLK/SCLK_DIV，超过后非等比增加具体对应关系见相应DS说明
#define SCLK_INV            0               //默认对齐方式为下降沿,1为上升沿对齐,需要和实际时序匹配
#define MCLK_SOURCE         MCLK_PIN        //是否硬件没接MCLK需要用SCLK当作MCLK

#define ADCChannelSel       1               //单声道ADC输入通道选择是CH1(MIC1P/1N)还是CH2(MIC2P/2N)，
#define DACChannelSel       0               //单声道DAC输出通道选择:默认选择0:L声道,1:R声道
#define VDDA_VOLTAGE        VDDA_1V8        //模拟电压选择为3V3还是1V8,需要和实际硬件匹配
#define ADC_PGA_GAIN        8               //ADC模拟增益:(选择范围0~10),具体对应关系见相应DS说明

#define ADC_Volume          200             //ADC数字增益:(选择范围0~255),191:0DB,±0.5dB/Step
#define DAC_Volume          255             //DAC数字增益:(选择范围0~255),191:0DB,±0.5dB/Step

#define Dmic_Selon          0               //DMIC选择:默认选择关闭0,打开为1
#define ADC2DAC_Sel         0               //LOOP选择:内部ADC数据给到DAC自回环输出:默认选择关闭0,打开为1
#define DACHPModeOn         0               //输出负载开启HP驱动:默认选择关闭0,打开为1

#define AUDIO_CODEC_ES8311_IIC_ADDR    (0x18)
#define CODEC_ES8311_I2C_ID        CM_I2C_DEV_1

#define cm_log_printf(log_level, fmt, ...)  app_printf("[audio_es8311]" fmt "\r\n", ##__VA_ARGS__)

static cm_fs_t app_amr_file_fd = NULL;                   //保存至文件系统中的AMR录音文件句柄


osThreadId_t AUDIO_ES8311_APP_TaskHandle = NULL;    //APP主任务句柄


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
        cm_log_printf(0, "CM_AUDIO_PLAY_EVENT_FINISHED %s\n", param);
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
 * ES8311 CODEC驱动应用线程
 *
 */
void audio_es8311_demo_appimg_enter(char *param)
{
    osDelay(400);

    cm_audio_player_set_driver(CM_AUDIO_PLAY_DRIVER_CODEC);
    cm_audio_enable_mobvoi(true);
    while(1)
    {
        osDelay(400);

        /* 录音为AMR文件并播放 */
        app_recorder_amr();
    }
}

int cm_audio_es8311_entry(char * param)
{
    osThreadAttr_t app_task_attr = {0};
    app_task_attr.name  = "audio_es8311_demo_appimg_enter_demo_task";
    app_task_attr.stack_size = 4096 * 2;
    app_task_attr.priority = osPriorityNormal1;

    AUDIO_ES8311_APP_TaskHandle = osThreadNew((osThreadFunc_t)audio_es8311_demo_appimg_enter,0,&app_task_attr);

    return 0;
}

NR_SHELL_CMD_EXPORT(audio_es8311, cm_audio_es8311_entry);
#endif

