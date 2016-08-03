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
static volatile uint32_t spi_slave_poweron_flag = 0;
static volatile uint32_t spi_slave_cfg_write_flag = 0;
static volatile uint32_t spi_slave_write_data_flag = 0;
static volatile uint32_t spi_slave_timeout_flag = 0;
extern SPI_SLAVE_REGISTER_T *const spi_slave_register[HAL_SPI_SLAVE_MAX];
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

/* Sample code for user callback */
void slave_callback(hal_spi_slave_transaction_status_t status, void *user_data)
{
    uint16_t slave_status;
    const uint8_t *data = (uint8_t *)0x04000000;
    uint32_t size = 0x00000010;
    uint8_t *buffer = (uint8_t *)0x04000400;
    hal_spi_slave_config_t spi_configure;
    if (HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION == (status.fsm_status)) {
        /* Normal fsm behavior */
        slave_status = status.interrupt_status;
        switch (slave_status) {
            case SPISLV_IRQ_POWERON_IRQ_MASK:
                /* PDN is turned on, initializes spi slave controller here */
                spi_configure.bit_order = HAL_SPI_SLAVE_LSB_FIRST;
                spi_configure.phase = HAL_SPI_SLAVE_CLOCK_PHASE0;
                spi_configure.polarity = HAL_SPI_SLAVE_CLOCK_POLARITY0;
                spi_configure.timeout_threshold = 0xFFFFFFFF;
                hal_spi_slave_init(HAL_SPI_SLAVE_0, &spi_configure);
                spi_slave_poweron_flag = 1;
                log_hal_info("---Test-POWERON----\n");
                break;
            case SPISLV_IRQ_POWEROFF_IRQ_MASK:
                hal_spi_slave_deinit(HAL_SPI_SLAVE_0);
                spi_slave_poweron_flag = 0;
                log_hal_info("---Test-POWEROFF----\n");
                break;
            case SPISLV_IRQ_CRD_FINISH_IRQ_MASK:
                /* Call SPI slave send function here to set data address and size*/
                hal_spi_slave_send(HAL_SPI_SLAVE_0, data, size);
                log_hal_info("---Test-CRD_FINISH----\n");
                break;
            case SPISLV_IRQ_CWR_FINISH_IRQ_MASK:
                /* call spi slave read function here to set data address and size*/
                hal_spi_slave_receive(HAL_SPI_SLAVE_0, buffer, size);
                spi_slave_cfg_write_flag = 1;
                log_hal_info("---Test-CWR_FINISH----\n");
                break;
            case SPISLV_IRQ_RD_FINISH_IRQ_MASK:
                /*  */
                log_hal_info("---Test-RD_FINISH----\n");
                break;
            case SPISLV_IRQ_WR_FINISH_IRQ_MASK:
                /* User can now get the data from the address set before */
                spi_slave_write_data_flag = 1;
                log_hal_info("---Test-WR_FINISH----\n");
                break;
            case SPISLV_IRQ_RD_ERR_IRQ_MASK:
                /* Data buffer may be reserved for retransmit depending on user's usage */
                log_hal_info("---Test-RD_ERR----\n");
                break;
            case SPISLV_IRQ_WR_ERR_IRQ_MASK:
                /* Data in the address set before isn't correct, user may abandon them */
                log_hal_info("---Test-WR_ERR----\n");
                break;
            case SPISLV_IRQ_TIMEOUT_ERR_IRQ_MASK:
                /* */
                spi_slave_timeout_flag = 1;
                break;
            default:
                break;
        }
    } else if (HAL_SPI_SLAVE_FSM_INVALID_OPERATION != (status.fsm_status)) {
        switch (status.fsm_status) {
            case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR:
                log_hal_info("HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR, fsm is poweroff\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW:
                log_hal_info("HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW, fsm is poweroff\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR:
                log_hal_info("HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR, fsm is CR\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW:
                log_hal_info("HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW, fsm is CR\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW:
                log_hal_info("HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW, fsm is CW\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR:
                log_hal_info("HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR, fsm is CW\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR:
                log_hal_info("HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR, fsm is poweron\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW:
                log_hal_info("HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW, fsm is poweron\n");
                break;
            default:
                break;
        }
    } else {
        log_hal_info("HAL_SPI_SLAVE_FSM_INVALID_OPERATION, fsm is poweron\n");
    }

}

void spi_slave_receive_data_two_boards_example()
{

    log_hal_info("---spi_slave_receive_data_two_boards_example begin---\r\n");

    hal_gpio_init(HAL_GPIO_25);
    hal_gpio_init(HAL_GPIO_26);
    hal_gpio_init(HAL_GPIO_27);
    hal_gpio_init(HAL_GPIO_28);

    /* GPIO configuaration for SPI slave */
    hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_SLV_SPI0_CS);
    hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_SLV_SPI0_SCK);
    hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_SLV_SPI0_MOSI);
    hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_SLV_SPI0_MISO);

    /* Register callback function */
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_register_callback(HAL_SPI_SLAVE_0, slave_callback, NULL)) {
        log_hal_info("SPI slave register callback failed \r\n");
    }

    /* Timeout settings for SPI slave to avoid timeout error if SPI master didn't transfer data for a long time */
    spi_slave_register[HAL_SPI_SLAVE_0]->SPISLV_TIMOUT_THR = 0xFFFFFFFF;

    while (!spi_slave_poweron_flag) {
        if (spi_slave_timeout_flag) {
            log_hal_info("Slave timeout error happened\r\n");
        }
        log_hal_info("Slave is waiting for power_on command\r\n");
    }
    log_hal_info("Slave is in power_on status\r\n");
    while (!spi_slave_cfg_write_flag) {
        if (spi_slave_timeout_flag) {
            log_hal_info("Slave timeout error happened\r\n");
        }
        log_hal_info("Slave is waiting for cfg_write command\r\n");
    }
    log_hal_info("Slave is received cfg_write command\r\n");

    while (!spi_slave_write_data_flag) {
        if (spi_slave_timeout_flag) {
            log_hal_info("Slave timeout error happened\r\n");
        }
        log_hal_info("Slave is waiting for write data command\r\n");
    }
    log_hal_info("Slave has received data\r\n");
    while (spi_slave_poweron_flag) {
        if (spi_slave_timeout_flag) {
            log_hal_info("Slave timeout error happened\r\n");
        }
        log_hal_info("Slave is waiting for power_off command\r\n");
    }
    log_hal_info("Slave is in power_off status\r\n");

    hal_gpio_deinit(HAL_GPIO_25);
    hal_gpio_deinit(HAL_GPIO_26);
    hal_gpio_deinit(HAL_GPIO_27);
    hal_gpio_deinit(HAL_GPIO_28);

    log_hal_info("spi_slave_receive_data_two_boards finished\r\n");
}

int main(void)
{
    /* Configure System clock */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware ready to run the test Configure the hardware */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    log_hal_info("\r\n\r\n");
  
    /* The output UART used by log_hal_info is set by log_uart_init() */
    log_hal_info("welcome to main()\r\n");
    log_hal_info("\r\n\r\n");

    spi_slave_receive_data_two_boards_example();

    while (1);
}
