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
  * @page hal_i2s HAL I2S example project
  * @{

@par Overview

  - Example project description
    - This example project shows the usage of APIs of I2S module and it runs without FreeRTOS.
  - Features of the example project
    - This example project demonstrates how to use I2S APIs to transmit audio digital signals through the LinkIt 2523 HDK.
      This example project connects the I2S TX and RX links via internal loopback mode, which does not need to connect to any external DAC components.
      The special sine-wave signals sent by the I2S TX link will be looped back to the I2S RX link.
  - Process / procedure of the example
    - Open flow
      - Initialize I2S module
      - Configure I2S setting
      - Enable TX and RX links to transmit/receive signals
    - Set 3000ms time delay to the system. The main flow pauses while the I2S hardware transmits the data at the period of delay.
    - Close flow
      - Disable TX and RX links
      - Deinitialize I2S module
    - Dump received data via UART port
  - Input to the example
    - Data in i2s_tone2k_32ksr array: 2 kHz sine wave audio frequency tone under 32kHz sample rate.
      0xFFF4  0x30F0  0x5A79  0x763C  0x7FFF  0x7646  0x5A8B  0x3107
      0x000C  0xCF10  0xA587  0x89C4  0x8001  0x89BB  0xA576  0xCEF9
      0xFFF3  0x30F0  0x5A79  0x763C  0x7FFF  0x7645  0x5A8A  0x3106
      0x000C  0xCF10  0xA587  0x89C4  0x8001  0x89BB  0xA576  0xCEF9
    - This example project will send the data out repeatedly.
  - Output from the example
    - A transmission delay from the TX to the RX link is unavoidable when the internal loopback mode is set. The first few samples
      of the RX link are expected to be zero before receiving valid samples. Because of this, the example records samples after several
      transmissions. Since the recorded data is extracted from a series of transmissions, it would not start from the beginning
      of i2s_tone2k_32ksr, but follows the pattern below.
      0x89bb  0xa576  0xcef9  0xfff3  0x30ef  0x5a78  0x763b  0x7ffe
      0x7644  0x5a89  0x3105  0x000b  0xcf10  0xa587  0x89c4  0x8001
      0x89bb  0xa576  0xcef9  0xfff4  0x30ef  0x5a78  0x763b  0x7ffe
      0x7645  0x5a8a  0x3106  0x000b  0xcf10  0xa587  0x89c4  0x8001
      The output through I2S interface would not be exactly the same (not bit-true) as the input owing to the slight gain adjustment from the hardware.
      However, the audio performance would not be affected at all.

  - Limitation of the example
    To simplify the example project, raw data received from the RX link will be shown via the log.

@par Hardware and software environment

  - Supported platform
    - LinkIt 2523 HDK.

  - HDK switches and pin configuration
    - N/A

  - PC/environment configuration
    - A serial tool is required, such as hyper terminal, for UART logging.
    - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
    - hal_examples/i2s_internal_loopback/src/main.c                          Main program.
    - hal_examples/i2s_internal_loopback/src/system_mt2523.c                 MT2523x's clock configuration file.
    - hal_examples/i2s_internal_loopback/inc/hal_feature_config.h            MT2523x's feature configuration file.
    - hal_examples/i2s_internal_loopback/inc/memory_map.h                    MT2523x's memory layout symbol file.
    - hal_examples/i2s_internal_loopback/GCC/startup_mt2523.s                MT2523x's startup file for GCC.
    - hal_examples/i2s_internal_loopback/GCC/syscalls.c                      MT2523x's syscalls for GCC.
    - hal_examples/i2s_internal_loopback/MDK-ARM/startup_mt2523.s            MT2523x's startup file for Keil.

  - Project configuration files using GCC
     - hal_examples/i2s_internal_loopback/GCC/feature.mk                    Feature configuration.
     - hal_examples/i2s_internal_loopback/GCC/Makefile                      Makefile.
     - hal_examples/i2s_internal_loopback/GCC/flash.ld                      Linker script.

  - Project configuration files using Keil
     - hal_examples/i2s_internal_loopback/MDK-ARM/i2s_internal_loopback.uvprojx     uVision5 project file. Contains the project structure inXML format.
     - hal_examples/i2s_internal_loopback/MDK-ARM/i2s_internal_loopback.uvoptx      uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
     - hal_examples/i2s_internal_loopback/MDK-ARM/flash.sct                         Linker script.

  - Project configuration files using IAR
   - hal_examples/i2s_internal_loopback/EWARM/i2s_internal_loopback.ewd     IAR project options. Contains the settings for the debugger.
   - hal_examples/i2s_internal_loopback/EWARM/i2s_internal_loopback.ewp     IAR project file. Contains the project structure in XML format
   - hal_examples/i2s_internal_loopback/EWARM/i2s_internal_loopback.eww     IAR workspace file. Contains project information.
   - hal_examples/i2s_internal_loopback/EWARM/flash.icf                     Linker script.

@par Run the example
  - Connect board to PC with serial port cable.
  - Build the example project with the command, "./build.sh mt2523_hdk i2s_internal_loopback bl" from the SDK root folder and download the bin file to LinkIt 2523 development board.
  - Run the example.
    The log will show the raw data from RX link. Please refer to "Input to the example" and "Output from the example" section.

* @}
*/
