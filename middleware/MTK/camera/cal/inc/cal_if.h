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

#ifndef __CAL_IF_H__
#define __CAL_IF_H__

#if 0
#include "kal_release.h"
#include "stack_common.h"
#include "stack_msgs.h"
#include "app_ltlcom.h" /* Task message communiction */
#include "syscomp_config.h"
#include "task_config.h"        /* Task creation */

#include "usbvideo_custom.h"
#include "jpeg_enum.h"
#include "jpeg_encode_wrapper.h"
#include "cal_api.h"
#include "isp_comm_def.h"
#include "sensor_comm_def.h"
#include "lens_comm_def.h"
#include "lens_module.h"
#include "cal_trace.h"
#include "pp_comm_def.h"
#include "pp_api.h"
#include "cal_comm_def.h"
#include "mpl_recorder.h"
#endif

#include "FreeRTOS.h"
#include "cal_api.h"
#include "isp_comm_def.h"
#include "sensor_comm_def.h"
#include "cal_comm_def.h"
#include "queue.h"
#include "task.h"
#include "idp_cam_if.h"

#define CAL_NVRAM_READ_EVENT            0x00000001
#define CAL_GET_PREVIEW_IMAGE_EVENT     0x00000002
#define CAL_PP_ABORT_DONE_EVENT     0x00000003

#define CAL_STILL_CAPTURE_MAIN_IMAGE_BIT            0x00000001
#define CAL_STILL_CAPTURE_QUICK_VIEW_IMAGE_BIT      0x00000002
#define CAL_STILL_CAPTURE_THUMBNAIL_IMAGE_BIT       0x00000004
#define CAL_STILL_CAPTURE_IMAGE_MASK                0x00000007

#define CAL_MAX(A,B)         ( ((A)>(B)) ? A : B)
#define CAL_MIN(A,B)         ( ((A)<(B)) ? A : B)

typedef enum
{
    CAL_IDLE_STATE,
    CAL_STANDBY_STATE,
    CAL_CAMERA_PREVIEW_STATE,
    CAL_CAMERA_CAPTURE_STATE,
    CAL_VIDEO_PREVIEW_STATE,
    CAL_VIDEO_RECORD_STATE,
    CAL_VIDEO_MJPEG_STASTE,
    CAL_JPEG_ENCODE_STATE,
    CAL_WEBCAM_PREVIEW_STATE,
    CAL_WEBCAM_CAPTURE_STATE,
    CAL_POSTPROC_STATE
} CAL_STATE_ENUM;

typedef enum
{
    CAL_PANO_STANDBY_STATE,
    CAL_PANO_ADDIMAGE_STATE,
    CAL_PANO_STITCHING_STATE,
    CAL_PANO_STITCHED_STATE,
    CAL_PANO_ENCODING_STATE,
    CAL_PANO_ENCODED_STATE,
    CAL_PANO_STOPPING_STATE
} CAL_PANO_STATE_ENUM;

typedef enum
{
    CAL_HDR_STANDBY_STATE,
    CAL_HDR_DECODING_STATE,
    CAL_HDR_DECODED_STATE,
    CAL_HDR_CORE_STATE,
    CAL_HDR_QVI_STATE,
    CAL_HDR_ENCODING_STATE,
    CAL_HDR_ENCODED_STATE,
    CAL_HDR_JAIA_STATE,
    CAL_HDR_IDLE_STATE,
} CAL_HDR_STATE_ENUM;

typedef struct
{
    kal_uint32  ImgBufferAddr;
    kal_uint16  ImgWidth;
    kal_uint16  ImgHeight;
    MM_IMAGE_FORMAT_ENUM  ImgFormat;
    kal_bool ThumbnailEnable;
}CAL_PP_CB_JPG_INFO_CONFIG, *P_CAL_PP_CB_JPG_INFO_CONFIG;

typedef struct
{
    kal_uint16 MainImageWidth;
    kal_uint16 MainImageHeight;
    kal_uint16 QuickViewPanoImageWidth;
    kal_uint16 QuickViewPanoImageHeight;
    CAL_MDP_POSTPROC_STRUCT CalMdpPostProcPara;
    kal_bool ImageAlignmentHandle;
    kal_bool JaiaQviBufAligned;
} CAL_PANO_POSTPROC_STRUCT, *P_CAL_PANO_POSTPROC_STRUCT;

typedef enum
{
    CAL_MAV_STANDBY_STATE,
    CAL_MAV_CAPTURE_STATE,
    CAL_MAV_POSTPROC_STATE,
    CAL_MAV_ENCODE_STATE,
    CAL_MAV_COMPLETE_STATE
} CAL_MAV_STATE_ENUM;

#if 0
typedef struct
{
    kal_uint32 ImageBufferAddr; // start addr for continuous 9 images
    kal_uint32 ImageBufferSize;
    kal_uint16 ImageWidth;    // each image size
    kal_uint16 ImageHeight;
    kal_int16 ImageMV[MAV_IMAGE_NUM][2];
    kal_uint8 ImageNumber;
    kal_uint32 WorkingBuffAddr;
    kal_uint32 WorkingBuffSize;
} CAL_MAV_POSTPROC_STRUCT, *P_CAL_MAV_POSTPROC_STRUCT;
#endif

typedef struct
{
    kal_uint8 state;            /* Camera module state */
    kal_uint16 app_id;
    kal_char* src_mod;        /* the source module of request */
//    kal_eventgrpid cam_event;   /* the event group of camera module */

    kal_uint8 media_mode;       /* media mode, FILE, ARRAY, STREAM */
    kal_int32 file_handle;      /* the file handle of the file */
    kal_wchar *open_file_name_p;
    kal_wchar *file_name_p;     /* the capture file name pointer */
    kal_uint32 *file_size_p;    /* the file size pointer for capture result */
    kal_uint16 seq_num;         /* sequence num */
    kal_uint16 cam_id;          /* camera id */
    kal_uint8 source_device;
    kal_uint8 image_quality;    /* image quality */
    kal_uint8 continue_capture;
//    STFSAL    fsal_file;

    kal_bool is_quickview_ready;
    kal_bool is_thumbnail_ready;
    kal_bool is_main_jpeg_ready;
    kal_bool is_waiting_save;
    kal_bool is_profiling_on;

    kal_uint8  capture_mode;
    /* preview screen parameters */
    kal_uint16 preview_width;       /* preview screen width */
    kal_uint16 preview_height;      /* preview screen height */
    kal_uint16 preview_offset_x;    /* preview screen offset x */
    kal_uint16 preview_offset_y;    /* preview screen offset y */
    kal_uint32 frame_buffer1_p;     /* pointer of the preview image frame buffer 1 */
    kal_uint32 frame_buffer2_p;     /* pointer of the preview image frame buffer 2 */
    kal_uint32 frame_buffer3_p;     /* pointer of the preview image frame buffer 3 */
    kal_uint32 frame_buffer_temp_p;     /* pointer of the preview image frame temp buffer  */   
    kal_uint32 frame_buffer_size;   /* size of the preview image frame buffer */
//#ifdef DRV_MDP_LIMIT_CACHE_LINE_32BYTE_ALIGNMENT
    kal_uint32 app_frame_buffer_p;
    kal_uint32 app_frame_buffer_size;
//#endif
    
#ifdef MDP_SUPPORT_HW_OVERLAP      
    kal_bool   overlay_frame_mode;                /* KAL_TRUE or KAL_FALSE to enable or disable overlay function */
    kal_uint8  overlay_color_depth;              /* The color depth of overlay frame buffer */
    kal_uint8  overlay_frame_source_key;
    kal_uint16 overlay_frame_width;             /* Image width of overlay frame buffer */
    kal_uint16 overlay_frame_height;            /* Image height of overlay frame buffer */
    kal_uint32 overlay_frame_buffer_address;    /* The starting address of overlay frame buffer */
    kal_uint32 overlay_palette_addr;
    kal_uint8  overlay_palette_size;
#endif    

    MM_IMAGE_FORMAT_ENUM  image_data_format;
    kal_uint8  ui_rotate;
    kal_uint8  preview_rotate;
    kal_uint8  capture_rotate;
    kal_uint8  sensor_rotate;
    kal_uint8  included_angle;      /* the angle between UI and sensor rotate */
    kal_uint8  quickview_lcd_rotate;

    kal_bool   binning_mode;
    kal_bool   is_fast_zoom;      /* fast zoom */
    kal_uint8  zoom_step;            /* current zoom step number */
    kal_uint8  total_zoom_step;      /* total zoom step number */
    kal_uint32 max_zoom_factor;      /* maximum zoom factor */
    kal_uint8  multishot_info;

    kal_uint8  image_mirror;                        /* horizonal/vertical mirror from image sensor */
    CAL_CAMERA_CONTRAST_ENUM        contrast_level;     /* ISP contrast gain level */
    CAL_CAMERA_SATURATION_ENUM      saturation_level;   /* image processor saturation level */
    CAL_CAMERA_SHARPNESS_ENUM       sharpness_level;
    CAL_CAMERA_WB_ENUM              wb_mode;            /* white balance mode */
    CAL_CAMERA_EV_ENUM              ev_value;           /* EV adjust value */
    CAL_CAMERA_BANDING_FREQ_ENUM    banding_freq;       /* Banding frequency selection */
    CAL_CAMERA_IMAGE_EFFECT_ENUM    image_effect;       /* image effect for preview */
    CAL_CAMERA_SCENE_MODE_ENUM      scene_mode;
    CAL_CAMERA_FLASH_ENUM           flash_mode;         /* AUTO, ON, OFF, REDEYE */
    CAL_CAMERA_AF_RANGE_ENUM        af_range;    
    CAL_CAMERA_AF_OPERATION_MODE_ENUM   af_operation_mode;
    CAL_CAMERA_AE_METERING_MODE_ENUM    ae_metering_mode;    

    kal_uint8 camera_scene_detected_mode;   // for ASD result

    kal_uint32 extmem_start_address;    /* external memroy start address for hardware engine buffer */
    kal_uint32 extmem_size;             /* external memory size for hardware engine buffer */
    kal_uint8  capture_mem_result;      /* memory out result */

    kal_uint8 expo_bracket_level;
    
#ifdef __YUVCAM_INTERPOLATION_SW__
    kal_uint16 interpolate_image_width;
    kal_uint16 interpolate_image_height;
    kal_uint32 interpolate_work_buffer_address;
#endif

    kal_bool   quickview_output; /* is direct couple to another buffer */
    kal_uint8  quickview_output_type;
    kal_uint16 quickview_output_width;
    kal_uint16 quickview_output_height;
    kal_uint32 quickview_output_buffer_address;
    kal_uint32 quickview_output_buffer_size;
    
    kal_uint32 capture_buffer_p;        /* pointer of the capture image buffer */
    kal_uint32 capture_buffer_size;

#if defined(__CAMERA_CAPTURE_USING_APP_MEMORY__)
    kal_uint32 capture_y_buffer_address;
    kal_uint32 capture_y_buffer_size;
    kal_uint32 capture_u_buffer_address;
    kal_uint32 capture_u_buffer_size;
    kal_uint32 capture_v_buffer_address;
    kal_uint32 capture_v_buffer_size;
#endif

    kal_uint32 app_capture_buffer_p;        /* pointer of the capture image buffer */
    kal_uint32 app_capture_buffer_size;

#ifdef __MED_CAM_BESTSHOT_SUPPORT__
    kal_uint32 bss_buffer1_p;
    kal_uint32 bss_buffer1_size;
    kal_uint32 bss_buffer2_p;
    kal_uint32 bss_buffer2_size;
    kal_uint32 best_image_buffer_p;
    kal_uint32 best_image_p;
    kal_uint32 best_image_size;
#endif /* __MED_CAM_BESTSHOT_SUPPORT__ */   
#ifdef __MED_CAM_HDR_SUPPORT__
    kal_uint32 hdr_src_buffer_p;
    kal_uint32 hdr_src_buffer_size;
    kal_uint32 hdr_dst_image_p;                 /* the hdr processed image */
    kal_uint32 hdr_dst_image_size;              /* the hdr processed image size */
    kal_uint32 hdr_working_buffer_p;
    kal_uint32 hdr_working_buffer_size;
    kal_uint32 hdr_quickview_buffer_p;
    kal_uint32 hdr_quickview_buffer_size;
    kal_wchar *hdr_file_name_p;                 /* the file name pointer */
    kal_bool   is_hdr_done;
    kal_bool   is_hdr_waiting_save;
#endif    
    
    void        **capture_buffer_pp;           /* pointer of the capture image buffer */
    kal_uint32  *capture_buffer_size_p;

    kal_bool    raw_capture_enable;         /* enable/disable sensor raw data captured to memory before ISP processing */
    kal_uint32  raw_image_buffer_address;   /* the buffer address for store sensor bayer image */

    kal_uint32 thumbnail_buffer_address;
    kal_uint32 thumbnail_buffer_size;
    kal_uint32 thumbnail_image_address;
    kal_uint32 thumbnail_image_size;

    kal_uint32 jpeg_buffer_address;        /* pointer of the capture image buffer */
    kal_uint32 jpeg_buffer_size;
    kal_uint32 jpeg_image_address;        /* pointer of the capture image buffer */
    kal_uint32 jpeg_image_size;
    
    kal_uint8 snapshot_number;          /* snapshot number */
    kal_uint8 current_shot_number;
    kal_uint8 captured_number;
//    jpegs_struct jpeg_file_info;
//    jpegs_struct *jpeg_file_info_p;
    /* lcd layer parameters */
    kal_bool lcd_update;
    kal_uint8 lcd_id;                   /* LCD ID */
    kal_uint32 update_layer;            /* the updated layer */
    kal_uint32 hw_update_layer;         

    kal_uint32 file_size;
    kal_uint16 image_width;
    kal_uint16 image_height;
    kal_uint16 image_resolution;

    kal_int8   pano_state;
    kal_bool   is_waiting_burst_capture_stop; /* wiat for burst shot complete */

    kal_bool   jaia_disable;
} cam_context_struct;


typedef enum {
    MSG_ID_MEDIA_CAM_POWER_DOWN_REQ,
    MSG_ID_MED_CAM_CODE_BEGIN = MSG_ID_MEDIA_CAM_POWER_DOWN_REQ,
    MSG_ID_MEDIA_CAM_PREVIEW_REQ,
    MSG_ID_MEDIA_CAM_STOP_PREVIEW_REQ,
#if 0
    MSG_ID_MEDIA_CAM_NVRAM_END_CNF,
    MSG_ID_MEDIA_CAM_READY_IND,
    MSG_ID_MEDIA_CAM_PREVIEW_FAIL_IND,
    MSG_ID_MEDIA_CAM_STOP_REQ,
    MSG_ID_MEDIA_CAM_CAPTURE_REQ,
    MSG_ID_MEDIA_CAM_ENCODE_IMG_REQ,
    MSG_ID_MEDIA_CAM_SET_PARAM_REQ,
    MSG_ID_MEDIA_CAM_SET_FLASH_REQ,
    MSG_ID_MEDIA_CAM_SET_CAP_RESOLUTION_REQ,
    MSG_ID_MEDIA_CAM_START_FAST_ZOOM_REQ,
    MSG_ID_MEDIA_CAM_STOP_FAST_ZOOM_REQ,
    MSG_ID_MEDIA_CAM_GET_ZOOM_STEP_FACTOR_REQ,
    MSG_ID_MEDIA_CAM_GET_FOCUS_STEPS_REQ,
    MSG_ID_MEDIA_CAM_GET_FOCUS_ZONE_REQ,
    MSG_ID_MEDIA_CAM_GET_FD_RESULT_REQ,
    MSG_ID_MEDIA_CAM_CAPTURE_EVENT_IND,
    MSG_ID_MEDIA_CAM_AF_PROCESS_DONE_IND,
    MSG_ID_MEDIA_CAM_EVENT_IND,
    MSG_ID_MEDIA_CAM_SAVE_REQ,
    MSG_ID_MEDIA_CAM_XENON_FLASH_STATUS_IND,
    MSG_ID_MEDIA_CAM_RESUME_PREVIEW_PP_REQ,   
    MSG_ID_MEDIA_CAM_PAUSE_PREVIEW_PP_REQ,
    MSG_ID_MEDIA_CAM_SET_FD_ID_REQ,
    MSG_ID_MEDIA_CAM_UNSET_FD_ID_REQ,
    MSG_ID_MEDIA_CAM_FD_RESULT_IND,
    MSG_ID_MEDIA_CAM_START_STITCH_REQ,
    MSG_ID_MEDIA_CAM_STOP_STITCH_REQ,
    MSG_ID_MEDIA_CAM_PAUSE_STITCH_REQ,
    MSG_ID_MEDIA_CAM_RESUME_STITCH_REQ,
    MSG_ID_MEDIA_CAM_STITCH_RESULT_IND,
    MSG_ID_MEDIA_CAM_STITCH_QUICK_VIEW_IND,
    MSG_ID_MEDIA_CAM_PANORAMA_RESET_3A_REQ,
    MSG_ID_MEDIA_CAM_SET_PANORAMA_STITCH_MEM_REQ,
    MSG_ID_MEDIA_CAM_PANORAMA_ENCODE_REQ,
    MSG_ID_MEDIA_CAM_GET_AUTOCAP_RESULT_REQ,
    MSG_ID_MEDIA_CAM_SET_CAMERA_ID_REQ,
    MSG_ID_MEDIA_CAM_SENSOR_QUERY_CAPBILITY_REQ,
    MSG_ID_MEDIA_CAM_SENSOR_QUERY_ZOOM_REQ,
    MSG_ID_MEDIA_CAM_GET_BUFFER_CONTENT_REQ,
    MSG_ID_MEDIA_CAM_GET_PREDICTED_IMAGE_SIZE_REQ,
    MSG_ID_MEDIA_CAM_SD_EVENT_IND,
    MSG_ID_MEDIA_CAM_STOP_HDR_REQ,
    MSG_ID_MEDIA_CAM_SAVE_HDR_IMG_REQ,    
    MSG_ID_MEDIA_CAM_HDR_EVENT_IND,
    MSG_ID_MEDIA_CAM_PROFILING_SET_ACTIVE_REQ,    
    MSG_ID_MEDIA_CAM_PROFILING_SET_BUFFER_REQ,
    MSG_ID_MEDIA_CAM_PROFILING_LOG_REQ,
    MSG_ID_MEDIA_CAM_PROFILING_GET_APPEND_REQ,
    MSG_ID_MEDIA_CAM_MAV_RESULT_IND,
    MSG_ID_MEDIA_CAM_MAV_CAPTURE_IMAGE_IND,
    MSG_ID_MEDIA_CAM_START_MAV_STITCH_REQ,
    MSG_ID_MEDIA_CAM_CAMERA_GYRO_RESULT_IND,
    MSG_ID_MED_CAM_CODE_END = MSG_ID_MEDIA_CAM_CAMERA_GYRO_RESULT_IND,
#endif

} msg_type;

typedef struct {
    kal_char *src_mod;
    msg_type msg_id;
} CAL_MSG;

extern QueueHandle_t cal_queue_handle;
extern CAL_STATE_ENUM CalState;

//extern kal_eventgrpid CalEventId;
extern kal_bool IsHwJpegEncSupport;
//extern kal_bool IsHwJpegDecSupport;

extern CAL_CAMERA_SOURCE_ENUM CurrentCameraId;
extern CAL_SCENARIO_ENUM CurrentScenarioId;
extern P_ISP_FUNCTION_STRUCT pfIspFunc;
extern IDP_CAL_INTF_FUNC_T MdpFunc;
extern P_IMAGE_SENSOR_FUNCTION_STRUCT pfImageSensor;
extern MM_ERROR_CODE_ENUM (* pfCalCallback) (CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
extern kal_bool MainSensorSupport, SubSensorSupport;
extern kal_uint32 MainSensorId, SubSensorId;
//extern P_CAL_FEATURE_CTRL_STRUCT pCalFeatureInPara, pCalFeatureOutPara;
extern CAL_SENSOR_SUPPORT_STRUCT MainSubCameraStatus;
extern P_IMAGE_SENSOR_LIST_STRUCT pImageSensorList;
extern CAL_SCENARIO_ENUM CurrentStillScenarioId;

extern P_ISP_SCENARIO_CTRL_STRUCT pIspScenarioCtrl;
extern IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT SensorScenarioParaIn;
extern CAL_MDP_PREVIEW_STRUCT CalPreviewInfo;
extern CAL_MDP_STILL_CAPTURE_STRUCT CalStillCaptureInfo;
//extern CAL_MDP_VIDEO_STRUCT CalVideoInfo;
extern ISP_FEATURE_CURRENT_ZOOM_INFO_STRUCT CurrentZoomInfo;

//extern PP_FEATURE_CONFIG_INFO_STRUCT        PpFeatureConfigPara;

//extern JPEG_ENCODER_HANDLE JpegWrapperHandle;
//extern JPEG_WRAPPER JpegWrapper;
extern kal_uint8 *pJpegEncodedDataAddr;
extern kal_uint32 JpegEncodedDataSize;

extern kal_uint32 CurrentStillCaptureStatus;     // to keep how many images (main/quick view/thumbnail) are ready for still capture
extern kal_uint32 CurrentStillCaptureMask;       // to keep how many images (main/quick view/thumbnail) has to be captured

extern kal_bool DisplayPreviewFrameFlag;
extern kal_uint32 IdpCalKey;

#if 0
extern Camera_Preview_Idp_Cal_Struct MdpPreviewPara;
extern Video_Encode_Idp_Cal_Struct MdpVideoPara;
extern Camera_Capture_Idp_Cal_Struct MdpStillCapturePara;
extern MM_IMAGE_ROTATE_ENUM MdpDisplayImageRotAngle;
#endif

extern kal_bool gCalPreviewPostProcPaused;  /* to avoid CPU resource conflict between CAL & VRT task */
extern kal_bool PostProcImageFromDisplayImage;
extern kal_bool gMdpPostProcTrigger; /* reduce dummy PpCtrl() calls in MdpCallback to optimize SD capture performance */
extern MM_IMAGE_ROTATE_ENUM DisplayImageRotateAngle;
// for MT6235 only, we need to use software rotation to avoid tearing by HW LCD rotate
extern MM_IMAGE_ROTATE_ENUM ImageSwRotateAngleForDisplay;

extern kal_uint32 VideoRotBuffAddr;
//extern MPL_BUFFER_INFO_T MplBuffInfo;

#ifdef __CAL_CAMERA_VIDEO_ROTATE_SUPPORT__
extern MM_IMAGE_ROTATE_ENUM SwVideoRotAngle;
#endif

extern kal_uint32 QuickViewImageDestBuffAddr;

extern kal_bool IsZsdScenario;

//JPEG_YUV_FORMAT_ENUM GetJpegWrapperFormat(MM_IMAGE_FORMAT_ENUM ImageFormat);
MM_ERROR_CODE_ENUM CalScenarioCtrl(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode,
                                   void *pCtrlPara, kal_uint16 CtrlParaLen);
kal_bool CalImageSwRotateDetect(P_CAL_FEATURE_CAMERA_QUERY_SW_ROTATE_IN_STRUCT pCamSwRotPara);

#if 0
///webcam related
extern CAL_FEATURE_WEBCAM_GET_VIDEO_SIZE_STRUCT CalWebcamVideoSize;
extern CAL_FEATURE_WEBCAM_GET_STILL_SIZE_STRUCT CalWebcamStillSize;
extern CAL_FEATURE_WEBCAM_GET_COMPRESSION_STRUCT CalWebcamCompression;
#endif
MM_ERROR_CODE_ENUM CalWebcamCtrl(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen);
MM_ERROR_CODE_ENUM CalWebcamFeatureCtrl(P_CAL_FEATURE_CTRL_STRUCT pFeatureIn,
                                        P_CAL_FEATURE_CTRL_STRUCT pFeatureOut, kal_uint32 FeatureOutLen);
void CalWebcamInit(void);

extern kal_bool CalPanoEnabled;

MM_IMAGE_ROTATE_ENUM GetRestRotAngle(MM_IMAGE_ROTATE_ENUM DesiredRotAngle,
                                     MM_IMAGE_ROTATE_ENUM AlreadyRotAngle);

MM_ERROR_CODE_ENUM CalFeatureCtrl(P_CAL_FEATURE_CTRL_STRUCT pFeatureIn,
                                  P_CAL_FEATURE_CTRL_STRUCT pFeatureOut,
                                  kal_uint32 FeatureOutLen, kal_uint32 *pRealFeaureOutLen);

/* For MODIS */
MM_ERROR_CODE_ENUM CalModisScenarioCtrl(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode,
                                        void *pCtrlPara, kal_uint16 CtrlParaLen);

MM_ERROR_CODE_ENUM CalModisFeatureCtrl(P_CAL_FEATURE_CTRL_STRUCT pFeatureIn,
                                       P_CAL_FEATURE_CTRL_STRUCT pFeatureOut,
                                       kal_uint32 FeatureOutLen, kal_uint32 *pRealFeatureOutLen);
/* For MODIS */

// cal_task_main.c
extern kal_bool WaitStartupCnfFlag;

#if 0
void CalSendIlm(module_type SrcModId, module_type DestModId, kal_uint16 MsgId,
                void *pLocalPara, void *pPeerBuff, kal_bool FixSrcModId);

// cal_camera_nvram.c
extern ISP_FEATURE_NVRAM_INFO_STRUCT CameraNvramInfo;
extern kal_uint32 MainSensorListIdx, SubSensorListIdx, MatvSensorListIdx, CurrentSensorListIdx;
extern kal_uint32 CurrentNvramFileLid, LastNvramFileLid;

void CalSendMsgToNvram(msg_type MsgId, kal_uint16 ef_id, void *data_ptr, kal_uint16 Length);
void CalNvramReadCnfHandle(ilm_struct *pIlmPtr);
void CalNvramWriteCnfHandle(ilm_struct *pIlmPtr);
#endif

kal_uint32 GetSensorListIdx(kal_bool BySensorId, kal_uint32 SensorId, IMAGE_SENSOR_INDEX_ENUM SensorIdx);

void CctLoadNvram(void);
void CctSaveNvram(void);

// cal_feature_ctrl.c
extern kal_uint32 CalFeatureCtrlBuff1[];
extern kal_uint32 CalFeatureCtrlBuff2[];
extern IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT CurrentSensorInfo;
extern kal_uint8 NextDigitalZoomStep;
extern CAL_CAMERA_FAST_ZOOM_STRUCT FastDigitalZoomPara;
extern CAL_CAMERA_DIGITAL_ZOOM_OUT_STRUCT CurrentDzInfo;
extern CAM_IMAGE_SIZE_ENUM CurrentStillCaptureSize;
extern CAL_CAMERA_GET_PREVIEW_IMAGE_STRUCT CamPreviewImageInfo;
extern kal_bool GetPreviewImageEnable;


MM_ERROR_CODE_ENUM CalPassSensorInfoToIsp(CAL_CAMERA_SOURCE_ENUM CameraId);
MM_ERROR_CODE_ENUM CalSearchSensor(void);

// cal_cb.c
MM_ERROR_CODE_ENUM IspCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM MdpCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
//kal_int32 JpegEncCallback(JPEG_CODEC_STATE_ENUM State);
//kal_int32 MjpegEncCallback(JPEG_CODEC_STATE_ENUM State);
//MM_ERROR_CODE_ENUM PpCallback(PP_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM JaiaCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM ImageRotCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM ImageResizeCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM PreviewImageCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM Y2RDitheringCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
MM_ERROR_CODE_ENUM StackTimeOutCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
kal_int32 CalWebcamCallback(kal_uint32 return_code);
MM_ERROR_CODE_ENUM ImageEffectCallback(CAL_CALLBACK_ID_ENUM CbId, void *pCallbackPara, kal_uint16 CallbackParaLen);
//void SendMsgToCal(msg_type MsgId, void *pLocalData);
void QuickViewImageRotate(kal_uint16 SrcWidth, kal_uint16 SrcHeight, kal_uint32 SrcBuffAddr,
                          kal_uint32 DestBuffAddr, MM_IMAGE_ROTATE_ENUM RotAngle);
#if 0//defined(IDP_MM_COLOR_SUPPORT)
void QuickViewImagePQ(void);
#endif
void CalHISR(void);
void DisplayBufferAcquireCallback(void);

// cal_scenario_ctrl.c
kal_bool CalExtMemSwitchCachableRegion(kal_uint32 *pMemAddr, kal_uint32 BufferSize, kal_bool CacheableFlag);

// cal_cct_feature_ctrl.c
MM_ERROR_CODE_ENUM CalCctFeatureCtrl(P_CAL_FEATURE_CTRL_STRUCT pFeatureIn,
                                     P_CAL_FEATURE_CTRL_STRUCT pFeatureOut,
                                     kal_uint32 FeatureOutLen, kal_uint32 *pRealFeatureOutLen);

//cal_profiling.c
void CameraCaptureProfilingWrapper(CAL_CAMERA_PROFILING_LOG_ENUM LogId);
MM_ERROR_CODE_ENUM CalProfilingFeatureCtrl(P_CAL_FEATURE_CTRL_STRUCT pFeatureIn,
                                     P_CAL_FEATURE_CTRL_STRUCT pFeatureOut,
                                     kal_uint32 FeatureOutLen, kal_uint32 *pRealFeatureOutLen);
//cal_webcam.c
void CalWebcamInit(void);
kal_int32 CalWebcamCallback(kal_uint32 return_code);
MM_ERROR_CODE_ENUM CalWebcamCtrl(CAL_SCENARIO_ENUM ScenarioId, kal_uint32 CtrlCode, void *pCtrlPara, kal_uint16 CtrlParaLen);
//kal_int32 CalWebcamSendFrameReadyRequest(JPEG_CODEC_STATE_ENUM return_code);

// image postproc effect

#endif /* __CAL_IF_H__ */
