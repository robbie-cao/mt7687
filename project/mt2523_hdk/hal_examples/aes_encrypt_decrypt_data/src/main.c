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

/* Log the data in the format of 16 bytes per line */
void aes_result_dump(uint8_t *result, uint8_t length)
{
    uint8_t i;

    for (i = 0; i < length; i++) {
        if (i % 16 == 0) {
            log_hal_info("\r\n");
        }

        log_hal_info(" %02x ", result[i]);
    }
    log_hal_info("\r\n");

}

/* Example of AES encryption/decryption including CBC and ECB modes */
void aes_encrypt_decrypt_data_example(void)
{
    uint8_t hardware_id[16] = {
        0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30,
        0x32, 0x30, 0x31, 0x34, 0x30, 0x38, 0x31, 0x35
    };
    uint8_t aes_cbc_iv[HAL_AES_CBC_IV_LENGTH] = {
        0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43,
        0x77, 0x68, 0x36, 0x33, 0x50, 0x76, 0x33, 0x46
    };
    uint8_t plain[] = {
        0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55,
        66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
    };

    uint8_t encrypted_buffer[32] = {0};
    uint8_t decrypted_buffer[32] = {0};

    hal_aes_buffer_t plain_text = {
        .buffer = plain,
        .length = sizeof(plain)
    };
    hal_aes_buffer_t key = {
        .buffer = hardware_id,
        .length = sizeof(hardware_id)
    };

    hal_aes_buffer_t encrypted_text = {
        .buffer = encrypted_buffer,
        .length = sizeof(encrypted_buffer)
    };

    log_hal_info("Origin data:");
    aes_result_dump(plain_text.buffer, plain_text.length);
    log_hal_info("aes_cbc_iv:");
    aes_result_dump(aes_cbc_iv, HAL_AES_CBC_IV_LENGTH);
    log_hal_info("Key:");
    aes_result_dump(key.buffer, key.length);

    hal_aes_cbc_encrypt(&encrypted_text, &plain_text, &key, aes_cbc_iv);

    log_hal_info("Encrypted data(AES CBC):");
    aes_result_dump(encrypted_text.buffer, encrypted_text.length);

    hal_aes_buffer_t decrypted_text = {
        .buffer = decrypted_buffer,
        .length = sizeof(plain) /* If using sizeof(decrypted_buffer) as the length, padding bits will be read back */
    };
    hal_aes_cbc_decrypt(&decrypted_text, &encrypted_text, &key, aes_cbc_iv);
    log_hal_info("Decrypted data(AES CBC):");
    aes_result_dump(decrypted_text.buffer, decrypted_text.length);

    hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
    log_hal_info("Encrypted data(AES ECB):");
    aes_result_dump(encrypted_text.buffer, encrypted_text.length);

    hal_aes_ecb_decrypt(&decrypted_text, &encrypted_text, &key);
    log_hal_info("Decrypted data(AES ECB):");
    aes_result_dump(decrypted_text.buffer, decrypted_text.length);
}

/* main function */
int main(void)
{
    /* Configure system clock */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware */
    prvSetupHardware();

    /* Add your application code here */
    log_hal_info("\r\n\r\n");/* The output UART used by log_hal_info is set by log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("\r\n\r\n");

    aes_encrypt_decrypt_data_example();

    for (;;);
}

