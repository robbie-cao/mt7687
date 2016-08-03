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

// For Register AT command handler
// System head file

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "at_command.h"
#include "bma2x2.h"
#include "syslog.h"
#include "hal_gpio.h"

#if defined(__GNUC__)

log_create_module(atci_gsensor, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(atci_gsensor, "[bma255]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci_gsensor, "[bma255]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci_gsensor ,"[bma255]"fmt,##arg)

#define VENDOR_ID 0xFA

/*
 * sample code
*/

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_gsensor(atci_parse_cmd_param_t *parse_cmd);


// AT command handler
atci_status_t atci_cmd_hdlr_gsensor(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    int  param1_val = -1;
    
    LOGI("atci_cmd_hdlr_keypad \r\n");

    memset(&resonse, 0, sizeof(atci_response_t));    
    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+EKEYURC=?
            strcpy((char *)resonse.response_buf, "+EGSENSOR=(0,1)\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EKEYURC=<op>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            param = strtok(parse_cmd->string_ptr, ",\n\r");
            param = strtok(parse_cmd->string_ptr, "AT+EGSENSOR=");
            param1_val = atoi(param);
            
            if (param && (param1_val == 0)) {
                uint8_t data;
                /* valid parameter, update the data and return "OK"
                                param1_val = 0 :  I2C operation.
                */
                
                /* initialize vsm */
                bma255_init();
                /*get chip id*/
                bma2x2_read_reg(BMA2x2_CHIP_ID__REG, &data, C_BMA2x2_ONE_U8X);
                if (data != VENDOR_ID) {
                    LOGI("g sensor data not consistent...\r\n");
					resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
					resonse.response_len   = 0;
                } else {
                    LOGI("g sensor data are consistent...\r\n");
					resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
					resonse.response_len   = 0;                
                }
            }
            else {
            
                /*invalide parameter, return "ERROR"*/ 
                strcpy((char *)resonse.response_buf, "ERROR\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
            }
            
            atci_send_response(&resonse);
            param = NULL;
            break;
            
        default :
            /* others are invalid command format */ 
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}
#else
/*--- Function ---*/
atci_status_t atci_cmd_hdlr_gsensor(atci_parse_cmd_param_t *parse_cmd);


// AT command handler
atci_status_t atci_cmd_hdlr_gsensor(atci_parse_cmd_param_t *parse_cmd)
{
	return ATCI_STATUS_OK;
}

#endif
