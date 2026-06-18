/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        nvshare_table.h
 *
 * @brief       CP 共享 NV 表定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */


#ifndef NVSHARE_TABLE_H
#define NVSHARE_TABLE_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <top_ram_config.h>
#include <nvparam_config.h>
/************************************************************************************
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define DDR_BASE_ADDR_PHY_NV    DRAM_BASE_ADDR_NV_NC

#define NVF_RAM_ADDR            DDR_BASE_ADDR_PHY_NV
#define NVF_RAM_LEN             (NV_FACTORY_RESV_SIZE)
#define NVF_NVADDR              (0)
#define NVF_NVLEN               (NVF_RAM_LEN)

#define NVD_PHYUSER_RAM_ADDR    (NVF_RAM_ADDR + NVF_NVLEN)
#define NVD_PHYUSER_RAM_LEN     (MODEM_PHYUSER_SIZE)
#define NVD_PHYUSER_NVADDR      (0)
#define NVD_PHYUSER_NVLEN       (MODEM_PHYUSER_SIZE)

#define NVD_ROPLAT_RAM_ADDR     (NVD_PHYUSER_RAM_ADDR + NVD_PHYUSER_RAM_LEN)
#define NVD_ROPLAT_RAM_LEN      (MODEM_ROPLAT_SIZE)
#define NVD_ROPLAT_NVADDR       (NVD_PHYUSER_NVADDR + NVD_PHYUSER_NVLEN)
#define NVD_ROPLAT_NVLEN        (MODEM_ROPLAT_SIZE)

#define NVD_ROPSCFG_RAM_ADDR    (NVD_ROPLAT_RAM_ADDR + NVD_ROPLAT_RAM_LEN)
#define NVD_ROPSCFG_RAM_LEN     (MODEM_ROPSCFG_SIZE)
#define NVD_ROPSCFG_NVADDR      (NVD_ROPLAT_NVADDR + NVD_ROPLAT_NVLEN)
#define NVD_ROPSCFG_NVLEN       (MODEM_ROPSCFG_SIZE)

#define NVD_RWPLAT_RAM_ADDR     (NVD_ROPSCFG_RAM_ADDR + NVD_ROPSCFG_RAM_LEN)
#define NVD_RWPLAT_RAM_LEN      (MODEM_RWPLAT_SIZE)
#define NVD_RWPLAT_NVADDR       (NVD_ROPSCFG_NVADDR + NVD_ROPSCFG_NVLEN)
#define NVD_RWPLAT_NVLEN        (MODEM_RWPLAT_SIZE)

#define NVD_RWPSCFG_RAM_ADDR    (NVD_RWPLAT_RAM_ADDR + MODEM_RWPLAT_SIZE)
#define NVD_RWPSCFG_RAM_LEN     (MODEM_RWPSCFG_SIZE)
#define NVD_RWPSCFG_NVADDR      (NVD_RWPLAT_NVADDR + NVD_RWPLAT_NVLEN)
#define NVD_RWPSCFG_NVLEN       (MODEM_RWPSCFG_SIZE)

#define NVD_RWPSPHYCFG_RAM_ADDR (NVD_RWPSCFG_RAM_ADDR)

#define NVD_PUBCFG_RAM_ADDR     (NVD_RWPSCFG_RAM_ADDR + NVD_RWPSCFG_RAM_LEN)
#define NVD_PUBCFG_RAM_LEN      (NV_ITEM_SIZE(pubConfig))
#define NVD_PUBCFG_NVADDR       (NV_ITEM_ADDR(pubConfig))
#define NVD_PUBCFG_NVLEN        (NV_ITEM_SIZE(pubConfig))


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef struct {
    uint32_t  nvAddr;           /* NV src addr(flash)*/
    uint32_t  ramAddr;          /* NV dst addr(DDRAM)*/
    uint32_t  size;             /* NV SIZE*/
    uint32_t  flag;             /* NV flag: 0 - nvf, 1 - nvd/nvw */
}NVM_NVShareItem;


/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
#if 0
/*    优化: 全局变量改为局部变量               */
const NVM_NVShareItem g_shareTable[] =
{
    /*NV base addr */           /*DDR base addr */      /*nv size*/         /*nv flag*/
    {NVF_NVADDR,                NVF_RAM_ADDR,           NVF_NVLEN,          0}, \
    {NVD_PHYUSER_NVADDR,        NVD_PHYUSER_RAM_ADDR,   NVD_PHYUSER_NVLEN,  1}, \
    {NVD_ROPLAT_NVADDR,         NVD_ROPLAT_RAM_ADDR,    0,                  1}, \
    {NVD_ROPSCFG_NVADDR,        NVD_ROPSCFG_RAM_ADDR,   NVD_ROPSCFG_NVLEN,  1}, \
    {NVD_RWPLAT_NVADDR,         NVD_RWPLAT_RAM_ADDR,    0,                  1}, \
    {NVD_RWPSCFG_NVADDR,        NVD_RWPSCFG_RAM_ADDR,   NVD_RWPSCFG_NVLEN,  1}, \
    {NVD_PUBCFG_NVADDR,         NVD_PUBCFG_RAM_ADDR,    NVD_PUBCFG_NVLEN,   1}, \
    {0,                         0,                      0,                  0}, \
};
#endif

    /*NV base addr */           /*DDR base addr */      /*nv size*/         /*nv flag*/
#define NV_SHARE_TABLE  {   \
    {NVF_NVADDR,                NVF_RAM_ADDR,           NVF_NVLEN,          0}, \
    {NVD_PHYUSER_NVADDR,        NVD_PHYUSER_RAM_ADDR,   NVD_PHYUSER_NVLEN,  1}, \
    {NVD_ROPLAT_NVADDR,         NVD_ROPLAT_RAM_ADDR,    0,                  1}, \
    {NVD_ROPSCFG_NVADDR,        NVD_ROPSCFG_RAM_ADDR,   NVD_ROPSCFG_NVLEN,  1}, \
    {NVD_RWPLAT_NVADDR,         NVD_RWPLAT_RAM_ADDR,    0,                  1}, \
    {NVD_RWPSCFG_NVADDR,        NVD_RWPSCFG_RAM_ADDR,   NVD_RWPSCFG_NVLEN,  1}, \
    {NVD_PUBCFG_NVADDR,         NVD_PUBCFG_RAM_ADDR,    NVD_PUBCFG_NVLEN,   1}, \
    {0,                         0,                      0,                  0}, \
}

#endif /*NVSHARE_TABLE_H*/
