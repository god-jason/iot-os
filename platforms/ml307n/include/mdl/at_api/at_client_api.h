/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief OpenCPU发送AT命令API
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */

#ifndef __AT_CLIENT_H__
#define __AT_CLIENT_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup AtClient
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           处理AT命令响应的回调函数
 *
 * @param[in]       channelId        通道ID
 * @param[in]       rsp              命令响应内容
 * @param[in]       rspLen           命令响应长度
 * @param[in]       userdata         附加用户数据
 *
 * @return none
 ************************************************************************************
*/
typedef void (*at_client_cmd_rsp_cb)(uint8_t channelId, char *rsp, uint32_t rspLen, void *userdata);
/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           在OpenCPU时发送一个AT命令并处理命令响应
 *
 * @param[in]       channelId        要发送命令的通道, 0表示不指定通道, 正常取值范围[AT_CHANNEL_ID_OC_BEGIN : AT_CHANNEL_ID_OC_END]
 * @param[in]       cmd              要执行的AT命令，命令以\r或\r\n结束
 * @param[in]       cmdLen           命令长度, 包括命令结束符
 * @param[in]       isBlock          是否阻塞, true表示接口会阻塞直到命令最终响应; false 不阻塞
 * @param[in]       rspCallback      处理命令响应的回调函数
 * @param[in]       userdata         附加用户数据, 传递给rspCallback
 *
 * @return 0 命令发送成功
 * @note   当isBlock为true, 表示阻塞接口, 不能在AT命令处理函数,主动上报回调函数及rspCallback中调用
 * @note   channelId 指定通道建议在命令响应时使用，且用回调函数携带的通道
 ************************************************************************************
*/
int32_t AT_Client_ChannelSendCommand(uint8_t channelId, char *cmd, uint32_t cmdLen, bool isBlock, at_client_cmd_rsp_cb rspCallback, void *userdata);

/**
 ************************************************************************************
 * @brief           在OpenCPU时发送一个AT命令并通过回调函数处理命令响应
 *
 * @param[in]       cmd              要执行的AT命令，命令以\r或\r\n结束
 * @param[in]       cmdLen           命令长度, 包括命令结束符
 * @param[in]       isBlock          是否阻塞, true表示接口会阻塞直到命令最终响应; false 不阻塞
 * @param[in]       rspCallback      处理命令响应的回调函数
 * @param[in]       userdata         附加用户数据, 传递给rspCallback
 *
 * @return 0 命令发送成功
 * @note   当isBlock为true, 表示阻塞接口, 不能在AT命令处理函数,主动上报回调函数及rspCallback中调用
 ************************************************************************************
*/
int32_t AT_Client_SendCommand(char *cmd, uint32_t cmdLen, bool isBlock, at_client_cmd_rsp_cb rspCallback, void *userdata);

/**
 ************************************************************************************
 * @brief           在OpenCPU时发送一个AT命令并等待命令执行完成
 *
 * @param[in]       cmd  要执行的AT命令字符串，命令以\r或\r\n结束，cmd中的数据要以‘\0’结束
 *
 * @return 0 命令发送并执行成功，< 0 命令发送失败，> 0 命令执行失败时的错误码
 * @note   该接口阻塞执行，直到AT命令最终响应上报，适用配置命令
 *         不能在AT命令处理函数和主动上报回调函数中调用
 ************************************************************************************
*/
int32_t AT_Client_SendCommandWaitResult(char *cmd);
#ifdef __cplusplus
}
#endif
#endif
/** @} */
