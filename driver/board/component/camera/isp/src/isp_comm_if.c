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

#include "isp_comm_def.h"
#include "sensor_comm_def.h"
#include "cal_comm_def.h"

#include "isp_if.h"

//#include "isp_pca.h" //PCA related 
#include "isp_comm_if.h"
//#include "camera_tuning_para.h"
//#include "aaa_awb.h"    

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code = "DYNAMIC_CODE_CAMCAL_ROCODE", rodata = "DYNAMIC_CODE_CAMCAL_ROCODE"
#endif


CameraDriverOperationParaStruct CameraDriverOperParaBuffer;


void CameraDriverDefaultOperationPara(CameraDriverOperationParaStruct *p_oper_data)
{
    p_oper_data->yuv_af_timeout_frame = 20;
}


void CameraTuningDefaultOperationPara(CameraTuningOperationParaStruct *p_oper_data)
{
}


void CameraDriverCheckOperationPara(CameraDriverOperationParaStruct *p_oper_data)
{
    /* To protect invalide value */
    if(p_oper_data->capture_delay_frame > 3)
    {
        p_oper_data->capture_delay_frame=2;
    }
    /* To protect invalide value */
    if((p_oper_data->preview_delay_frame > 16) || (p_oper_data->preview_delay_frame == 0))
    {
        p_oper_data->preview_delay_frame = 2;
    }

#ifdef LED_FLASHLIGHT_SUPPORT
    if(p_oper_data->flashlight_prestrobe_frame == 0)
            p_oper_data->flashlight_prestrobe_frame = 8;

    if(p_oper_data->flashlight_prestrobe_to_redeyestrobe_delay_frame == 0)
            p_oper_data->flashlight_prestrobe_to_redeyestrobe_delay_frame = 2;

    if(p_oper_data->flashlight_redeyestrobe_frame == 0)
            p_oper_data->flashlight_redeyestrobe_frame = 2;

    if(p_oper_data->flashlight_mainstrobe_frame == 0)
            p_oper_data->flashlight_mainstrobe_frame = 2;
#endif
}


void CameraTuningCheckOperationPara(CameraTuningOperationParaStruct *p_oper_data)
{
}


void CameraInitOperationPara(void)
{
    // for camera HAL UT
    IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT CameraInitOperationPara;
    // for camera HAL UT
    register CameraDriverOperationParaStruct *p_driver_para = &CameraDriverOperParaBuffer;
    CameraDriverDefaultOperationPara(p_driver_para);
    //  marked for camera HAL UT
    //  CameraDriverInitOperationPara(p_driver_para);
    CustomDriverFunc.SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_GET_INIT_OPERATION_PARA, NULL, &CameraInitOperationPara,
                                       sizeof(IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT), &SensorFeatureParaOutLen);

    p_driver_para->capture_delay_frame =CameraInitOperationPara.CaptureDelayFrame;
    p_driver_para->preview_delay_frame =CameraInitOperationPara.PreviewDelayFrame;
    p_driver_para->preview_display_wait_frame =CameraInitOperationPara.PreviewDisplayWaitFrame;

#ifdef LED_FLASHLIGHT_SUPPORT
    ///For YUV LED Flashlight control
    p_driver_para->flashlight_prestrobe_frame = CameraInitOperationPara.FlashlightPrestrobeFrame;
    p_driver_para->flashlight_prestrobe_to_redeyestrobe_delay_frame = CameraInitOperationPara.FlashlightPrestrobeToRedeyeFrame;
    p_driver_para->flashlight_redeyestrobe_frame = CameraInitOperationPara.FlashlightRedeyeStrobeFrame;
    p_driver_para->flashlight_mainstrobe_frame = CameraInitOperationPara.FlashlightMainStrobeDelayFrame;
#endif

    CameraDriverCheckOperationPara(p_driver_para);

}


#if (defined(__ATV_SUPPORT__)&& defined(__MTK_TARGET__))
void MatvInitOperationPara(void)
{
    // for camera HAL UT
    IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT CameraInitOperationPara;
    // for camera HAL UT
    register CameraDriverOperationParaStruct *p_driver_para = &CameraDriverOperParaBuffer;
    CameraDriverDefaultOperationPara(p_driver_para);
    //  marked for camera HAL UT
    //  CameraDriverInitOperationPara(p_driver_para);
    CustomDriverFunc.SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_GET_INIT_OPERATION_PARA, NULL, &CameraInitOperationPara,
                                       sizeof(IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT), &SensorFeatureParaOutLen);

    p_driver_para->capture_delay_frame =CameraInitOperationPara.CaptureDelayFrame;
    p_driver_para->preview_delay_frame =CameraInitOperationPara.PreviewDelayFrame;
    p_driver_para->preview_display_wait_frame =CameraInitOperationPara.PreviewDisplayWaitFrame;

    CameraDriverCheckOperationPara(p_driver_para);

}
#endif

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif

