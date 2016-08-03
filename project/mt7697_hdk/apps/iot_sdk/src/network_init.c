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

#include <string.h>
#include "lwip/sockets.h"
#include "nvdm.h"
#include "connsys_profile.h"
#include "connsys_util.h"
#include "get_profile_string.h"
#include "network_init.h"
#include "type_def.h"
#include "syslog.h"
#include "wpa_supplicant_task.h"
#include "wifi_init.h"
#include "ethernetif.h"
#include "dhcpd.h"
#include "dhcp.h"
#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
#include <inband_queue.h>
#include "wifi_scan.h"
#endif
#include "wifi_inband.h"


#ifdef __ICCARM__
#define STRCPY strncpy
#else
#define STRCPY strlcpy
#endif

static wifi_ip_ready_callback_t g_ip_ready_callback = NULL;

typedef struct {
    sys_cfg_t wifi_config;
    tcpip_config_t tcpip_config;
    dhcpd_settings_t dhcpd_settings;
} sys_config_t;

BSS_IN_TCM static sys_config_t g_sys_config = {{0}};

/**
* @brief      This function registers a callback and this callback will be called on the moment a valid IP address is got after wifi connection is done
* @param[in]  callback  is the target function to be called on right moment
* @warning    This callback function runs in lwIP task, therefore, calling lwIP socket APIs in this callback function will cause deadlock.
*             Please create a new task and invoke lwIP socket APIs in it if necessary.
*/
void wifi_register_ip_ready_callback(wifi_ip_ready_callback_t callback)
{
    g_ip_ready_callback = callback;
}

wifi_phy_mode_t wifi_change_wireless_mode_5g_to_2g(wifi_phy_mode_t wirelessmode)
{
    if (WIFI_PHY_11A == wirelessmode) {
        return WIFI_PHY_11B;
    } else if (WIFI_PHY_11ABG_MIXED == wirelessmode) {
        return WIFI_PHY_11BG_MIXED;
    } else if (WIFI_PHY_11ABGN_MIXED == wirelessmode) {
        return WIFI_PHY_11BGN_MIXED;
    } else if (WIFI_PHY_11AN_MIXED == wirelessmode) {
        return WIFI_PHY_11N_2_4G;
    } else if (WIFI_PHY_11AGN_MIXED == wirelessmode) {
        return WIFI_PHY_11GN_MIXED;
    } else if (WIFI_PHY_11N_5G == wirelessmode) {
        return WIFI_PHY_11N_2_4G;
    } else {
        return wirelessmode;
    }
}

static void save_wep_key_length(uint8_t *length, char *wep_key_len, uint8_t key_id)
{
    uint8_t id = 0;
    uint8_t index = 0;

    do {
        if ('\0' == wep_key_len[index]) {
            LOG_E(wifi, "index not found");
            return;
        }
        if (key_id == id) {
            *length = (uint8_t)atoi(&wep_key_len[index]);
            return;
        }
        if (',' == wep_key_len[index++]) {
            id++;
        }
    } while (id < 4);
    LOG_E(wifi, "index not found: %d", key_id);
}

static void save_shared_key(uint8_t *wep_key, uint8_t *raw_wep_key, uint8_t length, uint8_t key_id)
{
    uint8_t id = 0;
    uint8_t index = 0;

    do {
        if ('\0' == raw_wep_key[index]) {
            LOG_E(wifi, "index not found");
            return;
        }
        if (key_id == id) {
            memcpy(wep_key, &raw_wep_key[index], length);
            wep_key[length] = '\0';
            LOG_E(wifi, "obtained wep key: %s", wep_key);
            return;
        }
        if (',' == raw_wep_key[index++]) {
            id++;
        }
    } while (id < 4);
    LOG_E(wifi, "index not found: %d", key_id);
}

int32_t wifi_config_init(sys_cfg_t *wifi_config)
{
#ifdef MTK_WIFI_PROFILE_ENABLE

    // init wifi profile
    uint8_t buff[PROFILE_BUF_LEN];
    uint32_t len = sizeof(buff);

    // common
    len = sizeof(buff);
    nvdm_read_data_item("common", "OpMode", buff, &len);
    wifi_config->opmode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryRegion", buff, &len);
    wifi_config->country_region = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryRegionABand", buff, &len);
    wifi_config->country_region_a_band = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryCode", buff, &len);
    memcpy(wifi_config->country_code, buff, 4);
    len = sizeof(buff);
    nvdm_read_data_item("common", "RadioOff", buff, &len);
    wifi_config->radio_off = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "RTSThreshold", buff, &len);
    wifi_config->rts_threshold = (uint16_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "FragThreshold", buff, &len);
    wifi_config->frag_threshold = (uint16_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "DbgLevel", buff, &len);
    wifi_config->dbg_level = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "IpAddr", buff, &len);
    wifi_conf_get_ip_from_str(wifi_config->ap_ip_addr, (char *)buff);
    wifi_conf_get_ip_from_str(wifi_config->sta_ip_addr, (char *)buff);

    // STA
    len = sizeof(buff);
    nvdm_read_data_item("STA", "LocalAdminMAC", buff, &len);
    wifi_config->sta_local_admin_mac = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "MacAddr", buff, &len);
    wifi_conf_get_mac_from_str((char *)wifi_config->sta_mac_addr, (char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "SsidLen", buff, &len);
    wifi_config->sta_ssid_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "Ssid", buff, &len);
    memcpy(wifi_config->sta_ssid, buff, wifi_config->sta_ssid_len);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BssType", buff, &len);
    wifi_config->sta_bss_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "Channel", buff, &len);
    wifi_config->sta_channel = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BW", buff, &len);
    wifi_config->sta_bw = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WirelessMode", buff, &len);
    if (wifi_5g_support() < 0) {
        wifi_config->sta_wireless_mode = (uint8_t)wifi_change_wireless_mode_5g_to_2g((wifi_phy_mode_t)atoi((char *)buff));
    }else {
        wifi_config->sta_wireless_mode = (uint8_t)atoi((char *)buff);
    }
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BADecline", buff, &len);
    wifi_config->sta_ba_decline = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "AutoBA", buff, &len);
    wifi_config->sta_auto_ba = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_MCS", buff, &len);
    wifi_config->sta_ht_mcs = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_BAWinSize", buff, &len);
    wifi_config->sta_ht_ba_win_size = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_GI", buff, &len);
    wifi_config->sta_ht_gi = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_PROTECT", buff, &len);
    wifi_config->sta_ht_protect = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_EXTCHA", buff, &len);
    wifi_config->sta_ht_ext_ch = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WmmCapable", buff, &len);
    wifi_config->sta_wmm_capable = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "ListenInterval", buff, &len);
    wifi_config->sta_listen_interval = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "AuthMode", buff, &len);
    wifi_config->sta_auth_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "EncrypType", buff, &len);
    wifi_config->sta_encryp_type = (uint8_t)atoi((char *)buff);

    if (WIFI_ENCRYPT_TYPE_WEP_ENABLED == wifi_config->sta_encryp_type) {
        len = sizeof(buff);
        nvdm_read_data_item("STA", "DefaultKeyId", buff, &len);
        wifi_config->sta_default_key_id = (uint8_t)atoi((char *)buff);

        len = sizeof(buff);
        nvdm_read_data_item("STA", "SharedKeyLen", buff, &len);
        save_wep_key_length(&wifi_config->sta_wpa_psk_len, (char *)buff, wifi_config->sta_default_key_id);

        len = sizeof(buff);
        nvdm_read_data_item("STA", "SharedKey", buff, &len);
        save_shared_key(wifi_config->sta_wpa_psk, buff, wifi_config->sta_wpa_psk_len, wifi_config->sta_default_key_id);
    } else {
        len = sizeof(buff);
        nvdm_read_data_item("STA", "WpaPskLen", buff, &len);
        wifi_config->sta_wpa_psk_len = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "WpaPsk", buff, &len);
        memcpy(wifi_config->sta_wpa_psk, buff, wifi_config->sta_wpa_psk_len);
    }

    len = sizeof(buff);
    nvdm_read_data_item("STA", "PMK", buff, &len);
    memcpy(wifi_config->sta_pmk, buff, 32); // TODO: How to save binary PMK value not ending by ' ; ' ?
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PairCipher", buff, &len);
    wifi_config->sta_pair_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "GroupCipher", buff, &len);
    wifi_config->sta_group_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PSMode", buff, &len);
    wifi_config->sta_ps_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "KeepAlivePeriod", buff, &len);
    wifi_config->sta_keep_alive_period = (uint8_t)atoi((char *)buff);

    // AP
#ifdef MTK_WIFI_REPEATER_ENABLE
    if (wifi_config->opmode == WIFI_MODE_REPEATER) {
        len = sizeof(buff);
        nvdm_read_data_item("STA", "Channel", buff, &len);
        wifi_config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "BW", buff, &len);
        wifi_config->ap_bw = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "WirelessMode", buff, &len);
        wifi_config->ap_wireless_mode = (uint8_t)atoi((char *)buff);
    } else {
#endif
        /* Use STA MAC/IP as AP MAC/IP for the time being, due to N9 dual interface not ready yet */
        len = sizeof(buff);
        nvdm_read_data_item("AP", "Channel", buff, &len);
        wifi_config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "BW", buff, &len);
        wifi_config->ap_bw = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "WirelessMode", buff, &len);
        if (wifi_5g_support() < 0) {
            wifi_config->ap_wireless_mode = (uint8_t)wifi_change_wireless_mode_5g_to_2g((wifi_phy_mode_t)atoi((char *)buff));
        }else {
            wifi_config->ap_wireless_mode = (uint8_t)atoi((char *)buff);
        }
#ifdef MTK_WIFI_REPEATER_ENABLE
    }
#endif /* MTK_WIFI_REPEATER_ENABLE */
    len = sizeof(buff);
    nvdm_read_data_item("AP", "LocalAdminMAC", buff, &len);
    wifi_config->ap_local_admin_mac = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "MacAddr", buff, &len);
    wifi_conf_get_mac_from_str((char *)wifi_config->ap_mac_addr, (char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "SsidLen", buff, &len);
    wifi_config->ap_ssid_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "Ssid", buff, &len);
    memcpy(wifi_config->ap_ssid, buff, wifi_config->ap_ssid_len);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AutoBA", buff, &len);
    wifi_config->ap_auto_ba = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_MCS", buff, &len);
    wifi_config->ap_ht_mcs = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_BAWinSize", buff, &len);
    wifi_config->ap_ht_ba_win_size = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_GI", buff, &len);
    wifi_config->ap_ht_gi = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_PROTECT", buff, &len);
    wifi_config->ap_ht_protect = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_EXTCHA", buff, &len);
    wifi_config->ap_ht_ext_ch = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WmmCapable", buff, &len);
    wifi_config->ap_wmm_capable = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "DtimPeriod", buff, &len);
    wifi_config->ap_dtim_period = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HideSSID", buff, &len);
    wifi_config->ap_hide_ssid = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AutoChannelSelect", buff, &len);
    wifi_config->ap_auto_channel_select = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AuthMode", buff, &len);
    wifi_config->ap_auth_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "EncrypType", buff, &len);
    wifi_config->ap_encryp_type = (uint8_t)atoi((char *)buff);

    if (WIFI_ENCRYPT_TYPE_WEP_ENABLED == wifi_config->ap_encryp_type) {
        len = sizeof(buff);
        nvdm_read_data_item("AP", "DefaultKeyId", buff, &len);
        wifi_config->ap_default_key_id = (uint8_t)atoi((char *)buff);

        len = sizeof(buff);
        nvdm_read_data_item("AP", "SharedKeyLen", buff, &len);
        save_wep_key_length(&wifi_config->ap_wpa_psk_len, (char *)buff, wifi_config->ap_default_key_id);

        len = sizeof(buff);
        nvdm_read_data_item("AP", "SharedKey", buff, &len);
        save_shared_key(wifi_config->ap_wpa_psk, buff, wifi_config->ap_wpa_psk_len, wifi_config->ap_default_key_id);
    } else {
        len = sizeof(buff);
        nvdm_read_data_item("AP", "WpaPskLen", buff, &len);
        wifi_config->ap_wpa_psk_len = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "WpaPsk", buff, &len);
        memcpy(wifi_config->ap_wpa_psk, buff, wifi_config->ap_wpa_psk_len);
    }

    len = sizeof(buff);
    nvdm_read_data_item("AP", "PMK", buff, &len);
    memcpy(wifi_config->ap_pmk, buff, 32); // TODO: How to save binary PMK value not ending by ' ; ' ?
    len = sizeof(buff);
    nvdm_read_data_item("AP", "PairCipher", buff, &len);
    wifi_config->ap_pair_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "GroupCipher", buff, &len);
    wifi_config->ap_group_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "BcnDisEn", buff, &len);
    wifi_config->ap_beacon_disable = (uint8_t)atoi((char *)buff);

    // scan channel table and regulatory table
    len = sizeof(buff);
    nvdm_read_data_item("common", "BGChannelTable", buff, &len);
    wifi_config->bg_band_entry_num = wifi_conf_get_ch_table_from_str(wifi_config->bg_band_triple, 10, (char *)buff, strlen((char *)buff));

    len = sizeof(buff);
    nvdm_read_data_item("common", "AChannelTable", buff, &len);
    wifi_config->a_band_entry_num = wifi_conf_get_ch_table_from_str(wifi_config->a_band_triple, 10, (char *)buff, strlen((char *)buff));

    wifi_config->forwarding_zero_copy = 1;

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
    /* These are for MBSS support, but not exist trunk (it's customer feature), however,
            we have to add them here due to N9 FW has them (only one version of N9 FW)
         */
    // TODO: How to solve it in the future...Michael
    wifi_config->mbss_enable = 0;
    memset(wifi_config->mbss_ssid1, 0x0, sizeof(wifi_config->mbss_ssid1));;
    wifi_config->mbss_ssid_len1 = 0;
    memset(wifi_config->mbss_ssid2, 0x0, sizeof(wifi_config->mbss_ssid2));;
    wifi_config->mbss_ssid_len2 = 0;

    len = sizeof(buff);
    nvdm_read_data_item("common", "ConfigFree_Ready", buff, &len);
    wifi_config->config_free_ready = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "ConfigFree_Enable", buff, &len);
    wifi_config->config_free_enable = (uint8_t)atoi((char *)buff);
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */
    len = sizeof(buff);
    nvdm_read_data_item("common", "StaFastLink", buff, &len);
    wifi_config->sta_fast_link = (uint8_t)atoi((char *)buff);
#else
    //wifi profile is disabled, take the user

#endif
    return 0;
}

static int32_t tcpip_config_init(tcpip_config_t *tcpip_config)
{
    uint8_t ip_addr[4] = {0};
    uint8_t buff[PROFILE_BUF_LEN] = {0};
    uint32_t sz = sizeof(buff);

    nvdm_read_data_item("STA", "IpMode", buff, &sz);
    tcpip_config->sta_ip_mode = strcmp((char *)buff, "dhcp") ? STA_IP_MODE_STATIC : STA_IP_MODE_DHCP;
    sz = sizeof(buff);
    nvdm_read_data_item("STA", "MacAddr", buff, &sz);
    wifi_conf_get_mac_from_str((char *)tcpip_config->sta_mac_addr, (char *)buff);
    sz = sizeof(buff);
    nvdm_read_data_item("AP", "MacAddr", buff, &sz);
    wifi_conf_get_mac_from_str((char *)tcpip_config->ap_mac_addr, (char *)buff);

    sz = sizeof(buff);
    nvdm_read_data_item("common", "IpAddr", buff, &sz);
    wifi_conf_get_ip_from_str(ip_addr, (char *)buff);
    IP4_ADDR(&tcpip_config->sta_addr, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    IP4_ADDR(&tcpip_config->ap_addr, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    sz = sizeof(buff);
    nvdm_read_data_item("common", "IpNetmask", buff, &sz);
    wifi_conf_get_ip_from_str(ip_addr, (char *)buff);
    IP4_ADDR(&tcpip_config->sta_mask, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    IP4_ADDR(&tcpip_config->ap_mask, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    sz = sizeof(buff);
    nvdm_read_data_item("common", "IpGateway", buff, &sz);
    wifi_conf_get_ip_from_str(ip_addr, (char *)buff);
    IP4_ADDR(&tcpip_config->sta_gateway, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    IP4_ADDR(&tcpip_config->ap_gateway, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    return 0;
}

static uint32_t ip_number_to_big_endian(uint32_t ip_number)
{
    uint8_t *byte = (uint8_t *)&ip_number;
    return (uint32_t)((byte[0] << 24) | (byte[1] << 16) | (byte[2] << 8) | byte[3]);
}

static void ip_number_to_string(uint32_t ip_number, char ip_string[IP4ADDR_STRLEN_MAX])
{
    snprintf(ip_string,
                IP4ADDR_STRLEN_MAX,
                "%d.%d.%d.%d",
                (uint8_t)((ip_number >> 24) & 0xFF),
                (uint8_t)((ip_number >> 16) & 0xFF),
                (uint8_t)((ip_number >> 8) & 0xFF),
                (uint8_t)((ip_number >> 0) & 0xFF));
}

static void dhcpd_set_ip_pool(const ip4_addr_t *ap_ip_addr,
                              const ip4_addr_t *ap_net_mask,
                              char ip_pool_start[IP4ADDR_STRLEN_MAX],
                              char ip_pool_end[IP4ADDR_STRLEN_MAX])
{
    uint32_t ap_ip_number = ip_number_to_big_endian(ip4_addr_get_u32(ap_ip_addr));
    uint32_t ap_mask_number = ip_number_to_big_endian(ip4_addr_get_u32(ap_net_mask));
    uint32_t ip_range_min = ap_ip_number & ap_mask_number;
    uint32_t ip_range_max = ip_range_min | (~ap_mask_number);

    if ((ip_range_max - ap_ip_number) > (ap_ip_number - ip_range_min)) {
        ip_number_to_string(ap_ip_number + 1, ip_pool_start);
        ip_number_to_string(ip_range_max - 1, ip_pool_end);
    } else {
        ip_number_to_string(ip_range_min + 1, ip_pool_start);
        ip_number_to_string(ap_ip_number - 1, ip_pool_end);
    }
}

static void dhcpd_settings_init(const tcpip_config_t *tcpip_config,
                                dhcpd_settings_t *dhcpd_settings)
{
    STRCPY(dhcpd_settings->dhcpd_server_address,
               ip4addr_ntoa(&tcpip_config->ap_addr),
               IP4ADDR_STRLEN_MAX);

    STRCPY(dhcpd_settings->dhcpd_netmask,
               ip4addr_ntoa(&tcpip_config->ap_mask),
               IP4ADDR_STRLEN_MAX);

    STRCPY(dhcpd_settings->dhcpd_gateway,
               (char *)dhcpd_settings->dhcpd_server_address,
               IP4ADDR_STRLEN_MAX);

    STRCPY(dhcpd_settings->dhcpd_primary_dns,
               (char *)dhcpd_settings->dhcpd_server_address,
               IP4ADDR_STRLEN_MAX);

    /* secondary DNS is not defined by default */
    STRCPY(dhcpd_settings->dhcpd_secondary_dns,
               "0.0.0.0",
               IP4ADDR_STRLEN_MAX);

    dhcpd_set_ip_pool(&tcpip_config->ap_addr,
                      &tcpip_config->ap_mask,
                      dhcpd_settings->dhcpd_ip_pool_start,
                      dhcpd_settings->dhcpd_ip_pool_end);
}

static int32_t system_config_init(sys_config_t *sys_config)
{
    int32_t ret_val = wifi_config_init(&sys_config->wifi_config);
    if (0 != ret_val) {
        LOG_E(common, "wifi config init fail");
        return -1;
    }

    ret_val = tcpip_config_init(&sys_config->tcpip_config);
    if (0 != ret_val) {
        LOG_E(common, "tcpip config init fail");
        return -1;
    }

    dhcpd_settings_init(&g_sys_config.tcpip_config, &g_sys_config.dhcpd_settings);

    return 0;
}

static void ip_change_call_back(struct netif *netif)
{
    uint8_t op_mode = 0;
    if (!ip4_addr_isany_val(netif->ip_addr)) {
        char ip_addr[17] = {0};
		if(NULL != inet_ntoa(netif->ip_addr)) {
            strcpy(ip_addr, inet_ntoa(netif->ip_addr));
            LOG_I(common, "************************");
            LOG_I(common, "DHCP got IP:%s", ip_addr);
            LOG_I(common, "************************");
        } else {
            LOG_E(common, "DHCP got Failed");
        }

#ifdef MTK_WIFI_REPEATER_ENABLE
        struct netif *ap_if = netif_find("ap1");
        wifi_config_get_opmode(&op_mode);
        if (WIFI_MODE_REPEATER == op_mode) {
            g_sys_config.wifi_config.opmode = op_mode;
            netif_set_addr(ap_if, &netif->ip_addr, &netif->netmask, &netif->gw);
        }
#endif
    }

    /* notify uplayer */
    if (NULL != g_ip_ready_callback) {
        g_ip_ready_callback(netif);
    }
}

static int32_t wifi_init_done_handler(wifi_event_t event,
                                      uint8_t *payload,
                                      uint32_t length)
{
    LOG_I(common, "WiFi Init Done: port = %d", payload[6]);
    return 1;
}

static int32_t wifi_station_port_secure_event_handler(wifi_event_t event,
                                                    uint8_t *payload,
                                                    uint32_t length)
{
    uint8_t opmode  = 0;
    LOG_HEXDUMP_I(wifi, "event id: %d", payload, length, event);
    wifi_config_get_opmode(&opmode);
    if ((WIFI_MODE_AP_ONLY != opmode) && (WIFI_EVENT_IOT_PORT_SECURE == event)) {
        LOG_I(wifi, "wifi connect, %d", event);
        struct netif *sta_if = netif_find("st2");
        netif_set_status_callback(sta_if, ip_change_call_back);
        netif_set_link_up(sta_if);
        if (STA_IP_MODE_STATIC == g_sys_config.tcpip_config.sta_ip_mode) {
            netif_set_addr(sta_if,
                           &g_sys_config.tcpip_config.sta_addr,
                           &g_sys_config.tcpip_config.sta_mask,
                           &g_sys_config.tcpip_config.sta_gateway);
            /* notify uplayer */
            if (NULL != g_ip_ready_callback) {
                g_ip_ready_callback(sta_if);
            }
        }
    }
    return 1;
}

static int32_t wifi_station_connected_event_handler(wifi_event_t event,
                                                    uint8_t *payload,
                                                    uint32_t length)
{
    LOG_HEXDUMP_I(wifi, "event id: %d", payload, length, event);
    return 1;
}

static int32_t wifi_station_disconnected_event_handler(wifi_event_t event,
                                                       uint8_t *payload,
                                                       uint32_t length)
{
    uint8_t opmode  = 0;
    LOG_HEXDUMP_I(wifi, "event id: %d", payload, length, event);
    wifi_config_get_opmode(&opmode);
    if ((WIFI_MODE_AP_ONLY != opmode) && WIFI_EVENT_IOT_DISCONNECTED == event) {
        LOG_I(wifi, "wifi disconnect, %d", event);
        if (STA_IP_MODE_DHCP == g_sys_config.tcpip_config.sta_ip_mode) {
            struct netif *sta_if = netif_find("st2");
            netif_set_status_callback(sta_if, NULL);
            netif_set_link_down(sta_if);
            netif_set_addr(sta_if, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY);
        }
    }
    return 1;
}

static void process_softap(void)
{
    struct netif *ap_if = netif_find("ap1");
    netif_set_default(ap_if);

    dhcpd_start(&g_sys_config.dhcpd_settings);
}

void network_init(void)
{
    if (0 != system_config_init(&g_sys_config)) {
        LOG_E(common, "sys config init fail");
        return;
    }

    wifi_config_t config = {0};
    wifi_config_ext_t config_ext = {0};

    config.opmode = g_sys_config.wifi_config.opmode;

    memcpy(config.sta_config.ssid, g_sys_config.wifi_config.sta_ssid, 32);
    config.sta_config.ssid_length = g_sys_config.wifi_config.sta_ssid_len;
    config.sta_config.bssid_present = 0;
    memcpy(config.sta_config.password, g_sys_config.wifi_config.sta_wpa_psk, 64);
    config.sta_config.password_length = g_sys_config.wifi_config.sta_wpa_psk_len;
    config_ext.sta_wep_key_index_present = 1;
    config_ext.sta_wep_key_index = g_sys_config.wifi_config.sta_default_key_id;
    config_ext.sta_auto_connect_present = 1;
    config_ext.sta_auto_connect = 1;

    memcpy(config.ap_config.ssid, g_sys_config.wifi_config.ap_ssid, 32);
    config.ap_config.ssid_length = g_sys_config.wifi_config.ap_ssid_len;
    memcpy(config.ap_config.password, g_sys_config.wifi_config.ap_wpa_psk, 64);
    config.ap_config.password_length = g_sys_config.wifi_config.ap_wpa_psk_len;
    config.ap_config.auth_mode = (wifi_auth_mode_t)g_sys_config.wifi_config.ap_auth_mode;
    config.ap_config.encrypt_type = (wifi_encrypt_type_t)g_sys_config.wifi_config.ap_encryp_type;
    config.ap_config.channel = g_sys_config.wifi_config.ap_channel;
    config.ap_config.bandwidth = g_sys_config.wifi_config.ap_bw;
    config.ap_config.bandwidth_ext = WIFI_BANDWIDTH_EXT_40MHZ_UP;
    config_ext.ap_wep_key_index_present = 1;
    config_ext.ap_wep_key_index = g_sys_config.wifi_config.ap_default_key_id;
    config_ext.ap_hidden_ssid_enable_present = 1;
    config_ext.ap_hidden_ssid_enable = g_sys_config.wifi_config.ap_hide_ssid;

    wifi_init(&config, &config_ext);

    tcpip_stack_init(&g_sys_config.tcpip_config, config.opmode);

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_INIT_COMPLETE, wifi_init_done_handler);
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_PORT_SECURE, wifi_station_port_secure_event_handler);
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_CONNECTED, wifi_station_connected_event_handler);
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_DISCONNECTED, wifi_station_disconnected_event_handler);
}

void network_dhcp_start(uint8_t opmode)
{
    if (opmode > WIFI_MODE_P2P_ONLY) {
        LOG_E(common, "opmode not valid: %d", (int)opmode);
        return;
    }

    static const char *mode_name[] = {
        "STA", "STA", "AP", "Repeater", "Monitor", "P2P"
    };
    LOG_I(common, "In %s mode.", mode_name[opmode]);

    struct netif *sta_if = netif_find("st2");
	struct netif *ap_if = netif_find("ap1");
    if (opmode == WIFI_MODE_AP_ONLY) {
        LOG_I(common, "Target ssid: %s", (char *)g_sys_config.wifi_config.ap_ssid);
        netif_set_link_up(ap_if);
        process_softap();
        netif_set_status_callback(ap_if, NULL);
    }
#ifdef MTK_WIFI_REPEATER_ENABLE
    else if (opmode == WIFI_MODE_REPEATER) {
        LOG_I(common, "Target ssid: %s", (char *)g_sys_config.wifi_config.sta_ssid);
        if (STA_IP_MODE_DHCP == g_sys_config.tcpip_config.sta_ip_mode) {
#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
            dhcp_set_no_discover_handler(inband_evt_rescan);
#endif
            netif_set_status_callback(sta_if, ip_change_call_back);
            dhcp_start(sta_if);
        }
    }
#endif
    else {
        LOG_I(common, "Target ssid: %s", (char *)g_sys_config.wifi_config.sta_ssid);
        if (STA_IP_MODE_DHCP == g_sys_config.tcpip_config.sta_ip_mode) {
#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
            dhcp_set_no_discover_handler(inband_evt_rescan);
#endif
            netif_set_link_down(sta_if);
            netif_set_status_callback(sta_if, ip_change_call_back);
            dhcp_start(sta_if);
        }
    }
}

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
extern int32_t mtk_smart_connect(void);

int32_t cf_set_ssid(uint8_t port, uint8_t *ssid , uint8_t ssid_length)
{
    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (ssid_length > WIFI_MAX_LENGTH_OF_SSID) {
        LOG_I(wifi, "incorrect length(=%d)", ssid_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == ssid) {
        LOG_E(wifi, "ssid is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    char ssid_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char ssid_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    os_memcpy(ssid_buf, ssid, ssid_length);
    ssid_buf[ssid_length] = '\0';

    sprintf(ssid_len_buf, "%d", ssid_length);
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "Ssid",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_buf, os_strlen(ssid_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "SsidLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_len_buf, os_strlen(ssid_len_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Ssid",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_buf, os_strlen(ssid_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SsidLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_len_buf, os_strlen(ssid_len_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}

int32_t cf_set_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length)
{
    char pass_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char pass_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (passphrase == NULL) {
        LOG_E(wifi, "passphrase is null.");
        return WIFI_ERR_PARA_INVALID;
    }
    if ((passphrase_length < 8) || (passphrase_length > WIFI_LENGTH_PASSPHRASE)) {
        LOG_E(wifi, "incorrect length(=%d)", passphrase_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (passphrase_length == WIFI_LENGTH_PASSPHRASE) {
        for (uint8_t index = 0; index < WIFI_LENGTH_PASSPHRASE; index++) {
            if (!hex_isdigit(passphrase[index])) {
                LOG_E(wifi, "length(=%d) but the strings are not hex strings!", passphrase_length);
                return WIFI_ERR_PARA_INVALID;
            }
        }
    }

    sprintf(pass_len_buf, "%d", passphrase_length);
    os_memcpy(pass_buf, passphrase, passphrase_length);
    pass_buf[passphrase_length] = '\0';

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPskLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_len_buf, os_strlen(pass_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPsk",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_buf, os_strlen(pass_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPskLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_len_buf, os_strlen(pass_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPsk",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_buf, os_strlen(pass_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}
int32_t cf_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type)
{
    char auth_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char encrypt_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (!wifi_is_auth_mode_valid(auth_mode)) {
        LOG_E(wifi, "auth_mode is invalid: %d", auth_mode);
        return WIFI_ERR_PARA_INVALID;
    }
    if (!wifi_is_encrypt_type_valid(encrypt_type)) {
        LOG_E(wifi, "encrypt_type is invalid: %d", encrypt_type);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(auth_buf, "%d", auth_mode);
    sprintf(encrypt_buf, "%d", encrypt_type);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "AuthMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)auth_buf, os_strlen(auth_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "EncrypType",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)encrypt_buf, os_strlen(encrypt_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "AuthMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)auth_buf, os_strlen(auth_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "EncrypType",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)encrypt_buf, os_strlen(encrypt_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}

int32_t cf_set_channel(uint8_t port, uint8_t channel)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%d", channel);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "Channel",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Channel",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    return WIFI_SUCC;
}
int save_cf_credential_to_nvdm(P_IOT_CONFIG_FREE_IE cred)
{
    int status = 0;

    if (cred == NULL) {
        LOG_E(wifi, "ERROR! invalid cred pointer(NULL)");
        return -1;
    } else {
        LOG_E(wifi, "[ConfigFree] cred: Ssid = %s, SsidLen = %d, AuthMode = %d, EncrypType = %d, WpaPsk = %s, WpaPskLen = %d, Ch = %d",
              cred->Ssid,
              cred->SsidLen,
              cred->AuthMode,
              cred->EncrypType,
              cred->WpaPsk,
              cred->WpaPskLen,
              cred->Channel);
    }

    // Set NVRAM STA configuration by credential
    if (cf_set_ssid(WIFI_PORT_STA, cred->Ssid, cred->SsidLen) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][STA] wifi_profile_set_ssid failed (Ssid=%s, Len=%d)", cred->Ssid, cred->SsidLen);
        status = -1;
    }

    if (cf_set_security_mode(WIFI_PORT_STA, cred->AuthMode, cred->EncrypType) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][STA] wifi_profile_set_security_mode failed (Auth=%d, Encry=%d)", cred->AuthMode, cred->EncrypType);
        status = -1;
    }

    if (cf_set_wpa_psk_key(WIFI_PORT_STA, cred->WpaPsk, cred->WpaPskLen) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][STA] wifi_profile_set_wpa_psk_key failed (WpaPsk=%s, Len=%d)", cred->WpaPsk, cred->WpaPskLen);
        status = -1;
    }

    // Set NVRAM AP configuration by credential
    if (cf_set_ssid(WIFI_PORT_AP, cred->Ssid, cred->SsidLen) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][AP] wifi_profile_set_ssid failed (Ssid=%s, Len=%d)", cred->Ssid, cred->SsidLen);
        status = -1;
    }

    if (cf_set_security_mode(WIFI_PORT_AP, cred->AuthMode, cred->EncrypType) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][AP] wifi_profile_set_security_mode failed (Auth=%d, Encry=%d)", cred->AuthMode, cred->EncrypType);
        status = -1;
    }

    if (cf_set_channel(WIFI_PORT_AP, cred->Channel) != 0) {
        LOG_E(wifi, "ERROR! [ConfigFree][AP] wifi_profile_set_channel failed (Ch=%d)", cred->Channel);
        status = -1;
    }

    return status;
}

int32_t save_cf_ready_to_nvdm(uint8_t config_ready)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (0 != config_ready && 1 != config_ready) {
        LOG_E(wifi, "config_ready is invalid: %d", config_ready);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", WIFI_MODE_REPEATER);
    LOG_I(wifi, "wifi_profile_set_opmode: opmode=%s", buf);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    os_memset(buf, 0x0, WIFI_PROFILE_BUFFER_LENGTH);

    sprintf(buf, "%d", config_ready);
    LOG_I(wifi, "ConfigFree ready: %s", buf);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "ConfigFree_Ready",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}

int32_t get_cf_ready_to_nvdm(uint8_t *config_ready)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == config_ready) {
        LOG_E(wifi, "config_ready is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_COMMON, "ConfigFree_Ready", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *config_ready = atoi(buf);
    return WIFI_SUCC;
}
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */

void network_full_init(void)
{
#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
    uint8_t configured = 0;
    register_configure_free_callback(save_cf_credential_to_nvdm,  save_cf_ready_to_nvdm);
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */
    network_init();
    network_dhcp_start(g_sys_config.wifi_config.opmode);
#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
    get_cf_ready_to_nvdm(&configured);
    if (!configured) { // not configured
        /* Config-Free Demo */
        if (g_sys_config.wifi_config.opmode == 1) {
            mtk_smart_connect();
        }
    }
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */

}

static int32_t net_clean_on_mode_change(uint8_t origin_op_mode)
{
    struct netif *ap_if = netif_find("ap1");
    struct netif *sta_if = netif_find("st2");

    /* unregister the callbacks in case it is unexpectedly called */
    netif_set_status_callback(ap_if, NULL);
    netif_set_status_callback(sta_if, NULL);

    if (WIFI_MODE_STA_ONLY == origin_op_mode) {
        if (STA_IP_MODE_DHCP == g_sys_config.tcpip_config.sta_ip_mode) {
            /* dhcp should be stopped */
            dhcp_stop(sta_if);
        }
        return 0;
    }

    if (WIFI_MODE_AP_ONLY == origin_op_mode) {
        /* dhcpd should be stopped */
        dhcpd_stop();
        return 0;
    }

    if (WIFI_MODE_REPEATER == origin_op_mode) {
        if (STA_IP_MODE_DHCP == g_sys_config.tcpip_config.sta_ip_mode) {
            /* dhcp should be stopped */
            struct netif *sta_if = netif_find("st2");
            dhcp_stop(sta_if);
        }
        return 0;
    }

    if (WIFI_MODE_MONITOR == origin_op_mode) {
        /* nothing need to be cleaned */
        return 0;
    }

    if (WIFI_MODE_P2P_ONLY == origin_op_mode) {
        /* TBD for P2P Mode */
        return 0;
    }

    return -1;
}

uint8_t wifi_set_opmode(uint8_t target_mode)
{
    uint8_t origin_op_mode = 0;
    wifi_config_get_opmode(&origin_op_mode);

    //clean up
    if (0 != net_clean_on_mode_change(origin_op_mode)) {
        LOG_E(wifi, "net_clean_on_mode_change fail. origin_op_mode: %d", origin_op_mode);
        return 1;
    }

    wifi_config_set_opmode(target_mode);
    LOG_I(wifi, "set opmode to [%d]", target_mode);

    struct netif *ap_if = netif_find("ap1");
    struct netif *sta_if = netif_find("st2");

    /* Recover the ip/mask/gateway, which could be changed in STA mode by DHCP */
    netif_set_addr(ap_if,
                   &g_sys_config.tcpip_config.ap_addr,
                   &g_sys_config.tcpip_config.ap_mask,
                   &g_sys_config.tcpip_config.ap_gateway);
    netif_set_addr(sta_if,
                   &g_sys_config.tcpip_config.sta_addr,
                   &g_sys_config.tcpip_config.sta_mask,
                   &g_sys_config.tcpip_config.sta_gateway);

    if (WIFI_MODE_STA_ONLY == target_mode) {
        netif_set_default(sta_if);
        if (STA_IP_MODE_DHCP == g_sys_config.tcpip_config.sta_ip_mode) {
            netif_set_status_callback(sta_if, ip_change_call_back);
            dhcp_start(sta_if);
        }
        return 0;
    }

    if (WIFI_MODE_AP_ONLY == target_mode) {
        netif_set_default(ap_if);
        netif_set_link_up(ap_if);
        dhcpd_start(&g_sys_config.dhcpd_settings);
        return 0;
    }

    if (WIFI_MODE_REPEATER == target_mode) {
        netif_set_default(sta_if);
        if (STA_IP_MODE_DHCP == g_sys_config.tcpip_config.sta_ip_mode) {
            netif_set_status_callback(sta_if, ip_change_call_back);
            dhcp_start(sta_if);
        }
        return 0;
    }

    if (WIFI_MODE_MONITOR == target_mode) {
        //nothing need to do in Monitor MOde
        return 0;
    }

    if (WIFI_MODE_P2P_ONLY == target_mode) {
        //TBD for P2P Mode
        return 0;
    }

    return 1;
}

