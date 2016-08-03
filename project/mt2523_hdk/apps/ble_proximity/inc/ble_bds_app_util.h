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
typedef struct
{
    uint8_t *p_list;    /**< Pointer the byte array containing the encoded opaque structure based on IEEE 11073-20601 specification. */
    uint8_t list_len;   /**< Length of the byte array. */
} regcertdatalist_t;

/**
 * @brief SFLOAT format (IEEE-11073 16-bit FLOAT, meaning 4 bits for exponent (base 10) and 12 bits mantissa) 
 */
typedef struct
{
    int8_t  exponent;   /**< Base 10 exponent, should be using only 4 bits */
    int16_t mantissa;   /**< Mantissa, should be using only 12 bits */
} sfloat_t;

/**
 * @brief Date and Time structure. 
 */
typedef struct
{
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
typedef struct
{
    uint8_t length;    /*string length*/
    uint8_t *p_str;    /*string data*/
} ble_srv_utf8_str_t;

/**
 * @brief Byte array type structure. 
 */
typedef struct
{
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
 #if 0
static uint8_t bds_uint16_encode(uint8_t *p_encoded_data, uint16_t *value)
{
    p_encoded_data[0] = (uint8_t) ((*value & 0x00FF) >> 0);
    p_encoded_data[1] = (uint8_t) ((*value & 0xFF00) >> 8);
    return sizeof(uint16_t);
}
#endif
/**
 * @brief Function for encoding a int16 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
 #if 0
static uint8_t bds_int16_encode(uint8_t *p_encoded_data, int16_t *value)
{
	uint16_t tmp = *value;
	return bds_uint16_encode(p_encoded_data, &tmp);
}
#endif
/**
 * @brief Function for encoding a sint16 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
 #if 0
static uint8_t bds_sint16_encode(uint8_t *p_encoded_data, sint16_t *value)
{
	return bds_int16_encode(p_encoded_data, value);
}
#endif
/**
 * @brief Function for encoding a sint8 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
 #if 0
static uint8_t bds_sint8_encode(uint8_t *p_encoded_data, sint8_t *value)
{
	uint8_t tmp = *value;
	*p_encoded_data = tmp;
	return sizeof(uint8_t);
}
#endif
/**
 * @brief Function for encoding a int8 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
 #if 0
static uint8_t bds_int8_encode(uint8_t *p_encoded_data, int8_t *value)
{
	uint8_t tmp = *value;
	*p_encoded_data = tmp;
	return sizeof(uint8_t);
}
#endif
/**
 * @brief Function for encoding a undefined_type_t value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
 #if 0
static uint8_t bds_undefined_type_encode(uint8_t *p_encoded_data, undefined_type_t *value)
{
	int8_t tmp =(int8_t)*value;
	return bds_int8_encode(p_encoded_data, &tmp);
}
#endif
/**
 * @brief Function for encoding a uint24 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
 #if 0
static uint8_t bds_uint24_encode(uint8_t *p_encoded_data, uint32_t *value)
{
    p_encoded_data[0] = (uint8_t) ((*value & 0x000000FF) >> 0);
    p_encoded_data[1] = (uint8_t) ((*value & 0x0000FF00) >> 8);
    p_encoded_data[2] = (uint8_t) ((*value & 0x00FF0000) >> 16);
    return (3);
}
#endif   
/**
 * @brief Function for encoding a uint32 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
 #if 0
static uint8_t bds_uint32_encode(uint8_t *p_encoded_data, uint32_t *value)
{
    p_encoded_data[0] = (uint8_t) ((*value & 0x000000FF) >> 0);
    p_encoded_data[1] = (uint8_t) ((*value & 0x0000FF00) >> 8);
    p_encoded_data[2] = (uint8_t) ((*value & 0x00FF0000) >> 16);
    p_encoded_data[3] = (uint8_t) ((*value & 0xFF000000) >> 24);
    return sizeof(uint32_t);
}
#endif    
/**
 * @brief Function for encoding a uint40 value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes  written.
 */
 #if 0
static uint8_t bds_uint40_encode(uint8_t *p_encoded_data, uint64_t *value)
{
    p_encoded_data[0] = (uint8_t) ((*value & 0x00000000000000FF) >> 0);
    p_encoded_data[1] = (uint8_t) ((*value & 0x000000000000FF00) >> 8);
    p_encoded_data[2] = (uint8_t) ((*value & 0x0000000000FF0000) >> 16);
    p_encoded_data[3] = (uint8_t) ((*value & 0x00000000FF000000) >> 24);
    p_encoded_data[4] = (uint8_t) ((*value & 0x000000FF00000000) >> 32);
    return 5;
}
#endif
/**
 * @brief Function for encoding a sfloat value.
 *
 * @param[in]   p_value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes written.
 */
 #if 0
static uint8_t bds_sfloat_encode(uint8_t *p_encoded_data, sfloat_t *p_value)
{
    uint16_t encoded_val = 0;

    encoded_val = ((p_value->exponent << 12) & 0xF000) | ((p_value->mantissa <<  0) & 0x0FFF);
	
    return(bds_uint16_encode(p_encoded_data, &encoded_val));
}
#endif
/**
 * @brief Function for encoding a uint8_array value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[out]  p_decoded_val    decoded value
 */
 #if 0
static uint8_t bds_uint8_array_encode(uint8_t *p_encoded_data, uint8_array_t *p_value)
{
    memcpy(p_encoded_data, p_value->p_data, p_value->size);
    return p_value->size;
}    
#endif
/**
 * @brief Function for encoding a utf8_str value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[out]  p_decoded_val    decoded value
 */
 #if 0
static uint8_t bds_ble_srv_utf8_str_encode(uint8_t *p_encoded_data, ble_srv_utf8_str_t *p_value)
{
    memcpy(p_encoded_data, p_value->p_str, p_value->length);
    return p_value->length;
}    
#endif
/**
 * @brief Function for encoding a regcertdatalist value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[out]  p_decoded_val    decoded value
 */
 #if 0
static uint8_t bds_regcertdatalist_encode(uint8_t *p_encoded_data, regcertdatalist_t *p_value)
{
    memcpy(p_encoded_data, p_value->p_list, p_value->list_len);
    return p_value->list_len;
}    
#endif
/**
 * @brief Function for encoding a Date and Time value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[out]  p_date_time      decoded value
 */
 #if 0
static uint8_t bds_ble_date_time_encode(uint8_t *p_encoded_data, ble_date_time_t *p_date_time)
{
    uint8_t len = bds_uint16_encode(&p_encoded_data[0], &p_date_time->year);
    
    p_encoded_data[len++] = p_date_time->month;
    p_encoded_data[len++] = p_date_time->day;
    p_encoded_data[len++] = p_date_time->hours;
    p_encoded_data[len++] = p_date_time->minutes;
    p_encoded_data[len++] = p_date_time->seconds;
    
    return len;
}
#endif
/**
 * @brief Function for converting the uint8 value to the uint16 value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_uint16_decode(uint16_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    *p_decoded_val = (((uint16_t)((uint8_t *)p_encoded_data)[0])) | 
                     (((uint16_t)((uint8_t *)p_encoded_data)[1]) << 8 );
    return (sizeof(uint16_t));
}
#endif
/**
 * @brief Function for converting the uint8 value to the int16 value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_int16_decode(int16_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    uint16_t tmp = *p_decoded_val;
    return bds_uint16_decode(&tmp, p_encoded_data, len);
}
#endif

/**
 * @brief Function for converting the uint8 value to the sint16 value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_sint16_decode(sint16_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    return bds_int16_decode(p_decoded_val, p_encoded_data, len);
}
#endif
/**
 * @brief Function for converting the uint8 value to the sint8 value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_sint8_decode(sint8_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    uint8_t tmp = *p_decoded_val;
	tmp = (((uint8_t)((uint8_t *)p_encoded_data)[0]));
	*p_decoded_val = tmp;
    return len;
}
#endif
/**
 * @brief Function for converting the uint8 value to the int8 value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_int8_decode(int8_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    uint8_t tmp = *p_decoded_val;
	tmp = (((uint8_t)((uint8_t *)p_encoded_data)[0]));
	*p_decoded_val = tmp;
    return len;
}
#endif
/**
 * @brief Function for converting the uint8 value to the undefined_type_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_undefined_type_decode(undefined_type_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
	int8_t tmp = (int8_t)*p_decoded_val;
	return bds_int8_decode(&tmp, p_encoded_data, len);
}
#endif
/**
 * @brief Function for converting the uint8 value to the uint24_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_uint24_decode(uint32_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    *p_decoded_val = (((uint32_t)((uint8_t *)p_encoded_data)[0]) << 0)  |
                     (((uint32_t)((uint8_t *)p_encoded_data)[1]) << 8)  |
                     (((uint32_t)((uint8_t *)p_encoded_data)[2]) << 16);
    return (3);
}
#endif
/**
 * @brief Function for converting the uint8 value to the uint32_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_uint32_decode(uint32_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    *p_decoded_val = (((uint32_t)((uint8_t *)p_encoded_data)[0]) << 0)  |
                     (((uint32_t)((uint8_t *)p_encoded_data)[1]) << 8)  |
                     (((uint32_t)((uint8_t *)p_encoded_data)[2]) << 16) |
                     (((uint32_t)((uint8_t *)p_encoded_data)[3]) << 24 );
    return (sizeof(uint32_t));
}
#endif
/**
 * @brief Function for converting the uint8 value to the uint40_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_uint40_decode(uint64_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    *p_decoded_val = (((uint64_t)((uint8_t *)p_encoded_data)[0]) << 0)  |
                     (((uint64_t)((uint8_t *)p_encoded_data)[1]) << 8)  |
                     (((uint64_t)((uint8_t *)p_encoded_data)[2]) << 16) |
                     (((uint64_t)((uint8_t *)p_encoded_data)[3]) << 24 )|
                     (((uint64_t)((uint8_t *)p_encoded_data)[4]) << 32 );
    return (40);
}
#endif
/**
 * @brief Function for converting the uint8 value to the sfloat_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_sfloat_decode(sfloat_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    
    p_decoded_val->exponent = 0;
    bds_uint16_decode((uint16_t*)&p_decoded_val->mantissa, p_encoded_data, len);
    p_decoded_val->exponent = (uint8_t)((p_decoded_val->mantissa & 0xF000) >> 12);
    p_decoded_val->mantissa &= 0x0FFF;
    return len;
}
#endif
/**
 * @brief Function for converting the uint8 value to the uint8_array_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_uint8_array_decode(uint8_array_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    memcpy(p_decoded_val->p_data, p_encoded_data, len);
    p_decoded_val->size = len;
    return p_decoded_val->size;
}   
#endif
/**
 * @brief Function for converting the uint8 value to the ble_srv_utf8_str_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_ble_srv_utf8_str_decode(ble_srv_utf8_str_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    p_decoded_val->p_str = (uint8_t*)p_encoded_data;
    p_decoded_val->length = len;
    return p_decoded_val->length;
}   
#endif
/**
 * @brief Function for converting the uint8 value to the regcertdatalist_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_regcertdatalist_decode(regcertdatalist_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    memcpy(p_decoded_val->p_list, p_encoded_data, len);
    p_decoded_val->list_len = len;
    return p_decoded_val->list_len;
}    
#endif
/**
 * @brief Function for converting the uint8 value to the ble_date_time_t value.
 *
 * @param[in]   p_encoded_data   Buffer where the encoded data is stored.
 * @param[in]   len              length of the encoded field.
 * @param[in]   p_decoded_val    Buffer where the decoded data is stored.
 *
 * @return      length of the decoded field.
 */
 #if 0
static uint8_t bds_ble_date_time_decode(ble_date_time_t *p_date_time, uint8_t *p_encoded_data, uint8_t len)
{
    uint8_t pos          = bds_uint16_decode(&p_date_time->year, &p_encoded_data[0], len);
    p_date_time->month   = p_encoded_data[pos++];
    p_date_time->day     = p_encoded_data[pos++];
    p_date_time->hours   = p_encoded_data[pos++];
    p_date_time->minutes = p_encoded_data[pos++];
    p_date_time->seconds = p_encoded_data[pos++];

    return pos;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BLE_APP_UTIL_BDS_H__*/



