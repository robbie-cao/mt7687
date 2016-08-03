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

/* Kernel includes. */
#include "bt_os_layer_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timer.h"
#include "semphr.h"
#include "portmacro.h"
#include "queue.h"
#include <timers.h>
#include <string.h>
#include "syslog.h"
#include "hal_aes.h"

static TimerHandle_t bt_rtos_timer = NULL; /**< Timer handler. */
static bt_os_layer_timer_expired_t bt_rtos_timer_cb;  /**< Timer callback function. */

extern int rand(void);
uint16_t bt_os_layer_generate_random(void)
{
    return rand();
}

void bt_os_layer_aes_encrypt(bt_os_layer_aes_buffer_t* encrypted_data, bt_os_layer_aes_buffer_t* plain_text, bt_os_layer_aes_buffer_t* key)
{
    hal_aes_ecb_encrypt((hal_aes_buffer_t*)encrypted_data, (hal_aes_buffer_t*)plain_text, (hal_aes_buffer_t*)key);
}

static void bt_os_layer_rtos_timer_os_expire(TimerHandle_t timer)
{
    if(bt_rtos_timer_cb != NULL) {
        bt_rtos_timer_cb();
    }
}

void bt_os_layer_init_timer(void)
{
    if(bt_rtos_timer == NULL) {
        bt_rtos_timer = xTimerCreate( "hb timer", 0xffff, pdFALSE, NULL, bt_os_layer_rtos_timer_os_expire);
        bt_rtos_timer_cb = NULL;
    }
}

void bt_os_layer_deinit_timer(void)
{
    if (bt_rtos_timer != NULL) {
        xTimerDelete(bt_rtos_timer, 0);
        bt_rtos_timer = NULL;
    }
}

void bt_os_layer_sleep_task(uint32_t ms)
{
    uint32_t time_length = ms/portTICK_PERIOD_MS;
    if(time_length > 0) {
        vTaskDelay(time_length);
    }
}

uint32_t bt_os_layer_get_current_task_id(void)
{
    return (uint32_t)xTaskGetCurrentTaskHandle();
}

//MUTEX LOCK
uint32_t bt_os_layer_create_mutex(void)
{
    return (uint32_t)xSemaphoreCreateRecursiveMutex();
}

void bt_os_layer_take_mutex(uint32_t mutex_id)
{
    if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }
    xSemaphoreTakeRecursive((SemaphoreHandle_t)mutex_id, portMAX_DELAY);
}

void bt_os_layer_give_mutex(uint32_t mutex_id)
{
    if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }
    xSemaphoreGiveRecursive((SemaphoreHandle_t)mutex_id);
}

void bt_os_layer_delete_mutex(uint32_t mutex_id)
{
    vSemaphoreDelete((SemaphoreHandle_t)mutex_id);
}

uint32_t bt_os_layer_create_semaphore()
{
    return (uint32_t)xSemaphoreCreateBinary();
}

void bt_os_layer_take_semaphore_from_isr(uint32_t semaphore_id)
{
    BaseType_t priorityTaskWoken;
    xSemaphoreTakeFromISR((SemaphoreHandle_t)semaphore_id, &priorityTaskWoken);
}

void bt_os_layer_take_semaphore(uint32_t semaphore_id)
{
    xSemaphoreTake((SemaphoreHandle_t)semaphore_id, portMAX_DELAY);
}

void bt_os_layer_give_semaphore_from_isr(uint32_t semaphore_id)
{
    BaseType_t priorityTaskWoken;
    xSemaphoreGiveFromISR((SemaphoreHandle_t)semaphore_id, &priorityTaskWoken);
    return;
}

void bt_os_layer_give_semaphore(uint32_t semaphore_id)
{
    xSemaphoreGive((SemaphoreHandle_t)semaphore_id);
}

void bt_os_layer_delete_semaphore(uint32_t semaphore_id)
{
    vSemaphoreDelete((SemaphoreHandle_t)semaphore_id);
}

uint32_t bt_os_layer_get_system_tick(void)
{
    return xTaskGetTickCount();
}

void bt_os_layer_register_timer_callback(bt_os_layer_timer_expired_t callback)
{
    bt_rtos_timer_cb = callback;
}

void bt_os_layer_start_timer(uint32_t ms)
{
    uint32_t time_length = ms/portTICK_PERIOD_MS + 1;
    if (bt_rtos_timer == NULL) {
        return;
    }
    if (bt_os_layer_is_timer_active() == 1) {
        bt_os_layer_stop_timer();
    }
    xTimerChangePeriod(bt_rtos_timer, time_length, portMAX_DELAY);
    xTimerReset(bt_rtos_timer, portMAX_DELAY);
}

void bt_os_layer_stop_timer(void)
{
    if  ((bt_rtos_timer != NULL) && (bt_os_layer_is_timer_active() == 1)) {
        xTimerStop(bt_rtos_timer, portMAX_DELAY);
    }
}

uint32_t bt_os_layer_is_timer_active(void)
{
    if ((bt_rtos_timer != NULL) && (xTimerIsTimerActive(bt_rtos_timer) != pdFALSE)) {
        return 1;
    }
    else {
        return 0;
    }
}

void bt_os_layer_disable_interrupt(void)
{
    taskDISABLE_INTERRUPTS();
}

void bt_os_layer_enable_interrupt(void)
{
    taskENABLE_INTERRUPTS();
}

void bt_os_layer_disable_system_sleep(void)
{

}

void bt_os_layer_enable_system_sleep(void)
{

}

int32_t bt_os_layer_get_return_address(void)
{
	#if defined(__ARMCC_VERSION)
		return 0;
	#else
    return (int32_t)__builtin_return_address(0);
	#endif
}

void* bt_os_layer_memcpy(void* dest, const void* src, uint32_t size)
{
    return memcpy(dest, src, size);                
}

int bt_os_layer_memcmp(const void* buf1, const void* buf2, uint32_t size)
{
    return memcmp(buf1, buf2, size);                
}

void* bt_os_layer_memset(void* buf, uint8_t ch, uint32_t size)
{
    return memset(buf, ch, size);                
}

void* bt_os_layer_memmove(void* dest, const void* src, uint32_t size)
{
    return memmove(dest, src, size);                
}

