/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_hw.h
 *
 * @brief     os hardware头文件.
 *
 * @revision  1.0
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version   <th>Author     <th>Description
 * <tr><td>2023-06-21  <td>1.0       <td>ICT Team   <td>初始版本
 * </table>
 ************************************************************************************
 */

#ifndef __OS_HW_H__
#define __OS_HW_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup OsHw
 */

/**@{*/

/*
 * Some macros define
 */

#define AP_CORE_ID  (1)
#define CP_CORE_ID  (2)

#ifdef _CPU_AP
    #define SELF_CORE_ID    AP_CORE_ID
#else
    #define SELF_CORE_ID    CP_CORE_ID
#endif

#ifndef OS_CACHE_LINE_SZ
#define OS_CACHE_LINE_SZ    32
#endif

#define OS_EXT_IRQ_TO_IRQ(ext_irq)  ((ext_irq) + 19)

typedef enum {
    IRQ_HIGH_LEVEL,
    IRQ_LOW_LEVEL,
    IRQ_POSITIVE_EDGE,
    IRQ_NEGATIVE_EDGE,
    IRQ_EDGE_BOTH
} IrqType_t;

typedef enum {
    IRQ_PRI_LOW,
    IRQ_PRI_NORMAL,
    IRQ_PRI_HIGH,
} IrqPri_t;

/*
 * Cache interfaces
 */
void osICacheEnable(void);
void osICacheDisable(void);
void osICacheInvalid(void* addr, size_t size);

void osDCacheEnable(void);
void osDCacheDisable(void);

/**
 ************************************************************************************
 * @brief           无效指令cache部分区域
 *
 * @param[in]       addr        区域起始地址
 * @param[in]       size        区域大小
 *
 * @return          void.
 ************************************************************************************
 */
void osICacheInvalidRange(void* beginaddr, size_t size);

/**
 ************************************************************************************
 * @brief           清理数据cache部分区域
 *
 * @param[in]       addr        区域起始地址
 * @param[in]       size        区域大小
 *
 * @return          void.
 ************************************************************************************
 */
void osDCacheCleanRange(void* addr, size_t size);


/**
 ************************************************************************************
 * @brief           无效数据cache部分区域
 *
 * @param[in]       addr        区域起始地址
 * @param[in]       size        区域大小
 *
 * @return          void.
 ************************************************************************************
 */
void osDCacheInvalidRange(void* addr, size_t size);

/**
 ************************************************************************************
 * @brief           清理并无效数据cache部分区域
 *
 * @param[in]       addr        区域起始地址
 * @param[in]       size        区域大小
 *
 * @return          void.
 ************************************************************************************
 */
void osDCacheCleanAndInvalidRange(void* addr, size_t size);  // flush

/*
 * CPU interfaces
 */
void osCpuReset(void);
void osCpuShutdown(void);

uint8_t *osStackInit(void           *entry,
                            void    *parameter,
                            uint8_t *stackAddr,
                            void    *exit);

uint32_t osStackMaxUsed(const char *begin, uint32_t size);

void* osCurrentSp(void);

/*
 * Interrupt handler definition
 */
typedef void (*IsrHandler_t)(int vector, void *param);

/*
 * Interrupt interfaces
 */
void osInterruptInit(void);
void osInterruptMask(int vector);
void osInterruptUnmask(int vector);
uint32_t osInterruptEnabled(int vector);
void osInterruptConfig(int vector, IrqPri_t priority, IrqType_t type);
IsrHandler_t osInterruptInstall(int vector, IsrHandler_t handler, void *param);
void osInterruptUninstall(int vector);
int32_t osInterruptGetPending(int irq_id);
void osInterruptSetPending(int irq_id);
void osInterruptClrPending(int irq_id);
void osInterruptWakeup(int irq_id, int en);
void osInterruptMaskAll(void);
void osInterruptUnmaskAll(void);

/*
 * Exception interfaces
 */
void osExceptionInit(void);
void osXipReqTimerInit(void);
void osXipResume(void);

/**
 * CPU interrupt enable&disable
 */

/**
 ************************************************************************************
 * @brief           保存当前的中断状态并锁中断
 *
 * @param[in]       void
 *
 * @return          中断当前状态，osInterruptEnable时使用.
 ************************************************************************************
 */
ubase_t osInterruptDisable(void);
//ubase_t osInterruptDisableEx(void);

/**
 ************************************************************************************
 * @brief           恢复之前保存的中断状态
 *
 * @param[in]       level       osInterruptDisable时保存的中断状态
 *
 * @return          void.
 ************************************************************************************
 */
void osInterruptEnable(ubase_t level);
//void osInterruptEnableEx(ubase_t level);
ubase_t osInterruptIsEnable(void);

/*
 * Context interfaces
 */
void osContextSwitch(ubase_t from, ubase_t to);
void osContextSwitchTo(ubase_t to);
void osContextSwitchInterrupt(ubase_t from, ubase_t to);

void osConsoleSetUart(void *hUart);
void* osConsoleGetUart(void);
void *osLogGetUart(void);
void osConsoleOutput(const char *str);

void osBackTrace(uint32_t *fp, ubase_t thread_entry);

/*
 * delay interfaces
 */
void osUsDelay(uint32_t us);
void osCntDelay(uint32_t cnt);

/*
 * time interfaces
 */
void osSetSysCompareTimeUs(osTime_t us);
osTime_t osGetSysTime(void);
osTime_t osGetSysTimeUs(void);
osTime_t osGetTime(void);
osTime_t osGetTimeUs(void);
void SysTimer_Restore(uint64_t tick);

/*
 * psm interfaces
 */
void osCoreMemSave(void);
void osCoreMemRestore(void);
void osEnterWfi(void);
void osSetBarrier(void);
void osBpuEnable(void);
void osSysTimerStop(void);
void osSysTimerStart(uint64_t osTickStart);
uint64_t osGetSysTimeCnt64(void);

#ifdef __cplusplus
}
#endif

#endif
/** @} */

