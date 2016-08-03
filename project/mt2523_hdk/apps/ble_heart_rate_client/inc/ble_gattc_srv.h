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

#ifndef __BLE_GATTC_H__
#define __BLE_GATTC_H__

#include <stdint.h>
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_gattc_utils_db.h"
#include "ble_gattc_handle_op.h"
#include "ble_status.h"
#include "syslog.h"

/************************************************
*    GATTC request definition
*************************************************/

/* op type */
#define GATTC_READ_CHAR_RESULT  2
#define GATTC_READ_DESCR_RESULT 3
#define GATTC_WRITE_CHAR_RESULT 4
#define GATTC_WRITE_DESC_RESULT 5
#define GATTC_CONFIG_MTU_RESULT 6
#define GATTC_CHAR_IND  7
#define GATTC_DESCR_IND  8

#define GATTC_STATUS_SRV_NOT_FOUND -1
#define GATTC_STATUS_CHAR_NOT_FOUND -2
#define GATTC_STATUS_DESC_NOT_FOUND -3
#define GATTC_STATUS_BUSY_SEARCHING -4
#define GATTC_STATUS_BUSY_RESEARCHING -5


/*READ CHAR BY DIFFERENT TYPE*/
typedef enum {
    READ_CHAR,
    READ_CHAR_BY_UUID,
    READ_MULTI_CHAR,
    READ_LONG_CHAR
} gattc_read_char_type_t;

typedef struct {
    uint16_t len;
    uint8_t uuid[16];
} app_uuid_t;

/*WRITE A VALUT TO CHARACTERISTIC OR DESCRIPTOR*/
typedef struct {
    uint16_t len;
    uint8_t  value[512];/*this vaule length maybe change*/
} gattc_value_t;

/*USER UINT*/
typedef struct {
    void *reg_cntx; /*gattc srv alloc cntx to user by uuid*/
    uint16_t conn_id;
} gattc_user_connect_struct;

/*CHARACTERISTIC'S INFORMATION*/
typedef struct {
    gatt_srv_uuid_t svc_uuid; /*service uuid*/
    gatt_uuid_t char_uuid; /*charateristic uuid */
} gattc_char_info_struct;

/* user uint of  in gattc srv*/
typedef struct {
    ListNode node; /*for user List*/
    ListNode conn_list; /*after connect, for connect list */
    uint8_t *uuid; /*app care's service uuid*/
    uint8_t uuid_count; /*app care's service uuid's  number*/
    void *appCb; /*app callbcak*/
} gattc_user_context_t;

typedef struct {
    ListNode userlist;
    ListNode freeuserlist;
    gattc_user_context_t userContext[GATTC_MAX_USER];

    ListNode connlist;
    ListNode unusedconnlist;
    gattc_conn_t conntext[SRV_MAX_DEV];
} gattc_context_t;

/* CONNECT_REQ */
typedef struct {
    void *reg_ctx;
    uint8_t bd_addr[6]; /*address*/
    uint8_t peer_addr_type; //refer BLE_GAP_ADDRESS_TYPE_T
} gattc_connect_req_struct;

/* DISCONNECT_REQ */
typedef struct {
    gattc_user_connect_struct *conn;
    ble_address_t *bd_addr;
} gattc_disconn_req_struct;

/* GATTC_REISTER_REQ */
typedef struct {
    uint8_t *uuid; /*app care's service uuid*/
    uint8_t uuid_count; /*app care's service uuid's  number*/
} gattc_register_req_struct;

/* GATTC_GET_PRIMARY_SERVICE_REQ */
typedef struct {
    gattc_user_connect_struct *conn; /*register and represent link */

    gatt_srv_uuid_t uuid; /*service uuid */
} gattc_search_service_req_t;

/* GATTC_GET_INCL_SERVICE_SEARCH_REQ */
typedef struct {
    gattc_user_connect_struct *conn;

    gatt_srv_uuid_t *svc_uuid; /*service uuid */
    gatt_srv_uuid_t *incl_svc_uuid; /*include service uuid */
} gattc_search_include_service_req_t;

/* GATTC_GET_CHAR_REQ */
typedef struct {
    gattc_user_connect_struct *conn;
    gatt_srv_uuid_t *svc_uuid; /*service uuid */
    gatt_uuid_t *start_char_uuid; /*start characteristic uuid */
} gattc_get_char_req_t;

/*GATTC_GET_DESCR_REQ */
typedef struct {
    gattc_user_connect_struct *conn;
    gatt_srv_uuid_t *svc_uuid; /*service uuid */
    gatt_uuid_t *char_uuid; /*char uuid */
    gatt_uuid_t *start_descr_uuid; /*start descriptor uuid */
} gattc_get_descr_req_t;

/* GATTC_READ_DESCR_REQ */
typedef struct {
    gattc_user_connect_struct *conn;
    gatt_srv_uuid_t *svc_uuid;
    gatt_uuid_t *char_uuid;
    gatt_uuid_t *descr_uuid; /*descriptor uuid */
} gattc_read_descr_req_t;

/* GATTC_READ_CHAR_REQ */
typedef struct {
    gattc_user_connect_struct *conn;
    gatt_srv_uuid_t *svc_uuid; /*service uid*/
    gatt_uuid_t *char_uuid; /*characteristic uuid */
    uint8_t count; /*characteristic 's count*/
    uint16_t offset; /*when read long it will be used*/
    gattc_read_char_type_t type;
} gattc_read_char_req_t;

/* GATTC_WRITE_CHAR_REQ */
typedef struct {
    gattc_user_connect_struct *conn;
    gatt_srv_uuid_t *svc_uuid;
    gatt_uuid_t *char_uuid;
    ble_gatt_write_type_t write_type;
    uint16_t len; /*value length*/
    uint8_t value[512];
} gattc_write_char_req_t;

/* GATTC_WRITE_DESCR_REQ */
typedef struct {
    gattc_user_connect_struct *conn;
    gatt_srv_uuid_t *svc_uuid;
    gatt_uuid_t *char_uuid;
    gatt_uuid_t *desc_uuid;
    uint16_t len;
    uint8_t  value[512];
} gattc_write_descr_req_t;

/* GATTC_CONFIGURE_MTU_REQ */
typedef struct {
    gattc_user_connect_struct *conn;

    uint16_t mtu_size;
} gattc_cfg_mtu_t;

typedef struct {
    gattc_user_connect_struct *conn;
    gatt_srv_uuid_t *svc_uuid;
    gatt_uuid_t *char_uuid;
} gattc_hv_confirm_t;

typedef struct {
    gatt_srv_uuid_t svc_uuid;
    gatt_uuid_t char_uuid;
    gatt_uuid_t descr_uuid;
} gattc_descr_info_t;

typedef struct {
    uint8_t prop;
    gatt_uuid_t char_info;
} gatt_get_char_result_t;

typedef struct {
    ListNode node;
    gattc_user_context_t *user;
} gatt_user_service_node_t;



typedef void *gattc_reg_handle;
extern gattc_context_t g_gattc_ctx;
#define GATTC(s) (g_gattc_ctx.s)

/*for callback done*/

/*this function will be callled when  client  has  scanned some remote devices information  */
typedef void (*app_scan_callback)(void *reg_cntx, ble_gap_advertising_ind_t *param);

/*this function will be called when client has  connected or disconnected with a remote device */
typedef void (*app_connect_callback)(gattc_user_connect_struct *conn, uint16_t connected, ble_address_t *bd_addr, uint16_t is_ind);

/* this function will be called when client  recevieved  a nofity or indication from remote device*/
typedef void (*app_noti_callback)(gattc_user_connect_struct *conn, gattc_char_info_struct *char_info, gattc_value_t *value, uint16_t is_notify);

/*this function will be called when  discovery service from remote device has been done*/
typedef void (*app_search_complete_callback)(gattc_user_connect_struct *conn, int32_t result);

typedef void (*app_research_service_callback)(gattc_user_connect_struct *conn, uint16_t result, gattc_research_change_type_t change_type);

/*this function will be called when the operation of writing  descriptor  has been responsed from remote device */
typedef void (*app_write_descriptor_callback)(gattc_user_connect_struct *conn, int32_t result, gattc_descr_info_t *descr_info);

/*this function will be called when the operation of writing  characteristic  has been responsed from remote device */
typedef void (*app_write_characteristic_callback)(gattc_user_connect_struct *conn, int32_t result, gattc_char_info_struct *char_info);

/*this function will be called when  the operation of reading  characteristic  has been responsed from remote device */
typedef void (*app_read_characteristic_callback)(gattc_user_connect_struct *conn, int32_t result,
        gattc_char_info_struct *char_info, gattc_value_t *value, uint16_t offset);

/*this function will be called when  the operation of reading  descriptor  has been responsed from remote device */
typedef void (*app_read_descriptor_callback)(gattc_user_connect_struct *conn, int32_t result,
        gattc_descr_info_t *descr_info, gattc_value_t *value);

/*gattc srv notify to user app */
typedef struct {
    app_scan_callback scan_cb;
    app_connect_callback connect_cb;
    app_search_complete_callback search_complete_cb;
    app_noti_callback notify_cb;
    app_read_characteristic_callback read_characteristic_cb;
    app_write_characteristic_callback write_characteristic_cb;
    app_read_descriptor_callback read_descriptor_cb;
    app_write_descriptor_callback write_descriptor_cb;
    app_research_service_callback research_service_cb;
} app_callback_struct;


/*Function defination*/

void gattc_init(gattc_conn_func_t *conn);

/*every user reigister in gattsrv*/
void  *gattc_register(gattc_register_req_struct *req, app_callback_struct *cb);

/*every user dereigister from  gattsrv*/
void gattc_deregister(gattc_user_context_t *user);

/*get primary service from gattc srv*/
int32_t gattc_search_service(gattc_search_service_req_t *req);

/*get include service from gattc srv*/
int32_t gattc_get_incl_service(gattc_search_include_service_req_t *req, gatt_srv_uuid_t *incl_uuid);

/*get characteristic of service from gattc srv*/
int32_t gattc_get_char(gattc_get_char_req_t *req, gatt_get_char_result_t *char_info);

/*get descriptor of characteristic from gattc srv*/
int32_t gattc_get_descr(gattc_get_descr_req_t *req, gatt_uuid_t *descr_info);

/*read charateristic's value from remote server*/
int32_t gattc_read_char(gattc_read_char_req_t *req);

/*read descriptor's value from remote server*/
int32_t gattc_read_descr(gattc_read_descr_req_t *req);

/*write  value to the charateristic of  remote server*/
int32_t gattc_write_char(gattc_write_char_req_t *req);

/*write  value to the descriptor of  remote server*/
int32_t gattc_write_descr(gattc_write_descr_req_t *req);

/*configure mtu size */
int32_t gattc_configure_mtu(gattc_cfg_mtu_t *req);

/*give  rsp to remote server when recevied indication from remote server*/
int32_t gattc_hv_confirm(gattc_hv_confirm_t *req);

/*notify scan result to user*/
void gattc_notify_scan_info_to_all_user(ble_gap_advertising_ind_t *param);

/*notify connect result to user*/
void gattc_notify_conn_info_to_all_user(gattc_conn_t *gattc_conn, ble_status_t error_code, uint16_t type);

uint16_t gattc_research_service(gattc_research_service_req_t *req);

#endif


