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
 * @file        riscv_interrupt.h
 *
 * @brief       Header of  Interrupt function.
 *
 * @par History:
 * <table>
 * <tr><th>Date                <th> Author              <th> Notes
 * <tr><td>2022-04-06          <td> ICT Team            <td> First version
 * </table>
 *
 ***********************************************************************************************************************
 */


#ifndef __RISCV_INTERRUPT_H__
#define __RISCV_INTERRUPT_H__
#include <stddef.h>
#include <os_def.h>
#include <os_hw.h>
#include <chip_int_num.h>

/**
 * \brief  Union type to access CLICFG configure register.
 */
typedef union
{
    struct {
        uint8_t _reserved0:1;                   /*!< bit:     0   Overflow condition code flag */
        uint8_t nlbits:4;                       /*!< bit:     29  Carry condition code flag */
        uint8_t _reserved1:2;                   /*!< bit:     30  Zero condition code flag */
        uint8_t _reserved2:1;                   /*!< bit:     31  Negative condition code flag */
    } b;                                        /*!< Structure used for bit  access */
    uint8_t w;                                  /*!< Type      used for byte access */
} CLICCFG_Type;

/**
 * \brief  Union type to access CLICINFO information register.
 */
typedef union {
    struct {
        uint32_t numint:13;                     /*!< bit:  0..12   number of maximum interrupt inputs supported */
        uint32_t version:8;                     /*!< bit:  13..20  20:17 for architecture version,16:13 for implementation version */
        uint32_t intctlbits:4;                  /*!< bit:  21..24  specifies how many hardware bits are actually implemented in the clicintctl registers */
        uint32_t _reserved0:7;                  /*!< bit:  25..31  Reserved */
    } b;                                        /*!< Structure used for bit  access */
    uint32_t w;                                 /*!< Type      used for word access */
} CLICINFO_Type;

/**
 * \brief Access to the structure of a vector interrupt controller.
 */
typedef struct {
    volatile uint8_t  INTIP;                       /*!< Offset: 0x000 (R/W)  Interrupt set pending register */
    volatile uint8_t  INTIE;                       /*!< Offset: 0x001 (R/W)  Interrupt set enable register */
    volatile uint8_t  INTATTR;                     /*!< Offset: 0x002 (R/W)  Interrupt set attributes register */
    volatile uint8_t  INTCTRL;                     /*!< Offset: 0x003 (R/W)  Interrupt configure register */
} CLIC_CTRL_Type;

typedef struct {
    volatile uint8_t   CFG;                        /*!< Offset: 0x000 (R/W)  CLIC configuration register */
    uint8_t RESERVED0[3];
    volatile const uint32_t  INFO;                        /*!< Offset: 0x004 (R/ )  CLIC information register */
    uint8_t RESERVED1[3];
    volatile uint8_t  MTH;                         /*!< Offset: 0x00B (R/W)  CLIC machine mode threshold register */
    uint32_t RESERVED2[0x3FD];
    CLIC_CTRL_Type CTRL[4096];                  /*!< Offset: 0x1000 (R/W) CLIC register structure for INTIP, INTIE, INTATTR, INTCTL */
} CLIC_Type;

#define CLIC_CLICCFG_NLBIT_Pos                 1U                                       /*!< CLIC CLICCFG: NLBIT Position */
#define CLIC_CLICCFG_NLBIT_Msk                 (0xFUL << CLIC_CLICCFG_NLBIT_Pos)        /*!< CLIC CLICCFG: NLBIT Mask */

#define CLIC_CLICINFO_CTLBIT_Pos                21U                                     /*!< CLIC INTINFO: __ECLIC_GetInfoCtlbits() Position */
#define CLIC_CLICINFO_CTLBIT_Msk                (0xFUL << CLIC_CLICINFO_CTLBIT_Pos)     /*!< CLIC INTINFO: __ECLIC_GetInfoCtlbits() Mask */

#define CLIC_CLICINFO_VER_Pos                  13U                                      /*!< CLIC CLICINFO: VERSION Position */
#define CLIC_CLICINFO_VER_Msk                  (0xFFUL << CLIC_CLICINFO_VER_Pos)        /*!< CLIC CLICINFO: VERSION Mask */

#define CLIC_CLICINFO_NUM_Pos                  0U                                       /*!< CLIC CLICINFO: NUM Position */
#define CLIC_CLICINFO_NUM_Msk                  (0xFFFUL << CLIC_CLICINFO_NUM_Pos)       /*!< CLIC CLICINFO: NUM Mask */

#define CLIC_INTIP_IP_Pos                      0U                                       /*!< CLIC INTIP: IP Position */
#define CLIC_INTIP_IP_Msk                      (0x1UL << CLIC_INTIP_IP_Pos)             /*!< CLIC INTIP: IP Mask */

#define CLIC_INTIE_IE_Pos                      0U                                       /*!< CLIC INTIE: IE Position */
#define CLIC_INTIE_IE_Msk                      (0x1UL << CLIC_INTIE_IE_Pos)             /*!< CLIC INTIE: IE Mask */

#define CLIC_INTATTR_TRIG_Pos                  1U                                       /*!< CLIC INTATTR: TRIG Position */
#define CLIC_INTATTR_TRIG_Msk                  (0x3UL << CLIC_INTATTR_TRIG_Pos)         /*!< CLIC INTATTR: TRIG Mask */

#define CLIC_INTATTR_SHV_Pos                   0U                                       /*!< CLIC INTATTR: SHV Position */
#define CLIC_INTATTR_SHV_Msk                   (0x1UL << CLIC_INTATTR_SHV_Pos)          /*!< CLIC INTATTR: SHV Mask */

#define ECLIC_MAX_INTCTLBITS                   8U                                   /*!< Max bit of the CLICINTCTLBITS */
#define ECLIC_MAX_NLBITS                       8U                                       /*!< Max nlbit of the CLICINTCTLBITS */
#define ECLIC_MODE_MTVEC_Msk                   3U                                       /*!< ECLIC Mode mask for MTVT CSR Register */

#define ECLIC_NON_VECTOR_INTERRUPT             0x0                                      /*!< Non-Vector Interrupt Mode of ECLIC */
#define ECLIC_VECTOR_INTERRUPT                 0x1                                      /*!< Vector Interrupt Mode of ECLIC */


/**\brief ECLIC Trigger Enum for different Trigger Type */
typedef enum ECLIC_TRIGGER {
		ECLIC_LEVEL_TRIGGER = 0x0,			/*!< Level Triggerred, trig[0] = 0 */
		ECLIC_POSTIVE_EDGE_TRIGGER = 0x1,	/*!< Postive/Rising Edge Triggered, trig[1] = 1, trig[0] = 0 */
		ECLIC_NEGTIVE_EDGE_TRIGGER = 0x3,	/*!< Negtive/Falling Edge Triggered, trig[1] = 1, trig[0] = 0 */
		ECLIC_MAX_TRIGGER = 0x3 			/*!< MAX Supported Trigger Mode */
} ECLIC_TRIGGER_Type;

typedef enum IRQn
	{
	/* =======================================	Nuclei Core Specific Interrupt Numbers	======================================== */
	
		Reserved0_IRQn			  =   0,			  /*!<	Internal reserved */
		Reserved1_IRQn			  =   1,			  /*!<	Internal reserved */
		Reserved2_IRQn			  =   2,			  /*!<	Internal reserved */
		SysTimerSW_IRQn 		  =   3,			  /*!<	System Timer SW interrupt */
		Reserved3_IRQn			  =   4,			  /*!<	Internal reserved */
		Reserved4_IRQn			  =   5,			  /*!<	Internal reserved */
		Reserved5_IRQn			  =   6,			  /*!<	Internal reserved */
		SysTimer_IRQn			  =   7,			  /*!<	System Timer Interrupt */
		Reserved6_IRQn			  =   8,			  /*!<	Internal reserved */
		Reserved7_IRQn			  =   9,			  /*!<	Internal reserved */
		Reserved8_IRQn			  =  10,			  /*!<	Internal reserved */
		Reserved9_IRQn			  =  11,			  /*!<	Internal reserved */
		Reserved10_IRQn 		  =  12,			  /*!<	Internal reserved */
		Reserved11_IRQn 		  =  13,			  /*!<	Internal reserved */
		Reserved12_IRQn 		  =  14,			  /*!<	Internal reserved */
		Reserved13_IRQn 		  =  15,			  /*!<	Internal reserved */
		Reserved14_IRQn 		  =  16,			  /*!<	Internal reserved */
		Reserved15_IRQn 		  =  17,			  /*!<	Internal reserved */
		Reserved16_IRQn 		  =  18,			  /*!<	Internal reserved */
	
	/* ===========================================	hbird Specific Interrupt Numbers  ========================================= */
	/* ToDo: add here your device specific external interrupt numbers. 19~1023 is reserved number for user. Maxmum interrupt supported
			 could get from clicinfo.NUM_INTERRUPT. According the interrupt handlers defined in startup_Device.s
			 eg.: Interrupt for Timer#1 	  eclic_tim1_handler   ->	TIM1_IRQn */
		SOC_INT19_IRQn			 = 19,				  /*!< Device Interrupt */
		SOC_INT20_IRQn			 = 20,				  /*!< Device Interrupt */
		
		SOC_INT_MAX = Reserved16_IRQn + INT_MAX_NUM + 2,
} IRQn_Type;

#define __ECLIC_BASEADDR    0x00420000UL                /*!< Set to ECLIC baseaddr of Innochip FPGA */
/* ECLIC Memory mapping of Device */
#define ECLIC_BASE          __ECLIC_BASEADDR            /*!< ECLIC Base Address */
#define ECLIC               ((CLIC_Type *)ECLIC_BASE)  /*!< CLIC configuration struct */
#define ECLIC_MEMSIZE       ((sizeof(CLIC_Type) - (4096 - SOC_INT_MAX)*sizeof(CLIC_CTRL_Type) + 0x10) & (~0xf))

typedef void (*os_irq_handler_t)(int irq_id, void* irq_data);

extern unsigned short priority_min;
extern unsigned short priority_max;

typedef struct {
	os_irq_handler_t    handler;
	void				*irq_data;
#ifdef OS_USING_CPU_MONITER
    osCpuUsage_t        usageInfo;
#endif
} irq_entry_t;

#ifdef __cplusplus
    extern "C" {
#endif

extern void ECLIC_Init(void);
extern void ECLIC_DisableIRQ(IRQn_Type IRQn);
extern void ECLIC_EnableIRQ(IRQn_Type IRQn);
extern uint32_t ECLIC_GetEnableIRQ(IRQn_Type IRQn);
extern void ECLIC_SetShvIRQ(IRQn_Type IRQn, uint32_t shv);
extern void ECLIC_SetTrigIRQ(IRQn_Type IRQn, uint32_t trig);
extern void ECLIC_SetLevelAndPriorityIRQ(IRQn_Type IRQn, uint8_t pri);
extern uint8_t ECLIC_GetLevelAndPriorityIRQ(IRQn_Type IRQn);
extern int32_t ECLIC_GetPendingIRQ(IRQn_Type IRQn);
extern void ECLIC_SetPendingIRQ(IRQn_Type IRQn);
extern void ECLIC_ClearPendingIRQ(IRQn_Type IRQn);
extern int32_t ECLIC_Register_IRQ(IRQn_Type IRQn, uint8_t shv, ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority, void *handler);

#ifdef __cplusplus
    }
#endif

#endif /* __RISCV_INTERRUPT_H__ */

