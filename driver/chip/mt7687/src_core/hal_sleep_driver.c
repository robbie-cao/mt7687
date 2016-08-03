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

#include "hal_sleep_driver.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_sleep_driver.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_eint.h"
#include "string.h"
#include "stdio.h"

extern uint32_t save_and_set_interrupt_mask(void);
extern void restore_interrupt_mask(uint32_t mask);

sleep_driver_struct sleepdrv;
uint32_t sleep_driver_handle_array[MAX_SLEEP_HANDLE];
uint32_t sleep_driver_handle_ref_count[MAX_SLEEP_HANDLE];

static int SDMutex = 1; //hal layer can not use OS mutex.

#define MAX_CB_NUM 32
int8_t suspend_cb_idx = 0;
int8_t resume_cb_idx = 0;
sleepdrv_cb_t suspend_cbs[MAX_CB_NUM];
sleepdrv_cb_t resume_cbs[MAX_CB_NUM];


void sleep_driver_init()
{
    sleepdrv.handleInit = false;
    sleepdrv.handleCount = 0;
    sleepdrv.sleepDisable = 0;
    sleepdrv.handleInit = true;
    memset(sleep_driver_handle_array, 0, sizeof(sleep_driver_handle_array));
    memset(sleep_driver_handle_ref_count, 0, sizeof(sleep_driver_handle_ref_count));

    sleepdrv_register_callback();
}

bool sleepdrv_get_mutex(void)
{
    int32_t savedMask;
    savedMask = save_and_set_interrupt_mask();
    if (SDMutex == 1) {
        SDMutex--;
        restore_interrupt_mask(savedMask);
        return true;
    } else if (SDMutex == 0) {
        restore_interrupt_mask(savedMask);
        return false;
    } else {
        restore_interrupt_mask(savedMask);
    }
    return false;
}

void sleepdrv_release_mutex(void)
{
    int32_t savedMask;
    savedMask = save_and_set_interrupt_mask();
    if (SDMutex == 0) {
        SDMutex++;
        restore_interrupt_mask(savedMask);
    } else {
        restore_interrupt_mask(savedMask);
    }
}

uint8_t sleepdrv_get_handle(const char *handle_name, uint8_t size)
{
    uint8_t handle_index = 0;

    sleepdrv_get_mutex();
    for (handle_index = 0; handle_index < MAX_SLEEP_HANDLE; handle_index++) {
        if (sleep_driver_handle_array[handle_index] == 0) {
            sleepdrv.handleCount++;
            sleep_driver_handle_array[handle_index] = (uint32_t)handle_name;
            break;
        }
    }

    if (handle_index >= MAX_SLEEP_HANDLE) {
        log_hal_info("Fatal error, cannot get sleep handle\n");
        handle_index = INVALID_SLEEP_HANDLE;
    }
    sleepdrv_release_mutex();
    return handle_index;
}

void sleepdrv_release_handle(uint8_t handle)
{
    sleepdrv_get_mutex();
    sleepdrv.handleCount--;
    sleep_driver_handle_array[handle] = 0;
    sleepdrv_release_mutex();
}

bool sleepdrv_is_handle_valid(uint8_t handle_index)
{
    bool valid = false;
    sleepdrv_get_mutex();
    if (sleep_driver_handle_array[handle_index] != 0)
        valid = true;
    sleepdrv_release_mutex();
    return valid;
}

bool sleepdrv_release_sleep_lock(uint8_t handle_index)
{
    bool ret = false;

    if (!sleepdrv_is_handle_valid(handle_index))
        return ret;

    if (handle_index < MAX_SLEEP_HANDLE) {
        uint32_t _savedMask;
        _savedMask = save_and_set_interrupt_mask();
        if (sleep_driver_handle_ref_count[handle_index] != 0) {
            sleep_driver_handle_ref_count[handle_index]--;
            if (sleep_driver_handle_ref_count[handle_index] == 0) {
                sleepdrv.sleepDisable &= ~(1 << handle_index);
            }
            ret = true;
        } else {
            ret = false;
        }
        restore_interrupt_mask(_savedMask);
    }

    return ret;
}

bool sleepdrv_hold_sleep_lock(uint8_t handle_index)
{
    bool ret = false;

    if (!sleepdrv_is_handle_valid(handle_index))
        return ret;

    if (handle_index < MAX_SLEEP_HANDLE) {
        uint32_t _savedMask;
        _savedMask = save_and_set_interrupt_mask();
        sleep_driver_handle_ref_count[handle_index]++;
        sleepdrv.sleepDisable |= (1 << handle_index);
        restore_interrupt_mask(_savedMask);
        ret = true;
    }

    return ret;
}

bool sleepdrv_get_handle_status(uint8_t handle)
{
    if (sleepdrv.sleepDisable & (1 << handle)) {
        return true;
    } else {
        return false;
    }
}

bool sleepdrv_check_sleep_locks()
{
    uint32_t _savedMask;
    bool lock;
    _savedMask = save_and_set_interrupt_mask();
    lock = sleepdrv.sleepDisable != 0 ? true : false;
    restore_interrupt_mask(_savedMask);
    return lock;
}

uint32_t sleepdrv_get_lock_index(void)
{
    //printf("index : %#x is locking\n", (unsigned int)sleepdrv.sleepDisable);
    return sleepdrv.sleepDisable;
}

uint32_t sleepdrv_get_lock_module(void)
{
    int i;
    for (i = 0; i < MAX_SLEEP_HANDLE; i++) {
        if (sleepdrv.sleepDisable & (1 << i)) {
            printf("index : %s is locking\n", (char *)sleep_driver_handle_array[i]);
        }
    }
    return sleepdrv.sleepDisable;
}

bool sleepdrv_register_suspend_cb(sleepdrv_cb func, void *data)
{
    if (suspend_cb_idx >= MAX_CB_NUM) {
        printf("over max suspend callback function can be registered\n");
        return false;
    }

    suspend_cbs[suspend_cb_idx].func = func;
    suspend_cbs[suspend_cb_idx].para = data;
    suspend_cb_idx++;
    return true;
}

void sleepdrv_run_suspend_cbs(void)
{
    for (int32_t i = 0; i < suspend_cb_idx; i++) {
        suspend_cbs[i].func(suspend_cbs[i].para);
    }
}

bool sleepdrv_register_resume_cb(sleepdrv_cb func, void *data)
{
    if (resume_cb_idx >= MAX_CB_NUM) {
        printf("over max resume callback function can be registered\n");
        return false;
    }

    resume_cbs[resume_cb_idx].func = func;
    resume_cbs[resume_cb_idx].para = data;

    resume_cb_idx++;

    return true;
}

void sleepdrv_run_resume_cbs(void)
{
    for (int32_t i = 0; i < resume_cb_idx; i++) {
        resume_cbs[i].func(resume_cbs[i].para);
    }
}

void sleepdrv_register_callback(void)
{
//Example
//   sleepdrv_register_suspend_cb(i2c_sleep_cb, "i2c_xxx");
//   sleepdrv_register_resume_cb(i2c_wakeup_cb, "i2c_xxx");
}


#endif /* HAL_SLEEP_MANAGER_ENABLED */
