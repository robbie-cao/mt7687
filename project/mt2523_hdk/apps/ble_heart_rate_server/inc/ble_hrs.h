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

#ifndef __BLE_HRS_H__
#define __BLE_HRS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts_srv_common.h"
#include "ble_bds_app_util.h"
#include "ble_gatt.h"
#include "ble_gap.h"

/* Error codes */
#define HEART_RATE_CONTROL_POINT_NOT_SUPPORTED 0x80;  /**< Heart rate Control Point value not supported. */

/**@brief Heart Rate event type. */
typedef enum
{ 
    BLE_HRS_EVT_READ_MULTIPLE,                        /**< hrs read multiple event. */
    BLE_HRS_HEART_RATE_MEASUREMENT_EVT_CCCD_WRITE,    /**< Heart Rate Measurement CCCD write event. */
    BLE_HRS_HEART_RATE_MEASUREMENT_EVT_CCCD_READ,     /**< Heart Rate Measurement CCCD read event. */
    BLE_HRS_BODY_SENSOR_LOCATION_EVT_READ,            /**< Body Sensor Location read event. */
	BLE_HRS_HEART_RATE_CONTROL_POINT_EVT_WRITE,       /**< Heart Rate Control Point write event. */
    BLE_HRS_HEART_RATE_CONTROL_POINT_EVT_EXCUTE_WRITE,/**< Heart Rate Control Point excute write event. */
} ble_hrs_evt_type_t;

/**@brief Forward declaration of the ble_hrs_t type.*/
typedef struct ble_hrs_s ble_hrs_t;

typedef enum
{ 
    BLE_HRS_HEART_RATE_VALUE_FORMAT_BIT_HEART_RATE_VALUE_FORMAT_IS_SET_TO_UINT8__UNITS__BEATS_PER_MINUTE__BPM_ = 0, 
    BLE_HRS_HEART_RATE_VALUE_FORMAT_BIT_HEART_RATE_VALUE_FORMAT_IS_SET_TO_UINT16__UNITS__BEATS_PER_MINUTE__BPM_ = 1, 
} ble_hrs_enum_flags_heart_rate_value_format_bit_t;

typedef enum
{ 
    BLE_HRS_SENSOR_CONTACT_STATUS_BITS_SENSOR_CONTACT_FEATURE_IS_NOT_SUPPORTED_IN_THE_CURRENT_CONNECTION_0 = 0, 
    BLE_HRS_SENSOR_CONTACT_STATUS_BITS_SENSOR_CONTACT_FEATURE_IS_NOT_SUPPORTED_IN_THE_CURRENT_CONNECTION_1 = 1, 
    BLE_HRS_SENSOR_CONTACT_STATUS_BITS_SENSOR_CONTACT_FEATURE_IS_SUPPORTED__BUT_CONTACT_IS_NOT_DETECTED = 2, 
    BLE_HRS_SENSOR_CONTACT_STATUS_BITS_SENSOR_CONTACT_FEATURE_IS_SUPPORTED_AND_CONTACT_IS_DETECTED = 3, 
} ble_hrs_enum_flags_sensor_contact_status_bits_t;

typedef enum
{ 
    BLE_HRS_ENERGY_EXPENDED_STATUS_BIT_ENERGY_EXPENDED_FIELD_IS_NOT_PRESENT = 0, 
    BLE_HRS_ENERGY_EXPENDED_STATUS_BIT_ENERGY_EXPENDED_FIELD_IS_PRESENT__UNITS__KILO_JOULES = 1, 
} ble_hrs_enum_flags_energy_expended_status_bit_t;

typedef enum
{ 
    BLE_HRS_RR_INTERVAL_BIT_RR_INTERVAL_VALUES_ARE_NOT_PRESENT_ = 0, 
    BLE_HRS_RR_INTERVAL_BIT_ONE_OR_MORE_RR_INTERVAL_VALUES_ARE_PRESENT__UNITS__1_1024_SECONDS = 1, 
} ble_hrs_enum_flags_rr_interval_bit_t;

typedef struct
{
    ble_hrs_enum_flags_heart_rate_value_format_bit_t heart_rate_value_format_bit; 
    ble_hrs_enum_flags_sensor_contact_status_bits_t sensor_contact_status_bits; 
    ble_hrs_enum_flags_energy_expended_status_bit_t energy_expended_status_bit; 
    ble_hrs_enum_flags_rr_interval_bit_t rr_interval_bit; 
} ble_hrs_heart_rate_measurement_flags_t; 

typedef enum
{ 
    BLE_HRS_BODY_SENSOR_LOCATION_OTHER = 0, 
    BLE_HRS_BODY_SENSOR_LOCATION_CHEST = 1, 
    BLE_HRS_BODY_SENSOR_LOCATION_WRIST = 2, 
    BLE_HRS_BODY_SENSOR_LOCATION_FINGER = 3, 
    BLE_HRS_BODY_SENSOR_LOCATION_HAND = 4, 
    BLE_HRS_BODY_SENSOR_LOCATION_EAR_LOBE = 5, 
    BLE_HRS_BODY_SENSOR_LOCATION_FOOT = 6, 
} ble_hrs_enum_body_sensor_location_t;

typedef struct
{
    ble_hrs_enum_body_sensor_location_t body_sensor_location; 
} ble_hrs_body_sensor_location_body_sensor_location_t; 

typedef enum
{ 
    BLE_HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED__RESETS_THE_VALUE_OF_THE_ENERGY_EXPENDED_FIELD_IN_THE_HEART_RATE_MEASUREMENT_CHARACTERISTIC_TO_0 = 1, 
} ble_hrs_enum_heart_rate_control_point_t; 

typedef struct
{
    ble_hrs_enum_heart_rate_control_point_t heart_rate_control_point; 
} ble_hrs_heart_rate_control_point_heart_rate_control_point_t; 

/**@brief Heart Rate Measurement structure. */
typedef struct
{
    ble_hrs_heart_rate_measurement_flags_t flags;
    uint8_t heart_rate_measurement_value__uint8_;
    uint16_t heart_rate_measurement_value__uint16_;
    uint16_t energy_expended;
    uint16_t rr_interval;
} ble_hrs_heart_rate_measurement_t;

/**@brief Body Sensor Location structure. */
typedef struct
{
    ble_hrs_body_sensor_location_body_sensor_location_t body_sensor_location;
} ble_hrs_body_sensor_location_t;

/**@brief Heart Rate Control Point structure. */
typedef struct
{
    ble_hrs_heart_rate_control_point_heart_rate_control_point_t heart_rate_control_point;
} ble_hrs_heart_rate_control_point_t;

/**@brief Heart Rate Service event. */
typedef struct
{
    uint16_t conn_id;                                                /**< Handle of the current connection (is 0 if not in a connection). */
    ble_hrs_evt_type_t evt_type;                                     /**< Type of event. */
    struct {
        uint16_t *handle_list;                                       /**< characteristic handle list, for read multiple */
        uint16_t heart_rate_measurement_cccd_value;                  /**< Holds decoded data in Notify and Indicate event handler. */
        uint16_t body_sensor_location_read_offset;                   /**< offset for read long*/
        ble_hrs_heart_rate_control_point_t heart_rate_control_point; /**< Holds decoded data in Write event handler, not for write long. */
        uint8_t *heart_rate_control_point_write_long_data;           /**< Holds data only for write long*/
        uint8_t heart_rate_control_point_is_prepare;                 /**< is prepare(TRUE) or not(FALSE)*/
        uint16_t heart_rate_control_point_write_offset;              /**< offset for write long*/
        uint16_t heart_rate_control_point_write_length;              /**< the data length of write value */
        uint8_t heart_rate_control_point_is_excute;                  /**< is execute(TRUE) or cancel write(FALSE)*/
    } params;
} ble_hrs_evt_t;

/**@brief Heart Rate Service event handler type. */
typedef void (*ble_hrs_evt_handler_t) (ble_hrs_t *p_hrs, ble_hrs_evt_t *p_evt);

/**@brief Heart Rate Service init structure. This contains all options and data needed for initialization of the service */
typedef struct
{
    ble_hrs_evt_handler_t     evt_handler; /**< Event handler to be called for handling events in the Heart Rate Service. */
} ble_hrs_init_t;

/**@brief Heart Rate Service structure. This contains various status information for the service.*/
struct ble_hrs_s
{
    ble_hrs_evt_handler_t evt_handler;                         /**< Event handler to be called for handling events in the Heart Rate Service. */
    uint16_t service_handle;                                   /**< Handle of Heart Rate Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t heart_rate_measurement_handles;   /**< Handles related to the Heart Rate Measurement characteristic. */
    ble_gatts_char_handles_t body_sensor_location_handles;     /**< Handles related to the Body Sensor Location characteristic. */
    ble_gatts_char_handles_t heart_rate_control_point_handles; /**< Handles related to the Heart Rate Control Point characteristic. */
};

/**
 * @brief Function for initializing the Heart Rate.
 *
 * @param[out] p_hrs        Heart Rate Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]  p_hrs_init   Information needed to initialize the service.
 *
 * @return     0 means successful initialization of service, otherwise an error code.
 */
ble_status_t ble_hrs_init(ble_hrs_t *p_hrs, const ble_hrs_init_t *p_hrs_init);

/**@brief Function for handling the Application's BLE Stack events.*/
void ble_hrs_on_ble_evt(ble_hrs_t *p_hrs, ble_evt_t *p_ble_evt);

/**
 * @brief Function for response the Heart Rate Measurement cccd read event.
 *
 * @details Sets a new value of the Heart Rate Measurement characteristic client configuration descriptor. 
 *          The new value will be sent to the client by read response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   conn_id     Connection ID.
 * @param[in]   cccd_value  Value of the Heart Rate Measurement characteristic client configuration descriptor.
 *
 * @return      0 on success, otherwise an error code.
 */	
ble_status_t ble_hrs_heart_rate_measurement_cccd_read_rsp(ble_hrs_t *p_hrs, uint16_t conn_id, uint16_t cccd_value);

/**
 * @brief Function for sending the Heart Rate Measurement.
 *
 * @details The application calls this function after having performed a heart rate measurement.
 *          The heart rate measurement data is encoded and sent to the client.
 *          This function is only generated if the characteristic's Notify or Indicate property is not 'Excluded'.
 *
 * @param[in]   p_hrs                    Heart Rate Service structure.
 * @param[in]   p_heart_rate_measurement New heart rate measurement.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
ble_status_t ble_hrs_heart_rate_measurement_send(ble_hrs_t *p_hrs, ble_hrs_heart_rate_measurement_t *p_heart_rate_measurement, uint16_t conn_id);

/**
 * @brief Function for response the Body Sensor Location read event.
 *
 * @details Sets a new value of the Body Sensor Location characteristic. The new value will be sent
 *          to the client by read characteristic response.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in] p_hrs                   Heart Rate Service structure.
 * @param[in] p_body_sensor_location  New Body Sensor Location.
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_hrs_body_sensor_location_read_rsp(ble_hrs_t *p_hrs, ble_hrs_body_sensor_location_t *p_body_sensor_location, uint16_t conn_id, uint16_t offset);


/**
 * @brief Function for response the hrs read multiple characteristic event, if your app not care read multiple, please do not call this API.
 *
 * @details Sets a set of characteristic value of the hrs. The values will be sent
 *          to the client by read multiple characteristic response.
 *
 * @param[in] p_hrs           Heart Rate Service structure.
 * @param[in] uint8_t *data   data to send                 
 *
 * @return    0 on success, otherwise an error code.
 */
ble_status_t ble_hrs_read_multiple_rsp(ble_hrs_t *p_hrs, uint16_t conn_id, uint16_t length, const uint8_t *data);

#ifdef __cplusplus
}
#endif
	
#endif /**_BLE_HRS_H__*/



