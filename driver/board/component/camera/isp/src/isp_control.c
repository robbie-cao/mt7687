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
#include "cal_task_msg_if.h"

#include "isp_comm_def.h"

#include "isp_if.h"

//#include "exif.h"
//#include "emi_sw.h" // for EMI_SetGMCBW();
//#include "aaa_process.h"
//#include "mm_intmem.h"
//#include "isp_flashlight_if.h"
//#include "aaa_ae.h"
//#include "kal_trace.h"
#include "isp_mem.h"
#include "stdio.h"
#include "string.h"

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code = "DYNAMIC_CODE_CAMCAL_ROCODE", rodata = "DYNAMIC_CODE_CAMCAL_ROCODE"
#endif

ISP_CAMERA_PREVIEW_STRUCT IspPreviewConfigData;
CAL_ISP_PREVIEW_STRUCT CalIspPreviewData;
ISP_QUALITY_TUNINIG_LEVEL_STRUCT IspQualityTuningData;

ISP_CAMERA_VIDEO_STRUCT IspVideoConfigData;
ISP_CAMERA_STILL_CAPTURE_STRUCT IspCaptureConfigData;
CAL_ISP_STILL_CAPTURE_STRUCT CalIspCaptureData;
ISP_FEATURE_GET_OUTPUT_RESOLUTION_OUT_STRUCT OutputResolution;

IMAGE_SENSOR_MIPI_PARA_IN_STRUCT MipiInputData;
IMAGE_SENSOR_MIPI_PARA_OUT_STRUCT MipiOutputData;
    
kal_uint32 IspProcessStage;
kal_uint32 IspFrameCount;

kal_uint16 ExifIspTargetWidth=0, ExifIspTargetHeight=0;
kal_uint16 ExifOrientation = (kal_uint16) MM_IMAGE_ROTATE_0;

kal_bool IspDigitalZoomFlag;
MULTISHOT_INFO_ENUM IspMultiShotInfo = MULTISHOT_FALSE;

#if defined(__JPEG_SENSOR_SUPPORT__)
extern JPEG_SENSOR_SRC_BUFFER_STRUCT JpegSensorParaIn;
extern JPEG_SENSOR_PARSE_DATA_STRUCT JpegSensorParaOut;
#endif

#if defined(__ISP_SERIAL_SENSOR_SUPPORT__)
extern void IspSerialInfSetSize(P_SERIAL_SENSOR_INFO_OUT_STRUCT para, kal_uint32 check_code);
#endif

MM_ERROR_CODE_ENUM IspYuvPreview(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen)
{

    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    P_ISP_CAMERA_PREVIEW_STRUCT pIspPreviewPara=(P_ISP_CAMERA_PREVIEW_STRUCT)pCtrlPara;
    P_CAL_ISP_PREVIEW_STRUCT pCalIspPreviewPara=pIspPreviewPara->pCalIspPreviewPara;
#if defined(AF_SUPPORT)
    extern kal_bool IspAFkeyRelease;
#endif	
#if  defined(__SERIAL_SENSOR_V1_SUPPORT__)
    SERIAL_SENSOR_INFO_IN_STRUCT SerialSensorInfoIn;
    SERIAL_SENSOR_INFO_OUT_STRUCT SerialSensorInfoOut;
	
	memset(&SerialSensorInfoOut, 0, sizeof(SerialSensorInfoOut));
#endif


    if(CAL_CTRL_CODE_STOP & CtrlCode)
    {
        /* update digital zoom control flag */
        if (IspOperationState!=ISP_PREVIEW_STATE)
        {
            //ASSERT(0);
            configASSERT(0);
        }

        /* stop isp engine */
        IspStopInput(ISP_PREVIEW_STATE);

        IspDigitalZoomFlag=KAL_FALSE;

        /* stop 3a algorithm */
        //AaaConfigExitPreview();

        /* update isp operation state */
        IspOperationState=ISP_STANDBY_STATE;

        IspProcessStage|=LEAVE_EXIT_CAMERA_PREVIEW;

 
    }
    else if(CAL_CTRL_CODE_START & CtrlCode)
    {
        memcpy(&IspPreviewConfigData, pIspPreviewPara, sizeof(ISP_CAMERA_PREVIEW_STRUCT));
        memcpy(&CalIspPreviewData, pCalIspPreviewPara, sizeof(CAL_ISP_PREVIEW_STRUCT));
        IspPreviewConfigData.pCalIspPreviewPara = &CalIspPreviewData;

    #if  defined(__SERIAL_SENSOR_V1_SUPPORT__)
        if(CurrentSensorInfo.SensorIf == IMAGE_SENSOR_IF_SERIAL) 
        {
            SerialSensorInfoIn.ScenarioId = CAL_SCENARIO_CAMERA_PREVIEW;
            ErrorCode = pfImageSensor->SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_GET_SERIAL_SENSOR_INFO,  (void *) &SerialSensorInfoIn,   
                                         (void *) &SerialSensorInfoOut, sizeof(SERIAL_SENSOR_INFO_OUT_STRUCT), &SensorFeatureParaOutLen);        
            if(MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT != ErrorCode)
            {
                IspSerialInfSetSize(&SerialSensorInfoOut , 0x00);
            }
        }
    #endif

        IspOperationState = ISP_PREVIEW_STATE;

        #if defined(LED_FLASHLIGHT_SUPPORT)
        IspYuvFlashlightRestore3AProcess();
        #endif
        
    #if(defined(AF_SUPPORT))
        if(ISP_YUV_SUPPORT == IspInputDataFormat)
        {
            if (KAL_FALSE == IspAFkeyRelease)
            {
                IspOperationSetting(CAL_FEATURE_CAMERA_AF_KEY, CAM_AF_RELEASE);
            }
        }
    #endif
        IspOperationSetting(CAL_FEATURE_CAMERA_AE_METERING_MODE,IspPreviewConfigData.pCalIspPreviewPara->AeMeteringMode);
    #if (defined(AF_SUPPORT))
        IspOperationSetting(CAL_FEATURE_CAMERA_AF_OPERATION_MODE,IspPreviewConfigData.pCalIspPreviewPara->AfOperationMode);
        IspOperationSetting(CAL_FEATURE_CAMERA_AF_RANGE_CONTROL,IspPreviewConfigData.pCalIspPreviewPara->AfRange);
    #endif
    #if (defined(LED_FLASHLIGHT_SUPPORT))
        IspOperationSetting(CAL_FEATURE_CAMERA_FLASH_MODE,IspPreviewConfigData.pCalIspPreviewPara->FlashMode);
    #endif
        IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE,IspPreviewConfigData.pCalIspPreviewPara->SceneMode);
        
        IspSetImageSetting(CAL_FEATURE_CAMERA_WB, IspPreviewConfigData.pCalIspPreviewPara->WbMode);
        IspSetImageSetting(CAL_FEATURE_CAMERA_EV_VALUE, IspPreviewConfigData.pCalIspPreviewPara->EvValue);
        IspSetImageSetting(CAL_FEATURE_CAMERA_BANDING_FREQ, IspPreviewConfigData.pCalIspPreviewPara->BandingFreq);    
        IspSetImageSetting(CAL_FEATURE_CAMERA_IMAGE_EFFECT, IspPreviewConfigData.pCalIspPreviewPara->ImageEffect);    

        IspSetImageSetting(CAL_FEATURE_CAMERA_HUE, IspPreviewConfigData.pCalIspPreviewPara->HueValue);
        IspSetImageSetting(CAL_FEATURE_CAMERA_CONTRAST, IspPreviewConfigData.pCalIspPreviewPara->ContrastLevel);
        IspSetImageSetting(CAL_FEATURE_CAMERA_SATURATION, IspPreviewConfigData.pCalIspPreviewPara->SaturationLevel);
        IspSetImageSetting(CAL_FEATURE_CAMERA_SHARPNESS, IspPreviewConfigData.pCalIspPreviewPara->SharpnessLevel);
        IspSetImageSetting(CAL_FEATURE_CAMERA_BRIGHTNESS, IspPreviewConfigData.pCalIspPreviewPara->BrightnessLevel);

        IspWaitSensorStableDelayFrame();
        IspStartInput(IspOperationState);
        IspWaitOneFrameDone(IspVsyncTimeOut250MS);// wait one frame done

        LOG_I(hal, "0xA0420004=%x, 0xA0420008=%x, 0xA042000C=%x\r\n", *((volatile unsigned int *)0xA0420004), *((volatile unsigned int *)0xA0420008), *((volatile unsigned int *)0xA042000C));

        IspProcessStage|=LEAVE_CAMERA_PREVIEW;
    }
    return ErrorCode;
}

MM_ERROR_CODE_ENUM IspYuvVideo(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode,
                                  void *pCtrlPara, kal_uint16 CtrlParaLen)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    P_ISP_CAMERA_VIDEO_STRUCT pIspVideoPara=(P_ISP_CAMERA_VIDEO_STRUCT)pCtrlPara;
    P_CAL_ISP_VIDEO_STRUCT pCalIspVideoPara=pIspVideoPara->pCalIspVideoPara;
#if  defined(__SERIAL_SENSOR_V1_SUPPORT__)
    SERIAL_SENSOR_INFO_IN_STRUCT SerialSensorInfoIn;
    SERIAL_SENSOR_INFO_OUT_STRUCT SerialSensorInfoOut;
	memset(&SerialSensorInfoOut, 0, sizeof(SerialSensorInfoOut));
#endif


    if (CAL_CTRL_CODE_STOP & CtrlCode)
    {
        if (IspOperationState!=ISP_MPEG4_ENCODE_STATE)
            configASSERT(0);//ASSERT(0);

        /* stop isp engine */
        /* YUV interface */
        IspStopInput(ISP_MPEG4_ENCODE_STATE);

        /* update digital zoom control flag */
        IspDigitalZoomFlag=KAL_FALSE;

        /* update isp operation state */
        IspOperationState=ISP_STANDBY_STATE;

        IspProcessStage|=LEAVE_EXIT_MPEG4_ENCODE;
    }
    else if (CAL_CTRL_CODE_START & CtrlCode)
    {
        memcpy(&IspVideoConfigData, pIspVideoPara, sizeof(ISP_CAMERA_VIDEO_STRUCT));
    #if  defined(__SERIAL_SENSOR_V1_SUPPORT__)
        if(CurrentSensorInfo.SensorIf == IMAGE_SENSOR_IF_SERIAL) 
        {
            SerialSensorInfoIn.ScenarioId = CAL_SCENARIO_CAMERA_PREVIEW;
            ErrorCode = pfImageSensor->SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_GET_SERIAL_SENSOR_INFO,  (void *) &SerialSensorInfoIn,   
                                         (void *) &SerialSensorInfoOut, sizeof(SERIAL_SENSOR_INFO_OUT_STRUCT), &SensorFeatureParaOutLen);        
            if(MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT != ErrorCode)
            {
                IspSerialInfSetSize(&SerialSensorInfoOut , 0x00);
            }
        }
    #endif

        IspPreviewConfigData.pCalIspPreviewPara->ContrastLevel = pCalIspVideoPara->ContrastLevel;
        IspPreviewConfigData.pCalIspPreviewPara->SaturationLevel = pCalIspVideoPara->SaturationLevel;
        IspPreviewConfigData.pCalIspPreviewPara->WbMode = pCalIspVideoPara->WbMode;
        IspPreviewConfigData.pCalIspPreviewPara->EvValue = pCalIspVideoPara->EvValue;
        IspPreviewConfigData.pCalIspPreviewPara->BandingFreq = pCalIspVideoPara->BandingFreq;
        IspPreviewConfigData.pCalIspPreviewPara->ImageEffect = pCalIspVideoPara->ImageEffect;
        

        /* update isp operation state */
        IspOperationState=ISP_MPEG4_ENCODE_STATE;

        /* config dsc operation parameter */
    #if (defined(AF_SUPPORT))
        IspOperationSetting(CAL_FEATURE_CAMERA_AF_OPERATION_MODE,IspVideoConfigData.pCalIspVideoPara->AfOperationMode);
        IspOperationSetting(CAL_FEATURE_CAMERA_AF_RANGE_CONTROL,IspVideoConfigData.pCalIspVideoPara->AfRange);
    #endif
        IspOperationSetting(CAL_FEATURE_CAMERA_SCENE_MODE,IspVideoConfigData.pCalIspVideoPara->VideoSceneMode);

        /* config camera image parameter */
        IspSetImageSetting(CAL_FEATURE_CAMERA_WB, IspVideoConfigData.pCalIspVideoPara->WbMode);
        IspSetImageSetting(CAL_FEATURE_CAMERA_EV_VALUE, IspVideoConfigData.pCalIspVideoPara->EvValue);
        IspSetImageSetting(CAL_FEATURE_CAMERA_BANDING_FREQ, IspVideoConfigData.pCalIspVideoPara->BandingFreq);    
        IspSetImageSetting(CAL_FEATURE_CAMERA_IMAGE_EFFECT, IspVideoConfigData.pCalIspVideoPara->ImageEffect);    

        IspSetImageSetting(CAL_FEATURE_CAMERA_HUE, IspVideoConfigData.pCalIspVideoPara->HueValue);
        IspSetImageSetting(CAL_FEATURE_CAMERA_CONTRAST, IspVideoConfigData.pCalIspVideoPara->ContrastLevel);
        IspSetImageSetting(CAL_FEATURE_CAMERA_SATURATION, IspVideoConfigData.pCalIspVideoPara->SaturationLevel);
        IspSetImageSetting(CAL_FEATURE_CAMERA_SHARPNESS, IspVideoConfigData.pCalIspVideoPara->SharpnessLevel);
        IspSetImageSetting(CAL_FEATURE_CAMERA_BRIGHTNESS, IspVideoConfigData.pCalIspVideoPara->BrightnessLevel);

        //enable_raw_cal_error_handling_interrupt(); // Enable overrun interrupt...
        /* Wait AE AWB Stable */
        //AaaWaitAeAwbStable();
        IspWaitSensorStableDelayFrame();

        /* start isp engine */
        IspStartInput(IspOperationState);

    #if defined(__CAL_TRACE_ENABLE__)
        kal_trace(TRACE_INFO,CAL_PROFILING_PREVIEW_FPS,IspCurrentFrameRate);
    #endif
        IspFrameCount=0;
        IspProcessStage|=LEAVE_MPEG4_ENCODE;

    }
    else
    {
        configASSERT(0);
    }

    return ErrorCode;
}


MM_ERROR_CODE_ENUM IspYuvStillCapture(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode,
                                       void *pCtrlPara, kal_uint16 CtrlParaLen)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    P_ISP_CAMERA_STILL_CAPTURE_STRUCT pIspCapturePara=(P_ISP_CAMERA_STILL_CAPTURE_STRUCT)pCtrlPara;
    P_CAL_ISP_STILL_CAPTURE_STRUCT pCalIspCapturePara=pIspCapturePara->pCalIspCapturePara;

//    extern ISP_DATA_FORMAT_ENUM IspInputDataFormat;
#if defined(EXIF_SUPPORT)
    exif_cam_para_struct* exif_cam_para_ptr;
    //exif_MTK_para_struct* exif_MTK_para_ptr;
#endif
#if  defined(__SERIAL_SENSOR_V1_SUPPORT__)
    SERIAL_SENSOR_INFO_IN_STRUCT SerialSensorInfoIn;
    SERIAL_SENSOR_INFO_OUT_STRUCT SerialSensorInfoOut;
	
	memset(&SerialSensorInfoOut, 0, sizeof(SerialSensorInfoOut));
#endif
    if (CAL_CTRL_CODE_STOP & CtrlCode)
    {
        /// Turn off Main strobe
    #if (defined(LED_FLASHLIGHT_SUPPORT))
        IspYuvFlashlightMainStrobeProcess(KAL_FALSE);
    #endif
        /* stop isp engine */
    #if defined(__JPEG_SENSOR_SUPPORT__)
        if((IMAGE_SENSOR_DATA_OUT_FORMAT_JPEG == SensorCaptureDataFormat)&&(IMAGE_SENSOR_DATA_OUT_FORMAT_JPEG == CurrentSensorInfo.CaptureDataFormat))
        {
            IspJpegSensorCaptureStop();
        }
        else
    #endif
        {
            IspStopInput(ISP_CAPTURE_JPEG_STATE);
        }

        /* update isp operation state */
        IspOperationState=ISP_STANDBY_STATE;

    #if(defined(AF_SUPPORT))
        if(ISP_YUV_SUPPORT == IspInputDataFormat)
        {
            if (((MULTISHOT_FALSE == IspMultiShotInfo) || (MULTISHOT_END == IspMultiShotInfo))&&(CAM_STILL_CAPTURE_PANO_VIEW != CalIspCaptureData.StillCaptureMode))
                IspOperationSetting(CAL_FEATURE_CAMERA_AF_KEY, CAM_AF_RELEASE);
        }
    #endif
        //disable_raw_cal_error_handling_interrupt();
        IspProcessStage|=LEAVE_STOP_CAMERA_CAPTURE;
    }
    else if (CAL_CTRL_CODE_START & CtrlCode)
    {
        memcpy(&IspCaptureConfigData, pIspCapturePara, sizeof(ISP_CAMERA_STILL_CAPTURE_STRUCT));
        memcpy(&CalIspCaptureData, pCalIspCapturePara, sizeof(CAL_ISP_STILL_CAPTURE_STRUCT));
        IspCaptureConfigData.pCalIspCapturePara = &CalIspCaptureData;

        if (IspOperationState==ISP_STANDBY_STATE)
        {
            //reset_raw_cal_error_handling();
        #if  defined(__SERIAL_SENSOR_V1_SUPPORT__)
            if(CurrentSensorInfo.SensorIf == IMAGE_SENSOR_IF_SERIAL) 
            {
                SerialSensorInfoIn.ScenarioId = CAL_SCENARIO_CAMERA_STILL_CAPTURE;
                ErrorCode = pfImageSensor->SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_GET_SERIAL_SENSOR_INFO,  (void *) &SerialSensorInfoIn,   
                                     (void *) &SerialSensorInfoOut, sizeof(SERIAL_SENSOR_INFO_OUT_STRUCT), &SensorFeatureParaOutLen);        
                if(MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT != ErrorCode)
                {
                    IspSerialInfSetSize(&SerialSensorInfoOut , 0x00);
                }//else
                    //ASSERT(0);
            }
        #endif

            SET_CAMERA_CAPTURE_MODE;

            ExifIspTargetWidth	=pIspCapturePara->ImageTargetWidth;
            ExifIspTargetHeight	=pIspCapturePara->ImageTargetHeight;
            ExifOrientation = pIspCapturePara->StillImageExifOrientation;

            /* config 3a algorithm */
            /******** Check if needed, why the value all 0 ********/
            IspOperationState = ISP_CAPTURE_JPEG_STATE;
 
            IspMultiShotInfo = pCalIspCapturePara->MultishotInfo;
            if ((MULTISHOT_FALSE==pCalIspCapturePara->MultishotInfo)||
                (MULTISHOT_FIRST==pCalIspCapturePara->MultishotInfo)||
                (CAM_STILL_CAPTURE_EV_BRACKETING==pCalIspCapturePara->StillCaptureMode))
            {
                //if(CAM_STILL_CAPTURE_EV_BRACKETING==pCalIspCapturePara->StillCaptureMode)
                //{

                /// Redeye strobe
            #if (defined(LED_FLASHLIGHT_SUPPORT))
                IspYuvFlashlightRedeyeStrobeProcess();
            #endif
                /* drop few unstable frames while changing sensor setting */
                IspWaitCaptureDelayFrame();

            #if defined(__CAL_PROFILING_ENABLE__)
                CameraCaptureProfilingWrapper(CAL_CAMERA_PROFILING_CONFIG_WAIT_STABLE_COMPLETE);
            #endif
                    /* config camera mode parameter */
                    // camera_mode_para_setting(&camera_mode_data);

                    /* config camera image parameter */
                    //IspSetImageSetting(CAL_FEATURE_CAMERA_WB, IspPreviewConfigData.pCalIspPreviewPara->WbMode);
                    //IspSetImageSetting(CAL_FEATURE_CAMERA_EV_VALUE, IspPreviewConfigData.pCalIspPreviewPara->EvValue);
                    //IspSetImageSetting(CAL_FEATURE_CAMERA_BANDING_FREQ, IspPreviewConfigData.pCalIspPreviewPara->BandingFreq);    
                    //IspSetImageSetting(CAL_FEATURE_CAMERA_IMAGE_EFFECT, IspPreviewConfigData.pCalIspPreviewPara->ImageEffect);    
                //}
            }


        #if (defined(EXIF_SUPPORT))
            /* config exif header here to update bracketing setting & has recorded dz when exit_preview */
            exif_cam_para_ptr=exif_get_cam_para_struct();
            exif_get_MTK_para_struct();
            ExifUpdateCamParaStruct(exif_cam_para_ptr);
        #endif

#if 0   // do not trigger ISP since we have to start MDP first
            if (ISP_CAPTURE_JPEG_STATE==IspOperationState)
            {
                IspStartInput(IspOperationState);
                //enable_raw_cal_error_handling_interrupt();	//for debug use

                IspProcessStage|=LEAVE_CAMERA_CAPTURE_JPEG;
            }
#endif
        }
        else if (ISP_CAPTURE_JPEG_STATE==IspOperationState)
        {
            /// Main strobe(must be immediate before the start ISP)
        #if (defined(LED_FLASHLIGHT_SUPPORT))
            IspYuvFlashlightMainStrobeProcess(KAL_TRUE);
        #endif
        #if defined(__JPEG_SENSOR_SUPPORT__)
            if((IMAGE_SENSOR_DATA_OUT_FORMAT_JPEG == SensorCaptureDataFormat)&&(IMAGE_SENSOR_DATA_OUT_FORMAT_JPEG == CurrentSensorInfo.CaptureDataFormat))
            {
                stack_start_timer(&CalTaskStackTimer, CAL_STACK_TIMER_INDEX_STILL_CAPTURE_TIMEOUT, 1100);                           
                ErrorCode = IspJpegSensorCaptureStart(JpegSensorParaIn.ImageBuffAddress);
            }
            else
        #endif
            {
                IspStartInput(IspOperationState);
                //enable_raw_cal_error_handling_interrupt();	//for debug use
            }

            IspProcessStage|=LEAVE_CAMERA_CAPTURE_JPEG;
        }
    }
    else
    {
        configASSERT(0);
    }

    return ErrorCode;
}   /* IspStillCaptureCtrl() */


kal_uint32 IspGetDigitalZoomFactor(void)
{
    kal_uint32 CurrentZoomFactor;

    if(IspGlobalInfo.ZoomStep!=IspGlobalInfo.TotalZoomStep)
    {
        CurrentZoomFactor=IspResultWindowWidth*100/IspZoomResultWindowWidth;
    }
    else
    {
        CurrentZoomFactor=IspGlobalInfo.MaxZoomFactor;
    }
    
    return CurrentZoomFactor;
}


kal_uint32 IspGetDigitalZoomVerticalFactor(void)
{
    kal_uint32 CurrentZoomFactor;
    
    if(IspGlobalInfo.ZoomStep!=IspGlobalInfo.TotalZoomStep)
    {
        CurrentZoomFactor=IspResultWindowHeight*100/IspZoomResultWindowHeight;
    }
    else
    {
        CurrentZoomFactor=IspGlobalInfo.MaxZoomFactor;
    }
    
    return CurrentZoomFactor;
}


/*********** zoom temp ****************/
void IspDigitalZoom(CAL_CAMERA_DIGITAL_ZOOM_TYPE_ENUM DzType,
    kal_uint8 ZoomStep,  const kal_uint16 *pZoomFactorListTable,
    P_ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT pParaOut)
{
//    kal_uint32 save_irq_mask;


    IspGlobalInfo.DzType=DzType;
    IspGlobalInfo.ZoomStep=ZoomStep;
    IspGlobalInfo.pZoomFactorListTable=pZoomFactorListTable;

    IspDigitalZoomConfig(IspOperationState,IspGlobalInfo.DzType,IspGlobalInfo.ZoomStep,IspGlobalInfo.pZoomFactorListTable,&OutputResolution);

    pParaOut->MdpCropEnable = KAL_TRUE;
    pParaOut->IspOutWidth = IspGrabWidth;
    pParaOut->IspOutHeight = IspGrabHeight;
    pParaOut->DigitalZoomWidth = IspZoomResultWindowWidth;
    pParaOut->DigitalZoomHeight = IspZoomResultWindowHeight;    

    //save_irq_mask=SaveAndSetIRQMask();
    IspDigitalZoomFlag=KAL_TRUE;
    //RestoreIRQMask(save_irq_mask);

    IspProcessStage|=LEAVE_ISP_DIGITAL_ZOOM;
}	/* raw_digital_zoom() */


kal_bool IspDigitalZoomCb(void *user_data)
{
    return KAL_TRUE;
}   /* camera_digital_zoom_cb() */




#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif



