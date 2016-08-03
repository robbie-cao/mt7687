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
#include "cal_api.h"
#include "cal_if.h"

#include "cal_comm_def.h"
#include "isp_comm_def.h"
//#include "dcl.h"

#include "isp_hw_feature_def.h"
#include "isp_if.h"
#include "isp_comm_if.h"
#include "isp_hw_limitation.h"

#if defined (__AUTO_SCENE_DETECT_SUPPORT__) && defined (YUV_MAIN_CAMERA)
#include "camera_tuning_para.h"
#endif

#if defined (DRV_ISP_PCA_SUPPORT) && defined (__AUTO_SCENE_DETECT_SUPPORT__)
#include "isp_pca.h"
#endif

//#include "aaa_process.h" 
//#include "aaa_ae.h"
//#include "aaa_awb.h"
//#include "aaa_af.h"
//#include "mm_power_ctrl.h"
#include "cal_task_msg_if.h"
#if (defined(LED_FLASHLIGHT_SUPPORT))
#include "isp_flashlight_if.h"
#endif

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code = "DYNAMIC_CODE_CAMCAL_ROCODE", rodata = "DYNAMIC_CODE_CAMCAL_ROCODE"
#endif

/*********************temporary********************/
//#include "exif.h"
/*********************temporary********************/
volatile kal_bool IspIdleDoneFlag, IspDoneFlag, IspJpegCaptureStart;
volatile kal_bool FirstSensorFrameFlag;
kal_bool WaitFirstFrameFlag;
kal_uint8 IspPreviewFrameCount;
kal_uint16 IspImageSettingIndex;
#if defined (__AUTO_SCENE_DETECT_SUPPORT__)
kal_uint8 IspAsdCompensationIndex = 0;
CAL_CAMERA_SCENE_MODE_ENUM IspAsdSceneType = CAM_AUTO_DSC;
kal_bool IspAsdEnabled;
#endif
kal_bool IspCheckFirstVsync;

kal_uint32 IspBackupReg0x214;
/******* Not need to add to isp_if_hal.h for extern ******/
#if (defined(__ISP_BAYER_SENSOR_SUPPORT__))
DSC_OPERATION_STRUCT DscStatus;
#endif

//static DCL_HANDLE IspVsyncGptHandle=NULL;
static volatile kal_bool IspVsyncGptTimeOut = KAL_FALSE;
//static DCL_HANDLE IspFrameDoneGptHandle=NULL;
static volatile kal_bool IspFrameDoneGptTimeOut = KAL_FALSE;

#if (defined(EXIF_SUPPORT))
kal_uint8 IspAeMeteringModeTbl[CAM_NO_OF_AE_METERING]= { 0 , 3, 2, 1};
kal_uint8 IspLightSourceModeTbl[CAM_NO_OF_WB]= { 0 , 10, 1, 3, 2, 3 ,0};
#endif

extern kal_bool IspFirstPreviewFlag;
extern kal_bool IspVDReadyEvent, IspFrameDoneEvent;

/*************************************************************************
* FUNCTION
*    InitISP
*
* DESCRIPTION
*    initialize ISP interface for raw sensor
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
*************************************************************************/
void InitIsp(void)
{
    RESET_CMOS_SENSOR;
    POWER_ON_CMOS_SENSOR;
    ENABLE_CMOS_SENSOR;

    ENABLE_CAMERA_FRAME_READY_INT;
    DISABLE_CAMERA_RESIZER_OVERRUN_INT;
    DISABLE_CAMERA_IDLE_INT;
    DISABLE_CAMERA_GMC_OVERRUN_INT;
    DISABLE_CAMERA_ISP_DONE_INT;

    DISABLE_CAMERA_AE_DONE_INT;

    ENABLE_CAMERA_TG_DONE_INT;
}



void IspBackgndImageSetting(void)
{
    kal_uint32 i;
    CAL_FEATURE_CTRL_STRUCT SensorFeatureSetData;

    if((ISP_IDLE_STATE==IspOperationState)||(ISP_STANDBY_STATE==IspOperationState))
    {
        //ASSERT(0);
        return;
    }

    /****************** Need to separate YUV and RAW *********************/
    for (i=0;i<CAL_FEATURE_CAMERA_END;i++)
    {
        if (IspImageSettingIndex & (1<<i))
        {
            SensorFeatureSetData.FeatureCtrlCode=CAL_FEATURE_SET_OPERATION;

            switch (i)
            {
                case CAL_FEATURE_CAMERA_CONTRAST:
                    SensorFeatureSetData.FeatureSetValue=IspQualityTuningData.IspContrastLevel;
                    CustomDriverFunc.SensorFeatureCtrl(i, &SensorFeatureSetData, NULL, 0, NULL);
                break;
                case CAL_FEATURE_CAMERA_SATURATION:
                    SensorFeatureSetData.FeatureSetValue=IspQualityTuningData.IspSaturationLevel;
                    CustomDriverFunc.SensorFeatureCtrl(i, &SensorFeatureSetData, NULL, 0, NULL);
                break;
                case CAL_FEATURE_CAMERA_SHARPNESS:
                    SensorFeatureSetData.FeatureSetValue=IspQualityTuningData.IspSharpnessLevel;
                    CustomDriverFunc.SensorFeatureCtrl(i, &SensorFeatureSetData, NULL, 0, NULL);
                break;
                case CAL_FEATURE_CAMERA_WB:
                    SensorFeatureSetData.FeatureSetValue=IspPreviewConfigData.pCalIspPreviewPara->WbMode;
                    CustomDriverFunc.SensorFeatureCtrl(i, &SensorFeatureSetData, NULL, 0, NULL);
                break;
                case CAL_FEATURE_CAMERA_EV_VALUE:
                    SensorFeatureSetData.FeatureSetValue=IspPreviewConfigData.pCalIspPreviewPara->EvValue;
                    CustomDriverFunc.SensorFeatureCtrl(i, &SensorFeatureSetData, NULL, 0, NULL);
                break;
                case CAL_FEATURE_CAMERA_IMAGE_EFFECT:
                    SensorFeatureSetData.FeatureSetValue=IspPreviewConfigData.pCalIspPreviewPara->ImageEffect;
                    CustomDriverFunc.SensorFeatureCtrl(i, &SensorFeatureSetData, NULL, 0, NULL);
                break;
                case CAL_FEATURE_CAMERA_BANDING_FREQ:
                    SensorFeatureSetData.FeatureSetValue=IspPreviewConfigData.pCalIspPreviewPara->BandingFreq;
                    CustomDriverFunc.SensorFeatureCtrl(i, &SensorFeatureSetData, NULL, 0, NULL);
                break;
            } 
            
            IspImageSettingIndex &=~(1<<i);
        }
    }
}



/************************* Separate YUV part & switch case enum define ***************/
void IspOperationSetting(kal_uint8 Func, kal_uint16 Para)
{
    IMAGE_SENSOR_COMM_DATA_STRUCT SensorCommData;
    IMAGE_SENSOR_COMM_CTRL_STRUCT SensorCommCtrl;
#if (defined(AF_SUPPORT)||defined(LED_FLASHLIGHT_SUPPORT)||defined(LED_ONOFF_SUPPORT))
    CAL_FEATURE_CTRL_STRUCT FeatureParaIn;
#endif
//    extern SENSOR_AF_DELAY_STRUCT IspAFQueryDelay;
//    extern kal_bool   IspWaitAFResult;
   
    {
        SensorCommData.FeatureValue= Para;
        switch(Func)
        {
            case CAL_FEATURE_CAMERA_SCENE_MODE:
            /* CAM_AUTO_DSC, CAM_PORTRAIT, CAM_LANDSCAPE, CAM_SPORT, CAM_FLOWER, CAM_NIGHTSCENE,
            CAM_TV_MODE, CAM_AV_MODE, CAM_ISO_MODE. Each mode are EXCLUSIVE!! */
                IspPreviewConfigData.pCalIspPreviewPara->SceneMode= (CAL_CAMERA_SCENE_MODE_ENUM)Para;
                if ((Para== CAM_NIGHTSCENE) || (Para == CAM_VIDEO_NIGHT))					
                {
                    SensorCommCtrl.FeatureEnable=KAL_TRUE;
                }
                else
                {
                    SensorCommCtrl.FeatureEnable=KAL_FALSE;
                }
                CustomDriverFunc.SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_CTRL_NIGHTMODE, &SensorCommCtrl, NULL, 0, NULL);
            break;
            case CAL_FEATURE_CAMERA_AE_METERING_MODE:
            /* CAM_AE_METER_AUTO, CAM_AE_METER_SPOT, CAM_AE_METER_CENTRAL, CAM_AE_METER_AVERAGE for auto exposure */
                IspPreviewConfigData.pCalIspPreviewPara->AeMeteringMode= (CAL_CAMERA_AE_METERING_MODE_ENUM)Para;
                CustomDriverFunc.SensorFeatureCtrl(CAL_FEATURE_CAMERA_AE_METERING_MODE, &SensorCommData, NULL, 0, NULL);
            break;
        #if (defined(LED_FLASHLIGHT_SUPPORT)||defined(LED_ONOFF_SUPPORT))
            case CAL_FEATURE_CAMERA_FLASH_MODE:
            /* CAM_FLASH_OFF, CAM_FLASH_ON, CAM_FLASH_AUTO, CAM_FLASH_REDEYE */
                IspPreviewConfigData.pCalIspPreviewPara->FlashMode= (CAL_CAMERA_FLASH_ENUM)Para;
                FeatureParaIn.FeatureSetValue = Para;
                FeatureParaIn.FeatureId = CAL_FEATURE_CAMERA_FLASH_MODE;
                FeatureParaIn.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
                CustomDriverFunc.SensorFeatureCtrl(CAL_FEATURE_CAMERA_FLASH_MODE, &FeatureParaIn, NULL, 0, NULL);
            break;        
        #endif
            //case IMAGE_SENSOR_FEATURE_YUV_3A_CTRL:
                //CustomDriverFunc.SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_YUV_3A_CTRL, &SensorCommCtrl, NULL, 0, NULL);
            //break;        
        #if (defined(AF_SUPPORT))
            case CAL_FEATURE_CAMERA_AF_KEY:
                FeatureParaIn.FeatureSetValue = Para;
                FeatureParaIn.FeatureId = CAL_FEATURE_CAMERA_AF_KEY;
                FeatureParaIn.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
                CustomDriverFunc.SensorFeatureCtrl(CAL_FEATURE_CAMERA_AF_KEY, &FeatureParaIn, &IspAFQueryDelay, 0, NULL);
                if (CAM_AF_ONE_TIME == SensorCommData.FeatureValue || CAM_AF_CONTINUE == SensorCommData.FeatureValue)
                    IspWaitAFResult = KAL_TRUE;
                else
                    IspWaitAFResult = KAL_FALSE;
            break;
            case CAL_FEATURE_CAMERA_AF_RANGE_CONTROL:
                FeatureParaIn.FeatureSetValue = Para;
                FeatureParaIn.FeatureId = CAL_FEATURE_CAMERA_AF_RANGE_CONTROL;
                FeatureParaIn.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
                CustomDriverFunc.SensorFeatureCtrl(CAL_FEATURE_CAMERA_AF_RANGE_CONTROL, &FeatureParaIn, NULL, 0, NULL);
            break;
            case CAL_FEATURE_CAMERA_AF_OPERATION_MODE:        
                FeatureParaIn.FeatureSetValue = Para;
                FeatureParaIn.FeatureId = CAL_FEATURE_CAMERA_AF_OPERATION_MODE;
                FeatureParaIn.FeatureCtrlCode = CAL_FEATURE_SET_OPERATION;
                CustomDriverFunc.SensorFeatureCtrl(CAL_FEATURE_CAMERA_AF_OPERATION_MODE, &FeatureParaIn, NULL, 0, NULL);
            break;
        #endif
            default:
            break;
        }    
    }
}


/************************* Separate YUV part & switch case enum define ***************/
void IspSetImageSetting(kal_uint8 Type, kal_uint16 Para)
{
    CAL_FEATURE_CTRL_STRUCT SensorFeatureSetData;

    if((ISP_IDLE_STATE==IspOperationState)||(ISP_STANDBY_STATE==IspOperationState))
    {
        //ASSERT(0);
        return;
    }

    switch (Type)
    {
        case CAL_FEATURE_CAMERA_DIGITAL_ZOOM:
            //IspDigitalZoom((kal_uint8)Para);
        break;
        case CAL_FEATURE_CAMERA_CONTRAST:
            IspQualityTuningData.IspContrastLevel=(kal_uint8) Para;
        break;
        case CAL_FEATURE_CAMERA_SATURATION:
            IspQualityTuningData.IspSaturationLevel=(kal_uint8) Para;
        break;
        case CAL_FEATURE_CAMERA_SHARPNESS:
            IspQualityTuningData.IspSharpnessLevel=(kal_uint8) Para;
        break;
        case CAL_FEATURE_CAMERA_WB:        
            IspPreviewConfigData.pCalIspPreviewPara->WbMode=(CAL_CAMERA_WB_ENUM)Para;
        break;
        case CAL_FEATURE_CAMERA_EV_VALUE:
            IspPreviewConfigData.pCalIspPreviewPara->EvValue=(CAL_CAMERA_EV_ENUM)Para;
        break;
        case CAL_FEATURE_CAMERA_IMAGE_EFFECT:
            IspPreviewConfigData.pCalIspPreviewPara->ImageEffect=(CAL_CAMERA_IMAGE_EFFECT_ENUM)Para;
        break;
        case CAL_FEATURE_CAMERA_BANDING_FREQ:
            IspPreviewConfigData.pCalIspPreviewPara->BandingFreq=(CAL_CAMERA_BANDING_FREQ_ENUM)Para;
        break;
    }

    if (VIEW_FINDER_MODE_IS_ENABLE)
    {
        IspImageSettingIndex |= (1<<Type);
        return;
    }
    
    {
        //SensorCommData.FeatureValue= Para;
        SensorFeatureSetData.FeatureCtrlCode=CAL_FEATURE_SET_OPERATION;
        SensorFeatureSetData.FeatureSetValue=Para;
        switch (Type)
        {
            case CAL_FEATURE_CAMERA_CONTRAST:
            case CAL_FEATURE_CAMERA_SATURATION:
            case CAL_FEATURE_CAMERA_SHARPNESS:
            case CAL_FEATURE_CAMERA_WB:
            case CAL_FEATURE_CAMERA_EV_VALUE:
            case CAL_FEATURE_CAMERA_IMAGE_EFFECT:
            case CAL_FEATURE_CAMERA_BANDING_FREQ:
                //CustomDriverFunc.SensorFeatureCtrl(CAL_FEATURE_CAMERA_BANDING_FREQ, &SensorCommData, NULL, 0, NULL);
                CustomDriverFunc.SensorFeatureCtrl(Type, &SensorFeatureSetData, NULL, 0, NULL);
            break;
        }
    }
}

#if defined (__AUTO_SCENE_DETECT_SUPPORT__)
#if defined (YUV_MAIN_CAMERA)

//#define YUV_FACIALAE_DEBUG
#if defined (YUV_FACIALAE_DEBUG)
kal_uint32 FacialEvCnt =0;
#endif
kal_uint32 IspYUVSensorFacialAECalculate(kal_uint8 FaceLuminance)
{
    kal_uint32 Ev_value = CAM_EV_ZERO;
    kal_uint8 dev_face_mod;
    kal_int32 dev_face;

    kal_trace(TRACE_INFO,ISP_ASD_FACE_LUM, FaceLuminance);		        

    if(FaceLuminance<FACE_MEAN_LOWBOUND) /* small than lower bound case */    
    {
       /* gain let face increase to lower bound */
       dev_face = ((kal_int32) FaceLuminance - (kal_int32) FACE_MEAN_LOWBOUND)/METERING_FACE_EV_STEP_MEAN_ON_YUV_SENSOR;
    }
    else if(FaceLuminance>FACE_MEAN_HIGHBOUND) /* larger than upper bound case */
    {
        /* gain let face reduce to upper bound */
        dev_face = ((kal_int32)FaceLuminance - (kal_int32) FACE_MEAN_HIGHBOUND)/METERING_FACE_EV_STEP_MEAN_ON_YUV_SENSOR;
    }
    else /* inside bound */
    {
        dev_face = 0; /* no gain change */
    }

    if (dev_face >= 0)
    {
        dev_face_mod = (dev_face%10);

        dev_face = (dev_face/10);
        if (dev_face_mod>=5)
        {
         dev_face++;
        }
    }
    else
    {
        dev_face_mod = ((-dev_face)%10);    

        dev_face = (dev_face/10);
        if (dev_face_mod>=5)
        {
         dev_face--;
        }
    }

    switch (dev_face)
    {
        case -4: Ev_value = CAM_EV_POS_4_3; break;
        case -3: Ev_value = CAM_EV_POS_3_3; break;
        case -2: Ev_value = CAM_EV_POS_2_3; break;
        case -1: Ev_value = CAM_EV_POS_1_3; break;
        case  0: Ev_value = CAM_EV_ZERO; break;
        case  1: Ev_value = CAM_EV_NEG_1_3; break;
        case  2: Ev_value = CAM_EV_NEG_2_3; break;
        case  3: Ev_value = CAM_EV_NEG_3_3; break;
        case  4: Ev_value = CAM_EV_NEG_4_3; break;
        default:
           if (dev_face < -4) Ev_value = CAM_EV_POS_4_3;
           if (dev_face > 4) Ev_value = CAM_EV_NEG_4_3;
           break;
    }

    kal_trace(TRACE_INFO,ISP_ASD_FACE_COMPENSATION_EV, Ev_value);		           

#if defined (YUV_FACIALAE_DEBUG)
    FacialEvCnt++;
    if (FacialEvCnt%2)
    {
        Ev_value = CAM_EV_ZERO;
    }
    else
    {
        Ev_value = Ev_value;   
    }
    kal_trace(TRACE_INFO,ISP_ASD_FACE_COMPENSATION_EV_REAL, Ev_value);		          
#endif   

    return Ev_value;
}
#endif

void IspSetAsdCompensation(P_ISP_CAMERA_SCENE_COMPENSATION_STRUCT pIspSceneCompensation)
{
#if defined (YUV_MAIN_CAMERA)
    CAL_FEATURE_CTRL_STRUCT SensorFeatureSetData;
#endif

    if(ISP_IDLE_STATE==IspOperationState)
    {
        ASSERT(0);
    }

    IspAsdSceneType = pIspSceneCompensation->SceneType;
    IspAsdEnabled  = pIspSceneCompensation->AsdEnabled;

#if defined(YUV_MAIN_CAMERA)
    if(ISP_YUV_SUPPORT == IspInputDataFormat)
    {
       if (pIspSceneCompensation->FaceEVBiosEnable) // Face EV Bios only apply on YUV
       {
           IspAsdCompensationIndex |= (1<<ISP_ASD_COMP_FACE_EV_BIOS);
       }    
    }
#endif

    if (pIspSceneCompensation->NightModeEnable)
    {
        IspAsdCompensationIndex |= (1<<ISP_ASD_COMP_NIGHT_MODE_ON);
    }
    else
    {
        IspAsdCompensationIndex |= (1<<ISP_ASD_COMP_NIGHT_MODE_OFF);    
    }

    {

        if (pIspSceneCompensation->SceneContrast < CAM_CONTRAST_MAX)
        {
            IspQualityTuningData.IspContrastLevel = pIspSceneCompensation->SceneContrast;
            IspAsdCompensationIndex |= (1<<ISP_ASD_COMP_SCENE_ENHANCE);
        }

        if (pIspSceneCompensation->SceneSaturation < CAM_SATURATION_MAX)
        {
            IspQualityTuningData.IspSaturationLevel = pIspSceneCompensation->SceneSaturation;
            IspAsdCompensationIndex |= (1<<ISP_ASD_COMP_SCENE_ENHANCE);						
        }

        if (pIspSceneCompensation->SceneSharpness < CAM_SHARPNESS_MAX)
        {
            IspQualityTuningData.IspSharpnessLevel = pIspSceneCompensation->SceneSharpness;		
            IspAsdCompensationIndex |= (1<<ISP_ASD_COMP_SCENE_ENHANCE);			
        }
    }

    if (VIEW_FINDER_MODE_IS_ENABLE) // Apply in HISR
    {
        return;
    }

    //apply face ev bios
#if defined (YUV_MAIN_CAMERA)
    if (IspAsdCompensationIndex & (1<<ISP_ASD_COMP_FACE_EV_BIOS))
    {  
        // do compensation here, not in HISR (because it only apply in capture mode)
    #if (FACE_COMPENSATION_ON_YUV_SENSOR_ENABLE)        
        SensorFeatureSetData.FeatureCtrlCode= CAL_FEATURE_SET_OPERATION;
        SensorFeatureSetData.FeatureSetValue= IspYUVSensorFacialAECalculate(pIspSceneCompensation->FaceLuminance);
        CustomDriverFunc.SensorFeatureCtrl(CAL_FEATURE_CAMERA_EV_VALUE, &SensorFeatureSetData, NULL, 0, NULL);
    #endif
        IspAsdCompensationIndex &=~(1<<ISP_ASD_COMP_FACE_EV_BIOS);    
    }
#endif

    //enable night scene
    if (IspAsdCompensationIndex & (1<<ISP_ASD_COMP_NIGHT_MODE_ON))
    {
        if (IspAsdEnabled)
        {
            IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE,CAM_NIGHTSCENE);
        }
        else
        {
            IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE, IspAsdSceneType);
        }
        IspAsdCompensationIndex &=~(1<<ISP_ASD_COMP_NIGHT_MODE_ON);
    }

    //disable night scene
    if (IspAsdCompensationIndex & (1<<ISP_ASD_COMP_NIGHT_MODE_OFF))
    {
        if (IspAsdEnabled)
        {
             IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE, CAM_AUTO_DSC);
        }
        else
        {
             IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE, IspAsdSceneType);
        }
        IspAsdCompensationIndex &=~(1<<ISP_ASD_COMP_NIGHT_MODE_OFF);
    }

    // apply contrast/saturation/sharpness	
    if (IspAsdCompensationIndex & (1<<ISP_ASD_COMP_SCENE_ENHANCE))
    {
        SENSOR_SCENE_ENHANCE_STRUCT Sensor_Scene_Enhance;

        Sensor_Scene_Enhance.SceneContrast = IspQualityTuningData.IspContrastLevel;
        Sensor_Scene_Enhance.SceneSaturation = IspQualityTuningData.IspSaturationLevel;
        Sensor_Scene_Enhance.SceneSharpness = IspQualityTuningData.IspSharpnessLevel;

        CustomDriverFunc.SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_SET_SCENE_ENHANCE, &Sensor_Scene_Enhance, NULL, 0, NULL);
        IspAsdCompensationIndex &=~(1<<ISP_ASD_COMP_SCENE_ENHANCE);
    }	
}


void IspBackgndSetAsdCompensation(void)
{
    if((ISP_IDLE_STATE==IspOperationState)||(ISP_STANDBY_STATE==IspOperationState))
    {
        ASSERT(0);
    }

    // enable night scene
    if (IspAsdCompensationIndex & (1<<ISP_ASD_COMP_NIGHT_MODE_ON))
    {
        if (IspAsdEnabled)
        {
            IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE, CAM_NIGHTSCENE);
        }
        else
        {
            IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE, IspAsdSceneType);
        }
        IspAsdCompensationIndex &=~(1<<ISP_ASD_COMP_NIGHT_MODE_ON);
    }

    // disable night scene
    if (IspAsdCompensationIndex & (1<<ISP_ASD_COMP_NIGHT_MODE_OFF))
    {
        if (IspAsdEnabled)
        {
            IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE, CAM_AUTO_DSC);
        }
        else
        {
            IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE, IspAsdSceneType);
        }
        IspAsdCompensationIndex &=~(1<<ISP_ASD_COMP_NIGHT_MODE_OFF);
    }

    // apply contrast/saturation/sharpness
    if (IspAsdCompensationIndex & (1<<ISP_ASD_COMP_SCENE_ENHANCE))
    {
        SENSOR_SCENE_ENHANCE_STRUCT Sensor_Scene_Enhance;

        Sensor_Scene_Enhance.SceneContrast = IspQualityTuningData.IspContrastLevel;
        Sensor_Scene_Enhance.SceneSaturation = IspQualityTuningData.IspSaturationLevel;
        Sensor_Scene_Enhance.SceneSharpness = IspQualityTuningData.IspSharpnessLevel;

        CustomDriverFunc.SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_SET_SCENE_ENHANCE, &Sensor_Scene_Enhance, NULL, 0, NULL);
        IspAsdCompensationIndex &=~(1<<ISP_ASD_COMP_SCENE_ENHANCE);
    }
}
#endif



/**************** naming, 6253 yuv difference ******************/
void IspStartInput(ISP_OPERATION_STATE_ENUM IspState)
{
#if (defined(ISP_SUPPORT))

#if (defined(__ISP_CRZ_HW_FRAME_SYNC_SUPPORT__))
    ENABLE_CAMERA_TO_CRZ_INIT;	/*Enable hardware frame reset*/
    ENABLE_CAMERA_RESIZER_OVERRUN_INT;
#endif

    /******* confirm if necessary ********/
    ENABLE_CAMERA_RESIZER_OVERRUN_INT;
    ENABLE_CAMERA_GMC_OVERRUN_INT;
    
    switch(IspState)
    {
        case ISP_PREVIEW_STATE:
    #if defined(WEBCAM_SUPPORT)
        case ISP_PREVIEW_WEBCAM_STATE:
    #endif
        case ISP_MPEG4_ENCODE_STATE:
            WaitFirstFrameFlag=KAL_TRUE;
            IspPreviewFrameCount=0;
            SET_CAMERA_PREVIEW_MODE;
            SET_CAMERA_FRAME_RATE(0);
            ENABLE_VIEW_FINDER_MODE;
        break;
        case ISP_CAPTURE_JPEG_STATE:         
        #if(defined(__ISP_AUTO_RECAPTURE_SUPPORT__))
        #if (!(defined(LED_FLASHLIGHT_SUPPORT) ||defined(MSHUTTER_SUPPORT) ))
            ENABLE_REZ_OVRUN_FLIMIT_EN;
            SET_REZ_OVRUN_FLIMIT_NO(CAPTURE_RETRIAL_FRAME_NO);//recapture times
        #endif      
        #endif
            IspIdleDoneFlag=KAL_FALSE;
            IspDoneFlag=KAL_FALSE;
            ENABLE_CAMERA_ISP_DONE_INT;
            ENABLE_CAMERA_IDLE_INT;
            SET_CAMERA_CAPTURE_MODE;
            ENABLE_VIEW_FINDER_MODE;
        break;
    #if defined(WEBCAM_SUPPORT) 
        case ISP_CAPTURE_WEBCAM_STATE:
            IspIdleDoneFlag=KAL_FALSE;
            IspDoneFlag=KAL_FALSE;
            ENABLE_CAMERA_ISP_DONE_INT;
            ENABLE_CAMERA_IDLE_INT;
            SET_CAMERA_CAPTURE_MODE;
            ENABLE_VIEW_FINDER_MODE;
        break;
    #endif
        default: 
        break;
    }

    DISABLE_REZ_DISCONN;
    
#endif    
}

kal_bool IspWaitIspDone(ISP_OPERATION_STATE_ENUM IspState, kal_uint16 loop, kal_uint16 unit)
{
    volatile kal_uint32 i=0;
    while(KAL_FALSE==IspDoneFlag)
    {
        i++;
        vTaskDelay(unit);
        if(i>loop) break;
    }
    if(i>loop)
        return KAL_FALSE;
    else
        return KAL_TRUE;
}


kal_bool IspWaitIspIdle(ISP_OPERATION_STATE_ENUM IspState, kal_uint16 loop, kal_uint16 unit)
{
    volatile kal_uint32 i=0;

    while(KAL_FALSE==IspIdleDoneFlag)
    {
        i++;
        vTaskDelay(unit);
        if(i>loop) break;
    }
    if(i>loop)
        return KAL_FALSE;
    else
        return KAL_TRUE;

}

void IspStopInput(ISP_OPERATION_STATE_ENUM IspState)
{
#if (defined(ISP_SUPPORT))
    volatile kal_uint32 j=0;

    /******* confirm if necessary ********/
    ENABLE_CAMERA_RESIZER_OVERRUN_INT;
    ENABLE_CAMERA_GMC_OVERRUN_INT;

    switch(IspState)
    {
        case ISP_PREVIEW_STATE:
    #if defined(WEBCAM_SUPPORT)
        case ISP_PREVIEW_WEBCAM_STATE:
    #endif
        case ISP_MPEG4_ENCODE_STATE:
            IspIdleDoneFlag=KAL_FALSE;
            ENABLE_CAMERA_IDLE_INT;
            DISABLE_VIEW_FINDER_MODE;
            IspWaitIspIdle(IspState,9 , 5);
        break;
        case ISP_CAPTURE_JPEG_STATE:
            if(IspGlobalInfo.ZoomStep == 0)
                IspWaitIspDone(IspState,100 , 2);
        
            DISABLE_CAMERA_ISP_DONE_INT;
            DISABLE_VIEW_FINDER_MODE;
            
            if(IspGlobalInfo.ZoomStep == 0)
                IspWaitIspIdle(IspState,100 , 2);
 
            DISABLE_CAMERA_IDLE_INT;
        break;
    #if defined(WEBCAM_SUPPORT) 
        case ISP_CAPTURE_WEBCAM_STATE:
            IspWaitIspDone(IspState,100 , 2);
            DISABLE_CAMERA_ISP_DONE_INT;
            DISABLE_VIEW_FINDER_MODE;

            IspWaitIspIdle(IspState,100 , 2);
            DISABLE_CAMERA_IDLE_INT;
        break;
    #endif
        default:
        break;
    }
    
#if defined(WEBCAM_SUPPORT) 
    if (IspState == ISP_PREVIEW_WEBCAM_STATE || IspState == ISP_CAPTURE_WEBCAM_STATE)
    {
        ENABLE_REZ_DISCONN;
    }
    else
#endif
    {
        RESET_ISP;	 //flash modify
        for(j=0;j<0x80;j++); //flash modify
        CLEAR_RESET_ISP; //flash modify
    }

#if (defined(__ISP_CRZ_HW_FRAME_SYNC_SUPPORT__))
    DISABLE_CAMERA_TO_CRZ_INIT;	/*Enable hardware frame reset*/
    DISABLE_CAMERA_RESIZER_OVERRUN_INT;
#endif
    IspVsyncTgCnt =0 ; // set to 0 
    IspFrameReadyCnt=0; // set to 0 

#endif
}  /* stop_isp() */

void IspDisableViewFinder(void)
{
    IspIdleDoneFlag=KAL_TRUE;
    IspDoneFlag=KAL_TRUE;
    DISABLE_VIEW_FINDER_MODE;
}

void IspCompensationConfig(ISP_OPERATION_STATE_ENUM IspState, kal_uint32 IntMemAddr, kal_uint32 CurrentWidth, IMAGE_SENSOR_MIRROR_ENUM ImageMirror )
{
#if (defined(ISP_SUPPORT))
#endif
}  /* isp_compensation_config() */


kal_bool IspDropFrame(void)
{
#if (defined(ISP_SUPPORT))
    return KAL_TRUE;
#endif
}

void IspEnableOverrunInt(void) 
{
    ENABLE_CAMERA_RESIZER_OVERRUN_INT;
    ENABLE_CAMERA_GMC_OVERRUN_INT;
}

void IspDisableOverrunInt(void) 
{
    DISABLE_CAMERA_RESIZER_OVERRUN_INT;
    DISABLE_CAMERA_GMC_OVERRUN_INT;
}


static void _IspVsyncTimeOutReportHandler(void *parameter)
{
#if 0
    if(IspVsyncGptHandle != NULL)
    {
        kal_set_eg_events(IspEventId,CAMERA_ISP_VD_READY_EVENT,KAL_OR);
        IspVsyncGptTimeOut = KAL_TRUE; 	
    }
#endif
}

static void _IspVsyncTimeOutReportStart(ISP_VSYNC_TIME_OUT_UNIT TimeUnit)
{
#if 0
    SGPT_CTRL_START_T start;
    
    if (IspVsyncGptHandle == NULL)
    {
        //GPTI_GetHandle(&IspVsyncGptHandle);
        IspVsyncGptHandle = DclSGPT_Open(DCL_GPT_CB,0);
        if (IspVsyncGptHandle!=NULL)
        {
            //GPTI_StartItem(IspVsyncGptHandle, TimeUnit,   
            //_IspVsyncTimeOutReportHandler, NULL);
            start.u2Tick=TimeUnit;
            start.pfCallback=_IspVsyncTimeOutReportHandler;
            start.vPara=NULL;
            DclSGPT_Control(IspVsyncGptHandle,SGPT_CMD_START,(DCL_CTRL_DATA_T*)&start); 				
        }
    }
    IspVsyncGptTimeOut = KAL_FALSE; 
#endif
}


static void _IspVsyncTimeOutReportRelease(void)
{
#if 0
    if(IspVsyncGptHandle != NULL)
    {
        //GPTI_StopItem(IspVsyncGptHandle);
        DclSGPT_Control(IspVsyncGptHandle,SGPT_CMD_STOP,0);
        //GPTI_ReleaseHandle(&IspVsyncGptHandle);
        DclSGPT_Close(&IspVsyncGptHandle);
        IspVsyncGptHandle = NULL;
    }
    IspVsyncGptTimeOut = KAL_TRUE;	
#endif
}


void IspWaitOneVsync(ISP_VSYNC_TIME_OUT_UNIT TimeOut)
{
#if 0
    kal_uint32 EventGroup;
    if(TimeOut ==0 || TimeOut > IspVsyncTimeOut500MS)
        TimeOut = IspVsyncTimeOut500MS;
    
    _IspVsyncTimeOutReportStart(TimeOut);
    kal_set_eg_events(IspEventId,0,KAL_AND);
    kal_retrieve_eg_events(IspEventId,CAMERA_ISP_VD_READY_EVENT,KAL_OR_CONSUME,&EventGroup,KAL_SUSPEND);
    _IspVsyncTimeOutReportRelease();
#endif

    kal_uint8 TOcount = 0;

    while(!IspVDReadyEvent && TOcount < 10) {
        vTaskDelay(TimeOut/portTICK_PERIOD_MS);
        TOcount++;
    }
    IspVDReadyEvent = KAL_FALSE;

}

void IspWaitNVsync(ISP_VSYNC_TIME_OUT_UNIT TimeOut, kal_int32 Num)
{
#if 0
    kal_uint32 EventGroup;
    if(TimeOut ==0 || TimeOut > IspVsyncTimeOut500MS)
        TimeOut = IspVsyncTimeOut500MS;

    if(Num == 0)
        return;

    while(Num>0)
    {
        _IspVsyncTimeOutReportStart(TimeOut);
        kal_set_eg_events(IspEventId,0,KAL_AND);
        kal_retrieve_eg_events(IspEventId,CAMERA_ISP_VD_READY_EVENT,KAL_OR_CONSUME,&EventGroup,KAL_SUSPEND);
        _IspVsyncTimeOutReportRelease();		
        Num--;
    }
#endif

    vTaskDelay(10);

}

static void _IspFrameDoneTimeOutReportHandler(void *parameter)
{
#if 0
    if(IspFrameDoneGptHandle != NULL)
    {
        kal_set_eg_events(IspEventId,CAMERA_ISP_FRAME_READY_EVENT,KAL_OR);
        IspFrameDoneGptTimeOut = KAL_TRUE; 	
    }
#endif
}

static void _IspFrameDoneTimeOutReportStart(ISP_VSYNC_TIME_OUT_UNIT TimeUnit)
{
#if 0
    SGPT_CTRL_START_T start;
    
    if (IspFrameDoneGptHandle == NULL)
    {
        //GPTI_GetHandle(&IspVsyncGptHandle);
        IspFrameDoneGptHandle = DclSGPT_Open(DCL_GPT_CB,0);
        if (IspFrameDoneGptHandle!=NULL)
        {
            //GPTI_StartItem(IspVsyncGptHandle, TimeUnit,   
            //_IspVsyncTimeOutReportHandler, NULL);
            start.u2Tick=TimeUnit;
            start.pfCallback=_IspFrameDoneTimeOutReportHandler;
            start.vPara=NULL;
            DclSGPT_Control(IspFrameDoneGptHandle,SGPT_CMD_START,(DCL_CTRL_DATA_T*)&start); 				
        }
    }
    IspFrameDoneGptTimeOut = KAL_FALSE; 
#endif
}


static void _IspFrameDoneTimeOutReportRelease(void)
{
#if 0
    if(IspFrameDoneGptHandle != NULL)
    {
        //GPTI_StopItem(IspVsyncGptHandle);
        DclSGPT_Control(IspFrameDoneGptHandle,SGPT_CMD_STOP,0);
        //GPTI_ReleaseHandle(&IspVsyncGptHandle);
        DclSGPT_Close(&IspFrameDoneGptHandle);
        IspFrameDoneGptHandle = NULL;
    }
    IspFrameDoneGptTimeOut = KAL_TRUE;	
#endif
}

void IspWaitOneFrameDone(ISP_VSYNC_TIME_OUT_UNIT TimeOut)
{
#if 0
    kal_uint32 EventGroup;
    if(TimeOut ==0 || TimeOut > IspVsyncTimeOut500MS)
        TimeOut = IspVsyncTimeOut500MS;
    
    _IspFrameDoneTimeOutReportStart(TimeOut);
    kal_set_eg_events(IspEventId,0,KAL_AND);
    kal_retrieve_eg_events(IspEventId,CAMERA_ISP_FRAME_READY_EVENT,KAL_OR_CONSUME,&EventGroup,KAL_SUSPEND);
    _IspFrameDoneTimeOutReportRelease();		
#endif
    kal_uint8 TOcount = 0;

    while(!IspFrameDoneEvent && TOcount < 10) {
        vTaskDelay(TimeOut/portTICK_PERIOD_MS);
        TOcount++;
    }

    IspFrameDoneEvent = KAL_FALSE;
}


void IspWaitSensorStableDelayFrame(void)
{
#if (defined(ISP_SUPPORT))
    kal_uint32 i=0;

    if(IspFirstPreviewFlag)
    {
        IspFirstPreviewFlag = KAL_FALSE;
        for(i=1;i<(CameraDriverOperParaBuffer.preview_display_wait_frame +IspGlobalInfo.WaitStableFrameNum);i++)		
        {
            IspWaitOneVsync(IspVsyncTimeOut100MS);
        }
    }
    else
    {
        for(i=1;i<(CameraDriverOperParaBuffer.preview_delay_frame +IspGlobalInfo.WaitStableFrameNum);i++)		
        {
            IspWaitOneVsync(IspVsyncTimeOut100MS);
        }
    }
#endif
}



void IspWaitCaptureDelayFrame(void)
{
#if (defined(ISP_SUPPORT))
    kal_uint32 i=0;
#if defined(MIPI_SENSOR_SUPPORT)
    kal_bool HaveTriedResetCsi2=KAL_FALSE;
#endif

    for(i=1;i<(CameraDriverOperParaBuffer.capture_delay_frame+IspGlobalInfo.WaitStableFrameNum);i++)		
    {
        IspWaitOneVsync(IspVsyncTimeOut100MS);
    }
#endif
}   /* wait_sensor_capture_stable_frame() */

void IspReset(void)
{
    kal_uint32 j;

    RESET_ISP;
    for(j=0;j<0x80;j++)
        ;
    
    CLEAR_RESET_ISP;
}


/***********temp ***********************/
kal_uint32 IspEmbedMtkInfoIsp(kal_uint32 *addr)
{
    return 0;
}

kal_uint32 CameraEmbedMtkInfo(kal_uint32 *addr)
{
    return 0;
}

void IspQueryProcessDataBitDepth(P_ISP_PROCESS_DATA_BIT_DEPTH_STRUCT pIspDataBitDepth)
{
}


void IspSetCamera3AEnable(kal_bool Flag)
{
    IMAGE_SENSOR_COMM_DATA_STRUCT SensorCommData;
    
    {
        SensorCommData.FeatureValue=Flag;
        CustomDriverFunc.SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_SET_AE_BYPASS, &SensorCommData, NULL, 0, NULL);
        CustomDriverFunc.SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_SET_AWB_BYPASS, &SensorCommData, NULL, 0, NULL);
    }
}


/****************************************************************************
 *          EXIF         *
 ****************************************************************************/

#if (defined(EXIF_SUPPORT))
void exif_update_MTK_para_struct(exif_MTK_para_struct* para)
{
#if(defined(__PANORAMA_VIEW_SUPPORT__))
    //exif_update_MTK_para_struct_PANO(mtk_para_data);
#endif
}



void ExifUpdateCamParaStruct(exif_cam_para_struct *pExifPara)
{
    kal_uint8 meteringmode=0,lightsource=0,scenetype=0;

    /********    RAW/YUV common part    *********************************************/
    /* metering mode setting */
    if(CAM_NO_OF_AE_METERING > IspPreviewConfigData.pCalIspPreviewPara->AeMeteringMode)
    {
        meteringmode= IspAeMeteringModeTbl[IspPreviewConfigData.pCalIspPreviewPara->AeMeteringMode] ;
    }
    else
    {
        meteringmode=0;
    }
    /* awb setting */
    if(CAM_NO_OF_WB > IspPreviewConfigData.pCalIspPreviewPara->WbMode)
    {
        lightsource = IspLightSourceModeTbl[IspPreviewConfigData.pCalIspPreviewPara->WbMode];
    }
    else
    {
        lightsource=0;
    }
    
#if defined (__AUTO_SCENE_DETECT_SUPPORT__)
    if(CAM_AUTO_DSC == IspAsdSceneType)
    {
        scenetype=0;
    }
    else if(CAM_LANDSCAPE == IspAsdSceneType)
    {
        scenetype=1;
    }
    else if(CAM_PORTRAIT == IspAsdSceneType)
    {
        scenetype=2;
    }
    else if(CAM_NIGHTSCENE == IspAsdSceneType)
    {
        scenetype=3;
    }
    else
    {
        scenetype=0;
    }
#else
    if(CAM_AUTO_DSC == IspPreviewConfigData.pCalIspPreviewPara->SceneMode)
    {
        scenetype=0;
    }
    else if(CAM_LANDSCAPE == IspPreviewConfigData.pCalIspPreviewPara->SceneMode)
    {
        scenetype=1;
    }
    else if(CAM_PORTRAIT == IspPreviewConfigData.pCalIspPreviewPara->SceneMode)
    {
        scenetype=2;
    }
    else if(CAM_NIGHTSCENE == IspPreviewConfigData.pCalIspPreviewPara->SceneMode)
    {
        scenetype=3;
    }
    else
    {
        scenetype=0;
    }
#endif
    
    pExifPara->FNumber[0] = 28;
    pExifPara->FNumber[1] = 10;
    pExifPara->ExposureProgram = 2;
    pExifPara->PixelXDimension = ExifIspTargetWidth;/* global variable */
    pExifPara->PixelYDimension = ExifIspTargetHeight;/* global variable */
    pExifPara->DigitalZoomRatio[0] = IspGetDigitalZoomFactor();/* global variable */
    pExifPara->DigitalZoomRatio[1] = 100;
    pExifPara->SceneCaptureType=scenetype;
    pExifPara->MeteringMode = meteringmode;
    pExifPara->LightSource = lightsource;
    pExifPara->Orientation = ExifOrientation;
    
    /********    RAW/YUV different part    *********************************************/
    {
    /* Exif IFD default values */
    pExifPara->ExposureBiasValue[0] = 5*((kal_int8)IspPreviewConfigData.pCalIspPreviewPara->EvValue- 4);
    pExifPara->ExposureBiasValue[1] = 10;
    pExifPara->ISOSpeedRatings = 100;

    pExifPara->Flash = 0;
#ifdef LED_FLASHLIGHT_SUPPORT
    {
        if(IspPreviewConfigData.pCalIspPreviewPara->FlashMode == CAM_FLASH_REDEYE)
            pExifPara->Flash = 0x58 ;
        else if(IspPreviewConfigData.pCalIspPreviewPara->FlashMode == CAM_FLASH_AUTO)
            pExifPara->Flash = 0x18 ;
        else if(IspPreviewConfigData.pCalIspPreviewPara->FlashMode == CAM_FLASH_ON)
            pExifPara->Flash = 0x08 ;
        else if(IspPreviewConfigData.pCalIspPreviewPara->FlashMode == CAM_FLASH_OFF)
            pExifPara->Flash = 0x10 ;

        {
            extern kal_bool IspYUVFlashlightTriggerEnable ; 
            pExifPara->Flash |= IspYUVFlashlightTriggerEnable;
        }
                
        ///for MAUI_02938103
        if(pExifPara->Flash == 0x58)
        {
            pExifPara->Flash = 0x18;
        }
    }
#endif
}

}

#endif


#if (defined(__ISP_MIPI_SENSOR_SUPPORT__))
void MipiInterfaceInit()
{
    ///Bin added for MIPI AVDD power leakage
    //(*((volatile unsigned int *)(PLL_base + 0x0B08))) &= 0xFFFE; //PLL_EN
    (*((volatile unsigned int *)(PLL_base + 0x0B08))) &= 0xFFFE; //LNT_BGR_EN
}
#endif

#ifdef MIPI_SENSOR_SUPPORT
kal_bool MipiInterfaceEnable(kal_bool Enable, P_IMAGE_SENSOR_MIPI_PARA_IN_STRUCT pMipiParaIn)
{
    if(Enable)
    {
        kal_uint32 SensorOutLen;
        IMAGE_SENSOR_MIPI_PARA_OUT_STRUCT MipiParaOut;

        ASSERT(pMipiParaIn != NULL);

        // turn off CSI2 FIRST
        DISABLE_CSI2_ECC;
        DISABLE_CSI2;

        pfImageSensor->SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_GET_MIPI_INFO, pMipiParaIn,   
            (void *) &MipiParaOut, sizeof(IMAGE_SENSOR_MIPI_PARA_OUT_STRUCT), &SensorOutLen);        

        DISABLE_CSI2_HS_SOL2;		///always disable SOL2 due to MAUI_02854461

        SET_LP2HS_DELAY_COUNT(MipiParaOut.LowPwr2HighSpeedDelayCount);

        ENABLE_CSI2_ECC;
        ENABLE_CSI2;

        if(1==MipiParaOut.MipiLaneNum)
        {
            DISABLE_DLANE2;
        }
        else if (2==MipiParaOut.MipiLaneNum)
        {
            ENABLE_DLANE2;
        }

    }
    else
    {
        DISABLE_CSI2_ECC;
        DISABLE_CSI2;
    }

    return KAL_TRUE;

}
#endif

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif


