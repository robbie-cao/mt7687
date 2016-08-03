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
 * This file implements PXp app main function
 *
 ****************************************************************************/\
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_events.h"
#include "ble_message.h"
#include "ble_gatts_srv_common.h"
#include "syslog.h"

#include "bt_gap.h"
#include "bt_events.h"
#include <string.h>


/********************************************************
 * Function
 *
 ********************************************************/
 
log_create_module(BLE_STATIC_CB, PRINT_LEVEL_INFO);


/**
 * @brief Function for distinguishing a BLE stack event to all modules with a BLE stack Gatt event handler.
 *
 * @param[in] event_id Bluetooth stack event. 
 *
 * @param[in] *param   Bluetooth stack event parameters.
 */
void ble_gatts_common_callback(ble_event_t event_id, const void *param)
{
    LOG_I(BLE_STATIC_CB, "ble_gatts_common_callback: event[%d] \n", event_id);

    switch (event_id) {	
        case BLE_GATT_EXCHANGE_MTU_REQ:
        {  
            ble_gatt_exchange_mtu_req_t *mtu_change = ble_gatts_alloc_memory(sizeof(ble_gatt_exchange_mtu_req_t));
            memcpy(mtu_change, param, sizeof(ble_gatt_exchange_mtu_req_t));            
            LOG_I(BLE_STATIC_CB, "ble_gatts_common_callback: expected_mtu[%d] \n", mtu_change->expected_mtu);
            ble_message_send(BLE_MSG_GATT_EXCHANGE_MTU_REQ_IND, mtu_change);
        }
            break;
        case BLE_GATTS_READ_REQ:
        {
            ble_gatts_read_req_t *temp_req = (ble_gatts_read_req_t *)param;
            ble_gatts_read_req_t *read_req = ble_gatts_alloc_memory(sizeof(ble_gatts_read_req_t));
            memcpy(read_req, param, sizeof(ble_gatts_read_req_t));

            read_req->handle = (uint16_t *)ble_gatts_alloc_memory(sizeof(uint16_t) * read_req->count);
            memcpy(read_req->handle, temp_req->handle, (sizeof(uint16_t) * read_req->count));
            ble_message_send(BLE_MSG_GATTS_READ_REQ_IND, read_req); 
        }
            break;	
        case BLE_GATTS_WRITE_REQ:
        {    
            ble_gatts_write_req_t *temp_data = (ble_gatts_write_req_t *)param;
            ble_gatts_write_req_t *write_req = ble_gatts_alloc_memory(sizeof(ble_gatts_write_req_t));
            memcpy(write_req, param, sizeof(ble_gatts_write_req_t));

            write_req->data = (uint8_t *)ble_gatts_alloc_memory(sizeof(uint8_t) * write_req->length);
            memcpy(write_req->data, temp_data->data, (sizeof(uint8_t) * write_req->length));
            ble_message_send(BLE_MSG_GATTS_WRITE_REQ_IND, write_req);
        }
            break;	
        case BLE_GATTS_EXECUTE_WRITE_REQ:
        {    
            ble_gatts_execute_write_req_t *excute_write_req = ble_gatts_alloc_memory(sizeof(ble_gatts_execute_write_req_t));
            memcpy(excute_write_req, param, sizeof(ble_gatts_execute_write_req_t));
            ble_message_send(BLE_MSG_GATTS_EXCUTE_WRITE_REQ_IND, excute_write_req); 
        }
            break;
        case BLE_GATTS_HANDLE_VALUE_CNF:
        {
            ble_gatts_handle_value_cnf_t *hvf_ind = ble_gatts_alloc_memory(sizeof(ble_gatts_handle_value_cnf_t));
            memcpy(hvf_ind, param, sizeof(ble_gatts_handle_value_cnf_t));
            ble_message_send(BLE_MSG_GATTS_HANDLE_VALUE_IND, hvf_ind); 
        }
            break;					
        default:
            break;	
    }	
}

/**
 * @brief Function for distinguishing a BLE stack event to all modules with a BLE stack Gap event handler.
 *
 * @param[in] event_id Bluetooth stack event. 
 *
 * @param[in] *param   Bluetooth stack event parameters.
 */
void ble_gap_common_callback(ble_event_t event_id, const void *param)
{
    LOG_I(BLE_STATIC_CB, "ble_gap_common_callback: event[%d] \n", event_id);

    switch (event_id) {
        case BLE_GAP_CONNECTED_IND:
        {
            ble_gap_connected_ind_t *connected = ble_gatts_alloc_memory(sizeof(ble_gap_connected_ind_t)); 
            memcpy(connected, param, sizeof(ble_gap_connected_ind_t));
            ble_message_send(BLE_MSG_GAP_CONNECTED_IND, connected);  
        }
            break;
        case BLE_GAP_DISCONNECTED_IND:
        {
            ble_gap_disconnected_ind_t *disconnected = ble_gatts_alloc_memory(sizeof(ble_gap_disconnected_ind_t)); 
            memcpy(disconnected, param, sizeof(ble_gap_disconnected_ind_t));
            ble_message_send(BLE_MSG_GAP_DISCONNECTED_IND, disconnected);    
        }
            break;
        case BLE_GAP_REPORT_RSSI_IND:
        {
            ble_gap_report_rssi_ind_t *rssi_value = ble_gatts_alloc_memory(sizeof(ble_gap_report_rssi_ind_t)); 
            memcpy(rssi_value, param, sizeof(ble_gap_report_rssi_ind_t));
            ble_message_send(BLE_MSG_GAP_REPORT_RSSI_IND, rssi_value);     
        }
            break;
        case BLE_GAP_ADVERTISING_IND:
        {   
            ble_gap_advertising_ind_t *advertising = ble_gatts_alloc_memory(sizeof(ble_gap_advertising_ind_t)); 
            memcpy(advertising, param, sizeof(ble_gap_advertising_ind_t));
            ble_message_send(BLE_MSG_GAP_ADVERTISING_IND, advertising);      
        }
            break;	
        case BLE_GAP_CONNECTION_PARAM_UPDATED_IND:
        {
            ble_gap_connection_param_updated_ind_t *conn_params = ble_gatts_alloc_memory(sizeof(ble_gap_connection_param_updated_ind_t)); 
            memcpy(conn_params, param, sizeof(ble_gap_connection_param_updated_ind_t));
            ble_message_send(BLE_MSG_GAP_CONNECTION_PARAM_UPDATED_IND, conn_params);     
        }
            break;
        default:
            break;
    }	
}

void bt_gap_common_callback(bt_event_t event_id, void *param)
{
    switch (event_id) {
        case BT_GAP_POWER_ON_CNF:
        {
            ble_power_on_cnf_t *power_on = ble_gatts_alloc_memory(sizeof(ble_power_on_cnf_t));
            power_on->result = 0;
            ble_message_send(BLE_MSG_GAP_POWER_ON_IND, power_on);   
            LOG_I(BLE_STATIC_CB, "bt_gap_common_callback: power on\r\n");
        }
            break;
        case BT_GAP_POWER_OFF_CNF:
        {
            ble_power_off_cnf_t *power_off = ble_gatts_alloc_memory(sizeof(ble_power_off_cnf_t));            
            power_off->result = 0;
            ble_message_send(BLE_MSG_GAP_POWER_OFF_IND, power_off);  
            
            LOG_I(BLE_STATIC_CB, "bt_gap_common_callback: power off\r\n");
        }
            break;
        default :
            break;
    }
}



