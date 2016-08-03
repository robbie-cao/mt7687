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

#ifdef MTK_CTP_ENABLE

#include <string.h>


#include "ctp.h"
#include "bsp_ctp.h"
#include "hal_gpt.h"
#include "hal_nvic.h"
#include "hal_log.h"
#include "hal_gpt_internal.h"
#include "hal_eint_internal.h"
#include <assert.h>

#ifdef CTP_USED_TASK_DEALY
#include "FreeRTOS.h"
#include "task.h"
#endif

ctp_config_t                    ctp_config_info;
ctp_customize_function_struct   *ctp_fun_ptr;
touch_panel_data_struct_t       ctp_context;
bsp_ctp_multiple_event_t        g_ctp_event;
touch_panel_buffer_data_t       touch_panel_data_buffer;
ctp_calibration_t               ctp_calibration;

static uint32_t tp_state_count[TP_SUPPORT_POINTS];

static int16_t  tp_accum_plh_x_diff[TP_SUPPORT_POINTS], tp_accum_plh_y_diff[TP_SUPPORT_POINTS];
static int16_t  tp_accum_prh_x_diff[TP_SUPPORT_POINTS], tp_accum_prh_y_diff[TP_SUPPORT_POINTS];
static uint32_t tp_state_start_tick[TP_SUPPORT_POINTS];

static bsp_ctp_event_status_t ctp_state_manchine_abort(int16_t x_diff, int16_t y_diff, uint16_t count);
static bsp_ctp_event_status_t ctp_state_manchine_pen_move(int16_t x_diff, int16_t y_diff, uint16_t count);
static bsp_ctp_event_status_t ctp_state_manchine_pen_down(int16_t x_diff, int16_t y_diff, uint16_t count);
static bsp_ctp_event_status_t ctp_state_manchine_pen_longtap_hold(int16_t x_diff, int16_t y_diff, uint16_t count);
static bsp_ctp_event_status_t ctp_state_manchine_pen_repeat_hold(int16_t x_diff, int16_t y_diff, uint16_t count);

extern void touch_panel_goodix_custom_data_init(void);
extern void touch_panel_ite_custom_data_init(void);
extern ctp_customize_function_struct *ctp_goodix_gt9xx_get_custom_function(void);
ctp_customize_function_struct *ctp_ite_7258_get_custom_function(void);

typedef bsp_ctp_event_status_t (*ctp_state_machine_function)(int16_t x_diff, int16_t y_diff, int16_t count);

const ctp_state_machine_function ctp_state_machine[] = {
    (ctp_state_machine_function)ctp_state_manchine_abort,           /*0 if last event is CTP_PEN_UP aseert*/
    (ctp_state_machine_function)ctp_state_manchine_pen_down,        /*1 if last event is CTP_PEN_DOWN*/
    (ctp_state_machine_function)ctp_state_manchine_pen_move,        /*2 if last event is CTP_PEN_MOVE*/
    (ctp_state_machine_function)ctp_state_manchine_pen_move,        /*3 if last evetn is CTP_PEN_LONGTAP*/
    (ctp_state_machine_function)ctp_state_manchine_pen_move,        /*4 if last evetn is CTP_PEN_REPEAT*/
    (ctp_state_machine_function)ctp_state_manchine_pen_longtap_hold, /*5 if last evetn is CTP_PEN_LONGTAP_HOLD*/
    (ctp_state_machine_function)ctp_state_manchine_pen_repeat_hold, /*6 if last evetn is CTP_PEN_REPEAT_HOLD*/
    (ctp_state_machine_function)ctp_state_manchine_abort,           /*7 if last evetn is CTP_PEN_ABORT*/
};

static void ctp_state_machine_absolute_value(int16_t *x_diff, int16_t *y_diff)
{
    if (*x_diff < 0) {
        *x_diff = -*x_diff;
    }
    if (*y_diff < 0) {
        *y_diff = -*y_diff;
    }
}

static bsp_ctp_event_status_t ctp_state_manchine_abort(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    assert(0);
    return CTP_PEN_DOWN;
}

static bsp_ctp_event_status_t ctp_state_manchine_pen_move(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    tp_state_count[count]      = 0;
    tp_state_start_tick[count] = ctp_get_time_us();

    /*convert to absolute vaule, otherwise compare with unsigned vaule will cause always larger*/
    ctp_state_machine_absolute_value(&x_diff, &y_diff);

    if ((x_diff > ctp_context.pen_move_offset) || (y_diff > ctp_context.pen_move_offset)) {
        return CTP_PEN_MOVE;
    } else {
        return CTP_PEN_REPEAT_HOLD; /*it may change to PEN_REPEAT in timer check.*/
    }
}

static bsp_ctp_event_status_t ctp_state_manchine_pen_down(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    tp_accum_plh_x_diff[count] = 0;
    tp_accum_plh_y_diff[count] = 0; /*reset the difference whenever pen down.*/

    tp_accum_prh_x_diff[count] = 0;
    tp_accum_prh_y_diff[count] = 0; /*otherwise it would accumulate last pen down-up diff.*/

    tp_state_count[count]      = 0;
    tp_state_start_tick[count] = ctp_get_time_us();

    /*cosmos already handle longtap, do not need to check longtap*/
    if (ctp_context.pen_longtap_offset == 0) {
        return ctp_state_machine[CTP_PEN_MOVE](x_diff, y_diff, count);
    }

    /*convert to absolute vaule, otherwise compare with unsigned vaule will cause always larger*/
    ctp_state_machine_absolute_value(&x_diff, &y_diff);

    if ((x_diff > ctp_context.pen_longtap_offset) || (y_diff > ctp_context.pen_longtap_offset)) {
        return CTP_PEN_MOVE;
    } else {
        tp_accum_plh_x_diff[count] += x_diff;
        tp_accum_plh_y_diff[count] += y_diff;
        return CTP_PEN_LONGTAP_HOLD; /*it may change to PEN_LOGNTAP in timer check.*/
    }

}

static bsp_ctp_event_status_t ctp_state_manchine_pen_longtap_hold(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    /*cosmos already handle longtap, do not need to check longtap*/
    if (ctp_context.pen_longtap_offset == 0) {
        return ctp_state_machine[CTP_PEN_MOVE](x_diff, y_diff, count);
    }

    tp_state_count[count]++;
    tp_accum_plh_x_diff[count] += x_diff;
    tp_accum_plh_y_diff[count] += y_diff;

    if ((tp_accum_plh_x_diff[count] && (tp_accum_plh_x_diff[count] > ctp_context.pen_longtap_offset))  || \
            (tp_accum_plh_y_diff[count] && (tp_accum_plh_y_diff[count] > ctp_context.pen_longtap_offset)) || \
            (-tp_accum_plh_x_diff[count] && (-tp_accum_plh_x_diff[count] > ctp_context.pen_longtap_offset)) || \
            (-tp_accum_plh_y_diff[count] && (-tp_accum_plh_y_diff[count] > ctp_context.pen_longtap_offset))) {
        tp_accum_plh_x_diff[count] = 0;
        tp_accum_plh_y_diff[count] = 0;
        return CTP_PEN_MOVE;
    } else {
        if (ctp_get_duration_time_us_to_ms(tp_state_start_tick[count]) >= ctp_context.pen_longtap_time) {
            tp_accum_plh_x_diff[count] = 0;
            tp_accum_plh_y_diff[count] = 0;
            return CTP_PEN_LONGTAP;
        } else {
            return CTP_PEN_LONGTAP_HOLD; //it may change to PEN_LOGNTAP in timer check.
        }
    }
}

static bsp_ctp_event_status_t ctp_state_manchine_pen_repeat_hold(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    tp_state_count[count]++;
    tp_accum_prh_x_diff[count] += x_diff;
    tp_accum_prh_y_diff[count] += y_diff;
    if ((tp_accum_prh_x_diff[count] && (tp_accum_prh_x_diff[count] > ctp_context.pen_move_offset))  ||
            (tp_accum_prh_y_diff[count] && (tp_accum_prh_y_diff[count] > ctp_context.pen_move_offset))  ||
            (-tp_accum_prh_x_diff[count] && (-tp_accum_prh_x_diff[count] > ctp_context.pen_move_offset)) ||
            (-tp_accum_prh_y_diff[count] && (-tp_accum_prh_y_diff[count] > ctp_context.pen_move_offset))) {
        tp_accum_prh_x_diff[count] = tp_accum_prh_y_diff[count] = 0;
        return CTP_PEN_MOVE;
    } else {
        if (ctp_get_duration_time_us_to_ms(tp_state_start_tick[count]) >= ctp_context.pen_repeat_time) {
            tp_accum_prh_x_diff[count] = 0;
            tp_accum_prh_y_diff[count] = 0;
            return CTP_PEN_REPEAT;
        } else {
            return CTP_PEN_REPEAT_HOLD;    //it may change to PEN_REPEAT in timer check.
        }
    }
}

void ctp_get_buf_roomleft(uint16_t *pleft)
{
    if (touch_panel_data_buffer.touch_buffer_rindex <= touch_panel_data_buffer.touch_buffer_windex) {
        *pleft = TOUCH_PANEL_BUFFER_EVENT_SIZE - touch_panel_data_buffer.touch_buffer_windex + touch_panel_data_buffer.touch_buffer_rindex;
    } else {
        *pleft = touch_panel_data_buffer.touch_buffer_rindex - touch_panel_data_buffer.touch_buffer_windex;
    }
}

uint32_t ctp_get_time_us(void)
{
    uint32_t count;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count);

    return count;
}

uint32_t ctp_get_duration_time_us_to_ms(uint32_t start_count)
{
    uint32_t current_count;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_count);
    if (current_count >= start_count) {
        return ((current_count - start_count) / 1000);
    } else {
        return ((0xffffffff - (start_count - current_count)) / 1000);
    }
}

void ctp_delay_us(uint32_t time)
{
    hal_gpt_delay_us(time);

}
void ctp_delay_ms(uint32_t time)
{
#ifdef CTP_USED_TASK_DEALY
    vTaskDelay(time / portTICK_PERIOD_MS);
#else
    hal_gpt_delay_ms(time);
#endif
}

void ctp_mask_eint_interrupt(void)
{
    hal_eint_mask(ctp_config_info.ctp_eint.eint_num);
    ctp_context.eint_mask = true;
}

void ctp_unmask_eint_interrupt(void)
{
    eint_ack_interrupt(ctp_config_info.ctp_eint.eint_num);
    hal_eint_unmask(ctp_config_info.ctp_eint.eint_num);
    ctp_context.eint_mask = false;
}

void ctp_mask_gpt_interrupt(void)
{

}

void ctp_unmask_gpt_interrupt(void)
{


}

void ctp_user_callback_handler(void)
{
    if (ctp_context.send_touch_ilm == true) {
        //call user's callback
        log_hal_info("[CTP][MSG]inform user to get data\r\n");
        ctp_context.send_touch_ilm = false;
        ctp_context.user_callback.callback(ctp_context.user_callback.pdata);
    }
}


bool ctp_calibrate_coordinate(bsp_ctp_multiple_event_t *tp_event)
{
    uint32_t i = 0;

    for (i = 0; i < tp_event->model; i++) { // check each point of last event in buffer.
        //log_hal_info("[CTP][cali]before calibration,x=%d, y =%d, model:%d, i:%d\r\n",tp_event->points[i].x, tp_event->points[i].y, tp_event->model,i);
        tp_event->points[i].x = (uint32_t)(ctp_calibration.x_scope * tp_event->points[i].x + ctp_calibration.x_offset);
        tp_event->points[i].y = (uint32_t)(ctp_calibration.y_scope * tp_event->points[i].y + ctp_calibration.y_offset);

        if (tp_event->points[i].x < ctp_context.x_coord_start) {
            tp_event->points[i].x = ctp_context.x_coord_start;
        } else if (tp_event->points[i].x > ctp_context.x_coord_end) {
            tp_event->points[i].x = ctp_context.x_coord_end;
        }

        if (tp_event->points[i].y < ctp_context.y_coord_start) {
            tp_event->points[i].y = ctp_context.y_coord_start;
        } else if (tp_event->points[i].y > ctp_context.y_coord_end) {
            tp_event->points[i].y = ctp_context.y_coord_end;
        }
        //log_hal_info("[CTP][cali]after calibration,x = %d, y = %d, model:%d, i:%d\r\n",tp_event->points[i].x, tp_event->points[i].y, tp_event->model,i);
    }


    return true;
}

void touch_panel_multiple_touch_flush_buffer(void)
{
    ctp_mask_eint_interrupt();

    touch_panel_multiple_touch_stop_timer();

    ctp_context.is_buff_full     = false;
    ctp_context.first_hold_valid = 0;

    //memset(&touch_panel_data_buffer, 0, sizeof(touch_panel_data_buffer));
    touch_panel_data_buffer.touch_buffer_rindex = 0;
    touch_panel_data_buffer.touch_buffer_windex = 0;
    touch_panel_data_buffer.touch_buffer_last_rindex = 0;
    touch_panel_data_buffer.touch_buffer_last_windex = 0;

    ctp_context.skip_unrelease_state = true;

    ctp_unmask_eint_interrupt();
}

static void touch_panel_push_one_event(bsp_ctp_multiple_event_t *push_event)
{
    uint16_t index;
    uint16_t i;

    index = touch_panel_data_buffer.touch_buffer_windex;

    touch_panel_data_buffer.touch_buffer_last_windex = index; //save last windex



    touch_panel_data_buffer.event[index].model      = push_event->model;
    touch_panel_data_buffer.event[index].padding    = push_event->padding;
    touch_panel_data_buffer.event[index].time_stamp = push_event->time_stamp;

    for (i = 0; i < touch_panel_data_buffer.event[index].model; i++) {
        touch_panel_data_buffer.event[index].points[i].event = push_event->points[i].event;
        touch_panel_data_buffer.event[index].points[i].x = push_event->points[i].x;
        touch_panel_data_buffer.event[index].points[i].y = push_event->points[i].y;
    }

    touch_panel_data_buffer.touch_buffer_windex++;
    if (touch_panel_data_buffer.touch_buffer_windex >= TOUCH_PANEL_BUFFER_EVENT_SIZE) {
        touch_panel_data_buffer.touch_buffer_windex = 0;
    }
}

static void touch_panel_multiple_touch_data_push(bsp_ctp_multiple_event_t *push_event)
{
    uint32_t roomleft = 0;
    uint32_t i;
    uint16_t left_size = 0;

    ctp_mask_gpt_interrupt();

    /* only mask eint when it's unmasked. */
    if (ctp_context.eint_mask == false) {
        ctp_mask_eint_interrupt();
    }

    if (ctp_context.is_buff_full == true) {
        log_hal_info("[CTP][push point]data buffer is full\r\n");
        return;
    }

    ctp_get_buf_roomleft((uint16_t *)(&left_size));

    roomleft = (uint32_t)left_size;

#ifdef CTP_DEBUG_LOG_PUSH_POP_DATA
    for (i = 0; i < push_event->model; i++) {
        log_hal_info("[CTP][push point]event=%d, x=%d, y=%d, left=%d, i=%d\r\n", \
                     push_event->points[i].event,   \
                     push_event->points[i].x,       \
                     push_event->points[i].y,       \
                     roomleft, \
                     i);
    }
#endif

    /*check if buffer is full*/
    if (roomleft <= 0) {
        ctp_context.is_buff_full = true;

        if (ctp_context.eint_mask == true) {
            //ctp_unmask_eint_interrupt();
        }
        log_hal_info("[CTP][push point]data buffer is not enough\r\n");
        //  ctp_unmask_gpt_interrupt();
        return;
    }

    touch_panel_push_one_event(push_event);

    ctp_user_callback_handler();

    if (ctp_context.eint_mask == true) {
        ctp_unmask_eint_interrupt();
    }

    ctp_unmask_gpt_interrupt();

}

void touch_panel_multipel_touch_retrieve_event(bsp_ctp_multiple_event_t *tp_event, uint16_t *index, bool increase_index)
{
    uint16_t current_index, i;

    current_index = *index;

    tp_event->model      = touch_panel_data_buffer.event[current_index].model;
    tp_event->padding    = touch_panel_data_buffer.event[current_index].padding;
    tp_event->time_stamp = touch_panel_data_buffer.event[current_index].time_stamp;


    for (i = 0; i < touch_panel_data_buffer.event[current_index].model; i++) {
        tp_event->points[i].event = touch_panel_data_buffer.event[current_index].points[i].event;
        tp_event->points[i].x     = touch_panel_data_buffer.event[current_index].points[i].x;
        tp_event->points[i].y     = touch_panel_data_buffer.event[current_index].points[i].y;
    }

    if (increase_index == true) {
        (*index)++;
        if ((*index) >= TOUCH_PANEL_BUFFER_EVENT_SIZE) {
            *index = 0;
        }
    }


    if (tp_event->model > TP_SUPPORT_POINTS) {
        log_hal_info("[CTP][retrieve]tp_event->model=%d [Error]\r\n", tp_event->model);
#ifdef CTP_DEBUG_LOG_RETRIEVE_DATA
        for (i = 0; i < tp_event->model; i++) {
            log_hal_info("[CTP][retrieve]event=%d, x=%d, y=%d, i=%d\r\n",
                         tp_event->points[i].event, \
                         tp_event->points[i].x,     \
                         tp_event->points[i].y,     \
                         i);
        }
        assert(0);
#endif
    }


}

bool touch_panel_capacitive_peek_event(bsp_ctp_multiple_event_t *touch_data)
{
    bool     result;
    uint8_t  i;
    uint16_t roomleft = 0;

    assert(touch_data);

    ctp_mask_gpt_interrupt();

    /* only mask eint when it's unmasked. */
    if (ctp_context.eint_mask == false) {
        ctp_mask_eint_interrupt();
    }

    if (touch_panel_data_buffer.touch_buffer_rindex != touch_panel_data_buffer.touch_buffer_windex) {
        touch_panel_multipel_touch_retrieve_event(touch_data, &touch_panel_data_buffer.touch_buffer_rindex, false);

        ctp_get_buf_roomleft(&roomleft);

        for (i = 0; i < touch_data->model; i++) {
            log_hal_info("[CTP][peek point]event=%d, x=%d, y=%d, left=%d, i=%d\r\n", \
                         touch_data->points[i].event,   \
                         touch_data->points[i].x,       \
                         touch_data->points[i].y,       \
                         roomleft, \
                         i);
        }
        result = true;
    } else {
        ctp_context.send_touch_ilm = true;
        result = false;
    }

    if (ctp_context.eint_mask == true) {
        ctp_unmask_eint_interrupt();
    }

    ctp_unmask_gpt_interrupt();

    return result;
}


bool touch_panel_capacitive_get_event(bsp_ctp_multiple_event_t *touch_data)
{
    bool        result;
    uint16_t    i;
    uint16_t    roomleft;

    if (touch_data == NULL) {
        log_hal_info("[CTP][get_event]parameter pointer pased by user is [NULL]\r\n");
        assert(0);
    }

    ctp_mask_gpt_interrupt();

    /* only mask eint when it's unmasked. */
    if (ctp_context.eint_mask == false) {
        ctp_mask_eint_interrupt();
    }

    if (ctp_context.is_buff_full == true) {
        touch_panel_multipel_touch_retrieve_event(touch_data, &touch_panel_data_buffer.touch_buffer_rindex, false);
        for (i = 0; i < touch_data->model; i++) {
            touch_data->points[i].event = CTP_PEN_ABORT;
            touch_data->points[i].x     = 0;
            touch_data->points[i].y     = 0;
        }
        if (ctp_context.eint_mask == true) {
            ctp_unmask_eint_interrupt();
        }
        ctp_unmask_gpt_interrupt();
        return true;
    }


    if (touch_panel_data_buffer.touch_buffer_rindex != touch_panel_data_buffer.touch_buffer_windex) {
        touch_panel_multipel_touch_retrieve_event(touch_data, &touch_panel_data_buffer.touch_buffer_rindex, true);

#ifdef CTP_DEBUG_LOG_PUSH_POP_DATA
        ctp_get_buf_roomleft(&roomleft);

        for (i = 0; i < touch_data->model; i++) {
            log_hal_info("[CTP][pop point]event=%d, x=%d, y=%d, left=%d, i=%d\r\n",
                         touch_data->points[i].event,   \
                         touch_data->points[i].x,       \
                         touch_data->points[i].y,       \
                         roomleft, \
                         i);
        }
#endif

        result = true;

    } else {
        ctp_context.send_touch_ilm = true;
        result = false;
    }

    if (ctp_context.eint_mask == true) {
        ctp_unmask_eint_interrupt();
    }

    ctp_unmask_gpt_interrupt();

    return result;
}


ctp_pen_state_enum_t touch_panel_capacitive_get_data(bsp_ctp_multiple_event_t *tp_event)
{
    bool     result = false;
    uint32_t uscnti1, uscnti2;

    uscnti1 = ctp_get_time_us();
    result = ctp_fun_ptr->ctp_get_data(tp_event);

    tp_event->time_stamp = ctp_get_time_us();

    uscnti2 = ctp_get_time_us();

    if (uscnti2 > uscnti1) {

    } else {

    }

    return result ? CTP_DOWN : CTP_UP;
}

bsp_ctp_event_status_t touch_panel_multiple_touch_state_machine(int16_t x_diff, int16_t y_diff, bsp_ctp_event_status_t pre_event, int16_t count)
{

#ifdef CTP_DEBUG_LOG
    log_hal_info("[CTP][machine]x_diff=%d, y_diff=%d, pre_event=%d, count=%d\r\n", x_diff, y_diff, pre_event, count);
#endif

    if (pre_event > CTP_PEN_ABORT) {
        log_hal_info("[CTP][machine]x_diff=%d, y_diff=%d, pre_event=%d, count=%d\r\n", x_diff, y_diff, pre_event, count);
        assert(0);
    }

    return ctp_state_machine[pre_event](x_diff, y_diff, count);
}

void touch_panel_multiple_touch_event_handler(bsp_ctp_multiple_event_t *tp_event, bsp_ctp_multiple_event_t *last_tp_event)
{
    /* This API may increase model, it depends on the pen up counts.*/
    uint32_t    i = 0, event;
    int16_t     x_diff, y_diff;
    uint32_t    event_need_handle;

    event_need_handle = (last_tp_event->model > tp_event->model) ? last_tp_event->model : tp_event->model;
    /*the last model may include up event.*/
    /*the current model do not include up event*/

#ifdef CTP_DEBUG_LOG
    log_hal_info("[CTP][Event] event handler last_tp_event->model=%d, tp_event->model=%d\r\n", last_tp_event->model, tp_event->model);
#endif

    /*check all points which needed to be handle*/
    for (i = 0; i < event_need_handle; i++) {
        /*not included in this time, must be up.*/
        if (i >= tp_event->model) {
            if ((last_tp_event->points[i].event == CTP_PEN_UP)) {
#ifdef CTP_DEBUG_LOG
                log_hal_info("[CTP][Event]CTP_PEN_UP again\r\n");
#endif

                event_need_handle = i;
                break;
            } else {
                event = CTP_PEN_UP;
                tp_event->points[i].x = last_tp_event->points[i].x;
                tp_event->points[i].y = last_tp_event->points[i].y;

#ifdef CTP_DEBUG_LOG
                log_hal_info("[CTP][Event]CTP_PEN_UP\r\n");
#endif
            }
        } else if ((i >= last_tp_event->model) || (last_tp_event->points[i].event == CTP_PEN_UP)) { /*new down*/
            event = CTP_PEN_DOWN;

#ifdef CTP_DEBUG_LOG
            log_hal_info("[CTP][Event]CTP_PEN_UP\r\n");
#endif

        } else { /* still down*/
            x_diff = tp_event->points[i].x - last_tp_event->points[i].x;
            y_diff = tp_event->points[i].y - last_tp_event->points[i].y;
            event = touch_panel_multiple_touch_state_machine(x_diff, y_diff, last_tp_event->points[i].event, i);
#ifdef CTP_DEBUG_LOG
            log_hal_info("[CTP][Event]CTP_STATE_MACHINE_RETURN %d\r\n", event);
#endif
        }
        tp_event->points[i].event = (bsp_ctp_event_status_t)event;
    }

    tp_event->model = event_need_handle;
}


void touch_panel_capacitive_up_handler(void)
{
    uint32_t                    i = 0;
    bsp_ctp_multiple_event_t tp_event;

    /*check if the down point is reasonable*/
    if (ctp_context.wait_next_down == true) {
        return;
    }

    if (ctp_context.skip_unrelease_state == false) {
        //touch_panel_multiple_touch_stop_timer();
        touch_panel_multipel_touch_retrieve_event(&tp_event, &touch_panel_data_buffer.touch_buffer_last_windex, false); //not increase index

        if (tp_event.points[0].event == CTP_PEN_UP) {
            //drv_trace0(TRACE_GROUP_4, CTP_UP_HDR_CTP_PEN_UP_AGAIN);
            return;
        }


        for (i = 0; i < tp_event.model; i++) { //set all exist event as UP
            if ((tp_event.points[i].event == CTP_PEN_UP)) {
                tp_event.model = i; //all other points already up in last event
                break;
            } else { /*non-handwriting*/
                tp_event.points[i].event = CTP_PEN_UP;
            }
        }
        //tp_event.time_stamp = L1I_GetTimeStamp();
        tp_event.time_stamp = ctp_get_time_us();
        touch_panel_multiple_touch_data_push(&tp_event);
    } else {
        /*TP.skip_unrelease_state==true*/
        ctp_context.skip_unrelease_state = false;
    }
}

bool touch_panel_multiple_touch_check_event_valid(bsp_ctp_multiple_event_t *tp_event)
{
    uint32_t i;

    for (i = 0; i < tp_event->model; i++) {
        if ((tp_event->points[i].event < CTP_PEN_LONGTAP_HOLD)) {
            ctp_context.first_hold_valid = 0;
            return true;
        }
    }
    ctp_context.first_hold_valid++;
    return false;
}

static void  touch_panel_multiple_touch_event_callback(void)
{
    bool                        valid_event;
    uint16_t                    model;
    bsp_ctp_multiple_event_t    *tp_event;
    bsp_ctp_multiple_event_t    last_tp_event;

#ifdef CTP_DEBUG_PERFORMANCE
    uint32_t time[5];
    time[0] = ctp_get_time_us();
#endif

    //log_hal_info("[CTP][1]eint mask status = %x\r\n", *(volatile uint32_t*)0xa2030320);
    //log_hal_info("[CTP][1]eint ack  status = %x\r\n", *(volatile uint32_t*)0xa2030300);

    tp_event = (bsp_ctp_multiple_event_t *)(&g_ctp_event);

    /*store mode*/
    model = tp_event->model;

    touch_panel_multipel_touch_retrieve_event(&last_tp_event, &touch_panel_data_buffer.touch_buffer_last_windex, false);

    touch_panel_multiple_touch_event_handler(tp_event, &last_tp_event); //feed PEN_MOVE or PEN_STATICAL

    valid_event = touch_panel_multiple_touch_check_event_valid(tp_event);

    tp_event->time_stamp = ctp_get_time_us();

    if (ctp_context.first_hold_valid <= 1) {
        touch_panel_multiple_touch_data_push(tp_event);
    }

    if (valid_event == true) {
        touch_panel_multiple_touch_set_timer(CTP_TIMER_SAMPLE_RATE_HIGH);
    } else {
        if (tp_event->model != 0) { //we need to know the event is up or just invalid event.
            touch_panel_multiple_touch_set_timer(CTP_TIMER_SAMPLE_RATE_LOW);
        }
    }

    /*restore mode*/
    tp_event->model = model;

    if (ctp_context.eint_mask == true) {
        ctp_unmask_eint_interrupt();
    }

#ifdef CTP_DEBUG_PERFORMANCE
    time[1] = ctp_get_time_us();
    log_hal_info("[CTP]timer callback whole flow time = %d us\r\n", time[1] - time[0]);

    //log_hal_info("[CTP][2]eint mask status = %x\r\n", *(volatile uint32_t*)0xa2030320);
    //log_hal_info("[CTP][2]eint ack  status = %x\r\n", *(volatile uint32_t*)0xa2030300);

#endif
}

void touch_panel_multiple_touch_stop_timer(void)
{
    hal_gpt_status_t return_status;

    return_status = hal_gpt_sw_stop_timer_ms(ctp_context.ctp_gpt_handle);
    if (return_status != HAL_GPT_STATUS_OK) {
        log_hal_info("[CTP][timer]stop timer faile return = %d error!\r\n", return_status);
    }

}

void touch_panel_multiple_touch_set_timer(ctp_gpt_rate_t rate)
{
    uint32_t time_out_ms;
    hal_gpt_status_t return_status;

    if (rate == CTP_TIMER_SAMPLE_RATE_HIGH) {
        if (ctp_context.timer_sample_rate.high_sample_period != 0) {
            time_out_ms = ctp_context.timer_sample_rate.high_sample_period;
        } else {
            time_out_ms = 10;       /*default set to 10 ms*/
        }
    } else if (rate == CTP_TIMER_SAMPLE_RATE_LOW) {
        if (ctp_context.timer_sample_rate.low_sample_period != 0) {
            time_out_ms = ctp_context.timer_sample_rate.low_sample_period;
        } else {
            time_out_ms = 30;       /*default set to 30 ms*/
        }

    } else {
        time_out_ms = 10;       /*default set to 10 ms*/
    }


    return_status = hal_gpt_sw_start_timer_ms(\
                    ctp_context.ctp_gpt_handle, \
                    time_out_ms, \
                    (hal_gpt_callback_t)touch_panel_multiple_touch_event_callback, \
                    NULL);
    if (return_status != HAL_GPT_STATUS_OK) {
        log_hal_info("[CTP][timer]start timer faile return = %d error!\r\n", return_status);
    }
}

void touch_panel_capacitive_down_handler(void)
{
    uint32_t i = 0;

    bsp_ctp_multiple_event_t tp_event;

    /* only mask eint when it's unmasked. */
    if (ctp_context.eint_mask == false) {
        ctp_mask_eint_interrupt();
    }

    memcpy((uint8_t *)(&tp_event), (uint8_t *)(&g_ctp_event), sizeof(bsp_ctp_multiple_event_t));

    if (ctp_context.eint_mask == true) {
        //ctp_unmask_eint_interrupt();
    }

    /*CTP_g_event may change during HISR prempt, if model become 0, ignore the touch*/
    if (tp_event.model == 0) {
        log_hal_info("[CTP][Down]tp_event.model = %d, ignore\r\n", tp_event.model);
        return;
    }

    ctp_context.skip_unrelease_state = false;
    ctp_context.first_hold_valid = 0;

    //TP.area = touch_panel_area_check(tp_event.points[0].x, tp_event.points[0].y);

    for (i = 0; i < tp_event.model; i++) {
#if 0
        if (TP.area == HAND_WRITING) { //if the first
            //touch_panel_capacitive_feed_event(STROKE_DOWN, &data, i);
            tp_event.points[i].event = STROKE_DOWN;
            CTP_para.Report_interval = TP.high_sample_period * 9; //GPT tick: 10ms per tick
        }

        else/*non-handwriting*/
#endif
        {
            //touch_panel_capacitive_feed_event(CTP_PEN_DOWN, &data, i);
            tp_event.points[i].event = CTP_PEN_DOWN;
            //CTP_para.Report_interval = TP.low_sample_period*9; //GPT tick: 10ms per tick
        }
    }
    //ctp_fun_ptr->ctp_parameters(&CTP_para, 0, CTP_PARA_REPORT_INTVAL);

    touch_panel_multiple_touch_data_push(&tp_event);
    touch_panel_multiple_touch_set_timer(CTP_TIMER_SAMPLE_RATE_HIGH);
}



void touch_panel_eint_handler(hal_eint_number_t eint_number)
{
    ctp_pen_state_enum_t    state            = CTP_UP;
    bool                    is_get_new_event = false;
    bsp_ctp_multiple_event_t current_tp_event, last_tp_event;

#ifdef CTP_DEBUG_PERFORMANCE
    uint32_t time[5];
    time[0] = ctp_get_time_us();
#endif

    eint_number = eint_number;

    if (ctp_hw_i2c_init(&ctp_config_info.ctp_i2c) == false) {
        ctp_unmask_eint_interrupt();

#ifdef CTP_DEBUG_PERFORMANCE
        time[1] = ctp_get_time_us();
        log_hal_info("[CTP]eint i2c init fail flow time = %d us\r\n", time[1] - time[0]);
#endif

        return;
    }

    log_hal_info("[CTP]enter eint handler \r\n");
    ctp_mask_eint_interrupt();

    /* if data is invalid, skip.*/
    if (ctp_fun_ptr->ctp_data_valid() == true) {

        /* get current points*/
        state  = touch_panel_capacitive_get_data(&current_tp_event);

        /* get last event from buffer*/
        touch_panel_multipel_touch_retrieve_event(&last_tp_event, &touch_panel_data_buffer.touch_buffer_last_windex, false);
        if ((last_tp_event.model <= g_ctp_event.model) || (g_ctp_event.model >= current_tp_event.model) || (g_ctp_event.model == 0)) {
            memcpy((uint8_t *)(&g_ctp_event), (uint8_t *)(&current_tp_event), sizeof(bsp_ctp_multiple_event_t));
            is_get_new_event = true;
            if ((g_ctp_event.model != last_tp_event.model) && (last_tp_event.model != 0) && (current_tp_event.model != 0)) { //pen count changes need handle it immediately
                //log_hal_info("[CTP]push new event\r\n");
                // do not handle first pen down and last pen up. tp task will handle that.
                ctp_calibrate_coordinate(&g_ctp_event);
                touch_panel_multiple_touch_event_handler(&g_ctp_event, &last_tp_event); //feed PEN_MOVE or PEN_STATICAL
                touch_panel_multiple_touch_data_push(&g_ctp_event);
                ctp_unmask_eint_interrupt();
                ctp_hw_i2c_deinit();

#ifdef CTP_DEBUG_PERFORMANCE
                time[1] = ctp_get_time_us();
                log_hal_info("[CTP]eint a new point down or up flow time = %d us\r\n", time[1] - time[0]);
#endif

                return;
            }
        }

        if (state == CTP_DOWN) { /*low*/
            if (is_get_new_event) { /* Don't need to change to coordinate again, if not get new event */
                ctp_calibrate_coordinate(&g_ctp_event);
            }
            if (ctp_context.state == CTP_UP) {
                ctp_context.state = state;

                log_hal_info("[CTP][Down] enter down handler\r\n");
                touch_panel_capacitive_down_handler();
            } else { /* not trigger TP task if down->down */

            }

        } else { /*high*/
            if (ctp_context.state == CTP_DOWN) {
                touch_panel_multiple_touch_stop_timer();
                ctp_context.state = state;
                log_hal_info("[CTP][UP] enter up handler\r\n");
                touch_panel_capacitive_up_handler();
            } else {
                log_hal_info("[CTP] eint_handler up_to_up %d\r\n", ctp_context.state);
            }
        }

    }
    ctp_unmask_eint_interrupt();
    ctp_hw_i2c_deinit();

#ifdef CTP_DEBUG_PERFORMANCE
    time[1] = ctp_get_time_us();
    log_hal_info("[CTP]eint whole flow time = %d us\r\n", time[1] - time[0]);
#endif

    //log_hal_info("[CTP][3]eint mask status = %x\r\n", *(volatile uint32_t*)0xa2030320);
    //log_hal_info("[CTP][3]eint ack  status = %x\r\n", *(volatile uint32_t*)0xa2030300);

}

static double ctp_tunning_caculate_scope(int32_t p1, int32_t p2, int32_t p3, int32_t p4)
{
    return ((double)(p2 - p1) / (double)(p4 - p3));
}

static double ctp_tunning_caculate_offset(int32_t p1, int32_t p2, double scope)
{
    return ((double)p1 - scope * ((double)(p2)));

}
void touch_panel_tunning(touch_panel_data_struct_t *tp_data, ctp_calibration_t *calibration)
{
    calibration->x_scope = ctp_tunning_caculate_scope(tp_data->x_coord_end, \
                           tp_data->x_coord_start, \
                           tp_data->verndor_x_coord_end, \
                           tp_data->verndor_x_coord_start);

    calibration->x_offset = ctp_tunning_caculate_offset(tp_data->x_coord_start, \
                            tp_data->verndor_x_coord_start, \
                            calibration->x_scope);

    calibration->y_scope = ctp_tunning_caculate_scope(tp_data->y_coord_end, \
                           tp_data->y_coord_start, \
                           tp_data->verndor_y_coord_end, \
                           tp_data->verndor_y_coord_start);

    calibration->y_offset = ctp_tunning_caculate_offset(tp_data->y_coord_start, \
                            tp_data->verndor_y_coord_start, \
                            calibration->y_scope);

}


void touch_custom_parameter_init(void)
{

    touch_panel_tunning(&ctp_context, &ctp_calibration);

    if (ctp_context.pen_longtap_time == 0) {
        ctp_context.pen_longtap_time    = 800;  /*set longtap time default 800ms*/
    }

    if (ctp_context.pen_repeat_time == 0) {
        ctp_context.pen_repeat_time     = 500;  /*set repeat  time default 500ms*/
    }
}

bool touch_custom_init(void)
{
    ctp_delay_ms(3);

    /*ite7258 init*/
    touch_panel_reset_data();
    touch_panel_ite_custom_data_init();
    touch_custom_parameter_init();
    ctp_fun_ptr = ctp_ite_7258_get_custom_function();
    if (ctp_fun_ptr->ctp_init() == true) {
        return true;
    }

    /*goodix gt9137*/
    touch_panel_reset_data();
    touch_panel_goodix_custom_data_init();
    touch_custom_parameter_init();
    ctp_fun_ptr = ctp_goodix_gt9xx_get_custom_function();
    if (ctp_fun_ptr->ctp_init() == true) {
        return true;
    }

    return false;
}

void touch_panel_reset_data(void)
{
    memset(&ctp_context, 0, sizeof(ctp_context));
    memset(&g_ctp_event, 0, sizeof(g_ctp_event));
    memset(&ctp_config_info, 0, sizeof(ctp_config_info));
    memset(&touch_panel_data_buffer, 0, sizeof(touch_panel_data_buffer));
    memset(&ctp_calibration, 0, sizeof(ctp_calibration));
}
bool touch_panel_init(void)
{
    hal_gpt_status_t return_stats;
    ctp_mask_eint_interrupt();

    if (touch_custom_init() == false) {
        return false;
    }

    ctp_context.is_buff_full    = false;
    ctp_context.wait_next_down  = false;
    ctp_context.send_touch_ilm  = true;

    /*allocate software timer*/
    return_stats = hal_gpt_sw_get_timer(&ctp_context.ctp_gpt_handle);
    if (return_stats != HAL_GPT_STATUS_OK) {
        log_hal_info("[CTP][init]gpt timer handle = 0x%x error, status=%d!\r\n", ctp_context.ctp_gpt_handle, return_stats);
    }


    /* set eint trigger mode and debounce time */

    hal_eint_init(ctp_config_info.ctp_eint.eint_num, &ctp_config_info.ctp_eint.init_config);
    hal_eint_register_callback(ctp_config_info.ctp_eint.eint_num, (hal_eint_callback_t)touch_panel_eint_handler, NULL);

    return true;
}

void touch_panel_enable(void)
{
    hal_eint_unmask(ctp_config_info.ctp_eint.eint_num);
    NVIC_EnableIRQ(EINT_IRQn);
}

void touch_panel_disable(void)
{
    hal_eint_mask(ctp_config_info.ctp_eint.eint_num);

}


void touch_panel_capacitive_power_on(bool on)
{
    ctp_fun_ptr->ctp_power_on(on);
}

bool touch_panel_capacitive_power_state(ctp_device_mode_enum_t mode)
{
    return ctp_fun_ptr->ctp_set_device_mode(mode);
}

uint32_t touch_panel_capacitive_command(uint32_t cmd, void *p1, void *p2)
{
    return ctp_fun_ptr->ctp_command((ctp_control_command_enum_t)cmd, p1, p2);
}

bool touch_panel_capacitive_set_device(ctp_device_mode_enum_t mode)
{
    return ctp_fun_ptr->ctp_set_device_mode(mode);
}

#endif /*MTK_CTP_ENABLE*/

