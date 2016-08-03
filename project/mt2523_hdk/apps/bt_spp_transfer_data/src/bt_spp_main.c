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

#include "bt_spp_main.h"
#include "bt_spp_server.h"
#include "bt_spp_client.h"
#include "bt_gap.h"

log_create_module(bt_spp_example, PRINT_LEVEL_INFO);

/*for spp server*/
QueueHandle_t spp_server_queue;
spp_message_t spp_server_message;

/*for spp client*/
QueueHandle_t spp_client_queue;
spp_message_t spp_client_message;


bt_status_t bt_gap_get_local_name(char* name)
{
    memcpy(name, "bt_spp_example_test", 19);
    return BT_STATUS_SUCCESS;
}

void bt_gap_common_callback(bt_event_t event_id, void *param)
{
   // LOG_I(bt_spp_example,"id: 0x%x", event_id);

    switch (event_id) {
        case BT_GAP_SET_SCAN_MODE_CNF:
            break;

        case BT_GAP_IO_CAPABILITY_REQUEST_IND:
/*just work pair*/
            bt_gap_reply_io_capability_request((bt_address_t *)param,
                BT_GAP_IO_NO_INPUT_NO_OUTPUT, true, BT_GAP_BONDING_MODE_DEDICATED,
                false, NULL, NULL);
/*user confirm pair*/            
           /* bt_gap_reply_io_capability_request((bt_address_t *)param,
                BT_GAP_IO_DISPLAY_YES_NO, true, BT_GAP_BONDING_MODE_DEDICATED,
                false, NULL, NULL);*/
            break;

        case BT_GAP_USER_CONFIRM_REQUEST_IND:
            bt_gap_reply_user_confirm_request((bt_address_t *)param, true); 
            break;
            
        case BT_GAP_POWER_ON_CNF: 
            LOG_I(bt_spp_example,"bt power on confirm.");
            bt_gap_set_scan_mode(BT_GAP_MODE_GENERAL_ACCESSIBLE);
            //spp server start
            spp_server_enable();
			//spp client start
            spp_client_send_connect_request();
            break;
            
        default:
            break;
    }
}

void spp_server_event_handler(bt_event_t event_id, void* parameter)
{
    LOG_I(bt_spp_example,"server event:%x, %x", event_id, parameter);

    switch (event_id) {
        case BT_SPP_CONNECT_IND:
        {
            bt_spp_connect_ind_t message;
            memcpy((void*)&message, parameter, sizeof(bt_spp_connect_ind_t));
            spp_server_handle_connect_ind(&message);
        } 
            break;
        case BT_SPP_CONNECT_CNF:
        {
            bt_spp_connect_cnf_t message;
            memcpy((void*)&message, parameter, sizeof(bt_spp_connect_cnf_t));
            spp_server_handle_connect_cnf(&message);
            if(message.result == BT_STATUS_SUCCESS) {
                spp_server_send_data();
            }
        }
            break;
        case BT_SPP_DISCONNECT_IND:
        {
            bt_spp_disconnect_ind_t message;
            memcpy((void*)&message, parameter, sizeof(bt_spp_disconnect_ind_t));
            spp_server_handle_disconnect_ind(&message);
            }
            break;  
        case BT_SPP_READY_TO_READ_IND:
        {
            bt_spp_ready_to_read_ind_t message;
            memcpy((void*)&message, parameter, sizeof(bt_spp_ready_to_read_ind_t));
            spp_server_handle_ready_to_read_ind(&message);
        }
            break;  
        case BT_SPP_READY_TO_WRITE_IND:
        {
            bt_spp_ready_to_write_ind_t message;
            memcpy((void*)&message, parameter, sizeof(bt_spp_ready_to_write_ind_t));
            spp_server_handle_ready_to_write_ind(&message);
        }
            break;   
        default:
            break;
    }
}

void spp_client_event_handler(bt_event_t event_id, void* parameter)
{

    LOG_I(bt_spp_example,"client event: %x, %x", event_id, parameter);

    switch(event_id) {
        case BT_SPP_CONNECT_CNF: 
        {
            bt_spp_connect_cnf_t message;
            memcpy((void*)&message, parameter, sizeof(bt_spp_connect_cnf_t));
            spp_client_handle_connect_cnf(&message);
            if(message.result == BT_STATUS_SUCCESS) {
                spp_client_send_data();
            }
        }   
            break;
        case BT_SPP_DISCONNECT_IND:
        {     
            bt_spp_disconnect_ind_t message;
            memcpy((void*)&message, parameter, sizeof(bt_spp_disconnect_ind_t));
            spp_client_handle_disconnect_ind(&message);
        }
            break;
         
        case BT_SPP_READY_TO_READ_IND:
        {
            bt_spp_ready_to_read_ind_t message;
            memcpy((void*)&message, parameter, sizeof(bt_spp_ready_to_read_ind_t));
            spp_client_handle_ready_to_read_ind(&message);
        }
            break;  
        case BT_SPP_READY_TO_WRITE_IND:
        {
            bt_spp_ready_to_write_ind_t message;
            memcpy((void*)&message, parameter, sizeof(bt_spp_ready_to_write_ind_t));
            spp_client_handle_ready_to_write_ind(&message);
        }
            break;   
        default:
            break;
    }
}

void spp_client_task(void *arg)
{
	spp_client_queue = xQueueCreate(3, sizeof(spp_message_t));
	if( spp_client_queue == NULL ) {
		LOG_I(bt_spp_example,"create client queue failed!");
        return;
	}
    
	while(1) {
        if(xQueueReceive(spp_client_queue, (void *)&spp_client_message, 0)) {
            LOG_I(bt_spp_example,"client message:%x,%x",spp_client_message.event_id, spp_client_message.param);
            spp_client_event_handler(spp_client_message.event_id, (void*) (spp_client_message.param));
            memset((void*) &spp_client_message, 0, sizeof(spp_message_t));
		}
	}
}

void spp_server_task(void *arg)
{
	spp_server_queue = xQueueCreate(3, sizeof(spp_message_t));
	if( spp_server_queue == NULL ) {
		LOG_I(bt_spp_example,"create server queue failed!");
        return;
	}
    
	while(1) {
        if(xQueueReceive(spp_server_queue, (void *)&spp_server_message, 0)) {
            LOG_I(bt_spp_example,"server message:%x,%x",spp_server_message.event_id, spp_server_message.param);
            spp_server_event_handler(spp_server_message.event_id, (void*) (spp_server_message.param));
            memset((void*) &spp_server_message, 0, sizeof(spp_message_t));
		}
	}
}

void bt_spp_main(void)
{
    TaskHandle_t xCreatedClientTask;
    TaskHandle_t xCreatedServerTask;
    
    LOG_I(bt_spp_example,"create spp app task!");
    xTaskCreate(spp_client_task, "spp_client_task", 256, NULL, 1, &xCreatedClientTask);
    xTaskCreate(spp_server_task, "spp_server_task", 256, NULL, 1, &xCreatedServerTask);
}

