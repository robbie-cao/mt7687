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

#ifndef __SENSOR_COMM_DEF_H__
#define __SENSOR_COMM_DEF_H__

#if 0
#include "cal_comm_def.h"
#include "isp_comm_def.h"
#include "usbvideo_custom.h"
#endif

#include "cal_comm_def.h"
#include "isp_comm_def.h"

#define MAX_SENSOR_SUPPORT_NUMBER   5       /* Main/Main bak/sub/sub bak/MATV */
#define NULL_SENSOR_ID  (0xFFFFFFFF)

#define CAMERA_FEATURE_SUPPORT(n)       (1<<(n))

/* the sensor feature will be used by CAL and ISP driver only */
typedef enum
{
    IMAGE_SENSOR_FEATURE_BEGIN = CAL_SENSOR_FEATURE_BEGIN,
    IMAGE_SENSOR_FEATURE_GET_SENSOR_ID,             // In : NULL, Out : IMAGE_SENSOR_COMM_DATA_STRUCT, read the sensor ID from sensor.
    IMAGE_SENSOR_FEATURE_SET_CAMERA_SOURCE,         // In : IMAGE_SENSOR_SET_CAMERA_SOURCE_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_SET_REGISTER,              // In : IMAGE_SENSOR_REG_IN_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_GET_REGISTER,              // In : IMAGE_SENSOR_REG_IN_STRUCT, Out : IMAGE_SENSOR_REG_OUT_STRUCT
    IMAGE_SENSOR_FEATURE_GET_DATA_INFO,             // In : NULL, Out : SENSOR_REG_WIDTH_STRUCT
    IMAGE_SENSOR_FEATURE_GET_SENSOR_INFO,           // In : NULL, Out : IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT
    IMAGE_SENSOR_FEATURE_GET_MIPI_INFO,             //In : IMAGE_SENSOR_MIPI_PARA_IN_STRUCT, Out: IMAGE_SENSOR_MIPI_PARA_OUT_STRUCT
    // for parsing the JPEG inforation of JPEG sensor
    IMAGE_SENSOR_FEATURE_GET_JPEG_INFO,             // In : Out : TBD for MT6276
    // for those sensor that has to apply sensor's sub sample cability. especially for JPEG sensor
    IMAGE_SENSOR_FEATURE_GET_ZOOM_INFO,             // In : Out :, for JPEG sensor only, TBD for MT6276
    IMAGE_SENSOR_FEATURE_GET_SENSOR_FRAME_RATE,     // In: Null, Out:IMAGE_SENSOR_COMM_DATA_STRUCT, get current sensor frame rate, in video, it has to be passed to AE
    IMAGE_SENSOR_FEATURE_GET_SHADING_TABLE,         // In : SENSOR_SHADING_TABLE_IN_STRUCT, Out : SENSOR_SHADING_TABLE_OUT_STRUCT, get shading table from EEPROM

// check is it necessary to leave the nightmode features since the camera will stop then start
// when we switch between night mode and normal mode.
    IMAGE_SENSOR_FEATURE_CTRL_NIGHTMODE,            // In : IMAGE_SENSOR_COMM_CTRL_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_GET_PREVIEW_FLASHLIGHT_INFO,   //In : NULL , Out :FLASHLIGHT_PREVIEW_PARA_STURCT
    IMAGE_SENSOR_FEATURE_GET_CAPTURE_FLASHLIGHT_INFO,   //In : FLASHLIGHT_CAPTURE_SENSOR_PARA_IN_STRUCT, Out :FLASHLIGHT_CAPTURE_PARA_STURCT
    IMAGE_SENSOR_FEATURE_CTRL_FLASHLIGHT,           // In : IMAGE_SENSOR_COMM_CTRL_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_SET_ESHUTTER,              // In : IMAGE_SENSOR_COMM_DATA_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_SET_GAIN,                  // In : IMAGE_SENSOR_COMM_DATA_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_GET_GAIN,                  // In : IMAGE_SENSOR_COMM_DATA_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_GET_AE_INFO,               // In : NULL, Out : IMAGE_SENSOR_AE_INFO_STRUCT
    IMAGE_SENSOR_FEATURE_GET_AE_LUT,                // In : AE_LUT_INFO_STRUCT, Out : IMAGE_SENSOR_AE_LUT_STRUCT
    IMAGE_SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR,     // In : nvram_camera_para_struct, Out : NULL
    IMAGE_SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA,     // In : NULL, Out : nvram_camera_para_struct
    IMAGE_SENSOR_FEATURE_SET_NVRAM_SENSOR_INFO,     // In : nvram_camera_para_struct, Out : NULL

    IMAGE_SENSOR_FEATURE_GET_INIT_OPERATION_PARA,   // In : NULL, Out : IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT
    IMAGE_SENSOR_FEATURE_GET_DSC_SUPPORT_INFO,      // In : NULL, Out : IMAGE_SENSOR_GET_DSC_SUPPORT_INFO_STRUCT
    IMAGE_SENSOR_FEATURE_GET_CAPTURE_FORMAT,             // In : Out : TBD for MT6276
    IMAGE_SENSOR_FEATURE_GET_EV_AWB_REF,    // In : NULL, Out : SENSOR_AE_AWB_REF_STRUCT //__AUTO_SCENE_DETECT_SUPPORT__
    IMAGE_SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN,  // In : NULL, Out : SENSOR_AE_AWB_CUR_STRUCT  //__AUTO_SCENE_DETECT_SUPPORT__

    // for YUV sensor (YUV sensor only)
    IMAGE_SENSOR_FEATURE_YUV_BEGIN = CAL_SENSOR_FEATURE_BEGIN + 400,
    IMAGE_SENSOR_FEATURE_YUV_3A_CTRL,                   // In : IMAGE_SENSOR_COMM_CTRL_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_SET_AE_BYPASS,                 // In : AeBypassEnable, Out: NULL
    IMAGE_SENSOR_FEATURE_SET_AWB_BYPASS,                // In : AwbBypassEnable, Out: NULL
    IMAGE_SENSOR_FEATURE_VSYNC_HISR_INFORM_FRAME_TIME,	// In : IMAGE_SENSOR_COMM_DATA_STRUCT, Out: NULL
    IMAGE_SENSOR_FEATURE_GET_YUV_DATA_PIN_CONNECTION,   // In : NULL, Out : DATA_PIN_CONNECTION_ENUM
    IMAGE_SENSOR_FEATURE_GET_HW_DEPENDENT_SETTING,//In : NULL, Out: IMAGE_SENSOR_HW_DEPENDENT_STRUCT
    IMAGE_SENSOR_FEATURE_SET_SENSOR_POWER_DOWN,//In : NULL, Out: NULL
    IMAGE_SENSOR_FEATURE_GET_SERIAL_SENSOR_INFO,//In : SERIAL_SENSOR_INFO_IN_STRUCT, Out: SERIAL_SENSOR_INFO_OUT_STRUCT
    ////////for YUV LED flashlight control/////////////
    IMAGE_SENSOR_FEATURE_YUV_CHECK_IF_TRIGGER_FLASHLIGHT,           ///In: NULL,  Out: TRUE or FALSE
    IMAGE_SENSOR_FEATURE_QUERY_YUV_CTRL_FLASHLIGHT_TRIGGER_MODE,    ///In:   Out: IMAGE_SENSOR_COMM_DATA_STRUCT
    IMAGE_SENSOR_FEATURE_SET_YUV_CTRL_FLASHLIGHT_TRIGGER_MODE,      ///In: IMAGE_SENSOR_COMM_DATA_STRUCT  Out: NULL
    IMAGE_SENSOR_FEATURE_SET_SCENE_ENHANCE, // In : SENSOR_SCENE_ENHANCE_STRUCT, Out : NULL  //__AUTO_SCENE_DETECT_SUPPORT__
    /// for jpeg sensor
    IMAGE_SENSOR_FEATURE_SET_JPEG_BUFF_SIZE,             // In :   JPEG_SENSOR_SET_BUFF_IN_STRUCT   Out : NULL
    /////////////////end///////////////////////
    IMAGE_SENSOR_FEATURE_SET_IN_HISR,             // In :   IMAGE_SENSOR_SET_IN_HISR_STRUCT   Out : NULL
    IMAGE_SENSOR_FEATURE_SET_AF_BYPASS,                 // In : AfBypassEnable, Out: NULL

    // for engineering mode
    IMAGE_SENSOR_FEATURE_ENG_BEGIN= CAL_SENSOR_FEATURE_BEGIN + 600,
    IMAGE_SENSOR_FEATURE_SET_ENG_REGISTER,          // In : IMAGE_SENSOR_REG_IN_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_GET_ENG_REGISTER,          // In : IMAGE_SENSOR_REG_IN_STRUCT, Out : IMAGE_SENSOR_REG_OUT_STRUCT

    // for CCT mode (Raw sensor only)
    IMAGE_SENSOR_FEATURE_CCT_BEGIN= CAL_SENSOR_FEATURE_BEGIN + 700,
    IMAGE_SENSOR_FEATURE_GET_ENG_INFO,              // In : NULL, Out : SENSOR_INFO_STRUCT
    IMAGE_SENSOR_FEATURE_SET_CCT_REGISTER,          // In : IMAGE_SENSOR_REG_IN_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_GET_CCT_REGISTER,          // In : IMAGE_SENSOR_REG_IN_STRUCT, Out : IMAGE_SENSOR_REG_OUT_STRUCT
    IMAGE_SENSOR_FEATURE_GET_CCT_GROUP_INFO,        // In : NULL, Out : CAL_CCT_SENSOR_GET_ENG_GROUP_INFO_STRUCT
    IMAGE_SENSOR_FEATURE_GET_CCT_GROUP_COUNT,       // In : NULL, Out : IMAGE_SENSOR_COMM_DATA_STRUCT
    IMAGE_SENSOR_FEATURE_GET_CCT_ITEM_INFO,         // In : NULL, Out : CAL_CCT_SENSOR_ITEM_INFO_STRUCT
    IMAGE_SENSOR_FEATURE_SET_CCT_ITEM_INFO,         // In : CAL_CCT_SENSOR_ITEM_INFO_STRUCT, Out : NULL
    IMAGE_SENSOR_FEATURE_MAX,
} IMAGE_SENSOR_FEATURE_ENUM;

typedef enum
{
    IMAGE_SENSOR_MAIN,
    IMAGE_SENSOR_SUB,
    IMAGE_SENSOR_BAK1,
    IMAGE_SENSOR_SUB_BAK1,
    IMAGE_SENSOR_MATV,
    IMAGE_SENSOR_MAX,
} IMAGE_SENSOR_INDEX_ENUM;

typedef enum
{
    IMAGE_SENSOR_IF_NONE=0,
    IMAGE_SENSOR_IF_PARALLEL,
    IMAGE_SENSOR_IF_MIPI,
    IMAGE_SENSOR_IF_SERIAL,         // general serial interface that is defined by MTK
    IMAGE_SENSOR_IF_DIRECT,
} IMAGE_SENSOR_INTERFACE_ENUM;

typedef enum
{
    IMAGE_SENSOR_DATA_OUT_FORMAT_RAW_B_FIRST=1,
    IMAGE_SENSOR_DATA_OUT_FORMAT_RAW_Gb_FIRST,
    IMAGE_SENSOR_DATA_OUT_FORMAT_RAW_R_FIRST,
    IMAGE_SENSOR_DATA_OUT_FORMAT_RAW_Gr_FIRST,
    IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY,
    IMAGE_SENSOR_DATA_OUT_FORMAT_VYUY,
    IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV,
    IMAGE_SENSOR_DATA_OUT_FORMAT_YVYU,
    IMAGE_SENSOR_DATA_OUT_FORMAT_CbYCrY,
    IMAGE_SENSOR_DATA_OUT_FORMAT_CrYCbY,
    IMAGE_SENSOR_DATA_OUT_FORMAT_YCbYCr,
    IMAGE_SENSOR_DATA_OUT_FORMAT_YCrYCb,
    IMAGE_SENSOR_DATA_OUT_FORMAT_JPEG,
    IMAGE_SENSOR_DATA_OUT_FORMAT_RGB565,
} IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM;

typedef enum
{
    POLARITY_HIGH=0,
    POLARITY_LOW,
} SIGNAL_POLARITY_ENUM;

typedef enum
{
    DATA_PIN_CONNECT_BIT9_2=0,
    DATA_PIN_CONNECT_BIT7_0=1,
} DATA_PIN_CONNECTION_ENUM;

typedef enum
{
    SERIAL_1_LANE=0,
    SERIAL_2_LANE,
    SERIAL_3_LANE,//QQ
    SERIAL_4_LANE,
} SERIAL_LANE_NUMBER_ENUM;

// for IMAGE_SENSOR_FEATURE_GET_HW_DEPENDENT_SETTING
typedef struct
{
    DATA_PIN_CONNECTION_ENUM  DataPinConnection;  
    ISP_DRIVING_CURRENT_ENUM IspDrivingCurrent;

} IMAGE_SENSOR_HW_DEPENDENT_STRUCT, *P_IMAGE_SENSOR_HW_DEPENDENT_STRUCT;


// for IMAGE_SENSOR_FEATURE_SET_REGISTER, IMAGE_SENSOR_FEATURE_GET_REGISTER
// IMAGE_SENSOR_FEATURE_SET_CCT_REGISTER, IMAGE_SENSOR_FEATURE_GET_CCT_REGISTER,
// IMAGE_SENSOR_FEATURE_SET_ENG_REGISTER and IMAGE_SENSOR_FEATURE_GET_ENG_REGISTER
typedef struct
{
    kal_uint16  RegNum;  // to identify the number of Addr/Data pairs can be filled.
    kal_uint32 *pRegAddr;
    kal_uint32 *pRegData;
} IMAGE_SENSOR_REG_IN_STRUCT, *P_IMAGE_SENSOR_REG_IN_STRUCT;

// for IMAGE_SENSOR_FEATURE_GET_REGISTER
// IMAGE_SENSOR_FEATURE_GET_CCT_REGISTER,
// IMAGE_SENSOR_FEATURE_GET_ENG_REGISTER
typedef struct
{
    kal_uint16 ValidRegNum;     // number of pairs of addr/data are filled into the buffer
    kal_uint16 TotalRegNum;     // Total number of pairs of addr/data want to fill into the buffer
} IMAGE_SENSOR_REG_OUT_STRUCT, *P_IMAGE_SENSOR_REG_OUT_STRUCT;


// for IMAGE_SENSOR_FEATURE_GET_SENSOR_ID
// IMAGE_SENSOR_FEATURE_SET_ESHUTTER
// IMAGE_SENSOR_FEATURE_SET_GAIN
// IMAGE_SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ
// IMAGE_SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ
// IMAGE_SENSOR_FEATURE_GET_MAX_EXPOSURE_LINES,
// IMAGE_SENSOR_FEATURE_GET_MIN_EXPOSURE_LINES,
// IMAGE_SENSOR_FEATURE_GET_CCT_GROUP_COUNT
typedef struct
{
    kal_uint32 FeatureValue;
} IMAGE_SENSOR_COMM_DATA_STRUCT, *P_IMAGE_SENSOR_COMM_DATA_STRUCT;

// for IMAGE_SENSOR_FEATURE_SET_CAMERA_SOURCE
typedef struct
{
    IMAGE_SENSOR_INDEX_ENUM ImageSensorIdx;
} IMAGE_SENSOR_SET_CAMERA_SOURCE_STRUCT, *P_IMAGE_SENSOR_SET_CAMERA_SOURCE_STRUCT;

// for IMAGE_SENSOR_FEATURE_CTRL_NIGHTMODE
// IMAGE_SENSOR_FEATURE_CTRL_FLASHLIGHT
// IMAGE_SENSOR_FEATURE_CTRL_YUV_3A
typedef struct
{
    kal_bool FeatureEnable;
} IMAGE_SENSOR_COMM_CTRL_STRUCT, *P_IMAGE_SENSOR_COMM_CTRL_STRUCT;

// for IMAGE_SENSOR_FEATURE_GET_PERIOD
typedef struct
{
    kal_uint16 PixelNum;
    kal_uint16 LineNum;
} IMAGE_SENSOR_GET_PERIOD_STRUCT, *P_IMAGE_SENSOR_GET_PERIOD_STRUCT;

// for IMAGE_SENSOR_FEATURE_GET_INIT_OPERATION_PARA
typedef struct
{
	kal_uint8	CaptureDelayFrame;			/* wait stable frame when sensor change mode (pre to cap) */
	kal_uint8	PreviewDelayFrame;			/* wait stable frame when sensor change mode (cap to pre) */
	kal_uint8	PreviewDisplayWaitFrame;	/* Wait AE stable to display fist frame image */
	kal_uint8	AeCalDelayFrame;			/* The frame of calculation */
	kal_uint8	AeShutDelayFrame;           /* The frame of setting shutter */
	kal_uint8	AeSensorGainDelayFrame;	    /* The frame of setting sensor gain */
	kal_uint8	AeIspGainDelayFrame;        /* The frame of setting gain */
	kal_uint8	AeCalPeriod;				/* AE AWB calculation period */
	kal_uint16	PregainMax;             	/* max pregain */
	kal_uint16	IspPregainMax;              /* max isp gain */
	kal_uint16	SensorPregainMax;           /* max sensor gain */
	kal_uint16	PregainMin;					/* min pregain 1x gain */
	kal_uint16	IspPregainMin;				/* min isp gain 1x gain */
	kal_uint16	SensorPregainMin;	        /* min sensor gain 1x gain */
	kal_uint8	YuvAfTimeoutFrame;	        /* frame no of af result status checking for YUV sensor */
	kal_uint8	FlashlightMode;		        /* FLASH_LIGHT_TYPE */
	kal_uint8	FlashlightDeltaMainLum;     /* delta main lum@60cm / delta sub lum@60cm * 64 */
	kal_uint8   LedFlashlightRedeyeDelayFrame;    /*redeye delay frame of LED flashlight */
	kal_uint8	PreviewAeStableFrame;		/* wait stable frame when ae stable (pre) */

    ///for YUV flashlight control
    kal_uint8   FlashlightPrestrobeFrame;
    kal_uint8   FlashlightPrestrobeToRedeyeFrame;
    kal_uint8   FlashlightRedeyeStrobeFrame;
    kal_uint8   FlashlightMainStrobeDelayFrame;
} IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT, *P_IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT;

// for IMAGE_SENSOR_FEATURE_GET_DSC_SUPPORT_INFO
typedef struct
{
	camcorder_mode_info_struct dscmode;
	dsc_comp_info_struct dsccomp;
	flash_mode_info_struct flashlight;
	af_info_struct af;
}camcorder_info_struct, IMAGE_SENSOR_GET_DSC_SUPPORT_INFO_STRUCT, *P_IMAGE_SENSOR_GET_DSC_SUPPORT_INFO_STRUCT;

// for IMAGE_SENSOR_FEATURE_GET_SENSOR_INFO
// these information will be get at the beginning after select Main/Sub camera
typedef struct
{
    kal_uint32 SensorId;
    IMAGE_SENSOR_INTERFACE_ENUM SensorIf;
    IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM PreviewNormalDataFormat;
    IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM PreviewHMirrorDataFormat;
    IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM PreviewVMirrorDataFormat;
    IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM PreviewHVMirrorDataFormat;
    IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM CaptureDataFormat;
    SERIAL_LANE_NUMBER_ENUM SerialLaneNo;       // for MIPI and serial interfce only
    kal_bool PixelClkInv;
    SIGNAL_POLARITY_ENUM PixelClkPolarity;
    SIGNAL_POLARITY_ENUM HsyncPolarity;
    SIGNAL_POLARITY_ENUM VsyncPolarity;
    kal_uint32 PreviewMclkFreq;     // Mclk that is request from sensor
    kal_uint32 CaptureMclkFreq;     // Mclk that is request from sensor
    kal_uint32 VideoMclkFreq;       // Mclk that is request from sensor
    kal_uint16 PreviewWidth;
    kal_uint16 PreviewHeight;
    kal_uint16 FullWidth;
    kal_uint16 FullHeight;
    kal_bool SensorAfSupport;       // identify whether YUV sensor support AF or not
    kal_bool SensorFlashSupport;    // identify whether YUV sensor support Flash light or not    
    kal_bool IsSensorDriverCtrlMclk;  // sensor driver control mclk enable & disable
} IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT, *P_IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT;

// for IMAGE_SENSOR_FEATURE_GET_AE_INFO
typedef struct
{
    kal_uint16 DefaultPrevLinePixels;
    kal_uint16 CurrentPrevLinePixels;
    kal_uint32 CurrentPclk;
} IMAGE_SENSOR_AE_INFO_STRUCT, *P_IMAGE_SENSOR_AE_INFO_STRUCT;

// for IMAGE_SENSOR_FEATURE_GET_AE_LUT
// this part should be modified by 3A owners
typedef struct
{
	kal_uint8	lutsize;
	kal_uint16	step;		       /*Ev_step*1000, ex:0.1 -> 100*/
	kal_uint16	minEv;	              /*Ev*1000*/
	kal_uint16	maxEv;	              /*Ev*1000*/
	kal_uint8	iris;		                     /*No. of Iris*/
	kal_uint8	sensor_basegain;	/*Sensor base gain, bypass capture compensate when != 0x40*/
	kal_uint8	binning_enable;	/*binning enable*/
	kal_uint8	binning_offset;	/*binning offset*/
}ae_info_struct, AE_INFO_STRUCT,*P_AE_INFO_STRUCT;

typedef struct
{
	kal_bool	strobe_pol;
	kal_uint8	lutidx;
	kal_uint16	duty;
	kal_uint16	offset;
	kal_uint16	shutter;
	kal_uint16	min_shutter;
	kal_uint16	max_shutter;
	kal_uint16	fwb_rgain;
	kal_uint16	fwb_ggain;
	kal_uint16	fwb_bgain;
	kal_uint16	sensorgain;
	kal_uint8	ispgain;
}flash_info_struct;

typedef struct
{
	ae_info_struct		ae;
	flash_info_struct	autoflash;
}device_info_struct;
// this part should be modified by 3A owners

typedef struct
{
    device_info_struct *pDeviceInfo;
    const exposure_lut_struct **pAeLut;
} IMAGE_SENSOR_AE_LUT_STRUCT, *P_IMAGE_SENSOR_AE_LUT_STRUCT;

#if 0
// for IMAGE_SENSOR_FEATURE_GET_WEBCAM_SIZE_INFO
typedef struct
{
    USBVideo_Video_Size_Info *pWebcamVideoSizeInfo;
    USBVideo_Still_Size_Info *pWebcamStillSizeInfo;
    USBVideo_Compression_Info *pWebCamStillCompressInfo;
    USBVIDEO_VIDEO_SIZE_TYPE *pDefaultWebcamVideoSize;
    USBVIDEO_STILL_SIZE_TYPE *pDefaultWebcamStillSize;
    USBVIDEO_COMPRESSION_TYPE *pDefautlCompression;
} IMAGE_SENSOR_GET_WEBCAM_SIZE_INFO_STRUCT, *P_IMAGE_SENSOR_GET_WEBCAM_SIZE_INFO_STRUCT;
#endif

// for IMAGE_SENSOR_FEATURE_GET_SHADING_TABLE
/* start data structure for getting the shading table from sensor's EEPROM */
typedef enum
{
    SHADING_TABLE_FIXED=0,
    SHADING_TABLE_DYNAMIC,
    SHADING_TABLE_SINGLE_SLIM,
    SHADING_TABLE_NONE,         // do not apply ISP shading block function
    SHADING_TABEL_MAX
} SHADING_TABLE_TYPE_ENUM;

typedef enum
{
    SHADING_TALE_ROTATE_ANGLE_0=0,
    SHADING_TALE_ROTATE_ANGLE_180,
    SHADING_TABLE_ROTATE_ANGLE_MAX
} SHADING_TABLE_ROATE_ENUM;

typedef struct
{
    kal_uint16 PreShadingTableSize;          // < MAX_SHADING2_PRV_SIZE
    kal_uint16 CapShadingTableSize;          // < (MAX_SHADING1_CAP_SIZE+MAX_SHADING2_CAP_SIZE+MAX_DEFECT2_CAP_SIZE)
} FIXED_SHADING_TABLE_PARA_STRUCT, *P_FIXED_SHADING_TABLE_PARA_STRUCT;

typedef struct
{
    kal_uint8 ShadingBlockNumberX;
    kal_uint8 ShadingBlockNumberY;
    kal_uint16 ShadingBlockWidth;
    kal_uint16 ShadingBlockHeight;
    kal_uint16 LastShadingBlockWidth;
    kal_uint16 LastShadingBlockHeight;
} SINGLE_SLIM_SHADING_TABLE_PARA_STRUCT, *P_SINGLE_SLIM_SHADING_TABLE_PARA_STRUCT;

typedef struct
{
	// in order to fit the data structure of NVRAM, we separate the capture shading table into 3 parts.
    kal_uint32 *pPreShadingTable;        // buffer size = MAX_SHADING2_PRV_SIZE*4, SHADING2.previewTable, for fixed preview shading table and single slim shading table
    kal_uint32 *pCapShadingTable1;       // buffer size = MAX_SHADING1_CAP_SIZE*4, SHADING1.captureTable1, for dynamic shading table and fixed capture shading table part 1
    kal_uint32 *pCapShadingTable2;       // buffer size = MAX_SHADING2_CAP_SIZE*4, SHADING2.captureTable2, for fixed capture shading table part 2
    kal_uint32 *pCapShadingTable3;       // buffer size = MAX_DEFECT2_CAP_SIZE*4, DEFECT2.captureTable2, for fixed capture shading table part 3
} SENSOR_SHADING_TABLE_IN_STRUCT, *P_SENSOR_SHADING_TABLE_IN_STRUCT;

typedef struct
{
    // information of target shading table that will be applied on ISP hardware
    // for fixed shading table and dynamic shading table, the following items also
    // represent the information of calibrated shading table.
    kal_uint8 PreShadingBlockNumberX;
    kal_uint8 PreShadingBlockNumberY;
    kal_uint16 PreShadingBlockWidth;
    kal_uint16 PreShadingBlockHeight;
    kal_uint16 PreLastShadingBlockWidth;
    kal_uint16 PreLastShadingBlockHeight;
    kal_uint8 CapShadingBlockNumberX;
    kal_uint8 CapShadingBlockNumberY;
    kal_uint16 CapShadingBlockWidth;
    kal_uint16 CapShadingBlockHeight;
    kal_uint16 CapLastShadingBlockWidth;
    kal_uint16 CapLastShadingBlockHeight;

    // information of the calibrated shding table
    kal_bool ShadingTableSupport;
    SHADING_TABLE_TYPE_ENUM ShadingTableType;
    SHADING_TABLE_ROATE_ENUM ShadingTableAngle;

    FIXED_SHADING_TABLE_PARA_STRUCT         FixedShadingPara;
    SINGLE_SLIM_SHADING_TABLE_PARA_STRUCT   SingleSlimShadingPara;
} SENSOR_SHADING_TABLE_OUT_STRUCT, *P_SENSOR_SHADING_TABLE_OUT_STRUCT;
/* end of data structure for getting the shading table from sensor's EEPROM */

typedef enum
{
    CAMERA_SYSTEM_NORMAL_MODE=0,
    CAMERA_SYSTEM_META_MODE,
} CAMERA_SYSTEM_MODE_ENUM;

// Input parameters for SensorCtrl()
typedef struct
{
    IMAGE_SENSOR_MIRROR_ENUM ImageMirror;
    kal_uint16 ImageTargetWidth;            // image preview/capture/record width
    kal_uint16 ImageTargetHeight;           // image preview_capture_record height
    kal_uint16 ZoomFactor;                  // digital zoom factor
    kal_bool NightMode;                 // Enable/Disable night mode

    // capture only
    CAMERA_SYSTEM_MODE_ENUM MetaMode;   // Meta/Normal mode
    CAL_RAW_CAPTURE_MODE_ENUM CaptureScenario;  // for Raw sensor only
    kal_bool EnableShutterTransfer;
    kal_bool EnableFlashlightTransfer;
    kal_uint16 FlashlightDuty;
    kal_uint16 FlashlightOffset;
    kal_uint16 FlashlightShutFactor;
    kal_uint16 CaptureShutter;

    // for video scenario
    kal_uint32 MaxVideoFrameRate;       // maximum allowable video frame rate according to video encoder, quality, storage type,

    // for YUV sensor only
    kal_uint8 ContrastLevel;
    kal_uint8 BrightnessLevel;
    kal_uint8 SaturationLevel;
    CAL_CAMERA_WB_ENUM WbMode;
    CAL_CAMERA_EV_ENUM EvValue;
    CAL_CAMERA_BANDING_FREQ_ENUM BandingFreq;
    CAL_CAMERA_IMAGE_EFFECT_ENUM ImageEffect;
    kal_uint16 HueValue;
//    JPEG_ENCODE_QUALITTY_ENUM JpegEncQuality; // for jpeg sensor encode quality
    kal_uint32 JpegSensorBufferSize;
} IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT, *P_IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT;

// output parameters for SensorCtrl()
typedef struct
{
    kal_uint16 GrabStartX;	            /* The first grabed column data of the image sensor in pixel clock count */
    kal_uint16 GrabStartY;	            /* The first grabed row data of the image sensor in pixel clock count */
    kal_uint16 ExposureWindowWidth;	    /* Exposure window width of image sensor */
    kal_uint16 ExposureWindowHeight;    /* Exposure window height of image sensor */
    kal_uint16 ExposurePixel;           /* exposure window width of image sensor + dummy pixel */
    kal_uint16 CurrentExposurePixel;    /* exposure window width of image sensor + dummy pixel */
    kal_uint16 ExposureLine;            /* exposure window width of image sensor + dummy line */
    kal_uint16 SensorFrameRate;
    kal_uint8 WaitStableFrameNum;
} IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT, *P_IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT;

//input parameters for IMAGE_SENSOR_FEATURE_GET_MIPI_INFO
typedef struct
{
    CAL_SCENARIO_ENUM ScenarioMode;
    CAL_RAW_CAPTURE_MODE_ENUM CaptureScenario;  // for Raw sensor only
    kal_bool NightModeEnable;
    kal_uint32 DigitalZoomFactor;
    kal_uint32 TargetWidth;
    kal_uint32 TargetHeight;
}IMAGE_SENSOR_MIPI_PARA_IN_STRUCT, *P_IMAGE_SENSOR_MIPI_PARA_IN_STRUCT;

//output parameters for IMAGE_SENSOR_FEATURE_GET_MIPI_INFO
typedef struct
{
    kal_uint8 MipiLaneNum;
    kal_uint32 LowPwr2HighSpeedDelayCount;
}IMAGE_SENSOR_MIPI_PARA_OUT_STRUCT, *P_IMAGE_SENSOR_MIPI_PARA_OUT_STRUCT;


typedef struct
{
    kal_uint32	Addr;
    kal_uint32	Para;
} SENSOR_REG_STRUCT, *P_SENSOR_REG_STRUCT;
typedef struct
{
    kal_uint32	AddrWidth;
    kal_uint32	ParaWidth;
} SENSOR_REG_WIDTH_STRUCT, *P_SENSOR_REG_WIDTH_STRUCT;

#if defined (__AUTO_SCENE_DETECT_SUPPORT__)
typedef struct
{
    kal_uint16 AeRefLV05Shutter; /* Sensor AE Shutter under Lv05 */
    kal_uint16 AeRefLV13Shutter; /* Sensor AE Shutter under Lv13 */
    kal_uint16 AeRefLV05Gain; /* Sensor AE Gain under Lv05 */
    kal_uint16 AeRefLV13Gain; /* Sensor AE Gain under Lv13 */
} SENSOR_AE_REF_STRUCT, *P_SENSOR_AE_REF_STRUCT;


typedef struct
{
    kal_uint16 AwbRefD65Rgain; /* Sensor AWB R Gain under D65 */
    kal_uint16 AwbRefD65Bgain; /* Sensor AWB B Gain under D65 */
    kal_uint16 AwbRefCWFRgain; /* Sensor AWB R Gain under CWF */
    kal_uint16 AwbRefCWFBgain; /* Sensor AWB B Gain under CWF */
} SENSOR_AWB_GAIN_REF_STRUCT, *P_SENSOR_AWB_GAIN_REF_STRUCT;


typedef struct
{
    SENSOR_AE_REF_STRUCT  SensorAERef; /* AE Ref information for ASD usage */
    SENSOR_AWB_GAIN_REF_STRUCT  SensorAwbGainRef;  /* AWB Gain Ref information for ASD usage */
    kal_uint32	SensorLV05LV13EVRef; /* EV calculate  for ASD usage */
} SENSOR_AE_AWB_REF_STRUCT, *P_SENSOR_AE_AWB_REF_STRUCT;


typedef struct
{
    kal_uint16 AeCurShutter; /* Current Sensor AE Shutter */
    kal_uint16 AeCurGain; /* Current Sensor AE Gain */
} SENSOR_AE_CUR_STRUCT, *P_SENSOR_AE_CUR_STRUCT;


typedef struct
{
    kal_uint16 AwbCurRgain; /* Current Sensor AWB R Gain */
    kal_uint16 AwbCurBgain; /* Current Sensor AWB R Gain */
} SENSOR_AWB_GAIN_CUR_STRUCT, *P_SENSOR_AWB_GAIN_CUR_STRUCT;


typedef struct
{
    SENSOR_AE_CUR_STRUCT  SensorAECur; /* AE Current information for ASD usage */
    SENSOR_AWB_GAIN_CUR_STRUCT  SensorAwbGainCur;  /* AWB Gain Current information for ASD usage */
} SENSOR_AE_AWB_CUR_STRUCT, *P_SENSOR_AE_AWB_CUR_STRUCT;


typedef struct
{
    kal_uint8 SceneContrast; /* Scene Contrast */
    kal_uint8 SceneSaturation; /* Scene Saturation */
    kal_uint8 SceneSharpness; /* Scene Sharpness */
} SENSOR_SCENE_ENHANCE_STRUCT, *P_SENSOR_SCENE_ENHANCE_STRUCT;
#endif

typedef struct
{
   MM_ERROR_CODE_ENUM (* SensorOpen)(void);
   MM_ERROR_CODE_ENUM (* SensorFeatureCtrl) (kal_uint32 FeatureId, void* pFeatureParaIn, void* pFeatureParaOut,
                                             kal_uint32 FeatureParaOutLen, kal_uint32 *pRealFeatureParaOutLen);
   MM_ERROR_CODE_ENUM (* SensorCtrl) (CAL_SCENARIO_ENUM ScenarioId, void *pParaIn, void *pParaOut, kal_uint32 ParaOutLen,
                                      kal_uint32 *pRealParaOutLen);
   MM_ERROR_CODE_ENUM (* SensorClose)(void);
} IMAGE_SENSOR_FUNCTION_STRUCT, *P_IMAGE_SENSOR_FUNCTION_STRUCT;

typedef struct
{
    kal_uint32 SensorId;
    MM_ERROR_CODE_ENUM (* SensorFunc) (P_IMAGE_SENSOR_FUNCTION_STRUCT *pfSensorFunc);
    IMAGE_SENSOR_INDEX_ENUM SensorIdx;
} IMAGE_SENSOR_LIST_STRUCT, *P_IMAGE_SENSOR_LIST_STRUCT;


typedef struct
{
    kal_uint16  GroupIdx;
    kal_uint16  ItemIdx;
    kal_uint8   ItemNamePtr[50];     	// item name
    kal_uint32 	ItemValue;              // item value
    kal_bool    IsTrueFalse;            // is this item for enable/disable functions
    kal_bool    IsReadOnly;             // is this item read only
    kal_bool    IsNeedRestart;          // after set this item need restart
    kal_uint32  Min;                    // min value of item value
    kal_uint32  Max;                    // max value of item value
} CAL_CCT_SENSOR_ITEM_INFO_STRUCT, *P_CAL_CCT_SENSOR_ITEM_INFO_STRUCT;


typedef struct
{
	kal_uint32 	WorkingBuffAddress;		/* The working buffer address for parsing jpeg file */
	kal_uint32 	WorkingBuffSize;	
	kal_uint32 	ImageBuffAddress;		/* The image buffer address used for dump jpeg data to memory */
	kal_uint32 	ImageBuffSize;	

}JPEG_SENSOR_SRC_BUFFER_STRUCT, *PJPEG_SENSOR_SRC_BUFFER_STRUCT;

typedef struct
{
	kal_uint32 	JpegDataAddress;		/* The jpeg header & copressed data start address. 0xFF 0xD8....0xFF 0xD9 */
	kal_uint32 	JpegDataSize;
	kal_bool 	IsHeaderSupport;			/* KAL_TRUE: support jpeg header, KAL_FALSE: don't support jpeg header. */
	kal_bool 	IsThumbnailSupport;		
	kal_uint32 	JpegHeaderAddress;		/* The jpeg header buffer address used to store the jpeg header */
	kal_uint32 	JpegHeaderSize;
	kal_uint32 	JpegImageWidth;				/* The captured jpeg image size, such as 2048x1536 */
	kal_uint32 	JpegImageHeight;
	kal_uint32 	ThumbnailAddress;		/* The start address of the thumbnail buffer */
	kal_uint32 	ThumbnailSize;
	kal_uint32 	ThumbnailWidth;			/* Thumbnail resolution, such as 320x240, the size should be 320*2*240 if use RGB565 format */
	kal_uint32 	ThumbnailHeight;
	IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM 	ThumbnailFormat;			/* RGB565, RGB888, YUV422... */
} JPEG_SENSOR_PARSE_DATA_STRUCT, *PJPEG_SENSOR_PARSE_DATA_STRUCT;
/* end - data structure for JPEG sensor & new digital zoom interface */

typedef struct
{
    kal_int32 JpegSensorBufferSize;
} JPEG_SENSOR_SET_BUFF_IN_STRUCT, *P_JPEG_SENSOR_SET_BUFF_IN_STRUCT;



typedef struct
{
    IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM DataType;
} IMAGE_SENSOR_CAPTURE_FORMAT_STRUCT,  *P_IMAGE_SENSOR_CAPTURE_FORMAT_STRUCT;

typedef enum
{
    DELAYFRAME=0,
    DELAYTIME
}SENSOR_AF_DELAY_TYPE_ENUM;

typedef struct
{
    SENSOR_AF_DELAY_TYPE_ENUM type;
    kal_int32                count;
}SENSOR_AF_DELAY_STRUCT,*P_SENSOR_AF_DELAY_STRUCT;


typedef struct
{
    CAL_SCENARIO_ENUM ScenarioId;
} SERIAL_SENSOR_INFO_IN_STRUCT, *P_SERIAL_SENSOR_INFO_IN_STRUCT;


typedef struct
{
	kal_bool		para_1;/* clk_inv */
	kal_uint8		para_2;/* cyc */
	kal_uint16	para_3;/* sensor_data_width */
	kal_uint16	para_4;/* sensor_data_height */
	kal_uint8		para_5; //lane number
	kal_bool		para_6;//DDR supprt
	kal_bool 		para_7;//CRC support
} SERIAL_SENSOR_INFO_OUT_STRUCT, *P_SERIAL_SENSOR_INFO_OUT_STRUCT;
/* SENSOR_FEATURE_GET_INTERFACE_INFO, Output */

#endif /* __SENSOR_COMM_DEF_H__ */
