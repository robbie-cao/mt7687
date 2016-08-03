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

#include "bt_sink_utils.h"
#include "bt_sink_hf.h"
#include "bt_sink_conmgr.h"

extern bt_sink_hf_context_t bt_sink_hf_context[BT_SINK_CM_MAX_DEVICE_NUMBER];
static bt_sink_hf_context_t *bt_sink_hf_hightlight_p;

void bt_sink_hf_set_highlight_device(bt_sink_hf_context_t *device)
{
    bt_sink_report("[Sink][HF][MP] set highlight device, prev:%x, next:%x",
                   bt_sink_hf_hightlight_p, device);

    bt_sink_hf_hightlight_p = device;
}

bt_sink_hf_context_t *bt_sink_hf_get_highlight_device(void)
{
    bt_sink_report("[Sink][HF][MP] highlight device:%x", bt_sink_hf_hightlight_p);
    return bt_sink_hf_hightlight_p;
}

void bt_sink_hf_mp_state_change(bt_sink_hf_context_t *device)
{
    if (device) {
        if (BT_SINK_HF_CALL_STATE_IDLE == device->call_state) {
            uint32_t i;

            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; i++) {
                if (BT_SINK_HF_CALL_STATE_IDLE != bt_sink_hf_context[i].call_state) {

                    bt_sink_hf_set_highlight_device(&bt_sink_hf_context[i]);

                    if (bt_sink_hf_context[i].call_state & BT_SINK_HF_CALL_STATE_HELD_REMAINING) {
                        bt_hf_hold_call(bt_sink_hf_context[i].conn_id, BT_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER, 0);
                    }
                    break;
                }
            }

            if (BT_SINK_CM_MAX_DEVICE_NUMBER == i) {
                bt_sink_hf_set_highlight_device(NULL);
            }
        } else {
            if (NULL == bt_sink_hf_get_highlight_device()) {
                bt_sink_hf_set_highlight_device(device);
            }
        }
    }
}

void bt_sink_hf_mp_answer(bt_sink_hf_context_t *current_device, bool accept)
{
    bt_sink_hf_context_t *highlight = bt_sink_hf_get_highlight_device();

    if (current_device != highlight) {
        if (accept && (highlight->call_state & BT_SINK_HF_CALL_STATE_ACTIVE)) {
            bt_hf_hold_call(highlight->conn_id, BT_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER, 0);
            bt_hf_audio_transfer(highlight->conn_id, BT_HF_AUDIO_TO_AG);
        }

        bt_hf_answer_call(current_device->conn_id, accept);

        if (accept) {
            bt_sink_hf_set_highlight_device(current_device);
        }
    }
}
