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

#ifndef __ISP_MEM_H__
#define __ISP_MEM_H__

#include "sensor_capability.h"
//#include "jpeg_mem.h"

#define ISP_MAX_EXT_MEM(A,B)         ( ((A)>(B)) ? (A) : (B))

#if(defined(ISP_SUPPORT))

  #if(defined(MT6268)||defined(MT6268A)||defined(MT6268H)||defined(MT6268T)||defined(MT6236))
  	#define ISP_EXT_WORKING_BUF_SIZE    (0)
	#if(defined(RAW_MAIN_CAMERA)||defined(RAW_BAK1_CAMERA)||defined(RAW_SUB_CAMERA)||defined(RAW_SUB_BAK1_CAMERA))
	  #if(defined(CAM_OFFLINE_CAPTURE)||defined(CAM_STANDARD_CAPTURE))
	    #if(defined(MT6236))
	 	   #define ISP_IMAGE_BUF_SIZE            (((((IMAGE_SENSOR_SOURCE_MAX_WIDTH *IMAGE_SENSOR_SOURCE_MAX_HEIGHT)*14)/10)+31)&0xFFFFFFE0)
	    #else
	    	#define ISP_IMAGE_BUF_SIZE            (((IMAGE_SENSOR_SOURCE_MAX_WIDTH *IMAGE_SENSOR_SOURCE_MAX_HEIGHT)+32)&0xFFFFFFE0)
	    #endif
	  #else//defined(CAM_OTF_CAPTURE)
	  	#define ISP_IMAGE_BUF_SIZE    (0)
	  #endif    
	#else
	  #define ISP_IMAGE_BUF_SIZE    (0)
	#endif

  #else // YUV sensor case    MT6235/53/53EL/58/76
  
//    #ifdef __CAMERA_MOTION_SENSOR_SUPPORT_BY_TAG__
    #if 1	
        #define ISP_JPEG_SENSOR_PARSE_BUF_SIZE  ((LCD_WIDTH*LCD_HEIGHT*2 > 640*480*2L) ? (LCD_WIDTH*LCD_HEIGHT*2) : (640 * 480 *2L))
    #else
        #define ISP_JPEG_SENSOR_PARSE_BUF_SIZE  (640*480*2L)
    #endif

      #if defined(__JPEG_SENSOR_SUPPORT__)
      #ifndef JPEG_SENSOR_WORKING_BUF_SIZE
      #define JPEG_SENSOR_WORKING_BUF_SIZE  (0)
      #endif	  
//      #define ISP_EXT_WORKING_BUF_SIZE           (JPEG_DEC_INT_MIN_MEM_SIZE+JPEG_DEC_EXT_MIN_MEM_SIZE+ ISP_JPEG_SENSOR_PARSE_BUF_SIZE)  //define in  image_sensor_xxxx.h  (((4096)+31)&0xFFFFFFE0)
      #define ISP_EXT_WORKING_BUF_SIZE           (((JPEG_DEC_INT_MIN_MEM_SIZE+ISP_MAX_EXT_MEM(JPEG_SW_DEC_EXT_MIN_MEM_SIZE,JPEG_HW_DEC_EXT_MIN_MEM_SIZE)+ ISP_JPEG_SENSOR_PARSE_BUF_SIZE)+31)&0xffffffe0)  //define in  image_sensor_xxxx.h  (((4096)+31)&0xFFFFFFE0)
      #else
      #define ISP_EXT_WORKING_BUF_SIZE            (0)
      #endif
	  
      #if (defined(MT6276)||defined(MT6256))
      #define ISP_IMAGE_BUF_SIZE            (0)
      #elif defined(MT6255) // the memory is not used by ISP, it used by MDP rotdma for capture memory out. allocate by ISP is used for min. memory
	  #if defined(__JPEG_SENSOR_SUPPORT__)
	  #define ISP_IMAGE_BUF_SIZE            ((IMAGE_SENSOR_SOURCE_MAX_WIDTH * IMAGE_SENSOR_SOURCE_MAX_HEIGHT * 3/2/4+31)&0xFFFFFFE0)
	  #else
	  #define ISP_IMAGE_BUF_SIZE            ((IMAGE_SENSOR_SOURCE_MAX_WIDTH * IMAGE_SENSOR_SOURCE_MAX_HEIGHT * 3/2+31)&0xFFFFFFE0)
	  #endif
      #else  //35/53/53EL/76
	  
            #if (defined(__YUVCAM_INTERPOLATION_SW__))
            #define ISP_IMAGE_BUF_SIZE           ((((IMAGE_SENSOR_SOURCE_MAX_WIDTH * IMAGE_SENSOR_SOURCE_MAX_HEIGHT*3)>>1) +31)&0xFFFFFFE0) // for y,u,v buffer
            #else //hw interpolation   or others use capture target size
            #define ISP_IMAGE_BUF_SIZE           ((((IMAGE_SENSOR_TARGET_MAX_WIDTH * IMAGE_SENSOR_TARGET_MAX_HEIGHT*3)>>1) +31)&0xFFFFFFE0) // for y,u,v buffer
            #endif
      #endif
	  
  #endif

#endif//#if(defined(ISP_SUPPORT))

#endif /* _ISP_MEM_H */
