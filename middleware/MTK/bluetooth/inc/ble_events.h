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

#ifndef __BLE_EVENTS_H__
#define __BLE_EVENTS_H__

/**
 * @addtogroup BLE
 * @{
 * @addtogroup CommonBLE Common
 * @{
 * @addtogroup CommonBLE_2 Bluetooth Low Energy events
 * @{
 * This section provides details on the Bluetooth Low Energy events.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b LE                         | Low Energy (LE, often referred to as "BLE" for "Bluetooth Low Energy") is a wireless personal area network technology designed and marketed by the Bluetooth Special Interest Group aimed at novel applications in the healthcare, fitness, beacons, security, and home entertainment industries. |
 * |\b RSSI                       | Received Signal Strength Indicator (RSSI) is a measurement of the power present in a received radio signal. |
 * |\b MTU                        | The Maximum Transmission Unit (MTU) is the size of the largest protocol data unit that the communications layer can pass onwards. |
 * |\b Out-of-Band                | An association mode, primarily designed for scenarios where an Out-of-Band mechanism is used to discover devices and to exchage or transfer cryptographic numbers used in the pairing process. |
 * |\b SM                         | Security Manager protocol defines the protocol and behavior to manage pairing, authentication and encryption between low energy devices. |
 * |\b GATT                       | Generic Attribute Profile. A service framework using the Attribute Protocol for discovering services, and for reading and writing characteristic values on a peer device. |
 */

/**
 * @defgroup ble_events_define Define
 * @{
 */
#define BLE_EVENT_BASE         0x2000                                                           /**< The base of the Bluetooth Low Energy events. */

#define BLE_GAP_EVENT_RANGE    0x100                                                            /**< The range of the Bluetooth Low Energy GAP events. */
#define BLE_GAP_EVENT_START    BLE_EVENT_BASE /* 0x2000 */                                      /**< The start of the Bluetooth Low Energy GAP events. */
#define BLE_GAP_EVENT_END      (BLE_GAP_EVENT_START + BLE_GAP_EVENT_RANGE - 1) /* 0x20FF */     /**< The end of the Bluetooth Low Energy GAP events. */

#define BLE_SM_EVENT_RANGE     0x100                                                            /**< The range of the SM events. */
#define BLE_SM_EVENT_START     (BLE_GAP_EVENT_END + 1) /* 0x2100 */                             /**< The start of the SM events. */
#define BLE_SM_EVENT_END       (BLE_SM_EVENT_START + BLE_SM_EVENT_RANGE - 1) /* 0x21FF */       /**< The end of the SM events. */

#define BLE_GATT_EVENT_RANGE   0x100                                                            /**< The range of GATT events. */
#define BLE_GATT_EVENT_START   (BLE_SM_EVENT_END + 1) /* 0x2200 */                              /**< The start of the GATT events. */
#define BLE_GATT_EVENT_END     (BLE_GATT_EVENT_START + BLE_GATT_EVENT_RANGE - 1) /* 0x22FF */   /**< The end of the GATT events. */

/**
 * @}
 */

/**
 * @defgroup ble_events_enum Enum
 * @{
 */

/** @brief This enumeration defines the Bluetooth Low Energy events. */
typedef enum {
    BLE_GAP_CONNECTED_IND = BLE_GAP_EVENT_START,    /**< A mandatory event, indicates the LE Create Connection is complete.
                                                         For more information on the event structure, please refer to #ble_gap_connected_ind_t. */

    BLE_GAP_DISCONNECTED_IND,                       /**< A mandatory event, indicates the LE Disconnect Connection is complete.
                                                         For more information on the event structure, please refer to #ble_gap_disconnected_ind_t. */

    BLE_GAP_ADVERTISING_IND,                        /**< A mandatory event, indicates a Bluetooth device or multiple Bluetooth devices have responded to
                                                         an active scan or received some information during a passive scan.
                                                         For more information on the event structure, please refer to #ble_gap_advertising_ind_t. */

    BLE_GAP_CONNECTION_PARAM_UPDATED_IND,           /**< A mandatory event, indicates the LE Connection Update is complete.
                                                         For more information on the event structure, please refer to #ble_gap_connection_param_updated_ind_t. */

    BLE_GAP_REPORT_RSSI_IND,                        /**< A mandatory event, indicates Read RSSI is complete.
                                                         For more information on the event structure, please refer to #ble_gap_report_rssi_ind_t. */

    BLE_POWER_ON_CNF,                               /**< A mandatory event, indicates the Bluetooth power on is complete.
                                                         For more information on the event structure, please refer to #ble_power_on_cnf_t. */

    BLE_POWER_OFF_CNF,                              /**< A mandatory event, indicates the Bluetooth power off is complete.
                                                         For more information on the event structure, please refer to #ble_power_off_cnf_t. */

    BLE_SM_PAIRING_REQ = BLE_SM_EVENT_START,        /**< A mandatory event, user should provide the pair attributes by calling #ble_sm_reply_pairing_param().
                                                         For more information on the event structure, please refer to #ble_sm_pairing_req_t. */

    BLE_SM_PAIRING_RESULT_IND,                      /**< An optional event, which can be ignored, to notify the pairing result.
                                                         For more information on the event structure, please refer to #ble_sm_pairing_result_ind_t. */

    BLE_SM_PAIRING_AUTHORIZATION_REQ,               /**< A mandatory event, only if a pairing authorization is enabled using #ble_sm_set_pairing_authorization().
                                                         User can accept or reject the pairing authorization by calling #ble_sm_reply_pairing_authorization().
                                                         For more information on the event structure, please refer to #ble_sm_pairing_authorization_req_t. */

    BLE_SM_INPUT_PASSKEY_REQ,                       /**< A mandatory event, in case the passkey entry method is used.
                                                         The input passkey UI is displayed when this event occurs, then the user can provide the passkey or reject it by calling #ble_sm_reply_input_passkey().
                                                         For more information on the event structure, please refer to #ble_sm_input_passkey_req_t. */

    BLE_SM_DISPLAY_PASSKEY_REQ,                     /**< A mandatory event, in case the passkey entry method is used.
                                                         The passkey can be modified before displaying it to the user or can be shown as is, when receiving this event.
                                                         Call #ble_sm_reply_display_passkey() to provide a different passkey or accept it.
                                                         For more information on the event structure, please refer to #ble_sm_display_passkey_req_t. */

    BLE_SM_OOB_DATA_REQ,                            /**< A mandatory event, in case the Out-of-Band data method is used.
                                                         Call #ble_sm_reply_oob_data() to provide the Out-of-Band data received.
                                                         For more information on the event structure, please refer to #ble_sm_oob_data_req_t. */

    BLE_SM_STORE_KEYS_REQ,                          /**< An optional event, could be ignored if the keys don't need to be stored.
                                                         Always store keys in the application when keys are ready during pairing.
                                                         For more information on the event structure, please refer to #ble_sm_store_keys_req_t. */

    BLE_SM_RETRIEVE_KEYS_REQ,                       /**< An optional event, could be ignored if the keys don't need to be stored.
                                                         Always retrieve keys from the application when needed.
                                                         For more information on the event structure, please refer to #ble_sm_retrieve_keys_req_t. */

    BLE_SM_ENCRYPT_RESULT_IND,                      /**< An optional event, ignored if the encryption result is irrelevant.
                                                         Notify the user when encryption is complete. The user receives this event after pairing or encryption.
                                                         For more information on the event structure, please refer to #ble_sm_encrypt_result_ind_t. */

    BLE_GATT_EXCHANGE_MTU_REQ = BLE_GATT_EVENT_START, /**< A mandatory event, call #ble_gatt_confirm_exchange_mtu() to send the expected MTU. */

    BLE_GATT_EXCHANGE_MTU_RSP,                      /**< A mandatory event, call #ble_gatt_exchange_mtu() to receive this event.
                                                         The client sends an exchange MTU request with its supported MTU size and receives a response back based on the server's MTU.
                                                         The final MTU exchange size should be the minimum of the client and server supported MTUs. */

    BLE_GATTC_DISCOVER_PRIMARY_SERVICES_RSP,        /**< A mandatory event, call #ble_gattc_discover_primary_services()
                                                         or #ble_gattc_discover_primary_services_by_uuid() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_discover_service_rsp_t. */

    BLE_GATTC_FIND_INCLUDED_SERVICES_RSP,           /**< A mandatory event, call #ble_gattc_find_included_services() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_find_included_service_rsp_t. */

    BLE_GATTC_DISCOVER_CHAR_RSP,                    /**< A mandatory event, call #ble_gattc_discover_char() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_discover_char_rsp_t. */

    BLE_GATTC_DISCOVER_CHAR_BY_UUID_RSP,            /**< A mandatory event, call #ble_gattc_discover_char_by_uuid() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_discover_char_rsp_t. */

    BLE_GATTC_DISCOVER_DESCRIPTORS_RSP,             /**< A mandatory event, call #ble_gattc_discover_descriptors() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_discover_descriptor_rsp_t. */

    BLE_GATTC_READ_CHAR_VALUE_RSP,                  /**< A mandatory event, call #ble_gattc_read_char_value() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_read_char_value_rsp_t. */

    BLE_GATTC_READ_CHAR_VALUE_BY_UUID_RSP,          /**< A mandatory event, call #ble_gattc_read_char_value_by_uuid() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_read_char_value_rsp_t. */

    BLE_GATTC_READ_LONG_CHAR_VALUE_RSP,             /**< A mandatory event, call #ble_gattc_read_long_char_value() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_read_char_value_rsp_t. */

    BLE_GATTC_READ_MULTIPLE_CHAR_VALUE_RSP,         /**< A mandatory event, call #ble_gattc_read_multiple_char_value() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_read_multiple_char_value_rsp_t. */

    BLE_GATTC_READ_DESCRIPTOR_RSP,                  /**< A mandatory event, call #ble_gattc_read_descriptor_value() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_read_char_descriptor_rsp_t. */

    BLE_GATTC_READ_LONG_DESCRIPTOR_RSP,             /**< A mandatory event, call #ble_gattc_read_long_descriptor_value() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_read_char_descriptor_rsp_t. */

    BLE_GATTC_WRITE_DESCRIPTOR_RSP,                 /**< A mandatory event, call #ble_gattc_write_descriptor_value() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_write_rsp_t. */

    BLE_GATTC_WRITE_CHAR_VALUE_RSP,                 /**< A mandatory event, call #ble_gattc_write_char_value() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_write_rsp_t. */

    BLE_GATTC_PREPARE_WRITE_RSP,                    /**< A mandatory event, call #ble_gattc_prepare_write() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_prepare_write_rsp_t. */

    BLE_GATTC_EXECUTE_WRITE_RSP,                    /**< A mandatory event, call #ble_gattc_execute_write() to receive this event.
                                                         For more information on the event structure, please refer to #ble_gattc_execute_write_rsp_t. */

    BLE_GATTC_HANDLE_VALUE_NOTIFICATION,            /**< An optional event, user receives this event when the server sends a notification to handle the value. */

    BLE_GATTC_HANDLE_VALUE_IND,                     /**< A mandatory event, user receives this event when the server sends an indication to handle the value.
                                                         For more information on the event structure, please refer to #ble_gattc_handle_value_ind_t.
                                                         Call #ble_gattc_send_handle_value_confirmation() to reply. */

    BLE_GATTS_HANDLE_VALUE_CNF,                     /**< A mandatory event, user receives this event when the client sends a confirmation to handle the value.
                                                         For more information on the event structure, please refer to #ble_gatts_handle_value_cnf_t. */

    BLE_GATTS_READ_REQ,                             /**< A mandatory event, user receives this event when the client sends a read request.
                                                         For more information on the event structure, please refer to #ble_gatts_read_req_t. */

    BLE_GATTS_WRITE_REQ,                            /**< A mandatory event, user receives this event when the client sends a write request.
                                                         For more information on the event structure, please refer to #ble_gatts_write_req_t. */

    BLE_GATTS_EXECUTE_WRITE_REQ,                    /**< A mandatory event, user receives this event when the client sends an execute write request.
                                                         For more information on the event structure, please refer to #ble_gatts_execute_write_req_t. */
    BLE_GATT_REQUEST_QUEUE_AVAILABLE,               /**< An optional event, user receives this event when the request queue is available. */

    BLE_EVENT_END
} ble_event_t;

/**
 * @}
 */

/**
 * @}
 * @}
 * @}
 */

#endif /* __BLE_EVENTS_H__ */

