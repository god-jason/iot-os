/**
 *  @file    cm_demo_rtc.c
 *  @brief   OpenCPU RTC 示例
 *  @copyright copyright © 2025 China Mobile IOT. All rights reserved.
 *  @author by cmiot0752
 *  @date 2025/06/06
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cm_sys.h"
#include "cm_rtc.h"
#include "cm_demo_uart.h"
#include "cm_demo_rtc.h"

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
    cm_log_printf(0, "RTC Alarm Triggered!\n");
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
    }
    else
    {
        cm_demo_printf("Failed to set time\n");
    }
}

/**
 *  @brief 设置闹钟时间为当前时间 + offset 秒
 *
 *  @param [in] offset_seconds 偏移秒数
 *  @return None
 */
static void __set_alarm_time(int32_t offset_seconds)
{
    uint64_t now = cm_rtc_get_current_time() + 8*3600 + offset_seconds;
    cm_tm_t alarm_time = {0};
    __sec_to_date(now, &alarm_time);
    int32_t ret = cm_rtc_set_alarm(&alarm_time);

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
void cm_test_rtc(EmbeddedCli *cli, char *args, void *context)
{
    const char *cmd = embeddedCliGetToken(args, 1); // 获取第一个参数

    if (strcmp(cmd, "set_time") == 0)
    {
        __set_current_time(0); // 设置为当前时间
    }
    else if (strcmp(cmd, "set_alarm") == 0)
    {
        cm_rtc_register_alarm_cb(__rtc_alarm_cb);
        __set_alarm_time(5); // 设置为当前时间+5秒
        cm_rtc_enable_alarm(true);
    }
    else if (strcmp(cmd, "get_alarm") == 0)
    {
        cm_tm_t alarm_time = {0};
        cm_rtc_get_alarm(&alarm_time);
        cm_demo_printf("Alarm get: %d-%02d-%02d %02d:%02d:%02d\n",
            alarm_time.tm_year, alarm_time.tm_mon, alarm_time.tm_mday,
            alarm_time.tm_hour, alarm_time.tm_min, alarm_time.tm_sec);
    }
    else if (strcmp(cmd, "get_time") == 0)
    {
        uint64_t now = cm_rtc_get_current_time();
        cm_demo_printf("Current timestamp: %llu\n", (unsigned long long)now);
    }
    else if (strcmp(cmd, "get_timezone") == 0)
    {
        int32_t tz = cm_rtc_get_timezone();
        cm_demo_printf("Current timezone: %d\n", tz);
    }
    else if (strcmp(cmd, "set_timezone") == 0)
    {
        int32_t tz = atoi(embeddedCliGetToken(args, 2)); // 第二个参数是时区
        int32_t ret = cm_rtc_set_timezone(tz);
        cm_demo_printf("Timezone set ret: %d\n", ret);
    }
    else
    {
        cm_demo_printf("Usage:\n");
        cm_demo_printf("  rtc set_time       - set time\n");
        cm_demo_printf("  rtc get_time       - get time\n");
        cm_demo_printf("  rtc set_timezone X - set timezone\n");
        cm_demo_printf("  rtc get_timezone   - get timezone\n");
        cm_demo_printf("  rtc set_alarm      - set alarm\n");
        cm_demo_printf("  rtc get_alarm      - get alarm\n");
    }
}
