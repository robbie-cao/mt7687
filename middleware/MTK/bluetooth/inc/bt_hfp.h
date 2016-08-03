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

#ifndef __BT_HFP_H__
#define __BT_HFP_H__

/**
 * @addtogroup BT
 * @{
 * @addtogroup HFP
 * @{
 * This section introduces the HFP APIs including terms and acronyms, supported features, software architecture,
 * details on how to use it, function groups, enums, structures and functions.
 * The Hands-free Profile (HFP) defines the minimum set of functions such that a Mobile Phone can be used in conjunction with a Hands-Free device,
 * with a Bluetooth Link providing a wireless means for both remote control of the Mobile Phone by the Hands-Free device and voice connection between the Mobile Phone and the Hands-Free device.
 * The following roles are defined for this profile.
 * Audio Gateway (AG): This is the device that is the gateway of the audio, both for input and output. Typical devices acting as Audio Gateways are cellular phones.
 * Hands-Free unit (HF): This is the device acting as the Audio Gateway's remote audio input and output mechanism. It also provides remote control means.
 *
 * Supported features
 * ======
 *  - \b HFP \b V1.6 - Supports Hands-free only, does not support Audio Gateway.
 *   - Call waiting & 3-way calling.
 *   - CLI presentation capability.
 *   - Voice recognition activation.
 *   - Remote volume control.
 *   - Enhanced call status.
 *   - Enhanced call control.
 *   - Codec negotiation.
 *
 *  - \b HSP \b V1.2 - Supports Headset only, does not support Audio Gateway.
 *   - Remote volume control.
 *   - Headset keypad control.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b BATTCHG                    | Battery Charge indicator of AG. |
 * |\b BRSF                       | Bluetooth Retrieve Supported Features, the features of AG or HF supported. |
 * |\b SYNC                       | Synchronize. |
 * |\b MIC                        | Microphone. |
 * |\b DTMF                       | Dual Tone Multi Frequency, is an in-band telecommunication signaling system using the voice-frequency band over telephone lines. |
 * |\b AG                         | Audio Gateway. |
 * |\b NREC                       | Noise Reduction and Echo Canceling. |
 * |\b WBS                        | Wide Band Speech. |
 * |\b CLI                        | Calling Line Identification. |
 * |\b DIR                        | Director, the call's director, an atrribuite of a call. |
 * |\b HSP                        | Headset Profile |
 * |\b CVSD                       | Continuously Variable Slope Delta modulation, a codec of audio. |
 * |\b MSBC                       | Modified Sub Band Codec, a codec of audio. |
 * |\b CHLD                       | Call Hold. |
 *
 * @section bt_hf_api_usage How to use this module
 *
 *  - Step1: Implement #bt_hf_get_init_params() to set the initial configuration.
 *   - Sample code:
 *    @code
 *      bt_status_t bt_hf_get_init_params(bt_hf_init_params_t *param, char *extra_indicators)
 *      {
 *          param->disable_nrec = true;
 *          param->extended_errors = true;
 *
 *          param->optional_indicators.battery = BT_HF_INDICATOR_ON;
 *          param->optional_indicators.roaming = BT_HF_INDICATOR_ON;
 *          param->optional_indicators.service = BT_HF_INDICATOR_ON;
 *          param->optional_indicators.signal = BT_HF_INDICATOR_ON;
 *
 *          param->supported_features = BT_HF_FEATURE_CALL_WAITING|BT_HF_FEATURE_CLI_PRESENTATION|BT_HF_FEATURE_VOICE_RECOGNITION|BT_HF_FEATURE_ECHO_NOISE|BT_HF_FEATURE_VOLUME_CONTROL
 *              |BT_HF_FEATURE_ENHANCED_CALL_STATUS|BT_HF_FEATURE_ENHANCED_CALL_CTRL|BT_HF_FEATURE_CODEC_NEG;
 *
 *          param->supported_profile = BT_HF_PROFILE_V16;
 *
 *          param->supported_wbs_codecs = BT_HF_WBS_CODEC_MASK_MSBC;
 *
 *          return BT_STATUS_SUCCESS;
 *      }
 *    @endcode
 *  - Step2: Implement #bt_hf_common_callback() to get the event id and the related params.
 *  - Step3: Call #bt_hf_connect() to connect to a remote device,
 *      after connection is established, the API #bt_hf_common_callback() gets the event ID #BT_HF_SLC_CONNECT_IND.
 *  - Step4: Call #bt_hf_dial_number() to send HF command to dial the number,
 *      - if the AG gets the command and responses, the API #bt_hf_common_callback() will get the event ID #BT_HF_DIAL_NUMBER_CNF;
 *      - if the outgoing call is set up, the API #bt_hf_common_callback() will get the event id #BT_HF_CIEV_CALL_SETUP_IND, and the parameter of state will be #BT_HF_CALL_SETUP_OUT;
 *      - if the call is answered in remote side the API #bt_hf_common_callback() will get the event id #BT_HF_CIEV_CALL_IND, and the parameter of state will be #BT_HF_CALL_SETUP_OUT.
 */

#include <stdint.h>
#include <stdbool.h>
#include "bt_address.h"
#include "bt_status.h"
#include "bt_events.h"

/**
 * @defgroup bt_hf_define Define
 * @{
 */

/**
 * @brief The maximum size of the phone number for the HF call operation.
 */
#define BT_HF_MAX_PHONE_NUMBER                64
/**
 * @}
 */

/**
 * @defgroup bt_hf_enum Enum
 * @{
 */


/**
 * @brief HF supported CODEC.
 */
typedef uint8_t bt_hf_audio_codec_t;
enum {
    BT_HF_WBS_CODEC_MASK_NONE = 0x00,               /**<  Does not support any audio codec.*/
    BT_HF_WBS_CODEC_MASK_CVSD = 0x01,               /**<  Supports the CVSD codec.*/
    BT_HF_WBS_CODEC_MASK_MSBC = 0X02                /**<  Supports the mSBC codec.*/
};


/**
 * @brief HF supported profiles.
 */

typedef uint8_t bt_hf_support_profile_t;
enum {
    BT_HF_PROFILE_NONE = 0x00,                      /**<  Do not support any HSP or HFP profile.*/
    BT_HF_RPOFILE_HSP = 0x01,                       /**<  Supports HSP.*/
    BT_HF_PROFILE_V15 = 0x02,                       /**<  Supports the HFP version 1.5.*/
    BT_HF_PROFILE_V16 = 0x04,                       /**<  Supports the HFP version 1.6.*/
};


/**
 * @brief HFP CODEC types.
 */
typedef enum {
    BT_HF_CODEC_CVSD = 1,                          /**<  CVSD codec.*/
    BT_HF_CODEC_MSBC = 2                           /**<  MSBC codec.*/
} bt_hf_codec_type_t;


/**
 * @brief This enum defines the HF feature set.
 */
typedef enum {
    BT_HF_FEATURE_ECHO_NOISE = 0x00000001,                      /**<  Echo canceling and/or noise reduction.*/
    BT_HF_FEATURE_CALL_WAITING = 0x00000002,                    /**<  Call-waiting and 3-way calling.*/
    BT_HF_FEATURE_CLI_PRESENTATION = 0x00000004,                /**<  CLI presentation capability.*/
    BT_HF_FEATURE_VOICE_RECOGNITION = 0x00000008,               /**<  Voice recognition.*/
    BT_HF_FEATURE_VOLUME_CONTROL = 0x00000010,                  /**<  Remote volume control.*/
    BT_HF_FEATURE_ENHANCED_CALL_STATUS = 0x00000020,            /**<  Enhanced Call Status.*/
    BT_HF_FEATURE_ENHANCED_CALL_CTRL = 0x00000040,              /**<  Enhanced Call Control.*/
    BT_HF_FEATURE_CODEC_NEGOTIATION = 0x00000080                /**<  Codec negotiation.*/
} bt_hf_features_t;


/**
 * @brief This enum defines the AG feature set.
 */
typedef enum {
    BT_HF_AG_FEATURE_3_WAY = 0x00000001,                             /**<  3-way calling.*/
    BT_HF_AG_FEATURE_ECHO_NOISE = 0x00000002,                        /**<  Echo canceling and/or noise reduction function.*/
    BT_HF_AG_FEATURE_VOICE_RECOGNITION = 0x00000004,                 /**<  Voice recognition function.*/
    BT_HF_AG_FEATURE_IN_BAND_RING = 0x00000008,                      /**<  In-band ring tone.*/
    BT_HF_AG_FEATURE_VOICE_TAG = 0x00000010,                         /**<  Voice tag.*/
    BT_HF_AG_FEATURE_CALL_REJECT = 0x00000020,                       /**<  Reject a call.*/
    BT_HF_AG_FEATURE_ENHANCED_CALL_STATUS = 0x00000040,              /**<  Enhanced call status.*/
    BT_HF_AG_FEATURE_ENHANCED_CALL_CTRL = 0x00000080,                /**<  Enhanced call control.*/
    BT_HF_AG_FEATURE_EXTENDED_ERROR = 0x00000100,                    /**<  Extended error.*/
    BT_HF_AG_FEATURE_CODEC_NEGOGIATION = 0x00000200                  /**<  Codec negotiation.*/
} bt_hf_ag_features_t;


/**
 * @brief This enum defines the gateway's 3-Way calling (hold) feature set.
 */
typedef enum {
    BT_HF_AG_HOLD_RELEASE_HELD_CALLS = 0x01,                    /**<  Releases all held calls or sets User Determined User Busy  (UDUB) for a waiting call.*/
    BT_HF_AG_HOLD_RELEASE_ACTIVE_CALLS = 0x02,                  /**<  Releases all active calls (if any) and accepts the other (held or waiting) call.*/
    BT_HF_AG_HOLD_RELEASE_SPECIFIC_CALL = 0x04,                 /**<  Releases a specific call.*/
    BT_HF_AG_HOLD_HOLD_ACTIVE_CALLS = 0x08,                     /**<  Places all active calls (if any) on hold and accepts the other (held or waiting) call.*/
    BT_HF_AG_HOLD_HOLD_SPECIFIC_CALL = 0x10,                    /**<  Places a specific call on hold.*/
    BT_HF_AG_HOLD_ADD_HELD_CALL = 0x20,                         /**<  Adds a held call to the conversation.*/
    BT_HF_AG_HOLD_CALL_TRANSFER = 0x40                          /**<  Connects the two calls and disconnects the AG from both calls (Explicit Call Transfer).*/
} bt_hf_ag_hold_features_t;


/**
 * @brief This enum defines the current call state indicated by the AG.
 */
typedef enum {
    BT_HF_CALL_NONE = 0,                                        /**<  No existing call on the AG.*/
    BT_HF_CALL_ACTIVE = 1                                       /**<  A call is active on the AG.*/
} bt_hf_call_active_state_t;


/**
 * @brief This enum defines the current call setup state indicated by the AG.
 */
typedef enum {
    BT_HF_CALL_SETUP_NONE = 0,                                  /**<  No outgoing or incoming call is present on the AG.*/
    BT_HF_CALL_SETUP_IN,                                        /**<  An incoming call is present on the AG.*/
    BT_HF_CALL_SETUP_OUT,                                       /**<  An outgoing call is present on the AG.*/
    BT_HF_CALL_SETUP_ALERT                                      /**<  An outgoing call is being alerted on the AG.*/
} bt_hf_call_setup_state_t;


/**
 * @brief This enum defines the current call held state indicated by the AG.
 */
typedef enum {
    BT_HF_CALL_HELD_NONE = 0,                                   /**<  No call is on hold state on the AG. */
    BT_HF_CALL_HELD_ACTIVE,                                     /**<  Some call is on hold state on the AG. */
    BT_HF_CALL_HELD_ALL                                         /**<  All calls are on hold state on the AG. */
} bt_hf_call_held_state_t;


/**
 * @brief This enum defines the current state of a call.  Not all states are supported by all
 * AGs.  At the very minimum, #BT_HF_CALL_STATUS_NONE,
 * #BT_HF_CALL_STATUS_DIALING, #BT_HF_CALL_STATUS_INCOMING and #BT_HF_CALL_STATUS_ACTIVE
 * will be supported.
 */
typedef enum {
    BT_HF_CALL_STATUS_ACTIVE = 0,                           /**<  An active call exists. */
    BT_HF_CALL_STATUS_HELD,                                 /**<  The call is on hold.*/
    BT_HF_CALL_STATUS_DIALING,                              /**<  A call is outgoing. This state occurs when attempting a call using any of the dialing functions.*/
    BT_HF_CALL_STATUS_ALERTING,                             /**<  The remote party is alerted.*/
    BT_HF_CALL_STATUS_INCOMING,                             /**<  A call is incoming.*/
    BT_HF_CALL_STATUS_WAITING,                              /**<  The call is waiting. This state occurs only when the AG supports 3-Way calling.*/
    BT_HF_CALL_STATUS_NONE,                                 /**<  There is no active call.*/
    BT_HF_CALL_STATUS_UNKNOWN                               /**<  Unknown call state.*/
} bt_hf_call_status_t;


/**
 * @brief This enum defines the call mode.
 */
typedef enum {
    BT_HF_CALL_MODE_VOICE = 0,                                 /**<  Voice Call.  */
    BT_HF_CALL_MODE_DATA,                                      /**<  Data Call.  */
    BT_HF_CALL_MODE_FAX                                        /**<  FAX Call.  */
} bt_hf_call_mode_t;


/**
 * @brief This enum defines the indicator status.
 */
typedef enum {
    BT_HF_INDICATOR_OFF,                                    /**<  The indicator is not off. */
    BT_HF_INDICATOR_ON,                                     /**<  The indicator is on.  */
    BT_HF_INDICATOR_IGNORE,                                 /**<  The indicator is ignored, the default value is on.  */
} bt_hf_indicator_status_t;


/**
 * @brief This enum defines the CHLD actions.
 */
typedef enum {
    BT_HF_CHLD_RELEASE_HELD_REJECT_WAITING = 0,             /**<  Release the held and waiting calls. */
    BT_HF_CHLD_RELEASE_ACTIVE_ACCEPT_OTHER = 1,             /**<  Release specified active call or release all the active calls. */
    BT_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER = 2,                /**<  Switch the active call to a call on hold or set a call to an active and switch the others to calls on hold. */
    BT_HF_CHLD_ADD_HELD_TO_MULTIPARTY = 3,                  /**<  Switch all calls on hold to active. */
    BT_HF_CHLD_JOIN_CALLS_AND_HANG_UP = 4                   /**<  Hang up AG and make others to active. */
} bt_hf_chld_action_t;


/**
 * @brief This enum defines the switch audio direction actions.
 */
typedef enum {
    BT_HF_AUDIO_TO_HF,                                      /**<  Set the audio direction to HF.*/
    BT_HF_AUDIO_TO_AG                                       /**<  Set the audio direction to AG.*/
} bt_hf_audio_direction_t;


/**
 * @}
 */


/**
 * @defgroup bt_hf_struct Struct
 * @{
 */

/**
 *  @brief This is the common structure the HF indication events which haven't the special data.
 */
typedef struct {
    uint16_t              conn_id;                      /**<  Current connection ID.*/
} bt_hf_common_ind_t;


/**
 *  @brief This structure defines #BT_HF_RING_IND result.
 */
typedef bt_hf_common_ind_t bt_hf_ring_ind_t;


/**
 *  @brief This structure defines #BT_HF_CONNECT_REQUEST_IND result.
 */
typedef struct {
    bt_address_t              dev_addr;                 /**<  The Bluetooth address of a remote device.*/
} bt_hf_connect_request_ind_t;


/**
 *  @brief This structure defines #BT_HF_SLC_CONNECT_IND result.
 */
typedef struct {
    uint16_t                conn_id;                    /**<  Current connection ID.*/
    bt_address_t            dev_addr;                   /**<  The Bluetooth address of a remote device.*/
    bt_hf_support_profile_t profile_type;               /**<  Type of connected profile.*/
} bt_hf_service_connect_ind_t;


/**
 *  @brief This structure defines #BT_HF_SLC_DISCONNECT_IND result.
 */
typedef struct {
    uint16_t              conn_id;                      /**<  Current connection ID.*/
    bt_address_t          dev_addr;                     /**<  The Bluetooth address of a remote device.*/
} bt_hf_disconnect_ind_t;


/**
 *  @brief This structure defines #BT_HF_AUDIO_CONNECT_IND result.
 */
typedef struct {
    uint16_t                     conn_id;               /**<  Current connection ID.*/
    bt_hf_audio_codec_t          codec;                 /**<  The codec of the current connection.*/
} bt_hf_audio_connect_ind_t;


/**
 *  @brief This structure defines #BT_HF_AUDIO_DISCONNECT_IND result.
 */
typedef struct {
    uint16_t              conn_id;                      /**<  Current connection ID.*/
} bt_hf_audio_disconnect_ind_t;



/**
 *  @brief This structure defines #BT_HF_BRSF_FEATURES_IND result.
 */
typedef struct {
    uint16_t                    conn_id;               /**<  Current connection ID.*/
    bt_hf_features_t            hf;                    /**<  The supported features of a local device.*/
    bt_hf_ag_features_t         ag;                    /**<  The supported features of a remote device.*/
} bt_hf_brsf_features_ind_t;


/**
 *  @brief This structure defines #BT_HF_CALL_HELD_FEATURES_IND result.
 */
typedef struct {
    uint16_t                       conn_id;             /**<  Current connection ID.*/
    bt_hf_ag_hold_features_t       feature;             /**<  The supported call hold operations of a remote device.*/
} bt_hf_hold_features_ind_t;


/**
 *  @brief This structure defines #BT_HF_CIEV_CALL_IND result.
 */
typedef struct {
    uint16_t                       conn_id;             /**<  Current connection ID.*/
    bt_hf_call_active_state_t      state ;              /**<  The call indicator's value of a remote device.*/
} bt_hf_call_ind_t;


/**
 *  @brief This structure defines #BT_HF_CIEV_CALL_SETUP_IND result.
 */
typedef struct {
    uint16_t                       conn_id;             /**<  Current connection ID.*/
    bt_hf_call_setup_state_t       state ;              /**<  The call setup indicator's value of a remote device.*/
} bt_hf_call_setup_ind_t;


/**
 *  @brief This structure defines #BT_HF_CIEV_CALL_HELD_IND result.
 */
typedef struct {
    uint16_t                      conn_id;              /**<  Current connection ID.*/
    bt_hf_call_held_state_t       state ;               /**<  The call held indicator's value of a remote device.*/
} bt_hf_call_held_ind_t;


/**
 *  @brief This structure defines #BT_HF_CALL_WAITING_IND result.
 */
typedef struct {
    uint16_t              conn_id;                                /**<  Current connection ID.*/
    uint16_t              num_size;                               /**<  The length of a phone number of a current waiting call.*/
    uint8_t               number[BT_HF_MAX_PHONE_NUMBER + 1];     /**<  The phone number of a current waiting call.*/
    uint8_t               classmap;                               /**<  The voice parameters of a current waiting call.*/
    uint8_t               type;                                   /**<  The address type of a current waiting call.*/
} bt_hf_call_waiting_ind_t;


/**
 *  @brief This structure defines #BT_HF_CALLER_ID_IND result.
 */
typedef struct {
    uint16_t              conn_id;                                  /**<  Current connection ID.*/
    uint16_t              num_size;                                 /**<  The length of a phone number of the call.*/
    uint8_t               number[BT_HF_MAX_PHONE_NUMBER + 1];       /**<  The phone number of the call.*/
    uint8_t               type;                                     /**<  The address type of the call.*/
} bt_hf_caller_id_ind_t;


/**
 *  @brief This structure defines #BT_HF_CURRENT_CALLS_IND result.
 */
typedef struct {
    uint16_t              conn_id;                                   /**<  Current connection ID. */
    uint8_t               index;                                     /**<  Call index on the AG. */
    uint8_t               dir;                                       /**<  The director of the call, 0 - Mobile Originated, 1 = Mobile Terminated. */
    bt_hf_call_status_t   state;                                     /**<  Call state. */
    bt_hf_call_mode_t     mode;                                      /**<  Call mode. */
    uint8_t               multiple_party;                            /**<  If the call is multiple party, 0 - Not Multiparty, 1 - Multiparty. */
    uint16_t              num_size;                                  /**<  The length of a phone number of the call.*/
    uint8_t               number[BT_HF_MAX_PHONE_NUMBER + 1];        /**<  The phone number of the call.*/
    uint8_t               type;                                      /**<  The address type of the call.*/
} bt_hf_call_list_ind_t;


/**
 *  @brief This structure defines #BT_HF_CIEV_SERVICE_IND result.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    uint8_t               state;                            /**<  The service state of the AG.*/
} bt_hf_service_ind_t;


/**
 *  @brief This structure defines #BT_HF_CIEV_BATTCHG_IND result.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    uint8_t               level;                            /**<  The battery charge level of the AG.*/
} bt_hf_battery_ind_t;


/**
 *  @brief This structure defines #BT_HF_CIEV_SIGNAL_IND result.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    uint8_t               level ;                           /**<  The signal level of the AG.*/
} bt_hf_signal_ind_t;


/**
 *  @brief This structure defines #BT_HF_CIEV_ROAM_IND result.
 */
typedef struct {
    uint16_t               conn_id;                         /**<  Current connection ID. */
    uint8_t                state ;                          /**<  The roaming state of the AG.*/
} bt_hf_roaming_ind_t;


/**
 *  @brief This structure defines #BT_HF_VOICE_RECOGNITION_IND result.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    bool                  enable;                           /**<  The voice recognition state of the AG.*/
} bt_hf_voice_recognition_ind_t;


/**
 *  @brief This structure defines #BT_HF_IN_BAND_RING_IND result.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    bool                  enable;                           /**<  The in-band ring state of the AG.*/
} bt_hf_in_band_ring_status_ind_t;


/**
 *  @brief This structure defines #BT_HF_VOLUME_SYNC_SPEAKER_GAIN_IND result.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    uint32_t              data;                             /**<  The speaker volume of the AG.*/
} bt_hf_volume_sync_speaker_gain_ind_t;


/**
 *  @brief This structure defines #BT_HF_VOLUME_SYNC_MIC_GAIN_IND result.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    uint32_t              data;                             /**<  The microphone volume of the AG.*/
} bt_hf_volume_sync_mic_gain_ind_t;


/**
 *  @brief This structure defines #BT_HF_EXTRA_INDICATOR_INFO_IND result.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    uint16_t              indicator_register_index;         /**<  The index of an indicator registered in the HF.*/
    uint16_t              indicator_index;                  /**<  The index of an indicator in the AG.*/
    uint16_t              min_range;                        /**<  The minimum value of the indicator.*/
    uint16_t              max_range;                        /**<  The maximum value of the indicator.*/
} bt_hf_extra_indicator_ind_t;


/**
 *  @brief This structure defines #BT_HF_EXTRA_INDICATOR_UPDATE_IND result.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    uint16_t              indicator_index;                  /**<  The index of an indicator in the AG. */
    uint16_t              value;                            /**<  The value of the indicator. */
} bt_hf_extra_indicator_update_ind_t;


/**
 *  @brief The general structure of the AG confirmation.
 */
typedef struct {
    uint16_t              conn_id;                          /**<  Current connection ID. */
    int32_t               result;                           /**<  Confirmation result. */
} bt_hf_general_cnf_t;



/**
 *  @brief This structure defines #BT_HF_SLC_CONNECT_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_connect_cnf_t;


/**
 *  @brief This structure defines #BT_HF_DIAL_NUMBER_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_dial_number_cnf_t;


/**
 *  @brief This structure defines #BT_HF_ENABLE_VOICE_RECOGNITION_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_enable_voice_recognition_cnf_t;


/**
 *  @brief This structure defines #BT_HF_ENABLE_CALLER_ID_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_enable_caller_id_cnf_t;


/**
 *  @brief This structure defines #BT_HF_ANSWER_CALL_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_answer_call_cnf_t;


/**
 *  @brief This structure defines #BT_HF_TERMINATE_CALL_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_terminate_call_cnf_t;


/**
 *  @brief This structure defines #BT_HF_ENABLE_CALL_WAITING_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_enable_call_waiting_cnf_t;


/**
 *  @brief This structure defines #BT_HF_CALL_HOLD_ACTION_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_call_hold_cnf_t;


/**
 *  @brief This structure defines #BT_HF_DTMF_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_dtmf_cnf_t;


/**
 *  @brief This structure defines #BT_HF_CURRENT_CALLS_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_current_calls_cnf_t;


/**
 *  @brief This structure defines #BT_HF_MEMORY_DIAL_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_memory_dial_cnf_t;


/**
 *  @brief This structure defines #BT_HF_SPEAKER_VOLUME_SYNC_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_speaker_volume_sync_cnf_t;


/**
 *  @brief This structure defines #BT_HF_MIC_VOLUME_SYNC_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_mic_volume_sync_cnf_t;


/**
 *  @brief This structure defines #BT_HS_SEND_KEYPAD_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hs_send_keypad_cnf_t;


/**
 *  @brief This structure defines #BT_HF_EXTERN_AT_CMD_CNF result.
 */
typedef bt_hf_general_cnf_t bt_hf_extern_at_cmd_cnf_t;


/**
 *  @brief This structure defines the status ofoptional indicators.
 */
typedef struct {
    bt_hf_indicator_status_t service;                           /**<  The service indicator status in the HF. */
    bt_hf_indicator_status_t signal;                            /**<  The signal indicator status in the HF. */
    bt_hf_indicator_status_t roaming;                           /**<  The roaming indicator status in the HF. */
    bt_hf_indicator_status_t battery;                           /**<  The battery charge indicator status in the HF. */
} bt_hf_optional_indicators_t;


/**
 *  @brief This structure defines the initialize params of the HF.
 */
typedef struct {
    bt_hf_support_profile_t  supported_profile;                 /**<  Type of the supported profiles in HF. */
    uint16_t supported_features;                                /**<  Type of the supported features in HF. */
    bt_hf_audio_codec_t    supported_wbs_codecs;                /**<  Type of the supported codecs in HF. */
    bt_hf_optional_indicators_t  optional_indicators;           /**<  Type of the supported optional indicators in HF. */
    bool  disable_nrec;                                         /**<  Status of the nrec, if need to disable nrec(noise reduction and echo canceling) in HF. */
    bool  extended_errors;                                      /**<  Status of the extended errors, if need to get extended error code in HF. */
} bt_hf_init_params_t;


/**
 * @}
 */

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief                                            This function sends a HFP connect request to the AG. The event #BT_HF_SLC_CONNECT_CNF restuns the connection result.
 * @param[in] dev_addr                               The Bluetooth address of the AG.
 * @return                                           Check the result of sent reuqest.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 * @sa                                               #bt_hf_disconnect()
 */
bt_status_t bt_hf_connect(const bt_address_t  *dev_addr);


/**
 * @brief                                            This function sends a keypad command(AT+CKPD = 200) request to the AG.
 *                                                   The event #BT_HS_SEND_KEYPAD_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                Current connection ID.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hs_send_keypad(uint16_t conn_id);


/**
 * @brief                                            This function sends a HFP disconnect request to AG. the event #BT_HF_SLC_DISCONNECT_IND returnes current reuqest's reslut.
 * @param[in] conn_id                                Current connection ID.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 * @sa                                               #bt_hf_connect()
 */
bt_status_t bt_hf_disconnect(uint16_t conn_id);


/**
 * @brief                                            This function sends an enable call waiting command(AT+CCWA) request to the AG.
 *                                                   The event #BT_HF_ENABLE_CALL_WAITING_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] enable                                 Enable or disable the call waiting notification.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_set_call_waiting(uint16_t conn_id, bool enable);


/**
 * @brief                                            This function sends an enable call line identification notification command(AT+CLIP) request to the AG.
 *                                                   The event #BT_HF_ENABLE_CALLER_ID_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] enable                                 Enable or disable the enable call line identification notification.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_set_caller_id(uint16_t conn_id, bool enable);


/**
 * @brief                                            This function sends an enable voice recognition activation command(AT+BRVA) request to the AG.
 *                                                   The event #BT_HF_ENABLE_VOICE_RECOGNITION_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] enable                                 Enable or disable the voice recognition activation.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_set_voice_recognition(uint16_t conn_id, bool enable);


/**
 * @brief                                            This function sends an answer the call or reject the call command(ATA or AT+CHUP) request to the AG.
 *                                                   The event #BT_HF_ANSWER_CALL_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] accept                                 Answer the call or hang-up the call.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 * @sa                                               #bt_hf_terminate_call()
 */
bt_status_t bt_hf_answer_call(uint16_t conn_id, bool accept);


/**
 * @brief                                            This function sends a reject the call command( AT+CHUP) request to the AG.
 *                                                   The event BT_#HF_TERMINATE_CALL_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 * @sa                                               #bt_hf_answer_call()
 */
bt_status_t bt_hf_terminate_call(uint16_t conn_id);


/**
 * @brief                                            This function sends a dial by number command(ATDddd...dd) request to the AG.
 *                                                   The event #BT_HF_DIAL_NUMBER_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] number[]                               The number which need to dial.
 * @param[in] size                                   The size of the number.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 * @sa                                               #bt_hf_dial_memory_location()
 */
bt_status_t bt_hf_dial_number(uint16_t conn_id, const uint8_t number[], uint32_t size);


/**
 * @brief                                            This function sends a dial by AG memory command(ATD>Nan...) request to the AG.
 *                                                   the AG will dial with the nubmer stored in AG memory location.
 *                                                   The event #BT_HF_MEMORY_DIAL_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] *memory_location                       The memory location stored in AG.
 * @param[in] size                                   The size of the memory location.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 * @sa                                               #bt_hf_dial_number()
 */
bt_status_t bt_hf_dial_memory_location(uint16_t conn_id, const uint8_t *memory_location, uint16_t size);


/**
 * @brief                                            This function sends a dial by the last number command(AT+BLDN) request to the AG.
 *                                                   the AG will dial the last number.
 *                                                   The event #BT_HF_DIAL_NUMBER_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_dial_last_number(uint16_t conn_id);


/**
 * @brief                                            This function sends a call hold action command(AT+CHLD) request to the AG.
 *                                                   The event #BT_HF_CALL_HOLD_ACTION_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] action                                 The call held action, please refer to @ref bt_hf_chld_action_t.
 * @param[in] index                                  The index of the action.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_hold_call(uint16_t conn_id, bt_hf_chld_action_t action, uint8_t index);


/**
 * @brief                                            This function sends a speaker volume sync command(AT+VGS) request to the AG.
 *                                                   The event #BT_HF_SPEAKER_VOLUME_SYNC_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] volume                                 The volume value need to sync.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 * @sa                                               #bt_hf_sync_mic_gain()
 */
bt_status_t bt_hf_sync_speaker_gain(uint16_t conn_id, uint8_t volume);


/**
 * @brief                                            This function sends a microphone volume sync command(AT+VMS) request to the AG.
 *                                                   The event #BT_HF_MIC_VOLUME_SYNC_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] volume                                 The volume value need to sync.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 * @sa                                               #bt_hf_sync_speaker_gain()
 */
bt_status_t bt_hf_sync_mic_gain(uint16_t conn_id, uint8_t volume);


/**
 * @brief                                            This function sends a DTMF sending command(AT+VTS) request to the AG.
 *                                                   The event #BT_HF_DTMF_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] dtmf                                   The DTMF value that need to send.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_send_dtmf(uint16_t conn_id, uint8_t dtmf);


/**
 * @brief                                            This function sends a query current calls command(AT+CLCC) request to the AG.
 *                                                   The event #BT_HF_CURRENT_CALLS_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_query_current_calls(uint16_t conn_id);


/**
 * @brief                                            Transfer the audio direction with the AG.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] direction                              The direction of which side the audio need to switch, please refer to @ref bt_hf_audio_direction_t.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_audio_transfer(uint16_t conn_id, bt_hf_audio_direction_t direction);


/**
 * @brief                                            Disconnect the SCO link.
 * @param[in] conn_id                                The id of current connection.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_audio_disconnect(uint16_t conn_id);


/**
 * @brief                                            This function sends a AT command which defined by app to AG.
 *                                                   The event #BT_HF_EXTERN_AT_CMD_CNF returnes current reuqest's reslut.
 * @param[in] conn_id                                The id of current connection.
 * @param[in] command                                The AT command which need to send.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_send_at_command(uint16_t conn_id, const char *command);


/**
 * @brief                                            The connection response when the AG wants to connect to platform.
 *                                                   Application need to call this API when get the event #BT_HF_CONNECT_REQUEST_IND to accept or reject the connection.
 * @param[in] dev_addr                               The device's Bluetooth address which sent the request.
 * @param[in] accept                                 Accept or reject current connection request.
 * @return                                           Check the result of the request sending.
 *                                                   If it is successful, the returned value is zero;
 *                                                   if it failed, the returned value is less than zero.
 */
bt_status_t bt_hf_connect_response(const bt_address_t *dev_addr, bool accept);


/**
 * @brief                                            This function is the common callback with indication or response result to the application.
 *                                                   Application need to implement this API to get the event callback.
 * @param[in] event_id                               The event ID which current callback get. please refer to @ref #bt_event_t to get the ID list, from @ref #BT_HF_SLC_CONNECT_IND to @ref #BT_HF_EXTERN_AT_CMD_CNF.
 * @param[in] param                                  The callback parameters, app need to convert it to the right data type according to event.
 */
void bt_hf_common_callback(bt_event_t event_id, const void *param);


/**
 * @brief                                            The function initializes the HF.
 *                                                   The application need to implement this API to pass the initialization parameters to the profile.
 * @param[in] param                                  For more details about the initialization parameters, please refer to @ref #bt_hf_init_params_t.
 * @param[in] extra_indicators                       The HFP V1.7 supports extra indicators. It's unavailible in current version, application should set it as NULL.
 */
bt_status_t bt_hf_get_init_params(bt_hf_init_params_t *param, char *extra_indicators);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif/*__BT_HFP_H__*/

