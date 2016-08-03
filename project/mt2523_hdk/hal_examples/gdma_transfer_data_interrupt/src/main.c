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

/* hal includes */
#include "hal.h"

/*memory attributes*/
#include "memory_attribute.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint32_t dma_length = 64;
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  source_data[64];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t  destination_data[64];


/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware( void );

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 one at a time */
    hal_uart_put_char(HAL_UART_0, ch);
    return ch;
}

/* UART hardware initialization for log output */
static void plain_log_uart_init(void)
{
    hal_uart_config_t uart_config;

    /* COM port settings */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;
    hal_uart_init(HAL_UART_0, &uart_config);
}

static void SystemClock_Config(void)
{
    hal_clock_init();
}

static void prvSetupHardware( void )
{
    /* System HW initialization */

    /* Peripherals initialization */
    plain_log_uart_init();
    hal_flash_init();
    hal_nvic_init();

    /* Board HW initialization */
}

static void gdma_callback(uint32_t user_data)
{
  /*gdma_callback will be triggered when gdma transfers finished.*/
    uint32_t count = 0;

    hal_gdma_stop(HAL_GDMA_CHANNEL_0);

    hal_gdma_deinit(HAL_GDMA_CHANNEL_0);

    for (count = 0; count < dma_length; count++) {
        if (destination_data[count] != source_data[count]) {
            log_hal_info("GDMA destination data is not match with source data.\n");
        }

    }
    log_hal_info("\r\n ---gdma_example finished!!!---\r\n");

}
/* Example of GDMA transfer data with interrupt mode */
void gdma_transfer_data_interrupt()
{
    uint32_t count = 0;

    log_hal_info("\r\n ---gdma_example begin---\r\n");

    for (count = 0; count < dma_length; count++) {
        source_data[count] = count;
    }

    for (count = 0; count < dma_length; count++) {
        destination_data[count] = 0x5A;
    }

    hal_gdma_init(HAL_GDMA_CHANNEL_0);

    hal_gdma_register_callback(HAL_GDMA_CHANNEL_0, (hal_gdma_callback_t)gdma_callback, (void *)1);

    hal_gdma_start_interrupt(HAL_GDMA_CHANNEL_0, (uint32_t)destination_data, (uint32_t)source_data, dma_length);


}

int main(void)
{
    /* Configure system clock */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    log_hal_info("\r\n\r\n");
   /* The output UART used by log_hal_info is set by plain_log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("\r\n\r\n");

    gdma_transfer_data_interrupt();

    while (1);
}

