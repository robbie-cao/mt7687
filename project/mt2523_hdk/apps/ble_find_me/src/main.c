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
#include "hal_uart.h"
#include "hal_log.h"
#include "hal_flash.h"
//#include "bt_log.h"
//#include "ble_bds_app_util.h"
#include "ble_fmp_main.h"
#include "ble_message.h"
//#include "hal_sleep_manager.h"
#include "memory_attribute.h"
#include "bsp_gpio_ept_config.h"
#include "bt_gap.h"
#include "hal_nvic.h"
#include "hal_clock.h"

/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware( void );
//static void vTestTask( void *pvParameters );


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

#if 0
void __aeabi_assert(const char* expr, const char* file, int Line){
	  while(1);
}
#endif

void ble_app_handle_message(ble_message_context_struct *message)
{
    if (message->message_id >= BLE_MSG_GATT_EXCHANGE_MTU_REQ_IND && message->message_id <= BLE_MSG_GATTS_HANDLE_VALUE_IND) {
        ble_gatts_message_handler((ble_gatts_message_id_t)(message->message_id), message->parameter);
    } else if (message->message_id >= BLE_MSG_GAP_POWER_ON_IND && message->message_id <= BLE_MSG_GAP_DISCONNECTED_IND) {
     ble_gap_message_handler((ble_gap_message_id_t)(message->message_id), message->parameter);
    }
}

void app_task(void *arg)
{
    ble_message_context_struct message;
    //ble_fmp_main();
    ble_message_create_queue();
    while(1) {
        if (ble_message_receive(&message)) {               
            ble_app_handle_message(&message);
            ble_message_free_message_buffer(&message);
        }
    }
}


bool hci_log_enabled(void)
{
    return true;
}

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN char    uart_rx_buff[1024];
char    g_uart_rx_buff[1024];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN char    uart_tx_buff[1024];

void bt_hci_uart_irq(hal_uart_callback_event_t event, void *parameter)
{
    uint32_t length = 0;

    log_hal_info("bt_hci_uart_irq event = %d\n", event);

    length = hal_uart_get_available_receive_bytes(HAL_UART_1);
    if (HAL_UART_EVENT_READY_TO_READ == event) {
        length = hal_uart_get_available_receive_bytes(HAL_UART_1);
        if (length > 1024) {
            length = 1024;
        }
        hal_uart_receive_dma(HAL_UART_1, (uint8_t *)(g_uart_rx_buff), length);
    }
}


void bt_hci_uart_init(hal_uart_port_t port)
{
    hal_uart_config_t uart_config;
    hal_uart_dma_config_t dma_config;

    //LOGW("atci_uart_init \r\n");

    #if 0
    hal_gpio_init(HAL_GPIO_30);
    hal_pinmux_set_function(HAL_GPIO_30, 4);
    hal_gpio_init(HAL_GPIO_31);
    hal_pinmux_set_function(HAL_GPIO_31, 4);
    #endif

    /* Configure UART PORT */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    log_hal_info("bt hci uart init\n");

    printf("call hal_uart_init,port=%d \r\n", port);
    if (HAL_UART_STATUS_OK != hal_uart_init(port, &uart_config)) {
        // log: uart init success
        printf("hal_uart_init fail\r\n");
    } else {
        // log: uart init fail 
        printf("hal_uart_init success \n");
        dma_config.receive_vfifo_alert_size = 50;
        dma_config.receive_vfifo_buffer = (uint8_t *)uart_rx_buff;
        dma_config.receive_vfifo_buffer_size = 1024;
        dma_config.receive_vfifo_threshold_size = 128;
        dma_config.send_vfifo_buffer = (uint8_t *)uart_tx_buff;
        dma_config.send_vfifo_buffer_size = 1024;
        dma_config.send_vfifo_threshold_size = 51;

        hal_uart_set_dma(port, &dma_config);
        hal_uart_register_callback(port, bt_hci_uart_irq, NULL);
    }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  //int idx=0;

    /* SystemClock Config */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware ready to run the test. */
    prvSetupHardware();

    log_uart_init(HAL_UART_0);
    log_init(NULL, NULL, NULL);

    /* Output a message on Hyperterminal using printf function */
    log_hal_info("FreeRTOS Running\n");

    bt_hci_uart_init(HAL_UART_1);

    bt_gap_power_on();
    
    xTaskCreate(app_task, "app_task", 512, NULL, 1, NULL);
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

static void prvSetupHardware( void )
{

    /* peripherals init */
    hal_flash_init(); /* flash init */

    hal_nvic_init();  /* nvic init */

    bsp_ept_gpio_setting_init();
}

static void SystemClock_Config(void)
{
    hal_clock_init();
}







