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

#include "ble_events.h"
#include "bt_events.h"
#include "syslog.h"
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_sm.h"


#define GATT_MTU_SIZE 158


void ble_gatts_common_callback(ble_event_t event_id, const void *param)
{
    switch (event_id) {
        case BLE_GATT_EXCHANGE_MTU_REQ:
            LOG_I(ANCS, "[ANCS]expected_mtu = %d \r\n", ((ble_gatt_exchange_mtu_req_t *)param)->expected_mtu);
            ble_gatt_confirm_exchange_mtu(((ble_gatt_exchange_mtu_req_t *)param)->conn_id, GATT_MTU_SIZE);
            break;

        default:
            break;
    }
}

void ble_gattc_common_callback(ble_event_t event_id, const void *param)
{

    //LOG_I(ANCS, "[ANCS] event = 0x%x", event_id);

    ble_gattc_client_event_handler(event_id, param);

}

void ble_gap_common_callback(ble_event_t event_id, const void *param)
{

    //LOG_I(ANCS, "[ANCS]event = %d", event_id);

    switch (event_id) {
        case BLE_GAP_ADVERTISING_IND:
            ble_gap_event_handler(event_id, (ble_gap_advertising_ind_t *)param);
            break;

        case BLE_GAP_CONNECTED_IND: {
            ble_gap_connection_params_t conn_params;

            ble_gap_event_handler(event_id, (ble_gap_connected_ind_t *)param);

            //update connection interval
            conn_params.conn_timeout = 0x0258;              /** TBC: 6000ms : 600 * 10 ms. */
            conn_params.min_conn_interval = 0x0010;         /** TBC: 20ms : 16 * 1.25 ms. */
            conn_params.max_conn_interval = 0x0020;         /** TBC: 40ms : 32 * 1.25 ms. */
            conn_params.conn_latency = 0;

            ble_gap_update_connection_params(((ble_gap_connected_ind_t *)param)->conn_id, &conn_params);
        }
        break;

        case BLE_GAP_DISCONNECTED_IND:
            ble_gap_event_handler(event_id, (ble_gap_disconnected_ind_t *)param);
            break;

        default:
            break;
    }
}

void bt_gap_common_callback(bt_event_t event_id, void *param)
{

    switch (event_id) {
        case BT_GAP_POWER_ON_CNF: {
            uint8_t buff[31] = {2, 1, 0x1A};
            char *LE_name = "ancs";
            uint8_t len = 0;
            ble_address_t addr;

            LOG_I(ANCS, "POWER ON, set advertising\n");

            addr.address[0] = 0x0C;
            addr.address[1] = 1;
            addr.address[2] = 3;
            addr.address[3] = 5;
            addr.address[4] = 7;
            addr.address[5] = 0xC3;
            ble_gap_stop_advertiser();
            ble_gap_set_random_address(&addr);
            len = strlen(LE_name);
            buff[3] = len + 1;
            buff[4] = 9;
            memcpy(buff + 5, LE_name, len);
            ble_gap_set_advertising_data(
                buff,
                len + 5,
                NULL,
                0
            );
            ble_gap_set_advertising_params(
                0x100,
                0x100,
                BLE_GAP_ADVERTISING_CONNECTABLE_UNDIRECTED,
                BLE_ADDRESS_TYPE_RANDOM,
                BLE_ADDRESS_TYPE_RANDOM,
                NULL,
                0x7,
                0
            );
            ble_gap_start_advertiser();

            ble_gap_set_device_name(LE_name, len);
            break;
        }
        default:
            break;
    }
}

void ble_sm_common_callback(ble_event_t event_id, const void *param)
{

    ble_ancs_sm_event_handler(event_id, param);
}

