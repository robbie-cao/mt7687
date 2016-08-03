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
  * @page template project guidelines
  * @{

@par Overview

 - Template project description
   - This project is a template project and it runs without FreeRTOS.
 - Features of the project
   - This project shows how a simple appliation runs up.
 - Input to the example
   - N/A.
 - Output from the example
   - a welcome message will be showed in log terminal window.

@par Hardware and software environment

  - Supported platform
    - MediaTek LinkIt 7687 HDK.

  - HDK switches and pin configuration
    - N/A

  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - template/src/main.c  			Main program
   - template/src/system_mt2523.c		MT76x7's clock configuration file.
   - template/inc/hal_feature_config.h 		MT76x7's feature configuration file.
   - template/inc/flash_map.h		        MT76x7's memory layout symbol file.
   - template/GCC/startup_mt2523.s		MT76x7's start up file for GCC.
   - template/GCC/syscalls.c			MT76x7's syscalls for GCC.
   - template/MDK-ARM/startup_mt2523.s	        MT76x7's start up file for Keil.

  - Project configuration files using GCC
   - template/GCC/feature.mk		    	Feature configuration.
   - template/GCC/Makefile		 	Makefile.
   - template/GCC/mt7687_flash.ld		Linker script.
  - Project configuration files using Keil
   - template/MDK-ARM/template.uvprojx		uVision5 Project File. Contains the project structure in XML format.
   - template/MDK-ARM/template.uvoptx		uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - template/MDK-ARM/flash.sct		        Linker script.

@par Run the example
  - Connect board to PC with serial port cable.
  - Build the project with the command, "./build.sh mt7687_hdk template" from the SDK root folder and download the bin file to LinkIt 7687 development board.
  - Run the example, the terminal window will show "welcome" message.

* @}
*/
