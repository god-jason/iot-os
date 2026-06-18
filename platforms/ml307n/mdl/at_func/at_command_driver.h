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

#ifndef __AT_COMMAND_DRIVER_H__
#define __AT_COMMAND_DRIVER_H__

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
void AT_DrvCfgSet(uint8_t iChannelId, uint8_t *pPara, uint16_t iParaLen);
void AT_DrvCfgRead(uint8_t iChannelId);
void AT_DrvCfgTest(uint8_t iChannelId);
void AT_PlatCfgSet(uint8_t iChannelId, uint8_t *pPara, uint16_t iParaLen);
void AT_PlatCfgRead(uint8_t iChannelId);
void AT_PlatCfgTest(uint8_t iChannelId);
#ifdef OS_USING_SHELL
void AT_ShellSet(uint8_t iChannelId, uint8_t *pPara, uint16_t iParaLen);
#endif
void AT_AudioSet(uint8_t ch_id, uint8_t *p_para, uint16_t para_len);
void AT_ChipId_Exec(uint8_t channelId);
void AT_ChipInfo_Exec(uint8_t channelId);
void AT_PowerOnType_Exec(uint8_t channelId);

#ifdef __cplusplus
}
#endif
#endif


