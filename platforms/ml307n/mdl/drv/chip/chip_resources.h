/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        chip_resources.h
 *
 * @brief       设备资源定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21   ict team        创建
 ************************************************************************************
 */

#ifndef _CHIP_RESOURCES_H
#define _CHIP_RESOURCES_H

/**
 * @addtogroup DrvCommon
 */

/**@{*/

/************************************************************************************
 *                                 头文件
 ************************************************************************************/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/**
 * 芯片控制器资源使用示例
 *
 * I2C_Regs *pReg;
 * uint16_t intNum;
 *
 * pReg = DRV_RES(I2C, 0)->regBase;
 * intNum = DRV_RES(I2C, 0)->intNum;
 *
 */

#define DRV_RES(type, num)  (&g_##type##_Res[num])
#define DRV_RES_NUM(type)  (sizeof(g_##type##_Res) / sizeof(struct type##_Res))

/**
 * 芯片管脚资源使用示例
 *
 * GPIO_Write(PIN_RES(PIN_6), GPIO_HIGH);  //设置6号PIN脚为高
 * GPIO_Read(PIN_RES(PIN_6));              //读取6号PIN脚电平
 *
 */
#define PIN_RES(pinNum)  (&g_##pinNum##_Res)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/**
 * @brief PIN配置包括mux,上下拉，去抖,slew rate定义
 */
typedef struct PIN_Config {
  uint32_t func;
  uint32_t is;
  uint32_t pe;
  uint32_t ps;
  uint32_t drvCap;
  uint32_t slewRate;
  uint32_t ie;
} *PIN_Config_t;

/**
 * @brief PIN资源定义
 */
typedef struct PIN_Res {
    uint8_t  gpioPort;
    uint8_t  gpioNum;
    struct PIN_Config pinConfig;
} *PIN_Res_t;

/**
 * @brief I2C资源定义
 */
typedef struct I2C_Res {
    void     *regBase;
    uint16_t  intNum;
    uint32_t  pclk;
    uint32_t  wclk;
    uint32_t  pclkRst;
    uint32_t  wclkRst;
} *I2C_Res_t;

/**
 * @brief SPI资源定义
 */
typedef struct SPI_Res {
    void     *regBase;
    uint16_t  intNum;
    uint32_t  txLogicID;
    uint32_t  rxLogicID;
    const struct PIN_Res *pinCs;
} *SPI_Res_t;

/**
 * @brief SSP资源定义
 */
typedef struct SSP_Res {
    void     *regBase;
    uint16_t  intNum;
    uint32_t  txLogicID;
    uint32_t  rxLogicID;
    const struct PIN_Res *pinCs;
} *SSP_Res_t;

/**
 * @brief UART资源定义
 */
typedef struct UART_Res {
    void     *regBase;
    uint16_t  intNum;
    uint16_t  ctsNum;
    uint32_t  txLogicID;
    uint32_t  rxLogicID;
    uint32_t  crmRegWCLK;
    uint32_t  crmRegCLKSEL;
    uint32_t  clk_sel;
    uint32_t  domain;
} *UART_Res_t;

/**
 * @brief GPIO资源定义
 */
typedef struct GPIO_Res {
    void     *regBase;
    uint16_t  intNum;
} *GPIO_Res_t;

/**
 * @brief I2S资源定义
 */
typedef struct I2S_Res {
    void     *regBase;
    uint16_t  intNum;
} *I2S_Res_t;

/**
 * @brief RTC资源定义
 */
typedef struct RTC_Res {
    void     *regBase;
    uint16_t  intNum;
} *RTC_Res_t;

/**
 * @brief WDT资源定义
 */
typedef struct WDT_Res {
    void     *regBase;
    uint16_t  intNum;
} *WDT_Res_t;

/**
 * @brief LCD资源定义
 */
typedef struct LCD_Res {
    void     *regBase;
    uint16_t  intNum;
    uint16_t  dmaReq;
} *LCD_Res_t;

/**
 * @brief CAM资源定义
 */
typedef struct CAM_Res {
    void     *regBase;
    uint16_t  intNum;
} *CAM_Res_t;

/**
 * @brief ADC资源定义
 */
typedef struct ADC_Res {
    void     *regBase;
    uint16_t  intNum;
} *ADC_Res_t;

/**
 * @brief KEY资源定义
 */
typedef struct KEY_Res {
    void     *regBase;
    uint16_t  intNum;
    uint32_t  pclk;
    uint32_t  wclk;
} *KEY_Res_t;

/**
 * @brief ICP资源定义
 */
typedef struct ICP_Res {
    void     *regBase;
    uint16_t  intNum;
} *ICP_Res_t;

/**
 * @brief Timer资源定义
 */
typedef struct TIM_Res {
    void     *regBase;
    uint16_t  intNum;
} *TIM_Res_t;

/**
 * @brief DMA资源定义
 */
typedef struct DMA_Res {
    void     *regBase;
    uint16_t  intNum;
} *DMA_Res_t;

/**
 * @brief DMA CHANNEL CONFIG资源定义
 */
typedef struct DMA_CHCFG_Res {
    void     *regBase;

} *DMA_CHCFG_Res_t;

/**
 * @brief XIP_CTRL资源定义
 */
typedef struct XIP_Res {
    void     *regBase;
    uint16_t  intNum;
    uint32_t  txLogicID;
    uint32_t  rxLogicID;
} *XIP_Res_t;

/**
 * @brief PIN_CTRL资源定义
 */
typedef struct PIN_CTRL_Res {
    void     *regBase;
} *PIN_CTRL_Res_t;

/**
 * @brief APM资源定义
 */
typedef struct APM_Res {
    void     *regBase;
} *APM_Res_t;

/**
 * @brief HDLC资源定义
 */
typedef struct HDLC_Res {
    void     *regBase;
} *HDLC_Res_t;

/**
 * @brief EDMA资源定义
 */
typedef struct EDMA_Res {
    void     *regBase;
    uint16_t  intNum;
    uint16_t  wrIntNum;
    uint16_t  rdIntNum;
} *EDMA_Res_t;

/**
 * @brief PWM Audio资源定义
 */
typedef struct PWMAUDIO_Res {
    void     *regBase;
    uint16_t  intNum;
} *PWMAUDIO_Res_t;

/**
 * @brief APBMON资源定义
 */
typedef struct APBMON_Res {
    void     *regBase;
    uint16_t  intNum;
} *APBMON_Res_t;

/**
 * @brief DAC 资源定义
 */
typedef struct DAC_Res {
    void     *regBase;
    uint16_t  intNum;
} *DAC_Res_t;

/**
 * @brief KTI资源定义，模拟环境的keyboard, touch.
 */
typedef struct KTI_Res {
    void     *regBase;
    uint16_t  intNum;
} *KTI_Res_t;

/**
 * @brief EFUSE资源定义
 */
typedef struct EFUSE_Res {
    void     *regBase;
    uint16_t  intNum;
} *EFUSE_Res_t;

/************************************************************************************
 *                                 各设备资源定义
 ************************************************************************************/

/******  芯片资源定义，请使用DRV_RES宏访问             ，如下内容仅为声明的设备结构体，检查链接错误使用                           *****/
//I2C
extern const struct I2C_Res g_I2C_Res[4];
//SPI
extern const struct SPI_Res g_SPI_Res[1];
//SSP
extern const struct SSP_Res g_SSP_Res[1];
//UART
extern const struct UART_Res g_UART_Res[4];
//GPIO
extern const struct GPIO_Res g_GPIO_Res[2];
//I2S
extern const struct I2S_Res g_I2S_Res[1];
//RTC
extern const struct RTC_Res g_RTC_Res[1];
//WDT
extern const struct WDT_Res g_WDT_Res[1];
//LCD
extern const struct LCD_Res g_LCD_Res[1];
//CAM
extern const struct CAM_Res g_CAM_Res[1];
//ADC
extern const struct ADC_Res g_ADC_Res[1];
//KEY
extern const struct KEY_Res g_KEY_Res[1];
//ICP
extern const struct ICP_Res g_ICP_Res[2];
//Timer
extern const struct TIM_Res g_TIM_Res[8];
//DMA
extern const struct DMA_Res g_DMA_Res[1];
//DMA CHANNEL CFG
extern const struct DMA_CHCFG_Res g_DMA_CHCFG_Res[1];
//XIP_CTRL
extern const struct XIP_Res g_XIP_Res[1];
//PIN_CTRL
extern const struct PIN_CTRL_Res g_PIN_Ctrl_Res[2];
//APM
extern const struct APM_Res g_APM_Res[ 4 ];
//HDLC
extern const struct HDLC_Res g_HDLC_Res[1];
//EDMA
extern const struct EDMA_Res g_EDMA_Res[ 1 ];
//PWM AUDIO
extern const struct PWMAUDIO_Res g_PWMAUDIO_Res[ 1 ];
//APB Monitor
extern const struct APBMON_Res g_APBMON_Res[2];
//DAC
extern const struct DAC_Res g_DAC_Res[1];
//KTI
extern const struct KTI_Res g_KTI_Res[2];
//EFUSE
extern const struct EFUSE_Res g_EFUSE_Res[1];

/******  芯片管脚资源定义，请使用PIN_RES宏访问             ，如下内容仅为声明的结构体，检查链接错误使用                           *****/
extern const struct PIN_Res g_PIN_5_Res;
extern const struct PIN_Res g_PIN_6_Res;
extern const struct PIN_Res g_PIN_7_Res;
extern const struct PIN_Res g_PIN_8_Res;
extern const struct PIN_Res g_PIN_9_Res;
extern const struct PIN_Res g_PIN_10_Res;
extern const struct PIN_Res g_PIN_11_Res;
extern const struct PIN_Res g_PIN_12_Res;
extern const struct PIN_Res g_PIN_13_Res;
extern const struct PIN_Res g_PIN_14_Res;
extern const struct PIN_Res g_PIN_21_Res;
extern const struct PIN_Res g_PIN_22_Res;
extern const struct PIN_Res g_PIN_23_Res;
extern const struct PIN_Res g_PIN_24_Res;
extern const struct PIN_Res g_PIN_25_Res;
extern const struct PIN_Res g_PIN_26_Res;
extern const struct PIN_Res g_PIN_27_Res;
extern const struct PIN_Res g_PIN_28_Res;
extern const struct PIN_Res g_PIN_29_Res;
extern const struct PIN_Res g_PIN_30_Res;
extern const struct PIN_Res g_PIN_31_Res;
extern const struct PIN_Res g_PIN_32_Res;
extern const struct PIN_Res g_PIN_33_Res;
extern const struct PIN_Res g_PIN_34_Res;
extern const struct PIN_Res g_PIN_35_Res;
extern const struct PIN_Res g_PIN_36_Res;
extern const struct PIN_Res g_PIN_37_Res;
extern const struct PIN_Res g_PIN_38_Res;
extern const struct PIN_Res g_PIN_39_Res;
extern const struct PIN_Res g_PIN_40_Res;
extern const struct PIN_Res g_PIN_41_Res;
extern const struct PIN_Res g_PIN_42_Res;
extern const struct PIN_Res g_PIN_43_Res;
extern const struct PIN_Res g_PIN_44_Res;
extern const struct PIN_Res g_PIN_45_Res;
extern const struct PIN_Res g_PIN_46_Res;
extern const struct PIN_Res g_PIN_47_Res;
extern const struct PIN_Res g_PIN_48_Res;
extern const struct PIN_Res g_PIN_49_Res;
extern const struct PIN_Res g_PIN_50_Res;
extern const struct PIN_Res g_PIN_51_Res;
extern const struct PIN_Res g_PIN_52_Res;
extern const struct PIN_Res g_PIN_53_Res;

#endif  // _CHIP_RESOURCES_H
/** @} */