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
 * The file is used for testing BT notification..
 *
 ****************************************************************************/

#include "bt_notify.h"
#include "queue.h"
#include "task.h"
#include "bt_events.h"
#include "bt_gap.h"
#include "ble_dogp_adp_service.h"

#include "syslog.h"
log_create_module(NOTIFY_APP, PRINT_LEVEL_INFO);

#define APPHEADER "app_test apk 0 "  /*format:'sender  receiver datatype '*/ /*app_test apk 0 strlen(data_string) data_string*/

#define APP_RECEIVE_BUF_SIZE  2*1024
typedef struct {
    int32_t write_len;
    int32_t write_total_len;
    uint8_t write_buffer[APP_RECEIVE_BUF_SIZE];
    bool is_write_done;
    int32_t read_len;
    int32_t read_total_len;
    uint8_t read_buffer[APP_RECEIVE_BUF_SIZE];
} bt_notify_test_cntx_t;

bt_notify_test_cntx_t g_text_cntx;

/*****************************************************************************
 * define
 *****************************************************************************/
#define APP_QUEUE_SIZE      2000

/*****************************************************************************
 * typedef
 *****************************************************************************/
typedef struct {
    bt_event_t event_id;
    void *data_ptr;
} app_queue_struct_t;

/*****************************************************************************
 * global variable
 *****************************************************************************/
/*bt address to store remote bt address */
bt_address_t g_bt_addr;

QueueHandle_t app_queue = NULL;

void bt_notify_callback_func(bt_event_t event_id, const void *parameter)
{
    LOG_I(NOTIFY_APP, "\r\n[App test]task,evt_id = %d\r\n", event_id);
    app_queue_struct_t queue_item;
    queue_item.event_id = (bt_event_t)event_id;
    queue_item.data_ptr = NULL;

    switch (event_id) {
        case BT_SPP_CONNECT_CNF: { /*5377*/
            queue_item.data_ptr = (bt_spp_connect_cnf_t *)pvPortMalloc(sizeof(bt_spp_connect_cnf_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_connect_cnf_t));
            break;
        }
        case BT_SPP_CONNECT_IND: { /*5376*/
            queue_item.data_ptr = (bt_spp_connect_ind_t *)pvPortMalloc(sizeof(bt_spp_connect_ind_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_connect_ind_t));
            break;
        }
        case BT_SPP_DISCONNECT_IND: { /*5378*/
            queue_item.data_ptr = (bt_spp_disconnect_ind_t *)pvPortMalloc(sizeof(bt_spp_disconnect_ind_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_disconnect_ind_t));
            break;
        }
        case BT_SPP_READY_TO_READ_IND: { /*5379*/
            queue_item.data_ptr = (bt_spp_ready_to_read_ind_t *)pvPortMalloc(sizeof(bt_spp_ready_to_read_ind_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_ready_to_read_ind_t));
            break;
        }
        case BT_SPP_READY_TO_WRITE_IND: { /*5380*/
            queue_item.data_ptr = (bt_spp_ready_to_write_ind_t *)pvPortMalloc(sizeof(bt_spp_ready_to_write_ind_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_ready_to_write_ind_t));
            break;
        }

        case BLE_DOGP_ADP_EVENT_CONNECT_IND: { /*0*/
            queue_item.data_ptr = (ble_dogp_adp_connect_t *)pvPortMalloc(sizeof(ble_dogp_adp_connect_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(ble_dogp_adp_connect_t));
            break;
        }
        case BLE_DOGP_ADP_EVENT_DISCONNECT_IND: { /*1*/
            queue_item.data_ptr = (ble_dogp_adp_disconnect_t *)pvPortMalloc(sizeof(ble_dogp_adp_disconnect_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(ble_dogp_adp_disconnect_t));
            break;
        }
        case BLE_DOGP_ADP_EVENT_READY_TO_READ_IND: { /*2*/
            queue_item.data_ptr = (ble_dogp_adp_ready_to_read_t *)pvPortMalloc(sizeof(ble_dogp_adp_ready_to_read_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(ble_dogp_adp_ready_to_read_t));
            break;
        }
        case BLE_DOGP_ADP_EVENT_READY_TO_WRITE_IND: { /*3*/
            queue_item.data_ptr = (ble_dogp_adp_ready_to_write_t *)pvPortMalloc(sizeof(ble_dogp_adp_ready_to_write_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(ble_dogp_adp_ready_to_write_t));
            break;
        }
        default:
            break;
    }

    if (xQueueSend(app_queue, (void *)&queue_item, 0) != pdPASS) {
        LOG_I(NOTIFY_APP, "\n[App test]QUEUE_FULL\n");
    }

}

void bt_notify_callback_func_hdlr(uint32_t event_id, void *parameter)
{

    LOG_I(NOTIFY_APP, "\n[App test]task run! evt = %d\r\n", event_id);

    switch (event_id) {
        case BT_SPP_CONNECT_CNF: {
            bt_notify_handle_connect_ind(parameter, BT_NOTIFY_DATA_SOURCE_SPP);
            break;
        }
        case BT_SPP_CONNECT_IND: {
            bt_spp_connect_ind_t *para = (bt_spp_connect_ind_t *)parameter;
            bt_notify_spp_connect_ind(para);
            break;
        }
        case BT_SPP_DISCONNECT_IND: {
            bt_notify_handle_disconnect_ind(parameter, BT_NOTIFY_DATA_SOURCE_SPP);
            break;
        }
        case BT_SPP_READY_TO_READ_IND: {
            bt_notify_handle_ready_to_read_ind(parameter, BT_NOTIFY_DATA_SOURCE_SPP);
            break;
        }
        case BT_SPP_READY_TO_WRITE_IND: {
            bt_notify_handle_ready_to_write_ind(parameter, BT_NOTIFY_DATA_SOURCE_SPP);
            break;
        }
        case BLE_DOGP_ADP_EVENT_CONNECT_IND: {
            bt_notify_handle_connect_ind(parameter, BT_NOTIFY_DATA_SOURCE_DOGP);
            break;
        }
        case BLE_DOGP_ADP_EVENT_DISCONNECT_IND: {
            bt_notify_handle_disconnect_ind(parameter, BT_NOTIFY_DATA_SOURCE_DOGP);
            break;
        }
        case BLE_DOGP_ADP_EVENT_READY_TO_READ_IND: {
            bt_notify_handle_ready_to_read_ind(parameter, BT_NOTIFY_DATA_SOURCE_DOGP);
            break;
        }
        case BLE_DOGP_ADP_EVENT_READY_TO_WRITE_IND: {
            bt_notify_handle_ready_to_write_ind(parameter, BT_NOTIFY_DATA_SOURCE_DOGP);
            break;
        }
        default:
            break;
    }
    if (parameter) {
        vPortFree(parameter);
    }

}

/*****************************************************************************
 * FUNCTION
 *  app_task
 * DESCRIPTION
 *  app task implementation
 * PARAMETERS
 *  arg        :[IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_notify_test_task(void *arg)
{
    app_queue_struct_t queue_data_item;
    LOG_I(NOTIFY_APP, "[App test]App test task begin\r\n");

    app_queue = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_queue_struct_t));
    if ( app_queue == NULL ) {
        LOG_I(NOTIFY_APP, "[App test]create queue failed!\r\n");
        return;
    }

    /*main loop*/
    while (1) {
        if (xQueueReceive(app_queue, (void *)&queue_data_item, portMAX_DELAY)) {
            /*  LOG_I(NOTIFY_APP, "[App test]handle one message id = %d\r\n", queue_data_item.event_id);*/
            bt_notify_callback_func_hdlr(queue_data_item.event_id, queue_data_item.data_ptr);
        }
    }

}
/*****************************************************************************
 * FUNCTION
 *  bt_notify_test_callback_hdlr
 * DESCRIPTION
 *  handle event received from remote end
 * PARAMETERS
 *  data      :[IN] event data
 * RETURNS
 *  void
 *****************************************************************************/
void bt_notify_test_callback_hdlr(void *data)
{
    bt_noti_callback_t *p_data = (bt_noti_callback_t *)data;

    bt_notify_test_cntx_t *text_cntx = &g_text_cntx;

    LOG_I(NOTIFY_APP, "[App test]callback evt_id = %d!\r\n", p_data->evt_id);
    switch (p_data->evt_id) {
        case BT_NOTIFY_EVENT_CONNECTION: {
            memcpy(g_bt_addr.address, p_data->bt_addr.address, 6);
            /*connected with the remote device*/
        }
        break;
        case BT_NOTIFY_EVENT_DISCONNECTION: {
            memset(g_bt_addr.address, 0, 6);
            /*disconnected with the remote device*/
        }
        break;
        case BT_NOTIFY_EVENT_READY_TO_READ: {
            /*read data flow start*/
            bt_notify_read_data(&g_bt_addr);
        }
        break;
        case BT_NOTIFY_EVENT_READY_TO_WRITE: {
            /*send  new/the rest data flow start*/
            int32_t send_size;
            send_size = bt_notify_send_data(&g_bt_addr, (const char*)(text_cntx->write_buffer - text_cntx->write_len), strlen((const char*)(text_cntx->write_buffer - text_cntx->write_len)), false);
            text_cntx->write_len += send_size;
            if (text_cntx->write_len >= text_cntx->write_total_len) {
                text_cntx->is_write_done = true;
            } else {
                text_cntx->is_write_done = false;
            }

            LOG_I(NOTIFY_APP, "[App test]1 = %d,2 = %d,3 = %d\r\n", text_cntx->write_total_len, text_cntx->write_len, send_size);

        }
        break;
        case BT_NOTIFY_EVENT_DATA: {
            char len[5] = {0};
            /*receive data*/
            LOG_I(NOTIFY_APP, "[App test] data = %s, len = %d\r\n", p_data->noti_data.data, p_data->noti_data.len);
            strcat((char *)text_cntx->read_buffer, (const char*)p_data->noti_data.data);
            text_cntx->read_total_len += strlen((const char*)p_data->noti_data.data);
            if (text_cntx->read_total_len < p_data->noti_data.len) {
                LOG_I(NOTIFY_APP, "[App test]no-read-done!\r\n");
            } else {

                if (text_cntx->is_write_done == true) {
                    memcpy(text_cntx->write_buffer, APPHEADER, strlen(APPHEADER) + 1);
                    sprintf(len, "%d ", (int)text_cntx->read_total_len);
                    strcat((char *)text_cntx->write_buffer, len);
                    strcat((char *)text_cntx->write_buffer, (const char*)text_cntx->read_buffer);

                    LOG_I(NOTIFY_APP, "[App test]write_buffer = %s\n", text_cntx->write_buffer);
                    text_cntx->write_len  = bt_notify_send_data(&g_bt_addr, (const char*)text_cntx->write_buffer, strlen((const char*)text_cntx->write_buffer), true);

                    LOG_I(NOTIFY_APP, "[App test]re-send data = %s\r\n", p_data->noti_data.data);
                    memcpy(text_cntx->write_buffer, text_cntx->read_buffer, APP_RECEIVE_BUF_SIZE);
                    text_cntx->write_total_len = text_cntx->read_total_len;
                    memset(text_cntx->read_buffer, 0, APP_RECEIVE_BUF_SIZE);
                    text_cntx->read_total_len = 0;

                }

            }

        }
        break;
        case BT_NOTIFY_EVENT_NOTIFICATION: {
            /*receive a notification*/
            uint32_t i = 0, j = 0;
            bt_notify_noti_t *noti = &p_data->notification;

            LOG_I(NOTIFY_APP, "********Noti_S,UCS2******************\r\n");
            LOG_I(NOTIFY_APP, "notification id: %d\r\n", noti->msg_id);
            LOG_I(NOTIFY_APP, "action: %d\r\n", noti->action);
            if (noti->action != BT_NOTIFY_NOTIFICATION_DELETE) {
                LOG_I(NOTIFY_APP, "sender: %s\r\n", noti->sender);
                LOG_I(NOTIFY_APP, "timestamp: %d\r\n", noti->timestamp);
                LOG_I(NOTIFY_APP, "app id: %d\r\n", noti->app_id);
#ifdef _ICON_SUPPORT_
                LOG_I(NOTIFY_APP, "icon buffer: %s\r\n", noti->icon->icon_buffer);
                LOG_I(NOTIFY_APP, "action size: %d\r\n", noti->icon->icon_size);
#endif /*_ICON_SUPPORT_*/

                LOG_I(NOTIFY_APP, "page num: %d\r\n", noti->page_num);
                {
                    bt_notify_page_content_t *page_content;

                    for (i = noti->page_num; i > 0; i--) {
                        page_content = noti->page_content;
                        for (j = 0; j < i - 1; j++) {
                            page_content = page_content->next;
                        }

                        LOG_I(NOTIFY_APP, "content: %s\r\n", page_content->content);
                        LOG_I(NOTIFY_APP, "title: %s\r\n", page_content->title);

                    }
                }

                LOG_I(NOTIFY_APP, "group id: %s\r\n", noti->group_id);
                LOG_I(NOTIFY_APP, "action number: %d\r\n", noti->action_number);
                for (i = 0; i < noti->action_number; i++) {
                    LOG_I(NOTIFY_APP, "action name: %s\r\n", noti->noti_action[i].action_name);
                    LOG_I(NOTIFY_APP, "action id: %s\r\n", noti->noti_action[i].action_id);
                }

            }

            LOG_I(NOTIFY_APP, "********Noti_E******************\r\n");
        }
        break;
        case BT_NOTIFY_EVENT_MISSED_CALL: {

            /*receive a missed call*/
            bt_notify_call_t *call = &p_data->missed_call;

            LOG_I(NOTIFY_APP, "********MissedCall_S******************\r\n");
            LOG_I(NOTIFY_APP, "sender: %s\r\n", call->sender);
            LOG_I(NOTIFY_APP, "send number: %s\r\n", call->send_number);
            LOG_I(NOTIFY_APP, "missed call count: %d\r\n", call->missed_call_count);
            LOG_I(NOTIFY_APP, "notification id: %d\r\n", call->msg_id);
            LOG_I(NOTIFY_APP, "timestamp: %d\r\n", call->timestamp);
            LOG_I(NOTIFY_APP, "********MissedCall_E******************\r\n");

        }
        break;
        case BT_NOTIFY_EVENT_SMS: {
            /*receive a SMS*/
            bt_notify_sms_t *sms =  &p_data->sms;

            LOG_I(NOTIFY_APP, "********SMS_S UCS2******************\r\n");
            LOG_I(NOTIFY_APP, "sender: %s\r\n", sms->sender);
            LOG_I(NOTIFY_APP, "send number: %s\r\n", sms->send_number);
            LOG_I(NOTIFY_APP, "page content: %s\r\n", sms->page_content->content);
            LOG_I(NOTIFY_APP, "page title: %s\r\n", sms->page_content->title);
            LOG_I(NOTIFY_APP, "notification id: %d\r\n", sms->msg_id);
            LOG_I(NOTIFY_APP, "timestamp: %d\r\n", sms->timestamp);
            LOG_I(NOTIFY_APP, "********SMS_E******************\r\n");

        }
        break;
        default:
            break;
    }
}
void bt_dogp_notify_callback_func(ble_dogp_adp_event_t event, void *callback_param)
{
    bt_notify_callback_func(event, callback_param);
}

void bt_notify_test_init(void)
{
    uint8_t spp_uuid[16] = {0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFF};
    memset(&g_text_cntx, 0, sizeof(bt_notify_test_cntx_t));
    g_text_cntx.is_write_done = true;
    bt_notify_init();
    bt_spp_start_service(spp_uuid, (const int8_t*)"BT_NOTIFY", bt_notify_callback_func);
    ble_dogp_adp_init(bt_dogp_notify_callback_func);

    if (BT_NOTIFY_REGISTER_RET_OK !=  bt_notify_register_callback(NULL, "app_test", bt_notify_test_callback_hdlr)) {
        return;
    }
}
void bt_notify_test_task_init(void)
{
    TaskHandle_t xCreatedTask;
    LOG_I(NOTIFY_APP, "[App test]create task!\r\n");
    xTaskCreate(bt_notify_test_task, "bt_notify_test_task", 2048, NULL, 1, &xCreatedTask);
    bt_notify_test_init();
}





