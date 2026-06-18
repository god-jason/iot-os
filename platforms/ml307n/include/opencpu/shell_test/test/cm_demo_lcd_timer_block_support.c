#include "cm_os.h"
#include "cm_sys.h"
#include "cm_gpio.h"
#include "cm_spi_lcd.h"
#include "cm_iomux.h"
#include "stdlib.h"
#include "nr_micro_shell.h"
#include "cm_common_api.h"
#ifdef CM_DEMO_TIMER_BLOCK_SUPPORT

#define CM_DEMO_TIMER_BLOCK_LOG(fmt, ...) cm_log_printf(0, fmt, ##__VA_ARGS__)

#ifdef OS_USING_SHELL
#define cm_demo_printf(fmt, ...) shell_printf(fmt, ##__VA_ARGS__)
#else
#define cm_demo_printf(fmt, ...) CM_DEMO_TIMER_BLOCK_LOG(fmt, ##__VA_ARGS__)
#endif

/* 支持的APP定时器数量 */
#define APP_TIMER_NUN       (20)

#define BLACK   ((0x00 << 11) | (0x00 << 5) | 0x00)
#define WHITE   ((0x1F << 11) | (0x3F << 5) | 0x1F)

#define X_START   (161 - 32 + 1)
#define X_END     (161)
#define Y_START   (2)
#define Y_END     (2 + 16 - 1)

/* 网络信号强度阈值 */
#define RSSI_LEVEL_1_THRESHOLD   (9)
#define RSSI_LEVEL_2_THRESHOLD   (15)
#define RSSI_LEVEL_3_THRESHOLD   (20)
#define RSSI_LEVEL_4_THRESHOLD   (25)

/* 定时器相关 */
#define NET_TIMER_INTERVAL       (5000)
#define TIMER_EVENT_QUEUE_SIZE   (10)
#define TIMER_EVENT_THREAD_STACK  (1024 * 4)


#define SPILCD_TE_PIN               CM_IOMUX_PIN_39                          // SWD0_SWDIO
#define SPILCD_BACKLIGHT_PIN        CM_IOMUX_PIN_54                          // PD_GPIO_2
#define SPILCD_RESET_PIN            CM_IOMUX_PIN_38                          // SWD0_SWCLK
// #define SPILCD_DCX_PIN              CM_IOMUX_PIN_28                          // PD_GPIO_6
#define SPILCD_CSX_PIN              CM_IOMUX_PIN_87                          // PD_GPIO_12
#define SPILCD_SCL_PIN              CM_IOMUX_PIN_76                          // PD_GPIO_13
#define SPILCD_SDI_PIN              CM_IOMUX_PIN_86                          // PD_GPIO_14
#define SPILCD_SDO_PIN              CM_IOMUX_PIN_77                          // PD_GPIO_15


typedef void (*app_callback_on_timer)(void *param);
typedef void * app_timer;


osThreadId_t g_timer_app_task_handle = NULL;



typedef struct{
    osTimerId_t timer;                  /**< 定时器句柄 */
    uint32_t interval;                  /**< 首次定时时间，与app_timer_start()接口中interval对应 */
    uint32_t resched_interval;          /**< 循环定时时间，与app_timer_start()接口中resched_interval对应 */
    void *param;                        /**< 回调函数参数，与app_timer_start()接口中param对应 */
    app_callback_on_timer cb;           /**< 回调函数 */
    uint32_t index;                     /**< 定时器序列号 */
}app_timer_ctx_t;

typedef struct{
    int argument;
} timer_event_msg_t;

static app_timer_ctx_t g_app_timer[APP_TIMER_NUN] = {0};

static osMessageQueueId_t g_timer_event_queue = NULL;
static osThreadId_t g_timer_event_thread = NULL;

/* 网络信号显示图标，无网络 */
static uint16_t g_app_net_no_signal[32 * 16] = {BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, \
                                              BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, \
                                              BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

/* 网络信号显示图标，等级1 */
static uint16_t g_app_net_signal_l1[32 * 16] = {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

/* 网络信号显示图标，等级2 */
static uint16_t g_app_net_signal_l2[32 * 16] = {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

/* 网络信号显示图标，等级3 */
static uint16_t g_app_net_signal_l3[32 * 16] = {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

/* 网络信号显示图标，等级4 */
static uint16_t g_app_net_signal_l4[32 * 16] = {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

/* 网络信号显示图标，等级5 */
static uint16_t g_app_net_signal_l5[32 * 16] = {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, \
                                              BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};


static void __app_timer_callback(void *argument)
{
    int32_t index = (int)argument;

    //发事件至timer_event_task线程，由线程调用回调
    timer_event_msg_t msg = {0};

    msg.argument = index;

    if(g_timer_event_queue != NULL)//向队列发送数据
    {
        osMessageQueuePut(g_timer_event_queue, &msg, 0, 0);
    }
}

static void app_timer_event_task(void *arg)
{
    timer_event_msg_t msg = {0};

    while(1)
    {
        if(osMessageQueueGet(g_timer_event_queue, &msg, NULL, osWaitForever) == osOK)
        {
            cm_demo_printf("timer event msg argument = %d\r\n", msg.argument);
            int index = msg.argument;

            //检查index是否在有效范围内
            if(index >= 0 && index < APP_TIMER_NUN)
            {
                //回调执行
                if (g_app_timer[index].cb)
                {
                    g_app_timer[index].cb(g_app_timer[index].param);
                }

                if (0 != g_app_timer[index].resched_interval)
                {
                    if (osOK == osTimerStart(g_app_timer[index].timer, g_app_timer[index].resched_interval / 5))
                    {
                        cm_demo_printf("ant_timer_start() app_timer[%d].timer %x\n", index, g_app_timer[index].timer);
                    }
                }
                else
                {
                    //回调执行后清空a_timer[index]资源
                    g_app_timer[index].timer = NULL;
                    g_app_timer[index].cb = NULL;
                    g_app_timer[index].param = NULL;
                    g_app_timer[index].interval = 0;
                    g_app_timer[index].resched_interval = 0;
                    g_app_timer[index].index = 0;
                }
            }
            else
            {
                cm_demo_printf("app_timer_event_task() invalid index %d\r\n", index);
            }
        }
    }
}

static int32_t timer_event_task_create(void)
{
    if(g_timer_event_queue == NULL)
    {
        g_timer_event_queue = osMessageQueueNew(TIMER_EVENT_QUEUE_SIZE, sizeof(timer_event_msg_t), NULL);
        if(g_timer_event_queue == NULL)
        {
            cm_demo_printf("timer_event_queue create failed\r\n");
            return -1;
        }
    }

    if(g_timer_event_thread == NULL)
    {
        osThreadAttr_t attr1 = {
            .name = "timer_event",
            .priority = osPriorityNormal - 2,
            .stack_size = TIMER_EVENT_THREAD_STACK,
        };
        g_timer_event_thread = osThreadNew(app_timer_event_task, NULL, (const osThreadAttr_t*)&attr1);
        if(g_timer_event_thread == NULL)
        {
            osMessageQueueDelete(g_timer_event_queue);
            cm_demo_printf("timer_event_thread create failed\r\n");
            return -1;
        }
    }

    return 0;
}

/**
 * 创建定时器
 * @param  name  定时器名称
 *
 * @return 成功: 返回定时器对象
 *         失败: 返回NULL
 */
static app_timer app_timer_create(const char *name)
{
    int32_t index = 0;
    int32_t ret = 0;

    ret = timer_event_task_create();
    if(ret != 0)
    {
        cm_demo_printf("timer_event_task_create failed\r\n");
        return NULL;
    }

    for (index = 0; index < APP_TIMER_NUN; index++)
    {
        if (NULL == g_app_timer[index].timer)
        {
            g_app_timer[index].index = index;

            osTimerAttr_t attr = {0};
            if(name != NULL)
            {
                attr.name = name;
            }

            osTimerId_t timer = osTimerNew(__app_timer_callback, osTimerOnce, (void *)g_app_timer[index].index, &attr);

            if (NULL == timer)
            {
                return NULL;
            }
            else
            {
                g_app_timer[index].timer = timer;
                cm_demo_printf("app_timer_create() app_timer[index].timer %x\r\n", g_app_timer[index].timer);
                return g_app_timer[index].timer;
            }
        }
    }

    cm_demo_printf("app_timer_create() error\r\n");
    return NULL;
}

/**
 * 启动定时器
 *
 * @param timer 定时器对象
 * @param callback 时间到达时的回调函数
 * @param param 回调函数参数
 * @param interval 定时时间
 * @param resched_interval 定时事件发生时，自动重新开启定时的时间，如果传入0，则定时器自动停止
 *
 * @return 0   成功
 *         <0  失败
 */
static int32_t app_timer_start(app_timer timer, app_callback_on_timer callback, void *param, int32_t interval, int32_t resched_interval)
{
    int32_t index = 0;
    int32_t found = 0;

    if(timer == NULL || callback == NULL)
    {
        cm_demo_printf("app_timer_start() invalid parameters\r\n");
        return -1;
    }

    for (index = 0; index < APP_TIMER_NUN; index++)
    {
        if (g_app_timer[index].timer == timer)
        {
            g_app_timer[index].cb = callback;
            g_app_timer[index].param = param;
            g_app_timer[index].interval = interval;
            g_app_timer[index].resched_interval = resched_interval;
            cm_demo_printf("app_timer_start() interval %d resched_interval %d\r\n", interval, resched_interval);
            found = 1;
            break;
        }
    }

    if(!found)
    {
        cm_demo_printf("app_timer_start() timer not found\r\n");
        return -1;
    }

    if (osOK == osTimerStart(timer, interval / 5))
    {
        cm_demo_printf("app_timer_start() app_timer[index].timer %p\r\n", timer);
        return 0;
    }
    else
    {
        cm_demo_printf("app_timer_start() error app_timer[index].timer %p\r\n", timer);
        return -1;
    }
}

/**
 * 停止定时器对象
 *
 * @param timer 定时器对象
 *
 * @return 0   成功
 *         <0  失败
 */        
static int32_t app_timer_stop(app_timer timer)
{
    if(timer == NULL)
    {
        cm_demo_printf("app_timer_stop() invalid timer\r\n");
        return -1;
    }
    
    if (osOK == osTimerStop(timer))
    {
        cm_demo_printf("app_timer_stop() a_timer[index].timer %p\r\n", timer);
        return 0;
    }
    else
    {
        cm_demo_printf("app_timer_stop() error a_timer[index].timer %p\r\n", timer);
        return -1;
    }
}

/**
 * 销毁定时器
 *
 * @param timer 定时器对象
 *
 * @return 0   成功
 *         <0  失败
 */
static int32_t app_timer_destroy(app_timer timer)
{
    int32_t index = 0;

    if(timer == NULL)
    {
        cm_demo_printf("app_timer_destroy() invalid timer\r\n");
        return -1;
    }

    for (index = 0; index < APP_TIMER_NUN; index++)
    {
        if (g_app_timer[index].timer == timer)
        {
            osTimerStop(g_app_timer[index].timer);
            osTimerDelete(g_app_timer[index].timer);
            g_app_timer[index].timer = NULL;
            g_app_timer[index].cb = NULL;
            g_app_timer[index].param = NULL;
            g_app_timer[index].interval = 0;
            g_app_timer[index].resched_interval = 0;
            g_app_timer[index].index = 0;
            cm_demo_printf("app_timer_destroy()__%d__ app_timer[index].timer %p\r\n", __LINE__, timer);
            return 0;
        }
    }

    cm_demo_printf("app_timer_destroy() error__%d__ app_timer[index].timer %p\r\n", __LINE__, timer);
    return -1;	
}

static void __app_lcd_no_signal_display(void)
{
    uint8_t cmd1 = 0x2a;
    cm_spi_lcd_write_cmd(&cmd1, 1);
    uint8_t data1[] = {0x00, (uint8_t)X_START, 0x00, (uint8_t)X_END};
    cm_spi_lcd_write_data(data1, 4);
    
    uint8_t cmd2 = 0x2b;
    cm_spi_lcd_write_cmd(&cmd2, 1);
    uint8_t data2[] = {0x00, (uint8_t)Y_START, 0x00, (uint8_t)Y_END};
    cm_spi_lcd_write_data(data2, 4);

    uint8_t cmd3 = 0x2c;
    cm_spi_lcd_write_cmd(&cmd3, 1);
    cm_spi_lcd_write_data((uint8_t *)g_app_net_no_signal, 32 * 16 * 2);
}

static void __app_lcd_signal_l1_display(void)
{
    uint8_t cmd1 = 0x2a;
    cm_spi_lcd_write_cmd(&cmd1, 1);
    uint8_t data1[] = {0x00, (uint8_t)X_START, 0x00, (uint8_t)X_END};
    cm_spi_lcd_write_data(data1, 4);
    
    uint8_t cmd2 = 0x2b;
    cm_spi_lcd_write_cmd(&cmd2, 1);
    uint8_t data2[] = {0x00, (uint8_t)Y_START, 0x00, (uint8_t)Y_END};
    cm_spi_lcd_write_data(data2, 4);

    uint8_t cmd3 = 0x2c;
    cm_spi_lcd_write_cmd(&cmd3, 1);
    cm_spi_lcd_write_data((uint8_t *)g_app_net_signal_l1, 32 * 16 * 2);
}

static void __app_lcd_signal_l2_display(void)
{
    uint8_t cmd1 = 0x2a;
    cm_spi_lcd_write_cmd(&cmd1, 1);
    uint8_t data1[] = {0x00, (uint8_t)X_START, 0x00, (uint8_t)X_END};
    cm_spi_lcd_write_data(data1, 4);
    
    uint8_t cmd2 = 0x2b;
    cm_spi_lcd_write_cmd(&cmd2, 1);
    uint8_t data2[] = {0x00, (uint8_t)Y_START, 0x00, (uint8_t)Y_END};
    cm_spi_lcd_write_data(data2, 4);

    uint8_t cmd3 = 0x2c;
    cm_spi_lcd_write_cmd(&cmd3, 1);
    cm_spi_lcd_write_data((uint8_t *)g_app_net_signal_l2, 32 * 16 * 2);
}

static void __app_lcd_signal_l3_display(void)
{
    uint8_t cmd1 = 0x2a;
    cm_spi_lcd_write_cmd(&cmd1, 1);
    uint8_t data1[] = {0x00, (uint8_t)X_START, 0x00, (uint8_t)X_END};
    cm_spi_lcd_write_data(data1, 4);
    
    uint8_t cmd2 = 0x2b;
    cm_spi_lcd_write_cmd(&cmd2, 1);
    uint8_t data2[] = {0x00, (uint8_t)Y_START, 0x00, (uint8_t)Y_END};
    cm_spi_lcd_write_data(data2, 4);

    uint8_t cmd3 = 0x2c;
    cm_spi_lcd_write_cmd(&cmd3, 1);
    cm_spi_lcd_write_data((uint8_t *)g_app_net_signal_l3, 32 * 16 * 2);
}

static void __app_lcd_signal_l4_display(void)
{
    uint8_t cmd1 = 0x2a;
    cm_spi_lcd_write_cmd(&cmd1, 1);
    uint8_t data1[] = {0x00, (uint8_t)X_START, 0x00, (uint8_t)X_END};
    cm_spi_lcd_write_data(data1, 4);
    
    uint8_t cmd2 = 0x2b;
    cm_spi_lcd_write_cmd(&cmd2, 1);
    uint8_t data2[] = {0x00, (uint8_t)Y_START, 0x00, (uint8_t)Y_END};
    cm_spi_lcd_write_data(data2, 4);

    uint8_t cmd3 = 0x2c;
    cm_spi_lcd_write_cmd(&cmd3, 1);
    cm_spi_lcd_write_data((uint8_t *)g_app_net_signal_l4, 32 * 16 * 2);
}

static void __app_lcd_signal_l5_display(void)
{
    uint8_t cmd1 = 0x2a;
    cm_spi_lcd_write_cmd(&cmd1, 1);
    uint8_t data1[] = {0x00, (uint8_t)X_START, 0x00, (uint8_t)X_END};
    cm_spi_lcd_write_data(data1, 4);
    
    uint8_t cmd2 = 0x2b;
    cm_spi_lcd_write_cmd(&cmd2, 1);
    uint8_t data2[] = {0x00, (uint8_t)Y_START, 0x00, (uint8_t)Y_END};
    cm_spi_lcd_write_data(data2, 4);

    uint8_t cmd3 = 0x2c;
    cm_spi_lcd_write_cmd(&cmd3, 1);
    cm_spi_lcd_write_data((uint8_t *)g_app_net_signal_l5, 32 * 16 * 2);
}

static void cm_st7789v_pin_mux_init(void)
{
    cm_iomux_set_pin_func(SPILCD_CSX_PIN, CM_IOMUX_FUNC_FUNCTION1); //PIN_33_MUX_SPI_LCD_CSX
    cm_iomux_set_pin_func(SPILCD_SCL_PIN, CM_IOMUX_FUNC_FUNCTION1); //PIN_34_MUX_SPI_LCD_SCL
    cm_iomux_set_pin_func(SPILCD_SDO_PIN, CM_IOMUX_FUNC_FUNCTION1); //PIN_36_MUX_SPI_LCD_SDO
    cm_iomux_set_pin_func(SPILCD_SDI_PIN, CM_IOMUX_FUNC_FUNCTION1); //PIN_35_MUX_SPI_LCD_SDI
    cm_iomux_set_pin_func(SPILCD_BACKLIGHT_PIN, CM_IOMUX_FUNC_FUNCTION0); //PIN_23_MUX_GPIO
    cm_gpio_set_level(SPILCD_BACKLIGHT_PIN, 0);
    cm_gpio_set_direction(SPILCD_BACKLIGHT_PIN, CM_GPIO_DIRECTION_OUTPUT);
    cm_gpio_set_level(SPILCD_BACKLIGHT_PIN, CM_GPIO_LEVEL_HIGH);
    cm_iomux_set_pin_func(SPILCD_RESET_PIN, CM_IOMUX_FUNC_FUNCTION4); //SWD0_SWCLK
    cm_gpio_set_level(SPILCD_RESET_PIN, 0);
    cm_gpio_set_direction(SPILCD_RESET_PIN, CM_GPIO_DIRECTION_OUTPUT);
}

/**
 * 初始化LCD
 *
 */
static void app_lcd_init(void)
{
    cm_gpio_cfg_t cfg = {
        .direction = CM_GPIO_DIRECTION_OUTPUT,
        .pull = CM_GPIO_PULL_DOWN
    };

    cm_st7789v_pin_mux_init();

    /* hw reset */
    cm_gpio_set_level(SPILCD_RESET_PIN, CM_GPIO_LEVEL_HIGH);
    osDelay(20);
    cm_gpio_set_level(SPILCD_RESET_PIN, CM_GPIO_LEVEL_LOW);
    osDelay(20);
    cm_gpio_set_level(SPILCD_RESET_PIN, CM_GPIO_LEVEL_HIGH);

    /* 初始化SPI LCD */
    cm_spi_lcd_cfg_t spi_lcd_cfg = {
        .spi_dev = CM_SPI_DEV_2,
        .dcx_gpio = CM_GPIO_NUM_MAX,
        .rst_gpio = SPILCD_RESET_PIN
    };

    cm_spi_lcd_init(&spi_lcd_cfg);

    osDelay(30);
    uint8_t cmd1 = 0x1;
    cm_spi_lcd_write_cmd(&cmd1, 1);
    osDelay(30);
    uint8_t cmd2 = 0x11;
    cm_spi_lcd_write_cmd(&cmd2, 1);
    osDelay(30);
    
    uint8_t cmd3 = 0xB1;
    cm_spi_lcd_write_cmd(&cmd3, 1);
    uint8_t data3[] = {0x05, 0x3A, 0x3A};
    cm_spi_lcd_write_data(data3, 3);
    
    uint8_t cmd4 = 0xB2;
    cm_spi_lcd_write_cmd(&cmd4, 1);
    uint8_t data4[] = {0x05, 0x3A, 0x3A};
    cm_spi_lcd_write_data(data4, 3);
    
    uint8_t cmd5 = 0xB3;
    cm_spi_lcd_write_cmd(&cmd5, 1);
    uint8_t data5[] = {0x05, 0x3A, 0x3A, 0x05, 0x3A, 0x3A};
    cm_spi_lcd_write_data(data5, 6);
    
    uint8_t cmd6 = 0xB4;
    cm_spi_lcd_write_cmd(&cmd6, 1);
    uint8_t data6 = 0x03;
    cm_spi_lcd_write_data(&data6, 1);
    
    uint8_t cmd7 = 0xC0;
    cm_spi_lcd_write_cmd(&cmd7, 1);
    uint8_t data7[] = {0x62, 0x02, 0x04};
    cm_spi_lcd_write_data(data7, 3);
    
    uint8_t cmd8 = 0xC1;
    cm_spi_lcd_write_cmd(&cmd8, 1);
    uint8_t data8 = 0xC0;
    cm_spi_lcd_write_data(&data8, 1);
    
    uint8_t cmd9 = 0xC2;
    cm_spi_lcd_write_cmd(&cmd9, 1);
    uint8_t data9[] = {0x0D, 0x00};
    cm_spi_lcd_write_data(data9, 2);
    
    uint8_t cmd10 = 0xC3;
    cm_spi_lcd_write_cmd(&cmd10, 1);
    uint8_t data10[] = {0x8D, 0x6A};
    cm_spi_lcd_write_data(data10, 2);
    
    uint8_t cmd11 = 0xC4;
    cm_spi_lcd_write_cmd(&cmd11, 1);
    uint8_t data11[] = {0x8D, 0xEE};
    cm_spi_lcd_write_data(data11, 2);
    
    uint8_t cmd12 = 0xC5;
    cm_spi_lcd_write_cmd(&cmd12, 1);
    uint8_t data12 = 0x12;
    cm_spi_lcd_write_data(&data12, 1);
    
    uint8_t cmd13 = 0x36;
    cm_spi_lcd_write_cmd(&cmd13, 1);
    uint8_t data13 = 0x60;
    cm_spi_lcd_write_data(&data13, 1);
    osDelay(5);
    
    uint8_t cmd14 = 0xE0;
    cm_spi_lcd_write_cmd(&cmd14, 1);
    uint8_t data14[] = {0x03, 0x1B, 0x12, 0x11, 0x3F, 0x3A, 0x32, 0x34, 0x2F, 0x2B, 0x30, 0x3A, 0x00, 0x01, 0x02, 0x05};
    cm_spi_lcd_write_data(data14, 16);
    
    uint8_t cmd15 = 0xE1;
    cm_spi_lcd_write_cmd(&cmd15, 1);
    uint8_t data15[] = {0x03, 0x1B, 0x12, 0x11, 0x32, 0x2F, 0x2A, 0x2F, 0x2E, 0x2C, 0x35, 0x3F, 0x00, 0x00, 0x01, 0x05};
    cm_spi_lcd_write_data(data15, 16);
    
    uint8_t cmd16 = 0x3A;
    cm_spi_lcd_write_cmd(&cmd16, 1);
    uint8_t data16 = 0x05;
    cm_spi_lcd_write_data(&data16, 1);
    
    uint8_t cmd17 = 0x35;
    cm_spi_lcd_write_cmd(&cmd17, 1);
    uint8_t data17 = 0x00;
    cm_spi_lcd_write_data(&data17, 1);

    __app_lcd_no_signal_display();

    uint8_t cmd18 = 0x29;
    cm_spi_lcd_write_cmd(&cmd18, 1);
}

/**
 * LCD自检
 *
 */
static void app_lcd_self_test(void)
{
    osDelay(200);
    __app_lcd_signal_l1_display();

    osDelay(200);
    __app_lcd_signal_l2_display();

    osDelay(200);
    __app_lcd_signal_l3_display();

    osDelay(200);
    __app_lcd_signal_l4_display();

    osDelay(200);
    __app_lcd_signal_l5_display();
}

static void _net_timer_expired(void *param)
{
    /* 获取cid 1路PDP上下文激活状态 */
    int32_t ret = cm_modem_get_pdp_state(1);

    if(1 == ret)
    {
        char s_rssi[16] = {0};
        char s_ber[16] = {0};
        int32_t rssi = 0;

        /* 获取CSQ信息 */
        if (0 == cm_modem_get_csq(s_rssi, s_ber))
        {
            rssi = atoi(s_rssi);
            cm_demo_printf("[network] Activated cid 1 is activated, rssi[%s, %d] ber[%d]\r\n", s_rssi, rssi, atoi(s_ber));

            /* 根据获取的rssi，在LCD屏幕右上角显示网络信号图标 */
            if (RSSI_LEVEL_1_THRESHOLD >= rssi)
            {
                __app_lcd_signal_l1_display();
            }
            else if ((RSSI_LEVEL_1_THRESHOLD < rssi) && (RSSI_LEVEL_2_THRESHOLD >= rssi))
            {
                __app_lcd_signal_l2_display();
            }
            else if ((RSSI_LEVEL_2_THRESHOLD < rssi) && (RSSI_LEVEL_3_THRESHOLD >= rssi))
            {
                __app_lcd_signal_l3_display();
            }
            else if ((RSSI_LEVEL_3_THRESHOLD < rssi) && (RSSI_LEVEL_4_THRESHOLD >= rssi))
            {
                __app_lcd_signal_l4_display();
            }
            else
            {
                __app_lcd_signal_l5_display();
            }
        }
        else
        {
            cm_demo_printf("[network] Activated cid 1 is not activated, rssi[%s] ber[%s]\r\n", s_rssi, s_ber);

            /* 在LCD屏幕右上角显示无网络图标 */
            __app_lcd_no_signal_display();
        }
    }
    else
    {
        cm_demo_printf("[network] Activated cid 1 is not activated (ret is %d), get CSQ fail\r\n", ret);

        /* 在LCD屏幕右上角显示无网络图标 */
        __app_lcd_no_signal_display();
    }
}

/**
 * 定时器定期检查网络线程
 *
 */
void timer_demo_appimg_enter(char *param)
{
    /* 初始化LCD */
    app_lcd_init();

    /* LCD自检 */
    app_lcd_self_test();

    /* 创建定时器 */
    app_timer net_timer = app_timer_create("net_timer");

    if (NULL != net_timer)
    {
        int32_t ret = app_timer_start(net_timer, _net_timer_expired, "nettimer", NET_TIMER_INTERVAL, NET_TIMER_INTERVAL);

        if (0 > ret)
        {
            cm_demo_printf("app_timer_start() fail\r\n");
            app_timer_destroy(net_timer);
        }
    }

    /* 用户程序主业务，此处示例采用空实现 */
    while(1)
    {
        osDelay(1000);
    }

    /* 删除定时器 */
    app_timer_stop(net_timer);
    app_timer_destroy(net_timer);
}

void cm_test_timer_block(int argc, char **argv)
{
    if(argv == NULL || argc < 2)
    {
        cm_demo_printf("invalid param\r\n");
        return;
    }
    if(argv[1] == NULL)
    {
        cm_demo_printf("invalid param\r\n");
        return;
    }
    const char *cmd = argv[1];
    if (strncmp(cmd, "test", 4) == 0)
    {
        cm_demo_printf("timer block test start!!\r\n");
        cm_thread_create("timer_block_test_task", 4096 * 2, osPriorityNormal, (cm_thread_func_t)timer_demo_appimg_enter);
    }
    else
    {
        cm_demo_printf("invalid cmd=%s error\r\n", cmd);
    }
}

NR_SHELL_CMD_EXPORT(timer_block, cm_test_timer_block);

#endif
