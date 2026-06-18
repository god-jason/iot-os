/**  @file
  yopen_datacall.h

  @brief
  TODO

*/
/**
 * @defgroup ycom_datacall 数据拨号功能
 * @{
 */
	


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/
#ifndef _YOPEN_DATACALL_H_
#define _YOPEN_DATACALL_H_
//nw is releated to network 
#ifdef __cplusplus   // this area code will compile with c program
extern "C" {
#endif

#include "yopen_api_common.h"
#include "yopen_type.h"

#define YOPEN_DATACALL_ERRCODE_BASE        (YOPEN_COMPONENT_NETWORK_MANAGE<<16)
#define PROFILE_IDX_MIN                 1
#define PROFILE_IDX_MAX                 8

#define APN_LEN_MAX                     64
#define USERNAME_LEN_MAX                64
#define PASSWORD_LEN_MAX                64

#define YOPEN_DATA_TYPE_INVALID          	-1
#define YOPEN_DATA_TYPE_IP                 1           
#define YOPEN_DATA_TYPE_IPV6               2           
#define YOPEN_DATA_TYPE_IPV4V6             3      

#define YOPEN_DATA_CID_MIN                 1
#define YOPEN_DATA_CID_MAX                 15

#define YOPEN_DATACALL_REGISTER_ALL_SIM     0XFF
#define YOPEN_DATACALL_REGISTER_ALL_PDP     0XFF
/**@brief 认证类型枚举*/
typedef enum
{
    YOPEN_DATA_AUTH_TYPE_NONE = 0,		///< 无认证协议	            //no authentication protocol is used for this PDP context. Username and password are removed if previously specified
    YOPEN_DATA_AUTH_TYPE_PAP,			///< PAP认证协议   			        
    YOPEN_DATA_AUTH_TYPE_CHAP,			///< CHAP认证协议	
	YOPEN_DATA_AUTH_TYPE_CHAP_PAP,      	            
}yopen_data_auth_type_e;
/**@brief IPv4信息结构体*/
typedef struct{
	UINT32      addr;///< 获取的IPv4地址
}yopen_ip4_addr_t;

/**@brief IPv6信息结构体*/
typedef struct{
	UINT32      addr[4];///< 获取的IPv6地址
}yopen_ip6_addr_t;

typedef struct{
	union{
		yopen_ip4_addr_t   v4;
		yopen_ip6_addr_t   v6;
	}addr;

	UINT8      type;
}yopen_ip_addr_t;

typedef struct
{
	INT32     cid;
	INT32     ip_type;
	BOOL     apn_valid;
	INT32     apn_len;
	CHAR     apn[APN_LEN_MAX];
	BOOL     user_valid;
	INT32     user_len;
	CHAR     username[USERNAME_LEN_MAX];
	BOOL      pwd_valid;
	INT32     pwd_len;
	char       password[PASSWORD_LEN_MAX];
	CHAR     auth_type_valid;
	INT32     auth_type;
}yopen_data_profile_info_t;
/**@brief IPv4地址信息结构*/
struct v4_address_status
{
	yopen_ip4_addr_t ip;		///< 获取的IPv4地址
};
/**@brief IPv4信息结构体*/
struct v4_info
{
	INT32 state;					///< 拨号状态
	struct v4_address_status addr;	///< IPv4地址信息
};
/**@brief IPv6地址信息结构体*/
struct v6_address_status
{
	yopen_ip6_addr_t  ip;		///< 获取的IPv6地址 
};
/**@brief IPv6信息结构体*/
struct v6_info
{
	INT32 state;					///< 拨号状态
	struct v6_address_status addr;	///< IPv6地址信息
};
/**@brief 数据拨号信息结构体*/
typedef struct
{
	INT32 cid;						///< PDP上下文ID
	INT32 ip_version;				///< IP类型
	struct v4_info v4;				///< IPv4信息
	struct v6_info v6;				///< IPv6信息
	char   apn_name[APN_LEN_MAX];	///< APN名称
}yopen_data_call_info_t;


/**@brief 数据拨号默认承载配置信息结构体*/
typedef struct
{
	uint8_t			ip_version;				///< IP类型
	yopen_ip4_addr_t      ipv4;				///< Ipv4地址
	yopen_ip6_addr_t      ipv6;				///< Ipv6地址
	char 			apn_name[APN_LEN_MAX];	///< APN名称
}yopen_data_call_default_pdn_info_s;

typedef enum{
	YOPEN_DATACALL_CONNECT_EVENT = 1,
	YOPEN_DATACALL_DISCONNECT_EVENT = 2,
	YOPEN_DATACALL_AUTO_DISCONNECT_EVENT = 3,
}yopen_datacall_event_id_e;

typedef enum{
	YOPEN_DATACALL_STATE_IDLE = 0,
	YOPEN_DATACALL_STATE_ACTIVING,
	YOPEN_DATACALL_STATE_ACTIVED,
	YOPEN_DATACALL_STATE_DEACTIVING,
}yopen_datacall_state_e;

/**@brief 数据拨号结果码*/
typedef enum
{
	YOPEN_DATACALL_SUCCESS                     = 0,							///< 函数执行成功
    YOPEN_DATACALL_EXECUTE_ERR                 = 1 | YOPEN_DATACALL_ERRCODE_BASE, ///< 函数执行失败 
    YOPEN_DATACALL_MEM_ADDR_NULL_ERR,         									///< 参数地址为NULL         
    YOPEN_DATACALL_INVALID_PARAM_ERR,											///< 参数无效
    YOPEN_DATACALL_NW_REGISTER_TIMEOUT_ERR,									///< 网络注册超时
    YOPEN_DATACALL_CFW_ACT_STATE_GET_ERR       = 5 | YOPEN_DATACALL_ERRCODE_BASE,	///< 获取PDP上下文激活状态失败
    YOPEN_DATACALL_REPEAT_ACTIVE_ERR,											///< 重复激活PDP上下文
    YOPEN_DATACALL_REPEAT_DEACTIVE_ERR,										///< 重复反激活PDP上下文
    YOPEN_DATACALL_CFW_PDP_CTX_SET_ERR,										///< 设置PDP上下文错误
    YOPEN_DATACALL_CFW_PDP_CTX_GET_ERR,										///< 获取PDP上下文错误
    YOPEN_DATACALL_CS_CALL_ERR                 = 10 | YOPEN_DATACALL_ERRCODE_BASE,///< 正在通话导致数据业务操作失败
    YOPEN_DATACALL_CFW_CFUN_GET_ERR,											///< 功能模式获取失败
    YOPEN_DATACALL_CFUN_DISABLE_ERR,											///< 非全功能模式导致数据业务操作失败
    YOPEN_DATACALL_NW_STATUS_GET_ERR,											///< 网络注册状态获取失败
    YOPEN_DATACALL_NOT_REGISTERED_ERR,											///< 网络未注册
    YOPEN_DATACALL_NO_MEM_ERR                  = 15 | YOPEN_DATACALL_ERRCODE_BASE,///< 内存申请失败
    YOPEN_DATACALL_CFW_ATTACH_STATUS_GET_ERR,									///< 获取网络附着状态失败
    YOPEN_DATACALL_SEMAPHORE_CREATE_ERR,										///< 创建信号量失败
    YOPEN_DATACALL_SEMAPHORE_TIMEOUT_ERR,										///< 等待信号量超时
    YOPEN_DATACALL_CFW_ATTACH_REQUEST_ERR,										///< 网络附着被拒
    YOPEN_DATACALL_CFW_ACTIVE_REQUEST_ERR      = 20 | YOPEN_DATACALL_ERRCODE_BASE,///< PDP上下文激活被拒
    YOPEN_DATACALL_ACTIVE_FAIL_ERR,											///< PDP上下文激活失败
    YOPEN_DATACALL_CFW_DEACTIVE_REQUEST_ERR,									///< PDP上下文反激活被拒
    YOPEN_DATACALL_NO_DFTPDN_CFG_CONTEXT,										///< 未配置默认承载上下文
    YOPEN_DATACALL_NO_DFTPDN_INFO_CONTEXT,										///< 无默认承载上下文信息
	YOPEN_DATACALL_OTHER
}yopen_datacall_errcode_e;


typedef enum 
{
    YOPEN_NETIF_UPLOAD_FASTPATH_PPP = 1, //ppp lan fastpath to wan
    YOPEN_NETIF_UPLOAD_FASTPATH_ETH, //eth lan fastpath to wan
    YOPEN_NETIF_UPLOAD_LWIP_TOTAL,  //lwip output process(include local output and forward from lan)
    YOPEN_NETIF_UPLOAD_LWIP_FORWARD_PPP, //forward from ppp lan
    YOPEN_NETIF_UPLOAD_LWIP_FORWARD_ETH, //forward from eth lan
    YOPEN_NETIF_DOWNLOAD_FASTPATH_PPP, //wan fastpath to ppp lan
    YOPEN_NETIF_DOWNLOAD_FASTPATH_ETH, //wan fastpath to eth lan
    YOPEN_NETIF_DOWNLOAD_LWIP_TOTAL, //lwip input process(include local input and forward to lan)
    YOPEN_NETIF_DOWNLOAD_LWIP_FORWARD_PPP, //forward to ppp lan
    YOPEN_NETIF_DOWNLOAD_LWIP_FORWARD_ETH, //forward to eth lan
}yopen_netif_type;

typedef void (*yopen_netif_info_cb)(int cid, yopen_netif_type type, int size);

/**
 * @brief 该回调函数用于上报数据拨号事件
 * @param nSim			SIM卡索引
 * @param ind_type		数据拨号事件类型  yopen_api_event_id_e
 *                                      YOPEN_DATACALL_PDP_DEACTIVE_IND		PDP上下文去激活
 *                                      YOPEN_DATACALL_PDP_ACTIVE_IND		PDP上下文激活
 * @param cid        	PDP上下文ID
 * @param result 		PDP上下文激活和反激活的结果
 * @param *ctx			回调函数的传参指针
 * @return void
 */
typedef void (*yopen_datacall_callback)(uint8_t nSim, unsigned int ind_type, int cid, bool result, void *ctx);


/*========================================================================
  *  Callback Definition
  *========================================================================*/


/*========================================================================
 *	function Definition
 *========================================================================*/
/**
 * @brief 设置数据拨号信息
 * @param nSim          SIM卡索引
 * @param profile_idx   PDP上下文ID
 * @param call_info     数据拨号信息
 * @return yopen_datacall_errcode_e
 */
extern yopen_datacall_errcode_e yopen_set_data_call_info(UINT8 nSim, INT32 profile_idx, yopen_data_call_info_t *call_info);

/**
 * @brief 获取数据拨号信息
 * @param nSim          SIM卡索引
 * @param profile_idx   PDP上下文ID
 * @param call_info     数据拨号信息
 * @return yopen_datacall_errcode_e
 */
extern yopen_datacall_errcode_e yopen_get_data_call_info(UINT8 nSim, INT32 profile_idx, yopen_data_call_info_t *call_info);


/**
 * @brief 启动数据拨号，默认为同步模式
 * @param nSim			SIM卡索引
 * @param cid			PDP上下文ID
 * @param ip_version 	IP类型
 * @param apn_name		APN名称
 * @param username		用户名称
 * @param password		用户密码
 * @param auth_type		认证类型
 * @return yopen_datacall_errcode_e
 */
extern yopen_datacall_errcode_e yopen_start_data_call(UINT8 nSim, INT32 cid, INT32 ip_version, CHAR *apn_name, CHAR *username, CHAR *password, INT32 auth_type);


/**
 * @brief 获取默认承载配置
 * @param nSim		SIM卡索引
 * @param ctx		数据拨号默认承载配置信息
 * @return yopen_datacall_errcode_e
 */
extern yopen_datacall_errcode_e yopen_datacall_get_default_pdn_info(uint8_t nSim, yopen_data_call_default_pdn_info_s* ctx);


/**
 * @brief 获取当前PDP上下文激活状态
 * @param nSim			SIM卡索引
 * @param profile_idx	PDP上下文ID
 * @return TRUE	or FALSE
 */
extern bool yopen_datacall_get_sim_profile_is_active(uint8_t nSim, int profile_idx);


/**
 * @brief 终止数据拨号，默认为同步模式。
 * @param nSim			SIM卡索引
 * @param profile_idx	PDP上下文ID
 * @return yopen_datacall_errcode_e
 */
extern yopen_datacall_errcode_e   yopen_stop_data_call(UINT8 nSim, INT32 profile_idx);



/**
 * @brief 等待网络注册结果。
 * @param nSim			SIM卡索引
 * @param timeout_s		等待网络注册的超时时间
 * @return yopen_datacall_errcode_e
 */
extern yopen_datacall_errcode_e yopen_network_register_wait(uint8_t nSim, unsigned int timeout_s);


/**
 * @brief 用于启用NAT功能。模块重启后配置生效
 * @param nSim			SIM卡索引
 * @param natmode		设置的NAT模式
 * @return yopen_datacall_errcode_e
 */
extern yopen_datacall_errcode_e yopen_datacall_set_nat(uint8_t nSim, UINT32 natmode);

/**
 * @brief 该函数用于查询已启用NAT功能
 * @param nSim			SIM卡索引
 * @param natmode		当前启用的NAT模式
 * @return yopen_datacall_errcode_e
 */
extern yopen_datacall_errcode_e yopen_datacall_get_nat(uint8_t nSim, UINT32 *natmode);

/**
 * @brief 设置dns的服务器地址
 * @param cid			PDP上下文ID
 * @param dns		    服务器地址
 * @param num		    服务器地址个数
 * @return 				成功设置dns服务器的个数
 */
extern INT32   yopen_set_dns_server(INT32 cid, yopen_ip_addr_t *dns, uint8 num);


/**
 * @brief 获取dns的服务器地址
 * @param cid			PDP上下文ID
 * @param dns		    服务器地址
 * @return 				获取dns服务器的个数
 */
extern INT32   yopen_get_dns_server(INT32 cid, yopen_ip_addr_t *dns);

/**
 * @brief 设置netif回调函数,上报上下行使用流量
 * @param nSim			SIM卡索引
 * @param cb			回调函数
 */
void yopen_netif_info_cb_set(yopen_netif_info_cb cb);
/**
 * @brief 用于注册数据拨号的回调函数
 * @param nSim          SIM卡索引
 * @param datacall_cb   待注册的回调函数
 * @param *ctx			回调函数的传参指针
 * @return yopen_datacall_errcode_e	
 */
yopen_datacall_errcode_e yopen_datacall_register_cb(uint8_t nSim, yopen_datacall_callback datacall_cb, void *ctx);

/**
 * @brief 设置auto apn enable, 该功能是根据用户使用的SIM卡，自动匹配对应的APN
 * @param nSim          SIM卡索引
 * @param enable        是否使能auto apn
 * @return yopen_datacall_errcode_e	
 */
extern yopen_datacall_errcode_e yopen_auto_apn_enable_set(uint8_t nSim, bool enable);

/**
 * @brief 获取auto apn enable
 * @param nSim          SIM卡索引
 * @param enable        是否使能auto apn
 * @return yopen_datacall_errcode_e	
 */
extern yopen_datacall_errcode_e yopen_auto_apn_enable_get(uint8_t nSim, bool *enable);
#ifdef __cplusplus
}
#endif
#endif
/** @}*/

