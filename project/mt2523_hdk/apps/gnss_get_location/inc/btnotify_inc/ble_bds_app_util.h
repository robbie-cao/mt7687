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

#ifndef __BLE_APP_UTIL_BDS_H__
#define __BLE_APP_UTIL_BDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/************************************************
*   structures
*************************************************/
typedef uint8_t  undefined_type_t;
typedef uint8_t  nibble_t;
typedef uint16_t uint12_t;
typedef uint32_t uint24_t;
typedef uint64_t uint40_t;
typedef uint64_t uint48_t;
typedef int8_t   sint8_t;
typedef int16_t  sint16_t;
typedef int32_t  sint32_t;
typedef int16_t  sint12_t;
typedef int32_t  sint24_t;
typedef int64_t  sint40_t;
typedef int64_t  sint48_t;
typedef int64_t  sint64_t;


/**
 * @brief IEEE 11073-20601 Regulatory Certification Data List Structure
 */
typedef struct {
    uint8_t *p_list;    /**< Pointer the byte array containing the encoded opaque structure based on IEEE 11073-20601 specification. */
    uint8_t list_len;   /**< Length of the byte array. */
} regcertdatalist_t;

/**
 * @brief SFLOAT format (IEEE-11073 16-bit FLOAT, meaning 4 bits for exponent (base 10) and 12 bits mantissa)
 */
typedef struct {
    int8_t  exponent;   /**< Base 10 exponent, should be using only 4 bits */
    int16_t mantissa;   /**< Mantissa, should be using only 12 bits */
} sfloat_t;

/**
 * @brief Date and Time structure.
 */
typedef struct {
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hours;
    uint8_t  minutes;
    uint8_t  seconds;
} ble_date_time_t;

/**
 * @brief UTF8 structure.
 */
typedef struct {
    uint8_t length;    /*string length*/
    uint8_t *p_str;    /*string data*/
} ble_srv_utf8_str_t;

/**
 * @brief Byte array type structure.
 */
typedef struct {
    uint8_t size;      /*number of array entries*/
    uint8_t *p_data;   /*pointer of array entries*/
} uint8_array_t;


/************************************************
*   Utilities
*************************************************/

/**
 * @brief Function for encoding a uint16 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
uint8_t bds_uint16_encode(uint8_t *p_encoded_data, uint16_t *value);

/**
 * @brief Function for encoding a utf8_str value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[out]  p_decoded_val    decoded value
 */
uint8_t bds_ble_srv_utf8_str_encode(uint8_t *p_encoded_data, ble_srv_utf8_str_t *p_value);

/**
 * @brief Function for converting the uint8 value to the uint16 value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
uint8_t bds_uint16_decode(uint16_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len);

/**
 * @brief Function for converting the uint8 value to the ble_srv_utf8_str_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
uint8_t bds_ble_srv_utf8_str_decode(ble_srv_utf8_str_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len);




#ifdef __cplusplus
}
#endif

#endif /*__BLE_APP_UTIL_BDS_H__*/





