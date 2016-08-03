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
  * @page hal_spi HAL SPI SLAVE example project
  * @{

@par Overview

 - Example project description
   - This example project shows how to use the SPI slave HAL APIs with the SPI master module and it runs without FreeRTOS.
 - Features of the example project
   - This example project demonstrates how to use SPI slave API to communicate with SPI master with one LinkIt 7687 HDK.
   - This example project hardware connection as shown below.
   _________________________ 
  |           ______________|
  |          |SPI Master    |
  |          |              |
  |          |   CS_N(GPIO7)|____________
  |          |              |           |
  |          |   SCK(GPIO26)|_________  |
  |          |              |        |  |
  |          |  MOSI(GPIO24)|______  |  |
  |          |              |     |  |  |
  |          |  MISO(GPIO25)|___  |  |  |
  |          |______________|  |  |  |  |
  |                         |  |  |  |  |
  |           ______________|  |  |  |  |
  |          |SPI Slave     |  |  |  |  |
  |          |              |  |  |  |  |
  |          |  MISO(GPIO30)|__|  |  |  |
  |          |              |     |  |  |
  |          |  MOSI(GPIO29)|_____|  |  |
  |          |              |        |  |
  |          |   SCK(GPIO31)|________|  |
  |          |              |           |
  |          |  CS_N(GPIO32)|___________|
  |          |______________|
  |                         |
  |_MT7687__________________|

@par Hardware and software environment

  - Supported platform
    -LinkIt 7687 HDK

  - HDK switches and pin configuration
    - Connect J34.5 to J36.6
    - Connect J34.6 to J36.5
    - Connect J34.7 to J36.4
    - Connect J34.8 to J36.3

  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - hal_examples/spi_slave_receive_data_from_spi_master/src/main.c                      Main program.
   - hal_examples/spi_slave_receive_data_from_spi_master/src/system_mt7687.c             MT76x7's system clock configuration fil.e
   - hal_examples/spi_slave_receive_data_from_spi_master/inc/hal_feature_config.h        MT76x7's feature configuration file.
   - hal_examples/spi_slave_receive_data_from_spi_master/inc/flash_map.h                 MT76x7's memory layout symbol file.
   - hal_examples/spi_slave_receive_data_from_spi_master/GCC/startup_mt7687.s            MT76x7's startup file of GCC.
   - hal_examples/spi_slave_receive_data_from_spi_master/GCC/syscalls.c                  MT76x7's syscalls of GCC.
   - hal_examples/spi_slave_receive_data_from_spi_master/MDK-ARM/startup_mt7687.s        MT76x7's startup file.

  - Project configuration files using GCC
   - hal_examples/spi_slave_receive_data_from_spi_master/GCC/feature.mk                  Feature configuration file.
   - hal_examples/spi_slave_receive_data_from_spi_master/GCC/Makefile                    Makefile.
   - hal_examples/spi_slave_receive_data_from_spi_master/GCC/mt7687_flash.ld             Linker script.

  - Project configuration files using Keil
   - hal_examples/spi_slave_receive_data_from_spi_master/MDK-ARM/spi_slave_receive_data_from_spi_master.uvprojx             uVision5 Project File. Contains the project structure in XML format.
   - hal_examples/spi_slave_receive_data_from_spi_master/MDK-ARM/spi_slave_receive_data_from_spi_master.uvoptx              uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - hal_examples/spi_slave_receive_data_from_spi_master/MDK-ARM/flash.sct                                                  Linker script.

  - Project configuration files using IAR
   - hal_examples/spi_slave_receive_data_from_spi_master/EWARM/spi_slave_receive_data_from_spi_master.ewd     IAR project options. Contains the settings for the debugger.
   - hal_examples/spi_slave_receive_data_from_spi_master/EWARM/spi_slave_receive_data_from_spi_master.ewp     IAR project file. Contains the project structure in XML format
   - hal_examples/spi_slave_receive_data_from_spi_master/EWARM/spi_slave_receive_data_from_spi_master.eww     IAR workspace file. Contains project information.
   - hal_examples/spi_slave_receive_data_from_spi_master/EWARM/flash.icf                                      Linker script.

@par Run the example
  - Connect the board to the PC with the serial cable.
  - Build the example project with the command, "./build.sh mt7687_hdk spi_slave_receive_data_from_spi_master" under the SDK root folder and download the bin file to LinkIt 7687 development board.
  - Run the example. The log will show the communication result with SPI master.

* @}
*/
