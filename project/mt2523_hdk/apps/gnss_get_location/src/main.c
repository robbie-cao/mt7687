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
#include "queue.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal_cache.h"
#include "hal_mpu.h"
#include "hal_uart.h"
#include "hal_audio.h"
#include "hal_clock.h"
#include "hal_rtc.h"
#include "hal_dvfs.h"

#include "hal_sys_topsm.h"
#include "hal_dsp_topsm.h"

#include "hal_pdma_internal.h"

#include "bsp_gpio_ept_config.h"
#include "nvdm.h"
#include "hal_flash.h"
/* hal includes */
#include "bt_gap.h"
#include "bt_log.h"
#include "bt_notify_test.h"
#include "bt_spp.h"
#include "ble_dogp_adp_service.h"
#include "hal_sleep_manager.h"

#include "syslog.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

#include "memory_map.h"

extern void ME_SetLocalDeviceName(char* device_name, int len);

log_create_module(fota_dl_m, PRINT_LEVEL_INFO);



/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware( void );

void vApplicationTickHook(void)
{

}
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    log_putchar(ch);
    return ch;
}

#define ATCI_DEMO
#define NVDM_DEMO

#ifdef NVDM_DEMO
#include "nvdm.h"
#endif

static uint8_t console_cmd[100];
static uint8_t cmd_received;
void bt_console_cmd(const char * cmd);
#if !defined (MTK_DEBUG_LEVEL_NONE)
LOG_CONTROL_BLOCK_DECLARE(a2dp);
LOG_CONTROL_BLOCK_DECLARE(avrcp);
LOG_CONTROL_BLOCK_DECLARE(common);
LOG_CONTROL_BLOCK_DECLARE(DOGP_ADP);
LOG_CONTROL_BLOCK_DECLARE(fota_dl_m);
LOG_CONTROL_BLOCK_DECLARE(GATTS_SRV);
LOG_CONTROL_BLOCK_DECLARE(GNSS_TAG);
LOG_CONTROL_BLOCK_DECLARE(hal);

log_control_block_t *syslog_control_blocks[] =
{
   &LOG_CONTROL_BLOCK_SYMBOL(a2dp),
   &LOG_CONTROL_BLOCK_SYMBOL(avrcp),
   &LOG_CONTROL_BLOCK_SYMBOL(common),
   &LOG_CONTROL_BLOCK_SYMBOL(DOGP_ADP),
   &LOG_CONTROL_BLOCK_SYMBOL(fota_dl_m),
   &LOG_CONTROL_BLOCK_SYMBOL(GATTS_SRV),
   &LOG_CONTROL_BLOCK_SYMBOL(GNSS_TAG),
   &LOG_CONTROL_BLOCK_SYMBOL(hal),
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
        /* Popuplate the syslog nvdm with the image setting */
        syslog_config_save(config);
    }
    return 0;
}
#endif /* MTK_DEBUG_LEVEL_NONE */

void bt_task(void *arg)
{

    while(1)
    {
        if (cmd_received == 1)
        {
            printf("Receive command\n");
            bt_console_cmd((char*)console_cmd);
            console_cmd[0] = '\0';
            cmd_received = 0;
        }

        vTaskDelay(100);
    }
}

static void bt_console_print_help()
{
    printf("MediaTek Bluetooth on MT7687\n"
           " h  Help\n"
           " i  EDR search\n"
           " N  set dev name and EDR visible(test only)\n"
           " LE set LE visible(test only)\n"
           );
}


void bt_console_cmd(const char * cmd)
{
    printf("[main]bt_console_cmd (enter)cmd[0]: %x,cmd[1]: %x\n",cmd[0],cmd[1]);
    if (cmd[0] == 'h') {
        bt_console_print_help();
    } else if (cmd[0] == 'p'&& cmd[1] == 'o') {
        bt_gap_power_on();
    } else if (cmd[0] == 'p'&& cmd[1] == 'f') {
        bt_gap_power_off();
    } else if (cmd[0] == 'i') {
#ifndef BLE_ONLY_MODE
    bt_gap_inquiry(50, 8);
#endif
    } else if (cmd[0] == 'N') {
        ME_SetLocalDeviceName("MMMMMM Bluetooth", 16);
        bt_gap_set_scan_mode(BT_GAP_MODE_GENERAL_ACCESSIBLE);
    } else if (cmd[0] == 'L'&& cmd[1] == 'E') {
            uint8_t buff[31] = {2, 1, 0x1A};
            char* LE_name = "minmina";
            uint8_t len = 0;
            ble_address_t addr;
            addr.address[0] = 0xC0;
            addr.address[1] = 6;
            addr.address[2] = 7;
            addr.address[3] = 8;
            addr.address[4] = 9;
            addr.address[5] = 0xC4;
            printf("It's better to reset the BLE random addr and name in main.c(%d))\n", __LINE__);
            ble_gap_stop_advertiser();
            ble_gap_set_random_address(&addr);
            len = strlen(LE_name);
            buff[3] = len + 1;
            buff[4] = 9;
            memcpy(buff+5, LE_name, len);
            ble_gap_set_advertising_data(
                      buff,
                      len + 5,
                      NULL,
                      0
                      );
            ble_gap_set_advertising_params(
                    0x100,
                    0x100,
                    BLE_GAP_ADVERTISING_CONNECTABLE_UNDIRECTED,
                    BLE_ADDRESS_TYPE_RANDOM,
                    BLE_ADDRESS_TYPE_PUBLIC,
                    NULL,
                    0x7,
                    0
                    );
            ble_gap_start_advertiser();

    }

}
void cmd_task(void *arg)
{
    //uint8_t cmd[100];
    uint8_t revByte;
    uint8_t index = 0;
    cmd_received = 0;
    while(1)
    {
        revByte = hal_uart_get_char(HAL_UART_0);
        if (cmd_received == 0)
        {
            if (revByte == 0xd)
            {
                index = 0;
                printf("\n");
                cmd_received = 1;
            }
            else
            {
                console_cmd[index] = revByte;
                printf("%c", revByte);
                index++;
            }
        }
    }
}
void BTBMDumpBdAddr(uint8_t *addr)
{
    log_bt_info("Random Address %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",
                addr[5], addr[4], addr[3], addr[2],
                addr[1], addr[0]);
}
void bt_parm_trace(uint8_t len, uint8_t *parms)
{
    log_bt_info("[TODO] Parameter Trace\n");
}

int BleServiceDbInitialize()
{
    log_bt_info("[TODO] BleServiceDbInitialize should be implemented\n");
    return 0;
}

void bt_gap_common_callback(bt_event_t event_id, void *param)
{
    log_bt_info("Receive event %d", event_id);
    switch (event_id) {
        case BT_GAP_IO_CAPABILITY_REQUEST_IND:
            bt_gap_reply_io_capability_request((bt_address_t *)param, BT_GAP_IO_NO_INPUT_NO_OUTPUT, true, BT_GAP_BONDING_MODE_DEDICATED, false, NULL, NULL);
            break;
        case BT_GAP_USER_CONFIRM_REQUEST_IND:
            bt_gap_reply_user_confirm_request((bt_address_t *)param, true);
            break;
        case BT_GAP_POWER_ON_CNF:
            log_bt_info("POWER ON CNF");
            bt_gap_set_scan_mode(BT_GAP_MODE_GENERAL_ACCESSIBLE);
            break;
        default:
            break;
    }
}

bt_status_t bt_gap_get_local_address(bt_address_t *local_addr) {
    local_addr->address[0] = 0x22;
    local_addr->address[1] = 0x33;
    local_addr->address[2] = 0x44;
    local_addr->address[3] = 0x55;
    local_addr->address[4] = 0x66;
    local_addr->address[5] = 0x77;
    return BT_STATUS_SUCCESS;
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

#ifdef __ICCARM__
__weak void tool_init()
#else
__attribute__((weak)) void tool_init()
#endif
{

}

extern void gnss_demo_main(void);



#if 0 //!defined (MTK_DEBUG_LEVEL_NONE)
LOG_CONTROL_BLOCK_DECLARE(GNSS_TAG);
#ifdef TOOL_LOG_MODULE
LOG_CONTROL_BLOCK_DECLARE(TOOL_LOG_MODULE);
#endif

log_control_block_t *syslog_control_blocks[] =
{
   &LOG_CONTROL_BLOCK_SYMBOL(GNSS_TAG),
#ifdef TOOL_LOG_MODULE
   &LOG_CONTROL_BLOCK_SYMBOL(TOOL_LOG_MODULE),
#endif
   NULL
};
#endif /* MTK_DEBUG_LEVEL_NONE */


extern void epo_download_task(void *arg);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    //int idx;

    TaskHandle_t xCreatedTask0;
    //TaskHandle_t xCreatedTask1;
    TaskHandle_t xCreatedTask2;

    /* SystemClock Config */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware ready to run the test. */
    prvSetupHardware();

    log_uart_init(HAL_UART_0);
    printf("test printf0\n");
#ifdef NVDM_DEMO
    nvdm_init();
#endif
    log_init(syslog_config_save, syslog_config_load, syslog_control_blocks);
    LOG_I(hal, "enter main!!\n\r");
    /* Output a message on Hyperterminal using printf function */
    printf("test printf1\n");

    xTaskCreate(bt_task, "bt_task", 2048, NULL, 1, &xCreatedTask0);
    xTaskCreate(epo_download_task, "epo_download_task", 1024, NULL, 1, &xCreatedTask2);
    gnss_demo_main();
    printf("test printf2\n");
	
    bt_gap_power_on();
    ME_SetLocalDeviceName("2523_FOTATEST", 13);
    bt_gap_set_scan_mode(BT_GAP_MODE_GENERAL_ACCESSIBLE);

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for( ;; );
}


/*-----------------------------------------------------------*/
#define mainCHECK_DELAY  ( ( portTickType ) 1000 / portTICK_RATE_MS )

static void prvSetupHardware( void )
{
    /* System HW init */
    cache_init();
    mpu_init();

    /* Peripherals init */
    hal_flash_init();  /* flash init */

    hal_nvic_init();  /* nvic init */

    bsp_ept_gpio_setting_init();

    hal_sleep_manager_init();

    hal_rtc_init();
}

static void SystemClock_Config(void)
{
    hal_clock_init();
    hal_dvfs_init();
}
int BT_XFile_EncryptionCommand()
{
    return 0;
}


