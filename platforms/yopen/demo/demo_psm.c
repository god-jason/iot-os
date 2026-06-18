
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "yopen_debug.h"
#include "yopen_os.h"
#include "yopen_debug.h"
#include "yopen_power.h"
#include "yopen_ril.h"
#include "yopen_nw.h"
#include "yopen_sim.h"
#include "yopen_usb.h"

#define DEMO_PSM_TRACE(fmt, ...) yopen_trace("[PSM]"fmt, ##__VA_ARGS__)

//设置休眠5分钟，唤醒2秒。注意：
//1、实际休眠时间以网络协商最终结果为准。
//2、如应用需要定期唤醒，可以使用yopen_deepslp_timer_start来定时唤醒应用
#define DEMO_RIL_CMD_PSM_ENABLE "AT+CPSMS=1,,,\"10100101\",\"00000001\"\r\n" 

static int32_t _urc_cb(char *line, uint32_t line_len)
{
    DEMO_PSM_TRACE("========== RIL_URC_IND rsp %s ==========", line);
    return 0;
}

static  int32_t _rsp_cb(char *line, uint32_t line_len, void *user_data)
{
	DEMO_PSM_TRACE("========== line %s, %d, userdata %x ==========", line, line_len, user_data);
    return 0;
}

static void pre_sleep_process(yopen_sleep_depth_e state)
{
	yopen_trace("==========enter sleep success state %d==========", state);
}

static void psm_setup(void)
{
    yopen_nw_reg_status_info_s nw_status;

	uint32_t port = 0;

    yopen_ril_send_atcmd((char*)DEMO_RIL_CMD_PSM_ENABLE, strlen(DEMO_RIL_CMD_PSM_ENABLE), _rsp_cb, 0);

    yopen_log_port_get(&port);
    if(port != 1)
    {
      yopen_log_port_set(1);
      yopen_power_reset(RESET_QUICK);
      return;
    }
    yopen_sim_set_present_det(0, 0);
    yopen_usb_disable();

    // yopen_nw_set_rrc_release_time(1);

    yopen_set_sleep_depth(YOPEN_SLP_SLP2_STATE);

	yopen_sleep_register_cb(pre_sleep_process);

	yopen_trace("==========PSM wait nw ready ==========");
    do
    {
        yopen_nw_get_reg_status(0, &nw_status);

        yopen_rtos_task_sleep_ms(1000);
    } while (!(nw_status.data_reg.state == YOPEN_NW_REG_STATE_HOME_NETWORK || nw_status.data_reg.state == YOPEN_NW_REG_STATE_ROAMING));

	yopen_trace("==========PSM nw ready ==========");

    yopen_autosleep_enable(YOPEN_ALLOW_SLEEP);
}

void yopen_power_psm_demo_thread(void* argv)
{
    psm_setup();

    yopen_ril_init(_urc_cb);


    yopen_ril_send_atcmd((char*)"AT+CPSMS?", strlen("AT+CPSMS?"), _rsp_cb, 0);
    

    yopen_rtos_task_delete(NULL);
}
