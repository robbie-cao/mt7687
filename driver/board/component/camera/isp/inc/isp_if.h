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

#ifndef __ISP_IF_H__
#define __ISP_IF_H__

#if 0
#include "kal_release.h"
#include "mm_comm_def.h"
#include "isp_comm_def.h"
#include "sensor_comm_def.h"
#include "isp_flashlight_if.h"
#include "cct_comm_def.h"
#include "cal_api.h"
#include "exif.h"
#include "custom_EMI_release.h"
#include "aaa_process.h"
#include "isp_nvram.h"

#include "drv_features.h"
#endif

#include "mm_comm_def.h"
#include "isp_comm_def.h"
#include "sensor_comm_def.h"
#include "cal_api.h"
#include "isp_if_6250_series_hw.h"

#define __SENSOR_FRAME_RATE_SUPPORT__   //would be remove later

/*38 series if sw.h*/
#define CAMERA_ISP_IDLE_EVENT				(0x00000001)
#define CAMERA_ISP_FRAME_READY_EVENT	(0x00000002)
#define CAMERA_JPEG_ENCODE_EVENT			(0x00000004)
#define CAMERA_ISP_VD_READY_EVENT		(0x00000008)
#define CAMERA_ISP_DZ_READY_EVENT		(0x00000010)

#define LEAVE_INIT_ISP_IF							0x00000001
#define LEAVE_POWER_OFF_ISP						0x00000002
#define LEAVE_ISP_DIGITAL_ZOOM					0x00000004
#define LEAVE_ISP_FAST_ZOOM_SETTING				0x00000008
#define LEAVE_CAMERA_PREVIEW						0x00000010
#define LEAVE_EXIT_CAMERA_PREVIEW				0x00000020
#define LEAVE_CAMERA_CAPTURE_JPEG				0x00000040
#define LEAVE_STOP_CAMERA_CAPTURE				0x00000080
#define LEAVE_CAMERA_CAPTURE_BARCODE			0x00000100
#define LEAVE_STOP_CAMERA_CAPTURE_BARCODE		0x00000200
#define LEAVE_PAUSE_CAMERA_CAPTURE_BARCODE	       0x00000400
#define LEAVE_RESUME_CAMERA_CAPTURE_BARCODE	0x00000800
#define LEAVE_CAMERA_CAPTURE_MEM					0x00001000
#define LEAVE_MPEG4_ENCODE						0x00002000
#define LEAVE_MPEG4_ENCODE_START					0x00004000
#define LEAVE_MPEG4_ENCODE_PAUSE					0x00008000
#define LEAVE_MPEG4_ENCODE_RESUME				0x00010000
#define LEAVE_EXIT_MPEG4_ENCODE					0x00020000
#define LEAVE_MJPEG_ENCODE						0x00040000
#define LEAVE_MJPEG_ENCODE_START					0x00080000
#define LEAVE_MJPEG_ENCODE_PAUSE					0x00100000
#define LEAVE_MJPEG_ENCODE_RESUME				0x00200000
#define LEAVE_EXIT_MJPEG_ENCODE					0x00400000
#define LEAVE_VIDEO_FULLDUPLEX_ENCODE			0x00800000
#define LEAVE_EXIT_VIDEO_FULLDUPLEX_ENCODE		0x01000000

/*isp_if_6238_series_if.h*/
#define CAPTURE_RETRIAL_FRAME_NO (2)
#define CAMERA_TIME_TICK_BUFFER_COUNT (5)

//zoom ability
#define ISP_BASE_DIGITAL_ZOOM_FACTOR  (100)
#define ISP_MAX_DIGITAL_ZOOM_FACTOR  (400)
#define ISP_MAX_DIGITAL_ZOOM_STEP  (40)

/*aaa_process.h*/
typedef struct
{
    kal_uint8 Metering;
    kal_uint16  TV;
    kal_uint8 AV;
    kal_uint8 ISO;
    kal_uint8 Flash;
} AE_OPERATION_STRUCT;

typedef struct
{
    kal_uint8 Trigger;
    kal_uint8 Mode;
    kal_uint8 Range;
    kal_uint8 Mfdir;
} AF_OPERATION_STRUCT;

typedef struct
{
    kal_uint8       DscMode;
    AE_OPERATION_STRUCT Ae;
    AF_OPERATION_STRUCT Af;
} DSC_OPERATION_STRUCT;

typedef struct
{ 
    kal_uint8   SnapshotNumber;       /* Snapshot number 0: no limit, 1~n: cont' shot num */    
    kal_uint8   CurrentSnapshotIndex; /* Current Snapshot number 0~n */   
    kal_uint8   BestSnapshotIndex;    /* Best Snapshot number 0~n */      
    kal_uint32  BestSnapshotValue;    /* Best Snapshot value(FV/JPEG) */        
} ISP_BSS_INFO_STRUCT, *P_ISP_BSS_INFO_STRUCT;

typedef enum
{
	IspVsyncTimeOut100MS=10,
	IspVsyncTimeOut200MS=20,
	IspVsyncTimeOut250MS=25,
	IspVsyncTimeOut300MS=30,
	IspVsyncTimeOut500MS=50
} ISP_VSYNC_TIME_OUT_UNIT;


//isp_isr.c
//extern kal_eventgrpid IspEventId;
extern kal_uint8 IspSleepModeHandler;
extern kal_bool IspDynamicSettingFlag;
extern kal_uint32 IspTimeTickBuff[CAMERA_TIME_TICK_BUFFER_COUNT];
extern kal_uint32 IspCurrentFrameRate;
extern kal_uint32 IspVsyncTgCnt ;
extern kal_uint32 IspFrameReadyCnt;
void IspInitEvent(void);
void IspInitISR(void);
void IspCameraLISR(void);
void IspCameraHISR(void);
//kal_uint32 IspGetFrameRate(void);
//kal_uint32 IspGetFrameTime(void);


//isp_comm_if.c
void IspLoadCameraPara(void);
void IspGetSensorShadingInfo(void);

//void apply_camera_shading_to_reg(ISP_OPERATION_STATE_ENUM isp_state, CAMERA_TUNING_SET_ENUM tuning_set, kal_uint32 *addr,IMAGE_SENSOR_MIRROR_ENUM ImageMirror);
//void apply_camera_defect_to_reg(ISP_OPERATION_STATE_ENUM isp_state, CAMERA_TUNING_SET_ENUM tuning_set, kal_uint32 *addr);




//isp_control.c
typedef struct
{
    kal_uint8 IspContrastLevel;
    kal_uint8 IspSaturationLevel;
    kal_uint8 IspSharpnessLevel;
  
} ISP_QUALITY_TUNINIG_LEVEL_STRUCT, *P_ISP_QUALITY_TUNINIG_LEVEL_STRUCT;


extern kal_uint16 ExifIspTargetWidth, ExifIspTargetHeight;
extern kal_uint16 ExifOrientation;
extern ISP_CAMERA_PREVIEW_STRUCT IspPreviewConfigData;
extern CAL_ISP_PREVIEW_STRUCT CalIspPreviewData;
extern ISP_QUALITY_TUNINIG_LEVEL_STRUCT IspQualityTuningData;
extern ISP_CAMERA_VIDEO_STRUCT IspVideoConfigData;
extern ISP_CAMERA_STILL_CAPTURE_STRUCT IspCaptureConfigData;

#if 0
extern AAA_PREVIEW_PROCESS_STRUCT AaaPrevConfigData;
extern AAA_CAPTURE_COMPENSATE_PROCESS_STRUCT AaaCaptureCompensateData;
#endif

MM_ERROR_CODE_ENUM IspRawPreview(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen);
MM_ERROR_CODE_ENUM IspRawVideo(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen);
MM_ERROR_CODE_ENUM IspRawStillCapture(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen);
MM_ERROR_CODE_ENUM IspYuvPreview(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen);
MM_ERROR_CODE_ENUM IspYuvVideo(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen);
MM_ERROR_CODE_ENUM IspYuvStillCapture(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen);

void IspDigitalZoom(CAL_CAMERA_DIGITAL_ZOOM_TYPE_ENUM DzType,
    kal_uint8 ZoomStep,  const kal_uint16 *pZoomFactorListTable,
    P_ISP_FEATURE_GET_DIGITAL_ZOOM_RESOLUTION_OUT_STRUCT pParaOut);
void IspDigitalZoomConfig(ISP_OPERATION_STATE_ENUM IspState,
    CAL_CAMERA_DIGITAL_ZOOM_TYPE_ENUM DzType,
    kal_uint8 ZoomStep,  const kal_uint16 *pZoomFactorListTable,
                          P_ISP_FEATURE_GET_OUTPUT_RESOLUTION_OUT_STRUCT pParaOut);
kal_uint32 IspGetDigitalZoomFactor(void);
kal_uint32 IspGetDigitalZoomVerticalFactor(void);

extern kal_uint32 BackupRegCam0550h;
extern kal_uint16 IspGrabWidth,IspGrabHeight,IspGrabStartX,IspGrabStartY;
extern kal_uint16 IspResultWindowWidth,IspResultWindowHeight,IspResultWindowStartX,IspResultWindowStartY;
extern kal_uint16 IspResultWindowStartXBak,IspResultWindowStartYBak;
extern kal_uint32 IspZoomResultWindowWidth,IspZoomResultWindowHeight;
extern kal_uint32 IspProcessStage;
extern kal_uint32 IspFrameCount;

//isp_driver_if.c
typedef enum
{
    CCT_CAP_RAWBIT_DEFAULT=0,
    CCT_CAP_RAWBIT_8,
    CCT_CAP_RAWBIT_10,
    CCT_CAP_NO_OF_RAWBIT_VALID
}CCT_CAP_RAWBIT_ENUM;
extern kal_uint8 IspCctCapRawBit;
extern kal_bool IspCctCapPreviewSize;
extern kal_uint32 IspCctRawCaptureWidth;
extern kal_uint32 IspCctRawCaptureHeight;

typedef enum
{
	EM_SAVE_RAW_8=0,
	EM_SAVE_RAW_10
}EM_SAVE_RAW_BIT_ENUM;

typedef struct
{
	kal_uint32 RawBufAddr;
	kal_uint32 RawBufSize;
	kal_uint32 RawWidth;
	kal_uint32 RawHeight;
}ISP_EM_RAW_SAVE_STRUCT;


#if (defined(RAW_MAIN_CAMERA))				
extern IMAGE_SENSOR_GET_DSC_SUPPORT_INFO_STRUCT DscSupportInfo;
#endif
extern volatile kal_bool IspIdleDoneFlag, IspDoneFlag;
extern volatile kal_bool FirstSensorFrameFlag;
extern kal_bool WaitFirstFrameFlag;
extern kal_uint8 IspPreviewFrameCount;
extern kal_uint16 IspImageSettingIndex;
#if defined (__AUTO_SCENE_DETECT_SUPPORT__)
extern kal_uint8 IspAsdCompensationIndex;
#endif
extern kal_bool IspCheckFirstVsync;
extern kal_uint32 IspCurrentFrameRate;
extern ISP_EM_RAW_SAVE_STRUCT IspSaveRawInfo;

extern DSC_OPERATION_STRUCT DscStatus;
extern device_info_struct device_support_info;
void InitIsp(void);
#if defined (__AUTO_SCENE_DETECT_SUPPORT__)
void IspBackgndSetAsdCompensation(void);
#endif
void IspBackgndImageSetting(void);
void IspOperationSetting(kal_uint8 Func, kal_uint16 Para);
void IspSetImageSetting(kal_uint8 Type, kal_uint16 Para);
void IspStartInput(ISP_OPERATION_STATE_ENUM IspState);
void IspStopInput(ISP_OPERATION_STATE_ENUM IspState);
void IspDisableViewFinder(void);
void IspCompensationConfig(ISP_OPERATION_STATE_ENUM IspState, kal_uint32 IntMemAddr, kal_uint32 CurrentWidth,IMAGE_SENSOR_MIRROR_ENUM ImageMirror );
kal_bool IspDropFrame(void);
void IspEnableOverrunInt(void);
void IspDisableOverrunInt(void);

void IspWaitOneVsync(ISP_VSYNC_TIME_OUT_UNIT TimeOut);
void IspWaitOneFrameDone(ISP_VSYNC_TIME_OUT_UNIT TimeOut);
void IspWaitSensorStableDelayFrame(void);

void IspWaitCaptureDelayFrame(void);
void IspStartRawCapture(kal_uint32 RawBufAddr);
void IspCloseRawCapture(void);
void IspPresetEnableMemoryInFreq(void);
void IspResetDisableMemoryInFreq(void);
void IspSetFirstGrabPixelColor(IMAGE_SENSOR_MIRROR_ENUM ImageMirror );

#if defined(__JPEG_SENSOR_SUPPORT__)
MM_ERROR_CODE_ENUM IspJpegSensorCaptureStart(kal_uint32 JpegBufAddr);
void IspJpegSensorCaptureStop(void);
#endif

kal_uint32 IspEmbedMtkInfoIsp(kal_uint32 *addr);
kal_uint32 CameraEmbedMtkInfo(kal_uint32 *addr);
void IspQueryProcessDataBitDepth(P_ISP_PROCESS_DATA_BIT_DEPTH_STRUCT pIspDataBitDepth);
void IspSetInterruptTriggerDelayLines(kal_uint16 Lines);
void IspSetWebcamContrast(kal_uint8 Level);
void IspSetWebcamBrightness(kal_uint8 Level);
void IspSetWebcamSaturation(kal_uint8 Level);
void IspSetWebcamHue(kal_uint16 Degree);
void IspEmSetSaveRawBit(kal_uint8 Bit);
EM_SAVE_RAW_BIT_ENUM IspEmGetSaveRawBit(void);
void IspEmSetSaveRawAddr(kal_uint32 RawImageAddr);
void IspEmGetRawSaveInfo(ISP_EM_RAW_SAVE_STRUCT *RawSaveInfo);
void IspEmEnableShading(kal_bool En);
#define isp_em_set_save_raw_bit IspEmSetSaveRawBit
#define isp_em_get_save_raw_bit IspEmGetSaveRawBit
#define isp_em_set_save_raw_addr IspEmSetSaveRawAddr
#define isp_em_get_raw_save_info IspEmGetRawSaveInfo
void IspCheckMemoryInAddr(kal_uint32 RawBufAddr);

#if (defined(EXIF_SUPPORT))
void exif_update_MTK_para_struct(exif_MTK_para_struct* para);
void ExifUpdateCamParaStruct(exif_cam_para_struct *pExifPara);
#endif

#if (defined(__ISP_MIPI_SENSOR_SUPPORT__))
void MipiInterfaceInit(void);
#endif

#ifdef MIPI_SENSOR_SUPPORT
kal_bool MipiInterfaceEnable(kal_bool Enable, P_IMAGE_SENSOR_MIPI_PARA_IN_STRUCT pMipiParaIn);
#endif



//isp_image_effect.c
extern kal_uint32 IspCurrentEffect;
void IspImageEffectSetting(kal_uint8 Effect);

//isp_tuning_oper.c
void IspSharpnessSetting(kal_uint8 Index);
void IspSaturationSetting(kal_uint8 Index);
void IspContrastSetting(kal_uint8 Index);
void IspNR1Setting(kal_uint8 Index);
void IspNR2Setting(kal_uint8 Index);
void IspAutodefectSetting(kal_uint8 Index);
void IspSetQualityTuningLevel(ISP_QUALITY_TUNINIG_LEVEL_STRUCT *pQualityPara);
void IspDynamicImageSetting(void);
#if 0
kal_bool CameraImageDynamicSetting(ISP_OPERATION_STATE_ENUM CameraState, kal_uint32 AeTotalGain,
                                   nvram_camera_para_struct *pCameraPara);
#endif

// isp_main.c
extern ISP_OPERATION_STATE_ENUM IspOperationState;
extern kal_uint32 SensorFeatureParaOutLen;
extern kal_bool IspIsDscSupportInfoQueried;

extern MM_ERROR_CODE_ENUM (* pfCalIspCb) (CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);

MM_ERROR_CODE_ENUM IspInit(MM_ERROR_CODE_ENUM (* IspCallback) (CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen),
                           P_ISP_FUNCTION_STRUCT *pfIspFunc);



//isp_nvram.c
MM_ERROR_CODE_ENUM IspSetNvramData(P_ISP_FEATURE_NVRAM_DATA_STRUCT pNvramData);
MM_ERROR_CODE_ENUM IspGetNvramData(P_ISP_FEATURE_NVRAM_DATA_STRUCT pNvramDataIn,
                                   P_ISP_FEATURE_NVRAM_DATA_STRUCT pNvramDataOut);
MM_ERROR_CODE_ENUM IspGetNvramInfo(P_CAL_SENSOR_SUPPORT_STRUCT pSensorInfo,
                                   P_ISP_FEATURE_NVRAM_INFO_STRUCT pNvramInfo);

// isp_feature.c 
extern ISP_DATA_FORMAT_ENUM IspInputDataFormat; 
extern IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT SensorInfo1; 
extern ISP_FEATURE_CUSTOM_DRIVER_FEATURE_CTRL_FUN_STRUCT CustomDriverFunc; 
extern ISP_FEATURE_GET_OUTPUT_RESOLUTION_IN_STRUCT IspGlobalInfo;
extern kal_uint32 IspCpllOutClk;

extern volatile kal_bool IspJpegCaptureStart;
extern IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM SensorCaptureDataFormat;



void IspBssConfig(MULTISHOT_INFO_ENUM MultishotInfo);
kal_uint8 IspGetBssResult(kal_uint8 SnapshotIndex, kal_uint32 JpegLength);
MM_ERROR_CODE_ENUM IspFeatureCtrl(kal_uint32 FeatureId, void *pIspParaIn,
                                  void *pIspParaOut, kal_uint32 IspParaOutLen,
                                  kal_uint32 *pRealIspParaOutLen);

// isp_webcam.c
extern kal_uint32 IspMainWebcamAttrValues[];
extern kal_uint32 IspSubWebcamAttrValues[];
extern kal_uint32 *pIspWebcamAttrValues;
MM_ERROR_CODE_ENUM IspWebcamFeatureCtrl(kal_uint32 FeatureId, void *pIspParaIn,
                                  void *pIspParaOut, kal_uint16 IspParaOutLen);
MM_ERROR_CODE_ENUM IspWebcam(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode,
                             void *pCtrlPara, kal_uint16 CtrlParaLen);                                  
void IspWebcamUpdateAttrSetting(void);
void IspWebcamBackgroundSettingAttr(void);
void IspWebcamInit(void);


extern void IspExifUpdateMtkParaStruct(kal_uint32* pPara, kal_uint32 ParaSize);

typedef struct
{	
  kal_uint32	MtkPara[300];
} EXIF_MTK_CAMERA_PARA_STRUCT, exif_MTK_camera_para_struct;

#define CAM_DBG_EXIF_AE_SIZE  (75) // unit:dword
#define CAM_DBG_EXIF_AWB_SIZE (75) // unit:dword
#define CAM_DBG_EXIF_AF_SIZE  (90) // unit:dword
#define CAM_DBG_EXIF_ISP_SIZE (60) // unit:dword
#define CAM_DBG_EXIF_AE_BASE  (0)
#define CAM_DBG_EXIF_AWB_BASE (CAM_DBG_EXIF_AE_BASE +CAM_DBG_EXIF_AE_SIZE)
#define CAM_DBG_EXIF_AF_BASE  (CAM_DBG_EXIF_AWB_BASE+CAM_DBG_EXIF_AWB_SIZE)
#define CAM_DBG_EXIF_ISP_BASE (CAM_DBG_EXIF_AF_BASE +CAM_DBG_EXIF_AF_SIZE)
#define CAM_DBG_EXIF_AE_END_TAG  (0xA0E0E0D0)
#define CAM_DBG_EXIF_AWB_END_TAG (0xA0B0E0D0)
#define CAM_DBG_EXIF_AF_END_TAG  (0xA0F0E0D0)
#define CAM_DBG_EXIF_ISP_END_TAG (0xF0F0F0F0)

#define CAM_EMBED_INFO_SIZE_AE  (640) // unit:dword
#define CAM_EMBED_INFO_SIZE_AWB (640) // unit:dword
#define CAM_EMBED_INFO_SIZE_AF  (640) // unit:dword
#define CAM_EMBED_INFO_SIZE_ISP (512) // unit:dword
#define CAM_EMBED_INFO_END_TAG_AE  (0xA0E0E0D0) // unit:dword
#define CAM_EMBED_INFO_END_TAG_AWB (0xA0B0E0D0) // unit:dword
#define CAM_EMBED_INFO_END_TAG_AF  (0xA0F0E0D0) // unit:dword
#define CAM_EMBED_INFO_END_TAG_ISP (0xF0F0F0F0) // unit:dword
#define CAM_EMBED_INFO_MTK_TAG0  (0x6964654D) // ideM
#define CAM_EMBED_INFO_MTK_TAG1  (0x6B655461) // keTa

#endif /* __ISP_IF_H__ */
