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
 * This file implements Tx Power service main function
 *
 ****************************************************************************/
#include "ble_tps.h"
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ble_gatt.h"
#include "ble_gap.h"


/************************************************
*   Macro
*************************************************/
#define ATTRIBUTE_HANDLE_OFFSET 3           /**< A Offset of the Whole handle numbles in one service. */

/************************************************
*   Global
*************************************************/
uint16_t MAX_TPS_TX_POWER_LEVEL_LEN;        /**< Maximum size of a transmitted Tx Power Level. */ 

/************************************************
*   Utilities
*************************************************/
/**
 * @brief Function for encoding Tx Power Level.
 *
 * @param[in]   p_tx_power_level    Tx Power Level characteristic structure to be encoded.
 * @param[out]  encoded_buffer    Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t tx_power_level_encode(uint8_t *encoded_buffer, ble_tps_tx_power_level_t *p_tx_power_level)
{
    uint8_t len = 0; 
    encoded_buffer[len++] = p_tx_power_level->tx_power;
    return len;
}

/**
 * @brief Function for handling the Write event.
 *
 * @param[in]   p_tps       Tx Power Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_tps_on_write(ble_tps_t *p_tps, ble_gatts_write_req_t*p_ble_evt)
{ 
    /**< No implementation needed. */
}

/**
 * @brief Function for handling the Excute Write event.
 *
 * @param[in]   p_tps       Tx Power Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_tps_on_excute_write(ble_tps_t *p_tps, ble_gatts_execute_write_req_t *p_ble_evt)
{
    /**< No implementation needed. */
}

/**
 * @brief Function for handling the Read event.
 *
 * @param[in]   p_tps       Tx Power Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_tps_on_read(ble_tps_t *p_tps, ble_gatts_read_req_t *p_ble_evt)
{ 
    /**< read Multiple characteristic values. */
    if (p_ble_evt->count > 1) {
        if (p_tps->evt_handler != NULL) {
            ble_tps_evt_t evt;
			memset(&evt,0,sizeof(ble_tps_evt_t));
            evt.conn_id = p_ble_evt->conn_id;
            evt.evt_type = BLE_TPS_EVT_READ_MULTIPLE;
			evt.params.handle_list = p_ble_evt->handle;
			p_tps->evt_handler(p_tps, &evt);		
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic , read long characteristic. */
	    if (*p_ble_evt->handle == p_tps->tx_power_level_handles.value_handle) {
            if (p_tps->evt_handler != NULL) {
                ble_tps_evt_t evt;
				memset(&evt,0,sizeof(ble_tps_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
	            evt.evt_type = BLE_TPS_TX_POWER_LEVEL_EVT_READ;
				evt.params.tx_power_level_read_offset = p_ble_evt->offset; /**< default is 0 */
				p_tps->evt_handler(p_tps, &evt);		
            }
        }
	}
}

/**
 * @brief Function for handling the indication confirm event.
 *
 * @param[in]   p_tps       Tx Power Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_tps_on_hvc_confirm(ble_tps_t *p_tps, ble_gatts_handle_value_cnf_t *p_ble_evt)
{ 
    /**< No implementation needed. */
}

/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_tps       Tx Power Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
void ble_tps_on_ble_evt(ble_tps_t *p_tps, ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id) {
	    case BLE_GATTS_READ_REQ:
	        ble_tps_on_read(p_tps, &(p_ble_evt->evt.gatts_evt.read));
	        break;
			
        case BLE_GATTS_WRITE_REQ:
            ble_tps_on_write(p_tps, &(p_ble_evt->evt.gatts_evt.write));
            break;
			
        case BLE_GATTS_EXECUTE_WRITE_REQ:
            ble_tps_on_excute_write(p_tps, &(p_ble_evt->evt.gatts_evt.excute_write));
            break;
			
	    case BLE_GATTS_HANDLE_VALUE_CNF:
            ble_tps_on_hvc_confirm(p_tps, &(p_ble_evt->evt.gatts_evt.confirm));
            break;	
			
        default:
            /**< No implementation needed. */
            break;
    }
}

/**
 * @brief Function for initializing the Tx Power.
 *
 * @param[in]   p_tps       Tx Power Service structure.
 * @param[in]   p_tps_init  Tx Power Service init structure.
 *
 * @return      None.
 */
ble_status_t ble_tps_init(ble_tps_t *p_tps, const ble_tps_init_t *p_tps_init)
{
    ble_status_t err_code = BLE_STATUS_GATT_FAILED;
    uint16_t tps_handle_num = 0;
    ble_gatt_service_t tps_t;
    uint8_t tps_uuid[2] = {0};
    uint16_t tps_uuid16 = 0;

    /**< Initialize service structure. */
    p_tps->evt_handler = p_tps_init->evt_handler;

	/**< Add service */
    tps_uuid16 = 0x1804;
    bds_uint16_encode(tps_uuid, &tps_uuid16);
    ble_gatt_uuid128_generate(tps_t.uuid, NULL, tps_uuid);
    tps_handle_num = 2 * 1 + ATTRIBUTE_HANDLE_OFFSET;
                
    err_code = ble_gatts_add_service(BLE_GATT_SERVICE_PRIMARY, tps_t.uuid, tps_handle_num, &(p_tps->service_handle));
    if (err_code != BLE_STATUS_SUCCESS) {
        return err_code;
    } 
    {
        /**< Add Tx Power Level characteristic. */
    	ble_gatt_char_t tx_power_level_t;
    	uint32_t tx_power_level_permission = 0;
    	memset(&tx_power_level_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t tx_power_level_uuid[2] = {0};
            uint16_t tx_power_level_uuid16 = 0x2A07;
            bds_uint16_encode(tx_power_level_uuid, &tx_power_level_uuid16);
            ble_gatt_uuid128_generate(tx_power_level_t.uuid, NULL, tx_power_level_uuid);
        }
	    tx_power_level_t.properties = 0;
	    tx_power_level_t.properties = tx_power_level_t.properties | BLE_GATT_CHAR_PROP_READ; 
		
		tx_power_level_permission = tx_power_level_permission | BLE_GATT_PERM_READ;
	    err_code = ble_gatts_add_char(p_tps->service_handle, &(tx_power_level_t), tx_power_level_permission, &(p_tps->tx_power_level_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
	        return err_code;
	    } 
    }
    return BLE_STATUS_SUCCESS;
}

/**
 * @brief Function for reading response the Tx Power Level.
 *
 * @param[in]   p_tps             Tx Power Service structure.
 * @param[in]   p_tx_power_level  Tx Power Characteristic structure.
 * @param[in]   offset            client role read offset for read long.
 * @param[in]   conn_id           Connection ID. 
 *
 * @return      ble_status_t      BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_tps_tx_power_level_read_rsp(ble_tps_t *p_tps, ble_tps_tx_power_level_t *p_tx_power_level, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct. */
        MAX_TPS_TX_POWER_LEVEL_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_TPS_TX_POWER_LEVEL_LEN);
        memset(encoded_value, 0, MAX_TPS_TX_POWER_LEVEL_LEN);
        length = tx_power_level_encode(encoded_value, p_tx_power_level);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_tps->tx_power_level_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}

/**
 * @brief Function for response the tps read multiple characteristic event, if your app not care read multiple, please do not call this API.
 *
 * @details Sets a set of characteristic value of the tps. The values will be sent
 *          to the client by read multiple characteristic response.
 *
 * @param[in] p_tps             Tx Power Service structure.
 * @param[in] uint8_t *data     data to send                       
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_tps_read_multiple_rsp(ble_tps_t *p_tps, uint16_t conn_id, uint16_t length, const uint8_t *data)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint8_t *encoded_value = NULL;
        uint16_t mtu_length = 0;
        
        /**< Compose value struct. */
        mtu_length = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(mtu_length);
        memset(encoded_value, 0, mtu_length);
        memcpy(encoded_value, data, length);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, 0, 0, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}








