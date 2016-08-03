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

/*User self log api, to avoid wake log task*/

#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "wf_image.h"
#include "hal_rtc.h"
#include "mt25x3_hdk_lcd.h"
#include "mt25x3_hdk_backlight.h"
#include "syslog.h"
#include "main_screen.h"
#include "hal_sleep_manager.h"
#include "task_def.h"
#include "memory_attribute.h"
#include "bsp_ctp.h"

#define IMG_UPDATE_HEIGHT 49
#define IMG_UPDATE_WIDTH 240
/*40*4+16+24*2*/
#define NUM_OF_DIGIT 5
#define WF_EVENT_RTC 1

QueueHandle_t g_wtf_queue_handle;
bool g_wf_is_show_screen;
bool g_wf_is_task_need_delete;
ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t g_wf_time_update_area_img[IMG_UPDATE_HEIGHT * IMG_UPDATE_WIDTH/8];
uint8_t g_wf_witdh_offset;


uint32_t g_wf_index_color_table[16] = 
{
	0x0,
	0xFFFF,
	0x7E0,
	0x1F,
	0x4208,
	0x8410,
	0xFFFF,
	0xFF00,
	0x0,
	0xF800,
	0x7E0,
	0x1F,
	0x4208,
	0x8410,
	0xFFFF,
	0xFF00
};

void wf_app_task(void *arg);
static void wf_app_update_time(hal_rtc_time_t *curr_time);


void wf_app_rtc_time_callback(void *user_data)
{
			 uint8_t event;
			 event = WF_EVENT_RTC;
			 BaseType_t xHigherPriorityTaskWoken;
//			 LOG_I(common,"app task receive rtc event");
			 xQueueSendFromISR(g_wtf_queue_handle, (void *) &event , &xHigherPriorityTaskWoken);
			 
			 // Now the buffer is empty we can switch context if necessary.
			 if( xHigherPriorityTaskWoken ) {
			     // Actual macro used here is port specific.
			     portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
 }

}


static void wf_app_combine_buffer(uint8_t *dst, uint8_t *src, uint8_t width)
{
    uint32_t i;
    for (i = 0; i < IMG_UPDATE_HEIGHT;i++) {
        memcpy(dst + g_wf_witdh_offset + i * IMG_UPDATE_WIDTH/8, src + i * width/8, width/8);
    }
    
}

static void wf_app_combine_small_buffer(uint8_t *dst, uint8_t *src, uint8_t width)
{
    uint32_t i;
    for (i = 0; i < 20;i++) {
        memset(dst + g_wf_witdh_offset + i * IMG_UPDATE_WIDTH/8, 0x0, width/8);
    }
    for (i = 20; i < 49; i++) {
        memcpy(dst + g_wf_witdh_offset + i * IMG_UPDATE_WIDTH/8, src + (i-20) * width/8, width/8);
    }
    
}


static uint8_t *wf_app_get_number_buffer(uint8_t num)
{
    uint8_t *img_ptr = NULL;
    switch (num) {
        case 0:
        	   img_ptr = big_number_0;
        	   break;
        case 1:
        	   img_ptr = big_number_1;
        	   break;
        case 2:
        	   img_ptr = big_number_2;
        	   break;
        case 3:
        	   img_ptr = big_number_3;
        	   break;
        case 4:
        	   img_ptr = big_number_4;
        	   break;
        case 5:
        	   img_ptr = big_number_5;
        	   break;
        case 6:
        	   img_ptr = big_number_6;
        	   break;
        case 7:
        	   img_ptr = big_number_7;
        	   break;
        case 8:
        	   img_ptr = big_number_8;
        	   break;
        case 9:
        	   img_ptr =  big_number_9;
        	   break;
        case 0xff:
        	   img_ptr  = big_number_colon;
        	   break;
        default:
                   LOG_I(common, "wrong big number");
                   img_ptr = big_number_0;
        	   break;
    }
    return img_ptr;
}

static uint8_t *wf_app_get_small_number_buffer(uint8_t num)
{
    uint8_t *img_ptr = NULL;
    switch (num) {
        case 0:
        	   img_ptr = small_number_0;
        	   break;
        case 1:
        	   img_ptr = small_number_1;
        	   break;
        case 2:
        	   img_ptr = small_number_2;
        	   break;
        case 3:
        	   img_ptr = small_number_3;
        	   break;
        case 4:
        	   img_ptr = small_number_4;
        	   break;
        case 5:
        	   img_ptr = small_number_5;
        	   break;
        case 6:
        	   img_ptr = small_number_6;
        	   break;
        case 7:
        	   img_ptr = small_number_7;
        	   break;
        case 8:
        	   img_ptr = small_number_8;
        	   break;
        case 9:
        	   img_ptr =  small_number_9;
        	   break; 
        default:
                   LOG_I(common, "wrong small number");
                   img_ptr = small_number_0;
        	   break;
    }
    return img_ptr;
}
static uint8_t *wf_app_get_time_img_buffer(hal_rtc_time_t *time)
{
    uint8_t *temp_ptr;
    uint8_t temp_num;
    g_wf_witdh_offset = 1;
    temp_num = time->rtc_hour/10;
    temp_ptr = wf_app_get_number_buffer(temp_num);
    wf_app_combine_buffer(g_wf_time_update_area_img, temp_ptr, 40);
    g_wf_witdh_offset += 5;

    temp_num = time->rtc_hour%10;
    temp_ptr = wf_app_get_number_buffer(temp_num);
    wf_app_combine_buffer(g_wf_time_update_area_img, temp_ptr, 40);
    g_wf_witdh_offset += 5;

    temp_num = 0xff; /*":"*/
    temp_ptr = wf_app_get_number_buffer(temp_num);
    wf_app_combine_buffer(g_wf_time_update_area_img, temp_ptr, 16);
    g_wf_witdh_offset += 2;

    temp_num = time->rtc_min/10;
    temp_ptr = wf_app_get_number_buffer(temp_num);
    wf_app_combine_buffer(g_wf_time_update_area_img, temp_ptr, 40);
    g_wf_witdh_offset += 5;

    temp_num = time->rtc_min%10;
    temp_ptr = wf_app_get_number_buffer(temp_num);
    wf_app_combine_buffer(g_wf_time_update_area_img, temp_ptr, 40);
    g_wf_witdh_offset += 5;
    
    temp_num = time->rtc_sec/10;
    temp_ptr = wf_app_get_small_number_buffer(temp_num);
    wf_app_combine_small_buffer(g_wf_time_update_area_img, temp_ptr, 24);
    g_wf_witdh_offset += 3;

    temp_num = time->rtc_sec%10;
    temp_ptr = wf_app_get_small_number_buffer(temp_num);
    wf_app_combine_small_buffer(g_wf_time_update_area_img, temp_ptr, 24);
  
    return g_wf_time_update_area_img;
    
    
}

void wf_app_log(const char *format, ...)
 {
     char buffer[124];   /* Output buffer */
     va_list args;
 
     va_start(args, format);
     vsnprintf(buffer, sizeof(buffer), format, args);
     va_end(args);
     LOG_I(common,"%s\n", buffer);
 }


 
 static void wf_app_rtc_init(void)
 {
     hal_rtc_status_t result = hal_rtc_init();
     if (result != HAL_RTC_STATUS_OK) {
         wf_app_log("rtc init failed : %d", result);
         return;
     }
 #ifdef HAL_RTC_FEATURE_TIME_CALLBACK    
     hal_rtc_set_time_callback(wf_app_rtc_time_callback, NULL);
 #endif
 }
 
 static void wf_app_lcm_init(void)
 {
#if 0
     *((volatile uint32_t *) (0xA2020824))|= 0xFFF000;

    hal_pinmux_set_function(HAL_GPIO_38, 1);		// LSRSTB
    hal_pinmux_set_function(HAL_GPIO_39, 1);		// LSCE_B0
    hal_pinmux_set_function(HAL_GPIO_40, 1);		// LSCK0
    hal_pinmux_set_function(HAL_GPIO_41, 1);		// LSA0
    hal_pinmux_set_function(HAL_GPIO_42, 1);		// LSA0DA0
    hal_pinmux_set_function(HAL_GPIO_43, 1);		// LPTE

	*((volatile uint32_t *) (0xA2020824))|= 0xFFF000;
     HAL_LCD_ERROR_ENUM result = BSP_LCD_Init(0xF800);
     if (result != HAL_LCD_NO_ERROR) {
         wf_app_log("lcm init failed : %d", result);
     }
#endif
 }


void wf_app_init(void)
{

    g_wtf_queue_handle =  xQueueCreate(1, sizeof(uint8_t));
    //xTaskCreate(wf_app_task, "wf_task", 1024, NULL, 1, NULL);

}

void wf_app_task_enable_show(void)
{
    BSP_LCD_ClearScreen(0);
    demo_ui_register_touch_event_callback(NULL, NULL);
    g_wf_is_show_screen = true;
    g_wf_is_task_need_delete = true;
}



void wf_app_deinit(void)
{
    g_wf_is_show_screen = false;
}

extern uint8_t at_cmd_lock_sleep_handle;

void wf_app_task(void *arg)
{
    uint8_t event;
    hal_rtc_time_t time;
    wf_app_rtc_init();
    wf_app_lcm_init();
#ifdef HAL_RTC_FEATURE_TIME_CALLBACK   
    hal_rtc_set_time_notification_period(HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND);
#endif 
    //wf_app_task_enable_show();
    //LOG_I(common,"app task start");
    while(1) { /*receive message/semphore from RTC LISR*/
    	   if ( xQueueReceive(g_wtf_queue_handle, &event, portMAX_DELAY) == pdPASS) {
               //LOG_I(common,"app task receive event %d", event);
            if (event == WF_EVENT_RTC) {
                if (g_wf_is_task_need_delete == true) {
					BSP_Backlight_deinit();
#ifdef MTK_CTP_ENABLE
			bsp_ctp_power_on(false);
#endif
                     task_def_delete_wo_curr_task();
					 hal_sleep_manager_unlock_sleep(at_cmd_lock_sleep_handle);
                     g_wf_is_task_need_delete  =  false;
                }
                hal_rtc_get_time(&time);
                wf_app_update_time(&time);
            }
    	   }
      
    }

}


hal_display_lcd_roi_output_t lcd_para_wf;
hal_display_lcd_layer_input_t lcm_para_wf;

static void wf_app_update_time(hal_rtc_time_t *curr_time)
{
    uint8_t *img_ptr;
    //LOG_I(common, "app task to draw screen %d", g_wf_is_show_screen);
    if (g_wf_is_show_screen) {

    	  lcd_para_wf.target_start_x = 0;
    	  lcd_para_wf.target_start_y = (240 - IMG_UPDATE_HEIGHT)/2;
    	  lcd_para_wf.target_end_x = 239;
    	  lcd_para_wf.target_end_y = (240 - IMG_UPDATE_HEIGHT)/2+IMG_UPDATE_HEIGHT-1;
    	  lcd_para_wf.roi_offset_x = 0;
    	  lcd_para_wf.roi_offset_y = 0;
          lcd_para_wf.main_lcd_output = LCM_16BIT_16_BPP_RGB565_1;

    	  BSP_LCD_ConfigROI(&lcd_para_wf);
		  hal_display_lcd_turn_on_mtcmos();
          BSP_LCD_set_index_color_table(g_wf_index_color_table);
    	  lcm_para_wf.source_key_flag= 0;
    	  lcm_para_wf.alpha_flag= 0;
    	  lcm_para_wf.color_format= HAL_DISPLAY_LCD_LAYER_COLOR_1BIT_INDEX;
    	  lcm_para_wf.alpha = 0;
    	  lcm_para_wf.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
    	  lcm_para_wf.layer_enable= 0;
    	  lcm_para_wf.pitch = IMG_UPDATE_WIDTH/8;
    	  lcm_para_wf.row_size = IMG_UPDATE_HEIGHT;
    	  lcm_para_wf.column_size = IMG_UPDATE_WIDTH;
    	  lcm_para_wf.window_x_offset = 0;
    	  //lcm_para.window_y_offset = (240 - IMG_UPDATE_HEIGHT)/2;
          lcm_para_wf.window_y_offset = 0;

          memset(g_wf_time_update_area_img, 0x00, IMG_UPDATE_HEIGHT * IMG_UPDATE_WIDTH/8);
          img_ptr = wf_app_get_time_img_buffer(curr_time);
    	  lcm_para_wf.buffer_address = (uint32_t)img_ptr;

    	  BSP_LCD_ConfigLayer(&lcm_para_wf);
		  BSP_LCD_ExitIdle();
    	  BSP_LCD_UpdateScreen(lcd_para_wf.target_start_x, lcd_para_wf.target_start_y, lcd_para_wf.target_end_x, lcd_para_wf.target_end_y);
		  BSP_LCD_EnterIdle();
          hal_display_lcd_turn_off_mtcmos();
    }
}

void wf_app_update_area(void)
{
    
}

