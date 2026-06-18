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
 * @file        riscv_general.h
 *
 * @brief       Header of  General function.
 *
 * @par History:
 * <table>
 * <tr><th>Date                <th> Author              <th> Notes
 * <tr><td>2022-04-06          <td> ICT Team            <td> First version
 * </table>
 *
 ***********************************************************************************************************************
 */



#ifndef __RISCV_GENERAL_H__
#define __RISCV_GENERAL_H__

#include <stddef.h>
#include <stdint.h>
#include "riscv_encoding.h"

void __enable_irq(void);
void __disable_irq(void);

#define STR(x)                  XSTR(x)
#define XSTR(x)                 #x
#define __STR(s)                #s
#define STRINGIFY(s)            __STR(s)

//#define  __ADDR_CACHABLE_TO_NONCACHABLE(addr)        ((void *)((addr) + 0x08000000))   
#define  __ADDR_CACHABLE_TO_NONCACHABLE(addr)        ((unsigned int)(uintptr_t)((addr) + 0x08000000))  

#ifndef __RISCV_XLEN
	  /** \brief Refer to the width of an integer register in bits(either 32 or 64) */
  #ifndef __riscv_xlen
    #define __RISCV_XLEN    32
  #else
    #define __RISCV_XLEN    __riscv_xlen
  #endif
#endif /* __RISCV_XLEN */

	
	/** \brief Type of Control and Status Register(CSR), depends on the XLEN defined in RISC-V */
#if __RISCV_XLEN == 32
	  typedef uint32_t rv_csr_t;
#elif __RISCV_XLEN == 64
	  typedef uint64_t rv_csr_t;
#else
	  typedef uint32_t rv_csr_t;
#endif

	/** \brief provide the compiler with branch prediction information, the branch is usually true */
#ifndef   __USUALLY
  #define __USUALLY(exp)                         __builtin_expect((exp), 1)
#endif
	
	/** \brief provide the compiler with branch prediction information, the branch is rarely true */
#ifndef   __RARELY
  #define __RARELY(exp)                          __builtin_expect((exp), 0)
#endif



#ifndef __ASSEMBLY__
	
	/**
	 * \brief CSR operation Macro for csrrw instruction.
	 * \details
	 * Read the content of csr register to __v,
	 * then write content of val into csr register, then return __v
	 * \param csr	CSR macro definition defined in
	 *				\ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
	 * \param val	value to store into the CSR register
	 * \return the CSR register value before written
	 */
#define __RV_CSR_SWAP(csr, val)                                 \
		({															\
			register rv_csr_t __v = (unsigned long)(val);			\
			__asm volatile("csrrw %0, " STRINGIFY(csr) ", %1"		\
						 : "=r"(__v)								\
						 : "rK"(__v)								\
						 : "memory");								\
			__v;													\
		})
	
	/**
	 * \brief CSR operation Macro for csrr instruction.
	 * \details
	 * Read the content of csr register to __v and return it
	 * \param csr	CSR macro definition defined in
	 *				\ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
	 * \return the CSR register value
	 */
#define __RV_CSR_READ(csr)                                      \
		({															\
			register rv_csr_t __v;									\
			__asm volatile("csrr %0, " STRINGIFY(csr)				\
						 : "=r"(__v)								\
						 :											\
						 : "memory");								\
			__v;													\
		})
	
	/**
	 * \brief CSR operation Macro for csrw instruction.
	 * \details
	 * Write the content of val to csr register
	 * \param csr	CSR macro definition defined in
	 *				\ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
	 * \param val	value to store into the CSR register
	 */
#define __RV_CSR_WRITE(csr, val)                                \
		({															\
			register rv_csr_t __v = (rv_csr_t)(val);				\
			__asm volatile("csrw " STRINGIFY(csr) ", %0"			\
						 :											\
						 : "rK"(__v)								\
						 : "memory");								\
		})
	
	/**
	 * \brief CSR operation Macro for csrrs instruction.
	 * \details
	 * Read the content of csr register to __v,
	 * then set csr register to be __v | val, then return __v
	 * \param csr	CSR macro definition defined in
	 *				\ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
	 * \param val	Mask value to be used wih csrrs instruction
	 * \return the CSR register value before written
	 */
#define __RV_CSR_READ_SET(csr, val)                             \
		({															\
			register rv_csr_t __v = (rv_csr_t)(val);				\
			__asm volatile("csrrs %0, " STRINGIFY(csr) ", %1"		\
						 : "=r"(__v)								\
						 : "rK"(__v)								\
						 : "memory");								\
			__v;													\
		})
	
	/**
	 * \brief CSR operation Macro for csrs instruction.
	 * \details
	 * Set csr register to be csr_content | val
	 * \param csr	CSR macro definition defined in
	 *				\ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
	 * \param val	Mask value to be used wih csrs instruction
	 */
#define __RV_CSR_SET(csr, val)                                  \
		({															\
			register rv_csr_t __v = (rv_csr_t)(val);				\
			__asm volatile("csrs " STRINGIFY(csr) ", %0"			\
						 :											\
						 : "rK"(__v)								\
						 : "memory");								\
		})
	
	/**
	 * \brief CSR operation Macro for csrrc instruction.
	 * \details
	 * Read the content of csr register to __v,
	 * then set csr register to be __v & ~val, then return __v
	 * \param csr	CSR macro definition defined in
	 *				\ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
	 * \param val	Mask value to be used wih csrrc instruction
	 * \return the CSR register value before written
	 */
#define __RV_CSR_READ_CLEAR(csr, val)                           \
		({															\
			register rv_csr_t __v = (rv_csr_t)(val);				\
			__asm volatile("csrrc %0, " STRINGIFY(csr) ", %1"		\
						 : "=r"(__v)								\
						 : "rK"(__v)								\
						 : "memory");								\
			__v;													\
		})
	
	/**
	 * \brief CSR operation Macro for csrc instruction.
	 * \details
	 * Set csr register to be csr_content & ~val
	 * \param csr	CSR macro definition defined in
	 *				\ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
	 * \param val	Mask value to be used wih csrc instruction
	 */
#define __RV_CSR_CLEAR(csr, val)                                \
		({															\
			register rv_csr_t __v = (rv_csr_t)(val);				\
			__asm volatile("csrc " STRINGIFY(csr) ", %0"			\
						 :											\
						 : "rK"(__v)								\
						 : "memory");								\
		})
#endif /* __ASSEMBLY__ */

	/**
	 * \brief	Save necessary CSRs into variables for vector interrupt nesting
	 * \details
	 * This macro is used to declare variables which are used for saving
	 * CSRs(MCAUSE, MEPC, MSUB), and it will read these CSR content into
	 * these variables, it need to be used in a vector-interrupt if nesting
	 * is required.
	 * \remarks
	 * - Interrupt will be enabled after this macro is called
	 * - It need to be used together with \ref RESTORE_IRQ_CSR_CONTEXT
	 * - Don't use variable names __mcause, __mpec, __msubm in your ISR code
	 * - If you want to enable interrupt nesting feature for vector interrupt,
	 * you can do it like this:
	 * \code
	 * // __INTERRUPT attribute will generates function entry and exit sequences suitable
	 * // for use in an interrupt handler when this attribute is present
	 * __INTERRUPT void eclic_mtip_handler(void)
	 * {
	 *	   // Must call this to save CSRs
	 *	   SAVE_IRQ_CSR_CONTEXT();
	 *	   // !!!Interrupt is enabled here!!!
	 *	   // !!!Higher priority interrupt could nest it!!!
	 *
	 *	   // put you own interrupt handling code here
	 *
	 *	   // Must call this to restore CSRs
	 *	   RESTORE_IRQ_CSR_CONTEXT();
	 * }
	 * \endcode
	 */
#define SAVE_IRQ_CSR_CONTEXT()                                              \
			rv_csr_t __mcause = __RV_CSR_READ(CSR_MCAUSE);						\
			rv_csr_t __mepc = __RV_CSR_READ(CSR_MEPC);							\
			rv_csr_t __msubm = __RV_CSR_READ(CSR_MSUBM);						\
			__enable_irq();
	
	/**
	 * \brief	Restore necessary CSRs from variables for vector interrupt nesting
	 * \details
	 * This macro is used restore CSRs(MCAUSE, MEPC, MSUB) from pre-defined variables
	 * in \ref SAVE_IRQ_CSR_CONTEXT macro.
	 * \remarks
	 * - Interrupt will be disabled after this macro is called
	 * - It need to be used together with \ref SAVE_IRQ_CSR_CONTEXT
	 */
#define RESTORE_IRQ_CSR_CONTEXT()                                           \
			__disable_irq();													\
			__RV_CSR_WRITE(CSR_MSUBM, __msubm); 								\
			__RV_CSR_WRITE(CSR_MEPC, __mepc);									\
			__RV_CSR_WRITE(CSR_MCAUSE, __mcause);
	
	/** @} */ /* End of Doxygen Group NMSIS_Core_IntExc */



#ifdef __cplusplus
    extern "C" {
#endif
extern void PMP_LockXipMemory(void);
extern void PMP_UnlockXipMemory(void);
extern void PMP_OpenXipMemory(void);
extern void PMP_OpenXipMemoryForRead(void);
extern void PMP_CloseXipMemory(void);

#ifdef __cplusplus
    }
#endif

#endif /* __RISCV_GENERAL_H__ */


