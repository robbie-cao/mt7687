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

#ifndef __FEATURE_COMM_DEF_H__
#define __FEATURE_COMM_DEF_H__

#include "drv_features_isp.h"

#if defined(__AUTOCAP_PANORAMA_SUPPORT__)
    #if (defined(__MMI_MAINLCD_480X800__)||defined(__MMI_MAINLCD_800X480__))
    #define PANORAMA_MAX_INPUT_IMAGE_SIZE_1M
    #if (defined(__AUTOCAP_PANORAMA_MODE_3__))
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (3)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (2800)
    #else
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (4)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (3800)
    #endif
    #else
    #define PANORAMA_MAX_INPUT_IMAGE_SIZE_VGA
    #if (defined(__AUTOCAP_PANORAMA_MODE_3__))
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (3)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (1800)
    #elif (defined(__AUTOCAP_PANORAMA_MODE_4__))
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (4)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (2000)
    #elif (defined(__AUTOCAP_PANORAMA_MODE_5__)) // RAM: 5.4MB
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (5)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (2000)
    #elif (defined(__AUTOCAP_PANORAMA_MODE_6__)) // RAM: 7MB
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (6)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (3230)
    #elif (defined(__AUTOCAP_PANORAMA_MODE_7__))
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (7)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (3600)
    #elif (defined(__AUTOCAP_PANORAMA_MODE_8__)) // RAM:9.4MB
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (8)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (3800)
    #else
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (8)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (3800)
    #endif
    #endif
    
    #define PANORAMA_JPEG_ENCODE_SOURCE_FORMAT_YUV420
    #define PANORAMA_INPUT_IMAGE_FORMAT_YUV420
    #define PANO_MAX_OUTPUT_IMG_WIDTH PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH

    #if (defined(MT6235))
        #define AUTOCAP_SOURCE_IMAGE_FROM_CRZ
    #else /* others)*/
        #define AUTOCAP_SOURCE_IMAGE_FROM_PRZ
    #endif
#else
#if (defined(DRV_ISP_6235_SERIES))
    #define PANORAMA_MAX_INPUT_IMAGE_SIZE_VGA
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (3)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (1832)
    #define PANORAMA_JPEG_ENCODE_SOURCE_FORMAT_YUV420
    #define PANORAMA_INPUT_IMAGE_FORMAT_YUV420
#elif (defined(DRV_ISP_6238_SERIES) || defined(DRV_ISP_6276_SERIES))
    #if (defined(DRV_ISP_MT6268_HW_SUPPORT))
        #define PANORAMA_MAX_INPUT_IMAGE_SIZE_2M
        #define PANORAMA_MAX_INPUT_IMAGE_NUM        (3)
        #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (4095)
        #define PANORAMA_JPEG_ENCODE_SOURCE_FORMAT_RGB565
        #define PANORAMA_INPUT_IMAGE_FORMAT_JPEG
    #else /*(defined(MT6236) || defined(MT6276))*/
        #define PANORAMA_MAX_INPUT_IMAGE_SIZE_2M
        #define PANORAMA_MAX_INPUT_IMAGE_NUM        (3)
        #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (4095)
        #define PANORAMA_JPEG_ENCODE_SOURCE_FORMAT_YUV420
        #define PANORAMA_INPUT_IMAGE_FORMAT_JPEG
    #endif
#else /* others */
    #define PANORAMA_MAX_INPUT_IMAGE_SIZE_VGA
    #define PANORAMA_MAX_INPUT_IMAGE_NUM        (3)
    #define PANORAMA_JPEG_ENCODE_SOURCE_MAX_WIDTH   (1832)
    #define PANORAMA_JPEG_ENCODE_SOURCE_FORMAT_YUV420
    #define PANORAMA_INPUT_IMAGE_FORMAT_JPEG
#endif
#endif

#if defined(__MULTIPLE_ANGLE_VIEW_SUPPORT__)
#define MAV_MAX_SUPPORT_IMAGE_NUM 9    // It can be set at most to 15
#elif defined(__3D_IMAGE_SUPPORT__)
#define MAV_MAX_SUPPORT_IMAGE_NUM 2    // left and right eyes
#else
#define MAV_MAX_SUPPORT_IMAGE_NUM 1
#endif

#if (defined(MT6235))
    #define FD_SOURCE_IMAGE_FROM_CRZ
#else /* others)*/
    #define FD_SOURCE_IMAGE_FROM_PRZ
#endif


#endif /* __FEATURE_COMM_DEF_H__ */
