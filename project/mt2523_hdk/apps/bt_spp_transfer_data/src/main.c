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

/*bt spp example includes*/
#include "bt_spp_main.h"
#include "bt_gap.h"

/* device.h includes */
#include "mt2523.h"
/* hal includes */
#include "hal_log.h"
#include "hal_cache.h"
#include "hal_mpu.h"
#include "hal_uart.h"
#include "hal_clock.h"
#include "hal_flash.h"
#include "hal_sys_topsm.h"
#include "hal_pdma_internal.h"

#include "bsp_gpio_ept_config.h"
#include "memory_attribute.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware( void );

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
bool hci_log_enabled(void)
{
    return true;
}

unsigned char  g_uart_rx_buff[1024];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static unsigned char    uart_rx_buff[1024];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static unsigned char    uart_tx_buff[1024];

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
        hal_uart_receive_dma(HAL_UART_1, g_uart_rx_buff, length);
    }
}


void bt_hci_uart_init(hal_uart_port_t port)
{
    hal_uart_config_t uart_config;
    hal_uart_dma_config_t dma_config;


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
        dma_config.receive_vfifo_buffer = uart_rx_buff;
        dma_config.receive_vfifo_buffer_size = 1024;
        dma_config.receive_vfifo_threshold_size = 128;
        dma_config.send_vfifo_buffer = uart_tx_buff;
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


    bt_status_t result;
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
    result = bt_gap_power_on();

	if(result == BT_STATUS_SUCCESS) {
        bt_gap_set_scan_mode(BT_GAP_MODE_GENERAL_ACCESSIBLE);
		spp_server_enable();

	}

    bt_spp_main();

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







