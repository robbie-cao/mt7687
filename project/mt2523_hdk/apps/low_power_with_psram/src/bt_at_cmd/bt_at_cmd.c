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

#include "ble_gap.h"
#include "bt_gap.h"
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "atci.h"
#include "syslog.h"


static void bt_common_convert_str_hex(const char *str, char *output, uint8_t len) 
{
    int32_t i = 0;
    char tempbuf[2];
    while(len) {
        memcpy(tempbuf, (str + (i*2)), 2);
        output[i] = strtol(tempbuf, NULL, 16);
        len = len -2 ;
        i++;
    }
}
static bool bt_atci_set_advertise(char *string)
{
    uint32_t adv_type;
    uint32_t min_interval;
    uint32_t max_interval;
    uint32_t own_addr_type;
    uint8_t buff[32]; 
    char string_buff[31*2];
    uint8_t len;
    if (sscanf(string, "%u%*c%u%*c%u%*c%u,%62s", (unsigned int *)&min_interval, (unsigned int *)&max_interval, (unsigned int *)&adv_type, (unsigned int *)&own_addr_type, (char *)string_buff) > 0) {
        LOG_I(common, "adv type %d, min_interval %d, max_interval  %d, onw_addr_type %d", adv_type, min_interval, max_interval, own_addr_type);
        ble_gap_set_advertising_params(
             min_interval,
             max_interval,
             adv_type,
             own_addr_type,
             BLE_ADDRESS_TYPE_PUBLIC,
             NULL,
             0x07,
             0
             );
        len = strlen(string_buff);
        LOG_I(common, "adv data %s, len %d", string_buff, len); 
        bt_common_convert_str_hex(string_buff, (char *)buff, len);
        ble_gap_set_advertising_data(
             buff,
             len/2,
             NULL,
             0
             ); 

        return true;
    } else {
        return false;
    }
}
static bool bt_atci_parse_cmd(char *string)
{
    uint16_t addr_5,addr_4,addr_3,addr_2,addr_1,addr_0;
    bool result = true;
    if (strstr(string, "AT+EBLESA=1") != 0) {
        /* start adv */
        ble_gap_start_advertiser();

    } else if (strstr(string, "AT+EBLESA=0") != 0) {

        ble_gap_stop_advertiser();

    } else if (strstr(string, "AT+EBLESEA=") != 0) {
         /*AT+EBLESAP 11 */
        result = bt_atci_set_advertise(string+11); 
    
    } else if (strstr(string, "AT+EBLESAD=") != 0) {
    /* avoid memory address's end is 1 */
        //uint32_t addr_5,addr_4,addr_3,addr_2,addr_1,addr_0;
        ble_address_t addr;
        if (sscanf(string+11, "%2x%*c%2x%*c%2x%*c%2x%*c%2x%*c%2x", (unsigned int *)&addr_5,
               (unsigned int *)&addr_4,(unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0) > 0) {
            addr.address[5] = addr_5;
            addr.address[4] = addr_4;
            addr.address[3] = addr_3;
            addr.address[2] = addr_2;
            addr.address[1] = addr_1;
            addr.address[0] = addr_0;
            LOG_I(common, "addr:%x-%x-%x-%x-%x-%x",addr.address[5],  addr.address[4], addr.address[3],addr.address[2],addr.address[1],addr.address[0]);

            ble_gap_set_random_address(&addr);

        } else {
            result = false;
        }
        

    } else if (strstr(string, "AT+EBTES=") != 0) {
        uint16_t mode = atoi(string + 9);
        LOG_I(common, "mode %d", mode);
        bt_gap_set_scan_mode(mode);
    } else if (strstr(string, "AT+EBTSI=") != 0) {
        bt_gap_sniff_info_t info = {0};
        if (sscanf(string+9, "%u,%u,%u,%u", (unsigned int *)&info.max_interval, (unsigned int *)&info.min_interval, (unsigned int *)&info.attempt, (unsigned int *)&info.timeout)) {
            LOG_I(common, "max_inter %d, min_inter %d, attempt %d, to %d", info.max_interval,info.min_interval, info.attempt, info.timeout);
            bt_gap_set_sniff_info(&info);
        } else {
            result = false;
        }

    } else if (strstr(string, "AT+EBLECON=") != 0) {
        ble_address_t addr;
        ble_gap_connection_params_t conn_param;
        uint16_t addr_type;
        if (sscanf(string+11, "%2x:%2x:%2x:%2x:%2x:%2x,%u,%u,%u,%u,%u", (unsigned int *)&addr_5, (unsigned int *)&addr_4,
                (unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0, (unsigned int *)&addr_type,
                (unsigned int *)&conn_param.min_conn_interval, 
                (unsigned int *)&conn_param.max_conn_interval,
                (unsigned int *)&conn_param.conn_latency,
                (unsigned int *)&conn_param.conn_timeout) > 0) {
            //addr_5 = 0xC3;
            addr.address[5] = addr_5;
            addr.address[4] = addr_4;
            addr.address[3] = addr_3;
            addr.address[2] = addr_2;
            addr.address[1] = addr_1;
            addr.address[0] = addr_0;
            //addr_type = (ble_address_type_t)_addr_type;
            LOG_I(common, "addr5%x",  addr_5);
            LOG_I(common, "addr:%x-%x-%x-%x-%x-%x,type:%d,min_interval:%d, max_interval:%d, latency: %d, conn_timeout: %d"
                        ,addr.address[5],  addr.address[4], addr.address[3],addr.address[2],addr.address[1],addr.address[0],
                        addr_type, conn_param.min_conn_interval,
                        conn_param.max_conn_interval,
                        conn_param.conn_latency,
                        conn_param.conn_timeout);
            ble_gap_connect(BLE_GAP_NOT_USE_WHITE_LIST,
                        addr_type,
                        &addr,
                        BLE_ADDRESS_TYPE_RANDOM,
                        &conn_param);
        } else {
            result = false;
        }

    } else if  (strstr(string, "AT+EBLEDIS=") != 0) {

        //uint32_t addr_5,addr_4,addr_3,addr_2,addr_1,addr_0;
        ble_address_t addr;
        if (sscanf(string+11, "%2x%*c%2x%*c%2x%*c%2x%*c%2x%*c%2x", (unsigned int *)&addr_5,
                (unsigned int *)&addr_4, (unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0) > 0) {
            addr.address[5] = addr_5;
            addr.address[4] = addr_4;
            addr.address[3] = addr_3;
            addr.address[2] = addr_2;
            addr.address[1] = addr_1;
            addr.address[0] = addr_0;
            ble_gap_disconnect(ble_gap_get_connection_id(&addr));

        } else {
            result = false;
        }
    }
    return result;
}

static atci_status_t bt_atci_reg_callback(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    LOG_I(common,"[BT_ATCI] mode:%d, CMD:%s", parse_cmd->mode, parse_cmd->string_ptr);
     
    if (parse_cmd->mode == ATCI_CMD_MODE_EXECUTION) {
        bool result;
        result = bt_atci_parse_cmd(parse_cmd->string_ptr);
        if (result) {
            strcpy((char *)output.response_buf, "Parse OK\n");
        } else {
            strcpy((char *)output.response_buf, "Parse failed\n");
        }
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK; 
        atci_send_response(&output); 
         
    } else {

        strcpy((char *)output.response_buf, "Not Support\n");
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR; 
        atci_send_response(&output); 
    }
    return ATCI_STATUS_OK; 

}

static atci_cmd_hdlr_item_t bt_at_cmd[] = {
{"AT+EBLESA", bt_atci_reg_callback, 0, 0},
{"AT+EBLESEA", bt_atci_reg_callback, 0, 0},
{"AT+EBLESAD", bt_atci_reg_callback, 0, 0},
{"AT+EBLECON", bt_atci_reg_callback, 0, 0},
{"AT+EBLEDIS", bt_atci_reg_callback, 0, 0},
{"AT+EBTES", bt_atci_reg_callback, 0, 0},
{"AT+EBTSI", bt_atci_reg_callback, 0, 0},
{"AT+EBTSS", bt_atci_reg_callback, 0, 0}
};

#define BT_ATCI_COMMAND_COUNT (sizeof(bt_at_cmd)/sizeof(atci_cmd_hdlr_item_t))


void bt_atci_init(void)
{
    atci_status_t ret = atci_register_handler(&bt_at_cmd[0], BT_ATCI_COMMAND_COUNT);
    if (ret == ATCI_STATUS_OK) {
        LOG_I(common, "bt_atci register success");
    } else {
        LOG_W(common, "bt_atci register fail");
    }
}

