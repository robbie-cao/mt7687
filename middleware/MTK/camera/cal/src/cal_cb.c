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
#include "cal_drv_features.h"
#include "cal_task_msg_if.h"
#include "cal_mem_def.h"

#include "isp_comm_def.h"
#include "sensor_comm_def.h"
#include "image_sensor.h"
#include "idp_cam_if.h"

//#include "drv_features.h"
//#include "drv_sw_features_mdp.h"

// rotator
//#include "rotator_enum.h"
//#include "yuv_rotator_api.h"
//#include "rgb_rotator_api.h"

//#include "visualhisr.h"

#ifdef __CAL_CAMERA_SW_Y2R_DITHERING_FOR_QV_SUPPORT__
CAL_IMAGE_Y2R_DITHERING_STRUCT ImageY2RDitheringPara;
#endif

ISP_FEATURE_CURRENT_ZOOM_INFO_STRUCT CurrentZoomInfo;
kal_uint32 CurrentStillCaptureStatus=0;     // to keep how many images (main/quick view/thumbnail) are ready for still capture

#if defined (__CAL_CAPTURE_RAW_DATA_SUPPORT__)
extern CAL_CAMERA_STILL_CAPTURE_ENUM CalStillCaptureMode;
#endif

MM_ERROR_CODE_ENUM IspCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;

    switch (CbId)
    {
    #ifdef __CAL_CAMERA_AF_SUPPORT__
        case CAL_CBID_AF_RESULT:
            pfCalCallback(CAL_CBID_AF_RESULT, pCallbackPara, CallbackParaLen);
        break;
    #endif
        default:
        break;
    }

    return ErrorCode;
} /* IspCallback() */

MM_ERROR_CODE_ENUM MdpCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    //P_CAL_BUFFER_CB_STRUCT pCbBuff=(P_CAL_BUFFER_CB_STRUCT) pCallbackPara;
    Digital_Zoom_Crop_Ctrl_Struct MdpDigitalZoomPara;
    P_ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_IN_STRUCT pIspDigitalZoomResParaIn;
    P_ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT pIspDigitalZoomResParaOut;
    kal_uint32 FeatureOutLen;//, ImageResolution;
    kal_bool ZoomEnable=KAL_FALSE;
    kal_bool EnableCrzCb=KAL_FALSE;

    //Camera_Capture_Crop_Buffer_Struct *pCbCropBuff=(Camera_Capture_Crop_Buffer_Struct*) pCallbackPara;

    switch (CbId)
    {
        case IDPCAL_CBID_CAM_CAPTURE_YUV_IMG_DONE:
            pfIspFunc->IspFeatureCtrl(ISP_FEATURE_DISABLE_VIEWFINDER, NULL, NULL, 0, 0);
        break;
        case IDPCAL_CBID_CAM_PREVIEW_DISPLAY_FRM_DONE:
        case IDPCAL_CBID_VIDEO_ENCODE_PREVIEW_DISPLAY_FRM_DONE:
        break;
        case IDPCAL_CBID_CAM_PREVIEW_CRZ_FRM_START:
        case IDPCAL_CBID_VIDEO_ENCODE_CRZ_FRM_START:
        // 2. Notify ISP to set the result window, execute in MdpCallback()
            pIspDigitalZoomResParaIn=(P_ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_IN_STRUCT) CalFeatureCtrlBuff1;
            pIspDigitalZoomResParaOut=(P_ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT) CalFeatureCtrlBuff2;

            {   // single step zoom
                pIspDigitalZoomResParaIn->ZoomStep=NextDigitalZoomStep;
                CurrentZoomInfo.CurrentZoomStep=NextDigitalZoomStep;
                ZoomEnable=KAL_TRUE;
            }

            if (KAL_TRUE==ZoomEnable)
            {
                ErrorCode=pfIspFunc->IspFeatureCtrl(ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION, pIspDigitalZoomResParaIn,
                                                    pIspDigitalZoomResParaOut, sizeof(ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT),
                                                    &FeatureOutLen);

                // 3. Notify MDP to configure CRZ, execute in MdpCallback()
                MdpDigitalZoomPara.mdp_crop_enable=pIspDigitalZoomResParaOut->MdpCropEnable;
                MdpDigitalZoomPara.updated_source_width=pIspDigitalZoomResParaOut->IspOutWidth;
                MdpDigitalZoomPara.updated_source_height=pIspDigitalZoomResParaOut->IspOutHeight;
                MdpDigitalZoomPara.cropped_width=pIspDigitalZoomResParaOut->DigitalZoomWidth;
                MdpDigitalZoomPara.cropped_height=pIspDigitalZoomResParaOut->DigitalZoomHeight;
                ErrorCode = MdpFunc.pfnIdpCalSet(CurrentScenarioId, IdpCalKey, IDP_CAL_SET_ZOOM_PARAM, &MdpDigitalZoomPara);
            }

            if (KAL_FALSE==EnableCrzCb)
            {   // stop CRZ callback here
                ErrorCode = MdpFunc.pfnIdpCalSet(CurrentScenarioId, IdpCalKey,  IDP_CAL_SET_CRZ_FRAMESTART_CB_ENABLE, &EnableCrzCb);
            }
        break;
        case IDPCAL_CBID_CAM_CAPTURE_MAIN_IMAGE_DONE:
        break;
    #ifdef __CAL_HW_QUICKVIEW_THUMBNAIL_SUPPORT__
        case IDPCAL_CBID_CAM_CAPTURE_QUICKVIEW_IMAGE_DONE:
        break;
        case IDPCAL_CBID_CAM_CAPTURE_THUMBNAIL_IMAGE_DONE:
            CurrentStillCaptureStatus |= CAL_STILL_CAPTURE_THUMBNAIL_IMAGE_BIT;
        break;
    #endif  // __CAL_HW_QUICKVIEW_THUMBNAIL_SUPPORT__
        case IDPCAL_CBID_CAM_CAPTURE_CROP_SIZE_IMAGE_DONE:
        break;
        default:
        break;
    }

    if (CAL_CAMERA_CAPTURE_STATE==CalState)
    {
        #ifdef LED_FLASHLIGHT_SUPPORT
        if (CurrentStillCaptureStatus & CAL_STILL_CAPTURE_IMAGE_MASK)
        {
            pfIspFunc->IspFeatureCtrl(ISP_FEATURE_STOP_FLASHLIGHT_PROCESS_AFTER_CAPTURE_DONE, NULL, NULL, 0, 0);
        }
        #endif
    }

    return ErrorCode;
} /* MdpCallback() */


