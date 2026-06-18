
/**  @file
  demo_tts.c

  @brief
  TODO

*/

#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "yopen_app_main.h"
#include "yopen_debug.h"
#include "yopen_os.h"
#include "mem_map.h"
#include "yopen_tts.h"

#include "yopen_uart.h"
#include "yopen_gpio.h"
#include "yopen_i2s.h"

//PA  HAA9811 根据硬件修改
#define YOPEN_PA_GPIO              1
#define YOPEN_PA_PIN               63
#define YOPEN_PA_FUNC               0

//DAC_EN ME6217供电
#define YOPEN_DAC_EN_GPIO              18
#define YOPEN_DAC_EN_PIN               66//21
#define YOPEN_PA_EN_FUNC               4

//i2s pin ,  codec TM8211 根据硬件修改
#define YOPEN_USP0_LRCK_PIN  YOPEN_PIN_GPIO14
#define YOPEN_USP0_LRCK_FUNC  6

#define YOPEN_USP0_BCLK_PIN  YOPEN_PIN_GPIO13
#define YOPEN_USP0_BCLK_FUNC  6

#define YOPEN_USP0_DIN_PIN  YOPEN_PIN_GPIO4
#define YOPEN_USP0_DIN_FUNC  6

#define YOPEN_USP0_DOUT_PIN  YOPEN_PIN_GPIO5
#define YOPEN_USP0_DOUT_FUNC  6
   

typedef enum 
{
    YOPEN_TTS_I2S_PLAY,
    YOPEN_TTS_I2S_STOP,
    YOPEN_TTS_I2S_SEND,
} yopen_tts_event_e;

typedef struct 
{
    int evet;
    int size;
    char *data;
} yopen_tts_msg_s;

typedef enum
{   
    YOPEN_TTS_STATUS_STOP,
    YOPEN_TTS_STATUS_PLAYING,
    YOPEN_TTS_STATUS_PLAY
} yopen_tts_satus_e;

static yopen_queue_t s_i2s_queue_ref;
static yopen_tts_t  s_tts_handle;
static volatile yopen_tts_satus_e  s_tts_status;

static void _i2s_pin_init(void)
{
    //codec TM8211 不需要MCLK
    //yopen_pin_set_func(YOPEN_PIN_GPIO1, 6); //USIM2_CLK USP0_MCLK

    //yopen_pin_set_func(YOPEN_PIN_GPIO2, 6); //USIM2_CLK USP0_BCLK
    //yopen_pin_set_func(YOPEN_PIN_GPIO3, 6); //USIM2_CLK USP0_LRCK
    yopen_pin_set_func(YOPEN_USP0_LRCK_PIN, YOPEN_USP0_LRCK_FUNC);  //NETSTATUS USP0_LRCK
    yopen_pin_set_func(YOPEN_USP0_BCLK_PIN, YOPEN_USP0_BCLK_FUNC); //WAKEUP_IN USP0_BCLK

    yopen_pin_set_func(YOPEN_USP0_DIN_PIN, YOPEN_USP0_DIN_FUNC); //UART2_RX  USP0_DIN
	yopen_pin_set_func(YOPEN_USP0_DOUT_PIN, YOPEN_USP0_DOUT_FUNC); //UART2_TX  USP0_DOUT
}


//AB类防破音
static void _pa_mode(int mode)
{
    extern void delay_us(uint32_t us);
    delay_us(2000);
    yopen_gpio_set_level(YOPEN_PA_GPIO, LVL_LOW);
    delay_us(4000);
    yopen_gpio_set_level(YOPEN_PA_GPIO, LVL_HIGH);
    delay_us(12000);

    for (int i=0; i<mode; i++)
    {
        yopen_gpio_set_level(YOPEN_PA_GPIO, LVL_LOW);
        delay_us(20);
        yopen_gpio_set_level(YOPEN_PA_GPIO, LVL_HIGH);
        delay_us(20);
    }

    yopen_trace("========== _pa_mode mode %d ==========", mode);
}

static void _pa_on(bool on)
{
    static bool init = false;

    if (!init)
    {
        init = true;
        yopen_pin_set_func(YOPEN_PA_PIN, YOPEN_PA_FUNC);
        yopen_gpio_init(YOPEN_PA_GPIO, GPIO_OUTPUT,FORCE_PULL_NONE, on);
    }
    
	yopen_gpio_set_level(YOPEN_PA_GPIO, on);

    yopen_trace("========== _pa_on on %d ==========", on);
	
    if(on)
    {
        //AB类防破音模式
        _pa_mode(5);
    }
}

static void _dac_en(bool on)
{
    static bool init = false;

    if (!init)
    {
        init = true;
        yopen_pin_set_func(YOPEN_DAC_EN_PIN, YOPEN_PA_EN_FUNC);
        yopen_gpio_init(YOPEN_DAC_EN_GPIO, GPIO_OUTPUT, FORCE_PULL_NONE, on);
    }

    yopen_gpio_set_level(YOPEN_DAC_EN_GPIO, on);
}

static void _i2s_init(void)
{
    yopen_errcode_i2s_e ret;
    yopen_i2s_config_s config;

    yopen_aon_power_on();

    _i2s_pin_init();

    memset(&config, 0, sizeof(config));
    config.mode = YOPEN_PCM_MODE;
    config.role = YOPEN_MASTER_MODE;
    config.sample_rate = YOPEN_SAMPLERATE_8K;
    config.frame_size = YOPEN_FRAME_SIZE_16_16;
    config.channel_sel = YOPEN_MONO;
    config.polarity = YOPEN_POLARITY_0;
    ret = yopen_i2s_open(YOPEN_I2S_PORT_0, &config);
    
    yopen_trace("========== yopen_i2s_open ret %d ==========", ret);

    yopen_rtos_task_sleep_ms(3000);
}

static void _i2s_msg_send(int event, int size, char *data)
{
    yopen_tts_msg_s msg;

    memset(&msg, 0, sizeof(msg));
    msg.evet = event;
    msg.size = size;
    msg.data = data;
    yopen_rtos_queue_release(s_i2s_queue_ref, sizeof(msg), (uint8*)&msg, YOPEN_NO_WAIT);
}


static void _data_cb(yopen_tts_t tts_ref, uint8_t *data, uint16_t len)
{
    char *pcm = (char *)malloc(len);

    memcpy(pcm, data, len);

    _i2s_msg_send(YOPEN_TTS_I2S_SEND, len, pcm);

    yopen_trace("[2.1]  write pcm data len %d", len);
}

static void _end_cb(yopen_tts_t tts_ref, int end_result)
{
    _i2s_msg_send(YOPEN_TTS_I2S_STOP, 0, NULL);
	yopen_trace("tts callback:_end_cb result:[%d]",end_result);
}

static void _mute_play(bool stop)
{
    char *mute_data = malloc(640);

    memset(mute_data, 0, 640);
    
    for (int i=0; i<5; i++)
    {
        yopen_i2s_write(YOPEN_I2S_PORT_0, (uint8_t *)mute_data, 640, stop);
        yopen_trace("========== _mute_play stop %d==========", stop);
    }

    free(mute_data);
}


void demo_tts_init(void)
{
    int speed = 0;
	int volume = 0;
    int ret = 0;

    _i2s_init();

    yopen_tts_create(&s_tts_handle, tts_resource_8k);

    yopen_tts_set_config_param(s_tts_handle, YOPEN_TTS_CONFIG_SPEED, YOPEN_TTS_SPEED_NORMAL);
    ret = yopen_tts_get_config_param(s_tts_handle, YOPEN_TTS_CONFIG_SPEED, &speed);

    yopen_tts_set_config_param(s_tts_handle, YOPEN_TTS_CONFIG_VOLUME, YOPEN_TTS_VOLUME_NORMAL);
    ret = yopen_tts_get_config_param(s_tts_handle, YOPEN_TTS_CONFIG_VOLUME, &volume);

    yopen_trace("speed[%d],volume[%d]",speed,volume);

    ret = yopen_tts_set_config_param(s_tts_handle, YOPEN_TTS_CONFIG_CODEPAGE, YOPEN_TTS_CODEPAGE_UTF8);
    yopen_trace("yopen_set_coding ret %d", ret);
}

void demo_tts_play(char *test, int len)
{
    if (!s_tts_handle)
    {
        return;
    }

    s_tts_status = YOPEN_TTS_STATUS_PLAYING;
    _i2s_msg_send(YOPEN_TTS_I2S_PLAY, 0, NULL);
    yopen_tts_start(s_tts_handle, test, len, _data_cb, _end_cb);
}

void demo_tts_stop(void)
{
    if (!s_tts_handle)
    {
        return;
    }

    yopen_tts_stop(s_tts_handle);
    _i2s_msg_send(YOPEN_TTS_I2S_STOP, 0, NULL);
}

bool demo_tts_is_running(void)
{
    return (s_tts_status == YOPEN_TTS_STATUS_PLAYING) || (s_tts_status == YOPEN_TTS_STATUS_PLAY);
}

void demo_i2s_task(void *argv)
{
    yopen_rtos_queue_create(&s_i2s_queue_ref, sizeof(yopen_tts_msg_s), 50);
  
	while(1)
	{
		static bool _is_play = false;
        yopen_tts_msg_s msg;

        yopen_rtos_queue_wait(s_i2s_queue_ref, (uint8*)&msg, sizeof(msg), YOPEN_WAIT_FOREVER);

        yopen_trace("========== evet %x, size %d data %x, _is_playing %d, _is_play %d==========", msg.evet, msg.size, msg.data, s_tts_status, _is_play);

        if (msg.evet == YOPEN_TTS_I2S_SEND)
        {
            yopen_i2s_write(YOPEN_I2S_PORT_0, (uint8_t *)msg.data, msg.size, 0);
            
            if (msg.data)
            {
                free(msg.data);
            }
        }
        else if (msg.evet == YOPEN_TTS_I2S_PLAY)
        {
            _dac_en(true);
            _pa_on(true);
            _mute_play(false);
            _is_play = true;
            yopen_trace("========== YOPEN_TTS_I2S_PLAY ==========");
        }
        else if (msg.evet == YOPEN_TTS_I2S_STOP)
        {
            _pa_on(false);
            _mute_play(true);
            _dac_en(false);
            s_tts_status = YOPEN_TTS_STATUS_STOP;
            _is_play = false;
            yopen_trace("========== YOPEN_TTS_STATUS_STOP ==========");
        }
    }
}

application_init(demo_i2s_task, "I2S", 2, 24,NULL);