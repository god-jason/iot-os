/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        psm_common.h
 *
 * @brief       PSM驱动代码
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team          创建
 ************************************************************************************
 */

#ifndef __PSM_COMMON__
#define __PSM_COMMON__

#include <psm_layout.h>

/**
 * @addtogroup Psm
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#ifndef __ASSEMBLER__
/**
\brief:Psm idle flow state.
*/
typedef enum {
    PSM_WORKING = 0,
    PSM_SINGLE_CORE_SLEEP,
    PSM_HARDWARE_SLEEP,
} PSM_IdleState;
#endif

//#define PSM_STANDBY_SLEEPTIME_THREHOLD      600000
#define PSM_DEEPSLEEP_SLEEPTIME_THREHOLD    20
#define PSM_DEEPSLEEP_AHEAD_WAKEUP_TIME     12
#define PSM_DCXO26MON_AHEAD_WAKEUP_TIME     4
#define PSM_SWIDLE_AHEAD_WAKEUP_TIME        2
#define PSM_SW_SLEEPTIME_THREHOLD           10

#define PSM_WARNING_SLEEPTIME_THREHOLD      10

#define SHRAM_RETENTION_BITMAP              0xFF

#define ALIGN16(x)                          (((x) + 0xf) & ~0xf)

#define READ_BIT_U8(ADDR, CNT, OFFSET)      ((READ_U8(ADDR) >> (OFFSET)) & ((1<<(CNT)) - 1))      ///< 读8位寄存器的BIT位
#define READ_BIT_U16(ADDR, CNT, OFFSET)     ((READ_U16(ADDR) >> (OFFSET)) & ((1<<(CNT)) - 1))     ///< 读16位寄存器的BIT位
#define READ_BIT_U32(ADDR, CNT, OFFSET)     ((READ_U32(ADDR) >> (OFFSET)) & ((1<<(CNT)) - 1))     ///< 读32位寄存器的BIT位
#define READ_BIT_U64(ADDR, CNT, OFFSET)     ((READ_U64(ADDR) >> (OFFSET)) & ((1<<(CNT)) - 1))     ///< 读64位寄存器的BIT位

#define PSM_SET_BIT(ADDR, BIT)              (WRITE_U32((ADDR), READ_U32(ADDR) | (1 << (BIT))))
#define PSM_CLR_BIT(ADDR, BIT)              (WRITE_U32((ADDR), READ_U32(ADDR) & ~(1 << (BIT))))

#define PSM_GET_BIT(ADDR, BIT)              ((READ_U32(ADDR) >> (BIT)) & 1)

#define SECTION_IRAM_FUNC                   __IRAM_CODE_PSM_RE
#define SECTION_IRAM_DATA                   __IRAM_DATA_PSM_RE

#define PSM_LOG_OUTPUT                      (0)
#define PSM_LOG_NORMAL                      (1)
#define PSM_LOG_DEBUG                       (2)

#define PSM_PRINTF(level, format, ...) \
    {   \
        if (PSM_LOG_ON >= level) \
            osPrintf(format, ##__VA_ARGS__); \
    }

//#define _PSM_TEST
//#define USE_32KRC

//#define PSM_GPIO_ANA
#ifdef PSM_GPIO_ANA

#define PSM_GPIO_BASE_ADDR  0xF2803020
#ifdef _CPU_AP
#define GPIO_BIT 1
#else
#define GPIO_BIT 2
#endif

#define GPIO_SET_LOW        PSM_CLR_BIT(PSM_GPIO_BASE_ADDR, GPIO_BIT)
#define GPIO_SET_HIGH       PSM_SET_BIT(PSM_GPIO_BASE_ADDR, GPIO_BIT)

#define GPIO_TEST  (PSM_GET_BIT(PSM_GPIO_BASE_ADDR, GPIO_BIT) ? GPIO_SET_LOW : GPIO_SET_HIGH)

#endif
#endif
/** @} */