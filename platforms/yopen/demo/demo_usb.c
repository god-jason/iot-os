#include "yopen_usb.h"
#include "yopen_os.h"
#include "yopen_type.h"
#include "yopen_uart.h"
#include "yopen_power.h"
#include "yopen_debug.h"
#include "yopen_error.h"

#include <stdio.h>
#include <string.h>

int yopen_usb_notify_cb(YOPEN_USB_HOTPLUG_E state, void *ctx)
{
    yopen_trace("USB change hotplug state(%d)", state);
	return 0;
}

void usb_demo_thread(void *arvg)
{
	bool usbisenabled=FALSE;
	uint32_t port = 0;
	yopen_errcode_usb_e ret;

	yopen_rtos_task_sleep_ms(1000);
	yopen_log_port_get(&port);
	if(port != 1)
	{
		yopen_log_port_set(1);
		yopen_power_reset(RESET_QUICK);
		return;
	}
	ret = yopen_usb_bind_hotplug_cb(yopen_usb_notify_cb);
	yopen_trace("USB yopen_usb_bind_hotplug_cb ret %d", ret);
	
	while(1)
	{
		yopen_trace("USB get hotplug state(%d)", yopen_usb_get_hotplug_state());
		usbisenabled=yopen_usb_isenabled();
		yopen_trace("usbisenabled:%d", usbisenabled);
		yopen_rtos_task_sleep_ms(10000);
		yopen_usb_disable();
		usbisenabled=yopen_usb_isenabled();
		yopen_trace("usbisenabled:%d", usbisenabled);
		yopen_rtos_task_sleep_ms(10000);
		yopen_usb_enable();
	}
}

