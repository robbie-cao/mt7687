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

#include "hal_g2d.h"

#ifdef HAL_G2D_MODULE_ENABLED

#include "hal_g2d_define.h"

#if 1//defined(G2D_HW_RECT_FILL_SUPPORT) L

#include "hal_g2d_drv_api.h"
extern bool INT_QueryExceptionStatus(void);
int INT_QueryIsNonCachedRAM(uint32_t addr, int size);




//Dream: 2523 assert
#include "assert.h"
#define ASSERT assert

//========== Dream Program for freeRTOS and IAR  2016.06.20 ==========
#if G2D_FreeRTOS
#include "syslog.h"
#define G2D_LOGI(fmt,...) LOG_I(common,  (fmt), ##__VA_ARGS__)
#else
#include <stdio.h>
#define G2D_LOGI printf
#endif


//=====================================================================================

hal_g2d_status_t hal_g2d_rectfill_set_destination_window(hal_g2d_handle_t *handle, int32_t x, int32_t y, int32_t width, int32_t height)
{
    G2D_WINDOW_STRUCT *dstWindow = &(handle->rectFillFunc.dstWindow);
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    if (HAL_G2D_CODEC_TYPE_HW == (handle->type)) {
        status = g2d_drv_check_dst_window(x, y, width, height);
    }

    dstWindow->topLeft.x = x;
    dstWindow->topLeft.y = y;
    dstWindow->buttomRight.x = x + width - 1;
    dstWindow->buttomRight.y = y + height - 1;

    return status;
}



hal_g2d_status_t hal_g2d_rectfill_set_color(hal_g2d_handle_t *handle, uint32_t color)
{
    G2D_RECTANGLE_FILL_STRUCT *rectFillFunc = &(handle->rectFillFunc);

    rectFillFunc->rectFillColor = color;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_rectangle_fill_query_window_range(hal_g2d_handle_t *handle,
        int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h)
{
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    switch (handle->type) {
        case HAL_G2D_CODEC_TYPE_HW: {
            status = g2d_drv_query_dst_window(min_x, max_x, min_y, max_y, min_w, max_w, min_h, max_h);
        }
        break;
        default:
            ASSERT(0);
            break;
    }

    return status;
}


hal_g2d_status_t hal_g2d_rectfill_start(hal_g2d_handle_t *handle)
{
    G2D_LOGI("hal_g2d_rectangle_fill_start start\n\r");
    switch (handle->type) {
        case HAL_G2D_CODEC_TYPE_HW: {
#if 1//defined(__MTK_TARGET__)
            if (true != INT_QueryExceptionStatus()) {
                //kal_take_mutex(g2d_state_mutex);L
                handle->g2dState = G2D_STATE_READY;
                //kal_give_mutex(g2d_state_mutex);L
            } else
#endif
            {
                handle->g2dState = G2D_STATE_READY;
            }

#if 1//defined(__MTK_TARGET__)
            {
                G2D_CANVAS_INFO_STRUCT *dstCanvas = &(handle->dstCanvas);
                ASSERT(INT_QueryIsNonCachedRAM((uint32_t)(dstCanvas->RGBBufferAddr), dstCanvas->RGBBufferSize));
            }
#endif
            g2d_drv_rectfill_start(handle);

#if 1//defined(__MTK_TARGET__)
            if (true != INT_QueryExceptionStatus()) {
                //kal_take_mutex(g2d_state_mutex);L
                handle->g2dState = G2D_STATE_BUSY;
                //kal_give_mutex(g2d_state_mutex);L
            } else
#endif
            {
                handle->g2dState = G2D_STATE_BUSY;
            }
        }
        break;
        default:
            ASSERT(0);
            break;
    }
    //G2D_LOGI("hal_g2d_rectangle_fill_start end\n\r");

    return HAL_G2D_STATUS_OK;
}

#else

hal_g2d_status_t hal_g2d_rectangle_set_destination_window(hal_g2d_handle_t *handle, int32_t x, int32_t y, int32_t width, int32_t height)
{
    return HAL_G2D_STATUS_NOT_SUPPORT;
}



hal_g2d_status_t hal_g2d_rectangle_fill_set_color(hal_g2d_handle_t *handle, uint32_t color)
{
    return HAL_G2D_STATUS_NOT_SUPPORT;
}



hal_g2d_status_t hal_g2d_rectangle_fill_query_window_range(hal_g2d_handle_t *handle,
        int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h)
{
    return HAL_G2D_STATUS_NOT_SUPPORT;
}


hal_g2d_status_t hal_g2d_rectangle_fill_start(hal_g2d_handle_t *handle)
{
    return HAL_G2D_STATUS_NOT_SUPPORT;
}


#endif


#endif

