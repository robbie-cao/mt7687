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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal_cache.h"
#include "hal_mpu.h"
#include "hal_uart.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_dvfs.h"
#include "hal_rtc.h"
#include "hal_nvic.h"
#include "hal_dcxo.h"

#include "hal_sleep_manager.h"
//#include "bt_error_solution.h"

/*bt spp include */
#include "bt_spp.h"
#include "bt_common_dispatch.h"
#include "hal_flash.h"
#include "nvdm.h"

extern void bt_atci_init(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


#include "bsp_gpio_ept_config.h"
#include "memory_map.h"

#ifdef MTK_SYSTEM_CLOCK_26M
    static const uint32_t target_freq = 26000;
#else
#ifdef MTK_SYSTEM_CLOCK_104M
    static const uint32_t target_freq = 104000;
#else
    static const uint32_t target_freq = 208000;
#endif
#endif


/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware( void );

void vApplicationTickHook(void)
{

}

int __io_putchar(int ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  log_putchar(ch);
  return ch;
}

static void cache_init(void)
{
    hal_cache_region_t region, region_number;

    /* Max region number is 16 */
    hal_cache_region_config_t region_cfg_tbl[] = {
        /* cacheable address, cacheable size(both MUST be 4k bytes aligned) */
#ifdef RTOS_UBIN_LENGTH
        {BL_BASE, BL_LENGTH + RTOS_LENGTH + RTOS_UBIN_LENGTH},
#else
        {BL_BASE, BL_LENGTH + RTOS_LENGTH},
#endif
        /* virtual memory */
        {VRAM_BASE, VRAM_LENGTH}
    };

    region_number = (hal_cache_region_t) (sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]));

    hal_cache_init();
    hal_cache_set_size(HAL_CACHE_SIZE_32KB);
    for (region = HAL_CACHE_REGION_0; region < region_number; region++) {
        hal_cache_region_config(region, &region_cfg_tbl[region]);
        hal_cache_region_enable(region);
    }
    for ( ; region < HAL_CACHE_REGION_MAX; region++) {
        hal_cache_region_disable(region);
    }
    hal_cache_enable();
}


#if !defined (MTK_DEBUG_LEVEL_NONE)

LOG_CONTROL_BLOCK_DECLARE(atci);
LOG_CONTROL_BLOCK_DECLARE(atci_bt);
LOG_CONTROL_BLOCK_DECLARE(atci_charger);
LOG_CONTROL_BLOCK_DECLARE(atci_ctp);
LOG_CONTROL_BLOCK_DECLARE(atci_keypad);
LOG_CONTROL_BLOCK_DECLARE(atci_reg);
LOG_CONTROL_BLOCK_DECLARE(atcmd);
LOG_CONTROL_BLOCK_DECLARE(BLE_MESSAGE);
LOG_CONTROL_BLOCK_DECLARE(BLE_STATIC_CB);
LOG_CONTROL_BLOCK_DECLARE(bt);
LOG_CONTROL_BLOCK_DECLARE(bt_audio);
LOG_CONTROL_BLOCK_DECLARE(bt_sink);
LOG_CONTROL_BLOCK_DECLARE(bt_spp);
LOG_CONTROL_BLOCK_DECLARE(common);
LOG_CONTROL_BLOCK_DECLARE(GNSS_TAG);
LOG_CONTROL_BLOCK_DECLARE(GRAPHIC_TAG);
LOG_CONTROL_BLOCK_DECLARE(hal);
LOG_CONTROL_BLOCK_DECLARE(PXP_MAIN);
LOG_CONTROL_BLOCK_DECLARE(RTC_ATCI);
LOG_CONTROL_BLOCK_DECLARE(sensor);

log_control_block_t *syslog_control_blocks[] =
{
    &LOG_CONTROL_BLOCK_SYMBOL(atci),
    &LOG_CONTROL_BLOCK_SYMBOL(atci_bt),
    &LOG_CONTROL_BLOCK_SYMBOL(atci_charger),
    &LOG_CONTROL_BLOCK_SYMBOL(atci_ctp),
    &LOG_CONTROL_BLOCK_SYMBOL(atci_keypad),
    &LOG_CONTROL_BLOCK_SYMBOL(atci_reg),
    &LOG_CONTROL_BLOCK_SYMBOL(atcmd),
    &LOG_CONTROL_BLOCK_SYMBOL(BLE_MESSAGE),
    &LOG_CONTROL_BLOCK_SYMBOL(BLE_STATIC_CB),
    &LOG_CONTROL_BLOCK_SYMBOL(bt),
    &LOG_CONTROL_BLOCK_SYMBOL(bt_audio),
    &LOG_CONTROL_BLOCK_SYMBOL(bt_sink),
    &LOG_CONTROL_BLOCK_SYMBOL(bt_spp),
    &LOG_CONTROL_BLOCK_SYMBOL(common),
    &LOG_CONTROL_BLOCK_SYMBOL(GNSS_TAG),
    &LOG_CONTROL_BLOCK_SYMBOL(GRAPHIC_TAG),
    &LOG_CONTROL_BLOCK_SYMBOL(hal),
    &LOG_CONTROL_BLOCK_SYMBOL(PXP_MAIN),
    &LOG_CONTROL_BLOCK_SYMBOL(RTC_ATCI),
    &LOG_CONTROL_BLOCK_SYMBOL(sensor),
    NULL
};

static char syslog_filter_buf[SYSLOG_FILTER_LEN] = {0};

static void syslog_config_save(const syslog_config_t *config)
{
    nvdm_status_t status;

    syslog_convert_filter_val2str((const log_control_block_t **)config->filters, syslog_filter_buf);
    status = nvdm_write_data_item("common",
                                  "syslog_filters",
                                  NVDM_DATA_ITEM_TYPE_STRING,
                                  (const uint8_t *)syslog_filter_buf,
                                  strlen(syslog_filter_buf));
    LOG_I(common, "syslog config save, status=%d", status);
}

static uint32_t syslog_config_load(syslog_config_t *config)
{
    uint32_t sz = SYSLOG_FILTER_LEN;

    if (nvdm_read_data_item("common", "syslog_filters", (uint8_t*)syslog_filter_buf, &sz) == NVDM_STATUS_OK) {
        syslog_convert_filter_str2val(config->filters, syslog_filter_buf);
    } else {
        /* popuplate the syslog nvdm with the image setting */
        syslog_config_save(config);
    }
    return 0;
}

#endif /* MTK_DEBUG_LEVEL_NONE */

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* SystemClock Config */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware ready to run the test. */
    prvSetupHardware();

    log_uart_init(HAL_UART_0);

    hal_sleep_manager_init();

    hal_dcxo_init();

    clock_dump_info();

    nvdm_init();

    log_init(syslog_config_save, syslog_config_load, syslog_control_blocks);

    task_def_init();

    //task_def_create();

    bt_atci_init();
    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for( ;; );
}

static void prvSetupHardware( void )
{
    /* system HW init */
    cache_init();

    /* peripherals init */
    hal_flash_init(); /* flash init */

    hal_nvic_init();  /* nvic init */

    //sys_topsm_init();

    bsp_ept_gpio_setting_init();

//#ifdef BT_IT
//    io_def_uart_init();
//#endif

    hal_rtc_init();

    *((volatile uint32_t *) (0xA2090040))&= 0xFFFFFFFE;
}

static void SystemClock_Config(void)
{
    hal_clock_init();

    hal_dvfs_init();
    hal_dvfs_target_cpu_frequency(target_freq, HAL_DVFS_FREQ_RELATION_H);
}

void gnss_driver_deinit_coclock()
{
    *((volatile uint32_t *)(0xA2030348)) = 0x00000400;
}

