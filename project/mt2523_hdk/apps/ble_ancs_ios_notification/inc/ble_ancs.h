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

#include "ble_gattc_utils_db.h"

#define BLE_ANCS_CHAR_NUM 3                       /*The number of characteristics */
#define BLE_ANCS_DESCRIPTOR_VALUE_LEN 2                     /*The length of descriptor*/

typedef enum {
    CUT_NONE            = 0,
    CUT_ATTR_ID         = 1,                            /*Cut from the beginning of attribute id*/
    CUT_ATTR_LEN        = 2,                            /*Cut from the beginning of attribute length*/
    CUT_ATTR_LENM       = 3,                            /*Cut from the middle of attribute length*/
    CUT_ATTR_VAL        = 4,                            /*Cut from the beginning or middle of attribute data*/
    CUT_APP_ID          = 5,
    CUT_APP_IDD         = 6
} ble_ancs_cut_type_t;

typedef enum {
    /*DON'T change the enum value*/
    BLE_ANCS_ACT_NONE                   = 0,
    BLE_ANCS_ACT_ENABLE_DATA_SOURCE     = 1,
    BLE_ANCS_ACT_ENABLE_NOTIF_SOURCE    = 2,
    BLE_ANCS_ACT_DISABLE_DATA_SOURCE    = 3,
    BLE_ANCS_ACT_DISABLE_NOTIF_SOURCE   = 4,
    BLE_ANCS_ACT_GET_NOTIF_ATTRIBUTE    = 5,
    BLE_ANCS_ACT_GET_APP_ATTRIBUTE      = 6,
    BLE_ANCS_ACT_PERFORM_ACTION         = 7,
    BLE_ANCS_ACT_END_OF_TYPE
} ble_ancs_action_type_t;

typedef enum {
    BLE_ANCS_PRIMARY_SERVICE_UUID        = 0,
    BLE_ANCS_NOTIFICATION_SOURCE_UUID    = 1,
    BLE_ANCS_CONTROL_POINT_UUID          = 2,
    BLE_ANCS_DATA_SOURCE_UUID            = 3,
    BLE_ANCS_UUID_END_OF_TYPE
} ble_ancs_uuid_type_t;


typedef struct {
    uint8_t attr_id;
    uint16_t real_len;                                  /*The real length of the attribute received*/
    uint16_t max_len;                                   /*The length of the data buffer, it is the max length of the parameter*/
    uint8_t *data;
} ble_ancs_attr_node_t;

typedef struct {
    uint8_t uuid[4];
    uint16_t appid_len;
    uint8_t *appid;
    ble_ancs_cut_type_t cut_type;
    uint16_t left_to_read;                              /*the number of bytes to read in next package */
    uint16_t req_attr;                                  /*a flag which attribute is requested*/
    uint8_t attr_num;                                   /*the nubmer of attributes to request*/
    ble_ancs_attr_node_t attr_list[ANCS_NOTIFICATION_ATTR_NUM];
    ble_ancs_attr_node_t *curr_attr;
} ble_ancs_notification_context_t;

typedef struct {
    ble_ancs_uuid_type_t uuid_type;
    gatt_uuid_t char_uuid;
    gatt_uuid_t desc_uuid;                              /*descriptor uuid*/
    uint16_t desc_len;                                  /*descriptor value length*/
    uint8_t desc_value[BLE_ANCS_DESCRIPTOR_VALUE_LEN];      /*descriptor value*/
} ble_ancs_char_t;

typedef struct {
    uint16_t conn_id;
    ble_ancs_action_type_t action_type;
    ble_ancs_char_t char_content[BLE_ANCS_CHAR_NUM];
    /*the context of the notification in process now: getting notification attribute, getting app attribute, etc.*/
    ble_ancs_notification_context_t curr_notif;
} ble_ancs_conn_context_t;

typedef struct {
    void *reg_cntx;                                     /*pointer to the gattc service register context*/
    ble_ancs_event_handler_t evt_handler;
    ble_ancs_conn_context_t conn_cntx[BLE_ANCS_MAX_CONN];
} ble_ancs_context_t;


