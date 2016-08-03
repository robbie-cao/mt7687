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

#ifdef MTK_WIFI_PROFILE_ENABLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os.h"
#include "os_util.h"
#include "cli.h"
#include "wifi_api.h"
#include "nvdm.h"
#include "connsys_profile.h"
#include "wifi_ex_config.h"
#include "get_profile_string.h"
#include "misc.h"
/**
    @file       wifi_ex_profile.c
    @brief      WiFi - Profile API Examples

    @history    2015/08/07  Initial for 1st draft  (Michael Rong)
 */

static uint8_t wifi_profile_set_opmode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t mode = atoi(param[0]);

    status = wifi_profile_set_opmode(mode);
    status = wifi_profile_commit_setting(WIFI_PROFILE_BUFFER_COMMON);

    printf("save opmode=%d done.\n", mode);

    return status;
}

/**
 * @brief Store channel to the profile in the Flash memory.
 * wifi profile set ch <port> <ch>
 * @param [IN]port
 * @param 0 STA / AP Client
 * @param 1 AP
 * @param [IN]channel    1~14 are supported for 2.4G only product.
 *
 * @return  >=0 means success, <0 means fail
 */
static uint8_t wifi_profile_set_channel_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t ch = atoi(param[1]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_channel((uint8_t)port, ch);

    status = wifi_profile_commit_setting(section);

    printf("[%s] save ch =%d done.\n", section, ch);

    return status;
}

/**
 * @brief Store bandwidth to the profile in the Flash memory.
 * wifi profile set bw <port> <bw>
 * @param [IN]port
 * @param 0 STA / AP Client
 * @param 1 AP
 * @param [IN]bandwidth Bandwidth IOT_CMD_CBW_20MHZ, IOT_CMD_CBW_40MHZ,
 *                  IOT_CMD_CBW_2040MHZ are supported.
 * @return  >=0 means success, <0 means fail
 */
static uint8_t wifi_profile_set_bandwidth_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t bw = atoi(param[1]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_bandwidth((uint8_t)port, bw);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save bw =%d done.\n", section, bw);

    return status;
}

/**
* @brief Store SSID to the profile in the Flash memory.
* wifi profile set ssid <port> <ssid>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]ssid SSID
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_ssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    char *ssid = param[1];
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_ssid((uint8_t)port, (uint8_t *)ssid, strlen(ssid));
    status = wifi_profile_commit_setting(section);

    printf("[%s] save ssid = %s done.\n", section, ssid);

    return status;
}
#if 0
/**
* @brief Store WiFi BSSID to the profile in the Flash memory.
* wifi profile set bssid <port> <bssid>
* ex: wifi profile set bssid 0 00:0c:43:76:87:02
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]bssid BSSID
*
* @return  >=0 means success, <0 means fail
*/

static uint8_t wifi_profile_set_bssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t port = atoi(param[0]);
    uint8_t bssid[6];

    __wifi_conf_get_mac_from_str((char *)bssid, param[1]);

    status = wifi_profile_set_bssid(port, bssid);

    printf("save bssid = %02x:%02x:%02x:%02x:%02x:%02x done.\n",
           bssid[0],
           bssid[1],
           bssid[2],
           bssid[3],
           bssid[4],
           bssid[5]);

    return status;
}
#endif

/**
* @brief Store WiFi Wireless Mode to the profile in the Flash memory.
* wifi profile set wirelessmode <port> <mode>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]mode
* @param 1 legacy 11B only
* @param 2 legacy 11A only
* @param 3 legacy 11A/B/G mixed
* @param 4 legacy 11G only
* @param 5 11ABGN mixed
* @param 6 11N only in 2.4G
* @param 7 11GN mixed
* @param 8 11AN mixed
* @param 9 11BGN mixed
* @param 10 11AGN mixed
*
* @return  >=0 means success, <0 means fail
*/

static uint8_t wifi_profile_set_wireless_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    wifi_phy_mode_t mode = (wifi_phy_mode_t)atoi(param[1]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_wireless_mode((uint8_t)port, mode);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save wireless mode =%d done.\n", section, mode);

    return status;
}
static uint8_t wifi_profile_set_country_region_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t band = atoi(param[0]);
    uint8_t region = atoi(param[1]);

    status = wifi_profile_set_country_region(band, region);
    status = wifi_profile_commit_setting(WIFI_PROFILE_BUFFER_COMMON);

    printf("save country band(%d) region =%d done.\n", band, region);

    return status;
}
static uint8_t wifi_profile_set_dtim_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dtim = atoi(param[0]);

    status = wifi_profile_set_dtim_interval(dtim);
    status = wifi_profile_commit_setting(WIFI_PROFILE_BUFFER_AP);

    printf("save dtim interval =%d done.\n", dtim);

    return status;
}
static uint8_t wifi_profile_set_listen_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t interval = atoi(param[0]);

    status = wifi_profile_set_listen_interval(interval);
    status = wifi_profile_commit_setting(WIFI_PROFILE_BUFFER_STA);

    printf("save listen interval =%d done.\n", interval);

    return status;
}
static uint8_t wifi_profile_set_mac_address_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t mac[6] = {0};

#ifdef MTK_WIFI_REPEATER_ENABLE
    uint8_t mode;
    char *section = WIFI_PROFILE_BUFFER_STA;

    status = wifi_profile_get_opmode(&mode);
    if (mode == WIFI_MODE_REPEATER) {
        strcpy(section,(port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP));
    } else {
        strcpy(section, WIFI_PROFILE_BUFFER_STA);
    }
    //char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);
#else
    /* Use STA MAC/IP as AP MAC/IP for the time being, due to N9 dual interface not ready yet */
    char *section = WIFI_PROFILE_BUFFER_STA;
#endif /* MTK_WIFI_REPEATER_ENABLE */

    if (port < 0) {
        return 1;
    }

    wifi_conf_get_mac_from_str((char *)mac, param[1]);

    status = wifi_profile_set_mac_address((uint8_t)port, mac);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save mac address = %02x:%02x:%02x:%02x:%02x:%02x done.\n",
           section,
           mac[0],
           mac[1],
           mac[2],
           mac[3],
           mac[4],
           mac[5]);
    return status;
}

static uint8_t wifi_profile_set_psk_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    wifi_auth_mode_t auth = (wifi_auth_mode_t)atoi(param[1]);
    wifi_encrypt_type_t encryp = (wifi_encrypt_type_t)atoi(param[2]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_security_mode((uint8_t)port, auth, encryp);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save auth mode=%d, encrypt type=%d done.\n", section, auth, encryp);

    return status;
}

static uint8_t wifi_profile_set_psk_key_ex(uint8_t len, char *param[])
{
    int8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    char *password = param[1];
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_wpa_psk_key((uint8_t)port, (uint8_t *)password, strlen(password));
    if (status < 0) {
        printf("[%s] ERROR! Fail to save password =%s len = %d.\n", section, password, strlen(password));
        return 1;
    }
    status = wifi_profile_commit_setting(section);

    printf("[%s] save password =%s len = %d done.\n", section, password, strlen(password));

    return 0;
}

/**
* @brief Example of Store PMK for the specified STA/AP port into the Flash memory
* wifi profile set pmk <port> <PMK>
* @param [IN]port
*       0 STA / AP Client
*       1 AP
* @param  [IN] PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
static uint8_t wifi_profile_set_pmk_ex(uint8_t len, char *param[])
{
    int8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);
    uint8_t hex[32] = {0};

    if (port < 0) {
        return 1;
    }

    os_memset(hex, 0, sizeof(hex));
    convert_string_to_hex_array(param[1], hex);
    status = wifi_profile_set_pmk((uint8_t)port, hex);
    if (status < 0) {
        printf("[%s] ERROR! Fail to save pmk.\n", section);
        return 1;
    }
    status = wifi_profile_commit_setting(section);

    printf("[%s] wifi_profile_set_pmk_ex: status:%d\n",
           section, status);

    return 0;

}

/**
* @brief Example of Get profile stored in NVRAM
*
* @param [IN]port
* @param 0 AP
* @param 1 AP Client
* @param 2 STA
* @param [OUT]profile WiFi Profile
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_init(sys_cfg_t *wifi_config);
uint8_t wifi_profile_get_profile_ex(uint8_t len, char *param[])
{
    sys_cfg_t wifi_profile_struct = {0};
    sys_cfg_t *wifi_profile = &wifi_profile_struct;
    uint8_t status = wifi_config_init(&wifi_profile_struct);

    // COMMON
    printf("[COMMON]\n");
    printf("\tOpMode = %d\n", wifi_profile->opmode);
    printf("\tCountryRegion = %d\n", wifi_profile->country_region);
    printf("\tCountryRegionABand = %d\n", wifi_profile->country_region_a_band);
    printf("\tCountryCode = %s\n", wifi_profile->country_code);
    printf("\tRadioOff = %d\n", wifi_profile->radio_off);
    printf("\tRTSThreshold = %d\n", wifi_profile->rts_threshold);
    printf("\tFragThreshold = %d\n", wifi_profile->frag_threshold);
    printf("\tDbgLevel = %d\n", wifi_profile->dbg_level);
    printf("\tForwardingZeroCopy = %d\n", wifi_profile->forwarding_zero_copy);
    // STA
    printf("[STA]\n");
    printf("\tLocalAdminMAC = %d\n", wifi_profile->sta_local_admin_mac);
    printf("\tIpAddr = %d.%d.%d.%d\n",
           wifi_profile->sta_ip_addr[0],
           wifi_profile->sta_ip_addr[1],
           wifi_profile->sta_ip_addr[2],
           wifi_profile->sta_ip_addr[3]);
    printf("\tMacAddr = %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_profile->sta_mac_addr[0],
           wifi_profile->sta_mac_addr[1],
           wifi_profile->sta_mac_addr[2],
           wifi_profile->sta_mac_addr[3],
           wifi_profile->sta_mac_addr[4],
           wifi_profile->sta_mac_addr[5]);
    printf("\tSsidLen = %d\n", wifi_profile->sta_ssid_len);
    printf("\tSsid = %s\n", wifi_profile->sta_ssid);
    printf("\tBssType = %d\n", wifi_profile->sta_bss_type);
    printf("\tChannel = %d\n", wifi_profile->sta_channel);
    printf("\tBW = %d\n", wifi_profile->sta_bw);
    printf("\tWirelessMode = %d\n", wifi_profile->sta_wireless_mode);
    printf("\tBADecline = %d\n", wifi_profile->sta_ba_decline);
    printf("\tAutoBA = %d\n", wifi_profile->sta_auto_ba);
    printf("\tHT_MCS = %d\n", wifi_profile->sta_ht_mcs);
    printf("\tHT_BAWinSize = %d\n", wifi_profile->sta_ht_ba_win_size);
    printf("\tHT_GI = %d\n", wifi_profile->sta_ht_gi);
    printf("\tHT_PROTECT = %d\n", wifi_profile->sta_ht_protect);
    printf("\tHT_EXTCHA = %d\n", wifi_profile->sta_ht_ext_ch);
    printf("\tWmmCapable = %d\n", wifi_profile->sta_wmm_capable);
    printf("\tListenInterval = %d\n", wifi_profile->sta_listen_interval);
    printf("\tAuthMode = %d\n", wifi_profile->sta_auth_mode);
    printf("\tEncrypType = %d\n", wifi_profile->sta_encryp_type);
    printf("\tWpaPskLen = %d\n", wifi_profile->sta_wpa_psk_len);
    printf("\tPairCipher = %d\n", wifi_profile->sta_pair_cipher);
    printf("\tGroupCipher = %d\n", wifi_profile->sta_group_cipher);
    printf("\tDefaultKeyId = %d\n", wifi_profile->sta_default_key_id);
    printf("\tPSMode = %d\n", wifi_profile->sta_ps_mode);
    printf("\tKeepAlivePeriod = %d\n", wifi_profile->sta_keep_alive_period);

    hex_dump("WpaPsk", wifi_profile->sta_wpa_psk, sizeof(wifi_profile->sta_wpa_psk));
    hex_dump("PMK", wifi_profile->sta_pmk, sizeof(wifi_profile->sta_pmk));

    // AP
    printf("[AP]\n");
    printf("\tLocalAdminMAC = %d\n", wifi_profile->ap_local_admin_mac);
    printf("\tIpAddr = %d.%d.%d.%d\n",
           wifi_profile->ap_ip_addr[0],
           wifi_profile->ap_ip_addr[1],
           wifi_profile->ap_ip_addr[2],
           wifi_profile->ap_ip_addr[3]);
    printf("\tMacAddr = %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_profile->ap_mac_addr[0],
           wifi_profile->ap_mac_addr[1],
           wifi_profile->ap_mac_addr[2],
           wifi_profile->ap_mac_addr[3],
           wifi_profile->ap_mac_addr[4],
           wifi_profile->ap_mac_addr[5]);
    printf("\tSsidLen = %d\n", wifi_profile->ap_ssid_len);
    printf("\tSsid = %s\n", wifi_profile->ap_ssid);
    printf("\tChannel = %d\n", wifi_profile->ap_channel);
    printf("\tBW = %d\n", wifi_profile->ap_bw);
    printf("\tWirelessMode = %d\n", wifi_profile->ap_wireless_mode);
    printf("\tAutoBA = %d\n", wifi_profile->ap_auto_ba);
    printf("\tHT_MCS = %d\n", wifi_profile->ap_ht_mcs);
    printf("\tHT_BAWinSize = %d\n", wifi_profile->ap_ht_ba_win_size);
    printf("\tHT_GI = %d\n", wifi_profile->ap_ht_gi);
    printf("\tHT_PROTECT = %d\n", wifi_profile->ap_ht_protect);
    printf("\tHT_EXTCHA = %d\n", wifi_profile->ap_ht_ext_ch);
    printf("\tWmmCapable = %d\n", wifi_profile->ap_wmm_capable);
    printf("\tDtimPeriod = %d\n", wifi_profile->ap_dtim_period);
    printf("\tHideSSID = %d\n", wifi_profile->ap_hide_ssid);
    printf("\tAutoChannelSelect = %d\n", wifi_profile->ap_auto_channel_select);
    printf("\tAuthMode = %d\n", wifi_profile->ap_auth_mode);
    printf("\tEncrypType = %d\n", wifi_profile->ap_encryp_type);
    printf("\tWpaPskLen = %d\n", wifi_profile->ap_wpa_psk_len);
    printf("\tPairCipher = %d\n", wifi_profile->ap_pair_cipher);
    printf("\tGroupCipher = %d\n", wifi_profile->ap_group_cipher);
    printf("\tDefaultKeyId = %d\n", wifi_profile->ap_default_key_id);

    hex_dump("WpaPsk", wifi_profile->ap_wpa_psk, sizeof(wifi_profile->ap_wpa_psk));
    hex_dump("PMK", wifi_profile->ap_pmk, sizeof(wifi_profile->ap_pmk));

#if 0 // not ready
    // scan channel table and regulatory table
    printf("[COMMON-misc.]\n");

    printf("\tBGChannelTable:\n");
    for (i = 0; i < 10; i++)
        printf("\t\t{%d,%d,%d}\n",
               wifi_profile->bg_band_triple[i].first_channel,
               wifi_profile->bg_band_triple[i].num_of_ch,
               wifi_profile->bg_band_triple[i].channel_prop);

    printf("\tAChannelTable:\n");
    for (i = 0; i < 10; i++)
        printf("\t\t{%d,%d,%d}\n",
               wifi_profile->a_band_triple[i].first_channel,
               wifi_profile->a_band_triple[i].num_of_ch,
               wifi_profile->a_band_triple[i].channel_prop);
#endif
    return status;
}

/**
* @brief Example of Get PMK for the specified STA/AP port from NVRAM
* wifi profile get pmk <port>
* @param [IN]port
*       0 STA / AP Client
*       1 AP
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
static uint8_t wifi_profile_get_pmk_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    uint8_t hex[32] = {0};
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    os_memset(hex, 0, sizeof(hex));
    status = wifi_profile_get_pmk(port, hex);
    printf("wifi_profile_get_pmk_ex: port:%ld, status:%d, key dump:\n",
           port, status);
    for (i = 0; i < sizeof(hex); i++) {
        if (i % 16 == 0) {
            printf("\n\t");
        }
        printf("%02x ", (unsigned int)hex[i]);
    }

    return status;

}



/**
* @brief Store WiFi WEP Keys to the profile in NVRAM.
* wifi profile set wep <port> <key id> <key>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]wifi_wep_key_t
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_wep_key_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t index = 0;
    wifi_wep_key_t wep_key;
    char *ptr = NULL;
    int32_t port = port_sanity_check(param[0]);
    char *keys = param[2];
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    os_memset(&wep_key, 0, sizeof(wep_key));
    wep_key.wep_tx_key_index = atoi(param[1]);

    index = 0;
    for (ptr = rstrtok((char *)keys, ","); (ptr); ptr = rstrtok(NULL, ",")) {
        printf("[%d] key=%s\n", index, ptr);
        if (strlen(ptr) == 1 || strlen(ptr) == 5 || strlen(ptr) == 13 || strlen(ptr) == 10 || strlen(ptr) == 26) {
            os_memcpy(wep_key.wep_key[index], ptr, strlen(ptr));
            wep_key.wep_key_length[index] = strlen(ptr);
        } else {
            printf("invalid length of value.\n");
        }
        index++;
        if (index >= WIFI_NUMBER_WEP_KEYS) {
            break;
        }
    }


    status = wifi_profile_set_wep_key((uint8_t)port, &wep_key);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save wep key =(%s, %s, %s, %s) key id=%d, len=(%d, %d, %d, %d) done.\n",
           section,
           wep_key.wep_key[0],
           wep_key.wep_key[1],
           wep_key.wep_key[2],
           wep_key.wep_key[3],
           wep_key.wep_tx_key_index,
           wep_key.wep_key_length[0],
           wep_key.wep_key_length[1],
           wep_key.wep_key_length[2],
           wep_key.wep_key_length[3]);

    return status;
}
#if 0
/**
 * @brief Configure packet format wanted to be received
 * wifi profile set rxfilter <flag>
 * @param flag [IN] flag defined in iot_rx_filter_t.
 * @return >=0 means success, <0 means fail
 * @note Default value will be WIFI_DEFAULT_IOT_RX_FILTER
 */
static uint8_t wifi_profile_set_rx_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint32_t flag = atoi(param[0]);

    status = wifi_profile_set_rx_filter(flag);

    printf("save RX filter = 0x%x done.\n", (unsigned int)flag);

    return status;
}


static uint8_t wifi_profile_set_smart_connection_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t flag = atoi(param[0]);

    status = wifi_profile_set_smart_connection_filter(flag);

    printf("save MTK RX filter =0x%x done.\n", flag);

    return status;
}
#endif
static uint8_t wifi_profile_get_opmode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t mode;

    status = wifi_profile_get_opmode(&mode);

    printf("fetch opmode=%d done.\n", mode);

    return status;
}

/**
 * @brief Get channel from the profile in NVRAM.
 * wifi profile get ch <port>
 * @param [IN]port
 * @param 0 STA / AP Client
 * @param 1 AP
 * @param [OUT]channel    1~14 are supported for 2.4G only product.
 *
 * @return  >=0 means success, <0 means fail
 */

static uint8_t wifi_profile_get_channel_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t ch;

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_channel((uint8_t)port, &ch);

    printf("fetch ch =%d done.\n", ch);

    return status;
}

/**
 * @brief  Get bandwidth from the profile in NVRAM.
 * wifi profile get bw <port>
 * @param [IN]port
 * @param 0 STA / AP Client
 * @param 1 AP
 * @param [OUT]bandwidth The wirelss bandwidth.
 *                       IOT_CMD_CBW_20MHZ,
 *                       IOT_CMD_CBW_40MHZ, and
 *                       IOT_CMD_CBW_2040MHZ are supported.
 *
 * @return  >=0 means success, <0 means fail
 *
 * @note    Default value is HT_20
 */

static uint8_t wifi_profile_get_bandwidth_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t bw;

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_bandwidth((uint8_t)port, &bw);

    printf("fetch bw =%d done.\n", bw);

    return status;
}

/**
* @brief Get SSID from the profile in NVRAM.
* wifi profile get ssid <port>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [OUT]ssid SSID
* @param [OUT]ssid_length Length of SSID
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_ssid_ex(uint8_t length, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t ssid[32] = {0};
    uint8_t len;

    if (port < 0) {
        return 1;
    }

    os_memset(ssid, 0x0, sizeof(ssid));
    status = wifi_profile_get_ssid((uint8_t)port, ssid, &len);

    printf("fetch ssid = %s, len=%d done.\n", ssid, len);

    return status;
}
#if 0
/**
* @brief Store WiFi BSSID to the profile in NVRAM.
* wifi profile get bssid <port>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]bssid BSSID
*
* @return  >=0 means success, <0 means fail
*/

static uint8_t wifi_profile_get_bssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t port = atoi(param[0]);
    uint8_t bssid[6];

    status = wifi_profile_set_bssid(port, bssid);

    printf("fetch bssid = %02x:%02x:%02x:%02x:%02x:%02x done.\n",
           bssid[0],
           bssid[1],
           bssid[2],
           bssid[3],
           bssid[4],
           bssid[5]);

    return status;
}
#endif

/**
* @brief Get WiFi Wireless Mode from the profile in NVRAM.
* wifi profile get wirelessmode <port>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [OUT]mode
* @param 1 legacy 11B only
* @param 2 legacy 11A only
* @param 3 legacy 11A/B/G mixed
* @param 4 legacy 11G only
* @param 5 11ABGN mixed
* @param 6 11N only in 2.4G
* @param 7 11GN mixed
* @param 8 11AN mixed
* @param 9 11BGN mixed
* @param 10 11AGN mixed
*/

static uint8_t wifi_profile_get_wireless_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t mode;

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_wireless_mode((uint8_t)port, (wifi_phy_mode_t *)&mode);

    printf("fetch wireless mode =%d done.\n", mode);

    return status;
}


static uint8_t wifi_profile_get_country_region_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t band = atoi(param[0]);
    uint8_t region;

    status = wifi_profile_get_country_region(band, &region);

    printf("fetch band(%d) region =%d done.\n", band, region);

    return status;
}

static uint8_t wifi_profile_get_dtim_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dtim;

    status = wifi_profile_get_dtim_interval(&dtim);

    printf("fetch dtim = %d done.\n", dtim);

    return status;
}
static uint8_t wifi_profile_get_listen_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t interval;

    status = wifi_profile_get_listen_interval(&interval);

    printf("fetch listen interval = %d done.\n", interval);

    return status;
}
static uint8_t wifi_profile_get_mac_address_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t mac[6] = {0};

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_mac_address((uint8_t)port, mac);

    printf("fetch mac address = %02x:%02x:%02x:%02x:%02x:%02x done.\n",
           mac[0],
           mac[1],
           mac[2],
           mac[3],
           mac[4],
           mac[5]);
    return status;

}
static uint8_t wifi_profile_get_psk_mode_ex(uint8_t length, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t auth;
    uint8_t encryp;

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_security_mode((uint8_t)port, (wifi_auth_mode_t *)&auth, (wifi_encrypt_type_t *)&encryp);

    printf("fetch auth mode=%d, encrypt type=%d done.\n", auth, encryp);

    return status;
}

static uint8_t wifi_profile_get_psk_key_ex(uint8_t length, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t password[65] = {0};
    uint8_t len;

    if (port < 0) {
        return 1;
    }

    os_memset(password, 0, sizeof(password));
    status = wifi_profile_get_wpa_psk_key((uint8_t)port, password, &len);

    printf("fetch password =%s len=%d done.\n", password, len);

    return status;
}

/**
* @brief Get WiFi WEP Keys from the profile in NVRAM.
* wifi profile get wep <port>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [OUT]wifi_wep_key_t
*
* @return >=0 means success, <0 means fail
*/

static uint8_t wifi_profile_get_wep_key_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    wifi_wep_key_t wep_key;
    int index, i;

    if (port < 0) {
        return 1;
    }

    os_memset(&wep_key, 0, sizeof(wep_key));
    status = wifi_profile_get_wep_key((uint8_t)port, &wep_key);

    wep_key.wep_key[0][wep_key.wep_key_length[0]] = '\0';
    wep_key.wep_key[1][wep_key.wep_key_length[1]] = '\0';
    wep_key.wep_key[2][wep_key.wep_key_length[2]] = '\0';
    wep_key.wep_key[3][wep_key.wep_key_length[3]] = '\0';

    printf("fetch wep key id =%d, len = (%d, %d, %d, %d) done.\n",
           wep_key.wep_tx_key_index,
           wep_key.wep_key_length[0],
           wep_key.wep_key_length[1],
           wep_key.wep_key_length[2],
           wep_key.wep_key_length[3]);
    for (index = 0; index < WIFI_NUMBER_WEP_KEYS; index++) {
        printf("[%d]: ", index);
        for (i = 0; i < wep_key.wep_key_length[index]; i++) {
            printf("%02x", wep_key.wep_key[index][i]);
        }
        printf("\n");
    }
    return status;
}

/**
* @brief Save N9 Debug Level into NVRAM
* wifi profile set n9dbg <dbg_level>
* @param [IN]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_n9_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dbg_level = atoi(param[0]);

    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%d", dbg_level);
    if (nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "DbgLevel",NVDM_DATA_ITEM_TYPE_STRING, (const uint8_t *)buf, os_strlen(buf)) != NVDM_STATUS_OK) {
        printf("write NVDM DbgLevel fail\n");
        status = 1;
    }


    printf("wifi_profile_set_n9_dbg_level, level = %d, status:%d\n", dbg_level, status);

    return status;
}


/**
* @brief Save CM4 Debug Level into NVRAM
* wifi profile set cm4dbg <dbg_level>
* @param [IN]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_cm4_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dbg_level = atoi(param[0]);

    // fixme, try to leverage mlog module
    printf("[Not Ready] wifi_profile_set_cm4_dbg_level, level = %d, status:%d\n", dbg_level, status);

    return status;
}

/**
* @brief Get N9 Debug Level from NVRAM
* wifi profile get n9dbg
* @param [OUT]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_n9_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dbg_level = 0;

    // init wifi profile
    char buff[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    int buff_sz = sizeof(buff);
    // common
    nvdm_read_data_item("common", "DbgLevel", (uint8_t *)buff, (uint32_t *)&buff_sz);
    dbg_level = atoi(buff);

    printf("wifi_profile_get_n9_dbg_level, level = %d, status:%d\n", dbg_level, status);

    return status;
}

void user_data_item_reset_to_default(char *group_name);
uint8_t wifi_profile_reset_ex(uint8_t len, char *param[])
{
    user_data_item_reset_to_default(NULL);
    return 0;
}


/**
* @brief Get CM4 Debug Level from NVRAM
* wifi profile get cm4dbg
* @param [OUT]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_cm4_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dbg_level = 0;

    // fixme, try to leverage mlog module
    printf("[Not Ready] wifi_profile_get_cm4_dbg_level, level = %d, status:%d\n", dbg_level, status);

    return status;
}

cmd_t   wifi_profile_set_cli[] = {
    { "opmode",         "STA/AP/Dual",      wifi_profile_set_opmode_ex},
    { "ch",             "channel",          wifi_profile_set_channel_ex},
    { "bw",             "bandwidth",        wifi_profile_set_bandwidth_ex},
    { "ssid",           "SSID",             wifi_profile_set_ssid_ex      },
//  { "bssid",          "BSSID",            wifi_profile_set_bssid_ex     },
    { "wirelessmode",   "wireless mode",    wifi_profile_set_wireless_mode_ex},
    { "country",        "country region",   wifi_profile_set_country_region_ex},
    { "dtim",           "DTIM interval",    wifi_profile_set_dtim_interval_ex},
    { "listen",         "listen interval",  wifi_profile_set_listen_interval_ex},
    { "mac",            "MAC address",      wifi_profile_set_mac_address_ex},
    { "pskmode",        "WPA/WPA2PSK Authmode, Encrypt Type",   wifi_profile_set_psk_mode_ex   },
    { "pskkey",         "WPA/WPA2PSK Key",  wifi_profile_set_psk_key_ex   },
    { "pmk",            "PMK for WPA/WPA2PSK",  wifi_profile_set_pmk_ex   },
    { "wep",            "WEP key",          wifi_profile_set_wep_key_ex },
    { "n9dbg",          "set N9 debug level",   wifi_profile_set_n9_dbg_level},
    { "cm4dbg",         "set CM4 debug level",  wifi_profile_set_cm4_dbg_level},
    { NULL }
};

cmd_t   wifi_profile_get_cli[] = {
    { "opmode",         "STA/AP/Dual",      wifi_profile_get_opmode_ex},
    { "ch",             "channel",          wifi_profile_get_channel_ex},
    { "bw",             "bandwidth",        wifi_profile_get_bandwidth_ex},
    { "ssid",           "SSID",             wifi_profile_get_ssid_ex      },
//  { "bssid",          "BSSID",            wifi_profile_get_bssid_ex     },
    { "wirelessmode",   "wireless mode",    wifi_profile_get_wireless_mode_ex},
    { "country",        "country region",   wifi_profile_get_country_region_ex},
    { "dtim",           "DTIM interval",    wifi_profile_get_dtim_interval_ex},
    { "listen",         "listen interval",  wifi_profile_get_listen_interval_ex},
    { "mac",            "MAC address",      wifi_profile_get_mac_address_ex},
    { "pskmode",        "WPA/WPA2PSK Authmode, Encrypt Type",   wifi_profile_get_psk_mode_ex   },
    { "pskkey",         "WPA/WPA2PSK Key",  wifi_profile_get_psk_key_ex  },
    { "pmk",            "PMK for WPA/WPA2PSK",  wifi_profile_get_pmk_ex   },
    { "wep",            "WEP key",          wifi_profile_get_wep_key_ex },
    { "profile",            "get profile txt",      wifi_profile_get_profile_ex},
    { "n9dbg",          "get N9 debug level",   wifi_profile_get_n9_dbg_level},
    { "cm4dbg",         "get CM4 debug level",  wifi_profile_get_cm4_dbg_level},
    { NULL }
};

#endif //MTK_WIFI_PROFILE_ENABLE
