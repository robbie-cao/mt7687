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
#include <string.h>
#include "FreeRTOS.h"
#include "nvdm.h"
#include "syslog.h"
#include "hal_efuse.h"
#ifdef MTK_HOMEKIT_ENABLE
#include "hal_sys.h"

#define MAX_KEY_PAIRS   16
#define CFG_KEY_HAP_CTRL_LTPK             "HAP_CTRL_LTPK"
#define NVDM_GROUP_HOMEKIT                "homekit"
#endif

typedef struct {
    char *item_name;
    nvdm_data_item_type_t data_type;
    char *item_default_value;
    uint32_t item_size;
} group_data_item_t;

/* common config */
static const group_data_item_t g_common_data_item_array[] = {
    {
        "OpMode",
        NVDM_DATA_ITEM_TYPE_STRING,
#ifdef MTK_HOMEKIT_ENABLE
        "2",
        sizeof("2")
#else
        "1",
        sizeof("1")
#endif
    },
    {
        "CountryRegion",
        NVDM_DATA_ITEM_TYPE_STRING,
        "5",
        sizeof("5")
    },
    {
        "CountryCode",
        NVDM_DATA_ITEM_TYPE_STRING,
        "TW",
        sizeof("TW")
    },
    {
        "CountryRegionABand",
        NVDM_DATA_ITEM_TYPE_STRING,
        "3",
        sizeof("3")
    },
    {
        "IpAddr",
        NVDM_DATA_ITEM_TYPE_STRING,
        "192.168.1.1",
        sizeof("192.168.1.1")
    },
    {
        "IpNetmask",
        NVDM_DATA_ITEM_TYPE_STRING,
        "255.255.255.0",
        sizeof("255.255.255.0")
    },
    {
        "IpGateway",
        NVDM_DATA_ITEM_TYPE_STRING,
        "192.168.1.254",
        sizeof("192.168.1.254")
    },
    {
        "RadioOff",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "DbgLevel",
        NVDM_DATA_ITEM_TYPE_STRING,
        "3",
        sizeof("3")
    },
    {
        "RTSThreshold",
        NVDM_DATA_ITEM_TYPE_STRING,
        "2347",
        sizeof("2347")
    },
    {
        "FragThreshold",
        NVDM_DATA_ITEM_TYPE_STRING,
        "2346",
        sizeof("2346")
    },
    {
        "BGChannelTable",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1,14,0|",
        sizeof("1,14,0|")
    },
    {
        "AChannelTable",
        NVDM_DATA_ITEM_TYPE_STRING,
        "52,3,1|149,4,0|",
        sizeof("52,3,1|149,4,0|")
    },
    {
        "syslog_filters",
        NVDM_DATA_ITEM_TYPE_STRING,
        "",
        sizeof("")
    },
#if defined(MTK_MINISUPP_ENABLE)
    {
        "Manufacturer",
        NVDM_DATA_ITEM_TYPE_STRING,
        "MTK",
        sizeof("MTK")
    },
    {
        "ModelName",
        NVDM_DATA_ITEM_TYPE_STRING,
        "MTK Wireless Device",
        sizeof("MTK Wireless Device")
    },
    {
        "ModelNumber",
        NVDM_DATA_ITEM_TYPE_STRING,
        "MT7687",
        sizeof("MT7687")
    },
    {
        "SerialNumber",
        NVDM_DATA_ITEM_TYPE_STRING,
        "12345678",
        sizeof("12345678")
    },
    {
        "DeviceName",
        NVDM_DATA_ITEM_TYPE_STRING,
        "MTK IoT",
        sizeof("MTK IoT")
    },
#endif
    {
        "ConfigFree_Ready",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "ConfigFree_Enable",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "StaFastLink",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
};

/* STA config */
static const group_data_item_t g_sta_data_item_array[] = {
    {
        "LocalAdminMAC",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "MacAddr",
        NVDM_DATA_ITEM_TYPE_STRING,
        "00:0c:43:76:87:22",
        sizeof("00:0c:43:76:87:22")
    },
    {
        "Ssid",
        NVDM_DATA_ITEM_TYPE_STRING,
        "MTK_SOFT_AP",
        sizeof("MTK_SOFT_AP")
    },
    {
        "SsidLen",
        NVDM_DATA_ITEM_TYPE_STRING,
        "11",
        sizeof("11")
    },
    {
        "BssType",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "Channel",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "BW",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "WirelessMode",
        NVDM_DATA_ITEM_TYPE_STRING,
        "9",
        sizeof("9")
    },
    {
        "BADecline",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "AutoBA",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HT_MCS",
        NVDM_DATA_ITEM_TYPE_STRING,
        "33",
        sizeof("33")
    },
    {
        "HT_BAWinSize",
        NVDM_DATA_ITEM_TYPE_STRING,
        "64",
        sizeof("64")
    },
    {
        "HT_GI",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HT_PROTECT",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HT_EXTCHA",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "WmmCapable",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "ListenInterval",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "AuthMode",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "EncrypType",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "WpaPsk",
        NVDM_DATA_ITEM_TYPE_STRING,
        "12345678",
        sizeof("12345678")
    },
    {
        "WpaPskLen",
        NVDM_DATA_ITEM_TYPE_STRING,
        "8",
        sizeof("8")
    },
    {
        "Password",
        NVDM_DATA_ITEM_TYPE_STRING,
        "12345678",
        sizeof("12345678")
    },
    {
        "PMK",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "PMK_INFO",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "PairCipher",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "GroupCipher",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "DefaultKeyId",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "SharedKey",
        NVDM_DATA_ITEM_TYPE_STRING,
        "aaaaaaaaaaaaa,bbbbbbbbbbbbb,ccccccccccccc,ddddddddddddd",
        sizeof("aaaaaaaaaaaaa,bbbbbbbbbbbbb,ccccccccccccc,ddddddddddddd")
    },
    {
        "SharedKeyLen",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0,0,0,0",
        sizeof("0,0,0,0")
    },
    {
        "PSMode",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "KeepAlivePeriod",
        NVDM_DATA_ITEM_TYPE_STRING,
        "10",
        sizeof("10")
    },
    {
        "IpMode",
        NVDM_DATA_ITEM_TYPE_STRING,
        "dhcp",
        sizeof("dhcp")
    }
};

/* AP config */
static const group_data_item_t g_ap_data_item_array[] = {
    {
        "LocalAdminMAC",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "MacAddr",
        NVDM_DATA_ITEM_TYPE_STRING,
        "00:0c:43:76:62:12",
        sizeof("00:0c:43:76:62:12")
    },
    {
        "Ssid",
        NVDM_DATA_ITEM_TYPE_STRING,
        "MTK_SOFT_AP",
        sizeof("MTK_SOFT_AP")
    },
    {
        "SsidLen",
        NVDM_DATA_ITEM_TYPE_STRING,
        "11",
        sizeof("11")
    },
    {
        "Channel",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "BW",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "WirelessMode",
        NVDM_DATA_ITEM_TYPE_STRING,
        "9",
        sizeof("9")
    },
    {
        "AutoBA",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HT_MCS",
        NVDM_DATA_ITEM_TYPE_STRING,
        "33",
        sizeof("33")
    },
    {
        "HT_BAWinSize",
        NVDM_DATA_ITEM_TYPE_STRING,
        "64",
        sizeof("64")
    },
    {
        "HT_GI",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HT_PROTECT",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HT_EXTCHA",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "WmmCapable",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "DtimPeriod",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "AuthMode",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "EncrypType",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "WpaPsk",
        NVDM_DATA_ITEM_TYPE_STRING,
        "12345678",
        sizeof("12345678")
    },
    {
        "WpaPskLen",
        NVDM_DATA_ITEM_TYPE_STRING,
        "8",
        sizeof("8")
    },
    {
        "Password",
        NVDM_DATA_ITEM_TYPE_STRING,
        "12345678",
        sizeof("12345678")
    },
    {
        "PMK",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "PairCipher",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "GroupCipher",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "DefaultKeyId",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "SharedKey",
        NVDM_DATA_ITEM_TYPE_STRING,
        "11111,22222,33333,44444",
        sizeof("11111,22222,33333,44444")
    },
    {
        "SharedKeyLen",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0,0,0,0",
        sizeof("0,0,0,0")
    },
    {
        "HideSSID",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "RekeyInterval",
        NVDM_DATA_ITEM_TYPE_STRING,
        "3600",
        sizeof("3600")
    },
    {
        "BcnDisEn",
        NVDM_DATA_ITEM_TYPE_STRING,
#ifdef MTK_HOMEKIT_ENABLE
        "1",
        sizeof("1")
#else
        "0",
        sizeof("0")
#endif
    }
};

#ifdef MTK_HOMEKIT_ENABLE
/* homekit config */
static const group_data_item_t g_homekit_data_item_array[] = {
    {
        "WACDONE",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "WAC_ACS_NAME",
        NVDM_DATA_ITEM_TYPE_STRING,
        "New Wi-Fi Device",
        sizeof("New Wi-Fi Device")
    },
    {
        "HOMEKIT_AUTO_START",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HOMEKIT_DELAY",
        NVDM_DATA_ITEM_TYPE_STRING,
        "3",
        sizeof("3")
    },
    {
        "HAP_MODEL_NAME",
        NVDM_DATA_ITEM_TYPE_STRING,
        "MT7687E2",
        sizeof("MT7687E2")
    },
    {
        "HAP_ACS_NAME",
        NVDM_DATA_ITEM_TYPE_STRING,
        "MTK_Aces",
        sizeof("MTK_Aces")
    },
    {
        "HAP_SERIAL_NUM",
        NVDM_DATA_ITEM_TYPE_STRING,
        "123456789",
        sizeof("123456789")
    },
    {
        "HAP_SETUP_CODE",
        NVDM_DATA_ITEM_TYPE_STRING,
        "482-11-763",
        sizeof("482-11-763")
    },
    {
        "HAP_CONFIG_NUM",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HAP_MFR",
        NVDM_DATA_ITEM_TYPE_STRING,
        "MTK",
        sizeof("MTK")
    },
    {
        "HAP_ACS_LTPK",
        NVDM_DATA_ITEM_TYPE_STRING,
        "",
        sizeof("")
    },
    {
        "HAP_ACS_LTSK",
        NVDM_DATA_ITEM_TYPE_STRING,
        "",
        sizeof("")
    },
    {
        "HAP_CTRL_LTPK",
        NVDM_DATA_ITEM_TYPE_STRING,
        "",
        sizeof("")
    },
    {
        "HAP_MFI_ENABLE",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HAP_PAIR_SETUP_DISABLE",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "HAP_BIND_INF",
        NVDM_DATA_ITEM_TYPE_STRING,
        "apcli0",
        sizeof("apcli0")
    },
    {
        "HAP_DEBUG_LEVEL",
        NVDM_DATA_ITEM_TYPE_STRING,
        "4",
        sizeof("4")
    },
    {
        "HAP_ACS_CONF",
        NVDM_DATA_ITEM_TYPE_STRING,
        "3E,4A",
        sizeof("3E,4A")
    },
    {
        "HAP_ACS_CI",
        NVDM_DATA_ITEM_TYPE_STRING,
        "9",
        sizeof("9")
    },
    {
        "HAP_APP_THERMO_HC_CURRENT",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HAP_APP_THERMO_HC_TARGET",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HAP_APP_THERMO_TP_CURRENT",
        NVDM_DATA_ITEM_TYPE_STRING,
        "50",
        sizeof("50")
    },
    {
        "HAP_APP_THERMO_TP_TARGET",
        NVDM_DATA_ITEM_TYPE_STRING,
        "25",
        sizeof("25")
    },
    {
        "HAP_APP_THERMO_TP_UNITS",
        NVDM_DATA_ITEM_TYPE_STRING,
        "1",
        sizeof("1")
    },
    {
        "HAP_APP_THERMO_NAME",
        NVDM_DATA_ITEM_TYPE_STRING,
        "Thermostat_Service",
        sizeof("Thermostat_Service")
    },
    {
        "HAP_APP_THERMO_RH_CURRENT",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "HAP_APP_THERMO_RH_TARGET",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    },
    {
        "HAP_APP_THERMO_TC_THRESHOLD",
        NVDM_DATA_ITEM_TYPE_STRING,
        "10",
        sizeof("10")
    },
    {
        "HAP_APP_THERMO_TH_THRESHOLD",
        NVDM_DATA_ITEM_TYPE_STRING,
        "0",
        sizeof("0")
    }
};
#endif

void user_data_item_check_default_value(void);

#ifdef MTK_LOAD_MAC_ADDR_FROM_EFUSE
int32_t nvdm_get_mac_addr_from_efuse(const char *group_name, char *mac_addr)
{
    uint8_t buf[16] = {0};//efuse is 16 byte aligned
    uint16_t mac_offset = 0x00;//mac addr offset in efuse
    if (HAL_EFUSE_OK != hal_efuse_read(mac_offset, buf, sizeof(buf))) {
        LOG_W(common, "efuse read mac addr fail, default mac will be applied");
        return -1;
    }
    if (0 == strcmp("STA", group_name)) {
        /* original efuse MAC address for STA */
        sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);
    } else {
        /* original efuse MAC address with byte[5]+1 for AP */
        sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", buf[4], buf[5], buf[6], buf[7], buf[8], buf[9] + 1);
    }
    LOG_I(common, "efuse mac addr: %s", mac_addr);
    return 0;
}
#endif

static void check_default_value(const char *group_name, const group_data_item_t *group_data_array, uint32_t count)
{
    uint8_t buffer[1024] = {0};

    for (uint32_t index = 0; index < count; index++) {
        uint32_t buffer_size = 1024;
        nvdm_status_t status = nvdm_read_data_item(group_name,
                               group_data_array[index].item_name,
                               buffer,
                               &buffer_size);
        if (NVDM_STATUS_OK != status) {
#ifdef MTK_LOAD_MAC_ADDR_FROM_EFUSE
            char mac_addr[18] = {0};
            if ((0 == strcmp("MacAddr", group_data_array[index].item_name)
                    && (0 == nvdm_get_mac_addr_from_efuse(group_name, mac_addr)))) {
                status = nvdm_write_data_item(group_name,
                                              group_data_array[index].item_name,
                                              group_data_array[index].data_type,
                                              (uint8_t *)mac_addr,
                                              strlen(mac_addr));
                if (status != NVDM_STATUS_OK) {
                    LOG_I(common, "nvdm_write_data_item error");
                }
            } else
#endif
            {
                status = nvdm_write_data_item(group_name,
                                              group_data_array[index].item_name,
                                              group_data_array[index].data_type,
                                              (uint8_t *)group_data_array[index].item_default_value,
                                              group_data_array[index].item_size);
                if (NVDM_STATUS_OK != status) {
                    LOG_I(common, "write to [%s]%s error", group_name, group_data_array[index].item_name);
                }
            }
        }
    }
}

static void reset_to_default(const char *group_name, const group_data_item_t *group_data_array, uint32_t count)
{
    for (uint32_t index = 0; index < count; index++) {
        nvdm_status_t status;
#ifdef MTK_LOAD_MAC_ADDR_FROM_EFUSE
        char mac_addr[18] = {0};
        if ((0 == strcmp("MacAddr", group_data_array[index].item_name)
                && (0 == nvdm_get_mac_addr_from_efuse(group_name, mac_addr)))) {
            status = nvdm_write_data_item(group_name,
                                          group_data_array[index].item_name,
                                          NVDM_DATA_ITEM_TYPE_STRING,
                                          (uint8_t *)mac_addr,
                                          strlen(mac_addr));
            if (status != NVDM_STATUS_OK) {
                LOG_I(common, "nvdm_write_data_item error");
            }
        } else
#endif
        {
            status = nvdm_write_data_item(group_name,
                                          group_data_array[index].item_name,
                                          group_data_array[index].data_type,
                                          (uint8_t *)group_data_array[index].item_default_value,
                                          group_data_array[index].item_size);
            if (NVDM_STATUS_OK != status) {
                LOG_I(common, "write to [%s]%s error", group_name, group_data_array[index].item_name);
            }
        }
    }
}

static void show_group_value(const char *group_name, const group_data_item_t *group_data_array, uint32_t count)
{
    uint8_t buffer[1024] = {0};
    for (uint32_t index = 0; index < count; index++) {
        uint32_t buffer_size = 1024;
        nvdm_status_t status = nvdm_read_data_item(group_name,
                               group_data_array[index].item_name,
                               buffer,
                               &buffer_size);
        if (NVDM_STATUS_OK == status) {
            printf("[%s]%s: %s\r\n", group_name, group_data_array[index].item_name, (char *)buffer);
        } else {
            printf("read from [%s]%s error.\r\n", group_name, group_data_array[index].item_name);
        }
    }
}

/* user defined callback functions for each group */
static void common_check_default_value(void)
{
    check_default_value("common",
                        g_common_data_item_array,
                        sizeof(g_common_data_item_array) / sizeof(g_common_data_item_array[0]));
}

static void common_reset_to_default(void)
{
    reset_to_default("common",
                     g_common_data_item_array,
                     sizeof(g_common_data_item_array) / sizeof(g_common_data_item_array[0]));
}

static void common_show_value(void)
{
    show_group_value("common",
                     g_common_data_item_array,
                     sizeof(g_common_data_item_array) / sizeof(g_common_data_item_array[0]));
}

static void sta_check_default_value(void)
{
    check_default_value("STA",
                        g_sta_data_item_array,
                        sizeof(g_sta_data_item_array) / sizeof(g_sta_data_item_array[0]));
}

static void sta_reset_to_default(void)
{
    reset_to_default("STA",
                     g_sta_data_item_array,
                     sizeof(g_sta_data_item_array) / sizeof(g_sta_data_item_array[0]));
}

static void sta_show_value(void)
{
    show_group_value("STA",
                     g_sta_data_item_array,
                     sizeof(g_sta_data_item_array) / sizeof(g_sta_data_item_array[0]));
}

static void ap_check_default_value(void)
{
    check_default_value("AP",
                        g_ap_data_item_array,
                        sizeof(g_ap_data_item_array) / sizeof(g_ap_data_item_array[0]));
}

static void ap_reset_to_default(void)
{
    reset_to_default("AP",
                     g_ap_data_item_array,
                     sizeof(g_ap_data_item_array) / sizeof(g_ap_data_item_array[0]));
}

static void ap_show_value(void)
{
    show_group_value("AP",
                     g_ap_data_item_array,
                     sizeof(g_ap_data_item_array) / sizeof(g_ap_data_item_array[0]));
}

#ifdef MTK_HOMEKIT_ENABLE
static void homekit_check_default_value(void)
{
    check_default_value("homekit",
                        (group_data_item_t *)g_homekit_data_item_array,
                        sizeof(g_homekit_data_item_array) / sizeof(g_homekit_data_item_array[0]));
    LOG_I(common, "homekit_check start %d", sizeof(g_homekit_data_item_array) / sizeof(group_data_item_t));

    uint8_t *buffer = NULL;
    buffer = (uint8_t *)pvPortMalloc(1024 * sizeof(uint8_t));
    if (buffer == NULL) {
        LOG_E(common, "homekit_check_value, failed to malloc");
        return;
    }
    memset(buffer, 0, 1024 * sizeof(uint8_t));

    for (uint32_t idx = 0 ; idx < MAX_KEY_PAIRS; idx++) {
        char keyName[64] = {0};
        uint32_t buffer_size = 1024;
        snprintf(keyName, sizeof(keyName), "%s_%d", CFG_KEY_HAP_CTRL_LTPK, (int16_t)idx);
        nvdm_status_t status = nvdm_read_data_item(NVDM_GROUP_HOMEKIT, keyName, buffer, &buffer_size);
        LOG_I(common, "HAP cli: homekit_check start CTRL_LTPK %d", status);
        if (status != NVDM_STATUS_OK) {
            nvdm_status_t status = nvdm_write_data_item(NVDM_GROUP_HOMEKIT,
                                   keyName,
                                   NVDM_DATA_ITEM_TYPE_STRING,
                                   (const uint8_t *)(""),
                                   sizeof(""));
            LOG_I(common, "HAP cli: homekit_check start CTRL_LTPK %d", status);
            if (status != NVDM_STATUS_OK) {
                LOG_I(common, "homekit_check CTRL_LTPK error: %s", g_homekit_data_item_array[idx].item_name);
            }
        }
    }
    vPortFree(buffer);
    LOG_I(common, "homekit_check end");
}

static void homekit_reset_to_default(void)
{
    reset_to_default("homekit",
                     (group_data_item_t *)g_homekit_data_item_array,
                     sizeof(g_homekit_data_item_array) / sizeof(g_homekit_data_item_array[0]));

    for (uint32_t idx = 0 ; idx < MAX_KEY_PAIRS; idx++) {
        char keyName[64] = {0};
        snprintf(keyName, sizeof(keyName), "%s_%d", CFG_KEY_HAP_CTRL_LTPK, (int16_t)idx);
        nvdm_status_t status = nvdm_write_data_item(NVDM_GROUP_HOMEKIT,
                               keyName,
                               NVDM_DATA_ITEM_TYPE_STRING,
                               (const uint8_t *)(""),
                               sizeof(""));
        if (status != NVDM_STATUS_OK) {
            LOG_I(common, "          error: %s", keyName);
        }
    }
    LOG_I(common, "homekit_reset end.         System will reboot now... ...");
    hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
}

static void homekit_show_value(void)
{
    show_group_value("homekit",
                     (group_data_item_t *)g_homekit_data_item_array,
                     sizeof(g_homekit_data_item_array) / sizeof(g_homekit_data_item_array[0]));

    LOG_I(common, "homekit_show_value start");

    uint8_t *buffer = NULL;
    buffer = (uint8_t *)pvPortMalloc(1024 * sizeof(uint8_t));
    if (buffer == NULL) {
        LOG_E(common, "homekit_show_value, failed to malloc");
        return;
    }
    memset(buffer, 0, 1024 * sizeof(uint8_t));

    for (uint32_t idx = 0 ; idx < MAX_KEY_PAIRS; idx++) {
        char keyName[64] = {0};
        uint32_t buffer_size = 1024;
        snprintf(keyName, sizeof(keyName), "%s_%d", CFG_KEY_HAP_CTRL_LTPK, (int16_t)idx);
        nvdm_status_t status = nvdm_read_data_item(NVDM_GROUP_HOMEKIT, keyName, buffer, &buffer_size);
        if (status == NVDM_STATUS_OK) {
            LOG_I(common, "          %s: %s", keyName, buffer);
        } else {
            LOG_I(common, "          error: %s %d", keyName, status);
        }
    }

    vPortFree(buffer);
    LOG_I(common, "homekit_show_value end");
}
#endif

typedef struct {
    const char *group_name;
    void (*check_default_value)(void);
    void (*reset_default_value)(void);
    void (*show_value)(void);
} user_data_item_operate_t;

static const user_data_item_operate_t user_data_item_operate_array[] = {
    {
        "common",
        common_check_default_value,
        common_reset_to_default,
        common_show_value,
    },
    {
        "STA",
        sta_check_default_value,
        sta_reset_to_default,
        sta_show_value,
    },
    {
        "AP",
        ap_check_default_value,
        ap_reset_to_default,
        ap_show_value,
    },
#ifdef MTK_HOMEKIT_ENABLE
    {
        "homekit",
        homekit_check_default_value,
        homekit_reset_to_default,
        homekit_show_value,
    },
#endif
};

/* This function is used to check whether data is exist in NVDM region,
 * and write default value to NVDM region if no value can be found in NVDM region.
 */
void user_check_default_value(void)
{
    uint32_t index;
    uint32_t max = sizeof(user_data_item_operate_array) / sizeof(user_data_item_operate_t);

    for (index = 0; index < max; index++) {
        user_data_item_operate_array[index].check_default_value();
    }
}

void user_data_item_reset_to_default(char *group_name)
{
    uint32_t index;
    uint32_t max = sizeof(user_data_item_operate_array) / sizeof(user_data_item_operate_t);

    if (group_name == NULL) {
        for (index = 0; index < max; index++) {
            user_data_item_operate_array[index].reset_default_value();
        }
    } else {
        for (index = 0; index < max; index++) {
            if (memcmp(user_data_item_operate_array[index].group_name, group_name,
                       strlen(user_data_item_operate_array[index].group_name)) == 0) {
                user_data_item_operate_array[index].reset_default_value();
                break;
            }
        }
    }
}

void user_data_item_show_value(char *group_name)
{
    uint32_t index;
    uint32_t max = sizeof(user_data_item_operate_array) / sizeof(user_data_item_operate_t);

    if (group_name == NULL) {
        for (index = 0; index < max; index++) {
            user_data_item_operate_array[index].show_value();
        }
    } else {
        for (index = 0; index < max; index++) {
            if (memcmp(user_data_item_operate_array[index].group_name, group_name,
                       strlen(user_data_item_operate_array[index].group_name)) == 0) {
                user_data_item_operate_array[index].show_value();
                break;
            }
        }
    }
}

