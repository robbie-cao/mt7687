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

/*****************************************************************************
 *
 * Description:
 * ------------
 * The header file for users.
 *
 ****************************************************************************/



#ifndef __BT_NOTIFY_H__
#define __BT_NOTIFY_H__
#include "stdbool.h"
#include <stdint.h>
#include "bt_address.h"
#include <string.h>
#include <stdio.h>
#include "bt_spp.h"

#ifndef WIN32_DEBUG
#include <FreeRTOS.h>
#include <timers.h>
#endif

/*All API usages, please reference the eample code in the file(bt_notify_test.c) */
/*the default buffer size is (2*1024+1), if you want change it, must changer longer. */

/*****************************************************************************
 * define
 *****************************************************************************/

#define BT_NOTIFY_RECEIVE_DATA_BUF_SIZE   (2*1024 + 1)  /*common EXCD data buffer(send to app part)*/
#define BT_NOTIFY_PARSER_BUF_SIZE         (3*1024)      /*it is for notification parser buffer. you can modify it, could change longer*/

#define BT_NOTIFY_MAX_CHANNEL             (1)

#define BT_NOTIFY_SENDER_LENTH              50
#define BT_NOTIFY_CONTENT_LENTH             300         /* remote device limitation is 256 */
#define BT_NOTIFY_MAX_ICON_LENGTH           2560        /*icon size*/
#define BT_NOTIFY_TITLE_LENTH               30
#define BT_NOTIFY_NUMBER_LENTH              20

/*actions related*/
#define BT_NOTIFY_ACTION_MAX_NUM            5
#define BT_NOTIFY_ACTION_NAME_LENTH         20
#define BT_NOTIFY_ACTION_ID_LENTH           20


/*group releated*/
#define BT_NOTIFY_GROUP_MAX_LENGTH         30

/*****************************************************************************
 * Typedef
 *****************************************************************************/

typedef void (*bt_noti_callback)(void *data);
/*callback struct */
typedef enum {
    BT_NOTIFY_EVENT_NEW_MSG = 1,
    BT_NOTIFY_EVENT_DATA,
    BT_NOTIFY_EVENT_CONNECTION,
    BT_NOTIFY_EVENT_DISCONNECTION,
    BT_NOTIFY_EVENT_READY_TO_READ,
    BT_NOTIFY_EVENT_READY_TO_WRITE,
    BT_NOTIFY_EVENT_NOTIFICATION,
    BT_NOTIFY_EVENT_MISSED_CALL,
    BT_NOTIFY_EVENT_SMS,
    BT_NOTIFY_EVENT_END

} bt_notify_indication_event_t;
/**
 *   This structure defines BT_NOTIFY_EVENT_DATA.
 */

typedef struct {
    char sender_id[16];                         /* sender id for FP */
    char receiver_id[16];                       /* receiver id for SP */
    int32_t err_code;   /* error code */
    uint8_t  data[BT_NOTIFY_RECEIVE_DATA_BUF_SIZE];
    uint16_t len;
} bt_noti_data_t;

/**
 *  remote device type..
 */

typedef enum {
    BT_NOTIFY_REMOTE_SYSTEM_NONE,
    BT_NOTIFY_REMOTE_SYSTEM_ANDROID,
    BT_NOTIFY_REMOTE_SYSTEM_IOS
} bt_notify_remote_system_t;

/**
 *  remote device type..
 */

typedef enum {
    BT_NOTIFY_NOTIFICATION_NEW,
    BT_NOTIFY_NOTIFICATION_DELETE
} bt_notify_notification_action_t;


#ifdef _ICON_SUPPORT_ /*The icon parse feature, should open the macro, and need decode*/
typedef struct {
    uint8_t icon_buffer[BT_NOTIFY_MAX_ICON_LENGTH]; /*need base64 decode*/
    uint32_t icon_size;
} bt_notify_data_icon_t;
#endif /*_ICON_SUPPORT_*/

typedef struct page_struct {
    uint16_t content[BT_NOTIFY_CONTENT_LENTH];
    uint16_t title[BT_NOTIFY_TITLE_LENTH];
    struct page_struct *next;
} bt_notify_page_content_t;

typedef struct {
    uint16_t action_id[BT_NOTIFY_ACTION_ID_LENTH];
    uint16_t action_name[BT_NOTIFY_ACTION_NAME_LENTH + 3];
} bt_notify_action_t;

typedef struct {
    uint16_t sender[BT_NOTIFY_SENDER_LENTH];
    uint32_t page_num;
    bt_notify_page_content_t *page_content;
    uint32_t action_number;
    bt_notify_action_t noti_action[BT_NOTIFY_ACTION_MAX_NUM];
    uint8_t group_id[BT_NOTIFY_GROUP_MAX_LENGTH];
    uint32_t timestamp;
    uint32_t app_id;
    bt_notify_notification_action_t action;
    uint32_t msg_id;
#ifdef _ICON_SUPPORT_
    bt_notify_data_icon_t *icon;
#endif /*_ICON_SUPPORT_*/

} bt_notify_noti_t;

typedef struct {
    uint16_t sender[BT_NOTIFY_SENDER_LENTH];
    uint16_t send_number[BT_NOTIFY_NUMBER_LENTH];
    bt_notify_page_content_t *page_content;
    uint32_t msg_id;
    uint32_t timestamp;
} bt_notify_sms_t;

typedef struct {
    uint16_t sender[BT_NOTIFY_SENDER_LENTH];
    uint16_t send_number[BT_NOTIFY_NUMBER_LENTH];
    uint32_t msg_id;
    uint32_t missed_call_count;
    uint32_t timestamp;
} bt_notify_call_t;

/**
*if evt_id is BT_NOTIFY_EVENT_DATA,             need read 'bt_noti_data_t noti_data';
*if evt_id is BT_NOTIFY_EVENT_NOTIFICATION, need read 'bt_notify_noti_t notification';
*if evt_id is BT_NOTIFY_EVENT_MISSED_CALL, need read 'bt_notify_sms_t sms';
*if evt_id is BT_NOTIFY_EVENT_SMS,               need read 'bt_notify_call_t missed_call';
*/

typedef struct {
    bt_notify_indication_event_t evt_id;
    bt_notify_remote_system_t remote_system;
    bt_address_t bt_addr;
    bt_noti_data_t noti_data;
    bt_notify_noti_t notification;
    bt_notify_sms_t sms;
    bt_notify_call_t missed_call;
} bt_noti_callback_t;

/**
 *  the ret value..
 */

typedef enum {

    BT_NOTIFY_NO_SUPPORT = -9,
    BT_NOTIFY_REGISTER_RET_INVALID_PARAMETER,
    BT_NOTIFY_REGISTER_RET_SYSTEM_REGISTERED,
    BT_NOTIFY_REGISTER_RET_REPEAT_REGISTER,
    BT_NOTIFY_REGISTER_RET_NOT_IMPLEMENT,
    BT_NOTIFY_RET_INVALID_PARAMETER,
    BT_NOTIFY_RET_BUFFER_NOT_ENOUGH,
    BT_NOTIFY_RET_PARSE_PARA_ERROR,
    BT_NOTIFY_RET_NO_CHANNAL,
    BT_NOTIFY_REGISTER_RET_OK,
    BT_NOTIFY_RET_NO_ERROR = BT_NOTIFY_REGISTER_RET_OK,
} bt_notify_result_t;
typedef enum {
    BT_NOTIFY_DATA_SOURCE_UNKNOWN = 0x00,
    BT_NOTIFY_DATA_SOURCE_SPP = 0x01,
    BT_NOTIFY_DATA_SOURCE_DOGP = 0x02,
} bt_notify_data_source_t;

/*****************************************************************************
 * Function
 *****************************************************************************/

/**
 * This function is for app register callback function in BT Notify.
 * bt_addr       is the BT address of remote device,if no care, set NULL.
 * sender         is the name of app.
 * cb_ptr         is the callback fuction of the app.
 * return         The result of the register success or not.
 *
 */

bt_notify_result_t bt_notify_register_callback(bt_address_t *bt_addr,  char *sender, bt_noti_callback cb_ptr);
/**
 * his function is for app deregister callback function in BT Notify.
 * bt_addr       is the BT address of remote device.
 * cb_ptr         is the callback fuction of the app.
 *                   The result of the deregister success or not.
 *                   n/a.
 */

bool bt_notify_deregister_callback(bt_address_t *bt_addr, bt_noti_callback cb_ptr);
/**
 * This function is for app send data.
 * bt_addr       is the BT address of remote device.
 * data            is the data  the app want to send.
 * data_len      is the data length.
 * is_new        is the flag whether is the new data.
 * return         send data length.
 * note            if the new data is not send all, you need to wait BT_NOTIFY_EVENT_READY_TO_WRITE event callback, then send the rest data.
 *
 */

int32_t bt_notify_send_data(bt_address_t *bt_addr,  const char *data, uint16_t data_len, bool is_new);
/**
 * This function is for app read data.
 * bt_addr       is the BT address of remote device.
 * return                        .
 * note            if receive BT_NOTIFY_EVENT_READY_TO_READ event callback, then need call then api to read data.
 *                    then, when parsing is done, BT notify will callback the data to app,you can wait the event callback BT_NOTIFY_EVENT_DATA.
 *
 */

void bt_notify_read_data(bt_address_t *bt_addr);
/**
 * This function is for BT Notify init.
 * note                          call it in app's init function or other.
 *
 */

void bt_notify_init(void);
/**
 * This function is for getting device addr by channel index.
 * cntx_chnl         is the channel index.
 * return            is the BT address of remote device          .
 * note
 *
 */

bt_address_t *bt_notify_get_bt_device_addr(uint8_t cntx_chnl);
/**
 * This function is for getting app callback.
 * cntx_chnl         is the channel index.
 * return            is the callback structure.          .
 * note
 *
 */

bt_noti_callback_t *bt_notify_get_callback_struct(uint8_t cntx_chnl);
/**
 * This function is for reset the ststus of receiving the data.
 * cntx_chnl         is the channel index.
 * return            is the result of the status.            .
 * note
 *
 */

bt_notify_result_t bt_notify_reset_parse_status(uint8_t cntx_chnl);
/**
 * This function is for callback app.
 * cntx_chnl         is the channel index.
 * return                        .
 * note
 *
 */

void bt_notify_callback_app(uint8_t cntx_chnl);

void bt_notify_handle_connect_ind(void *para, bt_notify_data_source_t source);
void bt_notify_spp_connect_ind(bt_spp_connect_ind_t *parameter);
void bt_notify_handle_disconnect_ind(void *prameter, bt_notify_data_source_t source);
void bt_notify_handle_ready_to_read_ind(void *para, bt_notify_data_source_t source);
void bt_notify_handle_ready_to_write_ind(void *para, bt_notify_data_source_t source);

#endif/*__BT_NOTIFY_H__*/

