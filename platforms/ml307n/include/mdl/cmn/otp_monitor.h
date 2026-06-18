/**
 *************************************************************************************

*
 *
 * @file        otp_monitor.h
 *
 * @brief       芯片过温监测
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     cat1 team          创建
 ************************************************************************************
 */

#ifndef __OTP_MONITOR_H__
#define __OTP_MONITOR_H__


int OTP_SetOtpThreshold(uint16_t threshold);
int OTP_GetOtpThreshold(uint16_t *threshold);

#endif

