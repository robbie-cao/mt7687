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

#include "bt_sink_conmgr.h"
#include "nvdm.h"
#include "atci.h"
#include <stdint.h>
#include <stdio.h>
#include "bt_sink_utils.h"
#include "bt_sink_conmgr.h"
#include <string.h>

static atci_status_t bt_sink_cm_atci_handler(atci_parse_cmd_param_t *parse_cmd);

static atci_cmd_hdlr_item_t bt_sink_atci_cmd = {
    .command_head = "AT+BTADDR",    /**< AT command string. */
    .command_hdlr = bt_sink_cm_atci_handler,
    .hash_value1 = 0,
    .hash_value2 = 0,
};

static int16_t bt_sink_cm_convert_bt_addr(const char *index, bt_address_t *bt_addr)
{
    int16_t result = 0;
    uint32_t total_num = strlen(index), bt_count = 6, bt_bit = 1;
    const char *temp_index = index;
    bt_sink_memset(bt_addr, 0, sizeof(bt_address_t));
    while (total_num) {
        if (*temp_index <= '9' && *temp_index >= '0') {
            bt_addr->address[bt_count - 1] += (*temp_index - '0') * (bt_bit * 15 + 1);
            if (bt_bit == 0) {
                bt_count--;
                bt_bit = 1;
            } else {
                bt_bit--;
            }

        } else if (*temp_index <= 'F' && *temp_index >= 'A') {
            bt_addr->address[bt_count - 1] += (*temp_index - 'A' + 10) * (bt_bit * 15 + 1);
            if (bt_bit == 0) {
                bt_count--;
                bt_bit = 1;
            } else {
                bt_bit--;
            }
        } else if (*temp_index <= 'f' && *temp_index >= 'a') {
            bt_addr->address[bt_count - 1] += (*temp_index - 'a' + 10) * (bt_bit * 15 + 1);
            if (bt_bit == 0) {
                bt_count--;
                bt_bit = 1;
            } else {
                bt_bit--;
            }
        }
        if (!bt_count) {
            break;
        }
        total_num--;
        temp_index++;
    }

    if (bt_count) {
        bt_sink_memset(bt_addr, 0, sizeof(bt_address_t));
        result = -1;
    }

    bt_sink_report("local addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   bt_addr->address[5],
                   bt_addr->address[4],
                   bt_addr->address[3],
                   bt_addr->address[2],
                   bt_addr->address[1],
                   bt_addr->address[0]);
    return result;
}

int16_t bt_sink_cm_write_local_address(bt_address_t *local_addr)
{
    nvdm_status_t result =
        nvdm_write_data_item("BT_SINK",
                             "address",
                             NVDM_DATA_ITEM_TYPE_RAW_DATA,
                             (const uint8_t *)local_addr,
                             sizeof(bt_address_t));

    bt_sink_report("write address result:%d", result);
    return (int16_t)result;
}

bt_status_t bt_sink_cm_read_local_address(bt_address_t *local_addr)
{
    nvdm_status_t status;
    uint32_t size = sizeof(bt_address_t);
    bt_address_t empty = {{0}};

    if (NULL != local_addr) {
        status = nvdm_read_data_item("BT_SINK", "address", (uint8_t *)local_addr, &size);

        bt_sink_report("read address result:%d", status);

        if (NVDM_STATUS_OK != status || (0 == bt_sink_memcmp(local_addr, &empty, sizeof(bt_address_t)))) {
            local_addr->address[0] = 0x34;
            local_addr->address[1] = 0x21;
            local_addr->address[2] = 0x13;
            local_addr->address[3] = 0x58;
            local_addr->address[4] = 0x23;
            local_addr->address[5] = 0x11;
        }

        bt_sink_report("local addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                       local_addr->address[5],
                       local_addr->address[4],
                       local_addr->address[3],
                       local_addr->address[2],
                       local_addr->address[1],
                       local_addr->address[0]);
    }
    return BT_STATUS_SUCCESS;
}

atci_status_t bt_sink_cm_atci_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    bt_address_t local_address;
    int16_t result;

    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+BTADDR=<BT address> */
            result = bt_sink_cm_convert_bt_addr(parse_cmd->string_ptr + 10, &local_address);
            if (result == 0) {
                result = bt_sink_cm_write_local_address(&local_address);
                if (result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    snprintf((char *)response.response_buf,
                             ATCI_UART_TX_FIFO_BUFFER_SIZE,
                             "write error:%d\r\n",
                             result);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else {
                strcpy((char *)response.response_buf, "invalid address\r\n");
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

void bt_sink_cm_atci_init(void)
{
    atci_status_t ret;

    ret = atci_register_handler(&bt_sink_atci_cmd, 1);

    if (ret != ATCI_STATUS_OK) {
        bt_sink_report("at_cmd_init register fail\r\n");
    }
}

