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

#include "ble_gattc_connect.h"
#include "ble_gattc_srv.h"

#include "gattc_utils.h"
#include <stdlib.h>
#include "ble_gattc_utils_db.h"
#include "ble_gap.h"
#include "bt_gap.h"
#include "string.h"
#include "ctype.h"

#ifdef __BLE_GATTC_SRV__

static void gattc_scan_cb(ble_gap_advertising_ind_t *param);

static void gattc_connect_cb(ble_gap_connected_ind_t *param);

static void gattc_alloc_conn(ble_gap_connection_info_t *conn_info);

static void gattc_disconnect_cb(ble_gap_disconnected_ind_t *param);

static bool gattc_conn_is_exist(ble_gap_connection_info_t *conn_info);

//static void gattc_set_adv();

//static void ble_gap_event_handler(ble_event_t event_id, const void *param);

#if defined(__ICCARM__) || defined(__CC_ARM)
char *strupr(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = toupper(*str);
    return orign;
}
#endif

/*void ble_gap_common_callback(ble_event_t event_id, const void *param)
{
    hear_rate_message_struct msg;
    switch (event_id) {
        case BLE_GAP_ADVERTISING_IND: {
            memcpy((void*)msg.param, (void*)param, sizeof(ble_gap_advertising_ind_t));
            break;
        }
        case BLE_GAP_CONNECTED_IND: {
            memcpy((void*)msg.param, (void*)param, sizeof(ble_gap_connected_ind_t));
            break;
        }
        case BLE_GAP_DISCONNECTED_IND: {
            memcpy((void*)msg.param, (void*)param, sizeof(ble_gap_disconnected_ind_t));
            break;
        }
    }
    msg.event_id = event_id;

    xQueueSend(app_queue, (void*)&msg, 0);
}*/


void ble_gap_event_handler(ble_event_t event_id, const void *param)
{
    switch (event_id) {
        case BLE_GAP_ADVERTISING_IND: {
            gattc_scan_cb((ble_gap_advertising_ind_t *)param);
            break;
        }
        case BLE_GAP_CONNECTED_IND: {
            gattc_connect_cb((ble_gap_connected_ind_t *)param);
            break;
        }
        case BLE_GAP_DISCONNECTED_IND: {
            gattc_disconnect_cb((ble_gap_disconnected_ind_t *)param);
            break;
        }
    }


}


void gattc_service_init()
{
    /*initition*/
    uint32_t i, count;
    LOG_I(gattc_srv, "[GATTC]gattc_service_init: start");

    memset(&g_gattc_ctx, 0, sizeof (gattc_context_t));
    Init_List_Head(&GATTC(connlist));
    Init_List_Head(&GATTC(unusedconnlist));

    for (i = 0; i < sizeof(GATTC(conntext)) / sizeof(GATTC(conntext)[0]); i++) {
        Insert_Head_List(&GATTC(unusedconnlist), &GATTC(conntext)[i].node);
    }

    Init_List_Head(&GATTC(userlist));
    Init_List_Head(&GATTC(freeuserlist));
    count = sizeof(GATTC(userContext)) / sizeof(GATTC(userContext)[0]);

    for (i = 0 ; i < count; i++) {
        Insert_Head_List(&GATTC(freeuserlist), &GATTC(userContext)[i].node);
    }
    //heart_rate_init();
}


/**
*need listern when bluetooth power on , it should do scan
*/
void gattc_start_scan()
{
    ble_status_t status = BLE_STATUS_GATT_SUCCESS;
    ble_gap_scan_params_t scan_params;

    LOG_I(gattc_srv, "[GATTC]gattc_start_scan: -- start");
    scan_params.scan_type = BLE_GAP_ACTIVE_SCAN;
    scan_params.own_address_type = BLE_ADDRESS_TYPE_RANDOM;
    scan_params.interval = 0x1000;
    scan_params.window = 0x800;
    scan_params.scanning_filter_policy = BLE_GAP_SCAN_FILTER_ALLOW_ALL;
    ble_gap_set_scan_params(&scan_params);

    status = ble_gap_start_scanner();

    LOG_I(gattc_srv, "[GATTC]gattc_start_scan: -- end: status = %d", status);
}


static void gattc_scan_cb(ble_gap_advertising_ind_t *param)
{
    gattc_notify_scan_info_to_all_user(param);

}


ble_status_t gattc_connect(ble_gap_connection_params_t *conn_params, ble_address_t *addr)
{
    ble_status_t status = BLE_STATUS_SUCCESS;
    LOG_I(gattc_srv, "[GATTC]gattc_connect: -- end: start");
    /* ble_gap_add_white_list(BLE_ADDRESS_TYPE_RANDOM, &addr); if need add white  list , can use this api*/
    status = ble_gap_connect(
                 BLE_GAP_NOT_USE_WHITE_LIST,
                 BLE_ADDRESS_TYPE_RANDOM,
                 addr,
                 BLE_ADDRESS_TYPE_RANDOM,
                 conn_params
             );
    LOG_I(gattc_srv, "[GATTC]gattc_connect: -- end: status = %d", status);
    return status;
}


static void gattc_connect_cb(ble_gap_connected_ind_t *param)
{
    gattc_conn_t *gattc_conn = NULL;
    bool is_exist = false;
    const ble_gap_connection_info_t *conn_info = NULL;
    ble_gap_connection_info_t connection_info;

    conn_info = ble_gap_get_connection_info(param->conn_id);

    if (conn_info) {
    memcpy(&connection_info, conn_info, sizeof(ble_gap_connection_info_t));
    }

    LOG_I(gattc_srv, "[GATTC]gattc_connect_cb: -- start: con_id = %d, err_code = %d", param->conn_id, param->error_code);

    if (conn_info) {

        is_exist = gattc_conn_is_exist (&connection_info);

        LOG_I(gattc_srv, "[GATTC]gattc_connect_cb: -- start: is_exist", is_exist);

        if (is_exist == false) {
            gattc_alloc_conn(&connection_info);
        }
    }

    gattc_conn = ble_gattc_link_info_by_id(param->conn_id);

    if (gattc_conn) {

        LOG_I(gattc_srv, "[GATTC]gattc_connect_cb: -- conn link is valid");

        if (param->error_code == BLE_STATUS_SUCCESS) {
            /*need remove to  up layer*/
            gattc_conn->state = GATTC_CONNECTED;
            gattc_notify_conn_info_to_all_user(gattc_conn, param->error_code, 1);
        } else {
            gattc_free_conn(gattc_conn);
        }
    }
    LOG_I(gattc_srv, "[GATTC]gattc_connect_cb: -- end");
}


void  gattc_disconnect()
{
    gattc_conn_t *connect;
    LOG_I(gattc_srv, "[GATTC]gattc_disconnect: -- start");
    connect = (gattc_conn_t *)Get_Head_List(&GATTC(connlist));
    if (!Is_List_Empty(&GATTC(connlist))) {
        while ((ListNode *)connect != &GATTC(connlist)) {
            if (connect) {
                LOG_I(gattc_srv, "[GATTC]gattc_disconnect: -- con_id", connect->conn_id);
                ble_gap_disconnect(connect->conn_id);
                connect = (gattc_conn_t *)Get_Next_Node(&connect->node);
            }
        }
    }
}


static void gattc_disconnect_cb(ble_gap_disconnected_ind_t *param)
{
    gattc_conn_t *gattc_conn;

    LOG_I(gattc_srv, "[GATTC]gattc_disconnect_cb: -- start: con_id = %d, err_code = %d", param->conn_id, param->error_code);
    gattc_conn = ble_gattc_link_info_by_id(param->conn_id);

    if (gattc_conn) {

        LOG_I(gattc_srv, "[GATTC]gattc_disconnect_cb: --gattc_conn = %x ", gattc_conn);
        gattc_notify_conn_info_to_all_user(gattc_conn, param->error_code, 0);
        gattc_free_conn(gattc_conn);
    }
    LOG_I(gattc_srv, "[GATTC]gattc_disconnect_cb: -- end");
}


/*when listern connected evt , it will call  this API*/
static void gattc_alloc_conn(ble_gap_connection_info_t *conn_info)
{
    gattc_conn_t *connect;
    if (!Is_List_Empty(&GATTC(unusedconnlist))) {

        connect = (gattc_conn_t *)Remove_Head_List(&GATTC(unusedconnlist));
        memset(connect, 0, sizeof(gattc_conn_t));
        memcpy(connect->addr, conn_info->peer_addr.address, 6);
        connect->conn_id = conn_info->conn_id;
        gattc_init(&connect->conn_func);
        Insert_Head_List(&GATTC(connlist), &connect->node);
    }
}


static bool gattc_conn_is_exist(ble_gap_connection_info_t *conn_info)
{
    bool is_exist = false;
    gattc_conn_t *conn;
    LOG_I(gattc_srv, "[GATTC]gattc_conn_is_exist: -- start");

    if (!Is_List_Empty(&GATTC(connlist))) {
        conn = (gattc_conn_t *)Get_Head_List(&GATTC(connlist));
        while ((ListNode *)conn != &GATTC(connlist)) {
            if (conn) {
                LOG_I(gattc_srv, "[GATTC]gattc_conn_is_exist:con_id = %d, info_conid = %d", conn->conn_id, conn_info->conn_id);
                if (conn->conn_id == conn_info->conn_id) {
                    is_exist = true;
                    break;
                }
                conn = (gattc_conn_t *)Get_Next_Node(&conn->node);
            }

        }
    }

    LOG_I(gattc_srv, "[GATTC]gattc_conn_is_exist: -- end, is_exist = %d", is_exist);

    return is_exist;
}


void gattc_free_conn(gattc_conn_t *conn)
{
    if (conn) {
        Remove_Entry_List(&conn->node);
        Insert_Tail_List(&GATTC(unusedconnlist), &conn->node);
    }
}


/*By connect id find the connect list*/
gattc_conn_t *ble_gattc_link_info_by_id(uint16_t conn_id)
{
    //extern gattc_conn_t g_gattc_conn;
    gattc_conn_t *gattc_conn;
    ListNode *entry = NULL;
    ListNode *head = &GATTC(connlist);

    LOG_I(gattc_srv, "[Gattc]ble_gattc_link_info_by_id--start: con_id = %d", conn_id);

    for (entry = Get_Next_Node(head); entry != head; entry = Get_Next_Node(entry)) {

        gattc_conn = (gattc_conn_t *)entry;
        if (gattc_conn->conn_id == conn_id) {

            LOG_I(gattc_srv, "[Gattc]ble_gattc_link_info_by_id--Find conn info ");
            return (gattc_conn_t *)entry;
        }
    }
    LOG_I(gattc_srv, "[Gattc]ble_gattc_link_info_by_id--NOT Find conn info ");

    return NULL;

}

#endif /*__BLE_GATTC_SRV__*/

