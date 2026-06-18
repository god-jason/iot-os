/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */

#ifndef __AT_COMMAND_MIDDLEWARE_H__
#define __AT_COMMAND_MIDDLEWARE_H__

#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void AT_E_Set(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_E_Exec(uint8_t channelId);

void AT_CMEE_Set(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_CMEE_Read(uint8_t channelId);
void AT_CMEE_Test(uint8_t channelId);

void AT_NetDevCtl_Set(uint8_t channelId, uint8_t *param, uint16_t paramLen);
void AT_NetDevCtl_Read(uint8_t channelId);
void AT_NetDevCtl_Test(uint8_t channelId);

void AT_QNetDevCtl_StatusCallback(uint8_t cid, uint8_t status);
void AT_QNetDevCtl_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_QNetDevCtl_Test(uint8_t channelId);
void AT_QNetDevCtl_Read(uint8_t channelId);

void AT_AndW_Set(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_AndW_Exec(uint8_t channelId);

void AT_CMD_TestExec(uint8_t channelId);
void AT_CMD_TestSet(uint8_t channelId, uint8_t *cmd, uint16_t cmdLen);
void AT_CMD_TestRead(uint8_t channelId);
void AT_CMD_TestTest(uint8_t channelId);
#ifdef __cplusplus
}
#endif
#endif


