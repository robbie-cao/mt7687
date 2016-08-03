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

/**
 * @file network_init.h
 *
 */

#ifndef __WIFI_NETWORK_INIT_H__
#define __WIFI_NETWORK_INIT_H__


#include <stdint.h>
#include "lwip/netif.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief This defines the IP ready callback handler. Call #register_ip_ready_callback() to register a handler, and it will be called on the moment a valid IP address is got after wifi connection is done
*
* @param[in] netif  is the IP information got, including IP address, mask and gateway.
*
*/
typedef void (*wifi_ip_ready_callback_t)(const struct netif *netif);

/**
* @brief This function initializes the network related modules
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/
void network_init(void);

/**
* @brief      This function registers a callback and this callback will be called on the moment a valid IP address is got after wifi connection is done
* @param[in]  callback  is the target function to be called on right moment
* @warning    This callback function runs in lwIP task, therefore, calling lwIP socket APIs in this callback function will cause deadlock.
*             Please create a new task and invoke lwIP socket APIs in it if necessary.
*/
void wifi_register_ip_ready_callback(wifi_ip_ready_callback_t callback);

/**
* @brief This function initializes the network related modules and DHCP.
*/
void network_full_init(void);

/**
* @note This api is only for internal use
*/
uint8_t wifi_set_opmode(uint8_t target_mode);


#ifdef __cplusplus
}
#endif

#endif /* __WIFI_NETWORK_INIT_H__ */

