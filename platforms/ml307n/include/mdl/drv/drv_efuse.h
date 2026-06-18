/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_efuse.h
 *
 * @brief       EFUSE控制器驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2024-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef __DRV_EFUSE_H__
#define __DRV_EFUSE_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/

/****** Common function macro ******/
#include <os.h>
#include <drv_common.h>

/**
 * @addtogroup Efuse
 */

/**@{*/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

#define BIT(x) ((uint32_t)((uint32_t)0x01U<<(x)))
#define GENMASK(h, l) \
        (((~0ULL) << (l)) & ((0x1ULL << (h + 1)) - 1))

#define EFUSE_BITS_NUM      (1024 / 2)
#define EFUSE_WORD_BITS_NUM (32)

/****** EFUSE specific error codes *****/
#define EFUSE_ERR_EPERM  (DRV_ERR_SPECIFIC - 1) ///< Specified action not permit

/* SECURE LOCK word index */
#define EFUSE_LOCK_FLAG_WORD_INDEX (0)

/* EFUSE bits offset & length */
#define EFUSE_LOCK_FLAG_WORD_SECURE_EN_OFFS          (0)
#define EFUSE_LOCK_FLAG_WORD_SECURE_EN_LEN           (1)

#define EFUSE_LOCK_FLAG_WORD_DIS_ATE_EFUSE_OFFS      (1)
#define EFUSE_LOCK_FLAG_WORD_DIS_ATE_EFUSE_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_DIS_ATE_FLASH_OFFS      (2)
#define EFUSE_LOCK_FLAG_WORD_DIS_ATE_FLASH_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_DOWNLOAD_LOCK_OFFS      (3)
#define EFUSE_LOCK_FLAG_WORD_DOWNLOAD_LOCK_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W0_OFFS      (16)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W0_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W1_OFFS      (17)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W1_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W2_OFFS      (18)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W2_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W3_OFFS      (19)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W3_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W4_OFFS      (20)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W4_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W5_OFFS      (21)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W5_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W6_OFFS      (22)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W6_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W7_OFFS      (23)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W7_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W8_OFFS      (24)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W8_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W9_OFFS      (25)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W9_LEN       (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W10_OFFS     (26)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W10_LEN      (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W11_OFFS     (27)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W11_LEN      (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W12_OFFS     (28)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W12_LEN      (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W13_OFFS     (29)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W13_LEN      (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W14_OFFS     (30)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W14_LEN      (1)

#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W15_OFFS     (31)
#define EFUSE_LOCK_FLAG_WORD_EFUSE_LOCK_W15_LEN      (1)

#define EFUSE_PMU_TRIM_WORD_LPBGR_TRIM_OFFS          (64)
#define EFUSE_PMU_TRIM_WORD_LPBGR_TRIM_LEN           (4)

#define EFUSE_PMU_TRIM_WORD_DCDC2F_TRIM_OFFS         (68)
#define EFUSE_PMU_TRIM_WORD_DCDC2F_TRIM_LEN          (4)

#define EFUSE_PMU_TRIM_WORD_DCDC2S_TRIM_OFFS         (72)
#define EFUSE_PMU_TRIM_WORD_DCDC2S_TRIM_LEN          (4)

#define EFUSE_PMU_TRIM_WORD_OPM_TRIM_OFFS            (76)
#define EFUSE_PMU_TRIM_WORD_OPM_TRIM_LEN             (4)

#define EFUSE_PMU_TRIM_WORD_LDO1_TRIM_OFFS           (80)
#define EFUSE_PMU_TRIM_WORD_LDO1_TRIM_LEN            (4)

#define EFUSE_PMU_TRIM_WORD_LDO2_TRIM_OFFS           (84)
#define EFUSE_PMU_TRIM_WORD_LDO2_TRIM_LEN            (4)

#define EFUSE_PMU_TRIM_WORD_LDO3_TRIM_OFFS           (88)
#define EFUSE_PMU_TRIM_WORD_LDO3_TRIM_LEN            (4)

#define EFUSE_PMU_TRIM_WORD_LDO4_TRIM_OFFS           (92)
#define EFUSE_PMU_TRIM_WORD_LDO4_TRIM_LEN            (4)

#define EFUSE_PMU_TRIM_WORD_LDO5_TRIM_OFFS           (96)
#define EFUSE_PMU_TRIM_WORD_LDO5_TRIM_LEN            (4)

#define EFUSE_PMU_TRIM_WORD_LDO6_TRIM_OFFS           (100)
#define EFUSE_PMU_TRIM_WORD_LDO6_TRIM_LEN            (4)

#define EFUSE_PMU_TRIM_WORD_LDO7_TRIM_OFFS           (104)
#define EFUSE_PMU_TRIM_WORD_LDO7_TRIM_LEN            (4)

#define EFUSE_PMU_TRIM_WORD_LDO8_TRIM_OFFS           (108)
#define EFUSE_PMU_TRIM_WORD_LDO8_TRIM_LEN            (5)

#define EFUSE_PMU_TRIM_WORD_32KRC_FREQ_CTUNE_LV_OFFS (113)
#define EFUSE_PMU_TRIM_WORD_32KRC_FREQ_CTUNE_LV_LEN  (6)

#define EFUSE_PMU_TRIM_WORD_32KRC_FREQ_FTUNE_LV_OFFS (119)
#define EFUSE_PMU_TRIM_WORD_32KRC_FREQ_FTUNE_LV_LEN  (4)

#define EFUSE_PMU_TRIM_WORD_DIG_BGM_VBG_0P8_TRIM_OFFS (124)
#define EFUSE_PMU_TRIM_WORD_DIG_BGM_VBG_0P8_TRIM_LEN  (4)

#define EFUSE_PMU_TRIM_WORD_DIG_BGM_RTRIM_OFFS        (128)
#define EFUSE_PMU_TRIM_WORD_DIG_BGM_RTRIM_LEN         (5)

#define EFUSE_PMU_TRIM_WORD_DIG_BGM_ITRIM_OFFS        (133)
#define EFUSE_PMU_TRIM_WORD_DIG_BGM_ITRIM_LEN         (5)

#define EFUSE_PMU_TRIM_WORD_DIG_AUX_ADC0_SBGR045_OFFS (138)
#define EFUSE_PMU_TRIM_WORD_DIG_AUX_ADC0_SBGR045_LEN  (12)

#define EFUSE_PMU_TRIM_WORD_DIG_AUX_ADC0_SBGR135_OFFS (150)
#define EFUSE_PMU_TRIM_WORD_DIG_AUX_ADC0_SBGR135_LEN  (12)

#define EFUSE_PMU_TRIM_WORD_DIG_AUX_ADC3_SBGR16_OFFS  (162)
#define EFUSE_PMU_TRIM_WORD_DIG_AUX_ADC3_SBGR16_LEN   (12)

#define EFUSE_PMU_TRIM_WORD_DIG_AUX_ADC3_SBGR08_OFFS  (176)
#define EFUSE_PMU_TRIM_WORD_DIG_AUX_ADC3_SBGR08_LEN   (12)

#define EFUSE_DIE_ID_WORD_ID0_OFFS                    (192)
#define EFUSE_DIE_ID_WORD_ID0_LEN                     (32)

#define EFUSE_DIE_ID_WORD_ID1_OFFS                    (224)
#define EFUSE_DIE_ID_WORD_ID1_LEN                     (32)

#define EFUSE_PUK_HASH_WORD_DIGEST0_OFFS              (256)
#define EFUSE_PUK_HASH_WORD_DIGEST0_LEN               (32)

#define EFUSE_PUK_HASH_WORD_DIGEST1_OFFS              (288)
#define EFUSE_PUK_HASH_WORD_DIGEST1_LEN               (32)

#define EFUSE_PUK_HASH_WORD_DIGEST2_OFFS              (320)
#define EFUSE_PUK_HASH_WORD_DIGEST2_LEN               (32)

#define EFUSE_PUK_HASH_WORD_DIGEST3_OFFS              (352)
#define EFUSE_PUK_HASH_WORD_DIGEST3_LEN               (32)

#define EFUSE_PUK_HASH_WORD_DIGEST4_OFFS              (384)
#define EFUSE_PUK_HASH_WORD_DIGEST4_LEN               (32)

#define EFUSE_PUK_HASH_WORD_DIGEST5_OFFS              (416)
#define EFUSE_PUK_HASH_WORD_DIGEST5_LEN               (32)

#define EFUSE_PUK_HASH_WORD_DIGEST6_OFFS              (448)
#define EFUSE_PUK_HASH_WORD_DIGEST6_LEN               (32)

#define EFUSE_PUK_HASH_WORD_DIGEST7_OFFS              (480)
#define EFUSE_PUK_HASH_WORD_DIGEST7_LEN               (32)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

#define EFUSE_OP_SINGLE_READ       (0)
#define EFUSE_OP_SINGLE_WRITE      (2)

#define EFUSE_OP_LOCK()        (osMutexAcquire(&g_eManager.eMutex, osWaitForever))
#define EFUSE_OP_UNLOCK()      (osMutexRelease(&g_eManager.eMutex))

/**
 * @brief  EFUSE Ctrl
 */
typedef struct
{
   __IO uint32_t version;
   __IO uint32_t rsv0[3];
   __IO uint32_t pd;
   __IO uint32_t ps;
   __IO uint32_t csb;
   __IO uint32_t mr;
   __IO uint32_t op_code;
   __IO uint32_t op_addr;
   __IO uint32_t op_start;
   __IO uint32_t op_rdata;
   __IO uint32_t misc;
   __IO uint32_t timing;
   __IO uint32_t rsv1;
   __IO uint32_t sec_ctrl;
} EFUSE_CtrlRegs;


typedef struct
{
    uint8_t op;           ///< read or write
    uint16_t block_index; ///< block index
    union {
        uint32_t *rval; ///< read value pointer
        uint32_t wval;  ///< write value
    };
} EfuseReadWriteOp_t;

/**
 * @brief  Efuse mutex
 */
typedef struct {
    struct osMutex  eMutex;    ///< 读写 efuse 操作互斥体
} EFUSE_Manager;


/************************************************************************************
 *                                 变量定义
 ************************************************************************************/
extern EFUSE_Manager g_eManager;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/

int EFUSE_Init(void);

/**
 ************************************************************************************************************************
 * @brief       EFUSE 读接口.
 * @param[in]   index             EFUSE word 索引号.
 * @param[in]   val               EFUSE 读出值.
 * @return      int32_t.
 * @retval      DRV_OK             成功.
 ************************************************************************************************************************
 */

int32_t EFUSE_Read(uint32_t index, uint32_t *val);

/**
 ************************************************************************************************************************
 * @brief       EFUSE 写接口.
 * @param[in]   index             EFUSE word 索引号.
 * @param[in]   val               EFUSE 写入值.
 * @return      int32_t.
 * @retval      DRV_OK             成功.
 ************************************************************************************************************************
 */

int32_t EFUSE_Write(uint32_t index, uint32_t val);

/**
 ************************************************************************************************************************
 * @brief       EFUSE bits读接口.
 * @param[in]   bitIndex          EFUSE bit 索引号.
 * @param[in]   bitLen            EFUSE 待读取 bits长度.
 * @param[in]   bitVal            EFUSE 读出值.
 * @return      int32_t.
 * @retval      DRV_OK             成功.
 ************************************************************************************************************************
 */

int32_t EFUSE_ReadBits(uint32_t bitIndex, uint32_t bitLen, uint32_t *bitVal);

/**
 ************************************************************************************************************************
 * @brief        EfuseCacheReadWrite接口.
 * @param[in]          .
 * @return      bool.
 * @retval      DRV_OK             成功.
 ************************************************************************************************************************
 */

bool EfuseCacheReadWrite(EfuseReadWriteOp_t *params, uint32_t count);

/**
 ************************************************************************************************************************
 * @brief       获取SECUREBOOT 版本号接口.
 * @param[in]          .
 * @return      uint8_t.
 * @retval      verno              成功.
 ************************************************************************************************************************
 */

uint8_t EFUSE_GetSecVer(void);

#endif /* __DRV_EFUSE_H__ */

/** @} */