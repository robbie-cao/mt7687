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

#ifndef __idp_cam_if_h__
#define __idp_cam_if_h__

//#include "drv_features_mdp.h"

//#include "img_common_enum.h"
#include "mm_comm_def.h"
#include "cal_comm_def.h"
#include "cal_api.h"
//#include "cal_if.h"
//#include "idp_core.h"

typedef enum
{
    IDP_CAL_SET_CONFIG = 0,
    IDP_CAL_SET_CRZ_FRAMESTART_CB_ENABLE,
    IDP_CAL_SET_ZSD_CAPTURE_TO_MEM_START,
    IDP_CAL_SET_ZSD_CAPTURE_TO_JPEG_START,
    IDP_CAL_SET_MAV_CAPTURE_ENABLE,
    IDP_CAL_SET_ZOOM_PARAM,
    IDP_CAL_SET_ENCODE_ROTATION,
    IDP_CAL_SET_REC_PATH_CTRL,
    IDP_CAL_SET_LOCK_BUFFER,    /**< Lock a MDP output buffer to prevent further hardware write. Return buffer address and rotated angle*/
    IDP_CAL_SET_UNLOCK_BUFFER,  /**< Unlock a MDP output buffer */
    IDP_CAL_SET_DISPLAY_UPDATE, /**< Unlock a MDP output buffer */
    IDP_CAL_SET_OVERRIDE_EMI_BW_TO_RAW_CAPTURE_MODE,
    IDP_CAL_SET_VT_SENSOR_FPS_VIDEO_FPS
} IDP_CAL_SET_TYPE_T;

typedef enum
{
    IDP_CAL_GET_DISPLAY_BUFFER = 0
} IDP_CAL_GET_TYPE_T;

typedef enum
{
    IDP_CAL_QUERY_PATH_SUPPORT = 0,
      /* Query format
       *           Description            Data Type
       * Input :   Path ID                IDP_CAL_QUERY_DATA_PATH_ENUM
       * Output:   Support(1) or Not(0)   kal_bool
       */
    IDP_CAL_QUERY_ROT_CAP,
      /* Query format
       *           Description               Data Type
       * Input :   Path ID                   IDP_CAL_QUERY_DATA_PATH_ENUM
       *           Color format              IDP_CAL_QUERY_POSTPROC_COLOR_FMT_CAP
       *           Desired rotation angle    img_rot_angle_enum_t
       * Output:   Suggested rotation angle  img_rot_angle_enum_t
       */
    //IDP_CAL_QUERY_DISP_COLOR_FMT_CAP
    //IDP_CAL_QUERY_POSTPROC_COLOR_FMT_CAP
    //IDP_CAL_QUERY_VIDEO_ENC_COLOR_FMT_CAP
    IDP_CAL_QUERY_FRAME_DONE_CB_FUNC
      /* Query format
       *         Description               Data Type
       * Input:  Path ID                   IDP_CAL_QUERY_DATA_PATH_ENUM
       * Output: pointer to IDP frame done callback function of specific path  void*
       */
} IDP_CAL_QUERY_TYPE_T;

typedef enum
{
    IDP_CAL_QUERY_ARG_DISPLAY_PATH = 0,
    IDP_CAL_QUERY_ARG_POSTPROC_PATH,            // including postproc and video encode paths
    IDP_CAL_QUERY_ARG_CAPTURE_MAIN_IMAGE_PATH,
    IDP_CAL_QUERY_ARG_QUICKVIEW_PATH,
    IDP_CAL_QUERY_ARG_THUMBNAIL_PATH
} IDP_CAL_QUERY_DATA_PATH_ENUM;

typedef enum
{
    IDP_CAL_DISPLAY_BUFFER_NOT_LOCKED = 0,
    IDP_CAL_DISPLAY_BUFFER_LOCKING,
    IDP_CAL_DISPLAY_BUFFER_LOCKED,
    IDP_CAL_DISPLAY_BUFFER_UNLOCKING
} IDP_CAL_DISPLAY_BUFFER_LOCK_STATUS_T;

typedef MM_ERROR_CODE_ENUM (*PFN_IDPCAL_OPEN_T)(CAL_SCENARIO_ENUM eCalScenID, kal_uint32 * const key, char const * const filename, kal_uint32 const lineno);
typedef MM_ERROR_CODE_ENUM (*PFN_IDPCAL_CLOSE_T)(CAL_SCENARIO_ENUM eCalScenID, kal_uint32 const key);
typedef MM_ERROR_CODE_ENUM (*PFN_IDPCAL_START_T)(CAL_SCENARIO_ENUM eCalScenID, kal_uint32 const key);
typedef MM_ERROR_CODE_ENUM (*PFN_IDPCAL_STOP_T)(CAL_SCENARIO_ENUM eCalScenID, kal_uint32 const key);
typedef MM_ERROR_CODE_ENUM (*PFN_IDPCAL_IS_BUSY)(CAL_SCENARIO_ENUM eCalScenID, kal_uint32 const key, void *pvParam);
typedef MM_ERROR_CODE_ENUM (*PFN_IDPCAL_IS_INUSE)(CAL_SCENARIO_ENUM eCalScenID);
typedef MM_ERROR_CODE_ENUM (*PFN_IDPCAL_SET_T)(CAL_SCENARIO_ENUM eCalScenID, kal_uint32 const key, IDP_CAL_SET_TYPE_T eSetType, void *pvParam);
typedef MM_ERROR_CODE_ENUM (*PFN_IDPCAL_GET_T)(CAL_SCENARIO_ENUM eCalScenID, kal_uint32 const key, IDP_CAL_GET_TYPE_T eGetType, void *pvParam);
typedef MM_ERROR_CODE_ENUM (*PFN_IDPCAL_QUERY_T)(CAL_SCENARIO_ENUM eCalScenID, kal_uint32 const key, IDP_CAL_QUERY_TYPE_T eQryType, void *pvArg, void *pvResult);

typedef struct
{
  IDP_CAL_QUERY_DATA_PATH_ENUM ePath;
  MM_IMAGE_COLOR_FORMAT_ENUM   eClrFmt;
  img_rot_angle_enum_t         eDesiredAngle;
  kal_uint32                   eRotateWidth;
} IDP_CAL_QUERY_ARG_T;

typedef struct
{
    PFN_IDPCAL_OPEN_T   pfnIdpCalOpen;
    PFN_IDPCAL_CLOSE_T  pfnIdpCalClose;
    PFN_IDPCAL_START_T  pfnIdpCalStart;
    PFN_IDPCAL_STOP_T   pfnIdpCalStop;
    PFN_IDPCAL_IS_BUSY  pfnIdpCalIsBusy;
    PFN_IDPCAL_IS_INUSE  pfnIdpCalIsInUse;
    PFN_IDPCAL_SET_T    pfnIdpCalSet;
    PFN_IDPCAL_GET_T    pfnIdpCalGet;
    PFN_IDPCAL_QUERY_T  pfnIdpCalQuery;
} IDP_CAL_INTF_FUNC_T;


typedef struct  // Camera_Preview_Idp_Cal_Struct;
{
    /* Basic IDP Setting */
    kal_uint32                  extmem_start_address;   /* external memroy start address for hardware engine buffer */
    kal_uint32                  extmem_size;            /* external memory size for hardware engine buffer */
    kal_uint16                  source_width;           /* camera source image width */
    kal_uint16                  source_height;          /* camera source image height */
    kal_uint16                  display_width;          /* target display image width */
    kal_uint16                  display_height;         /* target display image height */
    kal_uint32                  frame_buffer_address1;  /* IDP output buffer */
    kal_uint32                  frame_buffer_address2;  /* IDP output buffer */
    kal_uint32                  frame_buffer_address3;  /* IDP output buffer */
    kal_uint32                  frame_buffer_temp;  /* IDP output buffer */
    img_rot_angle_enum_t        display_rot_angle;
    MM_IMAGE_COLOR_FORMAT_ENUM  preview_image_color_format;
    kal_bool                    display_image_done_hisr_cb_enable;
    kal_bool                    preview_pitch_enable;
    kal_uint16                  preview_pitch_width;

    kal_bool                     mdp_crop_enable;
    kal_uint32                  cropped_width;
    kal_uint32                  cropped_height;

    /* Specific IDP Module Setting */
    kal_uint32                  overlay_palette_size;
    kal_uint32                  overlay_palette_addr;

    kal_bool                    enable_overlay;         /* KAL_TRUE or KAL_FALSE to enable or disable overlay function */
    kal_uint8                   overlay_color_depth;    /* The color depth of overlay frame buffer */
    kal_uint8                   overlay_frame_source_key;
    kal_uint16                  overlay_frame_width;    /* Image width of overlay frame buffer */
    kal_uint16                  overlay_frame_height;   /* Image height of overlay frame buffer */
    kal_uint32                  overlay_frame_buffer_address;    /* The starting address of overlay frame buffer */

    /* Extension Functionalities */
    /* Image for postprocessing functions: Face detection/Smile detection/AutoCapture Panorama */
    kal_bool                    postproc_image_enable;
    img_rot_angle_enum_t        postproc_image_rot_angle;
    MM_IMAGE_COLOR_FORMAT_ENUM  postproc_image_color_format;
    kal_uint32                  postproc_image_width;
    kal_uint32                  postproc_image_height;
    kal_uint32                  postproc_image_buffer_address_1;
    kal_uint32                  postproc_image_buffer_address_2;
    kal_uint32                  postproc_image_buffer_address_3;
    kal_bool                    postproc_image_done_hisr_cb_enable;

    /* Digital Zoom */
    kal_bool                    digital_zoom_frame_start_hisr_cb_enable;

    /* Output Setting */
    kal_bool                    lcd_update_enable;
    kal_bool                    trigger_display_delay_to_frame_start_enable;
    kal_bool                    dbuf_tearing_free_enable;
} Camera_Preview_Idp_Cal_Struct;

typedef struct  // Video_Encode_Idp_Cal_Struct
{
    /* Basic IDP Setting */
    kal_uint32                  extmem_start_address;   /* external memroy start address for hardware engine buffer */
    kal_uint32                  extmem_size;            /* external memory size for hardware engine buffer */
    kal_uint16                  source_width;           /* camera source image width */
    kal_uint16                  source_height;          /* camera source image height */
    kal_uint16                  display_width;          /* target display image width */
    kal_uint16                  display_height;         /* target display image height */
    kal_uint32                  frame_buffer_address1;  /* IDP output buffer */
    kal_uint32                  frame_buffer_address2;  /* IDP output buffer */
    kal_uint32                  frame_buffer_address3;  /* IDP output buffer */
    kal_uint32                  frame_buffer_temp;  /* IDP output buffer */
    img_rot_angle_enum_t        display_rot_angle;
    MM_IMAGE_COLOR_FORMAT_ENUM  preview_image_color_format;
    kal_bool                    preview_pitch_enable;
    kal_uint16                  preview_pitch_width;

    kal_bool                     mdp_crop_enable;
    kal_uint32                  cropped_width;
    kal_uint32                  cropped_height;

    /* Specific IDP Module Setting */
    kal_uint32                  overlay_palette_size;
    kal_uint32                  overlay_palette_addr;

    kal_bool                    enable_overlay;         /* KAL_TRUE or KAL_FALSE to enable or disable overlay function */
    kal_uint8                   overlay_color_depth;    /* The color depth of overlay frame buffer */
    kal_uint8                   overlay_frame_source_key;
    kal_uint16                  overlay_frame_width;    /* Image width of overlay frame buffer */
    kal_uint16                  overlay_frame_height;   /* Image height of overlay frame buffer */
    kal_uint32                  overlay_frame_buffer_address;    /* The starting address of overlay frame buffer */

    /* Extension Functionalities */
    /* Image for Video encode */
    img_rot_angle_enum_t        video_encode_image_rot_angle;
    MM_IMAGE_COLOR_FORMAT_ENUM  video_encode_image_color_format;
    kal_uint32                  video_encode_image_width;
    kal_uint32                  video_encode_image_height;

    kal_bool                    video_encode_frame_start_hisr_cb_enable;

    /* Output Setting */
    kal_bool                    lcd_update_enable;

    /* Special Setting */
    // MT6268
    kal_bool                    force_lcd_hw_trigger;

    /* for VT encode drop frame */
    kal_uint32                 sensor_fps;
    kal_uint32                 video_fps;

    kal_uint8                  quality_level;
    kal_bool                   trigger_display_delay_to_frame_start_enable;
} Video_Encode_Idp_Cal_Struct;

typedef struct  // Camera_Preview_Idp_Cal_Struct;
{
    /* Basic IDP Setting */
    kal_uint32                  extmem_start_address;   /* external memroy start address for hardware engine buffer */
    kal_uint32                  extmem_size;            /* external memory size for hardware engine buffer */
    kal_uint16                  source_width;           /* camera source image width */
    kal_uint16                  source_height;          /* camera source image height */
    kal_uint16                  display_width;          /* target display image width */
    kal_uint16                  display_height;         /* target display image height */
    kal_uint32                  frame_buffer_address1;  /* IDP output buffer */
    kal_uint32                  frame_buffer_address2;  /* IDP output buffer */
    img_rot_angle_enum_t        display_rot_angle;
    MM_IMAGE_COLOR_FORMAT_ENUM  preview_image_color_format;
    kal_bool                    display_image_done_hisr_cb_enable;
    kal_bool                    preview_pitch_enable;
    kal_uint16                  preview_pitch_width;

    kal_uint8                   mjpeg_yuv_mode; /* YUV420, YUV422 or Gray mode*/
    kal_uint32                  mjpeg_encode_image_width;
    kal_uint32                  mjpeg_encode_image_height;

    kal_bool                     mdp_crop_enable;
    kal_uint32                  cropped_width;
    kal_uint32                  cropped_height;

    /* Specific IDP Module Setting */
    kal_uint32                  overlay_palette_size;
    kal_uint32                  overlay_palette_addr;

    kal_bool                    enable_overlay;         /* KAL_TRUE or KAL_FALSE to enable or disable overlay function */
    kal_uint8                   overlay_color_depth;    /* The color depth of overlay frame buffer */
    kal_uint8                   overlay_frame_source_key;
    kal_uint16                  overlay_frame_width;    /* Image width of overlay frame buffer */
    kal_uint16                  overlay_frame_height;   /* Image height of overlay frame buffer */
    kal_uint32                  overlay_frame_buffer_address;    /* The starting address of overlay frame buffer */

    /* Digital Zoom */
    kal_bool                    video_encode_frame_start_hisr_cb_enable;

    /* Output Setting */
    kal_bool                    lcd_update_enable;
    
    kal_bool                    trigger_display_delay_to_frame_start_enable; 
} Mjpeg_Encode_Idp_Cal_Struct;

typedef struct  // Camera_Capture_Idp_Cal_Struct
{
    /* Basic IDP Setting */
    kal_uint32                  extmem_start_address;   /* external memroy start address for hardware engine buffer */
    kal_uint32                  extmem_size;            /* external memory size for hardware engine buffer */
    kal_uint16                  source_width;           /* camera source image width */
    kal_uint16                  source_height;          /* camera source image height */
    kal_uint32                  resizer_u_value;        /* CRZ */
    kal_uint32                  resizer_d_value;        /* CRZ */

    kal_bool                     mdp_crop_enable;
    kal_uint32                  cropped_width;
    kal_uint32                  cropped_height;

    /* Specific IDP Module Setting */
    kal_uint32                  overlay_palette_size;
    kal_uint32                  overlay_palette_addr;

    kal_bool                    enable_overlay;         /* KAL_TRUE or KAL_FALSE to enable or disable overlay function */
    kal_uint8                   overlay_color_depth;    /* The color depth of overlay frame buffer */
    kal_uint8                   overlay_frame_source_key;
    kal_uint16                  overlay_frame_width;    /* Image width of overlay frame buffer */
    kal_uint16                  overlay_frame_height;   /* Image height of overlay frame buffer */
    kal_uint32                  overlay_frame_buffer_address;    /* The starting address of overlay frame buffer */

    /* Extension Functionalities */
    /* Full size image is the image for JPEG-encoding, either by hardware encoder or later-stage software encoder */
    img_rot_angle_enum_t        fullsize_image_rot_angle;
    MM_IMAGE_FORMAT_ENUM        fullsize_image_format;
    kal_uint16                  fullsize_image_width;
    kal_uint16                  fullsize_image_height;
    kal_uint32                  fullsize_image_buffer_y_addr;
    kal_uint32                  fullsize_image_buffer_u_addr;
    kal_uint32                  fullsize_image_buffer_v_addr;
    kal_uint8                    fullsize_image_jpeg_yuv_mode; /* YUV420, YUV422 or Gray mode*/

    /* Quickview image is of display size */
    kal_bool                    quickview_image_enable;
    img_rot_angle_enum_t        quickview_image_rot_angle;
    MM_IMAGE_COLOR_FORMAT_ENUM  quickview_image_color_format;
    kal_uint16                  quickview_image_width;
    kal_uint16                  quickview_image_height;
    kal_uint32                  quickview_image_buffer_addr;
    kal_bool                    quickview_pitch_enable;
    kal_uint16                  quickview_pitch_width;

    /* Thumbnail image */
    kal_bool                    thumbnail_image_enable;
    img_rot_angle_enum_t        thumbnail_image_rot_angle;
    MM_IMAGE_COLOR_FORMAT_ENUM  thumbnail_image_color_format;
    kal_uint16                  thumbnail_image_width;
    kal_uint16                  thumbnail_image_height;
    kal_uint32                  thumbnail_image_buffer_addr;
} Camera_Capture_Idp_Cal_Struct;

typedef void (*pCropBufferCallback_t)(void*);

typedef struct  // Camera_Capture_Idp_Cal_Struct
{
  MM_IMAGE_FORMAT_ENUM ImageBuffFormat;         
  MM_ERROR_CODE_ENUM ErrorStatus;               
  kal_uint32 ImageBuffYAddr;
  kal_uint32 ImageBuffUAddr;
  kal_uint32 ImageBuffVAddr;
  kal_uint32 ImageBuffWidth;
  kal_uint32 ImageBuffHeight;
  pCropBufferCallback_t CropBufferCbfn;    
} Camera_Capture_Crop_Buffer_Struct;


typedef struct  // Digital_Zoom_Crop_Ctrl_Struct;
{
    kal_uint32 updated_source_width;
    kal_uint32 updated_source_height;
    kal_bool mdp_crop_enable;
    kal_uint32 cropped_width;
    kal_uint32 cropped_height;
} Digital_Zoom_Crop_Ctrl_Struct;

typedef struct  // VT encode drop frame
{
    kal_uint32                 sensor_fps;
    kal_uint32                 video_fps;
} VT_Sensor_Fps_Video_Fps_Info_Struct;

typedef struct
{
    kal_uint32                  buffer_start_address;
    kal_uint16                  buffer_width;
    kal_uint16                  buffer_height;
    img_rot_angle_enum_t        buffer_rot_angle;
    MM_IMAGE_COLOR_FORMAT_ENUM  buffer_color_format;
} IDP_ROTATING_BUFFER_DESCRIPTOR_T;

typedef struct
{
    kal_uint32                  u4CompletedBufAdr;
    img_rot_angle_enum_t        eBufRotatedAngle;
    MM_IMAGE_COLOR_FORMAT_ENUM  eBufColorFormat;
} IDPCAL_CB_ARG_FRAME_DONE;

extern MM_ERROR_CODE_ENUM Idp_Cal_If_Init(
    MM_ERROR_CODE_ENUM (*pfnCallback)(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen),
    IDP_CAL_INTF_FUNC_T *prIdpCalFuncPtrTbl);

extern MM_ERROR_CODE_ENUM (*pfIDP_CAL_CB)
    (CAL_CALLBACK_ID_ENUM CbId,
     void *pCallbackPara,
     kal_uint16 CallbackParaLen);

#endif //__idp_cam_if_h__
