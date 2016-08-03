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

#include "hal_sleep_manager.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_sleep_driver.h"
#include "hal_sys_topsm.h"
#include "hal_sleep_manager_platform.h"
#include "hal_log.h"
#include "hal_cm4_topsm.h"
#include "hal_gpt.h"
#include "hal_eint.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "string.h"
#include "hal_gpio.h"
#include "stdio.h"

sleep_driver_struct sleepdrv;
extern cm4_topsm_mtcmos_control mtcmos_control;
uint32_t sleep_driver_handle_array[MAX_SLEEP_HANDLE];
uint32_t sleep_driver_handle_ref_count[MAX_SLEEP_HANDLE];

extern uint32_t SaveAndSetIRQMask(void);
extern void RestoreIRQMask(uint32_t x);
ATTR_RWDATA_IN_TCM extern bool FAST_WAKEUP_VERIFICATION;
static int SDMutex = 1; //hal layer can not use OS mutex.
hal_gpio_pin_t GPIO_INDEX;
hal_eint_number_t EINT_INDEX;
void fast_wakeup_eint_handler(void *parameter)
{
    hal_eint_unmask(EINT_INDEX);
    printf("[Eint Wakeup]\n");
}
void sleep_driver_fast_wakeup_eint_setting(hal_gpio_pin_t gpio_index, hal_eint_number_t eint_index)
{
    GPIO_INDEX = gpio_index;
    EINT_INDEX = eint_index;
    hal_eint_config_t config;

    hal_gpio_init(GPIO_INDEX);
    hal_pinmux_set_function(GPIO_INDEX, 1);
    config.trigger_mode = HAL_EINT_EDGE_FALLING;
    config.debounce_time = 0;

    hal_eint_init(EINT_INDEX, &config);    //set EINT trigger mode and debounce time.
    hal_eint_register_callback(EINT_INDEX, fast_wakeup_eint_handler, NULL); // register a user callback.
    hal_eint_unmask(EINT_INDEX);
}
void sleep_driver_fast_wakeup_verification_Setting()
{
    FAST_WAKEUP_VERIFICATION = true;
    cm_bus_clk_26m();
    PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_HP, PMIC_VCORE_0P9V);
    PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_LP, PMIC_VCORE_0P9V);
    PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_S0, PMIC_VCORE_0P9V);
    *((volatile uint32_t *) 0xA2010238) = 0x1B;
    *((volatile uint32_t *) 0xa2020C30) = 0x77777777;
    *((volatile uint32_t *) 0xa2020C40) = 0x77771117;
    *((volatile uint32_t *) 0xa2020C50) = 0x7700;
    *((volatile uint32_t *) 0xA2020C18) = 0xF0000000;
    *((volatile uint32_t *) 0xA2020004) = 0x00008000;
    *((volatile uint32_t *) 0xA2020304) = 0x00008000; //high
}
void sleep_driver_init()
{
    sleepdrv.handleInit = false;
    sleepdrv.handleCount = 0;
    sleepdrv.sleepDisable = 0;
    sleepdrv.handleInit = true;
    memset(sleep_driver_handle_array, 0, sizeof(sleep_driver_handle_array));
    memset(sleep_driver_handle_ref_count, 0, sizeof(sleep_driver_handle_ref_count));
}

bool sleepdrv_get_mutex(void)
{
    int32_t savedMask;
    savedMask = SaveAndSetIRQMask();
    if (SDMutex == 1) {
        SDMutex--;
        RestoreIRQMask(savedMask);
        return true;
    } else if (SDMutex == 0) {
        RestoreIRQMask(savedMask);
        return false;
    } else {
        RestoreIRQMask(savedMask);
    }
    return false;
}

void sleepdrv_release_mutex(void)
{
    int32_t savedMask;
    savedMask = SaveAndSetIRQMask();
    if (SDMutex == 0) {
        SDMutex++;
        RestoreIRQMask(savedMask);
    } else {
        RestoreIRQMask(savedMask);
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

void sleepdrv_release_sleep_lock(uint8_t handle_index)
{
    if (handle_index < MAX_SLEEP_HANDLE) {
        uint32_t _savedMask;
        _savedMask = SaveAndSetIRQMask();
        if (sleep_driver_handle_ref_count[handle_index] != 0) {
            sleep_driver_handle_ref_count[handle_index]--;
            if (sleep_driver_handle_ref_count[handle_index] == 0) {
                sleepdrv.sleepDisable &= ~(1 << handle_index);
            }
        } else {
            printf("lock has already released\n");
        }
        RestoreIRQMask(_savedMask);
    }
}

void sleepdrv_hold_sleep_lock(uint8_t handle_index)
{
    if (handle_index < MAX_SLEEP_HANDLE) {
        uint32_t _savedMask;
        _savedMask = SaveAndSetIRQMask();
        sleep_driver_handle_ref_count[handle_index]++;
        sleepdrv.sleepDisable |= (1 << handle_index);
        RestoreIRQMask(_savedMask);
    }
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
    _savedMask = SaveAndSetIRQMask();
    lock = sleepdrv.sleepDisable != 0 ? true : false;
    RestoreIRQMask(_savedMask);
    return lock;
}

uint32_t sleepdrv_get_lock_index(void)
{
    return sleepdrv.sleepDisable;
}

uint32_t sleepdrv_get_lock_module(void)
{
    int i;
    for (i = 0; i < MAX_SLEEP_HANDLE; i++) {
        if (sleepdrv.sleepDisable & (1 << i)) {
            printf("Lock index : %ld is locking\n", sleep_driver_handle_array[i]);
        }
    }
    return sleepdrv.sleepDisable;
}

void sleep_driver_mtcmos_control(hal_sys_topsm_mtcmos_enum_t mtcmos,bool config)
{
    sys_topsm_mtcmos_control(mtcmos, config);
}

void PSI_S0_Run_AT_LP(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xC0000000; // [27]=0 turn off sw_sta_hld. [31][30]=3 ,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON1 = (*PSI_MST_CON1 & 0xFE00FFFF) | (PSI_S0_LP_SETTLE_TIME << 16 ); // [24:16] : s0_lp_settle
    if(is_clk_use_lfosc()){
        *PSI_MST_CON1 = (*PSI_MST_CON1 & 0xFE00FFFF) | (1 << 16 ); // fast wakeup
    }
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x60; // [6] = 1 idle, [5]=1 SW enable
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            //*PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 & 0x69; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1; //[0]=1, enable
            *PSI_MST_S0 = *PSI_MST_S0 & 0xBF;
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0x9; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1; //[0]=1, enable
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S1 & 0x9; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S2 = *PSI_MST_S1 | 0x1; //[0]=1, enable
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0x9; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1; //[0]=1, enable
            break;
    }
}

void PSI_S0_RUN_AT_HP(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON1 = (*PSI_MST_CON1 & 0xFFFFFE00) | PSI_S0_HP_SETTLE_TIME; // [8:0] s0_hp_settle
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x60; // [6] = 1 idle, [5]=1 SW enable
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            //*PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x4;  //[2]=1 , enable hp_sel
            *PSI_MST_S0 = *PSI_MST_S0 & 0x6D; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1 , enable
            *PSI_MST_S0 = *PSI_MST_S0 & 0xBF;
            break;
        case 0x1:
            *PSI_MST_S1 = *PSI_MST_S1 | 0x4; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 & 0xD; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1; //[1], mask:idle

            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;  //[0]=1 , enable
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;  //[0]=1 , enable

            *PSI_MST_S2 = *PSI_MST_S2 | 0x4; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 & 0xD; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1; //[1], mask:idle
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;  //[0]=1 , enable

            *PSI_MST_S4 = *PSI_MST_S4 | 0x4; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 & 0xD; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1; //[1], mask:idle

            break;
    }
}

void PSI_S1_RUN_AT_LP(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON2 = (*PSI_MST_CON2 & 0xFE00FFFF) | PSI_S1_LP_SETTLE_TIME << 16; // [8:0] s1_hp_settle
    //*PSI_MST_CON2 = 0x01;
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x60; // [6] = 1 idle, [5]=1 SW enable
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;

            //*PSI_MST_S0 = *PSI_MST_S0 | 0x40;
            *PSI_MST_S0 = *PSI_MST_S0 & 0x69;
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;
            *PSI_MST_S0 = *PSI_MST_S0 & 0xBF; // [6] = 1 idle, [5]=1 SW enable
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2;
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;

            *PSI_MST_S1 = *PSI_MST_S1 & 0x9;
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2;
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;

            *PSI_MST_S2 = *PSI_MST_S2 & 0x9;
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2;
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2;

            *PSI_MST_S4 = *PSI_MST_S4 & 0x9;
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1;
            break;
    }
}

void PSI_S1_RUN_AT_HP(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xC0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON2 = (*PSI_MST_CON2 & 0xFFFFFE00) | PSI_S1_HP_SETTLE_TIME; // [8:0] s1_hp_settle
    //*PSI_MST_CON2 = 0x01;
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x60; // [6] = 1 idle, [5]=1 SW enable
            //*PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x4;  //[2]=1 , enable hp_sel
            *PSI_MST_S0 = *PSI_MST_S0 & 0x6D; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1 , enable
            *PSI_MST_S0 = *PSI_MST_S0 & 0xBF; // [6] = 1 idle, [5]=1 SW enable
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;  //[0]=1 , enable

            *PSI_MST_S1 = *PSI_MST_S1 | 0x400000004; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 & 0xD; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1; //[1], mask:idle

            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2;  //[0]=1 , enable

            *PSI_MST_S2 = *PSI_MST_S2 | 0x400000004; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 & 0xD; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1; //[1], mask:idle

            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2;  //[2]=1 , enable hp_sel
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2;  //[0]=1 , enable
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[4]=0 , disable force rdy [1]=0,idle_mask: let (non)idle in [6]=0,non-idle

            *PSI_MST_S4 = *PSI_MST_S4 | 0x400000004; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 & 0xD; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1; //[1], mask:idle

            break;
    }
}

void PSI_HP_RUN_AT_LP(const uint32_t master_id)
{
//  //printf("PSI_HP_RUN_AT_LP start\n");
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0xFFFF00FF) | (PSI_HP_LP_SETTLE_TIME << 8);  // [15:8] hp_lp_settle

    switch (master_id) {
        case 0x0:
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S0 = *PSI_MST_S0 & 0x69; //[4]=0 ,disable force rdy [2]=0,disable hp_sel, [1]=0 ,idle_mask: let (non) idle in
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1 , enable
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0x9; //[4]=0 ,disable force rdy [2]=0,disable hp_sel, [1]=0 ,idle_mask: let (non) idle in
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1;  //[0]=1 , enable
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S2 & 0x9; //[4]=0 ,disable force rdy [2]=0,disable hp_sel, [1]=0 ,idle_mask: let (non) idle in
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1;  //[0]=1 , enable
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0x9; //[4]=0 ,disable force rdy [2]=0,disable hp_sel, [1]=0 ,idle_mask: let (non) idle in
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1;  //[0]=1 , enable
            break;

    }
}

void PSI_LP_RUN_AT_HP(const uint32_t master_id)
{
//  //printf("PSI_LP_RUN_AT_HP start\n");
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0xFFFFFF00) | PSI_LP_HP_SETTLE_TIME; // [24:16] s1_lp_settle
    switch (master_id) {
        case 0x0:
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S0 = *PSI_MST_S0 & 0xD; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x4; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1, enable
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0xD; //[5]=1 SW
            *PSI_MST_S1 = *PSI_MST_S1 | 0x4; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1;  //[0]=1, enable
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S2 & 0xD; //[5]=1 SW
            *PSI_MST_S2 = *PSI_MST_S2 | 0x4; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1;  //[0]=1, enable
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0xD; //[5]=1 SW
            *PSI_MST_S4 = *PSI_MST_S4 | 0x4; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1;  //[0]=1, enable
            break;
    }
}

void PSI_SLEEP_AT_S0(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON3 = (*PSI_MST_CON3 & 0xFF00FFFF) | (PSI_T0_S0_SETTLE_TIME << 16); // [24:16] to_s0_settle
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x20; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 & 0x6D; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x9;  //[0]=1, enable

            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            if (mtcmos_control.BT_Count == 0) {
                *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            }

            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0xD; //[1], mask:idle
            if(is_clk_use_lfosc()){
            *PSI_MST_S1 = *PSI_MST_S1 | 0x10; //[1], mask:idle //fast wakeup
            }
            *PSI_MST_S1 = *PSI_MST_S1 | 0x9; //[1], mask:idle
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            if (mtcmos_control.BT_Count == 0) {
                *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            }
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S2 & 0xD; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x9; //[1], mask:idle
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0xD; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x9; //[1], mask:idle
            break;
    }
}

void PSI_SLEEP_AT_S1(const uint32_t master_id)
{
    *PSI_MST_CON0 = (*PSI_MST_CON0 & 0x7FFFFFF) | 0xc0000000; // [27]=0 turn off sw_sta_hid, [31][30]=3,sw_return_diff_en,sw_hp_sta_en
    *PSI_MST_CON3 = (*PSI_MST_CON3 & 0xFFFFFF00) | PSI_T0_S1_SETTLE_TIME; // [24:16] to_s0_settle
    switch (master_id) {
        case 0x0:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x40; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 | 0x20; //[5]=1 SW
            *PSI_MST_S0 = *PSI_MST_S0 & 0x65; //[4]=0 disable force rdy, [2]=0, enable hp_sel,[1]=0 idle_mask ; let (non)idle in [6]=0 , non-idle
            *PSI_MST_S0 = *PSI_MST_S0 | 0x1;  //[0]=1, enable

            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle
            break;
        case 0x1:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            if (mtcmos_control.BT_Count == 0) {
                *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            }
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S1 = *PSI_MST_S1 & 0x5; //[1], mask:idle
            if(is_clk_use_lfosc()){
            *PSI_MST_S1 = *PSI_MST_S1 | 0x10; //[1], mask:idle //fast wakeup
            }
            *PSI_MST_S1 = *PSI_MST_S1 | 0x1; //[1], mask:idle
            break;
        case 0x2:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            if (mtcmos_control.BT_Count == 0) {
                *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            }
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x2; //[1], mask:idle

            *PSI_MST_S2 = *PSI_MST_S2 & 0x5; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x1; //[1], mask:idle
            break;
        case 0x4:
            *PSI_MST_S0 = *PSI_MST_S0 | 0x2; //[1], mask:idle
            *PSI_MST_S1 = *PSI_MST_S1 | 0x2; //[1], mask:idle
            *PSI_MST_S2 = *PSI_MST_S2 | 0x2; //[1], mask:idle
            *PSI_MST_S3 = *PSI_MST_S3 | 0x2; //[1], mask:idle

            *PSI_MST_S4 = *PSI_MST_S4 & 0x5; //[1], mask:idle
            *PSI_MST_S4 = *PSI_MST_S4 | 0x1; //[1], mask:idle
            break;
    }
}
#endif /* HAL_SLEEP_MANAGER_ENABLED */
