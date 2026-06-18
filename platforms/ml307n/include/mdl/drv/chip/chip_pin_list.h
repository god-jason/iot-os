/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        chip_pins_list.h
 *
 * @brief       芯片PIN脚定义.
 *
 * @revision
 *
 * 日期           作者            修改内容
 * 2023-04-21   ict team        创建
 ************************************************************************************
 */


#ifndef _CHIP_PIN_LIST
#define _CHIP_PIN_LIST

/**
 * @addtogroup DrvCommon
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/**@name GPIO端口定义
 * @{
 */
#define GPIO_PORT_AON                             0
#define GPIO_PORT_PD                              1
/** @}*/


// Pin name: LPUART0_RX
#define PIN_5                                     5
// MUX
#define PIN_5_MUX_LPUART0_RX                      0
#define PIN_5_MUX_GPIO                            1
#define PIN_5_MUX_AON_INT_2                       2
// #define PIN_5_MUX_LPUART0_RX                      3
#define PIN_5_MUX_UART1_CTS                       4
#define PIN_5_MUX_SBY_DEBUG_TEST_PIN0             5
#define PIN_5_MUX_CLK_EXT_OUT                     6
#define PIN_5_MUX_KBR_0                           7
// GPIO
#define PIN_5_GPIO_PORT                           GPIO_PORT_AON
#define PIN_5_GPIO_NUM                            2

// Pin name: LPUART0_TX
#define PIN_6                                     6
// MUX
#define PIN_6_MUX_LPUART0_TX                      0
#define PIN_6_MUX_GPIO                            1
#define PIN_6_MUX_AON_INT_3                       2
// #define PIN_6_MUX_LPUART0_TX                      3
#define PIN_6_MUX_UART1_RTS                       4
#define PIN_6_MUX_SBY_DEBUG_TEST_PIN1             5
#define PIN_6_MUX_CLK_EXT_REQ                     6
#define PIN_6_MUX_KBR_1                           7
// GPIO
#define PIN_6_GPIO_PORT                           GPIO_PORT_AON
#define PIN_6_GPIO_NUM                            3

// Pin name: LPUART0_CTS
#define PIN_7                                     7
// MUX
#define PIN_7_MUX_GPIO                            0
#define PIN_7_MUX_CLK_32K_EXTIN                   1
#define PIN_7_MUX_AON_INT_4                       2
#define PIN_7_MUX_LPUART0_CTS                     3
#define PIN_7_MUX_UART1_TX                        4
#define PIN_7_MUX_SBY_DEBUG_TEST_PIN2             5
#define PIN_7_MUX_AON_PWM_0                       6
#define PIN_7_MUX_KBR_2                           7
// GPIO
#define PIN_7_GPIO_PORT                           GPIO_PORT_AON
#define PIN_7_GPIO_NUM                            4

// Pin name: LPUART0_RTS
#define PIN_8                                     8
// MUX
#define PIN_8_MUX_CLK_32K_EXTOUT                  0
#define PIN_8_MUX_GPIO                            1
#define PIN_8_MUX_AON_INT_5                       2
#define PIN_8_MUX_LPUART0_RTS                     3
#define PIN_8_MUX_UART1_RX                        4
#define PIN_8_MUX_SBY_DEBUG_TEST_PIN3             5
#define PIN_8_MUX_AON_PWM_1                       6
#define PIN_8_MUX_KBR_3                           7
// GPIO
#define PIN_8_GPIO_PORT                           GPIO_PORT_AON
#define PIN_8_GPIO_NUM                            5

// Pin name: AON_GPIO_0
#define PIN_9                                     9
// MUX
#define PIN_9_MUX_EN_BK1_N                        0
#define PIN_9_MUX_GPIO                            1
#define PIN_9_MUX_AON_INT_0                       2
#define PIN_9_MUX_UART1_TX                        3
#define PIN_9_MUX_KBR_0                           4
#define PIN_9_MUX_AON_DEBUG_TEST_PIN0             5
#define PIN_9_MUX_AON_PWM_0                       6
#define PIN_9_MUX_CLK_EXT_OUT                     7
// GPIO
#define PIN_9_GPIO_PORT                           GPIO_PORT_AON
#define PIN_9_GPIO_NUM                            0

// Pin name: AON_GPIO_1
#define PIN_10                                    10
// MUX
#define PIN_10_MUX_FPWM_BK1                       0
#define PIN_10_MUX_GPIO                           1
#define PIN_10_MUX_AON_INT_1                      2
#define PIN_10_MUX_UART1_RX                       3
#define PIN_10_MUX_KBR_1                          4
#define PIN_10_MUX_AON_DEBUG_TEST_PIN1            5
#define PIN_10_MUX_AON_PWM_1                      6
#define PIN_10_MUX_CLK_EXT_REQ                    7
// GPIO
#define PIN_10_GPIO_PORT                          GPIO_PORT_AON
#define PIN_10_GPIO_NUM                           1

// Pin name: CLK_EXT_OUT
#define PIN_11                                    11
// MUX
#define PIN_11_MUX_CLK_EXT_OUT                    0
#define PIN_11_MUX_KBR_4                          1
#define PIN_11_MUX_AON_INT_6                      2
#define PIN_11_MUX_GPIO                           3
#define PIN_11_MUX_CLK_26M_EXTIN                  4
#define PIN_11_MUX_PDCORE_RSTN_0                  5
#define PIN_11_MUX_CAM_MCLK                       6
#define PIN_11_MUX_CLK_AU_MCLK                    7
// GPIO
#define PIN_11_GPIO_PORT                          GPIO_PORT_AON
#define PIN_11_GPIO_NUM                           6

// Pin name: SWD0_SWCLK
#define PIN_12                                    12
// MUX
#define PIN_12_MUX_SWD0_SWCLK                     0
#define PIN_12_MUX_KBR_4                          1
#define PIN_12_MUX_KBC_3                          2
#define PIN_12_MUX_UART1_RX                       3
#define PIN_12_MUX_GPIO                           4
#define PIN_12_MUX_SWD1_SWCLK                     5
// GPIO
#define PIN_12_GPIO_PORT                          GPIO_PORT_PD
#define PIN_12_GPIO_NUM                           24

// Pin name: SWD0_SWDIO
#define PIN_13                                    13
// MUX
#define PIN_13_MUX_SWD0_SWDIO                     0
#define PIN_13_MUX_KBR_3                          1
#define PIN_13_MUX_KBC_4                          2
#define PIN_13_MUX_UART1_TX                       3
#define PIN_13_MUX_GPIO                           4
#define PIN_13_MUX_SWD1_SWDIO                     5
// GPIO
#define PIN_13_GPIO_PORT                          GPIO_PORT_PD
#define PIN_13_GPIO_NUM                           25

// Pin name: CLK_AU_MCLK
#define PIN_14                                    14
// MUX
#define PIN_14_MUX_EN_BK2_N                       0
#define PIN_14_MUX_GPIO                           1
#define PIN_14_MUX_CLK_AU_MCLK                    2
#define PIN_14_MUX_CAM_MCLK                       3
#define PIN_14_MUX_PD_PWM                         4
// GPIO
#define PIN_14_GPIO_PORT                          GPIO_PORT_PD
#define PIN_14_GPIO_NUM                           26

// Pin name: PD_GPIO_0
#define PIN_21                                    21
// MUX
#define PIN_21_MUX_GPIO                           0
#define PIN_21_MUX_UART1_RX                       1
#define PIN_21_MUX_SSP0_CLK                       2
#define PIN_21_MUX_PD_INT_0                       3
#define PIN_21_MUX_I2C0_SCL                       4
#define PIN_21_MUX_CLK_AU_MCLK                    5
#define PIN_21_MUX_KBC_0                          6
#define PIN_21_MUX_LB_DEBUG_TEST_PIN_0            7
// GPIO
#define PIN_21_GPIO_PORT                          GPIO_PORT_PD
#define PIN_21_GPIO_NUM                           0

// Pin name: PD_GPIO_1
#define PIN_22                                    22
// MUX
#define PIN_22_MUX_GPIO                           0
#define PIN_22_MUX_UART1_TX                       1
#define PIN_22_MUX_SSP0_RXD                       2
#define PIN_22_MUX_PD_INT_1                       3
#define PIN_22_MUX_I2C0_SDA                       4
#define PIN_22_MUX_KBC_1                          6
#define PIN_22_MUX_LB_DEBUG_TEST_PIN_1            7
// GPIO
#define PIN_22_GPIO_PORT                          GPIO_PORT_PD
#define PIN_22_GPIO_NUM                           1

// Pin name: PD_GPIO_2
#define PIN_23                                    23
// MUX
#define PIN_23_MUX_GPIO                           0
#define PIN_23_MUX_SWD1_SWCLK                     1
#define PIN_23_MUX_SSP0_TXD                       2
#define PIN_23_MUX_PD_INT_2                       3
#define PIN_23_MUX_I2C1_SCL                       4
#define PIN_23_MUX_SPI_CAM_DI2                    5
#define PIN_23_MUX_KBC_2                          6
#define PIN_23_MUX_LB_DEBUG_TEST_PIN_2            7
// GPIO
#define PIN_23_GPIO_PORT                          GPIO_PORT_PD
#define PIN_23_GPIO_NUM                           2

// Pin name: PD_GPIO_3
#define PIN_24                                    24
// MUX
#define PIN_24_MUX_GPIO                           0
#define PIN_24_MUX_SWD1_SWDIO                     1
#define PIN_24_MUX_SSP0_CS                        2
#define PIN_24_MUX_PD_INT_3                       3
#define PIN_24_MUX_I2C1_SDA                       4
#define PIN_24_MUX_SPI_CAM_DI3                    5
#define PIN_24_MUX_KBC_3                          6
#define PIN_24_MUX_CAM_MCLK                       7
// GPIO
#define PIN_24_GPIO_PORT                          GPIO_PORT_PD
#define PIN_24_GPIO_NUM                           3

// Pin name: PD_GPIO_4
#define PIN_25                                    25
// MUX
#define PIN_25_MUX_GPIO                           0
#define PIN_25_MUX_I2C1_SCL                       1
#define PIN_25_MUX_PD_PWM                         2
#define PIN_25_MUX_SPI_CAM_DI3                    4
#define PIN_25_MUX_I2S0_WS                        5
#define PIN_25_MUX_KBC_4                          6
#define PIN_25_MUX_I2C0_SDA                       7
// GPIO
#define PIN_25_GPIO_PORT                          GPIO_PORT_PD
#define PIN_25_GPIO_NUM                           4

// Pin name: PD_GPIO_5
#define PIN_26                                    26
// MUX
#define PIN_26_MUX_GPIO                           0
#define PIN_26_MUX_I2C1_SDA                       1
#define PIN_26_MUX_PD_PWM                         2
#define PIN_26_MUX_SPI_CAM_TXD                    4
#define PIN_26_MUX_I2S0_CLK                       5
#define PIN_26_MUX_I2C0_SCL                       7
// GPIO
#define PIN_26_GPIO_PORT                          GPIO_PORT_PD
#define PIN_26_GPIO_NUM                           5

// Pin name: PD_GPIO_6
#define PIN_27                                    27
// MUX
#define PIN_27_MUX_GPIO                           0
#define PIN_27_MUX_I2C0_SCL                       1
#define PIN_27_MUX_UART2_RX                       2
#define PIN_27_MUX_I2S0_DIN                       5
#define PIN_27_MUX_SPI_LCD_DCX                    7
// GPIO
#define PIN_27_GPIO_PORT                          GPIO_PORT_PD
#define PIN_27_GPIO_NUM                           6

// Pin name: PD_GPIO_7
#define PIN_28                                    28
// MUX
#define PIN_28_MUX_GPIO                           0
#define PIN_28_MUX_I2C0_SDA                       1
#define PIN_28_MUX_UART2_TX                       2
#define PIN_28_MUX_I2S0_DOUT                      5
#define PIN_28_MUX_SPI_LCD_SDA                    7
// GPIO
#define PIN_28_GPIO_PORT                          GPIO_PORT_PD
#define PIN_28_GPIO_NUM                           7

// Pin name: PD_GPIO_8
#define PIN_29                                    29
// MUX
#define PIN_29_MUX_GPIO                           0
#define PIN_29_MUX_SPI_CAM_CS                     1
#define PIN_29_MUX_I2S0_WS                        2
#define PIN_29_MUX_PD_INT_4                       3
#define PIN_29_MUX_PD_PWM                         4
#define PIN_29_MUX_I2C0_SCL                       5
#define PIN_29_MUX_SIM0_RST                       6
// GPIO
#define PIN_29_GPIO_PORT                          GPIO_PORT_PD
#define PIN_29_GPIO_NUM                           8

// Pin name: PD_GPIO_9
#define PIN_30                                    30
// MUX
#define PIN_30_MUX_GPIO                           0
#define PIN_30_MUX_SPI_CAM_CLK                    1
#define PIN_30_MUX_I2S0_CLK                       2
#define PIN_30_MUX_PD_INT_5                       3
#define PIN_30_MUX_I2C0_SDA                       5
#define PIN_30_MUX_SIM0_CLK                       6
#define PIN_30_MUX_LB_DEBUG_TEST_PIN_3            7
// GPIO
#define PIN_30_GPIO_PORT                          GPIO_PORT_PD
#define PIN_30_GPIO_NUM                           9

// Pin name: PD_GPIO_10
#define PIN_31                                    31
// MUX
#define PIN_31_MUX_GPIO                           0
#define PIN_31_MUX_SPI_CAM_DI0                    1
#define PIN_31_MUX_I2S0_DIN                       2
#define PIN_31_MUX_UART1_RX                       3
#define PIN_31_MUX_KBC_3                          4
#define PIN_31_MUX_I2C1_SCL                       5
#define PIN_31_MUX_SIM0_DATA                      6
#define PIN_31_MUX_LB_DEBUG_TEST_PIN_4            7
// GPIO
#define PIN_31_GPIO_PORT                          GPIO_PORT_PD
#define PIN_31_GPIO_NUM                           10

// Pin name: PD_GPIO_11
#define PIN_32                                    32
// MUX
#define PIN_32_MUX_GPIO                           0
#define PIN_32_MUX_SPI_CAM_DI1                    1
#define PIN_32_MUX_I2S0_DOUT                      2
#define PIN_32_MUX_UART1_TX                       3
#define PIN_32_MUX_I2C0_SDA                       4
#define PIN_32_MUX_I2C1_SDA                       5
#define PIN_32_MUX_KBC_4                          6
#define PIN_32_MUX_SPI_LCD_DCX                    7
// GPIO
#define PIN_32_GPIO_PORT                          GPIO_PORT_PD
#define PIN_32_GPIO_NUM                           11

// Pin name: PD_GPIO_12
#define PIN_33                                    33
// MUX
#define PIN_33_MUX_GPIO                           0
#define PIN_33_MUX_SPI_LCD_CSX                    1
#define PIN_33_MUX_I2C1_SCL                       2
#define PIN_33_MUX_I2C0_SCL                       3
#define PIN_33_MUX_PD_PWM                         4
#define PIN_33_MUX_UART1_RX                       5
#define PIN_33_MUX_I2S0_WS                        6
// GPIO
#define PIN_33_GPIO_PORT                          GPIO_PORT_PD
#define PIN_33_GPIO_NUM                           12

// Pin name: PD_GPIO_13
#define PIN_34                                    34
// MUX
#define PIN_34_MUX_GPIO                           0
#define PIN_34_MUX_SPI_LCD_SCL                    1
#define PIN_34_MUX_UART1_RX                       2
#define PIN_34_MUX_I2C0_SDA                       3
#define PIN_34_MUX_UART1_TX                       5
#define PIN_34_MUX_I2S0_CLK                       6
#define PIN_34_MUX_LB_DEBUG_TEST_PIN_5            7
// GPIO
#define PIN_34_GPIO_PORT                          GPIO_PORT_PD
#define PIN_34_GPIO_NUM                           13

// Pin name: PD_GPIO_14
#define PIN_35                                    35
// MUX
#define PIN_35_MUX_GPIO                           0
#define PIN_35_MUX_SPI_LCD_SDI                    1
#define PIN_35_MUX_I2C1_SDA                       2
#define PIN_35_MUX_PD_INT_6                       3
#define PIN_35_MUX_SPI_CAM_TXD                    4
#define PIN_35_MUX_UART1_CTS                      5
#define PIN_35_MUX_I2S0_DIN                       6
#define PIN_35_MUX_SPI_LCD_DCX                    7
// GPIO
#define PIN_35_GPIO_PORT                          GPIO_PORT_PD
#define PIN_35_GPIO_NUM                           14

// Pin name: PD_GPIO_15
#define PIN_36                                    36
// MUX
#define PIN_36_MUX_GPIO                           0
#define PIN_36_MUX_SPI_LCD_SDO                    1
#define PIN_36_MUX_UART1_TX                       2
#define PIN_36_MUX_PD_INT_7                       3
#define PIN_36_MUX_KBC_0                          4
#define PIN_36_MUX_UART1_RTS                      5
#define PIN_36_MUX_I2S0_DOUT                      6
#define PIN_36_MUX_SPI_LCD_SDA                    7
// GPIO
#define PIN_36_GPIO_PORT                          GPIO_PORT_PD
#define PIN_36_GPIO_NUM                           15

// Pin name: SIM0_RST
#define PIN_37                                    37
// MUX
#define PIN_37_MUX_SIM0_RST                       0
#define PIN_37_MUX_GPIO                           1
#define PIN_37_MUX_I2C0_SCL                       2
#define PIN_37_MUX_KBC_2                          3
#define PIN_37_MUX_UART1_RX                       4
#define PIN_37_MUX_I2S0_CLK                       5
#define PIN_37_MUX_SPI_CAM_CS                     6
// GPIO
#define PIN_37_GPIO_PORT                          GPIO_PORT_PD
#define PIN_37_GPIO_NUM                           29

// Pin name: SIM0_CLK
#define PIN_38                                    38
// MUX
#define PIN_38_MUX_SIM0_CLK                       0
#define PIN_38_MUX_GPIO                           1
#define PIN_38_MUX_I2C0_SDA                       2
#define PIN_38_MUX_KBC_3                          3
#define PIN_38_MUX_UART1_TX                       4
#define PIN_38_MUX_I2S0_WS                        5
#define PIN_38_MUX_SPI_CAM_CLK                    6
// GPIO
#define PIN_38_GPIO_PORT                          GPIO_PORT_PD
#define PIN_38_GPIO_NUM                           30

// Pin name: SIM0_DATA
#define PIN_39                                    39
// MUX
#define PIN_39_MUX_SIM0_DATA                      0
#define PIN_39_MUX_GPIO                           1
#define PIN_39_MUX_PD_INT_4                       2
#define PIN_39_MUX_KBC_4                          3
#define PIN_39_MUX_KBC_3                          4
#define PIN_39_MUX_I2S0_DIN                       5
#define PIN_39_MUX_SPI_CAM_DI1                    6
#define PIN_39_MUX_PD_PWM                         7
// GPIO
#define PIN_39_GPIO_PORT                          GPIO_PORT_PD
#define PIN_39_GPIO_NUM                           31

// Pin name: RF_CONTROL_0
#define PIN_40                                    40
// MUX
#define PIN_40_MUX_GPIO                           0
#define PIN_40_MUX_RFC_GPIO_0                     1
#define PIN_40_MUX_RF_SPI0_CS                     2
#define PIN_40_MUX_KBC_2                          3
#define PIN_40_MUX_LB_DEBUG_TEST_PIN_6            7
// GPIO
#define PIN_40_GPIO_PORT                          GPIO_PORT_PD
#define PIN_40_GPIO_NUM                           16

// Pin name: RF_CONTROL_1
#define PIN_41                                    41
// MUX
#define PIN_41_MUX_GPIO                           0
#define PIN_41_MUX_RFC_GPIO_1                     1
#define PIN_41_MUX_RF_SPI0_CLK                    2
#define PIN_41_MUX_KBC_3                          3
#define PIN_41_MUX_LB_DEBUG_TEST_PIN_7            7
// GPIO
#define PIN_41_GPIO_PORT                          GPIO_PORT_PD
#define PIN_41_GPIO_NUM                           17

// Pin name: RF_CONTROL_2
#define PIN_42                                    42
// MUX
#define PIN_42_MUX_GPIO                           0
#define PIN_42_MUX_RFC_GPIO_2                     1
#define PIN_42_MUX_RF_SPI0_DIN                    2
#define PIN_42_MUX_CLK_AU_MCLK                    3
#define PIN_42_MUX_UART2_RX                       4
#define PIN_42_MUX_SWD1_SWCLK                     5
#define PIN_42_MUX_LB_DEBUG_TEST_PIN_8            7
// GPIO
#define PIN_42_GPIO_PORT                          GPIO_PORT_PD
#define PIN_42_GPIO_NUM                           18

// Pin name: RF_CONTROL_3
#define PIN_43                                    43
// MUX
#define PIN_43_MUX_GPIO                           0
#define PIN_43_MUX_RFC_GPIO_3                     1
#define PIN_43_MUX_RF_SPI0_DOUT                   2
#define PIN_43_MUX_CAM_MCLK                       3
#define PIN_43_MUX_UART2_TX                       4
#define PIN_43_MUX_SWD1_SWDIO                     5
#define PIN_43_MUX_LB_DEBUG_TEST_PIN_9            7
// GPIO
#define PIN_43_GPIO_PORT                          GPIO_PORT_PD
#define PIN_43_GPIO_NUM                           19

// Pin name: RF_CONTROL_4
#define PIN_44                                    44
// MUX
#define PIN_44_MUX_GPIO                           0
#define PIN_44_MUX_RFC_GPIO_4                     1
#define PIN_44_MUX_RF_SPI0_CS                     2
#define PIN_44_MUX_RFFE_SDATA                     3
#define PIN_44_MUX_UART2_RX                       4
#define PIN_44_MUX_LB_DEBUG_TEST_PIN_10           7
// GPIO
#define PIN_44_GPIO_PORT                          GPIO_PORT_PD
#define PIN_44_GPIO_NUM                           20

// Pin name: RF_CONTROL_5
#define PIN_45                                    45
// MUX
#define PIN_45_MUX_GPIO                           0
#define PIN_45_MUX_RFC_GPIO_5                     1
#define PIN_45_MUX_RF_SPI0_CLK                    2
#define PIN_45_MUX_RFFE_SCLK                      3
#define PIN_45_MUX_UART2_TX                       4
#define PIN_45_MUX_LB_DEBUG_TEST_PIN_11           7
// GPIO
#define PIN_45_GPIO_PORT                          GPIO_PORT_PD
#define PIN_45_GPIO_NUM                           21

// Pin name: RF_CONTROL_6
#define PIN_46                                    46
// MUX
#define PIN_46_MUX_BOOT_MODE_0                    0
#define PIN_46_MUX_GPIO                           1
#define PIN_46_MUX_RF_SPI0_DIN                    2
#define PIN_46_MUX_KBC_0                          3
#define PIN_46_MUX_SWD1_SWCLK                     4
// #define PIN_46_MUX_BOOT_MODE_0                    5
#define PIN_46_MUX_PD_GPIO_22                     6
#define PIN_46_MUX_LB_DEBUG_TEST_PIN_12           7
// GPIO
#define PIN_46_GPIO_PORT                          GPIO_PORT_PD
#define PIN_46_GPIO_NUM                           6

// Pin name: RF_CONTROL_7
#define PIN_47                                    47
// MUX
#define PIN_47_MUX_BOOT_MODE_1                    0
#define PIN_47_MUX_GPIO                           1
#define PIN_47_MUX_RF_SPI0_DOUT                   2
#define PIN_47_MUX_KBC_1                          3
#define PIN_47_MUX_SWD1_SWDIO                     4
// #define PIN_47_MUX_BOOT_MODE_1                    5
#define PIN_47_MUX_PD_GPIO_23                     6
#define PIN_47_MUX_LB_DEBUG_TEST_PIN_13           7
// GPIO
#define PIN_47_GPIO_PORT                          GPIO_PORT_PD
#define PIN_47_GPIO_NUM                           7

// Pin name: AON_GPIO_7
#define PIN_48                                    48
// MUX
#define PIN_48_MUX_GPIO                           0
#define PIN_48_MUX_KBC_4                          1
#define PIN_48_MUX_AON_INT_7                      2
#define PIN_48_MUX_UART3_RX                       3
// GPIO
#define PIN_48_GPIO_PORT                          GPIO_PORT_AON
#define PIN_48_GPIO_NUM                           7

// Pin name: AON_GPIO_8
#define PIN_49                                    49
// MUX
#define PIN_49_MUX_GPIO                           0
#define PIN_49_MUX_KBR_4                          1
#define PIN_49_MUX_AON_INT_8                      2
#define PIN_49_MUX_UART3_TX                       3
// GPIO
#define PIN_49_GPIO_PORT                          GPIO_PORT_AON
#define PIN_49_GPIO_NUM                           8

// Pin name: PD_GPIO_16
#define PIN_50                                    50
// MUX
#define PIN_50_MUX_GPIO                           0
#define PIN_50_MUX_SSP1_CS                        1
#define PIN_50_MUX_UART3_TX                       2
#define PIN_50_MUX_I2C2_SCL                       3
#define PIN_50_MUX_SPI_CAM_CS                     4
#define PIN_50_MUX_I2S0_WS                        5
#define PIN_50_MUX_PD_INT_8                       6
// GPIO
#define PIN_50_GPIO_PORT                          GPIO_PORT_PD
#define PIN_50_GPIO_NUM                           27

// Pin name: PD_GPIO_17
#define PIN_51                                    51
// MUX
#define PIN_51_MUX_GPIO                           0
#define PIN_51_MUX_SSP1_CLK                       1
#define PIN_51_MUX_UART3_RX                       2
#define PIN_51_MUX_I2C2_SDA                       3
#define PIN_51_MUX_SPI_CAM_CLK                    4
#define PIN_51_MUX_I2S0_CLK                       5
#define PIN_51_MUX_PD_INT_9                       6
// GPIO
#define PIN_51_GPIO_PORT                          GPIO_PORT_PD
#define PIN_51_GPIO_NUM                           28

// Pin name: PD_GPIO_18
#define PIN_52                                    52
// MUX
#define PIN_52_MUX_GPIO                           0
#define PIN_52_MUX_SSP1_RXD                       1
#define PIN_52_MUX_UART3_CTS                      2
#define PIN_52_MUX_I2C3_SCL                       3
#define PIN_52_MUX_SPI_CAM_DI0                    4
#define PIN_52_MUX_I2S0_DIN                       5
#define PIN_52_MUX_PD_INT_10                      6
// GPIO
#define PIN_52_GPIO_PORT                          GPIO_PORT_PD
#define PIN_52_GPIO_NUM                           32

// Pin name: PD_GPIO_19
#define PIN_53                                    53
// MUX
#define PIN_53_MUX_GPIO                           0
#define PIN_53_MUX_SSP1_TXD                       1
#define PIN_53_MUX_UART3_RTS                      2
#define PIN_53_MUX_I2C3_SDA                       3
#define PIN_53_MUX_SPI_CAM_DI1                    4
#define PIN_53_MUX_I2S0_DOUT                      5
#define PIN_53_MUX_PD_INT_11                      6
// GPIO
#define PIN_53_GPIO_PORT                          GPIO_PORT_PD
#define PIN_53_GPIO_NUM                           33

#endif  // _CHIP_PIN_LIST
/** @} */