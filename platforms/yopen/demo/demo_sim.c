#include "stdio.h"
#include "string.h"

#include "yopen_debug.h"
#include "yopen_sim.h"
#include "yopen_dev.h"


#define DEMO_SIM_TRACE(fmt, ...) yopen_trace("[SIM] "fmt, ##__VA_ARGS__)


void yopen_sim_demo_task(void * arg)
{
    int ret;
    char data[64];
    int i;
    yopen_sim_status_e card_status;

    yopen_rtos_task_sleep_ms(2000);
	DEMO_SIM_TRACE("========== sim demo start ==========");

    ret = yopen_sim_get_imsi(0, data, sizeof(data));
    DEMO_SIM_TRACE("========== yopen_sim_get_imsi %s ret %d ==========", data, ret);

    ret = yopen_sim_get_iccid(0, data, sizeof(data));
    DEMO_SIM_TRACE("========== yopen_sim_get_iccid %s ret %d ==========", data, ret);

    ret = yopen_sim_get_phonenumber(0, data, sizeof(data));
    DEMO_SIM_TRACE("========== yopen_sim_get_phonenumber %s ret %d ==========", data, ret);

    ret = yopen_sim_get_card_status(0, &card_status);
    DEMO_SIM_TRACE("========== yopen_sim_get_card_status %d ret %d ==========", card_status, ret);

    ret = yopen_sim_get_operate_id();
    DEMO_SIM_TRACE("========== yopen_sim_get_operate_id %d  ==========", ret);
    
    for (i=0; i<2; i++)
    {
        //切卡前先CFUN 0
        yopen_dev_set_modem_fun(YOPEN_DEV_CFUN_MIN, 0, 0);
        ret= yopen_sim_set_operate_id(i);
        DEMO_SIM_TRACE("========== yopen_sim_set_operate_id %d ret %d  ==========", i, ret);
        yopen_dev_set_modem_fun(YOPEN_DEV_CFUN_FULL, 0, 0);

        yopen_rtos_task_sleep_ms(2000);

        ret = yopen_sim_get_imsi(0, data, sizeof(data));
        DEMO_SIM_TRACE("========== yopen_sim_get_imsi (sim%d) %s ret %d ==========", i, data, ret);

        ret = yopen_sim_get_iccid(0, data, sizeof(data));
        DEMO_SIM_TRACE("========== yopen_sim_get_iccid (sim%d) %s ret %d ==========", i, data, ret);

        ret = yopen_sim_get_phonenumber(0, data, sizeof(data));
        DEMO_SIM_TRACE("========== yopen_sim_get_phonenumber (sim%d) %s ret %d ==========", i, data, ret);

        ret = yopen_sim_get_card_status(0, &card_status);
        DEMO_SIM_TRACE("========== yopen_sim_get_card_status (sim%d) %d ret %d ==========", i, card_status, ret);
    }

    //切回卡0
    i = 0;
    yopen_dev_set_modem_fun(YOPEN_DEV_CFUN_MIN, 0, 0);
    ret= yopen_sim_set_operate_id(i);
    DEMO_SIM_TRACE("========== yopen_sim_set_operate_id %d ret %d  ==========", i, ret);
    yopen_dev_set_modem_fun(YOPEN_DEV_CFUN_FULL, 0, 0);

    DEMO_SIM_TRACE("========== sim demo end ==========");

    yopen_rtos_task_delete(NULL);  // kill itself
}
