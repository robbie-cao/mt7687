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

/** 
  * @page Bluetooth Android Notification example project
  * @{

@par Overview

 - Example project description
   - This example demonstrates a simple and convenient way to access various types of notifications generated from Android notification center.
 - Features of the example
   - This example supports the notifications received in the notification center on the Android smartphone.
 - Process / procedure of the sample
   - Output from the example project: The notifications will show in the log.

@par Hardware and software environment

  - Supported platform
    - LinkIt 2523 HDK.
  - Remote device configuration
    - Install the MediaTek SmartDevice APK on an Android smartphone.
  - PC/environment configuration
    - Use SecureCRT or other hyperterminal to output system log.
    - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.
    
@par Directory contents

  - Source and header files
   - apps/bt_android_notification/src/main.c				     Main program.
   - apps/bt_android_notification/src/system_mt2523.c			     MT2523x's system clock configuration file.
   - apps/bt_android_notification/inc/hal_feature_config.h 		     MT2523x's feature configuration file.
   - apps/bt_android_notification/src/hal_gpio_ept_config.c 	             GPIO program file to initialize the pins with the EPT generated files.
   - apps/bt_android_notification/src/ept_gpio_var.c        	             GPIO pin assignment with variable names.
   - apps/bt_android_notification/src/ept_eint_var.c        	             EINT assignment file with variable names.
   - apps/bt_android_notification/inc/hal_gpio_ept_config.h 	             File to map the EPT generated macros to 32-bits data.
   - apps/bt_android_notification/inc/memory_map.h		             MT2523x's memory layout symbol file.
   - apps/bt_android_notification/inc/ept_eint_drv.h		             EINT basic configuration header file.
   - apps/bt_android_notification/inc/ept_gpio_drv.h		             GPIO pin assignment and basic configuration file.
   - apps/bt_android_notification/src/ble_dogp_service.c                     DOGP(Data over GATT profile) profile main file.
   - apps/bt_android_notification/src/ble_dogp_adp_service.c                 DOGP service file.
   - apps/bt_android_notification/src/ble_gatts_srv_common.c                 Implementation of SDK common callbacks.
   - apps/bt_android_notification/src/bt_notify_conversion.c                 Data conversion.
   - apps/bt_android_notification/src/bt_notify_data_parse.c                 The notification element parser file.
   - apps/bt_android_notification/src/xml_main.c                             XML parser file.
   - apps/bt_android_notification/src/bt_notify_test.c                       App test file, show notifications or send data to Andoird.
   - apps/bt_android_notification/lib/libbtnotify.a                          Library for the data transfer link.
   - apps/bt_android_notification/GCC/startup_mt2523.s		             MT2523x's startup file for GCC.
   - apps/bt_android_notification/GCC/syscalls.c			     MT2523x's syscalls for GCC.

  - Project configuration files using GCC
   - apps/bt_android_notification/GCC/feature.mk		    	     Feature configuration.
   - apps/bt_android_notification/GCC/Makefile		 	             Makefile.
   - apps/bt_android_notification/GCC/flash.ld			             Linker script.
   

@par Run the demo

  - Connect board to PC with serial port cable.
  - Power on the LinkIt 2523 HDK.
  - Connect to LinkIt 2523 HDK from Android smartphone using Mediatek SmartDevice APK
  - The notifications will be received.
  - The notification will show in the log on the connected SecureCRT or Hyper Terminal.


* @}
*/
