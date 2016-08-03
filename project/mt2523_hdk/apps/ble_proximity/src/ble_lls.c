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
 * This file implements Link Loss service main function
 *
 ****************************************************************************/
 
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_lls.h"
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"

/************************************************
*   Macro
*************************************************/
#define ATTRIBUTE_HANDLE_OFFSET 3                      /**< A Offset of the Whole handle numbles in one service. */

/************************************************
*   Global
*************************************************/
uint16_t MAX_LLS_ALERT_LEVEL_LEN;                      /**< Maximum size of a transmitted Alert Level. */ 

extern uint8_t bds_uint16_encode(uint8_t *p_encoded_data, uint16_t *value);

/************************************************
*   Utilities
*************************************************/
/**
 * @brief Function for encoding Alert Level.
 *
 * @param[in]   p_alert_level    Alert Level structure to be encoded.
 * @param[out]  p_encoded_buffer Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t ble_lls_alert_level_alert_level_encode(uint8_t *encoded_buffer, ble_lls_alert_level_alert_level_t *p_alert_level)
{
    uint8_t alert_level = 0;
    alert_level = p_alert_level->alert_level;
    encoded_buffer[0] = alert_level;
    return 1; 
}

/**
 * @brief Function for encoding Alert Level.
 *
 * @param[in]   p_alert_level    Alert Level characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t alert_level_encode(uint8_t *encoded_buffer, ble_lls_alert_level_t *p_alert_level)
{
    uint8_t len = 0; 
    len += ble_lls_alert_level_alert_level_encode(&encoded_buffer[len], &p_alert_level->alert_level); 
    return len;
}

/**
 * @brief Function for decoding Alert Level.
 *
 * @param[in]   data_len              Length of the field to be decoded.
 * @param[in]   p_data                Buffer where the encoded data is stored.
 * @param[out]  p_write_val           Decoded data.
 *
 * @return      Length of the decoded field.
 */
 static uint8_t ble_lls_alert_level_alert_level_decode(ble_lls_alert_level_alert_level_t *p_write_val, uint8_t *p_data, uint8_t data_len)
 {
    uint8_t pos = 0; 
    p_write_val->alert_level = (ble_lls_enum_alert_level_t)p_data[pos++]; 
    return pos;
 }

/**
 * @brief Function for decoding Alert Level.
 *
 * @param[in]   data_len              Length of the field to be decoded.
 * @param[in]   p_data                Buffer where the encoded data is stored.
 * @param[out]  p_write_val           Decoded data.
 *
 * @return      Length of the decoded field.
 */
static uint8_t alert_level_decode(ble_lls_alert_level_t *p_write_val, uint8_t *p_data, uint8_t data_len)
{
    uint8_t pos = 0;
    pos += ble_lls_alert_level_alert_level_decode(&p_write_val->alert_level, &p_data[pos], (data_len-pos)); 
    return pos;
} 

/**
 * @brief Function for handling the Write event.
 *
 * @param[in]   p_lls       Link Loss Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_lls_on_write(ble_lls_t *p_lls, ble_gatts_write_req_t*p_ble_evt)
{ 
    if (p_ble_evt->handle == p_lls->alert_level_handles.value_handle) {
        if (p_lls->evt_handler != NULL) {
            ble_lls_evt_t evt;
            memset(&evt,0,sizeof(ble_lls_evt_t));
            evt.conn_id = p_ble_evt->conn_id;
            evt.evt_type = BLE_LLS_ALERT_LEVEL_EVT_WRITE;
            evt.params.alert_level_is_prepare = false;
            evt.params.alert_level_write_offset = p_ble_evt->offset;
            evt.params.alert_level_write_length = p_ble_evt->length; 
            evt.params.alert_level_write_long_data = NULL;

            if (BLE_GATT_WRITE_REQUEST == p_ble_evt->flags) {
                if (true == p_ble_evt->is_prepare) {
                    evt.params.alert_level_is_prepare = true;
                    evt.params.alert_level_write_long_data = p_ble_evt->data;
                    ble_gatts_prepare_write_response(p_ble_evt->conn_id, BLE_STATUS_GATT_SUCCESS, p_ble_evt->handle, p_ble_evt->offset);		
                } else {
                    alert_level_decode(&evt.params.alert_level, p_ble_evt->data, p_ble_evt->length);
                    ble_gatts_write_response(p_ble_evt->conn_id, BLE_STATUS_GATT_SUCCESS);
                }
            } else {
                alert_level_decode(&evt.params.alert_level, p_ble_evt->data, p_ble_evt->length);
            }
            p_lls->evt_handler(p_lls, &evt);
        } else {
            if (BLE_GATT_WRITE_REQUEST == p_ble_evt->flags) {
                if (true == p_ble_evt->is_prepare) {
                    ble_gatts_prepare_write_response(p_ble_evt->conn_id, BLE_STATUS_GATT_FAILED, p_ble_evt->handle, p_ble_evt->offset);		
                } else {
                    ble_gatts_write_response(p_ble_evt->conn_id, BLE_STATUS_GATT_FAILED);
                }
            }
        }
    }
}

/**
 * @brief Function for handling the Excute Write event.
 *
 * @param[in]   p_lls       Link Loss Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_lls_on_excute_write(ble_lls_t *p_lls, ble_gatts_execute_write_req_t *p_ble_evt)
{ 	

}

/**
 * @brief Function for handling the Read event.
 *
 * @param[in]   p_lls       Link Loss Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_lls_on_read(ble_lls_t *p_lls, ble_gatts_read_req_t *p_ble_evt)
{ 
    /**< read Multiple characteristic values. */
    if (p_ble_evt->count > 1) {
        if (p_lls->evt_handler != NULL) {
            ble_lls_evt_t evt;
            memset(&evt,0,sizeof(ble_lls_evt_t));
            evt.conn_id = p_ble_evt->conn_id;
            evt.evt_type = BLE_LLS_EVT_READ_MULTIPLE;
            evt.params.handle_list = p_ble_evt->handle;
            p_lls->evt_handler(p_lls, &evt);		
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic , read long characteristic. */
        if (*p_ble_evt->handle == p_lls->alert_level_handles.value_handle) {
            if (p_lls->evt_handler != NULL) {
                ble_lls_evt_t evt;
                memset(&evt,0,sizeof(ble_lls_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_LLS_ALERT_LEVEL_EVT_READ;
                evt.params.alert_level_read_offset = p_ble_evt->offset;/**< default is 0 */
                p_lls->evt_handler(p_lls, &evt);		
            }
        }
    }
}

/**
 * @brief Function for handling the indication confirm event.
 *
 * @param[in]   p_lls       Link Loss Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_lls_on_hvc_confirm(ble_lls_t *p_lls, ble_gatts_handle_value_cnf_t *p_ble_evt)
{ 
    /**< No implementation needed. */
}

/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_lls       Link Loss Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
void ble_lls_on_ble_evt(ble_lls_t *p_lls, ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTS_READ_REQ:
            ble_lls_on_read(p_lls, &(p_ble_evt->evt.gatts_evt.read));
            break;

        case BLE_GATTS_WRITE_REQ:
            ble_lls_on_write(p_lls, &(p_ble_evt->evt.gatts_evt.write));
            break;

        case BLE_GATTS_EXECUTE_WRITE_REQ:
            ble_lls_on_excute_write(p_lls, &(p_ble_evt->evt.gatts_evt.excute_write));
            break;

        case BLE_GATTS_HANDLE_VALUE_CNF:
            ble_lls_on_hvc_confirm(p_lls, &(p_ble_evt->evt.gatts_evt.confirm));
            break;	

        default:
            break;
    }
}

/**
 * @brief Function for initializing the Link Loss.
 *
 * @param[in]   p_lls       Link Loss Service structure.
 * @param[in]   p_lls_init  Link Loss Service init structure.
 *
 * @return      None.
 */
ble_status_t ble_lls_init(ble_lls_t *p_lls, const ble_lls_init_t *p_lls_init)
{
    ble_status_t err_code = BLE_STATUS_GATT_FAILED;
    uint16_t lls_handle_num = 0;
    ble_gatt_service_t lls_t;
    uint8_t lls_uuid[2] = {0};
    uint16_t lls_uuid16 = 0;

    /**< Initialize service structure. */
    p_lls->evt_handler = p_lls_init->evt_handler;

    /**< Add service. */
    lls_uuid16 = 0x1803;
    bds_uint16_encode(lls_uuid, &lls_uuid16);
    ble_gatt_uuid128_generate(lls_t.uuid, NULL, lls_uuid);
    lls_handle_num = 2 * 1 + ATTRIBUTE_HANDLE_OFFSET;

    err_code = ble_gatts_add_service(BLE_GATT_SERVICE_PRIMARY, lls_t.uuid, lls_handle_num, &(p_lls->service_handle));
    if (err_code != BLE_STATUS_SUCCESS) {
        return err_code;
    }
    {
        /**< Add Alert Level characteristic. */
        ble_gatt_char_t alert_level_t;
        uint32_t alert_level_permission = 0;
        memset(&alert_level_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t alert_level_uuid[2] = {0};
            uint16_t alert_level_uuid16 = 0x2A06;
            bds_uint16_encode(alert_level_uuid, &alert_level_uuid16);
            ble_gatt_uuid128_generate(alert_level_t.uuid, NULL, alert_level_uuid);
        }
        alert_level_t.properties = 0;
        alert_level_t.properties = alert_level_t.properties | BLE_GATT_CHAR_PROP_READ; 
        alert_level_t.properties = alert_level_t.properties | BLE_GATT_CHAR_PROP_WRITE;

        alert_level_permission = alert_level_permission | BLE_GATT_PERM_READ;
        alert_level_permission = alert_level_permission | BLE_GATT_PERM_WRITE;
        err_code = ble_gatts_add_char(p_lls->service_handle, &(alert_level_t), alert_level_permission, &(p_lls->alert_level_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    return BLE_STATUS_SUCCESS;
}

/**
 * @brief Function for reading response the Alert Level.
 *
 * @param[in]   p_lls          Link Loss Service structure.
 * @param[in]   p_alert_level  Link Loss Characteristic structure.
 * @param[in]   offset         client role read offset for read long.
 *
 * @return      ble_status_t   BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_lls_alert_level_read_rsp(ble_lls_t *p_lls, ble_lls_alert_level_t *p_alert_level, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct. */
        MAX_LLS_ALERT_LEVEL_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_LLS_ALERT_LEVEL_LEN);
        memset(encoded_value, 0, MAX_LLS_ALERT_LEVEL_LEN);
        length = alert_level_encode(encoded_value, p_alert_level);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_lls->alert_level_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}

/**
 * @brief Function for response the lls read multiple characteristic event, if your app not care read multiple, please do not call this API.
 *
 * @details Sets a set of characteristic value of the lls. The values will be sent
 *          to the client by read multiple characteristic response.
 *
 * @param[in] p_lls           Link Loss Service structure.
 * @param[in] uint8_t *data          data to send                 
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_lls_read_multiple_rsp(ble_lls_t *p_lls, uint16_t conn_id, uint16_t length, const uint8_t *data)
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








