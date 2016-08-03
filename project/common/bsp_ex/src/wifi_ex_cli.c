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

#include <stdlib.h>

#include "wifi_ex_config.h"
#include "wifi_ex_connect.h"
#include "wifi_ex_profile.h"

#include "wifi_ex.h"


static cmd_t   wifi_config_cli[] = {
    { "set",   "wifi config set",    NULL,   wifi_config_set_cli },
    { "get",   "wifi config get",    NULL,   wifi_config_get_cli },
    { NULL,    NULL,                 NULL,   NULL                }
};

static cmd_t   wifi_connect_cli[] = {
    { "set",   "wifi connect set",   NULL,   wifi_connect_set_cli },
    { "get",   "wifi connect get",   NULL,   wifi_connect_get_cli },
    { NULL,    NULL,                 NULL,   NULL                 }
};

#ifdef MTK_WIFI_PROFILE_ENABLE
static cmd_t   wifi_profile_cli[] = {
    { "set",   "wifi profile set",   NULL,   wifi_profile_set_cli },
    { "get",   "wifi profile get",   NULL,   wifi_profile_get_cli },
    { "reset", "wifi profile reset", wifi_profile_reset_ex,   NULL },
    { NULL,    NULL,                 NULL,   NULL                 }
};
#endif //MTK_WIFI_PROFILE_ENABLE

cmd_t   wifi_cli[] = {
    { "config",     "wifi config",  NULL,       wifi_config_cli     },
    { "connect",    "wifi connect", NULL,       wifi_connect_cli    },
#ifdef MTK_WIFI_PROFILE_ENABLE
    { "profile",    "wifi profile", NULL,       wifi_profile_cli    },
#endif //MTK_WIFI_PROFILE_ENABLE
    { "pm",         "PM state",     _cli_pm,    NULL                },
    { "wow",        "wow features", _cli_wow,   NULL                },
    { NULL,         NULL,           NULL,       NULL                }
};
