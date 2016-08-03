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
  * @page hal_spi HAL SPI MASTER example project
  * @{

@par Overview

 - Example project description
   - This example project shows the API usage of SPI master module to send data.
 - Features of the example project
   - This example project demonstrates how to use SPI master APIs to send data with polling mode.
 - Output form the example
   - Waveform of the sending data shows on the SPI Pins.

@par Hardware and software environment

  - Supported platform
    -LinkIt 7687 HDK

  - HDK swithes, pin configurations
    - SPI master module pins mapping table are shown as below.
      | SPI Pin | GPIOx     |    PINx      |
      |-------  |---------  |------------  |
      |  CS_N   | GPIO_32   |    J34.8     |
      |  SCK    | GPIO_31   |    J34.5     |
      |  MOSI   | GPIO_29   |    J34.7     |
      |  MISO   | GPIO_30   |    J34.6     |

  - PC/environment configuration
   - Need an oscilloscope to capture the waveform.

@par Directory contents
  -Source and header files
   - hal_examples/spi_master_send_data_polling/src/main.c                      Main program.
   - hal_examples/spi_master_send_data_polling/src/system_mt7687.c             MT76x7's system clock configuration fil.e
   - hal_examples/spi_master_send_data_polling/inc/hal_feature_config.h        MT76x7's feature configuration file.
   - hal_examples/spi_master_send_data_polling/inc/flash_map.h                 MT76x7's memory layout symbol file.
   - hal_examples/spi_master_send_data_polling/GCC/startup_mt7687.s            MT76x7's startup file of GCC.
   - hal_examples/spi_master_send_data_polling/GCC/syscalls.c                  MT76x7's syscalls of GCC.
   - hal_examples/spi_master_send_data_polling/MDK-ARM/startup_mt7687.s        MT76x7's startup file.

  - Project configuration files using GCC
   - hal_examples/spi_master_send_data_polling/GCC/feature.mk                  Feature configuration file.
   - hal_examples/spi_master_send_data_polling/GCC/Makefile                    Makefile.
   - hal_examples/spi_master_send_data_polling/GCC/mt7687_flash.ld             Linker script.

  - Project configuration files using Keil
   - hal_examples/spi_master_send_data_polling/MDK-ARM/spi_master_send_data_polling.uvprojx             uVision5 Project File. Contains the project structure in XML format.
   - hal_examples/spi_master_send_data_polling/MDK-ARM/spi_master_send_data_polling.uvoptx              uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - hal_examples/spi_master_send_data_polling/MDK-ARM/flash.sct                                        Linker script.

  - Project configuration files using IAR
   - hal_examples/spi_master_send_data_polling/EWARM/spi_master_send_data_polling.ewd     IAR project options. Contains the settings for the debugger.
   - hal_examples/spi_master_send_data_polling/EWARM/spi_master_send_data_polling.ewp     IAR project file. Contains the project structure in XML format
   - hal_examples/spi_master_send_data_polling/EWARM/spi_master_send_data_polling.eww     IAR workspace file. Contains project information.
   - hal_examples/spi_master_send_data_polling/EWARM/flash.icf                            Linker script.

   
@par Run the example
  - Connect the board to the PC with the serial cable.
  - Build the example project with the command, "./build.sh mt7687_hdk spi_master_send_data_polling" under the SDK root folder and download the bin file to LinkIt 7687 development board.
  - Run the example. The waveform can be captured by the oscilloscope.

* @}
*/
