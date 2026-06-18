#include "drv_dac.h"
#include "aw87390.h"
#include "drv_pwm_audio.h"
#include "cm_audio_common.h"
#include "cm_codec_driver.h"
#include "cm_audio_player.h"
#include "cm_ringBuffer.h"
#include "cm_mem.h"
#include "vfs.h"
#include "cm_audio_private.h"

#ifdef CM_AUDIO_PLAYER_ENABLE

#ifdef CM_AUDIO_PLAY_HW_DRIVER_PWM
#include "chip_pin_list.h"
//#include "cm_iomux.h"
#endif

#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
#include "mp3dec.h"
#include "cm_audio_mp3_decode.h"
#endif

#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
#include "cm_audio_wav_decode.h"
#include "wav.h"
#endif

#ifdef CM_AUDIO_PLAYER_AMR_ENABLE
#include "interf_dec.h"
#include "dec_if.h"
#include "amr.h"
#endif

#define STEREO_TO_MONO_MIX_2_CH

//#define PERF
#ifdef PERF

#define PERF_START      uint32_t start_tick, end_tick; \
                        static uint32_t last_start = 0; \
                        start_tick= osTickGet(); \
                        MAP_LOG("MIPS: s 0x%lx l 0x%lx", start_tick, (start_tick - last_start)); \
                        last_start = start_tick;

#define PERF_STOP(x)    end_tick = osTickGet(); \
                        MAP_LOG("MIPS: %s e 0x%lx 0x%lx", x, end_tick, (end_tick-start_tick));


#else /* PERF */
#define PERF_START    /* null definition */
#define PERF_STOP(x)  /* null definition */
#endif /* PERF */

#define AUDIO_PLAY_FILE_TASK_STACK_SIZE 20480
#define AUDIO_PLAY_FILE_TASK_PRIORITY osPriorityNormal1

#define AUDIO_PLAY_INSERT_DATA_TASK_STACK_SIZE 4096
#define AUDIO_PLAY_INSERT_DATA_TASK_PRIORITY osPriorityNormal
#define AUDIO_FILE_PATH_LENGTH 130

typedef struct {
    uint8_t *buffer;
    uint32_t length;
} cm_audio_play_file_data_msg_t;

typedef struct {

    char filePath[AUDIO_FILE_PATH_LENGTH];
    VFS_File* fp;
    uint8_t *buff;
    uint32_t buff_len;
    uint32_t read_pos;
    cm_audio_play_format_e audioType;
    cm_audio_sample_param_t sampleParam;

    cm_audio_play_callback_t filePlayCb;
    void *userParam;

    cm_audio_play_file_state_e state;
    cm_audio_play_file_hw_driver_state_e hwState;

    osMessageQueueId_t pcmDataQueue;
    osMessageQueueId_t insert_data_queue;
    void *decoder;

    osSemaphoreId_t stopSemaphor;
    uint32_t is_finished;
    uint32_t insert_is_deinit;
    uint32_t frame_size;
    uint8_t is_file;
} cm_audio_play_file_context_t;

typedef struct {
    uint8_t *buffer;
    uint32_t length;
} cm_audio_play_buff_t;

typedef enum {
    CM_AUDIO_PLAY_STREAM_STATE_INIT,
    CM_AUDIO_PLAY_STREAM_STATE_PLAY,
    CM_AUDIO_PLAY_STREAM_STATE_FINISH,
} cm_audio_play_stream_state_e;

typedef struct {
    cm_audio_play_format_e audioType;
    cm_audio_sample_param_t sampleParam;
    CircularBuffer streamBuffer;
    cm_audio_play_stream_state_e state;

    osSemaphoreId_t startSemaphor;
    osSemaphoreId_t closeSemaphor;
    osSemaphoreId_t clearCloseSemaphor;
    osMessageQueueId_t pcmDataQueue;

    void *decoder;
    uint32_t frame_size;
    uint8_t *buff;
    uint32_t buff_len;
    uint32_t read_pos;
} cm_audio_play_stream_context_t;

typedef enum {
    AUDIO_STREAM_NOT_CONSUME,
    AUDIO_STREAM_SET_CONSUMED,
    AUDIO_STREAM_HAS_STOPED,
} cm_stream_consume_state_e;

cm_audio_play_file_context_t *playFileCtx_p = NULL;
cm_audio_play_stream_context_t *playStreamCtx_p = NULL;

#if defined(USE_MOBVOI_DSP)
#define AUDIO_PLAY_16000HZ_PCM_FRAMES_DECODE_ONCE_MAX    320
#define AUDIO_PLAY_8000HZ_PCM_FRAMES_DECODE_ONCE_MAX     160
#endif

#define PCM_FRAMES_DECODE_ONCE_MAX 160
#define DEFAULT_PCM_FRAMES_DECODE_ONCE_MAX 160
#define BYTES_PER_PCM_FRAME 2 // 仅支持 mono 16bit 采样格式
#define QUEUE_MAX_SIZE (PCM_FRAMES_DECODE_ONCE_MAX * 60)

//audio hw driver selector, default: PWM
static cm_audio_play_driver_e s_audio_hw_driver = CM_AUDIO_PLAY_DRIVER_DAC;
static uint32_t g_driver_is_init = 0;
static uint32_t g_driver_is_start = 0;
static osMutexId_t audio_mutex = NULL;
static cm_audio_pa_ctrl_e s_audio_pa_ctrl = CM_AUDIO_PA_CHIP;

static char* g_audio_fifo = NULL;//[QUEUE_MAX_SIZE];
static int g_audio_fifo_w;
static int g_audio_fifo_r;
static int g_file_offset = 0;
static int g_file_size = 0;
static int g_stream_consumed = 0;
static int g_play_channel = CM_AUDIO_PLAY_CHANNEL_RECEIVER;
static bool isEmpty()
{
    bool ret;
    osMutexAcquire(audio_mutex, osWaitForever);//上锁
    ret = (g_audio_fifo_w == g_audio_fifo_r) ? true:false;
    osMutexRelease(audio_mutex);//释放锁
    return ret;
}

static bool isFull()
{
     bool ret;
     osMutexAcquire(audio_mutex, osWaitForever);//上锁
     ret = ((g_audio_fifo_w + 1) % QUEUE_MAX_SIZE == g_audio_fifo_r) ? true:false;
     osMutexRelease(audio_mutex);//释放锁
     return ret;
}

static int queueSize()
{
    if(isEmpty())
    {
      return 0;
    }
    int size;
    osMutexAcquire(audio_mutex, osWaitForever);//上锁
    if(g_audio_fifo_w > g_audio_fifo_r)
    {
      size = g_audio_fifo_w - g_audio_fifo_r;
    }
    else{
      size = g_audio_fifo_w + QUEUE_MAX_SIZE - g_audio_fifo_r;
    }
    osMutexRelease(audio_mutex);//释放锁
    return size;
}

static bool enqueue(char *buffer, int count)
{
      if(count <= 0)
      {
          return false;
      }
      if(count > (QUEUE_MAX_SIZE - queueSize()))
      {
          return false;
      }
      osMutexAcquire(audio_mutex, osWaitForever);//上锁
      for(int i = 0; i < count; i++)
      {
          if(g_audio_fifo == NULL)
            return false;
          g_audio_fifo[g_audio_fifo_w] = buffer[i];
          ++g_audio_fifo_w;
          g_audio_fifo_w = g_audio_fifo_w % QUEUE_MAX_SIZE;
      }
      osMutexRelease(audio_mutex);//释放锁
      return true;
}

static int dequeue(char * buffer, int count)
{
    if(count < 0)
    {
        return -1;
    }
    if(count > queueSize())
    {
        count = queueSize();
    }
    osMutexAcquire(audio_mutex, osWaitForever);//上锁
    if(g_audio_fifo == NULL)
        return -1;
    for(int i = 0; i < count; i++)
    {
      buffer[i] = g_audio_fifo[g_audio_fifo_r];
      ++g_audio_fifo_r;
      g_audio_fifo_r = g_audio_fifo_r % QUEUE_MAX_SIZE;
    }
    osMutexRelease(audio_mutex);//释放锁
    return count;
}

//在子线程中读文件到队列中，节省父线处理时间
int read_file_to_fifo(int read_len)
{
    osMutexAcquire(audio_mutex, osWaitForever);//上锁
    if(playFileCtx_p == NULL)
    {
        osMutexRelease(audio_mutex);//释放锁
        return -1;
    }
    VFS_File* fp = playFileCtx_p->fp;
    if(fp == NULL)
    {
        osMutexRelease(audio_mutex);//释放锁
        return -1;
    }
    if(read_len + g_file_offset > g_file_size)
    {
        read_len = g_file_size - g_file_offset;
    }
    osMutexRelease(audio_mutex);//释放锁
    if(read_len <= 0)
        return 0;
    if(read_len + queueSize() >= QUEUE_MAX_SIZE)
        return 0;
    char buffer[PCM_FRAMES_DECODE_ONCE_MAX * 30] = {0};
    int count = VFS_ReadFile(buffer, 1, read_len, fp);
    if(count < 0)
    {
        MAP_LOG("[CMO_AUDIO] read_file_to_fifo failed\r\n");
        return -1;
    }
    enqueue(buffer, count);
    osMutexAcquire(audio_mutex, osWaitForever);//上锁
    g_file_offset += count;
    osMutexRelease(audio_mutex);//释放锁
    return 0;
}

int32_t audio_fifo_init(cm_audio_play_format_e audioType)
{
    g_audio_fifo_w = 0;
    g_audio_fifo_r = 0;
    g_file_offset = 0;
    if(g_audio_fifo == NULL)
    {
        g_audio_fifo = (char *)cm_malloc(QUEUE_MAX_SIZE);
        if(NULL == g_audio_fifo)
        {
            MAP_LOG("g_audio_fifo malloc fail");
            return -1;
        }
    }
    g_file_size = VFS_GetFileSize(playFileCtx_p->fp);
    if(g_file_size < 0)
    {
        MAP_LOG("audio_fifo_init fail");
        return -1;
    }
    if(audioType == CM_AUDIO_PLAY_FORMAT_WAV)
    {
#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
        int wav_offset = seek_wav_file_fp();
        if(wav_offset < 0)
        {
            MAP_LOG("get wav offset fail");
            return -1;
        }
        g_file_size -= wav_offset;
#endif
    }
    MAP_LOG("g_file_size %d", g_file_size);
}

void cm_audio_hw_driver_unint(void)
{
    if(g_driver_is_init == 0)
    {
        MAP_LOG("driver_is_init is 0");
        return;
    }

#ifdef CM_AUDIO_PLAY_HW_DRIVER_CODEC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_CODEC)
    {
        cm_codec_stop(CODEC_PLAY_MODE);
        cm_codec_uninit();
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_PWM
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_PWM)
    {
        //cm_codec_disablePA();
        PwmAudio_Uninitialize();
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_DAC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_DAC)
    {
        //cm_codec_disablePA();
        DAC_Uninitialize();
    }
#endif
    g_driver_is_init = 0;
}

int32_t cm_audio_hw_driver_start(uint32_t bufferSize)
{
    uint8_t buf_num = 2;
    if (g_driver_is_start == 1)
    {
        MAP_LOG("already started");
        return 0;
    }

#ifdef CM_AUDIO_PLAY_HW_DRIVER_PWM
    buf_num = PWM_AUDIO_BUF_NUM;
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_DAC
    buf_num = DAC_BUF_NUM;
#endif

    uint8_t *pcmBuffer = (uint8_t *)cm_malloc(bufferSize * buf_num);
    if(pcmBuffer == NULL)
    {
        MAP_LOG("pcmBuffer MALLOC FAIL");
        return 0;
    }
    memset(pcmBuffer, 0, bufferSize * buf_num);

#ifdef CM_AUDIO_PLAY_HW_DRIVER_CODEC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_CODEC)
    {
        cm_codec_start(CODEC_PLAY_MODE);
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_PWM
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_PWM)
    {
        PwmAudio_Start(pcmBuffer, bufferSize * buf_num);
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_DAC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_DAC)
    {
        DAC_Start(pcmBuffer, bufferSize * buf_num);
    }
#endif


    cm_free(pcmBuffer);
    cm_audio_pa_control(s_audio_pa_ctrl, 1);
    osDelay(50);
    g_driver_is_start = 1;

    return 0;
}

void cm_audio_hw_driver_stop(void)
{
    if(g_driver_is_start == 0)
    {
        MAP_LOG("driver_is_start is 0");
        return;
    }

#ifdef CM_AUDIO_PLAY_HW_DRIVER_CODEC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_CODEC)
    {
        cm_codec_stop(CODEC_PLAY_MODE);
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_PWM
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_PWM)
    {
        PwmAudio_Stop();
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_DAC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_DAC)
    {
        DAC_Stop(STOP_DAC_DMA);
    }
#endif
    cm_audio_pa_control(s_audio_pa_ctrl, 0);
    g_driver_is_start = 0;

}

int32_t cm_audio_hw_driver_insertData(const uint8_t *data, uint32_t dataSize)
{

#ifdef CM_AUDIO_PLAY_HW_DRIVER_CODEC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_CODEC)
    {
        return cm_codec_insertData(data, dataSize);
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_PWM
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_PWM)
    {
        return PwmAudio_InsertData(data, dataSize);
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_DAC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_DAC)
    {
        return DAC_InsertData(data, dataSize);
    }
#endif
    MAP_LOG_E("error");
    return -1;
}






/* ===================================== paly from file =============================== */






 cm_audio_play_file_state_e get_playFileCtx_state()
{
    if(playFileCtx_p == NULL)
    {
        return CM_AUDIO_PLAY_FILE_STATE_IDLE;
    }
    return playFileCtx_p->state;
}

static int32_t set_playFileCtx_state(cm_audio_play_file_state_e state)
{
    playFileCtx_p->state = state;
    return 0;
}

 cm_audio_play_file_hw_driver_state_e get_playFileCtx_hwState()
{
    if(playFileCtx_p == NULL)
    {
        return CM_AUDIO_PLAY_FILE_STATE_IDLE;
    }
    return playFileCtx_p->hwState;
}

static int32_t set_playFileCtx_hwState(cm_audio_play_file_hw_driver_state_e state)
{
    playFileCtx_p->hwState = state;
    return 0;
}

#define AUDIO_PLAY_QUEUE_SIZE_MAX       20

static int32_t cm_audio_play_file_context_init(void *data, cm_audio_play_format_e format, cm_audio_sample_param_t *sample_param, cm_audio_play_callback_t cb, void *cb_param, uint8_t is_file)
{
    playFileCtx_p = (cm_audio_play_file_context_t *)cm_malloc(sizeof(cm_audio_play_file_context_t));
    if(playFileCtx_p == NULL)
    {
        MAP_LOG_E("cm_malloc failed");
        return -1;
    }
    memset(playFileCtx_p, 0, sizeof(cm_audio_play_file_context_t));
    if(audio_mutex == NULL)
    {
        audio_mutex = osMutexNew(NULL);
    }
    playFileCtx_p->is_file = is_file;
    if(is_file)
    {
        snprintf(playFileCtx_p->filePath, AUDIO_FILE_PATH_LENGTH, "%s", (char *)data);
        playFileCtx_p->fp = VFS_OpenFile(playFileCtx_p->filePath, "rb");
        if (playFileCtx_p->fp == NULL) {
            cm_free(playFileCtx_p);
            MAP_LOG_E("open fail[%s]", playFileCtx_p->filePath);
            return -2;
        }
    }
    else
    {
        cm_audio_play_buff_t * play_buff = (cm_audio_play_buff_t* )data;
        playFileCtx_p->buff = (uint8_t *)cm_malloc(play_buff->length);
        if(NULL == playFileCtx_p->buff)
        {
            cm_free(playFileCtx_p);
            MAP_LOG_E("playFileCtx_p->buff malloc fail");
            return -2;
        }
        playFileCtx_p->buff_len = play_buff->length;
        playFileCtx_p->read_pos = 0;
        memcpy(playFileCtx_p->buff, play_buff->buffer, play_buff->length);
    }

    playFileCtx_p->audioType = format;
    if (format == CM_AUDIO_PLAY_FORMAT_PCM && sample_param != NULL) {
        uint32_t sampleRate = 0;
        uint8_t dataBits = 0;
        uint16_t frameSize = 0;
        cm_codec_get_param(&sampleRate, &dataBits, &frameSize);
        if(sampleRate != 0 && sampleRate != sample_param->rate)
        {
            if(is_file)
            {
                if(playFileCtx_p->fp)
                {
                    VFS_CloseFile(playFileCtx_p->fp);
                }
                playFileCtx_p->fp = NULL;
            }
            else
            {
                if(playFileCtx_p->buff)
                {
                    cm_free(playFileCtx_p->buff);
                    playFileCtx_p->buff = NULL;
                }
            }
            
            cm_free(playFileCtx_p);
            MAP_LOG_E("codec has inited a different sampleRate");
            return -2;
        }
        if(dataBits != 0 && dataBits != sample_param->sample_format)
        {
            if(is_file)
            {
                if(playFileCtx_p->fp)
                {
                    VFS_CloseFile(playFileCtx_p->fp);
                }
                playFileCtx_p->fp = NULL;
            }
            else
            {
                if(playFileCtx_p->buff)
                {
                    cm_free(playFileCtx_p->buff);
                    playFileCtx_p->buff = NULL;
                }
            }
            cm_free(playFileCtx_p);
            MAP_LOG_E("codec has inited a different dataBits");
            return -2;
        }
        playFileCtx_p->sampleParam = *sample_param;
    }

    playFileCtx_p->filePlayCb = cb;
    playFileCtx_p->userParam = cb_param;
    playFileCtx_p->pcmDataQueue = osMessageQueueNew (AUDIO_PLAY_QUEUE_SIZE_MAX, sizeof(cm_audio_play_file_data_msg_t), NULL);
    playFileCtx_p->insert_data_queue = osMessageQueueNew (AUDIO_PLAY_QUEUE_SIZE_MAX, sizeof(cm_audio_play_file_data_msg_t), NULL);

    playFileCtx_p->stopSemaphor = osSemaphoreNew(1, 0, NULL);

    playFileCtx_p->hwState = CM_AUDIO_PLAY_FILE_HW_DRIVER_STOP;
    playFileCtx_p->state = CM_AUDIO_PLAY_FILE_STATE_INIT;

    MAP_LOG("success");
    return 0;
}

static void cm_audio_play_file_context_uninit()
{
    osSemaphoreDelete(playFileCtx_p->stopSemaphor);

    osMessageQueueDelete(playFileCtx_p->pcmDataQueue);
    osMessageQueueDelete(playFileCtx_p->insert_data_queue);
    osMutexAcquire(audio_mutex, osWaitForever);//上锁
    if(playFileCtx_p->fp)
    {
        VFS_CloseFile(playFileCtx_p->fp);
    }
    playFileCtx_p->fp = NULL;
    if(playFileCtx_p->buff)
    {
        cm_free(playFileCtx_p->buff);
        playFileCtx_p->buff = NULL;
        playFileCtx_p->buff_len = 0;
        playFileCtx_p->read_pos = 0;
    }
    if (g_audio_fifo) {
        cm_free(g_audio_fifo);
        g_audio_fifo = NULL;
    }
    cm_free(playFileCtx_p);

    playFileCtx_p = NULL;
    osMutexRelease(audio_mutex);//释放锁
    if(audio_mutex)
    {
        osMutexDelete(audio_mutex);
        audio_mutex = NULL;
    }
}

void cm_audio_player_file_callback(cm_audio_play_event_e event) {
    if (playFileCtx_p->filePlayCb != NULL) {
        playFileCtx_p->filePlayCb(event, playFileCtx_p->userParam);
    }
}


#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
static size_t file_mp3_read_proc(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    return VFS_ReadFile(pBufferOut, 1, bytesToRead, (VFS_File*) pUserData);
}
#endif

#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
#endif

#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
int seek_wav_file_fp()
{

    uint8_t pData[PCM_FRAMES_DECODE_ONCE_MAX] = {0};
    int32_t readSize = fread(pData, 1, PCM_FRAMES_DECODE_ONCE_MAX, playFileCtx_p->fp);

    uint32_t dataChunkOffset = sizeof(WavFileHeader);
    int32_t ret = WAV_FindDataChunk(pData, readSize);
    if(ret < 0)
    {
        return -1;
    }
    dataChunkOffset += ret;
    fseek(playFileCtx_p->fp, dataChunkOffset, SEEK_SET);
    return dataChunkOffset;
}
int jump_wav_buff_head(uint8_t *wav_buff, uint32_t buff_size)
{
    uint8_t pData[PCM_FRAMES_DECODE_ONCE_MAX] = {0};
    memcpy(pData, wav_buff, buff_size < PCM_FRAMES_DECODE_ONCE_MAX ? buff_size:PCM_FRAMES_DECODE_ONCE_MAX);
    uint32_t dataChunkOffset = sizeof(WavFileHeader);
    int32_t ret = WAV_FindDataChunk(pData, buff_size < PCM_FRAMES_DECODE_ONCE_MAX ? buff_size:PCM_FRAMES_DECODE_ONCE_MAX);
    if(ret < 0)
    {
        return -1;
    }
    dataChunkOffset += ret;
    return dataChunkOffset;
}
#endif

static int32_t cm_audio_play_file_decoder_init()
{
    cm_audio_play_format_e audioType = playFileCtx_p->audioType;
    VFS_File* fp = playFileCtx_p->fp;
#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
        HMP3Decoder mp3 = {0};
        if (audioType == CM_AUDIO_PLAY_FORMAT_MP3) {
            if(playFileCtx_p->is_file)
            {
                mp3 = cm_audio_mp3_file_init(playFileCtx_p->fp, &playFileCtx_p->sampleParam.sample_format, &playFileCtx_p->sampleParam.rate,\
                                &playFileCtx_p->sampleParam.num_channels);
            }
            else
            {
                mp3 = cm_audio_mp3_buff_init(playFileCtx_p->buff, playFileCtx_p->buff_len, &playFileCtx_p->sampleParam.sample_format, &playFileCtx_p->sampleParam.rate,\
                                &playFileCtx_p->sampleParam.num_channels);
            }
            
            playFileCtx_p->decoder = (void *)mp3;
            playFileCtx_p->frame_size = get_mp3_frame_size();
            MAP_LOG("frame_size[%d]", playFileCtx_p->frame_size);
            if ((mp3 == NULL) || (playFileCtx_p->frame_size == 0))
            {
                MAP_LOG("MP3InitDecoder failed");
                return -1;
            }
        }
#endif

#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
        if (audioType == CM_AUDIO_PLAY_FORMAT_WAV) {
            uint8_t pData[PCM_FRAMES_DECODE_ONCE_MAX] = {0};
            if(playFileCtx_p->is_file)
            {
                if(fread(pData, 1, sizeof(WavFileHeader), fp) < 0)
                {
                    return -1;
                }
                if(Audio_GetWavFileInfo(pData, sizeof(WavFileHeader), &playFileCtx_p->sampleParam.sample_format,
                    &playFileCtx_p->sampleParam.rate, &playFileCtx_p->sampleParam.num_channels) < 0)
                {
                    return -1;
                }
            }
            else
            {
                if(playFileCtx_p->buff == NULL || playFileCtx_p->buff_len == 0)
                {
                    return -1;
                }
                if(Audio_GetWavFileInfo(playFileCtx_p->buff, sizeof(WavFileHeader), &playFileCtx_p->sampleParam.sample_format,
                    &playFileCtx_p->sampleParam.rate, &playFileCtx_p->sampleParam.num_channels) < 0)
                {
                    return -1;
                }
                playFileCtx_p->read_pos = jump_wav_buff_head(playFileCtx_p->buff, playFileCtx_p->buff_len);
                if(playFileCtx_p->read_pos < 0)
                {
                    return -1;
                }
            }
        }
#endif

#ifdef CM_AUDIO_PLAYER_AMR_ENABLE
    if(audioType == CM_AUDIO_PLAY_FORMAT_AMRNB || audioType == CM_AUDIO_PLAY_FORMAT_AMRWB)
    {
        int ret = 0;
        if(playFileCtx_p->is_file)
        {
            ret = cm_auido_amr_file_decoder_init(&playFileCtx_p->decoder,playFileCtx_p->fp, &playFileCtx_p->sampleParam.sample_format, \
             &playFileCtx_p->sampleParam.rate, &playFileCtx_p->sampleParam.num_channels);
        }
        else
        {
            ret = cm_auido_amr_buff_decoder_init(&playFileCtx_p->decoder, playFileCtx_p->buff, playFileCtx_p->buff_len, &playFileCtx_p->sampleParam.sample_format, \
             &playFileCtx_p->sampleParam.rate, &playFileCtx_p->sampleParam.num_channels);
        }
        if (ret < 0) {
                MAP_LOG("cm_auido_amr_decoder_init failed");
                return -1;
            }
    }
#endif
/*
    if( playFileCtx_p->sampleParam.num_channels == CM_AUDIO_SOUND_STEREO)
    {
        MAP_LOG("sampleParam is invalid");
        return -2;
    }
*/
    MAP_LOG("====== play from stream =======");
    MAP_LOG("sample rate: %d Hz", playFileCtx_p->sampleParam.rate);
    MAP_LOG("channels: %d", playFileCtx_p->sampleParam.num_channels);
    MAP_LOG("sample format: %d-bit", playFileCtx_p->sampleParam.sample_format);

    return 0;
}


static void cm_audio_play_file_decoder_uninit()
{
    cm_audio_play_format_e audioType = playFileCtx_p->audioType;
    void *decoder = playFileCtx_p->decoder;

    if (audioType == CM_AUDIO_PLAY_FORMAT_PCM) {
        return;
    }

#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
    if (audioType == CM_AUDIO_PLAY_FORMAT_MP3) {
        cm_audio_mp3_deinit((HMP3Decoder *)decoder);
    }
#endif

#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
#endif

#ifdef CM_AUDIO_PLAYER_AMR_ENABLE
     if (audioType == CM_AUDIO_PLAY_FORMAT_AMRNB || audioType == CM_AUDIO_PLAY_FORMAT_AMRWB) {
        amr_release();
        if(playFileCtx_p->decoder)
        {
            decoder_release(playFileCtx_p->decoder);
            playFileCtx_p->decoder =NULL;
        }
    }
#endif

}

// 16位整型 PCM 双声道转单声道
uint32_t stereo_to_mono_s16(int16_t* stereo_data, int16_t* mono_data, size_t frame_count) {
    size_t i = 0;
    for (i = 0; i < frame_count; i++) {
#ifdef STEREO_TO_MONO_MIX_2_CH
        // 混合左右声道，简单平均
        int32_t left = stereo_data[i * 2];
        int32_t right = stereo_data[i * 2 + 1];
        mono_data[i] = (int16_t)((left + right) / 2);
#else
        mono_data[i] = stereo_data[i * 2];
#endif /* STEREO_TO_MONO_MIX_2_CH */
    }
    return (frame_count);
}

static uint32_t cm_audio_play_file_decoder_read_pcm_frames_s16(uint32_t framesToRead, uint8_t *buffer)
{
    uint64_t framesRead;
    cm_audio_play_format_e audioType = playFileCtx_p->audioType;
    void *decoder = playFileCtx_p->decoder;

    if (audioType == CM_AUDIO_PLAY_FORMAT_PCM) {
        if(playFileCtx_p->is_file)
        {
            framesRead = dequeue(buffer, framesToRead * BYTES_PER_PCM_FRAME);
        }
        else
        {
            framesRead = framesToRead * BYTES_PER_PCM_FRAME;
            if(framesToRead * BYTES_PER_PCM_FRAME + playFileCtx_p->read_pos > playFileCtx_p->buff_len)
            {
                framesRead = playFileCtx_p->buff_len - playFileCtx_p->read_pos;
            }
            memcpy(buffer, playFileCtx_p->buff + playFileCtx_p->read_pos, framesRead);
            playFileCtx_p->read_pos += framesRead;
        }
        
        return framesRead;
        //VFS_File* fp = playFileCtx_p->fp;
        //return VFS_ReadFile(buffer, 1, framesToRead * BYTES_PER_PCM_FRAME, fp);
    }

#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
    if (audioType == CM_AUDIO_PLAY_FORMAT_MP3) {
        PERF_START;
        if(playFileCtx_p->is_file)
        {
            framesRead = mp3_file_read_pcm_data(decoder, (int16_t *)buffer);
        }
        else
        {
            framesRead = mp3_buff_read_pcm_data(decoder, (int16_t *)buffer);
        }
        if(2 == playFileCtx_p->sampleParam.num_channels)
        {
            framesRead = stereo_to_mono_s16((int16_t *)buffer, (int16_t *)buffer, framesRead/(BYTES_PER_PCM_FRAME*2));
            PERF_STOP("STER");
            return (framesRead*BYTES_PER_PCM_FRAME);
        }
        else
        {
            PERF_STOP("MONO");
            return framesRead;
        }
    }
#endif

#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
    if (audioType == CM_AUDIO_PLAY_FORMAT_WAV) {
        if(playFileCtx_p->is_file)
        {
            framesRead = dequeue(buffer, framesToRead * BYTES_PER_PCM_FRAME);
        }
        else
        {
            framesRead = framesToRead * BYTES_PER_PCM_FRAME;
            if(framesToRead * BYTES_PER_PCM_FRAME + playFileCtx_p->read_pos > playFileCtx_p->buff_len)
            {
                framesRead = playFileCtx_p->buff_len - playFileCtx_p->read_pos;
            }
            memcpy(buffer, playFileCtx_p->buff + playFileCtx_p->read_pos, framesRead);
            playFileCtx_p->read_pos += framesRead;
        }
        
        if(2 == playFileCtx_p->sampleParam.num_channels)
        {
            framesRead = stereo_to_mono_s16((int16_t *)buffer, (int16_t *)buffer, framesRead/(BYTES_PER_PCM_FRAME*2));
            return (framesRead*BYTES_PER_PCM_FRAME);
        }
        return framesRead;
    }
#endif

#ifdef CM_AUDIO_PLAYER_AMR_ENABLE
    if (audioType == CM_AUDIO_PLAY_FORMAT_AMRNB)
    {
        uint32_t pcmframesRead = 0;
        if(playFileCtx_p->is_file)
        {
            pcmframesRead = amr_file_read_pcm_frames_s16(decoder, 1, buffer);
        }
        else
        {
            pcmframesRead = amr_buff_read_pcm_frames_s16(decoder, 1, buffer);
        }
        
        return (DEFAULT_PCM_FRAMES_DECODE_ONCE_MAX * BYTES_PER_PCM_FRAME * pcmframesRead);
    }

#endif
    return 0;
}


void cm_audio_play_file_hw_driver_cb(uint32_t arg)
{

    cm_audio_play_file_data_msg_t msg = {0};
    osMessageQueueId_t pcmDataQueue = playFileCtx_p->pcmDataQueue;
    osMessageQueueId_t insert_data_queue = playFileCtx_p->insert_data_queue;

    if (osMessageQueueGet(pcmDataQueue, &msg, NULL, 0) == osOK) {
        //MAP_LOG("insert_data_queue %x msg.buffer %x msg.length %d",insert_data_queue,msg.buffer,msg.length);
        int32_t ret = osMessageQueuePut(insert_data_queue, (void *)&msg, 0, 0);
        if (ret != osOK)
        {
            MAP_LOG_E("osMessageQueuePut ret[%d]", ret);
        }
        ret = cm_audio_hw_driver_insertData(msg.buffer, msg.length);
        if (ret != 0)
        {
            MAP_LOG_E("insertData ret[%d]", ret);
        }
    }
}

int cm_audio_play_file_clear_data_queue(osMessageQueueId_t queue)
{
    cm_audio_play_file_data_msg_t msg = {0};

    while (osMessageQueueGetCount(queue) != 0) {
        if (osMessageQueueGet(queue, &msg, NULL, 0) == osOK) {
            cm_free(msg.buffer);
        }
    }

    return 0;
}


static void cm_audio_free_insert_data_work_task(void *param)
{
    cm_audio_play_file_data_msg_t msg = {0};
    osMessageQueueId_t insert_data_queue = playFileCtx_p->insert_data_queue;
    while(1)
    {
        int ret = osMessageQueueGetCount(insert_data_queue);
        //MAP_LOG(" insert_data_queue cnt %d, is_finished %d",ret, playFileCtx_p->is_finished);
        osMutexAcquire(audio_mutex, osWaitForever);
        uint32_t isFinished = playFileCtx_p->is_finished;
        osMutexRelease(audio_mutex);
        if(isFinished == 1 && ret == 0)
        {
            playFileCtx_p->insert_is_deinit = 1;
            MAP_LOG("insert_data_work_task finished. insert_data_queue cnt %d",ret);
            break;
        }
        if (osMessageQueueGet(insert_data_queue, &msg, NULL, 10) == osOK) {
            cm_free(msg.buffer);
        }
    }
}
void cm_audio_play_fifo_work_task(void *param)
{
    MAP_LOG("cm_audio_play_fifo_work_task begin");
    uint32_t isFinished = 0;
    while(1)
    {
        osMutexAcquire(audio_mutex, osWaitForever);
        if(playFileCtx_p != NULL)
        {
            isFinished = playFileCtx_p->is_finished;
        }
        else
        {
            isFinished = 1;
        }
        int left_size = g_file_size - g_file_offset;
        osMutexRelease(audio_mutex);
        if(isFinished)
        {
            break;
        }
        if(left_size > 0)
        {
            int q_size = queueSize();
            if(q_size + PCM_FRAMES_DECODE_ONCE_MAX * 8 < QUEUE_MAX_SIZE)
            {
                if(read_file_to_fifo(PCM_FRAMES_DECODE_ONCE_MAX * 20) < 0)
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
        osDelay(5);
    }

}
static void cm_audio_play_file_work_task(void *param)
{
    MAP_LOG("start");
    int loopPeriod = 1; // ms
    cm_audio_play_file_data_msg_t msg = {0};
    osMessageQueueId_t pcmDataQueue = playFileCtx_p->pcmDataQueue;
    osMessageQueueId_t insert_data_queue = playFileCtx_p->insert_data_queue;
    osSemaphoreId_t stopSemaphor = playFileCtx_p->stopSemaphor;
    int ret = 0;
    uint32_t frame_buffer_size = PCM_FRAMES_DECODE_ONCE_MAX * BYTES_PER_PCM_FRAME;

    cm_audio_state_set(1, CM_AUDIO_TASK_PLAY);
    while (true) {

        if (osSemaphoreAcquire(stopSemaphor, 0) == osOK) {
            if (get_playFileCtx_hwState() == CM_AUDIO_PLAY_FILE_HW_DRIVER_START) {
                cm_audio_hw_driver_stop();
                set_playFileCtx_hwState(CM_AUDIO_PLAY_FILE_HW_DRIVER_STOP);
            }

            cm_audio_play_file_clear_data_queue(pcmDataQueue);
            cm_audio_play_file_clear_data_queue(insert_data_queue);
            osMutexAcquire(audio_mutex, osWaitForever);
            playFileCtx_p->is_finished = 1;
            osMutexRelease(audio_mutex);
            MAP_LOG("stopSemaphor");
            break;
        }

        if (get_playFileCtx_state() == CM_AUDIO_PLAY_FILE_STATE_INIT)
        {
            ret = cm_audio_play_file_decoder_init();
            if (ret < 0)
            {
                osDelay(100);
                MAP_LOG_E("decoder init ret[%d]", ret);
                break;
            }
            MAP_LOG_E("sample_format[%d]rate[%d]num_channels[%d]",playFileCtx_p->sampleParam.sample_format,
                playFileCtx_p->sampleParam.rate, playFileCtx_p->sampleParam.num_channels);
            ret = cm_audio_hw_driver_init(&playFileCtx_p->sampleParam, cm_audio_play_file_hw_driver_cb, CM_AUDIO_PLAY_CNTX_FILE);
            if(ret < 0)
            {
                MAP_LOG_E("ret[%d]", ret);
                break;
            }
            if(playFileCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_MP3)
            {
                //codec only
                frame_buffer_size = playFileCtx_p->frame_size;
            }
            if(playFileCtx_p->sampleParam.num_channels == CM_AUDIO_SOUND_STEREO &&
               s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_DAC)
            {
                //dac only
                frame_buffer_size /= CM_AUDIO_SOUND_STEREO;
            }


            if(playFileCtx_p->is_file && 
                (playFileCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_PCM || playFileCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_WAV))
            {
                if(audio_fifo_init(playFileCtx_p->audioType) < 0)
                {
                    break;
                }
                read_file_to_fifo(PCM_FRAMES_DECODE_ONCE_MAX * 30);
                osThreadAttr_t attr = {"Audio_Play_Fifo", osThreadDetached, NULL, 0U, NULL, AUDIO_PLAY_FILE_TASK_STACK_SIZE, AUDIO_PLAY_FILE_TASK_PRIORITY, 0U, 0U};
                osThreadId_t p_task_id = osThreadNew(cm_audio_play_fifo_work_task, NULL, &attr);
                if (p_task_id == NULL)
                {
                    MAP_LOG_E("play Fifo task creat fail");
                }
            }
            cm_audio_hw_driver_start(frame_buffer_size);

            set_playFileCtx_hwState(CM_AUDIO_PLAY_FILE_HW_DRIVER_START);
            set_playFileCtx_state(CM_AUDIO_PLAY_FILE_STATE_PLAY);
            MAP_LOG("init Succ with frame_buffer_size[%u]", frame_buffer_size);
        }

        if (get_playFileCtx_state() == CM_AUDIO_PLAY_FILE_STATE_PLAY)
        {
            if (get_playFileCtx_hwState() == CM_AUDIO_PLAY_FILE_HW_DRIVER_STOP)
            {
                if(playFileCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_MP3)
                {
                    frame_buffer_size = playFileCtx_p->frame_size;
                }
                cm_audio_hw_driver_start(frame_buffer_size);
                set_playFileCtx_hwState(CM_AUDIO_PLAY_FILE_HW_DRIVER_START);
                MAP_LOG("resume");
            }

            uint8_t *data = NULL;
            if (playFileCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_MP3)
            {
#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
                data = (uint8_t *)cm_malloc(CM_MP3_MAX_FRAME_SIZE);
#else
                data = NULL;
#endif
            }
            else
            {
                data = (uint8_t *)cm_malloc(PCM_FRAMES_DECODE_ONCE_MAX * BYTES_PER_PCM_FRAME);
            }

            if (data == NULL)
            {
                MAP_LOG_E("data NULL");
                break;
            }
            uint32_t bytesRead = cm_audio_play_file_decoder_read_pcm_frames_s16(DEFAULT_PCM_FRAMES_DECODE_ONCE_MAX, data);
            if (bytesRead == 0)
            {
                if(playFileCtx_p->is_file &&
                    (playFileCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_PCM || playFileCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_WAV))
                {
                    osMutexAcquire(audio_mutex, osWaitForever);
                    int left_size = g_file_size - g_file_offset;
                    osMutexRelease(audio_mutex);//释放锁
                    if(left_size > 0)
                    {
                        osDelay(loopPeriod);
                        continue;
                    }
                }
                cm_free(data);
                MAP_LOG("success!!");
                break;
            }
#ifdef CM_AUDIO_PLAY_HW_DRIVER_CODEC
            if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_CODEC)
            {
                PERF_START;
                int ret = cm_audio_hw_driver_insertData(data, bytesRead);
                PERF_STOP("PLAY");
                //MAP_LOG("cm_audio_hw_driver_insertData ret[%d]", ret);
                cm_free(data);
            }
            else
#endif
            {
                msg.buffer = data;
                msg.length = bytesRead;
                osMessageQueuePut(pcmDataQueue, (void *)&msg, 0, osWaitForever);
            }
            osDelay(loopPeriod);
        }

        if (get_playFileCtx_state() == CM_AUDIO_PLAY_FILE_STATE_PAUSE) {
            if (get_playFileCtx_hwState() == CM_AUDIO_PLAY_FILE_HW_DRIVER_START) {
                cm_audio_hw_driver_stop();
                set_playFileCtx_hwState(CM_AUDIO_PLAY_FILE_HW_DRIVER_STOP);
            }

            osDelay(loopPeriod);
        }
    }

    while (osMessageQueueGetCount(pcmDataQueue) != 0) {
        osDelay(loopPeriod);
    }

    osMutexAcquire(audio_mutex, osWaitForever);
    playFileCtx_p->is_finished = 1;
    osMutexRelease(audio_mutex);
    if(s_audio_hw_driver != CM_AUDIO_PLAY_DRIVER_CODEC)
    {
        MAP_LOG("pcmDataQueue cnt[%d]insert_is_deinit[%d]",
                osMessageQueueGetCount(pcmDataQueue), playFileCtx_p->insert_is_deinit);
        while(playFileCtx_p->insert_is_deinit == 0)
        {
            osDelay(loopPeriod);
        }
    }

    //MAP_LOG("deinit start s_audio_hw_driver %d",s_audio_hw_driver);
    cm_audio_state_set(0, CM_AUDIO_TASK_PLAY);
    cm_audio_hw_driver_stop();
    if(cm_audio_state_get() == 0)
    {   
        cm_audio_hw_driver_unint();
    }
    cm_audio_play_file_decoder_uninit();
    if(ret == 0)
    {
    cm_audio_player_file_callback(CM_AUDIO_PLAY_EVENT_FINISHED);
    }
    else
    {
        cm_audio_player_file_callback(CM_AUDIO_PLAY_EVENT_INTERRUPT);
    }
    cm_audio_play_file_context_uninit();
    cm_psm_unlock();
    // MAP_LOG("cm_psm_unlock");
    
    MAP_LOG("play file work task finished");
}


static int32_t cm_audio_play_file_start()
{
    osThreadAttr_t attr = {"Audio_Play_File", osThreadDetached, NULL, 0U, NULL, AUDIO_PLAY_FILE_TASK_STACK_SIZE, AUDIO_PLAY_FILE_TASK_PRIORITY, 0U, 0U};
    osThreadId_t p_task_id = osThreadNew(cm_audio_play_file_work_task, NULL, &attr);

    if (p_task_id == NULL)
    {
        MAP_LOG_E("play task creat fail");
        return -1;
    }

    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_CODEC)
    {
        return 0;
    }
    osThreadAttr_t attr1 = {"Audio_Play_Insert", osThreadDetached, NULL, 0U, NULL, AUDIO_PLAY_INSERT_DATA_TASK_STACK_SIZE, AUDIO_PLAY_INSERT_DATA_TASK_PRIORITY, 0U, 0U};

    if (NULL == osThreadNew(cm_audio_free_insert_data_work_task, NULL, &attr1))
    {
        osThreadTerminate(p_task_id);
        return -2;
    }

    return 0;
}

int32_t cm_audio_play_file(const char *path, cm_audio_play_format_e format, cm_audio_sample_param_t *sample_param, cm_audio_play_callback_t cb, void *cb_param)
{
    if (playFileCtx_p != NULL || playStreamCtx_p != NULL) {
        MAP_LOG_E("audio busy");
        return -2;
    }

    if(cm_audio_play_file_context_init(path, format, sample_param, cb, cb_param, 1) != 0)
    {
        MAP_LOG_E("cm_audio_play_file_context_init fail");
        return -3;
    }

    if (0 != cm_audio_play_file_start())
    {
        MAP_LOG_E("cm_audio_play_file_start fail");
        cm_audio_play_file_context_uninit();
        return -4;
    }

    cm_psm_lock();
    osDelay(100);

    MAP_LOG("success");
    return 0;

}

int32_t cm_audio_player_pause(void)
{
    if (playFileCtx_p == NULL) {
        return -1;
    }

    set_playFileCtx_state(CM_AUDIO_PLAY_FILE_STATE_PAUSE);

    return 0;
}

int32_t cm_audio_player_resume(void)
{
    if (playFileCtx_p == NULL) {
        return -1;
    }

    set_playFileCtx_state(CM_AUDIO_PLAY_FILE_STATE_PLAY);

    return 0;
}

int32_t cm_audio_player_stop(void)
{
    if (playFileCtx_p == NULL) {
        return -1;
    }
    osMutexAcquire(audio_mutex, osWaitForever);
    uint32_t isFinished = playFileCtx_p->is_finished;
    osMutexRelease(audio_mutex);
    if(isFinished){
        return -1;
    }
    osSemaphoreRelease(playFileCtx_p->stopSemaphor);

    return 0;
}

/* ===================================== paly from stream =============================== */

#define AUDIO_PLAY_STREAM_BUFFER_SIZE 204800
#define AUDIO_PLAY_STREAM_TASK_STACK_SIZE 20480
#define AUDIO_PLAY_STREAM_TASK_PRIORITY osPriorityNormal

typedef struct {
    uint8_t *buffer;
    uint32_t length;
} cm_audio_play_stream_data_msg_t;

cm_audio_player_stream_cb playStreamCb = NULL;
int g_stop_flag = 0;

int32_t cm_audio_hw_driver_init(cm_audio_sample_param_t *sample_param, cm_audio_hw_driver_cb cb, uint8_t cntx_type)
{
    /*if (g_driver_is_init == 1)
    {
        MAP_LOG("already inited");
        return 0;
    }*/
    if (sample_param == NULL)
    {
        MAP_LOG_E("sample_param NULL");
        return -1;
    }
#ifdef CM_AUDIO_PLAY_HW_DRIVER_CODEC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_CODEC)
    {
        uint16_t frameSize = DEFAULT_PCM_FRAMES_DECODE_ONCE_MAX * BYTES_PER_PCM_FRAME/sample_param->num_channels;
        if(cntx_type == CM_AUDIO_PLAY_CNTX_FILE)
        {
            if ((playFileCtx_p != NULL) && (playFileCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_MP3))
            {
                frameSize = playFileCtx_p->frame_size/sample_param->num_channels;
            }
        }
        else
        {
            if ((playStreamCtx_p != NULL) && (playStreamCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_MP3))
            {
                frameSize = playStreamCtx_p->frame_size/sample_param->num_channels;
            }
        }
#if defined(USE_MOBVOI_DSP)
        if ((cm_audio_get_mobvoi()) && (sample_param->rate == CM_AUDIO_SAMPLE_RATE_16000HZ))
        {
            if(cntx_type == CM_AUDIO_PLAY_CNTX_FILE)    
            {
                if(playFileCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_PCM)
                    frameSize = AUDIO_PLAY_16000HZ_PCM_FRAMES_DECODE_ONCE_MAX * BYTES_PER_PCM_FRAME/sample_param->num_channels;
            }
            else
            {
                if(playStreamCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_PCM)
                    frameSize = AUDIO_PLAY_16000HZ_PCM_FRAMES_DECODE_ONCE_MAX * BYTES_PER_PCM_FRAME/sample_param->num_channels;
            }
            cm_codec_init_with_mobvoi(sample_param->rate, \
                          sample_param->sample_format, \
                          frameSize, \
                          NULL);
        }
        else if ((cm_audio_get_mobvoi()) &&(sample_param->rate == CM_AUDIO_SAMPLE_RATE_8000HZ))
        {
            cm_codec_init_with_mobvoi(sample_param->rate, \
                          sample_param->sample_format, \
                          frameSize, \
                          NULL);
        }
        else
        {
            cm_codec_init(sample_param->rate, \
                          sample_param->sample_format, \
                          frameSize, \
                          NULL);
        }
#else
        cm_codec_init(sample_param->rate, \
                      sample_param->sample_format, \
                      frameSize, \
                      NULL);
#endif
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_PWM
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_PWM)
    {
        //cm_codec_enablePA();
        // cm_iomux_set_pin_func(CM_IOMUX_PIN_75 ,CM_IOMUX_FUNC_FUNCTION2);
        PwmAudio_Initialize(sample_param->sample_format, \
                            sample_param->rate, \
                            cb);
    }
#endif

#ifdef CM_AUDIO_PLAY_HW_DRIVER_DAC
    if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_DAC)
    {
        DAC_Initialize(DAC_HW_CLOCK_TRIG, \
                    sample_param->rate, \
                    sample_param->sample_format, \
                    cb);
    }
#endif
    g_driver_is_init = 1;
    MAP_LOG("rate[%d]sample_format[%d]", sample_param->rate, sample_param->sample_format);

    osDelay(50);
    return 0;

}


static cm_audio_play_stream_state_e get_streamCtx_state()
{
    if(playStreamCtx_p == NULL)
        return 0;
    return playStreamCtx_p->state;
}

static int32_t set_streamCtx_state(cm_audio_play_stream_state_e state)
{
    if(playStreamCtx_p == NULL)
        return -1;
    playStreamCtx_p->state = state;
    return 0;
}


static int32_t cm_audio_play_stream_context_init(cm_audio_play_format_e format, cm_audio_sample_param_t *sample_param)
{
    playStreamCtx_p = (cm_audio_play_stream_context_t*)cm_malloc(sizeof(cm_audio_play_stream_context_t));
    memset(playStreamCtx_p, 0, sizeof(cm_audio_play_stream_context_t));

    initCircularBuffer(&playStreamCtx_p->streamBuffer, AUDIO_PLAY_STREAM_BUFFER_SIZE);

    playStreamCtx_p->startSemaphor = osSemaphoreNew(1, 0, NULL);
    playStreamCtx_p->closeSemaphor = osSemaphoreNew(1, 0, NULL);
    playStreamCtx_p->clearCloseSemaphor = osSemaphoreNew(1, 0, NULL);

    playStreamCtx_p->pcmDataQueue = osMessageQueueNew (12, sizeof(cm_audio_play_stream_data_msg_t), NULL);

    playStreamCtx_p->audioType = format;
    if (format == CM_AUDIO_PLAY_FORMAT_PCM && sample_param != NULL) {
        playStreamCtx_p->sampleParam = *sample_param;
    }
    if (playStreamCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_MP3) {
            playStreamCtx_p->buff = (uint8_t *)cm_malloc(CM_MP3_MAX_FRAME_SIZE);
            if(playStreamCtx_p->buff == NULL)
                return -1;
    }
    playStreamCtx_p->state = CM_AUDIO_PLAY_STREAM_STATE_INIT;


    return 0;
}

static void cm_audio_play_stream_context_uninit()
{
    if(playStreamCtx_p == NULL)
        return;
    osMessageQueueDelete(playStreamCtx_p->pcmDataQueue);

    osSemaphoreDelete(playStreamCtx_p->clearCloseSemaphor);
    osSemaphoreDelete(playStreamCtx_p->closeSemaphor);
    osSemaphoreDelete(playStreamCtx_p->startSemaphor);
    if(playStreamCtx_p->buff)
    {
        cm_free(playStreamCtx_p->buff);
        playStreamCtx_p->buff = NULL;
        playStreamCtx_p->buff_len = 0;
        playStreamCtx_p->read_pos = 0;
    }
    uninitCircularBuffer(&playStreamCtx_p->streamBuffer);
    cm_free(playStreamCtx_p);
    playStreamCtx_p = NULL;
}

#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
static size_t stream_mp3_read_proc(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    if(playStreamCtx_p == NULL)
        return 0;
    size_t bytesRead = \
        readCircularBuffer(&(playStreamCtx_p->streamBuffer), pBufferOut, bytesToRead);

    return bytesRead;
}
#endif

#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
#endif


static int32_t cm_audio_play_stream_decoder_init()
{
    if(playStreamCtx_p == NULL)
        return -1;
    cm_audio_play_format_e audioType = playStreamCtx_p->audioType;
#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
        HMP3Decoder mp3 = {0};
        if (audioType == CM_AUDIO_PLAY_FORMAT_MP3) {
            CircularBuffer *buffer_p = &playStreamCtx_p->streamBuffer;
            int read_len = readCircularBuffer(buffer_p, playStreamCtx_p->buff, CM_MP3_MAX_FRAME_SIZE);
            uint32_t headSize;
            MP3GetID3TagSize(playStreamCtx_p->buff, 0xFFFFFFFF, &headSize);

            // skip long head
            MAP_LOG("before skip head, headOffset %d mp3DataSize %d", headSize, read_len);
            while(headSize >= read_len) {
                headSize -= read_len;
                read_len = readCircularBuffer(buffer_p, playStreamCtx_p->buff, CM_MP3_MAX_FRAME_SIZE);
                osDelay(2);
            }
            if (headSize > 0) {
                read_len -= headSize;
                memmove(playStreamCtx_p->buff, playStreamCtx_p->buff + headSize, read_len);
            }
            MAP_LOG("after skip head, headOffset %d mp3DataSize %d", headSize, read_len);

            mp3 = cm_audio_mp3_buff_init(playStreamCtx_p->buff, read_len, &playStreamCtx_p->sampleParam.sample_format, &playStreamCtx_p->sampleParam.rate,\
                                &playStreamCtx_p->sampleParam.num_channels);
            playStreamCtx_p->decoder = (void *)mp3;
            playStreamCtx_p->frame_size = get_mp3_frame_size();
            if ((mp3 == NULL) || (playStreamCtx_p->frame_size == 0))
            {
                MAP_LOG("MP3InitDecoder failed");
                return -1;
            }
        }
#endif

#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
        if (audioType == CM_AUDIO_PLAY_FORMAT_WAV) {
        }
#endif

#ifdef CM_AUDIO_PLAYER_AMR_ENABLE
        if(audioType == CM_AUDIO_PLAY_FORMAT_AMRNB || audioType == CM_AUDIO_PLAY_FORMAT_AMRWB)
        {
            //cm_auido_amr_file_decoder_init(&playStreamCtx_p->decoder,playStreamCtx_p->fp, &playStreamCtx_p->sampleParam.sample_format, \
            //     &playStreamCtx_p->sampleParam.rate, &playStreamCtx_p->sampleParam.num_channels);
        }
#endif


    //MAP_LOG("====== play from stream =======");
    MAP_LOG("sample rate: %d Hz", playStreamCtx_p->sampleParam.rate);
    MAP_LOG("channels: %d", playStreamCtx_p->sampleParam.num_channels);
    MAP_LOG("sample format: %d-bit", playStreamCtx_p->sampleParam.sample_format);

    return 0;
}

static void cm_audio_play_stream_decoder_uninit()
{
    if(playStreamCtx_p == NULL)
        return;
    cm_audio_play_format_e audioType = playStreamCtx_p->audioType;
    void *decoder = playStreamCtx_p->decoder;

    if (audioType == CM_AUDIO_PLAY_FORMAT_PCM) {
        return;
    }

#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
    if (audioType == CM_AUDIO_PLAY_FORMAT_MP3) {
        cm_audio_mp3_deinit((HMP3Decoder *)decoder);
    }
#endif

#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
#endif


}

static uint32_t cm_audio_play_stream_decoder_read_pcm_frames_s16(uint32_t framesToRead, uint8_t *buffer)
{
    if(playStreamCtx_p == NULL)
        return 0;
    uint64_t framesRead;
    cm_audio_play_format_e audioType = playStreamCtx_p->audioType;
    void *decoder = playStreamCtx_p->decoder;

    if (audioType == CM_AUDIO_PLAY_FORMAT_PCM) {
        CircularBuffer *buffer_p = &playStreamCtx_p->streamBuffer;
        return readCircularBuffer(buffer_p, buffer, framesToRead * BYTES_PER_PCM_FRAME);
    }
    if (audioType == CM_AUDIO_PLAY_FORMAT_MP3) {
#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
        PERF_START;
        uint32_t pos = mp3_buff_get_read_pos();
        uint32_t data_len = mp3_buff_get_data_len();
        uint32_t capacity = data_len - pos;
        if(capacity < 512)
        {
            int read_len = 0;
            if(capacity != 0)
            {
                uint8_t tmp_buff[512] = {0};
                memcpy(tmp_buff, playStreamCtx_p->buff + pos, capacity);
                memcpy(playStreamCtx_p->buff, tmp_buff, capacity);
            }
            CircularBuffer *buffer_p = &playStreamCtx_p->streamBuffer;
            read_len = readCircularBuffer(buffer_p, playStreamCtx_p->buff + capacity, CM_MP3_MAX_FRAME_SIZE - capacity);
            mp3_buff_reset(read_len + capacity);
        }
        framesRead = mp3_buff_read_pcm_data(decoder, (int16_t *)buffer);
        if(2 == playStreamCtx_p->sampleParam.num_channels)
        {
            framesRead = stereo_to_mono_s16((int16_t *)buffer, (int16_t *)buffer, framesRead/(BYTES_PER_PCM_FRAME*2));
            PERF_STOP("STER");
            return (framesRead*BYTES_PER_PCM_FRAME);
        }
        else
        {
            PERF_STOP("MONO");
            return framesRead;
        }
#endif
    }


#ifdef CM_AUDIO_PLAYER_WAV_ENABLE
#endif

    return 0;
}

void cm_audio_play_stream_hw_driver_cb(uint32_t arg)
{
    if(playStreamCtx_p == NULL)
        return;
    cm_audio_play_stream_data_msg_t msg = {0};
    osMessageQueueId_t pcmDataQueue = playStreamCtx_p->pcmDataQueue;


    if(osMessageQueueGetCount(pcmDataQueue) == 0 && get_streamCtx_state() == CM_AUDIO_PLAY_STREAM_STATE_PLAY)
    {
        CircularBuffer *buffer_p = &playStreamCtx_p->streamBuffer;
        osMutexAcquire(audio_mutex, osWaitForever);//上锁
        if(getCurrentLength(buffer_p) == 0)
        {
            g_stream_consumed = AUDIO_STREAM_SET_CONSUMED;
        }
        osMutexRelease(audio_mutex);//释放锁
        return;
    }
    if (osMessageQueueGet(pcmDataQueue, &msg, NULL, 0) == osOK) {
        cm_audio_hw_driver_insertData(msg.buffer, msg.length);
        cm_free(msg.buffer);
    }
}

static void cm_audio_play_stream_work_task(void *param)
{
    int loopPeriod = 5; // ms
    if (playStreamCtx_p == NULL)
    {
        MAP_LOG("playStreamCtx_p NULL");
        cm_psm_unlock();
        return;
    }
    osMessageQueueId_t pcmDataQueue = playStreamCtx_p->pcmDataQueue;
    osSemaphoreId_t startSemaphor = playStreamCtx_p->startSemaphor;
    osSemaphoreId_t closeSemaphor = playStreamCtx_p->closeSemaphor;
    osSemaphoreId_t clearCloseSemaphor = playStreamCtx_p->clearCloseSemaphor;
    uint8_t zeroReadFlag = 1;
    cm_audio_play_stream_data_msg_t msg = {0};
    uint32_t frame_buffer_size = 0;
    uint32_t decode_once_size = DEFAULT_PCM_FRAMES_DECODE_ONCE_MAX;
    while (true) {
        if (osSemaphoreAcquire(clearCloseSemaphor, 0) == osOK && get_streamCtx_state() != CM_AUDIO_PLAY_STREAM_STATE_INIT) {
            break;
        }
        size_t  ringbuff_length = getCurrentLength(&playStreamCtx_p->streamBuffer);

        if(ringbuff_length == 0 && zeroReadFlag == 1)
        {
            if (osSemaphoreAcquire(playStreamCtx_p->closeSemaphor, 0) == osOK)
            {
                set_streamCtx_state(CM_AUDIO_PLAY_STREAM_STATE_FINISH);
                osDelay(400);
                break;
            }
        }
        osMutexAcquire(audio_mutex, osWaitForever);//上锁
        if(g_stream_consumed == AUDIO_STREAM_SET_CONSUMED)
        {
            g_stream_consumed = AUDIO_STREAM_HAS_STOPED;
            cm_audio_hw_driver_stop();
            if(cm_audio_state_get() == 0)
            {
                cm_audio_hw_driver_unint();
            }
        }
        osMutexRelease(audio_mutex);//解锁
        if (get_streamCtx_state() == CM_AUDIO_PLAY_STREAM_STATE_INIT) {
#if defined(USE_MOBVOI_DSP)
                if (cm_audio_get_mobvoi()) {
                    if (playStreamCtx_p->sampleParam.rate == CM_AUDIO_SAMPLE_RATE_16000HZ) {
                        decode_once_size = AUDIO_PLAY_16000HZ_PCM_FRAMES_DECODE_ONCE_MAX;
                    }
                    else if (playStreamCtx_p->sampleParam.rate == CM_AUDIO_SAMPLE_RATE_8000HZ) {
                        decode_once_size = AUDIO_PLAY_8000HZ_PCM_FRAMES_DECODE_ONCE_MAX;
                    }
                }
#endif
            if (osSemaphoreAcquire(startSemaphor, cm_ms_to_tick(loopPeriod)) != osOK) {
                continue;
            }
            if(cm_audio_play_stream_decoder_init()<0)
            {
                MAP_LOG("decoder_init err");
                break;
            }
            frame_buffer_size = decode_once_size * BYTES_PER_PCM_FRAME;
            if(playStreamCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_MP3)
            {
                //codec only
                frame_buffer_size = playStreamCtx_p->frame_size;
            }
            if(playStreamCtx_p->sampleParam.num_channels == CM_AUDIO_SOUND_STEREO &&
               s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_DAC)
            {
                //dac only
                frame_buffer_size /= CM_AUDIO_SOUND_STEREO;
            }
            if(cm_audio_hw_driver_init(&playStreamCtx_p->sampleParam, cm_audio_play_stream_hw_driver_cb, CM_AUDIO_PLAY_CNTX_STREAM) == 0)
            {
                cm_audio_state_set(1, CM_AUDIO_TASK_PLAY);
            }
 
            cm_audio_hw_driver_start(frame_buffer_size);
            set_streamCtx_state(CM_AUDIO_PLAY_STREAM_STATE_PLAY);
        }

        if (get_streamCtx_state() == CM_AUDIO_PLAY_STREAM_STATE_PLAY) {
            uint8_t *data = NULL;
            if (playStreamCtx_p->audioType == CM_AUDIO_PLAY_FORMAT_MP3)
            {
#ifdef CM_AUDIO_PLAYER_MP3_ENABLE
                data = (uint8_t *)cm_malloc(CM_MP3_MAX_FRAME_SIZE);
#else
                data = NULL;
#endif
            }
            else
            {
                data = (uint8_t *)cm_malloc(decode_once_size * BYTES_PER_PCM_FRAME);
            }
            if (data == NULL)
            {
                MAP_LOG_E("data NULL");
                break;
            }
            uint32_t bytesRead = cm_audio_play_stream_decoder_read_pcm_frames_s16(decode_once_size, data);
            if (bytesRead == 0) {
                zeroReadFlag = 1;
                cm_free(data);
                osDelay(loopPeriod);
                continue;
            }

            zeroReadFlag = 0;
            // MAP_LOG("cm_audio_play_stream_decoder_read_pcm_frames_s16 read %d", bytesRead);
#ifdef CM_AUDIO_PLAY_HW_DRIVER_CODEC
            if(s_audio_hw_driver == CM_AUDIO_PLAY_DRIVER_CODEC)
            {
                //MAP_LOG("available buffer count %d", cm_codec_getAvaliableBufCount(CODEC_PLAY_MODE));
                int ret = cm_audio_hw_driver_insertData(data, bytesRead);
                // MAP_LOG("cm_audio_hw_driver_insertData ret[%d]", ret);
                cm_free(data);
            }
            else
#endif
            {
                msg.buffer = data;
                msg.length = bytesRead;
                osMutexAcquire(audio_mutex, osWaitForever);//上锁
                if(g_stream_consumed != AUDIO_STREAM_NOT_CONSUME)
                {
                    cm_audio_hw_driver_init(&playStreamCtx_p->sampleParam, cm_audio_play_stream_hw_driver_cb, CM_AUDIO_PLAY_CNTX_STREAM);
                    cm_audio_hw_driver_start(frame_buffer_size);
                    g_stream_consumed = AUDIO_STREAM_NOT_CONSUME;
                }
                osMutexRelease(audio_mutex);//释放锁
                osMessageQueuePut(pcmDataQueue, (void *)&msg, 0, osWaitForever);
            }
        }
        osDelay(loopPeriod);


    }
    cm_audio_state_set(0, CM_AUDIO_TASK_PLAY);
    cm_audio_hw_driver_stop();
    if(cm_audio_state_get() == 0)
    {
        cm_audio_hw_driver_unint();
    }
    cm_audio_play_file_clear_data_queue(pcmDataQueue);
    cm_audio_play_stream_decoder_uninit();
    cm_audio_play_stream_context_uninit();

    cm_audio_player_stream_callback(CM_AUDIO_TRACK_EVENT_CLOSED);
    

    MAP_LOG("stream play finished");
    cm_psm_unlock();
}

static int32_t cm_audio_play_stream_start()
{
    osThreadAttr_t attr = {"AudioPlayStream", osThreadDetached, NULL, 0U, NULL, AUDIO_PLAY_STREAM_TASK_STACK_SIZE, AUDIO_PLAY_STREAM_TASK_PRIORITY, 0U, 0U};
    osThreadId_t p_task_id = osThreadNew(cm_audio_play_stream_work_task, NULL, &attr);
    if (p_task_id == NULL)
    {
        MAP_LOG_E("p_task_id NULL");
        return -1;
    }


    return 0;
}

void cm_audio_player_stream_cb_reg(cm_audio_player_stream_cb cb)
{
    playStreamCb = cb;
}

void cm_audio_player_stream_callback(cm_audio_player_stream_event_e event) {
    if (playStreamCb != NULL) {
        playStreamCb(event);
    }
}

int32_t cm_audio_player_stream_open(cm_audio_play_format_e format, cm_audio_sample_param_t *sample_param)
{
    if (playStreamCtx_p != NULL || playFileCtx_p != NULL) {
        MAP_LOG_E("audio busy");
        return -1;
    }

    cm_audio_play_stream_context_init(format, sample_param);
    g_stop_flag = 0;
    int ret = cm_audio_play_stream_start();
    if (ret != 0)
    {
        MAP_LOG_E("play_stream_start ret[%d]", ret);
        cm_audio_play_stream_context_uninit();
        return -2;
    }

    cm_psm_lock();
    osDelay(100);

    return 0;
}

int32_t cm_audio_player_stream_push(uint8_t *data, uint32_t size)
{
    if (playStreamCtx_p == NULL) {
        return -1;
    }
    if(g_stop_flag)
    {
        return -1;
    }
    CircularBuffer *buffer_p = &playStreamCtx_p->streamBuffer;
    int ret = writeCircularBuffer(buffer_p, data, size);
    if (get_streamCtx_state() == CM_AUDIO_PLAY_STREAM_STATE_INIT) {
        osSemaphoreRelease(playStreamCtx_p->startSemaphor);
    }
    return ret;
}

void cm_audio_player_stream_close(void)
{
    if (playStreamCtx_p == NULL) {
        return -1;
    }
    g_stop_flag = 1;
    osSemaphoreRelease(playStreamCtx_p->closeSemaphor);
}

void cm_audio_player_stream_clear_close(void)
{
    if (playStreamCtx_p == NULL) {
        return -1;
    }

    osSemaphoreRelease(playStreamCtx_p->clearCloseSemaphor);
}

void cm_audio_player_set_driver(cm_audio_play_driver_e driver_type)
{
    s_audio_hw_driver = driver_type;
}

cm_audio_play_driver_e cm_audio_player_get_driver()
{
    return s_audio_hw_driver;
}

void cm_audio_player_set_pa(cm_audio_pa_ctrl_e pa_type)
{
    s_audio_pa_ctrl = pa_type;
}

int cm_audio_player_set_volume(int vol)
{
    return cm_codec_set_volume(vol);
}

int cm_audio_player_get_volume(int* vol)
{
    return cm_codec_get_volume(vol);
}

int32_t cm_audio_play_set_cfg(cm_audio_play_cfg_type_e type, void *value)
{
    if(type == CM_AUDIO_PLAY_CFG_CHANNEL)
    {
        int channel = (int)(uintptr_t)value;
        if(channel < CM_AUDIO_PLAY_CHANNEL_RECEIVER || channel > CM_AUDIO_PLAY_CHANNEL_REMOTE)
        {
            return -1;
        }
        g_play_channel = channel;
        return 0;
    }
    else if(type == CM_AUDIO_PLAY_CFG_VOLUME)
    {
        int volume = (int)(uintptr_t)value;
        if(volume < 0 || volume > 100)
        {
            return -1;
        }
        return cm_audio_player_set_volume(volume);
    }
    return -1;
}

int32_t cm_audio_play_get_cfg(cm_audio_play_cfg_type_e type, void *value)
{
    if(type == CM_AUDIO_PLAY_CFG_CHANNEL)
    {
        value = (void *)(uintptr_t)g_play_channel;
        return 0;
    }
    else if(type == CM_AUDIO_PLAY_CFG_VOLUME)
    {
        int volume = 0;
        cm_codec_get_volume(&volume);
        value = (void *)(uintptr_t)volume;
        return 0;
    }
    else
    {
        return -1;
    }
}   


int32_t cm_audio_play(const void *data, uint32_t size, cm_audio_play_format_e format, cm_audio_sample_param_t *sample_param, cm_audio_play_callback_t cb, void *cb_param)
{
    if (playFileCtx_p != NULL || playStreamCtx_p != NULL) {
        MAP_LOG_E("audio busy");
        return -2;
    }
    cm_audio_play_buff_t  play_buff = {0};
    play_buff.buffer = data;
    play_buff.length = size;
    if(cm_audio_play_file_context_init(&play_buff, format, sample_param, cb, cb_param, 0) != 0)
    {
        MAP_LOG_E("cm_audio_play_file_context_init fail");
        return -3;
    }

    if (0 != cm_audio_play_file_start())
    {
        MAP_LOG_E("cm_audio_play_file_start fail");
        cm_audio_play_file_context_uninit();
        return -4;
    }

    cm_psm_lock();
    osDelay(100);

    MAP_LOG("success");
    return 0;
}

#endif
