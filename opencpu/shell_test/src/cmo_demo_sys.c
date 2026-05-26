/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_SYS_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <nr_micro_shell.h>
#include "cm_sys.h"
#include "cm_modem_info.h"
#include "cm_sim.h"

#define cm_demo_printf osPrintf

/** 运营商信息 */
typedef struct
{
    uint8_t mode;         /**网络选择模式 */
    uint8_t format;       /**运营商信息格式 */
    uint8_t oper[17];     /**运营商信息 */
    uint8_t act;          /**网络接入技术 */
}cm_cops_info_t;

/** 无线信息 */


/** 小区信息 */

/** EDRX set配置 */
typedef struct
{
    uint8_t mode;       /**EDRX配置模式 */
    uint8_t act_type;   /**网络访问技术*/
    uint8_t requested_edrx_value; /**要配置的EDRX值*/
} cm_edrx_cfg_set_t;

/** EDRX get配置 */
typedef struct
{
    uint8_t act_type;   /**网络访问技术*/
    uint8_t requested_edrx_value; /**要配置的EDRX值*/
} cm_edrx_cfg_get_t;

/**PSM配置*/
typedef struct
{
    uint8_t mode;    /**模式*/
    uint8_t requested_periodic_tau; /**T3412值*/
    uint8_t requested_active_time;  /**T3324值*/
} cm_psm_cfg_t;

/**PS网络注册状态*/
typedef struct
{
    uint8_t n;            /**模式*/
    uint8_t state;        /**网络注册状态*/
    uint16_t lac;         /**位置区码*/
    uint32_t ci;          /**小区识别码*/
    uint8_t act;          /**网络访问技术*/
    uint8_t rac;          /**路由区域编码*/
    uint8_t cause_type;   /**reject_cause类型*/
    uint8_t reject_cause; /**注册失败原因*/
    uint8_t active_time;  /**T3324值*/
    uint8_t periodic_tau; /**T3412值*/
} cm_cereg_state_t;

void SHELL_testSys(char argc, char **argv)
{
    int ret;
    char buf[CM_VER_LEN] = {0};

    cm_demo_printf("\r\n======= CM OpenCPU testSys Starts =======\r\n");

    // cm_sys.h
    cm_demo_printf("\r\n[TEST] int32_t cm_sys_get_cm_ver(char *ver_buff, uint32_t len)\r\n");
    ret = cm_sys_get_cm_ver(NULL, CM_VER_LEN);
    cm_demo_printf("[ILLEGAL] ver_buff = NULL, ret[%d]\r\n", ret);
    ret = cm_sys_get_cm_ver(buf, CM_VER_LEN);
    cm_demo_printf("[LEGAL] SDK VERSION: %s, ret[%d]\r\n", buf, ret);

    memset(buf, 0, CM_VER_LEN);

    cm_demo_printf("\r\n[TEST] int32_t cm_sys_get_sn(char *sn)\r\n");
    ret = cm_sys_get_sn(NULL);
    cm_demo_printf("[ILLEGAL] sn = NULL, ret[%d]\r\n", ret);
    ret = cm_sys_get_sn(buf);
    cm_demo_printf("[LEGAL] SN: %s, ret[%d]\r\n", buf, ret);

    memset(buf, 0, CM_VER_LEN);

    cm_demo_printf("\r\n[TEST] int32_t cm_sys_get_imei(char *imei)\r\n");
    ret = cm_sys_get_imei(NULL);
    cm_demo_printf("[ILLEGAL] imei = NULL, ret[%d]\r\n", ret);
    ret = cm_sys_get_imei(buf);
    cm_demo_printf("[LEGAL] IMEI: %s, ret[%d]\r\n", buf, ret);

    // cm_sim.h
    cm_demo_printf("\r\n[TEST] int32_t cm_sim_get_cpin(void)\r\n");
    ret = cm_sim_get_cpin();
    cm_demo_printf("[LEGAL] CPIN: 0(READY),1(PIN),2(PUK), ret[%d]\r\n", ret);

    memset(buf, 0, CM_VER_LEN);

    cm_demo_printf("\r\n[TEST] int32_t cm_sim_get_imsi(char* imsi)\r\n");
    ret = cm_sim_get_imsi(NULL);
    cm_demo_printf("[ILLEGAL] imsi = NULL, ret[%d]\r\n", ret);
    ret = cm_sim_get_imsi(buf);
    cm_demo_printf("[LEGAL] IMSI: %s, ret[%d]\r\n", buf, ret);

    memset(buf, 0, CM_VER_LEN);

    cm_demo_printf("\r\n[TEST] int32_t cm_sim_get_iccid(char* iccid)\r\n");
    ret = cm_sim_get_iccid(NULL);
    cm_demo_printf("[ILLEGAL] iccid = NULL, ret[%d]\r\n", ret);
    ret = cm_sim_get_iccid(buf);
    cm_demo_printf("[LEGAL] ICCID: %s, ret[%d]\r\n", buf, ret);

    memset(buf, 0, CM_VER_LEN);

    cm_demo_printf("\r\n[TEST] int cm_sim_get_eid(char *eid)\r\n");
    ret = cm_sim_get_eid(NULL);
    cm_demo_printf("[ILLEGAL] eid = NULL, ret[%d]\r\n", ret);
    ret = cm_sim_get_eid(buf);
    cm_demo_printf("[LEGAL] EID: %s, ret[%d]\r\n", buf, ret);

    cm_sim_switch_t mode = {0};
    cm_demo_printf("\r\n[TEST] int cm_sim_get_switch(cm_sim_switch_t *mode)\r\n");
    ret = cm_sim_get_switch(NULL);
    cm_demo_printf("[ILLEGAL] mode = NULL, ret[%d]\r\n", ret);
    ret = cm_sim_get_switch(&mode);
    cm_demo_printf("[LEGAL] sim0_enable[%d],sim0_hotswap_enable[%d],sim1_enable[%d],sim1_hotswap_enable[%d], ret[%d]\r\n",
        mode.sim0_enable, mode.sim0_hotswap_enable, mode.sim1_enable, mode.sim1_hotswap_enable, ret);

    memset(&mode, 0, sizeof(cm_sim_switch_t));
    cm_demo_printf("\r\n[TEST] int cm_sim_set_switch(cm_sim_switch_t mode)\r\n");
    ret = cm_sim_set_switch(mode);
    cm_demo_printf("[ILLEGAL] sim0_enable&sim1_enable = false, ret[%d]\r\n", ret);
    mode.sim0_enable = true;
    mode.sim1_enable = true;
    ret = cm_sim_set_switch(mode);
    cm_demo_printf("[ILLEGAL] sim0_enable&sim1_enable = true, ret[%d]\r\n", ret);
    mode.sim0_enable = false;
    mode.sim1_enable = true;
    mode.sim1_hotswap_enable = true;
    ret = cm_sim_set_switch(mode);
    cm_demo_printf("[ILLEGAL] sim1_hotswap_enable = true, ret[%d]\r\n", ret);
    mode.sim0_enable = true;
    mode.sim1_enable = false;
    mode.sim1_hotswap_enable = false;
    ret = cm_sim_set_switch(mode);
    cm_demo_printf("[LEGAL] ret[%d] reboot to check hot plug disable\r\n", ret);

    memset(buf, 0, CM_VER_LEN);

    cm_demo_printf("\r\n[TEST] cm_log_printf(log_level, fmt, ...)\r\n");
    cm_log_printf(0, "[LEGAL] cm_log_printf test OK!\r\n");

    cm_demo_printf("\r\n======= CM OpenCPU testSys Ends =======\r\n");
}

int32_t cm_modem_get_pdp_state(uint16_t cid)
{
    int32_t pdp_state = -1;
    uint8_t rsp[20] = {0};
    int32_t rsp_len = 0;
    if(cm_virt_at_send_sync((const uint8_t *)"AT+CGACT?\r\n", rsp, &rsp_len, 32) == 0)
    {
        if (strstr((char *)rsp, "+CGACT") != NULL)
            {
                // 解析 CGACT 信息，数据格式: "+CGACT: <cid>,<state>"
                char *ptr = (char *)rsp;
                while (1)
                {
                    // 跳过以 "+" 开头的部分
                    ptr = strstr(ptr, "+CGACT:");
                    if (ptr == NULL) break; // 没有找到下一组数据，退出循环

                    // 解析字段
                    unsigned short cid_read = 0;
                    int state_read = 0;

                    // 使用 sscanf 读取 cid 和 state
                    if (sscanf(ptr, "+CGACT: %hu,%d", &cid_read, &state_read) == 2)
                    {
                        if (cid == cid_read)
                        {
                            pdp_state = state_read;
                            break; // 找到目标，退出循环
                        }
                    }

                    // 移动到下一个可能的 "+CGACT:" 子串
                    ptr += strlen("+CGACT:");
                }
            }
    }
    return pdp_state;
}

void SHELL_testModem(char argc, char **argv)
{
    char rsp[50] = {0};
    int32_t rsp_len = 0;

    cm_demo_printf("======= CM OpenCPU testModem Starts =======\r\n");

    int cm_get_pin = -1;
    if(cm_virt_at_send_sync((const uint8_t *)"AT+CPIN?\r\n", rsp, &rsp_len, 32) == 0)
    {
		if(NULL != strstr((char *)rsp, "+CPIN: READY"))
        {
            cm_get_pin = 0;
        }
    }
    cm_demo_printf("cm_get_pin:%d\r\n",cm_get_pin);

    // 不支持 AT+CMGMR
    /*
    memset(rsp, 0, 50);
    cm_virt_at_send_sync((const uint8_t *)"AT+CMGMR\r\n", (uint8_t *)rsp, &rsp_len, 32);
    cm_demo_printf("cgmr:%s\r\n",rsp);
    */

    memset(rsp, 0, 50);
    cm_virt_at_send_sync((const uint8_t *)"AT+CGMM\r\n", (uint8_t *)rsp, &rsp_len, 32);
    cm_demo_printf("cgmm:%s\r\n",rsp);

    memset(rsp, 0, 50);
    cm_virt_at_send_sync((const uint8_t *)"AT+CGMI\r\n", (uint8_t *)rsp, &rsp_len, 32);
    cm_demo_printf("cgmi:%s\r\n",rsp);

    cm_cops_info_t *cops = NULL;
    cops = (cm_cops_info_t *)osMalloc(sizeof(cm_cops_info_t));
    memset(cops,0,sizeof(cm_cops_info_t));
    memset(rsp, 0, 50);
    if(cm_virt_at_send_sync((const uint8_t *)"AT+COPS?\r\n", rsp, &rsp_len, 32) == 0)
    {
        if (strstr((char *)rsp, "+COPS") != NULL)
        {
            // 解析 COPS 信息，数据格式: "+COPS: <mode>,<format>,<oper>,<act>"
            sscanf((char *)rsp, "\r\n+COPS: %hhu,%hhu,%16s,%hhu",
            &cops->mode, &cops->format, cops->oper, &cops->act);
        }
    }
    cm_demo_printf("cops->mode:%d\r\n", cops->mode);
    if(cops->mode)
    {
        cm_demo_printf("cops->act:%d cops->format:%d, cops->oper:%s\r\n",cops->act, cops->format, cops->oper);
    }

    uint8_t rssi,ber = 0;
    memset(rsp, 0, 50);
    if(cm_virt_at_send_sync((const uint8_t *)"AT+CSQ\r\n", rsp, &rsp_len, 32) == 0)
    {
        if (strstr((char *)rsp, "+CSQ") != NULL)
        {
            // 解析 CSQ 信息，数据格式: "+CSQ: <rssi>,<ber>"
            sscanf((char *)rsp, "\r\n+CSQ: %hhu,%hhu", &rssi, &ber);
        }
    }
    cm_demo_printf("rssi:%u,ber:%u\r\n",rssi, ber);

    cm_radio_info_t *radio_info = NULL;
    radio_info = (cm_radio_info_t *)osMalloc(sizeof(cm_radio_info_t));
    memset(radio_info,0,sizeof(cm_radio_info_t));
    radio_info->rat = cops->mode;
    osFree(cops);

    if(0==cm_modem_info_radio(NULL))
    {
        cm_demo_printf("cm_modem_info_radio test fail\r\n");
    }
    cm_modem_info_radio(radio_info);
    cm_demo_printf("\r\nradio_info->last_cellid:%d, radio_info->last_earfcn:%d \r\n",
                    radio_info->last_cellid,   radio_info->last_earfcn);
    cm_demo_printf("radio_info->last_ecl:%d,    radio_info->last_pci:%d \r\n",
                    radio_info->last_ecl,   radio_info->last_pci);
    cm_demo_printf("radio_info->last_snr:%d,    radio_info->rat:%d \r\n",
                    radio_info->last_snr,   radio_info->rat);
    cm_demo_printf("radio_info->rsrp:%d,        radio_info->rsrq:%d   \r\n",
                    radio_info->rsrp,   radio_info->rsrq);
    cm_demo_printf("radio_info->rssi:%d,        radio_info->rx_time:%d   \r\n",
                    radio_info->rssi,   radio_info->rx_time);
    cm_demo_printf("radio_info->rxlev:%d,       radio_info->tx_power:%d   \r\n",
                    radio_info->rxlev,   radio_info->tx_power);
    osFree(radio_info);

    cm_cell_info_t cell_info[1];
    if(1!=cm_modem_info_cell(cell_info,1))
    {
        cm_demo_printf("cm_modem_info_cell test fail\r\n");
    }
    cm_demo_printf("\r\ncell_info[0].bandwidth:%d, cell_info[0].cid:%.8x   \r\n",
                    cell_info[0].bandwidth,   cell_info[0].cid);
    cm_demo_printf("cell_info[0].earfcn:%d,     cell_info[0].earfcn_offset:%d   \r\n",
                    cell_info[0].earfcn,   cell_info[0].earfcn_offset);
    cm_demo_printf("cell_info[0].mcc:%s,        cell_info[0].mnc:%s   \r\n",
                    cell_info[0].mcc,   cell_info[0].mnc);
    cm_demo_printf("cell_info[0].pci:%d,        cell_info[0].primary_cell:%d   \r\n",
                    cell_info[0].pci,   cell_info[0].primary_cell);
    cm_demo_printf("cell_info[0].rsrp:%d,       cell_info[0].rsrq:%d   \r\n",
                    cell_info[0].rsrp,   cell_info[0].rsrq);
    cm_demo_printf("cell_info[0].rssi:%d,       cell_info[0].snr:%d   \r\n",
                    cell_info[0].rssi,   cell_info[0].snr);
    cm_demo_printf("cell_info[0].tac:%d\n", cell_info[0].tac);

    int get_fun = -1;
    memset(rsp, 0, 50);
    if(cm_virt_at_send_sync((const uint8_t *)"AT+CFUN?\r\n", rsp, &rsp_len, 32) == 0)
    {
        if (strstr((char *)rsp, "+CFUN") != NULL)
        {
            // 解析 CFUN 信息，数据格式: "+CFUN: <cfun_value>"
            sscanf((char *)rsp, "\r\n+CFUN: %d", &get_fun);
        }
    }
    cm_demo_printf("\r\nget_fun:%d\r\n",get_fun);

    uint8_t act_type = 0;
    char edrx_value[10] = {0};
    memset(rsp, 0, 50);
    if(cm_virt_at_send_sync((const uint8_t *)"AT+CEDRXS?\r\n", rsp, &rsp_len, 32) == 0)
    {
        if (strstr((char *)rsp, "+CEDRXS") != NULL)
        {
            // 解析 CEDRXS 信息，数据格式: "+CEDRXS: <act_type>,<edrx_value>"
            sscanf((char *)rsp, "\r\n+CEDRXS: %d,\"%[^\"]", &act_type, edrx_value);
        }
    }
    cm_demo_printf("\r\nedrx_cfg->act_type:%d,edrx_cfg->requested_edrx_value:%s\r\n",act_type, edrx_value);

    char periodic_tau[10] = {0};
    char active_time[10] = {0};
    uint8_t mode = 0;
    memset(rsp, 0, 50);
    if(cm_virt_at_send_sync((const uint8_t *)"AT+CPSMS?\r\n", rsp, &rsp_len, 32) == 0)
    {
        sscanf((char *)rsp, "\r\n+CPSMS: %d,,,\"%[^\"]\",\"%[^\"]", &mode, periodic_tau, active_time);
    }
    cm_demo_printf("\r\npsm_cfg->mode:%d, psm_cfg->requested_active_time:%s, psm_cfg->requested_periodic_tau:%s\r\n",mode, active_time, periodic_tau);

    cm_cereg_state_t *cereg;
    cereg = (cm_cereg_state_t *)osMalloc(sizeof(cm_cereg_state_t));
    memset(cereg,0,sizeof(cm_cereg_state_t));
    memset(rsp, 0, 50);
    cm_virt_at_send_sync((const uint8_t *)"AT+CEREG=2\r\n", rsp, &rsp_len, 32);
    memset(rsp, 0, 50);
    if(cm_virt_at_send_sync((const uint8_t *)"AT+CEREG?\r\n", rsp, &rsp_len, 32) == 0)
    {
        if (strstr((char *)rsp, "+CEREG") != NULL)
        {
            // 解析 CEREG 信息，数据格式: "+CEREG: <n>,<state>,<lac>,<ci>,<act>"
            sscanf((char *)rsp, "\r\n+CEREG: %d,%d,\"%4x\",\"%8x\",%d",
                        &cereg->n, &cereg->state, &cereg->lac, &cereg->ci, &cereg->act);
        }
    }
    cm_demo_printf("\r\ncereg->act:%d,      cereg->state:%d   \r\n",
                    cereg->act,    cereg->state);
    cm_demo_printf("cereg->lac:%d,          cereg->ci:%.8x   \r\n",
                    cereg->lac,   cereg->ci);
    cm_demo_printf("cereg->n:%d   \r\n",cereg->n);
    osFree(cereg);

    memset(rsp, 0, 50);
    cm_virt_at_send_sync((const uint8_t *)"AT+CSCON?\r\n", rsp, &rsp_len, 32);
    cm_demo_printf("\r\nget_cscon:%s\r\n",rsp);

    int pdp_state = cm_modem_get_pdp_state(1);
    cm_demo_printf("\r\npdp_state:%d\r\n",pdp_state);

    cm_demo_printf("======= CM OpenCPU testModem ends =======\r\n");
}

void SHELL_testUiccSwitch(char argc, char **argv)
{
    int ret;
    cm_demo_printf("\r\n======= CM OpenCPU testUiccSwitch Starts =======\r\n");

    cm_sim_switch_t mode = {0};
    cm_demo_printf("\r\n[TEST] int cm_sim_get_switch(cm_sim_switch_t *mode)\r\n");
    ret = cm_sim_get_switch(&mode);
    cm_demo_printf("[LEGAL] sim0_enable[%d],sim0_hotswap_enable[%d],sim1_enable[%d],sim1_hotswap_enable[%d], ret[%d]\r\n",
        mode.sim0_enable, mode.sim0_hotswap_enable, mode.sim1_enable, mode.sim1_hotswap_enable, ret);

    cm_demo_printf("\r\n[TEST] int cm_sim_set_switch(cm_sim_switch_t *mode)\r\n");
    mode.sim0_enable = (mode.sim0_enable==true)?false:true;
    mode.sim1_enable = (mode.sim0_enable==true)?false:true;
    mode.sim0_hotswap_enable = true;
    ret = cm_sim_set_switch(mode);
    cm_demo_printf("[LEGAL] ret[%d] reboot to check switch to SIM\r\n", ret);

    cm_demo_printf("\r\n======= CM OpenCPU testUiccSwitch Ends =======\r\n");
}

NR_SHELL_CMD_EXPORT(cmo_sys, SHELL_testSys);
NR_SHELL_CMD_EXPORT(cmo_modem, SHELL_testModem);
NR_SHELL_CMD_EXPORT(cmo_uicc_switch, SHELL_testUiccSwitch);
#endif
#endif

