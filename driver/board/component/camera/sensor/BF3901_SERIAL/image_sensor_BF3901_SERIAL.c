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
	kal_uint8 BandingFreq; /* BF3901_SERIAL_50HZ or BF3901_SERIAL_60HZ for 50Hz/60Hz */
	kal_uint32 InternalClock; /* internal clock which using process pixel(for exposure) */
	kal_uint32 Pclk; /* output clock which output to baseband */
	kal_uint32 Gain; /* base on 0x40 */
	kal_uint32 Shutter; /* unit is (linelength / internal clock) s */
	kal_uint32 FrameLength; /* total line numbers in one frame(include dummy line) */
	kal_uint32 LineLength; /* total pixel numbers in one line(include dummy pixel) */
	IMAGE_SENSOR_INDEX_ENUM SensorIdx;
//	sensor_data_struct *NvramData;
	kal_bool sccb_opened;			/* The I2C can work when sccb_opened is KAL_TRUE */
} BF3901_SERIALSensor;

#ifdef BF3901_SERIAL_LOAD_FROM_T_FLASH
/*************************************************************************
* FUNCTION
*  BF3901_SERIAL_Initialize_from_T_Flash
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
static kal_uint8 BF3901_SERIAL_Initialize_from_T_Flash()
{
#include <stdlib.h>
#include "med_utility.h"
#include "fs_type.h"

#if BF3901_SERIAL_I2C_ADDR_BITS == CAMERA_SCCB_16BIT
    #define BF3901_SERIAL_REG_SKIP    0x08
#else
    #define BF3901_SERIAL_REG_SKIP    0x06
#endif
#if BF3901_SERIAL_I2C_DATA_BITS == CAMERA_SCCB_16BIT
    #define BF3901_SERIAL_VAL_SKIP    0x08
#else
    #define BF3901_SERIAL_VAL_SKIP    0x06
#endif
  
    /* 0 - Initial value, 1 - Register, 2 - Delay, 3 - End of setting. */
    #define BF3901_SERIAL_OP_CODE_INI    0x00    /* Initial value. */
    #define BF3901_SERIAL_OP_CODE_REG    0x01    /* Register */
    #define BF3901_SERIAL_OP_CODE_DLY    0x02    /* Delay */
    #define BF3901_SERIAL_OP_CODE_END    0x03    /* End of initial setting. */

    typedef struct
    {
        kal_uint16 init_reg;
        kal_uint16 init_val;  /* Save the register value and delay tick */
        kal_uint8 op_code;    /* 0 - Initial value, 1 - Register, 2 - Delay, 3 - End of setting. */
    } BF3901_SERIAL_initial_set_struct;
  
    static BF3901_SERIAL_initial_set_struct BF3901_SERIAL_Init_Reg[1000];
    static WCHAR BF3901_SERIAL_set_file_name[256] = {0};
    FS_HANDLE fp = -1;        /* Default, no file opened. */
    kal_uint8 *data_buff = NULL;
    kal_uint8 *curr_ptr = NULL;
    kal_uint32 file_size = 0;
    kal_uint32 bytes_read = 0;
    kal_uint32 i = 0, j = 0;
    kal_uint8 func_ind[3] = {0};  /* REG or DLY */
    
    kal_mem_cpy(BF3901_SERIAL_set_file_name, L"C:\\BF3901_SERIAL_Initialize_Setting.Bin", sizeof(L"C:\\BF3901_SERIAL_Initialize_Setting.Bin"));
    
    /* Search the setting file in all of the user disk. */
    curr_ptr = (kal_uint8 *)BF3901_SERIAL_set_file_name;
    while (fp < 0)
    {
        if ((*curr_ptr >= 'c' && *curr_ptr <= 'z') || (*curr_ptr >= 'C' && *curr_ptr <= 'Z'))
        {
            fp = FS_Open(BF3901_SERIAL_set_file_name, FS_READ_ONLY);
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
        BF3901_SERIAL_TRACE("!!! Warning, Can't find the initial setting file!!!");
        return 0;
    }
    
    FS_GetFileSize(fp, &file_size);
    if (file_size < 20)
    {
        BF3901_SERIAL_TRACE("!!! Warning, Invalid setting file!!!");
        return 0;      /* Invalid setting file. */
    }
    
    data_buff = med_alloc_ext_mem(file_size);
    if (data_buff == NULL)
    {
        BF3901_SERIAL_TRACE("!!! Warning, Memory not enoughed...");
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
            BF3901_SERIAL_Init_Reg[i].op_code = BF3901_SERIAL_OP_CODE_REG;
      
            BF3901_SERIAL_Init_Reg[i].init_reg = strtol((const char *)curr_ptr, NULL, 16);
            curr_ptr += BF3901_SERIAL_REG_SKIP;  /* Skip "0x0000, " */
      
            BF3901_SERIAL_Init_Reg[i].init_val = strtol((const char *)curr_ptr, NULL, 16);
            curr_ptr += BF3901_SERIAL_VAL_SKIP;  /* Skip "0x0000);" */
        }
        else                  /* DLY */
        {
            /* Need add delay for this setting. */
            BF3901_SERIAL_Init_Reg[i].op_code = BF3901_SERIAL_OP_CODE_DLY;
      
            BF3901_SERIAL_Init_Reg[i].init_reg = 0xFF;
            BF3901_SERIAL_Init_Reg[i].init_val = strtol((const char *)curr_ptr, NULL, 10);  /* Get the delay ticks, the delay should less then 50 */
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
    BF3901_SERIAL_Init_Reg[i].op_code = BF3901_SERIAL_OP_CODE_END;
    BF3901_SERIAL_Init_Reg[i].init_reg = 0xFF;
    BF3901_SERIAL_Init_Reg[i].init_val = 0xFF;
    i++;

    BF3901_SERIAL_TRACE("%d register read...", i - 1);
    med_free_ext_mem((void **)&data_buff);
    FS_Close(fp);

    BF3901_SERIAL_TRACE("Start apply initialize setting.");
    /* Start apply the initial setting to sensor. */
    for (j=0; j<i; j++)
    {
        if (BF3901_SERIAL_Init_Reg[j].op_code == BF3901_SERIAL_OP_CODE_END)  /* End of the setting. */
        {
            break ;
        }
        else if (BF3901_SERIAL_Init_Reg[j].op_code == BF3901_SERIAL_OP_CODE_DLY)
        {
            vTaskDelay(BF3901_SERIAL_Init_Reg[j].init_val);    /* Delay */
        }
        else if (BF3901_SERIAL_Init_Reg[j].op_code == BF3901_SERIAL_OP_CODE_REG)
        {
            CamWriteCmosSensor(BF3901_SERIAL_Init_Reg[j].init_reg, BF3901_SERIAL_Init_Reg[j].init_val);
        }
        else
        {
            configASSERT(0);
        }
    }
    BF3901_SERIAL_TRACE("%d register applied...", j);
    return 1;  
}
#endif

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSetMirror
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
static void BF3901_SERIALSetMirror(kal_uint8 Mirror)
{
	// TODO: set mirror&flip here
	switch (Mirror)
	{
	case IMAGE_SENSOR_MIRROR_H:
		CamWriteCmosSensor(0x1e,0x29); 
		break;
	case IMAGE_SENSOR_MIRROR_V:
		CamWriteCmosSensor(0x1e,0x19); 
		break;
	case IMAGE_SENSOR_MIRROR_HV:
		CamWriteCmosSensor(0x1e,0x39); 
		break;
	default:
		CamWriteCmosSensor(0x1e,0x09);
	break;
	}
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSetClock
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
static void BF3901_SERIALSetClock(kal_uint32 InternalClock)
{
    if (BF3901_SERIALSensor.InternalClock == InternalClock)
    {
        return;
    }
    BF3901_SERIALSensor.InternalClock = InternalClock;
    // TODO: set internal clock(use process pixel) and output clock(ouput to baseband) here
    {
    	 kal_uint8 ClkDiv;
        /* add PLL comment here */
        switch (InternalClock)
        {
		case BF3901_SERIAL_MCLK / 2:
			ClkDiv = 0x00; 
			break; /* pclk = mclk */
		case BF3901_SERIAL_MCLK / 4: 
			ClkDiv = 0x11; 
			break; /* pclk = mclk / 2 */
		case BF3901_SERIAL_MCLK / 8: 
			ClkDiv = 0x32; 
			break; /* pclk = mclk / 4 */
		default: configASSERT(0);
        }
	 CamWriteCmosSensor(0xfa, ClkDiv);
    }
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALWriteShutter
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
static void BF3901_SERIALWriteShutter(kal_uint32 Shutter)
{
    // TODO: set integration time here
	if (Shutter < 1)
	{
		Shutter = 1;
	}
	
	/* Max exporsure time is 1 frmae period event if Tex is set longer than 1 frame period */
	CamWriteCmosSensor(0x8c, (Shutter & 0xFF00) >> 8);	//AEC[8:15]
	CamWriteCmosSensor(0x8D, Shutter & 0xFF);			//AEC[7:0]
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALReadShutter
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
static kal_uint32 BF3901_SERIALReadShutter(void)
{
    // TODO: read integration time here
	kal_uint8 temp_reg1, temp_reg2;
	kal_uint16 shutter;
	
	temp_reg1 = CamReadCmosSensor(0x8c);
	temp_reg2 = CamReadCmosSensor(0x8d);
	
	shutter = ((temp_reg1&0xff)<<8)|(temp_reg2);
	
	return shutter;    
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALWriteGain
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
static void BF3901_SERIALWriteGain(kal_uint32 Gain)
{
    // TODO: set global gain here
    
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALReadGain
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
static kal_uint32 BF3901_SERIALReadGain(void)
{
    // TODO: read global gain here
    
    return 0x40;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALAeEnable
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
static void BF3901_SERIALAeEnable(kal_bool Enable)
{
	kal_uint16 AeTemp = 0;
	if (BF3901_SERIALSensor.BypassAe)
	{
		Enable = KAL_FALSE;
	}
	AeTemp = CamReadCmosSensor(0x13);
	// TODO: enable or disable AE here
	{
		if (Enable)
		{
			CamWriteCmosSensor(0x13, (AeTemp| 0x01)); 
		}
		else
		{
			CamWriteCmosSensor(0x13, (AeTemp&(~0x01))); 
		}
	}
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALAwbEnable
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
static void BF3901_SERIALAwbEnable(kal_bool Enable)
{
	kal_uint16 AwbTemp = 0;
	if (BF3901_SERIALSensor.BypassAwb)
	{
		Enable = KAL_FALSE;
	}
	AwbTemp = CamReadCmosSensor(0x13);
	// TODO: enable or disable AWB here
	{
		if (Enable)
		{
			CamWriteCmosSensor(0x13, (AwbTemp | 0x02));
		}
		else
		{
			CamWriteCmosSensor(0x13, (AwbTemp & ~0x02)); 
		}
	}
}

/*************************************************************************
* FUNCTION
*    BF3901_SERIALSetBandingStep
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
static void BF3901_SERIALSetBandingStep(void)
{
    // TODO: set banding step here
    
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSetDummy
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
static void BF3901_SERIALSetDummy(kal_uint32 DummyPixel, kal_uint32 DummyLine)
{
	CamWriteCmosSensor(0x2a, ((DummyPixel&0x700)>>4));
	CamWriteCmosSensor(0x2b, (DummyPixel&0xFF));
	CamWriteCmosSensor(0x92, (DummyLine&0xFF));
	CamWriteCmosSensor(0x93, ((DummyLine&0xFF00)>>8));
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSetMinFps
*
* DESCRIPTION
*   This function calculate & set min frame rate
*
* PARAMETERS
*   Fps: min frame rate, base on BF3901_SERIAL_FPS(x)
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void BF3901_SERIALSetMinFps(kal_uint16 Fps)
{

	kal_uint16 Int_Max;    
	kal_uint16 temp_reg;	

	temp_reg = CamReadCmosSensor(0x89);

	Int_Max = (kal_uint16) (BF3901_SERIALSensor.BandingFreq * BF3901_SERIAL_FPS(1) / Fps); 

	CamWriteCmosSensor(0x89, ((Int_Max<<3)|(temp_reg&0x07)));

}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSetMaxFps
*
* DESCRIPTION
*   This function calculate & set max frame rate
*
* PARAMETERS
*   Fps: max frame rate, base on BF3901_SERIAL_FPS(x)
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void BF3901_SERIALSetMaxFps(kal_uint16 Fps)
{

	kal_uint32 FrameLength,lines;

	/* get max line FrameLength */
	FrameLength = BF3901_SERIALSensor.InternalClock * BF3901_SERIAL_FPS(1) / (Fps * BF3901_SERIAL_PV_PERIOD_PIXEL_NUMS);

	if (FrameLength > BF3901_SERIAL_PV_PERIOD_LINE_NUMS) /* overflow check */
	{
		lines = FrameLength - BF3901_SERIAL_PV_PERIOD_LINE_NUMS;
	}
	else
	{
		lines = 0;
	}

	/* limit max frame rate to Fps specified */
	CamWriteCmosSensor(0x93, ((lines&0xFF00)>>8));
	CamWriteCmosSensor(0x92, (lines&0xFF));

}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSetVideoFps
*
* DESCRIPTION
*   This function calculate & fix frame rate in video mode
*
* PARAMETERS
*   Fps: target frame rate to fixed, base on BF3901_SERIAL_FPS(x)
*
* RETURNS
*   None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void BF3901_SERIALSetVideoFps(kal_uint16 Fps)
{
    /* limit max frame rate */
    BF3901_SERIALSetMaxFps(Fps);
    BF3901_SERIALSetMinFps(Fps);
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALNightMode
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
static void BF3901_SERIALNightMode(kal_bool Enable)
{
	// TODO: set night mode here
	if (Enable) 
	{
		CamWriteCmosSensor(0x86, 0xa0);
	}
	else 
	{
		CamWriteCmosSensor(0x86, 0x77);
	} 
	
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALEv
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
static MM_ERROR_CODE_ENUM BF3901_SERIALEv(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
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
		switch (In->FeatureSetValue)
		{
		case CAM_EV_NEG_4_3:							/* EV -2 */
			CamWriteCmosSensor(0x55, 0xF8);		
			break;
		case CAM_EV_NEG_3_3:							/* EV -1.5 */
			CamWriteCmosSensor(0x55, 0xD8);
			break;
		case CAM_EV_NEG_2_3:							/* EV -1 */
			CamWriteCmosSensor(0x55, 0xB8);
			break;
		case CAM_EV_NEG_1_3:							/* EV -0.5 */
			CamWriteCmosSensor(0x55, 0x98);
			break;
		case CAM_EV_ZERO:								/* EV 0 */
			CamWriteCmosSensor(0x55, 0x00);
			break;
		case CAM_EV_POS_1_3:							/* EV +0.5 */
			CamWriteCmosSensor(0x55, 0x18);
			break;
		case CAM_EV_POS_2_3:							/* EV +1 */
			CamWriteCmosSensor(0x55, 0x38);
			break;
		case CAM_EV_POS_3_3:							/* EV +1.5 */
			CamWriteCmosSensor(0x55, 0x58);
			break;
		case CAM_EV_POS_4_3:							/* EV +2 */
			CamWriteCmosSensor(0x55, 0x78);
			break;
		default:
			return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
		}
	}
	return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALContrast
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
static MM_ERROR_CODE_ENUM BF3901_SERIALContrast(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
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
        switch (In->FeatureSetValue)
        {
        case CAM_CONTRAST_HIGH:
            /* add setting here */
            break;
        case CAM_CONTRAST_MEDIUM:
            /* add setting here */
            break;
        case CAM_CONTRAST_LOW:
            /* add setting here */
            break;
        default:
            return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
        }
    }
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSaturation
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
static MM_ERROR_CODE_ENUM BF3901_SERIALSaturation(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
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
*   BF3901_SERIALSharpness
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
static MM_ERROR_CODE_ENUM BF3901_SERIALSharpness(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
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
*   BF3901_SERIALWb
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
static MM_ERROR_CODE_ENUM BF3901_SERIALWb(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
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
			BF3901_SERIALAwbEnable(KAL_TRUE);
			break;
		case CAM_WB_CLOUD:
			BF3901_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x01, 0x10);
			CamWriteCmosSensor(0x02, 0x28);
			break;
		case CAM_WB_DAYLIGHT:
			BF3901_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x01, 0x13);
			CamWriteCmosSensor(0x02, 0x26);
			break;
		case CAM_WB_INCANDESCENCE:
			BF3901_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x01, 0x1f);
			CamWriteCmosSensor(0x02, 0x15);
			break;
		case CAM_WB_FLUORESCENT:
			BF3901_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x01, 0x1a);
			CamWriteCmosSensor(0x02, 0x1e);
			break;
		case CAM_WB_TUNGSTEN:
			BF3901_SERIALAwbEnable(KAL_FALSE);
			CamWriteCmosSensor(0x01, 0x1a);
			CamWriteCmosSensor(0x02, 0x0d);
			break;
		default:
			return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
		}
	}
	return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALEffect
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
static MM_ERROR_CODE_ENUM BF3901_SERIALEffect(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
	if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
	{
		const P_CAL_FEATURE_TYPE_ENUM_STRUCT Effect = &Out->FeatureInfo.FeatureEnum;

		Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
		Out->FeatureOperationSupport = CAL_FEATURE_SET_OPERATION|CAL_FEATURE_QUERY_OPERATION;
		Out->FeatureModeSupport = CAL_FEATURE_CAMERA_VIDEO_SUPPORT;
		// TODO: add supported effect enum here
		Effect->IsSupport = KAL_TRUE;
		Effect->ItemCount = 14;
		Effect->SupportItem[0] = CAM_EFFECT_ENC_NORMAL;
		Effect->SupportItem[1] = CAM_EFFECT_ENC_GRAYSCALE;
		Effect->SupportItem[2] = CAM_EFFECT_ENC_SEPIA;
		Effect->SupportItem[3] = CAM_EFFECT_ENC_SEPIAGREEN;
		Effect->SupportItem[4] = CAM_EFFECT_ENC_SEPIABLUE;
		Effect->SupportItem[5] = CAM_EFFECT_ENC_COLORINV;
		Effect->SupportItem[6] = CAM_EFFECT_ENC_GRAYINV;
		Effect->SupportItem[7] = CAM_EFFECT_ENC_BLACKBOARD;
		Effect->SupportItem[8] = CAM_EFFECT_ENC_WHITEBOARD;
		Effect->SupportItem[9] = CAM_EFFECT_ENC_COPPERCARVING;
		Effect->SupportItem[10] = CAM_EFFECT_ENC_EMBOSSMENT;
		Effect->SupportItem[11] = CAM_EFFECT_ENC_BLUECARVING;
		Effect->SupportItem[12] = CAM_EFFECT_ENC_CONTRAST;
		Effect->SupportItem[13] = CAM_EFFECT_ENC_SKETCH;
	}
	else if (CAL_FEATURE_SET_OPERATION == In->FeatureCtrlCode)
	{
		// TODO: adjust effect here
		switch (In->FeatureSetValue)
		{
		case CAM_EFFECT_ENC_NORMAL:
			CamWriteCmosSensor(0x70, 0x0f);		
			CamWriteCmosSensor(0x69, 0x00);
			CamWriteCmosSensor(0x67, 0x80);		
			CamWriteCmosSensor(0x68, 0x80);	
			CamWriteCmosSensor(0xb4, 0x60);	
			CamWriteCmosSensor(0x98, 0x1a);		
			CamWriteCmosSensor(0x56, 0x40);	
			break;
		case CAM_EFFECT_ENC_GRAYSCALE:
			CamWriteCmosSensor(0x70, 0x0f);		
			CamWriteCmosSensor(0x69, 0x20);
			CamWriteCmosSensor(0x67, 0x80);		
			CamWriteCmosSensor(0x68, 0x80);	
			CamWriteCmosSensor(0xb4, 0x60);	
			CamWriteCmosSensor(0x98, 0x0a);		
			CamWriteCmosSensor(0x56, 0x40);	
			break;
		case CAM_EFFECT_ENC_SEPIA:
			CamWriteCmosSensor(0x70, 0x0f);		
			CamWriteCmosSensor(0x69, 0x20);
			CamWriteCmosSensor(0x67, 0x60);		
			CamWriteCmosSensor(0x68, 0xa0);	
			CamWriteCmosSensor(0xb4, 0x60);	
			CamWriteCmosSensor(0x98, 0x0a);		
			CamWriteCmosSensor(0x56, 0x40);	
			break;
		case CAM_EFFECT_ENC_SEPIAGREEN:
			CamWriteCmosSensor(0x70, 0x0f);		
			CamWriteCmosSensor(0x69, 0x20);
			CamWriteCmosSensor(0x67, 0x60);		
			CamWriteCmosSensor(0x68, 0x70);	
			CamWriteCmosSensor(0xb4, 0x60);	
			CamWriteCmosSensor(0x98, 0x0a);		
			CamWriteCmosSensor(0x56, 0x40);	
			break;
		case CAM_EFFECT_ENC_SEPIABLUE:
			CamWriteCmosSensor(0x70, 0x0f);		
			CamWriteCmosSensor(0x69, 0x20);
			CamWriteCmosSensor(0x67, 0xe0);		
			CamWriteCmosSensor(0x68, 0x60);	
			CamWriteCmosSensor(0xb4, 0x60);	
			CamWriteCmosSensor(0x98, 0x0a);		
			CamWriteCmosSensor(0x56, 0x40);	
			break;
		case CAM_EFFECT_ENC_COLORINV:
			CamWriteCmosSensor(0x70, 0x0f);		
			CamWriteCmosSensor(0x69, 0x01);
			CamWriteCmosSensor(0x67, 0x80);		
			CamWriteCmosSensor(0x68, 0x80);	
			CamWriteCmosSensor(0xb4, 0x60);	
			CamWriteCmosSensor(0x98, 0x0a);		
			CamWriteCmosSensor(0x56, 0x40);
			break;
		case CAM_EFFECT_ENC_GRAYINV:
			CamWriteCmosSensor(0x70, 0x0f); 	
			CamWriteCmosSensor(0x69, 0x21);
			CamWriteCmosSensor(0x67, 0x80); 	
			CamWriteCmosSensor(0x68, 0x80); 
			CamWriteCmosSensor(0xb4, 0x60); 
			CamWriteCmosSensor(0x98, 0x0a); 	
			CamWriteCmosSensor(0x56, 0x40); 
			break;
		case CAM_EFFECT_ENC_BLACKBOARD:
			CamWriteCmosSensor(0x70, 0x6f); 	
			CamWriteCmosSensor(0x69, 0x00);
			CamWriteCmosSensor(0x67, 0x80); 	
			CamWriteCmosSensor(0x68, 0x80); 
			CamWriteCmosSensor(0xb4, 0x60); 
			CamWriteCmosSensor(0x98, 0x8a); 	
			CamWriteCmosSensor(0x56, 0x40); 
			break;
		case CAM_EFFECT_ENC_WHITEBOARD:
			CamWriteCmosSensor(0x70, 0x7f); 	
			CamWriteCmosSensor(0x69, 0x00);
			CamWriteCmosSensor(0x67, 0x80); 	
			CamWriteCmosSensor(0x68, 0x80); 
			CamWriteCmosSensor(0xb4, 0x60); 
			CamWriteCmosSensor(0x98, 0x8a); 	
			CamWriteCmosSensor(0x56, 0x40); 
			break;
		case CAM_EFFECT_ENC_COPPERCARVING:
			CamWriteCmosSensor(0x70, 0x4f); 	
			CamWriteCmosSensor(0x69, 0x00);
			CamWriteCmosSensor(0x67, 0x80); 	
			CamWriteCmosSensor(0x68, 0x80); 
			CamWriteCmosSensor(0xb4, 0x00); 
			CamWriteCmosSensor(0x98, 0x8a); 	
			CamWriteCmosSensor(0x56, 0x40); 
			break;
		case CAM_EFFECT_ENC_EMBOSSMENT:
			CamWriteCmosSensor(0x70, 0x1f);//0x2f	
			CamWriteCmosSensor(0x69, 0x00);
			CamWriteCmosSensor(0x67, 0x80); 	
			CamWriteCmosSensor(0x68, 0x80); 
			CamWriteCmosSensor(0xb4, 0x60); 
			CamWriteCmosSensor(0x98, 0x8a); 	
			CamWriteCmosSensor(0x56, 0x40); 
			break;
		case CAM_EFFECT_ENC_BLUECARVING:
			CamWriteCmosSensor(0x70, 0x5f); 	
			CamWriteCmosSensor(0x69, 0x00);
			CamWriteCmosSensor(0x67, 0x80); 	
			CamWriteCmosSensor(0x68, 0x80); 
			CamWriteCmosSensor(0xb4, 0x00); 
			CamWriteCmosSensor(0x98, 0x8a); 	
			CamWriteCmosSensor(0x56, 0x40);
			break;
		case CAM_EFFECT_ENC_CONTRAST:
			CamWriteCmosSensor(0x70, 0x0f); 	
			CamWriteCmosSensor(0x69, 0x20);
			CamWriteCmosSensor(0x67, 0x80); 	
			CamWriteCmosSensor(0x68, 0x80); 
			CamWriteCmosSensor(0xb4, 0x60); 
			CamWriteCmosSensor(0x98, 0x0a); 	
			CamWriteCmosSensor(0x56, 0x80); 
			break;
		case CAM_EFFECT_ENC_SKETCH:
			CamWriteCmosSensor(0x70, 0x3f); 	
			CamWriteCmosSensor(0x69, 0x00);
			CamWriteCmosSensor(0x67, 0x80); 	
			CamWriteCmosSensor(0x68, 0x80); 
			CamWriteCmosSensor(0xb4, 0x60); 
			CamWriteCmosSensor(0x98, 0x8a); 	
			CamWriteCmosSensor(0x56, 0x40);
			break;
		default:
			return MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
		}
	}
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALStillCaptureSize
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
MM_ERROR_CODE_ENUM BF3901_SERIALStillCaptureSize(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
	const P_CAL_FEATURE_TYPE_ENUM_STRUCT CapSize = &Out->FeatureInfo.FeatureEnum;

	if (CAL_FEATURE_QUERY_OPERATION == In->FeatureCtrlCode)
	{
		Out->FeatureType = CAL_FEATURE_TYPE_ENUMERATE;
		Out->FeatureOperationSupport = CAL_FEATURE_QUERY_OPERATION;
		Out->FeatureModeSupport = CAL_FEATURE_CAMERA_SUPPORT;
		CapSize->IsSupport = KAL_TRUE;
		// TODO: add supported capture size enum here
		CapSize->ItemCount = 3;
		CapSize->SupportItem[0] = CAM_IMAGE_SIZE_WALLPAPER;
		CapSize->SupportItem[1] = CAM_IMAGE_SIZE_QVGA;
		CapSize->SupportItem[2] = CAM_IMAGE_SIZE_VGA;
	}
	return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALBanding
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
static MM_ERROR_CODE_ENUM BF3901_SERIALBanding(P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
{
	const P_CAL_FEATURE_TYPE_ENUM_STRUCT Banding = &Out->FeatureInfo.FeatureEnum;
	kal_uint8 TempReg = CamReadCmosSensor(0x80);
	
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
		BF3901_SERIALSensor.BandingFreq = (CAM_BANDING_50HZ == In->FeatureSetValue ? BF3901_SERIAL_50HZ : BF3901_SERIAL_60HZ);

		if (CAM_BANDING_50HZ == In->FeatureSetValue)
		{
			CamWriteCmosSensor(0x80, TempReg);
			CamWriteCmosSensor(0x8a, 0x5c);
		}
		else
		{
			CamWriteCmosSensor(0x80, TempReg&0xfd);
			CamWriteCmosSensor(0x8b, 0x4c);
		}
	}
	return MM_ERROR_NONE;
}


/*************************************************************************
* FUNCTION
*   BF3901_SERIALSceneMode
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
static MM_ERROR_CODE_ENUM BF3901_SERIALSceneMode(kal_bool IsCam, P_CAL_FEATURE_CTRL_STRUCT In, P_CAL_FEATURE_CTRL_STRUCT Out)
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
*	BF3901_SERIALGammaSelect
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
static void BF3901_SERIALGammaSelect(kal_uint32 GammaLvl)
{

}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALInitialSetting
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
static void BF3901_SERIALInitialSetting(void)
{
	//CamWriteCmosSensor(0x12, 0x80);//bit[7]:1-soft reset

	//===== drive capability======
	CamWriteCmosSensor(0x20, 0x09);//bit[6]:0-same drive capability 1-drive capability independently
	CamWriteCmosSensor(0x09, 0x03);//bit[4]:1-soft sleep mode
	
	//======= SPI Setting=======
	CamWriteCmosSensor(0x11, 0x30);//Mclk_div control 0x90-divided by 2;0xb0-divided by 4;0x30:divided by 8;
	CamWriteCmosSensor(0x1b, 0x06);//PLL setting
	CamWriteCmosSensor(0x6b, 0x41);//open ck_gate and ccir656
	CamWriteCmosSensor(0x12, 0x20);//0x20-YUV422, 0x04-RGB565
	CamWriteCmosSensor(0x3a, 0x00);//YUV422 Sequence
	CamWriteCmosSensor(0x62, 0x81);//bit[1:0] 01:output spi mode;10:output 2 bit mode;11:output 4 bit mode
	CamWriteCmosSensor(0x15, 0x02);//Bit[1]: VSYNC option.Bit[0]: HSYNC option.Bit[4]:VCLK reverse
	CamWriteCmosSensor(0x08, 0xa0);//bit[7]:serial mode select for vclk(0:nomal;1:serial mode)

	CamWriteCmosSensor(0xbb, 0x21);//Bit[1:0]:vclk delay select
	CamWriteCmosSensor(0x3e, 0x05);//Bit[2:0]:vddd adjust 
	
	//====== Analog Setting ======
	CamWriteCmosSensor(0x06, 0x68);
	
	//====== Sensor Control ======
	CamWriteCmosSensor(0x2b, 0x20);//Dummy Pixel Insert LSB

	//====== Black Control ======
	CamWriteCmosSensor(0x27, 0x97);
	
	//====== Data Format ======
	CamWriteCmosSensor(0x17, 0x01);
	CamWriteCmosSensor(0x18, 0x79);
	CamWriteCmosSensor(0x19, 0x00);
	CamWriteCmosSensor(0x1a, 0xa0);
	CamWriteCmosSensor(0x03, 0x00);
	
	//====== Initial AWB and AE Setting ======
	CamWriteCmosSensor(0x13, 0x00);//Manu AWB & AE
	CamWriteCmosSensor(0x01, 0x13);
	CamWriteCmosSensor(0x02, 0x20);
	CamWriteCmosSensor(0x87, 0x16);
	CamWriteCmosSensor(0x8c, 0x01);
	CamWriteCmosSensor(0x8d, 0xcc);
	CamWriteCmosSensor(0x13, 0x07);

	//====== Lens Shading =======
	CamWriteCmosSensor(0x33, 0x10);//value of lens shading auto offset( black_aver)
	CamWriteCmosSensor(0x34, 0x1d);//value of lens shading manul offset
	CamWriteCmosSensor(0x35, 0x46);//lens shading gain of R
	CamWriteCmosSensor(0x36, 0x40);
	CamWriteCmosSensor(0x37, 0xa4);//Center Y coordinate LSB for B(vertical center)
	CamWriteCmosSensor(0x38, 0x7c);//Center X coordinate LSB for B(horizontal  center)
	CamWriteCmosSensor(0x65, 0x46);//lens shading gain of G
	CamWriteCmosSensor(0x66, 0x46);//lens shading gain of B
	CamWriteCmosSensor(0x6e, 0x20);//the threshold of GLB_GAIN
	CamWriteCmosSensor(0x9b, 0xa4);//Center Y coordinate LSB for G(vertical center)
	CamWriteCmosSensor(0x9c, 0x7c);//Center X coordinate LSB for G(horizontal  center)
	CamWriteCmosSensor(0xbc, 0x0c);
	CamWriteCmosSensor(0xbd, 0xa4);//Center Y coordinate LSB for R(vertical center)
	CamWriteCmosSensor(0xbe, 0x7c);//Center X coordinate LSB for R(horizontal  center)

	//====== De-noise =======
	CamWriteCmosSensor(0x72, 0x2f);//0x72[7:4]: The bigger, the smaller noise 
	CamWriteCmosSensor(0x73, 0x2f);//0x73[7:4]: The bigger, the smaller noise
	CamWriteCmosSensor(0x74, 0xa7);//0x74[3:0]: The smaller, the smaller noise

	//====== Edge Enhancement =======
	CamWriteCmosSensor(0x75, 0x12);//0x75[6:4]: Bright edge enhancement; 0x75[2:0]: Dark edge enhancement

	//====== De-noise in Low Light =======
	CamWriteCmosSensor(0x79, 0x8d);//0x79[3]: 0: Off, 1: On; 0x79[7:4]: Y_AVER threshold for de-noise
	CamWriteCmosSensor(0x7a, 0x00);//0x7a[7]: 0: Choose luminance of current pixel for de-noise, 1: Choose Y_AVER
	CamWriteCmosSensor(0x7e, 0xfa);//0x7e[7]=1 open lowCtr，[3:0]The smaller, the smaller noise for black substance

	//====== De-noise for Outdoor =======
	CamWriteCmosSensor(0x70, 0x0f);//0x70[7]: 0: Disable De-noise outdoor, 1: Enable De-noise outdoor
	CamWriteCmosSensor(0x7c, 0x84);//0x7c[7:6]: The smaller, the smaller noise
	CamWriteCmosSensor(0x7d, 0xba);//0x7d[7:6]: The bigger, the smaller noise

	//====== Color Fringe Correction =======
	CamWriteCmosSensor(0x5b, 0xc2);
	CamWriteCmosSensor(0x76, 0x90);
	CamWriteCmosSensor(0x7b, 0x55);

	CamWriteCmosSensor(0x71, 0x46);
	CamWriteCmosSensor(0x77, 0xdd);

	//==AE Gain Line,此小段不允许修改 ==
	CamWriteCmosSensor(0x13, 0x0f);//以下为AE gain曲线寄存器
	CamWriteCmosSensor(0x8a, 0x10);
	CamWriteCmosSensor(0x8b, 0x20);
	CamWriteCmosSensor(0x8e, 0x21);
	CamWriteCmosSensor(0x8f, 0x40);
	CamWriteCmosSensor(0x94, 0x41);
	CamWriteCmosSensor(0x95, 0x7e);
	CamWriteCmosSensor(0x96, 0x7f);
	CamWriteCmosSensor(0x97, 0xf3);

	//====== AE setting&Used to Modify Y_AVER  ========
	CamWriteCmosSensor(0x13, 0x07);
	CamWriteCmosSensor(0x24, 0x58);//Bit[6:0]: AE target. 0x51相当于原来的0xa2
	CamWriteCmosSensor(0x97, 0x48);//Y target value1 for F light.          
	CamWriteCmosSensor(0x25, 0x08);//Bit[7:4]: AE_LOC_INT; Bit[3:0]:AE_LOC_GLB
	CamWriteCmosSensor(0x94, 0xb5);//Bit[7:4]: Threshold for over exposure pixels, the smaller, the more over exposure pixels; Bit[3:0]: Control the start of AE.
	CamWriteCmosSensor(0x95, 0xc0);//modify the Y_AVER
	CamWriteCmosSensor(0x80, 0xf6);//Bit[3:2]: the bigger, Y_AVER_MODIFY is smaller
	CamWriteCmosSensor(0x81, 0xe0);//AE speed
	              
	//====== Default Frame ======              
	CamWriteCmosSensor(0x82, 0x1b);//GLB_MIN1:minimum global gain
	CamWriteCmosSensor(0x83, 0x37);//GLB_MAX1
	CamWriteCmosSensor(0x84, 0x39);//GLB_MIN2
	CamWriteCmosSensor(0x85, 0x58);//GLB_MAX2
	
	/*//====== Frame Faster1 ======
	CamWriteCmosSensor(0x82, 0x25);
	CamWriteCmosSensor(0x83, 0x4a);
	CamWriteCmosSensor(0x84, 0x50);
	CamWriteCmosSensor(0x85, 0x5d);
	
	//====== Frame Faster2 ======
	CamWriteCmosSensor(0x82, 0x30);
	CamWriteCmosSensor(0x83, 0x55);
	CamWriteCmosSensor(0x84, 0x50);
	CamWriteCmosSensor(0x85, 0x6c);*/
	
	CamWriteCmosSensor(0x86, 0x77);//GLB_MAX3:maximum global gain
	CamWriteCmosSensor(0x89, 0x74);//INT_MAX_MID//76
	CamWriteCmosSensor(0x8a, 0x5c);//50HZ Banding Filter step
	CamWriteCmosSensor(0x8b, 0x4c);//60HZ Banding Filter step
	CamWriteCmosSensor(0x98, 0x1a);//AE window
	//====== Gamma Offset ======
	CamWriteCmosSensor(0x39, 0x98);
	CamWriteCmosSensor(0x3f, 0x98);

	//Auto Offset in High & Low Light Scene 
	CamWriteCmosSensor(0x90, 0xa0);//Bit[6:0]: Int_time threshold for auto offset in high light scene
	CamWriteCmosSensor(0x91, 0xe0);//Bit[6:0]: Y_aver threshold for auto offset in low light scene//{0x90[7],0x91[7]}=01: offset is small, 1x: offset is bigger than 01, 00: offset is the biggest

	//====== Gamma Setting1 过暴过渡好======
	CamWriteCmosSensor(0x40, 0x20);
	CamWriteCmosSensor(0x41, 0x28);
	CamWriteCmosSensor(0x42, 0x26);
	CamWriteCmosSensor(0x43, 0x25);
	CamWriteCmosSensor(0x44, 0x1f);
	CamWriteCmosSensor(0x45, 0x1a);
	CamWriteCmosSensor(0x46, 0x16);
	CamWriteCmosSensor(0x47, 0x12);
	CamWriteCmosSensor(0x48, 0x0f);
	CamWriteCmosSensor(0x49, 0x0d);
	CamWriteCmosSensor(0x4b, 0x0b);
	CamWriteCmosSensor(0x4c, 0x0a);
	CamWriteCmosSensor(0x4e, 0x08);
	CamWriteCmosSensor(0x4f, 0x06);
	CamWriteCmosSensor(0x50, 0x06);

	/*//====== Gamma Setting2 噪声稍小======
	CamWriteCmosSensor(0x40, 0x1c);
	CamWriteCmosSensor(0x41, 0x27);
	CamWriteCmosSensor(0x42, 0x25);
	CamWriteCmosSensor(0x43, 0x23);
	CamWriteCmosSensor(0x44, 0x20);
	CamWriteCmosSensor(0x45, 0x18);
	CamWriteCmosSensor(0x46, 0x16);
	CamWriteCmosSensor(0x47, 0x13);
	CamWriteCmosSensor(0x48, 0x0f);
	CamWriteCmosSensor(0x49, 0x0c);
	CamWriteCmosSensor(0x4b, 0x0b);
	CamWriteCmosSensor(0x4c, 0x0a);
	CamWriteCmosSensor(0x4e, 0x08);
	CamWriteCmosSensor(0x4f, 0x08);
	CamWriteCmosSensor(0x50, 0x06);
	
	//====== Gamma Setting3 噪声小======
	CamWriteCmosSensor(0x40, 0x18);
	CamWriteCmosSensor(0x41, 0x25);
	CamWriteCmosSensor(0x42, 0x22);
	CamWriteCmosSensor(0x43, 0x1f);
	CamWriteCmosSensor(0x44, 0x1c);
	CamWriteCmosSensor(0x45, 0x1a);
	CamWriteCmosSensor(0x46, 0x17);
	CamWriteCmosSensor(0x47, 0x15);
	CamWriteCmosSensor(0x48, 0x11);
	CamWriteCmosSensor(0x49, 0x0e);
	CamWriteCmosSensor(0x4b, 0x0b);
	CamWriteCmosSensor(0x4c, 0x0a);
	CamWriteCmosSensor(0x4e, 0x09);
	CamWriteCmosSensor(0x4f, 0x08);
	CamWriteCmosSensor(0x50, 0x06);
           
	//====== Gamma Setting4 清晰亮丽======
	CamWriteCmosSensor(0x40, 0x1E);
	CamWriteCmosSensor(0x41, 0x2f);
	CamWriteCmosSensor(0x42, 0x2B);
	CamWriteCmosSensor(0x43, 0x25);
	CamWriteCmosSensor(0x44, 0x21);
	CamWriteCmosSensor(0x45, 0x1d);
	CamWriteCmosSensor(0x46, 0x16);
	CamWriteCmosSensor(0x47, 0x12);
	CamWriteCmosSensor(0x48, 0x0E);
	CamWriteCmosSensor(0x49, 0x0a);
	CamWriteCmosSensor(0x4b, 0x09);
	CamWriteCmosSensor(0x4c, 0x08);
	CamWriteCmosSensor(0x4e, 0x07);
	CamWriteCmosSensor(0x4f, 0x06);
	CamWriteCmosSensor(0x50, 0x06);*/       
           
	//==绿色植物效果好 for Outdoor Scene==
	CamWriteCmosSensor(0x5a, 0x56);
	CamWriteCmosSensor(0x51, 0x12);
	CamWriteCmosSensor(0x52, 0x0d);
	CamWriteCmosSensor(0x53, 0x92);
	CamWriteCmosSensor(0x54, 0x7d);
	CamWriteCmosSensor(0x57, 0x97);
	CamWriteCmosSensor(0x58, 0x43);
	/*
	//== Color Coefficient 1 for Outdoor Scene==
	CamWriteCmosSensor(0x5a, 0x56);
	CamWriteCmosSensor(0x51, 0x1b);
	CamWriteCmosSensor(0x52, 0x04);
	CamWriteCmosSensor(0x53, 0x4a);
	CamWriteCmosSensor(0x54, 0x26);
	CamWriteCmosSensor(0x57, 0x75);
	CamWriteCmosSensor(0x58, 0x2b);
    
	//== Color Coefficient 2 for Outdoor Scene==
	CamWriteCmosSensor(0x5a, 0x56);
	CamWriteCmosSensor(0x51, 0x21);
	CamWriteCmosSensor(0x52, 0x12);
	CamWriteCmosSensor(0x53, 0x52);
	CamWriteCmosSensor(0x54, 0x45);
	CamWriteCmosSensor(0x57, 0x7f);
	CamWriteCmosSensor(0x58, 0x35);	*/
	         
	//== Color Coefficient  for Indoor Scene==20122522
	CamWriteCmosSensor(0x5a, 0xd6);
	CamWriteCmosSensor(0x51, 0x21);
	CamWriteCmosSensor(0x52, 0x1c);
	CamWriteCmosSensor(0x53, 0x9a);
	CamWriteCmosSensor(0x54, 0x6e);
	CamWriteCmosSensor(0x57, 0x5c);
	CamWriteCmosSensor(0x58, 0x14);

	/*//肤色和照其它物体彩色还比较好
	CamWriteCmosSensor(0x5a, 0xd6);
	CamWriteCmosSensor(0x51, 0x1f);
	CamWriteCmosSensor(0x52, 0x0f);
	CamWriteCmosSensor(0x53, 0x47);
	CamWriteCmosSensor(0x54, 0x20);
	CamWriteCmosSensor(0x57, 0x2f);
	CamWriteCmosSensor(0x58, 0x24);

	//肤色好
	CamWriteCmosSensor(0x5a, 0xd6);
	CamWriteCmosSensor(0x51, 0x06);
	CamWriteCmosSensor(0x52, 0x09);
	CamWriteCmosSensor(0x53, 0x29);
	CamWriteCmosSensor(0x54, 0x1d);
	CamWriteCmosSensor(0x57, 0x18);
	CamWriteCmosSensor(0x58, 0x03);

	//色彩艳丽   20120522
	CamWriteCmosSensor(0x5a, 0xd6);
	CamWriteCmosSensor(0x51, 0x20);
	CamWriteCmosSensor(0x52, 0x2b);
	CamWriteCmosSensor(0x53, 0x95);
	CamWriteCmosSensor(0x54, 0x80);
	CamWriteCmosSensor(0x57, 0x77);
	CamWriteCmosSensor(0x58, 0x0d);*/
		
	//===== Color Saturation ======
	CamWriteCmosSensor(0x5c, 0x28);//the smaller, the smaller color noise in low light scene
	CamWriteCmosSensor(0xb0, 0xe0);//Saturation control. When bit[7]=1, the smaller, the smaller color noise in low light scene
	CamWriteCmosSensor(0xb1, 0xc0);//Blue Coefficient
	CamWriteCmosSensor(0xb2, 0xb0);//Red Coefficient
	CamWriteCmosSensor(0xb3, 0x4f);
	CamWriteCmosSensor(0xb4, 0x63);//Bit[6:5]: Gray region De-noise, 00: don't do de-noise; 11: the noise is smaller than 00; 01: depends on 0xb3[3:0], 0xb3[3:0] the smaller, the noise is smaller.

	//=====  A light Color Saturation =====
	CamWriteCmosSensor(0xb4, 0xe3);
	CamWriteCmosSensor(0xb1, 0xf0);
	CamWriteCmosSensor(0xb2, 0xa0);
	//===== Contrast & Brightness ======
	CamWriteCmosSensor(0x55, 0x00);
	CamWriteCmosSensor(0x56, 0x40);
	
	//====== AWB Setting =======
	CamWriteCmosSensor(0x96, 0x50);//outdoor brightness condition
	CamWriteCmosSensor(0x9a, 0x30);//outdoor integration time condition
	CamWriteCmosSensor(0x6a, 0x81);
	CamWriteCmosSensor(0x23, 0x33);//GreenGain
	CamWriteCmosSensor(0xa0, 0xd0);
	CamWriteCmosSensor(0xa1, 0x31);//Bit[3:0]: AWB update speed, the smaller, the faster
	CamWriteCmosSensor(0xa6, 0x04);
	CamWriteCmosSensor(0xa2, 0x0f);//the low limit of blue gain for indoor scene
	CamWriteCmosSensor(0xa3, 0x2b);//the upper limit of blue gain for indoor scene
	CamWriteCmosSensor(0xa4, 0x0f);//the low limit of red gain for indoor scene
	CamWriteCmosSensor(0xa5, 0x2b);//the upper limit of red gain for indoor scene
	CamWriteCmosSensor(0xa7, 0x9a);//Base B gain,Don't suggest to modify
	CamWriteCmosSensor(0xa8, 0x1c);//Base R gain,Don't suggest to modify
	CamWriteCmosSensor(0xa9, 0x11);
	CamWriteCmosSensor(0xaa, 0x16);
	CamWriteCmosSensor(0xab, 0x16);
	CamWriteCmosSensor(0xac, 0x3c);
	CamWriteCmosSensor(0xad, 0xf0);
	CamWriteCmosSensor(0xae, 0x57);
	CamWriteCmosSensor(0xc6, 0xaa);//Pure color threshold
	CamWriteCmosSensor(0xd2, 0x78);
	
	CamWriteCmosSensor(0xd0, 0xb4);//F light offset
	CamWriteCmosSensor(0xd1, 0x00);//Non-F light offset
	
	//== AWB for Outdoor Scene==	
	CamWriteCmosSensor(0xc8, 0x10);//the low limit of blue gain for outdoor scene
	CamWriteCmosSensor(0xc9, 0x12);//the upper limit of blue gain for outdoor scene
	CamWriteCmosSensor(0xd3, 0x09);//the low limit of red gain for outdoor scene
	CamWriteCmosSensor(0xd4, 0x2a);//the upper limit of red gain for outdoor scene
	
	CamWriteCmosSensor(0xee, 0x4c);
	
	//====== Black Sun Correction =======	
	CamWriteCmosSensor(0x7e, 0xba);//Bit[6]: 1: Enable black sun correction, 0: Disable black sun correction
	
	//如果要去掉black sun，请将此段代码打开 20120522
	/*CamWriteCmosSensor(0x7e, 0xfa);
	CamWriteCmosSensor(0x74, 0xa7);
	CamWriteCmosSensor(0x78, 0x4e);
	CamWriteCmosSensor(0x60, 0xe5);
	CamWriteCmosSensor(0x61, 0xc8);
	CamWriteCmosSensor(0x6d, 0x70);
	CamWriteCmosSensor(0x1e, 0x08);
	CamWriteCmosSensor(0x16, 0xaf);*/

	//====== Switch Direction =======	
	CamWriteCmosSensor(0x1e, 0x09);//09: Normal  19: IMAGE_V_MIRROR   29: IMAGE_H_MIRROR  39: IMAGE_HV_MIRROR

}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALPvSetting
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
static void BF3901_SERIALPvSetting(void)
{
    // TODO: add preview setting here
    
    BF3901_SERIALSensor.InternalClock = 3000000;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALCapSetting
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
static void BF3901_SERIALCapSetting(void)
{
    // TODO: add capture setting here
    
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALGetSensorInfo
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
static void BF3901_SERIALGetSensorInfo(P_IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT Info)
{
    Info->SensorId = BF3901_SERIAL_SENSOR_ID;
    Info->SensorIf = IMAGE_SENSOR_IF_SERIAL;

    /* data format */
    Info->PreviewNormalDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;
    Info->PreviewHMirrorDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;
    Info->PreviewVMirrorDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;
    Info->PreviewHVMirrorDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;
    Info->CaptureDataFormat = IMAGE_SENSOR_DATA_OUT_FORMAT_YUYV;

    Info->PreviewMclkFreq = BF3901_SERIAL_MCLK;
    Info->CaptureMclkFreq = BF3901_SERIAL_MCLK;
    Info->VideoMclkFreq = BF3901_SERIAL_MCLK;
	
    Info->PreviewWidth = BF3901_SERIAL_IMAGE_SENSOR_PV_WIDTH;
    Info->PreviewHeight = BF3901_SERIAL_IMAGE_SENSOR_PV_HEIGHT;
    Info->FullWidth = BF3901_SERIAL_IMAGE_SENSOR_FULL_WIDTH;
    Info->FullHeight = BF3901_SERIAL_IMAGE_SENSOR_FULL_HEIGHT;
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
*    BF3901SerialSensorFeatureCtrl
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
MM_ERROR_CODE_ENUM BF3901_SERIALGetSerialSensorInfo(P_SERIAL_SENSOR_INFO_IN_STRUCT In,P_SERIAL_SENSOR_INFO_OUT_STRUCT Out)
{
	Out->para_1 = KAL_TRUE; 								/* SCK clock inverse */
	Out->para_2 = 4;										/* Cycle */	
	
	if (In->ScenarioId == CAL_SCENARIO_CAMERA_STILL_CAPTURE)
	{
		Out->para_3 = BF3901_SERIAL_IMAGE_SENSOR_PV_WIDTH; 	/* Sensor capture data valid width */
		Out->para_4 = BF3901_SERIAL_IMAGE_SENSOR_PV_HEIGHT;	/* Sensor capture data valid height */
	}
	else
	{
		Out->para_3 = BF3901_SERIAL_IMAGE_SENSOR_FULL_WIDTH; 	/* Sensor preview data valid width */
		Out->para_4 = BF3901_SERIAL_IMAGE_SENSOR_FULL_HEIGHT; 	/* Sensor preview data valid height */
	}

	Out->para_5  = 1;//lane number
	Out->para_6  = KAL_FALSE;//DDR supprt
	Out->para_7  = KAL_FALSE;//CRC support
	
	return MM_ERROR_NONE; 
}


/*************************************************************************
* FUNCTION
*   BF3901_SERIALPowerOn
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
static kal_uint32 BF3901_SERIALPowerOn(void)
{
	static kal_uint8 BF3901_SERIALI2cAddr[] = {BF3901_SERIAL_WRITE_ID_0,
#if (BF3901_SERIAL_WRITE_ID_1 != 0xFF)
	BF3901_SERIAL_WRITE_ID_1,
#endif
#if (BF3901_SERIAL_WRITE_ID_2 != 0xFF)
	BF3901_SERIAL_WRITE_ID_2,
#endif
#if (BF3901_SERIAL_WRITE_ID_3 != 0xFF)
	BF3901_SERIAL_WRITE_ID_3,
#endif
	};
	kal_uint32 SensorId;
	kal_uint8	temp1,temp2;
	kal_uint8 i;

	CisModulePowerOn(BF3901_SERIALSensor.SensorIdx, KAL_TRUE);
	CameraSccbOpen(CAMERA_SCCB_SENSOR, BF3901_SERIAL_WRITE_ID_0,
	BF3901_SERIAL_I2C_ADDR_BITS, BF3901_SERIAL_I2C_DATA_BITS, BF3901_SERIAL_HW_I2C_SPEED);

	BF3901_SERIALSensor.sccb_opened = KAL_TRUE;

	/* add under line if sensor I2C do not support repeat start */
	//CameraSccbDisWR(CAMERA_SCCB_SENSOR);
	vTaskDelay(2);
	// TODO: add hardware reset  by spec here
	{
		BF3901_SERIAL_SET_PDN_LOW;
		vTaskDelay(1);
	}

	//for SPI
//	GPIO_ModeSetup(47, 2);//Data Output
//	GPIO_ModeSetup(59, 2);//Clock Output

	for (i = 0; i < sizeof(BF3901_SERIALI2cAddr) / sizeof(BF3901_SERIALI2cAddr[0]); i++)
	{
		//CameraSccbSetAddr(CAMERA_SCCB_SENSOR, BF3901_SERIALI2cAddr[i]);

		// TODO: read sensor id here
		temp1 = CamReadCmosSensor(0xfc);
		temp2 = CamReadCmosSensor(0xfd);
		SensorId = (temp1 << 8) | temp2;
		BF3901_SERIAL_TRACE("SENSOR ID: %x", SensorId);
	
		if (SensorId == BF3901_SERIAL_SENSOR_ID)
		{
			kal_uint8 Temp;

			/* swap the correct i2c address to first one, it will speed up next time read sensor ID */
			Temp = BF3901_SERIALI2cAddr[0];
			BF3901_SERIALI2cAddr[0] = BF3901_SERIALI2cAddr[i];
			BF3901_SERIALI2cAddr[i] = Temp;
			
			break;
		}
	}

    return SensorId;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALPowerDown
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
static void BF3901_SERIALPowerDown(void)
{
    // TODO: power down sensor by spec here
    if(BF3901_SERIALSensor.sccb_opened == KAL_TRUE)
    {
		CamWriteCmosSensor(0x09, 0x10);
    }
	BF3901_SERIAL_SET_RST_LOW;
	vTaskDelay(1);
	BF3901_SERIAL_SET_PDN_HIGH;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALPreview
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
static void BF3901_SERIALPreview(CAL_SCENARIO_ENUM Id, P_IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT In, P_IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT Out)
{
	kal_uint16 iDummyPixels = 32, iDummyLines = 0;

	BF3901_SERIALSensor.BandingFreq = (CAM_BANDING_50HZ == In->BandingFreq ? BF3901_SERIAL_50HZ : BF3901_SERIAL_60HZ);
	/* change to preview size output */
	BF3901_SERIALSensor.PvMode = KAL_TRUE;
	BF3901_SERIALPvSetting();
	//BF3901_SERIALSetClock(BF3901_SERIAL_PV_INTERNAL_CLK);

	/* set preview frame rate range */
	switch (Id)
	{
	case CAL_SCENARIO_VIDEO:
		BF3901_SERIALSetVideoFps(In->MaxVideoFrameRate);
		break;
	default:
		//BF3901_SERIALSetMaxFps(BF3901_SERIAL_FPS(30));
		BF3901_SERIALSetMinFps(In->NightMode ? BF3901_SERIAL_FPS(5) : BF3901_SERIAL_FPS(10));

		/* roll back shutter&gain from capture state */
		if (BF3901_SERIALSensor.CapState)
		{
			BF3901_SERIALWriteShutter(BF3901_SERIALSensor.Shutter);
			//BF3901_SERIALWriteGain(BF3901_SERIALSensor.Gain);
		}
	}

	/* misc setting */
	BF3901_SERIALSetMirror(In->ImageMirror);
	BF3901_SERIALNightMode(In->NightMode);

	/* change to preview state */
	BF3901_SERIALSensor.CapState = KAL_FALSE;

	/* enable ae/awb */
	BF3901_SERIALAeEnable(KAL_TRUE);
	BF3901_SERIALAwbEnable(KAL_TRUE);

	BF3901_SERIALSensor.LineLength = BF3901_SERIAL_PV_PERIOD_PIXEL_NUMS + iDummyPixels;
	BF3901_SERIALSetDummy(iDummyPixels, iDummyLines);

	/* set grab window */
	Out->WaitStableFrameNum = 0;
	Out->GrabStartX = BF3901_SERIAL_PV_GRAB_START_X;
	Out->GrabStartY = BF3901_SERIAL_PV_GRAB_START_Y;
	Out->ExposureWindowWidth = BF3901_SERIAL_PV_GRAB_WIDTH;
	Out->ExposureWindowHeight = BF3901_SERIAL_PV_GRAB_HEIGHT;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALCapture
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
static void BF3901_SERIALCapture(CAL_SCENARIO_ENUM Id, P_IMAGE_SENSOR_SCENARIO_PARA_IN_STRUCT In, P_IMAGE_SENSOR_SCENARIO_PARA_OUT_STRUCT Out)
{
	kal_uint32 PvClk, PvLineLength, CapShutter, CapLineLength, DummyPixel = 0;

	/* back up preview clock/linelength/shutter */
	PvClk = BF3901_SERIALSensor.InternalClock;
	PvLineLength = BF3901_SERIALSensor.LineLength;
	BF3901_SERIALSensor.Shutter = BF3901_SERIALReadShutter();
	//BF3901_SERIALSensor.Gain = BF3901_SERIALReadGain();

	/* change to capture state */
	BF3901_SERIALSensor.CapState = KAL_TRUE;
	if (In->ImageTargetWidth <= BF3901_SERIAL_IMAGE_SENSOR_PV_WIDTH && In->ImageTargetHeight <= BF3901_SERIAL_IMAGE_SENSOR_PV_HEIGHT)
	{
		DummyPixel = 0;

		if (DummyPixel)
		{
			/* disable ae/awb */
			BF3901_SERIALAeEnable(KAL_FALSE);
			BF3901_SERIALAwbEnable(KAL_FALSE);

			BF3901_SERIALSetDummy(DummyPixel, 0);

			CapLineLength = BF3901_SERIAL_PV_PERIOD_PIXEL_NUMS + DummyPixel;
			CapShutter = (BF3901_SERIALSensor.Shutter * PvLineLength) / CapLineLength;

			/* write shutter */
			BF3901_SERIALWriteShutter(CapShutter);

			Out->WaitStableFrameNum = BF3901_SERIAL_CAPTURE_DELAY_FRAME;
		}
		else
		{
			Out->WaitStableFrameNum = 0;
		}

		/* set grab window */
		Out->GrabStartX = BF3901_SERIAL_PV_GRAB_START_X;
		Out->GrabStartY = BF3901_SERIAL_PV_GRAB_START_Y;
		Out->ExposureWindowWidth = BF3901_SERIAL_PV_GRAB_WIDTH;
		Out->ExposureWindowHeight = BF3901_SERIAL_PV_GRAB_HEIGHT;
	}
	else
	{
		DummyPixel = 32;

		/* disable ae/awb */
		BF3901_SERIALAeEnable(KAL_FALSE);
		BF3901_SERIALAwbEnable(KAL_FALSE);

		/* change to full size output */
		BF3901_SERIALSensor.PvMode = KAL_FALSE;


		BF3901_SERIALSetDummy(DummyPixel, 0);

		CapLineLength = BF3901_SERIAL_PV_PERIOD_PIXEL_NUMS + DummyPixel;
		CapShutter = (BF3901_SERIALSensor.Shutter * PvLineLength) / CapLineLength;

		/* write shutter */
		BF3901_SERIALWriteShutter(CapShutter);

		/* set grab window */
		Out->WaitStableFrameNum = BF3901_SERIAL_CAPTURE_DELAY_FRAME;
		Out->GrabStartX = BF3901_SERIAL_FULL_GRAB_START_X;
		Out->GrabStartY = BF3901_SERIAL_FULL_GRAB_START_Y;
		Out->ExposureWindowWidth = BF3901_SERIAL_FULL_GRAB_WIDTH;
		Out->ExposureWindowHeight = BF3901_SERIAL_FULL_GRAB_HEIGHT;
	}
}

static MM_ERROR_CODE_ENUM BF3901_SERIALSensorClose(void);

/*************************************************************************
* FUNCTION
*   BF3901_SERIALDetectSensorId
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
static kal_uint32 BF3901_SERIALDetectSensorId(void)
{
    kal_uint32 SensorId;
    IMAGE_SENSOR_INDEX_ENUM InvIdx;
    kal_int8 i, j;

    switch (BF3901_SERIALSensor.SensorIdx)
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
            SensorId = BF3901_SERIALPowerOn();
            BF3901_SERIALSensorClose();
            if (BF3901_SERIAL_SENSOR_ID == SensorId)
            {
                return BF3901_SERIAL_SENSOR_ID;
            }
        }
    }
    return MM_ERROR_SENSOR_READ_ID_FAIL;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALInitOperationPara
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
static void BF3901_SERIALInitOperationPara(P_IMAGE_SENSOR_GET_INIT_OPERATION_PARA_STRUCT Para)
{
    Para->CaptureDelayFrame = 0; /* use WaitStableFrameNum instead of this */
    Para->PreviewDelayFrame = BF3901_SERIAL_PREVIEW_DELAY_FRAME;
    Para->PreviewDisplayWaitFrame = BF3901_SERIAL_FIRST_PREVIEW_DELAY_FRAME;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSensorOpen
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
static MM_ERROR_CODE_ENUM BF3901_SERIALSensorOpen(void)
{
	if (BF3901_SERIALPowerOn() != BF3901_SERIAL_SENSOR_ID)
	{
		return MM_ERROR_SENSOR_READ_ID_FAIL;
	}
#ifdef BF3901_SERIAL_LOAD_FROM_T_FLASH
	if (!BF3901_SERIAL_Initialize_from_T_Flash()) /* for debug use. */
#endif
	{
		BF3901_SERIALInitialSetting(); /* apply the sensor initial setting */
	}

	/* default setting */
	BF3901_SERIALSensor.BypassAe = KAL_FALSE; /* for panorama view */
	BF3901_SERIALSensor.BypassAwb = KAL_FALSE; /* for panorama view */
	BF3901_SERIALSensor.CapState = KAL_FALSE; /* preview state */
	BF3901_SERIALSensor.PvMode = KAL_TRUE; /* preview size output mode */
	BF3901_SERIALSensor.BandingFreq = BF3901_SERIAL_50HZ;
	BF3901_SERIALSensor.InternalClock = 1; /* will be update by setclock function */
	BF3901_SERIALSensor.Shutter = 1; /* default shutter 1 avoid divide by 0 */
	BF3901_SERIALSensor.Gain = 0x40; /* default gain 1x */
	BF3901_SERIALSensor.FrameLength = BF3901_SERIAL_PV_PERIOD_LINE_NUMS; /* will be update by setdummy function */
	BF3901_SERIALSensor.LineLength = BF3901_SERIAL_PV_PERIOD_PIXEL_NUMS; /* will be update by setdummy function */

	return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSensorFeatureCtrl
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
static MM_ERROR_CODE_ENUM BF3901_SERIALSensorFeatureCtrl(kal_uint32 Id, void *In, void *Out, kal_uint32 OutLen, kal_uint32 *RealOutLen)
{
    MM_ERROR_CODE_ENUM ErrCode = MM_ERROR_NONE;

#if 0	
    if (Id >= CAL_FEATURE_WEBCAM_BEGIN && Id <= CAL_FEATURE_WEBCAM_END)
    {
        return BF3901_SERIALWebcamFeatureCtrl(Id, In, Out, OutLen, RealOutLen);
    }
#endif
    switch (Id)
    {
    /* query and set series */
    case CAL_FEATURE_CAMERA_BRIGHTNESS:
    case CAL_FEATURE_CAMERA_EV_VALUE:
        ErrCode = BF3901_SERIALEv(In, Out);
        break;
    case CAL_FEATURE_CAMERA_CONTRAST:
        ErrCode = BF3901_SERIALContrast(In, Out);
        break;
    case CAL_FEATURE_CAMERA_SATURATION:
        ErrCode = BF3901_SERIALSaturation(In, Out);
        break;
    case CAL_FEATURE_CAMERA_SHARPNESS:
        ErrCode = BF3901_SERIALSharpness(In, Out);
        break;
    case CAL_FEATURE_CAMERA_WB:
        ErrCode = BF3901_SERIALWb(In, Out);
        break;
    case CAL_FEATURE_CAMERA_IMAGE_EFFECT:
        ErrCode = BF3901_SERIALEffect(In, Out);
        break;
    case CAL_FEATURE_CAMERA_STILL_CAPTURE_SIZE:
        ErrCode = BF3901_SERIALStillCaptureSize(In, Out);
        break;
    case CAL_FEATURE_CAMERA_BANDING_FREQ:
        ErrCode = BF3901_SERIALBanding(In, Out);
        break;
    case CAL_FEATURE_CAMERA_SCENE_MODE:
        ErrCode = BF3901_SERIALSceneMode(KAL_TRUE, In, Out);
        break;
    case CAL_FEATURE_CAMERA_VIDEO_SCENE_MODE:
        ErrCode = BF3901_SERIALSceneMode(KAL_FALSE, In, Out);
        break;
    case CAL_FEATURE_CAMERA_FLASH_MODE:
    case CAL_FEATURE_CAMERA_HUE:
    case CAL_FEATURE_CAMERA_GAMMA:
        ErrCode = MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
        break;

    /* get info series */
    case IMAGE_SENSOR_FEATURE_GET_SENSOR_INFO:
        BF3901_SERIALGetSensorInfo(Out);
        break;
    case IMAGE_SENSOR_FEATURE_GET_SENSOR_ID:
        ((P_IMAGE_SENSOR_COMM_DATA_STRUCT)Out)->FeatureValue = BF3901_SERIALDetectSensorId();
        break;
    case IMAGE_SENSOR_FEATURE_GET_SENSOR_FRAME_RATE:
        ((P_IMAGE_SENSOR_COMM_DATA_STRUCT)Out)->FeatureValue = \
        BF3901_SERIALSensor.InternalClock * BF3901_SERIAL_FPS(1) / (BF3901_SERIALSensor.LineLength * BF3901_SERIALSensor.FrameLength);
        break;
    case IMAGE_SENSOR_FEATURE_GET_INIT_OPERATION_PARA:
        BF3901_SERIALInitOperationPara(Out);
        break;
    case IMAGE_SENSOR_FEATURE_GET_HW_DEPENDENT_SETTING:
        CamGetHWInfo(BF3901_SERIALSensor.SensorIdx, Out);
        break;

    /* set para series */
    case IMAGE_SENSOR_FEATURE_SET_SENSOR_POWER_DOWN:
        BF3901_SERIALPowerDown();
        break;
    case IMAGE_SENSOR_FEATURE_SET_NVRAM_SENSOR_INFO:
//        BF3901_SERIALSensor.NvramData = &(((nvram_camera_para_struct *)In)->SENSOR);
        break;
    case IMAGE_SENSOR_FEATURE_SET_CAMERA_SOURCE:
        BF3901_SERIALSensor.SensorIdx = ((P_IMAGE_SENSOR_SET_CAMERA_SOURCE_STRUCT)In)->ImageSensorIdx;
        break;
    case IMAGE_SENSOR_FEATURE_CTRL_NIGHTMODE: /* move to preview */
        break;
    case IMAGE_SENSOR_FEATURE_SET_AE_BYPASS:
        BF3901_SERIALSensor.BypassAe = (kal_bool)((P_IMAGE_SENSOR_COMM_DATA_STRUCT)In)->FeatureValue;
        BF3901_SERIALAeEnable(BF3901_SERIALSensor.BypassAe ? KAL_FALSE : KAL_TRUE);
        break;
    case IMAGE_SENSOR_FEATURE_SET_AWB_BYPASS:
        BF3901_SERIALSensor.BypassAwb = (kal_bool)((P_IMAGE_SENSOR_COMM_DATA_STRUCT)In)->FeatureValue;
        BF3901_SERIALAwbEnable(BF3901_SERIALSensor.BypassAwb ? KAL_FALSE : KAL_TRUE);
        break;
    case IMAGE_SENSOR_FEATURE_GET_SERIAL_SENSOR_INFO:
		BF3901_SERIALGetSerialSensorInfo(In, Out);
		break;
    default:
        ErrCode = MM_ERROR_SENSOR_FEATURE_NOT_SUPPORT;
        break;
    }
    return ErrCode;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSensorCtrl
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
static MM_ERROR_CODE_ENUM BF3901_SERIALSensorCtrl(CAL_SCENARIO_ENUM Id, void *In, void *Out, kal_uint32 OutLen, kal_uint32 *RealOutLen)
{
    switch (Id)
    {
    case CAL_SCENARIO_CAMERA_PREVIEW:
    case CAL_SCENARIO_VIDEO:
    case CAL_SCENARIO_WEBCAM_PREVIEW:
    case CAL_SCENARIO_WEBCAM_CAPTURE:
        BF3901_SERIALPreview(Id, In, Out);
        break;
    case CAL_SCENARIO_CAMERA_STILL_CAPTURE:
        BF3901_SERIALCapture(Id, In, Out);
        break;
    default:
        break;
    }
    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSensorClose
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
static MM_ERROR_CODE_ENUM BF3901_SERIALSensorClose(void)
{
    BF3901_SERIALPowerDown();
    CisModulePowerOn(BF3901_SERIALSensor.SensorIdx, KAL_FALSE);
	
	BF3901_SERIALSensor.sccb_opened = KAL_FALSE;
    CameraSccbClose(CAMERA_SCCB_SENSOR);

    //Resume
//    GPIO_ModeSetup(47, 1);
//    GPIO_ModeSetup(59, 1);

    return MM_ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*   BF3901_SERIALSensorFunc
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
MM_ERROR_CODE_ENUM BF3901_SERIALSensorFunc(P_IMAGE_SENSOR_FUNCTION_STRUCT *pfSensorFunc)
{
    static IMAGE_SENSOR_FUNCTION_STRUCT ImageSensorFuncBF3901_SERIAL =
    {
        BF3901_SERIALSensorOpen,
        BF3901_SERIALSensorFeatureCtrl,
        BF3901_SERIALSensorCtrl,
        BF3901_SERIALSensorClose,
    };

    *pfSensorFunc = &ImageSensorFuncBF3901_SERIAL;
  
    return MM_ERROR_NONE;
}

#ifdef __BF3901_SERIAL_DEBUG_TRACE__
static kal_bool BF3901_SERIALAtGetValue(char *Str, kal_uint32 *Data)
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

    if (!BF3901_SERIALAtGetValue(Str, Data))
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

            BF3901_SERIAL_TRACE("Current Frame Rate: %d.%d fps", IspCurrentFrameRate / 10, IspCurrentFrameRate % 10);
        }
        break;
    case 0x02:
        BF3901_SERIAL_TRACE("Read BB Reg[%x] = %x", Data[2], DRV_Reg32(Data[2]));
        break;
    case 0x03:
        DRV_Reg32(Data[2]) = Data[3];
        BF3901_SERIAL_TRACE("Write BB Reg[%x] = %x", Data[2], DRV_Reg32(Data[2]));
        break;
    case 0x04:
        BF3901_SERIAL_TRACE("Read Sensor Reg[%x] = %x", Data[2], CamReadCmosSensor(Data[2]));
        break;
    case 0x05:
        CamWriteCmosSensor(Data[2], Data[3]);
        BF3901_SERIAL_TRACE("Write Sensor Reg[%x] = %x", Data[2], CamReadCmosSensor(Data[2]));
        break;
    case 0x06:
        CamWriteCmosSensor(Data[2], Data[3]);
        BF3901_SERIAL_TRACE("Read Sensor Reg[%x:%x] = %x", Data[3], Data[4], CamReadCmosSensor(Data[4]));
        break;
    case 0x07:
        CamWriteCmosSensor(Data[2], Data[3]);
        CamWriteCmosSensor(Data[4], Data[5]);
        CamWriteCmosSensor(Data[2], Data[3]);
        BF3901_SERIAL_TRACE("Write Sensor Reg[%x:%x] = %x", Data[3], Data[4], CamReadCmosSensor(Data[4]));
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
                BF3901_SERIAL_TRACE("Read Sensor Reg[%x] = %x", Data[2] + i, CamReadCmosSensor(Data[2] + i));
                if (!(i % 200))
                {
                    vTaskDelay(30);
                }
            }
        }
        break;
    case 0x0B:
        {
            kal_uint32 i;

            for (i = 0; i < Data[3]; i++)
            {
                BF3901_SERIAL_TRACE("Read BB Reg[%x] = %x", Data[2] + i * 4, DRV_Reg32(Data[2] + i * 4));
                if (!(i % 200))
                {
                    vTaskDelay(30);
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

