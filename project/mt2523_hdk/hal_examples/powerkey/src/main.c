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

#include "keypad_custom.h"


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
void log_uart_init(void)
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
    log_uart_init();
    hal_flash_init();
    hal_nvic_init();
}

static void keypad_user_powerkey_handler(void)
{
    hal_keypad_status_t         ret;
    hal_keypad_powerkey_event_t powekey_event;
    char *string[5] = {"release", "press", "longpress", "repeat", "pmu_longpress"};

    while (1) {
        ret = hal_keypad_powerkey_get_key(&powekey_event);

        /*If an error occurs, there is no key in the buffer*/
        if (ret == HAL_KEYPAD_STATUS_ERROR) {
            log_hal_info("[keypad][test]powerkey no key in buffer\r\n\r\n");
            break;
        }

        log_hal_info("[keypad][test]powerkey data:[%d], state:[%s]\r\n", (int)powekey_event.key_data, (char *)string[powekey_event.state]);
    }
}

/*Use powerkey*/
void hal_powerkey_example(void)
{
    bool                ret_bool;
    hal_keypad_status_t ret_state;


    /*Initialize powerkey*/
    ret_bool = keypad_custom_powerkey_init();
    if (ret_bool == false) {
        log_hal_info("[keypad][test]keypad_custom_init init failed\r\n");
        return;
    }

    ret_state = hal_keypad_powerkey_register_callback((hal_keypad_callback_t)keypad_user_powerkey_handler, NULL);
    if (ret_state != HAL_KEYPAD_STATUS_OK) {
        log_hal_info("[keypad][test]hal_keypad_powerkey_register_callback failed, state = %d\r\n", ret_state);
    }
}


int main(void)
{
    /* Configure system clock */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware */
    prvSetupHardware();

    /* Enable NVIC interrupts and fault exception*/
    __enable_irq();
    __enable_fault_irq();

    log_hal_info("\r\n\r\n");/* The output UART used by log_hal_info is set by log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("\r\n\r\n");

    hal_powerkey_example();

    while (1);
}

