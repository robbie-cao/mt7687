#if defined(_MSC_VER)
#else
#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#endif

#include "graphic_interface.h"
#include "main_screen.h"
#include "gnss_app.h"
#include "task.h"
#include "gnss_log.h"

#define LEFT_GAP 20
#define ITEM_HEIGHT 28

struct {
    int32_t title_x;
    int32_t title_y;
    int32_t lat_x;
    int32_t lat_y;
    int32_t long_x;
    int32_t long_y;
    int32_t fix_x;
    int32_t fix_y;
    int32_t sv_used_x;
    int32_t sv_used_y;
    int32_t sv_total_x;
    int32_t sv_totol_y;
    int32_t speed_x;
    int32_t speed_y;
    int32_t back_x1;
    int32_t back_y1;
    int32_t back_x2;
    int32_t back_y2;
    gdi_color bg_color;
    gdi_color font_color;
} gnss_screen_cntx;

uint8_t latitude[15]="\0";
uint8_t longitude[15]="\0";
uint8_t fix[15] = "No Fix";
uint8_t sv_used[15] = "0";
uint8_t sv_total[15] = "00";
uint8_t speed[15] = "0";

TaskHandle_t gnss_task_handle;

uint8_t* gnss_convert_string_to_wstring(uint8_t* string)
{
    static uint8_t wstring[50];
    int32_t index = 0;
    if (!string) {
        return NULL;
    }

    while (*string) {
        wstring[index] = *string;
        wstring[index + 1] = 0;
        string++;
        index+=2;
    }
    return wstring;
}

void gnss_pen_event_handler(touch_event_struct_t* pen_event, void* user_data)
{
    static int32_t back_hit;
    if (pen_event->position.x <= gnss_screen_cntx.back_x2 && pen_event->position.x >= gnss_screen_cntx.back_x1) {
        if (pen_event->position.y <= gnss_screen_cntx.back_y2 && pen_event->position.y >= gnss_screen_cntx.back_y1) {
            if (pen_event->type == TOUCH_EVENT_DOWN) {
                back_hit = 1;
            } else if (back_hit == 1 && pen_event->type == TOUCH_EVENT_UP) {
                back_hit = 0;
                gnss_demo_app_stop();
                gnss_demo_app_destroy(gnss_task_handle);
                show_main_screen();
            }
        }
    } else {
        back_hit = 0;
    }
}

int32_t gnss_update_data(gnss_sentence_info_t *input)
{
    uint8_t *gpgga = input->GPGGA;
    uint8_t *gpgsv = input->GPGSV;
    uint8_t *gprmc = input->GPRMC;
    int32_t i = 0;
    int32_t j = 0;
    int32_t flag = 0;
    strcpy(sv_used, "0");
    strcpy(sv_total, "00");
    GNSSLOGD("[GNSS Demo] gnss_update_data\n");
    if (gpgsv[0] != 0){
        sv_total[0] = gpgsv[11];
        if (gpgsv[12] != ','){
            sv_total[1] = gpgsv[12];
            sv_total[2] = 0;
        }
        else{
            sv_total[1] = '0';
            sv_total[2] = 0;
        }
        GNSSLOGD("sv_total:%s", sv_total);
    }

    for (i = 0; i < 100; i++) {
        if (gpgga[i] == ',') {
            if (gpgga[i + 1] == 'N') {
                int32_t len = i - j - 1;
                latitude[len] = 0;
                while (len > 0) {
                    latitude[len - 1] = gpgga[j + len];
                    len--;
                }
            }
            if (gpgga[i + 1] == 'E') {
                int32_t len = i - j - 1;
                sv_used[0] = gpgga[i + 5];
                if (gpgga[i + 6] != ','){
                    sv_used[1] = gpgga[i + 6];
                    sv_used[2] = 0;
                }
                else{
                    sv_used[1] = 0;
                }
                GNSSLOGD("sv_total:%s\n",sv_used);

                if (gpgga[i + 3] == '0') {
                    GNSSLOGD("[GNSS Demo] Get location, invalid data!\n");
                    longitude[0] = 0;
                    latitude[0] = 0;
                    strcpy(fix, "No Fix");
                    strcpy(speed, "0");
                    return 1;
                }
                longitude[len] = 0;
                while (len >= 0) {
                    longitude[len - 1] = gpgga[j + len];
                    len--;
                }
                break;
            }
            if (gpgga[i + 1] == 'S') {
                int32_t len = i - j - 1;
                latitude[0] = '-';
                latitude[len + 1] = 0;
                while (len > 0) {
                    latitude[len] = gpgga[j + len];
                    len--;
                }
            }
            if (gpgga[i + 1] == 'W') {
                int32_t len = i - j - 1;
                sv_used[0] = gpgga[i + 5];
                if (gpgga[i + 6] != ','){
                    sv_used[1] = gpgga[i + 6];
                    sv_used[2] = 0;
                }
                else{
                    sv_used[1] = 0;
                }
                GNSSLOGD("sv_total:%s\n",sv_used);
                
                if (gpgga[i + 3] == '0') {
                    GNSSLOGD("[GNSS Demo] Get location, invalid data!\n");
                    longitude[0] = 0;
                    latitude[0] = 0;
                    strcpy(fix, "No Fix");
                    strcpy(speed, "0");
                    return 1;
                }
                longitude[0] = '-';
                longitude[len + 1] = 0;
                while (len >= 0) {
                    longitude[len] = gpgga[j + len];
                    len--;
                }
                break;
            }
            j = i;            
        }
    }

    j = 0;
    for (i = 0; i < 100; i++) {
        if (gprmc[i] == ',') {
            if (gprmc[i + 1] == 'E' || gprmc[i + 1] == 'W') {
                int32_t len = 0;
                while (gprmc[i + 3 + len] != ',') {
                    speed[len] = gprmc[i + 3 + len];
                    len++;
                }
                speed[len] = 0;
                break;
            }
            j = i;            
        }
    }

    if (latitude[0] == 0 && longitude[0] == 0) {
        strcpy(fix, "No Fix");
        strcpy(speed, "0");
        return 1;
    }

    strcpy(fix, "3D Fixed");

    return 0;

}

void gnss_demo_app_location_handle(gnss_location_handle_type_t type, void* param)
{
    if (type == GNSS_LOCATION_HANDLE_TYPE_ERROR) {
        GNSSLOGD("[GNSS Demo] location handle error! type: %d\n", (int)param);
    } else {
        gnss_location_struct_t *location = (gnss_location_struct_t *)param;
        GNSSLOGD("[GNSS Demo] App Get Location, latitude:%s, longitude:%s, accuracy:%d\n", location->latitude, location->longitude, (int)location->accuracy);
        gnss_update_data(&location->nmea_sentence);		
        ui_send_event(MESSAGE_ID_GNSS_NMEA, 0, 0);
    }
}

void gnss_demo_main(){
    int32_t periodic = 1;
    
    latitude[0] = 0; 
    longitude[0] = 0;
    strcpy(fix, "No Fix");
    strcpy(sv_total, "00");
    strcpy(sv_used, "0");
    strcpy(speed, "0");

    gnss_task_handle = gnss_demo_app_create();
    gnss_demo_app_config(periodic, gnss_demo_app_location_handle);
    gnss_demo_app_start();
}

void gnss_event_handler(message_id_enum event_id, int32_t param1, void* param2)
{
    show_gnss_screen_int();
}

void show_gnss_screen_int(void)
{
    int32_t string_width, string_height;
    gnss_screen_cntx.bg_color = 0;
    gnss_screen_cntx.font_color = 0xFFFF;
    gnss_screen_cntx.title_x = 5;
    gnss_screen_cntx.title_y = 6;
    gnss_screen_cntx.lat_x = LEFT_GAP;
    gnss_screen_cntx.lat_y = ITEM_HEIGHT + gnss_screen_cntx.title_y;
    gnss_screen_cntx.long_x = LEFT_GAP;
    gnss_screen_cntx.long_y = ITEM_HEIGHT + gnss_screen_cntx.lat_y;
    gnss_screen_cntx.fix_x = LEFT_GAP;
    gnss_screen_cntx.fix_y = ITEM_HEIGHT + gnss_screen_cntx.long_y;
    gnss_screen_cntx.sv_used_x = LEFT_GAP;
    gnss_screen_cntx.sv_used_y = ITEM_HEIGHT + gnss_screen_cntx.fix_y;
    gnss_screen_cntx.sv_total_x = LEFT_GAP;
    gnss_screen_cntx.sv_totol_y = ITEM_HEIGHT + gnss_screen_cntx.sv_used_y;
    gnss_screen_cntx.speed_x = LEFT_GAP;
    gnss_screen_cntx.speed_y = ITEM_HEIGHT + gnss_screen_cntx.sv_totol_y;
    gnss_screen_cntx.back_x1 = 160;
    gnss_screen_cntx.back_y1 = ITEM_HEIGHT + gnss_screen_cntx.speed_y;
    gnss_screen_cntx.back_x2 = 239;
    gnss_screen_cntx.back_y2 = 239;


    gdi_draw_solid_rect(0,0,239,239, gnss_screen_cntx.bg_color);

    font_engine_get_string_width_height(
        gnss_convert_string_to_wstring("GNSS "), 
        strlen("GNSS "),
        &string_width,
        &string_height);
    font_engine_show_string(gnss_screen_cntx.title_x, 
        gnss_screen_cntx.title_y, 
        gnss_convert_string_to_wstring("GNSS "), 
        strlen("GNSS "), 
        gnss_screen_cntx.font_color);


    font_engine_get_string_width_height(
        gnss_convert_string_to_wstring("Latitude: "), 
        strlen("Latitude: "),
        &string_width,
        &string_height);
    font_engine_show_string(gnss_screen_cntx.lat_x, 
        gnss_screen_cntx.lat_y, 
        gnss_convert_string_to_wstring("Latitude: "), 
        strlen("Latitude: "), 
        gnss_screen_cntx.font_color);
    font_engine_show_string(gnss_screen_cntx.lat_x + string_width, 
        gnss_screen_cntx.lat_y, 
        gnss_convert_string_to_wstring(latitude), 
        strlen(latitude), 
        gnss_screen_cntx.font_color);


    font_engine_get_string_width_height(
        gnss_convert_string_to_wstring("Longitude: "), 
        strlen("Longitude: "),
        &string_width,
        &string_height);
    font_engine_show_string(gnss_screen_cntx.long_x, 
        gnss_screen_cntx.long_y, 
        gnss_convert_string_to_wstring("Longitude: "), 
        strlen("Longitude: "), 
        gnss_screen_cntx.font_color);
    font_engine_show_string(gnss_screen_cntx.long_x + string_width, 
        gnss_screen_cntx.long_y, 
        gnss_convert_string_to_wstring(longitude), 
        strlen(longitude), 
        gnss_screen_cntx.font_color);


    font_engine_get_string_width_height(
        gnss_convert_string_to_wstring("Fix: "), 
        strlen("Fix: "),
        &string_width,
        &string_height);
    font_engine_show_string(gnss_screen_cntx.fix_x, 
        gnss_screen_cntx.fix_y, 
        gnss_convert_string_to_wstring("Fix: "), 
        strlen("Fix: "), 
        gnss_screen_cntx.font_color);
    font_engine_show_string(gnss_screen_cntx.fix_x + string_width, 
        gnss_screen_cntx.fix_y, 
        gnss_convert_string_to_wstring(fix), 
        strlen(fix), 
        gnss_screen_cntx.font_color);


    font_engine_get_string_width_height(
        gnss_convert_string_to_wstring("SV used: "), 
        strlen("SV used: "),
        &string_width,
        &string_height);
    font_engine_show_string(gnss_screen_cntx.sv_used_x, 
        gnss_screen_cntx.sv_used_y, 
        gnss_convert_string_to_wstring("SV used: "), 
        strlen("SV used: "), 
        gnss_screen_cntx.font_color);
    font_engine_show_string(gnss_screen_cntx.sv_used_x + string_width, 
        gnss_screen_cntx.sv_used_y, 
        gnss_convert_string_to_wstring(sv_used), 
        strlen(sv_used), 
        gnss_screen_cntx.font_color);


    font_engine_get_string_width_height(
        gnss_convert_string_to_wstring("SV total: "), 
        strlen("SV total: "),
        &string_width,
        &string_height);
    font_engine_show_string(gnss_screen_cntx.sv_total_x, 
        gnss_screen_cntx.sv_totol_y, 
        gnss_convert_string_to_wstring("SV total: "), 
        strlen("SV total: "), 
        gnss_screen_cntx.font_color);
    font_engine_show_string(gnss_screen_cntx.sv_total_x + string_width, 
        gnss_screen_cntx.sv_totol_y, 
        gnss_convert_string_to_wstring(sv_total), 
        strlen(sv_total), 
        gnss_screen_cntx.font_color);


    font_engine_get_string_width_height(
        gnss_convert_string_to_wstring("Speed (knots): "), 
        strlen("Speed (knots): "),
        &string_width,
        &string_height);
    font_engine_show_string(gnss_screen_cntx.speed_x, 
        gnss_screen_cntx.speed_y, 
        gnss_convert_string_to_wstring("Speed (knots): "), 
        strlen("Speed (knots): "), 
        gnss_screen_cntx.font_color);
    font_engine_show_string(gnss_screen_cntx.speed_x + string_width, 
        gnss_screen_cntx.speed_y, 
        gnss_convert_string_to_wstring(speed), 
        strlen(speed),  
        gnss_screen_cntx.font_color);


    font_engine_get_string_width_height(
        gnss_convert_string_to_wstring("Exit"), 
        strlen("Exit"),
        &string_width,
        &string_height);
    font_engine_show_string(gnss_screen_cntx.back_x1, 
        gnss_screen_cntx.back_y1, 
        gnss_convert_string_to_wstring("Exit"), 
        strlen("Exit"), 
        gnss_screen_cntx.font_color);

    gdi_flush_screen();
}

void show_gnss_screen(void)
{
    demo_ui_register_touch_event_callback(gnss_pen_event_handler, NULL);
    gnss_demo_main();
    show_gnss_screen_int();
}

