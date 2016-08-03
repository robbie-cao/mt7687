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
#include "bt_sink_conmgr.h"
#include "bt_sink_resource.h"
#include "bt_sink_utils.h"
#ifdef __BT_SINK_AM_SUPPORT__
#include "bt_sink_ami.h"
#endif /* __BT_SINK_AM_SUPPORT__ */
#ifdef __BT_SINK_SCO_LOOPBACK__
#include "bt_hfp_codec_internal.h"
#endif /* __BT_SINK_SCO_LOOPBACK__ */
#ifdef __BT_SINK_HF_INIT_PARAMS_SWITCH__
#include "nvdm.h"
#endif /* __BT_SINK_HF_INIT_PARAMS_SWITCH__ */
#ifdef __BT_SINK_AM_SUPPORT__
#define BT_SINK_HF_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HEADSET
#ifdef MTK_DEVELOPMENT_BOARD_HDK
#define BT_SINK_HF_INPUT_DEVICE HAL_AUDIO_DEVICE_MAIN_MIC
#else
#define BT_SINK_HF_INPUT_DEVICE HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC
#endif
#endif /* __BT_SINK_AM_SUPPORT__ */

bt_sink_profile_t bt_sink_hf[BT_SINK_CM_MAX_DEVICE_NUMBER];
bt_sink_hf_context_t bt_sink_hf_context[BT_SINK_CM_MAX_DEVICE_NUMBER];
static bool bt_sink_sco_loopback;
#ifdef __BT_SINK_SCO_LOOPBACK__
static bt_media_handle_t *sink_hf_media_handle;
#endif /* __BT_SINK_SCO_LOOPBACK__ */

static uint16_t bt_sink_hf_tone_16ksr[64] = {
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6
};

#ifdef __BT_SINK_DEBUG_INFO__
const static char *g_sink_hf_event_string[] = {
    "BT_HF_SLC_CONNECT_IND",     /**< The service level connection is connected. */
    "BT_HF_CONNECT_REQUEST_IND",                      /**< The Audio Gateway (AG) requested to connect Hands Free (HF). */
    "BT_HF_SLC_DISCONNECT_IND",                       /**< The service level connection is disconnected. */
    "BT_HF_AUDIO_CONNECT_IND",                        /**< The Audio SCO is connected. */
    "BT_HF_AUDIO_DISCONNECT_IND",                     /**< The Audio SCO is disconnected. */
    "BT_HF_BRSF_FEATURES_IND",                        /**< The BRSF features are supported. */
    "BT_HF_CALL_HELD_FEATURES_IND",                   /**< The call held features are supported. */
    "BT_HF_CIEV_CALL_SETUP_IND",                      /**< The call setup status is changed. */
    "BT_HF_CIEV_CALL_IND",                            /**< The call status is changed. */
    "BT_HF_CIEV_CALL_HELD_IND",                       /**< The call hold status is changed. */
    "BT_HF_CIEV_SERVICE_IND",                         /**< The service status is changed. */
    "BT_HF_CIEV_SIGNAL_IND",                          /**< The signal value is changed. */
    "BT_HF_CIEV_ROAM_IND",                            /**< The roaming status is changed. */
    "BT_HF_CIEV_BATTCHG_IND",                         /**< The battery value is changed. */
    "BT_HF_RING_IND",                                 /**< A call rang. */
    "BT_HF_CALL_WAITING_IND",                         /**< The waiting call information is available for a waiting call. */
    "BT_HF_CALLER_ID_IND",                            /**< The caller information is available for an incoming call. */
    "BT_HF_CURRENT_CALLS_IND",                        /**< The current call information is available for the queried calls. */
    "BT_HF_VOICE_RECOGNITION_IND",                    /**< The voice recognition feature is changed. */
    "BT_HF_VOLUME_SYNC_SPEAKER_GAIN_IND",             /**< The value of a remote speaker volume is changed. */
    "BT_HF_VOLUME_SYNC_MIC_GAIN_IND",                 /**< The value of a remote microphone volume is changed. */
    "BT_HF_IN_BAND_RING_IND",                         /**< The in-band ring feature is changed. */
    "BT_HF_EXTRA_INDICATOR_INFO_IND",                 /**< The extra indicator information in negotiation state is available. */
    "BT_HF_EXTRA_INDICATOR_UPDATE_IND",               /**< The extra indicator value is changed. */
    "BT_HF_SLC_CONNECT_CNF",                          /**< The result of a connection request is available. */
    "BT_HF_DIAL_NUMBER_CNF",                          /**< The result of a dial request is available. */
    "BT_HF_ENABLE_VOICE_RECOGNITION_CNF",             /**< The voice recognition request is enabled. */
    "BT_HF_ENABLE_CALLER_ID_CNF",                     /**< The caller ID request is enabled. */
    "BT_HF_ANSWER_CALL_CNF",                          /**< The result to answer the call request is available. */
    "BT_HF_TERMINATE_CALL_CNF",                       /**< The call request is terminated. */
    "BT_HF_ENABLE_CALL_WAITING_CNF",                  /**< The call waiting request is enabled. */
    "BT_HF_CALL_HOLD_ACTION_CNF",                     /**< The result of call hold action request is available. */
    "BT_HF_DTMF_CNF",                                 /**< The result of send DTMF request is available. */
    "BT_HF_CURRENT_CALLS_CNF",                        /**< The result of current inquired call requests is available. */
    "BT_HF_MEMORY_DIAL_CNF",                          /**< The result of memory dial request is available. */
    "BT_HF_SPEAKER_VOLUME_SYNC_CNF",                  /**< The result of speaker volume sync request is available. */
    "BT_HF_MIC_VOLUME_SYNC_CNF",                      /**< The result of microphone volume sync request is available. */
    "BT_HF_EXTERN_AT_CMD_CNF",                        /**< The result of external AT command sending request is available. */
    "BT_HS_SEND_KEYPAD_CNF",                          /**< The result of send keypad request is available. */
};

const static char *g_sink_hf_call_state[] = {
    "IDLE",
    "INCOMING",
    "INCOMING_HELD",
    "OUTGOING",
    "ACTIVE",
    "TWC_INCOMING",
    "TWC_OUTGOING",
    "HELD_ACTIVE",
    "HELD_REMAINING",
    "MULTIPARTY",
    "UNDEFINED"
};

const static char *sink_hf_audio_type[] = {
    "BT_SINK_HF_AUDIO_CALL",
    "BT_SINK_HF_AUDIO_SCO",
    "BT_SINK_HF_AUDIO_RING",
    "BT_SINK_HF_AUDIO_TOTAL"
};

const static char *sink_hf_audio_msg[] = {
    "AUD_SELF_CMD_REQ",
    "AUD_RESUME_IND",
    "AUD_SUSPEND_IND",
    "AUD_SUSPEND_BY_IND",
    "AUD_A2DP_PROC_IND",
    "AUD_STREAM_EVENT_IND",
    "AUD_TIMER_IND",
    "AUD_SINK_OPEN_CODEC"
};

const static char *sink_hf_audio_sub_msg[] = {
    "AUD_EMPTY",
    "AUD_SINK_PROC_PTR",
    "AUD_CMD_FAILURE",
    "AUD_CMD_COMPLETE",
    "AUD_SUSPEND_BY_NONE",
    "AUD_SUSPEND_BY_PCM",
    "AUD_SUSPEND_BY_HFP",
    "AUD_SUSPEND_BY_A2DP"
};
#endif

#ifdef __BT_SINK_HF_INIT_PARAMS_SWITCH__
static const bt_sink_hf_init_swtich_params_t bt_sink_hf_init_swtich_params[BT_SINK_HF_MAX_PARAMS_INDEX] = {
    {   BT_HF_RPOFILE_HSP, BT_HF_WBS_CODEC_MASK_CVSD,
        (BT_HF_FEATURE_CALL_WAITING | BT_HF_FEATURE_CLI_PRESENTATION | BT_HF_FEATURE_VOICE_RECOGNITION
        | BT_HF_FEATURE_ECHO_NOISE | BT_HF_FEATURE_VOLUME_CONTROL
        | BT_HF_FEATURE_ENHANCED_CALL_STATUS)
    },
    {   BT_HF_PROFILE_V15, BT_HF_WBS_CODEC_MASK_CVSD,
        (BT_HF_FEATURE_CALL_WAITING | BT_HF_FEATURE_CLI_PRESENTATION | BT_HF_FEATURE_VOICE_RECOGNITION
        | BT_HF_FEATURE_ECHO_NOISE | BT_HF_FEATURE_VOLUME_CONTROL
        | BT_HF_FEATURE_ENHANCED_CALL_STATUS)
    },
    {   BT_HF_PROFILE_V16, (BT_HF_WBS_CODEC_MASK_CVSD | BT_HF_WBS_CODEC_MASK_MSBC),
        (BT_HF_FEATURE_CALL_WAITING | BT_HF_FEATURE_CLI_PRESENTATION | BT_HF_FEATURE_VOICE_RECOGNITION
        | BT_HF_FEATURE_ECHO_NOISE | BT_HF_FEATURE_VOLUME_CONTROL
        | BT_HF_FEATURE_ENHANCED_CALL_STATUS | BT_HF_FEATURE_CODEC_NEGOTIATION)
    }
};
#endif /* __BT_SINK_HF_INIT_PARAMS_SWITCH__ */

static bool bt_sink_hf_audio_connect(bt_sink_hf_context_t *bt_sink_hf_context_p, bt_sink_hf_audio_type_t audio_type);
static void bt_sink_hf_audio_disconnect(bt_sink_hf_context_t *bt_sink_hf_context_p, bt_sink_hf_audio_type_t audio_type);

void bt_sink_hf_post_callback(bt_sink_event_id_t event_id, bt_sink_status_t result, void *parameters)
{
    switch (event_id) {
        case BT_SINK_EVENT_HF_CALL_STATE_CHANGE:
        case BT_SINK_EVENT_HF_CONNECT_STATE_CHANGE:
        case BT_SINK_EVENT_HF_AUDIO_STATE_CHANGE:
        case BT_SINK_EVENT_HF_CALLER_INFORMATION:
        case BT_SINK_EVENT_HF_RING_TONE: {
            if (NULL != parameters) {
                bt_sink_memory_free(parameters);
            }
        }
        break;

        default:
            break;
    }
}

#ifdef __BT_SINK_SCO_LOOPBACK__
bt_sink_status_t bt_sink_loopback_handler(bt_sink_event_id_t event, const void *param)
{
    if (event == BT_SINK_EVENT_DUT_MODE_LOOP_BACK_ON) {
        bt_sink_sco_loopback = true;
    } else if (event == BT_SINK_EVENT_DUT_MODE_LOOP_BACK_OFF) {
        bt_sink_sco_loopback = false;
    }
}

void bt_sink_sco_loopback_init(void)
{
    bt_sink_sco_loopback = false;

    bt_sink_event_register_callback(BT_SINK_EVENT_DUT_MODE_LOOP_BACK_ON, bt_sink_loopback_handler);
    bt_sink_event_register_callback(BT_SINK_EVENT_DUT_MODE_LOOP_BACK_OFF, bt_sink_loopback_handler);
}
#endif /* __BT_SINK_SCO_LOOPBACK__ */

static void bt_sink_hf_sco_disconnect_callback(void *parameter)
{
    bt_sink_hf_context_t *bt_sink_hf_context_p = (bt_sink_hf_context_t *)parameter;

    bt_hf_audio_disconnect(bt_sink_hf_context_p->conn_id);
}

static void bt_sink_hf_sco_disconnect(bt_sink_hf_context_t *bt_sink_hf_context_p)
{
    bt_sink_timer_start(BT_SINK_TIMER_HF_AUDIO_DISC,
                        BT_SINK_HF_AUDIO_DISC_DURATION,
                        bt_sink_hf_sco_disconnect_callback,
                        bt_sink_hf_context_p);
}


bool bt_sink_hf_audio_get_audio_id(bt_sink_hf_audio_type_t type, uint8_t *audio_id)
{
    bool result = false;
    uint32_t i;

    bt_sink_assert(BT_SINK_HF_AUDIO_TOTAL > type);

    for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; i++) {
        if (bt_sink_hf_context[i].audio_type & (1 << type)) {
            if (NULL != audio_id) {
                *audio_id = bt_sink_hf_context[i].audio_id[type];
            }
            result = true;
        }
    }
    bt_sink_report("[Sink][HF] type:%d, result:%d", type, result);
    return result;
}

#ifdef __BT_SINK_AM_SUPPORT__
void bt_sink_hf_audio_am_callback(bt_sink_am_id_t aud_id, bt_sink_am_cb_msg_class_t msg_id, bt_sink_am_cb_sub_msg_t sub_msg, void *parm)
{
    uint8_t current_id;
#ifdef __BT_SINK_DEBUG_INFO__
    if (sub_msg <= AUD_SUSPEND_BY_A2DP) {
        bt_sink_report("[Sink][HF][AM] cb, aud_id:%d, msg_id:%s, sub_msg:%s",
                       aud_id, sink_hf_audio_msg[msg_id], sink_hf_audio_sub_msg[sub_msg]);
    } else {
        bt_sink_report("[Sink][HF][AM] cb, aud_id:%x, msg_id:%x, sub_msg:%x", aud_id, msg_id, sub_msg);
    }
#else
    bt_sink_report("[Sink][HF][AM] cb, aud_id:%x, msg_id:%x, sub_msg:%x", aud_id, msg_id, sub_msg);
#endif /* __BT_SINK_DEBUG_INFO__ */

    if (AUD_RESUME_IND == msg_id) {
        if (bt_sink_hf_audio_get_audio_id(BT_SINK_HF_AUDIO_CALL, &current_id) && current_id == aud_id) {
            bt_sink_am_audio_capability_t capability;

            capability.type = NONE;
            bt_sink_ami_audio_play(aud_id, &capability);
        }
    } else if (AUD_SUSPEND_BY_IND == msg_id && AUD_SUSPEND_BY_NONE == sub_msg
               && bt_sink_hf_audio_get_audio_id(BT_SINK_HF_AUDIO_SCO, &current_id) && current_id == aud_id) {
        bt_sink_hf_context_t *highlight = bt_sink_hf_get_highlight_device();

        if (NULL != highlight) {
            bt_sink_hf_audio_disconnect(highlight, BT_SINK_HF_AUDIO_SCO);
            bt_sink_hf_audio_connect(highlight, BT_SINK_HF_AUDIO_SCO);
        }
    }
}

static void bt_sink_hf_audio_pcm_parameter_init(bt_sink_am_audio_capability_t *audio_capability)
{
    if (NULL != audio_capability) {
        audio_capability->type = PCM;
        audio_capability->audio_stream_out.audio_device = BT_SINK_HF_OUTPUT_DEVICE;
        audio_capability->audio_stream_out.audio_volume = AUD_VOL_OUT_LEVEL6;
        audio_capability->codec.pcm_format.stream.stream_sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
        audio_capability->codec.pcm_format.stream.stream_channel = HAL_AUDIO_STEREO;
        audio_capability->codec.pcm_format.stream.buffer = bt_sink_hf_tone_16ksr;
        audio_capability->codec.pcm_format.stream.size = sizeof(bt_sink_hf_tone_16ksr);
        audio_capability->codec.pcm_format.in_out = STREAM_OUT;
        audio_capability->audio_path_type = HAL_AUDIO_PLAYBACK_MUSIC;
    }
}

static void bt_sink_hf_audio_sco_parameter_init(bt_sink_am_audio_capability_t *audio_capability, bt_hf_codec_type_t codec)
{
    if (NULL != audio_capability) {
        audio_capability->type = HFP;
        audio_capability->codec.hfp_format.hfp_codec.type = codec;
        audio_capability->audio_stream_in.audio_device = BT_SINK_HF_INPUT_DEVICE;
        audio_capability->audio_stream_in.audio_volume = AUD_VOL_IN_LEVEL0;
        audio_capability->audio_stream_out.audio_device = BT_SINK_HF_OUTPUT_DEVICE;
        audio_capability->audio_stream_out.audio_volume = AUD_VOL_OUT_LEVEL6;
        bt_sink_report("[Sink][HF][AM] sco input device: 0x%x", BT_SINK_HF_INPUT_DEVICE);
    }
}


#endif /* __BT_SINK_AM_SUPPORT__ */

static bool bt_sink_hf_audio_connect(bt_sink_hf_context_t *bt_sink_hf_context_p, bt_sink_hf_audio_type_t audio_type)
{
    bool result = false;
#ifdef __BT_SINK_AM_SUPPORT__
    uint8_t current_id;
    bt_sink_am_audio_capability_t audio_capability;

    bt_sink_assert(audio_type < BT_SINK_HF_AUDIO_TOTAL && bt_sink_hf_context_p);
#ifdef __BT_SINK_DEBUG_INFO__
    bt_sink_report("[Sink][HF][Audio] Connect %s", sink_hf_audio_type[audio_type]);
#else
    bt_sink_report("[Sink][HF][Audio] Connect 0x%x", audio_type);
#endif /* __BT_SINK_DEBUG_INFO__ */

    bt_sink_memset(&audio_capability, 0, sizeof(bt_sink_am_audio_capability_t));

    if (bt_sink_hf_audio_get_audio_id(BT_SINK_HF_AUDIO_CALL, &current_id)) {
        if (BT_SINK_HF_AUDIO_CALL == audio_type) {
            bt_sink_hf_context_p->audio_type |= (1 << audio_type);
            bt_sink_hf_context_p->audio_id[audio_type] = current_id;
        } else {
            if (bt_sink_hf_context_p == bt_sink_hf_get_highlight_device()) {
                if (bt_sink_hf_audio_get_audio_id(audio_type, &current_id)) {
                    bt_sink_hf_context_p->audio_type |= (1 << audio_type);
                    bt_sink_hf_context_p->audio_id[audio_type] = current_id;
                } else {
                    bt_sink_am_id_t audio_id = bt_sink_ami_audio_open(AUD_HIGH, bt_sink_hf_audio_am_callback);

                    if (AUD_ID_INVALID != audio_id) {
                        bt_sink_hf_context_p->audio_type |= (1 << audio_type);
                        bt_sink_hf_context_p->audio_id[audio_type] = audio_id;
                        if (BT_SINK_HF_AUDIO_SCO == audio_type) {
                            bt_sink_hf_audio_sco_parameter_init(&audio_capability, (bt_hf_codec_type_t)bt_sink_hf_context_p->codec);
                        } else {
                            bt_sink_hf_audio_pcm_parameter_init(&audio_capability);
                        }
                        bt_sink_ami_audio_play(audio_id, &audio_capability);
                        result = true;
                    } else {
                        bt_sink_report("Open fail, result:%x", audio_id);
                    }
                }
            } else {
                if (BT_SINK_HF_AUDIO_SCO == audio_type) {
                    bt_sink_hf_sco_disconnect(bt_sink_hf_context_p);
                }
            }
        }
    } else {
        if (BT_SINK_HF_AUDIO_CALL == audio_type) {
            bt_sink_am_id_t audio_id = bt_sink_ami_audio_open(AUD_HIGH, bt_sink_hf_audio_am_callback);

            if (AUD_ID_INVALID != audio_id) {
                bt_sink_hf_context_p->audio_type |= (1 << audio_type);
                bt_sink_hf_context_p->audio_id[audio_type] = audio_id;
                audio_capability.type = NONE;
                bt_sink_ami_audio_play(audio_id, &audio_capability);
                result = true;
            }
        } else {
            if (bt_sink_hf_audio_get_audio_id(audio_type, &current_id)) {
                bt_sink_hf_context_p->audio_type |= (1 << audio_type);
                bt_sink_hf_context_p->audio_id[audio_type] = current_id;
            } else {
                bt_sink_am_id_t audio_id = bt_sink_ami_audio_open(AUD_MIDDLE, bt_sink_hf_audio_am_callback);

                if (AUD_ID_INVALID != audio_id) {
                    bt_sink_hf_context_p->audio_type |= (1 << audio_type);
                    bt_sink_hf_context_p->audio_id[audio_type] = audio_id;
                    if (BT_SINK_HF_AUDIO_SCO == audio_type) {
                        bt_sink_hf_audio_sco_parameter_init(&audio_capability, (bt_hf_codec_type_t)bt_sink_hf_context_p->codec);
                    } else {
                        bt_sink_hf_audio_pcm_parameter_init(&audio_capability);
                    }
                    bt_sink_ami_audio_play(audio_id, &audio_capability);
                    result = true;
                }
            }
        }
    }
#endif /* __BT_SINK_AM_SUPPORT__ */
    bt_sink_report("[Sink][HF][Audio] Connect result:%d", result);
    return result;
}

static void bt_sink_hf_audio_terminate(bt_sink_hf_context_t *bt_sink_hf_context_p, bt_sink_hf_audio_type_t audio_type)
{
#ifdef __BT_SINK_AM_SUPPORT__
    if (NULL != bt_sink_hf_context_p && (bt_sink_hf_context_p->audio_type & (1 << audio_type))) {
#ifdef __BT_SINK_DEBUG_INFO__
        bt_sink_report("[Sink][HF][Audio] Terminate type:%s", sink_hf_audio_type[audio_type]);
#else
        bt_sink_report("[Sink][HF][Audio] Terminate type:0x%x", audio_type);
#endif /* __BT_SINK_DEBUG_INFO__ */

        bt_sink_hf_context_p->audio_type &= (~(1 << audio_type));

        if (!bt_sink_hf_audio_get_audio_id(audio_type, NULL)) {
            bt_sink_ami_audio_stop(bt_sink_hf_context_p->audio_id[audio_type]);
            bt_sink_ami_audio_close(bt_sink_hf_context_p->audio_id[audio_type]);
        }
        bt_sink_hf_context_p->audio_id[audio_type] = BT_SINK_HF_INVALID_AUDIO_ID;
    }
#endif /* __BT_SINK_AM_SUPPORT__ */
}

static void bt_sink_hf_audio_disconnect(bt_sink_hf_context_t *bt_sink_hf_context_p, bt_sink_hf_audio_type_t audio_type)
{
    bt_sink_assert(audio_type < BT_SINK_HF_AUDIO_TOTAL && bt_sink_hf_context_p);

#ifdef __BT_SINK_DEBUG_INFO__
    bt_sink_report("[Sink][HF][Audio] Disconnect type:%s", sink_hf_audio_type[audio_type]);
#else
    bt_sink_report("[Sink][HF][Audio] Disconnect type:0x%x", audio_type);
#endif /* __BT_SINK_DEBUG_INFO__ */

    bt_sink_hf_audio_terminate(bt_sink_hf_context_p, audio_type);

    if (BT_SINK_HF_AUDIO_CALL == audio_type) {
        if (!bt_sink_hf_audio_get_audio_id(BT_SINK_HF_AUDIO_CALL, NULL)) {
            bt_sink_hf_audio_terminate(bt_sink_hf_context_p, BT_SINK_HF_AUDIO_RING);
            bt_sink_hf_audio_terminate(bt_sink_hf_context_p, BT_SINK_HF_AUDIO_SCO);
        }
    }
}

uint8_t bt_sink_hf_volume_to_bt(uint8_t local_volume)
{
    uint8_t sink_hf_vol_mapping[7] = {0, 3, 6, 9, 11, 13, 15};

    bt_sink_assert(7 > local_volume);
    return sink_hf_vol_mapping[local_volume];
}

uint8_t bt_sink_hf_volume_to_local(uint8_t bt_volume)
{
    uint8_t sink_hf_vol_mapping[16] = {AUD_VOL_OUT_LEVEL0,
                                       AUD_VOL_OUT_LEVEL1,
                                       AUD_VOL_OUT_LEVEL1,
                                       AUD_VOL_OUT_LEVEL2,
                                       AUD_VOL_OUT_LEVEL2,
                                       AUD_VOL_OUT_LEVEL3,
                                       AUD_VOL_OUT_LEVEL3,
                                       AUD_VOL_OUT_LEVEL3,
                                       AUD_VOL_OUT_LEVEL4,
                                       AUD_VOL_OUT_LEVEL4,
                                       AUD_VOL_OUT_LEVEL4,
                                       AUD_VOL_OUT_LEVEL5,
                                       AUD_VOL_OUT_LEVEL5,
                                       AUD_VOL_OUT_LEVEL5,
                                       AUD_VOL_OUT_LEVEL6,
                                       AUD_VOL_OUT_LEVEL6
                                      };

    bt_sink_assert(15 >= bt_volume);
    return sink_hf_vol_mapping[bt_volume];
}


static void bt_sink_hf_handle_sco_connected_ind(bt_hf_audio_connect_ind_t *message)
{
    LOG_I(bt, "BT_HF_AUDIO_CONNECT_IND, codec:%d", message->codec);

    if (bt_sink_sco_loopback) {
#ifdef __BT_SINK_SCO_LOOPBACK__
        bt_hf_loopback_t loopback;

        loopback.type = BT_HF_CODEC_CVSD;
        loopback.mode = BT_HF_LOOPBACK_WITHOUT_CODEC;
        sink_hf_media_handle = bt_hf_open_loopback(&loopback);

        bt_sink_report(bt, "[Sink][HF] sink_hf_media_handle:%x", sink_hf_media_handle);

        if (NULL != sink_hf_media_handle) {
            if (BT_STATUS_SUCCESS != sink_hf_media_handle->play(sink_hf_media_handle)) {
                bt_sink_report("[Sink][HF] play error");
            }
        }
        bt_sink_report("[Sink][HF] loopback on, hd:%x", sink_hf_media_handle);
#endif /* __BT_SINK_SCO_LOOPBACK__ */
    } else {
        bt_sink_hf_sco_state_change_t *sco_change;

        sco_change = bt_sink_memory_alloc(sizeof(*sco_change));
        sco_change->conn_id = message->conn_id;
        sco_change->connected = true;
        sco_change->codec = message->codec;
        bt_sink_event_send(BT_SINK_EVENT_HF_AUDIO_STATE_CHANGE_SYNC, sco_change);
        bt_sink_event_post(BT_SINK_EVENT_HF_AUDIO_STATE_CHANGE,
                           sco_change,
                           bt_sink_hf_post_callback);
    }
}

static void bt_sink_hf_handle_sco_disconnected_ind(bt_hf_audio_disconnect_ind_t *message)
{
    if (bt_sink_sco_loopback) {
#ifdef __BT_SINK_SCO_LOOPBACK__
        if (NULL != sink_hf_media_handle) {
            if (BT_STATUS_SUCCESS == sink_hf_media_handle->stop(sink_hf_media_handle)) {
                if (BT_STATUS_SUCCESS == bt_hf_close_loopback()) {
                    sink_hf_media_handle = NULL;
                } else {
                    bt_sink_report("[Sink][HF] close error");
                }
            } else {
                bt_sink_report("[Sink][HF] stop error");
            }
        }
        bt_sink_report("[Sink][HF] loopback off, hd:%x", sink_hf_media_handle);
#endif /* __BT_SINK_SCO_LOOPBACK__ */
    } else {
        bt_sink_hf_sco_state_change_t *sco_change;

        sco_change = bt_sink_memory_alloc(sizeof(*sco_change));
        sco_change->conn_id = message->conn_id;
        sco_change->connected = false;
        bt_sink_event_send(BT_SINK_EVENT_HF_AUDIO_STATE_CHANGE_SYNC, sco_change);
        bt_sink_event_post(BT_SINK_EVENT_HF_AUDIO_STATE_CHANGE,
                           sco_change,
                           bt_sink_hf_post_callback);
    }
}

static void bt_sink_hf_handle_ring_ind(bt_hf_ring_ind_t *message)
{
    bt_sink_hf_ringtone_t *ringtone = bt_sink_memory_alloc(sizeof(*ringtone));

    ringtone->conn_id = message->conn_id;

    bt_sink_event_send(BT_SINK_EVENT_HF_RING_TONE_SYNC, ringtone);
    bt_sink_event_post(BT_SINK_EVENT_HF_RING_TONE,
                       ringtone,
                       bt_sink_hf_post_callback);
}

bool bt_sink_hf_volume_change(bt_sink_hf_volume_act_t vol_act, bool min_max)
{
    bool result = false;
    bt_sink_speaker_volume_change_t volume_change;
    bt_sink_hf_context_t *bt_sink_hf_context_p = bt_sink_hf_get_highlight_device();

    if (NULL != bt_sink_hf_context_p &&
            bt_sink_hf_context_p->call_state > BT_SINK_HF_CALL_STATE_IDLE) {
        bt_sink_hf_stored_data_t stored_data;

        bt_sink_cm_get_profile_data(bt_sink_hf_context_p->conn_id, BT_SINK_UUID_HF, &stored_data);
        volume_change.conn_id = bt_sink_hf_context_p->conn_id;

        if (BT_SINK_HF_VOL_ACT_UP == vol_act) {
            if (min_max) {
                bt_sink_status_t result;
                volume_change.volume = (uint8_t)BT_SINK_HF_MAX_VOLUME;
                result = bt_sink_event_send(BT_SINK_EVENT_HF_SPEAKER_VOLUME_CHANGE, &volume_change);
                bt_sink_report("[Sink][HF] result:%d", result);
                bt_hf_sync_speaker_gain(bt_sink_hf_context_p->conn_id, bt_sink_hf_volume_to_bt(volume_change.volume));
            } else {
                if (BT_SINK_HF_MAX_VOLUME > stored_data.speaker_volume) {
                    volume_change.volume = stored_data.speaker_volume + 1;
                    bt_sink_event_send(BT_SINK_EVENT_HF_SPEAKER_VOLUME_CHANGE, &volume_change);
                    bt_hf_sync_speaker_gain(bt_sink_hf_context_p->conn_id, bt_sink_hf_volume_to_bt(volume_change.volume));
                }
            }
            result = true;
        } else if (BT_SINK_HF_VOL_ACT_DOWN == vol_act) {
            if (min_max) {
                volume_change.volume = (uint8_t)BT_SINK_HF_MIN_VOLUME;
                bt_sink_event_send(BT_SINK_EVENT_HF_SPEAKER_VOLUME_CHANGE, &volume_change);
                bt_hf_sync_speaker_gain(bt_sink_hf_context_p->conn_id, bt_sink_hf_volume_to_bt((volume_change.volume)));
            } else {
                if (BT_SINK_HF_MIN_VOLUME < stored_data.speaker_volume) {
                    volume_change.volume = stored_data.speaker_volume - 1;
                    bt_sink_event_send(BT_SINK_EVENT_HF_SPEAKER_VOLUME_CHANGE, &volume_change);
                    bt_hf_sync_speaker_gain(bt_sink_hf_context_p->conn_id, bt_sink_hf_volume_to_bt(volume_change.volume));
                }
            }
            result = true;
        }
    }
    return result;
}

void bt_sink_hf_common_callback(bt_event_t event_id, const void *parameters)
{
    bt_sink_profile_t *bt_sink_hf_p = NULL;
    bt_sink_hf_context_t *bt_sink_hf_context_p = NULL;

#ifdef __BT_SINK_DEBUG_INFO__
    if (event_id >= BT_HF_SLC_CONNECT_IND && event_id <= BT_HF_EXTERN_AT_CMD_CNF) {
        bt_sink_report("[Sink][HF] SDK event:%s", g_sink_hf_event_string[event_id - BT_HF_SLC_CONNECT_IND]);
    } else {
        bt_sink_report("[Sink][HF] SDK event:0x%x", event_id);
    }
#else
    bt_sink_report("[Sink][HF] SDK event:0x%x", event_id);
#endif /* __BT_SINK_DEBUG_INFO__ */

    switch (event_id) {
        /* SDK event */
        case BT_HF_SLC_CONNECT_IND: {
            bt_hf_service_connect_ind_t *message = (bt_hf_service_connect_ind_t *)parameters;

            bt_sink_report("CONNECT_IND, conn_id:%d", message->conn_id);

            bt_hf_set_caller_id(message->conn_id, true);
            bt_hf_set_call_waiting(message->conn_id, true);

            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(message->conn_id, BT_SINK_UUID_HF);

            if (NULL != bt_sink_hf_p) {
                bt_sink_hf_stored_data_t stored_data;
                bt_sink_hf_connect_state_change_t *connect_change;;

                bt_sink_hf_p->state = BT_SINK_PROFILE_STATE_CONNECTED;
                bt_sink_hf_context_p = (bt_sink_hf_context_t *)bt_sink_hf_p->data;
                bt_sink_hf_context_p->conn_id = message->conn_id;

                connect_change = bt_sink_memory_alloc(sizeof(*connect_change));
                connect_change->conn_id = message->conn_id;
                connect_change->connected = true;
                bt_sink_memcpy((void *)&connect_change->bt_addr, &message->dev_addr, sizeof(bt_address_t));
                bt_sink_event_post(BT_SINK_EVENT_HF_CONNECT_STATE_CHANGE,
                                   connect_change,
                                   bt_sink_hf_post_callback);

                if (bt_sink_cm_get_profile_data(message->conn_id, BT_SINK_UUID_HF, &stored_data)) {
                    if (stored_data.speaker_volume == AUD_VOL_OUT_LEVEL0) {
                        stored_data.speaker_volume = BT_SINK_HF_DEFAULT_VOLUME;
                        bt_sink_cm_set_profile_data(message->conn_id, BT_SINK_UUID_HF, &stored_data);
                    }
                }
            }
        }
        break;

        case BT_HF_CONNECT_REQUEST_IND: {
            bt_hf_connect_response(&((bt_hf_connect_request_ind_t *)parameters)->dev_addr, true);
        }
        break;

        case BT_HF_SLC_DISCONNECT_IND: {
            bt_hf_disconnect_ind_t *message = (bt_hf_disconnect_ind_t *)parameters;

            bt_sink_report("DISCONNECT_IND, conn_id:%d", message->conn_id);

            bt_sink_hf_p = bt_sink_cm_find_profile_by_address(&message->dev_addr, BT_SINK_UUID_HF);

            if (bt_sink_hf_p) {
                bt_sink_hf_connect_state_change_t *connect_change;
                bt_sink_hf_context_p = (bt_sink_hf_context_t *)bt_sink_hf_p->data;

                bt_sink_hf_audio_disconnect(bt_sink_hf_context_p, BT_SINK_HF_AUDIO_CALL);

                bt_sink_memset(bt_sink_hf_context_p, 0, sizeof(bt_sink_hf_context_t));

                if (bt_sink_hf_get_highlight_device() == bt_sink_hf_context_p) {
                    bt_sink_hf_mp_state_change(bt_sink_hf_context_p);
                }

                bt_sink_hf_p->state = BT_SINK_PROFILE_STATE_DISCONNECTED;
                connect_change = bt_sink_memory_alloc(sizeof(*connect_change));
                connect_change->conn_id = message->conn_id;
                connect_change->connected = false;
                bt_sink_memcpy((void *)&connect_change->bt_addr, &message->dev_addr, sizeof(bt_address_t));
                bt_sink_event_post(BT_SINK_EVENT_HF_CONNECT_STATE_CHANGE,
                                   connect_change,
                                   bt_sink_hf_post_callback);
            }
        }
        break;

        case BT_HF_BRSF_FEATURES_IND: {
            bt_hf_brsf_features_ind_t *message = (bt_hf_brsf_features_ind_t *)parameters;

            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(message->conn_id, BT_SINK_UUID_HF);

            bt_sink_report("[Sink][HF] cntx:%x, conn_id:%d", bt_sink_hf_p, message->conn_id);

            if (bt_sink_hf_p) {
                bt_sink_hf_context_p = (bt_sink_hf_context_t *)bt_sink_hf_p->data;
                bt_sink_hf_context_p->conn_id = message->conn_id;
                bt_sink_hf_context_p->ag_featues = message->ag;
            }
        }
        break;

        case BT_HF_CALL_HELD_FEATURES_IND: {
            bt_hf_hold_features_ind_t *message = (bt_hf_hold_features_ind_t *)parameters;

            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(message->conn_id, BT_SINK_UUID_HF);

            if (bt_sink_hf_p) {
                bt_sink_hf_context_p = (bt_sink_hf_context_t *)bt_sink_hf_p->data;
                bt_sink_hf_context_p->ag_chld_feature = message->feature;
            }
        }
        break;

        case BT_HF_AUDIO_CONNECT_IND: {
            bt_sink_hf_handle_sco_connected_ind((bt_hf_audio_connect_ind_t *)parameters);
        }
        break;

        case BT_HF_AUDIO_DISCONNECT_IND: {
            bt_sink_hf_handle_sco_disconnected_ind((bt_hf_audio_disconnect_ind_t *)parameters);
        }
        break;

        case BT_HF_CIEV_CALL_SETUP_IND: {
            bt_hf_call_setup_ind_t *callsetup = (bt_hf_call_setup_ind_t *)parameters;

            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(callsetup->conn_id, BT_SINK_UUID_HF);

            if (NULL != bt_sink_hf_p) {
                bt_sink_hf_handle_setup_ind((bt_sink_hf_context_t *)bt_sink_hf_p->data, callsetup->state);
            }
        }
        break;

        case BT_HF_CIEV_CALL_IND: {
            bt_hf_call_ind_t *call = (bt_hf_call_ind_t *)parameters;

            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(call->conn_id, BT_SINK_UUID_HF);

            if (NULL != bt_sink_hf_p) {
                bt_sink_hf_handle_call_ind((bt_sink_hf_context_t *)bt_sink_hf_p->data, call->state);
            }
        }

        break;

        case BT_HF_CIEV_CALL_HELD_IND: {
            bt_hf_call_held_ind_t *callheld = (bt_hf_call_held_ind_t *)parameters;

            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(callheld->conn_id, BT_SINK_UUID_HF);

            if (NULL != bt_sink_hf_p) {
                bt_sink_hf_handle_held_ind((bt_sink_hf_context_t *)bt_sink_hf_p->data, callheld->state);
            }
        }
        break;

        case BT_HF_CURRENT_CALLS_IND: {
            bt_hf_call_list_ind_t *message = (bt_hf_call_list_ind_t *)parameters;

            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(message->conn_id, BT_SINK_UUID_HF);

            if (NULL != bt_sink_hf_p) {
                bt_sink_hf_handle_call_info_ind((bt_sink_hf_context_t *)bt_sink_hf_p->data, message);
            }
        }
        break;

        case BT_HF_RING_IND: {
            bt_sink_hf_handle_ring_ind((bt_hf_ring_ind_t *)parameters);
        }
        break;

        case BT_HF_CALLER_ID_IND: {
            bt_hf_caller_id_ind_t *message = (bt_hf_caller_id_ind_t *)parameters;
            bt_sink_caller_information_t *caller_info;

            caller_info = bt_sink_memory_alloc(sizeof(*caller_info));

            if (message && caller_info) {
                caller_info->conn_id = message->conn_id;
                caller_info->num_size = message->num_size;
                caller_info->type = message->type;
                caller_info->waiting = false;
                bt_sink_memcpy(caller_info->number, message->number, sizeof(caller_info->number));
                bt_sink_event_post(BT_SINK_EVENT_HF_CALLER_INFORMATION,
                                   caller_info,
                                   bt_sink_hf_post_callback);
            }
        }
        break;

        case BT_HF_CALL_WAITING_IND: {
            bt_hf_call_waiting_ind_t *message = (bt_hf_call_waiting_ind_t *)parameters;
            bt_sink_caller_information_t *caller_info;

            caller_info = bt_sink_memory_alloc(sizeof(*caller_info));

            if (message && caller_info) {
                caller_info->conn_id = message->conn_id;
                caller_info->num_size = message->num_size;
                caller_info->type = message->type;
                caller_info->waiting = true;
                bt_sink_memcpy(caller_info->number, message->number, sizeof(caller_info->number));
                bt_sink_event_post(BT_SINK_EVENT_HF_CALLER_INFORMATION,
                                   caller_info,
                                   bt_sink_hf_post_callback);
            }
        }
        break;

        case BT_HF_VOLUME_SYNC_SPEAKER_GAIN_IND: {
            bt_hf_volume_sync_speaker_gain_ind_t *message = (bt_hf_volume_sync_speaker_gain_ind_t *)parameters;
            bt_sink_speaker_volume_change_t volume_change;

            volume_change.conn_id = message->conn_id;
            volume_change.volume = bt_sink_hf_volume_to_local((uint8_t)message->data);
            bt_sink_event_send(BT_SINK_EVENT_HF_SPEAKER_VOLUME_CHANGE, &volume_change);
        }
        break;

        case BT_HF_IN_BAND_RING_IND: {
            bt_hf_in_band_ring_status_ind_t *message = (bt_hf_in_band_ring_status_ind_t *)parameters;

            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(message->conn_id, BT_SINK_UUID_HF);

            if (NULL != bt_sink_hf_p) {
                bt_sink_hf_context_p = (bt_sink_hf_context_t *)bt_sink_hf_p->data;

                if (message->enable) {
                    bt_sink_hf_context_p->ag_featues |= BT_HF_AG_FEATURE_IN_BAND_RING;
                } else {
                    bt_sink_hf_context_p->ag_featues &= (~BT_HF_AG_FEATURE_IN_BAND_RING);
                }
            }
        }
        break;

        default:
            break;
    }
}

bt_sink_status_t bt_sink_hf_event_handler(bt_sink_event_id_t event_id, void *parameters)
{
    bt_sink_status_t result = BT_SINK_STATUS_SUCCESS;
    bt_sink_profile_t *bt_sink_hf_p = NULL;
    bt_sink_hf_context_t *bt_sink_hf_context_p = NULL;

    bt_sink_report("[Sink][HF] bt_sink_hf_event_handler, event_id:%x", event_id);

    switch (event_id) {
        /* CM event */
        case BT_SINK_EVENT_CM_PROFILE_INIT: {
            uint32_t i;

            bt_sink_memset(bt_sink_hf_context, 0, sizeof(bt_sink_hf_context));

            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; i++) {
                bt_sink_hf[i].uuid = BT_SINK_UUID_HF;
                bt_sink_hf[i].state = BT_SINK_PROFILE_STATE_DISCONNECTED;
                bt_sink_hf[i].data = &bt_sink_hf_context[i];
                bt_sink_memset((void *)bt_sink_hf_context[i].audio_id,
                               BT_SINK_HF_INVALID_AUDIO_ID,
                               BT_SINK_HF_AUDIO_TOTAL);
            }
        }
        break;

        case BT_SINK_EVENT_CM_PROFILE_DEINIT: {
            uint32_t i;
            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; i++) {
                bt_sink_hf_audio_disconnect(&bt_sink_hf_context[i], BT_SINK_HF_AUDIO_CALL);
            }
        }
        break;

        case BT_SINK_EVENT_CM_PROFILE_CONNECT_REQ: {
            bt_sink_hf_p = bt_sink_cm_find_profile_by_address((bt_address_t *)parameters, BT_SINK_UUID_HF);

            if (NULL != bt_sink_hf_p) {
                bt_sink_hf_p->state = BT_SINK_PROFILE_STATE_CONNECTING;
            }
            bt_hf_connect((bt_address_t *)parameters);
        }
        break;

        case BT_SINK_EVENT_CM_PROFILE_DISCONNECT_REQ: {
            bt_hf_disconnect(*((uint16_t *)parameters));
        }
        break;

        /* System Event */
        case BT_SINK_EVENT_HF_CALL_STATE_CHANGE_SYNC: {
            bt_sink_hf_call_state_change_t *message = (bt_sink_hf_call_state_change_t *)parameters;
#ifdef __BT_SINK_DEBUG_INFO__
            uint32_t index, previous = 0, now = 0;

            for (index = 0; index < 11; index++) {
                if ((1 << index) & message->previous_state) {
                    previous = index + 1;
                } else if ((1 << index) & message->new_state) {
                    now = index + 1;
                }
            }
            bt_sink_report("[Sink][HF] Call state change, conn_id:%d, prev:%s, new:%s",
                           message->conn_id, g_sink_hf_call_state[previous], g_sink_hf_call_state[now]);
#endif /* __BT_SINK_DEBUG_INFO__ */
            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(message->conn_id, BT_SINK_UUID_HF);

            if (NULL != bt_sink_hf_p) {
                bt_sink_hf_context_p = (bt_sink_hf_context_t *)bt_sink_hf_p->data;

                if (BT_SINK_HF_CALL_STATE_IDLE != message->previous_state &&
                        BT_SINK_HF_CALL_STATE_IDLE == message->new_state) {
                    bt_sink_hf_audio_disconnect(bt_sink_hf_context_p, BT_SINK_HF_AUDIO_CALL);
                } else if (BT_SINK_HF_CALL_STATE_IDLE == message->previous_state &&
                           BT_SINK_HF_CALL_STATE_IDLE != message->new_state) {
                    bt_sink_hf_audio_connect(bt_sink_hf_context_p, BT_SINK_HF_AUDIO_CALL);
                } else if (BT_SINK_HF_CALL_STATE_INCOMING == message->previous_state &&
                           BT_SINK_HF_CALL_STATE_INCOMING != message->new_state) {
                    bt_sink_hf_audio_disconnect(bt_sink_hf_context_p, BT_SINK_HF_AUDIO_RING);
                }
            }

            if (BT_SINK_HF_CALL_STATE_INCOMING == message->new_state
                    && BT_SINK_HF_CALL_STATE_IDLE == message->previous_state) {
                bt_sink_event_post(BT_SINK_EVENT_HF_ANSWER, NULL, NULL);
            }
        }
        break;

        case BT_SINK_EVENT_HF_AUDIO_STATE_CHANGE_SYNC: {
            bt_sink_hf_sco_state_change_t *sco_change = (bt_sink_hf_sco_state_change_t *)parameters;

            if (sco_change) {
                bt_sink_hf_p = bt_sink_cm_find_profile_by_id(sco_change->conn_id, BT_SINK_UUID_HF);

                if (bt_sink_hf_p) {
                    bt_sink_hf_context_p = (bt_sink_hf_context_t *)bt_sink_hf_p->data;

                    if (sco_change->connected) {
                        bt_sink_hf_context_p->codec = sco_change->codec;

                        if (bt_sink_hf_audio_connect(bt_sink_hf_context_p, BT_SINK_HF_AUDIO_SCO)) {
                            bt_sink_hf_p->state = BT_SINK_PROFILE_STATE_AUDIO_ON;
                        }
                    } else {
                        bt_sink_hf_p->state = BT_SINK_PROFILE_STATE_CONNECTED;
                        bt_sink_hf_context_p->codec = BT_HF_WBS_CODEC_MASK_NONE;
                        bt_sink_hf_audio_disconnect(bt_sink_hf_context_p, BT_SINK_HF_AUDIO_SCO);
                    }
                }
            }
        }
        break;

        case BT_SINK_EVENT_HF_RING_TONE_SYNC: {
            bt_sink_hf_ringtone_t *ringtone = (bt_sink_hf_ringtone_t *)parameters;
            bt_sink_hf_p = bt_sink_cm_find_profile_by_id(ringtone->conn_id, BT_SINK_UUID_HF);

            if (NULL != bt_sink_hf_p) {
                bt_sink_hf_context_p = (bt_sink_hf_context_t *)bt_sink_hf_p->data;

                // Don't support in-band ringtone, need to play local ringtone.
                if (!(bt_sink_hf_context_p->ag_featues & BT_HF_AG_FEATURE_IN_BAND_RING)) {
                    if (bt_sink_hf_context_p == bt_sink_hf_get_highlight_device()) {
                        uint8_t current_id;

                        if (bt_sink_hf_audio_get_audio_id(BT_SINK_HF_AUDIO_RING, &current_id)) {
                            bt_sink_ami_audio_continue_stream(current_id, bt_sink_hf_tone_16ksr, sizeof(bt_sink_hf_tone_16ksr));
                        } else {
                            // bt_sink_hf_audio_connect(bt_sink_hf_context_p, BT_SINK_HF_AUDIO_RING);
                        }
                    }
                }
            }
        }
        break;

        case BT_SINK_EVENT_HF_SPEAKER_VOLUME_CHANGE: {
            bt_sink_hf_stored_data_t stored_data;
            bt_sink_speaker_volume_change_t *volume_change = (bt_sink_speaker_volume_change_t *)parameters;

            if (bt_sink_cm_get_profile_data(volume_change->conn_id, BT_SINK_UUID_HF, &stored_data)
                    && stored_data.speaker_volume != volume_change->volume) {

                stored_data.speaker_volume = volume_change->volume;
                bt_sink_cm_set_profile_data(volume_change->conn_id, BT_SINK_UUID_HF, &stored_data);
                bt_sink_report("[Sink][HF] Volume Change: conn_id:%d, volume:%d", volume_change->conn_id, volume_change->volume);
                bt_sink_hf_p = bt_sink_cm_find_profile_by_id(volume_change->conn_id, BT_SINK_UUID_HF);
                if (NULL != bt_sink_hf_p) {
                    bt_sink_hf_context_p = (bt_sink_hf_context_t *)bt_sink_hf_p->data;
                    if (NULL != bt_sink_hf_context_p) {
                        if (bt_sink_hf_context_p->audio_type & (1 << BT_SINK_HF_AUDIO_SCO)) {
                            if (BT_SINK_HF_INVALID_AUDIO_ID != bt_sink_hf_context_p->audio_id[BT_SINK_HF_AUDIO_SCO]) {
                                bt_sink_ami_audio_set_volume(bt_sink_hf_context_p->audio_id[BT_SINK_HF_AUDIO_SCO],
                                                             volume_change->volume, STREAM_OUT);
                            }
                        } else if (bt_sink_hf_context_p->audio_type & (1 << BT_SINK_HF_AUDIO_RING)) {
                            if (BT_SINK_HF_INVALID_AUDIO_ID != bt_sink_hf_context_p->audio_id[BT_SINK_HF_AUDIO_RING]) {
                                bt_sink_ami_audio_set_volume(bt_sink_hf_context_p->audio_id[BT_SINK_HF_AUDIO_RING],
                                                             volume_change->volume, STREAM_OUT);
                            }
                        }
                    }
                }
            }
        }
        break;

        /* User Event */
        case BT_SINK_EVENT_HF_ANSWER: {
            bt_sink_hf_answer_call(true);
        }
        break;

        case BT_SINK_EVENT_HF_REJECT: {
            bt_sink_hf_answer_call(false);
        }
        break;

        case BT_SINK_EVENT_HF_HANG_UP: {
            bt_sink_hf_terminate_call();
        }
        break;

        case BT_SINK_EVENT_HF_DIAL_LAST: {
            bt_sink_hf_dial_last();
        }
        break;

        case BT_SINK_EVENT_HF_3WAY_RELEASE_ALL_HELD: {
            bt_sink_hf_release_all_held_call();
        }
        break;

        case BT_SINK_EVENT_HF_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER: {
            bt_sink_hf_release_all_active_accept_others();
        }
        break;

        case BT_SINK_EVENT_HF_3WAY_HOLD_ACTIVE_ACCEPT_OTHER: {
            bt_sink_hf_hold_all_active_accept_others();
        }
        break;

        case BT_SINK_EVENT_HF_SWITCH_AUDIO_PATH: {
            bt_sink_hf_switch_audio_path();
        }
        break;

        case BT_SINK_EVENT_VOLUME_UP: {
            if (bt_sink_hf_volume_change(BT_SINK_HF_VOL_ACT_UP, false)) {
                result = BT_SINK_STATUS_EVENT_STOP;
            }
        }
        break;

        case BT_SINK_EVENT_VOLUME_DOWN: {
            if (bt_sink_hf_volume_change(BT_SINK_HF_VOL_ACT_DOWN, false)) {
                result = BT_SINK_STATUS_EVENT_STOP;
            }
        }
        break;

        case BT_SINK_EVENT_VOLUME_MAX: {
            if (bt_sink_hf_volume_change(BT_SINK_HF_VOL_ACT_UP, true)) {
                result = BT_SINK_STATUS_EVENT_STOP;
            }
        }
        break;

        case BT_SINK_EVENT_VOLUME_MIN: {
            if (bt_sink_hf_volume_change(BT_SINK_HF_VOL_ACT_DOWN, true)) {
                result = BT_SINK_STATUS_EVENT_STOP;
            }
        }
        break;

        default:
            bt_sink_report("[Sink][HF] Unexcepted event_id:%x", event_id);
            break;
    }
    return result;
}

#ifdef __BT_SINK_HF_INIT_PARAMS_SWITCH__
bt_sink_status_t bt_sink_hf_set_init_params(uint8_t index)
{
    bt_sink_status_t result;

    if (index >= BT_SINK_HF_MAX_PARAMS_INDEX) {
        result = BT_SINK_STATUS_INVALID_PARAM;
    } else {
        nvdm_status_t nvdm_result =
            nvdm_write_data_item("BT_SINK",
                                 "hf_params",
                                 NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                 &index,
                                 sizeof(index));

        result = (bt_sink_status_t)nvdm_result;
        bt_sink_report("write hfp init params index:%d, result:%d", index, result);
    }
    return result;
}
#endif /* __BT_SINK_HF_INIT_PARAMS_SWITCH__ */

bt_status_t bt_sink_hf_get_init_params(bt_hf_init_params_t *param, char *extra_indicators)
{
#ifdef __BT_SINK_HF_INIT_PARAMS_SWITCH__
    nvdm_status_t status;
    uint8_t index;
    uint32_t size = sizeof(index);
#endif /* __BT_SINK_HF_INIT_PARAMS_SWITCH__ */

    param->disable_nrec = true;
    param->extended_errors = true;

    param->optional_indicators.battery = BT_HF_INDICATOR_OFF;
    param->optional_indicators.roaming = BT_HF_INDICATOR_OFF;
    param->optional_indicators.service = BT_HF_INDICATOR_OFF;
    param->optional_indicators.signal = BT_HF_INDICATOR_OFF;

#ifdef __BT_SINK_HF_INIT_PARAMS_SWITCH__
    status = nvdm_read_data_item("BT_SINK", "hf_params", &index, &size);

    bt_sink_report("read hf params result:%d, index:%d", status, index);

    if (NVDM_STATUS_OK != status || index >= BT_SINK_HF_MAX_PARAMS_INDEX) {
        index = 2; // default == HFP v1.6, mSBC
    }
    bt_sink_report("hf params index:%d", index);

    param->supported_features = bt_sink_hf_init_swtich_params[index].support_features;
    param->supported_profile = bt_sink_hf_init_swtich_params[index].support_profile;
    param->supported_wbs_codecs = bt_sink_hf_init_swtich_params[index].support_codec;
#else
    param->supported_features = BT_HF_FEATURE_CALL_WAITING | BT_HF_FEATURE_CLI_PRESENTATION | BT_HF_FEATURE_VOICE_RECOGNITION | BT_HF_FEATURE_ECHO_NOISE | BT_HF_FEATURE_VOLUME_CONTROL
                                | BT_HF_FEATURE_ENHANCED_CALL_STATUS | BT_HF_FEATURE_CODEC_NEGOTIATION;
    param->supported_profile = BT_HF_PROFILE_V16;
    param->supported_wbs_codecs = (BT_HF_WBS_CODEC_MASK_CVSD | BT_HF_WBS_CODEC_MASK_MSBC);
#endif

    return BT_STATUS_SUCCESS;
}

