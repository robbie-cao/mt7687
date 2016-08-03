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

#include <string.h>
#include "network_init.h"
#include "lwip_network.h"
#include "syslog.h"
#include "wifi_api.h"


#ifdef __ICCARM__
#define STRCPY strncpy
#else
#define STRCPY strlcpy
#endif


wifi_phy_mode_t wifi_change_wireless_mode_5g_to_2g(wifi_phy_mode_t wirelessmode)
{
    if (WIFI_PHY_11A == wirelessmode) {
        return WIFI_PHY_11B;
    } else if (WIFI_PHY_11ABG_MIXED == wirelessmode) {
        return WIFI_PHY_11BG_MIXED;
    } else if (WIFI_PHY_11ABGN_MIXED == wirelessmode) {
        return WIFI_PHY_11BGN_MIXED;
    } else if (WIFI_PHY_11AN_MIXED == wirelessmode) {
        return WIFI_PHY_11N_2_4G;
    } else if (WIFI_PHY_11AGN_MIXED == wirelessmode) {
        return WIFI_PHY_11GN_MIXED;
    } else if (WIFI_PHY_11N_5G == wirelessmode) {
        return WIFI_PHY_11N_2_4G;
    } else {
        return wirelessmode;
    }
}

uint8_t wifi_set_opmode(uint8_t target_mode)
{
    uint8_t origin_op_mode = 0;
    wifi_config_get_opmode(&origin_op_mode);
    if(target_mode == origin_op_mode) {
        LOG_I(wifi, "same opmode %d, do nothing", target_mode);
        return 1;
    }
    lwip_net_stop(origin_op_mode);

    wifi_config_set_opmode(target_mode);
    LOG_I(wifi, "set opmode to [%d]", target_mode);

    lwip_net_start(target_mode);
    return 0;
}

