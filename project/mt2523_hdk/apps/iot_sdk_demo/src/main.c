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
#include "hal_mipi_tx_config_internal.h"
#include "hal_nvic.h"
#include "hal_dcxo.h"

#include "hal_sleep_manager.h"
//#include "bt_error_solution.h"

/*bt spp include */
#include "bt_spp.h"
#include "bt_common_dispatch.h"

#include "nvdm.h"
#include "hal_flash.h"

#ifdef MTK_SMART_BATTERY_ENABLE
#include "hal_charger.h"
#include "hal_sleep_manager.h"
#include "hal_gpt.h"
#include "cust_battery_meter.h"
/* battery management includes */
#include "battery_management.h"
#endif

#include "audio_middleware_api.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


#include "bsp_gpio_ept_config.h"
#include "memory_map.h"
#include "hal_display_lcd.h"


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

int BT_XFile_EncryptionCommand()
{
    return 0;
}

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 and Loop until the end of transmission */
    hal_uart_put_char( HAL_UART_0, ch);
    if (ch == '\n') {
        hal_uart_put_char(HAL_UART_0, '\r');
    }
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

uint32_t caculate_mpu_region_size( uint32_t region_size )
{
    uint32_t count;

    if (region_size < 32) {
        return 0;
    }
    for (count = 0; ((region_size  & 0x80000000) == 0); count++, region_size  <<= 1);
    return 30 - count;
}

static void mpu_init(void)
{
    hal_mpu_region_t region, region_number;
    hal_mpu_region_config_t region_config;
    typedef struct {
        uint32_t mpu_region_base_address;			/**< MPU region start address */
        uint32_t mpu_region_end_address;			/**< MPU region end address */
        hal_mpu_access_permission_t mpu_region_access_permission; /**< MPU region access permission */
        uint8_t mpu_subregion_mask; 			/**< MPU sub region mask*/
        bool mpu_xn;					/**< XN attribute of MPU, if set TRUE, execution of an instruction fetched from the corresponding region is not permitted */
    } mpu_region_information_t;

#if defined (__GNUC__) || defined (__CC_ARM)

    //RAM: VECTOR TABLE+RAM CODE+RO DATA
    extern uint32_t Image$$RAM_TEXT$$Base;
    extern uint32_t Image$$RAM_TEXT$$Limit;
    //TCM: TCM CODE+RO DATA
    extern uint32_t Image$$TCM$$RO$$Base;
    extern uint32_t Image$$TCM$$RO$$Limit;

    /* MAX region number is 8 */
    mpu_region_information_t region_information[] = {
        /* mpu_region_start_address, mpu_region_end_address, mpu_region_access_permission, mpu_subregion_mask, mpu_xn */
        {(uint32_t) &Image$$RAM_TEXT$$Base,  (uint32_t) &Image$$RAM_TEXT$$Limit, HAL_MPU_READONLY, 0x0, FALSE},//Vector table+RAM code+RAM rodata
        {(uint32_t) &Image$$RAM_TEXT$$Base + VRAM_BASE,  (uint32_t) &Image$$RAM_TEXT$$Limit + VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE},//Virtual memory
        {(uint32_t) &Image$$TCM$$RO$$Base, (uint32_t) &Image$$TCM$$RO$$Limit, HAL_MPU_READONLY, 0x0, FALSE}//TCM code+TCM rodata
    };

#elif defined (__ICCARM__)

#pragma section = ".intvec"
#pragma section = ".ram_rodata"
#pragma section = ".tcm_code"
#pragma section = ".tcm_rwdata"

    /* MAX region number is 8, please DO NOT modify memory attribute of this structure! */
    _Pragma("location=\".ram_rodata\"") static mpu_region_information_t region_information[] = {
        /* mpu_region_start_address, mpu_region_end_address, mpu_region_access_permission, mpu_subregion_mask, mpu_xn */
        {(uint32_t)__section_begin(".intvec"), (uint32_t)__section_end(".ram_rodata"), HAL_MPU_READONLY, 0x0, FALSE},//Vector table+RAM code+RAM rodata
        {(uint32_t)__section_begin(".intvec") + VRAM_BASE, (uint32_t)__section_end(".ram_rodata") + VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
        {(uint32_t)__section_begin(".tcm_code"), (uint32_t)__section_begin(".tcm_rwdata"), HAL_MPU_READONLY, 0x0, FALSE}//TCM code+TCM rodata
    };

#endif

    hal_mpu_config_t mpu_config = {
        /* PRIVDEFENA, HFNMIENA */
        TRUE, TRUE
    };

    region_number = (hal_mpu_region_t) (sizeof(region_information) / sizeof(region_information[0]));

    hal_mpu_init(&mpu_config);
    for (region = HAL_MPU_REGION_0; region < region_number; region++) {
        /* Updata region information to be configured */
        region_config.mpu_region_address = region_information[region].mpu_region_base_address;
        region_config.mpu_region_size = (hal_mpu_region_size_t) caculate_mpu_region_size(region_information[region].mpu_region_end_address - region_information[region].mpu_region_base_address);
        region_config.mpu_region_access_permission = region_information[region].mpu_region_access_permission;
        region_config.mpu_subregion_mask = region_information[region].mpu_subregion_mask;
        region_config.mpu_xn = region_information[region].mpu_xn;

        hal_mpu_region_configure(region, &region_config);
        hal_mpu_region_enable(region);
    }
    /* make sure unused regions are disabled */
    for ( ; region < HAL_MPU_REGION_MAX; region++) {
        hal_mpu_region_disable(region);
    }
    hal_mpu_enable();
}

static void mipi_power_init(void)
{
    MIPITX_CONFIG_REGISTER_T *mipi_tx_config_register_ptr = (MIPITX_CONFIG_REGISTER_T *)(MIPI_TX_CFG_BASE);

    /* MIPI analog module power off */
    mipi_tx_config_register_ptr->mipitx_con6_register.field.RG_LNT_BGR_EN = 0;
}

#if !defined (MTK_DEBUG_LEVEL_NONE)

LOG_CONTROL_BLOCK_DECLARE(atci);
#ifdef MTK_BT_AT_COMMAND_ENABLE
LOG_CONTROL_BLOCK_DECLARE(atci_bt);
#endif
LOG_CONTROL_BLOCK_DECLARE(atci_charger);
#ifdef MTK_CTP_ENABLE
LOG_CONTROL_BLOCK_DECLARE(atci_ctp);
#endif
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
#ifdef MTK_SMART_BATTERY_ENABLE
LOG_CONTROL_BLOCK_DECLARE(bmt);
#endif

log_control_block_t *syslog_control_blocks[] = {
    &LOG_CONTROL_BLOCK_SYMBOL(atci),
#ifdef MTK_BT_AT_COMMAND_ENABLE
    &LOG_CONTROL_BLOCK_SYMBOL(atci_bt),
#endif    
    &LOG_CONTROL_BLOCK_SYMBOL(atci_charger),
#ifdef MTK_CTP_ENABLE
    &LOG_CONTROL_BLOCK_SYMBOL(atci_ctp),
#endif
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
#ifdef MTK_SMART_BATTERY_ENABLE
    &LOG_CONTROL_BLOCK_SYMBOL(bmt),
#endif

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

#ifdef MTK_SMART_BATTERY_ENABLE
static void check_battery_voltage(void)
{
    /* Check power on battery voltage and pre-charging battery sample code */
    hal_charger_init();
    while (1) {
        int32_t battery_voltage = 0;
        bool charger_status;

        hal_charger_meter_get_battery_voltage_sense(&battery_voltage);
        LOG_I(common, "Check battery_voltage = %d mV", battery_voltage);
        /* Check battery voltage  > SHUTDOWN_SYSTEM_VOLTAGE (3400mV)  + 100mV */
        if (battery_voltage >= SHUTDOWN_SYSTEM_VOLTAGE + 100) {
            break;
        } else {
            hal_charger_get_charger_detect_status(&charger_status);
            if (charger_status == true) {
                LOG_I(common, "SW charging battery_voltage = %d mV", battery_voltage);
                hal_charger_init();
                /* Setup pre-charging current. It depends on the battery specifications */
                hal_charger_set_charging_current(HAL_CHARGE_CURRENT_70_MA);
                hal_charger_enable(true);
                /* Reset watchdog timer */
                hal_charger_reset_watchdog_timer();
                hal_gpt_delay_ms(1 * 1000);
            } else {
                LOG_I(common, "Low battery power off !! battery_voltage = %d mV", battery_voltage);
                hal_sleep_manager_enter_power_off_mode();
            }

        }
    }

}
#endif

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

    /* init UART earlier */
    log_uart_init(HAL_UART_0);

    /* Configure the hardware ready to run the test. */
    prvSetupHardware();

    hal_sleep_manager_init();

    clock_dump_info();

    nvdm_init();

    log_init(syslog_config_save, syslog_config_load, syslog_control_blocks);

#ifdef MTK_SMART_BATTERY_ENABLE
    /* Check battery voltage */
    check_battery_voltage();

    battery_management_init();
#endif

#ifdef MTK_SENSOR_BIO_USE_MT2511
	/*turn on VA28 for mt2511*/
    PMIC_VR_CONTROL(PMIC_VA28   , PMIC_VR_CTL_ENABLE);
#endif


    audio_middleware_init();

    task_def_init();

    task_def_create();

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}

static void prvSetupHardware( void )
{
    /* system HW init */
    cache_init();
    mpu_init();

    /* peripherals init */
    hal_flash_init();

    hal_nvic_init();

    bsp_ept_gpio_setting_init();

    hal_rtc_init();
    hal_dcxo_init();

    mipi_power_init();
}

static void SystemClock_Config(void)
{
    hal_clock_init();

    hal_display_lcd_set_clock(HAL_DISPLAY_LCD_INTERFACE_CLOCK_124MHZ);

    hal_dvfs_init();
    hal_dvfs_target_cpu_frequency(target_freq, HAL_DVFS_FREQ_RELATION_H);
}

