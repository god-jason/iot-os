
/**
 * @defgroup yopen_nw 蜂窝网络信息功能
 * @{
 */

#ifndef _YOPEN_NW_H_
#define _YOPEN_NW_H_
//nw is releated to network 
#ifdef __cplusplus   // this area code will compile with c program
extern "C" {
#endif
#include "yopen_api_common.h"
#include "yopen_type.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define YOPEN_NW_MCC_MAX_LEN 3                 //maximum length of MCC
#define YOPEN_NW_MNC_MAX_LEN 3                 //maximum length of MNC
#define YOPEN_NW_LONG_OPER_MAX_LEN 32          //maximum length of long operator name
#define YOPEN_NW_SHORT_OPER_MAX_LEN 32         //maximum length of short operator name

#define YOPEN_NW_CELL_MAX_NUM 7                //maxinum number of cell infomation (include serving cell and neighbour cell)

/** Use single BIT to represent rat */
#define YOPEN_NW_RAT_BIT_NONE_0         (0) /*!< Reserved bit 0 */
#define YOPEN_NW_RAT_BIT_NONE_1         (1) /*!< Reserved bit 1 */
#define YOPEN_NW_RAT_BIT_GSM            (2) /*!< GSM RAT bit */
#define YOPEN_NW_RAT_BIT_WCDMA          (3) /*!< WCDMA RAT bit */
#define YOPEN_NW_RAT_BIT_LTE            (4) /*!< LTE RAT bit */
#define YOPEN_NW_RAT_BIT_NR5G           (5) /*!< NR5G RAT bit */
/*===========================================================================
 * Enum
 ===========================================================================*/
/**@brief 蜂窝网络信息API返回值枚举*/
typedef enum
{
	YOPEN_NW_SUCCESS                   = 0,								///< 函数执行成功
    YOPEN_NW_EXECUTE_ERR               = 1 | (YOPEN_COMPONENT_NETWORK << 16),	///< 函数执行失败
    YOPEN_NW_MEM_ADDR_NULL_ERR,         									///< 函数的参数地址为空
    YOPEN_NW_INVALID_PARAM_ERR,											///< 函数的参数无效
    YOPEN_NW_CFW_CFUN_GET_ERR,												///< 功能模式获取失败
    YOPEN_NW_CFUN_DISABLE_ERR          = 5 | (YOPEN_COMPONENT_NETWORK << 16),	///< 非全功能模式，导致相关信息无法获取
    YOPEN_NW_CFW_NW_STATUS_GET_ERR,										///< 获取网络状态失败，导致相关信息无法获取
    YOPEN_NW_NOT_SEARCHING_ERR,											///< 未搜索到要注册的运营商，导致相关信息无法获取
    YOPEN_NW_NOT_REGISTERED_ERR,											///< 未注册网络，导致相关信息无法获取
    YOPEN_NW_CFW_GPRS_STATUS_GET_ERR,										///< 获取PS域网络状态失败
    YOPEN_GPRS_NOT_SEARCHING_ERR     = 10 | (YOPEN_COMPONENT_NETWORK << 16),	///< 未搜索到PS域要注册的运营商
    YOPEN_GPRS_NOT_REGISTERED_ERR,											///< 未注册到PS域网络
    YOPEN_NW_CFW_NW_QUAL_GET_ERR,											///< 获取信号质量失败
    YOPEN_NW_CFW_OPER_ID_GET_ERR,											///< 获取运营商ID信息失败
    YOPEN_NW_CFW_OPER_NAME_GET_ERR,										///< 获取运营商名称失败
    YOPEN_NW_CFW_OPER_SET_ERR        = 15 | (YOPEN_COMPONENT_NETWORK << 16),	///< 设置运营商失败
    YOPEN_NW_SIM_ERR,														///< (U)SIM卡相关错误，导致信息获取失败
    YOPEN_NW_NO_MEM_ERR,													///< 申请内存失败
    YOPEN_NW_SEMAPHORE_CREATE_ERR,											///< 创建信号量失败
    YOPEN_NW_SEMAPHORE_TIMEOUT_ERR,										///< 等待信号量超时
    YOPEN_NW_NITZ_NOT_UPDATE_ERR     = 20 | (YOPEN_COMPONENT_NETWORK << 16),	///< 网络时间未同步
    YOPEN_NW_CFW_EMOD_START_ERR,											///< 进入工程模式失败，导致小区信息获取失败
    YOPEN_NW_OPERATOR_NOT_ALLOWED,											///< 操作不允许
    YOPEN_NW_CFW_RRCRel_SET_ERR,											///< 设置RRC释放时间值失败
}yopen_nw_errcode_e;

/*network access technology type, only support 7*/
/**@brief 注网制式枚举*/
typedef enum 
{
    YOPEN_NW_ACCESS_TECH_GSM                 = 0,	///< 2G（GSM，不支持）
    YOPEN_NW_ACCESS_TECH_GSM_COMPACT         = 1,	///< 2G（GSM COMPACT，不支持）
    YOPEN_NW_ACCESS_TECH_UTRAN               = 2,	///< 3G（UTRAN，不支持）
    YOPEN_NW_ACCESS_TECH_GSM_wEGPRS          = 3,	///< 2G（EGPRS，不支持）
    YOPEN_NW_ACCESS_TECH_UTRAN_wHSDPA        = 4,	///< 3G（HSDPA，不支持）
    YOPEN_NW_ACCESS_TECH_UTRAN_wHSUPA        = 5,	///< 3G（HSUPA，不支持）
    YOPEN_NW_ACCESS_TECH_UTRAN_wHSDPA_HSUPA  = 6,	///< 3G（HSDPA和HSUPA，不支持）
    YOPEN_NW_ACCESS_TECH_E_UTRAN             = 7,	///< 4G（E-UTRAN）
}yopen_nw_act_type_e;

/*network register status, equal to creg*/
/**@brief 注网状态枚举*/
typedef enum
{
    YOPEN_NW_REG_STATE_NOT_REGISTERED=0,            	///< 未注册，MT当前未搜索到注册的运营商
    YOPEN_NW_REG_STATE_HOME_NETWORK=1,              	///< 已注册，归属地网络
    YOPEN_NW_REG_STATE_TRYING_ATTACH_OR_SEARCHING=2,	///< 未注册，MT当前正在尝试注网或者正在搜索运营商以进行注网
    YOPEN_NW_REG_STATE_DENIED=3,                    	///< 注册被拒绝
    YOPEN_NW_REG_STATE_UNKNOWN=4,                   	///< 未知状态
    YOPEN_NW_REG_STATE_ROAMING=5,                   	///< 已注册，漫游状态
}yopen_nw_reg_state_e;

typedef enum
{
    YOPEN_NW_EUTRA_BAND_WIDTH_1_4_MHZ = 0,
    YOPEN_NW_EUTRA_BAND_WIDTH_3_MHZ,
    YOPEN_NW_EUTRA_BAND_WIDTH_5_MHZ,
    YOPEN_NW_EUTRA_BAND_WIDTH_10_MHZ,
    YOPEN_NW_EUTRA_BAND_WIDTH_15_MHZ,
    YOPEN_NW_EUTRA_BAND_WIDTH_20_MHZ,
    YOPEN_NW_EUTRA_BAND_WIDTH_MAX,
}yopen_nw_eutra_band_width_e;



typedef enum
{
    YOPEN_NW_NAS_RRC_REL_DURING_ATTACH = 0, /*!< RRC release, without ATTACH ACCEPT/REJECT */
    YOPEN_NW_NAS_RRC_REL_DURING_TAU,        /*!< RRC release, without TAU ACCEPT/REJECT */
    YOPEN_NW_NAS_NO_RSP_DURING_ATTACH,      /*!< No response for ATTACH REQUEST */
    YOPEN_NW_NAS_NO_RSP_DURING_TAU,         /*!< No response for TAU REQUEST */
    YOPEN_NW_NAS_NO_RSP_DURING_SR,          /*!< No response for SERVICE REQUEST */
    YOPEN_NW_NAS_NO_RSP_DURING_DETACH,      /*!< No response for DETACH REQUEST */
    YOPEN_NW_NAS_ATTACH_REJECT,             /*!< ATTACH REJECT */
    YOPEN_NW_NAS_TAU_REJECT,                /*!< TAU REJECT */
    YOPEN_NW_NAS_SERVICE_REJECT             /*!< SERVICE REJECT */
} yopen_nw_nas_event_type_e;

/**
 * @enum yopen_nw_rat_e
 * @brief Represents rat, uses bit to represent rat information, can be used in combination when setting rat mode
 *
 */
typedef enum
{
    YOPEN_NW_RAT_UNKNOWN,
    YOPEN_NW_RAT_2G = 1 << YOPEN_NW_RAT_BIT_GSM,   /*!< RAT: GSM */
    YOPEN_NW_RAT_4G = 1 << YOPEN_NW_RAT_BIT_LTE,   /*!< RAT: LTE */
} yopen_nw_rat_e;


/*===========================================================================
 * Struct
 ===========================================================================*/
/**@brief 语音或数据拨号的注网状态信息结构体*/
typedef struct
{
	yopen_nw_reg_state_e   state;			///< 注网状态
	int                 lac;			///< 位置区码
	int                 cid;			///< 小区全球识别码
	yopen_nw_act_type_e    act;			///< 注网制式
}yopen_nw_common_reg_status_info_s;
/**@brief 当前网络时间信息结构体*/
typedef struct 
{
	char      nitz_time[32];			///< 本地时间。格式为：YY/MM/DD  HH:MM:SS  '+/-'TZ	DSTB，TZ：本地时区（以15分钟为单位显示本地时间和GMT时间的差异），DST：夏令时调整时间。例如：20/10/21 09:17:43 +32 00
	long      abs_time;                 ///< 从1970-1-1，00:00:00（UTC时间）到现在的总秒数。0表示不可用
}yopen_nw_nitz_time_info_s;
/**@brief 运营商信息结构体*/
typedef struct
{
    char long_oper_name[YOPEN_NW_LONG_OPER_MAX_LEN+1];		///< 运营商名称全称
    char short_oper_name[YOPEN_NW_SHORT_OPER_MAX_LEN+1];	///< 运营商名称简称
    char mcc[YOPEN_NW_MCC_MAX_LEN+1];						///< 移动设备国家代码
    char mnc[YOPEN_NW_MNC_MAX_LEN+1];						///< 移动设备网络代码
}yopen_nw_operator_info_s;
/**@brief 注网状态信息结构体*/
typedef struct
{
	yopen_nw_common_reg_status_info_s   data_reg;          ///< 数据拨号注网状态信息
}yopen_nw_reg_status_info_s;

/**@brief 选择的运营商信息结构体*/
typedef struct 
{
    unsigned char     nw_selection_mode;                ///< 运营商选择方式。0	Modem自动选择，MCC和MNC无效。1	手动选择
    char              mcc[YOPEN_NW_MCC_MAX_LEN+1];         ///< 移动设备国家代码
    char              mnc[YOPEN_NW_MNC_MAX_LEN+1];         ///< 移动设备网络代码
    yopen_nw_act_type_e  act;                              ///< 注网制式
/**@brief 网络信号强度信息结构体*/
}yopen_nw_seclection_info_s;
typedef struct
{
    int rssi;                                   ///< 接收信号强度指示，99表示无效值
    int bitErrorRate;                           ///< 信道误码率，只在通话过程中有效，99表示无效值
    int rsrq;                                   ///< 参考信号接收质量，用于LTE制式下，255表示无效值
    int rsrp;				                    ///< 参考信号接收功率，用于LTE制式下，255表示无效值
    int snr;				                    ///< 服务小区snr值。范围：-20~40。单位：dBm
}yopen_nw_signal_strength_info_s;
/**@brief LTE制式下获取到的小区信息结构体*/
typedef struct
{
    int flag;                               ///< 小区类别。0	主小区。1	频间相邻小区
    int cid;                                ///< 小区全球识别码；0表示未接收到小区全球识别码
    int mcc;                                ///< 移动设备国家代码
    int mnc;                                ///< 移动设备网络代码
    int tac;                                ///< 跟踪区域码
    int pci;                                ///< 物理小区识别码
    int earfcn;                             ///< E-UTRA绝对射频信道号。范围：0~65535
    int rsrp;
	int rsrq;
	int snr;
	char mnc_len;							///< 移动设备网络代码长度
	BOOL isTdd;								///< 无线网络制式。0:FDD      1:TDD,仅服务小区有效
	char band;								///< 小区的频段。仅服务小区有效
}yopen_nw_lte_cell_info_s;

/**@brief 小区信息结构体*/
typedef struct 
{
    int                     lte_info_valid;                     ///< LTE制式下小区信息是否可用。0	不可用。1	可用
    int                     lte_info_num;                       ///< LTE制式下获取到的小区个数
    yopen_nw_lte_cell_info_s   lte_info[YOPEN_NW_CELL_MAX_NUM];       ///< LTE制式下获取到的小区信息列表
}yopen_nw_cell_info_s;

typedef struct
{
    int pci;
    int snr;
    int rssi;
    int rsrp;   /* Units: dBm. */
    int rsrq;   /* Units: dB. */
    int band;
    int ul_earfcn;
    int ul_band_width;  /* Index of band width. See yopen_nw_eutra_band_width_e. */
    int ul_bler;    /* Percentage = ul_bler/10000. e.g. 0.21% = 21 / 10000. */
    int dl_earfcn;
    int dl_band_width;  /* Index of band width. See yopen_nw_eutra_band_width_e. */
    int dl_bler;    /* Percentage = dl_bler/10000. e.g. 0.21% = 21 / 10000. */
    int tx_power;   /* Units: dBm. */
    int tx_max_power;   /* Units: dBm. */
    int pusch_tx_power; /* Units: dBm. */
    int pucch_tx_power; /* Units: dBm. */
    int tx_time;    /* Units: 0.1s. */
    int rx_time;    /* Units: 0.1s. */
}yopen_nw_phy_status_info_s;

//GSM measurement data
typedef int16_t yopen_signal_level_t; /**< received signal strength; unit dBm; measurement value; range: -113 ~ -48; invalid value is YOPEN_NW_PI_INT16 */
//TODO: add signal quality data constraints
typedef int16_t yopen_ber_t; /**< bit error rate; unit: none; report value; range: 0~7, 99 means unknown */

//LTE or NR 5G measurement data, note that in 5G, rsrp rsrq sinr use SS prefix conversion macros
typedef int16_t yopen_rsrp_t;   /**< reference signal received power; unit dBm; measurement value*100; range: -15600 ~ -4400 */
typedef int16_t yopen_rssi_t;   /**< received signal strength indicator; unit dBm; measurement value*100; range: -10000 ~ -2500 */
typedef int16_t yopen_rsrq_t;   /**< reference signal received quality; unit dB; measurement value*100; range: -3400 ~ -250 */
typedef int16_t yopen_sinr_t;   /**< signal to interference plus noise ratio; unit dB; measurement value*100; range: -2300 ~ 4000 */
typedef int16_t yopen_srxlev_t; /**< cell selection S value; unit dB; actual value; range -; invalid value is YOPEN_NW_PI_INT16 */
typedef int16_t yopen_squal_t;  /**< cell selection signal quality; unit dB; actual value; range -; invalid value is YOPEN_NW_PI_INT16 */

/**
 * @enum yopen_nw_rrc_access_type_e
 * @brief RRC access type
 *
 */
typedef enum
{
    YOPEN_NW_RRC_ACCESS_TYPE_GERAN,      /*!< GERAN */
    YOPEN_NW_RRC_ACCESS_TYPE_UTRAN_TDD,  /*!< UTRAN TDD */
    YOPEN_NW_RRC_ACCESS_TYPE_UTRAN_FDD,  /*!< UTRAN FDD */
    YOPEN_NW_RRC_ACCESS_TYPE_EUTRAN_TDD, /*!< EUTRAN TDD */
    YOPEN_NW_RRC_ACCESS_TYPE_EUTRAN_FDD, /*!< EUTRAN FDD */
} yopen_nw_rrc_access_type_e;

/**
 * @enum yopen_nw_rrc_conn_mode_e
 * @brief RRC connection mode
 *
 */
typedef enum
{
    YOPEN_NW_RRC_CONN_MODE_IDLE,      /*!< RRC connection idle status */
    YOPEN_NW_RRC_CONN_MODE_CONNECTED, /*!< RRC connection established */
} yopen_nw_rrc_conn_mode_e;

/**
 * @struct yopen_nw_reg_signal_quality_event_t
 * @brief Signal status change report event
 * @brief Event YOPEN_EVENT_MODEM_NW_SIGNAL_QUALITY report carries data content
 *
 */
typedef struct
{
    uint8_t  simid; /*!< SIM card ID */

    yopen_rssi_t rssi; /*!< RSSI under LTE: refers to received signal strength indicator; unit: dBm; measurement value*100, range: -10000 ~ -2500 */
    yopen_rsrp_t rsrp; /*!< rsrp under LTE: refers to reference signal received power; unit: dBm; measurement value*100, range: -15600 ~ -4400 */
    yopen_sinr_t sinr; /*!< sinr under LTE: refers to signal to interference plus noise ratio; unit: dB; measurement value*100, range: -2300 ~ 4000 */
    yopen_rsrq_t rsrq; /*!< rsrq under LTE: refers to reference signal received quality; unit: dB; measurement value*100, range: -3400 ~ -250 */

} yopen_nw_reg_signal_quality_event_t;

/**
 * @struct yopen_nw_nas_event_t
 * @brief NAS event parameters, corresponding to event ID YOPEN_EVENT_MODEM_NW_NAS_EVENT
 *
 */
typedef struct
{
    uint8_t             simid;        /*!< SIM card ID, identifying which SIM card the event belongs to */
    yopen_nw_nas_event_type_e event_type;   /*!< NAS event type, identifying specific event category */
    uint16_t            reject_cause; /*!< Reject cause code, used when event is reject type */
} yopen_nw_nas_event_t;


/**
 * @enum yopen_nw_rrc_state_e
 * @brief RRC state
 *
 */
typedef enum
{
    YOPEN_NW_RRC_UTRAN_URA_PCH_STATE,
    YOPEN_NW_RRC_UTRAN_CELL_PCH_STATE,
    YOPEN_NW_RRC_UTRAN_CELL_FACH_STATE,
    YOPEN_NW_RRC_UTRAN_CELL_DCH_STATE,
    YOPEN_NW_RRC_GERAN_CS_CONNECTED_STATE,
    YOPEN_NW_RRC_GERAN_PS_CONNECTED_STATE,
    YOPEN_NW_RRC_GERAN_CS_AND_PS_CONNECTED_STATE,
    YOPEN_NW_RRC_EUTRAN_CONNECTED_STATE,
} yopen_nw_rrc_state_e;

/**
 * @struct yopen_nw_rrc_conn_event_t
 * @brief RRC connection status report event parameters, corresponding to event ID YOPEN_EVENT_MODEM_NW_RRC_STATUS
 *
 */
typedef struct
{
    uint8_t              simid  : 8; /*!< SIM card ID */
    yopen_nw_rrc_conn_mode_e   mode   : 8; /*!< RRC connection mode */
} yopen_nw_rrc_conn_event_t;


/**
 * @brief 移动网络事件回调函数
 * @param nSim			SIM卡索引
 * @param ind_type		移动网络事件类型   yopen_api_event_id_e
 *                                      YOPEN_NW_SIGNAL_QUALITY_IND		信号质量, 对应 uint_8 csq值
 *                                      YOPEN_NW_DATA_REG_STATUS_IND	注册状态, 对应 yopen_nw_common_reg_status_info_s
 *                                      YOPEN_NW_NITZ_TIME_UPDATE_IND 	网络时间, 对应 yopen_nw_nitz_time_info_s
 *                                      YOPEN_NW_NAS_EVENT_IND			NAS事件, 对应 yopen_nw_nas_event_t
 *                                      YOPEN_NW_SIGNAL_QUALITY_EXT_IND 扩展信号质量, 对应 yopen_nw_reg_signal_quality_event_t
 *                                      YOPEN_NW_RRC_STATUS_IND			RRC状态变化, 对应 yopen_nw_rrc_conn_event_t

 * @param *ctx			事件对应的信息数据
 * @return void
 */
typedef void (*yopen_nw_callback)(uint8_t nSim, unsigned int ind_type, void *ctx);
/**
 * @brief 异步获取主小区和相邻小区信息事件的回调函数
 * @param *cell_info			主小区和相邻小区信息
 * @return void
 */
typedef void (*yopen_getcell_async_callback)(yopen_nw_cell_info_s *cell_info);
	
/**
 * @brief 获取csq信号强度
 * @param nSim		 SIM卡索引
 * @param csq 		 返回csq信号强度信息
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_get_csq(uint8_t nSim, unsigned char *csq);


/**
 * @brief 获取当前注网的运营商信息，需要等待注网成功后才可获取到
 * @param nSim		 	 SIM卡索引
 * @param oper_info		 获取运营商信息
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_get_operator_name(uint8_t nSim, yopen_nw_operator_info_s *oper_info);


/**
 * @brief 获取当前网络注册信息。
 * @param nSim			SIM卡索引
 * @param reg_info 	 	获取当前网络注册信息
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_get_reg_status(uint8_t nSim, yopen_nw_reg_status_info_s *reg_info);


/**
 * @brief 设置运营商的选择，同步API
 * @param nSim				SIM卡索引
 * @param select_info		选择的运营商信息
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_set_selection(uint8_t nSim, yopen_nw_seclection_info_s *select_info);


/**
 * @brief 获取选择的运营商信息，需要等待注网成功
 * @param nSim				SIM卡索引
 * @param select_info		选择的运营商信息
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_get_selection(uint8_t nSim, yopen_nw_seclection_info_s *select_info);


/**
 * @brief 获取详细信号强度信息
 * @param nSim				SIM卡索引
 * @param pt_info			获取的详细信号强度信息
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_get_signal_strength(uint8_t nSim, yopen_nw_signal_strength_info_s *pt_info);


/**
 * @brief 获取当前基站时间，此时间只在注网成功的那一刻更新，需要当地网络支持
 * @param nitz_info				返回 modem 从基站获取的时间信息
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_get_nitz_time_info(yopen_nw_nitz_time_info_s *nitz_info);


/**
 * @brief 获取当前服务及邻近小区信息。同步API
 * @param nSim				SIM卡索引
 * @param cell_info			返回当前服务及邻近小区信息
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_get_cell_info(uint8_t nSim, yopen_nw_cell_info_s *cell_info);


/**
 * @brief 注册事件回调函数
 * @param nw_cb				事件回调函数。
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_register_cb(yopen_nw_callback nw_cb);


/**
 * @brief 设置使用的band
 * @param band_num				需要设置的band数量
 * @param band_arr				需要设置的band数组
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_set_band(uint8_t band_num, uint8_t *band_arr);


/**
 * @brief 获取设置的band
 * @param band_num				获取当前支持的band数量
 * @param band_arr				获取当前支持的band数组
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_get_band(uint8_t *band_num, uint8_t *band_arr);


/*
 * @brief 获取当前服务小区的物理层状态信息。
 * @param nSim				SIM卡索引
 * @param phy_status_info 	当前服务小区的物理层状态信息。
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_get_phy_status_info(uint8_t nSim,yopen_nw_phy_status_info_s *phy_status_info);

/*
 * @brief 设置RRC RELEASE时间
 * @param time				时间单位S
 * @return yopen_nw_errcode_e
 */
extern yopen_nw_errcode_e yopen_nw_set_rrc_release_time(int time);

/*
 * @brief 获取RRC RELEASE时间
 * @return 返回RRC RELEASE时间， 返回-1表示RRC RELEASE时间由网络控制
 */
extern int yopen_nw_get_rrc_release_time(void);


#ifdef __cplusplus
}
#endif
#endif
/** @}*/


