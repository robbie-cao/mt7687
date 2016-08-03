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

#ifndef __CAM_SW_TYPES_H
#define __CAM_SW_TYPES_H

/*
 * Definitions for BOOLEAN
 */

#define FALSE          0
#define TRUE           1
#undef NULL
#define NULL           0

typedef void *kal_semid;
typedef void *kal_taskid;

typedef char                    kal_char;
typedef unsigned short          kal_wchar;
typedef unsigned char           kal_uint8;
typedef signed char             kal_int8;
typedef unsigned short int      kal_uint16;
typedef signed short int        kal_int16;
typedef unsigned int            kal_uint32;
typedef signed int              kal_int32;
/* portable 64-bit unsigned integer */
typedef unsigned long long   kal_uint64;
/* portable 64-bit signed integer */
typedef signed long long     kal_int64;

typedef unsigned int  		  U32;
typedef unsigned short int    U16;
typedef unsigned char  		  U8;

typedef volatile unsigned char *          P_U8;
typedef volatile signed char *            P_S8;
typedef volatile unsigned short *         P_U16;
typedef volatile signed short *           P_S16;
typedef volatile unsigned int *           P_U32;
typedef volatile signed int *             P_S32;

typedef enum 
{
    KAL_FALSE,
    KAL_TRUE
} kal_bool;

#define DRV_WriteReg16(addr,data)     ((*(volatile kal_uint16 *)(addr)) = (kal_uint16)(data))
#define DRV_Reg16(addr)               (*(volatile kal_uint16 *)(addr))
#define DRV_WriteReg32(addr,data)     ((*(volatile kal_uint32 *)(addr)) = (kal_uint32)(data))
#define DRV_Reg32(addr)               (*(volatile kal_uint32 *)(addr))
#define DRV_WriteReg8(addr,data)     ((*(volatile kal_uint8 *)(addr)) = (kal_uint8)(data))
#define DRV_Reg8(addr)               (*(volatile kal_uint8 *)(addr))

typedef kal_uint32 kal_status;

typedef enum
{
    KAL_NO_WAIT,
    KAL_INFINITE_WAIT
} kal_wait_mode;

#endif
