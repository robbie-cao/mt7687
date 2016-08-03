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
 * @file        wifi_ex.h
 * @brief       WiFi API Example CLI
 *
 * @history     2015/08/18   Initial for 1st draft  (Michael Rong)
 */

#ifndef __WIFI_CLI_H__
#define __WIFI_CLI_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "cli.h"

/**
* @brief Example of Set WiFi Raw Packet Receiver
* <br><b>
* wifi config set rxraw <enable>
* </b></br>
* @param [IN]enable 0: unregister, 1: register
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_rx_raw_pkt_ex(uint8_t len, char *param[]);



uint8_t wifi_tx_raw_pkt_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get MAC address for STA/AP wireless port
* <br><b>
* wifi config get mac <port> --> get port0 (STA) MAC address
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_mac_address_ex(uint8_t len, char *param[]);
/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
* <br><b>
* wifi config set bw <port> <bandwidth>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [IN]bw   0: HT20, 1: HT40, 2: HT20/40 coexistence
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_bandwidth_ex(uint8_t len, char *param[]);
/**
* @brief Example of get bandwidth for STA/AP wireless port.
* <br><b>
* wifi config get bw <port>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_bandwidth_ex(uint8_t len, char *param[]);

/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
*
* wifi config set bw <port>  <0:HT20/1:HT40> <below_above_ch>
* @parameter
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_bandwidth_extended_ex(uint8_t len, char *param[]);

/**
* @brief Example of get bandwidth for STA/AP wireless port.
*  wifi config get bw <port>
* @parameter
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_bandwidth_extended_ex(uint8_t len, char *param[]);

/**
* @brief Example of Configure channel for STA/AP wireless port. STA will keep idle and stay in channel specified
* <br><b>
* wifi config set ch <port> <ch>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @param [IN]ch  1~14 are supported for 2.4G only product
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_channel_ex(uint8_t len, char *param[]);
/**
* @brief Example of get the current channel for STA/AP wireless port.
* <br><b>
* wifi config get ch <port>
* </b></br>
* @param [IN]port 0: STA, 1: AP
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_channel_ex(uint8_t len, char *param[]);
/**
* @brief Example of configure rx filter for packets wanted to be received
* <br><b>
* wifi config set rxfilter <flag>
* </b></br>
* @param [IN]flag defined in  wifi_rx_filter_t
* @return >=0 means success, <0 means fail
* @note Default value will be WIFI_DEFAULT_IOT_RX_FILTER
*/
uint8_t wifi_config_set_rx_filter_ex(uint8_t len, char *param[]);
/**
* @brief Example of get rx filter for packets format wanted to be received
* <br><b>
* wifi config get rxfilter
* </b></br>
* @param [OUT]flag defined in  wifi_rx_filter_t
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_rx_filter_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set the authentication mode for the specified STA/AP port
* <br><b>
* wifi config set psk <port> <auth> <encrypt> <passphrase>
* </b></br>
*
* @param [IN]port 0: STA, 1: AP
* @param [IN]auth authentication mode
*           1: WPAPSK,
*           2: WPA2PSK,
*           3: WPA1PSKWPA2PSK
* @param  [IN]encrypt encryption method index:
*           1: AES,
*           2: TKIP,
*           3: TKIPAES
* @param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_set_psk_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set PMK for the specified STA/AP port
* <br><b>
* wifi config set pmk <port> <PMK>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [IN]PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_set_pmk_ex(uint8_t len, char *param[]);
/**
* @brief Example of Get the authentication mode for the specified STA/AP port
* <br><b>
* wifi config get psk <port>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [OUT]authmode
*           1: WPAPSK,
*           2: WPA2PSK,
*           3: WPA1PSKWPA2PSK
* @param  [OUT] encryption method index:
*           1: AES,
*           2: TKIP,
*           3: TKIPAES
* @param [OUT]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @param [OUT]passphrase_len 8 ~ 64
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_get_psk_ex(uint8_t len, char *param[]);
/**
* @brief Example of Get PMK for the specified STA/AP port
* <br><b>
* wifi config get pmk <port>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [OUT]PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_get_pmk_ex(uint8_t len, char *param[]);
/**
* @brief Example of get WiFi WEP Keys
* <br><b>
* wifi config get wep <port>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [OUT]wifi_wep_key_t
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_wep_key_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set WiFi WEP Keys
* <br><b>
* wifi config set wep <port> <key_id> <key_string>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [IN]key_id 0~3
* @param [IN]key_string 0~26 (ASCII length: 5 or 13; Hex length:10 or 26)
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_wep_key_ex(uint8_t len, char *param[]);
/**
* @brief Example of Get WiFi Operation Mode.
* <br><b>
* wifi config get opmode
* </b></br>
* @param [OUT]mode
*        1: WIFI_MODE_STA_ONLY,
*        2: WIFI_MODE_AP_ONLY,
*        3: WIFI_MODE_REPEATER
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_opmode_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set WiFi Operation Mode.
* <br><b>
* wifi config set opmode <mode>
* </b></br>
* @param [IN]mode
*       1: WIFI_MODE_STA_ONLY,
*       2: WIFI_MODE_AP_ONLY,
*       3: WIFI_MODE_REPEATER
* @return  >=0 means success, <0 means fail
* @note Set WiFi Operation Mode will RESET all the configuration set by previous WIFI-CONFIG APIs
*/
uint8_t wifi_config_set_opmode_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi SSID.
* <br><b>
* wifi config get ssid <port>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [OUT]ssid SSID
* @param [OUT]ssid_len Length of SSID
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_ssid_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set WiFi SSID.
* <br><b>
* wifi config set ssid <port> <ssid>
* </b></br>
* @param [IN]port 0: STA / AP Client, 1: AP
* @param [IN]ssid SSID
* @param [IN]ssid_len Length of SSID
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_ssid_ex(uint8_t len, char *param[]);
/**
* @brief Example of Reload configuration
* <br><b>
* wifi config set reload
* </b></br>
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_reload_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get MTK Smart Connection Filter
* <br><b>
* wifi config get mtk_smart
* </b></br>
* @param [OUT]flag
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_mtk_smartconnection_filter_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set MTK Smart Connection Filter
* <br><b>
* wifi config set mtk_smart <flag>
* </b></br>
* @param [IN]flag  0: Disable, 1: Enable
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_mtk_smartconnection_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Radio ON/OFF
* <br><b>
* wifi config set radio <onoff>
* </b></br>
* @param [IN]onoff  0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off at the same time
*/
uint8_t wifi_config_set_radio_on_ex(uint8_t len, char *param[]);
/**
* @brief Example of get WiFi Radio ON/OFF
* <br><b>
* wifi config get radio
* </b></br>
* @param [OUT]onoff 0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off at the same time
*/
uint8_t wifi_config_get_radio_on_ex(uint8_t len, char *param[]);
/**
* @brief Example of Get WiFi Wireless Mode
* <br><b>
* wifi config get wirelessmode
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
uint8_t wifi_config_get_wireless_mode_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set WiFi Wireless Mode
* <br><b>
* wifi config set wirelessmode <mode>
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
uint8_t wifi_config_set_wireless_mode_ex(uint8_t len, char *param[]);
/**
* @brief Example of Get WiFi Country Region
* <br><b>
* wifi config get country <band>
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
uint8_t wifi_config_get_country_region_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set WiFi Country Region
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
uint8_t wifi_config_set_country_region_ex(uint8_t len, char *param[]);
/**
* @brief Example of Get WiFi DTIM Interval
* <br><b>
* wifi config get dtim
* </b></br>
* @param  [OUT]interval: 1~255
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_dtim_interval_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi DTIM Interval
* <br><b>
* wifi config set dtim <interval>
* </b></br>
* @param [IN]interval 1 ~ 255
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_dtim_interval_ex(uint8_t len, char *param[]);
/**
* @brief Example of Get WiFi Listen Interval
* <br><b>
* wifi config get listen
* </b></br>
* @param  [OUT]interval: 1~255
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_listen_interval_ex(uint8_t len, char *param[]);
/**
* @brief Example of Set WiFi Listen Interval
* <br><b>
* wifi config set listen <listen interval>
* </b></br>
* @param [IN]interval 1 ~ 255
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_listen_interval_ex(uint8_t len, char *param[]);



/* ---------------------- Connect Ex ----------------------------*/




/**
* @brief Example of  connect / link up to specifed AP
* <br><b>
* wifi connect set connection <linkup/down>
* </b></br>
* @param [IN] link  0: link down, 1: link up
* @return >=0 means success, <0 means fail
* @note Be sure to configure security setting before connect to AP
*/
uint8_t wifi_connect_set_connection_ex(uint8_t len, char *param[]);
/**
* @brief Example of Get the current STA port link up / link down status of the connection
* <br><b>
* wifi connect get linkstatus
* </b></br>
* @param [out] link  0: link down, 1: link up
* @return >=0 means success, <0 means fail
* @note WIFI_STATUS_LINK_DISCONNECTED indicates STA may in IDLE/ SCAN/ CONNECTING state
*/
uint8_t wifi_connect_get_link_status_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Associated Station List
* <br><b>
* wifi connect get stalist
* </b></br>
* @param [OUT]station_list
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_connect_get_station_list_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get the max number of the supported stations in ap mode or Repeater Mode.
* wifi connect get max_num_sta
* @param [OUT] number The max number of supported stations will be returned
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_connect_get_max_station_number_ex(uint8_t len, char *param[]);

/**
* @brief Example of deauth some WiFi connection
* <br><b>
* wifi connect deauth <MAC>
* </b></br>
* @param [IN]addr STA MAC Address
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_connect_deauth_station_ex(uint8_t len, char *param[]);
/**
* @brief Example of Get rssi of the connected AP
* <br><b>
* wifi connect get rssi
* </b></br>
* @param [OUT]rssi
* @return >=0 means success, the status will be:
*/
uint8_t wifi_connect_get_rssi_ex(uint8_t len, char *param[]);
/**
* @brief Example of Register WiFi Event Notifier
* <br><b>
* wifi connect set eventcb <enable> <event_id>
* </b></br>
* @param [IN]enable 0: unregister, 1: register
* @param [IN]event_id Event ID
*        LinkUP(0)
*        SCAN COMPLETE(1)
*        DISCONNECT(2)
*        PORT_SECURE(3)
*        REPORT_BEACON_PROBE_RESPONSE(4)
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_connect_set_event_callback_ex(uint8_t len, char *param[]);

uint8_t wifi_connect_set_scan_ex(uint8_t len, char *param[]);

/*WIFI CLI ENTRY definiation*/

#if defined(MTK_BSPEXT_ENABLE)
#define WIFI_PUB_CLI_ENTRY          { "wifi",    "wifi api",     NULL,   wifi_pub_cli },
#else
#define WIFI_PUB_CLI_ENTRY
#endif

#if defined(MTK_SMTCN_ENABLE)
#define WIFI_PUB_CLI_SMNT_ENTRY     { "smart",   "smart connection", _smart_config_test },
#else
#define WIFI_PUB_CLI_SMNT_ENTRY
#endif


/*extern function*/
#if defined(MTK_SMTCN_ENABLE)
extern uint8_t _smart_config_test(uint8_t len, char *param[]);
#endif
extern cmd_t   wifi_pub_cli[];


#ifdef __cplusplus
}
#endif

#endif /*  __WIFI_CLI_H__ */
