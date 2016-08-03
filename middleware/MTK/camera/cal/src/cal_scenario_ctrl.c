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

//#include "kal_release.h"
//#include "stack_timer.h"

#include "cal_api.h"
#include "cal_if.h"
#include "cal_drv_features.h"
#include "cal_task_msg_if.h"

#include "sensor_comm_def.h"
#include "image_sensor.h"
#include "isp_if.h"
#include "idp_cam_if.h"

//#include "jpeg_mem.h"
//#include "idp_mem_def.h"
#include "isp_mem.h"
//#include "pp_mem_def.h"
//#include "jaia_mem.h"
#include "cal_mem_def.h"
//#include "mpl_recorder.h"
#if (defined(__PANORAMA_VIEW_SUPPORT__)||defined(__AUTOCAP_PANORAMA_SUPPORT__))
#include "feature_comm_def.h"
#endif  //__PANORAMA_VIEW_SUPPORT__ || __AUTOCAP_PANORAMA_SUPPORT__
#if defined(__ZERO_SHUTTER_DELAY_SUPPORT__)
#include "isp_comm_def.h"
#endif //__ZERO_SHUTTER_DELAY_SUPPORT__

//#include "drv_features.h"
//#include "drv_sw_features_mdp.h"

#include "string.h"

#include "sensor_frame_rate_lut.h"

//#include "cache_sw.h"   /* For dynamic switch cacheability */
//#include "mmu.h"        /* For dynamic switch cacheability */

// That I can turn it all off here...
#ifndef CAL_DD_ASSERT
#define CAL_DD_ASSERT(_expr_) ASSERT(_expr_)
#endif

#if (defined(__MULTIPLE_ANGLE_VIEW_SUPPORT__))
#include "mav_encoder.h"
#include "mav_mem.h"
#endif

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code = "DYNAMIC_CODE_CAMCAL_ROCODE", rodata = "DYNAMIC_CODE_CAMCAL_ROCODE"
#endif

MM_IMAGE_ROTATE_ENUM DisplayImageSrcRotAngle;
MM_IMAGE_ROTATE_ENUM DisplayImageRotAngle=MM_IMAGE_ROTATE_0;
MM_IMAGE_ROTATE_ENUM MdpDisplayImageRotAngle;

CAL_MDP_PREVIEW_STRUCT CalPreviewInfo;

IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT SensorScenarioParaIn;
IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT SensorScenarioParaOut;

kal_uint32 IdpCalKey;

P_ISP_SCENARIO_CTRL_STRUCT pIspScenarioCtrl=(P_ISP_SCENARIO_CTRL_STRUCT) CalFeatureCtrlBuff1;
P_ISP_FEATURE_GET_OUTPUT_RESOLUTION_IN_STRUCT pIspGetOutResParaIn=(P_ISP_FEATURE_GET_OUTPUT_RESOLUTION_IN_STRUCT) CalFeatureCtrlBuff1;
P_ISP_FEATURE_GET_OUTPUT_RESOLUTION_OUT_STRUCT pIspGetOutResParaOut=(P_ISP_FEATURE_GET_OUTPUT_RESOLUTION_OUT_STRUCT) CalFeatureCtrlBuff2;

MM_IMAGE_ROTATE_ENUM GetRestRotAngle(MM_IMAGE_ROTATE_ENUM DesiredRotAngle,
                                     MM_IMAGE_ROTATE_ENUM AlreadyRotAngle)
{
    MM_IMAGE_ROTATE_ENUM RestRotAngle=MM_IMAGE_ROTATE_0;

    if (DesiredRotAngle<MM_IMAGE_H_MIRROR)
    {
        if (AlreadyRotAngle<MM_IMAGE_H_MIRROR)
        {
            RestRotAngle = (MM_IMAGE_ROTATE_ENUM) (DesiredRotAngle + MM_IMAGE_H_MIRROR - AlreadyRotAngle);
            RestRotAngle %= MM_IMAGE_H_MIRROR;
        }
        else
        {
            RestRotAngle = (MM_IMAGE_ROTATE_ENUM) (AlreadyRotAngle - DesiredRotAngle);
            RestRotAngle %= MM_IMAGE_H_MIRROR;
            RestRotAngle += MM_IMAGE_H_MIRROR;
        }
    }
#if 1//def __CAL_DUAL_CAMERA_SUPPORT__  // only front camera need to do H mirror
    else if (DesiredRotAngle<MM_NO_OF_IMAGE_ROTATE)
    {
        if (AlreadyRotAngle<MM_IMAGE_H_MIRROR)
        {
            RestRotAngle = (MM_IMAGE_ROTATE_ENUM) (DesiredRotAngle - AlreadyRotAngle);
            RestRotAngle %= MM_IMAGE_H_MIRROR;
            RestRotAngle += MM_IMAGE_H_MIRROR;
        }
        else
        {
            RestRotAngle = (MM_IMAGE_ROTATE_ENUM)(AlreadyRotAngle + MM_IMAGE_H_MIRROR - DesiredRotAngle) ;
            RestRotAngle %= MM_IMAGE_H_MIRROR;
        }
    }
#endif
    else
        configASSERT(0);

    return RestRotAngle;
}   /* GetRestRotAngle() */

MM_ERROR_CODE_ENUM ConfigIspGetResPara(kal_uint16 ZoomStep, kal_uint16 TargetWidth, kal_uint16 TargetHeight,
                                       CAL_CAMERA_DIGITAL_ZOOM_TYPE_ENUM DzType)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    kal_uint32 FeatureOutLen;

        pIspGetOutResParaIn->ZoomStep=ZoomStep;
        pIspGetOutResParaIn->SensorImageGrabStartX=SensorScenarioParaOut.GrabStartX;
        pIspGetOutResParaIn->SensorImageGrabStartY=SensorScenarioParaOut.GrabStartY;
        pIspGetOutResParaIn->SensorImageWidth=SensorScenarioParaOut.ExposureWindowWidth;
        pIspGetOutResParaIn->SensorImageHeight=SensorScenarioParaOut.ExposureWindowHeight;
        pIspGetOutResParaIn->WaitStableFrameNum=SensorScenarioParaOut.WaitStableFrameNum;
        pIspGetOutResParaIn->TargetWidth=TargetWidth;
        pIspGetOutResParaIn->TargetHeight=TargetHeight;
        pIspGetOutResParaIn->DzType=DzType;
        pIspGetOutResParaIn->pZoomFactorListTable=CurrentDzInfo.pZoomFactorListTable;
        ErrorCode=pfIspFunc->IspFeatureCtrl(ISP_FEATURE_GET_OUTPUT_RESOLUTION, pIspGetOutResParaIn,
                                            pIspGetOutResParaOut, sizeof(ISP_FEATURE_GET_OUTPUT_RESOLUTION_OUT_STRUCT),
                                            &FeatureOutLen);
    return ErrorCode;
}   /* ConfigIspGetResPara() */

MM_IMAGE_ROTATE_ENUM GetSensorRotAngle(MM_IMAGE_ROTATE_ENUM ImageRotAngle,
                                       img_rot_angle_enum_t *MdpRotAngle,
                                       IMAGE_SENSOR_MIRROR_ENUM *SensorMirror)
{
    MM_IMAGE_ROTATE_ENUM SensorRotate;

    switch (ImageRotAngle)
    {
        case MM_IMAGE_ROTATE_180:
            *MdpRotAngle= (img_rot_angle_enum_t) MM_IMAGE_ROTATE_0;
            *SensorMirror = IMAGE_SENSOR_MIRROR_HV;
            SensorRotate = MM_IMAGE_ROTATE_180;
        break;
        case MM_IMAGE_ROTATE_270:
            *MdpRotAngle = (img_rot_angle_enum_t) MM_IMAGE_ROTATE_90;
            *SensorMirror = IMAGE_SENSOR_MIRROR_HV;
            SensorRotate = MM_IMAGE_ROTATE_180;
        break;
    #ifdef __CAL_DUAL_CAMERA_SUPPORT__
        case MM_IMAGE_H_MIRROR_ROTATE_90:
            *MdpRotAngle = (img_rot_angle_enum_t) MM_IMAGE_ROTATE_270;
            *SensorMirror = IMAGE_SENSOR_MIRROR_H;
            SensorRotate = MM_IMAGE_H_MIRROR;
        break;
        case MM_IMAGE_H_MIRROR_ROTATE_180:
            *MdpRotAngle = (img_rot_angle_enum_t) MM_IMAGE_ROTATE_0;
            *SensorMirror = IMAGE_SENSOR_MIRROR_V;
            SensorRotate = MM_IMAGE_H_MIRROR_ROTATE_180;
        break;
        case MM_IMAGE_H_MIRROR_ROTATE_270:
            *MdpRotAngle = (img_rot_angle_enum_t) MM_IMAGE_ROTATE_90;
            *SensorMirror = IMAGE_SENSOR_MIRROR_H;
            SensorRotate = MM_IMAGE_H_MIRROR;
        break;
        case MM_IMAGE_H_MIRROR:
            *MdpRotAngle = (img_rot_angle_enum_t) MM_IMAGE_ROTATE_0;
            *SensorMirror = IMAGE_SENSOR_MIRROR_H;
            SensorRotate = MM_IMAGE_H_MIRROR;
        break;
    #endif
        default :
            *MdpRotAngle = (img_rot_angle_enum_t) ImageRotAngle;
            *SensorMirror = IMAGE_SENSOR_MIRROR_NORMAL;
            SensorRotate = MM_IMAGE_ROTATE_0;
        break;
    }

    return SensorRotate;
}

MM_ERROR_CODE_ENUM CalPreviewCtrl(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    P_CAL_CAMERA_PREVIEW_STRUCT pCalPreviewPara = (P_CAL_CAMERA_PREVIEW_STRUCT) pCtrlPara;
    P_CAL_ISP_PREVIEW_STRUCT pCalIspPreviewPara=pCalPreviewPara->pIspPara;
    P_CAL_MDP_PREVIEW_STRUCT pCalMdpPreviewPara=pCalPreviewPara->pMdpPara;
    ISP_CAMERA_PREVIEW_STRUCT IspPreviewPara;

    ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_IN_STRUCT IspDigitalZoomResParaIn;
    ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT IspDigitalZoomResParaOut;

    ISP_FEATURE_SET_TG_STRUCT IspTgSettingInfo;
    SENSOR_FRAMERATE_IN_STRUCT  SensorQueryFrameRateIn;
    SENSOR_FRAMERATE_OUT_STRUCT SensorQueryFrameRateOut;

    // for IDP capability query
    IDP_CAL_QUERY_ARG_T MdpCapQueryPara;
    kal_uint32 MdpCapQueryResult=0;
    kal_uint16 MdpResGetCount=0;

    kal_uint32 FeatureOutLen;
    kal_uint32 CtrlOutLen;

#ifdef __CAL_MINIMIZE_GLOBAL_VARIABLE__
    Camera_Preview_Idp_Cal_Struct MdpPreviewPara;
#endif

    LOG_I(hal, "Enter CalPreviewCtrl");

    if (CAL_CTRL_CODE_STOP & CtrlCode)
    {
        pIspScenarioCtrl->ScenarioCtrlCode=CAL_CTRL_CODE_STOP;
        ErrorCode=pfIspFunc->IspCtrl(ScenarioId, pIspScenarioCtrl,  NULL, 0, &CtrlOutLen);

        MdpFunc.pfnIdpCalClose(ScenarioId, IdpCalKey);
        CalState = CAL_STANDBY_STATE;

        return ErrorCode;
    }
    if (CAL_CTRL_CODE_START & CtrlCode)
    {
        IspTgSettingInfo.ScenarioId=CAL_SCENARIO_CAMERA_PREVIEW;
        IspTgSettingInfo.pSensorInfo=(void *) &CurrentSensorInfo;
        //pfIspFunc->IspFeatureCtrl(ISP_FEATURE_SET_TG, &IspTgSettingInfo, NULL, 0,&FeatureOutLen);

        /* config sensor driver */
        GetSensorRotAngle(pCalMdpPreviewPara->ImageRotAngle, &MdpPreviewPara.display_rot_angle, &SensorScenarioParaIn.ImageMirror);

        IspPreviewPara.ImageMirror=SensorScenarioParaIn.ImageMirror;

#if !defined (MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
        // if CP triple buffer does not support, set FrameBuffAddr3 to NULL
        pCalMdpPreviewPara->FrameBuffAddr3 = (kal_uint32)NULL;
#endif
        memcpy(&CalPreviewInfo, pCalMdpPreviewPara, sizeof(CAL_MDP_PREVIEW_STRUCT));

        while(MdpFunc.pfnIdpCalIsInUse(CAL_SCENARIO_CAMERA_PREVIEW))
        {
            vTaskDelay(10/portTICK_PERIOD_MS);
            MdpResGetCount++;
            if (10<=MdpResGetCount)
                configASSERT(0);
        }

        ErrorCode = MdpFunc.pfnIdpCalOpen( ScenarioId, &IdpCalKey, NULL, 0);

        // query the rotation angle that MDP can support.
        // Pass the rest rotation angle to Display driver.
        // if the rotation angle is depend on post image process, then CAL will rotate the image for display driver.
        MdpCapQueryPara.ePath=IDP_CAL_QUERY_ARG_DISPLAY_PATH;
        MdpCapQueryPara.eClrFmt=(MM_IMAGE_COLOR_FORMAT_ENUM) pCalMdpPreviewPara->ImageDataFormat;
        MdpCapQueryPara.eDesiredAngle=MdpPreviewPara.display_rot_angle;
        MdpFunc.pfnIdpCalQuery(ScenarioId, IdpCalKey, IDP_CAL_QUERY_ROT_CAP, &MdpCapQueryPara, &MdpCapQueryResult);

        /* Idp only return 0 degree in MT2523 */
        DisplayImageRotAngle=GetRestRotAngle((MM_IMAGE_ROTATE_ENUM)MdpPreviewPara.display_rot_angle, (MM_IMAGE_ROTATE_ENUM) MdpCapQueryResult);

        MdpPreviewPara.display_rot_angle = (img_rot_angle_enum_t) MdpCapQueryResult;
        MdpDisplayImageRotAngle = (MM_IMAGE_ROTATE_ENUM) MdpPreviewPara.display_rot_angle;
        DisplayImageSrcRotAngle = (MM_IMAGE_ROTATE_ENUM) MdpPreviewPara.display_rot_angle;

        MdpPreviewPara.frame_buffer_address1= pCalMdpPreviewPara->FrameBuffAddr1;
        MdpPreviewPara.frame_buffer_address2= pCalMdpPreviewPara->FrameBuffAddr2;
        MdpPreviewPara.frame_buffer_address3= pCalMdpPreviewPara->FrameBuffAddr3;
        MdpPreviewPara.frame_buffer_temp = pCalMdpPreviewPara->FrameBuffAddrTemp;
        MdpPreviewPara.dbuf_tearing_free_enable = KAL_TRUE;


        SensorScenarioParaIn.ImageTargetWidth=pCalMdpPreviewPara->DisplayImageWidth;
        SensorScenarioParaIn.ImageTargetHeight=pCalMdpPreviewPara->DisplayImageHeight;
        if (CAM_NIGHTSCENE==pCalIspPreviewPara->SceneMode)
            SensorScenarioParaIn.NightMode=KAL_TRUE;
        else
            SensorScenarioParaIn.NightMode=KAL_FALSE;

        SensorScenarioParaIn.MetaMode=CAMERA_SYSTEM_NORMAL_MODE;
        SensorScenarioParaIn.ContrastLevel=pCalIspPreviewPara->ContrastLevel;
        SensorScenarioParaIn.BrightnessLevel=pCalIspPreviewPara->BrightnessLevel;
        SensorScenarioParaIn.SaturationLevel=pCalIspPreviewPara->SaturationLevel;
        SensorScenarioParaIn.WbMode=pCalIspPreviewPara->WbMode;
        SensorScenarioParaIn.EvValue=pCalIspPreviewPara->EvValue;
        SensorScenarioParaIn.BandingFreq=pCalIspPreviewPara->BandingFreq;
        SensorScenarioParaIn.ImageEffect=pCalIspPreviewPara->ImageEffect;
        SensorScenarioParaIn.HueValue=pCalIspPreviewPara->HueValue;

        SensorQueryFrameRateIn.Scenario= CAL_SCENARIO_CAMERA_PREVIEW;
        SensorQueryFrameRateIn.CameraIf=  CurrentSensorInfo.SensorIf;
        SensorQueryFrameRateIn.DataFormat= CurrentSensorInfo.PreviewNormalDataFormat;
        SensorQueryFrameRateIn.SourceWidth= CurrentSensorInfo.PreviewWidth;
        SensorQueryFrameRateIn.SourceHeight= CurrentSensorInfo.PreviewHeight;
        SensorQueryFrameRateIn.NighhtMode= SensorScenarioParaIn.NightMode;
        ErrorCode= SensorCommonGetFrameRate(&SensorQueryFrameRateIn, &SensorQueryFrameRateOut);
        if(ErrorCode == MM_ERROR_NONE)
            SensorScenarioParaIn.MaxVideoFrameRate = SensorQueryFrameRateOut.MaxSensorFrameRate;
        else
            SensorScenarioParaIn.MaxVideoFrameRate = 300;
        {

        pfImageSensor->SensorCtrl(ScenarioId, &SensorScenarioParaIn, &SensorScenarioParaOut,
                                  sizeof(IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT),&CtrlOutLen);
        }

        /* calculate digital zoom step */
        CurrentZoomInfo.CurrentZoomStep=pCalIspPreviewPara->ZoomStep;

        pIspGetOutResParaIn->IspState=ISP_PREVIEW_STATE; 

        ErrorCode=ConfigIspGetResPara(pCalIspPreviewPara->ZoomStep, pCalMdpPreviewPara->DisplayImageWidth,
                                      pCalMdpPreviewPara->DisplayImageHeight, CurrentDzInfo.DzType);

        IspDigitalZoomResParaIn.DzType=CurrentDzInfo.DzType;
        IspDigitalZoomResParaIn.pZoomFactorListTable=CurrentDzInfo.pZoomFactorListTable;
        IspDigitalZoomResParaIn.ZoomStep=pCalIspPreviewPara->ZoomStep;
        ErrorCode=pfIspFunc->IspFeatureCtrl(ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION, &IspDigitalZoomResParaIn,
                                            &IspDigitalZoomResParaOut, sizeof(ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT),
                                            &FeatureOutLen);
        MdpPreviewPara.mdp_crop_enable=IspDigitalZoomResParaOut.MdpCropEnable;
        MdpPreviewPara.cropped_width=IspDigitalZoomResParaOut.DigitalZoomWidth;
        MdpPreviewPara.cropped_height=IspDigitalZoomResParaOut.DigitalZoomHeight;

        /* config idp */
        MdpPreviewPara.source_width = pIspGetOutResParaOut->IspOutWidth;
        MdpPreviewPara.source_height = pIspGetOutResParaOut->IspOutHeight;
        MdpPreviewPara.display_width = pCalMdpPreviewPara->DisplayImageWidth;
        MdpPreviewPara.display_height = pCalMdpPreviewPara->DisplayImageHeight;
        MdpPreviewPara.preview_image_color_format=(MM_IMAGE_COLOR_FORMAT_ENUM)pCalMdpPreviewPara->ImageDataFormat;
        MdpPreviewPara.digital_zoom_frame_start_hisr_cb_enable=KAL_FALSE;
        MdpPreviewPara.lcd_update_enable = KAL_TRUE;

        ErrorCode = MdpFunc.pfnIdpCalSet( ScenarioId, IdpCalKey, IDP_CAL_SET_CONFIG, &MdpPreviewPara);
        ErrorCode = MdpFunc.pfnIdpCalStart( ScenarioId, IdpCalKey);
        configASSERT(ErrorCode == MM_ERROR_NONE);

        /* config and start ISP */
        IspPreviewPara.ImageTargetWidth=pCalMdpPreviewPara->DisplayImageWidth;
        IspPreviewPara.ImageTargetHeight=pCalMdpPreviewPara->DisplayImageHeight;
        IspPreviewPara.pCalIspPreviewPara=pCalIspPreviewPara;

        pIspScenarioCtrl->ScenarioCtrlCode=CAL_CTRL_CODE_START;
        pIspScenarioCtrl->pScenarioCtrlPara=&IspPreviewPara;
        pIspScenarioCtrl->ScenarioCtrlParaLen=sizeof(ISP_CAMERA_PREVIEW_STRUCT);
        ErrorCode=pfIspFunc->IspCtrl(ScenarioId, pIspScenarioCtrl,  NULL, 0, &CtrlOutLen);

        if(MM_ERROR_NONE != ErrorCode)
        { //MM_ERROR_ISP_NO_VSYNC
            MdpFunc.pfnIdpCalClose(ScenarioId, IdpCalKey);
            CalState = CAL_STANDBY_STATE;

            return ErrorCode;
        }

        CalState = CAL_CAMERA_PREVIEW_STATE;

#if 0
    #ifdef __CAL_PROFILING_ENABLE__
        CameraCaptureProfilingWrapper(CAL_CAMERA_PROFILING_CAMERA_PREVIEW_COMPLETE);
    #endif
#endif
        return ErrorCode;
    }
    return ErrorCode;
} /* CalPreviewCtrl() */


/* To Start/Stop/Pause Camera preview, Camera Capture and Video Capture */
MM_ERROR_CODE_ENUM CalScenarioCtrl(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode,
                                   void *pCtrlPara, kal_uint16 CtrlParaLen)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;

    switch (ScenarioId)
    {
    #ifdef __CAL_MATV_SUPPORT__
        case CAL_SCENARIO_MATV:
    #endif
        case CAL_SCENARIO_CAMERA_PREVIEW:
            ErrorCode=CalPreviewCtrl(ScenarioId, CtrlCode, pCtrlPara, CtrlParaLen);
        break;
        case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        #ifdef __ZERO_SHUTTER_DELAY_SUPPORT__
            IsZsdScenario=KAL_FALSE;
        #endif
            //ErrorCode=CalStillCaptureCtrl(ScenarioId, CtrlCode, pCtrlPara, CtrlParaLen);
        break;
        case CAL_SCENARIO_VIDEO:
    #ifdef __MEDIA_VT__
        case CAL_SCENARIO_VIDEO_TELEPHONY:
    #endif
            //ErrorCode=CalVideoCtrl(ScenarioId, CtrlCode, pCtrlPara, CtrlParaLen);
        break;
    #ifdef WEBCAM_SUPPORT
        case CAL_SCENARIO_WEBCAM_PREVIEW:
        case CAL_SCENARIO_WEBCAM_CAPTURE:
            //ErrorCode=CalWebcamCtrl(ScenarioId, CtrlCode, pCtrlPara, CtrlParaLen);
        break;
    #endif
    #ifdef __ZERO_SHUTTER_DELAY_SUPPORT__
        case CAL_SCENARIO_CAMERA_ZSD_PREVIEW:
            //ErrorCode=CalPreviewCtrl(ScenarioId, CtrlCode, pCtrlPara, CtrlParaLen);
        break;
        case CAL_SCENARIO_CAMERA_ZSD_CAPTURE:
            IsZsdScenario=KAL_TRUE;
            //ErrorCode=CalStillCaptureCtrl(ScenarioId, CtrlCode, pCtrlPara, CtrlParaLen);
        break;
    #endif
        default :
        break;
    }

    return ErrorCode;
}   /* CalScenarioCtrl() */

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif

