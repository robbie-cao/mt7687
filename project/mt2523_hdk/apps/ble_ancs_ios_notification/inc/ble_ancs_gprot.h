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

#include "stdint.h"
#include "ble_events.h"
#include "ble_status.h"


#define BLE_ANCS_MAX_CONN    1                          /* Only support 1 link */

#define ANCS_EVENT_ID_NUM 3                             /*added, modified and removed*/
#define ANCS_CATEGORY_ID_NUM 12

#define ANCS_NOTIFICATION_ATTR_NUM 8                    /*The number of notificaiton attribute*/

typedef enum {
    BLE_ANCS_ACTION_ID_POSITIVE = 0,
    BLE_ANCS_ACTION_ID_NEGATIVE = 1,
    BLE_ANCS_ACTION_ID_END_OF_ENUM
} ble_ancs_action_id_t;     /*only supported by IOS8.0 or later*/

typedef enum {
    /*DON'T change the enum value*/
    ANCS_ENABLE_DATA_SOURCE,
    ANCS_ENABLE_NOTIF_SOURCE,
    ANCS_DISABLE_DATA_SOURCE,
    ANCS_DISABLE_NOTIF_SOURCE,
    ANCS_SWITCH_END_OF_ENUM
} ble_ancs_switch_type_t;

typedef enum {
    BLE_ANCS_CATEGORY_ID_OTHER              = 0,
    BLE_ANCS_CATEGORY_ID_INCOMINGCALL       = 1,
    BLE_ANCS_CATEGORY_ID_MISSEDCALL         = 2,
    BLE_ANCS_CATEGORY_ID_VOICEMAIL          = 3,
    BLE_ANCS_CATEGORY_ID_SOCIAL             = 4,
    BLE_ANCS_CATEGORY_ID_SCHEDULE           = 5,
    BLE_ANCS_CATEGORY_ID_EMAIL              = 6,
    BLE_ANCS_CATEGORY_ID_NEWS               = 7,
    BLE_ANCS_CATEGORY_ID_HEALTHANDFITNESS   = 8,
    BLE_ANCS_CATEGORY_ID_BUSINESSANDFINANCE = 9,
    BLE_ANCS_CATEGORY_ID_LOCATION           = 10,
    BLE_ANCS_CATEGORY_ID_ENTERTAINMENT      = 11,
    BLE_ANCS_CATEGORY_ID_END_OF_ENUM
} ble_ancs_category_id_t;

typedef enum {
    BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED    = 0,
    BLE_ANCS_EVENT_ID_NOTIFICATION_MODIFIED = 1,
    BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED  = 2,
    BLE_ANCS_EVENT_ID_END_OF_ENUM
} ble_ancs_event_id_t;

typedef enum {
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER           = 0,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE                    = 1,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_SUBTITLE                 = 2,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE                  = 3,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE_SIZE             = 4,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_DATE                     = 5,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_POSITIVE_ACTION_LABEL    = 6,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_NEGATIVE_ACTION_LABEL    = 7,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_END_OF_ENUM
} ble_ancs_notification_attribute_id_t;

typedef enum {
    BLE_ANCS_EVENT_DISCOVER_COMPLETED       = 0,
    BLE_ANCS_EVENT_IOS_NOTIFICATION         = 1,
    BLE_ANCS_EVENT_NOTIFICATION_ATTRIBUTE   = 2,
    BLE_ANCS_EVENT_APP_ATTRIBUTE            = 3,
    BLE_ANCS_EVENT_SWITCH_COMPLETED         = 4,
    BLE_ANCS_EVENT_PERFORM_ACTION_COMPLETED = 5,
    BLE_ANCS_EVENT_END_OF_ENUM
} ble_ancs_event_type_t;

typedef ble_status_t ble_ancs_result_t;
#define BLE_ANCS_SUCCESS            1
#define BLE_ANCS_RESULT_BASE        -0x02300
#define BLE_ANCS_DISCOVER_FAIL      -0x02300
#define BLE_ANCS_PARSE_FAIL         -0x02302
#define BLE_ANCS_BUSY               -0x02303
#define BLE_ANCS_WRONG_PAMAMETER    -0x02304
#define BLE_ANCS_FAIL               -0x02305

typedef struct {
    ble_ancs_notification_attribute_id_t attribute_id;
    uint16_t atrribute_len;
    uint8_t *data;
} ble_ancs_attribute_list_t;

typedef struct {
    uint8_t command_id;
    uint8_t notification_uid[4];
    uint8_t attr_num;
    ble_ancs_attribute_list_t attr_list[ANCS_NOTIFICATION_ATTR_NUM];
} ble_ancs_event_attribute_t;

typedef struct {
    ble_ancs_event_id_t event_id;
    uint8_t event_flags;
    ble_ancs_category_id_t category_id;
    uint8_t category_count;
    uint8_t notification_uid[4];
} ble_ancs_event_notification_t;

typedef struct {
    ble_ancs_event_type_t evt_type;
    uint16_t conn_id;
    ble_ancs_result_t result;
    union {
        ble_ancs_event_notification_t notification;
        ble_ancs_event_attribute_t attribute;
        ble_ancs_switch_type_t switch_type;
    } data;
} ble_ancs_event_t;

typedef void(* ble_ancs_event_handler_t)(ble_ancs_event_t *p_evt);


typedef struct {
    ble_ancs_event_handler_t evt_handler;
} ble_ancs_init_t;

ble_ancs_result_t ble_ancs_init(const ble_ancs_init_t *init_info);
ble_ancs_result_t ble_ancs_deinit();
ble_ancs_result_t ble_ancs_enable_notification_source(const uint16_t conn_id);
ble_ancs_result_t ble_ancs_disable_notification_source(const uint16_t conn_id);
ble_ancs_result_t ble_ancs_enable_data_source(const uint16_t conn_id);
ble_ancs_result_t ble_ancs_disable_data_source(const uint16_t conn_id);
ble_ancs_result_t ble_ancs_get_notification_attributes(const uint16_t conn_id,
        const uint8_t *uid, ble_ancs_attribute_list_t *attr_list, const uint8_t attr_num);
ble_ancs_result_t ble_ancs_get_app_attributes(const uint16_t conn_id,
        uint8_t *appid,
        const uint16_t appid_len,
        ble_ancs_attribute_list_t *attr_list,
        const uint8_t attr_num);
ble_ancs_result_t ble_ancs_perform_notification_action(const uint16_t conn_id,
        const uint8_t *uid, const ble_ancs_action_id_t action_id);


