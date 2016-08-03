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
  * @page ble_heart_rate_server example project
  * @{
  
@par Overview

  - Example project description 
    - The example project implements the Heart Rate service and Device Information service, and sets the virtual test values to 
      all characteristics of the service. After the BLE connection is established between a peer device and the LinkIt 2523 HDK, then 
      the peer device can read these values. 
  
  
@par Hardware and software environment  

  - Supported platform 
    - LinkIt 2523 HDK.
    
  - PC/environment configuration
    - Need a serial tool (such as hyper terminal) for UART logging.
    - Com port srtting: baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

  - Peer test device
    - on iphone 5 or later version device, install the "LightBlue" app, which can be downloaded from App Store.

 
@par Directory contents 
  - Source and header files
    - apps/ble_heart_rate_server/src/main.c  			   Main program.
    - apps/ble_heart_rate_server/src/ble_gatts_srv_common.c        GATT server common function file.
    - apps/ble_heart_rate_server/src/ble_message.c                 GATT and GAP message file.
    - apps/ble_heart_rate_server/src/ble_static_callback.c         GATT and GAP static callback file.
    - apps/ble_heart_rate_server/src/ble_hrs.c                     GATT service HRS header file.	
    - apps/ble_heart_rate_server/src/ble_dis.c                     GATT service DIS header file.
    - apps/ble_heart_rate_server/src/ble_hr_main.c                 Heart rate main program file.   
    - apps/ble_heart_rate_server/src/system_mt2523.c		   MT2523x's system clock configuration file.
    - apps/ble_heart_rate_server/GCC/startup_mt2523.s		   MT2523x's startup file for GCC.
    - apps/ble_heart_rate_server/GCC/syscalls.c			   MT2523x's syscalls for GCC
    - apps/ble_heart_rate_server/MDK-ARM/startup_mt2523.s	   MT2523x's startup file for Keil.
    - apps/ble_heart_rate_server/inc/hal_feature_config.h 	   MT2523x's feature configuration file.
    - apps/ble_heart_rate_server/inc/memory_map.h		   MT2523x's memory layout symbol file.
    - apps/ble_heart_rate_server/inc/ble_hr_main.h                 Heart rate main program header file.
    - apps/ble_heart_rate_server/inc/ble_hrs.h                     GATT service HRS header file.
    - apps/ble_heart_rate_server/inc/ble_dis.h                     GATT service DIS header file.
    - apps/ble_heart_rate_server/inc/ble_message.h                 BLE message header file.
    - apps/ble_heart_rate_server/inc/ble_gatts_srv_common.h        GATT server header file.
    - apps/ble_heart_rate_server/inc/ble_app_utils.h               BLE app utils header file.
    - apps/ble_heart_rate_server/inc/ble_bds_app_util.h            BDS tool header file.
   

  - Project configuration files using GCC
    - apps/ble_heart_rate_server/GCC/feature.mk		    	   Feature configuration file.
    - apps/ble_heart_rate_server/GCC/Makefile		 	   Makefile.
    - apps/ble_heart_rate_server/GCC/flash.ld			   Linker script.

  - Project configuration files using Keil
    - apps/ble_heart_rate_server/MDK-ARM/ble_heart_rate_server.uvoptx.uvprojx	uVision5 Project File. Contains the project structure in XML format.
    - apps/ble_heart_rate_server/MDK-ARM/ble_heart_rate_server.uvoptx.uvoptx	uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
    - apps/ble_heart_rate_server/MDK-ARM/flash.sct		                Linker script.

  
@par Run the example
  - pre-condition
    - Connect board to PC with serial port cable.
    - Build the sample project with the command "./build.sh mt2523_hdk ble_heart_rate_server bl" under the SDK root folder on GCC, or 
      build the project ble_heart_rate_server.uvprojx under the MDK-ARM folder on Keil, then download the bin file to MT2523.

 - Step-by-step instructions on how to run the example project.
   - Install a BLE related application on the peer device, Like "LightBlue" on iphone.
   - Enter "LightBlue" APP in peer device, then make the BLE connection with LinkIt 2523 HDK.   
   - Read Heart rate or the device infornmation, then you will see test values from LinkIt 2523 HDK.
  
  
  * @}
*/