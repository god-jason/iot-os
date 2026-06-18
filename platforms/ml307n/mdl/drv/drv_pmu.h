/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_pmu.h
 *
 * @brief       PMU驱动代码
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team          创建
 ************************************************************************************
 */
#ifndef __DRV_PMU__
#define __DRV_PMU__

/************************************************************************************
 *                                 头文件定义
 ************************************************************************************/
#include <drv_common.h>

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define PMU_AON_BASE_ADDR               BASE_PMU_AON_CTRL
#define PMU_RESET_CTRL                  (PMU_AON_BASE_ADDR)
#define PMU_CTRL_DEBUG                  (PMU_AON_BASE_ADDR + 0x30)
#define PMU_CTRL_AON                    (PMU_AON_BASE_ADDR + 0x100)

// RSTN_FLAG
#define POR5_RSTN_FLAG              (1 << 0)
#define EXT_RSTN_FLAG               (1 << 1)
#define G_SW_RSTN_FLAG              (1 << 2)
#define APSS_WDT_RSTN_FLAG          (1 << 3)
#define CPSS_WDT_RSTN_FLAG          (1 << 4)

//ps hold
#define PS_HOLD_EN_BIT              0
#define EXTRST_MODE_BIT             1
#define LDO1OK_BYPASS_BIT           2

#define APSS_WDTRST_BYPASS_BIT      0
#define CPSS_WDTRST_BYPASS_BIT      1

#define RC32K_EN_BIT                11
#define RC32K_HWTRIM_BIT            0
#define PMU_CTRL_FSM_REQEN_BIT      3
#define SW_RESET_BIT                12

//keyon ctrl
#define KEY_DET_CLKGATE_EN_BIT      12
#define LP_SHUTDOWN_EN_BIT          8
#define LLP_SHUTDOWN_EN_BIT         9
#define LLP_RESTART_EN_BIT          10

#define LDO_STABLE_TIME_OFF         4
#define LDO_STABLE_TIME_WID         4

#define DCDC_STABLE_TIME_OFF        23
#define DCDC_STABLE_TIME_WID        6

#define RFBGP_STABLE_TIME_OFF       18
#define RFBGP_STABLE_TIME_WID       5

#define POWERUP_DELAY_TIME_OFF      23
#define POWERUP_DELAY_TIME_WID      5

#define SW_RSET_DELAY_TIME_OFF      2
#define SW_RSET_DELAY_TIME_WID      10

#define SP_TIME_OFFSET              0
#define SP_TIME_WIDTH               4
#define LP_TIME_OFFSET              4
#define LP_TIME_WIDTH               4
#define LLP_TIME_OFFSET             8
#define LLP_TIME_WIDTH              4

#define KEY_CLKGATE_OFFSET          12

#define RESTART_TIME_OFFSET         11
#define RESTART_TIME_WIDTH          4

#define RTC_REG_STATUS_TIMER_INT    (1 << 7)
#define RTC_REG_STATUS_ALARM_INT    (1 << 6)

//SW DCXO SEL
#define SW_RF_BGP_EN_LV_SEL_BIT                15
#define SW_TX_PLL_LOOP_REGUL_ECO_SEL_BIT       18
#define SW_DCXO_VCOMP_HIGH_SEL_SEL_BIT         19
#define SW_DCXO_GMCAL_HOLDON_PULSE_SEL_BIT     20

#define DEEPSLEEP_RC32K_HW_TRIM_BIT            28

#define AMT_MODE_FLAG               0x544D

#define LDO_TIME_INTVAL             244
#define DCDC_TIME_INTVAL            1920
#define RFBGP_TIME_INTVAL           244
#define POWERUP_DELAY_TIME_INTVAL   244
#define SW_REST_DELAY_INTVAL        1950
#define SP_TIME_INTVAL              125000
#define LP_TIME_INTVAL              1000000
#define LLP_TIME_INTVAL             1000000
#define RESTART_DELAY_INTVAL        125000

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

typedef enum
{
/*** Bit definition for [regResetCtrl] register(0x00000000) ****/
    PMU_REG_APSS_WDT_RSTN_BYPASS        = (0x00000001),  /*!<apss_wdt_rstn  bypass */
    PMU_REG_CPSS_WDT_RSTN_BYPASS        = (0x00000101),  /*!<cpss_wdt_rstn  bypass */
    PMU_REG_SW_RSTN_DELAY               = (0x0000020a),  /*!<sw reset delay time ,2ms < time < 2s,unit:1.95ms */
    PMU_REG_GLOBAL_SW_RSTN_FLAG         = (0x00000c01),  /*!<reg_global_sw_rstn_flag */
    PMU_REG_APSS_WDT_RSTN_FLAG          = (0x00000d01),  /*!<reg_apss_wdt_rstn_flag */
    PMU_REG_CPSS_WDT_RSTN_FLAG          = (0x00000e01),  /*!<reg_cpss_wdt_rstn_flag */
    PMU_REG_GLOBAL_SW_RSTN_FLAG_CLR     = (0x00000f01),  /*!<reg_global_sw_rstn_flag clear, write opration clear */
    PMU_REG_APSS_WDT_RSTN_FLAG_CLR      = (0x00001001),  /*!<reg_apss_wdt_rstn_flag clear,write  opration clear */
    PMU_REG_CPSS_WDT_RSTN_FLAG_CLR      = (0x00001101),  /*!<reg_cpss_wdt_rstn_flag clear,write  opration clear */
    PMU_REG_RFBGP_STABLE_TIME           = (0x00001205),  /*!<RF BGP stable time,default 3ms，unit:244us */
    PMU_REG_POWERUP_DELAY_TIME          = (0x00001705),  /*!<power up delay time,default 5ms，unit:244us */


    /*** Bit definition for [regPmuCtrlDebug] register(0x00000030) ****/
    PMU_PMU_SBY_TESTPINS                = (0x00300010),  /*!<pmu_ctrl_testpins */
    PMU_PMU_AON_TESTPINS                = (0x00301010),  /*!<pmu_ctrl_testpins */


    /*** Bit definition for [regPmuCtrlAon] register(0x00000100) ****/
    PMU_REG_CHIP_LS_MODE                = (0x01000001),  /*!<Light sleep mode */
    PMU_REG_CHIP_DS_MODE                = (0x01000101),  /*!<Deep sleep mode */
    PMU_REG_PCU_AON_STATUS_BYPASS       = (0x01000201),  /*!<Bypass PCU_SBY休眠入口条件：PCU_AON休眠状态 */
    PMU_REG_PMU_AON_FSM_REQEN           = (0x01000301),  /*!<PCU_SBY 状态机自动时钟门控请求使能 */
    PMU_REG_MAIN_CLKGATE_EN             = (0x01000401),  /*!<26m主时钟门控 */
    PMU_REG_DCXO_ON                     = (0x01000501),  /*!<DCXO 32Kless模式, dcxo on */
    PMU_REG_RFBGP_ON                    = (0x01000601),  /*!<DCXO 32Kless模式下，控制rfbgp 是否关闭 */
    PMU_REG_CLK_REQ_SEL                 = (0x01000701),  /*!< */
    PMU_REG_CLK_REQ                     = (0x01000801),  /*!< */
    PMU_REG_PDCORE_ISOEN_SEL            = (0x01000901),  /*!< */
    PMU_REG_PDCORE_ISOEN                = (0x01000a01),  /*!< */
    PMU_REG_PDCORE_RSTN_BYPASS          = (0x01000b01),  /*!< */
    PMU_GLOBAL_SW_RSTN                  = (0x01000c01),  /*!<全局软复位 */
    PMU_SW_DCXO_LDO_EN_SEL              = (0x01000d01),  /*!< */
    PMU_SW_RF_LPBGP_EN_SEL              = (0x01000e01),  /*!< */
    PMU_SW_RF_BGP_EN_SEL                = (0x01000f01),  /*!< */
    PMU_SW_ABBPLL_LDO_EN_SEL            = (0x01001001),  /*!< */
    PMU_SW_DCXO_BUFFER_EN_SEL           = (0x01001101),  /*!< */
    PMU_REG_TX_PLL_LOOP_REGUL_ECO_SEL   = (0x01001201),  /*!< */
    PMU_REG_DCXO_VCOMP_HIGH_SEL_SEL     = (0x01001301),  /*!< */
    PMU_REG_DCXO_GMCAL_HOLDON_PULSE_SEL = (0x01001401),  /*!< */
    PMU_SW_SIPPMU_BK1_SEL               = (0x01001901),  /*!< */
    PMU_REG_SIPPMU_BK1_EN               = (0x01001a01),  /*!< */
    PMU_REG_SIPPMU_BK1_FPWM             = (0x01001b01),  /*!< */
    PMU_REG_RC32K_HW_TRIM_EN            = (0x01001c01),  /*!<DS模式下，触发32K RC 硬件trim的使能 */
} PMU_RegPmuCtrlAon;


typedef enum {
    PS_HOLD_EN_ID,
    EXTRST_MODE_ID,
    APSS_WDTRST_BYPASS_ID,
    CPSS_WDTRST_BYPASS_ID,
    RC32K_EN_ID,
    PMU_CTRL_FSM_ID,
    KEY_DET_CLKGATE_ID,
    LP_SHUTDOWN_ID,
    LLP_SHUTDOWN_ID,
    LLP_RESTART_ID,
    RC32K_HWTRIM_ID,
    SW_RESET_ID,
} PMU_FuncId;

typedef enum {
    LDO_STABLE_TIME_ID,
    DCDC_STABLE_TIME_ID,
    RFBGP_STABLE_TIME_ID,
    RFLDO_STABLE_TIME_ID,
    POWERUP_DELAY_TIME_ID,
    SW_RSET_DELAY_TIME_ID,
    SP_TIME_ID,
    LP_TIME_ID,
    LLP_TIME_ID,
    RESTART_TIME_ID,
} PMU_TimeId;

typedef enum {
    RB_POWER_KEY = 0,
    RB_USB_INSERT = 1,
    RB_RTC = 2,
    RB_POWER_KEY_LONG = 3,
    RB_RESET_NOMAL = 4,
    RB_RESET_USB_OFF = 5,
    RB_RESET_EXCEPT = 6,
    RB_POWER_BOOST_IN = 7,
    RB_RESET_ALARM = 8,
    RB_AMT = 9,
    RB_PRODUCTION = 10,
    RB_WDT = 11,
    RB_FOTA = 12,
    RB_CHARGER = 13,
    RB_RESET_STANDBY = 14,
    RB_RESET_KEY = 15,
    UNKNOWN_BOOT_REASON
} BOOT_Reason;

typedef enum {
    PT_KEYON_CHARGER = 1,
    PT_CHARGER = 2,
    PT_KEYON = 3,
    PT_USB_INSERT = 4,
    UNKNOWN_POWER_TYPE
} POWERON_Type;
/**
 ************************************************************************************
 * @brief           配置PMU 寄存器等
 *
 * @param[in]       regBit      地址偏移/寄存器BIT偏移/位宽
 * @param[in]       bitVal      配置值
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PMU_SetRegPmuCtrlAon(PMU_RegPmuCtrlAon regBit, uint32_t bitVal);

/**
 ************************************************************************************
 * @brief           获取PMU 配置
 * @param[in]       regBit      地址偏移/寄存器BIT偏移/位宽
 *
 * @return          uint32_t
 * @retval
 ************************************************************************************
*/
uint32_t PMU_GetRegPmuCtrlAon(PMU_RegPmuCtrlAon regBit);

/**
 ************************************************************************************
 * @brief           pmu 按键等模块功能禁用/使能
 * @param[in]       PMU_FuncId              功能模块ID
 * @param[in]       bool_t                  禁用/使能
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PMU_FuncCfg(PMU_FuncId id, bool_t en);

/**
 ************************************************************************************
 * @brief           pmu模块按键时长，模块复位延时等时长配置
 * @param[in]       PMU_TimeId              time功能模块ID
 * @param[in]       uint32_t                时长（us）
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PMU_TimeCfg(PMU_TimeId id, uint32_t us);

#endif

/** @} */