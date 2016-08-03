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
  * @page Graphic interface introduction
  * @{

@par Overview

 - Application description
   - This UI interface provides the low level graphic function and provides a wrapper for touch event.
 - Features of the application
   - Simple draw shape function such as draw point, draw line, draw rect.
   - Support draw ASCII font character with fixed font size 20.
   - Show main menu on main screen.
 - Output from the application
   - Show a list of demo items.

@par Hardware and software environment
  - Please refer to the readme document on project\mt2523_hdk\apps\iot_sdk_demo

@par Directory contents
  - Source and header files
   - project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib/FontRes.c                     Font data
   - project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib/gd_primitive16.c              Put pixel function.
   - project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib/gdi_font.c   Draw font function
   - project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib/gdi_layer.c   Frame buffer operation.
   - project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib/gdi_lcd.c     LCM driver wrapper
   - project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib/gdi_primitive.c   Draw shape funtion.
   - project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib/main_screen.c     Draw list menu.
   - project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib/PixcomFontEngine.c  Show string interface
   - project/mt2523_hdk/apps/iot_sdk_demo/src/graphicLib/ui_demo_task.c   Main task

  - Project configuration files using GCC
   - project/mt2523_hdk/apps/iot_sdk_demo/GCC/graphicLib.mk

@par Run this application
  - Build the application with the command, "./build.sh mt2523_hdk iot_sdk_demo bl" under the SDK root folder.
  - Download the bin file to MT2523G.
  - Power on 2523G HDK, main screen will be shown.

* @}
*/