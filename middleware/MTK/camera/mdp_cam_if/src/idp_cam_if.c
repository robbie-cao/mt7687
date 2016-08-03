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

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "mm_comm_def.h"
#include "cal_comm_def.h"
#include "idp_cam_if.h"
#include "idp_camera_preview.h"
//#include "idp_camera_capture_to_jpeg.h"
//#include "idp_video_encode.h"
//#include "idp_camera_capture_to_mem.h"
//#include "idp_video_call_decode.h"

MM_ERROR_CODE_ENUM (*pfIDP_CAL_CB)(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);


static MM_ERROR_CODE_ENUM Idp_Cal_Open(
        CAL_SCENARIO_ENUM eCalScenID,
        kal_uint32 * const key,
        char const * const filename,
        kal_uint32 const lineno)
{
    MM_ERROR_CODE_ENUM eRet = MM_ERROR_NONE;
    kal_bool result = KAL_FALSE;

    switch(eCalScenID)
    {
      case CAL_SCENARIO_CAMERA_PREVIEW:
        result = idp_camera_preview_open_real(key, filename, lineno);
        break;
      case CAL_SCENARIO_VIDEO:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE_MEM:
        break;
      default:
        break;
    }

    if (result == KAL_FALSE)
    {eRet = MM_ERROR_MDP_BEGIN;}
    else
    {eRet = MM_ERROR_NONE;}

    return eRet;
}


static MM_ERROR_CODE_ENUM Idp_Cal_Close(
        CAL_SCENARIO_ENUM eCalScenID,
        kal_uint32 const key)
{
    MM_ERROR_CODE_ENUM eRet = MM_ERROR_NONE;
    kal_bool result = KAL_FALSE;

    switch(eCalScenID)
    {
      case CAL_SCENARIO_CAMERA_PREVIEW:
        result = idp_camera_preview_close(key);
        break;
      case CAL_SCENARIO_VIDEO:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE_MEM:
        break;
      default:
        break;
    }

    if (result == KAL_FALSE)
    // temp solution, need to modify error type to MM_ERROR_MDP_CLOSE
    {eRet = MM_ERROR_MDP_BEGIN;}
    else
    {eRet = MM_ERROR_NONE;}

    return eRet;
}


static MM_ERROR_CODE_ENUM Idp_Cal_Start(
        CAL_SCENARIO_ENUM eCalScenID,
        kal_uint32 const key)
{
    MM_ERROR_CODE_ENUM eRet = MM_ERROR_NONE;
    kal_bool result = KAL_FALSE;

    switch(eCalScenID)
    {
      case CAL_SCENARIO_CAMERA_PREVIEW:
        result = idp_camera_preview_start_all(key);
        break;
      case CAL_SCENARIO_VIDEO:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE_MEM:
        break;
      default:
        break;
    }

    if (result == KAL_FALSE)
    // temp solution, need to modify error type to MM_ERROR_MDP_START
    {eRet = MM_ERROR_MDP_BEGIN;}
    else
    {eRet = MM_ERROR_NONE;}

    return eRet;
}

static MM_ERROR_CODE_ENUM Idp_Cal_Stop(
        CAL_SCENARIO_ENUM eCalScenID,
        kal_uint32 const key)
{
    switch(eCalScenID)
    {
      case CAL_SCENARIO_CAMERA_PREVIEW:
        break;
      case CAL_SCENARIO_VIDEO:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE_MEM:
        break;
      default:
        break;
    }

    return MM_ERROR_NONE;
}


static kal_bool Idp_Cal_Is_In_Use(
        CAL_SCENARIO_ENUM eCalScenID)
{
    kal_bool result = KAL_TRUE;

    switch(eCalScenID)
    {
      case CAL_SCENARIO_CAMERA_PREVIEW:
        result = idp_camera_preview_is_in_use();
        break;
      case CAL_SCENARIO_VIDEO:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        break;
      case CAL_SCENARIO_CAMERA_STILL_CAPTURE_MEM:
        break;
      default:
        result = KAL_FALSE;
        break;
    }

    return result;
}


static MM_ERROR_CODE_ENUM Idp_Cal_Camera_Preview_Set(
        kal_uint32 const key,
        IDP_CAL_SET_TYPE_T eSetType,
        void *pvParam)
{
    MM_ERROR_CODE_ENUM eRet = MM_ERROR_NONE;
    kal_bool result = KAL_TRUE;

    switch(eSetType)
    {
      case IDP_CAL_SET_CONFIG:
        {
          Camera_Preview_Idp_Cal_Struct *prCamPrev;
          prCamPrev = (Camera_Preview_Idp_Cal_Struct*) pvParam;

          result &= idp_camera_preview_config(
            key,
            IDP_CAMERA_PREVIEW_IMAGE_SRC_WIDTH, prCamPrev->source_width,
            IDP_CAMERA_PREVIEW_IMAGE_SRC_HEIGHT, prCamPrev->source_height,
            IDP_CAMERA_PREVIEW_PREVIEW_WIDTH, prCamPrev->display_width,
            IDP_CAMERA_PREVIEW_PREVIEW_HEIGHT, prCamPrev->display_height,
            IDP_CAMERA_PREVIEW_PREVIEW_IMAGE_COLOR_FORMAT, prCamPrev->preview_image_color_format,
            IDP_CAMERA_PREVIEW_FRAME_BUFFER_ADDRESS, prCamPrev->frame_buffer_address1,
            IDP_CAMERA_PREVIEW_FRAME_BUFFER_ADDRESS1, prCamPrev->frame_buffer_address2,
            IDP_CAMERA_PREVIEW_POSTPROC_PATH_ENABLE, prCamPrev->postproc_image_enable,
            IDP_CAMERA_PREVIEW_DISPLAY_ROT_ANGLE, prCamPrev->display_rot_angle,
            IDP_CAMERA_PREVIEW_ENABLE_TRIGGER_LCD, prCamPrev->lcd_update_enable,
            IDP_CAMERA_PREVIEW_FRAME_START_HISR_CB_ENABLE, prCamPrev->digital_zoom_frame_start_hisr_cb_enable,
            IDP_CAMERA_PREVIEW_PREVIEW_CROP_ENABLE,   prCamPrev->mdp_crop_enable,
            IDP_CAMERA_PREVIEW_PREVIEW_CROP_WIDTH,    prCamPrev->cropped_width,
            IDP_CAMERA_PREVIEW_PREVIEW_CROP_HEIGHT,   prCamPrev->cropped_height,
            IDP_CAMERA_PREVIEW_ENABLE_DOUBLE_BUFFER_TEARING_FREE,   prCamPrev->dbuf_tearing_free_enable,
            //In general matv receive data in crz frame to mdp frame done(rotdma done). for example MT5192.
            //MT5193 changes the original design, it will receive data about 16~20ms before frame start.
            //So, in order to aovid LCD interference MATV, we will delay trigger display in next frame start.
            IDP_CAMERA_PREVIEW_ENABLE_TRIGGER_DISPLAY_DELAY_TO_FRAME_START, prCamPrev->trigger_display_delay_to_frame_start_enable,
            0);
          configASSERT(KAL_TRUE == result);

          if(prCamPrev->postproc_image_enable == KAL_TRUE)
          {
            result &= idp_camera_preview_config(
              key,
              IDP_CAMERA_PREVIEW_POSTPROC_ROT_ANGLE, prCamPrev->postproc_image_rot_angle,
              IDP_CAMERA_PREVIEW_POSTPROC_IMAGE_COLOR_FORMAT, prCamPrev->postproc_image_color_format,
              IDP_CAMERA_PREVIEW_POSTPROC_IMAGE_WIDTH, prCamPrev->postproc_image_width,
              IDP_CAMERA_PREVIEW_POSTPROC_IMAGE_HEIGHT, prCamPrev->postproc_image_height,
              IDP_CAMERA_PREVIEW_POSTPROC_BUFFER_ADDRESS_1, prCamPrev->postproc_image_buffer_address_1,
              IDP_CAMERA_PREVIEW_POSTPROC_BUFFER_ADDRESS_2, prCamPrev->postproc_image_buffer_address_2,
              IDP_CAMERA_PREVIEW_POSTPROC_BUFFER_ADDRESS_3, prCamPrev->postproc_image_buffer_address_3,
              IDP_CAMERA_PREVIEW_ENG_CONFIG, KAL_TRUE,
              0);
          }
#if (defined(DRV_IDP_6252_SERIES))
          else
          {
            result &= idp_camera_preview_config(
              key,
              IDP_CAMERA_PREVIEW_ENG_CONFIG, KAL_TRUE,
              0);
          }
#endif
        }
        break;

      case IDP_CAL_SET_CRZ_FRAMESTART_CB_ENABLE:
        result &= idp_camera_preview_config_fast(key, IDP_CAMERA_PREVIEW_FRAME_START_HISR_CB_ENABLE,  *(kal_uint32*)pvParam);
        break;

      case IDP_CAL_SET_DISPLAY_UPDATE:
        result &= idp_camera_preview_config_fast(key, IDP_CAMERA_PREVIEW_ENABLE_TRIGGER_LCD,  *(kal_uint32*)pvParam);
        break;

      case IDP_CAL_SET_ZOOM_PARAM:
        {
          Digital_Zoom_Crop_Ctrl_Struct *prCamPrev = (Digital_Zoom_Crop_Ctrl_Struct*) pvParam;
          result &= idp_camera_preview_config(
            key,
            IDP_CAMERA_PREVIEW_IMAGE_SRC_WIDTH,       prCamPrev->updated_source_width,
            IDP_CAMERA_PREVIEW_IMAGE_SRC_HEIGHT,      prCamPrev->updated_source_height,
            IDP_CAMERA_PREVIEW_PREVIEW_CROP_ENABLE,   prCamPrev->mdp_crop_enable,
            IDP_CAMERA_PREVIEW_PREVIEW_CROP_WIDTH,    prCamPrev->cropped_width,
            IDP_CAMERA_PREVIEW_PREVIEW_CROP_HEIGHT,   prCamPrev->cropped_height,
            0);
        }
        break;

      default:
        configASSERT(0);
        break;
    }

    if (result == KAL_FALSE)
    {eRet = MM_ERROR_INVALID_PARAMETER;}
    else
    {eRet = MM_ERROR_NONE;}

    return eRet;
} // end of Idp_Cal_Camera_Preview_Set()

static MM_ERROR_CODE_ENUM Idp_Cal_Set(
        CAL_SCENARIO_ENUM eCalScenID,
        kal_uint32 const key,
        IDP_CAL_SET_TYPE_T eSetType,
        void *pvParam)
{
    MM_ERROR_CODE_ENUM eRet = MM_ERROR_NONE;

    switch(eCalScenID)
    {
        case CAL_SCENARIO_CAMERA_PREVIEW:
            eRet = Idp_Cal_Camera_Preview_Set(key, eSetType, pvParam);
        break;
        case CAL_SCENARIO_VIDEO:
        break;
        case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        break;
        case CAL_SCENARIO_CAMERA_STILL_CAPTURE_MEM:
        break;
        default:
        break;
    }

    return eRet;
}


/* Note: The query function is designed for selected capabilities. Not all combinations can be queried, but it is
 *       definitely possible to extend for general capabilities.
 *       Please refer to the comment of IDP_CAL_QUERY_TYPE_T for the usage of different query types.
 */
static MM_ERROR_CODE_ENUM Idp_Cal_Query(
        const CAL_SCENARIO_ENUM eCalScenID,
        const kal_uint32 key,
        const IDP_CAL_QUERY_TYPE_T eQryType,
        const IDP_CAL_QUERY_ARG_T *prArg,
        void *pvResult)
{

    MM_ERROR_CODE_ENUM eRet = MM_ERROR_NONE;
    kal_uint32 u4QueryResult;

    switch(eCalScenID)
    {
        case CAL_SCENARIO_CAMERA_PREVIEW:
            idp_camera_preview_query(key,
                IDP_CAMERA_PREVIEW_QUERY_TYPE, eQryType,
                IDP_CAMERA_PREVIEW_QUERY_ARG_PATH, prArg->ePath,
                IDP_CAMERA_PREVIEW_QUERY_ARG_CLR_FMT, prArg->eClrFmt,
                IDP_CAMERA_PREVIEW_QUERY_ARG_DESIRED_ANGLE,  prArg->eDesiredAngle,
                IDP_CAMERA_PREVIEW_QUERY_RESULT, &u4QueryResult,
                0
                );
            break;

        case CAL_SCENARIO_VIDEO:
            break;

        case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
            break;

        case CAL_SCENARIO_CAMERA_STILL_CAPTURE_MEM:
            break;
        default:
            configASSERT(0);
            break;
    }

    *((kal_uint32 *)pvResult) = u4QueryResult;

    return eRet;
}  // end of Idp_Cal_Query

MM_ERROR_CODE_ENUM Idp_Cal_If_Init(
    MM_ERROR_CODE_ENUM (*pfnCallback)(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen),
    IDP_CAL_INTF_FUNC_T *prIdpCalFuncPtrTbl)
{
    prIdpCalFuncPtrTbl->pfnIdpCalOpen    = Idp_Cal_Open;
    prIdpCalFuncPtrTbl->pfnIdpCalClose   = Idp_Cal_Close;
    prIdpCalFuncPtrTbl->pfnIdpCalStart   = Idp_Cal_Start;
    prIdpCalFuncPtrTbl->pfnIdpCalStop    = Idp_Cal_Stop;
    prIdpCalFuncPtrTbl->pfnIdpCalIsBusy  = NULL;
    prIdpCalFuncPtrTbl->pfnIdpCalIsInUse = (PFN_IDPCAL_IS_INUSE) Idp_Cal_Is_In_Use;
    prIdpCalFuncPtrTbl->pfnIdpCalSet     = Idp_Cal_Set;
    prIdpCalFuncPtrTbl->pfnIdpCalGet     = NULL;
    prIdpCalFuncPtrTbl->pfnIdpCalQuery   = (PFN_IDPCAL_QUERY_T) Idp_Cal_Query;

    pfIDP_CAL_CB = pfnCallback;
    return MM_ERROR_NONE;
}

#if defined(__MM_DCM_SUPPORT__)
#pragma arm section code, rodata
#endif

