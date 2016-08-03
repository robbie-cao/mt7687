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
static void prvSetupHardware( void );
extern void top_xtal_init(void);

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
    /* Set Pinmux to UART */
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_UART1_RTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_UART1_CTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);

    /* COM port settings */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;
    hal_uart_init(HAL_UART_0, &uart_config);
}

static void SystemClock_Config(void)
{
    top_xtal_init();
}

static void prvSetupHardware( void )
{
    /* Peripherals initialization */
    log_uart_init();
}

static uint32_t get_current_milisecond(void)
{
    hal_gpt_status_t ret;
    uint32_t count = 0;    
    ret = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    
    if ( ret != HAL_GPT_STATUS_OK) {
        log_hal_info("hal_gpt_get_free_run_count return = %d error!\r\n", ret);
    }
    
    return (uint32_t)(((uint64_t)((uint64_t)count * 1000)) / 32768);
}

void gpt_callback(uint32_t *parameter)
{
    log_hal_info("Expired at %d ms\r\n", (int)get_current_milisecond());
    
}

void hal_gpt_interrupt_repeat_mode_example(void)
{
    hal_gpt_status_t         ret;
    hal_gpt_port_t           gpt_port;
    uint32_t                 time;
    
    gpt_port = HAL_GPT_0;
    time     = 1000;
    
    /* Initilizes GPT channel */
    ret = hal_gpt_init(gpt_port);
    if ( ret != HAL_GPT_STATUS_OK) {
        log_hal_info("hal_gpt_init return = %d [error]!\r\n", ret);
        return;
    }
    
    /* Register callback */    
    ret = hal_gpt_register_callback(gpt_port, (hal_gpt_callback_t)gpt_callback, NULL);
    if ( ret != HAL_GPT_STATUS_OK) {
        log_hal_info("hal_gpt_register_callback return = %d error!\r\n", ret);
        hal_gpt_deinit(gpt_port);
        return;
    }
    
    /* Set GPT timer type and start GPT */  
    ret = hal_gpt_start_timer_ms(gpt_port,  time, HAL_GPT_TIMER_TYPE_REPEAT);
    if ( ret != HAL_GPT_STATUS_OK) {
        log_hal_info("hal_gpt_start_timer_ms return = %d error!\r\n", ret);
    }
        
    log_hal_info("Start timer at %d ms.\r\n", (int)get_current_milisecond());

}

int main(void)
{
    /* Configure system clock */
    SystemClock_Config();

    /* Configure the hardware */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    log_hal_info("\r\n\r\n");/* The output UART used by log_hal_info is set by log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("\r\n\r\n");

    hal_gpt_interrupt_repeat_mode_example();

    while(1);
}


