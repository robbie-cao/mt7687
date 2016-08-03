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

#include "ble_gattc_srv.h"
#include "ble_gap.h"
#include "ble_gattc_handle_op.h"
#include "gattc_utils.h"
#include "ble_gattc_connect.h"
#include <string.h>

log_create_module(gattc_srv, PRINT_LEVEL_INFO);

#ifdef __BLE_GATTC_SRV__
/*for all op callback*/
static void gattc_read_multi_char_value_cb(ble_gattc_read_multiple_char_value_rsp_t *param);

static void gattc_read_char_descr_value_cb(ble_gattc_read_char_descriptor_rsp_t *param);

static void gattc_write_value_cb(ble_gattc_write_rsp_t *param);

static void gattc_report_indication(ble_gattc_handle_value_ind_t *param);

static void gattc_cfg_mtu_cb(ble_gatt_exchange_mtu_rsp_t *param);

static void gattc_discover_complete_cb(void *param);

static gattc_user_context_t *gattc_service_alloc_user();

static void gatt_free_user_context(gattc_user_context_t *user);

static void gattc_init_user_context(gattc_user_context_t *user);

static void gattc_get_char_info_by_node(gatt_service_node_t *srv_node, gatt_char_node_t *char_node , gattc_char_info_struct *char_info);

static void gattc_get_descr_info_by_node(gatt_service_node_t *srv_node, gatt_char_node_t *char_node, gatt_char_desc_node_t *descr_node, gattc_descr_info_t *descr_info);

gattc_context_t g_gattc_ctx;


void gattc_init(gattc_conn_func_t *conn)
{
    conn->connReadchardescrCfm = gattc_read_char_descr_value_cb;
    conn->connSearchComplete = gattc_discover_complete_cb;
    conn->connIndNotifHandler = gattc_report_indication;
    conn->conSetMtuCfm = gattc_cfg_mtu_cb;
    conn->conWriteCfm = gattc_write_value_cb;
    conn->conReadMultiCfm = gattc_read_multi_char_value_cb;
}


void  *gattc_register(gattc_register_req_struct *req, app_callback_struct *cb)
{
    gattc_user_context_t *user;
    uint8_t count, i = 0, size = 0;

    count = req->uuid_count;
    //LOG_I(gattc_srv, "[GATTC]Gattc_Register: -- start: count = %d", count);

    user = gattc_service_alloc_user();
    if (user != NULL) {
        user->appCb = cb; /*need  transfer gattc_callback_struct*/
        user->uuid_count = count;
        while (i < count) {
            user->uuid = (uint8_t *)GATTC_UTILS_MALLOC (16); /*alloc momery*/
            memcpy((user->uuid + size), req->uuid, 16);
            size += 16;
            i++;
        }
    }

    //LOG_I(gattc_srv, "[GATTC]Gattc_Register: -- start: end = %x", user);
    return (void *)user;
}


void gattc_deregister(gattc_user_context_t *user)
{
    if (user) {
        if (Is_Valid_User(user)) {
            GATTC_UTILS_FREE(user->uuid);
            gatt_free_user_context(user);
        }
    }
}


static void gatt_free_user_context(gattc_user_context_t *user)
{
    if (user) {
        if (Is_Valid_User(user)) {
            Remove_Entry_List(&user->node);/*free user in userlist */
            Insert_Tail_List(&GATTC(freeuserlist), &user->node);
        }
    }
}


void gattc_notify_scan_info_to_all_user(ble_gap_advertising_ind_t *param)
{
    ListNode *next;
    gattc_user_context_t *user;
    app_callback_struct *cb = NULL;
    //LOG_I(gattc_srv, "[GATTC]gattc_notify_conn_info_to_all_user: --start");

    if (!Is_List_Empty(&GATTC(userlist))) {/*notify all user*/

        //LOG_I(gattc_srv, "[GATTC]gattc_notify_conn_info_to_all_user: user is not null");
        next = Get_Next_Node(&GATTC(userlist));
        while (next != &GATTC(userlist)) {

            user = (gattc_user_context_t *)next;
            if (user) {
                cb = (app_callback_struct *)user->appCb;
            }
            if (cb) {
                if (cb->scan_cb) {
                    cb->scan_cb((void *)user, param);
                }
            }
            next = Get_Next_Node(next);
        }
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_notify_conn_info_to_all_user: --end");
}


void gattc_notify_conn_info_to_all_user(gattc_conn_t *gattc_conn, ble_status_t error_code, uint16_t type)
{
    app_callback_struct *cb = NULL;
    gattc_user_context_t *user;
    const ble_gap_connection_info_t *conn_info = NULL;
    ListNode *next;
    uint16_t is_ind = 0, connected = 0;
    gattc_user_connect_struct conn;

    conn.conn_id = gattc_conn->conn_id;
    //LOG_I(gattc_srv, "[GATTC]gattc_notify_conn_info_to_all_user: --start:con_id = %d", conn.conn_id);
    if (type) {
        conn_info = ble_gap_get_connection_info(conn.conn_id);
        if (conn_info == NULL) {

            //LOG_I(gattc_srv, "[GATTC]gattc_notify_conn_info_to_all_user: --conn info is null");
            return ;
        }
        is_ind = (conn_info->role == BLE_GAP_ROLE_PERIPHERAL) ? 1 : 0;

        if (type && error_code == BLE_STATUS_SUCCESS) {
            connected = 1;
        }

    }

    //LOG_I(gattc_srv, "[GATTC]gattc_notify_conn_info_to_all_user: --conn = %d, error_code = %d", is_ind, error_code);

    if (!Is_List_Empty(&GATTC(userlist))) {/*notify all user*/
        next = Get_Next_Node(&GATTC(userlist));
        while (next != &GATTC(userlist)) {
            user = (gattc_user_context_t *)next;
            if (user) {
                cb = (app_callback_struct *)user->appCb;
            }
            if (cb) {
                if (cb->connect_cb) {
                    conn.reg_cntx = (void *)user;
                    if (type) {
                        cb->connect_cb(&conn, connected, (ble_address_t *)&conn_info->peer_addr, is_ind);
                    } else {
                        cb->connect_cb(&conn, connected, NULL, is_ind);
                    }
                }
            }

            if (!type) {

                gatt_service_database_t *service_db;
                service_db = ble_gattc_query_database_is_exist(conn.conn_id);

                if (service_db) {
                    ble_gattc_free_database(service_db);
                }
            }
            next = Get_Next_Node(next);
        }
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_notify_conn_info_to_all_user: --end");
}


int32_t gattc_search_service(gattc_search_service_req_t *req)
{
    gattc_conn_t *p_connect_cntx;
    gatt_service_database_t *service_db;
    gatt_service_node_t *p_service_node;
    //const ble_gap_connection_info_t *connect_info = NULL;
    int32_t status = GATTC_STATUS_INVALID;
    uint32_t conn_id;


    conn_id = req->conn->conn_id;

    p_connect_cntx =  ble_gattc_link_info_by_id(conn_id);

    //LOG_I(gattc_srv, "[GATTC]gattc_search_service: -- start: conn_id = %d", conn_id);

    if (p_connect_cntx == NULL) {
        //LOG_I(gattc_srv, "[GATTC]gattc_search_service: -- conn link is null");
        return status;
    }

    service_db = ble_gattc_query_database_is_exist(conn_id);

    //LOG_I(gattc_srv, "[GATTC]gattc_search_service: -- state = %d", p_connect_cntx->state);

    if (p_connect_cntx->state == GATTC_SEARCH_DONE) {

        p_service_node =  ble_gattc_read_service_by_uuid(conn_id, NULL);

        while (p_service_node) {
            if (memcmp(p_service_node->service.uuid, req->uuid.uuid.uuid, 16) == 0) {
                status = GATTC_STATUS_SUCCESS;
                break;
            }
            p_service_node = ble_gattc_next_service_node_by_uuid(service_db, p_service_node, NULL);
        }
    } else if (p_connect_cntx->state == GATTC_IN_SEARCHING) {

        status = GATTC_STATUS_BUSY_SEARCHING;

    } else if (p_connect_cntx->state == GATTC_CONNECTED) {
        status = ble_gattc_start_discover_service(conn_id);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_search_service: --end:  status = %d", status);

    return status;
}


int32_t gattc_get_incl_service(gattc_search_include_service_req_t *req, gatt_srv_uuid_t *incl_uuid)
{
    gattc_conn_t *p_connect_cntx;
    gatt_service_node_t *srv_node;
    uint16_t  conn_id;
    int32_t status = GATTC_STATUS_INVALID;

    conn_id = req->conn->conn_id;
    p_connect_cntx =  ble_gattc_link_info_by_id(conn_id);
    if (p_connect_cntx) {

        if (p_connect_cntx->state == GATTC_SEARCH_DONE) {

            gatt_include_service_node_t *curr_include_node;
            srv_node = ble_gattc_read_service_by_uuid(conn_id, req->svc_uuid);
            if (req->svc_uuid == NULL) {
                curr_include_node = ble_gattc_find_include_service(srv_node, req->incl_svc_uuid);

            } else {

                curr_include_node = ble_gattc_find_include_service(srv_node, req->incl_svc_uuid);
                if (curr_include_node) {
                    curr_include_node = ble_gattc_find_next_include_service_by_uuid(srv_node, curr_include_node, req->incl_svc_uuid);
                }
            }
            if (curr_include_node) {

                incl_uuid->is_primary = 0;
                memcpy(incl_uuid->uuid.uuid, curr_include_node->include_service.service.uuid, 16);
            }
        } else if (p_connect_cntx->state == GATTC_IN_SEARCHING) {
            status = GATTC_STATUS_BUSY_SEARCHING;     /*processing*/
        }
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_get_Incl_service: --end:  status = %d", status);

    return status;
}


int32_t gattc_get_char(gattc_get_char_req_t *req, gatt_get_char_result_t *char_info)
{
    uint16_t conn_id;
    gattc_conn_t *p_connect_cntx;
    int32_t status = GATTC_STATUS_INVALID;

    conn_id = req->conn->conn_id;

    //LOG_I(gattc_srv, "[GATTC]gattc_get_char: --start : conn_id = %d", conn_id);
    p_connect_cntx =  ble_gattc_link_info_by_id(conn_id);

    if (p_connect_cntx) {
        //LOG_I(gattc_srv, "[GATTC]gattc_get_char: --state = %d", p_connect_cntx->state);

        if (p_connect_cntx->state == GATTC_SEARCH_DONE) {

            gatt_service_node_t *srv_node;
            gatt_char_node_t *char_node;
            srv_node = ble_gattc_read_service_by_uuid(conn_id, req->svc_uuid);

            //LOG_I(gattc_srv, "[GATTC]gattc_get_char: --start_uuid = %x", req->start_char_uuid);

            if (srv_node) {

                if (req->start_char_uuid == NULL) {
                    char_node = ble_gattc_find_characteristic(srv_node, NULL);
                } else {
                    char_node = ble_gattc_find_characteristic(srv_node, req->start_char_uuid);
                    if (char_node) {
                        char_node = ble_gattc_find_next_characteristic_by_uuid(srv_node, char_node, NULL);
                    }
                }

                if (char_node) {

                    status = GATTC_STATUS_SUCCESS; /*success*/
                    char_info->prop = char_node->characteristic.property;
                    memcpy(char_info->char_info.uuid, char_node->characteristic.uuid, 16);
                    //LOG_I(gattc_srv, "[GATTC]gattc_get_char: --result = %d, pro = %d", status,
                    //char_info->prop);

                } else {

                    char_info = NULL;
                    status = GATTC_STATUS_CHAR_NOT_FOUND;/*Failed*/
                }
            }
        }
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_get_char: --end : status = %d", status);
    return status;
}


int32_t gattc_get_descr(gattc_get_descr_req_t *req, gatt_uuid_t *descr_info)
{
    int32_t status = GATTC_STATUS_SRV_NOT_FOUND;
    uint16_t conn_id;
    gattc_conn_t *p_connect_cntx;

    conn_id = req->conn->conn_id;

    //LOG_I(gattc_srv, "[GATTC]gattc_get_descr: --start : conn_id = %d", conn_id);
    p_connect_cntx =  ble_gattc_link_info_by_id(conn_id);

    if (p_connect_cntx) {

        //LOG_I(gattc_srv, "[GATTC]gattc_get_descr: --state = %d", p_connect_cntx->state);

        if (p_connect_cntx->state == GATTC_SEARCH_DONE) {

            gatt_service_node_t *srv_node;
            gatt_char_node_t *char_node;
            gatt_char_desc_node_t *descr_node;
            srv_node = ble_gattc_read_service_by_uuid(conn_id, req->svc_uuid);

            if (srv_node) {
                char_node = ble_gattc_find_characteristic_by_uuid(srv_node, req->char_uuid);

                if (char_node) {
                    descr_node = ble_gattc_find_descriptor_by_uuid(char_node, req->start_descr_uuid);
                    if (req->start_descr_uuid) {/*if start not null*/
                        descr_node = ble_gattc_find_next_descriptor_by_uuid(char_node, descr_node, NULL);
                    }

                    if (descr_node) {

                        status = GATTC_STATUS_SUCCESS;
                        memcpy(descr_info->uuid, descr_node->descriptor.uuid, 16);
                    } else {
                        status = GATTC_STATUS_DESC_NOT_FOUND;
                    }
                    //LOG_I(gattc_srv, "[GATTC]gattc_get_descr: --result = %d", status);
                } else {
                    status = GATTC_STATUS_CHAR_NOT_FOUND;
                }
            } else {
                status = GATTC_STATUS_SRV_NOT_FOUND;
            }
        }
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_get_char: --end : status = %d", status);
    return status;
}


static void  gattc_get_char_info_by_node(gatt_service_node_t *srv_node, gatt_char_node_t *char_node, gattc_char_info_struct *char_info)
{
    memset(char_info, 0, sizeof (gattc_char_info_struct));
    if (srv_node && char_node) {
        memcpy(char_info->svc_uuid.uuid.uuid, srv_node->service.uuid, 16);
        memcpy(char_info->char_uuid.uuid, char_node->characteristic.uuid, 16);
        char_info->svc_uuid.is_primary = 1;
    }
}


static void gattc_get_descr_info_by_node(gatt_service_node_t *srv_node, gatt_char_node_t *char_node,
        gatt_char_desc_node_t *descr_node, gattc_descr_info_t *descr_info)
{
    memset(descr_info, 0, sizeof (gattc_descr_info_t));

    if (srv_node && char_node) {
        memcpy(descr_info->svc_uuid.uuid.uuid, srv_node->service.uuid, 16);
        memcpy(descr_info->char_uuid.uuid, char_node->characteristic.uuid, 16);
        memcpy(descr_info->descr_uuid.uuid, descr_node->descriptor.uuid, 16);
        descr_info->svc_uuid.is_primary = 1;
    }
}


int32_t gattc_read_char(gattc_read_char_req_t *req)
{
    gattc_conn_t *gattc_conn;
    uint16_t conn_id, i;
    int32_t status = GATTC_STATUS_INVALID;
    conn_id = req->conn->conn_id;

    //LOG_I(gattc_srv, "[GATTC]gattc_read_char: -- start: conn_id = %d", conn_id);
    gattc_conn = ble_gattc_link_info_by_id(conn_id);

    if (gattc_conn == NULL) {
        //LOG_I(gattc_srv, "[GATTC]gattc_read_char: --gattc_conn is null or conn_info is wrong");
        return status;
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_read_char: -- state = %d, type = %d", gattc_conn->state, req->type);

    if (gattc_conn->state == GATTC_SEARCH_DONE) {
        gatt_service_node_t *srv_node;
        gatt_char_node_t *char_node;
        uint16_t handle;
        srv_node = ble_gattc_read_service_by_uuid(conn_id, req->svc_uuid);

        if (srv_node) {

            if (req->type == READ_MULTI_CHAR) {

                uint16_t temp_handle[10] ;

                for (i = 0; i < req->count; i++) {
                    char_node = ble_gattc_find_characteristic_by_uuid(srv_node, req->char_uuid + i);

                    if (char_node) {
                        temp_handle[i] = char_node->characteristic.value_handle;
                    } else {
                        status = GATTC_STATUS_CHAR_NOT_FOUND;
                    }
                }
                status = ble_gattc_read_multiple_characteristics_value(conn_id, req->count, temp_handle);

            } else {

                char_node = ble_gattc_find_characteristic_by_uuid(srv_node, req->char_uuid);

                if (char_node) {

                    handle = char_node->characteristic.value_handle;
                    if (req->type == READ_CHAR_BY_UUID) {

                        uint16_t start_handle, end_handle;
                        start_handle = srv_node->service.start_handle;
                        end_handle = srv_node->service.end_handle;
                        status = ble_gattc_read_using_char_uuid(conn_id, start_handle, end_handle, req->char_uuid->uuid);

                    } else if (req->type == READ_LONG_CHAR) {

                        ble_gattc_read_lchar_req_t lreq;
                        lreq.handle = handle;
                        lreq.offset = req->offset;
                        ble_gattc_read_long(conn_id, &lreq);

                    } else {
                        status = ble_gattc_read_char(conn_id, handle);
                    }
                } else {

                    status = GATTC_STATUS_CHAR_NOT_FOUND;
                }
            }
        } else {
            status = GATTC_STATUS_SRV_NOT_FOUND;
        }
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_read_char: -- end: status = %d", status);
    return status;
}


int32_t gattc_read_descr(gattc_read_descr_req_t *req)
{
    gattc_conn_t *gattc_conn;
    uint16_t conn_id;
    int32_t status = GATTC_STATUS_INVALID;
#ifdef __GATTC_SRV_TEST__
    uint8_t uuid[16] = {
        0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
        0x00, 0x10, 0x00, 0x00, 0xA0, 0x2A, 0x00, 0x00,
    };
    uint16_t temp_handle[2] = {261, 303};
#endif
    conn_id = req->conn->conn_id;
    gattc_conn = ble_gattc_link_info_by_id(conn_id);

    //LOG_I(gattc_srv, "[GATTC]gattc_read_descr: -- start: conn_id = %d", conn_id);

    if (!gattc_conn) {
        //LOG_I(gattc_srv, "[GATTC]gattc_read_descr: -- gattc_conn is null or connect_info is null");
        return status;
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_read_descr: -- start: state = %d", gattc_conn->state);

    if (gattc_conn->state == GATTC_SEARCH_DONE) {

        gatt_service_node_t *srv_node;
        gatt_char_node_t *char_node;
        gatt_char_desc_node_t *desc_node;
        uint16_t handle;
        srv_node = ble_gattc_read_service_by_uuid(conn_id, req->svc_uuid);

        if (srv_node) {
            char_node = ble_gattc_find_characteristic_by_uuid(srv_node, req->char_uuid);

            if (char_node) {
                desc_node = ble_gattc_find_descriptor_by_uuid(char_node, req->descr_uuid);
                if (desc_node) {
                    handle = desc_node->descriptor.handle;
#ifdef __GATTC_SRV_TEST__
                    status = ble_gattc_read_char(conn_id, 261); /*test read char */
                    status = ble_gattc_read_using_char_uuid(conn_id, 261, 263, uuid);/*test read char by uuid */
                    status = ble_gattc_read_multiple_characteristics_value(conn_id, 2 ,
                             temp_handle); /*test read multi char*/

#else
                    status = ble_gattc_read_descr(conn_id, handle);
#endif
                } else {
                    status = GATTC_STATUS_DESC_NOT_FOUND;
                }
            } else {
                status = GATTC_STATUS_CHAR_NOT_FOUND;
            }
        } else {
            status = GATTC_STATUS_SRV_NOT_FOUND;
        }
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_read_descr: -- end: status = %d", status);
    return status;
}


int32_t gattc_write_char(gattc_write_char_req_t *req)
{
    gattc_conn_t *gattc_conn;
    uint16_t conn_id;
    ble_gattc_write_char_req_t write_req;
    int32_t status = GATTC_STATUS_INVALID;

    conn_id = req->conn->conn_id;

    //LOG_I(gattc_srv, "[GATTC]gattc_write_char: -- start: conn_id = %d", conn_id);

    gattc_conn = ble_gattc_link_info_by_id(conn_id);

    if (!gattc_conn) {

        //LOG_I(gattc_srv, "[GATTC]gattc_write_char: -- gattc_conn is null or connect_info is null");
        return status;
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_write_char: -- state = %d", gattc_conn->state);

    if (gattc_conn->state == GATTC_SEARCH_DONE) {
        gatt_service_node_t *srv_node = NULL;
        gatt_char_node_t *char_node;

        srv_node = ble_gattc_read_service_by_uuid(conn_id, req->svc_uuid);
        if (srv_node) {
            char_node = ble_gattc_find_characteristic_by_uuid(srv_node, req->char_uuid);
            //LOG_I(gattc_srv, "[GATTC]char node = %x\r\n", char_node);
            if (char_node) {
                write_req.handle = char_node->characteristic.value_handle;
                write_req.value = req->value;
                write_req.type = req->write_type;
                write_req.size = req->len;
                status = ble_gattc_write_char(conn_id, &write_req);
                //LOG_I(gattc_srv, "[GATTC]write status = %d\r\n", status);
            } else {
                status = GATTC_STATUS_CHAR_NOT_FOUND;
            }
        } else {
            status = GATTC_STATUS_SRV_NOT_FOUND;
        }
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_write_char: -- status = %d", status);
    return status;
}


int32_t gattc_write_descr(gattc_write_descr_req_t *req)
{
    gattc_conn_t *conn;
    uint16_t conn_id;
    ble_gattc_write_descr_req_t write_req;
    int32_t status = GATTC_STATUS_INVALID;

    conn_id = req->conn->conn_id;
    //LOG_I(gattc_srv, "[GATTC]gattc_write_descr: -- start: conn_id = %d", conn_id);
    conn = ble_gattc_link_info_by_id(conn_id);

    if (!conn) {
        //LOG_I(gattc_srv, "[GATTC]gattc_write_char: -- gattc_conn is null or connect_info is null");
        return status;
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_write_descr: -- state = %d", conn->state);

    if (conn->state == GATTC_SEARCH_DONE) {

        gatt_service_node_t *srv_node = NULL;
        gatt_char_node_t *char_node;
        gatt_char_desc_node_t *descr_node;

        srv_node = ble_gattc_read_service_by_uuid(conn_id, req->svc_uuid);

        if (srv_node) {

            char_node = ble_gattc_find_characteristic_by_uuid(srv_node, req->char_uuid);
            if (char_node) {

                descr_node = ble_gattc_find_descriptor_by_uuid(char_node, req->desc_uuid);

                if (descr_node) {
                    write_req.handle = descr_node->descriptor.handle;
                    write_req.size = req->len;
                    write_req.value = req->value;
                    status = ble_gattc_write_descr(conn_id, &write_req);
                } else {
                    status = GATTC_STATUS_DESC_NOT_FOUND;
                }
            } else {
                status = GATTC_STATUS_CHAR_NOT_FOUND;
            }
        } else {
            status = GATTC_STATUS_SRV_NOT_FOUND;
        }
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_write_descr: -- end:status = %d", status);
    return status;
}


int32_t gattc_configure_mtu(gattc_cfg_mtu_t *req)
{
    uint16_t  conn_id, mtu_size;
    int32_t status = GATTC_STATUS_INVALID;
    conn_id = req->conn->conn_id;

    ble_gatt_get_mtu_size(conn_id, &mtu_size);
    //LOG_I(gattc_srv, "[GATTC]gattc_config_mtu: -- start:conn_id = %d, req_mtu = %d, mtu_size = %d", conn_id, req->mtu_size, mtu_size);

    if (req->mtu_size != mtu_size) {/*is not same with current mtu size*/

        status = ble_gattc_set_mtu(conn_id, req->mtu_size);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_config_mtu: -- start:conn_id = %d, req_mtu = %d, mtu_size = %d", status);
    return status;
}


int32_t gattc_hv_confirm(gattc_hv_confirm_t *req)
{
    uint16_t conn_id;
    gattc_conn_t *conn;
    int32_t status = GATTC_STATUS_INVALID;
    conn_id = req->conn->conn_id;
    //LOG_I(gattc_srv, "[GATTC]gattc_hv_confirm: -- start:conn_id = %d", conn_id);

    conn = ble_gattc_link_info_by_id(conn_id);
    if (conn) {
        gatt_service_node_t *srv_node;
        gatt_char_node_t *char_node;
        srv_node = ble_gattc_read_service_by_uuid(conn_id, req->svc_uuid);
        if (srv_node) {

            char_node = ble_gattc_find_characteristic_by_uuid(srv_node, req->char_uuid);
            if (char_node) {
                status = ble_gattc_handle_confirm(conn_id, char_node->characteristic.handle);
            } else {
                status = GATTC_STATUS_CHAR_NOT_FOUND;
            }
        } else {
            status = GATTC_STATUS_SRV_NOT_FOUND;
        }
    }
    //LOG_I(gattc_srv, "[GATTC]gattc_hv_confirm: -- end:conn_id = %d", status);

    return status;
}


static gattc_user_context_t *gattc_service_alloc_user()
{
    ListNode *entry = NULL;

    //LOG_I(gattc_srv, "[GATTC]gattc_service_alloc_user: -- start:");
    if (!Is_List_Empty(&GATTC(freeuserlist))) {
        //LOG_I(gattc_srv, "[GATTC]gattc_service_alloc_user: free user list is not null");
        entry = Remove_Head_List(&GATTC(freeuserlist));
        Init_List_Head(entry);
        gattc_init_user_context((gattc_user_context_t *)entry);

        Insert_Tail_List(&GATTC(userlist), entry);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_service_alloc_user--end: entry = %x", entry);

    return (gattc_user_context_t *)entry;
}


static void gattc_init_user_context(gattc_user_context_t *user)
{
    user->appCb = NULL;
    user->uuid = NULL;
    user->uuid_count = 0;
    Init_List_Head(&user->node);
}


static void gattc_read_char_descr_value_cb(ble_gattc_read_char_descriptor_rsp_t *param)
{
    uint16_t handle, type, conn_id;
    ListNode *user_list;
    gatt_service_node_t *service_node;
    gatt_char_node_t *char_node;
    gatt_char_desc_node_t *desc_node;
    app_callback_struct *cb;
    gatt_user_service_node_t *user;
    gattc_char_info_struct char_info;
    gattc_descr_info_t descr_info;
    gattc_value_t value;
    gattc_user_connect_struct user_conn;

    conn_id = param->conn_id;

    handle = param->handle_value->handle;

    //LOG_I(gattc_srv, "[GATTC]gattc_read_value_cb: -- start:conn_id = %d, handle = %d, result =%d", conn_id, handle, param->result);


    service_node = ble_gattc_check_service_handle_range(conn_id, handle);
    if (!service_node) {
        //LOG_I(gattc_srv, "[GATTC]gattc_read_value_cb: -- service_node is null");
        return;
    }

    char_node = ble_gattc_check_char_handle_range(service_node, handle);

    if (char_node) {

        type = GATTC_READ_CHAR_RESULT;

    } else {

        type = GATTC_READ_DESCR_RESULT;
        char_node = ble_gattc_check_char_handle_range(service_node, handle - 1);
        desc_node = ble_gattc_check_descr_by_handle(char_node, handle);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_read_value_cb: -- type = %d", type);

    user_list = Get_Head_List(&service_node->userlist);
    while (&service_node->userlist != user_list) { /*every service node 's userlist*/

        user = (gatt_user_service_node_t *)user_list;
        user_conn.conn_id = conn_id;
        user_conn.reg_cntx = (void *)user->user;
        cb = (app_callback_struct *)user->user->appCb;
        memset(&value, 0, sizeof (gattc_value_t));

        //LOG_I(gattc_srv, "[GATTC]gattc_read_value_cb: -- param_len = %d", param->handle_value->length);
        if (param->result == BLE_STATUS_SUCCESS) {
            value.len = param->handle_value->length;
            memcpy(value.value, param->handle_value->value, value.len);
        }

        if (type == GATTC_READ_CHAR_RESULT) {
            if (cb->read_characteristic_cb) {

                gattc_get_char_info_by_node(service_node, char_node, &char_info);
                cb->read_characteristic_cb(&user_conn, param->result, &char_info, &value, param->offset);
            }
        } else {
            if (cb->read_descriptor_cb) {
                gattc_get_descr_info_by_node(service_node, char_node, desc_node, &descr_info);
                cb->read_descriptor_cb(&user_conn, param->result, &descr_info, &value);
            }
        }
        user_list = Get_Next_Node(user_list);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_read_value_cb: -- end");

}


static void gattc_write_value_cb(ble_gattc_write_rsp_t *param)
{
    ListNode *userlist;
    gatt_service_node_t *srv_node;
    gatt_char_node_t *char_node;
    gatt_char_desc_node_t *desc_node;
    gatt_user_service_node_t *user;
    app_callback_struct *cb;
    uint16_t type, conn_id;
    gattc_user_connect_struct user_connect;


    conn_id = param->conn_id;
    //LOG_I(gattc_srv, "[GATTC]gattc_write_value_cb: --start : conn_id = %d", conn_id);
    srv_node = ble_gattc_check_service_handle_range(conn_id, param->handle);
    char_node = ble_gattc_check_char_handle_range(srv_node, param->handle);

    if (!srv_node) {
        //LOG_I(gattc_srv, "[GATTC]gattc_write_value_cb: --srv_node is null or char_node is null");
        return ;
    }

    if (char_node) {

        type = GATTC_WRITE_CHAR_RESULT;
    } else {
        type = GATTC_WRITE_DESC_RESULT;
        char_node = ble_gattc_check_char_handle_range(srv_node, param->handle - 1); /*as spec*/
        desc_node = ble_gattc_check_descr_by_handle(char_node, param->handle);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_write_value_cb: type =%d", type);

    userlist = Get_Head_List(&srv_node->userlist);
    while (userlist != &srv_node->userlist) {/*every service node 's userlist*/

        //LOG_I(gattc_srv, "[GATTC]gattc_write_value_cb: user_list is not empty =%x", userlist);
        user = (gatt_user_service_node_t *)userlist;
        user_connect.conn_id = conn_id;
        user_connect.reg_cntx = (void *)user->user;
        cb = (app_callback_struct *)user->user->appCb;

        if (type == GATTC_WRITE_CHAR_RESULT) {
            if (cb->write_characteristic_cb) {

                gattc_char_info_struct char_info;
                gattc_get_char_info_by_node(srv_node, char_node, &char_info);
                cb->write_characteristic_cb(&user_connect, param->result, &char_info);
            }
        } else {

            gattc_descr_info_t descr_info;
            if (cb->write_descriptor_cb) {

                gattc_get_descr_info_by_node(srv_node, char_node, desc_node, &descr_info);
                cb->write_descriptor_cb(&user_connect, param->result, &descr_info);
            }
        }
        userlist = Get_Next_Node(userlist);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_write_value_cb:--end: user =%x", userlist);
}


static void gattc_cfg_mtu_cb(ble_gatt_exchange_mtu_rsp_t *param)
{
    /*there only record cfg mtu rsp callbcak , no need do anything */
    //LOG_I(gattc_srv, "[GATT]gattc_cfg_mtu_cb:result = %d", param->expected_mtu, param->result);
}


static void gattc_report_indication(ble_gattc_handle_value_ind_t *param)
{
    ListNode *userlist;
    gatt_service_node_t *service_node;
    gatt_char_node_t *char_node;
    //gatt_char_desc_node_t *desc_node = NULL;
    gatt_user_service_node_t *user;
    app_callback_struct *cb;

    //int8_t size = 0;
    //uint16_t type = 0;
    gattc_value_t value;
    gattc_user_connect_struct conn;

    gattc_char_info_struct char_info;
    service_node = ble_gattc_check_service_handle_range(param->conn_id, param->handle);

    if (!service_node) {
        //LOG_I(gattc_srv, "[GATTC]gattc_report_indication:---srv_node is null or char_node is null");
        return ;
    }

    if (service_node) {
        char_node = ble_gattc_check_char_handle_range(service_node, param->handle);
    }

    if (char_node) {

        //type = GATTC_CHAR_IND;

    } else {

        //type = GATTC_DESCR_IND; /*char may be not find,because it is desc callback*/
        char_node = ble_gattc_check_char_handle_range(service_node, param->handle - 1);
        //desc_node = ble_gattc_check_descr_by_handle(char_node, param->handle);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_report_indication:--type = %d", type);

    userlist = Get_Head_List(&service_node->userlist);
    gattc_get_char_info_by_node(service_node, char_node, &char_info);

    while (userlist != &service_node->userlist) { /*service node 's all userlist*/

        user = (gatt_user_service_node_t *)userlist;
        conn.conn_id = param->conn_id;
        conn.reg_cntx = (void *)user->user;
        cb = (app_callback_struct *)user->user->appCb;
        if (cb) {
            if (cb->notify_cb) {
                memcpy(value.value, param->data, param->length);
                value.len = param->length;
                cb->notify_cb(&conn, &char_info, &value, param->type);
            }
        }
        userlist = Get_Next_Node(userlist);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_report_indication:--end user_lisr = %x", userlist);
}


static void gattc_discover_complete_cb(void *param)
{
    gattc_conn_t *conn;
    ListNode *user;
    conn = (gattc_conn_t *)(param);
    /*notify app*/
    if (conn) {
        if (conn->state == GATTC_SEARCH_DONE) {
            user = &GATTC(userlist);
            while ((user = Get_Next_Node(user)) != &GATTC(userlist)) { /*for all user in userlist*/
                gattc_user_context_t *user_context = (gattc_user_context_t *)user;
                gattc_user_connect_struct user_conn;
                user_conn.reg_cntx = (void *)user;
                user_conn.conn_id = conn->conn_id;
                ((app_callback_struct *)user_context->appCb)->search_complete_cb(&user_conn, conn->result);
            }
        } else if ( conn->state == GATTC_RE_SEARCH_DONE ) {

            //gattc_research_result_notify(conn);
        }
    }
}


static void gattc_read_multi_char_value_cb(ble_gattc_read_multiple_char_value_rsp_t *param)
{
    uint16_t handle, conn_id;
    ListNode *user_list;
    gatt_service_node_t *service_node;
    gatt_char_node_t *char_node;
    //gatt_char_desc_node_t *desc_node;
    app_callback_struct *cb;
    gatt_user_service_node_t *user;
    gattc_char_info_struct char_info;
    //gattc_descr_info_t descr_info;
    gattc_value_t value;
    gattc_user_connect_struct user_conn;

    conn_id = param->conn_id;
    handle = param->handle;

    //LOG_I(gattc_srv, "[GATTC]gattc_read_value_cb: -- start:conn_id = %d, handle = %d, result =%d", conn_id, handle, param->result);

    service_node = ble_gattc_check_service_handle_range(conn_id, handle);

    if (!service_node) {
        //LOG_I(gattc_srv, "[GATTC]gattc_read_value_cb: -- service_node is null");
        return;
    }

    char_node = ble_gattc_check_char_handle_range(service_node, handle);
    user_list = Get_Head_List(&service_node->userlist);

    while (&service_node->userlist != user_list) { /*service node 's all userlist*/

        user = (gatt_user_service_node_t *)user_list;
        user_conn.conn_id = conn_id;
        user_conn.reg_cntx = (void *)user->user;
        cb = (app_callback_struct *)user->user->appCb;
        memset(&value, 0, sizeof (gattc_value_t));
        memset(&char_info, 0, sizeof (gattc_char_info_struct));
        //LOG_I(gattc_srv, "[GATTC]gattc_read_value_cb: -- param_len = %d", param->length);

        if (cb->read_characteristic_cb) {
            if (param->result == BLE_STATUS_SUCCESS) {
                value.len = param->length;
                memcpy(value.value, param->value, value.len);
                gattc_get_char_info_by_node(service_node, char_node, &char_info);
            }
            cb->read_characteristic_cb(&user_conn, param->result, &char_info, &value, 0);
        }

        user_list = Get_Next_Node(user_list);
    }

    //LOG_I(gattc_srv, "[GATTC]gattc_read_value_cb: -- end");

}

#endif /*__BLE_GATTC_SRV__*/

