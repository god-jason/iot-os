/**
 ***********************************************************************************************************************
 * Copyright (c) 2022, 芯昇科技有限公司
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        drv_uicc.h
 *
 * @brief       Header of UICC driver.
 *
 * @par History:
 * 
 * Date              Author             Notes
 * 2022-04-06        ICT Team           First version
 * 
 *
 ***********************************************************************************************************************
 */

#ifndef _DRV_UICC_H
#define _DRV_UICC_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @addtogroup Uicc
 */

/**@{*/

typedef enum {
    UICC_OK                         = 0,        // 成功
    UICC_ERR                        = -1,       // 错误
    UICC_ERR_REJECT_CARD            = -2,       // 拒绝该卡的操作（复位）
    UICC_ERR_ALREADY_DEACTIVATED    = -3,       // 卡已经去激活
    UICC_ERR_RETRY                  = -4,       // 卡需要重试
    UICC_ERR_NOCARD                 = -5,       // 无卡
}UICC_ERR_CODE;

typedef enum {
    UICC_CARD_NUMBER_0,
    UICC_CARD_NUMBER_1,
    UICC_CARD_NUMBER_2,
    UICC_CARD_NUMBER_3
} UICC_CARD_NUMBER;

typedef enum {
    UICC_CMD_CASE_1,                /*!< No Tx nor Tx command. */
    UICC_CMD_CASE_2,                /*!< Rx command.           */
    UICC_CMD_CASE_3,                /*!< Tx command.           */
    UICC_CMD_CASE_4                 /*!< Tx and Rx command.    */
} UICC_CMD_CASE;

typedef enum {
    UICC_EVENT_CARD_INSERTED,
    UICC_EVENT_CARD_REMOVED
} UICC_EVENT;


 /**
 * \struct UICC_ApduHeader
 * Structure holding the APDU command header. 
 */
typedef struct {
    uint8_t  cla;               /*!< Command CLAss.       */
    uint8_t  ins;               /*!< Command INStruction. */
    uint8_t  p1;                /*!< Command Parameter.   */
    uint8_t  p2;                /*!< Command Parameter.   */
    uint16_t lc;                /*!< Tx-size. */
    uint16_t le;                /*!< Rx-Size. */
} UICC_ApduHeader;

/**
 * \struct UICC_ApduResponse
 * Structure holding the APDU command response 'header'. 
 */
typedef struct {
    uint16_t luicc;             /*!< Actual Rx-size recived. */
    uint8_t  sw1;               /*!< Received Status Word SW1. */
    uint8_t  sw2;               /*!< Received Status Word SW2. */
} UICC_ApduResponse;

typedef struct {
    UICC_ERR_CODE (*close)(UICC_CARD_NUMBER card_selector);
    UICC_ERR_CODE (*reset)(UICC_CARD_NUMBER card_selector);
    UICC_ERR_CODE (*xfer)(UICC_CARD_NUMBER card_selector, UICC_CMD_CASE command_case, bool extended_length,
                                            UICC_ApduHeader c_apdu, UICC_ApduResponse *r_apdu_ptr, uint8_t *apdu_data_ptr);
    UICC_ERR_CODE (*atr)(UICC_CARD_NUMBER card_selector, uint8_t *atr);
}UICC_VsimOps;

UICC_ERR_CODE UICC_VsimInit(UICC_CARD_NUMBER card_selector, UICC_VsimOps *func);
UICC_ERR_CODE UICC_VsimDeInit(UICC_CARD_NUMBER card_selector);

UICC_ERR_CODE UICC_Reset(UICC_CARD_NUMBER card_selector);
UICC_ERR_CODE UICC_Close(UICC_CARD_NUMBER card_selector);
UICC_ERR_CODE UICC_TransportApdu(UICC_CARD_NUMBER card_selector, UICC_CMD_CASE command_case, bool extended_length, 
                                 UICC_ApduHeader c_apdu, UICC_ApduResponse *r_apdu_ptr, uint8_t *apdu_data_ptr);

void UICC_RegisterCallback(void (*callback)(UICC_EVENT, UICC_CARD_NUMBER));
void UICC_UnRegisterCallback(void);

bool UICC_HotPlug_IsEnable(UICC_CARD_NUMBER card_selector);
void UICC_PM_PowerHoldEnable(UICC_CARD_NUMBER num, bool en);


#endif
/** @} */