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

#if !defined (MTK_DEBUG_LEVEL_NONE)

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "syslog.h"
#include "toi.h"
#include "hal_gpt.h"
#include "exception_handler.h"

/* max size of each block of normal log message */
#define MAX_LOG_SIZE (256)

/* max size of each block of hex dump message */
#define MAX_DUMP_MESSAGE_SIZE (128)

/* max size of each block of hex dump message */
#define MAX_DUMP_DATA_SIZE (128)

/* max hex dump length */
#define MAX_HEXDUMP_LENGTH (2048)

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || defined(MTK_NO_PSRAM_ENABLE)

/* log queue size */
#define LOG_QUEUE_SIZE (8)

/* buffer pool items */
#define LOG_POOL_ITEMS (16)

/* maximum delay to wait for available log queue */
#define LOG_QUEUE_WAITING_TIME (portMAX_DELAY)

#elif (PRODUCT_VERSION == 2523)

/* For MT2523, the bt audio related feature generates lots of bt logs. */

/* log queue size */
#define LOG_QUEUE_SIZE (16*32)

/* buffer pool items */
#define LOG_POOL_ITEMS (16*32)

/* maximum delay to wait for available log queue */
#define LOG_QUEUE_WAITING_TIME (0)

#endif

/* log task stack size */
#define LOG_TASK_STACK_SIZE (192)

/* log task priority */
#define LOG_TASK_PRIO (1)

typedef enum {
    MESSAGE_TYPE_NORMAL_LOG,
    MESSAGE_TYPE_HEX_DUMP
} message_type_t;

typedef struct {
    const char *module_name;
    print_level_t print_level;
    const char *func_name;
    int line_number;
    uint32_t timestamp;
    char message[MAX_LOG_SIZE + 1]; //C string format
} normal_log_message_t;

typedef struct {
    const char *module_name;
    print_level_t print_level;
    const char *func_name;
    int line_number;
    uint32_t timestamp;
    char message[MAX_DUMP_MESSAGE_SIZE + 1]; //C string format
    uint32_t data_length;
    char data[MAX_DUMP_DATA_SIZE];
    char *additional_data;
} hex_dump_message_t;


/* log message definition */
typedef struct {
    message_type_t message_type;
    union {
        normal_log_message_t normal_log_message;
        hex_dump_message_t   hex_dump_message;
    } u;
} log_message_t;

typedef struct {
    uint32_t occupied;
    uint8_t  buffer[sizeof(log_message_t)];
} syslog_buffer_t;

/* syslog buffer pool */
static syslog_buffer_t syslog_buffer_pool[LOG_POOL_ITEMS];

/* log queue handle */
static xQueueHandle g_log_queue = NULL;

#define SYSLOG_UART_INIT_DONE 0x1
#define SYSLOG_GPT_INIT_DONE  0x2
#define SYSLOG_NVDM_INIT_DONE 0x4
#define SYSLOG_UART_DMA_MODE  0x8

static uint32_t syslog_init_time = 0;
static uint32_t syslog_init_done = 0;
static uint32_t syslog_task_ready = false;

/* create common module for unspecified module to use */
log_create_module(common, PRINT_LEVEL_INFO);

static syslog_config_t syslog_config;

static const char *log_switch_table[]  = { "on", "off" };

static const char *print_level_table[] = { "info", "warning", "error" };

static hal_uart_port_t g_log_uart_port;

#if defined(SYSLOG_DMA_ENABLE)

/* uart dma rx buffer size */
#define LOG_UART_RX_DMA_SIZE (16)

/* uart dma tx buffer size */
#define LOG_UART_TX_DMA_SIZE (256)

#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  __attribute__ ((__section__(".noncached_zidata"),__aligned__(4)))

static uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN g_rx_vfifo_buffer[LOG_UART_RX_DMA_SIZE];
static uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN g_tx_vfifo_buffer[LOG_UART_TX_DMA_SIZE];
static SemaphoreHandle_t mutex_tx, mutex_rx;

#endif /* SYSLOG_DMA_ENABLE */

/*******************************************************************************
 *  syslog uart port management
 ******************************************************************************/

hal_uart_status_t log_uart_init(hal_uart_port_t port)
{
    hal_uart_config_t uart_config;
    hal_uart_status_t ret;

    /* Configure UART PORT */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    g_log_uart_port = port;
    ret = hal_uart_init(port, &uart_config);

    syslog_init_done |= SYSLOG_UART_INIT_DONE;

    return ret;
}

void log_putchar(char byte)
{
    hal_uart_put_char(g_log_uart_port, byte);
}

#if defined(SYSLOG_DMA_ENABLE)

static hal_uart_status_t log_switch_to_uart_polling(void)
{
    syslog_init_done &= ~(SYSLOG_UART_INIT_DONE | SYSLOG_UART_DMA_MODE);

    hal_uart_deinit(g_log_uart_port);
    log_uart_init(g_log_uart_port);

    syslog_init_done |= SYSLOG_UART_INIT_DONE;
    return HAL_UART_STATUS_OK;
}

static void syslog_uart_dma_callback(hal_uart_callback_event_t status, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken;

    if (status == HAL_UART_EVENT_READY_TO_WRITE) {
        xSemaphoreGiveFromISR(mutex_tx, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    else if (status == HAL_UART_EVENT_READY_TO_READ) {
        xSemaphoreGiveFromISR(mutex_rx, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}

static hal_uart_status_t log_switch_to_uart_dma(void)
{
    hal_uart_config_t uart_config;
    hal_uart_dma_config_t dma_config;

    syslog_init_done &= ~SYSLOG_UART_INIT_DONE;

    hal_uart_deinit(g_log_uart_port);
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    hal_uart_init(g_log_uart_port, &uart_config);

    dma_config.receive_vfifo_alert_size = LOG_UART_RX_DMA_SIZE / 8;
    dma_config.receive_vfifo_buffer = g_rx_vfifo_buffer;
    dma_config.receive_vfifo_buffer_size = LOG_UART_RX_DMA_SIZE;
    dma_config.receive_vfifo_threshold_size = LOG_UART_RX_DMA_SIZE / 4;
    dma_config.send_vfifo_buffer = g_tx_vfifo_buffer;
    dma_config.send_vfifo_buffer_size = LOG_UART_TX_DMA_SIZE;
    dma_config.send_vfifo_threshold_size = LOG_UART_TX_DMA_SIZE / 16;
    hal_uart_set_dma(g_log_uart_port, &dma_config);
    hal_uart_register_callback(g_log_uart_port, syslog_uart_dma_callback, NULL);

    syslog_init_done |= (SYSLOG_UART_INIT_DONE | SYSLOG_UART_DMA_MODE);
    return HAL_UART_STATUS_OK;
}

bool log_send_dma(char *buf, int len)
{
    uint32_t avail_space, left_size, real_byte, sent_byte;
    char *pbuf;

    pbuf = buf;
    left_size = len;
    while(1){
        avail_space = hal_uart_get_available_send_space(g_log_uart_port);
        if(avail_space > left_size) {
            real_byte = left_size;
        }
        else {
            real_byte = avail_space;
        }
        sent_byte = hal_uart_send_dma(g_log_uart_port, (const uint8_t *)pbuf, real_byte);
        left_size -= sent_byte;
        pbuf += sent_byte;
        if(sent_byte != real_byte) {
            return false;
        }
        if(!left_size) {
            break;
        }
        xSemaphoreTake(mutex_tx, portMAX_DELAY);
    }
    return true;
}

int log_receive_dma(char *buf, int len)
{
    uint32_t left_size, avail_bytes, rcv_bytes;
    char *pbuf;

    pbuf = buf;
    left_size = len;
    while (1) {
        avail_bytes = hal_uart_get_available_receive_bytes(g_log_uart_port);
        avail_bytes = (left_size < avail_bytes) ? left_size : avail_bytes;
        rcv_bytes = hal_uart_receive_dma(g_log_uart_port, (uint8_t *)buf, avail_bytes);
        left_size -= rcv_bytes;
        pbuf += rcv_bytes;
        if (rcv_bytes != avail_bytes) {
            return 0;
        }
        if (!left_size) {
            return len;
        }
        xSemaphoreTake(mutex_rx, portMAX_DELAY);
    }
}

int log_write(char *buf, int len)
{
    int DataIdx;
    int ret = 0;

    if (syslog_init_done & SYSLOG_UART_INIT_DONE)  {
        if (syslog_init_done & SYSLOG_UART_DMA_MODE) {
            ret = log_send_dma(buf, len) ? len : 0;
        } else {
            for (DataIdx = 0; DataIdx < len; DataIdx++) {
                log_putchar( *buf++ );
            }
            ret = len;
        }
    }
    return ret;
}

static void exception_io_init(void)
{
    log_switch_to_uart_polling();
    syslog_task_ready = false;
}

#endif /* SYSLOG_DMA_ENABLE */

#define LOG_TS(p) (((log_message_t*)(p)->buffer)->u.normal_log_message.timestamp)

static syslog_buffer_t *sort_syslog_buffer(syslog_buffer_t *pList)
{
    syslog_buffer_t *pHead = NULL, *pCurr, *p;

    if (pList == NULL || pList->occupied == 0) {
        return pList;
    }

    while (pList != NULL) {
        pCurr = pList;
        pList = (syslog_buffer_t*)(pList->occupied);
        if ((pHead == NULL) || LOG_TS(pCurr) < LOG_TS(pHead)) {
           pCurr->occupied = (uint32_t)pHead;
           pHead = pCurr;
        }
        else {
           p = pHead;
           while (p != NULL) {
              if ((p->occupied == 0) ||
                  LOG_TS(pCurr) < LOG_TS((syslog_buffer_t*)(p->occupied))) {
                  pCurr->occupied = p->occupied;
                  p->occupied = (uint32_t)pCurr;
                  break;
              }
              p = (syslog_buffer_t*)(p->occupied);
           }
        }
    }
    return pHead;
}

void process_log_message(const log_message_t *log_message);

static void syslog_buffer_dump(void)
{
    uint32_t index;
    syslog_buffer_t *pHead = NULL, *pCurr;

    /* create a link list of syslog buffer(s) in use now */
    for (index = 0; index < LOG_POOL_ITEMS; index++) {
        /* reuse the 'occupied' field as a pointer */
        if (syslog_buffer_pool[index].occupied) {
            syslog_buffer_pool[index].occupied = (uint32_t)pHead;
            pHead = &syslog_buffer_pool[index];
        }
    }

    /* sort the log by time */
    pHead = sort_syslog_buffer(pHead);

    printf("\n\r>>> dump syslog buffer\n\r\n\r");

    /* print the sorted log */
    while (pHead) {
        pCurr = pHead;
        pHead = (syslog_buffer_t*)(pHead->occupied);
        pCurr->occupied = 1;
        process_log_message((log_message_t *)(pCurr->buffer));
    }
}

static void syslog_exception_cb(void)
{
#if defined(SYSLOG_DMA_ENABLE)
    exception_io_init();
#endif
    syslog_buffer_dump();
}

/*******************************************************************************
 *  syslog memory pool management
 ******************************************************************************/
void buffer_pool_init()
{
    memset(syslog_buffer_pool, 0, sizeof(syslog_buffer_pool));
}

syslog_buffer_t *buffer_pool_alloc(void)
{
    UBaseType_t uxSavedInterruptStatus;

    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    for (uint32_t index = 0; index < LOG_POOL_ITEMS; index++) {
        if (!syslog_buffer_pool[index].occupied) {
            syslog_buffer_pool[index].occupied = true;
            portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
            return &syslog_buffer_pool[index];
        }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
    return NULL;
}

void buffer_pool_release(syslog_buffer_t *syslog_buffer)
{
    UBaseType_t uxSavedInterruptStatus;
    log_message_t *log_message = (log_message_t *)syslog_buffer->buffer;

    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    syslog_buffer->occupied = false;
    if ((log_message->message_type == MESSAGE_TYPE_HEX_DUMP)
            && (log_message->u.hex_dump_message.additional_data) ) {
        vPortFree(log_message->u.hex_dump_message.additional_data);
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
}

static unsigned int get_current_time_in_ms(void)
{
    uint32_t count = 0;
    uint64_t count64 = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    count64 = ((uint64_t)count)*1000/32768;
    return (unsigned int)count64;
}

/*******************************************************************************
 *  syslog queue management
 ******************************************************************************/
void log_queue_create(void)
{
    g_log_queue = xQueueCreate(LOG_QUEUE_SIZE, sizeof(syslog_buffer_t *));
    if (NULL == g_log_queue) {
        printf("log_queue_create: create log queue failed.\r\n");
    }
}

void log_queue_send(syslog_buffer_t **log_buffer)
{
    if (portNVIC_INT_CTRL_REG & 0xff) {
        BaseType_t xTaskWokenByPost = pdFALSE;
        if (xQueueSendFromISR(g_log_queue, log_buffer, &xTaskWokenByPost) ==  errQUEUE_FULL ) {
            buffer_pool_release(*log_buffer);
        } else {
            portYIELD_FROM_ISR(xTaskWokenByPost);
        }
    } else {
        xQueueSend(g_log_queue, log_buffer, LOG_QUEUE_WAITING_TIME);
    }
}

int log_queue_receive(syslog_buffer_t **log_buffer)
{
    BaseType_t ret_val = xQueueReceive(g_log_queue, log_buffer, portMAX_DELAY);
    return (pdPASS == ret_val) ? 0 : -1;
}

/*******************************************************************************
 *  syslog implementation
 ******************************************************************************/
#define change_level_to_string(level) \
                ((level) == PRINT_LEVEL_WARNING ? "WARNING" : \
                 (level) == PRINT_LEVEL_ERROR   ? "ERROR" : \
                                                  "INFO")

void print_normal_log(const normal_log_message_t *normal_log_message)
{
    printf("[T: %u M: %s C: %s F: %s L: %d]: ",
           (unsigned int)normal_log_message->timestamp,
           normal_log_message->module_name,
           change_level_to_string(normal_log_message->print_level),
           normal_log_message->func_name,
           normal_log_message->line_number);
    printf("%s\r\n", (char *)normal_log_message->message);
}

void hex_dump_data(const char *data, int length)
{
    int index = 0;
    for (index = 0; index < length; index++) {
        printf("%02X", (int)(data[index]));
        if ((index + 1) % 16 == 0) {
            printf("\r\n");
            continue;
        }
        if (index + 1 != length) {
            printf(" ");
        }
    }
    if (0 != index && 0 != index % 16) {
        printf("\r\n");//add one more blank line
    }
}

void print_hex_dump(const hex_dump_message_t *hex_dump_message)
{
    printf("[T: %u M: %s C: %s F: %s L: %d]: ",
           (unsigned int)hex_dump_message->timestamp,
           hex_dump_message->module_name,
           change_level_to_string(hex_dump_message->print_level),
           hex_dump_message->func_name,
           hex_dump_message->line_number);
    printf("%s\r\n", (char *)hex_dump_message->message);
    if (hex_dump_message->data_length > MAX_DUMP_DATA_SIZE) {
        hex_dump_data(hex_dump_message->data, MAX_DUMP_DATA_SIZE);
        hex_dump_data(hex_dump_message->additional_data, hex_dump_message->data_length - MAX_DUMP_DATA_SIZE);
    } else {
        hex_dump_data(hex_dump_message->data, hex_dump_message->data_length);
    }
}

void process_log_message(const log_message_t *log_message)
{
    if (MESSAGE_TYPE_NORMAL_LOG == log_message->message_type) {
        print_normal_log(&log_message->u.normal_log_message);
    } else {
        print_hex_dump(&log_message->u.hex_dump_message);
    }
}

void syslog_task_entry(void *args)
{
#if defined(SYSLOG_DMA_ENABLE)
    log_switch_to_uart_dma();
#endif

    syslog_task_ready = true;
    for (;;) {
        syslog_buffer_t *log_buffer = NULL;
        if (0 == log_queue_receive(&log_buffer)) {
            process_log_message((log_message_t *)(log_buffer->buffer));
            buffer_pool_release(log_buffer);
        }
    }
}

void log_init(syslog_save_fn         save,
              syslog_load_fn         load,
              log_control_block_t  **entries)
{
    exception_config_type exception_config;

    syslog_init_done |= SYSLOG_UART_INIT_DONE; /* for backward compatible */
    buffer_pool_init();
    log_queue_create();
    syslog_task_ready = false;

    exception_config.init_cb = syslog_exception_cb;
    exception_config.dump_cb = NULL;
    exception_register_callbacks(&exception_config);

#if defined(SYSLOG_DMA_ENABLE)
    mutex_tx = xSemaphoreCreateBinary();
    mutex_rx = xSemaphoreCreateBinary();
#endif

    xTaskHandle xHandle;
    if (pdPASS != xTaskCreate(syslog_task_entry,
                              "syslog",
                              LOG_TASK_STACK_SIZE,
                              NULL,
                              LOG_TASK_PRIO,
                              &xHandle)) {
        printf("log_init: create syslog task failed.\r\n");
    }

    syslog_init_time = get_current_time_in_ms();
    syslog_init_done |= SYSLOG_GPT_INIT_DONE;

    syslog_config.save_fn = save;
    syslog_config.filters = entries;

    if (load) {
        /* overwrite the rom code definition if any */
        if (load(&syslog_config) != 0) {
            LOG_E(common, "load syslog config failed\n");
        }
    }
    syslog_init_done |= SYSLOG_NVDM_INIT_DONE;
}

void vprint_module_log(void *handle,
                       const char *func,
                       int line,
                       print_level_t level,
                       const char *message,
                       va_list list)
{
    log_control_block_t *context = (log_control_block_t *)handle;
    if ((context->log_switch == DEBUG_LOG_ON)
            && (context->print_level <= level)) {

        if (!syslog_task_ready
                || (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED
                    && (portNVIC_INT_CTRL_REG & 0xff) == 0)) {
            if (syslog_init_done > 0) {
                UBaseType_t uxSavedInterruptStatus;
                uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
                if (syslog_init_done > SYSLOG_UART_INIT_DONE) {
                    /* gpt init done, time can be printed  */
                    printf("[T: %u M: %s C: %s F: %s L: %d]: ",
                           get_current_time_in_ms(),
                           context->module_name,
                           change_level_to_string(level),
                           func,
                           line);
                }
                vprintf(message, list);
                printf("\r\n");
                portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
            }
        } else {
            /* syslog task is ready, send to queue to print */
            syslog_buffer_t *syslog_buffer = buffer_pool_alloc();
            if (NULL == syslog_buffer) {
                printf("No syslog buffer, log dropped!\r\n");
                return;
            }
            log_message_t *log_message = (log_message_t *)syslog_buffer->buffer;

            log_message->message_type = MESSAGE_TYPE_NORMAL_LOG;
            normal_log_message_t *normal_log_message = &log_message->u.normal_log_message;
            normal_log_message->module_name = context->module_name;
            normal_log_message->print_level = level;
            normal_log_message->func_name = func;
            normal_log_message->line_number = line;
            normal_log_message->timestamp = get_current_time_in_ms();

            /* This step might fail when log message is too long,
               but syslog will do it's best to print the log */
            (void)vsnprintf(normal_log_message->message, MAX_LOG_SIZE, message, list);
            log_queue_send(&syslog_buffer);
        }
    }
}

void print_module_log(void *handle,
                      const char *func,
                      int line,
                      print_level_t level,
                      const char *message, ...)
{
    va_list ap;
    va_start(ap, message);
    vprint_module_log(handle, func, line, level, message, ap);
    va_end(ap);
}

void vdump_module_buffer(void *handle,
                         const char *func,
                         int line,
                         print_level_t level,
                         const void *data,
                         int length,
                         const char *message,
                         va_list list)
{
    log_control_block_t *context = (log_control_block_t *)handle;
    int additional_length;

    if ((context->log_switch == DEBUG_LOG_ON)
            && (context->print_level <= level)) {

        if (!syslog_task_ready
                || (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED
                    && (portNVIC_INT_CTRL_REG & 0xff) == 0)) {
            if (syslog_init_done > 0) {
                UBaseType_t uxSavedInterruptStatus;
                uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
                if (syslog_init_done > SYSLOG_UART_INIT_DONE) {
                    /* gpt init done, time can be printed  */
                    printf("[T: %u M: %s C: %s F: %s L: %d]: ",
                           get_current_time_in_ms(),
                           context->module_name,
                           change_level_to_string(level),
                           func,
                           line);
                }
                vprintf(message, list);
                printf("\r\n");
                hex_dump_data((char *)data, length);
                portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
            }
        } else {
            /* syslog task is ready, send to queue to print */
            syslog_buffer_t *syslog_buffer = buffer_pool_alloc();
            if (NULL == syslog_buffer) {
                printf("No syslog buffer, log dropped!\r\n");
                return;
            }
            log_message_t *log_message = (log_message_t *)syslog_buffer->buffer;

            log_message->message_type = MESSAGE_TYPE_HEX_DUMP;
            hex_dump_message_t *hex_dump_message = &log_message->u.hex_dump_message;
            hex_dump_message->module_name = context->module_name;
            hex_dump_message->print_level = level;
            hex_dump_message->func_name = func;
            hex_dump_message->line_number = line;
            hex_dump_message->timestamp = get_current_time_in_ms();

            /* This step might fail when log message is too long,
               but syslog will do it's best to print the log */
            (void)vsnprintf(hex_dump_message->message, MAX_DUMP_MESSAGE_SIZE, message, list);

            if (length > MAX_HEXDUMP_LENGTH) {
                length = MAX_HEXDUMP_LENGTH;
            }

            additional_length = 0;
            if (length > MAX_DUMP_DATA_SIZE) {
                if ( (portNVIC_INT_CTRL_REG & 0xff) == 0) {
                    additional_length = length - MAX_DUMP_DATA_SIZE;
                }
                length = MAX_DUMP_DATA_SIZE;
            }

            memcpy(hex_dump_message->data, data, length);
            hex_dump_message->data_length = length;

            hex_dump_message->additional_data = NULL;
            if (additional_length) {
                if ((hex_dump_message->additional_data = (char *) pvPortMalloc(additional_length)) != NULL) {
                    memcpy(hex_dump_message->additional_data, (char *)data + length, additional_length);
                    hex_dump_message->data_length += additional_length;
                }
            }

            log_queue_send(&syslog_buffer);
        }
    }
}

void dump_module_buffer(void *handle,
                        const char *func,
                        int line,
                        print_level_t level,
                        const void *data,
                        int length,
                        const char *message, ...)
{
    va_list ap;
    va_start(ap, message);
    vdump_module_buffer(handle, func, line, level, data, length, message, ap);
    va_end(ap);
}


void syslog_get_config(syslog_config_t *config)
{
    *config = syslog_config;
}

/**
 * @param p  the string pointer.
 * @param pp the pointer be assigned the start of next string.
 *
 * @retval 0 No valid string found.
 * @return the length of the found string.
 */
static int syslog_get_a_string(const char *p, char **pp)
{
    const char *start;

    while (*p == ' ') {
        p++;
    }

    start = p;

    /* first non-space is '\0' */
    if (*p == '\0') {
        *pp = NULL;
        return 0;
    }

    *pp = (char *)p;

    while (*p != ' ' && *p != '\0') {
        p++;
    }

    return (p - start);
}

static int syslog_get_index_from_module_name(log_control_block_t **filters, const char *module_name)
{
    int i = 0;

    if (filters == NULL) {
        return -1;
    }

    while (filters[i]) {
        if (!strcmp(filters[i]->module_name, module_name)) {
            return i;
        }
        i++;
    }

    /* no match tag name found */
    return -1;
}

int syslog_set_filter(char *module_name, char *log_switch_str, char *print_level_str, int save)
{
    int         i;
    int         log_switch;
    int         print_level;

    log_switch = str_to_log_switch(log_switch_str);
    if (log_switch < 0) {
        return -1;
    }

    print_level = str_to_print_level(print_level_str);
    if (print_level < 0) {
        return -1;
    }

    if (strcmp(module_name, "*") == 0) {

        if (syslog_config.filters == NULL) {
            return -1;
        }

        i = 0;
        while (syslog_config.filters[i]) {
            syslog_config.filters[i]->log_switch = (log_switch_t)log_switch;
            syslog_config.filters[i]->print_level = (print_level_t)print_level;
            i++;
        }

    } else {

        i = syslog_get_index_from_module_name(syslog_config.filters, module_name);
        /* no match tag name found */
        if (i < 0) {
            return -1;
        }
        syslog_config.filters[i]->log_switch = (log_switch_t)log_switch;
        syslog_config.filters[i]->print_level = (print_level_t)print_level;

    }

    if (save) {
        syslog_config.save_fn(&syslog_config);
    }

    return 0;
}

int syslog_at_set_filter(char *module_name, int log_switch, int print_level, int save)
{
    int         i;

    if (strcmp(module_name, "*") == 0) {

        if (syslog_config.filters == NULL) {
            return -1;
        }

        i = 0;
        while (syslog_config.filters[i]) {
            syslog_config.filters[i]->log_switch = (log_switch_t)log_switch;
            syslog_config.filters[i]->print_level = (print_level_t)print_level;
            i++;
        }

    } else {

        i = syslog_get_index_from_module_name(syslog_config.filters, module_name);
        /* no match tag name found */
        if (i < 0) {
            return -1;
        }
        syslog_config.filters[i]->log_switch = (log_switch_t)log_switch;
        syslog_config.filters[i]->print_level = (print_level_t)print_level;

    }

    if (save) {
        syslog_config.save_fn(&syslog_config);
    }

    return 0;
}

/**
 * Find name and filter spec pair and convert into filter.
 */
int syslog_convert_filter_str2val(log_control_block_t **filters, char *buff)
{
    int     pos = 0;
    int     i;
    char    *module_name;
    char    *log_switch_str;
    char    *print_level_str;
    bool    done = false;
    uint32_t    log_switch;
    uint32_t    print_level;
    uint8_t     type;

    while (!done) {

        /*
         * 1. find module_name
         */

        i = syslog_get_a_string(&buff[pos], &module_name);
        if (i == 0 || buff[pos + i] == '\0') {
            /* premature end */
            return -1;
        }

        buff[pos + i]  = '\0';
        pos           += i + 1;

        /*
         * 2. find log_switch
         */

        i = syslog_get_a_string(&buff[pos], &log_switch_str);
        if (i == 0) {
            /* premature end */
            return -2;
        }

        done = (buff[i + pos] == '\0');

        buff[pos + i]  = '\0';
        pos           += i + 1;

        /*
         * 3. find print_level
         */

        i = syslog_get_a_string(&buff[pos], &print_level_str);
        if (i == 0) {
            /* premature end */
            return -2;
        }

        done = (buff[i + pos] == '\0');

        buff[pos + i]  = '\0';
        pos           += i + 1;

        /* 4. check and set */

        i = syslog_get_index_from_module_name(filters, module_name);

        /* no match tag found */
        if (i < 0) {
            return -3;
        }

        log_switch = toi(log_switch_str, &type);

        /* not a valid number */
        if (type == TOI_ERR) {
            return -4;
        }

        /* handle out of range */
        log_switch = (log_switch > DEBUG_LOG_OFF) ? DEBUG_LOG_OFF : log_switch;

        print_level = toi(print_level_str, &type);

        /* not a valid number */
        if (type == TOI_ERR) {
            return -4;
        }

        /* handle out of range */
        print_level = (print_level > PRINT_LEVEL_ERROR) ? PRINT_LEVEL_ERROR : print_level;

        filters[i]->log_switch = (log_switch_t)log_switch;
        filters[i]->print_level = (print_level_t)print_level;
    }

    return 0;
}

int syslog_convert_filter_val2str(const log_control_block_t **filters, char *buff)
{
    int i   = 0;
    int pos = 0;
    int n;

    while (filters && filters[i]) {
        n = snprintf(&buff[pos], SYSLOG_FILTER_LEN - pos, "%s%s 0x%01x 0x%01x",
                    (i == 0) ? "" : " ",
                    filters[i]->module_name,
                    filters[i]->log_switch,
                    filters[i]->print_level);
        pos += n;
        i++;
    }

    configASSERT (pos < SYSLOG_FILTER_LEN);

    return pos;
}

const char *log_switch_to_str(log_switch_t log_switch)
{
    configASSERT (log_switch >= DEBUG_LOG_ON && log_switch <= DEBUG_LOG_OFF);
    return  log_switch_table[log_switch];
}

const char *print_level_to_str(print_level_t print_level)
{
    configASSERT (print_level >= PRINT_LEVEL_INFO && print_level <= PRINT_LEVEL_ERROR);
    return  print_level_table[print_level];
}

int str_to_log_switch(const char *log_switch_str)
{
    int i;

    for (i = 0; i < sizeof(log_switch_table) / sizeof(char *); i++) {
        if (!strcmp(log_switch_str, log_switch_table[i])) {
            return i;
        }
    }
    return -1;
}

int str_to_print_level(const char *print_level_str)
{
    int i;

    for (i = 0; i < sizeof(print_level_table) / sizeof(char *); i++) {
        if (!strcmp(print_level_str, print_level_table[i])) {
            return i;
        }
    }
    return -1;
}

int __wrap_printf( const char *format, ... )
{
    va_list ap;
    int ret;
    UBaseType_t uxSavedInterruptStatus;

    if ( !syslog_init_done ) {
        return 0;
    }

    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();

    va_start(ap, format);
    ret = vprintf(format, ap);
    va_end(ap);

    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    return ret;
}

#else

#include "syslog.h"

/* create common module for unspecified module to use */
log_create_module(common, PRINT_LEVEL_INFO);

static hal_uart_port_t g_log_uart_port;

hal_uart_status_t log_uart_init(hal_uart_port_t port)
{
    hal_uart_config_t uart_config;
    hal_uart_status_t ret;

    /* Configure UART PORT */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    g_log_uart_port = port;
    ret = hal_uart_init(port, &uart_config);

    return ret;
}

void log_putchar(char byte)
{
    hal_uart_put_char(g_log_uart_port, byte);
}

void print_module_log(void *handle,
                      const char *func,
                      int line,
                      print_level_t level,
                      const char *message, ...)
{
}

void dump_module_buffer(void *handle,
                        const char *func,
                        int line,
                        print_level_t level,
                        const void *data,
                        int length,
                        const char *message, ...)
{
}

void vprint_module_log(void *handle,
                       const char *func,
                       int line,
                       print_level_t level,
                       const char *message,
                       va_list list)
{
}

void vdump_module_buffer(void *handle,
                         const char *func,
                         int line,
                         print_level_t level,
                         const void *data,
                         int length,
                         const char *message,
                         va_list list)
{
}

#endif /* MTK_DEBUG_LEVEL_NONE */
