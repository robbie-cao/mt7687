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
 *
 * Description:
 * ------------
 * This file implements DOGP service service main function
 *
 ****************************************************************************/

#include "ble_dogp_service.h"
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ble_gatt.h"
#include "ble_gap.h"
#include <string.h>

/************************************************
*   Macro
*************************************************/
#define ATTRIBUTE_HANDLE_OFFSET 3                   /** < A Offset of the Whole handle numbles in one service. */

/************************************************
*   Global
*************************************************/
uint16_t MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN;  /**< Maximum size of a transmitted DOGP service read characteristic. */
uint16_t MAX_DOGP_SERVICE_WRITE_CHARACTERISTIC_LEN; /**< Maximum size of a transmitted DOGP service write characteristic. */
uint16_t g_att_handle = 0;

/************************************************
*   Utilities
*************************************************/

/**
 * @brief Function for encoding DOGP service read characteristic.
 *
 * @param[in]   p_dogp_service_read_characteristic    DOGP service read characteristic characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer                      Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t dogp_service_read_characteristic_encode(uint8_t *encoded_buffer, ble_dogp_service_dogp_service_read_characteristic_t *p_dogp_service_read_characteristic)
{
    uint8_t len = 0;
    len += bds_ble_srv_utf8_str_encode(&encoded_buffer[len], &p_dogp_service_read_characteristic->read__field);
    return len;
}

/**
 * @brief Function for decoding DOGP service write characteristic.
 *
 * @param[in]   data_len              Length of the field to be decoded.
 * @param[in]   p_data                Buffer where the encoded data is stored.
 * @param[out]  p_write_val           Decoded data.
 *
 * @return      Length of the decoded field.
 */
static uint8_t dogp_service_write_characteristic_decode(ble_dogp_service_dogp_service_write_characteristic_t *p_write_val, uint8_t *p_data, uint8_t data_len)
{
    uint8_t pos = 0;
    pos += bds_ble_srv_utf8_str_decode(&p_write_val->write_field, &p_data[pos], (data_len - pos));
    return pos;
}

/**
 * @brief Function for handling the Connect event.
 *
 * @param[in]   p_dogp_service       DOGP service Service structure.
 * @param[in]   p_ble_evt            Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dogp_service_on_connect(ble_dogp_service_t *p_dogp_service, ble_evt_t *p_ble_evt)
{
    p_dogp_service->conn_id = p_ble_evt->evt.gap_evt.connected.conn_id;
}

/**
 * @brief Function for handling the Disconnect event.
 *
 * @param[in]   p_dogp_service       DOGP service Service structure.
 * @param[in]   p_ble_evt            Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dogp_service_on_disconnect(ble_dogp_service_t *p_dogp_service, ble_evt_t *p_ble_evt)
{
    p_dogp_service->conn_id = 0; /** invalid connect id */
}

/**
 * @brief Function for handling the Write event.
 *
 * @param[in]   p_dogp_service       DOGP service Service structure.
 * @param[in]   p_ble_evt            Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dogp_service_on_write(ble_dogp_service_t *p_dogp_service, ble_gatts_write_req_t *p_ble_evt)
{
    if (p_ble_evt->handle == p_dogp_service->dogp_service_read_characteristic_handles.cccd_handle) {
        if ((p_dogp_service->evt_handler != NULL) && (p_dogp_service->conn_id == p_ble_evt->conn_id)) {
            ble_dogp_service_evt_t evt;
            memset(&evt, 0, sizeof(ble_dogp_service_evt_t));
            evt.evt_type = BLE_DOGP_SERVICE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_WRITE;
            bds_uint16_decode(&evt.params.dogp_service_read_characteristic_cccd_value, p_ble_evt->data, p_ble_evt->length);
            p_dogp_service->evt_handler(p_dogp_service, &evt);
            if (BLE_GATT_WRITE_REQUEST == p_ble_evt->flags) {
                if (true == p_ble_evt->is_prepare) {
                    ble_gatts_prepare_write_response(p_dogp_service->conn_id, BLE_STATUS_GATT_SUCCESS, p_ble_evt->handle, p_ble_evt->offset);
                } else {
                    ble_gatts_write_response(p_dogp_service->conn_id, BLE_STATUS_GATT_SUCCESS);
                }
            }
        } else {
            if (BLE_GATT_WRITE_REQUEST == p_ble_evt->flags) {
                if (true == p_ble_evt->is_prepare) {
                    ble_gatts_prepare_write_response(p_dogp_service->conn_id, BLE_STATUS_GATT_FAILED, p_ble_evt->handle, p_ble_evt->offset);
                } else {
                    ble_gatts_write_response(p_dogp_service->conn_id, BLE_STATUS_GATT_FAILED);
                }
            }
        }
    }

    if (p_ble_evt->handle == p_dogp_service->dogp_service_write_characteristic_handles.value_handle) {
        if ((p_dogp_service->evt_handler != NULL) && (p_dogp_service->conn_id == p_ble_evt->conn_id)) {
            ble_dogp_service_evt_t evt;
            memset(&evt, 0, sizeof(ble_dogp_service_evt_t));
            evt.evt_type = BLE_DOGP_SERVICE_DOGP_SERVICE_WRITE_CHARACTERISTIC_EVT_WRITE;
            evt.params.dogp_service_write_characteristic_is_prepare = false;
            evt.params.dogp_service_write_characteristic_write_offset = p_ble_evt->offset;
            evt.params.dogp_service_write_characteristic_write_length = p_ble_evt->length;
            evt.params.dogp_service_write_characteristic_write_long_data = NULL;

            if (BLE_GATT_WRITE_REQUEST == p_ble_evt->flags) {
                if (true == p_ble_evt->is_prepare) {
                    g_att_handle = p_dogp_service->dogp_service_write_characteristic_handles.value_handle;
                    evt.params.dogp_service_write_characteristic_is_prepare = true;
                    evt.params.dogp_service_write_characteristic_write_long_data = p_ble_evt->data;
                    ble_gatts_prepare_write_response(p_dogp_service->conn_id, BLE_STATUS_GATT_SUCCESS, p_ble_evt->handle, p_ble_evt->offset);
                } else {
                    dogp_service_write_characteristic_decode(&evt.params.dogp_service_write_characteristic, p_ble_evt->data, p_ble_evt->length);
                }
            } else {
                dogp_service_write_characteristic_decode(&evt.params.dogp_service_write_characteristic, p_ble_evt->data, p_ble_evt->length);
            }
            p_dogp_service->evt_handler(p_dogp_service, &evt);
        } else {
            if (BLE_GATT_WRITE_REQUEST == p_ble_evt->flags) {
                if (true == p_ble_evt->is_prepare) {
                    ble_gatts_prepare_write_response(p_dogp_service->conn_id, BLE_STATUS_GATT_FAILED, p_ble_evt->handle, p_ble_evt->offset);
                } else {
                    ble_gatts_write_response(p_dogp_service->conn_id, BLE_STATUS_GATT_FAILED);
                }
            }
        }
    }
}

/**
 * @brief Function for handling the Excute Write event.
 *
 * @param[in]   p_dogp_service       DOGP service Service structure.
 * @param[in]   p_ble_evt            Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dogp_service_on_excute_write(ble_dogp_service_t *p_dogp_service, ble_gatts_execute_write_req_t *p_ble_evt)
{
    if ((p_dogp_service->evt_handler != NULL) && (p_dogp_service->conn_id == p_ble_evt->conn_id)) {
        if (true == p_ble_evt->is_execute) {
            ble_dogp_service_evt_t evt;
            memset(&evt, 0, sizeof(ble_dogp_service_evt_t));
            evt.evt_type = BLE_DOGP_SERVICE_DOGP_SERVICE_WRITE_CHARACTERISTIC_EVT_EXCUTE_WRITE;
            evt.params.dogp_service_write_characteristic_is_excute = true;
            ble_gatts_execute_write_response(p_dogp_service->conn_id, BLE_STATUS_GATT_SUCCESS, g_att_handle);
        }
    }
}

/**
 * @brief Function for handling the Read event.
 *
 * @param[in]   p_dogp_service       DOGP service Service structure.
 * @param[in]   p_ble_evt            Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dogp_service_on_read(ble_dogp_service_t *p_dogp_service, ble_gatts_read_req_t *p_ble_evt)
{
    if (p_ble_evt->count == 1) {
        /**read characteristic */
        if (*p_ble_evt->handle == p_dogp_service->dogp_service_read_characteristic_handles.value_handle) {
            if ((p_dogp_service->evt_handler != NULL) && (p_dogp_service->conn_id == p_ble_evt->conn_id)) {
                ble_dogp_service_evt_t evt;
                memset(&evt, 0, sizeof(ble_dogp_service_evt_t));
                evt.evt_type = BLE_DOGP_SERVICE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_READ;
                evt.params.dogp_service_read_characteristic_read_offset = p_ble_evt->offset;/**default is 0*/
                p_dogp_service->evt_handler(p_dogp_service, &evt);
            }
        }
        /**read descriptor*/
        if (*p_ble_evt->handle == p_dogp_service->dogp_service_read_characteristic_handles.cccd_handle) {
            if ((p_dogp_service->evt_handler != NULL) && (p_dogp_service->conn_id == p_ble_evt->conn_id)) {
                ble_dogp_service_evt_t evt;
                memset(&evt, 0, sizeof(ble_dogp_service_evt_t));
                evt.evt_type = BLE_DOGP_SERVICE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_READ;
                p_dogp_service->evt_handler(p_dogp_service, &evt);
            }
        }
    }
}

/**
 * @brief Function for handling the indication confirm event.
 *
 * @param[in]   p_dogp_service       DOGP service Service structure.
 * @param[in]   p_ble_evt            Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dogp_service_on_hvc_confirm(ble_dogp_service_t *p_dogp_service, ble_gatts_handle_value_cnf_t *p_ble_evt)
{
    /** No implementation needed.*/
}

/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_dogp_service       DOGP service Service structure.
 * @param[in]   p_ble_evt            Event received from the BLE stack.
 *
 * @return      None.
 */
void ble_dogp_service_on_ble_evt(ble_dogp_service_t *p_dogp_service, ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_CONNECTED_IND:
            ble_dogp_service_on_connect(p_dogp_service, p_ble_evt);
            break;

        case BLE_GAP_DISCONNECTED_IND:
            ble_dogp_service_on_disconnect(p_dogp_service, p_ble_evt);
            break;

        case BLE_GATTS_READ_REQ:
            ble_dogp_service_on_read(p_dogp_service, &(p_ble_evt->evt.gatts_evt.read));
            break;

        case BLE_GATTS_WRITE_REQ:
            ble_dogp_service_on_write(p_dogp_service, &(p_ble_evt->evt.gatts_evt.write));
            break;

        case BLE_GATTS_EXECUTE_WRITE_REQ:
            ble_dogp_service_on_excute_write(p_dogp_service, &(p_ble_evt->evt.gatts_evt.excute_write));
            break;

        case BLE_GATTS_HANDLE_VALUE_CNF:
            ble_dogp_service_on_hvc_confirm(p_dogp_service, &(p_ble_evt->evt.gatts_evt.confirm));
            break;

        default:
            /** No implementation needed.*/
            break;
    }
}

/**
 * @brief Function for initializing the DOGP service.
 *
 * @param[in]   p_dogp_service       DOGP service Service structure.
 * @param[in]   p_dogp_service_init  DOGP service Service init structure.
 *
 * @return      None.
 */
ble_status_t ble_dogp_service_init(ble_dogp_service_t *p_dogp_service, const ble_dogp_service_init_t *p_dogp_service_init)
{
    ble_status_t err_code;
    uint16_t dogp_service_handle_num;
    ble_gatt_service_t dogp_service_t;
    uint8_t dogp_service_uuid[2] = {0};
    uint16_t dogp_service_uuid16 = 0;

    /**Initialize service structure*/
    p_dogp_service->evt_handler = p_dogp_service_init->evt_handler;
    p_dogp_service->conn_id = 0; /**invalid hanldle;*/

    /**Add service*/
    dogp_service_uuid16 = 0x18A0;
    bds_uint16_encode(dogp_service_uuid, &dogp_service_uuid16);
    ble_gatt_uuid128_generate(dogp_service_t.uuid, NULL, dogp_service_uuid);
    dogp_service_handle_num = 2 * 2 + ATTRIBUTE_HANDLE_OFFSET;

    err_code = ble_gatts_add_service(BLE_GATT_SERVICE_PRIMARY, dogp_service_t.uuid, dogp_service_handle_num, &(p_dogp_service->service_handle));
    if (err_code != BLE_STATUS_SUCCESS) {
        return err_code;
    }
    {
        /**Add DOGP service read characteristic characteristic*/
        ble_gatt_char_t dogp_service_read_characteristic_t;
        uint32_t dogp_service_read_characteristic_permission = 0;
        memset(&dogp_service_read_characteristic_t, 0, sizeof(ble_gatt_char_t));
        {
            uint8_t dogp_service_read_characteristic_uuid[2] = {0};
            uint16_t dogp_service_read_characteristic_uuid16 = 0x2AA0;
            bds_uint16_encode(dogp_service_read_characteristic_uuid, &dogp_service_read_characteristic_uuid16);
            ble_gatt_uuid128_generate(dogp_service_read_characteristic_t.uuid, NULL, dogp_service_read_characteristic_uuid);
        }
        dogp_service_read_characteristic_t.properties = 0;
        dogp_service_read_characteristic_t.properties = dogp_service_read_characteristic_t.properties | BLE_GATT_CHAR_PROP_NOTIFY;
        dogp_service_read_characteristic_t.properties = dogp_service_read_characteristic_t.properties | BLE_GATT_CHAR_PROP_READ;

        dogp_service_read_characteristic_permission = dogp_service_read_characteristic_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dogp_service->service_handle, &(dogp_service_read_characteristic_t), dogp_service_read_characteristic_permission, &(p_dogp_service->dogp_service_read_characteristic_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        }
    }
    {
        /**Add DOGP service read characteristic new descriptor descriptor*/
        ble_gatt_descriptor_t dogp_service_read_characteristic_new_descriptor_t;
        uint32_t dogp_service_read_characteristic_new_descriptor_permission = 0;
        uint8_t dogp_service_read_characteristic_new_descriptor_uuid[2] = {0};
        uint16_t dogp_service_read_characteristic_new_descriptor_uuid16 = 0x2902;
        memset(&dogp_service_read_characteristic_new_descriptor_t, 0, sizeof(ble_gatt_descriptor_t));
        bds_uint16_encode(dogp_service_read_characteristic_new_descriptor_uuid, &dogp_service_read_characteristic_new_descriptor_uuid16);
        ble_gatt_uuid128_generate(dogp_service_read_characteristic_new_descriptor_t.uuid, NULL, dogp_service_read_characteristic_new_descriptor_uuid);

        if (BLE_UUID_CHAR_DESC_CLIENT_CHAR_CONFIG == dogp_service_read_characteristic_new_descriptor_uuid16) {
            dogp_service_read_characteristic_new_descriptor_permission = dogp_service_read_characteristic_new_descriptor_permission | BLE_GATT_PERM_WRITE;
            dogp_service_read_characteristic_new_descriptor_permission = dogp_service_read_characteristic_new_descriptor_permission | BLE_GATT_PERM_READ;
            err_code = ble_gatts_add_descriptor(p_dogp_service->service_handle, &(dogp_service_read_characteristic_new_descriptor_t), dogp_service_read_characteristic_new_descriptor_permission, &(p_dogp_service->dogp_service_read_characteristic_handles.cccd_handle));
        } else if (BLE_UUID_CHAR_DESC_SERVER_CHAR_CONFIG == dogp_service_read_characteristic_new_descriptor_uuid16) {

            err_code = ble_gatts_add_descriptor(p_dogp_service->service_handle, &(dogp_service_read_characteristic_new_descriptor_t), dogp_service_read_characteristic_new_descriptor_permission, &(p_dogp_service->dogp_service_read_characteristic_handles.sccd_handle));
        }
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        }
    }
    {
        /**Add DOGP service write characteristic characteristic*/
        ble_gatt_char_t dogp_service_write_characteristic_t;
        uint32_t dogp_service_write_characteristic_permission = 0;
        memset(&dogp_service_write_characteristic_t, 0, sizeof(ble_gatt_char_t));
        {
            uint8_t dogp_service_write_characteristic_uuid[2] = {0};
            uint16_t dogp_service_write_characteristic_uuid16 = 0x2AA1;
            bds_uint16_encode(dogp_service_write_characteristic_uuid, &dogp_service_write_characteristic_uuid16);
            ble_gatt_uuid128_generate(dogp_service_write_characteristic_t.uuid, NULL, dogp_service_write_characteristic_uuid);
        }
        dogp_service_write_characteristic_t.properties = 0;
        dogp_service_write_characteristic_t.properties = dogp_service_write_characteristic_t.properties | BLE_GATT_CHAR_PROP_WRITE;

        dogp_service_write_characteristic_permission = dogp_service_write_characteristic_permission | BLE_GATT_PERM_WRITE;
        err_code = ble_gatts_add_char(p_dogp_service->service_handle, &(dogp_service_write_characteristic_t), dogp_service_write_characteristic_permission, &(p_dogp_service->dogp_service_write_characteristic_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        }
    }
    return BLE_STATUS_SUCCESS;
}


/**
 * @brief Function for reading response the DOGP service read characteristic.
 *
 * @param[in]   p_dogp_service                        DOGP service Service structure.
 * @param[in]   p_dogp_service_read_characteristic    DOGP service Characteristic structure.
 * @param[in]   offset                                client role read offset for read long.
 *
 * @return      ble_status_t                          0 means success.
 */
ble_status_t ble_dogp_service_dogp_service_read_characteristic_read_rsp(ble_dogp_service_t *p_dogp_service, ble_dogp_service_dogp_service_read_characteristic_t *p_dogp_service_read_characteristic, uint16_t offset)
{
    ble_status_t err_code;

    if (p_dogp_service->conn_id != 0) {
        uint16_t length;
        uint8_t *encoded_value;
        uint16_t read_handle;
        uint16_t read_offset;

        MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN = ble_gatts_get_mtu_size(p_dogp_service->conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN);
        memset(encoded_value, 0, MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN);
        length = dogp_service_read_characteristic_encode(encoded_value, p_dogp_service_read_characteristic);
        read_handle = p_dogp_service->dogp_service_read_characteristic_handles.value_handle;
        read_offset = offset;

        err_code = ble_gatts_read_response(p_dogp_service->conn_id, BLE_STATUS_GATT_SUCCESS, read_handle, read_offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}

/**
 * @brief Function for reading response the DOGP service read characteristic descriptor 0x2902.
 *
 * @param[in]   p_dogp_service                        DOGP service Service structure.
 * @param[in]   cccd_value                            the value of descriptor 0x2902.
 *
 * @return      ble_status_t                          0 means success.
 */
ble_status_t ble_dogp_service_dogp_service_read_characteristic_cccd_read_rsp(ble_dogp_service_t *p_dogp_service, uint16_t cccd_value)
{
    ble_status_t err_code;
    if (p_dogp_service->conn_id != 0) {
        uint8_t *encoded_value;
        uint16_t read_handle;

        MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN = ble_gatts_get_mtu_size(p_dogp_service->conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN);
        memset(encoded_value, 0, MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN);
        bds_uint16_encode(encoded_value, &cccd_value);
        read_handle = p_dogp_service->dogp_service_read_characteristic_handles.cccd_handle;

        err_code = ble_gatts_read_response(p_dogp_service->conn_id, BLE_STATUS_GATT_SUCCESS, read_handle, 0, BLE_CCCD_VALUE_LEN, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}

/**
 * @brief Function for sending the DOGP service read characteristic.
 *
 * @param[in]   p_dogp_service                        DOGP service Service structure.
 * @param[in]   p_dogp_service_read_characteristic    DOGP service Characteristic structure.
 *
 * @return      ble_status_t                          0 means success.
 */
ble_status_t ble_dogp_service_dogp_service_read_characteristic_send(ble_dogp_service_t *p_dogp_service, ble_dogp_service_dogp_service_read_characteristic_t *p_dogp_service_read_characteristic)
{
    ble_status_t err_code;

    if (p_dogp_service->conn_id != 0) {
        uint8_t *encoded_value;
        uint16_t hvx_len;
        uint16_t hvx_handle;
        bool need_confirm = false;

        /**Initialize value struct.*/
        MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN = ble_gatts_get_mtu_size(p_dogp_service->conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN);
        memset(encoded_value, 0, MAX_DOGP_SERVICE_READ_CHARACTERISTIC_LEN);
        hvx_len = dogp_service_read_characteristic_encode(encoded_value, p_dogp_service_read_characteristic);
        hvx_handle = p_dogp_service->dogp_service_read_characteristic_handles.value_handle;

        err_code = ble_gatts_send_handle_value_indication(p_dogp_service->conn_id, hvx_handle, need_confirm, hvx_len, encoded_value);

        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}





