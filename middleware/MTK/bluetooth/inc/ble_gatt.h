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

#ifndef __BLE_GATT_H__
#define __BLE_GATT_H__

/**
 * @addtogroup BLE
 * @{
 * @addtogroup GATT
 * @{
 * This section introduces the GATT APIs including details on how to use this module, enums, structures and functions.
 * The Generic Attribute Profile (GATT) defines a service framework using the Attribute Protocol,
 * that provides procedures and formats of services and their characteristics, including discovering,
 * reading, writing, notifying and indicating characteristics, as well as configuring the broadcast of characteristics.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b MTU                        | Maximum Transmission Unit, specifies the maximum size of a packet to send. |
 * |\b GATTC                      | GATT Client, is the device that initiates commands and requests towards the server. |
 * |\b GATTS                      | GATT Server, is the device that accepts incoming commands and requests from the client. |
 * |\b SM                         | Security Manager defines the protocol and behavior to manage pairing, authentication and encryption between low energy devices. |
 * |\b MITM                       | Man-in-the-middle attack, is an attack where the attacker secretly relays and possibly alters the communication between two parties who believe they are directly communicating with each other. |
 *
 * @section ble_gatt_api_usage How to use this module
 *
 * - Step1: Mandatory, get and save the connection ID from BLE_GAP once a BLE device is connected.
 *  - Sample code:
 *     @code
 *        static uint16_t conn_id = 0;
 *        void ble_gap_common_callback(ble_event_t event_id, const void* param)
 *        {
 *            switch (event_id) {
 *                case BLE_GAP_CONNECTED_IND: {
 *                    ble_gap_connected_ind_t *evt;
 *                    evt = (ble_gap_connected_ind_t *)param;
 *                    conn_id = evt->conn_id;
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 *     @endcode
 *
 * - Step2: Mandatory, implement the API #ble_gatts_common_callback() to handle the events sent by GATT client.
 *          An event handler is provided as a sample code for each API.
 *  - Sample code:
 *     @code
 *        void ble_gatts_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_GATT_EXCHANGE_MTU_REQ: {
 *                    ble_gatt_exchange_mtu_req_t *req = (ble_gatt_exchange_mtu_req_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTS_HANDLE_VALUE_CNF: {
 *                    ble_gatts_handle_value_cnf_t *hvc = (ble_gatts_handle_value_cnf_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTS_READ_REQ: {
 *                    ble_gatts_read_req_t *read_req = (ble_gatts_read_req_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTS_WRITE_REQ: {
 *                    ble_gatts_write_req_t *write_req = (ble_gatts_write_req_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTS_EXECUTE_WRITE_REQ: {
 *                    ble_gatts_execute_write_req_t *req = (ble_gatts_execute_write_req_t *)param;
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 *     @endcode
 *
 * - Step3: Mandatory, implement the API #ble_gattc_common_callback() to handle the events sent by GATT server.
 *          An event handler is provided as a sample code for each API.
 *  - Sample code:
 *     @code
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_GATT_EXCHANGE_MTU_RSP: {
 *                    ble_gatt_exchange_mtu_rsp_t *mtu_rsp = (ble_gatt_exchange_mtu_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_DISCOVER_PRIMARY_SERVICES_RSP: {
 *                    ble_gattc_discover_service_rsp_t *srvc_rsp = (ble_gattc_discover_service_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_FIND_INCLUDED_SERVICES_RSP: {
 *                    ble_gattc_find_included_service_rsp_t *srvc_rsp = (ble_gattc_find_included_service_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_DISCOVER_CHAR_RSP: {
 *                    ble_gattc_discover_char_rsp_t *char_rsp = (ble_gattc_discover_char_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_DISCOVER_CHAR_BY_UUID_RSP: {
 *                    ble_gattc_discover_char_rsp_t *char_rsp = (ble_gattc_discover_char_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_DISCOVER_DESCRIPTORS_RSP: {
 *                    ble_gattc_discover_descriptor_rsp_t *rsp = (ble_gattc_discover_descriptor_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_READ_CHAR_VALUE_RSP: {
 *                    ble_gattc_read_char_value_rsp_t *rsp = (ble_gattc_read_char_value_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_READ_CHAR_VALUE_BY_UUID_RSP: {
 *                    ble_gattc_read_char_value_rsp_t *rsp = (ble_gattc_read_char_value_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_READ_LONG_CHAR_VALUE_RSP: {
 *                    ble_gattc_read_char_value_rsp_t *rsp = (ble_gattc_read_char_value_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_READ_MULTIPLE_CHAR_VALUE_RSP: {
 *                    ble_gattc_read_multiple_char_value_rsp_t *rsp = (ble_gattc_read_multiple_char_value_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_READ_DESCRIPTOR_RSP: {
 *                    ble_gattc_read_char_descriptor_rsp_t *rsp = (ble_gattc_read_char_descriptor_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_WRITE_DESCRIPTOR_RSP: {
 *                    ble_gattc_write_rsp_t *rsp = (ble_gattc_write_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_WRITE_CHAR_VALUE_RSP: {
 *                    ble_gattc_write_rsp_t *rsp = (ble_gattc_write_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_PREPARE_WRITE_RSP: {
 *                    ble_gattc_prepare_write_rsp_t *rsp = (ble_gattc_prepare_write_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_EXECUTE_WRITE_RSP: {
 *                    ble_gattc_execute_write_rsp_t *rsp = (ble_gattc_execute_write_rsp_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_HANDLE_VALUE_NOTIFICATION: {
 *                    ble_gattc_handle_value_ind_t *hvx = (ble_gattc_handle_value_ind_t *)param;
 *                    break;
 *                }
 *                case BLE_GATTC_HANDLE_VALUE_IND: {
 *                    ble_gattc_handle_value_ind_t *hvx = (ble_gattc_handle_value_ind_t *)param;
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 *     @endcode
 */

#include <stdint.h>
#include "ble_events.h"
#include "ble_status.h"

/**
 * @defgroup bt_gatt_typedef Typedef
 * @{
 */

/**
 * @brief The data type of GATT characteristic properties.
 */

typedef uint8_t ble_gatt_char_properties_t;

/**
 * @}
 */

/**
 * @defgroup bt_gatt_define Define
 * @{
 */

/**
 * @brief The GATT characteristic properties.
 */

#define BLE_GATT_CHAR_PROP_BROADCAST         0x01  /**< If set, enables to broadcast the characteristic value configured by the server characteristic configuration descriptor. */
#define BLE_GATT_CHAR_PROP_READ              0x02  /**< If set, enables to read the characteristic value. */
#define BLE_GATT_CHAR_PROP_WRITE_WO_RESPONSE 0x04  /**< If set, enables to write the characteristic value with no response. */
#define BLE_GATT_CHAR_PROP_WRITE             0x08  /**< If set, enables to write the characteristic value with a response. */
#define BLE_GATT_CHAR_PROP_NOTIFY            0x10  /**< If set, permits notifications of a characteristic value with no acknowledgement. */
#define BLE_GATT_CHAR_PROP_INDICATE          0x20  /**< If set, permits indications of a characteristic value with an acknowledgement. */
#define BLE_GATT_CHAR_PROP_SIGNED_WRITE      0x40  /**< If set, permits signed writes to the characteristic value. */
#define BLE_GATT_EXT_PROP                    0x80  /**< If set, additional characteristic properties are defined in the characteristic extended properties descriptor. */

/**
 * @}
 */

/**
 * @addtogroup bt_gatt_typedef Typedef
 * @{
 */

/**
 * @brief The data type for GATT characteristic permission.
 */

typedef uint32_t ble_gatt_permission_t;

/**
 * @}
 */

/**
 * @addtogroup bt_gatt_define Define
 * @{
 */

/**
 * @brief The permission data types of GATT characteristics.
 */

#define BLE_GATT_PERM_READ                (1 << 0)  /**< If set, permits the client to read the characteristic value. */
#define BLE_GATT_PERM_READ_ENCRYPTED      (1 << 1)  /**< If set, permits the client to read the characteristic value when the client and server are paired with encryption by SM. */
#define BLE_GATT_PERM_READ_ENC_MITM       (1 << 2)  /**< If set, permits the client to read the characteristic value when the client and server are paired with encryption and MITM by SM. */
#define BLE_GATT_PERM_WRITE               (1 << 4)  /**< If set, permits the client to write the characteristic value. */
#define BLE_GATT_PERM_WRITE_ENCRYPTED     (1 << 5)  /**< If set, permits the client to write the characteristic value when the client and server are paired with encryption by SM. */
#define BLE_GATT_PERM_WRITE_ENC_MITM      (1 << 6)  /**< If set, permits the client to write the characteristic value when the client and server are paired with encryption and MITM by SM. */
#define BLE_GATT_PERM_WRITE_SIGNED        (1 << 7)  /**< If set, permits the client to write the characteristic value when the client and server are paired by SM and the client sends signed write request. */
#define BLE_GATT_PERM_WRITE_SIGNED_MITM   (1 << 8)  /**< If set, permits the client to write the characteristic value when the client and server are paired with MITM by SM and the client sends signed write request. */

/**
 * @brief The event type for a notification or an indication.
 */

#define BLE_GATT_HVX_INVALID      0x00    /**< Invalid value. */
#define BLE_GATT_HVX_NOTIFICATION 0x01    /**< Notification type. */
#define BLE_GATT_HVX_INDICATION   0x02    /**< Indication type. */

/**
 * @}
 */

/** @defgroup ble_gatt_enum Enum
  * @{
  */

/**
 *  @brief The type for write characteristic value request.
 */

typedef enum {
    BLE_GATT_WRITE_COMMAND = 1, /**< Write the characteristic value without a response. */
    BLE_GATT_WRITE_SIGNED,      /**< Signed write the characteristic value without a response. */
    BLE_GATT_WRITE_REQUEST      /**< Write the characteristic value with a response. */
} ble_gatt_write_type_t;

/**
 *  @brief The service type
 */

typedef enum {
    BLE_GATT_SERVICE_INVALID,   /**< Invalid.  */
    BLE_GATT_SERVICE_PRIMARY,   /**< Primary service type. */
    BLE_GATT_SERVICE_SECONDARY  /**< Secondary service type. */
} ble_gatt_service_type_t;

/**
 * @}
 */

/** @defgroup ble_gatt_struct Struct
  * @{
  */

/**
 *  @brief Substruture for #ble_gattc_discover_service_rsp_t.
 */

typedef struct {
    uint8_t  uuid[16];      /**< 128 bit service UUID. */
    uint16_t start_handle;  /**< A handle for the service declaration. */
    uint16_t end_handle;    /**< A handle for the last attribute within the service definition. */
} ble_gatt_service_t;

/**
 *  @brief Substruture for #ble_gattc_find_included_service_rsp_t.
 */

typedef struct {
    uint16_t handle;               /**< The attribute handle for the include service. */
    ble_gatt_service_t service;    /**< The service to be included. */
} ble_gatt_included_service_t;

/**
 *  @brief Substruture for #ble_gattc_discover_char_rsp_t.
 */

typedef struct {
    uint16_t handle;         /**< The attribute handle for characteristic declaration. */
    uint16_t value_handle;   /**< The attribute handle for characteristic value. */
    uint8_t  properties;     /**< The type of characteristic properties, such as #BLE_GATT_CHAR_PROP_BROADCAST and etc. */
    uint8_t  uuid[16];       /**< The UUID of the characteristic value. */
} ble_gatt_char_t;

/**
 *  @brief Substruture for #ble_gattc_discover_descriptor_rsp_t.
 */

typedef struct {
    uint16_t handle;        /**< The attribute handle for characteristic descriptor declaration. */
    uint8_t  uuid[16];      /**< The UUID of the characteristic descriptor. */
} ble_gatt_descriptor_t;

/**
 *  @brief This event (#BLE_GATTC_DISCOVER_PRIMARY_SERVICES_RSP) structure indicates a response from the server when the client sends a service discovery request.
 */

typedef struct {
    ble_status_t  result;          /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t conn_id;              /**< The connection ID for a BLE device connection. */
    uint16_t count;                /**< The count of service. */
    ble_gatt_service_t *service;   /**< The service array. */
} ble_gattc_discover_service_rsp_t;

/**
 *  @brief This event (#BLE_GATTC_FIND_INCLUDED_SERVICES_RSP) structure indicates a response from the server when the client sends a request to find included service.
 */

typedef struct {
    ble_status_t  result;                   /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t conn_id;                       /**< The connection ID for a BLE device connection. */
    uint16_t count;                         /**< The count of service. */
    ble_gatt_included_service_t *service;   /**< The service array. */
} ble_gattc_find_included_service_rsp_t;

/**
 *  @brief This event (#BLE_GATT_EXCHANGE_MTU_REQ) structure indicates exchange MTU request received from a client.
 */

typedef struct {
    uint16_t conn_id;       /**< The connection ID for a BLE device connection. */
    uint16_t expected_mtu;  /**< The expected MTU for a client. */
} ble_gatt_exchange_mtu_req_t;

/**
 *  @brief This event (#BLE_GATT_EXCHANGE_MTU_RSP) structure indicates a response from the server for a negotiatory MTU size when the client sends a exchange MTU request.
 */

typedef struct {
    ble_status_t  result;                 /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t conn_id;                     /**< The connection ID for a BLE device connection. */
    uint16_t expected_mtu;                /**< Negotiatory MTU size for both client and server if the operation completes successfully. */
} ble_gatt_exchange_mtu_rsp_t;

/**
 *  @brief This event (#BLE_GATTC_DISCOVER_CHAR_RSP or #BLE_GATTC_DISCOVER_CHAR_BY_UUID_RSP) structure indicates a server response
 *  on discovered characteristic information when the client sends a characteristic discovery request.
 */

typedef struct {
    ble_status_t   result;             /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t  conn_id;                 /**< The connection ID for a BLE device connection. */
    uint16_t  count;                   /**< The count of characteristic discovered. */
    ble_gatt_char_t *char_info;        /**< The characteristic array. */
} ble_gattc_discover_char_rsp_t;

/**
 *  @brief This event (#BLE_GATTC_DISCOVER_DESCRIPTORS_RSP) structure indicates a server response on the discovered descriptor information when the client sends a descriptor discovery request.
 */

typedef struct {
    ble_status_t  result;              /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t conn_id;                  /**< The connection ID for a BLE device connection. */
    uint16_t count;                    /**< The count of descriptor discovered. */
    ble_gatt_descriptor_t *descr;      /**< The descriptor array. */
} ble_gattc_discover_descriptor_rsp_t;

/**
 *  @brief Substruture for #ble_gattc_read_char_value_rsp_t.
 */

typedef struct {
    uint16_t handle;            /**< The attribute handle for the characteristic value declaration. */
    uint16_t length;            /**< The length of the characteristic value. */
    uint8_t *value;             /**< Characteristic value data. */
} ble_gattc_handle_value_t;

/**
 *  @brief This event (#BLE_GATTC_HANDLE_VALUE_IND or #BLE_GATT_HVX_NOTIFICATION) structure indicates a server indication to a client
 *  when characteristic indication enabled and characteristic value changed.
 */

typedef struct {
    uint16_t conn_id;  /**< The connection ID for a BLE device connection. */
    uint16_t handle;   /**< The handle of the characteristic value. */
    uint16_t length;   /**< The data length of characteristic value. */
    uint8_t  type;     /**< type: indication (#BLE_GATT_HVX_INDICATION) or notification (#BLE_GATT_HVX_NOTIFICATION). */
    uint8_t *data;     /**< Characteristic value data. */
} ble_gattc_handle_value_ind_t;

/**
 *  @brief This event (#BLE_GATTC_READ_CHAR_VALUE_RSP or #BLE_GATTC_READ_CHAR_VALUE_BY_UUID_RSP or #BLE_GATTC_READ_LONG_CHAR_VALUE_RSP)
 *  structure indicates a server response on the characteristic value information when the client sends a request to read the characteristic value.
 */

typedef struct {
    ble_status_t  result;                    /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t conn_id;                        /**< The connection ID for a BLE device connection. */
    uint16_t count;                          /**< The count of handle_value (>1 for #BLE_GATTC_READ_CHAR_VALUE_BY_UUID_RSP). */
    uint16_t offset;                         /**< The offset of characteristic value data(>0 for #BLE_GATTC_READ_LONG_CHAR_VALUE_RSP). */
    ble_gattc_handle_value_t *handle_value;  /**< The characteristic value array. */
} ble_gattc_read_char_value_rsp_t;

/**
 * @}
 */

/**
 * @addtogroup bt_gatt_typedef Typedef
 * @{
 */

/**
 * @brief The structure of characteristic descriptor rsp is the same as characteristic value rsp, should be for event #BLE_GATTC_READ_DESCRIPTOR_RSP.
 */

typedef ble_gattc_read_char_value_rsp_t ble_gattc_read_char_descriptor_rsp_t;

/**
 * @}
 */

/** @addtogroup ble_gatt_struct Struct
  * @{
  */

/**
 *  @brief This event (#BLE_GATTC_READ_MULTIPLE_CHAR_VALUE_RSP) structure indicates a server response when the client sends a request to read multiple characteristics.
 */

typedef struct {
    ble_status_t  result;    /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t handle;         /**< The handle of characteristic value to read. */
    uint16_t conn_id;        /**< The connection ID for a BLE device connection. */
    uint16_t length;         /**< The length of the characteristic value. */
    uint8_t *value;          /**< The data of characteristic value. */
} ble_gattc_read_multiple_char_value_rsp_t;

/**
 *  @brief This event (#BLE_GATTC_WRITE_CHAR_VALUE_RSP) structure indicates a server response when the client send a write request.
 */

typedef struct {
    ble_status_t  result;            /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t conn_id;                /**< The connection ID for a BLE device connection. */
    uint16_t handle;                 /**< The handle of the characteristic value to write. */
} ble_gattc_write_rsp_t;

/**
 *  @brief This event (#BLE_GATTC_PREPARE_WRITE_RSP) structure indicates a server response when the client sends a prepare write request
 *  to write long characteristic value, then user needs to send an execute write request to complete the whole write process.
 */

typedef struct {
    ble_status_t  result;           /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t conn_id;               /**< The connection ID for a BLE device connection. */
    uint16_t handle;                /**< The handle of the characteristic value to write. */
    uint16_t offset;                /**< The offset within the characteristic value to be written, to write the complete characteristic value,
                                                                        the offset should be set to 0x0000 for the first prepare write request, the offset for the subsequent
                                                                        prepare write request is the next octet that has yet to be written. */
    uint16_t length;                /**< The length of characteristic value has been written. */
    uint8_t  is_reliable;           /**< Whether reliable or not. */
    uint8_t *value;                 /**< If reliable write, it is the value has been written. */
} ble_gattc_prepare_write_rsp_t;

/**
 *  @brief This event (#BLE_GATTC_EXECUTE_WRITE_RSP) structure indicates a server response when the client sends an execute write request following the
 *  prepare write request.
 */

typedef struct {
    ble_status_t  result;            /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t conn_id;                /**< The connection ID for a BLE device connection. */
    uint16_t handle;                 /**< The handle of the characteristic value to write. */
} ble_gattc_execute_write_rsp_t;

/**
 *  @brief This event (#BLE_GATTS_HANDLE_VALUE_CNF) structure indicates a confirmation from the client when the server sends a characteristic value changed indication to the client.
 */

typedef struct {
    ble_status_t  result;            /**< Success or error codes defined as #BLE_STATUS_GATT_SUCCESS. */
    uint16_t conn_id;                /**< The connection ID for a BLE device connection. */
    uint16_t handle;                 /**< The handle of the characteristic value to indicate. */
} ble_gatts_handle_value_cnf_t;

/**
 *  @brief This event (#BLE_GATTS_READ_REQ) structure indicates a read request from a client to obtain characteristic value or descriptor.
 */

typedef struct {
    uint16_t  conn_id;   /**< The connection id for a BLE device connection. */
    uint16_t  count;     /**< The count of handles (>1 for read multiple characteristic). */
    uint16_t *handle;    /**< The characteristic (>1 for read multiple characteristic) / descriptor handle list. */
    uint16_t  offset;    /**< The characteristic (>0 for read long characteristic) / descriptor offset. */
} ble_gatts_read_req_t;

/**
 *  @brief This event (#BLE_GATTS_WRITE_REQ) structure indicates a client request to write the characteristic or descriptor value.
 */

typedef struct {
    uint16_t conn_id;            /**< The connection ID for a BLE device connection. */
    ble_gatt_write_type_t flags; /**< The type of write characteristic value request. */
    uint16_t handle;             /**< The characteristic / descriptor handle to write. */
    uint16_t offset;             /**< The characteristic (>0 for write long characteristic) / descriptor offset. */
    uint16_t length;             /**< The data length of characteristic value to write. */
    uint8_t  is_prepare;         /**< Is prepare write request (1) or not (0) */
    uint8_t *data;               /**< The characteristic value / descriptor data to write. */
} ble_gatts_write_req_t;

/**
 *  @brief This event (#BLE_GATTS_EXECUTE_WRITE_REQ) structure indicates a client request to execute write characteristic / descriptor value following
 *  prepare write request for long characteristic / descriptor.
 */

typedef struct {
    uint16_t conn_id;       /**< The connection ID for a BLE device connection. */
    uint8_t  is_execute;    /**< Is execute (1) or cancel write (0). */
} ble_gatts_execute_write_req_t;

/**
 * @}
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief               In this function, the client sends a request to exchange MTU size with the server, then the server replies with a response.
 * @param[in] conn_id   is the connection ID for a BLE device connection.
 * @param[in] rx_mtu    is the expected RX MTU for the client.
 * @return              If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                  #ble_gatt_confirm_exchange_mtu()
 * @note                Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                 Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t rx_mtu = 28;
 *        ble_status_t status;
 *        status = ble_gatt_exchange_mtu(conn_id, rx_mtu);
 *        if (status == BLE_STATUS_GATT_PENDING) {
 *            // A request has queued to send, wait for a confirmation from the server
 *        } else if (status == BLE_STATUS_GATT_NO_CONNECTION) {
 *           // No GATT connection for such conn_id
 *        } else if (status == BLE_STATUS_GATT_INVALID_PARAM) {
 *            // Input param rx_mtu is invalid
 *        } else if (status == BLE_STATUS_GATT_NO_RESOURCES) {
 *            // There is no resource to send a request
 *        }
 *
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATT_EXCHANGE_MTU_RSP: {
 *                      ble_gatt_exchange_mtu_rsp_t *mtu_rsp = (ble_gatt_exchange_mtu_rsp_t *)param;
 *                      OS_Report("[GATT]Exchange mtu rsp, conn_id:%x, result:%d, mtu:%d", mtu_rsp->conn_id, mtu_rsp->result, mtu_rsp->expected_mtu);
 *                      break;
 *                  }
 *
 *             }
 *        }
 * @endcode
 */

ble_status_t ble_gatt_exchange_mtu(uint16_t conn_id, uint16_t rx_mtu);

/**
 * @brief                   In this function, the server sends a confirmation to exchange the MTU size according to the client's request.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] server_rx_mtu is the negotiatory RX MTU size from server.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #ble_gatt_exchange_mtu()
 * @note                    Before using this function, please make sure the BLE devices is connected.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        ble_status_t status;
 *        void ble_gatts_common_callback(ble_event_t event_id, const void *param)
 *        {
 *          switch (event_id) {
 *              case BLE_GATT_EXCHANGE_MTU_REQ: {
 *                  ble_gatt_exchange_mtu_req_t *mtu_req = (ble_gatt_exchange_mtu_req_t *)param;
 *                  uint16_t mtu = 40;
 *                  // Should check if we could meets the request's MTU.
 *                  if (mtu_req->expected_mtu < mtu) {
 *                      mtu = mtu_req->expected_mtu;
 *                  }
 *                  ble_gatt_confirm_exchange_mtu(mtu_req->conn_id, mtu);
 *                  break;
 *          }
 *          default:
 *                   break;
 *            }
 *        }
 * @endcode
 */

ble_status_t ble_gatt_confirm_exchange_mtu(uint16_t conn_id, uint16_t server_rx_mtu);

/**
 * @brief                   Get negotiatory MTU size for the ATT connection between client and server.
 * @param[in]   conn_id     is the connection ID for a BLE device connection.
 * @param[out] mtu_size     is the negotiatory MTU size for both client and server.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Before using this function, please make sure the BLE device is connected.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t mtu_size;
 *        ble_status_t status;
 *        status = ble_gatt_get_mtu_size(conn_id, mtu_size);
 *        if (status == BLE_STATUS_GATT_SUCCESS) {
 *            // User can use mtu_size.
 *        }
 * @endcode
 */

ble_status_t ble_gatt_get_mtu_size(uint16_t conn_id, uint16_t *mtu_size);

/**
 * @brief                   Client sends a request to discover primary services from server.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] start_handle  is start handle, e.g.: 0x0001.
 * @param[in] end_handle    is end handle  e.g.: 0xFFFF.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #ble_gattc_discover_primary_services_by_uuid()
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t start_handle = 0x0001;
 *        uint16_t  end_handle = 0xFFFF;
 *        ble_gattc_discover_primary_services(conn_id, start_handle, end_handle);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_DISCOVER_PRIMARY_SERVICES_RSP: {
 *                      ble_gattc_discover_service_rsp_t *srvc_rsp = (ble_gattc_discover_service_rsp_t *)param;
 *                      int i;
 *                      if (BLE_STATUS_GATT_SUCCESS == srvc_rsp->result) {
 *                          OS_Report("[GATT]Discover Primary Service success-conn_id:%x-count:%d\n", srvc_rsp->conn_id, srvc_rsp->count);
 *                          for (i = 0; i < srvc_rsp->count; ++i) {
 *                              OS_Report("[GATT]Discover Primary Service: %d (start:%x, end:%x)\n", i, srvc_rsp->service[i].start_handle, srvc_rsp->service[i].end_handle);
 *                          }
 *                      } else {
 *                              OS_Report("[GATT]Discover Primary Service fail-conn_id:%x-error:%x-handle:%x", srvc_rsp->conn_id, srvc_rsp->result, srvc_rsp->service[0].start_handle);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_discover_primary_services(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle);

/**
 * @brief                   Client sends a request to discover primary services by specification UUID from server.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] start_handle  is start handle, e.g.: 0x0001.
 * @param[in] end_handle    is end handle  e.g.: 0xFFFF.
 * @param[in] uuid128       is 128-bit UUID data.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #ble_gattc_discover_primary_services()
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t start_handle = 0x0001;
 *        uint16_t  end_handle = 0xFFFF;
 *        uint8_t uuid128[16] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0d, 0x18, 0x00, 0x00};
 *        ble_gattc_discover_primary_services_by_uuid(conn_id, start_handle, end_handle, uuid128);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_DISCOVER_PRIMARY_SERVICES_RSP: {
 *                      ble_gattc_discover_service_rsp_t *srvc_rsp = (ble_gattc_discover_service_rsp_t *)param;
 *                      int i;
 *                      if (BLE_STATUS_GATT_SUCCESS == srvc_rsp->result) {
 *                          OS_Report("[GATT]Discover Primary Service success-conn_id:%x-count:%d\n", srvc_rsp->conn_id, srvc_rsp->count);
 *                          for (i = 0; i < srvc_rsp->count; ++i) {
 *                              OS_Report("[GATT]Discover Primary Service: %d (start:%x, end:%x)\n", i, srvc_rsp->service[i].start_handle, srvc_rsp->service[i].end_handle);
 *                          }
 *                      } else {
 *                          OS_Report("[GATT]Discover Primary Service fail-conn_id:%x-error:%x-handle:%x", srvc_rsp->conn_id, srvc_rsp->result, srvc_rsp->service[0].start_handle);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_discover_primary_services_by_uuid(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle, const uint8_t *uuid128);

/**
 * @brief                   Client sends a request to find included services from server.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] start_handle  is start handle, e.g.: 0x0001.
 * @param[in] end_handle    is end handle  e.g.: 0xFFFF.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                     Example
 * @code
 *        // BLE devices is connected, and conn_id has been obtained from BLE_GAP at step 1
 *        uint16_t start_handle = 0x0001;
 *        uint16_t end_handle = 0xFFFF;
 *        ble_gattc_find_included_services(conn_id, start_handle, end_handle);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_FIND_INCLUDED_SERVICES_RSP: {
 *                      ble_gattc_find_included_service_rsp_t *srvc_rsp = (ble_gattc_find_included_service_rsp_t *)param;
 *                      int i;
 *                      if (BLE_STATUS_GATT_SUCCESS == srvc_rsp->result) {
 *                          OS_Report("[GATT]Find Included Service success-conn_id:%x-count:%d\n", srvc_rsp->conn_id, srvc_rsp->count);
 *                          for (i = 0; i < srvc_rsp->count; ++i) {
 *                              OS_Report("[GATT]Find Included Service: %d (attHandle: %x, start:%x, end:%x)\n", i, srvc_rsp->service[i].handle, srvc_rsp->service[i].service.start_handle, srvc_rsp->service[i].service.end_handle);
 *                          }
 *                      } else {
 *                          OS_Report("[GATT]Find Included Service fail-conn_id:%x-error:%x-handle:%x", srvc_rsp->conn_id, srvc_rsp->result, srvc_rsp->service[0].handle);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_find_included_services(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle);

/**
 * @brief                   Client sends a request to discover characteristics from server.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] start_handle  is start handle, e.g.: 0x0001.
 * @param[in] end_handle    is end handle  e.g.: 0xFFFF.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t start_handle = 0x0001;
 *        uint16_t  end_handle = 0x0005;
 *        ble_gattc_discover_char(conn_id, start_handle, end_handle);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_DISCOVER_CHAR_RSP: {
 *                      ble_gattc_discover_char_rsp_t *char_rsp = (ble_gattc_discover_char_rsp_t *)param;
 *                      int i;
 *                      if (BLE_STATUS_GATT_SUCCESS == char_rsp->result) {
 *                          OS_Report("[GATT]Discover Character success-conn_id:%x-count:%d\n", char_rsp->conn_id, char_rsp->count);
 *                          for (i = 0; i < char_rsp->count; ++i) {
 *                              OS_Report("[GATT]Discover Character: %d (handle:%x, value_handle:%x)\n", i, char_rsp->char_info[i].handle, char_rsp->char_info[i].value_handle);
 *                          }
 *                      } else if (BLE_STATUS_GATT_CONNECTION_DISCONNECT != char_rsp->result) {
 *                          OS_Report("[GATT]Discover Character fail-conn_id:%x-error:%x-handle:%x", char_rsp->conn_id, char_rsp->result, char_rsp->char_info[0].handle);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_discover_char(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle);

/**
 * @brief                   Client sends a request to discover characteristics by UUID from server.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] start_handle  is start handle, e.g.: 0x0001.
 * @param[in] end_handle    is end handle  e.g.: 0xFFFF.
 * @param[in] uuid128       is 128-bit UUID data.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t start_handle = 0x0001;
 *        uint16_t  end_handle = 0x0005;
 *        uint8_t uuid128[16] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00};
 *        ble_gattc_discover_char_by_uuid(conn_id, start_handle, end_handle, uuid128);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_DISCOVER_CHAR_BY_UUID_RSP: {
 *                      ble_gattc_discover_char_rsp_t *char_rsp = (ble_gattc_discover_char_rsp_t *)param;
 *                      int i;
 *                      if (BLE_STATUS_GATT_SUCCESS == char_rsp->result) {
 *                          OS_Report("[GATT]Discover Character success-conn_id:%x-count:%d\n", char_rsp->conn_id, char_rsp->count);
 *                          for (i = 0; i < char_rsp->count; ++i) {
 *                              OS_Report("[GATT]Discover Character: %d (handle:%x, value_handle:%x)\n", i, char_rsp->char_info[i].handle, char_rsp->char_info[i].value_handle);
 *                          }
 *                      } else if (BLE_STATUS_GATT_CONNECTION_DISCONNECT != char_rsp->result) {
 *                          OS_Report("[GATT]Discover Character fail-conn_id:%x-error:%x-handle:%x", char_rsp->conn_id, char_rsp->result, char_rsp->char_info[0].handle);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_discover_char_by_uuid(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle, const uint8_t *uuid128);

/**
 * @brief                   Client sends a request to discover descriptors from server.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] start_handle  is start handle, e.g.: 0x0001.
 * @param[in] end_handle    is end handle  e.g.: 0xFFFF.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t start_handle = 0x0001;
 *        uint16_t end_handle = 0xFFFF;
 *        status = ble_gattc_discover_descriptors(conn_id, start_handle, end_handle);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_DISCOVER_DESCRIPTORS_RSP: {
 *                      ble_gattc_discover_descriptor_rsp_t *rsp = (ble_gattc_discover_descriptor_rsp_t *)param;
 *                      int i;
 *                      if (BLE_STATUS_GATT_SUCCESS == rsp->result) {
 *                          OS_Report("[GATT]Discover Descriptor success-conn_id:%x-count:%d\n", rsp->conn_id, rsp->count);
 *                          for (i = 0; i < rsp->count; ++i) {
 *                              OS_Report("[GATT]Discover Descriptor: %d-handle:%x\n", i, rsp->descr[i].handle);
 *                          }
 *                      } else if (BLE_STATUS_GATT_CONNECTION_DISCONNECT != rsp->result) {
 *                          OS_Report("[GATT]Discover Descriptor fail-conn_id:%x-error:%2x-handle:%2x", rsp->conn_id, rsp->result, rsp->descr[0].handle);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_discover_descriptors(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle);

/**
 * @brief               Client sends a request to read characteristic value from server.
 * @param[in] conn_id   is the connection ID for a BLE device connection.
 * @param[in] handle    is handle of a characteristic value.
 * @return              If success, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                 Example
 * @code
 *        // BLE devices is connected, and conn_id has been obtained from BLE_GAP at step 1
 *        uint16_t handle = 0x107;
 *        ble_gattc_read_char_value(conn_id, handle);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_READ_CHAR_VALUE_RSP: {
 *                      ble_gattc_read_char_value_rsp_t *rsp = (ble_gattc_read_char_value_rsp_t *)param;
 *                      if (BLE_STATUS_GATT_SUCCESS == rsp->result) {
 *                          OS_Report("[GATT]Read Character Value success-conn_id:%x-value:%s\n", rsp->conn_id, rsp->handle_value->value);
 *                      } else {
 *                          OS_Report("[GATT]Read Character Value fail-conn_id:%x-error:%x", rsp->conn_id, rsp->result);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_read_char_value(uint16_t conn_id, uint16_t handle);

/**
 * @brief                   Client sends request to read characteristic value from server.
 * @param[in] conn_id       is the connection id of BLE devices connection.
 * @param[in] start_handle  is start handle, e.g.: 0x0001.
 * @param[in] end_handle    is end handle  e.g.: 0xFFFF.
 * @param[in] uuid128       is 128-bit UUID data.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t start_handle = 0x0001;
 *        uint16_t end_handle = 0xFFFF;
 *        uint8_t name_uuid[] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00};
 *        ble_gattc_read_char_value(conn_id, start_handle, end_handle, name_uuid);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_READ_CHAR_VALUE_BY_UUID_RSP: {
 *                      ble_gattc_read_char_value_rsp_t *rsp = (ble_gattc_read_char_value_rsp_t *)param;
 *                      OS_Report("[GATTC]ReadCharValueByUUIDRsp-conn_id:%x-result:%x", rsp->conn_id, rsp->result);
 *                      if (BLE_STATUS_GATT_SUCCESS == rsp->result) {
 *                          OS_Report("[GATTC]ReadCharValueByUUIDRsp Content:\n\t%s", rsp->handle_value->value);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_read_char_value_by_uuid(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle, const uint8_t *uuid128);

/**
 * @brief               Client sends a request to read long characteristic value from server.
 * @param[in] conn_id   is the connection ID for a BLE device connection.
 * @param[in] handle    is the characteristic value handle.
 * @param[in] offset    is the start position to read for this request.
 * @return              If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                 Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t handle = 0x108;
 *        uint8_t offset = 30;
 *        ble_gattc_read_long_char_value(conn_id, handle, offset);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_READ_LONG_CHAR_VALUE_RSP: {
 *                      ble_gattc_read_char_value_rsp_t *rsp = (ble_gattc_read_char_value_rsp_t *)param;
 *                      OS_Report("[GATTC]ReadLongCharValueRsp-conn_id:%x-result:%x", rsp->conn_id, rsp->result);
 *                      if (BLE_STATUS_GATT_SUCCESS == rsp->result) {
 *                          OS_Report("[GATTC]ReadLongCharValueRsp Offset:%d, Content:\n\t%s", rsp->offset, rsp->handle_value->value);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_read_long_char_value(uint16_t conn_id, uint16_t handle, uint16_t offset);

/**
 * @brief               Client sends a request to read multiple characteristics value from server.
 * @param[in] conn_id   is the connection ID for a BLE device connection.
 * @param[in] count     is count of handles, max support 7 handles.
 * @param[in] handles   is a characteristics value handle list.
 * @return              If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                 Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t count = 2;
 *        uint16_t handles[] = {0x0104, 0x0109};
 *        ble_gattc_read_multiple_char_value(conn_id, count, handles);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_READ_MULTIPLE_CHAR_VALUE_RSP: {
 *                      uint8_t char1[4];
 *                      ble_gattc_read_multiple_char_value_rsp_t *rsp = (ble_gattc_read_multiple_char_value_rsp_t *)param;
 *                      OS_Report("[GATTC]ReadMultiCharValueRsp-conn_id:%x-result:%x", rsp->conn_id, rsp->result);
 *                      if (BLE_STATUS_GATT_SUCCESS == rsp->result) {
 *                          OS_Report("[GATTC]ReadMultiCharValueRsp value length:%d", rsp->length);
 *                          //User should know the length for each characteristic value.
 *                          // e.g.: if the characteristic value is 4 bytes
 *                          OS_MemCpy(char1, rsp->value, 4);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_read_multiple_char_value(uint16_t conn_id, uint16_t count, const uint16_t *handles);

/**
 * @brief               Client sends a request to write characteristic value to server.
 * @param[in] conn_id   is the connection ID for a BLE device connection.
 * @param[in] flags     is the type for write characteristic value request.
 * @param[in] handle    is the characteristic value handle to write.
 * @param[in] size      is the size of characteristic value to write.
 * @param[in] value     is the characteristic value data to write.
 * @return              If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                 Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t handle = 0x109;
 *        ble_gattc_write_char_value(conn_id, BLE_GATT_WRITE_COMMAND, handle, 6, "abcdef");
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_READ_CHAR_VALUE_RSP: {
 *                      ble_gattc_write_rsp_t *rsp = (ble_gattc_write_rsp_t *)param;
 *                      OS_Report("[GATTC]WriteCharValueRsp conn_id:%x, handle:%x, result:%d", rsp->conn_id, rsp->handle, rsp->result);
 *                  }
 *                  break;
 *             }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_write_char_value(uint16_t conn_id, ble_gatt_write_type_t flags, uint16_t handle, uint16_t size, const uint8_t *value);

/**
 * @brief               Client sends a request to read characteristic descriptor from server.
 * @param[in] conn_id   is the connection ID for a BLE device connection.
 * @param[in] handle    is handle of a characteristic descriptor.
 * @return              If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                 Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t handle = 0x311;
 *        ble_gattc_read_descriptor_value(conn_id, handle);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_READ_CHAR_VALUE_RSP: {
 *                      ble_gattc_read_char_descriptor_rsp_t *rsp = (ble_gattc_read_char_descriptor_rsp_t *)param;
 *                      if (BLE_STATUS_GATT_SUCCESS == rsp->result) {
 *                          OS_Report("[GATTC]ReadDescriptorRsp Content:\n\t%s", rsp->handle_value->value);
 *                      } else {
 *                          OS_Report("[GATT]Read Descriptor  fail-conn_id:%x-error:%x", rsp->conn_id, rsp->result);
 *                      }
 *                  }
 *                  break;
 *             }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_read_descriptor_value(uint16_t conn_id, uint16_t handle);

/**
 * @brief               Client sends a request to read long descriptor value from server.
 * @param[in] conn_id   is the connection ID for a BLE device connection.
 * @param[in] handle    is the descriptor handle.
 * @param[in] offset    is the start position to read for this request.
 * @return              If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                 Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t handle = 0x312;
 *        uint8_t offset = 30;
 *        ble_gattc_read_long_descriptor_value(conn_id, handle, offset);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_READ_LONG_CHAR_VALUE_RSP: {
 *                      ble_gattc_read_char_descriptor_rsp_t *rsp = (ble_gattc_read_char_descriptor_rsp_t *)param;
 *                      OS_Report("[GATTC]ReadLongDescriptorValueRsp-conn_id:%x-result:%x", rsp->conn_id, rsp->result);
 *                      if (BLE_STATUS_GATT_SUCCESS == rsp->result) {
 *                          OS_Report("[GATTC]ReadLongDescriptorValueRsp Offset:%d, Content:\n\t%s", rsp->offset, rsp->handle_value->value);
 *                      }
 *                  }
 *                  break;
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_read_long_descriptor_value(uint16_t conn_id, uint16_t handle, uint16_t offset);

/**
 * @brief               Client sends a request to write characteristic descriptor to server.
 * @param[in] conn_id   is the connection ID for a BLE device connection.
 * @param[in] handle    is handle of the characteristic descriptor to write.
 * @param[in] size      is length of the characteristic descriptor data to write .
 * @param[in] value     is the characteristic descriptor data to write.
 * @return              If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.
 * @par                 Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t handle = 0x311;
 *        ble_gattc_write_descriptor_value(conn_id, handle);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_WRITE_DESCRIPTOR_RSP: {
 *                      ble_gattc_write_rsp_t *rsp = (ble_gattc_write_rsp_t *)param;
 *                      OS_Report("[GATT]WriteDescrRsp, conn_id:%x, handle:%x, result:%d", rsp->conn_id, rsp->handle, rsp->result);
 *                  }
 *                  break;
 *             }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_write_descriptor_value(uint16_t conn_id, uint16_t handle, uint16_t size, const uint8_t *value);

/**
 * @brief                   Client sends a prepare request to write long characteristic or descriptor value to server.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] handle        is handle of the characteristic value or descriptor to write.
 * @param[in] offset        is the start written position for this write request.
 * @param[in] size          is length of the data to write.
 * @param[in] value         is data to write.
 * @param[in] is_reliable   is for user setting: if set 1, user will check the written data in RSP, or set 0, user will not check the written data.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #ble_gattc_execute_write()
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.\n
 *                          After calling this function, user needs to call #ble_gattc_execute_write() to complete the write process.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t handle = 0x308;
 *        ble_gattc_prepare_write(conn_id, handle, 1, 5, "HELLO", 0);
 *
 *        // Call #ble_gattc_prepare_write() more times for to prepare the left data to write.
 *        ble_gattc_execute_write(conn_id , 1); // Finally, call this API to execute the write.
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_PREPARE_WRITE_RSP: {
 *                      ble_gattc_prepare_write_rsp_t *rsp = (ble_gattc_prepare_write_rsp_t *)param;
 *                      if (BLE_STATUS_GATT_SUCCESS == rsp->result) {
 *                          // Call API to check the rsp->offset, rsp->length, rsp->is_reliable, rsp->value
 *                      } else {
 *                          OS_Report("[GATT]Prepare write  fail-conn_id:%x-error:%x", rsp->conn_id, rsp->result);
 *                      }
 *                  }
 *                  break;
 *             }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_prepare_write(uint16_t conn_id, uint16_t handle, uint16_t offset, uint16_t size, const uint8_t *value, uint8_t is_reliable);

/**
 * @brief                   Client sends an execute write request to complete write long characteristic or descriptor value to server.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] is_execute    is execute(1) or cancel(0) the write operation.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #ble_gattc_prepare_write()
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.\n
 *                          And user needs to call #ble_gattc_prepare_write() to prepare the write process.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t handle = 0x308;
 *        ble_gattc_prepare_write(conn_id, handle, 1, 5, "HELLO", 0);
 *
 *        // Call #ble_gattc_write_descriptor_value() more times to prepare the left data to write.
 *        ble_gattc_execute_write(conn_id , 1); // Finally, call this API to execute the write.
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTC_EXECUTE_WRITE_RSP: {
 *                      ble_gattc_execute_write_rsp_t *rsp = (ble_gattc_execute_write_rsp_t *)param;
 *                      OS_Report("[GATT]Execute write response, conn_id:%x, handle:%x, result:%d", rsp->conn_id, rsp->handle, rsp->result);
 *                  }
 *                  break;
  *             }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_execute_write(uint16_t conn_id, uint8_t is_execute);

/**
 * @brief                   Client sends a characteristic handle value confirmation after receiving indication from server.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] handle        is characteristic value handle.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gattc_common_callback() API is implemented to handle the events sent by GATT server.\n
 *                          And user needs to call #ble_gattc_prepare_write() to prepare the write process.
 * @par                     Example
 * @code
 *        // BLE device is connected, and conn_id is obtained from BLE_GAP at step 1.
 *        uint16_t handle = 0x308;
 *        ble_gattc_send_handle_value_confirmation(conn_id, handle);
 *
 *        void ble_gattc_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {.
 *                  case BLE_GATTC_HANDLE_VALUE_NOTIFICATION:
 *                  case BLE_GATTC_HANDLE_VALUE_IND: {
 *                      ble_gattc_handle_value_ind_t *hvx = (ble_gattc_handle_value_ind_t *)param;
 *                      OS_Report("[GATTC]HandleValue conn_id:%x, handle:%x, type:%d, data:%s", hvx->conn_id, hvx->handle, hvx->type, hvx->data);
 *                      if (BLE_GATT_HVX_INDICATION == hvx->type) {
 *                          ble_gattc_send_handle_value_confirmation(conn_id, hvx->handle);
 *                      }
 *                      break;
 *                  }
 *             }
 *        }
 * @endcode
 */

ble_status_t ble_gattc_send_handle_value_confirmation(uint16_t conn_id, uint16_t handle);

/**
 * @brief                   Server uses #ble_gatts_add_service() to add a service.
 * @param[in] type          is the service type to be added.
 * @param[in] uuid128       is 128 bit UUID of the service.
 * @param[in] num_handle    is the number of attribute in this service (such as, service declaration/include declaration/characteristic declaration/descriptor declaration, and etc.)
 * @param[out] handle       is the ATT handle of the service declaration.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Should be added before BLE device is connected, if devices already connected, server must notify client that service changed.
 * @par                     Example
 * @code
 *        ble_status_t status;
 *        static const uint8_t hr_uuid128[] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0d, 0x18, 0x00, 0x00};
 *        status = ble_gatts_add_service(BLE_GATT_SERVICE_PRIMARY, hr_uuid128, 9, &hr_svc_handle);
 *        if (status == BLE_STATUS_GATT_SUCCESS) {
 *            // Request has queued to send, wait for a confirmation from server.
 *        } else if (status == BLE_STATUS_GATT_NO_RESOURCES) {
 *            // There is no resource to send the request.
 *        } else if (status == BLE_STATUS_GATT_INVALID_PARAM) {
 *            // Input param size or value is invalid.
 *        }
 * @endcode
 */

ble_status_t ble_gatts_add_service(ble_gatt_service_type_t type, const uint8_t *uuid128, uint16_t num_handle, uint16_t *handle);

/**
 * @brief                               Server adds include service to the service.
 * @param[in] service_handle            is handle of the service that the include service will be added to.
 * @param[in] include_service_handle    is handle of the include service to be added.
 * @param[out] handle                   is the ATT handle of the included service declaration.
 * @return                              If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                                  #ble_gatts_add_service()
 * @note                                There should be two existing services, one is included service, another is the service to reference the included service.
 * @par                                 Example
 * @code
 *        ble_status_t status;
 *        static const uint8_t hr_uuid128[] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0d, 0x18, 0x00, 0x00};
 *        status = ble_gatts_add_service(BLE_GATT_SERVICE_PRIMARY, hr_uuid128, 9, &hr_svc_handle);
 *        // hr_svc_handle1 is another existing service.
 *        status = ble_gatts_add_included_service(hr_svc_handle, hr_svc_handle1, &hr_svc_inc_handle);
 *        if (status == BLE_STATUS_GATT_SUCCESS) {
 *            // Request has queued to send, wait for a confirmation from server.
 *        } else if (status == BLE_STATUS_GATT_NO_RESOURCES) {
 *            // There is no resource to send the request.
 *        } else if (status == BLE_STATUS_GATT_INVALID_PARAM) {
 *            // Input param size or value is invalid.
 *        }
 * @endcode
 */

ble_status_t ble_gatts_add_included_service(uint16_t service_handle, uint16_t include_service_handle, uint16_t *handle);

/**
 * @brief                           Server adds characteristic to the service.
 * @param[in] service_handle        is the existing service handle.
 * @param[in] char_info             is the info of characteristic to be added.
 * @param[in] permission            is the characteristic permission type, see also #BLE_GATT_PERM_READ,etc.
 * @param[out] value_handle         is the ATT handle of the characteristic declaration.
 * @return                          If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                              #ble_gatts_add_service()
 * @note                            There should be one existing service that can add characteristic.
 * @par                             Example
 * @code
 *        ble_status_t status;
 *        ble_gatt_char_t hrm_char;
 *        static const uint8_t hrm_uuid128[] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x37, 0x2a, 0x00, 0x00};
 *        OS_MemCpy(hrm_char.uuid, hrm_uuid128, 16);
 *        hrm_char.properties = BLE_GATT_CHAR_PROP_NOTIFY;
 *        // hr_svc_handle is service handle in #ble_gatts_add_service().
 *        status = ble_gatts_add_char(hr_svc_handle, &hrm_char, BLE_GATT_PERM_WRITE | BLE_GATT_PERM_READ, &hrm_char.value_handle);
 *        if (status == BLE_STATUS_GATT_SUCCESS) {
 *            // Request has queued to send, wait for a confirmation from server.
 *        } else if (status == BLE_STATUS_GATT_NO_RESOURCES) {
 *            // There is no resource to send the request.
 *        } else if (status == BLE_STATUS_GATT_INVALID_PARAM) {
 *            // Input param size or value is invalid.
 *        }
 * @endcode
 */

ble_status_t ble_gatts_add_char(uint16_t service_handle, const ble_gatt_char_t *char_info, uint32_t permission, uint16_t *value_handle);

/**
 * @brief                       Server adds characteristic descriptor to the service.
 * @param[in] service_handle    is the existing service handle.
 * @param[in] descr             is the info of characteristic descriptor to be added.
 * @param[in] permission        is the characteristic permission type, see also #BLE_GATT_PERM_READ,etc.
 * @param[out] handle           is the ATT handle of the characteristic descriptor.
 * @return                      If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                          #ble_gatts_add_service()
 * @note                        There should be one existing service that can add characteristic, descriptor will be added to the last characteristic of the given service_handle.
 * @par                         Example
 * @code
 *        ble_status_t status;
 *        ble_gatt_descriptor_t hrccc;
 *        static const uint8_t hrccc_uuid128[] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x02, 0x29, 0x00, 0x00};
 *        OS_MemCpy(hrccc.uuid, hrccc_uuid128, 16);
 *        // hr_svc_handle is service handle in #ble_gatts_add_service().
 *        status = ble_gatts_add_descriptor(hr_svc_handle, &hrccc, BLE_GATT_PERM_WRITE | BLE_GATT_PERM_READ, &hrccc.handle);
 *        if (status == BLE_STATUS_GATT_SUCCESS) {
 *            // Request has queued to send, wait for a confirmation from server.
 *        } else if (status == BLE_STATUS_GATT_NO_RESOURCES) {
 *            // There is no resource to send the request.
 *        } else if (status == BLE_STATUS_GATT_INVALID_PARAM) {
 *            // Input param size or value is invalid.
 *        }
 * @endcode
 */

ble_status_t ble_gatts_add_descriptor(uint16_t service_handle, const ble_gatt_descriptor_t *descr, uint32_t permission, uint16_t *handle);

/**
 * @brief                   Server uses to send service change indication.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] handle        is the ATT handle of the service declaration.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #ble_gatts_add_service()
 * @note                    If devices already connected, server must notify client that service has changed.
 * @par                     Example
 * @code
 *        ble_status_t status;
 *        uint16_t     hr_svc_handle = 0x1122;
 *        status = ble_gatts_send_service_change_indication(hr_svc_handle);
 *        if (status == BLE_STATUS_GATT_SUCCESS) {
 *            // Request has queued to send, wait for a confirmation from client.
 *        } else if (status == BLE_STATUS_GATT_NO_RESOURCES) {
 *            // There is no resource to send the request.
 *        } else if (status == BLE_STATUS_GATT_INVALID_PARAM) {
 *            // Input param size or value is invalid.
 *        }
 * @endcode
 */

ble_status_t ble_gatts_send_service_change_indication(uint16_t conn_id, uint16_t handle);

/**
 * @brief                   Server sends handle value indication or notification to client.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] handle        is handle of a characteristic.
 * @param[in] need_confirm  is used for indication (= 1) or notification (= 0).
 * @param[in] value_len     is the size of characteristic value.
 * @param[in] value         is the data of characteristic value.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #ble_gatts_add_service(), #ble_gatts_add_char(), #ble_gatts_add_descriptor
 * @note                    There should be one existing service that can add characteristic, descriptor will be added to the last characteristic of the given service_handle.
 * @par                     Example
 * @code
 *        ble_status_t status;
 *        uint16_t value_handle = 0x312;
 *        status = ble_gatts_send_handle_value_indication(conn_id, value_handle, 1, 4, "1122");  // For indication, need confirm.
 *        // ble_gatts_send_handle_value_indication(conn_id, character.value_handle, 0, 3, "123"); // For notification, no need confirm.
 *        if (status == BLE_STATUS_GATT_PENDING) {
 *            // Request has queued to send, wait for a confirmation from server.
 *        } else if (status == BLE_STATUS_GATT_NO_CONNECTION) {
 *            // No GATT connection for such conn_id.
 *        } else if (status == BLE_STATUS_GATT_NO_RESOURCES) {
 *            // There is no resource to send the request.
 *        } else if (status == BLE_STATUS_GATT_FAILED) {
 *            // Input param size or value is invalid.
 *        }
 *
 *
 *        void ble_gatts_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTS_HANDLE_VALUE_CNF: {
 *                      ble_gatts_handle_value_cnf_t * hvc = (ble_gatts_handle_value_cnf_t *)param;
 *                      OS_Report("[GATTS]Received confirmation to handle:%x, result:%d, conn_id:%d", hvc->handle, hvc->result, hvc->conn_id);
 *                      break;
 *              }
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gatts_send_handle_value_indication(uint16_t conn_id, uint16_t handle, uint8_t need_confirm, uint16_t value_len, const uint8_t *value);

/**
 * @brief                   Server sends a response for read request from client.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] result        is read result, such as #BLE_STATUS_GATT_SUCCESS, etc.
 * @param[in] handle        is the characteristic value or descriptor handle to read.
 * @param[in] offset        is characteristic value data offset for read long characteristic value request, or will be 0 for other read request.
 * @param[in] length        is the data length of the characteristic value.
 * @param[in] data          is the data of the characteristic value.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gatts_common_callback() API is implemented to handle the events sent by GATT client.
 * @par                     Example
 * @code
 *        // Call after receiving read request event.
 *        void ble_gatts_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTS_READ_REQ: {
 *                      ble_gatts_read_req_t *read_req = (ble_gatts_read_req_t *)param;
 *                      int a;
 *                      OS_Report("[GATTS]Read_req conn_id:%x count:%d offset:%d\n", read_req->conn_id, read_req->count, read_req->offset);
 *                      for (a = 0; a < read_req->count; ++a) {
 *                          OS_Report("[GATTS]Read_req handle[%d]:%x\n", a, read_req->handle[a]);
 *                      }
 *                      // Profiles should construct data to response.
 *                      ble_gatts_read_response(conn_id, BLE_STATUS_GATT_SUCCESS, read_req->handle[0], read_req->offset, 4, "HEHE");
 *                      break;
 *              }
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gatts_read_response(uint16_t conn_id, ble_status_t result, uint16_t handle, uint16_t offset, uint16_t length, const uint8_t *data);

/**
 * @brief                   Server sends a response for write request from client.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] result        is write result, such as #BLE_STATUS_GATT_SUCCESS, etc.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gatts_common_callback() API is implemented to handle the events sent by GATT client.
 * @par                     Example
 * @code
 *        // Call after receiving write request event
 *        ble_status_t status;
 *        void ble_gatts_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTS_WRITE_REQ: {
 *                      ble_gatts_write_req_t *write_req = (ble_gatts_write_req_t *)param;
 *                      OS_Report("[GATTS]Write_req1 conn_id:%d write_type:%d\n", write_req->conn_id, write_req->flags);
 *                      OS_Report("[GATTS]Write_req2 handle:%x offset:%d is_prepare:%d\n", write_req->handle, write_req->offset, write_req->is_prepare);
 *                      OS_Report("[GATTS]Write_req3 length:%d data:%s\n", write_req->length, write_req->data);
 *                      // Profiles should save data.
 *                      ble_gatts_write_response(conn_id, BLE_STATUS_GATT_SUCCESS);
 *                      break;
 *                   }
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gatts_write_response(uint16_t conn_id, ble_status_t result);

/**
 * @brief                   Server sends a response for prepare write request from client.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] result        is prepare write result, such as #BLE_STATUS_GATT_SUCCESS, etc.
 * @param[in] handle        is the characteristic value or descriptor handle to write.
 * @param[in] offset        is characteristic value data offset for write long characteristic value request, or will be 0 for other request.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #ble_gatts_execute_write_response()
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gatts_common_callback() API is implemented to handle the events sent by GATT client.
 * @par                     Example
 * @code
 *        // Call after receiving write request event.
 *        void ble_gatts_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTS_WRITE_REQ: {
 *                      ble_gatts_write_req_t *write_req = (ble_gatts_write_req_t *)param;
 *                      OS_Report("[GATTS]Write_req1 conn_id:%d write_type:%d\n", write_req->conn_id, write_req->flags);
 *                      OS_Report("[GATTS]Write_req2 handle:%x offset:%d is_prepare:%d\n", write_req->handle, write_req->offset, write_req->is_prepare);
 *                      OS_Report("[GATTS]Write_req3 length:%d data:%s\n", write_req->length, write_req->data);
 *                      // Profiles should save data to temp buffer first, then processed data by received next request #BLE_GATTS_EXECUTE_WRITE_REQ.
 *                      ble_gatts_prepare_write_response(conn_id, BLE_STATUS_GATT_SUCCESS, write_req->handle, write_req->offset);
 *                      break;
 *              }
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gatts_prepare_write_response(uint16_t conn_id, ble_status_t result, uint16_t handle, uint16_t offset);

/**
 * @brief                   Server sends a response for execute write request from client.
 * @param[in] conn_id       is the connection ID for a BLE device connection.
 * @param[in] result        is execute write result, such as #BLE_STATUS_GATT_SUCCESS, etc.
 * @param[in] handle        is the characteristic value or descriptor handle to write.
 * @return                  If the operation was successful, the returned value is #BLE_STATUS_GATT_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #ble_gatts_prepare_write_response()
 * @note                    Before using this function, please make sure the BLE device is connected, and the #ble_gatts_common_callback() API is implemented to handle the events sent by GATT client.
 * @par                     Example
 * @code
 *        // Call after receiving write request event.
 *        void ble_gatts_common_callback(ble_event_t event_id, const void *param)
 *        {
 *              switch (event_id) {
 *                  case BLE_GATTS_EXECUTE_WRITE_REQ: {
 *                      ble_gatts_execute_write_req_t *write_req = (ble_gatts_execute_write_req_t *)param;
 *                      OS_Report("[GATTS] execute Write req conn_id:%d is_execute:%d\n", write_req->conn_id, write_req->is_execute);
 *                      // Profiles should finally execute save data or not, according to the flag write_req->is_execute.
 *                      ble_gatts_execute_write_response(conn_id, BLE_STATUS_GATT_SUCCESS, write_handle);
 *                      break;
 *              }
 *              }
 *        }
 * @endcode
 */

ble_status_t ble_gatts_execute_write_response(uint16_t conn_id, ble_status_t result, uint16_t handle);

/**
 * @brief                   Static callback for GATT client user to listen event from server, client user needs to implement this API.
 * @param[in] event_id      is notification sent by GATT server.
 * @param[in] param         is parameter pointer as per the events.
 * @return                  None
 */

void ble_gattc_common_callback(ble_event_t event_id, const void *param);

/**
 * @brief                   Static callback for GATT server user to listen event from client, server user needs to implement this API.
 * @param[in] event_id      is notification sent by GATT client.
 * @param[in] param         is parameter pointer as per the events.
 * @return                  None
 */

void ble_gatts_common_callback(ble_event_t event_id, const void *param);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif /* __BLE_GATT_H__ */

