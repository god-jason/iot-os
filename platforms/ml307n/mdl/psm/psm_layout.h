/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        psm_layout.h
 *
 * @brief       省电共享内存布局
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team          创建
 ************************************************************************************
 */

#ifndef __PSM_LAYOUT__
#define __PSM_LAYOUT__

#include <top_ram_config.h>

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define PSM_IRAM_CACHE_TO_NONCACHE(addr)        ((addr) - IRAM_CACHE_TO_NONCACHE_OFFSET)
#define PSM_IRAM_NONCACHE_TO_CACHE(addr)        ((addr) + IRAM_CACHE_TO_NONCACHE_OFFSET)
#define PSM_PSRAM_CACHE_TO_NONCACHE(addr)       ((addr) - DDR_CACHE_TO_NONCACHE_OFFSET)
#define PSM_PSRAM_NONCACHE_TO_CACHE(addr)       ((addr) + DDR_CACHE_TO_NONCACHE_OFFSET)

#define PSM_DEBUG

//CODE/DATA
#define PSM_SHARM_AP_ADDR                       PSM_IRAM_CACHE_TO_NONCACHE(IRAM_BASE_ADDR_AP_PSM)
#define PSM_SHARM_AP_SIZE                       IRAM_BASE_LEN_AP_PSM

#if (PSM_SHARM_AP_ADDR != 0x10200800)
#error "PSM_SHARM_AP_ADDR != 0x10200800, must sync with riscv_psm_asm.S"
#endif

#define PSM_SHARM_CP_ADDR                       PSM_IRAM_CACHE_TO_NONCACHE(IRAM_BASE_ADDR_CP_PSM)
#define PSM_SHARM_CP_SIZE                       IRAM_BASE_LEN_CP_PSM

//SHARE MEM
#define PSM_SHARM_ADDR_BASE                     PSM_IRAM_CACHE_TO_NONCACHE(IRAM_BASE_ADDR_PSM_SHARE)
#define PSM_SHARM_INFO_LEN                      (256)

//PSM SYS CMN DEV SAVE ADDR
#define PSM_CMNDEV_SAVE_ADDR                    DRAM_BASE_ADDR_AP_MODEM_SHARE_NC
#define PSM_CMNDEV_SAVESIZE                     (1024)
#define PSM_COREMEM_SAVESIZE                    (1024)

//AP CORE MEM SAVE ADDR
#define PSM_APCOREMEM_SAVEADDR                  (PSM_CMNDEV_SAVE_ADDR + PSM_CMNDEV_SAVESIZE)

//AP TCM SAVE ADDR
#define PSM_AP_ITCM_SAVE_ADDR                   (PSM_APCOREMEM_SAVEADDR + PSM_COREMEM_SAVESIZE)
#define PSM_AP_TCM_SIZE                         (16384)
//#define PSM_AP_DTCM_SAVE_ADDR                   (PSM_AP_ITCM_SAVE_ADDR + PSM_AP_TCM_SIZE)

//AP IRAM SAVE ADDR
//#define PSM_AP_IRAM_SAVE_ADDR                   (PSM_AP_DTCM_SAVE_ADDR + PSM_AP_TCM_SIZE)

//CP CORE MEM SAVE ADDR
#define PSM_CPCOREMEM_SAVEADDR                  (DRAM_BASE_ADDR_CP_MODEM_SHARE_NC)
//CP TCM SAVE ADDR
#define PSM_CP_ITCM_SAVE_ADDR                   (PSM_CPCOREMEM_SAVEADDR + PSM_COREMEM_SAVESIZE)
#define PSM_CP_TCM_SIZE                         (32768)
//#define PSM_CP_DTCM_SAVE_ADDR                   (PSM_CP_ITCM_SAVE_ADDR + PSM_CP_TCM_SIZE)

//CP IRAM SAVE ADDR
//#define PSM_CP_IRAM_SAVE_ADDR                   (PSM_CP_DTCM_SAVE_ADDR + PSM_CP_TCM_SIZE)

#define SHMEM_APSS_ADDR                         (PSM_SHARM_AP_ADDR + PSM_SHARM_AP_SIZE - 0x100)
#define SHMEM_CPSS_ADDR                         (PSM_SHARM_CP_ADDR + PSM_SHARM_CP_SIZE - 0x100)

#if defined(_CPU_AP)
#define PSM_APCORE                              (0x1234)
#define PSM_WAKECORE                            PSM_APCORE
#else
#define PSM_CPCORE                              (0x4321)
#define PSM_WAKECORE                            PSM_CPCORE
#endif

// IRAM 共享内存分配
#if defined(_CPU_AP)
#define PSM_REGSUSPEND_ADDR                     (PSM_SHARM_ADDR_BASE+0x0)
#define PSM_PSRAMSUSPEND_ADDR                   (PSM_SHARM_ADDR_BASE+0x04)
#define PSM_PLLSUSPEND_ADDR                     (PSM_SHARM_ADDR_BASE+0x08)
#define PSM_REGRESUME_ADDR                      (PSM_SHARM_ADDR_BASE+0x0C)
#define PSM_PSRAMRESUME_ADDR                    (PSM_SHARM_ADDR_BASE+0x10)
#define PSM_PLLRESUME_ADDR                      (PSM_SHARM_ADDR_BASE+0x14)

#define PSM_CPSS_REGSUSPEND_ADDR                (PSM_SHARM_ADDR_BASE+0x18)
#define PSM_CPSS_PSRAMSUSPEND_ADDR              (PSM_SHARM_ADDR_BASE+0x1C)
#define PSM_CPSS_PLLSUSPEND_ADDR                (PSM_SHARM_ADDR_BASE+0x20)
#define PSM_CPSS_REGRESUME_ADDR                 (PSM_SHARM_ADDR_BASE+0x24)
#define PSM_CPSS_PSRAMRESUME_ADDR               (PSM_SHARM_ADDR_BASE+0x28)
#define PSM_CPSS_PLLRESUME_ADDR                 (PSM_SHARM_ADDR_BASE+0x2C)
#define PSM_PSRAM_ENTERHALFSLEEP_ADDR           (PSM_SHARM_ADDR_BASE+0x38)
#define PSM_PSRAM_EXITHALFSLEEP_ADDR            (PSM_SHARM_ADDR_BASE+0x3C)
#else
#define PSM_REGSUSPEND_ADDR                     (PSM_SHARM_ADDR_BASE+0x18)
#define PSM_PSRAMSUSPEND_ADDR                   (PSM_SHARM_ADDR_BASE+0x1C)
#define PSM_PLLSUSPEND_ADDR                     (PSM_SHARM_ADDR_BASE+0x20)
#define PSM_REGRESUME_ADDR                      (PSM_SHARM_ADDR_BASE+0x24)
#define PSM_PSRAMRESUME_ADDR                    (PSM_SHARM_ADDR_BASE+0x28)
#define PSM_PLLRESUME_ADDR                      (PSM_SHARM_ADDR_BASE+0x2C)
#define PSM_PSRAM_ENTERHALFSLEEP_ADDR           (PSM_SHARM_ADDR_BASE+0x38)
#define PSM_PSRAM_EXITHALFSLEEP_ADDR            (PSM_SHARM_ADDR_BASE+0x3C)
#endif

#define PSM_APSLEEP_SFN_ADDR                    (PSM_SHARM_ADDR_BASE+0x30)
#define PSM_APSLEEP_SFN                         (*(volatile uint32_t *)PSM_APSLEEP_SFN_ADDR)

#define PSM_APSLEEP_MRTR_ADDR                   (PSM_SHARM_ADDR_BASE+0x34)
#define PSM_APSLEEP_MRTR                        (*(volatile uint32_t *)PSM_APSLEEP_MRTR_ADDR)

#define CPSS_PSMBOOT_ADDR                       (PSM_SHARM_ADDR_BASE+0x40)
#define PSM_IDLESTATE_ADDR                      (PSM_SHARM_ADDR_BASE+0x44)
#define PSM_IDLESTATE                           (*(volatile uint32_t *)PSM_IDLESTATE_ADDR)
#define PSM_DS_CFG_ADDR                         (PSM_SHARM_ADDR_BASE+0x48)
#define PSM_DS_CFG                              (*(volatile uint32_t *)PSM_DS_CFG_ADDR)

#ifdef PSM_DEBUG
#define PSM_LOCK_ADDR                           (PSM_SHARM_ADDR_BASE+0x54)
#define PSM_LOCK                                (*(volatile uint32_t *)PSM_LOCK_ADDR)
#define PSM_SLEEP_CORE_ADDR                     (PSM_SHARM_ADDR_BASE+0x5C)
#define PSM_SLEEP_CORE                          (*(volatile uint32_t *)PSM_SLEEP_CORE_ADDR)
#endif

#define PSM_WAKEUP_CORE_ADDR                    (PSM_SHARM_ADDR_BASE+0x58)
#define PSM_WAKEUP_CORE                         (*(volatile uint32_t *)PSM_WAKEUP_CORE_ADDR)
#define PSM_SET_WAKEUP_CORE                     (PSM_WAKEUP_CORE = PSM_WAKECORE)
#define PSM_CLR_WAKEUP_CORE                     (PSM_WAKEUP_CORE = 0)
#define PSM_IS_WAKEUP_CORE                      (PSM_WAKEUP_CORE == PSM_WAKECORE)

#if defined (PSM_USE_UART)
//#define PSM_CPUART_SAVE0                        (PSM_SHARM_ADDR_BASE+0x60)
#define PSM_CPUART_SAVE1                        (PSM_SHARM_ADDR_BASE+0x64)
//#define PSM_CPUART_SAVE2                        (PSM_SHARM_ADDR_BASE+0x68)
#define PSM_CPUART_SAVE3                        (PSM_SHARM_ADDR_BASE+0x6C)
#define PSM_CPUART_SAVE4                        (PSM_SHARM_ADDR_BASE+0x70)
#define PSM_CPUART_SAVE5                        (PSM_SHARM_ADDR_BASE+0x74)
#define PSM_CPUART_SAVE6                        (PSM_SHARM_ADDR_BASE+0x78)
#endif

#define PSM_STANDBY_SLEEPTIME_THREHOLD_ADDR     (PSM_SHARM_ADDR_BASE+0x60)
#define PSM_STANDBY_SLEEPTIME_THREHOLD          (*(volatile uint32_t *)PSM_STANDBY_SLEEPTIME_THREHOLD_ADDR)

#define PSM_STANDBY_RTC_WUTIME_ADDR             (PSM_SHARM_ADDR_BASE+0x68)
#define PSM_STANDBY_RTC_WUTIME                  (*(volatile uint32_t *)PSM_STANDBY_RTC_WUTIME_ADDR)

//CP 工作任务标志，置位后AP无法进入standby。CP清0后，需要主动唤醒AP，AP重新检查休眠。
#define PSM_CP_WORKTASK_FLAG_ADDR               (PSM_SHARM_ADDR_BASE+0x7C)
#define PSM_CP_WORKTASK_FLAG                    (*(volatile uint32_t *)PSM_CP_WORKTASK_FLAG_ADDR)

#define PSM_LOG_UART_CLK_ADDR                   (PSM_SHARM_ADDR_BASE+0x80)
#define PSM_LOG_UART_CLK                        (*(volatile uint32_t *)PSM_LOG_UART_CLK_ADDR)

#define PSM_DSOPTIMIZE_ADDR                     (PSM_SHARM_ADDR_BASE+0x84)
#define PSM_DSOPTIMIZE                          (*(volatile uint8_t *)PSM_DSOPTIMIZE_ADDR)

#define PSM_DS26MDCXO_ADDR                      (PSM_SHARM_ADDR_BASE+0x85)
#define PSM_DS26MDCXO                           (*(volatile uint8_t *)PSM_DS26MDCXO_ADDR)

#define PSM_DIAG_ADDR                           (PSM_SHARM_ADDR_BASE+0x86)
#define PSM_DIAG                                (*(volatile uint8_t *)PSM_DIAG_ADDR)

#define PSM_DRX_CYCLE_ADDR                      (PSM_SHARM_ADDR_BASE+0x88)
#define PSM_DRX_CYCLE                           (*(volatile uint32_t *)PSM_DRX_CYCLE_ADDR)

#define PSM_SPLL0_POSTDIV1_ADDR                 (PSM_SHARM_ADDR_BASE+0x8C)
#define PSM_SPLL0_POSTDIV1                      (*(volatile uint32_t *)PSM_SPLL0_POSTDIV1_ADDR)

#define PSM_SW_CLK_SEL_ADDR                     (PSM_SHARM_ADDR_BASE+0x90)
#define PSM_SW_CLK_SEL                          (*(volatile uint32_t *)PSM_SW_CLK_SEL_ADDR)

#define PSM_SW_ACS_ACG0_ADDR                    (PSM_SHARM_ADDR_BASE+0xF0)
#define PSM_SW_ACS_ACG0                         (*(volatile uint32_t *)PSM_SW_ACS_ACG0_ADDR)

#define PSM_SW_ACS_ACG1_ADDR                    (PSM_SHARM_ADDR_BASE+0xF4)
#define PSM_SW_ACS_ACG1                         (*(volatile uint32_t *)PSM_SW_ACS_ACG1_ADDR)

#define PSM_AP_NORMAL_TIMER_ADDR                (PSM_SHARM_ADDR_BASE+0xF8)
#define PSM_AP_NORMAL_TIME                      (*(volatile uint32_t *)PSM_AP_NORMAL_TIMER_ADDR)

#define PSM_AP_RELAX_TIMER_ADDR                 (PSM_SHARM_ADDR_BASE+0xFC)
#define PSM_AP_RELAX_WAKEUP_TIME                (*(volatile uint32_t *)PSM_AP_RELAX_TIMER_ADDR)

#ifdef PSM_DEBUG
#define PSM_AP_FLOW_ADDR                        (PSM_SHARM_ADDR_BASE+0x94)
#define PSM_AP_FLOW                             (*(volatile uint32_t *)PSM_AP_FLOW_ADDR)
#define PSM_CP_FLOW_ADDR                        (PSM_SHARM_ADDR_BASE+0x98)
#define PSM_CP_FLOW                             (*(volatile uint32_t *)PSM_CP_FLOW_ADDR)
#if defined(_CPU_AP)
#define PSM_FLOW                                PSM_AP_FLOW
#else
#define PSM_FLOW                                PSM_CP_FLOW
#endif
#endif

#define PSM_CP_SLEEPCASE_ADDR                   (PSM_SHARM_ADDR_BASE+0x9C)
#define PSM_CP_SLEEPCASE_FLAG                   (*(volatile uint32_t *)PSM_CP_SLEEPCASE_ADDR)

#define PSM_SBYFLAG_ADDR                        (PSM_SHARM_ADDR_BASE+0xA0)
#define PSM_SBYFLAG                             (*(volatile uint32_t *)PSM_SBYFLAG_ADDR)

#define PSM_CP_SLEEPTIME_ADDR                   (PSM_SHARM_ADDR_BASE+0xA4)
#define PSM_CP_SLEEPTIME                        (*(volatile uint32_t *)PSM_CP_SLEEPTIME_ADDR)

#define PSM_AP_SLEEPTIME_ADDR                   (PSM_SHARM_ADDR_BASE+0xA8)
#define PSM_AP_SLEEPTIME                        (*(volatile uint32_t *)PSM_AP_SLEEPTIME_ADDR)

#define PSM_SLEEPCASE_ADDR                      (PSM_SHARM_ADDR_BASE+0xAC)
#define PSM_SLEEPCASE                           (*(volatile uint32_t *)PSM_SLEEPCASE_ADDR)

#define PSM_APTICK_ADDR                         (PSM_SHARM_ADDR_BASE+0xB0)
#define PSM_APTICK                              (*(volatile uint32_t *)PSM_APTICK_ADDR)

#define PSM_CPTICK_ADDR                         (PSM_SHARM_ADDR_BASE+0xB4)
#define PSM_CPTICK                              (*(volatile uint32_t *)PSM_CPTICK_ADDR)

#define PSM_SLEEPFLAG_ADDR                      (PSM_SHARM_ADDR_BASE+0xB8)
#define PSM_SLEEPFLAG                           (*(volatile uint32_t *)PSM_SLEEPFLAG_ADDR)

#define PSM_CPWAKEUP_HSFN_ADDR                  (PSM_SHARM_ADDR_BASE+0xC0)
#define PSM_CPWAKEUP_HSFN                       (*(volatile uint32_t *)PSM_CPWAKEUP_HSFN_ADDR)

#define PSM_CPWAKEUP_MRTR_ADDR                  (PSM_SHARM_ADDR_BASE+0xC4)
#define PSM_CPWAKEUP_MRTR                       (*(volatile uint32_t *)PSM_CPWAKEUP_MRTR_ADDR)

#define PSM_APSLEEPMODE_ADDR                    (PSM_SHARM_ADDR_BASE+0xC8)
#define PSM_APSLEEPMODE                         (*(volatile uint32_t *)PSM_APSLEEPMODE_ADDR)

#define PSM_CPMODE_ADDR                         (PSM_SHARM_ADDR_BASE+0xCA)
#define PSM_CPMODE                              (*(volatile uint8_t *)PSM_CPMODE_ADDR)

#define PSM_CP_AWSNR0_ADDR                      (PSM_SHARM_ADDR_BASE+0xCC)
#define PSM_CP_AWSNR0                           (*(volatile uint32_t *)PSM_CP_AWSNR0_ADDR)
#define PSM_CP_AWSNR1_ADDR                      (PSM_SHARM_ADDR_BASE+0xD0)
#define PSM_CP_AWSNR1                           (*(volatile uint32_t *)PSM_CP_AWSNR1_ADDR)

#define PSM_CP_BACKUP_FLAG_ADDR                 (PSM_SHARM_ADDR_BASE+0xD4)
#define PSM_CP_BACKUP_FLAG                      (*(volatile uint8_t *)PSM_CP_BACKUP_FLAG_ADDR)

#define PSM_CP_RESTORE_FLAG_ADDR                (PSM_SHARM_ADDR_BASE+0xD5)
#define PSM_CP_RESTORE_FLAG                     (*(volatile uint8_t *)PSM_CP_RESTORE_FLAG_ADDR)

#define PSM_OFF_AP_ADDR                         (PSM_SHARM_ADDR_BASE+0x50)
#define PSM_AP_OFF                              (*(volatile uint8_t *)PSM_OFF_AP_ADDR)

#define PSM_OFF_CP_ADDR                         (PSM_SHARM_ADDR_BASE+0x51)
#define PSM_CP_OFF                              (*(volatile uint8_t *)PSM_OFF_CP_ADDR)

#define PSM_LEVEL_ADDR                          (PSM_SHARM_ADDR_BASE+0x52)
#define PSM_LEVEL                               (*(volatile uint8_t *)PSM_LEVEL_ADDR)

#define PSM_LOG_ADDR                            (PSM_SHARM_ADDR_BASE+0x53)
#define PSM_LOG_ON                              (*(volatile uint8_t *)PSM_LOG_ADDR)

#define PSM_AP_SLEEPMODE_ADDR                   (PSM_SHARM_ADDR_BASE+0xD8)
#define PSM_AP_SLEEPMODE                        (*(volatile uint8_t *)PSM_AP_SLEEPMODE_ADDR)

#define PSM_CP_SLEEPMODE_ADDR                   (PSM_SHARM_ADDR_BASE+0xD9)
#define PSM_CP_SLEEPMODE                        (*(volatile uint8_t *)PSM_CP_SLEEPMODE_ADDR)

#define PSM_AP_SLEEPFLOW_ADDR                   (PSM_SHARM_ADDR_BASE+0xDA)
#define PSM_AP_SLEEPFLOW                        (*(volatile uint8_t *)PSM_AP_SLEEPFLOW_ADDR)

#define PSM_CP_SLEEPFLOW_ADDR                   (PSM_SHARM_ADDR_BASE+0xDB)
#define PSM_CP_SLEEPFLOW                        (*(volatile uint8_t *)PSM_CP_SLEEPFLOW_ADDR)

#define PSM_CP_SLEEPCNT_ADDR                    (PSM_SHARM_ADDR_BASE+0xDC)
#define PSM_CP_SLEEPCNT                         (*(volatile uint32_t *)PSM_CP_SLEEPCNT_ADDR)

#define PSM_AP_SLEEPCNT_ADDR                    (PSM_SHARM_ADDR_BASE+0xE0)
#define PSM_AP_SLEEPCNT                         (*(volatile uint32_t *)PSM_AP_SLEEPCNT_ADDR)

#define PSM_SYS_SLEEPCNT_ADDR                   (PSM_SHARM_ADDR_BASE+0xE4)
#define PSM_SYS_SLEEPCNT                        (*(volatile uint32_t *)PSM_SYS_SLEEPCNT_ADDR)

#define PSM_CPSS_SPSAVE_ADDR                    (PSM_SHARM_ADDR_BASE+0xE8)
#define PSM_CPSS_SPSAVE                         (*(volatile uint32_t *)PSM_CPSS_SPSAVE_ADDR)

#define PSM_APSS_SPSAVE_ADDR                    (PSM_SHARM_ADDR_BASE+0xEC)
#define PSM_APSS_SPSAVE                         (*(volatile uint32_t *)PSM_APSS_SPSAVE_ADDR)
#if defined(_CPU_AP)
#define PSM_SLEEP_TIME_TEST                     PSM_AP_SLEEPTIME
#define PSM_TICK_TEST                           PSM_APTICK
#define PSM_COREMEM_SAVEADDR                    PSM_APCOREMEM_SAVEADDR
#define PSM_OFF                                 PSM_AP_OFF
#define PSM_SLEEP_MODE                          PSM_AP_SLEEPMODE
#define PSM_SUBSYS_SLEEPCNT                     PSM_AP_SLEEPCNT
#define PSM_SPSAVE_ADDR                         PSM_APSS_SPSAVE_ADDR
#define PSM_SLEEPFLOW                           PSM_AP_SLEEPFLOW
#else
#define PSM_SLEEP_TIME_TEST                     PSM_CP_SLEEPTIME
#define PSM_TICK_TEST                           PSM_CPTICK
#define PSM_COREMEM_SAVEADDR                    PSM_CPCOREMEM_SAVEADDR
#define PSM_OFF                                 PSM_CP_OFF
#define PSM_SLEEP_MODE                          PSM_CP_SLEEPMODE
#define PSM_SUBSYS_SLEEPCNT                     PSM_CP_SLEEPCNT
#define PSM_SPSAVE_ADDR                         PSM_CPSS_SPSAVE_ADDR
#define PSM_SLEEPFLOW                           PSM_CP_SLEEPFLOW
#endif

#define PSM_SHARM_IDLE_SAVE_ADDR_BASE           (PSM_SHARM_ADDR_BASE + PSM_SHARM_INFO_LEN)
#define PSM_SHARM_IDLE_SAVE_LEN                 (0x80)

#define PSM_IDLE_LDO_ADDR_BASE                  (PSM_SHARM_IDLE_SAVE_ADDR_BASE)
#define PSM_IDLE_LDO_LEN                        (0x20)

#define PSM_IDLE_CLK_ADDR_BASE                  (PSM_IDLE_LDO_ADDR_BASE + PSM_IDLE_LDO_LEN)
#define PSM_IDLE_CLK_LEN                        (0x60)

// XIP 配置信息
#define PSM_XIP_SAVE0                           (IRAM_BASE_ADDR_XIP_CONFIG+0x00)
#define PSM_XIP_SAVE1                           (IRAM_BASE_ADDR_XIP_CONFIG+0x04)
#define PSM_XIP_SAVE2                           (IRAM_BASE_ADDR_XIP_CONFIG+0x08)
#define PSM_XIP_SAVE3                           (IRAM_BASE_ADDR_XIP_CONFIG+0x0C)
#define PSM_XIP_SAVE4                           (IRAM_BASE_ADDR_XIP_CONFIG+0x10)
#define PSM_XIP_SAVE5                           (IRAM_BASE_ADDR_XIP_CONFIG+0x14)
#define PSM_XIP_SAVE6                           (IRAM_BASE_ADDR_XIP_CONFIG+0x18)
#endif
