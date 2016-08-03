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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd_sw.h"
#include "mt25x3_hdk_lcd.h"
#include "hal_display_dsi_internal.h"
#include "hal_gpt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HEIGHT 240
#define WIDTH 240

extern volatile LCD_REGISTER_T *lcd_register_ptr;
extern volatile LCD_DSI_REGISTER_T *lcd_dsi_register_ptr;

uint32_t ranesis_delay;

void Delayms(uint32_t delay_count)
{
  volatile uint32_t delay;
  for (delay =0;delay <(delay_count*4);delay++) {}
}

void LCD_Init_Interface_nt35510(hal_display_lcd_if_timing_para_t *lcd_if_time_para, hal_display_lcd_if_mode_para_t *lcd_if_mode_para)
{

	hal_display_dsi_DPHY_timing_struct_t	timing;

	timing.DA_HS_TRAIL = 0x04;
	timing.DA_HS_ZERO = 0x07;
	timing.DA_HS_PREP = 0x02;
	timing.LPX = 0x03;
	timing.DA_HS_EXIT = 0x04;
	timing.TA_GET = 0xF;
	timing.TA_SURE = 0x02;
	timing.TA_GO = 0x0C;
	timing.CLK_HS_TRAIL = 0x03;
	timing.CLK_HS_ZERO = 0x0C;
	timing.CLK_HS_POST = 0x09;
	timing.CLK_HS_PREP = 0x01;

	hal_display_dsi_set_DPHY_timing(&timing);

	lcd_if_mode_para->if_cs_no = HAL_DISPLAY_LCD_IF_SERIAL_0;
	lcd_if_mode_para->if_3wire_mode = 1;
	lcd_if_mode_para->if_cs_stay_low_mode = 0;
	lcd_if_mode_para->if_driving = HAL_DISPLAY_LCD_DRIVING_16mA;
	lcd_if_mode_para->if_single_a0_mode = 0;
	lcd_if_mode_para->if_use_SDI = 0;
	lcd_if_mode_para->if_width = HAL_DISPLAY_LCD_SCNF_IF_WIDTH_8;
	lcd_if_mode_para->if_hw_cs = 1;
	lcd_if_mode_para->if_power_domain = HAL_DISPLAY_LCD_POWER_DOMAIN_1V8;

	lcd_if_time_para->if_cs_no = HAL_DISPLAY_LCD_IF_SERIAL_0;
	lcd_if_time_para->csh = 0;
	lcd_if_time_para->css = 0;
	lcd_if_time_para->wr_1st= 0;
	lcd_if_time_para->wr_2nd= 0;
	lcd_if_time_para->rd_1st= 7;
	lcd_if_time_para->rd_2nd= 7;
	lcd_if_time_para->clk_freq = HAL_DISPLAY_LCD_SERIAL_CLOCK_124MHZ;

} /* init_lcd_interface() */

/***********************nt35510********************************/

void LCD_EnterSleep_nt35510(void)
{
	unsigned int data_array[16];

	data_array[0] = 0x00100500;
	hal_display_dsi_set_cmdq(data_array, 1, 1);
	hal_gpt_delay_ms(120);

	data_array[0] = 0x00280500;
	hal_display_dsi_set_cmdq(data_array, 1, 1);
	hal_gpt_delay_ms(10);

	data_array[0] = 0x014F1500;
	hal_display_dsi_set_cmdq(data_array, 1, 1);
	hal_gpt_delay_ms(40);
}

void LCD_ExitSleep_nt35510(void)
{
	LCD_Init_nt35510(0, 0);
}

void LCD_blockClear_nt35510(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t data)
{
}

void LCD_ClearAll_nt35510(uint16_t data)
{
	LCD_blockClear_nt35510(0,0,HEIGHT-1,WIDTH-1,data);
}

uint32_t device_code;

void LCD_Init_nt35510(uint32_t bkground, void **buf_addr)
{
	uint32_t data_array[16];

  // Do HW Reset
  	hal_display_lcd_toggle_reset(10, 120);

	data_array[0] = 0x00053902;
	data_array[1] = 0x2555AAFF;
	data_array[2] = 0x00000001;
	hal_display_dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00093902;
	data_array[1] = 0x000201F8;
	data_array[2] = 0x00133320;
	data_array[3] = 0x00000048;
	hal_display_dsi_set_cmdq(data_array, 4, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000108;
	hal_display_dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x0D0D0DB0;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x343434B6;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x0D0D0DB1;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x343434B7;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x000000B2;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x242424B8;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x000001BF;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x0F0F0FB3;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x343434B9;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x080808B5;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x000003C2;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x242424BA;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x007800BC;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x007800BD;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x006400BE;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00353902;
	data_array[1] = 0x003300D1;
	data_array[2] = 0x003A0034;
	data_array[3] = 0x005C004A;
	data_array[4] = 0x00A60081;
	data_array[5] = 0x011301E5;
	data_array[6] = 0x01820154;
	data_array[7] = 0x020002CA;
	data_array[8] = 0x02340201;
	data_array[9] = 0x02840267;
	data_array[10] = 0x02B702A4;
	data_array[11] = 0x02DE02CF;
	data_array[12] = 0x03FE02F2;
	data_array[13] = 0x03330310;
	data_array[14] = 0x0000006D;
	hal_display_dsi_set_cmdq(data_array, 15, 1);

	data_array[0] = 0x00353902;
	data_array[1] = 0x003300D2;
	data_array[2] = 0x003A0034;
	data_array[3] = 0x005C004A;
	data_array[4] = 0x00A60081;
	data_array[5] = 0x011301E5;
	data_array[6] = 0x01820154;
	data_array[7] = 0x020002CA;
	data_array[8] = 0x02340201;
	data_array[9] = 0x02840267;
	data_array[10] = 0x02B702A4;
	data_array[11] = 0x02DE02CF;
	data_array[12] = 0x03FE02F2;
	data_array[13] = 0x03330310;
	data_array[14] = 0x0000006D;
	hal_display_dsi_set_cmdq(data_array, 15, 1);

	data_array[0] = 0x00353902;
	data_array[1] = 0x003300D3;
	data_array[2] = 0x003A0034;
	data_array[3] = 0x005C004A;
	data_array[4] = 0x00A60081;
	data_array[5] = 0x011301E5;
	data_array[6] = 0x01820154;
	data_array[7] = 0x020002CA;
	data_array[8] = 0x02340201;
	data_array[9] = 0x02840267;
	data_array[10] = 0x02B702A4;
	data_array[11] = 0x02DE02CF;
	data_array[12] = 0x03FE02F2;
	data_array[13] = 0x03330310;
	data_array[14] = 0x0000006D;
	hal_display_dsi_set_cmdq(data_array, 15, 1);

	data_array[0] = 0x00353902;
	data_array[1] = 0x003300D4;
	data_array[2] = 0x003A0034;
	data_array[3] = 0x005C004A;
	data_array[4] = 0x00A60081;
	data_array[5] = 0x011301E5;
	data_array[6] = 0x01820154;
	data_array[7] = 0x020002CA;
	data_array[8] = 0x02340201;
	data_array[9] = 0x02840267;
	data_array[10] = 0x02B702A4;
	data_array[11] = 0x02DE02CF;
	data_array[12] = 0x03FE02F2;
	data_array[13] = 0x03330310;
	data_array[14] = 0x0000006D;
	hal_display_dsi_set_cmdq(data_array, 15, 1);

	data_array[0] = 0x00353902;
	data_array[1] = 0x003300D5;
	data_array[2] = 0x003A0034;
	data_array[3] = 0x005C004A;
	data_array[4] = 0x00A60081;
	data_array[5] = 0x011301E5;
	data_array[6] = 0x01820154;
	data_array[7] = 0x020002CA;
	data_array[8] = 0x02340201;
	data_array[9] = 0x02840267;
	data_array[10] = 0x02B702A4;
	data_array[11] = 0x02DE02CF;
	data_array[12] = 0x03FE02F2;
	data_array[13] = 0x03330310;
	data_array[14] = 0x0000006D;
	hal_display_dsi_set_cmdq(data_array, 15, 1);

	data_array[0] = 0x00353902;
	data_array[1] = 0x003300D6;
	data_array[2] = 0x003A0034;
	data_array[3] = 0x005C004A;
	data_array[4] = 0x00A60081;
	data_array[5] = 0x011301E5;
	data_array[6] = 0x01820154;
	data_array[7] = 0x020002CA;
	data_array[8] = 0x02340201;
	data_array[9] = 0x02840267;
	data_array[10] = 0x02B702A4;
	data_array[11] = 0x02DE02CF;
	data_array[12] = 0x03FE02F2;
	data_array[13] = 0x03330310;
	data_array[14] = 0x0000006D;
	hal_display_dsi_set_cmdq(data_array, 15, 1);
	hal_gpt_delay_ms(10);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000008;
	hal_display_dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x00007CB1;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x000005B6;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x007070B7;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x030301B8;
	data_array[2] = 0x00000003;
	hal_display_dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x000002BC;
	hal_display_dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x5002D0C9;
	data_array[2] = 0x00005050;
	hal_display_dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00351500;
	hal_display_dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x773A1500;
	hal_display_dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x00110500;
	hal_display_dsi_set_cmdq(data_array, 1, 1);
	hal_gpt_delay_ms(120);

	data_array[0] = 0x00290500;
	hal_display_dsi_set_cmdq(data_array, 1, 1);
	hal_gpt_delay_ms(20);
}

 void LCD_EnterIdle_nt35510(void)
{
	hal_display_dsi_enterULPS();
}

void LCD_ExitIdle_nt35510(void)
{
	hal_display_dsi_exitULPS();
}
  
void LCD_BlockWrite_nt35510(uint16_t startx,uint16_t starty,uint16_t endx,uint16_t endy)
{
	unsigned int data_array[16];

	unsigned char x0_MSB = (startx >> 8) & 0xFF;
	unsigned char x0_LSB = startx & 0xFF;
	unsigned char x1_MSB = (endx >> 8) & 0xFF;
	unsigned char x1_LSB = endx & 0xFF;
	unsigned char y0_MSB = (starty >> 8) & 0xFF;
	unsigned char y0_LSB = starty & 0xFF;
	unsigned char y1_MSB = (endy >> 8) & 0xFF;
	unsigned char y1_LSB = endy & 0xFF;

	data_array[0] = 0x00053902;
	data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2A;
	data_array[2] = x1_LSB;
	hal_display_dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] = (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2B;
	data_array[2] = y1_LSB;
	hal_display_dsi_set_cmdq(data_array, 3, 1);

	hal_display_dsi_set_transfer_mode(HAL_DISPLAY_DSI_TRANSFER_MODE_HS);
	hal_display_dsi_set_transfer_speed(HAL_DISPLAY_DSI_SPEED_QUATER);

	hal_display_lcd_toggle_DMA(1);

	data_array[0] = 0x002C3909;
	hal_display_dsi_set_cmdq(data_array, 1, 1);
//	hal_dsiplay_dsi_start_BTA_transfer(0x002C3909);

	while(lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.FRAME_DONE_INT_FLAG == 0);
	hal_display_dsi_set_transfer_mode(HAL_DISPLAY_DSI_TRANSFER_MODE_LP);
	hal_display_dsi_set_transfer_speed(HAL_DISPLAY_DSI_SPEED_FULL);
}


/***************************** Take Notice *******************************
*************************************************************************/ 
 
void LCD_IOCTRL_nt35510(LCM_IOCTRL_ID_ENUM ID, void* Parameters)
{
	switch (ID)
	{
		case LCM_IOCTRL_QUERY__FRAME_RATE:
			*((uint32_t*)(Parameters)) = 60;
			break;

		case LCM_IOCTRL_QUERY__BACK_PORCH:
			*(uint32_t *)(Parameters) = 12;
			break;
		    
		case LCM_IOCTRL_QUERY__FRONT_PORCH:
			*(uint32_t *)(Parameters) = 12;
			break;

		case LCM_IOCTRL_QUERY__IF_CS_NUMBER:
			*(uint32_t *)(Parameters) = HAL_DISPLAY_LCD_IF_SERIAL_0;
			break;

		case LCM_IOCTRL_QUERY__LCM_WIDTH: 
			*((unsigned long*)(Parameters)) = 240;// physical, not use app's LCD_WIDTH
			break;

		case LCM_IOCTRL_QUERY__LCM_HEIGHT: 
			*((unsigned long*)(Parameters)) = 240;//320;// physical, not use app's LCD_HEIGHT
			break;

#if 0
		//#ifdef __LCD_ESD_RECOVERY__
		/** LCM ESD feature control, Check LCD ESD is happend or not **/
		case LCM_IOCTL_ESD_CHECK:
		{
		LCM_ESD_INFO_STRUCT* pESDInfo = (LCM_ESD_INFO_STRUCT *)Parameters;
		kal_bool lcm_esd_test = KAL_FALSE;

		lcm_esd_test = LCD_Read_ESD_nt35510();

		if(pESDInfo)
		{
		 //TODO: add ESD real check function here
		 // If happend, must take lcm_esd_test = KAL_TURE;
		 if(lcm_esd_test)
		 {
		  lcm_esd_test = KAL_FALSE;
		  pESDInfo->bEsdHappened = KAL_TRUE;
		 }
		 else
		  pESDInfo->bEsdHappened = KAL_FALSE;
		}

		return LCM_IOCTRL_OK;
		break;
		}
			   
		/** LCM ESD feature control, If LCD ESD is happend, We should do something here like resetLCM. **/
		case LCM_IOCTL_ESD_RESET:
		//TODO: add LCM reset function here
		//Example
		LCD_Init_nt35510(0,0);

		break;  
#endif

		default:
		    break;
	}
}

void LCD_ClearScreen_nt35510(int color)
{
	LCD_ClearAll_nt35510(color);
}

void LCD_ClearScreenBW_nt35510(void)
{
}

LCD_Funcs LCD_func_nt35510 = {
	LCD_Init_nt35510,
	LCD_Init_Interface_nt35510,
	LCD_BlockWrite_nt35510,
	LCD_EnterSleep_nt35510,
	LCD_ExitSleep_nt35510,
	LCD_EnterIdle_nt35510,
	LCD_ExitIdle_nt35510,
	LCD_ClearScreen_nt35510,
	LCD_ClearScreenBW_nt35510,
	LCD_IOCTRL_nt35510
};

#ifdef __cplusplus
}
#endif

