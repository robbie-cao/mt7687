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

#ifndef __BT_GATT_HANDLE_OP_H__
#define __BT_GATT_HANDLE_OP_H__
#include "ble_gattc_utils_db.h"
#include "gattc_utils.h"
#include "ble_gatt.h"
#include "ble_status.h"
#include "stdint.h"
#include "syslog.h"

//#define __GATTC_SRV_TEST__

#define GATTC_MAX_USER 12
#define MAX_SUPPORT_SERVICE  40
#define __BT_MAX_LE_LINK_NUM__ 4
#define SRV_MAX_DEV  __BT_MAX_LE_LINK_NUM__// ble link num 

/*gattc current state*/
#define GATTC_IN_IDLE 0
#define GATTC_CONNECTED 1
#define GATTC_SEARCH_DONE   2
#define GATTC_IN_SEARCHING 3
#define GATTC_IN_RE_SEARCHING 4
#define GATTC_RE_SEARCH_DONE   5

typedef enum {
    GATTC_RESERACH_NO_CHANGE,
    GATTC_RESERACH_DELETE,
    GATTC_RESERACH_ADD,
    GATTC_RESERACH_MODIFY
} gattc_research_change_type_t;


typedef struct {
    uint16_t serviceNumber; /*current primary service number*/
    uint16_t totalSupportedServices;   /* Number of supported services found on the Server */
    uint16_t service_explore_idx;/* Index into serviceStore of service currently being discovered*/
    uint16_t start_handle;
    uint16_t end_handle;
    uint32_t conn_id;
    ble_gatt_service_t service_store[MAX_SUPPORT_SERVICE];     /* Stores all the supported services found on the Server */
} gatt_data_t;

/*Callback*/
typedef struct {
    void (*connSearchComplete)(void *param);
    void (*connIndNotifHandler)(ble_gattc_handle_value_ind_t *param);/*notify & ind*/
    void (*connReadchardescrCfm)(ble_gattc_read_char_descriptor_rsp_t *param);/*read rsp*/
    void (*conReadMultiCfm)(ble_gattc_read_multiple_char_value_rsp_t *param);
    void (*conWriteCfm)(ble_gattc_write_rsp_t *param);
    void (*conSetMtuCfm)(ble_gatt_exchange_mtu_rsp_t *param);
} gattc_conn_func_t;

typedef struct {
    ListNode node;/*for insert user list*/
    uint8_t addr[6];
    uint16_t state;
    uint16_t result;
    uint16_t conn_id;
    gatt_data_t data;
    gattc_conn_func_t conn_func;
} gattc_conn_t;

typedef struct {
    uint16_t handle;
    uint16_t offset;
} ble_gattc_read_lchar_req_t;

typedef struct {
    uint16_t start_handle;
    uint16_t end_handle;
    uint8_t uuid[128];
} ble_gattc_read_char_by_uuid_req_t;

typedef struct {
    uint16_t *handle;
    uint16_t count;
} ble_gattc_read_multi_char_req_t;

typedef struct {
    uint8_t *value;
    uint16_t handle;
    uint16_t size;
    ble_gatt_write_type_t type;
} ble_gattc_write_char_req_t;

typedef struct {
    uint8_t *value;
    uint16_t handle;
    uint16_t size;
} ble_gattc_write_descr_req_t;

typedef struct {
    uint16_t conn_id;
    uint16_t handle;
    uint16_t result;
    uint16_t offset;
    uint8_t *value;
    uint16_t type;
} ble_gattc_write_rsp_params_t;

typedef struct {
    uint16_t conn_id;
    uint16_t start_handle;
    uint16_t end_handle;
} gattc_research_service_req_t;

typedef struct {
    ListNode node;
    ListNode userlist;
    ble_gatt_service_t service;
} gatt_research_service_node_t;


/*start discovery service from remote device*/
int32_t ble_gattc_start_discover_service(uint16_t conn_id);

/*discobery pimary service by uuid from remote device*/
ble_status_t ble_gattc_discover_service_by_uuid (uint16_t conn_id, uint8_t *uuid);

/*read characteristic by multi read type from remote device*/
ble_status_t ble_gattc_read_multiple_characteristics_value(uint16_t conn_id, uint16_t count, uint16_t *handle);

/*read characteristic by charateristic's uuid from remote device*/
ble_status_t ble_gattc_read_using_char_uuid(uint16_t conn_id,
        uint16_t start_handle,
        uint16_t end_handle,
        uint8_t *uuid128);

/*read descriptor's value from remote device*/
ble_status_t ble_gattc_read_descr(uint16_t conn_id, uint16_t handle);

/*set mtu sie req to remote device*/
ble_status_t ble_gattc_set_mtu(uint16_t conn_id, uint16_t mtu_size);

/*indication's rsp to remote device*/
ble_status_t ble_gattc_handle_confirm(uint16_t conn_id, uint16_t handle);

/*write charateristic's value*/
ble_status_t  ble_gattc_write_char(uint16_t conn_id, ble_gattc_write_char_req_t *req);

/*write descriptor's value*/
ble_status_t ble_gattc_write_descr(uint16_t conn_id, ble_gattc_write_descr_req_t *req);

/*read characterisitic's value*/
ble_status_t ble_gattc_read_char(uint16_t conn_id, uint16_t handle);

/*characteristic's value is > mtu size , read characteristic's value by read long type*/
ble_status_t ble_gattc_read_long(uint16_t conn_id, ble_gattc_read_lchar_req_t *req);

void ble_gattc_client_event_handler(ble_event_t event_id, const void *param);

#endif
