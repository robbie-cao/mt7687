/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/*****************************************************************************
 *
 * Description:
 * ------------
 * This file implements BLE app messsage structures
 *
 ****************************************************************************/

#ifndef __BLE_MESSAGE_H__
#define __BLE_MESSAGE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/********************************************************
 * Macro & Define
 *
 ********************************************************/
#define BLE_MESSAGE_MAX_COUNT 30

/********************************************************
 * Enum & Structures
 *
 ********************************************************/
typedef enum{
    BLE_MSG_GATT_EXCHANGE_MTU_REQ_IND = 0x4900,/**< please not conflict to SDK event. */
    BLE_MSG_GATTS_WRITE_REQ_IND,
    BLE_MSG_GATTS_EXCUTE_WRITE_REQ_IND,
    BLE_MSG_GATTS_READ_REQ_IND,
    BLE_MSG_GATTS_HANDLE_VALUE_IND = 0x4904,
} ble_gatts_message_id_t;

typedef enum{
    BLE_MSG_GAP_POWER_ON_IND = 0x4905,/**< please not conflict to SDK event. */
    BLE_MSG_GAP_POWER_OFF_IND,
    BLE_MSG_GAP_CONNECTED_IND,
    BLE_MSG_GAP_REPORT_RSSI_IND,
    BLE_MSG_GAP_ADVERTISING_IND,
    BLE_MSG_GAP_CONNECTION_PARAM_UPDATED_IND,
    BLE_MSG_GAP_DISCONNECTED_IND = 0x490B,
} ble_gap_message_id_t;  

typedef struct{
    uint16_t message_id;
    void *parameter;
} ble_message_context_struct;

/********************************************************
 * External Function
 *
 ********************************************************/
void ble_message_create_queue(void);
void ble_message_free_message_buffer(ble_message_context_struct *msg_buffer);
void ble_message_send(uint16_t event, void *parameter);
bool ble_message_receive(ble_message_context_struct *message);
const ble_message_context_struct *ble_message_allocate_message_buffer(void);
extern void ble_gatts_message_handler(ble_gatts_message_id_t message_id, void *param);
extern void ble_gap_message_handler(ble_gap_message_id_t message_id, void *param);



#ifdef __cplusplus
}
#endif

 
#endif /**__BLE_MESSAGE_H__*/



