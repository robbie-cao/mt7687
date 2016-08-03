#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#include "gnss_app.h"

#ifdef GNSS_SUPPORT_TOOL_BRIDGE

#include "gnss_bridge.h"
#include "gnss_ring_buffer.h"

#define GNSS_BRIDGE_QUEUE_SIZE 40
#define GNSS_BRIDGE_COMMAND_BUFFER_SIZE 256
#define GNSS_BRIDGE_RING_BUFFER_SIZE 10240

typedef enum gnss_bridge_message_enum {
    GNSS_BRIDGE_MESSAGE_ID_CAN_READ,
    GNSS_BRIDGE_MESSAGE_ID_CAN_WRITE,
    GNSS_BRIDGE_MESSAGE_ID_WRITE_DATA,
    GNSS_BRIDGE_MESSAGE_ID_TIMEOUT
} gnss_bridge_message_enum;

typedef struct gnss_bridge_message_struct {
    int message_id;
    int param1;
    void* param2;
} gnss_bridge_message_struct_t;

struct {
    QueueHandle_t gnss_bridge_task_queue;
} gnss_bridge_task_cntx;

uint8_t gnss_bridge_command_buf[GNSS_BRIDGE_COMMAND_BUFFER_SIZE + 1];
static ring_buf_struct_t gnss_bridge_ring_buf;
static uint8_t ring_buf[GNSS_BRIDGE_RING_BUFFER_SIZE];

int32_t test_call_back_num = 0;

static int32_t put_len = 0;
static int32_t consume_len = 0;

uint32_t gnss_bridge_put_data(uint8_t *buf, uint32_t buf_len)
{
    gnss_bridge_message_struct_t message;
    //int32_t is_need_notify;
    //GNSSLOGD("gnss_bridge_put_data, buf_len:%d\r\n", buf_len);
    //is_need_notify = need_notify(&gnss_bridge_ring_buf);
    //GNSSLOGD("\r\ngnss_pd:%s\r\n", buf);
    put_len += buf_len;
    put_data(&gnss_bridge_ring_buf, (int8_t*)buf, (int32_t)buf_len);
    if (0)//(is_need_notify && buf_len)
    {
        GNSSLOGD("put data send message\r\n");
        test_call_back_num++;
        message.message_id = GNSS_BRIDGE_MESSAGE_ID_WRITE_DATA;
        xQueueSend(gnss_bridge_task_cntx.gnss_bridge_task_queue, &message, 0);
    }
    return buf_len;
}
void gnss_bridge_task_callback(gnss_bridge_callback_type_t type)
{
    gnss_bridge_message_struct_t message = {0};
    BaseType_t xHigherPriorityTaskWoken;
    test_call_back_num++;
    if (type == GNSS_BRIDGE_CALLBACK_TYPE_CAN_READ) {
        message.message_id = GNSS_BRIDGE_MESSAGE_ID_CAN_READ;
    }
    else if (type == GNSS_BRIDGE_CALLBACK_TYPE_CAN_WRITE) {
        message.message_id = GNSS_BRIDGE_MESSAGE_ID_CAN_WRITE;
    }
    xQueueSendFromISR(gnss_bridge_task_cntx.gnss_bridge_task_queue, &message, &xHigherPriorityTaskWoken);
}

static void gnss_bridge_msg_handler(gnss_bridge_message_struct_t *message)
{
    if (!message) {
        return;
    }
    GNSSLOGD("gnss_bridge_msg_handler, message id:%d, %d, P[%d]C[%d]RS[%d]RE[%d]\r\n",
        message->message_id, test_call_back_num--, put_len, consume_len, gnss_bridge_ring_buf.start_ind, gnss_bridge_ring_buf.end_ind);
    switch (message->message_id) {
        case GNSS_BRIDGE_MESSAGE_ID_CAN_READ:
            gnss_bridge_read_command(gnss_bridge_command_buf, GNSS_BRIDGE_COMMAND_BUFFER_SIZE);
            gnss_app_send_cmd_by_other_task((int8_t*)gnss_bridge_command_buf, strlen((char*)gnss_bridge_command_buf));
            break;
        case GNSS_BRIDGE_MESSAGE_ID_CAN_WRITE:
        case GNSS_BRIDGE_MESSAGE_ID_WRITE_DATA:
        case GNSS_BRIDGE_MESSAGE_ID_TIMEOUT:
            {
                static uint8_t temp_buf[256 + 1];
                static uint32_t data_len;
                static uint32_t send_len;
                do {
                    if (data_len == 0) {
                        data_len = consume_data(&gnss_bridge_ring_buf, (int8_t*)temp_buf, 256);
                        consume_len += data_len;
                        temp_buf[data_len] = 0;
                        //GNSSLOGD("comsume data [%s]\r\n", temp_buf);
                    }
                    if (data_len > 0) {
                        send_len += gnss_bridge_send_debug_data(temp_buf + send_len, data_len - send_len);
                    } else {
                        break;
                    }
                    if (data_len == send_len) {
                        data_len = send_len = 0;
                    } else {
                        GNSSLOGD("wait can write message,left size:%d\r\n", data_len - send_len);
                        break;
                    }
                } while (1);
            }
            break;
    }
}

static void gnss_bridge_task_main()
{
    gnss_bridge_message_struct_t queue_item;
    gnss_bridge_task_cntx.gnss_bridge_task_queue = xQueueCreate( GNSS_BRIDGE_QUEUE_SIZE, sizeof( gnss_bridge_message_struct_t ) );
    gnss_uart_bridge_init(GNSS_BRIDGE_UART_PORT, gnss_bridge_task_callback);
    GNSSLOGD("gnss_bridge_task_main\r\n");
    while (1) {
        if (xQueueReceive(gnss_bridge_task_cntx.gnss_bridge_task_queue, &queue_item, portMAX_DELAY)) {
            gnss_bridge_msg_handler(&queue_item);
        }
    }
}
void gnss_bridge_timer_callback( TimerHandle_t timer_handle )
{
    gnss_bridge_message_struct_t message;
    test_call_back_num++;
    message.message_id = GNSS_BRIDGE_MESSAGE_ID_TIMEOUT;
    xQueueSend(gnss_bridge_task_cntx.gnss_bridge_task_queue, &message, 10);
}

TaskHandle_t gnss_bridge_task_init()
{
    TaskHandle_t task_handler;
    TimerHandle_t time_handle;
    GNSSLOGD("gnss_bridge_task_init\r\n");
    ring_buf_init(&gnss_bridge_ring_buf, (int8_t*)ring_buf, GNSS_BRIDGE_RING_BUFFER_SIZE);
    time_handle = xTimerCreate("BRDGT",       // Just a text name, not used by the kernel.
                    ( 80 ),   // The timer period in ticks.
                    pdTRUE,        // The timer will auto-reload themselves when they expire.
                    ( void * ) 0,  // Assign each timer a unique id equal to its array index.
                    gnss_bridge_timer_callback // Each timer calls the same callback when it expires.
                    );
    xTimerStart(time_handle, 0);
    task_handler = (TaskHandle_t) xTaskCreate( (TaskFunction_t) gnss_bridge_task_main, "GNSS_BRIDGE", 1500, NULL, 3, &task_handler );
    GNSSLOGD("create task handler:%dr\n", task_handler);
    return task_handler;
}

void gnss_bridge_task_deinit(TaskHandle_t taskHandler)
{
    GNSSLOGD("gnss_bridge_task_deinit handle:%d\r\n", taskHandler);
    gnss_uart_bridge_deinit();
    vTaskDelete(taskHandler);
}
#endif


