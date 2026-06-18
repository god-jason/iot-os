/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_icp.h
 *
 * @brief       ICP驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team         创建
 ************************************************************************************
 */

#ifndef _DRV_ICP_H_
#define _DRV_ICP_H_

#include <os.h>
#include <drv_common.h>

/**
 * @addtogroup Icp
 */

/**@{*/

#define ICP_ERR_BUF_NOT_ENOUGH     (DRV_ERR_SPECIFIC - 1) // 接收数据缓存空间不够
#define ICP_ERR_OTHER              (DRV_ERR_SPECIFIC - 2) // 其他错误

/**
 * @brief ICP中断位ID
 * @note  ICP_IntID有效范围为[2~ICP_INT_ID_MAX], 0和1为内部预留, 对应ICP_INT_ID_MAX取值最大为ICP_MSG_MAX_INT_BIT.
 */
typedef enum
{
    ICP_INT_ID_MIN  = 2,
    ICP_INT_ID_RAMDUMP = ICP_INT_ID_MIN,
    ICP_INT_ID_WAKEUP,
    ICP_INT_ID_PSRAMDFS,
    ICP_INT_ID_LOG_WAKEUP,
    ICP_INT_ID_MAX,
} ICP_IntID;

typedef void (*ICP_Callback)(uint16_t msgId, void *data, uint16_t len);

/**
 * @brief ICP消息ID
 * @note  ICP_MsgID有效范围为[0~255], 对应ICP_MSG_ID_MAX取值最大为256
 */
typedef enum
{
    ICP_MSG_ID_MIN = 0,
    ICP_MSG_ID_PSM = ICP_MSG_ID_MIN,
    ICP_MSG_ID_ADC,
    ICP_MSG_ID_CONSOLE,
#ifdef USE_CATCH_DATA
    ICP_MSG_ID_CATCH_DATA,
    ICP_MSG_ID_HDLC_CONTROL,
#endif

    ICP_MSG_ID_MAX,
} ICP_MsgID;

typedef struct
{
    ICP_MsgID           msgId;
    ICP_Callback        cbFunc;
} ICP_MsgCallbackTable;

/**
 ***********************************************************************************************************************
 * @brief           接收同步消息. 非线程安全, 非阻塞
 *
 * @param[out]      msgId                      返回接收的同步消息ID
 * @param[in]       data                       接收数据指针, 用户申请和释放
 * @param[in]       len                        接收数据长度
 *
 * @return          接收结果.
 * @retval          >=0                        接收成功，返回实际接收的数据长度
 *                  DRV_ERR_EMPTY              接收失败，核间消息已读空
 *                  ICP_ERR_BUF_NOT_ENOUGH     接收失败，用户传入的数据缓存不足够保存接收到的同步消息
 *                  ICP_ERR_OTHER              接收失败，其他错误
 ***********************************************************************************************************************
 */
int32_t ICP_RecvSyncMsg(uint16_t *msgId, uint8_t *data, uint16_t len);

/**
 ***********************************************************************************************************************
 * @brief           发送同步消息. 线程安全, 内部存在获取互斥锁阻塞操作. 
 * @note            发送前，用户需要查询对端核是否唤醒。如未唤醒，需调用ICP_SendInterrupt进行唤醒.
 *
 * @param[in]       msgId                       消息ID
 * @param[in]       data                        数据指针
 * @param[in]       len                         数据长度
 *
 * @return          发送结果.
 * @retval          DRV_OK                      发送成功
 *                  DRV_ERR_FULL                发送失败, 核间消息已满，无法写入. 用户需要自行缓存同步消息.
 *                  DRV_ERR_PARAMETER           发送失败, 无效输入参数
 ***********************************************************************************************************************
 */
int32_t ICP_SendSyncMsg(uint16_t msgId, uint8_t *data, uint16_t len);

/**
 ***********************************************************************************************************************
 * @brief           发送异步消息. 线程安全, 内部存在获取互斥锁阻塞操作, 发送异步消息会触发对端核产生ICP中断
 * @note            
 *
 * @param[in]       msgId                       消息ID
 * @param[in]       data                        数据指针
 * @param[in]       len                         数据长度
 *
 * @return          发送结果.
 * @retval          DRV_OK                      发送成功
 *                  DRV_ERR_FULL                发送失败，核间消息已满，无法写入. 用户需要自行缓存异步消息.
 *                  DRV_ERR_PARAMETER           发送失败, 无效输入参数
 ***********************************************************************************************************************
 */
int32_t ICP_SendAsyncMsg(uint16_t msgId, uint8_t *data, uint16_t len);

/**
 ***********************************************************************************************************************
 * @brief           注册异步消息接收回调. 只能注册一次.
 * @note            1) 异步回调在ICP中断中调用，用户需保证回调中无阻塞性操作.
 *                  2) 异步回调中用户需要将buf拷贝走，回调完成buf会被释放
 *
 * @param[in]       callback                    异步消息接收回调
 *
 * @return          无.
 * @retval
 ***********************************************************************************************************************
 */
void ICP_RegisterAsyncMsgCallBack(ICP_Callback callback);

/**
 ***********************************************************************************************************************
 * @brief           发送Icp中断. 线程安全, 非阻塞, 用于RAMDUMP、WAKEUP等仅需要触发中断的场景.
 *                  ICP_IntID会独立分配ICP中断中的一个中断位. ICP支持最多(ICP_MSG_MAX_INT_BIT + 1)个ICP中断位. 异步消息和普通ICP消息已占用两个.
 *
 * @param[in]       icpInt                      ICP中断位ID
 *
 * @return          无
 *
 * @note
 ***********************************************************************************************************************
 */
void ICP_SendInterrupt(ICP_IntID icpInt);

/**
 ***********************************************************************************************************************
 * @brief           发送ICP消息，并触发对端核产生ICP中断. 线程安全, 内部存在获取互斥锁阻塞操作, 用于需要触发中断并携带最大4个字节消息内容的场景.
 *                  ICP消息共用ICP中断中的一个中断位.
 *
 * @param[in]       msgId                       ICP消息ID
 * @param[in]       data                        数据指针. 可为NULL, 最大长度为4字节
 * @param[in]       len                         数据长度. 最小为0字节, 最大为4字节
 *
 * @return          发送结果.
 * @retval          DRV_OK                      发送成功
 *                  DRV_ERR_FULL                发送失败，核间消息已满，无法写入
 *                  DRV_ERR_PARAMETER           发送失败, 无效输入参数
 *
 * @note
 ***********************************************************************************************************************
 */
int32_t ICP_SendMsg(ICP_MsgID msgId, uint8_t *data, uint8_t len);

/**
 ***********************************************************************************************************************
 * @brief           同步消息是否为空.
 *
 * @param[in]       void
 *
 * @return          结果.
 * @retval          OS_TRUE                     空
 *                  OS_FALSE                    非空
 *
 * @note
 ***********************************************************************************************************************
 */
bool_t ICP_SyncMsgIsEmpty(void);

/**
 ***********************************************************************************************************************
 * @brief           异步消息是否为空.
 *
 * @param[in]       void
 *
 * @return          结果.
 * @retval          OS_TRUE                     空
 *                  OS_FALSE                    非空
 *
 * @note
 ***********************************************************************************************************************
 */
bool_t ICP_AsyncMsgIsEmpty(void);

/**
 ***********************************************************************************************************************
 * @brief           Mask Icp中断.
 *
 * @param[in]       icpInt                      ICP中断位ID
 *
 * @return          无.
 *
 * @note
 ***********************************************************************************************************************
 */
void ICP_MaskInterrupt(ICP_IntID icpInt);

/**
 ***********************************************************************************************************************
 * @brief           Unmask Icp中断.
 *
 * @param[in]       icpInt                      ICP中断位ID
 *
 * @return          无.
 *
 * @note
 ***********************************************************************************************************************
 */
void ICP_UnmaskInterrupt(ICP_IntID icpInt);

#endif /* End of _DRV_ICP_H_ */
/** @} */