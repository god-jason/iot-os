/**
 * @file        cm_camera.h
 * @brief       相机接口
 * @copyright   Copyright @2023 China Mobile IOT. All rights reserved.
 * @author      By chenxy
 * @date        2023/03/13
 *
 * @defgroup camera
 * @ingroup camera
 * @{
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include "os.h"
#include "cm_camera.h"

#include "cmo_camera.h"

#include "gc032a.h"
#include "drv_capture.h"
#include "drv_display.h"
#include "cm_fs.h"

#ifdef USE_LIBJPEG_TURBO
#include "jpeg.h"
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

//打印LOG 需根据平台适配
#define CAMERA_DEBUG_LOG_SHELL
#ifdef CAMERA_DEBUG_LOG_SHELL
#define cam_log(fmt, args...)       os_kprintf("[CAM]%s %u:"   fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#define cam_log_debug(fmt, args...) os_kprintf("[CAM_D]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#define cam_log_err(fmt, args...)   os_kprintf("[CAM_E]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#else
#define cam_log(fmt, args...)       app_printf("[CAM]%s %u:"   fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#define cam_log_debug(fmt, args...) app_printf("[CAM_D]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#define cam_log_err(fmt, args...)   app_printf("[CAM_E]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#endif
#define cam_log_sh(fmt, args...)    os_kprintf("[CAM]%s %u:"   fmt "\r\n", __FUNCTION__, __LINE__,  ##args)

#define CM_DISPLAY_TEST_MAX_FPS                     0
//#define CAMERA_PREVIEW_WITH_LCD
#define CM_CAMERA_OP_TIMEOUT_SECOND                 (10 * 1000)
//#define CM_CAMERA_CAPTURE_TAKE_JPEG_FILE_SIZE_MAX   (30 * 1024)

/****************************************************************************
 * Public Types
 ****************************************************************************/
typedef enum
{
    CAMERA_PARA_TYPE_RESOLUTION,
    CAMERA_PARA_TYPE_MAX,
} cm_camera_para_type_e;



typedef enum
{
    CAMERA_STATE_NONE,
    CAMERA_STATE_OPEN,
    CAMERA_STATE_PREVIEW,
    CAMERA_STATE_CAPTURE,
    CAMERA_STATE_MAX,
} cm_camera_state_e;

typedef GC032A_Handle camera_handler_t;
typedef CaptureDevice_t camera_device_t;


/**
 * @brief camera预览接口配置参数
 */
typedef struct
{
    bool opened;
    bool preview_running;

    cm_camera_state_e state;
    camera_device_t * p_device;
    camera_handler_t  handler;
    cm_camera_preview_cfg_t preview_cfg;
    osThreadId_t task_id;
} cm_camera_mgr_t;



/****************************************************************************
 * Public Data
 ****************************************************************************/
static cm_camera_mgr_t g_camera_mgr = {0};

#ifdef CAMERA_PREVIEW_WITH_LCD
extern void Video_Open(void);
extern void Video_Close(void);

extern int Video_DispInit(void (*callback)(void *data), void *data);
extern int Video_DispDeInit(void);
extern int Video_DispFlip(void);
extern int Video_DispFlush(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint8_t *p_image);

#define CAMERA_LCD_PREVIEW_HEIGHT   320
#define CAMERA_LCD_PREVIEW_WIDTH    240

static osCompletion g_sem_camera_display_done = {0};
uint32_t g_camera_display_counter             = 0;

DispDevice_t *g_dispDev = NULL;
#endif

static osCompletion g_sem_camera_capture_done = {0};
static osCompletion g_sem_camera_shot_done    = {0};
static osCompletion g_sem_camera_stop_done    = {0};
static osCompletion g_sem_camera_save_done    = {0};

uint8_t g_camera_capture_shot_flag            = 0;
uint8_t g_camera_stop_flag                    = 0;

uint8_t *g_p_camera_capture_shot_img          = NULL;

uint32_t g_camera_preview_counter             = 0;

/*
GC032A_PinConfig g_gc032aPin =
{
    .i2cBusNum = BSP_I2C_BUS_NUM,
    .pinPowerDown = {.pinRes = SPICAM_POWERDOWN_PIN_RES, .pinMux = SPICAM_POWERDOWN_PIN_MUX},
    .pinMclk      = {.pinRes = SPICAM_MCLK_PIN_RES, .pinMux = SPICAM_MCLK_PIN_MUX},
    .pinSpiClk    = {.pinRes = SPICAM_CLK_PIN_RES, .pinMux = SPICAM_CLK_PIN_MUX},
    .pinSpiDi0    = {.pinRes = SPICAM_DI0_PIN_RES, .pinMux = SPICAM_DI0_PIN_MUX},
    .pinSpiDi1    = {.pinRes = SPICAM_DI0_PIN_RES, .pinMux = SPICAM_DI1_PIN_MUX},
};
*/

/*
static GC032A_PIN_DEFINE(g_gc032aPin, BSP_I2C_BUS_NUM,
    SPICAM_POWERDOWN_PIN_RES, SPICAM_POWERDOWN_PIN_MUX,
    SPICAM_MCLK_PIN_RES, SPICAM_MCLK_PIN_MUX,
    SPICAM_CLK_PIN_RES, SPICAM_CLK_PIN_MUX,
    SPICAM_DI0_PIN_RES, SPICAM_DI0_PIN_MUX,
    SPICAM_DI1_PIN_RES, SPICAM_DI1_PIN_MUX);
*/

// use ICT ISP driver
/*
static GC032A_PinConfig g_cameraPinConfig =
{
    .i2cBusNum = BSP_I2C_BUS_NUM,
    .pinPowerDown = {.pinRes = SPICAM_POWERDOWN_PIN_RES, .pinMux = SPICAM_POWERDOWN_PIN_MUX},
    .pinMclk      = {.pinRes = SPICAM_MCLK_PIN_RES, .pinMux = SPICAM_MCLK_PIN_MUX},
    .pinSpiClk    = {.pinRes = SPICAM_CLK_PIN_RES, .pinMux = SPICAM_CLK_PIN_MUX},
    .pinSpiDi0    = {.pinRes = SPICAM_DI0_PIN_RES, .pinMux = SPICAM_DI0_PIN_MUX},
    .pinSpiDi1    = {.pinRes = SPICAM_DI0_PIN_RES, .pinMux = SPICAM_DI1_PIN_MUX},
};
*/

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


static int32_t cm_camera_pin_init(void)
{
    // 初始化引脚配置（这里需要根据实际硬件配置）
//    GC032A_PinInit(&g_cameraPinConfig);
    extern int Board_CamPinInit(void);
    Board_CamPinInit();

    return CM_CAM_ERR_OK;
}

static int32_t cm_camera_pin_deinit(void)
{

    return CM_CAM_ERR_OK;
}

static int32_t cm_camera_i2c_init(void)
{

    return CM_CAM_ERR_OK;
}

static int32_t cm_camera_i2c_deinit(void)
{

    return CM_CAM_ERR_OK;
}

typedef enum
{
    CM_CAMERA_SENSOR_DATA_TYPE_I2C,
    CM_CAMERA_SENSOR_DATA_TYPE_RESOLUTION,
    CM_CAMERA_SENSOR_DATA_TYPE_MAX,
} cm_camera_sensor_data_type_e;


static int32_t cm_camera_para_valid(cm_cam_sensor_data_t *p_sensor_data)
{
    int32_t ret = CM_CAM_ERR_OK;
#if 0
    if (p_sensor_data == NULL)
    {
        cam_log_err("p_sensor_data NULL");

        return CM_CAM_ERR_INVALID_PARA;
    }

    if (p_sensor_data->res != NULL)
    {
        uint8_t res_index = (p_sensor_data->cur_res_index < 1) ? 0 : (p_sensor_data->cur_res_index - 1);
        cm_cam_sensor_resolution *p_sensor_res = p_sensor_data->res + res_index;
        if ((p_sensor_res->height > p_sensor_res->height_max) ||
            (p_sensor_res->height > p_sensor_res->height_max))
        {
            cam_log("res_index[%d][%ux%u][%ux%u]", res_index,
                    p_sensor_res->height, p_sensor_res->width,
                    p_sensor_res->height_max, p_sensor_res->width_max);
            return CM_CAM_ERR_INVALID_PARA;
        }
    }
    else
    {
        cam_log_err("res NULL");
        ret = CM_CAM_ERR_INVALID_PARA;
    }
#endif

    return ret;
}


static int32_t cm_cameara_sensor_data_query(cm_camera_sensor_data_type_e type_id, void *p_data)
{
    if (p_data == NULL)
    {
        cam_log_err("p_data NULL");
        return CM_CAM_ERR_INVALID_PARA;
    }

    cm_cam_sensor_data_t * p_sensor_data = cm_camera_sensor_get_data();
    if (p_sensor_data == NULL)
    {
        cam_log_err("p_sensor_data NULL");
        return CM_CAM_ERR_WRONG_CFG_DATA;
    }

    int32_t ret = CM_CAM_ERR_OK;
    switch (type_id)
    {
    case CM_CAMERA_SENSOR_DATA_TYPE_I2C:
        cm_cam_sensor_i2c_attr *p_i2c = (cm_cam_sensor_i2c_attr *)p_data;
        if (p_sensor_data->i2c_attr != NULL)
        {
            uint8_t i2c_index = (p_sensor_data->cur_i2c_index < 1) ? 0 : (p_sensor_data->cur_i2c_index - 1);
            memcpy(p_i2c, p_sensor_data->i2c_attr + i2c_index, sizeof(cm_cam_sensor_i2c_attr));
        }
        else
        {
            cam_log_err("i2c_attr NULL");
            ret = -3;
        }
        break;
    case CM_CAMERA_SENSOR_DATA_TYPE_RESOLUTION:
        cm_cam_sensor_resolution *p_res = (cm_cam_sensor_resolution *)p_data;
        if (p_sensor_data->res != NULL)
        {
            uint8_t res_index = (p_sensor_data->cur_res_index < 1) ? 0 : (p_sensor_data->cur_res_index - 1);
            memcpy(p_res, p_sensor_data->res + res_index, sizeof(cm_cam_sensor_resolution));
            cam_log("res_index[%d][%ux%u]", res_index, p_res->height, p_res->width);
        }
        else
        {
            cam_log_err("res NULL");
            ret = -4;
        }

        break;
    default:
        cam_log_err("unknow type_id=%u", type_id);
        ret = 0xFFFF;
        break;
    }

    return ret;
}

/**
 * @brief 获取底层驱动输出大小
 *
 * @param [out] width       宽度
 * @param [out] height      高度
 *
 * @return  0 成功；-1 失败
 *
 * @details 获取底层驱动输出大小
 */
int32_t cm_camera_get_info(uint32_t *width, uint32_t *height)
{
    if ((width == NULL) || (height == NULL))
    {
        cam_log_err("width[%X]height[%X]", width, height);
        return CM_CAM_ERR_INVALID_PARA;
    }

    cm_cam_sensor_resolution sensor_resolution = {0};
    int ret = cm_cameara_sensor_data_query(CM_CAMERA_SENSOR_DATA_TYPE_RESOLUTION, &sensor_resolution);
    if (ret != CM_CAM_ERR_OK)
    {
        cam_log_err("cam resolution[%d][%ux%u]", ret, sensor_resolution.height, sensor_resolution.width);
        return ret;
    }
    else
    {
        *width  = sensor_resolution.width;
        *height = sensor_resolution.height;
    }
    cam_log("cam resolution[%ux%u]", sensor_resolution.height, sensor_resolution.width);

    return CM_CAM_ERR_OK;
}

void cm_camera_data_handler(uint8_t *p_img, uint32_t img_size)
{

}

static void cm_cameara_task(void *param)
{
    CaptureDevice_t *p_camera_device = g_camera_mgr.p_device;
    OS_ASSERT(p_camera_device != NULL);

    cm_cam_sensor_resolution sensor_resolution = {0};
    int32_t ret = cm_cameara_sensor_data_query(CM_CAMERA_SENSOR_DATA_TYPE_RESOLUTION, &sensor_resolution);
    OS_ASSERT(ret == CM_CAM_ERR_OK);

    cam_log("cam resolution[%ux%u]", sensor_resolution.height, sensor_resolution.width);
    uint32_t cam_img_size = sensor_resolution.height * sensor_resolution.width * 2;
    uint8_t *p_camera_image = NULL;
    uint32_t no_camera_data_counter = 0;

    while (1)
    {
        osWaitForCompletion(&g_sem_camera_capture_done, osWaitForever);
        p_camera_image = p_camera_device->capture(p_camera_device);
        if (p_camera_image == NULL)
        {
            no_camera_data_counter++;
            cam_log_debug("no image=%u", no_camera_data_counter);
            osThreadSleep(50);
            continue;
        }

        //cam_log_debug("got new image");
        // notify user to receive preview data
        if (g_camera_mgr.preview_cfg.enqueueCamRecordBuffer != NULL)
        {
            g_camera_mgr.preview_cfg.enqueueCamRecordBuffer(p_camera_image, cam_img_size);
        }

#ifdef CAMERA_PREVIEW_WITH_LCD
        Video_DispFlush(0, CAMERA_LCD_PREVIEW_WIDTH - 1, 0, CAMERA_LCD_PREVIEW_HEIGHT - 1, p_camera_image);
        ret = osWaitForCompletion(&g_sem_camera_display_done, osWaitForever);
        if (ret != osOK)
        {
            cam_log_err("Video_DispFlush ret=%d", ret);
        }
#endif
        if (g_camera_capture_shot_flag)
        {
            cam_log_debug("capture shot");
            //memcpy(g_p_camera_capture_shot_img, p_camera_image, cam_img_size);
            g_p_camera_capture_shot_img = p_camera_image;
            osComplete(&g_sem_camera_shot_done);
            g_camera_capture_shot_flag = 0;

            // wait to finish saving
            ret = osWaitForCompletion(&g_sem_camera_save_done, CM_CAMERA_OP_TIMEOUT_SECOND);
            if (ret != osOK)
            {
                cam_log_err("save timeout ret=%d", ret);
            }

            p_camera_device->release(p_camera_device, p_camera_image);
            p_camera_image = NULL;
            break;
        }
        p_camera_device->release(p_camera_device, p_camera_image);
        p_camera_image = NULL;

        if (g_camera_stop_flag != 0)
        {
            cam_log_debug("capture stop");
            break;
        }
    }

    osComplete(&g_sem_camera_stop_done);
}

/**
 * @brief 摄像头回调函数
 *
 * @param data 用户数据
 */
static void cm_camera_preview_int_callback(void *p_user_data)
{
    g_camera_preview_counter++;

    //cam_log_debug("preview_counter[%u]", g_camera_preview_counter);

    if(p_user_data)
    {
        //cam_log_debug("sem g_sem_camera_capture_done release");
        osComplete(p_user_data);
    }
}

#ifdef CAMERA_PREVIEW_WITH_LCD
static void cm_camera_display_int_callback(void *p_user_data)
{
    g_camera_display_counter++;

    //cam_log_debug("display_counter[%u]", g_camera_display_counter);

    if (p_user_data != NULL)
    {
        //cam_log_debug("sem g_sem_camera_display_done release");
        osComplete(p_user_data);
    }
}
#endif

/**
 * @brief 打开相机
 *
 * @return  0 成功；-1 失败
 *
 * @details 打开相机
 */
int32_t cm_camera_open(void)
{
    if (g_camera_mgr.opened)
    {
        cam_log_debug("already open");
        return CM_CAM_ERR_OK;
    }

    cm_cam_sensor_data_t *p_sensor_data = cm_camera_sensor_get_data();
    if (p_sensor_data == NULL)
    {
        cam_log_err("p_senser_data NULL");
        return CM_CAM_ERR_WRONG_CFG_DATA;
    }

    if (CM_CAM_ERR_OK != cm_camera_para_valid(p_sensor_data))
    {
        cam_log_err("p_senser_data NULL");
        return CM_CAM_ERR_INVALID_PARA;
    }

    cm_cam_sensor_resolution sensor_resolution = {0};
    int ret = cm_cameara_sensor_data_query(CM_CAMERA_SENSOR_DATA_TYPE_RESOLUTION, &sensor_resolution);
    if (ret != CM_CAM_ERR_OK)
    {
        cam_log_err("ret[%d]", ret);
        return CM_CAM_ERR_INVALID_PARA;
    }

#ifdef CAMERA_PREVIEW_WITH_LCD
    sensor_resolution.width  = CAMERA_LCD_PREVIEW_WIDTH;
    sensor_resolution.height = CAMERA_LCD_PREVIEW_HEIGHT;
    cam_log("fixed resolution[%ux%u]", sensor_resolution.height, sensor_resolution.width);
#endif

    CaptureDevice_t * p_camera_device = CamGC032A_GetDevice();
    if (p_camera_device == NULL)
    {
        cam_log_err("p_camera_device NULL");
        return CM_CAM_ERR_NO_DEV;
    }
    p_camera_device->horRes = sensor_resolution.width;
    p_camera_device->verRes = sensor_resolution.height;
    cam_log("resolution[%u X %u]", p_camera_device->verRes, p_camera_device->horRes);

    g_camera_mgr.p_device = p_camera_device;

    g_camera_stop_flag = 0;

    g_sem_camera_stop_done.name = "stop";
    osInitCompletion(&g_sem_camera_stop_done);

    g_sem_camera_capture_done.name = "preview";
    osInitCompletion(&g_sem_camera_capture_done);

    g_sem_camera_shot_done.name = "shot";
    osInitCompletion(&g_sem_camera_shot_done);

    g_sem_camera_save_done.name = "save";
    osInitCompletion(&g_sem_camera_save_done);

#ifdef CAMERA_PREVIEW_WITH_LCD
    osInitCompletion(&g_sem_camera_display_done);
    ret = Video_DispInit(cm_camera_display_int_callback, &g_sem_camera_display_done);
    if (ret != CM_CAM_ERR_OK)
    {
        cam_log_err("ret[%d]", ret);
        return CM_CAM_ERR_UNKNOW;
    }
#endif

    ret = p_camera_device->init(p_camera_device, cm_camera_preview_int_callback, &g_sem_camera_capture_done);
    if(ret != CM_CAM_ERR_OK)
    {
        cam_log_err("ret[%d]", ret);
        return CM_CAM_ERR_INIT_FAIL;
    }
    osThreadAttr_t attr = {"cmCameraPreview", osThreadDetached, NULL, 0U, NULL, 2048, osPriorityISR - 25, 0U, 0U};
    g_camera_mgr.task_id = osThreadNew(cm_cameara_task, NULL, &attr);

    g_camera_mgr.opened = true;

    return CM_CAM_ERR_OK;
}

/**
 * @brief 关闭相机
 *
 * @return  0 成功；-1 失败
 *
 * @details 关闭相机
 */
int32_t cm_camera_close(void)
{
    if (g_camera_mgr.opened == false)
    {
        cam_log_debug("already closed");
        return CM_CAM_ERR_OK;
    }
#if 0
    g_camera_stop_flag  = 1; // notify camera task to exit and post stop sem

    osStatus_t status = osOK;
    do
    {
        status = osWaitForCompletion(&g_sem_camera_stop_done, osNoWait);
        osThreadSleep(100);
    } while (status != osOK);
    g_camera_mgr.opened = false;

    cam_log_debug("task exit done");
#endif
    g_camera_mgr.opened = false;

    CaptureDevice_t * p_camera_device = g_camera_mgr.p_device;

#ifdef CAMERA_PREVIEW_WITH_LCD
        Video_DispDeInit();
#endif
    int ret = p_camera_device->deinit(p_camera_device);
    cam_log_debug("deinit ret[%d]", ret);

    osThreadDetach(g_camera_mgr.task_id);
    osSemaphoreDetach(&g_sem_camera_stop_done);
#ifdef CAMERA_PREVIEW_WITH_LCD
    osSemaphoreDetach(&g_sem_camera_display_done);
#endif
    osSemaphoreDetach(&g_sem_camera_capture_done);
    osSemaphoreDetach(&g_sem_camera_shot_done);
    osSemaphoreDetach(&g_sem_camera_save_done);

    return CM_CAM_ERR_OK;
}

/**
 * @brief 开始预览
 *
 * @param [in] cfg          配置参数
 *
 * @return  0 成功；-1 失败
 *
 * @details 开始预览，数据通过cfg中设置的回调函数返回，回调函数中可
 * 调用cm_camera_preview_stop关闭预览，但不可调用cm_camera_close
 */
int32_t cm_camera_preview_start(cm_camera_preview_cfg_t *cfg)
{
    if (!g_camera_mgr.opened)
    {
        cam_log_err("camera not open");
        return CM_CAM_ERR_NOT_OPEN;
    }

    if (g_camera_mgr.preview_running)
    {
        cam_log_debug("camera preview already running");
        return CM_CAM_ERR_OK;
    }

    if (cfg == NULL)
    {
        cam_log_err("cfg NULL");
        return CM_CAM_ERR_WRONG_CFG_DATA;
    }

    g_camera_mgr.preview_cfg = *cfg;

    CaptureDevice_t * p_camera_device = g_camera_mgr.p_device;
    p_camera_device->start(p_camera_device);
    g_camera_mgr.preview_running = true;

    return CM_CAM_ERR_OK;
}

/**
 * @brief 关闭预览
 *
 * @return  0 成功；-1 失败
 *
 * @details 关闭预览
 */
int32_t cm_camera_preview_stop(void)
{
    if (!g_camera_mgr.opened || !g_camera_mgr.preview_running)
    {
        cam_log_debug("opened[%d]preview_running[%d]", g_camera_mgr.opened, g_camera_mgr.preview_running);
        return CM_CAM_ERR_OK;
    }

    g_camera_stop_flag  = 1; // notify camera task to exit and post stop sem

    osStatus_t status = osOK;
    do
    {
        status = osWaitForCompletion(&g_sem_camera_stop_done, osNoWait);
        osThreadSleep(100);
    } while (status != osOK);

    g_camera_mgr.preview_running = false;

    cam_log_debug("task exit done");

    CaptureDevice_t * p_camera_device = g_camera_mgr.p_device;
    int ret = p_camera_device->stop(p_camera_device);
    if (ret != 0)
    {
        cam_log_err("ret[%d]", ret);
        return CM_CAM_ERR_STOP_FAIL;
    }

    // 调用flush回调
    if (g_camera_mgr.preview_cfg.flushCamRecordBuffers)
    {
        g_camera_mgr.preview_cfg.flushCamRecordBuffers();
    }

    g_camera_mgr.preview_running = false;

    return CM_CAM_ERR_OK;
}


/**
 * @brief 拍照
 *
 * @param [in] cfg 配置参数
 *
 * @return  0 成功；-1 失败
 *
 * @details 拍照，数据自动保存至指定文件
 */
int32_t cm_camera_capture_take(cm_camera_capture_cfg_t *p_cfg)
{
    int32_t ret = CM_CAM_ERR_UNKNOW;

    if (g_camera_mgr.opened == false)
    {
        cam_log_err("camera not open");
        return CM_CAM_ERR_NOT_OPEN;
    }

    if (p_cfg == NULL)
    {
        cam_log_err("p_cfg NULL");
        return CM_CAM_ERR_WRONG_CFG_DATA;
    }

    if ((p_cfg->file_name == NULL) && (p_cfg->image.buf_size == 0))
    {
        cam_log_err("buf_size[%u]", p_cfg->image.buf_size);
        return CM_CAM_ERR_WRONG_CFG_DATA;
    }

    if ((p_cfg->recordFormat != CAM_OUT_FMT_RGB565) && (p_cfg->recordFormat != CAM_OUT_FMT_JPEG))
    {
        cam_log_err("fmt[%d]", p_cfg->recordFormat);
        return CM_CAM_ERR_WRONG_CFG_DATA;
    }

    cm_cam_sensor_resolution sensor_resolution = {0};
    ret = cm_cameara_sensor_data_query(CM_CAMERA_SENSOR_DATA_TYPE_RESOLUTION, &sensor_resolution);
    if (ret != CM_CAM_ERR_OK)
    {
        cam_log_err("ret[%d]", ret);
        return CM_CAM_ERR_INVALID_PARA;
    }

    uint32_t capture_img_size = sensor_resolution.height * sensor_resolution.width * 2;
    cam_log("capture resolution[%ux%u]", sensor_resolution.height, sensor_resolution.width);
    if (!g_camera_mgr.preview_running)
    {
        cam_log_err("cm_camera_preview_start ret=%d", ret);
        cm_camera_preview_cfg_t preview_cfg = {0};
        preview_cfg.recordFormat = p_cfg->recordFormat;
        ret = cm_camera_preview_start(&preview_cfg);
        if (ret != CM_CAM_ERR_OK)
        {
            cam_log_err("cm_camera_preview_start ret=%d", ret);
            return ret;
        }
    }

    g_p_camera_capture_shot_img = NULL;
    g_camera_capture_shot_flag  = 1;
    ret = osWaitForCompletion(&g_sem_camera_shot_done, CM_CAMERA_OP_TIMEOUT_SECOND);
    if (ret != osOK)
    {
        cam_log_err("shot fail[%d]", ret);
    }

    if (g_p_camera_capture_shot_img != NULL)
    {
        cm_mem_dump((const char *)"CAMERA_TAKE", (const char *)g_p_camera_capture_shot_img, 64);
    }

    uint32_t output_image_size = capture_img_size;
    uint8_t *p_output_image    = g_p_camera_capture_shot_img;
    uint8_t *p_image           = NULL;
    uint8_t *p_compressed_jpeg = NULL;
    do
    {
        if (p_output_image == NULL)
        {
            cam_log_err("take shot sfail");
            ret = CM_CAM_ERR_CAPTURE_TAKE_FAIL;
            break;
        }
        if (p_cfg->recordFormat == CAM_OUT_FMT_JPEG)
        {
            uint32_t compressed_jpeg_size  = 0;
#ifdef USE_LIBJPEG_TURBO
#ifdef CM_CAMERA_CAPTURE_TAKE_JPEG_FILE_SIZE_MAX
            p_compressed_jpeg = osMallocAlign(CM_CAMERA_CAPTURE_TAKE_JPEG_FILE_SIZE_MAX, OS_CACHE_LINE_SZ);
            if (p_compressed_jpeg == NULL)
            {
                cam_log_err("p_compressed_jpeg NULL");
                ret = CM_CAM_ERR_NOT_MEMORY;
                break;
            }
            cam_log_debug("p_compressed_jpeg[0x%X]", p_compressed_jpeg);
            compressed_jpeg_size  = CM_CAMERA_CAPTURE_TAKE_JPEG_FILE_SIZE_MAX;
#endif
            ret = JPEG_Compress(p_output_image, sensor_resolution.height, sensor_resolution.width, &p_compressed_jpeg, &compressed_jpeg_size);
            if (ret != 0)
            {
                cam_log_err("JPEG_Compress fail[%d]", ret);
                ret = CM_CAM_ERR_JPEG_OP_FAIL;
                break;
            }
            cam_log_debug("p_compressed_jpeg[0x%X][%u]", p_compressed_jpeg, compressed_jpeg_size);

            output_image_size = compressed_jpeg_size;
            p_output_image    = p_compressed_jpeg;
#else
            cam_log_err("JPEG_Compress fail[%d]", ret);
            ret = CM_CAM_ERR_JPEG_OP_FAIL;
            break;
#endif
        }

        cam_log_debug("cfg:format[%u]image.buf_size[%u]output_image:[%u][0x%X][0x%X]", p_cfg->recordFormat, p_cfg->image.buf_size,
                      output_image_size, p_output_image, p_compressed_jpeg);
        if (p_cfg->image.buf_size != 0)
        {
            if (p_cfg->recordFormat != CAM_OUT_FMT_JPEG)
            {
                p_image = osMallocAlign(capture_img_size, OS_CACHE_LINE_SZ);
                if (p_image == NULL)
                {
                    cam_log_err("p_image NULL");
                    ret = CM_CAM_ERR_NOT_MEMORY;
                    break;
                }
                cam_log_debug("p_image[0x%X]", p_image);
                output_image_size = capture_img_size;
                p_output_image    = p_image;

                memcpy(p_output_image, g_p_camera_capture_shot_img, capture_img_size);
            }

            p_cfg->image.buf_size  = output_image_size;
            p_cfg->image.p_buf     = p_output_image;
            ret = CM_CAM_ERR_OK;
        }
        else
        {
            cam_log_debug("save to file");
            cm_fs_t fd = cm_fs_open((const char *)p_cfg->file_name, CM_FS_WB);
            if (fd == NULL)
            {
                cam_log_err("fd NULL");
                ret = CM_CAM_ERR_FILE_OP_FAIL;
                break;
            }
            ret = cm_fs_write(fd, p_output_image, output_image_size);
            if (ret < 0)
            {
               cam_log_err("write[%d]", ret);
               ret = CM_CAM_ERR_FILE_OP_FAIL;
               cam_log("close[%d]", cm_fs_close(fd));
               break;
            }
            ret = cm_fs_close(fd);
            if (ret < 0)
            {
                cam_log_err("close[%d]", ret);
                ret = CM_CAM_ERR_FILE_OP_FAIL;
            }
        }
    }while(0);

    osComplete(&g_sem_camera_save_done);

    cm_camera_preview_stop();

    if (ret == CM_CAM_ERR_FILE_OP_FAIL)
    {
        ret = cm_fs_delete((const char *)p_cfg->file_name);
        cam_log_debug("delete[%d]", ret);
        ret = CM_CAM_ERR_FILE_OP_FAIL;
    }

    if ((p_cfg->image.buf_size == 0) || (ret != CM_CAM_ERR_OK))
    {
        cam_log_err("ret=%d", ret);
        if (p_compressed_jpeg != NULL)
        {
            osFree(p_compressed_jpeg);
            p_compressed_jpeg = NULL;
        }
        if (p_image != NULL)
        {
            osFree(p_image);
            p_image = NULL;
        }
    }

    cam_log_debug("camera shot finish[%d]", ret);

    return ret;
}

#undef EXTERN
#ifdef __cplusplus
}
#endif

/** @}*/

