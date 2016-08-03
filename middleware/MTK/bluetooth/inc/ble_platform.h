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

#ifndef __BLE_PLATFORM_H__
#define __BLE_PLATFORM_H__

#ifdef BT_PLATFORM_BLE_ONLY

/**
 * @addtogroup BLE
 * @{
 * This section includes all BLE APIs.
 *
 * MediaTek MT7697 introduces a platform support for Bluetooth Low Energy devices and provides APIs to use in the applications to discover devices, query
 * for services, and read/write characteristics. In contrast to Classic Bluetooth, Bluetooth Low Energy (BLE) is designed to
 * provide significantly lower power consumption. This enables MT7697 applications to communicate with BLE devices that have low power
 * requirements, such as proximity sensors, heart rate monitors, fitness devices, and so on.
 *
 */

/**
 * @}
 */

/**
 * @}
 */

#else /* BT_PLATFORM_BLE_ONLY */

/**
 * @addtogroup BLE
 * @{
 * This section includes all BLE APIs.
 *
 * MediaTek MT2523 introduces a platform support for Bluetooth Low Energy devices and provides APIs to use in the applications to discover devices, query
 * for services, and read/write characteristics. In contrast to @ref BT "Classic Bluetooth", Bluetooth Low Energy (BLE) is designed to
 * provide significantly lower power consumption. This enables MT2523 applications to communicate with BLE devices that have low power
 * requirements, such as proximity sensors, heart rate monitors, fitness devices, and so on.
 *
 */

/**
 * @}
 */

/**
 * @}
 */

#endif /* BT_PLATFORM_BLE_ONLY */

#endif /* __BLE_PLATFORM_H__ */

