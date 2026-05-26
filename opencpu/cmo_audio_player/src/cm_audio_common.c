#include "os.h"
#include "drv_pin.h"
#include "nvm.h"
#include "cm_common_api.h"
#include "cm_audio_common.h"
#include "cm_audio_player.h"
#include "cm_audio_recorder.h"

#ifdef AUDIO_COMMON_DEBUG_LOG_SHELL
#define AUDIO_LOG(fmt, ...)   osPrintf("[AUDIO]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define AUDIO_LOG_E(fmt, ...) osPrintf("[AUDIO_E]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define AUDIO_LOG(fmt, ...)   app_printf("[AUDIO]%s %u:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define AUDIO_LOG_E(fmt, ...) app_printf("[AUDIO_E]%s %u:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

static uint8_t audio_accupied = 0;

uint8_t cm_audio_state_get(void)
{
    return audio_accupied;
}

uint8_t cm_audio_state_set(uint8_t state)
{
    audio_accupied = state;

    return 0;
}

/*
 pa_gpio_id 0/1/2/3, corresponding to 0/1/2/3 in AT+MGPIO
 -> module-pin 76/77/86/87
 -> chip PD_GPIO_13/15/14/12
 -> chip resource PIN_34/36/35/33
*/
static const struct PIN_Res * get_pa_gpio_drv_resource(int16_t pa_gpio_id)
{
    const struct PIN_Res * p_pin_res = NULL;
    AUDIO_LOG("gpio_id[%d]", pa_gpio_id);
    switch (pa_gpio_id)
    {
        case CM_AUDIO_PA_GPIO_0:
            p_pin_res = PIN_RES(PIN_34);
            break;
        case CM_AUDIO_PA_GPIO_1:
            p_pin_res = PIN_RES(PIN_36);
            break;
        case CM_AUDIO_PA_GPIO_2:
            p_pin_res = PIN_RES(PIN_35);
            break;
        case CM_AUDIO_PA_GPIO_3:
            p_pin_res = PIN_RES(PIN_33);
            break;
        default:
            CD_LOGAUDIO_LOG_E_E("unsupported pa_gpio_id[%d]", pa_gpio_id);
            break;
    }

    return p_pin_res;
}

void cm_audio_pa_control(int pa_ctrl_id, uint8_t state)
{
    if ((pa_ctrl_id >= CM_AUDIO_PA_CTRL_MAX) || (pa_ctrl_id < 0))
    {
        AUDIO_LOG_E("invalid pa_ctrl_id[%d]", pa_ctrl_id);
        return;
    }

    if(pa_ctrl_id == CM_AUDIO_PA_CHIP)
    {
        cm_audio_pa_chip_set(state);
        return;
    }

    struct PIN_Res * p_pa_pin_res = NULL;

    // GPIO ctrol PA
    p_pa_pin_res = get_pa_gpio_drv_resource(pa_ctrl_id);
    if (p_pa_pin_res != NULL)
    {
        PIN_SetMux(p_pa_pin_res, PIN_33_MUX_GPIO);
        GPIO_SetDir(p_pa_pin_res, GPIO_OUTPUT);
        GPIO_Write(p_pa_pin_res, (state == 0) ? GPIO_LOW : GPIO_HIGH);
    }
    AUDIO_LOG("pa_ctrl_id[%d]", pa_ctrl_id);
}

extern int Board_AudioPinInit(void);
void cm_audio_io_init(void)
{
    // audio PA init
    //cm_audio_pa_control(0);
}

void cm_audio_init(void)
{
    //cm_audio_io_init();

    //cm_audio_player_init();
    //cm_audio_recorder_init();
    AUDIO_LOG("audio init done");
}

