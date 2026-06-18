#include "yopen_usbnet.h"
#include "yopen_usb.h"
#include "yopen_os.h"
#include "yopen_nw.h"
#include "yopen_type.h"

#include "yopen_power.h"
#include "yopen_debug.h"

#include <stdio.h>
#include <string.h>


void usbnet_demo_thread(void *arvg)
{
	yopen_usbnet_errcode_e ret = 0;
	yopen_usbnet_type_e saved_type = 0;
	yopen_usbnet_type_e target_type = YOPEN_USBNET_ECM;
	uint8_t reset_flag = 0;

	yopen_rtos_task_sleep_ms(1000);


	ret = yopen_usbnet_get_type(&saved_type);
	yopen_trace("get usbnet saved type ret: 0x%x, %d", ret, saved_type);		
	if(0 != ret)
	{
		goto exit;
	}

	if(saved_type != target_type)
	{
		ret = yopen_usbnet_set_type(target_type);
		yopen_trace("yopen_usbnet_set_type err, ret=0x%x", ret);
		if(0 != ret)
		{
			goto exit;
		}
		reset_flag = 1;		//usbnet type reboot to take effect
	}

	if(reset_flag==1)
	{
		yopen_power_reset(RESET_NORMAL);
	}
exit:
    yopen_rtos_task_delete(NULL);	
}

