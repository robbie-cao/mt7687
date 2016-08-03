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

#include "mt25x3_hdk_backlight.h"
#include "hal_isink.h"
#ifdef HAL_DISPALY_DSI_MODULE_ENABLED
#include "hal_display_dsi.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void BSP_Backlight_deinit(void)
{
#ifdef HAL_ISINK_MODULE_ENABLED
	hal_isink_deinit(HAL_ISINK_CHANNEL_0);
	hal_isink_deinit(HAL_ISINK_CHANNEL_1);
#endif

#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED
	hal_display_pwm_deinit();
#endif
}

void BSP_Backlight_enable(bool enable)
{
#ifdef HAL_ISINK_MODULE_ENABLED

#endif

#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED
	if(enable)
		BSP_Backlight_init_display_pwm();
	else
		BSP_Backlight_deinit();
#endif	
}

#ifdef HAL_ISINK_MODULE_ENABLED

bool isDobuleEn = false;

void BSP_Backlight_init_isink(hal_isink_channel_t channel, bool isDouble, hal_isink_current_t current)
{
	hal_isink_init(channel);
	hal_isink_set_mode(channel, HAL_ISINK_MODE_REGISTER);
	hal_isink_set_clock_source(channel, HAL_ISINK_CLOCK_SOURCE_2MHZ);
	if(isDouble)
		hal_isink_set_double_current(channel, current);
	else
		hal_isink_set_step_current(channel, current);
	isDobuleEn = isDouble;
}

void BSP_Backlight_set_clock_source_isink(hal_isink_channel_t channel, hal_isink_clock_source_t source_clock)
{
	hal_isink_set_clock_source(channel, source_clock);
}

void BSP_Backlight_set_step_current(hal_isink_channel_t channel, hal_isink_current_t current)
{
	if(isDobuleEn)
	{
		BSP_Backlight_deinit();
		BSP_Backlight_init_isink(channel, false, current);
	}
	else
		hal_isink_set_step_current(channel, current);
}

void BSP_Backlight_set_double_current_isink(hal_isink_channel_t channel, hal_isink_current_t current)
{
	if(!isDobuleEn)
	{
		BSP_Backlight_deinit();
		BSP_Backlight_init_isink(channel, true, current);
	}
	else
		hal_isink_set_double_current(channel, current);
}
#endif

#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED
void BSP_Backlight_init_display_pwm(void)
{
	hal_display_pwm_init(HAL_DISPLAY_PWM_CLOCK_26MHZ);
	hal_display_pwm_set_duty(80);
}

void BSP_Backlight_set_clock_source_display_pwm(hal_display_pwm_clock_t source_clock)
{
	hal_display_pwm_set_clock(source_clock);
}

void BSP_Backlight_set_duty_display_pwm(uint8_t percent)
{
	hal_display_pwm_set_duty(percent);
}

void BSP_Backlight_set_width_display_pwm(uint32_t width)
{
	hal_display_pwm_set_width(width);
}

#endif

#ifdef HAL_DISPALY_DSI_MODULE_ENABLED
void BSP_Backlight_init_lcm_brightness(void)
{
    uint32_t data_array[16];

    data_array[0] = 0x00023902;
    data_array[1] = 0x51 | (0xFF << 8);
    hal_display_dsi_set_command_queue(data_array, 2, true);
}

void BSP_Backlight_deinit_lcm_brightness(void)
{
    uint32_t data_array[16];

    data_array[0] = 0x00023902;
    data_array[1] = 0x51 | (0x0 << 8);
    hal_display_dsi_set_command_queue(data_array, 2, true);
}

void BSP_Backlight_set_step_lcm_brightness(uint8_t level)
{
    uint32_t data_array[16];

    if(level > 255)
        level = 255;

    data_array[0] = 0x00023902;
    data_array[1] = 0x51 | (level << 8);
    hal_display_dsi_set_command_queue(data_array, 2, true);
}
#endif

#ifdef __cplusplus
}
#endif

