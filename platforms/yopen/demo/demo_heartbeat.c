
/**  @file
  demo_socket.c

  @brief
  TODO

*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "yopen_debug.h"
#include "yopen_os.h"
#include "yopen_sockets.h"
#include "yopen_datacall.h"
#include "yopen_nw.h"
#include "yopen_power.h"
#include "yopen_debug.h"
#include "yopen_sim.h"
#include "yopen_usb.h"
// #include "cmips.h"

#define TEST_SOCKET_SERVER "test.yuge-info.com"
#define TEST_SOCKET_TCP_PORT 8001
#define TEST_SOCKET_DATA "tcp_data %d"

#define HEARTBEAT_ERROR(c, v) if (!(c)){yopen_trace("==========heartbeat socket error line %d val %d", __LINE__, c); break;}

static void pre_sleep_process(yopen_sleep_depth_e state)
{
	yopen_trace("==========enter sleep success state %d==========", state);
}

static void post_sleep_process(yopen_sleep_depth_e state)
{
	yopen_trace("==========exit sleep src %d, state %d==========", yopen_get_wakeup_src(), state);

}

static void heartbeat_setup(void)
{
    yopen_nw_reg_status_info_s nw_status;

	uint32_t port = 0;

    yopen_log_port_get(&port);
    if(port != 1)
    {
      yopen_log_port_set(1);
      yopen_power_reset(RESET_QUICK);
      return;
    }
    yopen_sim_set_present_det(0, 0);
    yopen_usb_disable();

    yopen_set_sleep_depth(YOPEN_SLP_SLP1_STATE);

	yopen_sleep_register_cb(pre_sleep_process);
	yopen_wakeup_register_cb(post_sleep_process);

	yopen_trace("==========heartbeat_setup wait nw ready ==========");
    do
    {
        yopen_nw_get_reg_status(0, &nw_status);

        yopen_rtos_task_sleep_ms(1000);
    } while (!(nw_status.data_reg.state == YOPEN_NW_REG_STATE_HOME_NETWORK || nw_status.data_reg.state == YOPEN_NW_REG_STATE_ROAMING));

	yopen_trace("==========heartbeat_setup nw ready ==========");

    yopen_autosleep_enable(YOPEN_ALLOW_SLEEP);
}

void heartbeat_demo_thread(void * arg)
{
    int ret;
    char data[64];
    uint16_t data_len;
    int i = 0, flags = 0;
    struct hostent *hostent;
    int socket_fd;

    struct sockaddr_in local4;

	heartbeat_setup();

    hostent = gethostbyname(TEST_SOCKET_SERVER);

    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    memset(&local4, 0x00, sizeof(struct sockaddr_in));
    local4.sin_family = AF_INET;
    local4.sin_port = 1000;


	do
	{
		ret = bind(socket_fd,(struct sockaddr *)&local4,sizeof(struct sockaddr));
		HEARTBEAT_ERROR(ret == 0, ret);

		struct sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(TEST_SOCKET_TCP_PORT);
		serv_addr.sin_addr = *((struct in_addr *)hostent->h_addr);

		ret = connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		HEARTBEAT_ERROR(ret == 0, ret);

		flags |= O_NONBLOCK;
		ret = fcntl(socket_fd, F_SETFL,flags);
		HEARTBEAT_ERROR(ret != -1, ret);


		while (1)
		{
			memset(data, 0, sizeof(data));
			sprintf(data, TEST_SOCKET_DATA, i);
			data_len = strlen(data);

			ret = send(socket_fd, data, data_len, 0);

			yopen_trace("==========heartbeat send data %s", data);

			HEARTBEAT_ERROR(ret == data_len, ret);

			yopen_rtos_task_sleep_ms(300*1000);
		}
		ret = close(socket_fd);
		HEARTBEAT_ERROR(ret == 0, ret);
	}while (0);

	yopen_trace("==========heartbeat socket error end ==========");

    yopen_rtos_task_delete(NULL);

}



