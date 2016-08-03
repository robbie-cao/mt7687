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
  * @page ble_find_me example project
  * @{
  
@par Overview

  - Example project description 
    -  This example project demonstrates the basic functions of Find Me profile.
 
  
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
    - apps/ble_find_me/src/main.c  			  Main program.
    - apps/ble_find_me/src/ble_gatts_srv_common.c         GATT server common function file.
    - apps/ble_find_me/src/ble_message.c                  GATT and GAP message file.
    - apps/ble_find_me/src/ble_static_callback.c          GATT and GAP static callback file.
    - apps/ble_find_me/src/ble_ias.c                      GATT service IAS header file.	
    - apps/ble_find_me/src/ble_fmp_main.c                 Find Me Profile main program file.
    - apps/ble_find_me/src/system_mt2523.c		  MT2523x's system clock configuration file.
    - apps/ble_find_me/GCC/startup_mt2523.s		  MT2523x's startup file for GCC.
    - apps/ble_find_me/GCC/syscalls.c			  MT2523x's syscalls for GCC.
    - apps/ble_find_me/MDK-ARM/startup_mt2523.s	          MT2523x's startup file for Keil.
    - apps/ble_find_me/inc/hal_feature_config.h 	  MT2523x's feature configuration file.
    - apps/ble_find_me/inc/memory_map.h		          MT2523x's memory layout symbol file.
    - apps/ble_find_me/inc/ble_app_utils.h                BLE app utility header file.
    - apps/ble_find_me/inc/ble_bds_app_util.h             BDS tool utility header filee.
    - apps/ble_find_me/inc/ble_gatts_srv_common.h         GATT server common function header file.
    - apps/ble_find_me/inc/ble_message.h                  GATT and GAP message header file.
    - apps/ble_find_me/inc/ble_fmp_main.h                 Find Me Profile main program header file.
    - apps/ble_find_me/inc/ble_ias.h                      GATT service IAS header file.
  
  - Project configuration files using GCC
    - apps/ble_find_me/GCC/feature.mk		    	  Feature configuration file.
    - apps/ble_find_me/GCC/Makefile		 	  Makefile.
    - apps/ble_find_me/GCC/flash.ld			  Linker script.

  - Project configuration files using Keil
    - apps/ble_find_me/MDK-ARM/ble_find_me.uvoptx.uvprojx	uVision5 Project File. Contains the project structure in XML format.
    - apps/ble_find_me/MDK-ARM/ble_find_me.uvoptx.uvoptx	uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
    - apps/ble_find_me/MDK-ARM/flash.sct		        Linker script.
  
@par Run the example
  - pre-condition
    - Connect board to PC with serial port cable.
    - Build the sample project with the command "./build.sh mt2523_hdk ble_find_me bl" under the SDK root folder on GCC, or 
      build the project ble_find_me.uvprojx under the MDK-ARM folder on Keil, then download the bin file to MT2523.

 - Step-by-step instructions on how to run the example project.
   - Install a BLE related application on the peer device, Like "LightBlue" on iphone.
   - Enter "LightBlue" APP in peer device, then make the BLE connection with LinkIt 2523 HDK.  
   - Write an alert level value in the operation UI, then LinkIt 2523 HDK will log this value.
     Note: you can filter "BLE_FMP" the result log, and "alert_level" for write result log.
  
  
  * @}
*/