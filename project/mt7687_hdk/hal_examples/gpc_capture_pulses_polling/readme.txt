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
  * @page hal_gpc GPC example project
  * @{

@par Overview

 - Example project description
   - This example project shows the usage of APIs of the GPC module.
 - Features of the example project
   - This example project demonstrates GPC module usage by generating pulse signal from GPIO33, and using GPC module to count the generated pulses.
 - Input to the example
   - Pulses, generated on GPIO33, for GPC module to capture.
 - Output from the example
   - The log will display the captured pulse number.

@par Hardware and software environment

  - Supported platform
    - LinkIt 7687 HDK.

  - HDK switches and pin configuration
    - GPC module mapping is listed below, connect pin GPIO33(Pin J35.3) to GPIO6(Pin J35.4).
      |CHx  |GPIOx  |PINx    |   |GPIOx |PINx    |
      |-----|-------|--------|   |------|--------|
      |  0  |  33   |J35.3   |==>|  6   |J35.4   |



  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - hal_examples/gpc_capture_pulses_polling/src/main.c                 Main program.
   - hal_examples/gpc_capture_pulses_polling/src/system_mt7687.c        MT76x7's clock configuration file.
   - hal_examples/gpc_capture_pulses_polling/inc/hal_feature_config.h   MT76x7's feature configuration file.
   - hal_examples/gpc_capture_pulses_polling/inc/flash_map.h            MT76x7's memory layout symbol file.
   - hal_examples/gpc_capture_pulses_polling/GCC/startup_mt7687.s       MT76x7's startup file for GCC.
   - hal_examples/gpc_capture_pulses_polling/GCC/syscalls.c             MT76x7's syscalls for GCC.
   - hal_examples/gpc_capture_pulses_polling/MDK-ARM/startup_mt7687.s   MT76x7's startup file for Keil.
   - hal_examples/gpc_capture_pulses_polling/EWARM/startup_mt7687.s	MT7687's startup file for IAR.  

  - Project configuration files using GCC
   - hal_examples/gpc_capture_pulses_polling/GCC/feature.mk             Feature configuration.
   - hal_examples/gpc_capture_pulses_polling/GCC/Makefile               Makefile.
   - hal_examples/gpc_capture_pulses_polling/GCC/mt7687_flash.ld        Linker script.

  - Project configuration files using Keil
   - hal_examples/gpc_capture_pulses_polling/MDK-ARM/gpc_capture_pulses_polling.uvprojx     uVision5 project file. Contains the project structure in XML format.
   - hal_examples/gpc_capture_pulses_polling/MDK-ARM/gpc_capture_pulses_polling.uvoptx      uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - hal_examples/gpc_capture_pulses_polling/MDK-ARM/flash.sct                      Linker script.

  - Project configuration files using IAR
   - hal_examples/gpc_capture_pulses_polling/EWARM/gpc_capture_pulses_polling.ewd          IAR project options. Contains the settings for the debugger.
   - hal_examples/gpc_capture_pulses_polling/EWARM/gpc_capture_pulses_polling.ewp          IAR project file. Contains the project structure in XML format
   - hal_examples/gpc_capture_pulses_polling/EWARM/gpc_capture_pulses_polling.eww          IAR workspace file. Contains project information.
@par Run the example
  - Connect board to PC with serial port cable.
  - Build the example project with the command, "./build.sh mt7687_hdk gpc_capture_pulses_polling" from the SDK root folder and download the bin file to LinkIt 7687 development board.
  - Connect corresponding pin as described above.
  - Run the example. The log will dispaly the pulse number captured by GPC.

* @}
*/

