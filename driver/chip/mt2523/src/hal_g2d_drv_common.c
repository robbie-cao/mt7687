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
#include <stdio.h>

#include "hal_g2d.h"

#ifdef HAL_G2D_MODULE_ENABLED
#include "hal_g2d_define.h"
//#include "g2d_macro.h"
#include "hal_g2d_drv_internal.h"

#include "mt2523.h"   //Dream: you can find G2D_IRQn for Larkdpur
#include "hal_nvic_internal.h" //Dream: NVIC_SetPriority(G2D_IRQn, G2D_IRQ_PRIORITY);

#if 0
#include "FreeRTOS.h"	//RTOS function
#include "queue.h"	//Queue
#include "semphr.h"	//Queue
#endif

//Dream: 2523 assert
#include "assert.h"
#define ASSERT assert

//Dream: Larkspur===============
#if G2D_FreeRTOS
#include "syslog.h"
#define G2D_LOGI(fmt,...) LOG_I(common,  (fmt), ##__VA_ARGS__)
#else
#define G2D_LOGI printf
#endif
//==============================

#if defined(G2D_HW_C_MODEL_SUPPORT)
extern uint32_t g2d_hw_registers[100];
#endif

static hal_g2d_callback_t _g2d_hw_hisr_callback = NULL;

#if 1

static uint8_t _g2d_hw_init_flag = 0;

#if 0//!defined(G2D_FPGA) For Larkspur
#include "mm_power_ctrl.h"    /// mm power gating

///#include "hisr_config.h"
#include "drv_gfx_hisr_ctrl.h"

#ifndef __MAUI_BASIC__
extern uint8_t L1SM_GetHandle(void);
extern void L1SM_SleepEnable(uint8_t handle);
extern void L1SM_SleepDisable(uint8_t handle);
uint8_t   g2d_sleep_mode_handler_new = 0xFF;
#endif
#endif


#if 0
void g2d_drv_isr_hisr(void)
{
    CLEAR_G2D_INT_STATUS;

    if (!_g2d_hw_hisr_callback) {
        NVIC_EnableIRQ(G2D_IRQn);//IRQUnmask(IRQ_G2D_CODE);
        return;
    } else {
        (*_g2d_hw_hisr_callback)(NULL);
    }

    NVIC_EnableIRQ(G2D_IRQn);//IRQUnmask(IRQ_G2D_CODE);
}
static void  _g2d_drv_isr_lisr(void)
{
    G2D_LOGI("_g2d_drv_isr_lisr! \r\n");
    g2d_drv_isr_hisr();
    /*Larkspur
       IRQMask(IRQ_G2D_CODE);

       #if defined(G2D_FPGA)
          g2d_drv_isr_hisr();
       #else
          drv_gfx_hisr_activate(DRV_GFX_HISR_MODULE_G2D);
       #endif*/
}
#endif

typedef struct {
    int msg_id;
} G2D_MSG;
#ifdef G2D_USE_Task
extern QueueHandle_t G2D_Queue;
#endif
void g2d_drv_isr(void)
{
    CLEAR_G2D_INT_STATUS;

    if (!_g2d_hw_hisr_callback) {
        NVIC_EnableIRQ(G2D_IRQn);//IRQUnmask(IRQ_G2D_CODE);
        return;
    } else {
#ifdef G2D_USE_Task
        G2D_MSG msg;
        msg.msg_id = 200;
        xQueueSendFromISR(G2D_Queue, (void *) &msg, (TickType_t) 0);
#else
        (*_g2d_hw_hisr_callback)(NULL);
#endif
    }
    NVIC_EnableIRQ(G2D_IRQn);//IRQUnmask(IRQ_G2D_CODE);
}



static void _g2d_drv_isr_init()//Dream Program it on 2015.12.01
{
    hal_nvic_register_isr_handler(G2D_IRQn, (hal_nvic_isr_t)g2d_drv_isr);
    NVIC_SetPriority(G2D_IRQn, G2D_IRQ_PRIORITY);
    NVIC_EnableIRQ(G2D_IRQn);
}



void g2d_drv_power_on(void)
{
#if 0//!defined(G2D_FPGA) Larkspur
#ifndef __MAUI_BASIC__
    L1SM_SleepDisable(g2d_sleep_mode_handler_new);
#endif

    /// Power on G2D Engine
    mm_enable_power(MMPWRMGR_G2D);

    /// Power on G2D command queue
    ///mm_enable_power(MMPWRMGR_GCMQ);
#endif
}



void g2d_drv_power_off(void)
{
#if 0//!defined(G2D_FPGA) Larkspur
    /// Power off G2D
    mm_disable_power(MMPWRMGR_G2D);

    /// Power off G2D command queue
    ///mm_disable_power(MMPWRMGR_GCMQ);

#ifndef __MAUI_BASIC__
    L1SM_SleepEnable(g2d_sleep_mode_handler_new);
#endif
#endif
}



void g2d_drv_init(void)
{
    HARD_RESET_G2D_ENGINE;

    if (0 == _g2d_hw_init_flag) {
#if 0//defined(G2D_FPGA) Dream: Larkspur have other IRQ way
        EnableIRQ();
        //SAL_IRQ_Code_Unmask(IRQ_G2D_CODE);
        //register_isr(IRQ_G2D_CODE, _g2d_drv_isr_lisr);
        register_isr_code(IRQ_G2D_CODE, _g2d_drv_isr_lisr);
        //SAL_IRQ_Code_Sens(IRQ_G2D_CODE, LEVEL_SENSITIVE);
#endif

        //IRQSensitivity(IRQ_G2D_CODE, LEVEL_SENSITIVE); L

        _g2d_drv_isr_init();

#if 0//!defined(G2D_FPGA) Larkspur
#ifndef __MAUI_BASIC__
        if (g2d_sleep_mode_handler_new == 0xFF) {
            g2d_sleep_mode_handler_new = L1SM_GetHandle();
        }
#endif
#endif
        NVIC_EnableIRQ(G2D_IRQn);//IRQUnmask(IRQ_G2D_CODE);
    } else {
        _g2d_hw_init_flag = 1;
    }
}



#endif



int32_t g2d_drv_get_hw_status()
{
    return (REG_G2D_STATUS & HAL_G2D_STATUS_BUSY_BIT);
}



void g2d_drv_set_replacement_colors(hal_g2d_handle_t *handle)
{
    /// setting the avoidance and replacement color based on destination color format
    if (true == handle->colorReplaceEnable) {
        /// enable color replacement bit
        REG_G2D_ROI_CON |= G2D_ROI_CON_REPLACEMENT_ENABLE_BIT;

        switch (handle->dstCanvas.colorFormat) {
            case HAL_G2D_COLOR_FORMAT_RGB565: {
                REG_G2D_AVOIDANCE_COLOR = G2D_RGB888_TO_RGB565(handle->colorToAvoid.red, handle->colorToAvoid.green,
                                          handle->colorToAvoid.blue);

                REG_G2D_REPLACEMENT_COLOR = G2D_RGB888_TO_RGB565(handle->colorToReplace.red, handle->colorToReplace.green,
                                            handle->colorToReplace.blue);
            }
            break;
            case HAL_G2D_COLOR_FORMAT_PARGB8888:
            case HAL_G2D_COLOR_FORMAT_ARGB8888: {
                REG_G2D_AVOIDANCE_COLOR = G2D_ARGB8888_PACKED(handle->colorToAvoid.alpha, handle->colorToAvoid.red,
                                          handle->colorToAvoid.green, handle->colorToAvoid.blue);

                REG_G2D_REPLACEMENT_COLOR = G2D_ARGB8888_PACKED(handle->colorToReplace.alpha, handle->colorToReplace.red,
                                            handle->colorToReplace.green, handle->colorToReplace.blue);

            }
            break;
            case HAL_G2D_COLOR_FORMAT_RGB888:
            case HAL_G2D_COLOR_FORMAT_BGR888: {
                REG_G2D_AVOIDANCE_COLOR = G2D_RGB888_PACKED(handle->colorToAvoid.red, handle->colorToAvoid.green,
                                          handle->colorToAvoid.blue);

                REG_G2D_REPLACEMENT_COLOR = G2D_RGB888_PACKED(handle->colorToReplace.red, handle->colorToReplace.green,
                                            handle->colorToReplace.blue);


            }
            break;
            case HAL_G2D_COLOR_FORMAT_ARGB8565:
            case HAL_G2D_COLOR_FORMAT_PARGB8565: {

                REG_G2D_AVOIDANCE_COLOR = G2D_ARGB8888_TO_ARGB8565(handle->colorToAvoid.alpha, handle->colorToAvoid.red,
                                          handle->colorToAvoid.green, handle->colorToAvoid.blue);


                REG_G2D_REPLACEMENT_COLOR = G2D_ARGB8888_TO_ARGB8565(handle->colorToAvoid.alpha, handle->colorToAvoid.red,
                                            handle->colorToAvoid.green, handle->colorToAvoid.blue);
            }
            break;
            case HAL_G2D_COLOR_FORMAT_ARGB6666:
            case HAL_G2D_COLOR_FORMAT_PARGB6666: {
                REG_G2D_AVOIDANCE_COLOR = G2D_ARGB8888_TO_ARGB6666(handle->colorToAvoid.alpha, handle->colorToAvoid.red,
                                          handle->colorToAvoid.green, handle->colorToAvoid.blue);


                REG_G2D_REPLACEMENT_COLOR = G2D_ARGB8888_TO_ARGB6666(handle->colorToAvoid.alpha, handle->colorToAvoid.red,
                                            handle->colorToAvoid.green, handle->colorToAvoid.blue);
            }
            break;
            default:
                ASSERT(0);
                break;

        }
    }
}



void g2d_drv_set_clipping_window(hal_g2d_handle_t *handle)
{
    G2D_WINDOW_STRUCT *targetClipWindow = &handle->targetClipWindow;

    int32_t topLeftX = targetClipWindow->topLeft.x;
    int32_t topLeftY = targetClipWindow->topLeft.y;
    int32_t buttomRightX = targetClipWindow->buttomRight.x;
    int32_t buttomRightY = targetClipWindow->buttomRight.y;

    /// -2048 <= x <= 2047
    ASSERT(((unsigned)(topLeftX + 2048)) < 4096);
    ASSERT(((unsigned)(topLeftY + 2048)) < 4096);
    ASSERT(((unsigned)(buttomRightX  + 2048)) < 4096);
    ASSERT(((unsigned)(buttomRightY + 2048)) < 4096);

#if defined(G2D_FPGA)
    if (handle->clipEnable) {
        REG_G2D_ROI_CON |= G2D_ROI_CON_CLIP_ENABLE_BIT;
    } else {
        REG_G2D_ROI_CON &= ~G2D_ROI_CON_CLIP_ENABLE_BIT;
    }
#else
    REG_G2D_ROI_CON |= G2D_ROI_CON_CLIP_ENABLE_BIT;
#endif

    /// clip_min
    REG_G2D_CLP_MIN = (((int16_t)(topLeftX) & 0xFFFF) << 16) | ((int16_t)(topLeftY) & 0xFFFF);
    /// clip_max
    REG_G2D_CLP_MAX = (((int16_t)(buttomRightX) & 0xFFFF) << 16) | ((int16_t)(buttomRightY) & 0xFFFF);
}



void g2d_drv_set_source_color_key(hal_g2d_handle_t *handle, hal_g2d_color_format_t colorFormat)
{
    if (true == handle->srcKeyEnable) {
        uint32_t src_key;

        ENABLE_G2D_LAYER_CON_SRC_KEY(1);

        switch (colorFormat) {
            case HAL_G2D_COLOR_FORMAT_RGB565: {
                src_key = G2D_RGB888_TO_RGB565(handle->srcKeyValue.red, handle->srcKeyValue.green, handle->srcKeyValue.blue);
            }
            break;
            case HAL_G2D_COLOR_FORMAT_ARGB8888:
            case HAL_G2D_COLOR_FORMAT_PARGB8888: {
                src_key = G2D_ARGB8888_PACKED(handle->srcKeyValue.alpha, handle->srcKeyValue.red, handle->srcKeyValue.green, handle->srcKeyValue.blue);

            }
            break;
            case HAL_G2D_COLOR_FORMAT_RGB888:
            case HAL_G2D_COLOR_FORMAT_BGR888: {
                src_key = G2D_RGB888_PACKED(handle->srcKeyValue.red, handle->srcKeyValue.green, handle->srcKeyValue.blue);

            }
            break;
            case HAL_G2D_COLOR_FORMAT_ARGB8565:
            case HAL_G2D_COLOR_FORMAT_PARGB8565: {
                src_key = G2D_ARGB8888_TO_ARGB8565(handle->srcKeyValue.alpha, handle->srcKeyValue.red, handle->srcKeyValue.green, handle->srcKeyValue.blue);
            }
            break;
            case HAL_G2D_COLOR_FORMAT_ARGB6666:
            case HAL_G2D_COLOR_FORMAT_PARGB6666: {
                src_key = G2D_ARGB8888_TO_ARGB6666(handle->srcKeyValue.alpha, handle->srcKeyValue.red, handle->srcKeyValue.green, handle->srcKeyValue.blue);
            }
            break;
            default:
                ASSERT(0);
                break;

        }
        SET_G2D_LAYER_SRC_KEY(1, src_key);
    }
}



void g2d_drv_set_dithering(hal_g2d_handle_t *handle)
{
    if (true == (handle->ditheringEnable)) {
        if (G2D_DITHERING_MODE_RANDOM_ALGORITHM == (handle->ditheringMode)) {
            SET_G2D_DI_CON(handle->ditherBitR, handle->ditherBitG, handle->ditherBitB, 0x1);

#if 0//defined(__MTK_TARGE__) Dream: Larkspur
            {
                extern unsigned int drv_get_current_time(void);
                uint32_t seed = drv_get_current_time();

                SET_G2D_MW_INIT(seed);
                SET_G2D_MZ_INIT(seed);

            }
#elif 1 //defined(G2D_FPGA)
            SET_G2D_MW_INIT(handle->ditherMw);
            SET_G2D_MZ_INIT(handle->ditherMz);
#else /// defined(G2D_CODEC_STANDALONE)
            SET_G2D_MW_INIT(rand());
            SET_G2D_MZ_INIT(rand());
#endif

        } else {
            SET_G2D_DI_CON(handle->ditherBitR, handle->ditherBitG, handle->ditherBitB, 0x2);
        }

    } else {
        REG_G2D_DI_CON = 0;
    }
}



void g2d_drv_set_burst_type(hal_g2d_handle_t *handle)
{
#if defined(G2D_FPGA)
    REG_G2D_SLOW_DOWN &= ~(G2D_READ_BURST_TYPE_MASK | G2D_WRITE_BURST_TYPE_MASK);
    REG_G2D_SLOW_DOWN |= (handle->writeBurstType) << 20;
    REG_G2D_SLOW_DOWN |= (handle->readBurstType) << 24;
#else
    REG_G2D_SLOW_DOWN = 0x00400000;
#endif
}



void g2d_drv_set_slow_down(hal_g2d_handle_t *handle)
{
#if defined(G2D_FPGA)
    REG_G2D_SLOW_DOWN &= ~G2D_SLOW_DOWN_ENABLE_BIT;

    if (true == (handle->slowDownEnable)) {
        REG_G2D_SLOW_DOWN |= G2D_SLOW_DOWN_ENABLE_BIT;
        REG_G2D_SLOW_DOWN &= ~G2D_SLOW_DOWN_COUNT_MASK;
        REG_G2D_SLOW_DOWN |= (handle->slowDownCount) & G2D_SLOW_DOWN_COUNT_MASK;
    }
#endif
}



void g2d_drv_set_tile(hal_g2d_handle_t *handle)
{
    //Enable Tile
#if defined(G2D_FPGA)
    ///ASSERT(false == (handle->autoTileSizeEnable));
    if (false == (handle->autoTileSizeEnable)) {
        REG_G2D_ROI_CON |= G2D_ROI_CON_ENABLE_FORCE_TS_BIT;
        switch (handle->tileSize) {
            case G2D_TILE_SIZE_TYPE_8x8:
                REG_G2D_ROI_CON |= G2D_ROI_CON_TILE_SIZE_8x8_BIT;
                break;
            case G2D_TILE_SIZE_TYPE_16x8:
                REG_G2D_ROI_CON |= G2D_ROI_CON_TILE_SIZE_16x8_BIT;
                break;
            default:
                ASSERT(0);
        }
    } else {
        REG_G2D_ROI_CON &= ~G2D_ROI_CON_ENABLE_FORCE_TS_BIT;
    }
#else
    REG_G2D_ROI_CON &= ~G2D_ROI_CON_ENABLE_FORCE_TS_BIT;
    ///REG_G2D_ROI_CON |= G2D_ROI_CON_TILE_SIZE_8x8_BIT;
#endif
}



void g2d_drv_set_hw_hisr_callback(hal_g2d_callback_t hisr_callback)
{
    _g2d_hw_hisr_callback = hisr_callback;
}



/// Check function
hal_g2d_status_t g2d_drv_check_dst_canvas_info(uint32_t pitch)
{
    if (pitch > 0x2000) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    } else {
        return HAL_G2D_STATUS_OK;
    }
}



/// Check function
hal_g2d_status_t g2d_drv_check_src_canvas_info(uint32_t pitch)
{
    if (pitch > 0x2000) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    } else {
        return HAL_G2D_STATUS_OK;
    }
}



/// Check function
hal_g2d_status_t g2d_drv_check_src_window(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    /// 0 <= x <= 2047
    if (((unsigned)(x)) > 2047) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }
    if (((unsigned)(y)) > 2047) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    /// 1 <= x <= 2048
    if (((unsigned)(w - 1)) >= 2048) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }
    if (((unsigned)(h - 1)) >= 2048) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2d_drv_check_lt_src_window(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    return g2d_drv_check_src_window(x, y, w, h);
}



hal_g2d_status_t g2d_drv_check_dst_window(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    /// -2048 <= x <= 2047
    if (((unsigned)(x + 2048)) >= 4096) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }
    if (((unsigned)(y + 2048)) >= 4096) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    /// 1 <= x <= 2048
    if (((unsigned)(w - 1)) >= 2048) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }
    if (((unsigned)(h - 1)) >= 2048) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2d_drv_check_lt_dst_window(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    return g2d_drv_check_dst_window(x, y, w, h);
}


int g2d_abs(int input)//program by Dream
{
    if (input < 0) {
        return (0 - input);
    }
    return input;
}

hal_g2d_status_t g2d_drv_check_clip_window(int32_t x, int32_t y, uint32_t x_max_clip, uint32_t y_max_clip)
{
    //G2D_LOGI("X=%d Y=%d XM=%d YM=%d\n", x + 2048, y + 2048, (unsigned)(x_max_clip - 1), (unsigned)(y_max_clip - 1));//Dream: check for range
    /// -2048 <= x <= 2047
    if (((unsigned)(x + 2048)) >= 4096) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }
    if (((unsigned)(y + 2048)) >= 4096) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    /// 1 <= x <= 2048
    if (((unsigned)(x_max_clip - 1)) >= 2048) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }
    if (((unsigned)(y_max_clip - 1)) >= 2048) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2d_drv_query_src_canvas_info(uint32_t *min_width, uint32_t *max_width, uint32_t *min_height, uint32_t *max_height)
{
    *min_width = 1;
    *max_width = 2048;
    *min_height = 1;
    *max_height = 2048;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2d_drv_query_dst_canvas_info(uint32_t *min_width, uint32_t *max_width, uint32_t *min_height, uint32_t *max_height)
{
    *min_width = 1;
    *max_width = 2048;
    *min_height = 1;
    *max_height = 2048;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2d_drv_query_src_window(int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h)
{
    *min_x = 0;
    *max_x = 2047;
    *min_y = 0;
    *max_y = 2047;

    *min_w = 1;
    *max_w = 2048;
    *min_h = 1;
    *max_h = 2048;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2d_drv_query_dst_window(int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h)
{
    *min_x = -2048;
    *max_x = 2047;
    *min_y = -2048;
    *max_y = 2047;

    *min_w = 1;
    *max_w = 2048;
    *min_h = 1;
    *max_h = 2048;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2d_drv_query_lt_src_window(int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h)
{
    return g2d_drv_query_src_window(min_x, max_x, min_y, max_y, min_w, max_w, min_h, max_h);
}

hal_g2d_status_t g2d_drv_query_lt_dst_window(int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h)
{
    return g2d_drv_query_dst_window(min_x, max_x, min_y, max_y, min_w, max_w, min_h, max_h);
}

#endif

