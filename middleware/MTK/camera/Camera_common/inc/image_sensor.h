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

#ifndef _IMAGE_SENSOR_H
#define _IMAGE_SENSOR_H

//#include "image_sensor_nvram.h"
#include "sensor_comm_def.h"

#if (defined(SIC320A_SERIAL_YUV))
#include "image_sensor_SIC320A_SERIAL.h"
#endif	

#if (defined(GC6113_SERIAL_YUV))
#include "image_sensor_GC6113_SERIAL.h"
#endif	

#if (defined(GC6123_SERIAL_YUV))
#include "image_sensor_GC6123_SERIAL.h"
#endif

#if (defined(GC0310_SERIAL_YUV))
#include "image_sensor_GC0310_SERIAL.h"
#endif

#if (defined(HM0155_SERIAL_YUV))
#include "image_sensor_HM0155_SERIAL.h"
#endif	

#if (defined(BF3901_SERIAL_YUV))
#include "image_sensor_BF3901_SERIAL.h"
#endif

#if (defined(SP0820_SERIAL_YUV))
#include "image_sensor_SP0820_SERIAL.h"
#endif	

/* Serial Sensor (SPI Interface) ID*/
#define SIC320A_SERIAL_SENSOR_ID				0x0011
#define GC6113_SERIAL_SENSOR_ID					0x00B8
#define GC6123_SERIAL_SENSOR_ID					0x00B9
#define GC0310_SERIAL_SENSOR_ID					0xA310
#define HM0155_SERIAL_SENSOR_ID					0x1055
#define BF3901_SERIAL_SENSOR_ID					0x3901
#define SP0820_SERIAL_SENSOR_ID					0x009C

#define ADD_SENSOR(id, prefix_name, SensorIdx)\
{id, prefix_name##SensorFunc, SensorIdx }

/*-----------------------Variables declaration------------------------*/
extern IMAGE_SENSOR_LIST_STRUCT SensorList[MAX_SENSOR_SUPPORT_NUMBER + 1];

MM_ERROR_CODE_ENUM GetSensorInitFunc(P_IMAGE_SENSOR_LIST_STRUCT *pSensorList);

/* ------------------------ Camera HW API ------------------------ */
void CamRstPinCtrl(IMAGE_SENSOR_INDEX_ENUM Idx, kal_int8 Data);
void CamPdnPinCtrl(IMAGE_SENSOR_INDEX_ENUM Idx, kal_int8 Data);
void CamGetHWInfo(IMAGE_SENSOR_INDEX_ENUM Idx, P_IMAGE_SENSOR_HW_DEPENDENT_STRUCT Info);
void CisModulePowerOn(IMAGE_SENSOR_INDEX_ENUM Idx, kal_bool On);
void FlashlightPowerOn(IMAGE_SENSOR_INDEX_ENUM Idx, kal_bool On);


#endif /* _IMAGE_SENSOR_H */

