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

#ifndef __BT_PLATFORM_H__
#define __BT_PLATFORM_H__

/**
 * This header file describes the task apis or packed define used by user or SDK header file.
 */

#ifdef __cplusplus
#define BT_EXTERN_C_BEGIN extern "C" {
#else
#define BT_EXTERN_C_BEGIN
#endif

#ifdef __cplusplus
#define BT_EXTERN_C_END }
#else
#define BT_EXTERN_C_END
#endif

#include <stdint.h>
#include <string.h>
BT_EXTERN_C_BEGIN

/**
 * @brief BT_PACKED is used in structure define to make the structure more compact.The define is different as per compiler.
 */
#if _MSC_VER >= 1500
#define BT_PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#elif defined(__GNUC__)
#define BT_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#define BT_ALIGNMENT4(...) __VA_ARGS__ __attribute__((aligned(4)))
#elif defined(__ARMCC_VERSION)
#pragma anon_unions
#define BT_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#define BT_ALIGNMENT4(...) __VA_ARGS__ __attribute__((aligned(4)))
#elif defined(__ICCARM__)
#define BT_PACKED(...) __packed __VA_ARGS__
#define BT_ALIGNMENT4(...) _Pragma("data_alignment=4") __VA_ARGS__
#else
#error "Unsupported Platform"
#endif

/**
 * @brief     Bluetooth task exit function.
 * @return    None.
 */
void bt_exit_task(void);

/**
 * @brief     Bluetooth task entry function, and must be used when create bluetooth task.
 * @param[in] arg is NULL or the public address pointer, check bt_bd_addr_t for more details. 
              If it's address, will set public address when first power on.
 * @return    None.
 * @par       Example
 * @code
 *     bt_bd_addr_t local_public_addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
 *     xTaskCreate(bt_task, "bt_task", 1024, (void *)local_public_addr, 5, NULL);
 * @endcode
 */
void bt_task(void * arg);

BT_EXTERN_C_END

#endif
