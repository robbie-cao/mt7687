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
 * This file implements Heart Rate service main function
 *
 ****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_hrs.h"
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"
#include <string.h>


#define ATTRIBUTE_HANDLE_OFFSET 3              /** < A Offset of the Whole handle numbles in one service. */

uint16_t MAX_HRS_HEART_RATE_MEASUREMENT_LEN;   /**< Maximum size of a transmitted Heart Rate Measurement. */ 
uint16_t MAX_HRS_BODY_SENSOR_LOCATION_LEN;     /**< Maximum size of a transmitted Body Sensor Location. */ 
uint16_t g_att_handle = 0;


/**
 * @brief Function for encoding Flags.
 *
 * @param[in]   p_flags           Flags structure to be encoded.
 * @param[out]  p_encoded_buffer  Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t ble_hrs_heart_rate_measurement_flags_encode(uint8_t *encoded_buffer, ble_hrs_heart_rate_measurement_flags_t *p_flags)
{
    uint8_t flags = 0;
    flags = flags | (p_flags->heart_rate_value_format_bit << 0); 
    flags = flags | (p_flags->sensor_contact_status_bits << 1); 
    flags = flags | (p_flags->energy_expended_status_bit << 3); 
    flags = flags | (p_flags->rr_interval_bit << 4); 
    encoded_buffer[0] = flags;
    return 1; 
}

/**
 * @brief Function for encoding Heart Rate Measurement.
 *
 * @param[in]   p_heart_rate_measurement    Heart Rate Measurement characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer            Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t heart_rate_measurement_encode(uint8_t *encoded_buffer, ble_hrs_heart_rate_measurement_t *p_heart_rate_measurement)
{
    uint8_t len = 0; 
    bool C1_required = (p_heart_rate_measurement->flags.heart_rate_value_format_bit == BLE_HRS_HEART_RATE_VALUE_FORMAT_BIT_HEART_RATE_VALUE_FORMAT_IS_SET_TO_UINT8__UNITS__BEATS_PER_MINUTE__BPM_); 
    bool C2_required = (p_heart_rate_measurement->flags.heart_rate_value_format_bit == BLE_HRS_HEART_RATE_VALUE_FORMAT_BIT_HEART_RATE_VALUE_FORMAT_IS_SET_TO_UINT16__UNITS__BEATS_PER_MINUTE__BPM_); 
    bool C3_required = (p_heart_rate_measurement->flags.energy_expended_status_bit == BLE_HRS_ENERGY_EXPENDED_STATUS_BIT_ENERGY_EXPENDED_FIELD_IS_PRESENT__UNITS__KILO_JOULES); 
    bool C4_required = (p_heart_rate_measurement->flags.rr_interval_bit == BLE_HRS_RR_INTERVAL_BIT_ONE_OR_MORE_RR_INTERVAL_VALUES_ARE_PRESENT__UNITS__1_1024_SECONDS); 
    len += ble_hrs_heart_rate_measurement_flags_encode(&encoded_buffer[len], &p_heart_rate_measurement->flags); 
    if (C1_required) {
        encoded_buffer[len++] = p_heart_rate_measurement->heart_rate_measurement_value__uint8_;
    } 
    if (C2_required) {
        len += bds_uint16_encode(&encoded_buffer[len], &p_heart_rate_measurement->heart_rate_measurement_value__uint16_); 
    } 
    if (C3_required) {
        len += bds_uint16_encode(&encoded_buffer[len], &p_heart_rate_measurement->energy_expended); 
    } 
    if (C4_required) {
        len += bds_uint16_encode(&encoded_buffer[len], &p_heart_rate_measurement->rr_interval); 
    } 
    return len;
}

/**
 * @brief Function for encoding Body Sensor Location.
 *
 * @param[in]   p_body_sensor_location  Body Sensor Location structure to be encoded.
 * @param[out]  p_encoded_buffer   		Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t ble_hrs_body_sensor_location_body_sensor_location_encode(uint8_t *encoded_buffer, ble_hrs_body_sensor_location_body_sensor_location_t *p_body_sensor_location)
{
    uint8_t body_sensor_location = 0;
    body_sensor_location = p_body_sensor_location->body_sensor_location;
    encoded_buffer[0] = body_sensor_location;
    return 1; 
}

/**
 * @brief Function for encoding Body Sensor Location.
 *
 * @param[in]   p_body_sensor_location   Body Sensor Location characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer         Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t body_sensor_location_encode(uint8_t *encoded_buffer, ble_hrs_body_sensor_location_t *p_body_sensor_location)
{
    uint8_t len = 0; 
    len += ble_hrs_body_sensor_location_body_sensor_location_encode(&encoded_buffer[len], &p_body_sensor_location->body_sensor_location); 
    return len;
}

/**
 * @brief Function for decoding Heart Rate Control Point.
 *
 * @param[in]   data_len              Length of the field to be decoded.
 * @param[in]   p_data                Buffer where the encoded data is stored.
 * @param[out]  p_write_val           Decoded data.
 *
 * @return      Length of the decoded field.
 */
 static uint8_t ble_hrs_heart_rate_control_point_heart_rate_control_point_decode(ble_hrs_heart_rate_control_point_heart_rate_control_point_t *p_write_val, uint8_t *p_data, uint8_t data_len)
 {
    uint8_t pos = 0; 
    p_write_val->heart_rate_control_point = (ble_hrs_enum_heart_rate_control_point_t)p_data[pos++]; 
    return pos;
 }
/**
 * @brief Function for decoding Heart Rate Control Point.
 *
 * @param[in]   data_len              Length of the field to be decoded.
 * @param[in]   p_data                Buffer where the encoded data is stored.
 * @param[out]  p_write_val           Decoded data.
 *
 * @return      Length of the decoded field.
 */
static uint8_t heart_rate_control_point_decode(ble_hrs_heart_rate_control_point_t *p_write_val, uint8_t *p_data, uint8_t data_len)
{
    uint8_t pos = 0;
    pos += ble_hrs_heart_rate_control_point_heart_rate_control_point_decode(&p_write_val->heart_rate_control_point, &p_data[pos], (data_len-pos)); 
    return pos;
} 

/**
 * @brief Function for handling the Write event.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_hrs_on_write(ble_hrs_t *p_hrs, ble_gatts_write_req_t *p_ble_evt)
{ 
    if (p_ble_evt->handle == p_hrs->heart_rate_measurement_handles.cccd_handle) {
        if (p_hrs->evt_handler != NULL) {
            ble_hrs_evt_t evt;
            memset(&evt,0,sizeof(ble_hrs_evt_t));
            evt.conn_id = p_ble_evt->conn_id;
            evt.evt_type = BLE_HRS_HEART_RATE_MEASUREMENT_EVT_CCCD_WRITE;
            bds_uint16_decode(&evt.params.heart_rate_measurement_cccd_value, p_ble_evt->data, p_ble_evt->length);
            p_hrs->evt_handler(p_hrs, &evt);			
            if (BLE_GATT_WRITE_REQUEST == p_ble_evt->flags) {
                if (true == p_ble_evt->is_prepare) {
                    ble_gatts_prepare_write_response(p_ble_evt->conn_id, BLE_STATUS_GATT_SUCCESS, p_ble_evt->handle, p_ble_evt->offset);		
                } else {
                    ble_gatts_write_response(p_ble_evt->conn_id, BLE_STATUS_GATT_SUCCESS);
                }
            }
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
    if (p_ble_evt->handle == p_hrs->heart_rate_control_point_handles.value_handle) {
        if (p_hrs->evt_handler != NULL) {
            ble_hrs_evt_t evt;
            memset(&evt,0,sizeof(ble_hrs_evt_t));
            evt.conn_id = p_ble_evt->conn_id;
            evt.evt_type = BLE_HRS_HEART_RATE_CONTROL_POINT_EVT_WRITE;
            evt.params.heart_rate_control_point_is_prepare = false;
            evt.params.heart_rate_control_point_write_offset = p_ble_evt->offset;
            evt.params.heart_rate_control_point_write_length = p_ble_evt->length; 
            evt.params.heart_rate_control_point_write_long_data = NULL;

            if (BLE_GATT_WRITE_REQUEST == p_ble_evt->flags) {
                if (true == p_ble_evt->is_prepare) {
                    g_att_handle = p_hrs->heart_rate_control_point_handles.value_handle;
                    evt.params.heart_rate_control_point_is_prepare = true;
                    evt.params.heart_rate_control_point_write_long_data = p_ble_evt->data;
                    ble_gatts_prepare_write_response(p_ble_evt->conn_id, BLE_STATUS_GATT_SUCCESS, p_ble_evt->handle, p_ble_evt->offset);		
                } else {
                    heart_rate_control_point_decode(&evt.params.heart_rate_control_point, p_ble_evt->data, p_ble_evt->length);
                    ble_gatts_write_response(p_ble_evt->conn_id, BLE_STATUS_GATT_SUCCESS);
                }
            } else {
                heart_rate_control_point_decode(&evt.params.heart_rate_control_point, p_ble_evt->data, p_ble_evt->length);
            }
            p_hrs->evt_handler(p_hrs, &evt);
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
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_hrs_on_excute_write(ble_hrs_t *p_hrs, ble_gatts_execute_write_req_t *p_ble_evt)
{ 	
    if (p_hrs->evt_handler != NULL) {
        if (true == p_ble_evt->is_execute) {
            ble_hrs_evt_t evt;
            memset(&evt,0,sizeof(ble_hrs_evt_t));
            evt.conn_id = p_ble_evt->conn_id;
            evt.evt_type = BLE_HRS_HEART_RATE_CONTROL_POINT_EVT_EXCUTE_WRITE;
            evt.params.heart_rate_control_point_is_excute = true;
            ble_gatts_execute_write_response(p_ble_evt->conn_id, BLE_STATUS_GATT_SUCCESS, g_att_handle); 
        }
    }
}

/**
 * @brief Function for handling the Read event.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_hrs_on_read(ble_hrs_t *p_hrs, ble_gatts_read_req_t *p_ble_evt)
{ 
    /**< read Multiple characteristic values. */
    if (p_ble_evt->count > 1) {
        if (p_hrs->evt_handler != NULL) {
            ble_hrs_evt_t evt;
            memset(&evt,0,sizeof(ble_hrs_evt_t));
            evt.conn_id = p_ble_evt->conn_id;
            evt.evt_type = BLE_HRS_EVT_READ_MULTIPLE;
            evt.params.handle_list = p_ble_evt->handle;
            p_hrs->evt_handler(p_hrs, &evt);		
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read descriptor 2902. */
        if (*p_ble_evt->handle == p_hrs->heart_rate_measurement_handles.cccd_handle) {
            if (p_hrs->evt_handler != NULL) {
                ble_hrs_evt_t evt;
                memset(&evt,0,sizeof(ble_hrs_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_HRS_HEART_RATE_MEASUREMENT_EVT_CCCD_READ;
                p_hrs->evt_handler(p_hrs, &evt);		
            }
        }
    } 
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_hrs->body_sensor_location_handles.value_handle) {
            if (p_hrs->evt_handler != NULL) {
                ble_hrs_evt_t evt;
                memset(&evt,0,sizeof(ble_hrs_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_HRS_BODY_SENSOR_LOCATION_EVT_READ;
                evt.params.body_sensor_location_read_offset = p_ble_evt->offset;/**default is 0*/
                p_hrs->evt_handler(p_hrs, &evt);		
            }
        }
    }
}

/**
 * @brief Function for handling the indication confirm event.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_hrs_on_hvc_confirm(ble_hrs_t *p_hrs, ble_gatts_handle_value_cnf_t *p_ble_evt)
{ 
    /**< No implementation needed. */
}

/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
void ble_hrs_on_ble_evt(ble_hrs_t *p_hrs, ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTS_READ_REQ:
            ble_hrs_on_read(p_hrs, &(p_ble_evt->evt.gatts_evt.read));
            break;

        case BLE_GATTS_WRITE_REQ:
            ble_hrs_on_write(p_hrs, &(p_ble_evt->evt.gatts_evt.write));
            break;

        case BLE_GATTS_EXECUTE_WRITE_REQ:
            ble_hrs_on_excute_write(p_hrs, &(p_ble_evt->evt.gatts_evt.excute_write));
            break;

        case BLE_GATTS_HANDLE_VALUE_CNF:
            ble_hrs_on_hvc_confirm(p_hrs, &(p_ble_evt->evt.gatts_evt.confirm));
            break;	

        default:
            break;
    }
}

/**
 * @brief Function for initializing the Heart Rate.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_hrs_init  Heart Rate Service init structure.
 *
 * @return      None.
 */
ble_status_t ble_hrs_init(ble_hrs_t *p_hrs, const ble_hrs_init_t *p_hrs_init)
{
    ble_status_t err_code = BLE_STATUS_GATT_FAILED;
    uint16_t hrs_handle_num = 0;
    ble_gatt_service_t hrs_t;
    uint8_t hrs_uuid[2] = {0};
    //uint8_t hrs_uuid128[16] = {0};
    uint16_t hrs_uuid16 = 0;

    /**< Initialize service structure. */
    p_hrs->evt_handler = p_hrs_init->evt_handler;

    /**< Add service. */
    hrs_uuid16 = 0x180D;
    bds_uint16_encode(hrs_uuid, &hrs_uuid16);
    ble_gatt_uuid128_generate(hrs_t.uuid, NULL, hrs_uuid);
    hrs_handle_num = 2 * 3 + ATTRIBUTE_HANDLE_OFFSET;

    err_code = ble_gatts_add_service(BLE_GATT_SERVICE_PRIMARY, hrs_t.uuid, hrs_handle_num, &(p_hrs->service_handle));
    if (err_code != BLE_STATUS_SUCCESS) {
        return err_code;
    }
    {
        /**< Add Heart Rate Measurement characteristic. */
        ble_gatt_char_t heart_rate_measurement_t;
        uint32_t heart_rate_measurement_permission = 0;
        memset(&heart_rate_measurement_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t heart_rate_measurement_uuid[2] = {0};
            uint16_t heart_rate_measurement_uuid16 = 0x2A37;
            bds_uint16_encode(heart_rate_measurement_uuid, &heart_rate_measurement_uuid16);
            ble_gatt_uuid128_generate(heart_rate_measurement_t.uuid, NULL, heart_rate_measurement_uuid);
        }
        heart_rate_measurement_t.properties = 0;
        heart_rate_measurement_t.properties = heart_rate_measurement_t.properties | BLE_GATT_CHAR_PROP_NOTIFY; 

        err_code = ble_gatts_add_char(p_hrs->service_handle, &(heart_rate_measurement_t), heart_rate_measurement_permission, &(p_hrs->heart_rate_measurement_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add Client Characteristic Configuration descriptor. */
        ble_gatt_descriptor_t client_characteristic_configuration_t;
        uint32_t client_characteristic_configuration_permission = 0;
        uint8_t client_characteristic_configuration_uuid[2] = {0};
        uint16_t client_characteristic_configuration_uuid16 = 0x2902;
        memset(&client_characteristic_configuration_t, 0, sizeof(ble_gatt_descriptor_t));
        bds_uint16_encode(client_characteristic_configuration_uuid, &client_characteristic_configuration_uuid16); 
        ble_gatt_uuid128_generate(client_characteristic_configuration_t.uuid, NULL, client_characteristic_configuration_uuid);

        if (BLE_UUID_CHAR_DESC_CHAR_EXTEND_PROPERTY == client_characteristic_configuration_uuid16) {
            err_code = ble_gatts_add_descriptor(p_hrs->service_handle, &(client_characteristic_configuration_t), client_characteristic_configuration_permission, &(p_hrs->heart_rate_measurement_handles.cepd_handle));
        } else if (BLE_UUID_CHAR_DESC_CHAR_USER_DESC == client_characteristic_configuration_uuid16) {
            err_code = ble_gatts_add_descriptor(p_hrs->service_handle, &(client_characteristic_configuration_t), client_characteristic_configuration_permission, &(p_hrs->heart_rate_measurement_handles.cudd_handle));
        } else if (BLE_UUID_CHAR_DESC_CLIENT_CHAR_CONFIG == client_characteristic_configuration_uuid16) {
            client_characteristic_configuration_permission = client_characteristic_configuration_permission | BLE_GATT_PERM_WRITE;
            client_characteristic_configuration_permission = client_characteristic_configuration_permission | BLE_GATT_PERM_READ;
            err_code = ble_gatts_add_descriptor(p_hrs->service_handle, &(client_characteristic_configuration_t), client_characteristic_configuration_permission, &(p_hrs->heart_rate_measurement_handles.cccd_handle));
        } else if (BLE_UUID_CHAR_DESC_SERVER_CHAR_CONFIG == client_characteristic_configuration_uuid16) {
            err_code = ble_gatts_add_descriptor(p_hrs->service_handle, &(client_characteristic_configuration_t), client_characteristic_configuration_permission, &(p_hrs->heart_rate_measurement_handles.sccd_handle));
        }
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        }
    }
    {
        /**< Add Body Sensor Location characteristic. */
        ble_gatt_char_t body_sensor_location_t;
        uint32_t body_sensor_location_permission = 0;
        memset(&body_sensor_location_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t body_sensor_location_uuid[2] = {0};
            uint16_t body_sensor_location_uuid16 = 0x2A38;
            bds_uint16_encode(body_sensor_location_uuid, &body_sensor_location_uuid16);
            ble_gatt_uuid128_generate(body_sensor_location_t.uuid, NULL, body_sensor_location_uuid);
        }
        body_sensor_location_t.properties = 0;
        body_sensor_location_t.properties = body_sensor_location_t.properties | BLE_GATT_CHAR_PROP_READ; 

        body_sensor_location_permission = body_sensor_location_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_hrs->service_handle, &(body_sensor_location_t), body_sensor_location_permission, &(p_hrs->body_sensor_location_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add Heart Rate Control Point characteristic. */
        ble_gatt_char_t heart_rate_control_point_t;
        uint32_t heart_rate_control_point_permission = 0;
        memset(&heart_rate_control_point_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t heart_rate_control_point_uuid[2] = {0};
            uint16_t heart_rate_control_point_uuid16 = 0x2A39;
            bds_uint16_encode(heart_rate_control_point_uuid, &heart_rate_control_point_uuid16);
            ble_gatt_uuid128_generate(heart_rate_control_point_t.uuid, NULL, heart_rate_control_point_uuid);
        }
        heart_rate_control_point_t.properties = 0;
        heart_rate_control_point_t.properties = heart_rate_control_point_t.properties | BLE_GATT_CHAR_PROP_WRITE;

        heart_rate_control_point_permission = heart_rate_control_point_permission | BLE_GATT_PERM_WRITE;
        err_code = ble_gatts_add_char(p_hrs->service_handle, &(heart_rate_control_point_t), heart_rate_control_point_permission, &(p_hrs->heart_rate_control_point_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    return BLE_STATUS_SUCCESS;
}

/**
 * @brief Function for reading response the Heart Rate Measurement descriptor 0x2902.
 *
 * @param[in]   p_hrs         Heart Rate Service structure.
 * @param[in]   cccd_value    the value of descriptor 0x2902.
 *
 * @return      ble_status_t  0 means success.
 */
ble_status_t ble_hrs_heart_rate_measurement_cccd_read_rsp(ble_hrs_t *p_hrs, uint16_t conn_id, uint16_t cccd_value)
{
    ble_status_t err_code;
    if (conn_id != 0) {
        uint8_t *encoded_value = NULL;
        uint16_t read_handle = 0;

        /**< Compose value struct. */
        MAX_HRS_HEART_RATE_MEASUREMENT_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_HRS_HEART_RATE_MEASUREMENT_LEN);
        memset(encoded_value, 0, MAX_HRS_HEART_RATE_MEASUREMENT_LEN);
        bds_uint16_encode(encoded_value, &cccd_value); 
        read_handle = p_hrs->heart_rate_measurement_handles.cccd_handle;

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, read_handle, 0, BLE_CCCD_VALUE_LEN, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}

/**
 * @brief Function for sending the Heart Rate Measurement.
 *
 * @param[in]   p_hrs                     Heart Rate Service structure.
 * @param[in]   p_heart_rate_measurement  Heart Rate Characteristic structure.
 *
 * @return      ble_status_t              0 means success.
 */
ble_status_t ble_hrs_heart_rate_measurement_send(ble_hrs_t *p_hrs, ble_hrs_heart_rate_measurement_t *p_heart_rate_measurement, uint16_t conn_id)
{
    ble_status_t err_code;

    if (conn_id != 0) { 
        bool need_confirm = false;
        uint8_t *encoded_value = NULL;
        uint16_t hvx_len = 0;
        uint16_t hvx_handle = 0;

        /**< Compose value struct. */
        MAX_HRS_HEART_RATE_MEASUREMENT_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_HRS_HEART_RATE_MEASUREMENT_LEN);
        memset(encoded_value, 0, MAX_HRS_HEART_RATE_MEASUREMENT_LEN);
        hvx_len = heart_rate_measurement_encode(encoded_value, p_heart_rate_measurement);
        hvx_handle = p_hrs->heart_rate_measurement_handles.value_handle;

        err_code = ble_gatts_send_handle_value_indication(conn_id, hvx_handle, need_confirm, hvx_len, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 * @brief Function for reading response the Body Sensor Location.
 *
 * @param[in]   p_hrs                   Heart Rate Service structure.
 * @param[in]   p_body_sensor_location  Heart Rate Characteristic structure.
 * @param[in]   offset                  client role read offset for read long.
 *
 * @return      ble_status_t            BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_hrs_body_sensor_location_read_rsp(ble_hrs_t *p_hrs, ble_hrs_body_sensor_location_t *p_body_sensor_location, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct. */
        MAX_HRS_BODY_SENSOR_LOCATION_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_HRS_BODY_SENSOR_LOCATION_LEN);
        memset(encoded_value, 0, MAX_HRS_BODY_SENSOR_LOCATION_LEN);
        length = body_sensor_location_encode(encoded_value, p_body_sensor_location);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_hrs->body_sensor_location_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}

/**
 * @brief Function for response the hrs read multiple characteristic event, if your app not care read multiple, please do not call this API.
 *
 * @details Sets a set of characteristic value of the hrs. The values will be sent
 *          to the client by read multiple characteristic response.
 *
 * @param[in] p_hrs            Heart Rate Service structure.
 * @param[in] uint8_t *data    data to send                 
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_hrs_read_multiple_rsp(ble_hrs_t *p_hrs, uint16_t conn_id, uint16_t length, const uint8_t *data)
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








