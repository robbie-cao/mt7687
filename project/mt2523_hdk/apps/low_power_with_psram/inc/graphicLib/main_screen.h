
#ifndef __MAIN_SCREEN_H__
#define __MAIN_SCREEN_H__
#if defined(_MSC_VER)
#include "gdi_type_adaptor.h"
#else
#include "stdint.h"
#endif


typedef enum {
    TOUCH_EVENT_UP,
    TOUCH_EVENT_DOWN,
    TOUCH_EVENT_ABORT,
    TOUCH_EVENT_MAX
} touch_event_enum;

typedef struct touch_pos_struct {
    int16_t x;
    int16_t y;
} touch_pos_struct_t;

typedef struct touch_event_struct {
    touch_pos_struct_t position;
    touch_event_enum type;
} touch_event_struct_t;


#define CONFIG_MESSAGE_ID_ENUM
typedef enum{
    MESSAGE_ID_PEN_EVENT,
#include "screen_config.h"
} message_id_enum;
#undef CONFIG_MESSAGE_ID_ENUM

typedef void (*show_screen_proc_f)(void);

typedef void (*event_handle_func)(message_id_enum event_id, int32_t param1, void* param2);

typedef void (*touch_event_proc_func)(touch_event_struct_t* touch_event, void* user_data);


// go back to main screen
void show_main_screen(void);

// send event to your screen. can used this api to transfer data between tasks and so on.
int32_t ui_send_event(message_id_enum event_id, int32_t param1, void* param2);

// register pen event for your application.
void demo_ui_register_touch_event_callback(touch_event_proc_func proc_func, void* user_data);

#endif

