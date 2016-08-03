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
  * @page hal_uart UART example project
  * @{

@par Overview

 - Example project description
   - This example project shows the useage of APIs of UART module and it runs without FreeRTOS.
 - Features of the example project
   - This example project demonstrate how to use UART port to input and output data with UART DMA mode.
 - Input to the example
   - User inputs the test data into UART port 0.
 - Output from the example
   - UART port 0 then outputs the received test data from the user.

@par Hardware and software environment

  - Supported platform
    - LinkIt 7687 HDK.

  - HDK switches and pin configuration
    - UART module channel mapping is listed below.
     |   CHx    | GPIOx	|  PINx  |
     |----------|-------|--------|
     |  0-RX    |   2	|  CON5  |
     |  0-TX    |   3	|  CON5  |

  - PC/environment configuration
   - A serial tool is required, such as hyper terminal, for UART logging.
   - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity: none and flow control: off.

@par Directory contents
  - Source and header files
   - hal_examples/uart_loopback_data_dma/src/main.c  		        Main program.
   - hal_examples/uart_loopback_data_dma/src/system_mt7687.c		MT76x7's clock configuration file.
   - hal_examples/uart_loopback_data_dma/inc/hal_feature_config.h   MT76x7's feature configuration file.
   - hal_examples/uart_loopback_data_dma/inc/flash_map.h	        MT76x7's memory layout symbol file.
   - hal_examples/uart_loopback_data_dma/GCC/startup_mt7687.s		MT76x7's startup file for GCC.
   - hal_examples/uart_loopback_data_dma/GCC/syscalls.c		        MT76x7's syscalls for GCC.
   - hal_examples/uart_loopback_data_dma/MDK-ARM/startup_mt7687.s	MT76x7's startup file for Keil.
   - hal_examples/uart_loopback_data_dma/EWARM/startup_mt7687.s		MT76x7's startup file for IAR. 

  - Project configuration files using GCC
   - hal_examples/uart_loopback_data_dma/GCC/feature.mk                 Feature configuration.
   - hal_examples/uart_loopback_data_dma/GCC/Makefile                   Makefile.
   - hal_examples/uart_loopback_data_dma/GCC/mt7687_flash.ld            Linker script.

  - Project configuration files using Keil
   - hal_examples/uart_loopback_data_dma/MDK-ARM/uart.uvprojx           uVision5 project file. Contains the project structure in XML format.
   - hal_examples/uart_loopback_data_dma/MDK-ARM/uart.uvoptx            uVision5 project options. Contains the settings for the debugger, trace configuration, breakpoints, currently open files, etc.
   - hal_examples/uart_loopback_data_dma/MDK-ARM/flash.sct              Linker script.

  - Project configuration files using IAR
   - hal_examples/uart_loopback_data_dma/EWARM/uart_loopback_data_dma.ewd     IAR project options. Contains the settings for the debugger.
   - hal_examples/uart_loopback_data_dma/EWARM/uart_loopback_data_dma.ewp     IAR project file. Contains the project structure in XML format
   - hal_examples/uart_loopback_data_dma/EWARM/uart_loopback_data_dma.eww     IAR workspace file. Contains project information.
   - hal_examples/uart_loopback_data_dma/EWARM/flash.icf                      Linker script.

@par Run the example
  - Connect board to PC with serial port cable.
  - Build the example project with the command, "./build.sh mt7687_hdk uart_loopback_data_dma bl" from the SDK root folder and download the bin file to LinkIt 7687 development board.
  - Open a serial tool on the PC.
  - Input test data into UART port 0 according to the corresponding serial tool.
  - Test data is then received from UART port 0 and displayed on the corresponding serial tool.

* @}
*/
