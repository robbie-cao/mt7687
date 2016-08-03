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

/*

	Dream: those function is provide to G2D test framework

*/


#define HAL_G2D_MODULE_ENABLED 1
#include "hal_g2d.h"




#include "hal_g2d_define.h"

#include "string.h"	//memset
#include "FreeRTOS.h"	//RTOS function
#include "task.h"	//task
#include "hal_g2d_drv_internal.h"
#include "hal_clock.h"	//power on

#include "g2d_drv_c_model.h"	//For C model test


#ifdef G2D_SET_BURST
//header file define
hal_g2d_status_t g2dSetTileSize(hal_g2d_handle_t *handle, bool autoTileSizeEnable, G2D_TILE_SIZE_TYPE_ENUM tileSize);
hal_g2d_status_t g2dSetWriteBurstType(hal_g2d_handle_t *handle, G2D_WRITE_BURST_TYPE_ENUM writeBurstType);
hal_g2d_status_t g2dSetReadBurstType(hal_g2d_handle_t *handle, G2D_READ_BURST_TYPE_ENUM readBurstType);
#endif

void g2d_c_model(G2D_REG* reg);

typedef enum {
   G2D_TEST_Fill_Rect = 0,
   G2D_TEST_Font,
   G2D_TEST_Overlay,
   G2D_TEST_Rotation,   

   G2D_TEST_Fill_Rect_MultiThread,
   
   G2D_TEST_Performance,
   G2D_TEST_Callback
} G2D_TEST_CASE_ENUM;


typedef struct _G2D_PAR_ {
	G2D_TEST_CASE_ENUM		test_case;
	hal_g2d_color_format_t	color_format;
	hal_g2d_rotate_angle_t rotate_angle;
}G2D_Parameter;


//Dream: Larkspur===============
#include "syslog.h"
#define G2D_LOGI(fmt,...) LOG_I(common,  (fmt), ##__VA_ARGS__)
//======================================================================================
G2D_REG g2d_sw_register;

unsigned char	*dst_sw_image;
unsigned char	*dst_hw_image;
unsigned char	*layer0_image;
unsigned char	*layer1_image;

const int BufferSize = 120*120*4;


int g2d_compare_sw_hw_results(void)
{
    int i;

    for(i = 0; i < (BufferSize); i++ )
    {
        if(dst_sw_image[i] != dst_hw_image[i])
        {
            G2D_LOGI("Error in index = %d   sw=%x != hw=%x\n\r",i, dst_sw_image[i], dst_hw_image[i]);
            //while(1);
            return 0;
        }
    }
    G2D_LOGI("g2d_compare_sw_hw_results bit perfect\n");//Dream add
    return 1;
}
//======================================================================================
static int g2d_compare_sw_hw_result_100x100(void)
{
    int i;

    for(i = 0; i < (100*100*4); i++ )
    {
        if(dst_sw_image[i] != dst_hw_image[i])
        {
            G2D_LOGI("Error in index = %d, HW: %x, SW: %x\n\r",i, dst_hw_image[i], dst_sw_image[i]);
            return 0;
        }
    }
    G2D_LOGI("g2d_compare_sw_hw_result_100x100 bit perfect\n");//Dream add

    return 1;
}
//======================================================================================

void g2d_set_registers_to_c_models(G2D_REG* reg, unsigned char *dstBuf)
{
    reg->G2D_START = REG_G2D_START;
    reg->G2D_MODE_CON = REG_G2D_MODE_CTRL;
    reg->G2D_IRQ = REG_G2D_IRQ;
    reg->G2D_SLOW_DOWN = REG_G2D_SLOW_DOWN;

    reg->G2D_ROI_CON = REG_G2D_ROI_CON;
    reg->G2D_W2M_ADDR = (uint32_t) dstBuf;;
    reg->G2D_W2M_PITCH = REG_G2D_W2M_PITCH;
    reg->G2D_ROI_OFS = REG_G2D_ROI_OFFSET;
    reg->G2D_ROI_SIZE = REG_G2D_ROI_SIZE;
    reg->G2D_ROI_BGCLR = REG_G2D_ROI_BGCLR;

    reg->G2D_CLP_MIN = REG_G2D_CLP_MIN;
    reg->G2D_CLP_MAX = REG_G2D_CLP_MAX;
    reg->G2D_AVO_CLR = REG_G2D_AVOIDANCE_COLOR;
    reg->G2D_REP_CLR = REG_G2D_REPLACEMENT_COLOR;
    reg->G2D_ROI_MOFS = REG_G2D_W2M_OFFSET;


    reg->G2D_MW_INIT = REG_G2D_MW_INIT;
    reg->G2D_MZ_INIT = REG_G2D_MZ_INIT;
    reg->G2D_DI_CON = REG_G2D_DI_CON;

    reg->G2D_L0_CON = REG_G2D_L0_CON;
    reg->G2D_L0_CLRKEY = REG_G2D_L0_SRC_KEY;
    reg->G2D_L0_ADDR = REG_G2D_L0_ADDR;
    reg->G2D_L0_PITCH = REG_G2D_L0_PITCH;
    reg->G2D_L0_SIZE = REG_G2D_L0_SIZE;
    reg->G2D_L0_OFS = REG_G2D_L0_OFFSET;

    reg->G2D_L1_CON = REG_G2D_L1_CON;
    reg->G2D_L1_CLRKEY = REG_G2D_L1_SRC_KEY;
    reg->G2D_L1_ADDR = REG_G2D_L1_ADDR;
    reg->G2D_L1_PITCH = REG_G2D_L1_PITCH;
    reg->G2D_L1_SIZE = REG_G2D_L1_SIZE;
    reg->G2D_L1_OFS = REG_G2D_L1_OFFSET;

    reg->G2D_L2_CON = REG_G2D_L2_CON;
    reg->G2D_L2_CLRKEY = REG_G2D_L2_SRC_KEY;
    reg->G2D_L2_ADDR = REG_G2D_L2_ADDR;
    reg->G2D_L2_PITCH = REG_G2D_L2_PITCH;
    reg->G2D_L2_SIZE = REG_G2D_L2_SIZE;
    reg->G2D_L2_OFS = REG_G2D_L2_OFFSET;

    reg->G2D_L3_CON = REG_G2D_L3_CON;
    reg->G2D_L3_CLRKEY = REG_G2D_L3_SRC_KEY;
    reg->G2D_L3_ADDR = REG_G2D_L3_ADDR;
    reg->G2D_L3_PITCH = REG_G2D_L3_PITCH;
    reg->G2D_L3_SIZE = REG_G2D_L3_SIZE;
    reg->G2D_L3_OFS = REG_G2D_L3_OFFSET;
}




//======================================================================================
int g2d_rectfill_testframework(hal_g2d_color_format_t	color_format)
{
	/*****************************************
	 * To test the layer rectangle fill.
	 * 1. Layer 0
	 *	  a. Enable alpha blending
	 *	  b. ARGB8888, PARGB8888
	 *****************************************/
	
	hal_g2d_handle_t *g2dHandle;
	
	uint32_t layer0ColorFormat;
	uint32_t layer0Width, layer0Height;
	int32_t layer0RectX, layer0RectY;
	uint32_t layer0RectW, layer0RectH;
	hal_g2d_rotate_angle_t layer0Rotate;
	uint32_t layer0AlphaValue;
	
	uint32_t bgColor;
	int32_t roiRectX, roiRectY;
	uint32_t roiRectW, roiRectH;
	
	uint8_t *dstBuf;
	hal_g2d_color_format_t dstColorFormat;
	int32_t dstRectX, dstRectY;
	uint32_t dstWidth, dstHeight;
	int ret = 0;
	
	{
	   ///layer0Addr = (uint8_t *)&layer0_image[0];
	   //layer0ColorFormat = HAL_G2D_COLOR_FORMAT_PARGB8888;
	   layer0Width = 100;
	   layer0Height = 110;
	   layer0RectX = 0;
	   layer0RectY = 0;
	   layer0RectW = 100;
	   layer0RectH = 110;
	   
	   layer0Rotate = HAL_G2D_ROTATE_ANGLE_0;
	   layer0AlphaValue = 0x56;
	   memset(layer0_image, 0x56, 100*110*4);
	
	   bgColor = 0x46;
	   roiRectX = 10;
	   roiRectY = 10;
	   roiRectW = 110;
	   roiRectH = 110;
	
	   dstBuf = (uint8_t *)&dst_hw_image[0];
	   dstColorFormat = HAL_G2D_COLOR_FORMAT_PARGB8565;
	   dstRectX = 10;
	   dstRectY = 10;
	   dstWidth = 110;
	   dstHeight = 110;
	
		/// HAL_G2D_STATUS_BUSY means someone is using G2D
		if(HAL_G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_BLOCKING))
		{
		   G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
		   return 0;
		}
	
		G2D_LOGI("g2d_overlay_layer_rectangle_fill_test()\n\r");
		{
		  memset(dst_hw_image, 0xCD, 120*120*4);
	
		  hal_g2d_register_callback(g2dHandle, NULL);
		  hal_g2d_overlay_set_window(g2dHandle, dstRectX, dstRectY);
		  hal_set_rgb_buffer(g2dHandle, (uint8_t *)dstBuf, BufferSize, dstWidth, dstHeight, dstColorFormat);
		  
#ifdef G2D_SET_BURST	
		  g2dSetTileSize(g2dHandle, true, G2D_TILE_SIZE_TYPE_8x8);
		  g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
		  g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);
#endif	
		  hal_g2d_overlay_set_background_color(g2dHandle, bgColor);
		  hal_g2d_overlay_set_roi_window(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
	
		  /// Layer 0
		  hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
		  hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_RECTFILL);
		  //hal_g2d_overlay_set_layer_buffer_information(g2dHandle, HAL_G2D_OVERLAY_LAYER0, NULL, layer0Width, layer0Height, layer0ColorFormat);
		  hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
		  hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0Rotate);
		  hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0AlphaValue);
		  hal_g2d_overlay_set_layer_rectfill_color(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0x12345678);

		  for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
		  {
			 if(HAL_G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
				continue;
			 G2D_LOGI("Color Format: %d\n\r", layer0ColorFormat);
	
			 hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, NULL, 0, layer0Width, layer0Height, (hal_g2d_color_format_t)layer0ColorFormat);
	
			 hal_g2d_overlay_start(g2dHandle);
			 while(hal_g2d_get_status(g2dHandle)==HAL_G2D_STATUS_BUSY) {};
			 {
				/// C-model
				memset(dst_sw_image, 0xCD, 120*120*4);
				dstBuf = (uint8_t *)&dst_sw_image[0];
	
				g2d_set_registers_to_c_models(&g2d_sw_register, dstBuf);
	
				g2d_c_model(&g2d_sw_register);
			 }
	
			 {
				ret = g2d_compare_sw_hw_results();
			 }
		  }
	   }
	   hal_g2d_release_handle(g2dHandle);
	}
	return ret ;

}
//======================================================================================
int g2d_overlay_layer_normal_font_testframework(void)
{
    /*****************************************
     * To test the normal font.
     * 1. Layer 0
     *    a. ARGB8888
     *    b. Enable Font
     *    c. Test 1 bit index color
     *****************************************/

    hal_g2d_handle_t *g2dHandle;

    uint8_t *layer0Addr;
    int32_t layer0RectX, layer0RectY;
    uint32_t layer0RectW, layer0RectH;

    hal_g2d_rotate_angle_t layer0Rotate;

    uint32_t bgColor;
    int32_t roiRectX, roiRectY;
    uint32_t roiRectW, roiRectH;

    uint8_t *dstBuf;
    hal_g2d_color_format_t dstColorFormat;
    int32_t dstRectX, dstRectY;
    uint32_t dstWidth, dstHeight;

    int ret = 0	;

    G2D_LOGI("g2d_overlay_normal_font_test()\n\r");

    {
        layer0Addr = (uint8_t *)&layer0_image[0];
        //layer0Width = 100;
        //layer0Height = 110;
        layer0RectX = 0;
        layer0RectY = 0;
        layer0RectW = 100;
        layer0RectH = 110;
        
        layer0Rotate = HAL_G2D_ROTATE_ANGLE_0;
        
        memset(layer0_image, 0xAA, 100*110*4);

        bgColor = 0x46;

        roiRectX = 10;
        roiRectY = 10;
        roiRectW = 110;
        roiRectH = 110;

        dstBuf = (uint8_t *)&dst_hw_image[0];
        dstColorFormat = HAL_G2D_COLOR_FORMAT_PARGB8888;
        dstRectX = 10;
        dstRectY = 10;
        dstWidth = 110;
        dstHeight = 110;

        {
            uint32_t i;
            uint32_t *ptr = (uint32_t *)&layer0_image[0];

            for(i = 0; i < 120 * 120; i++)
            {
                *ptr = (i*123) & 0xFFFFFFFF;	//rand() is replace by (i*123)
                ptr++;
            }
        }

        {
            memset(dst_hw_image, 0xCD, 120*120*4);

            /// HAL_G2D_STATUS_BUSY means someone is using G2D
            if(HAL_G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_BLOCKING))
            {
            	G2D_LOGI("g2d_overlay_layer_normal_font_testframework get handle busy\n");
                return 0;
            }
            hal_g2d_register_callback(g2dHandle, NULL);
            hal_g2d_overlay_set_window(g2dHandle, dstRectX, dstRectY);
            hal_set_rgb_buffer(g2dHandle, (uint8_t *)dstBuf, BufferSize, dstWidth, dstHeight, dstColorFormat);
#ifdef G2D_SET_BURST
            g2dSetTileSize(g2dHandle, true, G2D_TILE_SIZE_TYPE_8x8);
            g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
            g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);
#endif
            hal_g2d_overlay_set_background_color(g2dHandle, bgColor);
            hal_g2d_overlay_set_roi_window(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

            /// Layer 0
            hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
            hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_NORMAL_FONT);
            hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

            hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0Rotate);

            {

                hal_g2d_overlay_set_layer_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0Addr , BufferSize);

                hal_g2d_overlay_start(g2dHandle);
                while(hal_g2d_get_status(g2dHandle)==HAL_G2D_STATUS_BUSY) {};
                {
                    /// C-model
                    memset(dst_sw_image, 0xCD, 120*120*4);
                    dstBuf = (uint8_t *)&dst_sw_image[0];

                    g2d_set_registers_to_c_models(&g2d_sw_register, dstBuf);

                    g2d_c_model(&g2d_sw_register);
                }

                {
                    ret = g2d_compare_sw_hw_results();
                }
            }

            hal_g2d_release_handle(g2dHandle);
        }
    }


    return ret;
}
//======================================================================================

int g2d_overlay_format_test(void)
{
    /*****************************************
     * To test all kind of layer color format
     * 1. Memory output fixes as ARGB8888
     * 2. Overlay layer 0, enable background color
     * 3. Test layer color format
     *    a. RGB565
     *    b. BGR888
     *    c. RGB888
     *    d. YUYV422
     *    e. ARGB8888
     *    f. PARGB8888
     *****************************************/

    hal_g2d_handle_t *g2dHandle;

    uint8_t *layer0Addr;
    uint32_t layer0ColorFormat;
    uint32_t layer0Width, layer0Height;
    int32_t layer0RectX, layer0RectY;
    uint32_t layer0RectW, layer0RectH;
    hal_g2d_rotate_angle_t layer0Rotate;
    uint32_t layer0AlphaValue;

    uint32_t bgColor;

    int32_t roiRectX, roiRectY;
    uint32_t roiRectW, roiRectH;

    uint8_t *dstBuf;
    hal_g2d_color_format_t dstColorFormat;
    int32_t dstRectX, dstRectY;
    uint32_t dstWidth, dstHeight;
    int ret = 0	;
    {
        layer0Addr = (uint8_t *)&layer0_image[0];
        //layer0ColorFormat = HAL_G2D_COLOR_FORMAT_RGB565;
        layer0Width = 111;
        layer0Height = 112;
        layer0RectX = 0;
        layer0RectY = 0;
        layer0RectW = 100;
        layer0RectH = 99;
        
        layer0Rotate = HAL_G2D_ROTATE_ANGLE_0;
        
        layer0AlphaValue = 0x56;
        memset(layer0_image, 0x56, BufferSize);

        bgColor = 0x46;

        roiRectX = 0;
        roiRectY = 0;
        roiRectW = 66;
        roiRectH = 77;

        dstBuf = (uint8_t *)&dst_hw_image[0];
        dstColorFormat = HAL_G2D_COLOR_FORMAT_ARGB8888;
        dstRectX = 0;
        dstRectY = 0;
        dstWidth = 66;
        dstHeight = 77;

        G2D_LOGI("g2d_overlay_layer_color_format_test()\n\r");
        {
            dstBuf = (uint8_t *)&dst_hw_image[0];
            memset(dst_hw_image, 0xCD, 120*120*4);

            /// HAL_G2D_STATUS_BUSY means someone is using G2D
            if(HAL_G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_BLOCKING))
            {
                G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
                return 0;
            }

            hal_g2d_register_callback(g2dHandle, NULL);
            hal_g2d_overlay_set_window(g2dHandle, dstRectX, dstRectY);
            hal_set_rgb_buffer(g2dHandle, (uint8_t *)dstBuf, BufferSize, dstWidth, dstHeight, dstColorFormat);

			//you still can test if u need color replace
			//hal_g2d_replace_color(g2dHandle, 0, 255, 0, 0, 0, 0, 0, 255);
#ifdef G2D_SET_BURST
            g2dSetTileSize(g2dHandle, true, G2D_TILE_SIZE_TYPE_8x8);
            g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
            g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);
#endif
            hal_g2d_overlay_set_background_color(g2dHandle, bgColor);

            hal_g2d_overlay_set_roi_window(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

            /// Layer 0
            hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
            hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
            //hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0Addr, layer0Width, layer0Height, layer0ColorFormat);
            hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

            hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0Rotate);
            hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0AlphaValue);


            for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
            {
                G2D_LOGI("\tColor Format: %d\n\r", layer0ColorFormat);
                if(HAL_G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
                {
                    uint8_t *bufferAddr[3];
                    uint32_t bufferSize[3];

                    bufferAddr[0] = layer0Addr;
                    bufferAddr[1] = NULL;
                    bufferAddr[2] = NULL;
                    bufferSize[0] = BufferSize;
                    bufferSize[1] = 0;
                    bufferSize[2] = 0;

                    hal_g2d_overlay_set_layer_yuv_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, bufferAddr, bufferSize, layer0Width, layer0Height, (hal_g2d_color_format_t)layer0ColorFormat);
                }
                else
                    hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0Addr, BufferSize, layer0Width, layer0Height, (hal_g2d_color_format_t)layer0ColorFormat);

                hal_g2d_overlay_start(g2dHandle);
                while(hal_g2d_get_status(g2dHandle)==HAL_G2D_STATUS_BUSY) {};
                {
                    /// C-model
                    memset(dst_sw_image, 0xCD, 120*120*4);
                    dstBuf = (uint8_t *)&dst_sw_image[0];

                    g2d_set_registers_to_c_models(&g2d_sw_register, dstBuf);

                    g2d_c_model(&g2d_sw_register);
                }

                {
                    ret = g2d_compare_sw_hw_results();
                }
            }

            hal_g2d_release_handle(g2dHandle);
        }
    }
	return ret;
}

//======================================================================================
int g2d_overlay_layer_rotation_test(hal_g2d_rotate_angle_t rotate_angle)
{
    /*****************************************
     * To test layer rotation in all directions
     * 1. Layer color format is RGB565
     * 2. Test layer rotation
     *    a. 0, 90, 180, 270
     *    b. rotation with mirror
     * 3. Layer combinations
     *    a. layer 0
     *    b. layer 0 + 1
     *    c. layer 0 + 1 + 2
     *    d. layer 0 + 1 + 2 + 3
     *****************************************/

    hal_g2d_handle_t *g2dHandle;

    uint8_t *layerAddr = (uint8_t *)&layer0_image[0];
    hal_g2d_color_format_t layerColorFormat = HAL_G2D_COLOR_FORMAT_RGB565;
    uint32_t layerWidth = 80;
    uint32_t layerHeight = 90;
    uint32_t layerRectW = 51;
    uint32_t layerRectH = 61;

    uint32_t layerSrcKeyValue = 0xFFFFFFFF;
    uint32_t layerAlphaValue = 0x33;

    uint32_t bgColor = 0x0;
    int32_t roiRectX = 0;
    int32_t roiRectY = 0;
    uint32_t roiRectW = 80;
    uint32_t roiRectH = 90;

    hal_g2d_color_format_t dstColorFormat = HAL_G2D_COLOR_FORMAT_RGB888;
    int32_t dstRectX = 0;
    int32_t dstRectY = 0;
    uint32_t dstWidth = 80;
    uint32_t dstHeight = 90;

    int32_t i1=0, i2=0, i3=0;
    bool i1On = false, i2On = false, i3On = false;

    int ret = 0;
    {
        /// HAL_G2D_STATUS_BUSY means someone is using G2D
        if(HAL_G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_BLOCKING))
        {
            G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
            return 0;
        }

        hal_g2d_register_callback(g2dHandle, NULL);
        hal_g2d_overlay_set_window(g2dHandle, dstRectX, dstRectY);
        hal_set_rgb_buffer(g2dHandle, (uint8_t *)&dst_hw_image[0], BufferSize, dstWidth, dstHeight, dstColorFormat);
#ifdef G2D_SET_BURST
        g2dSetTileSize(g2dHandle, true, G2D_TILE_SIZE_TYPE_8x8);
        g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
        g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);
#endif
        hal_g2d_overlay_set_background_color(g2dHandle, bgColor);
        hal_g2d_overlay_set_roi_window(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

        // layer 0
        hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
        hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layerAddr, 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
        hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 23, 0, layerRectW, layerRectH);
        hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layerSrcKeyValue);
        hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layerAlphaValue);

        // layer 1
        hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER1, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
        hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, layerAddr, 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
        hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER1, 0, 21, layerRectW, layerRectH);
        hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER1, layerSrcKeyValue);
        hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER1, layerAlphaValue);

        // layer 2
        hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER2, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
        hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER2, layerAddr, 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
        hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER2, 11, 15, layerRectW, layerRectH);
        hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER2, layerSrcKeyValue);
        hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER2, layerAlphaValue);

        // layer 3
        hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER3, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
        hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER3, layerAddr, 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
        hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER3, -3, -2, layerRectW, layerRectH);
        hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER3, layerSrcKeyValue);
        hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER3, layerAlphaValue);

        G2D_LOGI("g2d_overlay_layer_rotation_test()\n\r");
        {
            //i3On = true;
            //for(i3 = 0; i3< 8; i3++) easy
            {
                if(i3On)
                {
                    hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER3, true);
                    hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER3, (hal_g2d_rotate_angle_t)i3);
                }
                else
                {
                    i3On = true;
                    i3 = -1;
                }

                //i2On = true;
                //for(i2 = 0; i2< 8; i2++) easy
                {
                    if(i2On)
                    {
                        hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER2, true);
                        hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER2, (hal_g2d_rotate_angle_t)i2);
                    }
                    else
                    {
                        i2On = true;
                        i2 = -1;
                    }

                    //i1On = true;
                    //for(i1 = 0; i1 < 8; i1++) easy
                    {
                        if(i1On)
                        {
                            hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, true);
                            hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER1, (hal_g2d_rotate_angle_t)i1);
                        }
                        else
                        {
                            i1On = true;
                            i1 = -1;
                        }

                        //for(i0 = 0; i0 < 8; i0++)
                        {
                            hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
                            hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, rotate_angle);

                            G2D_LOGI("Layer 0: %d, Layer 1: %d, Layer 2: %d, Layer 3: %d \n\r", rotate_angle, i1On?i1:9, i2On?i2:9, i3On?i3:9);

                            memset(dst_hw_image, 0xCD, 120*120*4);

                            hal_g2d_overlay_start(g2dHandle);
                            while(hal_g2d_get_status(g2dHandle)==HAL_G2D_STATUS_BUSY) {};
                            {
                                /// C-model
                                memset(dst_sw_image, 0xCD, BufferSize);

                                g2d_set_registers_to_c_models(&g2d_sw_register, (uint8_t *)&dst_sw_image[0]);
                                g2d_c_model(&g2d_sw_register);
                            }
                            {
                                ret  = g2d_compare_sw_hw_result_100x100();
                            }
                        }
                    }
                }
            }
        }

        hal_g2d_release_handle(g2dHandle);
    }

	return ret;
}
//====================================================
int g2d_overlay_performance_testframework(void)
{
//EdwardYC is fuck need this test before Chinese new year !(just kidding XD)    
//Dream: 2016.02.05
   
   hal_g2d_handle_t *g2dHandle;
   const int TestWidth = 120;	
   
   uint8_t *layerAddr = (uint8_t *)&layer0_image[0];
   hal_g2d_color_format_t layerColorFormat = HAL_G2D_COLOR_FORMAT_RGB565;
   uint32_t layerWidth = TestWidth;
   uint32_t layerHeight = TestWidth;
   uint32_t layerRectW = TestWidth;
   uint32_t layerRectH = TestWidth;

   uint32_t layerSrcKeyValue = 0xFFFFFFFF;
   uint32_t layerAlphaValue = 0x33;
   
   uint32_t bgColor = 0x0;
   int32_t roiRectX = 0;
   int32_t roiRectY = 0;
   uint32_t roiRectW = TestWidth;
   uint32_t roiRectH = TestWidth;
   
   hal_g2d_color_format_t dstColorFormat = HAL_G2D_COLOR_FORMAT_RGB888;
   int32_t dstRectX = 0;
   int32_t dstRectY = 0;
   uint32_t dstWidth = TestWidth;
   uint32_t dstHeight = TestWidth;
   
   int32_t i0=0;
   uint32_t violet_time1, violet_time2, violet_time_total = 0;
   G2D_LOGI("g2d_overlay_performance_testframework start time=%d\n\r", xTaskGetTickCount());
   int ret = 0;
   for(i0=0 ; i0 < 5; i0++)
   {
	   /// HAL_G2D_STATUS_BUSY means someone is using G2D
	   if(HAL_G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_BLOCKING))
	   {
		   G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
		   return 0;
	   }
   
	   hal_g2d_register_callback(g2dHandle, NULL);
	   hal_g2d_overlay_set_window(g2dHandle, dstRectX, dstRectY);
	   hal_set_rgb_buffer(g2dHandle, (uint8_t *)&dst_hw_image[0], BufferSize, dstWidth, dstHeight, dstColorFormat);
#ifdef G2D_SET_BURST
	   g2dSetTileSize(g2dHandle, true, G2D_TILE_SIZE_TYPE_8x8);
	   g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
	   g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);
#endif   
	   hal_g2d_overlay_set_background_color(g2dHandle, bgColor);
	   hal_g2d_overlay_set_roi_window(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
   
	   // layer 0
	   hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
	   hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layerAddr, TestWidth * TestWidth * 2, layerWidth, layerHeight, layerColorFormat);
	   hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 23, 0, layerRectW, layerRectH);
	   hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layerSrcKeyValue);
	   hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layerAlphaValue);
   
	   // layer 1
	   hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER1, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
	   hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, layerAddr, TestWidth * TestWidth * 2, layerWidth, layerHeight, layerColorFormat);
	   hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER1, 0, 21, layerRectW, layerRectH);
	   hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER1, layerSrcKeyValue);
	   hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER1, layerAlphaValue);
   
	   // layer 2
	   hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER2, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
	   hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER2, layerAddr, TestWidth * TestWidth * 2, layerWidth, layerHeight, layerColorFormat);
	   hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER2, 11, 15, layerRectW, layerRectH);
	   hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER2, layerSrcKeyValue);
	   hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER2, layerAlphaValue);
   
	   // layer 3
	   hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER3, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
	   hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER3, layerAddr, TestWidth * TestWidth * 2, layerWidth, layerHeight, layerColorFormat);
	   hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER3, -3, -2, layerRectW, layerRectH);
	   hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER3, layerSrcKeyValue);
	   hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER3, layerAlphaValue);

	   hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER3, true);
	   hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER3, (hal_g2d_rotate_angle_t)i0);

   	   hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER2, true);
	   hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER2, (hal_g2d_rotate_angle_t)i0);

	   hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, true);
	   hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER1, (hal_g2d_rotate_angle_t)i0);

	   hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
       hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, (hal_g2d_rotate_angle_t)i0);


	   violet_time1 = xTaskGetTickCount();
	   hal_g2d_overlay_start(g2dHandle);
	   while(hal_g2d_get_status(g2dHandle)==HAL_G2D_STATUS_BUSY) {};
	   violet_time2 = xTaskGetTickCount();
	   violet_time_total += (violet_time2 - violet_time1);
	   hal_g2d_release_handle(g2dHandle);
	}   
   ret = violet_time_total;
   
   G2D_LOGI("g2d_overlay_performance_testframework result=%d ms(*5)\n\r", ret);
   return ret;

}

//======================================================================================
/*

void g2d_callback_math()//this function may execute so many time
{
    int i, j, n=1000 ;
    for(i=1; i<n; i++)
    {
        for(j=2;j<i;j++)
        {
            if(i%j==0)break;
        }
        if(i==j)printf("%d\n",j);
    }
    REG_G2D_IRQ &= ~G2D_IRQ_ENABLE_BIT;
    return ;	
}


int g2d_callback_testframework(void)
{
	hal_g2d_handle_t *g2dHandle;
	
	uint32_t layer0ColorFormat;
	uint32_t layer0Width, layer0Height;
	int32_t layer0RectX, layer0RectY;
	uint32_t layer0RectW, layer0RectH;

	uint32_t layer0SrcKeyValue;
	hal_g2d_rotate_angle_t layer0Rotate;

	uint32_t layer0AlphaValue;
	
	uint32_t bgColor;
	uint32_t outputAlphaValue;
	int32_t roiRectX, roiRectY;
	uint32_t roiRectW, roiRectH;
	
	uint8_t *dstBuf;
	hal_g2d_color_format_t dstColorFormat;
	int32_t dstRectX, dstRectY;
	uint32_t dstWidth, dstHeight;
	int ret ;
	
	{
	   ///layer0Addr = (uint8_t *)&layer0_image[0];
	   //layer0ColorFormat = HAL_G2D_COLOR_FORMAT_PARGB8888;
	   layer0Width = 100;
	   layer0Height = 110;
	   layer0RectX = 0;
	   layer0RectY = 0;
	   layer0RectW = 100;
	   layer0RectH = 110;
	   
	   layer0SrcKeyValue = 0xFFFFFFFF;
	   layer0Rotate = HAL_G2D_CW_ROTATE_ANGLE_000;
	   
	   layer0AlphaValue = 0x56;
	   memset(layer0_image, 0x56, 100*110*4);
	
	   bgColor = 0x46;
	   outputAlphaValue = 0xFF;
	   roiRectX = 10;
	   roiRectY = 10;
	   roiRectW = 110;
	   roiRectH = 110;
	
	   dstBuf = (uint8_t *)&dst_hw_image[0];
	   dstColorFormat = HAL_G2D_COLOR_FORMAT_PARGB8565;
	   dstRectX = 10;
	   dstRectY = 10;
	   dstWidth = 110;
	   dstHeight = 110;
	
		/// HAL_G2D_STATUS_BUSY means someone is using G2D
		if(HAL_G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
		{
		   G2D_LOGI("ERROR on callback\n\r");
		   return 0;
		}
	
		G2D_LOGI("g2d_callback_testframework()\n\r");
		{
		  memset(dst_hw_image, 0xCD, 120*120*4);
	
		  hal_g2d_register_callback(g2dHandle, g2d_callback_math);
		  hal_g2d_overlay_set_window(g2dHandle, dstRectX, dstRectY);
		  hal_set_rgb_buffer(g2dHandle, (uint8_t *)dstBuf, BufferSize, dstWidth, dstHeight, dstColorFormat);
		  hal_g2d_set_color_replacement(g2dHandle, false, 0, 255, 0, 0, 0, 0, 0, 255);
		  hal_g2d_set_destination_clip_window(g2dHandle, false, 0, 0, dstWidth, dstHeight);
#ifdef G2D_SET_BURST
		  g2dSetTileSize(g2dHandle, true, G2D_TILE_SIZE_TYPE_8x8);
		  g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
		  g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);
#endif	
		  hal_g2d_overlay_set_background_color(g2dHandle, bgColor);
		  hal_g2d_overlay_set_roi_window(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
	
		  /// Layer 0
		  hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
		  hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_RECT);
		  //hal_g2d_overlay_set_layer_buffer_information(g2dHandle, HAL_G2D_OVERLAY_LAYER0, NULL, layer0Width, layer0Height, layer0ColorFormat);
		  hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

		  hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0Rotate);
		  hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0AlphaValue);
		  hal_g2d_overlay_set_layer_rectfill_color(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0x12345678);
	
		  //for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
		  {
			layer0ColorFormat = HAL_G2D_COLOR_FORMAT_RGB888;
	
			 hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, NULL, 0, layer0Width, layer0Height, (hal_g2d_color_format_t)layer0ColorFormat);
	
			 hal_g2d_overlay_start(g2dHandle);
			 while(hal_g2d_get_status(g2dHandle)==G2D_STATUS_BUSY) {};
			 {
				/// C-model
				memset(dst_sw_image, 0xCD, 120*120*4);
				dstBuf = (uint8_t *)&dst_sw_image[0];
	
				g2d_set_registers_to_c_models(&g2d_sw_register, dstBuf);
	
				g2d_c_model(&g2d_sw_register);
			 }
	
			 {
				ret = g2d_compare_sw_hw_results();
			 }
		  }
	   }
	   hal_g2d_release_handle(g2dHandle);
	}
	return ret ;

}
*/
//======================================================================================
#ifdef G2D_USE_Task    

int g_taskcount = 0;
void test_font_task(void)
{
	g2d_overlay_layer_normal_font_testframework();
	g_taskcount++;	
	vTaskDelete(NULL);
}

//==============

void test_task1(void)
{

	int times_perfert1 = 0;
	G2D_LOGI("task1 start\n\r");

	{
		hal_g2d_handle_t *g2dHandle;		
		uint32_t layer0ColorFormat;
		uint32_t layer0Width, layer0Height;
		int32_t layer0RectX, layer0RectY;
		uint32_t layer0RectW, layer0RectH;

		uint32_t layer0SrcKeyValue;
		hal_g2d_rotate_angle_t layer0Rotate;
		uint32_t layer0AlphaValue;
		
		uint32_t bgColor;
		uint32_t outputAlphaValue;
		int32_t roiRectX, roiRectY;
		uint32_t roiRectW, roiRectH;
		
		uint8_t *dstBuf;
		hal_g2d_color_format_t dstColorFormat;
		int32_t dstRectX, dstRectY;
		uint32_t dstWidth, dstHeight;
		int i, flag ;
		for(layer0ColorFormat = 1; layer0ColorFormat < 10; layer0ColorFormat++)
		{
		   ///layer0Addr = (uint8_t *)&layer0_image[0];
		   //layer0ColorFormat = HAL_G2D_COLOR_FORMAT_PARGB8888;
		   layer0Width = 100;
		   layer0Height = 110;
		   layer0RectX = 0;
		   layer0RectY = 0;
		   layer0RectW = 100;
		   layer0RectH = 110;
		   
		   layer0SrcKeyValue = 0xFFFFFFFF;
		   layer0Rotate = HAL_G2D_CW_ROTATE_ANGLE_000;
		   
		   layer0AlphaValue = 0x56;
		   memset(layer0_image, 0x56, 100*110*4);
		
		   bgColor = 0x46;
		   
		   outputAlphaValue = 0xFF;
		   roiRectX = 10;
		   roiRectY = 10;
		   roiRectW = 110;
		   roiRectH = 110;
		
		   dstBuf = (uint8_t *)&dst_hw_image[0];
		   dstColorFormat = HAL_G2D_COLOR_FORMAT_PARGB8565;
		   dstRectX = 10;
		   dstRectY = 10;
		   dstWidth = 110;
		   dstHeight = 110;
		
			/// HAL_G2D_STATUS_BUSY means someone is using G2D
			if(HAL_G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_BLOCKING))
			{
			   G2D_LOGI("=====impossible case on multi task====");
			   return ;
			}
		
			G2D_LOGI("g2d_overlay_layer_rectangle_fill_test()  task1\n\r");
			{
			  memset(dst_hw_image, 0xCD, 120*120*4);
		
			  hal_g2d_register_callback(g2dHandle, NULL);
			  hal_g2d_overlay_set_window(g2dHandle, dstRectX, dstRectY);
			  hal_set_rgb_buffer(g2dHandle, (uint8_t *)dstBuf, BufferSize, dstWidth, dstHeight, dstColorFormat);
#ifdef G2D_SET_BURST		
			  g2dSetTileSize(g2dHandle, true, G2D_TILE_SIZE_TYPE_8x8);
			  g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
			  g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);
#endif		
			  hal_g2d_overlay_set_background_color(g2dHandle, bgColor);

			  hal_g2d_overlay_set_roi_window(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
		
			  /// Layer 0
			  hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
			  hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_RECT);
			  //hal_g2d_overlay_set_layer_buffer_information(g2dHandle, HAL_G2D_OVERLAY_LAYER0, NULL, layer0Width, layer0Height, layer0ColorFormat);
			  hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

			  hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0Rotate);
			  hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0AlphaValue);
			  hal_g2d_overlay_set_layer_rectfill_color(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0x12345678);
		
			  
			  {
				 if(HAL_G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
					continue;
				 G2D_LOGI("Color Format: %d task1\n\r", layer0ColorFormat);
		
				 hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, NULL, 0, layer0Width, layer0Height, (hal_g2d_color_format_t)layer0ColorFormat);
		
				 hal_g2d_overlay_start(g2dHandle);
				 while(hal_g2d_get_status(g2dHandle)==HAL_G2D_STATUS_BUSY) {};
				 {
					/// C-model
					memset(dst_sw_image, 0xCD, 120*120*4);
					dstBuf = (uint8_t *)&dst_sw_image[0];
		
					g2d_set_registers_to_c_models(&g2d_sw_register, dstBuf);
		
					g2d_c_model(&g2d_sw_register);
				 }
				 flag = 1;
				 for(i = 0; i < (100*100*4); i++ )
				 {
			        if(dst_sw_image[i] != dst_hw_image[i])
        			{
        				flag = 0;
			            G2D_LOGI("Error in index = %d, HW: %x, SW: %x task1\n\r",i, dst_hw_image[i], dst_sw_image[i]);
            			break;
			        }
			     }
				 if(flag)
				 {
					times_perfert1++;
			     	G2D_LOGI("g2d_compare_sw_hw_result_100x100 bit perfect task1\n");//Dream add
				 }
			  }
		   }
		   hal_g2d_release_handle(g2dHandle);
		}
	}	
	G2D_LOGI("Task1 is sucess %d times\n", times_perfert1);//Dream add
	g_taskcount++;	
	vTaskDelete(NULL);
}
void test_task2(void)
{
	const int Size_Image2 = 4096;
	char * dst_hw_image2 = pvPortMalloc(Size_Image2);
	char * dst_sw_image2 = pvPortMalloc(Size_Image2);
    int times_perfert2 = 0;

	G2D_LOGI("task2 start\n\r");
	{
		hal_g2d_handle_t *g2dHandle;		
		uint32_t layer0ColorFormat;
		uint32_t layer0Width, layer0Height;
		int32_t layer0RectX, layer0RectY;
		uint32_t layer0RectW, layer0RectH;

		uint32_t layer0SrcKeyValue;
		hal_g2d_rotate_angle_t layer0Rotate;

		uint32_t layer0AlphaValue;
		
		uint32_t bgColor;
		bool outputAlphaEnable;
		uint32_t outputAlphaValue;
		int32_t roiRectX, roiRectY;
		uint32_t roiRectW, roiRectH;
		
		uint8_t *dstBuf;
		hal_g2d_color_format_t dstColorFormat;
		int32_t dstRectX, dstRectY;
		uint32_t dstWidth, dstHeight;
		int i, flag=0 ;
		for(layer0ColorFormat = 1; layer0ColorFormat < 10; layer0ColorFormat++)
		{
		   ///layer0Addr = (uint8_t *)&layer0_image[0];
		   //layer0ColorFormat = HAL_G2D_COLOR_FORMAT_PARGB8888;
		   layer0Width = 100;
		   layer0Height = 110;
		   layer0RectX = 0;
		   layer0RectY = 0;
		   layer0RectW = 100;
		   layer0RectH = 110;
		   
		   layer0SrcKeyValue = 0xFFFFFFFF;
		   layer0Rotate = HAL_G2D_CW_ROTATE_ANGLE_000;
		   
		   layer0AlphaValue = 0x56;
		   memset(layer0_image, 0x56, 100*110*4);
		
		   bgColor = 0x46;
		   outputAlphaEnable = false;
		   outputAlphaValue = 0xFF;
		   roiRectX = 10;
		   roiRectY = 10;
		   roiRectW = 20;
		   roiRectH = 20;
		
		   dstBuf = (uint8_t *)&dst_hw_image2[0];
		   dstColorFormat = HAL_G2D_COLOR_FORMAT_PARGB8565;
		   dstRectX = 10;
		   dstRectY = 10;
		   dstWidth = 32;
		   dstHeight = 32;
		
			/// HAL_G2D_STATUS_BUSY means someone is using G2D
			if(HAL_G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_BLOCKING))
			{
			   G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
			   return ;
			}
		
			G2D_LOGI("g2d_overlay_layer_rectangle_fill_test() task2\n\r");
			{
			  memset(dst_hw_image2, 0xCD, Size_Image2);
		
			  hal_g2d_register_callback(g2dHandle, NULL);
			  hal_g2d_overlay_set_window(g2dHandle, dstRectX, dstRectY);
			  hal_set_rgb_buffer(g2dHandle, (uint8_t *)dstBuf, Size_Image2, dstWidth, dstHeight, dstColorFormat);
#ifdef G2D_SET_BURST		
			  g2dSetTileSize(g2dHandle, true, G2D_TILE_SIZE_TYPE_8x8);
			  g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
			  g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);
#endif		
			  hal_g2d_overlay_set_background_color(g2dHandle, bgColor);
			  hal_g2d_overlay_set_destination_alpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
			  hal_g2d_overlay_set_roi_window(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
		
			  /// Layer 0
			  hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
			  hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_RECT);
			  //hal_g2d_overlay_set_layer_buffer_information(g2dHandle, HAL_G2D_OVERLAY_LAYER0, NULL, layer0Width, layer0Height, layer0ColorFormat);
			  hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

			  hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0Rotate);
			  hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER0, layer0AlphaValue);
			  hal_g2d_overlay_set_layer_rectfill_color(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0x12345678);
		
			  
			  {
				 if(HAL_G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
					continue;
				 G2D_LOGI("Color Format: %d task2\n\r", layer0ColorFormat);
		
				 hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, NULL, 0, layer0Width, layer0Height, (hal_g2d_color_format_t)layer0ColorFormat);
		
				 hal_g2d_overlay_start(g2dHandle);
				 while(hal_g2d_get_status(g2dHandle)==HAL_G2D_STATUS_BUSY) {};
				 {
					/// C-model
					memset(dst_sw_image2, 0xCD, Size_Image2);
					dstBuf = (uint8_t *)&dst_sw_image2[0];
		
					g2d_set_registers_to_c_models(&g2d_sw_register, dstBuf);
		
					g2d_c_model(&g2d_sw_register);
				 }
				 flag = 1;
				 for(i = 0; i < Size_Image2; i++ )
				 {
			        if(dst_sw_image2[i] != dst_hw_image2[i])
        			{
			            G2D_LOGI("Error in index = %d, HW: %x, SW: %x task2\n\r",i, dst_hw_image[i], dst_sw_image[i]);
						flag = 0;
            			break;
			        }
			     }
				 if(flag)
				 {
			     	G2D_LOGI("g2d_compare_sw_hw_result_100x100 bit perfect task2\n");//Dream add
			     	times_perfert2++;
				 }
			  }
		   }
		   hal_g2d_release_handle(g2dHandle);
		}
	}	

	G2D_LOGI("Task2 is sucess %d times\n", times_perfert2);//Dream add
	g_taskcount++;	

	vPortFree(dst_hw_image2);
	vPortFree(dst_sw_image2);
	vTaskDelete(NULL);

}
#endif
//======================================================================================

int g2d_test_framework(int testcase, void* parameter)
{
    int ret = 0;
    G2D_Parameter *g2d_par = parameter;

	//cm4_topsm_lock_MD2G_MTCMOS();
	//cm4_topsm_lock_MM_MTCMOS();
	//cm4_topsm_lock_BT_MTCMOS();

    G2D_LOGI("======= Dream: SQC Test Start!! ========\n\r");
    hal_clock_enable(HAL_CLOCK_CG_G2D) ;//power on Larkspur
    hal_g2d_init();

	dst_hw_image = pvPortMalloc(BufferSize);
	dst_sw_image = pvPortMalloc(BufferSize);
	layer0_image = pvPortMalloc(BufferSize);
	//layer1_image = pvPortMalloc(BufferSize);
    if(testcase==0)
    {
		ret = g2d_rectfill_testframework(g2d_par->color_format);
    }
    else if(testcase==1)
    {
        ret = g2d_overlay_layer_normal_font_testframework();
    }
    else if(testcase==2)
    {
        ret = g2d_overlay_format_test();
    }
	else if(testcase==3)
	{
		ret = g2d_overlay_layer_rotation_test(g2d_par->rotate_angle);
	}
	else if(testcase==4)
	{
#ifdef G2D_USE_Task   	
		g_taskcount = 0; 		
		xTaskCreate(test_task1, "g2d rectfill task1", 1024, NULL ,1, NULL);		
		xTaskCreate(test_task2, "g2d rectfill task2", 1024, NULL ,1, NULL);		
		while(g_taskcount<2) vTaskDelay(1000);
#endif		
	}
	else if(testcase==G2D_TEST_Performance)
	{
		ret = g2d_overlay_performance_testframework();
	}
	else if(testcase==G2D_TEST_Callback)
	{
		//ret = g2d_callback_testframework();
	}

	
    vPortFree(dst_hw_image);
	vPortFree(dst_sw_image);
    vPortFree(layer0_image);
	//vPortFree(layer1_image);
    G2D_LOGI("======= Dream: SQC Test End %d!!! ========\n\r", ret);
    return ret;
}



