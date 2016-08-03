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

/* Private variables ---------------------------------------------------------*/

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

static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    plain_log_uart_init();
    hal_flash_init();
    hal_nvic_init();
}

/* Macro of debounce time, uint is millisecond */
#define HOOK_KEY_DEBOUNCE_TIME 300
#define PLUG_IN_DEBOUNCE_TIME  500
#define PLUG_OUT_DEBOUNCE_TIME 50

/* Debounce time of hook-key/plug-in/plug-out */
static hal_accdet_debounce_time_t debounce_time[2] = {
    {HOOK_KEY_DEBOUNCE_TIME, PLUG_IN_DEBOUNCE_TIME, PLUG_OUT_DEBOUNCE_TIME}, //Before plug in
    {HOOK_KEY_DEBOUNCE_TIME, PLUG_IN_DEBOUNCE_TIME >> 2, PLUG_OUT_DEBOUNCE_TIME} //After plug in
};

static void accdet_callback(hal_accdet_callback_event_t event, void *user_data)
{
    switch (event) {
        case HAL_ACCDET_EVENT_HOOK_KEY:
            hal_accdet_set_debounce_time(&debounce_time[1]);
            log_hal_info("\r\nHook Key Event!\r\n");
            break;
        case HAL_ACCDET_EVENT_PLUG_IN:
            hal_accdet_set_debounce_time(&debounce_time[1]);
            log_hal_info("\r\nPlug In Event!\r\n");
            break;
        case HAL_ACCDET_EVENT_PLUG_OUT:
            hal_accdet_set_debounce_time(&debounce_time[0]);
            log_hal_info("\r\nPlug Out Event!\r\n");
            break;
        default:
            log_hal_info("\r\nWrong Event!!!\r\n");
            break;
    }
}

/* EINT number of GPIO pin used for ACCDET, this global variable will be used by ACCDET
   driver and should be defined and initialized correctly if EPT tool isn't used.
*/
uint32_t HAL_ACCDET_EINT = 8;

/* ACCDET earphone status detection example */
void accdet_detect_earphone_status_example(void)
{
    uint32_t user_data;

    log_hal_info("\r\n ---accdet_example_begin---");

    hal_gpio_init(HAL_GPIO_9);

    /* Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool was not used to configure the related pinmux */
    hal_pinmux_set_function(HAL_GPIO_9, HAL_GPIO_9_EINT8);

    /* Disable pull of the corresponding GPIO */
    hal_gpio_set_pupd_register(HAL_GPIO_9, 0, 0, 0);
    hal_gpio_deinit(HAL_GPIO_9);

    hal_accdet_init();//ACCDET module init
    hal_accdet_set_debounce_time(&debounce_time[0]);
    hal_accdet_register_callback(accdet_callback, (void *)&user_data);//Register callback function
    hal_accdet_enable();//Enable ACCDET

    log_hal_info("\r\n ---accdet_example_end!!!---");
    log_hal_info("\r\n ---please insert earphone and see the log!!!---\r\n");
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

    accdet_detect_earphone_status_example();

    while (1);
}

