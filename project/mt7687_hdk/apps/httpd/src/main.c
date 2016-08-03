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
#include "syslog.h"
#include "sta_network.h"

#include "httpd.h"

// static int httpd_poll_status(void);
static int httpd_fb_handle(void);
static xQueueHandle mbox;

log_create_module(exa_httpd, PRINT_LEVEL_INFO);

/**
  * @brief      Http server init, start and stop flow.
  * @param      None
  * @return     None
  */
void httpd_test(void)
{
/*
 *  The webpages and settings can be customized.
 *
 *  The webpages on the server is stored in middleware/third_party/httpd/webhtml folder.
 *  The web server settings are defined in middleware/third_party/httpd/inc/config.h.
 */
    HTTPD_RESULT result = HTTPD_RESULT_SUCCESS;
    httpd_para parameter;
    // const char *server_port = NULL;

    LOG_I(exa_httpd, "[EXA_HTTPD] httpd_test()\n");

/*
 * Firstly, initialize the HTTP server.
 */
    result = httpd_init();

    switch(result)
    {
        case HTTPD_RESULT_SUCCESS:   /**<  Initialization was successful. */
            break;
        case HTTPD_RESULT_WAITING:   /**<  Needs to wait for the feedback. */
        case HTTPD_RESULT_UNINIT:   /**<  HTTP server is uninitialized. */
        case HTTPD_RESULT_QUEUE_FULL: /**<  The queue is full. */
            LOG_I(exa_httpd, "[EXA_HTTPD] httpd_test(): init failed.\n");
            return ;
    }

/*
 * Secondly, start the HTTP server.
 * After starting the HTTPD, the status can asynchronously queried, either through the message to listen to the status or through the polling.
 */
    mbox = xQueueCreate(4, sizeof(httpd_fb));
    if(mbox == NULL)
    {
        LOG_I(exa_httpd, "[EXA_HTTPD] httpd_test(): xQueueCreate failed.\n");
        return;
    }

    os_memset(&parameter, 0, sizeof(httpd_para));
    parameter.fb_queue = mbox;

    result = httpd_start(&parameter);

    switch(result)
    {
        case HTTPD_RESULT_SUCCESS: // Successful.
            break;
        case HTTPD_RESULT_UNINIT: // Uninitialized.
        case HTTPD_RESULT_QUEUE_FULL: // Mbox queue is full.
            LOG_I(exa_httpd, "[EXA_HTTPD] httpd_test(): start failed.\n");
            return;
        case HTTPD_RESULT_WAITING: // Need to wait for the feedback of the status notification or to poll the status.
        {
            if(httpd_fb_handle())
                break;
            else
            {
                LOG_I(exa_httpd, "[EXA_HTTPD] httpd_test(): start failed.\n");
                return;
            }
        }
    }

/*
 *  Right now, HTTP server has started. The user can browse the webpage.
 *
 *  Finally, the following flow can be used to stop HTTP server.
 */
 #if 0
    result = httpd_stop();

    switch(result)
    {
        case HTTPD_RESULT_SUCCESS: // Successful.
            break;
        case HTTPD_RESULT_UNINIT: // Uninitialized.
        case HTTPD_RESULT_QUEUE_FULL: // Mbox queue is full.
            LOG_I(exa_httpd, "[EXA_HTTPD] httpd_test(): stop failed.\n");
            return;
        case HTTPD_RESULT_WAITING: // Need to wait for the feedback of the status notification or to poll the status.
        {
            if(httpd_poll_status())
                break;
            else
            {
                LOG_I(exa_httpd, "[EXA_HTTPD] httpd_test(): start failed.\n");
                return;
            }
        }

    }
#endif
    LOG_I(exa_httpd, "[EXA_HTTPD] httpd_test(): Completed. \n");

    return;
}

/**
  * @brief      http server feedback handle
  * @param      None
  * @return     1, if http server run.\n
  *             0, if other status occurred.\n
  */
static int httpd_fb_handle(void)
{
    httpd_fb fb;

    LOG_I(exa_httpd, "[EXA_HTTPD] httpd_fb_handle()\n");

    for(;;)
    {
        if (xQueueReceive(mbox, (void *)&fb, portMAX_DELAY) == pdPASS)
        {
            LOG_I(exa_httpd, "[EXA_HTTPD] httpd_fb_handle(), status = %d\n", fb.status);

            switch(fb.status)
            {
                case HTTPD_STATUS_RUN:
                    return 1;  // HTTP server is running.
                case HTTPD_STATUS_STOPPING:
                case HTTPD_STATUS_STOP:
                    break; // HTTP server is stopping or has stopped.
                case HTTPD_STATUS_UNINIT:
                    return 0;  // Uninitialized.
            }
        }
    }

    // return 0;
}

/**
  * @brief      Polling http server status
  * @param      None
  * @return     1, if http server stopped.\n
  *             0, if other status occurred.\n
  */
#if 0
static int httpd_poll_status(void)
{
    HTTPD_STATUS status = HTTPD_STATUS_UNINIT;

    LOG_I(exa_httpd, "[EXA_HTTPD] httpd_poll_status()\n");

    for(;;)
    {
        vTaskDelay(1000); // Delay by 1000ms.

        status = httpd_get_status();

        switch(status)
        {
            case HTTPD_STATUS_STOP:
                return 1;  // HTTP server has stopped.
            case HTTPD_STATUS_STOPPING:
            case HTTPD_STATUS_RUN:
                break; // HTTP server is running.
            case HTTPD_STATUS_UNINIT:
                return 0;  // Uninitialized.
        }
    }

    // return 0;
}
#endif

/**
  * @brief      Create a task for SNTP client example
  * @param[in]  void *args:Not used
  * @return     None
  */
void user_entry(void *args)
{
    sta_network_ready();

    httpd_test();

    for (;;) {
        // Delay by 1000ms. Keep HTTPD task alive, which priority is 0.
        vTaskDelay(1000); 
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


