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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os.h"
#include "os_util.h"
#include "type_def.h"
#include "wifi_cli.h"
#include "cli.h"
#include "wifi_api.h"
#include "wifi_private_api.h"
#include "wifi_scan.h"
#include "wifi_rx_desc.h"
#include "network_init.h"
#include "get_profile_string.h"
#include "misc.h"

#if defined(MTK_SMTCN_ENABLE)
#include "smt_conn.h"
#endif

cmd_t   wifi_config_set_pub_cli[] = {
    { "opmode",         "STA/AP/Dual",      wifi_config_set_opmode_ex},
    { "ssid",           "SSID",             wifi_config_set_ssid_ex   },
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
    { NULL }
};

cmd_t   wifi_config_get_pub_cli[] = {
    { "opmode",         "STA/AP/Dual",      wifi_config_get_opmode_ex},
    { "mac",            "MAC address",      wifi_config_get_mac_address_ex},
    { "ssid",           "SSID",             wifi_config_get_ssid_ex   },
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
    { NULL }
};

cmd_t   wifi_connect_set_pub_cli[] = {
    { "scan",       "start/stop scan",                      wifi_connect_set_scan_ex}, /*need implement base on WiFi Scan API*/
    { "connection", "connect/disconnect to AP",             wifi_connect_set_connection_ex},
    { "deauth",     "de-authenticate specific STA",         wifi_connect_deauth_station_ex},
    { "eventcb",    "register/un-register event callback",  wifi_connect_set_event_callback_ex},
    { NULL }
};

cmd_t   wifi_connect_get_pub_cli[] = {
    { "stalist",     "get STA list",                        wifi_connect_get_station_list_ex},
    { "linkstatus",  "get link status",                     wifi_connect_get_link_status_ex},
    { "max_num_sta", "get max number of supported STA",     wifi_connect_get_max_station_number_ex},
    { "rssi",        "get rssi",                            wifi_connect_get_rssi_ex},
    { NULL }
};


static cmd_t   wifi_config_pub_cli[] = {
    { "set",   "wifi config set",    NULL,   wifi_config_set_pub_cli },
    { "get",   "wifi config get",    NULL,   wifi_config_get_pub_cli },
    { NULL,    NULL,                 NULL,   NULL                    }
};

static cmd_t   wifi_connect_pub_cli[] = {
    { "set",   "wifi connect set",   NULL,   wifi_connect_set_pub_cli },
    { "get",   "wifi connect get",   NULL,   wifi_connect_get_pub_cli },
    { NULL,    NULL,                 NULL,   NULL                     }
};

cmd_t   wifi_pub_cli[] = {
    { "config",     "wifi config",  NULL,    wifi_config_pub_cli      },
    { "connect",    "wifi connect", NULL,    wifi_connect_pub_cli     },
    { NULL,         NULL,           NULL,    NULL                     }
};


extern wifi_scan_list_item_t *g_scan_list;
extern uint8_t g_scan_list_size;

/**
* @brief
*
* @param
*
* @return  >=0 means success, <0 means fail
*/
int wlan_raw_pkt_rx_filter_sample(uint8_t *payload, unsigned int len)
{
    P_HW_MAC_RX_DESC_T        pRxWI;
    UINT8          ucRxPktType;
    UINT8          ucMacHeaderLength;
    UINT16         u2PayloadLength;
    PUINT8         pucMacHeader;       /* 802.11 header  */
    PUINT8         pucPayload;         /* 802.11 payload */
    UINT16         u2RxStatusOffst;

    pRxWI   = (P_HW_MAC_RX_DESC_T)(payload);
    // =======  Process RX Vector (if any) ========

    // 1. ToDo: Cache RXStatus in SwRfb
    u2RxStatusOffst = sizeof(HW_MAC_RX_DESC_T);
    ucRxPktType = HAL_RX_STATUS_GET_PKT_TYPE(pRxWI);

    if (ucRxPktType != RX_PKT_TYPE_RX_DATA) {
        return 0;    // not handled
    }

    if (ucRxPktType == RX_PKT_TYPE_RX_DATA) {
        UINT16          u2GrpOffst;
        UINT8              ucGroupVLD;

        // RX Status Group
        u2GrpOffst = sizeof(HW_MAC_RX_DESC_T);
        ucGroupVLD = HAL_RX_STATUS_GET_GROUP_VLD(pRxWI);
        if (ucGroupVLD & BIT(RX_GROUP_VLD_4)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_4_T);
        }

        if (ucGroupVLD & BIT(RX_GROUP_VLD_1)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_1_T);
        }

        if (ucGroupVLD & BIT(RX_GROUP_VLD_2)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_2_T);
        }

        if (ucGroupVLD & BIT(RX_GROUP_VLD_3)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_3_T);
        }

        u2RxStatusOffst = u2GrpOffst;
    }


    // Locate the MAC Header based on GroupVLD
    pucMacHeader = (PUINT8)(((UINT32)pRxWI + (HAL_RX_STATUS_GET_HEADER_OFFSET(pRxWI)) + u2RxStatusOffst) & 0xFFFFFFFE);
    ucMacHeaderLength = HAL_RX_STATUS_GET_HEADER_LEN(pRxWI);

    pucPayload = (PUINT8)((UINT32)(pucMacHeader + ucMacHeaderLength) & 0xFFFFFFFC);
    u2PayloadLength = (UINT16)(HAL_RX_STATUS_GET_RX_BYTE_CNT(pRxWI) - ((UINT32)pucPayload - (UINT32)pRxWI));

    printf("payload length=%d\n", u2PayloadLength); // To workaround compiler warning for u2PayloadLength not used
    return 1; /* handled */
}

int32_t port_sanity_check(char *port_str)
{
    int32_t port = (int32_t)atoi(port_str);

    if (port > WIFI_PORT_AP) {
        printf("Invalid port argument: %ld\n", port);
        return 1;
    }
    return port;
}

/**
* @brief Example of Get MAC address for STA/AP wireless port
*
*  wifi config get mac <port> --> get port0 (STA) MAC address
* @parameter
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_mac_address_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    uint8_t addr[WIFI_MAC_ADDRESS_LENGTH] = {0};
    unsigned char addr2[WIFI_MAC_ADDRESS_LENGTH] = {0};
    int32_t port = port_sanity_check(param[0]);

    if (port < 0) {
        return 1;
    }

    status = wifi_config_get_mac_address(port, addr);

    for (i = 0; i < WIFI_MAC_ADDRESS_LENGTH; i++) {
        addr2[i] = addr[i];
    }

    printf("wifi_config_get_mac_address(port%ld): (%02x:%02x:%02x:%02x:%02x:%02x), status:%d\n",
           port,
           addr2[0], addr2[1], addr2[2],
           addr2[3], addr2[4], addr2[5],
           status);

    return status;
}




/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
*
* wifi config set bw <port>  <0:HT20/1:HT40>
* @parameter
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_bandwidth_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    char bw = atoi(param[1]);
    if (port < 0) {
        return 1;
    }

    status = wifi_config_set_bandwidth(port, bw);
    printf("wifi_config_set_bandwidth(port%ld): %d, status:%d\n",
           port,  bw,  status);

    return status;
}

/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
*
* wifi config set bw <port>  <0:HT20/1:HT40> <below_above_ch>
* @parameter
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_bandwidth_extended_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
	int32_t port = port_sanity_check(param[0]);
	char bw = atoi(param[1]);
    char below_above_ch = atoi(param[2]);
    if (port < 0) {
        return 1;
    }

    status = wifi_config_set_bandwidth_extended(port, bw, below_above_ch);
    printf("wifi_config_set_bandwidth_extended(port%ld): %d, %d, status:%d\n",
           port,  bw, below_above_ch,  status);

    return status;
}


/**
* @brief Example of get bandwidth for STA/AP wireless port.
*  wifi config get bw <port>
* @parameter
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_bandwidth_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t bw = 0;
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    status = wifi_config_get_bandwidth(port, &bw);
    printf("wifi_config_get_bandwidth(port%ld): %d, status:%d\n",
           port,  bw,  status);

    return status;
}


/**
* @brief Example of Configure channel for STA/AP wireless port. STA will keep idle and stay in channel specified
* wifi config set ch <port> <ch>
* @parameter
*     [IN] channel  1~14 are supported for 2.4G only product
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_channel_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    char ch = atoi(param[1]);

    if (port < 0) {
        return 1;
    }

    status = wifi_config_set_channel(port, ch);
    printf("wifi_config_set_channel(port%ld): %d, status:%d\n",
           port,  ch,  status);

    return status;
}

/**
* @brief Example of get bandwidth for STA/AP wireless port.
*  wifi config get bw <port>
* @parameter
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_bandwidth_extended_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t bw = 0;
    uint8_t below_above_ch= 0;
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    status = wifi_config_get_bandwidth_extended(port, &bw, &below_above_ch);
    printf("wifi_config_get_bandwidth(port%ld): %d, %d, status:%d\n",
           port,  bw, below_above_ch, status);

    return status;
}


/**
* @brief Example of get the current channel for STA/AP wireless port.
* wifi config get channel <port> <ch>
* @parameter
*     [OUT] channel I1~14 are supported for 2.4G only product
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_channel_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t ch = 0;

    if (port < 0) {
        return 1;
    }

    status = wifi_config_get_channel(port, &ch);
    printf("wifi_config_get_channel(port%ld): %d, status:%d\n",
           port,  ch,  status);

    return status;
}


/**
* @brief Example of configure rx filter for packets wanted to be received
* wifi config set rxfilter <flag>
* @parameter
*    [IN]flag defined in  wifi_rx_filter_t
* @return >=0 means success, <0 means fail
* @note Default value will be WIFI_DEFAULT_IOT_RX_FILTER
*/
uint8_t wifi_config_set_rx_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint32_t flag = strtol(param[0], NULL, 0);
    status = wifi_config_set_rx_filter(flag);
    printf("wifi_config_set_rxfilter: 0x%x, status:%d\n",
           (unsigned int) flag,    status);

    return status;
}


/**
* @brief Example of get rx filter for packets format wanted to be received
* wifi config get rxfilter
* @parameter
*    [OUT]flag defined in  wifi_rx_filter_t
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_rx_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint32_t flag = 0;

    status = wifi_config_get_rx_filter(&flag);
    printf("wifi_config_get_rxfilter: 0x%x, status:%d\n",
           (unsigned int) flag,    status);

    return status;
}

/**
* @brief Example of Set WiFi Raw Packet Receiver
* wifi config set rxraw <enable>
* @param [IN]enable 0: unregister, 1: register
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_rx_raw_pkt_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t enable = atoi(param[0]);

    if (enable == 0) {
        status = wifi_config_unregister_rx_handler();
    } else {
        status = wifi_config_register_rx_handler((wifi_rx_handler_t) wlan_raw_pkt_rx_filter_sample);
    }

    printf("wifi_config_set_rx_raw_pkt_ex, enable = %d, status:%d\n", enable, status);
    return status;
}


uint8_t wifi_tx_raw_pkt_ex(uint8_t len, char *param[])
{
    int8_t status = 0;
    uint8_t *hex_buf = NULL;
    uint32_t len_hex_buf = os_strlen(param[0]) / 2 + os_strlen(param[0]) % 2;
    int i_hex = 0;
    //txd_param_t txd;

    if ((hex_buf = (uint8_t *)os_malloc(len_hex_buf)) == NULL) {
        printf("alloc buffer failed.\n");
        return 1;
    }
    os_memset(hex_buf, 0, len_hex_buf);

    for (int i = 0; i < os_strlen(param[0]); i++) {
        char cc = param[0][i];
        if ((i % 2 == 0) && i) {
            i_hex++;
        }

        if (cc >= '0' && cc <= '9') {
            hex_buf[i_hex] = (hex_buf[i_hex] << 4) + cc - '0';
        } else if (cc >= 'a' && cc <= 'f') {
            hex_buf[i_hex] = (hex_buf[i_hex] << 4) + cc - 'a' + 10;
        } else if (cc >= 'A' && cc <= 'F') {
            hex_buf[i_hex] = (hex_buf[i_hex] << 4) + cc - 'A' + 10;
        } else {
            printf("input invalid\n");
            return 1;
        }
    }

    if ((status = wifi_connection_send_raw_packet(hex_buf, len_hex_buf)) < 0) {
        printf("tx raw packet failed.\n");
    }
    os_free(hex_buf);
    return status;
}





/**
* @brief Example of Set the authentication mode for the specified STA/AP port
* wifi config set psk <port> <auth> <encrypt> <password>
* @param [IN]authmode
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param  [IN] encryption method index:
*              1 AES
*              2 TKIP
*                  3 TKIPAES
* @param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_set_psk_ex(uint8_t len, char *param[])
{
    int8_t status = 0;
    uint8_t length;
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    if(len == 2) {
        length = os_strlen(param[1]);
        status = wifi_config_set_wpa_psk_key(port, (uint8_t *)param[1], length);
        if (status < 0) {
            printf("wifi_config_set_psk_ex: ERROR! set_wpa_psk_key fail! port:%ld, psk(%d, %d, %s), status:%d\n",
                   port, auth, encrypt, param[1], status);
            return 1;
        }
        printf("wifi_config_set_psk_ex: port:%ld, psk(%s), status:%d\n",
               port, param[1], status);
    } else if (len == 4) {
        auth = atoi(param[1]);
        encrypt = atoi(param[2]);
        length = os_strlen(param[3]);
        status = wifi_config_set_security_mode(port, (wifi_auth_mode_t)auth, (wifi_encrypt_type_t)encrypt);
        if (status < 0) {
            printf("wifi_config_set_psk_ex: ERROR! set_security_mode fail! port:%ld, psk(%d, %d, %s), status:%d\n",
                   port, auth, encrypt, param[3], status);
            return 1;
        }
        status = wifi_config_set_wpa_psk_key(port, (uint8_t *)param[3], length);
        if (status < 0) {
            printf("wifi_config_set_psk_ex: ERROR! set_wpa_psk_key fail! port:%ld, psk(%d, %d, %s), status:%d\n",
                   port, auth, encrypt, param[3], status);
            return 1;
        }

        printf("wifi_config_set_psk_ex: port:%ld, psk(%d, %d, %s), status:%d\n",
               port, auth, encrypt, param[3], status);
    } else {
        printf("Invalid command format.\n");
    }

    return 0;

}


/**
* @brief Example of Set PMK for the specified STA/AP port
* wifi config set pmk <port> <PMK>
* @param [IN]port
*       0 STA / AP Client
*       1 AP
* @param  [IN] PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_set_pmk_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t hex[32];

    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    os_memset(hex, 0, sizeof(hex));
    convert_string_to_hex_array(param[1], hex);
    status = wifi_config_set_pmk(port, hex);

    printf("wifi_config_set_pmk_ex: port:%ld, status:%d\n",
           port, status);

    return status;

}

/**
* @brief Example of Set the authentication mode for the specified STA/AP port
* wifi config set psk <port>
* @param [OUT]authmode
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param  [OUT] encryption method index:
*              1 AES
*              2 TKIP
*                  3 TKIPAES
* @param [OUT]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @param [OUT]passphrase_len 8 ~ 64
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_get_psk_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    uint8_t length = 0;
    uint8_t password[64] = {0};
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    status = wifi_config_get_security_mode(port, (wifi_auth_mode_t *)&auth, (wifi_encrypt_type_t *)&encrypt);
    status = wifi_config_get_wpa_psk_key(port, password, &length);

    printf("wifi_config_get_psk_ex: port:%ld, psk(%d, %d, %s), status:%d\n",
           port, auth, encrypt, password, status);

    return status;

}

/**
* @brief Example of Get PMK for the specified STA/AP port
* wifi config set pmk <port>
* @param [IN]port
*       0 STA / AP Client
*       1 AP
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_get_pmk_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    uint8_t hex[32];
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    os_memset(hex, 0, sizeof(hex));
    status = wifi_config_get_pmk(port, hex);
    printf("wifi_config_get_pmk_ex: port:%ld, status:%d, key dump:\n",
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
* @brief Example of get WiFi WEP Keys
* wifi config get wep >port>
* @param [OUT]wifi_wep_key_t
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_wep_key_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    wifi_wep_key_t *keys = NULL;
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    keys = (wifi_wep_key_t *)os_zalloc(sizeof(wifi_wep_key_t));
    if (keys) {
        os_memset(keys, 0, sizeof(wifi_wep_key_t));
    } else {
        printf("%s: keys alloc fail.\n", __FUNCTION__);
        return 1;
    }
    status = wifi_config_get_wep_key(port, keys);

    printf("wifi_config_get_wep_ex: port:%ld, status:%d:", port, status);

    if (keys->wep_tx_key_index < 4) {
        int i;
        printf("keys[%d]=", (int)keys->wep_tx_key_index);
        for (i = 0; i < keys->wep_key_length[keys->wep_tx_key_index]; i++) {
            printf("%02x", keys->wep_key[keys->wep_tx_key_index][i]);
        }
        printf("\n");
    } else {
        printf("Invalid key id:[%d]\n", (int)keys->wep_tx_key_index);
        status = 1;
    }

    os_free(keys);
    return status;
}


/**
* @brief Example of Set WiFi WEP Keys
* wifi config set wep <port> <key_id> <key_string>
* @param [IN]wifi_wep_key_t
*
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_wep_key_ex(uint8_t len, char *param[])
{
    uint8_t index = 0;
    wifi_wep_key_t wep_key;
    char *ptr = NULL;
    int32_t port = port_sanity_check(param[0]);
    char *keys = param[2];
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    wep_key.wep_tx_key_index = atoi(param[1]);

    index = 0;
    for (ptr = (char *)rstrtok((char *)keys, ","); (ptr); ptr = (char *)rstrtok(NULL, ",")) {
        printf("[%d] key=%s\n", index, ptr);

        if (os_strlen(ptr) == 10 || os_strlen(ptr) == 26) {
            wep_key.wep_key_length[index] = os_strlen(ptr) / 2;
            AtoH((char *)ptr, (char *)&wep_key.wep_key[index], (int)wep_key.wep_key_length[index]);
        } else if (os_strlen(ptr) == 1 || os_strlen(ptr) == 5 || os_strlen(ptr) == 13) {
            os_memcpy(wep_key.wep_key[index], ptr, os_strlen(ptr));
            wep_key.wep_key_length[index] = os_strlen(ptr);
        } else {
            printf("invalid length of value.\n");
        }
        index++;
        if (index >= WIFI_NUMBER_WEP_KEYS) {
            break;
        }

    }

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

    return (uint8_t)wifi_config_set_wep_key((uint8_t)port, &wep_key);
}


/**
* @brief Example of Get WiFi Operation Mode.
* wifi config get opmode
* @param [OUT]mode
* @      1 WIFI_MODE_STA_ONLY
* @      2 WIFI_MODE_AP_ONLY
* @      3 WIFI_MODE_REPEATER
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_opmode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t mode = 0;

    status = wifi_config_get_opmode(&mode);
    printf("wifi_config_get_opmode(%d), status:%d\n", (unsigned int)mode, status);
    return status;
}

/**
* @brief Example of Set WiFi Operation Mode.
* wifi config set opmode <mode>
* @param [IN]mode
* @     1 WIFI_MODE_STA_ONLY
* @     2 WIFI_MODE_AP_ONLY
* @     3 WIFI_MODE_REPEATER
*
* @return  >=0 means success, <0 means fail
*
* @note Set WiFi Operation Mode will RESET all the configuration set by previous WIFI-CONFIG APIs
*/
uint8_t wifi_config_set_opmode_ex(uint8_t len, char *param[])
{
    uint8_t target_mode = (uint8_t)atoi(param[0]);
    return wifi_set_opmode(target_mode);
}

/**
* @brief Example of Get WiFi SSID.
* wifi config get ssid <port>
* @param [IN]port
*       0 STA
*       1 AP
* @param [OUT]ssid SSID
* @param [OUT]ssid_len Length of SSID
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_ssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t length = 0;
    uint8_t ssid[32] = {0};
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    status = wifi_config_get_ssid(port, ssid, &length);
    printf("wifi_config_get_ssid(port:%ld), [%s], status:%d\n", port, ssid, status);
    return status;
}


/**
* @brief Example of Set WiFi SSID.
* wifi config set ssid <port> <ssid>
* @param [IN]port
*       0 STA
*       1 AP
* @param [IN]ssid SSID
* @param [IN]ssid_len Length of SSID
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_ssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    status = wifi_config_set_ssid(port, (uint8_t *)param[1], os_strlen(param[1]));
    printf("wifi_config_set_ssid(port:%ld), [%s], status:%d\n", port, param[1], status);
    return status;
}


/**
* @brief Example of Reload configuration
* wifi config set reload
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_reload_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;

    status = wifi_config_reload_setting();
    printf("WiFi reload configuration, status:%d\n", status);
    return status;
}

/**
* @brief Example of Set MTK Smart Connection Filter
* wifi config get mtk_smart
* @param [IN]flag
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_mtk_smartconnection_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t flag = 0;

    status = wifi_config_get_smart_connection_filter(&flag);
    printf("wifi_config_get_mtk_smartconnection_filter(flag=0x%x), status:%d\n", (unsigned int)flag, status);
    return status;
}


/**
* @brief Example of Set MTK Smart Connection Filter
* wifi config set mtk_smart <flag>
* @param [IN]flag
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_mtk_smartconnection_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t flag = atoi(param[0]);

    status = wifi_config_set_smart_connection_filter(flag);
    printf("wifi_config_get_mtk_smartconnection_filter(flag=0x%x), status:%d\n", (unsigned int)flag, status);
    return status;
}


/**
* @brief Example of WiFi Radio ON/OFF
* wifi config set radio <onoff>
* @param [OUT]onoff
*       0 OFF
*       1 ON
* @return  >=0 means success, <0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off at the same time
*/
uint8_t wifi_config_set_radio_on_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t flag = atoi(param[0]);

    status = wifi_config_set_radio(flag);
    printf("wifi_config_set_radio(onoff=%d), status:%d\n", flag, status);
    return status;
}

/**
* @brief Example of get WiFi Radio ON/OFF
* wifi config get radio
* @param [OUT]onoff
*       0 OFF
*       1 ON
* @return  >=0 means success, <0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off at the same time
*/
uint8_t wifi_config_get_radio_on_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t flag = 0;

    status = wifi_config_get_radio(&flag);
    printf("wifi_config_get_radio(onoff=%d), status:%d\n", (int)flag, status);
    return status;
}



/**
* @brief Example of Get WiFi Wireless Mode
* wifi config get wirelessmode
* @return 1 legacy 11B only
* @return 2 legacy 11A only
* @return 3 legacy 11A/B/G mixed
* @return 4 legacy 11G only
* @return 5 11ABGN mixed
* @return 6 11N only in 2.4G
* @return 7 11GN mixed
* @return 8 11AN mixed
* @return 9 11BGN mixed
* @return 10 11AGN mixed
*
*/
uint8_t wifi_config_get_wireless_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t mode = 0;
    int32_t port = port_sanity_check(param[0]);

    if (port < 0) {
        return 1;
    }

    status = wifi_config_get_wireless_mode(port, (wifi_phy_mode_t *)&mode);
    printf("wifi_config_get_wireless_mode(mode=%d), status:%d\n", mode, status);
    return status;
}


/**
* @brief Example of Set WiFi Wireless Mode
* wifi config set wirelessmode <mode>
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
uint8_t wifi_config_set_wireless_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t mode;
    int32_t port = port_sanity_check(param[0]);

    if (port < 0) {
        return 1;
    }
    mode = atoi(param[1]);

    status = wifi_config_set_wireless_mode(port, (wifi_phy_mode_t)mode);
    printf("wifi_config_set_wireless_mode(mode=%d), status:%d\n", mode, status);
    return status;
}


/**
* @brief Example of Get WiFi Country Region
* wifi config get country <band>
* @param [IN]band
* @param 0 2.4G
* @param 1 5G
*
* @return  region: (2.4G)
* @return  0   CH1-11
* @return  1   CH1-13
* @return  2   CH10-11
* @return  3   CH10-13
* @return  4   CH14
* @return  5   CH1-14 all active scan
* @return  6   CH3-9
* @return  7   CH5-13
* @return
* @return  region: (5G)
* @return  0   CH36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165
* @return  1   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140
* @return  2   CH36, 40, 44, 48, 52, 56, 60, 64
* @return  3   CH52, 56, 60, 64, 149, 153, 157, 161
* @return  4   CH149, 153, 157, 161, 165
* @return  5   CH149, 153, 157, 161
* @return  6   CH36, 40, 44, 48
* @return  7   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165
* @return  8   CH52, 56, 60, 64
* @return  9   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165
* @return  10   CH36, 40, 44, 48, 149, 153, 157, 161, 165
* @return  11   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 149, 153, 157, 161
*/
uint8_t wifi_config_get_country_region_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t band = atoi(param[0]);
    uint8_t region = 0;

    status = wifi_config_get_country_region(band, &region);
    printf("wifi_config_get_country_region(band:%d), region:%d, status:%d\n", band, region, status);
    return status;
}


/**
* @brief Example of Set WiFi Country Region
* wifi config set country <band> <region>
* @param [IN]band
* @param 0 2.4G
* @param 1 5G
*
* @param [IN]region
* @param  region: (2.4G)
* @param  0   CH1-11
* @param  1   CH1-13
* @param  2   CH10-11
* @param  3   CH10-13
* @param  4   CH14
* @param  5   CH1-14 all active scan
* @param  6   CH3-9
* @param  7   CH5-13
* @param
* @param  region: (5G)
* @param  0   CH36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165
* @param  1   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140
* @param  2   CH36, 40, 44, 48, 52, 56, 60, 64
* @param  3   CH52, 56, 60, 64, 149, 153, 157, 161
* @param  4   CH149, 153, 157, 161, 165
* @param  5   CH149, 153, 157, 161
* @param  6   CH36, 40, 44, 48
* @param  7   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165
* @param  8   CH52, 56, 60, 64
* @param  9   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165
* @param  10   CH36, 40, 44, 48, 149, 153, 157, 161, 165
* @param  11   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 149, 153, 157, 161
*/
uint8_t wifi_config_set_country_region_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t band = atoi(param[0]);
    uint8_t region = atoi(param[1]);

    status = wifi_config_set_country_region(band, region);
    printf("wifi_config_set_country_region(band:%d, region:%d), status:%d\n", band, region, status);
    return status;
}


/**
* @brief Example of Get WiFi DTIM Interval
* wifi config get dtim
* @return interval: 1~255
*/
uint8_t wifi_config_get_dtim_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dtim = 0;

    status = wifi_config_get_dtim_interval(&dtim);
    printf("wifi_config_get_dtim_interval dtim:%d, status:%d\n", dtim, status);
    return status;
}


/**
* @brief Example of Set WiFi DTIM Interval
* wifi config set dtim <dtim interval>
* @param [IN]interval 1 ~ 255
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_dtim_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dtim = atoi(param[0]);

    status = wifi_config_set_dtim_interval(dtim);
    printf("wifi_config_set_dtim_interval dtim:%d, status:%d\n", dtim, status);
    return status;
}


/**
* @brief Example of Get WiFi Listen Interval
* wifi config get listen
* @return interval: 1~255
*/
uint8_t wifi_config_get_listen_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t listen = 0;

    status = wifi_config_get_listen_interval(&listen);
    printf("wifi_config_get_listen_interval listen:%d, status:%d\n", listen, status);
    return status;
}


/**
* @brief Example of Set WiFi Listen Interval
* wifi config set listen <listen interval>
* @param [IN]interval 1 ~ 255
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_listen_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t listen = atoi(param[0]);

    status = wifi_config_set_listen_interval(listen);
    printf("wifi_config_set_listen_interval listen:%d, status:%d\n", listen, status);
    return status;
}

/**
* @brief Example of Get WiFi Associated Station List
* wifi connect get stalist
* @param [OUT]station_list
*
* @return Number of associated stations will be returned
*/
uint8_t wifi_connect_get_station_list_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    wifi_sta_list_t list[WIFI_MAX_NUMBER_OF_STA];
    uint8_t size = 0;
    status = wifi_connection_get_sta_list(&size, list);

    printf("stalist size=%d\n", size);
    for (i = 0; i < size; i++) {
        printf("%d\n", i);
        printf("    last_tx_rate: MCS=%d, LDPC=%d, MODE=%d\n", (list[i].last_tx_rate.field.mcs), (list[i].last_tx_rate.field.ldpc), (list[i].last_tx_rate.field.mode));
        printf("    last_rx_rate: MCS=%d, LDPC=%d, MODE=%d\n", (list[i].last_rx_rate.field.mcs), (list[i].last_rx_rate.field.ldpc), (list[i].last_rx_rate.field.mode));
        printf("    rssi_sample.LastRssi0)=%d\n", (int)(list[i].rssi_sample.last_rssi));
        printf("    rssi_sample.AvgRssi0X8=%d\n", (int)(list[i].rssi_sample.average_rssi));
        printf("    addr=%02x:%02x:%02x:%02x:%02x:%02x\n", list[i].mac_address[0],
               list[i].mac_address[1],
               list[i].mac_address[2],
               list[i].mac_address[3],
               list[i].mac_address[4],
               list[i].mac_address[5]);
        printf("    power_save_mode=%d\n", (unsigned int)(list[i].power_save_mode));
        printf("    bandwidth=%d\n", (unsigned int)(list[i].bandwidth));
        printf("    keep_alive=%d\n", (unsigned int)(list[i].keep_alive));

    }
    printf("wifi_connection_get_sta_list, status:%d\n", status);
    return status;
}

/**
* @brief Example of Get the max number of the supported stations in ap mode or Repeater Mode.
* wifi connect get max_num_sta
* @param [OUT] number The max number of supported stations will be returned
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_connect_get_max_station_number_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t number = 0;
    status = wifi_connection_get_max_sta_number(&number);
    printf("wifi_connect_get_max_station_number_ex: max sta number=%d, status=%d\n", number, status);
    return status;
}

/**
* @brief Example of Get rssi of the connected AP
* wifi connect get rssi
* @parameter None
* @return >=0 means success, the status will be:
*
*/
uint8_t wifi_connect_get_rssi_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int8_t rssi = 0;

    status = wifi_connection_get_rssi(&rssi); // fixme

    printf("wifi_connect_get_rssi_ex(rssi=%d), status:%d\n", rssi, status);
    return status;
}


/**
* @brief Example of deauth some WiFi connection
* wifi connect deauth <MAC>
* @param [IN]addr STA MAC Address
*
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_connect_deauth_station_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    unsigned char addr[WIFI_MAC_ADDRESS_LENGTH] = {0};

    wifi_conf_get_mac_from_str((char *)addr, param[0]);
    status = wifi_connection_disconnect_sta(addr);

    printf("wifi_connect_deauth_station(%02x:%02x:%02x:%02x:%02x:%02x), status:%d\n",
           addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
           status);
    return status;
}


/**
* @brief Example of  connect / link up to specifed AP
* wifi connect set connection <linkup/down>
* @parameter
*     [IN] link  0: link down, 1: link up
* @return >=0 means success, <0 means fail
* @note Be sure to configure security setting before connect to AP
*/
uint8_t wifi_connect_set_connection_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t link = atoi(param[0]);

    if (link == 0) { // link down
        status = wifi_connection_disconnect_ap();
    } else {
        status = wifi_config_reload_setting();
    }

    printf("wifi_connect_set_connection_ex(link=%d), status:%d\n", link, status);
    return status;
}



/**
* @brief Example of Get the current STA port link up / link down status of the connection
* wifi connect get linkstatus
* @parameter None
* @return >=0 means success, the status will be:
*               WIFI_STATUS_LINK_DISCONNECTED(0) and
*               WIFI_STATUS_LINK_CONNECTED(1),
*               <0 means fail
* @note WIFI_STATUS_LINK_DISCONNECTED indicates STA may in IDLE/ SCAN/ CONNECTING state
*/
uint8_t wifi_connect_get_link_status_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t link = 0;

    status = wifi_connection_get_link_status(&link); // fixme

    printf("wifi_connect_get_link_status(link=%d), status:%d\n", link, status);
    return status;
}



/**
 * wifi_connect_ieee802_11_parse_elems - Parse information elements in Beacon/ProbeResponse frames
 * @pbuf: Pointer to the start of Beacon/ProbeResponse Frame
 * @len: Length of IE buffer in octets
 * @wifi_scan_list_item_t: Data structure for parsed elements
 * @show_errors: Whether to show parsing errors in debug log
 * Returns: Parsing result
 */
int wifi_connect_ieee802_11_parse_elems(unsigned char *pbuf, unsigned int len,
        wifi_scan_list_item_t *ap_data,
        int show_errors)
{
    size_t left = len;
    unsigned char *pos = NULL;
    PFRAME_802_11 pFrame;
    uint8_t *Ptr = NULL, *pVarIE = NULL;
    uint16_t VarIELen = 0, CapabilityInfo;

    if (ap_data == NULL) {
        if (show_errors) {
            printf("wifi_connect_ieee802_11_parse_elems "
                   "ap_data is NULL \n");
        }
        return -1;
    }

    pVarIE = (uint8_t *)os_zalloc(MAX_VIE_LEN);

    pFrame = (PFRAME_802_11) pbuf;

    // get BSSID from header
    COPY_MAC_ADDR(ap_data->bssid, pFrame->Hdr.Addr2);

    Ptr = pFrame->Octet + WIFI_TIMESTAMP_LENGTH + WIFI_BEACON_INTERVAL_LENGTH;
    left -= WIFI_LENGTH_802_11 + WIFI_TIMESTAMP_LENGTH + WIFI_BEACON_INTERVAL_LENGTH;

    // get capability info from payload and advance the pointer
    os_memmove(&CapabilityInfo, Ptr, WIFI_CAPABILITY_INFO_LENGTH);
    Ptr += WIFI_CAPABILITY_INFO_LENGTH;
    left -= WIFI_CAPABILITY_INFO_LENGTH;

    left -= 2; /* Last 2 bytes are RSSI and Channel from N9 */
    pos = Ptr;
    while (left >= 2) {
        unsigned char id, elen;

        id = *pos++;
        elen = *pos++;
        left -= 2;

        if (elen > left) {
            if (show_errors) {
                printf("IEEE 802.11 element "
                       "parse failed (id=%d elen=%d "
                       "left=%lu)\n",
                       id, elen, (unsigned long) left);
            }
            if (pVarIE) {
                os_free(pVarIE);
            }
            return -1;
        }

        switch (id) {
            case IE_SSID:
                os_memset(ap_data->ssid, 0, WIFI_MAX_LENGTH_OF_SSID);
                os_memcpy(ap_data->ssid, pos, elen);
                ap_data->ssid_length = elen;
                break;
            case IE_DS_PARM:
                ap_data->channel = *pos;
                break;
            case IE_VENDOR_SPECIFIC:
                if (os_memcmp(pos, WPS_OUI, 4) == 0) {
                    ap_data->is_wps_supported = 1;
                } else {
                    os_memcpy(pVarIE + VarIELen, pos - 2, elen + 2);
                    VarIELen += (elen + 2);
                }
                break;
            case IE_RSN:
                os_memcpy(pVarIE + VarIELen, pos - 2, elen + 2);
                VarIELen += (elen + 2);
                break;
            default:
                break;
        }

        left -= elen;
        pos += elen;
    }

    if (VarIELen != 0) {
        BssCipherParse(CapabilityInfo, (unsigned char *)&ap_data->auth_mode, (unsigned char *)&ap_data->encrypt_type, VarIELen, pVarIE);
    }

    if (ap_data->channel == 0) {
        ap_data->channel = (uint8_t)(pbuf[len - 1]);
    }
    ap_data->rssi = (int8_t)(pbuf[len - 2]);
    if (pVarIE) {
        os_free(pVarIE);
    }
    return 0;
}

int mtk_event_handler_sample(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    int handled = 0;
    int i = 0;

    switch (event_id) {
        case WIFI_EVENT_IOT_CONNECTED:
            handled = 1;
            if ((len == WIFI_MAC_ADDRESS_LENGTH) && (payload)) {
                printf("[MTK Event Callback Sample]: LinkUp! CONNECTED MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                       payload[0], payload[1], payload[2], payload[3], payload[4], payload[5]);
            } else {
                printf("[MTK Event Callback Sample]: LinkUp!\n");
            }
            break;
        case WIFI_EVENT_IOT_SCAN_COMPLETE:
            handled = 1;
            for (i = 0; i < g_scan_list_size; i++) {
                printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS");
                printf("%-4d", g_scan_list[i].channel);
                printf("%-33s", g_scan_list[i].ssid);
                printf("%02x:%02x:%02x:%02x:%02x:%02x   ",
                       g_scan_list[i].bssid[0],
                       g_scan_list[i].bssid[1],
                       g_scan_list[i].bssid[2],
                       g_scan_list[i].bssid[3],
                       g_scan_list[i].bssid[4],
                       g_scan_list[i].bssid[5]);
                printf("%-8d", g_scan_list[i].auth_mode);
                printf("%-8d", g_scan_list[i].encrypt_type);
                printf("%-8d", g_scan_list[i].rssi);
                printf("%-8d", g_scan_list[i].is_wps_supported);
                printf("\n");
            }
            printf("[MTK Event Callback Sample]: Scan Done!\n");
            break;
        case WIFI_EVENT_IOT_DISCONNECTED:
            handled = 1;
            if ((len == WIFI_MAC_ADDRESS_LENGTH) && (payload)) {
                printf("[MTK Event Callback Sample]: Disconnect! DISCONNECTED MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                       payload[0], payload[1], payload[2], payload[3], payload[4], payload[5]);
            } else {
                printf("[MTK Event Callback Sample]: Disconnect!\n");
            }
            break;
        case WIFI_EVENT_IOT_PORT_SECURE:
            handled = 1;
            if ((len == WIFI_MAC_ADDRESS_LENGTH) && (payload)) {
                printf("[MTK Event Callback Sample]: Port Secure! CONNECTED MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                       payload[0], payload[1], payload[2], payload[3], payload[4], payload[5]);
            } else {
                printf("[MTK Event Callback Sample]: Port Secure!\n");
            }
            break;
        case WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE:
            handled = 1;
            //hex_dump("Beacon/ProbeResponse", payload, len);
            if (len != 0) {
                wifi_scan_list_item_t ap_data;
                os_memset(&ap_data, 0, sizeof(wifi_scan_list_item_t));
                if (wifi_connect_ieee802_11_parse_elems(payload, len, &ap_data, 1) >= 0) {
                    printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS");
                    printf("%-4d", ap_data.channel);
                    printf("%-33s", ap_data.ssid);
                    printf("%02x:%02x:%02x:%02x:%02x:%02x   ",
                           ap_data.bssid[0],
                           ap_data.bssid[1],
                           ap_data.bssid[2],
                           ap_data.bssid[3],
                           ap_data.bssid[4],
                           ap_data.bssid[5]);
                    printf("%-8d", ap_data.auth_mode);
                    printf("%-8d", ap_data.encrypt_type);
                    printf("%-8d", ap_data.rssi);
                    printf("%-8d", ap_data.is_wps_supported);
                    printf("\n");
                }

            }
            break;
        default:
            handled = 0;
            printf("[MTK Event Callback Sample]: Unknown event(%d)\n", event_id);
            break;
    }
    return handled;
}


/**
* @brief Example of Register WiFi Event Notifier
* wifi connect set eventcb <enable> <event ID>
* @param [IN]evt
* @param evt Event ID
*           LinkUP(0)
*           SCAN COMPLETE(1)
*           DISCONNECT(2)
*           PortSecured Event(3)
*           Report Beacon/ProbeResponse(4)
* @param [IN]enable 0: register, 1: unregister
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_connect_set_event_callback_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t enable = atoi(param[0]);
    uint8_t event_id = atoi(param[1]);

    if (enable == 0) {
        status = wifi_connection_unregister_event_handler((wifi_event_t)event_id, (wifi_event_handler_t) mtk_event_handler_sample);
    } else {
        status = wifi_connection_register_event_handler((wifi_event_t)event_id, (wifi_event_handler_t) mtk_event_handler_sample);
    }

    printf("wifi_connect_set_event_callback(), status:%d\n", status);
    return status;
}

wifi_scan_list_item_t g_ap_list[8] = {{0}};
uint8_t wifi_connect_set_scan_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t start = atoi(param[0]);

    if (!start) {
        status = wifi_connection_stop_scan();
    } else {
        uint8_t scan_mode = atoi(param[1]);
        uint8_t scan_option = atoi(param[2]);

        char *ssid;
        uint8_t ssid_len = 0;
        uint8_t bssid_val[WIFI_MAX_NUMBER_OF_STA] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
        uint8_t *bssid = (uint8_t *)bssid_val;

        if (len >= 4) {
            ssid = param[3];
            ssid_len = os_strlen(ssid);
            if (len == 5) {
                wifi_conf_get_mac_from_str((char *)bssid_val, param[4]);

            } else {
                bssid = (uint8_t *)(NULL);
            }
        } else {
            ssid = NULL;
            ssid_len = 0;
            bssid = (uint8_t *)(NULL);
        }
        /******************reset scan list*******************************/
        wifi_connection_scan_init(g_ap_list, 8);
        printf("size = %d", sizeof(wifi_scan_list_item_t));
        status = wifi_connection_start_scan((uint8_t *)ssid, ssid_len, (uint8_t *)bssid, scan_mode, scan_option);
    }
    return status;
}


#if defined(MTK_SMTCN_ENABLE)
uint8_t _smart_config_test(uint8_t len, char *param[])
{
    if (len == 1) {
        if (!os_strcmp(param[0], "connect")) {
            mtk_smart_connect();
        } else if (!os_strcmp(param[0], "stop")) {
            mtk_smart_stop();
        } else {
            printf("Not supported cmd\n");
        }
    }

    return 1;
}
#endif

