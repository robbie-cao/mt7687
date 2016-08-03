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

#include "isp_comm_if.h"
#include "camera_sccb.h"
#include "image_sensor.h"
#include "cal_comm_def.h"
#include "isp_if.h"
#include "hal_i2c_master.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "string.h"

#if defined(DUAL_CAMERA_SUPPORT)
extern kal_bool camera_front_or_back;
#endif
#define  SP0820_DAYLIGHT_MAXGAIN             0x70
#define  SP0820_NIGHT_MAXGAIN                0xa0

static struct
{
  kal_bool BypassAe;
  kal_bool BypassAwb;
  
  kal_bool CapState; /* KAL_TRUE: in capture state, else in preview state */
  kal_bool PvMode; /* KAL_TRUE: in preview mode, else in capture mode */
  kal_uint8 BandingFreq; /* SP0820_SERIAL_50HZ or SP0820_SERIAL_60HZ for 50Hz/60Hz */
  
  kal_uint32 InternalClock; /* internal clock which using process pixel(for exposure) */
  kal_uint32 Pclk; /* output clock which output to baseband */
  kal_uint32 Gain; /* base on 0x40 */
  kal_uint32 Shutter; /* unit is (linelength / internal clock) s */
  
  kal_uint32 FrameLength; /* total line numbers in one frame(include dummy line) */
  kal_uint32 LineLength; /* total pixel numbers in one line(include dummy pixel) */
  
  IMAGE_SENSOR_INDEX_ENUM SensorIdx;
//  sensor_data_struct *NvramData;
  
  kal_uint16 MinFrameRate;	/* 150 means 15.0fps. */
  kal_uint16 MaxFrameRate;	/* 300 means 30.0fps. */
  
  kal_uint32 dummy_lines;
  kal_uint32 dummy_pixels;
  
  kal_uint32 curr_frame_time;	/* It's get from the TG Int. */
	
  kal_bool sccb_opened;			/* The I2C can work when sccb_opened is KAL_TRUE */
} SP0820_SERIALSensor;

kal_bool video_mode = KAL_FALSE;
kal_uint8 SP0820_SERIAL_CAM_BANDING_50HZ = CAM_BANDING_60HZ;
kal_bool SP0820_SERIAL_CAM_nightmode = KAL_FALSE;


/*************************************************************************
* FUNCTION
*   SP0820_SERIALSetMirror
*
* DESCRIPTION
*   This function set the mirror to the CMOS sensor
*
* PARAMETERS
*   Mirror
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALSetMirror(kal_uint8 Mirror)
{
  kal_uint8 CntrB;
  
  switch (Mirror)
  {
  case IMAGE_SENSOR_MIRROR_NORMAL:
  	CamWriteCmosSensor(0x31, 0x00); 
    break;
  case IMAGE_SENSOR_MIRROR_H:
    CamWriteCmosSensor(0x31, 0x02); 
    break;
  case IMAGE_SENSOR_MIRROR_V:
    CamWriteCmosSensor(0x31, 0x04); 
    break;
  case IMAGE_SENSOR_MIRROR_HV:
    CamWriteCmosSensor(0x31, 0x06); 
    break;
  default:
    /* no need to do error handling */
    break;
  }
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSetClock
*
* DESCRIPTION
*   This function set sensor internal clock and output clock
*
* PARAMETERS
*   Clk: internal clock
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALSetClock(kal_uint32 InternalClock)
{
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALWriteShutter
*
* DESCRIPTION
*   This function apply shutter to sensor
*
* PARAMETERS
*   Shutter: integration time, unit: (linelength / internal clock) s
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALWriteShutter(kal_uint32 Shutter)
{
    // TODO: set integration time here
	//CamWriteCmosSensor(0x03, Shutter >> 8);
	//CamWriteCmosSensor(0x04, Shutter);    
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALReadShutter
*
* DESCRIPTION
*   This function get shutter from sensor
*
* PARAMETERS
*   None
*
* RETURNS
*   Shutter: integration time, unit: (linelength / internal clock) s
*
* LOCAL AFFECTED
*
*************************************************************************/
static kal_uint32 SP0820_SERIALReadShutter(void)
{
    // TODO: read integration time here
	//return (CamReadCmosSensor(0x03) << 8)|CamReadCmosSensor(0x04);
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALWriteGain
*
* DESCRIPTION
*   This function apply global gain to sensor
*
* PARAMETERS
*   Gain: base on 0x40
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALWriteGain(kal_uint32 Gain)
{
    // TODO: set global gain here
    
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALReadGain
*
* DESCRIPTION
*   This function get global gain from sensor
*
* PARAMETERS
*   None
*
* RETURNS
*   Gain: base on 0x40
*
* LOCAL AFFECTED
*
*************************************************************************/
static kal_uint32 SP0820_SERIALReadGain(void)
{
    // TODO: read global gain here
    
  //  return 0x40;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALAeEnable
*
* DESCRIPTION
*   disable/enable AE
*
* PARAMETERS
*   Enable
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALAeEnable(kal_bool Enable)
{
	if (SP0820_SERIALSensor.BypassAe)
	{
		Enable = KAL_FALSE;
	}
	
	// TODO: enable or disable AE here
	{
		kal_uint16 temp_ae_reg = CamReadCmosSensor(0x32);
		if (Enable)
		{
	    		CamWriteCmosSensor(0x32, (temp_ae_reg | 0x01));
		}
		else
		{
	    		CamWriteCmosSensor(0x32, (temp_ae_reg & (~0x01))); /* Turn OFF AEC/AGC*/
		}
	}
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALAwbEnable
*
* DESCRIPTION
*   disable/enable awb
*
* PARAMETERS
*   Enable
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALAwbEnable(kal_bool Enable)
{
	if (SP0820_SERIALSensor.BypassAwb)
	{
		Enable = KAL_FALSE;
	}
	
	// TODO: enable or disable AWB here
	{
		kal_uint16 temp_awb_reg = CamReadCmosSensor(0x32);
		if (Enable)
		{
	    		CamWriteCmosSensor(0x32, (temp_awb_reg | 0x02));
		}
		else
		{
	    		CamWriteCmosSensor(0x32, (temp_awb_reg & (~0x02))); /* Turn OFF AWB*/
		}
	}
}


/*************************************************************************
* FUNCTION
*   SP0820_SERIALSetBandingStep
*
* DESCRIPTION
*   This function is to calculate & set shutter step register .
*
*************************************************************************/
static void SP0820_SERIALSetBandingStep(CAL_CAMERA_BANDING_FREQ_ENUM BandingFreq)
{
    // TODO: set banding step here
    
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSetDummy
*
* DESCRIPTION
*   This function set the dummy pixels(Horizontal Blanking) & dummy lines(Vertical Blanking), it can be
*   used to adjust the frame rate or gain more time for back-end process.
*
* PARAMETERS
*   DummyPixel
*   DummyLine
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALSetDummy(kal_uint32 DummyPixel, kal_uint32 DummyLine)
{
#if 0
    kal_uint32 LineLength, FrameLength;

    if (SP0820_SERIALSensor.PvMode) /* preview size output mode */
    {
        LineLength = DummyPixel + SP0820_SERIAL_PV_PERIOD_PIXEL_NUMS;
        FrameLength = DummyLine + SP0820_SERIAL_PV_PERIOD_LINE_NUMS;
    }
    else
    {
        LineLength = DummyPixel + SP0820_SERIAL_FULL_PERIOD_PIXEL_NUMS;
        FrameLength = DummyLine + SP0820_SERIAL_FULL_PERIOD_LINE_NUMS;
    }

    // TODO: set linelength/framelength or dummy pixel/line and return right value here
    {
        
    }
#endif
    /* config banding step or base shutter */
  //  if (SP0820_SERIALSensor.PvMode) /* preview size output mode */
  //  {
   //     SP0820_SERIALSetBandingStep();
   // }
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSetMinFps
*
* DESCRIPTION
*   This function calculate & set min frame rate
*
* PARAMETERS
*   Fps: min frame rate, base on SP0820_SERIAL_FPS(x)
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALSetMinFps(kal_uint16 Fps)
{
    // TODO: set max exposure time or max AE index here
    
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSetMaxFps
*
* DESCRIPTION
*   This function calculate & set max frame rate
*
* PARAMETERS
*   Fps: max frame rate, base on SP0820_SERIAL_FPS(x)
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALSetMaxFps(kal_uint16 Fps)
{
}



/*************************************************************************
* FUNCTION
*   SP0820_SERIALSetNightMode
*
* DESCRIPTION
*   This function switch on/off night mode of SP0820_SERIAL.
*
*************************************************************************/
static void SP0820_SERIALSetVideoFps(kal_uint16 Fps)
{
    // TODO: fix, set max exposure time or max AE index here
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALNightMode
*
* DESCRIPTION
*   This function set night mode to sensor.
*
* PARAMETERS
*   Enable
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALNightMode(kal_bool videomode,kal_uint8 banding,kal_bool nightmode)
{
	
	if (KAL_FALSE ==nightmode) //daylight
	{	
		if(KAL_TRUE == videomode)	
			{
				if(banding == CAM_BANDING_50HZ)
				{
					//Video record daylight 24M 50hz fix 14FPS 
					CamWriteCmosSensor(0x03,0x00); 		 
					CamWriteCmosSensor(0x04,0x8d); 	
					CamWriteCmosSensor(0x05,0x00); 		 
					CamWriteCmosSensor(0x06,0x00); 		 
					CamWriteCmosSensor(0x09,0x00); 		 
					CamWriteCmosSensor(0x0a,0x3f); 			 
									   
					CamWriteCmosSensor(0x9b,0x2f);//base			 
					CamWriteCmosSensor(0x9c,0x00); 		 
												   
					CamWriteCmosSensor(0xa2,0x49);//exp			 
					CamWriteCmosSensor(0xa3,0x01); 		 
					CamWriteCmosSensor(0xa4,0x2f); 		 
					CamWriteCmosSensor(0xa5,0x00); 		 
					CamWriteCmosSensor(0xa8,0x2f); 		 
					CamWriteCmosSensor(0xa9,0x00); 		 
					CamWriteCmosSensor(0xaa,0x01); 		 
					CamWriteCmosSensor(0xab,0x00);
	
		                   CamWriteCmosSensor(0xa6,SP0820_DAYLIGHT_MAXGAIN );
		                   CamWriteCmosSensor(0xac,SP0820_DAYLIGHT_MAXGAIN );
	
					SP0820_SERIAL_TRACE(" video 50Hz normal\r\n");
				}
				else if(banding == CAM_BANDING_60HZ)
				{
					//Video record daylight 24M 60hz fix 14FPS 
					CamWriteCmosSensor(0x03,0x00); 		 
					CamWriteCmosSensor(0x04,0x75); 
					CamWriteCmosSensor(0x05,0x00); 			
					CamWriteCmosSensor(0x06,0x00); 			
					CamWriteCmosSensor(0x09,0x00); 			
					CamWriteCmosSensor(0x0a,0x41); 				
													
					CamWriteCmosSensor(0x9b,0x27);//base			
					CamWriteCmosSensor(0x9c,0x00); 			
														
					CamWriteCmosSensor(0xa2,0x38);//exp			
					CamWriteCmosSensor(0xa3,0x01); 			
					CamWriteCmosSensor(0xa4,0x27); 			
					CamWriteCmosSensor(0xa5,0x00); 			
					CamWriteCmosSensor(0xa8,0x27); 			
					CamWriteCmosSensor(0xa9,0x00); 			
					CamWriteCmosSensor(0xaa,0x01); 			
					CamWriteCmosSensor(0xab,0x00); 
					
		                   CamWriteCmosSensor(0xa6,SP0820_DAYLIGHT_MAXGAIN );
		                   CamWriteCmosSensor(0xac,SP0820_DAYLIGHT_MAXGAIN );
					SP0820_SERIAL_TRACE(" video 60Hz normal\r\n");
				}
			}
		else
			{
				if(banding == CAM_BANDING_50HZ)
				{
				//caprure preview daylight 24M 50hz 14.07-8FPS
					CamWriteCmosSensor(0x03,0x00); 		 
					CamWriteCmosSensor(0x04,0x8d); 
					CamWriteCmosSensor(0x05,0x00); 		 
					CamWriteCmosSensor(0x06,0x00); 		 
					CamWriteCmosSensor(0x09,0x00); 		 
					CamWriteCmosSensor(0x0a,0x3f); 			 
									   
					CamWriteCmosSensor(0x9b,0x2f);//base			 
					CamWriteCmosSensor(0x9c,0x00); 		 
												   
					CamWriteCmosSensor(0xa2,0x34);//exp			 
					CamWriteCmosSensor(0xa3,0x02); 		 
					CamWriteCmosSensor(0xa4,0x2f); 		 
					CamWriteCmosSensor(0xa5,0x00); 		 
					CamWriteCmosSensor(0xa8,0x2f); 		 
					CamWriteCmosSensor(0xa9,0x00); 		 
					CamWriteCmosSensor(0xaa,0x01); 		 
					CamWriteCmosSensor(0xab,0x00);
	
		                   CamWriteCmosSensor(0xa6,SP0820_DAYLIGHT_MAXGAIN );
		                   CamWriteCmosSensor(0xac,SP0820_DAYLIGHT_MAXGAIN );
					SP0820_SERIAL_TRACE(" priview 50Hz normal\r\n");
				 
				}
				else if(banding == CAM_BANDING_60HZ)
				{
					//caprure preview daylight 24M 60hz 14.01-8FPS
					CamWriteCmosSensor(0x03,0x00); 		 
					CamWriteCmosSensor(0x04,0x75); 
					CamWriteCmosSensor(0x05,0x00); 			
					CamWriteCmosSensor(0x06,0x00); 			
					CamWriteCmosSensor(0x09,0x00); 			
					CamWriteCmosSensor(0x0a,0x41); 				
													
					CamWriteCmosSensor(0x9b,0x27);//base			
					CamWriteCmosSensor(0x9c,0x00); 			
														
					CamWriteCmosSensor(0xa2,0x49);//exp			
					CamWriteCmosSensor(0xa3,0x02); 			
					CamWriteCmosSensor(0xa4,0x27); 			
					CamWriteCmosSensor(0xa5,0x00); 			
					CamWriteCmosSensor(0xa8,0x27); 			
					CamWriteCmosSensor(0xa9,0x00); 			
					CamWriteCmosSensor(0xaa,0x01); 			
					CamWriteCmosSensor(0xab,0x00); 
	
		                   CamWriteCmosSensor(0xa6,SP0820_DAYLIGHT_MAXGAIN );
		                   CamWriteCmosSensor(0xac,SP0820_DAYLIGHT_MAXGAIN );
					SP0820_SERIAL_TRACE(" priview 60Hz normal\r\n");
				
				 
				}
			}	
		}
	else //night mode
	{
			if(KAL_TRUE == videomode)		
			{
				if(banding == CAM_BANDING_50HZ)
				{
					//Video record night 24M 50hz  fix 9FPS 
					CamWriteCmosSensor(0x03,0x00); 		 
					CamWriteCmosSensor(0x04,0x5d); 
					CamWriteCmosSensor(0x05,0x00); 		 
					CamWriteCmosSensor(0x06,0x00); 		 
					CamWriteCmosSensor(0x09,0x00); 		 
					CamWriteCmosSensor(0x0a,0xe4); 			 
									   
					CamWriteCmosSensor(0x9b,0x1f);//base			 
					CamWriteCmosSensor(0x9c,0x00); 		 
												   
					CamWriteCmosSensor(0xa2,0x55);//exp			 
					CamWriteCmosSensor(0xa3,0x01); 		 
					CamWriteCmosSensor(0xa4,0x1f); 		 
					CamWriteCmosSensor(0xa5,0x00); 		 
					CamWriteCmosSensor(0xa8,0x1f); 		 
					CamWriteCmosSensor(0xa9,0x00); 		 
					CamWriteCmosSensor(0xaa,0x01); 		 
					CamWriteCmosSensor(0xab,0x00);
	
		                   CamWriteCmosSensor(0xa6,SP0820_NIGHT_MAXGAIN );
		                   CamWriteCmosSensor(0xac,SP0820_NIGHT_MAXGAIN );
					SP0820_SERIAL_TRACE(" video 50Hz night\r\n"); 			
				}
				else if(banding == CAM_BANDING_60HZ)
				{
					//Video record night 24M 60hz fix 9FPS
					CamWriteCmosSensor(0x03,0x00); 		 
					CamWriteCmosSensor(0x04,0x4e); 
					CamWriteCmosSensor(0x05,0x00); 		 
					CamWriteCmosSensor(0x06,0x00); 		 
					CamWriteCmosSensor(0x09,0x00); 		 
					CamWriteCmosSensor(0x0a,0xe1); 			 
									   
					CamWriteCmosSensor(0x9b,0x1a);//base			 
					CamWriteCmosSensor(0x9c,0x00); 		 
												   
					CamWriteCmosSensor(0xa2,0x52);//exp			 
					CamWriteCmosSensor(0xa3,0x01); 		 
					CamWriteCmosSensor(0xa4,0x1a); 		 
					CamWriteCmosSensor(0xa5,0x00); 		 
					CamWriteCmosSensor(0xa8,0x1a); 		 
					CamWriteCmosSensor(0xa9,0x00); 		 
					CamWriteCmosSensor(0xaa,0x01); 		 
					CamWriteCmosSensor(0xab,0x00);
	
		                   CamWriteCmosSensor(0xa6,SP0820_NIGHT_MAXGAIN );
		                   CamWriteCmosSensor(0xac,SP0820_NIGHT_MAXGAIN );
					SP0820_SERIAL_TRACE(" video 60Hz night\r\n"); 
				}
			}
		   else
			{
				SP0820_SERIAL_TRACE(" banding=%x\r\n",banding);
				if(banding == CAM_BANDING_50HZ)
				{
					//caprure preview night 24M 50hz 14.07-6FPS	
					CamWriteCmosSensor(0x03,0x00); 		 
					CamWriteCmosSensor(0x04,0x8d); 
					CamWriteCmosSensor(0x05,0x00); 		 
					CamWriteCmosSensor(0x06,0x00); 		 
					CamWriteCmosSensor(0x09,0x00); 		 
					CamWriteCmosSensor(0x0a,0x3f); 			 
									   
					CamWriteCmosSensor(0x9b,0x2f);//base			 
					CamWriteCmosSensor(0x9c,0x00); 		 
												   
					CamWriteCmosSensor(0xa2,0xf0);//exp			 
					CamWriteCmosSensor(0xa3,0x02); 		 
					CamWriteCmosSensor(0xa4,0x2f); 		 
					CamWriteCmosSensor(0xa5,0x00); 		 
					CamWriteCmosSensor(0xa8,0x2f); 		 
					CamWriteCmosSensor(0xa9,0x00); 		 
					CamWriteCmosSensor(0xaa,0x01); 		 
					CamWriteCmosSensor(0xab,0x00);
	
		                    CamWriteCmosSensor(0xa6,SP0820_NIGHT_MAXGAIN );
		                    CamWriteCmosSensor(0xac,SP0820_NIGHT_MAXGAIN );
					SP0820_SERIAL_TRACE(" priview 50Hz night\r\n");	
				}
				else if(banding == CAM_BANDING_60HZ)
				{
					//caprure preview night 24M 60hz 14.37-6FPS	
					CamWriteCmosSensor(0x03,0x00); 		 
					CamWriteCmosSensor(0x04,0x78); 
					CamWriteCmosSensor(0x05,0x00); 			
					CamWriteCmosSensor(0x06,0x00); 			
					CamWriteCmosSensor(0x09,0x00); 			
					CamWriteCmosSensor(0x0a,0x38); 				
													
					CamWriteCmosSensor(0x9b,0x28);//base			
					CamWriteCmosSensor(0x9c,0x00); 			
														
					CamWriteCmosSensor(0xa2,0x20);//exp			
					CamWriteCmosSensor(0xa3,0x03); 			
					CamWriteCmosSensor(0xa4,0x28); 			
					CamWriteCmosSensor(0xa5,0x00); 			
					CamWriteCmosSensor(0xa8,0x28); 			
					CamWriteCmosSensor(0xa9,0x00); 			
					CamWriteCmosSensor(0xaa,0x01); 			
					CamWriteCmosSensor(0xab,0x00); 
	
		                   CamWriteCmosSensor(0xa6,SP0820_NIGHT_MAXGAIN );
		                   CamWriteCmosSensor(0xac,SP0820_NIGHT_MAXGAIN );
					SP0820_SERIAL_TRACE(" priview 60Hz night\r\n");	
				}
			}
	}
	
	
	
}


/*************************************************************************
* FUNCTION
*   SP0820_SERIALEv
*
* DESCRIPTION
*   Exposure setting.
*
* PARAMETERS
*   In, Out
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
static MM_ERROR_CODE_ENUM SP0820_SERIALEv(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
  if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
  {
    const P_CAL_FEATURE_TYPE_ENUM_STRUCT Exposure = &Out->FeatureInfo.FeatureEnum;
    
    Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
    Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
    Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;

    Exposure->IsSupport = KAL_TRUE;
    Exposure->ItemCount = 10;
    Exposure->SupportItem[0] = CAM_EV_NEG_4_3;
    Exposure->SupportItem[1] = CAM_EV_NEG_3_3;
    Exposure->SupportItem[2] = CAM_EV_NEG_2_3;
    Exposure->SupportItem[3] = CAM_EV_NEG_1_3;
    Exposure->SupportItem[4] = CAM_EV_ZERO;
    Exposure->SupportItem[5] = CAM_EV_POS_1_3;
    Exposure->SupportItem[6] = CAM_EV_POS_2_3;
    Exposure->SupportItem[7] = CAM_EV_POS_3_3;
    Exposure->SupportItem[8] = CAM_EV_POS_4_3;
    Exposure->SupportItem[9] = CAM_EV_NIGHT_SHOT;
  }
  else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
  {  
    switch (In->FeatureSetValue)
    {
    case CAM_EV_NEG_4_3:
      		        CamWriteCmosSensor(0x61,0xc0);    
      		  
      break;
    case CAM_EV_NEG_3_3:
      		        CamWriteCmosSensor(0x61,0xd0);   
      break;
    case CAM_EV_NEG_2_3:
			CamWriteCmosSensor(0x61,0xe0);     
      break;
    case CAM_EV_NEG_1_3:
			CamWriteCmosSensor(0x61,0xf0);    
      break;
    case CAM_EV_ZERO:
                       CamWriteCmosSensor(0x61,0x00); 
      break;
    case CAM_EV_POS_1_3:
			CamWriteCmosSensor(0x61,0x10);   
      break;
    case CAM_EV_POS_2_3:
			CamWriteCmosSensor(0x61,0x20);     
      break;
    case CAM_EV_POS_3_3:
      		       CamWriteCmosSensor(0x61,0x30);    
      break;
    case CAM_EV_POS_4_3:
      		       CamWriteCmosSensor(0x61,0x40); 
      break;
    default:
      return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
    }
  }
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALContrast
*
* DESCRIPTION
*   Contrast setting.
*
* PARAMETERS
*   In, Out
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
static MM_ERROR_CODE_ENUM SP0820_SERIALContrast(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
  if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
  {
    const P_CAL_FEATURE_TYPE_ENUM_STRUCT Contrast = &Out->FeatureInfo.FeatureEnum;
    
    Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
    Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
    Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;

    Contrast->IsSupport = KAL_TRUE;
    Contrast->ItemCount = 3;
    Contrast->SupportItem[0] = CAM_CONTRAST_HIGH;
    Contrast->SupportItem[1] = CAM_CONTRAST_MEDIUM;
    Contrast->SupportItem[2] = CAM_CONTRAST_LOW;
  }
  else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
  {
    switch (In->FeatureSetValue)
    {
    case CAM_CONTRAST_HIGH:
		CamWriteCmosSensor(0x58, 0x90);
		CamWriteCmosSensor(0x59, 0xb0);
      break;
    case CAM_CONTRAST_MEDIUM:
		CamWriteCmosSensor(0x58, 0x80);
		CamWriteCmosSensor(0x59, 0x94);
      break;
    case CAM_CONTRAST_LOW:
		CamWriteCmosSensor(0x58, 0x70);
		CamWriteCmosSensor(0x59, 0x80);
      break;
    default:
      return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
    }
  }
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSaturation
*
* DESCRIPTION
*   Saturation setting.
*
* PARAMETERS
*   In, Out
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
static MM_ERROR_CODE_ENUM SP0820_SERIALSaturation(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
  if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
  {
    const P_CAL_FEATURE_TYPE_ENUM_STRUCT Saturation = &Out->FeatureInfo.FeatureEnum;
    
    Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
    Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
    Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;
    // TODO: add supported saturation enum here
    Saturation->IsSupport = KAL_FALSE;
    //Saturation->ItemCount = 3;
    //Saturation->SupportItem[0] = CAM_SATURATION_HIGH;
    //Saturation->SupportItem[1] = CAM_SATURATION_MEDIUM;
    //Saturation->SupportItem[2] = CAM_SATURATION_LOW;
  }
  else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
  {
    // TODO: adjust saturation here
    //switch (In->FeatureSetValue)
    //{
    //case CAM_SATURATION_HIGH:
    //  break;
    //case CAM_SATURATION_MEDIUM:
    //  break;
    //case CAM_SATURATION_LOW:
    //  break;
    //default:
    //  return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
    //}
  }
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSharpness
*
* DESCRIPTION
*   Sharpness setting.
*
* PARAMETERS
*   In, Out
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
static MM_ERROR_CODE_ENUM SP0820_SERIALSharpness(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
  if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
  {
    const P_CAL_FEATURE_TYPE_ENUM_STRUCT Sharpness = &Out->FeatureInfo.FeatureEnum;
    
    Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
    Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
    Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;
    // TODO: add supported sharpness enum here
    Sharpness->IsSupport = KAL_FALSE;
    //Sharpness->ItemCount = 3;
    //Sharpness->SupportItem[0] = CAM_SHARPNESS_HIGH;
    //Sharpness->SupportItem[1] = CAM_SHARPNESS_MEDIUM;
    //Sharpness->SupportItem[2] = CAM_SHARPNESS_LOW;
  }
  else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
  {
    // TODO: adjust sharpness here
    //switch (In->FeatureSetValue)
    //{
    //case CAM_SHARPNESS_HIGH:
    //  break;
    //case CAM_SHARPNESS_MEDIUM:
    //  break;
    //case CAM_SHARPNESS_LOW:
    //  break;
    //default:
    //  return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
    //}
  }
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALWb
*
* DESCRIPTION
*   Wb setting.
*
* PARAMETERS
*   In, Out
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
static MM_ERROR_CODE_ENUM SP0820_SERIALWb(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
  if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
  {
    const P_CAL_FEATURE_TYPE_ENUM_STRUCT Wb = &Out->FeatureInfo.FeatureEnum;
    
    Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
    Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
    Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;

    Wb->IsSupport = KAL_TRUE;
    Wb->ItemCount = 6;
    Wb->SupportItem[0] = CAM_WB_AUTO;
    Wb->SupportItem[1] = CAM_WB_CLOUD;
    Wb->SupportItem[2] = CAM_WB_DAYLIGHT;
    Wb->SupportItem[3] = CAM_WB_INCANDESCENCE;
    Wb->SupportItem[4] = CAM_WB_FLUORESCENT;
    Wb->SupportItem[5] = CAM_WB_TUNGSTEN;
  }
  else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
  {
    switch (In->FeatureSetValue)
    {
    case CAM_WB_AUTO:
		CamWriteCmosSensor(0xc5, 0xcf);
		CamWriteCmosSensor(0xc6, 0xa1);
		CamWriteCmosSensor(0xe7, 0x03);
		CamWriteCmosSensor(0xe7, 0x00);
		CamWriteCmosSensor(0x32, 0x07);
		SP0820_SERIALAwbEnable(KAL_TRUE);
      break;
    case CAM_WB_CLOUD:
		SP0820_SERIALAwbEnable(KAL_FALSE);
		CamWriteCmosSensor(0x32, 0x05);
		CamWriteCmosSensor(0xc5, 0xe5);	
		CamWriteCmosSensor(0xc6, 0x80);	
      break;
    case CAM_WB_DAYLIGHT:
		SP0820_SERIALAwbEnable(KAL_FALSE);
		CamWriteCmosSensor(0x32, 0x05);
		CamWriteCmosSensor(0xc5, 0xc5);	
		CamWriteCmosSensor(0xc6, 0x8a);	
      break;
    case CAM_WB_INCANDESCENCE:
		SP0820_SERIALAwbEnable(KAL_FALSE);
		CamWriteCmosSensor(0x32, 0x05);
		CamWriteCmosSensor(0xc5, 0xa8);	
		CamWriteCmosSensor(0xc6, 0xae);	
      break;
    case CAM_WB_FLUORESCENT:
		SP0820_SERIALAwbEnable(KAL_FALSE);
		CamWriteCmosSensor(0x32, 0x05);
		CamWriteCmosSensor(0xc5, 0x9c);
		CamWriteCmosSensor(0xc6, 0xaf);	
      break;
    case CAM_WB_TUNGSTEN:
		SP0820_SERIALAwbEnable(KAL_FALSE);
		CamWriteCmosSensor(0x32, 0x05);
		CamWriteCmosSensor(0xc5, 0xa8);
		CamWriteCmosSensor(0xc6, 0xb7);	
      break;
    default:
      return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
    }
  }
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALEffect
*
* DESCRIPTION
*   Effect setting.
*
* PARAMETERS
*   In, Out
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
static MM_ERROR_CODE_ENUM SP0820_SERIALEffect(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
  if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
  {
    const P_CAL_FEATURE_TYPE_ENUM_STRUCT Effect = &Out->FeatureInfo.FeatureEnum;
    
    Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
    Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
    Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;

    Effect->IsSupport = KAL_TRUE;
    Effect->ItemCount = 6;
    Effect->SupportItem[0] = CAM_EFFECT_ENC_NORMAL;
    Effect->SupportItem[1] = CAM_EFFECT_ENC_GRAYSCALE;
    Effect->SupportItem[2] = CAM_EFFECT_ENC_SEPIA;
    Effect->SupportItem[3] = CAM_EFFECT_ENC_SEPIAGREEN;
    Effect->SupportItem[4] = CAM_EFFECT_ENC_SEPIABLUE;
    Effect->SupportItem[5] = CAM_EFFECT_ENC_COLORINV;
  }
  else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
  {
    switch (In->FeatureSetValue)
    {
    case CAM_EFFECT_ENC_NORMAL:
			CamWriteCmosSensor(0x62, 0x00);
			CamWriteCmosSensor(0x36, 0x80);
			CamWriteCmosSensor(0x37, 0x80);
      break;
    case CAM_EFFECT_ENC_GRAYSCALE:
			CamWriteCmosSensor(0x62, 0x04);
			CamWriteCmosSensor(0x36, 0x80);
			CamWriteCmosSensor(0x37, 0x80);
      break;
    case CAM_EFFECT_ENC_SEPIA:
			CamWriteCmosSensor(0x62, 0x08);
			CamWriteCmosSensor(0x36, 0xb0);
			CamWriteCmosSensor(0x37, 0x00);
      break;
    case CAM_EFFECT_ENC_SEPIAGREEN:
			CamWriteCmosSensor(0x62, 0x08);
			CamWriteCmosSensor(0x36, 0x50);
			CamWriteCmosSensor(0x37, 0x50);     
      break;
    case CAM_EFFECT_ENC_SEPIABLUE:
			CamWriteCmosSensor(0x62, 0x08);
			CamWriteCmosSensor(0x36, 0x20);
			CamWriteCmosSensor(0x37, 0xf0);
      break;
    case CAM_EFFECT_ENC_COLORINV:
			CamWriteCmosSensor(0x62, 0x01);
			CamWriteCmosSensor(0x36, 0x80);
			CamWriteCmosSensor(0x37, 0x80);
      break;
    default:
      return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
    }
  }
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALStillCaptureSize
*
* DESCRIPTION
*   This function get capture size
*
* PARAMETERS
*   In, Out
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
MM_ERROR_CODE_ENUM SP0820_SERIALStillCaptureSize(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
  const P_CAL_FEATURE_TYPE_ENUM_STRUCT CapSize = &Out->FeatureInfo.FeatureEnum;
  
  if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
  {
    Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
    Out->FeatureOperationSupport = CAL_FEATURE_QUERY_OPERATION;
    Out->FeatureModeSupport = CAL_FEATURE_CAMERA_SUPPORT;
    CapSize->IsSupport = KAL_TRUE;
    CapSize->ItemCount = 3;
    CapSize->SupportItem[0] = CAM_IMAGE_SIZE_WALLPAPER;
	CapSize->SupportItem[1] = CAM_IMAGE_SIZE_QVGA;
    CapSize->SupportItem[2] = CAM_IMAGE_SIZE_VGA;
  }
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALBanding
*
* DESCRIPTION
*   This function get banding setting
*
* PARAMETERS
*   In, Out
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
static MM_ERROR_CODE_ENUM SP0820_SERIALBanding(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
#if 0
  const P_CAL_FEATURE_TYPE_ENUM_STRUCT Banding = &Out->FeatureInfo.FeatureEnum;
  
  if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
  {
    Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
    Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
    Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;
    Banding->IsSupport = KAL_TRUE;
    Banding->ItemCount = 2;
    Banding->SupportItem[0] = CAM_BANDING_50HZ;
    Banding->SupportItem[1] = CAM_BANDING_60HZ;
  }
  else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
  {
        SP0820_SERIALSensor.BandingFreq = (CAM_BANDING_50HZ == In->FeatureSetValue ? SP0820_SERIAL_50HZ : SP0820_SERIAL_60HZ);
        // TODO: set manual banding here
        {
            if (CAM_BANDING_50HZ == In->FeatureSetValue)
            {
			// based on 24M MCLK 
		
			SP0820_SERIAL_CAM_BANDING_50HZ = KAL_TRUE;
            }
            else
            {			
			SP0820_SERIAL_CAM_BANDING_50HZ = KAL_FALSE;
            }
        }
    }
    return MM_ERROR_NONE;
#endif	
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSceneMode
*
* DESCRIPTION
*   This function get/set scene mode of sensor.
*
* PARAMETERS
*   IsCam: camera scene mode or not
*   In, Out
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static MM_ERROR_CODE_ENUM SP0820_SERIALSceneMode(kal_bool IsCam, P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
  if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
  {
    const P_CAL_FEATURE_TYPE_ENUM_STRUCT SceneMode = &Out->FeatureInfo.FeatureEnum;
    
    Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
    Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
    Out->FeatureModeSupport = CAL_FEATURE_CAMERA_SUPPORT;
    SceneMode->IsSupport = KAL_TRUE;
    SceneMode->ItemCount = 2;
    if (IsCam)
    {
      SceneMode->SupportItem[0] = CAM_AUTO_DSC;
      SceneMode->SupportItem[1] = CAM_NIGHTSCENE;
    }
    else
    {
      SceneMode->SupportItem[0] = CAM_VIDEO_AUTO;
      SceneMode->SupportItem[1] = CAM_VIDEO_NIGHT;
    }
  }
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALInitialSetting
*
* DESCRIPTION
*   This function initialize the registers of CMOS sensor
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALInitialSetting(void)
{ 
		CamWriteCmosSensor(0x19,0x00);
		CamWriteCmosSensor(0x2c,0x0f);
		CamWriteCmosSensor(0x2e,0x3c);
		CamWriteCmosSensor(0x30,0x03);
		CamWriteCmosSensor(0x31,0x00);//pclk inv

		CamWriteCmosSensor(0x0e,0x06); 
		CamWriteCmosSensor(0x0f,0x06); 
		CamWriteCmosSensor(0x11,0x05); 
		CamWriteCmosSensor(0x12,0x07);
		CamWriteCmosSensor(0x14,0x04);  
		CamWriteCmosSensor(0x15,0x00);  
		CamWriteCmosSensor(0x08,0x40);//[6]blk_en sun pot
		CamWriteCmosSensor(0x17,0x20);//同0829的0x1a
		CamWriteCmosSensor(0x27,0x88);
		CamWriteCmosSensor(0x28,0x03); 

		CamWriteCmosSensor(0x32,0x00);//awb 
		CamWriteCmosSensor(0xc5,0xcf);//r 
		CamWriteCmosSensor(0xc6,0xa1);//b
		CamWriteCmosSensor(0xe7,0x03);
		CamWriteCmosSensor(0xe7,0x00);

		CamWriteCmosSensor(0x5b,0x13);

		//black level en
		CamWriteCmosSensor(0x29,0x1f); //[0] 0ffset shift全删掉了  

		//caprure preview daylight 24M 50hz 14.07-8FPS
		CamWriteCmosSensor(0x03,0x00); 		 
		CamWriteCmosSensor(0x04,0x8d); 
		CamWriteCmosSensor(0x05,0x00); 		 
		CamWriteCmosSensor(0x06,0x00); 		 
		CamWriteCmosSensor(0x09,0x00); 		 
		CamWriteCmosSensor(0x0a,0x3f); 			 

		CamWriteCmosSensor(0x9b,0x2f);//base			 
		CamWriteCmosSensor(0x9c,0x00); 		 

		CamWriteCmosSensor(0xa2,0x34);//exp			 
		CamWriteCmosSensor(0xa3,0x02); 		 
		CamWriteCmosSensor(0xa4,0x2f); 		 
		CamWriteCmosSensor(0xa5,0x00); 		 
		CamWriteCmosSensor(0xa8,0x2f); 		 
		CamWriteCmosSensor(0xa9,0x00); 		 
		CamWriteCmosSensor(0xaa,0x01); 		 
		CamWriteCmosSensor(0xab,0x00);
		//pregain
		CamWriteCmosSensor(0x4c,0x80);//gr
		CamWriteCmosSensor(0x4d,0x80);//gb

		//gain
		CamWriteCmosSensor(0xa6,SP0820_DAYLIGHT_MAXGAIN);
		CamWriteCmosSensor(0xa7,0x2a);
		CamWriteCmosSensor(0xac,SP0820_DAYLIGHT_MAXGAIN);
		CamWriteCmosSensor(0xad,0x2a);
		CamWriteCmosSensor(0x8a,0x50);
		CamWriteCmosSensor(0x8b,0x3e);
		CamWriteCmosSensor(0x8c,0x36);
		CamWriteCmosSensor(0x8d,0x30);
		CamWriteCmosSensor(0x8e,0x30);
		CamWriteCmosSensor(0x8f,0x2e);
		CamWriteCmosSensor(0x90,0x2e);
		CamWriteCmosSensor(0x91,0x2c);
		CamWriteCmosSensor(0x92,0x2c);
		CamWriteCmosSensor(0x93,0x2c);
		CamWriteCmosSensor(0x94,0x2a);
		CamWriteCmosSensor(0x95,0x2a);
		CamWriteCmosSensor(0x96,0x2a);

		//awb_buf_gain

		CamWriteCmosSensor(0xc8,0x10);//y_bot_th
		CamWriteCmosSensor(0x5f,0xcb);//rg_base1
		CamWriteCmosSensor(0x60,0xa0);//bg_base1
		CamWriteCmosSensor(0x5c,0x09);//rg_dif_th1
		CamWriteCmosSensor(0x5d,0x09);//bg_dif_th1
		CamWriteCmosSensor(0x5e,0x0a);//rgb_limit1
		CamWriteCmosSensor(0x53,0x9e);//rg_base2
		CamWriteCmosSensor(0x54,0xb4);//bg_base2
		CamWriteCmosSensor(0x50,0x09);//rg_dif_th2
		CamWriteCmosSensor(0x51,0x09);//bg_dif_th2
		CamWriteCmosSensor(0x52,0x0a);//rgb_limit2  

		//dns 
		CamWriteCmosSensor(0x1b,0x03);// ;dns_flat_thr1
		CamWriteCmosSensor(0x1c,0x05);// ;dns_flat_thr2
		CamWriteCmosSensor(0x1d,0x08);// ;dns_flat_thr3
		CamWriteCmosSensor(0x1e,0x0d); //dns_flat_thr4
		//  sharp     
		CamWriteCmosSensor(0x1f,0x04); //sharp_flat_thr1
		CamWriteCmosSensor(0x20,0x07); //sharp_flat_thr2
		CamWriteCmosSensor(0x21,0x0a); //sharp_flat_thr3
		CamWriteCmosSensor(0x22,0x12); //sharp_flat_thr4

		CamWriteCmosSensor(0x56,0x24); //raw_sharp_pos
		CamWriteCmosSensor(0x57,0x20);//raw_sharp_neg

		//cm
		CamWriteCmosSensor(0x79,0xb1);//ca;80
		CamWriteCmosSensor(0x7a,0xcf);//b0;0 
		CamWriteCmosSensor(0x7b,0x00);//6 ;0 
		CamWriteCmosSensor(0x7c,0x19);//d7;f3
		CamWriteCmosSensor(0x7d,0x70);//c7;8e
		CamWriteCmosSensor(0x7e,0xf7);//e2;0 
		CamWriteCmosSensor(0x7f,0x18);//ff;0 
		CamWriteCmosSensor(0x80,0xc1);//96;e6
		CamWriteCmosSensor(0x81,0xa6);//eb;9a
		CamWriteCmosSensor(0x82,0x0c);//c ;0 
		CamWriteCmosSensor(0x83,0x30);//33;3 
		CamWriteCmosSensor(0x84,0x0c);//f ;c  

		//sat
		CamWriteCmosSensor(0x4e,0x60);//v
		CamWriteCmosSensor(0x4f,0x60);//U
		// heq       
		CamWriteCmosSensor(0x58,0x80);//ku
		CamWriteCmosSensor(0x59,0xa0);//kl
		CamWriteCmosSensor(0x5a,0x80);//hep mean

		//  auto lum lowlight  
		CamWriteCmosSensor(0x86,0x28);
		CamWriteCmosSensor(0x87,0x1f);
		CamWriteCmosSensor(0x88,0x30);
		CamWriteCmosSensor(0x89,0x45);

		//target
		CamWriteCmosSensor(0x98,0x88);//94
		CamWriteCmosSensor(0x9e,0x84);//90
		CamWriteCmosSensor(0x9f,0x7c);//88
		CamWriteCmosSensor(0x97,0x78);//84 

		CamWriteCmosSensor(0x9a,0x84);//90
		CamWriteCmosSensor(0xa0,0x80);//8c
		CamWriteCmosSensor(0xa1,0x78);//84
		CamWriteCmosSensor(0x99,0x74);//80 

		//gamma
		CamWriteCmosSensor(0x63,0x00);//00//00
		CamWriteCmosSensor(0x64,0x0a);//0f//02
		CamWriteCmosSensor(0x65,0x13);//21//04
		CamWriteCmosSensor(0x66,0x1c);//2     c//07
		CamWriteCmosSensor(0x67,0x25);//37//0d
		CamWriteCmosSensor(0x68,0x37);//46//18
		CamWriteCmosSensor(0x69,0x46);//53//2a
		CamWriteCmosSensor(0x6a,0x52);//5e//3e
		CamWriteCmosSensor(0x6b,0x5e);//6a//52
		CamWriteCmosSensor(0x6c,0x75);//7d//6e
		CamWriteCmosSensor(0x6d,0x88);//8d//85
		CamWriteCmosSensor(0x6e,0x9a);//9e//96
		CamWriteCmosSensor(0x6f,0xa9);//ac//a6 
		CamWriteCmosSensor(0x70,0xb5);//ba//b3
		CamWriteCmosSensor(0x71,0xc0);//c6//c0
		CamWriteCmosSensor(0x72,0xca);//d1//cb 
		CamWriteCmosSensor(0x73,0xd4);//da//d5 
		CamWriteCmosSensor(0x74,0xdd);//e4//df 
		CamWriteCmosSensor(0x75,0xe6);//eb//e9 
		CamWriteCmosSensor(0x76,0xef);//f2//f2
		CamWriteCmosSensor(0x77,0xf7);//f9//fa 
		CamWriteCmosSensor(0x78,0xff);//ff//ff

		CamWriteCmosSensor(0x9d,0x09);

		CamWriteCmosSensor(0x34,0x0f);
		CamWriteCmosSensor(0x32,0x07);
		SP0820_SERIAL_TRACE("SP0820_SERIALInitialSetting\r\n");

}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALPvSetting
*
* DESCRIPTION
*   This function apply the preview mode setting, normal the preview size is 1/4 of full size.
*   Ex. 2M (1600 x 1200)
*   Preview: 800 x 600 (use sub-sample or binning to acheive it)
*   Full Size: 1600 x 1200 (output every effective pixels.)
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALPvSetting(void)
{
    // TODO: add preview setting here
    
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALCapSetting
*
* DESCRIPTION
*   This function config capture settting to sensor
*
* PARAMETERS
*   None
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALCapSetting(void)
{
	SP0820_SERIALSensor.Pclk = 24000000;		/* It is the PCLK for sensor internal parallel interface */	
	SP0820_SERIALSensor.PvMode = KAL_FALSE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALGetSensorInfo
*
* DESCRIPTION
*   This function set sensor infomation
*
* PARAMETERS
*   Info
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALGetSensorInfo(P_IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT Info)
{
	Info->SensorId = SP0820_SERIAL_SENSOR_ID;
	Info->SensorIf = IMAGE_SENSOR_IF_SERIAL;

	Info->PreviewMclkFreq = SP0820_SERIAL_MCLK;
	Info->CaptureMclkFreq = SP0820_SERIAL_MCLK;
	Info->VideoMclkFreq = SP0820_SERIAL_MCLK;

	Info->PreviewWidth = SP0820_SERIAL_IMAGE_SENSOR_PV_WIDTH;
	Info->PreviewHeight = SP0820_SERIAL_IMAGE_SENSOR_PV_HEIGHT;
	Info->FullWidth = SP0820_SERIAL_IMAGE_SENSOR_FULL_WIDTH;
	Info->FullHeight = SP0820_SERIAL_IMAGE_SENSOR_FULL_HEIGHT;

	/* data format */
	Info->PreviewNormalDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY;
	Info->PreviewHMirrorDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY;
	Info->PreviewVMirrorDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY;
	Info->PreviewHVMirrorDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY;
	Info->CaptureDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_UYVY;

	/* isp pclk invert switch */
	Info->PixelClkInv 			= KAL_FALSE;
	Info->PixelClkPolarity 		= POLARITY_LOW;

	/* HSYNC/VSYNC polarity */
	Info->HsyncPolarity = POLARITY_LOW; /* Actually high active when set POLARITY_LOW */
	Info->VsyncPolarity = POLARITY_LOW;

	Info->SensorAfSupport = KAL_FALSE;
	Info->SensorFlashSupport = KAL_FALSE;

	/* If IsSensorDriverCtrlMCLK is TRUE, then the MCLK turn on/off is controlled by sensor driver.*/
	Info->IsSensorDriverCtrlMclk = KAL_TRUE;
}

/*************************************************************************
* FUNCTION
*    SP0820SerialSensorFeatureCtrl
*
* DESCRIPTION
*    This function set sensor feature mode
*
* PARAMETERS
*    id: scenario id
*
* RETURNS
*    error code
*
* LOCAL AFFECTED
*
*************************************************************************/
MM_ERROR_CODE_ENUM SP0820_SERIALGetSerialSensorInfo(P_SERIAL_SENSOR_INFO_IN_STRUCT In,P_SERIAL_SENSOR_INFO_OUT_STRUCT Out)
{
	Out->para_1 = KAL_TRUE; 								/* SCK clock inverse */
	Out->para_2 = 4;										/* Cycle */
	
	if (In->ScenarioId == CAL_SCENARIO_CAMERA_STILL_CAPTURE)
	{
		Out->para_3 = SP0820_SERIAL_IMAGE_SENSOR_PV_WIDTH; 	/* Sensor capture data valid width */
		Out->para_4 = SP0820_SERIAL_IMAGE_SENSOR_PV_HEIGHT;	/* Sensor capture data valid height */
	}
	else
	{
		Out->para_3 = SP0820_SERIAL_IMAGE_SENSOR_FULL_WIDTH; 	/* Sensor preview data valid width */
		Out->para_4 = SP0820_SERIAL_IMAGE_SENSOR_FULL_HEIGHT; 	/* Sensor preview data valid height */
	}

	Out->para_5  = 1;//lane number
	Out->para_6  = KAL_FALSE;//DDR supprt
	Out->para_7  = KAL_FALSE;//CRC support
	
	return MM_ERROR_NONE; 
}




/*************************************************************************
* FUNCTION
*   SP0820_SERIALPowerOn
*
* DESCRIPTION
*   This function apply the power on sequence and read the sensor ID to check if the I2C
*  communication can work or not.
*
* PARAMETERS
*   None
*
* RETURNS
*   Sensor id
*
* LOCAL AFFECTED
*
*************************************************************************/
static kal_uint32 SP0820_SERIALPowerOn(void)
{
    kal_uint32 SensorId = 0;
  
    CisModulePowerOn(SP0820_SERIALSensor.SensorIdx, KAL_TRUE);
    CameraSccbOpen(CAMERA_SCCB_SENSOR, SP0820_SERIAL_WRITE_ID_0,SP0820_SERIAL_I2C_ADDR_BITS, SP0820_SERIAL_I2C_DATA_BITS, SP0820_SERIAL_HW_I2C_SPEED);
    SP0820_SERIALSensor.sccb_opened = KAL_TRUE;
    vTaskDelay(3);
  
    /* Enable the MCLK */
    ENABLE_CAMERA_CLOCK_OUTPUT_TO_CMOS;
    vTaskDelay(2);
    
#if defined(DUAL_CAMERA_SUPPORT)
    SP0820_SERIALSensor.SensorIdx=camera_front_or_back;
#endif

    CamPdnPinCtrl(SP0820_SERIALSensor.SensorIdx, 0);   //need to check power on sequency //
    vTaskDelay(10);

    SensorId = CamReadCmosSensor(0x02);
    SP0820_SERIAL_TRACE("SP0820_SERIALPowerOn SP0820SensorId = %x\r\n",SensorId);

    return SensorId;
}
/*************************************************************************
* FUNCTION
*   SP0820_SERIALPreview
*
* DESCRIPTION
*   This function is the most important functions of (Init, Preview & Capture). 
*   1. Apply preview setting, make sensor output preview resolution, e.g. 800x600 for 2M sensor.
*   2. Record video resolution, and set mode based on the isp operation mode.
*   3. Configure the grab window.
*
* PARAMETERS
*   Id, In, Out
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALPreview(CAL_SCENARIO_ENUM Id, P_IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT In, P_IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT Out)
{

	if(0== In->BandingFreq)
	{
		SP0820_SERIAL_CAM_BANDING_50HZ =CAM_BANDING_50HZ;
	}
	else if(1== In->BandingFreq)
	{
		SP0820_SERIAL_CAM_BANDING_50HZ = CAM_BANDING_60HZ;
	}
	SP0820_SERIAL_CAM_nightmode = In->NightMode;
	SP0820_SERIALPvSetting();

	switch (Id)
	{
	case CAL_SCENARIO_VIDEO:
		video_mode = KAL_TRUE;
		Out->WaitStableFrameNum = 3;
		break;
	default:

		video_mode = KAL_FALSE;
		Out->WaitStableFrameNum = 3;
		break;
	}
  
	//SP0820_SERIALSetMirror(In->ImageMirror);

	SP0820_SERIALAeEnable(KAL_TRUE);
	SP0820_SERIALAwbEnable(KAL_TRUE);

	SP0820_SERIALNightMode(video_mode,SP0820_SERIAL_CAM_BANDING_50HZ,SP0820_SERIAL_CAM_nightmode);

	Out->GrabStartX = SP0820_SERIAL_PV_GRAB_START_X;
	Out->GrabStartY = SP0820_SERIAL_PV_GRAB_START_Y;
	Out->ExposureWindowWidth = SP0820_SERIAL_PV_GRAB_WIDTH;
	Out->ExposureWindowHeight = SP0820_SERIAL_PV_GRAB_HEIGHT;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALCapture
*
* DESCRIPTION
*   This function is the most important functions of (Init, Preview & Capture). 
*   1. Apply capture setting when capture size greatter then preview size, make sensor output 
*     full size, e.g. 1600x1200 for 2M sensor.
*   2. Increase the dummy pixels or derease pixel clock based on the digital zoom factor and
*    interpolation times.
*   3. Re-Calculate the shutter and sensor gain, to make the exposure value is same with preview
*   4. Configure the grab window based on sensor output.
*
* PARAMETERS
*   Id, In, Out
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALCapture(CAL_SCENARIO_ENUM Id, P_IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT In, P_IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT Out)
{
	SP0820_SERIALCapSetting();
	
	Out->WaitStableFrameNum = SP0820_SERIAL_CAPTURE_DELAY_FRAME;

	Out->GrabStartX = SP0820_SERIAL_FULL_GRAB_START_X;
	Out->GrabStartY = SP0820_SERIAL_FULL_GRAB_START_Y;
	Out->ExposureWindowWidth = SP0820_SERIAL_FULL_GRAB_WIDTH;
	Out->ExposureWindowHeight = SP0820_SERIAL_FULL_GRAB_HEIGHT;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALDetectSensorId
*
* DESCRIPTION
*   This function detect sensor id
*
* PARAMETERS
*   None
*
* RETURNS
*   Sensor id
*
* LOCAL AFFECTED
*
*************************************************************************/
static kal_uint32 SP0820_SERIALDetectSensorId(void)
{
    MM_ERROR_CODE_ENUM SP0820_SERIALSensorClose(void);
    kal_uint32 SensorId;
  	IMAGE_SENSOR_INDEX_ENUM AnotherSensorIdx;
	kal_int8 i, j;

	if (IMAGE_SENSOR_MAIN == SP0820_SERIALSensor.SensorIdx)
	{
		AnotherSensorIdx = IMAGE_SENSOR_SUB;
	}
	else
	{
		AnotherSensorIdx = IMAGE_SENSOR_MAIN;
	}
for (i = 1; i >= 0; i--)
{
	for (j = 1; j >= 0; j--)
	{
	CamRstPinCtrl(AnotherSensorIdx, i);
	CamPdnPinCtrl(AnotherSensorIdx, j);
      SensorId = SP0820_SERIALPowerOn();
      SP0820_SERIALSensorClose();
      if (SP0820_SERIAL_SENSOR_ID == SensorId)
      {
        return SP0820_SERIAL_SENSOR_ID;
      }
    }
  }
  return MM_ERROR_SENSOR_READ_ID_FAIL;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALInitOperationPara
*
* DESCRIPTION
*   This function set sensor operation para
*
* PARAMETERS
*   Para
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void SP0820_SERIALInitOperationPara(P_IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT Para)
{
  Para->CaptureDelayFrame = 0; /* use WaitStableFrameNum instead of this */
  Para->PreviewDelayFrame = SP0820_SERIAL_PREVIEW_DELAY_FRAME;
  Para->PreviewDisplayWaitFrame = SP0820_SERIAL_FIRST_PREVIEW_DELAY_FRAME;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSensorOpen
*
* DESCRIPTION
*   This function read sensor id and init sensor
*
* PARAMETERS
*   None
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/

 MM_ERROR_CODE_ENUM SP0820_SERIALSensorOpen(void)
{
  if (SP0820_SERIALPowerOn() != SP0820_SERIAL_SENSOR_ID)
  {
    return MM_ERROR_SENSOR_READ_ID_FAIL;
  }

  SP0820_SERIALInitialSetting(); /* apply the sensor initial setting */
  /* default setting */
  SP0820_SERIALSensor.BypassAe = KAL_FALSE;
  SP0820_SERIALSensor.BypassAwb = KAL_FALSE;
  SP0820_SERIALSensor.PvMode = KAL_TRUE;
  SP0820_SERIALSensor.BandingFreq = CAM_BANDING_50HZ;
  SP0820_SERIALSensor.InternalClock = 0;
  SP0820_SERIALSensor.Shutter = 1;
  //SP0820_SERIALSensor.Gain = 0x40;
  SP0820_SERIALSensor.FrameLength = 0;
  SP0820_SERIALSensor.LineLength = 0;
  
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSensorFeatureCtrl
*
* DESCRIPTION
*   This function set sensor feature mode
*
* PARAMETERS
*   Id: scenario id
*   In: input parameter
*   Out: output parameter
*   OutLen: output parameter length
*   RealOutLen: real output parameter length
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
 MM_ERROR_CODE_ENUM SP0820_SERIALSensorFeatureCtrl(kal_uint32 Id, void *In, void *Out, kal_uint32 OutLen, kal_uint32 *RealOutLen)
{
  MM_ERROR_CODE_ENUM ErrCode = MM_ERROR_NONE;
  
#if 0
  if (Id >= CAL_FEATURE_WEBCAM_BEGIN && Id <= CAL_FEATURE_WEBCAM_END)
  {
    return SP0820_SERIALWebcamFeatureCtrl(Id, In, Out, OutLen, RealOutLen);
  }
#endif

  switch (Id)
  {
  /* query and set series */
  case CAL_FEATURE_CAMERA_BRIGHTNESS:
  case CAL_FEATURE_CAMERA_EV_VALUE:
    ErrCode = SP0820_SERIALEv(In, Out);
    break;
  case CAL_FEATURE_CAMERA_CONTRAST:
    ErrCode = SP0820_SERIALContrast(In, Out);
    break;
  case CAL_FEATURE_CAMERA_SATURATION:
    ErrCode = SP0820_SERIALSaturation(In, Out);
    break;
  case CAL_FEATURE_CAMERA_SHARPNESS:
    ErrCode = SP0820_SERIALSharpness(In, Out);
    break;
  case CAL_FEATURE_CAMERA_WB:
    ErrCode = SP0820_SERIALWb(In, Out);
    break;
  case CAL_FEATURE_CAMERA_IMAGE_EFFECT:
    ErrCode = SP0820_SERIALEffect(In, Out);
    break;
  case CAL_FEATURE_CAMERA_STILL_CAPTURE_SIZE:
    ErrCode = SP0820_SERIALStillCaptureSize(In, Out);
    break;
  case CAL_FEATURE_CAMERA_BANDING_FREQ:
    ErrCode = ErrCode = SP0820_SERIALBanding(In, Out);
    break;
  case CAL_FEATURE_CAMERA_SCENE_MODE:
    ErrCode = SP0820_SERIALSceneMode(KAL_TRUE, In, Out);
    break;
  case CAL_FEATURE_CAMERA_VIDEO_SCENE_MODE:
    ErrCode = SP0820_SERIALSceneMode(KAL_FALSE, In, Out);
    break;
  case CAL_FEATURE_CAMERA_FLASH_MODE:
  case CAL_FEATURE_CAMERA_HUE:
  case CAL_FEATURE_CAMERA_GAMMA:
    ErrCode = MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
    break;
  
  /* get info series */
  case IMAGE_SENSOR_FEATURE_GET_SENSOR_INFO:
    SP0820_SERIALGetSensorInfo(Out);
    break;
  case IMAGE_SENSOR_FEATURE_GET_SENSOR_ID:
    ((P_IMAGE_SENSOR_COMM_DATA_STRUCT)Out)->FeatureValue = SP0820_SERIALDetectSensorId();
    break;
  case IMAGE_SENSOR_FEATURE_GET_SENSOR_FRAME_RATE:
    ((P_IMAGE_SENSOR_COMM_DATA_STRUCT)Out)->FeatureValue = SP0820_SERIALSensor.MaxFrameRate;
    break;
  case IMAGE_SENSOR_FEATURE_GET_INIT_OPERATION_PARA:
    SP0820_SERIALInitOperationPara(Out);
    break;
  case IMAGE_SENSOR_FEATURE_GET_HW_DEPENDENT_SETTING:
    CamGetHWInfo(SP0820_SERIALSensor.SensorIdx, Out);
    break;
  
  /* Set para series */
  case IMAGE_SENSOR_FEATURE_SET_SENSOR_POWER_DOWN:
   // SP0820_SERIALPowerDown();
    break;
  case IMAGE_SENSOR_FEATURE_SET_NVRAM_SENSOR_INFO:
    //SP0820_SERIALSensor.NvramData = &(((nvram_camera_para_struct *)In)->SENSOR);
    break;
  case IMAGE_SENSOR_FEATURE_SET_CAMERA_SOURCE:
    SP0820_SERIALSensor.SensorIdx = ((P_IMAGE_SENSOR_SET_CAMERA_SOURCE_STRUCT)In)->ImageSensorIdx;
    break;
  case IMAGE_SENSOR_FEATURE_CTRL_NIGHTMODE: /* move to preview */
    break;
  case IMAGE_SENSOR_FEATURE_SET_AE_BYPASS:
    SP0820_SERIALSensor.BypassAe = *(kal_bool *)In;
    SP0820_SERIALAeEnable(SP0820_SERIALSensor.BypassAe ? KAL_FALSE : KAL_TRUE);
    break;
  case IMAGE_SENSOR_FEATURE_SET_AWB_BYPASS:
    SP0820_SERIALSensor.BypassAwb = *(kal_bool *)In;
    SP0820_SERIALAwbEnable(SP0820_SERIALSensor.BypassAwb ? KAL_FALSE : KAL_TRUE);
    break;
case IMAGE_SENSOR_FEATURE_GET_SERIAL_SENSOR_INFO:
    SP0820_SERIALGetSerialSensorInfo(In, Out);
     break;
  default:
    ErrCode = MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
    break;
  }
  return ErrCode;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSensorCtrl
*
* DESCRIPTION
*   This function set sensor scenario mode
*
* PARAMETERS
*   Id: scenario id
*   In/Out: parameter
*   OutLen: out parameter length
*   RealOutLen: real out parameter length
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
 MM_ERROR_CODE_ENUM SP0820_SERIALSensorCtrl(CAL_SCENARIO_ENUM Id, void *In, void *Out, kal_uint32 OutLen, kal_uint32 *RealOutLen)
{
  switch (Id)
  {
  case CAL_SCENARIO_CAMERA_PREVIEW:
  case CAL_SCENARIO_VIDEO:
  case CAL_SCENARIO_WEBCAM_PREVIEW:
  case CAL_SCENARIO_WEBCAM_CAPTURE:
    SP0820_SERIALPreview(Id, In, Out);
    break;
  case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
    SP0820_SERIALCapture(Id, In, Out);
    break;
  default:
    break;
  }
  return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSensorClose
*
* DESCRIPTION
*   This function is to turn off sensor module power.
*
* PARAMETERS
*   None
*
* RETURNS
*   Error code
*
* LOCAL AFFECTED
*
*************************************************************************/
 MM_ERROR_CODE_ENUM SP0820_SERIALSensorClose(void)
{
   #if defined(DUAL_CAMERA_SUPPORT)
        SP0820_SERIALSensor.SensorIdx=camera_front_or_back;
   #endif

	/* Set Low to MCLK */
	DISABLE_CAMERA_CLOCK_OUTPUT_TO_CMOS;

	CisModulePowerOn(SP0820_SERIALSensor.SensorIdx, KAL_FALSE);
	vTaskDelay(2);
	CamPdnPinCtrl(0xFF, 0);

	SP0820_SERIALSensor.sccb_opened = KAL_FALSE;
	CameraSccbClose(CAMERA_SCCB_SENSOR);
//	GPIO_ModeSetup(SP0820_PCLK_PIN,1);   	//CMPCLK	
//	GPIO_ModeSetup(SP0820_DATA0_PIN,1);   	//Data0
	return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   SP0820_SERIALSensorFunc
*
* DESCRIPTION
*   This function get sensor function pointer
*
* PARAMETERS
*   Sensor function pointer
*
* RETURNS
*   Error code
*
* GLOBALS AFFECTED
*
*************************************************************************/
MM_ERROR_CODE_ENUM SP0820_SERIALSensorFunc(P_IMAGE_SENSOR_FUNCTION_STRUCT *pfSensorFunc)
{
  static IMAGE_SENSOR_FUNCTION_STRUCT ImageSensorFuncSP0820_SERIAL =
  {
    SP0820_SERIALSensorOpen,
    SP0820_SERIALSensorFeatureCtrl,
    SP0820_SERIALSensorCtrl,
    SP0820_SERIALSensorClose,
  };
  
  *pfSensorFunc = &ImageSensorFuncSP0820_SERIAL;
  
  return MM_ERROR_NONE;
}



