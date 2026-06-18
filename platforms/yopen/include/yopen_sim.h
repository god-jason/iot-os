
#ifndef _YOPEN_SIM_H_
#define _YOPEN_SIM_H_

#ifdef __cplusplus   // this area code will compile with c program
extern "C" {
#endif
#include "yopen_api_common.h"
#include "yopen_type.h"
#include "yopen_os.h"
#include "yopen_gpio.h"

/**
 * @defgroup yopen_sim 移动网络接口
 * @{
 */

/*========================================================================
 *  Marco Definition
 *========================================================================*/
#define YOPEN_SIM_ERRCODE_BASE (YOPEN_COMPONENT_SIM<<16)

#define YOPEN_SIM_PIN_LEN_MAX  8          // Maximum length of PIN data. 

#define SIM_1	0
#define	SIM_2 	1

/** SIM card function block initialization status */
#define YOPEN_SIMINI_STAT_START             (0x00) /*!< SIM card initialization start status */
#define YOPEN_SIMINI_STAT_CPIN_READY        (0x01) /*!< SIM card PIN code ready status */
#define YOPEN_SIMINI_STAT_SMS_DONE          (0x02) /*!< SMS function initialization completion status */
#define YOPEN_SIMINI_STAT_PB_DONE           (0x04) /*!< Phone book function initialization completion status */

/** Maximum length of IMSI, although specification documents define it as 15 digits, considering potential risks, here it is defined as 16 digits based on the size of SIM card files */
#define YOPEN_SIM_IMSI_LEN_MAX              (16)
/** the length of iccid */
#define YOPEN_SIM_ICCID_LEN                 (20)
/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
 
/**@brief SIM卡错误码枚举*/
typedef enum
{
    YOPEN_SIM_SUCCESS                          = 0,                      ///< 函数执行成功
    YOPEN_SIM_EXECUTE_ERR                      = 1|YOPEN_SIM_ERRCODE_BASE,  ///< 函数执行失败 
    YOPEN_SIM_MEM_ADDR_NULL_ERR,                                         ///< 函数的参数地址为NULL
    YOPEN_SIM_INVALID_PARAM_ERR,                                         ///< 函数的参数无效
    YOPEN_SIM_NO_MEMORY_ERR,                                             ///< 内存申请失败
    YOPEN_SIM_SEMAPHORE_CREATE_ERR             = 5|YOPEN_SIM_ERRCODE_BASE,  ///< 创建信号量失败
    YOPEN_SIM_SEMAPHORE_TIMEOUT_ERR,                                     ///< 等待信号量超时
    YOPEN_SIM_NOT_INSERTED_ERR,                                          ///< (U)SIM卡未插入
    YOPEN_SIM_CFW_IMSI_GET_REQUEST_ERR,                                  ///< 请求(U)SIM卡的IMSI号被拒
    YOPEN_SIM_CFW_IMSI_GET_RSP_ERR,                                      ///< 获取(U)SIM卡的IMSI号失败
    YOPEN_SIM_CFW_ICCID_GET_REQUEST_ERR        = 10|YOPEN_SIM_ERRCODE_BASE, ///< 请求(U)SIM卡的ICCID号被拒
    YOPEN_SIM_CFW_PHONE_NUM_GET_REQUEST_ERR,                             ///< 请求(U)SIM卡的本机号码被拒
    YOPEN_SIM_CFW_PHONE_NUM_GET_RSP_NULL_ERR,                            ///< (U)SIM卡本机号码为空
    YOPEN_SIM_CFW_STATUS_GET_REQUEST_ERR,                                ///< 请求(U)SIM卡状态被拒
    YOPEN_SIM_CFW_PIN_ENABLE_REQUEST_ERR,                                ///< 启动(U)SIM卡PIN码验证请求失败
    YOPEN_SIM_CFW_PIN_DISABLE_REQUEST_ERR      = 15|YOPEN_SIM_ERRCODE_BASE, ///< 关闭(U)SIM卡PIN码验证请求失败
    YOPEN_SIM_CFW_PIN_VERIFY_REQUEST_ERR,                                ///< 请求PIN码验证失败
    YOPEN_SIM_CFW_PIN_CHANGE_REQUEST_ERR,                                ///< 更改(U)SIM卡PIN码请求失败
    YOPEN_SIM_CFW_PIN_UNBLOCK_REQUEST_ERR,                               ///< 请求PUK码解锁失败
    YOPEN_SIM_OPERATION_NOT_ALLOWED_ERR,                                 ///< 当前操作不被允许
    YOPEN_PBK_NOT_EXIT_ERR                     = 100|YOPEN_SIM_ERRCODE_BASE,///< 指定的电话簿不存在
    YOPEN_PBK_NOT_INIT_ERR,                                              ///< 电话簿未初始化完成
    YOPEN_PBK_ITEM_NOT_FOUND_ERR,                                        ///< 未找到电话号码记录
} yopen_sim_errcode_e;

typedef struct
{	
	uint8 msgId;
	UINT8 level;
}YOPEN_Gpio_Manage_Msg;
/**@brief (U)SIM卡状态类型枚举*/
typedef enum 
{
    YOPEN_SIM_STATUS_READY                 = 0,    ///< (U)SIM卡已就绪
    YOPEN_SIM_STATUS_PIN1_READY,                   ///< PIN1码验证通过
    YOPEN_SIM_STATUS_SIMPIN,                       ///< 等待验证(U)SIM卡的PIN码
    YOPEN_SIM_STATUS_SIMPUK,                       ///< 等待验证(U)SIM卡的PUK码
    YOPEN_SIM_STATUS_PHONE_TO_SIMPIN,              ///< (U)SIM卡个性化检查失败导致(U)SIM卡被锁定，等待PCK码去激活(U)SIM卡个性化
    YOPEN_SIM_STATUS_PHONE_TO_FIRST_SIMPIN = 5,    ///< 待验证隐藏电话簿条目的密码
    YOPEN_SIM_STATUS_PHONE_TO_FIRST_SIMPUK,        ///< 待验证隐藏电话簿条目的解锁码
    YOPEN_SIM_STATUS_SIMPIN2,                      ///< 等待验证(U)SIM卡的PIN2
    YOPEN_SIM_STATUS_SIMPUK2,                      ///< 等待验证(U)SIM卡的PUK2
    YOPEN_SIM_STATUS_NETWORKPIN,                   ///< 网络个性化检查失败导致(U)SIM卡被锁定，等待NCK码去激活网络个性化
    YOPEN_SIM_STATUS_NETWORKPUK            = 10,   ///< 错误的NCK码导致(U)SIM卡被锁定，需要MEP解锁密码
    YOPEN_SIM_STATUS_NETWORK_SUBSETPIN,            ///< 网络子集个性化检查失败导致(U)SIM卡被锁定，等待NSCK码去激活网络子集个性化
    YOPEN_SIM_STATUS_NETWORK_SUBSETPUK,            ///< 错误的NSCK码导致(U)SIM卡被锁定，需要MEP解锁密码
    YOPEN_SIM_STATUS_PROVIDERPIN,                  ///< 服务提供商个性化检查失败导致(U)SIM 卡被锁定，等待SPCK码去激活服务提供商个性化
    YOPEN_SIM_STATUS_PROVIDERPUK,                  ///< 错误的SPCK码导致(U)SIM卡被锁定，需要MEP解锁密码
    YOPEN_SIM_STATUS_CORPORATEPIN          = 15,   ///< 公司个性化检查失败导致(U)SIM卡被锁定，等待CCK码去激活公司个性化
    YOPEN_SIM_STATUS_CORPORATEPUK,                 ///< 错误的CCK码导致(U)SIM卡被锁定，需要MEP解锁密码
    YOPEN_SIM_STATUS_NOSIM,                        ///< (U)SIM卡未插入
    YOPEN_SIM_STATUS_PIN1BLOCK,                    ///< 不支持
    YOPEN_SIM_STATUS_PIN2BLOCK,                    ///< 不支持
    YOPEN_SIM_STATUS_PIN1_DISABLE          = 20,   ///< 不支持
    YOPEN_SIM_STATUS_SIM_PRESENT,                  ///< 不支持
    YOPEN_SIM_STATUS_UNKNOW,                       ///< 未知状态
}yopen_sim_status_e;

/**@brief 热插拔检测功能状态枚举*/
typedef enum{
    YOPEN_SIM_HOTPLUG_DISABLE          = 0,        ///< 禁用
    YOPEN_SIM_HOTPLUG_ENABLE           = 1,        ///< 启用
}yopen_sim_hotplug_e;

/**@brief 热插拔检测引脚状态枚举*/
typedef enum{
    YOPEN_SIM_HOTPLUG_STATUS_OUT       = 0,        ///< (U)SIM卡拔出
    YOPEN_SIM_HOTPLUG_STATUS_IN        = 1,        ///< (U)SIM卡插入
}yopen_sim_hotplug_status_e;

/**@brief 热插拔检测引脚配置结构体*/
typedef struct
{
    yopen_GpioNum gpio_num;                        ///< GPIO引脚编号
    yopen_LvlMode gpio_level;                      ///< 引脚电平，仅对输出引脚有效
}yopen_sim_hotplug_gpio_s;

typedef struct
{
    yopen_GpioNum gpio_num;
    yopen_LvlMode gpio_level;
    yopen_sim_hotplug_e cur_status;
    bool isInit;
}yopen_sim_hotplug_s;

typedef struct
{
    uint8 usim;
    uint8 sim_det_enable;
    uint8 sim_insert_level;
}yopen_usim_det_stat_s;

typedef struct
{
	bool usim_int_first;
	yopen_LvlMode last_level;
}ycom_usim_detect_state_s;

typedef void (*yopen_sim_hotplug_cb_t)(uint8_t nSim, yopen_sim_hotplug_status_e status);
YOpenOSStatus yopen_usim_hotplug_handle(YOPEN_Gpio_Manage_Msg *pGpioManage);
YOpenOSStatus yopen_gpio_manage_msg_send(UINT8 msgId, UINT8 level);

/*========================================================================
*  Type Definition
*========================================================================*/
typedef struct 
{
    uint8_t                     pin_value[YOPEN_SIM_PIN_LEN_MAX+1];      ///< PIN码，最大长度为YOPEN_SIM_PIN_LEN_MAX（8）字节
}yopen_sim_verify_pin_info_s;  

typedef struct 
{
    uint8_t                     old_pin_value[YOPEN_SIM_PIN_LEN_MAX+1];  ///< 字符串类型。旧的PIN码，最大长度为YOPEN_SIM_PIN_LEN_MAX（8）字节
    uint8_t                     new_pin_value[YOPEN_SIM_PIN_LEN_MAX+1];  ///< 字符串类型。新的PIN码，最大长度为YOPEN_SIM_PIN_LEN_MAX（8）字节
}yopen_sim_change_pin_info_s; 

typedef struct 
{
    uint8_t                     puk_value[YOPEN_SIM_PIN_LEN_MAX+1];      ///< 字符串类型。PUK码，最大长度为YOPEN_SIM_PIN_LEN_MAX（8）字节
    uint8_t                     new_pin_value[YOPEN_SIM_PIN_LEN_MAX+1];  ///< 字符串类型。新的PIN码，最大长度为YOPEN_SIM_PIN_LEN_MAX（8）字节
}yopen_sim_unblock_pin_info_s;

/**
 * @struct yopen_sim_imsi_t
 * @brief type for imsi
 *
 */
typedef struct
{
    char imsi[YOPEN_SIM_IMSI_LEN_MAX + 1]; /*!< imsi, in string format */
} yopen_sim_imsi_t;
/**
 * @struct yopen_sim_iccid_t
 * @brief type for iccid
 *
 */
typedef struct
{
    char id[YOPEN_SIM_ICCID_LEN + 1]; /*!< iccid, in string format */
} yopen_sim_iccid_t;

/**
 * @struct yopen_sim_get_imsi_cnf_t
 * @brief the result of yopen_sim_get_imsi API
 *
 */
typedef struct
{
    YOPEN_MODEM_COMMON_CNF_HDR; /*!< common header, simid and err_code */
    yopen_sim_imsi_t imsi;      /*!< imsi data, in string format */
} yopen_sim_get_imsi_cnf_t;

/**
 * @struct yopen_sim_get_iccid_cnf_t
 * @brief the result of yopen_sim_get_iccid API
 *
 */
typedef struct
{
    YOPEN_MODEM_COMMON_CNF_HDR; /*!< common header, simid and err_code */
    yopen_sim_iccid_t iccid;    /*!< iccid in string format */
} yopen_sim_get_iccid_cnf_t;


/**
 * @struct yopen_sim_status_event_t
 * @brief Data content carried by event EVENT_MODEM_SIM_STATUS report
 *
 */
typedef struct
{
    uint8_t      simid;  /*!< SIM card ID */
    yopen_sim_status_e status; /*!< SIM card status */
    uint32_t     cause;  /*!< TODO 0:normal; 1: status change caused by cfun0 */
} yopen_sim_status_event_t;
/*========================================================================
  *  Callback Definition
  *========================================================================*/
/**
 * @brief SIM card callback function pointer type definition
 *
 * This type defines the general callback function format for SIM card related operations, used for result notification after asynchronous operations are completed.
 *
 * @param [in] ctx
 *           - User context pointer, passed in by the caller
 * @param [in] argv
 *           - Callback parameter pointer, pointing to the structure containing operation results
 */
typedef void (*sim_callback_ptr)(void *ctx, void *argv);


/**
 * @brief 移动网络事件回调函数
 * @param nSim			SIM卡索引
 * @param ind_type		移动网络事件类型   yopen_api_event_id_e
 *                                      YOPEN_SIM_STATUS_IND		信号质量, 对应 yopen_sim_status_event_t
 * @param *ctx			事件对应的信息数据
 * @return void
 */
typedef void (*yopen_sim_callback)(uint8_t nSim, unsigned int ind_type, void *ctx);

/*========================================================================
 *	function Definition
 *========================================================================*/

/**
 * @brief 该函数用于获取(U)SIM卡的IMSI
 * @param nSim			 所用的(U)SIM卡
 * @param imsi		     (U)SIM卡的IMSI
 * @param imsi_len	     imsi数组的长度
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_get_imsi(uint8_t nSim, char *imsi, size_t imsi_len);

/**
 * @brief Asynchronously get SIM card IMSI information
 *
 * This function is used to asynchronously read the IMSI (International Mobile Subscriber Identity) information of the SIM card.
 * IMSI is the unique identifier of the SIM card, used to identify user identity in the mobile network.
 * The operation is executed asynchronously, and the result is returned through the callback function.
 *
 * @param [in] simid
 *           - SIM card ID, identifying the SIM card to read
 *
 * @param [in] cb
 *           - Callback function pointer, used to receive operation results
 *           - Callback parameter type: yopen_sim_get_imsi_cnf_t
 *           - Return IMSI string on success, return error code on failure
 *
 * @param [in] ctx
 *           - User context pointer, will be passed as-is when calling back
 *
 * @return yopen_sim_errcode_e
 *
 * @note This function is an asynchronous operation, actual execution results are returned through the callback function
 * @note IMSI is usually a 15-digit number string, format: MCC (3 digits) + MNC (2-3 digits) + MSIN (9-10 digits)
 * @note Before calling this function, it is recommended to check if the SIM card status is ready (QOSA_SIM_STATUS_READY)
 */
extern yopen_sim_errcode_e yopen_sim_get_imsi_async(uint8_t simid, sim_callback_ptr cb, void *ctx);


/**
 * @brief 该函数用于获取(U)SIM卡的ICCID
 * @param nSim			所用的(U)SIM卡
 * @param iccid		    SIM卡的ICCID
 * @param iccid_len	    SIM卡的ICCID数组长度
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_get_iccid(uint8_t nSim, char *iccid, size_t iccid_len);



/**
 * @brief Asynchronously get SIM card ICCID information
 *
 * This function is used to asynchronously read the Integrated Circuit Card Identifier (ICCID) of the SIM card.
 * ICCID is the unique identifier of the SIM card, usually printed on the SIM card surface, used to identify the SIM card.
 * The operation is executed asynchronously, and the result is returned through the callback function.
 *
 * @param [in] simid
 *           - SIM card ID, identifying the SIM card to read
 *
 * @param [in] cb
 *           - Callback function pointer, used to receive operation results
 *           - Callback parameter type: yopen_sim_get_iccid_cnf_t
 *           - Return ICCID string on success, return error code on failure
 *
 * @param [in] ctx
 *           - User context pointer, will be passed as-is when calling back
 *
 * @return yopen_sim_errcode_e
 *
 * @note This function is an asynchronous operation, actual execution results are returned through the callback function
 * @note ICCID is usually a 19-20 digit number string, format: MMCC IINN NNNN NNNN NN C
 */
extern yopen_sim_errcode_e yopen_sim_get_iccid_async(uint8_t simid, sim_callback_ptr cb, void *ctx);


/**
 * @brief 该函数用于获取(U)SIM卡的本机号码
 * @param nSim			    所用的(U)SIM卡
 * @param phonenumber	    SIM卡的本机号码
 * @param phonenumber_len	SIM卡的本机号码数组长度
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_get_phonenumber(uint8_t nSim, char *phonenumber,size_t phonenumber_len);

/**
 * @brief 该函数用于IMS注册成功后，获取本机号码
 * @param nSim			    所用的(U)SIM卡
 * @param phonenumber	    SIM卡的本机号码
 * @param phonenumber_len	SIM卡的本机号码数组长度
 * @return yopen_sim_errcode_e
 */
yopen_sim_errcode_e yopen_sim_get_ims_phonenumber(uint8_t nSim, char *phonenumber,size_t phonenumber_len);

/**
 * @brief 该函数用于获取(U)SIM卡状态信息。
 * @param nSim			    所用的(U)SIM卡
 * @param card_status	    SIM卡状态信息
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_get_card_status(uint8_t nSim, yopen_sim_status_e *card_status);


/**
 * @brief 该函数用于启用(U)SIM卡PIN码验证
 * @param nSim			    所用的(U)SIM卡
 * @param pt_info	        PIN码
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_enable_pin(uint8_t nSim, yopen_sim_verify_pin_info_s *pt_info);


/**
 * @brief 该函数用于关闭(U)SIM卡PIN码验证
 * @param nSim			    所用的(U)SIM卡
 * @param pt_info	        PIN码
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_disable_pin(uint8_t nSim, yopen_sim_verify_pin_info_s *pt_info);

/**
 * @brief 该函数用于获取当前操作的(U)SIM卡卡槽ID。该函数仅适用于双卡单待应用。
 * @return 0   (U)SIM卡1
           1   (U)SIM卡2
 */
extern int yopen_sim_get_operate_id(void);


/**
 * @brief 该函数用于设置需要操作的(U)SIM卡卡槽ID，默认值为0。该函数仅适用于双卡单待应用，以切换操作的物理卡槽。
 * @param nSim			    所用的(U)SIM卡
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_set_operate_id(uint8_t nSim);


/**
 * @brief 设置SIM卡软件测
 * @param nSim			    所用的(U)SIM卡
 * @param period            检测周期，单位s，0表示关闭，目前只支持0和29                 
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_set_present_det(uint8_t nSim, uint16_t period);


/**
 * @brief 获取SIM卡软件测
 * @param nSim			    所用的(U)SIM卡
 * @param period            检测周期，单位s，0表示关闭，目前只支持0和29                 
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_get_present_det(uint8_t nSim, uint16_t* period);

/**
 * @brief Get SIM card function block initialization status
 *
 * This function is used to query the initialization status of the specified SIM card function module, and the return value is a combination of multiple status flag bits.
 * You can check whether each function module has completed initialization through bitwise operations.
 *
 * @param [in] simid
 *           - SIM card ID, identifying the SIM card to query
 *
 * @return int
 *           - Return value is a combination of the following status flag bits (bitwise OR):
 *           - YOPEN_SIMINI_STAT_START: SIM card initialization start status
 *           - YOPEN_SIMINI_STAT_CPIN_READY: SIM card PIN code ready status
 *           - YOPEN_SIMINI_STAT_SMS_DONE: SMS function initialization completion status
 *           - YOPEN_SIMINI_STAT_PB_DONE: Phone book function initialization completion status
 *           - You can use bitwise AND operation (&) to check if a specific status is set
 *
 * @note This function is a synchronous operation and will return results immediately
 * @note The return value is a combination of multiple status flag bits, which need to be parsed using bitwise operations
 * @note Different status flag bits may be set at different time points, reflecting different stages of SIM card initialization
 */
extern int yopen_sim_read_init_stat(uint8_t simid);


/**
 * @brief 该函数用于注册(U)SIM卡状态变化事件回调函数
 * @param simid			所用的(U)SIM卡
 * @param cb            回调函数指针，用于接收事件通知
 * @return yopen_sim_errcode_e
 */
extern yopen_sim_errcode_e yopen_sim_register_cb(uint8_t simid, yopen_sim_callback cb);

// 热插拔相关API暂不支持

/**
 * @brief 该函数用于切换和配置(U)SIM卡热插拔检测功能
 * @param nSim			    所用的(U)SIM卡
 * @param status	        热插拔检测功能状态 
 * @param gpio_cfg	        热插拔检测引脚的配置
 * @return yopen_sim_errcode_e
 */
// extern yopen_sim_errcode_e yopen_sim_set_hotplug(uint8_t nSim, yopen_sim_hotplug_e status, yopen_sim_hotplug_gpio_s* gpio_cfg);
// typedef yopen_sim_errcode_e(*_unuseapi_yopen_sim_set_hotplug_t)(uint8_t nSim, yopen_sim_hotplug_e status, yopen_sim_hotplug_gpio_s* gpio_cfg);

/**
 * @brief 该函数用于注册(U)SIM卡热插拔检测相关事件回调函数
 * @param cb			    热插拔检测相关事件回调函数
 * @return yopen_sim_errcode_e
 */
// extern yopen_sim_errcode_e yopen_sim_hotplug_register_cb(yopen_sim_hotplug_cb_t cb);
// typedef yopen_sim_errcode_e (*_unuseapi_yopen_sim_hotplug_register_cb_t)(yopen_sim_hotplug_cb_t cb);



/**
 * @brief 热插拔唤醒功能开关。
 *
 * @param state        [in]    热插拔功能状态，默认为0，0：关闭（通用GPIO模式）；1：开启（wakeup引脚模式）
 *
 * @return yopen_sim_errcode_e
 */
// extern yopen_sim_errcode_e yopen_sim_hotplug_wakeup_state(uint8 state);
// typedef yopen_sim_errcode_e(*_unuseapi_yopen_sim_hotplug_wakeup_state_t)(uint8 state);

/** @}*/

#ifdef __cplusplus
}
#endif
#endif

