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
//========== Dream Program for freeRTOS and IAR  2016.06.20 ==========
#if G2D_FreeRTOS
#include "syslog.h"
#define G2D_LOGI(fmt,...) LOG_I(common,  (fmt), ##__VA_ARGS__)
#else
#include <stdio.h>
#define G2D_LOGI printf
#endif
//====================================================================

#define G2D_HW_OVERLAY_TOTAL_LAYER 4

#if !defined(G2D_FPGA) //Dream add for make option
static bool _g2d_drv_overlay_check_window_range(hal_g2d_handle_t *handle)
{
    G2D_OVERLAY_STRUCT *overlayFunc = &(handle->overlayFunc);
    G2D_CANVAS_INFO_STRUCT *dstCanvas = &handle->dstCanvas;
    int32_t i;

    for (i = 0; i < G2D_HW_OVERLAY_TOTAL_LAYER; i++) {
        HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(overlayFunc->layerInfo[i]);

        if (true == (layerInfo->layerEnable)) {
            G2D_CANVAS_INFO_STRUCT *layerCanvas = &(layerInfo->layerCanvas);
            G2D_WINDOW_STRUCT *layerWindow = &(layerInfo->layerWindow);

            /// pitch = 0
            if (0 == (layerCanvas->width)) {
                return false;
            }
            /// width =0  height =0
            if ((0 > (layerWindow->buttomRight.x - layerWindow->topLeft.x)) || (0 > (layerWindow->buttomRight.y - layerWindow->topLeft.y))) {
                return false;
            }
        }
    }

    /// pitch = 0
    if (0 == (dstCanvas->width)) {
        G2D_LOGI("dstCanvas->width=0 fail\n");
        return false;
    }
    /// clip check
    if (true == handle->clipEnable) {
        G2D_WINDOW_STRUCT *clip_w = &handle->clipWindow;
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

#endif

static bool _g2d_drv_overlay_set_window_intersection(hal_g2d_handle_t *handle)
{
    G2D_WINDOW_STRUCT *clipWindow = &(handle->clipWindow);
    G2D_COORDINATE_STRUCT *clipWindowTopLeft     = &(clipWindow->topLeft);
    G2D_COORDINATE_STRUCT *clipWindowButtomRight = &(clipWindow->buttomRight);

    G2D_WINDOW_STRUCT *targetClipWindow = &(handle->targetClipWindow);


    int32_t topLeftX, topLeftY, buttomRightX, buttomRightY;

#if 0//!defined(G2D_FPGA) L
    int32_t roiWindow_topLeftX = (roiWindow->topLeft.x);
    int32_t roiWindow_topLeftY = (roiWindow->topLeft.y);
    int32_t roiWindow_buttomRightX = (roiWindow->buttomRight.x);
    int32_t roiWindow_buttomRightY = (roiWindow->buttomRight.y);

    if (false == handle->clipEnable) {
        topLeftX = roiWindow_topLeftX;
        topLeftY = roiWindow_topLeftY;

        buttomRightX = roiWindow_buttomRightX;
        buttomRightY = roiWindow_buttomRightY;
    } else {
        if ((clipWindowButtomRight->x) > roiWindow_buttomRightX) {
            buttomRightX = roiWindow_buttomRightX;
        } else {
            buttomRightX = clipWindowButtomRight->x;
        }
        if ((clipWindowButtomRight->y) > roiWindow_buttomRightY) {
            buttomRightY = roiWindow_buttomRightY;
        } else {
            buttomRightY = clipWindowButtomRight->y;
        }

        topLeftX = clipWindowTopLeft->x;
        topLeftY = clipWindowTopLeft->y;
    }
#else
    topLeftX = clipWindowTopLeft->x;
    topLeftY = clipWindowTopLeft->y;
    buttomRightX = clipWindowButtomRight->x;
    buttomRightY = clipWindowButtomRight->y;
#endif

    if (buttomRightX < topLeftX) {
        return false;
    }
    if (buttomRightY < topLeftY) {
        return false;
    }

    targetClipWindow->topLeft.x = topLeftX;
    targetClipWindow->topLeft.y = topLeftY;

    targetClipWindow->buttomRight.x = buttomRightX;
    targetClipWindow->buttomRight.y = buttomRightY;

    return true;
}



static void _g2d_drv_overlay_set_dst_info(hal_g2d_handle_t *handle)
{
    G2D_CANVAS_INFO_STRUCT *dstCanvas  = &(handle->dstCanvas);
    G2D_COORDINATE_STRUCT *dstCoordiante = &(handle->overlayFunc.dstCoordiante);
    uint32_t pitch;

    /// destination base addr
    ASSERT(dstCanvas->RGBBufferAddr);
    SET_G2D_W2M_ADDR((int32_t)dstCanvas->RGBBufferAddr);

    /// destination pitch
    pitch = (dstCanvas->width) * (dstCanvas->bytesPerPixel);
    ASSERT(((unsigned)pitch) <= 0x2000);
    SET_G2D_W2M_PITCH(pitch);

    /// -2048 <= x <= 2047
    ASSERT(((unsigned)((dstCoordiante->x) + 2048)) < 4096);
    ASSERT(((unsigned)((dstCoordiante->y) + 2048)) < 4096);
    /// destination rectangle start point(x,y)
    SET_G2D_W2M_OFFSET(dstCoordiante->x, dstCoordiante->y);

    /// setting destination color format bits
    switch (dstCanvas->colorFormat) {
        case HAL_G2D_COLOR_FORMAT_GRAY:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_GRAY);
            break;
        case HAL_G2D_COLOR_FORMAT_RGB565:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_RGB565);
            break;
        case HAL_G2D_COLOR_FORMAT_ARGB8888:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_ARGB8888);
            break;
        case HAL_G2D_COLOR_FORMAT_RGB888:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_RGB888);
            break;
        case HAL_G2D_COLOR_FORMAT_BGR888:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_BGR888);
            break;
        case HAL_G2D_COLOR_FORMAT_PARGB8888:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_PARGB8888);
            break;
        case HAL_G2D_COLOR_FORMAT_ARGB8565:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_ARGB8565);
            break;
        case HAL_G2D_COLOR_FORMAT_PARGB8565:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_PARGB8565);
            break;
        case HAL_G2D_COLOR_FORMAT_ARGB6666:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_ARGB6666);
            break;
        case HAL_G2D_COLOR_FORMAT_PARGB6666:
            SET_G2D_W2M_COLOR_FORMAT(G2D_ROI_CON_W2M_COLOR_PARGB6666);
            break;
        default:
            ASSERT(0);
            break;
    }
}



static void _g2d_drv_overlay_set_layer_info(hal_g2d_handle_t *handle)
{
    G2D_OVERLAY_STRUCT *overlayFunc = &(handle->overlayFunc);
    int32_t layer;

    for (layer = 0; layer < G2D_HW_OVERLAY_TOTAL_LAYER; layer++) {
        HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo = &(overlayFunc->layerInfo[layer]);
        G2D_WINDOW_STRUCT *layerWindow = &(layerInfo->layerWindow);
        G2D_CANVAS_INFO_STRUCT *layerCanvas = &(layerInfo->layerCanvas);

        if (true == (layerInfo->layerEnable)) {

            uint32_t layerWidth  = layerWindow->buttomRight.x - layerWindow->topLeft.x + 1;
            uint32_t layerHeight = layerWindow->buttomRight.y - layerWindow->topLeft.y + 1;
            uint32_t layerPitch;

            CLR_G2D_LAYER_CON(layer);
            switch (layerInfo->function) {
                case HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER: {
                    layerPitch = (layerCanvas->width) * (layerCanvas->bytesPerPixel);

                    if (true == (layerInfo->srcKeyEnable)) {
                        ENABLE_G2D_LAYER_CON_SRC_KEY(layer);
                        SET_G2D_LAYER_SRC_KEY(layer, layerInfo->srcKeyValue);
                    }
                    if (true == (layerInfo->layerAlphaEnable)) {
                        ENABLE_G2D_LAYER_CON_ALPHA(layer);
                        SET_G2D_LAYER_CON_ALPHA(layer, layerInfo->layerAlphaValue);
                    }
                    {
                        switch (layerCanvas->colorFormat) {
                            case HAL_G2D_COLOR_FORMAT_GRAY:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_GRAY);
                                break;
                            case HAL_G2D_COLOR_FORMAT_RGB565:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_RGB565);
                                break;
                            case HAL_G2D_COLOR_FORMAT_ARGB8888:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_ARGB8888);
                                break;
                            case HAL_G2D_COLOR_FORMAT_RGB888:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_RGB888);
                                break;
                            case HAL_G2D_COLOR_FORMAT_BGR888:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_BGR888);
                                break;
                            case HAL_G2D_COLOR_FORMAT_PARGB8888:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_PARGB8888);
                                break;
                            case HAL_G2D_COLOR_FORMAT_UYVY422:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_UYVY);
                                break;
                            case HAL_G2D_COLOR_FORMAT_ARGB8565:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_ARGB8565);
                                break;
                            case HAL_G2D_COLOR_FORMAT_PARGB8565:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_PARGB8565);
                                break;
                            case HAL_G2D_COLOR_FORMAT_ARGB6666:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_ARGB6666);
                                break;
                            case HAL_G2D_COLOR_FORMAT_PARGB6666:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_PARGB6666);
                                break;
                            default:
                                ASSERT(0);
                                break;
                        }
                    }
                }
                break;
                case HAL_G2D_OVERLAY_LAYER_FUNCTION_RECTFILL: {
                    layerPitch = (layerCanvas->width) * (layerCanvas->bytesPerPixel);

                    ENABLE_G2D_LAYER_CON_RECT_FILL(layer);
                    if (true == (layerInfo->srcKeyEnable)) {
                        ASSERT(0);
                    }
                    if (true == (layerInfo->layerAlphaEnable)) {
                        ENABLE_G2D_LAYER_CON_ALPHA(layer);
                        SET_G2D_LAYER_CON_ALPHA(layer, layerInfo->layerAlphaValue);
                    }
                    {
                        switch (layerCanvas->colorFormat) {
                            case HAL_G2D_COLOR_FORMAT_GRAY:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_GRAY);
                                break;
                            case HAL_G2D_COLOR_FORMAT_RGB565:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_RGB565);
                                break;
                            case HAL_G2D_COLOR_FORMAT_ARGB8888:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_ARGB8888);
                                break;
                            case HAL_G2D_COLOR_FORMAT_RGB888:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_RGB888);
                                break;
                            case HAL_G2D_COLOR_FORMAT_BGR888:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_BGR888);
                                break;
                            case HAL_G2D_COLOR_FORMAT_PARGB8888:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_PARGB8888);
                                break;
                            case HAL_G2D_COLOR_FORMAT_UYVY422:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_UYVY);
                                break;
                            case HAL_G2D_COLOR_FORMAT_ARGB8565:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_ARGB8565);
                                break;
                            case HAL_G2D_COLOR_FORMAT_PARGB8565:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_PARGB8565);
                                break;
                            case HAL_G2D_COLOR_FORMAT_ARGB6666:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_ARGB6666);
                                break;
                            case HAL_G2D_COLOR_FORMAT_PARGB6666:
                                SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_PARGB6666);
                                break;
                            default:
                                ASSERT(0);
                                break;
                        }
                    }
                    /// Set color;
                    SET_G2D_LAYER_RECTANGLE_FILL_COLOR(layer, (uint32_t)layerInfo->rectFillColor);
                }
                break;
                case HAL_G2D_OVERLAY_LAYER_FUNCTION_NORMAL_FONT: {
                    layerPitch = layerCanvas->width;

                    ENABLE_G2D_LAYER_CON_FONT(layer);
                    if ((true == (layerInfo->srcKeyEnable)) || (true == (layerInfo->layerAlphaEnable))) {
                        ASSERT(0);
                    }
                    ASSERT(HAL_G2D_ROTATE_ANGLE_0 == (layerInfo->rotation));

                    SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_ARGB8888);
                    layerCanvas->colorFormat = HAL_G2D_COLOR_FORMAT_ARGB8888;
                    SET_G2D_LAYER_FONT_FOREGROUND_COLOR(layer, layerInfo->foregroundColor);
                }
                break;
                case HAL_G2D_OVERLAY_LAYER_FUNCTION_AA_FONT: {
                    layerPitch = layerCanvas->width;

                    ENABLE_G2D_LAYER_CON_FONT(layer);
                    SET_G2D_LAYER_CON_AA_FONT_BIT(layer, (layerInfo->fontBit) << 28);

                    if ((true == (layerInfo->srcKeyEnable)) || (false == (layerInfo->layerAlphaEnable))) {
                        ASSERT(0);
                    }
                    ENABLE_G2D_LAYER_CON_ALPHA(layer);
                    SET_G2D_LAYER_CON_ALPHA(layer, 0xFF);

                    ASSERT(HAL_G2D_ROTATE_ANGLE_0 == (layerInfo->rotation));
                    layerCanvas->colorFormat = HAL_G2D_COLOR_FORMAT_ARGB8888;
                    SET_G2D_LAYER_CON_COLOR_FORMAT(layer, G2D_LX_CON_COLOR_ARGB8888);
                    SET_G2D_LAYER_FONT_FOREGROUND_COLOR(layer, layerInfo->foregroundColor);
                }
                break;
                default:
                    ASSERT(0);
            }
            /// rotation
            {
                switch (layerInfo->rotation) {
                    case HAL_G2D_ROTATE_ANGLE_0: {
                        SET_G2D_LAYER_CON_ROTATE(layer, G2D_LX_CON_CCW_ROTATE_000);
                        SET_G2D_LAYER_OFFSET(layer, layerWindow->topLeft.x, layerWindow->topLeft.y);
                    }
                    break;
                    case HAL_G2D_ROTATE_ANGLE_90: {
                        SET_G2D_LAYER_CON_ROTATE(layer, G2D_LX_CON_CCW_ROTATE_090);
                        SET_G2D_LAYER_OFFSET(layer, layerWindow->topLeft.x, layerWindow->topLeft.y + layerWidth - 1);
                    }
                    break;
                    case HAL_G2D_ROTATE_ANGLE_180: {
                        SET_G2D_LAYER_CON_ROTATE(layer, G2D_LX_CON_CCW_ROTATE_180);
                        SET_G2D_LAYER_OFFSET(layer, layerWindow->topLeft.x + layerWidth - 1, layerWindow->topLeft.y + layerHeight - 1);
                    }
                    break;
                    case HAL_G2D_ROTATE_ANGLE_270: {
                        SET_G2D_LAYER_CON_ROTATE(layer, G2D_LX_CON_CCW_ROTATE_270);
                        SET_G2D_LAYER_OFFSET(layer, layerWindow->topLeft.x + layerHeight - 1, layerWindow->topLeft.y);

                    }
                    break;
                    case HAL_G2D_ROTATE_ANGLE_MIRROR_0: {
                        SET_G2D_LAYER_CON_ROTATE(layer, G2D_LX_CON_CCW_ROTATE_MIRROR_000);
                        SET_G2D_LAYER_OFFSET(layer, layerWindow->topLeft.x + layerWidth - 1, layerWindow->topLeft.y);
                    }
                    break;
                    case HAL_G2D_ROTATE_ANGLE_MIRROR_90: {
                        SET_G2D_LAYER_CON_ROTATE(layer, G2D_LX_CON_CCW_ROTATE_MIRROR_090);
                        SET_G2D_LAYER_OFFSET(layer, layerWindow->topLeft.x, layerWindow->topLeft.y);
                    }
                    break;
                    case HAL_G2D_ROTATE_ANGLE_MIRROR_180: {
                        SET_G2D_LAYER_CON_ROTATE(layer, G2D_LX_CON_CCW_ROTATE_MIRROR_180);
                        SET_G2D_LAYER_OFFSET(layer, layerWindow->topLeft.x, layerWindow->topLeft.y + layerHeight - 1);
                    }
                    break;
                    case HAL_G2D_ROTATE_ANGLE_MIRROR_270: {
                        SET_G2D_LAYER_CON_ROTATE(layer, G2D_LX_CON_CCW_ROTATE_MIRROR_270);
                        SET_G2D_LAYER_OFFSET(layer, layerWindow->topLeft.x + layerHeight - 1, layerWindow->topLeft.y + layerWidth - 1);
                    }
                    break;
                    default:
                        ASSERT(0);
                        break;
                }
            }

            switch (layerCanvas->colorFormat) {
                case HAL_G2D_COLOR_FORMAT_GRAY:
                case HAL_G2D_COLOR_FORMAT_RGB565:
                case HAL_G2D_COLOR_FORMAT_ARGB8888:
                case HAL_G2D_COLOR_FORMAT_RGB888:
                case HAL_G2D_COLOR_FORMAT_BGR888:
                case HAL_G2D_COLOR_FORMAT_PARGB8888:
                case HAL_G2D_COLOR_FORMAT_ARGB8565:
                case HAL_G2D_COLOR_FORMAT_PARGB8565:
                case HAL_G2D_COLOR_FORMAT_ARGB6666:
                case HAL_G2D_COLOR_FORMAT_PARGB6666:
                    SET_G2D_LAYER_ADDR(layer, (uint32_t)(layerCanvas->RGBBufferAddr));
                    break;
                case HAL_G2D_COLOR_FORMAT_UYVY422:
                    SET_G2D_LAYER_ADDR(layer, (uint32_t)(layerCanvas->YUVBufferAddr[0]));
                    break;
                default:
                    ASSERT(0);
                    break;
            }

            ASSERT(((unsigned)layerPitch) <= 0x2000);
            SET_G2D_LAYER_PITCH(layer, layerPitch);

            /// This is set in rotation
            /// SET_G2D_LAYER_OFFSET(layer, layerWindow->topLeft.x, layerWindow->topLeft.y);
            SET_G2D_LAYER_SIZE(layer, layerWidth, layerHeight);
            ENABLE_G2D_ROI_LAYER(layer);
        }
    }
}



static void _g2d_drv_overlay_set_roi_info(hal_g2d_handle_t *handle)
{
    G2D_OVERLAY_STRUCT *overlayFunc = &(handle->overlayFunc);
    G2D_WINDOW_STRUCT *roiWindow = &(handle->overlayFunc.roiWindow);

    /// setting the avoidance and replacement colors
    g2d_drv_set_replacement_colors(handle);

    /// setting the clipping window
    g2d_drv_set_clipping_window(handle);

    SET_G2D_ROI_OFFSET(roiWindow->topLeft.x, roiWindow->topLeft.y);
    SET_G2D_ROI_SIZE(roiWindow->buttomRight.x - roiWindow->topLeft.x + 1, roiWindow->buttomRight.y - roiWindow->topLeft.y + 1);

#if defined(G2D_FPGA)
    if (true == (overlayFunc->backgroundDisable)) {
        DISABLE_G2D_ROI_CON_BG;
    } else {
        SET_G2D_ROI_CON_BG_COLOR(overlayFunc->backgroundColor);
    }
#else
    SET_G2D_ROI_CON_BG_COLOR(overlayFunc->backgroundColor);
#endif

    if (true == (overlayFunc->dstAlphaEnable)) {
        ENABLE_G2D_ROI_CON_ALPHA;
        SET_G2D_ROI_CON_ALPHA(overlayFunc->dstAlphaValue);
    }

    /// color setting is on _g2d_drv_overlay_set_dst_info
    //Enable Tile
    {
#if defined(G2D_FPGA)
        ///if(false == (handle->auto_tile_size_enable))
        ///{
        ///   REG_G2D_ROI_CON |= G2D_ROI_CON_ENABLE_FORCE_TS_BIT;
        ///   switch(handle->tile_size)
        ///   {
        ///      case G2D_TILE_SIZE_TYPE_8x8:
        ///         REG_G2D_ROI_CON |= G2D_ROI_CON_TILE_SIZE_8x8_BIT;
        ///      break;
        ///      case G2D_TILE_SIZE_TYPE_16x8:
        ///         REG_G2D_ROI_CON |= G2D_ROI_CON_TILE_SIZE_16x8_BIT;
        ///      break;
        ///      default:
        ///         ASSERT(0);
        ///   }
        ///}
#else
#endif
    }
}




#if defined(G2D_HW_C_MODEL_SUPPORT)
hal_g2d_status_t g2d_drv_c_model_start(hal_g2d_handle_t *handle);
#endif

hal_g2d_status_t g2d_drv_overlay_start(hal_g2d_handle_t *handle)
{
    {
        bool status;

#if !defined(G2D_FPGA)
        status = _g2d_drv_overlay_check_window_range(handle);
        if (false == status) {
            G2D_LOGI("_g2d_drv_overlay_check_window_range fail\n");//Dream add to check
            return HAL_G2D_STATUS_OK;
        }
#endif

        status = _g2d_drv_overlay_set_window_intersection(handle);
        if (false == status) {
            G2D_LOGI("_g2d_drv_overlay_set_window_intersection fail\n");//Dream add to check
            return HAL_G2D_STATUS_OK;
        }
    }

    HARD_RESET_G2D_ENGINE;
    REG_G2D_MODE_CTRL = 0;
    REG_G2D_ROI_CON = 0;

    /// setting the hw hisr callback function
    g2d_drv_set_hw_hisr_callback(&g2d_irq_handler);

    /// setting the lt destination infomation
    _g2d_drv_overlay_set_dst_info(handle);

    _g2d_drv_overlay_set_layer_info(handle);

    _g2d_drv_overlay_set_roi_info(handle);

    /// burst mode
    g2d_drv_set_burst_type(handle);

    /// slow down
    g2d_drv_set_slow_down(handle);

    g2d_drv_set_tile(handle);

    g2d_drv_set_dithering(handle);
    /// check if the callback function is setted, TURE => enable the interrupt configuration,
    ///                                          FALSE => disable the interrupt configuration
    if (true == handle->callbackEnable) {
        REG_G2D_IRQ |= G2D_IRQ_ENABLE_BIT;
    } else {
        REG_G2D_IRQ &= ~G2D_IRQ_ENABLE_BIT;
    }

    REG_G2D_MODE_CTRL |= G2D_MODE_CON_ENG_MODE_G2D_BITBLT_BIT;
    /// trigger
    REG_G2D_START = G2D_START_BIT;

#if defined(G2D_HW_C_MODEL_SUPPORT)
    g2d_drv_c_model_start(handle);
#endif

    return HAL_G2D_STATUS_OK;
}

#endif

