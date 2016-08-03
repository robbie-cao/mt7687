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

#include <stdint.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include <dma_sw.h>
#include <hal_uart.h>

#include "io_def.h"
#include "syslog.h"

#if configUSE_TICKLESS_IDLE == 2
#include "hal_sleep_driver.h"
#include "hal_gpt.h"
#endif


#define USE_UART_VFIFO


#ifdef USE_UART_VFIFO
#define VFIFO_SIZE              128
#define VFIFO_ALERT_LENGTH      0
#endif


/* Block UART definition ----------------------------------------------------*/


#ifdef USE_UART_VFIFO
static SemaphoreHandle_t    g_semaphore = NULL;
static uint8_t              g_tx_vfifo[VFIFO_SIZE];
static uint8_t              g_rx_vfifo[VFIFO_SIZE];
#endif


/****************************************************************************
 * Private Functions
 ****************************************************************************/


#ifdef USE_UART_VFIFO
static void _uart_event(hal_uart_callback_event_t event, void *user_data)
{
    BaseType_t  x_higher_priority_task_woken = pdFALSE;

    /* Notify consumer. */
    xSemaphoreGiveFromISR(g_semaphore, &x_higher_priority_task_woken);

    /*
     * xSemaphoreGiveFromISR() will set *pxHigherPriorityTaskWoken to pdTRUE if giving
     * the g_semaphore caused a task to unblock, and the unblocked task has a priority
     * higher than the currently running task. If xSemaphoreGiveFromISR() sets this
     * value to pdTRUE then a context switch should be requested before the interrupt
     * is exited.
     */
    portYIELD_FROM_ISR(x_higher_priority_task_woken);
}
#endif


/****************************************************************************
 * Public Functions
 ****************************************************************************/


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE {
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    hal_uart_put_char(CONSOLE_UART, ch);

    if (ch == '\n')
    {
        hal_uart_put_char(CONSOLE_UART, '\r');
    }

    return ch;
}


/**
  * @brief  Retargets the C library getchar function to the USART.
  * @param  None
  * @retval None
  */
#if configUSE_TICKLESS_IDLE == 2
extern uint8_t tickless_handle_index;
static hal_gpt_port_t tickless_gpt_port = HAL_GPT_0;
#endif

GETCHAR_PROTOTYPE {
#ifdef USE_UART_VFIFO
    /* Blocked UART Getchar */

    while (1)
    {
        uint32_t    len;

        len = hal_uart_get_available_receive_bytes(CONSOLE_UART);

        if (len > 0) {
            uint8_t ret;
            hal_uart_receive_dma(CONSOLE_UART, &ret, 1);

#if configUSE_TICKLESS_IDLE == 2
            if (!(sleepdrv_get_lock_index() & (1 << tickless_handle_index)))
                sleepdrv_hold_sleep_lock(tickless_handle_index);

            if (HAL_GPT_STATUS_OK != hal_gpt_start_timer_ms(tickless_gpt_port, 30000, HAL_GPT_TIMER_TYPE_ONE_SHOT))
                printf("gpt timer failed starting\n");
#endif

            return ret;
        } else {
            xSemaphoreTake(g_semaphore, portMAX_DELAY);
        }
    }
#else
    int ret;
    for (;;)
    {
        ret = getc_nowait();
        if (ret == -1) {
            vTaskDelay(CFG_CLI_CMD_DELAY_VALUE);
        } else {
            break;
        }
    }
    return ret;
#endif
}


void io_def_uart_init(void)
{
#ifdef USE_UART_VFIFO
    hal_uart_dma_config_t   dma_config = {
        .send_vfifo_buffer              = g_tx_vfifo,
        .send_vfifo_buffer_size         = VFIFO_SIZE,
        .receive_vfifo_buffer           = g_rx_vfifo,
        .receive_vfifo_buffer_size      = VFIFO_SIZE,
        .receive_vfifo_threshold_size   = VFIFO_SIZE,
        .receive_vfifo_alert_size       = VFIFO_ALERT_LENGTH
    };
#endif

    log_uart_init(CONSOLE_UART);

#ifdef USE_UART_VFIFO
    /* initialize Semephore */
    g_semaphore = xSemaphoreCreateBinary();

    hal_uart_set_dma(CONSOLE_UART, &dma_config);

    hal_uart_register_callback(CONSOLE_UART, _uart_event, NULL);
#endif
}

#if defined (__CC_ARM) || defined (__ICCARM__)
/**
  * @brief  Retargets the Keil C library printf function to the USART
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    hal_uart_put_char(CONSOLE_UART, ch);

    if (ch == '\n') {
        hal_uart_put_char(CONSOLE_UART, '\r');
    }

    return ch;
}
#endif
