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

#ifndef __WIFI_PRIVATE_H__
#define __WIFI_PRIVATE_H__

#include "stdint.h"
#include "type_def.h"
#include "wifi_api.h"
#include "wifi_inband.h"


#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_INBAND_CMD_IS_QUERY_SET            (0)
#define WIFI_INBAND_CMD_IS_QUERY_QUERY          (1)
#define WIFI_INBAND_CMD_IS_QUERY_NEED_STATUS    (2)

#define MGMT_PKT_FILTER_ASSOC_REQ    (0x1<<0)
#define MGMT_PKT_FILTER_ASSOC_RESP   (0x1<<1)
#define MGMT_PKT_FILTER_REASSOC_REQ  (0x1<<2)
#define MGMT_PKT_FILTER_REASSOC_RESP (0x1<<3)
#define MGMT_PKT_FILTER_PROBE_REQ    (0x1<<4)
#define MGMT_PKT_FILTER_PROBE_RESP   (0x1<<5)
#define MGMT_PKT_FILTER_BEACON       (0x1<<8)
#define MGMT_PKT_FILTER_ATIM         (0x1<<9)
#define MGMT_PKT_FILTER_REASSOC      (0x1<<10)
#define MGMT_PKT_FILTER_AUTH         (0x1<<11)
#define MGMT_PKT_FILTER_DEAUTH       (0x1<<12)

/** @brief This enumeration defines the extension command ID from Host to Wi-Fi firmware.
*/
     /**< 0x24 (Set)  */

typedef struct {
    uint8_t element_id;
    uint8_t length;
    uint8_t OUI[3];
}extra_vender_ie_t;


// Pengfei.Qiu  20160504  Only for internal use
// Need do committee and TC/TW review if put them into common SDK
/**
* @brief This function sets the bandwidth configuration that the WIFI driver uses for a specific wireless port.
* This operation takes effect immediately.
*
* @param[in] port indicates the WIFI port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  bandwidth is the wireless bandwidth.
*
* Value                                                |Definition                                                                 |
* -----------------------------------------------------|-------------------------------------------------|
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ          | 20MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ          | 40MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ        | 20 or 40MHz |
*
* @param[out]  below_above_ch is the primarychannel of bandwidth.
*
* Value                                     |Definition                                                                 |
* ------------------------------------------|-------------------------------------------------|
* \b 0                                      | Extended channel None  |
* \b 1                                      | Extended Channel Above |
* \b 3                                      | Extended channel Below |
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_get_bandwidth_extended(uint8_t port, uint8_t *bandwidth, uint8_t *below_above_ch);

/**
* @brief This function sets the bandwidth configuration that the WIFI driver uses for a specific wireless port.
* This operation takes effect immediately.
*
* @param[in] port indicates the WIFI port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in]  bandwidth is the wireless bandwidth.
*
* Value                                                |Definition                                                                 |
* -----------------------------------------------------|-------------------------------------------------|
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ          | 20MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ          | 40MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ        | 20 or 40MHz |
*
* @param[in]  below_above_ch is the primarychannel of bandwidth.
*
* Value                                     |Definition                                                                 |
* ------------------------------------------|-------------------------------------------------|
* \b 0                                      | Extended channel None  |
* \b 1                                      | Extended Channel Above |
* \b 3                                      | Extended channel Below |
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_set_bandwidth_extended(uint8_t port, uint8_t bandwidth, uint8_t below_above_ch);

int32_t wifi_inband_frame_type_filiter(uint8_t filter_switch, uint16_t frame_type, extra_vender_ie_t *vender_ie);

int32_t wifi_config_set_frame_filter(uint8_t filter_switch, uint16_t frame_type, extra_vender_ie_t *vender_ie);

int32_t wifi_config_get_rssi_threshold(uint8_t *enable, int8_t *rssi);

int32_t wifi_config_set_rssi_threshold(uint8_t enable, int8_t rssi);


#ifdef __cplusplus
}
#endif

#endif /* WIFI_API */

