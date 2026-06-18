#include "yopen_rtc.h"
#include "yopen_os.h"
#include "yopen_type.h"
#include "yopen_error.h"
#include "yopen_debug.h"
#include <stdio.h>
#include <string.h>

/*========================================================================
 *  Global Variable
 *========================================================================*/   


/*===========================================================================
 * Macro Definition
 ===========================================================================*/
//#define YOPEN_RTCDEMO_LOG(msg, ...)         yopen_trace("yopen_RTCDEMO %s", msg, ##__VA_ARGS__)
//#define YOPEN_RTCDEMO_LOG_PUSH(msg, ...)    yopen_trace("yopen_RTCDEMO %s", msg, ##__VA_ARGS__)

#define	leapyear(year)		((year) % 4 == 0)
#define	days_in_year(a)		(leapyear(a) ? 366 : 365)
#define	days_in_month(a)	(month_days[(a) - 1])

#define FEBRUARY		2
#define	STARTOFTIME		1970
#define SECDAY			86400L
#define SECYR			(SECDAY * 365)

#if !defined(rtc_demo_err_exit)
	#define rtc_demo_err_exit(x, action, str)																		\
			do                                                                                                  \
			{                                                                                                   \
				if(x)                                                                        				\
				{                                                                                               \
					yopen_trace(str);																			\
					{goto action;}																					\
				}                                                                                               \
			} while( 1==0 )
#endif

#define YOPEN_RTCDEMO_PRINT_TIME(tm) yopen_trace("[RTC] %d/%02d/%02d %02d:%02d:%02d%+02d", tm.tm_year, tm.tm_mon, tm.tm_mday, \
                                                        tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_zone)


void rtc_demo_thread(void *arvg)
{
    yopen_rtc_time_t tm;
    int64_t time_t;
    yopen_errcode_rtc_e err = YOPEN_RET_OK;
    yopen_errcode_rtc_e ret;
    int timezone_value = 0;
	yopen_rtos_task_sleep_s(5);
    //get current time
    ret = yopen_rtc_get_time(&tm);
    rtc_demo_err_exit(ret != YOPEN_RET_OK, YOPEN_RTC_DEMO_EXIT, "get time err");
    
    YOPEN_RTCDEMO_PRINT_TIME(tm);

    yopen_rtc_to_sec_time(&time_t, &tm);
    yopen_trace("getting time convert %ld sec.\r\n", (uint32_t)time_t);

    yopen_sec_to_rtc_time(&time_t, &tm);
    YOPEN_RTCDEMO_PRINT_TIME(tm);

    //set time
    ret = yopen_rtc_set_time(&tm);
    rtc_demo_err_exit(ret != YOPEN_RET_OK, YOPEN_RTC_DEMO_EXIT, "set time err");

    ret = yopen_rtc_get_time(&tm);
    rtc_demo_err_exit(ret != YOPEN_RET_OK, YOPEN_RTC_DEMO_EXIT, "get time err");
    
    YOPEN_RTCDEMO_PRINT_TIME(tm);
    
	while (1) 
    { 
		yopen_rtc_get_time(&tm);
		yopen_trace("=========print current time===========\r\n");
		YOPEN_RTCDEMO_PRINT_TIME(tm);
        
        yopen_rtc_set_timezone(32);    //UTC+32
        yopen_rtc_get_time(&tm);
        yopen_trace("=========print time===========\r\n");
		YOPEN_RTCDEMO_PRINT_TIME(tm);
        
        yopen_rtc_get_timezone(&timezone_value);
        yopen_trace("timezone_value_trace: %03d\r\n", timezone_value);
		yopen_rtos_task_sleep_s(5);
	}
YOPEN_RTC_DEMO_EXIT:
    err = yopen_rtos_task_delete(NULL);
    if(err != YOPEN_OSI_SUCCESS)
    {
        yopen_trace("task deleted failed");
    }

}




