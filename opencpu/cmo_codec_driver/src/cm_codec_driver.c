#include <drv_common.h>
#include "drv_i2s.h"
#include "i2c_device.h"
#include "es8311.h"
#include "aw87390.h"
#include "cm_audio_common.h"
#include "cm_codec_driver.h"
#include "nvparam_pubcfg.h"
#include "nvparam_top.h"
#include "nvm.h"
#if defined(USE_MOBVOI_DSP)
#include "voice_bridge.h"

static bool g_codec_mobvoi_enable = false;
static VoiceProcFunc g_voice_proc_func = {0};
#endif

#define AUDIO_NV_MAGIC_NUMBER           0x61756469

#define HTONL(x) ((((x) & (uint32_t)0x000000ffUL) << 24) |\
                    (((x) & (uint32_t)0x0000ff00UL) <<  8) |\
                    (((x) & (uint32_t)0x00ff0000UL) >>  8) |\
                    (((x) & (uint32_t)0xff000000UL) >> 24))

static NV_AudioCfgItem *g_audioCfg = NULL;
static int g_volume = 75;//ES8211推荐增益0DB，volume0xBF对应75

//static function
static int8_t cm_codec_setVolumeLevel(uint8_t level);
static int8_t  cm_codec_loadAllCfgFromDev(void);
static void cm_codec_loadAllCfgFromNv(void);
static int8_t cm_codec_syncAllToDev(void);
static int8_t cm_codec_checkParaRange(uint8_t paraNo, uint8_t *para, uint8_t len);
static int8_t cm_codec_getAdcDigitGain(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setAdcDigitGain(uint8_t *para, uint8_t len);
static int8_t cm_codec_getDacDigitGain(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setDacDigitGain(uint8_t *para, uint8_t len);
static int8_t cm_codec_getAdcPgaGain(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setAdcPgaGain(uint8_t *para, uint8_t len);
static int8_t cm_codec_getMicGain(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setMicGain(uint8_t *para, uint8_t len);
static int8_t cm_codec_getNrGate(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setNrGate(uint8_t *para, uint8_t len);
static int8_t cm_codec_getAdcEQ(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setAdcEQ(uint8_t *para, uint8_t len);
static int8_t cm_codec_getDacEQ(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setDacEQ(uint8_t *para, uint8_t len);
static int8_t cm_codec_getDrc(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setDrc(uint8_t *para, uint8_t len);
static int8_t cm_codec_getAlc(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setAlc(uint8_t *para, uint8_t len);
static int8_t cm_codec_getSideTone(uint8_t *para, uint8_t *len);
static int8_t cm_codec_setSideTone(uint8_t *para, uint8_t len);
static int8_t cm_codec_readCfg(void);

const static int32_t g_CodecVolumeDefaultLevel[] =
{
    -6000,  -5000, -4000, -3000, -2000, -1000, -1000, -1000, -1000, -1000
};

static I2C_BusPin g_i2cPin = {0};
typedef struct
{
    const struct PIN_Res *pinRes;
    uint8_t func;
}cm_codec_pin_cfg;

typedef struct
{
    cm_codec_pin_cfg codecWs;        ///< codec ws 信号
    cm_codec_pin_cfg codecSck;       ///< codec bit 时钟
    cm_codec_pin_cfg codecOut;       ///< 发送输出给codec
    cm_codec_pin_cfg codecIn;        ///< 从codec接收数据
    cm_codec_pin_cfg codecScl;       ///< 配置codec iic 时钟
    cm_codec_pin_cfg codecSda;       ///< 配置codec iic 数据
    uint8_t i2c_bus;                 ///< I2C 总线
}cm_codec_pin;

static cm_codec_pin *g_codec_PinPtr = NULL;

void cm_codec_pinCfg(void *pin)
{
    g_codec_PinPtr = (cm_codec_pin *)pin;
}

static void cm_codec_pinInit(void)
{
    // I2S 引脚复用
    PIN_SetMux(g_codec_PinPtr->codecWs.pinRes, g_codec_PinPtr->codecWs.func);
    PIN_SetMux(g_codec_PinPtr->codecSck.pinRes, g_codec_PinPtr->codecSck.func);
    PIN_SetMux(g_codec_PinPtr->codecIn.pinRes, g_codec_PinPtr->codecIn.func);
	PIN_SetMux(g_codec_PinPtr->codecOut.pinRes, g_codec_PinPtr->codecOut.func);
    // I2C 引脚复用
    PIN_SetMux(g_codec_PinPtr->codecSda.pinRes, g_codec_PinPtr->codecSda.func);
    PIN_SetMux(g_codec_PinPtr->codecScl.pinRes, g_codec_PinPtr->codecScl.func);
}

static void cm_codec_pinUnInit(void)
{
    // I2S 引脚复用
    PIN_SetMux(g_codec_PinPtr->codecWs.pinRes, 0);
    GPIO_SetDir(g_codec_PinPtr->codecWs.pinRes, GPIO_OUTPUT);
    GPIO_Write(g_codec_PinPtr->codecWs.pinRes, GPIO_LOW);

    PIN_SetMux(g_codec_PinPtr->codecSck.pinRes, 0);
    GPIO_SetDir(g_codec_PinPtr->codecSck.pinRes, GPIO_OUTPUT);
    GPIO_Write(g_codec_PinPtr->codecSck.pinRes, GPIO_LOW);

    PIN_SetMux(g_codec_PinPtr->codecIn.pinRes, 0);
    GPIO_SetDir(g_codec_PinPtr->codecIn.pinRes, GPIO_OUTPUT);
    GPIO_Write(g_codec_PinPtr->codecIn.pinRes, GPIO_LOW);

	PIN_SetMux(g_codec_PinPtr->codecOut.pinRes, 0);
    GPIO_SetDir(g_codec_PinPtr->codecOut.pinRes, GPIO_OUTPUT);
    GPIO_Write(g_codec_PinPtr->codecOut.pinRes, GPIO_LOW);

    // PIN_SetMux(g_codec_PinPtr->codecSda.pinRes, 0);
    // GPIO_SetDir(g_codec_PinPtr->codecSda.pinRes, GPIO_OUTPUT);
    // GPIO_Write(g_codec_PinPtr->codecSda.pinRes, GPIO_LOW);

    // PIN_SetMux(g_codec_PinPtr->codecScl.pinRes, 0);
    // GPIO_SetDir(g_codec_PinPtr->codecScl.pinRes, GPIO_OUTPUT);
    // GPIO_Write(g_codec_PinPtr->codecScl.pinRes, GPIO_LOW);

    CD_LOG("Codec_PinUnInit success");
}


static int8_t es8311_codec_Initialize(uint32_t sampleRate, uint8_t dataBits)
{

    g_i2cPin.sclRes = g_codec_PinPtr->codecScl.pinRes;
    g_i2cPin.sclMux = g_codec_PinPtr->codecScl.func;
    g_i2cPin.sdaRes = g_codec_PinPtr->codecSda.pinRes;
    g_i2cPin.sdaMux = g_codec_PinPtr->codecSda.func;

    I2C_BusPinInit(g_codec_PinPtr->i2c_bus, &g_i2cPin);

    es8311_i2c_config(g_codec_PinPtr->i2c_bus);
    if(sampleRate == CM_AUDIO_SAMPLE_RATE_11025HZ|| sampleRate == CM_AUDIO_SAMPLE_RATE_12000HZ)
    {
        es8311_codec_init(sampleRate, I2S_128FS *sampleRate, MEDIA_HAL_MODE_SLAVE);
    }
    else
    {
        es8311_codec_init(sampleRate, I2S_64FS  *sampleRate, MEDIA_HAL_MODE_SLAVE);
    }
    es8311_codec_config_format(MEDIA_HAL_CODEC_MODE_BOTH, MEDIA_HAL_I2S_LEFT);
    es8311_codec_set_bits_per_sample(MEDIA_HAL_CODEC_MODE_BOTH, dataBits);

    if(!g_audioCfg)
    {
        g_audioCfg = (NV_AudioCfgItem *)osMalloc(sizeof(NV_AudioCfgItem));
        OS_ASSERT(g_audioCfg);

        cm_codec_loadAllCfgFromNv();
        cm_codec_syncAllToDev();
    }

    return 0;
}


static int8_t es8311_codec_UnInitialize(void)
{
    es8311_i2c_deconfig(g_codec_PinPtr->i2c_bus);
    cm_codec_pinUnInit();

    if(g_audioCfg)
    {
        osFree(g_audioCfg);
        g_audioCfg = NULL;
    }
    return 0;
}
static int8_t cm_codec_i2s_init(uint32_t sampleRate, uint8_t dataBits, uint16_t frameSize, cm_codec_callback cb, bool use_mobvoi)
{
    I2S_BusCfg busCfg = {0};
    I2S_TimingCfg i2sTimingCfg = {0};

    i2sTimingCfg.chnl = I2S_LEFT_CHNL;
    i2sTimingCfg.dataBits = dataBits;
    i2sTimingCfg.dataCycle = DATA_32_BITS;

    busCfg.workMode = I2S_PLAY_MODE | I2S_RECORD_MODE;
    busCfg.timing = TIMING_I2S;
    busCfg.frameSize = frameSize;
    busCfg.timingCfg = &i2sTimingCfg;
    busCfg.sampleRate = sampleRate;
    busCfg.fs = I2S_64FS;   //I2S_64FS
#if defined(USE_MOBVOI_DSP)
    if (use_mobvoi)
    {
        voice_proc_set_cb_func(&g_voice_proc_func);
        return I2S_Initialize(&busCfg, cb, &g_voice_proc_func);
    }
    else
#endif
    {
        return I2S_Initialize(&busCfg, cb, NULL);
    }
}

static void cm_codec_i2s_uninit()
{
    I2S_UnInitialize();
}

static int8_t Codec_Start(uint8_t mode)
{
    if(mode & CODEC_PLAY_MODE)
    {
        es8311_codec_set_volume(g_volume);
        es8311_codec_start(MEDIA_HAL_CODEC_MODE_DECODE);
        osThreadMsSleep(50);
    }

    if(mode & CODEC_RECORD_MODE)
    {
        es8311_codec_start(MEDIA_HAL_CODEC_MODE_ENCODE);
    }
    cm_codec_syncAllToDev();
    return 0;
}

static int8_t Codec_Stop(uint8_t mode)
{
    if(mode & CODEC_PLAY_MODE)
    {
        es8311_codec_stop(MEDIA_HAL_CODEC_MODE_DECODE);
    }

    if(mode & CODEC_RECORD_MODE)
    {
        es8311_codec_stop(MEDIA_HAL_CODEC_MODE_ENCODE);
    }

    return 0;
}


int8_t cm_codec_init(uint32_t sampleRate, uint8_t dataBits, uint16_t frameSize, cm_codec_callback cb)
{
    cm_codec_pinInit();
    if (cm_codec_i2s_init(sampleRate, dataBits, frameSize, cb, false) < 0) {
        CD_LOG("i2s init fail");
        return -1;
    }

    if (es8311_codec_Initialize(sampleRate, dataBits) < 0)
    {
        CD_LOG("es8311 init fail");
        return -1;
    }
    //CD_LOG("codec init success");
    return 0;
}

#if defined(USE_MOBVOI_DSP)
int8_t cm_codec_init_with_mobvoi(uint32_t sampleRate, uint8_t dataBits, uint16_t frameSize, cm_codec_callback cb)
{
    cm_codec_pinInit();
    if (cm_codec_i2s_init(sampleRate, dataBits, frameSize, cb, true) < 0) {
        CD_LOG("i2s init fail");
        return -1;
    }

    if (es8311_codec_Initialize(sampleRate, dataBits) < 0)
    {
        CD_LOG("es8311 init fail");
        return -1;
    }
    //CD_LOG("codec init success");
    return 0;
}
#endif

void cm_codec_uninit(void)
{
    cm_codec_i2s_uninit();
    es8311_codec_UnInitialize();
}

int8_t cm_codec_start(uint8_t mode)
{
    Codec_Start(mode);
    I2S_BusStart(mode);//mode

    //CD_LOG("codec start mode [%d]", mode);
    return 0;
}

void cm_codec_stop(uint8_t mode)
{
    I2S_BusStop(mode);
    Codec_Stop(mode);

    //CD_LOG("code stop mode [%d]", mode);
}

int8_t cm_codec_insertData(const uint8_t *data, uint32_t dataSize)
{
    if (I2S_BusWrite(data, dataSize, osWaitForever) > 0) {
        return 0;
    }

    return -1;
}

int8_t cm_codec_readData(uint8_t *data, uint32_t dataSize)
{
    if(I2S_BusGetAvaliableBufCount(I2S_RECORD_MODE) > 0)
    {
        int32_t ret =I2S_BusRead(data,dataSize,osWaitForever);
        if(ret >0)
        {
            return 0;
        }
    }
    return -1;
}

uint8_t cm_codec_getAvaliableBufCount(uint8_t mode)
{
    return I2S_BusGetAvaliableBufCount(mode);
}

static int cm_codec_set_vol(int volume)
{
    if ((volume % AUDIO_VOLUME_MAX_LEVEL) != 0) // (0, 10 ,20  .... 100)
        return -2;

    if(volume == 0)
    {
        es8311_codec_set_mute(1);
        return 0;
    }
    else
    {
        return cm_codec_setVolumeLevel((volume / AUDIO_VOLUME_MAX_LEVEL) - 1);    // 0 - 9
    }
}

int cm_codec_set_volume(int volume)
{
    if(volume <= 0)
    {
        volume = 0;
    }
    else if(volume > 100)
    {
        volume = 100;
    }
    g_volume = volume ;
    CD_LOG("g_volume %d",g_volume);
    //g_volume = volume;
    return 0;
}

int cm_codec_get_volume(int *volume)
{
    if (volume == OS_NULL)
        return -1;

    if (g_volume <= 0)
    {
        *volume = 0;
    }
    else if (g_volume >= 100)
    {
        *volume = 100;
    }
    else
    {
        *volume = g_volume;
    }
    return 0;
}

int cm_audio_pa_chip_set(uint8_t state)
{
#ifdef AUDIO_PA_USE_CHIP
    if (g_codec_PinPtr == NULL)
    {
        CD_LOG_E("g_codec_PinPtr NULL");
        return -1;
    }

    if (state != 0)
    {
        I2C_BusPin i2c_pin = {0};
        i2c_pin.sclMux = g_codec_PinPtr->codecScl.func;
        i2c_pin.sclRes = g_codec_PinPtr->codecScl.pinRes;
        i2c_pin.sdaMux = g_codec_PinPtr->codecSda.func;
        i2c_pin.sdaRes = g_codec_PinPtr->codecSda.pinRes;
        I2C_BusPinInit(g_codec_PinPtr->i2c_bus, &i2c_pin);
        aw87390_init(g_codec_PinPtr->i2c_bus);
    }
    else
    {
        aw87390_deinit(g_codec_PinPtr->i2c_bus);
    }

    return 0;
#else
    return -1;
#endif
}

static int8_t cm_codec_setVolumeLevel(uint8_t level)
{
    if (level > AUDIO_VOLUME_MAX_LEVEL || !g_audioCfg)
        return -1;

    if(g_audioCfg->userVolumeLevel != level)
    {
        g_audioCfg->userVolumeLevel = level;
        NVM_Write(NV_ITEM_ADDR(pubConfig.AudioCfg.MusicCfg.userVolumeLevel), (uint8_t *)&g_audioCfg->userVolumeLevel, sizeof(uint8_t));
    }
    es8311_codec_set_volume(g_volume);
    app_printf(" es8311_codec_set_volume %d ",g_volume);
    return 0;
}

static int8_t  cm_codec_loadAllCfgFromDev(void)
{
    uint8_t len = 0;
    if(!g_audioCfg)
        return -1;

    cm_codec_getAdcDigitGain((uint8_t *)&g_audioCfg->AdcDigitGain, &len);
    cm_codec_getDacDigitGain((uint8_t *)&g_audioCfg->DacDigitGain, &len);
    cm_codec_getAdcPgaGain((uint8_t *)&g_audioCfg->AdcPgaGain, &len);
    cm_codec_getMicGain((uint8_t *)&g_audioCfg->MicGain, &len);
    cm_codec_getNrGate((uint8_t *)&g_audioCfg->NrGate, &len);
    cm_codec_getSideTone((uint8_t *)&g_audioCfg->SideTone, &len);
    cm_codec_getAdcEQ((uint8_t *)g_audioCfg->AdcEQ, &len);
    cm_codec_getDacEQ((uint8_t *)g_audioCfg->DacEQ, &len);
    cm_codec_getAlc((uint8_t *)g_audioCfg->Alc, &len);
    cm_codec_getDrc((uint8_t *)g_audioCfg->Drc, &len);

    g_audioCfg->userVolumeLevel = AUDIO_VOLUME_MAX_LEVEL - 2;
    memcpy(g_audioCfg->volumeLevel, g_CodecVolumeDefaultLevel, AUDIO_VOLUME_MAX_LEVEL * 4);
    for(uint8_t i = 0; i < AUDIO_VOLUME_MAX_LEVEL; i++)
        g_audioCfg->volumeLevel[i] = HTONL(g_audioCfg->volumeLevel[i]);

    return 0;
}

static int8_t cm_codec_readCfg(void)
{
    uint32_t audioCfgNvAddr = 0;
    audioCfgNvAddr = NV_ITEM_ADDR(pubConfig.AudioCfg.MusicCfg.codecID);

    if(NVM_Read(audioCfgNvAddr, (uint8_t *)g_audioCfg, sizeof(NV_AudioCfgItem)) != 0 || g_audioCfg->magicNumber != AUDIO_NV_MAGIC_NUMBER)
    {
        return -1;
    }
    return 0;
}

static void cm_codec_loadAllCfgFromNv(void)
{
    int8_t ret = cm_codec_readCfg();
    if(ret != 0)
    {
        cm_codec_loadAllCfgFromDev();
    }
    else
    {
        g_audioCfg->AdcDigitGain = HTONL(g_audioCfg->AdcDigitGain);
        g_audioCfg->DacDigitGain = HTONL(g_audioCfg->DacDigitGain);
        g_audioCfg->AdcPgaGain = HTONL(g_audioCfg->AdcPgaGain);
        g_audioCfg->MicGain = HTONL(g_audioCfg->MicGain);
        g_audioCfg->NrGate = HTONL(g_audioCfg->NrGate);

        for(uint8_t i = 0; i < AUDIO_VOLUME_MAX_LEVEL; i++)
            g_audioCfg->volumeLevel[i] = HTONL(g_audioCfg->volumeLevel[i]);
    }

    //  app_printf("NV AdcDigitGain: %x, DacDigitGain: %x, AdcPgaGain: %x, MicGain: %x, NrGate: %x, sideTone: %x \r\n",
    //  g_audioCfg->AdcDigitGain, g_audioCfg->DacDigitGain, g_audioCfg->AdcPgaGain, g_audioCfg->MicGain, g_audioCfg->NrGate, g_audioCfg->SideTone);

}

static int8_t cm_codec_syncAllToDev(void)
{
    if(!g_audioCfg)
        return -1;

    cm_codec_setAdcDigitGain((uint8_t *)&g_audioCfg->AdcDigitGain, 4);
    es8311_codec_set_volume(g_volume);

    cm_codec_setAdcPgaGain((uint8_t *)&g_audioCfg->AdcPgaGain,4);
    cm_codec_setMicGain((uint8_t *)&g_audioCfg->MicGain, 4);
    cm_codec_setNrGate((uint8_t *)&g_audioCfg->NrGate, 4);
    cm_codec_setSideTone((uint8_t *)&g_audioCfg->SideTone, 1);
    cm_codec_setAdcEQ((uint8_t *)g_audioCfg->AdcEQ, 46);
    cm_codec_setDacEQ((uint8_t *)g_audioCfg->DacEQ, 32);
    cm_delay(300);
    cm_codec_setAlc((uint8_t *)g_audioCfg->Alc, 12);
    cm_codec_setDrc((uint8_t *)g_audioCfg->Drc, 10);

    es8311_codec_set_mic_gain(18);
    es8311_write_reg(ES8311_ADC_REG17,0xFF);
    return 0;
}

typedef struct
{
    uint32_t paraNo;
    int32_t minVal;
    int32_t MaxVal;
    int32_t step;
}CodecParaRange;

typedef enum
{
    ADC_DIGITAL_GAIN = 1,
    DAC_DIGITAL_GAIN,
    ADC_PGA,
    MIC_GAIN,
    NR_GATE,
    ADC_EQ,
    DAC_EQ,
    SIDE_TONE,
    DRC,
    ALC,
    CODEC_ID,
    MCU_ID,
    VOLUME_LEVEL
}AudioParaType;

static const CodecParaRange g_CodecParaRange [] =
{
#if (CODEC_TYPE == CODEC_ES8311)
    {1, -9550,   3200,   50},           //x100
    {2, -9550,   3200,   50},           //x100
    {3,     0,   3000,  300},           //x100
    {4,     0,   4200,  600},           //x100
    {5, -9600,  -3000,  600},           //x100
    {8,     0,      1,      1},           //x1
    {10,    8,     32,      8},           //x1
#endif
};


static int8_t cm_codec_checkParaRange(uint8_t paraNo, uint8_t *para, uint8_t len)
{
    uint8_t i = 0;
    int32_t paraVal = 0;

    for(i = 0; i < sizeof(g_CodecParaRange)/sizeof(CodecParaRange); i++)
    {
        if(g_CodecParaRange[i].paraNo == paraNo)
        {
            switch (len)
            {
            case 1:
                paraVal = ((int32_t)para[0]) & 0x000000ff ;
                break;
            case 2:
                paraVal = (int32_t)((para[0] << 8) + para[1]) & 0x0000ffff;
                break;
            case 4:
                paraVal = (int32_t)((para[0] << 24) + (para[1] << 16) + (para[2] << 8) + para[3]);
                break;
            default:
                return -1;
                break;
            }

            if( (paraVal < g_CodecParaRange[i].minVal) || (paraVal > g_CodecParaRange[i].MaxVal) ||
                ((paraVal - g_CodecParaRange[i].minVal)% g_CodecParaRange[i].step) != 0)
            {
                // osPrintf("audio check para invalid%d, %d, %d \r\n",paraVal,
                //                                     g_CodecParaRange[i].minVal,
                //                                     g_CodecParaRange[i].MaxVal);
                return -1;
            }

            break;
        }
    }

    return 0;
}

static int8_t cm_codec_getAdcDigitGain(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_adc_digit_gain(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setAdcDigitGain(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;

#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 4)
        return -2;
    if(cm_codec_checkParaRange(ADC_DIGITAL_GAIN, para, len) < 0)
        return -1;
    if(es8311_codec_set_adc_digit_gain((para[0] << 24) + (para[1] << 16) + (para[2] << 8) + para[3]) < 0)
        return -1;

    g_audioCfg->AdcDigitGain = (para[0] ) + (para[1] << 8) + (para[2] << 16) + (para[3] <<24);
#endif
    return 0;
}

static int8_t cm_codec_getDacDigitGain(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_dac_digit_gain(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setDacDigitGain(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 4)
        return -2;
    if(cm_codec_checkParaRange(DAC_DIGITAL_GAIN, para, len) < 0)
        return -1;
    if(es8311_codec_set_dac_digit_gain((para[0] << 24) + (para[1] << 16) + (para[2] << 8) + para[3]) < 0)
        return -1;
    g_audioCfg->DacDigitGain = (para[0] ) + (para[1] << 8) + (para[2] << 16) + (para[3] <<24);
#endif
    return 0;
}


static int8_t cm_codec_getAdcPgaGain(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_adc_pga_gain(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setAdcPgaGain(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 4)
        return -2;
    if(cm_codec_checkParaRange(ADC_PGA, para, len) < 0)
        return -1;
    if(es8311_codec_set_adc_pga_gain((para[0] << 24) + (para[1] << 16) + (para[2] << 8) + para[3]) < 0)
        return -1;
    g_audioCfg->AdcPgaGain = (para[0] ) + (para[1] << 8) + (para[2] << 16) + (para[3] <<24);
#endif
    return 0;
}

static int8_t cm_codec_getMicGain(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_mic_gain(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setMicGain(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 4)
        return -2;
    if(cm_codec_checkParaRange(MIC_GAIN, para, len) < 0)
        return -1;
    if(es8311_codec_set_mic_gain((para[0] << 24) + (para[1] << 16) + (para[2] << 8) + para[3]) < 0)
        return -1;
    g_audioCfg->MicGain = (para[0] ) + (para[1] << 8) + (para[2] << 16) + (para[3] <<24);
#endif
    return 0;
}

static int8_t cm_codec_getNrGate(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_nr_gate(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setNrGate(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 4)
        return -2;
    if(cm_codec_checkParaRange(NR_GATE, para, len) < 0)
        return -1;
    if(es8311_codec_set_nr_gate((para[0] << 24) + (para[1] << 16) + (para[2] << 8) + para[3]) < 0)
        return -1;
    g_audioCfg->NrGate = (para[0] ) + (para[1] << 8) + (para[2] << 16) + (para[3] <<24);
#endif
    return 0;
}

static int8_t cm_codec_getAdcEQ(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_adc_eq(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setAdcEQ(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 46)  // 23 * 2
        return -2;

    if(es8311_codec_set_adc_eq(para, len) < 0)
        return -1;
    memcpy(&g_audioCfg->AdcEQ, para, len);
#endif
    return 0;
}

static int8_t cm_codec_getDacEQ(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_dac_eq(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setDacEQ(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 32)
        return -2;

    if(es8311_codec_set_dac_eq(para, len) < 0)
        return -1;
    memcpy(&g_audioCfg->DacEQ, para, len);
#endif
    return 0;
}

static int8_t cm_codec_getDrc(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_drc(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setDrc(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 10)
        return -2;

    if(es8311_codec_set_drc(para, len) < 0)
        return -1;

    memcpy(&g_audioCfg->Drc, para, len);
#endif
    return 0;
}

static int8_t cm_codec_getAlc(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_alc(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setAlc(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 12)
        return -2;

    if(es8311_codec_set_alc(para, len) < 0)
        return -1;

    memcpy(&g_audioCfg->Alc, para, len);
#endif
    return 0;
}

static int8_t cm_codec_getSideTone(uint8_t *para, uint8_t *len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(es8311_codec_get_side_tone(para, len) < 0)
        return -1;
#endif
    return 0;
}

static int8_t cm_codec_setSideTone(uint8_t *para, uint8_t len)
{
    if(para == OS_NULL)
        return -2;
#if (CODEC_TYPE == CODEC_ES8311)
    if(len != 1)
        return -2;
    if(cm_codec_checkParaRange(SIDE_TONE, para, len) < 0)
        return -1;
    if(es8311_codec_set_side_tone(para[0]) < 0)
        return -1;
    g_audioCfg->SideTone = para[0];
#endif
    return 0;
}

#if defined(USE_MOBVOI_DSP)
void cm_audio_set_mobvoi(bool on_off)
{
    g_codec_mobvoi_enable = on_off;
}

bool cm_audio_get_mobvoi(void)
{
    return g_codec_mobvoi_enable;
}
#endif

