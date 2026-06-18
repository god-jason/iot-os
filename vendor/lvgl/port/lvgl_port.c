/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        lvgl_port.c
 *
 * @brief       lvgl适配实现
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

/************************************************************************************
*                                 头文件
************************************************************************************/
#include <os.h>
#include "os_log.h"
#include "lv_conf.h"
#include "lvgl.h"
#include "lvgl_port_disp.h"
#include "lvgl_port_fs.h"
#include "lvgl_port_indev.h"
// #include "ui_common.h"
#include "lvgl_port.h"

/************************************************************************************
*                                 配置开关
************************************************************************************/
#define LVGL_THREAD_STACK_SIZE 4096

/************************************************************************************
*                                 宏定义
************************************************************************************/

/************************************************************************************
*                                 类型定义
************************************************************************************/
extern void lv_demo_keypad_encoder(void);

/************************************************************************************
*                                 全局变量定义
************************************************************************************/
static osThreadId_t g_lvglThread;
static uint8_t g_lvglThreadStack[LVGL_THREAD_STACK_SIZE];

static DispDevice_t *g_lvglDispDev;
static InputDevice_t *g_lvglInputTouch;
static InputDevice_t *g_lvglInputKey;
static InputDevice_t *g_lvglInputButton;
static osMutexId_t g_lvglMutex;

/************************************************************************************
*                                 函数定义
************************************************************************************/

#if LV_USE_LOG
static void LVGL_OutputLog(const char *buf)
{
    osPuts(buf);
}
#endif /* LV_USE_LOG */

/**
 ************************************************************************************
 * @brief           LVGL线程
 *
 * @param[in]       parameter        入参
 * @return          无.
 ************************************************************************************
 */
static void LVGL_ThreadEntry(void *parameter)
{
#if LV_USE_LOG
    lv_log_register_print_cb(LVGL_OutputLog);
#endif /* LV_USE_LOG */
    lv_init();

    LVGL_DispInit(g_lvglDispDev);
    LVGL_IndevInit(g_lvglInputTouch, g_lvglInputKey, g_lvglInputButton);

    // UI_Init();
    //lv_demo_keypad_encoder();
        
    /* handle the tasks of LVGL */
    while(1)
    {
        LVGL_Lock();
        lv_task_handler();
        lv_tick_inc(LV_DISP_DEF_REFR_PERIOD);
        LVGL_Unlock();
        
        osThreadSleepRelaxed(LV_DISP_DEF_REFR_PERIOD, osWaitForever);
    }
}

/**
 ************************************************************************************
 * @brief           锁LVGL UI刷新
 *
 * @param[in]       parameter        入参
 * @return          无.
 ************************************************************************************
 */ 
void LVGL_Lock(void)
{
    osMutexAcquire(g_lvglMutex, osWaitForever);
}

/**
 ************************************************************************************
 * @brief           释放LVGL UI刷新
 *
 * @param[in]       parameter        入参
 * @return          无.
 ************************************************************************************
 */ 
void LVGL_Unlock(void)
{
    osMutexRelease(g_lvglMutex);
}

/**
 ************************************************************************************
 * @brief           LVGL初始化
 *
 * @return          请求结果.
 * @retval          ==0              发送成功
 *                  < 0              失败
 ************************************************************************************
 */
int LVGL_PortInit(DispDevice_t *pDispDev, InputDevice_t *touch, InputDevice_t *keypad, InputDevice_t *button)
{
    osThreadAttr_t attr = {
      .name       = "LVGL",
      .attr_bits  = osThreadDetached,
      .cb_mem     = NULL,
      .cb_size    = 0U,
      .stack_mem  = g_lvglThreadStack,
      .stack_size = LVGL_THREAD_STACK_SIZE,
      .priority   = osPriorityNormal,
      .tz_module  = 0,
      .reserved   = 0
    };

    g_lvglDispDev = pDispDev;

    g_lvglInputTouch = touch;
    g_lvglInputKey = keypad;
    g_lvglInputButton = button;
    g_lvglMutex = osMutexNew(NULL);
    
    g_lvglThread = osThreadNew(LVGL_ThreadEntry, NULL, &attr);
    if(g_lvglThread == NULL)
    {
        LOG_E("Failed to create LVGL thread");
        return -1;
    }
    return 0;
}
