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

#ifndef _IMAGE_CAPABILITY_H
#define _IMAGE_CAPABILITY_H

#if defined (ISP_SUPPORT)
    #if (defined(MT9P012_RAW)||(defined(OV5642_RAW))||defined(S5K3E2FX_RAW)||defined(MT9P012_MIPI_RAW)||defined(OV5630_RAW)||defined(MT9P013_RAW) \
       ||defined(OV5650_RAW)||defined(OV5642_YUV)||defined(OV5650_MIPI_RAW)||defined(OV5642_MIPI_RAW)||defined(OV5640_YUV)||defined(MT9P015_RAW)||defined(S5K4ECGX_YUV)||defined(MT9P111_YUV)||defined(ET8EV3_MIPI_RAW))
        #define __IMAGE_SENSOR_5M__
    #elif (defined(MT9T012_RAW)||defined(MT9T013_RAW)||defined(OV3640_RAW)||defined(OV3647_RAW)||defined(OV3640_YUV)||defined(MT9T113_YUV)||defined(S5K5CAGA_YUV))
        #define __IMAGE_SENSOR_3M__
    #elif (defined(OV2640_RAW)||defined(OV2640_YUV)||defined(OV2650_RAW)||defined(OV2650_YUV)\
         ||defined(MT9D112_YUV)||defined(OV2655_RAW)||defined(OV2659_YUV)||defined(S5K5BAFX_YUV)||defined(T8ET2_YUV)\
         ||defined(MT9D113_YUV)||defined(MT9D115_YUV)||defined(SID130B_YUV)||defined(HI251_YUV)||defined(HI253_YUV)||defined(GT2005_YUV)||defined(SID020A_RAW))
        #define __IMAGE_SENSOR_2M__
    #elif (defined(OV9660_YUV)||defined(SIM120C_YUV)||defined(OV9665_YUV)||defined(NOON130PC51_YUV))
        #define __IMAGE_SENSOR_1M__
    #elif (defined(BF3703_YUV)||defined(OV7670_YUV)||defined(SIV121D_YUV)||defined(S5KA3DFX_YUV)||defined(MC501CB_YUV)||defined(SIV100A_YUV)||defined(SIV100B_YUV)||defined(SIV120B_YUV)||defined(SIV120A_YUV)\
         ||defined(OV3640VGA_YUV)||defined(HIV1CF_YUV)||defined(GC0308_YUV)||defined(OV7675_YUV)||defined(HI704_YUV)||defined(PAS6327_YUV)||defined(OV7690_YUV)||defined(NOON010PC30_YUV)||defined(GC0307_YUV)||defined(GC0329_YUV)\
         ||defined(PAS6180_SERIAL_YUV)||defined(HIQD1_SERIAL_YUV))||defined(GC0310_SERIAL_YUV)
        #if defined(__MTK_INTERNAL__)
            #if (defined(__IMAGE_SENSOR_PRETEND_CIF__))
                #define __IMAGE_SENSOR_01M__
            #else
                #define __IMAGE_SENSOR_03M__
            #endif
        #else
            #define __IMAGE_SENSOR_03M__
        #endif
    #elif (defined(GC6113_SERIAL_YUV)||defined(GC6123_SERIAL_YUV)||defined(SIC110A_YUV)||defined(SIC320A_SERIAL_YUV)||defined(HM0155_SERIAL_YUV)||defined(BF3901_SERIAL_YUV)||defined(SP0820_SERIAL_YUV))
        #define __IMAGE_SENSOR_01M__
    #elif (defined(OV2655_YUV))
        #if defined(__MTK_INTERNAL__)
            #if defined(__IMAGE_SENSOR_PRETEND_VGA__)
                #define __IMAGE_SENSOR_03M__
            #elif defined(__IMAGE_SENSOR_PRETEND_CIF__)
                #define __IMAGE_SENSOR_01M__
            #else
                #define __IMAGE_SENSOR_2M__
            #endif
        #else
            #define __IMAGE_SENSOR_2M__
        #endif
    #else
        #define __IMAGE_SENSOR_UNDEFINED__
        #error "Please add the sensor part number into correct resolution case!"
    #endif

    #ifdef DUAL_CAMERA_SUPPORT
        #if (defined(MT9P012_RAW_SUB)||(defined(OV5642_RAW_SUB))||defined(S5K3E2FX_RAW_SUB)||defined(MT9P012_MIPI_RAW_SUB)||defined(OV5630_RAW_SUB)||defined(MT9P013_RAW_SUB) \
           ||defined(OV5650_RAW_SUB)||defined(OV5642_YUV_SUB)||defined(OV5642_MIPI_RAW_SUB)||defined(OV5640_YUV_SUB)||defined(MT9P015_RAW_SUB)||defined(S5K4ECGX_YUV_SUB)||defined(MT9P111_YUV_SUB))
            #define __IMAGE_SENSOR_SUB_5M__
        #elif (defined(MT9T012_RAW_SUB)||defined(MT9T013_RAW_SUB)||defined(OV3640_RAW_SUB)||defined(OV3647_RAW_SUB)||defined(OV3640_YUV_SUB)||defined(MT9T113_YUV_SUB))
            #define __IMAGE_SENSOR_SUB_3M__
        #elif (defined(OV2640_RAW_SUB)||defined(OV2640_YUV_SUB)||defined(OV2650_RAW_SUB)||defined(OV2650_YUV_SUB)\
             ||defined(MT9D112_YUV_SUB)||defined(OV2655_RAW_SUB)||defined(OV2659_YUV_SUB)||defined(S5K5BAFX_YUV_SUB)||defined(T8ET2_YUV_SUB)\
             ||defined(MT9D113_YUV_SUB)||defined(MT9D115_YUV_SUB)||defined(SID130B_YUV_SUB)||defined(HI251_YUV_SUB)||defined(HI253_YUV_SUB)||defined(GT2005_YUV_SUB)||defined(SID020A_RAW_SUB))
            #define __IMAGE_SENSOR_SUB_2M__
        #elif (defined(OV9660_YUV_SUB)||defined(SIM120C_YUV_SUB)||defined(OV9665_YUV_SUB)||defined(NOON130PC51_YUV_SUB))
            #define __IMAGE_SENSOR_SUB_1M__
        #elif (defined(OV7670_YUV_SUB)||defined(S5KA3DFX_YUV_SUB)||defined(MC501CB_YUV_SUB)||defined(SIV100A_YUV_SUB)||defined(SIV100B_YUV_SUB)||defined(SIV120B_YUV_SUB)||defined(SIV120A_YUV_SUB)\
             ||defined(OV3640VGA_YUV_SUB)||defined(HIV1CF_YUV_SUB)||defined(OV7675_YUV_SUB)||defined(HI704_YUV_SUB)||defined(PAS6327_YUV_SUB)||defined(OV7690_YUV_SUB)||defined(NOON010PC30_YUV_SUB)||defined(GC0307_YUV_SUB)||defined(GC0329_YUV_SUB)\
             ||defined(PAS6180_SERIAL_YUV)||defined(HIQD1_SERIAL_YUV))||defined(GC0310_SERIAL_YUV_SUB)
            #define __IMAGE_SENSOR_SUB_03M__
        #elif (defined(GC6113_SERIAL_YUV_SUB)||defined(GC6123_SERIAL_YUV_SUB)||defined(SIC110A_YUV_SUB)||defined(SIC320A_SERIAL_YUV_SUB)||defined(HM0155_SERIAL_YUV_SUB)||defined(BF3901_SERIAL_YUV_SUB)||defined(SP0820_SERIAL_YUV_SUB))
            #define __IMAGE_SENSOR_SUB_01M__
        #elif (defined(OV2655_YUV_SUB))
            #if (defined(__FLAVOR_MULTIMEDIA_HQA__) || defined(__FLAVOR_MULTIMEDIA_MP4E_HQA__))
                #define __IMAGE_SENSOR_SUB_2M__
            #else
                #define __IMAGE_SENSOR_SUB_03M__
            #endif
        #else
            #error "Please add the sensor part number into correct resolution case!"
        #endif
    #endif

    /* Sensor support limitation on MTK chips */
    #undef __USED_IMAGE_SENSOR_OVER_VGA__
    #undef __USED_IMAGE_SENSOR_OVER_1M__
    #undef __USED_IMAGE_SENSOR_OVER_2M__
    #undef __USED_IMAGE_SENSOR_OVER_3M__
    #undef __USED_IMAGE_SENSOR_OVER_5M__
    #if defined(__IMAGE_SENSOR_UNDEFINED__)||defined(__IMAGE_SENSOR_03M__)||defined(__IMAGE_SENSOR_01M__)
         //
    #elif defined(__IMAGE_SENSOR_1M__)
         #define __USED_IMAGE_SENSOR_OVER_VGA__
    #elif defined(__IMAGE_SENSOR_2M__)
         #define __USED_IMAGE_SENSOR_OVER_VGA__
         #define __USED_IMAGE_SENSOR_OVER_1M__
    #elif defined(__IMAGE_SENSOR_3M__)
         #define __USED_IMAGE_SENSOR_OVER_VGA__
         #define __USED_IMAGE_SENSOR_OVER_1M__
         #define __USED_IMAGE_SENSOR_OVER_2M__
    #elif defined(__IMAGE_SENSOR_5M__)
         #define __USED_IMAGE_SENSOR_OVER_VGA__
         #define __USED_IMAGE_SENSOR_OVER_1M__
         #define __USED_IMAGE_SENSOR_OVER_2M__
         #define __USED_IMAGE_SENSOR_OVER_3M__
    #else
         #error
         //#define __USED_IMAGE_SENSOR_OVER_VGA__
         //#define __USED_IMAGE_SENSOR_OVER_1M__
         //#define __USED_IMAGE_SENSOR_OVER_2M__
         //#define __USED_IMAGE_SENSOR_OVER_3M__
         //#define __USED_IMAGE_SENSOR_OVER_5M__
    #endif
    
    // check by chip
    #if ( defined(MT6252)||defined(MT6252H) )
        #if defined(__USED_IMAGE_SENSOR_OVER_VGA__)
        #error "MT6252 cannot support sensor resolution bigger then VGA"
        #endif
    #endif
    
    #if ( defined(MT6253) )
        #if defined(__EMI_CLK_52MHZ__)       // 53DV EMI 52MHz
            #if defined(__USED_IMAGE_SENSOR_OVER_VGA__)
                #error "MT6253 with 52MHz EMI cannot support sensor resolution bigger then VGA"
            #endif
        #else                                               // normal EMI 104MHz
            #if defined(__USED_IMAGE_SENSOR_OVER_2M__)
                #error "MT6253 cannot support sensor resolution bigger then 2M"
            #endif
        #endif
    #endif
    
    #if ( defined(MT6235)||defined(MT6235B) )
        #if defined(__USED_IMAGE_SENSOR_OVER_2M__)
            #error "MT6235 cannot support sensor resolution bigger then 2M"
        #endif
    #endif
    
    #if ( defined(MT6268)||defined(MT6236)||defined(MT6236B) )
        #if defined(__USED_IMAGE_SENSOR_OVER_5M__)
            #error "MT6268/36 cannot support sensor resolution bigger then 5M"
        #endif
    #endif
    
    #if ( defined(MT6276)||defined(MT6256) )
        #if defined(__JPEG_SENSOR_SUPPORT__)             // jpeg sensor format
             // jpeg sensor has no limitation
        #else                                                                 // yuv sensor format
            #if defined(__USED_IMAGE_SENSOR_OVER_3M__)
            #error "MT6276/56 cannot support yuv sensor resolution bigger then 3M"
            #endif
        #endif
    #endif

#endif	/* #if defined (ISP_SUPPORT) */

#define IMAGE_01M_MAX_WIDTH                            (352+6)
#define IMAGE_03M_MAX_WIDTH                            (640+6)
#define IMAGE_1M_MAX_WIDTH                             (1280+6)
#define IMAGE_2M_MAX_WIDTH                             (1600+6)
#define IMAGE_3M_MAX_WIDTH                             (2048+6)
#define IMAGE_5M_MAX_WIDTH                             (2592+6)
#define IMAGE_8M_MAX_WIDTH                             (3264+6)

#define IMAGE_01M_MAX_HEIGHT                            (288+6)
#define IMAGE_03M_MAX_HEIGHT                            (480+6)
#define IMAGE_1M_MAX_HEIGHT                             (960+6)
//#define IMAGE_1M_MAX_HEIGHT                             (1024+6)
#define IMAGE_2M_MAX_HEIGHT                             (1200+6)
#define IMAGE_3M_MAX_HEIGHT                             (1536+6)
#define IMAGE_5M_MAX_HEIGHT                             (1944+6)
#define IMAGE_8M_MAX_HEIGHT                             (2448+6)

#if defined(__IMAGE_SENSOR_01M__)
    #define IMAGE_SENSOR_SOURCE_MAX_WIDTH                           IMAGE_01M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_MAX_HEIGHT                          IMAGE_01M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_03M__)
    #define IMAGE_SENSOR_SOURCE_MAX_WIDTH                           IMAGE_03M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_MAX_HEIGHT                          IMAGE_03M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_1M__)
    #define IMAGE_SENSOR_SOURCE_MAX_WIDTH                           IMAGE_1M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_MAX_HEIGHT                          IMAGE_1M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_2M__)
    #define IMAGE_SENSOR_SOURCE_MAX_WIDTH                           IMAGE_2M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_MAX_HEIGHT                          IMAGE_2M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_3M__)
    #define IMAGE_SENSOR_SOURCE_MAX_WIDTH                           IMAGE_3M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_MAX_HEIGHT                          IMAGE_3M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_5M__)
    #define IMAGE_SENSOR_SOURCE_MAX_WIDTH                            IMAGE_5M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_MAX_HEIGHT                          IMAGE_5M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_8M__)
    #define IMAGE_SENSOR_SOURCE_MAX_WIDTH                           IMAGE_8M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_MAX_HEIGHT                          IMAGE_8M_MAX_HEIGHT
#else
    //#error "The interpolation for this resolution doesn't support now"
    #define IMAGE_SENSOR_SOURCE_MAX_WIDTH                          0
    #define IMAGE_SENSOR_SOURCE_MAX_HEIGHT                         0
#endif


#if (defined(__YUVCAM_INTERPOLATION_SW__) || defined(__YUVCAM_INTERPOLATION_HW__))
    #if defined(__IMAGE_SENSOR_01M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_03M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_03M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_03M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_1M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_1M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_1M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_2M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_2M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_2M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_3M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_3M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_3M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_5M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_5M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_5M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_8M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_8M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_8M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_8M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_8M_MAX_HEIGHT
    #else
        //#error "The interpolation for this resolution doesn't support now"
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                          0
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                         0
    #endif
#else
    #if defined(__IMAGE_SENSOR_01M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_01M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_01M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_03M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_03M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_03M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_1M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_1M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_1M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_2M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_2M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_2M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_3M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_3M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_3M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_5M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_5M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_5M_MAX_HEIGHT
    #elif defined(__IMAGE_SENSOR_8M__)
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                           IMAGE_8M_MAX_WIDTH
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                          IMAGE_8M_MAX_HEIGHT
    #else
        //#error "The interpolation for this resolution doesn't support now"
        #define IMAGE_SENSOR_TARGET_MAX_WIDTH                          0
        #define IMAGE_SENSOR_TARGET_MAX_HEIGHT                         0
    #endif
#endif


#if defined(__IMAGE_SENSOR_SUB_03M__)
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_WIDTH                           IMAGE_03M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_HEIGHT                          IMAGE_03M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_SUB_1M__)
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_WIDTH                           IMAGE_1M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_HEIGHT                          IMAGE_1M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_SUB_2M__)
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_WIDTH                           IMAGE_2M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_HEIGHT                          IMAGE_2M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_SUB_3M__)
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_WIDTH                           IMAGE_3M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_HEIGHT                          IMAGE_3M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_SUB_5M__)
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_WIDTH                           IMAGE_5M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_HEIGHT                          IMAGE_5M_MAX_HEIGHT
#elif defined(__IMAGE_SENSOR_SUB_8M__)
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_WIDTH                           IMAGE_8M_MAX_WIDTH
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_HEIGHT                          IMAGE_8M_MAX_HEIGHT
#else
    //#error "The interpolation for this resolution doesn't support now"
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_WIDTH                           0
    #define IMAGE_SENSOR_SOURCE_SUB_MAX_HEIGHT                          0
#endif


#endif /* _IMAGE_CAPABILITY_H */
