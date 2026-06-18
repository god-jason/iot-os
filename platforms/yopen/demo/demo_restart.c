#include "yopen_debug.h"
#include "yopen_os.h"
#include "yopen_power.h"

enum  {
    YOPEN_REASON_POWERKEY_TEST,
    YOPEN_REASON_SWRESET_TEST,
    YOPEN_REASON_PADRESET_TEST,
    YOPEN_REASON_HARDFAULT_TEST,
    YOPEN_REASON_ASSERT_TEST,
    YOPEN_REASON_WDT_TEST,
} yopen_reset_test_opt_e;

static int test_opt = YOPEN_REASON_POWERKEY_TEST;

const char *yopen_reset_reason_str(yopen_reset_reason_e reason)
{
    switch (reason)
    {
        case YOPEN_REASON_POWERKEY:  return "POWERKEY";
        case YOPEN_REASON_SWRESET:   return "SWRESET";
        case YOPEN_REASON_PADRESET:  return "PADRESET";
        case YOPEN_REASON_HARDFAULT: return "HARDFAULT";
        case YOPEN_REASON_ASSERT:    return "ASSERT";
        case YOPEN_REASON_WDT:       return "WDT";
        case YOPEN_REASON_UNKNOWN: 
        default:                     return "UNKNOWN";  // 防止新增枚举未处理
    }
}

void restart_demo_thread(void* argv)
{
    uint16 i = 0;
    yopen_reset_reason_e reason;
    // 获取上电原因
    yopen_get_powerup_reason(&reason);
    
    while (1)
    {
        yopen_rtos_task_sleep_ms(1000);
        yopen_trace("restart_demo_thread cnt-%d, %s", i++, yopen_reset_reason_str(reason));

        if (i == 10) {
            switch(test_opt)
            {
                case YOPEN_REASON_SWRESET_TEST:
                    yopen_trace("test swreset");
                    yopen_power_reset(RESET_NORMAL);
                    break;
                case YOPEN_REASON_HARDFAULT_TEST:
                    yopen_trace("test hardfault");
                    *(volatile uint32*)0x00000000 = 0x12345678; // 触发硬错误
                    break;
                case YOPEN_REASON_ASSERT_TEST:
                    yopen_trace("test assert");
                    YOPEN_ASSERT(0); // 触发断言
                    break;
                case YOPEN_REASON_WDT_TEST:
                    yopen_trace("test wdt");
                    while(1);
                case YOPEN_REASON_PADRESET_TEST:
                case YOPEN_REASON_POWERKEY_TEST:
                default:
                    break;
            }
        }
    }
}
