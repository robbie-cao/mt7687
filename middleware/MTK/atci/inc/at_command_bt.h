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

#ifndef AT_COMMAND_BT_H
#define AT_COMMAND_BT_H

//System head file
//#include "FreeRTOS.h"
//#include "task.h"
#include <string.h>
#include <stdio.h>

//#ifdef MTK_BT_AT_COMMAND_ENABLE

#ifdef __cplusplus
extern "C"
{
#endif


/*---  Define ---*/
#define ATCI_BT_ATA_INQUIRY_NUM 1
#define ATCI_BT_ATA_INQUIRY_TIME 10

typedef struct {
    int message_id;
    int param1;
    uint8_t param2[256];
} at_command_bt_message_struct_t;

typedef enum {
    ATCI_BT_COMMAND_HCI_EVENT = 0,
    ATCI_BT_COMMAND_END
} acti_bt_command_message_id_enum;

typedef enum {
    ATCI_BT_EVENT_INQUIRY_COMPLETE               = 0x01,
    ATCI_BT_EVENT_CONNECT_COMPLETE               = 0x03,
    ATCI_BT_EVENT_DISCONNECT_COMPLETE            = 0x05,
    ATCI_BT_EVENT_AUTH_COMPLETE                  = 0x06,
    ATCI_BT_EVENT_REMOTE_NAME_REQ_COMPLETE       = 0x07,
    ATCI_BT_EVENT_CHNG_CONN_LINK_KEY_COMPLETE    = 0x09,
    ATCI_BT_EVENT_MASTER_LINK_KEY_COMPLETE       = 0x0A,
    ATCI_BT_EVENT_READ_REMOTE_FEATURES_COMPLETE  = 0x0B,
    ATCI_BT_EVENT_READ_REMOTE_VERSION_COMPLETE   = 0x0C,
    ATCI_BT_EVENT_QOS_SETUP_COMPLETE             = 0x0D,
    ATCI_BT_EVENT_COMMAND_COMPLETE               = 0x0E,
    ATCI_BT_EVENT_READ_CLOCK_OFFSET_COMPLETE     = 0x1C,
    ATCI_BT_EVENT_FLOW_SPECIFICATION_COMPLETE    = 0x21, /* 1.2 */
    ATCI_BT_EVENT_READ_REMOTE_EXT_FEAT_COMPLETE  = 0x23, /* 1.2 */
    ATCI_BT_EVENT_SYNC_CONNECT_COMPLETE          = 0x2C, /* 1.2 */
    ATCI_BT_EVENT_SIMPLE_PAIRING_COMPLETE        = 0x36, /* 2.1 */
    ATCI_BT_EVENT_ENHANCED_FLUSH_COMPLETE        = 0x39, /* 2.1 */
} acti_bt_event_msg_id_enum;

/*Context Define */
typedef struct {
    bool ata;
    bool is_inquiry_completed;
    uint8_t searched_num;
    bool is_register_rx_callback;
    bool is_power;
    bool is_sent_cmd;
} atci_bt_context_struct_t;

typedef struct {
    uint16_t op_code;
    uint8_t cmd_len;
    uint8_t cmd[256];
} atci_bt_hci_cmd;


typedef struct {
    uint8_t event;
    uint8_t status;
    uint16_t handle;
    uint8_t parmslen;
    uint8_t parms[256];
} atci_bt_hci_event;

#ifdef __cplusplus
}
#endif


//#endif /*MTK_BT_AT_COMMAND_ENABLE*/

#endif
