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

#ifndef __G2D_MACRO_H__
#define __G2D_MACRO_H__


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

/*

/// RGB565 to RGB888
#define RGB565_R_MASK 0xF800
#define RGB565_G_MASK 0x07E0
#define RGB565_B_MASK 0x001F

#define RGB565_TO_RGB888_R(VALUE)   ((((VALUE) & RGB565_R_MASK)>>8))
#define RGB565_TO_RGB888_G(VALUE)   ((((VALUE) & RGB565_G_MASK)>>3))
#define RGB565_TO_RGB888_B(VALUE)   ((((VALUE) & RGB565_B_MASK)<<3))

#define A_OF_RGB8888(c)              (((c)&0xFF000000 ) >> 24)
#define R_OF_RGB8888(c)              (((c)&0x00FF0000 ) >> 16)
#define G_OF_RGB8888(c)              (((c)&0x0000FF00 ) >> 8)
#define B_OF_RGB8888(c)              (((c)&0x000000FF ))
*/

#endif  /*g2d_macro.h*/
