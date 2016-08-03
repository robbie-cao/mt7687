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
#ifdef MTK_ATCI_CAMERA_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
//#include "camera.h"
/*
 * sample code
*/

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_camera(atci_parse_cmd_param_t *parse_cmd);

/*
AT+ECMP=<op>	            |   "OK" 
AT+ECMP=?                     |   "+ECMP=(0,1)","OK"

URC:
+ECMP: <camera mode>

*/
// AT command handler
atci_status_t atci_cmd_hdlr_camera(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {0};
    char *param = NULL;
    int  param1_val = -1;
    int  excute_value = 0;
    printf("atci_cmd_hdlr_camera \r\n");
    
    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+ECMP=?
            strcpy(resonse.response_buf, "+ECMP=(0,1)\r\nOK\r\n");
            resonse.response_len = strlen(resonse.response_buf);
            atci_send_response(&resonse);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EKEYURC=<op>  the handler need to parse the parameters
            param = strtok(parse_cmd->string_ptr, ",\n\r");
	    param = strtok(parse_cmd->string_ptr, "AT+ECMP=");
            param1_val = atoi(param);
            
            if (param != NULL && (param1_val == 0 || param1_val == 1)) {

		        /* valid parameter, update the data and return "OK"
                                param1_val = 0 : enter camera preview mode
                                param1_val = 1 :  camera power down*/
                                
                    if (param1_val == 0) {
                        AtCmdCalCameraPreviewReq();
		    } else {
                        CalCameraPowerDownReq();
                    }
				
                sprintf(resonse.response_buf,"+ECMP:%d\r\n", excute_value);
                /* ATCI will help append "OK" at the end of resonse buffer  */ 
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen(resonse.response_buf);
            } else {
                /*invalide parameter, return "ERROR"*/ 
                strcpy(resonse.response_buf, "ERROR\r\n");
                resonse.response_len = strlen(resonse.response_buf);
            };
            atci_send_response(&resonse);
			param = NULL;
            break;
            
        default :
            /* others are invalid command format */ 
            strcpy(resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen(resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}

#endif
