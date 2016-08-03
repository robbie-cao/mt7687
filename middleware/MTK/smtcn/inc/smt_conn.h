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
 * @file smt_conn.h
 *
 * MTK Smart Connection interfaces
 *
 * @author  Chang Yong
 * @version 2015/10/24 v0.9
 */
#ifndef __SMT_CONN_H__
#define __SMT_CONN_H__
#include <stdint.h>
#include "wifi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup Smart_Connection
 * @{
 * This document introduces MTK smart connection interfaces for upper applications.
 * It has wrapped the base interfaces of core.h, which supplies core functionality
 * of MTK elian protocol. MTK elian protocol is confidential and it doesn't
 * release its source code. The file smt_conn.c shows how to use these elian
 * APIs and WiFi APIs, such as switch channel, set rx filter, set raw packet
 * handler and so on.
 * You can replace MTK elian protocol and develop your own smart
 * config protocol easily.
 */


/**@defgroup SMTCN_DEFINE Define
 *@{
 */

/**@brief The value 2 specifies using MTK smart connection.
 *      Value 3 specifies using airkiss quick connect.
 */
//#define CFG_SUPPORT_SMNT_PROTO    2

#if (CFG_SUPPORT_SMNT_PROTO == 2)              /* elian protocol*/
/**@brief Specifies timeout value that can remain in each channel to listen
 * packets from smart phone.
 */
#define switch_channel_timems   (200)          /* elian 200ms */
/**@brief Specifies timeout value that can receive data packets in which
 * channel has sniffered sync packets.
 */
#define locked_channel_timems   (5 * 1000)     /* 5s */
#define use_smart_connection_filter  1

#elif (CFG_SUPPORT_SMNT_PROTO == 3)            /* airkiss protocol */
#define switch_channel_timems   (100)          /* airkiss 100ms */
#define locked_channel_timems   (10 * 1000)    /* 10s */
#define use_smart_connection_filter  1
#endif


/**@brief For FreeRTOS compatibility.
 */
#define tmr_nodelay         ( TickType_t ) 0

/**@brief After gained smart connection information, it will start a scan
 * process, this value specifies the maximum times to scan.
 */
#define max_scan_loop_count (5)

/**@brief Smart connection finish flag.
 */
#define SMTCN_FLAG_FIN  (0xAA)

/**@brief Smart connection fail flag.
 */
#define SMTCN_FLAG_FAIL  (0xAE)

/**@brief Smart connection stop flag.
 */
#define SMTCN_FLAG_EXIT  (0xAF)

/**@brief Scan finished flag, which means successfully scan the target AP
 * information.
 */
#define SCAN_FLAG_FIN (0xBB)

/**@brief Scan finish flag, which means not found the target AP information.
 */
#define SCAN_FLAG_NOT_FOUND (0xCC)

/**@brief for init purpose
 */
#define SCAN_FLAG_NONE   (0x00)

/**
 *@}
 */


/**@defgroup SMTCN_STRUCT Structure
*@{
*/

/** @brief smart connection result information.
*/
typedef struct {
    unsigned char                   pwd[WIFI_LENGTH_PASSPHRASE]; /**< store the password you got */
    unsigned char                   ssid[WIFI_MAX_LENGTH_OF_SSID]; /**< store the ssid you got */
    unsigned char                   pmk[WIFI_LENGTH_PMK]; /**< store the PMK if you have any */
    unsigned char                   *tlv_data;
    int                             tlv_data_len;
    wifi_auth_mode_t                auth_mode; /**< deprecated */
    unsigned char                   ssid_len; /**< ssid length */
    unsigned char                   pwd_len;  /**< password length */

    wifi_encrypt_type_t             encrypt_type; /**< Not used */
    unsigned char                   channel;      /**< Not used */
    unsigned char                   smtcn_flag;   /**< Flag to spicify whether smart connection finished. */
    unsigned char                   scan_flag;    /**< Flag to spicify whether scan finished. */
} smtcn_info;

/** @brief smart connection operations to adapt to specific protocol.
*/
typedef struct {
    int (*init)(const unsigned char *key, const unsigned char key_length);     /**< protocol specific initialize */
    void (*cleanup)(void); /**< protocol specific cleanup */
    int (*switch_channel_rst)(void);  /**< protocol specific reset operation when switch to next channel */
    int (*rx_handler)(char *, int);   /**< protocol specific packet handler */
} smtcn_proto_ops;

/**
 *@}
 */

/**
* @brief When have received the sync packets, you can use this function to stop
* switching channel. It means locked at the channel.
*
* @param[in]  None
*
* @return  None
*/
void smtcn_stop_switch(void);

/**
* @brief When locked channel timeout, you can use this function to continue
* to switch channel.
*
* @param[in]  None
*
* @return  None
*/
void smtcn_continue_switch(void);

/**
* @brief This function can be called to create a thread to process smart connection..
*
* @param[in]  None
*
* @return  0 means success, <0 means fail
*/
int32_t mtk_smart_connect(void);

/**
* @brief This function can be called to stop smart connection.
*
* @param[in]  None
*
* @return  0 means success, <0 means fail
*/
int32_t mtk_smart_exit(void);

int32_t mtk_smart_stop(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif
