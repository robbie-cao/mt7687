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

#ifndef __G2D_DRV_API_H__
#define __G2D_DRV_API_H__

//#include "g2d_enum.h"	    this header have to after g2d_define  2016.06.04
//#include "g2d_structure.h"

/// function prototype for common driver function
bool g2d_irq_handler(void *g2d_status);


hal_g2d_status_t g2d_drv_bitblt_start(hal_g2d_handle_t *handle);
hal_g2d_status_t g2d_drv_bitblt_query_src_alpha_support(void);

hal_g2d_status_t g2d_drv_font_start(hal_g2d_handle_t *handle);
hal_g2d_status_t g2d_drv_font_query_aa_font_support(void);
hal_g2d_status_t g2d_drv_font_query_tilt_font_support(void);

hal_g2d_status_t g2d_drv_sad_start(hal_g2d_handle_t *handle);
hal_g2d_status_t g2d_drv_sad_pause(hal_g2d_handle_t *handle);
hal_g2d_status_t g2d_drv_get_sad_sum(uint32_t *g2d_sad_sum);

hal_g2d_status_t g2d_drv_lt_start(hal_g2d_handle_t *handle);
hal_g2d_status_t g2d_drv_lt_query_src_alpha_support(void);
hal_g2d_status_t g2d_drv_lt_query_dst_alpha_support(void);

hal_g2d_status_t g2d_drv_rectfill_start(hal_g2d_handle_t *handle);
hal_g2d_status_t g2d_drv_overlay_start(hal_g2d_handle_t *handle);

void g2d_drv_set_clipping_window(hal_g2d_handle_t *handle);

void g2d_drv_set_replacement_colors(hal_g2d_handle_t *handle);

void g2d_drv_set_source_color_key(hal_g2d_handle_t *handle,
                                  hal_g2d_color_format_t src_color_format);

void g2d_drv_set_max_outstanding(hal_g2d_handle_t *handle);

void g2d_drv_power_on(void);
void g2d_drv_power_off(void);
void g2d_drv_init(void);
void g2d_drv_deinit(void);
void g2d_drv_set_hw_hisr_callback(hal_g2d_callback_t hisr_callback);
int32_t g2d_drv_get_hw_status(void);

void g2d_drv_set_dithering(hal_g2d_handle_t *handle);


void g2d_drv_set_burst_type(hal_g2d_handle_t *handle);
void g2d_drv_set_slow_down(hal_g2d_handle_t *handle);
void g2d_drv_set_tile(hal_g2d_handle_t *handle);


hal_g2d_status_t g2d_drv_check_dst_canvas_info(uint32_t pitch);
hal_g2d_status_t g2d_drv_check_src_canvas_info(uint32_t pitch);

hal_g2d_status_t g2d_drv_check_src_window(int32_t x, int32_t y, uint32_t w, uint32_t h);
hal_g2d_status_t g2d_drv_check_dst_window(int32_t x, int32_t y, uint32_t w, uint32_t h);

hal_g2d_status_t g2d_drv_check_lt_src_window(int32_t x, int32_t y, uint32_t w, uint32_t h);
hal_g2d_status_t g2d_drv_check_lt_dst_window(int32_t x, int32_t y, uint32_t w, uint32_t h);

hal_g2d_status_t g2d_drv_check_clip_window(int32_t x, int32_t y, uint32_t x_max_clip, uint32_t y_max_clip);

hal_g2d_status_t g2d_drv_query_src_canvas_info(uint32_t *min_width, uint32_t *max_width, uint32_t *min_height, uint32_t *max_height);
hal_g2d_status_t g2d_drv_query_dst_canvas_info(uint32_t *min_width, uint32_t *max_width, uint32_t *min_height, uint32_t *max_height);

hal_g2d_status_t g2d_drv_query_src_window(int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h);
hal_g2d_status_t g2d_drv_query_dst_window(int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h);
hal_g2d_status_t g2d_drv_query_lt_src_window(int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h);
hal_g2d_status_t g2d_drv_query_lt_dst_window(int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h);


#endif


