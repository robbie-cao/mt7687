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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BT_SPP_SERVER_H__
#define __BT_SPP_SERVER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "bt_spp.h"


/* Definition ----------------------------------------------------------------*/
#define SPP_SERVER_TX_BUFF_FOR_PROFILE  BT_SPP_MIN_TX_BUFSIZE  /*must be >= BT_SPP_MIN_TX_BUFSIZE*/
#define SPP_SERVER_RX_BUFF_FOR_PROFILE  BT_SPP_MIN_RX_BUFSIZE  /*must be >= BT_SPP_MIN_RX_BUFSIZE*/

#define SPP_SERVER_SEND_DATA_BUFF 256
#define SPP_SERVER_RECEIVED_DATA_BUFF 256

#define SPP_SERVER_STRING "Hello, SPP Client!"

/* Typedef ----------------------------------------------------------------*/
typedef struct
{
    bt_spp_port_id_t port_id;
    /*spp lib use it to buffer data to/from remote*/
    uint8_t spp_buffer_for_profile[SPP_SERVER_TX_BUFF_FOR_PROFILE + SPP_SERVER_RX_BUFF_FOR_PROFILE];
    uint32_t is_connected;
    /*the buffer for app data that will be sent*/
    uint8_t server_send_data_buffer[SPP_SERVER_SEND_DATA_BUFF];
    uint32_t server_remain_data_len;
    /*spp server use it to store received data from spp lib*/
    uint8_t server_receive_data_buf[SPP_SERVER_RECEIVED_DATA_BUFF];
    uint32_t server_received_data_len;
} bt_spp_server_cntx;

/* Functions ----------------------------------------------------------------*/
void spp_server_send_data(void);
void spp_server_enable(void);
void spp_server_disable(void);
void spp_server_send_data(void);
void spp_server_handle_ready_to_write_ind(bt_spp_ready_to_write_ind_t* parameter);
void spp_server_handle_ready_to_read_ind(bt_spp_ready_to_read_ind_t* parameter);
void spp_server_handle_connect_ind(bt_spp_connect_ind_t* parameter);
void spp_server_handle_connect_cnf(bt_spp_connect_cnf_t* parameter);
void spp_server_handle_disconnect_ind(bt_spp_disconnect_ind_t* parameter);
void spp_server_disconnect_request(void);
#ifdef __cplusplus
}
#endif

#endif /* __BT_SPP_SERVER_H__ */

