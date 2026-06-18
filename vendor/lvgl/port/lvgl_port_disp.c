/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        lvgl_port_disp.c
 *
 * @brief       .
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team         创建
 ************************************************************************************
 */

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include "lvgl.h"
#include "drv_display.h"
#include "drv_lcd.h"
#include "lvgl_port_disp.h"
#include "board.h"
#include "os_log.h"

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/************************************************************************************
 *                                 全局变量
 ************************************************************************************/
static osCompletion g_dispCmpl = {0};

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
 
static void LVGL_Flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    DispDevice_t *pDispDev = disp_drv->user_data;
    
    pDispDev->ops->flush(pDispDev, area->x1, area->x2, area->y1, area->y2, (uint8_t*)color_p);
}

static void LVGL_FlushCb(void * data)
{
    lv_disp_drv_t * disp_drv = data;
    
    lv_disp_flush_ready(disp_drv);
    osComplete(&g_dispCmpl);
}

static void LVGL_WaitComplete(struct _lv_disp_drv_t * disp_drv)
{
    osWaitForCompletion(&g_dispCmpl, osWaitForever);
}

void LVGL_DispInit(DispDevice_t *pDispDev)
{    
    osInitCompletion(&g_dispCmpl);
    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
    static lv_disp_draw_buf_t s_drawBufDsc;
    static lv_color_t *s_drawBuf;
    s_drawBuf = (lv_color_t*)osMalloc(sizeof(lv_color_t) * pDispDev->horRes * 10); /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&s_drawBufDsc, s_drawBuf, NULL, pDispDev->horRes * 10);   /*Initialize the display buffer*/
#if 0
    /* Example for 2) */
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
    static lv_disp_draw_buf_t draw_buf_dsc_3;
    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
                          MY_DISP_VER_RES * MY_DISP_HOR_RES);   /*Initialize the display buffer*/
#endif
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t s_dispDrv;                         /*Descriptor of a display driver*/
    
    lv_disp_drv_init(&s_dispDrv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    s_dispDrv.hor_res = pDispDev->horRes;
    s_dispDrv.ver_res = pDispDev->verRes;

    /*Used to copy the buffer's content to the display*/
    s_dispDrv.flush_cb = LVGL_Flush;
    s_dispDrv.wait_cb = LVGL_WaitComplete;

    /*Set a display buffer*/
    s_dispDrv.draw_buf = &s_drawBufDsc;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1;
    
    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*-------------------------
     * Initialize your display
     * -----------------------*/
    s_dispDrv.user_data = pDispDev;
    if(pDispDev->ops->init(pDispDev, LVGL_FlushCb, &s_dispDrv))
    {
        LOG_E("Display init error!\r\n");
        return;
    }
    
    /*Finally register the driver*/
    lv_disp_drv_register(&s_dispDrv);
}

