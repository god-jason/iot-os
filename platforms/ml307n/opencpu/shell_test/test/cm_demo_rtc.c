/**
 *  @file    cm_demo_rtc.c
 *  @brief   OpenCPU RTC 示例
 *  @copyright copyright © 2025 China Mobile IOT. All rights reserved.
 *  @author by cmiot0752
 *  @date 2025/06/06
 */
#ifdef CM_DEMO_RTC_SUPPORT
#ifdef OS_USING_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "cm_sys.h"
#include "cm_rtc.h"

#define cm_demo_printf  osPrintf

#define SECOND_OF_DAY (24*60*60)
static const char DayOfMon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


/**
 * @brief 将秒数转换为日期时间结构体
 *
 * @param [in]  a_lSec   时间戳（自1970-01-01以来的秒数）
 * @param [out] a_tTime  输出的时间结构体
 */
static void __sec_to_date(int64_t a_lSec, cm_tm_t *a_tTime)
{
    uint16_t i      = 1970;
    uint16_t j      = 0;
    uint16_t iDay   = 0;
    uint32_t lDay   = (uint32_t)(a_lSec / SECOND_OF_DAY);
    int64_t  lSec   = a_lSec % SECOND_OF_DAY;

    while (lDay > 365)
    {
        if (((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0)))
        {
            lDay -= 366;
        }
        else
        {
            lDay -= 365;
        }

        i++;
    }

    if ((lDay == 365) && !(((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0))))
    {
        lDay -= 365;
        i++;
    }

    a_tTime->tm_year = i;

    for (j = 0; j < 12; j++)
    {
        if ((j == 1) && (((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0))))
        {
            iDay = 29;
        }
        else
        {
            iDay = DayOfMon[j];
        }

        if (lDay >= iDay)
        {
            lDay -= iDay;
        }
        else
        {
            break;
        }
    }

    a_tTime->tm_mon  = j + 1;
    a_tTime->tm_mday = lDay + 1;
    a_tTime->tm_hour = (int32_t)(lSec / 3600) % 24; // 注意：已包含北京时间差8小时
    a_tTime->tm_min  = (int32_t)(lSec % 3600) / 60;
    a_tTime->tm_sec  = (int32_t)(lSec % 60);
}

/** 闹钟回调函数示例 */
static void __rtc_alarm_cb(void)
{
    cm_demo_printf("RTC Alarm Triggered!\n");
}

/**
 *  @brief 设置系统时间为当前时间 + offset 秒
 *
 *  @param [in] offset_seconds 偏移秒数
 *  @return None
 */
static void __set_current_time(int32_t offset_seconds)
{
    uint64_t now = cm_rtc_get_current_time();
    uint64_t new_time = now + offset_seconds;
    int32_t ret = cm_rtc_set_current_time(new_time);
    if (ret == 0)
    {
        cm_demo_printf("Time set successfully: %llu\n", (unsigned long long)new_time);
        cm_tm_t set_time = {0};
        __sec_to_date(new_time, &set_time);
        cm_demo_printf("Timer set for: %d-%02d-%02d %02d:%02d:%02d\n",
                       set_time.tm_year, set_time.tm_mon, set_time.tm_mday,
                       set_time.tm_hour, set_time.tm_min, set_time.tm_sec);
    }
    else
    {
        cm_demo_printf("Failed to set time\n");
    }
}

/** 定时器回调函数示例 */
static void __rtc_timer_cb(void * data)
{
    cm_demo_printf("RTC Timer Triggered!\n");
}

/**
 *  @brief 设置闹钟时间为当前时间 + offset 秒
 *
 *  @param [in] offset_seconds 偏移秒数
 *  @return None
 */
static void __set_alarm_time(int32_t offset_seconds)
{
    uint64_t now = cm_rtc_get_current_time() + cm_rtc_get_timezone_v2()*900 + offset_seconds;
    cm_tm_t alarm_time = {0};
    __sec_to_date(now, &alarm_time);
    int32_t ret = cm_rtc_set_alarm_v2(CM_RTC_ID_0, &alarm_time);

    if (ret == 0)
    {
        cm_demo_printf("Alarm set for: %d-%02d-%02d %02d:%02d:%02d\n",
                       alarm_time.tm_year, alarm_time.tm_mon, alarm_time.tm_mday,
                       alarm_time.tm_hour, alarm_time.tm_min, alarm_time.tm_sec);
    }
    else
    {
        cm_demo_printf("Failed to set alarm\n");
    }
}

/**
 *  @brief RTC 测试命令入口
 *
 *  @param [in] cli CLI句柄
 *  @param [in] args 参数列表
 *  @param [in] context 用户参数
 *  @return None
 */
void cm_test_rtc(char argc, char **argv)
{
    if(argc > 1 && argv[1] != NULL)
    {
        const char *cmd = argv[1]; // 获取第一个参数
        if (strcmp(cmd, "set_time") == 0)
        {
            __set_current_time(0); // 设置为当前时间
            return;
        }
        else if (strcmp(cmd, "set_alarm") == 0)
        {
            cm_rtc_register_alarm_cb_v2(CM_RTC_ID_0, __rtc_alarm_cb);
            __set_alarm_time(5); // 设置为当前时间+5秒
            cm_rtc_enable_alarm_v2(CM_RTC_ID_0, true);
            return;
        }
        else if (strcmp(cmd, "get_alarm") == 0)
        {
            cm_tm_t alarm_time = {0};
            cm_rtc_get_alarm_v2(CM_RTC_ID_0, &alarm_time);
            cm_demo_printf("Alarm get: %d-%02d-%02d %02d:%02d:%02d\n",
                alarm_time.tm_year, alarm_time.tm_mon, alarm_time.tm_mday,
                alarm_time.tm_hour, alarm_time.tm_min, alarm_time.tm_sec);
            return;
        }
        else if (strcmp(cmd, "get_time") == 0)
        {
            uint64_t now = cm_rtc_get_current_time();
            cm_demo_printf("Current timestamp: %llu\n", (unsigned long long)now);
            cm_tm_t get_time = {0};
            __sec_to_date(now , &get_time);
            cm_demo_printf("Timer get for: %d-%02d-%02d %02d:%02d:%02d\n",
                           get_time.tm_year, get_time.tm_mon, get_time.tm_mday,
                           get_time.tm_hour, get_time.tm_min, get_time.tm_sec);
            return;
        }
        else if (strcmp(cmd, "get_timezone") == 0)
        {
            int32_t tz = cm_rtc_get_timezone_v2();
            cm_demo_printf("Current timezone: %d\n", tz);
            return;
        }
        else if (argc == 3 && strcmp(cmd, "set_timezone") == 0 && argv[2]!= NULL)
        {
            int32_t tz = atoi(argv[2]); // 第二个参数是时区
            int32_t ret = cm_rtc_set_timezone_v2(tz);
            cm_demo_printf("Timezone set ret: %d\n", ret);
            return;
        }
        else if (argc == 3 && strcmp(cmd, "timer_start") == 0 && argv[2]!= NULL)
        {
            uint32_t timeout = atoi(argv[2]); // 第二个参数是定时器时长
            int32_t ret = cm_rtc_timer_start(CM_RTC_ID_0, timeout, __rtc_timer_cb, NULL);
            cm_demo_printf("Timer start ret: %d\n", ret);
            return;
        }
        else if (strcmp(cmd, "timer_stop") == 0)
        {
            int32_t ret = cm_rtc_timer_stop(CM_RTC_ID_0);
            cm_demo_printf("Timer stop ret: %d\n", ret);
            return;
        }
    }
    cm_demo_printf("Usage:\n");
    cm_demo_printf("  rtc set_time       - set time\n");
    cm_demo_printf("  rtc get_time       - get time\n");
    cm_demo_printf("  rtc set_timezone X - set timezone\n");
    cm_demo_printf("  rtc get_timezone   - get timezone\n");
    cm_demo_printf("  rtc set_alarm      - set alarm\n");
    cm_demo_printf("  rtc get_alarm      - get alarm\n");
    cm_demo_printf("  rtc timer_start X  - start timer\n");
    cm_demo_printf("  rtc timer_stop     - stop timer\n");
}

#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(rtc, cm_test_rtc);
#endif
#endif

