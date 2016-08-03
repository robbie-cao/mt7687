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
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* #include "mlog.h" */

#include "hal_lp.h"
#include "wifi_ex.h"

#include "mt7687.h"
#include "connsys_driver.h"

#include "timer.h"
#include "top.h"
#include "system_mt7687.h"
#include "hal_sleep_driver.h"
#include "hal_sleep_manager.h"
#include "FreeRTOS.h"
#include "toi.h"

#define HAL_REG_32(reg)         (*((volatile unsigned int *)(reg)))

extern uint32_t hal_sleep_manager_sleep_driver_dump_handle_name(void);

void hal_ex_lp_gpt_isr(void)
{
}

static uint8_t _cli_deep_sleep(uint8_t len, char *param[])
{
    /* Setup timer as wakeup source */
    /* TMR0 is usesd by iperf */
    drvTMR_init(TMR1, (atoi(param[0]) * 32768), 0, hal_ex_lp_gpt_isr);//1
    TMR_Stop(TMR0);
    TMR_Start(TMR1);

    /* Enable FW_OWN_BACK_INT interrupt */
    hal_lp_connsys_get_own_enable_int();

    /* Give connsys ownership to N9 */
    hal_lp_connsys_give_n9_own();

    /* Switch flash clock to XTAL */
    cmnSerialFlashClkConfToXtal();

    /* Switch MCU clock to XTAL */
    cmnCpuClkConfigureToXtal();

    /* Unclaim PLL usage */
    cmnPLL1OFF_PLL2OFF();

    hal_lp_deep_sleep();

    return 0;
}


static uint8_t _cli_legacy_sleep(uint8_t len, char *param[])
{
    /* Setup timer as wakeup source */
    /* TMR0 is usesd by iperf */
    drvTMR_init(TMR1, (atoi(param[0]) * 32768), 0, hal_ex_lp_gpt_isr);//1

    /* disable TMR0 which trigger a 30sec timer due to tickless */
    TMR_Stop(TMR0);
    
    TMR_Start(TMR1);

    /* Enable FW_OWN_BACK_INT interrupt */
    hal_lp_connsys_get_own_enable_int();

    /* Give connsys ownership to N9 */
    hal_lp_connsys_give_n9_own();

    /* Switch flash clock to XTAL */
    cmnSerialFlashClkConfToXtal();

    /* Switch MCU clock to XTAL */
    cmnCpuClkConfigureToXtal();

    /* Unclaim PLL usage */
    cmnPLL1OFF_PLL2OFF();

    hal_lp_legacy_sleep();

    /* After wakeup from legacy sleep */
    /* Enable MCU clock to 192MHz */
    cmnCpuClkConfigureTo192M();

    /* Enable flash clock to 64MHz */
    cmnSerialFlashClkConfTo64M();

    /* re-init connsys for handling inband-cmd response */
    connsys_open();

    TMR_Start(TMR0);
    return 0;

}

#if configUSE_TICKLESS_IDLE == 2
extern uint8_t tickless_handle_index;
uint32_t ticklessCount = 0;
uint32_t ticklessTryCount = 0;
int ticklessMode = 1;
#endif

static uint8_t _cli_tickless_mode(uint8_t len, char *param[])
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
#if configUSE_TICKLESS_IDLE == 2
    if (tickless_handle_index == 0xFF)
        tickless_handle_index = hal_sleep_manager_set_sleep_handle("tickless");

#ifdef configTICKLESS_DYNAMIC_ENABLE
    ticklessMode = atoi(param[0]);
    ticklessCount = 0;
    ticklessTryCount = 0;
#endif
#endif
#endif
    return 0;
}

static uint8_t _cli_tickless_status(uint8_t len, char *param[])
{
#if configUSE_TICKLESS_IDLE == 2
    printf("tickless:\n");
    printf("\tentered %u times\n", (unsigned int)ticklessCount);
    printf("\ttried   %u times\n", (unsigned int)ticklessTryCount);
    printf("\t%s\n", ticklessMode ? "enabled" : "disabled");
    if (ticklessMode == 1) {
        printf("\tmode: WFI\n");
    } else if (ticklessMode == 2) {
        printf("\tmode: Legacy Sleep\n");
    }
    printf("\t%s\n", hal_sleep_manager_is_sleep_locked() ? "locked" : "not locked");
    if (hal_sleep_manager_is_sleep_locked()) {
        hal_sleep_manager_sleep_driver_dump_handle_name();
    }
#endif

    return 0;
}

cmd_t lp_cli[] = {
    { "ds",              "deep sleep",                    _cli_deep_sleep      },
    { "ls",              "legacy sleep",                  _cli_legacy_sleep    },
    { "tick",            "tickless enable",               _cli_tickless_mode   },
    { "status",          "tickless status",               _cli_tickless_status },
    { NULL }
};

