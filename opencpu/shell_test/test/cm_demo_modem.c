#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cm_os.h"
#include "cm_mem.h"
#include "cm_sys.h"
#include "cm_demo_uart.h"
#include "cm_demo_modem.h"
#include "cm_virt_at.h"
#include "cm_modem.h"
#include "cm_dialup.h"


static void __modem_call_cb(cm_call_info_t *info)
{
    if(info)
    {
        cm_demo_printf("__cm_mipcall_cb status:%d %d", info->cid, info->status);

        if(info->status)
        {
            if(info->ip[0] != '\0')
            {
                cm_demo_printf("__modem_call_cb ip:%s",  info->ip);
            }
            if(info->ipv6[0] != '\0')
            {
                cm_demo_printf("__modem_call_cb ipv6:%s",  info->ipv6);
            }
        }
    }
    else
    {
        cm_demo_printf("info NULL");
    }
}

static void __cm_dialup_cb(cm_dialup_info_t *info)
{
    cm_demo_printf("__cm_dialup_cb...");
    if (info)
    {
        cm_demo_printf("__cm_dialup_cb cid:%d status:%d type:%d\n", 
                       info->cid, info->status, info->type);
        
        if (info->type == 0) // IPv4
        {
            cm_demo_printf("IPv4: IP=%s GW=%s DNS1=%s DNS2=%s\n",
                          info->info.ipv4info.ip,
                          info->info.ipv4info.ip_gw,
                          info->info.ipv4info.ip_dns1,
                          info->info.ipv4info.ip_dns2);
        }
        else // IPv6
        {
            cm_demo_printf("IPv6: IP=%s GW=%s DNS1=%s DNS2=%s\n",
                          info->info.ipv6info.ipv6,
                          info->info.ipv6info.ipv6_gw,
                          info->info.ipv6info.ipv6_dns1,
                          info->info.ipv6info.ipv6_dns2);
        }
    }
    else
    {
        cm_demo_printf("Dialup callback info is NULL\n");
    }
}

/**
*  modem功能功能调试使用示例
*  modem info                           //获取常用modem信息
*  modem pdp_get                        //获取PDP1与PDP2信息
*  modem pdp_set                        //设置PDP2信息
*  modem call_get                       //获取第1路与第2路拨号连接信息
*  modem call                           //第2路拨号连接
*  modem discall                        //第2路断开拨号连接
*  modem dial                           //上位机拨号联网建立连接
*  modem hangup                         //上位机拨号联网断开连接
*  modem dial_get                       //获取上位机拨号联网信息
*/

void cm_test_modem(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1);
    if (cmd == NULL)
    {
        cm_demo_printf("invalid param\n");
        return;
    }

    if (0 == strcasecmp((const char *)cmd, "info"))
    {
        char cgmm[10] = {0};
        cm_modem_get_cgmm(cgmm);
        cm_demo_printf("cgmm:%s\n",cgmm);

        char cgmi[10] = {0};
        cm_modem_get_cgmi(cgmi);
        cm_demo_printf("cgmi:%s\n",cgmi);

        cm_cops_info_t *cops = NULL;
        cops = cm_malloc(sizeof(cm_cops_info_t));
        memset(cops,0,sizeof(cm_cops_info_t));
        cm_modem_get_cops(cops);
        cm_demo_printf("cops->mode:%d\n", cops->mode);
        if(cops->mode)
        {
            cm_demo_printf("cops->act:%d cops->format:%d, cops->oper:%s\n",cops->act, cops->format, cops->oper);
        } 
        cm_free(cops);

        char rssi;
        char ber;
        cm_modem_get_csq(&rssi, &ber);
        cm_demo_printf("rssi:%d,ber:%d\n",rssi, ber);


        cm_radio_info_t *radio_info = NULL;
        radio_info = cm_malloc(sizeof(cm_radio_info_t));
        memset(radio_info,0,sizeof(cm_radio_info_t));
        cm_modem_get_radio_info(radio_info);
        cm_demo_printf("radio_info->last_cellid:%d,radio_info->last_earfcn:%d,radio_info->last_ecl:%d,radio_info->last_pci:%d,radio_info->last_snr:%d,radio_info->rat:%d,radio_info->rsrp:%d,radio_info->rsrq:%d,radio_info->rssi:%d,radio_info->rx_time:%d,radio_info->rxlev:%d,radio_info->tx_power:%d\n",
                        radio_info->last_cellid,   radio_info->last_earfcn,   radio_info->last_ecl,   radio_info->last_pci,   radio_info->last_snr,   radio_info->rat,   radio_info->rsrp,   radio_info->rsrq,   radio_info->rssi,   radio_info->rx_time,   radio_info->rxlev,   radio_info->tx_power);
        cm_free(radio_info);


        cm_cell_info_t cell_info[1];
        cm_modem_get_cell_info(cell_info,1);
        cm_demo_printf("cell_info[0].bandwidth:%d,cell_info[0].earfcn:%d,cell_info[0].earfcn_offset:%d,cell_info[0].mcc:%s,cell_info[0].mnc:%s,cell_info[0].pci:%d,cell_info[0].primary_cell:%d,cell_info[0].rsrp:%d,cell_info[0].rsrq:%d,cell_info[0].rssi:%d,cell_info[0].snr:%d\n",
                        cell_info[0].bandwidth,   cell_info[0].earfcn,   cell_info[0].earfcn_offset,   cell_info[0].mcc,   cell_info[0].mnc,   cell_info[0].pci,   cell_info[0].primary_cell,   cell_info[0].rsrp,   cell_info[0].rsrq,   cell_info[0].rssi,   cell_info[0].snr);

        int get_fun = cm_modem_get_cfun();
        cm_demo_printf("get_fun:%d\n",get_fun);

        cm_edrx_cfg_get_t *edrx_cfg;
        edrx_cfg = cm_malloc(sizeof(cm_edrx_cfg_get_t));
        memset(edrx_cfg,0,sizeof(cm_edrx_cfg_get_t));
        cm_modem_get_edrx_cfg(edrx_cfg);
        cm_demo_printf("edrx_cfg->act_type:%d,edrx_cfg->requested_edrx_value:%d\n",edrx_cfg->act_type, edrx_cfg->requested_edrx_value);
        cm_free(edrx_cfg);

        cm_psm_cfg_t *psm_cfg = NULL;
        psm_cfg = cm_malloc(sizeof(cm_psm_cfg_t));
        memset(psm_cfg,0,sizeof(cm_psm_cfg_t));
        cm_modem_get_psm_cfg(psm_cfg);
        cm_demo_printf("psm_cfg->mode:%d, psm_cfg->requested_active_time:%d, psm_cfg->requested_periodic_tau:%d\n",psm_cfg->mode, psm_cfg->requested_active_time, psm_cfg->requested_periodic_tau);
        cm_free(psm_cfg);


        cm_cereg_state_t *cereg;
        cereg = cm_malloc(sizeof(cm_cereg_state_t));
        memset(cereg,0,sizeof(cm_cereg_state_t));
        cm_modem_get_cereg_state(cereg);
        cm_demo_printf("cereg->act:%d, cereg->active_time:%d, cereg->cause_type:%d,cereg->ci:%.8x, cereg->lac:%d,cereg->n:%d,cereg->periodic_tau:%d,cereg->rac:%d,cereg->reject_cause:%d,cereg->state:%d\n",
                        cereg->act,    cereg->active_time,    cereg->cause_type,   cereg->ci,    cereg->lac,cereg->n,      cereg->periodic_tau,   cereg->rac,   cereg->reject_cause,   cereg->state);
        cm_free(cereg);

        int get_cscon = cm_modem_get_cscon();
        cm_demo_printf("get_cscon:%d\n",get_cscon);

    }  
    else if (0 == strcasecmp((const char *)cmd, "pdp_get"))
    {
        cm_pdp_context_t pdp_context = {};

        memset(&pdp_context, 0, sizeof(pdp_context));
        cm_modem_get_pdp_context(1, &pdp_context);
        cm_demo_printf("pdp1_context: %d %s %s %s\n",
            pdp_context.pdp_type,pdp_context.apn,pdp_context.userid,pdp_context.password);

        memset(&pdp_context, 0, sizeof(pdp_context));
        cm_modem_get_pdp_context(2, &pdp_context);
        cm_demo_printf("pdp2_context: %d %s %s %s\n",
            pdp_context.pdp_type,pdp_context.apn,pdp_context.userid,pdp_context.password);
    }
    else if (0 == strcasecmp((const char *)cmd, "pdp_set"))
    {
        cm_pdp_context_t pdp_context = {};

        memset(&pdp_context, 0, sizeof(pdp_context));
        pdp_context.pdp_type = CM_PDP_TYPE_IPV4V6;
        sprintf((void *)&pdp_context.apn, "cmiot");
        cm_modem_set_pdp_context(2, &pdp_context);

        cm_modem_get_pdp_context(2, &pdp_context);
        cm_demo_printf("pdp2_context: %d %s %s %s\n",
            pdp_context.pdp_type,pdp_context.apn,pdp_context.userid,pdp_context.password);
    }
    else if (0 == strcasecmp((const char *)cmd, "call_get"))
    {
        cm_call_info_t call_info = {};
        int ret = 0;

        memset(&call_info, 0, sizeof(call_info));
        ret = cm_modem_get_call_status(1, &call_info);
        cm_demo_printf("call_status:%d %d\n", ret, call_info.status);
        if((ret == 0) && (call_info.status == 1))
        {
            cm_demo_printf("ip:%s %s\n", call_info.ip, call_info.ipv6);
        }
        
        memset(&call_info, 0, sizeof(call_info));
        ret = cm_modem_get_call_status(2, &call_info);
        cm_demo_printf("call_status:%d %d\n", ret, call_info.status);
        if((ret == 0) && (call_info.status == 1))
        {
            cm_demo_printf("ip:%s %s\n", call_info.ip, call_info.ipv6);
        }
    }  
    else if (0 == strcasecmp((const char *)cmd, "call"))
    {
        cm_modem_set_call_callback(__modem_call_cb);
        cm_modem_call(1, 2);
    }
    else if (0 == strcasecmp((const char *)cmd, "discall"))
    {
        cm_modem_set_call_callback(__modem_call_cb);
        cm_modem_call(0, 2);
    }
    else if (0 == strcasecmp((const char *)cmd, "dial"))
    {
        cm_dialup_t dialup = {0};
        dialup.interface = CM_DIALUP_INTERFACE_USB;
        
        cm_dialup_set_callback(__cm_dialup_cb);
        int32_t ret = cm_dialup(1, 1, &dialup); // 连接CID=1
        cm_demo_printf("cm_dialup(connect):%d\n", ret);
    }
    else if (0 == strcasecmp((const char *)cmd, "hangup"))
    {
        cm_dialup_t dialup = {0};
        dialup.interface = CM_DIALUP_INTERFACE_USB;
        
        int32_t ret = cm_dialup(0, 1, &dialup); // 断开CID=1
        cm_demo_printf("cm_dialup(disconnect):%d\n", ret);
    }
    else if (0 == strcasecmp((const char *)cmd, "dial_get"))
    {
        cm_dialup_info_t info = {0};
        int32_t ret = cm_dialup_get_status(1, 0, &info); // 查询CID=1 IPv4状态
        cm_demo_printf("cm_dialup_get_status:%d cid:%d status:%d\n", 
                      ret, info.cid, info.status);
        
        if (ret == 0 && info.status == 1)
        {
            if (info.type == 0)
            {
                cm_demo_printf("IP: %s\n", info.info.ipv4info.ip);
            }
        }
    }
    else if(0 == strcasecmp((const char *)cmd, "thp"))
    {
        cm_thp_info_t thp_info = {0};
        int ret = -2;
        ret = cm_modem_get_thp_info(&thp_info);
        cm_demo_printf("thp_info:ret:%d\nmac_dl:%d mac_ul:%d\n", ret, thp_info.mac_dl, thp_info.mac_ul);
    }
    else if(0 == strcasecmp((const char *)cmd, "c5greg"))
    {
        cm_cereg_state_t cereg_info = {0};
        cm_c5greg_state_t c5greg_info = {0};
        int ret = -2;
        ret = cm_modem_get_cereg_state(&cereg_info);
        cm_demo_printf( "cereg_info:ret:%d\nact:%d\nci:%.8x lac:%.4x n:%d state:%d\n", ret, cereg_info.act, cereg_info.ci, cereg_info.lac, cereg_info.n, cereg_info.state);
        ret = cm_modem_get_c5greg_state(&c5greg_info);
        cm_demo_printf( "c5greg_info:ret:%d\nact:%d\nci:%.8x lac:%.4x n:%d state:%d\n", ret, c5greg_info.act, c5greg_info.ci, c5greg_info.lac, c5greg_info.n, c5greg_info.state);
    }
    else if(0 == strcasecmp((const char *)cmd, "set_apn"))
    {
        const char *pdp_type = embeddedCliGetToken(args, 2);
        const char *apn = embeddedCliGetToken(args, 3);
        cm_default_apn_t set_apn = {0};
        
        // 将字符串类型的pdp_type转换为整数
        set_apn.pdp_type = atoi(pdp_type);
        
        if (apn) {
            strncpy((char*)set_apn.apn, apn, CM_SYS_MAX_APN_LEN);
            ((char*)set_apn.apn)[CM_SYS_MAX_APN_LEN] = '\0';
        }
        
        int ret = cm_modem_set_default_apn(&set_apn);
        cm_demo_printf("set_apn:%d\n", ret);
        cm_demo_printf("pdp_type:%d, apn:%s\n", set_apn.pdp_type, set_apn.apn);
    }
    else if(0 == strcasecmp((const char *)cmd, "get_apn"))
    {
        cm_default_apn_t get_apn = {0};
        int ret = cm_modem_get_default_apn(&get_apn);
        cm_demo_printf("ret:%d get_apn:%d, %s\n", ret, get_apn.pdp_type, get_apn.apn);
    }
    else
    {
        cm_demo_printf("Unknown command: %s\n", cmd);
    }
}
