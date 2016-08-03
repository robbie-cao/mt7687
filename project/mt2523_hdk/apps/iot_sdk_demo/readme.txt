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
  * @page iot sdk demo application
  * @{

@par Overview

 - Application description
   - This demo application demonstrates five key features with UI.
 - Features of the application
   - GPS: Demonstrates GPS basic function and dynamically displays the position information.
   - BT audio: Demonstrates EDR(a2dp, avrcp, hfp) basic function, including music control and call control.
   - BLE PXP: Demonstrates PXP profile's basic function(out of range alert).
   - Sensor Subsystem: Demonstrates how to obtain sensor data from physical sensors and virtual sensors on sensor DTB, display the sensor data on smart phone via Bluetooth transmission.
   - Watch face: Demonstrates the low power scenario of LCM partial update.

@par Hardware and Software environment

  - Supported platform
    - LinkIt 2523 HDK
    - LCM module driven by Sitronix ST7789H2 (Or the other driver IC which can support MTK 2-data lane).

  - HDK swithes, pin config
    - LCD module pin mapping is listed below.
  |PIN Name	|GPIOx	|PINx	|
  |-------	|--------- 	|---------	|
  |  LSRSTB	|  38	| |
  | LSCE0_B0|  39	| |
  |  LSCK0	|  40	|	|
  |  LSDA0	|  41	|	|
  | LSA0DA0 |  42	|	|
  |  LPTE 	|  43	|	|

  - PC/environment config
   - Need a serial tool(such as hyper terminal) for UART logging.
   - Com port setting: baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - project\mt2523_hdk\apps\iot_sdk_demo\inc					Common header file.
   - project\mt2523_hdk\apps\iot_sdk_demo\src\gnss_screen			GPS demo source code.
   - project\mt2523_hdk\apps\iot_sdk_demo\src\bt_audio				BT audio source code.
   - project\mt2523_hdk\apps\iot_sdk_demo\src\pxp_screen			BLE PXP source code.
   - project\mt2523_hdk\apps\iot_sdk_demo\src\sensor_demo			Sensor algorithm demo source code.
   - project\mt2523_hdk\apps\iot_sdk_demo\src\watch_face			Watch face demo source code.
   - project\mt2523_hdk\apps\iot_sdk_demo\src\graphicLib			GDI interface source code.

  - Project configuration files using GCC
   - project/mt2523_hdk/apps/iot_sdk_demo/GCC/feature.mk   		Feature configuration file.
   - project/mt2523_hdk/apps/iot_sdk_demo/GCC/Makefile			Make file.
   - project/mt2523_hdk/apps/iot_sdk_demo/GCC/flash.ld			Linker script.
   - project/mt2523_hdk/apps/iot_sdk_demo/GCC/gnss_screen.mk		GNSS demo app makefile.
   - project/mt2523_hdk/apps/iot_sdk_demo/GCC/bt_audio.mk		bt_audio demo demo app makefile.
   - project/mt2523_hdk/apps/iot_sdk_demo/GCC/graphicLib.mk		graphicLib demo app makefile.
   - project/mt2523_hdk/apps/iot_sdk_demo/GCC/sensor_demo.mk		sensor algorithm demo app makefile.

  - Project configuration files using Keil
   - project/mt2523_hdk/apps/iot_sdk_demo/MDK-ARM/iot_sdk_demo.uvoptx	uVision5 Project File. Contains the project structure in XML format.
   - project/mt2523_hdk/apps/iot_sdk_demo/MDK-ARM/iot_sdk_demo.uvprojx	uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - project/mt2523_hdk/apps/iot_sdk_demo/MDK-ARM/flash.sct		Linker script.
   
  - Project configuration files using IAR
   - project/mt2523_hdk/apps/iot_sdk_demo/EWARM/iot_sdk_demo.ewd  Project options
   - project/mt2523_hdk/apps/iot_sdk_demo/EWARM/iot_sdk_demo.ewp  Project file
   - project/mt2523_hdk/apps/iot_sdk_demo/EWARM/iot_sdk_demo.eww  Workspace file
   - project/mt2523_hdk/apps/iot_sdk_demo/EWARM/flash.sct  Linker script


@par Run the application
  - How to build the iot_sdk_demo application
	- GCC version
		-make command "./build.sh mt2523_hdk iot_sdk_demo" under the SDK root folder
	- Keil
		-Open "iot_sdk_demo.uvprojx" under the MDK-ARM folder and build the keil project
	- IAR
		-Open "iot_sdk_demo.ewp" under the EWARM folder and build the IAR project

  - How to run each feature in the demo application
	- Please refer to the readme file in each sub-folder of the example project(e.g. GNSS demo application folder: project\mt2523_hdk\apps\iot_sdk_demo\src\gnss_screen)

* @}
*/
