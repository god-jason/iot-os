
/**  @file
  ycom_power.h

  @brief
  TODO

*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/
                
#ifndef _YOPEN_POWER_H_
#define _YOPEN_POWER_H_

#include "yopen_osi_def.h"
#include "yopen_type.h"
#include "yopen_api_common.h"
#include "yopen_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup yopen_power power功能
 * @{
*/

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

#define YOPEN_LOCK_NAME_MAX_LEN	32
#define YOPEN_LOCK_MAX_CNT 10

#define YOPEN_UART_ERRCODE_BASE    (YOPEN_COMPONENT_BSP_UART<<16)
#define YOPEN_UART_SUSPEND_MIN_DELAY  1         //unit:s
#define YOPEN_UART_SUSPEND_MAX_DELAY  255       //unit:s
#define YOPEN_UART_SUSPEND_DELAY_STD  5000      //unit:ms

#define YOPEN_POWER_ERRCODE_BASE (YOPEN_COMPONENT_PM<<16)
/*========================================================================
*  Enumeration Definition
*========================================================================*/
/** @brief 唤醒源*/
typedef enum 
{
    YOPEN_WAKEUP_FROM_POR = 0,			// 开机唤醒
    YOPEN_WAKEUP_FROM_RTC,				// RTC唤醒
    YOPEN_WAKEUP_FROM_PAD,				// 引脚唤醒
    YOPEN_WAKEUP_FROM_LPUART,			// 串口唤醒
    YOPEN_WAKEUP_FROM_LPUSB,			// USB唤醒
    YOPEN_WAKEUP_FROM_PWRKEY,			// POWERKEY唤醒
    YOPEN_WAKEUP_FROM_CHARG,			// USB充电唤醒
}yopen_wakeup_src_e;

typedef enum 
{
    YOPEN_REASON_POWERKEY = 0,			// 按键开机
    YOPEN_REASON_SWRESET,				// 软件重启
    YOPEN_REASON_PADRESET,				// reset键重启
    YOPEN_REASON_HARDFAULT,				// 异常重启
    YOPEN_REASON_ASSERT,				// ASSERT重启
    YOPEN_REASON_WDT,					// 看门狗重启
    YOPEN_REASON_FOTA,					// FOTA重启
	YOPEN_REASON_UNKNOWN                // 未知重启原因
} yopen_reset_reason_e;

/** @brief 休眠错误码*/

typedef enum
{
	YOPEN_SLEEP_SUCCESS      			= YOPEN_SUCCESS,							//函数执行成功
	YOPEN_SLEEP_INVALID_PARAM			= (YOPEN_COMPONENT_PM_SLEEP << 16) | 1000,	//参数无效
	YOPEN_SLEEP_LOCK_CREATE_FAIL		= (YOPEN_COMPONENT_PM_SLEEP << 16) | 1001, //创建唤醒锁失败
	YOPEN_SLEEP_LOCK_DELETE_FAIL		= (YOPEN_COMPONENT_PM_SLEEP << 16) | 1002, //删除唤醒锁失败
	YOPEN_SLEEP_LOCK_LOCK_FAIL		= (YOPEN_COMPONENT_PM_SLEEP << 16) | 1003, 	//锁定唤醒锁失败
	YOPEN_SLEEP_LOCK_UNLOCK_FAIL		= (YOPEN_COMPONENT_PM_SLEEP << 16) | 1004, //释放休眠锁失败
	YOPEN_SLEEP_LOCK_AUTOSLEEP_FAIL  = (YOPEN_COMPONENT_PM_SLEEP << 16) | 1005,   //启动自动休眠失败
	YOPEN_SLEEP_PARAM_SAVE_FAIL		= (YOPEN_COMPONENT_PM_SLEEP << 16) | 1006, //参数保存失败
	YOPEN_SLEEP_EXECUTE_FAIL 			= (YOPEN_COMPONENT_PM_SLEEP << 16) | 1007,	//操作失败
}yopen_errcode_sleep;
	
/** @brief 休眠深度*/	
typedef enum
{
	YOPEN_SLP_ACTIVE_STATE = 0,		   // MAX POWER 
	YOPEN_SLP_IDLE_STATE,				   // default power on is YOPEN_SLP_IDLE_STATE 
	YOPEN_SLP_SLP1_STATE,				   //nomal sleep, peripheral devices will power down
	YOPEN_SLP_SLP2_STATE,				   //
	YOPEN_SLP_HIB_STATE,				  //all ram will power dowm
	YOPEN_SLP_STATE_MAX
}yopen_sleep_depth_e;
	

/** @brief 系统运行状态*/	
typedef enum 
{
	YOPEN_SYSTEM_IS_NOT_SLEEP = 0,			//系统未处于休眠
	YOPEN_SYSTEM_IS_SLEEP,					//系统处于休眠
}yopen_system_slp_state_e;

/** @brief DTR控制休眠的模式*/ 
typedef enum 
{
	YOPEN_DTR_WAKEUP_MODE2 =0,				//DTR低电平允许进入休眠，高电平无法进入休眠
	YOPEN_DTR_WAKEUP_MODE1 =1,				//DTR高电平允许进入休眠，低电平无法进入休眠
}yopen_sleep_dtr_wakeup_mode_e;

/** @brief 可用于唤醒的Wakeup引脚*/ 
 typedef enum
 {
     YOPEN_WAKEUP_0,	//Wakup Pad0
     YOPEN_WAKEUP_1,	//Wakup Pad1
     YOPEN_WAKEUP_2,	//Wakup Pad2
     YOPEN_WAKEUP_3,	//Wakup Pad3
     YOPEN_WAKEUP_4,	//Wakup Pad4
     YOPEN_WAKEUP_5,	//Wakup Pad5
     YOPEN_WAKEUP_MAX,
 }yopen_Wakeup_id_e;

/** @brief Sleep Flag*/ 
typedef enum 
{
	YOPEN_NOT_ALLOW_SLEEP = 0,				//禁止系统进入休眠
	YOPEN_ALLOW_SLEEP,						//允许系统进入休眠
}YOPEN_SLEEP_FLAG_E;

/** @brief POWER errcode*/ 
typedef enum
{
	YOPEN_POWER_POWD_SUCCESS  = YOPEN_SUCCESS,										//关机执行成功
	YOPEN_POWER_RESET_SUCCESS = YOPEN_POWER_POWD_SUCCESS,								//重启执行成功

	YOPEN_POWER_FUNC_INIT_ERR = -1,												//函数启动失败

	YOPEN_POWER_CFW_CTRL_ERR					=  1|YOPEN_POWER_ERRCODE_BASE,			//CFW执行失败
	YOPEN_POWER_CFW_CTRL_RSP_ERR,													//CFW响应错误
	YOPEN_POWER_CFW_RESET_BUSY,													//当前不处于开机状态  

	YOPEN_POWER_SEMAPHORE_CREATE_ERR		 =	5|YOPEN_POWER_ERRCODE_BASE,			//信号量创建失败
	YOPEN_POWER_SEMAPHORE_TIMEOUT_ERR,												//信号量等待超时

	YOPEN_POWER_POWD_EXECUTE_ERR			  = 11|YOPEN_POWER_ERRCODE_BASE,			//关机执行失败
	YOPEN_POWER_POWD_INVALID_PARAM_ERR,											//关机的参数为错误值

	YOPEN_POWER_RESET_EXECUTE_ERR			   = 21|YOPEN_POWER_ERRCODE_BASE,			//重启执行失败
	YOPEN_POWER_RESET_INVALID_PARAM_ERR,											//重启的参数为错误值

	YOPEN_POWER_UP_REASON_GET_ERR			   = 31|YOPEN_POWER_ERRCODE_BASE,			//开机原因获取失败
	YOPEN_POWER_UP_REASON_MEM_NULL_ERR,											//开机原因参数为空指针

	YOPEN_POWER_KEY_CB_NULL_ERR				= 41|YOPEN_POWER_ERRCODE_BASE,			//PWRKEY按键的回调函数为空指针
	YOPEN_POWER_KEY_STATUS_GET_ERR,												//获取到错误的PWRKEY按键状态
	YOPEN_POWER_KEY_MEM_NULL_ERR,													//PWRKEY按键状态参数为空指针
	YOPEN_POWER_KEY_SHUTDOWN_TIME_SET_ERR,											//PWRKEY按键长按关机时间设置错误

	YOPEN_POWER_USB_DETECT_INVALID_PARAM	   = 51|YOPEN_POWER_ERRCODE_BASE,			//USB检查时间值参数设置为错误值
	YOPEN_POWER_USB_DETECT_SAVE_NV_ERR,											//保存设置USB检查时间值到NVRAM失败

	YOPEN_POWER_KEY_PULL_SET_ERR			   = 61|YOPEN_POWER_ERRCODE_BASE,			//PWRKEY按键上下拉设置失败
	YOPEN_POWER_KEY_PULL_INVALID_PARAM_ERR,										//PWRKEY按键上下拉参数为错误值

	YOPEN_POWER_KEY_INIT_SET_ERR			   = 71|YOPEN_POWER_ERRCODE_BASE,			//PWRKEY按键初始化设置失败
	YOPEN_POWER_KEY_INIT_INVALID_PARAM_ERR,										//PWRKEY按键初始化参数为错误值

}yopen_errcode_power;
	



/** @brief Power down mode select*/ 

typedef enum
{
	POWD_IMMDLY,		//立即关机
	POWD_NORMAL			//正常关机
}yopen_PowdMode;

/** @brief Reset mode select*/

typedef enum
{
    RESET_QUICK,		//立即重启
    RESET_NORMAL		//正常重启
}yopen_ResetMode;

/*===========================================================================
 * Struct
 ===========================================================================*/

/** @brief 唤醒锁结构体*/ 

/** @brief 关机信息结构体*/ 

/**
 * @brief  callback function before enter sleep
 * @param 
 * @return 
 */

typedef void (*yopen_enter_sleep_callback)(yopen_sleep_depth_e state);
/**
 * @brief callback function after exit sleep
 * @param 
 * @return 
 */

typedef void (*yopen_exit_sleep_callback)(yopen_sleep_depth_e state);
/**
 * @brief 该回调函数用于PWRKEY按键关机
 * @param 
 * @return 
 */


typedef void (*yopen_pwrkey_callback)(void);
/**
 * @brief 用于定义PSM回调函数
 * @param ctx			回调函数指针参数
 * @return 
 */


typedef void (*yopen_usr_deepslp_timer_cb)(void *ctx);

/**
 * @brief 该函数用于使能普通休眠
 * @param sleep_flag			休眠选择
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_autosleep_enable(YOPEN_SLEEP_FLAG_E sleep_flag);


/**
 * @brief 该函数用于注册休眠回调函数, 最多支持注册5个回调函数
 * @param cb			休眠回调函数指针
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_sleep_register_cb(yopen_enter_sleep_callback cb);


/**
 * @brief 该函数用于注册休眠唤醒回调函数，最多支持注册5个回调函数
 * @param cb			唤醒回调函数指针
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_wakeup_register_cb(yopen_exit_sleep_callback cb);

/**
 * @brief 该函数用于注销已经注册的休眠回调函数
 * @param cb			休眠回调函数指针
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_sleep_unregister_cb(yopen_enter_sleep_callback cb);


/**
 * @brief 该函数用于注销已经注册的休眠唤醒回调函数
 * @param cb			唤醒回调函数指针
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_wakeup_unregister_cb(yopen_exit_sleep_callback cb);

/**
 * @brief 该函数用于设置UART无数据进入休眠的时间
 * @param times			设置的时间。单位：秒；最小值为：1秒；最大值为：255秒
 * @return yopen_errcode_sleep
 */

/**
 * @brief 该函数用于创建唤醒锁
 * @param lock_name			唤醒锁的名称。长度不超过8个字节
 * @param name_size			名称长度
 * @return int >= 0 表示成功；< 0表示错误，参考yopen_errcode_power错误码
 */

extern int yopen_lpm_wakelock_create(char *lock_name, int name_size);


/**
 * @brief 该函数用于锁定唤醒锁
 * @param wakelock_fd			唤醒锁句柄
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_lpm_wakelock_lock(int wakelock_fd);


/**
 * @brief 该函数用于释放唤醒锁
 * @param wakelock_fd			唤醒锁句柄
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_lpm_wakelock_unlock(int wakelock_fd);


/**
 * @brief 该函数用于删除创建的唤醒锁
 * @param wakelock_fd			唤醒锁句柄
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_lpm_wakelock_delete(int wakelock_fd);


/**
 * @brief 该函数用于设置休眠深度
 * @param sleep_depth			休眠深度
 * @return yopen_errcode_sleep
 */
extern yopen_errcode_sleep yopen_set_sleep_depth(yopen_sleep_depth_e sleep_depth);

/**
 * @brief 该函数用于获取唤醒源
 * @param 			
 * @return yopen_wakeup_src_e
 */

extern yopen_wakeup_src_e yopen_get_wakeup_src(void);


/**
 * @brief 该函数获取上一次休眠的深度
 * @param 
 * @return yopen_sleep_depth_e
 */

extern yopen_sleep_depth_e yopen_get_last_slp_depth(void);

/**
 * @brief 该函数用于获取当前的系统状态
 * @param 
 * @return yopen_system_slp_state_e
 */

extern yopen_system_slp_state_e yopen_get_slp_state(void);

/**
 * @brief 该函数用于开始运行在hibernate休眠深度下的深度休眠定时器
 * @param nMs			运行的时间，单位:ms
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_deepslp_timer_start(uint32_t nMs);

/**
 * @brief 该函数用于查询深度休眠定时器是否正在运行
 * @param 	
 * @return 1:正在运行，0:不处于运行
 */

extern bool yopen_deepslp_timer_is_running();


/**
 * @brief 该函数用于删除深度休眠定时器
 * @param 
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_deepslp_timer_del();


/**
 * @brief 该函数用于注册深度休眠定时器超时函数
 * @param cb			深度休眠定时器超时回调函数指针
 * @return yopen_errcode_sleep
 */

extern yopen_errcode_sleep yopen_deepslp_timer_register_cb(yopen_usr_deepslp_timer_cb cb);


/**
 * @brief 该函数用于注册PSM回调函数。注册的回调函数会在进入休眠前执行。
 * @param cb			 回调函数指针
 * @param ctx			 回调函数指针参数
 * @return yopen_errcode_sleep
 */
#if 0
extern yopen_errcode_sleep yopen_psm_register_enter_cb(yopen_psm_enter_callback cb,void *ctx);


/**
* @brief 该函数用于控制UE省电模式（PSM）参数

* @param :
*	nSim				[in] sim index range: 0-1
*   psm_enable  		[in] Indication to disable or enable the use of PSM in the UE
*	periodic_TAU_time	[in] string type; one byte in an 8 bit format. Requested extended periodic TAU value (T3412) to be allocated to the UE in E-UTRAN. 
*							 The requested extended periodic TAU value is coded as one byte (octet 3) of the GPRS Timer 3 information element coded as bit 
*							 format (e.g. "01000111" equals 70 hours). For the coding and the value range, see the GPRS Timer 3 IE in 3GPP TS 24.008 [8] 
*							 Table 10.5.163a/3GPP TS 24.008. See also 3GPP TS 23.682 [149] and 3GPP TS 23.401 [82]. The default value, if available, is manufacturer specific
*						
*		GPRS Timer 3 value
*		Bits 5 to 1 represent the binary coded timer value
*		Bits 6 to 8 defines the timer value uint for the GPRS timer as follows
*		Bits
*		8	7	6
*		-----------------------------------------------------------
*		0	0	0	value is incremented in multiples of 10 minutes
*		0	0	1	value is incremented in multiples of 1 hour
*		0	1	0	value is incremented in multiples of 10 hours
*		0	1	1	value is incremented in multiples of 2 seconds
*		1	0	0	value is incremented in multiples of 30 seconds
*		1	0	1	value is incremented in multiples of 1 minute
*		1	1	0	value is incremented in multiples of 320 hours
*		1	1	1	value indicates that the timer is deactivated
*
*
*	active_time			[in] string type; one byte in an 8 bit format. Requested Active Time value (T3324) to be allocated to the UE. The requested Active Time 
*							 value is coded as one byte (octet 3) of the GPRS Timer 2 information element coded as bit format (e.g. "00100100" equals 4 minutes). 
*							 For the coding and the value range, see the GPRS Timer 2 IE in 3GPP TS 24.008 [8] Table 10.5.163/3GPP TS 24.008. See also 3GPP TS 23.682 [149], 
*							 3GPP TS 23.060 [47] and 3GPP TS 23.401 [82]. The default value, if available, is manufacturer specific.
*
*		GPRS Timer 2 value
*		Bits 5 to 1 represent the binary coded timer value
*		Bits 6 to 8 defines the timer value uint for the GPRS timer as follows
*		Bits
*		8	7	6
*		-----------------------------------------------------------
*		0	0	0	value is incremented in multiples of 2 seconds
*		0	0	1	value is incremented in multiples of 1 minute
*		0	1	0	value is incremented in multiples of decihours
*		1	1	1	value indicates that the timer is deactivated
*
* @return yopen_errcode_sleep
*/
extern yopen_errcode_sleep yopen_psm_sleep_enable(uint8_t nSim, BOOL psm_enable, const char *periodic_TAU_time, const char *active_time);



/**
* @brief 该函数用于获取网络侧下发的PSM相关定时器的参数
* 
* @param	yopen_t3412			[in]	网络侧下发的T3412定时器
* @param	yopen_t3324 			[in]	网络侧下发的T3324定时器
* @param	yopen_ext3412			[in]	网络侧下发的ex_T3412定时器
*
* @return yopen_errcode_sleep
*/

extern yopen_errcode_sleep yopen_get_psm_result(UINT32 *yopen_t3412,UINT32 *yopen_t3324,UINT32 *yopen_ext3412);

#endif

/*
	yopen_wait_wakeup_source/yopen_send_wakeup_event API暂不支持
*/

/**
* @brief 该函数用于开启休眠并指定时长自己醒来返回唤醒源
* 
* @param	timeout_ms			[in]	定时的时间,单位：毫秒
*
* @return 0-pad唤醒,1-pwkey唤醒,2-MQTT唤醒,3-USB插入唤醒,4-UART数据唤醒，0xff-定时器超时唤醒
*/
// extern uint32 yopen_wait_wakeup_source(uint32_t timeout_ms);
// typedef uint32 (*_unuseapi_yopen_wait_wakeup_source_t)(uint32_t timeout_ms);

/**
* @brief 仅供内部使用 发送唤醒源事件
* 
*/
// extern yopen_errcode_sleep yopen_send_wakeup_event(uint32_t event);
// typedef yopen_errcode_sleep (*_unuseapi_yopen_send_wakeup_event_t)(uint32_t event);

/**
 * @brief 该函数用于模块关机
 * @param powd_mode				关机方式
 * @return yopen_errcode_power
 */

extern yopen_errcode_power yopen_power_down
(
	yopen_PowdMode powd_mode
);




/**
 * @brief 该函数用于重启模块
 * @param reset_mode				重启方式
 * @return yopen_errcode_power
 */

extern yopen_errcode_power yopen_power_reset
(
	yopen_ResetMode reset_mode
);


/**
 * @brief 该函数用于获取模块的PWRKEY按键状态
 * @param level	PWRKEY 电平
 * @return yopen_errcode_power
 */

yopen_errcode_power yopen_get_pwrkey_level
(
	uint8_t *level
);



/**
 * @brief 该函数用于PWRKEY初始化
 * @param pwrkey_cb	回调函数
 * @return yopen_errcode_power
 */
yopen_errcode_power yopen_pwrkey_init
(
	yopen_pwrkey_callback pwrkey_cb
);

/**
 * @brief 该函数用于PWRKEY去初始化
 * @param
 * @return yopen_errcode_power
 */
yopen_errcode_power yopen_pwrkey_deinit
(
	void
);

/**
 * @brief 该函数用于获取开机原因
 * @param pwrup_reason 			开机原因
 * @return yopen_errcode_power
 */

extern yopen_errcode_power yopen_get_powerup_reason
(
	yopen_reset_reason_e *reason
);

/** @}*/


#ifdef __cplusplus
} /*"C" */
#endif

#endif
