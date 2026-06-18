#include "stdio.h"
#include "string.h"
#include "yopen_os.h"
#include "yopen_debug.h"
#include "yopen_datacall.h"
#include "yopen_sockets.h"

#define DEMO_DATACALL_TRACE(fmt, ...) yopen_trace("[DATACALL] "fmt, ##__VA_ARGS__)

#define YOPEN_DATACALL_CID (1)
#define YOPEN_DATACALL_ACT_CID (2)

void yopen_DATACALL_demo_task(void * arg)
{
	DEMO_DATACALL_TRACE("========== DATACALL DEMO START ==========");

    yopen_datacall_errcode_e ret;
    
    DEMO_DATACALL_TRACE("========== yopen_network_register_wait begin ==========");
    ret = yopen_network_register_wait(0, 100);
    DEMO_DATACALL_TRACE("========== yopen_network_register_wait end ret %d ==========", ret);

    yopen_data_call_default_pdn_info_s ctx;
    memset(&ctx, 0, sizeof(ctx));
    ret = yopen_datacall_get_default_pdn_info(0, &ctx);
    DEMO_DATACALL_TRACE("========== yopen_datacall_get_default_pdn_info ret %d ==========", ret);
    DEMO_DATACALL_TRACE("========== yopen_datacall_get_default_pdn_info apn_name %s type %d ==========", ctx.apn_name, ctx.ip_version);
    if (ctx.ip_version == YOPEN_DATA_TYPE_IP)
    {
        DEMO_DATACALL_TRACE("========== yopen_datacall_get_default_pdn_info ret %d IPV4 %s ==========", ret, ip4addr_ntoa((ip4_addr_t*)&ctx.ipv4));
    }
    else if(ctx.ip_version == YOPEN_DATA_TYPE_IPV6)
    {
        DEMO_DATACALL_TRACE("========== yopen_datacall_get_default_pdn_info ret %d IPV6 %s ==========", ret, ip6addr_ntoa((ip6_addr_t*)&ctx.ipv6));
    }
    else if(ctx.ip_version == YOPEN_DATA_TYPE_IPV4V6)
    {
        DEMO_DATACALL_TRACE("========== yopen_datacall_get_default_pdn_info ret %d IPV4 %s ==========", ret, ip4addr_ntoa((ip4_addr_t*)&ctx.ipv4));
        DEMO_DATACALL_TRACE("========== yopen_datacall_get_default_pdn_info ret %d IPV6 %s ==========", ret, ip6addr_ntoa((ip6_addr_t*)&ctx.ipv6));
    }

    DEMO_DATACALL_TRACE("========== yopen_datacall_get_sim_profile_is_active cid1 active %d ==========", yopen_datacall_get_sim_profile_is_active(0,1));
    DEMO_DATACALL_TRACE("========== yopen_datacall_get_sim_profile_is_active cid2 active %d ==========", yopen_datacall_get_sim_profile_is_active(0,2));

    UINT32 natmode;
    ret = yopen_datacall_get_nat(0, &natmode);
    DEMO_DATACALL_TRACE("========== yopen_datacall_get_nat ret %d natmode %d ==========", ret, natmode);

    yopen_data_call_info_t call_info;
    memset(&call_info, 0, sizeof(call_info));
    yopen_get_data_call_info(0, YOPEN_DATACALL_CID, &call_info);
    DEMO_DATACALL_TRACE("========== yopen_get_data_call_info ret %d ==========", ret);
    DEMO_DATACALL_TRACE("========== cid %d, ip type %d, apn %s ==========", call_info.cid, call_info.ip_version, call_info.apn_name);

    if (call_info.ip_version == YOPEN_DATA_TYPE_IP)
    {
        DEMO_DATACALL_TRACE("========== yopen_get_data_call_info ret %d IPV4 %s ==========", ret, ip4addr_ntoa((ip4_addr_t*)&call_info.v4.addr.ip));
    }

    yopen_ip_addr_t dns[4];

    ret = yopen_get_dns_server(YOPEN_DATACALL_CID, dns);
    DEMO_DATACALL_TRACE("========== yopen_get_dns_server count %d ==========", ret);

    for (int i=0; i<ret; i++)
    {
        DEMO_DATACALL_TRACE("========== yopen_get_dns_server index %d type %d ==========", i, dns[i].type);

        if (dns[i].type == YOPEN_DATA_TYPE_IP)
        {
            DEMO_DATACALL_TRACE("========== yopen_get_dns_server ret %d IPV4 %s ==========", ret, ip4addr_ntoa((ip4_addr_t*)&dns[i].addr.v4));
        }
    }
    
    ret = yopen_start_data_call(0, YOPEN_DATACALL_ACT_CID, YOPEN_DATA_TYPE_IPV4V6, "CTNET", "", "", YOPEN_DATA_AUTH_TYPE_NONE);
    DEMO_DATACALL_TRACE("========== yopen_start_data_call ret %d IPV4 %s ==========", ret);

    memset(&call_info, 0, sizeof(call_info));
    yopen_get_data_call_info(0, YOPEN_DATACALL_ACT_CID, &call_info);
    DEMO_DATACALL_TRACE("========== yopen_get_data_call_info YOPEN_DATACALL_ACT_CID ret %d ==========", ret);
    DEMO_DATACALL_TRACE("========== cid %d, ip type %d, apn %s ==========", call_info.cid, call_info.ip_version, call_info.apn_name);

    if (call_info.ip_version == YOPEN_DATA_TYPE_IPV4V6)
    {
        DEMO_DATACALL_TRACE("========== yopen_get_data_call_info IPV4 %s ==========", ip4addr_ntoa((ip4_addr_t*)&call_info.v4.addr.ip));
        DEMO_DATACALL_TRACE("========== yopen_get_data_call_info IPV6 %s ==========", ip6addr_ntoa((ip6_addr_t*)&call_info.v6.addr.ip));
    }
    yopen_rtos_task_sleep_ms(10000);
    DEMO_DATACALL_TRACE("========== yopen_start_data_call YOPEN_DATACALL_ACT_CID active %d ==========", yopen_datacall_get_sim_profile_is_active(0,YOPEN_DATACALL_ACT_CID));
    yopen_stop_data_call(0, YOPEN_DATACALL_ACT_CID);
    DEMO_DATACALL_TRACE("========== yopen_stop_data_call YOPEN_DATACALL_ACT_CID active %d ==========", yopen_datacall_get_sim_profile_is_active(0,YOPEN_DATACALL_ACT_CID));
    
    yopen_rtos_task_delete(NULL);  // kill itself
}

