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
   - This example project demonstrates how to use SPI slave API to communicate with SPI master with two LinkIt 2523 HDKs.
   - This example project: spi_slave_receive_data_two_boards for LinkIt 2523 HDK works as SPI Slave, as showm below.
   - The SPI master example project: spi_master_send_data_two_boards for LinkIt 2523 HDK works as SPI Master, as shown below.
   _________________________                        _________________________
  |           ______________|                      |______________           |
  |          |SPI Master    |                      |     SPI Slave|          |
  |          |              |                      |              |          |
  |          |  CS_N(GPIO25)|______________________|(GPIO25)CS_N  |          |
  |          |              |                      |              |          |
  |          |   SCK(GPIO26)|______________________|(GPIO26)SCK   |          |
  |          |              |                      |              |          |
  |          |  MOSI(GPIO27)|______________________|(GPIO27)MOSI  |          |
  |          |              |                      |              |          |
  |          |  MISO(GPIO28)|______________________|(GPIO28)MISO  |          |
  |          |______________|                      |______________|          |
  |                         |                      |                         |
  |                         |                      |                         |
  |                      GND|______________________|GND                      |
  |                         |                      |                         |
  |_MT2523__________________|                      |_MT2523__________________|


@par Hardware and software environment

  - Supported platform
    - LinkIt 2523 HDK

  - HDK switches and pin configuration
    - Connect J1007.2 to J1007.3
    - Connect J1009.2 to J1009.3
    - Connect J1010.2 to J1010.3
    - Connect J1011.2 to J1011.3
    - SPI master module pin mapping table is shown as below.
      | SPI Pin | GPIOx     |    PINx      |
      |-------  |---------  |------------  |
      |  CS_N   | GPIO_25   | CON6301.14   |
      |  SCK    | GPIO_26   | CON6301.12   |
      |  MOSI   | GPIO_27   | CON6301.10   |
      |  MISO   | GPIO_28   | CON6301.8    |

  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - hal_examples/spi_slave_receive_data_two_boards/src/main.c                      Main program.
   - hal_examples/spi_slave_receive_data_two_boards/src/system_mt2523.c             MT2523x's system clock configuration file.
   - hal_examples/spi_slave_receive_data_two_boards/inc/hal_feature_config.h        MT2523x's feature configuration file.
   - hal_examples/spi_slave_receive_data_two_boards/inc/memory_map.h                MT2523x's memory layout symbol file.
   - hal_examples/spi_slave_receive_data_two_boards/GCC/startup_mt2523.s            MT2523x's startup file for GCC.
   - hal_examples/spi_slave_receive_data_two_boards/GCC/syscalls.c                  MT2523x's syscalls for GCC.
   - hal_examples/spi_slave_receive_data_two_boards/MDK-ARM/startup_mt2523.s        MT2523x's start up file for Keil.

  - Project configuration files using GCC
   - hal_examples/spi_slave_receive_data_two_boards/GCC/feature.mk                  Feature configuration file.
   - hal_examples/spi_slave_receive_data_two_boards/GCC/Makefile                    Makefile.
   - hal_examples/spi_slave_receive_data_two_boards/GCC/flash.ld                    Linker script.

  - Project configuration files using Keil
   - hal_examples/spi_slave_receive_data_two_boards/MDK-ARM/spi_slave_receive_data_two_boards.uvprojx             uVision5 Project File. Contains the project structure in XML format.
   - hal_examples/spi_slave_receive_data_two_boards/MDK-ARM/spi_slave_receive_data_two_boards.uvoptx              uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - hal_examples/spi_slave_receive_data_two_boards/MDK-ARM/flash.sct                                             Linker script.

  - Project configuration files using IAR
   - hal_examples/spi_slave_receive_data_two_boards/EWARM/spi_slave_receive_data_two_boards.ewd     IAR project options. Contains the settings for the debugger.
   - hal_examples/spi_slave_receive_data_two_boards/EWARM/spi_slave_receive_data_two_boards.ewp     IAR project file. Contains the project structure in XML format
   - hal_examples/spi_slave_receive_data_two_boards/EWARM/spi_slave_receive_data_two_boards.eww     IAR workspace file. Contains project information.
   - hal_examples/spi_slave_receive_data_two_boards/EWARM/flash.icf                                 Linker script.

@par Run the example
  - Connect board to PC with serial port cable.
  - Build the example project for Master with the command, "./build.sh mt2523_hdk spi_master_send_data_two_boards bl" under the SDK root folder and download the bin file to the LinkIt 2523 HDK that works as SPI Master.
  - Build the example project for Slave with the command, "./build.sh mt2523_hdk spi_slave_receive_data_two_boards bl" under the SDK root folder and download the bin file to the LinkIt 2523 HDK that works as SPI Slave.
  - Connect the related pins of the two 2523 HDKs that shown in the "Features of the example project" part.
  - Run the example. The log will show the communication result with SPI master. Please make sure the two LinkIt 2523 HDK(Master & Slave) are powered up at the same time, within 2 seconds of each other.

* @}
*/
