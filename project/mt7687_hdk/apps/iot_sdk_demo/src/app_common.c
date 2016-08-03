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
#include "wifi_api.h"
#include "syslog.h"
#include "lwip_network.h"
#include "network_init.h"
#include "FreeRTOS.h"
#include "task.h"

bool g_wifi_init_ready = false;


int32_t user_wifi_init_complete_handler(wifi_event_t event,
                                      uint8_t *payload,
                                      uint32_t length)
{
    LOG_I(common, "WiFi Init Done: port = %d", payload[6]);
    g_wifi_init_ready = true;
    return 1;
}

int32_t user_wifi_init_query_status(void)
{
    while (g_wifi_init_ready == false) {
        vTaskDelay(20);
    }
    return 1;
}

void user_wifi_app_entry(void *args)
{
    /* This is an example to show how to use wifi config APIs to change WiFI settings
     * If user not enable this code, system keep the wifi settings which user set by wifi_init()*/
#if 0
    uint8_t ssid[] = "MTK_AP";
    uint8_t password[] = "12345678";

    /*Wait until wifi initial done*/
    user_wifi_init_query_status();

    /*Change to AP mode*/
    wifi_set_opmode(WIFI_MODE_AP_ONLY);
    wifi_config_set_ssid(WIFI_PORT_AP, ssid, strlen(ssid));
    wifi_config_set_security_mode(WIFI_PORT_AP, WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK, WIFI_ENCRYPT_TYPE_TKIP_AES_MIX);
    wifi_config_set_wpa_psk_key(WIFI_PORT_AP, password, strlen(password));
    wifi_config_set_channel(WIFI_PORT_AP, 6);
    wifi_config_reload_setting();
#endif

    while(1){
    }
}


