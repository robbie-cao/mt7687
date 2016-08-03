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
#include "hal_audio.h"
#include "hal_sys_topsm.h"
#include "hal_dsp_topsm.h"
#include "mt2523.h"
#include "hal_uart.h"
#include "hal_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static uint16_t audio_tone1k_16ksr[32] = {
0x0000, 0xe475, 0xcd1a, 0xbd80, 0xb806, 0xbd80, 0xcd1b, 0xe475,
0x0000, 0x1b8c, 0x32e5, 0x4281, 0x47fa, 0x4280, 0x32e6, 0x1b8b,
0x0000, 0xe475, 0xcd1b, 0xbd81, 0xb806, 0xbd80, 0xcd1b, 0xe474,
0x0001, 0x1b8c, 0x32e5, 0x4280, 0x47fb, 0x427f, 0x32e5, 0x1b8c
};

const char BSP_SPEAKER_EBABLE_PIN = 18;
const unsigned char HAL_MSDC_EINT = 108;
uint16_t hal_example_user_buffer[2048*2];
uint32_t hal_example_user_buf_read_ptr = 0;
uint16_t hal_example_internal_buffer[2048];

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

static void audio_test_fill_stream_out_buffer_2(void)
{
    int i;
    int temp = 0;
    for(i = 0 ; i< 4096 ; i+=2) {
        hal_example_user_buffer[i] = audio_tone1k_16ksr[temp];
        hal_example_user_buffer[i+1] = audio_tone1k_16ksr[temp];
        temp++;
        if(temp == 32) {
            temp = 0;
        }
    }
}

void hal_example_user_audio_stream_out_callback(hal_audio_event_t event, void *user_data)
{
    uint32_t sample_count;
    switch(event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:
           hal_audio_get_stream_out_sample_count(&sample_count);
           hal_audio_write_stream_out(hal_example_user_buffer+hal_example_user_buf_read_ptr, sample_count);
           hal_example_user_buf_read_ptr += (sample_count>>1);
           if (hal_example_user_buf_read_ptr>=2048) {
               hal_example_user_buf_read_ptr -= 2048;
           }
           break;
     }
}

void hal_audio_example()
{
    log_hal_info("\r\nAudio test case begin\r\n");
    hal_audio_init();
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HEADSET);
    audio_test_fill_stream_out_buffer_2();
    hal_audio_register_stream_out_callback(hal_example_user_audio_stream_out_callback, NULL);
    hal_audio_set_memory(&hal_example_internal_buffer);
    hal_audio_write_stream_out(hal_example_user_buffer, 4096);
    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
    hal_gpt_delay_ms(10000);
    hal_audio_stop_stream_out();
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
    log_hal_info("\r\n\r\n");/* The output UART used by log_hal_info is set by log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("\r\n\r\n");

    hal_sleep_manager_init();
    hal_audio_example();
    while (1);
}

