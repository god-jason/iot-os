/**  @file
  ycom_gpio.h

  @brief
  TODO

*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/

#ifndef _YOPEN_GPIO_H_
#define _YOPEN_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "yopen_osi_def.h"
#include "yopen_type.h"
#include "yopen_pin.h"
/*========================================================================
 *	Macro
 *========================================================================*/
#ifdef GPIO_0
#undef GPIO_0
#endif

#ifdef GPIO_1
#undef GPIO_1
#endif

#ifdef GPIO_2
#undef GPIO_2
#endif

#define YOPEN_GPIO_ERRCODE_BASE    (YOPEN_COMPONENT_BSP_GPIO<<16)
#define YOPEN_GPIO_PORT(x)   (x >> 4)
#define YOPEN_GPIO_NUM(x)    (x & 0x0F)

/*========================================================================
 *	Enum
 *========================================================================*/
typedef enum
{
    GPIO_0 = 0,
    GPIO_1,
    GPIO_2,
    GPIO_3,
    GPIO_4,
    GPIO_5,
    GPIO_6,
    GPIO_7,
    GPIO_8,
    GPIO_9,
    GPIO_10,
    GPIO_11,
    GPIO_12,
    GPIO_13,
    GPIO_14,
    GPIO_15,
    GPIO_16,
    GPIO_17,
    GPIO_18,
    GPIO_19,
    GPIO_20,
#ifdef TYPE_EC718M
    GPIO_21,
    GPIO_22,
    GPIO_23,
    GPIO_24,
    GPIO_25,
    GPIO_26,
    GPIO_27,
    GPIO_28,
    GPIO_29,
    GPIO_30,
    GPIO_31,
    GPIO_32,
    GPIO_33,
    GPIO_34,
    GPIO_35,
    GPIO_36,
    GPIO_37,
    GPIO_38,
	GPI_0, 
    GPI_1, 
    GPI_2,
    GPI_3, 
    GPI_4, 
    GPI_5, 
#else
    //GPI 功能不需要设置yopen_pin_set_func
    GPI_0, //wakeup0
    GPI_1, //wakeup1
    GPI_2,
    GPI_3, //AGPIOWU0
    GPI_4, //AGPIOWU1
    GPI_5, //AGPIOWU2
#endif
    GPIO_MAX
}yopen_GpioNum;

typedef enum
{
    GPIO_INPUT,
    GPIO_OUTPUT
}yopen_GpioDir;
    
typedef enum
{
    Vol_1_80V,
    Vol_3_30V
}yopen_Lvl;

typedef enum
{
    LVL_LOW,
    LVL_HIGH
}yopen_LvlMode;

typedef enum
{
    /** not force pull up/down, use function default */
    PULL_DEFAULT = 0,
    /** force pull, with neither pull up nor pull down */
    FORCE_PULL_NONE,
    /** force pull down */
    FORCE_PULL_DOWN,
    /** force pull up */
    FORCE_PULL_UP,
    /** force pull up with weakest strength (largest resistance) */
    FORCE_PULL_UP_1 = FORCE_PULL_UP,
    /** force pull up with smaller resistance */
    FORCE_PULL_UP_2,
    /** force pull up with smaller resistance */
    FORCE_PULL_UP_3
}yopen_PullMode;

typedef enum
{
    EDGE_TRIGGER,
    LEVEL_TRIGGER
}yopen_TriggerMode;

typedef enum
{
    DEBOUNCE_DIS,
    DEBOUNCE_EN = 10, 
}yopen_DebounceMode;

typedef enum
{
    EDGE_RISING,
    EDGE_FALLING,
    EDGE_BOTH
}yopen_EdgeMode;

typedef enum
{
	YOPEN_WAKEUP_PAD_0 = 0,
	YOPEN_WAKEUP_PAD_1,
	YOPEN_WAKEUP_PAD_2,
	YOPEN_WAKEUP_PAD_3,
	YOPEN_WAKEUP_PAD_4,
	YOPEN_WAKEUP_PAD_5,
	YOPEN_WAKEUP_PAD_MAX = 10
}YOPEN_APmuWakeupPad_e;

typedef enum
{
    YOPEN_WAKEUP_FUNC = 0,
    YOPEN_GPIO_FUNC,
    YOPEN_WAKEUP_FUNC_MAX 
}YOPEN_Wakeup_func_e;

typedef struct
{
	bool yopen_posEdgeEn;
	bool yopen_negEdgeEn;
	bool yopen_pullUpEn;
	bool yopen_pullDownEn;
}YOPEN_APmuWakeupPadSettings_e;

typedef enum
{
    YOPEN_GPIO_SUCCESS = YOPEN_SUCCESS,
    YOPEN_GPIO_EXECUTE_ERR = 1 |YOPEN_GPIO_ERRCODE_BASE,
    YOPEN_GPIO_INVALID_PARAM_ERR,
    YOPEN_GPIO_OPEN_ERR,
    YOPEN_GPIO_CONFIG_ERR,
    YOPEN_GPIO_PULL_SET_ERR,
    YOPEN_GPIO_CALLBACK_ERR,
    YOPEN_GPIO_LEVEL_TRIGGER_ERR
}yopen_errcode_gpio;

typedef struct 
{
	uint16_t pin;
	uint16_t paddr;
} yopen_pin_paddr_map;

typedef enum
{
	YOPEN_LDO_VDD_EXT,  //无法关闭，可以配置电压域和睡眠是否保持
	YOPEN_LDO_MAX
} yopen_ldo_id_e;

typedef enum
{
	YOPEN_LDO_VOL_0MV, //关闭
	YOPEN_LDO_VOL_1800MV,
    YOPEN_LDO_VOL_3300MV,
	YOPEN_LDO_VOL_MAX
}yopen_ldo_vol_e;

/**
 * @brief 初始化GPIO
 * 
 * @param gpio_num GPIO号(不是pin号)
 * @param gpio_dir 输入输出
 * @param gpio_pull 上拉下拉
 * @param gpio_lvl 设置电平
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_gpio_init
(
    yopen_GpioNum gpio_num,
    yopen_GpioDir gpio_dir,
    yopen_PullMode gpio_pull,
    yopen_LvlMode gpio_lvl
);


/**
 * @brief GPIO去初始化
 * 
 * @param gpio_num GPIO号
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_gpio_deinit
(
    yopen_GpioNum gpio_num
);


/**
 * @brief 设置GPIO电平
 * 
 * @param gpio_num GPIO号
 * @param gpio_lvl 需要设置的电平
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_gpio_set_level
(
    yopen_GpioNum gpio_num,
    yopen_LvlMode gpio_lvl
);


/**
 * @brief 获取GPIO电平
 * 
 * @param gpio_num GPIO号
 * @param gpio_lvl 获取到的电平
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_gpio_get_level
(
    yopen_GpioNum gpio_num,
    yopen_LvlMode *gpio_lvl
);


/**
 * @brief 设置GPIO的输入输出
 * 
 * @param gpio_num GPIO号
 * @param gpio_dir 设置输入输出参数
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_gpio_set_direction
(
    yopen_GpioNum gpio_num,
    yopen_GpioDir gpio_dir
);


/**
 * @brief 获取GPIO输入输出状态
 * 
 * @param gpio_num GPIO号
 * @param gpio_dir 获取到的输入输出状态
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_gpio_get_direction
(
    yopen_GpioNum gpio_num,
    yopen_GpioDir *gpio_dir
);


/**
 * @brief 设置GPIO上下拉
 * 
 * @param gpio_num GPIO号
 * @param gpio_pull 设置上下拉参数
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_gpio_set_pull
(
    yopen_GpioNum gpio_num,
    yopen_PullMode gpio_pull
);


/**
 * @brief 初始化GPIO中断配置
 * 
 * @param gpio_num GPIO号
 * @param gpio_trigger 触发方式（边沿/电平触发）
 * @param debounce_ms 防抖时间，0不防抖 单位ms
 * @param gpio_edge 配置边沿触发方式（上升/下降沿）
 * @param gpio_pull 配置GPIO上下拉
 * @param int_cb 注册中断回调
 * @param cb_ctx 中断回调参数
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_int_register
(
    yopen_GpioNum gpio_num,
    yopen_TriggerMode gpio_trigger,
    int debounce_ms,
    yopen_EdgeMode gpio_edge,
    yopen_PullMode gpio_pull,
    void *int_cb,
    void *cb_ctx
);


/**
 * @brief 使能GPIO中断
 * 
 * @param gpio_num GPIO号
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_int_enable
(
    yopen_GpioNum gpio_num
);


/**
 * @brief 关闭GPIO中断
 * 
 * @param gpio_num GPIO号
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_int_disable
(
    yopen_GpioNum gpio_num
);


/**
 * @brief 开启agpio供电
 * 
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_aon_power_on
(
    void
);

/**
 * @brief 关闭agpio供电
 * 
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_aon_power_off
(
    void
);


/**
 * @brief 设置agpio和gpio电压域
 * 
 * @param gpio_vol 电压域参数
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_gpio_set_voltage
(
    yopen_Lvl gpio_vol
);


//注释
/**
 * @brief 设置ldo
 *
 * @param id ldo id yopen_ldo_id_e
 * @param vol ldo 电压  yopen_ldo_vol_e
 * @param sleepEnable 睡眠是否保持, true 保持, false 不保持
 * @return yopen_errcode_gpio
 */
extern yopen_errcode_gpio yopen_ldo_set(yopen_ldo_id_e id, yopen_ldo_vol_e vol, bool sleepEnable);

/**
 * @brief 设置引脚的复用功能
 * 
 * @param pin_num 引脚号
 * @param func_sel 复用功能号
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_pin_set_pull
(
    uint8_t pin_num,
    yopen_PullMode pin_pull
);

/**
 * @brief 设置引脚的复用功能
 * 
 * @param pin_num 引脚号
 * @param func_sel 复用功能号
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_pin_set_func
(
    uint8_t pin_num,
    uint8_t func_sel
);


/**
 * @brief 获取当前引脚的复用功能
 * 
 * @param pin_num 引脚号
 * @param func_sel 复用功能号
 * @return yopen_errcode_gpio 
 */
extern yopen_errcode_gpio yopen_pin_get_func
(
    uint8_t pin_num,
    uint8_t *func_sel
);


#ifdef __cplusplus
} /*"C" */
#endif

#endif  //END _YOPEN_GPIO_H_

