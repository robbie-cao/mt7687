/* Copyright Statement:
 *
 * @2015 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek Inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE.
 */

/**
  * @page gnss_get_location GNSS example project
  * @{

@par Overview
 - Example project description
  - This example shows how to implement a location based application.
 - Features of the example
  - This example demonstrates how to use GNSS interface.
  - This example shows how to download EPO data via BT and how to use this data to shorten the time to get the position.
  - This example aslo shows how to pass GNSS log to PC through UART interface.

@par Hardware and software environment
  - Supported platform
   - LinkIt 2523G HDK.
   - Android smartphone(Android OS version 4.0+) for EPO download
  - Test enviroment
   - Ensure to run the example in an open area where the GNSS signal can be received. Otherwise, the device cannot get a location.
  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.
  - Macro
   - MTK_GNSS_ENABLE This macro should be enabled for GNSS middleware interface usage.
   - MTK_GNSS_SUPPORT_EPO_DOWNLOAD_BY_BT This macro should be enabled for EPO feature in this example project.

@par Directory contents
  - Source and header files
   - project/mt2523_hdk/apps/gnss_get_location/src/epo_demo.c                        Parse EPO data and send relate data to GNSS module.
   - project/mt2523_hdk/apps/gnss_get_location/src/epo_download.c                    Download EPO data form smartphone via BT.
   - project/mt2523_hdk/apps/gnss_get_location/src/ept_eint_var.c                    EINT EPT variable.
   - project/mt2523_hdk/apps/gnss_get_location/src/ept_gpio_var.c                    GPIO EPT variable.
   - project/mt2523_hdk/apps/gnss_get_location/src/gnss_app.c                        Main task for GNSS example.
   - project/mt2523_hdk/apps/gnss_get_location/src/gnss_bridge_task.c                The task that communicates with PC to output logs and receive commands.
   - project/mt2523_hdk/apps/gnss_get_location/src/gnss_ring_buffer.c                Ring buffer used to catch debug data.
   - project/mt2523_hdk/apps/gnss_get_location/src/gnss_timer.c                      Wrapper for freeRTOS timer.
   - project/mt2523_hdk/apps/gnss_get_location/src/gnss_uart_bridge.c                Wrapper for UART 2 which connects with PC, used by GNSS bridge task.
   - project/mt2523_hdk/apps/gnss_get_location/src/main.c                            Entry function for this example project.
   - project/mt2523_hdk/apps/gnss_get_location/src/system_mt2523.c                   MT2523x's system clock configuration file.
   - project/mt2523_hdk/apps/gnss_get_location/src/btnotify_src                      BT notification service related program.
   - project/mt2523_hdk/apps/gnss_get_location/inc/FreeRTOSConfig.h                  Kernel config file.
   - project/mt2523_hdk/apps/gnss_get_location/inc/btnotify_inc                      BT notification service relate header file.

  - Project configuration files using GCC.
   - project/mt2523_hdk/apps/gnss_get_location/GCC/Makefile                          Makefile.
   - project/mt2523_hdk/apps/gnss_get_location/GCC/feature.mk                        Feature configuration.
   - project/mt2523_hdk/apps/gnss_get_location/GCC/flash.ld                          Linker script.
   - project/mt2523_hdk/apps/gnss_get_location/GCC/startup_mt2523.s                  Startup file.
   - project/mt2523_hdk/apps/gnss_get_location/GCC/syscalls.c                        Syscalls for GCC.

  - Project configuration files using Keil.
   - project/mt2523_hdk/apps/gnss_get_location/MDK-ARM/gnss_get_location.uvprojx     uVision5 project File. Contains the project structure in XML format.
   - project/mt2523_hdk/apps/gnss_get_location/MDK-ARM/gnss_get_location.uvoptx      uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - project/mt2523_hdk/apps/gnss_get_location/MDK-ARM/flash.sct                     Linker script.

@par Run the example
  - Build the example project with the command, "./build.sh mt2523_hdk gnss_get_location" from the SDK root folder and download the bin file to LinkIt 2523G HDK.
  - Connect board to the PC with serial port cable.
  - Run the example. The board starts to search for location through the GNSS.
  - Install smart device application on an Android smartphone.
  - Make sure Bluetooth is on and the network is availble, then launch smart device application and connect to LinkIt 2523G HDK.
  - EPO file will be downloaded to LinkIt 2523G HDK automatically.

* @}
*/


