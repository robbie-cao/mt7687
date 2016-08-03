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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define BLOCK_NUMBER  (4)

/* Private variables ---------------------------------------------------------*/
const unsigned char HAL_MSDC_EINT = 19; /*this variable is defined in ept_eint_var.c*/


uint32_t  sd_read_mutil_buffer[BLOCK_NUMBER * 512 / 4];
uint32_t  sd_write_mutil_buffer[BLOCK_NUMBER * 512 / 4];


/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware(void);

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
void plain_log_uart_init(void)
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

static void prvSetupHardware(void)
{
    /* System HW initialization */

    /* Peripherals initialization */
    plain_log_uart_init();
    hal_flash_init();
    hal_nvic_init();

    /* Board HW initialization */
}

void sd_read_write_data_polling_example(void)
{
    hal_sd_config_t sd_cfg = {HAL_SD_BUS_WIDTH_4, 13000};
    uint32_t read_block_number = 1;
    uint32_t write_block_number = 1;
    uint32_t i;
    uint32_t retry = 0;

    /* set pinmux first */
    /* Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool was not used to configure the related pinmux */
    hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_MC0_CK);// MC0_CK
    hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_MC0_CM0);// MC0_CM0
    hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_MC0_DA0);// MC0_DA0
    hal_pinmux_set_function(HAL_GPIO_33, HAL_GPIO_33_MC0_DA1);// MC0_DA1
    hal_pinmux_set_function(HAL_GPIO_34, HAL_GPIO_34_MC0_DA2);// MC0_DA2
    hal_pinmux_set_function(HAL_GPIO_35, HAL_GPIO_35_MC0_DA3);// MC0_DA3

    /*init sd*/
    while (retry < 3) {
        if (HAL_SD_STATUS_OK != hal_sd_init(HAL_SD_PORT_0, &sd_cfg)) {
            retry++;
        } else {
            break;
        }
    }

    if (3 <= retry) {
        log_hal_error("SD init fail!\r\n");
    }

    /*init write buffer*/
    for (i = 0; i < 128; i++) {
        sd_write_mutil_buffer[i] = 0xA5A5A5A5;
    }

    /*write  with MCU mode*/
    retry = 0;
    while (retry < 3) {
        if (HAL_SD_STATUS_OK != hal_sd_write_blocks(HAL_SD_PORT_0, sd_write_mutil_buffer, 0, write_block_number)) {
            if (HAL_SD_STATUS_OK == hal_sd_init(HAL_SD_PORT_0, &sd_cfg)) {
                retry++;
            } else {
                log_hal_error("SD write (MCU mode) failed!\r\n");
                break;
            }
        } else {
            break;
        }
    }

    if (3 <= retry) {
        log_hal_error("SD write (MCU mode) failed!\r\n");
    }

    /*read with MCU mode*/
    retry = 0;
    while (retry < 3) {
        if (HAL_SD_STATUS_OK != hal_sd_read_blocks(HAL_SD_PORT_0, sd_read_mutil_buffer, 0, read_block_number)) {
            if (HAL_SD_STATUS_OK == hal_sd_init(HAL_SD_PORT_0, &sd_cfg)) {
                retry++;
            } else {
                log_hal_error("SD read (MCU mode) failed!\r\n");
                break;
            }
        } else {
            break;
        }
    }

    if (3 <= retry) {
        log_hal_error("SD read (MCU mode) failed!\r\n");
    }

    /*compare data*/
    for (i = 0; i < 128; i++) {
        if (sd_read_mutil_buffer[i] != 0xA5A5A5A5) {
            log_hal_error("SD data comparison failed!\r\n");
            log_hal_info("\r\n");
            return;
        }
    }

    log_hal_info("SD read write data polling example test ok!");
    log_hal_info("\r\n");
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
    log_hal_info("\r\n\r\n");/* The output UART used by log_hal_info is set by plain_log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("\r\n\r\n");

    sd_read_write_data_polling_example();

    while (1);
}

