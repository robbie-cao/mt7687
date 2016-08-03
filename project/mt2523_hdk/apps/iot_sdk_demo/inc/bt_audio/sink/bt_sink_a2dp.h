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

#ifndef __BT_SINK_A2DP_H
#define __BT_SINK_A2DP_H

#include "bt_sink_conmgr.h"
#include "bt_sink_ami.h"
#include "bt_a2dp.h"
#include "bt_sink_event.h"


#define BT_SINK_INVALID_AID                 (-1)
#define BT_SINK_A2DP_DATA_THRESHHOLD        (6 * 1024)

#define BT_SINK_A2DP_MAGIC_CODE             (0xba)
#define BT_SINK_MEDIA_PKT_HEADER_LEN        (12)
#define BT_SINK_MEDIA_SBC_SYNC_WORD         (0x3453)
#define BT_SINK_MEDIA_SBC_SYNC_WORD_LEN     (4)


typedef enum {
    BT_SINK_A2DP_EVENT_CONNECT,
    BT_SINK_A2DP_EVENT_DISCONNECT,
    BT_SINK_A2DP_EVENT_PLAY,
    BT_SINK_A2DP_EVENT_SUSPEND,
    BT_SINK_A2DP_EVENT_STREAMING,

    /* add before this event */
    BT_SINK_A2DP_EVENT_ALL
} bt_sink_a2dp_event_type_t;

typedef struct {
    bt_sink_a2dp_event_type_t event;
    void *param;
} bt_sink_a2dp_event_t;

typedef struct {
    int32_t ret;
    bool ind;
    uint16_t conn_id;
    bt_address_t dev_addr;
} bt_sink_a2dp_connect_t;

typedef struct {
    int32_t ret;
    bool ind;
    uint16_t conn_id;
    bt_address_t dev_addr;
} bt_sink_a2dp_disconnect_t;

typedef struct {
    uint16_t conn_id;
} bt_sink_a2dp_play_t;

typedef struct {
    uint16_t conn_id;
} bt_sink_a2dp_suspend_t;

typedef struct {
    uint16_t conn_id;
} bt_sink_a2dp_streaming_t;

typedef struct {
    uint8_t version; /* RTP Version */

    uint8_t padding; /*
                 * If the padding bit is set, the packet contains
                 * one or more additional padding octets at the end,
                 * which are not parts of the payload.  The last
                 * octet of the padding contains a count of how many
                 * padding octets should be ignored.
                 */
    uint8_t extension;
    uint8_t marker;  /*
                 * Profile dependent.  Used to mark significant events
                 * such as frame boundaries in the packet stream.
                 */

    uint16_t payload_type; /* Profile dependent.  Identifies the RTP payload type. */

    uint16_t sequence_number; /* Incremented by one for each packet sent */

    uint32_t timestamp;  /* Time stamp of the sample */

    uint32_t ssrc;   /* Synchronization source */

    uint8_t csrc_count;       /*
                         * The number of CSRC (Contributing Source) identifiers
                         * that follow the fixed header.
                         */
    uint32_t csrc_list[15];   /* List of CSRC identifiers */

} bt_sink_a2dp_media_header_t;

typedef struct {
    bt_sink_profile_t profile;
    bt_a2dp_codec_capability_t codec;
    bt_sink_am_media_handle_t med_hd;
    bt_sink_a2dp_media_header_t header;
} SinkProfileA2dp;


void bt_sink_a2dp_ami_hdr(bt_sink_am_id_t aud_id, bt_sink_am_cb_msg_class_t msg_id, bt_sink_am_cb_sub_msg_t sub_msg, void *user);

bt_sink_status_t bt_sink_a2dp_event_handler(bt_sink_event_id_t event, void *param);

bt_status_t bt_sink_a2dp_get_init_params(bt_a2dp_init_params_t *params);

void bt_sink_a2dp_common_callback(bt_event_t event_id, const void *param);

#endif
