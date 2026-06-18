/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_lpm.h
 *
 * @brief       LPM驱动代码
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-05-11     ICT Team          创建
 ************************************************************************************
 */
#ifndef __DRV_LPM__
#define __DRV_LPM__

/************************************************************************************
 *                                 头文件定义
 ************************************************************************************/
#include <drv_common.h>

/**
 * @addtogroup Lpm
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define LTE_LPM5_INT                    (OS_EXT_IRQ_TO_IRQ(AP_INT_NUM_20))

#define LTE_LPM_BASE                    BASE_LTE_LPM
#define LTE_LPM_TIME1_EN                (LTE_LPM_BASE + 0)
#define LTE_LPM_TIME5_EN                (LTE_LPM_BASE + 0x64)
#define LTE_LPM_TIME4_EN                (LTE_LPM_BASE + 0x60)
#define LTE_LPM_TIME3_EN                (LTE_LPM_BASE + 0x5C)
#define LTE_LPM_TIME2_EN                (LTE_LPM_BASE + 0x58)
#define LTE_LPM_SFN_REG                 (LTE_LPM_BASE + 0x4C)
#define LTE_LPM_LOCAL_MRTR_TRACE_REG    (LTE_LPM_BASE + 0x10)
#define LTE_LPM_INTER_SOFT_REG          (LTE_LPM_BASE + 0x38)
#define LTE_LPM_FRAC_SOFT_REG           (LTE_LPM_BASE + 0x3C)
#define LTE_LPM_CAL_CTRL                (LTE_LPM_BASE + 0x14)
#define LTE_LPM_FRAC_REAL_REG           (LTE_LPM_BASE + 0x20)
#define LTE_LPM_INTEG_REAL_REG          (LTE_LPM_BASE + 0x30)

#define LPM_FRAC_WID                    22
#define LPM_FRAC_OFF                    0
#define LPM_INTER_WID                   13
#define LPM_INTER_OFF                   0

#define LPM_CAL_SEL_BIT                 4

#define LPM_INVAL_NUM                   (0xFFFFFFFF)
#define MAX_HSFN_NUM                    1024UL
#define LPM_LTE_ENABLE                  (0x1)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
#if defined (_CPU_AP)
typedef struct
{
    uint16_t wLteSuperFrmCnt;
    uint16_t wLteFrmCnt;
    uint16_t wLteSubFrmCnt;
}LPM_LteTime;

typedef union
{
    struct
    {
        uint32_t dwTs : 14;
        uint32_t Slot : 5;
        uint32_t Frame : 10;
        uint32_t Resv : 3;
    };
    uint32_t dReg;
}LPM_Mrtr;

typedef void (*LPM_Callback)(void);

/**
 ************************************************************************************
 * @brief           使能LPM5 中断
 *
 * @param[in]       void
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void LPM_Int5Enable(void);

/**
 ************************************************************************************
 * @brief           关闭phy lpm中断
 *
 * @param[in]       void
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void LPM_PhyIntMask(void);

/**
 ************************************************************************************
 * @brief           关闭LPM5 中断
 *
 * @param[in]       void
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void LPM_Int5Disable(void);

/**
 ************************************************************************************
 * @brief           获取lte sfn
 *
 * @param[in]       void
 *
 * @return          uint32_t
 * @retval                          sfn的值
 ************************************************************************************
*/
uint32_t LPM_GetSfnLte(void);

/**
 ************************************************************************************
 * @brief           获取lte hfn
 *
 * @param[in]       void
 *
 * @return          uint32_t
 * @retval                          hfn的值
 ************************************************************************************
*/
uint32_t LPM_GetHfnLte(void);

/**
 ************************************************************************************
 * @brief           将sfn和mrtr 转化为ms值
 *
 * @param[in]       uint32_t sfn 超帧
 *                  uint32_t val mrtr
 *
 * @return          uint32_t
 * @retval                          ms值
 ************************************************************************************
*/
uint32_t LPM_SfnmrtrToMs(uint32_t sfn, uint32_t val);

/**
 ************************************************************************************
 * @brief           获取LTE 时间
 *
 * @param[out]      T_LteLpmTime        LTE LOCAL MRTR的值
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void LPM_GetLtetime(LPM_LteTime  *pLteNtLpmTime);

/**
 ************************************************************************************
 * @brief           获取lpm mrtr寄存器的值
 *
 * @param[in]       void
 *
 * @return          uint32_t
 * @retval                          lpm mrtr寄存器的值
 ************************************************************************************
*/
static inline uint32_t LPM_GetMrtr(void)
{
    return READ_U32(LTE_LPM_LOCAL_MRTR_TRACE_REG);
}

/**
 ************************************************************************************
 * @brief           获取lpm sfn寄存器的值
 *
 * @param[in]       void
 *
 * @return          uint32_t
 * @retval                          lpm sfn寄存器的值
 ************************************************************************************
*/
static inline uint32_t LPM_GetSfn(void)
{
    return READ_U32(LTE_LPM_SFN_REG);
}

/**
 ************************************************************************************
 * @brief           注册LPM回调
 * @note            LPM回调在LPM中断中调用，使用方需保证回调中无阻塞性操作.
 *
 * @param[in]       callback            LPM回调
 *
 * @return          void
 * @retval
 ************************************************************************************
*/
void LPM_RegisterCallback(LPM_Callback  callback);

/**
 ************************************************************************************
 * @brief           计算mrtr 时间差，并转化为ms
 *
 * @param[in]       uint32_t sfn     被减数 sfn
 *                  uint32_t val     被减数 mrtr
 *                  uint32_t sfnSub  减数   sfn
 *                  uint32_t valSub  减数 mrtr
 *
 * @return          uint32_t
 * @retval                          ms值
 ************************************************************************************
*/
uint32_t LPM_SubConvertToMs(uint32_t sfn, uint32_t val, uint32_t sfnSub, uint32_t valSub);
#endif
#endif /* __DRV_LPM__ */

/** @} */