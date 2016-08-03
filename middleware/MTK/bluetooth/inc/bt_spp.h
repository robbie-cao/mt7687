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

#ifndef __BT_SPP_H__
#define __BT_SPP_H__

/**
 * @addtogroup BT
 * @{
 * @addtogroup SPP
 * @{
 * This section describes the SPP APIs.
 * The Serial Port Profile (SPP) can be used to emulate a serial port connection using RFCOMM protocol between two peer devices.
 *
 * Supported features
 * ======
 * - \b SPP \b Server \n
 * SPP server is implemented for a server side connection (Device B, Acceptor) according to the SPP specification. It can wait
 * for another device to take initiative to connect.
 * - \b SPP \b Client \n
 * SPP client is implemented for a client side connection (Device A, Initiator) according to the SPP specification. It can
 * initiate a connection with another device.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b UUID                       | A Universally Unique Identifier (UUID) is a 16-octet (128-bit) number. |
 * |\b SPP                        | Serial Port Profile, a Bluetooth wireless-interface profile. It emulates a serial cable to provide a simple substitute for existing RS-232, including the familiar control signals. |
 * |\b RFCOMM                     | The Radio Frequency Communication (RFCOMM) is a simple set of transport protocols, made on top of the L2CAP protocol, providing emulated RS-232 serial ports. |
 * |\b SDP                        | Service Discovery Protocol. Used to allow devices to discover what services are supported by each other, and what parameters to use to connect to them. |
 *
 * @section bt_spp_api_usage How to use this module
 * - SPP connection in Server mode.
 *  - Step1: Call #bt_spp_start_service() to create a SPP server instance by registering a RFCOMM channel and SDP service record,
 *           then waiting for the SPP client on a remote device to connect.
 *    @code
 *       port = bt_spp_start_service(uuid128, NULL, server_app_callback);
 *    @endcode
 *  - Step2: If a remote SPP client initiates a connection to the SPP server, the #BT_SPP_CONNECT_IND event is generated to trigger a server callback function registered with the #bt_spp_start_service().
 *  - Step3: Call #bt_spp_connect_response() to accept or reject this connection request from the remote SPP client.
 *    @code
 *       server_app_callback(bt_event_t event_id, void* parameter)
 *       {
 *           switch (event_id) {
 *               case BT_SPP_CONNECT_IND:
 *                   bt_spp_connect_response(parameter->port, true, spp_tx_buf, tx_buf_size, spp_rx_buf, rx_buf_size);
 *                   break;
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - Step4: A #BT_SPP_CONNECT_CNF event is generated to trigger the server callback function if the device is connected.
 * - SPP connection in Client mode.
 *  - Step1: Call #bt_spp_connect() to initiate a connection to a remote server.
 *    @code
 *        bt_spp_connect(remote_address, uuid128, spp_tx_buffer, tx_size, spp_rx_buffer, rx_size, client_app_callback);
 *    @endcode
 *  - Step2: A #BT_SPP_CONNECT_CNF event is generated to trigger the client callback function registered with the #bt_spp_connect() if the device is connected.
 * -  Write data.
 *  - Step1: Call #bt_spp_write() to send application data to the remote device.
 *           If returned value is less than the size of application data to write, the application must wait for #BT_SPP_READY_TO_WRITE_IND event for the next write.
 *    @code
 *        int32_t total_sent_size = 0;
 *        uint32_t total_size = 1000;
 *        uint8_t app_data[1000];
 *        do {
 *            uint32_t real_sent_size = 0;
 *            real_sent_size = bt_spp_write(port, app_data + 1000 - total_size, total_size - total_sent_size);
 *            if (real_sent_size > 0) {
 *                total_sent_size += real_sent_size;
 *            } else {
 *                break;
 *            }
 *        } while (total_size - total_sent_size > 0);
 *    @endcode
 *  - Step2: If the application received #BT_SPP_READY_TO_WRITE_IND event, it can continue to write the rest of application data.
 *    @code
 *        do {
 *            uint32_t real_sent_size = 0;
 *            real_sent_size = bt_spp_write(port, app_data + 1000 - total_size, total_size - total_sent_size);
 *            if (real_sent_size > 0) {
 *                total_sent_size += real_sent_size;
 *            } else {
 *                break;
 *            }
 *        } while (total_size - total_sent_size > 0);
 *    @endcode
 * -  Read data.
 *  - Step1: Call #bt_spp_read() to receive application data from the remote device.
 *           If returned value is less than the size of application data to read, the application must wait for #BT_SPP_READY_TO_READ_IND event for the next read.
 *    @code
 *        int32_t total_received_size = 0;
 *        uint8_t app_rx_buffer[1000];
 *        uint32_t total_size = 1000;
 *        do {
 *            int32_t real_received_size = 0;
 *            real_received_size = bt_spp_read(port, app_rx_buffer + total_received_size, total_size - total_received_size);
 *            if (real_received_size > 0) {
 *                total_received_size += real_received_size;
 *            } else {
 *                break;
 *            }
 *        } while (total_size - total_received_size > 0);
 *    @endcode
 *  - Step2: If the application received #BT_SPP_READY_TO_READ_IND event, it can continue to read the rest of received data.
 *    @code
 *        do {
 *            int32_t real_received_size = 0;
 *            real_received_size = bt_spp_read(port, app_rx_buffer + total_received_size, total_size - total_received_size);
 *            if (real_received_size > 0) {
 *                total_received_size += real_received_size;
 *            } else {
 *                break;
 *            }
 *        } while (total_size - total_received_size > 0);
 *    @endcode
 * -  Disconnect the SPP connection.
 *  - Step1: Call #bt_spp_disconnect() to disconnect the SPP connection from a remote device.
 *    @code
 *           ret = bt_spp_disconnect(port);
 *    @endcode
 *  - Step2: A #BT_SPP_DISCONNECT_IND event will notify application callback function to indicate the result of the disconnection.
 *    @code
 *       app_callback(bt_event_t event_id, void* parameter)
 *       {
 *           switch (event_id) {
 *               case BT_SPP_DISCONNECT_IND:
 *                   if (parameter->result == BT_STATUS_SUCCESS) {
 *                       //SPP disconnected
 *                   }
 *                   break;
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 * -  Stop a SPP server in Server mode.
 *  - Step1: Call #bt_spp_stop_service() to delete the SPP server instance by unregistering the RFCOMM channel and SDP service record.
 *           The remote client will no longer be able to connect to the server.
 *    @code
 *           ret = bt_spp_stop_service(port);
 *    @endcode
 */

#include <stdbool.h>
#include <stdint.h>
#include "bt_address.h"
#include "bt_status.h"
#include "bt_events.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup bt_spp_typedef Typedef
 * @{
 */

/**
 * @brief               This is the application callback function type to listen to the SPP events.
 * @param[in] event_id  is the SPP event ID.
 * @param[in] parameter is the parameter data of the event.
 * @return              None
 */
typedef void (*bt_spp_common_callback)(bt_event_t event_id, const void *param);

/**
 * @brief               SPP virtual port number, it is a positive number and identifies a SPP client or server instance.
 *                      Each of them has one unique port ID.
 */
typedef int32_t bt_spp_port_id_t;

/**
 * @}
 */

/**
 * @defgroup bt_spp_define Define
 * @{
 */

/**
 * @brief The minimum TX buffer for SPP data transmission, #BT_SPP_MIN_TX_BUFSIZE should be larger than 2000 bytes.
 */
#define BT_SPP_MIN_TX_BUFSIZE 2000

/**
 * @brief The minimum RX buffer for SPP data transmission, #BT_SPP_MIN_RX_BUFSIZE should be larger than 2000 bytes.
 */
#define BT_SPP_MIN_RX_BUFSIZE 2000

/**
 * @}
 */

/**
 * @defgroup bt_spp_struct Struct
 * @{
 */

/**
 *  @brief This structure defines #BT_SPP_CONNECT_CNF.
 */
typedef struct {
    bt_spp_port_id_t port;      /**< SPP virtual port number. */
    bt_status_t result;         /**< Connection result. */
} bt_spp_connect_cnf_t;

/** @brief This structure defines #BT_SPP_CONNECT_IND. */
typedef struct {
    bt_spp_port_id_t port;      /**< SPP virtual port number. */
    bt_address_t address;       /**< Remote Bluetooth enabled device address. */
} bt_spp_connect_ind_t;

/** @brief This structure defines #BT_SPP_DISCONNECT_IND. */
typedef struct {
    bt_spp_port_id_t port;      /**< SPP virtual port number. */
    bt_status_t result;         /**< Disconnect result. */
} bt_spp_disconnect_ind_t;

/** @brief This structure defines #BT_SPP_READY_TO_READ_IND. */
typedef struct {
    bt_spp_port_id_t port;      /**< SPP virtual port number. */
} bt_spp_ready_to_read_ind_t;

/** @brief This structure defines #BT_SPP_READY_TO_WRITE_IND. */
typedef struct {
    bt_spp_port_id_t port;      /**< SPP virtual port number. */
} bt_spp_ready_to_write_ind_t;

/**
 * @}
 */

/**
 * @brief                       This function creates the SPP server instance, then waits for the SPP client on a remote device to connect.
 * @param[in] uuid128           is a 128-bit UUID to identify the SPP server. User can assign their own unique and private UUID. If it is
 *                              set to NULL, the default UUID {0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,
 *                              0x5F,0x9B,0x34,0xFB} which was assigned to identify the SPP profile in Bluetooth specification will be used.
 * @param[in] service_name      is to identify a SPP server service name, UTF-8. It is an optional parameter and can be set to NULL.
 * @param[in] app_callback      is a notify callback set by a server application to handle events received from SDK layer.
 * @return                      If create service operation was successful, the returned value is the port ID, otherwise the returned value is less than zero.
 * @sa                          #bt_spp_stop_service()
 * @note                        It can only be used by a SPP server. It's suggested to call this function immediately after the Bluetooth is powered on.
 */
int32_t bt_spp_start_service(const uint8_t *uuid128, const int8_t *service_name, bt_spp_common_callback app_callback);

/**
 * @brief                       This function is for the SPP client to connect to a remote server, a #BT_SPP_CONNECT_CNF event will be reported to the
 *                              application to indicate the result of the connection.
 * @param[in] bt_address        is the Bluetooth address of a remote device.
 * @param[in] uuid128           is a 128-bit UUID of remote server. If set to NULL, it will use default UUID {0x00,0x00,0x11,
 *                              0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB} to connect remote server.
 * @param[in] tx_buffer         is the SPP TX buffer, SPP profile will use it to send application's data to remote device.
 * @param[in] tx_size           is the size of TX buffer, it must be equal or greater than #BT_SPP_MIN_TX_BUFSIZE.
 * @param[in] rx_buffer         is the RX buffer size,  SPP profile will use it to receive data from remote device.
 * @param[in] rx_size           is the size of RX buffer, it must be equal or greater than #BT_SPP_MIN_RX_BUFSIZE.
 * @param[in] app_callback      is a notify callback set by a server application to handle events received from SDK layer.
 * @return                      If the operation was successful, the returned value is #BT_STATUS_SUCCESS, otherwise the returned value is less than zero.
 * @sa                          #bt_spp_disconnect()
 * @note                        It can only be used by a SPP client.
 */
bt_status_t bt_spp_connect(
    const bt_address_t *bt_address,
    const uint8_t *uuid128,
    void *tx_buffer, uint32_t tx_size,
    void *rx_buffer, uint32_t rx_size,
    bt_spp_common_callback app_callback
);

/**
 * @brief                   This function is for the SPP server to accept or reject connection from a remote client, a #BT_SPP_CONNECT_CNF event
 *                          will be reported to the application to indicate the result of the response.
 * @param[in] port          is port ID of the SPP server as a response to the established connection.
 * @param[in] response      is accept or reject.
 * @param[in] tx_buffer     is the SPP TX buffer, SPP profile will use it to send application's data to a remote device.
 * @param[in] tx_size       is the size of TX buffer, it must be equal or greater than #BT_SPP_MIN_TX_BUFSIZE.
 * @param[in] rx_buffer     is the RX buffer size,  SPP profile will use it to receive data from remote device.
 * @param[in] rx_size       is the size of RX buffer, it must be equal or greater than #BT_SPP_MIN_RX_BUFSIZE.
 * @return                  If the operation was successful, the returned value is #BT_STATUS_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #bt_spp_disconnect()
 * @note                    It can only be used by the SPP server.
 */
bt_status_t bt_spp_connect_response(
    bt_spp_port_id_t port,
    bool response,
    void *tx_buffer, uint32_t tx_size,
    void *rx_buffer, uint32_t rx_size
);

/**
 * @brief                   This function is for application read data.
 * @param[in] port          is the port ID of the SPP instance.
 * @param[in] app_buffer    is the data buffer that application want to read from the RX buffer.
 * @param[in] size          is the size of data that application want to read.
 * @return                  If the operation was successful, the returned value is the size of data read, otherwise it is an error code.
 * @sa                      #bt_spp_write()
 * @note                    If returned value is less than the size of data that application want to read, then there is no data available
 *                          from a remote device. The application must wait #BT_SPP_READY_TO_READ_IND event to apply the next read.
 */
int32_t bt_spp_read(bt_spp_port_id_t port, void *app_buffer, uint32_t size);

/**
 * @brief                   This function is for application write data.
 * @param[in] port          is the port ID of the SPP instance.
 * @param[in] app_buffer    is the data buffer that application want to write to the TX buffer.
 * @param[in] size          is the size of data that application want to write.
 * @return                  If the operation was successful, the returned value is the size of data to be written, otherwise it is an error code.
 * @sa                      #bt_spp_read()
 * @note                    If returned value is less than the size of data that application want to write, then the SPP TX buffer is full and data
                            cannot be transferred. The application must wait for #BT_SPP_READY_TO_WRITE_IND event to apply the next write.
 */
int32_t bt_spp_write(bt_spp_port_id_t port, void *app_buffer, uint32_t size);

/**
 * @brief                   This function is for application to disconnect a connection; a #BT_SPP_DISCONNECT_IND event will be reported to application
 *                          to indicate the result of the disconnection.
 * @param[in] port          is a SPP instance port id.
 * @return                  If the operation was successful, the returned value is #BT_STATUS_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #bt_spp_connect(), #bt_spp_connect_response()
 */
bt_status_t bt_spp_disconnect(bt_spp_port_id_t port);

/**
 * @brief                   This function is to stop the SPP server service.
 * @param[in] port          is the port ID of the SPP instance.
 * @return                  If the operation was successful, the returned value is #BT_STATUS_SUCCESS, otherwise the returned value is less than zero.
 * @sa                      #bt_spp_start_service()
 * @note                    It can only be used by a SPP server.
 */
bt_status_t bt_spp_stop_service(bt_spp_port_id_t port);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif /*__BT_SPP_H__*/

