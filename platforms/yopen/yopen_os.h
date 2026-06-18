


#ifndef _YOPEN_OS_H_
#define _YOPEN_OS_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "yopen_osi_def.h"
#include "yopen_type.h"

/**
 * @defgroup yopen_os os功能
 * @{
*/

/*===========================================================================
 *	Macro Definition
 ===========================================================================*/
#define YOPEN_APP_TASK_PRIORITY  			APP_PRIORITY_NORMAL
#define osFlagsWaitAny        				0x00000000U ///< Wait for any flag (default).
#define osFlagsWaitAll        				0x00000001U ///< Wait for all flags.
#define osFlagsNoClear        				0x00000002U ///< Do not clear flags which have been specified to wait for.
#define yopen_is_timer_init(timer)				((*timer != NULL)?(true):(false))

typedef int    YOpenOSStatus; //参考yopen_error.h文件，YOPEN_RET_OK表示成功，其他值表示失败

/*===========================================================================
 *	Structure Definition
 ===========================================================================*/
typedef void * yopen_task_t;
typedef void * yopen_sem_t;
typedef void * yopen_mutex_t;
typedef void * yopen_queue_t;
typedef void * yopen_timer_t;
typedef void * yopen_flag_t;
typedef void   (*yopen_timer_cb)(void);

/*========================================================================
 *	Enumeration Definition
*========================================================================*/
/** @brief 事件标志触发方式*/
typedef enum
{
	YOPEN_FLAG_WAIT_ANY = 0,	//等待任何标志
	YOPEN_FLAG_WAIT_ALL,		//等待所有标志
	YOPEN_FLAG_NO_CLEAR,		//不清除等待的标志
} yopen_flag_wait_e;

/** @brief 事件标志处理方式*/
typedef enum
{
	YOPEN_FLAG_AND 		= 5,	//等待输入事件中的所有位被设置, 事件处理完成后, 不清除事件标志
	YOPEN_FLAG_AND_CLEAR 	= 6,	//等待输入事件中的所有位被设置, 事件处理完成后, 清除事件标志
	YOPEN_FLAG_OR 			= 7,	//等待输入事件中的任何位被设置, 任务处理完事件后, 不清除事件标志
	YOPEN_FLAG_OR_CLEAR	= 8		//等待输入事件中的任何位被设置, 任务处理完事件后, 清除事件标志
} yopen_flag_op_e;

/** @brief 等待时间*/
typedef enum
{
	YOPEN_WAIT_FOREVER = 0xFFFFFFFFUL,		//一直等待
	YOPEN_NO_WAIT	  	= 0					//不等待
} yopen_wait_e;

/** @brief 定时器周期*/
typedef enum {
  YOPEN_TimerOnce               = 0,          //一次性定时器
  YOPEN_TimerPeriodic           = 1           //周期性定时器
} yopen_timertype_t;

/** @brief 任务状态*/
typedef enum
{
	Running = 0,	//任务正在运行
	Ready,			//任务处于就绪状态
	Blocked,		//任务处于阻塞状态
	Suspended,		//任务处于挂起状态
	Deleted,		//任务已被删除
	Invalid			//无效状态
} yopen_task_state_e;

typedef enum {
	TIMER_INSTANCE_0 = 0,
	TIMER_INSTANCE_MAX
}yopen_timerInstance_e;

typedef enum
{
    TIMER_RELOAD_OFF = 0U,				//一次性定时器
    TIMER_RELOAD_ON						//周期性定时器
}yopen_timerReloadOption_e;

/*===========================================================================
 *	Struct Definition
 ===========================================================================*/
/** @brief 任务属性结构体*/
typedef struct
{
	yopen_task_t			xHandle;				//任务句柄
	const char *		pcTaskName;				//任务名称
	yopen_task_state_e 	eCurrentState;			//任务状态
	unsigned long 		uxCurrentPriority;		//任务优先级
	uint16 				usStackHighWaterMark;	//任务堆栈高水位
} yopen_task_status_t;

/** @brief CPU使用信息结构体*/
typedef struct
{
	uint total_idle_tick;
	uint sys_tick_old;
	uint cpu_using;
	uint idle_in_tick;
	uint idle_out_tick;
}yopen_cpu_using_info_t;

/*========================================================================
 *	Function Definition
 *========================================================================*/
/**
 * @brief				创建一个任务(不可在中断上下文中调用)
 * @param	taskRef		任务句柄
 * @param	stackSize	任务堆栈大小
 * @param	priority	任务优先级
 * @param	taskName	任务名称
 * @param	taskStart	任务入口函数
 * @param	argv		任务入口函数参数
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_task_create
(
	yopen_task_t	 * taskRef, 			 /* OS task reference										   */
	uint32		   stackSize,			 /* number of bytes in task stack area						   */
	uint8 		   priority,			 /* task priority, 1 ~ 30									   */
	char		 * taskName,			 /* task name												   */
	void	   ( * taskStart)(void*),	 /* pointer to task entry point 			 				   */
	void*		   argv,				 /* task entry argument pointer 							   */
 	...
);


/**
 * @brief				创建一个静态任务(不可在中断上下文中调用)
 * @param	taskRef		任务句柄
 * @param	stackMem	任务堆栈内存地址
 * @param	stackSize	任务堆栈大小
 * @param	priority	任务优先级
 * @param	taskName	任务名称
 * @param	taskStart	任务入口函数
 * @param	argv		任务入口函数参数
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_task_create_static
(
	yopen_task_t	 * taskRef, 			 /* OS task reference										   */
	uint8_t 		*stackMem,
	uint32		   stackSize,			 /* number of bytes in task stack area						   */
	uint8 		   priority,			 /* task priority, 1 ~ 30									   */
	char		 * taskName,			 /* task name												   */
	void	   ( * taskStart)(void*),	 /* pointer to task entry point 			 				   */
	void*		   argv,				 /* task entry argument pointer 							   */
 	...
);
/**
 * @brief				删除一个任务(不可在中断上下文中调用)
 * @param	taskRef		任务句柄
 * @return	YOpenOSStatus
 */
//if you delete the current task itself, the code behind yopen_rtos_task_delete will have no chance to run
extern YOpenOSStatus yopen_rtos_task_delete
(
	yopen_task_t taskRef		/* OS task reference	*/
);


/**
 * @brief				挂起一个任务(不可在中断上下文中调用)
 * @param	taskRef		任务句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_task_suspend
(
	yopen_task_t taskRef		/* OS task reference	*/
);


/**
 * @brief				唤醒一个任务(不可在中断上下文中调用)
 * @param	taskRef		任务句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_task_resume
(
	yopen_task_t taskRef		/* OS task reference	*/
);


/**
 * @brief				释放CPU使用权(不可在中断上下文中调用)
 * @param
 * @return
 */
extern void yopen_rtos_task_yield(void);


/**
 * @brief				获取当前任务的任务句柄(不可在中断上下文中调用)
 * @param	taskRef		任务句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_task_get_current_ref
(
	yopen_task_t * taskRef		/* OS task reference	*/
);


/**
 * @brief					切换任务优先级(不可在中断上下文中调用)
 * @param	taskRef			任务句柄
 * @param	new_priority	任务的新优先级
 * @param	old_priority	任务的原优先级
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_task_change_priority
(
	yopen_task_t 	taskRef,			/* OS task reference			*/
	uint8 		new_priority,		/* OS task new priority	for in	*/
	uint8	   *old_priority		/* OS task old priority	for out	*/
);


/**
 * @brief					获取任务状态(不可在中断上下文中调用)
 * @param	taskRef			任务句柄
 * @param	status			任务状态
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_task_get_status
(
	yopen_task_t      		 task_ref,
	yopen_task_status_t 	 *status
);

/**
 * @brief			设置当前任务休眠时间(不可在中断上下文中调用)
 * @param	ms		休眠时长，单位ms
 * @return
 */
extern void yopen_rtos_task_sleep_ms
(
	uint32 ms	   /* OS task sleep time for ms	*/
);


/**
 * @brief			设置当前任务休眠时间(不可在中断上下文中调用)
 * @param	s		休眠时长，单位s
 * @return
 */
extern void yopen_rtos_task_sleep_s
(
	uint32 s	   /* OS task sleep time for s		*/
);

/**
 * @brief				进入临界区
 * @param
 * @return
 */
extern uint32_t yopen_rtos_enter_critical(void);


/**
 * @brief				退出临界区
 * @param
 * @return
 */
extern void yopen_rtos_exit_critical(uint32_t isrm);

/**
 * @brief					创建一个信号量
 * @param	semaRef			信号量句柄
 * @param	initialCount	信号量初始计数值 （<=1 为二值信号量, 可以在中断上下文中使用，否则不允许）
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_semaphore_create
(
	yopen_sem_t  	*semaRef,       /* OS semaphore reference                     	*/
	uint32      initialCount    /* initial count of the semaphore             	*/
);

/**
 * @brief Create a semaphore, you can configure the value of the maximum count of the semaphore.
 *
 * @param[in] yopen_sem_t * semaRef
 *          - Semaphore pointer handle
 *
 * @param[in] uint32 initialCount
 *          - Semaphore initialization, usually 0
 *
 * @param[in] uint32 max_cnt
 *          - Semaphore maximum count value
 *
 * @return    YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_semaphore_create_ex(yopen_sem_t* semaRef, uint32 initialCount, uint32 max_cnt);

/**
 * @brief					设置信号量等待时间(不可在中断上下文中调用)
 * @param	semaRef			信号量句柄
 * @param	timeout			信号量等待时间
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_semaphore_wait
(
	yopen_sem_t  	semaRef,       /* OS semaphore reference                     	*/
	uint32      timeout    	   /* YOPEN_WAIT_FOREVER, YOPEN_NO_WAIT, or timeout	*/
);



/**
 * @brief					释放一个信号量
 * @param	semaRef			信号量句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_semaphore_release
(
	yopen_sem_t   semaRef        /* OS semaphore reference						*/
);



/**
 * @brief					获取一个信号量的计数值
 * @param	semaRef			信号量句柄
 * @param	cnt_ptr			信号量的计数值
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_semaphore_get_cnt
(
	yopen_sem_t  	semaRef,       /* OS semaphore reference           				*/
	uint32    * cnt_ptr    	   /* out-parm to save the cnt of semaphore      	*/
);



/**
 * @brief					删除一个信号量(不可在中断上下文中调用)
 * @param	semaRef			信号量句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_semaphore_delete
(
	yopen_sem_t   semaRef        /* OS semaphore reference                  		*/
);


/**
 * @brief					创建一个互斥锁(不可在中断上下文中调用)
 * @param	mutexRef		互斥锁句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_mutex_create
(
    yopen_mutex_t  *mutexRef        /* OS mutex reference                         */
);


/**
 * @brief					获取一个互斥锁(不可在中断上下文中调用)
 * @param	mutexRef		互斥锁句柄
 * @param	timeout			互斥锁等待时间
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_mutex_lock
(
	yopen_mutex_t    mutexRef,       /* OS mutex reference                         */
	uint32        timeout   	  /* mutex wait timeout		             		*/
);


/**
 * @brief					尝试获取一个互斥锁(不可在中断上下文中调用)
 * @param	mutexRef		互斥锁句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_mutex_try_lock
(
	yopen_mutex_t  mutexRef        /* OS mutex reference                         */
);


/**
 * @brief					释放一个互斥锁(不可在中断上下文中调用)
 * @param	mutexRef		互斥锁句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_mutex_unlock
(
	yopen_mutex_t  mutexRef        /* OS mutex reference                         */
);


/**
 * @brief					删除一个互斥锁(不可在中断上下文中调用)
 * @param	mutexRef		互斥锁句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_mutex_delete
(
	yopen_mutex_t  mutexRef        /* OS mutex reference                         */
);


/**
 * @brief					创建一个队列(不可在中断上下文中调用)
 * @param	msgQRef			队列句柄
 * @param	maxSize			队列支持的最大消息大小
 * @param	maxNumber		队列支持的最大消息数量
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_queue_create
(
	yopen_queue_t   	*msgQRef,       	/* OS message queue reference              */
	uint32         	maxSize,        	/* max message size the queue supports     */
	uint32         	maxNumber	      	/* max # of messages in the queue          */
);

/**
 * @brief					等待一个队列中的消息
 * @param	msgQRef			队列句柄
 * @param	recvMsg			接收消息指针
 * @param	size			消息大小
 * @param	timeout			队列等待时间
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_queue_wait
(
	yopen_queue_t   	msgQRef,		/* message queue reference                 		*/
	uint8  	      	*recvMsg,       /* pointer to the message received         		*/
	uint32         	size, 			/* size of the message                     		*/
	uint32         	timeout         /* YOPEN_WAIT_FOREVER, YOPEN_NO_WAIT, or timeout  */
);


/**
 * @brief					发送消息到一个队列
 * @param	msgQRef			队列句柄
 * @param	size			消息大小
 * @param	msgPtr			发送消息指针
 * @param	timeout			队列发送等待时间
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_queue_release
(
    yopen_queue_t		msgQRef,        /* message queue reference                 			*/
    uint32         	size,           /* size of the message                     			*/
    uint8          	*msgPtr,        /* start address of the data to be sent    			*/
    uint32         	timeout         /* YOPEN_WAIT_FOREVER, YOPEN_NO_WAIT, or timeout   	*/
);



/**
 * @brief					获取一个队列中的消息数量
 * @param	msgQRef			队列句柄
 * @param	cnt_ptr			消息数量
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_queue_get_cnt
(
	yopen_queue_t		msgQRef,        /* message queue reference                 		*/
	uint32     		*cnt_ptr    	/* out-parm to save the cnt of message queue	*/
);


/**
 * @brief					删除一个队列(不可在中断上下文中调用)
 * @param	msgQRef			队列句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_queue_delete
(
	yopen_queue_t	msgQRef         /* message queue reference                 		*/
);


/**
 * @brief						创建一个定时器(不可在中断上下文中调用)
 * @param	timerRef			定时器句柄
 * @param	cyclicalEn			定时器类型(周期定时器/单次定时器)
 * @param	callBackRoutine		定时器回调函数
 * @param	timerArgc			定时器回调函数参数
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_timer_create
(
	yopen_timer_t	* timerRef, 				// OS supplied timer reference
	yopen_timertype_t	cyclicalEn,
	void		(*callBackRoutine)(void *), // timer call-back routine
	void		*timerArgc					// argument to be passed to call-back on expiration
);


/**
 * @brief						开启一个定时器
 * @param	timerRef			定时器句柄
 * @param	set_Time			定时器超时时间
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_timer_start
(
	yopen_timer_t 		 timerRef,					 // OS supplied timer reference
	uint32			 set_Time				     // timer set value
);

/**
 * @brief						开启一个定时器
 * @param	timerRef			定时器句柄
 * @param	set_Time			定时器超时时间
 * @param	cyclicalEn			定时器类型(周期定时器/单次定时器)
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_timer_start2
(
	yopen_timer_t 		 timerRef,					 // OS supplied timer reference
	uint32			 set_Time,				     // timer set value
	yopen_timertype_t	cyclicalEn
);

/**
 * @brief						判断一个定时器是否正在计时
 * @param	timerRef			定时器句柄
 * @return	TRUE/FALSE
 */
extern BOOLEAN yopen_rtos_timer_is_running
(
	yopen_timer_t 		 timerRef					 // OS supplied timer reference
);


/**
 * @brief						停止一个定时器
 * @param	timerRef			定时器句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_timer_stop
(
	yopen_timer_t timerRef 				/* OS supplied timer reference	*/
);

/**
 * @brief						删除一个定时器(不可在中断上下文中调用)
 * @param	timerRef			定时器句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_timer_delete
(
	yopen_timer_t timerRef 				/* OS supplied timer reference	*/
);


/**
 * @brief						获取系统时钟节拍数
 * @param
 * @return						系统时钟节拍数（1tick=1ms）
 */
extern uint32 yopen_rtos_get_system_tick(void);


/**
 * @brief					创建一个事件标志组(不可在中断上下文中调用)
 * @param	flagRef			事件标志组句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_flag_create
(
	yopen_flag_t  	*flagRef        /* OS flag reference                     */
);


/**
 * @brief					删除一个事件标志组(不可在中断上下文中调用)
 * @param	flagRef			事件标志组句柄
 * @return	YOpenOSStatus
 */
extern YOpenOSStatus yopen_rtos_flag_delete
(
	yopen_flag_t   flagRef        /* OS flag reference                     */
);


/**
 * @brief					设置事件标志位
 * @param	flagRef			事件标志组句柄
 * @param	bits			要设置的事件标志位
 * @return					设置前的事件标志位值
 */
extern uint32 yopen_rtos_flag_set
(
	yopen_flag_t   flagRef,        /* OS flag reference                     */
	uint32        bits             /* bits to set                          */
);


/**
 * @brief					清除事件标志位
 * @param	flagRef			事件标志组句柄
 * @param	bits			要清除的事件标志位
 * @return					清除前的事件标志位值
 */
extern uint32 yopen_rtos_flag_clear
(
	yopen_flag_t   flagRef,        /* OS flag reference                     */
	uint32        bits             /* bits to clear                        */
);


/**
 * @brief					等待事件标志位(不可在中断上下文中调用)
 * @param	flagRef			事件标志组句柄
 * @param	bits			要等待的事件标志位
 * @param	option			等待选项(YOPEN_FLAG_AND/YOPEN_FLAG_AND_CLEAR/YOPEN_FLAG_OR/YOPEN_FLAG_OR_CLEAR)
 * @param	timeout			等待超时时间(单位ms)
 * @return					满足条件时的事件标志位值，超时返回0
 */
extern uint32 yopen_rtos_flag_wait
(
	yopen_flag_t      flagRef,        /* OS flag reference                     */
	uint32           bits,            /* bits to wait for                      */
	yopen_flag_op_e  option,          /* wait option                           */
	uint32           timeout          /* YOPEN_WAIT_FOREVER, YOPEN_NO_WAIT, or timeout */
);


/**
 * @brief					获取当前事件标志位
 * @param	flagRef			事件标志组句柄
 * @return					当前事件标志位值
 */
extern uint32 yopen_rtos_flag_get
(
	yopen_flag_t   flagRef        /* OS flag reference                     */
);


#ifdef __cplusplus
} /*"C" */
#endif

#endif
