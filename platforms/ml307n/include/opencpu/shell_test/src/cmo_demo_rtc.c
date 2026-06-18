/*********************************************************
 *  @file    cm_demo_fs.c
 *  @brief   OpenCPU FS示例
 *  Copyright (c) 2021 China Mobile IOT.
 *  All rights reserved.
 *  created By WP0201 2022/7/14
 ********************************************************/
#ifdef CM_DEMO_RTC_SUPPORT
#ifdef OS_USING_SHELL
#include <os.h>
#include <stdlib.h>
#include <nr_micro_shell.h>
#include "cm_rtc.h"

#define SECOND_OF_DAY (24*60*60)
static const char * weekday[] = {"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
static const char DayOfMon[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
#define cm_demo_printf osPrintf

static void cm_sec_to_date(long lSec, cm_tm_t *tTime)
{
    unsigned short i,j,iDay;
    unsigned long lDay;

    lDay = lSec / SECOND_OF_DAY;
    lSec = lSec % SECOND_OF_DAY;

    i = 1970;
    while(lDay > 365)
    {
        if(((i%4==0)&&(i%100!=0)) || (i%400==0))
        {
            lDay -= 366;
        }
        else
        {
            lDay -= 365;
        }
        i++;
    }
    if((lDay == 365) && !(((i%4==0)&&(i%100!=0)) || (i%400==0)))
    {
       lDay -= 365;
       i++;
    }
    tTime->tm_year = i;
    for(j=0;j<12;j++)
    {
        if((j==1) && (((i%4==0)&&(i%100!=0)) || (i%400==0)))
        {
            iDay = 29;
        }
        else
        {
            iDay = DayOfMon[j];
        }
        if(lDay >= iDay) lDay -= iDay;
        else break;
  }
    tTime->tm_mon = j+1;
    tTime->tm_mday = lDay+1;
    tTime->tm_hour = ((lSec / 3600))%24;//这里注意，世界时间已经加上北京时间差8，
    tTime->tm_min = (lSec % 3600) / 60;
    tTime->tm_sec = (lSec % 3600) % 60;
}

static uint8_t cm_time_to_weekday(cm_tm_t *t)
{
    uint32_t u32WeekDay = 0;
    uint32_t u32Year = t->tm_year;
    uint8_t u8Month = t->tm_mon;
    uint8_t u8Day = t->tm_mday;
    if(u8Month < 3U)
    {
        /*D = { [(23 x month) / 9] + day + 4 + year + [(year - 1) / 4] - [(year - 1) / 100] + [(year - 1) / 400] } mod 7*/
        u32WeekDay = (((23U * u8Month) / 9U) + u8Day + 4U + u32Year + ((u32Year - 1U) / 4U) - ((u32Year - 1U) / 100U) + ((u32Year - 1U) / 400U)) % 7U;
    }
    else
    {
        /*D = { [(23 x month) / 9] + day + 4 + year + [year / 4] - [year / 100] + [year / 400] - 2 } mod 7*/
        u32WeekDay = (((23U * u8Month) / 9U) + u8Day + 4U + u32Year + (u32Year / 4U) - (u32Year / 100U) + (u32Year / 400U) - 2U ) % 7U;
    }

    if (0U == u32WeekDay)
    {
        u32WeekDay = 7U;
    }

    return (uint8_t)u32WeekDay;
}


void alarm_cb(void)
{
    cm_demo_printf("alarm_cb\n");
}

void SHELL_testRtc(char argc, char **argv)
{
    cm_tm_t t;
    cm_tm_t t_get;

    cm_demo_printf("CM OpenCPU testRtc Starts\n");

    cm_sec_to_date((long)cm_rtc_get_current_time(), &t);
    cm_demo_printf("Now:%d-%d-%d,%d:%d:%d,%s\n", t.tm_year, t.tm_mon , t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,weekday[cm_time_to_weekday(&t)-1]);

    if(cm_rtc_set_current_time(cm_rtc_get_current_time()+600)<0) //+10min set
    {
        cm_demo_printf("cm_rtc_get_current_time fail\n");
    }
    cm_sec_to_date((long)cm_rtc_get_current_time(), &t);
    cm_demo_printf("after settime +10min,Now:%d-%d-%d,%d:%d:%d,%s\n", t.tm_year, t.tm_mon , t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,weekday[cm_time_to_weekday(&t)-1]);

    t.tm_min += 1;
    cm_rtc_set_alarm(&t);
    cm_rtc_register_alarm_cb(alarm_cb);
    cm_rtc_enable_alarm(true);
    osDelay(65000);
    if(cm_rtc_get_alarm(&t_get)<0)
    {
        cm_demo_printf("cm_rtc_enable_alarm fail\n");
    }
    cm_demo_printf("after set&enable alarm,alarm:%d-%d-%d,%d:%d:%d\n", t_get.tm_year, t_get.tm_mon , t_get.tm_mday, t_get.tm_hour, t_get.tm_min, t_get.tm_sec);

    cm_rtc_enable_alarm(false);
    if(cm_rtc_get_alarm(&t_get)==0)
    {
        cm_demo_printf("after disable alarm,alarm:%d-%d-%d,%d:%d:%d\n", t_get.tm_year, t_get.tm_mon , t_get.tm_mday, t_get.tm_hour, t_get.tm_min, t_get.tm_sec);
    }

    cm_rtc_set_alarm(&t);
    cm_rtc_register_alarm_cb(alarm_cb);
    cm_rtc_enable_alarm(true);

    t.tm_min = 60;
    if(cm_rtc_set_alarm(&t)== 0)
    {
        cm_demo_printf("cm_rtc_set_alarm test fail\n");
    }
    cm_sec_to_date((long)cm_rtc_get_current_time(), &t);
    t.tm_sec = 60;
    if(cm_rtc_set_alarm(&t)== 0)
    {
        cm_demo_printf("cm_rtc_set_alarm test fail\n");
    }
    cm_sec_to_date((long)cm_rtc_get_current_time(), &t);
    t.tm_hour = 24;
    if(cm_rtc_set_alarm(&t)== 0)
    {
        cm_demo_printf("cm_rtc_set_alarm test fail\n");
    }
    cm_sec_to_date((long)cm_rtc_get_current_time(), &t);
    t.tm_mday = 32;
    if(cm_rtc_set_alarm(&t)== 0)
    {
        cm_demo_printf("cm_rtc_set_alarm test fail\n");
    }
    cm_sec_to_date((long)cm_rtc_get_current_time(), &t);
    t.tm_mon = 14;
    if(cm_rtc_set_alarm(&t)== 0)
    {
        cm_demo_printf("cm_rtc_set_alarm test fail\n");
    }

    cm_demo_printf("Now Time Zone:%d\n",cm_rtc_get_timezone());
    if(cm_rtc_set_timezone(cm_rtc_get_timezone()+1)<0)
    {
        cm_demo_printf("cm_rtc_set_timezone normal fail\n");
    }
    cm_demo_printf("after setzone +1,Now Time Zone:%d\n",cm_rtc_get_timezone());
    if(cm_rtc_set_timezone(cm_rtc_get_timezone()+24)== 0)
    {
        cm_demo_printf("cm_rtc_set_timezone abnormal fail\n");
    }

    cm_demo_printf("CM OpenCPU testRtc Ends\n");
}

NR_SHELL_CMD_EXPORT(cm_rtc, SHELL_testRtc);
#endif
#endif

