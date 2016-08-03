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
 * Description:
 * ------------
 * This file implements gatt server profile app common function
 *
 ****************************************************************************/

#ifndef __BLE_APP_UTIL_H__
#define __BLE_APP_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <string.h>
#include <stdbool.h>


/************************************************
*   macro
*************************************************/
#ifndef MIN
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif

/************************************************
*   Utilities
*************************************************/
/**
 * @brief Function for Retrieve a 16-bit number from the given buffer. The number is in Big-Endian format.
 *
 * @param[in]  ptr           the point of buffer to be Retrieved.
 *
 * @param[out] a 16-bit number is in Big-Endian format.
 */
uint16_t be_to_retrieve_host16(const uint8_t *ptr)
{
    return (uint16_t)(((uint16_t) *ptr << 8) | ((uint16_t) *(ptr + 1)));
}

/**
 * @brief Function for Retrieve a 32-bit number from the given buffer. The number is in Big-Endian format.
 *
 * @param[in]  ptr           the point of buffer to be Retrieved.
 *
 * @param[out] a 32-bit number is in Big-Endian format.
 */
uint32_t be_to_retrieve_host32(const uint8_t *ptr)
{
    return (uint32_t)(((uint32_t) *ptr << 24) | ((uint32_t) *(ptr + 1) << 16) | ((uint32_t) *(ptr + 2) << 8) | ((uint32_t) *(ptr + 3)));
}

/**
 * @brief Function for Retrieve a 64-bit number from the given buffer. The number is in Big-Endian format.
 *
 * @param[in]  ptr           the point of buffer to be Retrieved.
 *
 * @param[out] a 64-bit number is in Big-Endian format.
 */
uint64_t be_to_retrieve_host64(const uint8_t *ptr)
{
    return (uint64_t)(((uint64_t) *ptr << 56) |
                 ((uint64_t) *(ptr + 1) << 48) |
                 ((uint64_t) *(ptr + 2) << 40) |
                 ((uint64_t) *(ptr + 3) << 32) |
                 ((uint64_t) *(ptr + 4) << 24) |
                 ((uint64_t) *(ptr + 5) << 16) |
                 ((uint64_t) *(ptr + 6) << 8) |
                 ((uint64_t) *(ptr + 7)));
}

/**
 * @brief Function for Retrieve a 16-bit number from the given buffer. The number is in Little-Endian format.
 *
 * @param[in]  ptr           the point of buffer to be Retrieved.
 *
 * @param[out] a 16-bit number is in Little-Endian format.
 */
uint16_t le_to_retrieve_host16(const uint8_t *ptr)
{
    return (uint16_t)(((uint16_t) *(ptr + 1) << 8) | ((uint16_t) *ptr));
}

/**
 * @brief Function for Retrieve a 32-bit number from the given buffer. The number is in Little-Endian format.
 *
 * @param[in]  ptr           the point of buffer to be Retrieved.
 *
 * @param[out] a 32-bit number is in Little-Endian format.
 */
uint32_t le_to_retrieve_host32(const uint8_t *ptr)
{
    return (uint32_t)(((uint32_t) *(ptr + 3) << 24) | ((uint32_t) *(ptr + 2) << 16) | ((uint32_t) *(ptr + 1) << 8) | ((uint32_t) * (ptr)));
}

/**
 * @brief Function for Retrieve a 64-bit number from the given buffer. The number is in Little-Endian format.
 *
 * @param[in]  ptr           the point of buffer to be Retrieved.
 *
 * @param[out] a 64-bit number is in Little-Endian format.
 */
uint64_t le_to_retrieve_host64(const uint8_t *ptr)
{
    return (uint64_t)(((uint64_t) *(ptr + 7) << 56) |
                 ((uint64_t) *(ptr + 6) << 48) |
                 ((uint64_t) *(ptr + 5) << 40) |
                 ((uint64_t) *(ptr + 4) << 32) |
                 ((uint64_t) *(ptr + 3) << 24) |
                 ((uint64_t) *(ptr + 2) << 16) |
                 ((uint64_t) *(ptr + 1) << 8) |
                 ((uint64_t) *(ptr)));
}

/**
 * @brief Function for Store 16 bit value into a buffer in Little Endian format.
 *
 * @param[in]  le_value      the 16 bit value.
 *
 * @param[out] buff          the buffer in Little Endian format.
 */
void le_to_store_host16(uint8_t *buff, uint16_t le_value)
{
    buff[1] = (uint8_t)(le_value >> 8);
    buff[0] = (uint8_t) le_value;
}

/**
 * @brief Function for Store 32 bit value into a buffer in Little Endian format.
 *
 * @param[in]  le_value      the 32 bit value.
 *
 * @param[out] buff          the buffer in Little Endian format.
 */
void le_to_store_host32(uint8_t *buff, uint32_t le_value)
{
    buff[3] = (uint8_t)(le_value >> 24);
    buff[2] = (uint8_t)(le_value >> 16);
    buff[1] = (uint8_t)(le_value >> 8);
    buff[0] = (uint8_t) le_value;
}

/**
 * @brief Function for Store 64 bit value into a buffer in Little Endian format.
 *
 * @param[in]  le_value	     the 64 bit value.
 *
 * @param[out] buff          the buffer in Little Endian format.
 */
void le_to_store_host64(uint8_t *buff, uint64_t le_value)
{
    buff[7] = (uint8_t)(le_value >> 56);
    buff[6] = (uint8_t)(le_value >> 48);
    buff[5] = (uint8_t)(le_value >> 40);
    buff[4] = (uint8_t)(le_value >> 32);
    buff[3] = (uint8_t)(le_value >> 24);
    buff[2] = (uint8_t)(le_value >> 16);
    buff[1] = (uint8_t)(le_value >> 8);
    buff[0] = (uint8_t) le_value;
}

/**
 * @brief Function for Store 16 bit value into a buffer in Big Endian format.
 *
 * @param[in]  be_value	     the 16 bit value.
 *
 * @param[out] buff          the buffer in Big Endian format.
 */
void be_to_store_host16(uint8_t *buff, uint16_t be_value)
{
    buff[0] = (uint8_t)(be_value >> 8);
    buff[1] = (uint8_t) be_value;
}

/**
 * @brief Function for Store 32 bit value into a buffer in Big Endian format.
 *
 * @param[in]  be_value	     the 32 bit value.
 *
 * @param[out] buff          the buffer in Big Endian format.
 */
void be_to_store_host32(uint8_t *buff, uint32_t be_value)
{
    buff[0] = (uint8_t)(be_value >> 24);
    buff[1] = (uint8_t)(be_value >> 16);
    buff[2] = (uint8_t)(be_value >> 8);
    buff[3] = (uint8_t) be_value;
}

/**
 * @brief Function for Store 64 bit value into a buffer in Big Endian format.
 *
 * @param[in]  be_value	     the 64 bit value.
 *
 * @param[out] buff          the buffer in Big Endian format.
 */
void be_to_store_host64(uint8_t *buff, uint64_t be_value)
{
    buff[0] = (uint8_t)(be_value >> 56);
    buff[1] = (uint8_t)(be_value >> 48);
    buff[2] = (uint8_t)(be_value >> 40);
    buff[3] = (uint8_t)(be_value >> 32);
    buff[4] = (uint8_t)(be_value >> 24);
    buff[5] = (uint8_t)(be_value >> 16);
    buff[6] = (uint8_t)(be_value >> 8);
    buff[7] = (uint8_t) be_value;
}


#ifdef __cplusplus
}
#endif


#endif /*__BLE_APP_UTIL_H__*/


