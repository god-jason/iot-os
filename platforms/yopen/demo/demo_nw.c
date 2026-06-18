
#include <stdio.h>
#include <string.h>
#include "yopen_os.h"
#include "yopen_type.h"
#include "yopen_debug.h"
#include "yopen_nw.h"

#define DEMO_NW_TRACE(fmt, ...) yopen_trace("[NW]"fmt, ##__VA_ARGS__)

void yopen_nw_ind_callback(uint8_t nSim, unsigned int ind_type, void *ctx)
{
	char csq = 99;
	yopen_nw_common_reg_status_info_s *quec_nw_msg = NULL;
	yopen_nw_nitz_time_info_s *quec_nw_nitz_time_info = NULL;
	DEMO_NW_TRACE("yopen_nw_ind_callback nSim=%d, ind_type=%x", nSim, ind_type);
	switch(ind_type)
	{
		case YOPEN_NW_SIGNAL_QUALITY_IND:
			csq = *((char*)ctx);
			DEMO_NW_TRACE("csq=%d", csq);
			break;
		case YOPEN_NW_DATA_REG_STATUS_IND:
			quec_nw_msg = (yopen_nw_common_reg_status_info_s *)ctx;
			DEMO_NW_TRACE("regState=%d, lac=0x%X, cid=0x%X, act=%d", quec_nw_msg->state, quec_nw_msg->lac, quec_nw_msg->cid, quec_nw_msg->act);
			break;
		case YOPEN_NW_NITZ_TIME_UPDATE_IND:
			quec_nw_nitz_time_info = (yopen_nw_nitz_time_info_s *)ctx;
			DEMO_NW_TRACE("nitz_time=%s, abs_time=%ld", quec_nw_nitz_time_info->nitz_time, quec_nw_nitz_time_info->abs_time);
			break;
	}
	
}

void yopen_nw_demo_thread(void *arvg)
{
	int ret = 0;

	unsigned char csq = 0;
	yopen_nw_reg_status_info_s reg_info = {0};
	yopen_nw_signal_strength_info_s signal_info = {0};
	yopen_nw_operator_info_s oper_info = {0};
	yopen_nw_seclection_info_s select_info = {0};
	yopen_nw_nitz_time_info_s nitz_info = {0};
	yopen_nw_cell_info_s cell_info = {0};

	yopen_nw_phy_status_info_s phy_status_info = {0};

	DEMO_NW_TRACE("========== NW demo start ==========");
	yopen_nw_register_cb(yopen_nw_ind_callback);
	
	uint8_t get_band_num = 0;
	uint8_t get_band[32] = {0};
	uint8_t need_set_band[]={1,3,5}; 
	uint8_t all_support_band[]={1,3,5,8,34,38,39,40,41};
	int 	rsp_len = 0;
	CHAR    rspStr[32] = {0};	

	ret = yopen_nw_set_band(sizeof(need_set_band)/need_set_band[0],need_set_band);
	DEMO_NW_TRACE("ret=0x%x,yopen_nw_set_band(),set need band", ret);
	yopen_rtos_task_sleep_ms(100);

	ret = yopen_nw_get_band(&get_band_num,get_band);
	DEMO_NW_TRACE("ret=0x%x,yopen_nw_get_band(), gets the set band num:%d", ret,get_band_num);
	for(int i = 0 ; i < get_band_num;i++)
	{
		rsp_len += snprintf(rspStr + rsp_len, 32 - rsp_len -1,",%d", get_band[i]);
	}
	rsp_len = 0;
	DEMO_NW_TRACE("gets the set band:%s",rspStr);
	yopen_rtos_task_sleep_ms(100);

	ret = yopen_nw_set_band((sizeof(all_support_band)/all_support_band[0]),all_support_band);
	DEMO_NW_TRACE("ret=0x%x,yopen_nw_set_band(),set all support band,", ret);
	yopen_rtos_task_sleep_ms(100);
	
	memset(rspStr,0,sizeof(rspStr));
	rsp_len = 0;
	ret = yopen_nw_get_band(&get_band_num,get_band);
	DEMO_NW_TRACE("ret=0x%x,yopen_nw_get_band(), get_band_num:%d", ret,get_band_num);
	for(int i = 0 ; i < get_band_num;i++)
	{
		rsp_len += snprintf(rspStr + rsp_len, 32 - rsp_len -1,",%d", get_band[i]);
	}
	DEMO_NW_TRACE("supported by all band:%s",rspStr);

	while(1)
	{	
		
		ret = yopen_nw_get_reg_status(0, &reg_info);
        DEMO_NW_TRACE("yopen_nw_get_reg_status ret=0x%x", ret);
        DEMO_NW_TRACE("data:  state:%d, lac:0x%X, cid:0x%X, act:%d", reg_info.data_reg.state, reg_info.data_reg.lac, reg_info.data_reg.cid, reg_info.data_reg.act);

        ret = yopen_nw_get_csq(0, &csq);
        DEMO_NW_TRACE("yopen_nw_get_csq ret=0x%x, csq:%d", ret, csq);

        ret = yopen_nw_get_nitz_time_info(&nitz_info);
        DEMO_NW_TRACE("yopen_nw_get_nitz_time_info ret=0x%x, nitz_time:%s, abs_time:%ld", ret, nitz_info.nitz_time, nitz_info.abs_time);

		memset(&oper_info, 0, sizeof(oper_info));
		ret = yopen_nw_get_operator_name(0, &oper_info);
		DEMO_NW_TRACE("yopen_nw_get_operator_name ret %d, mcc %s, mnc %s", ret, oper_info.mcc, oper_info.mnc);

		memset(&signal_info, 0, sizeof(signal_info));
		ret = yopen_nw_get_signal_strength(0, &signal_info);
		DEMO_NW_TRACE("yopen_nw_get_signal_strength ret=0x%x, snr:%d, rsrp:%d,rsrq: %d, rssi %d", ret, signal_info.snr, signal_info.rsrp, signal_info.rsrq, signal_info.rssi);

		ret = yopen_nw_get_phy_status_info(0, &phy_status_info);
		DEMO_NW_TRACE("yopen_nw_get_phy_status_info ret=0x%x, band:%d, pci:%d,rsrp:%d rsrq %d, rssi %d, snr %d", ret, 
							phy_status_info.band,phy_status_info.pci,phy_status_info.rsrp,
							phy_status_info.rsrq, phy_status_info.rssi, phy_status_info.snr);

		memset(&cell_info, 0, sizeof(cell_info));
		ret = yopen_nw_get_cell_info(0, &cell_info);
		DEMO_NW_TRACE("yopen_nw_get_cell_info ret=0x%x, lte_info_valid:%d, lte_info_num: %d", ret, cell_info.lte_info_valid, cell_info.lte_info_num);

		if(cell_info.lte_info_valid)
		{
			for (int i=0; i<cell_info.lte_info_num; i++)
			{
				DEMO_NW_TRACE("index %d flag %d band %d tac %x pci %x cid %x, mcc %d, mnc_len %d, mnc %d, rsrp %d, rsrq %d, snr %d", i, cell_info.lte_info[i].flag, 
																cell_info.lte_info[i].band,
																cell_info.lte_info[i].tac,
																cell_info.lte_info[i].pci,
																cell_info.lte_info[i].cid,
																cell_info.lte_info[i].mcc,
																cell_info.lte_info[i].mnc_len,
																cell_info.lte_info[i].mnc,
																cell_info.lte_info[i].rsrp,
																cell_info.lte_info[i].rsrq,
																cell_info.lte_info[i].snr);
			}
		}

		memset(&select_info, 0, sizeof(select_info));
		ret = yopen_nw_get_selection(0, &select_info);
		DEMO_NW_TRACE("yopen_nw_get_selection ret %d, mcc %s, mnc %s", ret, select_info.mcc, select_info.mnc);

		memset(&select_info, 0, sizeof(select_info));
		select_info.act = YOPEN_NW_ACCESS_TECH_E_UTRAN;
		sprintf((char *)&select_info.mcc, "%s", "460");
		sprintf((char *)&select_info.mnc, "%s", "01");
		select_info.nw_selection_mode = 1;

		memset(&select_info, 0, sizeof(select_info));
		ret = yopen_nw_set_selection(0, &select_info);
		DEMO_NW_TRACE("yopen_nw_set_selection 46001 ret %d", ret);

		ret = yopen_nw_get_selection(0, &select_info);
		DEMO_NW_TRACE("yopen_nw_get_selection ret %d, mcc %s, mnc %s", ret, select_info.mcc, select_info.mnc);
		yopen_rtos_task_sleep_ms(5000);
		
	}
}
