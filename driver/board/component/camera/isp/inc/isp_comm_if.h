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

#ifndef __ISP_COMM_IF_H__
#define __ISP_COMM_IF_H__

//#include "kal_release.h"
//#include "isp_nvram.h"
#include "sensor_comm_def.h"
#include "isp_comm_def.h"



typedef enum
{
	PCA_LUT_LO = 0,              /* low color temperature */
	PCA_LUT_MD,                  /* medium color temperature */
	PCA_LUT_HI,                  /* high color temperature */
       PCA2_LUT_LO,        /* low color temperature for PCA2 */ //__AUTO_SCENE_DETECT_SUPPORT__
       PCA2_LUT_MD,        /* medium color temperature for PCA2 */ //__AUTO_SCENE_DETECT_SUPPORT__	
       PCA2_LUT_HI,        /* high color temperature for PCA2 */ //__AUTO_SCENE_DETECT_SUPPORT__	       
	PCA_LUT_TYPE_NUM
} PCA_TABLE_TYPE;

/* OBJ AF Mode Selection */
typedef enum
{
	OBJ_CONT_OFF,     /* while FD on, cont.AF awlays off */
	OBJ_CONT_FULL_ON, /* while FD on, cont.AF awlays on no matter if object is detected */
	OBJ_CONT_FACE_ON, /* while FD on, cont.AF only on while object is detected */
	OBJ_CONT_MODE_NUM
}AF_OBJ_MODE_ENUM;





typedef struct
{
	kal_uint8	capture_delay_frame;			/* wait stable frame when sensor change mode (pre to cap) */
	kal_uint8	preview_delay_frame;			/* wait stable frame when sensor change mode (cap to pre) */
	kal_uint8	preview_display_wait_frame;		/* Wait AE stable to display fist frame image */
	kal_uint8	ae_cal_delay_frame;				/* The frame of calculation */
	kal_uint8	ae_shut_delay_frame;			/* The frame of setting shutter */
	kal_uint8	ae_sensor_gain_delay_frame;	/* The frame of setting sensor gain */
	kal_uint8	ae_isp_gain_delay_frame;		/* The frame of setting gain */
	kal_uint8	ae_cal_period;					/* AE AWB calculation period */
	kal_uint16	pregain_max;					/* max pregain */
	kal_uint16	isp_pregain_max;				/* max isp gain */
	kal_uint16	sensor_pregain_max;			/* max sensor gain */
	kal_uint16	pregain_min;					/* min pregain 1x gain */
	kal_uint16	isp_pregain_min;				/* min isp gain 1x gain */
	kal_uint16	sensor_pregain_min;				/* min sensor gain 1x gain */
	kal_uint8	yuv_af_timeout_frame;			/* frame no of af result status checking for YUV sensor */
	kal_uint8	flashlight_mode;		            		/* FLASH_LIGHT_TYPE */		
	kal_uint8	flashlight_delta_main_lum;          	/* delta main lum@60cm / delta sub lum@60cm * 64 */
	kal_uint8   	led_flashlight_redeye_delay_frame;    /*redeye delay frame of LED flashlight */
	kal_uint8	preview_ae_stable_frame;		/* wait stable frame when ae stable (pre) */	

    ///for YUV LED flashlight
    kal_uint8   flashlight_prestrobe_frame;
    kal_uint8   flashlight_prestrobe_to_redeyestrobe_delay_frame;
    kal_uint8   flashlight_redeyestrobe_frame;
    kal_uint8   flashlight_mainstrobe_frame;
}CameraDriverOperationParaStruct, CAMERA_DRIVER_OPERATION_PARA_STRUCT, *P_CAMERA_DRIVER_OPERATION_PARA_STRUCT;

typedef struct
{
	kal_bool	enable_cap_shutter_compensate;	/* KAL_TRUE, KAL_FALSE */
	kal_uint16	shutter_compensate_pregain_max;/* For Capture Mode, comp max pregain */
	kal_uint8	bracket_expo_ev_step;         		 /* bracketing 0.1 Ev step */	
	kal_bool	preview_defect_table_enable;	/* KAL_TRUE or KAL_FALSE */
	kal_bool	capture_defect_table_enable;	/* KAL_TRUE or KAL_FALSE */
	kal_bool	preview_shading_comp_enable;	/* KAL_TRUE or KAL_FALSE */
	kal_bool	capture_shading_comp_enable;	/* KAL_TRUE or KAL_FALSE */
        kal_bool	pca_table_enable;	/* KAL_TRUE or KAL_FALSE */
	kal_bool 	exif_iso_report_standard_enable;/* ISO priority EXIF with report real ISO gain enable flag */
	kal_bool	dynamic_lsc_support_enable;	/* dynamic lens shading compensaiton en/disable flag, KAL_TRUE or KAL_FALSE */	
	kal_uint8	video_metering_mode;/* CAM_AE_METER_AUTO or CAM_AE_METER_AVERAGE */
	kal_uint8	af_aled_ae_idx_thr;/* AE table index range, below the threshold, af auxiliary led will emit */
    kal_uint8        af_obj_mode_selection; /* TURE/FALSE, enable obj cont tracking focus */
    kal_uint8       af_obj_size_refocus_sens_level; /* 0~2, sensitivity LO=0, MD=1, HI=2 */
    kal_uint8       af_obj_fv_refocus_sens_level;	/* 0~2, sensitivity LO=0, MD=1, HI=2 */
    kal_bool        af_obj_fv_refocus_enable_flag; /* TURE/FALSE, enable obj fv refocus, set false, only size refocus exists */
	kal_uint8	AfAledEvCompOffset; /* ev compensate -af_aled_ev_comp_offset when AF auxiliary LED on */
	kal_bool		AeFaceAeMeteringEnable;
	kal_uint8       AeFaceMeanLowbound;
	kal_uint8       AeFaceMeanHighbound;
	kal_bool		AeLsbCheckEnable;
	kal_uint8       AeLsbTargetMean;
	kal_uint8       AeLsbBrightThreshold;
	kal_uint8		AeLsbDarkThreshold;
	kal_uint8       AeCwmTargetMean; /* target mean for other meter modes */
	kal_uint8       AeAvgTargetMean;
	kal_uint8       AeSpotTargetMean;
}CameraTuningOperationParaStruct, CAMERA_TUNING_OPERATION_PARA_STRUCT, *P_CAMERA_TUNING_OPERATION_PARA_STRUCT;


typedef struct {
	kal_uint32 reg_info0;
	kal_uint32 reg_info1;
	kal_uint32 *src_tbl_addr;
	kal_uint32 *dst_tbl_addr;
} TBL_INFO_T;


extern CameraDriverOperationParaStruct CameraDriverOperParaBuffer;
extern CameraTuningOperationParaStruct CameraTuningOperParaBuffer;


void CameraInitOperationPara(void);
void CameraDriverInitOperationPara(P_CAMERA_DRIVER_OPERATION_PARA_STRUCT pOperData);
void CameraTuningInitOperationPara(IMAGE_SENSOR_INDEX_ENUM SensorIdx, P_CAMERA_TUNING_OPERATION_PARA_STRUCT pOperData);


#if 0
nvram_camera_para_struct *IspGetNvramCameraPara(void);
const nvram_camera_para_struct *IspGetNvramDefaultCameraPara(void);

nvram_camera_3a_struct *IspGetNvramCamera3A(void);
nvram_camera_defect1_struct *IspGetNvramCameraDefect1(void);
nvram_camera_defect2_struct *IspGetNvramCameraDefect2(void);
nvram_camera_shading1_struct *IspGetNvramCameraShading1(void);
nvram_camera_shading2_struct *IspGetNvramCameraShading2(void);

const nvram_camera_3a_struct *IspGetNvramDefaultCamera3a(void);
const nvram_camera_defect1_struct *IspGetNvramDefaultCameraDefect1(void);
const nvram_camera_defect2_struct *IspGetNvramDefaultCameraDefect2(void);
const nvram_camera_shading1_struct *IspGetNvramDefaultCameraShading1(void);
const nvram_camera_shading2_struct *IspGetNvramDefaultCameraShading2(void);

nvram_camera_pca_struct *IspGetNvramCameraPca(void);
const nvram_camera_pca_struct *IspGetNvramDefaultCameraPca(void);
#endif

/* interface to isp apply/update/load pca */
void apply_camera_pca_to_reg(kal_uint32 *addr);
void apply_camera_pca(kal_uint8 pcaidx,kal_bool PcaTblUpdate);
void update_camera_pca_from_reg(ISP_OPERATION_STATE_ENUM IspState, kal_uint32 *Addr);
void pcaUpdateLUT(void);

extern void lscCalTbl(TBL_INFO_T tbl_info);
void rotate_fixed_shading_table(kal_uint32 *pshading_table_addr, kal_uint16 shading_table_size);
void rotate_slim_shading_table(kal_uint16 block_number_x, kal_uint16 block_number_y, kal_uint32 *data);
void resize_slim_shading_table(kal_uint16 data_src_width, kal_uint16 data_src_height,
				               kal_uint16 data_tar_width, kal_uint16 data_tar_height,
				               kal_uint32 *src_data, kal_uint32 *tar_data);



// isp_nvram.c
#if 0
extern nvram_camera_para_struct MainCameraPara, *pCameraPara;
extern nvram_camera_3a_struct	MainCamera3A, *pCamera3A;
extern nvram_camera_defect1_struct	MainCameraDefect1, *pCameraDefect1;
extern nvram_camera_defect2_struct	MainCameraDefect2, *pCameraDefect2;
extern nvram_camera_shading1_struct  MainCameraShading1, *pCameraShading1;
extern nvram_camera_shading2_struct  MainCameraShading2, *pCameraShading2;
extern nvram_camera_pca_struct  MainCameraPca, *pCameraPca;
#if defined (DRV_ISP_PCA_SUPPORT) && defined (__AUTO_SCENE_DETECT_SUPPORT__)
extern nvram_camera_pca_struct  MainCameraPca2;
#endif		
extern nvram_camera_para_struct SubCameraPara;
extern nvram_camera_3a_struct	SubCamera3A;
extern nvram_camera_defect1_struct	SubCameraDefect1;
extern nvram_camera_defect2_struct	SubCameraDefect2;
extern nvram_camera_shading1_struct  SubCameraShading1;
extern nvram_camera_shading2_struct  SubCameraShading2;
extern nvram_camera_pca_struct  SubCameraPca;
#endif


#endif /* __ISP_COMM_IF_H__ */
