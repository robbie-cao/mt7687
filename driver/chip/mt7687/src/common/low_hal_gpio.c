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

/****************************************************************************
    Module Name:
    GPIO

    Abstract:
    GPIO.

    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
***************************************************************************/

#include "hal_eint.h"
#include "hal_gpio.h"
#if defined(HAL_EINT_MODULE_ENABLED) || defined(HAL_GPIO_MODULE_ENABLED)

#include "type_def.h"

#include "low_hal_gpio.h"
#include "hal_gpio_7687.h"


int32_t gpio_direction(ENUM_IOT_GPIO_NAME_T gpio_name, ENUM_DIR_T direction)
{
    return halGPIO_ConfDirection(gpio_name, direction);
}


int32_t gpio_write(ENUM_IOT_GPIO_NAME_T gpio_name, uint8_t data)
{
    return halGPO_Write(gpio_name, data);
}



int32_t gpio_int(ENUM_IOT_GPIO_NAME_T       gpio_name,
                 ENUM_NVIC_SENSE_T          edgeLevelTrig,
                 ENUM_DEBOUNCE_EN_T         debounceEnable,
                 ENUM_DEBOUNCE_POL_T        polarity,
                 ENUM_DEBOUNCE_DUAL_T       dual,
                 ENUM_DEBOUNCE_PRESCALER_T  prescaler,
                 uint8_t                    u1PrescalerCount,
                 void (*callback)(ENUM_IOT_GPIO_NAME_T gpio_name))
{
    return halRegGPInterrupt(gpio_name, edgeLevelTrig, debounceEnable , polarity , dual , prescaler, u1PrescalerCount , callback);
}

gpio_status gpio_get_status(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    gpio_status p;
    p.GPIO_PIN = gpio_name;
    p.GPIO_OUTEN = halgetGPIO_OutEnable(gpio_name);
    p.GPI_DIN = halgetGPIO_DIN(gpio_name);
    p.GPO_DOUT = halgetGPIO_DOUT(gpio_name);
    return p;
}

int32_t gpio_PullUp(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    INT32 ret = 0;
    topPAD_Control_GPIO(gpio_name, ePAD_CONTROL_BY_CM4); //let gpio control by cm4
    ret = halGPIO_PullUp_SET(gpio_name);
    return ret;
}

int32_t gpio_PullDown(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    INT32 ret = 0;
    topPAD_Control_GPIO(gpio_name, ePAD_CONTROL_BY_CM4); //let gpio control by cm4
    ret = halGPIO_PullDown_SET(gpio_name);
    return ret;
}
int32_t gpio_PullDisable(ENUM_IOT_GPIO_NAME_T gpio_name)
{
    INT32 ret = 0;
    topPAD_Control_GPIO(gpio_name, ePAD_CONTROL_BY_CM4); //let gpio control by cm4
    ret = halGPIO_PullDown_RESET(gpio_name);
    ret = halGPIO_PullUp_RESET(gpio_name);
    return ret;
}
#endif

