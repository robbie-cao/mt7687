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

#include "bt_sink_conmgr.h"
#include "bt_gap.h"
#include "bt_sink_event.h"
#include "bt_sink_avrcp.h"
#include "bt_sink_a2dp.h"
#include "bt_sink_hf.h"
#include "bt_sink_db.h"
#include "bt_sink_custom_db.h"
#include "bt_sink_utils.h"
#include "bt_sink_music.h"

extern bt_sink_profile_t bt_sink_hf[BT_SINK_CM_MAX_DEVICE_NUMBER];
extern SinkProfileA2dp sink_a2dp[BT_SINK_CM_MAX_DEVICE_NUMBER];
extern SinkProfileAvrcp sink_avrcp[BT_SINK_CM_MAX_DEVICE_NUMBER];

static uint8_t bt_sink_db_common_comparator(const uint8_t *record, const void *key);

bt_sink_cm_context_t sink_conmgr;
static bt_sink_cm_dev_info_t g_sink_cm_dev_info[BT_SINK_CM_MAX_TRUSTED_DEV];
static bt_sink_db_t g_bt_sink_cnmgr_db = {
    .name_p = BT_SINK_DB_DEVICE_INFO,
    .dirty = false,
    .size = sizeof(bt_sink_cm_dev_info_t),
    .count = BT_SINK_CM_MAX_TRUSTED_DEV,
    .used = 0,
    .comparator_function_p = bt_sink_db_common_comparator,
    .buffer_p = (uint8_t *)g_sink_cm_dev_info
};

#define BT_SINK_CM_STATE_CHANGE(device, new_state)                                    \
{                                                                                     \
    bt_sink_report("[Sink][CM] State Change, previous_state:%d, new_state:%d", device->state, new_state); \
    device->state = new_state;                                    \
}

#ifdef __BT_SINK_DEBUG_INFO__
const static char *g_gap_event_string[] = {
    "BT_GAP_DISCOVERY_RESULT_IND", /**< A mandatory event, indicates a BR/EDR Controller or multiple BR/EDR Controllers have responded so far during the current Inquiry process.
                                                         For more details on the event structure, please refer to #bt_gap_discovery_result_ind_t. */

    "BT_GAP_DISCOVERY_CNF",                           /**< A mandatory event, indicates the Inquiry is complete.
                                                         For more details on the event structure, please refer to #bt_gap_discovery_cnf_t. */

    "BT_GAP_CANCEL_INQUIRY_CNF",                      /**< A mandatory event, indicates the Inquiry Cancel is complete.
                                                         For more details on the event structure, please refer to #bt_gap_cancel_inquiry_cnf_t. */

    "BT_GAP_WRITE_INQUIRY_TX_CNF",                    /**< A mandatory event, indicates the Write Inquiry Transmit Power Level is complete.
                                                         For more details on the event structure, please refer to #bt_gap_write_inquiry_tx_cnf_t. */

    "BT_GAP_READ_INQUIRY_TX_CNF",                     /**< A mandatory event, indicates the Read Inquiry Response Transmit Power Level is complete.
                                                         For more details on the event structure, please refer to #bt_gap_read_inquiry_tx_cnf_t. */

    "BT_GAP_SET_SCAN_MODE_CNF",              /**< A mandatory event, indicates the Write Scan Enable Mode is complete. If the new mode is the same as the current mode, user will not receive this event.
                                                         For more details on the event structure, please refer to #bt_gap_set_scan_mode_cnf_t. */

    "BT_GAP_READ_RSSI_CNF",                           /**< A mandatory event, indicates the Read RSSI is complete.
                                                         For more details on the event structure, please refer to #bt_gap_read_rssi_cnf_t. */

    "BT_GAP_POWER_ON_CNF",                            /**< A mandatory event, indicates the Bluetooth power on is complete. If Bluetooth is already power on, user will not receive this event.
                                                         For more details on the event structure, please refer to #bt_gap_power_on_cnf_t. */

    "BT_GAP_POWER_OFF_CNF",                           /**< A mandatory event, indicates the Bluetooth power off is complete. If Bluetooth is already power off, user will not receive this event.
                                                         For more details on the event structure, please refer to #bt_gap_power_off_cnf_t. */

    "BT_GAP_READ_REMOTE_NAME_CNF",                    /**< A mandatory event, indicates the Remote Name Request is complete.
                                                         For more details on the event structure, please refer to #bt_gap_read_remote_name_cnf_t. */

    "BT_GAP_CREATE_LINK_CNF",                         /**< A mandatory event, indicates the Create Connection is complete.
                                                         For more details on the event structure, please refer to #bt_gap_create_link_cnf_t. */

    "BT_GAP_DETACH_SINGLE_LINK_CNF",                  /**< A mandatory event, indicates the Detach Link is complete.
                                                         For more details on the event structure, please refer to #bt_gap_detach_single_link_cnf_t. */

    "BT_GAP_SET_ROLE_CNF",                            /**< A mandatory event, indicates the result of the Set Role process.
                                                         For more details on the event structure, please refer to #bt_gap_set_role_cnf_t. */

    "BT_GAP_LINK_ACCEPT_CONFIRM_IND",                 /**< Event is reserved. */

    "BT_GAP_ENTER_TEST_MODE_CNF",                      /**< A mandatory event, indicates the Enable Device Under Test Mode is complete.
                                                         For more details on the event structure, please refer to #bt_gap_enter_test_mode_cnf_t. */

    "BT_GAP_SET_SNIFF_SUBRATE_POLICY_CNF",            /**< A mandatory event, indicates setting a policy for Sniff Subrating is complete.
                                                         For more details on the event structure, please refer to #bt_gap_set_sniff_subrate_policy_cnf_t. */

    "BT_GAP_USER_CONFIRM_RESPONSE_CNF",               /**< A mandatory event, indicates the User Confirmation Request Reply is complete.
                                                         For more details on the event structure, please refer to #bt_gap_user_confirm_response_cnf_t. */

    "BT_GAP_IO_CAPABILITY_RESPONSE_CNF",              /**< A mandatory event, indicates the IO Capability Request Reply is complete.
                                                         For more details on the event structure, please refer to #bt_gap_io_capability_response_cnf_t. */

    "BT_GAP_PIN_CODE_RESPONSE_CNF",                   /**< A mandatory event, indicates the PIN Code Request Reply is complete.
                                                         For more details on the event structure, please refer to #bt_gap_pin_code_response_cnf_t. */

    "BT_GAP_IO_CAPABILITY_REQUEST_IND",               /**< A mandatory event, indicates the IO capabilities of the Host are required for a simple pairing process.
                                                         For more details on the event structure, please refer to #bt_gap_io_capability_request_ind_t. */

    "BT_GAP_BONDING_CNF",                             /**< A mandatory event, indicates the Bonding process is complete.
                                                         For more details on the event structure, please refer to #bt_gap_bonding_cnf_t. */

    "BT_GAP_BONDING_RESULT_IND",                      /**< A mandatory event, indicates the result of the Bonding process.
                                                         For more details on the event structure, please refer to #bt_gap_bond_result_ind_t. */

    "BT_GAP_USER_CONFIRM_REQUEST_IND",                /**< A mandatory event, indicates user confirmation of a numeric value is required.
                                                         For more details on the event structure, please refer to #bt_gap_user_confirm_request_ind_t. */

    "BT_GAP_USER_PASSKEY_NOTIFICATION_IND",           /**< A mandatory event, used to provide a passkey for the Host to display to the user as required as part of a Simple Pairing process.
                                                         For more details on the event structure, please refer to #bt_gap_user_passkey_notification_ind_t. */

    "BT_GAP_KEYPRESS_NOTIFICATION_IND",               /**< A mandatory event, sent to the Host after a passkey notification has been received by the Link Manager on the given BD_ADDR.
                                                         For more details on the event structure, please refer to #bt_gap_keypress_notification_ind_t. */

    "BT_GAP_USER_PASSKEY_REQUEST_IND",                /**< A mandatory event, indicates a passkey is required as part of a Simple Pairing process.
                                                         For more details on the event structure, please refer to #bt_gap_user_passkey_request_ind_t. */

    "BT_GAP_PIN_CODE_REQUEST_IND",                    /**< A mandatory event, indicates a PIN code is required as part of a Pairing process.
                                                         For more details on the event structure, please refer to #bt_gap_pin_code_request_ind_t. */

    "BT_GAP_SEND_KEYPRESS_COMPLETE_IND",              /**< A mandatory event, indicates the Send Keypress Notification is complete.
                                                         For more details on the event structure, please refer to #bt_gap_send_keypress_complete_ind_t. */

    "BT_GAP_DELETE_LINK_KEY_IND",                     /**< A mandatory event, indicates the link key has to be deleted.
                                                         For more details on the event structure, please refer to #bt_gap_delete_link_key_ind_t. */

    "BT_GAP_AUTHENTICATE_RESULT_IND",                 /**< A mandatory event, indicates the authentication result.
                                                         For more details on the event structure, please refer to #bt_gap_authenticate_result_ind_t. */
};
#endif /* __BT_SINK_DEBUG_INFO__ */

static void bt_sink_cm_handle_page_scan_switch(void);
static void bt_sink_cm_handle_inquiry_scan_switch(void);
static void bt_sink_cm_handle_power_on_cnf(void);
static void bt_sink_cm_connect_internal(bt_address_t *addr);
static void bt_sink_cm_request_done(bt_sink_status_t result, bt_sink_cm_node_t *req);

static bool bt_sink_cm_is_valid_addr(bt_address_t *addr)
{
    uint32_t i;
    bool result = false;

    if (NULL != addr) {
        for (i = 0; i < 6; i++) {
            if (0 != (*addr).address[i]) {
                return true;
            }
        }
    }
    return result;
}

static void bt_sink_cm_node_insert_head(bt_sink_cm_node_t *head, bt_sink_cm_node_t *node)
{
    bt_sink_assert(head && node);
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

static void bt_sink_cm_node_insert_tail(bt_sink_cm_node_t *head, bt_sink_cm_node_t *node)
{
    bt_sink_assert(head && node);
    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
}

static bt_sink_cm_node_t *bt_sink_cm_node_new(bt_sink_cm_request_t *request)
{
    bt_sink_cm_node_t *new_req;

    bt_sink_assert(request);

    new_req = bt_sink_memory_alloc(sizeof(bt_sink_cm_node_t));
    bt_sink_memset(new_req, 0, sizeof(bt_sink_cm_node_t));
    new_req->parameters = bt_sink_memory_alloc(sizeof(bt_sink_cm_request_t));
    bt_sink_memcpy(new_req->parameters, request, sizeof(bt_sink_cm_request_t));

    return new_req;
}

static bt_sink_cm_node_t *bt_sink_cm_node_get_next(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_cm_node_t *cur_req = sink_conmgr.request_list.next;
    bt_sink_cm_node_t *result = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (cur_req != &sink_conmgr.request_list) {
        result = cur_req;
        cur_req->next->prev = cur_req->prev;
        cur_req->prev->next = cur_req->next;
        result->next = result;
        result->prev = result;
    }
    return result;
}

static void bt_sink_cm_node_delete(bt_sink_cm_node_t *del_req)
{
    if (del_req) {
        if (del_req->parameters) {
            bt_sink_memory_free(del_req->parameters);
        }
        bt_sink_memory_free(del_req);
    }
}

static void bt_sink_cm_node_remove(bt_sink_cm_node_t *node)
{
    bt_sink_assert(node);
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static void bt_sink_cm_request_execute(bt_sink_cm_node_t *req)
{
    switch (req->parameters->type) {
        case BT_SINK_CM_REQ_TYPE_NORMAL:
        case BT_SINK_CM_REQ_TYPE_POWER_ON:
        case BT_SINK_CM_REQ_TYPE_LINK_LOST: {
            bt_sink_cm_connect_internal(&req->parameters->addr);
        }
        break;

        default:
            break;
    }
}

static void bt_sink_cm_request_delay_execute(void *params)
{
    bt_sink_cm_node_t *req = (bt_sink_cm_node_t *)params;

    if (req) {
        bt_sink_cm_request_execute(req);
    }
}

static bt_sink_status_t bt_sink_cm_request_permit(bt_sink_cm_request_t *request)
{
    bt_sink_status_t result = BT_SINK_STATUS_INVALID_PARAM;

    bt_sink_assert(request);

    switch (request->type) {
        case BT_SINK_CM_REQ_TYPE_NORMAL:
        case BT_SINK_CM_REQ_TYPE_POWER_ON:
        case BT_SINK_CM_REQ_TYPE_LINK_LOST:
            if (!bt_sink_cm_is_valid_addr(&request->addr)) {
                result = BT_SINK_STATUS_FAIL;
            } else if (BT_SINK_CM_MAX_DEVICE_NUMBER == sink_conmgr.connected_dev) {
                result = BT_SINK_STATUS_MAX_LINK;
            } else if (NULL != bt_sink_cm_find_device(BT_SINK_CM_FIND_BY_ADDR, &request->addr)) {
                result = BT_SINK_STATUS_LINK_EXIST;
            } else if (!(sink_conmgr.flags & BT_SINK_FLAG_POWER_ON)) {
                result = BT_SINK_STATUS_INVALID_STATUS;
            } else {
                result = BT_SINK_STATUS_SUCCESS;
            }
            break;

        default:
            break;
    }
    bt_sink_report("[Sink][CM] permit result:%d", result);
    return result;
}

static bt_sink_status_t bt_sink_cm_request_run(void)
{
    bt_sink_status_t result;

    if (sink_conmgr.request_pending != NULL) {
        result = BT_SINK_STATUS_PENDING;
    } else {
        bt_sink_cm_node_t *req = bt_sink_cm_node_get_next();

        if (req && req->parameters) {
            result = bt_sink_cm_request_permit(req->parameters);

            if (req->parameters->attampts > 0) {
                req->parameters->attampts--;
            }

            if (BT_SINK_STATUS_SUCCESS == result) {
                sink_conmgr.request_pending = req;

                if (req->parameters->delay_time > 0) {
                    bt_sink_report("[Sink][CM] request delay:%d, type:%d", req->parameters->delay_time, req->parameters->type);
                    bt_sink_timer_start(BT_SINK_TIMER_CM_REQUEST_DELAY,
                                        req->parameters->delay_time,
                                        bt_sink_cm_request_delay_execute,
                                        (void *)req);
                } else {
                    bt_sink_cm_request_execute(req);
                }
            } else {
                bt_sink_cm_request_done(result, req);
            }
        } else {
            result = BT_SINK_STATUS_NO_REQUEST;
        }
    }
    return result;
}

void bt_sink_cm_request_done(bt_sink_status_t result, bt_sink_cm_node_t *req)
{
    if (req && req->parameters) {
        bt_sink_report("[Sink][CM] request done, result:%d, pending:0x%x, attampts:%d",
                       result, sink_conmgr.request_pending, req->parameters->attampts);

        sink_conmgr.request_pending = NULL;

        if (BT_SINK_STATUS_SUCCESS != result && req->parameters->attampts > 0) {
            req->parameters->delay_time += BT_SINK_REQUEST_DELAY_TIME_INCREASE;
            bt_sink_cm_node_insert_tail(&sink_conmgr.request_list, req);
            bt_sink_cm_request_run();
        } else if (BT_SINK_STATUS_NEED_RETRY == result) {
            req->parameters->delay_time = BT_SINK_REQUEST_DELAY_TIME;
            bt_sink_cm_node_insert_head(&sink_conmgr.request_list, req);
            bt_sink_cm_request_run();
        } else {
            if (req->parameters->callback) {
                req->parameters->callback(result, req->parameters);
            }
            bt_sink_cm_node_delete(req);
            bt_sink_cm_request_run();
        }
    }
}

bt_sink_cm_node_t *bt_sink_cm_request_find(bt_sink_cm_request_t *request)
{
    bt_sink_cm_node_t *find_node = NULL;

    if (request) {
        bt_sink_cm_node_t *cur_node = sink_conmgr.request_list.next;

        while (cur_node != &sink_conmgr.request_list) {
            if (cur_node->parameters &&
                    0 == bt_sink_memcmp(&cur_node->parameters->addr, &request->addr, sizeof(bt_address_t))) {
                find_node = cur_node;
                break;
            }
            cur_node = cur_node->next;
        }
    }

    bt_sink_report("[Sink][CM] request find:0x%x", find_node);

    return find_node;
}

bt_sink_status_t bt_sink_cm_request_new(bt_sink_cm_request_t *new_req)
{
    bt_sink_status_t result;

    if (NULL == bt_sink_cm_request_find(new_req)) {
        bt_sink_cm_node_t *new_node = bt_sink_cm_node_new(new_req);

        if (NULL != new_node) {
            bt_sink_cm_node_insert_tail(&sink_conmgr.request_list, new_node);
        }

        result = bt_sink_cm_request_run();

        bt_sink_report("[Sink][CM] request new, result:%d, pending:0x%x", result, sink_conmgr.request_pending);

        if (BT_SINK_STATUS_PENDING == result) {
            new_node->parameters->delay_time = BT_SINK_REQUEST_DELAY_TIME;
        }
    } else {
        result = BT_SINK_STATUS_REQUEST_EXIST;
    }
    return result;
}

void bt_sink_cm_request_cancel(bt_sink_cm_req_type_t type)
{
    bt_sink_cm_node_t *cur_node = sink_conmgr.request_list.next;
    bt_sink_cm_node_t *del_node = NULL;;

    while (cur_node != &sink_conmgr.request_list) {
        if (cur_node->parameters &&
                (type == cur_node->parameters->type || type == BT_SINK_CM_REQ_TYPE_NONE)) {
            del_node = cur_node;
        }

        cur_node = cur_node->next;

        if (del_node != NULL) {
            bt_sink_cm_node_remove(del_node);
            bt_sink_cm_node_delete(del_node);
            del_node = NULL;
        }
    }
}


#ifdef __BT_SINK_DEBUG_INFO__
static void bt_sink_cm_dump_link_key(uint8_t *linkkey)
{
    bt_sink_report("link key:%02x,%02x,%02x,%02x,  %02x,%02x,%02x,%02x,  %02x,%02x,%02x,%02x,  %02x,%02x,%02x,%02x",
                   linkkey[0], linkkey[1], linkkey[2], linkkey[3], linkkey[4], linkkey[5], linkkey[6], linkkey[7],
                   linkkey[8], linkkey[9], linkkey[10], linkkey[11], linkkey[12], linkkey[13], linkkey[14], linkkey[15]);
}
#endif /* __BT_SINK_DEBUG_INFO__ */

uint8_t bt_sink_db_common_comparator(const uint8_t *record, const void *key)
{
    bt_sink_cm_common_record *btdbRecord = (bt_sink_cm_common_record *)record;
    return (!bt_sink_memcmp(&btdbRecord->bt_addr, key, sizeof(bt_address_t)));
}


static bt_sink_cm_remote_device_t *bt_sink_cm_alloc_device(const bt_address_t *addr)
{
    uint32_t i;
    bt_sink_cm_remote_device_t *device;
    bt_sink_cm_remote_device_t *empty;

    for (i = 0, device = NULL, empty = NULL; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
        if (sink_conmgr.devices[i].conn_id != 0) {
            if (0 == bt_sink_memcmp((void *)addr, (void *)&sink_conmgr.devices[i].addr, sizeof(bt_address_t))) {
                device = &sink_conmgr.devices[i];
                break;
            }
        } else {
            empty = &sink_conmgr.devices[i];
        }
    }

    if (NULL == device) {
        device = empty;
    }

    if (NULL != device) {
        bt_sink_memcpy((void *)&device->addr, (void *) addr, sizeof(bt_address_t));
    }

    return device;
}

static void bt_sink_cm_free_device(bt_sink_cm_remote_device_t *device_p)
{
    if (NULL != device_p) {
        bt_sink_memset((void *)&device_p->addr, 0, sizeof(bt_address_t));
        device_p->conn_id = 0;
        device_p->state = BT_SINK_CM_STATE_DISCONNECTED;
    }
}

bt_sink_cm_remote_device_t *bt_sink_cm_find_device(bt_sink_cm_find_by_type_t type, void *param)
{
    bt_sink_cm_remote_device_t *dev_p;
    uint32_t i;

    for (i = 0, dev_p = NULL; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
        if (BT_SINK_CM_FIND_BY_ID == type && sink_conmgr.devices[i].conn_id == *((uint16_t *)param)) {
            dev_p = &sink_conmgr.devices[i];
            bt_sink_report("conn_id:%d", dev_p->conn_id);
            break;
        } else if (BT_SINK_CM_FIND_BY_ADDR == type &&
                   bt_sink_memcmp(param, &sink_conmgr.devices[i].addr, sizeof(bt_address_t)) == 0) {
            dev_p = &sink_conmgr.devices[i];
            bt_sink_report("conn_id:%d", dev_p->conn_id);
            break;
        }
    }
    bt_sink_report("dev_p:%x", dev_p);
    return dev_p;
}

void bt_sink_cm_connect_internal(bt_address_t *addr)
{
    if (bt_sink_cm_is_valid_addr(addr)) {
        bt_sink_cm_remote_device_t *device_p = bt_sink_cm_alloc_device(addr);
        bt_sink_report("[Sink][CM] device_p:%x, address:%x:%x:%x:%x:%x:%x", device_p,
                       addr->address[0], addr->address[1], addr->address[2], addr->address[3], addr->address[4], addr->address[5]);
        if (NULL != device_p) {
            // bt_gap_bond(addr);
            BT_SINK_CM_STATE_CHANGE(device_p, BT_SINK_CM_STATE_CONNECTING);
            bt_sink_event_send(BT_SINK_EVENT_CM_PROFILE_CONNECT_REQ, (void *)addr);
        } else {
            bt_sink_report("[Sink][CM] Fatal Error");
        }
    } else {
        bt_sink_report("[Sink][CM] Connect, invalid address");
    }
}

void bt_sink_cm_connect(bt_address_t *addr)
{
    bt_sink_cm_request_t request;

    bt_sink_memset((void *)&request, 0, sizeof(bt_sink_cm_request_t));
    request.type = BT_SINK_CM_REQ_TYPE_NORMAL;
    request.attampts = 1;
    bt_sink_memcpy((void *)&request.addr, (void *)addr, sizeof(*addr));
    bt_sink_cm_request_new(&request);
}

void bt_sink_cm_disconnect(bt_address_t *addr)
{
    bt_sink_cm_remote_device_t *device_p =
        bt_sink_cm_find_device(BT_SINK_CM_FIND_BY_ADDR, (void *)addr);

    bt_sink_report("[Sink][CM] Disconnect, device_p:0x%x", device_p);

    if (NULL != device_p) {
        BT_SINK_CM_STATE_CHANGE(device_p , BT_SINK_CM_STATE_DISCONNECTING);
        bt_sink_event_send(BT_SINK_EVENT_CM_PROFILE_DISCONNECT_REQ, (void *)&device_p->conn_id);
    }
}

void bt_sink_cm_link_connected(const bt_address_t *dev_addr, uint16_t conn_id)
{
    bt_sink_cm_remote_device_t *device_p = bt_sink_cm_alloc_device(dev_addr);

    bt_sink_report("[Sink][CM] CM connected conn id = %d", conn_id);

    if (NULL == device_p) {
        bt_sink_report("[Sink][CM] No connection available");
        bt_gap_disconnect(dev_addr, BT_GAP_DISCONNECTION_REASON_REMOTE_USER_TERMINATED);
    } else {
        device_p->conn_id = conn_id;
        sink_conmgr.connected_dev++;
        BT_SINK_CM_STATE_CHANGE(device_p, BT_SINK_STATE_CONNECTED);
        if (BT_SINK_CM_MAX_DEVICE_NUMBER == sink_conmgr.connected_dev
                && (sink_conmgr.flags & BT_SINK_FLAG_PAGE_SCAN)) {
            bt_sink_cm_handle_page_scan_switch();
        }
    }
}

void bt_sink_cm_link_disconnected(const bt_address_t *dev_addr, uint16_t conn_id)
{
    bt_sink_cm_remote_device_t *device_p = bt_sink_cm_find_device(BT_SINK_CM_FIND_BY_ID, &conn_id);

    if (NULL == device_p) {
        bt_sink_report("[Sink][CM] No connection available");
    } else {
        BT_SINK_CM_STATE_CHANGE(device_p, BT_SINK_STATE_DISCONNECTED);
        bt_sink_cm_free_device(device_p);
        sink_conmgr.connected_dev--;

        if (BT_SINK_CM_MAX_DEVICE_NUMBER > sink_conmgr.connected_dev
                && !(sink_conmgr.flags & BT_SINK_FLAG_PAGE_SCAN)) {
            bt_sink_cm_handle_page_scan_switch();
        }
    }
}

uint32_t bt_sink_cm_get_connected_device(uint16_t uuid, uint16_t device_list[BT_SINK_CM_MAX_DEVICE_NUMBER])
{
    uint32_t i, count;

    if (NULL != device_list) {
        bt_sink_memset((void *)device_list, 0, sizeof(uint16_t) * BT_SINK_CM_MAX_DEVICE_NUMBER);
    }

    for (i = 0, count = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; i++) {
        if (BT_SINK_STATE_CONNECTED == sink_conmgr.devices[i].state) {
            if (0 == uuid) {
                if (NULL != device_list) {
                    device_list[count] = sink_conmgr.devices[i].conn_id;
                }
                bt_sink_report("[Sink][CM] connected conn_id:%d", sink_conmgr.devices[i].conn_id);
                count++;
            } else {
                uint32_t j;

                for (j = 0; j < BT_SINK_CM_MAX_PROFILE_NUMBER; j++) {
                    if (NULL != sink_conmgr.devices[i].profile[j]
                            && (sink_conmgr.devices[i].profile[j]->uuid == uuid)
                            && (sink_conmgr.devices[i].profile[j]->state == BT_SINK_PROFILE_STATE_CONNECTED)) {
                        if (NULL != device_list) {
                            device_list[count] = sink_conmgr.devices[i].conn_id;
                        }
                        bt_sink_report("[Sink][CM] connected conn_id:%d", sink_conmgr.devices[i].conn_id);
                        count++;
                    }
                }
            }
        }
    }
    bt_sink_report("[Sink][CM] connected count:%d", count);
    return count;
}

bool bt_sink_cm_get_profile_data(uint16_t conn_id, uint16_t uuid, void *data_p)
{
    bool result = false;
    bt_sink_cm_remote_device_t *device_p = bt_sink_cm_find_device(BT_SINK_CM_FIND_BY_ID, &conn_id);
    bt_sink_cm_dev_info_t *record = bt_sink_memory_alloc(sizeof(bt_sink_cm_dev_info_t));

    if (NULL != device_p && NULL != data_p &&
            bt_sink_db_find_record(&g_bt_sink_cnmgr_db,
                                   (void *)&device_p->addr,
                                   (uint8_t *)record) == BT_SINK_STATUS_SUCCESS) {
        switch (uuid) {
#ifdef BT_SINK_HFP_STORAGE_SIZE
            case BT_SINK_UUID_HF:
                bt_sink_memcpy(data_p, record->hfp_info, BT_SINK_HFP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_A2DP_STORAGE_SIZE
            case BT_SINK_UUID_A2DP:
                bt_sink_memcpy(data_p, record->a2dp_info, BT_SINK_A2DP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_AVRCP_STORAGE_SIZE
            case BT_SINK_UUID_AVRCP:
                bt_sink_memcpy(data_p, record->avrcp_info, BT_SINK_AVRCP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_PBAP_STORAGE_SIZE
            case BT_SINK_UUID_PBAP:
                bt_sink_memcpy(data_p, record->pbap_info, BT_SINK_PBAP_STORAGE_SIZE);
                result = true;
                break;
#endif
            default:
                break;
        }
    }

    bt_sink_memory_free(record);

    return result;
}


bool bt_sink_cm_set_profile_data(uint16_t conn_id, uint16_t uuid, void *data_p)
{
    bool result = false;
    bt_sink_cm_remote_device_t *device_p = bt_sink_cm_find_device(BT_SINK_CM_FIND_BY_ID, &conn_id);
    bt_sink_cm_dev_info_t *record = bt_sink_memory_alloc(sizeof(bt_sink_cm_dev_info_t));

    if (NULL != device_p && NULL != data_p &&
            bt_sink_db_find_record(&g_bt_sink_cnmgr_db,
                                   (void *)&device_p->addr,
                                   (uint8_t *)record) == BT_SINK_STATUS_SUCCESS) {
        switch (uuid) {
#ifdef BT_SINK_HFP_STORAGE_SIZE
            case BT_SINK_UUID_HF:
                bt_sink_memcpy(record->hfp_info, data_p, BT_SINK_HFP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_A2DP_STORAGE_SIZE
            case BT_SINK_UUID_A2DP:
                bt_sink_memcpy(record->a2dp_info, data_p, BT_SINK_A2DP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_AVRCP_STORAGE_SIZE
            case BT_SINK_UUID_AVRCP:
                bt_sink_memcpy(record->avrcp_info, data_p, BT_SINK_AVRCP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_PBAP_STORAGE_SIZE
            case BT_SINK_UUID_PBAP:
                bt_sink_memcpy(record->pbap_info, data_p, BT_SINK_PBAP_STORAGE_SIZE);
                result = true;
                break;
#endif
            default:
                break;
        }
        bt_sink_db_add_record(&g_bt_sink_cnmgr_db,
                              (uint8_t *)record,
                              (void *)&device_p->addr);
    }
    bt_sink_memory_free(record);
    return result;
}

void *bt_sink_cm_find_profile_by_id(uint16_t id, uint16_t uuid)
{
    uint8_t i;
    uint8_t dev_id = BT_SINK_CM_DEVICE_ID_UNAVAILABLE;
    bt_sink_profile_t *profile = NULL;

    // search device
    for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
        if (sink_conmgr.devices[i].conn_id == id) {
            dev_id = i;
            break;
        }
    }
    if (dev_id >= BT_SINK_CM_MAX_DEVICE_NUMBER) {
        bt_sink_report("[Sink][CM] Can not find device\n");
        return NULL;
    }

    // search profile
    for (i = 0; i < BT_SINK_CM_MAX_PROFILE_NUMBER; ++i) {
        if (sink_conmgr.devices[dev_id].profile[i]) {
            profile = sink_conmgr.devices[dev_id].profile[i];
            if (profile->uuid == uuid) {
                return sink_conmgr.devices[dev_id].profile[i];
            }
        }
    }
    return NULL;
}

void *bt_sink_cm_find_profile_by_address(bt_address_t *bt_addr, uint16_t uuid)
{
    uint8_t i;
    uint8_t dev_id = BT_SINK_CM_DEVICE_ID_UNAVAILABLE;
    bt_sink_profile_t *profile = NULL;

    // search device
    for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
        if (bt_sink_cm_is_valid_addr(&sink_conmgr.devices[i].addr)
                && 0 == bt_sink_memcmp((uint8_t *)&sink_conmgr.devices[i].addr, (uint8_t *)bt_addr, sizeof(bt_address_t))) {
            dev_id = i;
            break;
        }
    }
    if (dev_id >= BT_SINK_CM_MAX_DEVICE_NUMBER) {
        bt_sink_report("[Sink][CM] Can not find device\n");
        return NULL;
    }

    // search profile
    for (i = 0; i < BT_SINK_CM_MAX_PROFILE_NUMBER; ++i) {
        if (sink_conmgr.devices[dev_id].profile[i]) {
            profile = sink_conmgr.devices[dev_id].profile[i];
            if (profile->uuid == uuid) {
                return sink_conmgr.devices[dev_id].profile[i];
            }
        }
    }
    return NULL;
}

static void bt_sink_cm_post_callback(bt_sink_event_id_t event_id, bt_sink_status_t result, void *parameters)
{
    switch (event_id) {
        case BT_SINK_EVENT_CM_PROFILE_CONNECT_IND:
            if (NULL != parameters) {
                bt_sink_memory_free(parameters);
            }
            break;

        default:
            break;
    }
}

static void bt_sink_cm_discoverable_timeout(void *parameter)
{
    bt_sink_cm_handle_inquiry_scan_switch();
}

static void bt_sink_cm_link_lost_callback(bt_sink_status_t result, void *parameters)
{
    if (BT_SINK_STATUS_SUCCESS == result) {
        bt_sink_cm_request_cancel(BT_SINK_CM_REQ_TYPE_LINK_LOST);
    }
}

static void bt_sink_cm_power_on_callback(bt_sink_status_t result, void *parameters)
{
    if (sink_conmgr.connected_dev >= BT_SINK_CM_MAX_DEVICE_NUMBER) {
        bt_sink_cm_request_cancel(BT_SINK_CM_REQ_TYPE_POWER_ON);
    }
}

void bt_sink_cm_power_on_reconnection(void)
{
    uint32_t i;
    bt_sink_cm_request_t request;

    for (i = 0; i < g_bt_sink_cnmgr_db.used; i++) {
        if (g_sink_cm_dev_info[i].trusted) {

            bt_sink_memset(&request, 0, sizeof(bt_sink_cm_request_t));
            request.type = BT_SINK_CM_REQ_TYPE_POWER_ON;
            request.attampts = 1;
            request.callback = bt_sink_cm_power_on_callback;
            bt_sink_memcpy(&request.addr, &g_sink_cm_dev_info[i].bdAddr, sizeof(bt_address_t));
            bt_sink_cm_request_new(&request);
        }
    }
}

void bt_sink_cm_init(void)
{
    uint8_t i;

    bt_sink_memset((void *)&sink_conmgr, 0, sizeof(bt_sink_cm_context_t));

    sink_conmgr.request_list.next = &sink_conmgr.request_list;
    sink_conmgr.request_list.prev = &sink_conmgr.request_list;

    // register profiles
    for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
        sink_conmgr.devices[i].profile[BT_SINK_CM_PROFILE_HF] = &bt_sink_hf[i];
        sink_conmgr.devices[i].profile[BT_SINK_CM_PROFILE_A2DP] = &sink_a2dp[i].profile;
        sink_conmgr.devices[i].profile[BT_SINK_CM_PROFILE_AVRCP] = &sink_avrcp[i].profile;
    }

    bt_sink_common_init();

    // init profiles
    bt_sink_event_send(BT_SINK_EVENT_CM_PROFILE_INIT, NULL);

    // init data base
    if (BT_SINK_STATUS_SUCCESS == bt_sink_db_open(&g_bt_sink_cnmgr_db)) {
        for (i = 0; i < BT_SINK_CM_MAX_TRUSTED_DEV; i++) {
            if (g_sink_cm_dev_info[i].trusted) {
                g_bt_sink_cnmgr_db.used++;
            }
        }
    } else {
        bt_sink_report("[Sink][CM] Opend database failed");
    }
    sink_conmgr.flags |= BT_SINK_FLAG_SWITCHING;
    if (BT_STATUS_SUCCESS == bt_gap_power_on()) {
        bt_sink_cm_handle_power_on_cnf();
    }
}

void bt_sink_cm_deinit(void)
{
    bt_sink_timer_stop(BT_SINK_TIMER_CM_REQUEST_DELAY);
    bt_sink_timer_stop(BT_SINK_TIMER_CM_DISCOVERABLE);
    bt_sink_cm_request_cancel(BT_SINK_CM_REQ_TYPE_NONE);
    sink_conmgr.request_pending = NULL;

    sink_conmgr.flags |= BT_SINK_FLAG_SWITCHING;
    if (sink_conmgr.connected_dev > 0) {
        uint32_t i;

        for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; i++) {
            if (sink_conmgr.devices[i].state != BT_SINK_STATE_DISCONNECTED) {
                bt_gap_disconnect(&sink_conmgr.devices[i].addr, BT_GAP_DISCONNECTION_REASON_REMOTE_USER_TERMINATED);
            }
        }
    } else {
        bt_sink_event_send(BT_SINK_EVENT_CM_PROFILE_DEINIT, NULL);
        bt_sink_db_close(&g_bt_sink_cnmgr_db);
        bt_gap_power_off();
    }
}

static void bt_sink_cm_handle_power_on_cnf(void)
{
    sink_conmgr.flags &= (~BT_SINK_FLAG_SWITCHING);
    sink_conmgr.flags |= BT_SINK_FLAG_POWER_ON;

#ifdef __BT_SINK_AUTO_DISCOVERABLE_ONCE__
    bt_sink_cm_handle_inquiry_scan_switch();
#else
    bt_sink_cm_handle_page_scan_switch();
#endif /* __BT_SINK_AUTO_DISCOVERABLE_ONCE__ */

#ifdef __BT_SINK_POWER_ON_RECONNECT__
    bt_sink_cm_power_on_reconnection();
#endif
}

static void bt_sink_cm_handle_io_capability_request_ind(bt_address_t *addr)
{
    bt_gap_reply_io_capability_request(addr, BT_SINK_CM_DB_IO,
                                       BT_SINK_CM_DB_MITM, BT_SINK_CM_DB_MODE, false, NULL, NULL);
}

static void bt_sink_cm_handle_power_switch()
{
    if (!(sink_conmgr.flags & BT_SINK_FLAG_SWITCHING)) {
        if (sink_conmgr.flags & BT_SINK_FLAG_POWER_ON) {
            bt_sink_cm_deinit();
        } else {
            bt_sink_cm_init();
        }
    }
}

static void bt_sink_cm_write_scan_enable_mode(uint8_t mode)
{
    bt_status_t status;

    status = bt_gap_set_scan_mode((bt_gap_mode_t)mode);

    bt_sink_report("[Sink][CM] Scan enable:%x, status:0x%x", mode, status);
}

void bt_sink_cm_handle_page_scan_switch(void)
{
    if (sink_conmgr.flags & BT_SINK_FLAG_PAGE_SCAN) {
        sink_conmgr.flags &= ~BT_SINK_FLAG_PAGE_SCAN;
        bt_sink_cm_write_scan_enable_mode(BT_GAP_MODE_NOT_ACCESSIBLE);
    } else {
        if (BT_SINK_CM_MAX_DEVICE_NUMBER > sink_conmgr.connected_dev) {
            sink_conmgr.flags |= BT_SINK_FLAG_PAGE_SCAN;
            bt_sink_cm_write_scan_enable_mode(BT_GAP_MODE_CONNECTABLE_ONLY);
        }
    }
}

static void bt_sink_cm_handle_inquiry_scan_switch(void)
{
    if (sink_conmgr.flags & BT_SINK_FLAG_PAGE_INQRUIRY_SCAN) {
        bt_sink_cm_write_scan_enable_mode(BT_GAP_MODE_CONNECTABLE_ONLY);
        if (bt_sink_timer_is_exist(BT_SINK_TIMER_CM_DISCOVERABLE)) {
            bt_sink_timer_stop(BT_SINK_TIMER_CM_DISCOVERABLE);
        }
        sink_conmgr.flags &= ~BT_SINK_FLAG_PAGE_INQRUIRY_SCAN;
    } else {
        if (BT_SINK_CM_MAX_DEVICE_NUMBER > sink_conmgr.connected_dev) {
            sink_conmgr.flags |= BT_SINK_FLAG_PAGE_INQRUIRY_SCAN;
            bt_sink_cm_write_scan_enable_mode(BT_GAP_MODE_GENERAL_ACCESSIBLE);
            bt_sink_timer_start(BT_SINK_TIMER_CM_DISCOVERABLE, BT_SINK_CM_DISCOVERABLE_DURATION,
                                bt_sink_cm_discoverable_timeout, NULL);
        }
    }
}

static void bt_sink_cm_handle_detach_single_link(bt_gap_detach_single_link_cnf_t *param)
{
    bt_sink_status_t result = BT_SINK_STATUS_FAIL;

    bt_sink_report("[Sink][CM] detach single link, reason: %x, id = %d", (-param->result), param->conn_id);
    bt_sink_cm_link_disconnected(&param->dev_addr, param->conn_id);

    if (sink_conmgr.flags & BT_SINK_FLAG_AUTHENTICATION_FAIL) {
        // User unbond the device on the SP,
        // link key is not currect, need to delete our link key and retry.
        result = BT_SINK_STATUS_NEED_RETRY;
        sink_conmgr.flags &= (~BT_SINK_FLAG_AUTHENTICATION_FAIL);
    }
    if (sink_conmgr.flags & BT_SINK_FLAG_BONDING_FAIL) {
        sink_conmgr.flags &= (~BT_SINK_FLAG_BONDING_FAIL);
        result = BT_SINK_STATUS_FAIL;
    }

    if (NULL != sink_conmgr.request_pending
            && 0 == bt_sink_memcmp(&param->dev_addr, &sink_conmgr.request_pending->parameters->addr, sizeof(bt_address_t))) {
        bt_sink_cm_request_done(result, sink_conmgr.request_pending);
    }

    if (BT_STATUS_CONNECTION_TIMEOUT == param->result) {
        bt_sink_cm_request_t link_lost;

        bt_sink_memset((void *)&link_lost, 0, sizeof(bt_sink_cm_request_t));
        link_lost.type = BT_SINK_CM_REQ_TYPE_LINK_LOST;
        link_lost.attampts = 3;
        link_lost.delay_time = BT_SINK_LINK_LOST_RECONNECT_DELAY;
        link_lost.callback = bt_sink_cm_link_lost_callback;
        bt_sink_memcpy((void *)&link_lost.addr, (const void *)&param->dev_addr, sizeof(bt_address_t));
        bt_sink_cm_request_new(&link_lost);
    }

    if ((sink_conmgr.flags & BT_SINK_FLAG_SWITCHING) && 0 == sink_conmgr.connected_dev) {
        bt_sink_event_send(BT_SINK_EVENT_CM_PROFILE_DEINIT, NULL);
        bt_sink_db_close(&g_bt_sink_cnmgr_db);
        bt_gap_power_off();
    }
}

static void bt_sink_cm_handle_create_link_cnf(const bt_gap_link_connection_result_t *param)
{
    bt_sink_cm_remote_device_t *device_p
        = bt_sink_cm_find_device(BT_SINK_CM_FIND_BY_ADDR, (void *) &param->dev_addr);

    if (param->result != BT_STATUS_SUCCESS) {
        if (NULL != device_p) {
            BT_SINK_CM_STATE_CHANGE(device_p, BT_SINK_CM_STATE_DISCONNECTED);
        }
        bt_sink_cm_free_device(device_p);
        bt_sink_report("[Sink][CM] Create link failed, %x", -param->result);
        // try reconnection the second-last device
        bt_sink_cm_request_done(BT_SINK_STATUS_FAIL, sink_conmgr.request_pending);
    } else {
        bt_sink_cm_link_connected(&param->dev_addr, param->conn_id);
        if (NULL == device_p) {
            bt_address_t *address = bt_sink_memory_alloc(sizeof(bt_address_t));
            bt_sink_memcpy((void *)address, &param->dev_addr, sizeof(bt_address_t));
            bt_sink_event_post(BT_SINK_EVENT_CM_PROFILE_CONNECT_IND, address, bt_sink_cm_post_callback);
        }
    }
}

static void bt_sink_cm_handle_bonding_result_ind(const bt_gap_bond_result_ind_t *param)
{
    bt_sink_cm_dev_info_t *record = bt_sink_memory_alloc(sizeof(bt_sink_cm_dev_info_t));

    if (sink_conmgr.flags & BT_SINK_FLAG_PAGE_INQRUIRY_SCAN) {
        bt_sink_cm_handle_inquiry_scan_switch();
    }

    if (param->result != BT_STATUS_SUCCESS) {
        bt_sink_report("[Sink][CM] Bonding result ind failed, param->result:-0x%x", -param->result);
        sink_conmgr.flags |= BT_SINK_FLAG_BONDING_FAIL;
    } else {
        bt_sink_memset((void *)record, 0, sizeof(bt_sink_cm_dev_info_t));
        bt_sink_memcpy((void *)&record->bdAddr, (void *)&param->dev_addr, sizeof(bt_address_t));
        bt_sink_memcpy((void *)record->linkKey, (void *)param->link_key, BT_GAP_LINK_KEY_LEN);
#ifdef __BT_SINK_DEBUG_INFO__
        bt_sink_cm_dump_link_key((uint8_t *)param->link_key);
#endif /* __BT_SINK_DEBUG_INFO__ */
        record->trusted = true;
        record->keyType = param->key_type;
        bt_sink_db_add_record(&g_bt_sink_cnmgr_db, (uint8_t *)record, &param->dev_addr);
    }
    bt_sink_memory_free(record);
}

static void bt_sink_cm_handle_authentication_ind(const bt_gap_authenticate_result_ind_t *param)
{
    if (param->result != BT_STATUS_SUCCESS) {
        bt_sink_cm_dev_info_t *record = bt_sink_memory_alloc(sizeof(bt_sink_cm_dev_info_t));

        bt_sink_report("[Sink][CM] Authentication failed");
        if (bt_sink_db_find_record(&g_bt_sink_cnmgr_db,
                                   (void *)&param->dev_addr,
                                   (uint8_t *)record) == BT_SINK_STATUS_SUCCESS) {
            bt_sink_memset(record->linkKey, 0, BT_GAP_LINK_KEY_LEN);
            bt_sink_db_add_record(&g_bt_sink_cnmgr_db, (uint8_t *)record, &param->dev_addr);
            sink_conmgr.flags |= BT_SINK_FLAG_AUTHENTICATION_FAIL;
        }
        bt_sink_memory_free(record);
    } else if (NULL != sink_conmgr.request_pending) {
        bt_sink_cm_request_done(BT_SINK_STATUS_SUCCESS, sink_conmgr.request_pending);
    }
}

bt_sink_status_t bt_sink_cm_event_handler(bt_sink_event_id_t event, void *param)
{
    bt_sink_report("[Sink][CM] bt_sink_cm_event_handler, event:%x", event);

    switch (event) {
        case BT_SINK_EVENT_CM_POWER_SWITCH:
            bt_sink_cm_handle_power_switch();
            break;

        case BT_SINK_EVENT_CM_CONNECT:
            bt_sink_cm_connect((bt_address_t *)param);
            break;

        case BT_SINK_EVENT_CM_DISCONNECT:
            bt_sink_cm_disconnect((bt_address_t *)param);
            break;

        case BT_SINK_EVENT_CM_CONNECTABLE:
            bt_sink_cm_handle_page_scan_switch();
            break;

        case BT_SINK_EVENT_CM_DISCOVERABLE:
            bt_sink_cm_handle_inquiry_scan_switch();
            break;

        default:
            break;
    }

    return BT_SINK_STATUS_SUCCESS;
}


void bt_sink_cm_gap_callback(bt_event_t event_id, void *param)
{
    bt_sink_report("[Sink][CM] bt_sink_cm_gap_callback, event_id:0x%x", event_id);

#ifdef __BT_SINK_DEBUG_INFO__
    if (event_id >= BT_GAP_DISCOVERY_RESULT_IND && event_id <= BT_GAP_AUTHENTICATE_RESULT_IND) {
        bt_sink_report("[Sink][CM] SDK event:%s", g_gap_event_string[event_id - BT_GAP_DISCOVERY_RESULT_IND]);
    } else {
        bt_sink_report("[Sink][CM] SDK event:0x%x", event_id);
    }
#else
    bt_sink_report("[Sink][CM] SDK event:0x%x", event_id);
#endif /* __BT_SINK_DEBUG_INFO__ */

    switch (event_id) {
        case BT_GAP_IO_CAPABILITY_REQUEST_IND:
            bt_sink_cm_handle_io_capability_request_ind((bt_address_t *)param);
            break;
        case BT_GAP_BONDING_CNF:
            /* could do sdp here after bonding successfully if need. */
            break;
        case BT_GAP_BONDING_RESULT_IND:
            bt_sink_cm_handle_bonding_result_ind((bt_gap_bond_result_ind_t *)param);
            break;
        case BT_GAP_USER_CONFIRM_REQUEST_IND:
            bt_gap_reply_user_confirm_request((bt_address_t *)param, true);
            break;
        case BT_GAP_SET_SCAN_MODE_CNF: {
            bt_gap_set_scan_mode_cnf_t *scan_result = (bt_gap_set_scan_mode_cnf_t *)param;
            bt_sink_report("[Sink][CM] scan_result:0x%x", *scan_result);
        }
        break;
        case BT_GAP_CREATE_LINK_CNF:
            bt_sink_cm_handle_create_link_cnf((bt_gap_link_connection_result_t *)param);
            break;
        case BT_GAP_POWER_ON_CNF:
            bt_sink_cm_handle_power_on_cnf();
            break;
        case BT_GAP_POWER_OFF_CNF:
            sink_conmgr.flags &= (~BT_SINK_FLAG_SWITCHING);
            sink_conmgr.flags &= (~BT_SINK_FLAG_POWER_ON);
            break;
        case BT_GAP_DETACH_SINGLE_LINK_CNF:
            bt_sink_cm_handle_detach_single_link((bt_gap_detach_single_link_cnf_t *)param);
            break;

        case BT_GAP_AUTHENTICATE_RESULT_IND:
            bt_sink_cm_handle_authentication_ind((bt_gap_authenticate_result_ind_t *)param);
            break;

        default:
            bt_sink_report("[Sink][CM] Unexcepted event:%x", event_id);
            break;
    }
    return;
}

bt_status_t bt_sink_cm_get_local_name(char *name)
{
    bt_sink_strcpy(name, BT_SINK_CM_DB_NAME);
    return BT_STATUS_SUCCESS;
}

uint32_t bt_sink_cm_get_local_cod(void)
{
    return BT_SINK_CM_COD;
}

bt_status_t bt_sink_cm_request_link_key(const bt_address_t *remote_addr, uint8_t *key, uint32_t key_len)
{
    bt_status_t result = BT_STATUS_PIN_OR_KEY_MISSING;
    bt_sink_cm_dev_info_t *record = bt_sink_memory_alloc(sizeof(bt_sink_cm_dev_info_t));

    bt_sink_report("[Sink][CM] bt_gap_request_link_key");

    if (bt_sink_db_find_record(&g_bt_sink_cnmgr_db,
                               (void *)remote_addr,
                               (uint8_t *)record) == BT_SINK_STATUS_SUCCESS) {
        uint8_t empty[BT_GAP_LINK_KEY_LEN] = {0};

        if (0 != bt_sink_memcmp(empty, record->linkKey, BT_GAP_LINK_KEY_LEN)) {
            bt_sink_memcpy((void *)key, (void *)record->linkKey, BT_GAP_LINK_KEY_LEN);
#ifdef __BT_SINK_DEBUG_INFO__
            bt_sink_cm_dump_link_key(key);
#endif /* __BT_SINK_DEBUG_INFO__ */
            result = BT_STATUS_SUCCESS;
        }
    }
    bt_sink_memory_free(record);
    return result;
}

