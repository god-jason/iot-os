/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        slog_print.h
 *
 * @brief       通用打印接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-19     ICT Team         创建
 ************************************************************************************
 */

#ifndef _SLOG_PRINT_H_
#define _SLOG_PRINT_H_

#include "slog.h"

/**
 * @addtogroup Slog
 */

/**@{*/

/**
 * @brief subModule范围:0~63
 */
//@SUBMODULE(SLOG_MODULE_PRINT){{
#define SLOG_PRINT_SUBMDL_PUB           (0)
#define SLOG_PRINT_SUBMDL_OS            (1)
#define SLOG_PRINT_SUBMDL_DRV           (2)
#define SLOG_PRINT_SUBMDL_AGT           (3)
#define SLOG_PRINT_SUBMDL_PSM           (4)
#define SLOG_PRINT_SUBMDL_PLAT          (5)
#define SLOG_PRINT_SUBMDL_TEST          (6)
#define SLOG_PRINT_SUBMDL_APP           (7)
#define SLOG_PRINT_SUBMDL_MUX           (8)
#define SLOG_PRINT_SUBMDL_OTA           (9)
#define SLOG_PRINT_SUBMDL_IMS           (10)
#define SLOG_PRINT_SUBMDL_MID_PUB       (11)
#define SLOG_PRINT_SUBMDL_MID_AT        (12)
#define SLOG_PRINT_SUBMDL_MID_NET       (13)
#define SLOG_PRINT_SUBMDL_MID_OTA       (14)
#define SLOG_PRINT_SUBMDL_MID_MUX       (15)
#define SLOG_PRINT_SUBMDL_UICC          (16)
#define SLOG_PRINT_SUBMDL_MID_MQTT      (17)
#define SLOG_PRINT_SUBMDL_UI_FRAME      (18)
#define SLOG_PRINT_SUBMDL_AT            (19)
#define SLOG_PRINT_SUBMDL_SVC           (20)
#define SLOG_PRINT_SUBMDL_PS            (21)
#define SLOG_PRINT_SUBMDL_22            (22)
#define SLOG_PRINT_SUBMDL_23            (23)
#define SLOG_PRINT_SUBMDL_24            (24)
#define SLOG_PRINT_SUBMDL_25            (25)
#define SLOG_PRINT_SUBMDL_26            (26)
#define SLOG_PRINT_SUBMDL_27            (27)
#define SLOG_PRINT_SUBMDL_28            (28)
#define SLOG_PRINT_SUBMDL_29            (29)
#define SLOG_PRINT_SUBMDL_30            (30)
#define SLOG_PRINT_SUBMDL_31            (31)
#define SLOG_PRINT_SUBMDL_32            (32)
#define SLOG_PRINT_SUBMDL_33            (33)
#define SLOG_PRINT_SUBMDL_34            (34)
#define SLOG_PRINT_SUBMDL_35            (35)
#define SLOG_PRINT_SUBMDL_36            (36)
#define SLOG_PRINT_SUBMDL_37            (37)
#define SLOG_PRINT_SUBMDL_38            (38)
#define SLOG_PRINT_SUBMDL_39            (39)
#define SLOG_PRINT_SUBMDL_40            (40)
#define SLOG_PRINT_SUBMDL_41            (41)
#define SLOG_PRINT_SUBMDL_42            (42)
#define SLOG_PRINT_SUBMDL_43            (43)
#define SLOG_PRINT_SUBMDL_44            (44)
#define SLOG_PRINT_SUBMDL_45            (45)
#define SLOG_PRINT_SUBMDL_46            (46)
#define SLOG_PRINT_SUBMDL_47            (47)
#define SLOG_PRINT_SUBMDL_48            (48)
#define SLOG_PRINT_SUBMDL_49            (49)
#define SLOG_PRINT_SUBMDL_ONEOS         (50)
#define SLOG_PRINT_SUBMDL_51            (51)
#define SLOG_PRINT_SUBMDL_52            (52)
#define SLOG_PRINT_SUBMDL_53            (53)
#define SLOG_PRINT_SUBMDL_54            (54)
#define SLOG_PRINT_SUBMDL_55            (55)
#define SLOG_PRINT_SUBMDL_56            (56)
#define SLOG_PRINT_SUBMDL_57            (57)
#define SLOG_PRINT_SUBMDL_58            (58)
#define SLOG_PRINT_SUBMDL_59            (59)
#define SLOG_PRINT_SUBMDL_IP_PACKET     (60)
#define SLOG_PRINT_SUBMDL_61            (61)
#define SLOG_PRINT_SUBMDL_62            (62)
#define SLOG_PRINT_SUBMDL_63            (63)
//}}

/**
 ***********************************************************************************************************************
 * @brief           通用打印接口.
 *
 * @param[in]       level                      打印等级.SLOG_LEVEL_DEBUG ~ SLOG_LEVEL_ERROR
 * @param[in]       submdl                     打印消息的模块.范围0~63, SLOG_PRINT_SUBMDL_*等
 * @param[in]       format                     打印格式化字符串. 支持如下格式:
 *                                             %d、%ld、%lld、%u、%lu、%llu、%o、%x、%X、
 *                                             %f、%lf、%llf、%c、%s、%p、%.*s、%*s、%*.*s
 *
 * @return          打印结果.
 * @retval          >=0                        成功
 *                  <0                         失败
 * @note            使用说明如下
 *                  示例: 打印5个参数写法如下
 *                  slogPrintf(level, submdl, "string %d, %d, %d, %d, %d", param1, param2, param3, param4, param5);
 ***********************************************************************************************************************
 */
#define slogPrintf(level, submdl, format, ...) SLOG_Kprintf(level, submdl, format, ##__VA_ARGS__)

/**
 ***********************************************************************************************************************
 * @brief           Dump数据接口.
 *
 * @param[in]       level                      打印等级.SLOG_LEVEL_DEBUG ~ SLOG_LEVEL_ERROR
 * @param[in]       submdl                     打印消息的模块.范围0~63, SLOG_PRINT_SUBMDL_*等
 * @param[in]       dump                       dump数据的内容
 * @param[in]       dumpLen                    dump数据的长度
 *
 * @return          打印结果.
 * @retval          >=0                        成功
 *                  <0                         失败
 * @note            IP数据包导出使用说明如下:
 *                  示例: slogDump(level, SLOG_PRINT_SUBMDL_IP_PACKET, dump, dumpLen);
 ***********************************************************************************************************************
 */
#define slogDump(level, submdl, dump, dumpLen) SLOG_Dump(level, submdl, dump, dumpLen);

#if defined(SLOG_SUPPORT_FORMAT_PRINTF)
/**
 ***********************************************************************************************************************
 * @brief           格式化打印接口。对于slogPrintf不支持的format格式打印, 可使用该接口, 与osPrintf支持的format一致。
 *
 * @param[in]       format                     the format.
 *
 * @return          打印结果.
 * @retval          >=0                        成功
 *                  <0                         失败
 * @note            使用建议：该接口格式化字符串会输出到LOG BUF中, 占用内存, 同时格式化字符串也增加版本大小，不建议使用。
 *                  优先使用slogPrintf(格式化字符串不占用内存)。
 *                  仅在slogPrintf不支持的format格式打印中，才使用SLOG_FormatPrintf接口。
 ***********************************************************************************************************************
 */
int32_t SLOG_FormatPrintf(const char *format, ...);
#define slogFormatPrintf(format, ...)   SLOG_FormatPrintf(format, ##__VA_ARGS__);
#endif

#endif /* End of _SLOG_PRINT_H_ */

/** @} */