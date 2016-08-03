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

#include "ble_gattc_srv.h"
#include "hr_client.h"
#include "ble_address.h"
#include <stdbool.h>
#include <stdlib.h>
#include "ble_gattc_utils_db.h"
#include "gattc_utils.h"
#include <string.h>

void gattc_decode_char_data(gattc_value_t *value, hr_data_t *data)
{
    uint8_t flag;
    uint8_t start_index;
    app_uuid_t uuid, uuid1, uuid2;

    flag = value->value[0];

    uuid.len = 2;
    uuid.uuid[0] = value->value[1];
    if (value->len < 2) {/*the min len is 2*/
        return;
    }
    if (flag << (8 - 1) & 1) { /*value is uint16*/

        uuid.uuid[1] = value->value[2];
        start_index = 3;
    } else {/*value is uint8*/
        uuid.uuid[1] = 0;
        start_index = 2;

    }
    data->val = gattc_convert_array_to_uuid16(&uuid);/*hr value*/
    uuid1.len = 2;
    if ((flag >> 3) & 1) {/*check energy_expend*/
        if (value->len >= 5) {
            uuid1.uuid[0] = value->value[start_index];
            uuid1.uuid[1] = value->value[start_index + 1];
            data->en_expend = gattc_convert_array_to_uuid16(&uuid1);
        } else {
            /*there has some error*/
            data->en_expend = 0;
        }
    }
    start_index = start_index + 1;
    uuid2.len = 2;
    if ((flag >> 4) & 1) {/*check RR_interval*/
        if (value->len >= 7) {
            uuid2.uuid[0] = value->value[start_index];
            uuid2.uuid[1] = value->value[start_index + 1];
            data->RR_inteval = gattc_convert_array_to_uuid16(&uuid2);
        } else {
            /*there has some error*/
            data->RR_inteval = 0;
        }
    }
}


void gattc_config_encode_data(descr_cfg_t *desc_value, gattc_value_t *value)
{
    uint16_t temp_value;
    temp_value = desc_value->desc_value;
    value->value[0] = (temp_value & 0x00ff);
    value->value[1] = (temp_value & 0xff00) >> 8;
    value->len = 2;
}

bool gattc_decode_scan_data(uint8_t *data, uint16_t len)
{
    bool is_filter = false;
    uint8_t uuid_len, flag, i;
    uint16_t srv_uuid, temp_len;
    temp_len = data[len];
    flag = 1;
    switch (data[len + 1]) {
        case 0x02:
        case 0x03:
            uuid_len = 2;
            break;
        case 0x04:
        case 0x05:
            uuid_len = 4;
            break;
        case 0x06:
        case 0x07:
            uuid_len = 16;
            break;
        default:
            flag = 0;
            break;
    }

    if (flag) {
        for (i = len + 2; i < len + 2 + temp_len - 1;) {
            app_uuid_t uuid;
            uuid.len = uuid_len;
            memcpy(uuid.uuid, data + i, uuid_len);
            srv_uuid = gattc_convert_array_to_uuid16(&uuid);
            if (SRV_HRM_SER_UUID == srv_uuid) {
                is_filter = true;

                break;
            }

            i += uuid_len;
        }
    }
    return is_filter;
}


uint16_t gattc_convert_srv_uuid_to_u16(gatt_uuid_t *uu)
{
    uint16_t uuid = 0;
    if (uu) {
        uuid = ((uint16_t)uu->uuid[13]) << 8 | uu->uuid[12];

    }

    return uuid;
}


uint16_t gattc_convert_array_to_uuid16(app_uuid_t *uu)
{
    uint16_t uuid = 0;
    if (uu) {
        if (uu->len == 2) {
            uuid = ((uint16_t)uu->uuid[1]) << 8 | uu->uuid[0];

        } else if (uu->len == 16) {
            uuid = ((uint16_t)uu->uuid[13]) << 8 | uu->uuid[12];
        }
    }
    return uuid;
}
