#ifndef _MSC_VER
#include "stdint.h"
#include "main_screen.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "graphic_log.h"
#include "bsp_ctp.h"

#define UI_TASK_QUEUE_SIZE 20
#define UI_TASK_NAME "UI_DEMO"
#define UI_TASK_STACK_SIZE 4800
#define UI_TASK_PRIORITY 3

typedef struct ui_task_message_struct {
    int message_id;
    int param1;
    void* param2;
} ui_task_message_struct_t;

struct {
    QueueHandle_t event_queue;
    touch_event_proc_func touch_event_callback_f;
    void* user_data;
} ui_task_cntx;


extern void common_event_handler(message_id_enum event_id, int32_t param1, void* param2);
extern void uart_printf(char *fmt,...);
static int32_t ui_send_event_from_isr(message_id_enum event_id, int32_t param1, void* param2);

log_create_module(GRAPHIC_TAG, PRINT_LEVEL_INFO);

void demo_ui_register_touch_event_callback(touch_event_proc_func proc_func, void* user_data)
{
    GRAPHICLOG("demo_ui_register_touch_event_callback, proc_func:%x", proc_func);
    ui_task_cntx.touch_event_callback_f = proc_func;
    ui_task_cntx.user_data = user_data;
}

void demo_ui_ctp_callback_func(void* param)
{
    ui_send_event_from_isr(MESSAGE_ID_PEN_EVENT, 0, NULL);
}

void demo_ui_process_sigle_event(bsp_ctp_multiple_event_t* event)
{
    // support single touch currently.
    bsp_ctp_event_status_t hit_type;
    int32_t i = 0;
    static touch_event_struct_t pre_event = {{0,0},TOUCH_EVENT_MAX};

    GRAPHICLOG("process single event, model:%d", event->model);
    if (event->model <= 0) {
        return;
    }

    if (pre_event.type == TOUCH_EVENT_MAX || pre_event.type == TOUCH_EVENT_UP || pre_event.type == TOUCH_EVENT_ABORT) {
        // skip all up & abort point
        hit_type = CTP_PEN_DOWN;
    } else {
        // skip all down & abort event
        hit_type = CTP_PEN_UP;
    }

    while (i < event->model) {
         GRAPHICLOG("[point] event = %d, piont[0].x = %d, piont[0].y = %d\r\n",
								event->points[i].event, 	\
								event->points[i].x,		\
								event->points[i].y);
         if (event->points[i].event == hit_type) {
             break;
         }
         i++;
    }
    if (i >= event->model) {
        GRAPHICLOG("no valid point, pre event:%d", pre_event.type);
        return;
    }

    pre_event.position.x = event->points[i].x;
    pre_event.position.y = event->points[i].y;
    pre_event.type = event->points[i].event;

    if (ui_task_cntx.touch_event_callback_f) {
        GRAPHICLOG("callback app, type:%d,[%d:%d]", pre_event.type, pre_event.position.x, pre_event.position.y);
        ui_task_cntx.touch_event_callback_f(&pre_event, ui_task_cntx.user_data);
    }
}

static void pen_event_handle()
{
    bsp_ctp_status_t ret;
    bsp_ctp_multiple_event_t ctp_event;

    // get pen event
    GRAPHICLOG("pen_event_handle");

    ret = bsp_ctp_get_event_data(&ctp_event);
    GRAPHICLOG("ctp_get_event_data ret:%d", ret);
    while (ret == BSP_CTP_OK) {
        ret = bsp_ctp_get_event_data(&ctp_event);
        demo_ui_process_sigle_event(&ctp_event);
    }
    if (ret == BSP_CTP_EVENT_EMPTY) {
        //demo_ui_process_sigle_event(&ctp_event);
    }

}

static void ui_task_msg_handler(ui_task_message_struct_t *message)
{
    if (!message) {
        return;
    }
    GRAPHICLOG("ui_task_msg_handler, message_id:%d", message->message_id);
    switch (message->message_id) {
        case MESSAGE_ID_PEN_EVENT:
            pen_event_handle();
            break;
        default:
            common_event_handler(message->message_id, message->param1, message->param2);
            break;
                
    }
}

int32_t ui_send_event(message_id_enum event_id, int32_t param1, void* param2)
{
    ui_task_message_struct_t message;
    message.message_id = event_id;
    message.param1 = param1;
    message.param2 = param2;
    
    return xQueueSend(ui_task_cntx.event_queue, &message, 10);
}

int32_t ui_send_event_from_isr(message_id_enum event_id, int32_t param1, void* param2)
{
    BaseType_t xHigherPriorityTaskWoken;
    ui_task_message_struct_t message;
    message.message_id = event_id;
    message.param1 = param1;
    message.param2 = param2;
    
    return xQueueSendFromISR(ui_task_cntx.event_queue, &message, &xHigherPriorityTaskWoken);
}
void ui_task_main()
{
    ui_task_message_struct_t queue_item;
    bsp_ctp_status_t ret;
 
    ret = bsp_ctp_init();
    GRAPHICLOG("ctp init, ret:%d", ret);
    ret = bsp_ctp_register_callback(demo_ui_ctp_callback_func, NULL);
    GRAPHICLOG("ctp register callback, ret:%d", ret);
    ui_task_cntx.event_queue = xQueueCreate(UI_TASK_QUEUE_SIZE , sizeof( ui_task_message_struct_t ) );
    GRAPHICLOG("ui_task_main");
    show_main_screen();
    while (1) {
        if (xQueueReceive(ui_task_cntx.event_queue, &queue_item, 10)) {
            ui_task_msg_handler(&queue_item);
        }
    }
}

TaskHandle_t demo_app_start()
{
    TaskHandle_t task_handler;
    xTaskCreate( ui_task_main, UI_TASK_NAME, UI_TASK_STACK_SIZE, NULL, UI_TASK_PRIORITY, &task_handler );
    GRAPHICLOG("demo_app_start, task_handler:%d", task_handler);
    return task_handler;
}

#else
#include "main_screen.h"
struct {
    touch_event_proc_func touch_event_callback_f;
    void* user_data;
} ui_task_cntx;
void demo_ui_register_touch_event_callback(touch_event_proc_func proc_func, void* user_data)
{
    ui_task_cntx.touch_event_callback_f = proc_func;
    ui_task_cntx.user_data = user_data;
}
void touch_event_proc(int x, int y, int event_type)
{
	touch_event_struct_t touch_event;
	touch_event.position.x = x;
	touch_event.position.y = y;
	touch_event.type = event_type;
	if (ui_task_cntx.touch_event_callback_f) {
		ui_task_cntx.touch_event_callback_f(&touch_event, ui_task_cntx.user_data);;
	}
}

int32_t ui_send_event(message_id_enum event_id, int32_t param1, void* param2)
{
    common_event_handler(event_id, param1, param2);
}
#endif
