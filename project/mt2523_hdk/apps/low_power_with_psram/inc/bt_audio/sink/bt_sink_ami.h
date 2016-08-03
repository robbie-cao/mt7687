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

#ifndef __AM_INTERFACE_H__
#define __AM_INTERFACE_H__

#define STANDALONE_TEST 0
//#define WIN32_UT
#ifndef  WIN32_UT
#define RTOS_TIMER
#define HAL_AUDIO_MODULE_ENABLED
#define __AM_DEBUG_INFO__
#ifdef __AM_DEBUG_INFO__
#include "bt_sink_utils.h"
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#else
#include <stdlib.h>
#include <corecrt_malloc.h>
#endif

#include "bt_sink_am_task.h"
#include "bt_codec.h"
#include "bt_a2dp.h"
#include "bt_hfp.h"
#include "hal_audio.h"
#include "bt_events.h"

#define FALSE                  0
#define TRUE                   1
#define AM_REGISTER_ID_TOTAL   8
#define DEVICE_IN_LIST        (HAL_AUDIO_DEVICE_MAIN_MIC | HAL_AUDIO_DEVICE_HEADSET_MIC | HAL_AUDIO_DEVICE_LINE_IN | HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC | HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC)
#define DEVICE_OUT_LIST       (HAL_AUDIO_DEVICE_HANDSET | HAL_AUDIO_DEVICE_HANDS_FREE_MONO | HAL_AUDIO_DEVICE_HANDS_FREE_STEREO | HAL_AUDIO_DEVICE_HEADSET | HAL_AUDIO_DEVICE_HEADSET_MONO)
#define DEVICE_LOUDSPEAKER    (HAL_AUDIO_DEVICE_HANDSET | HAL_AUDIO_DEVICE_HANDS_FREE_MONO | HAL_AUDIO_DEVICE_HANDS_FREE_STEREO)
#define DEVICE_EARPHONE       (HAL_AUDIO_DEVICE_HEADSET | HAL_AUDIO_DEVICE_HEADSET_MONO)

/*************************************
*         HAL Struct & Enum
**************************************/
typedef hal_audio_status_t          bt_sink_am_hal_result_t;
typedef hal_audio_event_t           bt_sink_am_event_result_t;
typedef hal_audio_device_t          bt_sink_am_device_set_t;
typedef hal_audio_sampling_rate_t   bt_sink_am_sample_rate_t;
typedef hal_audio_bits_per_sample_t bt_sink_am_bit_per_sample_t;
typedef hal_audio_channel_number_t  bt_sink_am_channel_number_t;
typedef hal_audio_active_type_t     bt_sink_am_active_type_t;
typedef bt_event_t                  bt_sink_am_bt_event_t;
typedef bt_codec_hfp_audio_t        bt_sink_am_hfp_codec_t;
typedef bt_codec_a2dp_audio_t       bt_sink_am_a2dp_codec_t;
typedef int8_t                      bt_sink_am_id_t;
typedef uint8_t                     bt_sink_am_volume_level_t;

#ifndef WIN32_UT
extern xSemaphoreHandle g_xSemaphore_ami;
#endif
extern bt_sink_am_id_t g_aud_id_num;
#define AUD_ID_INVALID  -1

/**
 * @defgroup am_enum Enum
 * @{
 */

/**
 *  @brief This enum defines audio handler ID state type.
 */
typedef enum {
    ID_CLOSE_STATE   = 0,
    ID_IDLE_STATE    = 1,
    ID_PLAY_STATE    = 2,
    ID_SUSPEND_STATE = 3,
    ID_RESUME_STATE  = 4
} bt_sink_am_reg_id_state_t;

/**
 *  @brief This enum defines audio handler prioirty type.
 */
typedef enum {
    AUD_LOW     = 0,
    AUD_MIDDLE  = 1,
    AUD_HIGH    = 2
} bt_sink_am_priority_t;

/**
 *  @brief This enum defines audio handler instruction type.
 */
typedef enum {
    AUD_SELF_CMD_REQ      = 0,
    AUD_RESUME_IND        = 1,
    AUD_SUSPEND_IND       = 2,
    AUD_SUSPEND_BY_IND    = 3,
    AUD_A2DP_PROC_IND     = 4,
    AUD_STREAM_EVENT_IND  = 5,
    AUD_TIMER_IND         = 6,
    AUD_SINK_OPEN_CODEC   = 7,
    AUD_HFP_EVENT_IND     = 8
} bt_sink_am_cb_msg_class_t;

/**
 *  @brief This enum defines audio handler result of instruction type.
 */
typedef enum {
    AUD_EMPTY                          = 0,
    AUD_SINK_PROC_PTR                  = 1,
    AUD_CMD_FAILURE                    = 2,
    AUD_CMD_COMPLETE                   = 3,
    AUD_SUSPEND_BY_NONE                = 4,
    AUD_SUSPEND_BY_PCM                 = 5,
    AUD_SUSPEND_BY_HFP                 = 6,
    AUD_SUSPEND_BY_A2DP                = 7,
    AUD_STREAM_EVENT_ERROR             = BT_MEDIA_EVENT_START,
    AUD_STREAM_EVENT_NONE              ,
    AUD_STREAM_EVENT_UNDERFLOW         ,
    AUD_STREAM_EVENT_DATA_REQ          ,
    AUD_STREAM_EVENT_DATA_NOTIFICATION ,
    AUD_STREAM_EVENT_TERMINATED        ,
    AUD_RESUME_PLAY_STATE              ,
    AUD_RESUME_IDLE_STATE
} bt_sink_am_cb_sub_msg_t;

/**
 *  @brief This enum defines audio layer type.
 */
typedef enum {
    MOD_AMI   = 0,
    MOD_AM    = 1,
    MOD_TMR   = 2,
    MOD_L1SP  = 3
} bt_sink_am_module_t;

/**
 *  @brief This enum defines audio handler message type.
 */
typedef enum {
    MSG_ID_AM_CODE_BEGIN                       = 0,
    MSG_ID_STREAM_OPEN_REQ                     = 1,
    MSG_ID_STREAM_PLAY_REQ                     = 2,
    MSG_ID_STREAM_STOP_REQ                     = 3,
    MSG_ID_STREAM_CLOSE_REQ                    = 4,
    MSG_ID_STREAM_SET_VOLUME_REQ               = 5,
    MSG_ID_STREAM_MUTE_DEVICE_REQ              = 6,
    MSG_ID_STREAM_CONFIG_DEVICE_REQ            = 7,
    MSG_ID_STREAM_READ_WRITE_DATA_REQ          = 8,
    MSG_ID_STREAM_GET_LENGTH_REQ               = 9,
    MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ        = 10,
    MSG_ID_MEDIA_HFP_EVENT_CALL_EXT_REQ        = 11,
    MSG_ID_MEDIA_EVENT_STREAM_OUT_CALL_EXT_REQ = 12,
    MSG_ID_MEDIA_EVENT_STREAM_IN_CALL_EXT_REQ  = 13,
    MSG_ID_TIMER_OUT_CALL_EXT_REQ              = 14,
    MSG_ID_AM_CODE_END                         = 15
} bt_sink_am_msg_id_t;

/**
 *  @brief This enum defines audio handler return result.
 */
typedef enum {
    AUD_EXECUTION_FAIL    = -1,
    AUD_EXECUTION_SUCCESS =  0,
} bt_sink_am_result_t;

/**
 *  @brief This enum defines audio stream type.
 */
typedef enum {
    STREAM_IN   = 0x0001,
    STREAM_OUT  = 0x0002
} bt_sink_am_stream_type_t;

/**
 *  @brief This enum defines audio type.
 */
typedef enum {
    PCM   = 0,
    A2DP  = 1,
    HFP   = 2,
    NONE  = 3
} bt_sink_am_type_t;

/**
 *  @brief This enum defines audio volume type.
 */
typedef enum {
    AUD_VOL_AUDIO   = 0,
    AUD_VOL_SPEECH  = 1,
    AUD_VOL_TYPE
} bt_sink_am_volume_type_t;


/**
 *  @brief This struct defines audio sink status detail info.
 */
typedef enum a2dp_sink_state {
    A2DP_SINK_CODEC_CLOSE  = 0,
    A2DP_SINK_CODEC_OPEN   = 1,
    A2DP_SINK_CODEC_PLAY   = 2,
    A2DP_SINK_CODEC_STOP   = 3,
    A2DP_SINK_CODEC_TOTAL  = 4
} bt_sink_am_sink_state_t;

/**
 *  @brief This struct defines audio volume-out level info.
 */
typedef enum {
    AUD_VOL_OUT_LEVEL0 = 0,
    AUD_VOL_OUT_LEVEL1 = 1,
    AUD_VOL_OUT_LEVEL2 = 2,
    AUD_VOL_OUT_LEVEL3 = 3,
    AUD_VOL_OUT_LEVEL4 = 4,
    AUD_VOL_OUT_LEVEL5 = 5,
    AUD_VOL_OUT_LEVEL6 = 6,
    AUD_VOL_OUT_MAX    = 7
} bt_sink_am_volume_level_out_t;

/**
 *  @brief This struct defines audio volume-in level info.
 */
typedef enum {
    AUD_VOL_IN_LEVEL0  = 0,
    AUD_VOL_IN_MAX     = 1
} bt_sink_am_volume_level_in_t;

/**
 *  @brief This struct defines audio volume-out device info.
 */
typedef enum {
    LOUDSPEAKER_STREAM_OUT  = 0,
    EARPHONE_STREAM_OUT     = 1,
    DEVICE_OUT_MAX          = 2
} bt_sink_am_device_out_t;

/**
 *  @brief This struct defines audio volume-in device info.
 */
typedef enum {
    MICPHONE_STREAM_IN  = 0,
    DEVICE_IN_MAX       = 1
} bt_sink_am_device_in_t;

/**
 * @}
 */

/**
 * @defgroup am_struct Struct
 * @{
 */

/**
 *  @brief This struct defines audio stream node detail info.
 */
typedef struct audio_stream_node {
    bt_sink_am_sample_rate_t    stream_sample_rate;
    bt_sink_am_bit_per_sample_t stream_bit_rate;
    bt_sink_am_channel_number_t stream_channel;
    void                        *buffer;
    uint32_t                    size;
} bt_sink_am_stream_node_t;

/**
 *  @brief This struct defines audio pcm format detail info.
 */
typedef struct am_pcm {
    bt_sink_am_stream_node_t    stream;
    bt_sink_am_stream_type_t    in_out;
    bt_sink_am_event_result_t   event;
} bt_sink_am_pcm_format_t;


/**
 *  @brief This struct defines audio a2dp format detail info.
 */
typedef struct am_a2dp {
    bt_sink_am_a2dp_codec_t    a2dp_codec;
    bt_sink_am_bt_event_t      a2dp_event;
} bt_sink_am_a2dp_format_t;


/**
 *  @brief This struct defines audio hfp format detail info.
 */
typedef struct am_hfp {
    bt_sink_am_hfp_codec_t     hfp_codec;
    bt_sink_am_bt_event_t      hfp_event;
} bt_sink_am_hfp_format_t;


/**
 *  @brief This struct defines audio codec type detail info.
 */
typedef union audio_codec {
    bt_sink_am_pcm_format_t   pcm_format;
    bt_sink_am_a2dp_format_t  a2dp_format;
    bt_sink_am_hfp_format_t   hfp_format;
} bt_sink_am_codec_t;

/**
 *  @brief This struct defines sink media handle detail info.
 */
typedef struct {
    bt_status_t (*get_write_buffer)(bt_sink_am_id_t aud_id, uint8_t **buffer, uint32_t *length);
    bt_status_t (*write_data_done)(bt_sink_am_id_t aud_id, uint32_t length);
    bt_status_t (*finish_write_data)(bt_sink_am_id_t aud_id);
    int32_t (*get_free_space)(bt_sink_am_id_t aud_id);
    bt_status_t (*play)(bt_sink_am_id_t aud_id);
    bt_status_t (*stop)(bt_sink_am_id_t aud_id);
} bt_sink_am_media_handle_t;

/**
 *  @brief This struct defines audio common stream-in info.
 */
typedef struct {
    bt_sink_am_device_set_t        audio_device;
    bt_sink_am_volume_level_in_t   audio_volume;
    bool                           audio_mute;
} bt_sink_am_audio_stream_in_t;

/**
 *  @brief This struct defines audio common stream-out info.
 */
typedef struct {
    bt_sink_am_device_set_t         audio_device;
    bt_sink_am_volume_level_out_t   audio_volume;
    bool                            audio_mute;
} bt_sink_am_audio_stream_out_t;


/**
 *  @brief This struct defines audio capability detail info.
 */
typedef struct audio_capability {
    bt_sink_am_type_t               type;
    bt_sink_am_codec_t 		        codec;
    bt_sink_am_audio_stream_in_t    audio_stream_in;
    bt_sink_am_audio_stream_out_t   audio_stream_out;
    bt_sink_am_active_type_t        audio_path_type;
} bt_sink_am_audio_capability_t;

/** @brief define audio callback function prototype for notification */
typedef void(*bt_sink_am_notify_callback)(bt_sink_am_id_t aud_id,
        bt_sink_am_cb_msg_class_t msg_id,
        bt_sink_am_cb_sub_msg_t sub_msg,
        void *parm);

/**
 *  @brief This struct defines audio handler in the background detail info.
 */
typedef struct am_background {
    bt_sink_am_id_t                 aud_id;
    bt_sink_am_type_t               type;
    bt_sink_am_priority_t           priority;
    bt_sink_am_stream_type_t        in_out;
    bt_sink_am_audio_stream_in_t    audio_stream_in;
    bt_sink_am_audio_stream_out_t   audio_stream_out;
    bt_sink_am_active_type_t        audio_path_type;
    bt_sink_am_notify_callback      notify_cb;
    uint32_t                        *data_length_ptr;
    struct am_background            *prior;
    struct am_background            *next;
    bt_sink_am_codec_t              local_context;
} bt_sink_am_background_t;
extern bt_sink_am_background_t *g_prCurrent_player;

/**
 *  @brief This struct defines audio ID detail info.
 */
typedef struct {
    bt_sink_am_reg_id_state_t   use;
    struct am_background        *contain_ptr;
} bt_sink_am_aud_id_type_t;
extern bt_sink_am_aud_id_type_t g_rAm_aud_id[AM_REGISTER_ID_TOTAL];

/**
 *  @brief This struct defines message of audio manager detail info.
 */
typedef struct amm {
    bt_sink_am_cb_msg_class_t   cb_msg_id;
    bt_sink_am_module_t         src_mod_id;      /* Source module ID of the message. */
    bt_sink_am_module_t         dest_mod_id;     /* Destination module ID of the message. */
    bt_sink_am_msg_id_t         msg_id;          /* Message identifier */
    bt_sink_am_background_t     background_info; /* background information*/
} bt_sink_am_amm_struct;
#ifdef WIN32_UT
extern bt_sink_am_amm_struct *g_prAmm_current;
#endif
extern bt_sink_am_amm_struct *ptr_callback_amm;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                   This function is employed to open a new audio handler to get the registered ID.
 * @param[in] priority      Priority level
 * @param[in] handler       Callback function for A.M. notification
 * @return                  A valid registered ID that is 0~(AM_REGISTER_ID_TOTAL-1) on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_am_id_t bt_sink_ami_audio_open(bt_sink_am_priority_t priority, bt_sink_am_notify_callback handler);

/**
 * @brief                   This function is employed to play the specified audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] capability_t  Representing the audio content format
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_am_result_t bt_sink_ami_audio_play(bt_sink_am_id_t aud_id, bt_sink_am_audio_capability_t *capability_t);

/**
 * @brief                   This function is employed to stop playing the specified audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_am_result_t bt_sink_ami_audio_stop(bt_sink_am_id_t aud_id);

/**
 * @brief                   This function is employed to close the opened audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_am_result_t bt_sink_ami_audio_close(bt_sink_am_id_t aud_id);

/**
 * @brief                   This function is employed to set audio in/out volume.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] volume        Representing the audio volume level
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_am_result_t bt_sink_ami_audio_set_volume(bt_sink_am_id_t aud_id, bt_sink_am_volume_level_t volume_level, bt_sink_am_stream_type_t in_out);

/**
 * @brief                   This function is employed to mute on audio stream in/out.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] mute          TRUE/FALSE
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_am_result_t bt_sink_ami_audio_set_mute(bt_sink_am_id_t aud_id, bool mute, bt_sink_am_stream_type_t in_out);

/**
 * @brief                   This function is employed to set audio device.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] device        HAL_AUDIO_DEVICE_NONE ~ HAL_AUDIO_DEVICE_LINEIN
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return	                AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_am_result_t bt_sink_ami_audio_set_device(bt_sink_am_id_t aud_id, bt_sink_am_device_set_t device);

/**
 * @brief                   This function is employed to write data for palyback / Read data for record.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] buffer        Pointer to user's data buffer for writing or reading usage
 * @param[in] data_count    Output data count for writing; recieve data count for reading
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_am_result_t bt_sink_ami_audio_continue_stream(bt_sink_am_id_t aud_id, void *buffer, uint32_t data_count);

/**
 * @brief                   This function is employed to query available input/output data length.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[out]data_length   Available input/output data length
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_am_result_t bt_sink_ami_audio_get_stream_length(bt_sink_am_id_t aud_id, uint32_t *data_length, bt_sink_am_stream_type_t in_out);

//extern void bt_sink_ami_send_amm(bt_sink_am_module_t dest_id, bt_sink_am_module_t src_id, bt_sink_am_cb_msg_class_t cb_msg_id, bt_sink_am_msg_id_t msg_id, bt_sink_am_background_t *background_info);

extern void bt_sink_ami_send_amm(bt_sink_am_module_t dest_id,
                                 bt_sink_am_module_t src_id,
                                 bt_sink_am_cb_msg_class_t cb_msg_id,
                                 bt_sink_am_msg_id_t msg_id,
                                 bt_sink_am_background_t *background_info,
                                 uint8_t fromISR,
                                 bt_sink_am_amm_struct *pr_Amm);

#ifdef __cplusplus
}
#endif
#endif /*__AM_INTERFACE_H__*/

