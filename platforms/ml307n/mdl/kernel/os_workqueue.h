/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_workqueue.h
 *
 * @brief     WORK接口.
 *
 * @revision  1.0
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version   <th>Author     <th>Description
 * <tr><td>2023-04-21  <td>1.0       <td>geanfeng   <td>初始版本
 * </table>
 ************************************************************************************
 */

#ifndef __OS_WORKQUEUE_H__
#define __OS_WORKQUEUE_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <os_def.h>
#include <cmsis_os2.h>

/************************************************************************************
 *                                 配置
 ************************************************************************************/

/**@name 工作队列数量配置
* @{
*/
#define OS_CONFIG_WORK_QUEUE_COUNT           (1)   ///< 队列数量
/** @}*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

#define OS_WORK_INITED                  0x55
#define OS_WORK_DEINITED                0xAA

/**@name 工作状态
 * @{
 */
#define OS_WORK_STAGE_IDLE       0x0    ///< IDLE
#define OS_WORK_STAGE_PENDING    0x1    ///< PENDING
#define OS_WORK_STAGE_DELAY      0x2    ///< DELAY
/** @}*/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

struct osWorkQueue;

/**
 * @brief  osWork
 */
struct osWork
{
    struct osListNode    workNode;
    struct osWorkQueue  *workQueue;     ///< Workqueue used to execute work.
    osTimerId_t          delayTimer;

    void (*func)(void *data);           ///< Callback func for work.
    void (*complete)(void *data);       ///< Callback complete for work.
    void *data;                         ///< Private data for callback function.

    uint8_t flag;
    uint8_t process;

    uint8_t inited;                     ///< If os_work is inited, value is OS_KOBJ_INITED.
};
typedef struct osWork *osWork_t;

/**
 * @brief  osWorkQueue
 */
struct osWorkQueue
{
    struct osListNode   workList;

    osWork_t            workCurrent;    ///< Work in progress on workqueue.
    osThreadId_t        workThreadId;   ///< Thread on the workqueue to execute work.

    uint32_t            workCount;
};
typedef struct osWorkQueue *osWorkQueue_t;

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void _osWorkQueueThread(osWorkQueue_t workQueue);
void _osWorkNew(osWorkQueue_t workQueue, osWork_t work, bool_t addFirst);

/**
 ************************************************************************************
 * @brief           初始化任务
 *
 * @param[in]       work             任务指针
 * @param[in]       run              任务run函数
 * @param[in]       complete         任务complete函数
 * @param[in]       data             任务函数入参
 * @param[in]       delayWork        是否为延时任务
 *
 * @return          void
 ************************************************************************************
*/
void osWorkInitEx(osWork_t work, void (*run)(void *data), void (*complete)(void *data), void *data, bool_t delayWork);

#define osWorkInit(work, run, data, delayWork) osWorkInitEx(work, run, OS_NULL, data, delayWork)

/**
 ************************************************************************************
 * @brief           去初始化任务
 *
 * @param[in]       work             任务指针
 *
 * @return          void
 ************************************************************************************
*/
void osWorkDeinit(osWork_t work);

/**
 ************************************************************************************
 * @brief           提交任务到工作队列
 *
 * @param[in]       work             任务指针
 * @param[in]       ticks            提交任务的延时
 *
 * @return          int32_t
 * @retval          osOK             成功
 * @retval          osErrorBusy      任务忙，已经在调度队列中
 ************************************************************************************
*/
int32_t osWorkSubmit(osWork_t work, uint32_t ticks);

/**
 ************************************************************************************
 * @brief           提交任务到工作队列，当前任务执行结束后处理
 *
 * @param[in]       work             任务指针
 *
 * @return          int32_t
 * @retval          osOK             成功
 * @retval          osErrorBusy      任务忙，已经在调度队列中
 ************************************************************************************
*/
int32_t osWorkSubmitLazy(osWork_t work);

/**
 ************************************************************************************
 * @brief           取消任务，不等待正在执行中的任务结束
 *
 * @param[in]       work             任务指针
 *
 * @return          void
 * @note            注意:应先取消提交任务的源头，再取消该任务；
 ************************************************************************************
*/
void osWorkCancel(osWork_t work);

/**
 ************************************************************************************
 * @brief           取消任务，会等待正在执行中的任务结束
 *
 * @param[in]       work             任务指针
 *
 * @return          void
 * @note            注意:应先取消提交任务的源头，再取消该任务
 ************************************************************************************
*/
void osWorkCancelSync(osWork_t work);

/**
 ************************************************************************************
 * @brief           获取当前线程对应的工作队列的当前工作
 *
 * @param[in]       void
 *
 * @return          如果当前上下文是中断上下文，返回NULL
                    如果当前线程不是工作队列线程，返回NULL
                    其他情况返回工作队列的当前工作
 ************************************************************************************
*/
osWork_t osWorkGetCurrent(void);

/**
 ************************************************************************************
 * @brief           初始化任务队列
 *
 * @return          int
 * @retval          0       成功
 * @retval          其他      失败
 ************************************************************************************
*/
int osWorkQueueInit(void);

#endif /* __OS_WORKQUEUE_H__ */
