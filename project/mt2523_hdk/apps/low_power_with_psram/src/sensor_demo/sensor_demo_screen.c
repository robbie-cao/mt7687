#if defined(_MSC_VER)
#else
#include "stdio.h"
#endif

#include "sensor_demo.h"

#ifdef SENSOR_DEMO

#include "graphic_interface.h"
#include "main_screen.h"
#include "bt_common_dispatch.h"
#include "bt_gap.h"
#include "sensor_demo.h"
#define LEFT_GAP 5
#define ITEM_HEIGHT 25

#include "syslog.h"
log_create_module(sensor_screen, PRINT_LEVEL_INFO);
#define SENSOR_DEMO_LOGI(fmt,...) LOG_I(sensor_screen,  (fmt), ##__VA_ARGS__)

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
static char s_hr_bpm_str[16] = {0} ;
static char s_hrv_sdnn_str[16] = {0} ;
static char s_hrv_lf_str[16] = {0} ;
static char s_hrv_hf_str[16] = {0} ;
static char s_hrv_lfhf_str[16] = {0} ;
static int32_t s_hr_bpm_val = 0;
static int32_t s_hrv_measure_status = 1;/* 1: init , 2: measuring , 3: measure done */
static int32_t s_bt_status_value = 1;/* 1 : unconnected 2: connected */
static int32_t g_btspp_enabled_flag = 1;/* 1 : disabled 2: enabled */
static int32_t s_hrv_sdnn_val = 15000;
static int32_t s_hrv_lf_val = 17000;
static int32_t s_hrv_hf_val = 23000;
static int32_t s_hrv_lfhf_val = 2000;



#define ITOA_LEN           20
#define ITOA_SLIM_UDIV_R(N, D, R) (((R)=(N)%(D)), ((N)/(D)))

void show_sensor_screen(void);

struct {
  int32_t sensor_title_x;
  int32_t sensor_title_y;
  int32_t hr_title_x;
  int32_t hr_title_y;
  int32_t hrv_title_x;
  int32_t hrv_title_y;
  int32_t hrv_value_x;
  int32_t hrv_value_y;
  int32_t bt_name_x;
  int32_t bt_name_y;
  int32_t bt_status_x;
  int32_t bt_status_y;
  int32_t back_x1;
  int32_t back_y1;
  int32_t back_x2;
  int32_t back_y2;
  gdi_color bg_color;
  gdi_color font_color;
} sensor_screen_cntx;

uint8_t* sensor_convert_string_to_wstring(char* string)
{
    static uint8_t wstring[50];
    int32_t index = 0;
    if (!string) {
        return NULL;
    }
    while (*string) {
        wstring[index] = *((uint8_t*)string);
        wstring[index + 1] = 0;
        string++;
        index+=2;
    }
    return wstring;
}

void sensor_pen_event_handler(touch_event_struct_t* pen_event, void* user_data)
{
    static int32_t back_hit;
    static int32_t hrv_hit;
    if (pen_event->position.x <= sensor_screen_cntx.back_x2 && pen_event->position.x >= sensor_screen_cntx.back_x1) {
        if (pen_event->position.y <= sensor_screen_cntx.back_y2 && pen_event->position.y >= sensor_screen_cntx.back_y1) {
            if (pen_event->type == TOUCH_EVENT_DOWN) {
                back_hit = 1;
            } else if (back_hit == 1 && pen_event->type == TOUCH_EVENT_UP) {
                back_hit = 0;
                s_hrv_measure_status = 1;
                SENSOR_DEMO_LOGI("sensor screen exit\r\n");
                SENSOR_DEMO_LOGI("disable_all_sensors\r\n");
                disable_all_sensors();
                if( g_btspp_enabled_flag==2 ){
                    SENSOR_DEMO_LOGI("bt_gap_power_off\r\n");
                    bt_gap_power_off();
                    g_btspp_enabled_flag = 1;
                }
                show_main_screen();

            }
        }
    } else if(pen_event->position.x <= 100 && pen_event->position.x >= sensor_screen_cntx.hrv_title_x) {
        if (pen_event->position.y <= sensor_screen_cntx.hrv_value_y && pen_event->position.y >= sensor_screen_cntx.hrv_title_y) {
            if (pen_event->type == TOUCH_EVENT_DOWN) {
                hrv_hit = 1;
            } else if (hrv_hit == 1 && pen_event->type == TOUCH_EVENT_UP) {
                hrv_hit = 0;
                SENSOR_DEMO_LOGI("press HRV\r\n");
                s_hrv_measure_status = 2;
                show_sensor_screen();
                enable_hrv();
            }
        }
    }
    else {
        back_hit = 0;
        hrv_hit = 0;
    }
}

void sensor_event_handler(message_id_enum event_id, int32_t param1, void* param2)
{
}

void show_sensor_screen(void)
{
    int32_t string_width, string_height;
    int32_t hrv_value_width,hr_str_width;
    int32_t tmp_hrv_sdnn_val;
    int32_t tmp_hrv_lf_val;
    int32_t tmp_hrv_hf_val;
    int32_t tmp_hrv_lfhf_val;

    demo_ui_register_touch_event_callback(sensor_pen_event_handler, NULL);

    sensor_screen_cntx.bg_color = 0;
    sensor_screen_cntx.font_color = 0xFFFF;
    /* first */
    sensor_screen_cntx.sensor_title_x = 5;
    sensor_screen_cntx.sensor_title_y = 6;

    /* middle */
    sensor_screen_cntx.hr_title_x = LEFT_GAP;
    sensor_screen_cntx.hr_title_y = ITEM_HEIGHT*2 + sensor_screen_cntx.sensor_title_y;
    sensor_screen_cntx.hrv_title_x = LEFT_GAP;
    sensor_screen_cntx.hrv_title_y = ITEM_HEIGHT + sensor_screen_cntx.hr_title_y;
    sensor_screen_cntx.hrv_value_x = LEFT_GAP;
    sensor_screen_cntx.hrv_value_y = ITEM_HEIGHT + sensor_screen_cntx.hrv_title_y;
    sensor_screen_cntx.bt_name_x = LEFT_GAP;
    sensor_screen_cntx.bt_name_y = ITEM_HEIGHT*2 + sensor_screen_cntx.hrv_value_y;

    /* end */
    sensor_screen_cntx.bt_status_x = LEFT_GAP;
    sensor_screen_cntx.bt_status_y = ITEM_HEIGHT*2 + sensor_screen_cntx.bt_name_y;
    sensor_screen_cntx.back_x1 = 160;
    sensor_screen_cntx.back_y1 = ITEM_HEIGHT*2 + sensor_screen_cntx.bt_name_y;
    sensor_screen_cntx.back_x2 = 239;
    sensor_screen_cntx.back_y2 = 239;

    gdi_draw_solid_rect(0,0,239,239, sensor_screen_cntx.bg_color);

    /* first */
    font_engine_get_string_width_height(sensor_convert_string_to_wstring("Sensor Subsystem"),strlen("Sensor Subsystem"),
            &string_width,&string_height);
    font_engine_show_string(sensor_screen_cntx.sensor_title_x,sensor_screen_cntx.sensor_title_y,
            sensor_convert_string_to_wstring("Sensor Subsystem"),strlen("Sensor Subsystem"),sensor_screen_cntx.font_color);

    /* HR */
    font_engine_get_string_width_height(sensor_convert_string_to_wstring("HR:"),strlen("HR:"),
            &string_width,&string_height);
    font_engine_show_string(sensor_screen_cntx.hr_title_x,sensor_screen_cntx.hr_title_y,
            sensor_convert_string_to_wstring("HR:"),strlen("HR:"),sensor_screen_cntx.font_color);
    hr_str_width = string_width+sensor_screen_cntx.hr_title_x;
    memset(s_hr_bpm_str, 0, 16);
    snprintf(s_hr_bpm_str,16, "%ld", s_hr_bpm_val);


    font_engine_show_string(sensor_screen_cntx.hr_title_x + string_width, sensor_screen_cntx.hr_title_y,
            sensor_convert_string_to_wstring(s_hr_bpm_str), strlen(s_hr_bpm_str), sensor_screen_cntx.font_color);
    font_engine_get_string_width_height(sensor_convert_string_to_wstring(s_hr_bpm_str),strlen(s_hr_bpm_str),
            &string_width,&string_height);
    hr_str_width = hr_str_width + string_width;
    font_engine_show_string(hr_str_width,sensor_screen_cntx.hr_title_y,
            sensor_convert_string_to_wstring(" BPM"),strlen(" BPM"),sensor_screen_cntx.font_color);
    hr_str_width = string_width+sensor_screen_cntx.hr_title_x;
    /* HRV*/
    font_engine_get_string_width_height(sensor_convert_string_to_wstring("HRV:"),strlen("HRV:"),
            &string_width,&string_height);
    font_engine_show_string(sensor_screen_cntx.hrv_title_x,sensor_screen_cntx.hrv_title_y,
            sensor_convert_string_to_wstring("HRV:"),strlen("HRV:"),sensor_screen_cntx.font_color);

    /* HRV value */

    if(s_hrv_measure_status==3){

        /* measure done */
        /* SDNN in HRV title start*/
        hrv_value_width = sensor_screen_cntx.hrv_title_x+string_width;
        font_engine_get_string_width_height(sensor_convert_string_to_wstring("SDNN="),strlen("SDNN="),
                &string_width,&string_height);
        font_engine_show_string(hrv_value_width,sensor_screen_cntx.hrv_title_y,
                sensor_convert_string_to_wstring("SDNN="),strlen("SDNN="),sensor_screen_cntx.font_color);
        /* SDNN val */
        memset(s_hrv_sdnn_str, 0, 16);
        tmp_hrv_sdnn_val = s_hrv_sdnn_val/100;
        snprintf(s_hrv_sdnn_str,16, "%ld.%ld", (int32_t)(tmp_hrv_sdnn_val/10),(int32_t)(tmp_hrv_sdnn_val%10));

        hrv_value_width = hrv_value_width + string_width;
        font_engine_show_string(hrv_value_width, sensor_screen_cntx.hrv_title_y,
                sensor_convert_string_to_wstring(s_hrv_sdnn_str), strlen(s_hrv_sdnn_str), sensor_screen_cntx.font_color);
        font_engine_get_string_width_height(sensor_convert_string_to_wstring(s_hrv_sdnn_str),strlen(s_hrv_sdnn_str),
                &string_width,&string_height);
        /* SDNN in HRV title end*/
        /* LF */
        hrv_value_width = sensor_screen_cntx.hrv_value_x;
        font_engine_get_string_width_height(sensor_convert_string_to_wstring("L="),strlen("L="),
                &string_width,&string_height);
        font_engine_show_string(hrv_value_width,sensor_screen_cntx.hrv_value_y,
                sensor_convert_string_to_wstring("L="),strlen("L="),sensor_screen_cntx.font_color);
        /* LF val */
        memset(s_hrv_lf_str, 0, 16);
        tmp_hrv_lf_val = s_hrv_lf_val/100;
        snprintf(s_hrv_lf_str,16, "%ld.%ld", (int32_t)(tmp_hrv_lf_val/10),(int32_t)(tmp_hrv_lf_val%10));

        hrv_value_width = hrv_value_width + string_width;
        font_engine_show_string(hrv_value_width, sensor_screen_cntx.hrv_value_y,
                sensor_convert_string_to_wstring(s_hrv_lf_str), strlen(s_hrv_lf_str), sensor_screen_cntx.font_color);
        font_engine_get_string_width_height(sensor_convert_string_to_wstring(s_hrv_lf_str),strlen(s_hrv_lf_str),
                &string_width,&string_height);
        /* HF */
        hrv_value_width = hrv_value_width + string_width;
        font_engine_get_string_width_height(sensor_convert_string_to_wstring(" H="),strlen(" H="),
                &string_width,&string_height);
        font_engine_show_string(hrv_value_width,sensor_screen_cntx.hrv_value_y,
                sensor_convert_string_to_wstring(" H="),strlen(" H="),sensor_screen_cntx.font_color);
        /* HF val */
        memset(s_hrv_hf_str, 0, 16);
        tmp_hrv_hf_val = s_hrv_hf_val/100;
        snprintf(s_hrv_hf_str,16, "%ld.%ld", (int32_t)(tmp_hrv_hf_val/10),(int32_t)(tmp_hrv_hf_val%10));

        hrv_value_width = hrv_value_width + string_width;
        font_engine_show_string(hrv_value_width, sensor_screen_cntx.hrv_value_y,
                sensor_convert_string_to_wstring(s_hrv_hf_str), strlen(s_hrv_hf_str), sensor_screen_cntx.font_color);
        font_engine_get_string_width_height(sensor_convert_string_to_wstring(s_hrv_hf_str),strlen(s_hrv_hf_str),
                &string_width,&string_height);
        /* LFHF */
        hrv_value_width = hrv_value_width + string_width;
        font_engine_get_string_width_height(sensor_convert_string_to_wstring(" LH="),strlen(" LH="),
                &string_width,&string_height);
        font_engine_show_string(hrv_value_width,sensor_screen_cntx.hrv_value_y,
                sensor_convert_string_to_wstring(" LH="),strlen(" LH="),sensor_screen_cntx.font_color);
        /* LFHF val */
        memset(s_hrv_lfhf_str, 0, 16);
        tmp_hrv_lfhf_val = s_hrv_lfhf_val/100;
        snprintf(s_hrv_lfhf_str,16, "%ld.%ld", (int32_t)(tmp_hrv_lfhf_val/10),(int32_t)(tmp_hrv_lfhf_val%10));

        hrv_value_width = hrv_value_width + string_width;
        font_engine_show_string(hrv_value_width, sensor_screen_cntx.hrv_value_y,
                sensor_convert_string_to_wstring(s_hrv_lfhf_str), strlen(s_hrv_lfhf_str), sensor_screen_cntx.font_color);
        font_engine_get_string_width_height(sensor_convert_string_to_wstring(s_hrv_lfhf_str),strlen(s_hrv_lfhf_str),
                &string_width,&string_height);
    }else if(s_hrv_measure_status==2){
        /* measuring */
        font_engine_get_string_width_height(sensor_convert_string_to_wstring("Measuring"),strlen("Measuring"),
                &string_width,&string_height);
        font_engine_show_string(sensor_screen_cntx.hrv_value_x,sensor_screen_cntx.hrv_value_y,
                sensor_convert_string_to_wstring("Measuring"),strlen("Measuring"),sensor_screen_cntx.font_color);
    }else{
        /* not measure */

        font_engine_get_string_width_height(sensor_convert_string_to_wstring("Press HRV to measure"),strlen("Press HRV to measure"),
                &string_width,&string_height);
        font_engine_show_string(sensor_screen_cntx.hrv_value_x,sensor_screen_cntx.hrv_value_y,
                sensor_convert_string_to_wstring("Press HRV to measure"),strlen("Press HRV to measure"),sensor_screen_cntx.font_color);
    }
    /* BT Name */
    font_engine_get_string_width_height(sensor_convert_string_to_wstring("BT Name: MTK device"),strlen("BT Name: MTK device"),
            &string_width,&string_height);
    font_engine_show_string(sensor_screen_cntx.bt_name_x,sensor_screen_cntx.bt_name_y,
            sensor_convert_string_to_wstring("BT Name: MTK device"),strlen("BT Name: MTK device"),sensor_screen_cntx.font_color);

    /* end line */
    /* BT status */
    if(s_bt_status_value == 1){
        font_engine_get_string_width_height(sensor_convert_string_to_wstring("Unconnected"),strlen("Unconnected"),
                &string_width,&string_height);
        font_engine_show_string(sensor_screen_cntx.bt_status_x,sensor_screen_cntx.bt_status_y,
                sensor_convert_string_to_wstring("Unconnected"),strlen("Unconnected"),sensor_screen_cntx.font_color);
    }else{
        font_engine_get_string_width_height(sensor_convert_string_to_wstring("Connected"),strlen("Connected"),
                &string_width,&string_height);
        font_engine_show_string(sensor_screen_cntx.bt_status_x,sensor_screen_cntx.bt_status_y,
                sensor_convert_string_to_wstring("Connected"),strlen("Connected"),sensor_screen_cntx.font_color);
    }

    font_engine_get_string_width_height(
                                sensor_convert_string_to_wstring("Exit"),
                                strlen("Exit"),
                                &string_width,
                                &string_height);
    font_engine_show_string(sensor_screen_cntx.back_x1,
                            sensor_screen_cntx.back_y1,
                            sensor_convert_string_to_wstring("Exit"),
                            strlen("Exit"),
                            sensor_screen_cntx.font_color);

    gdi_flush_screen();
}

void show_sensor_ready_to_connect_screen(void)
{
    if(g_btspp_enabled_flag==1){
        bt_common_callback_set_demo(BT_SPP_SENSOR);
        SENSOR_DEMO_LOGI("bt_gap_power_on and enable_all_sensors\r\n");
        bt_gap_power_on();
        enable_all_sensors();
        g_btspp_enabled_flag = 2;
    }
    s_bt_status_value = 1;
    SENSOR_DEMO_LOGI("show_sensor_ready_to_connect_screen\r\n");
    show_sensor_screen();
}
void show_sensor_connected_screen(void)
{
    s_bt_status_value = 2;
    SENSOR_DEMO_LOGI("show_sensor_connected_screen\r\n");
    show_sensor_screen();
}

void update_hr_data(int32_t bpm)
{
    s_hr_bpm_val = bpm;
    SENSOR_DEMO_LOGI("update_hr_data\r\n");
    show_sensor_screen();
}
void update_hrv_data(int32_t sdnn,int32_t lf,int32_t hf,int32_t lfhf)
{
    s_hrv_sdnn_val = sdnn;
    s_hrv_lf_val = lf;
    s_hrv_hf_val = hf;
    s_hrv_lfhf_val = lfhf;
    s_hrv_measure_status = 3;
    SENSOR_DEMO_LOGI("update_hrv_data\r\n");
    show_sensor_screen();
}
#endif // #ifdef SENSOR_DEMO

