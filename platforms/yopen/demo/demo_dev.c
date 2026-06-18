
/**  @file
  demo_dev.c

  @brief
  TODO

*/


#include "yopen_dev.h"
#include "yopen_os.h"
#include "string.h"
#include "yopen_debug.h"


#define DEMO_DEV_WRITE_SN    (0)
#define DEMO_DEV_WRITE_IMEI  (0) 

#define DEMO_DEV_TRACE(fmt, ...) yopen_trace("[DEV]"fmt, ##__VA_ARGS__)

void dev_demo_thread(void *argv)
{
    DEMO_DEV_TRACE("dev demo thread enter");

	yopen_rtos_task_sleep_ms(5000);
    char devinfo[64] = {0};
	yopen_dev_get_imei(devinfo, 64, 0);
	DEMO_DEV_TRACE("IMEI: %s", devinfo);
#if DEMO_DEV_WRITE_IMEI
	devinfo[0] = '8';

	yopen_dev_set_imei(devinfo, 15, 0);
	DEMO_DEV_TRACE("SET IMEI: %s", devinfo);

	memset(devinfo,0,sizeof(devinfo));
	yopen_dev_get_imei(devinfo, 64, 0);
	DEMO_DEV_TRACE("NEW IMEI: %s", devinfo);
#endif
	
    memset(devinfo,0,sizeof(devinfo));
	yopen_dev_get_sn(devinfo, 64, 0);
	DEMO_DEV_TRACE("SN: %s", devinfo);

#if DEMO_DEV_WRITE_SN
	strcpy(devinfo, "TESTSN0001");

	yopen_dev_set_sn(devinfo, strlen(devinfo), 0);
	DEMO_DEV_TRACE("SET SN: %s", devinfo);

	memset(devinfo,0,sizeof(devinfo));
	yopen_dev_get_sn(devinfo, 64, 0);
	DEMO_DEV_TRACE("NEW SN: %s", devinfo);

#endif

    memset(devinfo,0,sizeof(devinfo));
	yopen_dev_get_firmware_version(devinfo, 64);
	DEMO_DEV_TRACE("FW: %s", devinfo);

	uint8_t cfun = 0;
	yopen_dev_get_modem_fun(&cfun, 0);
	DEMO_DEV_TRACE("cfun: %d", cfun);
	yopen_dev_set_modem_fun(YOPEN_DEV_CFUN_MIN, 0, 0);
	yopen_dev_get_modem_fun(&cfun, 0);
	DEMO_DEV_TRACE("cfun: %d", cfun);

	yopen_rtos_task_sleep_ms(10000);
	
	yopen_dev_set_modem_fun(YOPEN_DEV_CFUN_FULL, 0, 0);
	yopen_dev_get_modem_fun(&cfun, 0);
	DEMO_DEV_TRACE("cfun: %d", cfun);
	
	yopen_rtos_task_delete(NULL);

	
}


































