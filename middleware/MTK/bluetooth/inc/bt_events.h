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

#ifndef __BT_EVENTS_H__
#define __BT_EVENTS_H__

/**
 * @addtogroup BT
 * @{
 * @addtogroup Common
 * @{
 * @addtogroup Common_2 Bluetooth events
 * @{
 * This section provides details on the Bluetooth events.
 * If an event is marked as mandatory, the event must be handled by the user application. If an event is not marked as mandatory,
 * it is optional for the user application to handle the event or not.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b RSSI                       | Received Signal Strength Indicator (RSSI) is a measurement of the power present in a received radio signal. |
 * |\b SCO                        | Synchronous Connection Oriented (SCO) is a protocol used for audio devices in the Bluetooth protocol stack. |
 * |\b GAP                        | The Generic Access Profile (GAP) defines the generic procedures related to discovery of Bluetooth devices and link management aspects of connecting to Bluetooth devices. |
 * |\b HFP                        | Hands-Free Profile, Bluetooth profiles allow hands-free kits to communicate with mobile phones in a car. |
 * |\b A2DP                       | Advanced Audio Distribution Profile (A2DP) defines how multimedia audio can be streamed from one device to another over a Bluetooth connection. |
 * |\b AVRCP                      | Audio/Video Remote Control Profile (AVRCP) is designed to provide a standard interface to allow a single remote control to control all of the A/V equipment to which a user has access to. |
 * |\b PBAPC                      | The Phone Book Access Profile (PBAP) defines the procedures and protocols to exchange Phone Book objects between devices. |
 * |\b SPP                        | Serial Port Profile, a Bluetooth wireless-interface profile. It emulates a serial cable to provide a simple substitute for existing RS-232, including the familiar control signals. |
 * |\b BRSF                       | Bluetooth Retrieve Supported Features, the features of Audio Gateway (AG) or Hands Free (HF) supported. |
 * |\b DTMF                       | Dual Tone Multi Frequency, is an in-band telecommunication signaling system using the voice-frequency band over telephone lines. |
 */

/**
 * @defgroup bt_events_define Define
 * @{
 */
#define BT_EVENT_BASE         0x1000                                                            /**< The base of the Bluetooth events. */

#define BT_GAP_EVENT_RANGE    0x100                                                             /**< The range of the Bluetooth GAP events. */
#define BT_GAP_EVENT_START    BT_EVENT_BASE /* 0x1000 */                                        /**< The start of the Bluetooth GAP events. */
#define BT_GAP_EVENT_END      (BT_GAP_EVENT_START + BT_GAP_EVENT_RANGE - 1) /* 0x10FF */        /**< The end of the Bluetooth GAP events. */

#define BT_HFP_EVENT_RANGE    0x100                                                             /**< The range of the HFP events. */
#define BT_HFP_EVENT_START    (BT_GAP_EVENT_END + 1) /* 0x1100 */                               /**< The start of the HFP events. */
#define BT_HFP_EVENT_END      (BT_HFP_EVENT_START + BT_HFP_EVENT_RANGE - 1) /* 0x11FF */        /**< The end of the HFP events. */

#define BT_A2DP_EVENT_RANGE   0x100                                                             /**< The range of the A2DP events. */
#define BT_A2DP_EVENT_START   (BT_HFP_EVENT_END + 1) /* 0x1200 */                               /**< The start of the A2DP events. */
#define BT_A2DP_EVENT_END     (BT_A2DP_EVENT_START + BT_A2DP_EVENT_RANGE - 1) /* 0x12FF */      /**< The end of the A2DP events. */

#define BT_AVRCP_EVENT_RANGE  0x100                                                             /**< The range of the AVRCP events. */
#define BT_AVRCP_EVENT_START  (BT_A2DP_EVENT_END + 1) /* 0x1300 */                              /**< The start of the AVRCP events. */
#define BT_AVRCP_EVENT_END    (BT_AVRCP_EVENT_START + BT_AVRCP_EVENT_RANGE - 1) /* 0x13FF */    /**< The end of the AVRCP events. */

#define BT_PBAPC_EVENT_RANGE  0x100                                                             /**< The range of the PBAPC events. */
#define BT_PBAPC_EVENT_START  (BT_AVRCP_EVENT_END + 1) /* 0x1400 */                             /**< The start of the PBAPC events. */
#define BT_PBAPC_EVENT_END    (BT_PBAPC_EVENT_START + BT_PBAPC_EVENT_RANGE - 1) /* 0x14FF */    /**< The end of the PBAPC events. */

#define BT_SPP_EVENT_RANGE    0x100                                                             /**< The range of the SPP events. */
#define BT_SPP_EVENT_START    (BT_PBAPC_EVENT_END + 1) /* 0x1500 */                             /**< The start of the SPP events. */
#define BT_SPP_EVENT_END      (BT_SPP_EVENT_START + BT_SPP_EVENT_RANGE - 1) /* 0x15FF */        /**< The end of the SPP events. */

#define BT_MEDIA_EVENT_RANGE  0x100                                                             /**< The range of the Media events. */
#define BT_MEDIA_EVENT_START  (BT_SPP_EVENT_END + 1) /* 0x1600 */                               /**< The start of the Media events. */
#define BT_MEDIA_EVENT_END    (BT_MEDIA_EVENT_START + BT_MEDIA_EVENT_RANGE - 1) /* 0x16FF */    /**< The end of the Media events. */

/**
 * @}
 */

/**
 * @defgroup bt_events_enum Enum
 * @{
 */

/** @brief This enumeration defines the Bluetooth events. */
typedef enum {
    BT_GAP_DISCOVERY_RESULT_IND = BT_GAP_EVENT_START, /**< A mandatory event, indicates a basic rate (BR)/EDR controller or multiple basic rate (BR)/EDR controllers have responded so far during the current inquiry process.
                                                         For more details on the event structure, please refer to #bt_gap_discovery_result_ind_t. */

    BT_GAP_DISCOVERY_CNF,                           /**< A mandatory event, indicates the inquiry is complete.
                                                         For more details on the event structure, please refer to #bt_gap_discovery_cnf_t. */

    BT_GAP_CANCEL_INQUIRY_CNF,                      /**< A mandatory event, indicates the inquiry is cancelled.
                                                         For more details on the event structure, please refer to #bt_gap_cancel_inquiry_cnf_t. */

    BT_GAP_WRITE_INQUIRY_TX_CNF,                    /**< A mandatory event, indicates the write inquiry transmit power level is complete.
                                                         For more details on the event structure, please refer to #bt_gap_write_inquiry_tx_cnf_t. */

    BT_GAP_READ_INQUIRY_RESPONSE_TX_CNF,            /**< A mandatory event, indicates the read inquiry response transmit power level is complete.
                                                         For more details on the event structure, please refer to #bt_gap_read_inquiry_response_tx_cnf_t. */

    BT_GAP_SET_SCAN_MODE_CNF,                       /**< A mandatory event, indicates the set scan mode is complete. If the new mode is the same as the current mode, the user will not receive this event.
                                                         For more details on the event structure, please refer to #bt_gap_set_scan_mode_cnf_t. */

    BT_GAP_READ_RSSI_CNF,                           /**< A mandatory event, indicates the read RSSI is complete.
                                                         For more details on the event structure, please refer to #bt_gap_read_rssi_cnf_t. */

    BT_GAP_POWER_ON_CNF,                            /**< A mandatory event, indicates the Bluetooth power on is complete. If the Bluetooth is powered on, the user will not receive this event.
                                                         For more details on the event structure, please refer to #bt_gap_power_on_cnf_t. */

    BT_GAP_POWER_OFF_CNF,                           /**< A mandatory event, indicates the Bluetooth power off is complete. If the Bluetooth is powered off, the user will not receive this event.
                                                         For more details on the event structure, please refer to #bt_gap_power_off_cnf_t. */

    BT_GAP_READ_REMOTE_NAME_CNF,                    /**< A mandatory event, indicates the remote name request is complete.
                                                         For more details on the event structure, please refer to #bt_gap_read_remote_name_cnf_t. */

    BT_GAP_CREATE_LINK_CNF,                         /**< A mandatory event, indicates the connection is created.
                                                         For more details on the event structure, please refer to #bt_gap_link_connection_result_t. */

    BT_GAP_DETACH_SINGLE_LINK_CNF,                  /**< A mandatory event, indicates the link is detached.
                                                         For more details on the event structure, please refer to #bt_gap_detach_single_link_cnf_t. */

    BT_GAP_SET_ROLE_CNF,                            /**< A mandatory event, indicates the result of the set role process.
                                                         For more details on the event structure, please refer to #bt_gap_set_role_cnf_t. */

    BT_GAP_LINK_ACCEPT_CONFIRM_IND,                 /**< Event is reserved. */

    BT_GAP_ENTER_TEST_MODE_CNF,                      /**< A mandatory event, indicates the device enters test mode.
                                                         For more details on the event structure, please refer to #bt_gap_enter_test_mode_cnf_t. */

    BT_GAP_SET_SNIFF_SUBRATE_POLICY_CNF,            /**< A mandatory event, indicates setting a policy for sniff sub-rating is complete.
                                                         For more details on the event structure, please refer to #bt_gap_set_sniff_subrate_policy_cnf_t. */

    BT_GAP_USER_CONFIRM_RESPONSE_CNF,               /**< A mandatory event, indicates the user confirmation request reply is complete.
                                                         For more details on the event structure, please refer to #bt_gap_user_confirm_response_cnf_t. */

    BT_GAP_IO_CAPABILITY_RESPONSE_CNF,              /**< A mandatory event, indicates the I/O capability request reply is complete.
                                                         For more details on the event structure, please refer to #bt_gap_io_capability_response_cnf_t. */

    BT_GAP_PIN_CODE_RESPONSE_CNF,                   /**< A mandatory event, indicates the PIN code request reply is complete.
                                                         For more details on the event structure, please refer to #bt_gap_pin_code_response_cnf_t. */

    BT_GAP_IO_CAPABILITY_REQUEST_IND,               /**< A mandatory event, indicates the I/O capability of the host is required to establish simple pairing.
                                                         For more details on the event structure, please refer to #bt_gap_io_capability_request_ind_t. */

    BT_GAP_BONDING_CNF,                             /**< A mandatory event, indicates the bonding process is complete.
                                                         For more details on the event structure, please refer to #bt_gap_bonding_cnf_t. */

    BT_GAP_BONDING_RESULT_IND,                      /**< A mandatory event, indicates the result of the bonding process.
                                                         For more details on the event structure, please refer to #bt_gap_bond_result_ind_t. */

    BT_GAP_USER_CONFIRM_REQUEST_IND,                /**< A mandatory event, indicates a user confirmation of a numeric value is required.
                                                         For more details on the event structure, please refer to #bt_gap_user_confirm_request_ind_t. */

    BT_GAP_USER_PASSKEY_NOTIFICATION_IND,           /**< A mandatory event, provides a passkey for the host to display to the user as a simple pairing process requirement.
                                                         For more details on the event structure, please refer to #bt_gap_user_passkey_notification_ind_t. */

    BT_GAP_KEYPRESS_NOTIFICATION_IND,               /**< A mandatory event, sent to the host after a passkey notification is received by the link manager.
                                                         For more details on the event structure, please refer to #bt_gap_keypress_notification_ind_t. */

    BT_GAP_USER_PASSKEY_REQUEST_IND,                /**< A mandatory event, indicates a passkey is required as part of a simple pairing process.
                                                         For more details on the event structure, please refer to #bt_gap_user_passkey_request_ind_t. */

    BT_GAP_PIN_CODE_REQUEST_IND,                    /**< A mandatory event, indicates a PIN code is required as part of a pairing process.
                                                         For more details on the event structure, please refer to #bt_gap_pin_code_request_ind_t. */

    BT_GAP_SEND_KEYPRESS_COMPLETE_IND,              /**< A mandatory event, indicates the send keypress notification is complete.
                                                         For more details on the event structure, please refer to #bt_gap_send_keypress_complete_ind_t. */

    BT_GAP_DELETE_LINK_KEY_IND,                     /**< A mandatory event, indicates the link key has to be deleted.
                                                         For more details on the event structure, please refer to #bt_gap_delete_link_key_ind_t. */

    BT_GAP_AUTHENTICATE_RESULT_IND,                 /**< A mandatory event, indicates the authentication result.
                                                         For more details on the event structure, please refer to #bt_gap_authenticate_result_ind_t. */

    BT_HF_SLC_CONNECT_IND = BT_HFP_EVENT_START,     /**< The service level connection is connected. */

    BT_HF_CONNECT_REQUEST_IND,                      /**< The Audio Gateway (AG) requested to connect Hands Free (HF). */

    BT_HF_SLC_DISCONNECT_IND,                       /**< The service level connection is disconnected. */

    BT_HF_AUDIO_CONNECT_IND,                        /**< The Audio SCO is connected. */

    BT_HF_AUDIO_DISCONNECT_IND,                     /**< The Audio SCO is disconnected. */

    BT_HF_BRSF_FEATURES_IND,                        /**< The BRSF features are supported. */

    BT_HF_CALL_HELD_FEATURES_IND,                   /**< The call held features are supported. */

    BT_HF_CIEV_CALL_SETUP_IND,                      /**< The call setup status is changed. */

    BT_HF_CIEV_CALL_IND,                            /**< The call status is changed. */

    BT_HF_CIEV_CALL_HELD_IND,                       /**< The call hold status is changed. */

    BT_HF_CIEV_SERVICE_IND,                         /**< The service status is changed. */

    BT_HF_CIEV_SIGNAL_IND,                          /**< The signal value is changed. */

    BT_HF_CIEV_ROAM_IND,                            /**< The roaming status is changed. */

    BT_HF_CIEV_BATTCHG_IND,                         /**< The battery value is changed. */

    BT_HF_RING_IND,                                 /**< A call rang. */

    BT_HF_CALL_WAITING_IND,                         /**< The waiting call information is available. */

    BT_HF_CALLER_ID_IND,                            /**< The caller information is available for an incoming call. */

    BT_HF_CURRENT_CALLS_IND,                        /**< The current call information is available for the queried calls. */

    BT_HF_VOICE_RECOGNITION_IND,                    /**< The voice recognition feature is changed. */

    BT_HF_VOLUME_SYNC_SPEAKER_GAIN_IND,             /**< The value of a remote speaker volume is changed. */

    BT_HF_VOLUME_SYNC_MIC_GAIN_IND,                 /**< The value of a remote microphone volume is changed. */

    BT_HF_IN_BAND_RING_IND,                         /**< The in-band ring feature is changed. */

    BT_HF_EXTRA_INDICATOR_INFO_IND,                 /**< The extra indicator information in negotiation state is available. */

    BT_HF_EXTRA_INDICATOR_UPDATE_IND,               /**< The extra indicator value is changed. */

    BT_HF_SLC_CONNECT_CNF,                          /**< The result of a connection request is available. */

    BT_HF_DIAL_NUMBER_CNF,                          /**< The result of a dial request is available. */

    BT_HF_ENABLE_VOICE_RECOGNITION_CNF,             /**< The voice recognition request is enabled. */

    BT_HF_ENABLE_CALLER_ID_CNF,                     /**< The caller ID request is enabled. */

    BT_HF_ANSWER_CALL_CNF,                          /**< The result to answer the call request is available. */

    BT_HF_TERMINATE_CALL_CNF,                       /**< The call request is terminated. */

    BT_HF_ENABLE_CALL_WAITING_CNF,                  /**< The call waiting request is enabled. */

    BT_HF_CALL_HOLD_ACTION_CNF,                     /**< The result of call hold action request is available. */

    BT_HF_DTMF_CNF,                                 /**< The result of send DTMF request is available. */

    BT_HF_CURRENT_CALLS_CNF,                        /**< The result of current inquired call requests is available. */

    BT_HF_MEMORY_DIAL_CNF,                          /**< The result of memory dial request is available. */

    BT_HF_SPEAKER_VOLUME_SYNC_CNF,                  /**< The result of speaker volume sync request is available. */

    BT_HF_MIC_VOLUME_SYNC_CNF,                      /**< The result of microphone volume sync request is available. */

    BT_HF_EXTERN_AT_CMD_CNF,                        /**< The result of external AT command sending request is available. */

    BT_HS_SEND_KEYPAD_CNF,                          /**< The result of send keypad request is available. */

    BT_A2DP_CONNECT_IND = BT_A2DP_EVENT_START,      /**< A connection is attempted from a remote device.
                                                         Call #bt_a2dp_connect_response() to accept or reject the attempt. */

    BT_A2DP_CONNECT_CNF,                            /**< The result of a connect attempt initiated by a local or remote device is available. */

    BT_A2DP_DISCONNECT_IND,                         /**< A disconnect attempt is initiated by a local or remote device, or a link loss occurred. */

    BT_A2DP_DISCONNECT_CNF,                         /**< The result of a disconnect attempt initiated by a local device is available. */

    BT_A2DP_START_STREAMING_IND,                    /**< A start streaming request is initiated from a remote device.
                                                         Call #bt_a2dp_start_streaming_response() to accept or reject the request. */

    BT_A2DP_START_STREAMING_CNF,                    /**< The result of a start streaming attempt initiated by a local device is available. */

    BT_A2DP_SUSPEND_STREAMING_IND,                  /**< A suspend streaming request is initiated from a remote device.
                                                         Call #bt_a2dp_suspend_streaming_response() to accept or reject the request. */

    BT_A2DP_SUSPEND_STREAMING_CNF,                  /**< The result of a suspend streaming attempt initiated by a local device is available. */

    BT_A2DP_RECONFIGURE_IND,                        /**< A reconfigure codec capability request is initiated from a remote device.
                                                         Call #bt_a2dp_reconfigure_response() to accept or reject the request. */

    BT_A2DP_RECONFIGURE_CNF,                        /**< The result of a reconfigure codec capability attempt initiated by a local device is available. */

    BT_A2DP_STREAM_RECEIVED_IND,                    /**< The A2DP streaming information is available. */

    BT_AVRCP_CONNECT_CNF = BT_AVRCP_EVENT_START,    /**< The result of a connect attempt initiated by a local or remote device is available. */

    BT_AVRCP_CONNECT_IND,                           /**< A connect attempt is initiated from a remote device.
                                                         Call #bt_avrcp_connect_response() to accept or reject the attempt. */

    BT_AVRCP_DISCONNECT_IND,                        /**< A disconnect attempt is initiated by a remote device, or a link loss occurred. */

    BT_AVRCP_PASS_THROUGH_CNF,                      /**< The response of a pass through command is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_pass_through_cnf_t. */

    BT_AVRCP_LIST_APP_ATTRIBUTES_CNF,               /**< The response of a list player application attribute command is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_list_app_attributes_cnf_t. */

    BT_AVRCP_LIST_APP_VALUE_CNF,                    /**< The response of a list player application value command is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_list_app_value_cnf_t. */

    BT_AVRCP_SET_APP_VALUE_CNF,                     /**< The response of a set player application value command is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_set_app_value_cnf_t. */

    BT_AVRCP_GET_APP_VALUE_CNF,                     /**< The response of a get player application value command is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_get_app_value_cnf_t. */

    BT_AVRCP_GET_ATTRIBUTE_TEXT_CNF,                /**< The response of a get player application attribute displayable text command is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_get_attribute_text_cnf_t. */

    BT_AVRCP_GET_VALUE_TEXT_CNF,                    /**< The response of a get player application setting value displayable text for the provided player application attribute values is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_get_value_text_cnf_t. */

    BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF,            /**< The response of a get element attributes command is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_get_element_attributes_cnf_t. */

    BT_AVRCP_GET_PLAYER_STATUS_CNF,                 /**< The response of a get player status command is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_get_player_status_cnf_t. */

    BT_AVRCP_REGISTER_NOTIFICATION_CNF,             /**< The response of register notification command is available.
                                                         For more details on the event structure, please refer to #bt_avrcp_register_notification_cnf_t. */

    BT_PBAPC_CONNECT_CNF = BT_PBAPC_EVENT_START,    /**< The result of a connect attempt initiated by a local device is available. */

    BT_PBAPC_AUTH_RESPONSE_CNF,                     /**< The result of an authentication request initiated by a remote device is available. */

    BT_PBAPC_AUTH_IND,                              /**< The authentication challenge information is available from a remote device. */

    BT_PBAPC_PULL_PHONEBOOK_CNF,                    /**< The result of pull phonebook request is available. */

    BT_PBAPC_SET_PHONEBOOK_CNF,                     /**< The result of set phonebook request is available. */

    BT_PBAPC_PULL_VCARD_LISTING_CNF,                /**< The result of pull vCard Listing request is available. */

    BT_PBAPC_PULL_VCARD_ENTRY_CNF,                  /**< The result of pull vCard entry request is available. */

    BT_PBAPC_ABORT_CNF,                             /**< The result of stop operation request is available. */

    BT_PBAPC_DISCONNECT_IND,                        /**< A disconnect attempt is initiated by a local or remote device. */

    BT_PBAPC_DATA_IND,                              /**< The data is available from a remote device. */

    BT_SPP_CONNECT_IND = BT_SPP_EVENT_START,        /**< A connect attempt is initiated from a remote device. Call #bt_spp_connect_response() to accept or reject the attempt. */

    BT_SPP_CONNECT_CNF,                             /**< The result of a connect attempt initiated by a local or remote device is available. */

    BT_SPP_DISCONNECT_IND,                          /**< A disconnect attempt is initiated by a local or remote device, or link loss occurred. */

    BT_SPP_READY_TO_READ_IND,                       /**< The SPP profile's RX buffer has received data. Call #bt_spp_read() to receive the data. */

    BT_SPP_READY_TO_WRITE_IND,                      /**< The SPP profile's TX buffer has free space to send data. Call #bt_spp_write() to send the application data. */

    BT_MEDIA_ERROR = BT_MEDIA_EVENT_START,          /**< The Bluetooth codec operation failed. Might be due to bitstream error. */

    BT_MEDIA_OK,                                    /**< The codec operation was successful. */

    BT_MEDIA_UNDERFLOW,                             /**< The bitstream has an underflow and the codec output is on silent. */

    BT_MEDIA_REQUEST,                               /**< The Bluetooth codec requested a bitstream from the user. */

    BT_MEDIA_DATA_NOTIFICATION,                     /**< The Bluetooth encoder is available to the user. */

    BT_MEDIA_TERMINATED,                            /**< The Bluetooth codec is terminated. */

    BT_EVENT_END
} bt_event_t;

/**
 * @}
 */

/**
 * @}
 * @}
 * @}
 */

#endif /* __BT_EVENTS_H__ */
