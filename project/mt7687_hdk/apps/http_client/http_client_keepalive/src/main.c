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
#include "timers.h"
#include "sta_network.h"
#include "httpclient.h"


#define BUF_SIZE   (1024 * 1)

#define HTTPS_MTK_CLOUD_GET_URL     "https://api.mediatek.com/mcs/v2/devices/D0n2yhrl/datachannels/1/datapoints"
#define HTTPS_MTK_CLOUD_POST_URL    "https://api.mediatek.com/mcs/v2/devices/D0n2yhrl/datapoints.csv"
#define HTTPS_MTK_CLOUD_HEADER      "deviceKey:FZoo0S07CpwUHcrt\r\n"

#define HTTP_GET_TIME_TICK          (10000/portTICK_PERIOD_MS)
#define HTTP_POST_TIME_TICK          (5000/portTICK_PERIOD_MS)

httpclient_t client1 = {0}, client2 = {0};
char *buf1 = NULL, *buf2 = NULL;
int post_count = 0, get_count = 0;

static TimerHandle_t http_get_timer = NULL, http_post_timer = NULL;


/**
  * @brief      Close "get" method socket connection.
  * @param      None
  * @return     None
  */
void httpclient_test_keepalive_get_close(void)
{
    printf("httpclient_test_keepalive_get_close. END\r\n");
    httpclient_close(&client1, HTTPS_PORT);
    vPortFree(buf1);
    buf1 = NULL;
}

/**
  * @brief      Close "post" method socket connection.
  * @param      None
  * @return     None
  */
void httpclient_test_keepalive_post_close(void)
{
    printf("httpclient_test_keepalive_post_close. END\r\n");
    httpclient_close(&client2, HTTPS_PORT);
    vPortFree(buf2);
    buf2 = NULL;
}

/**
  * @brief      Send/Recv package by "get" method.
  * @param      None
  * @return     0, if OK.\n
  *             Error code, if errors occurred.\n
  */
int httpclient_test_keepalive_get(void)
{
    int ret = HTTPCLIENT_ERROR_CONN;
    httpclient_data_t client_data = {0};
    char *get_url = HTTPS_MTK_CLOUD_GET_URL;
    char *header = HTTPS_MTK_CLOUD_HEADER;

    if (buf1 == NULL) {
        return ret;
    }
    buf1[0] = '\0';

    client_data.response_buf = buf1;
    client_data.response_buf_len = BUF_SIZE;
    httpclient_set_custom_header(&client1, header);;

    ret = httpclient_send_request(&client1, get_url, HTTPCLIENT_GET, &client_data);
    if (ret < 0) {
        return ret;
    }
    ret = httpclient_recv_response(&client1, &client_data);
    if (ret < 0) {
        return ret;
    }
    printf("\n************************\n");
    printf("httpclient_test_keepalive get data every 10 sec, count = %d, http status:%d, response data: %s\r\n", get_count++, httpclient_get_response_code(&client1), client_data.response_buf);
    printf("************************\n");

    return ret;
}

/**
  * @brief      Send/Recv package by "post" method.
  * @param      None
  * @return     0, if OK.\n
  *             Error code, if errors occurred.\n
  */
int httpclient_test_keepalive_post(void)
{
    int ret = HTTPCLIENT_ERROR_CONN;
    httpclient_data_t client_data = {0};
    char *post_url = HTTPS_MTK_CLOUD_POST_URL;
    char *content_type = "text/csv";
    char post_data[32];
    char *header = HTTPS_MTK_CLOUD_HEADER;

    if (buf2 == NULL) {
        return ret;
    }
    buf2[0] = '\0';

    client_data.response_buf = buf2;
    client_data.response_buf_len = BUF_SIZE;
    client_data.post_content_type = content_type;
    sprintf(post_data, "1,,temperature:%d", (10 + post_count));
    client_data.post_buf = post_data;
    client_data.post_buf_len = strlen(post_data);
    httpclient_set_custom_header(&client2, header);

    ret = httpclient_send_request(&client2, post_url, HTTPCLIENT_POST, &client_data);
    if (ret < 0) {
        return ret;
    }
    ret = httpclient_recv_response(&client2, &client_data);
    if (ret < 0) {
        return ret;
    }
    printf("\n************************\n");
    printf("httpclient_test_keepalive post data every 5 sec, count = %d, http status:%d, response data: %s\r\n", post_count++, httpclient_get_response_code(&client2), client_data.response_buf);
    printf("************************\n");

    return ret;
}

/**
  * @brief      Send "keepalive" package by "get" method in every HTTP_GET_TIME_TICK.
  * @param      None
  * @return     None
  */
static void httpclient_test_keepalive_get_timer_callback( TimerHandle_t tmr )
{
    int ret = HTTPCLIENT_ERROR_CONN;

    if (tmr) {
        xTimerStop(tmr, 0);
        xTimerDelete(tmr, portMAX_DELAY);
        http_get_timer = NULL;
    }

    ret = httpclient_test_keepalive_get();

    if (ret < 0 || get_count == 7) {
        httpclient_test_keepalive_get_close();
    } else {
        http_get_timer = xTimerCreate( "http_get_timer",
                                       HTTP_GET_TIME_TICK,
                                       pdFALSE,
                                       NULL,
                                       httpclient_test_keepalive_get_timer_callback);
        if (http_get_timer != NULL) {
            xTimerStart(http_get_timer, 0);
        }
    }
}

/**
  * @brief      Send "keepalive" package by "post" method in every HTTP_POST_TIME_TICK.
  * @param      None
  * @return     None
  */
static void httpclient_test_keepalive_post_timer_callback( TimerHandle_t tmr )
{
    int ret = HTTPCLIENT_ERROR_CONN;

    if (tmr) {
        xTimerStop(tmr, 0);
        xTimerDelete(tmr, portMAX_DELAY);
        http_post_timer = NULL;
    }

    ret = httpclient_test_keepalive_post();

    if (ret < 0 || post_count == 11) {
        httpclient_test_keepalive_post_close();
    } else {
        http_post_timer = xTimerCreate( "http_post_timer",
                                        HTTP_POST_TIME_TICK,
                                        pdFALSE,
                                        NULL,
                                        httpclient_test_keepalive_post_timer_callback);

        if (http_post_timer != NULL) {
            xTimerStart(http_post_timer, 0);
        }
    }
}

/**
  * @brief      Http client connect to test server and test "keepalive" function.
  * @param      None
  * @return     None
  */
void httpclient_test_keepalive(void)
{
    char *get_url = HTTPS_MTK_CLOUD_GET_URL;
    char *post_url = HTTPS_MTK_CLOUD_POST_URL;
    int ret1 = HTTPCLIENT_ERROR_CONN, ret2 = HTTPCLIENT_ERROR_CONN;

    get_count = 0;
    post_count = 0;

    buf1 = pvPortMalloc(BUF_SIZE);
    if (buf1 == NULL) {
        printf("httpclient_test_keepalive buf1 malloc failed.\r\n");
        return;
    }

    buf2 = pvPortMalloc(BUF_SIZE);
    if (buf2 == NULL) {
        printf("httpclient_test_keepalive buf2 malloc failed.\r\n");
        vPortFree(buf1);
        buf1 = NULL;
        return;
    }

    ret1 = httpclient_connect(&client1, get_url, HTTPS_PORT);
    ret2 = httpclient_connect(&client2, post_url, HTTPS_PORT);

    if (!ret1 && !ret2) {
        httpclient_test_keepalive_post_timer_callback(NULL);
        httpclient_test_keepalive_get_timer_callback(NULL);
    } else {
        httpclient_test_keepalive_post_close();
        httpclient_test_keepalive_get_close();
    }
}


/**
  * @brief      Create a task for http client example
  * @param[in]  void *args:Not used
  * @return     None
  */
void user_entry(void *args)
{
    sta_network_ready();

    httpclient_test_keepalive();

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

    xTaskCreate(user_entry, "user entry", 6*1024/sizeof(portSTACK_TYPE), NULL, 1, NULL);

    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}


