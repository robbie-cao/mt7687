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
	This fils = G2D enum + struct
*/

#ifndef __G2D_ENUM_H__
#define __G2D_ENUM_H__

#include "stdbool.h"
#include "stdint.h"

#ifndef __G2D_CODEC_DEFINE__

typedef enum {
    G2D_DITHERING_MODE_RANDOM_ALGORITHM = 0,
    G2D_DITHERING_MODE_FIXED_PATTERN
} g2d_dithering_mode_t;

#endif

typedef enum {
    G2D_STATE_IDLE        = 0x0,
    G2D_STATE_SETTING_PARAMETER,
    G2D_STATE_READY,
    G2D_STATE_BUSY,
    G2D_STATE_COMPLETE,
    G2D_STATE_ABORTING
} g2d_state_t;


typedef enum {
    G2D_CCW_ROTATE_ANGLE_000 = 0,
    G2D_CCW_ROTATE_ANGLE_090,
    G2D_CCW_ROTATE_ANGLE_180,
    G2D_CCW_ROTATE_ANGLE_270,
    G2D_CCW_ROTATE_ANGLE_MIRROR_000,
    G2D_CCW_ROTATE_ANGLE_MIRROR_090,
    G2D_CCW_ROTATE_ANGLE_MIRROR_180,
    G2D_CCW_ROTATE_ANGLE_MIRROR_270

} G2D_CCW_ROTATE_ANGLE_ENUM;



typedef enum {
    G2D_INDEX_COLOR_TABLE_SIZE_2 = 0,
    G2D_INDEX_COLOR_TABLE_SIZE_4,
    G2D_INDEX_COLOR_TABLE_SIZE_16,
    G2D_INDEX_COLOR_TABLE_SIZE_256

} G2D_INDEX_COLOR_TABLE_SIZE_ENUM;



typedef enum {
    G2D_FONT_BIT_1_BIT = 0,
    G2D_FONT_BIT_2_BIT,
    G2D_FONT_BIT_4_BIT,
    G2D_FONT_BIT_8_BIT

} g2d_font_bit_t;



typedef enum {
    G2D_LT_SAMPLE_MODE_TRUNCATED = 0,
    G2D_LT_SAMPLE_MODE_NEAREST = 1,
    G2D_LT_SAMPLE_MODE_BILINEAR_WITHOUT_EDGE_FILTER = 2,
    G2D_LT_SAMPLE_MODE_BILINEAR_WITH_EDGE_FILTER_DULPLICATE = 6,
    G2D_LT_SAMPLE_MODE_BILINEAR_WITH_EDGE_FILTER_DST_COLOR = 10,
    G2D_LT_SAMPLE_MODE_BILINEAR_WITH_EDGE_FILTER_USER_SPEC_COLOR = 14

} G2D_LT_SAMPLE_MODE_ENUM;



typedef enum {
    G2D_MEMORY_TYPE_NO_SWITCH  = 0,
    G2D_MEMORY_TYPE_SWITCH_TO_CACHEABLE,
    G2D_MEMORY_TYPE_SWITCH_TO_NONCACHEABLE

} G2D_MEMORY_TYPE_ENUM;



typedef enum {
    G2D_LT_SRC_KEY_EDGE_FILTER_USER_SPEC_COLOR = 0,
    G2D_LT_SRC_KEY_EDGE_FILTER_DULPLICATE

} G2D_LT_SRC_KEY_EDGE_FILTER_ENUM;


typedef enum {
    G2D_DITHERING_BIT_0_BIT = 0,
    G2D_DITHERING_BIT_1_BIT,
    G2D_DITHERING_BIT_2_BIT,
    G2D_DITHERING_BIT_3_BIT
} G2D_DITHERING_BIT_ENUM;




typedef enum {
    G2D_READ_BURST_TYPE_8 = 0,
    G2D_READ_BURST_TYPE_4 = 1,
    G2D_READ_BURST_TYPE_1 = 3
} G2D_READ_BURST_TYPE_ENUM;

typedef enum {
    G2D_WRITE_BURST_TYPE_1 = 0,
    G2D_WRITE_BURST_TYPE_4 = 2,
    G2D_WRITE_BURST_TYPE_8 = 3,
    G2D_WRITE_BURST_TYPE_16 = 4

} G2D_WRITE_BURST_TYPE_ENUM;





typedef enum {
    G2D_TILE_SIZE_TYPE_8x8 = 0,
    G2D_TILE_SIZE_TYPE_16x8
} G2D_TILE_SIZE_TYPE_ENUM;


typedef enum {
    G2D_CODEC_OWNER_G2D = 0,
    G2D_CODEC_OWNER_GOVL
} G2D_CODEC_OWNER_ENUM;


//==============================================================================================================
#ifndef MIN
#define MIN(x, y) ((((x)) <= ((y)))? x: y)
#endif // MIN

#ifndef MAX
#define MAX(x, y) ((((x)) <= ((y)))? y: x)
#endif // MAX


/// color format conversion

/// RGB888 to RGB565
#define  G2D_RGB888_TO_RGB565(R, G, B)          ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | ((B & 0xF8)>> 3)
/// packed RGB888 to 32-bit
#define  G2D_RGB888_PACKED(R, G, B)             (((R << 16)| (G << 8) | B) & 0x00FFFFFF )
/// ARGB888 to ARGB4444
#define  G2D_ARGB8888_TO_ARGB4444(A, R, G, B)   ((((A >> 4) << 12) | ((R >> 4) << 8) | ((G >> 4) << 4) | B ) & 0x0000FFFF)
/// packed ARGB8888 to 32-bit
#define  G2D_ARGB8888_PACKED(A, R, G, B)        ((A << 24) | (R << 16) | (G << 8) | B)
/// ARGB8888 to ARGB8565
#define  G2D_ARGB8888_TO_ARGB8565(A, R, G, B)   (((A << 16) | (G2D_RGB888_TO_RGB565(R, G, B))) & 0x00FFFFFF)
/// ARGB8888 to ARGB6666
#define  G2D_ARGB8888_TO_ARGB6666(A, R, G, B)   ((((A >> 2) << 18) | ((R >> 2) << 12) | ((G >> 2) << 6) | (B >> 2) ) & 0x00FFFFFF)




#endif

//==============================================================================================================
#ifndef __G2D_STRUCTURE_H__
#define __G2D_STRUCTURE_H__

//#include "g2d_enum.h"

typedef struct G2D_COORDINATE_T {
    int32_t x;
    int32_t y;

} G2D_COORDINATE_STRUCT;



typedef struct G2D_WINDOW_T {
    G2D_COORDINATE_STRUCT topLeft;
    G2D_COORDINATE_STRUCT buttomRight;

} G2D_WINDOW_STRUCT;



typedef struct G2D_COLOR_T {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

} G2D_COLOR_STRUCT;



typedef struct G2D_CANVAS_INFO_STRUCT_T {
    uint8_t *RGBBufferAddr;                  /// For the use of encode from RGB buffer
    uint8_t *YUVBufferAddr[3];
    uint32_t RGBBufferSize;
    uint32_t YUVBufferSize[3];
    uint32_t width;                         /// width in pixel
    uint32_t height;                        /// height in pixel
    hal_g2d_color_format_t colorFormat;        /// color format
    uint32_t bytesPerPixel;

} G2D_CANVAS_INFO_STRUCT;



typedef struct G2D_RECTANGLE_FILL_T {
    G2D_WINDOW_STRUCT dstWindow;          /// define destination buffer info.
    uint32_t rectFillColor;

} G2D_RECTANGLE_FILL_STRUCT;



typedef struct G2D_FONT_CACHING_T {
    G2D_CANVAS_INFO_STRUCT srcCanvas;

    G2D_WINDOW_STRUCT srcWindow;          /// define source rectangle buffer to be bitblt
    G2D_WINDOW_STRUCT dstWindow;          /// define destination buffer info.

    G2D_COLOR_STRUCT foregroundColor;

    bool aaFontEnable;

    g2d_font_bit_t fontBit;

#if defined(G2D_HW_TILT_FONT_SUPPORT)
    bool tiltModeEnable;                ///italic mode
    uint32_t tiltValued[32];
    int32_t tiltTableSize;
#endif
} G2D_FONT_STRUCT;



typedef struct G2D_BITBLT__T {
    /// src config
    G2D_CANVAS_INFO_STRUCT srcCanvas;

    G2D_WINDOW_STRUCT srcWindow;          /// define source rectangle buffer to be bitblt
    G2D_WINDOW_STRUCT dstWindow;          /// define destination buffer info.


    bool srcAlphaEnable;
    uint32_t srcAlphaValue;

    bool dstAlphaEnable;
    uint32_t dstAlphaValue;

    hal_g2d_rotate_angle_t rotation;

    uint32_t *indexColorTableAddr;
    G2D_INDEX_COLOR_TABLE_SIZE_ENUM  indexColorTableSize;

} G2D_BITBLT_STRUCT;



typedef struct G2D_SAD_IMG_STRUCT_T {
    uint8_t *bufferAddr;       /// the first byte of the first row
    uint32_t bufferSize;
    uint32_t width;               /// width in pixel
    uint32_t height;              /// height in pixel
    G2D_WINDOW_STRUCT window;

} G2D_SAD_IMG_STRUCT;



typedef struct G2D_SAD_T {
    G2D_SAD_IMG_STRUCT imgInfo[2];
    G2D_WINDOW_STRUCT dstWindow;

} G2D_SAD_STRUCT;



typedef union {
    uint32_t fixedMatrix[9];
    float floatMatrix[9];

} G2D_LT_MATRIX_UNION;



typedef struct G2D_LT_STRUCT_T {
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

typedef struct HAL_G2D_OVERLAY_LAYER_STRUCT_T {
    bool layerEnable;

    G2D_CANVAS_INFO_STRUCT layerCanvas;
    G2D_WINDOW_STRUCT layerWindow;

    bool layerAlphaEnable;
    uint32_t layerAlphaValue;

    bool srcKeyEnable;
    uint32_t srcKeyValue;

    hal_g2d_overlay_layer_function_t function;

    g2d_font_bit_t fontBit;
    uint32_t foregroundColor;

    uint32_t rectFillColor;
    hal_g2d_rotate_angle_t rotation;

} HAL_G2D_OVERLAY_LAYER_STRUCT;



typedef struct G2D_OVERLAY_STRUCT_T {
    HAL_G2D_OVERLAY_LAYER_STRUCT layerInfo[4/*G2D_HW_OVERLAY_TOTAL_LAYER*/];

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

//typedef bool (*hal_g2d_callback_t)(void *);

typedef struct hal_g2d_hanele_struct_t {
    g2d_state_t g2dState;
    hal_g2d_codec_type_t type;

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

    void *intMemAddr;
    uint32_t intMemSize;

#if 1//defined(G2D_HW_DITHERING_SUPPORT)
    bool ditheringEnable;
    g2d_dithering_mode_t ditheringMode;
#endif

#if 1//defined(G2D_HW_SUPPORT) L
    G2D_RECTANGLE_FILL_STRUCT rectFillFunc;
    G2D_FONT_STRUCT fontFunc;
    G2D_BITBLT_STRUCT bitbltFunc;

#if 1//defined(G2D_HW_OVERLAY_SUPPORT) L
    G2D_OVERLAY_STRUCT overlayFunc;
#endif
#endif//defined(G2D_HW_SUPPORT)

    G2D_LT_STRUCT ltFunc;

#if 1//defined(G2D_FPGA) Larkspur compile
    G2D_READ_BURST_TYPE_ENUM readBurstType;
    G2D_WRITE_BURST_TYPE_ENUM writeBurstType;
    bool slowDownEnable;
    uint32_t slowDownCount;

    bool autoTileSizeEnable;
    G2D_TILE_SIZE_TYPE_ENUM tileSize;
    //G2D_LT_CACHE_SIZE_ENUM cacheSize; LT
    //G2D_MAX_OUTSTANDING_NUM_ENUM maxOutstanding;

    G2D_DITHERING_BIT_ENUM ditherBitR;
    G2D_DITHERING_BIT_ENUM ditherBitG;
    G2D_DITHERING_BIT_ENUM ditherBitB;
    uint32_t ditherMw;
    uint32_t ditherMz;

#endif

    bool callbackEnable;
    hal_g2d_callback_t g2dCallback;

} hal_g2d_handle_struct;


#endif  /*g2d_structure.h*/
