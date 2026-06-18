/*********************************************************
 *  @file    cm_demo_gnss.c
 *  @brief   OpenCPU GNSS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created by ZhangQiong 2025/4/14
 ********************************************************/
#ifdef CM_DEMO_GNSS_SUPPORT

#include <stdio.h>
#include <string.h>
#include "cm_demo_common.h"
#include "cm_gnss.h"
#include "cm_mem.h"

#define cm_demo_printf osPrintf


/***********************************************原始模组部demo*************************************************/
void cm_test_gnss_enable(unsigned char **cmd,int len)
{
    if( (cmd == NULL) || (len != 5))
    {
        cm_demo_printf(" cmd = NULL or cmd len = %d error!\n", len);
        return;
    }

    int enable_param = atoi((char *)cmd[2]);
    int gnss_type = atoi((char *)cmd[3]);
    int agnss_enable = atoi((char *)cmd[4]);
    cm_demo_printf("enable_param(%d),gnss_type(%d),agnss_enable(%d)\n", enable_param, gnss_type, agnss_enable);

    if(enable_param == 1)
    {
        cm_gnss_open(gnss_type, agnss_enable);
    }
    else if(enable_param == 0)
    {
        cm_gnss_close();
    }
    else
    {
        cm_demo_printf("enable_param=%d error", enable_param);
    }
}


void cm_test_gnss_getlocateinfo(unsigned char **cmd,int len)
{
    if( (cmd == NULL) || (len != 2))
    {
        cm_demo_printf(" cmd = NULL or cmd len = %d error!\n", len);
        return;
    }

    cm_gnss_location_info_t *location_info = (cm_gnss_location_info_t *)cm_malloc(sizeof(cm_gnss_location_info_t));
    if(location_info == NULL)
    {
        cm_demo_printf("malloc fail!\n");
        return;
    }
    memset(location_info,0,sizeof(cm_gnss_location_info_t));

    if(0 == cm_gnss_get_location_info(location_info))
    {
        cm_demo_printf("latitude=%f\n",location_info->latitude);
        cm_demo_printf("longitude=%f\n",location_info->longitude);
        cm_demo_printf("hdop=%f\n",location_info->hdop);
        cm_demo_printf("altitude=%f\n",location_info->altitude);
        cm_demo_printf("cog=%f\n",location_info->cog);
        cm_demo_printf("spkm=%f\n",location_info->spkm);
        cm_demo_printf("spkn=%f\n",location_info->spkn);
        cm_demo_printf("nsat=%d\n",location_info->nsat);
        cm_demo_printf("dtype=%d\n",location_info->dtype);
        cm_demo_printf("fix=%u\n",location_info->fix);
    }
    else
    {
        cm_demo_printf("GNSS NO SINGAL\n");
    }
    cm_free(location_info);
}

void cm_gnss_nmea_cb(uint32_t type, const char *nmea, uint32_t len)
{
    cm_demo_printf("cm_test_gnss_getrawdata \n type = 0x%08x %d\r\n",type, type);
    cm_demo_printf("cm_test_gnss_getrawdata:%s,%d\r\n",nmea, len);
}

void cm_test_gnss_getrawdata(unsigned char **cmd,int len)
{
    if( (cmd == NULL) || (len != 3))
    {
        cm_demo_printf(" cmd = NULL or cmd len = %d error!\n", len);
        return;
    }

    uint32_t nmea_type = CM_GNSS_NAME_GGA|CM_GNSS_NAME_GSV|CM_GNSS_NAME_GSA|CM_GNSS_NAME_RMC|CM_GNSS_NAME_VTG|CM_GNSS_NAME_GLL;
    int cmd_option = atoi((char *)cmd[2]);
    cm_demo_printf("nmea_type:0x%04x\n",nmea_type);
    int32_t ret = 0;
    if(cmd_option == 1)
    {
        ret = cm_gnss_set_nmea_callback(cm_gnss_nmea_cb, nmea_type);
        cm_demo_printf("ret(%d)\n",ret);
        ret = cm_gnss_req_nmea();
        cm_demo_printf("ret(%d)\n",ret);
    }
    else if(cmd_option == 2)
    {
        ret = cm_gnss_set_nmea_callback(cm_gnss_nmea_cb, nmea_type);
        cm_demo_printf("ret(%d)\n",ret);
    }
}

static int cm_gnss_protocol_depark ( uint8_t* buffer,uint32_t head,uint32_t tail )
{
    int length = tail-head+1;
    uint16_t load_length = ( uint16_t ) ( buffer[head+5]<<8 ) | ( buffer[head+4] );
    uint8_t* binary_data = ( uint8_t* ) cm_malloc ( ( length+1 ) *sizeof ( uint8_t ) );
    if ( NULL == binary_data )
    {
        cm_demo_printf( "malloc binary data fail!" );
        return -1;
    }

#ifdef CM_GNSS_DRIVES_DEBUGEN
    char debug_buffer[32] = {0};
    int leng_in = 0;
    int j=0;
    for(j=0;j<length;j++)
    {
        leng_in += sprintf(debug_buffer+leng_in,"%02x",buffer[head+j]);
    }
    cm_demo_printf ( "binary recv:%s", debug_buffer);
#endif /*CM_GNSS_DRIVES_DEBUGEN*/

    memcpy ( binary_data,buffer+head+6,load_length );
    cm_demo_printf ( "binary recv:%s\n", binary_data);
    cm_free ( binary_data );
    return 0;
}

static void cm_gnss_receive_depark( uint8_t* recv_buffer,uint32_t length )
{
    cm_demo_printf("cm_gnss_receive_depark, recv_buffer(%s), length(%u)\n", recv_buffer, length);

    uint32_t i =0;
    uint32_t head = 0;
    uint32_t load_length = 0;
    uint32_t binary_tail = 0;

    for ( i=0; i<length; i++ )
    {
        if ( recv_buffer[i] == '$' )
        {
            head = i;
            cm_demo_printf("nmea_data:");
            while ( i <= length && recv_buffer[i] != '\n' )
            {
                cm_demo_printf("%c",recv_buffer[i]);
                i++ ;
            }

            if ( '\n' == recv_buffer[i] )
            {
                cm_demo_printf("%c",recv_buffer[i]);
            }
            else
            {
                i = head;
                continue ;
            }
        }

        if ( i < length-7 && recv_buffer[i] == 0xF1 && recv_buffer[i+1] == 0XD9 )
        {
            load_length = ( uint16_t ) ( recv_buffer[i+5]<<8 ) | ( recv_buffer[i+4] );
            binary_tail = i+load_length+7;
            if ( binary_tail > length ) //避免异常越界
            {
                cm_demo_printf ( "binary_tail:%d length:%d",binary_tail,length );
                continue ;
            }
            cm_gnss_protocol_depark ( recv_buffer,i,binary_tail );
        }
    }
}

void cm_gnss_sendrawdata_cb(const char *data, uint32_t len)
{
    cm_gnss_receive_depark(data,len);
}

void cm_test_gnss_sendrawdata(unsigned char **cmd,int len)
{
    if( (cmd == NULL) || (len != 3))
    {
        cm_demo_printf(" cmd = NULL or cmd len = %d error!\n", len);
        return;
    }

    int32_t ret = 0;
    char senddata[64]= {0};
    uint8_t senddata_len = atoi((char *)cmd[2]);
    if(senddata_len==0)
    {
        ret = cm_gnss_send_raw_data(NULL, 0, NULL);
    }
    else
    {
        //uint8_t cmd_version[]= {0xF1,0xD9,0x0A,0x04,0x00,0x00,0x0E,0x34};
        uint8_t cmd_cold_start[]= {0xF1,0xD9,0x06,0x40,0x01,0x00,0x01,0x48,0x22};
        ret = cm_gnss_send_raw_data(cmd_cold_start, 9, cm_gnss_sendrawdata_cb);
    }
    cm_demo_printf("ret=%d\n",ret);
}

void cm_test_gnss_config(unsigned char **cmd,int len)
{
    if((cmd == NULL) || (len != 4))
    {
        cm_demo_printf(" cmd = NULL or cmd len = %d error!\n", len);
        return;
    }

    char *value = NULL;
    int config_type = atoi((char *)cmd[2]);
    if(config_type == CM_GNSS_CONFIG_TYPE_AGNSS_LOCATION)
    {
//        cm_agnss_location_t location_value = {29.6202,106.4948,2};
        cm_agnss_location_t location_value = {32.121, 118.706, 2}; /* 可根据当地位置经纬度配置 */
        cm_demo_printf("location_value.latitude:%lf,location_value.longitude=%lf\r\n",location_value.latitude,location_value.longitude);
        cm_demo_printf("cm_gnss_config ret=:%d\r\n",cm_gnss_config(config_type, &location_value));
    }
    else
    {
        value = (char *)cmd[3];
        cm_demo_printf("cm_gnss_config ret=:%d\r\n",cm_gnss_config(config_type, value));
    }
}


void cm_agnss_update_callback(cm_agnss_update_result_e mode, const char *update_time, uint32_t size)
{
    cm_demo_printf("mode=%d\r\n",mode);
    cm_demo_printf("update_time=%s\r\n",update_time);
    cm_demo_printf("size=%d\r\n",size);
}

void cm_test_agnss_update(unsigned char **cmd,int len)
{
    if( (cmd == NULL) || (len != 3))
    {
        cm_demo_printf(" cmd = NULL or cmd len = %d error!\n", len);
        return;
    }

    int agnss_cmd = atoi((char *)cmd[2]);
    if(agnss_cmd == 1)
    {
        cm_agnss_data_start_update(cm_agnss_update_callback);
    }
    else if(agnss_cmd == 2)
    {
        cm_agnss_update_result_e agnss_mode = 0;
        char update_time[24] = {0};
        uint32_t data_size = 0;
        //cm_agnss_state_query(&agnss_mode, update_time, sizeof(update_time),&data_size);
        cm_agnss_state_query(&agnss_mode, update_time, &data_size);
        cm_demo_printf("agnss_mode=%d\r\n",agnss_mode);
        cm_demo_printf("update_time=%s\r\n",update_time);
        cm_demo_printf("data_size=%u\r\n",data_size);
    }
}

void cm_test_gnss_reset(unsigned char **cmd,int len)
{
    if( (cmd == NULL) || (len != 3))
    {
        cm_demo_printf(" cmd = NULL or cmd len = %d error!\n", len);
        return;
    }

    cm_gnss_reset_mode_e restart_mode = atoi((char *)cmd[2]);
    int ret = cm_gnss_reset(restart_mode);
    if(ret < 0)
    {
        cm_demo_printf("gnss reset mode=%d fail", restart_mode);
        return;
    }
}
/***********************************************原始模组部demo*************************************************/
void cm_test_gnss(char argc, char **argv)
{
    unsigned char operation[20] = {0};
    sprintf((char *)operation, "%s", argv[1]);
    cm_demo_printf("operation=%s\r\n", operation);
    if (0 == strcmp((const char *)operation, "gnss_enable"))
    {
        cm_test_gnss_enable(argv,argc);
    }
    else if (0 == strcmp((const char *)operation, "getlocateinfo"))
    {
        cm_test_gnss_getlocateinfo(argv,argc);
    }
    else if (0 == strcmp((const char *)operation, "getrawdata"))
    {
//        cm_test_gnss_getnmea(argv,0);
        cm_test_gnss_getrawdata(argv,argc);
    }
    else if (0 == strcmp((const char *)operation, "sendrawdata"))
    {
        cm_test_gnss_sendrawdata(argv,argc);
    }
    else if (0 == strcmp((const char *)operation, "config"))
    {
        cm_test_gnss_config(argv,argc);
    }
    else if (0 == strcmp((const char *)operation, "agnss_update"))
    {
        cm_test_agnss_update(argv,argc);
    }
    else if (0 == strcmp((const char *)operation, "reset"))
    {
        cm_test_gnss_reset(argv,argc);
    }
    else
    {
        cm_demo_printf("arg error!\n");
    }
    return;
}

#if defined(OS_USING_SHELL)
#include <nr_micro_shell.h>

static void SHELL_gnss(char argc, char **argv)
{
    shell_printf("argc=%d\r\n", argc);
    if (0 == strcmp(argv[1], "help"))
    {
        shell_printf("-----------------cm_gnss help---------------------\r\n");
        shell_printf("1 cm_gnss gnss_enable arg1 arg2 argn3\r\n [arg1:gnss enable(1:open 0:close)][arg2:type(cm_gnss_type_e)][arg3:gnss enable(1:close 2:open)]\r\n");
        shell_printf("2 cm_gnss getlocateinfo\r\n");
        shell_printf("3 cm_gnss getrawdata arg1\r\n [arg1:type]\r\n");
        shell_printf("4 cm_gnss sendrawdata arg1\r\n [arg1:cmd]\r\n");
        shell_printf("5 cm_gnss config arg1 arg2\r\n [arg1:config_type(cm_gnss_config_type_e) arg2:value(mask/cycle/location/gnss_ctrl)]\r\n");
        shell_printf("6 cm_gnss reset arg1\r\n [arg1:rst_mode(1:restart cold)]\r\n"); // 1：只支持冷启动
        shell_printf("7 cm_gnss agnss_update arg1\r\n [arg1:(1:agnss download 2:agnss query)]\r\n");
        shell_printf("-----------------cmcm_gnss_gnss--------------------------\r\n");
    }
    else
    {
        shell_printf("shell cm_gnss argv %s %s argc: %d\r\n", argv[0], argv[1], argc);
        cm_test_gnss(argc, argv);
    }
}

NR_SHELL_CMD_EXPORT(cm_gnss, SHELL_gnss);
#endif

#endif

