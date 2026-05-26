/**
 * @file        cm_sys.h
 * @brief       系统管理
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By XGL
 * @date        2021/04/18
 *
 * @defgroup sys sys
 * @ingroup SYS
 * @{
 */

#ifndef __CM_SYS_H__
#define __CM_SYS_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "slog_print.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CM_IMEI_LEN      16 /*!< IMEI存储长度 */
#define CM_SN_LEN        65 /*!< SN存储长度 */
#define CM_VER_LEN       65 /*!< 版本号存储长度 */

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * @brief 获取模组IMEI
 *
 * @param [out] imei IMEI，长度16字节
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details More details
 */
int32_t cm_sys_get_imei(char *imei);

/**
 * @brief 获取模组SN
 *
 * @param [out] sn SN，长度32字节
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details More details
 */
int32_t cm_sys_get_sn(char *sn);

/**
 * @brief 获取SDK版本号
 *
 * @param [in] ver_buff 存储版本号的内存指针
 * @param [in] len 传入的内存长度，单位字节
 *
 * @return
 *   >= 0  - 获取到的版本号长度，单位字节 \n
 *   < 0  - 失败, 返回值为错误码
 */
int32_t cm_sys_get_cm_ver(char *ver_buff, uint32_t len);

/**
 * @brief 打印log信息
 *
 * @param [in] log_level log等级,暂不支持，传入任意值即可
 * @param [in] fmt 格式化字符串
 * @param [in] ... 格式化字符串参数
 *
 * @return 空
 *
 * @details 本接口内部会申请一段180字节长度的buff用于组包log（不可传入超过180字节的数据。超过180字节时，接口会检查到数据错误，不会执行log打印） \n
 *          log通过DEBUG串口（DBG_RX DBG_TX）输出。本平台的DEBUG口为串口，频繁使用串口打印可能会影响系统功能 \n
 *          因为底层默认配置，此接口最多支持打印128字节（含OpenCPU标识“[ cmlog ][OPENCPU]:”）的log，故用户只能用此接口打印不超过108字节的log
 */
#define cm_log_printf(log_level, fmt, ...)  slogPrintf(SLOG_LEVEL_INFO,SLOG_PRINT_SUBMDL_ONEOS,fmt,##__VA_ARGS__)

#undef EXTERN
#ifdef __cplusplus
}
#endif

/** @}*/
/** @}*/

#endif /* __CM_SYS_H__ */

