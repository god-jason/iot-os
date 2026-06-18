/**
 * @file        cm_demo_camera.c
 * @brief       OpenCPU camera测试例程
 * @copyright   Copyright ? 2023 China Mobile IOT. All rights reserved.
 */

#ifdef CM_DEMO_CAMERA_SUPPORT

/****************************************************************************
* Included Files
****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "cm_iomux.h"
#include "cm_gpio.h"
#include "cm_mem.h"
#include "cm_fs.h"
#include "cm_camera_sensor.h"
#include "cm_camera.h"

#ifdef ML307A
#include "cm_demo_uart.h"
#include "cm_qr.h"
#endif

#include "res_tree.h"

#include "at_api.h"
#include "at_parser.h"


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

//#define CM_CAM_DEMO_LOG   cm_demo_printf
#define CM_CAM_DEMO_LOG(fmt, args...) os_kprintf("[CAM_SH]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define CAPTURE_FILE_NAME_RGB   "capture.rgb"
#define CAPTURE_FILE_NAME_JPEG  "capture.jpeg"

uint32_t g_camera_previw_duration_sec   = 5;
uint32_t g_camera_at_channel            = 1; //AT_CHANNEL_ID_UART
uint32_t enqueueCamRecordBuffer_counter = 0;

#ifdef ML307A
static uint8_t *qr_buff = NULL;
#endif

//raw8 640 * 486 spi 2 lan sdr crc  20fps
static const cm_cam_regval_tab GC032a_init_rgb565_spi2lan_crc_sdr[] = {
    {0xf3, 0x83},   /*System*/
    {0xf5, 0x0c},
    {0xf7, 0x13},   //    {0xf7, 0x11},
    {0xf8, 0x0f},   //20fps
    {0xf9, 0x4e},
    {0xfa, 0x10},
    {0xfc, 0x02},
    {0xfe, 0x02},
    {0x81, 0x03},
    {0xfe, 0x00},   /*Analog&Cisctl*/
    {0x03, 0x01},
    {0x04, 0xc2},
    {0x05, 0x01},
    {0x06, 0xa3},
    {0x07, 0x00},
    {0x08, 0x08},
    {0x0a, 0x04},
    {0x0c, 0x04},
    {0x0d, 0x01},
    {0x0e, 0xe8},
    {0x0f, 0x02},
    {0x10, 0x88},
    {0x17, 0x54},
    {0x19, 0x04},
    {0x1a, 0x0a},
    {0x1f, 0x40},
    {0x20, 0x30},
    {0x2e, 0x80},
    {0x2f, 0x2b},
    {0x30, 0x1a},
    {0xfe, 0x02},
    {0x03, 0x02},   //[4:0]post_tx_width
    {0x06, 0x60},   //[5:4]stsbp_mode
    {0x05, 0xd7},   //drv
    {0x12, 0x89},   //[7:6]init_ramp_mode
    {0xfe, 0x03},   /*SPI*/
    {0x51, 0x00},   //stream off
    {0x52, 0xda},
    {0x53, 0x24},   //no crc 0x24     crc 0xa4
    {0x54, 0x00},
    {0x55, 0x00},
    {0x59, 0x30},   //    {0x59, 0x30},
    {0x5a, 0x02},   //0x01  type
    {0x5b, 0x80},
    {0x5c, 0x02},
#if 0 /* defined (CONFIG_BOARD_CRANEM_EVB) */
    {0x5d, 0xe0}, // e0
#else
    {0x5d, 0xe6}, // e0
#endif
    {0x5e, 0x01},
    {0x64, 0x06},
    {0xfe, 0x00},   /*blk*/
    {0x18, 0x02},
    {0xfe, 0x02},
    {0x40, 0x22},
    {0x45, 0x00},
    {0x46, 0x00},
    {0x49, 0x20},
    {0x4b, 0x3c},
    {0x50, 0x20},
    {0x42, 0x10},
    {0xfe, 0x01},   /*isp*/
    {0x0a, 0xc5},
    {0x45, 0x00},   //[6]darksun_en
    {0xfe, 0x00},
    {0x40, 0xff},
    {0x41, 0x25},
    {0x42, 0xcf},
    {0x43, 0x10},
    {0x44, 0x99},
    {0x46, 0x26},
    {0x49, 0x03},
    {0x4f, 0x01},   //[0]AEC_en
    {0xde, 0x84},
    {0xfe, 0x02},
    {0x22, 0xf6},   //CISCTL_SUN_TH_R

    {0xfe, 0x00},
    {0x0a, 0x02},
    {0x55, 0x01},
    {0x56, 0xe6},

    {0xfe, 0x01},
    {0x0a, 0x65},
    {0xfe, 0x00},
    {0x4f, 0x00}, //aec off
    {0x42, 0x80}, //awb off
    {0x40, 0x1f}, //cc off
    {0x43, 0x00}, //gamma off

    {0xfe, 0x00},   //Analog&Cisctl
    {0x03, 0x06},    //0x01   exp_high[3:0]   exposure[11:8]
    {0x04, 0xb0},    //0xc2   exp_low[7:0]    exposure[7:0]

};


//raw8 640 * 486 spi 2 lan sdr crc  20fps
static const cm_cam_regval_tab GC032a_init_raw8_spi2lan_crc_sdr[] = {
    {0xf3, 0x83},   /*System*/
    {0xf5, 0x0c},
    {0xf7, 0x13},   //    {0xf7, 0x11},
    {0xf8, 0x0f},   //20fps
    {0xf9, 0x4e},
    {0xfa, 0x10},
    {0xfc, 0x02},
    {0xfe, 0x02},
    {0x81, 0x03},
    {0xfe, 0x00},   /*Analog&Cisctl*/
    {0x03, 0x01},
    {0x04, 0xc2},
    {0x05, 0x01},
    {0x06, 0xa3},
    {0x07, 0x00},
    {0x08, 0x08},
    {0x0a, 0x04},
    {0x0c, 0x04},
    {0x0d, 0x01},
    {0x0e, 0xe8},
    {0x0f, 0x02},
    {0x10, 0x88},
    {0x17, 0x54},
    {0x19, 0x04},
    {0x1a, 0x0a},
    {0x1f, 0x40},
    {0x20, 0x30},
    {0x2e, 0x80},
    {0x2f, 0x2b},
    {0x30, 0x1a},
    {0xfe, 0x02},
    {0x03, 0x02},   //[4:0]post_tx_width
    {0x06, 0x60},   //[5:4]stsbp_mode
    {0x05, 0xd7},   //drv
    {0x12, 0x89},   //[7:6]init_ramp_mode
    {0xfe, 0x03},   /*SPI*/
    {0x51, 0x00},   //stream off
    {0x52, 0xda},
    {0x53, 0x24},   //no crc 0x24     crc 0xa4
    {0x54, 0x00},
    {0x55, 0x00},
    {0x59, 0x30},   //    {0x59, 0x30},
    {0x5a, 0x02},   //0x01  type
    {0x5b, 0x80},
    {0x5c, 0x02},
#if 0 /* defined (CONFIG_BOARD_CRANEM_EVB) */
    {0x5d, 0xe0}, // e0
#else
    {0x5d, 0xe6}, // e0
#endif
    {0x5e, 0x01},
    {0x64, 0x06},
    {0xfe, 0x00},   /*blk*/
    {0x18, 0x02},
    {0xfe, 0x02},
    {0x40, 0x22},
    {0x45, 0x00},
    {0x46, 0x00},
    {0x49, 0x20},
    {0x4b, 0x3c},
    {0x50, 0x20},
    {0x42, 0x10},
    {0xfe, 0x01},   /*isp*/
    {0x0a, 0xc5},
    {0x45, 0x00},   //[6]darksun_en
    {0xfe, 0x00},
    {0x40, 0xff},
    {0x41, 0x25},
    {0x42, 0xcf},
    {0x43, 0x10},
    {0x44, 0x99},
    {0x46, 0x26},
    {0x49, 0x03},
    {0x4f, 0x01},   //[0]AEC_en
    {0xde, 0x84},
    {0xfe, 0x02},
    {0x22, 0xf6},   //CISCTL_SUN_TH_R

    {0xfe, 0x00},
    {0x0a, 0x02},
    {0x55, 0x01},
    {0x56, 0xe6},

    {0xfe, 0x01},
    {0x0a, 0x65},
    {0xfe, 0x00},
    {0x4f, 0x00}, //aec off
    {0x42, 0x80}, //awb off
    {0x40, 0x1f}, //cc off
    {0x43, 0x00}, //gamma off

    {0xfe, 0x00},   //Analog&Cisctl
    {0x03, 0x06},    //0x01   exp_high[3:0]   exposure[11:8]
    {0x04, 0xb0},    //0xc2   exp_low[7:0]    exposure[7:0]

};

//yuv422 640 * 480 spi 2 lan sdr
static const cm_cam_regval_tab gc032a_init_yuv422_spi2lan_nocrc_sdr[] = {
    // 15 fps
    {0xf3, 0x83},   /*System*/
    {0xf5, 0x0c},
    {0xf7, 0x13},   //    {0xf7, 0x11},
    {0xf8, 0x0f},
    {0xf9, 0x4e},
    {0xfa, 0x31}, //{0xfa, 0x10},
    {0xfc, 0x02},
    {0xfe, 0x02},
    {0x81, 0x03},

    {0xfe, 0x00},   /*Analog&Cisctl*/
    {0x03, 0x01},
    {0x04, 0xc2},
    {0x05, 0x01},
    {0x06, 0xa3},
    {0x07, 0x00},
    {0x08, 0x08},
    {0x0a, 0x04},
    {0x0c, 0x04},
    {0x0d, 0x01},
    {0x0e, 0xe8},
    {0x0f, 0x02},
    {0x10, 0x88},
    {0x17, 0x54},
    {0x19, 0x04},
    {0x1a, 0x0a},
    {0x1f, 0x40},
    {0x20, 0x30},
    {0x2e, 0x80},
    {0x2f, 0x2b},
    {0x30, 0x1a},
    {0xfe, 0x02},
    {0x03, 0x02},   //[4:0]post_tx_width
    {0x06, 0x60},   //[5:4]stsbp_mode
    {0x05, 0xd7},   //drv
    {0x12, 0x89},   //[7:6]init_ramp_mode
    {0xfe, 0x03},   /*SPI*/
    {0x51, 0x00},   //stream off
    {0x52, 0xda},
    {0x53, 0x24},   //no crc 0x24     crc 0xa4
    {0x54, 0x20},
    {0x55, 0x00},
    {0x59, 0x10},   //    {0x59, 0x30},
    {0x5a, 0x00},   //0x01  type
    {0x5b, 0x80},
    {0x5c, 0x02},
    {0x5d, 0xe0}, // e0
    {0x5e, 0x01},
    {0x64, 0x06},
    {0xfe, 0x00},   /*blk*/
    {0x18, 0x02},
    {0xfe, 0x02},
    {0x40, 0x22},
    {0x45, 0x00},
    {0x46, 0x00},
    {0x49, 0x20},
    {0x4b, 0x3c},
    {0x50, 0x20},
    {0x42, 0x10},
    {0xfe, 0x01},   /*isp*/
    {0x0a, 0xc5},
    {0x45, 0x00},   //[6]darksun_en
    {0xfe, 0x00},
    {0x40, 0xff},
    {0x41, 0x25},
    {0x42, 0xcf},
    {0x43, 0x10},
    {0x44, 0x83},
    {0x46, 0x26},
    {0x49, 0x03},
    {0x4f, 0x01},   //[0]AEC_en
    {0xde, 0x84},
    {0xfe, 0x02},
    {0x22, 0xf6},   //CISCTL_SUN_TH_R
    {0xfe, 0x01},   /*Shading*/
    {0xc1, 0x3c},
    {0xc2, 0x50},
    {0xc3, 0x00},
    {0xc4, 0x32},
    {0xc5, 0x24},
    {0xc6, 0x16},
    {0xc7, 0x08},
    {0xc8, 0x08},
    {0xc9, 0x00},
    {0xca, 0x20},
    {0xdc, 0x8a},
    {0xdd, 0xa0},
    {0xde, 0xa6},
    {0xdf, 0x75},
    {0xfe, 0x01},   /*AWB*/
    {0x7c, 0x09},
    {0x65, 0x06},
    {0x7c, 0x08},
    {0x56, 0xf4},
    {0x66, 0x0f},
    {0x67, 0x84},
    {0x6b, 0x80},
    {0x6d, 0x12},
    {0x6e, 0xb0},
    {0x86, 0x00},
    {0x87, 0x00},
    {0x88, 0x00},
    {0x89, 0x00},
    {0x8a, 0x00},
    {0x8b, 0x00},
    {0x8c, 0x00},
    {0x8d, 0x00},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0xef},
    {0x91, 0xe1},
    {0x92, 0x0c},
    {0x93, 0xef},
    {0x94, 0x65},
    {0x95, 0x1f},
    {0x96, 0x0c},
    {0x97, 0x2d},
    {0x98, 0x20},
    {0x99, 0xaa},
    {0x9a, 0x3f},
    {0x9b, 0x2c},
    {0x9c, 0x5f},
    {0x9d, 0x3e},
    {0x9e, 0xaa},
    {0x9f, 0x67},
    {0xa0, 0x60},
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xa3, 0x0a},
    {0xa4, 0xb6},
    {0xa5, 0xac},
    {0xa6, 0xc1},
    {0xa7, 0xac},
    {0xa8, 0x55},
    {0xa9, 0xc3},
    {0xaa, 0xa4},
    {0xab, 0xba},
    {0xac, 0xa8},
    {0xad, 0x55},
    {0xae, 0xc8},
    {0xaf, 0xb9},
    {0xb0, 0xd4},
    {0xb1, 0xc3},
    {0xb2, 0x55},
    {0xb3, 0xd8},
    {0xb4, 0xce},
    {0xb5, 0x00},
    {0xb6, 0x00},
    {0xb7, 0x05},
    {0xb8, 0xd6},
    {0xb9, 0x8c},
    {0xfe, 0x01},   /*CC*/
    {0xd0, 0x40},   //3a
    {0xd1, 0xf8},
    {0xd2, 0x00},
    {0xd3, 0xfa},
    {0xd4, 0x45},
    {0xd5, 0x02},
    {0xd6, 0x30},
    {0xd7, 0xfa},
    {0xd8, 0x08},
    {0xd9, 0x08},
    {0xda, 0x58},
    {0xdb, 0x02},
    {0xfe, 0x00},
    {0xfe, 0x00},   /*Gamma*/
    {0xba, 0x00},
    {0xbb, 0x04},
    {0xbc, 0x0a},
    {0xbd, 0x0e},
    {0xbe, 0x22},
    {0xbf, 0x30},
    {0xc0, 0x3d},
    {0xc1, 0x4a},
    {0xc2, 0x5d},
    {0xc3, 0x6b},
    {0xc4, 0x7a},
    {0xc5, 0x85},
    {0xc6, 0x90},
    {0xc7, 0xa5},
    {0xc8, 0xb5},
    {0xc9, 0xc2},
    {0xca, 0xcc},
    {0xcb, 0xd5},
    {0xcc, 0xde},
    {0xcd, 0xea},
    {0xce, 0xf5},
    {0xcf, 0xff},
    {0xfe, 0x00},   /*Auto Gamma*/
    {0x5a, 0x08},
    {0x5b, 0x0f},
    {0x5c, 0x15},
    {0x5d, 0x1c},
    {0x5e, 0x28},
    {0x5f, 0x36},
    {0x60, 0x45},
    {0x61, 0x51},
    {0x62, 0x6a},
    {0x63, 0x7d},
    {0x64, 0x8d},
    {0x65, 0x98},
    {0x66, 0xa2},
    {0x67, 0xb5},
    {0x68, 0xc3},
    {0x69, 0xcd},
    {0x6a, 0xd4},
    {0x6b, 0xdc},
    {0x6c, 0xe3},
    {0x6d, 0xf0},
    {0x6e, 0xf9},
    {0x6f, 0xff},
    {0xfe, 0x00},   /*Gain*/
    {0x70, 0x50},
    {0xfe, 0x00},   /*AEC*/
    {0x4f, 0x01},
    {0xfe, 0x01},
    {0x44, 0x04},
    {0x1f, 0x30},
    {0x20, 0x40},
    {0x26, 0x4e},
    {0x27, 0x01},
    {0x28, 0xd4},
    {0x29, 0x03},
    {0x2a, 0x0c},
    {0x2b, 0x03},
    {0x2c, 0xe9},
    {0x2d, 0x07},
    {0x2e, 0xd2},
    {0x2f, 0x0b},
    {0x30, 0x6e},
    {0x31, 0x0e},
    {0x32, 0x70},
    {0x33, 0x12},
    {0x34, 0x0c},
    {0x3c, 0x10},   //[5:4] Max level setting
    {0x3e, 0x20},
    {0x3f, 0x2d},
    {0x40, 0x40},
    {0x41, 0x5b},
    {0x42, 0x82},
    {0x43, 0xb7},
    {0x04, 0x0a},
    {0x02, 0x79},
    {0x03, 0xc0},
    {0xcc, 0x08},   /*measure window*/
    {0xcd, 0x08},
    {0xce, 0xa4},
    {0xcf, 0xec},
    {0xfe, 0x00},   /*DNDD*/
    {0x81, 0xb8},   //f8
    {0x82, 0x12},
    {0x83, 0x0a},
    {0x84, 0x01},
    {0x86, 0x50},
    {0x87, 0x18},
    {0x88, 0x10},
    {0x89, 0x70},
    {0x8a, 0x20},
    {0x8b, 0x10},
    {0x8c, 0x08},
    {0x8d, 0x0a},
    {0xfe, 0x00},   /*Intpee*/
    {0x8f, 0xaa},
    {0x90, 0x9c},
    {0x91, 0x52},
    {0x92, 0x03},
    {0x93, 0x03},
    {0x94, 0x08},
    {0x95, 0x44},
    {0x97, 0x00},
    {0x98, 0x00},
    {0xfe, 0x00},   /*ASDE*/
    {0xa1, 0x30},
    {0xa2, 0x41},
    {0xa4, 0x30},
    {0xa5, 0x20},
    {0xaa, 0x30},
    {0xac, 0x32},
    {0xfe, 0x00},   /*YCP*/
    {0xd1, 0x3c},
    {0xd2, 0x3c},
    {0xd3, 0x38},
    {0xd6, 0xf4},
    {0xd7, 0x1d},
    {0xdd, 0x73},
    {0xde, 0x84},
     //{0x4c, 0x08}   //for color bar
};

//yuv422 640 * 480 spi 2 lan ddr
static const cm_cam_regval_tab gc032a_init_yuv422_640x480_spi2lan_nocrc_ddr[] = {
    /*System*/
    {0xf3, 0xe3},
    {0xf5, 0x0c},
    {0xf7, 0x01},
    {0xf8, 0x03},
    {0xf9, 0x4e},
    {0xfa, 0x00},
    {0xfc, 0x02},
    {0xfe, 0x02},
    {0x81, 0x03},

    /*Analog&Cisctl*/
    {0xfe, 0x00},
    {0x03, 0x01}, // exp
    {0x04, 0xce},
    {0x05, 0x01},
    {0x06, 0xad},
    {0x07, 0x00},
    {0x08, 0x10},
    {0x0a, 0x04},
    {0x0c, 0x04},
    {0x0d, 0x01},
    {0x0e, 0xe8},
    {0x0f, 0x02},
    {0x10, 0x88},
    {0x17, 0x54},
    {0x19, 0x04},
    {0x1a, 0x0a},
    {0x1f, 0x40},
    {0x20, 0x30},
    {0x2e, 0x80},
    {0x2f, 0x2b},
    {0x30, 0x1a},
    {0xfe, 0x02},
    {0x03, 0x02},
    {0x05, 0xd7},
    {0x06, 0x60},
    {0x08, 0x80},
    {0x12, 0x89},

    /*SPI*/
    {0xfe, 0x03},
    {0x51, 0x01},
    {0x52, 0xfa},
    {0x53, 0x24},
    {0x54, 0x20},
    {0x55, 0x00},
    {0x59, 0x10},
    {0x5a, 0x00},
    {0x5b, 0x80},
    {0x5c, 0x02},
    {0x5d, 0xe0},
    {0x5e, 0x01},

    /*blk*/
    {0xfe, 0x00},
    {0x18, 0x02},
    {0xfe, 0x02},
    {0x40, 0x22},
    {0x45, 0x00},
    {0x46, 0x00},
    {0x49, 0x20},
    {0x4b, 0x3c},
    {0x50, 0x20},
    {0x42, 0x10},

    /*isp*/
    {0xfe, 0x01},
    {0x0a, 0xc5},
    {0x45, 0x00},
    {0xfe, 0x00},
    {0x40, 0xff},
    {0x41, 0x25},
    {0x42, 0xcf},
    {0x43, 0x10},
    {0x44, 0x83},
    {0x46, 0x26},
    {0x49, 0x03},
    {0xfe, 0x02},
    {0x22, 0xf6},

    /*Shading*/
    {0xfe, 0x01},
    {0xc1, 0x3c},
    {0xc2, 0x50},
    {0xc3, 0x00},
    {0xc4, 0x32},
    {0xc5, 0x24},
    {0xc6, 0x16},
    {0xc7, 0x08},
    {0xc8, 0x08},
    {0xc9, 0x00},
    {0xca, 0x20},
    {0xdc, 0x8a},
    {0xdd, 0xa0},
    {0xde, 0xa6},
    {0xdf, 0x75},

    /*AWB*/
    {0xfe, 0x01},
    {0x7c, 0x09},
    {0x65, 0x06},
    {0x7c, 0x08},
    {0x56, 0xf4},
    {0x66, 0x0f},
    {0x67, 0x84},
    {0x6b, 0x80},
    {0x6d, 0x12},
    {0x6e, 0xb0},
    {0x86, 0x00},
    {0x87, 0x00},
    {0x88, 0x00},
    {0x89, 0x00},
    {0x8a, 0x00},
    {0x8b, 0x00},
    {0x8c, 0x00},
    {0x8d, 0x00},
    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0xef},
    {0x91, 0xe1},
    {0x92, 0x0c},
    {0x93, 0xef},
    {0x94, 0x65},
    {0x95, 0x1f},
    {0x96, 0x0c},
    {0x97, 0x2d},
    {0x98, 0x20},
    {0x99, 0xaa},
    {0x9a, 0x3f},
    {0x9b, 0x2c},
    {0x9c, 0x5f},
    {0x9d, 0x3e},
    {0x9e, 0xaa},
    {0x9f, 0x67},
    {0xa0, 0x60},
    {0xa1, 0x00},
    {0xa2, 0x00},
    {0xa3, 0x0a},
    {0xa4, 0xb6},
    {0xa5, 0xac},
    {0xa6, 0xc1},
    {0xa7, 0xac},
    {0xa8, 0x55},
    {0xa9, 0xc3},
    {0xaa, 0xa4},
    {0xab, 0xba},
    {0xac, 0xa8},
    {0xad, 0x55},
    {0xae, 0xc8},
    {0xaf, 0xb9},
    {0xb0, 0xd4},
    {0xb1, 0xc3},
    {0xb2, 0x55},
    {0xb3, 0xd8},
    {0xb4, 0xce},
    {0xb5, 0x00},
    {0xb6, 0x00},
    {0xb7, 0x05},
    {0xb8, 0xd6},
    {0xb9, 0x8c},

    /*CC*/
    {0xfe, 0x01},
    {0xd0, 0x40},//3a
    {0xd1, 0xf8},
    {0xd2, 0x00},
    {0xd3, 0xfa},
    {0xd4, 0x45},
    {0xd5, 0x02},

    {0xd6, 0x30},
    {0xd7, 0xfa},
    {0xd8, 0x08},
    {0xd9, 0x08},
    {0xda, 0x58},
    {0xdb, 0x02},
    {0xfe, 0x00},

    /*Gamma*/
    {0xfe, 0x00},
    {0xba, 0x00},
    {0xbb, 0x04},
    {0xbc, 0x0a},
    {0xbd, 0x0e},
    {0xbe, 0x22},
    {0xbf, 0x30},
    {0xc0, 0x3d},
    {0xc1, 0x4a},
    {0xc2, 0x5d},
    {0xc3, 0x6b},
    {0xc4, 0x7a},
    {0xc5, 0x85},
    {0xc6, 0x90},
    {0xc7, 0xa5},
    {0xc8, 0xb5},
    {0xc9, 0xc2},
    {0xca, 0xcc},
    {0xcb, 0xd5},
    {0xcc, 0xde},
    {0xcd, 0xea},
    {0xce, 0xf5},
    {0xcf, 0xff},

    /*Auto Gamma*/
    {0xfe, 0x00},
    {0x5a, 0x08},
    {0x5b, 0x0f},
    {0x5c, 0x15},
    {0x5d, 0x1c},
    {0x5e, 0x28},
    {0x5f, 0x36},
    {0x60, 0x45},
    {0x61, 0x51},
    {0x62, 0x6a},
    {0x63, 0x7d},
    {0x64, 0x8d},
    {0x65, 0x98},
    {0x66, 0xa2},
    {0x67, 0xb5},
    {0x68, 0xc3},
    {0x69, 0xcd},
    {0x6a, 0xd4},
    {0x6b, 0xdc},
    {0x6c, 0xe3},
    {0x6d, 0xf0},
    {0x6e, 0xf9},
    {0x6f, 0xff},

    /*Gain*/
    {0xfe, 0x00},
    {0x70, 0x50},

    /*AEC*/
    {0xfe, 0x00},
    {0x4f, 0x01}, // aec_en
    {0xfe, 0x01},
    {0x12, 0xa0},
    {0x13, 0x3a},
    {0x44, 0x04},
    {0x1f, 0x30},
    {0x20, 0x40},
    {0x26, 0x9a},
    {0x27, 0x01},
    {0x28, 0xce},
    {0x29, 0x02},
    {0x2a, 0x68},
    {0x2b, 0x03},
    {0x2c, 0x9c},
    {0x2d, 0x07},
    {0x2e, 0xd2},
    {0x2f, 0x0b},
    {0x30, 0x6e},
    {0x31, 0x0e},
    {0x32, 0x70},
    {0x33, 0x12},
    {0x34, 0x0c},
    {0x3c, 0x20}, //[5:4] Max level setting
    {0x3e, 0x20},
    {0x3f, 0x2d},
    {0x40, 0x40},
    {0x41, 0x5b},
    {0x42, 0x82},
    {0x43, 0xb7},
    {0x04, 0x0a},
    {0x02, 0x79},
    {0x03, 0xc0},

    /*measure window*/
    {0xcc, 0x08},
    {0xcd, 0x08},
    {0xce, 0xa4},
    {0xcf, 0xec},

    /*DNDD*/
    {0xfe, 0x00},
    {0x81, 0xb8},//f8
    {0x82, 0x12},
    {0x83, 0x0a},
    {0x84, 0x01},
    {0x86, 0x50},
    {0x87, 0x18},
    {0x88, 0x10},
    {0x89, 0x70},
    {0x8a, 0x20},
    {0x8b, 0x10},
    {0x8c, 0x08},
    {0x8d, 0x0a},

    /*Intpee*/
    {0xfe, 0x00},
    {0x8f, 0xaa},
    {0x90, 0x9c},
    {0x91, 0x52},
    {0x92, 0x03},
    {0x93, 0x03},
    {0x94, 0x08},
    {0x95, 0x44},
    {0x97, 0x00},
    {0x98, 0x00},

    /*ASDE*/
    {0xfe, 0x00},
    {0xa1, 0x30},
    {0xa2, 0x41},
    {0xa4, 0x30},
    {0xa5, 0x20},
    {0xaa, 0x30},
    {0xac, 0x32},

    /*YCP*/
    {0xfe, 0x00},
    {0xd1, 0x3c},
    {0xd2, 0x3c},
    {0xd3, 0x38},
    {0xd6, 0xf4},
    {0xd7, 0x1d},
    {0xdd, 0x73},
    {0xde, 0x84},

     //{0x4c, 0x08}   //for color bar
};


static const cm_cam_regval_tab GC032a_id[] = {
    {0xf0, 0x23},
    {0xf1, 0x2a},
};
static const cm_cam_regval_tab GC032a_vts[] = {
    {0x00, 0x00},
};

static const cm_cam_regval_tab GC032a_stream_on[] = {
    {0xfe, 0x03},
    {0x51, 0x01},
};
static const cm_cam_regval_tab GC032a_stream_off[] = {
    {0xfe, 0x03},
    {0x51, 0x00},
};
static const cm_cam_regval_tab GC032a_expo[] = {
    {0xfe, 0x00},    //page select
    {0x03, 0x01},    //exp_high
    {0x04, 0x00},    //exp_low
};
static const cm_cam_regval_tab GC032a_gain[] = {
    {0xfe, 0x00},    //page select
    {0x48, 0x01},    //gain
};


static const cm_cam_regval_tab GC032a_vflip[] = {
    {0x00, 0x00},
};
static const cm_cam_regval_tab GC032a_hflip[] = {
    {0x00, 0x00},
};

static cm_cam_sensor_i2c_attr GC032a_i2c_attr[] = {
    {
        CAM_I2C_8BIT,
        CAM_I2C_8BIT,
        0x21,//0x42,
    }
};

static cm_cam_sensor_module GC032a_MODULE_INFO[] = {
    {
        0x01,
        "0x01",
        0,
    },
    {
        0xff,
        "0xff",
        0,
    },
};

static cm_cam_sensor_resolution GC032a_resolution_preview[] = {
    {
        CAM_SENSOR_FMT_RGB565,
        MEDIA_BUS_FMT_RGB565,
        240,
        320,
        0x0708,
        0x0d06,
        30,
        15,
        CAM_SENSOR_RES_BINING1,
        {
            GC032a_init_rgb565_spi2lan_crc_sdr,
            ARRAY_SIZE(GC032a_init_rgb565_spi2lan_crc_sdr),
        },
    },
};


static cm_cam_sensor_resolution GC032a_resolution[] = {
    {
        CAM_SENSOR_FMT_RGB565,
        MEDIA_BUS_FMT_RGB565,
        480,
        640,
        0x0708,
        0x0d06,
        30,
        15,
        CAM_SENSOR_RES_BINING1,
        {
            GC032a_init_rgb565_spi2lan_crc_sdr,
            ARRAY_SIZE(GC032a_init_rgb565_spi2lan_crc_sdr),
        },
    },
};

static void camera_debug_at_log(uint32_t value, int32_t user_data)
{
    uint8_t debug_str[50] = {0};

    if (value == 0)
    {
        snprintf(debug_str, 50, "\r\n+PREVIEW: %u\r\n", user_data);
    }
    else if (value == 1)
    {
        snprintf(debug_str, 50, "\r\n+PREVIEW: %s\r\n", user_data == 0 ? "STOP" : "FAIL");
    }
    else if (value == 2)
    {
        snprintf(debug_str, 50, "\r\n+CAMERA: take RGB(\/%s)\r\n", user_data == 0 ? CAPTURE_FILE_NAME_RGB : "FAIL");
    }
    else if (value == 3)
    {
        snprintf(debug_str, 50, "\r\n+CAMERA: take JPEG(\/%s)\r\n", user_data == 0 ? CAPTURE_FILE_NAME_JPEG : "FAIL");
    }
    else if (value == 4)
    {
        snprintf(debug_str, 50, "\r\n+CAMERA: take output JPEG %s\r\n", user_data == 0 ? "SUCC" : "FAIL");
    }
    else
    {
        snprintf(debug_str, 50, "\r\n+CAMERA: UNKNOW %u\r\n", value);
    }

    AT_SendResponse(g_camera_at_channel, debug_str, strlen(debug_str));
}

int32_t GC032a_ata_open(cm_cam_sensor_ata_t *ata)
{
    return 0;
}

int32_t GC032a_update_aec(cm_cam_sensor_aec_t *aec)
{
    int ret = 0;
//    uint32_t val1,val2,val3 = 0;
    uint32_t step_low = 0, step_high = 0;
    uint32_t step = 0;
    uint32_t min_exp = 0, max_exp = 0;
    uint32_t min_exp_l = 0, min_exp_h = 0;
    uint32_t max_exp_l = 0, max_exp_h = 0;
    uint32_t min_fps = 0, max_fps = 0;

    cm_cam_sensor_i2c_attr *i2c_attr = GC032a_i2c_attr;

    if (NULL == aec) {
        CM_CAM_DEMO_LOG("aec is NULL, return error!\n");
        return 0;
    }
    min_fps = aec->min_fps;
    max_fps = aec->max_fps;
    ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0xfe,0x01);
    ret = cm_camera_sensor_i2c_read(i2c_attr,0x25,&step_high);   // exp line(10ms)
    ret = cm_camera_sensor_i2c_read(i2c_attr,0x26,&step_low);
    ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0xfe,0x00);
    if(aec->sensor_out_yuv){
        CM_CAM_DEMO_LOG("GC032a set_fps [%d-%d] banding=%d \n",aec->min_fps, aec->max_fps, aec->banding);
        if(min_fps > max_fps){
            CM_CAM_DEMO_LOG("set_fps param error,min_fps must small or equal than max_fps \n");
            return 0;
        }
        if(0 == min_fps)
            min_fps =1;
        if(0 == max_fps)
            max_fps =1;

        step = ((step_high & 0x1f) << 0x8) | step_low;

        min_exp = 100 / (float)max_fps * step;
        max_exp = 100 / (float)min_fps * step;
        CM_CAM_DEMO_LOG("set_fps step =0x%x min_exp = 0x%x max_exp = 0x%x\n",step, min_exp,max_exp);

        min_exp_l = min_exp & 0xff;
        min_exp_h = (min_exp >> 0x8) & 0xff;

        max_exp_l = max_exp & 0xff;
        max_exp_h = (max_exp >> 0x8) & 0xff;

        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0xfe,0x01);
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x27,max_exp_h);//exp  level 1
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x28,max_exp_l);
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x29,max_exp_h);//exp  level 2
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x2a,max_exp_l + 0x1);
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x2b,max_exp_h);//exp  level 3
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x2c,max_exp_l + 0x2);
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x2d,max_exp_h);//exp  level 4
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x2e,max_exp_l + 0x3);
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x2f,max_exp_h);//exp  level 5
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x30,max_exp_l + 0x4);
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x31,max_exp_h);//exp  level 6
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x32,max_exp_l + 0x5);
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x33,max_exp_h);//exp  level 7
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x34,max_exp_l + 0x6);

        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x3c, 0x0); // max_exp_level[5:4] set 0, MaxLevel=1
        if( 30 > max_fps ){
            ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x3c,min_exp_h); // max_exp_level[5:4] set 0, MaxLevel=1
            ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0x3d,min_exp_l); //min_exp [11:0]
        }
        ret = cm_camera_sensor_i2c_write_single(i2c_attr, 0xfe,0x00);
    } else {
    }

    return ret;
}


#if 0
void GC032a_pin_init(GC032A_Handle *hdl, void (*callback)(void *data), void *data, uint16_t width, uint16_t height)
{
    OS_ASSERT(g_gc032aPinConfig);

    GC032A_PinConfig *pinConfig = g_gc032aPinConfig;
    SPICAM_Handle *spiCamHdl = &hdl->spiCam;

    hdl->pinConfig = pinConfig;

    hdl->cbEvent = callback;
    hdl->userData = data;
    hdl->ctrl = 0;

    GC032A_PowerControl(hdl, true);

    // MCLK IO
    PIN_SetMux(GC032A_PIN_RES_MCLK(pinConfig), GC032A_PIN_MUX_MCLK(pinConfig));               // MCLK 没有专用管脚，仅仅复用到GPIO管脚
    GPIO_SetDir(GC032A_PIN_RES_MCLK(pinConfig), GPIO_OUTPUT);
    PIN_SetMux(GC032A_PIN_RES_POWER_DOWN(pinConfig), GC032A_PIN_MUX_POWER_DOWN(pinConfig));   // SPI cam cs
    PIN_SetMux(GC032A_PIN_RES_SPI_CLK(pinConfig), GC032A_PIN_MUX_SPI_CLK(pinConfig));         // clk
    PIN_SetMux(GC032A_PIN_RES_SPI_DI0(pinConfig), GC032A_PIN_MUX_SPI_DI0(pinConfig));         // di0
    PIN_SetMux(GC032A_PIN_RES_SPI_DI1(pinConfig), GC032A_PIN_MUX_SPI_DI1(pinConfig));         // di1

    //Power On
    GPIO_SetDir(GC032A_PIN_RES_POWER_DOWN(pinConfig), GPIO_OUTPUT);
    GPIO_Write(GC032A_PIN_RES_POWER_DOWN(pinConfig), GPIO_LOW);
    osThreadMsSleep(10);
    GPIO_Write(GC032A_PIN_RES_POWER_DOWN(pinConfig), GPIO_HIGH);
    osThreadMsSleep(10);
    GPIO_Write(GC032A_PIN_RES_POWER_DOWN(pinConfig), GPIO_LOW);
    osThreadMsSleep(10);

    // SPICAM init
    spiCamHdl->cbEvent = SPICAM_CallBack;
    spiCamHdl->userData = hdl;
    spiCamHdl->res  = DRV_RES(CAM, 0);

    if(DRV_OK != SPICAM_Initialize(spiCamHdl)) {
        GC032A_PrintError("SPICAM Init Fail");
        GC032A_PowerControl(hdl, false);
        return -1;
    }

    SPICAM_Control(spiCamHdl, SPICAM_CONFIG_FRAME_WIDTH, width);
    SPICAM_Control(spiCamHdl, SPICAM_CONFIG_FREME_HEIGHT, height);
    SPICAM_Control(spiCamHdl, SPICAM_CONFIG_FRAME_DEPTH, CAMERA_BUFFER_DEPTH);
    SPICAM_Control(spiCamHdl, SPICAM_CONFIG_LINE_NUM, SPI_CAM_2_WIRE);

    SPICAM_PowerControl(spiCamHdl, DRV_POWER_FULL);

    if(GC032A_DrvInit(hdl, GC032A_I2C_BUS_NUM(pinConfig), width, height)) {
        GC032A_PrintError("GC032A Init Fail");
        return -1;
    }

    hdl->ctrl |= GC032A_INIT;

#ifdef OS_USING_PM
    PSM_WakelockInit(&hdl->wakeLock, PSM_DEEP_SLEEP);
#endif

    GC032A_PrintDebug("GC032A init");

    return 0;
}
#endif


int32_t GC032a_s_power(int32_t on)
{
    CM_CAM_DEMO_LOG("GC032a_s_power %d\n", on);

#ifdef ML307A
    cm_gpio_cfg_t cfg = {0, CM_GPIO_DIRECTION_OUTPUT, CM_GPIO_PULL_UP};
    cm_iomux_set_pin_func(CM_IOMUX_PIN_16, CM_IOMUX_FUNC_FUNCTION2); // pwdn1
    cm_gpio_init(CM_GPIO_NUM_18, &cfg);
    cm_gpio_set_level(CM_GPIO_NUM_18, CM_GPIO_LEVEL_LOW);
    cm_iomux_set_pin_func(CM_IOMUX_PIN_48, CM_IOMUX_FUNC_FUNCTION3); // cam_en
    cm_gpio_init(CM_GPIO_NUM_31, &cfg);
    cm_gpio_set_level(CM_GPIO_NUM_31, CM_GPIO_LEVEL_LOW);

    if (on > 0) {
        cm_camera_sensor_set_mclk(0);
        osDelay(1); //delay 5ms
        cm_gpio_set_level(CM_GPIO_NUM_31, CM_GPIO_LEVEL_HIGH);
        osDelay(1);
        cm_camera_sensor_set_mclk(1);
        osDelay(1);
        cm_gpio_set_level(CM_GPIO_NUM_18, CM_GPIO_LEVEL_HIGH);
        osDelay(2);
        cm_gpio_set_level(CM_GPIO_NUM_18, CM_GPIO_LEVEL_LOW);
    } else {
        cm_gpio_set_level(CM_GPIO_NUM_18, CM_GPIO_LEVEL_HIGH);
        osDelay(1); //delay 5ms
        cm_camera_sensor_set_mclk(0);
        osDelay(1);
        cm_gpio_set_level(CM_GPIO_NUM_31, CM_GPIO_LEVEL_LOW);
        osDelay(1);
        cm_gpio_set_level(CM_GPIO_NUM_18, CM_GPIO_LEVEL_LOW);
    }
#else
    //sensor GPIO init

    // power on
    if(on)
    {
        EXTLDO_Acquire(EXT_LDO_1V8);
        EXTLDO_Acquire(EXT_LDO_2V8);

        // mclock

    }
    else
    {
        EXTLDO_Release(EXT_LDO_1V8);
        EXTLDO_Release(EXT_LDO_2V8);
    }

#endif

    return 0;
}

cm_cam_sensor_spec_ops GC032a_ops = {
    NULL,
    NULL,
    NULL,
    GC032a_s_power,
    GC032a_ata_open,
    GC032a_update_aec,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

static cm_isp_tuning_tab gc032a_tuning_tab[] = {
    //LSC
    {0x680,0x00},    //0C
    {0x684,0x78},    //F0center_row
    {0x688,0xA0},    //A0center_col

    {0x0690,0x77},    //sign of b1
    {0x0694,0x77},    //sign of b1
    {0x0698,0x77},    //sign of b4
    {0x069C,0x70},    //sign of b4
    {0x06A0,0x77},    //sign of b22
    {0x06A4,0x70},    //sign of b22
    {0x06A8,0x03},    //Q1_b1 of R
    {0x06AC,0x1c},    //Q1_b1 of G
    {0x06B0,0x1f},    //Q1_b1 of B
    {0x06B4,0x1c},    //Q2_b1 of R
    {0x06B8,0x44},    //Q2_b1 of G
    {0x06BC,0x2b},    //Q2_b1 of B
    {0x06C0,0x57},    //Q3_b1 of R
    {0x06C4,0x6f},    //Q3_b1 of G
    {0x06C8,0x6a},    //Q3_b1 of B
    {0x06CC,0x14},    //Q4_b1 of R
    {0x06D0,0x24},    //Q4_b1 of G
    {0x06D4,0x2e},    //Q4_b1 of B
    {0x06D8,0xee},    //right_b2 of R
    {0x06DC,0xb6},    //right_b2 of G
    {0x06E0,0x86},    //right_b2 of B
    {0x06E4,0x7f},    //right_b4 of R
    {0x06E8,0x7f},    //right_b4 of G
    {0x06EC,0x7f},    //right_b4 of B
    {0x06F0,0xb2},    //left_b2 of R
    {0x06F4,0x62},    //left_b2 of G
    {0x06F8,0x5e},    //left_b2 of B
    {0x06FC,0x7f},    //left_b4 of R
    {0x0700,0x7f},    //left_b4 of G
    {0x0704,0x7f},    //left_b4 of B
    {0x0708,0xd3},    //up_b2 of R
    {0x070C,0xb0},    //up_b2 of G
    {0x0710,0xa8},    //up_b2 of B
    {0x0714,0x7f},    //up_b4 of R
    {0x0718,0x7f},    //up_b4 of G
    {0x071C,0x7f},    //up_b4 of B
    {0x0720,0x2a},    //down_b2 of R
    {0x0724,0x14},    //down_b2 of G
    {0x0728,0x00},    //down_b2 of B
    {0x072C,0x80},    //down_b4 of R
    {0x0730,0x80},    //down_b4 of G
    {0x0734,0x80},    //down_b4 of B
    {0x0740,0xfe},    //right_up_b22 of R
    {0x0744,0xfe},    //right_up_b22 of G
    {0x0748,0xfe},    //right_up_b22 of B
    {0x074C,0xfe},    //right_down_b22 of R
    {0x0750,0xfe},    //right_down_b22 of G
    {0x0754,0x00},    //right_down_b22 of B
    {0x0758,0xfe},    //left_up_b22 of R
    {0x075C,0xfe},    //left_up_b22 of G
    {0x0760,0xfe},    //left_up_b22 of B
    {0x0764,0xff},    //left_down_b22 of R
    {0x0768,0xff},    //left_down_b22 of G
    {0x076C,0xff},    //left_down_b22 of B
    {0x077C,0x05},

    //AWB
    {0x053C,0x00},
    {0x053C,0x00},
    {0x052C,0x01},
    {0x053C,0x00},

    {0x0530,0x01},//D65
    {0x0534,0x8f},
    {0x0538,0x02},
    {0x0530,0x01},
    {0x0534,0x90},
    {0x0538,0x02},
    {0x0530,0x01},
    {0x0534,0xae},
    {0x0538,0x02},
    {0x0530,0x01},
    {0x0534,0xaf},
    {0x0538,0x02},
    {0x0530,0x01},
    {0x0534,0xb0},
    {0x0538,0x02},

    {0x0530,0x01},//D50
    {0x0534,0x8c},
    {0x0538,0x03},
    {0x0530,0x01},
    {0x0534,0x8d},
    {0x0538,0x03},
    {0x0530,0x01},
    {0x0534,0xac},
    {0x0538,0x03},
    {0x0530,0x01},
    {0x0534,0xad},
    {0x0538,0x03},
    {0x0530,0x01},
    {0x0534,0xae},
    {0x0538,0x03},

    {0x0530,0x01},//CWF
    {0x0534,0xab},
    {0x0538,0x04},
    {0x0530,0x01},
    {0x0534,0xca},
    {0x0538,0x04},
    {0x0530,0x01},
    {0x0534,0xcb},
    {0x0538,0x04},

    {0x0530,0x01},//TL84
    {0x0534,0xeb},
    {0x0538,0x05},
    {0x0530,0x02},
    {0x0534,0x0a},
    {0x0538,0x05},

    {0x0530,0x02},//A
    {0x0534,0x6a},
    {0x0538,0x06},
    {0x0530,0x02},
    {0x0534,0x6b},
    {0x0538,0x06},
    {0x0530,0x02},
    {0x0534,0x8a},
    {0x0538,0x06},
    {0x0530,0x02},
    {0x0534,0x8B},
    {0x0538,0x06},
    {0x0530,0x02},
    {0x0534,0xaa},
    {0x0538,0x06},

    {0x053C,0x01},
    {0x0548,0x05},
    {0x0550,0x1C},
    {0x0574,0x13},
    {0x05d8,0x56},
    {0x05dc,0xa0},
    {0x05e0,0xa0},

    //CC
    {0x0B04,0x59},//D
    {0x0B08,0xF9},
    {0x0B0C,0x05},
    {0x0B10,0xF3},
    {0x0B14,0x5D},
    {0x0B18,0xF2},

    {0x0B1C,0x68},//C
    {0x0B20,0xF3},
    {0x0B24,0x00},
    {0x0B28,0xEC},
    {0x0B2C,0x5F},
    {0x0B30,0xEA},

    {0x0B34,0x5D},//A
    {0x0B38,0xF7},
    {0x0B3C,0xF5},
    {0x0B8c,0x08},
    {0x0B90,0x64},
    {0x0B94,0xCD},

    //YCP
    {0x0B44,0x2e},
    {0x0B48,0x2e},

    //AE_Target
    {0x044C,0x50},

    //block enable
    {0x0200,0x79},

    //////////////frame rate   50Hz
    {0x0494 ,0x00},
    {0x0498 ,0x9c}, //step
    {0x049C ,0x01}, //30fps  level 1
    {0x04A0 ,0xd4},
    {0x04A4 ,0x02},//25fps level 2
    {0x04A8 ,0x70},
    {0x04AC ,0x03}, //20fps level 3
    {0x04B0 ,0x0c},
    {0x04B4 ,0x03}, //16.7fps level 4
    {0x04B8 ,0xa8},
    {0x04BC ,0x06}, // 10fps level 5
    {0x04C0 ,0x18},
    {0x04C4 ,0x08}, // 7.14fps level 6
    {0x04C8 ,0x88},
    {0x04CC ,0x0c}, // 5fps level 7
    {0x04D0 ,0x30},

    //{0x04f0 ,0x60}, // max_exp_level[6:5]
    {0x04f4 ,0x04}, //min_exp [12:0]

};

static cm_isp_tuning_tab gc032a_50hz_banding_tab[] = {
    //////////////frame rate   50Hz
    {0x0494 ,0x00},
    {0x0498 ,0x9b}, //step
    {0x049C ,0x01}, //30fps  level 1
    {0x04A0 ,0xd4},
    {0x04A4 ,0x02},//25fps level 2
    {0x04A8 ,0x70},
    {0x04AC ,0x03}, //20fps level 3
    {0x04B0 ,0x0c},
    {0x04B4 ,0x03}, //16.7fps level 4
    {0x04B8 ,0xa8},
    {0x04BC ,0x06}, // 10fps level 5
    {0x04C0 ,0x18},
    {0x04C4 ,0x08}, // 7.14fps level 6
    {0x04C8 ,0x88},
    {0x04CC ,0x0c}, // 5fps level 7
    {0x04D0 ,0x30},

    //{0x04f0 ,0x60}, // max_exp_level[6:5]
    {0x04f4 ,0x04}, //min_exp [12:0]
};

static cm_isp_tuning_tab gc032a_60hz_banding_tab[] = {
    //////////////frame rate   50Hz
    {0x0494 ,0x00},
    {0x0498 ,0x82}, //step
    {0x049C ,0x01}, //30fps  level 1
    {0x04A0 ,0x86},
    {0x04A4 ,0x02},//25fps level 2
    {0x04A8 ,0x8a},
    {0x04AC ,0x03}, //20fps level 3
    {0x04B0 ,0x0c},
    {0x04B4 ,0x03}, //16.7fps level 4
    {0x04B8 ,0x8e},
    {0x04BC ,0x05}, // 10fps level 5
    {0x04C0 ,0x96},
    {0x04C4 ,0x08}, // 7.14fps level 6
    {0x04C8 ,0x20},
    {0x04CC ,0x0c}, // 5fps level 7
    {0x04D0 ,0x30},

    //{0x04f0 ,0x60}, // max_exp_level[6:5]
    {0x04f4 ,0x04}, //min_exp [12:0]
};

cm_cam_sensor_data_t GC032a_spi = {
    .name = "gc032a_spi",
    .ops  = &GC032a_ops,
    .position = 0,
    .i2c_attr = GC032a_i2c_attr,
    .num_i2c_attr = ARRAY_SIZE(GC032a_i2c_attr),
    .id = {
        .tab = GC032a_id,
        .num = ARRAY_SIZE(GC032a_id),
        },
    .global_setting = NULL,
    .num_global = 0,
    .res = GC032a_resolution,
    .num_res = ARRAY_SIZE(GC032a_resolution),
    .streamon = {
        .tab = GC032a_stream_on,
        .num = ARRAY_SIZE(GC032a_stream_on),
        },
    .streamoff = {
        .tab = GC032a_stream_off,
        .num = ARRAY_SIZE(GC032a_stream_off),
        },
    .expo_reg = {
        .tab = GC032a_expo,
        .num = ARRAY_SIZE(GC032a_expo),
        },
    .vts_reg = {
        .tab = GC032a_vts,
        .num = ARRAY_SIZE(GC032a_vts),
    },
    .gain_reg = {
        .tab = GC032a_gain,
        .num = ARRAY_SIZE(GC032a_gain),
        },
    .hflip = {
        .tab = GC032a_hflip,
        .num = ARRAY_SIZE(GC032a_hflip),
        },
    .vflip = {
        .tab = GC032a_vflip,
        .num = ARRAY_SIZE(GC032a_vflip),
        },
    .module     = GC032a_MODULE_INFO,
    .num_module = ARRAY_SIZE(GC032a_MODULE_INFO),
    .tuning_tab     = gc032a_tuning_tab,
    .num_tuning_tab = ARRAY_SIZE(gc032a_tuning_tab),
    .banding_tab = {
        {
            .tab = gc032a_50hz_banding_tab,
            .num = ARRAY_SIZE(gc032a_50hz_banding_tab),
        },
        {
            .tab = gc032a_60hz_banding_tab,
            .num = ARRAY_SIZE(gc032a_60hz_banding_tab),
        },
    },
    .interface_type = 0,    //CAM_SPI: spi    CAM_MIPI: mipi
    .lane_num       = 1,    // .lane_num
    .spi_sdr        = 0,    //0x0:sdr   0x1:ddr spi_sdr
    .spi_crc        = 0,    //0x0:no crc   0x1:crc,.spi_crc
    .reset_delay    = 100,  //.reset_delay
    .cur_i2c_index  = 0,    //auto set .cur_i2c_index
    .cur_res_index  = 1,    //int cur_res_index
};

//摄像头数据回调函数
static void cm_test_enqueueCamRecordBuffer_callback(void *pBuf, uint32_t len)
{
    enqueueCamRecordBuffer_counter++;
    if (enqueueCamRecordBuffer_counter % 15 == 0)
    {
        CM_CAM_DEMO_LOG("\r\nenqueueCamRecordBuffer_counter=%d\n", enqueueCamRecordBuffer_counter);
        //osPrintf("preview frame=%d\n", enqueueCamRecordBuffer_counter);
        //camera_debug_at_log(0, enqueueCamRecordBuffer_counter);
    }

/*    static int state = 0;
    if(state == 0)
    {
        state = 1;
        int fd = cm_fs_open(CAPTURE_FILE_NAME, CM_FS_WBPLUS);
        if(fd < 0)
        {
            CM_CAM_DEMO_LOG("cm_fs_open fail\n");
            return;
        }
        if(cm_fs_write(fd, pBuf, len) < 0)
        {
            CM_CAM_DEMO_LOG("cm_fs_write fail\n");
        }
        cm_fs_close(fd);
    }*/
}


static void cm_test_flushCamRecordBuffers_callback(void)
{
    CM_CAM_DEMO_LOG("preview stop\n");
//    camera_debug_at_log(1, 0);
}


void cm_test_camera_preview(unsigned char **cmd, int len)
{
    uint32_t width = 0;
    uint32_t height = 0;
    cm_camera_preview_cfg_t cfg = {0};
    CM_CAM_DEMO_LOG("cm_test_camera_preview\n");
    GC032a_spi.res = GC032a_resolution_preview; // 320x240
    cm_camera_sensor_set_data(&GC032a_spi);
    if (cm_camera_open() < 0)
    {
        CM_CAM_DEMO_LOG("cm_camera_open fail\n");
        return;
    }
    cm_camera_get_info(&width, &height);
    CM_CAM_DEMO_LOG("width[%d]height[%d]", width, height);
    cfg.recordFormat           = CAM_OUT_FMT_RGB565;
    cfg.enqueueCamRecordBuffer = cm_test_enqueueCamRecordBuffer_callback;
    cfg.flushCamRecordBuffers  = cm_test_flushCamRecordBuffers_callback;
    cm_camera_preview_start(&cfg);
    osDelay(g_camera_previw_duration_sec * 1000);
    cm_camera_preview_stop();
    cm_camera_close();
    return;
}

int32_t cm_test_camera_capture(unsigned char **cmd, int format, int save_to_file)
{
    uint32_t width = 0;
    uint32_t height = 0;
    CM_CAM_DEMO_LOG("cm_test_camera_capture[%d][%d]", format, save_to_file);

    GC032a_spi.res = GC032a_resolution;    //640x480
    cm_camera_sensor_set_data(&GC032a_spi);
    int ret = cm_camera_open();
    if(ret < 0)
    {
        CM_CAM_DEMO_LOG("ERR:cm_camera_open fail");
        return ret;
    }

    cm_camera_get_info(&width, &height);
    CM_CAM_DEMO_LOG("width[%d]height[%d]", width, height);
    cm_camera_capture_cfg_t cfg = {0};
    cfg.recordFormat = format;
    if (save_to_file != 0)
    {
        if (format == CAM_OUT_FMT_JPEG)
        {
            cfg.file_name = (uint8_t *)CAPTURE_FILE_NAME_JPEG;
        }
        else
        {
            cfg.file_name = (uint8_t *)CAPTURE_FILE_NAME_RGB;
        }
        cfg.image.buf_size = 0; // save to file
    }
    else
    {
        cfg.image.buf_size = 1; // output to user
    }
    CM_CAM_DEMO_LOG("cfg:format[%d]buf_size[%u]out[%X]", cfg.recordFormat, cfg.image.buf_size, cfg.image.p_buf);

    ret = cm_camera_capture_take(&cfg);
    CM_CAM_DEMO_LOG("ret[%d]image:buf_size[%u]out[%X]", ret, cfg.image.buf_size, cfg.image.p_buf);

    cm_camera_close();

    if ((cfg.image.buf_size != 0) && (ret == CM_CAM_ERR_OK))
    {
        if (cfg.image.p_buf != NULL)
        {
            cm_free(cfg.image.p_buf);
            cfg.image.p_buf = NULL;
        }
        else
        {
            CM_CAM_DEMO_LOG("ERR: *(cfg.image.pp_buf) NULL");
        }
    }
    CM_CAM_DEMO_LOG("take done");

    return ret;
}

void at_camera_set_handler(uint32_t mode)
{
    int32_t ret = CM_CAM_ERR_OK;
    switch (mode)
    {
    case 1:
        cm_test_camera_preview(NULL, 0);
        camera_debug_at_log(1, 0);
        break;
    case 2:
        ret = cm_test_camera_capture(NULL, CAM_OUT_FMT_RGB565, 1);
        camera_debug_at_log(2, ret);
        break;
    case 3:
        ret = cm_test_camera_capture(NULL, CAM_OUT_FMT_JPEG, 1);
        camera_debug_at_log(3, ret);
        break;
    case 4:
        ret = cm_test_camera_capture(NULL, CAM_OUT_FMT_JPEG, 0);
        camera_debug_at_log(4, ret);
        break;

        break;
    default:
        CM_CAM_DEMO_LOG("ERR:unsupport %d\n", mode);
        break;
    }
}

void at_camera_set(uint8_t id, uint8_t *para_p, uint16_t lenOfPara)
{
    uint32_t mode          = 0;
    uint8_t *para_p_new    = para_p;
    uint16_t lenOfPara_new = lenOfPara;

    at_errno_t at_errono = xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%u", &mode);
    if (AT_ERRNO_NO_ERROR != at_errono)
    {
        AT_SendResponseCMEError(id, 50);
        return;
    }

    at_errono = xs_at_parse_cm(&para_p_new, &lenOfPara_new, "$,%u", &g_camera_previw_duration_sec);
    if (AT_ERRNO_NO_ERROR != at_errono)
    {
        g_camera_previw_duration_sec = 5;
    }

    g_camera_at_channel = id;

    at_camera_set_handler(mode);

    AT_SendResponseOK(id);
}

void at_camera_get(uint8_t id)
{
    AT_SendResponseOK(id);
}

void at_camera_test(uint8_t id)
{
    AT_SendResponse(id, "+MGPIO: (1-3)\r\n\r\nOK\r\n", strlen("+MGPIO: (1-3)\r\n\r\nOK\r\n"));
}

void at_camera_exec(uint8_t id)
{
    AT_SendResponseOK(id);
}

AT_CMD_TABLE_SECTION_USER AT_CommandItem g_camera_at_tables[] =
{
    {.name = "+CAMTEST", .setFunc = at_camera_set, .readFunc = NULL, .testFunc = at_camera_test, .execFunc = NULL},
};

#endif //CM_DEMO_CAMERA_SUPPORT

