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
#include "top.h"

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

static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    log_uart_init();

}

/* Display the data in the format of 16 bytes each line */
void des_result_dump(uint8_t *result, uint8_t length)
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

/* Example of DES encryption/decryption including CBC and ECB modes */
void des_encrypt_decrypt_data_example(void)
{
    uint8_t plain[30] = {
        0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 0, 11, 22, 33, 44, 55, 66,
        77, 88, 99, 0, 11, 22, 33, 44, 55, 66, 77, 88, 99
    };
    uint8_t hardware_id[8] = {
        0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30
    };
    uint8_t des_cbc_iv[HAL_DES_CBC_IV_LENGTH] = {
        0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43
    };
    uint8_t encrypted[32] = {0};
    uint8_t decrypted[32] = {0};

    hal_des_buffer_t encrypted_text = {
        .buffer = encrypted,
        .length = sizeof(encrypted)
    };
    hal_des_buffer_t origin_text = {
        .buffer = plain,
        .length = sizeof(plain)
    };
    hal_des_buffer_t key = {
        .buffer = hardware_id,
        .length = sizeof(hardware_id)
    };

    log_hal_info("Origin:");
    des_result_dump(origin_text.buffer, origin_text.length);
    log_hal_info("Key:");
    des_result_dump(key.buffer, key.length);
    log_hal_info("IV:");
    des_result_dump(des_cbc_iv, sizeof(des_cbc_iv));

    hal_des_cbc_encrypt(&encrypted_text,
                        &origin_text,
                        &key,
                        des_cbc_iv);
    log_hal_info("Encrypted(DES CBC):");
    des_result_dump(encrypted_text.buffer, encrypted_text.length);

    hal_des_buffer_t decrypted_text = {
        .buffer = decrypted,
        .length = sizeof(decrypted)
    };
    hal_des_cbc_decrypt(&decrypted_text,
                        &encrypted_text,
                        &key,
                        des_cbc_iv);
    log_hal_info("Decrypted(DES CBC):");
    des_result_dump(decrypted_text.buffer, decrypted_text.length);


    hal_des_ecb_encrypt(&encrypted_text,
                        &origin_text,
                        &key);
    log_hal_info("Encrypted(DES ECB):");
    des_result_dump(encrypted_text.buffer, encrypted_text.length);

    hal_des_ecb_decrypt(&decrypted_text,
                        &encrypted_text,
                        &key);
    log_hal_info("Decrypted(DES ECB):");
    des_result_dump(decrypted_text.buffer, decrypted_text.length);
}

/* main function */
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

    des_encrypt_decrypt_data_example();

    for (;;);
}

