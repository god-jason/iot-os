/******************************************************************************
* Copyright (C) 2004 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xemaclite.h
* @{
* @details
*
* The Xilinx Ethernet Lite (EthLite) driver. This driver supports the Xilinx
* Ethernet Lite 10/100 MAC (EthLite).
*
* The Xilinx Ethernet Lite 10/100 MAC supports the following features:
*   - Media Independent Interface (MII) for connection to external
*     10/100 Mbps PHY transceivers
*   - Independent internal transmit and receive buffers
*   - CSMA/CD compliant operations for half-duplex modes
*   - Unicast and broadcast
*   - Automatic FCS insertion
*   - Automatic pad insertion on transmit
*   - Configurable ping/pong buffers for either/both transmit and receive
*     buffer areas
*   - Interrupt driven mode
*   - Internal loop back
*   - MDIO Support to access PHY Registers
*
* The Xilinx Ethernet Lite 10/100 MAC does not support the following features:
*   - multi-frame buffering
*     only 1 transmit frame is allowed into each transmit buffer,
*     only 1 receive frame is allowed into each receive buffer.
*     the hardware blocks reception until buffer is emptied
*   - Pause frame (flow control) detection in full-duplex mode
*   - Programmable inter frame gap
*   - Multicast and promiscuous address filtering
*   - Automatic source address insertion or overwrite
*
*
******************************************************************************/
#ifndef DRV_ETHLITE_H		/* prevent circular inclusions */
#define DRV_ETHLITE_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "drv_common.h"

/************************** Constant Definitions *****************************/
/*
 * Device information
 */
#define XEL_DEVICE_NAME	 "ethlite"
#define XEL_DEVICE_DESC	 "Ethernet Lite 10/100 MAC"

#define XPAR_XEMACLITE_NUM_INSTANCES 1

#define XPAR_EMACLITE_0_DEVICE_ID  240
#define XPAR_EMACLITE_0_BASEADDR    BASE_ETHLITE
#define XPAR_EMACLITE_0_TX_PING_PONG   1  /* Include TX Ping Pong buffers */
#define XPAR_EMACLITE_0_RX_PING_PONG   1  /* Include RX Ping Pong buffers */
#define XPAR_EMACLITE_0_INCLUDE_MDIO   0  /* Include MDIO support */
#define XPAR_EMACLITE_0_INCLUDE_INTERNAL_LOOPBACK 0 /* Include Internal */
#define XPAR_EMACLITE_0_IRQ_NUM    AP_INT_NUM_96    

#define XEL_HEADER_SIZE			14  /**< Size of header in bytes */
#define XEL_MTU_SIZE			1500 /**< Max size of data in frame */
#define XEL_FCS_SIZE			4    /**< Size of CRC */
                        
#define XEL_HEADER_OFFSET		12   /**< Offset to length field */
#define XEL_HEADER_SHIFT		16   /**< Right shift value to align length */

#define XEL_MAX_FRAME_SIZE (XEL_HEADER_SIZE+XEL_MTU_SIZE+ XEL_FCS_SIZE)	/**< Max
						length of Rx frame  used if
						length/type field
						contains the type (> 1500) */

#define XEL_MAX_TX_FRAME_SIZE (XEL_HEADER_SIZE + XEL_MTU_SIZE)	/**< Max
						length of Tx frame */


#define XEL_MAC_ADDR_SIZE		6	/**< length of MAC address */

/*
 * General Ethernet Definitions
 */
#define XEL_ETHER_PROTO_TYPE_IP		0x0800  /**< IP Protocol */
#define XEL_ETHER_PROTO_TYPE_ARP	0x0806  /**< ARP Protocol */
#define XEL_ETHER_PROTO_TYPE_VLAN	0x8100  /**< VLAN Tagged */
#define XEL_ARP_PACKET_SIZE		28  	/**< Max ARP packet size */
#define XEL_HEADER_IP_LENGTH_OFFSET	16  	/**< IP Length Offset */
#define XEL_VLAN_TAG_SIZE		4  	/**< VLAN Tag Size */


/**************************** Type Definitions *******************************/

/**
 * This typedef contains configuration information for a device.
 */
typedef struct {
	uint16_t DeviceId;	 /**< Unique ID  of device */
	uintptr_t BaseAddress; /**< Device base address */
	uint8_t TxPingPong;	 /**< 1 if TX Pong buffer configured, 0 otherwise */
	uint8_t RxPingPong;	 /**< 1 if RX Pong buffer configured, 0 otherwise */
	uint8_t MdioInclude;  /**< 1 if MDIO is enabled, 0 otherwise */
	uint8_t Loopback;     /**< 1 if internal loopback is enabled, 0 otherwise */
    uint8_t IrqNum;
} EthLite_Config;


/*
 * Callback when data is sent or received .
 * @param 	CallBackRef is a callback reference passed in by the upper layer
 *		when setting the callback functions, and passed back to the
 *		upper layer when the callback is invoked.
 */
typedef void (*EthLite_Handler) (void *CallBackRef);

/**
 * The EthLite driver instance data. The user is required to allocate a
 * variable of this type for every EthLite device in the system. A pointer
 * to a variable of this type is then passed to the driver API functions.
 */
typedef struct {
	EthLite_Config EthLiteConfig; /* Device configuration */
	uint32_t IsReady;			 /* Device is initialized and ready */

	uint32_t NextTxBufferToUse;		 /* Next TX buffer to write to */
	uint32_t NextRxBufferToUse;		 /* Next RX buffer to read from */

	/*
	 * Callbacks
	 */
	EthLite_Handler RecvHandler;
	void *RecvRef;
	EthLite_Handler SendHandler;
	void *SendRef;
} EthLite;

/***************** Macros (Inline Functions) Definitions *********************/

/****************************************************************************/
/**
*
* Return the next expected Transmit Buffer's address.
*
* @param	InstancePtr is the pointer to the instance of the driver to
*		be worked on
*
* @note		C-Style signature:
*		uint32_t EthLite_NextTransmitAddr(EthLite *InstancePtr);
*
* This macro returns the address of the next transmit buffer to put data into.
* This is used to determine the destination of the next transmit data frame.
*
*****************************************************************************/
#define EthLite_NextTransmitAddr(InstancePtr) 			\
	((InstancePtr)->EthLiteConfig.BaseAddress + 			\
		(InstancePtr)->NextTxBufferToUse)

/****************************************************************************/
/**
*
* Return the next expected Receive Buffer's address.
*
* @param	InstancePtr is the pointer to the instance of the driver to
*		be worked on
*
* @note		C-Style signature:
*		uint32_t EthLite_NextReceiveAddr(EthLite *InstancePtr);
*
* This macro returns the address of the next receive buffer to read data from.
* This is the expected receive buffer address if the driver is in sync.
*
*****************************************************************************/
#define EthLite_NextReceiveAddr(InstancePtr)				\
	((InstancePtr)->EthLiteConfig.BaseAddress + 			\
	(InstancePtr)->NextRxBufferToUse)

/*****************************************************************************/
/**
*
* This macro determines if the device is currently configured for MDIO.
*
* @param	InstancePtr is the pointer to the instance of the
*		EthLite driver.
*
* @return
* 		- TRUE if the device is configured for MDIO.
*		- FALSE if the device is NOT configured for MDIO.
*
* @note		C-Style signature:
*		int EthLite_IsMdioConfigured(EthLite *InstancePtr)
*
******************************************************************************/
#define EthLite_IsMdioConfigured(InstancePtr) 			\
	((InstancePtr)->EthLiteConfig.MdioInclude == 1)

/*****************************************************************************/
/**
*
* This macro determines if the device is currently configured for internal
* loopback.
*
* @param	InstancePtr is the pointer to the instance of the
*		EthLite driver.
*
* @return
* 		- TRUE if the device is configured for internal loopback.
*		- FALSE if the device is NOT configured for internal loopback.
*
* @note		C-Style signature:
*		int EthLite_IsLoopbackConfigured(EthLite *InstancePtr)
*
******************************************************************************/
#define EthLite_IsLoopbackConfigured(InstancePtr) 			\
            ((InstancePtr)->EthLiteConfig.Loopback == 1)

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/*
 * Functions in drv_ethlite.c
 */
void EthLite_SetMacAddress(EthLite *InstancePtr, uint8_t *AddressPtr);
int EthLite_TxBufferAvailable(EthLite *InstancePtr);
void EthLite_FlushReceive(EthLite *InstancePtr);

int EthLite_Send(EthLite *InstancePtr, uint8_t *FramePtr, unsigned ByteCount);
uint16_t EthLite_Recv(EthLite *InstancePtr, uint8_t *FramePtr);

int EthLite_PhyRead(EthLite *InstancePtr, uint32_t PhyAddress, uint32_t RegNum,
			uint16_t *PhyDataPtr);
int EthLite_PhyWrite(EthLite *InstancePtr, uint32_t PhyAddress, uint32_t RegNum,
			uint16_t PhyData);

/*
 * Initialization functions in xemaclite_sinit.c
 */
int EthLite_Initialize(EthLite *InstancePtr, uint16_t DeviceId);

/*
 * Interrupt driven functions in xemaclite_intr.c
 */
int EthLite_EnableInterrupts(EthLite *InstancePtr);
void EthLite_DisableInterrupts(EthLite *InstancePtr);

void EthLite_SetRecvHandler(EthLite *InstancePtr, void *CallBackRef,
			      EthLite_Handler FuncPtr);
void EthLite_SetSendHandler(EthLite *InstancePtr, void *CallBackRef,
			      EthLite_Handler FuncPtr);


#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */


/** @} */
