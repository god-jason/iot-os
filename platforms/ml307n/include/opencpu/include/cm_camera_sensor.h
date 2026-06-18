/**
 * @file        cm_camera_sensor.h
 * @brief       相机硬件驱动适配接口
 * @copyright   Copyright @2026 China Mobile IOT. All rights reserved.
 * @author
 * @date
 *
 * @defgroup camera
 * @ingroup camera
 * @{
 */

#ifndef __CM_CAMERA_SENSOR_H__
#define __CM_CAMERA_SENSOR_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/* raw8 */
#define MEDIA_BUS_FMT_SBGGR8_1X8        0x3001
#define MEDIA_BUS_FMT_SGBRG8_1X8        0x3013
#define MEDIA_BUS_FMT_SGRBG8_1X8        0x3002
#define MEDIA_BUS_FMT_SRGGB8_1X8        0x3014
/* raw10 */
#define MEDIA_BUS_FMT_SBGGR10_1X10        0x3007
#define MEDIA_BUS_FMT_SGBRG10_1X10        0x300e
#define MEDIA_BUS_FMT_SGRBG10_1X10        0x300a
#define MEDIA_BUS_FMT_SRGGB10_1X10        0x300f
/* yuv422 */
#define MEDIA_BUS_FMT_UYVY8_2X8            0x2006
#define MEDIA_BUS_FMT_VYUY8_2X8            0x2007
#define MEDIA_BUS_FMT_YUYV8_2X8            0x2008
#define MEDIA_BUS_FMT_YVYU8_2X8            0x2009
/* RGB */
#define MEDIA_BUS_FMT_RGB565            0x1234


/****************************************************************************
 * Public Types
 ****************************************************************************/

/**
 * @brief camera ata模式参数
 */
typedef struct {
    uint32_t on;
    int32_t read_crc_value;
}cm_cam_sensor_ata_t;

/**
 * @brief camera 频率
 */
typedef enum{
    CAM_BANDING_50HZ    = 0,
    CAM_BANDING_60HZ    = 1,
    CAM_BANDING_AUTO    = 2,
} cm_cam_sensor_banding;

/**
 * @brief camera 曝光参数
 */
typedef struct {
    uint32_t exp_high;
    uint32_t exp_low;
    uint32_t gain;
    uint32_t aec_en;
    uint32_t min_fps;
    uint32_t max_fps;
    cm_cam_sensor_banding banding;
    uint32_t sensor_out_yuv;
} cm_cam_sensor_aec_t;

/**
 * @brief camera操作函数
 * @details 上层接口操作camera的实际驱动接口，内部需自行实现，目前系统只用到s_power、
 * ata_open、ata_open三个接口，其余预留，设置为NULL。
 */
typedef struct {
    int32_t *get_pixel_rate;
    int32_t *get_dphy_desc;
    int32_t *update_otp;
    int32_t (*s_power)(int32_t onoff);                              /*!< camera上下电 */
    int32_t (*ata_open)(cm_cam_sensor_ata_t *ata);                  /*!< 打开/关闭camera 模组进入test pattern的功能 */
    int32_t (*update_aec)(cm_cam_sensor_aec_t *aec);                /*!< camera模组曝光参数设置 */
    int32_t *set_effect;
    int32_t *set_brightness;
    int32_t *set_contrast;
    int32_t *set_wb;
    int32_t *set_saturation;
    int32_t *set_banding;
}cm_cam_sensor_spec_ops;

/**
 * @brief i2c寄存器支持的数据宽度
 */
typedef enum {
    CAM_I2C_8BIT = 0,
    CAM_I2C_16BIT,
    CAM_I2C_32BIT
}cm_cam_sensor_i2c_len;

/**
 * @brief i2c寄存器地址及写入值
 */
typedef struct {
    uint16_t reg;        /*!< 寄存器地址 */
    uint16_t val;        /*!< 数据值 */
    uint16_t mask;       /*!< 延时时间ms */
}cm_cam_regval_tab;

/**
 * @brief i2c寄存器操作表
 */
typedef struct {
    const cm_cam_regval_tab *tab;       /*!< i2c寄存器操作表，可同时配置多个 */
    uint32_t num;                       /*!< i2c寄存器操作表个数 */
}cm_cam_sensor_regs;

/**
 * @brief i2c参数
 */
typedef struct {
    cm_cam_sensor_i2c_len reg_len;      /*!< i2c寄存器宽度*/
    cm_cam_sensor_i2c_len val_len;      /*!< i2c数据宽度*/
    uint8_t addr;                       /*!< i2c设备地址*/
}cm_cam_sensor_i2c_attr;

/**
 * @brief 预留参数相关结构体，暂无使用
 */
typedef struct {
    cm_cam_sensor_regs regs;
    int32_t sensor_out_format;
}cm_cam_sensor_global;

/**
 * @brief 预留参数相关结构体
 */
typedef enum {
    CAM_SENSOR_RES_BINING1,
    CAM_SENSOR_RES_BINING2,
    CAM_SENSOR_RES_CROPPED,
    CAM_SENSOR_RES_MAX,
}cm_cam_sensor_res_prop;

/**
 * @brief camera输出数据类型
 */
typedef enum {
    CAM_SENSOR_FMT_RAW8 = 0,
    CAM_SENSOR_FMT_ONLY_Y,
    CAM_SENSOR_FMT_RAW10,
    CAM_SENSOR_FMT_RAW12,
    CAM_SENSOR_FMT_YUV422,
    CAM_SENSOR_FMT_RGB565,
} cm_cam_sensor_fmt;

/**
 * @brief camera参数设置及初始化
 */
typedef struct {
    uint32_t sensor_out_format;                 /*!< camera输出数据类型cm_cam_sensor_fmt*/
    uint16_t mbus_code;                         /*!< 多媒体总线数据类型，相机调试时，如果存在色变，可修改该值*/
    uint16_t width;                             /*!< camera输出width像素数*/
    uint16_t height;                            /*!< camera输出height像素数*/
    uint32_t hts;                               /*!< 行周期值*/
    uint32_t min_vts;                           /*!< 最小场周期值*/
    uint8_t max_fps;                            /*!< 最大帧率*/
    uint8_t min_fps;                            /*!< 最小帧率*/
    cm_cam_sensor_res_prop prop;                /*!< 预留参数*/
    cm_cam_sensor_regs regs;                    /*!< 模组init代码*/
}cm_cam_sensor_resolution;

/**
 * @brief 预留参数结构体
 */
typedef struct {
    uint32_t id;
    char *name;/* as module id send to userspace, limit the size to 7 characters*/
    uint32_t apeture_size;
}cm_cam_sensor_module;

/**
 * @brief ISP 寄存器地址组及写入值
 */
typedef struct {
    uint16_t reg;            /*!< 寄存器地址*/
    uint16_t val;            /*!< 写入值*/
}cm_isp_tuning_tab;

/**
 * @brief ISP 寄存器操作表
 */
typedef struct {
    cm_isp_tuning_tab *tab; /*!< ISP寄存器操作表，可同时配置多个 */
    uint32_t num;           /*!< ISP寄存器操作表，可同时配置多个 */
}cm_isp_tuning_data;

/**
 * @brief camera驱动结构体
 */
typedef struct {
    char * const name;                          /*!< camera名称 */
    cm_cam_sensor_spec_ops *ops;                /*!< camera操作函数 */
    int32_t position;                           /*!< camera相机位置：0 后置，1 前置    */
    cm_cam_sensor_i2c_attr *i2c_attr;           /*!< camera i2c参数*/
    int32_t num_i2c_attr;                       /*!< i2c挂载的设备数 */
    cm_cam_sensor_regs id;                      /*!< camera id寄存器地址及对应的预期值，用于判断camera是否匹配 */
    cm_cam_sensor_global *global_setting;       /*!< 预留参数，暂未使用，可不设置 */
    int32_t num_global;                         /*!< 预留参数，暂未使用,可不设置 */
    cm_cam_sensor_resolution *res;              /*!< camera参数设置及初始化 */
    int32_t num_res;                            /*!< camera参数设置的组数 */
    cm_cam_sensor_regs streamon;                /*!< camera streamon需要用到的寄存器组及写入值 */
    cm_cam_sensor_regs streamoff;               /*!< camera streamoff需要用到的寄存器组及写入值 */
    cm_cam_sensor_regs expo_reg;                /*!< camera 曝光值需要用到的寄存器组及写入值 */
    cm_cam_sensor_regs vts_reg;                 /*!< camera 场周期值需要用到的寄存器组及写入值，预留值，暂未使用，可不设置 */
    cm_cam_sensor_regs gain_reg;                /*!< camera 增益需要用到的寄存器组及写入值，预留值，暂未使用，可不设置 */
    cm_cam_sensor_regs hflip;                   /*!< camera 水平翻转需要用到的寄存器组及写入值，预留值，暂未使用，可不设置 */
    cm_cam_sensor_regs vflip;                   /*!< camera 垂直翻转需要用到的寄存器组及写入值，预留值，暂未使用，可不设置 */
    cm_cam_sensor_module *module;               /*!< 预留参数，暂未使用，可不设置 */
    int32_t num_module;                         /*!< 预留参数，暂未使用，可不设置 */
    cm_isp_tuning_tab *tuning_tab;              /*!< ISP 寄存器地址组及写入值，用于ISP图像优化，由芯片厂商提供，一般设置为NULL */
    int32_t num_tuning_tab;                     /*!< ISP 寄存器地址组数 */
    cm_isp_tuning_data banding_tab[2];          /*!< ISP 寄存器地址组及写入值，用于ISP图像优化，由芯片厂商提供，一般设置为NULL  */
    int32_t interface_type;                     /*!< camera接口类型，目前只支持设置0（SPI） */
    uint8_t lane_num;                           /*!< camera具体的接口规格，目前只支持设置1（SPI_2_LAN） */
    uint8_t spi_sdr;                            /*!< SPI采样模式，0：sdr，即单边采样模式；1：ddr，即双边采样模式 */
    uint8_t spi_crc;                            /*!< ISP crc校验功能，0：不使能crc校验；1: 使能crc校验 */
    int32_t reset_delay;                        /*!< reset延迟，暂未使用，可不设置 */
    int32_t cur_i2c_index;                      /*!< i2c_attr中的I2C外设索引值，即当前使用哪一组i2c_attr */
    int32_t cur_res_index;                      /*!< res中的camera参数设置及初始化索引值，即当前使用哪一组res */
}cm_cam_sensor_data_t;

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


/**
 * @brief 使能mclk
 *
 * @param [in] enable    使能(1 打开， 0 关闭)
 *
 * @return
 *
 * @details 用于相机上电逻辑，控制mclk时钟输出
 */
void cm_camera_sensor_set_mclk(int32_t enable);

/**
 * @brief 设置驱动参数
 *
 * @param [in] data         驱动参数
 *
 * @return
 *
 * @details 操作相机前需设置
 */
void cm_camera_sensor_set_data(cm_cam_sensor_data_t *data);

/**
 * @brief i2c读取
 *
 * @param [in] attr         i2c参数
 * @param [in] reg          寄存器地址
 * @param [out] val         读取值
 *
 * @return  0 成功；-1 失败
 *
 * @details 通过i2c读取相机寄存器
 */
int32_t cm_camera_sensor_i2c_read(const cm_cam_sensor_i2c_attr *attr, uint32_t reg, uint32_t *val);

/**
 * @brief i2c写
 *
 * @param [in] attr         i2c参数
 * @param [in] regs         寄存器操作表
 *
 * @return  0 成功；-1 失败
 *
 * @details 通过i2c写相机寄存器
 */
int32_t cm_camera_sensor_i2c_write(const cm_cam_sensor_i2c_attr *attr, cm_cam_sensor_regs regs);

/**
 * @brief i2c写（单个数据）
 *
 * @param [in] attr         i2c参数
 * @param [in] reg          寄存器地址
 * @param [out] val         写入值
 *
 * @return  0 成功；-1 失败
 *
 * @details 通过i2c写相机寄存器
 */
int32_t cm_camera_sensor_i2c_write_single(const cm_cam_sensor_i2c_attr *attr, uint32_t reg, uint32_t val);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_CAMERA_SENSOR_H__ */


/** @}*/

