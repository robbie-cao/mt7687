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

#include "bt_common_dispatch.h"
#include "bt_audio.h"
#include "bt_events.h"
#include "bt_sink_resource.h"
#include "bt_sink_conmgr.h"
#include <syslog.h>
#include "main.h"
#include "sensor_demo.h"
#include <string.h>

static bt_demo_t g_common_demo = BT_IDLE;

/* BT_AUDIO */
extern void bt_sink_cm_gap_callback(bt_event_t event_id, void *param);
extern bt_status_t bt_sink_cm_get_local_name(char *name);
extern bt_status_t bt_sink_cm_request_link_key(const bt_address_t *remote_addr, uint8_t *key, uint32_t key_len);
extern bt_status_t bt_sink_hf_get_init_params(bt_hf_init_params_t *param, char *extra_indicators);
extern void bt_sink_hf_common_callback(bt_event_t event_id, const void *parameters);
extern bt_status_t bt_sink_a2dp_get_init_params(bt_a2dp_init_params_t *params);
extern void bt_sink_a2dp_common_callback(bt_event_t event_id, const void *param);
extern bt_status_t bt_sink_avrcp_get_init_params(bt_avrcp_init_params_t *param);
extern void bt_sink_avrcp_common_callback(bt_event_t event_id, const void *param);

void bt_gap_common_callback(bt_event_t event_id, void *param)
{
    bt_demo_t cur_demo = BT_IDLE;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            bt_sink_cm_gap_callback(event_id, param);
#ifdef __BT_SINK_WITH_PXP__
            if (BT_AUDIO == cur_demo) {
                bt_gap_common_callback_pxp(event_id, param);
            }
#endif /* __BT_SINK_WITH_PXP__ */
            break;
        }

        case BT_PXP: {
            bt_gap_common_callback_pxp(event_id, param);
            break;
        }

        case BT_SPP_SENSOR:
            switch (event_id) {
                case BT_GAP_IO_CAPABILITY_REQUEST_IND:
                    bt_gap_reply_io_capability_request((bt_address_t *)param, BT_GAP_IO_NO_INPUT_NO_OUTPUT, true, BT_GAP_BONDING_MODE_DEDICATED, false, NULL, NULL);
                    break;
                case BT_GAP_USER_CONFIRM_REQUEST_IND:
                    bt_gap_reply_user_confirm_request((bt_address_t *)param, true);
                    break;
                case BT_GAP_POWER_ON_CNF:
                    bt_gap_set_scan_mode(BT_GAP_MODE_GENERAL_ACCESSIBLE);
                    enable_bt_spp_server();
                    break;
                case BT_GAP_POWER_OFF_CNF:
                    disable_bt_spp_server();
                    bt_common_callback_set_demo(BT_IDLE);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}


bt_status_t bt_gap_get_local_name(char *name)
{
    bt_demo_t cur_demo = BT_IDLE;
    bt_status_t ret = BT_STATUS_SUCCESS;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            ret = bt_sink_cm_get_local_name(name);
            break;
        }
        case BT_SPP_SENSOR:
            strcpy(name, "MTK Device");
            break;
    }

    return ret;
}


uint32_t bt_gap_get_local_cod()
{
    bt_demo_t cur_demo = BT_IDLE;
    uint32_t cod = 0x240404;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            cod = bt_sink_cm_get_local_cod();
            break;
        }
    }

    return cod;
}


bt_status_t bt_gap_get_local_address(bt_address_t *local_addr)
{
    bt_demo_t cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_AUDIO:
            bt_sink_cm_read_local_address(local_addr);
            break;

        default:
            local_addr->address[0] = 0x00;
            local_addr->address[1] = 0x00;
            local_addr->address[2] = 0x00;
            local_addr->address[3] = 0x00;
            local_addr->address[4] = 0x00;
            local_addr->address[5] = 0x00;
            break;
    }

    return BT_STATUS_SUCCESS;
}


bt_status_t bt_gap_request_link_key(const bt_address_t *remote_addr, uint8_t *key, uint32_t key_len)
{
    bt_demo_t cur_demo = BT_IDLE;
    bt_status_t ret = BT_STATUS_PIN_OR_KEY_MISSING;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            ret = bt_sink_cm_request_link_key(remote_addr, key, key_len);
            break;
        }
    }

    return ret;
}


bt_status_t bt_hf_get_init_params(bt_hf_init_params_t *param, char *extra_indicators)
{
    bt_demo_t cur_demo = BT_IDLE;
    bt_status_t ret = BT_STATUS_HF_BASE;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            ret = bt_sink_hf_get_init_params(param, extra_indicators);
            break;
        }
    }

    return ret;
}


void bt_hf_common_callback(bt_event_t event_id, const void *param)
{
    bt_demo_t cur_demo = BT_IDLE;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            bt_sink_hf_common_callback(event_id, param);
            break;
        }
    }
}


bt_status_t bt_a2dp_get_init_params(bt_a2dp_init_params_t *params)
{
    bt_demo_t cur_demo = BT_IDLE;
    bt_status_t ret = BT_STATUS_A2DP_FATAL_ERROR;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            ret = bt_sink_a2dp_get_init_params(params);
            break;
        }
    }

    return ret;
}


void bt_a2dp_common_callback(bt_event_t event_id, const void *param)
{
    bt_demo_t cur_demo = BT_IDLE;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            bt_sink_a2dp_common_callback(event_id, param);
            break;
        }
    }
}


bt_status_t bt_avrcp_get_init_params(bt_avrcp_init_params_t *param)
{
    bt_demo_t cur_demo = BT_IDLE;
    bt_status_t ret = BT_STATUS_A2DP_FATAL_ERROR;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            ret = bt_sink_avrcp_get_init_params(param);;
            break;
        }
    }

    return ret;
}


void bt_avrcp_common_callback(bt_event_t event_id, const void *param)
{
    bt_demo_t cur_demo = BT_IDLE;

    cur_demo = bt_common_callback_get_demo();

    switch (cur_demo) {
        case BT_IDLE:
        case BT_AUDIO: {
            bt_sink_avrcp_common_callback(event_id, param);
            break;
        }
    }
}


void bt_common_callback_set_demo(bt_demo_t demo)
{
    LOG_I(bt, "[bt_audio]set_demo--old: %d, new: %d\n", g_common_demo, demo);

    g_common_demo = demo;
}


bt_demo_t bt_common_callback_get_demo(void)
{
    return g_common_demo;
}

