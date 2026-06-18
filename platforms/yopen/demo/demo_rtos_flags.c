/**
 * @file    demo_rtos_flags.c
 * @brief   FreeRTOS Event Flag 测试demo
 */

#include "yopen_os.h"
#include "yopen_error.h"
#include "yopen_debug.h"

#define FLAG_TASK_STACK_SIZE      1024*4
#define FLAG_TASK_PRIORITY        5
#define FLAG_TEST_BIT_0           (1UL << 0)
#define FLAG_TEST_BIT_1           (1UL << 1)
#define FLAG_TEST_BIT_2           (1UL << 2)
#define FLAG_TEST_ALL_BITS        (FLAG_TEST_BIT_0 | FLAG_TEST_BIT_1 | FLAG_TEST_BIT_2)

static yopen_flag_t g_test_flag = NULL;
static yopen_task_t g_flag_task_handle = NULL;
static yopen_timer_t g_flag_timer_handle = NULL;

static void flag_test_task(void *param)
{
    uint32_t flags;
    uint32_t result;

    (void)param;

    yopen_trace("[Flag Test] Task started, waiting for flags...");

    while (1)
    {
        flags = FLAG_TEST_BIT_0 | FLAG_TEST_BIT_1;

        result = yopen_rtos_flag_wait(
            g_test_flag,
            flags,
            YOPEN_FLAG_OR_CLEAR,
            5000
        );

        if (result == 0)
        {
            yopen_trace("[Flag Test] Wait timeout");
        }
        else if (result & flags)
        {
            yopen_trace("[Flag Test] Got flags: 0x%08X", result);
        }

        yopen_rtos_task_sleep_ms(500);
    }
}

static void flag_set_timer_callback(void *param)
{
    static uint8_t toggle = 0;

    (void)param;

    toggle ^= 1;

    if (toggle)
    {
        yopen_trace("[Flag Test] Set BIT_0");
        yopen_rtos_flag_set(g_test_flag, FLAG_TEST_BIT_0);
    }
    else
    {
        yopen_trace("[Flag Test] Set BIT_1");
        yopen_rtos_flag_set(g_test_flag, FLAG_TEST_BIT_1);
    }
}

void demo_rtos_flags_basic_test(void)
{
    uint32_t flags;
    uint32_t old_flags;

    yopen_trace("========== RTOS Flag Basic Test ==========");

    yopen_trace("[Test] Step 1: Create flag");
    if (yopen_rtos_flag_create(&g_test_flag) != YOPEN_RET_OK)
    {
        yopen_trace("[Test] Failed to create flag");
        return;
    }
    yopen_trace("[Test] Flag created successfully");

    yopen_trace("[Test] Step 2: Get initial flag value");
    flags = yopen_rtos_flag_get(g_test_flag);
    yopen_trace("[Test] Initial flags: 0x%08X", flags);

    yopen_trace("[Test] Step 3: Set flags (BIT_0 | BIT_1)");
    old_flags = yopen_rtos_flag_set(g_test_flag, FLAG_TEST_BIT_0 | FLAG_TEST_BIT_1);
    yopen_trace("[Test] Old flags before set: 0x%08X", old_flags);

    yopen_trace("[Test] Step 4: Get flags after set");
    flags = yopen_rtos_flag_get(g_test_flag);
    yopen_trace("[Test] Current flags: 0x%08X (expected: 0x%08X)", flags, FLAG_TEST_BIT_0 | FLAG_TEST_BIT_1);

    yopen_trace("[Test] Step 5: Wait for flag (OR, 2 seconds)");
    yopen_trace("[Test] Waiting for BIT_2...");
    flags = yopen_rtos_flag_wait(g_test_flag, FLAG_TEST_BIT_2, YOPEN_FLAG_OR, 2000);
    if (flags & FLAG_TEST_BIT_2)
    {
        yopen_trace("[Test] Wait success, got BIT_2: 0x%08X", flags);
    }
    else
    {
        yopen_trace("[Test] Wait timeout (expected), current flags: 0x%08X", flags);
    }

    yopen_trace("[Test] Step 6: Set BIT_2 from task context");
    yopen_rtos_flag_set(g_test_flag, FLAG_TEST_BIT_2);

    yopen_trace("[Test] Step 7: Wait for flag (OR, no wait)");
    flags = yopen_rtos_flag_wait(g_test_flag, FLAG_TEST_BIT_0, YOPEN_FLAG_OR, YOPEN_NO_WAIT);
    yopen_trace("[Test] Got flags: 0x%08X", flags);

    yopen_trace("[Test] Step 8: Clear flags (BIT_0)");
    old_flags = yopen_rtos_flag_clear(g_test_flag, FLAG_TEST_BIT_0);
    yopen_trace("[Test] Old flags before clear: 0x%08X", old_flags);

    yopen_trace("[Test] Step 9: Get flags after clear");
    flags = yopen_rtos_flag_get(g_test_flag);
    yopen_trace("[Test] Current flags: 0x%08X (expected: 0x%08X)", flags, FLAG_TEST_BIT_1 | FLAG_TEST_BIT_2);

    yopen_trace("[Test] Step 10: Wait for ALL flags (AND)");
    yopen_trace("[Test] Current flags: 0x%08X, waiting for ALL: 0x%08X", flags, FLAG_TEST_BIT_0 | FLAG_TEST_BIT_1 | FLAG_TEST_BIT_2);
    flags = yopen_rtos_flag_wait(g_test_flag, FLAG_TEST_BIT_0 | FLAG_TEST_BIT_1 | FLAG_TEST_BIT_2, YOPEN_FLAG_AND, 1000);
    if (flags == 0)
    {
        yopen_trace("[Test] Wait timeout (expected, not all bits set)");
    }

    yopen_trace("[Test] Step 11: Set all missing bits");
    yopen_rtos_flag_set(g_test_flag, FLAG_TEST_BIT_0);
    yopen_rtos_flag_wait(g_test_flag, FLAG_TEST_ALL_BITS, YOPEN_FLAG_OR_CLEAR, 1000);
    yopen_trace("[Test] After wait with clear, flags should be 0");
    flags = yopen_rtos_flag_get(g_test_flag);
    yopen_trace("[Test] Current flags: 0x%08X", flags);

    yopen_trace("[Test] Step 12: Delete flag");
    if (yopen_rtos_flag_delete(g_test_flag) != YOPEN_RET_OK)
    {
        yopen_trace("[Test] Failed to delete flag");
    }
    else
    {
        yopen_trace("[Test] Flag deleted successfully");
    }

    yopen_trace("========== RTOS Flag Basic Test Completed ==========");
}

void demo_rtos_flags_with_task_test(void)
{
    yopen_trace("========== RTOS Flag With Task Test ==========");

    yopen_trace("[Test] Create flag");
    if (yopen_rtos_flag_create(&g_test_flag) != YOPEN_RET_OK)
    {
        yopen_trace("[Test] Failed to create flag");
        return;
    }

    yopen_trace("[Test] Create flag test task");
    if (yopen_rtos_task_create(
        &g_flag_task_handle,
        FLAG_TASK_STACK_SIZE,
        FLAG_TASK_PRIORITY,
        "FlagTestTask",
        flag_test_task,
        NULL) != YOPEN_RET_OK)
    {
        yopen_trace("[Test] Failed to create task");
        yopen_rtos_flag_delete(g_test_flag);
        return;
    }

    yopen_trace("[Test] Create timer to set flags periodically");
    if (yopen_rtos_timer_create(
        &g_flag_timer_handle,
        YOPEN_TimerPeriodic,
        flag_set_timer_callback,
        NULL) != YOPEN_RET_OK)
    {
        yopen_trace("[Test] Failed to create timer");
        yopen_rtos_task_delete(g_flag_task_handle);
        yopen_rtos_flag_delete(g_test_flag);
        return;
    }

    yopen_trace("[Test] Start timer");
    yopen_rtos_timer_start(g_flag_timer_handle, 1000);

    yopen_trace("[Test] Wait 10 seconds for task to receive flags...");
    yopen_rtos_task_sleep_ms(10000);

    yopen_trace("[Test] Stop and delete timer");
    yopen_rtos_timer_stop(g_flag_timer_handle);
    yopen_rtos_timer_delete(g_flag_timer_handle);

    yopen_trace("[Test] Delete task");
    yopen_rtos_task_delete(g_flag_task_handle);

    yopen_trace("[Test] Delete flag");
    yopen_rtos_flag_delete(g_test_flag);

    yopen_trace("========== RTOS Flag With Task Test Completed ==========");
}

void rtos_flags_thread(void* argv)
{
    yopen_rtos_task_sleep_ms(2000);
    yopen_trace("******************************************************");
    yopen_trace("*           RTOS Event Flag API Demo                 *");
    yopen_trace("******************************************************");

    demo_rtos_flags_basic_test();

    demo_rtos_flags_with_task_test();

    yopen_trace("******************************************************");
    yopen_trace("*              All Tests Completed                   *");
    yopen_trace("******************************************************");
}
