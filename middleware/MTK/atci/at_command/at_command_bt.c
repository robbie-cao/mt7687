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

#ifdef MTK_BT_AT_COMMAND_ENABLE
// For Register AT command handler
#include "at_command.h"
#include "at_command_bt.h"
#include "syslog.h"
#include <FreeRTOS.h>

#include "queue.h"
#include "task.h"
//For bt head file
#include "bt_gap.h"
#include "timers.h"


//extern function
extern bool bt_enable_relay(void);
extern bool BTIF_Power_on(void);
extern bool BTIF_Power_off(void);
extern uint8_t BTIF_Hci_Send_Command(uint16_t opCode, uint8_t cmdLen, uint8_t *cmd);
extern uint8_t BTIF_Register_ATCI_Rx_Callback(void *rxCallback);
extern uint8_t bt_hci_event_parser(uint8_t *param, uint8_t paramLen, atci_bt_hci_event *hci_event);
extern int32_t btif_patch_download(void);

//static function
static void atci_bt_error_handle(void);
static void atci_bt_ata_cmd_paser(uint8_t *string);

static void atci_bt_power_on_handler(void);
static void atci_bt_power_on_cnf(void* param);

static void atci_bt_power_off_handler(void);
static void atci_bt_power_off_cnf(void* param);

static void atci_bt_search_handler(void);
static void atci_bt_gap_discovery_result_ind(void* param);
static void atci_bt_gap_discovery_cnf(void* param);
static void atci_bt_gap_read_remote_name_cnf(void* param);

static int8_t atci_bt_str2hex(uint8_t *string, uint32_t string_len, uint8_t data[]);
static int8_t atci_bt_hex2str(uint8_t data[], uint8_t data_len, uint8_t *string, uint32_t string_len);

static int8_t atci_bt_hci_string_to_cmd(uint8_t *string, uint32_t string_len, atci_bt_hci_cmd *hci_cmd);
static int8_t atci_bt_hci_event_to_string(atci_bt_hci_event *hci_event, uint8_t *string, uint32_t string_len);
static void atci_bt_task_init(void);
static void acti_bt_task_deinit(void);
static void atci_bt_cmd_task_main(void *pvParameters);
static void acti_bt_task_msg_handler(at_command_bt_message_struct_t *message);

log_create_module(atci_bt, PRINT_LEVEL_INFO);

/*---  Variant ---*/

atci_bt_context_struct_t atci_bt_cntx;
atci_bt_context_struct_t *atci_bt_cntx_p = &atci_bt_cntx;

#define ATCI_COMMAND_TASK_NAME "AT Ehanced"
#define ATCI_COMMAND_TASK_STACK_SIZE 1024 //should be fine tune
#define ATCI_COMMAND_TASK_PRIORITY 3 //should be arrange by scenario
#define ATCI_COMMAND_QUEUE_SIZE 10

TaskHandle_t at_command_task_hdlr;
QueueHandle_t at_command_task_queue;
static TimerHandle_t event_timer = NULL;

static bool acti_bt_check_event_complete(uint8_t event)
{
    bool is_complete = 0;
    LOG_I(atci_bt, "check_event_complete--event = %d\r\n", event);
    switch (event) {
        case ATCI_BT_EVENT_INQUIRY_COMPLETE:
        case ATCI_BT_EVENT_CONNECT_COMPLETE:
        case ATCI_BT_EVENT_DISCONNECT_COMPLETE:
        case ATCI_BT_EVENT_AUTH_COMPLETE:
        case ATCI_BT_EVENT_REMOTE_NAME_REQ_COMPLETE:
        case ATCI_BT_EVENT_CHNG_CONN_LINK_KEY_COMPLETE:
        case ATCI_BT_EVENT_MASTER_LINK_KEY_COMPLETE:
        case ATCI_BT_EVENT_READ_REMOTE_FEATURES_COMPLETE:
        case ATCI_BT_EVENT_READ_REMOTE_VERSION_COMPLETE:
        case ATCI_BT_EVENT_QOS_SETUP_COMPLETE:
        case ATCI_BT_EVENT_COMMAND_COMPLETE:
        case ATCI_BT_EVENT_READ_CLOCK_OFFSET_COMPLETE:
        case ATCI_BT_EVENT_FLOW_SPECIFICATION_COMPLETE:
        case ATCI_BT_EVENT_READ_REMOTE_EXT_FEAT_COMPLETE:
        case ATCI_BT_EVENT_SYNC_CONNECT_COMPLETE:
        case ATCI_BT_EVENT_SIMPLE_PAIRING_COMPLETE:
        case ATCI_BT_EVENT_ENHANCED_FLUSH_COMPLETE: {
            is_complete = 1;
            break;
        }
        default:
            break;
    }
    LOG_I(atci_bt, "check_event_complete--is_complete = %d\r\n", is_complete);
    return is_complete;
}

static void acti_bt_delete_timer()
{
    if (event_timer && (xTimerIsTimerActive(event_timer) != pdFALSE)) {
        LOG_I(atci_bt, "event complete: timer is exist\r\n");
        xTimerStop(event_timer, 0);
        xTimerDelete(event_timer, 0);
    }
}


void acti_bt_event_timer_handle_func(TimerHandle_t timer_id)
{
    LOG_I(atci_bt, "timer_handle:timer out\r\n");

    acti_bt_delete_timer();
    atci_bt_error_handle();

}


static void acti_bt_start_check_complete()
{


    if (event_timer && (xTimerIsTimerActive(event_timer) != pdFALSE)) {
        xTimerStop(event_timer, 0);

    } else {
        LOG_I(atci_bt, "timer is not exist\r\n");
        event_timer = xTimerCreate( "acti_bt_timer",
                                    (100000 / portTICK_PERIOD_MS),
                                    pdFALSE,
                                    NULL,
                                    acti_bt_event_timer_handle_func);
    }
    xTimerStart(event_timer, 0);
}


/*--- Callback Function ---*/
static void acti_bt_task_msg_handler(at_command_bt_message_struct_t *message)
{
    uint8_t result = 0;
    atci_bt_hci_event hci_event;
    atci_response_t output = {{0}};
    uint8_t string[526] = {0};
    uint8_t *temp_str;

    LOG_I(atci_bt, "msglen :0x%x", message->param1);
    temp_str = message->param2;
    for (int i = 0 ; i < message->param1; i++) {
       LOG_I(atci_bt, "msg_handler--msg:%x", *(temp_str + i));
    }

    result = bt_hci_event_parser((uint8_t *)message->param2, message->param1, &hci_event);

    LOG_I(atci_bt, "parser result:%d", result);

    if (result == 1) {
        atci_bt_error_handle();
    } else {
        result = atci_bt_hci_event_to_string(&hci_event, string, 522);
        LOG_I(atci_bt, "event to string result:%d", result);
        if (result > 0) {
            bool is_complete;
            snprintf((char *)output.response_buf, sizeof(output.response_buf),"\r\n+EBTSHC:%s\r\n", (char *)string);
            LOG_I(atci_bt, "event to string event:%d", hci_event.event);
            is_complete = acti_bt_check_event_complete(hci_event.event);
            if (is_complete) {
                LOG_I(atci_bt, "event is complete.");
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                acti_bt_delete_timer();
            } else {
                LOG_I(atci_bt, "event is status.");
                output.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                acti_bt_start_check_complete();
            }

            output.response_len = strlen((char *)output.response_buf);
        } else {
            strcpy((char *)output.response_buf, "Response data error!\n");
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            output.response_len = strlen((char *)output.response_buf);
            acti_bt_delete_timer();
        }
        atci_send_response(&output);
    }

}

void atci_bt_hci_cmd_rx_callback(uint8_t *data, uint32_t dataLen)
{   
    at_command_bt_message_struct_t msg = {0,0,{0}};
    msg.message_id = ATCI_BT_COMMAND_HCI_EVENT;
    msg.param1 = dataLen;
    memcpy(msg.param2, data, dataLen);
    xQueueSendFromISR(at_command_task_queue, &msg, 0);

    return;
}


void bt_gap_ata_callback(bt_event_t event_id, void *param, int32_t *handle)
{
    LOG_I(atci_bt, "receive event: 0x%x, ata:%d", event_id, atci_bt_cntx_p->ata);

    /*no ata mode*/
    if (!atci_bt_cntx_p->ata) {
        return;
    }

    *handle = 1;

    switch (event_id) {
        case BT_GAP_POWER_ON_CNF:
            atci_bt_power_on_cnf(param);
            break;
        case BT_GAP_POWER_OFF_CNF:
            atci_bt_power_off_cnf(param);
            break;

        case BT_GAP_DISCOVERY_RESULT_IND:
            atci_bt_gap_discovery_result_ind(param);
            break;

        case BT_GAP_DISCOVERY_CNF:
            atci_bt_gap_discovery_cnf(param);
            break;

        case BT_GAP_READ_REMOTE_NAME_CNF:
            atci_bt_gap_read_remote_name_cnf(param);
            break;
        default:
            break;
    }
}

/*--- Command handler Function ---*/
atci_status_t atci_cmd_hdlr_bt_ata(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /*AT+EBTAT=<op>*/
            atci_bt_ata_cmd_paser((uint8_t *)parse_cmd->string_ptr + 9);
            break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            strcpy((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
        }
        break;
    }

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_bt_power(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    bool result = false;
    uint8_t register_result;

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { /*AT+EBTPW=<op>*/
            LOG_I(atci_bt, "bt_power : %d", atci_bt_cntx_p->is_power);
            if (strncmp(parse_cmd->string_ptr, "AT+EBTPW=0", 10) == 0) {
                if (atci_bt_cntx_p->is_power) {
                    result = BTIF_Power_off();
                    atci_bt_cntx_p->is_register_rx_callback = false;
                    acti_bt_task_deinit();
                    atci_bt_cntx_p->is_power = 0;
                } else {
					result = true;
				}
            } else if (strncmp(parse_cmd->string_ptr, "AT+EBTPW=1", 10) == 0) {

                if (!atci_bt_cntx_p->is_power) {
                    result = BTIF_Power_on();
                    if (result) {
                        vTaskDelay(500);
						int32_t download_result = 0;
                        download_result = btif_patch_download();
						LOG_I(atci_bt, "patch download result: %d", download_result);
                        atci_bt_cntx_p->is_power = 1;
                        register_result = BTIF_Register_ATCI_Rx_Callback((void*)atci_bt_hci_cmd_rx_callback);
                        atci_bt_task_init();
                        if (register_result == 0) {
                            atci_bt_cntx_p->is_register_rx_callback = true;
                        } else {
                            atci_bt_cntx_p->is_register_rx_callback = false;
                        }
                    }
                } else {
					result = true;
				}
            } else {
                ; //not support
            }

            LOG_I(atci_bt, "result: 0x%x", result);
            if (result) {
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            output.response_len = 0;
        }
        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            strcpy((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }

    atci_send_response(&output);
    return ATCI_STATUS_OK;
}


static void atci_bt_cmd_task_main(void *pvParameters)
{

    at_command_bt_message_struct_t queue_item;
    LOG_I(atci_bt, "enter task main.");

    at_command_task_queue = xQueueCreate(ATCI_COMMAND_QUEUE_SIZE, sizeof(at_command_bt_message_struct_t));
    while (1) {
        if (xQueueReceive(at_command_task_queue, &queue_item, portMAX_DELAY)) {
            LOG_I(atci_bt, "enter massge handler.");
            acti_bt_task_msg_handler(&queue_item);
        }
    }

}

static void atci_bt_task_init(void)
{
    LOG_I(atci_bt, "atci_bt_task_init.");

    xTaskCreate(atci_bt_cmd_task_main, ATCI_COMMAND_TASK_NAME, ATCI_COMMAND_TASK_STACK_SIZE, NULL, ATCI_COMMAND_TASK_PRIORITY, &at_command_task_hdlr);
}

static void acti_bt_task_deinit(void)
{
    vTaskDelete(at_command_task_hdlr);
}

atci_status_t atci_cmd_hdlr_bt_relay(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    bool result;

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_ACTIVE: { /*AT+EBTER*/
            result = bt_enable_relay();
            LOG_I(atci_bt, "enter relay mode result: 0x%x", result);
            if (result) {
                output.response_len = 0;
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                output.response_len = 0;
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
        }
        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_EXECUTION:
        default: {
            strcpy((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }

    atci_send_response(&output);

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_bt_send_hci_command(atci_parse_cmd_param_t *parse_cmd)
{
    int8_t result;
    atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { //AT+EBTSHC=<op>
            LOG_I(atci_bt, "cmd_len:%d, name_len:%d, string_len:%d", strlen(parse_cmd->string_ptr), parse_cmd->name_len, parse_cmd->string_len);
            uint32_t op_length = strlen(parse_cmd->string_ptr) - parse_cmd->name_len - 1 - 2;
            result = atci_bt_hci_string_to_cmd((uint8_t *)parse_cmd->string_ptr + 10, op_length, &hci_cmd);
            LOG_I(atci_bt, "string to hci cmd result:%d", result);
            if (result > 0) {
                result = BTIF_Hci_Send_Command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);
                atci_bt_cntx_p->is_sent_cmd = true;
                LOG_I(atci_bt, "result:%d", result);
            } else {
                output.response_flag = 0;
                strcpy((char *)output.response_buf, "Command Error\n");
                output.response_len = strlen((char *)output.response_buf);
                atci_send_response(&output);
            }
        }

        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            strcpy((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
        }
        break;
    }

    return ATCI_STATUS_OK;
}

/*--- Static Function ---*/
static void atci_bt_error_handle(void)
{
    atci_response_t output;
    LOG_I(atci_bt, "exception handle.");

    output.response_len = 0;
    output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
    atci_send_response(&output);
}

static int8_t atci_bt_str2hex(uint8_t *string, uint32_t string_len, uint8_t data[])
{
    uint32_t i;
    uint8_t value = 0;
    uint8_t composed_value = 0;
    LOG_I(atci_bt, "string : %s, string_len : %d", string, string_len);

    for (i = 0; i < string_len; i++) {
        if ('0' <= string[i] && '9' >= string[i]) {
            value = string[i] - '0';
        } else if ('a' <= string[i] && 'f' >= string[i]) {
            value = string[i] - 'a' + 10;
        } else if ('A' <= string[i] && 'F' >= string[i]) {
            value = string[i] - 'A' + 10;
        } else {
            return -1;
        }

        if (i % 2 == 0) {
            composed_value += value * 16;
        } else {
            composed_value += value;
        }

        if (i % 2 == 1) {
            data[(i - 1) / 2] = composed_value;
            composed_value = 0;
        }

    }

    return 1;
}

static int8_t atci_bt_hex2str(uint8_t data[], uint8_t data_len, uint8_t *string, uint32_t string_len)
{
    uint32_t i;
    uint8_t high_8bits_string = 0;
    uint8_t low_8bits_string = 0;

    if ( (2 * data_len) > string_len ) {
        return -1;
    }

    for (i = 0; i < data_len; i++) {
        /*parse high 8 bits value*/
        high_8bits_string = data[i] / 16;
        if (high_8bits_string <= 9) {
            string[2 * i] = high_8bits_string + '0';
        } else if (10 <= high_8bits_string && high_8bits_string <= 15) {
            string[2 * i] = high_8bits_string - 10 + 'A';
        } else {
            return -2;
        }

        /*parse low 8 bits value*/
        low_8bits_string = data[i] % 16;
        if (low_8bits_string <= 9) {
            string[2 * i + 1] = low_8bits_string + '0';
        } else if (10 <= low_8bits_string && low_8bits_string <= 15) {
            string[2 * i + 1] = low_8bits_string - 10 + 'A';
        } else {
            return -3;
        }
    }

    return 1;
}

static int8_t atci_bt_hci_event_to_string(atci_bt_hci_event *hci_event, uint8_t *string, uint32_t string_len)
{
    int8_t parse_result = 0;
    uint8_t data[260];
    uint32_t string_pos = 0;
    uint32_t data_pos = 0;

    LOG_I(atci_bt, "event parmslen:%d", hci_event->parmslen);

    /*init data[]*/
    data[0] = hci_event->event;
    data[1] = hci_event->status ;
    data[2] = (uint8_t)((hci_event->handle & 0xFF00) >> 8);
    data[3] = (uint8_t)(hci_event->handle & 0x00FF) ;
    data[4] = hci_event->parmslen;
    memcpy(data + 5, hci_event->parms, hci_event->parmslen);

    /*1. parser hci_event->event*/
    parse_result = atci_bt_hex2str(data + data_pos, 1, string + string_pos, string_len);
    if (parse_result <= 0 ) {
        return -1;
    }
    string_pos += 2;
    data_pos += 1;
    string[string_pos++] = ',';

    /*2. parser hci_event->status*/
    parse_result = atci_bt_hex2str(data + data_pos, 1, string + string_pos, string_len - string_pos);
    if (parse_result <= 0 ) {
        return -2;
    }
    string_pos += 2;
    data_pos += 1;
    string[string_pos++] = ',';

    /*3. parser hci_event->handle*/
    parse_result = atci_bt_hex2str(data + data_pos, 2, string + string_pos, string_len - string_pos);
    if (parse_result <= 0 ) {
        return -3;
    }
    string_pos += 4;
    data_pos += 2;
    string[string_pos++] = ',';

    /*4. parser hci_event->parmslen*/
    parse_result = atci_bt_hex2str(data + data_pos, 1, string + string_pos, string_len - string_pos);
    if (parse_result <= 0 ) {
        return -5;
    }
    string_pos += 2;
    data_pos += 1;

    /*5. parser hci_event->parms[256]*/
    if (hci_event->parmslen > 0) {
        string[string_pos++] = ',';
        parse_result = atci_bt_hex2str(data + data_pos, hci_event->parmslen, string + string_pos, string_len - string_pos);
        if (parse_result <= 0 ) {
            return -6;
        }
        string_pos += 2 * hci_event->parmslen;
        data_pos += hci_event->parmslen;
    }

    string[string_pos] = '\0';

    return 1;
}

static int8_t atci_bt_hci_string_to_cmd(uint8_t *string, uint32_t string_len, atci_bt_hci_cmd *hci_cmd)
{
    int8_t parse_result = 0;
    uint8_t data[259];
    uint32_t string_pos = 0;
    uint32_t data_pos = 0;

    LOG_I(atci_bt, "command: %s, len:%d", string, string_len);

    /*reset hci cmd*/
    hci_cmd->op_code = 0;
    hci_cmd->cmd_len = 0;
    memset(hci_cmd->cmd, 0x0, 256);

    /*parse hci cmd string*/
    /*0401,02,1234*/

    /*parse opcode*/
    parse_result = atci_bt_str2hex(string + string_pos, 4, data + data_pos);
    if (parse_result <= 0) {
        return -1;
    }
    string_pos += 4;
    data_pos += 2;

    /*parse 1st ','*/
    if (string[string_pos] != ',') {
        return -2;
    }
    string_pos += 1;

    /*parse cmd length*/
    parse_result = atci_bt_str2hex(string + string_pos, 2, data + data_pos);
    if (parse_result <= 0) {
        return -3;
    }

    if (data[data_pos] == 0) {
        goto exit;
    }

    string_pos += 2;
    data_pos += 1;

    /*parse 2nd ','*/
    if (string[string_pos] != ',') {
        return -4;
    }
    string_pos += 1;

    LOG_I(atci_bt, "string : %s, str_pos:%d, data_pos = %d", string + string_pos, string_pos, data_pos);
    /*parse cmd data*/
    if (data[2] > 0) {
        parse_result = atci_bt_str2hex(string + string_pos, string_len - string_pos, data + data_pos);
        if (parse_result <= 0) {
            return -5;
        }
    }

exit:
    hci_cmd->op_code = ((data[0] << 8) | data[1]);
    hci_cmd->cmd_len = data[2];
    if (hci_cmd->cmd_len > 0) {
        memcpy(hci_cmd->cmd, data + 3, hci_cmd->cmd_len);
    }

    return 1;
}

static void atci_bt_ata_cmd_paser(uint8_t *string)
{
    atci_bt_cntx_p->ata = true;
    LOG_I(atci_bt, "enter parser, command: %s", string);
    if (strncmp((char *)string, "bt_power_on", 11) == 0) {
        atci_bt_power_on_handler();
    } else if (strncmp((char *)string, "bt_power_off", 12) == 0) {
        //check is power off
        atci_bt_power_off_handler();
    } else if (strncmp((char *)string, "bt_search", 9) == 0) {
        atci_bt_search_handler();
    } else {
        atci_bt_error_handle();
    }
}

static void atci_bt_power_on_handler(void)
{
    atci_response_t output;
    bt_status_t result;

    result = bt_gap_power_on();

    LOG_I(atci_bt, "power on, result: 0x%x", result);

    if (BT_STATUS_GAP_PENDING == result) {
        /*waiting power on cnf event*/
        return;
    } else {
        if (BT_STATUS_SUCCESS == result) {
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
        } else {
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        output.response_len = 0;
        atci_send_response(&output);
    }

}

static void atci_bt_power_on_cnf(void *param)
{
    atci_response_t output;
    bt_gap_power_on_cnf_t result = *(bt_gap_power_on_cnf_t *)param;

    LOG_I(atci_bt, "power on cnf result: 0x%x", result);

    if (result == BT_STATUS_SUCCESS) {
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
    } else {
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }

    output.response_len = 0;
    atci_send_response(&output);
}


static void atci_bt_power_off_handler(void)
{
    atci_response_t output;
    bt_status_t result;

    result = bt_gap_power_off();
    LOG_I(atci_bt, "power off result: 0x%x", result);

    if (BT_STATUS_GAP_PENDING == result) {
        /*waiting power off cnf event*/
        return;
    } else {
        if (BT_STATUS_SUCCESS == result) {
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
        } else {
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        output.response_len = 0;
        atci_send_response(&output);
        memset((void *)atci_bt_cntx_p, 0x0, sizeof(atci_bt_context_struct_t));
    }
}


static void atci_bt_power_off_cnf(void *param)
{
    atci_response_t output;
    bt_gap_power_off_cnf_t result = *(bt_gap_power_off_cnf_t *)param;

    LOG_I(atci_bt, "power off cnf result: 0x%x", result);

    if (result == BT_STATUS_SUCCESS) {
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
    } else {
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }

    output.response_len = 0;
    atci_send_response(&output);
}

static void atci_bt_gap_discovery_result_ind(void *param)
{
    bt_gap_discovery_result_ind_t *result = (bt_gap_discovery_result_ind_t *)param;

    LOG_I(atci_bt, "discovery result ind: 0x%x", result);
    if (result) {
        atci_bt_cntx_p->searched_num++ ;
        bt_gap_read_remote_name(&(result->dev_addr));
    } else {
        atci_bt_error_handle();
    }
}

static void atci_bt_gap_discovery_cnf(void *param)
{
    atci_response_t output;
    bt_gap_power_on_cnf_t result = *(bt_gap_power_on_cnf_t *)param;

    LOG_I(atci_bt, "discovery cnf result: 0x%x, searched num: %d", result, atci_bt_cntx_p->searched_num);

    atci_bt_cntx_p->is_inquiry_completed = true;

    if (atci_bt_cntx_p->searched_num == 0) {
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
        output.response_len = 0;
        atci_send_response(&output);
    }
}

static void atci_bt_gap_read_remote_name_cnf(void *param)
{
    atci_response_t output = {{0}};
    uint8_t temp_str[256];
    bt_gap_read_remote_name_cnf_t *parameter = (bt_gap_read_remote_name_cnf_t *)param;
    bt_address_t remote_address;

    if (parameter == NULL) {
        atci_bt_error_handle();
        return;
    }

    memcpy((void *)&remote_address, (void *) & (parameter->dev_addr.address), 6);
    atci_bt_cntx_p->searched_num-- ;

    LOG_I(atci_bt, "read remote name cnf result: 0x%x, searched num: %d", parameter->result, atci_bt_cntx_p->searched_num);
    if (BT_STATUS_SUCCESS == parameter->result) {
        /*send URC to atci*/
        snprintf((char *)temp_str, sizeof(temp_str),"%s, 0x%.2X%.2X%.2X%.2X%.2X%.2X", parameter->name, remote_address.address[5], remote_address.address[4], remote_address.address[3], remote_address.address[2], remote_address.address[1], remote_address.address[0]);
        snprintf ((char *)output.response_buf, sizeof(output.response_buf), "\r\n+EBTAT:%s\r\n", (char *)temp_str);
        LOG_I(atci_bt, "response string:%s", output.response_buf);
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_URC_FORMAT;
        atci_send_response(&output);

        /*send final response to atci*/
        if (atci_bt_cntx_p->is_inquiry_completed && (atci_bt_cntx_p->searched_num == 0)) {
            memset((void *)&output, 0x0,  sizeof(atci_response_t));
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            output.response_len = 0;
            atci_send_response(&output);
        }
    } else {
        atci_bt_error_handle();
    }
}

static void atci_bt_search_handler(void)
{
    atci_response_t output;
    bt_status_t result;

    result = bt_gap_inquiry(ATCI_BT_ATA_INQUIRY_NUM, ATCI_BT_ATA_INQUIRY_TIME);

    LOG_I(atci_bt, "inquiry result: 0x%x", result);
    if (BT_STATUS_GAP_PENDING == result) {
        /*waiting inquiry cnf event*/
    } else {
        output.response_len = 0;
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(&output);
    }
}


#endif /*MTK_BT_AT_COMMAND_ENABLE*/

