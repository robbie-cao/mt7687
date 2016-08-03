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

#include <stdio.h>
#include <string.h>

#include "bt_spp_main.h"
#include "bt_spp_server.h"

bt_spp_server_cntx spp_server_cntx;
bt_spp_server_cntx *spp_server_cntx_p = &spp_server_cntx;

void spp_server_common_callback(bt_event_t event_id, const void* parameter)
{
    LOG_I(bt_spp_example, "server common callback :%x", event_id);
    spp_message_t message;

    switch (event_id) {
        case BT_SPP_CONNECT_IND:
             memcpy((void*) message.param,(void*)parameter, sizeof(bt_spp_connect_ind_t));
            break;
        case BT_SPP_CONNECT_CNF:
            memcpy((void*) message.param,(void*)parameter, sizeof(bt_spp_connect_ind_t));
            break;
        case BT_SPP_DISCONNECT_IND:
            memcpy((void*) message.param,(void*)parameter, sizeof(bt_spp_disconnect_ind_t));
            break;  
        case BT_SPP_READY_TO_READ_IND:
            memcpy((void*) message.param,(void*)parameter, sizeof(bt_spp_ready_to_read_ind_t));
            break;  
        case BT_SPP_READY_TO_WRITE_IND:
             memcpy((void*) message.param,(void*)parameter, sizeof(bt_spp_ready_to_write_ind_t));
            break;   
        default:
            break;
    }

    /*send data from bt task to server app task, we must handle data in the app task.*/
    message.event_id = event_id;
    xQueueSend(spp_server_queue, (void*)&message, 0);
}

void spp_server_handle_connect_ind(bt_spp_connect_ind_t* parameter)
{
    int32_t result;

    LOG_I(bt_spp_example,"server connect ind");
    if (parameter == NULL) {
        return;
    } 
    
    result = bt_spp_connect_response(
			      parameter->port, 
			      true, 
			      spp_server_cntx_p->spp_buffer_for_profile, 
			      SPP_SERVER_TX_BUFF_FOR_PROFILE, 
			      spp_server_cntx_p->spp_buffer_for_profile + SPP_SERVER_TX_BUFF_FOR_PROFILE, 
			      SPP_SERVER_RX_BUFF_FOR_PROFILE
			        );
    LOG_I(bt_spp_example,"server responese result: %x",result);
    
	return;
}

void spp_server_handle_connect_cnf(bt_spp_connect_cnf_t* parameter)
{
    LOG_I(bt_spp_example,"server connect cnfirm.");
    
    if(parameter == NULL) {
        return;
    }

	LOG_I(bt_spp_example,"server connecte result:%x", parameter->result);

    if(parameter->result == BT_STATUS_SUCCESS) {
        spp_server_cntx_p->is_connected = true;
    } else {  }
    return;
}

void spp_server_handle_ready_to_read_ind(bt_spp_ready_to_read_ind_t* parameter)
{
    int32_t real_read_len = 0;
    LOG_I(bt_spp_example,"server ready to read data.");

	if(spp_server_cntx_p->is_connected) {
        do {
            if (spp_server_cntx_p->server_received_data_len >= SPP_SERVER_RECEIVED_DATA_BUFF) {
                LOG_I(bt_spp_example,"received data buffer is full!");
                spp_server_disconnect_request();
                return;
            } else {
                real_read_len = bt_spp_read(spp_server_cntx_p->port_id,
                                            spp_server_cntx_p->server_receive_data_buf + spp_server_cntx_p->server_received_data_len,
                                            SPP_SERVER_RECEIVED_DATA_BUFF);
                spp_server_cntx_p->server_received_data_len += real_read_len;
                LOG_I(bt_spp_example,"read len: %d, total read len: %ld", real_read_len , spp_server_cntx_p->server_received_data_len);
            }    
        } while(real_read_len > 0);

	{ 
        uint16_t i = 0;
        LOG_I(bt_spp_example,"print server recevied data start:");
        for (i = 0; i < spp_server_cntx_p->server_received_data_len; i++) {
		    LOG_I(bt_spp_example, "%c", spp_server_cntx_p->server_receive_data_buf[i]);
		}
		LOG_I(bt_spp_example,"print server recevied end!!!");
	}		
    } else {
        LOG_I(bt_spp_example,"read failed, server is not connected.");
    }

}

void spp_server_send_data(void)
{
    int32_t real_write_len = 0;
    LOG_I(bt_spp_example,"server send data.");

    if (spp_server_cntx_p->is_connected) {
        memcpy(spp_server_cntx_p->server_send_data_buffer, (uint8_t*)SPP_SERVER_STRING, strlen(SPP_SERVER_STRING));
        spp_server_cntx_p->server_remain_data_len = strlen(SPP_SERVER_STRING);

        do {
             if (spp_server_cntx_p->server_remain_data_len <= 0) {
                 LOG_I(bt_spp_example,"all data has been sent!");
                 //spp_server_disconnect_request();
                 return;
             } else {
                 real_write_len = bt_spp_write(spp_server_cntx_p->port_id, 
                                               (void*)(spp_server_cntx_p->server_send_data_buffer +
                                                strlen(SPP_SERVER_STRING) - spp_server_cntx_p->server_remain_data_len), 
                                                spp_server_cntx_p->server_remain_data_len);
                 spp_server_cntx_p->server_remain_data_len = spp_server_cntx_p->server_remain_data_len - real_write_len;
                 LOG_I(bt_spp_example,"write len: %ld, remain len: %ld", real_write_len, spp_server_cntx_p->server_remain_data_len);
             }
        } while (real_write_len > 0);
    } else {
        LOG_I(bt_spp_example,"data send failed, server is not connected.");
    }
}

void spp_server_handle_ready_to_write_ind(bt_spp_ready_to_write_ind_t* parameter)
{
    int32_t real_write_len = 0;
    LOG_I(bt_spp_example,"server ready to write data.");

    if (spp_server_cntx_p->is_connected) {
        do {
             if (spp_server_cntx_p->server_remain_data_len <= 0) {
                 LOG_I(bt_spp_example,"all data has been sent!");
                 //spp_server_disconnect_request();
                 return;
             } else {
                 real_write_len = bt_spp_write(spp_server_cntx_p->port_id, 
                                               (void*)(spp_server_cntx_p->server_send_data_buffer +
                                                strlen(SPP_SERVER_STRING) - spp_server_cntx_p->server_remain_data_len),
                                                spp_server_cntx_p->server_remain_data_len);
                 spp_server_cntx_p->server_remain_data_len = spp_server_cntx_p->server_remain_data_len - real_write_len;
                 LOG_I(bt_spp_example, "write len: %ld, remain len: %ld", real_write_len, spp_server_cntx_p->server_remain_data_len);
             }
        } while (real_write_len > 0);
    } else {
        LOG_I(bt_spp_example,"send failed, server is not connected.");
    }
}

void spp_server_handle_disconnect_ind(bt_spp_disconnect_ind_t* parameter)
{

    LOG_I(bt_spp_example,"server connect cnf.");

    if(parameter == NULL) {
        return;
    }	
	
	LOG_I(bt_spp_example,"server connecte result:(%x)", parameter->result);

    if(parameter->result == BT_STATUS_SUCCESS) {
        spp_server_cntx_p->is_connected = false;
        memset(spp_server_cntx_p->server_send_data_buffer, 0x0, SPP_SERVER_SEND_DATA_BUFF);
        spp_server_cntx_p->server_remain_data_len = 0;
        memset(spp_server_cntx_p->server_receive_data_buf, 0x0, SPP_SERVER_RECEIVED_DATA_BUFF);
        spp_server_cntx_p->server_received_data_len = 0;
    } else{ }

   spp_server_disable();
   return;
}

void spp_server_disconnect_request(void)
{
    int32_t result = bt_spp_disconnect(spp_server_cntx_p->port_id);
    LOG_I(bt_spp_example,"server send disconnect, port:%d, result:%x", spp_server_cntx_p->port_id, result);
    return;
}

void spp_server_enable(void)
{
    int32_t result = bt_spp_start_service(NULL, NULL, spp_server_common_callback);

    LOG_I(bt_spp_example,"server enable: result(%ld)", result);
	
    if(result > 0) {
        spp_server_cntx_p->port_id = result;
        memset(spp_server_cntx_p->server_send_data_buffer, 0x0, SPP_SERVER_SEND_DATA_BUFF);
        spp_server_cntx_p->server_remain_data_len = SPP_SERVER_SEND_DATA_BUFF;
        memset(spp_server_cntx_p->server_receive_data_buf, 0x0, SPP_SERVER_RECEIVED_DATA_BUFF);
        spp_server_cntx_p->server_received_data_len = 0;
    } else { }
    return;
}

void spp_server_disable(void)
{
    int32_t result = bt_spp_stop_service(spp_server_cntx_p->port_id);

    LOG_I(bt_spp_example,"server disable: port(%d),result(%ld)", spp_server_cntx_p->port_id, result);
    
    if(result == BT_STATUS_SUCCESS) {
        spp_server_cntx_p->port_id = 0;
    }
    
    return;
}

