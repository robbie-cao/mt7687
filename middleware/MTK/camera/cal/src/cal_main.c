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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   cal_main.c
 *
 * Project:
 * --------
 *   All
 *
 * Description:
 * ------------
 *   CAL (Camera Abstraction Layer) control the sensor, isp, LCD and MDP drivers.
 *
 * Author:
 * -------
 *   Tony Tsai (MTK12085)
 *
 *****************************************************************************/

#include "cal_api.h"
#include "idp_cam_if.h"
#include "cal_if.h"
#include "cal_drv_features.h"

//#include "camera_nvram_def.h"

#include "sensor_comm_def.h"
#include "image_sensor.h"
#include "isp_if.h"

#include "cal_mem_def.h"

#include <stdlib.h>
#include "syslog.h"

#include "hal_cache.h"
#include "hal_cache_internal.h"
#include "hal_dvfs.h"
#include "hal_dvfs_internal.h"

/* camera state */
#define CAM_IDLE                0
#define CAM_READY             1
#define CAM_PREVIEW         2
#define CAM_CAPTURE         3
#define CAM_HDR                 4
#define CAM_MAV                 5


P_IMAGE_SENSOR_LIST_STRUCT pImageSensorList=NULL;

CAL_STATE_ENUM CalState;
CAL_CAMERA_SOURCE_ENUM CurrentCameraId=CAL_CAMERA_SOURCE_MAIN;
QueueHandle_t cal_queue_handle;

CAL_SCENARIO_ENUM CurrentScenarioId=CAL_SCENARIO_CAMERA_PREVIEW;

/* SENSOR control callback functions */
P_IMAGE_SENSOR_FUNCTION_STRUCT pfImageSensor=NULL;
/* ISP control callback functions */
P_ISP_FUNCTION_STRUCT pfIspFunc=NULL;
/* Mdp control callback funtions */
IDP_CAL_INTF_FUNC_T MdpFunc;
/* Cam parameters structure */
cam_context_struct *cam_context_p=NULL;

#define CAM_ENTER_STATE(_s_) (cam_context_p->state = _s_)
#define CAM_IN_STATE(_s_) (cam_context_p->state == _s_)

#ifdef __ISP_SERIAL_SENSOR_SUPPORT__
extern void IspSerialInfCalibration(CAL_CAMERA_SOURCE_ENUM src);
#endif

/***********************************************
*                      Code Body                                                      *
***********************************************/

static bool cam_valid(uint32_t voltage, uint32_t frequency)
{
    /* Cam need to work under 208Mhz */
    return ((frequency < 208000) ?  false: true);
}

dvfs_notification_t cal_desc = {
    .domain = "VCORE",
    .module = "CM_CK0",
    .addressee = "cam_test",
    .ops = {
        .valid = cam_valid,
    },
};

void cam_free_working_buffer(void)
{

    if (cam_context_p->frame_buffer1_p)
    {
        vPortFree(cam_context_p->frame_buffer1_p);
    }

    if (cam_context_p->frame_buffer2_p)
    {
        vPortFree(cam_context_p->frame_buffer2_p);
    }

    if (cam_context_p->frame_buffer3_p)
    {
        vPortFree(cam_context_p->frame_buffer3_p);
    }

    if (cam_context_p->capture_buffer_p)
    {
        vPortFree(cam_context_p->capture_buffer_p);
        cam_context_p->capture_buffer_size = 0;
    }

    cam_context_p->frame_buffer_size = 0;
}

MM_ERROR_CODE_ENUM CalCameraPreviewReq(void)
{
    MM_ERROR_CODE_ENUM ret = MM_ERROR_NONE;
    CAL_SCENARIO_CTRL_STRUCT CalPreviewScenarioPara;
    CAL_CAMERA_PREVIEW_STRUCT CalPreviewPara;
    CAL_LCD_STRUCT CalLcdPara;
    CAL_ISP_PREVIEW_STRUCT CalIspPreviewPara;
    CAL_MDP_PREVIEW_STRUCT CalMdpPreviewPara;

    if (CAM_IN_STATE(CAM_READY))
    {
        cam_context_p->extmem_start_address = (kal_uint32)NULL;
        cam_context_p->extmem_size = 0;
        cam_context_p->binning_mode = 0;
        cam_context_p->app_id = 0;

        /* It will not use the app frame buffer, so set ut as null directly. */
        cam_context_p->app_frame_buffer_p = (kal_uint32)NULL;
        cam_context_p->app_frame_buffer_size = 0;

        cam_context_p->capture_buffer_p = 0;
        cam_context_p->capture_buffer_size = 0;

        cam_context_p->frame_buffer_size = LCD_WIDTH*LCD_HEIGHT*2;

        /* The user must prepare one buffer which width/height is frame_width/frame_height to store the test pattern. */
        cam_context_p->frame_buffer1_p = (kal_uint32)pvPortMalloc(LCD_WIDTH*LCD_HEIGHT*2);
        if(cam_context_p->frame_buffer1_p == 0 || (cache_is_buffer_cacheable(cam_context_p->frame_buffer1_p, LCD_WIDTH*LCD_HEIGHT*2))) {
            LOG_E(hal, "Cam FB1 malloc is fail\r\n");
            configASSERT(0);
        }

        LOG_I(hal, "Non-cacheable mem FB1:%x\r\n", cam_context_p->frame_buffer1_p);

        cam_context_p->frame_buffer2_p = (kal_uint32)pvPortMalloc(LCD_WIDTH*LCD_HEIGHT*2);
        if(cam_context_p->frame_buffer2_p == 0 || (cache_is_buffer_cacheable(cam_context_p->frame_buffer2_p, LCD_WIDTH*LCD_HEIGHT*2))) {
            LOG_E(hal, "Cam FB2 malloc is fail\r\n");
            configASSERT(0);
        }

        LOG_I(hal, "Non-cacheable mem FB2:%x\r\n", cam_context_p->frame_buffer1_p);

        cam_context_p->frame_buffer3_p = (kal_uint32)NULL;      /* Don't need use the tripple bubbfer for this hw verify. */

        cam_context_p->preview_offset_x = 0;
        cam_context_p->preview_offset_y = 0;
        cam_context_p->preview_width = LCD_WIDTH;
        cam_context_p->preview_height = LCD_HEIGHT;

        cam_context_p->ui_rotate = 0;
        cam_context_p->included_angle = 0;
        cam_context_p->preview_rotate = 0;
        cam_context_p->image_data_format = MM_IMAGE_FORMAT_PACKET_UYVY422;

        CalMdpPreviewPara.LcdUpdateEnable = cam_context_p->lcd_update = KAL_FALSE;
        CalMdpPreviewPara.DisplayImageWidth = cam_context_p->preview_width;
        CalMdpPreviewPara.DisplayImageHeight = cam_context_p->preview_height;
        CalMdpPreviewPara.FrameBuffAddr1 = cam_context_p->frame_buffer1_p;
        CalMdpPreviewPara.FrameBuffAddr2 = cam_context_p->frame_buffer2_p;
        CalMdpPreviewPara.FrameBuffAddr3 = (kal_uint32)NULL;    /* Don't need use the tripple bubbfer for this hw verify. */
        CalMdpPreviewPara.FrameBuffSize = cam_context_p->frame_buffer_size;
        CalMdpPreviewPara.OverlayEnable = KAL_FALSE;

        CalMdpPreviewPara.ImageRotAngle = 0;
        CalMdpPreviewPara.ImageDataFormat = (MM_IMAGE_FORMAT_ENUM)cam_context_p->image_data_format;

        CalIspPreviewPara.ZoomStep = cam_context_p->zoom_step = 0;
        CalIspPreviewPara.ContrastLevel = cam_context_p->contrast_level = 1;
        CalIspPreviewPara.SaturationLevel = cam_context_p->saturation_level = 1;
        CalIspPreviewPara.SharpnessLevel = cam_context_p->sharpness_level = 1;
        CalIspPreviewPara.WbMode = cam_context_p->wb_mode = 0;
        CalIspPreviewPara.EvValue = cam_context_p->ev_value = 4;
        CalIspPreviewPara.BandingFreq = cam_context_p->banding_freq = 0;
        CalIspPreviewPara.ImageEffect = cam_context_p->image_effect = 0;
        CalIspPreviewPara.SceneMode = cam_context_p->scene_mode = 0;
        CalIspPreviewPara.AeMeteringMode = cam_context_p->ae_metering_mode = 0;
        CalIspPreviewPara.FlashMode = cam_context_p->flash_mode = 0;
        CalIspPreviewPara.AfOperationMode = cam_context_p->af_operation_mode = 1;
        CalIspPreviewPara.AfRange = cam_context_p->af_range = 0;

        CalLcdPara.WfcElementHandle = 0x12345678;
        CalLcdPara.WfcContextHandle = 0x12345678;
        CalLcdPara.WfcDeviceHandle = 0x12345678;

        cam_context_p->extmem_size += CAL_PREVIEW_EXT_MEMORY_SIZE;
        if (cam_context_p->extmem_size)
        {
            cam_context_p->extmem_start_address = (kal_uint32)pvPortMalloc(cam_context_p->extmem_size);
            if(cam_context_p->extmem_start_address == 0) {
                LOG_E(hal, "Cam's extmem malloc error\r\n");
                configASSERT(0);
            }
        }

        CalPreviewPara.ExtmemStartAddress = cam_context_p->extmem_start_address;
        CalPreviewPara.ExtmemSize = cam_context_p->extmem_size;

        CalPreviewPara.pLcdPara = &CalLcdPara;
        CalPreviewPara.pMdpPara = &CalMdpPreviewPara;
        CalPreviewPara.pIspPara = &CalIspPreviewPara;
        CalPreviewPara.pPostProcPara = NULL;

        /* Start Camera Preview */
        CalPreviewScenarioPara.ScenarioId = CAL_SCENARIO_CAMERA_PREVIEW;
        CalPreviewScenarioPara.ScenarioCtrlCode = CAL_CTRL_CODE_START;
        CalPreviewScenarioPara.ScenarioCtrlParaLen = sizeof(CAL_CAMERA_PREVIEW_STRUCT);
        CalPreviewScenarioPara.pScenarioCtrlPara = &CalPreviewPara;

        ret = CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalPreviewScenarioPara, NULL, 0, NULL);

        if (ret == MM_ERROR_NONE) {
            CAM_ENTER_STATE(CAM_PREVIEW);
        } else {
            cam_free_working_buffer();
        }
    }

    return ret;
}

MM_ERROR_CODE_ENUM CalCameraPowerDownReq(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    CAL_SCENARIO_CTRL_STRUCT CalScenarioPara;

    if (!CAM_IN_STATE(CAM_IDLE))
    {
        if (CAM_IN_STATE(CAM_PREVIEW))
        {
            CalScenarioPara.ScenarioId=CAL_SCENARIO_CAMERA_PREVIEW;
            CalScenarioPara.ScenarioCtrlCode=CAL_CTRL_CODE_STOP;
            CalScenarioPara.ScenarioCtrlParaLen=0;
            CalScenarioPara.pScenarioCtrlPara=NULL;
            CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalScenarioPara, NULL, 0, NULL);
        }
        else if (CAM_IN_STATE(CAM_CAPTURE))
        {
            CalScenarioPara.ScenarioId=CAL_SCENARIO_CAMERA_STILL_CAPTURE;
            CalScenarioPara.ScenarioCtrlCode=CAL_CTRL_CODE_STOP;
            CalScenarioPara.ScenarioCtrlParaLen=0;
            CalScenarioPara.pScenarioCtrlPara=NULL;
            CalCtrl(CAL_SCENARIO_CTRL, (void *) &CalScenarioPara, NULL, 0, NULL);
        }

        CalClose();

        cam_free_working_buffer();
        CAM_ENTER_STATE(CAM_IDLE);

        return MM_ERROR_NONE;
    }

    return MM_ERROR_BEGIN;
}

MM_ERROR_CODE_ENUM AtCmdCalCameraPreviewReq(void)
{
    CAL_MSG msgs;

    msgs.src_mod = "ATCI";
    msgs.msg_id = MSG_ID_MEDIA_CAM_PREVIEW_REQ;

    printf("AtCmdCalCameraPreviewReq\r\n");

    xQueueSend(cal_queue_handle, (void*)&msgs, 0);

    return MM_ERROR_NONE;
}

MM_ERROR_CODE_ENUM CalCtrl(CAL_CTRL_ENUM CalCtrl_Num, void *pCalParaIn, void *pCalParaOut, kal_uint32 CalParaOutLen, kal_uint32 *pRealCalParaOutLen)
{
    MM_ERROR_CODE_ENUM ErrorCode;
    P_CAL_SCENARIO_CTRL_STRUCT pCalScenario=(P_CAL_SCENARIO_CTRL_STRUCT) pCalParaIn;

    ErrorCode = MM_ERROR_NONE;

    switch (CalCtrl_Num)
    {
        case CAL_SCENARIO_CTRL:
            CurrentScenarioId=pCalScenario->ScenarioId;
            ErrorCode = CalScenarioCtrl(pCalScenario->ScenarioId, pCalScenario->ScenarioCtrlCode,
                            pCalScenario->pScenarioCtrlPara, pCalScenario->ScenarioCtrlParaLen);
        break;
        case CAL_FEATURE_CTRL:
            ErrorCode = CalFeatureCtrl((P_CAL_FEATURE_CTRL_STRUCT) pCalParaIn, (P_CAL_FEATURE_CTRL_STRUCT) pCalParaOut,
                           CalParaOutLen, pRealCalParaOutLen);
        break;
        default:
            ErrorCode = MM_ERROR_CAL_INVALID_CTRL_TYPE;
        break;
    }
    return ErrorCode;
} /* CalCtrl() */

void CalInit(void)
{
    MM_ERROR_CODE_ENUM ErrorCode;

    cam_context_p = (cam_context_struct *)malloc(sizeof(cam_context_struct));

    ErrorCode = GetSensorInitFunc(&pImageSensorList);
    if(ErrorCode != MM_ERROR_NONE) {
        LOG_E(hal, "Get sensor init func is fail:%d\r\n", ErrorCode);
        configASSERT(0);
    }

    pImageSensorList->SensorFunc(&pfImageSensor);   /* default select the first sensor */

    ErrorCode = IspInit(&IspCallback, &pfIspFunc);
    if(ErrorCode != MM_ERROR_NONE) {
        LOG_E(hal, "Isp driver init is fail:%d\r\n", ErrorCode);
        configASSERT(0);
    }

    ErrorCode = Idp_Cal_If_Init(&MdpCallback, &MdpFunc);
    if(ErrorCode != MM_ERROR_NONE) {
        LOG_E(hal, "Idp cal interface init is fail:%d\r\n", ErrorCode);
        configASSERT(0);
    }

    cal_queue_handle = xQueueCreate(10, sizeof(CAL_MSG));

    CalState = CAL_IDLE_STATE;

} /* CalInit() */

MM_ERROR_CODE_ENUM CalOpen(void)
{
    MM_ERROR_CODE_ENUM ErrorCode = MM_ERROR_NONE;
    kal_uint32 FeatureOutLen;

    if (CalState!=CAL_IDLE_STATE)
        return MM_ERROR_CAL_INVALID_STATE;

    //ErrorCode = CalSearchSensor();

    dvfs_register_notification(&cal_desc);
    hal_dvfs_target_cpu_frequency(208000, HAL_DVFS_FREQ_RELATION_L);

    ErrorCode=CalPassSensorInfoToIsp(CurrentCameraId);    // default camera

    ErrorCode = pfIspFunc->IspFeatureCtrl(ISP_FEATURE_INIT_HW, (void *) &CurrentSensorInfo, NULL, 0, &FeatureOutLen);
    if(MM_ERROR_NONE != ErrorCode)
        LOG_E(hal, "Isp init hw is fail:%d\r\n", ErrorCode);

    ErrorCode=pfIspFunc->IspOpen();
    if(MM_ERROR_NONE != ErrorCode)
        LOG_E(hal, "IspOpen is fail:%d\r\n", ErrorCode);

    ErrorCode = pfImageSensor->SensorOpen();
    if(MM_ERROR_NONE != ErrorCode)
        LOG_E(hal, "SensorOpen is fail:%d\r\n", ErrorCode);

#if defined(__ISP_SERIAL_SENSOR_SUPPORT__)
//QQ calibration
    IspSerialInfCalibration(CurrentCameraId);
#endif

    if (MM_ERROR_NONE != ErrorCode)
    {
        LOG_E(hal, "Camera open process is fail.\r\n");
        // Close Sensor
        pfImageSensor->SensorClose();

        ///Deinit ISP HW
        pfIspFunc->IspFeatureCtrl(ISP_FEATURE_DEINIT_HW, (void *) &CurrentSensorInfo, NULL, 0, &FeatureOutLen);
        return ErrorCode;
    }

    CalState = CAL_STANDBY_STATE;
    CAM_ENTER_STATE(CAM_READY);

    return ErrorCode;
} /* CalOpen() */

MM_ERROR_CODE_ENUM CalClose(void)
{
    MM_ERROR_CODE_ENUM ErrorCode=MM_ERROR_NONE;
    kal_uint32 FeatureOutLen;

    CalState = CAL_IDLE_STATE;

    ErrorCode=pfImageSensor->SensorClose();
    if(MM_ERROR_NONE != ErrorCode)
        LOG_E(hal, "SensorClose is fail:%d\r\n", ErrorCode);

    ErrorCode=pfIspFunc->IspClose();
    if(MM_ERROR_NONE != ErrorCode)
        LOG_E(hal, "IspClose is fail:%d\r\n", ErrorCode);

    ErrorCode = pfIspFunc->IspFeatureCtrl(ISP_FEATURE_DEINIT_HW, (void *) &CurrentSensorInfo, NULL, 0, &FeatureOutLen);
    if(MM_ERROR_NONE != ErrorCode)
        LOG_E(hal, "Isp Deinit HW is fail:%d\r\n", ErrorCode);

    dvfs_deregister_notification(&cal_desc);

    return ErrorCode;
} /* CalClose() */

MM_ERROR_CODE_ENUM CalDeinit(void)
{
    return MM_ERROR_NONE;
} /* CalDeinit() */


