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

#include "sensor_comm_def.h"
#include "isp_comm_def.h"
#include "image_sensor.h"
#include "camera_sccb.h"
#include "hal_platform.h"
#include "hal_gpio.h"
#include "hal_pmu_internal.h"

//const kal_int16 gpio_camera_reset_pin = HAL_GPIO_24;
const kal_int16 gpio_camera_cmpdn_pin = HAL_GPIO_25;

//#define CAMERA_MAIN_RST_PIN   		gpio_camera_reset_pin
#define CAMERA_MAIN_PDN_PIN   		gpio_camera_cmpdn_pin
#define CAMERA_SUB_RST_PIN    		-1 /* -1: indicate not support */
#define CAMERA_SUB_PDN_PIN    		-1 /* -1: indicate not support */

//extern const char gpio_flashlight_en_pin;

#define FLASHLIGHT_EN_PIN   -		1 /* -1: indicate not support */

#define CAM_SCCB_SCL_MODE 			(1)//(4)
#define CAM_SCCB_SDA_MODE 			(1)//(3)

#define CAM_SCCB_SW_I2C_DELAY		(0xA0)

/*************************************************************************
* FUNCTION
*    CameraPinCtrl
*
* DESCRIPTION
*    this function contrl camera pin high or low
*
* PARAMETERS
*    Pin: gpio pin, -1: just return
*    Data: 0: low, 1: high, -1: just return
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void CameraPinCtrl(kal_int16 Pin, kal_int8 Data)
{
    //hal_pinmux_set_function((hal_gpio_pin_t)Pin, 0);
    hal_gpio_set_direction((hal_gpio_pin_t)Pin, (hal_gpio_direction_t)HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output((hal_gpio_pin_t)Pin, (hal_gpio_data_t)Data);
}

/*************************************************************************
* FUNCTION
*    CamRstPinCtrl
*
* DESCRIPTION
*    this function contrl camera reset pin high or low
*
* PARAMETERS
*    Idx: IMAGE_SENSOR_MAIN or IMAGE_SENSOR_SUB
*    Data: 0: low, 1: high
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void CamRstPinCtrl(IMAGE_SENSOR_INDEX_ENUM Idx, kal_int8 Data)
{
	switch (Idx)
	{
		case IMAGE_SENSOR_MAIN:
		case IMAGE_SENSOR_BAK1:
			//CameraPinCtrl(CAMERA_MAIN_RST_PIN, Data);
			break;
                case IMAGE_SENSOR_SUB:
                case IMAGE_SENSOR_SUB_BAK1:
                case IMAGE_SENSOR_MATV:
                case IMAGE_SENSOR_MAX:
                break;
	}
}

/*************************************************************************
* FUNCTION
*    CamPdnPinCtrl
*
* DESCRIPTION
*    this function contrl camera power down pin high or low
*
* PARAMETERS
*    Idx: IMAGE_SENSOR_MAIN or IMAGE_SENSOR_SUB
*    Data: 0: low, 1: high
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void CamPdnPinCtrl(IMAGE_SENSOR_INDEX_ENUM Idx, kal_int8 Data)
{
	 switch (Idx)
	{
		case IMAGE_SENSOR_MAIN:
		case IMAGE_SENSOR_BAK1:
			CameraPinCtrl(CAMERA_MAIN_PDN_PIN, Data);
			break;
                case IMAGE_SENSOR_SUB:
                case IMAGE_SENSOR_SUB_BAK1:
                case IMAGE_SENSOR_MATV:
                case IMAGE_SENSOR_MAX:
                break;
	}

}

/*************************************************************************
* FUNCTION
*    CamGetHWInfo
*
* DESCRIPTION
*    this function get current HW configuration
*
* PARAMETERS
*    Info
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void CamGetHWInfo(IMAGE_SENSOR_INDEX_ENUM Idx, P_IMAGE_SENSOR_HW_DEPENDENT_STRUCT Info)
{
	Info->DataPinConnection = DATA_PIN_CONNECT_BIT7_0; /*[Hesong 0303] For MT6250/6252,data pin fixed*/
	switch (Idx)
	{
	case IMAGE_SENSOR_MAIN:
	case IMAGE_SENSOR_BAK1:
		Info->IspDrivingCurrent = ISP_DRIVING_2MA;
		break;
        case IMAGE_SENSOR_SUB:
        case IMAGE_SENSOR_SUB_BAK1:
        case IMAGE_SENSOR_MATV:
        case IMAGE_SENSOR_MAX:
        break;
	}
}


// Compact Image Sensor Module Power ON/OFF
extern IMAGE_SENSOR_GET_SENSOR_INFO_STRUCT CurrentSensorInfo;
void CisModulePowerOn(IMAGE_SENSOR_INDEX_ENUM SensorIdx, kal_bool On)
{
	if(On == KAL_TRUE)
	{
                #if defined(__SERIAL_SENSOR_V1_SUPPORT__)
		if (CurrentSensorInfo.SensorIf == IMAGE_SENSOR_IF_SERIAL)
		{
			//hal_pinmux_set_function(26, 1); //I0:CMCSD0
			//hal_pinmux_set_function(27, 1); //I0:CMCSD1
			//hal_pinmux_set_function(28, 1); //O:CMMCLK
			//hal_pinmux_set_function(29, 1); //I0:CMCSK
		}
		#endif
                
                PMIC_VR_CONTROL(PMIC_VCAMA,1); //Enable VCAMA 2.8V
	}
	else
	{
                PMIC_VR_CONTROL(PMIC_VCAMA,0); //Disable VCAMA 2.8V

		/* output low to avoid electric leakage */
		CamPdnPinCtrl(IMAGE_SENSOR_MAIN, 1);
		CamRstPinCtrl(IMAGE_SENSOR_MAIN, 0);
	}
}

