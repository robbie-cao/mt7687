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
/**
    @file       wifi_ex_connect.c
    @brief      WiFi - Connect API Examples

    @history    2015/08/07  Initial for 1st draft  (Michael Rong)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi_scan.h"
#include "wifi_inband.h"
#include "inband_queue.h"
#include "os.h"
#include "os_util.h"
#include "cli.h"
#include "type_def.h"
#include "wifi_api.h"
#include "wifi_ex_connect.h"
#include "get_profile_string.h"
#include "misc.h"
#include "wifi_cli.h"

#if defined(MTK_HOMEKIT_ENABLE)
/**
* @brief Example of Get the AP information list of the last scan operation
* wifi connect get scanlist
* @param [OUT]scan_list
* @return Number of AP scaned and a SCAN_LIST data structure will be returned
* @note If no wifi_conn_start_scan() has been performed, an empty data structure is returned
*/
static uint8_t wifi_connect_get_scan_list_ex(uint8_t len, char *param[])
{
    wifi_scan_list_t scan_list[MAX_SCAN_ARRAY_ITEM];
    wifi_scan_list_t *ptr = scan_list;
    int i;
    uint8_t status = 0, number = 0;

    for (i = 0; i < MAX_SCAN_ARRAY_ITEM - 1; i++) {
        os_memset(scan_list[i].ap_data.ssid, 0, sizeof(scan_list[i].ap_data.ssid));
        scan_list[i].next = (struct _wifi_scan_list_t *)&scan_list[i + 1];
    }
    scan_list[MAX_SCAN_ARRAY_ITEM - 1].next = NULL;

    status = wifi_connection_get_scan_list(&number, scan_list);

    printf("wifi_connect_get_scan_list, status:%d\n", status);
    i = 0;
    while (ptr) {
        printf("%d\n", i++);
        printf("    rssi:%d\n", ptr->ap_data.rssi);
        printf("    ssid=%s, ssid_len=%d\n", ptr->ap_data.ssid, ptr->ap_data.ssid_length);
        printf("    channel=%d\n", ptr->ap_data.channel);
        printf("    central_channel=%d\n", ptr->ap_data.central_channel);
        printf("    bssid=%02X:%02X:%02X:%02X:%02X:%02X\n", ptr->ap_data.bssid[0],
               ptr->ap_data.bssid[1],
               ptr->ap_data.bssid[2],
               ptr->ap_data.bssid[3],
               ptr->ap_data.bssid[4],
               ptr->ap_data.bssid[5]);
        printf("    wps=%d\n", ptr->ap_data.is_wps_supported);
        printf("    auth_mode=%d\n", ptr->ap_data.auth_mode);
        printf("    encrypt_type=%d\n", ptr->ap_data.encrypt_type);

        ptr = (wifi_scan_list_t *)(ptr->next);
    }
    return status;
}
#endif

extern wifi_scan_list_item_t *g_scan_list;
extern uint8_t g_scan_list_size;

/**
* @brief Example of Start/Stop WiFi Scanning
* wifi connect set scan <start/stop> <mode> <operation> [ssid] [bssid]
* @param [IN]ssid SSID, "NULL" means not specified
* @param [IN]bssid BSSID, "NULL" means not specified
* @param [IN]flag
* @param 0 Active Scan
* @param 1 Passive Scan
*
* @return  >=0 means success, <0 means fail
*
* @note When SSID/BSSID specified (not NULL)
* @note 1. ProbeReq carries SSID (for Hidden AP)
* @note 2. Scan Tab always keeps the APs of this specified SSID/BSSID
*/
static wifi_scan_list_item_t g_ap_list[8] = {{0}};
static void parse_channel_list_from_string(const char *channel_list, ch_list_t *channel_item, uint8_t band)
{
    char string_data[4] = {0};
    uint32_t index = 0;
    uint32_t save_index = 0;
    uint32_t count = 0;
    uint32_t channel_section_count = 0;

    while ('\0' != channel_list[index]) {
        if ('|' == channel_list[index]) {
            channel_item->triplet[channel_section_count + channel_item->num_bg_band_triplet].channel_prop = (char)atoi((char *)string_data);
            channel_section_count++;
            os_memset(string_data, 0, sizeof(string_data));
            save_index = 0;
            count = 0;
        } else if (',' == channel_list[index]) {
            switch(count) {
            case 0: {
                channel_item->triplet[channel_section_count + channel_item->num_bg_band_triplet].first_channel = (char)atoi((char *)string_data);
                break;
            }
            case 1: {
                channel_item->triplet[channel_section_count + channel_item->num_bg_band_triplet].num_of_ch = (char)atoi((char *)string_data);
                break;
            }
            default:
                return;
            }
            os_memset(string_data, 0, sizeof(string_data));
            save_index = 0;
            count++;
        } else {
            string_data[save_index] = channel_list[index];
            save_index++;
        }
        index++;
    }
    channel_item->triplet[channel_section_count].channel_prop = (char)atoi((char *)string_data);
    if (WIFI_BAND_2_4_G == band) {
        channel_item->num_bg_band_triplet = channel_section_count + 1;
    } else if (WIFI_BAND_5_G == band) {
        channel_item->num_a_band_triplet = channel_section_count + 1;
    }
}

static uint8_t wifi_connect_scan_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    ch_list_t *scan_channel_list = NULL;
    uint8_t start = atoi(param[0]);

    int32_t len_param = 0;
    P_CMD_IOT_START_SCAN_T scan_param = NULL;

    scan_channel_list=(ch_list_t *)os_malloc(sizeof(ch_list_t)+6*sizeof(ch_desc_t));
    os_memset(scan_channel_list, 0, sizeof(ch_list_t)+6*sizeof(ch_desc_t));

    if (!start) {
        status = wifi_connection_stop_scan();
    } else {
        uint8_t scan_mode = atoi(param[1]);
        uint8_t scan_option = atoi(param[2]);

        char *chanel_string_2g = NULL;
        char *chanel_string_5g = NULL;
        char *ssid = NULL;
        uint8_t ssid_len = 0;
        uint8_t bssid_val[WIFI_MAX_NUMBER_OF_STA] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
        uint8_t *bssid = (uint8_t *)bssid_val;

        if (len >= 4) {
            chanel_string_2g = param[3];
            if (len == 5) {
                chanel_string_5g = param[4];
                ssid = NULL;
                ssid_len = 0;
                bssid = (uint8_t *)(NULL);
            } else if (len == 6) {
                chanel_string_5g = param[4];
                ssid = param[5];
                ssid_len = os_strlen(ssid);
                bssid = (uint8_t *)(NULL);
            } else if (len >6) {
                chanel_string_5g = param[4];
                ssid = param[5];
                ssid_len = os_strlen(ssid);
                wifi_conf_get_mac_from_str((char *)bssid_val, param[6]);
            }
        } else {
            chanel_string_2g = NULL;
            chanel_string_5g = NULL;
            ssid = NULL;
            ssid_len = 0;
            bssid = (uint8_t *)(NULL);
        }

        if (chanel_string_2g != NULL && strlen(chanel_string_2g) > 4) {
            parse_channel_list_from_string(chanel_string_2g, scan_channel_list, WIFI_BAND_2_4_G);
        }

        if (chanel_string_5g != NULL && strlen(chanel_string_5g) > 4) {
            parse_channel_list_from_string(chanel_string_5g, scan_channel_list, WIFI_BAND_5_G);
        }
        //hex_dump("scan_channel_list",scan_channel_list, sizeof(CH_LIST_T)+6*sizeof(CH_DESC_T));

        len_param = (scan_channel_list->num_a_band_triplet + scan_channel_list->num_bg_band_triplet) * sizeof(ch_desc_t) + sizeof(CMD_IOT_START_SCAN_T);
        scan_param = (P_CMD_IOT_START_SCAN_T)os_malloc(len_param);
        os_memset((unsigned char *)(scan_param), 0x0, len_param);

        /******************reset scan list*******************************/
        wifi_connection_scan_init(g_ap_list, 8);
        //printf("size = %d", sizeof(wifi_scan_list_item_t));
        /******************reset scan list*******************************/


        if (g_scanning && g_scan_by_supplicant) {
            wifi_connection_stop_scan();
            g_scanning = 0;
        }
        g_scan_by_supplicant = 0;
        g_scan_by_app = 1;


        if (ssid && (ssid_len > 0)) {
            os_memcpy(scan_param->SSID, ssid, ssid_len);
            scan_param->enable_ssid = (0x80 | ssid_len);
        }

        if (bssid) {
            os_memcpy(scan_param->BSSID, bssid, WIFI_MAC_ADDRESS_LENGTH);
            scan_param->enable_bssid = 1;
        }

        if (scan_mode > 0) {
            scan_param->partial = 1;
            scan_param->partial_bcn_cnt = 1;
        }
        scan_param->scan_mode = scan_option;
        os_memcpy(&scan_param->channel_list, scan_channel_list, (scan_channel_list->num_a_band_triplet + scan_channel_list->num_bg_band_triplet)* sizeof(ch_desc_t)+2);

        status = inband_queue_exec(WIFI_COMMAND_ID_IOT_START_SCAN, (unsigned char *)(scan_param), len_param, TRUE, FALSE);
        os_free(scan_param);
        if (status == 0) {
            g_scanning = 1;
        }

    }
    return status;
}

#if defined(MTK_HOMEKIT_ENABLE)
static uint8_t wifi_connect_set_scan_prefer(uint8_t len, char *param[])
{

    if (len > 0 && param != NULL) {
        wifi_scan_set_prefer_ssid(param[0], os_strlen(param[0]));
    } else {
        wifi_scan_set_prefer_ssid(NULL, 0);
    }

    return 0;
}
#endif


cmd_t   wifi_connect_set_cli[] = {
    { "scan",       "start/stop scan",                      wifi_connect_scan_ex},
    { "connection", "connect/disconnect to AP",             wifi_connect_set_connection_ex},
    { "deauth",     "de-authenticate specific STA",         wifi_connect_deauth_station_ex},
    { "eventcb",    "register/un-register event callback",  wifi_connect_set_event_callback_ex},
#if defined(MTK_HOMEKIT_ENABLE)
    { "prefer_scan", "scan preferred SSID",                 wifi_connect_set_scan_prefer},
#endif
    { NULL }
};

cmd_t   wifi_connect_get_cli[] = {
    { "stalist",     "get STA list",                        wifi_connect_get_station_list_ex},
    { "linkstatus",  "get link status",                     wifi_connect_get_link_status_ex},
    { "max_num_sta", "get max number of supported STA",     wifi_connect_get_max_station_number_ex},
    { "rssi",        "get rssi",                            wifi_connect_get_rssi_ex},
#if defined(MTK_HOMEKIT_ENABLE)
    { "scanlist",   "get scan list",        wifi_connect_get_scan_list_ex},
#endif
    { NULL }
};

