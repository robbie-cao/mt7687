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

#include "FreeRTOS.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "fota_gprot.h"
#include "fota.h"
#include "hal_flash.h"
#include "hal_wdt.h"
#include "bt_notify.h"
#include "bt_events.h"
#include "bt_gap.h"
#include "queue.h"
#include "task.h"
#include "syslog.h"
#include "bt_events.h"
#include "bl_fota.h"
#include "ble_dogp_adp_service.h"
#include "memory_map.h"
#include "flashtool.h"
#include "all_in_one_da.h"
#include "GNSS_DL_api.h"
#include "timers.h"
#include "gnss_app.h"

//#define GNSS_TEST
//#ifdef GNSS_TEST
//#include "gnss_test_cmd.h"
//#endif

/*****************************************************************************
 * define
 *****************************************************************************/
#define FOTA_QUEUE_SIZE      500
#define UPDATE_INFO_SIZE     sizeof(bl_fota_update_info_t) //32

#define BT_NOTIFY_READ_IND   9527
#define FLASH_BLOCK_SIZE     4096
/*****************************************************************************
 * typedef
 *****************************************************************************/
typedef struct {
    bt_event_t event_id;
    void *data_ptr;
} fota_queue_struct_t;


/*****************************************************************************
 * static function
 *****************************************************************************/
static void fota_bt_evt_cb(void *data);
static void fota_recv_data_start(bt_noti_data_t *p_data);
static void fota_recv_data_pack(bt_noti_data_t *p_data);
static int16_t fota_recv_data_end();
static void fota_init_mem_info(fota_update_enum_t udpate_type);

void fota_btnotify_msg_relay(uint32_t event_id, void *parameter);
void fota_init();
void fota_btnotify_msg_hdlr(uint32_t event_id, void *parameter);


/*****************************************************************************
 * global variable
 *****************************************************************************/
/* mem info struct to store flash info, bt address to store remote bt address */
bt_address_t g_fota_bt_addr;
fota_mem_info_t g_fota_mem_info;
fota_mem_info_t gnss_fota_mem_info;
fota_update_enum_t update_type_enum;


/* fota queue to store msg from bt */
QueueHandle_t fota_queue = NULL;

/* flow control global variable */
#ifdef FOTA_FLOW_CONTROL
uint32_t ready_to_read_received;
uint32_t ready_to_read_handled;

uint32_t data_pack_handled;
#endif





log_create_module(fota_dl_m, PRINT_LEVEL_INFO);

/*****************************************************************************
 * utility
 *****************************************************************************/
/**
 * @brief             fota main task, receive event from bt task, store fireware to flash
 * @return
 */
void fota_task(void *arg)
{
    fota_queue_struct_t queue_data_item;
    LOG_I(fota_dl_m, "\n[FOTA_MAIN]fota task entry\n");

    fota_init();

//#ifdef GNSS_TEST
//    gnss_test_cmd_init();
    epo_download_init();
//#endif


    fota_queue = xQueueCreate(FOTA_QUEUE_SIZE, sizeof(fota_queue_struct_t));
    if ( fota_queue == NULL ) {
        LOG_E(fota_dl_m, "[FOTA_MAIN]create queue failed!\r\n");
        return;
    }

    //main loop
    while (1) {
        LOG_I(fota_dl_m, "\n[FOTA_MAIN]try to get one msg\n");
        if (xQueueReceive(fota_queue, (void *)&queue_data_item, portMAX_DELAY)) {
#ifdef FOTA_FLOW_CONTROL
            LOG_I(fota_dl_m, "\n[FOTA_MAIN]handle one msg, msg_id = %d\n", queue_data_item.event_id);
#endif
            fota_btnotify_msg_hdlr(queue_data_item.event_id, queue_data_item.data_ptr);
        }
    }
}

/**
 * @brief             reply error code to remote end via bt
 * @param[IN]    sender is the id of the entity which send the error code
 * @param[IN]    receiver is the id of the entity which receive the error code
 * @param[IN]    reply code is the exact error code would be sent
 * @return
 */
void fota_reply_int_value(char *sender, char *receiver, int32_t reply_code)
{
    char data[50] = {0};
    char error[5] = {0};
    LOG_I(fota_dl_m, "\n[FOTA_REPLY]fota_reply_int_value sender = %s, receiver = %s, reply_code = %d\n", sender, receiver, reply_code);
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
                strlen(error) + 1,
                error);
    }

    bt_notify_send_data(&g_fota_bt_addr, data, strlen(data), 1);
}

/**
 * @brief             reply error according to current udate type
 * @param[IN]    update_type is the type of update file current been transftered
 * @param[IN]    reply code is the exact error code would be sent
 * @return
 */
void fota_reply_int_value_ex(fota_update_enum_t update_type, int32_t reply_code)
{
    LOG_I(fota_dl_m, "\n[FOTA_REPLY_EX]update_type = %d, reply_code = %d\n", update_type, reply_code);
    if (FOTA_UPDATE_FBIN == update_type) {
        fota_reply_int_value(FOTA_EXTCMD_UPDATE_BIN_SENDER, FOTA_EXTCMD_UPDATE_BIN_RECEIVER, reply_code);
    } else if (FOTA_UPDATE_GNSS == update_type) {
        fota_reply_int_value(FOTA_EXTCMD_GNSS_UPDATE_SENDER, FOTA_EXTCMD_GNSS_UPDATE_RECEIVER, reply_code);
    } else {
        LOG_E(fota_dl_m, "[FOTA_REPLY_EX]no current update");
    }
}


/**
 * @brief             init specific memory info for different update type
 * @param[IN]    update_type is the type of update file current been transftered
 * @return
 */
static void fota_init_mem_info(fota_update_enum_t update_type)
{
    LOG_I(fota_dl_m, "\n[FOTA_INIT_MEM] init mem info, udpate_type = %d\n", update_type);
    if (FOTA_UPDATE_FBIN == update_type) {
        g_fota_mem_info.start_address = 0x00200000;  //0x08200000
        g_fota_mem_info.end_address = 0x003e0000;
        g_fota_mem_info.reserved_size = 1920 * 1024;
        g_fota_mem_info.block_count = 648;
        g_fota_mem_info.block_size = 4 * 1024;
        g_fota_mem_info.block_type = HAL_FLASH_BLOCK_4K;
        g_fota_mem_info.total_received = 0;
        g_fota_mem_info.write_ptr = 0;
        g_fota_mem_info.ubin_pack_count = 0;
    } else if (FOTA_UPDATE_GNSS == update_type) {
        gnss_fota_mem_info.start_address = 0x00200000;
        gnss_fota_mem_info.end_address = 0x003e0000;
        gnss_fota_mem_info.reserved_size = 1920 * 1024;
        gnss_fota_mem_info.block_count = 648;
        gnss_fota_mem_info.block_size = 4 * 1024;
        gnss_fota_mem_info.block_type = HAL_FLASH_BLOCK_4K;
        gnss_fota_mem_info.total_received = 0;
        gnss_fota_mem_info.write_ptr = 0;
        gnss_fota_mem_info.ubin_pack_count = 0;
    } else {
        LOG_E(fota_dl_m, "\n[FOTA_INIT_MEM] init mem info, param error\n");
    }
}

//for gnss dota
void fota_gnss_update()
{
    int ret;
    GNSS_DA gnss_da;
    GNSS_Image_List gnss_image;
    GNSS_Download_Arg arg;

    unsigned char checkch[16] = {'T', 'H', 'I', 'S', ' ', 'C', 'O', 'D', 'E', ' ', 'i', 's', ' ', 'O', 'K', '!'};
    unsigned char ptmp[16] = {0};

    gnss_da.m_image = MT3333_all_in_one_da;
    gnss_da.m_size = sizeof(MT3333_all_in_one_da);
    gnss_image.m_num = 1;
    gnss_image.m_image_list[0].m_image = (kal_uint8 *)gnss_fota_mem_info.start_address  + 0x08000000;
    gnss_image.m_image_list[0].m_size = gnss_fota_mem_info.total_received;
    memset(&arg, 0, sizeof(arg));

    memset(&arg, 0, sizeof(arg));
    memcpy(ptmp, (const void *)(gnss_fota_mem_info.start_address + 0x08000000 + 0x60), 16);
    if (memcmp(ptmp, checkch, 16) != 0) {
        LOG_I(fota_dl_m, "[GNSSCHECK]error gnss bin\n\n");
        fota_reply_int_value_ex(FOTA_UPDATE_FBIN, FOTA_UPDATE_COMMON_ERROR);
        return;
    } else {
        LOG_I(fota_dl_m, "[GNSSCHECK]correct gnss bin\n\n");
    }

    ret = FlashDownload(&gnss_da, &gnss_image, arg);
    if (FT_OK != ret) {
    } else {
        fota_reply_int_value_ex(FOTA_UPDATE_FBIN, FOTA_UPDATE_SUCCESS);
    }
}



/**
 * @brief             fota init operation
 * @return
 */

void fota_init()
{
    bt_notify_result_t reg_ret_1;
    bt_notify_result_t reg_ret_2;
    uint8_t bt_spp_uuid[16] = {0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFF};
    LOG_I(fota_dl_m, "\n[FOTA_INIT]fota init\n");

    //TimerHandle_t gnss_update_timer;
    //gnss_update_timer = xTimerCreate("GNSS_UPDATE_TIME", 1000, pdFALSE, 0, fota_gnss_update);
    //xTimerStart(gnss_update_timer, 0);

    bt_spp_start_service((const uint8_t*)bt_spp_uuid, (const int8_t*)"BT_NOTIFY", (bt_spp_common_callback)fota_btnotify_msg_relay);
    ble_dogp_adp_init((ble_dogp_adp_common_callback_t)fota_btnotify_msg_relay);/*register dogp callback.*/
    bt_notify_init();

    fota_init_mem_info(FOTA_UPDATE_FBIN);
    fota_init_mem_info(FOTA_UPDATE_GNSS);
    update_type_enum = FOTA_UPDATE_NONE;

    LOG_I(fota_dl_m, "\n[FOTA_INIT] begin register extend command callback hdlr\n");
    reg_ret_1 = bt_notify_register_callback(NULL, FOTA_EXTCMD_UPDATE_BIN_SENDER, fota_bt_evt_cb);
    reg_ret_2 = bt_notify_register_callback(NULL, FOTA_EXTCMD_GNSS_UPDATE_SENDER, fota_bt_evt_cb);
    LOG_I(fota_dl_m, "\n[FOTA_INIT] register result: %d, %d\n", reg_ret_1, reg_ret_2);
}


/**
 * @brief             callback function to handle bt event
 * @param[IN]    data is what passed to callack by bt
 * @return
 */

//extern int32_t fota_test_retval;
static void fota_bt_evt_cb(void *data)
{
    fota_mem_info_t *mem_info_ptr = NULL;
    bt_noti_callback_t *bt_cb_data = (bt_noti_callback_t *)data;

    bl_fota_update_info_t *update_info_ptr;
    unsigned char update_info[UPDATE_INFO_SIZE];
    bool updated = false;
    uint32_t update_result;
    hal_flash_status_t flash_op_result;
    LOG_I(fota_dl_m, "\n[FOTA_CB]callback triggered, evt_id = %d\n", bt_cb_data->evt_id);

    //if connection event ,store bt address here
    if (bt_cb_data->evt_id == BT_NOTIFY_EVENT_CONNECTION) {
        if (g_fota_bt_addr.address != 0) {
            LOG_I(fota_dl_m, "\n[FOTA_CB]evt: BT_NOTIFY_EVENT_CONNECTION\n");
            memcpy(g_fota_bt_addr.address, bt_cb_data->bt_addr.address, 6);
            LOG_I(fota_dl_m, "[FOTA_CB] remote bt address: %s\n", g_fota_bt_addr.address);
        }


        hal_flash_read(FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - BL_FOTA_UPDATE_INFO_RESERVE_SIZE - BL_BASE,
                       update_info, sizeof(bl_fota_update_info_t));
        for (int i = 0; i < UPDATE_INFO_SIZE; i++) {
            if (update_info[i] != 0xFF) {
                updated = true;
                break;
            }
        }
        if (updated == true) {
            update_info_ptr = (bl_fota_update_info_t *)update_info;
            update_result = update_info_ptr->m_error_code;
            if (update_result == BL_FOTA_ERROR_NONE) {
                fota_reply_int_value_ex(FOTA_UPDATE_FBIN, FOTA_UPDATE_SUCCESS);
            } else {
                fota_reply_int_value_ex(FOTA_UPDATE_FBIN, FOTA_UPDATE_COMMON_ERROR);
                LOG_E(fota_dl_m, "[FOTA_CB]fota update fail, error_code = %d\n\n", update_result);
            }
            flash_op_result = hal_flash_erase(FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - FLASH_BLOCK_SIZE - BL_BASE, g_fota_mem_info.block_type);
            if (flash_op_result != HAL_FLASH_STATUS_OK) {
                LOG_E(fota_dl_m, "[FOTA_CB]erase result fail, error_code = %d\n\n", flash_op_result);
            } else {
                LOG_I(fota_dl_m, "[FOTA_CB]erase result success, error_code\n\n");
            }
        }

        return;
    }

    //if data  event, handle here, don't enqueue
    else if ((bt_cb_data->evt_id == BT_NOTIFY_EVENT_DATA)) {
        if (bt_cb_data->noti_data.err_code == FOTA_FILE_DATA_BEGIN) {
            LOG_I(fota_dl_m, "\n[FOTA_CB]evt: pack transfer: fota_recv_data_start\n");
            fota_recv_data_start(&(bt_cb_data->noti_data));
            //fota_test_retval = 100;
            return;
        } else if (bt_cb_data->noti_data.err_code == FOTA_FILE_DATA_PACK) {
#ifdef FOTA_FLOW_CONTROL
            data_pack_handled++;
            LOG_I(fota_dl_m, "\n[FOTA_CB]pack_data_handled = %d\n", data_pack_handled);
#endif
            fota_recv_data_pack(&(bt_cb_data->noti_data));
        } else if (bt_cb_data->noti_data.err_code == FOTA_FILE_DATA_END) {
            LOG_I(fota_dl_m, "\n[FOTA_CB]end pack has come\n");
            if (fota_recv_data_end() == FOTA_PACK_END_CORRECT) {
                if (update_type_enum == FOTA_UPDATE_FBIN) {
                    //trigger update here
                    fota_ret_t err;
                    err = fota_trigger_update();
                    LOG_I(fota_dl_m, "\n[FOTA_CB] fota trigger update result is %d\n", err);

                    //reboot device
                    hal_wdt_status_t ret;
                    hal_wdt_config_t wdt_config;
                    wdt_config.mode = HAL_WDT_MODE_RESET;
                    wdt_config.seconds = 1;
                    hal_wdt_init(&wdt_config);
                    ret = hal_wdt_software_reset();
                    LOG_I(fota_dl_m, "\n[FOTA_CB] wdt software reset result is %d\n", ret);
                } else {
                    fota_gnss_update();
                }
            } else {
                //don't erase first, check flash write result.
                fota_init_mem_info(update_type_enum);
                LOG_E(fota_dl_m, "\n[FOTA_CB]data pack end error\n");
            }
        } else {
            LOG_E(fota_dl_m, "\n[FOTA_CB]error data pack type\n");
        }

        //fota_test_retval = 100;
        return;
    }
#ifdef FOTA_FLOW_CONTROL
    else if (bt_cb_data->evt_id == BT_NOTIFY_EVENT_READY_TO_READ) {
        ready_to_read_received++;
        LOG_I(fota_dl_m, "\n[FOTA_CB]ready_to_read_received = %d\n", ready_to_read_received);
        //bt_notify_read_data(&g_fota_bt_addr);
        fota_queue_struct_t queue_item;
        queue_item.event_id = (bt_event_t)BT_NOTIFY_READ_IND;
        queue_item.data_ptr = NULL;
        if (xQueueSend(fota_queue, (void *)&queue_item, 0) != pdPASS) {
            LOG_E(fota_dl_m, "\n[FOTA_SPECIAL_OP]QUEUE_FULL\n");
        }
    } else if (bt_cb_data->evt_id == BT_NOTIFY_EVENT_DISCONNECTION) {
        memset(g_fota_bt_addr.address, 0, 6);

        if (FOTA_UPDATE_FBIN == update_type_enum) {
            mem_info_ptr = &g_fota_mem_info;
        } else if (FOTA_UPDATE_GNSS == update_type_enum) {
            mem_info_ptr = &gnss_fota_mem_info;
        }

        if (mem_info_ptr != NULL) {
            if (mem_info_ptr->write_ptr >= mem_info_ptr->start_address
                    && mem_info_ptr->write_ptr < mem_info_ptr->end_address
                    && mem_info_ptr->ubin_pack_count != 0) {
                LOG_E(fota_dl_m, "\n[FOTA_DISCONNECT] BT disconnect during data transfer\n");
                fota_init_mem_info(update_type_enum);
            }
        }
    }
#endif
}

/**
 * @brief             handle the start pack of fota transfer
 * @param[IN]    noti_data is the ptr of data body
 * @return
 */
static void fota_recv_data_start(bt_noti_data_t *noti_data)
{
    fota_mem_info_t *mem_info_ptr = NULL;
    if (!strcmp(noti_data->sender_id, FOTA_EXTCMD_UPDATE_BIN_SENDER) &&
            !strcmp(noti_data->receiver_id, FOTA_EXTCMD_UPDATE_BIN_SENDER)) {
        LOG_I(fota_dl_m, "\n[FOTA_START]FOTA_UPDATE_FBIN\n");
        mem_info_ptr = &g_fota_mem_info;
        update_type_enum = FOTA_UPDATE_FBIN;
    } else if (!strcmp(noti_data->sender_id, FOTA_EXTCMD_GNSS_UPDATE_SENDER) &&
               !strcmp(noti_data->receiver_id, FOTA_EXTCMD_GNSS_UPDATE_SENDER)) {
        LOG_I(fota_dl_m, "\n[FOTA_START]FOTA_UPDATE_GNSS\n");
        mem_info_ptr = &gnss_fota_mem_info;
        update_type_enum = FOTA_UPDATE_GNSS;
    } else {
        LOG_E(fota_dl_m, "\n[FOTA_START]error_sender = %d or error_receiver = %d\n", noti_data->sender_id, noti_data->receiver_id);
    }

    if (mem_info_ptr != NULL) {
        mem_info_ptr->write_ptr = mem_info_ptr->start_address;
        mem_info_ptr->ubin_pack_count = atoi((const char*)noti_data->data);
        LOG_I(fota_dl_m, "\n[FOTA_START]write_ptr = %x, data_pack_count = %d\n",
              mem_info_ptr->write_ptr, mem_info_ptr->ubin_pack_count);
#ifdef FOTA_FLOW_CONTROL
        ready_to_read_received = 0;
        ready_to_read_handled = 0;

        data_pack_handled = 0;
#endif
    }
}

/**
 * @brief             handle the pure data pack of fota transfer
 * @param[IN]    noti_data is the ptr of data body
 * @return
 */
static void fota_recv_data_pack(bt_noti_data_t *noti_data)
{
    int32_t ret;
    fota_mem_info_t *mem_info_ptr = NULL;

    if (update_type_enum == FOTA_UPDATE_FBIN) {
        mem_info_ptr = &g_fota_mem_info;
    } else if (update_type_enum == FOTA_UPDATE_GNSS) {
        mem_info_ptr = &gnss_fota_mem_info;
    } else {
        LOG_E(fota_dl_m, "\n[FOTA_TRANSFER] end error, not start yet\n");
    }

    LOG_I(fota_dl_m, "\n[FOTA_TRANSFER] write_ptr = 0x%x, start_addr = 0x%x, end_addr = 0x%x\n", mem_info_ptr->write_ptr, mem_info_ptr->start_address, mem_info_ptr->end_address);

    if (mem_info_ptr->write_ptr >= mem_info_ptr->start_address
            && mem_info_ptr->write_ptr < mem_info_ptr->end_address) {
        if (mem_info_ptr->write_ptr + noti_data->len < mem_info_ptr->end_address) {
            if (!(mem_info_ptr->write_ptr % (1 << 12))) {
                ret = hal_flash_erase(mem_info_ptr->write_ptr, mem_info_ptr->block_type);
                LOG_E(fota_dl_m, "\n[FOTA_TRANSFER] erase flash, ret = %d, address = 0x%x\n", ret, mem_info_ptr->write_ptr);
            }
            ret = hal_flash_write(mem_info_ptr->write_ptr, noti_data->data, noti_data->len);
            if (HAL_FLASH_STATUS_OK == ret) {
                mem_info_ptr->ubin_pack_count--;
                mem_info_ptr->write_ptr = mem_info_ptr->write_ptr + noti_data->len;
                mem_info_ptr->total_received = mem_info_ptr->total_received + noti_data->len;
                LOG_E(fota_dl_m, "\n[FOTA_TRANSFER] current received = %d\n", noti_data->len);
                LOG_E(fota_dl_m, "\n[FOTA_TRANSFER] total received = %d\n", mem_info_ptr->total_received);
                LOG_E(fota_dl_m, "\n[FOTA_TRANSFER] pack left = %d\n", mem_info_ptr->ubin_pack_count);
            } else {
                LOG_E(fota_dl_m, "\n[FOTA_TRANSFER] flash write error %d\n", ret);
                fota_reply_int_value_ex(update_type_enum, FOTA_WRITE_FLASH_FAIL);
                fota_init_mem_info(update_type_enum);
            }
        } else {
            LOG_E(fota_dl_m, "\n[FOTA_TRANSFER] file over reserved size\n");
            fota_reply_int_value_ex(update_type_enum, FOTA_UBIN_OVERSIZE);
            fota_init_mem_info(update_type_enum);
        }
    }
}

/**
 * @brief             handle the end pack of fota transfer
 * @return
 * #FOTA_PACK_END_CORRECT   data transfer end correct
 * #FOTA_PACK_END_WRONG      data transfer end wrong
 */
static int16_t fota_recv_data_end()
{
    fota_mem_info_t *mem_info_ptr = NULL;
    if (update_type_enum == FOTA_UPDATE_FBIN) {
        mem_info_ptr = &g_fota_mem_info;
    } else if (update_type_enum == FOTA_UPDATE_GNSS) {
        mem_info_ptr = &gnss_fota_mem_info;
    } else {
        LOG_E(fota_dl_m, "\n[FOTA_END]end error, not start yet\n");
    }

    if (mem_info_ptr != NULL) {
        if (mem_info_ptr->write_ptr >= mem_info_ptr->start_address
                && mem_info_ptr->write_ptr < mem_info_ptr->end_address) {
            mem_info_ptr->write_ptr = 0;
            if (mem_info_ptr->ubin_pack_count == 0) {
                LOG_I(fota_dl_m, "\n[FOTA_END]end correct\n");
                fota_reply_int_value_ex(update_type_enum, FOTA_PACK_END_CORRECT);
                return FOTA_PACK_END_CORRECT;
            } else {
                LOG_E(fota_dl_m, "\n[FOTA_END]end wrong, end pack come early\n");
                fota_reply_int_value_ex(update_type_enum, FOTA_PACK_END_WRONG);
                return FOTA_PACK_END_WRONG;
            }
        } else {
            LOG_E(fota_dl_m, "\n[FOTA_END]end wrong, write pointer wrong\n");
            return FOTA_PACK_END_WRONG;
        }
    }
    
    LOG_E(fota_dl_m, "\n[FOTA_END]mem_info_ptr is null\n");
    return FOTA_PACK_END_WRONG;
}



static bt_gap_bond_result_ind_t s_edr_keys[5];//temp solution

/**
 * @brief             general callback of gap to handle bt event
 * @param[IN]    event id is the exact event to be handled
 * @param[IN]    param is the data pointer of event
 * @return
 */
void fota_bt_gap_common_callback(bt_event_t event_id, void *param)
{
    LOG_I(fota_dl_m, "[FOTA_COMMON_CB]bt_gap_common_callback--id: 0x%x, %d\n", event_id, event_id);

    switch (event_id) {
        case BT_GAP_SET_SCAN_MODE_CNF: {
            break;
        }

        case BT_GAP_IO_CAPABILITY_REQUEST_IND: {
            bt_gap_reply_io_capability_request((bt_address_t *)param,
                                               BT_GAP_IO_NO_INPUT_NO_OUTPUT, true, BT_GAP_BONDING_MODE_DEDICATED,
                                               false, NULL, NULL);

            break;
        }

        case BT_GAP_USER_CONFIRM_REQUEST_IND: {
            bt_gap_reply_user_confirm_request((bt_address_t *)param, true);
            break;
        }

        case BT_GAP_DELETE_LINK_KEY_IND:
        {
            uint32_t i;
            for (i = 0; i < 5; i++) {
                    if ((s_edr_keys[i].result == 1) && (memcpy(s_edr_keys[i].dev_addr.address , ((bt_address_t *)param)->address, sizeof(bt_address_t))== 0)) {
                        memset(&s_edr_keys[i],0,sizeof(bt_gap_bond_result_ind_t));
                    }
                }
            break;
        }
        case BT_GAP_AUTHENTICATE_RESULT_IND:
        {
            const bt_gap_authenticate_result_ind_t* authen_result;
            uint32_t i;
            authen_result = (const bt_gap_authenticate_result_ind_t*)param;

            if (authen_result->result != BT_STATUS_SUCCESS) {
                for (i = 0; i < 5; i++) {
                    if ((s_edr_keys[i].result == 1) && (memcpy(s_edr_keys[i].dev_addr.address , authen_result->dev_addr.address, sizeof(bt_address_t))== 0)) {
                        memset(&s_edr_keys[i],0,sizeof(bt_gap_bond_result_ind_t));
                    }
                }
            }
            break;
        }

        default:
            break;
    }
}


bt_status_t fota_bt_gap_request_link_key(const bt_address_t *remote_addr, uint8_t *key, uint32_t key_len)
{
    uint32_t i;

    if (key_len >= BT_GAP_LINK_KEY_LEN) {
        for (i = 0; i < 5; i++) {
            if ((s_edr_keys[i].result == 1)&&(memcpy(s_edr_keys[i].dev_addr.address , remote_addr->address, sizeof(bt_address_t))== 0)) {
                memcpy(key, s_edr_keys[i].link_key, BT_GAP_LINK_KEY_LEN);
                return BT_STATUS_SUCCESS;
            }
        }
    }

    return BT_STATUS_PIN_OR_KEY_MISSING;
}



//btnotify relay
void fota_btnotify_msg_relay(uint32_t event_id, void *parameter)
{
    fota_queue_struct_t queue_item;
    queue_item.event_id = (bt_event_t)event_id;
    queue_item.data_ptr = NULL;

    switch (event_id) {
        case BT_SPP_CONNECT_CNF: {
            queue_item.data_ptr = (bt_spp_connect_cnf_t *)pvPortMalloc(sizeof(bt_spp_connect_cnf_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_connect_cnf_t));
            break;
        }
        case BT_SPP_CONNECT_IND: {
            queue_item.data_ptr = (bt_spp_connect_ind_t *)pvPortMalloc(sizeof(bt_spp_connect_ind_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_connect_ind_t));
            break;
        }
        case BT_SPP_DISCONNECT_IND: {
            queue_item.data_ptr = (bt_spp_disconnect_ind_t *)pvPortMalloc(sizeof(bt_spp_disconnect_ind_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_disconnect_ind_t));
            break;
        }
        case BT_SPP_READY_TO_READ_IND: {
            queue_item.data_ptr = (bt_spp_ready_to_read_ind_t *)pvPortMalloc(sizeof(bt_spp_ready_to_read_ind_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_ready_to_read_ind_t));
            break;
        }
        case BT_SPP_READY_TO_WRITE_IND: {
            queue_item.data_ptr = (bt_spp_ready_to_write_ind_t *)pvPortMalloc(sizeof(bt_spp_ready_to_write_ind_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(bt_spp_ready_to_write_ind_t));
            break;
        }

        case BLE_DOGP_ADP_EVENT_CONNECT_IND: {
            queue_item.data_ptr = (ble_dogp_adp_connect_t *)pvPortMalloc(sizeof(ble_dogp_adp_connect_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(ble_dogp_adp_connect_t));
            break;
        }
        case BLE_DOGP_ADP_EVENT_DISCONNECT_IND: {
            queue_item.data_ptr = (ble_dogp_adp_disconnect_t *)pvPortMalloc(sizeof(ble_dogp_adp_disconnect_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(ble_dogp_adp_disconnect_t));
            break;
        }
        case BLE_DOGP_ADP_EVENT_READY_TO_READ_IND: {
            queue_item.data_ptr = (ble_dogp_adp_ready_to_read_t *)pvPortMalloc(sizeof(ble_dogp_adp_ready_to_read_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(ble_dogp_adp_ready_to_read_t));
            break;
        }
        case BLE_DOGP_ADP_EVENT_READY_TO_WRITE_IND: {
            queue_item.data_ptr = (ble_dogp_adp_ready_to_write_t *)pvPortMalloc(sizeof(ble_dogp_adp_ready_to_write_t));
            memcpy(queue_item.data_ptr, parameter, sizeof(ble_dogp_adp_ready_to_write_t));
            break;
        }
        default:
            break;
    }

    if (xQueueSend(fota_queue, (void *)&queue_item, 0) != pdPASS) {
        LOG_E(fota_dl_m, "\n[FOTA_CB]QUEUE_FULL\n");
    }
}

void fota_btnotify_msg_hdlr(uint32_t event_id, void *parameter)
{

    switch (event_id) {
        case BT_SPP_CONNECT_CNF: {
            bt_notify_handle_connect_ind(parameter, BT_NOTIFY_DATA_SOURCE_SPP);
            vPortFree(parameter);
            break;
        }
        case BT_SPP_CONNECT_IND: {
            bt_spp_connect_ind_t *para = (bt_spp_connect_ind_t *)parameter;
            bt_notify_spp_connect_ind(para);
            vPortFree(parameter);
            break;
        }
        case BT_SPP_DISCONNECT_IND: {
            bt_notify_handle_disconnect_ind(parameter, BT_NOTIFY_DATA_SOURCE_SPP);
            vPortFree(parameter);
            break;
        }
        case BT_SPP_READY_TO_READ_IND: {
            bt_notify_handle_ready_to_read_ind(parameter, BT_NOTIFY_DATA_SOURCE_SPP);
            vPortFree(parameter);
            break;
        }
        case BT_SPP_READY_TO_WRITE_IND: {
            bt_notify_handle_ready_to_write_ind(parameter, BT_NOTIFY_DATA_SOURCE_SPP);
            vPortFree(parameter);
            break;
        }

        case BLE_DOGP_ADP_EVENT_CONNECT_IND: {
            bt_notify_handle_connect_ind(parameter, BT_NOTIFY_DATA_SOURCE_DOGP);
            vPortFree(parameter);
            break;
        }
        case BLE_DOGP_ADP_EVENT_DISCONNECT_IND: {
            bt_notify_handle_disconnect_ind(parameter, BT_NOTIFY_DATA_SOURCE_DOGP);
            vPortFree(parameter);
            break;
        }
        case BLE_DOGP_ADP_EVENT_READY_TO_READ_IND: {
            bt_notify_handle_ready_to_read_ind(parameter, BT_NOTIFY_DATA_SOURCE_DOGP);
            vPortFree(parameter);
            break;
        }
        case BLE_DOGP_ADP_EVENT_READY_TO_WRITE_IND: {
            bt_notify_handle_ready_to_write_ind(parameter, BT_NOTIFY_DATA_SOURCE_DOGP);
            vPortFree(parameter);
            break;
        }
        case BT_NOTIFY_READ_IND: {
            bt_notify_read_data(&g_fota_bt_addr);
            vPortFree(parameter);
            break;
        }
        default:
            break;
    }

}


