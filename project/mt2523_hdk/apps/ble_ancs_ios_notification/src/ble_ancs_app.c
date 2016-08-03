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

#include "ble_ancs_app.h"
#include "string.h"
#include "ble_gap.h"
#include "ble_gatt.h"
#include "ble_sm.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "nvdm.h"
#include "ble_sm.h"
#include "ble_gattc_connect.h"

#define ANCS_QUEUE_SIZE 50      /*Adjust it as you need. If it is too small ,notificaitons may lost at the time of connected*/

ble_ancs_app_cntx_t ancs_app_cntx;
ble_ancs_app_cntx_t *p_ancs_app = &ancs_app_cntx;

QueueHandle_t ancs_queue = NULL;

/*****************************************************************************
* FUNCTION
*  ble_ancs_remove_from_notification_list
* DESCRIPTION
* Remove the notification from the link list.
* PARAMETERS
*  notif            [IN]        The notification node to be removed
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_remove_from_notification_list(ble_ancs_notification_node_t *notif)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_ancs_notification_node_t *p_notif;
    ble_ancs_notification_node_t *p_pre = NULL;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    p_notif = &p_ancs_app->notif_list;

    while (p_notif && p_notif != notif) {
        p_pre = p_notif;
        p_notif = p_notif->next;
    }

    if (p_pre && p_notif) {
        p_pre->next = p_notif->next;
#ifndef WIN32
        vPortFree(p_notif);
#else
        free(p_notif);
#endif
        p_ancs_app->notif_num--;
    }
    return;
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_add_to_notification_list
* DESCRIPTION
* Add one notification to the link list.
* PARAMETERS
*  conn_id        [IN]        Connection id
*  notif            [IN]        The notification node to be added to list
* RETURNS
*  ancs_notification_node_t*
*****************************************************************************/
ble_ancs_notification_node_t *ble_ancs_add_to_notification_list(uint16_t conn_id, ble_ancs_event_notification_t *notif)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_ancs_notification_node_t *p_notif;
    ble_ancs_notification_node_t *new_node;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /*LOG_I(ANCS, "[ANCSapp]\r\n");*/

    p_notif = &p_ancs_app->notif_list;

    /*look for the tail of the link list*/
    while (p_notif->next != NULL) {
        p_notif = p_notif->next;
    }

#ifndef WIN32
    new_node = (ble_ancs_notification_node_t *)pvPortMalloc(sizeof(ble_ancs_notification_node_t));
#else
    new_node = (ble_ancs_notification_node_t *)malloc(sizeof(ble_ancs_notification_node_t));
#endif
    if (new_node) {
        new_node->notif.conn_id = conn_id;
        memcpy(new_node->notif.notif_uid, notif->notification_uid, 4);
        p_notif->next = new_node;
        new_node->next = NULL;
        p_ancs_app->notif_num++;
    } else {
        LOG_I(ANCS, "[ANCSapp]Allocate buffer fail!\r\n");
    }

    return new_node;
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_get_notification_information
* DESCRIPTION
* Get the notificaiton attributes.
* PARAMETERS
*
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_get_notification_information()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(ANCS, "[ANCSapp]notif_num = %d, curr_notif = 0x%x\r\n", p_ancs_app->notif_num, p_ancs_app->curr_notif);

    if (p_ancs_app->notif_num > 0 && p_ancs_app->curr_notif == NULL) {
        /*There is no notification in parsing*/
        ble_ancs_attribute_list_t attr_list[5];
        uint8_t attr_num;

        attr_num = 5;

        p_ancs_app->curr_notif = p_ancs_app->notif_list.next;

        /*If the buffer is not enough, the data will be truncated*/
        attr_list[0].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER;
        attr_list[0].atrribute_len = BLE_ANCS_APP_ID_LEN;
        attr_list[0].data = p_ancs_app->appid;

        attr_list[1].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE;
        attr_list[1].atrribute_len = BLE_ANCS_TITLE_LEN;
        attr_list[1].data = p_ancs_app->title;

        attr_list[2].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE;
        attr_list[2].atrribute_len = BLE_ANCS_MESSAGE_LEN;
        attr_list[2].data = p_ancs_app->message;

        /*WARNING: You can get positive/negative action label only when connected with IOS8.0 or later*/
        attr_list[3].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_POSITIVE_ACTION_LABEL;
        attr_list[3].atrribute_len = BLE_ANCS_LAB_LEN;
        attr_list[3].data = p_ancs_app->posi_lab;

        attr_list[4].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_NEGATIVE_ACTION_LABEL;
        attr_list[4].atrribute_len = BLE_ANCS_LAB_LEN;
        attr_list[4].data = p_ancs_app->nega_lab;

        ble_ancs_get_notification_attributes(p_ancs_app->curr_notif->notif.conn_id, p_ancs_app->curr_notif->notif.notif_uid,
                                             attr_list, attr_num);
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_get_app_information
* DESCRIPTION
* Get the app attributes.
* PARAMETERS
*  attr        [IN]        Pointer to notificaiton attribute
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_get_app_information(ble_ancs_event_attribute_t *attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_ancs_attribute_list_t attr_list[1];
    uint8_t attr_num;
    uint16_t appid_len = 0;
    uint8_t i;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /*Get app attribute*/
    for (i = 0; i < attr->attr_num; i++) {
        if (attr->attr_list[i].attribute_id == BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER) {
            appid_len = attr->attr_list[i].atrribute_len;
            break;
        }
    }
    if (appid_len > 0 && appid_len < BLE_ANCS_APP_ID_LEN) {
        /*[warning]if appid_len > ANCS_APP_ID_LEN, appid is truncated, you can't get the right app attribute*/
        attr_num = 1;
        attr_list[0].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER;
        attr_list[0].atrribute_len = BLE_ANCS_APP_NAME_LEN;
        attr_list[0].data = p_ancs_app->app_name;

        ble_ancs_get_app_attributes(p_ancs_app->curr_notif->notif.conn_id, attr->attr_list[i].data,
                                    appid_len, attr_list, attr_num);
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_show_attribute
* DESCRIPTION
* Show the detail information.
* PARAMETERS
*  attr        [IN]        Pointer to notificaiton attribute or app attribute
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_show_attribute(ble_ancs_event_attribute_t *attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t i;
    char *notif_attr_name[ANCS_NOTIFICATION_ATTR_NUM] = {
        "App Identifier",
        "Title",
        "Subtitle",
        "Message",
        "Message Size",
        "Date",
        "Positive Action Label",
        "Negative Action Label"
    };
    char *app_attr_name[1] = {
        "Display Name"
    };
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //LOG_I(ANCS, "[ANCSapp]*********************************************\r\n");
    //LOG_I(ANCS, "[ANCSapp]**********Show Attribute Start**********\r\n");

    if (attr->command_id == 0) {
        /*Show notification attribute*/
        for (i = 0; i < attr->attr_num; i++) {
            if (attr->attr_list[i].atrribute_len > 0) {
                LOG_I(ANCS, "*****Notification attribute*****%s : %s\r\n", notif_attr_name[attr->attr_list[i].attribute_id], attr->attr_list[i].data);
            }
        }
    } else if (attr->command_id == 1) {
        /*Show app attribute*/
        for (i = 0; i < attr->attr_num; i++) {
            if (attr->attr_list[i].atrribute_len > 0) {
                LOG_I(ANCS, "*****App attribute*****%s : %s\r\n", app_attr_name[attr->attr_list[i].attribute_id], attr->attr_list[i].data);
            }
        }
    }

    //LOG_I(ANCS, "[ANCSapp]**********Show Attribute End**********\r\n");
    //LOG_I(ANCS, "[ANCSapp]*********************************************\r\n");
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_show_notification
* DESCRIPTION
* Show notifications ,it is received first.
* PARAMETERS
*  attr        [IN]        Pointer to notificaiton source
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_show_notification(ble_ancs_event_notification_t *notif)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *eventid[ANCS_EVENT_ID_NUM] = {
        "Added",
        "Modified",
        "Removed"
    };
    char *catid[ANCS_CATEGORY_ID_NUM] = {
        "Other",
        "Incoming Call",
        "Missed Call",
        "Voice Mail",
        "Social",
        "Schedule",
        "Email",
        "News",
        "Health And Fitness",
        "Business And Finance",
        "Location",
        "Entertainment"
    };
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /*LOG_I(ANCS, "[ANCSapp]*********************************************\r\n");
    LOG_I(ANCS, "[ANCSapp]**********Show Notification Start**********\r\n");
    LOG_I(ANCS, "[ANCSapp]Event id: %s\r\n", eventid[notif->event_id]);
    LOG_I(ANCS, "[ANCSapp]Event flags: 0x%x\r\n", notif->event_flags);
    LOG_I(ANCS, "[ANCSapp]Category id: %s\r\n", catid[notif->category_id]);
    LOG_I(ANCS, "[ANCSapp]Category count: %d\r\n", notif->category_count);
    LOG_I(ANCS, "[ANCSapp]Category uuid: %d%d%d%d\r\n", notif->notification_uid[0], notif->notification_uid[1],notif->notification_uid[2],notif->notification_uid[3]);
    LOG_I(ANCS, "[ANCSapp]**********Show Notification End**********\r\n");
    LOG_I(ANCS, "[ANCSapp]*********************************************\r\n");*/
    LOG_I(ANCS, "*****NOTIFICATION*****Event_id = %s, Event_flags = 0x%x, Cat_id = %s, Cat_count = %d, Cat_uuid = %d%d%d%d\r\n",
          eventid[notif->event_id], notif->event_flags, catid[notif->category_id], notif->category_count,
          notif->notification_uid[0], notif->notification_uid[1], notif->notification_uid[2], notif->notification_uid[3]);
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_deal_with_new_notification
* DESCRIPTION
* Deal with the newly received notificaitons
* PARAMETERS
*  conn_id        [IN]        Connection id
*  notif            [IN]        Pointer to notificaiton source
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_deal_with_new_notification(uint16_t conn_id, ble_ancs_event_notification_t *notif)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_ancs_notification_node_t *p_notif;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(ANCS, "[ANCSapp]event_id = %d, notif_num = %d\r\n", notif->event_id,  p_ancs_app->notif_num);

    if (notif->event_id == BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED) {
        p_notif = p_ancs_app->notif_list.next;
        while (p_notif && (p_notif->notif.conn_id != conn_id ||
                           memcmp(p_notif->notif.notif_uid, notif->notification_uid, 4))) {
            p_notif = p_notif->next;
        }
        if (p_notif) {
            if (p_notif == p_ancs_app->new_notif) {
                p_ancs_app->new_notif = NULL;
                LOG_I(ANCS, "[ANCSapp]new notification is removed, can't perform action on it!\r\n");
            }
            ble_ancs_remove_from_notification_list(p_notif);
        }
    } else if (notif->event_id == BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED ||
               notif->event_id == BLE_ANCS_EVENT_ID_NOTIFICATION_MODIFIED) {
        if (p_ancs_app->notif_num == BLE_ANCS_MAX_NOTIF_NUM) {
            p_notif = p_ancs_app->notif_list.next;

            if (p_notif && p_notif == p_ancs_app->curr_notif) {
                p_notif = p_notif->next;
            }

            if (p_notif) {
                ble_ancs_remove_from_notification_list(p_notif);
            }
        }

        p_ancs_app->new_notif = ble_ancs_add_to_notification_list(conn_id, notif);

        ble_ancs_get_notification_information();
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_deal_with_write_fail
* DESCRIPTION
* if write descriptor fail because of unautherised, bond.
* PARAMETERS
*  conn_cntx                 [IN]        Connection context
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_deal_with_write_fail(ble_address_t remote_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_status_t status;
    ble_sm_pairing_attribute_t local_attributes = {true, true, false};/*mitm, bond, oob*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /*LOG_I(ANCS, "[ANCSapp]sm bond");*/

    ble_sm_set_pairing_authorization(true);
    status = ble_sm_bond(&remote_addr, &local_attributes);

    if (status != BLE_STATUS_SUCCESS && status != BLE_STATUS_GAP_PENDING) {
        LOG_I(ANCS, "[ANCSapp]Bond fail!!! status = %d\r\n", status);
    }
}


void ble_ancs_update_connection_interval(uint16_t conn_id, ble_ancs_conn_speed_t conn_speed)
{
    ble_gap_connection_params_t conn_params;

    conn_params.conn_timeout = 0x0258;            /** TBC: 6000ms : 600 * 10 ms. */

    switch (conn_speed) {
        case BLE_ANCS_CONN_HIGH_SPEED: {
            conn_params.min_conn_interval = 0x0010;/** TBC: 20ms : 16 * 1.25 ms. */
            conn_params.max_conn_interval = 0x0020;/** TBC: 40ms : 32 * 1.25 ms. */
            conn_params.conn_latency = 0;
        }
        break;
        case BLE_ANCS_CONN_LOW_POWER:
        case BLE_ANCS_CONN_DEFAULT: {
            conn_params.min_conn_interval = 0x0130;/** TBC: 380ms : 304 * 1.25 ms. */
            conn_params.max_conn_interval = 0x0140;/** TBC: 400ms : 320 * 1.25 ms. interval = 303 on ios 9.2*/
            conn_params.conn_latency = 4;
        }
        break;
        default: {
            conn_params.min_conn_interval = 0x0130;/*TBC: 380ms : 304 * 1.25 ms*/
            conn_params.max_conn_interval = 0x0140;/*TBC: 400ms : 320 * 1.25 ms*/
            conn_params.conn_latency = 4;
        }
        break;
    }

    ble_gap_update_connection_params(conn_id, &conn_params);
    return ;
}


void ble_ancs_timeout_callback(TimerHandle_t xTimer)
{
    const ble_gap_connection_info_t *conn_info;

    conn_info = ble_gap_get_connection_info(p_ancs_app->conn_id);
    LOG_I(ANCS, "[ANCSapp]conn_interval = %d, notif_num = %d\r\n", conn_info->conn_interval, p_ancs_app->notif_num);

    if (conn_info->conn_interval > 16 && conn_info->conn_interval < 32) {
        if (p_ancs_app->notif_num == 0) {
            /*set connection parameter to be low power*/
            LOG_I(ANCS, "[ANCSapp]high speed -> low power\r\n");
            ble_ancs_update_connection_interval(p_ancs_app->conn_id, BLE_ANCS_CONN_LOW_POWER);
        } else {
            if (xTimerReset(p_ancs_app->ancs_timer, 0) != pdPASS) {
                LOG_I(ANCS, "[ANCSapp]reset timer fail!\r\n");
            }
        }
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_event_callback
* DESCRIPTION
* To receive events
* PARAMETERS
*  ancs_evt            [IN]        Pointer to event
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_event_callback(ble_ancs_event_t *ancs_evt)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    xQueueSend(ancs_queue, (void *)ancs_evt, 0);  //if queue is full ,the new notifications will be dropped
}

void ble_ancs_message_handler(ble_ancs_event_t *ancs_evt)
{
    LOG_I(ANCS, "[ANCSapp]event_id = %d\r\n", ancs_evt->evt_type);

    switch (ancs_evt->evt_type) {
        case BLE_ANCS_EVENT_DISCOVER_COMPLETED: {
            if (ancs_evt->result >= BLE_ANCS_SUCCESS) {
                p_ancs_app->conn_id = ancs_evt->conn_id;
                ble_ancs_enable_data_source(ancs_evt->conn_id);
            } else {
                LOG_I(ANCS, "[ANCSapp]Discover service failed!\r\n");
            }
        }
        break;
        case BLE_ANCS_EVENT_SWITCH_COMPLETED: {
            /* if notification source is not enabled, enable it */
            if (ancs_evt->result >= 0) {
                if (ancs_evt->data.switch_type == ANCS_ENABLE_DATA_SOURCE) {
                    ble_ancs_enable_notification_source(ancs_evt->conn_id);
                } else if (ancs_evt->data.switch_type == ANCS_ENABLE_NOTIF_SOURCE) {
                    /*start low power timer*/
                    if (xTimerStart(p_ancs_app->ancs_timer, 0 ) != pdPASS ) {
                        LOG_I(ANCS, "[ANCSapp]Start timer fail!\r\n");
                    } else {
                        LOG_I(ANCS, "[ANCSapp]ANCS timer started!\r\n");
                    }
                }
            } else if (ancs_evt->result == BLE_STATUS_GATT_INSUFFICIENT_AUTHENTICATION ||
                       ancs_evt->result == BLE_STATUS_GATT_INSUFFICIENT_ENCRYPTION) {
                const ble_gap_connection_info_t *p_conn_info;

                p_conn_info = ble_gap_get_connection_info(ancs_evt->conn_id);
                ble_ancs_deal_with_write_fail(p_conn_info->peer_addr);
            } else {
                LOG_I(ANCS, "[ANCSapp]Enable data source failed! result = %x\r\n", ancs_evt->result);
            }
        }
        break;
        case BLE_ANCS_EVENT_IOS_NOTIFICATION: {
            const ble_gap_connection_info_t *conn_info;

            ble_ancs_show_notification(&ancs_evt->data.notification);

            /*If you need ,get notification attribute*/
            ble_ancs_deal_with_new_notification(ancs_evt->conn_id, &ancs_evt->data.notification);

            /*low power*/
            conn_info = ble_gap_get_connection_info(ancs_evt->conn_id);

            if (conn_info->conn_interval > 300) {
                /*it is in low power status*/
                ble_ancs_update_connection_interval(ancs_evt->conn_id, BLE_ANCS_CONN_HIGH_SPEED);
                LOG_I(ANCS, "[ANCSapp]low power -> high speed\r\n");
            } else if (conn_info->conn_interval > 16 && conn_info->conn_interval < 32) {
                /* it's in high speed status*/
                if (xTimerReset(p_ancs_app->ancs_timer, 0) != pdPASS) {
                    LOG_I(ANCS, "[ANCSapp]reset timer fail!\r\n");
                } else {
                    LOG_I(ANCS, "[ANCSapp]reset timer!\r\n");
                }
            } else {
                LOG_I(ANCS, "[ANCSapp]conn_interval = %d\r\n", conn_info->conn_interval);
            }
        }
        break;
        case BLE_ANCS_EVENT_NOTIFICATION_ATTRIBUTE: {
            if (ancs_evt->result >= BLE_ANCS_SUCCESS) {
                if (ancs_evt->data.attribute.command_id == 0) {
                    ble_ancs_show_attribute(&ancs_evt->data.attribute);

                    /*If you need ,get app attribute*/
                    ble_ancs_get_app_information(&ancs_evt->data.attribute);
                }
            } else {
                LOG_I(ANCS, "[ANCSapp]Get Notification Attribute failed!\r\n");
            }
        }
        break;
        case BLE_ANCS_EVENT_APP_ATTRIBUTE: {
            if (ancs_evt->result >= BLE_ANCS_SUCCESS) {
                ble_ancs_show_attribute(&ancs_evt->data.attribute);
                ble_ancs_remove_from_notification_list(p_ancs_app->curr_notif);
                p_ancs_app->curr_notif = NULL;
                ble_ancs_get_notification_information();
            } else {
                LOG_I(ANCS, "[ANCSapp]Get App Attribute failed!\r\n");
            }
        }
        break;

        case  BLE_ANCS_EVENT_PERFORM_ACTION_COMPLETED:
            if (ancs_evt->result >= BLE_ANCS_SUCCESS) {
                LOG_I(ANCS, "[ANCSapp]Perform action success!\r\n");
            }
            break;
        default:
            break;
    }

}


void ble_ancs_perform_action(uint8_t *cmd)
{
    LOG_I(ANCS, "[ANCS]new_notif = %x\r\n", p_ancs_app->new_notif);
    if (p_ancs_app->new_notif) {
        if (cmd[0] == 'a' && cmd[1] == 'n' && cmd[2] == 'c' && cmd[3] == 's' && cmd[4] == 'p' ) {
            ble_ancs_perform_notification_action(p_ancs_app->new_notif->notif.conn_id, p_ancs_app->new_notif->notif.notif_uid, BLE_ANCS_ACTION_ID_POSITIVE);
            p_ancs_app->new_notif = NULL;
        }
        if (cmd[0] == 'a' && cmd[1] == 'n' && cmd[2] == 'c' && cmd[3] == 's' && cmd[4] == 'n') {
            ble_ancs_perform_notification_action(p_ancs_app->new_notif->notif.conn_id, p_ancs_app->new_notif->notif.notif_uid, BLE_ANCS_ACTION_ID_NEGATIVE);
            p_ancs_app->new_notif = NULL;
        }

    }
}


void ble_ancs_sm_event_handler(ble_event_t event_id, const void *param)
{

    switch (event_id) {
        case BLE_SM_STORE_KEYS_REQ: {
            ble_ancs_pair_info_t pair_info;
            nvdm_status_t   write_status;
            ble_sm_store_keys_req_t *parameter = (ble_sm_store_keys_req_t *)param;

            memcpy(&pair_info.remote_address, &parameter->remote_address, sizeof(ble_address_t));
            memcpy(pair_info.ltk, parameter->local_keyset.encryption_info.ltk, 16);

            write_status = nvdm_write_data_item("ancs", "pair_infomation", NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                                (uint8_t *)&pair_info, sizeof(ble_ancs_pair_info_t));

            if (write_status < NVDM_STATUS_OK) {
                LOG_I(ANCS, "[ANCSapp]write nvdm status = %d\r\n", write_status);
            }
        }
        break;

        case BLE_SM_RETRIEVE_KEYS_REQ: {
            ble_sm_retrieve_keys_req_t *parameter = (ble_sm_retrieve_keys_req_t *)param;

            if (parameter->local_keyset.key_present & BLE_SM_KEY_IRK) {
                memset(parameter->local_keyset.identity_info.irk, 1, 16);
            }

            if (parameter->local_keyset.key_present & BLE_SM_KEY_CSRK) {
                memset(parameter->local_keyset.signing_info.csrk, 2, 16);
            }

            if (parameter->local_keyset.key_present & BLE_SM_KEY_COUNTER) {
                parameter->local_keyset.signing_info.sign_counter = 0;
            }

            if (parameter->local_keyset.key_present & BLE_SM_KEY_LTK) {
                ble_ancs_pair_info_t pair_info;
                nvdm_status_t read_status;
                uint32_t size = sizeof(ble_ancs_pair_info_t);

                read_status = nvdm_read_data_item("ancs", "pair_infomation", (uint8_t *)&pair_info, &size);
                if (read_status < NVDM_STATUS_OK) {
                    LOG_I(ANCS, "[ANCSapp]read nvdm status = %d\r\n", read_status);
                }
                if (!memcmp(&parameter->remote_address, &pair_info.remote_address, sizeof(ble_address_t))) {
                    LOG_I(ANCS, "copy ltk \r\n");
                    memcpy(parameter->local_keyset.encryption_info.ltk, pair_info.ltk, 16);
                }
            }
        }
        break;
        case BLE_SM_PAIRING_AUTHORIZATION_REQ: {
            ble_sm_pairing_authorization_req_t *parameter = (ble_sm_pairing_authorization_req_t *)param;

            ble_sm_reply_pairing_authorization(&parameter->remote_address, true);
        }
        break;

        case BLE_SM_PAIRING_REQ: {
            ble_sm_pairing_req_t *parameter = (ble_sm_pairing_req_t *)param;
            ble_sm_pairing_attribute_t local_attributes = {true, true, false};/* mitm, bond, oob */

            ble_sm_reply_pairing_param(&parameter->remote_address, &local_attributes);
        }
        break;

        case BLE_SM_PAIRING_RESULT_IND: {
            ble_sm_pairing_result_ind_t *parameter = (ble_sm_pairing_result_ind_t *)param;

            if (parameter->result) {

                //enable data source again
                ble_ancs_enable_data_source(p_ancs_app->conn_id);
            }
        }
        break;
        default:
            break;
    }

}


/*****************************************************************************
* FUNCTION
*  ancs_app_init
* DESCRIPTION
* Initialize the context p_ancs
* PARAMETERS
*
* RETURNS
*  ObStatus
*****************************************************************************/
void ble_ancs_app_init()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_ancs_init_t init_info;
    nvdm_status_t   nvdm_status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(p_ancs_app, 0, sizeof(ble_ancs_app_cntx_t));
    init_info.evt_handler = ble_ancs_event_callback;

    ble_ancs_init(&init_info);

    nvdm_status = nvdm_init();

    if (nvdm_status < NVDM_STATUS_OK) {
        LOG_I(ANCS, "[ANCSapp]nvdm init fail, status = %d\r\n", nvdm_status);
    }
    p_ancs_app->ancs_timer = xTimerCreate("ANCS_TIMER",
                                          BLE_ANCS_TIMER_PERIOD / portTICK_PERIOD_MS, pdFALSE,
                                          ( void *)0,
                                          ble_ancs_timeout_callback);

    if (!p_ancs_app->ancs_timer) {
        LOG_I(ANCS, "[ANCSapp]Create timer fail! 0x%x\r\n", p_ancs_app->ancs_timer);
    }
}

void ble_ancs_task_main(void *arg)
{
    ble_ancs_event_t   recv_msg;

    /*LOG_I(ANCS, "[ANCSapp]ancs_task_main start\r\n");*/

    gattc_service_init();

    ble_ancs_app_init();

    ancs_queue = xQueueCreate(ANCS_QUEUE_SIZE, sizeof(ble_ancs_event_t));
    if ( ancs_queue == NULL ) {
        LOG_I(ANCS, "[ANCSapp]Create queue fail!\r\n");
        return;
    }

    while (1) {
        if (xQueueReceive(ancs_queue, (void *)&recv_msg, portMAX_DELAY)) {

            /*LOG_I(ANCS, "[ANCSapp]handle one message id = 0x%x\r\n", recv_msg.evt_type);*/
            ble_ancs_message_handler(&recv_msg);
        }
    }
}

void ble_ancs_task_init()
{
    TaskHandle_t xCreatedTask;

    /*LOG_I(ANCS, "[ANCSapp]create task!\r\n");  */
    xTaskCreate(ble_ancs_task_main, "ancs", 420, NULL, 1, &xCreatedTask);
}

