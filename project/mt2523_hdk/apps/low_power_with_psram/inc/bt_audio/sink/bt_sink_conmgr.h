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

#ifndef BT_SINK_CONMGR_H
#define BT_SINK_CONMGR_H

#include <stdint.h>
#include "bt_gap.h"
#include "bt_sink_resource.h"

#define BT_SINK_CM_MAX_PROFILE_NUMBER    4
#define BT_SINK_CM_MAX_DEVICE_NUMBER    2
#define BT_SINK_CM_MAX_TRUSTED_DEV      6
#define BT_SINK_CM_DEVICE_ID_UNAVAILABLE 0xFF
#define BT_SINK_CM_DISCOVERABLE_DURATION 90000

#define BT_SINK_UUID_HF                     (0x111E)
#define BT_SINK_UUID_A2DP                   (0x110B)
#define BT_SINK_UUID_AVRCP                  (0x110C)
#define BT_SINK_UUID_PBAP                   (0x112F)

// database constants
#define BT_SINK_CM_DB_NAME  "BT_Audio_Demo"
#define BT_SINK_CM_DB_IO    BT_GAP_IO_NO_INPUT_NO_OUTPUT
#define BT_SINK_CM_DB_MITM  true
#define BT_SINK_CM_DB_MODE  BT_GAP_BONDING_MODE_DEDICATED
#define BT_SINK_CM_COD (BT_GAP_COD_AUDIO|BT_GAP_COD_RENDERING|BT_GAP_COD_MAJOR_AUDIO|BT_GAP_COD_MINOR_COMP_DESKTOP)

typedef enum {
    BT_SINK_CM_PROFILE_HF       = 0,
    BT_SINK_CM_PROFILE_A2DP     = 1,
    BT_SINK_CM_PROFILE_AVRCP    = 2,
    BT_SINK_CM_PROFILE_PBAP     = 3
} bt_sink_cm_profile_type_t;

typedef enum {
    BT_SINK_FLAG_POWER_ON              = 0x01,
    BT_SINK_FLAG_SWITCHING             = 0x02,
    BT_SINK_FLAG_PAGE_SCAN             = 0x04,
    BT_SINK_FLAG_PAGE_INQRUIRY_SCAN    = 0x08,
    BT_SINK_FLAG_AUTHENTICATION_FAIL   = 0x10,
    BT_SINK_FLAG_BONDING_FAIL          = 0x20
} bt_sink_cm_flags_t;

typedef enum {
    BT_SINK_STATE_DISCONNECTED,
    BT_SINK_STATE_CONNECTING,
    BT_SINK_STATE_CONNECTED
} bt_sink_cm_link_state_t;

typedef enum {
    BT_SINK_PROFILE_STATE_DISCONNECTED,
    BT_SINK_PROFILE_STATE_CONNECTING,
    BT_SINK_PROFILE_STATE_CONNECTED,
    BT_SINK_PROFILE_STATE_AUDIO_ON
} bt_sink_cm_profile_state_t;

typedef enum {
    BT_SINK_CM_STATE_DISCONNECTED,
    BT_SINK_CM_STATE_CONNECTING,
    BT_SINK_CM_STATE_CONNECTED,
    BT_SINK_CM_STATE_DISCONNECTING,
    BT_SINK_CM_STATE_END
} bt_sink_cm_state_t;

typedef enum {
    BT_SINK_CM_FIND_BY_ID,
    BT_SINK_CM_FIND_BY_ADDR
} bt_sink_cm_find_by_type_t;

typedef enum {
    BT_SINK_CM_REQ_TYPE_NONE,
    BT_SINK_CM_REQ_TYPE_NORMAL,
    BT_SINK_CM_REQ_TYPE_POWER_ON,
    BT_SINK_CM_REQ_TYPE_LINK_LOST,
    BT_SINK_CM_REQ_TYPE_TOTAL
} bt_sink_cm_req_type_t;

typedef struct {
    bt_address_t bt_addr;
} bt_sink_cm_common_record;

typedef struct {
    void *data;
    uint16_t uuid;
    uint8_t state;
} bt_sink_profile_t;

typedef struct {
    uint8_t state;
    uint16_t conn_id;
    bt_address_t addr;
    bt_sink_profile_t *profile[BT_SINK_CM_MAX_PROFILE_NUMBER];
} bt_sink_cm_remote_device_t;

typedef struct {
    uint8_t speaker_volume;
} bt_sink_hf_stored_data_t;

typedef void (*bt_sink_cm_req_callback)(bt_sink_status_t result, void *parameters);

typedef struct {
    bt_sink_cm_req_type_t type;
    bt_sink_cm_req_callback callback;
    uint32_t delay_time;
    bt_address_t addr;
    uint8_t attampts;
} bt_sink_cm_request_t;

typedef struct _bt_sink_cm_node_t {
    bt_sink_cm_request_t *parameters;
    struct _bt_sink_cm_node_t *prev;
    struct _bt_sink_cm_node_t *next;
} bt_sink_cm_node_t;

#define BT_SINK_HFP_STORAGE_SIZE sizeof(bt_sink_hf_stored_data_t)
#define BT_SINK_A2DP_STORAGE_SIZE 1
#define BT_SINK_REQUEST_DELAY_TIME (3000)
#define BT_SINK_LINK_LOST_RECONNECT_DELAY (5000)
#define BT_SINK_REQUEST_DELAY_TIME_INCREASE (15000)

typedef struct {
    bt_address_t bdAddr;
    bool trusted;
    uint8_t keyType;
    uint8_t linkKey[16];
#ifdef BT_SINK_HFP_STORAGE_SIZE
    uint8_t hfp_info[BT_SINK_HFP_STORAGE_SIZE];
#endif
#ifdef BT_SINK_A2DP_STORAGE_SIZE
    uint8_t a2dp_info[BT_SINK_A2DP_STORAGE_SIZE];
#endif
#ifdef BT_SINK_AVRCP_STORAGE_SIZE
    uint8_t avrcp_info[BT_SINK_AVRCP_STORAGE_SIZE];
#endif
#ifdef BT_SINK_PBAP_STORAGE_SIZE
    uint8_t pbap_info[BT_SINK_PBAP_STORAGE_SIZE];
#endif
} bt_sink_cm_dev_info_t;

typedef struct {
    uint8_t flags;
    uint8_t connected_dev;

    bt_sink_cm_node_t *request_pending;
    bt_sink_cm_node_t request_list;

    bt_sink_cm_remote_device_t devices[BT_SINK_CM_MAX_DEVICE_NUMBER];
} bt_sink_cm_context_t;

void bt_sink_cm_connect(bt_address_t *addr);

void bt_sink_cm_disconnect(bt_address_t *addr);

void bt_sink_cm_link_connected(const bt_address_t *dev_addr, uint16_t conn_id);

void bt_sink_cm_link_disconnected(const bt_address_t *dev_addr, uint16_t conn_id);

bool bt_sink_cm_get_profile_data(uint16_t conn_id, uint16_t uuid, void *data_p);

bool bt_sink_cm_set_profile_data(uint16_t conn_id, uint16_t uuid, void *data_p);

uint32_t bt_sink_cm_get_connected_device(uint16_t uuid, uint16_t device_list[]);

void *bt_sink_cm_find_profile_by_id(uint16_t id, uint16_t uuid);

void *bt_sink_cm_find_profile_by_address(bt_address_t *bt_addr, uint16_t uuid);

bt_sink_cm_remote_device_t *bt_sink_cm_find_device(bt_sink_cm_find_by_type_t type, void *param);

void bt_sink_cm_init(void);

void bt_sink_cm_deinit(void);

bt_sink_status_t bt_sink_cm_event_handler(bt_sink_event_id_t event, void *param);

void bt_sink_cm_gap_callback(bt_event_t event_id, void *param);

bt_status_t bt_sink_cm_get_local_name(char *name);

uint32_t bt_sink_cm_get_local_cod(void);

bt_status_t bt_sink_cm_read_local_address(bt_address_t *local_addr);

void bt_sink_cm_atci_init(void);

bt_status_t bt_sink_cm_request_link_key(const bt_address_t *remote_addr, uint8_t *key, uint32_t key_len);

#endif
