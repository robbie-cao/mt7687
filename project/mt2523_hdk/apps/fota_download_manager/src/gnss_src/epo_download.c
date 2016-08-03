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

/* add include file here */

#include "gnss_app.h"

#ifdef GNSS_SUPPORT_EPO_DOWNLOAD_BY_BT
#include "FreeRTOS.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "hal_flash.h"
#include "bt_notify.h"
#include "bt_events.h"
#include "bt_gap.h"
#include "gnss_log.h"

#include "epo_gprot.h"

#include "hal_rtc.h"

/*****************************************************************************
 * define
 *****************************************************************************/

#define MTKEPO_RECORD_SIZE 72



/*****************************************************************************
 * static function & variable
 *****************************************************************************/
static void    epo_cmd_cb(void *data);
static void    md5_cmd_cb(void *data);
static void    epo_recv_data_start(bt_noti_data_t *p_data);
static void    epo_recv_data_pack(bt_noti_data_t *p_data);
static int16_t epo_recv_data_end();
static void    epo_init_mem_info();


uint8_t epo_md5[32 + 1];
BaseType_t is_md5_received;
BaseType_t is_end_pack_come;

//log_create_module(GNSS_TAG, PRINT_LEVEL_INFO);

/*****************************************************************************
 * global variable & fucntion
 *****************************************************************************/

/* global variable */
bt_address_t    g_epo_bt_addr;
epo_mem_info_t    g_epo_mem_info;


/* function definition begin */
/*****************************************************************************
 * FUNCTION
 *  fota_reply_int_value
 * DESCRIPTION
 *  reply info to remote device
 * PARAMETERS
 *  sender        :[IN] sender id
 *  receiver      :[IN] receiver id
 *  reply_code  :[IN] error code
 * RETURNS
 *  void
 *****************************************************************************/
void epo_reply_int_value(char *sender, char *receiver, int32_t reply_code)
{
    char data[50] = {0};
    char error[5] = {0};
    GNSSLOGD( "[EPO] fota_reply_int_value sender = %s, receiver = %s, reply_code = %d\n",
              sender, receiver, reply_code);
    if (reply_code >= 0) {
        sprintf(error, "%d", (int)reply_code);
        sprintf(data, "%s %s %d %d %s",
                sender,
                receiver,
                0,
                strlen(error),
                error);
    } else {
        reply_code = 0 - reply_code;
        sprintf(error, "%d", (int)reply_code);
        sprintf(data, "%s %s %d %d -%s",
                sender,
                receiver,
                0,
                strlen(error),
                error);
    }

    bt_notify_send_data(&g_epo_bt_addr, data, strlen(data), 1);
}

/*****************************************************************************
 * FUNCTION
 *  epo_request_download
 * DESCRIPTION
 *  send request to smartphone to download epo flie
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void epo_request_download()
{
    char data[80] = {0};
    char cmd[20] = {0};

    sprintf(cmd, "%s", "epo_download");

    sprintf(data, "%s %s %d %d %s",
            EPO_EXTCMD_UPDATE_DATA_SENDER,
            EPO_EXTCMD_UPDATE_DATA_RECEIVER,
            0,
            strlen(cmd),
            cmd);


    GNSSLOGD("[EPO_REQUEST]send request to download data = %s", data);

    bt_notify_send_data(&g_epo_bt_addr, data, strlen(data), 1);
}


/*****************************************************************************
 * FUNCTION
 *  epo_init_mem_info
 * DESCRIPTION
 *  init memory config of fota
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void epo_init_mem_info()
{
    GNSSLOGD( "[EPO] init mem info\n");
    g_epo_mem_info.start_address = 0x003F0000;
    g_epo_mem_info.end_address = 0x00400000;
    g_epo_mem_info.reserved_size = 992 * 1024;
    g_epo_mem_info.block_size = 4 * 1024;
    g_epo_mem_info.block_type = HAL_FLASH_BLOCK_4K;
    g_epo_mem_info.block_count = 16;//248;

    g_epo_mem_info.total_received = 0;
    g_epo_mem_info.write_ptr = 0;
    g_epo_mem_info.ubin_pack_count = 0;
}



/*****************************************************************************
 * FUNCTION
 *  fota_init
 * DESCRIPTION
 *  init fota enviroment
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void epo_download_init()
{
    bt_notify_result_t reg_ret_1;
    bt_notify_result_t reg_ret_2;
    epo_init_mem_info();
    GNSSLOGD("[EPO_INIT] begin register extend command callback hdlr\n");
    reg_ret_1 = bt_notify_register_callback(NULL, EPO_EXTCMD_UPDATE_DATA_SENDER, epo_cmd_cb);
    reg_ret_2 = bt_notify_register_callback(NULL, EPO_EXTCMD_UPDATE_MD5_SENDER, md5_cmd_cb);
    GNSSLOGD("[EPO_INIT] register resul_1: %d, result_2 = %d\n", reg_ret_1, reg_ret_2);
}

/*****************************************************************************
 * FUNCTION
 *  md5_cmd_cb
 * DESCRIPTION
 *  for receive md5 data callback
 * PARAMETERS
 *  data      :[IN] command data
 * RETURNS
 *  void
 *****************************************************************************/
static void md5_cmd_cb(void *data)
{
    bt_noti_callback_t *bt_cb_data = (bt_noti_callback_t *)data;
    GNSSLOGD("\n[MD5_CB]callback triggered\n");

    if (bt_cb_data->evt_id == BT_NOTIFY_EVENT_DATA) {
        if (!strcmp(bt_cb_data->noti_data.sender_id, EPO_EXTCMD_UPDATE_MD5_SENDER) &&
                !strcmp(bt_cb_data->noti_data.receiver_id, EPO_EXTCMD_UPDATE_MD5_SENDER)) {
            GNSSLOGD("\n[MD5_CB]md5 data transfer: %s\n", bt_cb_data->noti_data.data);
            is_md5_received = pdTRUE;
            memcpy(epo_md5, bt_cb_data->noti_data.data, bt_cb_data->noti_data.len + 1);
            if (is_end_pack_come == pdTRUE) {
                if (g_epo_mem_info.ubin_pack_count == 0) {
                    epo_update_notify(EPO_UPDATE_NOTIFY_TYPE_SUCCESS, epo_md5);
                } else {
                    epo_update_notify(EPO_UPDATE_NOTIFY_TYPE_FAIL, epo_md5);
                }
            } else {
                GNSSLOGD("\n[MD5_CB]end pack not come yet\n");
            }
        } else {
            GNSSLOGD("\n[MD5_CB]error_sender: %s or error_receiver: %s\n", bt_cb_data->noti_data.sender_id, bt_cb_data->noti_data.receiver_id);
        }
    }

    if (bt_cb_data->evt_id == BT_NOTIFY_EVENT_READY_TO_READ) {
        if (!strcmp(bt_cb_data->noti_data.sender_id, EPO_EXTCMD_UPDATE_MD5_SENDER) &&
                !strcmp(bt_cb_data->noti_data.receiver_id, EPO_EXTCMD_UPDATE_MD5_SENDER)) {
            GNSSLOGD( "\n[EPO_CB]evt: BT_NOTIFY_EVENT_READY_TO_READ\n");
            bt_notify_read_data(&g_epo_bt_addr);
        } else {
            GNSSLOGD("\n[MD5_CB]error_sender: %s or error_receiver: %s\n", bt_cb_data->noti_data.sender_id, bt_cb_data->noti_data.receiver_id);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  epo_cmd_hdlr
 * DESCRIPTION
 *  receive command from bt notification and send message to gnss queue
 * PARAMETERS
 *  data      :[IN] command data
 * RETURNS
 *  void
 *****************************************************************************/
static void epo_cmd_cb(void *data)
{
    //send item to fota queue and copy data to ring buffer
    bt_noti_callback_t *bt_cb_data = (bt_noti_callback_t *)data;
    GNSSLOGD("\n[EPO_CB]callback triggered, evt_id = %d\n", bt_cb_data->evt_id);
    GNSSLOGD("\n[EPO_TRACE_ERROR_0]\n");


    //if connection event ,store bt address here
    switch (bt_cb_data->evt_id) {
        case BT_NOTIFY_EVENT_CONNECTION:
            GNSSLOGD( "\n[EPO_CB]evt: BT_NOTIFY_EVENT_CONNECTION\n");
            memcpy(g_epo_bt_addr.address, bt_cb_data->bt_addr.address, 6);
            GNSSLOGD( "[EPO_CB] remote bt address: %s\n", g_epo_bt_addr.address);
            //if (epo_need_update() == true)
            //{
            epo_request_download();
            //}
            break;

        case BT_NOTIFY_EVENT_DISCONNECTION:
            GNSSLOGD( "\n[EPO_CB]evt: BT_NOTIFY_EVENT_DISCONNECTION\n");
            memset(g_epo_bt_addr.address, 0, 6);
            epo_init_mem_info();
            break;

        case BT_NOTIFY_EVENT_READY_TO_READ:
            GNSSLOGD( "\n[EPO_CB]evt: BT_NOTIFY_EVENT_READY_TO_READ\n");
            bt_notify_read_data(&g_epo_bt_addr);
            break;

        case BT_NOTIFY_EVENT_READY_TO_WRITE:
            GNSSLOGD( "\n[EPO_CB]evt: BT_NOTIFY_EVENT_READY_TO_WRITE\n");
            break;

        case BT_NOTIFY_EVENT_DATA:
            GNSSLOGD( "\n[EPO_CB]evt: BT_NOTIFY_EVENT_DATA, error_code = %d\n", bt_cb_data->noti_data.err_code);
            GNSSLOGD("\n[EPO_TRACE_ERROR_1]\n");
            if (!strcmp(bt_cb_data->noti_data.sender_id, EPO_EXTCMD_UPDATE_DATA_SENDER) &&
                    !strcmp(bt_cb_data->noti_data.receiver_id, EPO_EXTCMD_UPDATE_DATA_SENDER)) {
                if (bt_cb_data->noti_data.err_code == EPO_FILE_DATA_BEGIN) {
                    GNSSLOGD( "\n[EPO_CB]pack transfer: 0\n");
                    GNSSLOGD("\n[EPO_TRACE_ERROR_2]\n");
                    epo_recv_data_start(&(bt_cb_data->noti_data));
                } else if (bt_cb_data->noti_data.err_code == EPO_FILE_DATA_PACK) {
                    GNSSLOGD( "\n[EPO_CB]pack transfer: 1\n");
                    GNSSLOGD("\n[EPO_TRACE_ERROR_3]\n");
                    epo_recv_data_pack(&(bt_cb_data->noti_data));
                } else if (bt_cb_data->noti_data.err_code == EPO_FILE_DATA_END) {
                    GNSSLOGD( "\n[EPO_CB]pack transfer: 2\n");
                    GNSSLOGD("\n[EPO_TRACE_ERROR_4]\n");
                    is_end_pack_come = pdTRUE;
                    if (epo_recv_data_end() == EPO_PACK_END_WRONG) {
                        epo_init_mem_info();
                        epo_update_notify(EPO_UPDATE_NOTIFY_TYPE_FAIL, epo_md5);
                    } else {
                        if (is_md5_received == pdTRUE) {
                            GNSSLOGD("\n[EPO_MSG_HDLR] md5 received\n");
                            GNSSLOGD("\n[EPO_TRACE_ERROR_8]\n");
                            epo_update_notify(EPO_UPDATE_NOTIFY_TYPE_SUCCESS, epo_md5);
                        }
                    }
                } else {
                    GNSSLOGD( "\n[EPO_CB]pack transfer, error_code\n", bt_cb_data->noti_data.err_code);
                }
            } else {
                GNSSLOGD("\n[EPO_CB]error_sender: %s or error_receiver: %s\n", bt_cb_data->noti_data.sender_id, bt_cb_data->noti_data.receiver_id);
            }
            break;

        default:
            break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  epo_recv_data_start
 * DESCRIPTION
 *  begin to receive ubin data
 * PARAMETERS
 *  noti_data      :[IN] begin data pack
 * RETURNS
 *  void
 *****************************************************************************/
static void epo_recv_data_start(bt_noti_data_t *noti_data)
{
    GNSSLOGD("\n[EPO_PACK]start\n");
    g_epo_mem_info.write_ptr = g_epo_mem_info.start_address;
    g_epo_mem_info.ubin_pack_count = atoi((const char*)noti_data->data);
    is_md5_received = pdFALSE;
    is_end_pack_come = pdFALSE;
    GNSSLOGD("[EPO]g_fota_mem_info.ubin_pack_count = %d\n", g_epo_mem_info.ubin_pack_count);
}

/*****************************************************************************
 * FUNCTION
 *  fota_recv_ubin_data_pack
 * DESCRIPTION
 *  handle data pack received from remote device
 * PARAMETERS
 *  noti_data      :[IN] data pack from remote device
 * RETURNS
 *  void
 *****************************************************************************/
static void epo_recv_data_pack(bt_noti_data_t *noti_data)
{
    int32_t ret;

    if (g_epo_mem_info.write_ptr >= g_epo_mem_info.start_address
            && g_epo_mem_info.write_ptr <= g_epo_mem_info.end_address) {
        if (g_epo_mem_info.write_ptr + noti_data->len <= g_epo_mem_info.end_address) {
            if (!(g_epo_mem_info.write_ptr % (1 << 12))) {
                ret = hal_flash_erase(g_epo_mem_info.write_ptr, g_epo_mem_info.block_type);
                GNSSLOGD("[EPO] erase flash = %d\n", ret);
            }

            ret = hal_flash_write(g_epo_mem_info.write_ptr, noti_data->data, noti_data->len);
            if (HAL_FLASH_STATUS_OK == ret) {
                g_epo_mem_info.ubin_pack_count--;
                g_epo_mem_info.write_ptr = g_epo_mem_info.write_ptr + noti_data->len;
                g_epo_mem_info.total_received = g_epo_mem_info.total_received + noti_data->len;
                GNSSLOGD("[EPO] current received = %d\n", noti_data->len);
                GNSSLOGD("[EPO] total received = %d\n", g_epo_mem_info.total_received);
                GNSSLOGD("[EPO] pack left = %d\n", g_epo_mem_info.ubin_pack_count);
                GNSSLOGD("[EPO] write_ptr = 0x%x\n", g_epo_mem_info.write_ptr);
            } else {
                epo_reply_int_value(EPO_EXTCMD_UPDATE_DATA_SENDER, EPO_EXTCMD_UPDATE_DATA_RECEIVER, EPO_WRITE_FLASH_FAIL);
                epo_init_mem_info();
            }
        } else {
            GNSSLOGD("[EPO] write_ptr = 0x%x\n", g_epo_mem_info.write_ptr);
            GNSSLOGD("[EPO] current_received = %d\n", noti_data->len);
            GNSSLOGD("[EPO] end_address = 0x%x\n", g_epo_mem_info.end_address);
            epo_reply_int_value(EPO_EXTCMD_UPDATE_DATA_SENDER, EPO_EXTCMD_UPDATE_DATA_RECEIVER, EPO_UBIN_OVERSIZE);
            epo_init_mem_info();
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  fota_recv_ubin_data_end
 * DESCRIPTION
 *  date pack end handle
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static int16_t epo_recv_data_end()
{
    GNSSLOGD("\n[EPO_PACK]end\n");
    if (g_epo_mem_info.write_ptr >= g_epo_mem_info.start_address
            && g_epo_mem_info.write_ptr <= g_epo_mem_info.end_address) {
        g_epo_mem_info.write_ptr = 0;
        if (g_epo_mem_info.ubin_pack_count == 0) {
            epo_reply_int_value(EPO_EXTCMD_UPDATE_DATA_SENDER, EPO_EXTCMD_UPDATE_DATA_RECEIVER, EPO_FILE_DATA_END);
            return EPO_PACK_END_CORRECT;
        } else {
            epo_reply_int_value(EPO_EXTCMD_UPDATE_DATA_SENDER, EPO_EXTCMD_UPDATE_DATA_RECEIVER, EPO_TRANSFER_DATA_FAIL);
            return EPO_PACK_END_WRONG;
        }
    } else {
        return EPO_PACK_END_WRONG;
    }
}

bool epo_need_update()
{
#if 0
    int segment, epo_gnss_hour, current_gnss_hour;
    hal_rtc_time_t rtc_time_org = {0};
    int32_t ret;

    ret = hal_flash_read(g_epo_mem_info.start_address, (uint8_t *)&epo_gnss_hour, sizeof(epo_gnss_hour));
    GNSSLOGD("epo_need_update:%d,%d\n", epo_gnss_hour, ret);
    epo_gnss_hour &= 0x00FFFFFF;
    hal_rtc_get_time(&rtc_time_org);
    current_gnss_hour = utc_to_gnss_hour(rtc_time_org.rtc_year + 2000, rtc_time_org.rtc_mon, rtc_time_org.rtc_day, rtc_time_org.rtc_hour);
    segment = (current_gnss_hour - epo_gnss_hour) / 6;
    GNSSLOGD("epo_demo_get_segment:%d,%d,%d\n", segment, epo_gnss_hour, current_gnss_hour);
    if ((segment < 0) || (segment >= (3 * 4))) { // over 3 days need update
        return true;
    }
#endif
    return false;
}

void epo_update_notify(epo_update_notify_type_t type, void *param)
{
    if (type == EPO_UPDATE_NOTIFY_TYPE_SUCCESS) {
        // checksum
        GNSSLOGD( "[EPO Demo] EPO download success!\n");
    } else {
        GNSSLOGD( "[EPO Demo] EPO download fail!\n");
    }
}
#endif /*GNSS_SUPPORT_EPO_DOWNLOAD_BY_BT*/

