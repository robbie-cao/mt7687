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

#include "cal_api.h"
#include "cal_if.h"
#include "image_sensor.h"
#include "isp_if.h"
#include "cal_if.h"
#include "cal_drv_features.h"
#include "sensor_comm_def.h"
#include "feature_comm_def.h"
#include "cal_task_msg_if.h"
#include "sensor_capability.h"
#include "string.h"

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code = "DYNAMIC_CODE_CAMCAL_ROCODE", rodata = "DYNAMIC_CODE_CAMCAL_ROCODE"
#endif

#define CAL_FEATURE_CTRL_BUFF1_SIZE     (CAL_MAX(CAL_MAX(CAL_MAX(sizeof(ISP_FEATURE_CUSTOM_DRIVER_FEATURE_CTRL_FUN_STRUCT), sizeof(IMAGE_SENSOR_SET_CAMERA_SOURCE_STRUCT)),CAL_MAX(sizeof(ISP_SCENARIO_CTRL_STRUCT),sizeof(ISP_FEATURE_GET_OUTPUT_RESOLUTION_IN_STRUCT))),sizeof(ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_IN_STRUCT)) + 8)>>2
#define CAL_FEATURE_CTRL_BUFF2_SIZE     (sizeof(ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT) + 8)>>2

kal_uint32 CalFeatureCtrlBuff1[CAL_FEATURE_CTRL_BUFF1_SIZE];
kal_uint32 CalFeatureCtrlBuff2[CAL_FEATURE_CTRL_BUFF2_SIZE];

IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT CurrentSensorInfo;
kal_uint32 MainSensorListIdx=0, CurrentSensorListIdx=0;
kal_uint8 NextDigitalZoomStep=0;
CAL_CAMERA_DIGITAL_ZOOM_OUT_STRUCT CurrentDzInfo = {CAM_DZ_TYPE_RANGE, 1 , 100 , NULL};
CAM_IMAGE_SIZE_ENUM CurrentStillCaptureSize=CAM_IMAGE_SIZE_OTHER;

MM_ERROR_CODE_ENUM CalPassSensorInfoToIsp(CAL_CAMERA_SOURCE_ENUM CameraId)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    kal_uint32 OutParaLen;
    P_IMAGE_SENSOR_SET_CAMERA_SOURCE_STRUCT pCameraSource;
    P_ISP_FEATURE_CUSTOM_DRIVER_FEATURE_CTRL_FUN_STRUCT pfIspCustomFunc;


    if ((CAL_IDLE_STATE!=CalState)&&(CAL_CAMERA_SOURCE_MATV==CurrentCameraId)&&
     (CameraId<CAL_CAMERA_SOURCE_MATV))
    {
        return MM_ERROR_CAL_INVALID_STATE;
    }

    // inform ISP new customized function pointers.
    if (CAL_CAMERA_SOURCE_MAIN==CameraId)
    {
        CurrentSensorListIdx=MainSensorListIdx;
    }
    else
    {
        configASSERT(0);
        return MM_ERROR_CAL_INVALID_CAMERA_SOURCE_SELECT;
    }

    (pImageSensorList+CurrentSensorListIdx)->SensorFunc(&pfImageSensor);
    CurrentCameraId = CameraId;

    // notify ISP driver to change the function pointer of sensor feature control
    memset(&CurrentSensorInfo, 0, sizeof(CurrentSensorInfo));
    pfImageSensor->SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_GET_SENSOR_INFO, NULL,
                                     (void *) &CurrentSensorInfo, sizeof(IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT), &OutParaLen);
    pCameraSource = (P_IMAGE_SENSOR_SET_CAMERA_SOURCE_STRUCT) &CalFeatureCtrlBuff1[0];
    pCameraSource->ImageSensorIdx = (pImageSensorList+CurrentSensorListIdx)->SensorIdx;
    pfImageSensor->SensorFeatureCtrl(IMAGE_SENSOR_FEATURE_SET_CAMERA_SOURCE, pCameraSource, NULL, 0, NULL);

    pfIspCustomFunc=(P_ISP_FEATURE_CUSTOM_DRIVER_FEATURE_CTRL_FUN_STRUCT) &CalFeatureCtrlBuff1[0];
    pfIspCustomFunc->CameraId=CameraId;
    pfIspCustomFunc->SensorIdx=(pImageSensorList+CurrentSensorListIdx)->SensorIdx;
    pfIspCustomFunc->pSensorInfo=(void *) &CurrentSensorInfo;
    pfIspCustomFunc->SensorFeatureCtrl=pfImageSensor->SensorFeatureCtrl;
    pfIspFunc->IspFeatureCtrl(ISP_FEATURE_SET_CUSTOM_DRIVER_FEATURE_CTRL_FUNC, pfIspCustomFunc, NULL, 0, &OutParaLen);

    return ErrorCode;
}   /* CalPassSensorInfoToIsp() */

MM_ERROR_CODE_ENUM CalFeatureCtrl(P_CAL_FEATURE_CTRL_STRUCT pFeatureIn,
                                  P_CAL_FEATURE_CTRL_STRUCT pFeatureOut,
                                  kal_uint32 FeatureOutLen, kal_uint32 *pRealFeatureOutLen)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    P_CAL_FEATURE_TYPE_ENUM_STRUCT pCamFeatureEnum=(P_CAL_FEATURE_TYPE_ENUM_STRUCT) &(pFeatureOut->FeatureInfo.FeatureEnum);
    P_CAL_CAMERA_DIGITAL_ZOOM_OUT_STRUCT pCameraDzOut=((P_CAL_FEATURE_CTRL_STRUCT)pFeatureOut)->FeatureInfo.FeatureStructured.pCamDzOutPara;

    kal_bool FeatureEnumIsSupport;
    if ((pFeatureIn->FeatureId>CAL_FEATURE_CAMERA_BEGIN)&&
             (pFeatureIn->FeatureId<CAL_FEATURE_CAMERA_END))
    {
        switch (pFeatureIn->FeatureId)
        {
            case CAL_FEATURE_CAMERA_SOURCE_SELECT:
                if (pFeatureIn->FeatureSetValue != CurrentCameraId)
                    ErrorCode =CalPassSensorInfoToIsp((CAL_CAMERA_SOURCE_ENUM) pFeatureIn->FeatureSetValue);
            break;
            case CAL_FEATURE_CAMERA_DIGITAL_ZOOM:
                {   // query from ISP driver. the type would be range
                    if (CAL_FEATURE_SET_OPERATION==pFeatureIn->FeatureCtrlCode)
                    {   // 1. Notify MDP to callback when CRZ interrupt
                        // 2. Notify ISP to set the result window, execute in MdpCallback()
                        // 3. Notify MDP to configure CRZ, execute in MdpCallback()
                        NextDigitalZoomStep=pFeatureIn->FeatureSetValue;
                        //EnableCrzCb=KAL_TRUE;
                        //ErrorCode = MdpFunc.pfnIdpCalSet(CurrentScenarioId, IdpCalKey,  IDP_CAL_SET_CRZ_FRAMESTART_CB_ENABLE, &EnableCrzCb);
                    }
                    else
                    {   // query/get from ISP driver directly
                        ErrorCode =pfIspFunc->IspFeatureCtrl(pFeatureIn->FeatureId, pFeatureIn, pFeatureOut, FeatureOutLen, pRealFeatureOutLen);
                        if(CAL_CAMERA_MODE_CAMERA==pFeatureIn->CameraMode)
                            memcpy(&CurrentDzInfo, pCameraDzOut, sizeof(CAL_CAMERA_DIGITAL_ZOOM_OUT_STRUCT));

                    }
                }
            break;
            case CAL_FEATURE_CAMERA_CURRENT_ZOOM_FACTOR:
            case CAL_FEATURE_CAMERA_CURRENT_ZOOM_STEP:
                if (CAL_FEATURE_GET_OPERATION==pFeatureIn->FeatureCtrlCode)
                {
                    ErrorCode =pfIspFunc->IspFeatureCtrl(ISP_FEATURE_GET_CURRENT_ZOOM_INFO, NULL, (void *) &CurrentZoomInfo, 0, &FeatureOutLen);
                    if (CAL_FEATURE_CAMERA_CURRENT_ZOOM_FACTOR==pFeatureIn->FeatureId)
                        pFeatureOut->FeatureCurrentValue=CurrentZoomInfo.CurrentZoomFactor;
                    else
                        pFeatureOut->FeatureCurrentValue=CurrentZoomInfo.CurrentZoomStep;
                }
            break;
            case CAL_FEATURE_CAMERA_BINNING_INFO:
                {
                    P_CAL_FEATURE_CAMERA_BINNING_INFO_STRUCT pBinningInfo=pFeatureOut->FeatureInfo.FeatureStructured.pBinningInfo;
                    kal_uint8 i;
                    for (i=CAM_ISO_AUTO;i<CAM_NO_OF_ISO;i++)
                    {
                        pBinningInfo->IsoBinningInfo[i].MaxResolution=CAM_IMAGE_SIZE_1M;
                        pBinningInfo->IsoBinningInfo[i].IsoSupported=KAL_FALSE;
                        pBinningInfo->IsoBinningInfo[i].BinningEnable=KAL_FALSE;
                    }
                }
            break;
            case CAL_FEATURE_CAMERA_STILL_CAPTURE_SIZE:
                // call to camera_common for customization
                ErrorCode=pfImageSensor->SensorFeatureCtrl(pFeatureIn->FeatureId, pFeatureIn, pFeatureOut, FeatureOutLen, pRealFeatureOutLen);
                if (CAL_FEATURE_SET_OPERATION==pFeatureIn->FeatureCtrlCode)
                    CurrentStillCaptureSize= (CAM_IMAGE_SIZE_ENUM) pFeatureIn->FeatureSetValue;
            break;
            case CAL_FEATURE_CAMERA_STILL_CAPTURE_SIZE_WITH_DIMENSION:
                //if ((CAL_FEATURE_QUERY_OPERATION==pFeatureIn->FeatureCtrlCode) && (NULL!=pFeatureIn->FeatureInfo.FeatureEnum.SupportItemInfo))
                {
                   memset(pFeatureIn->FeatureInfo.FeatureEnum.SupportItemInfo,0x0,sizeof(CAL_CAMERA_IMAGE_SIZE_STRUCT)*MAX_CAMERA_SUPPORT_ITEM_COUNT);
                }

                ErrorCode=pfImageSensor->SensorFeatureCtrl(CAL_FEATURE_CAMERA_STILL_CAPTURE_SIZE, pFeatureIn, pFeatureOut, FeatureOutLen, pRealFeatureOutLen);
                FeatureEnumIsSupport = pFeatureOut->FeatureInfo.FeatureEnum.IsSupport;
                pfImageSensor->SensorFeatureCtrl(pFeatureIn->FeatureId, pFeatureIn, pFeatureOut, FeatureOutLen, pRealFeatureOutLen);

                //if ((CAL_FEATURE_QUERY_OPERATION==pFeatureIn->FeatureCtrlCode) && (NULL!=pFeatureIn->FeatureInfo.FeatureEnum.SupportItemInfo))
                {
                     if (FeatureEnumIsSupport)
                     {
                         kal_uint8 i;
                         P_CAL_CAMERA_IMAGE_SIZE_STRUCT pCalImageSize;

                         pCalImageSize = (CAL_CAMERA_IMAGE_SIZE_STRUCT *) pFeatureOut->FeatureInfo.FeatureEnum.SupportItemInfo;
                         pFeatureOut->FeatureInfo.FeatureEnum.IsSupport = FeatureEnumIsSupport;

                         for (i=0;i<pFeatureOut->FeatureInfo.FeatureEnum.ItemCount;i++)
                         {
                               //if ((0 ==pCalImageSize[i].ImageSizeWidth) && (0 ==pCalImageSize[i].ImageSizeHeight)) // It doesn't update in sensor driver
                               if (0 ==pCalImageSize[i].ImageSizeWidth) // It doesn't update in sensor driver
                               {
                                    switch (pFeatureOut->FeatureInfo.FeatureEnum.SupportItem[i])
                                    {
                                        case CAM_IMAGE_SIZE_QVGA:
                                                 pCalImageSize[i].ImageSizeWidth = 320;
                                                 pCalImageSize[i].ImageSizeHeight = 240;
                                                 break;
                                    #if (IMAGE_SENSOR_TARGET_MAX_WIDTH>=IMAGE_03M_MAX_WIDTH)
                                        case CAM_IMAGE_SIZE_VGA:
                                                 pCalImageSize[i].ImageSizeWidth = 640;
                                                 pCalImageSize[i].ImageSizeHeight = 480;
                                                 break;
                                    #endif
                                    #if (IMAGE_SENSOR_TARGET_MAX_WIDTH>=IMAGE_1M_MAX_WIDTH)
                                        case CAM_IMAGE_SIZE_1M:
                                                 pCalImageSize[i].ImageSizeWidth = 1280;
                                                 pCalImageSize[i].ImageSizeHeight = 960;//1024 or 960 ?
                                                 break;
                                    #endif
                                    #if (IMAGE_SENSOR_TARGET_MAX_WIDTH>=IMAGE_2M_MAX_WIDTH)
                                        case CAM_IMAGE_SIZE_2M:
                                                 pCalImageSize[i].ImageSizeWidth = 1600;
                                                 pCalImageSize[i].ImageSizeHeight = 1200;
                                                 break;
                                    #endif
                                    #if (IMAGE_SENSOR_TARGET_MAX_WIDTH>=IMAGE_3M_MAX_WIDTH)
                                        case CAM_IMAGE_SIZE_3M:
                                                 pCalImageSize[i].ImageSizeWidth = 2048;
                                                 pCalImageSize[i].ImageSizeHeight = 1536;
                                                 break;
                                    #endif
                                    #if (IMAGE_SENSOR_TARGET_MAX_WIDTH>=IMAGE_5M_MAX_WIDTH)
                                        case CAM_IMAGE_SIZE_5M:
                                                 pCalImageSize[i].ImageSizeWidth = 2592;
                                                 pCalImageSize[i].ImageSizeHeight = 1944;
                                                 break;
                                    #endif
                                    #if (IMAGE_SENSOR_TARGET_MAX_WIDTH>=IMAGE_8M_MAX_WIDTH)
                                        case CAM_IMAGE_SIZE_8M:
                                                 pCalImageSize[i].ImageSizeWidth = 3264;
                                                 pCalImageSize[i].ImageSizeHeight = 2448;
                                                 break;
                                    #endif
                         default:
                            break;
                                    }
                               }
                         }
                     }
                }
            break;
            case CAL_FEATURE_CAMERA_STILL_CAPTURE_MODE:
                ErrorCode=pfIspFunc->IspFeatureCtrl(pFeatureIn->FeatureId, pFeatureIn, pFeatureOut, FeatureOutLen, pRealFeatureOutLen);
            break;
            case CAL_FEATURE_CAMERA_GET_JPEG_SIZE_ESTIMATION:
                if (CAL_FEATURE_GET_OPERATION==pFeatureIn->FeatureCtrlCode)
                {
//                    jpeg_wrapper_query_dst_buffer_size((kal_uint32 *) &(pFeatureOut->FeatureCurrentValue),
//                                                       pFeatureIn->FeatureInfo.FeatureStructured.pGetJpegSizeEstimationPara->JpegEncQuality,
//                                                       GetJpegWrapperFormat(pFeatureIn->FeatureInfo.FeatureStructured.pGetJpegSizeEstimationPara->JpegSamplingFormat),
//                                                       pFeatureIn->FeatureInfo.FeatureStructured.pGetJpegSizeEstimationPara->ImageWidth,
//                                                       pFeatureIn->FeatureInfo.FeatureStructured.pGetJpegSizeEstimationPara->ImageHeight);
#if 0
                    jpeg_wrapper_query_dst_buffer_size((kal_uint32 *) &(pFeatureOut->FeatureCurrentValue),
                                                       pGetJpegSize->JpegEncQuality,
                                                       GetJpegWrapperFormat(pGetJpegSize->JpegSamplingFormat),
                                                       pGetJpegSize->ImageWidth, pGetJpegSize->ImageHeight);
#endif
                }
            break;
            default:
                {   // YUV sensor, pass the features to sensor driver.
                    if((pFeatureOut!=NULL) && (CAL_FEATURE_QUERY_OPERATION == pFeatureIn->FeatureCtrlCode))
                    {
                        pCamFeatureEnum->IsSupport = KAL_FALSE;//default value
                    }

                    switch (pFeatureIn->FeatureId)
                    {
                        case CAL_FEATURE_CAMERA_IMAGE_EFFECT:
                            if(CAL_FEATURE_QUERY_OPERATION == pFeatureIn->FeatureCtrlCode)
                            {
                                ErrorCode= pfImageSensor->SensorFeatureCtrl(pFeatureIn->FeatureId, pFeatureIn, pFeatureOut, FeatureOutLen, pRealFeatureOutLen);
                            }
                            else
                            {
                                ErrorCode=pfIspFunc->IspFeatureCtrl(pFeatureIn->FeatureId, pFeatureIn, pFeatureOut, FeatureOutLen, pRealFeatureOutLen);
                            }
                            break;
                        case CAL_FEATURE_CAMERA_CONTRAST:
                        case CAL_FEATURE_CAMERA_SATURATION:
                        case CAL_FEATURE_CAMERA_SHARPNESS:
                        case CAL_FEATURE_CAMERA_WB:
                        case CAL_FEATURE_CAMERA_EV_VALUE:
                        case CAL_FEATURE_CAMERA_FLASH_MODE:
                        case CAL_FEATURE_CAMERA_BANDING_FREQ:
                        case CAL_FEATURE_CAMERA_AE_METERING_MODE:
                        case CAL_FEATURE_CAMERA_SCENE_MODE:
                        case CAL_FEATURE_CAMERA_VIDEO_SCENE_MODE:
                            if(CAL_FEATURE_QUERY_OPERATION == pFeatureIn->FeatureCtrlCode)
                                ErrorCode= pfImageSensor->SensorFeatureCtrl(pFeatureIn->FeatureId, pFeatureIn, pFeatureOut, FeatureOutLen, pRealFeatureOutLen);
                            else
                                ErrorCode=pfIspFunc->IspFeatureCtrl(pFeatureIn->FeatureId, pFeatureIn, pFeatureOut, FeatureOutLen, pRealFeatureOutLen);
                        break;
                        default:
                            ErrorCode = MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
                        break;
                    }
                }
            break;
        }
    }

    return ErrorCode;
} /* CalFeatureCtrl() */

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif


