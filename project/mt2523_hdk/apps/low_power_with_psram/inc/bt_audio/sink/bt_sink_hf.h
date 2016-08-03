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

#ifndef BT_SINK_HFP_H
#define BT_SINK_HFP_H

#include <stdint.h>
#include "bt_hfp.h"
#include "bt_sink_resource.h"
#include "bt_sink_event.h"
#include "bt_sink_conmgr.h"

#define BT_SINK_HF_AUDIO_DISC_DURATION  20
#define BT_SINK_HF_MAX_VOLUME  AUD_VOL_OUT_LEVEL6
#define BT_SINK_HF_MIN_VOLUME  AUD_VOL_OUT_LEVEL0
#define BT_SINK_HF_DEFAULT_VOLUME AUD_VOL_OUT_LEVEL4
#define BT_SINK_HF_INVALID_AUDIO_ID  0xFF
#define BT_SINK_HF_MAX_PARAMS_INDEX 3

typedef enum {
    BT_SINK_HF_CALL_STATE_IDLE              = 0x0000,
    BT_SINK_HF_CALL_STATE_INCOMING          = 0x0001,
    BT_SINK_HF_CALL_STATE_INCOMING_HELD     = 0x0002,
    BT_SINK_HF_CALL_STATE_OUTGOING          = 0x0004,
    BT_SINK_HF_CALL_STATE_ACTIVE            = 0x0008,
    BT_SINK_HF_CALL_STATE_TWC_INCOMING      = 0x0010,
    BT_SINK_HF_CALL_STATE_TWC_OUTGOING      = 0x0020,
    BT_SINK_HF_CALL_STATE_HELD_ACTIVE       = 0x0040,
    BT_SINK_HF_CALL_STATE_HELD_REMAINING    = 0x0080,
    BT_SINK_HF_CALL_STATE_MULTIPARTY        = 0x0100,
    BT_SINK_HF_CALL_STATE_UNDEFINED         = 0x0200
} bt_sink_hf_call_state_t;

typedef enum {
    BT_SINK_HF_VOL_ACT_UP,
    BT_SINK_HF_VOL_ACT_DOWN
} bt_sink_hf_volume_act_t;

typedef enum {
    BT_SINK_HF_AUDIO_CALL  = 0,
    BT_SINK_HF_AUDIO_SCO,
    BT_SINK_HF_AUDIO_RING,
    BT_SINK_HF_AUDIO_TOTAL
} bt_sink_hf_audio_type_t;

typedef struct {
    uint16_t conn_id;
    uint16_t audio_type;
    uint8_t audio_id[BT_SINK_HF_AUDIO_TOTAL];
    bt_hf_ag_features_t ag_featues;
    bt_hf_ag_hold_features_t ag_chld_feature;
    bt_sink_hf_call_state_t call_state;
    bt_hf_audio_codec_t codec;
} bt_sink_hf_context_t;

typedef struct {
    uint16_t conn_id;
    bt_address_t bt_addr;
    bool connected;
} bt_sink_hf_connect_state_change_t;

typedef struct {
    uint16_t conn_id;
    bt_sink_hf_call_state_t previous_state;
    bt_sink_hf_call_state_t new_state;
} bt_sink_hf_call_state_change_t;

typedef struct {
    uint16_t conn_id;
    bt_hf_audio_codec_t codec;
    bool connected;
} bt_sink_hf_sco_state_change_t;

typedef struct {
    uint16_t conn_id;
} bt_sink_hf_ringtone_t;

typedef struct {
    uint16_t              conn_id;                                  /**<  the id of current connection*/
    uint16_t              num_size;                                 /**<  the phone number size of the cal*/
    uint8_t               number[BT_HF_MAX_PHONE_NUMBER + 1];       /**<  the phone number of the call*/
    uint8_t               type;                                     /**<  the address type of the call*/
    bool                  waiting;
} bt_sink_caller_information_t;

typedef struct {
    uint16_t              conn_id;
    uint8_t               volume;
} bt_sink_speaker_volume_change_t;

typedef struct {
    uint16_t conn_id;
    bt_sink_hf_call_state_t call_state;
} bt_sink_hf_call_device_t;

#ifdef __BT_SINK_HF_INIT_PARAMS_SWITCH__
typedef struct {
    uint8_t  support_profile;
    uint8_t  support_codec;
    uint16_t support_features;
} bt_sink_hf_init_swtich_params_t;

bt_sink_status_t bt_sink_hf_set_init_params(uint8_t index);
#endif /* __BT_SINK_HF_INIT_PARAMS_SWITCH__ */
void bt_sink_hf_common_callback(bt_event_t event_id, const void *parameters);
bt_sink_status_t bt_sink_hf_event_handler(bt_sink_event_id_t event_id, void *parameters);
bt_status_t bt_sink_hf_get_init_params(bt_hf_init_params_t *param, char *extra_indicators);
void bt_sink_hf_handle_setup_ind(bt_sink_hf_context_t *link, bt_hf_call_setup_state_t setup);
void bt_sink_hf_handle_call_ind(bt_sink_hf_context_t *link, bt_hf_call_active_state_t call);
void bt_sink_hf_handle_held_ind(bt_sink_hf_context_t *link, bt_hf_call_held_state_t held);
void bt_sink_hf_handle_call_info_ind(bt_sink_hf_context_t *link, bt_hf_call_list_ind_t *clcc);
void bt_sink_hf_answer_call(bool accept);
void bt_sink_hf_terminate_call(void);
void bt_sink_hf_dial_last(void);
void bt_sink_hf_switch_audio_path(void);
void bt_sink_hf_release_all_held_call(void);
void bt_sink_hf_release_all_active_accept_others(void);
void bt_sink_hf_hold_all_active_accept_others(void);
void bt_sink_hf_release_speical(uint8_t index);
void bt_sink_hf_hold_speical(uint8_t index);
uint32_t bt_sink_hf_query_call_devices(bt_sink_hf_call_device_t call_list[BT_SINK_CM_MAX_DEVICE_NUMBER]);
bt_sink_hf_context_t *bt_sink_hf_get_highlight_device(void);
void bt_sink_hf_post_callback(bt_sink_event_id_t event_id, bt_sink_status_t result, void *parameters);
#ifdef __BT_SINK_SCO_LOOPBACK__
void bt_sink_sco_loopback_init(void);
#endif /* __BT_SINK_SCO_LOOPBACK__ */
void bt_sink_hf_mp_answer(bt_sink_hf_context_t *current_device, bool accept);
void bt_sink_hf_mp_state_change(bt_sink_hf_context_t *device);
#endif /* BT_SINK_HFP_H */
