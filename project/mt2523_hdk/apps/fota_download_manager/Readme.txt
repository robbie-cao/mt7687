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
  * @page fota_download_manager FOTA example project
  * @{

@par Overview

 - Example project description
   - This example demonstrates the basic workflow of full binary FOTA update, including how to use the interface of Bluetooth notification service, triggering FOTA update process, retrieving FOTA update result. It also contains an example demonstrating GNSS firmware update.
 - Features of the application
   - This example project demonstrates how to use Bluetooth notification service to receive FOTA package and trigger update on the target board.

@par Hardware and software environment

  - Supported platform
    - LinkIt 2523 HDK.
    - Android smart phone(Android OS version 4.0+)


  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - apps/fota_download_manager/src/main.c                   Main program
   - apps/fota_download_manager/src/system_mt2523.c          MT2523x's system clock configuration file
   - apps/fota_download_manager/src/fota_main.c              Download manager main program, connect with smart device application and exchange data through btnotify service.
   - apps/fota_download_manager/src/ept_gpio_var.c           GPIO pin assignment with variable names.
   - apps/fota_download_manager/src/ept_eint_var.c           EINT assignment file with variable names.
   - apps/fota_download_manager/inc/hal_feature_config.h     MT2523x's feature configuration file.
   - apps/fota_download_manager/inc/hal_gpio_ept_config.h    File to map the EPT generated macros to 32-bits data.
   - apps/fota_download_manager/inc/memory_map.h             MT2523x's memory layout symbol file.
   - apps/fota_download_manager/inc/ept_eint_drv.h           EINT basic configuration header file.
   - apps/fota_download_manager/inc/ept_gpio_drv.h           GPIO pin assignment and basic configuration file.
   - apps/fota_download_manager/inc/ept_keypad_drv.h         Keypad basic configuration header file.
   - apps/fota_download_manager/GCC/startup_mt2523.s         MT2523x's startup file for GCC.
   - apps/fota_download_manager/GCC/syscalls.c               MT2523x's syscalls for GCC.
   - apps/fota_download_manager/MDK-ARM/startup_mt2523.s     MT2523x's startup file for Keil.
   - apps/fota_download_manager/src/btnotify_src             BT notify service related program
   - apps/fota_download_manager/src/gnss_src                 GNSS firmware update program, handshake with GNSS and burn new firmware after downloading finished.

  - Project configuration files using GCC.
   - apps/fota_download_manager/GCC/feature.mk               Feature configuration.
   - apps/fota_download_manager/GCC/Makefile                 Makefile.
   - apps/fota_download_manager/GCC/flash.ld                 Linker script.

  - Project configuration files using Keil.
   - apps/fota_download_manager/MDK-ARM/fota_download_manager.uvprojx    uVision5 project File. Contains the project structure in XML format.
   - apps/fota_download_manager/MDK-ARM/fota_download_manager.uvoptx     uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - apps/fota_download_manager/MDK-ARM/flash.sct                        Linker script.

@par Run the example
  - Build the example project with the command, "./build.sh mt2523_hdk fota_download_manager bl" under the SDK root folder and download the binary file to LinkIt 2523 development board.
  - Install the application named "smartdevice.apk" on Android smartphone, and launch the application.
  - Power on LinkIt 2523 HDK.
  - Click "scan" item to start Bluetooth scanning in the application, find LinkIt 2523 device named "2523_FOTATEST" in scanned list, and then click this name.
  - Wait for connection established between the smartphone and LinkIt 2523 device, then click "Firmware Update(FBIN)" item in drop-down menu appeared at the top-right corner of screen.
  - Select the FOTA package file from the smartphone file system, then the application will start to push FOTA package to LinkIt 2523 device and enter FOTA update process.


@par Configuration
  - None.

* @}
*/

