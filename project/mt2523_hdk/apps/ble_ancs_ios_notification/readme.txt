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
  * @page ble_ancs_ios_notification sample project
  * @{

@par Overview
 - Example project description
   This example project demonstrates a simple and convenient way through BLE link to access the notifications generated on iOS devices.
 - Features of the example project
   The example project receives the notificaitons, retrieves the notification attributes, retrieves the App attributes and performs the notification actions.
 - Process / procedure of the example project 
   To receive the notifications, connect BLuetooth between the LinkIt 2523 HDK and an iOS device, and enable the data source and the notification source. 
   Then retrieve the notification attributes and the App attributes to get the detailed information of a notification.
 - Limitation of the example project
   The notification action is only supported in iOS 8.0 or later.
 	 
@par Hardware and software environment

  - Supported platform
    MediaTek LinkIt 2523 HDK.

  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200.

@par Directory contents
  - Source and header files
   - apps/ble_ancs_ios_notification/src/libble_ancs.a  		ANCS service, receive and parse notifications and attributes.
   - apps/ble_ancs_ios_notification/src/ble_ancs_app.c		App file, show notifications and attributes.
   - apps/ble_ancs_ios_notification/src/ble_common_callback.c	Implementation of GATT/GAP/SM common callbacks.
   - apps/ble_ancs_ios_notification/src/ept_eint_var.c	       	EINT assignment file with variable names.
   - apps/ble_ancs_ios_notification/src/ept_gpio_var.c	       	GPIO pin assignment with variable names.
   - apps/ble_ancs_ios_notification/src/hal_gpio_ept_config.c  	GPIO program file to initialize the pins with the EPT generated files.
   - apps/ble_ancs_ios_notification/src/hci_log.c		Show HCI log.
   - apps/ble_ancs_ios_notification/src/main.c			Main program.					
   - apps/ble_ancs_ios_notification/src/system_mt2523.c		mt2523x's system clock configuration file.
   - apps/ble_ancs_ios_notification/inc/hal_feature_config.h	MT2523x's feature configuration file.
   - apps/ble_ancs_ios_notification/inc/hal_gpio_ept_config.h 	File to map the EPT generated macros to 32-bits data.
   - apps/ble_ancs_ios_notification/inc/memory_map.h		MT2523 memory layout symbol file.
   - apps/ble_ancs_ios_notification/inc/ept_eint_drv.h		EINT basic configuration header file.
   - apps/ble_ancs_ios_notification/inc/ept_gpio_drv.h		GPIO pin assignment and basic configuration file.
   - apps/ble_ancs_ios_notification/inc/ept_keypad_drv.h	Keypad basic configuration header file.
   - apps/ble_ancs_ios_notification/inc/main.h			Main program header file.			
   - apps/ble_ancs_ios_notification/inc/hci_log.h		HCI log header file.			
   - apps/ble_ancs_ios_notification/GCC/startup_mt2523.s	MT2523x's startup file for GCC.
   - apps/ble_ancs_ios_notification/GCC/syscalls.c		MT2523x's syscalls for GCC.

  - Project configuration files using GCC.
   - apps/ble_ancs_ios_notification/GCC/feature.mk		Feature configuration.
   - apps/ble_ancs_ios_notification/GCC/Makefile		Makefile.
   - apps/ble_ancs_ios_notification/GCC/flash.ld		Linker script.
    
@par Run the example
  - Connect board to PC with serial port cable.
  - Build the example project with the command, "./build.sh mt2523_hdk ble_ancs_ios_notification bl" from the SDK root folder and download the bin file to LinkIt 2523 development board.
  - Power on the development board, and connect it from the iOS device. The notifications will show in the log.
* @}
*/
