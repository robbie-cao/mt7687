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


#include "hal_g2d_drv_api.h"

extern bool INT_QueryExceptionStatus(void);
int INT_QueryIsNonCachedRAM(uint32_t addr, int size);

//Dream: 2523 assert
#include "assert.h"
#define ASSERT assert

#define G2D_HW_OVERLAY_TOTAL_LAYER 4

hal_g2d_status_t hal_g2d_overlay_enable_layer(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, bool layerEnable)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);

    layerInfo->layerEnable = layerEnable;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_function(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, hal_g2d_overlay_layer_function_t function)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);

    layerInfo->function = function;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_font_bit(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, g2d_font_bit_t fontBit)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);

    layerInfo->fontBit = fontBit;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_font_fg_color(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint32_t foregroundColor)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);

    layerInfo->foregroundColor = foregroundColor;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_rectfill_color(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint32_t rectFillColor)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);

    layerInfo->rectFillColor = rectFillColor;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_buffer(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer,
        uint8_t *buffer_address, uint32_t buffer_size)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);
    G2D_CANVAS_INFO_STRUCT *layerCanvas = &(layerInfo->layerCanvas);
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    layerCanvas->RGBBufferAddr = buffer_address;
    layerCanvas->RGBBufferSize = buffer_size;
    layerCanvas->colorFormat = HAL_G2D_COLOR_FORMAT_ARGB8888;

    return status;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_rgb_buffer(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer,
        uint8_t *bufferAddr, uint32_t bufferSize,
        uint32_t width, uint32_t height, hal_g2d_color_format_t colorFormat)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);
    G2D_CANVAS_INFO_STRUCT *layerCanvas = &(layerInfo->layerCanvas);
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    layerCanvas->RGBBufferAddr = bufferAddr;
    layerCanvas->RGBBufferSize = bufferSize;
    layerCanvas->width = width;
    layerCanvas->height = height;
    layerCanvas->colorFormat = colorFormat;

    switch (colorFormat) {
        case HAL_G2D_COLOR_FORMAT_GRAY:
            layerCanvas->bytesPerPixel = 1;
            break;
        case HAL_G2D_COLOR_FORMAT_RGB565 :
            layerCanvas->bytesPerPixel = 2;
            break;
        case HAL_G2D_COLOR_FORMAT_RGB888:
        case HAL_G2D_COLOR_FORMAT_BGR888:
        case HAL_G2D_COLOR_FORMAT_ARGB8565:
        case HAL_G2D_COLOR_FORMAT_PARGB8565:
        case HAL_G2D_COLOR_FORMAT_ARGB6666:
        case HAL_G2D_COLOR_FORMAT_PARGB6666:
            layerCanvas->bytesPerPixel = 3;
            break;
        case HAL_G2D_COLOR_FORMAT_ARGB8888:
        case HAL_G2D_COLOR_FORMAT_PARGB8888:
            layerCanvas->bytesPerPixel = 4;
            break;
        default:
            ASSERT(0);
            break;
    }

    if (HAL_G2D_CODEC_TYPE_HW == (handle->type)) {
        status = g2d_drv_check_src_canvas_info(width);

    }

    return status;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_yuv_buffer(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer,
        uint8_t *bufferAddr[3], uint32_t bufferSize[3],
        uint32_t width, uint32_t height, hal_g2d_color_format_t colorFormat)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);
    G2D_CANVAS_INFO_STRUCT *layerCanvas = &(layerInfo->layerCanvas);
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    layerCanvas->YUVBufferAddr[0] = bufferAddr[0];
    layerCanvas->YUVBufferAddr[1] = bufferAddr[1];
    layerCanvas->YUVBufferAddr[2] = bufferAddr[2];
    layerCanvas->YUVBufferSize[0] = bufferSize[0];
    layerCanvas->YUVBufferSize[1] = bufferSize[1];
    layerCanvas->YUVBufferSize[2] = bufferSize[2];
    layerCanvas->width  = width;
    layerCanvas->height       = height;
    layerCanvas->colorFormat = colorFormat;


    switch (colorFormat) {
        case HAL_G2D_COLOR_FORMAT_UYVY422:
            layerCanvas->bytesPerPixel = 2;
            break;
        default:
            ASSERT(0);
            break;
    }

    if (HAL_G2D_CODEC_TYPE_HW == (handle->type)) {
        status = g2d_drv_check_src_canvas_info(width);

    }

    return status;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_window(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);
    G2D_WINDOW_STRUCT *layerWindow = &(layerInfo->layerWindow);
    G2D_CANVAS_INFO_STRUCT *layerCanvas = &(layerInfo->layerCanvas);
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    if (HAL_G2D_CODEC_TYPE_HW == (handle->type)) {
        status = g2d_drv_check_src_window(x, y, w, h);
    }

    layerWindow->topLeft.x     = x;
    layerWindow->topLeft.y     = y;
    layerWindow->buttomRight.x = x + (signed)w - 1;
    layerWindow->buttomRight.y = y + (signed)h - 1;

    if ((HAL_G2D_OVERLAY_LAYER_FUNCTION_NORMAL_FONT == (layerInfo->function)) || (HAL_G2D_OVERLAY_LAYER_FUNCTION_AA_FONT == (layerInfo->function))) {
        layerCanvas->width = w;
        layerCanvas->bytesPerPixel = 1;

        if (HAL_G2D_CODEC_TYPE_HW == (handle->type)) {
            status = g2d_drv_check_src_canvas_info(w);
        }
    }

    return status ;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_source_key(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint32_t srcKeyValue)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);

    
    layerInfo->srcKeyValue = srcKeyValue;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_rotation(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, hal_g2d_rotate_angle_t rotation)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);

    layerInfo->rotation = rotation;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_layer_alpha(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint32_t layerAlphaValue)
{
    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);

    layerInfo->layerAlphaEnable = true;
    layerInfo->layerAlphaValue = layerAlphaValue;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_background_color(hal_g2d_handle_t *handle, uint32_t backgroundColor)
{
    G2D_OVERLAY_STRUCT *overlayFunc = &(handle->overlayFunc);

    overlayFunc->backgroundColor = backgroundColor;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_alpha(hal_g2d_handle_t *handle, uint32_t dstAlphaValue)
{
    G2D_OVERLAY_STRUCT *overlayFunc = &(handle->overlayFunc);

    overlayFunc->dstAlphaEnable = true;
    overlayFunc->dstAlphaValue = dstAlphaValue;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_window(hal_g2d_handle_t *handle, int32_t x, int32_t y)
{
    G2D_COORDINATE_STRUCT *dstCoordiante = &(handle->overlayFunc.dstCoordiante);

    dstCoordiante->x = x;
    dstCoordiante->y = y;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_overlay_set_roi_window(hal_g2d_handle_t *handle, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    G2D_WINDOW_STRUCT *roiWindow = &(handle->overlayFunc.roiWindow);
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    if (HAL_G2D_CODEC_TYPE_HW == (handle->type)) {
        status = g2d_drv_check_src_window(x, y, w, h);
    }

    roiWindow->topLeft.x     = x;
    roiWindow->topLeft.y     = y;
    roiWindow->buttomRight.x = x + (signed)w - 1;
    roiWindow->buttomRight.y = y + (signed)h - 1;

    return status ;
}


hal_g2d_status_t hal_g2d_overlay_start(hal_g2d_handle_t *handle)
{
    switch (handle->type) {
        case HAL_G2D_CODEC_TYPE_HW: {
            if (true != INT_QueryExceptionStatus()) {
                //kal_take_mutex(g2d_state_mutex);L
                handle->g2dState = G2D_STATE_READY;
                //kal_give_mutex(g2d_state_mutex);L
            } else

            {
                handle->g2dState = G2D_STATE_READY;
            }

            {
                G2D_CANVAS_INFO_STRUCT *dstCanvas = &(handle->dstCanvas);

                int32_t layer;

                for (layer = 0; layer < G2D_HW_OVERLAY_TOTAL_LAYER; layer++) {
                    HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(handle->overlayFunc.layerInfo[layer]);
                    G2D_CANVAS_INFO_STRUCT *layerCanvas = &(layerInfo->layerCanvas);

                    if (HAL_G2D_COLOR_FORMAT_UYVY422 != (layerCanvas->colorFormat)) {
                        ASSERT(INT_QueryIsNonCachedRAM((uint32_t)(layerCanvas->RGBBufferAddr), layerCanvas->RGBBufferSize));
                    } else {
                        int32_t colorSize;

                        /// YUV
                        for (colorSize = 0; colorSize < 3; colorSize++) {
                            if (0 != (layerCanvas->YUVBufferSize[colorSize])) {
                                ASSERT(INT_QueryIsNonCachedRAM((uint32_t)(layerCanvas->YUVBufferAddr[colorSize]), layerCanvas->YUVBufferSize[colorSize]));
                            }
                        }
                    }
                }

                ASSERT(INT_QueryIsNonCachedRAM((uint32_t)(dstCanvas->RGBBufferAddr), dstCanvas->RGBBufferSize));
            }

            g2d_drv_overlay_start(handle);


            if (true != INT_QueryExceptionStatus()) {
                //kal_take_mutex(g2d_state_mutex);L
                handle->g2dState = G2D_STATE_BUSY;
                //kal_give_mutex(g2d_state_mutex);L
            } else

            {
                handle->g2dState = G2D_STATE_BUSY;
            }
        }
        break;
        default:
            ASSERT(0);
            break;
    }

    return HAL_G2D_STATUS_OK;
}

#endif


