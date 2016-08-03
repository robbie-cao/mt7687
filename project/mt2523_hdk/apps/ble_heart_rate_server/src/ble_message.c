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

/*****************************************************************************
 *
 * Description:
 * ------------
 * This file implements BLE app messsage and funtion
 *
 ****************************************************************************/
#include "FreeRTOS.h"
#include "ble_message.h"
#include "queue.h"
#include "projdefs.h"
#include <assert.h>
#include "syslog.h"

/********************************************************
 * Global varaible
 *
 ********************************************************/
static QueueHandle_t g_ble_app_queue;
static ble_message_context_struct g_ble_app_message_queue[BLE_MESSAGE_MAX_COUNT];
static uint8_t g_ble_app_message_queue_circle_read = 0;
static uint8_t g_ble_app_message_queue_circle_write = 0;

log_create_module(BLE_MESSAGE, PRINT_LEVEL_INFO);

/********************************************************
 * External Function
 *
 ********************************************************/
void ble_message_create_queue(void)
{
    uint8_t item_size = sizeof(ble_message_context_struct);
    g_ble_app_queue = xQueueCreate(BLE_MESSAGE_MAX_COUNT, item_size);
}

const ble_message_context_struct *ble_message_allocate_message_buffer(void)
{
    if ((g_ble_app_message_queue_circle_write < g_ble_app_message_queue_circle_read) && 
        (g_ble_app_message_queue_circle_write + 1 == g_ble_app_message_queue_circle_read)) {
        assert(0);
    }
    LOG_I(BLE_MESSAGE, "Alloc message = %d \n", g_ble_app_message_queue_circle_write);
    return &g_ble_app_message_queue[g_ble_app_message_queue_circle_write++];
}

void ble_message_free_message_buffer(ble_message_context_struct *msg_buffer)
{
    if (g_ble_app_message_queue_circle_read != g_ble_app_message_queue_circle_write) {
        /*g_ble_app_message_queue_circle_read++;*/
    }
    LOG_I(BLE_MESSAGE, "Free message = %d \n", g_ble_app_message_queue_circle_read);
}

void ble_message_send(uint16_t event, void *parameter)
{
    ble_message_context_struct message_buffer;
    /*message_buffer = ble_message_allocate_message_buffer();*/

    message_buffer.message_id = event;
    message_buffer.parameter = parameter;

    /*if can't allocate memroy, just assert*/
    /*1 measns pdPass*/
    if (xQueueSend(g_ble_app_queue,&message_buffer,portMAX_DELAY) != pdPASS) {
        assert(0);
    }
    LOG_I(BLE_MESSAGE, "Send message, r = %d, w = %d \n", g_ble_app_message_queue_circle_read, g_ble_app_message_queue_circle_write);
}

bool ble_message_receive(ble_message_context_struct *message)
{
    ble_message_context_struct temp_msg;
    bool result = false;
    
    if (g_ble_app_queue) {
        if (xQueueReceive(g_ble_app_queue, &temp_msg, portMAX_DELAY)) {
            *message = temp_msg;
            LOG_I(BLE_MESSAGE, "recieve mesasge id = %d \n", message->message_id);
            LOG_I(BLE_MESSAGE, "Recive message r = %d, w = %d \n", g_ble_app_message_queue_circle_read, g_ble_app_message_queue_circle_write);
  
            result = true;
        }     
    }
    return result;
}




