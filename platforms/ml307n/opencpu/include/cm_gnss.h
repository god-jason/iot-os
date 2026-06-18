/**
 * @file        cm_gnss.h
 * @brief       GNSS接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By zyf
 * @date        2021/07/30
 *
 * @defgroup gnss
 * @ingroup PI
 * @{
 */

#ifndef __CM_GNSS_H__
#define __CM_GNSS_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <time.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/**NMEA 协议信息输出类型宏定义，可参照格式自行添加*/
#define CM_GNSS_NAME_GGA (1 << 0)
#define CM_GNSS_NAME_GSV (1 << 1)
#define CM_GNSS_NAME_GSA (1 << 2)
#define CM_GNSS_NAME_RMC (1 << 3)
#define CM_GNSS_NAME_VTG (1 << 4)
#define CM_GNSS_NAME_GLL (1 << 5)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** gnss output mode */
typedef enum
{
    CM_AGNSS_DISABLE = 1,
    CM_AGNSS_ENABLE,
} cm_agnss_enable_e;

/** gnss satellite system */
typedef enum
{
    CM_GNSS_BDS = 1,
    CM_GNSS_GPS,
    CM_GNSS_BDS_GPS,
    CM_GNSS_GLONASS,
    CM_GNSS_BDS_GLONASS,
    CM_GNSS_GPS_GLONASS,
    CM_GNSS_BDS_GPS_GLONASS,
} cm_gnss_type_e;

typedef struct
{
    float latitude;  /** latitude,单位度， 保留4位小数 */
    float longitude; /** longitude,单位度，保留4位小数 */
    uint8_t mode;    /** 辅助位置模式，整型，0-单次有效(使用之后清楚）;1-单次有效(使用之后清除）;2-自动更新(允许根据位置信息自动更新） */
} cm_agnss_location_t;

/** gnss config type */
typedef enum
{
    CM_GNSS_CONFIG_TYPE_NMEA_MASK = 1,  /** NMEA输出配置(0-63)，设置值(value)对应的bit置1表示开启对应输出
                                               bit0: GGA;bit1: GSV;bit2: GSA;bit3: RMC;bit4: VTG;bit5: GLL；uint32_t */
    CM_GNSS_CONFIG_TYPE_NMEA_CYCLE,     /** NMEA输出周期（1-60）秒，当为0时只能查询，整型*/
    CM_GNSS_CONFIG_TYPE_NMEA_PORT,      /** 调整NMEA数据输出接口（字符串） */
    CM_GNSS_CONFIG_TYPE_AGNSS_LOCATION, /** AGNSS辅助定位注入位置信息设置,cm_agnss_location_t */
    CM_GNSS_CONFIG_TYPE_AGNSS_TIME,     /** AGNSS辅助定位注入时间信息设置,struct tm,UTC time */
    CM_GNSS_CONFIG_TYPE_AGNSS_URL,      /** AGNSS辅助定位服务器url，字符串（hostname:port,≤64字节） */
    CM_GNSS_CONFIG_TYPE_AGNSS_INTERVAL, /** AGNSS辅助定位数据自动更新间隔，（30-240）分钟，整型,默认值为0（禁止自动更新） */
    CM_GNSS_CONFIG_TYPE_DGNSS_URL,      /** DGNSS差分定位服务器url，字符串（hostname:port，≤64字节） */
    CM_GNSS_CONFIG_TYPE_DGNSS_USER,     /** DGNSS差分定位用户名，字符串（≤64字节） */
    CM_GNSS_CONFIG_TYPE_DGNSS_PWD,      /** DGNSS差分定位密码，字符串（≤64字节） */
    CM_GNSS_CONFIG_TYPE_DGNSS_MOUNT,    /** DNGSS差分定位挂载点，字符串（≤64字节） */
    CM_GNSS_CONFIG_TYPE_DGNSS_INTERVAL, /** DGNSS差分定位数据接收间隔，秒，整型,默认值为1（>=1） */
    CM_GNSS_CONFIG_TYPE_GNSS_CTRL,      /** GNSS定位配置,0:连续定位,1:单次定位,默认0 */
} cm_gnss_config_type_e;

typedef enum
{
    CM_GNSS_RESET_AUTO = 0, /** 自动切换为最佳模式，客户不知怎么设置时，建议选择此模式 */
    CM_GNSS_RESET_COOL,     /** 冷启动 */
    CM_GNSS_NMEA_WARM,      /** 温启动  */
    CM_GNSS_NMEA_HOT,       /** 热启动 */
} cm_gnss_reset_mode_e;

typedef struct
{
    struct tm date_time; /** UTC time */
    float latitude;      /** latitude,单位度 */
    float longitude;     /** longitude,单位度 */
    float hdop;          /** 水平精度因子，保留1位小数 */
    float altitude;      /** 海拔高度，单位米，保留1位小数 */
    float cog;           /** 运动角度，真北参照系，单位:度 */
    float spkm;          /** 水平运动速度，单位Km/h */
    float spkn;          /** 水平运动速度，单位Knots */
    uint8_t nsat;        /** 参与定位的卫星数 */
    uint8_t dtype;       /** 差分定位标识 */
    uint8_t fix;         /** 定位类型。0:未定位，2:2D定位，3:3D定位 */
} cm_gnss_location_info_t;

typedef enum
{
    CM_AGNSS_UPDATE_INVALID = 0, /** 辅助信息无效 */
    CM_AGNSS_UPDATE_VALID,       /** 辅助信息有效 */
    CM_AGNSS_UPDATE_FAIL_JUDGE,  /** 无法判断有效性  */
    CM_AGNSS_UPDATE_VOID,        /** 辅助数据为空 */
} cm_agnss_update_result_e;

/**
 * @brief NMEA信息回调函数
 *
 * @param [in] type  NMEA 报文类型(例如：CM_GNSS_NAME_GGA | CM_GNSS_NAME_GSV)
 * @param [in] nmea  NMEA 报文数据
 * @param [in] len   NMEA 报文长度
 *
 * @return void
 *
 * @details  须在注册回调函数时传入
 */
typedef void (*cm_gnss_nmea_callback)(uint32_t type, const char *nmea, uint32_t len);

/**
 * @brief 辅助信息更新结果回调函数
 *
 * @param [in] mode        辅助数据更新结果：0-辅助信息无效；1-辅助信息有效；2-无法判断有效性；3-辅助数据为空；
 * @param [in] update_time 辅助数据更新时间，年月日时分秒，各占2个字符
 * @param [in] size        辅助数据大小
 *
 * @return void
 *
 * @details  须在注册回调函数时传入
 */
typedef void (*cm_agnss_data_callback)(cm_agnss_update_result_e mode, const char *update_time, uint32_t size);

/**
 * @brief 发送透传信息给GPS芯片返回结果回调函数
 *
 * @param [in] data 返回结果数据
 * @param [in] len  返回结果数据长度
 *
 * @return void
 *
 * @details  须在注册回调函数时传入
 */
typedef void (*cm_gnss_rawdata_rsp_callback)(const char *data, uint32_t len);

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/
#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/**
 *  @brief gnss 功能打开
 *
 *  @param [in] gnss_type gnss 导航系统选择
 *  @param [in] agnss_enable   是否打开辅助定位
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 */
int32_t cm_gnss_open(cm_gnss_type_e gnss_type, cm_agnss_enable_e agnss_enable);

/**
 *  @brief gnss 功能关闭
 *
 *  @param [in] void
 *
 *  @return void
 */
void cm_gnss_close(void);

/**
 *  @brief gnss 参数信息配置
 *
 *  @param [in] config_type 参数类型
 *  @param [in] value       相应参数值
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 */

int32_t cm_gnss_config(cm_gnss_config_type_e config_type, void *value);

/**
 *  @brief gnss 注册NMEA 消息回调函数
 *
 *  @param [in] callback  回调函数设置
 *  @param [in] nmea_type 输出报文类型(例如：CM_GNSS_NAME_GGA | CM_GNSS_NAME_GSV)
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 *
 */
int32_t cm_gnss_set_nmea_callback(cm_gnss_nmea_callback callback, uint32_t nmea_type);

/**
 *  @brief gnss 重启
 *
 *  @param [in] mode 重启模式
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 *
 */
int32_t cm_gnss_reset(cm_gnss_reset_mode_e mode);

/**
 *  @brief 查询辅助定位数据是否有效
 *
 *  @param [out] mode        辅助数据更新结果：0-辅助信息无效；1-辅助信息有效；2-无法判断有效性；3-辅助数据为空；
 *  @param [out] update_time 辅助数据更新时间，年月日时分秒，各占2个字符
 *  @param [out] size        辅助数据大小
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 */
int32_t cm_agnss_state_query(cm_agnss_update_result_e *agnss_mode, char *update_time, uint32_t *data_size);

/**
 *  @brief 更新辅助定位数据
 *
 *  @param [in] callback 更新结果回调函数
 *
 *  @return
 *    = 0  - 成功 \n
 *    = 1  - business \n
 *    = -1 - 失败
 */
int32_t cm_agnss_data_start_update(cm_agnss_data_callback callback);

/**
 *  @brief dgnss 功能打开
 *
 *  @param [in] void
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 *
 *  @details 功能开启后，可以从NMEA/GGA数据中判断UE是否处于差分定位模式中。
 */
int32_t cm_dgnss_enable(void);

/**
 *  @brief dgnss 功能关闭
 *
 *  @param [in] void
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 */
int32_t cm_dgnss_disable(void);

/**
 *  @brief 向GPS芯片发送命令
 *
 *  @param [in] info nmea 报文
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 */
int32_t cm_gnss_send_raw_data(const char *raw_data, uint32_t len, cm_gnss_rawdata_rsp_callback callback);

/**
 *  @brief 对定位数据进行解析并返回定位结果
 *
 *  @param [in] raw_info nmea 报文
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 *
 *  @details 建议在NMEA全消息输出的情况下，调用此函数获取定位信息。
 */
int32_t cm_gnss_get_location_info(cm_gnss_location_info_t *info);

/**
 *  @brief 主动请求NMEA数据
 *
 *  @param [in] void
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 */
int32_t cm_gnss_req_nmea(void);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_GNSS_H__ */

/** @}*/
