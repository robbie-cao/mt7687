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
static uint8_t param[7] = {15, /*year*/
                           11, /*month*/
                           06, /*day*/
                           5,  /*day of week*/
                           15, /*hour*/
                           26, /*minute*/
                           15  /*second*/
                          };

static uint8_t alarm_param[7] = {15, /*year*/
                                 11, /*month*/
                                 06, /*day*/
                                 5,  /*day of week*/
                                 15, /*hour*/
                                 26, /*minute*/
                                 25  /*second*/
                                };

volatile uint8_t test_rtc_callback = 0;
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
static void plain_log_uart_init(void)
{
    hal_uart_config_t uart_config;
    /* Set Pinmux to UART */
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_UART1_RTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_UART1_CTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);

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
    /* System HW initialization */

    /* Peripherals initialization */
    plain_log_uart_init();

    /* Board HW initialization */
}



static void test_rtc_alarm_callback(void *parameter)
{
    test_rtc_callback = 1;
}

static void rtc_clear_time(hal_rtc_time_t *rtc_time)
{
    rtc_time->rtc_year = 0;
    rtc_time->rtc_mon = 0;
    rtc_time->rtc_day = 0;
    rtc_time->rtc_week = 0;
    rtc_time->rtc_hour = 0;
    rtc_time->rtc_min = 0;
    rtc_time->rtc_sec = 0;
}

static void rtc_fill_time(hal_rtc_time_t *rtc_time, uint8_t param[])
{
    rtc_time->rtc_year = param[0];
    rtc_time->rtc_mon = param[1];
    rtc_time->rtc_day = param[2];
    rtc_time->rtc_week = param[3];
    rtc_time->rtc_hour = param[4];
    rtc_time->rtc_min = param[5];
    rtc_time->rtc_sec = param[6];
}



void rtc_set_alarm_example()
{
    hal_rtc_time_t rtc_time;

    log_hal_info("---RTC example begin---\r\n");

    /* Initialize the RTC module */
    hal_rtc_init();

    /* Test set time */
    rtc_fill_time(&rtc_time, param);
    hal_rtc_set_time(&rtc_time);
    log_hal_info("Set RTC time: 20%d,%d,%d (%d) %d:%d:%d\n", rtc_time.rtc_year,
                 rtc_time.rtc_mon,
                 rtc_time.rtc_day,
                 rtc_time.rtc_week,
                 rtc_time.rtc_hour,
                 rtc_time.rtc_min,
                 rtc_time.rtc_sec);

    /* Test alarm(set and enable) */
    rtc_clear_time(&rtc_time);
    rtc_fill_time(&rtc_time, alarm_param);
    hal_rtc_set_alarm(&rtc_time);
    log_hal_info("Set RTC alarm time: 20%d,%d,%d (%d) %d:%d:%d\n", rtc_time.rtc_year,
                 rtc_time.rtc_mon,
                 rtc_time.rtc_day,
                 rtc_time.rtc_week,
                 rtc_time.rtc_hour,
                 rtc_time.rtc_min,
                 rtc_time.rtc_sec);

    hal_rtc_enable_alarm();
    log_hal_info("RTC alarm enabled\n");
    hal_rtc_set_alarm_callback(test_rtc_alarm_callback, NULL);
    /* Wait for the alarm. */
    while(test_rtc_callback == 0);
    log_hal_info("RTC alarm test pass\n");

    /* Alarm fired. Verify the current time. */
    rtc_clear_time(&rtc_time);
    hal_rtc_get_time(&rtc_time);
    log_hal_info("RTC current time: 20%d,%d,%d (%d) %d:%d:%d\n", rtc_time.rtc_year,
                 rtc_time.rtc_mon,
                 rtc_time.rtc_day,
                 rtc_time.rtc_week,
                 rtc_time.rtc_hour,
                 rtc_time.rtc_min,
                 rtc_time.rtc_sec);

    log_hal_info("---RTC example end---\r\n");
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
    log_hal_info("\r\n\r\n");
    
    /* The output UART used by log_hal_info is set by log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("\r\n\r\n");

    rtc_set_alarm_example();

    for ( ;; );
}


/*-----------------------------------------------------------*/


