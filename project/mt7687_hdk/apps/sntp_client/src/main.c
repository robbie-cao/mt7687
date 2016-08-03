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

#include "sntp.h"
#include "syslog.h"
#include "hal_rtc.h"


/**
* @brief         SNTP task process function.
* @param[in]     Not using
* @return        None
*/
void verify_proc(void *args)
{
    hal_rtc_time_t r_time;
    hal_rtc_status_t ret = 0;

    for (int i=0 ; i<90; i++) {
        LOG_I(common, "Waiting for SNTP success [%d]", i);
        ret = hal_rtc_get_time(&r_time);
        if (ret==0 && (r_time.rtc_year!=0 || r_time.rtc_mon!=1 || r_time.rtc_day!=1)) {
            LOG_I(common, "SNTP success [%d]", i);
            LOG_I(common, "cur_time[%d,%d,%d,%d]", r_time.rtc_year, r_time.rtc_mon, r_time.rtc_day, r_time.rtc_week);
            LOG_I(common, "[%d]cur_time[%d:%d:%d]", ret, r_time.rtc_hour, r_time.rtc_min, r_time.rtc_sec);
            sntp_stop();
            break;
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    LOG_I(common, "test_proc TaskDelete");
    vTaskDelete(NULL);
}


/**
* @brief        SNTP client example entry function
* @return       None
*/
void sntp_client(void)
{
#if (!SNTP_SERVER_DNS)
    struct ip4_addr test_addr;
#endif

    TaskHandle_t xCreatedTask;
    hal_rtc_time_t r_time = {6,6,6,1,1,6,0};
    hal_rtc_status_t ret = 0;

    //Set RTC to a incorrect time.
    ret = hal_rtc_set_time(&r_time);
    LOG_I(common, "[%d]cur_time[%d:%d:%d]", ret, r_time.rtc_hour, r_time.rtc_min, r_time.rtc_sec);

    //Create a task to check SNTP status.
    portBASE_TYPE type = xTaskCreate(verify_proc, "sntpverify", 1024, NULL, 1, &xCreatedTask);
    LOG_I(common, "xTaskCreate test_proc -- %d", type);

    //SNTP example start.
    LOG_I(common, "Begin to init SNTP");

#if SNTP_SERVER_DNS
    sntp_setservername(0, "1.hk.pool.ntp.org");
    sntp_setservername(1, "1.cn.pool.ntp.org");
#else
    IP4_ADDR(&test_addr, 213, 161, 194, 93);
    sntp_setserver(0, (const ip_addr_t *)(&test_addr));
    IP4_ADDR(&test_addr, 129, 6, 15, 29);
    sntp_setserver(1, (const ip_addr_t *)(&test_addr));
#endif
    sntp_init();

    LOG_I(common, "SNTP init done");
}


/**
  * @brief     Create a task for SNTP client example
  * @param[in] void *args:Not used
  * @retval    None
  */
void user_entry(void *args)
{
    sta_network_ready();

    sntp_client();

    while (1) {
    }
}


int main(void)
{
    system_init();

    sta_network_init();

    xTaskCreate(user_entry, "user entry", 1024, NULL, 1, NULL);

    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}


