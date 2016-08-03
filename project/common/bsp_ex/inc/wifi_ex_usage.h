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
 * @file        wifi_ex_usage.h
 * @brief       WiFi Example CLI Usage
 *
 * @history     2015/08/18   Initial for 1st draft  (Michael Rong)
 */

#ifndef __WIFI_EX_USAGE_H__
#define __WIFI_EX_USAGE_H__


#ifdef __cplusplus
extern "C" {
#endif


#if 0
/**
* @brief Example of Get WiFi BSSID.
* <br><b>
* wifi config get bssid <port>
* </b></br>
* @param [OUT]bssid BSSID
* @return  >=0 means success, <0 means fail
* @note Only used for WIFI_PORT_STA
*/
static uint8_t wifi_config_get_bssid_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set WiFi BSSID.
* <br><b>
* wifi config set bssid <port> <BSSID> --> "00:0c:43:76:87:00"
* </b></br>
* @param [IN]bssid BSSID
* @return  >=0 means success, <0 means fail
* @note Only used for WIFI_PORT_STA
*/
static uint8_t wifi_config_set_bssid_ex(uint8_t len, char *param[]);
#endif

/**
* @brief Example of Get WiFi Scan Table Size
* <br><b>
* wifi config get scantabsize
* </b></br>
* @param [OUT]num 1 ~ 16
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_get_scan_tab_size_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set WiFi Scan Table Size
* <br><b>
* wifi config set scantabsize <size>
* </b></br>
* @param [IN]size 1 ~ 16
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_set_scan_tab_size_ex(uint8_t len, char *param[]);
/**
* @brief Set N9 Debug Level
* <br><b>
* wifi config set n9dbg <dbg_level>
* </b></br>
* @param [IN]dbg_level
*        0: None,
*        1: ERROR,
*        2: WARNING,
*        3: TRACE,
*        4: INFO,
*        5: LAUD
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_set_n9_dbg_level(uint8_t len, char *param[]);
/**
* @brief Set CM4 Debug Level
* <br><b>
* wifi config set cm4dbg <dbg_level>
* </b></br>
* @param [IN]dbg_level
*        0: None,
*        1: ERROR,
*        2: WARNING,
*        3: TRACE,
*        4: INFO,
*        5: LAUD
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_set_cm4_dbg_level(uint8_t len, char *param[]);
/**
* @brief Get N9 Debug Level
* <br><b>
* wifi config get n9dbg
* </b></br>
* @param [OUT]dbg_level
*        0: None,
*        1: ERROR,
*        2: WARNING,
*        3: TRACE,
*        4: INFO,
*        5: LAUD
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_get_n9_dbg_level(uint8_t len, char *param[]);
/**
* @brief Get CM4 Debug Level
* <br><b>
* wifi config get cm4dbg
* </b></br>
* @param [OUT]dbg_level
*        0: None,
*        1: ERROR,
*        2: WARNING,
*        3: TRACE,
*        4: INFO,
*        5: LAUD
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_get_cm4_dbg_level(uint8_t len, char *param[]);





/* ---------------------- Connect Ex ----------------------------*/





/**
* @brief Example of Get the AP information list of the last scan operation
* <br><b>
* wifi connect get scanlist
* </b></br>
* @param [OUT]scan_list
* @return Number of AP scaned and a SCAN_LIST data structure will be returned
* @note If no wifi_conn_start_scan() has been performed, an empty data structure is returned
*/
static uint8_t wifi_connect_get_scan_list_ex(uint8_t len, char *param[]);
/**
* @brief Example of Start/Stop WiFi Scanning
* <br><b>
* wifi connect set scan <start/stop> <ssid> <bssid> <flag>
* </b></br>
* @param [IN]ssid SSID, "NULL" means not specified
* @param [IN]bssid BSSID, "NULL" means not specified
* @param [IN]flag
*        0 Active Scan
*        1 Passive Scan
*
* @return  >=0 means success, <0 means fail
* @note When SSID/BSSID specified (not NULL)
* @note 1. ProbeReq carries SSID (for Hidden AP)
* @note 2. Scan Tab always keeps the APs of this specified SSID/BSSID
*/
static uint8_t wifi_connect_scan_ex(uint8_t len, char *param[]);



/* ---------------------- Profile Ex ----------------------------*/

/**
* @brief Store opmode to the profile in NVRAM.
* <br><b>
* wifi profile set opmode <mode>
* </b></br>
* @param [IN]mode
*       1: WIFI_MODE_STA_ONLY,
*       2: WIFI_MODE_AP_ONLY,
*       3: WIFI_MODE_REPEATER
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_opmode_ex(uint8_t len, char *param[]);
/**
* @brief Store channel to the profile in NVRAM.
* <br><b>
* wifi profile set ch <port> <ch>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [IN]ch    1~14 are supported for 2.4G only product.
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_channel_ex(uint8_t len, char *param[]);
/**
* @brief Store bandwidth to the profile in NVRAM.
* <br><b>
* wifi profile set bw <port> <bw>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [IN]bw   0: HT20, 1: HT40, 2: HT20/40 coexistence
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_bandwidth_ex(uint8_t len, char *param[]);
/**
* @brief Store SSID to the profile in NVRAM.
* <br><b>
* wifi profile set ssid <port> <ssid>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [IN]ssid SSID
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_ssid_ex(uint8_t len, char *param[]);
/**
* @brief Store WiFi Wireless Mode to the profile in NVRAM.
* <br><b>
* wifi profile set wirelessmode <port> <mode>
* </b></br>
* @param [IN]mode
*        1: legacy 11B only,
*        2: legacy 11A only,
*        3: legacy 11A/B/G mixed,
*        4: legacy 11G only,
*        5: 11ABGN mixed,
*        6: 11N only in 2.4G,
*        7: 11GN mixed,
*        8: 11AN mixed,
*        9: 11BGN mixed,
*       10: 11AGN mixed
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_wireless_mode_ex(uint8_t len, char *param[]);
/**
* @brief Store WiFi Country Region to the profile in NVRAM
* <br><b>
* wifi config set country <band> <region>
* </b></br>
* @param [IN]band  0: 2.4G, 1: 5G
* @param [IN]region
* <br>
*        region: (2.4G)
* </br>
*        0:  CH1-11,
*        1:  CH1-13,
*        2:  CH10-11,
*        3:  CH10-13,
*        4:  CH14,
*        5:  CH1-14 all active scan,
*        6:  CH3-9,
*        7:  CH5-13
* </br>
* <br>
*        region: (5G)
* </br>
* <br>
*        0:   CH36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165
* </br>
* <br>
*        1:   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140
* </br>
* <br>
*        2:   CH36, 40, 44, 48, 52, 56, 60, 64
* </br>
* <br>
*        3:   CH52, 56, 60, 64, 149, 153, 157, 161
* </br>
* <br>
*        4:   CH149, 153, 157, 161, 165
* </br>
* <br>
*        5:   CH149, 153, 157, 161
* </br>
* <br>
*        6:   CH36, 40, 44, 48
* </br>
* <br>
*        7:   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165
* </br>
* <br>
*        8:   CH52, 56, 60, 64
* </br>
* <br>
*        9:   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165
* </br>
* <br>
*        10:  CH36, 40, 44, 48, 149, 153, 157, 161, 165
* </br>
* <br>
*        11:  CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 149, 153, 157, 161
* </br>
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_country_region_ex(uint8_t len, char *param[]);
/**
* @brief Store WiFi DTIM Interval to the profile in NVRAM
* <br><b>
* wifi profile set dtim <interval>
* </b></br>
* @param [IN]interval 1 ~ 255
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_dtim_interval_ex(uint8_t len, char *param[]);
/**
* @brief Store WiFi Listen Interval to the profile in NVRAM
* <br><b>
* wifi profile set listen <interval>
* </b></br>
* @param [IN]interval 1 ~ 255
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_listen_interval_ex(uint8_t len, char *param[]);
/**
* @brief Store WiFi MAC address for STA/AP wireless port to the profile in NVRAM
* <br><b>
* wifi profile set mac <port> <mac_addr>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [IN]mac_addr 6 byte
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_mac_address_ex(uint8_t len, char *param[]);
/**
* @brief Store the authentication mode for STA/AP wireless port to the profile in NVRAM
* <br><b>
* wifi profile set pskmode <port> <auth> <encrypt>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [IN]authmode
*           0: OPEN,
*           1: SHARED,
*           2: AUTO_WEP,
*           3: WPA,
*           4: WPA_PSK,
*           5: WPA_None,
*           6: WPA2,
*           7: WPA2_PSK,
*           8: WPA_WPA2,
*           9: WPA_PSK_WPA2_PSK
* @param [IN] encryption type
*           0: WEP_ENABLED,
*           1: ENCRYPT_DISABLED,
*           2: WEP_KEY_ABSENT,
*           3: WEP_NOT_SUPPORTED,
*           4: TKIP_ENABLED,
*           5: ENCRYPT2_KEY_ABSENT,
*           6: AES_ENABLED,
*           7: ENCRYPT3_KEY_ABSENT,
*           8: TKIP_AES_MIX,
*           9: ENCRYPT4_KEY_ABSENT,
*          10: GROUP_WEP40_ENABLED,
*          11: GROUP_WEP104_ENABLED
* @return >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_psk_mode_ex(uint8_t len, char *param[]);
/**
* @brief  Store the passphrase for the specified STA/AP port
* <br><b>
* wifi profile set pskkey <port> <passphrase>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @return >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_psk_key_ex(uint8_t len, char *param[]);
/**
* @brief Store WiFi PMK for the specified STA/AP port into NVRAM
* <br><b>
* wifi profile set pmk <port> <PMK>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [IN]PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
static uint8_t wifi_profile_set_pmk_ex(uint8_t len, char *param[]);
/**
* @brief Store WiFi WEP Keys to the profile in NVRAM.
* <br><b>
* wifi profile set wep <port> <key_id> <key>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [IN]key_id  0 ~ 3
* @param [IN]key  ascii (5 or 13 chars) or hex (10 or 26 chars)
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_wep_key_ex(uint8_t len, char *param[]);
/**
* @brief Get PMK for the specified STA/AP port from NVRAM
* <br><b>
* wifi profile get pmk <port>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
static uint8_t wifi_profile_get_pmk_ex(uint8_t len, char *param[]);

/**
* @brief Store WiFi WEP Keys to the profile in NVRAM.
* <br><b>
* wifi profile set wep <port> <key_id> <key>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [IN]key_id  0 ~ 3
* @param [IN]key  ascii (5 or 13 chars) or hex (10 or 26 chars)
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_opmode_ex(uint8_t len, char *param[]);
/**
* @brief Get channel from the profile in NVRAM.
* <br><b>
* wifi profile get ch <port>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [OUT]channel  1~14 are supported for 2.4G only product.
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_channel_ex(uint8_t len, char *param[]);
/**
* @brief  Get bandwidth from the profile in NVRAM.
* <br><b>
* wifi profile get bw <port>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [OUT]bandwidth The wirelss bandwidth.
*                       IOT_CMD_CBW_20MHZ,
*                       IOT_CMD_CBW_40MHZ, and
*                       IOT_CMD_CBW_2040MHZ are supported.
* @return  >=0 means success, <0 means fail
* @note    Default value is HT_20
*/
static uint8_t wifi_profile_get_bandwidth_ex(uint8_t len, char *param[]);
/**
* @brief Get SSID from the profile in NVRAM.
* <br><b>
* wifi profile get ssid <port>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [OUT]ssid SSID
* @param [OUT]ssid_length Length of SSID
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_ssid_ex(uint8_t length, char *param[]);
/**
* @brief Get WiFi Wireless Mode from the profile in NVRAM.
* <br><b>
* wifi profile get wirelessmode <port>
* </b></br>
* @param [OUT] wirelessmode
*        1: legacy 11B only,
*        2: legacy 11A only,
*        3: legacy 11A/B/G mixed,
*        4: legacy 11G only,
*        5: 11ABGN mixed,
*        6: 11N only in 2.4G,
*        7: 11GN mixed,
*        8: 11AN mixed,
*        9: 11BGN mixed,
*       10: 11AGN mixed
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_wireless_mode_ex(uint8_t len, char *param[]);
/**
* @brief  Get WiFi Country Region from the profile in NVRAM.
* <br><b>
* wifi profile get country <band>
* </b></br>
* @param [IN]band  0: 2.4G, 1: 5G
* @param [OUT]region
* <br>
*        region: (2.4G)
* </br>
*        0:  CH1-11,
*        1:  CH1-13,
*        2:  CH10-11,
*        3:  CH10-13,
*        4:  CH14,
*        5:  CH1-14 all active scan,
*        6:  CH3-9,
*        7:  CH5-13
* </br>
* <br>
*        region: (5G)
* </br>
* <br>
*        0:   CH36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165
* </br>
* <br>
*        1:   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140
* </br>
* <br>
*        2:   CH36, 40, 44, 48, 52, 56, 60, 64
* </br>
* <br>
*        3:   CH52, 56, 60, 64, 149, 153, 157, 161
* </br>
* <br>
*        4:   CH149, 153, 157, 161, 165
* </br>
* <br>
*        5:   CH149, 153, 157, 161
* </br>
* <br>
*        6:   CH36, 40, 44, 48
* </br>
* <br>
*        7:   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165
* </br>
* <br>
*        8:   CH52, 56, 60, 64
* </br>
* <br>
*        9:   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165
* </br>
* <br>
*        10:  CH36, 40, 44, 48, 149, 153, 157, 161, 165
* </br>
* <br>
*        11:  CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 149, 153, 157, 161
* </br>

* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_country_region_ex(uint8_t len, char *param[]);
/**
* @brief Get WiFi DTIM Interval from the profile in NVRAM.
* <br><b>
* wifi profile get dtim
* </b></br>
* @param  [OUT]interval: 1~255
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_dtim_interval_ex(uint8_t len, char *param[]);
/**
* @brief Get WiFi Listen Interval from the profile in NVRAM.
* <br><b>
* wifi profile get listen
* </b></br>
* @param  [OUT]interval: 1~255
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_listen_interval_ex(uint8_t len, char *param[]);
/**
* @brief Get WiFi mac address from the profile in NVRAM.
* <br><b>
* wifi profile get mac
* </b></br>
* @param  [OUT]mac_addr: 6 bytes
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_mac_address_ex(uint8_t len, char *param[]);
/**
* @brief Get the authentication mode for the specified STA/AP port from the profile in NVRAM.
* <br><b>
* wifi profile get pskmode <port>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [OUT]authmode
*           0: OPEN,
*           1: SHARED,
*           2: AUTO_WEP,
*           3: WPA,
*           4: WPA_PSK,
*           5: WPA_None,
*           6: WPA2,
*           7: WPA2_PSK,
*           8: WPA_WPA2,
*           9: WPA_PSK_WPA2_PSK
* @param [OUT] encryption type
*           0: WEP_ENABLED,
*           1: ENCRYPT_DISABLED,
*           2: WEP_KEY_ABSENT,
*           3: WEP_NOT_SUPPORTED,
*           4: TKIP_ENABLED,
*           5: ENCRYPT2_KEY_ABSENT,
*           6: AES_ENABLED,
*           7: ENCRYPT3_KEY_ABSENT,
*           8: TKIP_AES_MIX,
*           9: ENCRYPT4_KEY_ABSENT,
*          10: GROUP_WEP40_ENABLED,
*          11: GROUP_WEP104_ENABLED
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
static uint8_t wifi_profile_get_psk_mode_ex(uint8_t length, char *param[]);
/**
* @brief Get the passphrase for the specified STA/AP port from the profile in NVRAM.
* <br><b>
* wifi profile get pskkey <port>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [OUT]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @param [OUT]passphrase_len 8 ~ 64
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
static uint8_t wifi_profile_get_psk_key_ex(uint8_t length, char *param[]);
/**
* @brief Get WiFi WEP Keys from the profile in NVRAM.
* wifi profile get wep <port>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [OUT]wifi_wep_key_t
* @return >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_wep_key_ex(uint8_t len, char *param[]);
/**
* @brief Save N9 Debug Level into NVRAM
* <br><b>
* wifi profile set n9dbg <dbg_level>
* </b></br>
* @param [IN]dbg_level
*        0: None
*        1: ERROR
*        2: WARNING
*        3: TRACE
*        4: INFO
*        5: LAUD
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_n9_dbg_level(uint8_t len, char *param[]);
/**
* @brief Save CM4 Debug Level into NVRAM
* <br><b>
* wifi profile set cm4dbg <dbg_level>
* </b></br>
* @param [IN]dbg_level
*        0: None
*        1: ERROR
*        2: WARNING
*        3: TRACE
*        4: INFO
*        5: LAUD
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_cm4_dbg_level(uint8_t len, char *param[]);
/**
* @brief Get N9 Debug Level from NVRAM
* <br><b>
* wifi profile get n9dbg
* </b></br>
* @param [OUT]dbg_level
*        0: None
*        1: ERROR
*        2: WARNING
*        3: TRACE
*        4: INFO
*        5: LAUD
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_n9_dbg_level(uint8_t len, char *param[]);
/**
* @brief Get CM4 Debug Level from NVRAM
* <br><b>
* wifi profile get cm4dbg
* </b></br>
* @param [OUT]dbg_level
*        0: None
*        1: ERROR
*        2: WARNING
*        3: TRACE
*        4: INFO
*        5: LAUD
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_cm4_dbg_level(uint8_t len, char *param[]);



#ifdef __cplusplus
}
#endif

#endif /*  __WIFI_EX_USAGE_H__ */
