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

#include <stdbool.h>
#include "bt_hfp.h"
#include "bt_sink_hf.h"
#include "bt_sink_utils.h"
#include "bt_sink_conmgr.h"

extern bt_sink_hf_context_t bt_sink_hf_context[BT_SINK_CM_MAX_DEVICE_NUMBER];

static bool bt_sink_hf_is_feature_support(bt_sink_event_id_t user_event, bt_sink_hf_context_t *device)
{
    bool result = false;

    switch (user_event) {
        case BT_SINK_EVENT_HF_QUERY_CALL_LIST:
            if (device->ag_featues & (BT_HF_AG_FEATURE_3_WAY | BT_HF_AG_FEATURE_ENHANCED_CALL_STATUS)) {
                result = true;
            }
            break;

        case BT_SINK_EVENT_HF_3WAY_RELEASE_ALL_HELD:
            if ((device->ag_featues & (BT_HF_AG_FEATURE_3_WAY | BT_HF_AG_FEATURE_ENHANCED_CALL_STATUS))
                    && (device->ag_chld_feature & BT_HF_AG_HOLD_RELEASE_HELD_CALLS)) {
                result = true;
            }
            break;

        case BT_SINK_EVENT_HF_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER:
            if ((device->ag_featues & (BT_HF_AG_FEATURE_3_WAY | BT_HF_AG_FEATURE_ENHANCED_CALL_STATUS))
                    && (device->ag_chld_feature & BT_HF_AG_HOLD_RELEASE_ACTIVE_CALLS)) {
                result = true;
            }
            break;

        case BT_SINK_EVENT_HF_3WAY_HOLD_ACTIVE_ACCEPT_OTHER:
            if ((device->ag_featues & (BT_HF_AG_FEATURE_3_WAY | BT_HF_AG_FEATURE_ENHANCED_CALL_STATUS))
                    && (device->ag_chld_feature & BT_HF_AG_HOLD_HOLD_ACTIVE_CALLS)) {
                result = true;
            }
            break;

        case BT_SINK_EVENT_HF_3WAY_RELEASE_SPECIAL:
            if ((device->ag_featues & (BT_HF_AG_FEATURE_3_WAY | BT_HF_AG_FEATURE_ENHANCED_CALL_CTRL))
                    && (device->ag_chld_feature & BT_HF_AG_HOLD_RELEASE_SPECIFIC_CALL)) {
                result = true;
            }
            break;

        case BT_SINK_EVENT_HF_3WAY_HOLD_SPECIAL:
            if ((device->ag_featues & (BT_HF_AG_FEATURE_3_WAY | BT_HF_AG_FEATURE_ENHANCED_CALL_CTRL))
                    && (device->ag_chld_feature & BT_HF_AG_HOLD_HOLD_SPECIFIC_CALL)) {
                result = true;
            }
            break;


        case BT_SINK_EVENT_HF_3WAY_ADD_HELD_CALL_TO_CONVERSATION:
            if (device->ag_featues & BT_HF_AG_FEATURE_3_WAY &&
                    device->ag_chld_feature & BT_HF_AG_HOLD_ADD_HELD_CALL) {
                result = true;
            }
            break;

        case BT_SINK_EVENT_HF_3WAY_EXPLICIT_CALL_TRANSFER:
            if ((device->ag_featues & BT_HF_AG_FEATURE_3_WAY) &&
                    (device->ag_chld_feature & BT_HF_AG_HOLD_CALL_TRANSFER)) {
                result = true;
            }
            break;

        case BT_SINK_EVENT_HF_VOICE_RECOGNITION_ACTIVATE:
            if (device->ag_featues & BT_HF_AG_FEATURE_VOICE_RECOGNITION) {
                result = true;
            }
            break;

        default:
            break;
    }
    return result;
}

void bt_sink_hf_call_set_state(bt_sink_hf_context_t *link, bt_sink_hf_call_state_t new_state)
{
    if (link) {
        // TRACE(link->call_state && new_state)
        if (link->call_state != new_state) {
            bt_sink_hf_call_state_change_t *state_change =
                bt_sink_memory_alloc(sizeof(*state_change));

            //state change.
            state_change->conn_id = link->conn_id;
            state_change->previous_state = link->call_state;
            state_change->new_state = new_state;
            link->call_state = new_state;

            bt_sink_hf_mp_state_change(link);

            bt_sink_event_send(BT_SINK_EVENT_HF_CALL_STATE_CHANGE_SYNC, (void *)state_change);
            bt_sink_event_post(BT_SINK_EVENT_HF_CALL_STATE_CHANGE,
                               (void *)state_change,
                               bt_sink_hf_post_callback);
        }
    } else {
        //ASSERT
        //TRACE
    }
}

void bt_sink_hf_handle_setup_ind(bt_sink_hf_context_t *link, bt_hf_call_setup_state_t setup)
{
    if (link) {
        switch (setup) {
            case BT_HF_CALL_SETUP_NONE:
                switch (link->call_state) {
                    case BT_SINK_HF_CALL_STATE_INCOMING:
                    case BT_SINK_HF_CALL_STATE_OUTGOING:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_IDLE);
                        break;

                    case BT_SINK_HF_CALL_STATE_TWC_INCOMING:
                        bt_hf_query_current_calls(link->conn_id);
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_ACTIVE);
                        break;

                    case BT_SINK_HF_CALL_STATE_TWC_OUTGOING:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_HELD_ACTIVE);
                        break;

                    default:
                        break;
                }
                break;

            case BT_HF_CALL_SETUP_IN:
                switch (link->call_state) {
                    case BT_SINK_HF_CALL_STATE_IDLE:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_INCOMING);
                        break;

                    case BT_SINK_HF_CALL_STATE_ACTIVE:
                    case BT_SINK_HF_CALL_STATE_HELD_ACTIVE:
                    case BT_SINK_HF_CALL_STATE_HELD_REMAINING:
                    case BT_SINK_HF_CALL_STATE_MULTIPARTY:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_TWC_INCOMING);
                        break;

                    default:
                        break;
                }
                break;

            case BT_HF_CALL_SETUP_OUT:
            case BT_HF_CALL_SETUP_ALERT:
                switch (link->call_state) {
                    case BT_SINK_HF_CALL_STATE_IDLE:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_OUTGOING);
                        break;

                    case BT_SINK_HF_CALL_STATE_ACTIVE:
                    case BT_SINK_HF_CALL_STATE_HELD_ACTIVE:
                    case BT_SINK_HF_CALL_STATE_HELD_REMAINING:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_TWC_OUTGOING);
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}


void bt_sink_hf_handle_call_ind(bt_sink_hf_context_t *link, bt_hf_call_active_state_t call)
{
    if (link) {
        switch (call) {
            case BT_HF_CALL_NONE:
                switch (link->call_state) {
                    case BT_SINK_HF_CALL_STATE_IDLE:
                        break;

                    case BT_SINK_HF_CALL_STATE_TWC_INCOMING:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_INCOMING);
                        break;

                    case BT_SINK_HF_CALL_STATE_TWC_OUTGOING:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_OUTGOING);
                        break;

                    default:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_IDLE);
                        break;
                }
                break;

            case BT_HF_CALL_ACTIVE:
                switch (link->call_state) {
                    case BT_SINK_HF_CALL_STATE_IDLE:
                    case BT_SINK_HF_CALL_STATE_INCOMING:
                    case BT_SINK_HF_CALL_STATE_OUTGOING:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_ACTIVE);
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}

void bt_sink_hf_handle_held_ind(bt_sink_hf_context_t *link, bt_hf_call_held_state_t held)
{
    if (link) {
        switch (held) {
            case BT_HF_CALL_HELD_NONE:
                switch (link->call_state) {
                    case BT_SINK_HF_CALL_STATE_TWC_OUTGOING:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_OUTGOING);
                        break;

                    case BT_SINK_HF_CALL_STATE_HELD_REMAINING:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_ACTIVE);
                        break;

                    case BT_SINK_HF_CALL_STATE_HELD_ACTIVE:
                        bt_hf_query_current_calls(link->conn_id);
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_ACTIVE);
                        break;

                    default:
                        break;
                }
                break;

            case BT_HF_CALL_HELD_ACTIVE:
                switch (link->call_state) {
                    case BT_SINK_HF_CALL_STATE_ACTIVE:
                    case BT_SINK_HF_CALL_STATE_TWC_INCOMING:
                    case BT_SINK_HF_CALL_STATE_HELD_REMAINING:
                    case BT_SINK_HF_CALL_STATE_MULTIPARTY:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_HELD_ACTIVE);
                        break;

                    default:
                        break;
                }
                break;

            case BT_HF_CALL_HELD_ALL:
                switch (link->call_state) {
                    case BT_SINK_HF_CALL_STATE_ACTIVE:
                    case BT_SINK_HF_CALL_STATE_TWC_OUTGOING:
                    case BT_SINK_HF_CALL_STATE_HELD_ACTIVE:
                    case BT_SINK_HF_CALL_STATE_MULTIPARTY:
                        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_HELD_REMAINING);
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}


void bt_sink_hf_handle_call_info_ind(bt_sink_hf_context_t *link, bt_hf_call_list_ind_t *clcc)
{
    if (clcc->multiple_party) {
        bt_sink_hf_call_set_state(link, BT_SINK_HF_CALL_STATE_MULTIPARTY);
    }
}

static bt_sink_hf_context_t *bt_sink_hf_find_device_by_call_state(bt_sink_hf_call_state_t call_state)
{
    uint32_t i;
    bt_sink_hf_context_t *device = NULL, *highlight = NULL;

    highlight = bt_sink_hf_get_highlight_device();

    if (NULL != highlight && highlight->call_state & call_state) {
        device = highlight;
    } else {
        for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; i++) {
            if (highlight != &bt_sink_hf_context[i] &&
                    call_state & bt_sink_hf_context[i].call_state) {
                device = &bt_sink_hf_context[i];
                break;
            }
        }
    }
    return device;
}

uint32_t bt_sink_hf_query_call_devices(bt_sink_hf_call_device_t call_list[BT_SINK_CM_MAX_DEVICE_NUMBER])
{
    uint32_t index, count = 0;
    bt_sink_hf_context_t *highlight = bt_sink_hf_get_highlight_device();

    if (NULL != call_list) {
        bt_sink_memset(call_list, 0, BT_SINK_CM_MAX_DEVICE_NUMBER * sizeof(bt_sink_hf_call_device_t));
    }

    if (NULL != highlight && highlight->call_state > BT_SINK_HF_CALL_STATE_IDLE) {
        if (NULL != call_list) {
            call_list[count].conn_id = highlight->conn_id;
            call_list[count].call_state = highlight->call_state;
        }
        count++;
    }

    for (index = 0; index < BT_SINK_CM_MAX_DEVICE_NUMBER; index++) {
        if (0 != bt_sink_hf_context[index].conn_id &&
                bt_sink_hf_context[index].call_state > BT_SINK_HF_CALL_STATE_IDLE) {
            if (NULL != call_list) {
                call_list[count].conn_id = bt_sink_hf_context[index].conn_id;
                call_list[count].call_state = bt_sink_hf_context[index].call_state;
            }
            count++;
        }
    }

    return count;
}

void bt_sink_hf_answer_call(bool accept)
{
    bt_sink_hf_context_t *device;

    if (NULL != (device = bt_sink_hf_find_device_by_call_state(BT_SINK_HF_CALL_STATE_INCOMING))) {
        if (device != bt_sink_hf_get_highlight_device()) {
            bt_sink_hf_mp_answer(device, accept);
        } else {
            bt_hf_answer_call(device->conn_id, accept);
        }
    } else if (NULL != (device = bt_sink_hf_find_device_by_call_state(BT_SINK_HF_CALL_STATE_TWC_INCOMING))) {
        bt_hf_hold_call(device->conn_id, BT_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER, 0);
    }
}

void bt_sink_hf_terminate_call(void)
{
    bt_sink_hf_context_t *device;

    if (NULL != (device = bt_sink_hf_find_device_by_call_state((bt_sink_hf_call_state_t)
                          (BT_SINK_HF_CALL_STATE_OUTGOING | BT_SINK_HF_CALL_STATE_ACTIVE)))) {
        bt_hf_terminate_call(device->conn_id);
    } else if (NULL != (device = bt_sink_hf_find_device_by_call_state((bt_sink_hf_call_state_t)
                                 (BT_SINK_HF_CALL_STATE_TWC_INCOMING | BT_SINK_HF_CALL_STATE_TWC_OUTGOING |
                                  BT_SINK_HF_CALL_STATE_HELD_ACTIVE | BT_SINK_HF_CALL_STATE_MULTIPARTY)))) {
        bt_hf_hold_call(device->conn_id, BT_HF_CHLD_RELEASE_ACTIVE_ACCEPT_OTHER, 0);
    } else if (NULL != (device = bt_sink_hf_find_device_by_call_state(BT_SINK_HF_CALL_STATE_HELD_REMAINING))) {
        bt_hf_hold_call(device->conn_id, BT_HF_CHLD_RELEASE_HELD_REJECT_WAITING, 0);
    }
}

void bt_sink_hf_release_all_held_call(void)
{
    bt_sink_hf_context_t *device;

    device = bt_sink_hf_find_device_by_call_state((bt_sink_hf_call_state_t)
             (BT_SINK_HF_CALL_STATE_TWC_INCOMING |
              BT_SINK_HF_CALL_STATE_HELD_ACTIVE |
              BT_SINK_HF_CALL_STATE_HELD_REMAINING));

    if (NULL != device && bt_sink_hf_is_feature_support(BT_SINK_EVENT_HF_3WAY_RELEASE_ALL_HELD, device)) {
        bt_hf_hold_call(device->conn_id, BT_HF_CHLD_RELEASE_HELD_REJECT_WAITING, 0);
    }
}

void bt_sink_hf_release_all_active_accept_others(void)
{
    bt_sink_hf_context_t *device;

    device = bt_sink_hf_find_device_by_call_state((bt_sink_hf_call_state_t)
             (BT_SINK_HF_CALL_STATE_TWC_INCOMING | BT_SINK_HF_CALL_STATE_ACTIVE |
              BT_SINK_HF_CALL_STATE_HELD_ACTIVE | BT_SINK_HF_CALL_STATE_HELD_REMAINING));

    if (NULL != device && bt_sink_hf_is_feature_support(BT_SINK_EVENT_HF_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER, device)) {
        bt_hf_hold_call(device->conn_id, BT_HF_CHLD_RELEASE_ACTIVE_ACCEPT_OTHER, 0);
    }
}

void bt_sink_hf_hold_all_active_accept_others(void)
{
    bt_sink_hf_context_t *device;

    device = bt_sink_hf_find_device_by_call_state((bt_sink_hf_call_state_t)
             (BT_SINK_HF_CALL_STATE_TWC_INCOMING | BT_SINK_HF_CALL_STATE_HELD_ACTIVE |
              BT_SINK_HF_CALL_STATE_HELD_REMAINING | BT_SINK_HF_CALL_STATE_ACTIVE));

    if (NULL != device && bt_sink_hf_is_feature_support(BT_SINK_EVENT_HF_3WAY_HOLD_ACTIVE_ACCEPT_OTHER, device)) {
        bt_hf_hold_call(device->conn_id, BT_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER, 0);
    }
}


void bt_sink_hf_release_speical(uint8_t index)
{
    bt_sink_hf_context_t *device;

    device = bt_sink_hf_find_device_by_call_state(BT_SINK_HF_CALL_STATE_MULTIPARTY);

    if (NULL != device && bt_sink_hf_is_feature_support(BT_SINK_EVENT_HF_3WAY_RELEASE_SPECIAL, device)) {
        bt_hf_hold_call(device->conn_id, BT_HF_CHLD_RELEASE_ACTIVE_ACCEPT_OTHER, index);
    }
}

void bt_sink_hf_hold_speical(uint8_t index)
{
    bt_sink_hf_context_t *device;

    device = bt_sink_hf_find_device_by_call_state(BT_SINK_HF_CALL_STATE_MULTIPARTY);

    if (NULL != device && bt_sink_hf_is_feature_support(BT_SINK_EVENT_HF_3WAY_HOLD_SPECIAL, device)) {
        bt_hf_hold_call(device->conn_id, BT_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER, index);
    }
}


void bt_sink_hf_dial_last(void)
{
    uint16_t device_list[BT_SINK_CM_MAX_DEVICE_NUMBER];

    if (bt_sink_cm_get_connected_device(BT_SINK_UUID_HF, device_list) > 0) {
        bt_hf_dial_last_number(device_list[0]);
    }
}

void bt_sink_hf_switch_audio_path(void)
{
    bt_sink_hf_context_t *bt_sink_hf_context_p = bt_sink_hf_get_highlight_device();
    bt_sink_report("[Sink][HF] bt_sink_hf_context_p:0x%x", bt_sink_hf_context_p);
    if (NULL != bt_sink_hf_context_p && BT_SINK_HF_CALL_STATE_IDLE != bt_sink_hf_context_p->call_state) {
        bt_sink_report("[Sink][HF] call_state:0x%x, 0x%x", bt_sink_hf_context_p->call_state, bt_sink_hf_context_p->audio_type);
        if (bt_sink_hf_context_p->audio_type & (1 << BT_SINK_HF_AUDIO_SCO)) {
            bt_hf_audio_transfer(bt_sink_hf_context_p->conn_id, BT_HF_AUDIO_TO_AG);
        } else {
            bt_hf_audio_transfer(bt_sink_hf_context_p->conn_id, BT_HF_AUDIO_TO_HF);
        }
    }
}


