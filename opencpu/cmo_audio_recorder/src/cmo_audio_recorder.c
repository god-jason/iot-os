/**
 * @file        cmo_audio_recorder.c
 * @brief       Audio recorder 接口
 * @copyright   Copyright © 2025 China Mobile IOT. All rights reserved.
 * @author      By ZY
 * @date        2025/4/18
 */
#include "cm_codec_driver.h"
#include "cm_audio_recorder.h"

#ifdef CM_AUDIO_RECORDER_ENABLE

#ifdef CM_AUDIO_RECORDER_AMR_ENABLE
#include "amr.h"
#endif

#if defined(USE_MOBVOI_DSP)
#define AUDIO_RECORD_SAMPLE_RATE_16000HZ_FRAME_SIZE    640
#define AUDIO_RECORD_SAMPLE_RATE_8000HZ_FRAME_SIZE     320
#endif
#define DEFAULT_FRAMESIZE 320
#define AUDIO_RECORD_TASK_STACK_SIZE    (20*1024)
#define AUDIO_RECORD_TASK_PRIORITY  osPriorityNormal

#define AUREC_LOG(fmt, ...)     app_printf("[MREC]%s %u: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define AUREC_LOG_E(fmt, ...)   app_printf("[MREC_E]%s %u: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

typedef struct {
    void *userParam;
    cm_audio_record_callback_t cb;
    osSemaphoreId_t stopSemaphor;
    cm_audio_record_format_e fmt;
#if defined(USE_MOBVOI_DSP)
    cm_audio_sample_param_t sampleParam;
#endif
} cm_audio_recorder_context_t;

static cm_audio_recorder_context_t *recorderCtx_p = NULL;

static int recorder_context_init(void)
{
    if (recorderCtx_p != NULL)
        return -1;

    recorderCtx_p = (cm_audio_recorder_context_t *)cm_malloc(sizeof(cm_audio_recorder_context_t));
    if (recorderCtx_p == NULL)
        return -1;

    recorderCtx_p->stopSemaphor = osSemaphoreNew(1, 0, NULL);

    return 0;
}

static void recorder_context_uninit(void)
{
    osSemaphoreDelete(recorderCtx_p->stopSemaphor);
    cm_free(recorderCtx_p);
    recorderCtx_p = NULL;
}

void audio_recorder_work_task(void *param)
{
    if (recorderCtx_p == NULL) {
        return;
    }
    cm_audio_state_set(1);
    cm_audio_record_callback_t recordCb = recorderCtx_p->cb;
    osSemaphoreId_t stopSemaphor = recorderCtx_p->stopSemaphor;

    uint32_t frame_size = DEFAULT_FRAMESIZE;
#ifdef CM_AUDIO_RECORDER_AMR_ENABLE
    T_DrvVoice_AmrEncodeArg *amr_enc_arg_ptr = NULL;
    uint8_t amr_header_flag = 1;
#endif
#if defined(USE_MOBVOI_DSP)
    if (cm_audio_get_mobvoi()) {
        if (recorderCtx_p->sampleParam.rate == CM_AUDIO_SAMPLE_RATE_16000HZ) {
            frame_size = AUDIO_RECORD_SAMPLE_RATE_16000HZ_FRAME_SIZE;
        }
        else if (recorderCtx_p->sampleParam.rate == CM_AUDIO_SAMPLE_RATE_8000HZ) {
            frame_size = AUDIO_RECORD_SAMPLE_RATE_8000HZ_FRAME_SIZE;
        }
    }
#endif
    cm_audio_record_data_t msg = {0};
    msg.fmt = recorderCtx_p->fmt;
    msg.user =recorderCtx_p->userParam;
    uint8_t *data = (uint8_t *)cm_calloc(1, frame_size);
    if (data == NULL) {
        AUREC_LOG_E("data NULL");
        if (recordCb != NULL)
        {
            recordCb(CM_AUDIO_RECORD_EVENT_INTERRUPT, &msg);
        }
        recorder_context_uninit();
        cm_codec_stop(CODEC_RECORD_MODE);
        cm_codec_uninit();
        cm_psm_unlock();
        cm_audio_state_set(0);
        return;
    }
    if(recorderCtx_p->fmt >= CM_AUDIO_RECORD_FORMAT_AMRNB_475 && recorderCtx_p->fmt <= CM_AUDIO_RECORD_FORMAT_AMRNB_1220)
    {
#ifdef CM_AUDIO_RECORDER_AMR_ENABLE
        T_DrvVoice_EncOpenArg enc_open_arg = {0};
        enc_open_arg.isDtxEnable = 0;
        enc_open_arg.codecType = ZMMP_CODEC_AMR_NB_IETF;
        int ret = ict_amr_enc_open(AMR_NB_CTX, &enc_open_arg);
        if(ret < 0)
        {
            AUREC_LOG_E("ret[%d]", ret);
            if (recordCb != NULL)
            {
                recordCb(CM_AUDIO_RECORD_EVENT_INTERRUPT, &msg);
            }
            recorder_context_uninit();
            cm_codec_stop(CODEC_RECORD_MODE);
            cm_codec_uninit();
            cm_psm_unlock();
            cm_audio_state_set(0);
            return;
        }
        amr_enc_arg_ptr = (T_DrvVoice_AmrEncodeArg *)os_malloc(sizeof(T_DrvVoice_AmrEncodeArg));
        memset(amr_enc_arg_ptr, 0 , sizeof(T_DrvVoice_AmrEncodeArg));
        amr_enc_arg_ptr->pcmencsize = DEFAULT_FRAMESIZE;
        amr_enc_arg_ptr->pPcmEnc = (uint16_t *)os_malloc(DEFAULT_FRAMESIZE);
        amr_enc_arg_ptr->isWb = 0;
        amr_enc_arg_ptr->pAmrEnc = (os_uint8_t *)os_malloc(64);
        amr_enc_arg_ptr->mode = recorderCtx_p->fmt - 4;
#endif
    }

    AUREC_LOG("rec task");
    while(true) {
        if (osSemaphoreAcquire(stopSemaphor, cm_ms_to_tick(5)) == osOK) {
            break;
        }
        int ret = cm_codec_readData(data, frame_size);
        if(ret == 0) {
            if(recorderCtx_p->fmt >= CM_AUDIO_RECORD_FORMAT_AMRNB_475 && recorderCtx_p->fmt <= CM_AUDIO_RECORD_FORMAT_AMRNB_1220)
            {
#ifdef CM_AUDIO_RECORDER_AMR_ENABLE
                for (uint32_t i = 0; i <  DEFAULT_FRAMESIZE / 2; i++)
                {
                    const uint8_t* in = &(data[2*i]);
                    amr_enc_arg_ptr->pPcmEnc[i] = in[0] | (in[1] << 8);
                }
                ict_amr_enc_proc(AMR_NB_CTX, amr_enc_arg_ptr);
                if(amr_header_flag)
                {
                    amr_header_flag = 0;
                    os_uint8_t header_buff[64] = {0};
                    memcpy (header_buff, "#!AMR\n", strlen("#!AMR\n"));
                    memcpy (header_buff + strlen("#!AMR\n"), amr_enc_arg_ptr->pAmrEnc, amr_enc_arg_ptr->amrFrameSize);
                    memcpy (amr_enc_arg_ptr->pAmrEnc , header_buff, amr_enc_arg_ptr->amrFrameSize + strlen("#!AMR\n"));
                    amr_enc_arg_ptr->amrFrameSize += strlen("#!AMR\n");
                }
                msg.data = amr_enc_arg_ptr->pAmrEnc;
                msg.len = amr_enc_arg_ptr->amrFrameSize;
#endif
            }
            else
            {
                msg.data = data;
                msg.len = DEFAULT_FRAMESIZE;
            }
            

            if (recordCb != NULL)
            {
                recordCb(CM_AUDIO_RECORD_EVENT_DATA, &msg);
            }
        }
    }

    if(recorderCtx_p->fmt >= CM_AUDIO_RECORD_FORMAT_AMRNB_475 && recorderCtx_p->fmt <= CM_AUDIO_RECORD_FORMAT_AMRNB_1220)
    {
#ifdef CM_AUDIO_RECORDER_AMR_ENABLE
        ict_amr_enc_close(AMR_NB_CTX);
        os_free(amr_enc_arg_ptr->pPcmEnc);
        os_free(amr_enc_arg_ptr->pAmrEnc);
#endif
    }
    cm_free(data);
    recorder_context_uninit();
    cm_codec_stop(CODEC_RECORD_MODE);
    cm_codec_uninit();

    if (recordCb != NULL)
    {
        recordCb(CM_AUDIO_RECORD_EVENT_FINISHED, &msg);
    }
    cm_audio_state_set(0);
    cm_psm_unlock();
}

int32_t cm_audio_recorder_start(cm_audio_record_format_e format, cm_audio_sample_param_t *sample_param, cm_audio_record_callback_t cb, void *cb_param)
{
    int ret = 0;

    do
    {
        if (0 != cm_audio_state_get())
        {
            AUREC_LOG_E("audio busy");
            ret = -2;
            break;
        }
        ret = recorder_context_init();
        if (ret < 0)
        {
            AUREC_LOG_E("recorder_context_init ret[%d]", ret);
            break;
        }

        recorderCtx_p->userParam = cb_param;
        recorderCtx_p->cb = cb;
        recorderCtx_p->fmt = format;
#if defined(USE_MOBVOI_DSP)
        if (format == CM_AUDIO_RECORD_FORMAT_PCM && sample_param != NULL) {
            recorderCtx_p->sampleParam = *sample_param;
        }

        if ((cm_audio_get_mobvoi()) && (sample_param->rate == CM_AUDIO_SAMPLE_RATE_16000HZ))
        {
            cm_codec_init_with_mobvoi(sample_param->rate, \
                          sample_param->sample_format, \
                          AUDIO_RECORD_SAMPLE_RATE_16000HZ_FRAME_SIZE, \
                          NULL);
        }
        else if ((cm_audio_get_mobvoi()) && (sample_param->rate == CM_AUDIO_SAMPLE_RATE_8000HZ))
        {
            cm_codec_init_with_mobvoi(sample_param->rate, \
                          sample_param->sample_format, \
                          AUDIO_RECORD_SAMPLE_RATE_8000HZ_FRAME_SIZE, \
                          NULL);
        }
        else
        {
            cm_codec_init(sample_param->rate, \
                          sample_param->sample_format, \
                          DEFAULT_FRAMESIZE, \
                          NULL);
        }
#else
        ret = cm_codec_init(sample_param->rate, \
                      sample_param->sample_format, \
                      DEFAULT_FRAMESIZE, \
                      NULL);
        if (ret < 0)
        {
            AUREC_LOG_E("cm_codec_init ret[%d]", ret);
            break;
        }
#endif

        ret = cm_codec_start(CODEC_RECORD_MODE);
        if (ret < 0)
        {
            AUREC_LOG_E("cm_codec_start ret[%d]", ret);
            break;
        }

        cm_thread_id_t task_id = cm_thread_create("audio record task", \
                          AUDIO_RECORD_TASK_STACK_SIZE, \
                          AUDIO_RECORD_TASK_PRIORITY, \
                          audio_recorder_work_task);
        if (task_id == NULL)
        {
            AUREC_LOG_E("cm_thread_create NULL");
            ret = -2;
            break;
        }
    }while (0);
    if (ret != 0)
    {
        AUREC_LOG_E("ret[%d]", ret);
        ret = -3;
    }

    if (ret == 0)
    {
        cm_psm_lock();
    }

    return ret;
}



void cm_audio_recorder_stop(void)
{
    if(recorderCtx_p == NULL)
    {
        AUREC_LOG_E("recorderCtx_p NULL");
        return;
    }
    osSemaphoreRelease(recorderCtx_p->stopSemaphor);
}

#if defined(USE_MOBVOI_DSP)
void cm_audio_enable_mobvoi(bool on_off)
{
    cm_audio_set_mobvoi(on_off);
}
#endif

#endif