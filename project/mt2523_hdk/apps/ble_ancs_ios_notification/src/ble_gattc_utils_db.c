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

#include "ble_gattc_utils_db.h"
#include "ble_gattc_srv.h"
#include "gattc_utils.h"
#include <stdlib.h>
#include <string.h>

#ifdef __BLE_GATTC_SRV__
ListNode g_gatt_database_table;
//static uint16_t bt_gattc_read_service_defintion_in_local(ble_gatt_service_t *service);




void ble_gattc_init_database()
{
    Init_List_Head(&g_gatt_database_table);
}


gatt_service_node_t *ble_gattc_read_service_by_uuid(uint16_t conn_id, gatt_srv_uuid_t *svc_uuid)
{
    gatt_service_database_t *servicedb;
    gatt_service_node_t *service_node = NULL;

    servicedb = ble_gattc_query_database_is_exist(conn_id);
    if (servicedb == NULL) {

        servicedb = ble_gattc_create_new_database(conn_id);
        if (servicedb == NULL) {
            return NULL;
        }
#if 0
        /*about bound service*/
        if (0) { //BleReadServiceDB((BD_ADDR *)bt_addr, servicedb) != // BT_STATUS_SUCCESS)//this need modify read FS .
            GATTC_UTILS_FREE(servicedb);
            return NULL;
        }
#endif
        Insert_Tail_List(&g_gatt_database_table, &servicedb->database_list);
    }


    /* If uuid is null, return the first service in database */
    if (!svc_uuid) {
        if (!Is_List_Empty(&servicedb->service_list)) {
            service_node = (gatt_service_node_t *)Get_Head_List(&servicedb->service_list);
        }
    } else {
        service_node = ble_gattc_spec_service_node_by_uuid(servicedb, &svc_uuid->uuid);/*service by uuid*/
    }

    if (service_node == NULL) {
        return NULL;
    }

    return service_node;
}


gatt_service_node_t *ble_gattc_spec_service_node_by_uuid(gatt_service_database_t *servicedb, gatt_uuid_t *uuid)
{
    gatt_service_node_t *service_node;

    if (!servicedb) {
        return NULL;
    }
    service_node = (gatt_service_node_t *)Get_Head_List(&servicedb->service_list);

    while ((ListNode *)service_node != &servicedb->service_list) {
        if (!memcmp(service_node->service.uuid, uuid->uuid, 16)) {/*judge equal  service*/
            return service_node;
        }
        /* try the next node in the list */
        service_node = (gatt_service_node_t *)Get_Next_Node(&service_node->node);
    }

    return NULL;
}


gatt_service_node_t *ble_gattc_next_service_node_by_uuid(gatt_service_database_t *servicedb, gatt_service_node_t *service_node, uint8_t *uuid)
{
    gatt_service_node_t *srv_node;

    srv_node = (gatt_service_node_t *)Get_Next_Node(&service_node->node);
    if (!servicedb) {
        return NULL;
    }

    if (uuid == NULL) {
        if ((ListNode *)srv_node != &servicedb->service_list) {
            return srv_node;
        } else {
            // no more service in service list
            return NULL;
        }
    } else {
        while ((ListNode *)srv_node != &servicedb->service_list) {
            if (!memcmp(srv_node->service.uuid, uuid, 16)) {/*judge equal service*/
                return srv_node;
            }
            /* try the next node in the list */
            srv_node = (gatt_service_node_t *)Get_Next_Node(&srv_node->node);
        }
    }
    return NULL;

}


gatt_include_service_node_t *ble_gattc_find_include_service(gatt_service_node_t *service_node, gatt_srv_uuid_t *include_uuid)
{

    gatt_include_service_node_t *include_node = NULL;

    if (!service_node) {
        return NULL;
    }

    if (include_uuid == NULL) {
        if (!Is_List_Empty(&service_node->include_list)) {
            include_node = (gatt_include_service_node_t *)Get_Head_List(&service_node->include_list);
        }
    } else {
        include_node = ble_gattc_find_include_service_by_service_uuid(service_node, include_uuid);
    }

    return include_node;
}


gatt_include_service_node_t *ble_gattc_find_include_service_by_service_uuid(gatt_service_node_t *service_node, gatt_srv_uuid_t *include_uuid)
{
    gatt_include_service_node_t *include_service_node = (gatt_include_service_node_t *)Get_Head_List(&service_node->include_list);

    if (!service_node) {
        return NULL;
    }

    if (include_uuid) {
        while ((ListNode *)include_service_node != &service_node->include_list) {
            if (!memcmp(include_service_node->include_service.service.uuid, include_uuid->uuid.uuid, 16)) {/*judge equal service*/
                break;
            }
            /* try the next node in the list */
            include_service_node = (gatt_include_service_node_t *)Get_Next_Node(&service_node->include_list);
        }
    }
    return include_service_node;
}


gatt_include_service_node_t *ble_gattc_find_next_include_service_by_uuid(gatt_service_node_t *service_node, gatt_include_service_node_t *include_node, gatt_srv_uuid_t *include_uuid)
{
    include_node = (gatt_include_service_node_t *)Get_Next_Node(&include_node->node);

    if (!service_node) {
        return NULL;
    }

    if (include_uuid == NULL) {
        if ((ListNode *)include_node != &service_node->include_list) {
            return include_node;
        } else {
            // no more include service in list
            return NULL;
        }
    } else {
        while ((ListNode *)include_node != &service_node->include_list) {
            if (!memcmp(include_node->include_service.service.uuid, include_uuid->uuid.uuid, 16)) {
                return include_node;
            }
            /* try the next node in the list */
            include_node = (gatt_include_service_node_t *)Get_Next_Node(&include_node->node);
        }
        return NULL;
    }
}


gatt_char_node_t *ble_gattc_find_characteristic(gatt_service_node_t *srv_node, gatt_uuid_t *char_uuid)
{
    gatt_char_node_t *char_node = NULL;

    if (!srv_node) {
        return NULL;
    }

    if (char_uuid == NULL) {
        if (!Is_List_Empty(&srv_node->char_list)) {
            char_node = (gatt_char_node_t *)Get_Head_List(&srv_node->char_list);
        }
    } else {
        char_node = ble_gattc_find_characteristic_by_uuid(srv_node, char_uuid);
    }
    return char_node;
}


gatt_char_node_t *ble_gattc_find_characteristic_by_uuid(gatt_service_node_t *service_node, gatt_uuid_t *char_uuid)
{
    gatt_char_node_t *char_node;

    if (!service_node) {
        return NULL;
    }

    char_node = (gatt_char_node_t *)Get_Head_List(&service_node->char_list);

    if (char_uuid) {
        while ((ListNode *)char_node != &service_node->char_list) {
            if (!memcmp(char_node->characteristic.uuid, char_uuid->uuid, 16)) {/*judge equal service*/
                break;
            }
            /* try the next node in the list */
            char_node = (gatt_char_node_t *)Get_Next_Node(&char_node->node);
        }
    }
    return char_node;
}


gatt_char_node_t *ble_gattc_find_next_characteristic_by_uuid(gatt_service_node_t *service_node, gatt_char_node_t *char_node, gatt_uuid_t *char_uuid)
{
    if (!service_node) {
        return NULL;
    }

    char_node = (gatt_char_node_t *)Get_Next_Node(&char_node->node);

    if (char_uuid == NULL) {
        if ((ListNode *)char_node != &service_node->char_list) {
            return char_node;
        } else {
            // no more characteristic in list
            return NULL;
        }
    } else {
        while ((ListNode *)char_node != &service_node->char_list) {
            if (!memcmp(char_node->characteristic.uuid, char_uuid->uuid, 16)) {
                return char_node;
            }
            /* try the next node in the list */
            char_node = (gatt_char_node_t *)Get_Next_Node(&char_node->node);
        }
        return NULL;
    }
}


gatt_char_desc_node_t *ble_gattc_find_descriptor_by_uuid(gatt_char_node_t *char_node, gatt_uuid_t *descri_uuid)
{
    gatt_char_desc_node_t   *desc_node = NULL;

    if (!char_node) {
        return NULL;
    }
    if (!Is_List_Empty(&char_node->char_desc_list)) {
        desc_node = (gatt_char_desc_node_t *)Get_Head_List(&char_node->char_desc_list);
    }

    if (descri_uuid) {
        while ((ListNode *)desc_node != &char_node->char_desc_list) {
            if (!memcmp(desc_node->descriptor.uuid, descri_uuid->uuid, 16)) {
                return desc_node;
            }
            /* try the next node in the list */
            desc_node = (gatt_char_desc_node_t *)Get_Next_Node(&desc_node->node);
        }
    }
    return desc_node;
}


gatt_char_desc_node_t *ble_gattc_find_next_descriptor_by_uuid(gatt_char_node_t *char_node, gatt_char_desc_node_t *descri_node, gatt_uuid_t *descri_uuid)
{
    gatt_char_desc_node_t *desc_node;

    if (!char_node || !descri_node) {
        return NULL;
    }

    desc_node = (gatt_char_desc_node_t *)Get_Next_Node(&descri_node->node);

    if (descri_uuid == NULL) {
        if ((ListNode *)desc_node != &char_node->char_desc_list) {
            return desc_node;
        } else {
            return NULL;
        }
    } else {
        while ((ListNode *)desc_node != &char_node->char_desc_list) {
            if (!(memcmp(desc_node->descriptor.uuid, descri_uuid, 16))) {
                return desc_node;
            }
            /* try the next node in the list */
            desc_node = (gatt_char_desc_node_t *)Get_Next_Node(&desc_node->node);
        }
        return NULL;
    }
}

/*this about bound service now no do*/
uint16_t ble_gattc_read_service_by_uuid_from_db(gatt_service_database_t *servicedb, ble_gatt_service_t *srv)
{

    return 0;
}


uint16_t ble_gattc_delete_service_datebase(ble_address_t *addr)
{
    /*file delete*/
    return 0;
}


uint16_t ble_gattc_read_database(ble_address_t *addr)
{
    /*File read & write*/
    return 0;

}
/*end of bound service now not do */


gatt_service_database_t *ble_gattc_query_database_is_exist(uint16_t conn_id)
{
    gatt_service_database_t *servicedb;

    servicedb = (gatt_service_database_t *)Get_Head_List(&g_gatt_database_table);

    //LOG_I(gattc_srv, "[Gattc]--start:conn_id = %d\r\n", conn_id);

    while ((ListNode *)servicedb != &(g_gatt_database_table)) {

        if (conn_id == servicedb->conn_id) {
            //LOG_I(gattc_srv, "[Gattc]--end:service db is not null = 0x%x\r\n", servicedb);
            return servicedb;
        }
        servicedb = (gatt_service_database_t *)Get_Next_Node(&servicedb->database_list);
    }
    //LOG_I(gattc_srv, "[Gattc]--end:service db is null");
    return NULL;
}


gatt_service_database_t *ble_gattc_create_new_database(uint16_t conn_id)
{
    gatt_service_database_t *servicedb;
    const ble_gap_connection_info_t *conn_info;

    servicedb = (gatt_service_database_t *)GATTC_UTILS_MALLOC(sizeof(gatt_service_database_t));


    //LOG_I(gattc_srv, "[Gattc]ble_gattc_create_new_database--start: ser_db = %x", servicedb);
    conn_info = ble_gap_get_connection_info(conn_id);

    if (!conn_info) {
        return NULL;
    }
    if (servicedb) {
        memset(servicedb, 0, sizeof(gatt_service_database_t));
        servicedb->conn_id = conn_id;
        /* Initialize List */
        Init_List_Head(&servicedb->database_list);
        Init_List_Head(&servicedb->service_list);
        Init_List_Head(&servicedb->research_list);
        //LOG_I(gattc_srv, "[Gattc]ble_gattc_create_new_database--start: ser_db_>service_list = %x", servicedb->service_list);
        //LOG_I(gattc_srv, "[Gattc]ble_gattc_create_new_database--ser_db is not null");
        return servicedb;
    }
    //LOG_I(gattc_srv, "[Gattc]ble_gattc_create_new_database--start: ser_db is null");
    return NULL;
}


void ble_gattc_free_database(gatt_service_database_t *service_db)
{
    gatt_service_node_t *srv_node;

    if (service_db) {

        Remove_Entry_List(&service_db->database_list);

        while (!Is_List_Empty(&service_db->service_list)) {

            srv_node = (gatt_service_node_t *)Remove_Head_List(&service_db->service_list);
            ble_gatt_free_service_node(srv_node);
        }

        GATTC_UTILS_FREE(service_db);
    }

}


gatt_service_node_t *ble_gattc_create_new_service_node(ble_gatt_service_t *service)
{

    gatt_service_node_t *srv_node = (gatt_service_node_t *)GATTC_UTILS_MALLOC(sizeof(gatt_service_node_t));

    //LOG_I(gattc_srv, "[Gattc]ble_gattc_create_new_service_node--start: service_node = %x", srv_node);

    if (srv_node) {

        memset(srv_node, 0, sizeof(gatt_service_node_t));
        if (service) {
            //LOG_I(gattc_srv, "[Gattc]ble_gattc_create_new_service_node--start: start_handle = %x, end_handle = %x",
            //service->start_handle, service->end_handle);
            srv_node->service.start_handle = service->start_handle;
            srv_node->service.end_handle = service->end_handle;
            memcpy(&srv_node->service.uuid, service->uuid, 16);
        }


        Init_List_Head(&srv_node->node);
        Init_List_Head(&srv_node->include_list);
        Init_List_Head(&srv_node->char_list);
        Init_List_Head(&srv_node->userlist);
        return srv_node;
    }
    //LOG_I(gattc_srv, "[Gattc]ble_gattc_create_new_service_node--start: end");
    return NULL;
}


gatt_include_service_node_t *ble_gattc_create_new_include_service_node(ble_gatt_included_service_t *service)
{

    gatt_include_service_node_t *srv_node = (gatt_include_service_node_t *)GATTC_UTILS_MALLOC(sizeof(gatt_include_service_node_t));
    if (srv_node) {
        memset(srv_node, 0, sizeof(gatt_include_service_node_t));

        if (service) {
            srv_node->include_service.handle = service->handle;
            srv_node->include_service.service.start_handle = service->service.start_handle;
            srv_node->include_service.service.end_handle = service->service.end_handle;
            memcpy(srv_node->include_service.service.uuid, service->service.uuid, 16);

        }


        return srv_node;
    }
    return NULL;
}


gatt_char_node_t *ble_gattc_create_new_char_node(ble_gatt_char_t *char_info)
{

    gatt_char_node_t *char_node = (gatt_char_node_t *)GATTC_UTILS_MALLOC(sizeof(gatt_char_node_t));

    if (char_node) {
        memset(char_node, 0, sizeof(gatt_char_node_t));
        if (char_info) {
            char_node->characteristic.property = char_info->properties;
            char_node->characteristic.handle = char_info->handle;
            char_node->characteristic.value_handle = char_info->value_handle;
            memcpy(char_node->characteristic.uuid, char_info->uuid, 16);

        }

        Init_List_Head(&char_node->char_desc_list);
        return char_node;
    }
    return NULL;
}


gatt_char_desc_node_t  *ble_gattc_create_new_descriptor_node(ble_gatt_descriptor_t *descriptor)
{

    gatt_char_desc_node_t *descr_node = (gatt_char_desc_node_t *)GATTC_UTILS_MALLOC(sizeof(gatt_char_desc_node_t));


    if (descr_node) {
        memset(descr_node, 0, sizeof(gatt_char_desc_node_t));

        if (descriptor) {
            memcpy(descr_node->descriptor.uuid, descriptor->uuid, 16);
            descr_node->descriptor.handle = descriptor->handle;
        }

        return descr_node;
    }
    return NULL;
}


void  ble_gatt_free_service_node(gatt_service_node_t *service_node)
{
    gatt_include_service_node_t *include_node;
    gatt_char_node_t *char_node;
    gatt_char_desc_node_t *char_desc_node;
    if (!service_node) {
        return;
    }
    while (!Is_List_Empty(&service_node->include_list)) {
        include_node = (gatt_include_service_node_t *)Remove_Head_List(&service_node->include_list);
        GATTC_UTILS_FREE(include_node);

    }

    while (!Is_List_Empty(&service_node->char_list)) {

        char_node = (gatt_char_node_t *)Remove_Head_List(&service_node->char_list);
        while (!Is_List_Empty(&char_node->char_desc_list)) {
            char_desc_node = (gatt_char_desc_node_t *)Remove_Head_List(&char_node->char_desc_list);

            GATTC_UTILS_FREE(char_desc_node);
        }

        GATTC_UTILS_FREE(char_node);
    }

    GATTC_UTILS_FREE(service_node);

}


gatt_service_node_t *ble_gattc_check_service_handle_range(uint16_t conn_id, uint16_t handle)
{
    gatt_service_database_t *database;
    const ble_gap_connection_info_t *conn_info;
    gatt_service_node_t *srv_node = NULL;
    ListNode *head , *srv_list_node = NULL;

    conn_info = ble_gap_get_connection_info(conn_id);
    if (conn_info == NULL) {
        //LOG_I(gattc_srv, "[Gattc]ble_gattc_check_service_handle_range--conn info is null");
        return NULL;
    }
    database = ble_gattc_query_database_is_exist(conn_id);


    //LOG_I(gattc_srv, "[Gattc]ble_gattc_check_service_handle_range--con_id = %d, handle = %d", conn_id, handle);

    if (database != NULL) {
        head = &database->service_list;

        srv_list_node = Get_Head_List(&database->service_list);
        //LOG_I(gattc_srv, "[Gattc]ble_gattc_check_service_handle_range--header = %x, srv_list_node = %x", head, srv_list_node);
        while (srv_list_node != head) {
            srv_node = (gatt_service_node_t *)srv_list_node;
            //LOG_I(gattc_srv, "[Gattc]ble_gattc_check_service_handle_range--hd = %x, s_hd = %x, e_hd = %x", handle, srv_node->service.start_handle, srv_node->service.end_handle);

            if (handle >= srv_node->service.start_handle && handle <= srv_node->service.end_handle) {
                //LOG_I(gattc_srv, "[Gattc]ble_gattc_check_service_handle_range: found service node");
                return srv_node;
            }
            srv_list_node = Get_Next_Node(srv_list_node);
        }
    }
    //LOG_I(gattc_srv, "[Gattc]ble_gattc_check_service_handle_range: not found service node");
    return NULL;
}


gatt_char_node_t *ble_gattc_check_char_handle_range(gatt_service_node_t *service_node, uint16_t handle)
{
    gatt_char_node_t *char_node;

    if (service_node) {
        char_node = (gatt_char_node_t *)Get_Head_List(&service_node->char_list);
        while ((ListNode *)char_node != &service_node->char_list) {
            if (char_node->characteristic.value_handle == handle) {
                return char_node;
            }
            /* try the next node in the list */
            char_node = (gatt_char_node_t *)Get_Next_Node(&char_node->node);
        }
    }
    return NULL;
}


gatt_char_desc_node_t *ble_gattc_check_descr_by_handle(gatt_char_node_t *char_node, uint16_t handle)
{
    gatt_char_desc_node_t *descr_node;
    if (char_node) {

        descr_node = (gatt_char_desc_node_t *)Get_Head_List(&char_node->char_desc_list);

        while ((ListNode *)descr_node != &char_node->char_desc_list) {
            if (descr_node->descriptor.handle == handle) {
                return descr_node;
            }
            descr_node = (gatt_char_desc_node_t *)Get_Next_Node(&descr_node->node);
        }
    }

    return NULL;
}


void ble_gattc_add_user_list_to_service_node(gatt_service_node_t *srv_node, uint16_t conn_id)
{
    ListNode *user;
    //ListNode *entry;
    gattc_user_context_t *user_context;
    uint8_t uuid[16];
    int8_t size = 0;

    if (!srv_node) {
        return;
    }

    user = Get_Next_Node(&GATTC(userlist));
    Init_List_Head(&srv_node->userlist);  /* init user list */
    if (!Is_List_Empty(&GATTC(userlist))) {/*judge if it has user in list*/
        while (user != &GATTC(userlist)) { /*for all user in userlist*/
            user_context = (gattc_user_context_t *)user;
            memcpy(uuid, user_context->uuid + size, 16);
            size += 16;
            if (memcmp(srv_node->service.uuid, uuid, 16) == 0) {
                gatt_user_service_node_t *user_list = (gatt_user_service_node_t *)GATTC_UTILS_MALLOC(sizeof(gatt_user_service_node_t));
                user_list->user = user_context;
                Insert_Tail_List(&srv_node->userlist, &user_list->node);
            }
            size = 0;/*new user need reset the size*/
            user = Get_Next_Node(user);
        }
    }
}
#endif /*__BLE_GATTC_SRV__*/

