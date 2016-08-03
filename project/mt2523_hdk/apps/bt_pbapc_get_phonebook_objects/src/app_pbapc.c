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

#include "bt_pbapc.h"
#include <FreeRTOS.h>

#include "queue.h"
#include "task.h"

#include "app_pbapc.h"
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include "bt_gap.h"
#include <stdint.h>
#ifndef WIN32
#include "portable.h"
#endif
#include "syslog.h"
#include <string.h>
#include <ctype.h>

//#ifdef __APP_PBAPC_TEST__
log_create_module(app_pbapc, PRINT_LEVEL_INFO);

/*****************************************************************************
 * typedef
 *****************************************************************************/
typedef struct {
    bt_event_t event_id;
    int8_t param[512];
} app_pbapc_queue_struct_t;


app_pbapc_cntx_t g_app_cntx[MAX_EDR_RMT_DEV_NUM];
//app_pbapc_queue_struct_t queue_item;//by min


/* fota queue to store msg from bt */

QueueHandle_t app_queue = NULL;
app_pbapc_queue_struct_t app_pbapc_queue_data;


/*****************************************************************************
 * define
 *****************************************************************************/
#define APP_QUEUE_SIZE      10

static uint8_t *g_phb_pbapc_pull_phonebook[] = {
    (uint8_t *)"telecom/pb.vcf",
    (uint8_t *)"telecom/ich.vcf",
    (uint8_t *)"telecom/och.vcf",
    (uint8_t *)"telecom/mch.vcf",
    (uint8_t *)"telecom/cch.vcf",
    (uint8_t *)"SIM1/telecom/pb.vcf",
    (uint8_t *)"SIM1/telecom/ich.vcf",
    (uint8_t *)"SIM1/telecom/och.vcf",
    (uint8_t *)"SIM1/telecom/mch.vcf",
    (uint8_t *)"SIM1/telecom/cch.vcf",
    (uint8_t *)"End"
};

static uint8_t *g_phb_pbapc_set_path[] = {
    (uint8_t *)"",
    (uint8_t *)"telecom",
    (uint8_t *)"pb",
    (uint8_t *)"ich",
    (uint8_t *)"och",
    (uint8_t *)"mch",
    (uint8_t *)"cch",
    (uint8_t *)"SIM1",
};

static void app_pbapc_read_data(uint8_t type, uint8_t repo, uint8_t path, uint16_t conn_id);
static uint8_t app_pbapc_get_index_by_id(uint16_t conn_id);
static void app_pbapc_reset_conn_id(uint16_t conn_id);
//static void GetName_outlook(uint8_t *dataBuf);
static uint16_t app_asc_str_to_ucs2_str(int8_t *pOutBuffer, int8_t *pInBuffer, uint8_t out_len);
static uint8_t app_unicode_to_ucs2encoding(uint16_t unicode, uint8_t *charLength, uint8_t *arrOut);
static void app_pbapc_pull_vcard_entry(uint16_t conn_id, uint8_t type, uint8_t *cmd);

void bt_pbapc_test_callback_hdlr(bt_event_t event_id, void *param);

#if defined(__ICCARM__) || defined(__CC_ARM)
char *strupr(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = toupper(*str);
    return orign;
}
#endif

void pbap_string_to_bd_addr(const char *string, bt_address_t *bt_addr)
{
    char str[13] = {0};
    int32_t idx = 0;
    memcpy(str, string, 12);
    strupr(str);
    while (idx < 12) {
        uint8_t num = str[idx];
        if (num >= 0x30 && num <= 0x39) {
            num -= 0x30;
        } else if (num >= 'A' && num <= 'F') {
            num -= 0x37;
        }

        if (idx & 0x01) {
            bt_addr->address[idx >> 1] |= num;
        } else {
            bt_addr->address[idx >> 1] = (num << 4);
        }
        ++idx;
    }
    return;
}

void pbapc_command_handler(uint8_t *cmd)
{
    uint8_t i;
    bt_address_t addr;
    //bt_status_t status;
    LOG_I(app_pbapc, "[APP_PBAPC]pbapc_command_handler: c = %s\r\n", cmd);

    memset(&addr, 0, sizeof (bt_address_t));
    switch (cmd[1]) {
        case 'b': {
            pbap_string_to_bd_addr((char *)(cmd + 2), &addr);
            //status = bt_pbapc_connect(&addr);
            bt_pbapc_connect(&addr);
            break;
        }
        case 'd': {
            for (i = 0; i < MAX_EDR_RMT_DEV_NUM; i++) {
                if (g_app_cntx[i].flag) {
                    bt_pbapc_disconnect(g_app_cntx[i].conn_id);
                }
            }
            break;
        }
        case 'f': { /*read phone*/
            //int8_t rep, path;
            int8_t path;
            path = atoi((char *)(cmd + 2));
            for (i = 0; i < MAX_EDR_RMT_DEV_NUM; i++) {
                if (g_app_cntx[i].flag) {
                    app_pbapc_read_data(1, 1, path, g_app_cntx[i].conn_id);
                }
            }

            break;
        }

        case 'p': {
            int8_t path;
            path = atoi((char *)(cmd + 2));/* rep is 1 or 2 represents phone\sim */

            for (i = 0; i < MAX_EDR_RMT_DEV_NUM; i++) {
                if (g_app_cntx[i].flag) {
                    g_app_cntx[i].path = path;
                    app_pbapc_read_data(2, 0, path, g_app_cntx[i].conn_id);
                }
            }
            break;
        }
        case 'v': {

            for (i = 0; i < MAX_EDR_RMT_DEV_NUM; i++) {
                if (g_app_cntx[i].flag) {
                    app_pbapc_read_data(3, 0, 0, g_app_cntx[i].conn_id);
                }
            }
            break;
        }
        case 'e': {
            uint8_t len, name[10];
            len = strlen((char *)cmd);
            memcpy(name, cmd + 2, len);
            name[len] = '\0';
            strcat((char *)name, ".vcf");
            for (i = 0 ; i < MAX_EDR_RMT_DEV_NUM; i++) {

                if (g_app_cntx[i].flag) {
                    if (len > 2) {
                        app_pbapc_pull_vcard_entry(g_app_cntx[i].conn_id, 4, name);
                    }
                }

            }
            break;
        }
        case 'l': {
            uint8_t num;
            num = bt_pbapc_get_link_num();
            LOG_I(app_pbapc, "[APP_PBAPC]LINK NUM is :%d\r\n", num);
            break;
        }
        case 'n': {
            for (i = 0; i < MAX_EDR_RMT_DEV_NUM; i++) {

                memset(g_app_cntx + i, 0 , sizeof (app_pbapc_cntx_t));
            }
            break;
        }
        default: {
            break;
        }
    }
}


static void app_pbapc_read_data(uint8_t type, uint8_t repo, uint8_t path, uint16_t conn_id)
{
    switch (type) {
        case 1: { /*down load*/
            //uint8_t file_name[64], len, i ;
            uint8_t file_name[64];
            uint8_t *p_path = NULL;
            bt_pbapc_pull_phonebook_params_t param;
            param.filter = BT_PBAPC_VCARD_FILTER_ALL;
            param.list_start_offset = 0 ;
            param.max_list_count = MAX_PBAPC_READ_COUNT;
            param.format =  BT_PBAPC_VCARD_FORMAT_21;
            path = 5 * (repo - 1) + path;
            LOG_I(app_pbapc, "[APP_PBAPC][PULL_PHONEBOOK]index = %d\r\n", path);

            p_path = g_phb_pbapc_pull_phonebook[path];
            app_asc_str_to_ucs2_str((int8_t *)file_name, (int8_t *)p_path, 64);
            LOG_I(app_pbapc, "[APP_PBAPC][PULL_PHONEBOOK]path_name = %s, conn_id = %d\r\n", g_phb_pbapc_pull_phonebook[path], conn_id);
            bt_pbapc_pull_phonebook(conn_id, file_name, &param);

            break;
        }
        case 2: { /*browsing*/
            uint8_t file_name[64];
            uint8_t *p_path = NULL;
            LOG_I(app_pbapc, "[APP_PBAPC][SET_PATH]index = %d\r\n", path);
            if (path > 7) {
                LOG_I(app_pbapc, "[APP_PBAPC][SET_PATH]out of index range\r\n");
            }
            if (!path) {
                bt_pbapc_set_phonebook(conn_id, NULL, BT_PBAPC_FOLDER_TYPE_ROOT);

            } else {

                LOG_I(app_pbapc, "[APP_PBAPC][PULL_PHONEBOOK]path_name = %s, conn_id = %d\r\n", g_phb_pbapc_set_path[path], conn_id);
                p_path = g_phb_pbapc_set_path[path];
                app_asc_str_to_ucs2_str((int8_t *)file_name, (int8_t *)p_path, 64);

                bt_pbapc_set_phonebook(conn_id, file_name,  BT_PBAPC_FOLDER_TYPE_FORWARD);
            }

            break;
        }
        case 3: {
            bt_pbapc_pull_vcard_listing_t list_req;
            memset(&list_req, 0, sizeof (bt_pbapc_pull_vcard_listing_t));
            list_req.search_attribute = BT_PBAPC_SEARCH_ATTRIBUTE_NAME;
            list_req.order = BT_PBAPC_SORT_ORDER_BY_ALPHABETIC;
            list_req.search_value_length = 0;
            list_req.max_list_count = MAX_PBAPC_READ_COUNT;
            list_req.list_start_offset = 0;
            bt_pbapc_pull_vcard_listing(conn_id, NULL, &list_req);
            break;
        }
        default:
            break;

    }
}


static void app_pbapc_pull_vcard_entry(uint16_t conn_id, uint8_t type, uint8_t *cmd)
{
    switch (type) {
        case 4: {
            uint8_t name[64];
            bt_pbapc_pull_vcard_entry_t req;
            req.format = BT_PBAPC_VCARD_FORMAT_21;
            req.filter = BT_PBAPC_VCARD_FILTER_N | BT_PBAPC_VCARD_FILTER_FN | BT_PBAPC_VCARD_FILTER_TEL;
            memset(name, 0, 64);
            app_asc_str_to_ucs2_str((int8_t *)name, (int8_t *)cmd, 64);
            bt_pbapc_pull_vcard_entry(conn_id, name, &req);
            break;
        }
        case 5: {
            bt_pbapc_pull_vcard_listing_t list_req;
            memset(&list_req, 0, sizeof (bt_pbapc_pull_vcard_listing_t));
            list_req.search_attribute = BT_PBAPC_SEARCH_ATTRIBUTE_NAME;
            list_req.order = BT_PBAPC_SORT_ORDER_BY_ALPHABETIC;
            list_req.max_list_count = MAX_PBAPC_READ_COUNT;
            list_req.list_start_offset = 0;
            LOG_I(app_pbapc, "[APP_PBAPC][PULL VCARD]:Name: searchvalue = %s\r\n", cmd);
            list_req.search_value_length = strlen((char *)cmd);
            if (list_req.search_value_length > MAX_BT_PBAPC_SEARCH_VALUE_LEN + 1) {
                list_req.search_value_length = MAX_BT_PBAPC_SEARCH_VALUE_LEN + 1;
            }
            memcpy(list_req.search_value, cmd, list_req.search_value_length );
            bt_pbapc_pull_vcard_listing(conn_id, NULL, &list_req);
            break;
        }
#if 0
        case 6: {
            bt_pbapc_pull_vcard_listing_t list_req;
            memset(&list_req, 0, sizeof (bt_pbapc_pull_vcard_listing_t));
            list_req.search_attribute = BT_PBAPC_SEARCH_ATTRIBUTE_NUMBER;
            list_req.order = BT_PBAPC_SORT_ORDER_BY_INDEXED;

            list_req.max_list_count = MAX_PBAPC_READ_COUNT;
            list_req.list_start_offset = 0;
            LOG_I(app_pbapc, "[APP_PBAPC][PULL VCARD]:Number: searchvalue = %s\r\n", cmd);
            list_req.search_value_length = strlen(cmd);
            memcpy(list_req.search_value, cmd, list_req.search_value_length);
            list_req.search_value_length = strlen(&list_req.search_value);
            bt_pbapc_pull_vcard_listing(conn_id, NULL, &list_req);
            break;
        }
#endif
        default:
            break;
    }
}


static void app_pbapc_track_data(uint16_t conn_id)
{
    uint32_t len;
    uint8_t i = app_pbapc_get_index_by_id(conn_id);

    if (i >= MAX_EDR_RMT_DEV_NUM) {
        return;
    }
    LOG_I(app_pbapc, "[APP_PBAPC]show_data: con_id = %d, data = %x\r\n", conn_id, g_app_cntx[i].data);
    if (g_app_cntx[i].data) {
        len = g_app_cntx[i].offset;
        LOG_I(app_pbapc, "[APP_PBAPC]show_data: len = %d\r\n", len);
        if (len < 6000) {
            g_app_cntx[i].data[len] = '\0';
            LOG_I(app_pbapc, "%s\r\n", g_app_cntx[i].data);

#ifndef WIN32
            vPortFree(g_app_cntx[i].data);
#else
            free(g_app_cntx[i].data);
#endif


        }

        g_app_cntx[i].data = NULL;
        g_app_cntx[i].offset = 0;
    }
}


void app_pbapc_test_task(void *arg)
{

    //bt_event_t event_id;
    //void *param;
    LOG_I(app_pbapc, "[APP_PBAPC]App test task begin\r\n");
    /*queue ring buffer*/

    app_queue = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_pbapc_queue_struct_t));
    if ( app_queue == NULL ) {
        LOG_I(app_pbapc, "[APP_PBAPC]create queue failed!\r\n");
        return;
    }
    memset((void *)&app_pbapc_queue_data, 0, sizeof(app_pbapc_queue_struct_t));

    while (1) {
        LOG_I(app_pbapc, "[APP_PBAPC]try to get one msg\r\n");
        if (xQueueReceive(app_queue, (void *)&app_pbapc_queue_data, portMAX_DELAY)) {

            //event_id = app_pbapc_queue_data.event_id;
            //param =  (void *)(app_pbapc_queue_data.param);
            LOG_I(app_pbapc, "[APP_PBAPC]handle one message id = %d\r\n", app_pbapc_queue_data.event_id);
            bt_pbapc_test_callback_hdlr(app_pbapc_queue_data.event_id, (void *)(app_pbapc_queue_data.param));
        }
    }
}


void bt_pbapc_test_callback_hdlr(bt_event_t event_id, void *param)
{
    uint8_t i;
    LOG_I(app_pbapc, "[APP_PBAPC]bt_pbapc_test_callback_hdlr: evt_id = %d\r\n", event_id);

    switch (event_id) {
        case BT_PBAPC_CONNECT_CNF: {
            bt_pbapc_connect_cnf_t *data = (bt_pbapc_connect_cnf_t *)param;
            LOG_I(app_pbapc, "[APP_PBAPC]CONN_CNF: status = %d, conn_id = %d\r\n", data->status, data->conn_id);

            if (data->status == BT_STATUS_PBAPC_OK) {
                for (i = 0 ; i < MAX_EDR_RMT_DEV_NUM; i++) {
                    LOG_I(app_pbapc, "[APP_PBAPC]CONN_CNF: flag  = %d\r\n", g_app_cntx[i].flag);
                    if (g_app_cntx[i].flag == 0) {
                        g_app_cntx[i].flag = 1;
                        g_app_cntx[i].conn_id = data->conn_id;
                        LOG_I(app_pbapc, "[APP_PBAPC]CONN_CNF: flag  = %d\r\n", g_app_cntx[i].conn_id);
                        g_app_cntx[i].data = NULL;
                        g_app_cntx[i].offset = 0;
                        break;
                    }
                }
            }
            break;
        }
        case BT_PBAPC_AUTH_RESPONSE_CNF: {
            bt_pbapc_auth_response_cnf_t *data = (bt_pbapc_auth_response_cnf_t *)data;
            if (data) {
                LOG_I(app_pbapc, "[APP_PBAPC]AUTH_RESPONSE:result = %d\r\n", data->status);
            }

            break;
        }

        case BT_PBAPC_AUTH_IND: {
            uint8_t pass_word[6] = "0000", user_id[6] = "0000";

            bt_pbapc_auth_challenge_ind_t *data = (bt_pbapc_auth_challenge_ind_t *)param;

            LOG_I(app_pbapc, "[PBAPC]AUTH_IND:\r\n");

            bt_pbapc_connect_auth_response(&data->addr, pass_word, user_id, 0);

            break;
        }
        case BT_PBAPC_SET_PHONEBOOK_CNF: {
            bt_pbapc_set_phonebook_cnf_t *data = (bt_pbapc_set_phonebook_cnf_t *)param;
            LOG_I(app_pbapc, "[APP_PBAPC]SET_PATH: result = %d\r\n", data->status);

            if (data->status == BT_STATUS_PBAPC_OK) {
                i = app_pbapc_get_index_by_id(data->conn_id);
                LOG_I(app_pbapc, "[APP_PBAPC]SET_PATH: i = %d, conn_id = %d\r\n", i, data->conn_id);
                if (i >= MAX_EDR_RMT_DEV_NUM) {
                    LOG_I(app_pbapc, "[PBAPC]SET_PATH :index error \r\n");
                    return ;
                }

                LOG_I(app_pbapc, "[APP_PBAPC]SET_PATH : once set path success done, path =%d, \r\n", g_app_cntx[i].path);

            }

            break;
        }
        case BT_PBAPC_PULL_PHONEBOOK_CNF: {
            bt_pbapc_pull_phonebook_cnf_t *data = (bt_pbapc_pull_phonebook_cnf_t *)param;
            if (data->status == BT_STATUS_PBAPC_OK) {
                LOG_I(app_pbapc, "[APP_PBAPC]once time pull phonebook req done: con_id = %d\r\n", data->conn_id);

                app_pbapc_track_data(data->conn_id);
            } else {
                LOG_I(app_pbapc, "[PBAPC]Pullpb req done: it is not ok");
            }
            break;
        }

        case BT_PBAPC_PULL_VCARD_LISTING_CNF: {
            bt_pbapc_pull_vcard_list_cnf_t *data = (bt_pbapc_pull_vcard_list_cnf_t *)param;

            if (data->status == BT_STATUS_PBAPC_OK) {
                LOG_I(app_pbapc, "[APP_PBAPC]once time pull vcard listing req done: con_id = %d\r\n", data->conn_id);
                app_pbapc_track_data(data->conn_id);

            } else {
                LOG_I(app_pbapc, "[PBAPC]PullvCardList done: it is not ok");
            }

            break;
        }
        case BT_PBAPC_PULL_VCARD_ENTRY_CNF: {
            /*one read data done */
            bt_pbapc_pull_vcard_entry_cnf_t *data = (bt_pbapc_pull_vcard_entry_cnf_t *)param;

            if (data->status == BT_STATUS_PBAPC_OK) {

                LOG_I(app_pbapc, "[APP_PBAPC]once time pull vcard entry req done: con_id = %d\r\n", data->conn_id);

                app_pbapc_track_data(data->conn_id);
            } else {

                LOG_I(app_pbapc, "[PBAPC]PullvCardEntry done: it is ok\r\n");
            }
            break;
        }

        case BT_PBAPC_ABORT_CNF: {
            bt_pbapc_abort_cnf_t *data = (bt_pbapc_abort_cnf_t *)param;
            LOG_I(app_pbapc, "[PBAP]Abort cnf status = %d", data->status);
            break;
        }
        case BT_PBAPC_DISCONNECT_IND: {
            bt_pbapc_disconnect_ind_t *data = (bt_pbapc_disconnect_ind_t *)param;
            LOG_I(app_pbapc, "[APP_PBAPC]disconnect: disconnect_code = %d, conn_id = %d\r\n", data->status, data->conn_id);

            if (data->status == BT_STATUS_PBAPC_OK) {

                app_pbapc_reset_conn_id(data->conn_id);
            }

            break;
        }
        case BT_PBAPC_DATA_IND: {
            break;
        }
        default:
            break;
    }



}


static void bt_pbapc_save_data(const void *param)
{
    uint8_t i;
    uint32_t temp_len;
    bt_pbapc_data_ind_t *data = (bt_pbapc_data_ind_t *)param;

    i = app_pbapc_get_index_by_id(data->conn_id);

    LOG_I(app_pbapc, "[APP_PBAPC] save_data: i = %d, con_id = %d\r\n", i, data->conn_id);

    if (i >= MAX_EDR_RMT_DEV_NUM) {
        LOG_I(app_pbapc, "[APP_PBAPC] conn index is not found\r\n");
        return;
    }

    LOG_I(app_pbapc, "[[APP_PBAPC] save_data: data_buffer = %x \r\n", g_app_cntx[i].data);
    if (!g_app_cntx[i].data) {
#ifndef WIN32
        g_app_cntx[i].data = (uint8_t *)pvPortMalloc(MAX_PBAPC_READ_COUNT * 300);

#else
        g_app_cntx[i].data = (uint8_t *)malloc(MAX_PBAPC_READ_COUNT * 300);
#endif
    }
    temp_len = g_app_cntx[i].offset + data->data_len;
    LOG_I(app_pbapc, "[[APP_PBAPC] save_data: len = %d,offset = %d\r\n", temp_len, g_app_cntx[i].offset);
    if (temp_len < 6000) {
        memcpy(g_app_cntx[i].data + g_app_cntx[i].offset, data->data_buffer, data->data_len);
        g_app_cntx[i].offset += data->data_len;
    }
}


void bt_pbapc_common_callback(bt_event_t event_id, const void *param)
{
    app_pbapc_queue_struct_t queue_item;

    LOG_I(app_pbapc, "[APP_PBAPC]bt_pbapc_common_callback: event_id = %x\n", event_id);
    switch (event_id) {
        case BT_PBAPC_CONNECT_CNF: {

            memcpy((void *)queue_item.param, (void *)param, sizeof(bt_pbapc_connect_cnf_t));
            break;
        }
        case BT_PBAPC_AUTH_RESPONSE_CNF: {

            memcpy((void *)queue_item.param, (void *)param, sizeof(bt_pbapc_auth_response_cnf_t));
            break;
        }

        case BT_PBAPC_AUTH_IND: {
            memcpy((void *)queue_item.param, (void *)param, sizeof(bt_pbapc_auth_challenge_ind_t));
            break;
        }
        case BT_PBAPC_SET_PHONEBOOK_CNF: {
            memcpy((void *)queue_item.param, (void *)param, sizeof(bt_pbapc_set_phonebook_cnf_t));

            break;
        }
        case BT_PBAPC_PULL_PHONEBOOK_CNF: {
            memcpy((void *)queue_item.param, (void *)param, sizeof(bt_pbapc_pull_phonebook_cnf_t));

            break;
        }

        case BT_PBAPC_PULL_VCARD_LISTING_CNF: {

            memcpy((void *)queue_item.param, (void *)param, sizeof(bt_pbapc_pull_vcard_list_cnf_t));

            break;
        }
        case BT_PBAPC_PULL_VCARD_ENTRY_CNF: {

            /*one read data done */
            memcpy((void *)queue_item.param, (void *)param, sizeof(bt_pbapc_pull_vcard_entry_cnf_t));

            break;
        }

        case BT_PBAPC_ABORT_CNF: {

            break;
        }
        case BT_PBAPC_DISCONNECT_IND: {


            memcpy((void *)queue_item.param, (void *)param, sizeof(bt_pbapc_disconnect_ind_t));
            break;
        }
        case BT_PBAPC_DATA_IND: {
            bt_pbapc_data_ind_t *data = NULL;
            memcpy((void *)queue_item.param, (void *)param, sizeof(bt_pbapc_data_ind_t));

            data = (bt_pbapc_data_ind_t *)param;
            LOG_I(app_pbapc, "[APP_PBAPC] before--buffer =  %s, len = %d", data->data_buffer, data->data_len);
            data = (bt_pbapc_data_ind_t *)queue_item.param;

            bt_pbapc_save_data(param);
            break;
        }
        default:
            break;
    }

    queue_item.event_id = event_id;

    xQueueSend(app_queue, (void *)&queue_item, 0);
}


static void app_pbapc_reset_conn_id(uint16_t conn_id)
{
    uint8_t i;

    i = app_pbapc_get_index_by_id(conn_id);

    if (i < MAX_EDR_RMT_DEV_NUM) {

        memset(g_app_cntx + i, 0 , sizeof (app_pbapc_cntx_t));
        g_app_cntx[i].conn_id = 0xFF;

    }
}


static uint8_t app_pbapc_get_index_by_id(uint16_t conn_id)
{
    uint8_t i;
    for (i = 0 ; i < MAX_EDR_RMT_DEV_NUM; i++) {
        if (g_app_cntx[i].flag) {
            if (g_app_cntx[i].conn_id == conn_id) {
                break;
            }
        }
    }
    return i;
}

static uint16_t app_asc_str_to_ucs2_str(int8_t *pOutBuffer, int8_t *pInBuffer, uint8_t out_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int16_t count = -1;
    uint8_t charLen = 0;
    uint8_t arrOut[2];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (*pInBuffer != '\0' && count < out_len - 1 ) {

        app_unicode_to_ucs2encoding((uint16_t) (*((uint8_t *)pInBuffer)), &charLen, arrOut);

        pOutBuffer[++count] = arrOut[0];
        pOutBuffer[++count] = arrOut[1];
        pInBuffer++;
    }

    pOutBuffer[++count] = '\0';
    pOutBuffer[++count] = '\0';
    return count + 1;
}


static uint8_t app_unicode_to_ucs2encoding(uint16_t unicode, uint8_t *charLength, uint8_t *arrOut)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint8_t status = 1;
    uint8_t index = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (arrOut != 0) {

        if (unicode < 256) {
            arrOut[index++] = *((uint8_t *) (&unicode));
            arrOut[index] = 0;

        } else {
            arrOut[index++] = *((uint8_t *) (&unicode));
            arrOut[index] = *(((uint8_t *) (&unicode)) + 1);

        }
        *charLength = 2;
    } else {

        status = 0;
    }
    return status;
}


void app_pbapc_test_init(void)
{
    uint8_t i;
    for (i = 0 ; i < MAX_EDR_RMT_DEV_NUM; i++) {
        memset(g_app_cntx + i, 0, sizeof(app_pbapc_cntx_t));
    }
}


void app_pbapc_test_task_init(void)
{
    TaskHandle_t xCreatedTask;
    LOG_I(app_pbapc, "[App PBAPC]create task!\r\n");
    xTaskCreate(app_pbapc_test_task, "App_pbapc_test_task", 2048, NULL, 1, &xCreatedTask);
    app_pbapc_test_init();
}
//#endif
