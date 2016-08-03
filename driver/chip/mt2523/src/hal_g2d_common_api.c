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
#include "string.h"

#ifdef G2D_FreeRTOS
#include "FreeRTOS.h"	//RTOS function
#include "task.h"	//task
#include "queue.h"	//Queue
#endif

#include "hal_nvic_internal.h"	//Dream: restore_interrupt_mask and PRIORITY
#include "hal_cache_internal.h" //Dream: cache_is_buffer_cacheable
#include "hal_cm4_topsm.h"		//Dream: cm4_topsm_lock_MM_MTCMOS

//Dream: 2523 assert
#include "assert.h"
#define ASSERT assert


#define G2D_HW_SUPPORT
#define G2D_HW_FONT_SUPPORT
#define G2D_HW_AA_FONT_SUPPORT
#define G2D_HW_BITBLT_SUPPORT
#define G2D_HW_RECT_FILL_SUPPORT
#define G2D_HW_OVERLAY_SUPPORT
#define G2D_HW_OVERLAY_TOTAL_LAYER	 4
#define G2D_HW_DITHERING_SUPPORT

//========== Dream Program for freeRTOS and IAR  2016.06.20 ==========
#if G2D_FreeRTOS
#include "syslog.h"
#define G2D_LOGI(fmt,...) LOG_I(common,  (fmt), ##__VA_ARGS__)
#else
#include <stdio.h>
#define G2D_LOGI printf
#endif
//===========================================================
#if defined(G2D_SUPPORT_MUTEX)
#include "semphr.h"

xSemaphoreHandle g2d_state_mutex = NULL;//Dream: Larkspur's mutex
static TickType_t g_g2d_timeout_tick = 0;

#endif
//===========================================================


#include "hal_g2d_define.h"

#include "hal_g2d_drv_api.h"





volatile int  is_g2d_busy = 0;	//Dream add for larkspur 2016.01.14


static hal_g2d_handle_t _g2d_hw_handle;

bool g2d_irq_handler(void *g2d_status)
{
    if (_g2d_hw_handle.g2dCallback) {
        _g2d_hw_handle.g2dCallback(NULL);
    }
    is_g2d_busy = 0;//Dream add 2016.01.14
    return true;
}



hal_g2d_status_t hal_g2d_init(void)
{
#if defined(G2D_HW_SUPPORT)
#if defined(G2D_SUPPORT_MUTEX)
    if (NULL == g2d_state_mutex) {
        g2d_state_mutex = xSemaphoreCreateBinary();//Dream: Larkspur create mutex
        g_g2d_timeout_tick = (TickType_t)(500 / portTICK_PERIOD_MS);
    }

    G2D_LOGI("Dream: g2d_state_mutex=%d\r\n", g2d_state_mutex);
#endif

    g2d_drv_power_on();
    g2d_drv_init();
    g2d_drv_power_off();

    G2D_LOGI("Dream: g2d init sucess!\r\n");

#endif

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_get_status(hal_g2d_handle_t *handle)//Dream: important modify!! 2016.06.24
{
    switch (handle->type) {
        case HAL_G2D_CODEC_TYPE_HW: {
			if(g2d_drv_get_hw_status() == 1)	//BUSY
				return HAL_G2D_STATUS_BUSY;
			else
				return HAL_G2D_STATUS_OK;
        }
        case HAL_G2D_CODEC_TYPE_SW:
            return HAL_G2D_STATUS_OK;
        default:
            ASSERT(0);
    }

    return HAL_G2D_STATUS_OK;
}

//Dream: mutex function ======================================================
bool INT_QueryExceptionStatus(void)
{
    return false;	//Dream: Michael say that it's only reply true on boot
}

#if defined(G2D_SUPPORT_MUTEX)
void kal_take_mutex(xSemaphoreHandle mutex_ptr)
{
    xSemaphoreTake(mutex_ptr, g_g2d_timeout_tick);
}
void kal_give_mutex(xSemaphoreHandle mutex_ptr)
{
    BaseType_t xHigherPriorityTaskWoken;
    xSemaphoreGiveFromISR(mutex_ptr, &xHigherPriorityTaskWoken);
}
#endif

int INT_QueryIsNonCachedRAM(uint32_t addr, int size)
{
    if (cache_is_buffer_cacheable(addr, size)) {
        G2D_LOGI("====================WARING!!bufer is cacheable!!=============================\n");
        return 0;
    } else {
        return 1;//Dream: for system stable
    }
}
//======================================================

hal_g2d_status_t hal_g2d_get_handle(hal_g2d_handle_t **handlePtr, hal_g2d_codec_type_t codecType, hal_g2d_get_handle_mode_t handleMode)
{
    hal_g2d_handle_t *handle = NULL;

    switch (codecType) {
        case HAL_G2D_CODEC_TYPE_HW: {
#if !defined(G2D_HW_SUPPORT)	//Dream : let return if not support HW
            return HAL_G2D_STATUS_NOT_SUPPORT;
#endif /// G2D_HW_SUPPORT

            *handlePtr = &_g2d_hw_handle;
            handle = *handlePtr;
            if (HAL_G2D_GET_HANDLE_MODE_BLOCKING == handleMode) {
                while (1) {
#if defined(G2D_SUPPORT_MUTEX)
                    kal_take_mutex(g2d_state_mutex);
#endif
                    if (G2D_STATE_IDLE != (handle->g2dState)) {
#if defined(G2D_SUPPORT_MUTEX)                      
                        kal_give_mutex(g2d_state_mutex);
#endif                        
                        continue;
                    } else {
                        memset(handle, 0, sizeof(hal_g2d_handle_t));
                        handle->g2dState = G2D_STATE_SETTING_PARAMETER;
#if defined(G2D_SUPPORT_MUTEX)
                        kal_give_mutex(g2d_state_mutex);
#endif
                        break;
                    }
                }
            } else {
#if defined(G2D_SUPPORT_MUTEX)
                kal_take_mutex(g2d_state_mutex);
#endif
                if (G2D_STATE_IDLE != (handle->g2dState)) {
#if defined(G2D_SUPPORT_MUTEX)
                    kal_give_mutex(g2d_state_mutex);
#endif
                    return HAL_G2D_STATUS_BUSY;
                }
                memset(handle, 0, sizeof(hal_g2d_handle_t));
                handle->g2dState = G2D_STATE_SETTING_PARAMETER;
#if defined(G2D_SUPPORT_MUTEX)
                kal_give_mutex(g2d_state_mutex);
#endif
            }
            g2d_drv_power_on();
            break;
        }
#if 0//!defined(G2D_FPGA) //for Larkspur
        case HAL_G2D_CODEC_TYPE_SW:
            handle = (hal_g2d_handle_t *)malloc(sizeof(hal_g2d_handle_t));
            /// Can not memset to hw handle to 0
            memset(handle, 0, sizeof(hal_g2d_handle_t));
            handle->g2dState = G2D_STATE_SETTING_PARAMETER;
            *handlePtr = handle;
            break;
#endif
        default:
            ASSERT(0);
            *handlePtr = NULL;
            break;
    }
    handle->type = codecType;
#ifdef G2D_FreeRTOS
    cm4_topsm_lock_MM_MTCMOS();	//2016.02.23 for CM4
#endif
    return HAL_G2D_STATUS_OK;
}





hal_g2d_status_t hal_g2d_query_rgb_buffer_range(hal_g2d_handle_t *handle, uint32_t *min_width, uint32_t *max_width, uint32_t *min_height, uint32_t *max_height)
{
    switch (handle->type) {
#if defined(G2D_HW_SUPPORT)
        case HAL_G2D_CODEC_TYPE_HW:
            g2d_drv_query_dst_canvas_info(min_width, max_width, min_height, max_height);
            break;
#endif /// G2D_HW_SUPPORT
        case HAL_G2D_CODEC_TYPE_SW:
            ASSERT(0);
            break;
        default:
            ASSERT(0);
            break;
    }
    return HAL_G2D_STATUS_OK;
}

//Dream add it for flatten 2016.04.01
hal_g2d_status_t g2dSetOwner(hal_g2d_handle_t *handle, G2D_CODEC_OWNER_ENUM owner)
{
#if 1//defined(G2D_HW_SHARE_WITH_GOVL_SUPPORT)
    handle->owner = owner;

    return HAL_G2D_STATUS_OK;
#else
    return HAL_G2D_STATUS_NOT_SUPPORT;
#endif
}



hal_g2d_status_t hal_set_rgb_buffer(hal_g2d_handle_t *handle,
        uint8_t *bufferAddr, uint32_t bufferSize,
        uint32_t width, uint32_t height, hal_g2d_color_format_t colorFormat)
{
    G2D_CANVAS_INFO_STRUCT *dstCanvas = &(handle->dstCanvas);
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    ASSERT(bufferAddr);

    dstCanvas->RGBBufferAddr = bufferAddr;
    dstCanvas->RGBBufferSize = bufferSize;
    dstCanvas->width = width;
    dstCanvas->height = height;
    dstCanvas->colorFormat = colorFormat;

    switch (colorFormat) {
        case HAL_G2D_COLOR_FORMAT_GRAY :
            dstCanvas->bytesPerPixel = 1;
            break;
        case HAL_G2D_COLOR_FORMAT_RGB565 :
            dstCanvas->bytesPerPixel = 2;
            break;
        case HAL_G2D_COLOR_FORMAT_RGB888:
        case HAL_G2D_COLOR_FORMAT_BGR888:
        case HAL_G2D_COLOR_FORMAT_ARGB8565:
        case HAL_G2D_COLOR_FORMAT_ARGB6666:
        case HAL_G2D_COLOR_FORMAT_PARGB8565:
        case HAL_G2D_COLOR_FORMAT_PARGB6666:
            dstCanvas->bytesPerPixel = 3;
            break;
        case HAL_G2D_COLOR_FORMAT_ARGB8888:
        case HAL_G2D_COLOR_FORMAT_PARGB8888:
            dstCanvas->bytesPerPixel = 4;
            break;
        default:
            ASSERT(0);
            break;
    }
#if defined(G2D_HW_SUPPORT)
    if (HAL_G2D_CODEC_TYPE_HW == (handle->type)) {
        status = g2d_drv_check_dst_canvas_info(width);
    }
#endif

    return status;
}



/*L
hal_g2d_status_t g2dSetDstBufferSwitchBehavior(hal_g2d_handle_t *handle, GFX_CACHE_SWITCH_BEHAVIOR_ENUM behavior)
{
   #if defined(__DYNAMIC_SWITCH_CACHEABILITY__) && defined(__MTK_TARGET__)
      handle->dstCanvasCacheSwitchBehavior = behavior;
   #endif

   return HAL_G2D_STATUS_OK;
}
*/

//======================only for freeRTOS======================
#ifdef G2D_USE_Task
typedef struct {
    int msg_id;
} G2D_MSG;

QueueHandle_t G2D_Queue;

void v_callback_Task(void *pvParameters)
{
    hal_g2d_callback_t _g2d_hw_hisr_callback = pvParameters;

    G2D_MSG msgs;
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
    while (1) {
        if (xQueueReceive(G2D_Queue, &(msgs), xTicksToWait)) {
            if (msgs.msg_id == 200) {
                G2D_LOGI("v_callback_Task break!\r\n");
                vTaskDelay(200);	//Dream: Safe
                (*_g2d_hw_hisr_callback)(NULL);
                break;
            }
        }

    }

    vTaskDelete(NULL);
}
#endif
//=============================================================

hal_g2d_status_t hal_g2d_register_callback(hal_g2d_handle_t *handle, hal_g2d_callback_t g2d_callback)
{
    if (!g2d_callback) {
        handle->g2dCallback = NULL;
        handle->callbackEnable = false;
    } else {
#ifdef G2D_USE_Task
        G2D_Queue = xQueueCreate(10, sizeof(G2D_MSG));
        xTaskCreate(v_callback_Task, "g2d callback task", 512, (void *) g2d_callback , 1, NULL);
#endif
        handle->g2dCallback = g2d_callback;
        handle->callbackEnable = true;
    }

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_set_clipping_window(hal_g2d_handle_t *handle, int32_t x, int32_t y, int32_t w, int32_t h)
{
    G2D_COORDINATE_STRUCT *topLeft     = &(handle->clipWindow.topLeft);
    G2D_COORDINATE_STRUCT *buttomRight = &(handle->clipWindow.buttomRight);
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    handle->clipEnable = true;

    if (1) {
        uint32_t x_min_clip, y_min_clip;
        uint32_t x_max_clip, y_max_clip;

        x_min_clip = x;
        y_min_clip = y;
        x_max_clip = x + w - 1;
        y_max_clip = y + h - 1;

#if 1//defined(G2D_HW_SUPPORT)
        if (HAL_G2D_CODEC_TYPE_HW == (handle->type)) {
            status = g2d_drv_check_clip_window(x, y, x_max_clip, y_max_clip);
            //Dream add for check
            if (status == HAL_G2D_STATUS_INVALID_PARAMETER)	{
                G2D_LOGI("g2d_drv_check_clip_window over range!!\n");
            }
        }
#endif
        topLeft->x = x_min_clip;
        topLeft->y = y_min_clip;
        buttomRight->x = x_max_clip;
        buttomRight->y = y_max_clip;
    }

    return status;
}



hal_g2d_status_t hal_g2d_replace_color(hal_g2d_handle_t *handle,
        uint8_t a0, uint8_t r0, uint8_t g0, uint8_t b0,
        uint8_t a1, uint8_t r1, uint8_t g1, uint8_t b1)
{
    handle->colorReplaceEnable = true;

    if (1) {//TC/TW
        handle->colorToAvoid.alpha = a0;
        handle->colorToAvoid.red   = r0;
        handle->colorToAvoid.green = g0;
        handle->colorToAvoid.blue  = b0;

        handle->colorToReplace.alpha = a1;
        handle->colorToReplace.red   = r1;
        handle->colorToReplace.green = g1;
        handle->colorToReplace.blue  = b1;
    }

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_set_source_key(hal_g2d_handle_t *handle, uint8_t a0, uint8_t r0, uint8_t g0, uint8_t b0)
{
    //if (true == srcKeyEnable) { TC/TW
        handle->srcKeyEnable = true;
        handle->srcKeyValue.alpha = a0;
        handle->srcKeyValue.red   = r0;
        handle->srcKeyValue.green = g0;
        handle->srcKeyValue.blue  = b0;
    //} else {
    //    handle->srcKeyEnable = false;
    //}

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t hal_g2d_query_support(hal_g2d_codec_type_t codecType, hal_g2d_supported_function_t function)
{
    switch (codecType) {
        case HAL_G2D_CODEC_TYPE_HW: {
#if defined(G2D_HW_SUPPORT)
            switch (function) {
                case HAL_G2D_SUPPORTED_FUNCTION_BITBLT:
#if defined(G2D_HW_BITBLT_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif
                ///break;
                case HAL_G2D_SUPPORTED_FUNCTION_RECTFILL:
#if defined(G2D_HW_RECT_FILL_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif
                ///break;
                case HAL_G2D_SUPPORTED_FUNCTION_NORMAL_FONT:
#if defined(G2D_HW_FONT_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif
                ///break;
                case HAL_G2D_SUPPORTED_FUNCTION_AA_FONT:
#if defined(G2D_HW_AA_FONT_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif
                ///break;
                case HAL_G2D_SUPPORTED_FUNCTION_TILT_FONT:
#if defined(G2D_HW_TILT_FONT_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif
                ///break;
                case HAL_G2D_SUPPORTED_FUNCTION_LT:
#if defined(G2D_HW_LT_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif
                ///break;
/*                case HAL_G2D_SUPPORTED_FUNCTION_LT_WITH_ALPHA_BLENDING: TC/TW
#if defined(G2D_HW_LT_WITH_ALPHA_BLENDING_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORT;
#endif
                ///break;
*/                
                case HAL_G2D_SUPPORTED_FUNCTION_SAD:
#if defined(G2D_HW_SAD_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif
                ///break;
                case HAL_G2D_SUPPORTED_FUNCTION_OVERLAY:
#if 1//defined(G2D_HW_OVERLAY_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORT;
#endif
                ///break;
                case HAL_G2D_SUPPORTED_FUNCTION_DITHERING:
#if defined(G2D_HW_DITHERING_SUPPORT)
                    return HAL_G2D_STATUS_OK;
#else
                    return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif
                ///break;
                default:
                    ASSERT(0);
            }
#else
            return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif /// G2D_HW_SUPPORT
        }
        break;
        case HAL_G2D_CODEC_TYPE_SW: {
            switch (function) {
                case HAL_G2D_SUPPORTED_FUNCTION_LT:
                //case HAL_G2D_SUPPORT_FUNCTION_LT_WITH_ALPHA_BLENDING:
                    return HAL_G2D_STATUS_OK;
                default:
                    return HAL_G2D_STATUS_NOT_SUPPORTED;
            }
        }
        ///break;
        default:
            ASSERT(0);
    }
    return HAL_G2D_STATUS_OK;
}


/*  Dream: not to release on Larkspur */
hal_g2d_status_t hal_g2d_set_dithering(hal_g2d_handle_t *handle, bool ditherEnable, g2d_dithering_mode_t ditheringMode)
{
#if defined(G2D_HW_DITHERING_SUPPORT)
    handle->ditheringEnable = ditherEnable;
    handle->ditheringMode = ditheringMode;

    return HAL_G2D_STATUS_OK;
#else
    return HAL_G2D_STATUS_NOT_SUPPORTED;
#endif
}

hal_g2d_status_t g2dSetReadBurstType(hal_g2d_handle_t *handle, G2D_READ_BURST_TYPE_ENUM readBurstType)
{
    handle->readBurstType = readBurstType;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2dSetWriteBurstType(hal_g2d_handle_t *handle, G2D_WRITE_BURST_TYPE_ENUM writeBurstType)
{
    handle->writeBurstType = writeBurstType;

    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2dSetSlowDownCount(hal_g2d_handle_t *handle, bool slowDownEnable, uint32_t slowDownCount)
{
    handle->slowDownEnable = slowDownEnable;

    if (true == slowDownEnable) {
        handle->slowDownCount = slowDownCount;
    }
    return HAL_G2D_STATUS_OK;
}



hal_g2d_status_t g2dSetTileSize(hal_g2d_handle_t *handle, bool autoTileSizeEnable, G2D_TILE_SIZE_TYPE_ENUM tileSize)
{
    handle->autoTileSizeEnable = autoTileSizeEnable;

    if (false == autoTileSizeEnable) {
        handle->tileSize = tileSize;
    }
    return HAL_G2D_STATUS_OK;
}




hal_g2d_status_t hal_g2d_set_ditheringBit(hal_g2d_handle_t *handle, G2D_DITHERING_BIT_ENUM r, G2D_DITHERING_BIT_ENUM g, G2D_DITHERING_BIT_ENUM b)
{
#if defined(G2D_HW_DITHERING_SUPPORT)
    handle->ditherBitR = r;
    handle->ditherBitG = g;
    handle->ditherBitB = b;

    return HAL_G2D_STATUS_OK;
#else
    return HAL_G2D_STATUS_NOT_SUPPORT;
#endif
}



hal_g2d_status_t hal_g2d_set_ditheringInitialSeed(hal_g2d_handle_t *handle, uint32_t mw, uint32_t mz)
{
#if defined(G2D_HW_DITHERING_SUPPORT)
    handle->ditherMw = mw;
    handle->ditherMz = mz;

    return HAL_G2D_STATUS_OK;
#else
    return HAL_G2D_STATUS_NOT_SUPPORT;
#endif
}



//====================== program for TC/TW review 2016.06.24 ======================
void hal_g2d_overlay_dream_init(hal_g2d_handle_t *handle)
{
	HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo0 = &(handle->overlayFunc.layerInfo[0]);
	HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo1 = &(handle->overlayFunc.layerInfo[1]);
	HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo2 = &(handle->overlayFunc.layerInfo[2]);
	HAL_G2D_OVERLAY_LAYER_STRUCT *layerInfo3 = &(handle->overlayFunc.layerInfo[3]);
	G2D_OVERLAY_STRUCT *overlayFunc = &(handle->overlayFunc);

	handle->srcKeyEnable = false;
	handle->clipEnable = false;
	handle->colorReplaceEnable = false;
	
	layerInfo0->srcKeyEnable = false;
	layerInfo1->srcKeyEnable = false;	
	layerInfo2->srcKeyEnable = false;
	layerInfo3->srcKeyEnable = false;

	layerInfo0->layerAlphaEnable = false;
	layerInfo1->layerAlphaEnable = false;
	layerInfo2->layerAlphaEnable = false;
	layerInfo3->layerAlphaEnable = false;

	overlayFunc->dstAlphaEnable = false;

}


//=========================release============================
#define MAX_TIMES_WAIT_G2D 20
hal_g2d_status_t hal_g2d_release_handle(hal_g2d_handle_t *handle)
{
    switch (handle->type) {
        case HAL_G2D_CODEC_TYPE_HW:
#if defined(G2D_HW_SUPPORT)
        {
#if 1//defined(__MTK_TARGE__) L 2016.06.24
        {
					#ifdef G2D_FreeRTOS
            int index;
            //====Dream: let wait for G2D status sometime for Larkspur ======
            for (index = 0; index <= MAX_TIMES_WAIT_G2D; index++) {
                if (hal_g2d_get_status(handle) == HAL_G2D_STATUS_OK) {
                    break;
                }
                vTaskDelay(2);
                if (index == MAX_TIMES_WAIT_G2D) {
                    G2D_LOGI("hal_g2d_release_handle fail! it's always busy!!\r\n") ;
                    return HAL_G2D_STATUS_BUSY;
                }
            }
					#endif	
        }
            //==============================================================
            if (hal_g2d_get_status(handle)==HAL_G2D_STATUS_BUSY) {
                return HAL_G2D_STATUS_BUSY;
            }
            g2d_drv_power_off();



            if (true != INT_QueryExceptionStatus()) {
#if defined(G2D_SUPPORT_MUTEX)
                kal_take_mutex(g2d_state_mutex);
#endif
                handle->g2dState = G2D_STATE_IDLE;
#if defined(G2D_SUPPORT_MUTEX)
                kal_give_mutex(g2d_state_mutex);
#endif
            } else {
                handle->g2dState = G2D_STATE_IDLE;
            }
#else
            handle->g2dState = G2D_STATE_IDLE;
#endif
        }
#else
        return HAL_G2D_STATUS_NOT_SUPPORT;
#endif /// G2D_HW_SUPPORT
        break;
#if 0//!defined(G2D_FPGA) //Larkspur
        case HAL_G2D_CODEC_TYPE_SW:
            free(handle);
            break;
#endif
        default:
            ASSERT(0);
            break;
    }

    is_g2d_busy = 0;//Dream add 2016.01.14

	hal_g2d_overlay_dream_init(handle);

    handle->g2dState = G2D_STATE_IDLE;//Dream add 2015.12.29
    G2D_LOGI("hal_g2d_release_handle OK!%d \r\n", is_g2d_busy) ;
#ifdef G2D_FreeRTOS    
    cm4_topsm_unlock_MM_MTCMOS() ;	//2016.02.23 for CM4
#endif
    return HAL_G2D_STATUS_OK;
}

#endif


