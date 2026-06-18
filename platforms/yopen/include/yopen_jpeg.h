
/**  @file
  yopen_jpeg.h

  @brief
  TODO

*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/
                
#ifndef _YOPEN_JPEG_H
#define _YOPEN_JPEG_H
/*> include header files here*/

#include <stdio.h>
#include "yopen_api_common.h"
#include "yopen_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================
*  Enumeration Definition
*========================================================================*/

/****************************  error code about JPEG    ***************************/


/*===========================================================================
 * Struct
 ===========================================================================*/

typedef enum 
{
    YOPEN_JPEG_COLOR_FMT_Y = 0,
    YOPEN_JPEG_COLOR_FMT_YUV420P,
    YOPEN_JPEG_COLOR_FMT_YUYV,
    YOPEN_JPEG_COLOR_FMT_YUV444,
    YOPEN_JPEG_COLOR_FMT_RGB565,
    YOPEN_JPEG_COLOR_FMT_MAX
}YOPEN_JPEG_COLOR_FMT;


typedef struct 
{
    unsigned short	uWidth;
    unsigned short	uHeight;
    unsigned short	uEdgedWidth;
    unsigned short	uEdgedHeight;	
}YOPEN_JPEG_INFO;


typedef struct 
{
    YOPEN_JPEG_COLOR_FMT  eFmt;
    
    unsigned int    uWidth;
    unsigned int    uHeight;

    void           *pData[3];
}YOPEN_JPEG_IMAGE_BUF;


typedef struct 
{
    YOPEN_JPEG_COLOR_FMT  eFmt;

    unsigned int    uWidth;
    unsigned int    uHeight;
    unsigned int    uQuality;//1~100

}YOPEN_JPEG_ENC_PARAM;

/*===========================================================================
 * FUNCTION
 ===========================================================================*/

/*
*brief      create jpeg decoder handle
*
*param_in   none
*
*return     non-NULL, jpeg handle
*           NULL, error
*/
void *yopen_jpegD_create();


/*
*brief      destroy jpeg decoder handle
*
*param_in   hJpeg, jpeg handle
*
*return     none
*/
void yopen_jpegD_destroy(void *hJpeg);


/*
*brief      set jpeg data, decode jpeg head
*
*param_in   hJpeg, jpeg handle
*           pData, jpeg data pointer
*           uDataLen, jpeg data length
param_out   pInfo, jpeg image size
*
*return     0, success
*           <0, error
*/
int yopen_jpegD_decode_info(void *hJpeg,unsigned char *pData,unsigned int uDataLen,YOPEN_JPEG_INFO *pInfo);


/*
*brief      decode jpeg data
*
*param_in   hJpeg, jpeg handle
*           pIbuf, jpeg image buffer
*
*return     0, success
*           <0, error
*/
int yopen_jpegD_decode_image(void *hJpeg,YOPEN_JPEG_IMAGE_BUF *pIbuf);


/*
*brief      decode jpeg slice
*
*param_in   hJpeg, jpeg handle
*           pIbuf, jpeg image buffer, widthx16
*           pPosY, current image slice pos
*
*return     0, success
*           <0, error
*/
int yopen_jpegD_decode_line(void *hJpeg,YOPEN_JPEG_IMAGE_BUF *pIbuf,unsigned int *pPosY);



/*
*brief      create jpeg encoder handle
*
*param_in   none
*
*return     non-NULL, jpeg handle
*           NULL, error
*/
void *yopen_jpegE_create();


/*
*brief      destroy jpeg encoder handle
*
*param_in   hJpeg, jpeg handle
*
*return     none
*/
void yopen_jpegE_destroy(void *hJpeg);


/*
*brief      optional, set encoder parameter
*
*param_in   hJpeg, jpeg handle
*           pParam, encoder parameter
param_out   pSuggBufLen, suggested data buffer size
*
*return     0, success
*           <0, error
*/
int yopen_jpegE_set_param(void *hJpeg,YOPEN_JPEG_ENC_PARAM *pParam,unsigned int *pSuggBufLen);


/*
*brief      encode jpeg image
*
*param_in   hJpeg, jpeg handle
*           pIbuf, yuv image data
*           pBuf, data buffer pointer
*           pBufLen, data buffer size
param_out   pBufLen, actual data len
*
*return     0, success
*           <0, error
*/
int yopen_jpegE_encode(void *hJpeg,YOPEN_JPEG_IMAGE_BUF *pIbuf,unsigned char *pBuf,unsigned int *pBufLen);


/*
*brief      set log level
*
*param_in   uLevel, log level
*
*return     none
*/
void yopen_jpegE_set_log_level(unsigned int uLevel);


#ifdef __cplusplus
} /*"C" */
#endif

#endif




