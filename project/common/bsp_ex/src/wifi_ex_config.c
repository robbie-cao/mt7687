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
    @file       wifi_ex_config.c
    @brief      WiFi - Configure API Examples

    @history    2015/08/07  Initial for 1st draft  (Michael Rong)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toi.h>


#include "os.h"
#include "os_util.h"
#include "cli.h"
#include "type_def.h"
#include "connsys_driver.h"
#include "connsys_util.h"
#include "wifi_api.h"
#include "wifi_inband.h"
#include "wifi_private_api.h"
#include "wifi_scan.h"
#include "wifi_homekit.h"
#include "misc.h" //"wifi_ex_profile.h"
#include "ethernet_filter.h"

#include "lwip/netif.h"
#include "lwip/inet.h"
#include "dhcpd.h"
#include "lwip/dhcp.h"
#include "lwipopts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"

#include "nvdm.h"

#if defined(MTK_MINICLI_ENABLE)
#include "cli_def.h"
#endif
#if defined(MTK_SMTCN_ENABLE)
#include "smt_conn.h"
#endif
#include "network_init.h"
#include "wifi_inband.h"
#include "inband_queue.h"
#include "get_profile_string.h"
#include "driver_inband.h"
#include "wifi_rx_desc.h"
#include "wifi_cli.h"


#if 0
/**
* @brief Example of Configure MAC address for STA/AP wireless port
*
* wifi config set mac <port> <mac_addr>   00:0c:43:76:87:00   --> set port0 (STA) MAC address=00:0c:43:76:87:00
*
* @parameter
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_set_mac_address_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    unsigned int  addr[WIFI_MAC_ADDRESS_LENGTH] = {0};
    unsigned char addr2[WIFI_MAC_ADDRESS_LENGTH] = {0};
    char port = port_sanity_check(param[0]);

    if (port < 0) {
        return 1;
    }

    sscanf(param[1], "%x:%x:%x:%x:%x:%x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);

    for (i = 0; i < WIFI_MAC_ADDRESS_LENGTH; i++) {
        addr2[i] = addr[i];
    }

    status = wifi_config_set_mac_address(port, addr2);

    printf("wifi_config_set_mac_address(port%d): (%02x:%02x:%02x:%02x:%02x:%02x), status:%d\n",
           port,
           addr2[0], addr2[1], addr2[2],
           addr2[3], addr2[4], addr2[5],
           status);
    return 0;
}
#endif


static uint8_t wifi_config_get_rssi_threshold_ex(uint8_t len, char *param[])
{
    uint8_t enable = 0;
    int8_t rssi = 0;

    if(wifi_inband_get_rssi_threshold(&enable, &rssi) >= 0){
        printf("wifi_inband_get_rssi_threshold: success\n");
        printf("rssi threthold: [enabled: %d], [value: %d]\n", enable, rssi);
    } else {
        printf("wifi_inband_get_rssi_threshold: failed\n");
    }

    return 0;
}

static uint8_t wifi_config_set_rssi_threshold_ex(uint8_t len, char *param[])
{
    uint8_t enable = 0;
    int8_t rssi = 0;

    enable = (uint8_t)atoi(param[0]);
    if((len == 1) && (enable != 0)){
        printf("usage: wifi config set rssi_threshold <enabled> <rssi value>\n");
        return 1;
    }
    rssi = (int8_t)atoi(param[1]);

    wifi_inband_set_rssi_threshold(enable, rssi);
    return 0;
}
static int32_t wifi_frame_type_event_handler(wifi_event_t event,
                                                    uint8_t *payload,
                                                    uint32_t length)
{
    uint8_t type = 0;
    if(payload == NULL) {
        LOG_E(wifi, "payload is empty!");
        return -1;
    }
    LOG_HEXDUMP_E(wifi, "event id: %d", payload, length, event);
    type = payload[0]>>4;
    switch (type) {
        case 0: printf("frame type is assoc req\r\n");break;
        case 1: printf("frame type is assoc resp\r\n");break;
        case 2: printf("frame type is reassoc req\r\n");break;
        case 3: printf("frame type is reassoc resp\r\n");break;
        case 4: printf("frame type is probe req\r\n");break;
        case 5: printf("frame type is probe resp\r\n");break;
        case 8: printf("frame type is beacon\r\n");break;
        case 9: printf("frame type is atim\r\n");break;
        case 10: printf("frame type is disassoc\r\n");break;
        case 11: printf("frame type is auth\r\n");break;
        case 12: printf("frame type is deauth\r\n");break;
        default:
            printf("wrong management frame\r\n");
            break;
    }
    return 1;
}
/**
* @brief Example of set frame filter for packets format wanted to be received
* wifi config set frame_filter 1 8 221 0 15 172
* @parameter
* @return >=0 means success, <0 means fail
*/
static uint8_t wifi_config_set_frame_filter_ex(uint8_t len, char *param[])
{
    uint8_t enable = 0;
    uint16_t frame_type = 0;
    extra_vender_ie_t vender_ie = {0};
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_REPORT_FILTERED_FRAME, wifi_frame_type_event_handler);
    enable = (uint8_t)atoi(param[0]);
    frame_type = (uint16_t)atoi(param[1]);
    if (2 == len) {
        wifi_config_set_frame_filter(enable, frame_type, NULL);
    }
    else {
        vender_ie.element_id = (uint8_t)atoi(param[2]);
        vender_ie.OUI[0] = (uint8_t)atoi(param[3]);
        vender_ie.OUI[1] = (uint8_t)atoi(param[4]);
        vender_ie.OUI[2] = (uint8_t)atoi(param[5]);
        wifi_config_set_frame_filter(enable, frame_type, &vender_ie);
    }
    return 0;
}
#if 0

int demo_rcv(unsigned char *payload, unsigned int len)
{
};

/**
*  Example of Set the handler to receive 802.11 raw packet from network processor
*  The network processor will begin to indicate 802.11 raw packet with RXWI
*
* @parameter
*   [IN] receiver: handler routine
* @return >=0 means success, <0 means fail
* @note if handler == NULL, then network processor will stop to indicate 802.11 raw packet
*/
static uint8_t wifi_set_raw_pkt_receiver_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint32_t flag = atoi(param[0]);

    status = wifi_config_set_rx_filter(flag);
    status =  wifi_set_raw_pkt_receiver(&demo_rcv);
    printf("wifi_config_set_rxfilter: 0x%x, status:%d\n",
           flag,   status);

    return status;
}


/**
*  Example of TX 802.11 raw packet sender routine (to WiFi MAC hardware directly)
*
* @parameter
*   [IN] buf: payload of 802.11 raw packet, need to include hardware TXWI and PSE header
*   [IN] len: the length of payload
* @return =0 means success, <0 means fail
* @note Please refer to related document for TXWI and PSE header format
*/
static uint8_t wifi_raw_pkt_sender_ex(uint8_t len, char *param[])
{
    int ret = 0;

    ret = ethernet_raw_pkt_sender(buf, len);
    return ret;
}
#endif

#define EXT_SEC_CMD_SET_AUTH_MODE    0x00
#define EXT_SEC_CMD_SET_ENCR_TYPE    0x01
#define EXT_SEC_CMD_SET_WPAPSK       0x02
#define EXT_SEC_CMD_SET_WEP_KEY      0x03
#define EXT_SEC_CMD_MAX              0x04

typedef struct _EXT_SECURITY_CMD_T {
    UINT16     u2ExtCmdID;
    UINT16     u2DataLen;
    UINT      aucDataBuffer[1];
} EXT_SECURITY_CMD_T, *P_EXT_SECURITY_CMD_T;




#if 0
/**
* @brief Example of Get WiFi BSSID.
* wifi config get bssid <port>
*
* @param [OUT]bssid BSSID
*
* @return  >=0 means success, <0 means fail
*
* @note Only used for WIFI_PORT_STA
*/
static uint8_t wifi_config_get_bssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t addr[WIFI_MAC_ADDRESS_LENGTH] = {0};
    /*
    char port = port_sanity_check(param[0]);

    if (port < 0) return 1;

    status = wifi_config_get_bssid(port, addr);
    */
    status = wifi_config_get_bssid(addr);

    printf("wifi_config_get_bssid(port%d): (%02x:%02x:%02x:%02x:%02x:%02x), status:%d\n",
           WIFI_PORT_STA,
           addr[0], addr[1], addr[2],
           addr[3], addr[4], addr[5],
           status);
    return 0;
}


/**
* @brief Example of Set WiFi BSSID.
* wifi config set bssid <port> <BSSID> --> "00:0c:43:76:87:00"
*
* @param [IN]bssid BSSID
*
* @return  >=0 means success, <0 means fail
*
* @note Only used for WIFI_PORT_STA
*/
static uint8_t wifi_config_set_bssid_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    unsigned int addr[WIFI_MAC_ADDRESS_LENGTH] = {0};
    uint8_t addr2[WIFI_MAC_ADDRESS_LENGTH] = {0};
    /*
    char port = port_sanity_check(param[0]);

    if (port < 0) return 1;
    */

    sscanf(param[0], "%x:%x:%x:%x:%x:%x", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);

    for (i = 0; i < WIFI_MAC_ADDRESS_LENGTH; i++) {
        addr2[i] = addr[i];
    }

    //status = wifi_config_set_bssid(port, addr2);
    status = wifi_config_set_bssid(addr2);

    printf("wifi_config_set_bssid(port%d): (%02x:%02x:%02x:%02x:%02x:%02x), status:%d\n",
           WIFI_PORT_STA,
           addr2[0], addr2[1], addr2[2],
           addr2[3], addr2[4], addr2[5],
           status);
    return 0;
}
#endif


extern int32_t wifi_config_get_scan_table_size(uint8_t *size);


/**
* @brief Example of Get WiFi Scan Table Size
* wifi config get scantabsize
* @param [OUT]num 1 ~ 255
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_get_scan_tab_size_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t size = 0;

    status = wifi_config_get_scan_table_size(&size);
    printf("wifi_config_get_scan_tab_size size:%d, status:%d\n", size, status);
    return status;
}


int32_t wifi_config_set_scan_table_size(uint8_t size);


/**
* @brief Example of Set WiFi Scan Table Size
* wifi config set scantabsize <size>
* @param [IN]num 1 ~ 255
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_set_scan_tab_size_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t size = atoi(param[0]);

    status = wifi_config_set_scan_table_size(size);
    printf("wifi_config_set_scan_tab_size, size:%d, status:%d\n", size, status);
    return status;
}

/**
* @brief Set N9 Debug Level
* wifi config set n9dbg <dbg_level>
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
static uint8_t wifi_config_set_n9_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dbg_level = atoi(param[0]);

    status = (uint8_t)wifi_inband_set_debug_level(dbg_level);
    printf("wifi_config_set_n9_dbg_level, level = %d, status:%d\n", dbg_level, status);

    return status;
}


/**
* @brief Set CM4 Debug Level
* wifi config set cm4dbg <dbg_level>
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
static uint8_t wifi_config_set_cm4_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dbg_level = atoi(param[0]);

    // fixme, try to leverage mlog module
    printf("[Not Ready] wifi_config_set_cm4_dbg_level, level = %d, status:%d\n", dbg_level, status);

    return status;
}

/**
* @brief Get N9 Debug Level
* wifi config get n9dbg
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
static uint8_t wifi_config_get_n9_dbg_level(uint8_t len, char *param[])
{
    uint8_t status    = 0;
    uint8_t dbg_level = 0;

    status = (uint8_t)wifi_inband_get_debug_level(&dbg_level);

    printf("wifi_config_get_n9_dbg_level, level = %d, status:%d\n",
           dbg_level, status);

    return status;
}


/**
* @brief Get CM4 Debug Level
* wifi config get cm4dbg
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
static uint8_t wifi_config_get_cm4_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dbg_level = 0;

    // fixme, try to leverage mlog module
    printf("[Not Ready] wifi_config_get_cm4_dbg_level, level = %d, status:%d\n", dbg_level, status);

    return status;
}

/**
* @brief Enable/Disable Apple Airplay
* wifi config set airplay <enable>
* @param [IN]enable
*   0: Disable
*   1: Enable
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_set_airplay(uint8_t len, char *param[])
{
    uint8_t status = 0;
#if defined(MTK_HOMEKIT_ENABLE)
    uint8_t enable = atoi(param[0]);
#endif

#if defined(MTK_HOMEKIT_ENABLE)
    status = (uint8_t)wifi_enable_airplay(enable);
    printf("wifi_config_set_airplay, enable = %d, status:%d\n", enable, status);
#else
    printf("wifi_config_set_airplay, MTK_HOMEKIT_ENABLE not supported\n");
#endif
    return status;
}

/**
* @brief Update Apple IE
* wifi config set appleie <is_unconfigured>
* @param is_unconfigured
*   0: configured
*   1: unconfigured
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_config_set_apple_ie(uint8_t len, char *param[])
{
    uint8_t status = 0;
#if defined(MTK_HOMEKIT_ENABLE)
    uint8_t unconfigured = atoi(param[0]);
    INT32 ret = 0;
    uint8_t mac[WIFI_MAC_ADDRESS_LENGTH];
    uint8_t *ie = NULL;
    size_t ie_len;
    char name[32];
    char model[32];
    char manufacturer[32];

    os_memset(name, 0, 32);
    os_memset(model, 0, 32);
    os_memset(manufacturer, 0, 32);

    os_memcpy(name, "msound", os_strlen("msound"));
    os_memcpy(model, "MT7687", os_strlen("MT7687"));
    os_memcpy(manufacturer, "MediaTek", os_strlen("MediaTek"));

    ret = wifi_config_get_mac_address(WIFI_PORT_AP, mac);
    ret = wifi_create_homekit_ie(name, model, manufacturer, mac,
                                 unconfigured, // isUnconfigured,
                                 1, // supportsAirPlay,
                                 0, // supportsAirPrint,
                                 0, // supportsMFiConfigurationV1,
                                 1, // supportsWakeOnWireless,
                                 1, // supports2_4GHzNetworks,
                                 0, // supports5GHzNetworks,
                                 (uint8_t **)&ie, &ie_len);
    if (ret != 0  || ie == NULL) {
        printf("Create homekit IE fail: %d, ie:%p \n", ret, ie);
        return -1;
    }

    //status = (uint8_t)wifi_update_apple_ie(unconfigured);
    status = (uint8_t)wifi_update_homekit_ie(ie, ie_len);

    os_free(ie);
    printf("wifi_config_set_apple_ie, unconfigured = %d, status:%d\n", unconfigured, status);
#else
    printf("wifi_config_set_apple_ie, MTK_HOMEKIT_ENABLE not supported\n");
#endif

    return status;
}


#ifdef MTK_MINISUPP_ENABLE
static uint8_t _cli_supp_reload(uint8_t len, char *param[])
{
    wifi_config_reload_setting();
    return 0;
}

static uint8_t _cli_supp_show_ifaces(uint8_t len, char *param[])
{
    if (__g_wpa_supplicant_api.show_ifaces) {
        __g_wpa_supplicant_api.show_ifaces();
    } else {
        printf("NOT support show ifaces.\n");
    }
    return 0;
}

static uint8_t _cli_supp_wps_pbc(uint8_t len, char *param[])
{
    int ret = 0;
    if (__g_wpa_supplicant_api.do_wps_pbc) {
        ret = __g_wpa_supplicant_api.do_wps_pbc(param[0]);
        if (ret < 0) {
            printf("wps_pbc fail(=%d).", ret);
        }
    } else {
        printf("NOT support wps pbc.\n");
    }
    return 0;
}

static uint8_t _cli_supp_wps_pin(uint8_t len, char *param[])
{
    char buf[9]  = { 0 };
    int  buf_len = 9;
    int  ret     = 0;

    if (__g_wpa_supplicant_api.do_wps_pin) {
        ret = __g_wpa_supplicant_api.do_wps_pin((char *)param[0], &buf[0], buf_len);
        if (ret < 0) {
            printf("wps_pin fail(=%d).", ret);
        }
    } else {
        printf("NOT support wps pin.\n");
    }
    return 0;
}

static uint8_t _cli_supp_wps_reg(uint8_t len, char *param[])
{
    int  ret     = 0;

    if (__g_wpa_supplicant_api.do_wps_reg_pin) {
        ret = __g_wpa_supplicant_api.do_wps_reg_pin((char *)param[0]);
        if (ret < 0) {
            printf("wps_reg fail(=%d)\n", ret);
        }
    } else {
        printf("NOT support wps pbc.\n");
    }
    return 0;
}

static uint8_t _cli_supp_wps_ap_pin(uint8_t len, char *param[])
{
    char buf[9]  = { 0 };
    int  buf_len = 9;
    int  ret     = 0;

    if (__g_wpa_supplicant_api.wps_ap_pin) {
        ret = __g_wpa_supplicant_api.wps_ap_pin(param[0], &buf[0], buf_len);
        if (ret >= 0) {
            printf("\n\nWPS PIN: %s\n", buf);
        } else {
            printf("wps_pin fail(=%d).", ret);
        }
    } else {
        printf("NOT support wps pin.\n");
    }
    return 0;
}

static uint8_t _cli_supp_wps_get_pin(uint8_t len, char *param[])
{
    char buf[9]  = {0};
    int  buf_len = 9;
    int  ret     = 0;

    if (__g_wpa_supplicant_api.wps_get_pin) {
        ret = __g_wpa_supplicant_api.wps_get_pin((char *)param[0], &buf[0], buf_len);
        if (ret >= 0) {
            printf("\n\nWPS PIN: %s\n", buf);
        } else {
            printf("wps_get_pin fail(=%d).", ret);
        }
    } else {
        printf("NOT support wps pin.\n");
    }
    return 0;
}

static uint8_t _cli_supp_wps_device_info(uint8_t len, char *param[])
{
    int  ret     = 0;

#if defined(MTK_WIFI_WPS_ENABLE)
    ret = mtk_supplicant_ctrl_wps_device_info((char *)param[0]);
    if (ret < 0) {
        LOG_E(minisupp, "wps_devinfo fail\n");
    }
#else
    LOG_W(minisupp, "NOT support wps_devinfo\n");
#endif
    return ret;
}

static uint8_t _cli_supp_wps_crdential_handler(uint8_t len, char *param[])
{
    int  ret     = 0;

#if defined(MTK_WIFI_WPS_ENABLE)
    ret = mtk_supplicant_ctrl_wps_set_credential_handler((char *)param[0]);
    if (ret < 0) {
        LOG_E(minisupp, "wps_cred_handler control fail\n");
    }
#else
    LOG_E(minisupp, "NOT support wps_cred_handler control\n");
#endif
    return ret;
}


static uint8_t _cli_supp_wps_auto_connection(uint8_t len, char *param[])
{
    int  ret     = 0;

#if defined(MTK_WIFI_WPS_ENABLE)
    ret = mtk_supplicant_ctrl_wps_auto_conneciton((char *)param[0]);
    if (ret < 0) {
        LOG_E(minisupp, "wps_auto_connection control fail\n");
    }
#else
    LOG_E(minisupp, "NOT support wps_auto_connection control\n");
#endif
    return ret;
}


cmd_t   wifi_priv_cli[] = {
    { "reload",         "", _cli_supp_reload,       NULL },
    { "show_ifaces",    "", _cli_supp_show_ifaces,  NULL },
    { "wps_pbc",        "", _cli_supp_wps_pbc,      NULL },
    { "wps_pin",        "", _cli_supp_wps_pin,      NULL },
    { "wps_reg",        "", _cli_supp_wps_reg,      NULL },
    { "wps_ap_pin",     "", _cli_supp_wps_ap_pin,   NULL },
    { "wps_get_pin",    "", _cli_supp_wps_get_pin,  NULL },
    { "wps_devinfo",    "", _cli_supp_wps_device_info,  NULL },
    { "wps_cred_handler",    "", _cli_supp_wps_crdential_handler,  NULL },
    { "wps_auto_connection",   "", _cli_supp_wps_auto_connection,  NULL },
    { NULL, NULL, NULL, NULL },
};
#endif


static void dump_scan_param(CMD_IOT_START_SCAN_T *scan_param)
{
    int i;

    printf("partial=0x%02X\n", scan_param->partial);
    printf("scan_mode=0x%02X\n", scan_param->scan_mode);
    printf("partial_bcn_cnt=0x%02X\n", scan_param->partial_bcn_cnt);
    printf("periodic_bcn_cnt=0x%02X\n", scan_param->periodic_bcn_cnt);

    printf("BSSID=");
    for (i = 0; i < 5; i++) {
        printf("%02X:", scan_param->BSSID[i]);
    }
    printf("%02X\n", scan_param->BSSID[i]);
    printf("enable_bssid =0x%02X\n", scan_param->enable_bssid);

    printf("SSID='%s'\n", scan_param->SSID);
    printf("enable_ssid =0x%02X'\n", scan_param->enable_ssid);

    if (((scan_param->channel_list).num_bg_band_triplet) > 0) {
        printf("num_bg_band_triplet=%d\n", ((scan_param->channel_list).num_bg_band_triplet));
        printf("1st_Ch\tNum_Ch\tCh_Prop\tReserve\n");
        for (i = 0; i < ((scan_param->channel_list).num_bg_band_triplet); i++) {
            printf("%d\t%d\t0x%02X\t0x%02X\n", ((scan_param->channel_list).triplet[i].first_channel),
                   ((scan_param->channel_list).triplet[i].num_of_ch),
                   ((scan_param->channel_list).triplet[i].channel_prop),
                   ((scan_param->channel_list).triplet[i].reserve));
        }
    }

    if (((scan_param->channel_list).num_a_band_triplet) > 0) {
        printf("num_a_band_triplet=%d\n", ((scan_param->channel_list).num_a_band_triplet));
        printf("1st_Ch\tNum_Ch\tCh_Prop\tReserve\n");
        for (i = 0; i < ((scan_param->channel_list).num_a_band_triplet); i++) {
            printf("%d\t%d\t0x%02X\t0x%02X\n", ((scan_param->channel_list).triplet[i + ((scan_param->channel_list).num_bg_band_triplet)].first_channel),
                   ((scan_param->channel_list).triplet[i + ((scan_param->channel_list).num_bg_band_triplet)].num_of_ch),
                   ((scan_param->channel_list).triplet[i + ((scan_param->channel_list).num_bg_band_triplet)].channel_prop),
                   ((scan_param->channel_list).triplet[i + ((scan_param->channel_list).num_bg_band_triplet)].reserve));
        }
    }
}

uint8_t _cli_scan(uint8_t len, char *param[])
{
#ifdef DEF_HELP_MSG
#undef DEF_HELP_MSG
#endif
#define DEF_HELP_MSG "Usage:\n scan <partial> <scan_mode> <partial_bcn_cnt> <periodic_bcn_cnt> <BSSID/StayTime> <SSID> <BG_channel_list> <A_channel_list>\n"
    CMD_IOT_START_SCAN_T scan_param;
    CMD_IOT_START_SCAN_T *p_scan_param = &scan_param;
    unsigned int length = sizeof(scan_param);
    uint16_t  stay_time = 0;

    ch_desc_t bg_triplet[10];
    ch_desc_t a_triplet[10];


    os_memset(&scan_param, 0x0, sizeof(scan_param));

    switch (len) {
        case 8:
            if (os_strcmp(param[6] , "0")){
                scan_param.channel_list.num_bg_band_triplet = wifi_conf_get_ch_table_from_str((ch_desc_t *)bg_triplet, 10, (const char *)param[6], os_strlen(param[6]));
            }else{
                printf("Ignore Parameter 6: BGChannelTable\n");
            }

            if (os_strcmp(param[7] , "0")){
                scan_param.channel_list.num_a_band_triplet = wifi_conf_get_ch_table_from_str((ch_desc_t *)a_triplet, 10, (const char *)param[7], os_strlen(param[7]));
            }else{
                printf("Ignore Parameter 7: AChannelTable\n");
            }

            length = (sizeof(scan_param) + (sizeof(ch_desc_t) * (scan_param.channel_list.num_bg_band_triplet + scan_param.channel_list.num_a_band_triplet)));
            p_scan_param = (CMD_IOT_START_SCAN_T *)os_malloc(length);
            os_memset((unsigned char *)(p_scan_param), 0x0, length);

            p_scan_param->partial = strtol(param[0], NULL, 16);
            p_scan_param->scan_mode = strtol(param[1], NULL, 16);
            p_scan_param->partial_bcn_cnt = strtol(param[2], NULL, 16);
            p_scan_param->periodic_bcn_cnt = strtol(param[3], NULL, 16);


            if (os_strcmp(param[5] , "0")){
                p_scan_param->enable_ssid = (0x80 | os_strlen(param[5]));
                os_memcpy(p_scan_param->SSID, param[5], os_strlen(param[5]));
            }else{
                printf("Ignore Parameter 5: SSID\n");
            }

            if (os_strcmp(param[4] , "0")){  /*if BSSID is 0,   not set bssid filed*/
                if (os_strlen(param[4]) == 17)  { /*match the BSSID mac address length*/
                    p_scan_param->enable_bssid = 1;
                    wifi_conf_get_mac_from_str((char *)p_scan_param->BSSID, param[4]);
                    printf("Parameter 4 is BSSID\n");
                }else{
                    stay_time = (uint16_t)strtol(param[4], NULL, 16);
                    if(stay_time != 0){
                        p_scan_param->enable_ssid |= BIT(6);
                        p_scan_param->BSSID[0] = 0xff & stay_time;
                        p_scan_param->BSSID[1] = stay_time >> 8;
                        printf("Parameter 4 is StayTime\n");
                    }
                }
            }else{
                printf("Ignore Parameter 4: BSSID/StayTime\n");
            }


            if (os_strcmp(param[6] , "0")){
                (p_scan_param->channel_list).num_bg_band_triplet = scan_param.channel_list.num_bg_band_triplet;
                os_memcpy((p_scan_param->channel_list).triplet, bg_triplet, sizeof(ch_desc_t) * (scan_param.channel_list.num_bg_band_triplet));
            }else{
                printf("Ignore Parameter 6: BGChannelTable\n");
            }

            if (os_strcmp(param[7] , "0")){
            (p_scan_param->channel_list).num_a_band_triplet = scan_param.channel_list.num_a_band_triplet;
            os_memcpy(&((p_scan_param->channel_list).triplet[(scan_param.channel_list.num_bg_band_triplet)]), a_triplet, sizeof(ch_desc_t) * (scan_param.channel_list.num_a_band_triplet));
            }else{
                printf("Ignore Parameter 7: AChannelTable\n");
            }

            break;

        default:
            printf(DEF_HELP_MSG);
            return 1;
    }
    dump_scan_param(p_scan_param);
    hex_dump("p_scan_param", (unsigned char *)p_scan_param, length);
    inband_queue_exec(WIFI_COMMAND_ID_IOT_START_SCAN, (unsigned char *)(p_scan_param), length, TRUE, FALSE);
    //wifi_inband_scan_custom(p_scan_param, sizeof(*p_scan_param));

    if ((&scan_param) != p_scan_param) {
        os_free((void *)p_scan_param);
    }

    return 0;
}
#ifdef MTK_AP_SNIFFER_ENABLE
uint8_t _cli_ap_sniffer(uint8_t len, char *param[])
{
    switch (len) {
    case 1:
        if (!os_strcmp(param[0], "stop")) {
            wifi_exit_ap_sniffer_mode();
        } else {
            printf("unknown cli\n");
        }
        break;
    case 3:
        if (!os_strcmp(param[0], "start")) {
            uint16_t stay_time, optime;
            stay_time = atoi(param[1]);
            optime = atoi(param[2]);
            ap_sniffer_test(stay_time, optime);
        } else {
            printf("unknown cli\n");
        }
            break;
    }
}
#endif

uint8_t _cli_wow(uint8_t len, char *param[])
{
    if (len < 2) {
        printf("Usage: \n");
        printf("wow enable <enable ex.0/1>\n");
        printf("wow magic <enable ex.0/1> <bss_index ex.1/3...>\n");
        printf("wow bitmap <16 bytes bitmap mask> <128 bytes bitmap pattern>\n");
        printf("wow arp <ip_addr ex.192> <ip_addr ex.168> <ip_addr ex.0> <ip_addr ex.1>\n");
        printf("wow rekey <64 bytes ptk>\n");
        return 0;
    }

    if (!os_strcmp((char *)param[0], "enable")) {
        wifi_wow_enable_t wow_param;
        wow_param.enable = atoi(param[1]);

        wifi_inband_wow_enable(&wow_param);
        return 0;
    } else if (!os_strcmp((char *)param[0], "magic")) {
        wifi_wow_magic_packet_t wow_param;
        wow_param.enable = atoi(param[1]);
        wow_param.bss_index = strtol(param[2], NULL, 16);

        wifi_inband_wow_set_magic_packet(&wow_param);
        return 0;
    } else if (!os_strcmp((char *)param[0], "bitmap")) {
        wifi_wow_bitmap_pattern_t wow_param;
        os_memcpy(wow_param.bitmap_mask, param[1], sizeof(wow_param.bitmap_mask));
        os_memcpy(wow_param.bitmap_pattern, param[2], sizeof(wow_param.bitmap_pattern));

        wifi_inband_wow_set_bitmap_pattern(&wow_param);
        return 0;
    } else if (!os_strcmp((char *)param[0], "arp")) {
        wifi_wow_arp_offload_t wow_param;
        wow_param.ip_address[0] = atoi(param[1]);
        wow_param.ip_address[1] = atoi(param[2]);
        wow_param.ip_address[2] = atoi(param[3]);
        wow_param.ip_address[3] = atoi(param[4]);

        wifi_inband_wow_set_arp_offload(&wow_param);
        return 0;
    } else if (!os_strcmp((char *)param[0], "rekey")) {
        wifi_wow_rekey_offload_t wow_param;
        os_memcpy(wow_param.ptk, param[1], sizeof(wow_param.ptk));

        wifi_inband_wow_set_rekey_offload(&wow_param);
        return 0;
    }

    printf("Usage: \n");
    printf("wow enable <enable ex.0/1>\n");
    printf("wow magic <enable ex.0/1> <bss_index ex.1/3...>\n");
    printf("wow bitmap <16 bytes bitmap mask> <128 bytes bitmap pattern>\n");
    printf("wow arp <ip_addr ex.192> <ip_addr ex.168> <ip_addr ex.0> <ip_addr ex.1>\n");
    printf("wow rekey <64 bytes ptk>\n");

    return 0;
}

uint8_t _cli_pm(uint8_t len, char *param[])
{
    wifi_power_manage_state_t pm_state;

    if (len < 2) {
        printf("Usage: \n");
        printf("pm <pm number> <pm state>\n");
        return 0;
    }

    pm_state.power_manage_number = atoi(param[0]);
    pm_state.power_manage_state = atoi(param[1]);

    wifi_inband_pm_set_state(&pm_state);

    return 0;
}

uint8_t _cli_connsys_related(uint8_t len, char *param[])
{
    return connsys_util_cli_handler(len, param);
}

#if 0
uint8_t _cli_wifi_auto_connect(uint8_t len, char *param[])
{
    char ssid[32] = "MTK_AP";
    int sz = sizeof(ssid);
    nvdm_read_data_item("STA", "Ssid", (uint8_t *)ssid, (uint32_t *)&sz);
    /* TODO: call inband commands
    wifi_connect_connection(ssid, os_strlen(ssid));
    */
    //wifi_connect_start_scan_from_isr();
    wifi_connection_start_scan(NULL, 0, NULL, 0, 0);

    return 0;
}
#endif


#if defined(MTK_AWS_ENABLE)
extern void aws_test_start(void);
uint8_t _aws_test(uint8_t len, char *param[])
{
    if (len == 1) {
        if (!os_strcmp(param[0], "start")){
            aws_test_start();
        }/* else if(!os_strcmp(param[0], "stop")){
            mtk_smart_stop();
        } */else{
            printf("Not supported cmd\n");
        }
    }

    return 1;
}
#endif
#ifdef MTK_MINISUPP_ENABLE
extern void wpa_supplicant_entry_debug_level(int debug_level);
#endif

#ifdef MTK_MINISUPP_ENABLE
extern int wpa_debug_level;
#endif

#define REG32(x)                (*(volatile unsigned int *)(x))

uint8_t _cli_wifi(uint8_t len, char *param[])
{
    switch (atoi(param[0])) {
        case 0: {
            uint8_t link_status = 0;
            wifi_connection_get_link_status(&link_status);
            printf("wifi_conn_get_link_status() %d\n", link_status);
            break;
        }

        case 1: {
            printf("wifi_conn_start_scan()\n");
            //wifi_connect_start_scan_from_isr();
            wifi_connection_start_scan(NULL, 0, NULL, 0, 0);
            break;
        }

        case 2: {
            printf("wifi_conn_stop_scan()\n");
            wifi_connection_stop_scan();
            break;
        }

        case 3: {
            size_t length = os_strlen(param[2]);
            uint8_t  buf[WIFI_MAX_LENGTH_OF_SSID + 2] = {0};
            printf("wifi_conn_connect(port:%s, ssid:%s), len(%d)\n", param[1], param[2], length);

            buf[0] = atoi(param[1]);
            buf[1] = length;  /* bit7 - 0: normal connection, 1: wps connection  */
            os_memcpy(buf + 2, param[2], (length & 0x7F));
            inband_queue_exec(WIFI_COMMAND_ID_IOT_SSID, (unsigned char *)buf, length + 2, TRUE, FALSE);
            /* TODO: call inband command             */
            //wifi_connect_connection(param[1], len);
            break;
        }

        case 4: {
            printf("wifi_conn_disconnect()\n");
            wifi_connection_disconnect_ap();
            break;
        }

        case 5: {
            inband_queue_exec(WIFI_COMMAND_ID_IOT_SWITCH_MODE, (unsigned char *)param[1], 1, TRUE, FALSE);
            break;
        }

        case 7: {
            uint8_t  type;
            uint32_t reg, value;

            reg = toi(param[1], &type);

            if (type == TOI_ERR) {
                printf("Connsys Read fail!\n");
            } else {
                connsys_cr_read(reg, &value);
                printf("CR[0x%x]=0x%x\n", (unsigned int)reg, (unsigned int)value);
            }
            break;
        }

        case 8: {
            uint8_t  type;
            uint32_t reg, value;

            reg = toi(param[1], &type);
            if (type == TOI_ERR) {
                printf("Connsys write addr fail!\n");
            } else {
                value = toi(param[2], &type);
                if (type == TOI_ERR) {
                    printf("Connsys write value fail!\n");
                }

                connsys_cr_write(reg, value);
                printf("Write CR[0x%x]=0x%x\n", (unsigned int)reg, (unsigned int)value);
            }
            break;
        }

        case 9: {
            uint8_t  type;
            uint32_t reg, value;

            reg = toi(param[1], &type);

            if (type == TOI_ERR) {
                printf("CM4 Read fail!\n");
            } else {
                value = REG32(0x50200000 + reg);
                printf("CR[0x%x]=0x%x\n", (unsigned int)(0x50200000 + reg), (unsigned int)value);
            }
            break;
        }

        case 10: {
            /*
            //unsigned char addr[6]={0x00, 0x0c, 0x43, 0x76, 0x87, 0x30};
            int ret = wifi_config_set_mac_address(WIFI_PORT_STA, g_sta_own_addr);
            printf("Set MAC ADDR(0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X) Done, ret=%d\n",
                g_sta_own_addr[0], g_sta_own_addr[1], g_sta_own_addr[2],
                g_sta_own_addr[3], g_sta_own_addr[4], g_sta_own_addr[5], ret);
            */
            printf("Not support change MAC ADDR dynamically!\n");
            break;
        }

#ifdef MTK_MINISUPP_ENABLE
        case 12: {
            printf("SUPP debug level: %d\n", wpa_debug_level);
            break;
        }

        case 13: {
            if (param[1]) {
                wpa_supplicant_entry_debug_level(atoi(param[1]));
            } else {
                printf("Please assign debug level(0~6).\n");
            }
            break;
        }
#endif /* MTK_MINISUPP_ENABLE */

#if defined(MTK_BSP_LOOPBACK_ENABLE)
        case 15: {
            uint8_t link_status;

            if (loopback_start()) {
                loopback_start_set(0);
            } else {
                loopback_start_set(1);
            }

            /* use get link status to toggle loopback cmd for N9 */
            wifi_inband_get_link_status(&link_status);

            break;
        }
#endif // MTK_BSP_LOOPBACK_ENABLE

        default: {
            break;
        }
    }

    return 0;
}


uint8_t _cli_n9_consol_log(uint8_t len, char *param[])
{
    if (len < 2) {
        printf("Usage: \n");
        printf("n9log set off  -> Disable n9 logs\n");
        printf("n9log set n9   -> output n9 logs to uart\n");
        printf("n9log set host -> output n9 logs to host\n");
        return 0;
    }

    if (!os_strcmp(param[0], "set")){
        if (!os_strcmp(param[1], "off")){
            if (wifi_inband_set_n9_consol_log_state(0) < 0) {
                printf("n9log set off fail\n");
                return 1;
            }
        } else if (!os_strcmp(param[1], "n9")){
            if (wifi_inband_set_n9_consol_log_state(1) < 0) {
                printf("n9log set to n9 uart fail\n");
                return 1;
            }
        } else if (!os_strcmp(param[1], "host")){
            if (wifi_inband_set_n9_consol_log_state(2) < 0) {
                printf("n9log set to host fail\n");
                return 1;
            }
        } else {
            printf("Not Supported Cmd\n");
        }
    } else {
        printf("Not Supported Cmd\n");
    }

    return 0;
}

cmd_t   wifi_config_set_cli[] = {
    { "opmode",         "STA/AP/Dual",      wifi_config_set_opmode_ex},
    //{ "mac",          "MAC address",      wifi_config_set_mac_address_ex},
    { "ssid",           "SSID",             wifi_config_set_ssid_ex   },
    //{ "bssid",            "BSSID",            wifi_config_set_bssid_ex      },
    { "psk",            "WPA/WPA2PSK",      wifi_config_set_psk_ex    },
    { "pmk",            "PMK for WPA/WPA2PSK",  wifi_config_set_pmk_ex    },
    { "wep",            "WEP key",          wifi_config_set_wep_key_ex },
    { "rxfilter",       "RX filter",        wifi_config_set_rx_filter_ex},
    { "mtksmart",       "MTK smart connection", wifi_config_set_mtk_smartconnection_filter_ex},
    { "rxraw",          "set RX RAW packet",    wifi_config_set_rx_raw_pkt_ex},
    { "radio",          "RADIO on/off",     wifi_config_set_radio_on_ex},
    { "ch",             "channel",          wifi_config_set_channel_ex},
    { "bw",             "bandwidth",        wifi_config_set_bandwidth_ex},
    { "bw_extended",    "bandwidth",        wifi_config_set_bandwidth_extended_ex},
    { "wirelessmode",   "wireless mode",    wifi_config_set_wireless_mode_ex},
    { "country",        "country region",   wifi_config_set_country_region_ex},
    { "dtim",           "DTIM interval",    wifi_config_set_dtim_interval_ex},
    { "listen",         "listen interval",  wifi_config_set_listen_interval_ex},
    { "reload",         "reload the configuration", wifi_config_set_reload_ex},
    { "txraw",          "send TX RAW packet",   wifi_tx_raw_pkt_ex},
    { "scantabsize",    "scan table size",  wifi_config_set_scan_tab_size_ex},
    { "n9dbg",          "set N9 debug level",   wifi_config_set_n9_dbg_level},
    { "cm4dbg",         "set CM4 debug level",  wifi_config_set_cm4_dbg_level},
    { "airplay",        "enable apple airplay", wifi_config_set_airplay},
    { "appleie",        "update apple IE",      wifi_config_set_apple_ie},
	{ "rssi_thresh",    "rssi threshold",       wifi_config_set_rssi_threshold_ex},
    { "frame_filter",   "frame_filter",         wifi_config_set_frame_filter_ex},
    { NULL }
};

cmd_t   wifi_config_get_cli[] = {
    { "opmode",         "STA/AP/Dual",      wifi_config_get_opmode_ex},
    { "mac",            "MAC address",      wifi_config_get_mac_address_ex},
    { "ssid",           "SSID",             wifi_config_get_ssid_ex   },
    //{ "bssid",            "BSSID",            wifi_config_get_bssid_ex      },
    { "psk",            "WPA/WPA2PSK",      wifi_config_get_psk_ex    },
    { "pmk",            "PMK for WPA/WPA2PSK",  wifi_config_get_pmk_ex    },
    { "wep",            "WEP key",          wifi_config_get_wep_key_ex },
    { "rxfilter",       "RX filter",        wifi_config_get_rx_filter_ex},
    { "mtksmart",       "MTK smart connection", wifi_config_get_mtk_smartconnection_filter_ex},
    { "radio",          "RADIO on/off",     wifi_config_get_radio_on_ex},
    { "ch",             "channel",          wifi_config_get_channel_ex},
    { "bw",             "bandwidth",        wifi_config_get_bandwidth_ex},
    { "bw_extended",    "bandwidth",        wifi_config_get_bandwidth_extended_ex},
    { "wirelessmode",   "wireless mode",    wifi_config_get_wireless_mode_ex},
    { "country",        "country region",   wifi_config_get_country_region_ex},
    { "dtim",           "DTIM interval",    wifi_config_get_dtim_interval_ex},
    { "listen",         "listen interval",  wifi_config_get_listen_interval_ex},
    { "scantabsize",    "scan table size",  wifi_config_get_scan_tab_size_ex},
    { "n9dbg",          "get N9 debug level",   wifi_config_get_n9_dbg_level},
    { "cm4dbg",         "get CM4 debug level",  wifi_config_get_cm4_dbg_level},
    { "rssi_thresh",    "rssi threshold",       wifi_config_get_rssi_threshold_ex},
    { NULL }
};
