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

#ifndef __IUL_GL_X_MATH_H__
#define __IUL_GL_X_MATH_H__

typedef int              GLfixed;
typedef unsigned int     GLuint;
typedef int              GLint;
typedef __int64          GLint64;


#if defined(WIN32)
    #include <assert.h>
    #define GL_ASSERT(x)   assert(x)
#elif defined (__MTK_TARGET__)
    #include "kal_release.h"
    #define GL_ASSERT(X)   ASSERT(X)
#endif


#define _GL_X_FRAC_BITS                          16
#define _GL_X_INT_MASK                   0xffff0000
#define _GL_X_FRAC_MASK                  0x0000ffff

#define _GL_X_NEG_ONE                   ((GLfixed)0xffff0000)   /* S15.16 -1.0 */
#define _GL_X_ZERO                      ((GLfixed)0x00000000)   /* S15.16  0.0 */
#define _GL_X_ONE                       ((GLfixed)0x00010000)   /* S15.16  1.0 */
#define _GL_X_TWO                       ((GLfixed)0x00020000)   /* S15.16  2.0 */
#define _GL_X_THREE                     ((GLfixed)0x00030000)   /* S15.16  3.0 */
#define _GL_X_FOUR                      ((GLfixed)0x00040000)   /* S15.16  4.0 */
#define _GL_X_HALF                      ((GLfixed)0x00008000)   /* S15.16  0.5 */
#define _GL_X_EPSILON                   ((GLfixed)0x00000001)

#define _GL_F_2_X(a)                    ((GLfixed)((a) * _GL_X_ONE))
#define _GL_X_2_F(a)                    ((GLfloat)(a) / (_GL_X_ONE))
#define _GL_I_2_X(a)                    ((a) << _GL_X_FRAC_BITS)
#define _GL_X_2_I(a)                    ((GLfixed)(a) >= 0 ? (a)>>_GL_X_FRAC_BITS : -((-(GLfixed)(a))>>_GL_X_FRAC_BITS))
#define _GL_X_2_INT_POS(a)              ((a)>>_GL_X_FRAC_BITS)


#define _GL_X_ADD(a, b)                 _iul_x_add(a, b)
#define _GL_X_SUB(a, b)                 _iul_x_sub(a, b)
#define _GL_X_MUL(a, b)                 _iul_x_mul(a, b)
#define _GL_X_DIV(a, b)                 _iul_x_div(a, b)
#define _GL_X_SIN(x)                    _iul_x_sin(x)
#define _GL_X_COS(x)                    _iul_x_cos(x)
#define _GL_X_RECIPROCAL_FAST(a)        _iul_x_reciprocal_fast(a)
#define _GL_X_MUL_DIV(m1,m2,d)          _iul_x_mul_div((m1), (m2), (d))
#define _GL_X_FLOOR(a)                  ((a)&(_GL_X_INT_MASK))
#define _GL_X_CEIL(a)                   _GL_X_FLOOR((a)+(_GL_X_ONE-_GL_X_EPSILON))
#define _GL_INT_2_X(a)                  ((a)<<_GL_X_FRAC_BITS)


#if defined(__GNUC__) || defined(__arm)
   typedef long long GLint64;
   typedef unsigned long long GLuint64;
   #define _GL_LL(NUM) NUM##ll
#else
   typedef _int64 GLint64;
   typedef unsigned _int64 GLuint64;
   #define _GL_LL(NUM) NUM
#endif



#ifdef __cplusplus
extern "C" {
#endif

GLfixed _iul_x_add(GLfixed b, GLfixed c);
GLfixed _iul_x_sub(GLfixed b, GLfixed c);
GLfixed _iul_x_mul(GLfixed b, GLfixed c);
GLfixed _iul_x_div(GLfixed b, GLfixed c);
GLfixed _iul_x_sin(GLfixed angle);
GLfixed _iul_x_cos(GLfixed angle);
GLfixed _iul_x_sqrt(GLfixed n);
GLfixed _iul_x_neg_exp(GLfixed power);
GLfixed _iul_x_exp2(GLfixed power);
GLfixed _iul_x_log2(GLfixed n);
GLfixed _iul_x_power(GLfixed base, GLfixed power);

#ifdef __cplusplus
}
#endif

#endif  /* __IUL_GL_X_MATH_H__ */
