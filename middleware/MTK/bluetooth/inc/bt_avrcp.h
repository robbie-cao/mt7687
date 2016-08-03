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

#ifndef __BT_AVRCP_H__
#define __BT_AVRCP_H__

/**
 * @addtogroup BT
 * @{
 * @addtogroup AVRCP
 * @{
 * This section describes the AVRCP APIs.
 * The Audio/Video Remote Control Profile (AVRCP) defines the features and procedures
 * required in order to ensure interoperability between Bluetooth devices with audio/video
 * control functions in the Audio/Video distribution scenarios.
 * AVRCP SDK only supports controller role (CT) in V1.3.0.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                              |
 * |------------------------------|-----------------------------------------------------|
 * |\b A/V                        | Audio/Video. |
 * |\b AV/C                       | The AV/C Digital Interface Command Set. |
 * |\b CT                         | Controller role or the device playing the Controller role. |
 * |\b TG                         | Target role or the device playing the Target role. |
 *
 * @section bt_avrcp_api_usage How to use this module
 *
 *  - Step1: Mandatory, implement #bt_avrcp_get_init_params() to set the initial configuration, such as supported features and role.
 *   - Sample code:
 *    @code
 *       bt_status_t bt_avrcp_get_init_params(bt_avrcp_init_params_t *params)
 *       {
 *           param->device_role = BT_AVRCP_ROLE_CT;
 *           param->ct_supported_feature = BT_AVRCP_SUPPORT_CATEGORY_1;
 *           param->tg_supported_feature = 0x00;
 *
 *           return BT_STATUS_AVRCP_OK;
 *       }
 *    @endcode
 *  - Step2: Mandatory, implement #bt_avrcp_common_callback() to handle the AVRCP events, such as connect, disconnect, send pass through, register notification, etc.
 *   - Sample code:
 *    @code
 *       void bt_avrcp_common_callback(bt_event_t event_id, const void *param)
 *       {
 *           switch (event_id)
 *           {
 *               case BT_AVRCP_CONNECT_CNF:
 *               {
 *                   bt_avrcp_connect_cnf_t *params = (bt_avrcp_connect_cnf_t *)param;
 *                   // Save the params->conn_id if params->result is #BT_STATUS_AVRCP_OK.
 *                   break;
 *               }
 *               case BT_AVRCP_DISCONNECT_CNF:
 *               {
 *                   bt_avrcp_disconnect_cnf_t *params = (bt_avrcp_disconnect_cnf_t *)param;
 *                   // The AVRCP disconnect handler to clear the application state, context or other items.
 *                   break;
 *               }
 *               case BT_AVRCP_PASS_THROUGHT_CNF:
 *               {
 *                   bt_avrcp_pass_through_cnf_t *cnf = (bt_avrcp_pass_through_cnf_t *)param;
 *                   if (cnf->result == BT_STATUS_AVRCP_OK)
 *                   {
 *                       // Send key release pass through command if key press command confirmation is received.
 *                       if (cnf->key_state == BT_AVRCP_KEY_PRESS)
 *                       {
 *                           bt_avrcp_send_pass_through_command(cnf->conn_id, cnf->key_code, BT_AVRCP_KEY_RELEASE);
 *                           // You can keep the current AVRCP application state and context such that you handle the other event #BT_AVRCP_PASS_THROUGH_CNF for the key state #BT_AVRCP_KEY_RELEASE easily.
 *                       }
 *                   }
 *                   break;
 *               }
 *               case BT_AVRCP_REGISTER_NOTIFICATION_CNF:
 *               {
 *                   bt_avrcp_register_notification_cnf_t *cnf = (bt_avrcp_register_notification_cnf_t *)param;
 *                   if (cnf->result == BT_STATUS_AVRCP_OK)
 *                   {
 *                       // Response that the notification is successfully registered.
 *                       if (cnf->is_interim)
 *                       {
 *                           // Do something related to successfully registering a notification.
 *                       }
 *                       // The registered notification is changed.
 *                       // An additional NOTIFY command is expected to be sent.
 *                       else
 *                       {
 *                           // Handle the notification.
 *
 *                           // Send the notification again if it is required to be tracked once more.
 *                           bt_avrcp_register_notification(cnf->conn_id, cnf->event_id, interval);
 *                       }
 *                   }
 *                   break;
 *               }
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - Step3: Mandatory, active connect to remote device.
 *   - Sample code:
 *    @code
 *       // Active connect to remote device, passing remote address & local role, then handle.
 *       // The event #BT_AVRCP_CONNECT_CNF in #bt_avrcp_common_callback().
 *       ret = bt_avrcp_connect(&dev_addr, BT_AVRCP_ROLE_CT);
 *
 *    @endcode
 *  - Step4: Mandatory, active disconnect remote device.
 *   - Sample code:
 *    @code
 *       // Active disconnect remote device, passing connection ID.
 *       ret = bt_avrcp_disconnect(conn_id);
 *
 *    @endcode
 *  - Step5: Mandatory, in the CT role, register notification and send pass through
 *           command after AVRCP connection is established.
 *   - Sample code:
 *    @code
 *       // For command pass through:
 *       // Send pass through command, passing connection ID, key value and key state,
 *       // then handle the event #BT_AVRCP_PASS_THROUGHT_CNF in #bt_avrcp_common_callback().
 *       ret = bt_avrcp_send_pass_through_command(conn_id, BT_AVRCP_COMMAND_OPCODE_PLAY, BT_AVRCP_KEY_PRESS);
 *
 *       // For command register notification:
 *       // Register a notification, passing connection ID, event ID and interval, then
 *       // handle the event #BT_AVRCP_REGISTER_NOTIFICATION_CNF in #bt_avrcp_common_callback().
 *       // Note, according to the AVRCP specification, interval is applicable only for #BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED.
 *       ret = bt_avrcp_register_notification(conn_id, BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, 255);
 *    @endcode
 *  - Step6: Optional, list player application setting attributes and values, get
 *           or set player application setting values.
 *   - Sample code:
 *    @code
 *       // For command listing player application setting attributes:
 *       // Send list player application setting attributes command, passing connection ID,
 *       // then handle the event #BT_AVRCP_LIST_APP_ATTRIBUTES_CNF in #bt_avrcp_common_callback().
 *       ret = bt_avrcp_list_app_attributes(conn_id);
 *
 *       // For command listing player application setting values:
 *       // Send list player application setting values command, passing connection ID and attribute ID,
 *       // then handle the event #BT_AVRCP_LIST_APP_VALUE_CNF in #bt_avrcp_common_callback().
 *       ret = bt_avrcp_list_app_value(conn_id, attribute_id);
 *
 *       // For command getting player application setting values:
 *       // Send get player application setting values command, passing connection ID and attribute IDs,
 *       // then handle the event #BT_AVRCP_GET_APP_VALUE_CNF in #bt_avrcp_common_callback().
 *       ret = bt_avrcp_get_app_value(conn_id, attributes_num, &attributes_ids);
 *    @endcode
 *  - Step7: Mandatory, get status and play element attributes.
 *   - Sample code:
 *    @code
 *       // For command playing status:
 *       // Send get play status command, passing connection ID,
 *       // then handle the event #BT_AVRCP_GET_PLAYER_STATUS_CNF in #bt_avrcp_common_callback().
 *       ret = bt_avrcp_get_play_status(conn_id);
 *
 *       // For command getting element attributes:
 *       // Send get element attributes command, passing connection ID and attribute IDs,
 *       // then handle the event #BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF in #bt_avrcp_common_callback().
 *       ret = bt_avrcp_get_element_attributes(conn_id, attributes_num, &attribute_ids);
 *    @endcode
 */

#include <stdint.h>
#include <stdbool.h>
#include "bt_address.h"
#include "bt_status.h"
#include "bt_events.h"

/**
 * @defgroup bt_avrcp_define Define
 * @{
 */

/**
 * @brief The maxinum number of player application setting attributes.
 */
#define BT_AVRCP_MAX_APP_ATTRIBUTE_NUM              (4)
/**
 * @brief The maxinum number of player application setting attibute values.
 */
#define BT_AVRCP_MAX_APP_ATTRIBUTE_VALUE_NUM        (4)
/**
 * @brief The maxinum length of a string text.
 */
#define BT_AVRCP_MAX_APP_ATTRIBUTE_STRING_SIZE      (80)
/**
 * @brief The maxinum number of media elements quiried at once.
 */
#define BT_AVRCP_MAX_ELEMENT_ATTRIBUTE_NUM          (10)
/**
 * @brief The maxinum number of element attributes.
 */
#define BT_AVRCP_MAX_ELEMENET_ATTRIBUTE_BUFFER_SIZE (512)
/**
 * @}
 */

/**
 * @defgroup bt_avrcp_enum Enum
 * @{
 */

/**
 * @brief Version type.
 */
typedef enum {
    BT_AVRCP_VERSION_10 = 10,                       /**< v1.0 */
    BT_AVRCP_VERSION_13 = 13,                       /**< v1.3 */
    BT_AVRCP_VERSION_14 = 14,                       /**< v1.4 */
    BT_AVRCP_VERSION_TOTAL
} bt_avrcp_version_t;

/**
 * @brief Support feature type.
 */
typedef enum {
    BT_AVRCP_SUPPORT_CATEGORY_1 = 0x01,              /**< Category 1: player/recorder. */
    BT_AVRCP_SUPPORT_CATEGORY_2 = 0x02,              /**< Category 2: monitor/amplifier. */
    BT_AVRCP_SUPPORT_CATEGORY_3 = 0x04,              /**< Category 3: tuner. */
    BT_AVRCP_SUPPORT_CATEGORY_4 = 0x08,              /**< Category 4: menu. */

    BT_AVRCP_SUPPORT_FEATURE_ALL = 0x0F
} bt_avrcp_support_feature_t;

/**
 * @brief Role type.
 */
typedef enum {
    BT_AVRCP_ROLE_TG,                                /**< Target */
    BT_AVRCP_ROLE_CT,                                /**< Controller */
    BT_AVRCP_ROLE_CT_TG,                             /**< Controller and Target */

    BT_AVRCP_ROLE_TOTAL
} bt_avrcp_role_t;

/**
 * @brief Command control type.
 */
typedef enum {
    BT_AVRCP_COMMAND_CONTROL_NOT_IMPLEMENT = 0x08,           /**< AVRCP command control type: not implemented. */
    BT_AVRCP_COMMAND_CONTROL_ACCEPT = 0x09,                  /**< AVRCP command control type: accept. */
    BT_AVRCP_COMMAND_CONTROL_REJECT = 0x0A,                  /**< AVRCP command control type: reject. */
    BT_AVRCP_COMMAND_CONTROL_STABLE = 0x0C,                  /**< AVRCP command control type: stable. */
    BT_AVRCP_COMMAND_CONTROL_INTERIM = 0x0F                  /**< AVRCP command control type: interim. */
} bt_avrcp_command_control_t;


/**
 * @brief Key value type.
 */
typedef enum {
    BT_AVRCP_COMMAND_OPCODE_SELECT = 0x00,                  /**< AVRCP command operation code: select. */
    BT_AVRCP_COMMAND_OPCODE_UP = 0x01,                      /**< AVRCP command operation code: up. */
    BT_AVRCP_COMMAND_OPCODE_DOWN = 0x02,                    /**< AVRCP command operation code: down. */
    BT_AVRCP_COMMAND_OPCODE_LEFT = 0x03,                    /**< AVRCP command operation code: left. */
    BT_AVRCP_COMMAND_OPCODE_RIGHT = 0x04,                   /**< AVRCP command operation code: right. */
    BT_AVRCP_COMMAND_OPCODE_RIGHT_UP = 0x05,                /**< AVRCP command operation code: right up. */
    BT_AVRCP_COMMAND_OPCODE_RIGHT_DOWN = 0x06,              /**< AVRCP command operation code: right down. */
    BT_AVRCP_COMMAND_OPCODE_LEFT_UP = 0x07,                 /**< AVRCP command operation code: left up. */
    BT_AVRCP_COMMAND_OPCODE_LEFT_DOWN = 0x08,               /**< AVRCP command operation code: left down. */
    BT_AVRCP_COMMAND_OPCODE_ROOT_MENU = 0x09,               /**< AVRCP command operation code: root menu. */
    BT_AVRCP_COMMAND_OPCODE_SETUP_MENU = 0x0A,              /**< AVRCP command operation code: setup menu. */
    BT_AVRCP_COMMAND_OPCODE_CONTENTS_MENU = 0x0B,           /**< AVRCP command operation code: contents menu. */
    BT_AVRCP_COMMAND_OPCODE_FAVORITE_MENU = 0x0C,           /**< AVRCP command operation code: favorite menu. */
    BT_AVRCP_COMMAND_OPCODE_EXIT = 0x0D,                    /**< AVRCP command operation code: exit. */

    BT_AVRCP_COMMAND_OPCODE_0 = 0x20,                       /**< AVRCP command operation code: 0. */
    BT_AVRCP_COMMAND_OPCODE_1 = 0x21,                       /**< AVRCP command operation code: 1. */
    BT_AVRCP_COMMAND_OPCODE_2 = 0x22,                       /**< AVRCP command operation code: 2. */
    BT_AVRCP_COMMAND_OPCODE_3 = 0x23,                       /**< AVRCP command operation code: 3. */
    BT_AVRCP_COMMAND_OPCODE_4 = 0x24,                       /**< AVRCP command operation code: 4. */
    BT_AVRCP_COMMAND_OPCODE_5 = 0x25,                       /**< AVRCP command operation code: 5. */
    BT_AVRCP_COMMAND_OPCODE_6 = 0x26,                       /**< AVRCP command operation code: 6 */
    BT_AVRCP_COMMAND_OPCODE_7 = 0x27,                       /**< AVRCP command operation code: 7 */
    BT_AVRCP_COMMAND_OPCODE_8 = 0x28,                       /**< AVRCP command operation code: 8. */
    BT_AVRCP_COMMAND_OPCODE_9 = 0x29,                       /**< AVRCP command operation code: 9. */
    BT_AVRCP_COMMAND_OPCODE_DOT = 0x2A,                     /**< AVRCP command operation code: dot. */
    BT_AVRCP_COMMAND_OPCODE_ENTER = 0x2B,                   /**< AVRCP command operation code: enter. */
    BT_AVRCP_COMMAND_OPCODE_CLEAR = 0x2C,                   /**< AVRCP command operation code: clear. */

    BT_AVRCP_COMMAND_OPCODE_CHANNEL_UP = 0x30,              /**< AVRCP command operation code: channel up. */
    BT_AVRCP_COMMAND_OPCODE_CHANNEL_DOWN = 0x31,            /**< AVRCP command operation code: channel down. */
    BT_AVRCP_COMMAND_OPCODE_PREVIOUS_CHANNEL = 0x32,        /**< AVRCP command operation code: previous channel. */
    BT_AVRCP_COMMAND_OPCODE_SOUND_SELECT = 0x33,            /**< AVRCP command operation code: sound select. */
    BT_AVRCP_COMMAND_OPCODE_INPUT_SELECT = 0x34,            /**< AVRCP command operation code: input select. */
    BT_AVRCP_COMMAND_OPCODE_DISPLAY_INFO = 0x35,            /**< AVRCP command operation code: display information. */
    BT_AVRCP_COMMAND_OPCODE_HELP = 0x36,                    /**< AVRCP command operation code: help. */
    BT_AVRCP_COMMAND_OPCODE_PAGE_UP = 0x37,                 /**< AVRCP command operation code: page up. */
    BT_AVRCP_COMMAND_OPCODE_PAGE_DOWN = 0x38,               /**< AVRCP command operation code: page down. */

    BT_AVRCP_COMMAND_OPCODE_POWER = 0x40,                   /**< AVRCP command operation code: power. */
    BT_AVRCP_COMMAND_OPCODE_VOLUME_UP = 0x41,               /**< AVRCP command operation code: volume up. */
    BT_AVRCP_COMMAND_OPCODE_VOLUME_DOWN = 0x42,             /**< AVRCP command operation code: volume down. */
    BT_AVRCP_COMMAND_OPCODE_MUTE = 0x43,                    /**< AVRCP command operation code: mute. */
    BT_AVRCP_COMMAND_OPCODE_PLAY = 0x44,                    /**< AVRCP command operation code: play. */
    BT_AVRCP_COMMAND_OPCODE_STOP = 0x45,                    /**< AVRCP command operation code: stop. */
    BT_AVRCP_COMMAND_OPCODE_PAUSE = 0x46,                   /**< AVRCP command operation code: pause. */
    BT_AVRCP_COMMAND_OPCODE_RECORD = 0x47,                  /**< AVRCP command operation code: record. */
    BT_AVRCP_COMMAND_OPCODE_REWIND = 0x48,                  /**< AVRCP command operation code: rewind. */
    BT_AVRCP_COMMAND_OPCODE_FAST_FORWARD = 0x49,            /**< AVRCP command operation code: fast forward.*/
    BT_AVRCP_COMMAND_OPCODE_EJECT = 0x4A,                   /**< AVRCP command operation code: eject. */
    BT_AVRCP_COMMAND_OPCODE_FORWARD = 0x4B,                 /**< AVRCP command operation code: forward. */
    BT_AVRCP_COMMAND_OPCODE_BACKWARD = 0x4C,                /**< AVRCP command operation code: backward. */

    BT_AVRCP_COMMAND_OPCODE_ANGLE = 0x50,                   /**< AVRCP command operation code: angle. */
    BT_AVRCP_COMMAND_OPCODE_SUBPICTURE = 0x51,              /**< AVRCP command operation code: subpicture. */

    BT_AVRCP_COMMAND_OPCODE_F1 = 0x71,                      /**< AVRCP command operation code: F1. */
    BT_AVRCP_COMMAND_OPCODE_F2 = 0x72,                      /**< AVRCP command operation code: F2. */
    BT_AVRCP_COMMAND_OPCODE_F3 = 0x73,                      /**< AVRCP command operation code: F3. */
    BT_AVRCP_COMMAND_OPCODE_F4 = 0x74,                      /**< AVRCP command operation code: F4. */
    BT_AVRCP_COMMAND_OPCODE_F5 = 0x75,                      /**< AVRCP command operation code: F5. */

    BT_AVRCP_COMMAND_OPCODE_VENDOR_UNIQUE = 0x7E,           /**< AVRCP command operation code: vendor unique. */

    BT_AVRCP_COMMAND_OPCODE_RESERVED = 0x7F                 /**< AVRCP command operation code: reserved. */

} bt_avrcp_command_opcode_t;

/**
 * @brief Key state type.
 */
typedef enum {
    BT_AVRCP_KEY_PRESS = 0,                          /**< Operation state flag: key press. */
    BT_AVRCP_KEY_RELEASE,                            /**< Operation state flag: key release. */
    BT_AVRCP_KEY_END
} bt_avrcp_key_state_t;

/**
 * @brief Player application setting attributes type.
 */
typedef enum {
    BT_AVRCP_APP_EQUALIZER = 0x01,                   /**< Attribute: Equalizer. */
    BT_AVRCP_APP_REPEAT_MODE = 0x02,                 /**< Attribute: Repeat Mode. */
    BT_AVRCP_APP_SHUFFLE_MODE = 0x03,                /**< Attribute: Shuffle Mode. */
    BT_AVRCP_APP_SCAN_MODE = 0x04,                   /**< Attribute: Scan Mode. */

    BT_AVRCP_APP_UNDEFINED = 0xFF
} bt_avrcp_app_attributes_t;


/**
 * @brief Media attribute type.
 */
typedef enum {
    BT_AVRCP_MEDIA_TITLE = 0x01,                     /**< Attribute: Title. */
    BT_AVRCP_MEDIA_ARTIST = 0x02,                    /**< Attribute: Artist Name. */
    BT_AVRCP_MEDIA_ALBUM = 0x03,                     /**< Attribute: Album Name, the title of the recording (source) from which the audio in the file is taken. */
    BT_AVRCP_MEDIA_TRACK_NUMBER = 0x04,              /**< Attribute: Tracker Number, the order number of the audio-file on its original recording. */
    BT_AVRCP_MEDIA_TOTAL_TRACK_NUMBER = 0x05,        /**< Attribute: Total Number of Tracks, the total number of tracks or elements on the original recording. */
    BT_AVRCP_MEDIA_GENRE = 0x06,                     /**< Attribute: Genre, the category of the composition characterized of by a particular style. */
    BT_AVRCP_MEDIA_PLAYING_TIME = 0x07,              /**< Attribute: Playing Time, the length of the audio file in milliseconds. */

    BT_AVRCP_MEDIA_ATTR_UNDEFINED = 0xFF
} bt_avrcp_media_attributes_t;

/**
 * @brief Notification event type.
 */
typedef enum {
    BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED = 0x01,   /**< The playback status of the current track is changed. */
    BT_AVRCP_EVENT_TRACK_CHANGED = 0x02,             /**< The current track is changed. */
    BT_AVRCP_EVENT_TRACK_REACHED_END = 0x03,         /**< End of a track reached. */
    BT_AVRCP_EVENT_TRACK_REACHED_START = 0x04,       /**< At the start of a track. */
    BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED = 0x05,      /**< The playback position is changed. */
    BT_AVRCP_EVENT_BATT_STATUS_CHANGED = 0x06,       /**< The battery status is changed. */
    BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED = 0x07,     /**< The system status is changed. */
    BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED = 0x08,/**< The player application settings is changed. */

    BT_AVRCP_EVENT_UNDEFINED = 0XFF
} bt_avrcp_register_notification_event_t;

/**
 * @brief Play status type.
 */
typedef enum {
    BT_AVRCP_PLAY_STATUS_STOPPED = 0x00,             /**< Status: Stopped. */
    BT_AVRCP_PLAY_STATUS_PLAYING = 0x01,             /**< Status: Playing. */
    BT_AVRCP_PLAY_STATUS_PAUSED = 0x02,              /**< Status: Paused. */
    BT_AVRCP_PLAY_STATUS_FWD_SEEK = 0x03,            /**< Status: Seek Forward. */
    BT_AVRCP_PLAY_STATUS_REV_SEEK = 0x04,            /**< Status: Seek Rewind. */

    BT_AVRCP_PLAY_STATUS_ERROR = 0xFF                /**< Status: Error. */
} bt_avrcp_play_status_t;

/**
 * @}
 */

/**
 * @defgroup bt_avrcp_struct Struct
 * @{
 */
/**
 * @brief Structure for the AVRCP initialization parameters.
 */
typedef struct {
    bt_avrcp_role_t device_role;                                   /**< Local supported role, only the CT is available in V1.3.0. */
    bt_avrcp_support_feature_t ct_supported_feature;               /**< Local supported CT features. */
    bt_avrcp_support_feature_t tg_supported_feature;               /**< Local supported TG features. */
} bt_avrcp_init_params_t;

/**
 * @brief The connect confirmation structure for event #BT_AVRCP_CONNECT_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    uint16_t conn_id;                                              /**< Current connection ID. */
    bt_address_t device_addr;                                      /**< Bluetooth address of remote device. */
    bt_avrcp_support_feature_t remote_ct_support_feature;          /**< Supported CT features of remote device. */
    bt_avrcp_support_feature_t remote_tg_support_feature;          /**< supported TG features of remote device. */
} bt_avrcp_connect_cnf_t;

/**
 * @brief The connect indication structure for event #BT_AVRCP_CONNECT_IND.
 */
typedef struct {
    bt_address_t device_addr;                                      /**< Bluetooth address of remote device. */
} bt_avrcp_connect_ind_t;

/**
 * @brief The disconnect indication structure for event #BT_AVRCP_DISCONNECT_IND.
 */
typedef struct {
    uint16_t conn_id;                                               /**< Current connection ID. */
} bt_avrcp_disconnect_ind_t;

/**
 * @brief The pass through confirmation structure for event #BT_AVRCP_PASS_THROUGHT_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    uint16_t conn_id;                                              /**< Current connection ID. */
    bt_avrcp_key_state_t key_state;                                /**< operation state value (Pressed or Released). */
    bt_avrcp_command_opcode_t key_code;                             /**< Operation code. */
} bt_avrcp_pass_through_cnf_t;

/**
 * @brief The list application attributes confirmation structure for event #BT_AVRCP_LIST_APP_ATTRIBUTES_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    uint16_t conn_id;                                              /**< Current connection ID. */
    uint8_t count;                                                 /**< The number of attributes provided. */
    bt_avrcp_app_attributes_t attribute_id[BT_AVRCP_MAX_APP_ATTRIBUTE_NUM]; /**< The player application setting attribute IDs. */
} bt_avrcp_list_app_attributes_cnf_t;

/**
 * @brief The list application value confirmation structure for event #BT_AVRCP_LIST_APP_VALUE_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    bt_avrcp_app_attributes_t attribute_id;                        /**< The player application setting attribute ID. */
    uint16_t conn_id;                                              /**< Current connection ID. */
    uint8_t count;                                                 /**< The number of player application setting values provided. */
    uint8_t attribute_values[BT_AVRCP_MAX_APP_ATTRIBUTE_VALUE_NUM];/**< The player application setting value IDs. */
} bt_avrcp_list_app_value_cnf_t;

/**
 * @brief The get application value confirmation structure for event #BT_AVRCP_GET_APP_VALUE_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    uint16_t conn_id;                                              /**< Current connection ID. */
    uint8_t count;                                                 /**< The number of player application settings value provided. */
    bt_avrcp_app_attributes_t attribute_ids[BT_AVRCP_MAX_APP_ATTRIBUTE_VALUE_NUM]; /**< The player application setting attribute IDs for which a value is returned. */
    uint8_t attribute_values[BT_AVRCP_MAX_APP_ATTRIBUTE_VALUE_NUM];                /**< Player application setting values set on the target for the
                                                                                    *   corresponding requested player application setting attribute IDs*/
} bt_avrcp_get_app_value_cnf_t;

/**
 * @brief The set application value confirmation structure for event #BT_AVRCP_SET_APP_VALUE_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    uint16_t conn_id;                                              /**< Current connection ID. */
} bt_avrcp_set_app_value_cnf_t;

/**
 * @brief The get attribute text confirmation structure for event #BT_AVRCP_GET_ATTRIBUTE_TEXT_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    uint8_t count;                                                 /**< The number of player application setting attribute IDs with required corresponding strings. */
    uint16_t conn_id;                                              /**< Current connection ID. */
    bt_avrcp_app_attributes_t attribute_ids[BT_AVRCP_MAX_APP_ATTRIBUTE_NUM]; /**< The player application setting attribute IDs with a returned text to display. */
    uint16_t charset[BT_AVRCP_MAX_APP_ATTRIBUTE_NUM];                        /**< The character set ID for each attribute to display on the CT. */
    uint8_t attribute_string_len[BT_AVRCP_MAX_APP_ATTRIBUTE_NUM];            /**< The length of each player application setting attribute string. */
    uint8_t attribute_string[BT_AVRCP_MAX_APP_ATTRIBUTE_NUM][BT_AVRCP_MAX_APP_ATTRIBUTE_STRING_SIZE]; /**< The player application setting attribute
                                                                                                       *   strings in specified character set. */
} bt_avrcp_get_attribute_text_cnf_t;

/**
 * @brief The get value text confirmation structure for event #BT_AVRCP_GET_VALUE_TEXT_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    uint8_t count;                                                 /**< The number of player application settings value provided. */
    uint16_t conn_id;                                              /**< Current connection ID. */
    bt_avrcp_app_attributes_t attribute_id;                        /**< The player application setting attribute ID. */
    uint16_t charset[BT_AVRCP_MAX_APP_ATTRIBUTE_VALUE_NUM];        /**< The character set IDs for each value to display on the CT. */
    uint8_t value_ids[BT_AVRCP_MAX_APP_ATTRIBUTE_VALUE_NUM];       /**< The player application setting value IDs. */
    uint8_t value_string_len[BT_AVRCP_MAX_APP_ATTRIBUTE_VALUE_NUM];/**< The length of each player application setting value string. */
    uint8_t value_string[BT_AVRCP_MAX_APP_ATTRIBUTE_VALUE_NUM][BT_AVRCP_MAX_APP_ATTRIBUTE_STRING_SIZE]; /**< The player application setting value strings
                                                                                                         *   in specified character set. */
} bt_avrcp_get_value_text_cnf_t;

/**
 * @brief Structure for element atrributes.
 */
typedef struct {
    uint32_t  attribute_id;                                        /**< The attribute ID to be written. */
    uint16_t  charset;                                             /**< The character set ID to be displayed on the CT. */
    uint16_t  value_len;                                           /**< Length of the attribute's value. */
    uint16_t  value_text_offset;                                   /**< Offset of the attribute name in specified character set in the content table. */
} bt_avrcp_element_attributes_t;

/**
 * @brief The get elment attributes confirmation structure for event #BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF.
 */
typedef struct {
    bt_status_t result;                                                             /**< General result of the confirm. */
    uint8_t count;                                                                  /**< The number of attributes provided. */
    uint16_t conn_id;                                                               /**< Current connection ID. */
    bt_avrcp_element_attributes_t elements[BT_AVRCP_MAX_ELEMENT_ATTRIBUTE_NUM];     /**< The elment attributes table. */
    uint8_t element_data[BT_AVRCP_MAX_ELEMENET_ATTRIBUTE_BUFFER_SIZE];              /**< The string elment attributes table. */
} bt_avrcp_get_element_attributes_cnf_t;

/**
 * @brief The get player status confirmation structure for event #BT_AVRCP_GET_PLAYER_STATUS_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    uint16_t conn_id;                                              /**< Current connection ID. */
    uint32_t song_length;                                          /**< Duration of the playing song in milliseconds. */
    uint32_t song_pos;                                             /**< The number of milliseconds elapsed from the start of playing. */
    bt_avrcp_play_status_t play_status;                            /**< The current status of playing. */
} bt_avrcp_get_player_status_cnf_t;

/**
 * @brief The register notification confirmation structure for event #BT_AVRCP_REGISTER_NOTIFICATION_CNF.
 */
typedef struct {
    bt_status_t result;                                            /**< General result of the confirm. */
    uint16_t conn_id;                                              /**< Current connection ID. */
    bool is_interim;                                               /**< The AV/C interim as an initial response to this notify command. */
    bt_avrcp_register_notification_event_t event_id;               /**< The notification registeration event. */
    uint8_t status;                                                /**< The status for #BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, #BT_AVRCP_EVENT_BATT_STATUS_CHANGED and #BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED. */
    uint8_t identifier[8];                                         /**< The index of the current track for event #BT_AVRCP_EVENT_TRACK_CHANGED. */
    uint8_t count;                                                 /**< The number of player application setting attributes that follow for #BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED. */
    bt_avrcp_app_attributes_t attribute_id[BT_AVRCP_MAX_APP_ATTRIBUTE_NUM];  /**< The player application setting attribute IDs with returned values, for #BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED. */
    uint8_t attribute_value[BT_AVRCP_MAX_APP_ATTRIBUTE_NUM];       /**< The player application setting values on the target for the corresponding
                                                                    *   requested player application setting attribute IDs for #BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED. */
    uint32_t pos;                                                  /**< Current playback position in milliseconds for #BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED. */
} bt_avrcp_register_notification_cnf_t;

/**
 * @}
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief                 This function connects to the specified remote
 *                        device. #BT_AVRCP_CONNECT_CNF event is sent to upper layer
 *                        with the connect request result.
 * @param[in] device_addr is the pointer to the remote device's address.
 * @param[in] role        is the role played by local device.
 * @return                If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 * @sa                    #bt_avrcp_disconnect()
 */
bt_status_t bt_avrcp_connect(bt_address_t *device_addr, bt_avrcp_role_t role);

/**
 * @brief                 This function responds to the specified remote device's
 *                        incoming connection. #BT_AVRCP_CONNECT_CNF event is sent to
 *                        upper layer with the result of connect request.
 * @param[in] device_addr is the pointer to the remote device's address.
 * @param[in] accept      is whether to accept or reject to the remote device's connection request.
 * @return                If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_connect_response(bt_address_t *device_addr, bool accept);

/**
 * @brief             This function disconnects the specified remote device.
 * @param[in] conn_id is the identifier of the remote device.
 * @return            If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 * @sa                #bt_avrcp_connect()
 */
bt_status_t bt_avrcp_disconnect(uint16_t conn_id);

/**
 * @brief               This function sends pass through command to
 *                      specified TG device. #BT_AVRCP_PASS_THROUGHT_CNF event is
 *                      sent to the upper layer with the request result.
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[in] key_code is the operation code.
 * @param[in] key_code is the operation state.
 * @return              If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_send_pass_through_command(uint16_t conn_id,
        bt_avrcp_command_opcode_t key_code, bt_avrcp_key_state_t key_state);

/**
 * @brief             This function gets player application attributes
 *                    supported at TG device. #BT_AVRCP_LIST_APP_ATTRIBUTES_CNF event
 *                    is sent to the upper layer with the request result.
 * @param[in] conn_id is the identifier of the remote device.
 * @return            If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_list_app_attributes(uint16_t conn_id);

/**
 * @brief             This function gets the player application values
 *                    for a specified attribute at TG device. #BT_AVRCP_LIST_APP_VALUE_CNF event
 *                    is sent to the upper layer with the request result.
 * @param[in] conn_id is the identifier of the remote device.
 * @param[in] attribute_id is the specified attribute's ID.
 * @return            If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_list_app_value(uint16_t conn_id, bt_avrcp_app_attributes_t attribute_id);

/**
 * @brief             This function queries the player application setting values
 *                    for specified attributes at TG device. #BT_AVRCP_GET_APP_VALUE_CNF event
 *                    is sent to the upper layer with the request result.
 * @param[in] conn_id is the identifier of the remote device.
 * @param[in] attributes_size is the number of the queried attributes.
 * @param[in] attributes_ids is the specified attribute's ID.
 * @return            If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_get_app_value(uint16_t conn_id, uint8_t attributes_size, uint8_t attributes_ids[]);

/**
 * @brief             This function sets the player application setting values
 *                    to specified attributes at TG device. #BT_AVRCP_SET_APP_VALUE_CNF event
 *                    is sent to upper layer with the request result.
 * @param[in] conn_id is the identifier of the remote device.
 * @param[in] attributes_size is the number of the attribute settings.
 * @param[in] attributes_ids is a pointer to the specified attributes.
 * @param[in] attributes_value is a pointer to the attribute value settings.
 * @return            If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_set_app_value(uint16_t conn_id, uint8_t attributes_size, uint8_t attributes_ids[], uint8_t attributes_value[]);

/**
 * @brief             This function requests the TG device to provide
 *                    the displayable text for the player application setting
 *                    attributes. #BT_AVRCP_GET_ATTRIBUTE_TEXT_CNF event
 *                    is sent to upper layer with the request result.
 * @param[in] conn_id is the identifier of the remote device.
 * @param[in] attributes_size is the number of the attributes.
 * @param[in] attributes_ids is a pointer to the specified attributes' ID.
 * @return            If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_get_app_attribute_text(uint16_t conn_id, uint8_t attributes_size, uint8_t attributes_ids[]);

/**
 * @brief             This function requests the TG device to provide
 *                    the displayable text for the player application setting
 *                    value. #BT_AVRCP_GET_VALUE_TEXT_CNF
 *                    event is sent to the upper layer with the request result.
 * @param[in] conn_id is the identifier of the remote device.
 * @param[in] attribute_id is the identifier to the specified attribute.
 * @param[in] value_size is the number of the listed attribute values.
 * @param[in] attribute_values is a pointer to the attribute values provided.
 * @return            If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */

bt_status_t bt_avrcp_get_app_value_text(uint16_t conn_id, bt_avrcp_app_attributes_t attribute_id,
                                        uint8_t value_size, uint8_t attribute_values[]);

/**
 * @brief             This function obtains the detailed information on a
 *                    particular media file playing currently at TG device. The event
 *                    #BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF is sent to the upper layer
 *                    with the request result.
 * @param[in] conn_id is the identifier of the remote device.
 * @param[in] attributes_size is the number of the quiried attributes.
 * @param[in] attributes_id is the pointer to specified element attributes.
 * @return            If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_get_element_attributes(uint16_t conn_id, uint8_t attributes_size, uint32_t attributes_id[]);

/**
 * @brief             This function gets the status of the currently
 *                    playing media at TG device. The event #BT_AVRCP_GET_PLAYER_STATUS_CNF
 *                    is sent to the upper layer with the request result.
 * @param[in] conn_id is the identifier of the remote device.
 * @return            If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_get_play_status(uint16_t conn_id);

/**
 * @brief                    This function sends a register notification command to the
 *                           TG device. #BT_AVRCP_REGISTER_NOTIFICATION_CNF event
 *                           is sent to the upper layer with the request result.
 * @param[in] conn_id        is the identifier of the remote device.
 * @param[in] event_id       is the related AVRCP event ID.
 * @param[in] playback_interval is the time interval (in seconds) at which the change in
 *                           playback position is noticed.
 * @return                   If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_register_notification(uint16_t conn_id, bt_avrcp_register_notification_event_t event_id,
        uint32_t playback_interval);

/**
 * @brief              This function is the common callback with indication or
 *                     response result to the up layer.
 * @param[in] event_id is the related AVRCP event ID.
 * @param[in] param    is the result.
 * @return             None
 */
void bt_avrcp_common_callback(bt_event_t event_id, const void *param);

/**
 * @brief            This function is to gets the initialization parameters. It is user defined.
 * @param[out] param is the initialization parameter. Provide the implementation if it is called in a profile.
 * @return           If the operation was successful, the returned value is #BT_STATUS_AVRCP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_avrcp_get_init_params(bt_avrcp_init_params_t *param);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif /* __BT_AVRCP_H__ */

