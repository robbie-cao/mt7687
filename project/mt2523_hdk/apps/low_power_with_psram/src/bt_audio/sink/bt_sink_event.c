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

#include "bt_gap.h"
#include "bt_sink_event.h"
#include "bt_sink_conmgr.h"
#include "bt_sink_avrcp.h"
#include "bt_sink_a2dp.h"
#include "bt_sink_hf.h"
#include "bt_sink_utils.h"
#ifndef WIN32
#include "bt_sink_queue.h"
#endif /* WIN32 */

const static bt_sink_event_callback_table_t bt_sink_event_callback_table[] = {
    {BT_SINK_EVENT_ALL, bt_sink_cm_event_handler},
    {BT_SINK_EVENT_ALL, bt_sink_hf_event_handler},
    {BT_SINK_EVENT_ALL, bt_sink_a2dp_event_handler},
    {BT_SINK_EVENT_ALL, bt_sink_avrcp_event_handler},
};

static bt_sink_event_context_t bt_sink_event_context;

static void bt_sink_event_node_init(bt_sink_event_node_t *event_node)
{
    event_node->previous = event_node;
    event_node->next = event_node;
}

static void bt_sink_event_node_insert(bt_sink_event_node_t *head, bt_sink_event_node_t *node)
{
    node->next = head;
    node->previous = head->previous;
    head->previous->next = node;
    head->previous = node;
}

static void bt_sink_event_node_remove(bt_sink_event_node_t *node)
{
    node->previous->next = node->next;
    node->next->previous = node->previous;
}

static bt_sink_event_callback_node_t *bt_sink_event_node_find_callback(bt_sink_event_node_t *head,
        bt_sink_event_id_t event_id,
        bt_sink_event_callback_t callback)
{
    bt_sink_event_node_t *current_node = head->next;
    bt_sink_event_callback_node_t *result = NULL;

    while (current_node != head) {
        if (((bt_sink_event_callback_node_t *)current_node)->event_id == event_id
                && ((bt_sink_event_callback_node_t *)current_node)->callback == callback) {
            result = (bt_sink_event_callback_node_t *)current_node;
            break;
        }
        current_node = current_node->next;
    }
    return result;
}

static bt_sink_status_t bt_sink_event_invoke(bt_sink_event_id_t event, void *parameters)
{
    bt_sink_status_t result = BT_SINK_STATUS_SUCCESS;
    bt_sink_event_node_t *dynamic_callback = &bt_sink_event_context.dynamic_callback_header;
    uint32_t index;

    for (index = 0; index < sizeof(bt_sink_event_callback_table) / sizeof(bt_sink_event_callback_table_t); index++) {
        if (bt_sink_event_callback_table[index].event_id == BT_SINK_EVENT_ALL) {
            if (NULL != bt_sink_event_callback_table[index].callback) {
                result = bt_sink_event_callback_table[index].callback(event, parameters);
                if (BT_SINK_STATUS_EVENT_STOP == result) {
                    // TRACE
                    break;
                }
            } else {
                // TRACE
            }
        } else if (bt_sink_event_callback_table[index].event_id == event) {
            result = bt_sink_event_callback_table[index].callback(event, parameters);
            if (BT_SINK_STATUS_EVENT_STOP == result) {
                // TRACE
                break;
            }
        } else {
            // TRACE
        }
    }

    bt_sink_event_context.invoking = event;
    while ((dynamic_callback = dynamic_callback->next) != &bt_sink_event_context.dynamic_callback_header) {
        if (BT_SINK_EVENT_ALL == ((bt_sink_event_callback_node_t *)dynamic_callback)->event_id) {
            result = ((bt_sink_event_callback_node_t *)dynamic_callback)->callback(event, parameters);
            if (BT_SINK_STATUS_EVENT_STOP == result) {
                // TRACE
                break;
            }
        } else if (event == ((bt_sink_event_callback_node_t *)dynamic_callback)->event_id) {
            result = ((bt_sink_event_callback_node_t *)dynamic_callback)->callback(event, parameters);
            if (BT_SINK_STATUS_EVENT_STOP == result) {
                // TRACE
                break;
            }
        } else {
            // TRACE
        }
    }
    bt_sink_event_context.invoking = BT_SINK_EVENT_ALL;

    dynamic_callback = bt_sink_event_context.dynamic_callback_header.next;

    while (dynamic_callback != &bt_sink_event_context.dynamic_callback_header) {
        if (((bt_sink_event_callback_node_t *)dynamic_callback)->dirty) {
            bt_sink_event_node_t *dirty_node = dynamic_callback;

            dynamic_callback = dynamic_callback->next;
            bt_sink_event_node_remove(dirty_node);
            bt_sink_memory_free((void *)dirty_node);
            continue;
        }
        dynamic_callback = dynamic_callback->next;
    }
    return result;
}

void bt_sink_event_init(void)
{
    bt_sink_memset(&bt_sink_event_context, 0, sizeof(bt_sink_event_context_t));

    bt_sink_event_context.invoking = BT_SINK_EVENT_ALL;

    bt_sink_event_node_init(&bt_sink_event_context.dynamic_callback_header);
}

bt_sink_status_t bt_sink_event_send(bt_sink_event_id_t event_id, void *parameters)
{
    return bt_sink_event_invoke(event_id, parameters);
}


void bt_sink_event_post(bt_sink_event_id_t event_id, void *parameters, bt_sink_event_post_result_t callback)
{
    bt_sink_event_t *event = bt_sink_memory_alloc(sizeof(*event));
    bt_sink_queue_event_type_t queue_event_type = BT_SINK_QUEUE_EVENT_SINK_POST;

    bt_sink_report("[Sink] bt_sink_event_post, event:%x", event_id);

    if (NULL != event) {
        event->event_id = event_id;
        event->parameters = parameters;
        event->post_callback = callback;
    }

    if (event_id >= BT_SINK_EVENT_EXTERN_BASE) {
        queue_event_type = BT_SINK_QUEUE_EVENT_EXTERN_POST;
    }

    if (!bt_sink_queue_send(queue_event_type, event)) {
        if (NULL != callback) {
            callback(event_id, BT_SINK_STATUS_FAIL, parameters);
        }
        bt_sink_memory_free(event);
        bt_sink_report("[Sink][Fatal Error] event lost:0x%x", event_id);
    }
}

void bt_sink_event_register_callback(bt_sink_event_id_t event_id, bt_sink_event_callback_t callback)
{
    bt_sink_event_callback_node_t *callback_node =
        bt_sink_event_node_find_callback(&bt_sink_event_context.dynamic_callback_header, event_id, callback);

    if (NULL == callback_node) {
        callback_node = bt_sink_memory_alloc(sizeof(*callback_node));

        if (NULL != callback_node) {
            callback_node->event_id = event_id;
            callback_node->callback = callback;
            bt_sink_event_node_insert(&bt_sink_event_context.dynamic_callback_header, &callback_node->pointer);
        }
    } else {
        callback_node->dirty = false;
    }
}

void bt_sink_event_deregister_callback(bt_sink_event_id_t event_id, bt_sink_event_callback_t callback)
{
    bt_sink_event_callback_node_t *callback_node =
        bt_sink_event_node_find_callback(&bt_sink_event_context.dynamic_callback_header, event_id, callback);
    if (NULL != callback_node) {
        if ((event_id == BT_SINK_EVENT_ALL && BT_SINK_EVENT_ALL != bt_sink_event_context.invoking)
                || event_id == bt_sink_event_context.invoking) {
            callback_node->dirty = true;
        } else {
            bt_sink_event_node_remove(&callback_node->pointer);
            bt_sink_memory_free((void *)callback_node);
        }
    }
}

void bt_sink_event_process(bt_sink_event_t *event)
{
    bt_sink_status_t result;

    if (NULL != event) {
        bt_sink_report("[Sink] bt_sink_event_process:0x%x" , event->event_id);

        result = bt_sink_event_invoke(event->event_id, event->parameters);

        if (event->post_callback) {
            event->post_callback(event->event_id, result, event->parameters);
        }

        bt_sink_memory_free(event);
    } else {
        // TRACE
    }
}

static bt_sink_status_t bt_sink_event_invoke_extern(bt_sink_event_id_t event, void *parameters)
{
    bt_sink_status_t result = BT_SINK_STATUS_SUCCESS;
    bt_sink_event_node_t *dynamic_callback = &bt_sink_event_context.dynamic_callback_header;

    bt_sink_event_context.invoking = event;
    while ((dynamic_callback = dynamic_callback->next) != &bt_sink_event_context.dynamic_callback_header) {
        if (event == ((bt_sink_event_callback_node_t *)dynamic_callback)->event_id) {
            result = ((bt_sink_event_callback_node_t *)dynamic_callback)->callback(event, parameters);
            if (BT_SINK_STATUS_EVENT_STOP == result) {
                // TRACE
                break;
            }
        } else {
            // TRACE
        }
    }
    bt_sink_event_context.invoking = BT_SINK_EVENT_ALL;

    dynamic_callback = bt_sink_event_context.dynamic_callback_header.next;

    while (dynamic_callback != &bt_sink_event_context.dynamic_callback_header) {
        if (((bt_sink_event_callback_node_t *)dynamic_callback)->dirty) {
            bt_sink_event_node_t *dirty_node = dynamic_callback;

            dynamic_callback = dynamic_callback->next;
            bt_sink_event_node_remove(dirty_node);
            bt_sink_memory_free((void *)dirty_node);
            continue;
        }
        dynamic_callback = dynamic_callback->next;
    }
    return result;
}


void bt_sink_event_process_extern(bt_sink_event_t *event)
{
    bt_sink_status_t result;

    if (NULL != event) {
        bt_sink_report("[Sink] bt_sink_event_process_extern:0x%x", event->event_id);

        result = bt_sink_event_invoke_extern(event->event_id, event->parameters);

        if (event->post_callback) {
            event->post_callback(event->event_id, result, event->parameters);
        }

        bt_sink_memory_free(event);
    } else {
        // TRACE
    }
}

