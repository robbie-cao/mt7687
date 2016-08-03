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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "os.h"
#include "semphr.h"
#include "wifi_api.h"
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "portmacro.h"

#define USE_DHCP                 1

#define WIFI_SSID ("SQA_TEST_AP")
#define WIFI_PASSWORD ("77777777")

#define STA_IPADDR   ("192.168.1.101")
#define STA_NETMASK  ("255.255.255.0")
#define STA_GATEWAY  ("192.168.1.1")

SemaphoreHandle_t wifi_connected;
#if USE_DHCP
SemaphoreHandle_t ip_ready;
#endif

#if USE_DHCP
static void ip_ready_callback(struct netif *netif);
#endif
static int32_t wifi_station_port_secure_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);
static int32_t wifi_station_disconnected_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);

/**
  * @brief  dhcp got ip will callback this function.
  * @param[in] struct netif *netif: which network interface got ip.
  * @retval None
  */
#if USE_DHCP
static void ip_ready_callback(struct netif *netif)
{
    if (!ip4_addr_isany_val(netif->ip_addr)) {
        char ip_addr[17] = {0};
        if (NULL != inet_ntoa(netif->ip_addr)) {
            strcpy(ip_addr, inet_ntoa(netif->ip_addr));
            LOG_I(common, "************************");
            LOG_I(common, "DHCP got IP:%s", ip_addr);
            LOG_I(common, "************************");
        } else {
            LOG_E(common, "DHCP got Failed");
        }
    }
    xSemaphoreGive(ip_ready);
    LOG_I(common, "ip ready");
}
#endif

/**
  * @brief  wifi connected will call this callback function. set lwip status in this function
  * @param[in] wifi_event_t event: not used.
  * @param[in] uint8_t *payload: not used.
  * @param[in] uint32_t length: not used.
  * @retval None
  */
static int32_t wifi_station_port_secure_event_handler(wifi_event_t event,
        uint8_t *payload,
        uint32_t length)
{
    struct netif *sta_if;

    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    netif_set_link_up(sta_if);
    xSemaphoreGive(wifi_connected);
    LOG_I(common, "wifi connected");
    return 0;
}

/**
  * @brief  wifi disconnected will call this callback function. set lwip status in this function
  * @param[in] wifi_event_t event: not used.
  * @param[in] uint8_t *payload: not used.
  * @param[in] uint32_t length: not used.
  * @retval None
  */
static int32_t wifi_station_disconnected_event_handler(wifi_event_t event,
        uint8_t *payload,
        uint32_t length)
{
    struct netif *sta_if;

    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    netif_set_link_down(sta_if);
    LOG_I(common, "wifi disconnected");
    return 1;
}

/**
  * @brief  network init function. initial wifi and lwip config
  * @param None
  * @retval None
  */
void sta_network_init(void)
{
#if USE_DHCP
    struct netif *sta_if;
#endif
    wifi_config_t wifi_config = {0};
    lwip_tcpip_config_t tcpip_config = {{0}, {0}, {0}, {0}, {0}, {0}};

    wifi_config.opmode = WIFI_MODE_STA_ONLY;
    strcpy((char *)wifi_config.sta_config.ssid, WIFI_SSID);
    wifi_config.sta_config.ssid_length = strlen(WIFI_SSID);
    strcpy((char *)wifi_config.sta_config.password, WIFI_PASSWORD);
    wifi_config.sta_config.password_length = strlen(WIFI_PASSWORD);

#if !USE_DHCP
    ip4addr_aton(STA_IPADDR, &(tcpip_config.sta_addr));
    ip4addr_aton(STA_NETMASK, &tcpip_config.sta_mask);
    ip4addr_aton(STA_GATEWAY, &tcpip_config.sta_gateway);
#endif

    wifi_connected = xSemaphoreCreateBinary();
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_PORT_SECURE, wifi_station_port_secure_event_handler);
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_DISCONNECTED, wifi_station_disconnected_event_handler);

    wifi_init(&wifi_config, NULL);
    lwip_tcpip_init(&tcpip_config, WIFI_MODE_STA_ONLY);

#if USE_DHCP
    ip_ready = xSemaphoreCreateBinary();
    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    netif_set_status_callback(sta_if, ip_ready_callback);
    dhcp_start(sta_if);
#endif
}

/**
  * @brief  when wifi and ip ready will return.
  * @param None
  * @retval None
  */
void sta_network_ready(void)
{
    xSemaphoreTake(wifi_connected, portMAX_DELAY);
#if USE_DHCP
    xSemaphoreTake(ip_ready, portMAX_DELAY);
#endif
}

