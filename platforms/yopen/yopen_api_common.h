/**  
  @file
  yopen_api_common.h

  @brief

*/

/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/

#ifndef YOPEN_API_COMMON_H
#define YOPEN_API_COMMON_H


#ifdef __cplusplus
extern "C" {
#endif


/*========================================================================
 *  Macro Definition
 *========================================================================*/

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif
/*
//2022.05.18 add by Tomas.Qin EGICOMM-111  Query partition information
#define	YOPEN_FLASH_MEM_HEADER1_ADDR       FLASH_MEM_HEADER1_ADDR
#define	YOPEN_FLASH_MEM_HEADER2_ADDR       FLASH_MEM_HEADER2_ADDR
#define	YOPEN_BOOTLOADER_FLASH_LOAD_ADDR   BOOTLOADER_FLASH_LOAD_ADDR
#define	YOPEN_FLASH_MEM_RESERVED           BOOTLOADER_FLASH_LOAD_ADDR+BOOTLOADER_FLASH_LOAD_SIZE
#define	YOPEN_FLASH_FOTA_REGION_START      FLASH_FOTA_REGION_START
#define	YOPEN_NVRAM_FACTORY_PHYSICAL_BASE  NVRAM_FACTORY_PHYSICAL_BASE
#define	YOPEN_NVRAM_PHYSICAL_BASE          NVRAM_PHYSICAL_BASE
#define	YOPEN_AP_FLASH_LOAD_ADDR           AP_FLASH_LOAD_ADDR
#define	YOPEN_FLASH_FS_REGION_START        FLASH_FS_REGION_START
#define	YOPEN_UPDATER_FLASH_LOAD_ADDR      UPDATER_FLASH_LOAD_ADDR
#define	YOPEN_FLASH_MEM_BACKUP_ADDR        FLASH_MEM_BACKUP_ADDR
#define	YOPEN_FLASH_MEM_PLAT_INFO_ADDR     FLASH_MEM_PLAT_INFO_ADDR
*/
/*========================================================================
 *	Enumeration Definition
 *========================================================================*/
typedef enum
{
	YOPEN_COMPONENT_NONE			= 0,
	   
	YOPEN_COMPONENT_OSI			= 0x8100,
	
	YOPEN_COMPONENT_BSP			= 0x8200,
	YOPEN_COMPONENT_BSP_GPIO		= 0x8201,
	YOPEN_COMPONENT_BSP_PWM		= 0x8202,
	YOPEN_COMPONENT_BSP_SPI		= 0x8203,
	YOPEN_COMPONENT_BSP_I2C		= 0x8204,
	//YOPEN_COMPONENT_BSP_KEY		  = 0x8205,
	YOPEN_COMPONENT_BSP_UART		= 0x8206,
	YOPEN_COMPONENT_BSP_USB		= 0x8207,
	YOPEN_COMPONENT_BSP_CMUX		= 0x8208,
	YOPEN_COMPONENT_BSP_LCD		= 0x8209,
	YOPEN_COMPONENT_BSP_CAMERA 	= 0x820A,
	YOPEN_COMPONENT_BSP_BT 		= 0x820B,
	YOPEN_COMPONENT_BSP_GNSS		= 0x820C,
	YOPEN_COMPONENT_BSP_WIFISCAN	= 0x820D,
	YOPEN_COMPONENT_BSP_ADC		= 0x820E,
	YOPEN_COMPONENT_BSP_LED		= 0x820F,
	YOPEN_COMPONENT_BSP_DECODER	= 0x8210,
	YOPEN_COMPONENT_BSP_KEYPAD 	= 0x8211,
	YOPEN_COMPONENT_BSP_RTC		= 0x8212,
	YOPEN_COMPONENT_BSP_LVGL		= 0x8213,
	YOPEN_COMPONENT_BSP_CHARGE 	= 0x8214,
	YOPEN_COMPONENT_BSP_VIRT_AT	= 0x8215,	
	YOPEN_COMPONENT_BSP_OTHER	    = 0x82FF,
	
	YOPEN_COMPONENT_STORAGE				 = 0x8300,
	YOPEN_COMPONENT_STORAGE_FLASH			 = 0x8301,
	YOPEN_COMPONENT_STORAGE_SDEMMC 		 = 0x8302,
	YOPEN_COMPONENT_STORAGE_FILE			 = 0x8303,
	YOPEN_COMPONENT_STORAGE_EXTFLASH		 = 0x8304,			 //for nor flash
	YOPEN_COMPONENT_STORAGE_EXT_NANDFLASH	 = 0x8305,		//for nand flash
	YOPEN_COMPONENT_STORAGE_ZIP			 = 0x8306,
	YOPEN_COMPONENT_STORAGE_FATFS			 = 0x8307,		//for fatfs
	YOPEN_COMPONENT_STORAGE_EMBED_NOR_FLASH = 0x8308,		//for embed flash
	YOPEN_COMPONENT_STORAGE_QCFG			 = 0x8309,      //ycom_cfg
	
	YOPEN_COMPONENT_AUDIO			= 0x8400,
	YOPEN_COMPONENT_AUDIO_CODEC	= 0x8401,
	YOPEN_COMPONENT_AUDIO_REC_PLAY = 0x8402,
	YOPEN_COMPONENT_AUDIO_TTS		= 0x8403,
	YOPEN_COMPONENT_AUDIO_DTMF 	= 0x8404,
	YOPEN_COMPONENT_AUDIO_SETTING	= 0x8405,
	YOPEN_COMPONENT_AUDIO_HEADSET	= 0x8406,
	
	YOPEN_COMPONENT_LWIP			= 0x8500,
	YOPEN_COMPONENT_LWIP_SOCKET	= 0x8501, // tcp/udp/ping/dns
	YOPEN_COMPONENT_LWIP_HTTP		= 0x8502,
	YOPEN_COMPONENT_LWIP_MQTT		= 0x8503,
	YOPEN_COMPONENT_LWIP_FTP		= 0x8504,
	YOPEN_COMPONENT_LWIP_SSL		= 0x8505,
	YOPEN_COMPONENT_LWIP_MMS		= 0x8506,
	YOPEN_COMPONENT_LWIP_LBS		= 0x8507, // ycomlocator
	YOPEN_COMPONENT_LWIP_QUECTHING = 0x8508,
	YOPEN_COMPONENT_LWIP_QNTP		= 0x8509,
	YOPEN_COMPONENT_LWIP_SMTP		= 0x8510,
	
	YOPEN_COMPONENT_NETWORK		= 0x8600,
	YOPEN_COMPONENT_NETWORK_MANAGE = 0x8601,
	YOPEN_COMPONENT_NETWORK_PPP	= 0x8602,
	YOPEN_COMPONENT_NETWORK_USBNET = 0x8603,
	
	YOPEN_COMPONENT_SIM			= 0x8700,
	
	YOPEN_COMPONENT_PM 			= 0x8800,
	YOPEN_COMPONENT_PM_SLEEP		= 0x8801,
	
	YOPEN_COMPONENT_SMS			= 0x8900,
	
	YOPEN_COMPONENT_VOICE_CALL 	= 0x8A00, // cs call (csfb)
	
	YOPEN_COMPONENT_IMS			= 0x8B00, // volte, ps sms
	
	YOPEN_COMPONENT_FOTA			= 0x8C00,
	
	YOPEN_COMPONENT_STATE_INFO 	= 0x8D00, // basic info and state
	
	YOPEN_COMPONENT_LOG			= 0x8E00,

	YOPEN_COMPONENT_FTM			= 0x8F00, //ftm test
	
    YOPEN_COMPONENT_RIL        = 0x9100,
    YOPEN_COMPONENT_I2S        = 0x9200,
    YOPEN_COMPONENT_WIFI      = 0x9300,
    YOPEN_COMPONENT_LCD       = 0x9400,
    YOPEN_COMPONENT_CAMERA       = 0x9500,
	//
	//next is reserved for app to add component
	//
	YOPEN_COMPONENT_APP_START		= 0x9F00,// app component start
	
	YOPEN_COMPONENT_MAX			= 0x9FFF,//component end
}yopen_component_e;


/*
*						errcode definition
*
* 4 bytes, little endian, signed type, as below:
*
*		----------------------------------------------------------------------------------------------------
*		|			 4				|			  3 		   |		  2 		  | 		1		   |
*		----------------------------------------------------------------------------------------------------
*		| High byte of component ID | low byte of component ID | High byte of errcode | low byte of errcode|
*		----------------------------------------------------------------------------------------------------
*
* component ID defined at enum type <yopen_errcode_component_e>.
* detail errcode defined by each component.
*
*/
//common detail errcode, 2 bytes
typedef enum
{
	YOPEN_NO_ERROR = 0,
	YOPEN_SUCCESS = 0,
	YOPEN_INVALID_PARAM_ERR,
	YOPEN_READ_NVM_ERR,
	YOPEN_STORAGE_NVM_ERR,
	
	
	YOPEN_GENERAL_ERROR  = 0xFFFF,
} yopen_errcode_e;

typedef enum
{
    YOPEN_APP_EVENT_ID_MIN = 1|(YOPEN_COMPONENT_OSI<<16),
    YOPEN_TEST_EVENT_ID_1,
    YOPEN_TEST_EVENT_ID_2,
    YOPEN_TEST_EVENT_ID_3,
    YOPEN_KERNEL_FEED_DOG,

//network
    YOPEN_NW_SIGNAL_QUALITY_IND = 1|(YOPEN_COMPONENT_NETWORK<<16), //CSQ 信号质量
    YOPEN_NW_VOICE_REG_STATUS_IND,
    YOPEN_NW_DATA_REG_STATUS_IND, //EPS 注册状态 == CEREG
    YOPEN_NW_NITZ_TIME_UPDATE_IND, //NITZ 时间更新
    YOPEN_NW_JAMMING_DETECT_IND,
    YOPEN_NW_NAS_EVENT_IND, //NAS事件, 对应 yopen_nw_nas_event_t
    YOPEN_NW_SIGNAL_QUALITY_EXT_IND, //扩展信号质量
    YOPEN_NW_RRC_STATUS_IND, //RRC状态变化

//datacall    
    YOPEN_DATACALL_ACT_RSP_IND = 1|(YOPEN_COMPONENT_NETWORK_MANAGE<<16),
    YOPEN_DATACALL_DEACT_RSP_IND,
    YOPEN_DATACALL_PDP_DEACTIVE_IND,
    YOPEN_DATACALL_PDP_ACTIVE_IND,

//usbnet
	YOPEN_USBNET_START_RSP_IND = 1|(YOPEN_COMPONENT_NETWORK_USBNET<<16),	//response of usbnet start
	YOPEN_USBNET_DEACTIVE_IND,											//usbnet down, The cause may be loss of network coverage or detached by the network
	YOPEN_USBNET_PORT_CONNECT_IND,										//usb plug in
	YOPEN_USBNET_PORT_DISCONNECT_IND,									//usb pull out

//uart
    YOPEN_UART_RX_RECV_DATA_IND = 1|(YOPEN_COMPONENT_BSP_UART<<16), //rx fifo receive data
    YOPEN_UART_RX_OVERFLOW_IND,      // rx fifo overflow
    YOPEN_UART_TX_FIFO_COMPLETE_IND, 
    YOPEN_UART_TX_COMPLETE_APP_IND, //tx fifo send complete
    YOPEN_UART_RX_RECV_DATA_APP_IND, 

//usb
	YOPEN_USB_LINESTATE_IND = 1|(YOPEN_COMPONENT_BSP_USB<<16),		//only used for kernel, to handle usb linestate


//uac
	YOPEN_USB_AUDIO_CREATE_DONE = 2000|(YOPEN_COMPONENT_BSP_USB<<16),	//UAC created ok, only used for kernel
	YOPEN_USB_AUDIO_DATA_IN = 2001|(YOPEN_COMPONENT_BSP_USB<<16),	//UAC received data, only used for kernel	
	YOPEN_USB_AUDIO_START_PLAY = 2002|(YOPEN_COMPONENT_BSP_USB<<16), //UAC start receive data, only used for kernel	
	YOPEN_USB_AUDIO_STOP_PLAY = 2003|(YOPEN_COMPONENT_BSP_USB<<16),  //UAC stop receive data, only used for kernel	
	YOPEN_USB_AUDIO_START_RECORD = 2004|(YOPEN_COMPONENT_BSP_USB<<16), //UAC start send data to host, only used for kernel	
	YOPEN_USB_AUDIO_STOP_RECORD = 2005|(YOPEN_COMPONENT_BSP_USB<<16), //UAC stop send data to host, only used for kernel	
    
//bt
    YOPEN_BT_START_STATUS_IND = 1|(YOPEN_COMPONENT_BSP_BT<<16),     //bt/ble start
    YOPEN_BT_STOP_STATUS_IND,                                    //bt/ble stop
    YOPEN_BT_BLE_RESET_IND,                                      //bt/ble reset.need to stop bt and restart
    YOPEN_BT_VISIBILE_IND,                                       //
    YOPEN_BT_HIDDEN_IND,
    YOPEN_BT_SET_LOCALNAME_IND,
    YOPEN_BT_SET_LOCALADDR_IND,
    YOPEN_BT_INQURY_IND,
    YOPEN_BT_INQURY_END_IND,
    YOPEN_BT_CANCEL_INQURY_IND,
    YOPEN_BT_BOND_IND,
    YOPEN_BT_CANCELBOND_IND,
    YOPEN_BT_CONNECT_IND,
    YOPEN_BT_DISCONNECT_IND,
    YOPEN_BT_SPP_SEND_IND,
    YOPEN_BT_SPP_RECV_IND,

    YOPEN_BT_HFP_CONNECTION_IND,
//ble
    //all:server and client can recieve this event
    //server:only server can recieve this event
    //client:only client can recieve this event
    YOPEN_BLE_CONNECT_IND = 100|(YOPEN_COMPONENT_BSP_BT<<16),       //all:ble connect
    YOPEN_BLE_DISCONNECT_IND,                                    //all:ble disconnect
    YOPEN_BLE_UPDATE_CONN_PARAM_IND,                             //all:ble update connection parameter
    
    YOPEN_BLESCAN_REPORT_IND,                                    //client:ble gatt cliet scan and report other devices

    YOPEN_BLE_GATT_MTU,                                          //all:ble connection mtu
    YOPEN_BLE_GATT_RECV_IND,                                     //server:when ble client write characteristic value or descriptor,server get the notice
    YOPEN_BLE_GATT_RECV_READ_IND,                                //server:when ble client read characteristic value or descriptor,server get the notice
    YOPEN_BLE_GATT_RECV_NOTIFICATION_IND,                        //client:client recieve notification   
    YOPEN_BLE_GATT_RECV_INDICATION_IND,                          //client:client recieve indication
    YOPEN_BLE_GATT_SEND_END,                                     //server send notification,and recieve send end notice

    YOPEN_BLE_GATT_START_DISCOVER_SERVICE_IND,                   //client:start discover service
    YOPEN_BLE_GATT_DISCOVER_SERVICE_IND,                         //client:discover service
    YOPEN_BLE_GATT_DISCOVER_CHARACTERISTIC_DATA_IND,             //client:discover characteristic
    YOPEN_BLE_GATT_DISCOVER_CHARA_DESC_IND,                      //client:discover characteristic descriptor
    YOPEN_BLE_GATT_CHARA_WRITE_WITH_RSP_IND,                     //client:write characterisctc value with response
    YOPEN_BLE_GATT_CHARA_WRITE_WITHOUT_RSP_IND,                  //client:write characteristic value without response
    YOPEN_BLE_GATT_CHARA_READ_IND,                               //client:read characteristic value by hanlde
    YOPEN_BLE_GATT_CHARA_READ_BY_UUID_IND,                       //client:read characteristic value by uuid
    YOPEN_BLE_GATT_CHARA_MULTI_READ_IND,                         //client:read miltiple characteristic value
    YOPEN_BLE_GATT_DESC_WRITE_WITH_RSP_IND,                      //client:wirte characteristic descriptor
    YOPEN_BLE_GATT_DESC_READ_IND,                                //client:read characteristic descriptor
    YOPEN_BLE_GATT_ATT_ERROR_IND,                                //client:attribute error
    YOPEN_BLE_GATT_STATE_CHANGE_IND,                             //server:通道改变
    YOPEN_BLE_GATT_SEND_NODIFY_IND,                              //server:通知server线程从缓存里读取数据发送，只在标准版本使用

    YOPEN_BT_HFP_CONNECT_IND= 200|(YOPEN_COMPONENT_BSP_BT<<16),     //bt hfp connected
    YOPEN_BT_HFP_DISCONNECT_IND,                                 //bt hfp disconnected
    YOPEN_BT_HFP_CALL_IND,                                       //bt hfp call state callback
    YOPEN_BT_HFP_CALL_SETUP_IND,                                 //bt hfp call setup state callback
    YOPEN_BT_HFP_NETWORK_IND,                                    //bt hfp network state callback
    YOPEN_BT_HFP_NETWORK_SIGNAL_IND,                             //bt hfp network signal callback
    YOPEN_BT_HFP_BATTERY_IND,                                    //bt hfp battery level callback
    YOPEN_BT_HFP_CALLHELD_IND,                                   //bt hfp callheld state callback
    YOPEN_BT_HFP_AUDIO_IND,                                      //bt hfp audio state callback
    YOPEN_BT_HFP_VOLUME_IND,                                     //bt hfp volume type callback
    YOPEN_BT_HFP_NETWORK_TYPE_IND,                               //bt hfp network type callback
    YOPEN_BT_HFP_RING_IND,                                       //bt hfp ring indication callback
    YOPEN_BT_HFP_CODEC_IND,                                      //bt hfp codec type callback

    YOPEN_BT_A2DP_AUDIO_CONFIG_IND = 300|(YOPEN_COMPONENT_BSP_BT<<16),  //bt a2dp audio config callback
    YOPEN_BT_A2DP_AUDIO_STATE_START_IND,							    //bt a2dp audio state start callback
    YOPEN_BT_A2DP_AUDIO_STATE_STOPPED_IND,						    //bt a2dp audio state stop callback
    YOPEN_BT_A2DP_CONNECTION_STATE_CONNECTED_IND,				    //bt a2dp connect callback
    YOPEN_BT_A2DP_CONNECTION_STATE_DISCONNECTED_IND,				    //bt a2dp disconnect callback
    
    YOPEN_BT_AVRCP_CONNECTION_STATE_CONNECTED_IND  = 400|(YOPEN_COMPONENT_BSP_BT<<16),		//bt avrcp connect callback
    YOPEN_BT_AVRCP_CONNECTION_STATE_DISCONNECTED_IND,			                        //bt avrcp disconnect callback
    YOPEN_BT_AVRCP_VOLUME_CHANGE_IND,							                        //bt avrcp volume change callback
    
//led_cfg(network status for netlight)
    YOPEN_LEDCFG_EVENT_IND = 1|(YOPEN_COMPONENT_BSP_PWM<<16),
    YOPEN_LEDCFG_CB_CREATE_IND,
    YOPEN_VIRTUAL_PWM_EVENT,
    YOPEN_LEDCFG_VIRT_PWM_EVENT,
    YOPEN_HR_VIRT_PWM_EVENT,
//sleep
	YOPEN_SLEEP_ENETR_AUTO_SLEPP  = 1 | (YOPEN_COMPONENT_PM_SLEEP << 16),
	YOPEN_SLEEP_EXIT_AUTO_SLEPP   = 2 | (YOPEN_COMPONENT_PM_SLEEP << 16),

//power
	YOPEN_SLEEP_QUICK_POWER_DOWM  = 1 | (YOPEN_COMPONENT_PM << 16),
	YOPEN_SLEEP_NORMAL_POWER_DOWM = 2 | (YOPEN_COMPONENT_PM << 16),
	YOPEN_SLEEP_QUICK_RESET		 = 3 | (YOPEN_COMPONENT_PM << 16),
	YOPEN_SLEEP_NORMAL_RESET 	 = 4 | (YOPEN_COMPONENT_PM << 16),
	YOPEN_SLEEP_FAST_POWEROFF 	 = 5 | (YOPEN_COMPONENT_PM << 16),

//sim
    YOPEN_PBK_INIT_OK_IND           = 1 | (YOPEN_COMPONENT_SIM << 16),
    YOPEN_SIM_APREADY_CMD_IND     = 2 | (YOPEN_COMPONENT_SIM << 16),
    YOPEN_SIM_STATUS_IND            = 3 | (YOPEN_COMPONENT_SIM << 16),

//pwrkey
    YOPEN_PWRKEY_SHUTDOWN_START_IND  = 1 | (YOPEN_COMPONENT_BSP_KEYPAD << 16),
    YOPEN_PWRKEY_PRESS_IND,
    YOPEN_PWRKEY_RELEASE_IND,
    YOPEN_PWRKEY_LONGPRESS_IND,

//wifiscan
	YOPEN_WIFISCAN_EVENT_DO          = 1 | (YOPEN_COMPONENT_BSP_WIFISCAN << 16),
	YOPEN_WIFISCAN_EVENT_ASYNC_IND   = 5 | (YOPEN_COMPONENT_BSP_WIFISCAN << 16),
//gnss
    YOPEN_GNSS_OK_CMD_IND = 1|(YOPEN_COMPONENT_BSP_GNSS<<16),	
    YOPEN_GNSS_FAIL_0_CMD_IND = 2|(YOPEN_COMPONENT_BSP_GNSS<<16),	
    YOPEN_GNSS_FAIL_1_CMD_IND= 3|(YOPEN_COMPONENT_BSP_GNSS<<16),
    YOPEN_GPS_INFO_CMD= 4|(YOPEN_COMPONENT_BSP_GNSS<<16),
    YOPEN_GNSS_POWER_EVENT_REQ= 5|(YOPEN_COMPONENT_BSP_GNSS<<16),
    YOPEN_GNSS_DISABLE_STATUS_IND= 6|(YOPEN_COMPONENT_BSP_GNSS<<16),
    
//lvgl
    YOPEN_LVGL_QUIT_IND= 1|(YOPEN_COMPONENT_BSP_LVGL<<16),

//virt at
    YOPEN_VIRT_AT_RX_RECV_DATA_IND = 1 | (YOPEN_COMPONENT_BSP_VIRT_AT<<16),

//sd
    YOPEN_SDDET_EVENT_IND = 1 | (YOPEN_COMPONENT_STORAGE_SDEMMC<<16),

//headset_det
    YOPEN_HEADSET_DET_EVENT_IND = 1 | (YOPEN_COMPONENT_AUDIO_HEADSET<<16),


// tcpip app
	YOPEN_TCPIP_APP_PDP_EVT_IND	    = 1 | (YOPEN_COMPONENT_LWIP << 16),

//locator
	YOPEN_LBS_EVENT_WIFISCAN_REC	    = 1 | (YOPEN_COMPONENT_LWIP_LBS << 16),
//BSP GPIO
    YOPEN_GPIOCTRL_EVENT_IND         = 1 | (YOPEN_COMPONENT_BSP_GPIO << 16),
    YOPEN_GPIOCTRL_TIMER_EVENT_IND         = 2 | (YOPEN_COMPONENT_BSP_GPIO << 16),
    //
    //next is reserved for app to add event id!
    //
	YOPEN_EVENT_APP_START           = 1 | (YOPEN_COMPONENT_APP_START<<16),//app event start
	
    YOPEN_EVEN_MAX                  = 0xFFFF | (YOPEN_COMPONENT_MAX<<16),//event max
}yopen_api_event_id_e;

/*========================================================================
 *  Variable Definition
 *========================================================================*/
#define CHIP_SECTOR_SIZE 		  (0x1000)    	//erase 4k each times
#define CHIP_BLOCK_SIZE 		  (0x10000)    	//erase 64k each times

/** osa modem api cnf return result structure common parameters, the first two parameters of each cnf structure must be this */
#define YOPEN_MODEM_COMMON_CNF_HDR                                                                                                                              \
    uint8_t simid; /*!< sim id */                                                                                                                         \
    uint32_t                                                                                                                                              \
        err_code /*!< error code, success is OSA_OK, failure is others, when this value indicates failure, application layer is not allowed to parse other parameters */

/**
 * @struct yopen_modem_general_cnf_t
 * @brief Modem cnf general reply, used to return simple error and execution success reply.
 *
 */
typedef struct
{
    unsigned char simid;
    unsigned int  err_code;
} yopen_modem_general_cnf_t;
/*========================================================================
 *  function Definition
 *========================================================================*/
typedef void (*yopen_callback)(void *ctx);

/*
uint32_t yopen_get_free_heap_size(void);
uint32_t yopen_get_total_heap_size(void);
uint32_t yopen_flash_erase(uint32_t SectorAddress, uint32_t size);
uint32_t yopen_flash_read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint32_t yopen_flash_write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
*/
#ifdef __cplusplus
} /*"C" */
#endif

#endif /* YOPEN_API_COMMON_H */

