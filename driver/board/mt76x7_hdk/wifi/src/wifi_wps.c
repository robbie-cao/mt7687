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
* @file wifi_wps.c
*
* WIFI WPS Feature with Enrollee and Registrar
*
*/

#if defined(MTK_WIFI_WPS_ENABLE)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type_def.h"
#include "wifi_api.h"
#include "FreeRTOS.h"
#include "wifi_wps.h"
#include "nvdm.h"
#include "syslog.h"
#include "os.h"
#include "os_util.h"
#include "driver_inband.h"
#include "wifi_default_config.h"

int32_t wifi_wps_credential_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length)
{
    LOG_I(minisupp, "wifi_wps_credential_event_handler==>\n");
    LOG_I(minisupp, "wps cred length = %d\n", length);
    uint8_t cred_passphrase_len = 0;
    uint8_t passphrase[WIFI_LENGTH_PASSPHRASE] = {0};

    wifi_wps_credential_info_t *customer_cred = (wifi_wps_credential_info_t *)payload;

    if (event == WIFI_EVENT_IOT_WPS_COMPLETE) {
        LOG_I(minisupp, "WPS: ssid %s; ssid_len %d; auth_mode %d; encr_mode %d\n",
              customer_cred->ssid,
              customer_cred->auth_mode,
              customer_cred->encrypt_type);
        LOG_I(minisupp, "WPS: key_length:%d\n", customer_cred->key_length);

        for (uint8_t i = 0; i < customer_cred->key_length; i++) {
            LOG_I(minisupp, "WPS: key[%d] %02x\n", i, customer_cred->key[i]);
        }

        switch (customer_cred->encrypt_type) {
            case WIFI_ENCRYPT_TYPE_WEP_ENABLED:
                LOG_I(minisupp, "WPS: key_index %d\n", customer_cred->key_index);
                if (customer_cred->key_length) {
                    wifi_wep_key_t *wep_keys = NULL;
                    wep_keys = (wifi_wep_key_t *)os_malloc(sizeof(wifi_wep_key_t));
                    if (wep_keys) {
                        os_memset(wep_keys, 0, sizeof(wifi_wep_key_t));
                        wep_keys->wep_tx_key_index = customer_cred->key_index;
                        os_memcpy(wep_keys->wep_key[customer_cred->key_index],
                                  customer_cred->key,
                                  customer_cred->key_length);
                        wep_keys->wep_key_length[customer_cred->key_index] = customer_cred->key_length;
                    }
                }
                break;
            case WIFI_ENCRYPT_TYPE_TKIP_ENABLED:
            case WIFI_ENCRYPT_TYPE_AES_ENABLED:
                os_memcpy(passphrase, customer_cred->key, sizeof(customer_cred->key));
                cred_passphrase_len = customer_cred->key_length;
                LOG_I(minisupp, "WPS: passphrase %s\n", customer_cred->key);
                break;
            default :
                LOG_I(minisupp, "WPS encrypt type %d\n", customer_cred->encrypt_type);
                break;
        }
    }

    LOG_I(minisupp, "wifi_config_set_ssid==>\n");
    wifi_config_set_ssid(WIFI_PORT_STA, customer_cred->ssid, customer_cred->ssid_len);

    LOG_I(minisupp, "wifi_config_set_security_mode==>\n");
    wifi_config_set_security_mode(WIFI_PORT_STA, customer_cred->auth_mode, customer_cred->encrypt_type);

    LOG_I(minisupp, "wifi_config_set_wpa_psk_key==>\n");
    if (customer_cred->encrypt_type == WIFI_ENCRYPT_TYPE_TKIP_ENABLED || WIFI_ENCRYPT_TYPE_AES_ENABLED) {
        LOG_I(minisupp, "wifi_config_set_wpa_psk_key \n");
        wifi_config_set_wpa_psk_key(WIFI_PORT_STA, customer_cred->key, cred_passphrase_len);
    }

    wifi_wps_unregister_credential_handler();

    LOG_I(minisupp, "wifi_config_reload_setting==>\n");
    wifi_config_reload_setting();

    return 0;
}
int32_t wifi_wps_register_credential_handler(void)
{
    LOG_I(minisupp, "wifi_wps_register_credential_handler==>\n");
    return wifi_connection_register_event_handler(WIFI_EVENT_IOT_WPS_COMPLETE,
            (wifi_event_handler) wifi_wps_credential_event_handler);
}

int32_t wifi_wps_unregister_credential_handler(void)
{
    LOG_I(minisupp, "wifi_wps_unregister_credential_handler==>\n");
    return wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_WPS_COMPLETE,
            (wifi_event_handler) wifi_wps_credential_event_handler);
}

int32_t wifi_wps_config_set_auto_connection(bool auto_connection)
{
    uint8_t opmode = 0;
    LOG_I(minisupp, "wifi_wps_config_set_wps_auto_connection==>\n");

    if (wifi_config_get_opmode(&opmode) < 0) {
        return -1;
    }
    if(WIFI_MODE_REPEATER == opmode) {
        LOG_E(wifi, "Don't support WPS in Repeater mode.");
        return -1;
    }

    return mtk_supplicant_config_set_wps_auto_connection(auto_connection);
}

int32_t wifi_wps_config_get_auto_connection(bool *auto_connection)
{
    uint8_t opmode = 0;
    LOG_I(minisupp, "wifi_wps_config_get_wps_auto_connection==>\n");

    if (wifi_config_get_opmode(&opmode) < 0) {
        return -1;
    }
    if(WIFI_MODE_REPEATER == opmode) {
        LOG_E(wifi, "Don't support WPS in Repeater mode.");
        return -1;
    }
    mtk_supplicant_config_get_wps_auto_connection(auto_connection);
    return 0;
}

int32_t wifi_wps_connection_by_pbc(uint8_t port, uint8_t *bssid)
{
    uint8_t opmode = 0;

    if (wifi_config_get_opmode(&opmode) < 0) {
        return -1;
    }
    if(WIFI_MODE_REPEATER == opmode) {
        LOG_E(wifi, "Don't support WPS in Repeater mode.");
        return -1;
    }

    return mtk_supplicant_connection_wps_pbc(port, bssid);
}


int32_t wifi_wps_connection_by_pin(uint8_t port, uint8_t *bssid, uint8_t *pin_code)
{
    uint8_t opmode = 0;

    if (wifi_config_get_opmode(&opmode) < 0) {
        return -1;
    }
    if(WIFI_MODE_REPEATER == opmode) {
        LOG_E(wifi, "Don't support WPS in Repeater mode.");
        return -1;
    }

    return mtk_supplicant_connection_wps_pin(port, bssid, pin_code);
}


int32_t wifi_wps_config_get_pin_code(uint8_t port, uint8_t *pin_code)
{
    int32_t ret;
    int32_t buflen = 9;
    uint8_t opmode = 0;

    if (wifi_config_get_opmode(&opmode) < 0) {
        return -1;
    }
    if(WIFI_MODE_REPEATER == opmode) {
        LOG_E(wifi, "Don't support WPS in Repeater mode.");
        return -1;
    }
    ret = mtk_supplicatn_wps_get_pin(port, pin_code);
    if (ret >= 0) {
        if ((NULL == pin_code) || strlen((const char *)pin_code) >= buflen) {
            LOG_E(wifi, "AP PIN NULL or Lenth ERROR");
            return -1;
        }
        return ret;
    }
    return ret;
}

int32_t wifi_wps_get_device_info(wifi_wps_device_info_t *device_info)
{

#ifdef MTK_WIFI_PROFILE_ENABLE
    uint32_t buff_size = 0;
#endif

    if (NULL == device_info) {
        return -1;
    }

    os_memset(device_info, 0, sizeof(wifi_wps_device_info_t));

#ifdef MTK_WIFI_PROFILE_ENABLE

    /* Device Name */
    buff_size = WIFI_WPS_MAX_LENGTH_DEVICE_NAME;
    if (NVDM_STATUS_OK != nvdm_read_data_item("common",
            "DeviceName",
            (uint8_t *)(device_info->device_name),
            &buff_size)) {
        goto wps_default_setting;
    }

    /* Manufacturer */
    buff_size = WIFI_WPS_MAX_LENGTH_MANUFACTURER;
    if (NVDM_STATUS_OK != nvdm_read_data_item("common",
            "Manufacturer",
            (uint8_t *)(device_info->manufacturer),
            &buff_size)) {
        goto wps_default_setting;
    }

    /* Model Name */
    buff_size = WIFI_WPS_MAX_LENGTH_MODEL_NAME;
    if (NVDM_STATUS_OK != nvdm_read_data_item("common",
            "ModelName",
            (uint8_t *)(device_info->model_name),
            &buff_size)) {
        goto wps_default_setting;
    }

    /* Model Number */
    buff_size = WIFI_WPS_MAX_LENGTH_MODEL_NUMBER;
    if (NVDM_STATUS_OK != nvdm_read_data_item("common",
            "ModelNumber",
            (uint8_t *)(device_info->model_number),
            &buff_size)) {
        goto wps_default_setting;
    }

    /* Serial Number */
    buff_size = WIFI_WPS_MAX_LENGTH_SERIAL_NUMBER;
    if (NVDM_STATUS_OK != nvdm_read_data_item("common",
            "SerialNumber",
            (uint8_t *)(device_info->serial_number),
            &buff_size)) {
        goto wps_default_setting;
    }

wps_default_setting:

#endif

	os_memcpy(device_info->device_name,   WIFI_DEFAULT_DEVICE_NAME,   sizeof(WIFI_DEFAULT_DEVICE_NAME));
	os_memcpy(device_info->manufacturer,  WIFI_DEFAULT_MANUFACTURER, sizeof(WIFI_DEFAULT_MANUFACTURER));
	os_memcpy(device_info->model_name,    WIFI_DEFAULT_MODEL_NAME, sizeof(WIFI_DEFAULT_MODEL_NAME));
	os_memcpy(device_info->model_number,  WIFI_DEFAULT_MODEL_NUMBER, sizeof(WIFI_DEFAULT_MODEL_NUMBER));
	os_memcpy(device_info->serial_number, WIFI_DEFAULT_SERIAL_NUMBER, sizeof(WIFI_DEFAULT_SERIAL_NUMBER));

    return 0;
}
int32_t wifi_wps_config_set_device_info(wifi_wps_device_info_t *device_info)
{
    uint8_t opmode = 0;
    LOG_I(minisupp, "wifi_wps_config_set_device_info==>\n");

    if (wifi_config_get_opmode(&opmode) < 0) {
        return -1;
    }
    if(WIFI_MODE_REPEATER == opmode) {
        LOG_E(wifi, "Don't support WPS in Repeater mode.");
        return -1;
    }

    return mtk_supplicant_config_set_wps_device_info(device_info);
}

int32_t wifi_wps_config_get_device_info(wifi_wps_device_info_t *device_info)
{
    uint8_t opmode = 0;
    LOG_I(minisupp, "wifi_wps_config_get_device_info==>\n");

    if (wifi_config_get_opmode(&opmode) < 0) {
        return -1;
    }
    if(WIFI_MODE_REPEATER == opmode) {
        LOG_E(wifi, "Don't support WPS in Repeater mode.");
        return -1;
    }

    return mtk_supplicant_config_get_wps_device_info(device_info);
}
#endif /* MTK_WIFI_WPS_ENABLE */

