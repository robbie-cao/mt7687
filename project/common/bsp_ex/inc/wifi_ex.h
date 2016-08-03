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
 * @file        wifi_ex.h
 * @brief       WiFi API Example CLI
 *
 * @history     2015/08/18   Initial for 1st draft  (Michael Rong)
 */

#ifndef __WIFI_EX_H__
#define __WIFI_EX_H__

#include "cli.h"
#include "wifi_cli.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MTK_BSPEXT_ENABLE)
#define WIFI_CLI_ENTRY  { "wifi",   "wifi api", NULL,   wifi_cli },
#else
#define WIFI_CLI_ENTRY
#endif

#if defined(MTK_SMTCN_ENABLE)
#define WIFI_CLI_SMNT_ENTRY     { "smart",   "smart connection", _smart_config_test },
#else
#define WIFI_CLI_SMNT_ENTRY
#endif

#if defined(MTK_MINISUPP_ENABLE)
#define WIFI_SUPP_CLI_ENTRY     { "supp", "supplicant",   NULL, wifi_priv_cli },
#endif

#define WIFI_SCAN_CLI_ENTRY { "scan",   "scan features",    _cli_scan },


#if defined(MTK_AWS_ENABLE)
extern uint8_t _aws_test(uint8_t len, char *param[]);
#define AWS_CLI_ENTRY { "aws",   "aws test",    _aws_test },
#else
#define AWS_CLI_ENTRY
#endif

#if defined(MTK_AP_SNIFFER_ENABLE)
extern uint8_t _cli_ap_sniffer(uint8_t len, char *param[]);
#define SNIFFER_CLI_ENTRY     { "ap_sniffer", "sniffer mode",   _cli_ap_sniffer },
#else
#define SNIFFER_CLI_ENTRY
#endif
#define WIFI_CLI_WIFI_ENTRY { "f",  "wifi api", _cli_wifi },

#define WIFI_CONNSYS_CLI_ENTRY { "connsys",    "connsys related cmd",    _cli_connsys_related },

#define N9_LOG_CLI_ENTRY { "n9log",    "N9 consol log state", _cli_n9_consol_log},
extern uint8_t _cli_supp(uint8_t len, char *param[]);
extern uint8_t _cli_wifi(uint8_t len, char *param[]);

extern uint8_t _cli_scan(uint8_t len, char *param[]);

#if defined(MTK_SMTCN_ENABLE)
extern uint8_t _smart_config_test(uint8_t len, char *param[]);
#endif

extern uint8_t _cli_wow(uint8_t len, char *param[]);
extern uint8_t _cli_pm(uint8_t len, char *param[]);

extern uint8_t _cli_connsys_related(uint8_t len, char *param[]);
extern uint8_t _cli_inband_related(uint8_t len, char *param[]);
extern uint8_t _cli_n9_consol_log(uint8_t len, char *param[]);
extern cmd_t   wifi_cli[];
extern cmd_t   wifi_priv_cli[];

#if defined(MTK_BSPEXT_ENABLE) && defined(MTK_HAL_LOWPOWER_ENABLE)
extern cmd_t   lp_cli[];
#define LP_CLI_ENTRY { "lp",  "low power",  NULL, lp_cli },
#else
#define LP_CLI_ENTRY
#endif

#ifdef __cplusplus
}
#endif

#endif /*  __WIFI_EX_H__ */
