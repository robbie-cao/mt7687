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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "bt_sink_ami.h"

const uint32_t g_volume_out_config[DEVICE_OUT_MAX][AUD_VOL_TYPE][AUD_VOL_OUT_MAX][2] = {  {{{0xFFFFFC18, 0x044C}, {0xFFFFFCE0, 0x044C}, {0xFFFFFDA8, 0x044C}, {0xFFFFFE70, 0x044C}, {0xFFFFFF38, 0x044C}, {0x00000000, 0x044C}, {0x00000000, 0x0384}},
        {{0xFFFFFC18, 0x044C}, {0xFFFFFCE0, 0x044C}, {0xFFFFFDA8, 0x044C}, {0xFFFFFE70, 0x044C}, {0xFFFFFF38, 0x044C}, {0x00000000, 0x044C}, {0x00000000, 0x0384}}
    },
    {   {{0xFFFFF768, 0x0000}, {0xFFFFF8F8, 0x0000}, {0xFFFFFA88, 0x0000}, {0xFFFFFC18, 0x0000}, {0xFFFFFDA8, 0x0000}, {0xFFFFFF38, 0x0000}, {0x00000000, 0x00C8}},
        {{0xFFFFF768, 0x0000}, {0xFFFFF8F8, 0x0000}, {0xFFFFFA88, 0x0000}, {0xFFFFFC18, 0x0000}, {0xFFFFFDA8, 0x0000}, {0xFFFFFF38, 0x0000}, {0x00000000, 0x00C8}}
    }
};
const uint32_t g_volume_in_config[DEVICE_IN_MAX][AUD_VOL_IN_MAX][2] = {{{0x00000000, 0x0258}}};
bt_sink_am_amm_struct *ptr_callback_amm = NULL;
bt_sink_am_aud_id_type_t g_rAm_aud_id[AM_REGISTER_ID_TOTAL];
static bt_sink_am_background_t *g_prCurrent_resumer = NULL;
static uint8_t *g_bBT_Ringbuf = NULL;
#ifdef RTOS_TIMER
static TimerHandle_t g_xTimer_am;
static uint16_t g_lExpire_count = 0;
#endif
static bt_sink_am_background_t g_rBackground_head = {0};
static bt_sink_am_sink_state_t g_rSink_state = A2DP_SINK_CODEC_CLOSE;
static bt_sink_am_media_handle_t g_prA2dp_sink_event_handle;
static bt_media_handle_t *g_prHfp_media_handle = NULL;
static bt_media_handle_t *g_prA2dp_sink_handle = NULL;
static uint16_t *aud_memory = NULL;
static uint8_t g_bt_sink_am_ring_buffer[AM_RING_BUFFER_SIZE];

static void am_receive_msg(bt_sink_am_amm_struct *amm_ptr);
static void bt_codec_am_a2dp_sink_open(bt_sink_am_a2dp_codec_t *a2dp_codec_t);
static bt_status_t bt_codec_am_a2dp_sink_play(bt_sink_am_id_t aud_id);
static bt_status_t bt_codec_am_a2dp_sink_stop(bt_sink_am_id_t aud_id);
static bt_status_t bt_codec_am_a2dp_sink_close(void);
static void bt_codec_am_hfp_open(bt_sink_am_hfp_codec_t *hfp_codec_t);
static bt_status_t bt_codec_am_hfp_stop(void);

#ifndef WIN32_UT
/*****************************************************************************
 * FUNCTION
 *  am_task_create
 * DESCRIPTION
 *  This function is used to create a task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void am_task_create(void)
{
    xTaskCreate(am_task_main,
                "AudioManager",
                AM_TASK_STACK_SIZE,
                NULL,
                AM_TASK_PRIORITY,
                NULL);
    // vTaskStartScheduler();
#if 0
    // Use the handle to delete the task.
    if (xHandle != NULL) {
        vTaskDelete(xHandle);
    }
#endif
}
#endif

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_play
 * DESCRIPTION
 *  This function is used to play the audio handler.
 * PARAMETERS
 *	background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
bt_sink_am_hal_result_t aud_set_volume_level(bt_sink_am_stream_type_t in_out,
        bt_sink_am_volume_type_t vol_type,
        bt_sink_am_device_set_t device,
        bt_sink_am_volume_level_t level)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_hal_result_t eResult = HAL_AUDIO_STATUS_ERROR;
    bt_sink_am_device_out_t dev_out;
    bt_sink_am_device_in_t dev_in;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_sink_report("[Sink][AM]in_out:%d, level:%d, device:0x%x", in_out, level, device);

    if (in_out == STREAM_OUT) {
        if (level < AUD_VOL_OUT_MAX) {
            if (device & DEVICE_LOUDSPEAKER) {
                //device = LOUDSPEAKER_STREAM_OUT;
                dev_out = LOUDSPEAKER_STREAM_OUT;
                bt_sink_report("[Sink][AM]digital:0x%x, analog:0x%x", g_volume_out_config[dev_out][level][0], g_volume_out_config[dev_out][level][1]);
                eResult = hal_audio_set_stream_out_volume(g_volume_out_config[dev_out][vol_type][level][0], g_volume_out_config[dev_out][vol_type][level][1]);
            } else if (device & DEVICE_EARPHONE) {
                //device = EARPHONE_STREAM_OUT;
                dev_out = EARPHONE_STREAM_OUT;
                bt_sink_report("[Sink][AM]digital:0x%x, analog:0x%x", g_volume_out_config[dev_out][level][0], g_volume_out_config[dev_out][level][1]);
                eResult = hal_audio_set_stream_out_volume(g_volume_out_config[dev_out][vol_type][level][0], g_volume_out_config[dev_out][vol_type][level][1]);
            }
        }
    } else if (in_out == STREAM_IN) {
        if (level < AUD_VOL_IN_MAX) {
            if (device & DEVICE_IN_LIST) {
                //device = MICPHONE_STREAM_IN;
                dev_in = MICPHONE_STREAM_IN;
                eResult = hal_audio_set_stream_in_volume(g_volume_in_config[dev_in][level][0], g_volume_in_config[dev_in][level][1]);
            }
        }
    }
    if (eResult == HAL_AUDIO_STATUS_OK) {
        return HAL_AUDIO_STATUS_OK;
    } else {
#ifdef __AM_DEBUG_INFO__
        bt_sink_report("[Sink][AM][ERROR] Device and Stream type/Vol-level Not Matched");
#endif
        return HAL_AUDIO_STATUS_ERROR;
    }
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_play
 * DESCRIPTION
 *  This function is used to play the audio handler.
 * PARAMETERS
 *	background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_set_play(bt_sink_am_background_t *background_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_hal_result_t        eResult = HAL_AUDIO_STATUS_ERROR;
    bt_sink_am_id_t                bAud_id = background_ptr->aud_id;
    bt_sink_am_audio_stream_out_t  *stream_out = &(background_ptr->audio_stream_out);
    bt_sink_am_audio_stream_in_t   *stream_in = &(background_ptr->audio_stream_in);
    bt_sink_am_stream_node_t       *pcm_stream_node = NULL;
    bt_sink_am_stream_type_t       eIn_out;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ptr_callback_amm == NULL) {
        ptr_callback_amm = (bt_sink_am_amm_struct *)pvPortMalloc(sizeof(bt_sink_am_amm_struct));
    }
    if (background_ptr->type == NONE) {
        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
        background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    } else if (background_ptr->type == PCM) {
        pcm_stream_node = &(background_ptr->local_context.pcm_format.stream);
        eIn_out = background_ptr->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            hal_audio_set_stream_out_sampling_rate(pcm_stream_node->stream_sample_rate);
            hal_audio_set_stream_out_channel_number(pcm_stream_node->stream_channel);
            hal_audio_set_stream_out_device(stream_out->audio_device);
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_am_volume_level_t)stream_out->audio_volume);
            hal_audio_mute_stream_out(stream_out->audio_mute);
            eResult = hal_audio_write_stream_out(pcm_stream_node->buffer, pcm_stream_node->size);
        } else if (eIn_out == STREAM_IN) {
            hal_audio_set_stream_in_sampling_rate(pcm_stream_node->stream_sample_rate);
            hal_audio_set_stream_in_channel_number(pcm_stream_node->stream_channel);
            hal_audio_set_stream_in_device(stream_in->audio_device);
            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_am_volume_level_t)stream_in->audio_volume);
            hal_audio_mute_stream_in(stream_in->audio_mute);
            eResult = hal_audio_read_stream_in(pcm_stream_node->buffer, pcm_stream_node->size);
        }
        if (eResult == HAL_AUDIO_STATUS_ERROR) {
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
        } else {
            if (eIn_out == STREAM_OUT) {
                hal_audio_start_stream_out(background_ptr->audio_path_type);
            } else if (eIn_out == STREAM_IN) {
                hal_audio_start_stream_in(background_ptr->audio_path_type);
            }
            g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
        }
    } else if (background_ptr->type == A2DP) {
        aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_am_volume_level_t)stream_out->audio_volume);
        hal_audio_set_stream_in_device(stream_in->audio_device);
        bt_codec_am_a2dp_sink_open(&(background_ptr->local_context.a2dp_format.a2dp_codec));
    } else if (background_ptr->type == HFP) {
        if (background_ptr->local_context.hfp_format.hfp_codec.type == BT_HF_CODEC_CVSD) {
            hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
            hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
        } else if (background_ptr->local_context.hfp_format.hfp_codec.type == BT_HF_CODEC_MSBC) {
            hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
            hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
        }
        hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
        hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
        aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, stream_out->audio_device, (bt_sink_am_volume_level_t)stream_out->audio_volume);
        aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_am_volume_level_t)stream_in->audio_volume);
        hal_audio_set_stream_out_device(stream_out->audio_device);
        hal_audio_set_stream_in_device(stream_in->audio_device);

        bt_codec_am_hfp_open(&(background_ptr->local_context.hfp_format.hfp_codec));
    }
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_suspend
 * DESCRIPTION
 *  This function set to SUSPEND state when be interrupt by others.
 * PARAMETERS
 *	lead             [IN]
 *  background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_set_suspend(bt_sink_am_type_t lead, bt_sink_am_background_t *background_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_cb_sub_msg_t   msg_id = AUD_SUSPEND_BY_NONE;
    bt_sink_am_id_t             bAud_id = background_ptr->aud_id;
    bt_sink_am_stream_type_t    eIn_out;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (lead) {
        case NONE:
            msg_id = AUD_SUSPEND_BY_NONE;
            break;
        case PCM:
            msg_id = AUD_SUSPEND_BY_PCM;
            break;
        case A2DP:
            msg_id = AUD_SUSPEND_BY_A2DP;
            break;
        case HFP:
            msg_id = AUD_SUSPEND_BY_HFP;
            break;
        default:
            break;
    }

    if (background_ptr->type == NONE) {

    } else if (background_ptr->type == PCM) {
        eIn_out = background_ptr->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            hal_audio_stop_stream_out();
        } else if (eIn_out == STREAM_IN) {
            hal_audio_stop_stream_in();
        }
    } else if (background_ptr->type == A2DP) {
        if (g_rSink_state == A2DP_SINK_CODEC_OPEN) {
            if (bt_codec_am_a2dp_sink_close() == BT_STATUS_MEDIA_ERROR) {
#ifdef __AM_DEBUG_INFO__
                bt_sink_report("[Sink][AM][ERROR] Suspend A2DP");
#endif
                return;
            }
        } else if (g_rSink_state == A2DP_SINK_CODEC_PLAY) {
            if ((bt_codec_am_a2dp_sink_stop(background_ptr->aud_id) == BT_STATUS_MEDIA_ERROR) ||
                    (bt_codec_am_a2dp_sink_close() == BT_STATUS_MEDIA_ERROR)) {
#ifdef __AM_DEBUG_INFO__
                bt_sink_report("[Sink][AM][ERROR] Suspend A2DP");
#endif
                return;
            }
        }
    } else if (background_ptr->type == HFP) {
        if (bt_codec_am_hfp_stop() == BT_STATUS_MEDIA_ERROR) {
#ifdef __AM_DEBUG_INFO__
            bt_sink_report("[Sink][AM][ERROR] Suspend HFP");
#endif
            return;
        }
    }
    g_rAm_aud_id[bAud_id].use = ID_SUSPEND_STATE;
    background_ptr->notify_cb(background_ptr->aud_id, AUD_SUSPEND_BY_IND, msg_id, NULL);
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_search_suspended
 * DESCRIPTION
 *  This function is used to find the highest suspend event to resume it when no player.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_search_suspended(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t *recoder_current_ptr = NULL;
    bt_sink_am_id_t bAud_id;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    recoder_current_ptr = g_rBackground_head.next;
    while (recoder_current_ptr != NULL) {
        bAud_id = recoder_current_ptr->aud_id;
        if (g_rAm_aud_id[bAud_id].use == ID_SUSPEND_STATE) {
            g_rAm_aud_id[bAud_id].use = ID_RESUME_STATE;
            g_prCurrent_resumer = recoder_current_ptr;
#ifdef RTOS_TIMER
            xTimerStart(g_xTimer_am, 0);
#endif
            recoder_current_ptr->notify_cb(recoder_current_ptr->aud_id, AUD_RESUME_IND, AUD_RESUME_IDLE_STATE, NULL);
            break;
        }
        recoder_current_ptr = recoder_current_ptr->next;
    }
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_stop
 * DESCRIPTION
 *  This function is used to stop audio handler.
 * PARAMETERS
 *  background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_set_stop(bt_sink_am_background_t *background_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_stream_type_t    eIn_out;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (background_ptr->type == NONE) {
    } else if (background_ptr->type == PCM) {
        eIn_out = background_ptr->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            hal_audio_stop_stream_out();
        } else if (eIn_out == STREAM_IN) {
            hal_audio_stop_stream_in();
        }
    } else if (background_ptr->type == A2DP) {
        if (g_rSink_state == A2DP_SINK_CODEC_STOP) {
            if (bt_codec_am_a2dp_sink_close() == BT_STATUS_MEDIA_ERROR) {
#ifdef __AM_DEBUG_INFO__
                bt_sink_report("[Sink][AM][ERROR] A2DP codec close fail");
#endif
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                return;
            }
        } else if (g_rSink_state != A2DP_SINK_CODEC_CLOSE) {
#ifdef __AM_DEBUG_INFO__
            bt_sink_report("[Sink][AM][ERROR] A2DP codec NOT CLOSE");
#endif
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
            return;
        }
    } else if (background_ptr->type == HFP) {
        if (bt_codec_am_hfp_stop() == BT_STATUS_MEDIA_ERROR) {
#ifdef __AM_DEBUG_INFO__
            bt_sink_report("[Sink][AM][ERROR] Close HFP");
#endif
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
            return;
        }
    }
    g_rAm_aud_id[background_ptr->aud_id].use = ID_IDLE_STATE;
    background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
}

/*****************************************************************************
 * FUNCTION
 *  aud_initial
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_am_hal_result_t
 *****************************************************************************/
static void aud_initial(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(&g_rAm_aud_id[0], 0, sizeof(bt_sink_am_aud_id_type_t)*AM_REGISTER_ID_TOTAL);
    memset(&g_prA2dp_sink_event_handle, 0, sizeof(g_prA2dp_sink_event_handle));
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_open_stream_req_hdlr
 * DESCRIPTION
 *  Get register ID
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_open_stream_req_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_am_background_t *Background_ptr = NULL;
    bt_sink_am_background_t *recoder_current_ptr = NULL, *recoder_previous_ptr = NULL;
    bt_sink_am_hal_result_t eResult = HAL_AUDIO_STATUS_ERROR;
    uint32_t g_AudMem_size = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_aud_id_num == 1) {
        eResult = hal_audio_get_memory_size(&g_AudMem_size);
        if (eResult == HAL_AUDIO_STATUS_OK) {
#ifndef WIN32_UT
            aud_memory = (uint16_t *)pvPortMalloc(g_AudMem_size * sizeof(uint16_t));
#else
            aud_memory = (uint16_t *)malloc(g_AudMem_size * sizeof(uint16_t));
#endif
            hal_audio_set_memory(aud_memory);
        } else {
#ifdef __AM_DEBUG_INFO__
            bt_sink_report("[Sink][AM][ERROR] Get memoey size");
#endif
        }
    }

#ifndef WIN32_UT
    Background_ptr = (bt_sink_am_background_t *)pvPortMalloc(sizeof(bt_sink_am_background_t));
#else
    Background_ptr = (bt_sink_am_background_t *)malloc(sizeof(bt_sink_am_background_t));
#endif
    Background_ptr->type = NONE;
    Background_ptr->aud_id = bAud_id;
    Background_ptr->priority = am_background_temp->priority;
    Background_ptr->notify_cb = am_background_temp->notify_cb;

    recoder_previous_ptr = &g_rBackground_head;
    recoder_current_ptr = g_rBackground_head.next;
    while ((recoder_current_ptr != NULL) && (recoder_current_ptr->priority > Background_ptr->priority)) {
        recoder_previous_ptr = recoder_current_ptr;
        recoder_current_ptr = recoder_current_ptr->next;
    }
    Background_ptr->prior = recoder_previous_ptr;
    Background_ptr->next = recoder_current_ptr;
    recoder_previous_ptr->next = Background_ptr;
    if (recoder_current_ptr != NULL) {
        recoder_current_ptr->prior = Background_ptr;
    }
    g_rAm_aud_id[bAud_id].contain_ptr = Background_ptr;
    //Background_ptr->notify_cb(am_background_temp->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_play_stream_req_hdlr
 * DESCRIPTION
 *  Start to play the specified audio handler.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_play_stream_req_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_am_id_t         bAud_id = am_background_temp->aud_id;
    bt_sink_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;
    bt_sink_am_background_t *recoder_high_t = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_resumer != NULL) {
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_rAm_aud_id[g_prCurrent_resumer->aud_id].use = ID_IDLE_STATE;
        g_prCurrent_resumer = NULL;
    }

    recoder_current_ptr->type = am_background_temp->type;
    recoder_current_ptr->audio_path_type = am_background_temp->audio_path_type;
    memcpy(&(recoder_current_ptr->local_context), &(am_background_temp->local_context), sizeof(bt_sink_am_codec_t));
    memcpy(&(recoder_current_ptr->audio_stream_in), &(am_background_temp->audio_stream_in), sizeof(bt_sink_am_audio_stream_in_t));
    memcpy(&(recoder_current_ptr->audio_stream_out), &(am_background_temp->audio_stream_out), sizeof(bt_sink_am_audio_stream_out_t));
    /* Find which is high priority */
    recoder_high_t = g_rBackground_head.next;
    while ((recoder_high_t != NULL) && (recoder_high_t->priority > recoder_current_ptr->priority)) {
        recoder_high_t = recoder_high_t->next;
    }
    if (recoder_high_t->aud_id != bAud_id) {
        if (recoder_current_ptr->next != NULL) {
            recoder_current_ptr->next->prior = recoder_current_ptr->prior;
        }
        recoder_current_ptr->prior->next = recoder_current_ptr->next;
        recoder_high_t->prior->next = recoder_current_ptr;
        recoder_current_ptr->prior = recoder_high_t->prior;
        recoder_high_t->prior = recoder_current_ptr;
        recoder_current_ptr->next = recoder_high_t;
    }

    if (g_prCurrent_player != NULL) {
        if (recoder_current_ptr->priority < g_prCurrent_player->priority) {
            g_rAm_aud_id[bAud_id].use = ID_SUSPEND_STATE;
            recoder_current_ptr->notify_cb(bAud_id, AUD_SUSPEND_IND, AUD_EMPTY, NULL);
        } else {
            am_audio_set_suspend(recoder_current_ptr->type, g_prCurrent_player);
            g_prCurrent_player = recoder_current_ptr;
            am_audio_set_play(g_prCurrent_player);
        }
    } else {
        g_prCurrent_player = recoder_current_ptr;
        am_audio_set_play(g_prCurrent_player);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_stop_stream_req_hdlr
 * DESCRIPTION
 *  Stop playing the specified audio handler.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_stop_stream_req_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_prCurrent_player = NULL;
    am_audio_set_stop(recoder_current_ptr);
    am_audio_search_suspended();
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_close_stream_req_hdlr
 * DESCRIPTION
 *  Close the opening audio handler.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_close_stream_req_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_rAm_aud_id[bAud_id].use == ID_RESUME_STATE) {
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_prCurrent_resumer = NULL;
    }
    g_rAm_aud_id[bAud_id].contain_ptr = NULL;
    //recoder_current_ptr->notify_cb(bAud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    if (recoder_current_ptr->next != NULL) {
        recoder_current_ptr->next->prior = recoder_current_ptr->prior;
    }
    if (recoder_current_ptr->prior != NULL) {
        recoder_current_ptr->prior->next = recoder_current_ptr->next;
    }
#ifndef WIN32_UT
    vPortFree(recoder_current_ptr);
#else
    free(recoder_current_ptr);
#endif

    if (g_aud_id_num == 1) {
        if (aud_memory != NULL) {
#ifndef WIN32_UT
            vPortFree(aud_memory);
#else
            free(aud_memory);
#endif
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_volume_stream_out_req_hdlr
 * DESCRIPTION
 *  Set audio in/out volume.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_volume_stream_req_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_am_volume_level_t level;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (am_background_temp->in_out == STREAM_OUT) {
        level = (bt_sink_am_volume_level_t)am_background_temp->audio_stream_out.audio_volume;
        g_prCurrent_player->audio_stream_out.audio_volume = (bt_sink_am_volume_level_out_t)level;
        if (g_prCurrent_player->type == HFP) {
            aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, g_prCurrent_player->audio_stream_out.audio_device, level);
        } else {
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_out.audio_device, level);
        }
    } else if (am_background_temp->in_out == STREAM_IN) {
        level = (bt_sink_am_volume_level_t)am_background_temp->audio_stream_in.audio_volume;
        g_prCurrent_player->audio_stream_in.audio_volume = (bt_sink_am_volume_level_in_t)level;
        aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_in.audio_device, level);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_mute_device_stream_req_hdlr
 * DESCRIPTION
 *	Mute audio input/output device.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_mute_device_stream_req_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t *am_background_temp = &(amm_ptr->background_info);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (am_background_temp->in_out == STREAM_OUT) {
        g_prCurrent_player->audio_stream_out.audio_mute = am_background_temp->audio_stream_out.audio_mute;
        hal_audio_mute_stream_out(g_prCurrent_player->audio_stream_out.audio_mute);
    } else if (am_background_temp->in_out == STREAM_IN) {
        g_prCurrent_player->audio_stream_in.audio_mute = am_background_temp->audio_stream_in.audio_mute;
        hal_audio_mute_stream_out(g_prCurrent_player->audio_stream_in.audio_mute);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_config_device_stream_req_hdlr
 * DESCRIPTION
 *	Set audio input/output device.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_config_device_stream_req_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t *am_background_temp = &(amm_ptr->background_info);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (am_background_temp->in_out == STREAM_OUT) {
        g_prCurrent_player->audio_stream_out.audio_device = am_background_temp->audio_stream_out.audio_device;
        hal_audio_mute_stream_out(TRUE);
        hal_audio_set_stream_out_device(g_prCurrent_player->audio_stream_out.audio_device);
        hal_audio_mute_stream_out(FALSE);
    } else if (am_background_temp->in_out == STREAM_IN) {
        g_prCurrent_player->audio_stream_in.audio_device = am_background_temp->audio_stream_in.audio_device;
        hal_audio_mute_stream_in(TRUE);
        hal_audio_set_stream_in_device(g_prCurrent_player->audio_stream_out.audio_device);
        hal_audio_mute_stream_in(FALSE);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_rw_stream_data_req_hdlr
 * DESCRIPTION
 *	Write data to audio output for palyback / Read data to audio input for record.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_rw_stream_data_req_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t     *am_background_temp = &(amm_ptr->background_info);
    uint8_t                     bAud_id = am_background_temp->aud_id;
    bt_sink_am_hal_result_t     eResult = HAL_AUDIO_STATUS_ERROR;
    bt_sink_am_stream_type_t    eIn_out;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_rAm_aud_id[bAud_id].use == ID_RESUME_STATE) {
#ifdef __AM_DEBUG_INFO__
        bt_sink_report("[Sink][AM] Resume to play");
#endif
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_prCurrent_player = g_prCurrent_resumer;
        am_audio_set_play(g_prCurrent_player);
        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
        g_prCurrent_resumer = NULL;
    } else {
#ifdef __AM_DEBUG_INFO__
        bt_sink_report("[Sink][AM] Continue playing");
#endif
        eIn_out = g_prCurrent_player->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            eResult = hal_audio_write_stream_out(am_background_temp->local_context.pcm_format.stream.buffer,
                                                 am_background_temp->local_context.pcm_format.stream.size);
            if (eResult == HAL_AUDIO_STATUS_OK) {
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
                return;
            }
        } else if (eIn_out == STREAM_IN) {
            eResult = hal_audio_read_stream_in(am_background_temp->local_context.pcm_format.stream.buffer,
                                               am_background_temp->local_context.pcm_format.stream.size);
            if (eResult == HAL_AUDIO_STATUS_OK) {
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
                return;
            }
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_query_stream_len_req_hdlr
 * DESCRIPTION
 *	Query available input/output data length.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_query_stream_len_req_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_background_t     *am_background_temp = &(amm_ptr->background_info);
    bt_sink_am_id_t             bAud_id = am_background_temp->aud_id;
    bt_sink_am_background_t     *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;
    bt_sink_am_hal_result_t     eResult = HAL_AUDIO_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (am_background_temp->in_out == STREAM_OUT) {
        eResult = hal_audio_get_stream_out_sample_count(am_background_temp->data_length_ptr);
    } else if (am_background_temp->in_out == STREAM_IN) {
        eResult = hal_audio_get_stream_in_sample_count(am_background_temp->data_length_ptr);
    }
    /* CallBack to AP */
    if (eResult == HAL_AUDIO_STATUS_OK) {
        recoder_current_ptr->notify_cb(bAud_id, AUD_SELF_CMD_REQ,  AUD_CMD_COMPLETE, NULL);
    } else {
        recoder_current_ptr->notify_cb(bAud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_bt_codec_a2dp_callback
 * DESCRIPTION
 *  This function is used to send L1Audio events to A.M.
 * PARAMETERS
 *  handle           [IN]
 *  event_id         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_bt_codec_a2dp_callback(bt_media_handle_t *handle, bt_sink_am_bt_event_t event_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player->type == A2DP) {
        g_prCurrent_player->local_context.a2dp_format.a2dp_event = event_id;
        bt_sink_ami_send_amm(MOD_AM, MOD_L1SP, AUD_A2DP_PROC_IND,
                             MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ,
                             g_prCurrent_player,
                             TRUE, ptr_callback_amm);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_bt_codec_hfp_callback
 * DESCRIPTION
 *  This function is used to send L1Audio events to A.M.
 * PARAMETERS
 *  handle           [IN]
 *  event_id         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_bt_codec_hfp_callback(bt_media_handle_t *handle, bt_sink_am_bt_event_t event_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player->type == HFP) {
        g_prCurrent_player->local_context.hfp_format.hfp_event = event_id;
        bt_sink_ami_send_amm(MOD_AM, MOD_L1SP, AUD_HFP_EVENT_IND,
                             MSG_ID_MEDIA_HFP_EVENT_CALL_EXT_REQ,
                             g_prCurrent_player,
                             FALSE, ptr_callback_amm);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_stream_out_callback
 * DESCRIPTION
 *  This callback function is notified A.M. for stream-out by AUD HISR.
 * PARAMETERS
 *  event            [OUT]
 *  user_data        [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_stream_out_callback(bt_sink_am_event_result_t event, void *user_data)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player == NULL) {
        return;
    }
    if (g_prCurrent_player->type == PCM) {
        g_prCurrent_player->local_context.pcm_format.event = event;
        bt_sink_ami_send_amm(MOD_AM, MOD_L1SP, AUD_STREAM_EVENT_IND,
                             MSG_ID_MEDIA_EVENT_STREAM_OUT_CALL_EXT_REQ,
                             g_prCurrent_player,
                             TRUE, ptr_callback_amm);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_stream_in_callback
 * DESCRIPTION
 *  This callback function is notified A.M. for stream-in by AUD HISR.
 * PARAMETERS
 *  event            [OUT]
 *  user_data        [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_stream_in_callback(bt_sink_am_event_result_t event, void *user_data)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player == NULL) {
        return;
    }
    if (g_prCurrent_player->type == PCM) {
        g_prCurrent_player->local_context.pcm_format.event = event;
        bt_sink_ami_send_amm(MOD_AM, MOD_L1SP, AUD_STREAM_EVENT_IND,
                             MSG_ID_MEDIA_EVENT_STREAM_IN_CALL_EXT_REQ,
                             g_prCurrent_player,
                             TRUE, ptr_callback_amm);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_process_a2dp_callback_hdlr
 * DESCRIPTION
 *	This function is used to handle A2DP process callback.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_process_a2dp_callback_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_bt_event_t event_id = amm_ptr->background_info.local_context.a2dp_format.a2dp_event;
    //bt_sink_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_status_t eResult = BT_STATUS_MEDIA_ERROR;
    bt_sink_am_device_set_t dev;
    bt_sink_am_volume_level_out_t vol;
    uint32_t digital_vol = 0x7FFF, analog_vol = 0x0002;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        g_prA2dp_sink_handle->process(g_prA2dp_sink_handle, event_id);

        switch (event_id) {
            case BT_MEDIA_ERROR: {
                g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
                g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
                dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol = g_prCurrent_player->audio_stream_out.audio_volume;
                hal_audio_set_stream_out_device(dev);
                if (dev & DEVICE_LOUDSPEAKER) {
                    digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else if (dev & DEVICE_EARPHONE) {
                    digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else {
                    ;
                }
                hal_audio_set_stream_out_volume(digital_vol, analog_vol);

                eResult = g_prA2dp_sink_handle->play(g_prA2dp_sink_handle);
                if (eResult == BT_STATUS_MEDIA_ERROR) {
                    bt_sink_report("[sink][AM] Error\n");
                }
                bt_sink_report("[sink][AM]-restart play\n");
                break;
            }

            default:
                break;
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, (bt_sink_am_cb_sub_msg_t)event_id, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_process_hfp_callback_hdlr
 * DESCRIPTION
 *	This function is used to inform HF AP about hf error event.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_process_hfp_callback_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_bt_event_t event_id = amm_ptr->background_info.local_context.hfp_format.hfp_event;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        if (event_id == BT_MEDIA_ERROR) {
            bt_codec_am_hfp_stop();
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_HFP_EVENT_IND, (bt_sink_am_cb_sub_msg_t)event_id, NULL);

    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_event_stream_callback_hdlr
 * DESCRIPTION
 *	This function is used to handle stream event callback.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_event_stream_callback_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_event_result_t eEvent = amm_ptr->background_info.local_context.pcm_format.event;
    bt_sink_am_id_t bAud_id = amm_ptr->background_info.aud_id;
    bt_sink_am_stream_type_t    eIn_out;
    uint32_t                    data_length = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((g_prCurrent_player == NULL) || (bAud_id != g_prCurrent_player->aud_id)) {
        return;
    }
    if (g_prCurrent_player->type == PCM) {
        eIn_out = g_prCurrent_player->local_context.pcm_format.in_out;
        if (eEvent == HAL_AUDIO_EVENT_UNDERFLOW) {
            if (eIn_out == STREAM_OUT) {
                hal_audio_stop_stream_out();
            } else if (eIn_out == STREAM_IN) {
                hal_audio_stop_stream_in();
            }
            g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_IDLE_STATE;
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_UNDERFLOW, NULL);
        } else if (eEvent == HAL_AUDIO_EVENT_DATA_REQUEST) {
            if (eIn_out == STREAM_OUT) {
                hal_audio_get_stream_out_sample_count(&data_length);
            } else if (eIn_out == STREAM_IN) {
                hal_audio_get_stream_in_sample_count(&data_length);
            }
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_DATA_REQ, &data_length);
        } else if (eEvent == HAL_AUDIO_EVENT_ERROR) {
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_ERROR, NULL);
        } else if (eEvent == HAL_AUDIO_EVENT_NONE) {
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_UNDERFLOW, NULL);
        } else if (eEvent == HAL_AUDIO_EVENT_DATA_NOTIFICATION) {
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_DATA_NOTIFICATION, NULL);
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_get_write_buffer_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "get_write_buffer" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  buffer           [OUT]
 *	length           [OUT]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
static bt_status_t bt_get_write_buffer_a2dp_sink_codec(bt_sink_am_id_t aud_id, uint8_t **buffer, uint32_t *length)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->get_write_buffer(g_prA2dp_sink_handle, buffer, length);
        return BT_STATUS_MEDIA_OK;
    }
    return BT_STATUS_MEDIA_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_write_data_done_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "write data done" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  length           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
static bt_status_t bt_write_data_done_a2dp_sink_codec(bt_sink_am_id_t aud_id, uint32_t length)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->write_data_done(g_prA2dp_sink_handle, length);
        return BT_STATUS_MEDIA_OK;
    }
    return BT_STATUS_MEDIA_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_finish_write_data_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "finish_write_data" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  length           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
static bt_status_t bt_finish_write_data_a2dp_sink_codec(bt_sink_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->finish_write_data(g_prA2dp_sink_handle);
        return BT_STATUS_MEDIA_OK;
    }
    return BT_STATUS_MEDIA_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_get_free_space_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "get_free_space" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  length           [IN]
 * RETURNS
 *  int32_t
 *****************************************************************************/
static int32_t bt_get_free_space_a2dp_sink_codec(bt_sink_am_id_t aud_id)
{
    if (aud_id == g_prCurrent_player->aud_id) {
        return g_prA2dp_sink_handle->get_free_space(g_prA2dp_sink_handle);
    }
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_open
 * DESCRIPTION
 * 	This function is used to open codec for A2DP sink by BT APP
 * PARAMETERS
 *	a2dp_codec_t     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_codec_am_a2dp_sink_open(bt_sink_am_a2dp_codec_t *a2dp_codec_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (a2dp_codec_t->role) {
        case BT_A2DP_SINK:
            g_prA2dp_sink_handle = bt_codec_a2dp_open(aud_bt_codec_a2dp_callback, a2dp_codec_t);
            break;
        default:
            break;
    }
    if (g_bBT_Ringbuf == NULL) {
#ifndef WIN32_UT
        //g_bBT_Ringbuf = (uint8_t *)pvPortMalloc(AM_RING_BUFFER_SIZE * sizeof(uint8_t));
        g_bBT_Ringbuf = g_bt_sink_am_ring_buffer;
#else
        g_bBT_Ringbuf = (uint8_t *)malloc(AM_RING_BUFFER_SIZE * sizeof(uint8_t));
#endif
    }
    g_prA2dp_sink_handle->set_buffer(g_prA2dp_sink_handle, g_bBT_Ringbuf, AM_RING_BUFFER_SIZE);
    g_prA2dp_sink_event_handle.get_write_buffer = bt_get_write_buffer_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.write_data_done = bt_write_data_done_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.finish_write_data = bt_finish_write_data_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.get_free_space = bt_get_free_space_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.play = bt_codec_am_a2dp_sink_play;
    g_prA2dp_sink_event_handle.stop = bt_codec_am_a2dp_sink_stop;
    g_rSink_state = A2DP_SINK_CODEC_OPEN;
    g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
    g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SINK_OPEN_CODEC, AUD_SINK_PROC_PTR, &g_prA2dp_sink_event_handle);
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_play
 * DESCRIPTION
 *  This function is used to process "play" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_a2dp_sink_play(bt_sink_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_STATUS_MEDIA_ERROR;
    bt_sink_am_device_set_t dev;
    bt_sink_am_volume_level_out_t vol;
    uint32_t digital_vol = 0x7FFF, analog_vol = 0x0002;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((g_prCurrent_player != NULL) && (aud_id == g_prCurrent_player->aud_id)) {
        if (g_prA2dp_sink_handle != NULL) {
            dev = g_prCurrent_player->audio_stream_out.audio_device;
            vol = g_prCurrent_player->audio_stream_out.audio_volume;
            hal_audio_set_stream_out_device(dev);
            if (dev & DEVICE_LOUDSPEAKER) {
                digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
            } else if (dev & DEVICE_EARPHONE) {
                digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
            } else {
                ;
            }
            hal_audio_set_stream_out_volume(digital_vol, analog_vol);
            eResult = g_prA2dp_sink_handle->play(g_prA2dp_sink_handle);
            if (eResult == BT_STATUS_MEDIA_OK) {
                g_rSink_state = A2DP_SINK_CODEC_PLAY;
                return BT_STATUS_MEDIA_OK;
            }
        }
    }
    return BT_STATUS_MEDIA_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_stop
 * DESCRIPTION
 *  This function is used to process "play" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_a2dp_sink_stop(bt_sink_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_STATUS_MEDIA_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        if (g_prA2dp_sink_handle != NULL) {
            eResult = g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
            if (eResult == BT_STATUS_MEDIA_OK) {
                g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
                g_rSink_state = A2DP_SINK_CODEC_STOP;
                return BT_STATUS_MEDIA_OK;
            }
        }
    }
    return BT_STATUS_MEDIA_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_close
 * DESCRIPTION
 *  This function is used to process "close" for A2DP sink by BT APP.
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_a2dp_sink_close(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t	eResult = BT_STATUS_MEDIA_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        eResult = bt_codec_a2dp_close(g_prA2dp_sink_handle);
        if (eResult == BT_STATUS_MEDIA_OK) {
            g_prA2dp_sink_handle = NULL;
            g_rSink_state = A2DP_SINK_CODEC_CLOSE;
            if (g_bBT_Ringbuf != NULL) {
#ifndef WIN32_UT
                //vPortFree(g_bBT_Ringbuf);
                bt_sink_memset(g_bt_sink_am_ring_buffer, 0, sizeof(g_bt_sink_am_ring_buffer));
                g_bBT_Ringbuf = NULL;
#else
                free(g_bBT_Ringbuf);
                g_bBT_Ringbuf = NULL;
#endif
            }
            return BT_STATUS_MEDIA_OK;
        }
    }
    return BT_STATUS_MEDIA_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_hfp_open
 * DESCRIPTION
 *  This function is used to open HFP codec.
 * PARAMETERS
 *  hfp_codec        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_codec_am_hfp_open(bt_sink_am_hfp_codec_t *hfp_codec_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_STATUS_MEDIA_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_prHfp_media_handle = bt_codec_hfp_open(aud_bt_codec_hfp_callback, hfp_codec_t);
    eResult = g_prHfp_media_handle->play(g_prHfp_media_handle);
    if (eResult == BT_STATUS_MEDIA_OK) {
        g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    } else {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_hfp_stop
 * DESCRIPTION
 *	This function is used to stop HFP codec.
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_hfp_stop(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_STATUS_MEDIA_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    eResult = g_prHfp_media_handle->stop(g_prHfp_media_handle);
    if (eResult == BT_STATUS_MEDIA_OK) {
        if (bt_codec_hfp_close(g_prHfp_media_handle) == BT_STATUS_MEDIA_OK) {
            return BT_STATUS_MEDIA_OK;
        }
    }
    return BT_STATUS_MEDIA_ERROR;
}

#ifdef RTOS_TIMER
/*****************************************************************************
 * FUNCTION
 *  aud_timer_callback
 * DESCRIPTION
 *  This callback function is used to notify A.M. when the timer expires on timer service task.
 * PARAMETERS
 *  pxTimer          [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_timer_callback(TimerHandle_t pxTimer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t lTimer_ID;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __AM_DEBUG_INFO__
    //bt_sink_report("[Sink][AM] Timer tick");
#endif
    configASSERT(pxTimer);
    lTimer_ID = (uint32_t)pvTimerGetTimerID(pxTimer);
    if (lTimer_ID == AM_TIMER_ID) {
        g_lExpire_count++;
        if (g_lExpire_count == AM_EXPIRE_TIMER_MAX) {
            xTimerStop(pxTimer, 0);
            g_lExpire_count = 0;
            bt_sink_ami_send_amm(MOD_AM, MOD_TMR, AUD_TIMER_IND,
                                 MSG_ID_TIMER_OUT_CALL_EXT_REQ,
                                 g_prCurrent_resumer,
                                 FALSE, NULL);
        }
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  aud_timer_out_callback_hdlr
 * DESCRIPTION
 *	This function is used to handle timer callback on A.M. task.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_timer_out_callback_hdlr(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_rAm_aud_id[g_prCurrent_resumer->aud_id].use = ID_IDLE_STATE;
    g_prCurrent_resumer->notify_cb(g_prCurrent_resumer->aud_id, AUD_RESUME_IND, AUD_RESUME_IDLE_STATE, NULL);
    g_prCurrent_resumer = NULL;
    am_audio_search_suspended();
}

/*****************************************************************************
 * FUNCTION
 *  am_task_main
 * DESCRIPTION
 *  This function is a main message handler on A.M. task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef WIN32_UT
void am_task_main(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t bAud_timer_id = AM_TIMER_ID;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    hal_audio_register_stream_out_callback(aud_stream_out_callback, NULL);
    hal_audio_register_stream_in_callback(aud_stream_in_callback, NULL);
    aud_initial();
    while (1) {
        am_receive_msg(g_prAmm_current);
        free(g_prAmm_current);
        if (g_prCurrent_player != NULL) {
            printf("\tHave player: ID=%d\n", g_prCurrent_player->aud_id);
        } else if (g_prCurrent_resumer != NULL) {
            printf("\tHave resumer: ID=%d\n", g_prCurrent_resumer->aud_id);
        }
        break;
    }
}
#else
QueueHandle_t g_xQueue_am;
void am_task_main(void *arg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_am_amm_struct *amm_temp_t = NULL;
    uint32_t bAud_timer_id = AM_TIMER_ID;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __AM_DEBUG_INFO__
    bt_sink_report("[Sink][AM] Start AM_Task");
#endif
    g_xQueue_am = xQueueCreate(5, sizeof(bt_sink_am_amm_struct *));
    if (g_xQueue_am != 0) {
        g_xSemaphore_ami = xSemaphoreCreateMutex();
        g_xTimer_am = xTimerCreate("Timer",
                                   (AM_TIMER_PERIOD / portTICK_PERIOD_MS),
                                   pdTRUE,
                                   (void *)bAud_timer_id,
                                   aud_timer_callback);
        hal_audio_register_stream_out_callback(aud_stream_out_callback, NULL);
        hal_audio_register_stream_in_callback(aud_stream_in_callback, NULL);
        aud_initial();
        while (1) {
            if (xQueueReceive(g_xQueue_am, &amm_temp_t, portMAX_DELAY)) {
                am_receive_msg((bt_sink_am_amm_struct *)amm_temp_t);
                if (amm_temp_t != ptr_callback_amm) {
                    vPortFree(amm_temp_t);
                }
            }
        }
    }
#ifdef __AM_DEBUG_INFO__
    bt_sink_report("[Sink][AM][ERROR] Start AM_Task Error");
#endif
}
#endif /* WIN32_UT */

/*****************************************************************************
 * FUNCTION
 *  am_receive_msg
 * DESCRIPTION
 *  This function is a main message dispatching function of A.M.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void am_receive_msg(bt_sink_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /*
    #ifdef __AM_DEBUG_INFO__
     bt_sink_report("[Sink][AM] Received Message");
     bt_sink_report("[Sink][AM] Aud ID: %d, Type: %d, Priority: %d, Vol_D: %d, Vol_A: %d, SR: %d",
                     amm_ptr->background_info.aud_id,
                     amm_ptr->background_info.type,
                     amm_ptr->background_info.priority,
                     amm_ptr->background_info.audio_stream_out.audio_volume.digital_gain_index,
                     amm_ptr->background_info.audio_stream_out.audio_volume.analog_gain_index,
                     amm_ptr->background_info.local_context.pcm_format.stream.stream_sample_rate
                   );
    #endif
    */
    switch (amm_ptr->msg_id) {
        case MSG_ID_STREAM_OPEN_REQ:
            aud_set_open_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_PLAY_REQ:
            aud_set_play_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_STOP_REQ:
            aud_set_stop_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_CLOSE_REQ:
            aud_set_close_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_SET_VOLUME_REQ:
            aud_set_volume_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_MUTE_DEVICE_REQ:
            aud_mute_device_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_CONFIG_DEVICE_REQ:
            aud_config_device_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_READ_WRITE_DATA_REQ:
            aud_rw_stream_data_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_GET_LENGTH_REQ:
            aud_query_stream_len_req_hdlr(amm_ptr);
            break;

        case MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ:
            aud_process_a2dp_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_HFP_EVENT_CALL_EXT_REQ:
            aud_process_hfp_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_EVENT_STREAM_OUT_CALL_EXT_REQ:
            aud_event_stream_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_EVENT_STREAM_IN_CALL_EXT_REQ:
            aud_event_stream_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_TIMER_OUT_CALL_EXT_REQ:
            aud_timer_out_callback_hdlr(amm_ptr);
            break;
        default:
            break;
    }

#if 0
    bt_sink_report("[Sink][AM] Background Message");
    if (g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr != NULL) {
        bt_sink_report("[Sink][AM] Aud ID: %d, State: %d, Type: %d, Priority: %d, Vol: %d, pcm_SR: %d, a2dp_SR: %d",
                       amm_ptr->background_info.aud_id,
                       g_rAm_aud_id[amm_ptr->background_info.aud_id].use,
                       g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->type,
                       g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->priority,
                       g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->audio_stream_out.audio_volume,
                       g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->local_context.pcm_format.stream.stream_sample_rate,
                       g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->local_context.a2dp_format.a2dp_codec.codec_cap.codec.aac.sample_rate
                      );
    }
#endif

}

#if STANDALONE_TEST
/* Hal API func */
#ifndef WIN32_UT
hal_audio_status_t hal_audio_init(void)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_deinit(void)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_out_sampling_rate(hal_audio_sampling_rate_t sample_rate)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_out_channel_number(hal_audio_channel_number_t channel_number)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_start_stream_out(hal_audio_active_type_t active_type)
{
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_stop_stream_out(void)
{}

hal_audio_status_t hal_audio_set_stream_out_volume(uint32_t digital_volume_index, uint32_t analog_volume_index)
{
#ifdef __AM_DEBUG_INFO__
    bt_sink_report("[Sink][AM] Digital: %04x, Analog: %04x", digital_volume_index, analog_volume_index);
#endif
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_mute_stream_out(bool mute)
{}

hal_audio_status_t hal_audio_set_stream_out_device(hal_audio_device_t device)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_write_stream_out(const void *buffer, uint32_t size)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_get_stream_out_sample_count(uint32_t *sample_count)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_register_stream_out_callback(hal_audio_stream_out_callback_t callback, void *user_data)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_in_sampling_rate(hal_audio_sampling_rate_t sample_rate)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_in_channel_number(hal_audio_channel_number_t channel_number)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_start_stream_in(hal_audio_active_type_t active_type)
{
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_stop_stream_in(void)
{}

hal_audio_status_t hal_audio_set_stream_in_volume(uint32_t digital_volume_index, uint32_t analog_volume_index)
{
#ifdef __AM_DEBUG_INFO__
    bt_sink_report("[Sink][AM] Digital: %04x, Analog: %04x", digital_volume_index, analog_volume_index);
#endif
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_mute_stream_in(bool mute)
{}

hal_audio_status_t hal_audio_set_stream_in_device(hal_audio_device_t device)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_read_stream_in(void *buffer, uint32_t sample_count)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_get_stream_in_sample_count(uint32_t *sample_count)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_register_stream_in_callback(hal_audio_stream_in_callback_t callback, void *user_data)
{
    return HAL_AUDIO_STATUS_OK;
}


/* BT API func */
bt_media_handle_t *temp_t = NULL;
bt_media_handle_t *hf_temp_t = NULL;

void bt_codec_a2dp_set_buffer(bt_media_handle_t *handle, uint8_t  *buffer, uint32_t  length)
{

}

void bt_codec_a2dp_get_write_buffer(bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    *length = 500;
}

void bt_codec_a2dp_reset_buffer(bt_media_handle_t *handle)
{

}

bt_status_t bt_codec_a2dp_play(bt_media_handle_t *handle)
{
    return BT_STATUS_MEDIA_OK;
}

bt_status_t bt_codec_a2dp_stop(bt_media_handle_t *handle)
{
    return BT_STATUS_MEDIA_OK;
}

bt_media_handle_t *bt_codec_a2dp_open(bt_codec_a2dp_callback_t callback, const bt_sink_am_a2dp_codec_t *param)
{
    temp_t = (bt_media_handle_t *)pvPortMalloc(sizeof(bt_media_handle_t));
    temp_t->set_buffer         = bt_codec_a2dp_set_buffer;
    temp_t->get_write_buffer   = bt_codec_a2dp_get_write_buffer;
    temp_t->reset_share_buffer = bt_codec_a2dp_reset_buffer;
    temp_t->play               = bt_codec_a2dp_play;
    temp_t->stop               = bt_codec_a2dp_stop;
    return (bt_media_handle_t *)temp_t;
}

bt_status_t bt_codec_a2dp_close(bt_media_handle_t *handle)
{
    vPortFree(temp_t);
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_codec_hfp_play(bt_media_handle_t *handle)
{
    return BT_STATUS_MEDIA_OK;
}

bt_status_t bt_codec_hfp_stop(bt_media_handle_t *handle)
{
    return BT_STATUS_MEDIA_OK;
}

bt_media_handle_t *bt_codec_hfp_open(bt_codec_hfp_callback_t callback, const bt_sink_am_hfp_codec_t *param)
{
    hf_temp_t = (bt_media_handle_t *)pvPortMalloc(sizeof(bt_media_handle_t));
    hf_temp_t->play = bt_codec_hfp_play;
    hf_temp_t->stop = bt_codec_hfp_stop;
    return (bt_media_handle_t *)hf_temp_t;
}

bt_status_t bt_codec_hfp_close(bt_media_handle_t *handle)
{
    vPortFree(hf_temp_t);
    return BT_STATUS_SUCCESS;
}
#endif /*#ifndef WIN32_UT*/
#endif

