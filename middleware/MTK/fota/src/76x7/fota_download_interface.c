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

#ifndef MOD_CFG_FOTA_DISABLE_CLI

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* Other modules */
#include "toi.h"
#include "syslog.h"

#include "httpclient.h"
#include "fota_download_interface.h"
#include "fota.h"
#include "fota_config.h"
#include "fota_76x7.h"
#include "hal_sys.h"

/****************************************************************************
 * Macros
 ****************************************************************************/
#define FOTA_BUF_SIZE    (1024 * 4 + 1)
#define FOTA_URL_BUF_LEN    (256)

log_create_module(fota_dl_api, PRINT_LEVEL_INFO);
/****************************************************************************
 * Static variables
 ****************************************************************************/
httpclient_t _s_fota_httpclient = {0};

static int32_t _fota_http_retrieve_get(char* get_url, char* buf, uint32_t len)
{
    int32_t ret = HTTPCLIENT_ERROR_CONN;
    fota_status_t write_ret;
    httpclient_data_t client_data = {0};
    uint32_t count = 0;
    uint32_t recv_temp = 0;
    uint32_t data_len = 0;

    client_data.response_buf = buf;
    client_data.response_buf_len = len;

    ret = httpclient_send_request(&_s_fota_httpclient, get_url, HTTPCLIENT_GET, &client_data);
    if (ret < 0) {
        
        LOG_E(fota_dl_api, "[FOTA CLI] http client fail to send request.\n");
        return ret;
    }

    do {
        ret = httpclient_recv_response(&_s_fota_httpclient, &client_data);
        if (ret < 0) {
            return ret;
        }

        if (recv_temp == 0)
        {
            recv_temp = client_data.response_content_len;
        }

        LOG_I(fota_dl_api, "[FOTA CLI]retrieve_len = %d \n", client_data.retrieve_len);
        
        data_len = recv_temp - client_data.retrieve_len;
        LOG_I(fota_dl_api, "[FOTA CLI]data_len = %u \n", data_len);
        
        count += data_len;
        recv_temp = client_data.retrieve_len;
        vTaskDelay(100);/* Print log may block other task, so sleep some ticks */
        LOG_I(fota_dl_api, "[FOTA CLI]total data received %u\n", count);
        //client_data.response_buf[5] = '\0';
        //if (strcmp(string_table[seg++], client_data.response_buf) != 0) {
        //    return -1;
        //}
        write_ret = fota_write(FOTA_PARITION_TMP, (const uint8_t*)client_data.response_buf, data_len);
        if (FOTA_STATUS_OK != write_ret) {
            LOG_E(fota_dl_api, "fail to write flash, write_ret = %d\n", write_ret);
            return ret;
        }

        LOG_I(fota_dl_api, "[FOTA CLI] download progrses = %u\n", count * 100 / client_data.response_content_len);
        
    } while (ret == HTTPCLIENT_RETRIEVE_MORE_DATA);

    LOG_I(fota_dl_api, "[FOTA CLI]total length: %d\n", client_data.response_content_len);
    if (count != client_data.response_content_len || httpclient_get_response_code(&_s_fota_httpclient) != 200) {
        LOG_E(fota_dl_api, "[FOTA CLI]data received not completed, or invalid error code\r\n");
        
        return -1;
    }

    return ret;
}

uint8_t fota_download_by_http(char *param)
{
    char get_url[FOTA_URL_BUF_LEN];
    int32_t ret = HTTPCLIENT_ERROR_CONN;
    uint32_t len_param = strlen(param);

    if (len_param < 1) {
      return -1;
    }
    memset(get_url, 0, FOTA_URL_BUF_LEN);
    LOG_I(fota_dl_api, "length: %d\n", strlen(param));
    strcpy(get_url, param);
    if (fota_init(&fota_flash_default_config) != FOTA_STATUS_OK) {
        LOG_E(fota_dl_api, "[FOTA CLI] fota init fail. \n");
        return -2;
    }
    char* buf = pvPortMalloc(FOTA_BUF_SIZE);
    if (buf == NULL) {
        LOG_E(fota_dl_api, "buf malloc failed.\r\n");
        return -3;
    }
    ret = httpclient_connect(&_s_fota_httpclient, get_url, HTTP_PORT);
    if (!ret) {
        ret = _fota_http_retrieve_get(get_url, buf, FOTA_BUF_SIZE);
    }else {
        LOG_E(fota_dl_api, "[FOTA CLI] http client connect error. \r");
    }
    LOG_I(fota_dl_api, "Download result = %d \r\n", (int)ret);

    httpclient_close(&_s_fota_httpclient, HTTP_PORT);

    vPortFree(buf);
    buf = NULL;
    if ( 0 == ret) {
        fota_trigger_update();
        fota_ret_t err;
        err = fota_trigger_update();
        if (0 == err ){
            hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
            LOG_I(fota_dl_api, "Reboot device!");
            return 0;
        } else {
            LOG_E(fota_dl_api, "Trigger FOTA error!");
            return -1;
        }
        return 0;
    } else {
        return -1;
    }
}

#endif
