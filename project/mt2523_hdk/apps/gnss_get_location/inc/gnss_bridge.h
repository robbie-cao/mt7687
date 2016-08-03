#ifndef __GNSS_BRIDGE_H__
#define __GNSS_BRIDGE_H__

#include "hal_uart.h"

#define GNSS_BRIDGE_UART_PORT HAL_UART_1

typedef enum gnss_bridge_callback_type {
    GNSS_BRIDGE_CALLBACK_TYPE_CAN_READ,
    GNSS_BRIDGE_CALLBACK_TYPE_CAN_WRITE
} gnss_bridge_callback_type_t;


typedef void (*gnss_bridge_callback_f)(gnss_bridge_callback_type_t type);
extern uint32_t gnss_bridge_read_command(uint8_t* buf, int32_t buf_len);
extern uint32_t gnss_bridge_send_debug_data(uint8_t* buf, int32_t buf_len);
extern uint32_t gnss_bridge_put_data(uint8_t *buf, uint32_t buf_len);
extern TaskHandle_t gnss_bridge_task_init(void);
extern void gnss_bridge_task_deinit(TaskHandle_t taskHandler);
extern void gnss_uart_bridge_init(hal_uart_port_t port, gnss_bridge_callback_f callback);
extern void gnss_uart_bridge_deinit(void);

#endif
