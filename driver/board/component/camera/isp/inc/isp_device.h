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

#ifndef _ISP_DEVICE_H
#define _ISP_DEVICE_H

#include "aaa_ae.h"
#include "isp_cct_if.h"

#include "sensor_comm_def.h"
#include "cct_comm_def.h"
#include "isp_flashlight_if.h"
#include "aaa_flash.h"


typedef struct
{
    CAMERA_SYSTEM_MODE_ENUM MetaMode;   // Meta/Normal mode
    kal_bool EnableShutterTransfer;                //capture only 
    kal_bool EnableFlashlightTransfer;
    kal_uint16 FlashlightDuty;
    kal_uint16 FlashlightOffset;
    kal_uint16 FlashlightShutFactor;
    kal_uint16 CaptureShutter;
}ISP_DEVICE_FLASHLIGHT_STRUCT,*P_ISP_DEVICE_FLASHLIGHT_STRUCT;

extern CAL_CDT_FLASHLIGHT_CAPTURE_STRUCT IspCdtData;

flashlight_linearity_ae_result_struct *IspGetDeivceLinarityResult(void);
kal_uint8 IspDeviceFlashlightLinearityPrestrobe(void);
kal_uint32 IspDeviceFlashlightCharge(P_CAL_CDT_FLASHLIGHT_CHARGE_STRUCT ChargeData);
void IspDeviceFlashlightStrobe(P_CAL_CDT_FLASHLIGHT_STROBE_STRUCT StrobeData);
kal_uint16 IspDeviceXenonDurationEfficiency(kal_uint16 RequiredDuration);
kal_uint16 IspDeviceXenonDurationInverse(kal_uint16 RequiredDuration);
void IspLoadCdtXenonDurationLut(kal_uint16 *pLutTriggerTime, kal_uint16 *pLutResultTime);
kal_uint8  IspGetCdtFlashlightMode(void);

void IspDeviceFlashlightHisr(void);
const AE_EXPOSURE_LUT_STRUCT *IspDeviceFlashlightCaptureConfig(P_CAL_CDT_FLASHLIGHT_CAPTURE_STRUCT Data);
kal_uint8 IspDeviceFlashlightPrestrobe(camera_capture_cct_struct *IspCctData);
void IspDeviceFlashlightMainstrobe(void);
void IspDeviceFlashlightAeawb(void);
void IspDeviceFlashEnable(kal_bool On);

kal_uint32 IspGetCdtFlashlightChargeLevel(void);
kal_bool IspSetCdtFlashlightChargeLevel(kal_uint32 Level);
AE_EXPOSURE_LUT_STRUCT IspSetCdtFlashlightAeIndex(kal_uint8 Index);
kal_uint8 IspGetCdtCurrentAeIndex(void);

void IspCdtFlashSetAePara(kal_bool EnableFlag, const P_FLASH_AE_CDT_PARA_STRUCT FlashAePara);
void IspCdtFlashGetAePara(P_FLASH_AE_CDT_PARA_STRUCT FlashAePara);
#endif//_ISP_DEVICE_H
