
#ifndef __DRV_AMR_H__
#define __DRV_AMR_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    AMR_NB_CTX = 0,
    AMR_WB_CTX,
    MAX_AMR_CTX
}AMR_CtxId;

typedef enum 
{
	ZMMP_CODEC_AMR_NB_IF1, 
	ZMMP_CODEC_AMR_NB_IF2,
	ZMMP_CODEC_AMR_NB_IETF,
	ZMMP_CODEC_AMR_WB_IF1,
	ZMMP_CODEC_AMR_WB_IF2,
	ZMMP_CODEC_AMR_WB_IETF
} T_Mmp_CodecType;

typedef enum 
{
    ZMMP_AMR_ENC_MODE_0,     /* NB:4.75 kbit/s, WB:6.60 kbit/s  */
    ZMMP_AMR_ENC_MODE_1,     /* NB:5.15 kbit/s, WB:8.85 kbit/s  */	
    ZMMP_AMR_ENC_MODE_2,     /* NB:5.90 kbit/s, WB:12.65 kbit/s */
    ZMMP_AMR_ENC_MODE_3,     /* NB:6.70 kbit/s, WB:14.25 kbit/s */
    ZMMP_AMR_ENC_MODE_4,     /* NB:7.40 kbit/s, WB:15.85 kbit/s */
    ZMMP_AMR_ENC_MODE_5,     /* NB:7.95 kbit/s, WB:18.25 kbit/s */
    ZMMP_AMR_ENC_MODE_6,     /* NB:10.2 kbit/s, WB:19.85 kbit/s */
    ZMMP_AMR_ENC_MODE_7,     /* NB:12.2 kbit/s, WB:23.05 kbit/s */
    ZMMP_AMR_ENC_MODE_8,     /* NB:DTX,         WB:23.85 kbit/s */
    ZMMP_AMR_ENC_MODE_9,     /* NB:无效,        WB:DTX          */
    ZMMP_AMR_ENC_MODE_INVALID     /* NB:无效,   WB:无效          */
} T_Mmp_AmrEncMode;

typedef struct
{
	bool isDtxEnable;
    T_Mmp_CodecType codecType;
} T_DrvVoice_EncOpenArg;

typedef struct
{
	bool isWb;
	T_Mmp_AmrEncMode mode;
    uint16_t amrFrameSize;
	uint16_t pcmencsize;//NB:320 WB:640
	//const uint16_t *pPcmEnc;
    uint16_t *pPcmEnc;
	uint8_t *pAmrEnc;
} T_DrvVoice_AmrEncodeArg;

typedef struct
{
	bool isWb;;
    uint16_t pcmFrameSize;
    uint16_t amrdecsize; //NB:32 WB:61
	//const uint8_t *pAmrDec;
    uint8_t *pAmrDec;
	uint16_t *pPcmDec; 
} T_DrvVoice_AmrDecodeArg;

/**************************************************************************
* 函数声明区
**************************************************************************/


#define AMR_IOCTL_DECOPEN 		        1
#define AMR_IOCTL_ENCOPEN		        2
#define AMR_IOCTL_DECODE 		        3
#define AMR_IOCTL_ENCODE 		        4
#define AMR_IOCTL_DECODE_VOLTE 	        5
#define AMR_IOCTL_ENCODE_VOLTE 	        6
#define AMR_IOCTL_DECCLOSE 		        7
#define AMR_IOCTL_ENCCLOSE 		        8

/*  编解码接口函数指针  */
typedef void*  (*T_Mmp_AmrDecInit)(void);
typedef void   (*T_Mmp_AmrDecExit)(void*);
typedef void (*T_Mmp_AmrDecProc)(void*, const uint8_t*, int16_t*, int);
typedef void*  (*T_Mmp_AmrEncInit)(int);
typedef void   (*T_Mmp_AmrEncExit)(void*);
typedef int (*T_Mmp_AmrEncProc)(void*, int, const int16_t*, uint8_t*, int);

typedef struct
{
    T_Mmp_AmrDecInit decInit;
    T_Mmp_AmrDecExit decExit;
    T_Mmp_AmrDecProc decProc;
} T_Mmp_AmrDecHandle;

typedef struct
{
    bool             isDtxEnable;      /*  是否支持DTX  */
    T_Mmp_AmrEncInit encInit;
    T_Mmp_AmrEncExit encExit;
    T_Mmp_AmrEncProc encProc;
} T_Mmp_AmrEncHandle;

typedef struct
{
    bool isWb;
    T_Mmp_CodecType codecType;
    void             *pCodecState;      /*开源代码内部数据结构，用来存放当前编解码内部数据*/
    bool         isDtxEnable;      /*是否支持DTX*/
    void      *In;
    void      *Out;   
    union
    {
        T_Mmp_AmrDecHandle decHandle;
        T_Mmp_AmrEncHandle encHandle;
    } handle;
} T_Mmp_AmrCodecContext;

struct  ict_amr_cfg
{
    T_Mmp_AmrCodecContext *dec_codec_ctx[MAX_AMR_CTX];
    T_Mmp_AmrCodecContext *enc_codec_ctx[MAX_AMR_CTX];
};

struct ict_amr 
{
    struct ict_amr_cfg *cfg;
};

int8_t ict_amr_dec_open(AMR_CtxId ctxId, T_Mmp_CodecType codecType);
int8_t ict_amr_enc_open(AMR_CtxId ctxId, T_DrvVoice_EncOpenArg *open_arg);
int8_t ict_amr_dec_proc(AMR_CtxId ctxId, T_DrvVoice_AmrDecodeArg *dec_arg);
int8_t ict_amr_enc_proc(AMR_CtxId ctxId, T_DrvVoice_AmrEncodeArg *enc_arg);
int8_t ict_amr_enc_close(AMR_CtxId ctxId);
int8_t ict_amr_dec_close(AMR_CtxId ctxId);

#endif


