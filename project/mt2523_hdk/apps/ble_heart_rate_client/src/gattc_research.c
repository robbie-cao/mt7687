#include "ble_gattc_handle_op.h"
#include "ble_gattc_utils_db.h"
#include "ble_gap.h"
#include "ble_gatt.h"
#include <stdlib.h>

uint8_t gatt_uuid[16] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x01,
                         0x18, 0x00, 0x00
                        };

static gatt_research_service_node_t *gattc_new_research_node(ble_gatt_service_t *service, ListNode *userlist)
{
    gatt_research_service_node_t *service_node = NULL;
    //service_node = (gatt_research_service_node_t *)malloc(sizeof(gatt_research_service_node_t)); ToDO
    if (service_node) {
        memset((uint8_t *)service_node, 0, sizeof(gatt_research_service_node_t));
        Init_List_Head(&service_node->userlist);

        service_node->service.start_handle = service->start_handle;
        service_node->service.end_handle = service->end_handle;
        memcpy(service_node->service.uuid, service->uuid, 16);
        if (!Is_List_Empty(userlist)) {
            Insert_Tail_List(&service_node->userlist, Get_Head_List(userlist));
        }
    }
    return service_node;
}



static int32_t gattc_research_save_service_list(ble_gatt_service_t *start_handle, uint16_t end_handle, uint16_t conn_id)
{
    gatt_service_database_t *service_db;
    gatt_service_node_t *service_node;
    gatt_research_service_node_t *reservice_node;
    ble_gap_connection_info_t *connect_info = NULL;


    connect_info = ble_gap_get_connection_info(conn_id);
    service_db = bt_gattc_query_database_is_exist(connect_info->peer_addr.address);

    if (!service_db) {
        /*in research case, the servicedb should exist*/
        return -1;
    }
    if (!Is_List_Empty(&service_db->service_list)) {
        service_node = (gatt_service_node_t *)Get_Head_List(&service_db->service_list);
    } else {
        /* service list should not empty after init*/
        return -1;
    }

    Init_List_Head(&service_db->research_list);  /* init reseach list */
    while ((ListNode *)service_node != &service_db->service_list) {
        if (service_node->service.start_handle >= start_handle && service_node->service.end_handle <= end_handle) {
            /*all service during the range between start & end handle should list in research list, and will compare after research to inform user and judge the type(add/delete/modify)*/
            reservice_node = gattc_new_research_node(&service_node->service, &service_node->userlist);
            if (!reservice_node) {
                break;
            }
            Insert_Tail_List(&service_db->research_list, &reservice_node->node);
        }
        /* try the next node in the list */
        Remove_Entry_List(&service_node->node); /*remove from currently database for research*/
        //free(&service_node->node); //free buffer ToDO
        service_node = (gatt_service_node_t *)Get_Next_Node(&service_node->node);
    }
    return 0;
}

uint16_t gattc_research_service(gattc_research_service_req_t *req)
{
#if 0
    gattc_conn_t *p_connect_cntx;
    gatt_data_t *data;
    p_connect_cntx =  ble_gattc_link_info_by_id(req->conn_id);
    if (p_connect_cntx->state == GATTC_IN_SEARCHING) {
        return GATTC_STATUS_BUSY_SEARCHING; /*return busy if searching, not search also can do research if the device in white list */
    } else if (p_connect_cntx->state == GATTC_IN_RE_SEARCHING) {
        return GATTC_STATUS_BUSY_RESEARCHING;
    } else if (p_connect_cntx->state == GATTC_IN_IDLE) {
        if (0) { /*Todo */
            return;/*not in pair list, need to search first */
        }
    }

    if (gattc_research_save_service_list(req->start_handle, req->end_handle, req->conn_id) < 0) {
        return; /* */
    }
    p_connect_cntx->state = GATTC_IN_RE_SEARCHING;
    data = &p_connect_cntx->data;
    memset(data, 0, sizeof(gatt_data_t));
    data->start_handle = req->start_handle;
    data->end_handle = req->end_handle;
    data->totalSupportedServices = MAX_SUPPORT_SERVICE;
    ble_gattc_discover_primary_services(req->conn_id, req->start_handle, req->end_handle);
#endif
    return 0;
}

static gattc_research_change_type_t gattc_compare_research_pre_list(gatt_service_node_t *curr_node, gatt_service_database_t *service_db)
{
    gatt_service_node_t *service_node, *research_node;
    research_node = (gatt_service_node_t *)Get_Head_List(&service_db->research_list);
    while ((ListNode *)research_node != &service_db->research_list) { /*search currently node first*/
        if (!memcmp(research_node->service.uuid, curr_node->service.uuid, 16)) {
            Remove_Entry_List(&research_node->node);
            return GATTC_RESERACH_MODIFY;
        }
        research_node = (gatt_service_node_t *)Get_Next_Node(&research_node->node);
    }
    return GATTC_RESERACH_ADD;

}
uint16_t gattc_research_result_notify(gattc_conn_t *conn)
{

#if 0
    gatt_service_database_t *service_db;
    gatt_service_node_t *service_node, *research_node;
    gattc_research_change_type_t change_type;
    gattc_user_connect_struct user_conn;
    gattc_user_context_t *user_node;
    ble_gap_connection_information_t *connect_info = NULL;

    connect_info = ble_gap_get_connection_information(conn->conn_id);
    service_db = bt_gattc_query_database_is_exist(connect_info->peer_addr.address);

    if (!service_db) {
        /*in research case, the servicedb should exist*/
        return;
    }

    /*get currently list and pre-view list before research*/
    if (!Is_List_Empty(&service_db->service_list) && !Is_List_Empty(&service_db->research_list)) {
        service_node = (gatt_service_node_t *)Get_Head_List(&service_db->service_list);
        research_node = (gatt_service_node_t *)Get_Head_List(&service_db->research_list);
    } else {
        /* service list should not empty after init*/
        return;
    }


    if (conn->result == BT_GATT_SUCCESS) {

        while ((ListNode *)service_node != &service_db->service_list) { /*inform the added service*/
            if (service_node->service->start_handle >= conn->data->start_handle
                    && service_node->service->end_handle <= conn->data->end_handle) {
                change_type = gattc_compare_research_pre_list(service_node);
                user_node = Get_Head_List(&service_node->userlist);
                while ((ListNode *)user_node != &service_node->userlist) {
                    user_conn->conn_id = conn->conn_id;
                    user_conn->reg_cntx = user_node;
                    ((app_callback_struct *)user_node->appCb)->research_service_cb(user_conn, conn->result, change_type);
                    user_node = (gattc_user_context_t *)Get_Next_Node(user_node->node);
                }

            }
            service_node = (gatt_service_node_t *)Get_Next_Node(service_node->node);
        }

        while ((ListNode *)research_node != &service_db->research_list) { /*inform the delete service*/
            gatt_service_node_t *preview_node;
            user_node = Get_Head_List(&research_node->userlist);
            user_conn->conn_id = conn->conn_id;
            user_conn->reg_cntx = user_node;
            ((app_callback_struct *)user_node->appCb)->research_service_cb(user_conn, conn->result, GATTC_RESERACH_DELETE);
            preview_node = research_node;
            research_node = (gatt_service_node_t *)Get_Next_Node(research_node->node);
            Remove_Entry_List(&preview_node->node);
        }
    }


    while ((ListNode *)service_node != &service_db->service_list) { /*inform the research service submitter*/
        if (memcmp(service_node->service->uuid, gatt_uuid, 16)) {
            user_node = Get_Head_List(&service_node->userlist);
            while ((ListNode *)user_node != &service_node->userlist) {
                user_conn->conn_id = conn->conn_id;
                user_conn->reg_cntx = user_node;
                ((app_callback_struct *)user_node->appCb)->research_service_cb(user_conn, conn->result, 0);
                user_node = (gattc_user_context_t *)Get_Next_Node(user_node->node);
            }
        }
    }
#endif
}



