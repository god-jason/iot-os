/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_common.h
 *
 * @brief       驱动通用定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21   ict team        创建
 ************************************************************************************
 */

#ifndef _DRV_COMMON_H
#define _DRV_COMMON_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
 
/* 芯片通用信息 */
#include <chip_reg_base.h>
#include <chip_int_num.h>
#include <chip_pin_list.h>
#include <chip_dma_req.h>
#include <chip_resources.h>

/**
 * @addtogroup DrvCommon
 */

/**@{*/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/**@name 驱动通用返回值定义
 * @{
 */
#define DRV_OK                     0    ///< 操作成功
#define DRV_ERR                   -1    ///< 未知错误
#define DRV_ERR_BUSY              -2    ///< 设备忙
#define DRV_ERR_TIMEOUT           -3    ///< 操作超时
#define DRV_ERR_UNSUPPORTED       -4    ///< 不支持的操作
#define DRV_ERR_PARAMETER         -5    ///< 参数错误
#define DRV_ERR_FULL              -6    ///< 满状态
#define DRV_ERR_EMPTY             -7    ///< 空状态
#define DRV_ERR_SPECIFIC          -8    ///< 私有错误的起始编号
/** @}*/

/**@name 驱动通用时钟频率定义
 * @{
 */
#define KHz                       (1000)
#define MHz                       (1000000)
#define FREQ_32KHZ                (32768)       ///< 32768 Hz
#define FREQ_26MHZ                (26*MHz)      ///< 26MHz
#define FREQ_24MHZ                (24*MHz)      ///< 24MHz
#define FREQ_52MHZ                (52*MHz)      ///< 52MHz
#define FREQ_78MHZ                (78*MHz)      ///< 78MHz
#define FREQ_104MHZ               (104*MHz)     ///< 104MHz
#define FREQ_156MHZ               (156*MHz)     ///< 156MHz
#define FREQ_208MHZ               (208*MHz)     ///< 208MHz
#define FREQ_312MHZ               (312*MHz)     ///< 312MHz
#define FREQ_416MHZ               (416*MHz)     ///< 416MHz
#define FREQ_624MHZ               (624*MHz)     ///< 624MHz
#define MAIN_CLOCK                FREQ_26MHZ
/** @}*/


/* 寄存器宏修饰符 */
#define __IO  volatile


/**@name 使用结构体变量方式配置寄存器
 * @{
 */
#define SET_BIT(REG, BIT)      ((REG) |= (BIT))          ///< 写寄存器BIT位
#define CLEAR_BIT(REG, BIT)    ((REG) &= ~(BIT))         ///< 清寄存器BIT位
#define READ_BIT(REG, BIT)     ((REG) & (BIT))           ///< 读寄存器BIT位
#define CLEAR_REG(REG)         ((REG) = (0x0))          ///< 对寄存器清零
#define WRITE_REG(REG, VAL)    ((REG) = (VAL))          ///< 写寄存器
#define READ_REG(REG)          ((REG))                  ///< 读寄存器
#define MODIFY_REG(REG, CLEARMASK, SETMASK) (WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK))))    ///< 修改寄存器
/** @}*/


/**@name 使用直接地址方式配置寄存器
 * @{
 */
#define SET_BIT_U8(ADDR, BIT)     (*(volatile uint8_t*)(ADDR) |= (BIT))     ///< 写8位寄存器BIT位
#define SET_BIT_U16(ADDR, BIT)    (*(volatile uint16_t*)(ADDR) |= (BIT))    ///< 写16位寄存器BIT位
#define SET_BIT_U32(ADDR, BIT)    (*(volatile uint32_t*)(ADDR) |= (BIT))    ///< 写32位寄存器BIT位
#define SET_BIT_U64(ADDR, BIT)    (*(volatile uint64_t*)(ADDR) |= (BIT))    ///< 写64位寄存器BIT位

#define CLEAR_BIT_U8(ADDR, BIT)   (*(volatile uint8_t*)(ADDR) &= ~(BIT))     ///< 清8位寄存器BIT位
#define CLEAR_BIT_U16(ADDR, BIT)  (*(volatile uint16_t*)(ADDR) &= ~(BIT))    ///< 清16位寄存器BIT位
#define CLEAR_BIT_U32(ADDR, BIT)  (*(volatile uint32_t*)(ADDR) &= ~(BIT))    ///< 清32位寄存器BIT位
#define CLEAR_BIT_U64(ADDR, BIT)  (*(volatile uint64_t*)(ADDR) &= ~(BIT))    ///< 清64位寄存器BIT位

#define READ_U8(ADDR)        (*(volatile uint8_t*)(ADDR))      ///< 读8位寄存器
#define READ_U16(ADDR)       (*(volatile uint16_t*)(ADDR))     ///< 读16位寄存器
#define READ_U32(ADDR)       (*(volatile uint32_t*)(ADDR))     ///< 读32位寄存器
#define READ_U64(ADDR)       (*(volatile uint64_t*)(ADDR))     ///< 读64位寄存器

#define WRITE_U8(ADDR, VAL)     (*(volatile uint8_t*)(ADDR) = (VAL))      ///< 写8位寄存器
#define WRITE_U16(ADDR, VAL)    (*(volatile uint16_t*)(ADDR) = (VAL))     ///< 写16位寄存器
#define WRITE_U32(ADDR, VAL)    (*(volatile uint32_t*)(ADDR) = (VAL))     ///< 写32位寄存器
#define WRITE_U64(ADDR, VAL)    (*(volatile uint64_t*)(ADDR) = (VAL))     ///< 写64位寄存器

#define MODIFY_U8(ADDR, CLEARMASK, SETMASK)   WRITE_U8((ADDR), (((READ_U8(ADDR)) & (~(CLEARMASK))) | (SETMASK)))      ///< 修改8位寄存器
#define MODIFY_U16(ADDR, CLEARMASK, SETMASK)  WRITE_U16((ADDR), (((READ_U16(ADDR)) & (~(CLEARMASK))) | (SETMASK)))    ///< 修改16位寄存器
#define MODIFY_U32(ADDR, CLEARMASK, SETMASK)  WRITE_U32((ADDR), (((READ_U32(ADDR)) & (~(CLEARMASK))) | (SETMASK)))    ///< 修改32位寄存器
#define MODIFY_U64(ADDR, CLEARMASK, SETMASK)  WRITE_U64((ADDR), (((READ_U64(ADDR)) & (~(CLEARMASK))) | (SETMASK)))    ///< 修改64位寄存器

#define WRITE_BIT_U8(ADDR, VAL, CNT, OFFSET)  WRITE_U8((ADDR), ((READ_U8(ADDR) & ~(((1<<(CNT)) - 1) << (OFFSET))) | ((VAL) << (OFFSET))))      ///< 写8位寄存器的BIT位
#define WRITE_BIT_U16(ADDR, VAL, CNT, OFFSET) WRITE_U16((ADDR), ((READ_U16(ADDR) & ~(((1<<(CNT)) - 1) << (OFFSET))) | ((VAL) << (OFFSET))))    ///< 写16位寄存器的BIT位
#define WRITE_BIT_U32(ADDR, VAL, CNT, OFFSET) WRITE_U32((ADDR), ((READ_U32(ADDR) & ~(((1<<(CNT)) - 1) << (OFFSET))) | ((VAL) << (OFFSET))))    ///< 写32位寄存器的BIT位
#define WRITE_BIT_U64(ADDR, VAL, CNT, OFFSET) WRITE_U64((ADDR), ((READ_U64(ADDR) & ~(((1<<(CNT)) - 1) << (OFFSET))) | ((VAL) << (OFFSET))))    ///< 写64位寄存器的BIT位
/** @}*/


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/**
 * @brief 功耗控制
 */
typedef enum {
    DRV_POWER_OFF,                  ///< 断电模式
    DRV_POWER_LOW,                  ///< 低功耗模式，接收唤醒信号工作
    DRV_POWER_FULL                  ///< 最大功耗模式
} DRV_POWER_STATE;

#endif  // _DRV_COMMON_H

/** @} */
