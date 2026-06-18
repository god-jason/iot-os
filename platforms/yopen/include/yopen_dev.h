


#ifndef _YOPEN_DEV_H_
#define _YOPEN_DEV_H_

#ifdef __cplusplus   // this area code will compile with c program
extern "C" {
#endif

#include "yopen_api_common.h"
#include "yopen_type.h"

/**
 * @defgroup yopen_dev 设备
 * @{
*/

/**
 * @brief 设备产品模式
 * 
 */
typedef enum 
{
    YOPEN_ENABLE=0,
    YOPEN_DISABLE=1,
}ycom_prod_state_e;

/**
 * @brief 设备功能模式
 * 
 */
typedef enum
{
    YOPEN_DEV_CFUN_MIN  = 0,
    YOPEN_DEV_CFUN_FULL = 1,
    YOPEN_DEV_CFUN_AIR  = 4,
} yopen_dev_cfun_e;

/**
 * @brief 回调函数
 * 
 * @param ctx 回调参数
 * @param cnf yopen_modem_general_cnf_t指针
 */
typedef void (*dev_callback_ptr)(void *ctx, void *cnf); 

/**
 * @brief 获取设备imei号
 * 
 * @param p_imei 设备imei号
 * @param imei_len 设备imei号字符串长度
 * @param nSim 选择sim卡
 * @return ** 错误码，参考yopen_error.h文件  
 */
extern int yopen_dev_get_imei(char *p_imei,size_t imei_len,uint8_t nSim);

/**
 * @brief 设置设备imei号
 * 
 * @param p_imei 设备imei号
 * @param imei_len 设备imei号字符串长度，固定值15
 * @param nSim 选择sim卡
 * @return ** 错误码，参考yopen_error.h文件  
 */
extern int yopen_dev_set_imei(char *p_imei,size_t imei_len,uint8_t nSim);


/**
 * @brief 获取设备固件版本
 * 
 * @param p_version 设备固件版本
 * @param version_len 设备固件版本字符串长度
 * @return ** 错误码，参考yopen_error.h文件  
 */
extern int yopen_dev_get_firmware_version(char *p_version,size_t version_len);

/**
 * @brief 设置版本信息
 * 
 * @param major_version 主版本号字符串 (注:底层core的版本信息, 传NULL使用默认版本号，可通过ATI指令查询)
 * @param version_len 次版本号字符串 (注:应用层版本信息, 设置后，可通过ATI指令查询)
 */
extern void yopen_dev_set_firmware_version(char *major_version, char *minor_version);

/**
 * @brief 获取设备sn号
 * 
 * @param p_sn 设备sn号
 * @param sn_len 设备sn号字符串长度，最小值33
 * @param nSim 选择sim卡
 * @return ** 错误码，参考yopen_error.h文件  
 */
extern int yopen_dev_get_sn(char *p_sn,size_t sn_len,uint8_t nSim);

/**
 * @brief 设置设备sn号
 * 
 * @param p_sn 设备sn号
 * @param sn_len 设备sn号字符串长度，最大值32
 * @param nSim 选择sim卡
 * @return ** 错误码，参考yopen_error.h文件  
 */
extern int yopen_dev_set_sn(char *p_sn,size_t sn_len,uint8_t nSim);

/**
 * @brief 设置设备功能模式
 * 
 * @param fun 设备功能模式
 * @param rst 是否重启
 * @param nSim 选择sim卡
 * @return ** 错误码，参考yopen_error.h文件  
 */
extern int yopen_dev_set_modem_fun(yopen_dev_cfun_e fun, bool rst, uint8_t nSim);
 

/**
 * @brief 获取设备功能模式
 * 
 * @param p_function 当前设备功能模式
 * @param nSim 选择sim卡
 * @return ** 错误码，参考yopen_error.h文件  
 */
extern int yopen_dev_get_modem_fun(uint8_t *p_fun, uint8_t nSim);

/**
 * @brief 异步设置设备功能模式
 * 
 * @param fun 设备功能模式
 * @param rst 是否重启, 暂不支持，直接传入FALSE
 * @param nSim 选择sim卡
 * @param cb 回调函数
 * @param ctx 回调函数上下文
 * @return ** 错误码，参考yopen_error.h文件  
 */
extern int yopen_dev_set_modem_fun_async(yopen_dev_cfun_e fun, bool rst, uint8_t nSim, dev_callback_ptr cb, void *ctx);
/** @}*/

#ifdef __cplusplus
}
#endif


#endif





