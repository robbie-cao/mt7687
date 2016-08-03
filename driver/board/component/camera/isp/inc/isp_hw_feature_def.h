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

#ifndef __ISP_HW_FEATURE_DEF_H__
#define __ISP_HW_FEATURE_DEF_H__

//#include "drv_features.h"
#include "drv_features_isp.h"

#undef __ISP_BAYER_SENSOR_SUPPORT__			/* raw sensor support */
#undef __ISP_YUV_SENSOR_SUPPORT__				/* yuv sensor support */
#undef __ISP_COMPRESSED_YUV_SENSOR_SUPPORT__    /* compressed yuv output data sensor support */
#undef __ISP_RGB_SENSOR_SUPPORT__                           /*rgb format sensor support */
#undef __ISP_HW_CAPTURE_JPEG_DATA_SUPPORT__	/* isp hw jpeg sensor interface support */

#undef __ISP_PARALLE_SENSOR_SUPPORT__                   /* parallel interface support */
#undef __ISP_MIPI_SENSOR_SUPPORT__                          /* mipi interface support */
#undef __ISP_SERIAL_SENSOR_SUPPORT__                    /* serial interface support */
#undef __ISP_DIRECT_SENSOR_SUPPORT__                    /* direct interface support */ 

#undef __ISP_CPLL_SUPPORT__						/* cpll support 38,36,76 series*/
#undef __ISP_UPLL_SUPPORT__						/* upll 48M support */
#undef __ISP_CROP_WINDOW_SUPPORT__			/* digital zoom crop by isp */

#undef __ISP_BURST_READ_4_4_SUPPORT__                   /*Memory in burst read 4 x 4bytes for each read, 6268*/
#undef __ISP_BURST_READ_16_4_SUPPORT__                  /*Memory in burst read 4 x 4bytes for each read, 6236*/
#undef __ISP_BAYER_10_BIT_PACKET_OUTPUT_SUPPORT__   /*ISP output 10bit package(3 pixel in one word) rawdata support, 6236*/
#undef __ISP_AUTO_RECAPTURE_SUPPORT__                  /* isp auto recapture when capture fail, 6236*/
#undef __ISP_MEM_IN_SUPPORT__					/* isp mem in function */
#undef __ISP_MEM_OUT_SUPPORT__					/* isp mem dump function */
#undef __ISP_CRZ_HW_FRAME_SYNC_SUPPORT__		/* auto re-send full frame to crz */
#undef __ISP_RESET_MCLK_OUTPUT_INDEPENDENT__	/* reset isp won't reset mclk : 76,55,53EL,75,52,...*/

#undef __ISP_CAPTURE_MODE_BEST_SHOT_SUPPORT__		/* isp capture mode  */
#undef __ISP_CAPTURE_MODE_CONT_SHOT_SUPPORT__		/* isp capture mode  */
#undef __ISP_CAPTURE_MODE_BURST_SHOT_SUPPORT__	/* isp capture mode  */
#undef __ISP_CAPTURE_MODE_EV_BRACKET_SUPPORT__	/* isp capture mode  */
#undef __ISP_CAPTURE_MODE_ADD_FRAME_SUPPORT__		/* isp capture mode  */

// define some comoon compile option for ISP drivers of various chips.
#define __ISP_CAPTURE_MODE_CONT_SHOT_SUPPORT__		

#if defined(MT2523)
#define __ISP_YUV_SENSOR_SUPPORT__
#define __ISP_RGB_SENSOR_SUPPORT__
#define __ISP_PARALLE_SENSOR_SUPPORT__
#define __ISP_SERIAL_SENSOR_SUPPORT__
#define __ISP_DIRECT_SENSOR_SUPPORT__
#define __ISP_UPLL_SUPPORT__

#define __ISP_CAPTURE_MODE_CONT_SHOT_SUPPORT__		
#endif


#endif /* __ISP_HW_FEATURE_DEF_H__ */
