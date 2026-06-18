/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_pcu.h
 *
 * @brief       PCU驱动代码
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team          创建
 ************************************************************************************
 */
#ifndef __DRV_PCU__
#define __DRV_PCU__

/************************************************************************************
 *                                 头文件定义
 ************************************************************************************/
#include <drv_common.h>
#include <os_def.h>
#include <os_hw.h>

/**
 * @addtogroup Pcu
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define LSP_SBY_BASE_ADDR           BASE_SBY_LSP
#define LSP_SBY_POR_RESET_CTL       (LSP_SBY_BASE_ADDR + 0x00)
#define LSP_SBY_SHADOW0             (LSP_SBY_BASE_ADDR + 0x08)
#define LSP_SBY_SHADOW1             (LSP_SBY_BASE_ADDR + 0x0C)
#define LSP_SBY_SHADOW2             (LSP_SBY_BASE_ADDR + 0x10)
#define LSP_SBY_SHADOW4             (LSP_SBY_BASE_ADDR + 0x14)
#define LSP_SBY_SHADOW5             (LSP_SBY_BASE_ADDR + 0x18)
#define LSP_SBY_PMU_CTL             (LSP_SBY_BASE_ADDR + 0x20)
#define LSP_SBY_KEY_DETECT          (LSP_SBY_BASE_ADDR + 0x30)
#define LSP_SBY_INT_CTRL            (LSP_SBY_BASE_ADDR + 0x40)
#define LSP_SBY_INT_CTRL2           (LSP_SBY_BASE_ADDR + 0x44)
#define LSP_SBY_32K_CFG             (LSP_SBY_BASE_ADDR + 0x50)
#define LSP_SBY_32K_TRIM_CFG        (LSP_SBY_BASE_ADDR + 0x54)
#define LSP_SBY_32K_TRIM_CFG2       (LSP_SBY_BASE_ADDR + 0x58)
#define LSP_SBY_SHADOW3             (LSP_SBY_BASE_ADDR + 0x60)
#define LSP_SBY_SW_CTRL_SEL         (LSP_SBY_BASE_ADDR + 0x70)
#define LSP_SBY_LDO1_CONFIG         (LSP_SBY_BASE_ADDR + 0x224)
#define LSP_SBY_OPM_CONFIG          (LSP_SBY_BASE_ADDR + 0x244)
#define LSP_SBY_LPBGR_CONFIG        (LSP_SBY_BASE_ADDR + 0x248)
#define LSP_SBY_DCDC2_TRIM          (LSP_SBY_BASE_ADDR + 0x250)
#define LSP_SBY_LDO78_TRIM          (LSP_SBY_BASE_ADDR + 0x254)
#define LSP_SBY_CLK_RST_PARA        (LSP_SBY_BASE_ADDR + 0x300)

#define LSP_SBY_INT_WIDTH           6
#define LSP_SBY_INT_STATE_OFF       16
#define LSP_SBY_ABNL_INT_STATE_OFF  10
#define LSP_SBY_INT_CLR_OFF         8
#define LSP_SBY_ABNL_INT_CLR_OFF    18

#define LSP_SBY_EXT_INT_CLR_BIT         10
#define LSP_SBY_EXT_INT_TYP_BIT         11
#define LSP_SBY_EXT_INT_POL_BIT         12
#define LSP_SBY_EXT_INT_CPSSDIS_BIT     13
#define LSP_SBY_EXT_INT_APSSDIS_BIT     14
#define LSP_SBY_EXT_INT_AONDIS_BIT      15
#define LSP_SBY_EXT_INT_SBYDIS_BIT      16


#define PCU_AON_BASE_ADDR           BASE_PCU_AON
#define PCU_AON_APSS_CFG            (PCU_AON_BASE_ADDR + 0x00)
#define PCU_AON_CPSS_CFG            (PCU_AON_BASE_ADDR + 0x04)
#define PCU_AON_APSS_SLEEP_TIME     (PCU_AON_BASE_ADDR + 0x100)
#define PCU_AON_CPSS_SLEEP_TIME     (PCU_AON_BASE_ADDR + 0x104)
#define PCU_AON_SLEEP_TIME_LOAD     (PCU_AON_BASE_ADDR + 0x10C)
#define PCU_AON_SLEEP_BYPASS        (PCU_AON_BASE_ADDR + 0x200)
#define PCU_AON_INT_MASK_CTL        (PCU_AON_BASE_ADDR + 0x204)
#define PCU_AON_DELAY_TIME          (PCU_AON_BASE_ADDR + 0x208)
#define PCU_AON_ST_THRESHOLD        (PCU_AON_BASE_ADDR + 0x20C)
#define PCU_AON_MEM_DELAY_TIME      (PCU_AON_BASE_ADDR + 0x210)
#define PCU_AON_SHRAM_MODE          (PCU_AON_BASE_ADDR + 0x214)
#define PCU_AON_MAIN_CLK_CFG        (PCU_AON_BASE_ADDR + 0x218)
#define PCU_AON_CLK_EN              (PCU_AON_BASE_ADDR + 0x21C)
#define PCU_AON_SW_MUX_CFG          (PCU_AON_BASE_ADDR + 0x300)
#define PCU_AON_SHRAM_MUX_CFG       (PCU_AON_BASE_ADDR + 0x304)
#define PCU_AON_PVSENSE_RETOFF      (PCU_AON_BASE_ADDR + 0x400)
#define PCU_AON_WU_INTCLR           (PCU_AON_BASE_ADDR + 0x500)
#define PCU_AON_WU_INTTYP           (PCU_AON_BASE_ADDR + 0x504)
#define PCU_AON_WU_INTPOL           (PCU_AON_BASE_ADDR + 0x508)
#define PCU_AON_WU_INT_DISSBY       (PCU_AON_BASE_ADDR + 0x50C)
#define PCU_AON_WU_INT_DISAON       (PCU_AON_BASE_ADDR + 0x510)
#define PCU_AON_WU_INT_DISAPSS      (PCU_AON_BASE_ADDR + 0x514)
#define PCU_AON_WU_INT_DISCPSS      (PCU_AON_BASE_ADDR + 0x518)
#define PCU_AON_WU_INTCLR1          (PCU_AON_BASE_ADDR + 0x520)
#define PCU_AON_WU_INTTYP1          (PCU_AON_BASE_ADDR + 0x524)
#define PCU_AON_WU_INTPOL1          (PCU_AON_BASE_ADDR + 0x528)
#define PCU_AON_WU_INT_DISSBY1      (PCU_AON_BASE_ADDR + 0x52C)
#define PCU_AON_WU_INT_DISAON1      (PCU_AON_BASE_ADDR + 0x530)
#define PCU_AON_WU_INT_DISAPSS1     (PCU_AON_BASE_ADDR + 0x534)
#define PCU_AON_WU_INT_DISCPSS1     (PCU_AON_BASE_ADDR + 0x538)
#define PCU_AON_ECTINT_SYNCEN       (PCU_AON_BASE_ADDR + 0x540)
#define PCU_AON_WKUP_INT_FLAG       (PCU_AON_BASE_ADDR + 0x544)
#define PCU_AON_WKUP_INT_FLAG1      (PCU_AON_BASE_ADDR + 0x54C)
#define PCU_AON_CALI_CFG            (PCU_AON_BASE_ADDR + 0x600)
#define PCU_AON_CALI_DELAY_TIME     (PCU_AON_BASE_ADDR + 0x604)
#define PCU_AON_TESTPINS            (PCU_AON_BASE_ADDR + 0x700)
#define PCU_AON_SS_TESTPINS         (PCU_AON_BASE_ADDR + 0x704)
#define PCU_AON_TESTPIN_SEL         (PCU_AON_BASE_ADDR + 0x708)

#define PCU_AON_INT_CLR_OFF         0
#define PCU_AON_INT_CLR_WIDTH       6

#define AON_SYSCTRL_BASE_ADDR       BASE_AON_SYS_CTRL
#define AON_AP_BOOT_CFG             (AON_SYSCTRL_BASE_ADDR + 0x0)
#define AON_CP_BOOT_CFG             (AON_SYSCTRL_BASE_ADDR + 0x4)
#define AON_SPINLOCK0_ADDR          (AON_SYSCTRL_BASE_ADDR + 0x94)
#define AON_SPINLOCK1_ADDR          (AON_SYSCTRL_BASE_ADDR + 0x98)
#define AON_REG_PDCORE_CFG          (AON_SYSCTRL_BASE_ADDR + 0x9C)
#define AON_BP_CONFIG               (AON_SYSCTRL_BASE_ADDR + 0x134)
#define AON_RF_CONTROL              (AON_SYSCTRL_BASE_ADDR + 0x308)

//DCXO HIGH SEL
#define RF_BGP_EN_LV_BIT                2
#define TX_PLL_LOOP_REGUL_ECO_BIT       11
#define DCXO_VCOMP_HIGH_SEL_WID         3
#define DCXO_VCOMP_HIGH_SEL_OFF         13
#define DCXO_GMCAL_HOLDON_PULSE_BIT     17
#define DCXO_VCOMP_HIGH_SEL_SLEEP_VAL   2
#define DCXO_VCOMP_HIGH_SEL_WAKEUP_VAL  4

#define CP_RESET_BIT_CTL            (0)
#define BUF_CORE_EN_BIT_CTL         (1)

#define AON_CMN_DT_OFF              0
#define AON_CMN_DT_WIDTH            8
#define AON_WFI_CLR_TIME_OFF        8
#define AON_WFI_CLR_TIME_WIDTH      8
#define AON_TPUEN_DT_OFF            16
#define AON_TPUEN_DT_WIDTH          16
#define MEM_PSDH_DT_OFF             0
#define MEM_PSDH_DT_WIDTH           8
#define MEM_PSDL_DT_OFF             8
#define MEM_PSDL_DT_WIDTH           8
#define MEM_LP2NOR_DT_OFF           16
#define MEM_LP2NOR_DT_WIDTH         8

#define AON_CALI_DT_OFF             0
#define AON_CALI_DT_WIDTH           24

#define CORE_ID_MASK                0x3
#define PCU_WU_MASK                 (0x3 << 2)

#define SBY_WAKEUP_INT_CLR_OFF      1
#define SBY_WAKEUP_INT_TYP_OFF      6
#define SBY_WAKEUP_INT_POL_OFF      11
#define SBY_WAKEUP_INT_DISSBY_OFF   16
#define SBY_WAKEUP_INT_DISAON_OFF   21
#define SBY_WAKEUP_INT_DISAPSS_OFF  26
#define SBY_WAKEUP_INT_DISCPSS_OFF  0

#define MAX_IRQNUM_AON              34
#define MAX_IRQNUM_SBY              6

#define SBY_EXTINT_EVENT            (1 << 0)
#define SBY_SPINT_EVENT             (1 << 3)
#define SBY_RTCINT_EVENT            (1 << 4)
#define SBY_EVENT_REGISTERED        (1 << 7)

#define PERVAL_26M                  (39)
#define NS_TO_VAL(ns)               ((ns + PERVAL_26M - 1) / PERVAL_26M)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef void (* PCU_SbyCbFp)(void);

/**
\brief: pcu irq trigger type.
*/
typedef enum {
    ICT_PCU_HIGH_LEVEL    = 0,
    ICT_PCU_LOW_LEVEL     = 1,
    ICT_PCU_POSITIVE_EDGE = 2,
    ICT_PCU_NEGATIVE_EDGE = 3,
} PCU_IrqType;

/**
\brief: pcu chip mode.
*/
typedef enum {
    CHIP_LS_MODE = 0,
    CHIP_DS_MODE,
    CHIP_SBY_MODE,
    CHIP_SDN_MODE,
} PCU_ChipMode;

/**
\brief:ID of pcu aon supports functions.
*/
typedef enum {
    SPLL0_OFF = 0,
    MAIN_CLK_26M_EN,
    AON_CLK_CFG_REG,
    PCU2TPU_CLK_EN,
    PCU_WCLK_REQEN,
} PCU_AonFuncId;

/**
\brief:pcu IOPD id.
*/
typedef enum {
    PD0 = 0,
    PD1,
    PD2,
    PD3,
    PD4,
} PCU_PdId;

/**
\brief:sby wakeup irq event.
*/
typedef enum {
    SBY_EXTINT_EVENTID      = 0,
    SBY_SPINT_EVENTID       = 1,
    SBY_RTCINT_EVENTID      = 2,
    SBY_EVENTID_MAX,
}PCU_SbyEventID;

/**
\brief:sby wakeup irq num.
*/
enum {
    SBY_EXT_INT                 = OS_EXT_IRQ_TO_IRQ(AP_INT_NUM_1),
    SBY_LLP_INT,
    SBY_LP_INT,
    SBY_SP_INT,
    SBY_RTC_INT,
};

/**
\brief:aon wakeup irq num.
*/
enum {
    AON_KEY_INT = OS_EXT_IRQ_TO_IRQ(AP_INT_NUM_7),
    AON_LB_LSP_GPIO_INT,
    AON_LB_CP_PWM_TIMER_INT0,
    AON_LB_CP_PWM_TIMER_INT1,
    AON_LB_PS_PWM_TIMER_INT0,
    AON_LB_PS_PWM_TIMER_INT1,
    AON_EXT_INT0,
    AON_EXT_INT1,
    AON_EXT_INT2,
    AON_EXT_INT3,
    AON_EXT_INT4,
    AON_LPUART_CTS_INT,
    AON_LPUART_INT,
    AON_LTE_LPM_SUBGTAME_INT,
    AON_LTE_LPM_INT_FOR_PS,
    AON_LTE_EXT_NMI,
    AON_LTE_TIME2_IND,
    AON_LTE_TIME1_IND,
    PDCORE_CP_ICP_INT,
    PDCORE_PS_ICP_INT,
    PDCORE_PD_INT_IN0,
    PDCORE_PD_INT_IN1,
    PDCORE_PD_INT_IN2,
    PDCORE_PD_INT_IN3,
    PDCORE_PD_INT_IN4,
    PDCORE_PD_INT_IN5,
    PDCORE_PD_INT_IN6,
    PDCORE_PD_INT_IN7,
    PDCORE_GPIO_INT0,
    PDCORE_GPIO_INT1,
    PDCORE_LSP_ICP_INT0,
    PDCORE_LSP_ICP_INT1,
    AON_EXT_INT5,
    AON_EXT_INT6,
    AON_EXT_INT7 = OS_EXT_IRQ_TO_IRQ(AP_INT_NUM_74),
    AON_EXT_INT8,
    PDCORE_GPIO_INT2,
    PDCORE_PD_INT_IN8,
    PDCORE_PD_INT_IN9,
    PDCORE_PD_INT_IN10,
    PDCORE_PD_INT_IN11,
};

/**
\brief:ID of pcu aon supports delay time functions.
*/
typedef enum {
    AON_CMN_DT,
    AON_WFI_CLR_TIME,
    AON_TPUEN_DT,
    MEM_PSDH_DT,
    MEM_PSDL_DT,
    MEM_LP2NOR_DT,
} PCU_AonDelaytimeId;

typedef enum
{
    /*** Bit definition for [regApssConfig] register(0x00000000) ****/
    PCU_REG_APSS_DS_MODE                = (0x00000001),  /*!<apss deepsleep mode */
    PCU_REG_APSS_CPU_IDLE_BYPASS        = (0x00000101),  /*!<apss cpu idle bypass */
    PCU_REG_APSS_PERI_IDLE_BYPASS       = (0x00000201),  /*!<apss peri idle bypass */
    PCU_REG_APSS_WORK_CLK_ENABLE        = (0x00000301),  /*!<apss work clk gate enable */
    PCU_REG_APSS_INT_MASK_CONFIG        = (0x00000401),  /*!<software config apss int mask */
    PCU_REG_APSS_INT_MASK_ENABLE        = (0x00000501),  /*!<software enable apss int mask  */
    PCU_REG_APSS_PERI_IDLE_EN           = (0x00000601),  /*!<apss peri block idle enable */
    PCU_REG_APSS_PERI_IDLE              = (0x00000703),  /*!<apss peri idle flag,bit1  AP_DMA idle ,bit2  SDIO idle ,bit3 USB idle */
    /*** Bit definition for [regCpssConfig] register(0x00000004) ****/
    PCU_REG_CPSS_DS_MODE                = (0x00040001),  /*!<cpss deepsleep mode */
    PCU_REG_CPSS_CPU_IDLE_BYPASS        = (0x00040101),  /*!<cpss cpu idle bypass */
    PCU_REG_CPSS_PERI_IDLE_BYPASS       = (0x00040201),  /*!<cpss peri idle bypass */
    PCU_REG_CPSS_WORK_CLK_ENABLE        = (0x00040301),  /*!<cpss work clk gate enable */
    PCU_REG_CPSS_INT_MASK_CONFIG        = (0x00040401),  /*!<software config cpss int mask */
    PCU_REG_CPSS_INT_MASK_ENABLE        = (0x00040501),  /*!<software enablecpss int mask  */
    PCU_REG_CPSS_PERI_IDLE_EN           = (0x00040601),  /*!<cpss peri block idle enable */
    PCU_REG_CPSS_PERI_IDLE              = (0x00040703),  /*!<cpss peri idle flag,bit1  CP_DMA idle ,bit2  EDCP idle */
    /*** Bit definition for [regApssSleepTime] register(0x00000100) ****/
    PCU_REG_APSS_SLEEP_TIME             = (0x01000020),  /*!<apss sleep time */
    /*** Bit definition for [regCpssSleepTime] register(0x00000104) ****/
    PCU_REG_CPSS_SLEEP_TIME             = (0x01040020),  /*!<cpss sleep time */
    /*** Bit definition for [regSleepTimeLoad] register(0x0000010c) ****/
    PCU_APSS_SLEEP_TIME_LOAD_PULSE      = (0x010c0001),  /*!<apss sleep time load pulse */
    PCU_CPSS_SLEEP_TIME_LOAD_PULSE      = (0x010c0101),  /*!<cpss sleep time load pulse */
    /*** Bit definition for [regSleepEntryBypass] register(0x00000200) ****/
    PCU_REG_APSS_BYPASS                 = (0x02000001),  /*!<apss sleep bypass */
    PCU_REG_CPSS_BYPASS                 = (0x02000101),  /*!<cpss sleep bypass */
    PCU_REG_SLEEP_TIME_BYPASS           = (0x02000201),  /*!<sleep time bypass */
    /*** Bit definition for [regAonIntMaskCtrl] register(0x00000204) ****/
    PCU_REG_AON_INT_MASK_CONFIG         = (0x02040001),  /*!<software config int mask  */
    PCU_REG_AON_INT_MASK_ENABLE         = (0x02040101),  /*!<software endable int mask  */
    /*** Bit definition for [regDelayTime] register(0x00000208) ****/
    PCU_REG_COMMON_DELAY_TIME           = (0x02080008),  /*!<delay time for general purpose */
    PCU_REG_WFI_CLEAR_TIME              = (0x02080808),  /*!<delay time for idle clear (wfi) */
    PCU_REG_TPUEN_DELAY_TIME            = (0x02081010),  /*!<delay time from tpu clk off  to  modem clk off */
    /*** Bit definition for [regSleeptimeThreshold] register(0x0000020c) ****/
    PCU_REG_SLEEPTIME_THRESHOLD         = (0x020c0020),  /*!<chip sleep time threshold, */
    /*** Bit definition for [regMemDelayTime] register(0x00000210) ****/
    PCU_REG_MEM_PSDH_DELAY_TIME         = (0x02100008),  /*!<delay time for mem */
    PCU_REG_MEM_PSDL_DELAY_TIME         = (0x02100808),  /*!<delay time for mem */
    PCU_REG_MEM_LP2NORMAL_DELAY_TIME    = (0x02101008),  /*!<delay time for iram mem lp2normal */
    /*** Bit definition for [regMainClkConfig] register(0x00000218) ****/
    PCU_REG_SPLL0_OFF                   = (0x02180001),  /*!<SPLL OFF Control */
    PCU_REG_MAIN_CLK_26M_EN             = (0x02180101),  /*!<main clk 26m Control */
    /*** Bit definition for [regPcuAonClkEnable] register(0x0000021c) ****/
    PCU_SW_PCU2TPU_CLK_ENABLE           = (0x021c0001),  /*!<enable of sw_pcu2tpu_clk_en */
    PCU_REG_PCU_WCLK_REQEN              = (0x021c0101),  /*!<enable PCU_AON FSM auto clk gate request  */
    /*** Bit definition for [regPcuSwMuxConfig] register(0x00000300) ****/
    PCU_SW_PCU_APSS_GWCLK_EN            = (0x03000001),  /*!<sw config pcu_apss_gwclk_en          */
    PCU_REG_PCU_APSS_GWCLK_EN_SEL       = (0x03000101),  /*!<reg_pcu_apss_gwclk_en_sel : 1  sw ,0  hw */
    PCU_SW_PCU_CPSS_GWCLK_EN            = (0x03000201),  /*!<sw config pcu_cpss_gwclk_en          */
    PCU_REG_PCU_CPSS_GWCLK_EN_SEL       = (0x03000301),  /*!<reg_pcu_cpss_gwclk_en_sel  : 1  sw ,0  hw */
    PCU_SW_PDCORE_BUS_CLK_EN            = (0x03000401),  /*!<sw config pdcore_bus_clk_en          */
    PCU_REG_PDCORE_BUS_CLK_EN_SEL       = (0x03000501),  /*!<reg_pdcore_bus_clk_en_sel  : 1  sw ,0  hw   */
    PCU_SW_APSS_WORK_CLK_EN             = (0x03000601),  /*!<sw config apss_work_clk_en           */
    PCU_REG_APSS_CLK_EN_SEL             = (0x03000701),  /*!<reg_apss_clk_en_sel   : 1  sw ,0  hw         */
    PCU_SW_CPSS_WORK_CLK_EN             = (0x03000801),  /*!<sw config cpss_work_clk_en           */
    PCU_REG_CPSS_CLK_EN_SEL             = (0x03000901),  /*!<reg_cpss_clk_en_sel   : 1  sw ,0  hw  */
    PCU_SW_PCU2TPU_CLK_EN               = (0x03000a01),  /*!<sw config pcu2tpu_clk_en             */
    PCU_REG_PCU2TPU_CLK_EN_SEL          = (0x03000b01),  /*!<reg_pcu2tpu_clk_en_sel  : 1  sw ,0  hw  */
    PCU_SW_PCU_MODEM_CLK_EN             = (0x03000c01),  /*!<sw config pcu_modem_clk_en           */
    PCU_REG_PCU_MODEM_CLK_EN_SEL        = (0x03000d01),  /*!<reg_pcu_modem_clk_en_sel    : 1  sw ,0  hw   */
    PCU_SW_PCU_PSRAM_CLK_EN             = (0x03000e01),  /*!<sw config pcu_psram_clk_en           */
    PCU_REG_PCU_PSRAM_CLK_EN_SEL        = (0x03000f01),  /*!<reg_pcu_psram_clk_en_sel   : 1  sw ,0  hw    */
    PCU_SW_PCU_SPLL0_PD                 = (0x03001001),  /*!<sw config pcu_spll0_pd               */
    PCU_REG_PCU_SPLL0_PD_SEL            = (0x03001101),  /*!<reg_pcu_spll0_pd_sel     : 1  sw ,0  hw      */
    PCU_SW_PCU_ABBPLL_PD                = (0x03001201),  /*!<sw config pcu_abbpll_pd              */
    PCU_REG_PCU_ABBPLL_PD_SEL           = (0x03001301),  /*!<reg_pcu_abbpll_pd_sel   : 1  sw ,0  hw       */
    /*** Bit definition for [regShramMuxConfig] register(0x00000304) ****/
    PCU_SW_PDAON_SHRAM_PGEN             = (0x03040008),  /*!<sw config pdaon_shram_pgen           */
    PCU_SW_PDAON_SHRAM_RET2N            = (0x03040808),  /*!<sw config pdaon_shram_ret2n          */
    PCU_SW_PDAON_SHRAM_RET1N            = (0x03041008),  /*!<sw config pdaon_shram_ret1n          */
    PCU_REG_PDAON_SHRAM_CTRL_SEL        = (0x03041801),  /*!<reg_pdaon_shram_ctrl_sel   : 1  sw ,0  hw      */
    /*** Bit definition for [regPvsenseRetoff] register(0x00000400) ****/
    PCU_REG_PVSENS_RETOFF_PD0           = (0x04000001),  /*!<pd0 retention off control，1：not retention，0： retention */
    PCU_REG_PVSENS_RETOFF_PD1           = (0x04000101),  /*!<pd1 retention off control，1：not retention，0： retention */
    PCU_REG_PVSENS_RETOFF_PD2           = (0x04000201),  /*!<pd2 retention off control，1：not retention，0： retention */
    PCU_REG_PVSENS_RETOFF_PD3           = (0x04000301),  /*!<pd3 retention off control，1：not retention，0： retention */
    PCU_REG_PVSENS_RETOFF_PD4           = (0x04000401),  /*!<pd4 retention off control，1：not retention，0： retention */
    /*** Bit definition for [regWakeupIntClr] register(0x00000500) ****/
    PCU_AON_REG_WAKEUP_INT_CLR          = (0x05000020),  /*!<interrupt clear */
    /*** Bit definition for [regWakeupIntTyp] register(0x00000504) ****/
    PCU_AON_REG_WAKEUP_INT_TYP          = (0x05040020),  /*!<interrupt type,1:level; 0:edge */
    /*** Bit definition for [regWakeupIntPol] register(0x00000508) ****/
    PCU_AON_REG_WAKEUP_INT_POL          = (0x05080020),  /*!<interrupt polarity,1: High/posedge; 0:Low/negedge */
    /*** Bit definition for [regWakeupIntDisSby] register(0x0000050c) ****/
    PCU_AON_REG_WAKEUP_INT_DIS_SBY      = (0x050c0020),  /*!<disable wakeup sby */
    /*** Bit definition for [regWakeupIntDisAon] register(0x00000510) ****/
    PCU_AON_REG_WAKEUP_INT_DIS_AON      = (0x05100020),  /*!<disable wakeup aon */
    /*** Bit definition for [regWakeupIntDisApss] register(0x00000514) ****/
    PCU_AON_REG_WAKEUP_INT_DIS_APSS     = (0x05140020),  /*!<disable wakeup apss */
    /*** Bit definition for [regWakeupIntDisCpss] register(0x00000518) ****/
    PCU_AON_REG_WAKEUP_INT_DIS_CPSS     = (0x05180020),  /*!<disable wakeup cpss */

    /*** Bit definition for [regWakeupIntClr1] register(0x00000520) ****/
    PCU_AON_REG_WAKEUP_INT_CLR1         = (0x05200009),  /*!<interrupt clear */
    /*** Bit definition for [regWakeupIntTyp1] register(0x00000524) ****/
    PCU_AON_REG_WAKEUP_INT_TYP1         = (0x05240009),  /*!<interrupt type,1:level; 0:edge */
    /*** Bit definition for [regWakeupIntPol1] register(0x00000528) ****/
    PCU_AON_REG_WAKEUP_INT_POL1         = (0x05280009),  /*!<interrupt polarity,1: High/posedge; 0:Low/negedge */
    /*** Bit definition for [regWakeupIntDisSby1] register(0x0000052c) ****/
    PCU_AON_REG_WAKEUP_INT_DIS_SBY1     = (0x052c0009),  /*!<disable wakeup sby */
    /*** Bit definition for [regWakeupIntDisAon1] register(0x00000530) ****/
    PCU_AON_REG_WAKEUP_INT_DIS_AON1     = (0x05300009),  /*!<disable wakeup aon */
    /*** Bit definition for [regWakeupIntDisApss1] register(0x00000534) ****/
    PCU_AON_REG_WAKEUP_INT_DIS_APSS1    = (0x05340009),  /*!<disable wakeup apss */
    /*** Bit definition for [regWakeupIntDisCpss1] register(0x00000538) ****/
    PCU_AON_REG_WAKEUP_INT_DIS_CPSS1    = (0x05380009),  /*!<disable wakeup cpss */
    /*** Bit definition for [regAonectintSyncEn] register(0x00000540) ****/
    PCU_AON_REG_AONEXTINT_SYNC_EN       = (0x05400009),  /*!<AON ext int sync enable */
    /*** Bit definition for [regAonWakeupIntFlag] register(0x00000544) ****/
    PCU_AON_REG_WKUP_INT_FLAG           = (0x05440020),  /*!<aon wakeup int flag */
    PCU_AON_REG_WKUP_INT_FLAG1          = (0x054c0009),  /*!<aon wakeup int flag */
    /*** Bit definition for [regCalibrationConfig] register(0x00000600) ****/
    PCU_CALIBRATION_DELAY_REQ           = (0x06000001),  /*!<calibration delay request */
    PCU_CALIBRATION_LOAD_EN             = (0x06000101),  /*!<calibration delay time load enable,edge-triggered */
    PCU_CALIBRATION_START               = (0x06000201),  /*!<calibration delay start,edge-triggered */
    /*** Bit definition for [regCalibrationDelyTime] register(0x00000604) ****/
    PCU_CALIBRATION_DELAY_TIME          = (0x06040019),  /*!<calibration delay time */
    /*** Bit definition for [regAonTestpins] register(0x00000700) ****/
    PCU_AON_TSTPINS                     = (0x07000020),  /*!<pcu_aon  debug testpins */
    /*** Bit definition for [regSsTestpins] register(0x00000704) ****/
    PCU_APSS_TESTPINS                   = (0x07040010),  /*!<APSS debug testpins */
    PCU_CPSS_TESTPINS                   = (0x07041010),  /*!<CPSS debug testpins */
    /*** Bit definition for [regTestpinSel] register(0x00000708) ****/
    PCU_REG_TESTPINS_SEL                = (0x07080004),  /*!<aon  debug testpins sel */

} PCU_RegGenPcuAon;


typedef enum
{
    /*** Bit definition for [regPorResetCtrl] register(0x00000000) ****/
    PCU_REG_PS_HOLD                     = (0x00000001),  /*!<power supply hold register */
    PCU_REG_EXTRST_MODE                 = (0x00000101),  /*!<external reset mode：0,cold reset mode；1, hot reset mode */
    PCU_REG_LDO1OK_BYPASS               = (0x00000201),  /*!<ldo1ok bypass */
    PCU_REG_POR5_RSTN_FLAG              = (0x00000501),  /*!<reg_por5_rstn_flag clr,write 0 clear */
    PCU_REG_EXT_RSTN_FLAG               = (0x00000601),  /*!<reg_ext_rstn_flag */
    PCU_REG_EXT_RSTN_FLAG_CLR           = (0x00000701),  /*!<reg_ext_rstn_flag clear,write 1  opration clear */
    PCU_REG_KEYON                       = (0x00000a01),  /*!<keyon pad status */
    PCU_REG_WAKEUP                      = (0x00000b01),  /*!<wakeup pad status */
    PCU_REG_WAKEUP_BYPASS               = (0x00000c01),  /*!<bypass wakeup pad charging and turning on  */

    /*** Bit definition for [regShadow0] register(0x00000008) ****/
    PCU_REG_SHADOW0                     = (0x00080020),  /*!<software bakeup register 0 */
    PCU_REG_FASTBOOT                    = (0x00080001),  /*!<software fast boot flag*/
    PCU_REG_PSMLOG                      = (0x00080101),  /*!<software psm log flag*/
    PCU_REG_PWR_RESET                   = (0x00080206),  /*!<software reset flag */
    PCU_REG_PWR_BOOT                    = (0x00080804),  /*!<software bakeup boot mode */
    PCU_REG_PWRDOWN_SBY                 = (0x00080C01),  /*!<software power down to sby flag */
    PCU_REG_UNUSED                      = (0x00080D01),  /*!<unuesd flag */
    PCU_REG_PSRAMINFO                   = (0x00080E01),  /*!<software psram info flag*/
    PCU_REG_TO_MBL                      = (0x00080F01),  /*!<software boot to mbl flag*/
    PCU_REG_PWR_AMT                     = (0x00081010),  /*!<software amt reset */
    /*** Bit definition for [regShadow1] register(0x0000000c) ****/
    PCU_REG_SHADOW1                     = (0x000c0020),  /*!<software bakeup register 1 */
    /*** Bit definition for [regShadow2] register(0x00000010) ****/
    PCU_REG_SHADOW2                     = (0x00100020),  /*!<software bakeup register 2 */
    /*** Bit definition for [regShadow4] register(0x00000014) ****/
    PCU_REG_SHADOW4                     = (0x00140020),  /*!<software bakeup register 4 */
    /*** Bit definition for [regShadow5] register(0x00000018) ****/
    PCU_REG_SHADOW5                     = (0x00180020),  /*!<software bakeup register 5 */

    /*** Bit definition for [regPmuSbyCtrl] register(0x00000020) ****/
    PCU_REG_CHIP_SBY_MODE               = (0x00200001),  /*!<Standby sleep mode */
    PCU_REG_CHIP_SDN_MODE               = (0x00200101),  /*!<Shutdown sleep mode */
    PCU_REG_AON_STATUS_BYPASS           = (0x00200201),  /*!<Bypass PCU_SBY休眠入口条件：PCU_AON休眠状态 */
    PCU_REG_PMU_CTRL_FSM_REQEN          = (0x00200301),  /*!<pmu_ctrl_sby模块自动门控请求 */
    PCU_REG_LDO_STABLE_TIME             = (0x00200404),  /*!<LDO stable time,default 1ms，unit:244us */
    PCU_REG_LP_SHUTDOWN_EN              = (0x00200801),  /*!<long press shutdown enable */
    PCU_REG_LLP_SHUTDOWN_EN             = (0x00200901),  /*!<long long press shutdown enable */
    PCU_REG_LLP_RESTART_EN              = (0x00200a01),  /*!<long long press  restart enable */
    PCU_REG_RESTART_DELAY               = (0x00200b04),  /*!<restart_delay time ,default 125ms,unit:125ms */
    PCU_SBY_WAKEUP_INT_FLAG             = (0x00201006),  /*!<sby唤醒中断状态{sby_ext_int_s2 , rtc_int,  sp_int,  lp_int , llp_int ,sby_ext_int_s2} */
    PCU_REG_RTC_FREQ_DIV_SEL            = (0x00201601),  /*!<RTC时钟分频选择；0,32768分频，1,33419分频 */
    PCU_REG_DCDC_STABLE_TIME            = (0x00201706),  /*!<DCDC stable time,default 29ms，unit:1.92ms */
    PCU_REG_DCDC_OK_BYPASS              = (0x00201d01),  /*!<DCDC power on OK bypass */

    /*** Bit definition for [regKeyDetection] register(0x00000030) ****/
    PCU_REG_SP_TIME                     = (0x00300004),  /*!<SP time,default 125ms,unit:125ms */
    PCU_REG_LP_TIME                     = (0x00300404),  /*!<LP time,default 3s,unit:1s */
    PCU_REG_LLP_TIME                    = (0x00300804),  /*!<LLP time,default 10s,unit:1s按键到LLP time触发中断 */
    PCU_REG_KEY_DET_CLKGATE_EN          = (0x00300c01),  /*!<key_detection clk gate enable */

    /*** Bit definition for [regSbyIntCtrl] register(0x00000040) ****/
    PCU_REG_WAKEUP_INT_CLR              = (0x00400105),  /*!<PCU SBY唤醒中断清除信号 */
    PCU_REG_WAKEUP_INT_TYP              = (0x00400605),  /*!<唤醒中断wakeup类型,1:level; 0:edge */
    PCU_REG_WAKEUP_INT_POL              = (0x00400b05),  /*!<唤醒中断极性,1: High/posedge; 0:Low/negedge */
    PCU_REG_WAKEUP_INT_DIS_SBY          = (0x00401005),  /*!<sby唤醒中断disable ，1：disable，0：enable */
    PCU_REG_WAKEUP_INT_DIS_AON          = (0x00401505),  /*!<aon唤醒中断disable，1：disable，0：enable */
    PCU_REG_WAKEUP_INT_DIS_APSS         = (0x00401a05),  /*!<apss唤醒中断disable，1：disable，0：enable */

    /*** Bit definition for [regSbyIntCtrl2] register(0x00000044) ****/
    PCU_REG_WAKEUP_INT_DIS_CPSS         = (0x00440005),  /*!<cpss唤醒中断disable，1：disable，0：enable */
    PCU_REG_EXTINT_CFG                  = (0x00440a07),  /*!<外部中断配置(bit0:clr,bit1:typ,bit2:pol,bit3:dis_cpss,bit4:dis_apss,bit5:dis_aon,bit6:dis_sby) */
    PCU_REG_APSLV_INT_MASK              = (0x00441e01),  /*!<ap从控中断mask寄存器 */
    PCU_REG_CPSLV_INT_MASK              = (0x00441f01),  /*!<cp从控中断mask寄存器 */

    /*** Bit definition for [rc32kCfg] register(0x00000050) ****/
    PCU_REG_RC_32K_FREQ_FTUNE_LV        = (0x00500004),  /*!<32K RC精调软件校准值 */
    PCU_REG_RC_32K_FREQ_CTUNE_LV        = (0x00500406),  /*!<32K RC粗调软件校准值 */
    PCU_REG_RC_32K_FREQ_TUNE_SEL        = (0x00500a01),  /*!<32K RC校准寄存器选择，1 软件校准值，0 硬件校准值 */
    PCU_REG_RC_32K_EN                   = (0x00500b01),  /*!<32K RC 软件使能 */
    PCU_DIG_RC_32K_FREQ_FTUNE_LV        = (0x00500c04),  /*!<32K RC硬件校准精调校准值 */
    PCU_DIG_RC_32K_FREQ_CTUNE_LV        = (0x00501006),  /*!<32K RC硬件校准粗调校准值 */

    /*** Bit definition for [rc32kTrimCfg] register(0x00000054) ****/
    PCU_REG_RC32K_HW_TRIM_EN            = (0x00540001),  /*!<Standby模式下，触发32K RC 硬件trim的使能 */
    PCU_REG_PERIODIC_TRIM_SEL           = (0x00540102),  /*!<Standby/DS模式下，32K RC 硬件trim时间间隔选择：0、1.067min；1、4.267min；2，17.067min；3、67.267min */
    PCU_REG_RC_32K_TRIM_ERROR_NUM       = (0x00540308),  /*!<32K RC校准寄存器 */
    PCU_REG_RC_32K_CNT_CYCLE_NUM        = (0x00540b08),  /*!<32K RC校准寄存器 */
    /*** Bit definition for [rc32kTrimCfg2] register(0x00000058) ****/
    PCU_REG_RC_32K_REF_COUNTER_NUM      = (0x00580014),  /*!<RC 32K 校准寄存器 *

    /*** Bit definition for [regShadow3] register(0x00000060) ****/
    PCU_REG_SHADOW3                     = (0x00600020),  /*!<software bakeup register 3 */

    /*** Bit definition for [sbyClkRstPara] register(0x00000300) ****/
    PCU_PMU_CTRL_WCLK_EN                = (0x03000001),  /*!<pmu_ctrl wclk enable，1：enable，0：disable */
    PCU_LP_RTC_CLK_32K_SW_EN            = (0x03000101),  /*!<rtc时钟软门控使能 */
    PCU_LP_RTC_SW_RESETN                = (0x03000201),  /*!<rtc软复位 */
    PCU_RTC_CLK_REQEN                   = (0x03000301),  /*!<rtc clk request enable，1：enable，0：disable */
    PCU_SBY_32K_CLK_SEL                 = (0x03000401),  /*!<sby_32k_clk mux sel，bit0 ：0,rc 32k;1,aon_32k_clk */
} PCU_LbSbyLspReg;


/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           注册唤醒中断
 *
 * @param[in]       irqid               中断id
 * @param[in]       type                中断触发类型
 *
 * @return          void
 ************************************************************************************
*/
void PCU_WakeupIrqRegister(int irqid, PCU_IrqType type);

/**
 ************************************************************************************
 * @brief           注销唤醒中断
 *
 * @param[in]       irqid               中断id
 * @param[in]       flag                中断域
 *
 * @return          void
 ************************************************************************************
*/
void PCU_WakeupIrqUnregister(int irqid);

/**
 ************************************************************************************
 * @brief           清唤醒标志位
 *
 * @param[in]       irqid               中断id
 * @param[in]       flag                中断域
 *
 * @retval          DRV_OK              清唤醒标志位成功
 *                  DRV_ERR_PARAMETER   参数异常
 ************************************************************************************
*/
void PCU_WakeupIrqClrpending(int irqid);


/**
 ************************************************************************************
 * @brief           清所有唤醒中断标志
 *
 * @param[in]       void
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PCU_WakeupIrqClrAll(void);

/**
 ************************************************************************************
 * @brief           设置PCU硬件低功耗模式
 *
 * @param[in]       mode                PCU硬件低功耗模式
 * @param[in]       en                  PCU硬件低功耗模式
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_ChipModeCfg(PCU_ChipMode mode, bool_t en);

/**
 ************************************************************************************
 * @brief           设置子系统低功耗模式
 *
 * @param[in]       en                  使能/清除
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_SetSubSysSleepMode(bool_t en);

/**
 ************************************************************************************
 * @brief           Shram Retention 功能配置，RET2 模式
 *
 * @param[in]       void
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_ShramRetentionSet(void);

/**
 ************************************************************************************
 * @brief           Shram Retention 功能配置，使能模式
 *
 * @param[in]       void
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_ShramRetentionUnSet(void);

/**
 ************************************************************************************
 * @brief           PCU SBY接口
 *
 * @param[in]       void
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PCU_SbyEnter(void);

/**
 ************************************************************************************
 * @brief           PCU关机接口
 *
 * @param[in]       void
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PCU_ShutDown(void);

/**
 ************************************************************************************
 * @brief           配置PCU AON寄存器等
 *
 * @param[in]       regBit      地址偏移/寄存器BIT偏移/位宽
 * @param[in]       bitVal      配置值
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_SetRegGenPcuAon(PCU_RegGenPcuAon regBit, uint32_t bitVal);

/**
 ************************************************************************************
 * @brief           获取PCU AON寄存器配置
 *
 * @param[in]       regBit      地址偏移/寄存器BIT偏移/位宽
 *
 * @return          uint32_t    配置值
 * @retval
 ************************************************************************************
*/
uint32_t PCU_GetRegGenPcuAon(PCU_RegGenPcuAon regBit);

/**
 ************************************************************************************
 * @brief           配置PCU SBY寄存器等
 *
 * @param[in]       regBit      地址偏移/寄存器BIT偏移/位宽
 * @param[in]       bitVal      配置值
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_SetRegGenPcuSby(PCU_LbSbyLspReg regBit, uint32_t bitVal);

/**
 ************************************************************************************
 * @brief           获取PCU SBY寄存器配置
 *
 * @param[in]       regBit      地址偏移/寄存器BIT偏移/位宽
 *
 * @return          uint32_t    配置值
 * @retval
 ************************************************************************************
*/
uint32_t PCU_GetRegGenPcuSby(PCU_LbSbyLspReg regBit);

/**
 ************************************************************************************
 * @brief           配置ABB PLL
 *
 * @param[in]       en   1:打开; 0:关闭
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_AbbPllCfg(uint8_t en);

/**
 ************************************************************************************
 * @brief           DCXO 32K less clk 开关接口
 * @param[in]       en：开关配置
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_32klessEn(uint8_t en);

/**
 ************************************************************************************
 * @brief           Deepsleep 32KRC Trim 开关
 * @param[in]       en：开关配置
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_32kTrimEn(uint8_t en);

/**
 ************************************************************************************
 * @brief           休眠时26M CLK 开关接口
 * @param[in]       en：开关配置
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_26MClkEn(uint8_t en);

/**
 ************************************************************************************
 * @brief           深睡唤醒时钟配置接口
 * @param[in]       en：开关配置
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_Deepsleep_ClkEn(uint8_t en);


/**
 ************************************************************************************
 * @brief           aon32k时钟从rc切换到dcxo
 *
 * @param[in]       void
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PSM_RcSwitchToDcxo(void);
#if defined(_CPU_CP)
/**
 ************************************************************************************
 * @brief           设置LPM延时校准标志
 *
 * @param[in]       flag                LPM延时校准标志
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PCU_SetDelaycaliFlag(int flag);

/**
 ************************************************************************************
 * @brief           获取LPM延时校准标志
 *
 * @param[in]       void
 *
 * @return          int
 * @retval                              LPM延时校准标志
 ************************************************************************************
*/
int PCU_GetDelaycaliFlag(void);

/**
 ************************************************************************************
 * @brief           设置LPM延时校准时长
 *
 * @param[in]       time                LPM延时校准时长, time*30us
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_SetDelaycaliTime(uint32_t time);

/**
 ************************************************************************************
 * @brief           获取LPM延时校准时长
 *
 * @param[in]       void
 *
 * @return          uint32_t
 * @retval                              LPM延时校准时长, time*30us
 ************************************************************************************
*/
uint32_t PCU_GetDelaycaliTime(void);

/**
 ************************************************************************************
 * @brief           休眠前关闭tpu
 *
 * @param[in]       softctrl    1 软件控制
 *                              0 硬件控制
 *
 * @return          void
 ************************************************************************************
*/
void PCU_TpuDisable(uint8_t softctrl);

/**
 ************************************************************************************
 * @brief           唤醒后打开tpu
 *
 * @param[in]       softctrl    1 软件控制
 *                              0 硬件控制
 *
 * @return          void
 ************************************************************************************
*/
void PCU_TpuEnable(uint8_t softctrl);

#endif

#ifdef PSM_SUPPORT_ALL_FUNC
/**
 ************************************************************************************
 * @brief           配置延时时长
 *
 * @param[in]       id         pcu 延时功能id
 * @param[in]       ns         ns值
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PCU_AonDelaytimeCfg(PCU_AonDelaytimeId id, int ms);

/**
 ************************************************************************************
 * @brief           配置pcu aon支持的功能，26M时钟，TPU时钟等
 *
 * @param[in]       PCU_AonFuncId           功能ID
 * @param[in]       en                      使能/禁用
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PCU_AonFuncCfg(PCU_AonFuncId id, bool_t en);

#endif

/**
 ************************************************************************************
 * @brief           配置IO保持功能
 *
 * @param[in]       PCU_PdId                IO ID
 * @param[in]       en                      0:使能/1:禁用
 *
 * @return          void
 * @retval          null
 ************************************************************************************
*/
void PCU_IORetentionCfg(PCU_PdId id, bool_t en);

#if defined(_CPU_AP) && defined(OS_USING_PM)

/**
 ************************************************************************************
 * @brief           获取standby唤醒中断id
 *
 * @param[in]       void
 *
 * @return          PCU_SbyEventID    standby中断事件id
 * @retval
 ************************************************************************************
*/
PCU_SbyEventID PCU_GetSbyWkupIntID(void);

/**
 ************************************************************************************
 * @brief           注册sby 中断唤醒回调
 *
 * @param[in]       id  唤醒事件id
 *                  fp  唤醒回调
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_SbyCallbackRegister(PCU_SbyEventID id, PCU_SbyCbFp fp);

/**
 ************************************************************************************
 * @brief           调用sby 中断唤醒回调
 *
 * @param[in]       id  唤醒事件id
 *                  fp  唤醒回调
 *
 * @return          uint8_t    standby中断id
 * @retval
 ************************************************************************************
*/
void PCU_SbyCallbackCall(void);

#endif

/**
 ************************************************************************************
 * @brief           配置启动时的默认控制台
 *
 * @param[in]       flag    0:AP, 1:CP
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void PCU_SetConsole(uint8_t flag);

/**
 ************************************************************************************
 * @brief           获取启动时的默认控制台
 *
 * @param[in]       void
 *
 * @return          0:AP, 1:CP
 * @retval
 ************************************************************************************
*/
uint8_t PCU_GetConsole(void);

#endif
/** @} */