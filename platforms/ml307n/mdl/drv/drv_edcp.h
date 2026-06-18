/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_edcp.h
 *
 * @brief       EDCP驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef DRV_EDCP_H_
#define DRV_EDCP_H_

/*************************************************************************
  *                                  Include files                       *
  *************************************************************************/
#include "drv_common.h"

/**
 * @addtogroup Edcp
 */

/**@{*/

#define EDCP_OK                         (DRV_OK)                        // 启动成功
#define EDCP_START_OK_NOT_RESTART       (DRV_ERR_SPECIFIC - 1)          // 启动成功：上行追加模式特有
#define EDCP_START_ERROR                (DRV_ERR_SPECIFIC - 2)          // 启动失败
#define EDCP_START_ERR_IN_WAIT_STATE    (DRV_ERR_SPECIFIC - 3)
#define EDCP_START_ERR_IN_BUSY_STATE    (DRV_ERR_SPECIFIC - 4)

typedef struct
{ 
    uint8_t     CtrlPaneCk[16];     // EDPCP_CK_I寄存器4*4bytes
    uint8_t     UserPaneCk[16];     // EDPCP_CK_C寄存器4*4bytes
    uint8_t     Ik[16];             // EDPCP_IK_I寄存器4*4bytes
    uint8_t     Direct;             // 上行0，下行1
    uint8_t     CipherType;         // 加密指示  : 0不加密，1 snow3g， 2 aes， 3 zuc
    uint8_t     IntegType;          // 完整性指示: 0无完保，1 snow3g， 2 aes， 3 zuc
}EDCP_StCfg;

typedef struct{ 
    uint16_t     CPInd          : 1;    // 加密指示:   0 不加密，1 加密
    uint16_t     IPInd          : 1;    // 完整性指示: 0 无完保，1 完保
    uint16_t     DataSourceType : 1;    // 数据源类型  0 用户面，1 控制面
    uint16_t     BearerId       : 5;    // 加密参数
    uint16_t     CompHeaderLen  : 8;    // 压缩头长度
    uint16_t     DateLen;               // 数据源长度
    uint32_t     Count;                 // 加密参数
    uint8_t     *pDataCompHeaderSrc;    // 压缩头地址
    uint8_t     *pDataDest;             // 目标地址
    uint8_t     *pDataSrc;              // 数据源地址
} EDCP_DyCfg;

typedef struct{
    uint32_t DataSourceType : 1;        // 数据块类型    0：数据面    1：控制面
    uint32_t IPInd          : 1;        // 完整性保护    0：禁用      1：使能
    uint32_t CPInd          : 1;        // 是否加密      0：禁用      1：使能
    uint32_t Reserve0       : 1;
    uint32_t MultiSegFlag   : 1;        // 是否分段      0：不分段    1：分段
    uint32_t Reserve1       : 11;
    uint32_t Total_DateLen  : 14;       // 总长度
    uint32_t Reserve2       : 2;
} Edcp_Conf;

typedef struct{
    uint32_t DataLen    : 14;           // 当前分段长度
    uint32_t Reserve0   : 2;
    uint32_t Bearer     : 5;            // RB-ID
    uint32_t Reserve1   : 11;
} Edcp_Length_Bearer;

typedef struct{
    Edcp_Conf           conf;
    uint32_t            datain_addr;
    Edcp_Length_Bearer  length_bearer;
    uint32_t            dataout_addr;
    uint32_t            count;
} EDCP_CtrlIndex;

typedef struct{ 
    uint32_t            srcAddr;
    uint32_t            dataLen     : 15;
    uint32_t            reserved    : 17;       // 包含中断使能，不使用
    uint32_t            desAddr;
} EDCP_EmacCtrlIndex;

typedef enum{
    EDCP_UL_MODULE = 0x1,
    EDCP_DL_MODULE = 0x2,
    EDCP_EMAC_MODULE = 0x4,
    EDCP_ALL_MODULE,
}EDCP_Module;

typedef enum {
    EDCP_STATUS_IDLE = 0,
    EDCP_STATUS_QUEUING,
    EDCP_STATUS_RUNNING,
}EDCP_Status;

typedef void (*EDCP_SignalEvent)(void *data, uint32_t event);


/**
 ************************************************************************************
 * @brief           复位EDCP
 *
 * @return          void
 ************************************************************************************
*/
void EDCP_Reset(void);

/**
 ************************************************************************************
 * @brief           初始化下行RAM地址（只支持非乒乓模式下使用）
 *
 * @return          下行RAM分配的地址
 ************************************************************************************
*/
uint8_t *EDCP_DlRamInit(uint32_t num);

/**
 ************************************************************************************
 * @brief           注册EDCP回调
 *
 * @param[in]       cb              回调接口
 * @param[in]       param           参数
 *
 * @return          void 
 ************************************************************************************
*/
void EDCP_CallbackRegister(EDCP_SignalEvent cb, void* param);

/**
 ************************************************************************************
 * @brief           配置EDCP上行静态参数
 *
 * @param[in]       config          上行配置
 *
 * @return          void
 ************************************************************************************
*/
void EDCP_UlStCfg(EDCP_StCfg *config);

/**
 ************************************************************************************
 * @brief           配置EDCP下行静态参数
 *
 * @param[in]       config          下行配置
 *
 * @return          void
 ************************************************************************************
*/
void EDCP_DlStCfg(EDCP_StCfg *config);

/**
 ************************************************************************************
 * @brief           写EDCP上行动态参数
 *
 * @param[in]       config          上行配置
 *
 * @return          void
 ************************************************************************************
*/
void EDCP_UlIndexWrite(EDCP_DyCfg *config);

/**
 ************************************************************************************
 * @brief           写EDCP下行动态参数
 *
 * @param[in]       config          下行配置
 *
 * @return          void
 ************************************************************************************
*/
void EDCP_DlIndexWrite(EDCP_CtrlIndex *index, uint32_t num);

/**
 ************************************************************************************
 * @brief           写EDCP EMAC动态参数
 *
 * @param[in]       index           配置
 * @param[in]       num             个数
 *
 * @return          void
 ************************************************************************************
*/
void EDCP_EmacIndexWrite(EDCP_EmacCtrlIndex *index, uint32_t num);

/**
 ************************************************************************************
 * @brief           启动EDCP
 *
 * @param[in]       module
 *                                  EDCP_UL_MODULE
 *                                  EDCP_DL_MODULE
 *                                  EDCP_EMAC_MODULE
 *
 * @return          void           不需要判断返回值，函数内部发生错误会直接断言
 ************************************************************************************
*/
void EDCP_Start(EDCP_Module module);

/**
 ************************************************************************************
 * @brief           返回EDCP DL状态
 * 
 * @return          EDCP_STATUS_IDLE            空闲
 *                  EDCP_STATUS_QUEUING         空闲
 *                  EDCP_STATUS_RUNNING         空闲
 ************************************************************************************
*/
EDCP_Status EDCP_StatusGet(EDCP_Module module);

/**
 ************************************************************************************
 * @brief           硬件memset
 * 
 * @param[in]       addr            需要memset的地址
 * @param[in]       data            需要memset的值
 * @param[in]       len             长度（最大支持0xffffff）
 * 
 * @return          EDCP_STATUS_IDLE            空闲
 *                  EDCP_STATUS_QUEUING         空闲
 *                  EDCP_STATUS_RUNNING         空闲
 ************************************************************************************
*/
void EDCP_Memset(uint32_t addr, uint8_t data, uint32_t len);

#endif

/** @} */