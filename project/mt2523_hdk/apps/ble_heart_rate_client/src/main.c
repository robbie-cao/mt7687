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

/**
  ******************************************************************************
  * @file    main.c
  * @author  WCN IoT Team
  * @version V0.1.0
  * @date    01-May-2015
  * @brief   This is entry point of IoT system.
  *
  ******************************************************************************
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

#include "bt_log.h"
#include "memory_map.h"
#include "semphr.h"
#include "bsp_gpio_ept_config.h"
#include "memory_attribute.h"
#include "hal_nvic.h"
#include "bt_gap.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLE_UART1_IO     /*defines if hci and bt command can be used*/
/* Private macro -------------------------------------------------------------*/
#ifdef BLE_UART1_IO
#define VFIFO_SIZE_RX_BT (512)
#define VFIFO_SIZE_TX_BT (1024)
#define VFIFO_ALERT_LENGTH_BT (20)

/* Private variables ---------------------------------------------------------*/
static uint8_t  ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN g_bt_cmd_tx_vfifo[VFIFO_SIZE_TX_BT];
static uint8_t  ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN g_bt_cmd_rx_vfifo[VFIFO_SIZE_RX_BT];

static hal_uart_port_t      g_atci_bt_port;
static SemaphoreHandle_t    g_bt_io_semaphore = NULL;
#endif
/* Private functions ---------------------------------------------------------*/
//static void SystemClock_Config(void);
//static void prvSetupHardware( void );

bool hci_log_enabled(void)
{
    return TRUE;
}


void vApplicationTickHook(void)
{

}


extern void bt_hr_cmd_hdlr(uint8_t *cmd);
extern void heart_rate_task_init(void);

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


#ifdef BLE_UART1_IO
static void ble_at_cmd_trix_space(char *cmd_buf)
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

static void ble_at_cmd_processing(void)
{
    while (1) {
        uint32_t    read_len;

        read_len = hal_uart_get_available_receive_bytes(g_atci_bt_port);
        LOG_I(app_hrc, "[BT_CMD]read_len = %d\r\n", read_len);

        if (read_len > 0) {
            //uint32_t ret_len;

            hal_uart_receive_dma(g_atci_bt_port, g_bt_cmd_rx_vfifo, read_len);

            ble_at_cmd_trix_space((char *)g_bt_cmd_rx_vfifo);
            read_len = strlen((char *)g_bt_cmd_rx_vfifo);
            LOG_I(app_hrc, "[BT_CMD] command len: %d, CMD: %s", read_len, g_bt_cmd_rx_vfifo);
            bt_hr_cmd_hdlr(g_bt_cmd_rx_vfifo);
            //return read_len;
            return;
        } else {
            xSemaphoreTake(g_bt_io_semaphore, portMAX_DELAY);
        }
    }

}

static void ble_cmd_task(void *param)
{
    while (1) {
        ble_at_cmd_processing();
    }
}

static void ble_io_uart_irq(hal_uart_callback_event_t event, void *parameter)
{

    BaseType_t  x_higher_priority_task_woken = pdFALSE;

    LOG_I(app_hrc, "event = %d, semaphore_HD = 0x%x", event, g_bt_io_semaphore);
    if (HAL_UART_EVENT_READY_TO_READ == event) {
        xSemaphoreGiveFromISR(g_bt_io_semaphore, &x_higher_priority_task_woken);
        portYIELD_FROM_ISR( x_higher_priority_task_woken );
    }
}

static hal_uart_status_t ble_io_uart_init(hal_uart_port_t port)
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

    LOG_I(app_hrc, "UART INIT!!!");

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

        ret = hal_uart_register_callback(port, ble_io_uart_irq, NULL);

    }
    return ret;
}
#endif

static void prvSetupHardware( void )
{


    /* system HW init */

    /* peripherals init */

    cache_init();
    hal_flash_init(); /* flash init */

    hal_nvic_init();  /* nvic init */


    bsp_ept_gpio_setting_init();

    /*
    hal_sleep_manager_init();
    */

}

static void SystemClock_Config(void)
{
    hal_clock_init();
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

#ifdef BLE_UART1_IO
    ble_io_uart_init(HAL_UART_1);
    xTaskCreate(ble_cmd_task, "ble_cmd_task", 430, NULL,  3, NULL);
#endif

    //power on bluetooth
    bt_gap_power_on();

    heart_rate_task_init();


    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}

