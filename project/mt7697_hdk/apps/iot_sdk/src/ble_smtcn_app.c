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

#ifdef MTK_BLE_SMTCN_ENABLE
#include "bt_gap_le.h"
#include "string.h"
#include "bt_gatts.h"

void ble_smtcn_set_adv(void)
{
    bt_bd_addr_t addr = {0x0C, 0x01, 0x02, 0x03, 0x04, 0x05};
    bt_bd_addr_ptr_t random_addr = addr;

    bt_hci_cmd_le_set_advertising_parameters_t adv_param = {
        .advertising_interval_min = 0x00C0,//0x0020,
        .advertising_interval_max = 0x00C0,//0x0800,
        .advertising_type = BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED,
        .own_address_type = BT_ADDR_RANDOM,
        .advertising_channel_map = 7,
        .advertising_filter_policy = 0
    };

    bt_hci_cmd_le_set_advertising_enable_t enable;
    bt_hci_cmd_le_set_advertising_data_t adv_data = {
        .advertising_data_length = 12,
        .advertising_data = "DDDDDDDDDDTP",
    };
    
    extern char gatts_device_name[256];
    
    memset(gatts_device_name, 0x00, sizeof(gatts_device_name));
    memcpy(gatts_device_name, &adv_data.advertising_data[9], 3);
    
    adv_data.advertising_data[0] = 2; //adv_length
    adv_data.advertising_data[1] = BT_GAP_LE_AD_TYPE_FLAG;
    adv_data.advertising_data[2] = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;
    adv_data.advertising_data[3] = 3;
    adv_data.advertising_data[4] = BT_GAP_LE_AD_TYPE_16_BIT_UUID_COMPLETE;
    adv_data.advertising_data[5] = 0xAA;
    adv_data.advertising_data[6] = 0x18;
    adv_data.advertising_data[7] = 4; //adv_length
    adv_data.advertising_data[8] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
    
    enable.advertising_enable = BT_HCI_ENABLE;
    bt_gap_le_set_random_address(random_addr);

    bt_gap_le_set_advertising(&enable, &adv_param, &adv_data, NULL);
    
    bt_gatts_set_max_mtu(158);
}

#endif




