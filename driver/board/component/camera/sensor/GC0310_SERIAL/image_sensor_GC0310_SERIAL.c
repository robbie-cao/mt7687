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

static struct
{
    kal_bool BypassAe;
    kal_bool BypassAwb;
    kal_bool CapState; /* KAL_TRUE: in capture state, else in preview state */
    kal_bool PvMode; /* KAL_TRUE: preview size output, else full size output */
    kal_bool sccb_opened;
	kal_bool VideoMode; /* KAL_TRUE: video mode, else preview mode */
	kal_bool NightMode;/*KAL_TRUE:work in night mode, else normal mode*/
    kal_uint8 BandingFreq; /* GC0310_SERIAL_50HZ or GC0310_SERIAL_60HZ for 50Hz/60Hz */
    kal_uint32 InternalClock; /* internal clock which using process pixel(for exposure) */
    kal_uint32 Pclk; /* output clock which output to baseband */
    kal_uint32 Gain; /* base on 0x40 */
    kal_uint32 Shutter; /* unit is (linelength / internal clock) s */
    kal_uint32 FrameLength; /* total line numbers in one frame(include dummy line) */
    kal_uint32 LineLength; /* total pixel numbers in one line(include dummy pixel) */
    IMAGE_SENSOR_INDEX_ENUM SensorIdx;
    //sensor_data_struct *NvramData;
} GC0310_SERIALSensor;

#define GC0310_SERIAL_SET_PAGE0 CamWriteCmosSensor(0xfe , 0x00)
#define GC0310_SERIAL_SET_PAGE1 CamWriteCmosSensor(0xfe , 0x01)
#define GC0310_SERIAL_SET_PAGE2 CamWriteCmosSensor(0xfe , 0x02)
#define GC0310_SERIAL_SET_PAGE3 CamWriteCmosSensor(0xfe , 0x03)
kal_bool GC0310_SERIAL_CAM_BANDING_50HZ = KAL_FALSE;

#if (defined(__CATS_SUPPORT__)||defined(__ATA_SUPPORT__))
static const unsigned char CATSTestPattern[] = { 0 };
#endif

#ifdef GC0310_SERIAL_LOAD_FROM_T_FLASH
/*************************************************************************
* FUNCTION
*  GC0310_SERIAL_Initialize_from_T_Flash
*
* DESCRIPTION
*  Read the initialize setting from t-flash or user disk to speed up image quality tuning.
*
* PARAMETERS
*  None
*
* RETURNS
*  kal_uint8 - 0 : Load setting fail, 1 : Load setting successfully.
*
*************************************************************************/
static kal_uint8 GC0310_SERIAL_Initialize_from_T_Flash()
{
#include <stdlib.h>
#include "med_utility.h"
#include "fs_type.h"

#if GC0310_SERIAL_I2C_ADDR_BITS == CAMERA_SCCB_16BIT
    #define GC0310_SERIAL_REG_SKIP    0x08
#else
    #define GC0310_SERIAL_REG_SKIP    0x06
#endif
#if GC0310_SERIAL_I2C_DATA_BITS == CAMERA_SCCB_16BIT
    #define GC0310_SERIAL_VAL_SKIP    0x08
#else
    #define GC0310_SERIAL_VAL_SKIP    0x06
#endif
  
    /* 0 - Initial value, 1 - Register, 2 - Delay, 3 - End of setting. */
    #define GC0310_SERIAL_OP_CODE_INI    0x00    /* Initial value. */
    #define GC0310_SERIAL_OP_CODE_REG    0x01    /* Register */
    #define GC0310_SERIAL_OP_CODE_DLY    0x02    /* Delay */
    #define GC0310_SERIAL_OP_CODE_END    0x03    /* End of initial setting. */

    typedef struct
    {
        kal_uint16 init_reg;
        kal_uint16 init_val;  /* Save the register value and delay tick */
        kal_uint8 op_code;    /* 0 - Initial value, 1 - Register, 2 - Delay, 3 - End of setting. */
    } GC0310_SERIAL_initial_set_struct;
  
    static GC0310_SERIAL_initial_set_struct GC0310_SERIAL_Init_Reg[1000];
    static WCHAR GC0310_SERIAL_set_file_name[256] = {0};
    FS_HANDLE fp = -1;        /* Default, no file opened. */
    kal_uint8 *data_buff = NULL;
    kal_uint8 *curr_ptr = NULL;
    kal_uint32 file_size = 0;
    kal_uint32 bytes_read = 0;
    kal_uint32 i = 0, j = 0;
    kal_uint8 func_ind[3] = {0};  /* REG or DLY */
    
    kal_mem_cpy(GC0310_SERIAL_set_file_name, L"C:\\GC0310_SERIAL_Initialize_Setting.Bin", sizeof(L"C:\\GC0310_SERIAL_Initialize_Setting.Bin"));
    
    /* Search the setting file in all of the user disk. */
    curr_ptr = (kal_uint8 *)GC0310_SERIAL_set_file_name;
    while (fp < 0)
    {
        if ((*curr_ptr >= 'c' && *curr_ptr <= 'z') || (*curr_ptr >= 'C' && *curr_ptr <= 'Z'))
        {
            fp = FS_Open(GC0310_SERIAL_set_file_name, FS_READ_ONLY);
            if (fp >= 0)
            {
                break; /* Find the setting file. */
            }
            *curr_ptr = *curr_ptr + 1;
        }
        else
        {
            break ;
        }
    }
    if (fp < 0)    /* Error handle */
    {
        GC0310_SERIAL_TRACE("!!! Warning, Can't find the initial setting file!!!");
        return 0;
    }
    
    FS_GetFileSize(fp, &file_size);
    if (file_size < 20)
    {
        GC0310_SERIAL_TRACE("!!! Warning, Invalid setting file!!!");
        return 0;      /* Invalid setting file. */
    }
    
    data_buff = med_alloc_ext_mem(file_size);
    if (data_buff == NULL)
    {
        GC0310_SERIAL_TRACE("!!! Warning, Memory not enoughed...");
        return 0;        /* Memory not enough */
    }
    FS_Read(fp, data_buff, file_size, &bytes_read);
    
    /* Start parse the setting witch read from t-flash. */
    curr_ptr = data_buff;
    while (curr_ptr < (data_buff + file_size))
    {
        while ((*curr_ptr == ' ') || (*curr_ptr == '\t'))/* Skip the Space & TAB */
            curr_ptr++;
        if (((*curr_ptr) == '/') && ((*(curr_ptr + 1)) == '*'))
        {
            while (!(((*curr_ptr) == '*') && ((*(curr_ptr + 1)) == '/')))
            {
                curr_ptr++;    /* Skip block comment code. */
            }
            while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
            {
                curr_ptr++;
            }
            curr_ptr += 2;            /* Skip the enter line */
            continue ;
        }
    
        if (((*curr_ptr) == '/') || ((*curr_ptr) == '{') || ((*curr_ptr) == '}'))    /* Comment line, skip it. */
        {
            while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
            {
                curr_ptr++;
            }

            curr_ptr += 2;            /* Skip the enter line */

            continue ;
        }
        /* This just content one enter line. */
        if (((*curr_ptr) == 0x0D) && ((*(curr_ptr + 1)) == 0x0A))
        {
            curr_ptr += 2;
            continue ;
        }

        kal_mem_cpy(func_ind, curr_ptr, 3);
        curr_ptr += 4;          /* Skip "REG(" or "DLY(" */
        if (strcmp((const char *)func_ind, "REG") == 0)    /* REG */
        {
            GC0310_SERIAL_Init_Reg[i].op_code = GC0310_SERIAL_OP_CODE_REG;
      
            GC0310_SERIAL_Init_Reg[i].init_reg = strtol((const char *)curr_ptr, NULL, 16);
            curr_ptr += GC0310_SERIAL_REG_SKIP;  /* Skip "0x0000, " */
      
            GC0310_SERIAL_Init_Reg[i].init_val = strtol((const char *)curr_ptr, NULL, 16);
            curr_ptr += GC0310_SERIAL_VAL_SKIP;  /* Skip "0x0000);" */
        }
        else                  /* DLY */
        {
            /* Need add delay for this setting. */
            GC0310_SERIAL_Init_Reg[i].op_code = GC0310_SERIAL_OP_CODE_DLY;
      
            GC0310_SERIAL_Init_Reg[i].init_reg = 0xFF;
            GC0310_SERIAL_Init_Reg[i].init_val = strtol((const char *)curr_ptr, NULL, 10);  /* Get the delay ticks, the delay should less then 50 */
        }
        i++;
    
        /* Skip to next line directly. */
        while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
        {
            curr_ptr++;
        }
        curr_ptr += 2;
    }

    /* (0xFFFF, 0xFFFF) means the end of initial setting. */
    GC0310_SERIAL_Init_Reg[i].op_code = GC0310_SERIAL_OP_CODE_END;
    GC0310_SERIAL_Init_Reg[i].init_reg = 0xFF;
    GC0310_SERIAL_Init_Reg[i].init_val = 0xFF;
    i++;

    GC0310_SERIAL_TRACE("%d register read...", i - 1);
    med_free_ext_mem((void **)&data_buff);
    FS_Close(fp);

    GC0310_SERIAL_TRACE("Start apply initialize setting.");
    /* Start apply the initial setting to sensor. */
    for (j=0; j<i; j++)
    {
        if (GC0310_SERIAL_Init_Reg[j].op_code == GC0310_SERIAL_OP_CODE_END)  /* End of the setting. */
        {
            break ;
        }
        else if (GC0310_SERIAL_Init_Reg[j].op_code == GC0310_SERIAL_OP_CODE_DLY)
        {
            kal_sleep_task(GC0310_SERIAL_Init_Reg[j].init_val);    /* Delay */
        }
        else if (GC0310_SERIAL_Init_Reg[j].op_code == GC0310_SERIAL_OP_CODE_REG)
        {
            CamWriteCmosSensor(GC0310_SERIAL_Init_Reg[j].init_reg, GC0310_SERIAL_Init_Reg[j].init_val);
        }
        else
        {
            ASSERT(0);
        }
    }
    GC0310_SERIAL_TRACE("%d register applied...", j);
    return 1;  
}
#endif

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSetMirror
*
* DESCRIPTION
*   This function set the mirror to the CMOS sensor
*   IMPORTANT NOTICE:
*     the color should be normal when mirror or flip
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
static void GC0310_SERIALSetMirror(kal_uint8 Mirror)
{
	// TODO: set mirror&flip here
	CamWriteCmosSensor(0xfe, 0x00);
	switch (Mirror)
	{
		case IMAGE_SENSOR_MIRROR_H:
			CamWriteCmosSensor(0x17, 0x15);
			break;
		case IMAGE_SENSOR_MIRROR_V:
			CamWriteCmosSensor(0x17, 0x16);
			break;
		case IMAGE_SENSOR_MIRROR_HV:
			CamWriteCmosSensor(0x17, 0x17);
			break;
		default:
			CamWriteCmosSensor(0x17, 0x14);
			break;
	}
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSetClock
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
#if 0
static void GC0310_SERIALSetClock(kal_uint32 InternalClock)
{
    if (GC0310_SERIALSensor.InternalClock == InternalClock)
    {
        return;
    }
    GC0310_SERIALSensor.InternalClock = InternalClock;
    // TODO: set internal clock(use process pixel) and output clock(ouput to baseband) here
    {
    	 kal_uint8 ClkDiv;
        /* add PLL comment here */
        switch (InternalClock)
        {
		case GC0310_SERIAL_MCLK / 2:
			ClkDiv = 0x00; 
			break; /* pclk = mclk */
		case GC0310_SERIAL_MCLK / 4: 
			ClkDiv = 0x11; 
			break; /* pclk = mclk / 2 */
		case GC0310_SERIAL_MCLK / 8: 
			ClkDiv = 0x32; 
			break; /* pclk = mclk / 4 */
		default: configASSERT(0);
        }
	 CamWriteCmosSensor(0xfa, ClkDiv);
    }
}
#endif
/*************************************************************************
* FUNCTION
*   GC0310_SERIALWriteShutter
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
static void GC0310_SERIALWriteShutter(kal_uint32 Shutter)
{
    // TODO: set integration time here
	CamWriteCmosSensor(0x03, Shutter >> 8);
	CamWriteCmosSensor(0x04, Shutter);    
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALReadShutter
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
static kal_uint32 GC0310_SERIALReadShutter(void)
{
    // TODO: read integration time here
	return (CamReadCmosSensor(0x03) << 8)|CamReadCmosSensor(0x04);
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALWriteGain
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
static void GC0310_SERIALWriteGain(kal_uint32 Gain)
{
    // TODO: set global gain here
    
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALReadGain
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
#if 0
static kal_uint32 GC0310_SERIALReadGain(void)
{
    // TODO: read global gain here
    
    return 0x40;
}
#endif

/*************************************************************************
* FUNCTION
*   GC0310_SERIALAeEnable
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
static void GC0310_SERIALAeEnable(kal_bool Enable)
{
//	kal_uint16 temp_ae_reg;
	if (GC0310_SERIALSensor.BypassAe)
	{
		Enable = KAL_FALSE;
	}
	
	CamWriteCmosSensor(0xfe, 0x00);
	
	if (Enable)
	{
	   	CamWriteCmosSensor(0x4f, 01);
	}
	else
	{
	   	CamWriteCmosSensor(0x4f, 00); /* Turn OFF AEC/AGC*/
	}

}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALAwbEnable
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
static void GC0310_SERIALAwbEnable(kal_bool Enable)
{
	kal_uint16 temp_awb_reg;
	if (GC0310_SERIALSensor.BypassAwb)
	{
		Enable = KAL_FALSE;
	}
	

	CamWriteCmosSensor(0xfe, 0x00);
	temp_awb_reg = CamReadCmosSensor(0x42);
	
	if (Enable)
	{
	   	CamWriteCmosSensor(0x42, (temp_awb_reg | 0x02));
	}
	else
	{
	   	CamWriteCmosSensor(0x42, (temp_awb_reg & (~0x02))); /* Turn OFF AWB*/ 
	}

}

/*************************************************************************
* FUNCTION
*    GC0310_SERIALSetBandingStep
*
* DESCRIPTION
*    This function set base shutter or banding step to the CMOS sensor
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
#if 0
static void GC0310_SERIALSetBandingStep(void)
{
    
}
#endif
/*************************************************************************
* FUNCTION
*   GC0310_SERIALSetDummy
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
#if 0
static void GC0310_SERIALSetDummy(kal_uint32 DummyPixel, kal_uint32 DummyLine)
{
#if 0
    kal_uint32 LineLength, FrameLength;

    if (GC0310_SERIALSensor.PvMode) /* preview size output mode */
    {
        LineLength = DummyPixel + GC0310_SERIAL_PV_PERIOD_PIXEL_NUMS;
        FrameLength = DummyLine + GC0310_SERIAL_PV_PERIOD_LINE_NUMS;
    }
    else
    {
        LineLength = DummyPixel + GC0310_SERIAL_FULL_PERIOD_PIXEL_NUMS;
        FrameLength = DummyLine + GC0310_SERIAL_FULL_PERIOD_LINE_NUMS;
    }
#endif

    // TODO: set linelength/framelength or dummy pixel/line and return right value here
    {
        
    }

    /* config banding step or base shutter */
    if (GC0310_SERIALSensor.PvMode) /* preview size output mode */
    {
        GC0310_SERIALSetBandingStep();
    }
}
#endif
/*************************************************************************
* FUNCTION
*   GC0310_SERIALSetMinFps
*
* DESCRIPTION
*   This function calculate & set min frame rate
*
* PARAMETERS
*   Fps: min frame rate, base on GC0310_SERIAL_FPS(x)
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
#if 0
static void GC0310_SERIALSetMinFps(kal_uint16 Fps)
{

}
#endif
/*************************************************************************
* FUNCTION
*   GC0310_SERIALSetMaxFps
*
* DESCRIPTION
*   This function calculate & set max frame rate
*
* PARAMETERS
*   Fps: max frame rate, base on GC0310_SERIAL_FPS(x)
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
#if 0
static void GC0310_SERIALSetMaxFps(kal_uint16 Fps)
{
    kal_uint32 LineLength, FrameLength;

    /* get max line length */
    LineLength = GC0310_SERIALSensor.InternalClock * GC0310_SERIAL_FPS(1) / (Fps * GC0310_SERIAL_PV_PERIOD_LINE_NUMS);
    if (LineLength > GC0310_SERIAL_MAX_PV_LINELENGTH) /* overflow check */
    {
        LineLength = GC0310_SERIAL_MAX_PV_LINELENGTH;
    }
    if (LineLength < GC0310_SERIAL_PV_PERIOD_PIXEL_NUMS)
    {
        LineLength = GC0310_SERIAL_PV_PERIOD_PIXEL_NUMS;
    }

    /* get frame height */
    FrameLength = GC0310_SERIALSensor.InternalClock * GC0310_SERIAL_FPS(1) / (Fps * LineLength);
    if (FrameLength > GC0310_SERIAL_MAX_PV_FRAMELENGTH) /* overflow check */
    {
        FrameLength = GC0310_SERIAL_MAX_PV_FRAMELENGTH;
    }
    if (FrameLength < GC0310_SERIAL_PV_PERIOD_LINE_NUMS)
    {
        FrameLength = GC0310_SERIAL_PV_PERIOD_LINE_NUMS;
    }

    /* limit max frame rate to Fps specified */
    GC0310_SERIALSetDummy(LineLength - GC0310_SERIAL_PV_PERIOD_PIXEL_NUMS, FrameLength - GC0310_SERIAL_PV_PERIOD_LINE_NUMS);
}
#endif

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSetVideoFps
*
* DESCRIPTION
*   This function calculate & fix frame rate in video mode
*
* PARAMETERS
*   Fps: target frame rate to fixed, base on GC0310_SERIAL_FPS(x)
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
#if 0
static void GC0310_SERIALSetVideoFps(kal_uint16 Fps)
{
    /* limit max frame rate */
    GC0310_SERIALSetMaxFps(Fps);

    // TODO: fix, set max exposure time or max AE index here
    {
        
    }
}
#endif
/*************************************************************************
* FUNCTION
*   GC0310_SERIALNightMode
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
#if 0
static void GC0310_SERIALNightMode(kal_bool Enable)
{
    
}
#endif
/*************************************************************************
* FUNCTION
*   GC0310_SERIALEv
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
static MM_ERROR_CODE_ENUM GC0310_SERIALEv(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
    if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
    {
        const P_CAL_FEATURE_TYPE_ENUM_STRUCT Exposure = &Out->FeatureInfo.FeatureEnum;

        Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
        Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
        Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;
        // TODO: add supported EV enum here
        Exposure->IsSupport = KAL_TRUE;
        Exposure->ItemCount = 9;
        Exposure->SupportItem[0] = CAM_EV_NEG_4_3;
        Exposure->SupportItem[1] = CAM_EV_NEG_3_3;
        Exposure->SupportItem[2] = CAM_EV_NEG_2_3;
        Exposure->SupportItem[3] = CAM_EV_NEG_1_3;
        Exposure->SupportItem[4] = CAM_EV_ZERO;
        Exposure->SupportItem[5] = CAM_EV_POS_1_3;
        Exposure->SupportItem[6] = CAM_EV_POS_2_3;
        Exposure->SupportItem[7] = CAM_EV_POS_3_3;
        Exposure->SupportItem[8] = CAM_EV_POS_4_3;
    }
    else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
    {
    	 // TODO: adjust brightness here

		 CamWriteCmosSensor(0xfe, 0x00);
        switch (In->FeatureSetValue)
        {
		case CAM_EV_NEG_4_3:							/* EV -2 */
			CamWriteCmosSensor(0xfe, 0x01);
			CamWriteCmosSensor(0x13, 0x20);
			CamWriteCmosSensor(0xfe, 0x00);
			CamWriteCmosSensor(0xd5, 0xc0);
			break;
		case CAM_EV_NEG_3_3:							/* EV -1.5 */
			CamWriteCmosSensor(0xfe, 0x01);
			CamWriteCmosSensor(0x13, 0x30);
			CamWriteCmosSensor(0xfe, 0x00);
			CamWriteCmosSensor(0xd5, 0xd0);
			break;
		case CAM_EV_NEG_2_3:							/* EV -1 */
			CamWriteCmosSensor(0xfe, 0x01);
			CamWriteCmosSensor(0x13, 0x40);
			CamWriteCmosSensor(0xfe, 0x00);
			CamWriteCmosSensor(0xd5, 0xe0);
			break;
		case CAM_EV_NEG_1_3:							/* EV -0.5 */
			CamWriteCmosSensor(0xfe, 0x01);
			CamWriteCmosSensor(0x13, 0x50);
			CamWriteCmosSensor(0xfe, 0x00);
			CamWriteCmosSensor(0xd5, 0xf0);
			break;
		case CAM_EV_ZERO:								/* EV 0 */
			CamWriteCmosSensor(0xfe, 0x01);
			CamWriteCmosSensor(0x13, 0x60);
			CamWriteCmosSensor(0xfe, 0x00);
			CamWriteCmosSensor(0xd5, 0x00);
			break;
		case CAM_EV_POS_1_3:							/* EV +0.5 */
			CamWriteCmosSensor(0xfe, 0x01);
			CamWriteCmosSensor(0x13, 0x70);
			CamWriteCmosSensor(0xfe, 0x00);
			CamWriteCmosSensor(0xd5, 0x10);
			break;
		case CAM_EV_POS_2_3:							/* EV +1 */
			CamWriteCmosSensor(0xfe, 0x01);
			CamWriteCmosSensor(0x13, 0x80);
			CamWriteCmosSensor(0xfe, 0x00);
			CamWriteCmosSensor(0xd5, 0x20);
			break;
		case CAM_EV_POS_3_3:							/* EV +1.5 */
			CamWriteCmosSensor(0xfe, 0x01);
			CamWriteCmosSensor(0x13, 0x90);
			CamWriteCmosSensor(0xfe, 0x00);
			CamWriteCmosSensor(0xd5, 0x30);
			break;
		case CAM_EV_POS_4_3:							/* EV +2 */
			CamWriteCmosSensor(0xfe, 0x01);
			CamWriteCmosSensor(0x13, 0xA0);
			CamWriteCmosSensor(0xfe, 0x00);
			CamWriteCmosSensor(0xd5, 0x40);
			break;
		default:
	      return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
        }
        
        		 CamWriteCmosSensor(0xfe, 0x00);
    }
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALContrast
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
static MM_ERROR_CODE_ENUM GC0310_SERIALContrast(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
    if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
    {
        const P_CAL_FEATURE_TYPE_ENUM_STRUCT Contrast = &Out->FeatureInfo.FeatureEnum;

        Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
        Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
        Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;
        // TODO: add supported contrast enum here
        Contrast->IsSupport = KAL_TRUE;
        Contrast->ItemCount = 3;
        Contrast->SupportItem[0] = CAM_CONTRAST_HIGH;
        Contrast->SupportItem[1] = CAM_CONTRAST_MEDIUM;
        Contrast->SupportItem[2] = CAM_CONTRAST_LOW;
    }
    else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
    {
        // TODO: adjust contrast here
        CamWriteCmosSensor(0xfe, 0x00);
        switch (In->FeatureSetValue)
        {
        case CAM_CONTRAST_HIGH:
            CamWriteCmosSensor(0xd3, 0x48);
            break;
        case CAM_CONTRAST_MEDIUM:
            CamWriteCmosSensor(0xd3, 0x40);
            break;
        case CAM_CONTRAST_LOW:
            CamWriteCmosSensor(0xd3, 0x38);
            break;
        default:
            return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
        }
        		 CamWriteCmosSensor(0xfe, 0x00);
    }
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSaturation
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
static MM_ERROR_CODE_ENUM GC0310_SERIALSaturation(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
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
            /* add setting here */
        //    break;
        //case CAM_SATURATION_MEDIUM:
            /* add setting here */
        //    break;
        //case CAM_SATURATION_LOW:
            /* add setting here */
        //    break;
        //default:
        //    return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
        //}
    }
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSharpness
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
static MM_ERROR_CODE_ENUM GC0310_SERIALSharpness(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
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
            /* add setting here */
        //    break;
        //case CAM_SHARPNESS_MEDIUM:
            /* add setting here */
        //    break;
        //case CAM_SHARPNESS_LOW:
            /* add setting here */
        //    break;
        //default:
        //    return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
        //}
    }
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALWb
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
static MM_ERROR_CODE_ENUM GC0310_SERIALWb(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
    if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
    {
        const P_CAL_FEATURE_TYPE_ENUM_STRUCT Wb = &Out->FeatureInfo.FeatureEnum;

        Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
        Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
        Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;
        // TODO: add supported manual WB enum here
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
        // TODO: adjust manual WB here
        switch (In->FeatureSetValue)
	{
		case CAM_WB_AUTO:
			GC0310_SERIALAwbEnable(KAL_TRUE);
			break;
		case CAM_WB_CLOUD:
			GC0310_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x77,0x8c); 
			CamWriteCmosSensor(0x78,0x50);
			CamWriteCmosSensor(0x79,0x40);
			break;
		case CAM_WB_DAYLIGHT:
			GC0310_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x77,0x74); 
			CamWriteCmosSensor(0x78,0x52);
			CamWriteCmosSensor(0x79,0x40);	
			break;
		case CAM_WB_INCANDESCENCE:
			GC0310_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x77,0x48);
			CamWriteCmosSensor(0x78,0x40);
			CamWriteCmosSensor(0x79,0x5c);
			break;
		case CAM_WB_FLUORESCENT:
			GC0310_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x77,0x40);
			CamWriteCmosSensor(0x78,0x42);
			CamWriteCmosSensor(0x79,0x50);
			break;
		case CAM_WB_TUNGSTEN:
			GC0310_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x77,0x40);
			CamWriteCmosSensor(0x78,0x54);
			CamWriteCmosSensor(0x79,0x70);
			break;
	default:
		return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
	}
    CamWriteCmosSensor(0xfe, 0x00);
    }
    		 
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALEffect
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
static MM_ERROR_CODE_ENUM GC0310_SERIALEffect(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
    if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
    {
        const P_CAL_FEATURE_TYPE_ENUM_STRUCT Effect = &Out->FeatureInfo.FeatureEnum;

        Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
        Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
        Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;
        // TODO: add supported effect enum here
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
    	 CamWriteCmosSensor(0xfe, 0x00);
    			 
        // TODO: adjust effect here
        switch (In->FeatureSetValue)
	{
		case CAM_EFFECT_ENC_NORMAL:
			CamWriteCmosSensor(0x43, 0x00); 
			CamWriteCmosSensor(0xda, 0x00); 
			CamWriteCmosSensor(0xdb, 0x00); 			
			break;
		case CAM_EFFECT_ENC_GRAYSCALE:
			CamWriteCmosSensor(0x43, 0x02); 
			CamWriteCmosSensor(0xda, 0x00); 
			CamWriteCmosSensor(0xdb, 0x00); 
			break;
		case CAM_EFFECT_ENC_SEPIA:
			CamWriteCmosSensor(0x43, 0x02); 
			CamWriteCmosSensor(0xda, 0xd0); 
			CamWriteCmosSensor(0xdb, 0x28); 
			break;
		case CAM_EFFECT_ENC_SEPIAGREEN:
			CamWriteCmosSensor(0x43, 0x02); 
			CamWriteCmosSensor(0xda, 0xc0); 
			CamWriteCmosSensor(0xdb, 0xc0); 
			break;
		case CAM_EFFECT_ENC_SEPIABLUE:
			CamWriteCmosSensor(0x43, 0x02); 
			CamWriteCmosSensor(0xda, 0x50); 
			CamWriteCmosSensor(0xdb, 0xe0); 
			break;
		case CAM_EFFECT_ENC_COLORINV:
			CamWriteCmosSensor(0x43, 0x01); 
			break;
		default:
			return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
	}
    CamWriteCmosSensor(0xfe, 0x00);
    }
    		 
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALStillCaptureSize
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
MM_ERROR_CODE_ENUM GC0310_SERIALStillCaptureSize(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
    const P_CAL_FEATURE_TYPE_ENUM_STRUCT CapSize = &Out->FeatureInfo.FeatureEnum;

    if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
    {
        Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
        Out->FeatureOperationSupport = CAL_FEATURE_QUERY_OPERATION;
        Out->FeatureModeSupport = CAL_FEATURE_CAMERA_SUPPORT;
        CapSize->IsSupport = KAL_TRUE;
        // TODO: add supported capture size enum here
        
#if defined(__IMAGE_SENSOR_PRETEND_CIF__)
        CapSize->ItemCount = 2;
        CapSize->SupportItem[0] = CAM_IMAGE_SIZE_WALLPAPER;
        CapSize->SupportItem[1] = CAM_IMAGE_SIZE_QVGA;
#else		  
        CapSize->ItemCount = 3;
        CapSize->SupportItem[0] = CAM_IMAGE_SIZE_WALLPAPER;
        CapSize->SupportItem[1] = CAM_IMAGE_SIZE_QVGA;
        CapSize->SupportItem[2] = CAM_IMAGE_SIZE_VGA;	
#endif		
    }
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALBanding
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
static MM_ERROR_CODE_ENUM GC0310_SERIALBanding(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
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
        GC0310_SERIALSensor.BandingFreq = (CAM_BANDING_50HZ == In->FeatureSetValue ? GC0310_SERIAL_50HZ : GC0310_SERIAL_60HZ);
  
        if (CAM_BANDING_50HZ == In->FeatureSetValue)
        {		
  // 50Mhz, VGA size
		// based on 48M MCLK 
			CamWriteCmosSensor(0xfe,0x00);
			CamWriteCmosSensor(0x05,0x00);
			CamWriteCmosSensor(0x06,0x6A); //HB				  
			CamWriteCmosSensor(0x07,0x00);
			CamWriteCmosSensor(0x08,0x0A); //VB
			CamWriteCmosSensor(0x11,0x10); //0x10 : 66.62ms, 0x11 : 66.75ms

			CamWriteCmosSensor(0xfe,0x01);
			CamWriteCmosSensor(0x25,0x00);  //anti-flicker step [11:8]
			CamWriteCmosSensor(0x26,0x21);  //anti-flicker step [7:0]
			
			CamWriteCmosSensor(0x27,0x00);  //x6 exp level 30
			CamWriteCmosSensor(0x28,0xC6);
			CamWriteCmosSensor(0x29,0x01);  //x12 exp level 30 ~ 14.28
			CamWriteCmosSensor(0x2a,0x8C);
			CamWriteCmosSensor(0x2b,0x03);  //x24 exp level 30 ~ 7.5 
			CamWriteCmosSensor(0x2c,0x18);
			CamWriteCmosSensor(0x2d,0x06);  //x48 exp level 30 ~ 5
			CamWriteCmosSensor(0x2e,0x30);

//			CamWriteCmosSensor(0x3c,0x00);  //level 1

			CamWriteCmosSensor(0xfe,0x00);	
		
			GC0310_SERIAL_CAM_BANDING_50HZ = KAL_TRUE;
        }
        else  // 60Mhz
        {			
 // 60Mhz, VGA size
			CamWriteCmosSensor(0xfe,0x00);
			CamWriteCmosSensor(0x05,0x00);
			CamWriteCmosSensor(0x06,0x6A); //HB					
			CamWriteCmosSensor(0x07,0x00);
			CamWriteCmosSensor(0x08,0x0A); //VB
			CamWriteCmosSensor(0x11,0x10); //0x10 : 66.62ms, 0x11 : 66.75ms

			CamWriteCmosSensor(0xfe,0x01);
			CamWriteCmosSensor(0x25,0x00); //anti-flicker step [11:8]
			CamWriteCmosSensor(0x26,0x27); //anti-flicker step [7:0]
			
			CamWriteCmosSensor(0x27,0x00);//x6
			CamWriteCmosSensor(0x28,0xEA);					
			CamWriteCmosSensor(0x29,0x01);//x12
			CamWriteCmosSensor(0x2a,0xD4); 
			CamWriteCmosSensor(0x2b,0x03);//x24
			CamWriteCmosSensor(0x2c,0xA8); 
			CamWriteCmosSensor(0x2d,0x07);//x48
			CamWriteCmosSensor(0x2e,0x50);

//			CamWriteCmosSensor(0x3c,0x00);	//level 1

			CamWriteCmosSensor(0xfe,0x00);		

			GC0310_SERIAL_CAM_BANDING_50HZ = KAL_FALSE;
        }
    }
   
    return MM_ERROR_NONE;
}


/*************************************************************************
* FUNCTION
*   GC0310_SERIALSceneMode
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
static MM_ERROR_CODE_ENUM GC0310_SERIALSceneMode(kal_bool IsCam, P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
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
*	GC0310_SERIALGammaSelect
*
* DESCRIPTION
*	This function is served for FAE to select the appropriate GAMMA curve.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
#if 0
static void GC0310_SERIALGammaSelect(kal_uint32 GammaLvl)
{

}
#endif

/*************************************************************************
* FUNCTION
*   GC0310_SERIALInitialSetting
*
* DESCRIPTION
*   This function initialize the registers of CMOS sensor
*   IMPORTANT NOTICE:
*     the output format should be YUV422, order: YUYV
*     data output should be at pclk falling edge
*     VSYNC should be low active
*     HSYNC should be hight active
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
static void GC0310_SERIALInitialSetting(void)
{
//VGA setting	
	/////////////////////////////////////////////////
/////////////////   system reg  /////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0xfe,0xf0);
					CamWriteCmosSensor(0xfe,0xf0);
					CamWriteCmosSensor(0xfe,0x00);

					CamWriteCmosSensor(0xfc,0x16); //4e 
					CamWriteCmosSensor(0xfc,0x16); //4e // [0]apwd [6]regf_clk_gate 
					CamWriteCmosSensor(0xf2,0x07); //sync output
					CamWriteCmosSensor(0xf3,0x83); //ff//1f//01 data output
					CamWriteCmosSensor(0xf5,0x07); //sck_dely
					CamWriteCmosSensor(0xf7,0x88); //f8//fd
					CamWriteCmosSensor(0xf8,0x00);
					CamWriteCmosSensor(0xf9,0x4d); //0f//01 
					CamWriteCmosSensor(0xfa,0x10); //32
					CamWriteCmosSensor(0xfc,0xce);
					CamWriteCmosSensor(0xfd,0x00);

/////////////////////////////////////////////////
	/////////////////   CISCTL reg  /////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0x00,0x2f); //2f//0f//02//01灯管残影
					CamWriteCmosSensor(0x01,0x0f); //06
					CamWriteCmosSensor(0x02,0x04);
					CamWriteCmosSensor(0x03,0x04);
					CamWriteCmosSensor(0x04,0x58);
					CamWriteCmosSensor(0x05,0x00);
					CamWriteCmosSensor(0x06,0x6A); //HB				  
					CamWriteCmosSensor(0x07,0x00);
					CamWriteCmosSensor(0x08,0x0A); //VB
					CamWriteCmosSensor(0x09,0x00); //row start
					CamWriteCmosSensor(0x0a,0x00); //
					CamWriteCmosSensor(0x0b,0x00); //col start
					CamWriteCmosSensor(0x0c,0x06);
					CamWriteCmosSensor(0x0d,0x01); //height
					CamWriteCmosSensor(0x0e,0xe8); //height
					CamWriteCmosSensor(0x0f,0x02); //width
					CamWriteCmosSensor(0x10,0x88); //height
					CamWriteCmosSensor(0x11,0x10); //0x10 : 66.62ms, 0x11 : 66.75ms		
					CamWriteCmosSensor(0x17,0x14);
					CamWriteCmosSensor(0x18,0x0a); //0a//[4]double reset
					CamWriteCmosSensor(0x19,0x14); //AD pipeline
					CamWriteCmosSensor(0x1b,0x48);
					CamWriteCmosSensor(0x1e,0x6b); //3b//col bias
					CamWriteCmosSensor(0x1f,0x28); //20//00//08//txlow
					CamWriteCmosSensor(0x20,0x89); //88//0c//[3:2]DA15 
					CamWriteCmosSensor(0x21,0x49); //48//[3] txhigh
					CamWriteCmosSensor(0x22,0xb0);
					CamWriteCmosSensor(0x23,0x04); //[1:0]vcm_r
					CamWriteCmosSensor(0x24,0xff); //驱动能力//15
					CamWriteCmosSensor(0x34,0x20); //[6:4] rsg high//增加range

/////////////////////////////////////////////////
	////////////////////   BLK   ////////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0x26,0x23); //[1]dark_current_en [0]offset_en
					CamWriteCmosSensor(0x28,0xff); //BLK_limie_value
					CamWriteCmosSensor(0x29,0x00); //global offset
					CamWriteCmosSensor(0x33,0x18); //offset_ratio
					CamWriteCmosSensor(0x37,0x20); //dark_current_ratio
					CamWriteCmosSensor(0x47,0x80); //a7
					CamWriteCmosSensor(0x4e,0x66); //select_row
					CamWriteCmosSensor(0xa8,0x02); //win_width_dark, same with crop_win_width
					CamWriteCmosSensor(0xa9,0x80);

/////////////////////////////////////////////////
	//////////////////   ISP reg  ///////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0x40,0x7f);//48 
					CamWriteCmosSensor(0x41,0x21);//00//[0]curve_en
					CamWriteCmosSensor(0x42,0xcf);  //0a//[1]awn_en
					CamWriteCmosSensor(0x44,0x02); 
					CamWriteCmosSensor(0x46,0x02); //03//sync
					CamWriteCmosSensor(0x4a,0x11); 
					CamWriteCmosSensor(0x4b,0x01); 
					CamWriteCmosSensor(0x4c,0x20);//00[5]pretect exp
					CamWriteCmosSensor(0x4d,0x05);//update gain mode
					CamWriteCmosSensor(0x4f,0x01); 
					CamWriteCmosSensor(0x50,0x01);//crop enable
					CamWriteCmosSensor(0x55,0x01);//crop window height
					CamWriteCmosSensor(0x56,0xe0); 
					CamWriteCmosSensor(0x57,0x02);//crop window width
					CamWriteCmosSensor(0x58,0x80); 

	/////////////////////////////////////////////////  
	///////////////////   GAIN   ////////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0x70,0x50); //70 //80//global gain
					CamWriteCmosSensor(0x5a,0x98);//84//analog gain 0
					CamWriteCmosSensor(0x5b,0xdc);//c9
					CamWriteCmosSensor(0x5c,0xfe);//ed//not use pga gain highest level

//					CamWriteCmosSensor(0x77,0x74);//awb gain
//					CamWriteCmosSensor(0x78,0x40); 
//					CamWriteCmosSensor(0x79,0x5f); 

					CamWriteCmosSensor(0x7a,0x80); //R ratio///////////////////////////////////////////////// 
	///////////////////   DNDD  /////////////////////
///////////////////////////////////////////////// 
					CamWriteCmosSensor(0x82,0x05); //05
					CamWriteCmosSensor(0x83,0x0b);

///////////////////////////////////////////////// 
	//////////////////   EEINTP  ////////////////////
///////////////////////////////////////////////// 
					CamWriteCmosSensor(0x8f,0xff);//skin th
					CamWriteCmosSensor(0x90,0x8c);
					CamWriteCmosSensor(0x92,0x08);//10//05
					CamWriteCmosSensor(0x93,0x03);//10//00
					CamWriteCmosSensor(0x95,0x53);//88
					CamWriteCmosSensor(0x96,0x56);//88 

///////////////////////////////////////////////// 
	/////////////////////  ASDE  ////////////////////
///////////////////////////////////////////////// 
					CamWriteCmosSensor(0xfe,0x00);

					CamWriteCmosSensor(0x9a,0x20); //Y_limit
					CamWriteCmosSensor(0x9b,0xb0); //Y_slope
					CamWriteCmosSensor(0x9c,0x40); //LSC_th
					CamWriteCmosSensor(0x9d,0xa0); //80//LSC_slope

					CamWriteCmosSensor(0xa1,0xf0);//00
	CamWriteCmosSensor(0xa2,0x32); //12
					CamWriteCmosSensor(0xa4,0x30); //Sat_slope
					CamWriteCmosSensor(0xa5,0x30); //Sat_limit
					CamWriteCmosSensor(0xaa,0x50); //OT_th
					CamWriteCmosSensor(0xac,0x22); //DN_slope_low

/////////////////////////////////////////////////
	///////////////////   YCP  //////////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0xd0,0x40); 
					CamWriteCmosSensor(0xd1,0x2b);//28//38
					CamWriteCmosSensor(0xd2,0x2b);//28//38
					CamWriteCmosSensor(0xd3,0x40);//3c
					CamWriteCmosSensor(0xd6,0xf2); 
					CamWriteCmosSensor(0xd7,0x1b); 
					CamWriteCmosSensor(0xd8,0x18); 
					CamWriteCmosSensor(0xdd,0x73);//no edge dec saturation

/////////////////////////////////////////////////
	////////////////////   AEC   ////////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0xfe,0x01);
					CamWriteCmosSensor(0x05,0x30); //40//AEC_center-x1 X16
					CamWriteCmosSensor(0x06,0x75); //70//X2 
					CamWriteCmosSensor(0x07,0x40); //60//Y1 X8
					CamWriteCmosSensor(0x08,0xb0); //90//Y2
					CamWriteCmosSensor(0x0a,0xc5);//c1//81//81//01//f
					CamWriteCmosSensor(0x12,0x52);
					CamWriteCmosSensor(0x13,0x35); //28 //78//Y target
					
					CamWriteCmosSensor(0x1f,0x30); //QQ
					CamWriteCmosSensor(0x20,0xc0); //QQ 80

					CamWriteCmosSensor(0x25,0x00);  //anti-flicker step [11:8]
					CamWriteCmosSensor(0x26,0x27); //anti-flicker step [7:0]	
					CamWriteCmosSensor(0x27,0x00);//x6
					CamWriteCmosSensor(0x28,0xEA);					
					CamWriteCmosSensor(0x29,0x01);//x12
					CamWriteCmosSensor(0x2a,0xD4); 
					CamWriteCmosSensor(0x2b,0x03);//x24
					CamWriteCmosSensor(0x2c,0xA8); 
					CamWriteCmosSensor(0x2d,0x07);//x48
					CamWriteCmosSensor(0x2e,0x50);
					CamWriteCmosSensor(0x3c,0x00);
					CamWriteCmosSensor(0x3e,0x40); //40
					CamWriteCmosSensor(0x3f,0x5c); //57
					CamWriteCmosSensor(0x40,0x7b); //7d
					CamWriteCmosSensor(0x41,0xbd); //a7
					CamWriteCmosSensor(0x42,0xf6); //01
					CamWriteCmosSensor(0x43,0x63); //4e
//					CamWriteCmosSensor(0x04,0xe3);
					CamWriteCmosSensor(0x03,0x60); //70

					CamWriteCmosSensor(0x44,0x03);//04

/////////////////////////////////////////////////
	////////////////////   AWB   ////////////////////
/////////////////////////////////////////////////

					CamWriteCmosSensor(0x1c,0x91);//21 //luma_level_for_awb_select

					CamWriteCmosSensor(0x61,0x9e);
					CamWriteCmosSensor(0x62,0xa5); 
					CamWriteCmosSensor(0x63,0xd0);//a0//d0//a0//a0 x1-th
					CamWriteCmosSensor(0x64,0x20);
					CamWriteCmosSensor(0x65,0x06);//60
					CamWriteCmosSensor(0x66,0x02);//02//04
					CamWriteCmosSensor(0x67,0x84);//82//82
					CamWriteCmosSensor(0x6b,0x20);//40//00//40
					CamWriteCmosSensor(0x6c,0x10);//00
					CamWriteCmosSensor(0x6d,0x32);//00
					CamWriteCmosSensor(0x6e,0x38);//f8//38//indoor mode
					CamWriteCmosSensor(0x6f,0x62);//80//a0//indoor-th
					CamWriteCmosSensor(0x70,0x00);
					CamWriteCmosSensor(0x78,0xb0);

					CamWriteCmosSensor(0xfe,0x01);// gain limit
					CamWriteCmosSensor(0x0c,0x01);

					CamWriteCmosSensor(0xfe,0x01);// G gain limit
					CamWriteCmosSensor(0x78,0xa0);
					
					CamWriteCmosSensor(0xfe,0x01);
					CamWriteCmosSensor(0x1c,0x21);

					CamWriteCmosSensor(0xfe,0x01);
					CamWriteCmosSensor(0x90,0x00);
					CamWriteCmosSensor(0x91,0x00);
					CamWriteCmosSensor(0x92,0xf0);
					CamWriteCmosSensor(0x93,0xd1);
					CamWriteCmosSensor(0x94,0x50);
					CamWriteCmosSensor(0x95,0xfd);
					CamWriteCmosSensor(0x96,0xdf);
					CamWriteCmosSensor(0x97,0x1f);
					CamWriteCmosSensor(0x98,0x00);
					CamWriteCmosSensor(0x99,0xa5);
					CamWriteCmosSensor(0x9a,0x23);
					CamWriteCmosSensor(0x9b,0x0c);
					CamWriteCmosSensor(0x9c,0x55);
					CamWriteCmosSensor(0x9d,0x3b);
					CamWriteCmosSensor(0x9e,0xaa);
					CamWriteCmosSensor(0x9f,0x00);
					CamWriteCmosSensor(0xa0,0x00);
					CamWriteCmosSensor(0xa1,0x00);
					CamWriteCmosSensor(0xa2,0x00);
					CamWriteCmosSensor(0xa3,0x00);
					CamWriteCmosSensor(0xa4,0x00);
					CamWriteCmosSensor(0xa5,0x00);
					CamWriteCmosSensor(0xa6,0xcc);
					CamWriteCmosSensor(0xa7,0xb7);
					CamWriteCmosSensor(0xa8,0x50);
					CamWriteCmosSensor(0xa9,0xb6);
					CamWriteCmosSensor(0xaa,0x8d);
					CamWriteCmosSensor(0xab,0xa8);
					CamWriteCmosSensor(0xac,0x85);
					CamWriteCmosSensor(0xad,0x55);
					CamWriteCmosSensor(0xae,0xbb);
					CamWriteCmosSensor(0xaf,0xa9);
					CamWriteCmosSensor(0xb0,0xba);
					CamWriteCmosSensor(0xb1,0xa2);
					CamWriteCmosSensor(0xb2,0x55);
					CamWriteCmosSensor(0xb3,0x00);
					CamWriteCmosSensor(0xb4,0x00);
					CamWriteCmosSensor(0xb5,0x00);
					CamWriteCmosSensor(0xb6,0x00);
					CamWriteCmosSensor(0xb7,0x00);
					CamWriteCmosSensor(0xb8,0x9e);
					CamWriteCmosSensor(0xb9,0xc9);

/////////////////////////////////////////////////
	////////////////////   CC    ////////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0xfe,0x01);
					CamWriteCmosSensor(0xd0,0x44); 
					CamWriteCmosSensor(0xd1,0xFb); 
					CamWriteCmosSensor(0xd2,0x00); 
					CamWriteCmosSensor(0xd3,0x08); 
					CamWriteCmosSensor(0xd4,0x40); 
					CamWriteCmosSensor(0xd5,0x0d); 
					CamWriteCmosSensor(0xfe,0x00);
/////////////////////////////////////////////////
	////////////////////   LSC   ////////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0xfe,0x01); 
					CamWriteCmosSensor(0x76,0x60);//R_gain_limit
					CamWriteCmosSensor(0xc1,0x3c);//row center
					CamWriteCmosSensor(0xc2,0x50);//col center
					CamWriteCmosSensor(0xc3,0x00);//b4 sign
					CamWriteCmosSensor(0xc4,0x4b);// b2 R
					CamWriteCmosSensor(0xc5,0x38);//G
					CamWriteCmosSensor(0xc6,0x38);//B
					CamWriteCmosSensor(0xc7,0x10);//b4 R
					CamWriteCmosSensor(0xc8,0x00);//G
					CamWriteCmosSensor(0xc9,0x00);//b
					CamWriteCmosSensor(0xdc,0x20);//lsc_Y_dark_th
					CamWriteCmosSensor(0xdd,0x10);//lsc_Y_dark_slope
					CamWriteCmosSensor(0xdf,0x00); 
					CamWriteCmosSensor(0xde,0x00); 

/////////////////////////////////////////////////
	///////////////////  Histogram  /////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0x01,0x10); //precision
					CamWriteCmosSensor(0x0b,0x31); //close fix_target_mode
					CamWriteCmosSensor(0x0e,0x6c); //th_low
					CamWriteCmosSensor(0x0f,0x0f); //color_diff_th
					CamWriteCmosSensor(0x10,0x6e); //th_high
					CamWriteCmosSensor(0x12,0xa0); //a1//enable
					CamWriteCmosSensor(0x15,0x40); //target_Y_limit
					CamWriteCmosSensor(0x16,0x60); //th_for_disable_hist
					CamWriteCmosSensor(0x17,0x20); //luma_slope

/////////////////////////////////////////////////
	//////////////   Measure Window   ///////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0xcc,0x0c);//aec window size 
					CamWriteCmosSensor(0xcd,0x10); 
					CamWriteCmosSensor(0xce,0xa0); 
					CamWriteCmosSensor(0xcf,0xe6); 

/////////////////////////////////////////////////
	/////////////////   dark sun   //////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0x45,0xf7);
					CamWriteCmosSensor(0x46,0xff); //f0//sun vaule th
					CamWriteCmosSensor(0x47,0x15);
					CamWriteCmosSensor(0x48,0x03); //sun mode
					CamWriteCmosSensor(0x4f,0x60); //sun_clamp

/////////////////////////////////////////////////
	/////////////////////  SPI   ////////////////////
/////////////////////////////////////////////////
					CamWriteCmosSensor(0xfe,0x03);
					CamWriteCmosSensor(0x01,0x00);
					CamWriteCmosSensor(0x02,0x00);
					CamWriteCmosSensor(0x10,0x00);
					CamWriteCmosSensor(0x15,0x00);
					CamWriteCmosSensor(0x17,0x00); //01//03
					CamWriteCmosSensor(0x04,0x10);
					CamWriteCmosSensor(0x05,0x00);
					CamWriteCmosSensor(0x40,0x00);
					CamWriteCmosSensor(0x51,0x03);
					CamWriteCmosSensor(0x52,0x20); //a2 //a0//80//00
					CamWriteCmosSensor(0x53,0xa4); //24
					CamWriteCmosSensor(0x54,0x20);
					CamWriteCmosSensor(0x55,0x20); //QQ//01
					CamWriteCmosSensor(0x5a,0x40); //00 //yuv 
					CamWriteCmosSensor(0x5b,0x80);
					CamWriteCmosSensor(0x5c,0x02);
					CamWriteCmosSensor(0x5d,0xe0);
					CamWriteCmosSensor(0x5e,0x01);

					CamWriteCmosSensor(0xfe,0x00);

}

/*************************************************************************
* FUNCTION
*	GC0310_SERIAL_Write_More_Registers
*
* DESCRIPTION
*	This function is served for FAE to modify the necessary Init Regs. Do not modify the regs
*     in init_GC0310_SERIAL() directly.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void GC0310_SERIALMoreSetting(void)
{

}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALPvSetting
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
static void GC0310_SERIALPvSetting(void)
{
	
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALCapSetting
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
static void GC0310_SERIALCapSetting(void)
{

}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALGetSensorInfo
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
static void GC0310_SERIALGetSensorInfo(P_IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT Info)
{
    Info->SensorId = GC0310_SERIAL_SENSOR_ID;
    Info->SensorIf = IMAGE_SENSOR_IF_SERIAL;

    /* data format */
    Info->PreviewNormalDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;
    Info->PreviewHMirrorDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;
    Info->PreviewVMirrorDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;
    Info->PreviewHVMirrorDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;
    Info->CaptureDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;
    Info->SerialLaneNo = SERIAL_1_LANE;
    Info->PreviewMclkFreq = 24000000;//48000000;
    Info->CaptureMclkFreq = 48000000;
    Info->VideoMclkFreq = 48000000;
	
    Info->PreviewWidth = GC0310_SERIAL_IMAGE_SENSOR_PV_WIDTH;
    Info->PreviewHeight = GC0310_SERIAL_IMAGE_SENSOR_PV_HEIGHT;
    Info->FullWidth = GC0310_SERIAL_IMAGE_SENSOR_FULL_WIDTH;
    Info->FullHeight = GC0310_SERIAL_IMAGE_SENSOR_FULL_HEIGHT;
    Info->SensorAfSupport = KAL_FALSE;
    Info->SensorFlashSupport = KAL_FALSE;
    Info->PixelClkPolarity = POLARITY_LOW;

    /* HSYNC/VSYNC polarity */
    Info->HsyncPolarity = POLARITY_LOW; /* actually high active when set POLARITY_LOW */
    Info->VsyncPolarity = POLARITY_LOW;

    // TODO: set isp pclk invert here if needed
    /* isp pclk invert switch */
    Info->PixelClkInv = KAL_FALSE;
}


/*************************************************************************
* FUNCTION
*    GC0310SerialSensorFeatureCtrl
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
MM_ERROR_CODE_ENUM GC0310_SERIALGetSerialSensorInfo(P_SERIAL_SENSOR_INFO_IN_STRUCT In,P_SERIAL_SENSOR_INFO_OUT_STRUCT Out)
{
	Out->para_1 = KAL_TRUE; 								/* SCK clock inverse */
	Out->para_2 = 1;										/* Cycle */

	if (In->ScenarioId == CAL_SCENARIO_CAMERA_STILL_CAPTURE)
	{
		Out->para_3 = GC0310_SERIAL_IMAGE_SENSOR_FULL_WIDTH; 	/* Sensor capture data valid width */
		Out->para_4 = GC0310_SERIAL_IMAGE_SENSOR_FULL_HEIGHT;	/* Sensor capture data valid height */
	}
	else
	{
		Out->para_3 = GC0310_SERIAL_IMAGE_SENSOR_PV_WIDTH; 	/* Sensor preview data valid width */
		Out->para_4 = GC0310_SERIAL_IMAGE_SENSOR_PV_HEIGHT; 	/* Sensor preview data valid height */
	}

	Out->para_5  = 2;//lane number
	Out->para_6  = KAL_TRUE;//DDR supprt
	Out->para_7  = KAL_TRUE;//CRC support

	return MM_ERROR_NONE; 
}


/*************************************************************************
* FUNCTION
*   GC0310_SERIALPowerOn
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
static kal_uint32 GC0310_SERIALPowerOn(void)
{
    static kal_uint8 GC0310_SERIALI2cAddr[] = {GC0310_SERIAL_WRITE_ID_0,
#if (GC0310_SERIAL_WRITE_ID_1 != 0xFF)
    GC0310_SERIAL_WRITE_ID_1,
#endif
#if (GC0310_SERIAL_WRITE_ID_2 != 0xFF)
    GC0310_SERIAL_WRITE_ID_2,
#endif
#if (GC0310_SERIAL_WRITE_ID_3 != 0xFF)
    GC0310_SERIAL_WRITE_ID_3,
#endif
    };
    kal_uint32 SensorId;
    kal_uint32 SensorId1,SensorId2;
    kal_uint8 i;
	
    CisModulePowerOn(GC0310_SERIALSensor.SensorIdx, KAL_TRUE);
    GC0310_SERIALSensor.sccb_opened = KAL_TRUE;              // lanking 20120423
    CameraSccbOpen(CAMERA_SCCB_SENSOR, GC0310_SERIAL_WRITE_ID_0,
    GC0310_SERIAL_I2C_ADDR_BITS, GC0310_SERIAL_I2C_DATA_BITS, GC0310_SERIAL_HW_I2C_SPEED);

    /* add under line if sensor I2C do not support repeat start */
    //CameraSccbDisWR(CAMERA_SCCB_SENSOR);
    vTaskDelay(2);
    // TODO: add hardware reset  by spec here
    {
            GC0310_SERIAL_SET_PDN_LOW;
            vTaskDelay(1);

	    GC0310_SERIAL_SET_RST_HIGH;
	    vTaskDelay(1);
	    GC0310_SERIAL_SET_RST_LOW;
	    vTaskDelay(1);
	    GC0310_SERIAL_SET_RST_HIGH;
	    vTaskDelay(1);
    }

    for (i = 0; i < sizeof(GC0310_SERIALI2cAddr) / sizeof(GC0310_SERIALI2cAddr[0]); i++)
    {
        //CameraSccbSetAddr(CAMERA_SCCB_SENSOR, GC0310_SERIALI2cAddr[i]);

        vTaskDelay(1);
        SensorId1 = CamReadCmosSensor(0xf0);						
        SensorId2 = CamReadCmosSensor(0xf1);

		if ((SensorId1 == 0xA3)&&(SensorId2 == 0x10))
			SensorId = 0xA310;
		
        if (GC0310_SERIAL_SENSOR_ID == SensorId)
        {
            kal_uint8 Temp;

            /* swap the correct i2c address to first one, it will speed up next time read sensor ID */
            Temp = GC0310_SERIALI2cAddr[0];
            GC0310_SERIALI2cAddr[0] = GC0310_SERIALI2cAddr[i];
            GC0310_SERIALI2cAddr[i] = Temp;
            break;
        }
    }
    GC0310_SERIAL_TRACE("SENSOR ID: %x\r\n", SensorId);

    return SensorId;
}
/*************************************************************************
* FUNCTION
*   GC0310_SERIALPowerDown
*
* DESCRIPTION
*   This function power down the cmos sensor
*   IMPORTANT NOTICE:
*     data[7:0]/HSYNC/VSYNC/PCLK pin should be in High-Z state after executed this function
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
static void GC0310_SERIALPowerDown(void)
{
    // TODO: power down sensor by spec here
    if(GC0310_SERIALSensor.sccb_opened == KAL_TRUE)
    {
	CamWriteCmosSensor(0xfc, 0x01);
	CamWriteCmosSensor(0xf1, 0x00);
    }

    GC0310_SERIAL_SET_RST_LOW;
    vTaskDelay(1);
    GC0310_SERIAL_SET_PDN_HIGH;
}
/*************************************************************************
* FUNCTION
*   GC0310_SERIALPreview
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
static void GC0310_SERIALPreview(CAL_SCENARIO_ENUM Id, P_IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT In, P_IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT Out)
{
    GC0310_SERIALSensor.BandingFreq = (CAM_BANDING_50HZ == In->BandingFreq ? GC0310_SERIAL_50HZ : GC0310_SERIAL_60HZ);

	GC0310_SERIALSensor.NightMode = In->NightMode;
    /* change to preview size output */
    GC0310_SERIALSensor.PvMode = KAL_TRUE;
    GC0310_SERIALPvSetting();
    //GC0310_SERIALSetClock(GC0310_SERIAL_PV_INTERNAL_CLK);

    /* set preview frame rate range */
    switch (Id)
    {
    case CAL_SCENARIO_VIDEO:
		GC0310_SERIALSensor.VideoMode = KAL_TRUE;
        //GC0310_SERIALSetVideoFps(In->MaxVideoFrameRate);
          
        break;
    default:
		GC0310_SERIALSensor.VideoMode = KAL_FALSE;
        //GC0310_SERIALSetMaxFps(GC0310_SERIAL_FPS(30));
        //GC0310_SERIALSetMinFps(In->NightMode ? GC0310_SERIAL_FPS(5) : GC0310_SERIAL_FPS(10));
        /* roll back shutter&gain from capture state */
        if (GC0310_SERIALSensor.CapState)
        {
            GC0310_SERIALWriteShutter(GC0310_SERIALSensor.Shutter);
            GC0310_SERIALWriteGain(GC0310_SERIALSensor.Gain);
        }
    }

    /* misc setting */
    GC0310_SERIALSetMirror(In->ImageMirror);
//    GC0310_SERIALNightMode(In->NightMode);
    if(GC0310_SERIALSensor.NightMode == KAL_TRUE)
    {
    	if (GC0310_SERIALSensor.VideoMode == KAL_TRUE) //Video night mode 15FPS
    	{	
  	    	CamWriteCmosSensor(0xfe,0x01);
        	CamWriteCmosSensor(0x3c,0x20); 
        	CamWriteCmosSensor(0xfe,0x00);
        }
        else		//Night mode 5~30FPS
        {
    	 	CamWriteCmosSensor(0xfe,0x01);
        	CamWriteCmosSensor(0x3c,0x30); 
        	CamWriteCmosSensor(0xfe,0x00);      	
        }	
    }
    else
    {
      	if (GC0310_SERIALSensor.VideoMode == KAL_TRUE) //Video normal mode 30FPS
      	{
  	    	CamWriteCmosSensor(0xfe,0x01);
        	CamWriteCmosSensor(0x3c,0x00); 
        	CamWriteCmosSensor(0xfe,0x00);
      	}
      	else  //Normal mode 
      	{
   	    	CamWriteCmosSensor(0xfe,0x01);
        	CamWriteCmosSensor(0x3c,0x10); 
        	CamWriteCmosSensor(0xfe,0x00);
      	}	
    }

  
    /* change to preview state */
    GC0310_SERIALSensor.CapState = KAL_FALSE;

    /* enable ae/awb */
    GC0310_SERIALAeEnable(KAL_TRUE);
    GC0310_SERIALAwbEnable(KAL_TRUE);

    /* set grab window */
    Out->WaitStableFrameNum = 0;
    Out->GrabStartX = GC0310_SERIAL_PV_GRAB_START_X;
    Out->GrabStartY = GC0310_SERIAL_PV_GRAB_START_Y;
    Out->ExposureWindowWidth = GC0310_SERIAL_PV_GRAB_WIDTH;
    Out->ExposureWindowHeight = GC0310_SERIAL_PV_GRAB_HEIGHT;
    GC0310_SERIAL_TRACE("%s:%d GrabStartX=%d, GrabStartY=%d WWidth=%d, WHeight=%d\r\n", __FUNCTION__, __LINE__, Out->GrabStartX, Out->GrabStartY, Out->ExposureWindowWidth, Out->ExposureWindowHeight);
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALCapture
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
static void GC0310_SERIALCapture(CAL_SCENARIO_ENUM Id, P_IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT In, P_IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT Out)
{
    //kal_uint32 PvClk, PvLineLength, CapShutter, CapLineLength, DummyPixel = 0;
    kal_uint32 DummyPixel = 0;

    /* back up preview clock/linelength/shutter */
    //PvClk = GC0310_SERIALSensor.InternalClock;
    //PvLineLength = GC0310_SERIALSensor.LineLength;
    GC0310_SERIALCapSetting();
        
    GC0310_SERIALSensor.Shutter = GC0310_SERIALReadShutter();
    //GC0310_SERIALSensor.Gain = GC0310_SERIALReadGain();

    /* change to capture state */
    GC0310_SERIALSensor.CapState = KAL_TRUE;
    if(0) //(In->ImageTargetWidth <= GC0310_SERIAL_IMAGE_SENSOR_PV_WIDTH && In->ImageTargetHeight <= GC0310_SERIAL_IMAGE_SENSOR_PV_HEIGHT)
    {
        // TODO: add dummy pixel or reduce pclk until capture success
        {
#if (defined(DRV_ISP_YUV_BURST_MODE_SUPPORT) || defined(DRV_ISP_6276_SERIES)) /* MT6253/76 */

#else
//#error not verify yet
#endif
        }

        if (DummyPixel)
        {
            /* disable ae/awb */
            GC0310_SERIALAeEnable(KAL_FALSE);
            GC0310_SERIALAwbEnable(KAL_FALSE);

	     #if 0
            /* set dummy pixel */
            if (PvLineLength - GC0310_SERIAL_PV_PERIOD_PIXEL_NUMS > DummyPixel)
            {
                DummyPixel = PvLineLength - GC0310_SERIAL_PV_PERIOD_PIXEL_NUMS;
            }
            GC0310_SERIALSetDummy(DummyPixel, 0);

            /* shutter translation, GC0310_SERIALSensor.InternalClock >> 14 & PvClk >> 14 to avoid data overflow */
            CapShutter = (GC0310_SERIALSensor.Shutter * (GC0310_SERIALSensor.InternalClock >> 14) + (PvClk >> 15)) / (PvClk >> 14);
            CapShutter = (CapShutter * PvLineLength + (GC0310_SERIALSensor.LineLength >> 1)) / GC0310_SERIALSensor.LineLength;

            /* write shutter */
            GC0310_SERIALWriteShutter(CapShutter);
            #endif

            Out->WaitStableFrameNum = GC0310_SERIAL_CAPTURE_DELAY_FRAME;
        }
        else
        {
            Out->WaitStableFrameNum = 0;
        }

        /* set grab window */
        Out->GrabStartX = GC0310_SERIAL_PV_GRAB_START_X;
        Out->GrabStartY = GC0310_SERIAL_PV_GRAB_START_Y;
        Out->ExposureWindowWidth = GC0310_SERIAL_PV_GRAB_WIDTH;
        Out->ExposureWindowHeight = GC0310_SERIAL_PV_GRAB_HEIGHT;
    }
    else
    {
        /* disable ae/awb */
//        GC0310_SERIALAeEnable(KAL_FALSE);
//        GC0310_SERIALAwbEnable(KAL_FALSE);

        /* change to full size output */
        GC0310_SERIALSensor.PvMode = KAL_FALSE;
        //GC0310_SERIALCapSetting();
        //GC0310_SERIALSetClock(GC0310_SERIAL_CAP_INTERNAL_CLK);

        // TODO: add dummy pixel or reduce pclk until capture success
        {
#if (defined(DRV_ISP_YUV_BURST_MODE_SUPPORT) || defined(DRV_ISP_6276_SERIES)) /* MT6253/76 */
#else
//#error not verify yet
#endif
        }

	 #if 0
        /* set dummy pixel */
        CapLineLength = GC0310_SERIALSensor.InternalClock * GC0310_SERIAL_FPS(1) / (GC0310_SERIAL_FULL_PERIOD_LINE_NUMS * In->MaxVideoFrameRate);
        if (CapLineLength < GC0310_SERIAL_FULL_PERIOD_PIXEL_NUMS)
        {
            CapLineLength = GC0310_SERIAL_FULL_PERIOD_PIXEL_NUMS;
        }
        if (CapLineLength > GC0310_SERIAL_MAX_CAP_LINELENGTH) /* register limitation */
        {
            GC0310_SERIAL_TRACE("caplinelength: %d, capframerate: %d overflow!!!", CapLineLength, In->MaxVideoFrameRate);
            CapLineLength = GC0310_SERIAL_MAX_CAP_LINELENGTH;
        }
        if (CapLineLength - GC0310_SERIAL_FULL_PERIOD_PIXEL_NUMS > DummyPixel)
        {
            DummyPixel = CapLineLength - GC0310_SERIAL_FULL_PERIOD_PIXEL_NUMS;
        }
        GC0310_SERIALSetDummy(DummyPixel, 0);

        /* shutter translation, GC0310_SERIALSensor.InternalClock >> 14 & PvClk >> 14 to avoid data overflow */
        CapShutter = (GC0310_SERIALSensor.Shutter * (GC0310_SERIALSensor.InternalClock >> 14) + (PvClk >> 15)) / (PvClk >> 14);
        CapShutter = (CapShutter * PvLineLength + (GC0310_SERIALSensor.LineLength >> 1)) / GC0310_SERIALSensor.LineLength;

        /* write shutter */
        GC0310_SERIALWriteShutter(CapShutter);
	 #endif
	 
	 /* set grab window */
        Out->WaitStableFrameNum = GC0310_SERIAL_CAPTURE_DELAY_FRAME;
        Out->GrabStartX = GC0310_SERIAL_FULL_GRAB_START_X;
        Out->GrabStartY = GC0310_SERIAL_FULL_GRAB_START_Y;
        Out->ExposureWindowWidth = GC0310_SERIAL_FULL_GRAB_WIDTH;
        Out->ExposureWindowHeight = GC0310_SERIAL_FULL_GRAB_HEIGHT;
    }
}

static MM_ERROR_CODE_ENUM GC0310_SERIALSensorClose(void);

/*************************************************************************
* FUNCTION
*   GC0310_SERIALDetectSensorId
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
static kal_uint32 GC0310_SERIALDetectSensorId(void)
{
    kal_uint32 SensorId;
    IMAGE_SENSOR_INDEX_ENUM InvIdx;
    kal_int8 i, j;

    GC0310_SERIAL_TRACE("%s:%d", __FUNCTION__, __LINE__);

    switch (GC0310_SERIALSensor.SensorIdx)
    {
    case IMAGE_SENSOR_MAIN:
    case IMAGE_SENSOR_BAK1:
        InvIdx = IMAGE_SENSOR_SUB;
        break;
    default:
        InvIdx = IMAGE_SENSOR_MAIN;
        break;
    }
    for (i = 1; i >= 0; i--)
    {
        for (j = 1; j >= 0; j--)
        {
            CamRstPinCtrl(InvIdx, i);
            CamPdnPinCtrl(InvIdx, j);
            SensorId = GC0310_SERIALPowerOn();
            GC0310_SERIALSensorClose();
            if (GC0310_SERIAL_SENSOR_ID == SensorId)
            {
                return GC0310_SERIAL_SENSOR_ID;
            }
        }
    }
    return MM_ERROR_SENSOR_READ_ID_FAIL;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALInitOperationPara
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
static void GC0310_SERIALInitOperationPara(P_IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT Para)
{
    Para->CaptureDelayFrame = 0; /* use WaitStableFrameNum instead of this */
    Para->PreviewDelayFrame = GC0310_SERIAL_PREVIEW_DELAY_FRAME;
    Para->PreviewDisplayWaitFrame = GC0310_SERIAL_FIRST_PREVIEW_DELAY_FRAME;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSensorOpen
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
static MM_ERROR_CODE_ENUM GC0310_SERIALSensorOpen(void)
{
    if (GC0310_SERIALPowerOn() != GC0310_SERIAL_SENSOR_ID)
    {
        return MM_ERROR_SENSOR_READ_ID_FAIL;
    }
#ifdef GC0310_SERIAL_LOAD_FROM_T_FLASH
    if (!GC0310_SERIAL_Initialize_from_T_Flash()) /* for debug use. */
#endif
    {
        GC0310_SERIALInitialSetting(); /* apply the sensor initial setting */
	//GC0310_SERIALMoreSetting();
    }

    /* default setting */
    GC0310_SERIALSensor.BypassAe = KAL_FALSE; /* for panorama view */
    GC0310_SERIALSensor.BypassAwb = KAL_FALSE; /* for panorama view */
    GC0310_SERIALSensor.CapState = KAL_FALSE; /* preview state */
    GC0310_SERIALSensor.PvMode = KAL_TRUE; /* preview size output mode */
    GC0310_SERIALSensor.VideoMode = KAL_FALSE; /* KAL_TRUE: video mode, else preview mode */
    GC0310_SERIALSensor.NightMode = KAL_FALSE;/*KAL_TRUE:work in night mode, else normal mode*/
    GC0310_SERIALSensor.BandingFreq = GC0310_SERIAL_50HZ;
    GC0310_SERIALSensor.InternalClock = 1; /* will be update by setclock function */
    GC0310_SERIALSensor.Shutter = 1; /* default shutter 1 avoid divide by 0 */
    GC0310_SERIALSensor.Gain = 0x40; /* default gain 1x */
    //GC0310_SERIALSensor.FrameLength = GC0310_SERIAL_PV_PERIOD_LINE_NUMS; /* will be update by setdummy function */
    //GC0310_SERIALSensor.LineLength = GC0310_SERIAL_PV_PERIOD_PIXEL_NUMS; /* will be update by setdummy function */

    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSensorFeatureCtrl
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
static MM_ERROR_CODE_ENUM GC0310_SERIALSensorFeatureCtrl(kal_uint32 Id, void *In, void *Out, kal_uint32 OutLen, kal_uint32 *RealOutLen)
{
    MM_ERROR_CODE_ENUM ErrCode = MM_ERROR_NONE;

    switch (Id)
    {
    /* query and set series */
    case CAL_FEATURE_CAMERA_BRIGHTNESS:
    case CAL_FEATURE_CAMERA_EV_VALUE:
        ErrCode = GC0310_SERIALEv(In, Out);
        break;
    case CAL_FEATURE_CAMERA_CONTRAST:
        ErrCode = GC0310_SERIALContrast(In, Out);
        break;
    case CAL_FEATURE_CAMERA_SATURATION:
        ErrCode = GC0310_SERIALSaturation(In, Out);
        break;
    case CAL_FEATURE_CAMERA_SHARPNESS:
        ErrCode = GC0310_SERIALSharpness(In, Out);
        break;
    case CAL_FEATURE_CAMERA_WB:
        ErrCode = GC0310_SERIALWb(In, Out);
        break;
    case CAL_FEATURE_CAMERA_IMAGE_EFFECT:
        ErrCode = GC0310_SERIALEffect(In, Out);
        break;
    case CAL_FEATURE_CAMERA_STILL_CAPTURE_SIZE:
        ErrCode = GC0310_SERIALStillCaptureSize(In, Out);
        break;
    case CAL_FEATURE_CAMERA_BANDING_FREQ:
        ErrCode = GC0310_SERIALBanding(In, Out);
        break;
    case CAL_FEATURE_CAMERA_SCENE_MODE:
        ErrCode = GC0310_SERIALSceneMode(KAL_TRUE, In, Out);
        break;
    case CAL_FEATURE_CAMERA_VIDEO_SCENE_MODE:
        ErrCode = GC0310_SERIALSceneMode(KAL_FALSE, In, Out);
        break;
    case CAL_FEATURE_CAMERA_FLASH_MODE:
    case CAL_FEATURE_CAMERA_HUE:
    case CAL_FEATURE_CAMERA_GAMMA:
        ErrCode = MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
        break;

    /* get info series */
    case IMAGE_SENSOR_FEATURE_GET_SENSOR_INFO:
        GC0310_SERIALGetSensorInfo(Out);
        break;
    case IMAGE_SENSOR_FEATURE_GET_SENSOR_ID:
        ((P_IMAGE_SENSOR_COMM_DATA_STRUCT)Out)->FeatureValue = GC0310_SERIALDetectSensorId();
        break;
    case IMAGE_SENSOR_FEATURE_GET_SENSOR_FRAME_RATE:
        ((P_IMAGE_SENSOR_COMM_DATA_STRUCT)Out)->FeatureValue = \
        GC0310_SERIALSensor.InternalClock * GC0310_SERIAL_FPS(1) / (GC0310_SERIALSensor.LineLength * GC0310_SERIALSensor.FrameLength);
        break;
    case IMAGE_SENSOR_FEATURE_GET_INIT_OPERATION_PARA:
        GC0310_SERIALInitOperationPara(Out);
        break;
    case IMAGE_SENSOR_FEATURE_GET_HW_DEPENDENT_SETTING:
        CamGetHWInfo(GC0310_SERIALSensor.SensorIdx, Out);
        break;

    /* set para series */
    case IMAGE_SENSOR_FEATURE_SET_SENSOR_POWER_DOWN:
        GC0310_SERIALPowerDown();
        break;
    case IMAGE_SENSOR_FEATURE_SET_NVRAM_SENSOR_INFO:
        //GC0310_SERIALSensor.NvramData = &(((nvram_camera_para_struct *)In)->SENSOR);
        break;
    case IMAGE_SENSOR_FEATURE_SET_CAMERA_SOURCE:
        GC0310_SERIALSensor.SensorIdx = ((P_IMAGE_SENSOR_SET_CAMERA_SOURCE_STRUCT)In)->ImageSensorIdx;
        break;
    case IMAGE_SENSOR_FEATURE_CTRL_NIGHTMODE: /* move to preview */
        break;
    case IMAGE_SENSOR_FEATURE_SET_AE_BYPASS:
        GC0310_SERIALSensor.BypassAe = (kal_bool)((P_IMAGE_SENSOR_COMM_DATA_STRUCT)In)->FeatureValue;
        GC0310_SERIALAeEnable(GC0310_SERIALSensor.BypassAe ? KAL_FALSE : KAL_TRUE);
        break;
    case IMAGE_SENSOR_FEATURE_SET_AWB_BYPASS:
        GC0310_SERIALSensor.BypassAwb = (kal_bool)((P_IMAGE_SENSOR_COMM_DATA_STRUCT)In)->FeatureValue;
        GC0310_SERIALAwbEnable(GC0310_SERIALSensor.BypassAwb ? KAL_FALSE : KAL_TRUE);
        break;
    case IMAGE_SENSOR_FEATURE_GET_SERIAL_SENSOR_INFO:
	 GC0310_SERIALGetSerialSensorInfo(In, Out);
    break;
    default:
        ErrCode = MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
        break;
    }
    return ErrCode;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSensorCtrl
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
static MM_ERROR_CODE_ENUM GC0310_SERIALSensorCtrl(CAL_SCENARIO_ENUM Id, void *In, void *Out, kal_uint32 OutLen, kal_uint32 *RealOutLen)
{
    switch (Id)
    {
    case CAL_SCENARIO_CAMERA_PREVIEW:
    case CAL_SCENARIO_VIDEO:
    case CAL_SCENARIO_WEBCAM_PREVIEW:
    case CAL_SCENARIO_WEBCAM_CAPTURE:
        GC0310_SERIAL_TRACE("%s:%d\r\n", __FUNCTION__, __LINE__);
        GC0310_SERIALPreview(Id, In, Out);
        break;
    case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        GC0310_SERIALCapture(Id, In, Out);
        break;
    default:
        break;
    }
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSensorClose
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
static MM_ERROR_CODE_ENUM GC0310_SERIALSensorClose(void)
{
    GC0310_SERIALPowerDown();
    GC0310_SERIALSensor.sccb_opened = KAL_FALSE; // lanking 20120423
    CisModulePowerOn(GC0310_SERIALSensor.SensorIdx, KAL_FALSE);
    CameraSccbClose(CAMERA_SCCB_SENSOR);

    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   GC0310_SERIALSensorFunc
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
MM_ERROR_CODE_ENUM GC0310_SERIALSensorFunc(P_IMAGE_SENSOR_FUNCTION_STRUCT *pfSensorFunc)
{
    static IMAGE_SENSOR_FUNCTION_STRUCT ImageSensorFuncGC0310_SERIAL =
    {
        GC0310_SERIALSensorOpen,
        GC0310_SERIALSensorFeatureCtrl,
        GC0310_SERIALSensorCtrl,
        GC0310_SERIALSensorClose,
    };

    *pfSensorFunc = &ImageSensorFuncGC0310_SERIAL;
  
    return MM_ERROR_NONE;
}

#ifdef __GC0310_SERIAL_DEBUG_TRACE__
static kal_bool GC0310_SERIALAtGetValue(char *Str, kal_uint32 *Data)
{
    extern int isdigit(int c);
    extern int isxdigit(int c);
    extern int tolower(int c);
    char CmdName[15];
    kal_uint8 TmpIdx = 0, Index = 3;
    kal_uint32 Value;

    while (Str[Index] != '=' && Str[Index] != 13)
    {
        CmdName[TmpIdx++] = Str[Index++];
    }
    CmdName[TmpIdx] = '\0';

    if (strcmp(CmdName, "DO"))
    {
        return KAL_FALSE;
    }
    Value = Data[0] = 0;
    while (1)
    {
        TmpIdx = Str[++Index];
        switch (TmpIdx)
        {
        case 13:
        case ',':
            Data[++Data[0]] = Value;
            if (Data[0] == 9 || TmpIdx == 13)
            {
                return KAL_TRUE;
            }
            Value = 0;
            break;
        default:
            if (isdigit(TmpIdx))
            {
                Value = Value * 16 + TmpIdx - '0';
            }
            else if (isxdigit(TmpIdx))
            {
                Value = Value * 16 + tolower(TmpIdx) - 'a' + 10;
            }
            else
            {
                return KAL_TRUE;
            }
            break;
        }
    }
}

kal_bool AtFuncTest(kal_char *Str)
{
    kal_uint32 Data[10];

    if (!GC0310_SERIALAtGetValue(Str, Data))
    {
        return KAL_FALSE;
    }
    if (!Data[0])
    {
        return KAL_TRUE;
    }
    switch (Data[1])
    {
    case 0x00:
        {
            extern kal_uint32 IspCurrentFrameRate;

            GC0310_SERIAL_TRACE("Current Frame Rate: %d.%d fps", IspCurrentFrameRate / 10, IspCurrentFrameRate % 10);
        }
        break;
    case 0x02:
        GC0310_SERIAL_TRACE("Read BB Reg[%x] = %x", Data[2], DRV_Reg32(Data[2]));
        break;
    case 0x03:
        DRV_Reg32(Data[2]) = Data[3];
        GC0310_SERIAL_TRACE("Write BB Reg[%x] = %x", Data[2], DRV_Reg32(Data[2]));
        break;
    case 0x04:
        GC0310_SERIAL_TRACE("Read Sensor Reg[%x] = %x", Data[2], CamReadCmosSensor(Data[2]));
        break;
    case 0x05:
        CamWriteCmosSensor(Data[2], Data[3]);
        GC0310_SERIAL_TRACE("Write Sensor Reg[%x] = %x", Data[2], CamReadCmosSensor(Data[2]));
        break;
    case 0x06:
        CamWriteCmosSensor(Data[2], Data[3]);
        GC0310_SERIAL_TRACE("Read Sensor Reg[%x:%x] = %x", Data[3], Data[4], CamReadCmosSensor(Data[4]));
        break;
    case 0x07:
        CamWriteCmosSensor(Data[2], Data[3]);
        CamWriteCmosSensor(Data[4], Data[5]);
        CamWriteCmosSensor(Data[2], Data[3]);
        GC0310_SERIAL_TRACE("Write Sensor Reg[%x:%x] = %x", Data[3], Data[4], CamReadCmosSensor(Data[4]));
        break;
    case 0x09:
        //Data[4] = t[Data[2]];
        //t[Data[2]] = Data[3];
        //kal_print_string_trace(MOD_ENG, TRACE_INFO, "t[%d]: %d -> %d", Data[2], Data[4], t[Data[2]]);
        break;
    case 0x0A:
        {
            kal_uint32 i;

            for (i = 0; i < Data[3]; i++)
            {
                GC0310_SERIAL_TRACE("Read Sensor Reg[%x] = %x", Data[2] + i, CamReadCmosSensor(Data[2] + i));
                if (!(i % 200))
                {
                    kal_sleep_task(30);
                }
            }
        }
        break;
    case 0x0B:
        {
            kal_uint32 i;

            for (i = 0; i < Data[3]; i++)
            {
                GC0310_SERIAL_TRACE("Read BB Reg[%x] = %x", Data[2] + i * 4, DRV_Reg32(Data[2] + i * 4));
                if (!(i % 200))
                {
                    kal_sleep_task(30);
                }
            }
        }
        break;
    default:
        break;
    }
    return KAL_TRUE;
}
#endif

