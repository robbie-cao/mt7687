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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "os.h"
#include "sys_init.h"
#include "wifi_api.h"
#include "sta_network.h"
#include "httpclient.h"


#define BUF_SIZE   (1024 * 1)

/**
  * @brief      Http client "get" method working flow.
  * @param      None
  * @return     None
  */
static void httpclient_test(void)
{
    char *url = "http://apis.map.qq.com/ws/location/v1/ip?ip=61.135.17.68&key=6MABZ-VFKAF-DITJ6-JRPZN-OUOFJ-ULBWQ";
    /*char *url = "http://posttestserver.com/post.php";
    char *content_type = "application/json;charset=utf-8";
    char *post_data = "{\"title\":\"test\",\"sub\":[1,2,3]}";*/
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf;

    /* Log a message indicating the task has started. */
    printf("httpclient_test starts.\r\n");

    buf = pvPortMalloc(BUF_SIZE);
    if (buf == NULL) {
        printf("httpclient_test malloc failed.\r\n");
        return;
    }
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;

    httpclient_get(&client, url, HTTP_PORT, &client_data);
    /*client_data.post_buf = post_data;
    client_data.post_buf_len = strlen(post_data);    client_data.post_content_type= content_type;
    httpclient_post(&client, url, HTTP_PORT, &client_data);*/

    printf("httpclient_test data received: %s\r\n", client_data.response_buf);

    vPortFree(buf);

    printf("httpclient_test ends.\r\n");
}


/**
  * @brief      Create a task for http client example
  * @param[in]  void *args:Not used
  * @return     None
  */
void user_entry(void *args)
{
    sta_network_ready();

    httpclient_test();

    while (1) {
    }
}


/**
  * @brief      Main program
  * @param      None
  * @return     0
  */
int main(void)
{
    system_init();

    sta_network_init();

    xTaskCreate(user_entry, "user entry", 2048, NULL, 1, NULL);

    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}


