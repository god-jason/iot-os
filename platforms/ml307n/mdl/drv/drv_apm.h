/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_apm.h
 *
 * @brief       apm驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */


#ifndef DRIVER_APM_H_
#define DRIVER_APM_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>

/**
 * @addtogroup Apm
 */

/**@{*/

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define APM_MONITOR_NUM_MAX (4)

/****** APM Control Codes *****/

#define APM_CONTROL_Pos 0
#define APM_CONTROL_Msk (0xFFUL << APM_CONTROL_Pos)

#define APM_ST_DUART (0x01UL   << APM_CONTROL_Pos)    ///< APM 统计时长; arg = Baudrate
#define APM_WID_CONFIG (0x02UL << APM_CONTROL_Pos)    ///< APM ID选择寄存器; arg = wid
#define APM_RID_CONFIG (0x03UL  << APM_CONTROL_Pos)   ///< APM ID选择寄存器; arg = rid
#define APM_LEN_CONFIG (0x04UL  << APM_CONTROL_Pos)   ///< APM LEN选择寄存器; arg = burst_len
#define APM_SIZE_CONFIG (0x05UL  << APM_CONTROL_Pos)  ///< APM SIZE选择寄存器; arg = burst_size
#define APM_BUSRT_CONFIG (0x06UL << APM_CONTROL_Pos)  ///< APM busrt 类型选择寄存器; arg = burst_type
#define APM_CACHE_CONFIG (0x07UL << APM_CONTROL_Pos)  ///< APM cache选择寄存器; arg = cache type
#define APM_START_ADDR_CONFIG  (0x08UL << APM_CONTROL_Pos) ///< APM 地址下边界; arg = 地址
#define APM_END_ADDR_CONFIG  (0x09UL << APM_CONTROL_Pos)   ///< APM 地址上边界; arg = 地址
#define APM_CLEAR_FLGA   (0x0AUL<< APM_CONTROL_Pos)        ///< 统计计数器清零;
#define APM_START   (0x0BUL<< APM_CONTROL_Pos)        ///< start
#define APM_STOP   (0x0CUL<< APM_CONTROL_Pos)        ///< end
#define APM_PORT   (0x0DUL<< APM_CONTROL_Pos)        ///< port

#define APM_WID_DEFCONFIG_Pos 8
#define APM_WID_DEFCONFIG_Msk (0x1UL << APM_WID_DEFCONFIG_Pos)
#define APM_WID_DEFCONFIG (0x01UL << APM_WID_DEFCONFIG_Pos)

#define APM_RID_DEFCONFIG_Pos 9
#define APM_RID_DEFCONFIG_Msk (0x1UL << APM_RID_DEFCONFIG_Pos)
#define APM_RID_DEFCONFIG (0x01UL << APM_RID_DEFCONFIG_Pos)

#define APM_LEN_DEFCONFIG_Pos 10
#define APM_LEN_DEFCONFIG_Msk (0x01UL << APM_LEN_DEFCONFIG_Pos)
#define APM_LEN_DEFCONFIG (0x01UL << APM_LEN_DEFCONFIG_Pos)

#define APM_SIZE_DEFCONFIG_Pos 11
#define APM_SIZE_DEFCONFIG_Msk (0x01UL << APM_SIZE_DEFCONFIG_Pos)
#define APM_SIZE_DEFCONFIG (0x01UL << APM_SIZE_DEFCONFIG_Pos)

#define APM_BUSRT_DEFCONFIG_Pos 12
#define APM_BUSRT_DEFCONFIG_Msk (0x01UL << APM_BUSRT_DEFCONFIG_Pos)
#define APM_BUSRT_DEFCONFIG (0x01UL << APM_BUSRT_DEFCONFIG_Pos)

#define APM_CACHE_DEFCONFIG_Pos 13
#define APM_CACHE_DEFCONFIG_Msk (0x01UL << APM_CACHE_DEFCONFIG_Pos)
#define APM_CACHE_DEFCONFIG (0x0UL << APM_CACHE_DEFCONFIG_Pos)

#define APM_ADDR_MATCH_ENABLE_Pos 14
#define APM_ADDR_MATCH_ENABLE_Msk (0x03UL << APM_ADDR_MATCH_ENABLE_Pos)
#define APM_ADDR_MATCH_ENABLE (0x1UL << APM_ADDR_MATCH_ENABLE_Pos)
#define APM_ADDR_MATCH_DISABLE (0x2UL << APM_ADDR_MATCH_ENABLE_Pos)

#define APM_TTRANS_INS_SEL_Pos 16
#define APM_TTRANS_INS_SEL_MASK (0x03UL << APM_TTRANS_INS_SEL_Pos)
#define APM_TTRANS_INS_ENABLE   (0x1 << APM_TTRANS_INS_SEL_Pos)
#define APM_TTRANS_DATA_ENABLE  (0x2 << APM_TTRANS_INS_SEL_Pos)
#define APM_TTRANS_ALL_DISABLE  (0x3<< APM_TTRANS_INS_SEL_Pos)

#define APM_ADDR_MATCH_INT_Pos 18
#define APM_ADDR_MATCH_INT_MASK     (0x07UL << APM_ADDR_MATCH_INT_Pos)
#define APM_ADDR_MATCH_INT_AW       (0x1 << APM_ADDR_MATCH_INT_Pos)
#define APM_ADDR_MATCH_INT_AR       (0x2 << APM_ADDR_MATCH_INT_Pos)
#define APM_ADDR_MATCH_INT_DISABLE  (0x4<< APM_ADDR_MATCH_INT_Pos)

#define APM_ADDR_MATCH_INT_CLEAR_Pos 21
#define APM_ADDR_MATCH_INT_CLEAR_MASK     (0x03UL << APM_ADDR_MATCH_INT_CLEAR_Pos)
#define APM_ADDR_MATCH_INT_CLEAR_AW       (0x1 << APM_ADDR_MATCH_INT_CLEAR_Pos)
#define APM_ADDR_MATCH_INT_CLEAR_AR       (0x2 << APM_ADDR_MATCH_INT_CLEAR_Pos)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
  /**
  \brief Access structure of APM_Handle.
  */
  typedef struct
  {
    APM_Res_t          p_Res;
  } APM_Handle;

  /**
  \brief Access structure of APM count.
  */
  typedef struct
  {
    uint32_t aw_burst_num;
    uint32_t aw_data_num;
    uint32_t ar_burst_num;
    uint32_t ar_data_num;
    uint32_t w_beat_num;
    uint32_t w_last_num;
    uint32_t r_beat_num;
    uint32_t r_last_num;
  } APM_StaticsCount;

  /**
  \brief Access structure of APM addr match.
  */

  typedef struct
  {
    uint32_t flag;
    uint32_t addr;
  } APM_MatchAddr;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           控制APM控制器.
 *
 * @param[in]       APM            APM控制器句柄
 * @param[in]       control         控制命令
 * @param[in]       control         控制命令参数
 *
 * @return          初始化返回值.
 * @retval          ==DRV_OK        执行成功
 *                  == DRV_ERR_PARAMETER 参数有错误
 ************************************************************************************
 */
  int32_t APM_Control(APM_Handle *APM, uint32_t control, uint32_t arg);

  /**
   ************************************************************************************
   * @brief           获取Match Count.
   *
   * @param[in]       APM            APM控制器句柄
   *
   * @return          void.
   * @retval          ==void
   ************************************************************************************
   */
  extern void APM_GetMatchCount(APM_Handle *APM, APM_StaticsCount *mCount);

  /**
   ************************************************************************************
   * @brief           获取Total Count.
   *
   * @param[in]       APM            APM控制器句柄
   *
   * @return          void.
   * @retval          void
   ************************************************************************************
   */
  extern void APM_GetTotalCount(APM_Handle *APM, APM_StaticsCount *tCount);

/**
 ************************************************************************************
 * @brief           获取Matcah Addr.
 *
 * @param[in]       APM            APM控制器句柄
 *
 * @return          void.
 * @retval          void
 ************************************************************************************
 */
extern void APM_GetMatchAddr(APM_Handle *APM, APM_MatchAddr *tAddr);

extern void APM_Start(uint8_t mon, uint8_t port, uint8_t cache, uint32_t startAddr, uint32_t endAddr, uint8_t type);
extern void APM_Stop(uint8_t mon);
extern void APM_StartWithIrq(uint8_t mon, uint8_t port, uint8_t cache, uint32_t startAddr, uint32_t endAddr, uint8_t type);
extern void APM_StopAndGetResult(uint8_t mon, APM_StaticsCount *mCount, APM_StaticsCount *tCount);
extern void APM_Read(uint8_t mon);

#endif

/** @} */
