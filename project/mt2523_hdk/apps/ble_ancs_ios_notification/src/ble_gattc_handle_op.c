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

/*mian for  gattc operation*/
#include "ble_gattc_handle_op.h"
#include "ble_gatt.h"
#include "ble_gattc_connect.h"
#include "ble_gattc_utils_db.h"
#include "ble_gattc_srv.h"
#include <stdlib.h>
#include "gattc_utils.h"
#include <string.h>

#ifdef __BLE_GATTC_SRV__
gattc_conn_t g_gattc_conn;


static ble_status_t ble_gattc_explore_service(uint16_t conn_id);

static void ble_gattc_discover_primary_service_cnf(ble_gattc_discover_service_rsp_t *p_event_data);

static void ble_gattc_discover_include_service_cnf(ble_gattc_find_included_service_rsp_t *p_event_data);

static void ble_gattc_discover_charactiristics_cnf(ble_gattc_discover_char_rsp_t *p_event_data);

static void ble_gattc_discover_descriptors_cnf(ble_gattc_discover_descriptor_rsp_t *p_event_data);

static void ble_gattc_read_char_descr_value_cnf(ble_gattc_read_char_descriptor_rsp_t *rsp);

static void ble_gattc_write_char_cnf(ble_gattc_write_rsp_t *rsp);

static void ble_gattc_discover_complete(gattc_conn_t *conn);

static void ble_gatt_explorer_include_service(gattc_conn_t *data);

static ble_status_t ble_gattc_find_all_descriptor_of_char(uint16_t conn_id, gatt_service_node_t *srv_node, gatt_char_node_t *char_node);

static ble_status_t ble_gattc_notify_next_service(uint16_t conn_id);

static ble_status_t ble_gattc_query_database_defination(uint16_t conn_id, ble_gatt_service_t *service);

static void ble_gattc_read_multiple_char_value_cnf(ble_gattc_read_multiple_char_value_rsp_t *rsp);

static void ble_gattc_char_val_notify_and_ind(ble_gattc_handle_value_ind_t *rsp);

static void ble_gattc_set_mtu_cnf(ble_gatt_exchange_mtu_rsp_t *rsp);

static void ble_gattc_notify_error_status(gattc_conn_t *conn);


ble_status_t ble_gattc_discover_service_by_uuid (uint16_t conn_id, uint8_t *uuid)
{
    return ble_gattc_discover_primary_services_by_uuid(conn_id, 0x0001, 0xFFFF , uuid);
}


ble_status_t ble_gattc_discover_characteristic_by_uuid(uint16_t conn_id, uint8_t *uuid)
{
    return ble_gattc_discover_char_by_uuid(conn_id, 0x0001, 0xFFFF , uuid);
}


/* For Profile API only for connect id*/
int32_t ble_gattc_start_discover_service(uint16_t conn_id)
{
    gattc_conn_t *gattc_conn = NULL;
    gatt_data_t *data = NULL;
    int32_t status = GATTC_STATUS_INVALID;
    uint16_t start_handle = 0x0001, end_handle = 0xFFFF;

    LOG_I(gattc_srv, "[Gattc]ble_gattc_start_discover_service--start: conn_id = %d", conn_id);

    gattc_conn = ble_gattc_link_info_by_id(conn_id);
    if (gattc_conn) {

        if (gattc_conn->state == GATTC_CONNECTED) {

            gattc_conn->state = GATTC_IN_SEARCHING;
            data = &gattc_conn->data;
            memset(data, 0, sizeof(gatt_data_t));
            data->start_handle = start_handle;
            data->end_handle = end_handle;
            data->totalSupportedServices = MAX_SUPPORT_SERVICE;

            //LOG_I(gattc_srv, "[Gattc]ble_gattc_start_discover_service-- from remote");

            ble_gattc_init_database();
            status = ble_gattc_discover_primary_services(conn_id, start_handle, end_handle);

        } else if (gattc_conn->state == GATTC_IN_SEARCHING) {
            status = GATTC_IN_SEARCHING;
        }
    }

    LOG_I(gattc_srv, "[Gattc]ble_gattc_start_discover_service--end: status = %d", status);
    return status;
}


static void ble_gattc_discover_primary_service_cnf(ble_gattc_discover_service_rsp_t *p_event_data)
{
    ble_gatt_service_t *service, *para, *ser = NULL;
    gattc_conn_t *gattc_conn;
    gatt_data_t *data = NULL;
    //uint16_t i, num, start_handle, uuid, para_uuid;
    uint16_t i, start_handle, uuid, para_uuid;
    ble_status_t status = BLE_STATUS_SUCCESS;
    uint16_t conn_id = p_event_data->conn_id; //connection id
    uint16_t result = p_event_data->result;  //success or error code
    uint16_t count = p_event_data->count;    //service count

    LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_primary_service_cnf--start: conn_id = %d, result = %d, count = %d", conn_id, result, count);

    gattc_conn = ble_gattc_link_info_by_id(conn_id);
    data = &gattc_conn->data;
    service = p_event_data->service;

    if (result == BLE_STATUS_SUCCESS) {

        //num = data->serviceNumber;
        for (i = 0; i < count; i++) {
            ser = &data->service_store[data->serviceNumber];
            para = service + i;
            ser->start_handle = para->start_handle;
            ser->end_handle = para->end_handle;

            LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_primary_service_cnf--s_h = %d, e_h = %d", ser->start_handle, ser->end_handle);

            data->serviceNumber++;
            memcpy(ser->uuid, para->uuid, 16);
            para_uuid = ((uint16_t)para->uuid[13]) << 8 | para->uuid[12];
            uuid = ((uint16_t)ser->uuid[13]) << 8 | ser->uuid[12];

            LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_primary_service_cnf-- 16bytes:srv_uuid = %x, para_uuid = %x", uuid, para_uuid);
        }

        LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_primary_service_cnf--s_nu= %d", data->serviceNumber);

        if (data->serviceNumber <= data->totalSupportedServices) {

            // LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_primary_service_cnf--e_h= %x, data_end = %x", service->end_handle, data->end_handle);

            start_handle = ser->end_handle + 1;
            if (service->end_handle != 0xFFFF || start_handle <= data->end_handle) {

                //LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_primary_service_cnf--new_start =  %x, new_end = %x", start_handle, data->end_handle);

                status = ble_gattc_discover_primary_services(conn_id, start_handle, data->end_handle);
            } else {

                LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_primary_service_cnf: start explore service");

                status = ble_gattc_explore_service(conn_id);
            }
        } else {

            status = ble_gattc_explore_service(conn_id);
        }
    } else if (p_event_data->result == BLE_STATUS_GATT_ATTRIBUTE_NOT_FOUND) {

        status = ble_gattc_explore_service(conn_id);
    } else {

        ble_gattc_notify_error_status(gattc_conn);

    }

    LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_primary_service_cnf--end: status = %d", status);
}


static ble_status_t ble_gattc_explore_service(uint16_t conn_id)
{

    //gatt_service_database_t *service_db = NULL;
    gatt_data_t *data = NULL;
    gattc_conn_t *gatt_conn;
    ble_gatt_service_t *srv = NULL;
    //gatt_service_node_t *service_node = NULL;
    //gatt_service_node_t *next_service_node = NULL;
    ble_status_t status = BLE_STATUS_SUCCESS;

    gatt_conn = ble_gattc_link_info_by_id(conn_id);
    //LOG_I(gattc_srv, "[Gattc]ble_gattc_explore_service--start: conn_id = %d", conn_id);

    if (gatt_conn == NULL) {

        LOG_I(gattc_srv, "[Gattc]ble_gattc_explore_service--conn link not found" );

        return status;
    }
    data = &gatt_conn->data;

    LOG_I(gattc_srv, "[Gattc]ble_gattc_explore_service--exp_idx = %d, srv_num = %d", data->service_explore_idx, data->serviceNumber);

    while (data->service_explore_idx < data->serviceNumber) {

        srv = &gatt_conn->data.service_store[data->service_explore_idx];
        status = ble_gattc_query_database_defination(conn_id, srv);

        LOG_I(gattc_srv, "[Gattc]ble_gattc_explore_service--status = %d", status);

        if (status == GATTC_STATUS_SUCCESS) {/*this case is bonded device */

            ble_gatt_explorer_include_service(gatt_conn);// in local to read inlcude service
            data->service_explore_idx++;
        } else {
            break;
        }
    }

    if (status == GATTC_STATUS_SUCCESS) {

        gatt_conn->result = GATTC_STATUS_SUCCESS;
        ble_gattc_discover_complete(gatt_conn);

    } else  if (status != BLE_STATUS_GATT_PENDING) {

        /*discover fail's case */
        gatt_conn->result = 0xdead;
        ble_gattc_discover_complete(gatt_conn);
    }
    return status;
}


static ble_status_t ble_gattc_query_database_defination(uint16_t conn_id, ble_gatt_service_t *srv)
{
    ble_status_t status = BLE_STATUS_GATT_FAILED;
    uint16_t start_handle, end_handle;
    gatt_service_node_t *service_node;
    const ble_gap_connection_info_t *conn_info;
    gatt_service_database_t *servicedb;

    //LOG_I(gattc_srv, "[Gattc]ble_gattc_query_database_defination--con_id = %d", conn_id);

    conn_info = ble_gap_get_connection_info(conn_id);
    if (conn_info == NULL) {

        LOG_I(gattc_srv, "[Gattc]ble_gattc_query_database_defination--conn info is null");
        return status;

    }

    servicedb = ble_gattc_query_database_is_exist(conn_id);

    if (!servicedb) {
        servicedb = ble_gattc_create_new_database(conn_id);
        if (servicedb) {
#if 0
            /*this part about bound device will do in next */
            if (0) { /*this device is bonded device, now no this case so use if 0 */
                if (ble_gattc_read_database(&conn_info->peer_addr) == GATTC_STATUS_SUCCESS) {
                    status = ble_gattc_read_service_by_uuid_from_db(servicedb, srv);
                }

            } else { /*unbound device */
                ble_gattc_delete_service_datebase(&conn_info->peer_addr);
            }
#endif
            Insert_Tail_List(&g_gatt_database_table, &servicedb->database_list);
        } else {
            /*create fail*/;
            status = BLE_STATUS_GATT_NO_RESOURCES;
            return status;
        }
    }

    LOG_I(gattc_srv, "[Gattc]ble_gattc_query_database_defination--end_handle = %x", srv->end_handle);

    service_node = ble_gattc_create_new_service_node(srv);
    // LOG_I(gattc_srv, "[Gattc]ble_gattc_query_database_defination--service_node = %x", service_node);
    if (service_node) {

        start_handle = srv->start_handle;
        end_handle = srv->end_handle;
        Insert_Tail_List(&servicedb->service_list, &service_node->node);
        LOG_I(gattc_srv, "[Gattc]ble_gattc_query_database_defination--db_list = %x,service_node->node = %x ", servicedb->service_list, service_node);
        ble_gattc_add_user_list_to_service_node(service_node, conn_id);
        status = ble_gattc_find_included_services(conn_id, start_handle, end_handle);

    } else {

        status = BLE_STATUS_GATT_NO_RESOURCES;
    }

    LOG_I(gattc_srv, "[Gattc]ble_gattc_query_database_defination--end: status = %d", status);
    return status;
}


static void ble_gatt_explorer_include_service(gattc_conn_t *data)
{
    /*from local read data*/
}

static void ble_gattc_discover_include_service_cnf(ble_gattc_find_included_service_rsp_t *p_event_data)
{
    gatt_service_node_t *service_node = NULL;
    gatt_include_service_node_t *include_node = NULL;
    ble_gattc_find_included_service_rsp_t *service_list = NULL;
    ble_status_t status = BLE_STATUS_GATT_FAILED;
    ble_gatt_service_t *srv_data;
    uint16_t i, conn_id, start_handle, end_handle;
    gatt_data_t *data;

    gattc_conn_t *gattc_conn;
    conn_id = p_event_data->conn_id;

    // LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_include_service_cnf--start: con_id = %d", conn_id);

    gattc_conn = ble_gattc_link_info_by_id(conn_id);
    if (gattc_conn == NULL) {
        LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_include_service_cnf--conn link is null");
        return;
    }

    data = &gattc_conn->data;
    srv_data = &data->service_store[data->service_explore_idx];

    // LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_include_service_cnf--start_h = %x, rs= %d", srv_data->start_handle, p_event_data->result);

    service_node = ble_gattc_check_service_handle_range(conn_id, srv_data->start_handle);
    if (p_event_data->result == BLE_STATUS_SUCCESS) {
        // LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_include_service_cnf--count = %d", p_event_data->count);

        service_list = p_event_data;
        for (i = 0 ; i <= p_event_data->count; i++) {
            if (service_node) {
                include_node = ble_gattc_create_new_include_service_node(service_list->service + i);
                Insert_Tail_List(&service_node->include_list, &include_node->node);
            }
        }
        start_handle = service_list->service->handle + 1;
        end_handle = service_node->service.end_handle;
        LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_include_service_cnf-- SUCCES: start_h = %d, end_h = %d", start_handle, end_handle);

        if (end_handle != 0xFFFF && start_handle < end_handle) {

            status = ble_gattc_find_included_services(conn_id, start_handle, end_handle);
        } else {

            status = ble_gattc_discover_char(conn_id, start_handle, end_handle);
        }
    } else if (p_event_data->result == BLE_STATUS_GATT_ATTRIBUTE_NOT_FOUND) {
        if (service_node) {

            start_handle = service_node->service.start_handle;
            end_handle = service_node->service.end_handle;
            LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_include_service_cnf-- NOT FOUND:start_h = %d, end_h = %d", start_handle, end_handle);

            if (start_handle <= end_handle) {
                status = ble_gattc_discover_char(conn_id, start_handle, end_handle);
            }
        }
    } else {
        ble_gattc_notify_error_status(gattc_conn);
    }

    LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_include_service_cnf-- end : status = %d", status);
}


static void ble_gattc_discover_charactiristics_cnf(ble_gattc_discover_char_rsp_t *p_event_data)
{
    uint16_t i, conn_id, start_handle, end_handle;
    gattc_conn_t *gattc_conn;
    gatt_service_node_t *service_node;
    gatt_char_node_t *char_node, *read_char_node;
    ble_gatt_service_t *srv_data;
    ble_gatt_char_t *char_info = NULL;
    ble_status_t status = BLE_STATUS_SUCCESS;
    gatt_data_t *data;

    conn_id = p_event_data->conn_id;
    gattc_conn =  ble_gattc_link_info_by_id(conn_id);

    LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_charactiristics_cnf--start:gattc_conn = %x, conn_id = %d", gattc_conn, conn_id);

    if (gattc_conn == NULL) {
        return;
    }

    data = &gattc_conn->data;
    srv_data = &data->service_store[data->service_explore_idx];
    service_node = ble_gattc_check_service_handle_range(conn_id, srv_data->start_handle);

    LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_charactiristics_cnf--start:res = %d, count = %d", p_event_data->result, p_event_data->count);

    if (p_event_data->result == BLE_STATUS_SUCCESS) {
        for (i = 0 ; i < p_event_data->count; i++) {
            char_info = p_event_data->char_info + i;
            if (service_node) {
                char_node = ble_gattc_create_new_char_node(char_info);
                Insert_Tail_List(&service_node->char_list, &char_node->node);
            }
        }
        start_handle = char_info->handle + 1;
        end_handle = service_node->service.end_handle;
        LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_charactiristics_cnf--s_hd= %d, e_hd = %d", start_handle, end_handle);

        if (char_info->handle != 0xFF && start_handle < end_handle) {

            status = ble_gattc_discover_char(conn_id, start_handle, end_handle);

        } else {

            read_char_node = (gatt_char_node_t *)Get_Head_List(&service_node->char_list);
            status = ble_gattc_find_all_descriptor_of_char(conn_id , service_node, read_char_node);
        }
    } else if (p_event_data->result == BLE_STATUS_GATT_ATTRIBUTE_NOT_FOUND) {

        LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_charactiristics_cnf--discover char done");

        read_char_node = (gatt_char_node_t *)Get_Head_List(&service_node->char_list);
        status = ble_gattc_find_all_descriptor_of_char(conn_id, service_node, read_char_node);

    } else {

        ble_gattc_notify_error_status(gattc_conn);

    }

    LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_charactiristics_cnf--start:status = %d", status);
}


static ble_status_t ble_gattc_find_all_descriptor_of_char(uint16_t conn_id, gatt_service_node_t *srv_node, gatt_char_node_t *char_node)
{
    ble_status_t    status = BLE_STATUS_SUCCESS;
    // LOG_I(gattc_srv, "[Gattc]ble_gattc_find_all_descriptor_of_char--start:n_id= %d", conn_id);

    if ((ListNode *)char_node != &srv_node->char_list) {

        gatt_char_node_t *char_next_node;
        uint16_t start_handle;
        uint16_t end_handle;
        start_handle = char_node->characteristic.value_handle + 1;

        char_next_node = (gatt_char_node_t *)Get_Next_Node(&char_node->node);

        // LOG_I(gattc_srv, "[Gattc]ble_gattc_find_all_descriptor_of_char--cha_hd = %x, srv_endhd = %x", char_next_node->characteristic.handle, srv_node->service.end_handle);

        /* This one might be the last characteristic in service */
        if ((ListNode *)char_next_node == &srv_node->char_list) {

            end_handle = srv_node->service.end_handle;

        } else {
            end_handle = char_next_node->characteristic.handle - 1;
        }

        LOG_I(gattc_srv, "[Gattc]ble_gattc_find_all_descriptor_of_char--s_hd = %x, e_hd =%x", start_handle, end_handle);

        if (start_handle <= end_handle) {
            status = ble_gattc_discover_descriptors(conn_id, start_handle, end_handle);
        } else {
            status = ble_gattc_find_all_descriptor_of_char(conn_id, srv_node, char_next_node);/*read next chara*/
        }
    } else {/*all descriptor is complete done */

        LOG_I(gattc_srv, "[Gattc]ble_gattc_find_all_descriptor_of_char--next service");

        status = ble_gattc_notify_next_service(conn_id);
    }

    LOG_I(gattc_srv, "[Gattc]ble_gattc_find_all_descriptor_of_char--end:status = %d", status);
    return status;
}


static void ble_gattc_discover_descriptors_cnf(ble_gattc_discover_descriptor_rsp_t *p_event_data)
{
    gattc_conn_t    *gattc_conn;
    gatt_service_node_t *service_node;
    gatt_char_node_t *char_node, *read_char_node;
    gatt_char_desc_node_t *descr_node;
    ble_gatt_descriptor_t *descr_data = NULL;
    uint16_t i, conn_id, count;
    uint16_t start_handle, end_handle;
    gatt_data_t *data;
    ble_gatt_service_t *srv_data;
    ble_status_t status = BLE_STATUS_GATT_FAILED;

    count = p_event_data->count;
    conn_id = p_event_data->conn_id;

    gattc_conn = ble_gattc_link_info_by_id(conn_id);

    LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_descriptors_cnf--start:con_id = %d, result = %d", conn_id, p_event_data->result);

    if (gattc_conn == NULL) {
        return;
    }

    data = &gattc_conn->data;
    srv_data = &data->service_store[data->service_explore_idx];
    service_node = ble_gattc_check_service_handle_range(p_event_data->conn_id, srv_data->start_handle);  /*There has some question*/
    char_node = ble_gattc_check_char_handle_range(service_node, p_event_data->descr->handle - 1);

    if (!service_node || !char_node) {

        LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_descriptors_cnf--service node or char node not found");
        return ;
    }

    if (p_event_data->result == BLE_STATUS_SUCCESS) {

        if (service_node && char_node) {
            // LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_descriptors_cnf--start:count =%d", count);

            for (i = 0 ; i  < count; i++) {
                descr_data = p_event_data->descr + i;
                descr_node = ble_gattc_create_new_descriptor_node(descr_data);
                Insert_Tail_List(&char_node->char_desc_list, &descr_node->node);
            }
        }

        start_handle = descr_data->handle + 1;
        end_handle = char_node->characteristic.handle - 1;

        LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_descriptors_cnf--start:s_hd =%x, e_hd = %x", start_handle, end_handle);
        if (start_handle < end_handle) {/*read one character's other descriptor*/

            status = ble_gattc_discover_descriptors(conn_id, start_handle, end_handle);

        } else {/*read next char node*/

            read_char_node = (gatt_char_node_t *)Get_Next_Node(&char_node->node);
            status = ble_gattc_find_all_descriptor_of_char(conn_id, service_node, read_char_node);
        }
    } else if (p_event_data->result == BLE_STATUS_GATT_ATTRIBUTE_NOT_FOUND) {
        read_char_node = (gatt_char_node_t *)Get_Next_Node(&char_node->node); /*next  char */
        status = ble_gattc_find_all_descriptor_of_char(conn_id, service_node, read_char_node);
    } else {
        ble_gattc_notify_error_status(gattc_conn);
    }

    LOG_I(gattc_srv, "[Gattc]ble_gattc_discover_descriptors_cnf--end: status = %d",  status);
}


static ble_status_t ble_gattc_notify_next_service(uint16_t conn_id)
{
    gattc_conn_t *gattc_conn;
    gatt_data_t *data;

    gattc_conn = ble_gattc_link_info_by_id(conn_id);
    data = &gattc_conn->data;
    data->service_explore_idx++;
    return ble_gattc_explore_service(conn_id);
}


ble_status_t ble_gattc_read_char(uint16_t conn_id, uint16_t handle)
{
    return ble_gattc_read_char_value(conn_id, handle);
}


ble_status_t ble_gattc_read_descr(uint16_t conn_id, uint16_t handle)
{
    return  ble_gattc_read_descriptor_value(conn_id, handle);
}


static void ble_gattc_read_char_descr_value_cnf(ble_gattc_read_char_descriptor_rsp_t *rsp)
{
    gattc_conn_t *gattc_conn;
    //gatt_service_node_t *service_node;
    //gatt_char_node_t *char_node, *temp_char;
    //gatt_char_desc_node_t *desc_node;
    //uint16_t conn_id, status, mtu_size;
    uint16_t conn_id;

    conn_id = rsp->conn_id;

    //LOG_I(gattc_srv, "[Gattc]ble_gattc_read_char_descr_value_cnf--start:con_id = %d, result = %d", conn_id, rsp->result);
#ifdef __GATTC_SRV_TEST__
    /*sample code , this flow will remove to app*/
    ble_gatt_get_mtu_size(conn_id, &mtu_size);
    if (rsp->result == BLE_STATUS_SUCCESS) {
        uint16_t length = rsp->handle_value->length;

        //LOG_I(gattc_srv, "[Gattc]ble_gattc_read_char_descr_value_cnf--start:len = %d", length);

        if (length == mtu_size - 1) {
            ble_gattc_read_lchar_req_t req;
            req.handle = rsp->handle_value->handle;
            service_node = ble_gattc_check_service_handle_range(rsp->conn_id, req.handle);
            char_node = ble_gattc_check_char_handle_range(service_node, req.handle);
            if (char_node) {
                char_node->characteristic.value = (uint8_t *)GATTC_UTILS_MALLOC(length);
                memset(char_node->characteristic.value, 0, length);
                memcpy(char_node->characteristic.value, rsp->handle_value->value, (mtu_size - 1));
                char_node->characteristic.value_size = mtu_size - 1;
            } else {
                temp_char = ble_gattc_check_char_handle_range(service_node, req.handle - 1);
                desc_node = ble_gattc_check_descr_by_handle(temp_char, req.handle);
                if (desc_node) {
                    desc_node->descriptor.value = (uint8_t *)GATTC_UTILS_MALLOC(length);
                    memset(desc_node->descriptor.value, 0, length);
                    memcpy(desc_node->descriptor.value, rsp->handle_value->value, (mtu_size - 1));
                }
            }
            req.offset = mtu_size - 1;
            status = ble_gattc_read_long(conn_id, &req);
        } else {/*not long */
            gattc_conn = ble_gattc_link_info_by_id(conn_id);
            if (gattc_conn) {
                if (gattc_conn->conn_func.connReadchardescrCfm) {
                    gattc_conn->conn_func.connReadchardescrCfm(rsp);
                }
            }
        }
    } else {
        gattc_conn = ble_gattc_link_info_by_id(conn_id);
        if (gattc_conn) {
            if (gattc_conn->conn_func.connReadchardescrCfm) {
                gattc_conn->conn_func.connReadchardescrCfm(rsp);
            }
        }
    }
#else
    gattc_conn = ble_gattc_link_info_by_id(conn_id);
    if (gattc_conn) {
        if (gattc_conn->conn_func.connReadchardescrCfm) {
            gattc_conn->conn_func.connReadchardescrCfm(rsp);
        }
    }
#endif
}


ble_status_t ble_gattc_read_long(uint16_t conn_id, ble_gattc_read_lchar_req_t *req)
{
    return ble_gattc_read_long_char_value(conn_id, req->handle, req->offset);
}


ble_status_t ble_gattc_read_using_char_uuid(uint16_t conn_id, //connection id
        uint16_t start_handle, //start handle
        uint16_t end_handle,  //end handle
        uint8_t *uuid128)
{
    return ble_gattc_read_char_value_by_uuid(conn_id, start_handle, end_handle, uuid128);
}


ble_status_t ble_gattc_read_multiple_characteristics_value(uint16_t conn_id, uint16_t count, uint16_t *handle)
{
    return ble_gattc_read_multiple_char_value(conn_id, count, handle);
}


static void ble_gattc_read_multiple_char_value_cnf(ble_gattc_read_multiple_char_value_rsp_t *rsp)
{
    gattc_conn_t *gattc_conn;

    gattc_conn = ble_gattc_link_info_by_id(rsp->conn_id);
    if (gattc_conn) {
        if (gattc_conn->conn_func.conReadMultiCfm) {
            gattc_conn->conn_func.conReadMultiCfm(rsp);
        }
    }
}


ble_status_t  ble_gattc_write_char(uint16_t conn_id, ble_gattc_write_char_req_t *req)
{
    return  ble_gattc_write_char_value(conn_id, req->type, req->handle, req->size, req->value);
}


ble_status_t ble_gattc_write_descr(uint16_t conn_id, ble_gattc_write_descr_req_t *req)
{
    return ble_gattc_write_descriptor_value(conn_id, req->handle, req->size, req->value);
}


static void ble_gattc_write_char_cnf(ble_gattc_write_rsp_t *rsp)/*write char vaule */
{
    gattc_conn_t *gattc_conn;

    gattc_conn = ble_gattc_link_info_by_id(rsp->conn_id);
    if (gattc_conn) {
        if (gattc_conn->conn_func.conWriteCfm) {
            gattc_conn->conn_func.conWriteCfm(rsp);
        }

    }
}


ble_status_t ble_gattc_set_mtu(uint16_t conn_id, uint16_t mtu_size)
{
    return ble_gatt_exchange_mtu(conn_id, mtu_size);
}


static void ble_gattc_set_mtu_cnf(ble_gatt_exchange_mtu_rsp_t *rsp)
{

    gattc_conn_t *gattc_conn;

    gattc_conn = ble_gattc_link_info_by_id(rsp->conn_id);
    if (gattc_conn) {
        if (gattc_conn->conn_func.conSetMtuCfm) {
            gattc_conn->conn_func.conSetMtuCfm(rsp);
        }
    }
}


static void ble_gattc_char_val_notify_and_ind(ble_gattc_handle_value_ind_t *rsp)
{
    gattc_conn_t *gattc_conn;

    gattc_conn = ble_gattc_link_info_by_id(rsp->conn_id);
    if (gattc_conn) {
        if (gattc_conn->conn_func.connIndNotifHandler) {
            gattc_conn->conn_func.connIndNotifHandler(rsp);
        }
    }

}


ble_status_t ble_gattc_handle_confirm(uint16_t conn_id, uint16_t handle)
{
    return ble_gattc_send_handle_value_confirmation(conn_id, handle);
}

#if 0
void ble_gattc_common_callback(ble_event_t event_id, const void *param)
{
    LOG_I(gattc_srv, "[GATTC]ble_gattc_common_callback: --event = %d", event_id);
    hear_rate_message_struct msg;

    switch (event_id) {
        case BLE_GATTC_DISCOVER_PRIMARY_SERVICES_RSP: {
            memcpy((void *)msg.param, (void *)param, sizeof(ble_gattc_discover_service_rsp_t));
            //ble_gattc_discover_primary_service_cnf((ble_gattc_discover_service_rsp_t *)param);
            break;
        }
        case BLE_GATTC_FIND_INCLUDED_SERVICES_RSP: {
            memcpy((void *)msg.param, (void *)param, sizeof(ble_gattc_find_included_service_rsp_t));
            break;
        }
        case BLE_GATTC_DISCOVER_CHAR_RSP: {
            memcpy((void *)msg.param, (void *)param, sizeof(ble_gattc_discover_char_rsp_t));
            break;
        }
        case BLE_GATTC_DISCOVER_DESCRIPTORS_RSP: {
            memcpy((void *)msg.param, (void *)param, sizeof(ble_gattc_discover_descriptor_rsp_t));
            break;
        }

        case BLE_GATTC_READ_DESCRIPTOR_RSP:
        case BLE_GATTC_READ_CHAR_VALUE_RSP:
        case BLE_GATTC_READ_LONG_CHAR_VALUE_RSP:
        case BLE_GATTC_READ_CHAR_VALUE_BY_UUID_RSP: {
            memcpy((void *)msg.param, (void *)param, sizeof(ble_gattc_read_char_descriptor_rsp_t));
            break;
        }

        case BLE_GATTC_READ_MULTIPLE_CHAR_VALUE_RSP: { /*need to test*/
            memcpy((void *)msg.param, (void *)param, sizeof(ble_gattc_read_multiple_char_value_rsp_t));
            break;

        }
        case BLE_GATTC_WRITE_DESCRIPTOR_RSP:
        case BLE_GATTC_WRITE_CHAR_VALUE_RSP: {
            memcpy((void *)msg.param, (void *)param, sizeof(ble_gattc_write_rsp_t));

            break;
        }
        case BLE_GATTC_HANDLE_VALUE_NOTIFICATION:
        case BLE_GATTC_HANDLE_VALUE_IND: {
            memcpy((void *)msg.param, (void *)param, sizeof(ble_gattc_handle_value_ind_t));
            break;
        }
        case BLE_GATT_EXCHANGE_MTU_RSP: {
            memcpy((void *)msg.param, (void *)param, sizeof(ble_gatt_exchange_mtu_rsp_t));
            break;
        }
    }
    msg.event_id = event_id;

    xQueueSend(app_queue, (void *)&msg, 0);

}
#endif

void ble_gattc_client_event_handler(ble_event_t event_id, const void *param)
{

    switch (event_id) {
        case BLE_GATTC_DISCOVER_PRIMARY_SERVICES_RSP: {
            ble_gattc_discover_primary_service_cnf((ble_gattc_discover_service_rsp_t *)param);
            break;
        }
        case BLE_GATTC_FIND_INCLUDED_SERVICES_RSP: {
            ble_gattc_discover_include_service_cnf((ble_gattc_find_included_service_rsp_t *)param);
            break;
        }
        case BLE_GATTC_DISCOVER_CHAR_RSP: {
            ble_gattc_discover_charactiristics_cnf((ble_gattc_discover_char_rsp_t *)param);
            break;
        }
        case BLE_GATTC_DISCOVER_DESCRIPTORS_RSP: {
            ble_gattc_discover_descriptors_cnf((ble_gattc_discover_descriptor_rsp_t *)param);
            break;
        }

        case BLE_GATTC_READ_DESCRIPTOR_RSP:
        case BLE_GATTC_READ_CHAR_VALUE_RSP:
        case BLE_GATTC_READ_LONG_CHAR_VALUE_RSP:
        case BLE_GATTC_READ_CHAR_VALUE_BY_UUID_RSP: {
            ble_gattc_read_char_descr_value_cnf((ble_gattc_read_char_descriptor_rsp_t *)param);
            break;
        }

        case BLE_GATTC_READ_MULTIPLE_CHAR_VALUE_RSP: { /*need to test*/
            ble_gattc_read_multiple_char_value_cnf((ble_gattc_read_multiple_char_value_rsp_t *)param);
            break;

        }
        case BLE_GATTC_WRITE_DESCRIPTOR_RSP:
        case BLE_GATTC_WRITE_CHAR_VALUE_RSP: {
            ble_gattc_write_char_cnf((ble_gattc_write_rsp_t *)param);
            break;
        }
        case BLE_GATTC_HANDLE_VALUE_NOTIFICATION:
        case BLE_GATTC_HANDLE_VALUE_IND: {
            ble_gattc_char_val_notify_and_ind((ble_gattc_handle_value_ind_t *)param);
            break;
        }
        case BLE_GATT_EXCHANGE_MTU_RSP: {
            ble_gattc_set_mtu_cnf((ble_gatt_exchange_mtu_rsp_t *)param);
            break;
        }
    }


}
static void ble_gattc_notify_error_status(gattc_conn_t *conn)
{
    gatt_service_database_t *service_db;
    service_db = ble_gattc_query_database_is_exist(conn->conn_id);
    ble_gattc_free_database(service_db);
    conn->result = 0xdead;
    ble_gattc_discover_complete(conn);
}


static void ble_gattc_discover_complete(gattc_conn_t *conn)
{
    // LOG_I(gattc_srv, "[GATTC]ble_gattc_discover_complete: --start");
    if (conn) {
        LOG_I(gattc_srv, "[GATTC]ble_gattc_discover_complete: --state = %d, result = %d", conn->state, conn->result);

        if (conn->state == GATTC_IN_SEARCHING) {
            if (conn->result == GATTC_STATUS_SUCCESS) {
                conn->state = GATTC_SEARCH_DONE;
            } else {
                conn->state = GATTC_CONNECTED;
            }
        } else if ( conn->state == GATTC_IN_RE_SEARCHING ) {
            conn->state = GATTC_RE_SEARCH_DONE;
        }
        if (conn->conn_func.connSearchComplete) {
            conn->conn_func.connSearchComplete(conn);
        }
    }
    //LOG_I(gattc_srv, "[GATTC]ble_gattc_discover_complete: end");
}

#endif /*__BLE_GATTC_SRV__*/

