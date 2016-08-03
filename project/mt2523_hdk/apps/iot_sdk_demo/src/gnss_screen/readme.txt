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
  * @page GNSS application
  * @{

@par Overview

 - Application description
   - This is a simple GNSS application with UI display. It demonstrates the usage of GNSS module APIs.
 - Features of the application
   - This application shows how to send commands to GNSS module and recieve/parser NMEA data from GNSS module.
   - UI display with GNSS positioning information such as longitude, latitude, etc.
 - Output from the application
   - Positioning information on screen.

@par Hardware and software environment
  - Please refer to the readme document on project\mt2523_hdk\apps\iot_sdk_demo

@par Directory contents
  - Source and header files
   - project/mt2523_hdk/apps/iot_sdk_demo/src/gnss_screen/epo_demo.c                   EPO sample code.
   - project/mt2523_hdk/apps/iot_sdk_demo/src/gnss_screen/gnss_app.                    GNSS app task.
   - project/mt2523_hdk/apps/iot_sdk_demo/src/gnss_screen/gnss_demo_screen.c           GNSS UI and NMEA data parser.
   - project/mt2523_hdk/apps/iot_sdk_demo/src/gnss_screen/gnss_ring_buffer.c           Ring buffer for furthur usage
   - project/mt2523_hdk/apps/iot_sdk_demo/src/gnss_screen/gnss_timer.c                 RTOS timer wrapper

  - Project configuration files using GCC
   - project/mt2523_hdk/apps/iot_sdk_demo/GCC/gnss_screen.mk

@par Run the GNSS application
  - Build this application with the command, "./build.sh mt2523_hdk iot_sdk_demo bl" under the SDK root folder.
  - Download the bin file to MT2523G.
  - Power on LinkIt 2523 HDK, main screen will be shown.
  - Press "GNSS demo" menu item to enter GNSS screen.
  - GNSS screen will show "No fix" at beginning when GNSS receiver just starts working.
  - Wait for more than one minitue under open sky, Latitude and Longitude will be shown after the GNSS acquires the positioning information successfully.
  - Press "Exit" button to exit the GNSS Screen.

* @}
*/