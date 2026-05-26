/**
 * @file        cm_iomux.h
 * @brief       IOMUX接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By zyf
 * @date        2021/07/30
 *
 * @defgroup iomux
 * @ingroup PI
 * @{
 */
/**********************************************************************************
 ***********************IOMUX使用注意事项*************************************
 * 1、设备上电之后需要将各个引脚功能进行IOMUX配置,以唯一功能进行调试，中途不可变更;
 ***************************************************************************/

#ifndef __CM_IOMUX_H__
#define __CM_IOMUX_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
/****************************************************************************
 * Public Types
 ****************************************************************************/
/**IOMUX PIN  definition 和模组实际PIN脚对应一致,请参照资源综述*/
typedef enum{
    CM_IOMUX_PIN_16 = 0,    //PD_GPIO_0
    CM_IOMUX_PIN_17,        //LPUART0_RX
    CM_IOMUX_PIN_18,        //LPUART0_TX
    CM_IOMUX_PIN_20,        //PD_GPIO_4
    CM_IOMUX_PIN_21,        //PD_GPIO_17
    CM_IOMUX_PIN_22,        //LPUART0_CTS
    CM_IOMUX_PIN_23,        //LPUART0_RTS
    CM_IOMUX_PIN_25,        //AON_GPIO_7
    CM_IOMUX_PIN_28,        //PD_GPIO_6
    CM_IOMUX_PIN_29,        //PD_GPIO_7
    CM_IOMUX_PIN_38,        //SWD0_SWCLK
    CM_IOMUX_PIN_39,        //SWD0_SWDIO
    CM_IOMUX_PIN_49,        //AON_GPIO_8
    CM_IOMUX_PIN_54,        //PD_GPIO_2
    CM_IOMUX_PIN_55,        //PD_GPIO_3
    CM_IOMUX_PIN_56,        //PD_GPIO_1
    CM_IOMUX_PIN_57,        //CLK_AU_MCLK
    CM_IOMUX_PIN_58,        //PD_GPIO_16
    CM_IOMUX_PIN_62,        //PD_GPIO_9
    CM_IOMUX_PIN_63,        //PD_GPIO_8
    CM_IOMUX_PIN_64,        //PD_GPIO_10
    CM_IOMUX_PIN_74,        //AON_GPIO_0
    CM_IOMUX_PIN_75,        //PD_GPIO_5
    CM_IOMUX_PIN_76,        //PD_GPIO_13
    CM_IOMUX_PIN_77,        //PD_GPIO_15
    CM_IOMUX_PIN_79,        //AON_GPIO_1
    CM_IOMUX_PIN_80,        //CLK_EXT_OUT
    CM_IOMUX_PIN_81,        //PD_GPIO_11
    CM_IOMUX_PIN_86,        //PD_GPIO_14
    CM_IOMUX_PIN_87,        //PD_GPIO_12
    CM_IOMUX_PIN_MAX
} cm_iomux_pin_e;

/*IOMUX FUNC  definition */
typedef enum{
    CM_IOMUX_FUNC_FUNCTION0 ,         /*!<功能0*/
    CM_IOMUX_FUNC_FUNCTION1,          /*!<功能1*/
    CM_IOMUX_FUNC_FUNCTION2,          /*!<功能2*/
    CM_IOMUX_FUNC_FUNCTION3,          /*!<功能3*/
    CM_IOMUX_FUNC_FUNCTION4,          /*!<功能4*/
    CM_IOMUX_FUNC_FUNCTION5,          /*!<功能5*/
    CM_IOMUX_FUNC_FUNCTION6,          /*!<功能6*/
    CM_IOMUX_FUNC_FUNCTION7,          /*!<功能7*/
    CM_IOMUX_FUNC_FUNCTIONNUM_END,
} cm_iomux_func_e;


/*IOMUX PIN  cmd*/
typedef enum{
    CM_IOMUX_PINCMD0_SLEEP ,          /*!<休眠状态*/
    CM_IOMUX_PINCMD1_LPMEDEG,         /*!<边沿检测*/
    CM_IOMUX_PINCMD2_DRIVEABILITY,    /*!<驱动能力*/
    CM_IOMUX_PINCMD3_PULL,            /*!<上下拉*/
    CM_IOMUX_PINCMDNUM_END
} cm_iomux_pincmd_e;

/*IOMUX PIN  cmdnum enumeration*/
typedef enum{
    CM_IOMUX_PINCMD0_FUNC0_SLEEP_NONE,             /*!<不使能 pad 的 sleep 功能*/
    CM_IOMUX_PINCMD0_FUNC1_SLEEP_DIR,              /*!<不使能 sleep，只是设置为 sleep 时候，输入状态*/
    CM_IOMUX_PINCMD0_FUNC2_SLEEP_DATA,             /*!<不使能 sleep，只是设置为 sleep 时候，输出状态*/
    CM_IOMUX_PINCMD0_FUNC3_SLEEP_FLOAT,            /*!<使能 sleep 功能，设置为 sleep 时候，输入状态*/
    CM_IOMUX_PINCMD0_FUNC4_SLEEP_OUTPUT_HIGH,      /*!<使能 sleep 功能，设置为 sleep 时候，输出高状态*/
    CM_IOMUX_PINCMD0_FUNC5_SLEEP_OUTPUT_LOW,       /*!<使能 sleep 功能，设置为 sleep 时候，输出低状态*/
    CM_IOMUX_PINCMD0_FUNCNUM_END
} cm_iomux_pincmd0_e;

typedef enum{
    CM_IOMUX_PINCMD1_FUNC0_LPM_EDGE_NONE,        /*!<不使能 pad 的边沿检测功能,需配合休眠解锁cm_pm_work_unlock函数使用*/
    CM_IOMUX_PINCMD1_FUNC1_LPM_EDGE_RISE ,       /*!<使能 pad 的边沿检测功能，且是上升沿。并提供唤醒事件,需配合休眠锁cm_pm_work_lock函数使用*/
    CM_IOMUX_PINCMD1_FUNC2_LPM_EDGE_FALL,        /*!<使能 pad 的边沿检测功能，且是下降沿。并提供唤醒事件.需配合休眠锁cm_pm_work_lock函数使用*/
    CM_IOMUX_PINCMD1_FUNC3_EDGE_BOTH,            /*!<使能 pad 的边沿检测功能，且是双沿。并提供唤醒事件.需配合休眠锁cm_pm_work_lock函数使用*/
    CM_IOMUX_PINCMD1_FUNCNUM_END
} cm_iomux_pincmd1_e;

typedef enum{
    CM_IOMUX_PINCMD2_FUNC0_DRIVE_VERY_SLOW,
    CM_IOMUX_PINCMD2_FUNC1_DRIVE_SLOW,
    CM_IOMUX_PINCMD2_FUNC2_DRIVE_MEDIUM,
    CM_IOMUX_PINCMD2_FUNC3_DRIVE_FAST,
    CM_IOMUX_PINCMD2_FUNC4_MASK,
    CM_IOMUX_PINCMD2_FUNCNUM_END
} cm_iomux_pincmd2_e;

typedef enum{
    CM_IOMUX_PINCMD3_FUNC0_PULL_NONE,              /*!<不使能内部的上拉下拉功能*/
    CM_IOMUX_PINCMD3_FUNC1_PULL_LOW,               /*!<使能下拉*/
    CM_IOMUX_PINCMD3_FUNC2_PULL_HIGH,              /*!<使能上拉*/
    CM_IOMUX_PINCMD3_FUNC3_PULL_BOTH,              /*!<使能上下拉*/
    CM_IOMUX_PINCMD3_FUNC4_PULL_FLOAT,             /*!<浮空功能*/
    CM_IOMUX_PINCMD3_FUNCNUM_END
} cm_iomux_pincmd3_e;

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/*IOMUX PIN FUNCTION  definition*/


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
 * @brief IOMUX 设置引脚复用功能
 *
 * @param [in] pin PIN 定义号
 * @param [in] fun FUN 定义号
 *
 *  @return
 *    = 0  - 成功 \n
 *    = -1 - 失败
 */
int32_t cm_iomux_set_pin_func(cm_iomux_pin_e pin, cm_iomux_func_e fun);

/**
 * @brief IOMUX 获取引脚功能
 *
 * @param [in] pin PIN 定义号
 * @param [out] fun FUN 定义号
 *
 * @return
 *    = 0  - 成功\n
 *    < 0  - 失败
 */
int32_t cm_iomux_get_pin_func(cm_iomux_pin_e pin, cm_iomux_func_e *fun);

/**
 * @brief IOMUX PIN脚控制
 *
 * @param [in] pin PIN 定义号
 * @param [in] cmd 功能定义
 * @param [in] cmd_arg 具体功能值
 *
 * @return
 *    = 0  - 成功\n
 *    < 0  - 失败
 */
int32_t cm_iomux_set_pin_cmd(cm_iomux_pin_e pin, cm_iomux_pincmd_e cmd, uint8_t cmd_arg);


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_IOMUX_H__ */

/** @}*/
