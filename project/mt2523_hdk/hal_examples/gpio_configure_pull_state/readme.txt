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
  * @page hal_gpio GPIO example project
  * @{

@par Overview

 - Example project description
   - This example project shows the useage of APIs of GPIO module and it runs without FreeRTOS.
 - Features of the example project
   - This example project demonstrates how to use GPIO APIs to pull up and pull down GPIO1 and how to get input data after pull state is changed.
 - Output from the example
   - The log will show whether the pull state has changed successfully.

@par Hardware and software environment

  - Supported platform
    - LinkIt 2523 HDK

  - HDK switches and pin configurations
    - Make sure GPIO1 is disconnected.

  - PC/environment configurations
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - hal_examples/gpio_configure_pull_state/src/main.c  			Main program
   - hal_examples/gpio_configure_pull_state/src/system_mt2523.c		        MT2523x's system clock configuration file.
   - hal_examples/gpio_configure_pull_state/inc/hal_feature_config.h 	        MT2523x's feature configuration file.
   - hal_examples/gpio_configure_pull_state/inc/memory_map.h		        MT2523x's memory layout symbol file.
   - hal_examples/gpio_configure_pull_state/GCC/startup_mt2523.s		MT2523x's startup file for GCC.
   - hal_examples/gpio_configure_pull_state/GCC/syscalls.c			MT2523x's syscalls for GCC.
   - hal_examples/gpio_configure_pull_state/MDK-ARM/startup_mt2523.s	        MT2523x's startup file for Keil.

  - Project configuration files using GCC
   - hal_examples/gpio_configure_pull_state/GCC/feature.mk		    	Feature configuration file.
   - hal_examples/gpio_configure_pull_state/GCC/Makefile		 	Makefile.
   - hal_examples/gpio_configure_pull_state/GCC/flash.ld			Linker script.

  - Project configuration files using Keil
   - hal_examples/gpio_configure_pull_state/MDK-ARM/gpio_configure_pull_state.uvprojx		uVision5 Project File. Contains the project structure in XML format.
   - hal_examples/gpio_configure_pull_state/MDK-ARM/gpio_configure_pull_state.uvoptx		uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - hal_examples/gpio_configure_pull_state/MDK-ARM/flash.sct		                        Linker script.

  - Project configuration files using IAR
   - hal_examples/gpio_configure_pull_state/EWARM/gpio_configure_pull_state.ewd	    IAR project options. Contains the settings for the debugger.
   - hal_examples/gpio_configure_pull_state/EWARM/gpio_configure_pull_state.ewp     IAR project file. Contains the project structure in XML format
   - hal_examples/gpio_configure_pull_state/EWARM/gpio_configure_pull_state.eww     IAR workspace file. Contains project information.
   - hal_examples/gpio_configure_pull_state/EWARM/flash.icf			    Linker script.

@par Run the example
  - Connect board to PC with serial port cable.
  - Build the example project with the command, "./build.sh mt2523_hdk gpio_configure_pull_state" under the SDK root folder and download the bin file to LinkIt 2523 development board.
  - Disconnect GPIO1.
  - Run the example. The log will show whether the pull state has changed successfully.

* @}
*/
