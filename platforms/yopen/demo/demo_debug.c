/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/

#include "yopen_os.h"
#include "yopen_debug.h"
#include "yopen_type.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void debug_demo_task(void * argv)
{
	uint32_t logOwnerAndLevel;

	yopen_debug_heap_info    heap_info;
	uint32_t count = 10;
	
	yopen_rtos_task_sleep_ms(1000);
	
	yopen_trace("=========debug_demo=============");
	logOwnerAndLevel = yopen_debug_get_log_level();
	yopen_trace("kernel logOwnerAndLevel 0x%08x", logOwnerAndLevel);
	yopen_trace("set error level log for all kernel owner");
	yopen_debug_set_log_level(YOPEN_DEV_LOG_ID_HW, YOPEN_DEV_LOG_LEVEL_ERROR);
	yopen_debug_set_log_level(YOPEN_DEV_LOG_ID_PHY_ONLINE, YOPEN_DEV_LOG_LEVEL_ERROR);
	yopen_debug_set_log_level(YOPEN_DEV_LOG_ID_PHY_OFFLINE, YOPEN_DEV_LOG_LEVEL_ERROR);
	yopen_debug_set_log_level(YOPEN_DEV_LOG_ID_PLAT_AP, YOPEN_DEV_LOG_LEVEL_ERROR);
	yopen_debug_set_log_level(YOPEN_DEV_LOG_ID_PLAT_CP, YOPEN_DEV_LOG_LEVEL_ERROR);
	yopen_debug_set_log_level(YOPEN_DEV_LOG_ID_PS1, YOPEN_DEV_LOG_LEVEL_ERROR);

	logOwnerAndLevel = yopen_debug_get_log_level();

	yopen_trace("kernel new logOwnerAndLevel 0x%08x", logOwnerAndLevel);

	yopen_debug_get_heap_size(&heap_info);

	
	while (count-- > 0)
	{
		yopen_trace("heap total %dKB, curr free %dKB, min free %dKB", heap_info.total_size>>10,
					heap_info.curr_free_size>>10, heap_info.min_free_size>>10);
		yopen_rtos_task_sleep_ms(1000);
	}


	// *(int*)0 = 100;

	yopen_rtos_task_delete(NULL);

}


