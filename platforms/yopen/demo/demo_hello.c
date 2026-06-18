#include "yopen_debug.h"
#include "yopen_os.h"

void hello_demo_thread(void* argv)
{
    uint16 i = 0;
    while (1)
    {
        yopen_rtos_task_sleep_ms(1000);
        yopen_trace("hello_demo_thread cnt-%d", i++);
    }
    
}
