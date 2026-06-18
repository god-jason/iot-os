
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

void sockets_demo_thread(void * arg)
{

#define YOPEN_SOCKET_DEMO_DOMAIN_NAME_SYSTEM	1

#define YOPEN_SOCKET_DEMO_IPV4_TEST 1

#if YOPEN_SOCKET_DEMO_DOMAIN_NAME_SYSTEM
#define YOPEN_SOCKET_DEMO_URL "test.yuge-info.com"
#else
#define YOPEN_SOCKET_DEMO_URL "122.51.209.56"
#endif

	char send_buf[128]={0};
	int  send_len = 0;
	char recv_buf[128]={0};
	int  recv_len = 0;

	int ret = 0;
	int i = 0;
	int cid = 1;
    // yopen_data_call_info_t socket_info = {0};
	fd_set read_fds;
	fd_set write_fds;
	fd_set exp_fds;
	int socket_fd = -1;
	int flags = 0;
	int fd_changed = 0;
	int connected = 0;
	int closing = false;
#if YOPEN_SOCKET_DEMO_IPV4_TEST
	struct sockaddr_in local4, server_ipv4;
#else
	struct sockaddr_in6 local6, server_ipv6;
#endif
	struct addrinfo *pres = NULL;
	struct addrinfo *temp = NULL;
	uint8_t nSim = 0;
	uint16_t test_count = 10;
	yopen_nw_reg_status_info_s nw_status;


	yopen_rtos_task_sleep_ms(2000);
	yopen_trace("==========socket demo start==========");
	yopen_trace("==========wait for network register done==========");


	do{
		yopen_nw_get_reg_status(nSim, &nw_status);

		yopen_rtos_task_sleep_ms(1000);

		yopen_trace("==========nw_status %d==========", nw_status.data_reg.state);

		if(nw_status.data_reg.state == YOPEN_NW_REG_STATE_DENIED)
		{
			yopen_trace("network reg denied exit !!!!");
			goto exit;
		}
		
	}while(!(nw_status.data_reg.state == YOPEN_NW_REG_STATE_HOME_NETWORK || nw_status.data_reg.state == YOPEN_NW_REG_STATE_ROAMING));

	

	// while((ret = yopen_network_register_wait(nSim, 120)) != 0 && i < 10){
    // 	i++;
	// 	yopen_rtos_task_sleep_s(1);
	// }
	// if(ret == 0){
	// 	i = 0;
	// 	yopen_trace("====network registered!!!!====");
	// }else{
	// 	yopen_trace("====network register failure!!!!!====");
	// 	goto exit;
	// }

	memset(&local4, 0x00, sizeof(struct sockaddr_in));
	local4.sin_family = AF_INET;
	local4.sin_port = 1000;

	// yopen_set_data_call_asyn_mode(nSim, cid, 0);
	
	// yopen_trace("===start data call====");
	// yopen_start_data_call(nSim, cid, YOPEN_DATA_TYPE_IPV4V6, "APNTEST", "", "", YOPEN_DATA_AUTH_TYPE_NONE);

	// yopen_rtos_task_sleep_ms(4000);
	// // ret = yopen_get_data_call_info(nSim, cid, &socket_info);
	// // if(ret != 0){
	// // 	yopen_trace("yopen_get_data_call_info ret: %d", ret);
	// // }
	// // yopen_trace("info->cid: %d", socket_info.cid);
	// // yopen_trace("info->ip_version: %d", socket_info.ip_version);
	
// #if YOPEN_SOCKET_DEMO_IPV4_TEST


// 	yopen_ip4addr_aton(yopen_ip4addr_ntoa(&socket_info.v4.addr.ip), (yopen_ip4_addr_t *)&local4.sin_addr);
// 	yopen_trace("info->v4.state: %d", socket_info.v4.state);
// 	yopen_trace("info.v4.addr.ip: %s\r\n", yopen_ip4addr_ntoa(&socket_info.v4.addr.ip));
// 	yopen_trace("info.v4.addr.pri_dns: %s\r\n", yopen_ip4addr_ntoa(&socket_info.v4.addr.pri_dns));
// 	yopen_trace("info.v4.addr.sec_dns: %s\r\n", yopen_ip4addr_ntoa(&socket_info.v4.addr.sec_dns));
// #else
// 	memset(&local6, 0x00, sizeof(struct sockaddr_in6));
// 	local6.sin6_family = AF_INET6;
// 	local6.sin6_port = 0;

// 	yopen_ip6addr_aton(yopen_ip6addr_ntoa(&socket_info.v6.addr.ip), (yopen_ip6_addr_t *)&local6.sin6_addr);
// 	yopen_trace("info->v6.state: %d", socket_info.v6.state);
// 	yopen_trace("info.v6.addr.ip: %s\r\n", yopen_ip6addr_ntoa(&socket_info.v6.addr.ip));
// 	yopen_trace("info.v6.addr.pri_dns: %s\r\n", yopen_ip6addr_ntoa(&socket_info.v6.addr.pri_dns));
// 	yopen_trace("info.v6.addr.sec_dns: %s\r\n", yopen_ip6addr_ntoa(&socket_info.v6.addr.sec_dns));	
// #endif

loop:
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&exp_fds);
	flags = 0;
	connected = 0;
	closing = false;
	i = 0;
	yopen_trace("===socket demo %d====", __LINE__);

#if YOPEN_SOCKET_DEMO_IPV4_TEST
	ret = lwip_getaddrinfowithcid(YOPEN_SOCKET_DEMO_URL, NULL, NULL, &pres, cid);
#else
	ret = lwip_getaddrinfowithcid("2408:4004:E0:A401:814B:FD5E:430E:F38", NULL, NULL, &pres, cid);
#endif

	if (ret < 0 || pres == NULL)
	{
		yopen_trace("DNS getaddrinfo failed! ret=%d; pres=%p!\n",ret,pres);
		goto exit;
	}

	for(temp = pres; temp != NULL; temp = temp->ai_next){
#if YOPEN_SOCKET_DEMO_IPV4_TEST
		struct sockaddr_in * sin_res = (struct sockaddr_in *)temp->ai_addr;
		socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
#else	
		struct sockaddr_in6 * sin6_res = (struct sockaddr_in6 *)temp->ai_addr;
		socket_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6);
#endif
		if(socket_fd < 0){
			yopen_trace("socket failed!");
			continue;
		}

#if YOPEN_SOCKET_DEMO_IPV4_TEST
		ret = bind(socket_fd,(struct sockaddr *)&local4,sizeof(struct sockaddr));
#else
		ret = bind(socket_fd,(struct sockaddr *)&local6,sizeof(struct sockaddr));
#endif
		if(ret < 0){
			yopen_trace("bind failed!");
			close(socket_fd);
			socket_fd = -1;
			continue;
		}

		flags |= O_NONBLOCK;
		fcntl(socket_fd, F_SETFL,flags);

#if YOPEN_SOCKET_DEMO_IPV4_TEST
		memset(&server_ipv4, 0x00, sizeof(struct sockaddr_in));
		server_ipv4.sin_family = temp->ai_family;
		server_ipv4.sin_port = htons(8001);
		server_ipv4.sin_addr = sin_res->sin_addr;

		ret = connect(socket_fd, (struct sockaddr *)&server_ipv4, sizeof(server_ipv4));
#else
		memset(&server_ipv6, 0x00, sizeof(struct sockaddr_in6));
		server_ipv6.sin6_family = temp->ai_family;
		server_ipv6.sin6_port = htons(8001);
		server_ipv6.sin6_addr = sin6_res->sin6_addr;

		ret = connect(socket_fd, (struct sockaddr *)&server_ipv6, sizeof(server_ipv6));
#endif
		yopen_trace("===socket demo %d====ret: %d", __LINE__,ret);
		if(ret == 0){
			connected = 1;
			break;
		}else{
			yopen_trace("===socket demo %d====", __LINE__);
			// goto exit;
			break;
		}
	}
	if(socket_fd < 0){
		yopen_trace("===socket demo %d====", __LINE__);
		goto exit;
	}
	yopen_trace("===socket demo %d====", __LINE__);
	if(connected == 1){
		FD_SET(socket_fd, &read_fds);
#if YOPEN_SOCKET_DEMO_IPV4_TEST
		yopen_trace("=====connect to \"122.51.209.56:8001\" success=====");
#else
		yopen_trace("=====connect to \"2408:4004:E0:A401:814B:FD5E:430E:F38\" success=====");
#endif
		memset(send_buf, 0x00, 128);
		send_len = snprintf(send_buf, 128,"%d%s%d\r\n",i,"startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAend",i);

		if(write(socket_fd, send_buf, send_len) != send_len){
			FD_SET(socket_fd, &write_fds);
		}else{
			i++;
		}
	}else{
		FD_SET(socket_fd, &write_fds);
	}
	FD_SET(socket_fd, &exp_fds);

	while(test_count-- > 0){
		yopen_trace("===socket demo %d====", __LINE__);
		fd_changed = select(socket_fd+1, &read_fds, &write_fds, &exp_fds, NULL);
		yopen_trace("===socket demo %d====", __LINE__);
		if(fd_changed > 0){
			if(FD_ISSET(socket_fd, &write_fds)){
				FD_CLR(socket_fd, &write_fds);
				if(connected== 0){
					int value = 0;
					socklen_t len = 0;

					len = sizeof(value);
					getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &value, &len);

					yopen_trace("errno: %d", value);
					if(value == 0 || value == EISCONN ){
#if YOPEN_SOCKET_DEMO_IPV4_TEST
						yopen_trace("=====connect to \"122.51.209.56:8001\" success=====");
#else
						yopen_trace("=====connect to \"2408:4004:E0:A401:814B:FD5E:430E:F38\" success=====");
#endif
						connected = 1;
						FD_SET(socket_fd, &read_fds);

						memset(send_buf, 0x00, 128);
						send_len = snprintf(send_buf, 128,"%d%s%d\r\n",i,"startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAend",i);
						write(socket_fd, send_buf, send_len);
						i++;
					}else{
#if YOPEN_SOCKET_DEMO_IPV4_TEST
						yopen_trace("=====connect to \"122.51.209.56:8001\" failed=====");
#else
						yopen_trace("=====connect to \"2408:4004:E0:A401:814B:FD5E:430E:F38\" failed=====");
#endif
						close(socket_fd);
						socket_fd = -1;
						break;
					}
				}else{
					memset(send_buf, 0x00, 128);
					send_len = snprintf(send_buf, 128,"%d%s%d\r\n",i,"startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAend",i);
					write(socket_fd, send_buf, send_len);
					i++;
				}
			}else if(FD_ISSET(socket_fd, &read_fds)){
				FD_CLR(socket_fd, &read_fds);
			    memset(recv_buf,0x00, 128);
				recv_len = read(socket_fd, recv_buf, 128);
				if(recv_len > 0){
					yopen_trace(">>>>Recv: %s", recv_buf);
					memset(send_buf, 0x00, 128);
					send_len = snprintf(send_buf, 128,"%d%s%d\r\n",i,"startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAend",i);
					write(socket_fd, send_buf, send_len);
					i++;
					FD_SET(socket_fd, &read_fds);
					// if(yopen_nw_get_data_count(nSim, &data_count_info) == 0)
					// {
					// 	yopen_trace("data count: uplink %llu, downlink %llu", data_count_info.uplink_data_count, data_count_info.downlink_data_count);
					// }
				}else if(recv_len == 0){
					if(closing == false){
						yopen_trace("===passive close!!!!");
						shutdown(socket_fd, SHUT_WR);
						closing = true;
						FD_SET(socket_fd, &read_fds);
					}else{
						close(socket_fd);
						socket_fd = -1;
						break;
					}
				}else{
					// if(lwip_get_error(socket_fd) == EAGAIN){
					// 	FD_SET(socket_fd, &read_fds);
					// }else{
					// 	close(socket_fd);
					// 	socket_fd = -1;
					// 	break;
					// }
				}
			}else if(FD_ISSET(socket_fd, &exp_fds)){
				FD_CLR(socket_fd, &exp_fds);
				close(socket_fd);
				socket_fd = -1;
				break;
			}

		}
	}
    if(pres!=NULL)
    {
        freeaddrinfo(pres);
        pres = NULL;
    }
	goto loop;
exit:
	// yopen_stop_data_call(nSim, cid);
	yopen_rtos_task_delete(NULL);

   return;
}



