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

#include "hal_platform.h"

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED

#include "hal_display_lcd.h"
#include "hal_display_lcd_internal.h"
#include "hal_display_dsi_internal.h"
#include "hal_mipi_tx_config_internal.h"
#include "hal_gpt.h"
#include "stdio.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

volatile LCD_DSI_REGISTER_T*	lcd_dsi_register_ptr = (LCD_DSI_REGISTER_T *)(DSI_BASE);
volatile MIPITX_CONFIG_REGISTER_T*	mipi_tx_config_register_ptr = (MIPITX_CONFIG_REGISTER_T *)(MIPI_TX_CFG_BASE);
hal_display_dsi_dphy_timing_struct_t backup_timing;
bool backup_ssc;

bool restoreHSMode = FALSE;

hal_display_dsi_status_t display_dsi_init(bool ssc_enable)
{	
	*((volatile uint32_t *) (0xA0480010))|= 0x1;		// data path set to DSI

	backup_ssc = ssc_enable;

	mipi_tx_config_register_ptr->mipitx_con4_register.field.RG_MIPI_26M_REQ_EN = 1;
	mipi_tx_config_register_ptr->mipitx_con4_register.field.RG_MIPI_26M_CK_SEL = 0;
	mipi_tx_config_register_ptr->mipitx_con9_register.field.RG_LN_META_CTRL = 1;	
	mipi_tx_config_register_ptr->mipitx_con0_register.field.RG_PLL_DDSFBK_EN = 1;
	mipi_tx_config_register_ptr->mipitx_con6_register.field.RG_LNT_BGR_EN = 0;
	mipi_tx_config_register_ptr->mipitx_con6_register.field.RG_LNT_BGR_EN = 1;

	hal_gpt_delay_us(60);		// 30us

	mipi_tx_config_register_ptr->mipi_pll_pwr_register.field.DA_PLL_DDS_PWR_ON = 1;
	mipi_tx_config_register_ptr->mipi_pll_pwr_register.field.DA_PLL_DDS_ISO_EN = 0;
	mipi_tx_config_register_ptr->mipitx_con0_register.field.RG_LNT_HSTX_BIAS_EN = 1;
	mipi_tx_config_register_ptr->mipitx_con0_register.field.RG_LNT_LPTX_BIAS_EN = 1;	
	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_FBKDIV = 0x0F;	
	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_POSDIV = 0;	
	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_PREDIV = 0;	
	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_PHIDIV_SEL = 0;	
	if(ssc_enable)
	{
		mipi_tx_config_register_ptr->mipi_lcdds1_register.field.RG_LCDDS_SSC_PHASE_INI = 1;
		mipi_tx_config_register_ptr->mipi_lcdds1_register.field.RG_LCDDS_SSC_PRD = 0x01B1;		
	}
	else
		mipi_tx_config_register_ptr->mipi_lcdds1_register.field.RG_LCDDS_SSC_EN = 0;
	mipi_tx_config_register_ptr->mipi_lcdds_register.field.RG_LCDDS_PCW_NCPO = 0x17000000;	
	if(ssc_enable)
		mipi_tx_config_register_ptr->mipi_lcdds2_register.value = 0x02F102F21;
	mipi_tx_config_register_ptr->mipi_lcdds1_register.field.RG_LCDDS_PWDB = 1;
	mipi_tx_config_register_ptr->mipitx_con0_register.field.RG_PLL_EN = 1;	

	hal_gpt_delay_us(60);		// 20us

	mipi_tx_config_register_ptr->mipi_lcdds1_register.field.RG_LCDDS_PCW_NCPO_CHG = 0;
	mipi_tx_config_register_ptr->mipi_lcdds1_register.field.RG_LCDDS_PCW_NCPO_CHG = 1;

	hal_gpt_delay_us(60);		// 20us

	if(ssc_enable)
		mipi_tx_config_register_ptr->mipi_lcdds1_register.field.RG_LCDDS_SSC_EN = 1;
	mipi_tx_config_register_ptr->mipitx_con0_register.field.RG_DIG_CK_EN = 1;	
	mipi_tx_config_register_ptr->mipitx_con4_register.field.RG_PAD_PL_SEL = 0;
	mipi_tx_config_register_ptr->mipitx_con4_register.field.RG_PAD_PL_EN = 0;

	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_PHIDIV_SEL = 0;
	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_POSDIV = 0;
	for(int i =0; i < 128; i++)
		lcd_dsi_register_ptr->LCD_DSI_CMDQ_REGISTER[i].value = 0;

	lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.LANE_NUM = 1;		// Enable 1 lane
	lcd_dsi_register_ptr->LCD_DSI_PHY_LCCON_REGISTER.field.LC_HSTX_EN = 0;	// High speed enable
	lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.HSTX_CKLP_EN = 0;		// Auto LP/HS mode

	// timing default
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.DA_HS_TRAIL = 0x04;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.DA_HS_ZERO = 0x07;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.DA_HS_PREP = 0x02;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.LPX = 0x03;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.DA_HS_EXIT = 0x04;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.TA_GET = 0xF;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.TA_SURE = 0x02;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.TA_GO = 0x0C;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON2_REGISTER.field.CLK_HS_TRAIL = 0x03;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON2_REGISTER.field.CLK_HS_ZERO = 0x0C;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON3_REGISTER.field.CLK_HS_POST = 0x09;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON3_REGISTER.field.CLK_HS_PREP = 0x01;
	
	return HAL_DISPLAY_DSI_STATUS_OK;
}

hal_display_dsi_status_t display_dsi_deinit(void)
{	
	display_dsi_enter_ulps();
	while(lcd_dsi_register_ptr->LCD_DSI_STATE_DBG0_REGISTER.field.DPHY_CTL_STATE_C != 0x80);
	while(lcd_dsi_register_ptr->LCD_DSI_STATE_DBG1_REGISTER.field.DPHY_ESC_STATE_0 != 0x20);
	
	mipi_tx_config_register_ptr->mipitx_con4_register.field.RG_PAD_PL_SEL = 0;
	mipi_tx_config_register_ptr->mipitx_con4_register.field.RG_PAD_PL_EN = 1;
	mipi_tx_config_register_ptr->mipi_lcdds1_register.field.RG_LCDDS_SSC_EN = 0;
	mipi_tx_config_register_ptr->mipitx_con0_register.field.RG_PLL_EN = 0;	
	mipi_tx_config_register_ptr->mipi_lcdds1_register.field.RG_LCDDS_PWDB = 0;
	mipi_tx_config_register_ptr->mipi_pll_pwr_register.field.DA_PLL_DDS_ISO_EN = 1;	
	mipi_tx_config_register_ptr->mipi_pll_pwr_register.field.DA_PLL_DDS_PWR_ON = 0;
	mipi_tx_config_register_ptr->mipitx_con6_register.field.RG_LNT_BGR_EN = 0;
	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_FBKDIV = 0x0F;	
	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_POSDIV = 0;	
	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_PREDIV = 0;	
	mipi_tx_config_register_ptr->mipitx_con2_register.field.RG_PLL_PHIDIV_SEL = 0;	
	mipi_tx_config_register_ptr->mipi_lcdds_register.field.RG_LCDDS_PCW_NCPO = 0;	
	mipi_tx_config_register_ptr->mipitx_con4_register.field.RG_MIPI_26M_REQ_EN = 0;
	
	return HAL_DISPLAY_DSI_STATUS_OK;
}

hal_display_dsi_status_t display_dsi_set_command_queue(uint32_t *pData, uint8_t size, bool force_update)
{
	uint32_t i;

	for(i = 0; i < size; i++)
		lcd_dsi_register_ptr->LCD_DSI_CMDQ_REGISTER[i].value = pData[i];

	lcd_dsi_register_ptr->LCD_DSI_CMDQ_CON_REGISTER.field.CMDQ_SIZE = size;

	if(force_update)
	{
		lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.value = 0;
		lcd_dsi_register_ptr->LCD_DSI_START_REGISTER.field.START = 0;
		lcd_dsi_register_ptr->LCD_DSI_START_REGISTER.field.START = 1;
		while(lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.CMD_DONE_INT_FLAG == 0);
	}
	
	return HAL_DISPLAY_DSI_STATUS_OK;
}

hal_display_dsi_status_t display_dsi_read_register(uint8_t cmd, uint8_t *buffer, uint8_t size)
{
	uint32_t					max_try_count = 5;
	uint32_t					read_count, read_timeout_us;
	uint8_t						packet_type;
	LCD_DSI_CMD_REGISTER_T		cmd_reg, temp_reg;
	LCD_DSI_RX_DATA_REGISTER_T 	read_data0;
	LCD_DSI_RX_DATA_REGISTER_T 	read_data1;
	LCD_DSI_RX_DATA_REGISTER_T 	read_data2;
//	LCD_DSI_RX_DATA_REGISTER_T 	read_data3;

	if(buffer == NULL || size == 0)
		return HAL_DISPLAY_DSI_STATUS_INVALID_PARAMETER;

	do
	{
		if(max_try_count == 0)
			return HAL_DISPLAY_DSI_STATUS_READ_FAILED;
		max_try_count--;
		read_count = 0;
		read_timeout_us = 1000;
			
		cmd_reg.field.config = 0x04;
		cmd_reg.field.data0 = cmd;
		if(cmd < 0xB0)	// short cmd
			cmd_reg.field.dataID = 0x06;
		else
			cmd_reg.field.dataID = 0x14;
		cmd_reg.field.data1 = 0;

		// set max return size
		temp_reg.field.config = 0x00;
		temp_reg.field.dataID = 0x37;
		temp_reg.field.data0 = size<=10 ? size:10;
		temp_reg.field.data1 = 0;

		// set cmdq
		lcd_dsi_register_ptr->LCD_DSI_CMDQ_REGISTER[0].value = temp_reg.value;
		lcd_dsi_register_ptr->LCD_DSI_CMDQ_REGISTER[1].value = cmd_reg.value;
		lcd_dsi_register_ptr->LCD_DSI_CMDQ_CON_REGISTER.field.CMDQ_SIZE = 2;

		// trigger DSI 
		lcd_dsi_register_ptr->LCD_DSI_RX_RACK_REGISTER.field.RACK = 1;
		lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.value = 0;
		lcd_dsi_register_ptr->LCD_DSI_START_REGISTER.field.START = 0;
		lcd_dsi_register_ptr->LCD_DSI_START_REGISTER.field.START = 1;

		while(lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.LPRX_RD_RDY_INT_FLAG == 0)
		{
			hal_gpt_delay_us(100);
			read_timeout_us -= 100;
			if(read_timeout_us == 0 )
			{
				// reset DSI engine
				lcd_dsi_register_ptr->LCD_DSI_RX_RACK_REGISTER.field.RACK = 1;
				lcd_dsi_register_ptr->LCD_DSI_COM_CON_REGISTER.field.DSI_RESET = 1;	
				lcd_dsi_register_ptr->LCD_DSI_COM_CON_REGISTER.field.DSI_RESET = 0;	

				return HAL_DISPLAY_DSI_STATUS_READ_FAILED;
			}
		}

		// get data
		
		lcd_dsi_register_ptr->LCD_DSI_RX_RACK_REGISTER.field.RACK = 1;
		lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.value = 0;
		
		read_data0.value = lcd_dsi_register_ptr->LCD_DSI_RX_DATA03_REGISTER.value;
		read_data1.value = lcd_dsi_register_ptr->LCD_DSI_RX_DATA47_REGISTER.value;
		read_data2.value = lcd_dsi_register_ptr->LCD_DSI_RX_DATA8B_REGISTER.value;
//		read_data3.value = lcd_dsi_register_ptr->LCD_DSI_RX_DATAC_REGISTER.value;

		packet_type = read_data0.field.byte0;

		// 0x02: acknowledge & error report
		// 0x11: generic short read response(1 byte return)
		// 0x12: generic short read response(2 byte return)
		// 0x1a: generic long read response
		// 0x1c: dcs long read response
		// 0x21: dcs short read response(1 byte return)
		// 0x22: dcs short read response(2 byte return)

		if(packet_type == 0x1A || packet_type == 0x1C)	// long read response
		{
			read_count = read_data0.field.byte1 + read_data0.field.byte2 * 16;
			if(read_count > 10)
				read_count = 10;

			if(read_count > size)
				read_count = size;

			if(read_count<=4)
			{
				memcpy((void*)buffer, (void*)&read_data1, read_count);
			}
			else if(read_count<=8)
			{
				memcpy((void*)buffer, (void*)&read_data1, 4);
				memcpy((void*)(buffer+4), (void*)&read_data2, read_count-4);
			}
			else
			{
				memcpy((void*)buffer, (void*)&read_data1, 4);
				memcpy((void*)(buffer+4), (void*)&read_data2, 4);
				memcpy((void*)(buffer+8), (void*)&read_data2, read_count-8);
			}			
		}
		else if(packet_type == 0x11 || packet_type == 0x12 || packet_type == 0x21 || packet_type == 0x22) // short read response
		{
			if(packet_type == 0x11 || packet_type == 0x21)
			{
				read_count = 1;
				buffer[0] = read_data0.field.byte1;
			}
			else
			{
				read_count = 2;
				buffer[0] = read_data0.field.byte1;
				buffer[1] = read_data0.field.byte2;
			}
		}
		else if(packet_type == 0x02)
		{
			// re-read
		}
		else
		{
			return HAL_DISPLAY_DSI_STATUS_READ_FAILED;
		}
	}while(packet_type == 0x02);

	return HAL_DISPLAY_DSI_STATUS_OK;
}

hal_display_dsi_status_t display_dsi_enter_ulps(void)
{
	restoreHSMode = lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.HSTX_CKLP_EN;
	lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.HSTX_CKLP_EN = 0;
	lcd_dsi_register_ptr->LCD_DSI_PHY_LD0CON_REGISTER.field.L0_ULPM_EN = 1;
	lcd_dsi_register_ptr->LCD_DSI_PHY_LCCON_REGISTER.field.LC_ULPM_EN = 1;

	return HAL_DISPLAY_DSI_STATUS_OK;
}

hal_display_dsi_status_t display_dsi_exit_ulps(void)
{
	lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.HSTX_CKLP_EN = restoreHSMode;
	lcd_dsi_register_ptr->LCD_DSI_MODE_CON_REGISTER.field.SLEEP_MODE = 1;
	lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.LANE_NUM = 1;
	lcd_dsi_register_ptr->LCD_DSI_START_REGISTER.field.SLEEPOUT_START = 1;
	while(lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.SLEEPOUT_DONE_INT_FLAG == 0);
	lcd_dsi_register_ptr->LCD_DSI_MODE_CON_REGISTER.field.SLEEP_MODE = 0;
	lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.value = 0;
	lcd_dsi_register_ptr->LCD_DSI_START_REGISTER.field.SLEEPOUT_START = 0;
	
	return HAL_DISPLAY_DSI_STATUS_OK;
}


hal_display_dsi_status_t display_dsi_set_dphy_timing(hal_display_dsi_dphy_timing_struct_t* timing)
{
	memcpy(&backup_timing, timing, sizeof(hal_display_dsi_dphy_timing_struct_t));

	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.DA_HS_TRAIL = timing->da_hs_trail;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.DA_HS_ZERO = timing->da_hs_zero;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.DA_HS_PREP = timing->da_hs_prep;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.LPX = timing->lpx;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.DA_HS_EXIT = timing->da_hs_exit;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.TA_GET = timing->ta_get;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.TA_SURE = timing->ta_sure;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.TA_GO = timing->ta_go;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON2_REGISTER.field.CLK_HS_TRAIL = timing->clk_hs_trail;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON2_REGISTER.field.CLK_HS_ZERO = timing->clk_hs_zero;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON3_REGISTER.field.CLK_HS_POST = timing->clk_hs_post;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON3_REGISTER.field.CLK_HS_PREP = timing->clk_hs_prep;

	return HAL_DISPLAY_DSI_STATUS_OK;
}

hal_display_dsi_status_t display_dsi_restore_dsi_setting(void)
{
//	display_dsi_init(backup_ssc);
//	display_dsi_set_DPHY_timing(&backup_timing);

	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.DA_HS_TRAIL = backup_timing.da_hs_trail;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.DA_HS_ZERO = backup_timing.da_hs_zero;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.DA_HS_PREP = backup_timing.da_hs_prep;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON0_REGISTER.field.LPX = backup_timing.lpx;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.DA_HS_EXIT = backup_timing.da_hs_exit;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.TA_GET = backup_timing.ta_get;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.TA_SURE = backup_timing.ta_sure;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON1_REGISTER.field.TA_GO = backup_timing.ta_go;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON2_REGISTER.field.CLK_HS_TRAIL = backup_timing.clk_hs_trail;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON2_REGISTER.field.CLK_HS_ZERO = backup_timing.clk_hs_zero;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON3_REGISTER.field.CLK_HS_POST = backup_timing.clk_hs_post;
	lcd_dsi_register_ptr->LCD_DSI_PHY_TIMCON3_REGISTER.field.CLK_HS_PREP = backup_timing.clk_hs_prep;

	lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.LANE_NUM = 1;		// Enable 1 lane
	lcd_dsi_register_ptr->LCD_DSI_PHY_LCCON_REGISTER.field.LC_HSTX_EN = 0;	// High speed enable
	lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.HSTX_CKLP_EN = 0;		// Auto LP/HS mode

	return HAL_DISPLAY_DSI_STATUS_OK;
}

hal_display_dsi_status_t display_dsi_set_clock(uint32_t pll_clock, bool ssc_enable)
{
	return HAL_DISPLAY_DSI_STATUS_OK;
}

hal_display_dsi_status_t display_dsi_set_transfer_mode(hal_display_dsi_transfer_mode_t mode)
{
	switch(mode)
	{
		case HAL_DISPLAY_DSI_TRANSFER_MODE_LP:
			lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.HSTX_CKLP_EN = 0;
			lcd_dsi_register_ptr->LCD_DSI_PHY_LCCON_REGISTER.field.LC_HSTX_EN = 0;
			break;
		case HAL_DISPLAY_DSI_TRANSFER_MODE_HS:
			lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.HSTX_CKLP_EN = 0;
			lcd_dsi_register_ptr->LCD_DSI_PHY_LCCON_REGISTER.field.LC_HSTX_EN = 1;
			break;
		case HAL_DISPLAY_DSI_TRANSFER_MODE_AUTO:
			lcd_dsi_register_ptr->LCD_DSI_TXRX_CON_REGISTER.field.HSTX_CKLP_EN = 1;			
			lcd_dsi_register_ptr->LCD_DSI_PHY_LCCON_REGISTER.field.LC_HSTX_EN = 0;
			break;
	}
	
	return HAL_DISPLAY_DSI_STATUS_OK;
}

void dsiplay_dsi_start_bta_transfer(uint32_t cmd)
{
	// set cmdq
	lcd_dsi_register_ptr->LCD_DSI_CMDQ_REGISTER[0].value = 0x20;
	lcd_dsi_register_ptr->LCD_DSI_CMDQ_CON_REGISTER.field.CMDQ_SIZE = 1;
	
	// trigger DSI 
	lcd_dsi_register_ptr->LCD_DSI_RX_RACK_REGISTER.field.RACK = 1;
	lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.value = 0;
	lcd_dsi_register_ptr->LCD_DSI_START_REGISTER.field.START = 0;
	lcd_dsi_register_ptr->LCD_DSI_START_REGISTER.field.START = 1;

	while(lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.TE_RDY_INT_FLAG == 0);
	
	lcd_dsi_register_ptr->LCD_DSI_RX_RACK_REGISTER.field.RACK = 1;		// RACK

	hal_display_dsi_set_transfer_mode(HAL_DISPLAY_DSI_TRANSFER_MODE_HS);

	display_dsi_set_command_queue(&cmd, 1, true);
}

void display_dsi_apply_setting(void)
{
#if 0
	uint32_t temp, bpp = 1;
	LCD_DSI_PSCON_REGISTER_T test;

	hal_display_lcd_output_color_format_t lcd_to_lcm_fmt;

    GET_LCD_ROI_CTRL_OUTPUT_FORMAT(temp);
    lcd_to_lcm_fmt = (hal_display_lcd_status_t)((temp & 38) >> 3);

    switch(lcd_to_lcm_fmt)
    {
        case HAL_DISPLAY_LCD_ROI_OUTPUT_RGB565:
            bpp = 2;
            test= lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER;
            test.field.DSI_PS_SEL = 0x0;
            lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER.value = test.value;
            break;
        case HAL_DISPLAY_LCD_ROI_OUTPUT_RGB888:
            bpp = 3;
            test= lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER;
            test.field.DSI_PS_SEL = 0x3;
            lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER.value = test.value;
            break;
        default:
            return HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER;
            break;
    }

    lcd_dsi_register_ptr->LCD_DSI_VACT_NL_REGISTER.field.VACT_NL = roi_height;
    test = lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER;
    test.field.DSI_PS_WC = roi_width * bpp;
    lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER.value = test.value;
    
    lcd_dsi_register_ptr->LCD_DSI_MEM_CONTI_REGISTER.field.DSI_RWMEM_CONTI = 0x3C;
#endif    
}
#ifdef __cplusplus
}
#endif

#endif
