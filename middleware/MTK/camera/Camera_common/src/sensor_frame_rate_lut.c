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

#if (defined(ISP_SUPPORT))
//#include "kal_release.h"
//#include "drv_features.h"
//#if (defined(DRV_ISP_6235_SERIES)&&(defined(MT6253E)||defined(MT6253L)))
//#include "isp_if.h"
//#include "typedefs.h"
//#include "sccb_v2.h"
//#include "sensor.h"
//#include "isp_if_hw.h"
//#include "cirq.h"
//#include "img_common_enum.h"

//#include "idp_test.h"

//#include "Sensor_common_interface.h"
#include <assert.h>
#include <string.h>
#include "sensor_frame_rate_lut.h"

#if 1//defined(__SENSOR_FRAME_RATE_SUPPORT__)

SENSOR_FRAMERATE_IN_STRUCT SensorFrameRateInputPara;

const SENSOR_FRAME_RATE_ISP_STRUCT IspLutPara[MAX_ISP_HW_LIMITATION_LUT_NO] = ISP_HW_LIMITATION_LUT;

const SENSOR_FRAME_RATE_CHIP_RECORD_STRUCT ChipPreviewLutPara[SENSOR_FRAME_CHIP_LUT_PREVIEW_NO] = SENSOR_FRAME_RATE_CHIP_PREIVEW_LUT;
const SENSOR_FRAME_RATE_CHIP_RECORD_STRUCT ChipCaptureLutPara[SENSOR_FRAME_CHIP_LUT_CAPTURE_NO] = SENSOR_FRAME_RATE_CHIP_CAPTURE_LUT;
const SENSOR_FRAME_RATE_CHIP_RECORD_STRUCT ChipMp4LutPara[SENSOR_FRAME_CHIP_LUT_MP4_NO] = SENSOR_FRAME_RATE_CHIP_MP4_LUT;

//const SENSOR_FRAME_RATE_CHIP_STRUCT ChipLutPara[SENSOR_FRAME_CHIP_LUT_TOTAL_NO] = SENSOR_FRAME_RATE_CHIP_LUT;
SENSOR_FRAME_RATE_CHIP_STRUCT ChipLutPara = SENSOR_FRAME_RATE_CHIP_LUT;
//const SENSOR_FRAME_RATE_MP4_STRUCT Mp4LutPara[MAX_SENSOR_FRAME_MP4_LUT_NO] = SENSOR_FRAME_RATE_MP4_LUT;
SENSOR_FRAME_RATE_OVERALL_STRUCT SensorFrameRateOverallPara =SENSOR_FRAME_RATE_OVERALL_LUT;

SENSOR_FRAME_RATE_ISP_STRUCT IspSensorFrameRateLimitation;

kal_bool LutIspMatch = KAL_FALSE, LutChipMatch = KAL_FALSE,	 LutVideoMatch = KAL_FALSE;

kal_bool SensorCommonIsDataFormatMatch(IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM TableDataFmt ,IMAGE_SENSOR_DATA_OUT_FORMAT_ENUM SensorDataFmt){

    kal_bool IsMatch = KAL_FALSE;
    switch(SensorDataFmt){
        case IMAGE_SENSOR_DATA_OUT_FORMAT_RAW_B_FIRST:
        case IMAGE_SENSOR_DATA_OUT_FORMAT_RAW_Gb_FIRST:
        case IMAGE_SENSOR_DATA_OUT_FORMAT_RAW_R_FIRST:
        case IMAGE_SENSOR_DATA_OUT_FORMAT_RAW_Gr_FIRST:
            if(TableDataFmt<= IMAGE_SENSOR_DATA_OUT_FORMAT_RAW_Gr_FIRST){
                IsMatch = KAL_TRUE;
            }
            break;
			
        case IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY :
        case IMAGE_SENSOR_DATA_OUT_FORMAT_VYUY :
        case IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV :
        case IMAGE_SENSOR_DATA_OUT_FORMAT_YVYU :
        case IMAGE_SENSOR_DATA_OUT_FORMAT_JPEG :
        case IMAGE_SENSOR_DATA_OUT_FORMAT_CbYCrY :
        case IMAGE_SENSOR_DATA_OUT_FORMAT_CrYCbY :
        case IMAGE_SENSOR_DATA_OUT_FORMAT_YCbYCr :
        case IMAGE_SENSOR_DATA_OUT_FORMAT_YCrYCb :
            if(TableDataFmt<= IMAGE_SENSOR_DATA_OUT_FORMAT_JPEG&&TableDataFmt>= IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY){
                IsMatch = KAL_TRUE;
            }

            break;
        case IMAGE_SENSOR_DATA_OUT_FORMAT_RGB565 :
            if(TableDataFmt== IMAGE_SENSOR_DATA_OUT_FORMAT_RGB565){
                IsMatch = KAL_TRUE;
            }

            break;
        default:
            break;			
    }

    return IsMatch;
}



MM_ERROR_CODE_ENUM SensorCommonGetFrameRate(SENSOR_FRAMERATE_IN_STRUCT *InPara, SENSOR_FRAMERATE_OUT_STRUCT *OutPara)
{
	kal_uint16 i=0;
	kal_uint16 ChipLutMaxNo=0;
	kal_bool CrzTwoPassEnable;
	//kal_uint16 LutIndex[2] = {0,0};
	MM_ERROR_CODE_ENUM ret = MM_ERROR_NONE;
	PSENSOR_FRAME_RATE_CHIP_SUB_STRUCT pSensorFrameRateChipSub = NULL;

    LutIspMatch = KAL_FALSE; 
    LutChipMatch = KAL_FALSE;
    LutVideoMatch = KAL_FALSE;
#if !(defined(MT6253E)||defined(MT6253L)) // currently only apply on MT6253EL
	//return MM_ERROR_SENSOR_FRAME_RATE_LUT_NOT_SUPPORT;
#endif
	//set default value
	memcpy(&OutPara->IspHwLimitation,&SensorFrameRateOverallPara.pIspLutPara[0].IspLimitPara, sizeof(SENSOR_FRAME_RATE_ISP_LIMIT_PARA_STRUCT));

	if(InPara->Scenario == CAL_SCENARIO_CAMERA_PREVIEW)
	{
		pSensorFrameRateChipSub = &SensorFrameRateOverallPara.pChipLutPara->pPreviewChipLUT[0];
		ChipLutMaxNo = SENSOR_FRAME_CHIP_LUT_PREVIEW_NO;
	}else if(InPara->Scenario == CAL_SCENARIO_CAMERA_STILL_CAPTURE)
	{
		pSensorFrameRateChipSub = &SensorFrameRateOverallPara.pChipLutPara->pCaptureChipLUT[0];
		ChipLutMaxNo = SENSOR_FRAME_CHIP_LUT_CAPTURE_NO;
	}else if(InPara->Scenario == CAL_SCENARIO_VIDEO)
	{
		pSensorFrameRateChipSub = &SensorFrameRateOverallPara.pChipLutPara->pVideoChipLUT[0];
		ChipLutMaxNo = SENSOR_FRAME_CHIP_LUT_MP4_NO;
	}else
	{
		assert(0);
	}
	OutPara->OtfCriticalDzFactor = (pSensorFrameRateChipSub+i)->MaxDzFactor;
	OutPara->MaxSensorFrameRate = (pSensorFrameRateChipSub+i)->MaxSensorFrameRate;
	CrzTwoPassEnable = (pSensorFrameRateChipSub+i)->CrzTwoPassEnable;

	//dbg_print("1.OutPara  DZ (%d) FrameRate(%d)  Crz2Pass (%d)\r\n",
		//OutPara->OtfCriticalDzFactor ,	OutPara->MaxSensorFrameRate , CrzTwoPassEnable);
	for(i=0; i<MAX_ISP_HW_LIMITATION_LUT_NO ; i++ )
	{
		
	
		if((SensorFrameRateOverallPara.pIspLutPara[i].CameraIf == InPara->CameraIf) &&
            //(SensorFrameRateOverallPara.pIspLutPara[i].DataFormat == InPara->DataFormat)&&
			((SensorFrameRateOverallPara.pIspLutPara[i].SourceWidth >= (InPara->SourceWidth)) &&
			 (SensorFrameRateOverallPara.pIspLutPara[i].SourceHeight >= (InPara->SourceHeight)))&&
			(((SensorFrameRateOverallPara.pIspLutPara[i].SourceWidth*15)>>4 <= InPara->SourceWidth ) &&
			 ((SensorFrameRateOverallPara.pIspLutPara[i].SourceHeight*15)>>4 <= InPara->SourceHeight ))
		){
                if(KAL_TRUE == SensorCommonIsDataFormatMatch(SensorFrameRateOverallPara.pIspLutPara[i].DataFormat,InPara->DataFormat))
                {
			LutIspMatch = KAL_TRUE;
			memcpy(&OutPara->IspHwLimitation,&SensorFrameRateOverallPara.pIspLutPara[i].IspLimitPara, sizeof(SENSOR_FRAME_RATE_ISP_LIMIT_PARA_STRUCT));
			break;
                }
		}

	}


	for(i=0; i<ChipLutMaxNo ; i++ )
	{
		if(((pSensorFrameRateChipSub+i)->CameraIf == InPara->CameraIf) &&
			//((pSensorFrameRateChipSub+i)->DataFormat == InPara->DataFormat)&&
			(((pSensorFrameRateChipSub+i)->SourceWidth >= (InPara->SourceWidth)) &&
			((pSensorFrameRateChipSub+i)->SourceHeight >= (InPara->SourceHeight)))&&
			((((pSensorFrameRateChipSub+i)->SourceWidth*15)>>4 <= InPara->SourceWidth ) &&
			 (((pSensorFrameRateChipSub+i)->SourceHeight*15)>>4 <= InPara->SourceHeight ))
		){

                if(KAL_TRUE == SensorCommonIsDataFormatMatch((pSensorFrameRateChipSub+i)->DataFormat ,InPara->DataFormat))
                {

			LutChipMatch = KAL_TRUE;
			OutPara->OtfCriticalDzFactor =(pSensorFrameRateChipSub+i)->OtfCriticalDzFactor ;
			if(InPara->NighhtMode)
				OutPara->MaxSensorFrameRate=(pSensorFrameRateChipSub+i)->NightModeFrameRate ;
			else
				OutPara->MaxSensorFrameRate=(pSensorFrameRateChipSub+i)->MaxSensorFrameRate ;


			CrzTwoPassEnable = (pSensorFrameRateChipSub+i)->CrzTwoPassEnable ;
			break;
                }
		}
	}

	//dbg_print("2.OutPara  DZ (%d) FrameRate(%d)  Crz2Pass (%d)\r\n",
		//OutPara->OtfCriticalDzFactor ,	OutPara->MaxSensorFrameRate , CrzTwoPassEnable);


	if(InPara->Scenario == CAL_SCENARIO_VIDEO){// should also reference ME1 Video frame rate table
		if(OutPara->MaxSensorFrameRate> (InPara->EncodeFramRate*(CrzTwoPassEnable+1)))
			OutPara->MaxSensorFrameRate = (InPara->EncodeFramRate*(CrzTwoPassEnable+1));
	}
	//dbg_print("3.LutIspMatch  (%d) LutChipMatch(%d)  LutVideoMatch (%d) [%d,%d,%d]\r\n",
		//LutIspMatch,LutChipMatch,LutVideoMatch,LutIndex[0],LutIndex[1],LutIndex[2]);

	if(/*(LutIspMatch==KAL_FALSE)||*/(LutChipMatch==KAL_FALSE))
	{
		ret =  MM_ERROR_SENSOR_FRAME_RATE_LUT_NOT_MATCH;
		//ASSERT(0);
	}
	return ret;

}

#if 0

SENSOR_FRAMERATE_IN_STRUCT	SensorCommonFrameIn;
SENSOR_FRAMERATE_OUT_STRUCT	SensorCommonFrameOut;
void SensorFrameRateTest(void){

	kal_uint32 ret=0;
	kal_uint16 para1,para2,para3;

	SensorCommonFrameIn.Scenario= CAL_SCENARIO_CAMERA_PREVIEW;
	SensorCommonFrameIn.CameraIf=  IMAGE_SENSOR_IF_PARALLEL;		
	SensorCommonFrameIn.DataFormat= IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY;		
	SensorCommonFrameIn.SourceWidth= 620;
	SensorCommonFrameIn.SourceHeight= 470;
	SensorCommonFrameIn.TargetWidth= 320;
	SensorCommonFrameIn.TargetHeight= 240;
	SensorCommonFrameIn.EncodeFramRate = 10;
	SensorCommonFrameIn.NighhtMode= KAL_FALSE;
	ret= SensorCommonGetFrameRate(&SensorCommonFrameIn, &SensorCommonFrameOut);
	//if(ret !=FRAME_RATE_LUT_SEARCH_SUCCESS)
		//ASSERT(0);

	SensorCommonFrameIn.Scenario= CAL_SCENARIO_CAMERA_PREVIEW;
	SensorCommonFrameIn.CameraIf=  IMAGE_SENSOR_IF_PARALLEL;		
	SensorCommonFrameIn.DataFormat= IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY;		
	SensorCommonFrameIn.SourceWidth= 470;
	SensorCommonFrameIn.SourceHeight= 310;
	ret = SensorCommonGetFrameRate(&SensorCommonFrameIn, &SensorCommonFrameOut);
	//if(ret !=FRAME_RATE_LUT_SEARCH_SUCCESS)
		//ASSERT(0);


	SensorCommonFrameIn.Scenario= CAL_SCENARIO_CAMERA_PREVIEW;
	SensorCommonFrameIn.CameraIf=  IMAGE_SENSOR_IF_PARALLEL;		
	SensorCommonFrameIn.DataFormat= IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY;		
	SensorCommonFrameIn.SourceWidth= 340;
	SensorCommonFrameIn.SourceHeight= 280;
	SensorCommonGetFrameRate(&SensorCommonFrameIn, &SensorCommonFrameOut);
	//if(ret !=FRAME_RATE_LUT_SEARCH_SUCCESS)
		//ASSERT(0);


return;

/*
	SensorCommonFrameIn.Scenario= CAM_SCENARIO_CAPTURE;
	SensorCommonFrameIn.CameraIf= SENSOR_IF_PARALLEL;
	SensorCommonFrameIn.DataFormat= SENSOR_DATA_FORMAT_YUV422_VYUY;		
	SensorCommonFrameIn.SourceWidth= 640;
	SensorCommonFrameIn.SourceHeight= 480;
	SensorCommonFrameIn.TargetWidth= 640;
	SensorCommonFrameIn.TargetHeight= 480;

	SensorCommonGetFrameRate(&SensorCommonFrameIn, &SensorCommonFrameOut);

	SensorCommonFrameIn.Scenario= CAM_SCENARIO_MP4;
	SensorCommonFrameIn.CameraIf= SENSOR_IF_PARALLEL;
	SensorCommonFrameIn.DataFormat= SENSOR_DATA_FORMAT_YUV422_VYUY;		
	SensorCommonFrameIn.SourceWidth= 640;
	SensorCommonFrameIn.SourceHeight= 480;
	SensorCommonFrameIn.TargetWidth= 176;
	SensorCommonFrameIn.TargetHeight= 144;

	SensorCommonGetFrameRate(&SensorCommonFrameIn, &SensorCommonFrameOut);

	SensorCommonFrameIn.Scenario= CAM_SCENARIO_MP4;
	SensorCommonFrameIn.CameraIf= SENSOR_IF_PARALLEL;
	SensorCommonFrameIn.DataFormat= SENSOR_DATA_FORMAT_YUV422_VYUY;		
	SensorCommonFrameIn.SourceWidth= 640;
	SensorCommonFrameIn.SourceHeight= 480;
	SensorCommonFrameIn.TargetWidth= 176;
	SensorCommonFrameIn.TargetHeight= 144;
*/
}
#endif


#endif/*__SENSOR_FRAME_RATE_SUPPORT__*/


//#endif /* MT6235 */
#endif /* ISP_SUPPORT */

