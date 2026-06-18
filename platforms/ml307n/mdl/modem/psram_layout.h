/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        psram_layout.h
 *
 * @brief       psram地址规划.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-07-10   ICT Team        创建
 ************************************************************************************
 */

#ifndef _PSRAM_LAYOUT_H_
#define _PSRAM_LAYOUT_H_

#include <os_compiler.h>

#define DRAM_BASE_ADDR_CP               (0x80000000UL)
#if (DRAM_BASE_ADDR_CP != 0x80000000UL)
#error "DRAM_BASE_ADDR_CP != 0x80000000UL, must modify cpu-cp.lds cp.lua"
#endif
#define DRAM_BASE_LEN_CP                (352 * 1024UL)

/* AP and CP dram share flag start */
#define DRAM_BASE_ADDR_SHARE_FLAG       (DRAM_BASE_ADDR_CP + DRAM_BASE_LEN_CP)
#define DRAM_BASE_ADDR_SHARE_FLAG_NC    (DDR_ADDR_CACHE_TO_NONCACHE(DRAM_BASE_ADDR_SHARE_FLAG))
#define DRAM_BASE_LEN_SHARE_FLAG        (1 * 1024UL)

#define DRAM_BASE_ADDR_VERSION_SIZE     (DRAM_BASE_ADDR_SHARE_FLAG)
#define DRAM_BASE_ADDR_VERSION_SIZE_NC  (DRAM_BASE_ADDR_SHARE_FLAG_NC)  /* 0x40058000 */
#if (DRAM_BASE_ADDR_VERSION_SIZE != 0x80058000)
#error "DRAM_BASE_ADDR_VERSION_SIZE != 0x80058000, must modify"
#endif
#define DRAM_BASE_LEN_VERSION_SIZE      (128UL)

#define DRAM_BASE_ADDR_SCELL_MEAS_RSLT_NC   (DRAM_BASE_ADDR_VERSION_SIZE_NC + DRAM_BASE_LEN_VERSION_SIZE)
#define DRAM_BASE_LEN_SCELL_MEAS_RSLT       (16UL)

#define DRAM_BASE_ADDR_SIPCH_BUFF_NC        (DRAM_BASE_ADDR_SCELL_MEAS_RSLT_NC + DRAM_BASE_LEN_SCELL_MEAS_RSLT)
#define DRAM_BASE_LEN_SIPCH_BUFF            (576UL)

#define DRAM_BASE_ADDR_INSTRUMENT_CARD_FLAG_NC (DRAM_BASE_ADDR_SIPCH_BUFF_NC  + DRAM_BASE_LEN_SIPCH_BUFF)
#define DRAM_BASE_LEN_INSTRUMENT_CARD_FLAG  (4UL)

/* AP and CP dram share flag end */

#define DRAM_BASE_ADDR_LOG              (DRAM_BASE_ADDR_SHARE_FLAG + DRAM_BASE_LEN_SHARE_FLAG)
#define DRAM_BASE_ADDR_LOG_NC           (DDR_ADDR_CACHE_TO_NONCACHE(DRAM_BASE_ADDR_LOG))
#define DRAM_BASE_LEN_LOG               (103 * 1024UL)

#define DRAM_BASE_ADDR_MODEM_SHARE          (DRAM_BASE_ADDR_LOG + DRAM_BASE_LEN_LOG)
#define DRAM_BASE_ADDR_AP_MODEM_SHARE_NC    (DDR_ADDR_CACHE_TO_NONCACHE(DRAM_BASE_ADDR_MODEM_SHARE))
#define DRAM_BASE_LEN_AP_MODEM_SHARE        (160 * 1024UL)
#define DRAM_BASE_ADDR_CP_MODEM_SHARE_NC    (DRAM_BASE_ADDR_AP_MODEM_SHARE_NC + DRAM_BASE_LEN_AP_MODEM_SHARE)
#define DRAM_BASE_LEN_CP_MODEM_SHARE_SMALL  (82 * 1024UL)
#define DRAM_BASE_LEN_CP_MODEM_SHARE_BIG    (128 * 1024UL)
#define DRAM_BASE_LEN_CP_MODEM_SHARE        (DRAM_BASE_LEN_CP_MODEM_SHARE_SMALL + DRAM_BASE_LEN_CP_MODEM_SHARE_BIG)
#define DRAM_END_ADDR_CP_MODEM_SHARE_NC     (DRAM_BASE_ADDR_CP_MODEM_SHARE_NC + DRAM_BASE_LEN_CP_MODEM_SHARE)

/* Mutiplex AP modem share buf */
#define DRAM_BASE_ADDR_NV               (DRAM_BASE_ADDR_MODEM_SHARE)
#define DRAM_BASE_ADDR_NV_NC            (DDR_ADDR_CACHE_TO_NONCACHE(DRAM_BASE_ADDR_NV))
#define DRAM_BASE_LEN_NV                (96 * 1024UL)

/* Mutiplex CP modem share buf */
#define DRAM_BASE_ADDR_CP_WIFI_SCAN_NC  (DRAM_END_ADDR_CP_MODEM_SHARE_NC - DRAM_BASE_LEN_CP_WIFI_SCAN)
#define DRAM_BASE_LEN_CP_WIFI_SCAN      (42 * 1024UL)

#define DRAM_BASE_ADDR_AP_SHARE         (DRAM_BASE_ADDR_MODEM_SHARE + DRAM_BASE_LEN_AP_MODEM_SHARE + DRAM_BASE_LEN_CP_MODEM_SHARE)
#define DRAM_BASE_LEN_AP_SHARE          (192 * 1024UL)
#define DRAM_END_ADDR_AP_SHARE          (DRAM_BASE_ADDR_AP_SHARE  + DRAM_BASE_LEN_AP_SHARE)

/* Mutiplex ap share buf */
#define DRAM_BASE_ADDR_AMT_DATA         (DRAM_BASE_ADDR_AP_SHARE + 0x1000) //over dlmalloc top head
#define DRAM_BASE_ADDR_AMT_DATA_NC      (DDR_ADDR_CACHE_TO_NONCACHE(DRAM_BASE_ADDR_AP_SHARE))
#define DRAM_BASE_LEN_AMT_DATA          (64 * 1024UL)

#ifdef _OS_WIN
extern char ddr_base_addr_ap[];
#define DRAM_BASE_ADDR_AP               ((unsigned int)(ddr_base_addr_ap))
#else
#define DRAM_BASE_ADDR_AP               (DRAM_BASE_ADDR_AP_SHARE + DRAM_BASE_LEN_AP_SHARE)
#if (DRAM_BASE_ADDR_AP != 0x800FE800)
#error "DRAM_BASE_ADDR_AP != 0x800FE800, must modify cpu-ap.lds xmake.lua"
#endif
#endif

#ifdef OS_USING_CATCH_DATA
#define DRAM_BASE_LEN_AP                (2034 * 1024UL)
#else
#define DRAM_BASE_LEN_AP                (3058 * 1024UL)
#endif

#define DRAM_BASE_ADDR_APP_IMG          (DRAM_BASE_ADDR_AP + DRAM_BASE_LEN_AP)
#define DRAM_BASE_LEN_APP_IMG           (20 * 1024UL)


///////////////////////////// AMT交互RAM空间定义 /////////////////////////////

#ifdef _PLAT_PHY_FPGA
#define AMT_BUFFER_BASE_ADDR    (DRAM_BASE_ADDR_AMT_DATA_NC)
#elif defined _OS_WIN
#define AMT_BUFFER_BASE_ADDR    (DRAM_BASE_ADDR_AP)
#else
#define AMT_BUFFER_BASE_ADDR    (DRAM_BASE_ADDR_AMT_DATA_NC)
#endif

#define AMT_RECV_BUFFER_BASE    (AMT_BUFFER_BASE_ADDR)
#define AMT_RECV_BUFFER_SIZE    (0x8000>>CPU_SHIFT)//(0x80000)     // 数据区大小: 32K

#define AMT_SEND_BUFFER_BASE    (AMT_RECV_BUFFER_BASE + AMT_RECV_BUFFER_SIZE)
#define AMT_SEND_BUFFER_SIZE    (0x8000>>CPU_SHIFT)//(0x80000)     // 数据区大小: 32K


/* 物理层抓数区域，大小1MB */
#define DRAM_BASE_ADDR_CATCH_DATA       (0x80300000)
#define DRAM_BASE_ADDR_CATCH_DATA_NC    (DDR_ADDR_CACHE_TO_NONCACHE(DRAM_BASE_ADDR_CATCH_DATA))
#define DRAM_BASE_LEN_CATCH_DATA        (0x100000)
#define DRAM_BASE_ADDR_STUB_DATA        (0x40600000) // asic 需要修改

#endif  /* End of _PSRAM_LAYOUT_H_ */
