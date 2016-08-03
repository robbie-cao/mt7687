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

#ifndef _CTP_H_
#define _CTP_H_

#ifdef MTK_CTP_ENABLE

#include <stdint.h>
#include <stdbool.h>
#include "hal_eint.h"
#include "ctp_i2c.h"
#include "bsp_ctp.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CTP_PATTERN                     0xAA
#define TP_SUPPORT_POINTS               5
#define TOUCH_PANEL_BUFFER_EVENT_SIZE   300
//#define CTP_DEBUG_LOG
#define CTP_DEBUG_LOG_PUSH_POP_DATA
#define CTP_DEBUG_LOG_RETRIEVE_DATA
//#define CTP_USED_TASK_DEALY 					/*use task delay function as ctp delay*/
//#define CTP_DEBUG_PERFORMANCE					/*used to test ctp code flow run time*/

typedef enum {
    CTP_COMMAND_GET_VERSION,
    CTP_COMMAND_GET_CONFIG,
    CTP_COMMAND_LOAD_INT_CONFIG,
    CTP_COMMAND_LOAD_EXT_CONFIG,
    CTP_COMMAND_GET_DIFF_DATA,
    CTP_COMMAND_GET_FW_BUFFER,
    CTP_COMMAND_DO_FW_UPDATE
} ctp_control_command_enum_t ;


typedef struct {
    bool                    has_initialized;
    hal_gpio_pin_t          eint_pin;
    hal_eint_number_t       eint_num;
    hal_eint_config_t       init_config;
} ctp_eint_config_t;

typedef struct {
    bool                is_external_initialized;
    ctp_i2c_config_t    ctp_i2c;
    ctp_eint_config_t   ctp_eint;
    hal_gpio_pin_t      reset_pin;
    uint8_t             reg_address_byte;
} ctp_config_t;

typedef struct {
    uint16_t resolution; // CTP_RESOLTION,
    uint16_t threshold; //  CTP_THRESHOLD,
    uint16_t Report_interval;
    uint16_t Idle_time_interval;
    uint16_t sleep_time_interval;
    uint16_t gesture_active_distance;
    uint16_t MS_calibration[128];
} ctp_parameters_struct_t;// ctp_get_set_enum



typedef struct {
    char    vendor[8];
    char    product[8];
    char    firmware_version[8];
} ctp_custom_information_struct;

typedef enum {
    CTP_ACTIVE_MODE,
    CTP_IDLE_MODE,
    CTP_SLEEP_MODE,
    CTP_GESTURE_DETECTION_MODE,
    CTP_MULTIPLE_POINT_MODE,
    CTP_FIRMWARE_UPDATE,
    CTP_FM_ENABLE,
    CTP_FM_DISABLE
} ctp_device_mode_enum_t;


typedef enum {
    CTP_UP   = 0,
    CTP_DOWN,
} ctp_pen_state_enum_t;

#if 0
typedef enum {
    PEN_DOWN = 0,       /*0*/
    PEN_UP,             /*1*/
    PEN_MOVE,           /*2*/
    PEN_LONGTAP,        /*3*/
    PEN_REPEAT,         /*4*/
    PEN_ABORT,          /*5*/
    TP_UNKNOWN_EVENT,   /*6*/
    STROKE_MOVE,        /*7*/
    STROKE_STATICAL,    /*8*/
    STROKE_HOLD,        /*9*/
    PEN_LONGTAP_HOLD,   /*10*/
    PEN_REPEAT_HOLD,    /*11*/
    STROKE_DOWN_,       /*12*/
    STROKE_LONGTAP_,    /*13*/
    STROKE_UP_,         /*14*/
    STROKE_DOWN = 0xc0,     /*0*/      /*1*/
    STROKE_LONGTAP = 0x7e, /*8*/
    STROKE_UP = 0x7f        /*127*/
} touch_panel_event_enum_t;

typedef struct {
    // Touch_Panel_Event_enum
    touch_panel_event_enum_t event;
    /*coordinate point, not diff*/
    uint16_t x;
    uint16_t y;
    uint16_t z; //resistance TP: presure,  capacitive TP: area
} tp_single_event_t;

typedef struct {
    uint16_t         model;     // Single/Dual/Triple/Four/Five/All gesture
    uint16_t         padding;   //currently use for check the structure format correctness, 0xAA
    uint32_t         time_stamp;
    tp_single_event_t points[5];
} ctp_multiple_event_struct_t;
#endif

typedef struct {
    bool (*ctp_init)(void);
    bool (*ctp_set_device_mode)(ctp_device_mode_enum_t);
    ctp_pen_state_enum_t (*ctp_hisr)(void);
    bool (*ctp_get_data)(bsp_ctp_multiple_event_t *);
    bool (*ctp_parameters)(ctp_parameters_struct_t *, uint32_t, uint32_t);
    void (*ctp_power_on)(bool);
    uint32_t (*ctp_command)(ctp_control_command_enum_t, void *, void *);
    bool (*ctp_data_valid)(void);
} ctp_customize_function_struct;

typedef struct {
    bsp_ctp_callback_t      callback;           /*user's callback*/
    uint32_t                *pdata;         /*user's data pointer*/
} ctp_callback_t;

typedef enum {
    CTP_TIMER_SAMPLE_RATE_HIGH = 0,
    CTP_TIMER_SAMPLE_RATE_LOW
} ctp_gpt_rate_t;

typedef struct {
    bool                    is_buff_full;           /*inidcate if the buffer is full or not*/
    bool                    wait_next_down;         /*inidcate if the down point reasonable*/
    bool                    eint_mask;              /*inidcate if the eint mask or not*/
    bool                    skip_unrelease_state;
    bool                    send_touch_ilm;         /*inidcate if can call callback*/
    ctp_pen_state_enum_t    state;                  /*down or up*/

    int32_t                 x_coord_start;
    int32_t                 x_coord_end;
    int32_t                 y_coord_start;
    int32_t                 y_coord_end;

    int32_t                 verndor_x_coord_start;
    int32_t                 verndor_x_coord_end;
    int32_t                 verndor_y_coord_start;
    int32_t                 verndor_y_coord_end;

    ctp_callback_t          user_callback;          /*user's callback context*/

    bsp_ctp_gpt_sample_period_t timer_sample_rate;  /*timer sample, unit is ms*/
    uint32_t                ctp_gpt_handle;         /*gpt handle*/

    uint32_t                pen_move_offset;        /*pen move offset*/
    uint32_t                pen_longtap_offset;     /*longtap pen move offset*/
    uint32_t                pen_longtap_time;       /*stroke offset*/
    uint32_t                pen_repeat_time;        /*longtap stroke move offset*/

    uint32_t                first_hold_valid;
    uint32_t                support_max_points;     /*support multiple points number*/
} touch_panel_data_struct_t;

typedef struct {
    double x_scope;
    double x_offset;
    double y_scope;
    double y_offset;
} ctp_calibration_t;

typedef struct {
    bsp_ctp_multiple_event_t event[TOUCH_PANEL_BUFFER_EVENT_SIZE];
    uint16_t        touch_buffer_rindex;
    uint16_t        touch_buffer_windex;
    uint16_t        touch_buffer_last_rindex;
    uint16_t        touch_buffer_last_windex;
} touch_panel_buffer_data_t;

/********* varible extern *************/
extern ctp_config_t                     ctp_config_info;
extern ctp_customize_function_struct    *ctp_fun_ptr;
extern touch_panel_data_struct_t        ctp_context;
extern touch_panel_buffer_data_t        touch_panel_data_buffer;
extern bsp_ctp_multiple_event_t         g_ctp_event;
extern ctp_calibration_t                ctp_calibration;
extern bool  ctp_is_force_upgrade_firmware;       /*force update firmware*/

/******** funtion extern **************/
uint32_t ctp_get_time_us(void);
void ctp_delay_ms(uint32_t time);
void ctp_delay_us(uint32_t time);
uint32_t ctp_get_duration_time_us_to_ms(uint32_t start_count) ;
void ctp_get_buf_roomleft(uint16_t *pleft);

bool touch_custom_init(void);
bool touch_panel_init(void);
void touch_panel_enable(void);
void touch_panel_disable(void);
void touch_panel_reset_data(void);
bool touch_panel_capacitive_get_event(bsp_ctp_multiple_event_t *touch_data);
bool touch_panel_capacitive_peek_event(bsp_ctp_multiple_event_t *touch_data);
void ctp_mask_eint_interrupt(void);
void ctp_unmask_eint_interrupt(void);
void touch_panel_multiple_touch_set_timer(ctp_gpt_rate_t rate);
void touch_panel_multiple_touch_flush_buffer(void);
void touch_panel_multiple_touch_stop_timer(void);

void touch_panel_capacitive_power_on(bool on);

#ifdef __cplusplus
}
#endif

#endif /*MTK_CTP_ENABLE*/
#endif /*_CTP_H_*/

