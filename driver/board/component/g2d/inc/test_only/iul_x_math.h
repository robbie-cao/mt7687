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

#ifndef __IUL_X_MATH_H__
#define __IUL_X_MATH_H__

#include "iul_gl_x_math.h"

typedef int IUL_FIXED;                                   ///< Type definition as S15.16 fixed-point

#define IUL_I_TO_X(i)         ((i) << 16)                ///< Convert from integer to S15.16 fixed-point
#define IUL_X_TO_I(x)         (((x) + (1 << 15)) >> 16)  ///< Convert from S15.16 fixed-point to integer (round)
#define IUL_X_TO_I_ROUND(x)   (((x) + (1 << 15)) >> 16)  ///< Convert from S15.16 fixed-point to integer (round)
#define IUL_X_TO_I_CHOP(x)    ((x) >> 16)                ///< Convert from S15.16 fixed-point to integer (chop)
#define IUL_X_TO_I_CARRY(x)   (((x) + 0x0000FFFF) >> 16) ///< Convert from S15.16 fixed-point to integer (carry)
#define IUL_X_FRACTION(x)     ((x) & 0x0000FFFF)

#define IUL_X_HALF            (1 << 15)

#define IUL_X_TO_F(x)         ((float)x / 65536.0)
#define IUL_F_TO_X(x)         (IUL_FIXED)((x) * 65536.0)

#define IUL_X_MUL(a, b)       (((a) * (b)) >> 16)        /// S15.16 fixed-point multiplication, the overflow is not taken into consideration
                                                         /// for fixed-point multiplication with overflow checking, please use iul_x_mul(a, b)


/// function prototype declaration
IUL_FIXED iul_x_add(IUL_FIXED a, IUL_FIXED b);
IUL_FIXED iul_x_sub(IUL_FIXED a, IUL_FIXED b);
IUL_FIXED iul_x_mul(IUL_FIXED a, IUL_FIXED b);
IUL_FIXED iul_x_div(IUL_FIXED n, IUL_FIXED d);
IUL_FIXED iul_x_sin(IUL_FIXED angle);
IUL_FIXED iul_x_cos(IUL_FIXED angle);
IUL_FIXED iul_x_sqrt(IUL_FIXED n);
IUL_FIXED iul_x_power(IUL_FIXED base, IUL_FIXED power);
IUL_FIXED iul_x_log2(IUL_FIXED n);


#endif /*__IUL_X_MATH_H__*/
