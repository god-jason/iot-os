/**
 * @file        cm_demo_dmp.c
 * @brief       DMP入库功能认证，示例仅供参考
 * @copyright   Copyright © 2024 China Mobile IOT. All rights reserved.
 * @author      by shimingrui
 */

#ifdef CM_DEMO_DMP_SUPPORT
#ifdef OS_USING_SHELL

#include <os.h>
#include <string.h>
#include <nr_micro_shell.h>
#include "cm_dmp.h"
#include "cm_sys.h"
#include "cm_pm.h"
#include "cm_os.h"
#include "cm_common_api.h"

#define CM_DEMO_DMP_LOG     cm_demo_printf
#define cm_demo_printf osPrintf

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define CMDMP_GENERAL_STRING_MAX_LEN      16
#define CMDMP_APPINFO_STRING_MAX_LEN      128

typedef struct{
    int enable;
    int interval;
    char appkey[CMDMP_GENERAL_STRING_MAX_LEN];//M100000052
    char pswd[3*CMDMP_GENERAL_STRING_MAX_LEN];
    int tmltype;
    char host[3*CMDMP_GENERAL_STRING_MAX_LEN];
    int port;
    char appinfo[3*CMDMP_GENERAL_STRING_MAX_LEN];
    char mac[3*CMDMP_GENERAL_STRING_MAX_LEN];
    char cpu[CMDMP_GENERAL_STRING_MAX_LEN];
    char rom[8];
    char ram[8];
    char osver[CMDMP_GENERAL_STRING_MAX_LEN];
    char swver[CMDMP_GENERAL_STRING_MAX_LEN];
    char swname[CMDMP_GENERAL_STRING_MAX_LEN];
    char volte[8];
    char nettype[CMDMP_GENERAL_STRING_MAX_LEN];
    char account[CMDMP_GENERAL_STRING_MAX_LEN];
    char phonenum[CMDMP_GENERAL_STRING_MAX_LEN];
    char location[CMDMP_GENERAL_STRING_MAX_LEN];
#ifdef DM40_SUPPORT
    char brand[CMDMP_GENERAL_STRING_MAX_LEN];
    char model[32];
    char tmplID[CMDMP_GENERAL_STRING_MAX_LEN];
    char BatCap[CMDMP_GENERAL_STRING_MAX_LEN];
    char scrnSz[CMDMP_GENERAL_STRING_MAX_LEN];
    char rtMac[3*CMDMP_GENERAL_STRING_MAX_LEN];
    char btMac[3*CMDMP_GENERAL_STRING_MAX_LEN];
    char gpu[CMDMP_GENERAL_STRING_MAX_LEN];
    char board[CMDMP_GENERAL_STRING_MAX_LEN];
    char RES[CMDMP_GENERAL_STRING_MAX_LEN];
    int wearing;
#endif
} cmdmp_nv_cfg_t;

/*
	appkey 和 pswd 填入自己申请到的账号密码或者联系技术支持获取测试值
*/
static cmdmp_nv_cfg_t s_cmdmp_nv_cfg = {
    .enable = 1,
    .interval = 1,
    .appkey = "",
    .pswd = "",
#ifdef DM40_SUPPORT	
    .tmplID = "TY000123",
#endif
    .host = "shipei.fxltsbl.com",
    .port = 5683
};

static void cm_mobile_dm_init(void)
{
    /*判断是否启用DM功能*/
    if(s_cmdmp_nv_cfg.enable == 0)
    {
        cm_demo_printf("dm func disabled\r\n");
        return;
    }

    int appkey_len = strlen(s_cmdmp_nv_cfg.appkey);
    int pswd_len = strlen(s_cmdmp_nv_cfg.pswd);
    int host_len = strlen(s_cmdmp_nv_cfg.host);

    /*参数有效性检测*/
    if(appkey_len == 0 ||
       pswd_len == 0 ||
       host_len == 0 ||
       s_cmdmp_nv_cfg.interval < 1 || s_cmdmp_nv_cfg.interval > 1440)
    {
        cm_demo_printf("dm param err\r\n");
        return;
    }

   int appinfo_len = strlen(s_cmdmp_nv_cfg.appinfo);
   int mac_len = strlen(s_cmdmp_nv_cfg.mac);
   int rom_len = strlen(s_cmdmp_nv_cfg.rom);
   int ram_len = strlen(s_cmdmp_nv_cfg.ram);
   int cpu_len = strlen(s_cmdmp_nv_cfg.cpu);
   int osver_len = strlen(s_cmdmp_nv_cfg.osver);
   int swver_len = strlen(s_cmdmp_nv_cfg.swver);
   int swname_len = strlen(s_cmdmp_nv_cfg.swname);
   int volte_len = strlen(s_cmdmp_nv_cfg.volte);
   int nettype_len = strlen(s_cmdmp_nv_cfg.nettype);
   int account_len = strlen(s_cmdmp_nv_cfg.account);
   int phone_len = strlen(s_cmdmp_nv_cfg.phonenum);
   int location_len = strlen(s_cmdmp_nv_cfg.location);

#ifdef DM40_SUPPORT	
    int brand_len = strlen(s_cmdmp_nv_cfg.brand);
    int model_len = strlen(s_cmdmp_nv_cfg.model);
    int tmplID_len = strlen(s_cmdmp_nv_cfg.tmplID);
    int BatCap_len = strlen(s_cmdmp_nv_cfg.BatCap);
    int scrnSz_len = strlen(s_cmdmp_nv_cfg.scrnSz);
    int rtMac_len = strlen(s_cmdmp_nv_cfg.rtMac);
    int btMac_len = strlen(s_cmdmp_nv_cfg.btMac);
    int gpu_len = strlen(s_cmdmp_nv_cfg.gpu);
    int board_len = strlen(s_cmdmp_nv_cfg.board);
    int RES_len = strlen(s_cmdmp_nv_cfg.RES);
#endif

    /*参数设置*/
    cm_mobile_dm_set_option(CM_MOBILE_DM_APPKEY, appkey_len == 0 ? NULL : s_cmdmp_nv_cfg.appkey, appkey_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_PASSWORD, pswd_len == 0 ? NULL : s_cmdmp_nv_cfg.pswd, pswd_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_HOST, host_len == 0 ? NULL : s_cmdmp_nv_cfg.host, host_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_APPINFO, appinfo_len == 0 ? NULL : s_cmdmp_nv_cfg.appinfo, appinfo_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_MAC, mac_len == 0 ? NULL : s_cmdmp_nv_cfg.mac, mac_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_ROM, rom_len == 0 ? NULL : s_cmdmp_nv_cfg.rom, rom_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_RAM, ram_len == 0 ? NULL : s_cmdmp_nv_cfg.ram, ram_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_CPU, cpu_len == 0 ? NULL : s_cmdmp_nv_cfg.cpu, cpu_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_OSVER, osver_len == 0 ? NULL : s_cmdmp_nv_cfg.osver, osver_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_SWVER, swver_len == 0 ? NULL : s_cmdmp_nv_cfg.swver, swver_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_SWNAME, swname_len == 0 ? NULL : s_cmdmp_nv_cfg.swname, swname_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_VOLTE, volte_len == 0 ? NULL : s_cmdmp_nv_cfg.volte, volte_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_NETTYPE, nettype_len == 0 ? NULL : s_cmdmp_nv_cfg.nettype, nettype_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_ACCOUNT, account_len == 0 ? NULL : s_cmdmp_nv_cfg.account, account_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_PHONENUM, phone_len == 0 ? NULL : s_cmdmp_nv_cfg.phonenum, phone_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_LOCATION, location_len == 0 ? NULL : s_cmdmp_nv_cfg.location, location_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_INTERVAL, &(s_cmdmp_nv_cfg.interval), sizeof(s_cmdmp_nv_cfg.interval));
    cm_mobile_dm_set_option(CM_MOBILE_DM_PORT, &(s_cmdmp_nv_cfg.port), sizeof(s_cmdmp_nv_cfg.port));
    cm_mobile_dm_set_option(CM_MOBILE_DM_TMLTYPE, &(s_cmdmp_nv_cfg.tmltype), sizeof(s_cmdmp_nv_cfg.tmltype));

#ifdef DM40_SUPPORT	
    cm_mobile_dm_set_option(CM_MOBILE_DM_BRAND, brand_len == 0 ? NULL : s_cmdmp_nv_cfg.brand, brand_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_MODEL, model_len == 0 ? NULL : s_cmdmp_nv_cfg.model, model_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_TMPLID, tmplID_len == 0 ? NULL : s_cmdmp_nv_cfg.tmplID, tmplID_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_BATCAP, BatCap_len == 0 ? NULL : s_cmdmp_nv_cfg.BatCap, BatCap_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_SRCNSZ, scrnSz_len == 0 ? NULL : s_cmdmp_nv_cfg.scrnSz, scrnSz_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_RTMAC, rtMac_len == 0 ? NULL : s_cmdmp_nv_cfg.rtMac, rtMac_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_BTMAC, btMac_len == 0 ? NULL : s_cmdmp_nv_cfg.btMac, btMac_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_GPU, gpu_len == 0 ? NULL : s_cmdmp_nv_cfg.gpu, gpu_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_BOARD, board_len == 0 ? NULL : s_cmdmp_nv_cfg.board, board_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_RES, RES_len == 0 ? NULL : s_cmdmp_nv_cfg.RES, RES_len);
    cm_mobile_dm_set_option(CM_MOBILE_DM_WEARING, &(s_cmdmp_nv_cfg.wearing), sizeof(s_cmdmp_nv_cfg.wearing));
    cm_mobile_dm_set_option(CM_MOBILE_DM_ENABLE, &(s_cmdmp_nv_cfg.enable), sizeof(s_cmdmp_nv_cfg.enable));	
#endif

    /*启动*/
    cm_mobile_dm_start();
}

static void cm_dm40_test(void)
{
    /* 等待模组PDP激活 */
    int32_t pdp_time_out = 0;
    
    while(1)
    {
        if(pdp_time_out > 20)
        {
            cm_demo_printf("network timeout\r\n");
            cm_pm_reboot(0);
        }
        if(cm_modem_get_pdp_state(1) == 1)
        {
            cm_demo_printf("network ready\r\n");
            break;
        }
        osDelay(200);
        pdp_time_out++;
    }

    /* 适当等待（模组PDP激活到能进行数据通信需要适当等待，或者通过PING操作确定模组数据通信是否可用） */
    osDelay(1000);
    
    cm_mobile_dm_init();
}

void cm_test_dmp(int argc, char **argv)
{
    if(argv == NULL || argc < 2)
    {
        cm_demo_printf("invalid param\r\n");
        return;
    }

    if (argv[1] == NULL)
    {
        cm_demo_printf("cm_test_dmp: argv[1] is NULL\n");
        return;
    }
    const char *cmd = argv[1];

    if (strncmp(cmd, "test", 4) == 0)
    {
        cm_demo_printf("dmp test start!!\r\n");
        /* 创建线程执行测试功能 */
        cm_thread_create("dmp_test_thread", 4096 * 2, osPriorityNormal, (cm_thread_func_t)cm_dm40_test);
    }
    else
    {
        cm_demo_printf("invalid cmd=%s error\r\n", cmd);
    }
}

NR_SHELL_CMD_EXPORT(dmp, cm_test_dmp);

#endif
#endif
