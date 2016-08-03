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

/*****************************************************************************
 *
 *
 * Description:
 * ------------
 * This file implements gatt server profile common function for BDS tool
 *
 ****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "ble_bds_app_util.h"

/************************************************
*   Utilities
*************************************************/

/*
 * @brief Function for encoding a uint16 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
uint8_t bds_uint16_encode(uint8_t *p_encoded_data, uint16_t *value)
{
    p_encoded_data[0] = (uint8_t) ((*value & 0x00FF) >> 0);
    p_encoded_data[1] = (uint8_t) ((*value & 0xFF00) >> 8);
    return sizeof(uint16_t);
}


/**
 * @brief Function for encoding a utf8_str value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[out]  p_decoded_val    decoded value
 */
uint8_t bds_ble_srv_utf8_str_encode(uint8_t *p_encoded_data, ble_srv_utf8_str_t *p_value)
{
    memcpy(p_encoded_data, p_value->p_str, p_value->length);
    return p_value->length;
}


/**
 * @brief Function for converting the uint8 value to the uint16 value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
uint8_t bds_uint16_decode(uint16_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    *p_decoded_val = (((uint16_t)((uint8_t *)p_encoded_data)[0])) |
                     (((uint16_t)((uint8_t *)p_encoded_data)[1]) << 8 );
    return (sizeof(uint16_t));
}

/**
 * @brief Function for converting the uint8 value to the ble_srv_utf8_str_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
uint8_t bds_ble_srv_utf8_str_decode(ble_srv_utf8_str_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    p_decoded_val->p_str = (uint8_t *)p_encoded_data;
    p_decoded_val->length = len;
    return p_decoded_val->length;
}






