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
 * This file implements PXP app main function
 *
 ****************************************************************************/
#include "ble_ias.h"
#include "ble_tps.h"
#include "ble_lls.h"
#include "ble_gatts_srv_common.h"
#include "ble_pxp_main.h"
#include "ble_gatt.h"
#include "ble_gap.h"
#include "ble_message.h"
#include <stdio.h>
#include <string.h>
#include "graphic_interface.h"
#include "main_screen.h"
#include "syslog.h"
#include "bt_common_dispatch.h"
#include "bt_sink_event.h"

#define PXP_LEFT_GAP 40
#define PXP_ITEM_HEIGHT 28

/************************************************
*   Structures
*************************************************/
typedef struct
{
    ble_ias_alert_level_t alert_level;                                         /**< Holds decoded data in Write event handler, not for write long. */
    uint8_t  *alert_level_write_long_data;                                     /**< Holds data only for write long. */
    uint8_t  alert_level_is_prepare;                                           /**< is prepare write(TRUE) or not(FALSE). */ 
    uint8_t  alert_level_is_excute;                                            /**< is execute(TRUE) or cancel write(FALSE). */
    uint16_t alert_level_write_offset;                                         /**< offset for write long. */
    uint16_t alert_level_write_length;                                         /**< the data length of write value. */
    uint16_t *handle_list;                                                     /**< characteristic handle list, for read multiple. */
} ble_gatts_ias_cntx_t;

typedef struct
{
    ble_lls_alert_level_t alert_level;                                         /**< Holds decoded data in Write event handler, not for write long. */
    uint8_t *alert_level_write_long_data;                                      /**< Holds data only for write long. */
    uint8_t alert_level_is_prepare;                                            /**< is prepare(TRUE) or not(FALSE). */
    uint8_t alert_level_is_excute;                                             /**< is execute(TRUE) or cancel write(FALSE). */
    uint16_t alert_level_write_offset;                                         /**< offset for write long. */
    uint16_t alert_level_write_length;                                         /**< the data length of write value. */
    uint16_t alert_level_read_offset;                                          /**< offset for read long. */  
    uint16_t *handle_list;                                                     /**< characteristic handle list, for read multiple. */
} ble_gatts_lls_cntx_t;

typedef struct
{
    uint16_t *handle_list;                                                      /**< characteristic handle list, for read multiple. */
    uint16_t tx_power_level_read_offset;                                        /**< offset for read long. */
} ble_gatts_tps_cntx_t;

typedef struct
{
    uint16_t conn_id;
    ble_gatts_ias_cntx_t ias_cntx;
    ble_gatts_lls_cntx_t lls_cntx;
    ble_gatts_tps_cntx_t tps_cntx;
    ble_gap_conn_param_priority_t current_priority;
} ble_gatts_pxp_cntx_t;

typedef struct {
    int32_t title_x;
	int32_t title_y;
	int32_t content_x;
	int32_t content_y;
	int32_t back_x1;
	int32_t back_y1;
	int32_t back_x2;
	int32_t back_y2;
    gdi_color bg_color;
	gdi_color font_color;
} ble_pxp_screen_cntx;

/************************************************
*   Global variables
*************************************************/
#define PXP_MAX_MULTIPLE_HANDLE_NUMBER 8
#define PXP_MAX_WRITE_DATA_LENGTH 158

ble_evt_t p_ble_evt_t;
ble_ias_t p_ias_t;
ble_lls_t p_lls_t;
ble_tps_t p_tps_t;
ble_gatts_pxp_cntx_t g_pxp_cntx_t[MAX_LE_DEV_NUM];
uint16_t read_handle[PXP_MAX_MULTIPLE_HANDLE_NUMBER];
uint8_t write_data[PXP_MAX_WRITE_DATA_LENGTH];

uint16_t g_conn_id = 0;
bool g_disc_screen_exited = 0;
bool g_conn_screen_exited = 0;
bool g_range_screen_exited = 0;
ble_pxp_screen_cntx g_ble_pxp_screen_cntx;

bool bt_power_on_status = false;
/************************************************
*   Static utilities
*************************************************/
static void ble_pxp_evt_dispatch(ble_evt_t * p_ble_evt);
static void ble_pxp_services_init(void);
static void ble_pxp_on_ble_evt(ble_evt_t *p_ble_evt);
static bool ble_pxp_have_existed_in_cntx(uint16_t conn_id);
static uint16_t ble_pxp_get_cntx_index_by_conn_id(uint16_t conn_id);
static uint16_t ble_pxp_get_null_cntx_index(void);
static uint16_t ble_pxp_get_link_number(void);

log_create_module(PXP_MAIN, PRINT_LEVEL_INFO);

/************************************************
*   Functions
*************************************************/
void ble_pxp_start_advertise(void)
{
    uint8_t buff[31] = {2, 1, 0x1A};
    char* LE_name = "my_hj_dev";  /* you can rename it*/
    uint8_t len = 0;
    /* you can reassign your ble address and data here*/
    ble_address_t addr;
    addr.address[0] = 0x03;
    addr.address[1] = 9;
    addr.address[2] = 8;
    addr.address[3] = 7;
    addr.address[4] = 6;
    addr.address[5] = 0xC3;
    ble_gap_stop_advertiser();
    ble_gap_set_random_address(&addr);
    len = strlen(LE_name);
    buff[3] = len + 1;
    buff[4] = 9;
    memcpy(buff+5, LE_name, len);
    ble_gap_set_advertising_data(
        buff, 
        len + 5, 
        NULL, 
        0
    );
    ble_gap_set_advertising_params(
        0x100, 
        0x100, 
        BLE_GAP_ADVERTISING_CONNECTABLE_UNDIRECTED, 
        BLE_ADDRESS_TYPE_RANDOM, 
        BLE_ADDRESS_TYPE_RANDOM, 
        NULL, 
        0x7, 
        0
    );
    ble_gap_start_advertiser();
    LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_start_advertise\r\n");
}

uint8_t* ble_pxp_convert_string_to_wstring(uint8_t* string)
{
    static uint8_t wstring[50];
    int32_t index = 0;
    if (!string) {
        return NULL;
    }

    while (*string) {
        wstring[index] = *string;
        wstring[index + 1] = 0;
        string++;
        index+=2;
    }
    return wstring;
}

void ble_pxp_pen_event_handler(touch_event_struct_t* pen_event, void* user_data)
{
	static int32_t back_hit;
	if (pen_event->position.x <= g_ble_pxp_screen_cntx.back_x2 && pen_event->position.x >= g_ble_pxp_screen_cntx.back_x1) {
		if (pen_event->position.y <= g_ble_pxp_screen_cntx.back_y2 && pen_event->position.y >= g_ble_pxp_screen_cntx.back_y1) {
			if (pen_event->type == TOUCH_EVENT_DOWN) {
				back_hit = 1;
			} else if (back_hit == 1 && pen_event->type == TOUCH_EVENT_UP) {
				back_hit = 0;
                g_disc_screen_exited = 0;
                g_conn_screen_exited = 0;
                g_range_screen_exited = 0;
                if (0 != g_conn_id) {
                    ble_gap_disconnect(g_conn_id);
                }
                memset(&g_ble_pxp_screen_cntx, 0, sizeof(ble_pxp_screen_cntx));
                
                if (true == bt_power_on_status) {
                    ble_gap_stop_advertiser();
                    bt_gap_power_off();
                }
				show_main_screen();
			}
		}
	} else {
		back_hit = 0;
	}
}

void ble_pxp_screen_event_handler(message_id_enum event_id, int32_t param1, void* param2)
{

}

void ble_pxp_show_main_screen(void)
{
	int32_t string_width, string_height;
    demo_ui_register_touch_event_callback(ble_pxp_pen_event_handler, NULL);
    memset(&g_ble_pxp_screen_cntx, 0, sizeof(ble_pxp_screen_cntx));
    
	g_ble_pxp_screen_cntx.bg_color = 0;
	g_ble_pxp_screen_cntx.font_color = 0xFFFF;
	g_ble_pxp_screen_cntx.title_x = 5;
	g_ble_pxp_screen_cntx.title_y = 6;
	g_ble_pxp_screen_cntx.content_x = PXP_LEFT_GAP;

	g_ble_pxp_screen_cntx.back_x2 = 239;
	g_ble_pxp_screen_cntx.back_y2 = 239;
    /**g_ble_pxp_screen_cntx.back_x1 = 160;*/
	g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
	g_ble_pxp_screen_cntx.content_y = (g_ble_pxp_screen_cntx.back_y2 - g_ble_pxp_screen_cntx.title_y - PXP_ITEM_HEIGHT)/2;


	gdi_draw_solid_rect(0,0,239,239, g_ble_pxp_screen_cntx.bg_color);

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                                strlen("BLE PXP"),
                                &string_width,
                                &string_height);
    font_engine_show_string(g_ble_pxp_screen_cntx.title_x, 
                            g_ble_pxp_screen_cntx.title_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                            strlen("BLE PXP"), 
                            g_ble_pxp_screen_cntx.font_color);

    /**< determine what to display by "disconencted or connected or out of range". */
    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Unconnected"), 
                                strlen("Unconnected"),
                                &string_width,
                                &string_height);
	font_engine_show_string(g_ble_pxp_screen_cntx.content_x, 
                            g_ble_pxp_screen_cntx.content_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Unconnected"), 
                            strlen("Unconnected"), 
                            g_ble_pxp_screen_cntx.font_color);
 
    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                                strlen("Exit"),
                                &string_width,
                                &string_height);
    
	g_ble_pxp_screen_cntx.back_x1 = g_ble_pxp_screen_cntx.back_x2 - string_width;
	font_engine_show_string(g_ble_pxp_screen_cntx.back_x1, 
                            g_ble_pxp_screen_cntx.back_y1, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                            strlen("Exit"), 
                            g_ble_pxp_screen_cntx.font_color);

    g_disc_screen_exited = 1;
	gdi_flush_screen();

    if (true == bt_power_on_status) {
        ble_pxp_start_advertise();
    }
}

void ble_pxp_show_connected_screen()
{
	int32_t string_width, string_height;
    demo_ui_register_touch_event_callback(ble_pxp_pen_event_handler, NULL);
    
    memset(&g_ble_pxp_screen_cntx, 0, sizeof(ble_pxp_screen_cntx));
	g_ble_pxp_screen_cntx.bg_color = 0;
	g_ble_pxp_screen_cntx.font_color = 0xFFFF;
	g_ble_pxp_screen_cntx.title_x = 5;
	g_ble_pxp_screen_cntx.title_y = 6;
	g_ble_pxp_screen_cntx.content_x = PXP_LEFT_GAP;

	g_ble_pxp_screen_cntx.back_x2 = 239;
	g_ble_pxp_screen_cntx.back_y2 = 239;
	g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
	g_ble_pxp_screen_cntx.content_y = (g_ble_pxp_screen_cntx.back_y2 - g_ble_pxp_screen_cntx.title_y - PXP_ITEM_HEIGHT)/2;

	gdi_draw_solid_rect(0,0,239,239, g_ble_pxp_screen_cntx.bg_color);

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                                strlen("BLE PXP"),
                                &string_width,
                                &string_height);
    font_engine_show_string(g_ble_pxp_screen_cntx.title_x, 
                            g_ble_pxp_screen_cntx.title_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                            strlen("BLE PXP"), 
                            g_ble_pxp_screen_cntx.font_color);

    /**determine what to display by "disconencted or connected or out of range"*/
    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Connected"), 
                                strlen("Connected"),
                                &string_width,
                                &string_height);
	font_engine_show_string(g_ble_pxp_screen_cntx.content_x, 
                            g_ble_pxp_screen_cntx.content_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Connected"), 
                            strlen("Connected"), 
                            g_ble_pxp_screen_cntx.font_color);
 

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                                strlen("Exit"),
                                &string_width,
                                &string_height);
    
	g_ble_pxp_screen_cntx.back_x1 = g_ble_pxp_screen_cntx.back_x2 - string_width;
	font_engine_show_string(g_ble_pxp_screen_cntx.back_x1, 
                            g_ble_pxp_screen_cntx.back_y1, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                            strlen("Exit"), 
                            g_ble_pxp_screen_cntx.font_color);
    g_conn_screen_exited = 1;

	gdi_flush_screen();
}

void ble_pxp_show_range_alert_screen()
{
	int32_t string_width, string_height;
    demo_ui_register_touch_event_callback(ble_pxp_pen_event_handler, NULL);
    
    memset(&g_ble_pxp_screen_cntx, 0, sizeof(ble_pxp_screen_cntx));
	g_ble_pxp_screen_cntx.bg_color = 0;
	g_ble_pxp_screen_cntx.font_color = 0xFFFF;
	g_ble_pxp_screen_cntx.title_x = 5;
	g_ble_pxp_screen_cntx.title_y = 6;
	g_ble_pxp_screen_cntx.content_x = PXP_LEFT_GAP;

	g_ble_pxp_screen_cntx.back_x2 = 239;
	g_ble_pxp_screen_cntx.back_y2 = 239;
	g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
	g_ble_pxp_screen_cntx.content_y = (g_ble_pxp_screen_cntx.back_y2 - g_ble_pxp_screen_cntx.title_y - PXP_ITEM_HEIGHT)/2;

	gdi_draw_solid_rect(0,0,239,239, g_ble_pxp_screen_cntx.bg_color);

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                                strlen("BLE PXP"),
                                &string_width,
                                &string_height);
    font_engine_show_string(g_ble_pxp_screen_cntx.title_x, 
                            g_ble_pxp_screen_cntx.title_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                            strlen("BLE PXP"), 
                            g_ble_pxp_screen_cntx.font_color);

    /**determine what to display by "disconencted or connected or out of range"*/
    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Out of range"), 
                                strlen("Out of range"),
                                &string_width,
                                &string_height);
	font_engine_show_string(g_ble_pxp_screen_cntx.content_x, 
                            g_ble_pxp_screen_cntx.content_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Out of range"), 
                            strlen("Out of range"), 
                            g_ble_pxp_screen_cntx.font_color);
 

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                                strlen("Exit"),
                                &string_width,
                                &string_height);
    
	g_ble_pxp_screen_cntx.back_x1 = g_ble_pxp_screen_cntx.back_x2 - string_width;
	font_engine_show_string(g_ble_pxp_screen_cntx.back_x1, 
                            g_ble_pxp_screen_cntx.back_y1, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                            strlen("Exit"), 
                            g_ble_pxp_screen_cntx.font_color);
    g_range_screen_exited = 1;

	gdi_flush_screen();
}

static void ble_pxp_show_screen(ble_pxp_show_type_t type)
{
    switch (type) {
        case BLE_PXP_SHOW_DISCONNECT:
            if (BT_PXP == bt_common_callback_get_demo()) {
                ble_pxp_show_main_screen();
            } else if (BT_AUDIO == bt_common_callback_get_demo()) {
                bt_sink_event_post(BT_SINK_EVENT_EXTERN_PXP_DISCONNECT, NULL, NULL);
            }
            break;

        case BLE_PXP_SHOW_CONNECT:
            if (BT_PXP == bt_common_callback_get_demo()) {
                ble_pxp_show_connected_screen();
            } else if (BT_AUDIO == bt_common_callback_get_demo()) {
                bt_sink_event_post(BT_SINK_EVENT_EXTERN_PXP_CONNECT, NULL, NULL);
            }
            break;

        case BLE_PXP_SHOW_ALERT:
            if (BT_PXP == bt_common_callback_get_demo()) {
                ble_pxp_show_range_alert_screen();
            } else if (BT_AUDIO == bt_common_callback_get_demo()) {
                bt_sink_event_post(BT_SINK_EVENT_EXTERN_PXP_ALERT, NULL, NULL);
            }
            break;

        default:
            break;

    }
}

static uint16_t ble_pxp_get_cntx_index_by_conn_id(uint16_t conn_id)
{
    uint16_t i;
    uint16_t index = 0;
    
    for (i = 0; i < MAX_LE_DEV_NUM; i++) {
        if (conn_id == g_pxp_cntx_t[i].conn_id) {
            index = i;
            break;
        }
    }
	LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_get_cntx_index_by_conn_id: index[%d]\r\n", index);
    return index;
}

static bool ble_pxp_have_existed_in_cntx(uint16_t conn_id)
{
    uint16_t i;
    bool is_existed = false;
    
    for (i = 0; i < MAX_LE_DEV_NUM; i++) {
        if (conn_id == g_pxp_cntx_t[i].conn_id) {
            is_existed = true;
            break;
        }
    } 
	LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_have_existed_in_cntx: is_existed[%d],conn_id[%d]\r\n", is_existed, conn_id);
    return is_existed;
}

static uint16_t ble_pxp_get_null_cntx_index()
{
    uint16_t i;
    uint16_t index = 0;
    
    for (i = 0; i < MAX_LE_DEV_NUM; i++) {
        if (0 == g_pxp_cntx_t[i].conn_id) {
            index = i;
            break;
        }
    }
    LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_get_null_cntx_index: null index[%d]\r\n", index);
    return index;
}

static uint16_t ble_pxp_get_link_number()
{
    uint16_t i;
    uint16_t number = 0;
    for (i = 0; i < MAX_LE_DEV_NUM; i++) {
        if (0 != g_pxp_cntx_t[i].conn_id) {
            number ++;
        }
    }
    LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_get_link_number: linked number[%d]\r\n", number);
    return number;
}

/**
 * @brief Function for handling the IAS Service events.
 *
 * @details       This function will be called for all IAS Service events which are passed to
 *			      the application.
 *
 * @param[in]     p_ias	       IAS Service structure.
 * @param[in]     p_evt	       Event received from the IAS Service.
 */
void ble_pxp_ias_evt_handler(ble_ias_t *p_ias, ble_ias_evt_t *p_evt)
{
    if (false == ble_pxp_have_existed_in_cntx(p_evt->conn_id)) {
        LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_ias_evt_handler, invalid conn id \r\n");
        return;
    } else {
        uint16_t conn_id = p_evt->conn_id;
        uint16_t index = ble_pxp_get_cntx_index_by_conn_id(conn_id);
        LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_ias_evt_handler: conn_id[%d]\r\n", conn_id);

        switch (p_evt->evt_type) {
            case BLE_IAS_ALERT_LEVEL_EVT_WRITE:
            {
                g_pxp_cntx_t[index].ias_cntx.alert_level.alert_level.alert_level = p_evt->params.alert_level.alert_level.alert_level;
                LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_ias_evt_handler alert_level_write: level[%d]\r\n", p_evt->params.alert_level.alert_level.alert_level);
                if (((g_pxp_cntx_t[index].ias_cntx.alert_level.alert_level.alert_level == 1) ||
                    (g_pxp_cntx_t[index].ias_cntx.alert_level.alert_level.alert_level == 2) ||
                    (g_pxp_cntx_t[index].ias_cntx.alert_level.alert_level.alert_level == 3) ||
                    (g_pxp_cntx_t[index].ias_cntx.alert_level.alert_level.alert_level == 4)) &&
                    (g_conn_screen_exited == 1)) {
                    ble_pxp_show_screen(BLE_PXP_SHOW_ALERT);
                }
                if ((g_pxp_cntx_t[index].ias_cntx.alert_level.alert_level.alert_level == 0) &&
                    (g_range_screen_exited == 1)) {
                    ble_pxp_show_screen(BLE_PXP_SHOW_CONNECT);
                }
            }
            break;

            default :
            break;
        }
    }
}

/**
 * @brief Function for handling the LLS Service events.
 *
 * @details     This function will be called for all LLS Service events which are passed to
 *			    the application.
 *
 * @param[in]	p_lls  	LLS Service structure.
 * @param[in]	p_evt	Event received from the LLS Service.
 */
void ble_pxp_lls_evt_handler(ble_lls_t *p_lls, ble_lls_evt_t *p_evt)
{
    if (false == ble_pxp_have_existed_in_cntx(p_evt->conn_id)) {
        LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_lls_evt_handler, invalid conn id \r\n");
        return;
    } else {
        uint16_t conn_id = p_evt->conn_id;
        uint16_t index = ble_pxp_get_cntx_index_by_conn_id(conn_id);
        LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_lls_evt_handler: conn_id[%d]\r\n", conn_id);

        switch (p_evt->evt_type) {
            case BLE_LLS_ALERT_LEVEL_EVT_READ:
            {	
                /*default value: 0*/
                ble_lls_alert_level_read_rsp(p_lls, &g_pxp_cntx_t[index].lls_cntx.alert_level, conn_id, 0);
            }
             break;
            case BLE_LLS_ALERT_LEVEL_EVT_WRITE:
            {
                g_pxp_cntx_t[index].lls_cntx.alert_level.alert_level.alert_level = p_evt->params.alert_level.alert_level.alert_level;
                LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_lls_evt_handler alert_level_write: level[%d]\r\n", p_evt->params.alert_level.alert_level.alert_level);
            }
             break;
            default:
             break;
        }
    }
}

/**
 * @brief Function for handling the TPS Service events.
 *
 * @details     This function will be called for all TPS Service events which are passed to
 *			    the application.
 *
 * @param[in]	p_tps  	TPS Service structure.
 * @param[in]	p_evt	Event received from the TPS Service.
 */
void ble_pxp_tps_evt_handler(ble_tps_t *p_tps, ble_tps_evt_t *p_evt)
{
    if (false == ble_pxp_have_existed_in_cntx(p_evt->conn_id)) {
        LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_tps_evt_handler, invalid conn id \r\n");
        return;
    } else {
        uint16_t conn_id = p_evt->conn_id;
        /** uint16_t index = ble_pxp_get_cntx_index_by_conn_id(conn_id);*/
        LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_tps_evt_handler: conn_id[%d]\r\n", conn_id);

        switch (p_evt->evt_type) {
            case BLE_TPS_TX_POWER_LEVEL_EVT_READ:
            {
                ble_status_t error_code;
                error_code = ble_gap_get_rssi(conn_id);
                LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_tps_evt_handler txpower read error code:[%d]\r\n", error_code);
            }
            break;
             
            default :
             break;
        }
    }
}

void ble_pxp_get_rssi_callback(ble_tps_t *p_tps, ble_gap_report_rssi_ind_t *rssi)
{
	ble_tps_tx_power_level_t tx_power;
    
    if (false == ble_pxp_have_existed_in_cntx(rssi->conn_id)) {
        LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_get_rssi_callback, invalid conn id \r\n");
        return;
    } else {
        uint16_t conn_id = rssi->conn_id;
        /** uint16_t index = ble_pxp_get_cntx_index_by_conn_id(conn_id);*/
        LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_get_rssi_callback: conn_id[%d]\r\n", conn_id);
        
		if (rssi->error_code == BLE_STATUS_SUCCESS) {
			tx_power.tx_power = rssi->rssi;
			ble_tps_tx_power_level_read_rsp(p_tps, &tx_power, conn_id, 0);
		} else {
			LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_get_rssi_callback result: err[%d]\r\n", rssi->error_code);
		}
	}
}

void ble_pxp_connection_param_updated_callback(ble_gap_connection_param_updated_ind_t *conn_updated)
{
	ble_gap_conn_param_priority_t conn_param;
	conn_param = ble_gap_get_current_connection_interval(conn_updated);
    if (true == ble_pxp_have_existed_in_cntx(conn_updated->conn_id)) {
        uint16_t index = ble_pxp_get_cntx_index_by_conn_id(conn_updated->conn_id);
        if (conn_param == g_pxp_cntx_t[index].current_priority) {
            LOG_I(PXP_MAIN, "[BLE_PXP] connection param update successfully \r\n");
        }
    } else {
        LOG_I(PXP_MAIN, "[BLE_PXP] connection param update fail, invalid conn id \r\n");
    }

}

ble_status_t ble_pxp_set_advertising_interval(uint16_t conn_id)
{
	ble_status_t status;
    uint8_t name[8] = {0};
	ble_gap_adv_interval_enum_t interval = BLE_GAP_ADV_INTERVAL_FAST_CONNECT;
	
	memcpy(name, "pxp", sizeof("pxp"));
	status = ble_gap_set_advertising_interval(conn_id, name, interval);
    LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_set_advertising_interval: status[%d]\r\n", status);
	return status;
}

ble_status_t ble_pxp_update_connection_interval(uint16_t conn_id)
{
	ble_status_t status;
    uint8_t name[8] = {0};
	ble_gap_conn_param_priority_t interval = BLE_GAP_CONN_PARAM_HIGH_SPEED_IOS;
	
	memcpy(name, "pxp", sizeof("pxp"));
	status = ble_gap_update_connection_interval(conn_id, name, interval);
    if (status == BLE_STATUS_GAP_PENDING) {
        uint16_t index = ble_pxp_get_cntx_index_by_conn_id(conn_id);
        g_pxp_cntx_t[index].current_priority = interval;
    }     
    LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_update_connection_interval: status[%d]\r\n", status);
	return status;
}

/**
 * @brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_pxp_on_ble_evt(ble_evt_t *p_ble_evt)
{
	LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_on_ble_evt, evt_id: %d\r\n", p_ble_evt->header.evt_id);
    
    switch (p_ble_evt->header.evt_id) {
        case BLE_POWER_ON_CNF:
        {
            if (BLE_STATUS_SUCCESS == p_ble_evt->evt.gap_evt.power_on.result) {
                LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_on_ble_evt, power on\r\n");
                bt_power_on_status = true;
                ble_pxp_main();
                ble_pxp_start_advertise();
            }
        }
        break;
        case BLE_POWER_OFF_CNF:
        {
            if (BLE_STATUS_SUCCESS == p_ble_evt->evt.gap_evt.power_off.result) {
                 LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_on_ble_evt, power off\r\n");
                bt_power_on_status = false;
                bt_common_callback_set_demo(BT_IDLE);
            }
        }
        break;
        case BLE_GAP_CONNECTED_IND:
		{
            if (ble_pxp_get_link_number() < MAX_LE_DEV_NUM) {
                uint16_t conn_id = p_ble_evt->evt.gap_evt.connected.conn_id;
                if (false == ble_pxp_have_existed_in_cntx(conn_id)) {
                    uint16_t index = ble_pxp_get_null_cntx_index();
                    memset(&g_pxp_cntx_t[index], 0, sizeof(ble_gatts_pxp_cntx_t));
                    g_pxp_cntx_t[index].conn_id = conn_id;
                    if (g_disc_screen_exited == 1) {        
                        g_conn_id = conn_id;
                        
                        ble_gap_stop_advertiser();
                        ble_pxp_show_screen(BLE_PXP_SHOW_CONNECT);
                    }
                    LOG_I(PXP_MAIN, "[BLE_PXP] BLE_GAP_CONNECTED_IND: conn_id[%d]\r\n", conn_id);
                }
            }
    	}
            break;
        case BLE_GAP_DISCONNECTED_IND:
        {
            if (true == ble_pxp_have_existed_in_cntx(p_ble_evt->evt.gap_evt.disconnected.conn_id)) {
                uint16_t index = ble_pxp_get_cntx_index_by_conn_id(p_ble_evt->evt.gap_evt.disconnected.conn_id);
                memset(&g_pxp_cntx_t[index], 0, sizeof(ble_gatts_pxp_cntx_t));
                g_conn_id = 0;
                if ((g_conn_screen_exited == 1) || (g_range_screen_exited == 1)) {
                    ble_pxp_show_screen(BLE_PXP_SHOW_DISCONNECT);
                }
                
                LOG_I(PXP_MAIN, "[BLE_PXP] BLE_GAP_DISCONNECTED_IND: conn_id[%d]\r\n", p_ble_evt->evt.gap_evt.disconnected.conn_id);
            }
        }
            break;

		case BLE_GAP_ADVERTISING_IND:
			break;

		case BLE_GAP_CONNECTION_PARAM_UPDATED_IND:
			ble_pxp_connection_param_updated_callback(&(p_ble_evt_t.evt.gap_evt.conn_params));
			break;

		case BLE_GAP_REPORT_RSSI_IND:
			ble_pxp_get_rssi_callback(&p_tps_t, &(p_ble_evt_t.evt.gap_evt.tx_power));
			break;
		
        default:
            /**< No implementation needed. */
            break;
    }
}


/**
 * @brief Function for initializing services that will be used by the application.
 */
static void ble_pxp_services_init(void)
{
    int32_t err_code;
	ble_ias_init_t ias_init_t;
	ble_lls_init_t lls_init_t;
	ble_tps_init_t tps_init_t;
	
    /**< Initialize IAS Service. */
	memset(&ias_init_t, 0, sizeof(ble_ias_init_t));
	memset(&p_ias_t, 0, sizeof(ble_ias_t));
	ias_init_t.evt_handler = ble_pxp_ias_evt_handler;
	err_code = ble_ias_init(&p_ias_t, &ias_init_t);
	if (0 != err_code) {
		LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_services_init ias init: err_code[%d] \r\n", err_code);
	}
	
	/**< Initialize LLS Service. */
	memset(&lls_init_t, 0, sizeof(ble_lls_init_t));
	memset(&p_lls_t, 0, sizeof(ble_lls_t));
	lls_init_t.evt_handler = ble_pxp_lls_evt_handler;
	err_code = ble_lls_init(&p_lls_t, &lls_init_t);	
	if (0 != err_code) {
		LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_services_init lls init: err_code[%d] \r\n", err_code);
	}

    /**< Initialize TPS Service. */
	memset(&tps_init_t, 0, sizeof(ble_tps_init_t));
	memset(&p_tps_t, 0, sizeof(ble_tps_t));
	tps_init_t.evt_handler = ble_pxp_tps_evt_handler;
	err_code = ble_tps_init(&p_tps_t, &tps_init_t);
  	if (0 != err_code) {
		LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_services_init tps init: err_code[%d] \r\n", err_code);
	}
}


/**
 * @brief Function for application main entry.
 */
void ble_pxp_main(void)
{
    /**< Initialize */
	ble_pxp_services_init();
	LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_main\r\n");
}


/**
 * @brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details   This function is called from the BLE Stack event interrupt handler after a BLE stack
 *            event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_pxp_evt_dispatch(ble_evt_t *p_ble_evt)
{
	LOG_I(PXP_MAIN, "[BLE_PXP] ble_pxp_evt_dispatch\r\n");

	/**< add your app's gatts event handler as below. */
	ble_ias_on_ble_evt(&p_ias_t, p_ble_evt);
	ble_lls_on_ble_evt(&p_lls_t, p_ble_evt);
	ble_tps_on_ble_evt(&p_tps_t, p_ble_evt);
	ble_pxp_on_ble_evt(p_ble_evt);
}

/**
 * @brief Function for distinguishing a BLE stack event to all modules with a BLE stack Gatt event handler.
 *
 * @details   This function is called from the BLE Stack Gatt event interrupt handler 
 *
 * @param[in] message_id  Message from Bluetooth stack Task.
 * @param[in] *param      Message parameters.
 */
void ble_gatts_message_handler(ble_gatts_message_id_t message_id, void *param)
{
	LOG_I(PXP_MAIN, "[BLE_PXP] ble_gatts_message_handler: message_id[%d] \r\n", message_id);
    
	switch (message_id) {	
		case BLE_MSG_GATT_EXCHANGE_MTU_REQ_IND:
		{ 
            p_ble_evt_t.header.evt_id = BLE_GATT_EXCHANGE_MTU_REQ;
			memcpy(&(p_ble_evt_t.evt.gatts_evt.mtu_change), param, sizeof(ble_gatt_exchange_mtu_req_t));
            LOG_I(PXP_MAIN, "[BLE_PXP] mtu_change: conn_id[%d], expected_mtu[%d] \r\n", p_ble_evt_t.evt.gatts_evt.mtu_change.conn_id, p_ble_evt_t.evt.gatts_evt.mtu_change.expected_mtu);
			ble_gatt_confirm_exchange_mtu(p_ble_evt_t.evt.gatts_evt.mtu_change.conn_id, BLE_GATT_MTU_SIZE_CUSTOM);
		}
			break;
			
		case BLE_MSG_GATTS_READ_REQ_IND:
		{  
            ble_gatts_read_req_t *temp_req = (ble_gatts_read_req_t *)param;
            
            p_ble_evt_t.header.evt_id = BLE_GATTS_READ_REQ;
			memcpy(&(p_ble_evt_t.evt.gatts_evt.read), param, sizeof(ble_gatts_read_req_t));

            memset(read_handle, 0, PXP_MAX_MULTIPLE_HANDLE_NUMBER);
            if (temp_req->count <= PXP_MAX_MULTIPLE_HANDLE_NUMBER) {
                memcpy(read_handle, temp_req->handle, (sizeof(uint16_t) * temp_req->count));
            } else {
                memcpy(read_handle, temp_req->handle, (sizeof(uint16_t) * PXP_MAX_MULTIPLE_HANDLE_NUMBER));
            }
            ble_gatts_free_memory(temp_req->handle);
            p_ble_evt_t.evt.gatts_evt.read.handle = read_handle;
		}
			break;
			
		case BLE_MSG_GATTS_WRITE_REQ_IND:
		{  
            ble_gatts_write_req_t *temp_data = (ble_gatts_write_req_t *)param;
            
            p_ble_evt_t.header.evt_id = BLE_GATTS_WRITE_REQ;
			memcpy(&(p_ble_evt_t.evt.gatts_evt.write), param, sizeof(ble_gatts_write_req_t));
            memset(write_data, 0, PXP_MAX_WRITE_DATA_LENGTH);   
            if (temp_data->length <= PXP_MAX_WRITE_DATA_LENGTH) {
                memcpy(write_data, temp_data->data, (sizeof(uint8_t) * temp_data->length));
            } else {
                memcpy(write_data, temp_data->data, (sizeof(uint8_t) * PXP_MAX_WRITE_DATA_LENGTH));
            }
            ble_gatts_free_memory(temp_data->data);
            p_ble_evt_t.evt.gatts_evt.write.data = write_data;

		}
			break;
			
		case BLE_MSG_GATTS_EXCUTE_WRITE_REQ_IND:
		{    
            p_ble_evt_t.header.evt_id = BLE_GATTS_EXECUTE_WRITE_REQ;
			memcpy(&(p_ble_evt_t.evt.gatts_evt.excute_write), param, sizeof(ble_gatts_execute_write_req_t));
		}
			break;

		case BLE_MSG_GATTS_HANDLE_VALUE_IND:
		{
            p_ble_evt_t.header.evt_id = BLE_GATTS_HANDLE_VALUE_CNF;
			memcpy(&(p_ble_evt_t.evt.gatts_evt.confirm), param, sizeof(ble_gatts_handle_value_cnf_t));
		}
			break;
							
		default:
			break;	
	}
	if (param != NULL) {
        ble_gatts_free_memory(param);
    }
	ble_pxp_evt_dispatch(&p_ble_evt_t);	
}

/**
 * @brief Function for distinguishing a BLE stack event to all modules with a BLE stack Gap event handler.
 *
 * @details   This function is called from the BLE Stack Gap event interrupt handler 
 *
 * @param[in] message_id  Message from Bluetooth stack Task.
 * @param[in] *param      Message parameters.
 */
void ble_gap_message_handler(ble_gap_message_id_t message_id, void *param)
{
	LOG_I(PXP_MAIN, "[BLE_PXP] ble_gap_message_handler: message_id[%d] \r\n", message_id);
    
	switch (message_id) {
        case BLE_MSG_GAP_POWER_ON_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_POWER_ON_CNF;
			memcpy(&(p_ble_evt_t.evt.gap_evt.power_on), param, sizeof(ble_power_on_cnf_t));
        }
            break;
        case BLE_MSG_GAP_POWER_OFF_IND:
        {
            p_ble_evt_t.header.evt_id = BLE_POWER_OFF_CNF;
			memcpy(&(p_ble_evt_t.evt.gap_evt.power_off), param, sizeof(ble_power_off_cnf_t));
        }
            break;
		case BLE_MSG_GAP_CONNECTED_IND:
		{
            p_ble_evt_t.header.evt_id = BLE_GAP_CONNECTED_IND;
			memcpy(&(p_ble_evt_t.evt.gap_evt.connected), param, sizeof(ble_gap_connected_ind_t));
		}
			break;
		case BLE_MSG_GAP_DISCONNECTED_IND:
		{  
            p_ble_evt_t.header.evt_id = BLE_GAP_DISCONNECTED_IND;
			memcpy(&(p_ble_evt_t.evt.gap_evt.disconnected), param, sizeof(ble_gap_disconnected_ind_t));
		}
			break;
		case BLE_MSG_GAP_REPORT_RSSI_IND:
		{   
            p_ble_evt_t.header.evt_id = BLE_GAP_REPORT_RSSI_IND;
			memcpy(&(p_ble_evt_t.evt.gap_evt.tx_power), param, sizeof(ble_gap_report_rssi_ind_t));
		}
			break;
		case BLE_MSG_GAP_ADVERTISING_IND:
		{  
            p_ble_evt_t.header.evt_id = BLE_GAP_ADVERTISING_IND;
			memcpy(&(p_ble_evt_t.evt.gap_evt.advertising), param, sizeof(ble_gap_advertising_ind_t));
		}
			break;	
    	case BLE_MSG_GAP_CONNECTION_PARAM_UPDATED_IND:
		{   
            p_ble_evt_t.header.evt_id = BLE_GAP_CONNECTION_PARAM_UPDATED_IND;
			memcpy(&(p_ble_evt_t.evt.gap_evt.conn_params), param, sizeof(ble_gap_connection_param_updated_ind_t));
		}
			break;
		default:
			break;
	}
	
	if (param != NULL) {
        ble_gatts_free_memory(param);
    }
	ble_pxp_evt_dispatch(&p_ble_evt_t);
}

void ble_pxp_show_unconnected_screen(void)
{
    bt_common_callback_set_demo(BT_PXP);
    bt_gap_power_on(); 
    ble_pxp_show_screen(BLE_PXP_SHOW_DISCONNECT);
    printf("\n ble_pxp_show_unconnected_screen enter \n ");
}

void ble_app_handle_message(ble_message_context_struct *message)
{
    printf("Receive message = %d", message->message_id);
    if (message->message_id >= BLE_MSG_GATT_EXCHANGE_MTU_REQ_IND && message->message_id <= BLE_MSG_GATTS_HANDLE_VALUE_IND) {
        ble_gatts_message_handler((ble_gatts_message_id_t)(message->message_id), message->parameter);
    } else if (message->message_id >= BLE_MSG_GAP_POWER_ON_IND && message->message_id <= BLE_MSG_GAP_DISCONNECTED_IND) {
     ble_gap_message_handler((ble_gap_message_id_t)(message->message_id), message->parameter);
    }
}

void ble_app_task(void *arg)
{
    ble_message_context_struct message;
    ble_message_create_queue();
    while(1) {
        if (ble_message_receive(&message)) {
            ble_app_handle_message(&message);
            ble_message_free_message_buffer(&message);
        }
    }
}





