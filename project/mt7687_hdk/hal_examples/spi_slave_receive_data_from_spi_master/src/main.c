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

#include "mt7637_cm4_hw_memmap.h"
#include "hal_spim.h"
#include "spim.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware( void );
extern void top_xtal_init(void);
extern void cmnSerialFlashClkConfTo64M(void);
extern void cmnCpuClkConfigureTo192M(void);

#define TEST_SIZE (20 * 1024)
#define LOOP_COUNT (TEST_SIZE/4-1)
#define TEST_DATA (0x55)

uint8_t data_buffer[TEST_SIZE] = {0};

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


hal_cache_region_config_t region_cfg_tbl[] = {
    /* cache_region_address, cache_region_size */
    { CM4_EXEC_IN_PLACE_BASE, 0x1000000}
    /* add cache regions below if you have any */
};
static int32_t cache_enable(hal_cache_size_t cache_size)
{
    uint8_t region, region_number;

    region_number = sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]);
    if (region_number > HAL_CACHE_REGION_MAX) {
        return -1;
    }
    /* If cache is enabled, flush and invalidate cache */
    hal_cache_init();
    hal_cache_set_size(cache_size);
    for (region = 0; region < region_number; region++) {
        hal_cache_region_config((hal_cache_region_t)region, &region_cfg_tbl[region]);
        hal_cache_region_enable((hal_cache_region_t)region);
    }
    for (; region < HAL_CACHE_REGION_MAX; region++) {
        hal_cache_region_disable((hal_cache_region_t)region);
    }
    hal_cache_enable();
    return 0;
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
    if (cache_enable(HAL_CACHE_SIZE_32KB) < 0) {
        log_hal_info("cache enable failed");
    }

    /* Enable MCU clock to 192MHz */
    cmnCpuClkConfigureTo192M();

    /*Enable flash clock to 64MHz*/
    //cmnSerialFlashClkConfTo64M();
    /* Peripherals initialization */
    plain_log_uart_init();

    /* Board HW initialization */
}

static void spim_send_data_to_spis()
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t address = 0;
    uint32_t command_buffer = 0x05000c80;
    uint32_t write_low_data = 0x55550480;
    uint32_t write_high_data = 0x55550680;
    uint32_t write_low_addr = 0x0880;
    uint32_t write_high_addr = 0x0a80;
    address = (uint32_t)(&data_buffer);
    //log_hal_info("spis receive address is %x\n", address);

    write_low_addr |= (((address & 0xff00) << 8) | ((address & 0x00ff) << 24));
    write_high_addr |= (((address & 0xff000000) >> 8) | ((address & 0x00ff0000) << 8));
    //log_hal_info("write_low_addr is %x\n", write_low_addr);
    //log_hal_info("write_high_addr is %x\n", write_high_addr);

    hal_spi_master_send_polling(HAL_SPI_MASTER_0, (uint8_t *)(&write_low_addr), 4);
    hal_spi_master_send_polling(HAL_SPI_MASTER_0, (uint8_t *)(&write_high_addr), 4);
    hal_spi_master_send_polling(HAL_SPI_MASTER_0, (uint8_t *)(&write_low_data), 4);
    hal_spi_master_send_polling(HAL_SPI_MASTER_0, (uint8_t *)(&write_high_data), 4);
    //log_hal_info("write_data_reg=%x\n", (*((volatile uint32_t *)(0x21000704))));
    //log_hal_info("bus_address_reg=%x\n", (*((volatile uint32_t *)(0x21000708))));
    hal_spi_master_send_polling(HAL_SPI_MASTER_0, (uint8_t *)(&command_buffer), 4);
    for (i = 0; i < LOOP_COUNT; i++) {
        hal_spi_master_send_polling(HAL_SPI_MASTER_0, (uint8_t *)(&write_low_data), 4);
        hal_spi_master_send_polling(HAL_SPI_MASTER_0, (uint8_t *)(&write_high_data), 4);
        hal_spi_master_send_polling(HAL_SPI_MASTER_0, (uint8_t *)(&command_buffer), 4);
    }

    for (i = 0; i < 50; i++) {
        j++;
        if (data_buffer[i] != TEST_DATA) {
            log_hal_info("data_buffer[%d]=%x\n", i, data_buffer[i]);
            log_hal_info("Test fail\n");
            j--;
        }
    }
    for (i = (TEST_SIZE - 50); i < TEST_SIZE; i++) {
        j++;
        if (data_buffer[i] != TEST_DATA) {
            log_hal_info("data_buffer[%d]=%x\n", i, data_buffer[i]);
            log_hal_info("Test fail\n");
            j--;
        }
    }
    if (j == 100) {
        log_hal_info("Test successful\n");
    }
}

/* Example of SPI master send data with polling mode */
static void spim_send_data_2_spis_example()
{
    uint32_t frequency = 20000000;
    hal_spi_master_config_t spi_config;
    hal_spi_slave_config_t  spis_configure;

    log_hal_info("---spim_example begin---\r\n");

    hal_gpio_init(HAL_GPIO_7);
    hal_gpio_init(HAL_GPIO_24);
    hal_gpio_init(HAL_GPIO_25);
    hal_gpio_init(HAL_GPIO_26);
    hal_gpio_init(HAL_GPIO_29);
    hal_gpio_init(HAL_GPIO_30);
    hal_gpio_init(HAL_GPIO_31);
    hal_gpio_init(HAL_GPIO_32);
    /* Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool hasn't been used to configure the related pinmux */
    /* SPIM pinmux setting */
    hal_pinmux_set_function(HAL_GPIO_24, HAL_GPIO_24_SPI_MOSI_M_CM4);
    hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_SPI_MISO_M_CM4);
    hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_SPI_SCK_M_CM4);
    hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_SPI_CS_0_M_CM4);
    /* SPIS pinmux setting */
    hal_pinmux_set_function(HAL_GPIO_29, HAL_GPIO_29_SPI_MOSI_S_CM4);
    hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_SPI_MISO_S_CM4);
    hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_SPI_SCK_S_CM4);
    hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_SPI_CS_0_S_CM4);

    spis_configure.phase = HAL_SPI_SLAVE_CLOCK_PHASE0;
    spis_configure.polarity = HAL_SPI_SLAVE_CLOCK_POLARITY0;
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_init(HAL_SPI_SLAVE_0, &spis_configure)) {
        log_hal_info("hal_spi_slave_init fail\n");
    } else {
        log_hal_info("hal_spi_slave_init pass\n");
    }

    /* Init SPI master */
    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = frequency;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(HAL_SPI_MASTER_0, &spi_config)) {
        log_hal_info("hal_spi_master_init fail\n");
    } else {
        log_hal_info("hal_spi_master_init pass\n");
    }

    spim_send_data_to_spis();

    /* De-init spi master */
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(HAL_SPI_MASTER_0)) {
        log_hal_info("hal_spi_master_deinit fail\n");
    }

    hal_gpio_deinit(HAL_GPIO_7);
    hal_gpio_deinit(HAL_GPIO_24);
    hal_gpio_deinit(HAL_GPIO_25);
    hal_gpio_deinit(HAL_GPIO_26);
    hal_gpio_deinit(HAL_GPIO_29);
    hal_gpio_deinit(HAL_GPIO_30);
    hal_gpio_deinit(HAL_GPIO_31);
    hal_gpio_deinit(HAL_GPIO_32);
    log_hal_info("---spim_example end---\r\n");
}

int main(void)
{
    /* SystemClock Config */
    SystemClock_Config();

    /* Configure the hardware ready to run the test. */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    log_hal_info("\r\n\r\n");/* The output UART used by log_hal_info is set by log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("pls add your own code from here\r\n");
    log_hal_info("\r\n\r\n");

    spim_send_data_2_spis_example();

    for ( ;; );
}


/*-----------------------------------------------------------*/


