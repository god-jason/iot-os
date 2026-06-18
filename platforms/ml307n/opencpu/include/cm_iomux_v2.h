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
 *
 * @note 该头文件是产品定义（管脚映射关系）对应sdk/doc/ML307N-OC-V2产品定义.xlsx。
 *
 *
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
/*IOMUX PIN  definition 和模组实际PIN脚对应一致*/
typedef enum{
    CM_IOMUX_PIN_0 = 0,    /*!<PIN0 */
    CM_IOMUX_PIN_1,        /*!<PIN1 */
    CM_IOMUX_PIN_2,        /*!<PIN2 */
    CM_IOMUX_PIN_3,        /*!<PIN3 */
    CM_IOMUX_PIN_4,        /*!<PIN4 */
    CM_IOMUX_PIN_5,        /*!<PIN5 */
    CM_IOMUX_PIN_6,        /*!<PIN6 */
    CM_IOMUX_PIN_7,        /*!<PIN7 */
    CM_IOMUX_PIN_8,        /*!<PIN8 */
    CM_IOMUX_PIN_9,        /*!<PIN9 */
    CM_IOMUX_PIN_10,       /*!<PIN10 */
    CM_IOMUX_PIN_11,       /*!<PIN11 */
    CM_IOMUX_PIN_12,       /*!<PIN12 */
    CM_IOMUX_PIN_13,       /*!<PIN13 */
    CM_IOMUX_PIN_14,       /*!<PIN14 */
    CM_IOMUX_PIN_15,       /*!<PIN15 */
    CM_IOMUX_PIN_16,       /*!<PIN16 */
    CM_IOMUX_PIN_17,       /*!<PIN17 */
    CM_IOMUX_PIN_18,       /*!<PIN18 */
    CM_IOMUX_PIN_19,       /*!<PIN19 */
    CM_IOMUX_PIN_20,       /*!<PIN20 */
    CM_IOMUX_PIN_21,       /*!<PIN21 */
    CM_IOMUX_PIN_22,       /*!<PIN22 */
    CM_IOMUX_PIN_23,       /*!<PIN23 */
    CM_IOMUX_PIN_24,       /*!<PIN24 */
    CM_IOMUX_PIN_25,       /*!<PIN25 */
    CM_IOMUX_PIN_26,       /*!<PIN26 */
    CM_IOMUX_PIN_27,       /*!<PIN27 */
    CM_IOMUX_PIN_28,       /*!<PIN28 */
    CM_IOMUX_PIN_29,       /*!<PIN29 */
    CM_IOMUX_PIN_30,       /*!<PIN30 */
    CM_IOMUX_PIN_31,       /*!<PIN31 */
    CM_IOMUX_PIN_32,       /*!<PIN32 */
    CM_IOMUX_PIN_33,       /*!<PIN33 */
    CM_IOMUX_PIN_34,       /*!<PIN34 */
    CM_IOMUX_PIN_35,       /*!<PIN35 */
    CM_IOMUX_PIN_36,       /*!<PIN36 */
    CM_IOMUX_PIN_37,       /*!<PIN37 */
    CM_IOMUX_PIN_38,       /*!<PIN38 */
    CM_IOMUX_PIN_39,       /*!<PIN39 */
    CM_IOMUX_PIN_40,       /*!<PIN40 */
    CM_IOMUX_PIN_41,       /*!<PIN41 */
    CM_IOMUX_PIN_42,       /*!<PIN42 */
    CM_IOMUX_PIN_43,       /*!<PIN43 */
    CM_IOMUX_PIN_44,       /*!<PIN44 */
    CM_IOMUX_PIN_45,       /*!<PIN45 */
    CM_IOMUX_PIN_46,       /*!<PIN46 */
    CM_IOMUX_PIN_47,       /*!<PIN47 */
    CM_IOMUX_PIN_48,       /*!<PIN48 */
    CM_IOMUX_PIN_49,       /*!<PIN49 */
    CM_IOMUX_PIN_50,       /*!<PIN50 */
    CM_IOMUX_PIN_51,       /*!<PIN51 */
    CM_IOMUX_PIN_52,       /*!<PIN52 */
    CM_IOMUX_PIN_53,       /*!<PIN53 */
    CM_IOMUX_PIN_54,       /*!<PIN54 */
    CM_IOMUX_PIN_55,       /*!<PIN55 */
    CM_IOMUX_PIN_56,       /*!<PIN56 */
    CM_IOMUX_PIN_57,       /*!<PIN57 */
    CM_IOMUX_PIN_58,       /*!<PIN58 */
    CM_IOMUX_PIN_59,       /*!<PIN59 */
    CM_IOMUX_PIN_60,       /*!<PIN60 */
    CM_IOMUX_PIN_61,       /*!<PIN61 */
    CM_IOMUX_PIN_62,       /*!<PIN62 */
    CM_IOMUX_PIN_63,       /*!<PIN63 */
    CM_IOMUX_PIN_64,       /*!<PIN64 */
    CM_IOMUX_PIN_65,       /*!<PIN65 */
    CM_IOMUX_PIN_66,       /*!<PIN66 */
    CM_IOMUX_PIN_67,       /*!<PIN67 */
    CM_IOMUX_PIN_68,       /*!<PIN68 */
    CM_IOMUX_PIN_69,       /*!<PIN69 */
    CM_IOMUX_PIN_70,       /*!<PIN70 */
    CM_IOMUX_PIN_71,       /*!<PIN71 */
    CM_IOMUX_PIN_72,       /*!<PIN72 */
    CM_IOMUX_PIN_73,       /*!<PIN73 */
    CM_IOMUX_PIN_74,       /*!<PIN74 */
    CM_IOMUX_PIN_75,       /*!<PIN75 */
    CM_IOMUX_PIN_76,       /*!<PIN76 */
    CM_IOMUX_PIN_77,       /*!<PIN77 */
    CM_IOMUX_PIN_78,       /*!<PIN78 */
    CM_IOMUX_PIN_79,       /*!<PIN79 */
    CM_IOMUX_PIN_80,       /*!<PIN80 */
    CM_IOMUX_PIN_81,       /*!<PIN81 */
    CM_IOMUX_PIN_82,       /*!<PIN82 */
    CM_IOMUX_PIN_83,       /*!<PIN83 */
    CM_IOMUX_PIN_84,       /*!<PIN84 */
    CM_IOMUX_PIN_85,       /*!<PIN85 */
    CM_IOMUX_PIN_86,       /*!<PIN86 */
    CM_IOMUX_PIN_87,       /*!<PIN87 */
    CM_IOMUX_PIN_88,       /*!<PIN88 */
    CM_IOMUX_PIN_89,       /*!<PIN89 */
    CM_IOMUX_PIN_90,       /*!<PIN90 */
    CM_IOMUX_PIN_91,       /*!<PIN91 */
    CM_IOMUX_PIN_92,       /*!<PIN92 */
    CM_IOMUX_PIN_93,       /*!<PIN93 */
    CM_IOMUX_PIN_94,       /*!<PIN94 */
    CM_IOMUX_PIN_95,       /*!<PIN95 */
    CM_IOMUX_PIN_96,       /*!<PIN96 */
    CM_IOMUX_PIN_97,       /*!<PIN97 */
    CM_IOMUX_PIN_98,       /*!<PIN98 */
    CM_IOMUX_PIN_99,       /*!<PIN99 */
    CM_IOMUX_PIN_100,      /*!<PIN100 */
    CM_IOMUX_PIN_101,      /*!<PIN101 */
    CM_IOMUX_PIN_102,      /*!<PIN102 */
    CM_IOMUX_PIN_103,      /*!<PIN103 */
    CM_IOMUX_PIN_104,      /*!<PIN104 */
    CM_IOMUX_PIN_105,      /*!<PIN105 */
    CM_IOMUX_PIN_106,      /*!<PIN106 */
    CM_IOMUX_PIN_107,      /*!<PIN107 */
    CM_IOMUX_PIN_108,      /*!<PIN108 */
    CM_IOMUX_PIN_109,      /*!<PIN109 */
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
    CM_IOMUX_PINCMD0_SLEEP ,          /*!<不支持*/
    CM_IOMUX_PINCMD1_LPMEDEG,         /*!<不支持*/
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

#define pin_res pin_res_v2
extern const struct PIN_Res* pin_res[CM_IOMUX_PIN_MAX];

#define cm_iomux_set_pin_func cm_iomux_set_pin_func_v2
#define cm_iomux_get_pin_func cm_iomux_get_pin_func_v2
#define cm_iomux_set_pin_cmd cm_iomux_set_pin_cmd_v2

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
int32_t cm_iomux_set_pin_func_v2(cm_iomux_pin_e pin, cm_iomux_func_e fun);

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
int32_t cm_iomux_get_pin_func_v2(cm_iomux_pin_e pin, cm_iomux_func_e *fun);

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
int32_t cm_iomux_set_pin_cmd_v2(cm_iomux_pin_e pin, cm_iomux_pincmd_e cmd, uint8_t cmd_arg);


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_IOMUX_H__ */

/** @}*/
