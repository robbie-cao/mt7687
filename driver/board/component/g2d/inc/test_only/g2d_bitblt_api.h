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

#ifndef __G2D_BITBLT_H__
#define __G2D_BITBLT_H__


#include "g2d_enum.h"
#include "g2d_common_api.h"

/// re-define structure
/// This is for compatible with old AP
/// #define 	g2d_handle_t 		void

/**
 *  Setting the bitblt source buffer information
 *  @param addr         : address pointer of source buffer
 *  @param width        : the source buffer width
 *  @param height       : the source buffer height
 *  @param color_format : enumeration type of source color format
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks  This fuction sets the source buffer used in bitblt feature
 *            The buffer must be assigned before start bitblt operation
 */
g2d_status_t g2dBitBltSetSrcRGBBufferInfo(g2d_handle_t *handle,
                                             uint8_t *bufferAddr, uint32_t bufferSize,
                                             uint32_t width, uint32_t height, g2d_color_format_t color_format);



/**
 *  Setting the bitblt source rectangle information
 *  @param x : x-coordinate of top-left point in source bitblt rectangle
 *  @param y : y-coordinate of top-left point in source bitblt rectangle
 *  @param w : bitblt source rectangle width
 *  @param h : bitblt source rectangle height
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks  This function sets the source rectangle of the bitblt region,
 *            The rectangle must be assigned before start bitblt operation
 */
g2d_status_t g2dBitBltSetSrcWindow(g2d_handle_t *handle, int32_t x, int32_t y, uint32_t w, uint32_t h);




/**
 *  Setting the bitblt destination rectangle information
 *  @param x : x-coordinate of top-left point in destination bitblt rectangle
 *  @param y : y-coordinate of top-left point in destination bitblt rectangle
 *  @param w : bitblt destination rectangle width
 *  @param h : bitblt destination rectangle height
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks  This function sets the destination rectangle of the bitblt region,
 *            The rectangle must be assigned before start bitblt operation
 */
g2d_status_t g2dBitBltSetDstWindow(g2d_handle_t *handle, int32_t x, int32_t y, uint32_t w, uint32_t h);




/**
 *  Setting the bitblt rotation mode
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks This function sets the different rotation types of bitblt operation \n
 *           based on enumeration types of BITBLT_ROT_ANGLE_ENUM. There are 7 types \n
 *           of rotation can be specified
 *           The rotation can be appied in "bitlt only","bitblt with alpha blending", and
 *           "bitblt with ROP2" at the same time
 *
 */
g2d_status_t g2dBitBltSetRotation(g2d_handle_t *handle, G2D_CW_ROTATE_ANGLE_ENUM rotation);



/**
 *  Enable Bitblt with alpha blending
 *  @param alpha_blend_enable : To enable alpha blending or not
 *  @param constant_alpha : The constant alpha value
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks  This function sets the alpha value for bitblt operation,
 *            If the alpha channel exists( in ARGB8888 or ARGB4444 color format), \n
 *            the per-pixel alpha is used for blending operation instead of constant alpha
 */
g2d_status_t g2dBitBltSetSrcAlpha(g2d_handle_t *handle, bool src_alpha_enable, uint32_t src_alpha_value);



/**
 *  Enable transparent
 *  @param alpha_tarnsparent_enable : To enable tarnsparent or not
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks
 */
g2d_status_t g2dBitBltSetDstAlpha(g2d_handle_t *handle, bool dst_alpha_enable, uint32_t dst_alpha_value);



/**
 *  Set index-color table
 *  @param palette_table : Palette table base address
 *  @param palette_size  : Palette table size
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks
 */
g2d_status_t  g2dBitBltSetIndexColorTable(g2d_handle_t *handle, uint32_t *palette_table, G2D_INDEX_COLOR_TABLE_SIZE_ENUM palette_size);



/**
 *  Query the srouce buffer range
 *  @param min_width  : minimum of width
 *  @param max_width  : maximum of width
 *  @param min_height : minimum of height
 *  @param max_height : maximum of height
 *  @return           : enumeration type of G2D status
 *  @remarks
 */
g2d_status_t g2dBitBltQuerySrcRGBBufferRange(g2d_handle_t *handle, uint32_t *min_width, uint32_t *max_width, uint32_t *min_height, uint32_t *max_height);


/**
 *  Query the source ractangle range
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
g2d_status_t g2dBitBltQuerySrcWindowRange(g2d_handle_t *handle,
                                             int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
                                             uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h);


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
g2d_status_t g2dBitBltQueryDstWindowRange(g2d_handle_t *handle,
                                             int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
                                             uint32_t *min_w, uint32_t *max_w, uint32_t *min_h, uint32_t *max_h);


/**
 *  Start the bitblt operation feature
 *  @return g2d_status_t : enumeration type of G2D status
 *  @remarks This function start performing the bitblt operation based \n
 *           on previous API setting
 */
g2d_status_t g2dBitBltStart(g2d_handle_t *handle);




#endif
