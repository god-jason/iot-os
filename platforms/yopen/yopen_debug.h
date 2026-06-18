


#ifndef _YOPEN_DEBUG_H_
#define _YOPEN_DEBUG_H_

#ifdef __cplusplus   // this area code will compile with c program
extern "C" {
#endif

#include "yopen_api_common.h"
#include "yopen_type.h"

#ifndef YOPEN_OPEN_MODE_SUPPORT
/**
 * @defgroup yopen_debug 调试
 * @{
*/

/**
 * @brief log日志功能枚举
 * 
 */
typedef enum
{
    YOPEN_DEV_LOG_ID_HW = 0,			///< HW,硬件日志
    YOPEN_DEV_LOG_ID_PHY_ONLINE  = 1,	///< PHY_ONLINE,物理层在线日志
    YOPEN_DEV_LOG_ID_PHY_OFFLINE,		///< PHY_OFFLINE,物理层非在线日志
    YOPEN_DEV_LOG_ID_PLAT_AP,			///< PLAT_AP,AP核心平台日志
    YOPEN_DEV_LOG_ID_PLAT_CP,			///< PLAT_CP,CP核心平台日志
    YOPEN_DEV_LOG_ID_PS1,				///< PS1,分组交换域日志
    YOPEN_DEV_LOG_ID_PS2,				///< PS2,保留
    YOPEN_DEV_LOG_ID_APP,               ///< APP, 包含yopen_trace日志
} yopen_debug_log_owner_id;				

/**
 * @brief log日志等级枚举
 * 
 */
typedef enum
{
	YOPEN_DEV_LOG_LEVEL_DEBUG =0,	///< DEBUG及以上等级日志
	YOPEN_DEV_LOG_LEVEL_INFO,		///< INFO及以上等级日志
	YOPEN_DEV_LOG_LEVEL_VALUE,		///< VALUE及以上等级日志
	YOPEN_DEV_LOG_LEVEL_SIG,		///< SIG及以上等级日志
	YOPEN_DEV_LOG_LEVEL_WARNING,	///< WARNING及以上等级日志
	YOPEN_DEV_LOG_LEVEL_ERROR,		///< ERROR等级日志
} yopen_debug_log_owner_level;

typedef enum
{
    YOPEN_EXCEP_OPTION_DUMP_FLASH_EPAT_LOOP,                    ///< 0 -- dump full exception info to flash and EPAT tool then trapped in endless loop(while(1))*/
    YOPEN_EXCEP_OPTION_PRINT_RESET,                             ///< print necessary exception info, and then reset*/
    YOPEN_EXCEP_OPTION_DUMP_FLASH_RESET,                        ///< dump full exception info to flash, and then reset*/
    YOPEN_EXCEP_OPTION_DUMP_FLASH_EPAT_RESET,                   ///< dump full exception info to flash and EPAT tool, and then reset*/
    YOPEN_EXCEP_OPTION_SILENT_RESET,                            ///< reset directly*/

    YOPEN_EXCEP_OPTION_MAX

}yopen_debug_fault_action;

#define YOPEN_DEBUG_FAULT_OPT_BASE_INFO 0x01  //输出基本信息, 开机原因值和HEAP信息
#define YOPEN_DEBUG_FAULT_OPT_TASK_INFO 0x02  //输出task信息和堆栈情况
#define YOPEN_DEBUG_FAULT_OPT_SLEEP_INFO 0x04  //输出sleep投票情况

typedef union
{
    uint32_t param;
    struct
    {
        uint32_t action:       8;  //yopen_debug_fault_action 兼容R3.2之前的版本设置
        uint32_t opt:          8;  //定时输出调试信息, 0: 不输出 YOPEN_DEBUG_FAULT_OPT_BASE_INFO|YOPEN_DEBUG_FAULT_OPT_TASK_INFO|YOPEN_DEBUG_FAULT_OPT_SLEEP_INFO
        uint32_t period:      16; //周期,单位毫秒
    } field;
} yopen_debug_fault_action_ext;

typedef struct 
{
    size_t total_size;             ///< heap总大小，单位字节
    size_t curr_free_size;         ///< 当前剩余大小，单位字节
    size_t min_free_size;          ///< 最小剩余大小，单位字节
}yopen_debug_heap_info;


/**
 * @brief 输出EPAT Log
 * @param fmt 需要打印的Log内容
 * @return 错误码，参考yopen_error.h文件， YOPEN_RET_OK表示成功，其他表示失败
 */
extern int yopen_trace(const char *fmt, ...);

/**
 * @brief 配置看门狗
 * 
 * @param timeout 看门狗超时时长，单位s，数值范围：20-256，0表示关闭看门狗
 * @return 错误码，参考yopen_error.h文件， YOPEN_RET_OK表示成功，其他表示失败
 */
extern int yopen_debug_cfg_wdt(uint32_t timeout);


/**
 * @brief 喂看门狗
 *  
 */
extern void yopen_debug_feed_wdt(void);


/**
 * @brief 设置log输出的范围及等级
 * @param logOwnerId 	log范围
 * @param logOwnerLevel log等级
 * @return 错误码，参考yopen_error.h文件， YOPEN_RET_OK表示成功，其他表示失败
 */
extern int yopen_debug_set_log_level(yopen_debug_log_owner_id logOwnerId, yopen_debug_log_owner_level logOwnerLevel);


/**
 * @brief 查询log输出的范围及等级
 * @param 
 * @return log范围和等级
            取值范围是 0~0xFFFFFF
            默认值是0, 
            bit0-2表示 YOPEN_DEV_LOG_ID_HW 的 log level, 
            bit3-5表示 YOPEN_DEV_LOG_ID_PHY_ONLINE 的 log level
            其他以此类推。
            例如： 配置了 YOPEN_DEV_LOG_ID_PHY_ONLINE 的 log level为 YOPEN_DEV_LOG_LEVEL_INFO(1)，
                     “logOwnerAndLevelOld” 的值应该是 0x000008 
 */
extern uint32_t yopen_debug_get_log_level(void);


/**
 * @brief 设置CPU异常后，执行动作
 * @param action：异常动作：
 *                  调试版本：建议设置为：YOPEN_EXCEP_OPTION_DUMP_FLASH_EPAT_LOOP；
 *                  正常版本：建议设置为：YOPEN_EXCEP_OPTION_SILENT_RESET
 * @note  可以使用扩展参数yopen_debug_fault_action_ext 用来额外的输出调试信息           
 *          例如：
 *                  yopen_debug_fault_action_ext ext;
 *                  ext.field.action = YOPEN_EXCEP_OPTION_DUMP_FLASH_EPAT_LOOP; 兼容R3.2之前的版本设置
 *                  ext.field.opt = YOPEN_DEBUG_FAULT_OPT_BASE_INFO|YOPEN_DEBUG_FAULT_OPT_TASK_INFO|YOPEN_DEBUG_FAULT_OPT_SLEEP_INFO; 
 *                  ext.field.period = 1000; // 1000ms输出一次    
 *                  yopen_debug_set_fault_action(ext.param);          
 * @return 错误码，参考yopen_error.h文件， YOPEN_RET_OK表示成功，其他表示失败
 */
extern int yopen_debug_set_fault_action(uint32_t action);

/**
 * @brief 获取CPU异常后，执行动作
 *                        
 * @return ** uint32_t // yopen_debug_fault_action_ext
 */
extern uint32_t yopen_debug_get_fault_action(void);

/**
 * @brief 获取堆使用大小
 * @param[out] info: 堆使用情况                       
 * @return **
 */
extern void yopen_debug_get_heap_size(yopen_debug_heap_info* info);

/**
 * @brief 异常断言
 * @param[in] c: 断言条件，FALSE 产生断言异常                  
 * @return **
 */
#define YOPEN_ASSERT(c)  yopen_assert(c, __FUNCTION__, __LINE__)
extern void yopen_assert(bool condition, const char* func, uint32_t line);

/** @}*/


#endif

#ifdef __cplusplus
}
#endif


#endif
