/**
 ***********************************************************************************************************************
 * Copyright (c) 2022,Nanjing InnoChip Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        riscv_timer.h
 *
 * @brief       Header of  Timer function.
 *
 * @par History:
 * <table>
 * <tr><th>Date                <th> Author              <th> Notes
 * <tr><td>2022-04-06          <td> ICT Team            <td> First version
 * </table>
 *
 ***********************************************************************************************************************
 */


#ifndef __RISCV_TIMER_H__
#define __RISCV_TIMER_H__

#include <os_config.h>
#include <os_def.h>


#define TIMER_MSIP          0xFFC
#define TIMER_MSIP_size     0x4
#define TIMER_MTIMECMP      0x8
#define TIMER_MTIMECMP_size 0x8
#define TIMER_MTIME         0x0
#define TIMER_MTIME_size    0x8

#define TIMER_CTRL_ADDR   0xd1000000
#define TIMER_REG(offset) _REG32(TIMER_CTRL_ADDR, offset)
#define TIMER_FREQ        ((uint32_t)SystemCoreClock / 4)    // units HZ

#ifdef USE_TOP_FPGA
#define FPGA_CORE_FREQ          (60000000)
#endif
#ifdef USE_TOP_ASIC
#define FPGA_CORE_FREQ          (416000000)
#endif

#ifdef USING_EMULATOR
#define SYS_TIMER_FREQ          (1000000)           // for emulator
#define TIMER_TICKS             (SYS_TIMER_FREQ  / OS_TICK_PER_SECOND)
#else
#ifdef USE_TOP_FPGA
#define SYS_TIMER_FREQ          (961465)    //(26000000 / 52 * 2) for FPGA, 26MHz的52倍分频，timer双沿采样，最终为1MHz，systimer和tpu时间对不上，暂时软件做微调，等EVB回来再验证下
#endif
#ifdef USE_TOP_ASIC
#define SYS_TIMER_FREQ          (1000000)
#endif
#define TIMER_TICKS             (SYS_TIMER_FREQ * FPGA_TIME_DELAY_RATE / OS_TICK_PER_SECOND)
#endif

/**
 * \brief  Structure type to access the System Timer (SysTimer).
 * \details
 * Structure definition to access the system timer(SysTimer).
 * \remarks
 * - MSFTRST register is introduced in Nuclei N Core version 1.3(\ref __NUCLEI_N_REV >= 0x0103)
 * - MSTOP register is renamed to MTIMECTL register in Nuclei N Core version 1.4(\ref __NUCLEI_N_REV >= 0x0104)
 * - CMPCLREN and CLKSRC bit in MTIMECTL register is introduced in Nuclei N Core version 1.4(\ref __NUCLEI_N_REV >= 0x0104)
 */
typedef struct {
    volatile uint64_t MTIMER;                  /*!< Offset: 0x000 (R/W)  System Timer current value 64bits Register */
    volatile uint64_t MTIMERCMP;               /*!< Offset: 0x008 (R/W)  System Timer compare Value 64bits Register */
    volatile uint32_t RESERVED0[0x3F8];        /*!< Offset: 0x010 - 0xFEC Reserved */
    volatile uint32_t MSFTRST;                 /*!< Offset: 0xFF0 (R/W)  System Timer Software Core Reset Register */
    volatile uint32_t RESERVED1;               /*!< Offset: 0xFF4 Reserved */
    volatile uint32_t MTIMECTL;                /*!< Offset: 0xFF8 (R/W)  System Timer Control Register, previously MSTOP register */
    volatile uint32_t MSIP;                    /*!< Offset: 0xFFC (R/W)  System Timer SW interrupt Register */
} SysTimer_Type;


/* Timer Control / Status Register Definitions */
#define SysTimer_MTIMECTL_TIMESTOP_Pos      0U                                          /*!< SysTick Timer MTIMECTL: TIMESTOP bit Position */
#define SysTimer_MTIMECTL_TIMESTOP_Msk      (1UL << SysTimer_MTIMECTL_TIMESTOP_Pos)     /*!< SysTick Timer MTIMECTL: TIMESTOP Mask */
#define SysTimer_MTIMECTL_CMPCLREN_Pos      1U                                          /*!< SysTick Timer MTIMECTL: CMPCLREN bit Position */
#define SysTimer_MTIMECTL_CMPCLREN_Msk      (1UL << SysTimer_MTIMECTL_CMPCLREN_Pos)     /*!< SysTick Timer MTIMECTL: CMPCLREN Mask */
#define SysTimer_MTIMECTL_CLKSRC_Pos        2U                                          /*!< SysTick Timer MTIMECTL: CLKSRC bit Position */
#define SysTimer_MTIMECTL_CLKSRC_Msk        (1UL << SysTimer_MTIMECTL_CLKSRC_Pos)       /*!< SysTick Timer MTIMECTL: CLKSRC Mask */

#define SysTimer_MSIP_MSIP_Pos              0U                                          /*!< SysTick Timer MSIP: MSIP bit Position */
#define SysTimer_MSIP_MSIP_Msk              (1UL << SysTimer_MSIP_MSIP_Pos)             /*!< SysTick Timer MSIP: MSIP Mask */

#define SysTimer_MTIMER_Msk                 (0xFFFFFFFFFFFFFFFFULL)                     /*!< SysTick Timer MTIMER value Mask */
#define SysTimer_MTIMERCMP_Msk              (0xFFFFFFFFFFFFFFFFULL)                     /*!< SysTick Timer MTIMERCMP value Mask */
#define SysTimer_MTIMECTL_Msk               (0xFFFFFFFFUL)                              /*!< SysTick Timer MTIMECTL/MSTOP value Mask */
#define SysTimer_MSIP_Msk                   (0xFFFFFFFFUL)                              /*!< SysTick Timer MSIP   value Mask */
#define SysTimer_MSFTRST_Msk                (0xFFFFFFFFUL)                              /*!< SysTick Timer MSFTRST value Mask */

#define SysTimer_MSFRST_KEY                 (0x80000A5FUL)                              /*!< SysTick Timer Software Reset Request Key */


/* System Timer Memory mapping of Device  */
//#define __SYSTIMER_BASEADDR                 0x02000000UL          /*!< Set to SysTimer baseaddr of your device */
#define __SYSTIMER_BASEADDR                   0x00430000UL          /*!< Set to SysTimer baseaddr of your device */

#define SysTimer_BASE                       __SYSTIMER_BASEADDR                         /*!< SysTick Base Address */
#define SysTimer                            ((SysTimer_Type *) SysTimer_BASE)           /*!< SysTick configuration struct */



#ifdef __cplusplus
    extern "C" {
#endif

extern void setup_timer(void);
extern void eclic_mtip_handler(void);
extern void eclic_msip_handler(void);	
extern uint64_t SysTimer_GetLoadValue(void);


extern void SysTimer_SetSWIRQ(void);
extern void SysTimer_ClearSWIRQ(void);

#ifdef __cplusplus
    }
#endif

#endif /* __RISCV_TIMER_H__ */


