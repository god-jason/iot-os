/**
 * @file        cm_uart.h
 * @brief       KEYPAD设备接口
 * @copyright   Copyright @2021 China Mobile IOT. All rights reserved.
 * @author      By cls
 * @date        2024/12/16
 *
 * @defgroup adc adc
 * @ingroup PI
 * @{
 */

#ifndef __CM_KPD_H__
#define __CM_KPD_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include "drv_kpd.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Public Types
 ****************************************************************************/
 /**
 *  @brief key map
 *
 */
typedef uint32_t cm_keypad_map_t;

/**
 *	@brief key事件类型
 *
 */
typedef enum
{
    CM_KEY_EVENT_RELEASE = 0,		  /*!<按键被释放*/
    CM_KEY_EVENT_PRESS = 1, 		  /*!<按键被按下*/
} cm_keypad_event_e;

/*具体配置参照资源综述*/
typedef enum cm_keypad_kp_mkin{
    CM_KP_MKI0 = 0,
    CM_KP_MKI1,
    CM_KP_MKI2,
    CM_KP_MKI3,
    CM_KP_MKI4,
    CM_KP_MKI_NO_VALID
}cm_keypad_kp_mkin_e;

/*具体配置参照资源综述*/
typedef enum cm_keypad_kp_mkout{
    CM_KP_MKO0 = 0,
    CM_KP_MKO1,
    CM_KP_MKO2,
    CM_KP_MKO3,
    CM_KP_MKO4,
    CM_KP_MKO_NO_VALID
}cm_keypad_kp_mkout_e;


/****************************************************************************
 * Public Data
 ****************************************************************************/
/** 配置 */
typedef struct{
    cm_keypad_kp_mkin_e *cm_col;
    cm_keypad_kp_mkout_e *cm_row;
    uint8_t cm_col_len;//*cm_col长度
    uint8_t cm_row_len;//*cm_cow长度
} cm_keypad_cfg_t;


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
 *  @brief keypad事件回调函数
 *
 *  @param [in] key         键值
 *  @param [in] event       按键事件
 *
 */
typedef void (*cm_key_event_callback_t)(cm_keypad_map_t key, cm_keypad_event_e event);

/****************************************************************************/

/**
 *  @brief keypad事件回调注册
 *
 *  @param [in] cb  keypad回调函数
 *
 *  @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 *  @details  最大允许注册的回调函数个数为10
 */
 #if 1
 int32_t cm_keypad_register(cm_key_event_callback_t cb);
 #else
int32_t cm_keypad_register(KPD_Handle *KPD, cm_key_event_callback_t cb);
#endif

/**
 *  @brief keypad休眠使能
 *
 *  @param [in] enable   1,切换为休眠唤醒中断函数;0 ,切换为唤醒状态下中断函数
 *
 *  @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 *  @details  在进入休眠时需要将按键休眠使能，将按键回调函数指向唤醒中断；
 *            休眠状态下，按按键唤醒，唤醒之后需立即调用cm_keypad_sleep(0),将按键休眠去使能，且持有休眠锁，参照demo回调函数使用方式；
 *            在休眠进入的回调函数中调用cm_keypad_sleep(1)，将按键休眠唤醒使能。
 *          keypad硬件不支持休眠唤醒，代码中已加入休眠锁，使用keypad时不进休眠
 */
int32_t cm_keypad_sleep(int enable);

/**
 *  @brief keypad事件回调注销
 *
 *  @param [in] cb   keypad回调函数
 *
 *  @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 *  @details  最大允许注册的回调函数个数为10
 */
int32_t cm_keypad_unregister(cm_key_event_callback_t cb);


/**
 *  @brief keypad配置
 *
 *  @param [in] cm_keypad_cfg  keypad 配置参数指针
 *
 *  @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 *  @details 需在cm_keypad_init函数之前使用
 */

int32_t cm_keypad_config(cm_keypad_cfg_t *cm_keypad_cfg);

/**
 *  @brief keypad初始化
 *
 *  @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 */
int32_t cm_keypad_init(void);

/**
 *  @brief keypad去初始化
 *
 *  @details  由于任务删除不能释放已经分配的内存，因此重复初始化keypad功能会造成内存泄露
 */
void cm_keypad_deinit(void);


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_KPD_H__ */



/** @}*/
