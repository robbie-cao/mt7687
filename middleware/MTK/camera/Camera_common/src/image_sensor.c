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

#include "image_sensor.h"
#include "sensor_comm_def.h"

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code = "DYNAMIC_CODE_CAMCAL_ROCODE", rodata = "DYNAMIC_CODE_CAMCAL_ROCODE"
#endif

// for camera HAL
IMAGE_SENSOR_LIST_STRUCT SensorList[MAX_SENSOR_SUPPORT_NUMBER + 1] =
{
#if(defined(GC0310_SERIAL_YUV_MAIN))
        ADD_SENSOR(GC0310_SERIAL_SENSOR_ID,GC0310_SERIAL,IMAGE_SENSOR_MAIN),
#endif
#if(defined(SP0820_SERIAL_YUV_MAIN))
	ADD_SENSOR(SP0820_SERIAL_SENSOR_ID,SP0820_SERIAL,IMAGE_SENSOR_MAIN),
#endif
#if(defined(HM0155_SERIAL_YUV_MAIN))
	ADD_SENSOR(HM0155_SERIAL_SENSOR_ID,HM0155_SERIAL,IMAGE_SENSOR_MAIN),
#endif
#if(defined(GC6123_SERIAL_YUV_MAIN))
	ADD_SENSOR(GC6123_SERIAL_SENSOR_ID,GC6123_SERIAL,IMAGE_SENSOR_MAIN),
#endif
#if(defined(GC6113_SERIAL_YUV_MAIN))
	ADD_SENSOR(GC6113_SERIAL_SENSOR_ID,GC6113_SERIAL,IMAGE_SENSOR_MAIN),
#endif
#if(defined(SIC320A_SERIAL_YUV_MAIN))
	ADD_SENSOR(SIC320A_SERIAL_SENSOR_ID,SIC320A_SERIAL,IMAGE_SENSOR_MAIN),
#endif
#if(defined(BF3901_SERIAL_YUV_MAIN))
	ADD_SENSOR(BF3901_SERIAL_SENSOR_ID,BF3901_SERIAL,IMAGE_SENSOR_MAIN),
#endif
};

// for camera HAL
MM_ERROR_CODE_ENUM GetSensorInitFunc(P_IMAGE_SENSOR_LIST_STRUCT *pSensorList)
{
    *pSensorList=&SensorList[0];

    return MM_ERROR_NONE;
}

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif

