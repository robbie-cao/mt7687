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

#ifndef __BLE_GAP_H__
#define __BLE_GAP_H__
/**
 * @addtogroup BLE
 * @{
 * @addtogroup BLEGAP GAP
 * @{
 * This section describes the GAP APIs for Bluetooth Low Energy only.
 * The Bluetooth Low Energy GAP profile defines the generic procedures related to discovery of Bluetooth enabled devices (idle mode
 * procedures) and link management aspects of connecting to Bluetooth enabled devices (connecting mode
 * procedures). It also defines procedures related to the use of different security levels and
 * includes common format requirements for parameters accessible on the user interface level.
 *
 * @section ble_gap_api_usage How to use this module.
 *
 * - Step1: Mandatory, implement the API #ble_gap_common_callback() to handle the events sent by the Bluetooth Low Energy GAP.
 * - Step2: Mandatory, set the Bluetooth Low Energy address after the Bluetooth powers on.
 *  - Sample code:
 *     @code
 *        int32_t main(void)
 *        {
 *            ble_address_t addr;
 *            //Do something, such as power on the Bluetooth enabled device.
 *            addr.address[0] = 0xC0;
 *            addr.address[1] = 2;
 *            addr.address[2] = 3;
 *            addr.address[3] = 4;
 *            addr.address[4] = 5;
 *            addr.address[5] = 0xC4;
 *            ble_gap_set_random_address(&addr);
 *            //Do something, such as create a connection.
 *            return 0;
 *        }
 *     @endcode
 */

#include <stdint.h>
#include <stdbool.h>
#include "ble_status.h"
#include "ble_address.h"
#include "ble_events.h"

/** @defgroup ble_gap_enum Enum
  * @{
  */

/**
 *  @brief Bluetooth Low Energy scan type, invoked in #ble_gap_set_scan_params() API.
 */
typedef enum {
    BLE_GAP_PASSIVE_SCAN = 0,        /**< Passive scan, there is no #BLE_GAP_SCAN_RESPONSE type of advertising event report. */
    BLE_GAP_ACTIVE_SCAN              /**< Active scan, #BLE_GAP_SCAN_RESPONSE type of advertising event may be reported. */
} ble_gap_scan_type_t;

/**
 *  @brief Bluetooth Low Energy advertising type.
 */
typedef enum {
    BLE_GAP_ADVERTISING_CONNECTABLE_UNDIRECTED = 0,  /**< Connectable undirected advertising, can respond to scan and connect requests. */
    BLE_GAP_ADVERTISING_CONNECTABLE_DIRECTED,        /**< Connectable directed advertising, can respond to a connect request from a known device. */
    BLE_GAP_ADVERTISING_SCANNABLE_UNDIRECTED,        /**< Scannable undirected advertising, can respond to a scan request. */
    BLE_GAP_ADVERTISING_NON_CONNECTABLE_UNDIRECTED,  /**< Non connectable undirected advertising should not respond to scan and connect requests. */
    BLE_GAP_SCAN_RESPONSE                            /**< Only used when receiving an advertising event. */
} ble_gap_advertising_type_t;

/**
 *  @brief Bluetooth Low Energy role type of device for every connection.
 */
typedef enum {
    BLE_GAP_ROLE_CENTRAL = 0,    /**< Role of the device that initiates the connection. */
    BLE_GAP_ROLE_PERIPHERAL      /**< Role of the device that accepts the connection. */
} ble_gap_role_t;

/**
 *  @brief The filter type of scanning.
 */
typedef enum {
    BLE_GAP_SCAN_FILTER_ALLOW_ALL = 0,          /**< No filter. */
    BLE_GAP_SCAN_ALLOW_WHITE_LIST_ONLY          /**< Only report the advertising from white listed devices. */
} ble_gap_scan_filter_t;

/**
 *  @brief The filter type of a connection.
 */
typedef enum {
    BLE_GAP_NOT_USE_WHITE_LIST = 0,    /**< Connect to the given device. */
    BLE_GAP_USE_WHITE_LIST             /**< Connect to a white listed device. */
} ble_gap_initiator_filter_t;

/**
  * @}
  */


/** @defgroup ble_gap_struct Struct
  * @{
  */

/**
 *    @brief The connection characteristics.
 */
typedef struct {
    uint16_t                  conn_id;                /**< Range(0..0xF00], 0 is invalid ID. */
    ble_gap_role_t            role;                   /**< The role of the local device. */
    ble_address_type_t        peer_addr_type;         /**< The type of peer_addr. */
    ble_address_t             peer_addr;              /**< The address of the remote(peer) device. */
    ble_address_t             local_resolvable_addr;  /**< Supported in BT4.2. */
    ble_address_t             peer_resolvable_addr;   /**< Supported in BT4.2. */
    uint16_t                  conn_interval;          /**< Range[6..0xc80], Time = N* 1.25 msec. */
    uint16_t                  conn_latency;           /**< Range[0..0x1F3]. */
    uint16_t                  conn_timeout;           /**< range[0x0A..0xC80] Time = N * 10 msec. */
    uint8_t                   mast_clock_accuracy;    /**< Default 0. */
    uint8_t                   is_disconnecting;       /**< 1 if the connection is disconnecting, or 0. */
} ble_gap_connection_info_t;

/**
 *    @brief The time related characteristics of a connection.
 */
typedef struct {
    uint16_t min_conn_interval;         /**< Range[6..0xc80], Time = N* 1.25 msec. */
    uint16_t max_conn_interval;         /**< Range[6..0xc80], Time = N* 1.25 msec. */
    uint16_t conn_latency;              /**< Range[0..0x1F3]. */
    uint16_t conn_timeout;              /**< Range[0x0A..0xC80] Time = N * 10 msec. */
} ble_gap_connection_params_t;

/**
 *    @brief The scanning parameters.
 */
typedef struct {
    ble_gap_scan_type_t                  scan_type;                   /**< The type of scanning. */
    uint16_t                             interval;                    /**< Range[4..0x4000], Time = N * 0.625. */
    uint16_t                             window;                      /**< Range[4..0x4000], Time = N * 0.625. */
    ble_address_type_t                   own_address_type;            /**< Indicate which address will be used for scanning. */
    ble_gap_scan_filter_t                scanning_filter_policy;      /**< The filter used for scanning. */
} ble_gap_scan_params_t;

/**
 *    @brief This event structure presents the GAP connectivity parameters. #BLE_GAP_CONNECTED_IND event.
 *    @sa ble_gap_get_connection_info()
 */
typedef struct {
    ble_status_t     error_code;             /**< The event status. */
    uint16_t         conn_id;                /**< Range(0..0xF00], 0 is invalid ID. */
} ble_gap_connected_ind_t;

/**
 *    @brief This event structure presents the GAP disconnected event. #BLE_GAP_DISCONNECTED_IND event.
 *    @sa ble_gap_get_connection_info()
 */
typedef struct {
    ble_status_t      error_code;             /**< The event status. */
    uint16_t          conn_id;                /**< Range(0..0xF00], 0 is invalid ID. */
} ble_gap_disconnected_ind_t;

/**
 *    @brief This event structure presents the GAP connection parameter update. #BLE_GAP_CONNECTION_PARAM_UPDATED_IND event.
 */
typedef struct {
    uint16_t conn_id;               /**< Range(0..0xF00], 0 is invalid ID. */
    uint16_t conn_interval;         /**< Range[6..0xC80], Time = N * 1.25 msec. */
    uint16_t conn_latency;          /**< Range[0..0x1F3] */
    uint16_t conn_timeout;          /**< Range[0x0A..0xC80] Time = N * 10 msec. */
} ble_gap_connection_param_updated_ind_t;

/**
 *    @brief This event structure presents parameters to receive the RSSI. #BLE_GAP_REPORT_RSSI_IND event.
 *    @sa ble_gap_get_rssi()
 */
typedef struct {
    ble_status_t error_code;       /**< The event status. */
    uint16_t   conn_id;            /**< Range(0..0xF00], 0 is invalid ID. */
    int8_t      rssi;              /**< The value of the RSSI. */
} ble_gap_report_rssi_ind_t;

/**
 *    @brief This event structure presents advertising parameters. #BLE_GAP_ADVERTISING_IND event.
 */
typedef struct {
    ble_gap_advertising_type_t   advertising_type;          /**< Advertising event type. */
    ble_address_type_t           peer_addr_type;            /**< The type of peer_addr. */
    int8_t                       rssi;                      /**< The RSSI. */
    ble_address_t                peer_addr;                 /**< The address of the remote(peer) device. */
    uint16_t                     data_len;                  /**< The number of bytes in data. */
    uint8_t                      data[1];                   /**< The advertising buffer. */
} ble_gap_advertising_ind_t;

/**
 *  @brief This event structure presents Bluetooth power on parameters. #BLE_POWER_ON_CNF event.
 */
typedef struct {
    uint8_t result;                /**< If successful, return #BLE_STATUS_SUCCESS, otherwise return an error. */
} ble_power_on_cnf_t;

/**
 *  @brief This event structure presents Bluetooth power off parameters. #BLE_POWER_OFF_CNF event.
 */
typedef struct {
    uint8_t result;                /**< If successful, return #BLE_STATUS_SUCCESS, otherwise return an error. */
} ble_power_off_cnf_t;

/**
  * @}
  */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief              This function is a user defined API for a static callback to listen to the Bluetooth Low Energy GAP events.
 * @param[in] event_id is a notification sent by the GAP.
 * @param[in] param    is a pointer to the parameter of each event.
 * @return             None.
 */
void ble_gap_common_callback(ble_event_t event_id, const void *param);

/**
 * @brief                      This function is a user defined API for giving public address to the controller.
 * @param[out] local_addr      is the device address.
 * @return                     None.
 */
void ble_gap_get_local_address(ble_address_t *local_addr);

/**
 * @brief              This function sets the random address for the device, It's suggested to invoke after the Bluetooth is powered on.
 * @param[in] addr     is the device address to be set.
 * @return             #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @note               Bluetooth must be on, and the address will be reset when the Bluetooth turns on and off.
 */
ble_status_t ble_gap_set_random_address(const ble_address_t *addr);

/**
 * @brief              This function obtains the random address of the device. It's suggested to invoke after the Bluetooth is powered on and random address is set.
 * @param[out] addr    is the device address to return.
 * @return             #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @sa                 ble_gap_set_random_address()
 */
ble_status_t ble_gap_get_random_address(ble_address_t *addr);

/**
 * @brief                       This function sets the advertising buffer. It provides data to scanning packets that have a data field.
 * @param[in] adv_data          is the content of an advertising buffer.
 * @param[in] adv_len           is the length of adv_data with a maximum size of 31 bytes.
 * @param[in] scan_rsp_data     is the content of scanning response buffer.
 * @param[in] scan_rsp_len      is the length of scan_rsp_data with a maximum size of 31 bytes.
 * @return                      #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @note                        The advertiser must be stopped before setting a content.
 * @par       Example
 * @code
 *        void main(void)
 *        {
 *            uint8_t buff[31] = {2, 1, 0x1A};
 *            char* LE_name = "my xf_dev";
 *            uint8_t len = 0;
 *            ble_gap_stop_advertiser();
 *            len = strlen(LE_name);
 *            buff[3] = len + 1;
 *            buff[4] = 9;
 *            memcpy(buff + 5, LE_name, len);
 *            ble_gap_set_advertising_data(
 *                buff,
 *                len + 5,
 *                NULL,
 *                0
 *                );
 *            return;
 *        }
 * @endcode
 */
ble_status_t ble_gap_set_advertising_data(
    const void *adv_data,
    uint16_t adv_len,
    const void *scan_rsp_data,
    uint16_t scan_rsp_len
);

/**
 * @brief                   This function adds a single device to the white list stored in the controller.
 * @param[in] addr_type     is the type of the device address.
 * @param[in] addr          is the address of the device to be added to the white list.
 * @return                  #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 */
ble_status_t ble_gap_add_white_list(ble_address_type_t addr_type, const ble_address_t *addr);

/**
 * @brief                   This function cleans up the white listed devices added by #ble_gap_add_white_list().
 * @return                  #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @sa                      ble_gap_add_white_list()
 */
ble_status_t ble_gap_clean_white_list(void);

/**
 * @brief                          This function sets the advertising parameters.
 * @param[in] min_interval         is the minimum advertising interval for undirected / directed advertising.
                                             Range: 0x0020 to 0x4000.
                                             Default: N = 0x0800 (1.28 second).
                                             Time = N * 0.625 msec.
 * @param[in] max_interval         is the maximum advertising interval for undirected / directed advertising.
                                             Range: 0x0020 to 0x4000.
                                             Default: N = 0x0800 (1.28 second).
                                             Time = N * 0.625 msec.
 * @param[in] advertising_type     is the type of advertising.
 * @param[in] own_addr_type        is the address used by the advertiser.
 * @param[in] peer_addr_type       is the type of peer_addr for directed advertising.
 * @param[in] peer_addr            is the address of device advertising used in the directed advertising.
 * @param[in] channel_mask         is the channel mask used for directed advertising.
                                             xxxxxxx1b: Use channel 37.
                                             xxxxxx1xb: Use channel 38.
                                             xxxxx1xxb: Use channel 39.
                                             xxxxx111b: Default (all channels enabled).
                                             00000000b: Invalid.
 * @param[in] filter_policy        0x00: Default, process scan and connection requests from all devices.
                                   0x01: Process connection requests from all devices and only scan requests from
                                         devices that are in the white list.
                                   0x02: Process scan requests from all devices and only connection requests from
                                         devices that are in the white list.
                                   0x03: Process scan and connection requests only from devices in the white list.
 * @return                         #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @note                           Advertiser must be stopped before setting up the parameters.
 * @par       Example
 * @code
 *        void main(void)
 *        {
 *            ble_gap_stop_advertiser();
 *            ble_gap_set_advertising_params(
 *                0x100,
 *                0x100,
 *                BLE_GAP_ADVERTISING_CONNECTABLE_UNDIRECTED,
 *                BLE_ADDRESS_TYPE_RANDOM,
 *                BLE_ADDRESS_TYPE_PUBLIC,
 *                NULL,
 *                0x7,
 *                0
 *                );
 *            return;
 *        }
 * @endcode
 */
ble_status_t ble_gap_set_advertising_params(
    uint16_t min_interval,
    uint16_t max_interval,
    ble_gap_advertising_type_t advertising_type,
    ble_address_type_t own_addr_type,
    ble_address_type_t peer_addr_type,
    const ble_address_t *peer_addr,
    uint8_t channel_mask,
    uint8_t filter_policy
);

/**
 * @brief            This function starts the advertiser.
 * @return           #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @note             Advertising data and parameters must be set before starting the advertiser.
 * @par       Example
 * @code
 *        void main(void)
 *        {
 *            uint8_t buff[31] = {2, 1, 0x1A};
 *            char* LE_name = "my xf_dev";
 *            uint8_t len = 0;
 *            ble_gap_stop_advertiser();
 *            len = strlen(LE_name);
 *            buff[3] = len + 1;
 *            buff[4] = 9;
 *            memcpy(buff + 5, LE_name, len);
 *            ble_gap_set_advertising_data(
 *                buff,
 *                len + 5,
 *                NULL,
 *                0
 *                );
 *            ble_gap_set_advertising_params(
 *                0x100,
 *                0x100,
 *                BLE_GAP_ADVERTISING_CONNECTABLE_UNDIRECTED,
 *                BLE_ADDRESS_TYPE_RANDOM,
 *                BLE_ADDRESS_TYPE_PUBLIC,
 *                NULL,
 *                0x7,
 *                0
 *                );
 *            ble_gap_start_advertiser();
 *            return;
 *        }
 * @endcode
 */
ble_status_t ble_gap_start_advertiser(void);

/**
 * @brief       This function stops the advertiser.
 * @return      #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @sa          ble_gap_start_advertiser().
 */
ble_status_t ble_gap_stop_advertiser(void);

/**
 * @brief                  This function updates the connection parameters. A #BLE_GAP_CONNECTION_PARAM_UPDATED_IND event is reported when finished.
 * @param[in] conn_id      is the connection ID used in the settings.
 * @param[in] conn_params  are the parameters used for the settings.
 * @return                 #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @note                   #BLE_GAP_CONNECTION_PARAM_UPDATED_IND event is received after a successful update.
 */
ble_status_t ble_gap_update_connection_params(
    uint16_t conn_id,
    const ble_gap_connection_params_t *conn_params
);

/**
 * @brief                   This function sets the scan parameters.
 * @param[in] scan_params   are the parameters for the scanner.
 * @return                  #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @sa                      ble_gap_start_scanner().
 */
ble_status_t ble_gap_set_scan_params(const ble_gap_scan_params_t *scan_params);

/**
 * @brief        Start the scanner.
 * @return       #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @par       Example
 * @code
 *        void main(void)
 *        {
 *            //Do something.
 *            ble_gap_start_scanner();
 *            return;
 *        }
 *        void ble_gap_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_GAP_ADVERTISING_IND: {
 *                    ble_gap_advertising_ind_t *adv;
 *
 *                    adv = (ble_gap_advertising_ind_t *) param;
 *
 *                    //Handle an advertising event.
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 * @endcode
 */
ble_status_t ble_gap_start_scanner(void);

/**
 * @brief     This function stops the scanner.
 * @return    #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 */
ble_status_t ble_gap_stop_scanner(void);

/**
 * @brief                         This function creates a Link Layer connection to a connectable advertiser.
 * @param[in] filter              is the connection filter.
 * @param[in] peer_addr_type      is the type of peer_addr used for establishing a connection.
 * @param[in] peer_addr           is the address of the device to be connected. Ignore if using white listed devices.
 * @param[in] own_addr_type       is the type of address used in the connection request packets.
 * @param[in] conn_params         are the time related parameters.
 * @return                        #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @note                          #BLE_GAP_CONNECTED_IND event is received when connection is established successfully.
 * @par       Example
 * @code
 *        void main(void)
 *        {
 *            ble_address_t addr;
 *            ble_gap_connection_params_t conn_params;
 *
 *            conn_params.min_conn_interval = 0x100;
 *            conn_params.max_conn_interval = 0x400;
 *            conn_params.conn_latency = 0;
 *            conn_params.conn_timeout = 0x300;
 *
 *            addr.address[5] = 0xFF;
 *            addr.address[4] = 0x09;
 *            addr.address[3] = 0x2A;
 *            addr.address[2] = 0x3A;
 *            addr.address[1] = 0x74;
 *            addr.address[0] = 0xC0;
 *
 *            ble_gap_connect(
 *                BLE_GAP_USE_WHITE_LIST,
 *                BLE_ADDRESS_TYPE_RANDOM,
 *                &addr,
 *                BLE_ADDRESS_TYPE_RANDOM,
 *                &conn_params
 *                );
 *            return;
 *        }
 *        void ble_gap_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_GAP_CONNECTED_IND: {
 *                    ble_gap_connected_ind_t *evt;
 *
 *                    evt = (ble_gap_connected_ind_t *) param;
 *
 *                    //Handle a connected event.
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 * @endcode
 */
ble_status_t ble_gap_connect(
    ble_gap_initiator_filter_t filter,
    ble_address_type_t      peer_addr_type,
    const ble_address_t *peer_addr,
    ble_address_type_t      own_addr_type,
    const ble_gap_connection_params_t *conn_params
);

/**
 * @brief          This function cancels the connection procedure initiated by #ble_gap_connect().
 * @return         #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @sa             ble_gap_connect()
 */
ble_status_t ble_gap_cancel_connecting(void);

/**
 * @brief                 This function disconnects a Link Layer connection.
 * @param[in] conn_id     is the connection ID.
 * @return                #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @sa                    ble_gap_connect()
 * @par       Example
 * @code
 *        void main(void)
 *        {
 *            uint16_t conn_id;
 *            ble_address_t addr;
 *            addr.address[5] = 0xFF;
 *            addr.address[4] = 0x09;
 *            addr.address[3] = 0x2A;
 *            addr.address[2] = 0x3A;
 *            addr.address[1] = 0x74;
 *            addr.address[0] = 0xC0;
 *            conn_id = ble_gap_get_connection_id(&addr);
 *            if (0 != conn_id) {
 *                ble_gap_disconnect(conn_id);
 *            }
 *            return;
 *        }
 * @endcode
 */
ble_status_t ble_gap_disconnect(uint16_t conn_id);

/**
 * @brief                      This function reads the Received Signal Strength Indication (RSSI) value from the controller.
 * @param[in] conn_id          is the connection ID.
 * @return                     #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 * @par       Example
 * @code
 *        void main(void)
 *        {
 *            uint16_t conn_id;
 *            ble_address_t addr;
 *            addr.address[5] = 0xFF;
 *            addr.address[4] = 0x09;
 *            addr.address[3] = 0x2A;
 *            addr.address[2] = 0x3A;
 *            addr.address[1] = 0x74;
 *            addr.address[0] = 0xC0;
 *            conn_id = ble_gap_get_connection_id(&addr);
 *            if (0 != conn_id) {
 *                ble_gap_get_rssi(conn_id);
 *            }
 *            return;
 *        }
 *        void ble_gap_common_callback(ble_event_t event_id, const void *param)
 *        {
 *            switch (event_id) {
 *                case BLE_GAP_REPORT_RSSI_IND: {
 *                    ble_gap_report_rssi_ind_t *rssi_evt;
 *
 *                    rssi_evt = (ble_gap_report_rssi_ind_t *) param;
 *
 *                    //Handle an advertising event.
 *                    break;
 *                }
 *                default:
 *                    break;
 *            }
 *        }
 * @endcode
 */
ble_status_t ble_gap_get_rssi(uint16_t conn_id);

/**
 * @brief                 This function obtains the connection ID using the address of a remote device.
 * @param[in] peer_addr   is address of a remote device to connect to.
 * @return                connection ID, if the operation completed successfully, otherwise 0.
 * @sa                    ble_gap_get_connection_info()
 */
uint16_t ble_gap_get_connection_id(const ble_address_t *peer_addr);

/**
 * @brief                  This function obtains the connection information using the connection ID.
 * @param[in] conn_id      is the connection ID.
 * @return                 connection information #ble_gap_connection_info_t, if the operation completed successfully, otherwise NULL.
 */
const ble_gap_connection_info_t *ble_gap_get_connection_info(uint16_t conn_id);

/**
 * @brief                      This function sets the local device name.
 * @param[in] name             is the device name.
 * @param[in] len              is the length of device name with a maximum size of 31 bytes.
 * @return                     #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 */
ble_status_t ble_gap_set_device_name(const uint8_t *name, uint16_t len);

/**
 * @brief                      This function obtains the local device name.
 * @param[out] name            is the device name to be returned.
 * @param[out] len             is the length of device name to be returned.
 * @return                     #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 */
ble_status_t ble_gap_get_device_name(uint8_t *name, uint16_t *len);

/**
 * @brief                      This function sets the local device appearance.
 * @param[in] appearance       is the local appearance.
 * @return                     #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 */
ble_status_t ble_gap_set_appearance(uint16_t appearance);

/**
 * @brief                      This function obtains the local device appearance.
 * @param[out] appearance      is the local appearance to be returned.
 * @return                     #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 */
ble_status_t ble_gap_get_appearance(uint16_t *appearance);

/**
 * @brief                      This function sets the local device peripheral preferred connection parameters.
 * @param[in] conn_params      is the local peripheral preferred connection parameters.
 * @return                     #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 */
ble_status_t ble_gap_set_peripheral_preferred_connection_params(const ble_gap_connection_params_t *conn_params);

/**
 * @brief                      This function obtains the local device peripheral preferred connection parameters.
 * @param[out] conn_params     is the the local peripheral preferred connection parameters to be returned.
 * @return                     #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 */
ble_status_t ble_gap_get_peripheral_preferred_connection_params(ble_gap_connection_params_t *conn_params);

/**
 * @brief                    This function obtains the public address for the device, It's suggested to invoke after the Bluetooth is powered on.
 * @param[out] local_addr    is the device address to be returned.
 * @return                   #BLE_STATUS_SUCCESS, if the operation completed successfully, otherwise an error occurred.
 */
ble_status_t ble_gap_read_local_address(ble_address_t *local_addr);

/**
 * @brief          This function powers on the Bluetooth and initializes profiles.
 * @return         #BLE_STATUS_SUCCESS, if the Bluetooth is already on, otherwise power on the Bluetooth.
 * @note           #BLE_POWER_ON_CNF event is received, if the Bluetooth is successfully powered on.
 */
ble_status_t ble_power_on(void);

/**
 * @brief          This function powers off the Bluetooth and de-initializes profiles.
 * @return         #BLE_STATUS_SUCCESS, if Bluetooth is already off, otherwise power off the Bluetooth.
 * @note           #BLE_POWER_OFF_CNF event is received, if the Bluetooth is successfully powered off.
 */
ble_status_t ble_power_off(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif /* __BLE_GAP_H__ */

