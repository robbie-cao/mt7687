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
#ifndef __BLE_DIS_H__
#define __BLE_DIS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"
#include "ble_gatt.h"
#include "ble_gap.h"

/**@brief Device Information event type. */
typedef enum
{ 
    BLE_DIS_EVT_READ_MULTIPLE,                         /**< dis read multiple event. */
    BLE_DIS_MANUFACTURER_NAME_STRING_EVT_READ,         /**< Manufacturer Name String read event. */
    BLE_DIS_MODEL_NUMBER_STRING_EVT_READ,              /**< Model Number String read event. */
    BLE_DIS_SERIAL_NUMBER_STRING_EVT_READ,             /**< Serial Number String read event. */
    BLE_DIS_HARDWARE_REVISION_STRING_EVT_READ,         /**< Hardware Revision String read event. */
    BLE_DIS_FIRMWARE_REVISION_STRING_EVT_READ,         /**< Firmware Revision String read event. */
    BLE_DIS_SOFTWARE_REVISION_STRING_EVT_READ,         /**< Software Revision String read event. */
    BLE_DIS_SYSTEM_ID_EVT_READ,                        /**< System ID read event. */
    BLE_DIS_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST_EVT_READ, /**< IEEE 11073-20601 Regulatory Certification Data List read event. */
    BLE_DIS_PNP_ID_EVT_READ,                           /**< PnP ID read event. */
} ble_dis_evt_type_t;

/**@brief Forward declaration of the ble_dis_t type.*/
typedef struct ble_dis_s ble_dis_t;

typedef enum
{ 
    BLE_DIS_VENDOR_ID_SOURCE_BLUETOOTH_SIG_ASSIGNED_COMPANY_IDENTIFIER_VALUE_FROM_THE_ASSIGNED_NUMBERS_DOCUMENT = 1, 
    BLE_DIS_VENDOR_ID_SOURCE_USB_IMPLEMENTER_S_FORUM_ASSIGNED_VENDOR_ID_VALUE = 2, 
} ble_dis_enum_vendor_id_source_t; 

typedef struct
{
    ble_dis_enum_vendor_id_source_t vendor_id_source; 
} ble_dis_pnp_id_vendor_id_source_t; 

/**@brief Manufacturer Name String structure. */
typedef struct
{
    ble_srv_utf8_str_t manufacturer_name;
} ble_dis_manufacturer_name_string_t;

/**@brief Model Number String structure. */
typedef struct
{
    ble_srv_utf8_str_t model_number;
} ble_dis_model_number_string_t;

/**@brief Serial Number String structure. */
typedef struct
{
    ble_srv_utf8_str_t serial_number;
} ble_dis_serial_number_string_t;

/**@brief Hardware Revision String structure. */
typedef struct
{
    ble_srv_utf8_str_t hardware_revision;
} ble_dis_hardware_revision_string_t;

/**@brief Firmware Revision String structure. */
typedef struct
{
    ble_srv_utf8_str_t firmware_revision;
} ble_dis_firmware_revision_string_t;
/**@brief Software Revision String structure. */
typedef struct
{
    ble_srv_utf8_str_t software_revision;
} ble_dis_software_revision_string_t;

/**@brief System ID structure. */
typedef struct
{
    uint40_t manufacturer_identifier;
    uint24_t organizationally_unique_identifier;
} ble_dis_system_id_t;

/**@brief IEEE 11073-20601 Regulatory Certification Data List structure. */
typedef struct
{
    regcertdatalist_t data;
} ble_dis_ieee_11073_20601_regulatory_certification_data_list_t;

/**@brief PnP ID structure. */
typedef struct
{
    ble_dis_pnp_id_vendor_id_source_t vendor_id_source;
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t product_version;
} ble_dis_pnp_id_t;

/**@brief Device Information Service event. */
typedef struct
{
    uint16_t conn_id;                                    /**< Handle of the current connection (is 0 if not in a connection). */
    ble_dis_evt_type_t evt_type;                         /**< Type of event. */
    struct {
        uint16_t *handle_list;                           /**< characteristic handle list, for read multiple */
        uint16_t manufacturer_name_string_read_offset;   /**< offset for read long*/
        uint16_t model_number_string_read_offset;        /**< offset for read long*/
        uint16_t serial_number_string_read_offset;       /**< offset for read long*/
        uint16_t hardware_revision_string_read_offset;   /**< offset for read long*/
        uint16_t firmware_revision_string_read_offset;   /**< offset for read long*/
        uint16_t software_revision_string_read_offset;   /**< offset for read long*/
        uint16_t system_id_read_offset;                  /**< offset for read long*/
        uint16_t ieee_11073_20601_regulatory_certification_data_list_read_offset; /**< offset for read long*/
        uint16_t pnp_id_read_offset;                     /**< offset for read long*/
    } params;
} ble_dis_evt_t;

/**@brief Device Information Service event handler type. */
typedef void (*ble_dis_evt_handler_t) (ble_dis_t *p_dis, ble_dis_evt_t *p_evt);

/**@brief Device Information Service init structure. This contains all options and data needed for initialization of the service */
typedef struct
{
    ble_dis_evt_handler_t     evt_handler; /**< Event handler to be called for handling events in the Device Information Service. */
} ble_dis_init_t;

/**@brief Device Information Service structure. This contains various status information for the service.*/
struct ble_dis_s
{
    ble_dis_evt_handler_t evt_handler;                         /**< Event handler to be called for handling events in the Device Information Service. */
    uint16_t service_handle;                                   /**< Handle of Device Information Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t manufacturer_name_string_handles; /**< Handles related to the Manufacturer Name String characteristic. */
    ble_gatts_char_handles_t model_number_string_handles;      /**< Handles related to the Model Number String characteristic. */
    ble_gatts_char_handles_t serial_number_string_handles;     /**< Handles related to the Serial Number String characteristic. */
    ble_gatts_char_handles_t hardware_revision_string_handles; /**< Handles related to the Hardware Revision String characteristic. */
    ble_gatts_char_handles_t firmware_revision_string_handles; /**< Handles related to the Firmware Revision String characteristic. */
    ble_gatts_char_handles_t software_revision_string_handles; /**< Handles related to the Software Revision String characteristic. */
    ble_gatts_char_handles_t system_id_handles;                /**< Handles related to the System ID characteristic. */
    ble_gatts_char_handles_t ieee_11073_20601_regulatory_certification_data_list_handles; /**< Handles related to the IEEE 11073-20601 Regulatory Certification Data List characteristic. */
    ble_gatts_char_handles_t pnp_id_handles;                   /**< Handles related to the PnP ID characteristic. */
};

/**
 * @brief Function for initializing the Device Information.
 *
 * @param[out] p_dis        Device Information Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]  p_dis_init   Information needed to initialize the service.
 *
 * @return     0 means successful  initialization of service, otherwise an error code.
 */
ble_status_t ble_dis_init(ble_dis_t *p_dis, const ble_dis_init_t *p_dis_init);

/**@brief Function for handling the Application's BLE Stack events.*/
void ble_dis_on_ble_evt(ble_dis_t *p_dis, ble_evt_t *p_ble_evt);

/**
 * @brief Function for response the Manufacturer Name String read event.
 *
 * @details Sets a new value of the Manufacturer Name String characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dis                       Device Information Service structure.
 * @param[in] p_manufacturer_name_string  New Manufacturer Name String.
 * @param[in] offset                      New Manufacturer Name String read offset.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_manufacturer_name_string_read_rsp(ble_dis_t *p_dis, ble_dis_manufacturer_name_string_t *p_manufacturer_name_string, uint16_t conn_id, uint16_t offset);

/**
 * @brief Function for response the Model Number String read event.
 *
 * @details Sets a new value of the Model Number String characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dis                  Device Information Service structure.
 * @param[in] p_model_number_string  New Model Number String.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_model_number_string_read_rsp(ble_dis_t *p_dis, ble_dis_model_number_string_t *p_model_number_string, uint16_t conn_id, uint16_t offset);

/**
 * @brief Function for response the Serial Number String read event.
 *
 * @details Sets a new value of the Serial Number String characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dis                   Device Information Service structure.
 * @param[in] p_serial_number_string  New Serial Number String.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_serial_number_string_read_rsp(ble_dis_t *p_dis, ble_dis_serial_number_string_t *p_serial_number_string, uint16_t conn_id, uint16_t offset);

/**
 * @brief Function for response the Hardware Revision String read event.
 *
 * @details Sets a new value of the Hardware Revision String characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dis                       Device Information Service structure.
 * @param[in] p_hardware_revision_string  New Hardware Revision String.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_hardware_revision_string_read_rsp(ble_dis_t *p_dis, ble_dis_hardware_revision_string_t *p_hardware_revision_string, uint16_t conn_id, uint16_t offset);

/**
 * @brief Function for response the Firmware Revision String read event.
 *
 * @details Sets a new value of the Firmware Revision String characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dis                       Device Information Service structure.
 * @param[in] p_firmware_revision_string  New Firmware Revision String.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_firmware_revision_string_read_rsp(ble_dis_t *p_dis, ble_dis_firmware_revision_string_t *p_firmware_revision_string, uint16_t conn_id, uint16_t offset);

/**
 * @brief Function for response the Software Revision String read event.
 *
 * @details Sets a new value of the Software Revision String characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dis                       Device Information Service structure.
 * @param[in] p_software_revision_string  New Software Revision String.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_software_revision_string_read_rsp(ble_dis_t *p_dis, ble_dis_software_revision_string_t *p_software_revision_string, uint16_t conn_id, uint16_t offset);

/**
 * @brief Function for response the System ID read event.
 *
 * @details Sets a new value of the System ID characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dis        Device Information Service structure.
 * @param[in] p_system_id  New System ID.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_system_id_read_rsp(ble_dis_t *p_dis, ble_dis_system_id_t *p_system_id, uint16_t conn_id, uint16_t offset);

/**
 * @brief Function for response the IEEE 11073-20601 Regulatory Certification Data List read event.
 *
 * @details Sets a new value of the IEEE 11073-20601 Regulatory Certification Data List characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dis Device Information Service structure.
 * @param[in] p_ieee_11073_20601_regulatory_certification_data_list  New IEEE 11073-20601 Regulatory Certification Data List.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_ieee_11073_20601_regulatory_certification_data_list_read_rsp(ble_dis_t *p_dis, ble_dis_ieee_11073_20601_regulatory_certification_data_list_t *p_ieee_11073_20601_regulatory_certification_data_list, uint16_t conn_id, uint16_t offset);

/**
 * @brief Function for response the PnP ID read event.
 *
 * @details Sets a new value of the PnP ID characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_dis     Device Information Service structure.
 * @param[in] p_pnp_id  New PnP ID.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_pnp_id_read_rsp(ble_dis_t *p_dis, ble_dis_pnp_id_t *p_pnp_id, uint16_t conn_id, uint16_t offset);

/**@brief Function for response the dis read multiple characteristic event, if your app not care read multiple, please do not call this API.
 *
 * @details Sets a set of characteristic value of the dis. The values will be sent
 *          to the client by read multiple characteristic response.
 *
 * @param[in] p_dis           Device Information Service structure.
 * @param[in] uint8_t *data   data to send                 
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_dis_read_multiple_rsp(ble_dis_t *p_dis, uint16_t conn_id, uint16_t length, const uint8_t *data);

#ifdef __cplusplus
}
#endif
	
#endif /**_BLE_DIS_H__*/



