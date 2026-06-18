/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        nvparam_top.h
 *
 * @brief       NV顶层定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */


#ifndef _NV_TOP_LAYOUT_H
#define _NV_TOP_LAYOUT_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <nv/nvparam_config.h>
#include <nv/nvparam_pubcfg.h>
#ifdef BUILD_IN_SDK
#include <nvparam_customcfg.h>

/********************************************
****define the nv section address micro******
*********************************************/
#define APPCFG_CFUN1_NVADDR                 (NV_ITEM_ADDR(pubConfig.appConfig.auto_cfun1))
#define APPCFG_CID_NVADDR                   (NV_ITEM_ADDR(pubConfig.appConfig.auto_connect_cid))
#define APPCFG_SLEEPMODE_NVADDR             (NV_ITEM_ADDR(pubConfig.appConfig.sleep_mode))
#define APPCFG_LEDNET_NVADDR                (NV_ITEM_ADDR(pubConfig.appConfig.led_net))
#define APPCFG_LEDSTATUS_NVADDR             (NV_ITEM_ADDR(pubConfig.appConfig.led_status))
#define APPCFG_DIALMODE_NVADDR              (NV_ITEM_ADDR(pubConfig.appConfig.dial_mode))
#define APPCFG_AUTOCONN_NVADDR              (NV_ITEM_ADDR(pubConfig.appConfig.auto_conn))
#define APPCFG_FORCESCANEARFCN_NVADDR       (NV_ITEM_ADDR(pubConfig.appConfig.force_scan_earfcn))
#define APPCFG_NTWORKMODE_NVADDR            (NV_ITEM_ADDR(pubConfig.appConfig.work_mode))

#endif
/************************************************************************************
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
/**
 * @brief 获取 NV 项的地址.
 */
#define NV_ITEM_ADDR(item)          ((unsigned int)__builtin_offsetof(NV_Default, item))

/**
 * @brief 获取 NV 项的大小.
 */
#define NV_ITEM_SIZE(item)          ((unsigned int)sizeof(((NV_Default*)(0x100))->item))

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
/**
 * @struct NV_Default
 * @brief 顶层 NV 空间布局定义结构体, 禁止修改
 */
typedef struct {
    union {
    uint8_t         Modem;
    uint8_t         ModemArea[NV_MODEM_SIZE];
    };

    union {
    NV_PubConfig    pubConfig;
    uint8_t         pubConfigArea[NV_PUBCONFIG_SIZE];
    };

    union {
#ifdef BUILD_IN_SDK
    NV_CustomConfig customCfg;
#endif
    uint8_t         customCfgArea[NV_CUSTOMCFG_SIZE];
    };

    union {
    uint8_t         flag;
    uint8_t         flagArea[NV_FLAGINFO_SIZE];
    };

}NV_Default;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
static void NV_CheckTopSize(void)
{
    BUILD_BUG_ON(sizeof(NV_Default) > NV_LEAVE_FACTORY_SIZE);
    BUILD_BUG_ON(NV_ITEM_SIZE(Modem) > NV_ITEM_SIZE(ModemArea));
    BUILD_BUG_ON(NV_ITEM_SIZE(pubConfig) > NV_ITEM_SIZE(pubConfigArea));
#ifdef BUILD_IN_SDK
    BUILD_BUG_ON(NV_ITEM_SIZE(customCfg) > NV_ITEM_SIZE(customCfgArea));
#endif
    BUILD_BUG_ON(NV_ITEM_SIZE(flag) > NV_ITEM_SIZE(flagArea));
}

#endif /*_NV_TOP_LAYOUT_H*/
