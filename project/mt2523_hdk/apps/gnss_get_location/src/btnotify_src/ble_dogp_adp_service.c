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
 * This file implements dogp adaptor service main function
 *
 ****************************************************************************/

#include <stdio.h>
#include "syslog.h"
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_address.h"
#include "ble_dogp_service.h"
#include "ble_gatts_srv_common.h"
#include "ble_dogp_adp_service.h"
#include <timers.h>
#include <string.h>



/************************************************
*   Macro
*************************************************/
#define BLE_DOGP_ADP_SEND_BUFFER_SIZE           (1024 * 2)
#define BLE_DOGP_ADP_RECEIVE_BUFFER_SIZE        (1024 * 2)

typedef uint8_t ble_dogp_adp_state_t;
#define BLE_DOGP_ADP_STATE_DISCONENCTED         0
#define BLE_DOGP_ADP_STATE_CONENCTED            1
#define BLE_DOGP_ADP_STATE_READY_TO_WRITE       2
#define BLE_DOGP_ADP_STATE_READY_TO_READ        3

static TimerHandle_t xTimer_low_power = NULL;

/************************************************
*   Structures
*************************************************/
typedef struct {
    uint16_t                        conn_id;
    ble_dogp_adp_state_t	    state;
    ble_dogp_adp_common_callback_t  callback;
} ble_dogp_adp_cntx_t;

typedef struct {
    bool                            need_ready2write;
    bool                            first_write;
    bool                            notify_enabled;
    uint16_t                        conn_id;
    uint8_t                         send_buffer[BLE_DOGP_ADP_SEND_BUFFER_SIZE];
    uint32_t                        send_buffer_start;
    uint32_t                        send_buffer_length;
    uint8_t                         receive_buffer[BLE_DOGP_ADP_RECEIVE_BUFFER_SIZE];
    uint32_t                        receive_buffer_start;
    uint32_t                        receive_buffer_length;
} ble_dogp_adp_buffer_t;

typedef struct {
    uint16_t	                    conn_id;
    uint8_t                         remote_device_type;
    ble_gap_conn_param_priority_t   conn_priority;
    ble_gap_adv_interval_enum_t     adv_interval;
} ble_dogp_adp_low_powe_cntx_t;

/************************************************
*   Global variables
*************************************************/
ble_evt_t p_ble_evt_t;
ble_dogp_service_t p_dogps_t;
ble_dogp_adp_low_powe_cntx_t g_dogp_adp_low_power_cntx = {0};
ble_dogp_adp_cntx_t g_dogp_adp_cntx = {0};
ble_dogp_adp_buffer_t g_sogp_adp_buffer = {0};


/************************************************
*   static utilities
*************************************************/
int32_t ble_dogp_adp_evt_handler(ble_dogp_service_t *p_dogps, ble_dogp_service_evt_t *p_evt);
static void ble_dogp_adp_evt_dispatch(ble_evt_t *p_ble_evt);
static void ble_dogp_adp_on_ble_evt(ble_evt_t *p_ble_evt);
static void ble_dogp_adp_connection_param_updated_callback(ble_gap_connection_param_updated_ind_t *conn_updated);
static int32_t ble_dogp_adp_services_init(void);
static int32_t ble_dogp_adp_update_connection_interval(uint16_t conn_id);
static int32_t ble_dogp_adp_set_advertising_interval(uint16_t conn_id);
static ble_status_t ble_dogp_adp_advertise_data_init(void);

log_create_module(DOGP_ADP, PRINT_LEVEL_INFO);

/************************************************
*   Functions
*************************************************/
/**
 * @brief Function for application to set remote device's type, android or ios device.
 */
void ble_dogp_adp_set_remote_device_type(uint16_t conn_id, ble_dogp_adp_remote_device_type_t type)
{
    if ((conn_id != 0) && (conn_id == g_dogp_adp_low_power_cntx.conn_id)) {
        g_dogp_adp_low_power_cntx.remote_device_type = (uint8_t)type;
    }
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_set_remote_device_type: conn_id is [%d], master_type is [%d]\r\n", conn_id, type);
}

/**
 * @brief Function for application to write data to the send buffer.
 */
uint32_t ble_dogp_adp_write_data(uint16_t conn_id, uint8_t *buffer, uint32_t size)
{
    uint32_t send_size = 0;
    uint8_t notify_string[15] = {"ios indication"};
    ble_dogp_adp_buffer_t *buffer_t = &g_sogp_adp_buffer;
    ble_dogp_service_dogp_service_read_characteristic_t p_dogps_notify_to_read;

    if ((conn_id != 0) && (conn_id == buffer_t->conn_id)) {

        if (size > (BLE_DOGP_ADP_SEND_BUFFER_SIZE - buffer_t->send_buffer_length)) {
            send_size = BLE_DOGP_ADP_SEND_BUFFER_SIZE - buffer_t->send_buffer_length;
            buffer_t->need_ready2write = true;
            LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_write_data: write buffer is full\r\n");
        } else {
            send_size = size;
        }
        memcpy(&(buffer_t->send_buffer[buffer_t->send_buffer_length]), buffer, send_size);
        buffer_t->send_buffer_length += size;

        p_dogps_notify_to_read.read__field.p_str = notify_string;
        p_dogps_notify_to_read.read__field.length = sizeof(notify_string);
        ble_dogp_service_dogp_service_read_characteristic_send(&p_dogps_t, &p_dogps_notify_to_read);
        LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_write_data: send_size is [%d]\r\n", send_size);

        return send_size;
    }
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_write_data: conn id error [%d]\r\n", conn_id);
    return 0;
}

/**
 * @brief Function for application to read data from the receive buffer.
 */
uint32_t ble_dogp_adp_read_data(uint16_t conn_id, uint8_t *buffer, uint32_t size)
{
    uint32_t read_size = 0;
    ble_dogp_adp_buffer_t *buffer_t = &g_sogp_adp_buffer;

    if ((conn_id != 0) && (conn_id == buffer_t->conn_id)) {
        if (buffer_t->receive_buffer_length > size) {
            read_size = size;
        } else {
            read_size = buffer_t->receive_buffer_length;
        }

        if (0 == read_size) {
            LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_read_data: read buffer is null\r\n");
            return 0;
        }
        memcpy(buffer, buffer_t->receive_buffer, read_size);
        if (buffer_t->receive_buffer_length > read_size) {
            memmove(buffer_t->receive_buffer, &(buffer_t->receive_buffer[read_size]), (buffer_t->receive_buffer_length - read_size));
            buffer_t->receive_buffer_length -= read_size;
        } else {
            buffer_t->receive_buffer_length = 0;
            memset(buffer_t->receive_buffer, 0, sizeof(buffer_t->receive_buffer));
        }

        LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_read_data: read_size is [%d]\r\n", read_size);
        return read_size;
    }
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_read_data: conn id error [%d]\r\n", conn_id);
    return 0;
}

static void ble_dogp_adp_connection_param_updated_callback(ble_gap_connection_param_updated_ind_t *conn_updated)
{
    ble_gap_conn_param_priority_t conn_param;

    if ((conn_updated->conn_id != 0) && (conn_updated->conn_id == g_dogp_adp_low_power_cntx.conn_id)) {
        conn_param = ble_gap_get_current_connection_interval(conn_updated);
        if (conn_param == g_dogp_adp_low_power_cntx.conn_priority) {
            LOG_I(DOGP_ADP, "[BLE_DOGP] connection param update successfully \r\n");
        }
    }
}

static int32_t ble_dogp_adp_set_advertising_interval(uint16_t conn_id)
{
    int32_t status;
    uint8_t name[8];
    ble_gap_adv_interval_enum_t interval = BLE_GAP_ADV_INTERVAL_LOW_POWER;

    memset(name, 0, sizeof(name));
    memcpy(name, "dogp", sizeof("dogp"));
    g_dogp_adp_low_power_cntx.conn_id = conn_id;
    status = ble_gap_set_advertising_interval(conn_id, name, interval);
    if (BLE_STATUS_SUCCESS == status) {
        g_dogp_adp_low_power_cntx.adv_interval = BLE_GAP_ADV_INTERVAL_LOW_POWER;
    }
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_set_advertising_interval, status = %d \r\n", status);
    return status;
}

static int32_t ble_dogp_adp_update_connection_interval(uint16_t conn_id)
{
    int32_t status = BLE_STATUS_GAP_FAILED;
    uint8_t name[8];
    ble_gap_conn_param_priority_t interval = BLE_GAP_CONN_PARAM_PRIORITY_DEFAULT;

    if ((conn_id != 0) && (conn_id == g_dogp_adp_low_power_cntx.conn_id)) {
        if (BLE_DOGP_ADP_REMOTE_DEVICE_ANDROID == g_dogp_adp_low_power_cntx.remote_device_type) {
            interval = BLE_GAP_CONN_PARAM_HIGH_SPEED_ANDROID;
        } else if (BLE_DOGP_ADP_REMOTE_DEVICE_IOS == g_dogp_adp_low_power_cntx.remote_device_type) {
            interval = BLE_GAP_CONN_PARAM_HIGH_SPEED_IOS;
        }
        memset(name, 0, sizeof(name));
        memcpy(name, "dogp", sizeof("dogp"));
        status = ble_gap_update_connection_interval(conn_id, name, interval);
        if (status == BLE_STATUS_GAP_PENDING) {
            g_dogp_adp_low_power_cntx.conn_priority = interval;
        }
        LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_update_connection_interval, status = %d, interval = %d \r\n", status, interval);
    }
    LOG_I(DOGP_ADP,  "[BLE_DOGP]update,xTimer_low_power:%d\r\n", xTimer_low_power);
    if (xTimer_low_power != NULL) {
        if (xTimerIsTimerActive(xTimer_low_power)) {
            xTimerStop(xTimer_low_power, 0);
        }

        if (xTimerStart(xTimer_low_power, 0) != pdPASS) {
            LOG_I(DOGP_ADP,  "[BLE_DOGP] start LOW_POWER_TIMER fail\r\n");

        }

    } else {
        LOG_I(DOGP_ADP,  "[BLE_DOGP] create LOW_POWER_TIMER fail\r\n");

    }
    return status;

}

/**
 * @brief Function for handling the DOGP Service events.
 *
 *  @details     This function will be called for all DOGP Service events which are passed to
 *			    the application.
 *
 *  @param[in]	p_ias	DOGP Service structure.
 *  @param[in] 	p_evt	Event received from the DOGP Service.
 */
int32_t ble_dogp_adp_evt_handler(ble_dogp_service_t *p_dogps, ble_dogp_service_evt_t *p_evt)
{
    int32_t err_code = 0;
    ble_dogp_adp_buffer_t *buffer_t = &g_sogp_adp_buffer;
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_evt_handler: event type is [%d]\r\n", p_evt->evt_type);

    switch (p_evt->evt_type) {
        case BLE_DOGP_SERVICE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_WRITE: {
            if ((p_dogps->conn_id != 0) && (p_dogps->conn_id == buffer_t->conn_id)) {
                buffer_t->notify_enabled = p_evt->params.dogp_service_read_characteristic_cccd_value;
            }
        }
        break;
        case BLE_DOGP_SERVICE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_READ: {
            if ((p_dogps->conn_id != 0) && (p_dogps->conn_id == buffer_t->conn_id)) {
                ble_dogp_service_dogp_service_read_characteristic_cccd_read_rsp(p_dogps, buffer_t->notify_enabled);
            }
        }
        break;
        case BLE_DOGP_SERVICE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_READ: {
            /**send data to remote really*/
            if ((p_dogps->conn_id != 0) && (p_dogps->conn_id == buffer_t->conn_id)) {
                uint32_t read_size = 0;
                uint8_t *read_rsp_value = NULL;
                uint16_t mtu_size;
                ble_dogp_service_dogp_service_read_characteristic_t dogp_read_rsp_t;

                ble_gatt_get_mtu_size(p_dogps->conn_id, &mtu_size);
                if ((mtu_size - 1 - p_evt->params.dogp_service_read_characteristic_read_offset) <
                        buffer_t->send_buffer_length) {
                    read_size = mtu_size - 1 - p_evt->params.dogp_service_read_characteristic_read_offset;
                } else {
                    read_size = buffer_t->send_buffer_length;
                }

                LOG_I(DOGP_ADP, "[BLE_DOGP] read characteristic: read_size is %d\r\n", read_size);
                if (0 == read_size) {
                    LOG_I(DOGP_ADP, "[BLE_DOGP] read characteristic: read_size is 0\r\n");
                    dogp_read_rsp_t.read__field.p_str = read_rsp_value;
                    dogp_read_rsp_t.read__field.length = read_size;
                    ble_dogp_service_dogp_service_read_characteristic_read_rsp(p_dogps, &dogp_read_rsp_t,
                            p_evt->params.dogp_service_read_characteristic_read_offset);
                    return 0;
                }
                read_rsp_value = ble_gatts_alloc_memory(read_size);
                memset(read_rsp_value, 0, read_size);
                ble_dogp_adp_update_connection_interval(p_dogps->conn_id);
                memcpy(read_rsp_value, buffer_t->send_buffer, read_size);
                dogp_read_rsp_t.read__field.p_str = read_rsp_value;
                dogp_read_rsp_t.read__field.length = read_size;
                ble_dogp_service_dogp_service_read_characteristic_read_rsp(p_dogps, &dogp_read_rsp_t,
                        p_evt->params.dogp_service_read_characteristic_read_offset);
                ble_gatts_free_memory(read_rsp_value);
                if (buffer_t->send_buffer_length > read_size) {
                    memmove(buffer_t->send_buffer, &(buffer_t->send_buffer[read_size]), (buffer_t->send_buffer_length - read_size));
                    buffer_t->send_buffer_length -= read_size;
                    LOG_I(DOGP_ADP, "[BLE_DOGP] read characteristic: send notification length is [%d]\r\n",
                          read_size);
                } else {
                    buffer_t->send_buffer_length = 0;
                    memset(buffer_t->send_buffer, 0, sizeof(buffer_t->send_buffer));
                }
                LOG_I(DOGP_ADP, "[BLE_DOGP] read characteristic: read_size[%d], send_buffer_length[%d]\r\n",
                      read_size, buffer_t->send_buffer_length);
                if (buffer_t->need_ready2write) {
                    ble_dogp_adp_ready_to_write_t ready_to_write;
                    const ble_gap_connection_info_t *conn_info = NULL;

                    memset(&ready_to_write, 0, sizeof(ble_dogp_adp_ready_to_write_t));
                    conn_info = ble_gap_get_connection_info(p_dogps->conn_id);

                    ready_to_write.conn_id = p_dogps->conn_id;
                    memcpy(&ready_to_write.bdaddr, &conn_info->peer_addr, 6);

                    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_evt_handler: ready to write send\r\n");
                    g_dogp_adp_cntx.callback(BLE_DOGP_ADP_EVENT_READY_TO_WRITE_IND, (void *)&ready_to_write);
                    g_dogp_adp_cntx.state = BLE_DOGP_ADP_STATE_READY_TO_WRITE;
                    buffer_t->need_ready2write = false;

                }

                return read_size;
            }
            err_code = -2;/**invalid connection id*/
        }
        break;
        case BLE_DOGP_SERVICE_DOGP_SERVICE_WRITE_CHARACTERISTIC_EVT_WRITE: {
            /**remote write & notify app ready to read*/
            if ((p_dogps->conn_id != 0) && (p_dogps->conn_id == buffer_t->conn_id)) {
                /**ble_dogp_adp_buffer_t *buffer_t = &g_sogp_adp_buffer;*/
                if (p_evt->params.dogp_service_write_characteristic.write_field.length >
                        (BLE_DOGP_ADP_RECEIVE_BUFFER_SIZE - buffer_t->receive_buffer_length)) {
                    LOG_I(DOGP_ADP, "[BLE_DOGP] write characteristic: buffer full error\r\n");
                    ble_gatts_write_response(p_dogps->conn_id, BLE_STATUS_GATT_FAILED);
                    return -1; /**means fail, buffer full*/
                }

                ble_dogp_adp_update_connection_interval(p_dogps->conn_id);
                memcpy(&(buffer_t->receive_buffer[buffer_t->receive_buffer_length]),
                       p_evt->params.dogp_service_write_characteristic.write_field.p_str,
                       p_evt->params.dogp_service_write_characteristic.write_field.length);
                buffer_t->receive_buffer_length += p_evt->params.dogp_service_write_characteristic.write_field.length;

                if (g_dogp_adp_cntx.callback) {
                    ble_dogp_adp_ready_to_read_t ready_to_read;
                    const ble_gap_connection_info_t *conn_info = NULL;

                    memset(&ready_to_read, 0, sizeof(ble_dogp_adp_ready_to_read_t));
                    conn_info = ble_gap_get_connection_info(p_dogps->conn_id);

                    ready_to_read.conn_id = p_dogps->conn_id;
                    memcpy(&ready_to_read.bdaddr, &conn_info->peer_addr, 6);

                    if (false == buffer_t->first_write) {
                        ble_dogp_adp_connect_t connect_param;
                        memset(&connect_param, 0, sizeof(ble_dogp_adp_connect_t));

                        buffer_t->first_write = true;
                        buffer_t->conn_id = p_dogps->conn_id;
                        connect_param.conn_id = p_dogps->conn_id;
                        connect_param.conn_result = 0;
                        memcpy(&connect_param.bdaddr, &conn_info->peer_addr, 6);

                        LOG_I(DOGP_ADP, "[BLE_DOGP] write characteristic: connect and first write\r\n");
                        g_dogp_adp_cntx.callback(BLE_DOGP_ADP_EVENT_CONNECT_IND, (void *)&connect_param);
                        g_dogp_adp_cntx.state = BLE_DOGP_ADP_STATE_CONENCTED;
                    }

                    LOG_I(DOGP_ADP, "[BLE_DOGP] write characteristic: write size[%d] \r\n", p_evt->params.dogp_service_write_characteristic.write_field.length);
                    g_dogp_adp_cntx.callback(BLE_DOGP_ADP_EVENT_READY_TO_READ_IND, (void *)&ready_to_read);
                    g_dogp_adp_cntx.state = BLE_DOGP_ADP_STATE_READY_TO_READ;

                    ble_gatts_write_response(p_dogps->conn_id, BLE_STATUS_GATT_SUCCESS);
                }
                return 0;/**success*/
            }
            err_code = -2;/**invalid connect id*/
        }

        break;
        case BLE_DOGP_SERVICE_DOGP_SERVICE_WRITE_CHARACTERISTIC_EVT_EXCUTE_WRITE: {
            /** No implementation needed.*/
        }
        break;

        default :
            break;
    }
    return err_code;
}

/**
 * @brief Function for handling the Application's BLE Stack events.
 *
 *  @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_dogp_adp_on_ble_evt(ble_evt_t *p_ble_evt)
{
    ble_dogp_adp_buffer_t *buffer_t = &g_sogp_adp_buffer;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_CONNECTED_IND: {
            uint16_t conn_id = p_ble_evt->evt.gap_evt.connected.conn_id;
            ble_dogp_adp_set_advertising_interval(conn_id);
            buffer_t->conn_id = conn_id;
            g_dogp_adp_low_power_cntx.remote_device_type = BLE_DOGP_ADP_REMOTE_DEVICE_ANDROID;
            LOG_I(DOGP_ADP, "[BLE_DOGP] BLE_GAP_CONNECTED_IND, conn_id = %d \r\n", conn_id);
        }
        break;

        case BLE_GAP_DISCONNECTED_IND: {
            uint16_t conn_id;
            conn_id = p_ble_evt->evt.gap_evt.disconnected.conn_id;
            if (g_dogp_adp_cntx.callback) {
                ble_dogp_adp_disconnect_t disconnect_param;
                memset(&disconnect_param, 0, sizeof(ble_dogp_adp_disconnect_t));

                /**conn_info = ble_gap_get_connection_information(conn_id);*/
                disconnect_param.conn_id = conn_id;
                disconnect_param.disconn_result = 0;
                /**memcpy(disconnect_param.bdaddr, conn_info->peer_addr.address, 6);*/
                g_dogp_adp_cntx.callback(BLE_DOGP_ADP_EVENT_DISCONNECT_IND, (void *)&disconnect_param);
                g_dogp_adp_cntx.state = BLE_DOGP_ADP_STATE_DISCONENCTED;

                LOG_I(DOGP_ADP, "[BLE_DOGP] BLE_GAP_DISCONNECTED_IND, conn_id = %d \r\n", conn_id);
            }
            memset(buffer_t, 0, sizeof(ble_dogp_adp_buffer_t));
            memset(&g_dogp_adp_low_power_cntx, 0, sizeof(ble_dogp_adp_low_powe_cntx_t));
        }
        break;

        case BLE_GAP_CONNECTION_PARAM_UPDATED_IND:
            ble_dogp_adp_connection_param_updated_callback(&(p_ble_evt_t.evt.gap_evt.conn_params));
            break;

        default:
            /** No implementation needed.*/
            break;
    }
}


/**
 * @brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 *  @details     This function is called from the BLE Stack event interrupt handler after a BLE stack
 *               event has been received.
 *
 *  @param[in]   p_ble_evt  Bluetooth stack event.
 */
static void ble_dogp_adp_evt_dispatch(ble_evt_t *p_ble_evt)
{
    /**add your app's gatts event handler as below*/
    ble_dogp_service_on_ble_evt(&p_dogps_t, p_ble_evt);
    ble_dogp_adp_on_ble_evt(p_ble_evt);
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_evt_dispatch done \r\n");
}


/**
 * @brief Function for initializing services that will be used by the application.
 */
static ble_status_t ble_dogp_adp_advertise_data_init(void)
{
    ble_status_t status;
    bt_gap_set_adv_data_req_struct_t req;
    uint8_t ble_dogps_uuid[2] = {0x18, 0xA0};

    memset(&req, 0, sizeof(bt_gap_set_adv_data_req_struct_t));
    req.max_interval = 0xC0;/**high speed*/
    req.min_interval = 0xC0;/**high speed*/
    req.service_uuid_len = sizeof(ble_dogps_uuid);
    memcpy(req.service_uuid, ble_dogps_uuid, sizeof(ble_dogps_uuid));

    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_advertise_data_init \r\n");

    status = ble_gap_set_advertising_data_reqest(&req);
    return status;
}

/**
 * @brief Function for initializing services that will be used by the application.
 */
static int32_t ble_dogp_adp_services_init(void)
{
    int32_t err_code;
    ble_dogp_service_init_t dogps_init_t;

    /** Initialize DOGP Service.*/
    memset(&dogps_init_t, 0, sizeof(ble_dogp_service_init_t));
    memset(&p_dogps_t, 0, sizeof(ble_dogp_service_t));
    dogps_init_t.evt_handler = ble_dogp_adp_evt_handler;
    err_code = (int32_t)ble_dogp_service_init(&p_dogps_t, &dogps_init_t);
    if (0 != err_code) {
        LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_services_init dogp init: err_code[%d] \r\n", err_code);
    }

    return err_code;
}

static void ble_dogp_adp_update_connection_interval_timerout(TimerHandle_t xTimer)
{
    int32_t status = BLE_STATUS_GAP_FAILED;
    uint8_t name[8];
    ble_gap_conn_param_priority_t interval = BLE_GAP_CONN_PARAM_PRIORITY_DEFAULT;

    if (xTimerIsTimerActive(xTimer_low_power)) {
        xTimerStop(xTimer_low_power, 0);
    }

    interval = BLE_GAP_CONN_PARAM_LOW_POWER;
    memset(name, 0, sizeof(name));
    memcpy(name, "dogp", sizeof("dogp"));
    status = ble_gap_update_connection_interval(g_dogp_adp_low_power_cntx.conn_id, name, interval);
    if (status == BLE_STATUS_GAP_PENDING) {
        g_dogp_adp_low_power_cntx.conn_priority = interval;
    }
    LOG_I(DOGP_ADP, "[BLE_DOGP] update_connection_interval_timerout, status = %d, interval = %d \r\n", status, interval);

}

/**
 * @brief Function for application main entry.
 */
int32_t ble_dogp_adp_init(ble_dogp_adp_common_callback_t app_callback)
{
    int32_t status = 0;

    xTimer_low_power = xTimerCreate("LOW_POWER_TIMER",
                                    30 * 1000,
                                    pdTRUE,
                                    NULL,
                                    ble_dogp_adp_update_connection_interval_timerout);
    LOG_I(DOGP_ADP,  "[BLE_DOGP]xTimer_low_power:%d\r\n", xTimer_low_power);

    if (xTimer_low_power == NULL) {
        LOG_I(DOGP_ADP,  "[BLE_DOGP]init create LOW_POWER_TIMER fail\r\n");

    }

    if (app_callback) {
        /**Initialize.*/
        memset(&g_dogp_adp_cntx, 0, sizeof(ble_dogp_adp_cntx_t));
        memset(&g_sogp_adp_buffer, 0, sizeof(ble_dogp_adp_buffer_t));
        memset(&g_dogp_adp_low_power_cntx, 0, sizeof(ble_dogp_adp_low_powe_cntx_t));
        g_dogp_adp_cntx.callback = app_callback;
        /**register services*/
        ble_dogp_adp_services_init();
        ble_dogp_adp_advertise_data_init();
        ble_gap_start_advertiser();
        LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_init \r\n");

        return status;
    }
    return -1;/**means failed*/
}
/**
 *  @brief Function for distinguishing a BLE stack event to all modules with a BLE stack Gatt event handler.
 *
 *  @details   This function is called from the BLE Stack Gatt event interrupt handler
 *
 *  @param[in] event  Bluetooth stack event.
 */
void ble_gatts_common_callback(ble_event_t event_id, const void *param)
{
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_gatts_common_callback: event[%d] \r\n", event_id);
    switch (event_id) {
        case BLE_GATT_EXCHANGE_MTU_REQ: {
            memcpy(&(p_ble_evt_t.evt.gatts_evt.mtu_change), param, sizeof(ble_gatt_exchange_mtu_req_t));
            ble_gatt_confirm_exchange_mtu(p_ble_evt_t.evt.gatts_evt.mtu_change.conn_id, BLE_GATT_MTU_SIZE_CUSTOM);
        }
        break;

        case BLE_GATTS_READ_REQ: {
            memcpy(&(p_ble_evt_t.evt.gatts_evt.read), param, sizeof(ble_gatts_read_req_t));
        }
        break;

        case BLE_GATTS_WRITE_REQ: {
            memcpy(&(p_ble_evt_t.evt.gatts_evt.write), param, sizeof(ble_gatts_write_req_t));
        }
        break;

        case BLE_GATTS_EXECUTE_WRITE_REQ: {
            memcpy(&(p_ble_evt_t.evt.gatts_evt.excute_write), param, sizeof(ble_gatts_execute_write_req_t));
        }
        break;

        case BLE_GATTS_HANDLE_VALUE_CNF: {
            memcpy(&(p_ble_evt_t.evt.gatts_evt.confirm), param, sizeof(ble_gatts_handle_value_cnf_t));
        }
        break;

        default:
            break;
    }
    p_ble_evt_t.header.evt_id = event_id;
    ble_dogp_adp_evt_dispatch(&p_ble_evt_t);
}

/**
 *  @brief Function for distinguishing a BLE stack event to all modules with a BLE stack Gap event handler.
 *
 *  @details This function is called from the BLE Stack Gap event interrupt handler
 *
 *  @param[in] event  Bluetooth stack event.
 */
void ble_gap_common_callback(ble_event_t event_id, const void *param)
{
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_gap_common_callback: event[%d] \r\n", event_id);
    switch (event_id) {
        case BLE_GAP_CONNECTED_IND: {
            memcpy(&(p_ble_evt_t.evt.gap_evt.connected), param, sizeof(ble_gap_connected_ind_t));
        }
        break;
        case BLE_GAP_DISCONNECTED_IND: {
            memcpy(&(p_ble_evt_t.evt.gap_evt.disconnected), param, sizeof(ble_gap_disconnected_ind_t));
        }
        break;
        case BLE_GAP_REPORT_RSSI_IND: {
            memcpy(&(p_ble_evt_t.evt.gap_evt.tx_power), param, sizeof(ble_gap_report_rssi_ind_t));
        }
        break;
        case BLE_GAP_ADVERTISING_IND: {
            memcpy(&(p_ble_evt_t.evt.gap_evt.advertising), param, sizeof(ble_gap_advertising_ind_t));
        }
        break;
        case BLE_GAP_CONNECTION_PARAM_UPDATED_IND: {
            memcpy(&(p_ble_evt_t.evt.gap_evt.conn_params), param, sizeof(ble_gap_connection_param_updated_ind_t));
        }
        break;
        default:
            break;
    }
    p_ble_evt_t.header.evt_id = event_id;
    ble_dogp_adp_evt_dispatch(&p_ble_evt_t);
}

#if 0

/** for Ut */
void ble_dogp_adp_common_callback(ble_dogp_adp_event_t event, void *callback_param)
{

}

void ble_dogp_ut_main(void)
{
    ble_dogp_adp_init(ble_dogp_adp_common_callback);

}
#endif



