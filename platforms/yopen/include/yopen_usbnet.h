/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef _YOPEN_USBNET_H_
#define _YOPEN_USBNET_H_

#include "yopen_api_common.h"
#include "yopen_type.h"
#include "yopen_datacall.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define YOPEN_USBNET_ERRCODE_BASE        (YOPEN_COMPONENT_NETWORK_USBNET<<16)

/**
 * @defgroup yopen_usbnet 网卡拨号功能
 * @{
 */

/*===========================================================================
 * Enum
 ===========================================================================*/
 /** @brief USBNET错误码, 参考yopen_error.h*/
typedef int yopen_usbnet_errcode_e;

/** @brief 网卡类型*/
typedef enum 
{
	YOPEN_USBNET_NONE = 0,		///< 无实际意义
	YOPEN_USBNET_ECM,			///< ECM模式
	YOPEN_USBNET_RNDIS,		///< RNDIS模式
	YOPEN_USBNET_MAX			///< 无实际意义，客户无需使用
}yopen_usbnet_type_e;

/** USBNET subnet configuration parameter - IP address configuration bit */
#define YOPEN_USBNET_SUBNET_CONFIG_IPADDR  0x01
/** USBNET subnet configuration parameter - Gateway configuration bit */
#define YOPEN_USBNET_SUBNET_CONFIG_GATEWAY 0x02
/** USBNET subnet configuration parameter - Subnet mask configuration bit */
#define YOPEN_USBNET_SUBNET_CONFIG_MASK    0x04

typedef struct
{
    UINT8      flags : 4;  /*!< Bit field indicating which parameters are valid.
                             - In set operations, it specifies which fields should be configured.
                             - In get operations, it indicates which fields contain valid data.
                             Use QOSA_USBNET_SUBNET_CONFIG_* macros for bitwise combination. */
    char         ipAddr[4];  /*!< IP address, such as {192, 168, 22, 10} */
    char         gateway[4]; /*!< Gateway, such as {192, 168, 22, 1} */
    char         mask[4];    /*!< Subnet mask, such as {255, 255, 255, 0} */
} yopen_usbnet_subnet_config_t;

/*===========================================================================
 * function
 ===========================================================================*/

/**
 * @brief Configure IP, gateway, and mask allocated to the terminal in routing mode
 *
 * @param[in] simid
 *          - SIM card ID
 * @param[in] config
 *          - Subnet configuration parameter structure
 * @return qosa_usbnet_err_e
 */
extern yopen_usbnet_errcode_e yopen_usbnet_set_subnet(UINT8 simid, yopen_usbnet_subnet_config_t config);

/**
 * @brief Get IP, gateway, and mask allocated to the terminal
 *
 * @param[in] simid
 *          - SIM card ID
 * @param[out] config
 *          - Subnet configuration parameter structure pointer
 * @return yopen_usbnet_errcode_e
 */
extern yopen_usbnet_errcode_e yopen_usbnet_get_subnet(UINT8 simid, yopen_usbnet_subnet_config_t *config);

/**
* @brief 设置网卡类型，重启生效
* @param usbnet_type	[in]	网卡类型, 只支持ECM和RNDIS
*
* @return yopen_usbnet_errcode_e
*/

extern yopen_usbnet_errcode_e yopen_usbnet_set_type(yopen_usbnet_type_e usbnet_type);


/**
* @brief 获取当前网卡类型
* 
* @param usbnet_type	[out]	当前的网卡类型
*
* @return yopen_usbnet_errcode_e
*/

extern yopen_usbnet_errcode_e yopen_usbnet_get_type(yopen_usbnet_type_e *usbnet_type);


/** @}*/

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* YOPEN_API_USBNET_H */



