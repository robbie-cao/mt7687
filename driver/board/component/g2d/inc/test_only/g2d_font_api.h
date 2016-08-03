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

#ifndef __G2D_FONT_H__
#define __G2D_FONT_H__

#include "g2d_enum.h"
#include "g2d_common_api.h"

/// re-define structure
/// This is for compatible with old AP
/// #define 	g2d_handle_t 		void

/**
 *  Setting the font source buffer information
 *  @param addr         : address pointer of source buffer 
 *  @param color_format : enumeration type of source color format
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks This function sets the source buffer adress of font data, 
 *           The font data to be rendered must stored in one-bit-per-pixel
 *           format. The one bits will be rendered based on foreground color,
 *           and the zero bits will be rendered based on background color if
 *           the background color mode is enable.
 */
g2d_status_t g2dFontSetSrcBufferInfo(g2d_handle_t *handle, uint8_t *bufferAddr, uint32_t bufferSize);



/**
 *  Setting the tilt mode for font caching
 *  @param tilt_mode_enable : enable/disable the tilt mode
 *  @param tilt_table       : array to store tilted values, maximum items is 32
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks This function set the tilt_mode of font and the tilt value for font drawing
 */
g2d_status_t g2dFontSetTiltMode(g2d_handle_t *handle, bool tilt_mode_enable, int32_t *tilt_table, int32_t table_size);


/**
 *  Enable the AA font
 *  @param aa_font_enable    : enable/disable the AA font
 *  @param font_weight_value : The font weight value
 *  @param constant_alpha    : The constant alpha value
 *  @return g2d_status_t  : enumeration type of G2D status
 *  @remarks This function set the tilt_mode of font and the tilt value for font drawing
 */
g2d_status_t g2dFontSetAAFont(g2d_handle_t *handle, bool aa_font_enable);



/**
 *  Set font bit
 *  @param font_bit          : The font bit
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks This function set the tilt_mode of font and the tilt value for font drawing
 */
g2d_status_t g2dFontSetFontBit(g2d_handle_t *handle, G2D_FONT_BIT_ENUM font_bit);


/**
 *  Setting the foreground color for font caching
 *  @param a0 : alpha color component of foreground color
 *  @param r0 : red color component of foreground color
 *  @param g0 : green color component of foreground color
 *  @param b0 : blue components of foreground color
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks This function set the foreground color of font
 */
g2d_status_t g2dFontSetFgColor(g2d_handle_t *handle, uint8_t a0, uint8_t r0, uint8_t g0, uint8_t b0);



/**
 *  Setting the background color for font caching
 *  @param  bg_color_enable : enable/disable background color for font caching
 *  @param  a0              : alpha color components of background color
 *  @param  r0              : red color components of background color
 *  @param  g0              : green color components of background color
 *  @param  b0              : blue color components of background color
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks This function set the background color and bg_color_enable for font caching\n
 *           If bg_color_enable is TRUE, the background color will be appled
 */
///g2d_status_t g2dFontSetBackgroundColor(g2d_handle_t *handle, bool bg_color_enable,uint8_t a0, uint8_t r0, uint8_t g0, uint8_t b0);



/**
 *  Setting the font object
 *  @param  x0 : x-coordinate of top-left vertex of the the destination font rectangle
 *  @param  y0 : x-coordinate of top-left vertex of the the destination font rectangle
 *  @param  width  : width of the destination font rectangle
 *  @param  height : height of the destination font rectangle
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks This function define the rendering font's base location, width, and height 
 */
g2d_status_t g2dFontSetDstWindow(g2d_handle_t *handle, int32_t x0, int32_t y0, int32_t width, int32_t height);



/**
 *  Query the destination ractangle range
 *  @param min_x : minimum of x
 *  @param max_x : maximum of x
 *  @param min_y : minimum of y
 *  @param max_y : maximum of y  
 *  @param min_w : minimum of w
 *  @param max_w : maximum of w
 *  @param min_h : minimum of h
 *  @param max_h : maximum of h 
 *  @return           : enumeration type of G2D status
 *  @remarks  
 */
g2d_status_t g2dFontQueryDstWindowRange(g2d_handle_t *handle, 
                                           int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
                                           uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h);
                                           

/**
 *  Starting the font drawing feature
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks This function start performing the font drawing feature
 */
g2d_status_t g2dFontStart(g2d_handle_t *handle);


#endif /* font.h */
