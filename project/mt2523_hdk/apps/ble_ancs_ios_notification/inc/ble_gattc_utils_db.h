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

#ifndef __BT_GATTC_UTILS_DB_H__
#define __BT_GATTC_UTILS_DB_H__
#include "gattc_utils.h"
#include "ble_gatt.h"
#include "ble_gap.h"
#include <stdint.h>
#include "syslog.h"

#ifndef __BLE_GATTC_SRV__
#define __BLE_GATTC_SRV__
#endif

#ifdef __BLE_GATTC_SRV__

#define GATTC_STATUS_SUCCESS   0
#define GATTC_STATUS_FAILED    -1
#define GATTC_STATUS_NO_RESOURCES -2
#define GATTC_STATUS_INVALID   -3

extern ListNode g_gatt_database_table;

typedef struct {
    ListNode database_list; /* used for data base node */
    uint16_t conn_id;
    ListNode service_list;/*used for service node*/
    ListNode research_list;
} gatt_service_database_t;

typedef struct {
    ListNode node;
    ListNode include_list; /*used for include service node*/
    ListNode char_list; /*used for characteristic node*/
    ListNode userlist; /*user for user node*/
    ble_gatt_service_t service;
} gatt_service_node_t;

typedef struct {
    ListNode node;
    ble_gatt_included_service_t include_service;
} gatt_include_service_node_t;


typedef struct {
    uint8_t *value;
    uint8_t value_size;
    uint8_t uuid[16];
    uint16_t handle;
} gatt_descriptor_t;

typedef struct {
    ListNode node;
    gatt_descriptor_t descriptor;
} gatt_char_desc_node_t;

typedef struct {
    uint8_t uuid[16];
    uint8_t property;
    uint16_t handle;
    uint16_t value_handle;
    uint8_t value_size;
    uint8_t *value;
} gatt_characteristic_t;

typedef struct {
    ListNode node;
    gatt_characteristic_t characteristic;
    ListNode char_desc_list; /*used for descriptor node*/
} gatt_char_node_t;

typedef struct {
    uint8_t uuid[16];
} gatt_uuid_t;

typedef struct {
    uint8_t is_primary;
    gatt_uuid_t uuid;
} gatt_srv_uuid_t;

/*gattc srv operation  database*/
gatt_service_node_t *ble_gattc_read_service_by_uuid(uint16_t conn_id, gatt_srv_uuid_t *svc_uuid);

gatt_service_node_t *ble_gattc_spec_service_node_by_uuid(gatt_service_database_t *servicedb, gatt_uuid_t *uuid);

gatt_service_node_t *ble_gattc_next_service_node_by_uuid(gatt_service_database_t *servicedb, gatt_service_node_t *service_node, uint8_t *uuid);

gatt_include_service_node_t *ble_gattc_find_include_service(gatt_service_node_t *service_node, gatt_srv_uuid_t *include_uuid);

gatt_include_service_node_t *ble_gattc_find_include_service_by_service_uuid(gatt_service_node_t *service_node, gatt_srv_uuid_t *include_uuid);

gatt_include_service_node_t *ble_gattc_find_next_include_service_by_uuid(gatt_service_node_t *service_node, gatt_include_service_node_t *include_node, gatt_srv_uuid_t *include_uuid);

gatt_char_node_t *ble_gattc_find_characteristic(gatt_service_node_t *srv_node, gatt_uuid_t *char_uuid);

gatt_char_node_t *ble_gattc_find_characteristic_by_uuid(gatt_service_node_t *service_node, gatt_uuid_t *char_uuid);

gatt_char_node_t *ble_gattc_find_next_characteristic_by_uuid(gatt_service_node_t *service_node, gatt_char_node_t *char_node, gatt_uuid_t *char_uuid);

gatt_char_desc_node_t *ble_gattc_find_descriptor_by_uuid(gatt_char_node_t *char_node, gatt_uuid_t *descri_uuid);

gatt_char_desc_node_t *ble_gattc_find_next_descriptor_by_uuid(gatt_char_node_t *char_node, gatt_char_desc_node_t *descri_node, gatt_uuid_t *descri_uuid);

uint16_t ble_gattc_read_service_by_uuid_from_db(gatt_service_database_t *servicedb, ble_gatt_service_t *srv);

gatt_service_node_t *ble_gattc_create_new_service_node(ble_gatt_service_t *service);

gatt_include_service_node_t *ble_gattc_create_new_include_service_node(ble_gatt_included_service_t *service);

gatt_char_node_t *ble_gattc_create_new_char_node(ble_gatt_char_t *char_info);

gatt_char_desc_node_t  *ble_gattc_create_new_descriptor_node(ble_gatt_descriptor_t *descriptor);

gatt_service_node_t *ble_gattc_check_service_handle_range(uint16_t conn_id, uint16_t handle);

gatt_char_node_t *ble_gattc_check_char_handle_range(gatt_service_node_t *service_node, uint16_t handle);

gatt_char_desc_node_t *ble_gattc_check_descr_by_handle(gatt_char_node_t *char_node, uint16_t handle);

void  ble_gatt_free_service_node(gatt_service_node_t *service_node);

gatt_service_database_t *ble_gattc_query_database_is_exist(uint16_t conn_id);

uint16_t ble_gattc_delete_service_datebase(ble_address_t *addr);

gatt_service_database_t *ble_gattc_create_new_database(uint16_t conn_id);

uint16_t ble_gattc_read_database(ble_address_t  *addr);

void ble_gattc_init_database();

void ble_gattc_free_database(gatt_service_database_t *service_db);

void ble_gattc_add_user_list_to_service_node(gatt_service_node_t *srv_node, uint16_t conn_id);

#endif
#endif
