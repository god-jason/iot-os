/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_spinlock.h
 *
 * @brief       spinlock驱动头文件.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team         创建
 ************************************************************************************
 */

#ifndef _DRV_SPINLOCK_H
#define _DRV_SPINLOCK_H

/**
 * @addtogroup Spinlock
 */

/**@{*/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/**
\brief: 硬件锁id.
*/
typedef enum
{
    PCU_HWLOCK  = 0,/* PCU */
    CLK_HWLOCK  = 1,/* Clock */
    REGLOCK_HWLOCK  = 2,/* Reg */
    SOFTLOCK_HWLOCK = 3,/* Software */
    LOG_HWLOCK = 4,/* LOG */
    ADC_HWLOCK = 5,/* ADC */
    FLASH_HWLOCK = 6,/* ADC */
    MODEM_SHARE_BUF_HWLOCK = 7,/* Modem Share Buf */
    HWLOCK_NUM
} Spinlock_HwId;

/**
\brief: 软件锁id.
*/
typedef enum
{
    I2C0_SFLOCK = 0,/*i2c0*/
    I2C1_SFLOCK = 1,/*i2c1*/
    I2C2_SFLOCK = 2,/*pmic-i2c*/
    FLASH_SFLOCK = 3,/*NAND*/
    SD0_SFLOCK,
    SD1_SFLOCK,
    ADC_SFLOCK,     /*pmic adc*/
    SFLOCK_ID7,
    SFLOCK_ID8,
    SFLOCK_ID9,
    SFLOCK_ID10,
    SFLOCK_ID11,
    SFLOCK_ID12,
    SFLOCK_ID13,
    SFLOCK_ID14,
    SFLOCK_ID15,
    SFLOCK_ID16,
    SFLOCK_ID17,
    SFLOCK_ID18,
    SFLOCK_ID19,
    SFLOCK_ID20,
    SFLOCK_ID21,
    SFLOCK_ID22,
    SFLOCK_ID23,
    SFLOCK_ID24,
    SFLOCK_ID25,
    SFLOCK_ID26,
    SFLOCK_ID27,
    SFLOCK_ID28,
    SFLOCK_ID29,
    SFLOCK_ID30,
    REG_SFLOCK = 31,/*reg*/
    SFLOCK_NUM
} Spinlock_SwId;

#ifdef USE_SW_SPINLOCK
typedef struct {
    uint16_t  used;
    uint16_t  owner;
} Softlock_Desc;
#endif

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           锁中断并获取spinlock
 *
 * @param[in]       hwid        锁id
 * @param[out]      plevel      中断状态指针
 *
 * @return          void
 ************************************************************************************
*/
void Spinlock_HwLockIrq(Spinlock_HwId hwid, ubase_t *plevel);

/**
 ************************************************************************************
 * @brief           锁中断并尝试获取spinlock
 *
 * @param[in]       hwid        锁id
 * @param[out]      plevel      中断状态指针
 *
 * @return          状态码
 ************************************************************************************
*/
osStatus_t Spinlock_HwTryLockIrq(Spinlock_HwId hwid, ubase_t *plevel);

/**
 ************************************************************************************
 * @brief           释放spinlock并恢复之前的中断状态
 *
 * @param[in]       hwid        锁id
 * @param[in]       level       之前保存的中断状态
 *
 * @return          void
 ************************************************************************************
*/
void Spinlock_HwUnlockIrq(Spinlock_HwId hwid, ubase_t level);

/**
 ************************************************************************************
 * @brief           获取spinlock，仅在本函数中锁中断
 *
 * @param[in]       hwid        锁id
 *
 * @return          void
 ************************************************************************************
*/
void Spinlock_HwLock(Spinlock_HwId hwid);

/**
 ************************************************************************************
 * @brief           尝试获取spinlock，仅在本函数中锁中断
 *
 * @param[in]       hwid        锁id
 *
 * @return          状态码
 ************************************************************************************
*/
osStatus_t Spinlock_HwTryLock(Spinlock_HwId hwid);

/**
 ************************************************************************************
 * @brief           释放spinlock，仅在本函数中锁中断
 *
 * @param[in]       hwid        锁id
 *
 * @return          void
 ************************************************************************************
*/
void Spinlock_HwUnLock(Spinlock_HwId hwid);

#ifdef USE_SW_SPINLOCK
/**
 ************************************************************************************
 * @brief           获取软spinlock
 *
 * @param[in]       sfid        软件锁id
 *
 * @return          void
 ************************************************************************************
*/
void  Spinlock_SoftLock(Spinlock_SwId sfid);

/**
 ************************************************************************************
 * @brief           释放软spinlock
 *
 * @param[in]       sfid        软件锁id
 *
 * @return          void
 ************************************************************************************
*/
void  Spinlock_SoftUnLock(Spinlock_SwId sfid);
#endif
#endif/*_DRV_SPINLOCK_H*/
/** @} */
