/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_ext.h
 *
 * @brief     os扩展头文件.
 *
 * @revision  1.0
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version   <th>Author     <th>Description
 * <tr><td>2023-07-06  <td>1.0       <td>ICT Team   <td>初始版本
 * </table>
 ************************************************************************************
 */

#ifndef __OS_EXT_H__
#define __OS_EXT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup OsExt
 */

/**@{*/

/**
 * Tick 转换为毫秒
 */
#define osMsFromTick(tick)  ((tick)*1000/OS_TICK_PER_SECOND)

/**
 * 毫秒转换为 Tick
 */
#define osTickFromMs(ms)    (((ms) + 1000/OS_TICK_PER_SECOND - 1)/(1000/OS_TICK_PER_SECOND))

/**
 * 微秒转换为 Tick
 */
#define osTickFromUs(us)    (((us) + USEC_PER_SEC/OS_TICK_PER_SECOND - 1)/(USEC_PER_SEC/OS_TICK_PER_SECOND))

#define OS_MEM_TRACE_MAGIC_UP       (0x1212)
#define OS_MEM_TRACE_MAIGC_DOWN     (0x89)

#define OS_RESET_INFO_BUF_LEN   (2048)

typedef struct
{
    uint32_t    user;
    size_t      size;
    const char  *file;
    uint16_t    line;
    uint16_t    magic;
} osMemTrace_t;

typedef struct
{
    uint32_t    user;
    size_t      size;
    const char  *file;
    uint32_t    line;
    void        *ptr;
    uint32_t    time;
} osMemRecord_t;

typedef struct osDlmalloc {
  uint8_t num; /* number of av_ */
  uint8_t reserved[3]; /* reserved */
  char* base; /* start address of dlmalloc */
  int total;    /* total space allocated from system */
  int free_chunks;  /* number of non-inuse chunks */
  int used; /* total allocated space */
  int max_used; /* max allocated space */
  int user_used; /* total used space for user */
  int free; /* total non-inuse space */
  int top_size; /* top-most, releasable (via malloc_trim) space */
  int max_chunk; /* max size chunk, include top */
  uint64_t failed_num; /* alloc falied number */
} osDlmalloc_t;

/**
 ************************************************************************************
 * @brief           静态创建并初始化定时器
 *
 * @param[in]       timer       定时器结构体指针，空间由调用者提供
 * @param[in]       timeout     定时器回调函数指针
 * @param[in]       parameter   回调函数入参
 * @param[in]       time        定时器初始超时时长，单位：tick
 * @param[in]       flag        等待队列标志，OS_IPC_FLAG_FIFO或OS_IPC_FLAG_PRIO
 *
 * @return          void.
 ************************************************************************************
 */
void osTimerInit(struct osTimer *timer,
                   void (*timeout)(void *parameter),
                   void       *parameter,
                   uint32_t   time,
                   uint8_t  flag);

/**
 ************************************************************************************
 * @brief           启动或者重启定时器，支持睡眠唤醒，仅支持hardtimer和softimer
 *
 * @param[in]       timer_id        定时器id，通过osTimerNew获取
 * @param[in]       ms              定时器时长，单位：毫秒
 *
 * @return          状态码.
 ************************************************************************************
 */
#define osTimerStartMs(timer, ms)  osTimerStart((timer), osTickFromMs(ms))

/**
 ************************************************************************************
 * @brief           使用relaxed方式启动或者重启定时器，仅支持hardtimer和softimer
 *
 * @param[in]       timer_id        定时器id，通过osTimerNew获取
 * @param[in]       ms              定时器时长，单位：毫秒
 * @param[in]       relaxed_ms      relaxed时长，单位：毫秒
 *
 * @return          状态码.
 ************************************************************************************
 */
#define osTimerStartRelaxedMs(timer, ms, relaxed_ms)  osTimerStartRelaxed((timer), osTickFromMs(ms), osTickFromMsRelaxed(relaxed_ms))

/**
 ************************************************************************************
 * @brief           根据定时器id获取定时器当前剩余时长
 *
 * @param[in]       timer_id    定时器id
 *
 * @return          定时器当前剩余时长，单位：毫秒.
 ************************************************************************************
 */
uint64_t osTimerGetRemainMs(osTimer_t timer_id);

#ifdef OS_USING_PM
/**
 ************************************************************************************
 * @brief           获取协议栈最近到期的省电睡眠唤醒定时器信息
 *
 * @param[in]       void.
 * @param[out]      msg_id      消息id指针
 *
 * @return          定时器当前剩余时长，单位：毫秒.
 ************************************************************************************
 */
uint64_t osTimerGetPsRecentTimer(uint32_t *msg_id);
#endif

/**
 ************************************************************************************
 * @brief           静态创建并初始化线程
 *
 * @param[in]       thread      线程结构体指针，空间由调用者提供
 * @param[in]       name        线程名称，不要超过15字节
 * @param[in]       entry       线程入口函数
 * @param[in]       parameter   线程入口函数参数
 * @param[in]       stackStart  线程栈起始地址，空间由调用者提供
 * @param[in]       stackSize   线程栈大小
 * @param[in]       priority    线程优先级，0~32，0：优先级最高，32：优先级最低
 * @param[in]       tick        线程时间片，单位：tick
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osThreadInit(struct osThread *thread,
                        const char       *name,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stackStart,
                        uint32_t       stackSize,
                        uint8_t        priority,
                        uint32_t       tick);
/**
 ************************************************************************************
 * @brief           启动线程
 *
 * @param[in]       thread      线程id，通过osThreadNew或者osThreadGetId获取
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osThreadStart(osThread_t thread);

/**
 ************************************************************************************
 * @brief           获取下一个唤醒定时器时长
 *
 * @param[in]       void
 *
 * @return          唤醒定时器超时tick
 ************************************************************************************
 */
osTick_t osTimerListNextWakeUpTimeout(void);

/**
 ************************************************************************************
 * @brief           获取下一个定时器时长
 *
 * @param[in]       void
 *
 * @return          定时器超时tick
 ************************************************************************************
 */
osTick_t osTimerListNextTimeout(void);

/**
 ************************************************************************************
 * @brief           当前运行线程睡眠，让出cpu，支持睡眠唤醒
 *
 * @param[in]       ms      睡眠时长，单位：毫秒
 *
 * @return          状态码.
 ************************************************************************************
 */
#define osThreadMsSleep(ms)   osThreadSleep(osTickFromMs(ms))

/**
 ************************************************************************************
 * @brief           使用relaxed方式，当前运行线程睡眠，让出cpu
 *
 * @param[in]       ms          睡眠时长，单位：毫秒
 * @param[in]       relaxed_ms  relaxed时长，单位：毫秒
                                relaxed_ms等于osWaitForever，不支持睡眠唤醒
                                relaxed_ms等于0，支持睡眠唤醒，等同于osThreadMsSleep
                                relaxed_ms等于其他值，ms + relaxed_ms时长后唤醒
                                睡眠唤醒时间精度：毫秒
 *
 * @return          状态码.
 ************************************************************************************
 */
#define osThreadMsSleepRelaxed(ms, relaxed_ms)   osThreadSleepRelaxed(osTickFromMs(ms), osTickFromMsRelaxed(relaxed_ms))

/**
 ************************************************************************************
 * @brief           判断线程是否存在
 *
 * @param[in]       thread      线程id，通过osThreadNew或者osThreadGetId获取
 *
 * @return          线程是否存在.
 * @retval          OS_TRUE     存在.
 * @retval          OS_FALSE    不存在.
 ************************************************************************************
 */
bool_t osThreadCheckExist(osThreadId_t *thread);

#if defined(OS_USING_AP_SYSTEM_MEM) || defined(OS_USING_CP_SYSTEM_MEM)

#ifdef OS_USING_MEM_TRACE
extern void *osMallocTrace(size_t size, const char *file, uint32_t line);
extern void *osReallocTrace(void *ptr, size_t size, const char *file, uint32_t line);
extern void *osCallocTrace(size_t count, size_t size, const char *file, uint32_t line);
extern void *osMallocAlignTrace(size_t size, size_t align, const char *file, uint32_t line);
extern void *osMallocAlignNCTrace(size_t size, size_t align, const char *file, uint32_t line);
extern void osFreeTrace(void *ptr, const char *file, uint32_t line);
extern void osFreeAlignTrace(void *ptr, const char *file, uint32_t line);

#define osMalloc(size)                  osMallocTrace(size, __FILE__, __LINE__)
#define osRealloc(ptr, size)            osReallocTrace(ptr, size, __FILE__, __LINE__)
#define osCalloc(count, size)           osCallocTrace(count, size, __FILE__, __LINE__)
#define osMallocAlign(size, align)      osMallocAlignTrace(size, align, __FILE__, __LINE__)
#define osMallocAlignNC(size, align)    osMallocAlignNCTrace(size, align, __FILE__, __LINE__)
#define osFree(ptr)                     osFreeTrace(ptr, __FILE__, __LINE__)
#define osFreeAlign(ptr)                osFreeAlignTrace(ptr, __FILE__, __LINE__)

#else

/**
 ************************************************************************************
 * @brief           系统内存申请，可跨线程使用，不可跨核使用，性能高，地址区域开cache
 *
 * @param[in]       size      大小
 *
 * @return          内存地址.
 ************************************************************************************
 */
void *osMalloc(size_t size);

/**
 ************************************************************************************
 * @brief           系统内存调整
 *
 * @param[in]       ptr     要调整的内存地址
 * @param[in]       size    要调整的内存大小
 *
 * @return          调整后的内存地址.
 ************************************************************************************
 */
void *osRealloc(void *ptr, size_t size);

/**
 ************************************************************************************
 * @brief           系统内存分配并初始化为0
 *
 * @param[in]       count     对象个数
 * @param[in]       size      对象大小
 *
 * @return          内存地址.
 ************************************************************************************
 */
void *osCalloc(size_t count, size_t size);

/**
 ************************************************************************************
 * @brief           系统内存地址对齐申请
 *
 * @param[in]       size        大小
 * @param[in]       align       对齐大小，单位：字节
 *
 * @return          align对齐后的系统内存地址（cache地址）.
 ************************************************************************************
 */
void *osMallocAlign(size_t size, size_t align);

/**
 ************************************************************************************
 * @brief           系统内存地址对齐申请
 *
 * @param[in]       size        大小
 * @param[in]       align       对齐大小，单位：字节
 *
 * @return          align对齐后的系统内存地址（noncache地址）.
 ************************************************************************************
 */
void *osMallocAlignNC(size_t size, size_t align);

/**
 ************************************************************************************
 * @brief           系统内存释放
 *
 * @param[in]       ptr         内存地址，通过osMalloc获取
 *
 * @return          void.
 ************************************************************************************
 */
void osFree(void *ptr);

/**
 ************************************************************************************
 * @brief           系统内存地址对齐释放
 *
 * @param[in]       ptr         内存地址，通过osMallocAlign获取
 *
 * @return          void.
 ************************************************************************************
 */
void osFreeAlign(void *ptr);

#endif

/**
 ************************************************************************************
 * @brief           获取系统内存当前使用率
 *
 * @param[in]       void
 *
 * @return          使用率，值0~100，0表示使用率0%，100表示使用率100%.
 ************************************************************************************
 */
uint8_t osGetSystemMemUsedRate(void);

/**
 ************************************************************************************
 * @brief           获取系统内存当前状态
 *
 * @param[in]       stat    系统内存状态指针
 *
 * @return          void.
 ************************************************************************************
 */
void osGetSysMemStat(osDlmalloc_t *stat);

#endif

#ifdef OS_USING_AP_SHAREBUF

#ifdef OS_USING_MEM_TRACE
extern void *osGetApShareBufTrace(size_t size, const char *file, uint32_t line);
extern void *osGetApShareBufDownTrace(size_t size, const char *file, uint32_t line);

#define osGetApShareBuf(size)           osGetApShareBufTrace(size, __FILE__, __LINE__)
#define osGetApShareBufDown(size)       osGetApShareBufDownTrace(size, __FILE__, __LINE__)

#else
/**
 ************************************************************************************
 * @brief           AP share buf申请接口，申请大小32B~1600B，总大小DRAM_BASE_LEN_AP_SHARE，
 *                  返回地址32字节对齐，可跨线程使用，不可跨核使用，性能高，
 *                  地址区域开cache
 *
 * @param[in]       size        大小
 *
 * @return          32字节对齐的内存地址.
 ************************************************************************************
 */
void *osGetApShareBuf(size_t size);

#endif

/**
 ************************************************************************************
 * @brief           AP share buf释放接口
 *
 * @param[in]       ptr         内存地址，通过osGetApShareBuf获取
 *
 * @return          void.
 ************************************************************************************
 */
void osRetApShareBuf(void *ptr);

void osCheckApShareBuf(void *ptr);

/**
 ************************************************************************************
 * @brief           获取AP share buf当前使用率
 *
 * @param[in]       void
 *
 * @return          使用率，值0~100，0标识使用率0%，100表示使用率100%.
 ************************************************************************************
 */
uint8_t osGetApShareBufUsedRate(void);

/**
 ************************************************************************************
 * @brief           获取AP share buf当前剩余内存数量
 *
 * @param[in]       void
 *
 * @return          剩余内存数量，单位：字节.
 ************************************************************************************
 */
uint32_t osGetApShareBufRemain(void);

#endif

#ifdef OS_USING_CP_HARQBUF

#ifdef OS_USING_MEM_TRACE
extern void *osGetHarqBufTrace(size_t size, const char *file, uint32_t line);

#define osGetHarqBuf(size)   osGetHarqBufTrace(size, __FILE__, __LINE__)

#else

/**
 ************************************************************************************
 * @brief           CP harq buf申请接口，申请大小1B~16KB，总大小IRAM_BASE_LEN_HARQ，
 *                  返回地址32字节对齐，可跨线程使用，不可跨核使用，性能高，
 *                  地址区域non-cache
 *
 * @param[in]       size        大小
 *
 * @return          32字节对齐的内存地址.
 ************************************************************************************
 */
void *osGetHarqBuf(size_t size);

#endif

/**
 ************************************************************************************
 * @brief           CP harq buf释放接口
 *
 * @param[in]       ptr         内存地址，通过osGetHarqBuf获取
 *
 * @return          void.
 ************************************************************************************
 */
void osRetHarqBuf(void *ptr);

/**
 ************************************************************************************
 * @brief           获取CP harq buf当前使用率
 *
 * @param[in]       void
 *
 * @return          使用率，值0~100，0标识使用率0%，100表示使用率100%.
 ************************************************************************************
 */
uint8_t osGetHarqBufUsedRate(void);

#endif

#ifdef OS_USING_AP_UPNODE

#ifdef OS_USING_MEM_TRACE
extern void *osUpNodeAllocTrace(size_t size, const char *file, uint32_t line);

#define osUpNodeAlloc(size)     osUpNodeAllocTrace(size, __FILE__, __LINE__)

#else
/**
 ************************************************************************************
 * @brief           用户面节点空间申请接口，申请大小1B~64B，总大小IRAM_BASE_LEN_UP_NODE，
 *                  可跨线程使用，不可跨核使用，性能高，
 *                  申请失败从自动从系统内存申请，用户感知不到，地址区域开cache
 * @param[in]       size      大小
 *
 * @return          内存地址.
 ************************************************************************************
 */
void *osUpNodeAlloc(size_t size);

#endif

/**
 ************************************************************************************
 * @brief           用户面节点空间释放接口
 *
 * @param[in]       ptr         内存地址，通过osUpNodeAlloc获取
 *
 * @return          void.
 ************************************************************************************
 */
void osUpNodeFree(void *ptr);

#endif

#ifdef OS_USING_MODEM_SHAREBUF

#ifdef OS_USING_MEM_TRACE
extern void *osGetApModemShareBufTrace(size_t size, const char *file, uint32_t line);
extern void *osGetApModemShareBufAlignTrace(size_t size, size_t align, const char *file, uint32_t line);
extern void *osGetCpModemShareBufTrace(size_t size, const char *file, uint32_t line);
extern void *osGetCpModemShareBufAlignTrace(size_t size, size_t align, const char *file, uint32_t line);

#define osGetApModemShareBuf(size)                osGetApModemShareBufTrace(size, __FILE__, __LINE__)
#define osGetApModemShareBufAlign(size, align)    osGetApModemShareBufAlignTrace(size, align, __FILE__, __LINE__)
#define osGetCpModemShareBuf(size)                osGetCpModemShareBufTrace(size, __FILE__, __LINE__)
#define osGetCpModemShareBufAlign(size, align)    osGetCpModemShareBufAlignTrace(size, align, __FILE__, __LINE__)

#else
/**
 ************************************************************************************
 * @brief           Moderm Share Buffer申请接口，申请大小1B~1600B，总大小DRAM_BASE_LEN_MODEM_SHARE，
 *                  可跨线程使用，可跨核使用，性能低，地址区域non-cache，优先从AP侧获取
 * @param[in]       size        大小
 *
 * @return          内存地址.
 ************************************************************************************
 */
void *osGetApModemShareBuf(size_t size);

/**
 ************************************************************************************
 * @brief           Moderm Share Buffer对齐申请接口，申请大小1B~1600B，总大小
 *                  DRAM_BASE_LEN_AP_MODEM_SHARE+DRAM_BASE_LEN_CP_MODEM_SHARE，
 *                  可跨线程使用，可跨核使用，性能低，地址区域non-cache，优先从AP侧获取
 * @param[in]       size        大小
 *
 * @return          内存地址.
 ************************************************************************************
 */
void *osGetApModemShareBufAlign(size_t size, size_t align);
/**
 ************************************************************************************
 * @brief           Moderm Share Buffer申请接口，申请大小1B~1600B，总大小
 *                  DRAM_BASE_LEN_AP_MODEM_SHARE+DRAM_BASE_LEN_CP_MODEM_SHARE，
 *                  可跨线程使用，可跨核使用，性能低，地址区域non-cache，优先从CP侧获取
 * @param[in]       size        大小
 *
 * @return          内存地址.
 ************************************************************************************
 */
void *osGetCpModemShareBuf(size_t size);

/**
 ************************************************************************************
 * @brief           Moderm Share Buffer对齐申请接口，申请大小1B~1600B，总大小
 *                  DRAM_BASE_LEN_AP_MODEM_SHARE+DRAM_BASE_LEN_CP_MODEM_SHARE，
 *                  可跨线程使用，可跨核使用，性能低，地址区域non-cache，优先从CP侧获取
 * @param[in]       size        大小
 *
 * @return          内存地址.
 ************************************************************************************
 */
void *osGetCpModemShareBufAlign(size_t size, size_t align);

#endif

/**
 ************************************************************************************
 * @brief           Moderm Share Buffer释放接口
 *
 * @param[in]       ptr         内存地址，通过osGetApModemShareBuf
 *                              或者osGetCpModemShareBuf获取
 *
 * @return          void.
 ************************************************************************************
 */
void osRetApModemShareBuf(void *ptr);

/**
 ************************************************************************************
 * @brief           Moderm Share Buffer对齐释放接口
 *
 * @param[in]       ptr         内存地址，通过osGetApModemShareBufAlign
 *                              或者osGetCpModemShareBufAlign获取
 *
 * @return          void.
 ************************************************************************************
 */
void osRetApModemShareBufAlign(void *ptr);

/**
 ************************************************************************************
 * @brief           Moderm Share Buffer释放接口
 *
 * @param[in]       ptr         内存地址，通过osGetApModemShareBuf
 *                              或者osGetCpModemShareBuf获取
 *
 * @return          void.
 ************************************************************************************
 */
void osRetCpModemShareBuf(void *ptr);

/**
 ************************************************************************************
 * @brief           Moderm Share Buffer对齐释放接口
 *
 * @param[in]       ptr         内存地址，通过osGetApModemShareBufAlign
 *                              或者osGetCpModemShareBufAlign获取
 *
 * @return          void.
 ************************************************************************************
 */
void osRetCpModemShareBufAlign(void *ptr);

/**
 ************************************************************************************
 * @brief           获取Moderm Share Buffer当前使用率
 *
 * @param[in]       void
 *
 * @return          使用率，值0~100，0标识使用率0%，100表示使用率100%.
 ************************************************************************************
 */
uint8_t osGetModemShareBufUsedRate(void);

#endif


/**
 * @addtogroup IPC
 */

/**@{*/

#ifdef OS_USING_SEMAPHORE
/*
 * semaphore interface
 */
/**
 ************************************************************************************
 * @brief           静态创建并初始化信号量
 *
 * @param[in]       sem         信号量指针，空间由调用者提供
 * @param[in]       value       信号量初始计数值
 * @param[in]       max_value   信号量最大计数值
 * @param[in]       flag        等待队列标志，OS_IPC_FLAG_FIFO或OS_IPC_FLAG_PRIO
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osSemaphoreInit(struct osSemaphore *sem, uint32_t value, uint32_t max_value, uint8_t flag);
/**
 ************************************************************************************
 * @brief           去初始化信号量，不释放资源
 *
 * @param[in]       sem     信号量指针
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osSemaphoreDetach(osSem_t sem);

/*
 * completion interface
 */
OS_INLINE void osInitCompletion(osCompletion *cmpl)
{
    osSemaphoreInit(cmpl, 0, OS_SEM_VALUE_MAX, OS_IPC_FLAG_PRIO);
}

OS_INLINE osStatus_t osWaitForCompletion(osCompletion *cmpl, int32_t timeout)
{
    return osSemaphoreAcquire(cmpl, timeout);
}

OS_INLINE osStatus_t osComplete(osCompletion *cmpl)
{
    return osSemaphoreRelease(cmpl);
}

/**
 *  ReInitCompletion simply resets the value field to 0 ("not done"),
 *  Callers of this function must make sure that there no racy WaitForCompletion and Complete
 *  calls going on in parallel.
 */
OS_INLINE void osReInitCompletion(osCompletion *cmpl)
{
    cmpl->value = 0;
}

#endif

#ifdef OS_USING_MUTEX
/*
 * mutex interface
 */
/**
 ************************************************************************************
 * @brief           静态创建并初始化互斥体
 *
 * @param[in]       mutex   互斥体指针，空间由调用者提供
 * @param[in]       flag    等待队列标志，OS_IPC_FLAG_FIFO或OS_IPC_FLAG_PRIO
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osMutexInit(struct osMutex *mutex, uint8_t flag);
osStatus_t osMutexDetach(osMutex_t mutex);
#endif

#ifdef OS_USING_EVENT
/*
 * event interface
 */
/**
 ************************************************************************************
 * @brief           静态创建并初始化信号量
 *
 * @param[in]       event   信号量指针，空间由调用者提供
 * @param[in]       flag    等待队列标志，OS_IPC_FLAG_FIFO或OS_IPC_FLAG_PRIO
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osEventFlagsInit(struct osEvent *event, uint8_t flag);
osStatus_t osEventFlagsDetach(osEvent_t event);
#endif

#ifdef OS_USING_MAILBOX
/*
 * mailbox interface
 */
osStatus_t osMbInit(struct osMailbox *mb,
                    void        *msgpool,
                    size_t    size,
                    uint8_t   flag);
osStatus_t osMbDetach(osMb_t mb);
osMb_t osMbCreate(size_t size, uint8_t flag);
osStatus_t osMbDestory(osMb_t mb);
osStatus_t osMbSend(osMb_t mb, ubase_t  value, int32_t timeout);
osStatus_t osMbSendRelaxed(osMb_t mb, ubase_t value, int32_t timeout, int32_t relaxed_timeout);
osStatus_t osMbUrgent(osMb_t mb, ubase_t value);
osStatus_t osMbRecv(osMb_t mb, ubase_t *value, int32_t timeout);
osStatus_t osMbRecvRelaxed(osMb_t mb, ubase_t *value, int32_t timeout, int32_t relaxed_timeout);

void _osMbDestory(osMb_t mb);
osStatus_t _osMbSendRelaxed(osMb_t mb, const void *ptr, size_t size, int32_t timeout, int32_t relaxed_timeout);
osStatus_t _osMbSendFront(osMb_t mb, const void *ptr, size_t size, int32_t timeout);
osStatus_t _osMbUrgent(osMb_t mb, const void *ptr, size_t size);
osStatus_t _osMbRecvRelaxed(osMb_t mb, void *ptr, size_t size, int32_t timeout, int32_t relaxed_timeout);
uint32_t _osMbGetSpace(osMb_t mb);
uint32_t _osMbGetCount(osMb_t mb);

/**
 ************************************************************************************
 * @brief           线程间发送消息
 *
 * @param[in]       msg_id      消息id
 * @param[in]       buf         消息指针
 * @param[in]       size        消息大小
 * @param[in]       thread_id   目的线程id
 * @param[in]       time_ms     超时时长，单位：毫秒
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osMsgSend(uint32_t msg_id, void *buf, uint16_t size, osThread_t thread_id, int32_t time_ms);

/**
 ************************************************************************************
 * @brief           线程间发送紧急消息，会被接收线程优先处理
 *
 * @param[in]       msg_id      消息id
 * @param[in]       buf         消息指针
 * @param[in]       size        消息大小
 * @param[in]       thread_id   目的线程id
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osMsgUrgent(uint32_t msg_id, void *buf, uint16_t size, osThread_t thread_id);

/**
 ************************************************************************************
 * @brief           线程间接收消息
 *
 * @param[out]      msg_id          消息id指针，不能为NULL
 * @param[out]      buf             消息指针的指针，可以为NULL
 * @param[out]      size            消息大小指针，可以为NULL
 * @param[out]      msg_sender      消息发送者，可以为NULL
 * @param[in]       time_ms         超时时长，单位：毫秒
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osMsgRecv(uint32_t *msg_id, void **buf, uint16_t *size, osThread_t *msg_sender, int32_t time_ms);

/**
 ************************************************************************************
 * @brief           设置非周期性消息定时器，定时器超时后给设置的线程发送消息
 *
 * @param[in]       time_ms     定时器时长，单位：毫秒
 * @param[in]       relaxed_ms  relaxed时长，单位：毫秒
 * @param[in]       msg_id      消息id
 * @param[in]       arg         附加参数
 *
 * @return          定时器id或NULL.
 * @retval          定时器id           创建成功
 *                  NULL            创建失败
 ************************************************************************************
 */
osTimer_t osMsgSetRelativeTimer(uint32_t time_ms, uint32_t relaxed_ms, uint32_t msg_id, uint32_t arg);

#if 0
/**
 ************************************************************************************
 * @brief           设置非周期性消息定时器，定时器超时后给目的线程发送消息
 *
 * @param[in]       time_ms     定时器时长，单位：毫秒
 * @param[in]       msg_id      消息id
 * @param[in]       arg         附加参数
 * @param[in]       thread_id   目的线程id
 *
 * @return          定时器id或NULL.
 * @retval          定时器id           创建成功
 *                  NULL            创建失败
 ************************************************************************************
 */
osTimer_t osMsgSetRelativeTimerByThread(uint32_t time_ms, uint32_t msg_id, uint32_t arg, osThread_t thread_id);

/**
 ************************************************************************************
 * @brief           设置周期性消息定时器，定时器超时后给设置的线程发送消息
 *
 * @param[in]       time_ms     定时器时长，单位：毫秒
 * @param[in]       msg_id      消息id
 * @param[in]       arg         附加参数
 *
 * @return          定时器id或NULL.
 * @retval          定时器id           创建成功
 *                  NULL            创建失败
 ************************************************************************************
 */
osTimer_t osMsgSetLoopTimer(uint32_t time_ms, uint32_t msg_id, uint32_t arg);

/**
 ************************************************************************************
 * @brief           设置周期性消息定时器，定时器超时后给目的线程发送消息
 *
 * @param[in]       time_ms     定时器时长，单位：毫秒
 * @param[in]       msg_id      消息id
 * @param[in]       arg         附加参数
 * @param[in]       thread_id   目的线程id
 *
 * @return          定时器id或NULL.
 * @retval          定时器id           创建成功
 *                  NULL            创建失败
 ************************************************************************************
 */
osTimer_t osMsgSetLoopTimerByThread(uint32_t time_ms, uint32_t msg_id, uint32_t arg, osThread_t thread_id);
#endif

#endif

#ifdef OS_USING_MESSAGEQUEUE
/*
 * message queue interface
 */
/**
 ************************************************************************************
 * @brief           静态创建并初始化消息队列
 *
 * @param[in]       mq          消息队列结构体指针，空间由调用者提供
 * @param[in]       msgpool     消息队列消息空间指针，空间由调用者提供
 * @param[in]       msg_size    最大消息大小
 * @param[in]       pool_size   消息空间总大小
 * @param[in]       flag        等待队列标志，OS_IPC_FLAG_FIFO或OS_IPC_FLAG_PRIO
 *
 * @return          状态码.
 ************************************************************************************
 */
osStatus_t osMessageQueueInit(struct osMessageQueue *mq,
                    void       *msgpool,
                    size_t   msg_size,
                    size_t   pool_size,
                    uint8_t  flag);
osStatus_t osMessageQueueDetach(osMq_t mq);
osStatus_t osMessageQueueUrgent(osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio);
osStatus_t osMessageQueueControl(osMq_t mq, int cmd, void *arg);
#endif

/**@}*/

/**
 ************************************************************************************
 * @brief           关机接口
 *
 * @param[in]       type       关机类型: 关机或重启
 * @param[in]       reason     重启原因
 *
 * @return          无.
 * @note            函数会创建关机任务，函数返回不会立即关机
 ************************************************************************************
 */
void osShutdown(osShutdownType_t type, osRebootReason_t reason);

/**
 * @addtogroup KernelService
 */

/**@{*/

/*
 * general kernel service
 */
void osPrintf(const char *fmt, ...);
void osPuts(const char *str);

int32_t osVsprintf(char *dest, const char *format, va_list arg_ptr);
int32_t osVsnprintf(char *buf, size_t size, const char *fmt, va_list args);
int32_t osSprintf(char *buf, const char *format, ...);
int32_t osSnprintf(char *buf, size_t size, const char *format, ...);

osStatus_t osGetErrno(void);
void osSetErrno(osStatus_t no);
int *_osErrno(void);
#if !defined(OS_USING_NEWLIB) && !defined(_WIN32)
#ifndef os_errno
#define os_errno    *_osErrno()
#endif
#endif

int _osFfs(int value);

void *osMemset(void *src, int c, ubase_t n);
void *osMemcpy(void *dest, const void *src, ubase_t n);

int32_t osStrncmp(const char *cs, const char *ct, ubase_t count);
int32_t osStrcmp(const char *cs, const char *ct);
size_t osStrlen(const char *src);
size_t osStrnlen(const char *s, ubase_t maxlen);
char *osStrdup(const char *s);
#if defined(__CC_ARM) || defined(__CLANG_ARM)
/* leak strdup interface */
char* strdup(const char* str);
#endif

char *osStrstr(const char *str1, const char *str2);
int32_t osSscanf(const char *buf, const char *fmt, ...);
char *osStrncpy(char *dest, const char *src, ubase_t n);
void *osMemmove(void *dest, const void *src, ubase_t n);
int32_t osMemcmp(const void *cs, const void *ct, ubase_t count);
int32_t osStrcasecmp(const char *a, const char *b);

void osShowVersion(void);
osRebootReason_t osGetBootReason(void);
void osSetBootReason(osRebootReason_t reason);

#ifdef OS_DEBUG
extern void (*g_osAssertHook)(const char *file, uint32_t line);
void osAssertSetHook(void (*hook)(const char *file, uint32_t line));

void osAssertHandler(const char *file, uint32_t line);
#endif /* OS_DEBUG */

/**
 ************************************************************************************
 * @brief           系统软重启
 *
 * @param[in]       void
 *
 * @return          void
 ************************************************************************************
 */
void osSysSoftReset(void);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/** @} */

