/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_internal.h
 *
 * @brief     os内部头文件.
 *
 * @revision  1.0
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version   <th>Author     <th>Description
 * <tr><td>2023-07-03  <td>1.0       <td>ICT Team   <td>初始版本
 * </table>
 ************************************************************************************
 */

#ifndef __OS_INTERNAL_H__
#define __OS_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 ***********************************************************************************************************************
 * @def         SYS_MALLOC_CTRL_BLK
 *
 * @brief       System malloc memory for control block flag
 ***********************************************************************************************************************
 */
#define SYS_MALLOC_CTRL_BLK 0x0100

/**
 ***********************************************************************************************************************
 * @def         SYS_MALLOC_MEM
 *
 * @brief       System malloc memory flag
 ***********************************************************************************************************************
 */
#define SYS_MALLOC_MEM      0x0200

/**
 ***********************************************************************************************************************
 * @def         SYS_TIMER_FREE_PARA
 *
 * @brief       Frees timeout function's parameter when deleting timer
 ***********************************************************************************************************************
 */
#define SYS_TIMER_FREE_PARA 0x0400

/**
 ***********************************************************************************************************************
 * @def         SYS_USER_DATA_IS_MQ
 *
 * @brief       System thread user data is mq flag
 ***********************************************************************************************************************
 */
#define SYS_USER_DATA_IS_MQ 0x0800

/**
 ***********************************************************************************************************************
 * @def         SYS_OSOBJ_CMSIS
 *
 * @brief       System object created by CMSIS flag
 ***********************************************************************************************************************
 */
#define SYS_OSOBJ_CMSIS     0x8000

#define OS_EXCEPTION_INFO_MAGIC 0x1A1A1A1A

typedef struct {
	uint8_t integer_part;
	uint8_t fractional_part;
} osCpuRate_t;

typedef struct {
	uint32_t mcycle;
	uint32_t minstret;
} osCpuPerf_t;

typedef struct
{
    ubase_t core;
    const char *file;
    ubase_t line;
    void *pc;
} osAssert_t;

extern osAssert_t *g_osAssert;

extern volatile uint8_t g_osIrqNo;

typedef void (*osOutputFunc_t)(const char *str);

extern osOutputFunc_t SHELL_GetOutputFunc(void);

/**
 * @addtogroup KernelObject
 */

/**@{*/

/*
 * kernel object interface
 */
OS_INLINE void osObjectInit(struct osObject         *object,
                    enum osObjectClassType type)
{
    /* set object type to static */
    object->type = type | osObject_Class_Static;
    object->flag = 0x0;
    object->attr = 0x0;
}

OS_INLINE void osObjectDetach(osObject_t object)
{
    /* reset object type */
    object->type = 0;
}

osObject_t osObjectAllocate(enum osObjectClassType type, uint32_t object_size);
void osObjectDestroy(osObject_t object);

OS_INLINE bool_t osObjectIsSystemobject(osObject_t object)
{
    /* object check */
    if (object->type & osObject_Class_Static)
        return OS_TRUE;

    return OS_FALSE;
}

OS_INLINE uint8_t osObjectGetType(osObject_t object)
{
    /* object check */
    return object->type & ~osObject_Class_Static;
}

/**@}*/

/**
 * @addtogroup Clock
 */

/**@{*/

/*
 * clock & timer interface
 */
uint32_t osGetSysTimeCnt(void);
void osHrtimerCheck(void);
void osTickIncrease(void);
void osTimerSystemInit(void);
void osTimerSystemThreadInit(void);
void osHrtimerSystemInit(void);

osTick_t osTickGet(void);
void osTickSet(osTick_t tick);
osTick_t osTickFromMillisecond(int32_t ms);
osTick_t osTickFromMsRelaxed(uint32_t ms);
uint32_t osGetCpuCycle(void);
uint64_t osGetCpuCycle64(void);
uint64_t osGetCpuInstret64(void);

osStatus_t osTimerDetach(osTimer_t timer);
osStatus_t osTimerStartEX(osTimer_t timer);
osStatus_t osTimerControl(osTimer_t timer, int cmd, void *arg);

osTick_t osTimerNextTimeoutTick(void);
void osTimerCheck(void);

#ifdef OS_USING_HOOK
void osTimerSetEnterHook(void (*hook)(struct osTimer *timer));
void osTimerSetExitHook(void (*hook)(struct osTimer *timer));
#endif

/**@}*/

/**
 * @addtogroup Thread
 */

/**@{*/

/*
 * thread interface
 */
void osThreadCleanup(struct osThread *thread_cb);
osStatus_t osThreadDeinit(osThread_t thread);
osThread_t osThreadSelf(void);
/**
 ************************************************************************************
 * @brief           当前运行线程睡眠，让出cpu，支持睡眠唤醒
 *
 * @param[in]       tick    睡眠时长，单位：tick
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osThreadSleep(uint32_t tick);

/**
 ************************************************************************************
 * @brief           使用relaxed方式，当前运行线程睡眠，让出cpu
 *
 * @param[in]       tick            睡眠时长，单位：tick
 * @param[in]       relaxed_tick    relaxed时长，单位：tick
                                    relaxed_tick等于osWaitForever，不支持睡眠唤醒
                                    relaxed_tick等于0，支持睡眠唤醒，等同于osThreadSleep
                                    relaxed_tick等于其他值，tick + relaxed_tick时长后唤醒
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osThreadSleepRelaxed(uint32_t tick, uint32_t relaxed_tick);
osStatus_t osThreadSleepUntil(osTick_t tick);
osStatus_t osThreadSleepUntilRelaxed(osTick_t tick, uint32_t relaxed_tick);
osStatus_t osThreadControl(osThread_t thread, int cmd, void *arg);
osStatus_t osThreadSuspendEX(osThread_t thread);
osStatus_t osThreadResumeEX(osThread_t thread);
void osThreadTimeout(void *parameter);

#ifdef OS_USING_SIGNALS
void osThreadAllocSig(osThread_t tid);
void osThreadFreeSig(osThread_t tid);
int  osThreadKill(osThread_t tid, int sig);
#endif

#ifdef OS_USING_HOOK
void osThreadSetSuspendHook(void (*hook)(osThread_t thread));
void osThreadSetResumeHook (void (*hook)(osThread_t thread));
void osThreadSetInitedHook (void (*hook)(osThread_t thread));
#endif

uint32_t osGetCurContextId(void);
uint32_t osGetCurContext(void);
osList_t *osThreadGetList(void);
uint32_t osThreadGetMsgCount(osThreadId_t thread_id);

/*
 * idle thread interface
 */
void osIdleInit(void);
#if defined(OS_USING_HOOK) || defined(OS_USING_IDLE_HOOK)
osStatus_t osIdleSetHook(void (*hook)(void));
osStatus_t osIdleDelHook(void (*hook)(void));
#endif
void osIdleExcute(void);
osThread_t osIdleGetHandler(void);

/*
 * schedule service
 */
void osSchedulerSystemInit(void);
void osSchedulerSystemStart(void);

void osSchedule(void);
void osSchedulerInsertThread(struct osThread *thread);
void osSchedulerRemoveThread(struct osThread *thread);

void osCriticalEnter(void);
void osCriticalExit(void);
uint16_t osCriticalLevel(void);

#ifdef OS_USING_HOOK
void osSchedulerSetHook(void (*hook)(osThread_t from, osThread_t to));
#endif

osStatus_t osStartCpuMonitor(void);
osStatus_t osStopCpuMonitor(void);
osStatus_t osReadCpuMonitor(osCpuRate_t *cpu_rate);
osStatus_t osStopCpuMonitorSimple(void);

/**@}*/

/**
 * @addtogroup Signals
 * @{
 */
#ifdef OS_USING_SIGNALS
void osSignalMask(int signo);
void osSignalUnmask(int signo);
osSigHandler_t osSignalInstall(int signo, osSigHandler_t handler);
int osSignalWait(const osSigSet_t *set, osSigInfo_t *si, int32_t timeout);

int osSignalSystemInit(void);
#endif
/*@}*/

/**
 * @addtogroup MM
 */

/**@{*/

/*
 * memory management interface
 */
#ifdef OS_USING_MEMPOOL
/*
 * memory pool interface
 */
osStatus_t osMemoryPoolnit(struct osMempool *mp,
                    void            *start,
                    size_t          size,
                    size_t          block_size);
osStatus_t osMemoryPoolDetach(struct osMempool *mp);

#ifdef OS_USING_HOOK
void osMpSetAllocHook(void (*hook)(struct osMempool *mp, void *block));
void osMpSetFreeHook(void (*hook)(struct osMempool *mp, void *block));
#endif

#endif

#ifdef OS_USING_HEAP
#ifdef OS_USING_HOOK
void osMallocSetHook(void (*hook)(void *ptr, size_t size));
void osFreeSetHook(void (*hook)(void *ptr));
#endif
#endif

/*
 * memory interface
 */
void osSystemMemInit(void);
void osApShareBufInit(void);
void osApUpNodeInit(void);
void osCpHarqBufInit(void);
void osCpHarqBufDeInit(void);
void osApModemShareBufInit(void);
void osApModemShareBufDeInit(void);
bool_t osIsCpModemShareBufFree(void);
void osCpModemShareBufInit(void);
void osCpModemShareBufDeInit(void);
void osModemShareBufInit(void);

/**@}*/


/*
 * interrupt service
 */

/*
 * osInterruptEnter and osInterruptLeave only can be called by BSP
 */
void osInterruptEnter(void);
void osInterruptLeave(void);

/*
 * the number of nested interrupts.
 */
uint8_t osInterruptGetNest(void);

#ifdef OS_USING_INTERRUPT_HOOK
void osInterruptSetEnterHook(void (*hook)(uint32_t));
void osInterruptSetLeaveHook(void (*hook)(uint32_t));
#endif

#ifdef OS_USING_COMPONENTS_INIT
void osComponentsInit(void);
void osComponentsBoardInit(void);
#endif

/**
 * @addtogroup IPC
 */

/**@{*/

/**
 * This function will initialize an IPC object
 *
 * @param ipc the IPC object
 *
 */
OS_INLINE void osIpcObjectInit(struct osIpcObject *ipc)
{
    /* initialize ipc object */
    osListInit(&(ipc->suspendThread));
}

/**
 * This function will suspend a thread to a specified list. IPC object or some
 * double-queue object (mailbox etc.) contains this kind of list.
 *
 * @param list the IPC suspended thread list
 * @param thread the thread object to be suspended
 * @param flag the IPC object flag,
 *        which shall be OS_IPC_FLAG_FIFO/OS_IPC_FLAG_PRIO.
 *
 * @return the operation status, osOK on successful
 */
OS_INLINE osStatus_t osIpcListSuspend(osList_t        *list,
                                       struct osThread *thread,
                                       uint8_t        flag)
{
    /* suspend thread */
    osThreadSuspendEX(thread);

    switch (flag)
    {
    case OS_IPC_FLAG_FIFO:
        osListInsertBefore(list, &(thread->tlist));
        break;

    case OS_IPC_FLAG_PRIO:
        {
            struct osListNode *n;
            struct osThread *sthread;

            /* find a suitable position */
            for (n = list->next; n != list; n = n->next)
            {
                sthread = osListEntry(n, struct osThread, tlist);

                /* find out */
                if (thread->currentPriority < sthread->currentPriority)
                {
                    /* insert this thread before the sthread */
                    osListInsertBefore(&(sthread->tlist), &(thread->tlist));
                    break;
                }
            }

            /*
             * not found a suitable position,
             * append to the end of suspendThread list
             */
            if (n == list)
                osListInsertBefore(list, &(thread->tlist));
        }
        break;

    default:
        break;
    }

    return osOK;
}

/**
 * This function will resume the first thread in the list of a IPC object:
 * - remove the thread from suspend queue of IPC object
 * - put the thread into system ready queue
 *
 * @param list the thread list
 *
 * @return the operation status, osOK on successful
 */
OS_INLINE osStatus_t osIpcListResume(osList_t *list)
{
    struct osThread *thread;

    /* get thread entry */
    thread = osListEntry(list->next, struct osThread, tlist);
    //OS_DEBUG_LOG(OS_DEBUG_IPC, ("resume thread:%s\n", thread->name));

    /* resume it */
    osThreadResumeEX(thread);

    return osOK;
}

/**
 * This function will resume all suspended threads in a list, including
 * suspend list of IPC object and private list of mailbox etc.
 *
 * @param list of the threads to resume
 *
 * @return the operation status, osOK on successful
 */
OS_INLINE osStatus_t osIpcListResumeAll(osList_t *list)
{
    struct osThread *thread;
    register ubase_t temp;

    /* wakeup all suspended threads */
    while (!osListIsEmpty(list))
    {
        /* disable interrupt */
        temp = osInterruptDisable();

        /* get next suspended thread */
        thread = osListEntry(list->next, struct osThread, tlist);
        /* set error code to osError */
        thread->error = osError;

        /*
         * resume thread
         * In osThreadResumeEX function, it will remove current thread from
         * suspended list
         */
        osThreadResumeEX(thread);

        /* enable interrupt */
        osInterruptEnable(temp);
    }

    return osOK;
}

#ifdef OS_USING_THREAD_TRACE
void osThreadSwapin(osThread_t thread);
#endif
#ifdef OS_USING_IRQ_TRACE
void osIrqSwapout(uint32_t irq);
void osIrqSwapin(uint32_t irq);
#endif

void osReadArchPerf(void);

void osAssertInit(void);
void SHELL_showThread(char argc, char **argv);
void osShowErrorInfo(void);
void osShowBackTrace(void);

/**@}*/

#ifdef __cplusplus
}
#endif
#endif
