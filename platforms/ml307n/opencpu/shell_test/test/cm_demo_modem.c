#ifdef CM_DEMO_MODEM_SUPPORT
#ifdef OS_USING_SHELL
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cm_mem.h"
#include "cm_virt_at.h"
#include "cm_modem_info.h"
#include "cm_demo_common.h"

#define cm_demo_printf  osPrintf

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

static int argv_sanity_check(int argc, char **argv)
{
    if(argv == NULL)
    {
       cm_demo_printf("modem argv null\r\n");
       return -1;
    }
    if(argc < 2)
    {
        cm_demo_printf("modem argv too few\r\n");
        return -1;
    }
    for(int i = 0; i < argc; i++)
    {
        if(argv[i] == NULL)
        {
            cm_demo_printf("argv[%d] is NULL, invaild\r\n", i);
            return -2;
        }
    }
    return 0;
}

/**
*  modem功能功能调试使用示例
*  modem info                           //获取常用modem信息
*/
void cm_test_modem(char argc, char **argv)
{
    int ret = argv_sanity_check(argc, argv);
    if(ret != 0)
    {
        cm_demo_printf("argv_sanity_check failed ret[%d]\r\n",ret);
        return;
    }

    if ( argv[1] != NULL && 0 == strcmp(argv[1], "info"))
    {
        char cgmm[30] = {0};
        int32_t rsp_len = 0;
        cm_virt_at_send_sync((const uint8_t *)"AT+CGMM\r\n", (uint8_t *)cgmm, &rsp_len, 32);
        cm_demo_printf("cgmm:%s",cgmm+2);

        char cgmi[30] = {0};
        cm_virt_at_send_sync((const uint8_t *)"AT+CGMI\r\n", (uint8_t *)cgmi, &rsp_len, 32);
        cm_demo_printf("cgmi:%s",cgmi+2);

        cm_cops_info_t *cops = NULL;
        cops = cm_malloc(sizeof(cm_cops_info_t));
        char cops_rsp[50] = {0};
        if(cm_virt_at_send_sync((const uint8_t *)"AT+COPS?\r\n", cops_rsp, &rsp_len, 32) == 0)
        {
            if (strstr((char *)cops_rsp, "+COPS") != NULL)
            {
                // 解析 COPS 信息，数据格式: "+COPS: <mode>,<format>,<oper>,<act>"
                sscanf((char *)cops_rsp, "\r\n+COPS: %hhu,%hhu,%16s,%hhu",
                &cops->mode, &cops->format, cops->oper, &cops->act);
            }
        }
        cm_demo_printf("cops->mode:%d\n", cops->mode);
        if(cops->mode)
        {
            cm_demo_printf("cops->act:%d cops->format:%d, cops->oper:%s\n",cops->act, cops->format, cops->oper);
        }
        cm_free(cops);

        char s_rssi[16] = {0};
        char s_ber[16] = {0};
        cm_modem_get_csq(s_rssi, s_ber);
        cm_demo_printf("rssi:%d,ber:%d\n",atoi(s_rssi), atoi(s_ber));

        cm_radio_info_t *radio_info = NULL;
        radio_info = (cm_radio_info_t *)cm_malloc(sizeof(cm_radio_info_t));
        cm_modem_info_radio(radio_info);
        cm_demo_printf("radio_info->last_cellid:%d,radio_info->last_earfcn:%d,radio_info->last_ecl:%d,radio_info->last_pci:%d,radio_info->last_snr:%d\n",
                        radio_info->last_cellid,   radio_info->last_earfcn,   radio_info->last_ecl,   radio_info->last_pci,   radio_info->last_snr);
        cm_demo_printf("radio_info->rat:%d,radio_info->rsrp:%d,radio_info->rsrq:%d,radio_info->rssi:%d,radio_info->rx_time:%d,radio_info->rxlev:%d,radio_info->tx_power:%d\n",
                        radio_info->rat,   radio_info->rsrp,   radio_info->rsrq,   radio_info->rssi,   radio_info->rx_time,   radio_info->rxlev,   radio_info->tx_power);
        cm_free(radio_info);

        cm_cell_info_t cell_info[1];
        if(1!=cm_modem_info_cell(cell_info,1))
        {
            cm_demo_printf("cm_modem_info_cell test fail\r\n");
        }
        cm_demo_printf("cell_info[0].bandwidth:%d,cell_info[0].earfcn:%d,cell_info[0].earfcn_offset:%d,cell_info[0].mcc:%s,cell_info[0].mnc:%s,cell_info[0].pci:%d,cell_info[0].primary_cell:%d\n",
                        cell_info[0].bandwidth,   cell_info[0].earfcn,   cell_info[0].earfcn_offset,   cell_info[0].mcc,   cell_info[0].mnc,   cell_info[0].pci,   cell_info[0].primary_cell);
        cm_demo_printf("cell_info[0].rsrp:%d,cell_info[0].rsrq:%d,cell_info[0].rssi:%d,cell_info[0].snr:%d\n",
                        cell_info[0].rsrp,   cell_info[0].rsrq,   cell_info[0].rssi,   cell_info[0].snr);

        int get_fun = -1;
        char cfun_rsp[10] = {0};
        if(cm_virt_at_send_sync((const uint8_t *)"AT+CFUN?\r\n", cfun_rsp, &rsp_len, 32) == 0)
        {
            if (strstr(cfun_rsp, "+CFUN") != NULL)
            {
                // 解析 CFUN 信息，数据格式: "+CFUN: <cfun_value>"
                sscanf(cfun_rsp, "\r\n+CFUN: %d", &get_fun);
            }
        }
        cm_demo_printf("get_fun:%d\r\n",get_fun);

        uint8_t act_type = 0;
        char edrx_value[10] = {0};
        char cedrxs_rsp[50] = {0};
        if(cm_virt_at_send_sync((const uint8_t *)"AT+CEDRXS?\r\n", cedrxs_rsp, &rsp_len, 32) == 0)
        {
            if (strstr(cedrxs_rsp, "+CEDRXS") != NULL)
            {
                // 解析 CEDRXS 信息，数据格式: "+CEDRXS: <act_type>,<edrx_value>"
                sscanf(cedrxs_rsp, "\r\n+CEDRXS: %d,\"%[^\"]", &act_type, edrx_value);
            }
        }
        cm_demo_printf("nedrx_cfg->act_type:%d,edrx_cfg->requested_edrx_value:%s\n",act_type, edrx_value);

        char periodic_tau[10] = {0};
        char active_time[10] = {0};
        uint8_t mode = 0;
        char cpsms_rsp[50] = {0};
        if(cm_virt_at_send_sync((const uint8_t *)"AT+CPSMS?\r\n", cpsms_rsp, &rsp_len, 32) == 0)
        {
            sscanf(cpsms_rsp, "\r\n+CPSMS: %d,,,\"%[^\"]\",\"%[^\"]", &mode, periodic_tau, active_time);
        }
        cm_demo_printf("psm_cfg->mode:%d, psm_cfg->requested_active_time:%s, psm_cfg->requested_periodic_tau:%s\n",
                        mode, active_time, periodic_tau);

        cm_cereg_state_t *cereg;
        cereg = (cm_cereg_state_t *)cm_malloc(sizeof(cm_cereg_state_t));
        char cereg_rsp[50] = {0};
        cm_virt_at_send_sync((const uint8_t *)"AT+CEREG=2\r\n", cereg_rsp, &rsp_len, 32);
        if(cm_virt_at_send_sync((const uint8_t *)"AT+CEREG?\r\n", cereg_rsp, &rsp_len, 32) == 0)
        {
            if (strstr(cereg_rsp, "+CEREG") != NULL)
            {
                // 解析 CEREG 信息，数据格式: "+CEREG: <n>,<state>,<lac>,<ci>,<act>"
                sscanf(cereg_rsp, "\r\n+CEREG: %d,%d,\"%4x\",\"%8x\",%d",
                            &cereg->n, &cereg->state, &cereg->lac, &cereg->ci, &cereg->act);
            }
        }
        cm_demo_printf("cereg->act:%d, cereg->active_time:%d, cereg->cause_type:%d,cereg->ci:%.8x, cereg->lac:%d,cereg->n:%d,cereg->periodic_tau:%d,cereg->rac:%d,cereg->reject_cause:%d,cereg->state:%d\n",
                        cereg->act,    cereg->active_time,    cereg->cause_type,   cereg->ci,    cereg->lac,cereg->n,      cereg->periodic_tau,   cereg->rac,   cereg->reject_cause,   cereg->state);
        cm_free(cereg);

        char cscon_rsp[50] = {0};
        cm_virt_at_send_sync((const uint8_t *)"AT+CSCON?\r\n", cscon_rsp, &rsp_len, 32);
        cm_demo_printf("get_cscon:%s",cscon_rsp+2);

        int pdp_state = cm_modem_get_pdp_state(1);
        cm_demo_printf("pdp_state:%d\n",pdp_state);

        return;

    }
    osPrintf("Usage:\n");
    osPrintf("  modem info                        \n");
    return;
}

#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(modem, cm_test_modem);
#endif
#endif
