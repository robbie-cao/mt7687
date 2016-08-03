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
 * This file implements Device Information service main function
 *
 ****************************************************************************/
#include "ble_dis.h"
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ble_gatt.h"
#include "ble_gap.h"
#include <string.h>

#define ATTRIBUTE_HANDLE_OFFSET 3              /** < A Offset of the Whole handle numbles in one service. */

uint16_t MAX_DIS_MANUFACTURER_NAME_STRING_LEN; /**< Maximum size of a transmitted Manufacturer Name String. */ 
uint16_t MAX_DIS_MODEL_NUMBER_STRING_LEN;      /**< Maximum size of a transmitted Model Number String. */ 
uint16_t MAX_DIS_SERIAL_NUMBER_STRING_LEN;     /**< Maximum size of a transmitted Serial Number String. */ 
uint16_t MAX_DIS_HARDWARE_REVISION_STRING_LEN; /**< Maximum size of a transmitted Hardware Revision String. */ 
uint16_t MAX_DIS_FIRMWARE_REVISION_STRING_LEN; /**< Maximum size of a transmitted Firmware Revision String. */ 
uint16_t MAX_DIS_SOFTWARE_REVISION_STRING_LEN; /**< Maximum size of a transmitted Software Revision String. */ 
uint16_t MAX_DIS_SYSTEM_ID_LEN;                /**< Maximum size of a transmitted System ID. */ 
uint16_t MAX_DIS_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST_LEN; /**< Maximum size of a transmitted IEEE 11073-20601 Regulatory Certification Data List. */ 
uint16_t MAX_DIS_PNP_ID_LEN;                   /**< Maximum size of a transmitted PnP ID. */ 

/**
 * @brief Function for encoding Manufacturer Name String.
 *
 * @param[in]   p_manufacturer_name_string    Manufacturer Name String characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer              Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t manufacturer_name_string_encode(uint8_t *encoded_buffer, ble_dis_manufacturer_name_string_t *p_manufacturer_name_string)
{
    uint8_t len = 0; 
    len += bds_ble_srv_utf8_str_encode(&encoded_buffer[len], &p_manufacturer_name_string->manufacturer_name); 
    return len;
}

/**
 * @brief Function for encoding Model Number String.
 *
 * @param[in]   p_model_number_string    Model Number String characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer         Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t model_number_string_encode(uint8_t *encoded_buffer, ble_dis_model_number_string_t *p_model_number_string)
{
    uint8_t len = 0; 
    len += bds_ble_srv_utf8_str_encode(&encoded_buffer[len], &p_model_number_string->model_number); 
    return len;
}

/**
 * @brief Function for encoding Serial Number String.
 *
 * @param[in]   p_serial_number_string    Serial Number String characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer          Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t serial_number_string_encode(uint8_t *encoded_buffer, ble_dis_serial_number_string_t *p_serial_number_string)
{
    uint8_t len = 0; 
    len += bds_ble_srv_utf8_str_encode(&encoded_buffer[len], &p_serial_number_string->serial_number); 
    return len;
}

/**
 * @brief Function for encoding Hardware Revision String.
 *
 * @param[in]   p_hardware_revision_string    Hardware Revision String characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer              Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t hardware_revision_string_encode(uint8_t *encoded_buffer, ble_dis_hardware_revision_string_t *p_hardware_revision_string)
{
    uint8_t len = 0; 
    len += bds_ble_srv_utf8_str_encode(&encoded_buffer[len], &p_hardware_revision_string->hardware_revision); 
    return len;
}

/**
 * @brief Function for encoding Firmware Revision String.
 *
 * @param[in]   p_firmware_revision_string    Firmware Revision String characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer              Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t firmware_revision_string_encode(uint8_t *encoded_buffer, ble_dis_firmware_revision_string_t *p_firmware_revision_string)
{
    uint8_t len = 0; 
    len += bds_ble_srv_utf8_str_encode(&encoded_buffer[len], &p_firmware_revision_string->firmware_revision); 
    return len;
}

/**
 * @brief Function for encoding Software Revision String.
 *
 * @param[in]   p_software_revision_string    Software Revision String characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer              Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t software_revision_string_encode(uint8_t *encoded_buffer, ble_dis_software_revision_string_t *p_software_revision_string)
{
    uint8_t len = 0; 
    len += bds_ble_srv_utf8_str_encode(&encoded_buffer[len], &p_software_revision_string->software_revision); 
    return len;
}

/**
 * @brief Function for encoding System ID.
 *
 * @param[in]   p_system_id        System ID characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t system_id_encode(uint8_t *encoded_buffer, ble_dis_system_id_t *p_system_id)
{
    uint8_t len = 0; 
    len += bds_uint40_encode(&encoded_buffer[len], &p_system_id->manufacturer_identifier); 
    len += bds_uint24_encode(&encoded_buffer[len], &p_system_id->organizationally_unique_identifier); 
    return len;
}

/**
 * @brief Function for encoding IEEE 11073-20601 Regulatory Certification Data List.
 *
 * @param[in]   p_ieee_11073_20601_regulatory_certification_data_list    IEEE 11073-20601 Regulatory Certification Data List characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t ieee_11073_20601_regulatory_certification_data_list_encode(uint8_t *encoded_buffer, ble_dis_ieee_11073_20601_regulatory_certification_data_list_t *p_ieee_11073_20601_regulatory_certification_data_list)
{
    uint8_t len = 0; 
    len += bds_regcertdatalist_encode(&encoded_buffer[len], &p_ieee_11073_20601_regulatory_certification_data_list->data); 
    return len;
}

/**
 * @brief Function for encoding Vendor ID Source.
 *
 * @param[in]   p_vendor_id_source    Vendor ID Source structure to be encoded.
 * @param[out]  p_encoded_buffer   	  Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t ble_dis_pnp_id_vendor_id_source_encode(uint8_t *encoded_buffer, ble_dis_pnp_id_vendor_id_source_t *p_vendor_id_source)
{
    uint8_t vendor_id_source = 0;
    vendor_id_source = p_vendor_id_source->vendor_id_source;
    encoded_buffer[0] = vendor_id_source;
    return 1; 
}

/**
 * @brief Function for encoding PnP ID.
 *
 * @param[in]   p_pnp_id          PnP ID characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer  Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t pnp_id_encode(uint8_t *encoded_buffer, ble_dis_pnp_id_t *p_pnp_id)
{
    uint8_t len = 0; 
    len += ble_dis_pnp_id_vendor_id_source_encode(&encoded_buffer[len], &p_pnp_id->vendor_id_source); 
    len += bds_uint16_encode(&encoded_buffer[len], &p_pnp_id->vendor_id); 
    len += bds_uint16_encode(&encoded_buffer[len], &p_pnp_id->product_id); 
    len += bds_uint16_encode(&encoded_buffer[len], &p_pnp_id->product_version); 
    return len;
}

/**
 * @brief Function for handling the Write event.
 *
 * @param[in]   p_dis       Device Information Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dis_on_write(ble_dis_t *p_dis, ble_gatts_write_req_t*p_ble_evt)
{ 

}

/**
 * @brief Function for handling the Excute Write event.
 *
 * @param[in]   p_dis       Device Information Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dis_on_excute_write(ble_dis_t *p_dis, ble_gatts_execute_write_req_t *p_ble_evt)
{ 

}

/**
 * @brief Function for handling the Read event.
 *
 * @param[in]   p_dis       Device Information Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dis_on_read(ble_dis_t *p_dis, ble_gatts_read_req_t *p_ble_evt)
{ 
    /**< read Multiple characteristic values*/
    if (p_ble_evt->count > 1) {
        if (p_dis->evt_handler != NULL) {
            ble_dis_evt_t evt;
            memset(&evt,0,sizeof(ble_dis_evt_t));
            evt.conn_id = p_ble_evt->conn_id;
            evt.evt_type = BLE_DIS_EVT_READ_MULTIPLE;
            evt.params.handle_list = p_ble_evt->handle;
            p_dis->evt_handler(p_dis, &evt);		
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_dis->manufacturer_name_string_handles.value_handle) {
            if (p_dis->evt_handler != NULL) {
                ble_dis_evt_t evt;
                memset(&evt,0,sizeof(ble_dis_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_DIS_MANUFACTURER_NAME_STRING_EVT_READ;
                evt.params.manufacturer_name_string_read_offset = p_ble_evt->offset;  /**< default is 0 */
                p_dis->evt_handler(p_dis, &evt);		
            }
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_dis->model_number_string_handles.value_handle) {
            if (p_dis->evt_handler != NULL) {
                ble_dis_evt_t evt;
                memset(&evt,0,sizeof(ble_dis_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_DIS_MODEL_NUMBER_STRING_EVT_READ;
                evt.params.model_number_string_read_offset = p_ble_evt->offset; /**< default is 0 */
                p_dis->evt_handler(p_dis, &evt);		
            }
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_dis->serial_number_string_handles.value_handle) {
            if (p_dis->evt_handler != NULL) {
                ble_dis_evt_t evt;
                memset(&evt,0,sizeof(ble_dis_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_DIS_SERIAL_NUMBER_STRING_EVT_READ;
                evt.params.serial_number_string_read_offset = p_ble_evt->offset; /**< default is 0 */
                p_dis->evt_handler(p_dis, &evt);		
            }
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_dis->hardware_revision_string_handles.value_handle) {
            if (p_dis->evt_handler != NULL) {
                ble_dis_evt_t evt;
                memset(&evt,0,sizeof(ble_dis_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_DIS_HARDWARE_REVISION_STRING_EVT_READ;
                evt.params.hardware_revision_string_read_offset = p_ble_evt->offset; /**< default is 0 */
                p_dis->evt_handler(p_dis, &evt);		
            }
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_dis->firmware_revision_string_handles.value_handle) {
            if (p_dis->evt_handler != NULL) {
                ble_dis_evt_t evt;
                memset(&evt,0,sizeof(ble_dis_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_DIS_FIRMWARE_REVISION_STRING_EVT_READ;
                evt.params.firmware_revision_string_read_offset = p_ble_evt->offset; /**< default is 0 */
                p_dis->evt_handler(p_dis, &evt);		
            }
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_dis->software_revision_string_handles.value_handle) {
            if (p_dis->evt_handler != NULL) {
                ble_dis_evt_t evt;
                memset(&evt,0,sizeof(ble_dis_evt_t)); 
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_DIS_SOFTWARE_REVISION_STRING_EVT_READ;
                evt.params.software_revision_string_read_offset = p_ble_evt->offset;/**< default is 0 */
                p_dis->evt_handler(p_dis, &evt);		
            }
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_dis->system_id_handles.value_handle) {
            if (p_dis->evt_handler != NULL) {
                ble_dis_evt_t evt;
                memset(&evt,0,sizeof(ble_dis_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_DIS_SYSTEM_ID_EVT_READ;
                evt.params.system_id_read_offset = p_ble_evt->offset; /**< default is 0 */
                p_dis->evt_handler(p_dis, &evt);		
            }
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_dis->ieee_11073_20601_regulatory_certification_data_list_handles.value_handle) {
            if (p_dis->evt_handler != NULL) {
                ble_dis_evt_t evt;
                memset(&evt,0,sizeof(ble_dis_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_DIS_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST_EVT_READ;
                evt.params.ieee_11073_20601_regulatory_certification_data_list_read_offset = p_ble_evt->offset;/**default is 0*/
                p_dis->evt_handler(p_dis, &evt);		
            }
        }
    }
    if (p_ble_evt->count == 1) {
        /**< read characteristic. */
        if (*p_ble_evt->handle == p_dis->pnp_id_handles.value_handle) {
            if (p_dis->evt_handler != NULL) {
                ble_dis_evt_t evt;
                memset(&evt,0,sizeof(ble_dis_evt_t));
                evt.conn_id = p_ble_evt->conn_id;
                evt.evt_type = BLE_DIS_PNP_ID_EVT_READ;
                evt.params.pnp_id_read_offset = p_ble_evt->offset;/**< default is 0 */
                p_dis->evt_handler(p_dis, &evt);		
            }
        }
    }
}

/**
 * @brief Function for handling the indication confirm event.
 *
 * @param[in]   p_dis       Device Information Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
static void ble_dis_on_hvc_confirm(ble_dis_t *p_dis, ble_gatts_handle_value_cnf_t *p_ble_evt)
{

}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_dis       Device Information Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 *
 * @return      None.
 */
void ble_dis_on_ble_evt(ble_dis_t *p_dis, ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTS_READ_REQ:
            ble_dis_on_read(p_dis, &(p_ble_evt->evt.gatts_evt.read));
            break;

        case BLE_GATTS_WRITE_REQ:
            ble_dis_on_write(p_dis, &(p_ble_evt->evt.gatts_evt.write));
            break;

        case BLE_GATTS_EXECUTE_WRITE_REQ:
            ble_dis_on_excute_write(p_dis, &(p_ble_evt->evt.gatts_evt.excute_write));
            break;

        case BLE_GATTS_HANDLE_VALUE_CNF:
            ble_dis_on_hvc_confirm(p_dis, &(p_ble_evt->evt.gatts_evt.confirm));
            break;	

        default:
            /**< No implementation needed.*/
            break;
    }
}

/**
 * @brief Function for initializing the Device Information.
 *
 * @param[in]   p_dis       Device Information Service structure.
 * @param[in]   p_dis_init  Device Information Service init structure.
 *
 * @return      None.
 */
ble_status_t ble_dis_init(ble_dis_t *p_dis, const ble_dis_init_t *p_dis_init)
{
    ble_status_t err_code = BLE_STATUS_GATT_FAILED;
    uint16_t dis_handle_num = 0;
    ble_gatt_service_t dis_t;
    uint8_t dis_uuid[2] = {0};
    //uint8_t dis_uuid128[16] = {0};
    uint16_t dis_uuid16 = 0;

    /**< Initialize service structure. */
    p_dis->evt_handler = p_dis_init->evt_handler;

    /**< Add service */
    dis_uuid16 = 0x180A;
    bds_uint16_encode(dis_uuid, &dis_uuid16);
    ble_gatt_uuid128_generate(dis_t.uuid, NULL, dis_uuid);
    dis_handle_num = 2 * 9 + ATTRIBUTE_HANDLE_OFFSET;
                
    err_code = ble_gatts_add_service(BLE_GATT_SERVICE_PRIMARY, dis_t.uuid, dis_handle_num, &(p_dis->service_handle));
    if (err_code != BLE_STATUS_SUCCESS) {
        return err_code;
    }
    {
        /**< Add Manufacturer Name String characteristic. */
        ble_gatt_char_t manufacturer_name_string_t;
        uint32_t manufacturer_name_string_permission = 0;
        memset(&manufacturer_name_string_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t manufacturer_name_string_uuid[2] = {0};
            uint16_t manufacturer_name_string_uuid16 = 0x2A29;
            bds_uint16_encode(manufacturer_name_string_uuid, &manufacturer_name_string_uuid16);
            ble_gatt_uuid128_generate(manufacturer_name_string_t.uuid, NULL, manufacturer_name_string_uuid);
        }
        manufacturer_name_string_t.properties = 0;
        manufacturer_name_string_t.properties = manufacturer_name_string_t.properties | BLE_GATT_CHAR_PROP_READ; 

        manufacturer_name_string_permission = manufacturer_name_string_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dis->service_handle, &(manufacturer_name_string_t), manufacturer_name_string_permission, &(p_dis->manufacturer_name_string_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add Model Number String characteristic. */
        ble_gatt_char_t model_number_string_t;
        uint32_t model_number_string_permission = 0;
        memset(&model_number_string_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t model_number_string_uuid[2] = {0};
            uint16_t model_number_string_uuid16 = 0x2A24;
            bds_uint16_encode(model_number_string_uuid, &model_number_string_uuid16);
            ble_gatt_uuid128_generate(model_number_string_t.uuid, NULL, model_number_string_uuid);
        }
        model_number_string_t.properties = 0;
        model_number_string_t.properties = model_number_string_t.properties | BLE_GATT_CHAR_PROP_READ; 

        model_number_string_permission = model_number_string_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dis->service_handle, &(model_number_string_t), model_number_string_permission, &(p_dis->model_number_string_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add Serial Number String characteristic. */
        ble_gatt_char_t serial_number_string_t;
        uint32_t serial_number_string_permission = 0;
        memset(&serial_number_string_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t serial_number_string_uuid[2] = {0};
            uint16_t serial_number_string_uuid16 = 0x2A25;
            bds_uint16_encode(serial_number_string_uuid, &serial_number_string_uuid16);
            ble_gatt_uuid128_generate(serial_number_string_t.uuid, NULL, serial_number_string_uuid);
        }
        serial_number_string_t.properties = 0;
        serial_number_string_t.properties = serial_number_string_t.properties | BLE_GATT_CHAR_PROP_READ; 

        serial_number_string_permission = serial_number_string_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dis->service_handle, &(serial_number_string_t), serial_number_string_permission, &(p_dis->serial_number_string_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add Hardware Revision String characteristic. */
        ble_gatt_char_t hardware_revision_string_t;
        uint32_t hardware_revision_string_permission = 0;
        memset(&hardware_revision_string_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t hardware_revision_string_uuid[2] = {0};
            uint16_t hardware_revision_string_uuid16 = 0x2A27;
            bds_uint16_encode(hardware_revision_string_uuid, &hardware_revision_string_uuid16);
            ble_gatt_uuid128_generate(hardware_revision_string_t.uuid, NULL, hardware_revision_string_uuid);
        }
        hardware_revision_string_t.properties = 0;
        hardware_revision_string_t.properties = hardware_revision_string_t.properties | BLE_GATT_CHAR_PROP_READ; 

        hardware_revision_string_permission = hardware_revision_string_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dis->service_handle, &(hardware_revision_string_t), hardware_revision_string_permission, &(p_dis->hardware_revision_string_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add Firmware Revision String characteristic. */
        ble_gatt_char_t firmware_revision_string_t;
        uint32_t firmware_revision_string_permission = 0;
        memset(&firmware_revision_string_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t firmware_revision_string_uuid[2] = {0};
            uint16_t firmware_revision_string_uuid16 = 0x2A26;
            bds_uint16_encode(firmware_revision_string_uuid, &firmware_revision_string_uuid16);
            ble_gatt_uuid128_generate(firmware_revision_string_t.uuid, NULL, firmware_revision_string_uuid);
        }
        firmware_revision_string_t.properties = 0;
        firmware_revision_string_t.properties = firmware_revision_string_t.properties | BLE_GATT_CHAR_PROP_READ; 

        firmware_revision_string_permission = firmware_revision_string_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dis->service_handle, &(firmware_revision_string_t), firmware_revision_string_permission, &(p_dis->firmware_revision_string_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add Software Revision String characteristic. */
        ble_gatt_char_t software_revision_string_t;
        uint32_t software_revision_string_permission = 0;
        memset(&software_revision_string_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t software_revision_string_uuid[2] = {0};
            uint16_t software_revision_string_uuid16 = 0x2A28;
            bds_uint16_encode(software_revision_string_uuid, &software_revision_string_uuid16);
            ble_gatt_uuid128_generate(software_revision_string_t.uuid, NULL, software_revision_string_uuid);
        }
        software_revision_string_t.properties = 0;
        software_revision_string_t.properties = software_revision_string_t.properties | BLE_GATT_CHAR_PROP_READ; 

        software_revision_string_permission = software_revision_string_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dis->service_handle, &(software_revision_string_t), software_revision_string_permission, &(p_dis->software_revision_string_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add System ID characteristic. */
        ble_gatt_char_t system_id_t;
        uint32_t system_id_permission = 0;
        memset(&system_id_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t system_id_uuid[2] = {0};
            uint16_t system_id_uuid16 = 0x2A23;
            bds_uint16_encode(system_id_uuid, &system_id_uuid16);
            ble_gatt_uuid128_generate(system_id_t.uuid, NULL, system_id_uuid);
        }
        system_id_t.properties = 0;
        system_id_t.properties = system_id_t.properties | BLE_GATT_CHAR_PROP_READ; 

        system_id_permission = system_id_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dis->service_handle, &(system_id_t), system_id_permission, &(p_dis->system_id_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add IEEE 11073-20601 Regulatory Certification Data List characteristic. */
        ble_gatt_char_t ieee_11073_20601_regulatory_certification_data_list_t;
        uint32_t ieee_11073_20601_regulatory_certification_data_list_permission = 0;
        memset(&ieee_11073_20601_regulatory_certification_data_list_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t ieee_11073_20601_regulatory_certification_data_list_uuid[2] = {0};
            uint16_t ieee_11073_20601_regulatory_certification_data_list_uuid16 = 0x2A2A;
            bds_uint16_encode(ieee_11073_20601_regulatory_certification_data_list_uuid, &ieee_11073_20601_regulatory_certification_data_list_uuid16);
            ble_gatt_uuid128_generate(ieee_11073_20601_regulatory_certification_data_list_t.uuid, NULL, ieee_11073_20601_regulatory_certification_data_list_uuid);
        }
        ieee_11073_20601_regulatory_certification_data_list_t.properties = 0;
        ieee_11073_20601_regulatory_certification_data_list_t.properties = ieee_11073_20601_regulatory_certification_data_list_t.properties | BLE_GATT_CHAR_PROP_READ; 

        ieee_11073_20601_regulatory_certification_data_list_permission = ieee_11073_20601_regulatory_certification_data_list_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dis->service_handle, &(ieee_11073_20601_regulatory_certification_data_list_t), ieee_11073_20601_regulatory_certification_data_list_permission, &(p_dis->ieee_11073_20601_regulatory_certification_data_list_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    {
        /**< Add PnP ID characteristic */
        ble_gatt_char_t pnp_id_t;
        uint32_t pnp_id_permission = 0;
        memset(&pnp_id_t, 0, sizeof(ble_gatt_char_t)); 
        {
            uint8_t pnp_id_uuid[2] = {0};
            uint16_t pnp_id_uuid16 = 0x2A50;
            bds_uint16_encode(pnp_id_uuid, &pnp_id_uuid16);
            ble_gatt_uuid128_generate(pnp_id_t.uuid, NULL, pnp_id_uuid);
        }
        pnp_id_t.properties = 0;
        pnp_id_t.properties = pnp_id_t.properties | BLE_GATT_CHAR_PROP_READ; 

        pnp_id_permission = pnp_id_permission | BLE_GATT_PERM_READ;
        err_code = ble_gatts_add_char(p_dis->service_handle, &(pnp_id_t), pnp_id_permission, &(p_dis->pnp_id_handles.value_handle));
        if (err_code != BLE_STATUS_SUCCESS) {
            return err_code;
        } 
    }
    return BLE_STATUS_SUCCESS;
}

/**
 * @brief Function for reading response the Manufacturer Name String.
 *
 * @param[in]   p_dis                       Device Information Service structure.
 * @param[in]   p_manufacturer_name_string  Device Information Characteristic structure.
 * @param[in]   offset                      client role read offset for read long.
 * @param[in]   conn_id                     connection ID.
 *
 * @return      ble_status_t                BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_dis_manufacturer_name_string_read_rsp(ble_dis_t *p_dis, ble_dis_manufacturer_name_string_t *p_manufacturer_name_string, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct.*/
        MAX_DIS_MANUFACTURER_NAME_STRING_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DIS_MANUFACTURER_NAME_STRING_LEN);
        memset(encoded_value, 0, MAX_DIS_MANUFACTURER_NAME_STRING_LEN);
        length = manufacturer_name_string_encode(encoded_value, p_manufacturer_name_string);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_dis->manufacturer_name_string_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 * @brief Function for reading response the Model Number String.
 *
 * @param[in]   p_dis                  Device Information Service structure.
 * @param[in]   p_model_number_string  Device Information Characteristic structure.
 * @param[in]   offset                 client role read offset for read long.
 *
 * @return      ble_status_t           BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_dis_model_number_string_read_rsp(ble_dis_t *p_dis, ble_dis_model_number_string_t *p_model_number_string, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct. */
        MAX_DIS_MODEL_NUMBER_STRING_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DIS_MODEL_NUMBER_STRING_LEN);
        memset(encoded_value, 0, MAX_DIS_MODEL_NUMBER_STRING_LEN);
        length = model_number_string_encode(encoded_value, p_model_number_string);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_dis->model_number_string_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 * @brief Function for reading response the Serial Number String.
 *
 * @param[in]   p_dis                   Device Information Service structure.
 * @param[in]   p_serial_number_string  Device Information Characteristic structure.
 * @param[in]   offset                  client role read offset for read long.
 *
 * @return      ble_status_t            BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_dis_serial_number_string_read_rsp(ble_dis_t *p_dis, ble_dis_serial_number_string_t *p_serial_number_string, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct. */
        MAX_DIS_SERIAL_NUMBER_STRING_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DIS_SERIAL_NUMBER_STRING_LEN);
        memset(encoded_value, 0, MAX_DIS_SERIAL_NUMBER_STRING_LEN);
        length = serial_number_string_encode(encoded_value, p_serial_number_string);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_dis->serial_number_string_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 < @brief Function for reading response the Hardware Revision String.
 *
 * @param[in]   p_dis                       Device Information Service structure.
 * @param[in]   p_hardware_revision_string  Device Information Characteristic structure.
 * @param[in]   offset                      client role read offset for read long.
 *
 * @return      ble_status_t                BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_dis_hardware_revision_string_read_rsp(ble_dis_t *p_dis, ble_dis_hardware_revision_string_t *p_hardware_revision_string, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct.*/
        MAX_DIS_HARDWARE_REVISION_STRING_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DIS_HARDWARE_REVISION_STRING_LEN);
        memset(encoded_value, 0, MAX_DIS_HARDWARE_REVISION_STRING_LEN);
        length = hardware_revision_string_encode(encoded_value, p_hardware_revision_string);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_dis->hardware_revision_string_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 * @brief Function for reading response the Firmware Revision String.
 *
 * @param[in]   p_dis                       Device Information Service structure.
 * @param[in]   p_firmware_revision_string  Device Information Characteristic structure.
 * @param[in]   offset                      client role read offset for read long.
 *
 * @return      ble_status_t                BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_dis_firmware_revision_string_read_rsp(ble_dis_t *p_dis, ble_dis_firmware_revision_string_t *p_firmware_revision_string, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct.*/
        MAX_DIS_FIRMWARE_REVISION_STRING_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DIS_FIRMWARE_REVISION_STRING_LEN);
        memset(encoded_value, 0, MAX_DIS_FIRMWARE_REVISION_STRING_LEN);
        length = firmware_revision_string_encode(encoded_value, p_firmware_revision_string);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_dis->firmware_revision_string_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 * @brief Function for reading response the Software Revision String.
 *
 * @param[in]   p_dis                       Device Information Service structure.
 * @param[in]   p_software_revision_string  Device Information Characteristic structure.
 * @param[in]   offset                      client role read offset for read long.
 *
 * @return      ble_status_t                BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_dis_software_revision_string_read_rsp(ble_dis_t *p_dis, ble_dis_software_revision_string_t *p_software_revision_string, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct.*/
        MAX_DIS_SOFTWARE_REVISION_STRING_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DIS_SOFTWARE_REVISION_STRING_LEN);
        memset(encoded_value, 0, MAX_DIS_SOFTWARE_REVISION_STRING_LEN);
        length = software_revision_string_encode(encoded_value, p_software_revision_string);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_dis->software_revision_string_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 * @brief Function for reading response the System ID.
 *
 * @param[in]   p_dis        Device Information Service structure.
 * @param[in]   p_system_id  Device Information Characteristic structure.
 * @param[in]   offset       client role read offset for read long.
 *
 * @return      ble_status_t BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_dis_system_id_read_rsp(ble_dis_t *p_dis, ble_dis_system_id_t *p_system_id, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct.*/
        MAX_DIS_SYSTEM_ID_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DIS_SYSTEM_ID_LEN);
        memset(encoded_value, 0, MAX_DIS_SYSTEM_ID_LEN);
        length = system_id_encode(encoded_value, p_system_id);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_dis->system_id_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 * @brief Function for reading response the IEEE 11073-20601 Regulatory Certification Data List.
 *
 * @param[in]   p_dis           Device Information Service structure.
 * @param[in]   p_ieee_11073_20601_regulatory_certification_data_list  Device Information Characteristic structure.
 * @param[in]   offset          client role read offset for read long.
 *
 * @return      ble_status_t    BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_dis_ieee_11073_20601_regulatory_certification_data_list_read_rsp(ble_dis_t *p_dis, ble_dis_ieee_11073_20601_regulatory_certification_data_list_t *p_ieee_11073_20601_regulatory_certification_data_list, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct.*/
        MAX_DIS_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DIS_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST_LEN);
        memset(encoded_value, 0, MAX_DIS_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST_LEN);
        length = ieee_11073_20601_regulatory_certification_data_list_encode(encoded_value, p_ieee_11073_20601_regulatory_certification_data_list);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_dis->ieee_11073_20601_regulatory_certification_data_list_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 * @brief Function for reading response the PnP ID.
 *
 * @param[in]   p_dis     Device Information Service structure.
 * @param[in]   p_pnp_id  Device Information Characteristic structure.
 * @param[in]   offset    client role read offset for read long.
 *
 * @return      ble_status_t  BLE_STATUS_SUCCESS means success.
 */
ble_status_t ble_dis_pnp_id_read_rsp(ble_dis_t *p_dis, ble_dis_pnp_id_t *p_pnp_id, uint16_t conn_id, uint16_t offset)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint16_t length = 0;
        uint8_t *encoded_value = NULL;
        
        /**< Compose value struct.*/
        MAX_DIS_PNP_ID_LEN = ble_gatts_get_mtu_size(conn_id);
        encoded_value = ble_gatts_alloc_memory(MAX_DIS_PNP_ID_LEN);
        memset(encoded_value, 0, MAX_DIS_PNP_ID_LEN);
        length = pnp_id_encode(encoded_value, p_pnp_id);

        err_code = ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, p_dis->pnp_id_handles.value_handle, offset, length, encoded_value);
        ble_gatts_free_memory(encoded_value);
        return err_code;
    }
    return BLE_STATUS_GATT_FAILED;
}


/**
 * @brief Function for response the dis read multiple characteristic event, if your app not care read multiple, please do not call this API.
 *
 * @details Sets a set of characteristic value of the dis. The values will be sent
 *          to the client by read multiple characteristic response.
 *
 * @param[in] p_dis          Device Information Service structure.
 * @param[in] uint8_t *data  data to send                 
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_read_multiple_rsp(ble_dis_t *p_dis, uint16_t conn_id, uint16_t length, const uint8_t *data)
{
    ble_status_t err_code;

    if (conn_id != 0) {
        uint8_t *encoded_value = NULL;
        uint16_t mtu_length = 0;
        
        /**< Compose value struct.*/
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








