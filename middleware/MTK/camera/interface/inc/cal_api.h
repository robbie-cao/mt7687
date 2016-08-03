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

#ifndef __CAL_API_H__
#define __CAL_API_H__

#if 0
#include "fsal.h"
#include "mm_comm_def.h"
#include "cal_comm_def.h"
#include "idp_cam_if.h"
#include "usbvideo_custom.h"
#include "pp_comm_def.h"
#include "cal_feature_option.h"
#include "mpl_recorder.h"
#endif

#include "mm_comm_def.h"
#include "cal_comm_def.h"
//#include "idp_cam_if.h"
#include "cal_feature_option.h"

typedef struct
{
    kal_uint32 WfcElementHandle;
    kal_uint32 WfcDeviceHandle;
    kal_uint32 WfcContextHandle;
} CAL_LCD_STRUCT, *P_CAL_LCD_STRUCT;

typedef struct
{
    kal_uint8   ZoomStep;                           /* digital zoom step */

    CAL_CAMERA_CONTRAST_ENUM   ContrastLevel;       /* ISP contrast gain level */
    CAL_CAMERA_SATURATION_ENUM  SaturationLevel;    /* image processor saturation value */
    CAL_CAMERA_SHARPNESS_ENUM   SharpnessLevel;
    CAL_CAMERA_WB_ENUM   WbMode;                    /* white balance mode */
    CAL_CAMERA_EV_ENUM   EvValue;                   /* EV adjust value */
    CAL_CAMERA_BANDING_FREQ_ENUM   BandingFreq;     /* Banding frequency selection */
    CAL_CAMERA_IMAGE_EFFECT_ENUM   ImageEffect;     /* image effect for preview */

    CAL_CAMERA_SCENE_MODE_ENUM SceneMode;
    CAL_CAMERA_AE_METERING_MODE_ENUM AeMeteringMode;
    CAL_CAMERA_FLASH_ENUM   FlashMode;
    CAL_CAMERA_AF_OPERATION_MODE_ENUM AfOperationMode;
    CAL_CAMERA_AF_RANGE_ENUM AfRange;

    kal_uint8   BrightnessLevel;                    /* ISP brightness level, for YUV sensor only */
    kal_uint16  HueValue;                            /* image processor hue value, for YUV sensor only */
} CAL_ISP_PREVIEW_STRUCT, *P_CAL_ISP_PREVIEW_STRUCT;

typedef struct
{
    kal_uint32 PostProcMode;            //[0]:PP_MODE_FACE_DETECT, [1]:PP_MODE_SMILE_DETECT, [2]:PP_MODE_BLINK_DETECT
                                        //[3]:PP_MODE_PANORAMA, [4]: PP_MODE_AUTOCAP_PANORAMA, [5]:PP_MODE_SMART_CONTRAST
                                        // 0: disable all
} CAL_POSTPROC_STRUCT, *P_CAL_POSTPROC_STRUCT;

typedef enum
{
   /* must exactly mapped to MM_IMAGE_ROTATE_ENUM in mm_comm_def.h. 
      Otherwise, the callback functions should convert the rotation angle argument */   
    IMG_ROT_ANGLE_0   /**< clock-wise 0 degree. */
  , IMG_ROT_ANGLE_90  /**< clock-wise 90 degree. */
  , IMG_ROT_ANGLE_180 /**< clock-wise 180 degree. */
  , IMG_ROT_ANGLE_270 /**< clock-wise 270 degree. */
  , IMG_MIRROR_ROT_ANGLE_0
  , IMG_MIRROR_ROT_ANGLE_90   /**< clock-wise */
  , IMG_MIRROR_ROT_ANGLE_180  /**< clock-wise */
  , IMG_MIRROR_ROT_ANGLE_270  /**< clock-wise */
}img_rot_angle_enum_t;

typedef struct
{
    MM_IMAGE_ROTATE_ENUM ImageRotAngle;
    MM_IMAGE_ROTATE_ENUM StillImageRotAngle;    /* for overlap frame buffer rotation in Pluto MMI*/
    MM_IMAGE_FORMAT_ENUM ImageDataFormat;

    kal_uint16  DisplayImageWidth;          /* the width of target image */
    kal_uint16  DisplayImageHeight;         /* the height of target image */
    kal_uint32  FrameBuffAddr1;             /* for LCD buffer update */
    kal_uint32  FrameBuffAddr2;             /* for LCD buffer update */
    kal_uint32  FrameBuffAddr3;             /* for LCD buffer update */
    kal_uint32  FrameBuffAddrTemp;	
    kal_uint32  CameraAppFrameBuffAddr;     /* Frame buffer addr that comes from Camera AP for ZSD */
    kal_uint32  FrameBuffSize;              /* the frame buffer size of image DMA output for LCD fb updaet */

    kal_bool    OverlayEnable;
    kal_uint32  OverlayPaletteAddr;
    kal_uint32  OverlayPaletteSize;
    kal_uint8   OverlayColorDepth;          /* The color depth of overlay frame buffer */
    kal_uint8   OverlayFrameSourceKey;
    kal_uint16  OverlayFrameWidth;          /* Image width of overlay frame buffer */
    kal_uint16  OverlayFrameHeight;         /* Image height of overlay frame buffer */
    kal_uint32  OverlayFrameBuffAddr;       /* The starting address of overlay frame buffer */

    kal_bool LcdUpdateEnable;
} CAL_MDP_PREVIEW_STRUCT, *P_CAL_MDP_PREVIEW_STRUCT;

// data structure of CAL_SCENARIO_CAMERA_PREVIEW input parameters
typedef struct
{
    kal_uint32 	ExtmemStartAddress;		/* external memroy start address for hardware engine buffer */
    kal_uint32 	ExtmemSize;				/* external memory size for hardware engine buffer */

    P_CAL_LCD_STRUCT pLcdPara;
    P_CAL_MDP_PREVIEW_STRUCT pMdpPara;
    P_CAL_ISP_PREVIEW_STRUCT pIspPara;
    P_CAL_POSTPROC_STRUCT pPostProcPara;
} CAL_CAMERA_PREVIEW_STRUCT, *P_CAL_CAMERA_PREVIEW_STRUCT;

#if 0
// data structure of CAL_SCENARIO_CAMERA_STILL_CAPTURE input parameters
typedef struct
{
    JPEG_ENCODE_QUALITTY_ENUM JpegEncQuality;
    MM_IMAGE_FORMAT_ENUM JpegSamplingFormat;
    kal_uint32 JpegBuffAddr;
    kal_uint32 JpegBuffSize;

    // for direct output to file. If the file handle is NULL, it will output to the destination buffer
    STFSAL *pDstFileHandle;
} CAL_JPEG_ENC_STRUCT, *P_CAL_JPEG_ENC_STRUCT;
#endif

typedef struct
{
    kal_bool    BinningMode;                    // enable/disable binning mode
    kal_uint8   ExpoBracketLevel;
    kal_bool    RawCaptureEnable;
    kal_uint32  RawImageBuffAddr;
    kal_uint32  RawImageBuffSize;

    MULTISHOT_INFO_ENUM MultishotInfo;
    CAL_CAMERA_STILL_CAPTURE_ENUM StillCaptureMode;
} CAL_ISP_STILL_CAPTURE_STRUCT, *P_CAL_ISP_STILL_CAPTURE_STRUCT;

typedef struct
{
    kal_bool    OverlayEnable;
    kal_uint8   OverlayColorDepth;          /* The color depth of overlay frame buffer */
    kal_uint8   OverlayFrameSourceKey;
    kal_uint16  OverlayFrameWidth;          /* Image width of overlay frame buffer */
    kal_uint16  OverlayFrameHeight;         /* Image height of overlay frame buffer */
    kal_uint32  OverlayPaletteAddr;
    kal_uint32  OverlayPaletteSize;
    kal_uint32  OverlayFrameBuffAddr;       /* The starting address of overlay frame buffer */

    MM_IMAGE_FORMAT_ENUM MainImageDataFormat;
    MM_IMAGE_ROTATE_ENUM MainImageRotAngle;
    MM_IMAGE_FORMAT_ENUM MainImageJpegYuvSampleFormat;
    kal_uint16 MainImageWidth;
    kal_uint16 MainImageHeight;
    kal_uint32 MainImageBuffAddr;
    kal_uint32 MainImageBuffSize;
    kal_uint32 MainImageWorkingBuffAddr;
    kal_uint32 MainImageWorkingBuffSize;

#if defined(__CAMERA_CAPTURE_USING_APP_MEMORY__)
    kal_uint32 MainImageYBuffAddr;
    kal_uint32 MainImageYBuffSize;
    kal_uint32 MainImageUBuffAddr;
    kal_uint32 MainImageUBuffSize;
    kal_uint32 MainImageVBuffAddr;
    kal_uint32 MainImageVBuffSize;
    kal_bool   MainImageYUVEnagle;
#endif

    kal_bool   QuickViewImageEnable;
    MM_IMAGE_FORMAT_ENUM QuickViewImageFormat;
    MM_IMAGE_ROTATE_ENUM QuickViewImageRotAngle;
    MM_IMAGE_ROTATE_ENUM QuickViewImageLCDRotAngle;
    kal_uint16 QuickViewImageWidth;
    kal_uint16 QuickViewImageHeight;
    kal_uint32 QuickViewImageBuffAddr;
    kal_uint32 QuickViewImageBuffSize;

    kal_bool   ThumbnailImageEnable;
    MM_IMAGE_FORMAT_ENUM ThumbnailImageFormat;
    MM_IMAGE_ROTATE_ENUM ThumbnailImageRotAngle;
    kal_uint16 ThumbnailImageWidth;
    kal_uint16 ThumbnailImageHeight;
    kal_uint32 ThumbnailImageBuffAddr;
    kal_uint32 ThumbnailImageBuffSize;

    kal_bool   ImageJaiaDisable;

    kal_bool LcdUpdateEnable;
} CAL_MDP_STILL_CAPTURE_STRUCT, *P_CAL_MDP_STILL_CAPTURE_STRUCT;

#if 0
typedef struct
{
    kal_uint32 	ExtmemStartAddress;		    /* external memroy start address for hardware engine buffer */
    kal_uint32 	ExtmemSize;				    /* external memory size for hardware engine buffer */

    P_CAL_JPEG_ENC_STRUCT pJpegEncPara;     /* if the capture mode is not JPEG, these parameters is not necessary */
    P_CAL_MDP_STILL_CAPTURE_STRUCT pMdpPara;
    P_CAL_ISP_STILL_CAPTURE_STRUCT pIspPara;
    P_CAL_POSTPROC_STRUCT pPostProcPara;
} CAL_CAMERA_STILL_CAPTURE_STRUCT, *P_CAL_CAMERA_STILL_CAPTURE_STRUCT;
#endif

// data structure of CAL_SCENARIO_VIDEO input parameters
typedef struct
{
    kal_uint8   ZoomStep;                           /* digital zoom step */
    kal_uint16  MaxVideoFrameRate;                  /* maximum allowable video frame rate according to video encoder, quality, storage type */

    CAL_CAMERA_CONTRAST_ENUM   ContrastLevel;       /* ISP contrast gain level */
    CAL_CAMERA_SATURATION_ENUM  SaturationLevel;    /* image processor saturation value */
    CAL_CAMERA_SHARPNESS_ENUM   SharpnessLevel;
    CAL_CAMERA_WB_ENUM   WbMode;                    /* white balance mode */
    CAL_CAMERA_EV_ENUM   EvValue;                   /* EV adjust value */
    CAL_CAMERA_BANDING_FREQ_ENUM   BandingFreq;     /* Banding frequency selection */
    CAL_CAMERA_IMAGE_EFFECT_ENUM   ImageEffect;     /* image effect for preview */

    CAL_CAMERA_VIDEO_SCENE_MODE_ENUM VideoSceneMode;
    CAL_CAMERA_AF_OPERATION_MODE_ENUM AfOperationMode;
    CAL_CAMERA_AF_RANGE_ENUM AfRange;

    kal_uint8  BrightnessLevel;                     /* ISP brightness level, for YUV sensor only */
    kal_uint16 HueValue;                            /* image processor hue value, for YUV sensor only */
} CAL_ISP_VIDEO_STRUCT, *P_CAL_ISP_VIDEO_STRUCT;

#if 0
typedef struct
{
    MM_IMAGE_ROTATE_ENUM DisplayImageRotAngle;	    /* for sw rotation calling by MDP */
    MM_IMAGE_FORMAT_ENUM DisplayImageDataFormat;

    kal_uint16  VtEncodeFrameRate;          /* VT encode frame rate that MDP has to pass to VAL */

    kal_uint16  DisplayImageWidth;          /* the width of target image */
    kal_uint16  DisplayImageHeight;         /* the height of target image */
    kal_uint32  FrameBuffAddr1;             /* for LCD buffer update */
    kal_uint32  FrameBuffAddr2;             /* for LCD buffer update */
    kal_uint32  FrameBuffAddr3;             /* for LCD buffer update */
    kal_uint32  FrameBuffAddrTemp;          /* for LCD buffer update */
    kal_uint32  FrameBuffSize;              /* the frame buffer size of image DMA output for LCD fb updaet */

    kal_bool    VideoEncodeEnable;
    kal_uint8  VideoImageQuality;
    MM_IMAGE_ROTATE_ENUM VideoImageRotAngle;
    MM_IMAGE_FORMAT_ENUM VideoImageDataFormat;
    kal_uint16  VideoImageWidth;
    kal_uint16  VideoImageHeight;

    kal_bool LcdUpdateEnable;
    kal_bool ForceLcdHwTrigger;

    MPL_BUFFER_INFO_T (*pfGetVideoBuff)(void);
    void (*pfRetVideoBuff)(MPL_BUFFER_INFO_T BuffInfo);
} CAL_MDP_VIDEO_STRUCT, *P_CAL_MDP_VIDEO_STRUCT;

typedef struct
{
    kal_uint32 	ExtmemStartAddress;		/* external memroy start address for hardware engine buffer */
    kal_uint32 	ExtmemSize;				/* external memory size for hardware engine buffer */

    P_CAL_LCD_STRUCT pLcdPara;
    P_CAL_ISP_VIDEO_STRUCT pIspPara;
    P_CAL_MDP_VIDEO_STRUCT pMdpPara;
} CAL_CAMERA_VIDEO_RECORD_STRUCT, *P_CAL_CAMERA_VIDEO_RECORD_STRUCT;

typedef struct
{
	kal_uint32 	ExtmemStartAddress;	/* external memroy start address for hardware engine buffer */
	kal_uint32 	ExtmemSize;				/* external memory size for hardware engine buffer */
	kal_uint32 	ExtmemStartAddress2;	/* external memroy start address for jpeg buffer */

	// isp webcam
	USBVIDEO_VIDEO_SIZE_TYPE TargetSize;	/* FROM Media Task, the size index of target image */
	USBVIDEO_COMPRESSION_TYPE TargetCompression;	/* FROM Media Task, the comp Q index of target image */
	MM_IMAGE_ROTATE_ENUM   ImageRotate;					/* FROM Media Task, rotate value */
}CAL_CAMERA_WEBCAM_PROCESS_STRUCT, *P_CAL_CAMERA_WEBCAM_PROCESS_STRUCT;
#endif

typedef struct
{    
    /* HDR only */
    kal_uint16 MainImageWidth;
    kal_uint16 MainImageHeight;
    /* HDR only */ 

    kal_bool   QuickViewImageEnable;
    MM_IMAGE_FORMAT_ENUM QuickViewImageFormat;
    MM_IMAGE_ROTATE_ENUM QuickViewImageRotAngle;
    kal_uint16 QuickViewImageWidth;
    kal_uint16 QuickViewImageHeight;
    kal_uint32 QuickViewImageBuffAddr;
    
    /* HDR only */
    kal_bool   ThumbnailImageEnable;
    MM_IMAGE_FORMAT_ENUM ThumbnailImageFormat;
    MM_IMAGE_ROTATE_ENUM ThumbnailImageRotAngle;
    kal_uint16 ThumbnailImageWidth;
    kal_uint16 ThumbnailImageHeight;
    kal_uint32 ThumbnailImageBuffAddr;  
    /* HDR only */ 
    
    /* AutoRama only */
    kal_uint32 QuickViewImageBuffSize;
    kal_bool   QuickViewImageResizeByBuffSizeEnable;
    /* AutoRama only */
} CAL_MDP_POSTPROC_STRUCT, *P_CAL_MDP_POSTPROC_STRUCT;

typedef struct
{
    kal_uint32 ExtmemStartAddress;  /* external memroy start address for hardware engine buffer */
    kal_uint32 ExtmemSize;	        /* external memory size for hardware engine buffer */
    
    /* HDR only */
    MM_IMAGE_FORMAT_ENUM SrcImageFormat;
    kal_uint32 SrcImageAddr;
    kal_uint32 SrcImageSize;
    /* HDR only */ 

    kal_uint32 JpegBuffAddr;                                   // dst jpeg info
    kal_uint32 JpegBuffSize;                                    // dst jpeg info
    P_CAL_MDP_POSTPROC_STRUCT pMdpPara;
    P_CAL_POSTPROC_STRUCT pPostProcPara;
} CAL_CAMERA_POSTPROC_STRUCT, *P_CAL_CAMERA_POSTPROC_STRUCT;

// for CAL_SCENARIO_CTRL
typedef struct
{
    CAL_SCENARIO_ENUM ScenarioId;
    kal_uint32 ScenarioCtrlCode;
    kal_uint32 ScenarioCtrlParaLen;
    void *pScenarioCtrlPara;
} CAL_SCENARIO_CTRL_STRUCT, *P_CAL_SCENARIO_CTRL_STRUCT;

// for CAL_FEATURE_CTRL
typedef struct
{
    kal_uint32 FeatureId;
    CAL_CAMERA_MODE_ENUM CameraMode;
    CAL_FEATURE_OPERATION_ENUM FeatureCtrlCode;
    CAL_FEATURE_TYPE_ENUM FeatureType;
    CAL_FEATURE_TYPE_UNION FeatureInfo;	        /* pointer to the feature characteristic in GET operation */
    kal_uint32 FeatureSetValue;					/* assign new value when for SET operation */
    kal_uint32 FeatureCurrentValue;             /* current feature value for GET operation */
    kal_uint32 FeatureOperationSupport;		/* combination of CAL_FEATURE_GET_SUPPORT,
	                                           CAL_FEATURE_SET_SUPPORT, CAL_FEATURE_GET_SET_SUPPORT,
	                                           to identify wheather the feature is supported by the driver or not*/
    kal_uint32 FeatureModeSupport;              /* CAL_FEATURE_CAMERA_SUPPORT, CAL_FEATURE_VIDEO_SUPPORT,
	                                           CAL_FEATURE_CAMERA_VIDEO_SUPPORT or CAL_FEATURE_NOT_SUPPORT
	                                           to identify wheather the feature is supported by the driver in specific mode*/
} CAL_FEATURE_CTRL_STRUCT, *P_CAL_FEATURE_CTRL_STRUCT;

// CAL interface API
MM_ERROR_CODE_ENUM CalCameraPowerDownReq(void);
MM_ERROR_CODE_ENUM CalCameraPreviewReq(void);
MM_ERROR_CODE_ENUM AtCmdCalCameraPreviewReq(void);
void CalInit(void);
MM_ERROR_CODE_ENUM CalOpen(void);
MM_ERROR_CODE_ENUM CalCtrl(CAL_CTRL_ENUM CalCtrl_Num, void *pCalParaIn, void *pCalParaOut, kal_uint32 CalParaOutLen, kal_uint32 *pRealCalParaOutLen);
MM_ERROR_CODE_ENUM CalClose(void);

// Callback function for ISP, MDP and JPEG
//MM_ERROR_CODE_ENUM PpCallback(PP_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM IspCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM MdpCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM JpegCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
#endif /* __CAL_API_H__ */

