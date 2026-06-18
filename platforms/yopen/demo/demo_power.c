/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/
#include "yopen_os.h"
#include "yopen_type.h"
#include "yopen_power.h"
#include "yopen_gpio.h"
#include "yopen_nw.h"
#include "yopen_dev.h"
#include "yopen_debug.h"
#include "yopen_usb.h"
#include "yopen_sim.h"
#include <stdio.h>
#include <string.h>

#define DEMO_POWER_TRACE(fmt, ...) yopen_trace("[POWER]"fmt, ##__VA_ARGS__)

#define POWER_TEST_SLEEP_LEVEL  	YOPEN_SLP_SLP1_STATE				   //nomal sleep, peripheral devices will power down

#define POWER_TEST_WAKELOCK (0)
#define POWER_TEST_POWERKEY_WAKEUP (0)

#define POWER_TEST_AONGPIO (0)
#define POWER_TEST_GPIO    (0)
#define POWER_TEST_POWEROFF (0)

#define POWER_TEST_SLEEP_TIME (5*60*1000)
#define POWER_TEST_WAKE_TIME  (10*1000)

//#define POWER_REASON_DEBUG
#define POWER_REASON_CASE (0)

#if POWER_TEST_WAKELOCK
static uint8 wakelock_fd;
#endif

void yopen_pre_sleep_process(yopen_sleep_depth_e state)
{
	DEMO_POWER_TRACE("==========enter sleep success state %d==========", state);
	/*
	action before enter sleep 
	*/
}

void yopen_post_sleep_process(yopen_sleep_depth_e state)
{
	DEMO_POWER_TRACE("==========exit sleep src %d, state %d==========", yopen_get_wakeup_src(), state);
	/*
	 exit sleep action
	*/
}
#if POWER_TEST_POWERKEY_WAKEUP
//中断回调
static void pwrkey_demo_callback(void)
{
	yopen_errcode_e ret = YOPEN_SUCCESS;

	yopen_wakeup_src_e src = yopen_get_wakeup_src();

	DEMO_POWER_TRACE("yopen_lpm_wakelock_lock ret  %d src %d", ret, src);
	
}
#endif

#if POWER_TEST_POWERKEY_WAKEUP
static void yopen_deepslp_timer_cb(void *ctx)
{
	DEMO_POWER_TRACE("==========yopen_deepslp_timer_cb ==========");
}
#endif

#if POWER_TEST_AONGPIO
static void power_aongpio_output_high(void)
{
	// PIN 25 GPIO12
	yopen_aon_power_on();
	yopen_pin_set_func(YOPEN_PIN_GPIO12, 0);
	yopen_gpio_init(GPIO_12, GPIO_OUTPUT,FORCE_PULL_NONE, LVL_HIGH);
	yopen_gpio_set_level(GPIO_12, LVL_HIGH);
}
#endif
#if POWER_TEST_GPIO
static void power_gpio_output_high(void)
{
	// PIN 67 GPIO17
	yopen_pin_set_func(YOPEN_PIN_GPIO17, 4);
	yopen_gpio_init(GPIO_17, GPIO_OUTPUT,FORCE_PULL_NONE, LVL_HIGH);
	yopen_gpio_set_level(GPIO_17, LVL_HIGH);
}
#endif

#ifdef POWER_REASON_DEBUG
static void power_reason_test(void)
{
	yopen_reset_reason_e reason;
	yopen_errcode_power ret = yopen_get_powerup_reason(&reason);
	uint32_t reason_case = POWER_REASON_CASE;
	
	yopen_rtos_task_sleep_ms(1000);

	DEMO_POWER_TRACE("power_reason_test ret %d, reason %d, reason_case %d", ret, reason, reason_case);
	yopen_rtos_task_sleep_ms(1000);

	//测试不同方式导致的重启
	//reason_case = 0 时可以测试按键开机和reset键重启(YOPEN_REASON_POWERKEY YOPEN_REASON_PADRESET)

	//YOPEN_REASON_HARDFAULT
	if (reason_case == 1)
	{
		int *p=0;
		*p = 100;
	}
	//YOPEN_REASON_ASSERT
	else if (reason_case == 2)
	{
		assert(0);
	}
	//YOPEN_REASON_WDT
	else if (reason_case == 3)
	{
		while(1);
	}
	//YOPEN_REASON_SWRESET
	else if (reason_case == 4)
	{
		yopen_power_reset(RESET_QUICK);
	}
	//YOPEN_REASON_FOTA
	else if (reason_case == 5)
	{
		yopen_fota_power_reset();
	}
}
#endif

extern void slpManAonWdtStop(void);

void yopen_power_demo_thread(void *arvg)
{
	const yopen_sleep_depth_e sleep_depth = POWER_TEST_SLEEP_LEVEL;

	uint8_t try_count = 0;

	yopen_nw_reg_status_info_s nw_status;

	uint32_t port = 0;
	yopen_sim_status_e         sim_status;
	uint8_t                    cfun;

#ifdef POWER_REASON_DEBUG
	power_reason_test();
#endif

	yopen_debug_cfg_wdt(0);

#if POWER_TEST_AONGPIO
    power_aongpio_output_high();
#endif
#if POWER_TEST_GPIO
	power_gpio_output_high();
#endif	
	

	yopen_log_port_get(&port);
	if(port != 1)
	{
		yopen_log_port_set(1);
		yopen_power_reset(RESET_QUICK);
		return;
	}

	yopen_sim_set_present_det(0, 0);

	yopen_usb_disable();

	do{
		yopen_nw_get_reg_status(0, &nw_status);
		yopen_sim_get_card_status(0, &sim_status);

		yopen_trace("==========POWER demo network status %d sim status %d==========", nw_status.data_reg.state, sim_status);

		if(nw_status.data_reg.state == YOPEN_NW_REG_STATE_DENIED || sim_status != YOPEN_SIM_STATUS_READY)
		{
			if(try_count < 5)
			{
				try_count++;
			}
			else
			{
				yopen_trace("network reg denied set cfun 0");
				yopen_dev_set_modem_fun(YOPEN_DEV_CFUN_MIN, FALSE, 0);
				break;
			}
		}
		else
		{
			if(try_count < 60)
			{
				try_count++;
			}
			else
			{
				break;
			}
		}
		yopen_rtos_task_sleep_ms(1000);
		
	}while(!(nw_status.data_reg.state == YOPEN_NW_REG_STATE_HOME_NETWORK || nw_status.data_reg.state == YOPEN_NW_REG_STATE_ROAMING));

	DEMO_POWER_TRACE("========== POWER demo start ==========");

#if POWER_TEST_POWEROFF
	yopen_power_down(POWD_NORMAL);
#endif	

	yopen_dev_get_modem_fun(&cfun, 0);

	if((nw_status.data_reg.state == YOPEN_NW_REG_STATE_HOME_NETWORK || nw_status.data_reg.state == YOPEN_NW_REG_STATE_ROAMING))
	{
		yopen_nw_phy_status_info_s      phy_status_info;

		DEMO_POWER_TRACE("========== POWER demo in NET REG MODE ==========");

		yopen_nw_get_phy_status_info(0, &phy_status_info);
		yopen_trace("cell band:%d, pci:%d,rsrp:%d rsrq %d, rssi %d, snr %d",
						phy_status_info.band,phy_status_info.pci,phy_status_info.rsrp,
						phy_status_info.rsrq, phy_status_info.rssi, phy_status_info.snr);
	}
	else if(cfun == 0)
	{
		DEMO_POWER_TRACE("========== POWER demo in CFUN=0 MODE ==========");
	}
	else
	{
		DEMO_POWER_TRACE("========== POWER demo in NET UNREG MODE ==========");
	}

#if POWER_TEST_WAKELOCK
	wakelock_fd = yopen_lpm_wakelock_create("lock",strlen("lock"));
#endif

	yopen_set_sleep_depth(sleep_depth);

	if(sleep_depth == YOPEN_SLP_SLP2_STATE)
	{
		yopen_deepslp_timer_start(POWER_TEST_SLEEP_TIME+POWER_TEST_WAKE_TIME);
	}
	
#if POWER_TEST_POWERKEY_WAKEUP
	{
		DEMO_POWER_TRACE("========== POWERKEY wakeup ==========");
		yopen_deepslp_timer_register_cb(yopen_deepslp_timer_cb);
		//通过按键唤醒模块
		yopen_pwrkey_init(pwrkey_demo_callback);
	}
#endif

	yopen_sleep_register_cb(yopen_pre_sleep_process);
	yopen_wakeup_register_cb(yopen_post_sleep_process);
#if POWER_TEST_WAKELOCK
	yopen_autosleep_enable(YOPEN_ALLOW_SLEEP);
	while(1)
	{
		yopen_lpm_wakelock_lock(wakelock_fd);
		DEMO_POWER_TRACE("========== WAKE %dS ==========", POWER_TEST_WAKE_TIME/1000);
		yopen_rtos_task_sleep_ms(POWER_TEST_WAKE_TIME);

		yopen_lpm_wakelock_unlock(wakelock_fd);
		DEMO_POWER_TRACE("========== SLEEP %dS ==========", POWER_TEST_SLEEP_TIME/1000);
		DEMO_POWER_TRACE("yopen_get_slp_state  %d ", yopen_get_slp_state());
		yopen_rtos_task_sleep_ms(POWER_TEST_SLEEP_TIME);

	}
#else
	while (1)
	{
		yopen_autosleep_enable(YOPEN_NOT_ALLOW_SLEEP);
		DEMO_POWER_TRACE("========== WAKE %dS ==========", POWER_TEST_WAKE_TIME/1000);
		yopen_rtos_task_sleep_ms(POWER_TEST_WAKE_TIME);

		DEMO_POWER_TRACE("========== SLEEP %dS ==========", POWER_TEST_SLEEP_TIME/1000);
		yopen_autosleep_enable(YOPEN_ALLOW_SLEEP);
		yopen_rtos_task_sleep_ms(POWER_TEST_SLEEP_TIME);
	}
	
#endif
}
