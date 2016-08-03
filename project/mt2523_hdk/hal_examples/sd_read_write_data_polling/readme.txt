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
  * @page hal_sd SD example project
  * @{

@par Overview

 - Example project description
   - This example project shows the useage of APIs for the SD module and it runs without FreeRTOS.
 - Features of the example project
   - This example project demonstrate how to read data from the SD card or
   write data to the SD card in the polling mode(MCU mode).
 - Process / procedure of the example
   - Set the GPIO pinmux.
   - Initialize the MSDC and the SD card.
   - Write data to the SD card in the MCU mode.
   - Read data from the SD card in the MCU mode.
   - Compare read and write data.

@par Hardware and software environment

  - Supported platform
    - LinkIt 2523 HDK.

  - HDK switches and pin configuration
    - Corresponding pin of SD is J1001 to J1006,and it should jump pin2 to pin3.

  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - hal_examples/sd_read_write_data_polling/src/main.c                         Main program.
   - hal_examples/sd_read_write_data_polling/src/system_mt2523.c                MT2523x's clock configuration file.
   - hal_examples/sd_read_write_data_polling/inc/hal_feature_config.h           MT2523x's feature configuration file.
   - hal_examples/sd_read_write_data_polling/inc/memory_map.h                   MT2523x's memory layout symbol file.
   - hal_examples/sd_read_write_data_polling/GCC/startup_mt2523.s               MT2523x's startup file for GCC.
   - hal_examples/sd_read_write_data_polling/GCC/syscalls.c                     MT2523x's syscalls for GCC.
   - hal_examples/sd_read_write_data_polling/MDK-ARM/startup_mt2523.s           MT2523x's startup file for Keil.

  - Project configuration files using GCC
   - hal_examples/sd_read_write_data_polling/GCC/feature.mk                     Feature configuration.
   - hal_examples/sd_read_write_data_polling/GCC/Makefile                       Makefile.
   - hal_examples/sd_read_write_data_polling/GCC/flash.ld                       Linker script.

  - Project configuration files using Keil
   - hal_examples/sd_read_write_data_polling/MDK-ARM/sd.uvprojx                 uVision5 project file. Contains the project structure in XML format.
   - hal_examples/sd_read_write_data_polling/MDK-ARM/sd.uvoptx                  uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - hal_examples/sd_read_write_data_polling/MDK-ARM/flash.sct                  Linker script.

@par Run the example
  - Connect board to PC with serial port cable.
  - Build the example project with the command, "./build.sh mt2523_hdk sd_read_write_data_polling" from the SDK root folder and download the bin file to LinkIt 2523 development board.
  - Insert the SD card on the LinkIt 2523 development board.
  - Run the example. The log will show if it was successful or not.

* @}
*/
