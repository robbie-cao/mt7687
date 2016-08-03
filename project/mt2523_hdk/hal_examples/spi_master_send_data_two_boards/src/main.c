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

void spi_master_send_data_two_boards_example()
{
    uint32_t frequency = 6000000;
    uint32_t i;
    uint8_t cfg_wr_cmd[9] = {0x04, 0x00, 0x00, 0xaa, 0x55, 0x0f, 0x00, 0x00, 0x00};
    uint8_t wr_buffer[17] = {0};
    for (i = 1; i < 17; i++) {
        wr_buffer[i] = 0x50 + i;
    }
    wr_buffer[0] = 0x06;
    uint8_t status_cmd = 0x0a;
    uint8_t poweron_cmd = 0x0e;
    uint8_t poweroff_cmd = 0x0c;
    uint8_t status_receive[2] = {0};

    hal_spi_master_config_t spi_config;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    hal_spi_master_advanced_config_t advanced_config;
    log_hal_info("---spi_master_send_data_two_boards_example begins---\r\n");

    hal_gpio_init(HAL_GPIO_25);
    hal_gpio_init(HAL_GPIO_26);
    hal_gpio_init(HAL_GPIO_27);
    hal_gpio_init(HAL_GPIO_28);

    /* GPIO configuration for SPI master channel 3 */
    hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_MA_SPI3_A_CS);
    hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_MA_SPI3_A_SCK);
    hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_MA_SPI3_A_MOSI);
    hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_MA_SPI3_A_MISO);

    /* Initializes  SPI master */
    spi_config.bit_order = HAL_SPI_MASTER_LSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = frequency;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(HAL_SPI_MASTER_3, &spi_config)) {
        log_hal_info("SPI master init failed\r\n");
    }

    /* SPI master advanced configuration */
    advanced_config.byte_order = HAL_SPI_MASTER_LITTLE_ENDIAN;
    advanced_config.chip_polarity = HAL_SPI_MASTER_CHIP_SELECT_LOW;
    advanced_config.sample_select = HAL_SPI_MASTER_SAMPLE_POSITIVE;
    /* User may need to try different get_tick settings for timing tolerance when high SCK frequency is used */
    advanced_config.get_tick = HAL_SPI_MASTER_GET_TICK_DELAY1;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_set_advanced_config(HAL_SPI_MASTER_3, &advanced_config)) {
        log_hal_info("SPI master advanced configuration failed \r\n");
    }

    log_hal_info("Master starts to send power_on to slave \r\n");
    while ((status_receive[1] & (uint8_t)SPISLV_STATUS_SLV_ON_MASK) != (uint8_t)SPISLV_STATUS_SLV_ON_MASK) {
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(HAL_SPI_MASTER_3, &poweron_cmd, 1)) {
            log_hal_info("SPI master send polling failed \r\n");
        } else {
            log_hal_info("SPI master is sending data \r\n");
        }

        status_receive[1] = 0;
        spi_send_and_receive_config.receive_length = 2;
        spi_send_and_receive_config.send_length = 1;
        spi_send_and_receive_config.send_data = &status_cmd;
        spi_send_and_receive_config.receive_buffer = status_receive;
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_3, &spi_send_and_receive_config)) {
            log_hal_info("SPI master send_and_receive polling failed \r\n");
        }

        /* De-initializes SPI master after data transaction */
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(HAL_SPI_MASTER_3)) {
            log_hal_info("SPI master deinit failed\r\n");
        }

        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(HAL_SPI_MASTER_3, &spi_config)) {
            log_hal_info("SPI master init failed\r\n");
        }
        log_hal_info("status_receive[1]: 0x%x\r\n", status_receive[1]);
    }

    log_hal_info("Master starts to send cfg_wr_cmd to slave\n");
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(HAL_SPI_MASTER_3, cfg_wr_cmd, 9)) {
        log_hal_info("SPI master send polling failed \r\n");
    }
    hal_gpt_delay_ms(2000);
    status_receive[1] = 0;
    spi_send_and_receive_config.receive_length = 2;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.send_data = &status_cmd;
    spi_send_and_receive_config.receive_buffer = status_receive;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_3, &spi_send_and_receive_config)) {
        log_hal_info("SPI master send_and_receive polling failed \r\n");
    }
    if ((status_receive[1] & (uint8_t)SPISLV_STATUS_TXRX_FIFO_RDY_MASK) == (uint8_t)SPISLV_STATUS_TXRX_FIFO_RDY_MASK) {
        log_hal_info("Master sends cfg_wr to slave successfully \r\n");
    } else {
        log_hal_info("status_receive[1]: 0x%x\r\n", status_receive[1]);
    }
    /* De-initialize SPI master after data transaction */
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(HAL_SPI_MASTER_3)) {
        log_hal_info("SPI master deinit failed \r\n");
    }

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(HAL_SPI_MASTER_3, &spi_config)) {
        log_hal_info("SPI master init failed \r\n");
    }

    log_hal_info("Master start to send wr_cmd to slave\n");
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(HAL_SPI_MASTER_3, wr_buffer, 17)) {
        log_hal_info("\r\n SPI master send polling failed \r\n");
    }
    hal_gpt_delay_ms(2000);
    status_receive[1] = 0;
    spi_send_and_receive_config.receive_length = 2;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.send_data = &status_cmd;
    spi_send_and_receive_config.receive_buffer = status_receive;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_3, &spi_send_and_receive_config)) {
        log_hal_info("SPI master send_and_receive polling failed \r\n");
    }
    if ((status_receive[1] & (uint8_t)SPISLV_STATUS_RDWR_FINISH_MASK) == (uint8_t)SPISLV_STATUS_RDWR_FINISH_MASK) {
        log_hal_info("master send wr_cmd to slave successfully \r\n");
    } else {
        log_hal_info("status_receive[1]: 0x%x\r\n", status_receive[1]);
    }

    log_hal_info("master starts to send power_off to slave\n");
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(HAL_SPI_MASTER_3, &poweroff_cmd, 1)) {
        log_hal_info("SPI master send polling failed \r\n");
    }
    hal_gpt_delay_ms(2000);
    status_receive[1] = 0;
    spi_send_and_receive_config.receive_length = 2;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.send_data = &status_cmd;
    spi_send_and_receive_config.receive_buffer = status_receive;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_3, &spi_send_and_receive_config)) {
        log_hal_info("SPI master send_and_receive polling failed \r\n");
    }
    if ((status_receive[1] & (uint8_t)SPISLV_STATUS_SLV_ON_MASK) != (uint8_t)SPISLV_STATUS_SLV_ON_MASK) {
        log_hal_info("Master sends power_off to slave successfully \r\n");
    } else {
        log_hal_info("status_receive[1]: 0x%x\r\n", status_receive[1]);
    }

    /* Deinitialization SPI master after data transaction */
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(HAL_SPI_MASTER_3)) {
        log_hal_info("SPI master deinit failed \r\n");
    }

    hal_gpio_deinit(HAL_GPIO_25);
    hal_gpio_deinit(HAL_GPIO_26);
    hal_gpio_deinit(HAL_GPIO_27);
    hal_gpio_deinit(HAL_GPIO_28);

    log_hal_info("---spi_master_send_data_two_boards_example ends---\r\n");
}

int main(void)
{
    /* Configure System clock */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware*/
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    log_hal_info("\r\n\r\n");
    
   /* The output UART used by log_hal_info is set by log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("\r\n\r\n");

    spi_master_send_data_two_boards_example();

    while (1);
}
