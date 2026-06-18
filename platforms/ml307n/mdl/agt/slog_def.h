/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        slog_def.h
 *
 * @brief       SLOG内部定义头文件.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-19     ICT Team         创建
 ************************************************************************************
 */

#ifndef _SLOG_DEF_H_
#define _SLOG_DEF_H_

#include <stddef.h>
#include <stdint.h>
#include "os.h"

/**@name 通用返回值定义
 * @{
 */

/**@{*/

#define SLOG_OK                     0    ///< 操作成功
#define SLOG_ERR                   -1    ///< 未知错误
#define SLOG_ERR_TIMEOUT           -2    ///< 操作超时
#define SLOG_ERR_UNSUPPORTED       -3    ///< 不支持的操作
#define SLOG_ERR_PARAMETER         -4    ///< 参数错误
#define SLOG_ERR_FULL              -5    ///< 满状态
#define SLOG_ERR_EMPTY             -6    ///< 空状态
#define SLOG_ERR_FILTER            -7    ///< 过滤
#define SLOG_ERR_RESOURCE          -8    ///< 资源不可用
/** @}*/

#if defined(_CPU_CP)
#define SLOG_PRINT_PARM_MAX_NUM     15
#else
#define SLOG_PRINT_PARM_MAX_NUM     15
#endif
#define SLOG_LEVEL_NUM              4
#define SLOG_MODULE_NUM             4
#define SLOG_MASK_BYTES_NUM         8
#define SLOG_HEADER_VERSION         3
#ifdef USE_TOP_ASIC
#define SLOG_BUF_BLOCK_TIMEOUT_MS               (1000)                                      // ms
#define SLOG_LINK_CHECK_TIMER_PERIOD            (1000)                                      // ms
#define SLOG_SEND_BLOCK_TIME_MS                 (2000)                                      // ms
#else
#define SLOG_BUF_BLOCK_TIMEOUT_MS               (250)                                       // ms
#define SLOG_LINK_CHECK_TIMER_PERIOD            (250)                                       // ms
#define SLOG_SEND_BLOCK_TIME_MS                 (250)                                       // ms
#endif
#define SLOG_PSM_UART_CTS_TIMEOUT_MS            (1000)                                      // ms
#define SLOG_LOST_LINK_TIME_MS                  (60000)                                     // ms
#define SLOG_LOST_LINK_TIME_TICK                osTickFromMs(SLOG_LOST_LINK_TIME_MS)        // tick
#define SLOG_DBG_INFO_REPORT_TIME_MS            (60000)                                     // ms
#define SLOG_DBG_INFO_REPORT_TIME_TICK          osTickFromMs(SLOG_DBG_INFO_REPORT_TIME_MS)  // tick
#define SLOG_STRING_BUF_SIZE                    128
#define SLOG_MAX_SEQ                            0xFFFFFFFF
#define SLOG_ISR_LOG_MAX_SIZE                   (1024)
#define SLOG_ISR_MALLOC_LOG_MAX_SIZE            (4096)
#define SLOG_MALLOC_LOG_MAX_SIZE                (65536)
#define SLOG_SYS_MEM_USED_RATE_THRESHOLD        80
#define SLOG_VINT_ENCODE_BUF_MAX_SIZE           100

/**
 * @brief 根据printId生成带打印等级的打印ID
 * @note  level范围:    SLOG_LEVEL_DEBUG ~ SLOG_LEVEL_ERROR
 *        printId:		通过SLOG_PRINT_ID_NO_LEVEL生成的id
 */
#define SLOG_PRINT_ID_WITH_LEVEL(level, printId) ((((uint32_t)(level)&0x3)<<30) | (uint32_t)(printId))

/**
 * @brief 根据打印ID获取module
 * @note  printId：         通过SLOG_PRINT_ID_NO_LEVEL生成的打印ID
 */
#define SLOG_GET_MODULE_FROM_PRINT_ID(printId) (((uint32_t)(printId)>>28)&0x3)

/**
 * @brief 根据打印ID获取subModule
 * @note  printId：         通过SLOG_PRINT_ID_NO_LEVEL生成的打印ID
 */
#define SLOG_GET_SUBMODULE_FROM_PRINT_ID(printId) (((uint32_t)(printId)>>22)&0x3F)

/**
 * @brief 根据打印ID获取msgId
 * @note  printId：         通过SLOG_PRINT_ID_NO_LEVEL生成的打印ID
 */
#define SLOG_GET_MSGID_FROM_PRINT_ID(printId) (((uint32_t)(printId))&0xFFFF)

/**
 * @brief 根据打印ID获取level
 * @note  printId：         通过SLOG_PRINT_ID_WITH_LEVEL生成的打印ID
 */
#define SLOG_GET_LEVEL_FROM_PRINT_ID(printId) (((uint32_t)(printId)>>30)&0x3)

#ifdef _OS_WIN
#define SLOG_BUFFER_BASE                ((uint32_t)(ddr_base_addr_ap))
#else
#define SLOG_BUFFER_BASE                DRAM_BASE_ADDR_LOG_NC
#endif
#define SLOG_BUFFER_SHARE_CTRL_BASE     (SLOG_BUFFER_BASE)
#define SLOG_BUFFER_SHARE_CTRL_SIZE     (188)
#define SLOG_CMD_BUFFER_POOL_BASE       (SLOG_BUFFER_SHARE_CTRL_BASE + SLOG_BUFFER_SHARE_CTRL_SIZE)
#define SLOG_CMD_BUFFER_POOL_SIZE       (836)
#define SLOG_AP_BUFFER_POOL_BASE        (SLOG_CMD_BUFFER_POOL_BASE + SLOG_CMD_BUFFER_POOL_SIZE)
#define SLOG_AP_BUFFER_POOL_SIZE        (1024 * 16)
#define SLOG_CP_BUFFER_POOL_BASE        (SLOG_AP_BUFFER_POOL_BASE + SLOG_AP_BUFFER_POOL_SIZE)
#define SLOG_CP_BUFFER_POOL_SIZE        (1024 * 70)
#define SLOG_SHARE_BUFFER_POOL_BASE     (SLOG_CP_BUFFER_POOL_BASE + SLOG_CP_BUFFER_POOL_SIZE)
#define SLOG_SHARE_BUFFER_POOL_SIZE     (1024 * 16)
#define SLOG_BUF_POOL_CTRL_HDR_SIZE     (sizeof(SLOG_CachedBufBlock))

#define SLOG_HDLC_OBUF_TOTAL_LEN        (1024 * 64)

#ifdef USE_TOP_MTS
#define SLOG_CMD_BLOCK_SIZE             (52)
#define SLOG_MAX_CMD_BLOCK_NUM          (16)
#define SLOG_CMD_FREE_BLOCK_BIT_MAP     (0xFFFF)
#else
#define SLOG_CMD_BLOCK_SIZE             (44)
#define SLOG_MAX_CMD_BLOCK_NUM          (19)
#define SLOG_CMD_FREE_BLOCK_BIT_MAP     (0x7FFFF)
#endif
#define SLOG_SHARE_BLOCK_SIZE           (1024)

#if defined(_CPU_AP)
#define SLOG_PRIVATE_BLOCK_SIZE         (1024)
#define SLOG_MAX_PRIVATE_BLOCK_NUM      (16)
#define SLOG_PRIVATE_BLOCK_BIT_MAP      (0xFFFF)
#else
#define SLOG_PRIVATE_BLOCK_SIZE         (2240)
#define SLOG_MAX_PRIVATE_BLOCK_NUM      (32)
#define SLOG_PRIVATE_BLOCK_BIT_MAP      (0xFFFFFFFF)
#endif
#define SLOG_MAX_SHARE_BLOCK_NUM        (16)
#define SLOG_SHARE_BLOCK_BIT_MAP        (0xFFFF)

#define SLOG_SHARE_CTRL_MAGIC           (0x1F3D5B79)
#if defined(_CPU_AP)
#define SLOG_MAX_CACHED_BLOCK_NUM       (SLOG_MAX_CMD_BLOCK_NUM + SLOG_MAX_PRIVATE_BLOCK_NUM + SLOG_MAX_SHARE_BLOCK_NUM)
#else
#define SLOG_MAX_CACHED_BLOCK_NUM       (SLOG_MAX_PRIVATE_BLOCK_NUM + SLOG_MAX_SHARE_BLOCK_NUM)
#endif
#define SLOG_MAX_PACKET_NUM             (5)
#define SLOG_PACKET_SIZE                (1024 - SLOG_BUF_POOL_CTRL_HDR_SIZE)  //  min(SLOG_PRIVATE_BLOCK_SIZE - SLOG_BUF_POOL_CTRL_HDR_SIZE,SLOG_SHARE_BLOCK_SIZE - SLOG_BUF_POOL_CTRL_HDR_SIZE)

#define SLOG_SIG_MSG_EN_BIT_POS         (0)    ///< 标准LOG消息Enable Bit位
#define SLOG_PS_PRN_MSG_EN_BIT_POS      (1)    ///< 协议栈字符串打印消息Enable Bit位
#define SLOG_PHY_PRN_MSG_EN_BIT_POS     (2)    ///< 物理层字符串打印消息Enable Bit位
#define SLOG_PLAT_PRN_MSG_EN_BIT_POS    (3)    ///< 平台字符串打印消息Enable Bit位
#define SLOG_PRN_MSG_EN_BIT_POS         (4)    ///< PRINT字符串打印消息Enable Bit位

/********************************************************************************
LOG输出NV控制。采用BitMap形式。某个Bit的取值含义:0-不打印;1-打印。当采用NV控制时, PC LOG工具配置失效。
    15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    | | | | | | | | | | | | | | | | | | |
    | | | | | | | | | | | | | | | | | | +---- 协议栈打印
    | | | | | | | | | | | | | | | | | +------ 物理层打印
    | | | | | | | | | | | | | | | | +-------- 协议栈标准LOG打印
    | | | | | | | | | | | | | | | +---------- 物理层标准LOG打印
    | | | | | | | | | | | | | | +------------ 平台窗口打印
    | | | | | | | | | | | | | +-------------- PRINT窗口非DUMP打印
    | | | | | | | | | | | | +---------------- DUMP打印(如IP数据包打印)
    | | | | | | | | | | | +------------------ 保留
    | | | | | | | | | | +-------------------- 保留
    | | | | | | | | | +---------------------- 保留
    | | | | | | | | +------------------------ 保留
    | | | | | | | +-------------------------- 保留
    | | | | | | +---------------------------- 保留
    | | | | | +------------------------------ 保留
    | | | | +-------------------------------- 保留
    | | | +---------------------------------- 保留
    | | +------------------------------------ 保留
    | +-------------------------------------- 保留
    +---------------------------------------- 保留
*/
#define SLOG_NV_PS_PRN_MSG_EN_BIT_POS       (0)    // 协议栈打印Enable Bit位
#define SLOG_NV_PHY_PRN_MSG_EN_BIT_POS      (1)    // 物理层打印Enable Bit位
#define SLOG_NV_PS_SIG_MSG_EN_BIT_POS       (2)    // 协议栈标准LOG打印Enable Bit位
#define SLOG_NV_PHY_SIG_MSG_EN_BIT_POS      (3)    // 物理层标准LOG打印Enable Bit位
#define SLOG_NV_PLAT_PRN_MSG_EN_BIT_POS     (4)    // 平台窗口打印Enable Bit位
#define SLOG_NV_PRN_MSG_EN_BIT_POS          (5)    // PRINT窗口非DUMP打印Enable Bit位
#define SLOG_NV_DUMP_MSG_EN_BIT_POS         (6)    // DUMP打印Enable Bit位

typedef struct _SLOG_ListNode
{
    struct _SLOG_ListNode *next;
} SLOG_ListNode;

typedef struct
{
    uint32_t        count;
    SLOG_ListNode  *head;
    SLOG_ListNode  *tail;
} SLOG_List;

typedef enum
{
    SLOG_TYPE_WIDTH_32BIT,
    SLOG_TYPE_WIDTH_64BIT,
    SLOG_TYPE_WIDTH_PTR,
} SLOG_FMT_TYPE_WIDTH;

typedef enum
{
    SLOG_INVALID_MSG        = 0,    ///< 无效消息
    SLOG_PS_SIG_MSG         = 1,    ///< 协议栈信令/标准LOG消息
    SLOG_PHY_SIG_MSG        = 2,    ///< 物理层信令/标准LOG消息
    SLOG_PS_PRN_MSG         = 3,    ///< 协议栈字符串打印消息
    SLOG_PHY_PRN_MSG        = 4,    ///< 物理层字符串打印消息
    SLOG_AP_PLAT_PRN_MSG    = 5,    ///< AP平台字符串打印消息
    SLOG_CP_PLAT_PRN_MSG    = 6,    ///< CP平台字符串打印消息
    SLOG_AP_PRN_MSG         = 7,    ///< AP PRINT打印消息(不携带格式化字符串)
    SLOG_DIAG_MSG           = 8,    ///< 诊断消息
    SLOG_AP_PRN_FORMAT_MSG  = 9,    ///< AP PRINT打印消息(携带格式化字符串)
    SLOG_CP_PRN_MSG         = 10,   ///< CP PRINT打印消息(不携带格式化字符串)
    SLOG_CP_PRN_FORMAT_MSG  = 11,   ///< CP PRINT打印消息(携带格式化字符串)
    SLOG_AP_DUMP_MSG        = 12,   ///< AP DUMP消息
    SLOG_CP_DUMP_MSG        = 13,   ///< CP DUMP消息

    SLOG_MAX_MSG_TYPE,
} SLOG_MsgType;

typedef enum
{
    SLOG_SIG_TYPE        = 1,    ///< 标准LOG窗口
    SLOG_PS_PRN_TYPE     = 2,    ///< 协议栈字符串打印窗口
    SLOG_PHY_PRN_TYPE    = 3,    ///< 物理层字符串打印窗口
    SLOG_PLAT_PRN_TYPE   = 4,    ///< 平台字符串打印窗口
    SLOG_PRN_TYPE        = 5,    ///< PRINT打印窗口
} SLOG_PrintWndType;

typedef enum
{
    SLOG_LINK_OFF        = 0,
    SLOG_LINK_ON         = 1,
} SLOG_LinkStatus;

typedef struct
{
    uint8_t         version;
    uint8_t         msgType;    ///< SLOG_MsgType
    uint16_t        length;
} SLOG_Header;

typedef struct
{
    uint32_t        id;
    uint32_t        sfn;
    uint32_t        seq;
    uint32_t        timeStamp;
    uint8_t         compressFlag;   // 0:数据区不使用软件压缩 1：数据区使用软件压缩
    uint8_t         paramNum;
    uint16_t        length;
} SLOG_PrintHeader;

typedef struct
{
    uint32_t        id;
    uint32_t        sfn;
    uint32_t        seq;
    uint32_t        timeStamp;
    uint16_t        length;
    uint16_t        reserved;
} SLOG_DumpHeader;

typedef struct
{
    uint32_t        id;      ///< sigId = id & 0xFFFF
    uint32_t        sfn;
    uint32_t        seq;
    uint32_t        timeStamp;
    uint16_t        length;
    uint16_t        reserved;
} SLOG_StdHeader;

#endif /* End of _SLOG_DEF_H_ */