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

#ifndef __G2D_STRUCTURE_H__
#define __G2D_STRUCTURE_H__

//#include "g2d_enum.h"  




typedef struct G2D_COORDINATE_T
{
   int32_t x;
   int32_t y;

} G2D_COORDINATE_STRUCT;



typedef struct G2D_WINDOW_T
{
   G2D_COORDINATE_STRUCT topLeft;
   G2D_COORDINATE_STRUCT buttomRight;

} G2D_WINDOW_STRUCT;



typedef struct G2D_COLOR_T
{
   uint8_t alpha;
   uint8_t red;
   uint8_t green;
   uint8_t blue;

} G2D_COLOR_STRUCT;



typedef struct G2D_CANVAS_INFO_STRUCT_T
{
   uint8_t *RGBBufferAddr;                  /// For the use of encode from RGB buffer
   uint8_t *YUVBufferAddr[3];
   uint32_t RGBBufferSize;
   uint32_t YUVBufferSize[3];
   uint32_t width;                         /// width in pixel
   uint32_t height;                        /// height in pixel
   g2d_color_format_t colorFormat;        /// color format
   uint32_t bytesPerPixel;

} G2D_CANVAS_INFO_STRUCT;



typedef struct G2D_RECTANGLE_FILL_T
{
   G2D_WINDOW_STRUCT dstWindow;          /// define destination buffer info.
   uint32_t rectFillColor;

}G2D_RECTANGLE_FILL_STRUCT;



typedef struct G2D_FONT_CACHING_T
{
   G2D_CANVAS_INFO_STRUCT srcCanvas;

   G2D_WINDOW_STRUCT srcWindow;          /// define source rectangle buffer to be bitblt
   G2D_WINDOW_STRUCT dstWindow;          /// define destination buffer info.

   G2D_COLOR_STRUCT foregroundColor;

   bool aaFontEnable;

   G2D_FONT_BIT_ENUM fontBit;

   #if defined(G2D_HW_TILT_FONT_SUPPORT)
      bool tiltModeEnable;                ///italic mode
      uint32_t tiltValued[32];
      int32_t tiltTableSize;
   #endif
} G2D_FONT_STRUCT;



typedef struct G2D_BITBLT__T
{
   /// src config
   G2D_CANVAS_INFO_STRUCT srcCanvas;

   G2D_WINDOW_STRUCT srcWindow;          /// define source rectangle buffer to be bitblt
   G2D_WINDOW_STRUCT dstWindow;          /// define destination buffer info.


   bool srcAlphaEnable;
   uint32_t srcAlphaValue;

   bool dstAlphaEnable;
   uint32_t dstAlphaValue;

   G2D_CW_ROTATE_ANGLE_ENUM rotation;

   uint32_t* indexColorTableAddr;
   G2D_INDEX_COLOR_TABLE_SIZE_ENUM  indexColorTableSize;

} G2D_BITBLT_STRUCT;



typedef struct G2D_SAD_IMG_STRUCT_T
{
   uint8_t *bufferAddr;       /// the first byte of the first row
   uint32_t bufferSize;
   uint32_t width;               /// width in pixel
   uint32_t height;              /// height in pixel
   G2D_WINDOW_STRUCT window;

} G2D_SAD_IMG_STRUCT;



typedef struct G2D_SAD_T
{
   G2D_SAD_IMG_STRUCT imgInfo[2];        
   G2D_WINDOW_STRUCT dstWindow;
   
} G2D_SAD_STRUCT;



typedef union {
   uint32_t fixedMatrix[9];
   float floatMatrix[9];

}G2D_LT_MATRIX_UNION;



typedef struct G2D_LT_STRUCT_T
{
   G2D_CANVAS_INFO_STRUCT srcCanvas;
   G2D_WINDOW_STRUCT srcWindow;
   G2D_WINDOW_STRUCT dstWindow;

   bool srcAlphaEnable;
   uint32_t srcAlphaValue;

   bool dstAlphaEnable;
   uint32_t dstAlphaValue;

   G2D_LT_SAMPLE_MODE_ENUM samplingMode;

   G2D_LT_SRC_KEY_EDGE_FILTER_ENUM srcKeyFilterMode;
   uint32_t userSpecifiedColor;

   G2D_LT_MATRIX_UNION inverseMatrix;
   G2D_LT_MATRIX_UNION matrix;

} G2D_LT_STRUCT;

#if 1// (defined(G2D_HW_OVERLAY_SUPPORT))   

typedef struct G2D_OVERLAY_LAYER_STRUCT_T
{
   bool layerEnable;

   G2D_CANVAS_INFO_STRUCT layerCanvas;
   G2D_WINDOW_STRUCT layerWindow;

   bool layerAlphaEnable;
   uint32_t layerAlphaValue;

   bool srcKeyEnable;
   uint32_t srcKeyValue;

   G2D_OVERLAY_LAYER_FUNCTION_ENUM function;

   G2D_FONT_BIT_ENUM fontBit;
   uint32_t foregroundColor;

   uint32_t rectFillColor;
   G2D_CW_ROTATE_ANGLE_ENUM rotation;

} G2D_OVERLAY_LAYER_STRUCT;



typedef struct G2D_OVERLAY_STRUCT_T
{
   G2D_OVERLAY_LAYER_STRUCT layerInfo[G2D_HW_OVERLAY_TOTAL_LAYER];

   uint32_t backgroundColor;
   
   #if 1//L defined(G2D_FPGA)
      bool backgroundDisable;
   #endif
   
   bool dstAlphaEnable;
   uint32_t dstAlphaValue;

   G2D_COORDINATE_STRUCT dstCoordiante;
   G2D_WINDOW_STRUCT roiWindow;
} G2D_OVERLAY_STRUCT;

#endif

typedef bool (*G2D_CALLBACK_FUNC_PTR)(void *);

typedef struct G2D_HANDLE_STRUCT_T
{
   g2d_state_t g2dState;
   g2d_codec_type_t type;

   G2D_CODEC_OWNER_ENUM owner;
   
   /// coordinate system
   G2D_CANVAS_INFO_STRUCT dstCanvas;

   bool clipEnable;
   G2D_WINDOW_STRUCT clipWindow;

   /// The inter-section between clip_window and dst_window
   G2D_WINDOW_STRUCT targetClipWindow;

   /// for source key
   /// For Bitblt, Rect, LT
   bool srcKeyEnable;
   G2D_COLOR_STRUCT srcKeyValue;

   /// for color replacement
   bool colorReplaceEnable;
   G2D_COLOR_STRUCT colorToAvoid;           /// color to avoid (to be replaced)
   G2D_COLOR_STRUCT colorToReplace;         /// color to replace colorToAvoid

   void* intMemAddr;
   uint32_t intMemSize;

   #if defined(G2D_HW_DITHERING_SUPPORT)
      bool ditheringEnable;
      g2d_dithering_mode_t ditheringMode;
   #endif

   #if defined(G2D_HW_SUPPORT)
      G2D_RECTANGLE_FILL_STRUCT rectFillFunc;
      G2D_FONT_STRUCT fontFunc;
      G2D_BITBLT_STRUCT bitbltFunc;

      #if defined(G2D_HW_SAD_SUPPORT)
         G2D_SAD_STRUCT sadFunc;
      #endif

      #if defined(G2D_HW_OVERLAY_SUPPORT)
         G2D_OVERLAY_STRUCT overlayFunc;
      #endif
   #endif

   G2D_LT_STRUCT ltFunc;

   #if 1//defined(G2D_FPGA) Larkspur compile
      G2D_READ_BURST_TYPE_ENUM readBurstType;
      G2D_WRITE_BURST_TYPE_ENUM writeBurstType;
      bool slowDownEnable;
      uint32_t slowDownCount;
   
      bool autoTileSizeEnable;
      G2D_TILE_SIZE_TYPE_ENUM tileSize;
      G2D_LT_CACHE_SIZE_ENUM cacheSize;
      //G2D_MAX_OUTSTANDING_NUM_ENUM maxOutstanding;
      
      G2D_DITHERING_BIT_ENUM ditherBitR;
      G2D_DITHERING_BIT_ENUM ditherBitG;
      G2D_DITHERING_BIT_ENUM ditherBitB;
      uint32_t ditherMw;
      uint32_t ditherMz;     
      
   #endif

   bool callbackEnable;
   G2D_CALLBACK_FUNC_PTR g2dCallback;

} g2d_handle_t;


#endif  /*g2d_structure.h*/
