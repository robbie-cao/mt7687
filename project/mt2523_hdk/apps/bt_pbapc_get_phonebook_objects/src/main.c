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

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal_cache.h"
#include "hal_mpu.h"
#include "hal_uart.h"
#include "hal_clock.h"
#include "hal_flash.h"

#include "hal_sys_topsm.h"
#include "hal_pdma_internal.h"
#include "bsp_gpio_ept_config.h"

#include "bt_log.h"
#include "memory_map.h"
#include "semphr.h"
#include "memory_attribute.h"
#include "app_pbapc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware( void );
extern int32_t cache_enable(hal_cache_size_t cache_size);

bool hci_log_enabled(void)
{
    return TRUE;
}
#define VFIFO_SIZE_RX_BT (1024)
#define VFIFO_SIZE_TX_BT (1024)
#define VFIFO_ALERT_LENGTH_BT (20)


/* Private variables ---------------------------------------------------------*/

static hal_uart_port_t      g_atci_bt_port;
static SemaphoreHandle_t    g_bt_io_semaphore = NULL;

static uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN g_bt_cmd_tx_vfifo[VFIFO_SIZE_TX_BT];
static uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN g_bt_cmd_rx_vfifo[VFIFO_SIZE_RX_BT];
void bt_io_uart_irq_ex(hal_uart_callback_event_t event, void *parameter)
{
    BaseType_t  x_higher_priority_task_woken = pdFALSE;

    LOG_I(app_pbapc, "event = %d, semaphore_HD = 0x%x", event, g_bt_io_semaphore);
    if (HAL_UART_EVENT_READY_TO_READ == event) {
        xSemaphoreGiveFromISR(g_bt_io_semaphore, &x_higher_priority_task_woken);
        portYIELD_FROM_ISR( x_higher_priority_task_woken );
    }

}

static void bt_at_cmd_trix_space(char *cmd_buf)
{
    char *token = NULL;

    // remove '\n';
    token = strrchr(cmd_buf, '\n');
    while (token != NULL) {
        *token = '\0';
        token = strrchr(cmd_buf, '\n');
    }

    // remove '\r';
    token = strrchr(cmd_buf, '\r');
    while (token != NULL) {
        *token = '\0';
        token = strrchr(cmd_buf, '\r');
    }

}

hal_uart_status_t bt_io_uart_init_ex(hal_uart_port_t port)
{
    hal_uart_status_t ret;
    /* Configure UART PORT */
    hal_uart_config_t uart_config = {
        .baudrate = HAL_UART_BAUDRATE_115200,
        .word_length = HAL_UART_WORD_LENGTH_8,
        .stop_bit = HAL_UART_STOP_BIT_1,
        .parity = HAL_UART_PARITY_NONE
    };
    g_atci_bt_port = port;
    ret = hal_uart_init(port, &uart_config);

    if (HAL_UART_STATUS_OK == ret) {

        hal_uart_dma_config_t   dma_config = {
            .send_vfifo_buffer              = g_bt_cmd_tx_vfifo,
            .send_vfifo_buffer_size         = VFIFO_SIZE_TX_BT,
            .send_vfifo_threshold_size      = 128,
            .receive_vfifo_buffer           = g_bt_cmd_rx_vfifo,
            .receive_vfifo_buffer_size      = VFIFO_SIZE_RX_BT,
            .receive_vfifo_threshold_size   = 128,
            .receive_vfifo_alert_size       = VFIFO_ALERT_LENGTH_BT
        };

        g_bt_io_semaphore = xSemaphoreCreateBinary();

        ret = hal_uart_set_dma(port, &dma_config);

        ret = hal_uart_register_callback(port, bt_io_uart_irq_ex, NULL);

    }
    return ret;
}

void vApplicationTickHook(void)
{

}

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch,FILE *f)
#endif
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  log_putchar(ch);
  return ch;
}


int BT_XFile_EncryptionCommand()
{
    return 0;
}

/* bluetooth includes */
#include "bt_gap.h"
#include "ble_gap.h"

//static uint8_t console_cmd[100];
//static uint8_t cmd_received;


bt_status_t bt_gap_get_local_name(char *name)
{
    strcpy(name, "BtPBAPC Testdev");
    return BT_STATUS_SUCCESS;
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

            log_bt_info("POWER ON CNF,open ble adv");
            {
                uint8_t buff[31] = {2, 1, 0x1A};
                char *LE_name = "btPBAPC";
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
                memcpy(buff + 5, LE_name, len);
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
            break;
        default:
            break;
    }
}
#include "memory_map.h"

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


static void bt_at_cmd_processing(void)
{
    while (1) {
        uint32_t    read_len;

        read_len = hal_uart_get_available_receive_bytes(g_atci_bt_port);
        LOG_I(app_pbapc, "[PBAPC]read_len = %d\r\n", read_len);

        if (read_len > 0) {
            //uint32_t ret_len;

            hal_uart_receive_dma(g_atci_bt_port, g_bt_cmd_rx_vfifo, read_len);

            bt_at_cmd_trix_space((char *)g_bt_cmd_rx_vfifo);
            read_len = strlen((char *)g_bt_cmd_rx_vfifo);
            LOG_I(app_pbapc, "[PBAPC] command len: %d, CMD: %s", read_len, g_bt_cmd_rx_vfifo);
            pbapc_command_handler(g_bt_cmd_rx_vfifo);
            //return read_len;
            return;
        } else {
            xSemaphoreTake(g_bt_io_semaphore, portMAX_DELAY);
        }
    }

}


static void bt_cmd_task(void *param)
{
    while (1) {
        bt_at_cmd_processing();
    }
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    //int idx;

    /* SystemClock Config */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware ready to run the test. */
    prvSetupHardware();

    /* init sys log */
    log_uart_init(HAL_UART_0);
    log_init(NULL, NULL, NULL);

    bt_io_uart_init_ex(HAL_UART_1);
    xTaskCreate(bt_cmd_task, "ble_cmd_task", 360, NULL,  3, NULL);
    //power on bluetooth
    bt_gap_power_on();
    //bt_notify_test_task_init();

    app_pbapc_test_task_init();
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

    /* peripherals init */

    cache_init();
    hal_flash_init(); /* flash init */

    hal_nvic_init();  /* nvic init */


    bsp_ept_gpio_setting_init();
}

static void SystemClock_Config(void)
{
    hal_clock_init();
}
