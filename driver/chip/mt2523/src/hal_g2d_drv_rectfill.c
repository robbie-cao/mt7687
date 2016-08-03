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
#include "hal_g2d_drv_internal.h"
#include "hal_g2d_drv_api.h"

#if defined(G2D_HW_C_MODEL_SUPPORT)
extern uint32_t g2d_hw_registers[100];
#endif


//Dream: 2523 assert
#include "assert.h"
#define ASSERT assert


static void _g2d_drv_rectfill_set_info(hal_g2d_handle_t *handle)
{
    G2D_RECTANGLE_FILL_STRUCT *rectFillFunc = &(handle->rectFillFunc);
    G2D_CANVAS_INFO_STRUCT *dstCanvas  = &(handle->dstCanvas);
#if defined(G2D_FPGA)
    G2D_WINDOW_STRUCT *dstWindow  = &(handle->rectFillFunc.dstWindow);
#else
    G2D_WINDOW_STRUCT  *dstWindow = &(handle->targetClipWindow);
#endif
    G2D_COORDINATE_STRUCT *topLeft     = &(dstWindow->topLeft);
    G2D_COORDINATE_STRUCT *buttomRight = &(dstWindow->buttomRight);
    uint32_t width, height, pitch;


    /// destination base addr
    ASSERT(dstCanvas->RGBBufferAddr);
    REG_G2D_W2M_ADDR = (int32_t)dstCanvas->RGBBufferAddr;

    /// destination pitch
    pitch = (dstCanvas->width) * (dstCanvas->bytesPerPixel);
    ASSERT(((unsigned)pitch) <= 0x2000);
    SET_G2D_LAYER_PITCH(1, pitch);
    REG_G2D_W2M_PITCH = pitch;

    /// -2048 <= x <= 2047
    ASSERT(((unsigned)((topLeft->x) + 2048)) < 4096);
    ASSERT(((unsigned)((topLeft->y) + 2048)) < 4096);
    /// destination rectangle start point(x,y)
    SET_G2D_W2M_OFFSET(topLeft->x, topLeft->y);
    SET_G2D_ROI_OFFSET(topLeft->x, topLeft->y);
    SET_G2D_LAYER_OFFSET(1, topLeft->x, topLeft->y);

    width = buttomRight->x - topLeft->x + 1;
    height = buttomRight->y - topLeft->y + 1;

    /// 1 <= x <= 2048
    ASSERT(((unsigned)(width - 1)) <= 2048);
    ASSERT(((unsigned)(height - 1)) <= 2048);
    /// destination rectangle width and height
    REG_G2D_ROI_SIZE = (width << 16) | height;
    SET_G2D_LAYER_SIZE(1, width, height);

    /// setting destination color format bits
    switch (dstCanvas->colorFormat) {
        case HAL_G2D_COLOR_FORMAT_RGB565:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_RGB565);
            SET_G2D_LAYER_CON_COLOR_FORMAT(1, G2D_LX_CON_COLOR_RGB565);
            break;
        case HAL_G2D_COLOR_FORMAT_ARGB8888:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_ARGB8888);
            SET_G2D_LAYER_CON_COLOR_FORMAT(1, G2D_LX_CON_COLOR_ARGB8888);
            break;
        case HAL_G2D_COLOR_FORMAT_RGB888:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_RGB888);
            SET_G2D_LAYER_CON_COLOR_FORMAT(1, G2D_LX_CON_COLOR_RGB888);
            break;
        case HAL_G2D_COLOR_FORMAT_BGR888:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_BGR888);
            SET_G2D_LAYER_CON_COLOR_FORMAT(1, G2D_LX_CON_COLOR_BGR888);
            break;
        case HAL_G2D_COLOR_FORMAT_PARGB8888:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_PARGB8888);
            SET_G2D_LAYER_CON_COLOR_FORMAT(1, G2D_LX_CON_COLOR_PARGB8888);
            break;
        default:
            ASSERT(0);
            break;
    }

    /// Set color;
    SET_G2D_LAYER_RECTANGLE_FILL_COLOR(1, (uint32_t)rectFillFunc->rectFillColor);

    ENABLE_G2D_ROI_LAYER(1);
}



static bool _g2d_drv_rectfill_check_window_range(hal_g2d_handle_t *handle)
{
    G2D_CANVAS_INFO_STRUCT *dstCanvas  = &(handle->dstCanvas);
    G2D_WINDOW_STRUCT  *dstWindow = &(handle->rectFillFunc.dstWindow);
    G2D_COORDINATE_STRUCT *dstTopLeft = &(dstWindow->topLeft);
    G2D_COORDINATE_STRUCT *dstButtomRight = &(dstWindow->buttomRight);

    /// pitch = 0
    if (0 == (dstCanvas->width)) {
        return false;
    }
    /// width = 0  height = 0
    if ((0 > (dstButtomRight->x - dstTopLeft->x)) || (0 > (dstButtomRight->y - dstTopLeft->y))) {
        return false;
    }

    /// clip check
    if (true == handle->clipEnable) {
        G2D_WINDOW_STRUCT *clip_w = &(handle->clipWindow);
        G2D_COORDINATE_STRUCT *clipTopLeft = &(clip_w->topLeft);
        G2D_COORDINATE_STRUCT *clipButtomRight = &(clip_w->buttomRight);

        if (((clipButtomRight->x) < 0) && ((clipTopLeft->x) < 0)) {
            return false;
        }
        if (((clipButtomRight->y) < 0) && ((clipTopLeft->y) < 0)) {
            return false;
        }
        /// width =0  height =0
        if ((0 > (clipButtomRight->x - clipTopLeft->x)) || (0 > (clipButtomRight->y - clipTopLeft->y))) {
            return false;
        }
    }
    return true;
}



static bool _g2d_drv_rectfill_set_window_intersection(hal_g2d_handle_t *handle)
{
    G2D_WINDOW_STRUCT *clipWindow = &(handle->clipWindow);
    G2D_COORDINATE_STRUCT *clipWindowTopLeft     = &(clipWindow->topLeft);
    G2D_COORDINATE_STRUCT *clipWindowButtomRight = &(clipWindow->buttomRight);
    G2D_WINDOW_STRUCT *dstWindow  = &(handle->rectFillFunc.dstWindow);
    G2D_COORDINATE_STRUCT *dstWindowTopLeft     = &(dstWindow->topLeft);
    G2D_COORDINATE_STRUCT *dstWindowButtomRight = &(dstWindow->buttomRight);
    G2D_WINDOW_STRUCT *targetClipWindow = &(handle->targetClipWindow);

    uint32_t dstCanvasWidth  = handle->dstCanvas.width;
    uint32_t dstCanvasHeight = handle->dstCanvas.height;

    if (false == handle->clipEnable) {
        clipWindowTopLeft->x = dstWindowTopLeft->x;
        clipWindowTopLeft->y = dstWindowTopLeft->y;

        clipWindowButtomRight->x = dstWindowButtomRight->x;
        clipWindowButtomRight->y = dstWindowButtomRight->y;
    } else {
        clipWindowTopLeft->x = MAX(0, clipWindowTopLeft->x);
        clipWindowTopLeft->y = MAX(0, clipWindowTopLeft->y);

        clipWindowTopLeft->x = MAX(dstWindowTopLeft->x, clipWindowTopLeft->x);
        clipWindowTopLeft->y = MAX(dstWindowTopLeft->y, clipWindowTopLeft->y);

        clipWindowButtomRight->x = MIN(dstWindowButtomRight->x, clipWindowButtomRight->x);
        clipWindowButtomRight->y = MIN(dstWindowButtomRight->y, clipWindowButtomRight->y);

        if ((clipWindowButtomRight->x) > ((int)(dstCanvasWidth - 1))) {
            clipWindowButtomRight->x = dstCanvasWidth - 1;
        }
        if ((clipWindowButtomRight->y) > ((int)(dstCanvasHeight - 1))) {
            clipWindowButtomRight->y = dstCanvasHeight - 1;
        }
    }

    /// width =0  height =0
    if ((0 > (clipWindowButtomRight->x - clipWindowTopLeft->x)) || (0 > (clipWindowButtomRight->y - clipWindowTopLeft->y))) {
        return false;
    }

    targetClipWindow->topLeft.x = clipWindowTopLeft->x;
    targetClipWindow->topLeft.y = clipWindowTopLeft->y;

    targetClipWindow->buttomRight.x = clipWindowButtomRight->x;
    targetClipWindow->buttomRight.y = clipWindowButtomRight->y;

    return true;
}



#if defined(G2D_HW_C_MODEL_SUPPORT)
hal_g2d_status_t g2d_drv_c_model_start(hal_g2d_handle_t *handle);
#endif

hal_g2d_status_t g2d_drv_rectfill_start(hal_g2d_handle_t *handle)
{
    {
        bool status;
        status = _g2d_drv_rectfill_check_window_range(handle);
        if (false == status) {
            return HAL_G2D_STATUS_OK;
        }

        status = _g2d_drv_rectfill_set_window_intersection(handle);
        if (false == status) {
            return HAL_G2D_STATUS_OK;
        }
    }

    HARD_RESET_G2D_ENGINE;
    REG_G2D_MODE_CTRL = 0;
    REG_G2D_ROI_CON = 0;
    CLR_G2D_LAYER_CON(1);
    ENABLE_G2D_LAYER_CON_RECT_FILL(1);

    /// setting the hw hisr callback function
    g2d_drv_set_hw_hisr_callback(&g2d_irq_handler);

    /// setting the avoidance and replacement colors
    g2d_drv_set_replacement_colors(handle);

    /// setting the clipping window
    g2d_drv_set_clipping_window(handle);

    /// setting the bitblt destination infomation
    _g2d_drv_rectfill_set_info(handle);

    g2d_drv_set_dithering(handle);

    /// disable bg
    REG_G2D_ROI_CON |= G2D_ROI_CON_DISABLE_BG_BIT;

    /// burst mode
    g2d_drv_set_burst_type(handle);

    /// slow down
    g2d_drv_set_slow_down(handle);

    if (true == handle->callbackEnable) {
        REG_G2D_IRQ |= G2D_IRQ_ENABLE_BIT;
        //G2D_LOGI("hal_g2d_rectangle_fill_start irq enable\n\r");
    } else {
        REG_G2D_IRQ &= ~G2D_IRQ_ENABLE_BIT;
        //G2D_LOGI("hal_g2d_rectangle_fill_start irq disable\n\r");
    }

    REG_G2D_MODE_CTRL |= G2D_MODE_CON_ENG_MODE_G2D_BITBLT_BIT;
    /// trigger the rectangle fill
    REG_G2D_START = G2D_START_BIT;

#if defined(G2D_HW_C_MODEL_SUPPORT)
    g2d_drv_c_model_start(handle);
#endif

    return HAL_G2D_STATUS_OK;
}

#endif 

