
/**  @file
  ycom_adc.h

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
                
#ifndef _YOPEN_ADC_H
#define _YOPEN_ADC_H
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

/****************************  error code about ADC    ***************************/
typedef enum
{
	YOPEN_ADC_SUCCESS  			= 0,
    YOPEN_ADC_INVALID_PARAM_ERR    = 10  | (YOPEN_COMPONENT_BSP_ADC << 16),
	YOPEN_ADC_GET_VALUE_ERROR 		= 50  | (YOPEN_COMPONENT_BSP_ADC << 16),
	YOPEN_ADC_MEM_ADDR_NULL_ERROR	= 60  | (YOPEN_COMPONENT_BSP_ADC << 16),
	YOPEN_ADC_TASK_ERROR,
    YOPEN_ADC_OTHER
} yopen_errcode_adc_e;

typedef enum
{
	YOPEN_ADC0_CHANNEL,
    YOPEN_ADC1_CHANNEL,
	YOPEN_ADC_TEM_CHANNEL,
	YOPEN_ADC_VBAT_CHANNEL,
	YOPEN_ADC_CHANNEL_MAX,
} yopen_adc_chan_id;
/* List of ADC0/1 resdiv select options */
typedef enum
{
	YOPEN_ADC_AIO_RESDIV_RATIO_1          = 0U,  /**< ADC AIO RESDIV select as VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_14OVER16   = 1U,  /**< ADC AIO RESDIV select as 14/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_12OVER16   = 2U,  /**< ADC AIO RESDIV select as 12/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_10OVER16   = 3U,  /**< ADC AIO RESDIV select as 10/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_8OVER16   = 4U,  /**< ADC AIO RESDIV select as 8/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_7OVER16   = 5U,  /**< ADC AIO RESDIV select as 7/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_6OVER16    = 6U,  /**< ADC AIO RESDIV select as 6/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_5OVER16    = 7U,  /**< ADC AIO RESDIV select as 5/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_4OVER16    = 8U,  /**< ADC AIO RESDIV select as 4/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_3OVER16    = 9U,  /**< ADC AIO RESDIV select as 3/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_2OVER16    = 10U, /**< ADC AIO RESDIV select as 2/16 VIN */
    YOPEN_ADC_AIO_RESDIV_RATIO_1OVER16    = 11U, /**< ADC AIO RESDIV select as 1/16 VIN */
    YOPEN_ADC_AIO_RESDIV_BYPASS           = 12U, /**< BYPASS the whole ADC AIO RESDIV network(direct input) */
}YOPEN_AdcAioResDiv_e;

/*List of VBAT resdiv select options */
typedef enum
{
    YOPEN_ADC_VBAT_RESDIV_RATIO_8OVER32     = 0U,  /**< ADC AIO RESDIV select as 8/32 VBAT */
    YOPEN_ADC_VBAT_RESDIV_RATIO_7OVER32     = 1U,  /**< ADC AIO RESDIV select as 7/32 VBAT */
    YOPEN_ADC_VBAT_RESDIV_RATIO_6OVER32     = 2U,  /**< ADC AIO RESDIV select as 6/32 VBAT */
    YOPEN_ADC_VBAT_RESDIV_RATIO_5OVER32     = 3U,  /**< ADC AIO RESDIV select as 5/32 VBAT */
   	YOPEN_ADC_VBAT_RESDIV_RATIO_4OVER32     = 4U,  /**< ADC AIO RESDIV select as 4/32 VBAT */
    YOPEN_ADC_VBAT_RESDIV_RATIO_3OVER32     = 5U,  /**< ADC AIO RESDIV select as 3/32 VBAT */
    YOPEN_ADC_VBAT_RESDIV_RATIO_2OVER32     = 6U,  /**< ADC AIO RESDIV select as 2/32 VBAT */
    YOPEN_ADC_VBAT_RESDIV_RATIO_1OVER32     = 7U,  /**< ADC AIO RESDIV select as 1/32 VBAT */
} YOPEN_AdcVbatResdiv_e;

/*===========================================================================
 * Struct
 ===========================================================================*/

typedef struct
{
    uint32_t reqhandle;
    uint32_t request;
}yopen_atadc_msg;

/*===========================================================================
 * FUNCTION
 ===========================================================================*/


/**
 * @brief 电压采集接口
 * 
 * @param yopen_adc_channel_id ADC通道
 * @param adc_value         采集的电压
 * @return yopen_errcode_adc_e 
 */
extern yopen_errcode_adc_e yopen_adc_get_volt
(
    yopen_adc_chan_id yopen_adc_channel_id,
    int *adc_value
);


/**
 * @brief 电压采集接口，可设置分压系数
 * 
 * @param yopen_adc_channel_id    ADC通道
 * @param yopen_adc_div           分压系数
 * @param adc_value            采集的电压
 * @return yopen_errcode_adc_e 
 */
extern yopen_errcode_adc_e yopen_adc_get_volt_raw
(
    yopen_adc_chan_id yopen_adc_channel_id,
    YOPEN_AdcAioResDiv_e yopen_adc_div,
    int *adc_value
);

#ifdef __cplusplus
} /*"C" */
#endif

#endif




