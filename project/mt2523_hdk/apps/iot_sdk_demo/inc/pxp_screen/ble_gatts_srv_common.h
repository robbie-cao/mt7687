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
 * This file implements gatt server profile common function
 *
 ****************************************************************************/

#ifndef __BLE_GATTS_SRV_COMMON_H__
#define __BLE_GATTS_SRV_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "ble_gap.h"
#include "ble_gatt.h"
#include "ble_status.h"

#ifndef WIN32
#include "FreeRTOS.h"
#include "portable.h"
#endif

#if 0
#define MAX_LE_DEV_NUM      6
#else
#define MAX_LE_DEV_NUM      1
#endif


/************************************************
*   macro
*************************************************/

/** 
 * @defgroup UUID_SERVICES Service UUID definitions
 * @{ */
#define BLE_UUID_ALERT_NOTIFICATION_SERVICE                      0x1811     /**< Alert Notification service UUID. */
#define BLE_UUID_BATTERY_SERVICE                                 0x180F     /**< Battery service UUID. */
#define BLE_UUID_BLOOD_PRESSURE_SERVICE                          0x1810     /**< Blood Pressure service UUID. */
#define BLE_UUID_CURRENT_TIME_SERVICE                            0x1805     /**< Current Time service UUID. */
#define BLE_UUID_CYCLING_SPEED_AND_CADENCE                       0x1816     /**< Cycling Speed and Cadence service UUID. */
#define BLE_UUID_DEVICE_INFORMATION_SERVICE                      0x180A     /**< Device Information service UUID. */
#define BLE_UUID_GLUCOSE_SERVICE                                 0x1808     /**< Glucose service UUID. */
#define BLE_UUID_HEALTH_THERMOMETER_SERVICE                      0x1809     /**< Health Thermometer service UUID. */
#define BLE_UUID_HEART_RATE_SERVICE                              0x180D     /**< Heart Rate service UUID. */
#define BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE                  0x1812     /**< Human Interface Device service UUID. */
#define BLE_UUID_IMMEDIATE_ALERT_SERVICE                         0x1802     /**< Immediate Alert service UUID. */
#define BLE_UUID_LINK_LOSS_SERVICE                               0x1803     /**< Link Loss service UUID. */
#define BLE_UUID_NEXT_DST_CHANGE_SERVICE                         0x1807     /**< Next Dst Change service UUID. */
#define BLE_UUID_PHONE_ALERT_STATUS_SERVICE                      0x180E     /**< Phone Alert Status service UUID. */
#define BLE_UUID_REFERENCE_TIME_UPDATE_SERVICE                   0x1806     /**< Reference Time Update service UUID. */
#define BLE_UUID_RUNNING_SPEED_AND_CADENCE                       0x1814     /**< Running Speed and Cadence service UUID. */
#define BLE_UUID_SCAN_PARAMETERS_SERVICE                         0x1813     /**< Scan Parameters service UUID. */
#define BLE_UUID_TX_POWER_SERVICE                                0x1804     /**< TX Power service UUID. */
/** @} */

/** 
 * @defgroup UUID_CHARACTERISTICS Characteristic UUID definitions
 * @{ */
#define BLE_UUID_BATTERY_LEVEL_STATE_CHAR                        0x2A1B     /**< Battery Level State characteristic UUID. */
#define BLE_UUID_BATTERY_POWER_STATE_CHAR                        0x2A1A     /**< Battery Power State characteristic UUID. */
#define BLE_UUID_REMOVABLE_CHAR                                  0x2A3A     /**< Removable characteristic UUID. */
#define BLE_UUID_SERVICE_REQUIRED_CHAR                           0x2A3B     /**< Service Required characteristic UUID. */
#define BLE_UUID_ALERT_CATEGORY_ID_CHAR                          0x2A43     /**< Alert Category Id characteristic UUID. */
#define BLE_UUID_ALERT_CATEGORY_ID_BIT_MASK_CHAR                 0x2A42     /**< Alert Category Id Bit Mask characteristic UUID. */
#define BLE_UUID_ALERT_LEVEL_CHAR                                0x2A06     /**< Alert Level characteristic UUID. */
#define BLE_UUID_ALERT_NOTIFICATION_CONTROL_POINT_CHAR           0x2A44     /**< Alert Notification Control Point characteristic UUID. */
#define BLE_UUID_ALERT_STATUS_CHAR                               0x2A3F     /**< Alert Status characteristic UUID. */
#define BLE_UUID_BATTERY_LEVEL_CHAR                              0x2A19     /**< Battery Level characteristic UUID. */
#define BLE_UUID_BLOOD_PRESSURE_FEATURE_CHAR                     0x2A49     /**< Blood Pressure Feature characteristic UUID. */
#define BLE_UUID_BLOOD_PRESSURE_MEASUREMENT_CHAR                 0x2A35     /**< Blood Pressure Measurement characteristic UUID. */
#define BLE_UUID_BODY_SENSOR_LOCATION_CHAR                       0x2A38     /**< Body Sensor Location characteristic UUID. */
#define BLE_UUID_BOOT_KEYBOARD_INPUT_REPORT_CHAR                 0x2A22     /**< Boot Keyboard Input Report characteristic UUID. */
#define BLE_UUID_BOOT_KEYBOARD_OUTPUT_REPORT_CHAR                0x2A32     /**< Boot Keyboard Output Report characteristic UUID. */
#define BLE_UUID_BOOT_MOUSE_INPUT_REPORT_CHAR                    0x2A33     /**< Boot Mouse Input Report characteristic UUID. */
#define BLE_UUID_CURRENT_TIME_CHAR                               0x2A2B     /**< Current Time characteristic UUID. */
#define BLE_UUID_DATE_TIME_CHAR                                  0x2A08     /**< Date Time characteristic UUID. */
#define BLE_UUID_DAY_DATE_TIME_CHAR                              0x2A0A     /**< Day Date Time characteristic UUID. */
#define BLE_UUID_DAY_OF_WEEK_CHAR                                0x2A09     /**< Day Of Week characteristic UUID. */
#define BLE_UUID_DST_OFFSET_CHAR                                 0x2A0D     /**< Dst Offset characteristic UUID. */
#define BLE_UUID_EXACT_TIME_256_CHAR                             0x2A0C     /**< Exact Time 256 characteristic UUID. */
#define BLE_UUID_FIRMWARE_REVISION_STRING_CHAR                   0x2A26     /**< Firmware Revision String characteristic UUID. */
#define BLE_UUID_GLUCOSE_FEATURE_CHAR                            0x2A51     /**< Glucose Feature characteristic UUID. */
#define BLE_UUID_GLUCOSE_MEASUREMENT_CHAR                        0x2A18     /**< Glucose Measurement characteristic UUID. */
#define BLE_UUID_GLUCOSE_MEASUREMENT_CONTEXT_CHAR                0x2A34     /**< Glucose Measurement Context characteristic UUID. */
#define BLE_UUID_HARDWARE_REVISION_STRING_CHAR                   0x2A27     /**< Hardware Revision String characteristic UUID. */
#define BLE_UUID_HEART_RATE_CONTROL_POINT_CHAR                   0x2A39     /**< Heart Rate Control Point characteristic UUID. */
#define BLE_UUID_HEART_RATE_MEASUREMENT_CHAR                     0x2A37     /**< Heart Rate Measurement characteristic UUID. */
#define BLE_UUID_HID_CONTROL_POINT_CHAR                          0x2A4C     /**< Hid Control Point characteristic UUID. */
#define BLE_UUID_HID_INFORMATION_CHAR                            0x2A4A     /**< Hid Information characteristic UUID. */
#define BLE_UUID_IEEE_REGULATORY_CERTIFICATION_DATA_LIST_CHAR    0x2A2A     /**< IEEE Regulatory Certification Data List characteristic UUID. */
#define BLE_UUID_INTERMEDIATE_CUFF_PRESSURE_CHAR                 0x2A36     /**< Intermediate Cuff Pressure characteristic UUID. */
#define BLE_UUID_INTERMEDIATE_TEMPERATURE_CHAR                   0x2A1E     /**< Intermediate Temperature characteristic UUID. */
#define BLE_UUID_LOCAL_TIME_INFORMATION_CHAR                     0x2A0F     /**< Local Time Information characteristic UUID. */
#define BLE_UUID_MANUFACTURER_NAME_STRING_CHAR                   0x2A29     /**< Manufacturer Name String characteristic UUID. */
#define BLE_UUID_MEASUREMENT_INTERVAL_CHAR                       0x2A21     /**< Measurement Interval characteristic UUID. */
#define BLE_UUID_MODEL_NUMBER_STRING_CHAR                        0x2A24     /**< Model Number String characteristic UUID. */
#define BLE_UUID_UNREAD_ALERT_CHAR                               0x2A45     /**< Unread Alert characteristic UUID. */
#define BLE_UUID_NEW_ALERT_CHAR                                  0x2A46     /**< New Alert characteristic UUID. */
#define BLE_UUID_PNP_ID_CHAR                                     0x2A50     /**< PNP Id characteristic UUID. */
#define BLE_UUID_PROTOCOL_MODE_CHAR                              0x2A4E     /**< Protocol Mode characteristic UUID. */
#define BLE_UUID_RECORD_ACCESS_CONTROL_POINT_CHAR                0x2A52     /**< Record Access Control Point characteristic UUID. */
#define BLE_UUID_REFERENCE_TIME_INFORMATION_CHAR                 0x2A14     /**< Reference Time Information characteristic UUID. */
#define BLE_UUID_REPORT_CHAR                                     0x2A4D     /**< Report characteristic UUID. */
#define BLE_UUID_REPORT_MAP_CHAR                                 0x2A4B     /**< Report Map characteristic UUID. */
#define BLE_UUID_RINGER_CONTROL_POINT_CHAR                       0x2A40     /**< Ringer Control Point characteristic UUID. */
#define BLE_UUID_RINGER_SETTING_CHAR                             0x2A41     /**< Ringer Setting characteristic UUID. */
#define BLE_UUID_SCAN_INTERVAL_WINDOW_CHAR                       0x2A4F     /**< Scan Interval Window characteristic UUID. */
#define BLE_UUID_SCAN_REFRESH_CHAR                               0x2A31     /**< Scan Refresh characteristic UUID. */
#define BLE_UUID_SERIAL_NUMBER_STRING_CHAR                       0x2A25     /**< Serial Number String characteristic UUID. */
#define BLE_UUID_SOFTWARE_REVISION_STRING_CHAR                   0x2A28     /**< Software Revision String characteristic UUID. */
#define BLE_UUID_SUPPORTED_NEW_ALERT_CATEGORY_CHAR               0x2A47     /**< Supported New Alert Category characteristic UUID. */
#define BLE_UUID_SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR            0x2A48     /**< Supported Unread Alert Category characteristic UUID. */
#define BLE_UUID_SYSTEM_ID_CHAR                                  0x2A23     /**< System Id characteristic UUID. */
#define BLE_UUID_TEMPERATURE_MEASUREMENT_CHAR                    0x2A1C     /**< Temperature Measurement characteristic UUID. */
#define BLE_UUID_TEMPERATURE_TYPE_CHAR                           0x2A1D     /**< Temperature Type characteristic UUID. */
#define BLE_UUID_TIME_ACCURACY_CHAR                              0x2A12     /**< Time Accuracy characteristic UUID. */
#define BLE_UUID_TIME_SOURCE_CHAR                                0x2A13     /**< Time Source characteristic UUID. */
#define BLE_UUID_TIME_UPDATE_CONTROL_POINT_CHAR                  0x2A16     /**< Time Update Control Point characteristic UUID. */
#define BLE_UUID_TIME_UPDATE_STATE_CHAR                          0x2A17     /**< Time Update State characteristic UUID. */
#define BLE_UUID_TIME_WITH_DST_CHAR                              0x2A11     /**< Time With Dst characteristic UUID. */
#define BLE_UUID_TIME_ZONE_CHAR                                  0x2A0E     /**< Time Zone characteristic UUID. */
#define BLE_UUID_TX_POWER_LEVEL_CHAR                             0x2A07     /**< TX Power Level characteristic UUID. */
#define BLE_UUID_CSC_FEATURE_CHAR                                0x2A5C     /**< Cycling Speed and Cadence Feature characteristic UUID. */
#define BLE_UUID_CSC_MEASUREMENT_CHAR                            0x2A5B     /**< Cycling Speed and Cadence Measurement characteristic UUID. */
#define BLE_UUID_RSC_FEATURE_CHAR                                0x2A54     /**< Running Speed and Cadence Feature characteristic UUID. */
#define BLE_UUID_SC_CTRLPT_CHAR                                  0x2A55     /**< Speed and Cadence Control Point UUID. */
#define BLE_UUID_RSC_MEASUREMENT_CHAR                            0x2A53     /**< Running Speed and Cadence Measurement characteristic UUID. */
#define BLE_UUID_SENSOR_LOCATION_CHAR                            0x2A5D     /**< Sensor Location characteristic UUID. */
#define BLE_UUID_EXTERNAL_REPORT_REF_DESCR                       0x2907     /**< External Report Reference descriptor UUID. */
#define BLE_UUID_REPORT_REF_DESCR                                0x2908     /**< Report Reference descriptor UUID. */
/** @} */

/** 
 * @defgroup UUID_DESCRIPTORS Characteristic descriptor UUID definitions
 * @{ */
#define BLE_UUID_CHAR_DESC_CHAR_EXTEND_PROPERTY                  0x2900
#define BLE_UUID_CHAR_DESC_CHAR_USER_DESC                        0x2901
#define BLE_UUID_CHAR_DESC_CLIENT_CHAR_CONFIG                    0x2902
#define BLE_UUID_CHAR_DESC_SERVER_CHAR_CONFIG                    0x2903
#define BLE_UUID_CHAR_DESC_CHAR_FORMAT                           0x2904
#define BLE_UUID_CHAR_DESC_CHAR_AGGREGATE_FORMAT                 0x2905
/** @} */

/** 
 * @defgroup common length definitions
 * @{ */
 
#define BLE_GATT_MTU_SIZE_CUSTOM                                    (23)         /**< Default Length of MTU. user can configure*/
#define BLE_OPCODE_LENGTH                                        1           /**< Length of opcode . */
#define BLE_HANDLE_LENGTH                                        2           /**< Length of handle . */
#define BLE_SRV_ENCODED_REPORT_REF_LEN                           2           /**< The length of an encoded Report Reference Descriptor. */
#define BLE_CCCD_VALUE_LEN                                       2           /**< The length of a CCCD value. */
/** @} */

/** 
 * @defgroup LE advertising data definitions
 * @{ */
#define BLE_ADV_DATA_SIZE                                        31

/** 
 * @define mask for LE advertising data update 
 */
typedef uint16_t ble_adv_data_type_mask_t;

#define BLE_ADV_DATA_TYPE_NONE                         (0x0000)
#define BLE_ADV_DATA_TYPE_LOCAL_NAME                   (0x0001 << 1)
#define BLE_ADV_DATA_TYPE_APPEARANCE                   (0x0001 << 2)
#define BLE_ADV_DATA_TYPE_SERVICE_UUID                 (0x0001 << 3)
#define BLE_ADV_DATA_TYPE_SERVICE_16UUID               (0x0001 << 4) 
#define BLE_ADV_DATA_TYPE_SERVICE_32UUID               (0x0001 << 5)
#define BLE_ADV_DATA_TYPE_SERVICE_128UUID              (0x0001 << 6)
#define BLE_ADV_DATA_TYPE_SERVICE_DATA_16UUID          (0x0001 << 9)
#define BLE_ADV_DATA_TYPE_SERVICE_DATA_32UUID          (0x0001 << 10)
#define BLE_ADV_DATA_TYPE_SERVICE_DATA_128UUID         (0x0001 << 11)
#define BLE_ADV_DATA_TYPE_FLAGS                        (0x0001 << 12)
#define BLE_ADV_DATA_TYPE_MANU_DATA                    (0x0001 << 13)
#define BLE_ADV_DATA_TYPE_SLAVE_CONN_INTERVAL          (0x0001 << 14)
#define BLE_ADV_DATA_TYPE_TX_POWER_LEVEL               (0x0001 << 15)
#define BLE_ADV_DATA_TYPE_ADV_INTERVAL                 (0x0001 << 16)
#define BLE_ADV_DATA_TYPE_SERVICE_SOLICITATION_16UUID  (0x0001 << 17)
#define BLE_ADV_DATA_TYPE_SERVICE_SOLICITATION_32UUID  (0x0001 << 18)
#define BLE_ADV_DATA_TYPE_SERVICE_SOLICITATION_128UUID (0x0001 << 19)

/**
 * define mask for EIR 
 */
#define BLE_MAX_EIR_SDP_SIZE 30

#define BLE_EIR_FLAGS                               0x01
#define BLE_EIR_SERVICE_CLASS_16UUID_MORE           0x02
#define BLE_EIR_SERVICE_CLASS_16UUID_COMPLETE       0x03
#define BLE_EIR_SERVICE_CLASS_32UUID_MORE           0x04
#define BLE_EIR_SERVICE_CLASS_32UUID_COMPLETE       0x05
#define BLE_EIR_SERVICE_CLASS_128UUID_MORE          0x06
#define BLE_EIR_SERVICE_CLASS_128UUID_COMPLETE      0x07
#define BLE_EIR_NAME_SHORTENED                      0x08
#define BLE_EIR_NAME_COMPLETE                       0x09
#define BLE_EIR_TX_POWER                            0x0A
#define BLE_EIR_SLAVE_CONNINT_RANGE                 0x12 
#define BLE_EIR_APPEARANCE                          0x19
#define BLE_EIR_SERVICE_DATA_16UUID                 0x16
#define BLE_EIR_SERVICE_DATA_32UUID                 0x20
#define BLE_EIR_SERVICE_DATA_128UUID                0x21

#define BLE_EIR_MANUFACTURER                        0xFF
/** @} */

/** 
 * @defgroup LE connection interval definitions
 * @{ */
#define BLE_GATT_MAX_PROFILE_NUM                    (25)

#ifndef BLE_DEFAULT_MAX_CE_LENGTH
#define BLE_DEFAULT_MAX_CE_LENGTH                   0x140       /**< TBC: 200ms: 0x140 * 0,625ms */
#endif

#ifndef BLE_DEFAULT_MIN_CE_LENGTH
#define BLE_DEFAULT_MIN_CE_LENGTH                   0x60        /**< TBC: 60ms: 0x60 * 0.625ms */
#endif
/** @} */


/************************************************
*   Structures
*************************************************/

typedef enum
{
    BEF_LE_LIMITED_DISCOVERABLE_MODE       = 0x01,
    BEF_LE_GENERAL_DISCOVERABLE_MODE       = 0x02,
    BEF_BR_EDR_NOT_SUPPORTED               = 0x04,
    BEF_SIMUL_LE_BR_EDR_CAPABLE_CONTROLLER = 0x08,
    BEF_SIMUL_LE_BR_EDR_CAPABLE_HOST       = 0x10
} ble_advertise_flags_t;

typedef struct
{
    uint16_t manuDataLen;
    uint8_t  *manuData;
} adv_manufacturer_data_t;

/** 
 * Name type depedence by name length(shortened or complete) 
 */
typedef struct
{
    uint8_t nameLen;
    uint8_t *localName;
} adv_local_name_t;

typedef struct
{
    uint16_t minInterval;
    uint16_t maxInterval;
} slave_conn_interval_range_t;

/**
 * Spec: shall not contain more than one instance for each Service UUID data size  
 */
typedef struct
{
    uint8_t serviceNum;
    uint16_t *uuidList;
} service_16uuid_elem_t;

typedef struct
{
    uint8_t serviceNum;
    uint32_t *uuidList;
} service_32uuid_elem_t;

typedef struct
{
    uint8_t serviceNum;
    uint8_t *uuidList;
} service_128uuid_elem_t;

typedef struct
{    
    service_16uuid_elem_t  uuid16;
    service_32uuid_elem_t  uuid32;
    service_128uuid_elem_t uuid128;
} service_uuid_t;

typedef struct
{
    uint16_t uuidDataLen;
    uint8_t *uuidData;
} service_data_elem_t;

/**
 * Default support 16 UUID in current stage 
 */
typedef struct
{
    service_data_elem_t uuid16;
    service_data_elem_t uuid32;
    service_data_elem_t uuid128;
} service_data_t;

typedef struct
{
    service_16uuid_elem_t uuid16;      
    service_32uuid_elem_t uuid32; 
    service_128uuid_elem_t uuid128; 
} service_solicitation_t;

typedef struct
{ 
    int8_t                              txpower;
    uint16_t                            adv_int;         /**< advertising interval */
    uint16_t                            appearance; 
    service_uuid_t                      service;
    service_data_t                      service_data;
    adv_local_name_t                    name;            /**< local device name */
    ble_advertise_flags_t               flag;
    service_solicitation_t              solication;
    adv_manufacturer_data_t             manu;            /**< manufacturer specific data */
    slave_conn_interval_range_t         conn_int_range;  /**< slave prefered conn interval range */	
} ble_adv_data_type_t;

typedef struct
{
    bool set_scan_rsp;
    bool include_name;
    bool include_txpower;
    uint16_t min_interval;
    uint16_t max_interval;
    uint16_t appearance;
    uint16_t manufacturer_len;
    uint8_t *manufacturer_data;
    uint16_t service_data_len;
    uint8_t *service_data;
    uint16_t service_uuid_len;
    uint8_t service_uuid[16];
} bt_gap_set_adv_data_req_struct_t;

typedef struct
{
    uint16_t value_handle; /**< handle for  characteristic value decription */
    uint16_t cepd_handle;  /**< handle for characteristic extended properties decription */
    uint16_t cudd_handle;  /**< handle for characteristic user decription */
    uint16_t cccd_handle;  /**< handle for client characteristic configuration */
    uint16_t sccd_handle;  /**< handle for server characteristic configuration */
} ble_gatts_char_handles_t;

typedef struct
{
    ble_power_on_cnf_t power_on;
    ble_power_on_cnf_t power_off;
    ble_gap_connected_ind_t connected;
    ble_gap_disconnected_ind_t disconnected;
    ble_gap_advertising_ind_t advertising;
    ble_gap_report_rssi_ind_t tx_power;
    ble_gap_connection_param_updated_ind_t conn_params; /**< for conn interval update*/
} ble_gap_evt_t;

typedef struct
{
    ble_gatts_read_req_t read;
    ble_gatts_write_req_t         write;
    ble_gatt_exchange_mtu_req_t mtu_change;
    ble_gatts_handle_value_cnf_t  confirm;
    ble_gatts_execute_write_req_t excute_write;
} ble_gatts_evt_t;

typedef struct
{
    uint16_t evt_id;
} ble_evt_hdr_t;

typedef struct
{
    ble_evt_hdr_t header;
    struct {
        ble_gatts_evt_t gatts_evt;     
	ble_gap_evt_t gap_evt;
    } evt;
} ble_evt_t;

typedef enum
{
    BLE_GAP_ADV_INTERVAL_DEFAULT,
    BLE_GAP_ADV_INTERVAL_FAST_CONNECT,
    BLE_GAP_ADV_INTERVAL_LOW_POWER
} ble_gap_adv_interval_enum_t;


typedef enum
{
    BLE_GAP_CONN_PARAM_PRIORITY_DEFAULT,
    BLE_GAP_CONN_PARAM_HIGH_SPEED_ANDROID, /**< please use this High speed enum, when remote device is android*/
    BLE_GAP_CONN_PARAM_HIGH_SPEED_IOS,     /**< please use this High speed enum, when remote device is IOS*/
    BLE_GAP_CONN_PARAM_LOW_POWER
} ble_gap_conn_param_priority_t;

/************************************************
*   Utilities
*************************************************/
void *ble_gatts_alloc_memory(unsigned int size);
void ble_gatts_free_memory(void *pv);
void ble_gatt_uuid128_generate(uint8_t *uuid128, uint8_t *uuid128_base, const uint8_t *uuid16);
uint16_t ble_gatts_get_mtu_size(uint16_t conn_id);
ble_status_t ble_gap_set_advertising_data_reqest(bt_gap_set_adv_data_req_struct_t *req);
ble_status_t ble_gap_update_connection_interval(uint16_t conn_id, uint8_t *app_name, ble_gap_conn_param_priority_t conn_priority);
ble_status_t ble_gap_set_advertising_interval(uint16_t conn_id, uint8_t *app_name, ble_gap_adv_interval_enum_t adv_interval);
ble_gap_conn_param_priority_t ble_gap_get_current_connection_interval(ble_gap_connection_param_updated_ind_t *conn_updated);



#ifdef __cplusplus
}
#endif

#endif /**__BLE_GATTS_SRV_COMMON_H__*/

/** @} */



