#ifndef __CMO_LBS_H__
#define __CMO_LBS_H__


/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define CM_LBS_NVCONFIG_ONEOSPOS_PIDLEN (64)
#define CM_LBS_NVCONFIG_AMAP_KEYLEN (128)
#define CM_LBS_NVCONFIG_AMAP_SIGNALEN (128)



/****************************************************************************
 * Public Types
 ****************************************************************************/
/**LBS平台枚举*/
typedef enum
{
    CM_LBS_PLAT_START,
    CM_LBS_PLAT_AMAP10 = 10,             /*!<高德定位1.0,不支持*/
    CM_LBS_PLAT_AMAP20 = 11,             /*!<高德定位2.0,不支持*/
    CM_LBS_PLAT_MANTU = 20,              /*!<漫图定位,不支持*/
    CM_LBS_PLAT_BAIDU = 30,              /*!<百度定位,不支持*/
    CM_LBS_PLAT_ONEOSPOS= 40,            /*!<OneOsPos定位  */
    CM_LBS_PLAT_END
} cm_lbs_location_platform_e;

/**LBS定位错误信息*/
typedef enum
{
    CM_LBS_LOCATION_OK = 0,               /*!<定位成功*/
    CM_LBS_NET_ERR = -1,                  /*!<网络异常*/
    CM_LBS_TIMEOUT = -2,                  /*!<请求超时*/
    CM_LBS_KEY_ERR = -3,                  /*!<KEY非法或过期*/
    CM_LBS_PARAM_INVALID = -4,            /*!<请求参数无效*/
    CM_LBS_OVER_DAYQUTA = -5,             /*!<请求超出日配额*/
    CM_LBS_OVER_QUOTA = -6,               /*!<请求超出总配额*/
    CM_LBS_OVER_QPS = -7,                 /*!<请求超出QPS*/
    CM_LBS_UNKNOW_PID = -8,                 /*!<请求超出QPS*/
    CM_LBS_UNKNOW_ERR = -100              /*!<未知错误*/
} cm_lbs_callback_event_e;

/**LBS定位结果*/
typedef struct
{
    int platform;                        /*!<定位所用平台*/
    cm_lbs_callback_event_e state;       /*!<定位结果状态*/
    char *longitude;                     /*!<经度*/
    char *latitude;                      /*!<纬度*/
    char *radius;                        /*!<精度半径*/
    char *country;                       /*!<国家*/
    char *province;                      /*!<省份*/
    char *city;                          /*!<市*/
    char *citycode;                      /*!<城市编码*/
    char *adcode;                        /*!<区域编码*/
    char *poi;                           /*!<道路名称*/
    char *location_describe;             /*!<具体位置描述*/
}cm_lbs_location_rsp_t;

/**LBS高德定位版本枚举*/


/**高德智能硬件定位配置参数*/
typedef struct
{
    uint8_t *api_key;                   /*!<apikey*/
    int time_out;                       /*!<超时时间 sec*/
    int digital_sign_enable;            /*!<是否启动数字签名*/
    uint8_t *digital_key;               /*!<数字签名key*/
    int show_fields_enable;             /*!<高德定位2.0是否请求具体的位置描述 0：不请求 1：请求*/
    int nearbts_enable;                 /*!<是否启用邻区*/
}cm_lbs_amap_location_attr_t;

/**漫图定位配置参数*/
typedef struct
{
    uint8_t appid[64];                  /*!<应用编码 由onelink平台提供 */
    uint8_t password[64];               /*!<api接入密码 由中国移动物联网onelink平台提供*/
    uint8_t transid[64];                /*!<事务编码 由客户按照 APPID+YYYYMMDDHHMISS+8位数字序列自主生成*/
    int time_out;                       /*!<超时时间 sec*/

}cm_lbs_mantu_location_attr_t;


/*ONEOS 定位配置信息*/
typedef struct
{
  char *pid;                           /*!<设备pid*/
  int time_out;                        /*!<请求超时时间*/
  int nearbts_enable;                  /*!<是否启用邻区*/
}cm_lbs_oneospos_attr_t;



typedef struct
{
   char oneospospid[CM_LBS_NVCONFIG_ONEOSPOS_PIDLEN];
   char amapv1key[CM_LBS_NVCONFIG_AMAP_KEYLEN];
   char amapv1signa[CM_LBS_NVCONFIG_AMAP_SIGNALEN];
   char amapv2key[CM_LBS_NVCONFIG_AMAP_KEYLEN];
   char amapv2signa[CM_LBS_NVCONFIG_AMAP_SIGNALEN];
   int amapv1signa_enable;
   int amapv2signa_enable;
}cm_lbs_nvconfig_t;



/**LBS定位回调函数*/
typedef void (*cm_lbs_callback)(cm_lbs_callback_event_e event,cm_lbs_location_rsp_t *location,void *cb_arg);


/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif
/*
*@brief LBS初始化
*@param[in] platform 定位平台
*@param[in] attr 定位配置参数
*
*@return
*       0：成功\n
*      -1：参数错误\n
*      -2：未知错误
*/
int32_t cm_lbs_init(cm_lbs_location_platform_e platform,void *attr);

/*
*@brief LBS获取位置信息(异步)
*@param[in] cb 获取结果的回调函数
*@param[in] cb_arg 回调函数参数
*
*@return
*        0：成功 n
*		-1：参数错误\n
*		-2：未知错误\n
*		-3：LBS忙
*/
int32_t cm_lbs_location(cm_lbs_callback cb,void *cb_arg);

/*
*@brief LBS逆初始化
*
*/
void cm_lbs_deinit(void);

/*
*@brief 获取平台定位配置信息
*
*@param[in] platform 定位平台
*@param[out] attr 不同平台配置参数结构体
*
*@return
*        0:成功\n
*       -1：参数错误\n
*       -2：未知错误\n
*       -3：数据为空\n
*/
int32_t cm_lbs_get_attr(int platform,void *attr);


#undef EXTERN
#ifdef __cplusplus
}
#endif


#endif /* __CM_LBS_H__ */

/** @}*/
