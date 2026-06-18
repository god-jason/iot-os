/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/

#include "yopen_os.h"
#include "yopen_type.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "yopen_debug.h"

/*
   This project demotrates the usage of OS, in this example, we'll create two tasks, a semaphore and a message queue.
   The main task is created first, and the sub task sends a message to the main task after the main task is created. 
   The main task prints the message. 
   The test is completed after 10 cycles.
 */

#define MSG_MAX_SIZE 128
#define MSG_MAX_NUM 10
#define MSG_TO_QUEUE "message from sub task to main task"

// extern _yopen_trace_t yopen_trace;
static yopen_sem_t rtos_test_sem = NULL;
static yopen_queue_t rtos_test_queue = NULL;
static yopen_timer_t rtos_test_timer = NULL;
static int test_count = 0;

void os_demo_sub_task(void * argv)
{
	while(test_count <= 10)
	{
		yopen_rtos_semaphore_wait(rtos_test_sem, 0xFFFFFFFF); // wait for main task starting up
		
		yopen_trace("sub_task start");
		
		yopen_rtos_queue_release(rtos_test_queue, strlen(MSG_TO_QUEUE), (u8*)MSG_TO_QUEUE, 0);
		
		yopen_trace("sub_task send msg");
		
		yopen_rtos_task_sleep_ms(200);
	}
	
	yopen_trace("delete sub task");
	yopen_rtos_task_delete(NULL);  // kill itself
}
void yopen_ms_timer_func()
{
    yopen_trace("enter yopen_ms_timer_func");
}

void os_demo_main_task(void * argv)
{
	yopen_task_status_t status;
	test_count = 0;
	
	char* msg = malloc(MSG_MAX_SIZE);

	yopen_trace("main_task start0");
	
	yopen_rtos_semaphore_create(&rtos_test_sem, 0);

	yopen_trace("main_task start1");
		
	yopen_rtos_queue_create(&rtos_test_queue, MSG_MAX_SIZE, MSG_MAX_NUM);
	yopen_trace("main_task start2");
	
	while(test_count <= 10)
	{
		yopen_trace("main_task start");
		
		yopen_rtos_semaphore_release(rtos_test_sem);  // tell sub task that the main task has started
		
		yopen_rtos_queue_wait(rtos_test_queue, (u8*)msg, sizeof(msg), 0xFFFFFFFF);  // wait for the message from sub task
		
		yopen_trace("main_task recv msg: %s", msg);
		
		test_count += 1;
		
		yopen_trace("test count %d", test_count);
	}
	
	for(int i=0;i<6;i++)
	{
		yopen_trace("sleep %d s", i);
		yopen_rtos_task_sleep_ms(1000);
	}
	
	yopen_trace("ms timer example");
	yopen_rtos_timer_create(&rtos_test_timer, YOPEN_TimerOnce, yopen_ms_timer_func, NULL);
	
	yopen_rtos_timer_start(rtos_test_timer, 1000);

	yopen_rtos_task_get_status(NULL, &status);
	yopen_trace("========== rtos status =========");
	yopen_trace("task pcTaskName %s", status.pcTaskName);
	yopen_trace("task eCurrentState %d", status.eCurrentState);
	yopen_trace("task uxCurrentPriority %d", status.uxCurrentPriority);
	yopen_trace("task usStackHighWaterMark %d", status.usStackHighWaterMark);

	yopen_trace("========== rtos test finished");
	
	free(msg);
	
	yopen_trace("delete main task");
	yopen_rtos_task_delete(NULL);  // kill itself
}


