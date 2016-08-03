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

#ifndef __ISP_COMM_H__
#define __ISP_COMM_H__

#include "cal_api.h"
#include "cal_comm_def.h"

typedef enum
{
	ISP_IDLE_STATE=0,
	ISP_STANDBY_STATE,
	ISP_PREVIEW_STATE,
	ISP_CAPTURE_RAW_STATE,
	ISP_CAPTURE_JPEG_STATE,
	ISP_PREVIEW_WEBCAM_STATE,
	ISP_CAPTURE_WEBCAM_STATE,
	ISP_MPEG4_ENCODE_STATE,
	ISP_MJPEG_ENCODE_STATE,
	ISP_MPEG4_DECODE_STATE
} ISP_OPERATION_STATE_ENUM;


typedef enum
{
   	ISP_DRIVING_2MA=0,
   	ISP_DRIVING_4MA,
	ISP_DRIVING_6MA,
	ISP_DRIVING_8MA,
	ISP_DRIVING_0MA,
	ISP_DRIVING_NO
} ISP_DRIVING_CURRENT_ENUM;

typedef enum
{
    ISP_FEATURE_BEGIN=CAL_ISP_FEATURE_BEGIN,
    ISP_FEATURE_INIT_HW,                // In : IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT (defined in sensor comm header file), Out : NULL
    ISP_FEATURE_DEINIT_HW,              // In : IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT (defined in sensor comm header file), Out : NULL
    ISP_FEATURE_SET_TG,                 // In : ISP_FEATURE_SET_TG_STRUCT, Out : NULL
    ISP_FEATURE_SET_CUSTOM_DRIVER_FEATURE_CTRL_FUNC,    // In : ISP_FEATURE_CUSTOM_DRIVER_FEATURE_CTRL_FUN_STRUCT, Out : NULL
    ISP_FEATURE_GET_NVRAM_INFO,         // In : CAL_SENSOR_SUPPORT_STRUCT, Out : ISP_FEATURE_NVRAM_INFO_STRUCT
    ISP_FEATURE_SET_NVRAM_DATA,         // In : ISP_FEATURE_NVRAM_DATA_STRUCT, Out : NULL
    ISP_FEATURE_GET_NVRAM_DATA,         // In : ISP_FEATURE_NVRAM_DATA_STRUCT, Out : ISP_FEATURE_NVRAM_DATA_STRUCT
    ISP_FEATURE_GET_OUTPUT_RESOLUTION,  // In : ISP_FEATURE_GET_OUTPUT_RESOLUTION_IN_STRUCT, Out : ISP_FEATURE_GET_OUTPUT_RESOLUTION_OUT_STRUCT, calculat the ISP output resolution according to sensor input resolution, digital zoom facotr and target resolution
    ISP_FEATURE_GET_CURRENT_ZOOM_INFO,      // In : NULL, Out : ISP_FEATURE_CURRENT_ZOOM_INFO_STRUCT
    ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION,// In : ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_IN_STRUCT, Out : ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_IN_STRUCT
    ISP_FEATURE_SET_FACE_INFO,			//  In : AAA_FACE_INFO_STRUCT, Out : NULL
    ISP_FEATURE_SET_AE_BYPASS,              // In: , Out: NULL
    ISP_FEATURE_SET_AWB_BYPASS,             //In: , Out: NULL
    ISP_FEATURE_SET_AE_LIMIT_ENABLE,        //In: , Out: NULL
    ISP_FEATURE_GET_CAPTURE_EV_IDX,         //In: , Out: 
    ISP_FEATURE_SET_AF_BYPASS,             //In: , Out: NULL
    ISP_FEATURE_RESET_AF,                   //In: , Out: NULL
    ISP_FEATURE_SET_CAPTURE_FORMAT,
    ISP_FEATURE_CONFIG_3A_BEFORE_SENSOR_SWITCH_CAPTURE,  //In:ISP_FEATURE_CONFIG_3A_BEFORE_CAPTURE_STRUCT, Out: NULL  
    ISP_FEATURE_STOP_FLASHLIGHT_PROCESS_AFTER_CAPTURE_DONE, // In: , Out: NULL
    ISP_FEATURE_DISABLE_VIEWFINDER, //In: , Out: NULL
    ISP_FEATURE_UPDATE_EXIF_INFO  ,   //In:ISP_CAMERA_UPDATE_EXIF_INFO_STRUCT , Out: NULL
    ISP_FEATURE_SET_3A_REF, // In: SENSOR_AE_AWB_REF_STRUCT, Out: NULL //__AUTO_SCENE_DETECT_SUPPORT__
    ISP_FEATURE_GET_3A_REF,// In: NULL, Out: ISP_CAMERA_AWB_AF_REF_STRUCT //__AUTO_SCENE_DETECT_SUPPORT__
    ISP_FEATURE_GET_3A_INFO,// In: NULL, Out:ISP_CAMERA_INFO_3A_STRUCT //__AUTO_SCENE_DETECT_SUPPORT__
    ISP_FEATURE_SET_SCENE_COMPENSATION, // In: ISP_CAMERA_SCENE_COMPENSATION_STRUCT, Out: NULL //__AUTO_SCENE_DETECT_SUPPORT__
    ISP_FEASTURE_MAX,
} ISP_FEATURE_ENUM;

typedef enum
{
	ISP_RAW_SUPPORT=0,
	ISP_YUV_SUPPORT,
	ISP_SUPPORT_END
} ISP_DATA_FORMAT_ENUM;

typedef enum
{
    ISP_PROCESS_DATA_8_BIT=0,
    ISP_PROCESS_DATA_10_BIT
}ISP_BIT_DEPTH_ENUM;

#if defined (__AUTO_SCENE_DETECT_SUPPORT__)
typedef enum
{
    ISP_ASD_COMP_NIGHT_MODE_ON=0,
    ISP_ASD_COMP_NIGHT_MODE_OFF,		
    ISP_ASD_COMP_SKIN_COLOR_ENHANCE,
    ISP_ASD_COMP_FACE_EV_BIOS,
    ISP_ASD_COMP_SCENE_ENHANCE,    
    ISP_ASD_COMP_MAX,	
}ISP_ASD_COMPENSATION_ENUM;
#endif

typedef struct
{
    ISP_BIT_DEPTH_ENUM CameraPreviewBitDepth;
    ISP_BIT_DEPTH_ENUM CameraCaptureBitDepth;
}ISP_PROCESS_DATA_BIT_DEPTH_STRUCT, *P_ISP_PROCESS_DATA_BIT_DEPTH_STRUCT;

typedef struct
{
    kal_uint16 ImageWidth;
    kal_uint16 ImageHeight;
} ISP_CAMERA_PREVIEW_PARA_OUT_STRUCT, *P_ISP_CAMERA_PREVIEW_PARA_OUT_STRUCT;

typedef struct
{
    kal_uint32 ScenarioCtrlCode;
    kal_uint32 ScenarioCtrlParaLen;
    void *pScenarioCtrlPara;
} ISP_SCENARIO_CTRL_STRUCT, *P_ISP_SCENARIO_CTRL_STRUCT;

// for ISP_FEATURE_GET_NVRAM_INFO
typedef struct
{
    kal_bool SensorSupport[4];  // notify ISP which sensor will be supported,
                                // IMAGE_SENSOR_MAIN/IMAGE_SENSOR_SUB/IMAGE_SENSOR_BAK1/IMAGE_SENSOR_SUB/BAK1
} CAL_SENSOR_SUPPORT_STRUCT, *P_CAL_SENSOR_SUPPORT_STRUCT;

typedef struct
{
    kal_uint32 CameraNvramStartFileLid[4];  // the
    kal_uint32 CameraNvramEndFileLid[4];    // query result
} ISP_FEATURE_NVRAM_INFO_STRUCT, *P_ISP_FEATURE_NVRAM_INFO_STRUCT;

// for ISP_FEATURE_SET_NVRAM_DATA
// ISP_FEATURE_GET_NVRAM_DATA
typedef struct
{
    kal_uint32 CameraNvramFileLid;            // set para
    void *pCameraNvramFileData;               // set para
    kal_uint32 CameraNvramDataSize;
} ISP_FEATURE_NVRAM_DATA_STRUCT, *P_ISP_FEATURE_NVRAM_DATA_STRUCT;

// for ISP_FEATURE_GET_OUTPUT_RESOLUTION
typedef struct
{
    ISP_OPERATION_STATE_ENUM IspState;
    kal_bool MetaModeFlag;
    kal_bool   BinningMode;    			   // for capture mode only 
    kal_bool   RawCaptureEnable;
	
    kal_uint8  ZoomStep;
    kal_uint8  TotalZoomStep;
    kal_uint16 MaxZoomFactor;
    kal_uint16 SensorImageGrabStartX;
    kal_uint16 SensorImageGrabStartY;
    kal_uint16 SensorImageWidth;
    kal_uint16 SensorImageHeight;
    kal_uint16 TargetWidth;
    kal_uint16 TargetHeight;
    kal_uint8 WaitStableFrameNum;

    CAL_CAMERA_DIGITAL_ZOOM_TYPE_ENUM DzType;
    const kal_uint16 *pZoomFactorListTable;
} ISP_FEATURE_GET_OUTPUT_RESOLUTION_IN_STRUCT, *P_ISP_FEATURE_GET_OUTPUT_RESOLUTION_IN_STRUCT;

typedef struct
{
    kal_uint16 IspOutWidth;
    kal_uint16 IspOutHeight;
} ISP_FEATURE_GET_OUTPUT_RESOLUTION_OUT_STRUCT, *P_ISP_FEATURE_GET_OUTPUT_RESOLUTION_OUT_STRUCT;

// for ISP_FEATURE_GET_CURRENT_ZOOM_INFO
typedef struct
{
    kal_uint8   CurrentZoomStep;
    kal_uint8   TotalZoomStep;
    kal_uint16  MaxZoomFactor;
    kal_uint16  CurrentZoomFactor;
} ISP_FEATURE_CURRENT_ZOOM_INFO_STRUCT, *P_ISP_FEATURE_CURRENT_ZOOM_INFO_STRUCT;

// ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION
typedef struct
{
    kal_uint8 ZoomStep;
    CAL_CAMERA_DIGITAL_ZOOM_TYPE_ENUM DzType;
    const kal_uint16 *pZoomFactorListTable;
} ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_IN_STRUCT, *P_ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_IN_STRUCT;

typedef struct
{
    kal_bool  MdpCropEnable;        // Digital zoom is cropped by MDP or not.
    kal_uint16 IspOutWidth;         // ISP Output Image width
    kal_uint16 IspOutHeight;        // ISP output image height
    kal_uint16 DigitalZoomWidth;    // Image width after digital zoom
    kal_uint16 DigitalZoomHeight;   // Image height after digital zoom
} ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT, *P_ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT;

// for ISP_FEATURE_TG_STRUCT
typedef struct
{
    CAL_SCENARIO_ENUM ScenarioId;
    void *pSensorInfo;              // IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT
} ISP_FEATURE_SET_TG_STRUCT, *P_ISP_FEATURE_SET_TG_STRUCT;

typedef struct
{
    kal_bool    BinningMode;                    // enable/disable binning mode
    kal_uint8   ExpoBracketLevel;
    kal_bool    RawCaptureEnable;
    MULTISHOT_INFO_ENUM MultishotInfo;
    CAL_CAMERA_STILL_CAPTURE_ENUM StillCaptureMode;    
}ISP_FEATURE_CONFIG_3A_BEFORE_CAPTURE_STRUCT, *P_ISP_FEATURE_CONFIG_3A_BEFORE_CAPTURE_STRUCT;

// for ISP_FEATURE_SET_CUSTOM_DRIVER_FEATURE_CTRL_FUNC
// because isp_comm_def.h does not include sensor_comm_def.h such that we use kal_uint32 to
// replace IMAGE_SENSOR_FEATURE_ENUM for the parameter of SensorFeatureCtrl
typedef struct
{
    CAL_CAMERA_SOURCE_ENUM CameraId;
    kal_uint32 SensorIdx;       // IMAGE_SENSOR_INDEX_ENUM
    void *pSensorInfo;      // IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT
    MM_ERROR_CODE_ENUM (* SensorFeatureCtrl) (kal_uint32 FeatureId, void *pFeatureParaIn, void *pFeatureParaOut,
                                              kal_uint32 FeatureParaOutLen, kal_uint32 *pRealFeatureParaOutLen);
    MM_ERROR_CODE_ENUM (* LensFeatureControl) (kal_uint32 FeatureId, void *pFeatureParaIn, void *pFeatureParaOut,
                                                kal_uint32 FeatureParaOutLen, kal_uint32 *pRealFeatureParaOutLen);
//	kal_uint32 (* FlashLightFeatureControl) (FLASH_LIGHT_FEATURE_ENUM FeatureId, void *pFeatureParaIn, void *pFeatureParaOut, kal_uint32 FeatureParaOutLen);
} ISP_FEATURE_CUSTOM_DRIVER_FEATURE_CTRL_FUN_STRUCT, *P_ISP_FEATURE_CUSTOM_DRIVER_FEATURE_CTRL_FUN_STRUCT;


typedef struct
{
    kal_uint8   FaceNo;
    kal_uint8   FaceLum;
    kal_bool    FdEnable;
    kal_bool    SdEnable;
    kal_uint8   ResultCounter;
    kal_uint16  FaceX0;
    kal_uint16  FaceY0;
    kal_uint16  FaceX1;
    kal_uint16  FaceY1;
    kal_uint16  ImgSrcWidth;
    kal_uint16  ImgSrcHeight;
} AAA_FACE_INFO_STRUCT, *P_AAA_FACE_INFO_STRUCT;


// for IMAGE_SENSOR_FEATURE_GET_AE_LUT
typedef struct
{
    kal_uint8  Band;
    kal_bool   VideoMode;
    kal_uint8  DscMode;
    kal_uint16 VideoFrameRate;
} ae_lut_info_struct, AE_LUT_INFO_STRUCT, *P_AE_LUT_INFO_STRUCT;

typedef struct
{
	kal_uint16	shutter;
	kal_uint16	sensorgain;
	kal_uint8	ispgain;
} exposure_lut_struct, AE_EXPOSURE_LUT_STRUCT, *P_AE_EXPOSURE_LUT_STRUCT;

// for IMAGE_SENSOR_FEATURE_GET_DSC_SUPPORT_INFO
typedef struct
{
	kal_bool	autodsc;
	kal_bool	portrait;
	kal_bool	landscape;
	kal_bool	sport;
	kal_bool flower;
	kal_bool	nightscene;
	kal_bool	tvmode;
	kal_bool	avmode;
	kal_bool	isomode;
	kal_bool	document_mode;
	kal_bool	iso_anti_hand_shake_mode;
	kal_bool	iso_100_mode;
	kal_bool	iso_200_mode;
	kal_bool	iso_400_mode;
	kal_bool	iso_800_mode;
	kal_bool	iso_1600_mode;
	kal_bool	video_auto_mode;
	kal_bool	video_night_mode;
}camcorder_mode_info_struct;

typedef struct
{
	kal_bool	flashlight;
	kal_bool	autofocus;
}dsc_comp_info_struct;

typedef struct
{
	kal_bool	autoflash;
	kal_bool	forceon;
	kal_bool	forceoff;
	kal_bool	antiredeye;
}flash_mode_info_struct;

typedef struct
{
	kal_bool	af_support;
	kal_bool	single;
	kal_bool	multizone;
	kal_bool	continuous;
	kal_bool	range_auto;
	kal_bool	range_macro;
	kal_bool	range_landscape;
}af_preview_mode_info_struct;

typedef struct
{
	kal_bool	af_support;
	kal_bool	automode;
	kal_bool	multizone;
	kal_bool	continuous;
	kal_bool	range_auto;
	kal_bool	range_macro;
	kal_bool	range_landscape;
}af_video_mode_info_struct;

typedef struct
{
	af_preview_mode_info_struct	preview_mode;
	af_video_mode_info_struct	video_mode;
}af_info_struct;

/************* AAA Engineer mode ******************/
typedef enum 
{
    AF_EM_FULL_SCAN_FORW1=0,  // 0
    AF_EM_FULL_SCAN_FORW4,    // 1
    AF_EM_FULL_SCAN_BACK1,    // 2
    AF_EM_FULL_SCAN_BACK4,    // 3
    AF_EM_CONT_FOCUS_SHOT,    // 4
    AF_EM_BEST_BRACKET_SHOT,  // 5
    AF_EM_CONT_FORWARD_LOG,   // 6
    AF_EM_CONT_BACKWARD_LOG,  // 7
    AF_EM_SINGLE_FORWARD_LOG, // 8
    AE_EM_FULL_SCAN,
    AE_EM_BEST_BRACKET_SCAN,
    AWB_EM_SEQ_PREVIEW_LOG_SAVE,
    AAA_EM_OFF,
    AAA_EM_NO
} AAA_EM_MODE_ENUM;

typedef enum
{
    AAA_EM_CAP_PV_SIZE,
    AAA_EM_CAP_FULL_SIZE,
    AAA_EM_CAP_NO
} AAA_EM_CAP_SIZE_ENUM;

typedef struct
{
    AAA_EM_MODE_ENUM AaaEmMode;
} AAA_EM_IN_PARA_STRUCT, *P_AAA_EM_IN_PARA_STRUCT;

typedef struct
{
    kal_uint32 SnapshotNum; // delay time (ms) if pre_log_enable
    //AAA_EM_CAP_SIZE cap_size;
    kal_bool AfEnable;
    kal_bool PreLogEnable;
} AAA_EM_OUT_PARA_STRUCT, *P_AAA_EM_OUT_PARA_STRUCT;
kal_bool AaaSetEm(P_AAA_EM_IN_PARA_STRUCT pInPara, P_AAA_EM_OUT_PARA_STRUCT pOutPara);
void AaaSaveLog(char *pFileNameBuf);
/**********************************************/



// data structure for ISP
// for ISP preview scenario
typedef struct
{
    IMAGE_SENSOR_MIRROR_ENUM ImageMirror;
    kal_uint16 ImageTargetWidth;
    kal_uint16 ImageTargetHeight;

    P_CAL_ISP_PREVIEW_STRUCT pCalIspPreviewPara;
} ISP_CAMERA_PREVIEW_STRUCT, *P_ISP_CAMERA_PREVIEW_STRUCT;

// for ISP still capture scenario
typedef struct
{
    IMAGE_SENSOR_MIRROR_ENUM ImageMirror;
    CAL_RAW_CAPTURE_MODE_ENUM CaptureScenario;  // for Raw sensor only
    MM_IMAGE_ROTATE_ENUM StillImageExifOrientation;

    kal_uint16 ImageTargetWidth;
    kal_uint16 ImageTargetHeight;

    P_CAL_ISP_STILL_CAPTURE_STRUCT pCalIspCapturePara;
} ISP_CAMERA_STILL_CAPTURE_STRUCT, *P_ISP_CAMERA_STILL_CAPTURE_STRUCT;

// for Video preview/record scenario
typedef struct
{
    IMAGE_SENSOR_MIRROR_ENUM ImageMirror;
    kal_uint16 ImageTargetWidth;
    kal_uint16 ImageTargetHeight;

    P_CAL_ISP_VIDEO_STRUCT pCalIspVideoPara;
} ISP_CAMERA_VIDEO_STRUCT, *P_ISP_CAMERA_VIDEO_STRUCT;


typedef struct
{
    kal_uint16 ImageTargetWidth;
    kal_uint16 ImageTargetHeight;
    kal_uint8 StillImageExifOrientation;
    //...  may add for future usage

} ISP_CAMERA_UPDATE_EXIF_INFO_STRUCT, *P_ISP_CAMERA_UPDATE_EXIF_INFO_STRUCT;

#if defined (__AUTO_SCENE_DETECT_SUPPORT__)
typedef struct
{
    kal_bool    AeIsBacklit; /* AE Backlit Condition from AE algorithm */
    kal_bool    AeIsStable;  /* AE converge to stable situation */
    kal_bool    AwbIsStable; /* AWB converge to stable situation */
    kal_bool    AfIsStable;  /* Af converge to stable situation */
    kal_uint16 AwbCurRgain; /* Current AWB R channel gain */
    kal_uint16 AwbCurBgain; /* Current AWB B channel gain */
    kal_uint16 AfPosition;  /* Current lens position */     
    kal_uint8   AeEv;        /* information version control, 0,1,2 for YUV,WCP1-RAW, WCP2-RAW*/
    kal_int16  AeFaceEnhanceEv;   /* AE face Ev adjustment value  */
} ISP_CAMERA_INFO_3A_STRUCT, *P_ISP_CAMERA_INFO_3A_STRUCT;

typedef struct
{
    kal_bool AaaInfoFromRaw;    /* Raw or Yuv 3A */
    kal_uint16 AwbRefD65Rgain;  /* D65 R channel gain */
    kal_uint16 AwbRefD65Bgain;  /* D65 B channel gain */
    kal_uint16 AwbRefCwfRgain;  /* CWF R channel gain */
    kal_uint16 AwbRefCwfBgain;  /* CWF B channel gain */
    void  *AfTblParam;      /* the whole AF table including macro index & total number of the table */
} ISP_CAMERA_AWB_AF_REF_STRUCT, *P_ISP_CAMERA_AWB_AF_REF_STRUCT;

typedef struct
{
    kal_bool NightModeEnable; /* Night mode enable */
    kal_bool SkinColorEnhanceEnable; /* Skin Color Enhance(PCA) enable */
    kal_bool FaceEVBiosEnable; /* Facial AE enable */	
    CAL_CAMERA_CONTRAST_ENUM SceneContrast; /* Scene Contrast */
    CAL_CAMERA_SATURATION_ENUM SceneSaturation; /* Scene Saturation */
    CAL_CAMERA_SHARPNESS_ENUM SceneSharpness; /* Scene Sharpness */
    CAL_CAMERA_SCENE_MODE_ENUM SceneType;/* Scene Type */	
    kal_uint8 FaceLuminance; /* Face Luminance */
    kal_bool AsdEnabled; /* ASD enable or not */
} ISP_CAMERA_SCENE_COMPENSATION_STRUCT, *P_ISP_CAMERA_SCENE_COMPENSATION_STRUCT ;

typedef struct
{
    kal_bool    AeIsStable;  /* AE converge to stable situation */
    kal_uint8   AeEv;        /* information version control, 0,1,2 for YUV,WCP1-RAW, WCP2-RAW*/
} ISP_CAMERA_INFO_AE_STRUCT, *P_ISP_CAMERA_INFO_AE_STRUCT;
#endif

typedef struct
{
    MM_ERROR_CODE_ENUM (* IspOpen) (void);
    MM_ERROR_CODE_ENUM (* IspFeatureCtrl) (kal_uint32 FeatureId, void *pIspParaIn,
                                           void *pIspParaOut, kal_uint32 IspParaOutLen,
                                           kal_uint32 *pRealIspParaOutLen);
    MM_ERROR_CODE_ENUM (* IspCtrl) (CAL_SCENARIO_ENUM ScenarioId, void *pCtrlParaIn,
    					            void *pCtrlParaOut, kal_uint32 CtrlParaOutLen,
    					            kal_uint32 *pRealCtrlParaOutLen);
    MM_ERROR_CODE_ENUM (* IspClose) (void);
}	ISP_FUNCTION_STRUCT, *P_ISP_FUNCTION_STRUCT;

MM_ERROR_CODE_ENUM IspInit(MM_ERROR_CODE_ENUM (* pfIspCallback) (CAL_CALLBACK_ID_ENUM CbId,
                           void *pCallbackPara, kal_uint16 CallbackParaLen),
                           P_ISP_FUNCTION_STRUCT *pfIspFunc);


#endif /* __ISP_COMM_H__ */
