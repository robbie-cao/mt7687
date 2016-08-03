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
#include "at_command.h"
#include <string.h>
#include <stdio.h>

//For bt head file
#include "hal_audio.h"


#if defined(HAL_AUDIO_MODULE_ENABLED)

#include "hal_audio_test.h"

/*---  Define ---*/


/*---  Variant ---*/



/*--- Function ---*/
// AT command handler
atci_status_t atci_cmd_hdlr_audio(atci_parse_cmd_param_t *parse_cmd)
{
#if defined(__GNUC__)
    atci_response_t response = {{0}};
    printf("atci_cmd_hdlr_audio \r\n");

    response.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+ECMP=?
            strcpy((char * restrict)response.response_buf, "+EAUDIO =<op>[,<param>]\r\nOK\r\n");
            response.response_len = strlen((const char *)response.response_buf);
            atci_send_response(&response);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EAUDIO=<op>  the handler need to parse the parameters
            if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=1,1") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HEADSET);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_play_voice_1k_tone();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=1,2") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HEADSET);
                audio_test_set_audio_tone(true);
                uint8_t result = audio_test_play_audio_1k_tone();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=5,1") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_external_loopback_test();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=5,2") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC);
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_external_loopback_test();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=0") != NULL) {
                uint8_t result = audio_test_stop_1k_tone();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=2") != NULL) {
                uint8_t result = audio_test_detect_earphone();
                snprintf((char * restrict)response.response_buf, sizeof(response.response_buf), "+EAUDIO:%d\r\n", result);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
#if defined(HAL_AUDIO_SLT_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=4") != NULL) {
                uint8_t result = audio_test_detect_1k_tone_result();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
#endif
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=3") != NULL) {
                //callback
                register_accdet_callback();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
#if defined(HAL_AUDIO_SLT_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=6") != NULL) {
                uint8_t result = audio_slt_test();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=7") != NULL) {
                audio_test_stop_1k_tone();
                uint8_t result = audio_test_detect_1k_tone_result();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
#endif
#if defined(HAL_AUDIO_SDFATFS_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=8,1") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_audio_tone(false);
                audio_test_switch_mic_type(0); //acc
                uint8_t result = audio_test_pcm2way_record();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=8,2") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_audio_tone(false);
                audio_test_switch_mic_type(1); //dcc
                uint8_t result = audio_test_pcm2way_record();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=8,3") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_pcm2way_record();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=8,4") != NULL) {
                uint8_t result = audio_test_pcm2way_stop_1k_tone();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=8,5") != NULL) { //dcc wb
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_audio_tone(false);
                audio_test_switch_mic_type(1); //dcc
                uint8_t result = audio_test_pcm2way_wb_record();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=8,6") != NULL) { //acc wb
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_audio_tone(false);
                audio_test_switch_mic_type(0); //acc
                uint8_t result = audio_test_pcm2way_wb_record();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=9,1") != NULL) {
                uint8_t result = audio_test_play_audio_sd();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=9,2") != NULL) {
                uint8_t result = audio_test_stop_audio_sd();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
#endif
            else {
                /* invalid AT command */
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            break;

        default :
            /* others are invalid command format */
            strcpy((char * restrict)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((const char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
#endif /*defined(__GNUC__)*/    
    return ATCI_STATUS_OK;
}

#endif /* defined(HAL_AUDIO_MODULE_ENABLED) */
