/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        slog.h
 *
 * @brief       打印接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-19     ICT Team         创建
 ************************************************************************************
 */

#ifndef _SLOG_H_
#define _SLOG_H_

#include <stddef.h>
#include <stdint.h>
#include <os_def.h>

/**
 * @addtogroup Slog
 */

/**@{*/

#define SLOG_LEVEL_DEBUG   (0x0)
#define SLOG_LEVEL_INFO    (0x1)
#define SLOG_LEVEL_WARN    (0x2)
#define SLOG_LEVEL_ERROR   (0x3)

#define SLOG_MODULE_PS     (0x0)
#define SLOG_MODULE_PHY    (0x1)
#define SLOG_MODULE_PRINT  (0x2)
#define SLOG_MODULE_PLAT   (0x3)

/**
 * @brief 根据module、subModule、msgId生成不带打印等级的打印ID
 * @note  module范围：      SLOG_MODULE_PS ~ SLOG_MODULE_PLAT
 *        subModule范围:    0~63
 *        msgId范围:        0~0xFFFF
 */
#define SLOG_PRINT_ID_NO_LEVEL(module, subModule, msgId)  \
    ((((uint32_t)(module)&0x3)<<28) | (((uint32_t)(subModule)&0x3F)<<22) | ((uint32_t)(msgId)&0xFFFF))

/**
 *************************************************************************************
 * 打印消息ID定义
 * 用一个32bit来定义打印消息ID
 * 四个字节的定义如下：
 * |    XX   |   XX  |           XX XXXX          |XX XXXX|  XXXX XXXX XXXX XXXX  |
 * | 打印等级 |大类划分|   每个大类一级子模块划分     | 空闲  |   一级子模块消息ID划分  |
 *   DEBUG       PS    每个大类最大支持64种一级子模块  空闲    每个子模块消息ID空间范围:
 *   INFO        PHY                                            0x0~0xFFFF
 *   WARN        PRINT
 *   ERROR       PLAT
 ************************************************************************************
 */

/**
 ***********************************************************************************************************************
 * @brief           打印接口.
 *
 * @param[in]       level                      打印等级.SLOG_LEVEL_DEBUG ~ SLOG_LEVEL_ERROR
 * @param[in]       printId                    打印消息ID, 可用SLOG_PRINT_ID_NO_LEVEL生成
 * @param[in]       paraNum                    可变入参个数, 最大支持10个入参
 *
 * @return          打印结果.
 * @retval          >=0                        成功
 *                  <0                         失败
 * @note            使用说明如下
 *                  示例: 打印5个参数写法如下
 *                  //{{test:param1=%d, param2=%d, param3=%d, param4=%d, param5=%d}}
 *                  SLOG_Printf(level, printId, 5, param1, param2, param3, param4, param5);
 ***********************************************************************************************************************
 */
int32_t SLOG_Printf(uint8_t level, uint32_t printId, uint8_t paraNum, ...);

/**
 ***********************************************************************************************************************
 * @brief           通用打印接口.
 *
 * @param[in]       level                      打印等级.SLOG_LEVEL_DEBUG ~ SLOG_LEVEL_ERROR
 * @param[in]       submdl                     打印消息的模块.范围0~63
 * @param[in]       format                     打印格式化字符串. 支持如下格式:
 *                                             %d、%ld、%lld、%u、%lu、%llu、%o、%x、%X、
 *                                             %f、%lf、%llf、%c、%s、%p
 *
 * @return          打印结果.
 * @retval          >=0                        成功
 *                  <0                         失败
 * @note            使用说明如下
 *                  示例: 打印5个参数写法如下
 *                  SLOG_Kprintf(level, submdl, "string %d, %d, %d, %d, %d", param1, param2, param3, param4, param5);
 ***********************************************************************************************************************
 */
int32_t SLOG_Kprintf(uint32_t level, uint32_t submdl, char *format, ...);

/**
 ***********************************************************************************************************************
 * @brief           Dump数据接口.
 *
 * @param[in]       level                      打印等级.SLOG_LEVEL_DEBUG ~ SLOG_LEVEL_ERROR
 * @param[in]       submdl                     打印消息的模块.范围0~63
 * @param[in]       dump                       dump数据的内容
 * @param[in]       dumpLen                    dump数据的长度
 *
 * @return          打印结果.
 * @retval          >=0                        成功
 *                  <0                         失败
 * @note
 ***********************************************************************************************************************
 */
int32_t SLOG_Dump(uint32_t level, uint32_t submdl, uint8_t *dump, uint16_t dumpLen);

#if defined(_CPU_AP)
/**
 ***********************************************************************************************************************
 * @brief           协议栈标准LOG是否跟踪.
 *
 * @return          是否跟踪.
 * @retval          非0                       跟踪
 *                  0                         不跟踪
 ***********************************************************************************************************************
 */
bool_t SLOG_IsStdLogTraced(void);

/**
 ***********************************************************************************************************************
 * @brief           协议栈标准LOG打印接口.
 *
 * @param[in]       level                      打印等级.SLOG_LEVEL_DEBUG ~ SLOG_LEVEL_ERROR
 * @param[in]       printId                    打印消息ID, 可用SLOG_PRINT_ID_NO_LEVEL生成
 * @param[in]       reserved                   保留字段. 填充SLOG_StdHeader中的reserved
 * @param[in]       codeData                   编码数据指针
 * @param[in]       codeLen                    编码数据长度
 *
 * @return          接收结果.
 * @retval          >=0                        成功
 *                  <0                         失败
 ***********************************************************************************************************************
 */
int32_t SLOG_StdLogOutput(uint8_t level, uint32_t printId, uint16_t reserved, uint8_t *codeData, uint16_t codeLen);
#else
/**
 ***********************************************************************************************************************
 * @brief           物理层标准LOG打印接口.
 *
 * @param[in]       level                      打印等级.SLOG_LEVEL_DEBUG ~ SLOG_LEVEL_ERROR
 * @param[in]       printId                    打印消息ID, 可用SLOG_PRINT_ID_NO_LEVEL生成
 * @param[in]       data                       数据指针
 * @param[in]       len                        数据长度
 *
 * @return          接收结果.
 * @retval          >=0                        成功
 *                  <0                         失败
 ***********************************************************************************************************************
 */
int32_t SLOG_StdLogOutput(uint8_t level, uint32_t printId, uint8_t* data, uint16_t len);

/**
 ***********************************************************************************************************************
 * @brief           检查物理层LOG输出, 需要1ms调用一次, 否则LOG输出会存在异常
 *
 * @note            1) LOG发送, LOG空闲块更新, LOG使用时间戳更新, 等等
                    2) 检查物理层LOG区域, 超时触发LOG发送, 防止LOG区域大小一直未满足LOG发送的最小阈值
 *                  3) 物理层LOG打印异常信息统计和输出
 *
 * @return          无.
 ***********************************************************************************************************************
 */
void SLOG_CheckPhyLogOut(void);
#endif

#endif /* End of _SLOG_H_ */
/** @} */