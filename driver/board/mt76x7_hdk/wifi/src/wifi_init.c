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

#include "os.h"
#include "wifi_api.h"
#include "connsys_driver.h"
#include "wifi_scan.h"
#include "inband_queue.h"
#include "net_task.h"
#include "wifi_init.h"
#include "syslog.h"
#include "nvdm.h"
#include "wifi_profile.h"
#include "get_profile_string.h"
#include "wifi_default_config.h"
#include "wifi_inband.h"
#include "wpa_supplicant_task.h"
#include "hal_efuse.h"

/* bad implementation */
wifi_phy_mode_t wifi_change_wireless_mode_5g_to_2g(wifi_phy_mode_t wirelessmode);

log_create_module(wifi, PRINT_LEVEL_ERROR);

static int32_t wifi_is_mac_address_valid(uint8_t *mac_addr)
{
    uint32_t byte_sum = 0;
    for (uint32_t index = 0; index < WIFI_MAC_ADDRESS_LENGTH; index++) {
        byte_sum += mac_addr[index];
    }
    return (byte_sum != 0);
}

/**
* @brief get mac address from efuse
*/
static int32_t wifi_get_mac_addr_from_efuse(uint8_t port, uint8_t *mac_addr)
{
    uint8_t buf[16] = {0};//efuse is 16 byte aligned
    uint16_t mac_offset = 0x00;//mac addr offset in efuse
    if (HAL_EFUSE_OK != hal_efuse_read(mac_offset, buf, sizeof(buf))) {
        return -1;
    }
    if (!wifi_is_mac_address_valid(buf+4)) {
        LOG_HEXDUMP_W(wifi, "data in efuse is invalid", buf, sizeof(buf));
        return -1;
    }
    if (WIFI_PORT_STA == port) {
        /* original efuse MAC address for STA */
        os_memcpy(mac_addr, buf+4, WIFI_MAC_ADDRESS_LENGTH);
    } else {
        /* original efuse MAC address with byte[5]+1 for AP */
        os_memcpy(mac_addr, buf+4, WIFI_MAC_ADDRESS_LENGTH);
        mac_addr[WIFI_MAC_ADDRESS_LENGTH-1] += 1;
    }
    return 0;
}

/**
* @brief get mac address from nvdm
*/
static int32_t wifi_get_mac_addr_from_nvdm(uint8_t port, uint8_t *mac_addr)
{
    uint8_t buff[PROFILE_BUF_LEN] = {0};
    uint32_t len = sizeof(buff);
    char *group_name = (WIFI_PORT_STA == port) ? "STA" : "AP";

    if (NVDM_STATUS_OK != nvdm_read_data_item(group_name, "MacAddr", buff, &len)) {
        return -1;
    }

    wifi_conf_get_mac_from_str((char *)mac_addr, (char *)buff);
    return 0;
}

/**
* @brief Get WiFi Interface MAC Address.
*
*/
int32_t wifi_config_get_mac_address(uint8_t port, uint8_t *address)
{
    if (NULL == address) {
        LOG_E(wifi, "address is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }

    if (0 == wifi_get_mac_addr_from_efuse(port, address)) {
        return 0;
    }

    LOG_W(wifi, "wifi_get_mac_addr_from_efuse fail.");
    if (0 == wifi_get_mac_addr_from_nvdm(port, address)) {
        return 0;
    }

    LOG_E(wifi, "wifi_get_mac_addr_from_nvdm fail.");
    return -1;
}

uint8_t wifi_get_ps_mode(void)
{
#ifdef MTK_WIFI_PROFILE_ENABLE
    uint8_t buff[PROFILE_BUF_LEN];
    uint32_t len = sizeof(buff);
    nvdm_read_data_item("STA", "PSMode", buff, &len);
    return (uint8_t)atoi((char *)buff);
#else
    return WIFI_DEFAULT_STA_POWER_SAVE_MODE;
#endif
}


void wifi_driver_init(sys_cfg_t *wifi_config)
{
    connsys_init(wifi_config);
    wifi_scan_init(wifi_config->sta_ssid,
                   wifi_config->sta_ssid_len);
    inband_queue_init();
    NetTaskInit();
}

static void wifi_save_sta_ext_config(sys_cfg_t *syscfg, wifi_config_ext_t *config_ext)
{
    if (NULL != config_ext) {
        if (config_ext->sta_wep_key_index_present) {
            syscfg->sta_default_key_id = config_ext->sta_wep_key_index;
        }
        if (config_ext->sta_auto_connect_present) {
            /* save sta auto connect */
        }
    }
}

static void wifi_save_ap_ext_config(sys_cfg_t *syscfg, wifi_config_ext_t *config_ext)
{
    if (NULL != config_ext) {
        if (config_ext->ap_wep_key_index_present) {
            syscfg->ap_default_key_id = config_ext->ap_wep_key_index;
        }
        if (config_ext->ap_hidden_ssid_enable_present) {
            syscfg->ap_hide_ssid = config_ext->ap_hidden_ssid_enable;
        }
    }
}

static void wifi_apply_sta_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    os_memcpy(syscfg->sta_ssid, config->sta_config.ssid, WIFI_MAX_LENGTH_OF_SSID);

    syscfg->sta_ssid_len = config->sta_config.ssid_length;

    os_memcpy(syscfg->sta_wpa_psk, config->sta_config.password, WIFI_LENGTH_PASSPHRASE);

    syscfg->sta_wpa_psk_len = config->sta_config.password_length;

    if (config->sta_config.bssid_present) {
        //save bssid
    }

    /* save extension config */
    wifi_save_sta_ext_config(syscfg, config_ext);
}

static void wifi_apply_ap_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    os_memcpy(syscfg->ap_ssid, config->ap_config.ssid, WIFI_MAX_LENGTH_OF_SSID);

    syscfg->ap_ssid_len = config->ap_config.ssid_length;

    os_memcpy(syscfg->ap_wpa_psk, config->ap_config.password, WIFI_LENGTH_PASSPHRASE);

    syscfg->ap_wpa_psk_len = config->ap_config.password_length;

    syscfg->ap_auth_mode = config->ap_config.auth_mode;

    syscfg->ap_encryp_type = config->ap_config.encrypt_type;

    syscfg->ap_channel = config->ap_config.channel;

    syscfg->ap_bw = config->ap_config.bandwidth;

    syscfg->ap_ht_ext_ch = (WIFI_BANDWIDTH_EXT_40MHZ_UP == config->ap_config.bandwidth_ext) ? 1 : 3;

    /* save extension config */
    wifi_save_ap_ext_config(syscfg, config_ext);
}

static void wifi_apply_repeater_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    wifi_apply_sta_config(syscfg, config, config_ext);
    wifi_apply_ap_config(syscfg, config, config_ext);
}

static void wifi_apply_p2p_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    /* TBD */
}

#ifdef MTK_WIFI_PROFILE_ENABLE
static void wifi_get_config_from_nvdm(sys_cfg_t *config)
{
    // init wifi profile
    uint8_t buff[PROFILE_BUF_LEN];
    uint32_t len = sizeof(buff);

    // common
    len = sizeof(buff);
    nvdm_read_data_item("common", "OpMode", buff, &len);
    config->opmode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryRegion", buff, &len);
    config->country_region = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryRegionABand", buff, &len);
    config->country_region_a_band = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryCode", buff, &len);
    os_memcpy(config->country_code, buff, 4);
    len = sizeof(buff);
    nvdm_read_data_item("common", "RadioOff", buff, &len);
    config->radio_off = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "RTSThreshold", buff, &len);
    config->rts_threshold = (uint16_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "FragThreshold", buff, &len);
    config->frag_threshold = (uint16_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "DbgLevel", buff, &len);
    config->dbg_level = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "IpAddr", buff, &len);
    wifi_conf_get_ip_from_str(config->ap_ip_addr, (char *)buff);
    wifi_conf_get_ip_from_str(config->sta_ip_addr, (char *)buff);

    // STA
    len = sizeof(buff);
    nvdm_read_data_item("STA", "LocalAdminMAC", buff, &len);
    config->sta_local_admin_mac = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);

    //nvdm_read_data_item("STA", "MacAddr", buff, &len);
    //wifi_conf_get_mac_from_str((char *)config->sta_mac_addr, (char *)buff);
    wifi_config_get_mac_address(WIFI_PORT_STA, config->sta_mac_addr);

    len = sizeof(buff);
    nvdm_read_data_item("STA", "SsidLen", buff, &len);
    config->sta_ssid_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "Ssid", buff, &len);
    os_memcpy(config->sta_ssid, buff, config->sta_ssid_len);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BssType", buff, &len);
    config->sta_bss_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "Channel", buff, &len);
    config->sta_channel = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BW", buff, &len);
    config->sta_bw = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WirelessMode", buff, &len);
    if (wifi_5g_support() < 0) {
        config->sta_wireless_mode = (uint8_t)wifi_change_wireless_mode_5g_to_2g((wifi_phy_mode_t)atoi((char *)buff));
    }else {
        config->sta_wireless_mode = (uint8_t)atoi((char *)buff);
    }
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BADecline", buff, &len);
    config->sta_ba_decline = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "AutoBA", buff, &len);
    config->sta_auto_ba = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_MCS", buff, &len);
    config->sta_ht_mcs = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_BAWinSize", buff, &len);
    config->sta_ht_ba_win_size = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_GI", buff, &len);
    config->sta_ht_gi = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_PROTECT", buff, &len);
    config->sta_ht_protect = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_EXTCHA", buff, &len);
    config->sta_ht_ext_ch = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WmmCapable", buff, &len);
    config->sta_wmm_capable = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "ListenInterval", buff, &len);
    config->sta_listen_interval = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "AuthMode", buff, &len);
    config->sta_auth_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "EncrypType", buff, &len);
    config->sta_encryp_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WpaPskLen", buff, &len);
    config->sta_wpa_psk_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WpaPsk", buff, &len);
    os_memcpy(config->sta_wpa_psk, buff, config->sta_wpa_psk_len);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PMK", buff, &len);
    os_memcpy(config->sta_pmk, buff, 32); // TODO: How to save binary PMK value not ending by ' ; ' ?
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PairCipher", buff, &len);
    config->sta_pair_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "GroupCipher", buff, &len);
    config->sta_group_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "DefaultKeyId", buff, &len);
    config->sta_default_key_id = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PSMode", buff, &len);
    config->sta_ps_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "KeepAlivePeriod", buff, &len);
    config->sta_keep_alive_period = (uint8_t)atoi((char *)buff);

    // AP
#ifdef MTK_WIFI_REPEATER_ENABLE
    if (config->opmode == WIFI_MODE_REPEATER) {
        len = sizeof(buff);
        nvdm_read_data_item("STA", "Channel", buff, &len);
        config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "BW", buff, &len);
        config->ap_bw = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "WirelessMode", buff, &len);
        config->ap_wireless_mode = (uint8_t)atoi((char *)buff);
    } else {
#endif
        /* Use STA MAC/IP as AP MAC/IP for the time being, due to N9 dual interface not ready yet */
        len = sizeof(buff);
        nvdm_read_data_item("AP", "Channel", buff, &len);
        config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "BW", buff, &len);
        config->ap_bw = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "WirelessMode", buff, &len);
        if (wifi_5g_support() < 0) {
            config->ap_wireless_mode = (uint8_t)wifi_change_wireless_mode_5g_to_2g((wifi_phy_mode_t)atoi((char *)buff));
        }else {
            config->ap_wireless_mode = (uint8_t)atoi((char *)buff);
        }
#ifdef MTK_WIFI_REPEATER_ENABLE
    }
#endif /* MTK_WIFI_REPEATER_ENABLE */
    len = sizeof(buff);
    nvdm_read_data_item("AP", "LocalAdminMAC", buff, &len);
    config->ap_local_admin_mac = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);

    //nvdm_read_data_item("AP", "MacAddr", buff, &len);
    //wifi_conf_get_mac_from_str((char *)config->ap_mac_addr, (char *)buff);
    wifi_config_get_mac_address(WIFI_PORT_AP, config->ap_mac_addr);

    len = sizeof(buff);
    nvdm_read_data_item("AP", "SsidLen", buff, &len);
    config->ap_ssid_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "Ssid", buff, &len);
    os_memcpy(config->ap_ssid, buff, config->ap_ssid_len);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AutoBA", buff, &len);
    config->ap_auto_ba = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_MCS", buff, &len);
    config->ap_ht_mcs = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_BAWinSize", buff, &len);
    config->ap_ht_ba_win_size = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_GI", buff, &len);
    config->ap_ht_gi = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_PROTECT", buff, &len);
    config->ap_ht_protect = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_EXTCHA", buff, &len);
    config->ap_ht_ext_ch = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WmmCapable", buff, &len);
    config->ap_wmm_capable = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "DtimPeriod", buff, &len);
    config->ap_dtim_period = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HideSSID", buff, &len);
    config->ap_hide_ssid = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AutoChannelSelect", buff, &len);
    config->ap_auto_channel_select = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AuthMode", buff, &len);
    config->ap_auth_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "EncrypType", buff, &len);
    config->ap_encryp_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WpaPskLen", buff, &len);
    config->ap_wpa_psk_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WpaPsk", buff, &len);
    os_memcpy(config->ap_wpa_psk, buff, config->ap_wpa_psk_len);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "PMK", buff, &len);
    os_memcpy(config->ap_pmk, buff, 32); // TODO: How to save binary PMK value not ending by ' ; ' ?
    len = sizeof(buff);
    nvdm_read_data_item("AP", "PairCipher", buff, &len);
    config->ap_pair_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "GroupCipher", buff, &len);
    config->ap_group_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "DefaultKeyId", buff, &len);
    config->ap_default_key_id = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "BcnDisEn", buff, &len);
    config->ap_beacon_disable = (uint8_t)atoi((char *)buff);

    // scan channel table and regulatory table
    len = sizeof(buff);
    nvdm_read_data_item("common", "BGChannelTable", buff, &len);
    config->bg_band_entry_num = wifi_conf_get_ch_table_from_str(config->bg_band_triple, 10, (char *)buff, os_strlen((char *)buff));

    len = sizeof(buff);
    nvdm_read_data_item("common", "AChannelTable", buff, &len);
    config->a_band_entry_num = wifi_conf_get_ch_table_from_str(config->a_band_triple, 10, (char *)buff, os_strlen((char *)buff));

    config->forwarding_zero_copy = 1;

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
    /* These are for MBSS support, but not exist trunk (it's customer feature), however,
            we have to add them here due to N9 FW has them (only one version of N9 FW)
         */
    // TODO: How to solve it in the future...Michael
    config->mbss_enable = 0;
    os_memset(config->mbss_ssid1, 0x0, sizeof(config->mbss_ssid1));;
    config->mbss_ssid_len1 = 0;
    os_memset(config->mbss_ssid2, 0x0, sizeof(config->mbss_ssid2));;
    config->mbss_ssid_len2 = 0;

    len = sizeof(buff);
    nvdm_read_data_item("common", "ConfigFree_Ready", buff, &len);
    config->config_free_ready = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "ConfigFree_Enable", buff, &len);
    config->config_free_enable = (uint8_t)atoi((char *)buff);
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */
    len = sizeof(buff);
    nvdm_read_data_item("common", "StaFastLink", buff, &len);
    config->sta_fast_link = (uint8_t)atoi((char *)buff);
}
#endif
static void wifi_apply_user_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    syscfg->opmode = config->opmode;
    if (WIFI_MODE_STA_ONLY == syscfg->opmode) {
        wifi_apply_sta_config(syscfg, config, config_ext);
    } else if (WIFI_MODE_AP_ONLY == syscfg->opmode) {
        wifi_apply_ap_config(syscfg, config, config_ext);
    } else if (WIFI_MODE_REPEATER == syscfg->opmode) {
        wifi_apply_repeater_config(syscfg, config, config_ext);
    } else if (WIFI_MODE_P2P_ONLY == syscfg->opmode) {
        wifi_apply_p2p_config(syscfg, config, config_ext);
    } else {
        /* no configuration is required for Monitor Mode */
    }
}

/**
* @brief build the whole configurations
*/
static int32_t wifi_build_whole_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
#ifdef MTK_WIFI_PROFILE_ENABLE
    wifi_get_config_from_nvdm(syscfg);
#else
    if (0 != wifi_get_default_config(syscfg)) {
        return -1;
    }
#endif
    wifi_apply_user_config(syscfg, config, config_ext);
    return 0;
}

void wifi_init(wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    sys_cfg_t syscfg = {0};

    if (NULL == config) {
        LOG_E(wifi, "config is null.");
        return;
    }

    if (0 != wifi_build_whole_config(&syscfg, config, config_ext)) {
        LOG_E(wifi, "wifi_build_whole_config fail. initial aborted!");
        return;
    }

    connsys_init(&syscfg);
    wifi_scan_init(syscfg.sta_ssid,
                   syscfg.sta_ssid_len);
    inband_queue_init();
    NetTaskInit();

   /*Fix if SSID length = 0, switch to repeater mode from station mode, N9 will assert*/
    if(config->ap_config.ssid_length == 0) {
        config->ap_config.ssid_length = syscfg.ap_ssid_len;
        os_memcpy(config->ap_config.ssid, syscfg.ap_ssid, WIFI_MAX_LENGTH_OF_SSID);
    } else if(config->sta_config.ssid_length == 0) {
        config->sta_config.ssid_length = syscfg.sta_ssid_len;
        os_memcpy(config->sta_config.ssid, syscfg.sta_ssid, WIFI_MAX_LENGTH_OF_SSID);
    }

    wpa_supplicant_task_init(config, config_ext);
}


static bool wifi_security_valid = false;
bool wifi_get_security_valid(void)
{
    return wifi_security_valid;
}

void wifi_set_security_valid(bool value)
{
    wifi_security_valid = value;
    return;
}

/***************** Just for internal use **********************/
#include "hal_sleep_manager.h"
uint8_t wifi_set_sleep_handle(const char *handle_name)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_set_sleep_handle(handle_name);
#else
    return 0xff;
#endif
}

int32_t wifi_lock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_lock_sleep(handle_index);
#else
    return -1;
#endif
}

int32_t wifi_unlock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_unlock_sleep(handle_index);
#else
    return -1;
#endif
}

int32_t wifi_release_sleep_handle(uint8_t handle)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_release_sleep_handle(handle);
#else
    return -1;
#endif
}
/***************** Just for internal use **********************/
