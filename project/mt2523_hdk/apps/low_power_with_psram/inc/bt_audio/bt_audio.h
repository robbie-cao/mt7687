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

#ifndef __BT_AUDIO_H__
#define __BT_AUDIO_H__

#if defined(_MSC_VER)
#else
#include "stdio.h"
#endif

#include "stdbool.h"
#include "graphic_interface.h"
#include "main_screen.h"
#include "bt_a2dp.h"
#include "bt_avrcp.h"
#include "bt_hfp.h"
#include "bt_sink_resource.h"
#include <syslog.h>

#define BT_AUDIO_ITEM_H             (25 + 10)
#define BT_AUDIO_ITEM_W             (120)

#define BT_AUDIO_TITLE_X            (0)
#define BT_AUDIO_TITLE_Y            (0)

#define BT_AUDIO_CONNECT_X          (70)
#define BT_AUDIO_CONNECT_Y          (100)

#define BT_AUDIO_HOME_X             (200)
#define BT_AUDIO_HOME_Y             (220)

#define BT_AUDIO_MUSIC_X            (65)
#define BT_AUDIO_MUSIC_Y            (70 - 10)

#define BT_AUDIO_CALL_X             (65)
#define BT_AUDIO_CALL_Y             (50)

#define BT_AUDIO_CALL_GAP           (25)

#define BT_AUDIO_CALL_DROP_X        (70)
#define BT_AUDIO_CALL_DROP_Y        (100)

#define BT_AUDIO_PXP_STATUS_X       (0)
#define BT_AUDIO_PXP_STATUS_Y       (30)

#define BT_AUDIO_MAX_NUM_LEN        (45)

#define STR_BT_AUDIO_TITLE              (L"BT Audio")
#define STR_BT_AUDIO_TITLE_             ( "BT Audio")

#define STR_BT_AUDIO_HOME               (L"Exit")
#define STR_BT_AUDIO_HOME_              ( "Exit")

#define STR_BT_AUDIO_CONN               (L"Connecting")
#define STR_BT_AUDIO_CONN_              ( "Connecting")

#define STR_BT_AUDIO_RECONN               (L"1.  Reconnect")
#define STR_BT_AUDIO_RECONN_              ( "1.  Reconnect")

#define STR_BT_AUDIO_STOP               (L"1.  Stop music")
#define STR_BT_AUDIO_STOP_              ( "1.  Stop music")

#define STR_BT_AUDIO_PLAY               (L"1.  Play music")
#define STR_BT_AUDIO_PLAY_              ( "1.  Play music")

#define STR_BT_AUDIO_NEXT               (L"2.  Next")
#define STR_BT_AUDIO_NEXT_              ( "2.  Next")

#define STR_BT_AUDIO_PRE                (L"3.  Previous")
#define STR_BT_AUDIO_PRE_               ( "3.  Previous")

#define STR_BT_AUDIO_INCOMING           (L"Incoming call")
#define STR_BT_AUDIO_INCOMING_          ( "Incoming call")

#define STR_BT_AUDIO_ANSWER             (L"1.  Answer")
#define STR_BT_AUDIO_ANSWER_            ( "1.  Answer")

#define STR_BT_AUDIO_REJECT             (L"2.  Reject")
#define STR_BT_AUDIO_REJECT_            ( "2.  Reject")

#define STR_BT_AUDIO_DROP               (L"1.  Hang up")
#define STR_BT_AUDIO_DROP_              ( "1.  Hang up")

#define STR_BT_AUDIO_SWITCH_PATH        (L"2.  Switch audio")
#define STR_BT_AUDIO_SWITCH_PATH_       ( "2.  Switch audio")

#define STR_BT_AUDIO_PXP_IDLE_             ("BLE PXP [Unconnected]")
#define STR_BT_AUDIO_PXP_CONNECTED_        ("BLE PXP [Connected]")
#define STR_BT_AUDIO_PXP_ALERT_            ("BLE PXP [Out of range]")

typedef enum {
    BT_AUDIO_SCR_IDLE,
    BT_AUDIO_SCR_PLAYING,
    BT_AUDIO_SCR_STOP,
    BT_AUDIO_SCR_INCOMING,
    BT_AUDIO_SCR_CALLING,

    BT_AUDIO_SCR_TOAL
} bt_audio_screen_t;

typedef struct {
    uint8_t num[BT_AUDIO_MAX_NUM_LEN];
    uint8_t len;
} bt_audio_call_num_t;

typedef struct {
    int16_t x;
    int16_t y;
} bt_audio_point_t;

typedef struct {
    gdi_color bg_color;
    gdi_color font_color;
    bt_audio_screen_t scr;
    uint32_t flag;
    bt_sink_event_id_t pxp_status;
    bt_audio_screen_t bg_screen;
} bt_audio_context_t;


#define BT_SINK_MAX_DEV                    (2)
#define BT_SINK_A2DP_CONN_BIT              (1 << 0)
#define BT_SINK_AVRCP_CONN_BIT             (1 << 1)
#define BT_SINK_HFP_CONN_BIT               (1 << 2)

#define BT_SINK_FLAG_PLAYING               (1 << 0)
#define BT_SINK_FLAG_MT_CALL               (1 << 1)
#define BT_SINK_FLAG_CALLER                (1 << 2)

#define BT_SINK_INVALID_CID                (0xffff)

typedef enum {
    A2DP_IDLE,
    A2DP_READY,
    A2DP_PLAY,

    A2DP_TOTAL
} bt_sink_a2dp_state_t;

typedef enum {
    BT_SINK_DEV_CID,
    BT_SINK_DEV_ADDR,
    BT_SINK_DEV_UNUSED,
    BT_SINK_DEV_STOP,

    BT_SINK_DEV_TOTAL
} bt_sink_dev_type_t;

typedef struct {
    uint32_t flag;
    bt_sink_a2dp_state_t state;
    uint16_t conn_id;
    uint16_t conn_mask;             /* b0: a2dp, b1: avrcp */
    bt_address_t dev_addr;
    bt_audio_call_num_t caller_number;
} bt_sink_dev_t;

typedef struct {
    uint32_t flag;                  /* b0: 1st dev connect, b1: 2nd dev connect */
    uint16_t cid;
    bt_sink_dev_t dev[BT_SINK_MAX_DEV];
} bt_sink_context_t;

bt_sink_context_t *bt_audio_sink_get_context(void);

void show_bt_audio_screen(void);

void bt_audio_event_handler(message_id_enum event_id, int32_t param1, void *param2);

bool bt_audio_is_point_range(bt_audio_point_t *point, bt_audio_point_t *l_corner, bt_audio_point_t *r_corner);

#endif

