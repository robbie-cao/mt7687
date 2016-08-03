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
#include <stdlib.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"
#include "smt_core.h"
#include "smt_conn.h"
#include "smt_api.h"
#include "wifi_api.h"
#include "wifi_scan.h"
#include "semphr.h"
#include "syslog.h"
#include "wifi_rx_desc.h"
#include "nvdm.h"

#if (CFG_SUPPORT_SMNT_PROTO != 0)

log_create_module(smnt, PRINT_LEVEL_INFO);

#define smtcn_debug 1
#if (smtcn_debug == 1)
unsigned char channel_times[14] = {0};
#endif

#define SMTCN_RX_QUEUE_SIZE        4
#define USE_SMTCN_STATIC_BUF

#ifdef USE_SMTCN_STATIC_BUF
#define SMTCN_STATIC_BUF_COUNT     (SMTCN_RX_QUEUE_SIZE + 1)
#define SMTCN_STATIC_BUF_SIZE      64

#ifdef __ICCARM__
_Pragma("data_alignment=4") uint8_t  smtcn_rx_buf[SMTCN_STATIC_BUF_COUNT][SMTCN_STATIC_BUF_SIZE] = {{0}};
#else
uint8_t __attribute__((aligned(4))) smtcn_rx_buf[SMTCN_STATIC_BUF_COUNT][SMTCN_STATIC_BUF_SIZE] = {{0}};
#endif

uint8_t smtcn_rx_buf_idx = 0;
#endif


#if (CFG_SUPPORT_SMNT_PROTO == 2)
extern smtcn_proto_ops elian_proto_ops;
#elif (CFG_SUPPORT_SMNT_PROTO == 3)
extern smtcn_proto_ops ak_proto_ops;
#endif

SemaphoreHandle_t smart_config_mutex = NULL;
xQueueHandle smtcn_rx_queue = NULL;

/* smart connection thread wait scan done.*/
SemaphoreHandle_t scan_done_mutex = NULL;

typedef struct {
    uint8_t    *payload;
    uint32_t    len;
} raw_pkt_t, *praw_pkt_t;

smtcn_proto_ops *psmtcn_proto_ops;
smtcn_info   saved_smtcn_info;
wifi_smart_connection_callback_t smtcn_evt_cb = NULL;

#define scan_timems   (2 * 1000)    /* 2s */
#define scan_timeticks (scan_timems/portTICK_PERIOD_MS)

#define switch_channel_timeticks (switch_channel_timems/portTICK_PERIOD_MS)

#if (use_smart_connection_filter == 0)
#else
static uint32_t origin_rxfilter = 0;
#endif
static uint8_t origin_opmode = 0;
static uint8_t origin_bw = 0;

unsigned char channel_list[] = {
    1, 6, 11, 2, 5, 7, 1, 6, 11, 10, 12, 3, 1, 6, 11, 8, 13, 4, 9, 14
};

static int8_t scan_loop_count = 0;

/*smart connection swtich channel timer */
static TimerHandle_t switch_channel_timer = NULL;

static TimerHandle_t scan_timer = NULL;

static struct {
    uint16_t  search_idx;
    uint8_t   cur_chanl;
} smt_conn_st;


/* static function for scan */
static int scan_prb_rsp_handler(wifi_event_t event_id, unsigned char *payload, unsigned int len);
static int check_scan(wifi_event_t event_id, unsigned char *payload, unsigned int len);

#if 1
int smtcn_raw_pkt_handler(unsigned char *payload, unsigned int len)
{
    P_HW_MAC_RX_DESC_T        pRxWI;
    uint8_t          ucRxPktType;
    uint8_t          ucMacHeaderLength;
    uint16_t         u2PayloadLength;
    uint8_t *pucMacHeader;       /* 802.11 header  */
    uint8_t *pucPayload;         /* 802.11 payload */
    uint16_t         u2RxStatusOffst;


    pRxWI   = (P_HW_MAC_RX_DESC_T)(payload);
    // =======  Process RX Vector (if any) ========

    // 1. ToDo: Cache RXStatus in SwRfb
    u2RxStatusOffst = sizeof(HW_MAC_RX_DESC_T);
    ucRxPktType = HAL_RX_STATUS_GET_PKT_TYPE(pRxWI);

    /*
        LOG_HEXDUMP_I(smnt, "raw packet", payload, len);
        LOG_I(smnt, "pRxWI.u2RxByteCount:[0x%04x]", pRxWI->u2RxByteCount);
        LOG_I(smnt,"pRxWI.u2PktTYpe:[0x%04x]", pRxWI->u2PktTYpe);
        LOG_I(smnt,"pRxWI.ucMatchPacket:[0x%02x]", pRxWI->ucMatchPacket);
        LOG_I(smnt,"pRxWI.ucChanFreq:[0x%02x]", pRxWI->ucChanFreq);
        LOG_I(smnt,"pRxWI.ucHeaderLen:[0x%02x]", pRxWI->ucHeaderLen);
        LOG_I(smnt,"pRxWI.ucBssid:[0x%02x]", pRxWI->ucBssid);
        LOG_I(smnt,"ucRxPktType:[0x%02x]", ucRxPktType);
    */

    if ((ucRxPktType != RX_PKT_TYPE_RX_DATA) && (ucRxPktType != RX_PKT_TYPE_SW_DEFINED)) {
        return 0; // not handled
    }

    if (ucRxPktType == RX_PKT_TYPE_RX_DATA ||
            ucRxPktType == RX_PKT_TYPE_DUP_RFB) {
        uint16_t          u2GrpOffst;
        uint8_t              ucGroupVLD;

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
    pucMacHeader = (uint8_t *)(((uint32_t)pRxWI + (HAL_RX_STATUS_GET_HEADER_OFFSET(pRxWI)) + u2RxStatusOffst) & 0xFFFFFFFE);
    ucMacHeaderLength = HAL_RX_STATUS_GET_HEADER_LEN(pRxWI);

    pucPayload = (uint8_t *)((uint32_t)(pucMacHeader + ucMacHeaderLength) & 0xFFFFFFFC);
    u2PayloadLength = (uint16_t)(HAL_RX_STATUS_GET_RX_BYTE_CNT(pRxWI) - ((uint32_t)pucPayload - (uint32_t)pRxWI));

    //LOG_HEXDUMP_I(smnt, "mac header begin", pucMacHeader, ucMacHeaderLength+u2PayloadLength);
    psmtcn_proto_ops->rx_handler((char *)pucMacHeader, ucMacHeaderLength + u2PayloadLength);

    return 1; /* handled */
}
#endif

static void switch_next_channel(TimerHandle_t tmr)
{
    unsigned char channel_list_size;
    if (psmtcn_proto_ops && psmtcn_proto_ops->switch_channel_rst) {
        if (0 != psmtcn_proto_ops->switch_channel_rst()) {
            LOG_E(smnt, "switch_channel_rst fail.");
        }
    }

    channel_list_size = sizeof(channel_list) / sizeof(channel_list[0]);
    if (++smt_conn_st.search_idx >= channel_list_size) {
        smt_conn_st.search_idx = 0;
    }

    smt_conn_st.cur_chanl = channel_list[smt_conn_st.search_idx];
    wifi_config_set_channel(WIFI_PORT_STA, smt_conn_st.cur_chanl);

#if (smtcn_debug == 1)
    channel_times[smt_conn_st.cur_chanl - 1]++;
    /*
        uint8_t channel;
        wifi_config_get_channel(WIFI_PORT_STA, &channel);
        sc_dbg("now switched to channel :%d\n", channel);
        */
#endif
}

static void sc_connect(void)
{
    __g_wpa_supplicant_api.wpa_supplicant_disable_trigger_scan();
    wifi_connection_stop_scan();

    wifi_config_set_ssid(WIFI_PORT_STA, saved_smtcn_info.ssid, saved_smtcn_info.ssid_len);
    //wifi_config_set_channel(WIFI_PORT_STA, saved_smtcn_info.channel);

    if (saved_smtcn_info.auth_mode == WIFI_AUTH_MODE_OPEN) {
        if(saved_smtcn_info.encrypt_type == WIFI_ENCRYPT_TYPE_WEP_ENABLED){
            wifi_wep_key_t wep_key;
            if (saved_smtcn_info.pwd_len == 10 ||
                saved_smtcn_info.pwd_len == 26)
            {
                wep_key.wep_key_length[0] = saved_smtcn_info.pwd_len / 2;
                AtoH((char *)saved_smtcn_info.pwd, (char *)&wep_key.wep_key[0], (int)wep_key.wep_key_length[0]);
            } else if (saved_smtcn_info.pwd_len == 5 ||
                        saved_smtcn_info.pwd_len == 13)
            {
                wep_key.wep_key_length[0] = saved_smtcn_info.pwd_len;
                memcpy(wep_key.wep_key[0], saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);
            } else
            {
                printf("wep key length error.\n");
                return;
            }

            wep_key.wep_tx_key_index = 0;
            wifi_config_set_wep_key(WIFI_PORT_STA, &wep_key);
            wifi_config_set_security_mode(WIFI_PORT_STA, WIFI_AUTH_MODE_OPEN, WIFI_ENCRYPT_TYPE_WEP_ENABLED);
        } else {
            wifi_config_set_security_mode(WIFI_PORT_STA, WIFI_AUTH_MODE_OPEN, WIFI_ENCRYPT_TYPE_WEP_DISABLED);
        }
    } else {
        wifi_config_set_security_mode(WIFI_PORT_STA, saved_smtcn_info.auth_mode, saved_smtcn_info.encrypt_type);
        wifi_config_set_wpa_psk_key(WIFI_PORT_STA, saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);
    }

    LOG_I(smnt, "\n>>>>>> start connect <<<<<<:\n\n");

    //wifi_connection_stop_scan();
    __g_wpa_supplicant_api.wpa_supplicant_enable_trigger_scan();
    wifi_config_reload_setting();
}

int32_t sc_save_smtcn_info(void)
{
/*
    wifi_profile_set_opmode(WIFI_MODE_STA_ONLY);    
    wifi_profile_set_ssid(WIFI_PORT_STA, saved_smtcn_info.ssid, saved_smtcn_info.ssid_len);
    wifi_profile_set_security_mode(WIFI_PORT_STA, saved_smtcn_info.auth_mode, saved_smtcn_info.encrypt_type);
    if(saved_smtcn_info.auth_mode != WIFI_AUTH_MODE_OPEN){
        wifi_profile_set_wpa_psk_key(WIFI_PORT_STA, saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);
    }
    if((saved_smtcn_info.auth_mode == WIFI_AUTH_MODE_OPEN) &&
       (saved_smtcn_info.encrypt_type == WIFI_ENCRYPT_TYPE_WEP_ENABLED))
    {
        if (saved_smtcn_info.pwd_len == 5 ||
            saved_smtcn_info.pwd_len == 13 ||
            saved_smtcn_info.pwd_len == 10 ||
            saved_smtcn_info.pwd_len == 26)
        {
            wifi_wep_key_t wep_key;
            wep_key.wep_tx_key_index = 0;
            wep_key.wep_key_length[0] = saved_smtcn_info.pwd_len;
            memcpy(wep_key.wep_key[0], saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);
            wifi_profile_set_wep_key(WIFI_PORT_STA, &wep_key);
        }
    }
*/

    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%d", WIFI_MODE_STA_ONLY);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode",
                        NVDM_DATA_ITEM_TYPE_STRING,
                        (uint8_t *)buf, strlen(buf));

    memset(buf, 0, sizeof(buf));
    memcpy(buf, saved_smtcn_info.ssid, saved_smtcn_info.ssid_len);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Ssid",
                        NVDM_DATA_ITEM_TYPE_STRING,
                        (uint8_t *)buf, strlen(buf));

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", saved_smtcn_info.ssid_len);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SsidLen",
                        NVDM_DATA_ITEM_TYPE_STRING,
                        (uint8_t *)buf, strlen(buf));

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", saved_smtcn_info.auth_mode);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "AuthMode",
                        NVDM_DATA_ITEM_TYPE_STRING,
                        (uint8_t *)buf, strlen(buf));

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", saved_smtcn_info.encrypt_type);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "EncrypType",
                        NVDM_DATA_ITEM_TYPE_STRING,
                        (uint8_t *)buf, strlen(buf));

    if(saved_smtcn_info.auth_mode != WIFI_AUTH_MODE_OPEN){
        memset(buf, 0, sizeof(buf));
        memcpy(buf, saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);
        nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPsk",
                            NVDM_DATA_ITEM_TYPE_STRING,
                            (uint8_t *)buf, strlen(buf));

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", saved_smtcn_info.pwd_len);
        nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPskLen",
                            NVDM_DATA_ITEM_TYPE_STRING,
                            (uint8_t *)buf, strlen(buf));
    }

    if((saved_smtcn_info.auth_mode == WIFI_AUTH_MODE_OPEN) &&
       (saved_smtcn_info.encrypt_type == WIFI_ENCRYPT_TYPE_WEP_ENABLED))
    {
        if (saved_smtcn_info.pwd_len == 5 ||
            saved_smtcn_info.pwd_len == 13 ||
            saved_smtcn_info.pwd_len == 10 ||
            saved_smtcn_info.pwd_len == 26)
        {
            wifi_wep_key_t wep_key;
            wep_key.wep_tx_key_index = 0;
            wep_key.wep_key_length[0] = saved_smtcn_info.pwd_len;
            memcpy(wep_key.wep_key[0], saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);

            memset(buf, 0, sizeof(buf));
            memcpy(buf, wep_key.wep_key[0], wep_key.wep_key_length[0]);
            buf[wep_key.wep_key_length[0]] = ',';
            nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKey",
                                NVDM_DATA_ITEM_TYPE_STRING,
                                (uint8_t *)buf, strlen(buf));

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%d,", wep_key.wep_key_length[0]);
            nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKeyLen",
                                NVDM_DATA_ITEM_TYPE_STRING,
                                (uint8_t *)buf, strlen(buf));

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%d", wep_key.wep_tx_key_index);
            nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "DefaultKeyId",
                                NVDM_DATA_ITEM_TYPE_STRING,
                                (uint8_t *)buf, strlen(buf));   
        }
    }
    
    return 0;
}

static int32_t sc_scan_done(void)
{
    wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE, (wifi_event_handler_t)scan_prb_rsp_handler);
    //wifi_connection_register_event_handler(WIFI_EVENT_IOT_NONE, (wifi_event_handler_t)scan_prb_rsp_handler);
    wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE, (wifi_event_handler_t)check_scan);
    //wifi_connection_register_event_handler(WIFI_EVENT_IOT_NONE, (wifi_event_handler_t)check_scan);

    return 0;
}

static void next_scan_loop(const char *reason)
{
    LOG_I(smnt, "%s\n", reason);
    taskENTER_CRITICAL();
    if ((SCAN_FLAG_FIN == saved_smtcn_info.scan_flag) ||
            (SCAN_FLAG_NOT_FOUND == saved_smtcn_info.scan_flag)) {
        taskEXIT_CRITICAL();
        return;
    }

    if (++scan_loop_count >= max_scan_loop_count) {
        saved_smtcn_info.scan_flag = SCAN_FLAG_NOT_FOUND;
        sc_scan_done();

        if (scan_done_mutex != NULL) {
            xSemaphoreGive(scan_done_mutex);
        }
        taskEXIT_CRITICAL();
    } else {
        if (scan_timer != NULL) {
            xTimerStart(scan_timer, tmr_nodelay);
        }
        taskEXIT_CRITICAL();
        wifi_connection_start_scan(NULL, 0, NULL, 0, 0);
        LOG_I(smnt, "loop %d scan\n", scan_loop_count);
    }

}

static void scan_timeout(TimerHandle_t tmr)
{
    wifi_connection_stop_scan();

    /* Notice:
    * Here may need a semaphore to protect scan_loop_count against check_scan.
    */
    next_scan_loop("scan timeout");
}

static int check_scan(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    if (event_id != WIFI_EVENT_IOT_SCAN_COMPLETE) {
        return 0;
    }

    /* Notice:
    * Here may need a semaphore to protect scan_loop_count against scan_timeout.
    */
    xTimerStop(scan_timer, tmr_nodelay);
    next_scan_loop("scan complete event");
    return 1;
}

static int scan_prb_rsp_handler(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    if (event_id != WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE) {
        return 1;
    }

    if (len != 0) {
        wifi_scan_list_item_t ap_data;
        memset(&ap_data, 0, sizeof(wifi_scan_list_item_t));
        if (wifi_connection_parse_beacon(payload, len, &ap_data) >= 0) {
            if ((ap_data.ssid_length == saved_smtcn_info.ssid_len) &&
                    !memcmp(ap_data.ssid, saved_smtcn_info.ssid, ap_data.ssid_length)) {
                xTimerStop(scan_timer, tmr_nodelay);
#if (smtcn_debug == 1)
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
#endif
                /*
                sc_dbg("channel:[%d], central_channel:[%d]\n",
                    ap_data.channel, ap_data.central_channel);
                    */

                saved_smtcn_info.auth_mode = ap_data.auth_mode;
                saved_smtcn_info.channel = ap_data.channel;
                saved_smtcn_info.encrypt_type = ap_data.encrypt_type;

                LOG_I(smnt, "scan finished!\n");

#if (smtcn_debug == 1)
                uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID + 1] = {0};
                uint8_t passwd[WIFI_LENGTH_PASSPHRASE + 1] = {0};
                memcpy(ssid, saved_smtcn_info.ssid, saved_smtcn_info.ssid_len);
                memcpy(passwd, saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);

                LOG_I(smnt, "ssid:[%s], channel:[%d], authMode:[%d], encrypt type:[%d], psk:[%s]\n",
                      ssid, saved_smtcn_info.channel,
                      saved_smtcn_info.auth_mode, saved_smtcn_info.encrypt_type,
                      passwd);
#endif

                taskENTER_CRITICAL();
                saved_smtcn_info.scan_flag = SCAN_FLAG_FIN;
                taskEXIT_CRITICAL();
                sc_scan_done();
                xSemaphoreGive(scan_done_mutex);
            }
            return 0;
        }
    }

    return 1;
}

static int32_t sc_scan(void)
{
    LOG_I(smnt, "\n>>>>>> start scan <<<<<<\n\n");

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE,
                                           (wifi_event_handler_t) scan_prb_rsp_handler);

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE,
                                           (wifi_event_handler_t) check_scan);

    /*
        wifi_connection_start_scan(saved_smtcn_info.ssid,
                                   saved_smtcn_info.ssid_len,
                                   NULL, 0, 2);
                                   */
    wifi_connection_start_scan(NULL, 0, NULL, 0, 0);
    xTimerStart(scan_timer, tmr_nodelay);

    return 0;
}

static void sc_scan_cleanup(void)
{
    if (scan_done_mutex != NULL) {
        vSemaphoreDelete(scan_done_mutex);
        scan_done_mutex = NULL;
    }

    if (scan_timer != NULL) {
        xTimerDelete(scan_timer, tmr_nodelay);
        scan_timer = NULL;
    }
}

static int sc_scan_init(void)
{
    if (scan_done_mutex == NULL) {
        scan_done_mutex = xSemaphoreCreateCounting((UBaseType_t)1, (UBaseType_t)0);
        if (scan_done_mutex == NULL) {
            LOG_E(smnt, "scan_done_mutex create failed.\n");
            return -1;
        }
    }

	saved_smtcn_info.scan_flag = SCAN_FLAG_NONE;
	
    scan_timer = xTimerCreate("scan_timer",
                              scan_timeticks,
                              pdFALSE,
                              NULL,
                              scan_timeout);
    if (scan_timer == NULL) {
        LOG_E(smnt, "switch_channel_timer create fail.\n");
        goto fail;
    }

    {
        {
        wifi_config_set_ssid(WIFI_PORT_STA, saved_smtcn_info.ssid, saved_smtcn_info.ssid_len);
        if (saved_smtcn_info.pwd_len != 0) {
            wifi_config_set_wpa_psk_key(WIFI_PORT_STA, saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);
        }

        wifi_config_reload_setting();
        }

        uint8_t opmode = 0;
        wifi_config_get_opmode(&opmode);
        if((opmode != WIFI_MODE_STA_ONLY) &&
            (wifi_config_set_opmode(WIFI_MODE_STA_ONLY) < 0)){
            printf("config sta mode failed, cannot continue smart config\n");
            goto fail;
        }
        LOG_I(smnt, "Now STA only mode\n");
    }

    scan_loop_count = 0;
    return 0;

fail:
    sc_scan_cleanup();
    return -1;
}

extern int g_supplicant_ready;
static int32_t smtcn_scan_connect_ap(void)
{
    while(!g_supplicant_ready){
        vTaskDelay(50);
    }
    LOG_I(smnt, "Now start scan and connect.\n");
    if (sc_scan_init() != 0) {
        printf("sc_scan initialize failed.\n");
        return -1;
    }
    sc_scan();
    xSemaphoreTake(scan_done_mutex, portMAX_DELAY);
    sc_scan_cleanup();

    if(SCAN_FLAG_FIN == saved_smtcn_info.scan_flag){
        sc_connect();
    }else if(SCAN_FLAG_NOT_FOUND == saved_smtcn_info.scan_flag){
#if (smtcn_debug == 1)
        uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID+1] = {0};
        memcpy(ssid, saved_smtcn_info.ssid, WIFI_MAX_LENGTH_OF_SSID);
        LOG_I(smnt, "ssid:[%s] not found,"
            " 7687 may connect to the original AP.\n", ssid);
#else
        printf("smart connection target AP not found,"
            " 7687 may connect to the original AP.\n", ssid);
#endif
    }

    return 0;
}

int32_t smtcn_rx_handler(uint8_t *payload, uint32_t len)
{
    raw_pkt_t     pkt;

#ifdef USE_SMTCN_STATIC_BUF
    //int32_t  freecount=0;
    pkt.len = len;
    pkt.payload  = smtcn_rx_buf[smtcn_rx_buf_idx];
    memcpy(pkt.payload, payload, SMTCN_STATIC_BUF_SIZE);
    if (xQueueSendToBack(smtcn_rx_queue, (void *)&pkt, (TickType_t)5) == pdPASS) {
        smtcn_rx_buf_idx = (smtcn_rx_buf_idx + 1) % SMTCN_STATIC_BUF_COUNT;
    }
#else
    /* assign pkt */
    pkt.len = len;
    if ((pkt.payload = pvPortMalloc(len)) == NULL) {
        LOG_E(smnt, "%s:%d,pvPortMalloc failed\n", __FILE__, __LINE__);
        return 1;
    }
    memcpy(pkt.payload, payload, len);

    if (xQueueSendToBack(smtcn_rx_queue, (void *)&pkt, (TickType_t)5) != pdPASS){
        vPortFree(pkt.payload);
        //printf("can't add a job to smart connect queue\n");
    }
#endif

    return 1;
}

uint8_t smtcn_current_channel(void)
{
    return smt_conn_st.cur_chanl;
}
void smtcn_stop_switch(void)
{
    xTimerStop(switch_channel_timer, tmr_nodelay);
    if(smtcn_evt_cb) {
        (*smtcn_evt_cb)(WIFI_SMART_CONNECTION_EVENT_CHANNEL_LOCKED, NULL);
    }
#if (smtcn_debug == 1)
    LOG_I(smnt, "channel locked at %d, scaned %d times\n",
          smt_conn_st.cur_chanl,
          channel_times[smt_conn_st.cur_chanl - 1]);
#endif
}

void smtcn_continue_switch(void)
{
    switch_next_channel(NULL);
    xTimerStart(switch_channel_timer, tmr_nodelay);
}

int smtcn_done(void)
{
#if (use_smart_connection_filter == 0)
    wifi_config_set_opmode(origin_opmode);
#else
    wifi_config_set_rx_filter(origin_rxfilter);
    wifi_config_set_smart_connection_filter(0);
#endif
    wifi_config_set_bandwidth(WIFI_PORT_STA, origin_bw);
    wifi_config_unregister_rx_handler();
    LOG_I(smnt, "unregister rx handler finished.\n");

    raw_pkt_t pkt;
    while (xQueueReceive(smtcn_rx_queue, (void *)&pkt, (TickType_t)0) == pdPASS) {
#ifndef USE_SMTCN_STATIC_BUF
        vPortFree(pkt.payload);
#endif
    }

    return 0;
}

uint8_t atomic_read_smtcn_flag(void)
{
    return saved_smtcn_info.smtcn_flag;;
}

void atomic_write_smtcn_flag(uint8_t flag_value)
{
    saved_smtcn_info.smtcn_flag = flag_value;
}

int32_t mtk_scan_connect_ap(unsigned char *ssid, unsigned char *passwd)
{
    if(ssid == NULL)
        return -1;

    if(ssid != saved_smtcn_info.ssid){
        memcpy(saved_smtcn_info.ssid, ssid, strlen((const char*)ssid));
        saved_smtcn_info.ssid_len = strlen((const char*)ssid);
    }

    if(passwd != saved_smtcn_info.pwd){
        memcpy(saved_smtcn_info.pwd, passwd, strlen((const char*)passwd));
        saved_smtcn_info.pwd_len = strlen((const char*)passwd);
    }

    return smtcn_scan_connect_ap();
}

static void smtcn_rcv_pkt_thread(void *pvParameters)
{
    if (smart_config_mutex == NULL) {
        LOG_E(smnt, "mutex not created, please create the mutex and then call this function.\n");
        vTaskDelete(NULL);
    }
    xSemaphoreTake(smart_config_mutex, portMAX_DELAY);
    LOG_I(smnt, "\n>>>>>> Smart connect begin <<<<<<\n\n");

    memset((void *)&saved_smtcn_info, 0, sizeof(saved_smtcn_info));

    uint8_t smtcn_flag = 0;
    while ((SMTCN_FLAG_FIN != smtcn_flag) &&
            (SMTCN_FLAG_EXIT != smtcn_flag) &&
            (SMTCN_FLAG_FAIL != smtcn_flag)) {
        raw_pkt_t pkt;
        if (xQueueReceive(smtcn_rx_queue, (void *)&pkt, portMAX_DELAY) == pdPASS) {
            /* process this raw packet */
            smtcn_raw_pkt_handler(pkt.payload, pkt.len);
#ifndef USE_SMTCN_STATIC_BUF
            vPortFree(pkt.payload);
#endif
        }

        smtcn_flag = atomic_read_smtcn_flag();
    }

    smtcn_done();
    LOG_I(smnt, "\n>>>>>> Smart connect finished <<<<<<\n");
    if((SMTCN_FLAG_EXIT == smtcn_flag) || (SMTCN_FLAG_FAIL == smtcn_flag)){
        wifi_smart_connection_deinit();
        goto thread_exit;
    }

    if(smtcn_evt_cb){
        (*smtcn_evt_cb)(WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED, NULL);
    }

thread_exit:
    LOG_I(smnt, "give smart_config_mutex:[0x%0x]\n", (int)smart_config_mutex);
    xSemaphoreGive(smart_config_mutex);
    vTaskDelete(NULL);
}

void wifi_smart_connection_deinit(void)
{
    if (smtcn_rx_queue != NULL) {
        vQueueDelete(smtcn_rx_queue);
        smtcn_rx_queue = NULL;
    }

    if (switch_channel_timer != NULL) {
        xTimerDelete(switch_channel_timer, tmr_nodelay);
        switch_channel_timer = NULL;
    }

    if (psmtcn_proto_ops && psmtcn_proto_ops->cleanup) {
        psmtcn_proto_ops->cleanup();
    }

    smtcn_evt_cb = NULL;
}

wifi_smart_connection_status_t wifi_smart_connection_init (const uint8_t *key, const uint8_t key_length, wifi_smart_connection_callback_t  callback)
{
    if(psmtcn_proto_ops == NULL){
        //psmtcn_proto_ops = &ak_proto_ops;
        //psmtcn_proto_ops = &elian_proto_ops;
    #if (CFG_SUPPORT_SMNT_PROTO == 2)
        psmtcn_proto_ops = &elian_proto_ops;
    #elif (CFG_SUPPORT_SMNT_PROTO == 3)
        psmtcn_proto_ops = &ak_proto_ops;
    #endif
    }

    if (smart_config_mutex == NULL) {
        smart_config_mutex = xSemaphoreCreateMutex();
        if (smart_config_mutex == NULL) {
            LOG_E(smnt, "smart_config_mutex create failed.\n");
            return WIFI_SMART_CONNECTION_ERROR;
        }
    }

    if (smtcn_rx_queue == NULL) {
        smtcn_rx_queue = xQueueCreate(SMTCN_RX_QUEUE_SIZE, sizeof(raw_pkt_t));
        if (smtcn_rx_queue == NULL) {
            LOG_E(smnt, "smtcn_rx_queue create failed.\n");
            goto fail;
        }
        configASSERT(smtcn_rx_queue);
        vQueueAddToRegistry(smtcn_rx_queue, "smart connect");
    }

    switch_channel_timer = xTimerCreate("switch_channel_timer",
                                        switch_channel_timeticks,
                                        pdTRUE,
                                        NULL,
                                        switch_next_channel);
    if (switch_channel_timer == NULL) {
        LOG_E(smnt, "switch_channel_timer create fail.\n");
        goto fail;
    }

    smtcn_evt_cb = callback;
    if (psmtcn_proto_ops && psmtcn_proto_ops->init)
        if (psmtcn_proto_ops->init(key, key_length) != 0) {
            goto fail;
        }

    return WIFI_SMART_CONNECTION_OK;

fail:
    wifi_smart_connection_deinit();
    return WIFI_SMART_CONNECTION_ERROR;
}

wifi_smart_connection_status_t wifi_smart_connection_start (uint16_t timeout_seconds)
{
    static TaskHandle_t smtcn_handle = NULL;
    eTaskState smtcn_state;
    
    wifi_config_get_opmode(&origin_opmode);
    if (origin_opmode == WIFI_MODE_AP_ONLY) {
        LOG_W(smnt, "7687 in opmode:[%d], cannot continue smart connection\n",
              origin_opmode);
        return WIFI_SMART_CONNECTION_ERROR;
    }

    /* Here is just to avoid creating too many pending tasks which consume
     * too much memory.
     */
    if ((smtcn_handle != NULL) &&
            ((smtcn_state = eTaskGetState(smtcn_handle)) != eReady) &&
            (smtcn_state != eDeleted)) {
        LOG_E(smnt, "smtcn task state:%d, cannot create it\n", smtcn_state);
        return WIFI_SMART_CONNECTION_ERROR;
    }
    if (xTaskCreate(smtcn_rcv_pkt_thread, "smart connection thread", 512, NULL, configTIMER_TASK_PRIORITY, &smtcn_handle) != pdPASS) {
        LOG_E(smnt, "smtcn_rcv_pkt_thread create failed\n");
        return WIFI_SMART_CONNECTION_ERROR;
    }

#if (use_smart_connection_filter == 0)
    wifi_config_set_opmode(WIFI_MODE_MONITOR);
#else
    uint32_t rx_filter;
    wifi_config_get_rx_filter(&origin_rxfilter);
    rx_filter = origin_rxfilter | BIT(WIFI_RX_FILTER_RM_FRAME_REPORT_EN);
    rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_NOT_MY_BSSID);
    rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_NOT_UC2ME);
    rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_MC_FRAME);
    wifi_config_set_rx_filter(rx_filter);

    wifi_config_set_smart_connection_filter(1);
#endif
    wifi_config_get_bandwidth(WIFI_PORT_STA, &origin_bw);
    wifi_config_set_bandwidth(WIFI_PORT_STA, WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ);

    wifi_config_register_rx_handler((wifi_rx_handler_t)smtcn_rx_handler);

    memset((void *)&smt_conn_st, 0, sizeof(smt_conn_st));
    smt_conn_st.cur_chanl = channel_list[0];

    wifi_config_set_channel(WIFI_PORT_STA, smt_conn_st.cur_chanl);
#if (smtcn_debug == 1)
    memset(channel_times, 0, sizeof(channel_times));
    channel_times[smt_conn_st.cur_chanl - 1]++;
#endif
    xTimerStart(switch_channel_timer, tmr_nodelay);

    return WIFI_SMART_CONNECTION_OK;
}

wifi_smart_connection_status_t wifi_smart_connection_stop(void)
{
    atomic_write_smtcn_flag(SMTCN_FLAG_EXIT);
    return WIFI_SMART_CONNECTION_OK;
}

wifi_smart_connection_status_t wifi_smart_connection_get_result (uint8_t *ssid, uint8_t *ssid_length, 
                                            uint8_t *password, uint8_t *password_length, 
                                            uint8_t *tlv_data, uint8_t *tlv_data_length)

{
    if(saved_smtcn_info.ssid_len > 0){
        if(ssid != NULL){
            memcpy(ssid, saved_smtcn_info.ssid, saved_smtcn_info.ssid_len);
        }
        if(ssid_length != NULL){
            *ssid_length = saved_smtcn_info.ssid_len;
        }
    }

    if(saved_smtcn_info.pwd_len > 0){
        if(password != NULL){
            memcpy(password, saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);
        }
        if(password_length != NULL){
            *password_length = saved_smtcn_info.pwd_len;
        }
    }

    if(saved_smtcn_info.tlv_data != NULL){
        if(tlv_data != NULL){
            memcpy(tlv_data, saved_smtcn_info.tlv_data, saved_smtcn_info.tlv_data_len); 
        }
        if(tlv_data_length != NULL){
            *tlv_data_length = saved_smtcn_info.tlv_data_len;
        }
    }
    
    return WIFI_SMART_CONNECTION_OK;
}

#endif
