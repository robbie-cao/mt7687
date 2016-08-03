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
#include "isp_comm_def.h"
#include "isp_hw_feature_def.h"
#include "isp_if.h"
#include "isp_comm_if.h"
#include "sensor_comm_def.h"

extern IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT CurrentSensorInfo;

#if defined(__ISP_SERIAL_SENSOR_SUPPORT__)
extern void IspSerialInfCalibration(void);
#endif

kal_uint32 IspBakTgPhaseCountReg = 0x5a5a5a5a;
kal_uint32 IspBakSensorModeConfigReg = 0x5a5a5a5a;
kal_uint32 IspBakCameraModulePathConfigReg = 0x5a5a5a5a;
kal_uint32 IspBakProcessCtrlReg1= 0x5a5a5a5a;



MM_ERROR_CODE_ENUM (* pfCalIspCb) (CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);

ISP_OPERATION_STATE_ENUM IspOperationState=ISP_IDLE_STATE;
kal_uint32 SensorFeatureParaOutLen;
kal_bool IspFirstPreviewFlag = KAL_FALSE;
kal_bool IspIsDscSupportInfoQueried = KAL_FALSE;

MM_ERROR_CODE_ENUM IspCtrl(CAL_SCENARIO_ENUM ScenarioId, void *pCtrlParaIn,
                            void *pCtrlParaOut, kal_uint32 CtrlParaOutLen,
                            kal_uint32 *pRealCtrlParaOutLen)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    P_ISP_SCENARIO_CTRL_STRUCT pIspScenarioCtrl=(P_ISP_SCENARIO_CTRL_STRUCT)pCtrlParaIn;

    switch (ScenarioId)
    {
        case CAL_SCENARIO_CAMERA_PREVIEW:
        {
            ErrorCode = IspYuvPreview(ScenarioId, pIspScenarioCtrl->ScenarioCtrlCode,
                            pIspScenarioCtrl->pScenarioCtrlPara, pIspScenarioCtrl->ScenarioCtrlParaLen);
        }
        break;
        
        case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        {
            ErrorCode = IspYuvStillCapture(ScenarioId, pIspScenarioCtrl->ScenarioCtrlCode,
                            pIspScenarioCtrl->pScenarioCtrlPara, pIspScenarioCtrl->ScenarioCtrlParaLen);
        }
        break;
        
        case CAL_SCENARIO_VIDEO:
        {
            ErrorCode = IspYuvVideo(ScenarioId, pIspScenarioCtrl->ScenarioCtrlCode,
                            pIspScenarioCtrl->pScenarioCtrlPara, pIspScenarioCtrl->ScenarioCtrlParaLen);
        }
        break;
        case CAL_SCENARIO_CAMERA_ZSD_PREVIEW:
        case CAL_SCENARIO_CAMERA_ZSD_CAPTURE:
        case CAL_SCENARIO_VIDEO_TELEPHONY:
        case CAL_SCENARIO_WEBCAM_PREVIEW:
        case CAL_SCENARIO_WEBCAM_CAPTURE:
        case CAL_SCENARIO_MATV:
        case CAL_SCENARIO_POST_PROCESSING:
        case CAL_SCENARIO_CCT_CAMERA_PREVIEW:
        case CAL_SCENARIO_CCT_CAMERA_CAPTURE_JPEG:
        case CAL_SCENARIO_CCT_CAMERA_CAPTURE_RAW:
        case CAL_SCENARIO_CAMERA_STILL_CAPTURE_MEM:
        case CAL_SCENARIO_VIDEO_MJPEG:
        case CAL_SCENARIO_CAMERA_MAV_CAPTURE:
        case CAL_SCENARIO_CAMERA_GYRO_PREVIEW:
        case CAL_SCENARIO_MAX:
        break;

    }
    return ErrorCode;
} /* IspCtrl() */

MM_ERROR_CODE_ENUM IspOpen(void)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;


    if(IspBakTgPhaseCountReg == 0x5a5a5a5a)
    {
        IspBakTgPhaseCountReg = REG_ISP_TG_PHASE_COUNTER;
    }
    if(IspBakSensorModeConfigReg == 0x5a5a5a5a)
    {
        IspBakSensorModeConfigReg = REG_ISP_CMOS_SENSOR_MODE_CONFIG;
    }
    if(IspBakCameraModulePathConfigReg == 0x5a5a5a5a)
    {
        IspBakCameraModulePathConfigReg = REG_ISP_PATH_CONFIG;
    }
    if(IspBakProcessCtrlReg1 == 0x5a5a5a5a)
    {
        IspBakProcessCtrlReg1= REG_ISP_PREPROCESS_CTRL1;
    }

    IspOperationState=ISP_STANDBY_STATE;
    IspProcessStage = 0;
    IspFirstPreviewFlag = KAL_TRUE;

#if (defined(DUAL_CAMERA_SUPPORT))
    if(CAL_CAMERA_SOURCE_MAIN==CustomDriverFunc.CameraId || CAL_CAMERA_SOURCE_SUB==CustomDriverFunc.CameraId)
#else
    if(CAL_CAMERA_SOURCE_MAIN==CustomDriverFunc.CameraId )
#endif
    {
        CameraInitOperationPara();
   
        InitIsp();
        IspCheckFirstVsync = KAL_TRUE;
    }
 
    /*Init ISP interrupt service runtine */
    IspInitISR();
    ENABLE_CMOS_SENSOR;

    return ErrorCode;
} /* IspOpen() */

MM_ERROR_CODE_ENUM IspClose(void)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;

    if (IspOperationState!=ISP_STANDBY_STATE)
    {
        configASSERT(0);
    }

    POWER_OFF_CMOS_SENSOR;
    DISABLE_CMOS_SENSOR;
    REG_ISP_VIEW_FINDER_MODE_CTRL=0;	/* Clear ISP Pre/Cap Mode and disable view finder mode */

    NVIC_DisableIRQ(18);
    IspOperationState=ISP_IDLE_STATE;
    IspIsDscSupportInfoQueried=KAL_FALSE;

    /* Set Low to MCLK */
    if (CurrentSensorInfo.IsSensorDriverCtrlMclk == KAL_FALSE)
    {
        SET_CMOS_FALLING_EDGE(0);
        DISABLE_CAMERA_TG_PHASE_COUNTER;
        DISABLE_CAMERA_CLOCK_OUTPUT_TO_CMOS;
    }

    IspProcessStage|=LEAVE_POWER_OFF_ISP;

    return ErrorCode;
} /* IspClose() */


ISP_FUNCTION_STRUCT IspFunc=
{
    IspOpen,
    IspFeatureCtrl,
    IspCtrl,
    IspClose,
};
   

MM_ERROR_CODE_ENUM IspInit(MM_ERROR_CODE_ENUM (* pfIspCallback) (CAL_CALLBACK_ID_ENUM CbId,
                           void *pCallbackPara, kal_uint16 CallbackParaLen),
                           P_ISP_FUNCTION_STRUCT *pfIspFunc)
{
    pfCalIspCb = pfIspCallback;
    *pfIspFunc=&IspFunc;

    IspOperationState = ISP_IDLE_STATE;

    IspPreviewConfigData.pCalIspPreviewPara = &CalIspPreviewData;
    
    return MM_ERROR_NONE;
}

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif


