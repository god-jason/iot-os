#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "yopen_debug.h"
#include "yopen_os.h"
#include "yopen_wifi.h"

#define DEMO_WIFI_TRACE(fmt, ...) yopen_trace("[WIFI]"fmt, ##__VA_ARGS__)

void demo_wifi_scan_all_channel(void)
{
	yopen_wifi_config_s wifi_config;
	yopen_wifi_info_s wifi_info;
	uint32 tick;

	memset(&wifi_config, 0, sizeof(yopen_wifi_config_s));
	memset(&wifi_info, 0, sizeof(yopen_wifi_info_s));

	wifi_config.maxTimeOut = 10000;
	wifi_config.round = 1;
	wifi_config.maxBssidNum = YOPEN_WIFI_BSSID_MAX_NUM;
	wifi_config.scanTimeOut = 3;
	wifi_config.wifiPriority = 1; //0：数据优先 1：WIFI优先
	wifi_config.channelCount = 1;
	wifi_config.channelRecLen = 280;
	wifi_config.channelId[0] = 0;
	
	tick = yopen_rtos_get_system_tick();
	yopen_errcode_wifi_e ret = yopen_wifi_scan(wifi_config, &wifi_info);

	DEMO_WIFI_TRACE("yopen_wifi_scan ret=%x, time=%dms", ret, yopen_rtos_get_system_tick() - tick);
	
	if (ret == YOPEN_WIFI_SUCCESS)
	{
		DEMO_WIFI_TRACE("scan wifi success, bssidNum=%d", wifi_info.bssidNum);
		for (uint32 i = 0; i < wifi_info.bssidNum; i++)
		{
			DEMO_WIFI_TRACE("index %d bssid[%d]=%02x:%02x:%02x:%02x:%02x:%02x", 
				i, wifi_info.bssid[i][0], wifi_info.bssid[i][1], wifi_info.bssid[i][2], wifi_info.bssid[i][3], wifi_info.bssid[i][4], wifi_info.bssid[i][5]);
			DEMO_WIFI_TRACE("rssi=%d, channel %d, ssid_len %d", wifi_info.rssi[i], wifi_info.channel[i], wifi_info.ssidHexLen[i]);
			DEMO_WIFI_TRACE("ssid %s", wifi_info.ssidHex[i]);
				
		}
	}
}

void demo_wifi_scan(char channelId[YOPEN_WIFI_CHANNEL_MAX_NUM], int channel_count)
{
	yopen_wifi_config_s wifi_config;
	yopen_wifi_info_s wifi_info;
	uint32 tick;

	memset(&wifi_config, 0, sizeof(yopen_wifi_config_s));
	memset(&wifi_info, 0, sizeof(yopen_wifi_info_s));

	wifi_config.maxTimeOut = 10000;
	wifi_config.round = 1;
	wifi_config.maxBssidNum = YOPEN_WIFI_BSSID_MAX_NUM;
	wifi_config.scanTimeOut = 3;
	wifi_config.wifiPriority = 1; //0：数据优先 1：WIFI优先
	wifi_config.channelCount = channel_count;
	wifi_config.channelRecLen = 280;
	
	for (int i = 0; i < channel_count; i++)
	{
		DEMO_WIFI_TRACE("channelId[%d]=%d", i, channelId[i]);
		wifi_config.channelId[i] = channelId[i];
	}

	tick = yopen_rtos_get_system_tick();
	yopen_errcode_wifi_e ret = yopen_wifi_scan(wifi_config, &wifi_info);

	DEMO_WIFI_TRACE("yopen_wifi_scan ret=%x, time=%dms", ret, yopen_rtos_get_system_tick() - tick);
	
	if (ret == YOPEN_WIFI_SUCCESS)
	{
		DEMO_WIFI_TRACE("scan wifi success, bssidNum=%d", wifi_info.bssidNum);
		for (uint32 i = 0; i < wifi_info.bssidNum; i++)
		{
			DEMO_WIFI_TRACE("index %d bssid[%d]=%02x:%02x:%02x:%02x:%02x:%02x", 
				i, wifi_info.bssid[i][0], wifi_info.bssid[i][1], wifi_info.bssid[i][2], wifi_info.bssid[i][3], wifi_info.bssid[i][4], wifi_info.bssid[i][5]);
			DEMO_WIFI_TRACE("rssi=%d, channel %d, ssid_len %d", wifi_info.rssi[i], wifi_info.channel[i], wifi_info.ssidHexLen[i]);
			DEMO_WIFI_TRACE("ssid %s", wifi_info.ssidHex[i]);
				
		}
	}
}

void wifi_demo_thread(void* argv)
{
	char channelId[YOPEN_WIFI_CHANNEL_MAX_NUM];
	int channel_count;
	yopen_rtos_task_sleep_ms(5000);
    while (1)
    {
		//扫描所有通道
		DEMO_WIFI_TRACE("wifi scan all");
		demo_wifi_scan_all_channel();

		//扫描通道1
		DEMO_WIFI_TRACE("wifi scan 1");
		memset(channelId, 0, YOPEN_WIFI_CHANNEL_MAX_NUM);
		channelId[0] = 1;
		channel_count = 1;
		demo_wifi_scan(channelId, channel_count);

		//扫描通道11
		DEMO_WIFI_TRACE("wifi scan 11");
		memset(channelId, 0, YOPEN_WIFI_CHANNEL_MAX_NUM);
		channelId[0] = 11;
		channel_count = 1;
		demo_wifi_scan(channelId, channel_count);

		//扫描通道1和11
		DEMO_WIFI_TRACE("wifi scan 1 and 11");
		memset(channelId, 0, YOPEN_WIFI_CHANNEL_MAX_NUM);
		channelId[0] = 1;
		channelId[1] = 11;
		channel_count = 2;
		demo_wifi_scan(channelId, channel_count);
		
        yopen_rtos_task_sleep_ms(3000);
    }
}
