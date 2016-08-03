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
  * @page bootloader bootloader
  * @{

@par Overview

This is not a example project. This project is a bootloader for LinkIt 2523 HDK. The developer can refer this project to understand how to build a bootloader.

@par Hardware and software environment

  - Supported platform
    - LinkIt 2523 HDK.

  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - driver/board/mt25x3_hdk/bootloader/core/src/bl_main.c           Main program
   - apps/bootloader/inc/hal_feature_config.h                        MT2523x's feature configuration file.
   - apps/bootloader/GCC/startup_mt2523.s                            MT2523x's startup file for GCC.
   - apps/bootloader/GCC/syscalls.c                                  MT2523x's syscalls for GCC.
   - apps/bootloader/MDK-ARM/startup_mt2523.s                        MT2523x's startup file for Keil.
   - apps/bootloader/MDK-ARM/hal_log.h                               Delcalre HAL log function in bootloader for Keil.
   - apps/bootloader/src/custom_fota.c                               Fota update function customization.

  - Project configuration files using GCC.
   - apps/bootloader/GCC/feature.mk                                  Feature configuration.
   - apps/bootloader/GCC/Makefile                                    Makefile.
   - apps/bootloader/GCC/bootloader.ld                               Linker script.

  - Project configuration files using Keil.
   - apps/bootloader/MDK-ARM/bootloader.uvprojx                      uVision5 project File. Contains the project structure in XML format.
   - apps/bootloader/MDK-ARM/bootloader.uvoptx                       uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - apps/bootloader/MDK-ARM/ram.sct                                 Scatter file.

@par Run the example
  - Connect board to the PC with serial port cable.
  - Build bootloader and an example project with the command, "./build.sh mt2523_hdk example_project_name bl" under the SDK root folder and download the binary file to LinkIt MT2523 HDK.
  - Power on LinkIt 2523 HDK and bootloader log are showen on serial tool.

* @}
*/
