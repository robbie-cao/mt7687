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

#if defined(_MSC_VER)
#else
#include "stdio.h"
#endif

#include "graphic_interface.h"
#include "main_screen.h"
#include "bt_audio.h"
#include "bt_sink_conmgr.h"
#include "bt_sink_a2dp.h"
#include "bt_sink_avrcp.h"
#include "bt_sink_hf.h"
#include "syslog.h"
#include "bt_common_dispatch.h"
#include <string.h>


bt_audio_context_t g_bt_audio_cntx;

static bt_sink_context_t g_sink_cntx;

static bt_address_t reconnect_addr;

static bool g_sink_atci_switch;

log_create_module(bt_audio, PRINT_LEVEL_INFO);

static bt_sink_dev_t *bt_audio_set_profile_status(uint16_t conn_id, uint16_t profile, bool connected);

static bt_sink_dev_t *bt_sink_get_dev(bt_sink_dev_type_t type, uint16_t param);

bool hci_log_enabled(void)
{
    return g_sink_atci_switch;
}

uint8_t *bt_audio_convert_string_to_wstring(char *string)
{
    static uint8_t wstring[128];
    int32_t index = 0;
    if (!string) {
        return NULL;
    }

    while (*string) {
        wstring[index] = *string;
        wstring[index + 1] = 0;
        string++;
        index += 2;
    }
    return wstring;
}


static void bt_audio_pen_event_handler(touch_event_struct_t *pen_event, void *user_data)
{
    bt_audio_point_t point, l_corner, r_corner;
    int32_t touch = -1;

    LOG_I(bt_audio, "[bt_audio]pen_hdr(s)--type: %d, x: %d, y: %d, touch: %d\n",
          pen_event->type, pen_event->position.x, pen_event->position.y, touch);

    if (g_bt_audio_cntx.flag == 1) {
        if (pen_event->type == TOUCH_EVENT_UP) {
            point.x = pen_event->position.x;
            point.y = pen_event->position.y;
            switch (g_bt_audio_cntx.scr) {
                case BT_AUDIO_SCR_STOP: {
                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle stop touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_PAUSE, NULL, NULL);
                        bt_sink_event_send(BT_SINK_EVENT_AVRCP_PAUSE, NULL);
                        touch = 1;
                        break;
                    }

                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle next touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_NEXT_TRACK, NULL, NULL);
                        bt_sink_event_send(BT_SINK_EVENT_AVRCP_NEXT_TRACK, NULL);
                        touch = 2;
                        break;
                    }

                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 3;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle pre touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_PRE_TRACK, NULL, NULL);
                        bt_sink_event_send(BT_SINK_EVENT_AVRCP_PRE_TRACK, NULL);
                        touch = 3;
                        break;
                    }

                    break;
                }

                case BT_AUDIO_SCR_PLAYING: {
                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle play touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_PLAY, NULL, NULL);
                        bt_sink_event_send(BT_SINK_EVENT_AVRCP_PLAY, NULL);
                        touch = 4;
                        break;
                    }

                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle next touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_NEXT_TRACK, NULL, NULL);
                        bt_sink_event_send(BT_SINK_EVENT_AVRCP_NEXT_TRACK, NULL);
                        touch = 5;
                        break;
                    }

                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 3;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle pre touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_PRE_TRACK, NULL, NULL);
                        bt_sink_event_send(BT_SINK_EVENT_AVRCP_PRE_TRACK, NULL);
                        touch = 6;
                        break;
                    }

                    break;
                }

                case BT_AUDIO_SCR_INCOMING: {
                    l_corner.x = BT_AUDIO_CALL_X;
                    l_corner.y = BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 2;
                    r_corner.x = BT_AUDIO_CALL_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 3;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle answer touch */
                        //bt_sink_event_post(BT_SINK_EVENT_HF_ANSWER, NULL, NULL);
                        bt_sink_event_send(BT_SINK_EVENT_HF_ANSWER, NULL);
                        touch = 7;
                        break;
                    }

                    l_corner.x = BT_AUDIO_CALL_X;
                    l_corner.y = BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 3;
                    r_corner.x = BT_AUDIO_CALL_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 4;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle reject touch */
                        //bt_sink_event_post(BT_SINK_EVENT_HF_REJECT, NULL, NULL);
                        bt_sink_event_send(BT_SINK_EVENT_HF_REJECT, NULL);
                        touch = 8;
                        break;
                    }

                    break;
                }

                case BT_AUDIO_SCR_CALLING: {
                    l_corner.x = BT_AUDIO_CALL_DROP_X;
                    l_corner.y = BT_AUDIO_CALL_DROP_Y;
                    r_corner.x = BT_AUDIO_CALL_DROP_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle hang up touch */
                        //bt_sink_event_post(BT_SINK_EVENT_HF_HANG_UP, NULL, NULL);
                        bt_sink_event_send(BT_SINK_EVENT_HF_HANG_UP, NULL);
                        touch = 9;
                        break;
                    }

                    l_corner.x = BT_AUDIO_CALL_DROP_X;
                    l_corner.y = BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H;
                    r_corner.x = BT_AUDIO_CALL_DROP_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H * 2;

                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle hang up touch */
                        bt_sink_event_send(BT_SINK_EVENT_HF_SWITCH_AUDIO_PATH, NULL);
                        touch = 10;
                        break;
                    }

                    break;
                }

                case BT_AUDIO_SCR_IDLE: {
                    l_corner.x = BT_AUDIO_CONNECT_X;
                    l_corner.y = BT_AUDIO_CONNECT_Y + BT_AUDIO_ITEM_H;
                    r_corner.x = BT_AUDIO_CALL_DROP_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H * 2;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        bt_address_t empty_addr;
                        memset(&empty_addr, 0, sizeof(bt_address_t));
                        if (0 != memcmp(&empty_addr, &reconnect_addr, sizeof(bt_address_t))) {
                            bt_sink_event_send(BT_SINK_EVENT_CM_CONNECT, &reconnect_addr);
                            touch = 11;
                        }
                    }
                    break;
                }

                default:
                    break;
            }

            l_corner.x = BT_AUDIO_HOME_X;
            l_corner.y = BT_AUDIO_HOME_Y;
            r_corner.x = 240;
            r_corner.y = 240;
            if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                /* handle exit touch */
                bt_common_callback_set_demo(BT_IDLE);
                //bt_sink_event_post(BT_SINK_EVENT_CM_POWER_SWITCH, NULL, NULL);
                bt_sink_event_send(BT_SINK_EVENT_CM_POWER_SWITCH, NULL);
                g_bt_audio_cntx.flag = 0;
                g_sink_atci_switch = false;
                show_main_screen();
                touch = 10;
            }
        }
    }
    LOG_I(bt_audio, "[bt_audio]pen_hdr(e)--type: %d, x: %d, y: %d, touch: %d, flag: %d\n",
          pen_event->type, pen_event->position.x, pen_event->position.y, touch, g_bt_audio_cntx.flag);
}


static void bt_audio_show_th_str()
{
    int32_t width = 0, height = 0;

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_TITLE,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_TITLE_),
        strlen(STR_BT_AUDIO_TITLE_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_TITLE_X,
                            BT_AUDIO_TITLE_Y,
                            //STR_BT_AUDIO_TITLE,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_TITLE_),
                            strlen(STR_BT_AUDIO_TITLE_),
                            g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_HOME,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_HOME_),
        strlen(STR_BT_AUDIO_HOME_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_HOME_X,
                            BT_AUDIO_HOME_Y,
                            //STR_BT_AUDIO_HOME,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_HOME_),
                            strlen(STR_BT_AUDIO_HOME_),
                            g_bt_audio_cntx.font_color);
}


static void bt_audio_show_idle_screen()
{
    int32_t width = 0, height = 0;


    g_bt_audio_cntx.scr = BT_AUDIO_SCR_IDLE;
    gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_CONN,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_CONN_),
        strlen(STR_BT_AUDIO_CONN_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_CONNECT_X,
                            BT_AUDIO_CONNECT_Y,
                            //STR_BT_AUDIO_CONN,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_CONN_),
                            strlen(STR_BT_AUDIO_CONN_),
                            g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_RECONN_),
        strlen(STR_BT_AUDIO_RECONN_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_CONNECT_X,
                            BT_AUDIO_CONNECT_Y + BT_AUDIO_ITEM_H,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_RECONN_),
                            strlen(STR_BT_AUDIO_RECONN_),
                            g_bt_audio_cntx.font_color);


    bt_audio_show_th_str();

    gdi_flush_screen();
}


static void bt_audio_show_play_screen()
{
    int32_t width = 0, height = 0;

    g_bt_audio_cntx.scr = BT_AUDIO_SCR_PLAYING;
    gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_PLAY,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PLAY_),
        strlen(STR_BT_AUDIO_PLAY_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_MUSIC_X,
                            BT_AUDIO_MUSIC_Y,
                            //STR_BT_AUDIO_PLAY,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PLAY_),
                            strlen(STR_BT_AUDIO_PLAY_),
                            g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_NEXT,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_NEXT_),
        strlen(STR_BT_AUDIO_NEXT_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_MUSIC_X,
                            BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H,
                            //STR_BT_AUDIO_NEXT,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_NEXT_),
                            strlen(STR_BT_AUDIO_NEXT_),
                            g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_PRE,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PRE_),
        strlen(STR_BT_AUDIO_PRE_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_MUSIC_X,
                            BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2,
                            //STR_BT_AUDIO_PRE,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PRE_),
                            strlen(STR_BT_AUDIO_PRE_),
                            g_bt_audio_cntx.font_color);

    bt_audio_show_th_str();

    gdi_flush_screen();
}


static void bt_audio_show_stop_screen()
{
    int32_t width = 0, height = 0;

    g_bt_audio_cntx.scr = BT_AUDIO_SCR_STOP;
    gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_STOP,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_STOP_),
        strlen(STR_BT_AUDIO_STOP_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_MUSIC_X,
                            BT_AUDIO_MUSIC_Y,
                            //STR_BT_AUDIO_STOP,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_STOP_),
                            strlen(STR_BT_AUDIO_STOP_),
                            g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_NEXT,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_NEXT_),
        strlen(STR_BT_AUDIO_NEXT_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_MUSIC_X,
                            BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H,
                            //STR_BT_AUDIO_NEXT,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_NEXT_),
                            strlen(STR_BT_AUDIO_NEXT_),
                            g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_PRE,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PRE_),
        strlen(STR_BT_AUDIO_PRE_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_MUSIC_X,
                            BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2,
                            //STR_BT_AUDIO_PRE,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PRE_),
                            strlen(STR_BT_AUDIO_PRE_),
                            g_bt_audio_cntx.font_color);

    bt_audio_show_th_str();

    gdi_flush_screen();
}


static void bt_audio_show_incoming_screen(bt_audio_call_num_t *num)
{
    int32_t width = 0, height = 0;

    g_bt_audio_cntx.scr = BT_AUDIO_SCR_INCOMING;
    gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_INCOMING,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_INCOMING_),
        strlen(STR_BT_AUDIO_INCOMING_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_CALL_X,
                            BT_AUDIO_CALL_Y,
                            //STR_BT_AUDIO_INCOMING,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_INCOMING_),
                            strlen(STR_BT_AUDIO_INCOMING_),
                            g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        num->num,
        num->len,
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_CALL_X + BT_AUDIO_CALL_GAP,
                            BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H,
                            bt_audio_convert_string_to_wstring((char *)num->num),
                            num->len,
                            g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_ANSWER,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_ANSWER_),
        strlen(STR_BT_AUDIO_ANSWER_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_CALL_X,
                            BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 2,
                            //STR_BT_AUDIO_ANSWER,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_ANSWER_),
                            strlen(STR_BT_AUDIO_ANSWER_),
                            g_bt_audio_cntx.font_color);


    font_engine_get_string_width_height(
        //STR_BT_AUDIO_REJECT,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_REJECT_),
        strlen(STR_BT_AUDIO_REJECT_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_CALL_X,
                            BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 3,
                            //STR_BT_AUDIO_REJECT,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_REJECT_),
                            strlen(STR_BT_AUDIO_REJECT_),
                            g_bt_audio_cntx.font_color);

    bt_audio_show_th_str();

    gdi_flush_screen();
}


static void bt_audio_show_calling_screen()
{
    int32_t width = 0, height = 0;

    g_bt_audio_cntx.scr = BT_AUDIO_SCR_CALLING;
    gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        //STR_BT_AUDIO_DROP,
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_DROP_),
        strlen(STR_BT_AUDIO_DROP_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_CALL_DROP_X,
                            BT_AUDIO_CALL_DROP_Y,
                            //STR_BT_AUDIO_DROP,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_DROP_),
                            strlen(STR_BT_AUDIO_DROP_),
                            g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_SWITCH_PATH_),
        strlen(STR_BT_AUDIO_SWITCH_PATH_),
        &width,
        &height);

    font_engine_show_string(BT_AUDIO_CALL_DROP_X,
                            BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H,
                            bt_audio_convert_string_to_wstring(STR_BT_AUDIO_SWITCH_PATH_),
                            strlen(STR_BT_AUDIO_SWITCH_PATH_),
                            g_bt_audio_cntx.font_color);


    bt_audio_show_th_str();

    gdi_flush_screen();
}


static void bt_audio_show_screen(bt_audio_screen_t scr, void *param)
{
    switch (scr) {
        case BT_AUDIO_SCR_IDLE:
            //bt_sink_event_post(BT_SINK_EVENT_CM_DISCOVERABLE, NULL, NULL);
            bt_audio_show_idle_screen();
            break;

        case BT_AUDIO_SCR_PLAYING:
            bt_audio_show_play_screen();
            break;

        case BT_AUDIO_SCR_STOP:
            bt_audio_show_stop_screen();
            break;

        case BT_AUDIO_SCR_INCOMING:
            bt_audio_show_incoming_screen((bt_audio_call_num_t *)param);
            break;

        case BT_AUDIO_SCR_CALLING:
            bt_audio_show_calling_screen();
            break;

        default:
            break;
    }
}


bool bt_audio_is_point_range(bt_audio_point_t *point, bt_audio_point_t *l_corner, bt_audio_point_t *r_corner)
{
    bool ret = false;

    if (point->x >= l_corner->x && point->x <= r_corner->x &&
            point->y >= l_corner->y && point->y <= r_corner->y) {
        ret = true;
    }

    return ret;
}


void bt_audio_event_handler(message_id_enum event_id, int32_t param1, void *param2)
{
    LOG_I(bt_audio, "[bt_audio]event_handler--bt: %d, eid: %d, p1: %d, p2: 0x%x\n",
          MESSAGE_ID_BT_AUDIO, event_id, param1, param2);

    if (event_id == MESSAGE_ID_BT_AUDIO) {
        bt_audio_show_screen((bt_audio_screen_t)param1, param2);
    }
}


bt_sink_status_t bt_audio_sink_event_hdr(bt_sink_event_id_t event_id, void *parameters)
{
    bt_sink_a2dp_event_t *a2dp_event = NULL;
    bt_sink_avrcp_event_t *avrcp_event = NULL;
    bt_sink_a2dp_connect_t *a2dp_connect = NULL;
    bt_sink_a2dp_disconnect_t *a2dp_disconnect = NULL;
    bt_sink_a2dp_play_t *a2dp_play = NULL;
    bt_sink_a2dp_suspend_t *a2dp_suspend = NULL;
    bt_sink_a2dp_streaming_t *a2dp_streaming = NULL;
    bt_sink_avrcp_connect_t *avrcp_connect = NULL;
    bt_sink_avrcp_disconnect_t *avrcp_disconnect = NULL;
    bt_sink_dev_t *dev = NULL;

    LOG_I(bt_audio, "[bt_audio]event_hdr--id: 0x%x\r\n", event_id);

    switch (event_id) {
        case BT_SINK_EVENT_HF_CONNECT_STATE_CHANGE: {
            bt_sink_hf_connect_state_change_t *connect_state = (bt_sink_hf_connect_state_change_t *)parameters;
            bt_audio_set_profile_status(connect_state->conn_id, BT_SINK_HFP_CONN_BIT, connect_state->connected);
            if (connect_state->connected) {
                memcpy(&reconnect_addr, &connect_state->bt_addr, sizeof(bt_address_t));
            }
        }
        break;

        case BT_SINK_EVENT_HF_CALL_STATE_CHANGE: {
            bt_sink_hf_call_state_change_t *call_state = (bt_sink_hf_call_state_change_t *)parameters;

            dev = bt_sink_get_dev(BT_SINK_DEV_CID, call_state->conn_id);

            if (BT_SINK_HF_CALL_STATE_INCOMING == call_state->new_state) {
                if (dev != NULL) {
                    dev->flag |= BT_SINK_FLAG_MT_CALL;
                }
            } else if (BT_SINK_HF_CALL_STATE_ACTIVE == call_state->new_state
                       && BT_SINK_HF_CALL_STATE_INCOMING == call_state->previous_state) {
                if (dev->caller_number.len == 0) {
                    dev->caller_number.len = strlen("Unknown number");
                    strcpy((char *)dev->caller_number.num, "Unknown number");
                }
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_CALLING, (void *)(&dev->caller_number));
            } else if (BT_SINK_HF_CALL_STATE_IDLE == call_state->new_state
                       && BT_SINK_HF_CALL_STATE_IDLE != call_state->previous_state) {
                if (dev != NULL) {
                    dev->flag &= (~BT_SINK_FLAG_MT_CALL);
                    dev->flag &= (~BT_SINK_FLAG_CALLER);
                    dev->caller_number.len = 0;
                    if (A2DP_PLAY == dev->state) {
                        ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_STOP, NULL);
                    } else {
                        ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_PLAYING, NULL);
                    }
                }
            }
        }
        break;

        case BT_SINK_EVENT_HF_CALLER_INFORMATION: {
            bt_sink_caller_information_t *caller_info = (bt_sink_caller_information_t *)parameters;

            dev = bt_sink_get_dev(BT_SINK_DEV_CID, caller_info->conn_id);

            if (dev != NULL && (dev->flag & BT_SINK_FLAG_MT_CALL) && !(dev->flag & BT_SINK_FLAG_CALLER)) {
                dev->flag |= BT_SINK_FLAG_CALLER;
                dev->caller_number.len = (caller_info->num_size > BT_AUDIO_MAX_NUM_LEN) ? BT_AUDIO_MAX_NUM_LEN : caller_info->num_size;
                bt_sink_memcpy(dev->caller_number.num, caller_info->number, dev->caller_number.len);
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_INCOMING, (void *)(&dev->caller_number));
            }
        }
        break;

        case BT_SINK_EVENT_A2DP_NOTIFY: {
            a2dp_event = (bt_sink_a2dp_event_t *)parameters;

            LOG_I(bt_audio, "[bt_audio]event_hdr(a2dp)--id: %d\r\n", a2dp_event->event);

            switch (a2dp_event->event) {
                case BT_SINK_A2DP_EVENT_CONNECT: {
                    a2dp_connect = (bt_sink_a2dp_connect_t *)(a2dp_event->param);
                    if (a2dp_connect->ret == 0) {
                        dev = bt_audio_set_profile_status(a2dp_connect->conn_id, BT_SINK_A2DP_CONN_BIT, true);
                        /* a2dp connected */
                        dev->state = A2DP_READY;
                    }
                    break;
                }

                case BT_SINK_A2DP_EVENT_DISCONNECT: {
                    a2dp_disconnect = (bt_sink_a2dp_disconnect_t *)(a2dp_event->param);
                    bt_audio_set_profile_status(a2dp_disconnect->conn_id, BT_SINK_A2DP_CONN_BIT, false);
                    break;
                }

                case BT_SINK_A2DP_EVENT_PLAY: {
                    a2dp_play = (bt_sink_a2dp_play_t *)(a2dp_event->param);
                    dev = bt_sink_get_dev(BT_SINK_DEV_CID, a2dp_play->conn_id);
                    dev->state = A2DP_PLAY;
                    ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_STOP, NULL);
                    break;
                }

                case BT_SINK_A2DP_EVENT_SUSPEND: {
                    a2dp_suspend = (bt_sink_a2dp_suspend_t *)(a2dp_event->param);
                    dev = bt_sink_get_dev(BT_SINK_DEV_CID, a2dp_suspend->conn_id);
                    dev->state = A2DP_READY;
                    ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_PLAYING, NULL);
                    break;
                }

                case BT_SINK_A2DP_EVENT_STREAMING: {
                    a2dp_streaming = (bt_sink_a2dp_streaming_t *)(a2dp_event->param);
                    dev = bt_sink_get_dev(BT_SINK_DEV_CID, a2dp_streaming->conn_id);
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case BT_SINK_EVENT_AVRCP_NOTIFY: {
            avrcp_event = (bt_sink_avrcp_event_t *)parameters;

            LOG_I(bt_audio, "[bt_audio]event_hdr(avrcp)--id: %d\r\n", avrcp_event->event);

            switch (avrcp_event->event) {
                case BT_SINK_AVRCP_EVENT_CONNECT: {
                    avrcp_connect = (bt_sink_avrcp_connect_t *)(avrcp_event->param);
                    if (avrcp_connect->ret == 0) {
                        bt_audio_set_profile_status(avrcp_connect->conn_id, BT_SINK_AVRCP_CONN_BIT, true);
                    }
                    break;
                }

                case BT_SINK_AVRCP_EVENT_DISCONNECT: {
                    avrcp_disconnect = (bt_sink_avrcp_disconnect_t *)(avrcp_event->param);
                    bt_audio_set_profile_status(avrcp_disconnect->conn_id, BT_SINK_AVRCP_CONN_BIT, false);
                    break;
                }

                default:
                    break;
            }
            break;
        }

        default:
            break;
    }
    return BT_SINK_STATUS_SUCCESS;
}


static void bt_sink_init()
{
    int32_t i = 0;

    memset(&g_sink_cntx, 0x00, sizeof(bt_sink_context_t));

    g_sink_atci_switch = true;

    for (i = 0; i < BT_SINK_MAX_DEV; ++i) {
        g_sink_cntx.dev[i].conn_id = BT_SINK_INVALID_CID;
        //g_sink_cntx.run = 0;
    }
}


static bt_sink_dev_t *bt_sink_get_dev(bt_sink_dev_type_t type, uint16_t param)
{
    bt_sink_dev_t *dev = NULL;
    bt_sink_context_t *cntx = NULL;
    uint16_t cid = 0;
    int32_t i = 0;

    cntx = bt_audio_sink_get_context();

    switch (type) {
        case BT_SINK_DEV_CID: {
            cid = param;

            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->dev[i].conn_id != BT_SINK_INVALID_CID
                        && cntx->dev[i].conn_id == cid) {
                    dev = &(cntx->dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_DEV_UNUSED: {
            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->dev[i].conn_id == BT_SINK_INVALID_CID) {
                    dev = &(cntx->dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_DEV_STOP: {
            for (i = 0; i < BT_SINK_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->dev[i].state == A2DP_READY) {
                    dev = &(cntx->dev[i]);
                    break;
                }
            }

            break;
        }

        default:
            break;
    }

    return dev;
}


static void bt_sink_reset_dev(bt_sink_dev_t *dev)
{
    dev->conn_id = BT_SINK_INVALID_CID;
    dev->conn_mask = 0x00;
}

bt_sink_dev_t *bt_audio_set_profile_status(uint16_t conn_id, uint16_t profile, bool connected)
{
    bt_sink_dev_t *dev = bt_sink_get_dev(BT_SINK_DEV_CID, conn_id);
    uint16_t mask = 0;

    if (!dev) {
        dev = bt_sink_get_dev(BT_SINK_DEV_UNUSED, 0);
        dev->conn_id = conn_id;
    }

    LOG_I(bt_audio, "dev:0x%x, cid: %d, profile:0x%x, connected:%d", dev, conn_id, profile, connected);

    if (profile >= BT_SINK_A2DP_CONN_BIT && profile <= BT_SINK_HFP_CONN_BIT) {
        if (connected) {
            dev->conn_mask |= profile;
            mask = (BT_SINK_A2DP_CONN_BIT | BT_SINK_AVRCP_CONN_BIT | BT_SINK_HFP_CONN_BIT);
            if ((dev->conn_mask & mask) == mask) {
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_PLAYING, NULL);
            }
        } else {
            dev->conn_mask &= (~profile);
            if (0 == dev->conn_mask) {
                bt_sink_reset_dev(dev);
                dev = NULL;
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_IDLE, NULL);
            }
        }
    }
    return dev;
}

bt_sink_context_t *bt_audio_sink_get_context(void)
{
    return &g_sink_cntx;
}


void show_bt_audio_screen(void)
{
    g_bt_audio_cntx.bg_color = 0;
    g_bt_audio_cntx.font_color = 0xFFFF;
    LOG_I(bt_audio, "[bt_audio]enter\n");

    g_bt_audio_cntx.flag = 1;

    bt_sink_init();

    bt_common_callback_set_demo(BT_AUDIO);

    demo_ui_register_touch_event_callback(bt_audio_pen_event_handler, NULL);
    bt_sink_event_register_callback(BT_SINK_EVENT_ALL, bt_audio_sink_event_hdr);

    bt_sink_event_post(BT_SINK_EVENT_CM_POWER_SWITCH, NULL, NULL);

    ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_IDLE, NULL);
}

