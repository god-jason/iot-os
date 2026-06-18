/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        chip_reg_base.h
 *
 * @brief       芯片基地址定义.
 *
 * @revision
 *
 * 日期           作者            修改内容
 * 2023-04-21   ict team        创建
 ************************************************************************************
 */

#ifndef _CHIP_REG_BASE
#define _CHIP_REG_BASE

/**
 * @addtogroup DrvCommon
 */

/**@{*/

/************************************************************************************
 *                                 基地址宏定义
 ************************************************************************************/
#define BASE_ILM                                  0x00008000UL
#define BASE_DLM                                  0x00010000UL
#define BASE_IREGION                              0x00400000UL
#define BASE_AP_IINF0                             0x00400000UL
#define BASE_AP_DEBUG                             0x00410000UL
#define BASE_AP_ECLIC                             0x00420000UL
#define BASE_AP_TIMER                             0x00430000UL
#define BASE_ROM_NC                               0x10000000UL
#define BASE_SH_RAM0_NC                           0x10200000UL
#define BASE_SH_RAM1_NC                           0x10240000UL
#define BASE_SH_RAM2_NC                           0x10260000UL
#define BASE_MODEM_CFG_NC                         0x20000000UL
#define BASE_PSRAM_NC                             0x40000000UL
#define BASE_PSRAM                                0x80000000UL
#define BASE_ROM                                  0xC0000000UL
#define BASE_SH_RAM0                              0xC0200000UL
#define BASE_SH_RAM1                              0xC0240000UL
#define BASE_SH_RAM2                              0xC0260000UL
#define BASE_XIP_FLASH                            0xC8000000UL
#define BASE_XIP_FLASH_END                        0xC8400000UL
#define BASE_MODEM_CFG                            0xD0000000UL
#define BASE_PRIVATE_TIMER0                       0xE0000000UL
#define BASE_PRIVATE_TIMER1                       0xE0001000UL
#define BASE_PRIVATE_WDT                          0xE0002000UL
#define BASE_PRIVATE_ICP                          0xE0003000UL
#define BASE_SBY_LSP                              0xF0000000UL
#define BASE_RTC                                  0xF0001000UL
#define BASE_LTE_LPM                              0xF0008000UL
#define BASE_LP_UART                              0xF0009000UL
#define BASE_AON_CRM                              0xF1000000UL
#define BASE_PMU_AON_CTRL                         0xF1001000UL
#define BASE_AON_IOPAD                            0xF1002000UL
#define BASE_AON_PADCTRL                          0xF1003000UL
#define BASE_AON_GPIO                             0xF1004000UL
#define BASE_PCU_AON                              0xF1005000UL
#define BASE_PWMTIMER0                            0xF1006000UL
#define BASE_PWMTIMER1                            0xF1007000UL
#define BASE_KEYPAD                               0xF1008000UL
#define BASE_CPU_MEM_CRM                          0xF2000000UL
#define BASE_DMA_APB                              0xF2001000UL
#define BASE_DMA_CHN_CFG                          0xF2002000UL
#define BASE_HDLC_APB                             0xF2003000UL
#define BASE_EDMA_APB                             0xF2004000UL
#define BASE_APM0_CFG_APB                         0xF2005000UL
#define BASE_APM1_CFG_APB                         0xF2006000UL
#define BASE_APM2_CFG_APB                         0xF2007000UL
#define BASE_APM3_CFG_APB                         0xF2008000UL
#define BASE_CPCPU_CSR                            0xF2010000UL
#define BASE_APCPU_CSR                            0xF2014000UL
#define BASE_LSP_CRM                              0xF2800000UL
#define BASE_PD_SYS_CTRL                          0xF2801000UL
#define BASE_PD_PADCTRL                           0xF2802000UL
#define BASE_PD_GPIO                              0xF2803000UL
#define BASE_I2C0                                 0xF2804000UL
#define BASE_I2C1                                 0xF2805000UL
#define BASE_TOP_CRM                              0xF2806000UL
#define BASE_ICP                                  0xF2807000UL
#define BASE_RFFE_SSC                             0xF2808000UL
#define BASE_RF_CTRL                              0xF2809000UL
#define BASE_AON_SYS_CTRL                         0xF2809800UL
#define BASE_UART0                                0xF280A000UL
#define BASE_SPI_LCD                              0xF280B000UL
#define BASE_SPI_CAM                              0xF280C000UL
#define BASE_GP_SSP                               0xF280D000UL
#define BASE_I2S                                  0xF280E000UL
#define BASE_USIM                                 0xF280F000UL
#define BASE_PSRAM_CTRL_APB                       0xF2810000UL
#define BASE_PSRAM_PHY_APB                        0xF2811000UL
#define BASE_XIP_CTRL_APB                         0xF2812000UL
#define BASE_ICT_PWM_APB                          0xF2813000UL
#define BASE_UART2                                0xF2814000UL
#define BASE_AUX_DAC_CTRL                         0xF2815000UL
#define BASE_I2C2                                 0xF2816000UL
#define BASE_I2C3                                 0xF2817000UL
#define BASE_UART3                                0xF2818000UL
#define BASE_GP_SSP1                              0xF2819000UL
#define BASE_KTI_KEYBOARD                         0xF281A000UL   //for emulator
#define BASE_KTI_TOUCH                            0xF281B000UL   //for emulator
#define BASE_EDCP_CFG_AHB                         0xF3040000UL
#define BASE_EFUSE_CFG_AHB                        0xF3050000UL
#define BASE_ETHLITE                              0xF3060000UL   //for emulator
#define BASE_AP_ILM                               0xF4000000UL
#define BASE_AP_DLM                               0xF4004000UL
#define BASE_CP_ILM                               0xF5000000UL
#define BASE_CP_DLM                               0xF5008000UL

/************************************************************************************
 *                                 载体资源大小宏定义
 ************************************************************************************/
#define LEN_ILM_AP                                (16 * 1024UL)
#define LEN_ILM_CP                                (32 * 1024UL)
#define LEN_DLM_AP                                (16 * 1024UL)
#define LEN_DLM_CP                                (32 * 1024UL)
#define LEN_SH_RAM                                (512 * 1024UL)
#define LEN_PSRAM                                 (4 * 1024 * 1024UL)
#define LEN_XIP_FLASH                             (4 * 1024 * 1024UL)

#endif  // _CHIP_REG_BASE
/** @} */