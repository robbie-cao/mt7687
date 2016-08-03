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
 #include "type_def.h"
 #include "wifi_api.h"
 #include "smt_api.h"
 #include "wifi_scan.h"
 #include "string.h"
 
void smtcn_evt_handler(wifi_smart_connection_event_t event, void *data)
{
    uint8_t passwd[WIFI_LENGTH_PASSPHRASE + 1] = {0}; 
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID + 1] = {0};
    uint8_t ssid_len = 0;
    uint8_t passwd_len = 0;
    
    switch (event)
    {
        case WIFI_SMART_CONNECTION_EVENT_CHANNEL_LOCKED:
        case WIFI_SMART_CONNECTION_EVENT_TIMEOUT:
            break;
        case WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED:
            wifi_smart_connection_get_result(ssid, &ssid_len, passwd, &passwd_len, NULL, NULL);

            wifi_config_set_ssid(WIFI_PORT_STA, ssid, ssid_len);
            if (passwd_len != 0) {
                wifi_config_set_wpa_psk_key(WIFI_PORT_STA, passwd, passwd_len);
                if(passwd_len == 10 || passwd_len == 26 || passwd_len == 5 || passwd_len == 13)
                {
                    wifi_wep_key_t wep_key;
                    if (passwd_len == 10 || passwd_len == 26) {
                        wep_key.wep_key_length[0] = passwd_len / 2;
                        AtoH((char *)passwd, (char *)&wep_key.wep_key[0], (int)wep_key.wep_key_length[0]);
                    } else if (passwd_len == 5 || passwd_len == 13) {
                        wep_key.wep_key_length[0] = passwd_len;
                        memcpy(wep_key.wep_key[0], passwd, passwd_len);
                    }

                    wep_key.wep_tx_key_index = 0;
                    wifi_config_set_wep_key(WIFI_PORT_STA, &wep_key);
                }
                
            }
            wifi_config_reload_setting();

            wifi_smart_connection_deinit();
            break;
    }
}

int32_t mtk_smart_connect(void)
{
    if(wifi_smart_connection_init(NULL, 0, smtcn_evt_handler) < 0){
        return -1;
    }

    wifi_smart_connection_start(0);
    return 0;
}

int32_t mtk_smart_stop(void)
{
    wifi_smart_connection_stop();
    return 0;
}
